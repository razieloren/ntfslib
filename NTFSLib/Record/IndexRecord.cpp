#include "IndexRecord.h"

IndexRecord::IndexRecord(const INDEX_RECORD& indexRecord):
	m_index(indexRecord.Index){
	// Left blank.
}

const Index& IndexRecord::getIndexEntries() const {
	return m_index.getIndexEntries();
}
