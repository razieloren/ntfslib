#include "VolumeFile.h"
#include "..\NTFSLibError.h"
#include "..\StringResource.h"

VolumeFile::VolumeFile(const WCHAR volumeLetter):
	m_volumeLetter(volumeLetter) {
	// Checking volume letter.
	NTFSLIB_ASSERT(
		(m_volumeLetter >= 'a' && m_volumeLetter <= 'z') || (m_volumeLetter >= 'A' && m_volumeLetter <= 'Z'),
		BadVolumeCharacterError
	);

	// Formatting volume letter into the volume path.
	WCHAR tempBuffer[_MAX_PATH] = { 0 };
	wsprintf(tempBuffer, StringResource::baseVolumePath, m_volumeLetter);

	// Opening the volume.
	TRACE(DEBUG_LEVEL::INFO, "Opening volume handle: %ws", tempBuffer);
	m_volumeHandle = CreateFile(
		tempBuffer,												// File name
		GENERIC_READ,											// Desired access
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, // Share mode
		NULL,													// Security attributes
		OPEN_EXISTING,											// Creation disposition
		FILE_ATTRIBUTE_NORMAL,									// Flags & Attributes
		NULL													// Template file
	);
	WIN32_ASSERT(m_volumeHandle != INVALID_HANDLE_VALUE);
}

VolumeFile::~VolumeFile() {
	if (!CloseHandle(m_volumeHandle)) {
		TRACE_WITH_ERROR_CODE(DEBUG_LEVEL::CRITICAL, GetLastError(), "Could not close handle to volume: '%wc'", m_volumeLetter);
	}
}

ULONGLONG VolumeFile::seek(ULONGLONG position, SEEK_METHOD seekMethod /*= SEEK_METHOD::BEGIN*/) {
	LARGE_INTEGER largePosition, newPosition;
	largePosition.QuadPart = (LONGLONG)position;
	WIN32_ASSERT(SetFilePointerEx(
		m_volumeHandle,		// File handle
		largePosition,		// Distance to move
		&newPosition,		// New file pointer
		(DWORD)seekMethod	// Move method
		)!= 0);
	return (ULONGLONG)newPosition.QuadPart;
}

DWORD VolumeFile::read(PVOID buffer, DWORD bytesToRead) {
	DWORD bytesRead = 0;
	WIN32_ASSERT(ReadFile(
		m_volumeHandle,	// File handle
		buffer,			// Buffer
		bytesToRead,	// Bytes to read
		&bytesRead,		// Bytes read
		NULL			// Overlapped
		));
	return bytesRead;
}

DWORD VolumeFile::sendIoctl(DWORD code, PVOID inBuffer, DWORD inBufferSize, PVOID outBuffer, DWORD outBufferSize) {
	DWORD bytesRead = 0;
	WIN32_ASSERT(DeviceIoControl(
		m_volumeHandle, // Device 
		code,			// IO Control Code
		inBuffer,		// Input buffer
		inBufferSize,	// Input buffer size
		outBuffer,		// Output buffer
		outBufferSize,	// Output buffer size
		&bytesRead,		// Bytes returned
		NULL			// Overlapped
		) != 0);
	return bytesRead;
}

WCHAR VolumeFile::getVolumeLetter() const {
	return m_volumeLetter;
}
