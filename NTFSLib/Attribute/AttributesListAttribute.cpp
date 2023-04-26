#include "AttributesListAttribute.h"

AttributesListAttribute::AttributesListAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute) :
	AttributeRecord(ntfsVolume, attribute) {
	// Left blank.
}

AdditionalRecordRefs AttributesListAttribute::getAdditionalMFTReferences() const {
	AdditionalRecordRefs mftRefs;
	ULONGLONG offset = 0, totalLength = m_attribute->getDataSize();
	while (offset < totalLength) {
		ATTRIBUTE_LIST attrList;
		m_attribute->getData(&attrList, offset, sizeof(ATTRIBUTE_LIST));
		mftRefs.insert(MFT_REF(attrList.MFTReference));
		if (attrList.RecordSize == 0) {
			return mftRefs;
		}
		offset += attrList.RecordSize;
	}

	return mftRefs;
}