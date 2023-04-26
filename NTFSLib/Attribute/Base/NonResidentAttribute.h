#ifndef _NTFSLIB_NON_RESIDENT_ATTRIBUTE_H
#define _NTFSLIB_NON_RESIDENT_ATTRIBUTE_H

#include <vector>

#include "CommonAttribute.h"

using std::vector;

/**
 * An attribute which its data is saved out of the attribute's body (because it's too big), in a form called "data runs".
 * A data run is a range of clusters on the disk, containing all or part of the attribute's data.
 */
class NonResidentAttribute : public CommonAttribute {
public:
	NonResidentAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute);

	// see: CommonAttribute.getData
	virtual void getData(PVOID buffer, ULONGLONG offset, DWORD length) override;

	// see: CommonAttribute.getDataSize
	virtual ULONGLONG getDataSize() const override;

	// see: CommonAttribute.getDataPointer
	virtual PBYTE getDataPointer() const override;

private:
	FORBID_COPY_AND_ASSIGN(NonResidentAttribute);

	// Non-resident record.
	const PNONRESIDENT_ATTR_RECORD m_nonResident;

	// Attribute's data runs. Each data run represents a chunk of the attribute's data.
	DataRunList m_dataRuns;
};

#endif // _NTFSLIB_NON_RESIDENT_ATTRIBUTE_H
