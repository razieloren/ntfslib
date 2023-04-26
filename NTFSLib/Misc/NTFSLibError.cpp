#include "NTFSLibError.h"

NTFSLibError::NTFSLibError(const string& message, DWORD errorCode) :
	runtime_error(message.c_str()), m_errorCode(errorCode) {
	// Left blank.
}

DWORD NTFSLibError::getErrorCode() const {
	return m_errorCode;
}

// user-defined exception implementations.
IMPLEMENT_EXCEPTION(Win32Error);
IMPLEMENT_EXCEPTION(OutOfBoundsError);
IMPLEMENT_EXCEPTION(BadSizeError);
IMPLEMENT_EXCEPTION(UnexpectedActionError);
IMPLEMENT_EXCEPTION(AttributeNotFoundError);
IMPLEMENT_EXCEPTION(BadRecordHeaderError);
IMPLEMENT_EXCEPTION(UnsupportedNTFSVersionError);
IMPLEMENT_EXCEPTION(MFTRecordNotFoundError);
IMPLEMENT_EXCEPTION(BadVolumeCharacterError);
IMPLEMENT_EXCEPTION(BadPathError);
IMPLEMENT_EXCEPTION(USAFixupError);
IMPLEMENT_EXCEPTION(EmptyDataRunError);