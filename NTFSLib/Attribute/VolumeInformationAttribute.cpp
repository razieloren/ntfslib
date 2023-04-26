#include "VolumeInformationAttribute.h"

VolumeInformationAttribute::VolumeInformationAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute):
	AttributeRecord(ntfsVolume, attribute),
	m_volInfo((PVOLUME_INFORMATION)m_attribute->getDataPointer()) {
	// Left blank.
}

BYTE VolumeInformationAttribute::getMajorVersion() const {
	return m_volInfo->MajorVersion;
}

BYTE VolumeInformationAttribute::getMinorVersion() const {
	return m_volInfo->MinorVersion;
}

bool VolumeInformationAttribute::isVolumeSupported() const {
	return getMajorVersion() >= MINIMUM_NTFS_MAJOR_SUPPORTED;
}
