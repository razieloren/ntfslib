#ifndef _NTFSLIB_NTFS_VOLUME_H
#define _NTFSLIB_NTFS_VOLUME_H

#include <string>

#include "Misc\Defs.h"
#include "Misc\Win32\Win32.h"
#include "Misc\Win32\VolumeFile.h"
#include "Types\NTFSTypes.h"
#include "Types\ChangeJournalTypes.h"

using std::wstring;

/**
 * Represents an NTFS volume.
 */
class NTFSVolume {
public:
	NTFSVolume(WCHAR volumeLetter);

	/**
	 * Reads <numOfClusters> clusters starting from <startCluster> into <buffer>.
	 * if <isSparse> is true, <buffer> will be filled with 0's.
	 */
	DWORD readClusters(PVOID buffer, ULONGLONG startCluster, DWORD numOfClusters, bool isSparse);

	/**
	 * Reads the MFT record into <buffer>.
	 * <buffer> size must at least be MFT_RECORD_SIZE (typically 1024 bytes).
	 */
	DWORD readMFT(PVOID buffer);

	/**
	 * Queries the Change Journal for latest changes.
	 */
	ChangeJournalRecordList readChangeJournal(DWORD changeReason, bool onlyForward = false);

	/**
	 * Updates the Change Journal State.
	 */
	void updateChangeJournalState();

	/**
	 * Checks whether <letter> is this volume's letter.
	 */
	bool isDriveLetter(WCHAR letter);

	/**
	 * Returns true if Change Journal is available, false otherwise.
	 */
	bool isChangeJournalAvailable() const;

	/**
	 * Returns the volume's prefix.
	 * e.g.: "C:\"
	 */
	const wstring& getVolumePrefix() const;

	/**
	 * Returns the sector size.
	 */
	WORD getSectorSize() const;

	/**
 	 * Returns the cluster size.
 	 */
	WORD getClusterSize() const;

	/**
	 * Returns the MFT record size.
	 */
	WORD getMFTRecordSize() const;

	/**
	 * Returns the index record size.
	 */
	WORD getIndexRecordSize() const;

	/**
	 * Returns the volume's serial number.
  	 */
	ULONGLONG getVolumeSerialNumber() const;

	/**
	 * Returns the volume's media type.
	 */
	VOLUME_TYPE getVolumeType() const;

private:
	FORBID_COPY_AND_ASSIGN(NTFSVolume);

	// Volume's prefix.
	const wstring m_volumePrefix;

	// Volume's file handle.
	VolumeFile m_volumeFile;

	// Volume's properties.
	VolumeProperties m_volumeProperties;

	// Current journal data (updated with: updateChangeJournalState).
	JournalData m_journalData;

	// Last USN read from the Change Journal.
	USN m_lastUSN;

	// Is Change Journal available?
	bool m_journalAvailable;
};

#endif // _NTFSLIB_NTFS_VOLUME_H
