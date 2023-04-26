#include "DataStreamAttribute.h"

DataStreamAttribute::DataStreamAttribute(NTFSVolume& ntfsVolume, PCOMMON_ATTR_RECORD attribute) :
	AttributeRecord(ntfsVolume, attribute) {
	// Left blank.
}

ULONGLONG DataStreamAttribute::getSize() const {
	return m_attribute->getDataSize();
}

void DataStreamAttribute::getData(PVOID buffer, ULONGLONG offset, DWORD length) const {
	m_attribute->getData(buffer, offset, length);
}

wstring DataStreamAttribute::getStreamName() const {
	return m_attribute->getAttributeName();
}
