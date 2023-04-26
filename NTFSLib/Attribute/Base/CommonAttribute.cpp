#include "CommonAttribute.h"
#include "..\..\NTFSUtils.h"

CommonAttribute::CommonAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute):
	m_attribute(attribute),
	m_ntfsVolume(ntfsVolume) {
	// Left blank.
}

CommonAttribute::~CommonAttribute() {
	// Left blank.
}

NTFSVolume& CommonAttribute::getVolume() const {
	return m_ntfsVolume;
}

wstring CommonAttribute::getAttributeName() {
	return m_attribute->NameLength ?
		wstring((PWCHAR)((PBYTE)m_attribute + m_attribute->NameOffset), m_attribute->NameLength) 
		: L"";
}

WORD CommonAttribute::getAttributeInstance() const {
	return m_attribute->Instance;
}

DWORD CommonAttribute::getAttributeType() const {
	return m_attribute->Type;
}

DWORD CommonAttribute::getAttirbuteSize() const {
	return m_attribute->Length;
}

bool CommonAttribute::isResident() const {
	return m_attribute->NonResident == 0;
}

bool CommonAttribute::isEncrypted() const {
	return (m_attribute->Flags & (b2)ATTR_FLAGS::ATTR_IS_ENCRYPTED) != 0;
}

bool CommonAttribute::isCompressed() const {
	return (m_attribute->Flags & (b2)ATTR_FLAGS::ATTR_IS_COMPRESSED) != 0;
}

bool CommonAttribute::isSparse() const {
	return (m_attribute->Flags & (b2)ATTR_FLAGS::ATTR_IS_SPARSE) != 0;
}

