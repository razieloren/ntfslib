#ifndef _NTFSLIB_INDEX_LIST_H
#define _NTFSLIB_INDEX_LIST_H

#include <vector>

#include "IndexEntry.h"
#include "..\Misc\Defs.h"
#include "..\Types\NTFSTypes.h"

using std::vector;

typedef vector<IndexEntry> Index;

/**
 * INDEX_HEADER (see NTFSTypes.h) wrapper. Parses the related index entries.
 */
class IndexHeader {
public:
	IndexHeader(const INDEX_HEADER& indexHeader);

	/**
	 * Returns the related index entries.
	 */
	const Index& getIndexEntries() const;

private:
	FORBID_COPY_AND_ASSIGN(IndexHeader);

	// Holds the current index entries.
	Index m_entries;
};

#endif // _NTFSLIB_INDEX_LIST_H
