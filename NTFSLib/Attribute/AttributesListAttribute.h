#ifndef _NTFSLIB_ATTRIBUTES_LIST_ATTRIBUTE_H
#define _NTFSLIB_ATTRIBUTES_LIST_ATTRIBUTE_H

#include <set>

#include "..\Misc\Defs.h"
#include "Base\AttributeRecord.h"

using std::set;

typedef set<ULONGLONG> AdditionalRecordRefs;

/**
 * Every MFT attribute is limited to MFT_RECORD_SIZE bytes (mostly 1024 bytes). Whenever the record owns a lot
 * of attributes, it splits the remainder of the attributes on additional MFT records, and instead
 * adds an 'Attributes List' attribute to the original record. With this attribute, we can locate the additional
 * MFT records the File System has used.
 */
class AttributesListAttribute : public AttributeRecord {
public:
	AttributesListAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute);

	/**
	 * Returns all the additional MFT records references related to our MFT record, including our MFT record.
	 */
	AdditionalRecordRefs getAdditionalMFTReferences() const;

private:
	FORBID_COPY_AND_ASSIGN(AttributesListAttribute);
};

#endif // _NTFSLIB_ATTRIBUTES_LIST_ATTRIBUTE_H
