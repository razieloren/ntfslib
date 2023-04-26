#ifndef _NTFSLIB_ATTRIBUTE_RECORD_H
#define _NTFSLIB_ATTRIBUTE_RECORD_H

#include "CommonAttribute.h"
#include "..\..\NTFSVolume.h"
#include "..\..\Types\NTFSTypes.h"

/**
 * Defines the base of every attribute record.
 */
class AttributeRecord {
public:
	AttributeRecord(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute);

	virtual ~AttributeRecord();

protected:
	// Pointer to the attribute header. It can be either resident or non-resident.
	CommonAttribute* m_attribute;

private:
	FORBID_COPY_AND_ASSIGN(AttributeRecord);
};

#endif // _NTFSLIB_ATTRIBUTE_RECORD_H
