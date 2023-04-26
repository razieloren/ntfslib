#include "IndexAllocationAttribute.h"
#include "..\Misc\StringResource.h"
#include "..\Misc\NTFSLibError.h"

using std::make_shared;

IndexAllocationAttribute::IndexAllocationAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute):
	AttributeRecord(ntfsVolume, attribute) {
	// Left blank.
}

shared_ptr<IndexRecord> IndexAllocationAttribute::readIndexRecord(ULONGLONG indexRecordNumber) {
	WORD indexRecordSize = m_attribute->getVolume().getIndexRecordSize();
	INDEX_RECORD indexRecord;

	// Reading the actual index record data.
	m_attribute->getData(&indexRecord, (DWORD)indexRecordNumber * indexRecordSize, indexRecordSize);

	// Checking for magic and fixing update sequence array.
	NTFSLIB_ASSERT(
		CMP_STR((PCHAR)&indexRecord.RecordHeader.Magic, StringResource::indexRecordSignature),
		BadRecordHeaderError
	);
	WORD sectorSize = m_attribute->getVolume().getSectorSize();
	NTFSUtils::USARecordFixup(&indexRecord.RecordHeader, sectorSize);

	return make_shared<IndexRecord>(indexRecord);
}
