#ifndef _NTFSLIB_SERIALIZER_H
#define _NTFSLIB_SERIALIZER_H

#include "Defs.h"

/**
 * Simple data serialization utilities.
 */
class Serializer {
public:
	Serializer(DWORD totalLength);

	/**
	 * Serializes specified <data> to the buffer.
	 */
	void serialize(PVOID data, DWORD dataLength);

	/**
	 * Returns the serialized data.
	 */
	Buffer getSerializedData() const;

private:
	FORBID_COPY_AND_ASSIGN(Serializer);

	// Currently serialized data.
	Buffer m_serializedData;

	// Current index in the buffer.
	DWORD m_index;
};

#endif // _NTFSLIB_SERIALIZER_H
