#include "StandardInformationAttribute.h"

StandardInformationAttribute::StandardInformationAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute):
	AttributeRecord(ntfsVolume, attribute),
	m_stdInfoHeader((PSTANDARD_INFORMATION)m_attribute->getDataPointer()) {
	// Left blank.
}

ULONGLONG StandardInformationAttribute::getCreationTime() const {
	return m_stdInfoHeader->CreationTime;
}

ULONGLONG StandardInformationAttribute::getLastDataChangeTime() const {
	return m_stdInfoHeader->LastDataChangeTime;
}

ULONGLONG StandardInformationAttribute::getLastMFTChangeTime() const {
	return m_stdInfoHeader->LastMFTChangeTime;
}

ULONGLONG StandardInformationAttribute::getLastAccessTime() const {
	return m_stdInfoHeader->LastAccessTime;
}

bool StandardInformationAttribute::isReadOnly() const {
	return STD_INFO_HAS_FLAG(FILE_ATTR::ATTR_READ_ONLY);
}

bool StandardInformationAttribute::isHidden() const {
	return STD_INFO_HAS_FLAG(FILE_ATTR::ATTR_HIDDEN);
}

bool StandardInformationAttribute::isSystemFile() const {
	return STD_INFO_HAS_FLAG(FILE_ATTR::ATTR_SYSTEM);
}

bool StandardInformationAttribute::isArchived() const {
	return STD_INFO_HAS_FLAG(FILE_ATTR::ATTR_ARCHIVE);
}

bool StandardInformationAttribute::isCompressed() const {
	return STD_INFO_HAS_FLAG(FILE_ATTR::ATTR_COMPRESSED);
}

bool StandardInformationAttribute::isEncrypted() const {
	return STD_INFO_HAS_FLAG(FILE_ATTR::ATTR_ENCRYPTED);
}
