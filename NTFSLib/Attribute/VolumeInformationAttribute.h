#ifndef _NTFSLIB_VOLUME_INFORMATION_ATTRIBUTE_H
#define _NTFSLIB_VOLUME_INFORMATION_ATTRIBUTE_H

#include "..\Misc\Defs.h"
#include "Base\AttributeRecord.h"

// Minimum major version supported (Windows 2000).
#define MINIMUM_NTFS_MAJOR_SUPPORTED 3

/**
 * Holds the volume's version.
 * 1.0 - Since Windows NT 3.1 (1993).
 * 1.1 - Since Windows NT 3.51 (1995).
 * 1.2 - Since Windows NT 4.0 (1996).
 * 3.0 - Since Windows 2000 (2000).
 * 3.1 - Since Windows XP (2001).
 * Do not confuse with the volume's NTFS version (stated above), and the NTFS.sys driver version (same as the Windows version).
 */
class VolumeInformationAttribute : public AttributeRecord {
public:
	VolumeInformationAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute);

	/**
	 * Returns the volume's NTFS major version number.
	 */
	BYTE getMajorVersion() const;

	/**
	 * Returns the volume's NTFS minor version number.
	 */
	BYTE getMinorVersion() const;

	/**
	 * Returns true if this is a supported NTFS volume, false otherwise.
	 */
	bool isVolumeSupported() const;

private:
	FORBID_COPY_AND_ASSIGN(VolumeInformationAttribute);

	// Volume information structure pointer.
	const PVOLUME_INFORMATION m_volInfo;
};

#endif // _NTFSLIB_VOLUME_INFORMATION_ATTRIBUTE_H
