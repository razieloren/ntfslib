#include "..\NTFSUtils.h"
#include "VolumeNameAttribute.h"

VolumeNameAttribute::VolumeNameAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute):
	AttributeRecord(ntfsVolume, attribute) {
	// Left blank.
}

wstring VolumeNameAttribute::getVolumeName() const {
	return wstring(
		(PWCHAR)m_attribute->getDataPointer(), 
		(DWORD)m_attribute->getDataSize() / sizeof(WCHAR)
	);
}
