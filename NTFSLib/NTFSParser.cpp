#include <cctype>
#include <set>

#include "NTFSUtils.h"
#include "NTFSParser.h"
#include "Misc\StringResource.h"
#include "Attribute\IndexRootAttribute.h"
#include "Attribute\VolumeNameAttribute.h"
#include "Attribute\IndexAllocationAttribute.h"
#include "Attribute\VolumeInformationAttribute.h"
#include "Attribute\AttributesListAttribute.h"

using std::set;
using std::make_shared;

NTFSParser::NTFSParser(WCHAR volumeLetter):
	m_volume(volumeLetter),
	m_stopFullDirEvent(StringResource::stopFullDirEventName),
	m_stopFileDumpEvent(StringResource::stopFileDumpEventName) {
	Buffer mftBuffer(m_volume.getMFTRecordSize());
	NTFSLIB_ASSERT(
		m_volume.readMFT(mftBuffer.data()) == mftBuffer.size(),
		BadSizeError
	);
	m_MFTRecord = finalizeMFTRecord(mftBuffer);

	shared_ptr<MFTRecord> volumeFile = readMFTRecord((ULONGLONG)NTFS_SYSTEM_FILES::FILE_Volume);
	shared_ptr<VolumeInformationAttribute> volumeInfo = volumeFile->findAttribute<VolumeInformationAttribute>(ATTR_TYPE::AT_VOLUME_INFORMATION)[0];
	NTFSLIB_ASSERT(
		volumeInfo->isVolumeSupported(),
		UnsupportedNTFSVersionError
	);
	shared_ptr<VolumeNameAttribute> volumeName = volumeFile->findAttribute<VolumeNameAttribute>(ATTR_TYPE::AT_VOLUME_NAME)[0];
	m_volumeAttributes = {
		volumeInfo->getMajorVersion(),
		volumeInfo->getMinorVersion(),
		m_volume.getVolumeSerialNumber(),
		volumeName->getVolumeName()
	};
	TRACE(DEBUG_LEVEL::INFO, "Running on NTFS %u.%u volume '%ws' ('%wc'), Serial: %llX", m_volumeAttributes.MajorVersion, m_volumeAttributes.MinorVersion, m_volumeAttributes.Name.c_str(), volumeLetter, m_volumeAttributes.SerialNumber);

	// "Forwarding" Change Journal cursor to this exact moment.
	m_volume.readChangeJournal(0xffffffff, true);
}

shared_ptr<MFTRecord> NTFSParser::findMFTRecord(const wstring& path) {
	NTFSLIB_ASSERT(
		!path.empty(),
		BadPathError
		);
	vector<wstring> parts = NTFSUtils::splitWidePath(path);
	NTFSLIB_ASSERT(
		!parts.empty(),
		BadPathError
		);
	wstring firstElement = parts[0];
	NTFSLIB_ASSERT(
		!firstElement.empty(),
		BadPathError
		);
	// First element might be an environment variable.
	if (firstElement[0] == L'%' && firstElement[firstElement.length() - 1] == L'%') {
		firstElement = NTFSUtils::expandEnvironmentVariable(firstElement);
		firstElement += (firstElement[firstElement.length() - 1] == L'\\') ? L"" : StringResource::windowsPathSeperator;
		for (size_t i = 1; i < parts.size(); ++i) {
			firstElement += parts[i] + StringResource::windowsPathSeperator;
		}
		// No need to remove trailing '\'.
		return findMFTRecord(firstElement);
	}
	else {
		// If it's not an environment variable, it must be the volume's letter.
		NTFSLIB_ASSERT(
			m_volume.isDriveLetter(firstElement[0]),
			BadPathError
			);
	}

	std::shared_ptr<MFTRecord> currentFile = readMFTRecord((ULONGLONG)NTFS_SYSTEM_FILES::FILE_Root);
	// Means the user asked for  the volume itself, e.g. "C:\".
	if (parts.size() == 1) {
		return currentFile;
	}

	for (size_t i = 1; i < parts.size(); ++i) {
		currentFile = findMFTRecordInFolder(currentFile, parts[i]);
		// If it's not the last file, we should refer a directory.
		if (i < parts.size() - 1) {
			NTFSLIB_ASSERT(
				currentFile->isDirectory(),
				BadPathError
				);
		}
	}

	TRACE(DEBUG_LEVEL::VERBOSE, "Found file record: %ws", path.c_str());
	return currentFile;
}

DiffList NTFSParser::listDiffs(DWORD reason /* = 0xffffffff*/) {
	DiffList diffs;
	map<ULONGLONG, DiffLocalCache> localCache;
	ChangeJournalRecordList changeList = m_volume.readChangeJournal(reason);
	for (const ChangeJournalRecord& record : changeList) {
		try {
			shared_ptr<MFTRecord> relativeFileRecord = readMFTRecord(record.ReferenceNumber);
			diffs.push_back({
				record.ChangeReason,
				(ULONGLONG)record.TimeStamp.QuadPart,
				resolveFullFilePath(relativeFileRecord, localCache)
			});
		}
		catch (NTFSLibError&) {
			// Something went wrong with this specific record, we should just move on.
			TRACE(DEBUG_LEVEL::VERBOSE, "Error while adding record %#llx to the DiffList", record.ReferenceNumber);
		}
	}
	return diffs;
}

Dir NTFSParser::listFiles(const wstring path /*= L"C:"*/, bool recursive /*= false*/, int maxDepth /* = 1*/) {
	TRACE(DEBUG_LEVEL::VERBOSE, "Listing files for: %ws%s", path.c_str(), (recursive ? " (recursively)" : ""));
	shared_ptr<MFTRecord> ourFile = findMFTRecord(path);
	return listDirectoryFiles(ourFile, recursive, maxDepth);
}

void NTFSParser::dumpFullDir(NTFSOutStream& outStream, WORD maxFileRecordsPerFlush) {
	TRACE(DEBUG_LEVEL::VERBOSE, "Dumping full dir, flushing every %u records at most", maxFileRecordsPerFlush);
	DWORD maxBufferSize = (_MAX_PATH * 2 + sizeof(b1) + sizeof(b4) + sizeof(b8) * 8) * maxFileRecordsPerFlush;
	DWORD totalBytesRead = 0;
	WORD recordsRead = 0;
	ULONGLONG totalNumberOfRecords = m_MFTRecord->getSize() / m_volume.getMFTRecordSize();
	
	bool stopRequested = false;
	Buffer data;
	data.reserve(maxBufferSize);
	shared_ptr<MFTRecord> fileRecord = nullptr;
	for (ULONGLONG i = 0; i < totalNumberOfRecords; ++i) {
		if (m_stopFullDirEvent.isSignaled()) {
			TRACE(DEBUG_LEVEL::CRITICAL, "Stop full dir event signaled, stopping");
			stopRequested = true;
			break;
		}
		try {
			fileRecord = readMFTRecord(i);
			Buffer serizlizedData = fileRecord->serialize();
			data.insert(data.end(), serizlizedData.begin(), serizlizedData.end());
			recordsRead++;
			totalBytesRead += (DWORD)serizlizedData.size();

			if (recordsRead == maxFileRecordsPerFlush) {
				TRACE(DEBUG_LEVEL::VERBOSE, "Flushing %u records", recordsRead);
				outStream.write(data.data(), totalBytesRead);
				data.clear();
				totalBytesRead = 0;
				recordsRead = 0;
			}
		}
		catch (NTFSLibError&) {
			// Something went wring with this record, we should just skip it.
			TRACE(DEBUG_LEVEL::VERBOSE, "Error while dumping record %#llx", i);
		}
	}

	// Means we got some left overs (Max: maxFileRecordsPerFlush - 1 records).
	if (!stopRequested && recordsRead > 0) {
		TRACE(DEBUG_LEVEL::VERBOSE, "Flushing %u records", recordsRead);
		outStream.write(data.data(), totalBytesRead);
		// No need to reset flags again.
	}
}

void NTFSParser::dumpFile(NTFSOutStream& outStream, DWORD maxBlockSizePerFlush, const wstring& filePath, const wstring& streamName /*= L""*/, ULONGLONG offset /*= 0*/, DWORD amount /*= 0*/) {
	TRACE(DEBUG_LEVEL::VERBOSE, "Dumping file: %ws:[%ws], flushing every %lu bytes at most", filePath.c_str(),
		streamName.length() > 0 ? streamName.c_str() : L"Main Data Stream", maxBlockSizePerFlush);
	NTFSLIB_ASSERT(
		maxBlockSizePerFlush > 0,
		BadSizeError
	);

	shared_ptr<MFTRecord> ourFile = findMFTRecord(filePath);

	bool stopRequested = false;
	Buffer attrData;
	attrData.reserve(maxBlockSizePerFlush);
	DWORD totalSize = amount == 0 ? (DWORD)ourFile->getSize(streamName) : amount;
	DWORD numOfBlocks = totalSize / maxBlockSizePerFlush;
	for (DWORD i = 0; i < numOfBlocks; ++i) {
		if (m_stopFileDumpEvent.isSignaled()) {
			TRACE(DEBUG_LEVEL::CRITICAL, "Stop dump file event signaled, stopping");
			stopRequested = true;
			break;
		}
		ourFile->read(attrData.data(), streamName, offset + (i * maxBlockSizePerFlush), maxBlockSizePerFlush);
		outStream.write(attrData.data(), maxBlockSizePerFlush);
	}

	DWORD bytesLeft = totalSize % maxBlockSizePerFlush;
	if (!stopRequested && bytesLeft > 0) {
		ourFile->read(attrData.data(), streamName, offset + (numOfBlocks * maxBlockSizePerFlush), bytesLeft);
		outStream.write(attrData.data(), bytesLeft);
	}
}

void NTFSParser::stopFullDir() {
	m_stopFullDirEvent.signal();
}

void NTFSParser::stopFileDump() {
	m_stopFileDumpEvent.signal();
}

const VolumeAttributes& NTFSParser::getVolumeAttributes() const {
	return m_volumeAttributes;
}

shared_ptr<MFTRecord> NTFSParser::findMFTRecordInFolder(shared_ptr<MFTRecord> folder, const wstring& fileName) {
	shared_ptr<IndexRootAttribute> indexRoot = folder->findAttribute<IndexRootAttribute>(ATTR_TYPE::AT_INDEX_ROOT)[0];
	Index entries = indexRoot->getIndexEntries();
	for (const IndexEntry& entry : entries) {
		if (entry.compareFileName(fileName)) {
			return readMFTRecord(entry.getMFTReference());
		}
		if (entry.isSubNode()) {
			shared_ptr<MFTRecord> ourFile = findMFTRecordInSubNode(folder, fileName, entry.getSubNodeVCN());
			if (ourFile != nullptr) {
				return ourFile;
			}
		}
	}
	NTFSLIB_ERROR(MFTRecordNotFoundError, 0, "Could not find file record: %ws under folder: %#llx", fileName.c_str(), folder->getRecordNumber());
}

shared_ptr<MFTRecord> NTFSParser::findMFTRecordInSubNode(shared_ptr<MFTRecord> rootRecord, const wstring& fileName, ULONGLONG subNodeVCN) {
	shared_ptr<MFTRecord> ourFile = nullptr;
	shared_ptr<IndexAllocationAttribute> indexAlloc = rootRecord->findAttribute<IndexAllocationAttribute>(ATTR_TYPE::AT_INDEX_ALLOCATION)[0];
	shared_ptr<IndexRecord> indexRecord = indexAlloc->readIndexRecord(subNodeVCN);
	Index entries = indexRecord->getIndexEntries();
	for (const IndexEntry& entry : entries) {
		if (entry.compareFileName(fileName)) {
			ourFile = readMFTRecord(entry.getMFTReference());
		}
		else if (entry.isSubNode()) {
			ourFile = findMFTRecordInSubNode(rootRecord, fileName, entry.getSubNodeVCN());
		}

		if (ourFile != nullptr) {
			return ourFile;
		}
	}
	return nullptr;
}

shared_ptr<MFTRecord> NTFSParser::readMFTRecord(ULONGLONG recordIndex) {
	WORD mftRecordSize = m_volume.getMFTRecordSize();

	ULONGLONG fileRecordAddr = (ULONGLONG)mftRecordSize * recordIndex;
	Buffer recordBuffer(mftRecordSize);
	m_MFTRecord->read(recordBuffer.data(), L"", fileRecordAddr, mftRecordSize);
	return finalizeMFTRecord(recordBuffer);
}

shared_ptr<MFTRecord> NTFSParser::finalizeMFTRecord(Buffer& mftRecordBuffer) {
	PMFT_RECORD recordData = (PMFT_RECORD)mftRecordBuffer.data();
	NTFSLIB_ASSERT(
		CMP_STR((PCHAR)&recordData->RecordHeader.Magic, StringResource::fileRecordSignature),
		BadRecordHeaderError
	);
	NTFSUtils::USARecordFixup(&recordData->RecordHeader, m_volume.getSectorSize());

	shared_ptr<MFTRecord> record = make_shared<MFTRecord>(m_volume, recordData);
	vector<shared_ptr<AttributesListAttribute>> attributeLists = record->findAttribute<AttributesListAttribute>(ATTR_TYPE::AT_ATTRIBUTE_LIST, false);
	for (const shared_ptr<AttributesListAttribute> attributeList : attributeLists) {
		AdditionalRecordRefs additionalRecordRefs = attributeList->getAdditionalMFTReferences();
		for (const ULONGLONG& additionalRecordRef : additionalRecordRefs) {
			if (additionalRecordRef != record->getRecordNumber()) {
				record->addAdditionalFileRecord(readMFTRecord(additionalRecordRef));
			}
		}
	}
	record->loadMetadata();
	return record;
}

wstring NTFSParser::resolveFullFilePath(shared_ptr<MFTRecord> leaf, map<ULONGLONG, DiffLocalCache>& localCache) {
	wstring path = leaf->getFriendlyFileName();
	shared_ptr<MFTRecord> parent = nullptr;
	ULONGLONG currentRecordNumber = leaf->getParentRecordNumber();
	DiffLocalCache cached;

	try {
		cached = localCache.at(currentRecordNumber);
	}
	catch (std::out_of_range&) {
		parent = readMFTRecord(currentRecordNumber);
		cached = { parent->getParentRecordNumber(), parent->getFriendlyFileName() };
		localCache[currentRecordNumber] = cached;
	}

	WORD depth = 0;
	while (currentRecordNumber != (ULONGLONG)NTFS_SYSTEM_FILES::FILE_Root) {
		path = cached.Name + StringResource::windowsPathSeperator + path;
		try {
			cached = localCache.at(cached.ParentReferenceNumber);
			currentRecordNumber = cached.ParentReferenceNumber;
		}
		catch (std::out_of_range&) {
			parent = readMFTRecord(cached.ParentReferenceNumber);
			currentRecordNumber = parent->getParentRecordNumber();
			cached = { currentRecordNumber, parent->getFriendlyFileName() };
			localCache[parent->getRecordNumber()] = cached;
		}

		// Making sure we are not in an infinite loop.
		depth++;
		NTFSLIB_ASSERT(
			depth < 1024,
			BadPathError
		);
	}

	// Adding RootFile (Volume letter) to the path.
	return m_volume.getVolumePrefix() + cached.Name + StringResource::windowsPathSeperator + path;
}

Dir NTFSParser::listDirectoryFiles(shared_ptr<MFTRecord> root, bool recursive, int maxDepth) {
	NTFSLIB_ASSERT(
		root->isDirectory(),
		UnexpectedActionError
	);
	Dir dir;
	shared_ptr<IndexRootAttribute> indexRoot = root->findAttribute<IndexRootAttribute>(ATTR_TYPE::AT_INDEX_ROOT)[0];

	Index entries = indexRoot->getIndexEntries();
	vector<ULONGLONG> listedRefs;
	for (const IndexEntry& entry : entries) {
		if (entry.isSubNode()) {
			listSubNodeRecords(dir, root, entry.getSubNodeVCN(), recursive, maxDepth);
		}

		ULONGLONG recordRef = entry.getMFTReference();
		if (entry.isUserEntry() && NTFSUtils::hasElement<ULONGLONG>(listedRefs, recordRef)) {
			shared_ptr<MFTRecord> fileRecord = readMFTRecord(recordRef);
			shared_ptr<DirProduct> dirProduct = make_shared<DirProduct>(fileRecord);
			if (recursive && maxDepth > 0 && fileRecord->isDirectory()) {
				dirProduct->Children = listDirectoryFiles(fileRecord, recursive, maxDepth - 1);
			}
			dir.push_back(dirProduct);
			listedRefs.push_back(recordRef);
		}
	}

	return dir;
}

void NTFSParser::listSubNodeRecords(Dir& subNodeRecords, std::shared_ptr<MFTRecord> folder, ULONGLONG subNodeVCN, bool recursive, int maxDepth) {
	shared_ptr<IndexAllocationAttribute> indexAlloc = folder->findAttribute<IndexAllocationAttribute>(ATTR_TYPE::AT_INDEX_ALLOCATION)[0];
	shared_ptr<IndexRecord> indexRecord = indexAlloc->readIndexRecord(subNodeVCN);

	Index entries = indexRecord->getIndexEntries();
	vector<ULONGLONG> listedRefs;
	for (const IndexEntry& entry : entries) {
		if (entry.isSubNode()) {
			listSubNodeRecords(subNodeRecords, folder, entry.getSubNodeVCN(), recursive, maxDepth);
		}

		ULONGLONG recordRef = entry.getMFTReference();
		if (entry.isUserEntry() && NTFSUtils::hasElement<ULONGLONG>(listedRefs, recordRef)) {
			shared_ptr<MFTRecord> fileRecord = readMFTRecord(recordRef);
			shared_ptr<DirProduct> dirProduct = make_shared<DirProduct>(fileRecord);
			if (recursive && maxDepth > 0 && fileRecord->isDirectory()) {
				dirProduct->Children = listDirectoryFiles(fileRecord, recursive, maxDepth - 1);
			}
			subNodeRecords.push_back(dirProduct);
			listedRefs.push_back(recordRef);
		}
	}
}