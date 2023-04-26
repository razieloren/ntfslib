using std::make_shared;

template<class Attribute>
vector<shared_ptr<Attribute>> MFTRecord::findAttribute(ATTR_TYPE attributeType, bool throwIfNotFound /* = true */) const {
	vector<shared_ptr<Attribute>> attrInstances;
	PCOMMON_ATTR_RECORD attrBounds = (PCOMMON_ATTR_RECORD)((PBYTE)m_fileRecordHeader.get() + m_fileRecordHeader->BytesInUse);
	PCOMMON_ATTR_RECORD currentAttribute = (PCOMMON_ATTR_RECORD)((PBYTE)m_fileRecordHeader.get() + m_fileRecordHeader->AttributeOffset);
	while (currentAttribute->Type != (DWORD)END_OF_ATTRIBUTES && 
		currentAttribute <= attrBounds) {
		if (currentAttribute->Type == (DWORD)attributeType) {
			attrInstances.push_back(make_shared<Attribute>(m_ntfsVolume, currentAttribute));
		}
		currentAttribute = (PCOMMON_ATTR_RECORD)((PBYTE)currentAttribute + currentAttribute->Length);
	}

	// Search in additional attached file records.
	for (shared_ptr<MFTRecord> additionalFile : m_additionalRecords) {
		vector<shared_ptr<Attribute>> tempAttrs = additionalFile->findAttribute<Attribute>(attributeType);
		attrInstances.insert(attrInstances.end(), tempAttrs.begin(), tempAttrs.end());
	}

	if (throwIfNotFound && attrInstances.empty()) {
		NTFSLIB_ERROR(AttributeNotFoundError, NTFSLIB_DEFAULT_ERROR_CODE, "Attribute %#x was not found in record: %#llx", attributeType, m_recordNumber);
	}
	
	return attrInstances;
}