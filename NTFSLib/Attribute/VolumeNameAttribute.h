#ifndef _NTFSLIB_VOLUME_NAME_ATTRIBUTE_H
#define _NTFSLIB_VOLUME_NAME_ATTRIBUTE_H

#include "..\Misc\Defs.h"
#include "Base\AttributeRecord.h"

/**
 * VolumeName is in other words the volume's label. Refer to the Windows Command Line (cmd)
 * command: "label".
 */
class VolumeNameAttribute : public AttributeRecord {
public:
	VolumeNameAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute);

	/**
	 * Returns the volume's label (user-defined name).
	 */
	wstring getVolumeName() const;

private:
	FORBID_COPY_AND_ASSIGN(VolumeNameAttribute);
};

#endif // _NTFSLIB_VOLUME_NAME_ATTRIBUTE_H
