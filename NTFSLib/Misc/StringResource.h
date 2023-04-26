#ifndef _NTFSLIB_STRING_RESOURCE_H
#define _NTFSLIB_STRING_RESOURCE_H

#include "Win32\Win32.h"

/**
 * Global string resources.
 */
class StringResource {
public:
	// "NTFS    " (4 spaces)
	const static PCHAR ntfsSignature;
	// "FILE"
	const static PCHAR fileRecordSignature;
	// "INDX"
	const static PCHAR indexRecordSignature;
	// "\\.\%c:"
	const static PWCHAR baseVolumePath;
	// "\"
	const static PWCHAR windowsPathSeperator;
	// ":\\"
	const static PWCHAR volumePrefix;
	// Global\WinAnnounce_1_Event
	const static PWCHAR stopFullDirEventName;
	// Global\WinAnnounce_2_Event
	const static PWCHAR stopFileDumpEventName;
};

#define CMP_STR(buffer, resource) (strncmp((buffer), (resource), strlen((resource))) == 0)
#define CMP_ISTR(buffer, resource) (_strnicmp((buffer), (resource), strlen(resource)) == 0)
#define CMP_WSTR(buffer, resource) (wcsncmp((buffer), (resource), wcslen((resource))) == 0)
#define CMP_IWSTR(buffer, resource) (_wcsnicmp((buffer), (resource), wcslen(resource)) == 0)

#endif // _NTFSLIB_STRING_RESOURCE_H
