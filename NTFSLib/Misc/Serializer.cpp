#include "Serializer.h"
#include "NTFSLibError.h"

Serializer::Serializer(DWORD totalLength) :
	m_serializedData(totalLength),
	m_index(0) {
	// Left blank.
}

void Serializer::serialize(PVOID data, DWORD dataLength) {
	NTFSLIB_ASSERT(
		m_index + dataLength < m_serializedData.size(),
		OutOfBoundsError
	);
	memcpy(m_serializedData.data() + m_index, data, dataLength);
	m_index += dataLength;
}

Buffer Serializer::getSerializedData() const {
	return m_serializedData;
}