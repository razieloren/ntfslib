#ifndef _NTFSLIB_WIN32_H
#define _NTFSLIB_WIN32_H

// Exclude unnecessary, size-consuming APIs.
#ifndef NOCOMM
#define NOCOMM
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
// Explicitly including WinIoCtl since it was removed in WIN32_LEAN_AND_MEAN.
#include <WinIoCtl.h>

#endif // _NTFSLIB_WIN32_H
