#ifndef _NTFSLIB_INDEX_ROOT_ATTRIBUTE_H
#define _NTFSLIB_INDEX_ROOT_ATTRIBUTE_H

#include "..\Misc\Defs.h"
#include "..\Misc\IndexHeader.h"
#include "Base\AttributeRecord.h"

/**
 * Every directory owns an IndexRoot attribute which lists all the sub-entries of the directory.
 */
class IndexRootAttribute : public AttributeRecord {
public:
	IndexRootAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute);

	/**
	 * Returns all the directory's sub-entries.
	 */
	const Index& getIndexEntries() const;

private:
	FORBID_COPY_AND_ASSIGN(IndexRootAttribute);

	// IndexList instance which is used to parse the index entries.
	const IndexHeader m_index;
};

#endif // _NTFSLIB_INDEX_ROOT_ATTRIBUTE_H
