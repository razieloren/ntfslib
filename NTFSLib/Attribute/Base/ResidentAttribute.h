#ifndef _NTFSLIB_RESIDENT_ATTRIBUTE_H
#define _NTFSLIB_RESIDENT_ATTRIBUTE_H

#include "CommonAttribute.h"

/**
 * An attribute which its data is saved inside the attribute's body (since it's small enough).
 */
class ResidentAttribute : public CommonAttribute {
public:
	ResidentAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute);

	// see: CommonAttribute.getData
	virtual void getData(PVOID buffer, ULONGLONG offset, DWORD length) override;

	// see: CommonAttribute.getDataSize
	virtual ULONGLONG getDataSize() const override;

	// see: CommonAttribute.getDataPointer
	virtual PBYTE getDataPointer() const override;

private:
	FORBID_COPY_AND_ASSIGN(ResidentAttribute);

	// Resident record.
	const PRESIDENT_ATTR_RECORD m_resident;
};

#endif // _NTFSLIB_RESIDENT_ATTRIBUTE_H
