#ifndef _NTFSLIB_NTFS_OUT_STREAM_H
#define _NTFSLIB_NTFS_OUT_STREAM_H

#include "Misc\Win32\Win32.h"

/**
 * Stream object which all the "dump" functions in NTFSParser use.
 */
class NTFSOutStream {
public:
	NTFSOutStream();

	virtual ~NTFSOutStream();

	virtual void write(PBYTE data, DWORD dataLength) = 0;
};

#endif // _NTFSLIB_NTFS_OUT_STREAM_H
