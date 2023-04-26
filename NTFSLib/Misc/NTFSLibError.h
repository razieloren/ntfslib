#ifndef _NTFSLIB_NTFS_ERROR_H
#define _NTFSLIB_NTFS_ERROR_H

#include <stdexcept>
#include <string>

#include "Defs.h"
#include "Win32\Win32.h"

using std::runtime_error;
using std::string;

#define NTFSLIB_DEFAULT_ERROR_CODE 0

/**
 * Simple wrapper for std::runtime_error.
 */
class NTFSLibError : public runtime_error {
public:
	NTFSLibError(const string& message, DWORD errorCode);

	virtual DWORD getErrorCode() const;

private:
	const DWORD m_errorCode;
};

/**
 * Exception definition macro.
 */
#define DEFINE_EXCEPTION(ClassName) \
    class ClassName : public NTFSLibError \
    { \
    public: \
        ClassName(const string& message, DWORD errorCode); \
    }

/**
 * Exception implementation macro.
 */
#define IMPLEMENT_EXCEPTION(ClassName) \
	ClassName::ClassName(const string& message, DWORD errorCode): \
		NTFSLibError(message, errorCode) { }

/**
 * User-defined exceptions.
 */
DEFINE_EXCEPTION(Win32Error);
DEFINE_EXCEPTION(OutOfBoundsError);
DEFINE_EXCEPTION(BadSizeError);
DEFINE_EXCEPTION(UnexpectedActionError);
DEFINE_EXCEPTION(AttributeNotFoundError);
DEFINE_EXCEPTION(BadRecordHeaderError);
DEFINE_EXCEPTION(UnsupportedNTFSVersionError);
DEFINE_EXCEPTION(MFTRecordNotFoundError);
DEFINE_EXCEPTION(BadVolumeCharacterError);
DEFINE_EXCEPTION(BadPathError);
DEFINE_EXCEPTION(USAFixupError);
DEFINE_EXCEPTION(EmptyDataRunError);

// Throw an error with a custom error message.
#define NTFSLIB_ERROR(errorType, errorCode, errorMessage, ...) \
	do { \
		TRACE(DEBUG_LEVEL::INFO, (errorMessage), ##__VA_ARGS__); \
		throw errorType("", errorCode); \
	} while (0)

// Check for a generic condition, and throw the specified exception if failed.
#define NTFSLIB_ASSERT(condition, errorType) \
	do { \
		if (!(condition)) { \
			NTFSLIB_ERROR(errorType, NTFSLIB_DEFAULT_ERROR_CODE, "NTFSLib assertion failed: %s", #condition); \
		} \
	} while(0)

// Check for a Win32 condition, and throw an exception with the relevant error code if failed.
#define WIN32_ASSERT(condition) \
	do { \
		if (!(condition)) { \
			DWORD errorCode = GetLastError(); \
			NTFSLIB_ERROR(Win32Error, errorCode, "Win32 assertion failed: %s (Error: %d)", #condition, errorCode); \
		} \
	} while(0)

#endif // _NTFSLIB_NTFS_ERROR_H