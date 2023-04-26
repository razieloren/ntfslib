#ifndef _NTFSLIB_INDEX_ALLOCATION_ATTRIBUTE_H
#define _NTFSLIB_INDEX_ALLOCATION_ATTRIBUTE_H

#include <memory>

#include "..\Misc\Defs.h"
#include "Base\AttributeRecord.h"
#include "..\Record\IndexRecord.h"

using std::shared_ptr;

/**
 * Every folder owns an IndexRoot attribute which lists every index entry under it.
 * However, sometimes the IndexRoot attribute is too small to contain all the index data.
 * As a result, NTFS produces an IndexAllocation attribute which holds the remainder of index records.
 */
class IndexAllocationAttribute : public AttributeRecord {
public:
	IndexAllocationAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute);

	/**
	 * Reads an index record from the index allocation space.
	 */
	shared_ptr<IndexRecord> readIndexRecord(ULONGLONG indexRecordNumber);

private:
	FORBID_COPY_AND_ASSIGN(IndexAllocationAttribute);
};

#endif // _NTFSLIB_INDEX_ALLOCATION_ATTRIBUTE_H
