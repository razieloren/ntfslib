#include "FileNameAttribute.h"
#include "..\Misc\StringResource.h"
#include "..\NTFSUtils.h"

FileNameAttribute::FileNameAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute):
	AttributeRecord(ntfsVolume, attribute),
	m_fileName((PFILE_NAME)m_attribute->getDataPointer()) {
	// Left blank.
}

wstring FileNameAttribute::getFileName() const {
	return wstring((PWCHAR)m_fileName->Name, m_fileName->NameLength);
}

ULONGLONG FileNameAttribute::getParentMFTReference() const {
	return MFT_REF(m_fileName->ParentMFTReference);
}

bool FileNameAttribute::compareFileName(const wstring& otherFileName) const {
	return m_fileName->NameLength == otherFileName.length() &&
		CMP_IWSTR((PWCHAR)m_fileName->NameLength, otherFileName.c_str());
}

