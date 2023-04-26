#include "AttributeRecord.h"
#include "ResidentAttribute.h"
#include "NonResidentAttribute.h"
#include "..\..\NTFSUtils.h"

AttributeRecord::AttributeRecord(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute):
	// Determining the attribute's type.
	m_attribute(attribute->NonResident ? 
		(CommonAttribute*)new NonResidentAttribute(ntfsVolume, attribute) :
		(CommonAttribute*)new ResidentAttribute(ntfsVolume, attribute)) {
	// Left blank.
}

AttributeRecord::~AttributeRecord() {
	delete m_attribute;
}