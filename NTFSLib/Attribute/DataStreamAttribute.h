#ifndef _NTFSLIB_DATA_STREAM_ATTRIBUTE_H
#define _NTFSLIB_DATA_STREAM_ATTRIBUTE_H

#include "Base\AttributeRecord.h"

/**
 * Actual data stream of the MFT record.
 */
class DataStreamAttribute : public AttributeRecord {
public:
	DataStreamAttribute(NTFSVolume& ntfsVolume, PCOMMON_ATTR_RECORD attribute);

	/**
	 * Returns the size in bytes of this data stream.
	 */
	ULONGLONG getSize() const;

	/**
	 * Reads <length> bytes of data from this data stream to <buffer>, starting at <offset>.
	 * Throws OutOfBoundsError if <offset> + <length> is greater than the stream's size,
	 * and throws BadSizeError if less than <length> bytes were read. <buffer> size must be big enough
	 * to contain all the data.
	 */
	void getData(PVOID buffer, ULONGLONG offset, DWORD length) const;

	/**
	 * Returns the data stream's name.
	 */
	wstring getStreamName() const;

private:
	FORBID_COPY_AND_ASSIGN(DataStreamAttribute);
};

#endif // _NTFSLIB_DATA_STREAM_ATTRIBUTE_H
