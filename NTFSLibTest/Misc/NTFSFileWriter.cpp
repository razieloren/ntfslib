#include "NTFSFileWriter.h"
#include "..\..\NTFSLib\Misc\NTFSLibError.h"

NTFSFileWriter::NTFSFileWriter(const std::wstring filePath):
	m_bytesWritten(0) {
	m_fileHandle = CreateFileW(filePath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WIN32_ASSERT(m_fileHandle != INVALID_HANDLE_VALUE);
}

NTFSFileWriter::~NTFSFileWriter() {
	if (m_fileHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(m_fileHandle);
	}
}

void NTFSFileWriter::write(PBYTE data, DWORD dataLength) {
	DWORD bytesWritten = 0;
	WIN32_ASSERT(WriteFile(m_fileHandle, (PVOID)data, dataLength, &bytesWritten, NULL));
	m_bytesWritten += dataLength;
	// Flush at least every 5MB.
	if (m_bytesWritten >= 1024 * 1024 * 5) {
		WIN32_ASSERT(FlushFileBuffers(m_fileHandle));
		m_bytesWritten = 0;
	}
}

LONGLONG NTFSFileWriter::getFileSize() const {
	LARGE_INTEGER fileSize;
	WIN32_ASSERT(GetFileSizeEx(m_fileHandle, &fileSize) != 0);
	return fileSize.QuadPart;
}
