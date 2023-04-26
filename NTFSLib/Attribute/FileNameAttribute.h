#ifndef _NTFSLIB_FILE_NAME_ATTRIBUTE_H
#define _NTFSLIB_FILE_NAME_ATTRIBUTE_H

#include "..\Misc\Defs.h"
#include "Base\AttributeRecord.h"

/**
 * Contains the MFT record general file properties, including the record's name as shown to the user.
 */
class FileNameAttribute : public AttributeRecord {
public:
	FileNameAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute);

	/**
	 * Returns the record's name, as shown to the user.
	 */
	wstring getFileName() const;

	/**
	 * Returns the NFT reference of the parent record (the parent's directory, for instance).
	 */
	ULONGLONG getParentMFTReference() const;

	/**
	 * Compares two file name's, case-insensitive.
	 */
	bool compareFileName(const wstring& otherFileName) const;

private:
	FORBID_COPY_AND_ASSIGN(FileNameAttribute);

	// Actual "File Name" attribute pointer.
	const PFILE_NAME m_fileName;
};
 
#endif // _NTFSLIB_FILE_NAME_ATTRIBUTE_H
