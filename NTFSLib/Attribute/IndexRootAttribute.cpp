#include "IndexRootAttribute.h"

IndexRootAttribute::IndexRootAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute):
	AttributeRecord(ntfsVolume, attribute),
	m_index(((PINDEX_ROOT)m_attribute->getDataPointer())->Index) {
	// left blank.
}

const Index& IndexRootAttribute::getIndexEntries() const {
	return m_index.getIndexEntries();
}
