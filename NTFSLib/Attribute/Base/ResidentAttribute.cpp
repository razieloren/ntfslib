#include "ResidentAttribute.h"
#include "..\..\Misc\NTFSLibError.h"

ResidentAttribute::ResidentAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute) :
	CommonAttribute(ntfsVolume, attribute),
	m_resident((PRESIDENT_ATTR_RECORD)attribute){
	// Left blank.
}

void ResidentAttribute::getData(PVOID buffer, ULONGLONG offset, DWORD length) {
	NTFSLIB_ASSERT(
		offset + length <= getDataSize(),
		OutOfBoundsError
	);

	memcpy(buffer, getDataPointer() + offset, length);
}

ULONGLONG ResidentAttribute::getDataSize() const {
	return m_resident->ValueLength;
}

PBYTE ResidentAttribute::getDataPointer() const {
	return (PBYTE)m_resident + m_resident->ValueOffset;
}