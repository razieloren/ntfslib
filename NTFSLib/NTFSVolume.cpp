#include <cctype>

#include "NTFSVolume.h"
#include "NTFSUtils.h"
#include "Misc\NTFSLibError.h"
#include "Misc\StringResource.h"

using std::tolower;

NTFSVolume::NTFSVolume(WCHAR volumeLetter) :
	m_volumePrefix(wstring(1, volumeLetter) + StringResource::volumePrefix),
	m_volumeFile(volumeLetter), m_lastUSN(0), m_journalAvailable(false) {
	// Reading the Boot Sector.
	NTFS_BOOT_SECTOR bootSector;
	NTFSLIB_ASSERT(
		m_volumeFile.read(&bootSector, sizeof(NTFS_BOOT_SECTOR)) == sizeof(NTFS_BOOT_SECTOR),
		BadSizeError
	);
	NTFSLIB_ASSERT(
		CMP_STR((PCHAR)&bootSector.OEMID, StringResource::ntfsSignature),
		BadRecordHeaderError
	);

	// Parsing the BPB.
	WORD bytesPerCluster = bootSector.BPB.SectorsPerCluster * bootSector.BPB.BytesPerSector;

	int tempRecordSize;
	tempRecordSize = (char)bootSector.BPB.ClustersPerMFTRecord;
	WORD MFTRecordSize = (WORD)(tempRecordSize < 0 ? 1 << (-tempRecordSize) : bytesPerCluster * tempRecordSize);
	tempRecordSize = (char)bootSector.BPB.ClustersPerIndexRecord;
	WORD indexRecordSize = (WORD)(tempRecordSize < 0 ? 1 << (-tempRecordSize) : bytesPerCluster * tempRecordSize);

	m_volumeProperties = {
		bootSector.BPB.MediaDescriptor,
		bootSector.BPB.SectorsPerCluster,
		bootSector.BPB.ClustersPerMFTRecord,
		bootSector.BPB.ClustersPerIndexRecord,
		bootSector.BPB.BytesPerSector,
		bytesPerCluster,
		MFTRecordSize,
		indexRecordSize,
		bootSector.BPB.VolumeSerialNumber,
		bootSector.BPB.TotalSectors / bootSector.BPB.SectorsPerCluster,
		bootSector.BPB.TotalSectors,
		bootSector.BPB.MFTLCN * bytesPerCluster,
		bootSector.BPB.MFTLCN,
		bootSector.BPB.MFTMirrLCN,
	};
	
	// Updating Change Journal current state.
	updateChangeJournalState();
}

DWORD NTFSVolume::readClusters(PVOID buffer, ULONGLONG startCluster, DWORD numOfClusters, bool isSparse) {
	DWORD bytesToRead = numOfClusters * m_volumeProperties.ClusterSize;
	if (isSparse) {
		SecureZeroMemory(buffer, bytesToRead);
		return bytesToRead;
	}
	// Seeking to the desired address.
	m_volumeFile.seek(startCluster * m_volumeProperties.ClusterSize);
	// Reading number of specified clusters from the volume.
	return m_volumeFile.read(buffer, bytesToRead);
}

DWORD NTFSVolume::readMFT(PVOID buffer) {
	m_volumeFile.seek(m_volumeProperties.MFTAddr);
	return m_volumeFile.read(buffer, m_volumeProperties.MFTRecordSize);
}

ChangeJournalRecordList NTFSVolume::readChangeJournal(DWORD changeReason, bool onlyForward /* = false*/) {
	// This won't work if Change Journal is not available.
	NTFSLIB_ASSERT(
		m_journalAvailable,
		UnexpectedActionError
	);
	TRACE(DEBUG_LEVEL::VERBOSE, "%s Change Journal starting from %#016llx, searching for reasons: %#08lx", 
		onlyForward ? "Forwarding" : "Reading", m_lastUSN, changeReason);
	ChangeJournalRecordList changes;
	// Defining how we'd like to read the Change Journal.
	JournalReadDef journalReadDef { 
		m_lastUSN,					// Start USN.
		changeReason,				// Reason mask.
		FALSE,						// Return only on close.
		0,							// Timeout.
		0,							// Bytes to wait for.
		m_journalData.UsnJournalID	// Journal ID.
	};

	BYTE usnDataBuffer[JOURNAL_READ_LENGTH] = { 0 };
	PUSN_RECORD usnRecord = nullptr;
	DWORD bytesRead, actualRecordBytes;
	bool pumpedData = false, pumpCompleted = false;
	/**
	 * One read operation from the Change Journal returns:
	 * [USN(x + 1) {sizeof(USN) bytes == 8 bytes}][USN_RECORD(0)][USN_RECORD(1)][USN_RECORD(...)][USN_RECORD(x)]
	 * The first USN value is used for subsequent read calls.
	 */
	while (!pumpCompleted) {
		bytesRead = m_volumeFile.sendIoctl(FSCTL_READ_USN_JOURNAL, &journalReadDef, sizeof(journalReadDef), usnDataBuffer, JOURNAL_READ_LENGTH);
		// We're not interested in the first sizeof(USN) bytes currently (see explanation above).
		actualRecordBytes = bytesRead - sizeof(USN);
		usnRecord = (PUSN_RECORD)(((PBYTE)usnDataBuffer) + sizeof(USN));
		// Update starting USN for next call (the first 8 bytes of the buffer indicate the next USN).
		journalReadDef.StartUsn = *(USN*)&usnDataBuffer;

		while (actualRecordBytes > 0) {
			pumpedData = false;
			if (!onlyForward) {
				changes.push_back({
					usnRecord->TimeStamp,
					usnRecord->FileReferenceNumber,
					usnRecord->RecordLength,
					usnRecord->Reason,
					usnRecord->Usn,
				});
			}

			actualRecordBytes -= usnRecord->RecordLength;

			// Moving on to the next record.
			usnRecord = (PUSN_RECORD)(((PBYTE)usnRecord) + usnRecord->RecordLength);
		}
		// That indicates we've iterated twice without finding new record data, time to go home.
		pumpCompleted = (pumpedData == true);
		// Anyhow, we've just pumped some data...
		pumpedData = true;
	}
	if (!onlyForward) {
		TRACE(DEBUG_LEVEL::VERBOSE, "Finished reading Change Journal, found: %zu entries", changes.size());
	}

	// Updating the last USN for further reads.
	m_lastUSN = *(USN*)&usnDataBuffer;

	return changes;
}

bool NTFSVolume::isChangeJournalAvailable() const {
	return m_journalAvailable;
}

const wstring& NTFSVolume::getVolumePrefix() const {
	return m_volumePrefix;
}

WORD NTFSVolume::getSectorSize() const {
	return m_volumeProperties.SectorSize;
}

WORD NTFSVolume::getClusterSize() const {
	return m_volumeProperties.ClusterSize;
}

WORD NTFSVolume::getMFTRecordSize() const {
	return m_volumeProperties.MFTRecordSize;
}

WORD NTFSVolume::getIndexRecordSize() const {
	return m_volumeProperties.IndexRecordSize;
}

ULONGLONG NTFSVolume::getVolumeSerialNumber() const {
	return m_volumeProperties.SerialNumber;
}

VOLUME_TYPE NTFSVolume::getVolumeType() const {
	switch ((VOLUME_TYPE)m_volumeProperties.MediaDescriptor) {
	case VOLUME_TYPE::FIXED:
		return VOLUME_TYPE::FIXED;
	case VOLUME_TYPE::FLOPPY:
		return VOLUME_TYPE::FLOPPY;
	default:
		return VOLUME_TYPE::OTHER;
	}
}

void NTFSVolume::updateChangeJournalState() {
	TRACE(DEBUG_LEVEL::VERBOSE, "Updating Change Journal State");
	try {
		ZeroMemory(&m_journalData, sizeof(JournalData));
		m_volumeFile.sendIoctl(FSCTL_QUERY_USN_JOURNAL, NULL, 0, &m_journalData, sizeof(m_journalData));
		m_journalAvailable = true;
		TRACE(DEBUG_LEVEL::VERBOSE, "Change Journal is active and ready to use");
	}
	catch (...) {
		// We failed querying the journal, that means Change Journal is not available at the moment.
		m_journalAvailable = false;
		TRACE(DEBUG_LEVEL::VERBOSE, "Change Journal is not active at the moment");
	}
}

bool NTFSVolume::isDriveLetter(WCHAR letter) {
	return tolower(letter) == tolower(m_volumeFile.getVolumeLetter());
}
