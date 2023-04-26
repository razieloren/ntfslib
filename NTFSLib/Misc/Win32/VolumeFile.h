#ifndef _NTFSLIB_VOLUME_FILE_H
#define _NTFSLIB_VOLUME_FILE_H

#include "Win32.h"
#include "..\Defs.h"

// Available seeking methods.
enum class SEEK_METHOD {
	// Seek from the beginning of the file.
	BEGIN = FILE_BEGIN,
	// Seek from the current caret position.
	CURRENT = FILE_CURRENT,
	// Seek from the end of the file.
	END = FILE_END
};

/**
* Simple volume handle (actually, just a regular file handle) container.
*/
class VolumeFile {
public:
	VolumeFile(WCHAR volumeLetter);

	~VolumeFile();

	/**
	 * Seeks to <position> according to <seekMethod>.
	 * Returns the new position of the caret.
	 */
	ULONGLONG seek(ULONGLONG position, SEEK_METHOD seekMethod = SEEK_METHOD::BEGIN);

	/**
	 * Reads <bytesToRead> bytes into <buffer>, returns the actual bytes read.
	 */
	DWORD read(PVOID buffer, DWORD bytesToRead);

	/**
	 * Sends an IOCTL to the volume. Input is defined by <inBuffer>, and output will be placed in <outBuffer>.
	 */
	DWORD sendIoctl(DWORD code, PVOID inBuffer, DWORD inBufferSize, PVOID outBuffer, DWORD outBufferSize);

	/**
	 * Returns the volume's letter representation.
	 */
	WCHAR getVolumeLetter() const;

private:
	FORBID_COPY_AND_ASSIGN(VolumeFile);

	// Volume's letter (e.g. 'C').
	const WCHAR m_volumeLetter;

	// Volume's handle.
	HANDLE m_volumeHandle;
};

#endif // _NTFSLIB_VOLUME_FILE_H
