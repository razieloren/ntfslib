#ifndef _NTFSLIB_INDEX_RECORD_H
#define _NTFSLIB_INDEX_RECORD_H

#include "..\Misc\Defs.h"
#include "..\Misc\IndexHeader.h"
#include "..\Types\NTFSTypes.h"

/**
 * Index records are used whenever the are too many index entries for the IndexRoot to contain.
 * They are read from the index allocation.
 */ 
class IndexRecord {
public:
	IndexRecord(const INDEX_RECORD& indexRecord);

	/**
	 * Returns all the index entries related to this index record.
	 */
	const Index& getIndexEntries() const;

private:
	FORBID_COPY_AND_ASSIGN(IndexRecord);

	// Index header related to this index record.
	const IndexHeader m_index;
};

#endif // _NTFSLIB_INDEX_RECORD_H
