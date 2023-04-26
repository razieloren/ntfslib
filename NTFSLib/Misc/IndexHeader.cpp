#include "IndexHeader.h"

IndexHeader::IndexHeader(const INDEX_HEADER& indexHeader) {
	PINDEX_ENTRY tempEntry = (PINDEX_ENTRY)((PBYTE)(&(indexHeader.EntriesOffset)) + indexHeader.EntriesOffset);
	DWORD totalSize = tempEntry->Size;

	// Iterating over all entries.
	while (totalSize <= indexHeader.IndexLength) {
		m_entries.push_back(IndexEntry(tempEntry));

		// We are interested to insert the last entry, but then break.
		if ((tempEntry->Flags & (b1)INDEX_ENTRY_FLAGS::INDEX_ENTRY_END) != 0) {
			break;
		}
		tempEntry = (PINDEX_ENTRY)((PBYTE)tempEntry + tempEntry->Size);
		totalSize += tempEntry->Size;
	}
}

const Index& IndexHeader::getIndexEntries() const {
	return m_entries;
}
