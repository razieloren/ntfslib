#include "MFTRecord.h"

#include "..\NTFSUtils.h"
#include "..\Misc\Defs.h"
#include "..\Misc\Serializer.h"
#include "..\Misc\StringResource.h"
#include "..\Attribute\FileNameAttribute.h"
#include "..\Attribute\StandardInformationAttribute.h"
#include "..\Attribute\IndexAllocationAttribute.h"
#include "..\Attribute\AttributesListAttribute.h"

MFTRecord::MFTRecord(NTFSVolume& ntfsVolume, const PMFT_RECORD mftRecord):
	m_fileRecordHeader((PMFT_RECORD)new BYTE[mftRecord->BytesInUse]),
	m_fileExtendedInfo(nullptr),
	m_recordNumber(mftRecord->RecordNumber),
	m_parentRecordNumber(0),
	m_ntfsVolume(ntfsVolume) {
	memcpy(m_fileRecordHeader.get(), mftRecord, mftRecord->BytesInUse);
}

std::vector<BYTE> MFTRecord::serialize() {
	NTFSLIB_ASSERT(
		m_fileExtendedInfo != nullptr, 
		UnexpectedActionError
	);
	std::wstring fileName = getFriendlyFileName();
	b4 nameLength = (b4)fileName.length();
	DWORD totalLength = nameLength * 2 + sizeof(b1) + sizeof(b4) + sizeof(b8) * 8;
	Serializer serializer(totalLength);

	b1 flags = 0;
	flags |= (isDirectory() ? (b1)MFT_RECORD_SERIALIZATION_ATTRS::IS_DIRECTORY : 0);
	flags |= (isDeleted() ? (b1)MFT_RECORD_SERIALIZATION_ATTRS::IS_DELETED : 0);
	flags |= (m_fileExtendedInfo->isReadOnly() ? (b1)MFT_RECORD_SERIALIZATION_ATTRS::IS_READ_ONLY : 0);
	flags |= (m_fileExtendedInfo->isHidden() ? (b1)MFT_RECORD_SERIALIZATION_ATTRS::IS_HIDDEN : 0);
	flags |= (m_fileExtendedInfo->isSystemFile() ? (b1)MFT_RECORD_SERIALIZATION_ATTRS::IS_SYSTEM_FILE : 0);
	flags |= (m_fileExtendedInfo->isCompressed() ? (b1)MFT_RECORD_SERIALIZATION_ATTRS::IS_COMPRESSED : 0);
	flags |= (m_fileExtendedInfo->isEncrypted() ? (b1)MFT_RECORD_SERIALIZATION_ATTRS::IS_ENCRYPTED : 0);
	flags |= (m_fileExtendedInfo->isArchived() ? (b1)MFT_RECORD_SERIALIZATION_ATTRS::IS_ARCHIVED : 0);
	serializer.serialize(&flags, sizeof(b1));

	b8 tempValue = 0;
	tempValue = getSize();
	serializer.serialize(&tempValue, sizeof(b8));
	tempValue = getTotalSize();
	serializer.serialize(&tempValue, sizeof(b8));
	tempValue = m_fileExtendedInfo->getCreationTime();
	serializer.serialize(&tempValue, sizeof(b8));
	tempValue = m_fileExtendedInfo->getLastDataChangeTime();
	serializer.serialize(&tempValue, sizeof(b8));
	tempValue = m_fileExtendedInfo->getLastMFTChangeTime();
	serializer.serialize(&tempValue, sizeof(b8));
	tempValue = m_fileExtendedInfo->getLastAccessTime();
	serializer.serialize(&tempValue, sizeof(b8));
	serializer.serialize(&((b8)m_recordNumber), sizeof(b8));
	serializer.serialize(&((b8)m_parentRecordNumber), sizeof(b8));
	serializer.serialize(&nameLength, sizeof(b4));
	serializer.serialize((PVOID)fileName.c_str(), nameLength * 2);
	
	return serializer.getSerializedData();
}

void MFTRecord::loadMetadata() {
	vector<shared_ptr<StandardInformationAttribute>> exInfo = findAttribute<StandardInformationAttribute>(ATTR_TYPE::AT_STANDARD_INFORMATION, false);
	if (!exInfo.empty()) {
		m_fileExtendedInfo = exInfo[0];
	}
	vector<shared_ptr<FileNameAttribute>> fileProps = findAttribute<FileNameAttribute>(ATTR_TYPE::AT_FILE_NAME, false);
	if (!fileProps.empty()) {
		m_parentRecordNumber = fileProps[0]->getParentMFTReference();
		for (const shared_ptr<FileNameAttribute> fileProp : fileProps) {
			m_fileNames.push_back(fileProp->getFileName());
		}
	}
}

void MFTRecord::read(PVOID buffer, const wstring& streamName /*= L""*/, ULONGLONG offset /*= 0*/, DWORD length /*= 0*/) {
	const shared_ptr<DataStreamAttribute> dataStream = getDataStream(streamName);

	if (dataStream != nullptr) {
		DWORD bytesToRead = (length == 0 ? (DWORD)dataStream->getSize() : length);
		dataStream->getData(buffer, offset, bytesToRead);
	}
	else {
		NTFSLIB_ERROR(AttributeNotFoundError, NTFSLIB_DEFAULT_ERROR_CODE, "Could not read from stream [%ws] since it does not exist", streamName.c_str());
	}
}

const shared_ptr<DataStreamAttribute> MFTRecord::getDataStream(const wstring& streamName /*= L""*/) const {
	vector<shared_ptr<DataStreamAttribute>> streams = findAttribute<DataStreamAttribute>(ATTR_TYPE::AT_DATA, false);
	for (const shared_ptr<DataStreamAttribute> dataStream : streams) {
		if (streamName == dataStream->getStreamName()) {
			return dataStream;
		}
	}

	return nullptr;
}

vector<wstring> MFTRecord::listStreams() const {
	vector<shared_ptr<DataStreamAttribute>> streams = findAttribute<DataStreamAttribute>(ATTR_TYPE::AT_DATA, false);
	vector<wstring> streamNames;

	for (const std::shared_ptr<DataStreamAttribute> stream : streams) {
		wstring streamName = stream->getStreamName();
		if (!streamName.empty()) {
			streamNames.push_back(streamName);
		}
	}
	for (shared_ptr<MFTRecord> fileReference : m_additionalRecords) {
		vector<wstring> tempStreams = fileReference->listStreams();
		streamNames.insert(streamNames.end(), tempStreams.begin(), tempStreams.end());
	}
	return streamNames;
}

const vector<wstring>& MFTRecord::getFileNames() const {
	return m_fileNames;
}

const wstring& MFTRecord::getFriendlyFileName() const {
	if (m_fileNames.empty()) {
		NTFSLIB_ERROR(
			UnexpectedActionError, 
			NTFSLIB_DEFAULT_ERROR_CODE, 
			"No file name for this record"
		);
	}

	for (const wstring& name : m_fileNames) {
		// If the name is longest than 8 characters long, it must be "user-friendly" (NOT DOS name).
		if (name.length() > DOS_NAME_LENGTH) {
			return name;
		}
	}
	// If none of the names are longer than 8 characters, it means the DOS name and the "user-friendly" name are the same.
	return m_fileNames[0];
}

ULONGLONG MFTRecord::getRecordNumber() const {
	return m_fileRecordHeader->RecordNumber;
}

ULONGLONG MFTRecord::getParentRecordNumber() const {
	return m_parentRecordNumber;
}

ULONGLONG MFTRecord::getSize(const wstring& streamName /* = L"" */) {
	const shared_ptr<DataStreamAttribute> mainStream = getDataStream(streamName);
	if (mainStream != nullptr) {
		return mainStream->getSize();
	}
	return 0;
}

ULONGLONG MFTRecord::getTotalSize() {
	// Notice: Directories can own named streams.
	// e.g. 
	//	mkdir my_directory
	//	echo batman > my_directory:awesome_stream
	//  more < my_directory:awesome_stream => "batman"
	ULONGLONG totalSize = 0;
	vector<shared_ptr<DataStreamAttribute>> streams = findAttribute<DataStreamAttribute>(ATTR_TYPE::AT_DATA, false);
	for (const shared_ptr<DataStreamAttribute> stream : streams) {
		totalSize += stream->getSize();
	}
	for (shared_ptr<MFTRecord> fileReference : m_additionalRecords) {
		totalSize += fileReference->getTotalSize();
	}

	return totalSize;
}

const shared_ptr<StandardInformationAttribute> MFTRecord::getExtendedInfo() const {
	return m_fileExtendedInfo;
}

bool MFTRecord::isDirectory() const {
	return (m_fileRecordHeader->Flags & (b2)MFT_RECORD_FLAGS::MFT_RECORD_IS_DIRECTORY) != 0;
}

bool MFTRecord::isDeleted() const {
	return (m_fileRecordHeader->Flags & (b2)MFT_RECORD_FLAGS::MFT_RECORD_IN_USE) == 0;
}

void MFTRecord::addAdditionalFileRecord(std::shared_ptr<MFTRecord> fileRecord) {
	m_additionalRecords.push_back(fileRecord);
}
