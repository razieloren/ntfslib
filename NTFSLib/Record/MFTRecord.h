#ifndef _NTFSLIB_FILE_RECORD_H
#define _NTFSLIB_FILE_RECORD_H

#include <memory>
#include <vector>
#include <string>

#include "..\Misc\NTFSLibError.h"
#include "..\Types\NTFSTypes.h"
#include "..\NTFSVolume.h"
#include "..\NTFSOutStream.h"
#include "..\Attribute\Base\AttributeRecord.h"
#include "..\Attribute\StandardInformationAttribute.h"
#include "..\Attribute\DataStreamAttribute.h"
#include "IndexRecord.h"

using std::shared_ptr;
using std::vector;
using std::wstring;

// Supported flags in record serialization process.
enum class MFT_RECORD_SERIALIZATION_ATTRS {
	IS_DIRECTORY = 1,
	IS_DELETED = 2,
	IS_READ_ONLY = 4,
	IS_HIDDEN = 8,
	IS_SYSTEM_FILE = 16,
	IS_COMPRESSED = 32,
	IS_ENCRYPTED = 64,
	IS_ARCHIVED = 128
};

#define DOS_NAME_LENGTH 8

/**
 * NTFS MFT Record. Every MFT record is limited in its size to one 
 * specified in the boot sector. Each MFT record contains attributes, which
 * define the record and contain its data. Relate to "MFTRecord" as "GeneralRecord"
 * because everything in NTFS is a MFT Record - Files and Directories.
 */
class MFTRecord {
public:
	MFTRecord(NTFSVolume& ntfsVolume, const PMFT_RECORD mftRecord);

	/**
	* Returns the standard information related to this record.
	*/
	const shared_ptr<StandardInformationAttribute> getExtendedInfo() const;

	/**
	 * Returns the specified data stream in the record, or nullptr if not found.
	 */
	const shared_ptr<DataStreamAttribute> MFTRecord::getDataStream(const wstring& streamName = L"") const;

	/**
	* Returns all the file names (AKA hard links) of this file record.
	* Useful when you are interested in the file record's DOS name (e.g. PROGRA~1), other
	* then the actual name. Returns an empty vector if no file names are found.
	*/
	const vector<wstring>& getFileNames() const;

	/**
	* Returns the file name as shown to the user.
	* Throws an UnexpectedActionError if no file names available at all.
	*/
	const wstring& getFriendlyFileName() const;

	/**
	* Lists all the alternate data stream names the MFT record owns. NOT including the main data stream.
	* NOTICE: Directories can also own alternate data streams.
	*/
	vector<wstring> listStreams() const;

	/**
	 * Serializes all the file record's meta data into a vector of bytes.
	 * Output:
	 * [1 byte] Flags
	 * [8 byte] Unnamed data stream size
	 * [8 byte] All streams total size
	 * [8 byte] Creation time
	 * [8 byte] Last data change time
	 * [8 byte] Last record change time
	 * [8 byte] Last access time
	 * [8 byte] Record number
	 * [8 byte] Parent record number
	 * [4 byte] Name length
	 * { X = Name Length * 2 (because it's unicode / multibyte) }
	 * [X byte] Unicode name
	 *
	 * Maximum size (assuming max file name of _MAX_PATH, 260):
	 * 1 + 8 * 8 + 4 + (260 * 2) = 589 bytes.
	 * Average size (assuming file name length of 15):
	 * 1 + 8 * 8 + 4 + (15 * 2) = 99 bytes.
	 * Average sum for 1,000,000 records (Very active computer):
	 * 120 * 1,000,000 bytes = 94.414 MB.
	 */
	Buffer serialize();

	/**
	 * Returns this record's record number.
	 * Throws an NTFSLibError if extended information is not available for this record.
	 */
	ULONGLONG getRecordNumber() const;

	/**
	 * Returns the record number of the parent (e.g. The parent directory).
	 */
	ULONGLONG getParentRecordNumber() const;

	/**
	 * Return the size of a specific data stream.
	 */
	ULONGLONG getSize(const wstring& streamName = L"");

	/**
	 * Return the sum of all data stream sizes.
	 */
	ULONGLONG getTotalSize();

	/**
	 * Is this file record referencing a directory?
	 */
	bool isDirectory() const;

	/**
	 * Has this file record been marked for deletion?
	 */
	bool isDeleted() const;

	/**
	* Loads the record meta data. You should invoke this function after you read
	* all the "Attributes List" attributes.
	*/
	void loadMetadata();

	/**
	* Reads bytes from a specific data stream in the MFT record.
	* Use <length> = 0 to read the whole data stream.
	* If <offset> + <length> is greater than the stream size, an OutOfBounds error will be thrown.
	* If less than <length> bytes were read, a BadSizeError will be thrown.
	* <buffer> size must be at least <length> bytes long.
	*/
	void read(PVOID buffer, const wstring& streamName = L"", ULONGLONG offset = 0, DWORD length = 0);

	/**
	 * Adds additional file record to be referenced while parsing file record's attributes.
	 */
	void addAdditionalFileRecord(shared_ptr<MFTRecord> fileRecord);

	/**
	 * Finds an all the attribute instances in the record.
	 */
	template <class Attribute>
	vector<shared_ptr<Attribute>> findAttribute(ATTR_TYPE attributeType, bool throwIfNotFound = true) const;

private:
	FORBID_COPY_AND_ASSIGN(MFTRecord);

	// Reference to the record header.
	const shared_ptr<MFT_RECORD> m_fileRecordHeader;

	// Extended information about this record, if available.
	shared_ptr<StandardInformationAttribute> m_fileExtendedInfo;

	// Additional MFT records related to this record (derived from "Attributes List" attribute).
	vector <shared_ptr<MFTRecord>> m_additionalRecords;

	// All record names.
	vector<wstring> m_fileNames;

	// Record's number.
	ULONGLONG m_recordNumber;

	// Parent's record number.
	ULONGLONG m_parentRecordNumber;

	// Reference to the related NTFS volume.
	NTFSVolume& m_ntfsVolume;
};

#include "MFTRecord.inl"

#endif // _NTFSLIB_FILE_RECORD_H