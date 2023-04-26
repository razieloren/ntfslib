#ifndef _NTFSLIB_STANDARD_INFORMATION_ATRIBUTE_H
#define _NTFSLIB_STANDARD_INFORMATION_ATRIBUTE_H

#include "..\Misc\Defs.h"
#include "Base\AttributeRecord.h"

/**
 * Holds file times and file attributes for the MFT record.
 */
class StandardInformationAttribute : public AttributeRecord {
public:
	StandardInformationAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute);

	/**
	 * Returns the creation time of the record, in FILETIME format.
	 */
	ULONGLONG getCreationTime() const;

    /**
	 * Returns the last time the file content changed (e.g. one of the data streams), in FILETIME format.
	 */
	ULONGLONG getLastDataChangeTime() const;

	/**
	 * Returns the last time the MFT record changed (e.g. everything, metadata & actual data streams), in FILETIME format.
	 */
	ULONGLONG getLastMFTChangeTime() const;

	/**
	 * Returns the last access time of the record, in FILETIME format.
	 * NOTICE: Turned off by default on Windows 7+ (for performance).
	 * (Basically means it's always the same as CreationTime).
	 * If 'FSUTIL behavior query disablelastaccess' == 1, it's disabled.
	 * Furthermore, this attribute never gets updated on read-only volumes.
	 */
	ULONGLONG getLastAccessTime() const;

	/**
	 * Returns true if this record is read-only, false otherwise (Marked ad 'R' in ATTRIB windows command line tool).
	 */
	bool isReadOnly() const;

	/**
	 * Returns true if this record is hidden, false otherwise (Marked ad 'H' in ATTRIB windows command line tool).
	 */
	bool isHidden() const;

	/**
	* Returns true if this record is a system file, false otherwise (Marked ad 'S' in ATTRIB windows command line tool).
	*/
	bool isSystemFile() const;

	/**
	 * Returns true if this record is marked for archive, false otherwise (Marked ad 'A' in ATTRIB windows command line tool).
	 * NOTICE: Most of the files are marked for archive by default.
	 * Archiving in WindowsNT == Backup. Whenever a file is created the "archive bit"
	 * is set, and whenever	the file has been backed up it's cleared.
	 * In other words, if this function returns "true", the file has NOT been backed up by Windows.
	 */
	bool isArchived() const;

	/**
	 * Returns true if this record is compressed, false otherwise.
	 * That's NTFS Compression.
	 */
	bool isCompressed() const;

	/**
	 * Returns true if this record is encrypted, false otherwise (EFS, Encrypted File System).
	 */
	bool isEncrypted() const;

private:
	FORBID_COPY_AND_ASSIGN(StandardInformationAttribute);

	// Standard information structure pointer.
	const PSTANDARD_INFORMATION m_stdInfoHeader;
};

#define STD_INFO_HAS_FLAG(flag) ((m_stdInfoHeader->Flags & (b4)(flag)) != 0)

#endif // _NTFSLIB_STANDARD_INFORMATION_ATRIBUTE_H
