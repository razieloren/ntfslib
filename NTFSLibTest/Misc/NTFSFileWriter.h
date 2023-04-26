#ifndef _NTFSLIB_NTFS_FILE_WRITER
#define _NTFSLIB_NTFS_FILE_WRITER

#include <string>

#include "..\..\NTFSLib\NTFSOutStream.h"

class NTFSFileWriter : public NTFSOutStream {
public:
	NTFSFileWriter(const std::wstring filePath);

	virtual ~NTFSFileWriter();

	virtual void write(PBYTE data, DWORD dataLength);

	LONGLONG getFileSize() const;

private:
	HANDLE m_fileHandle;

	DWORD m_bytesWritten;
};

#endif // _NTFSLIB_NTFS_FILE_WRITER
