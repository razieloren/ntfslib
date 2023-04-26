#ifndef _NTFSLIB_DEFS_H
#define _NTFSLIB_DEFS_H

#include <vector>

#include "Win32\Win32.h"

using std::vector;

/**
 * Disabling 'Copy' constructor and 'operator=' constructor for a given type.
 * Usage:
 * class ClassName {
 * public:
 *    ....
 * private:
 *    FORBID_COPY_AND_ASSIGN(ClassName);
 * }
 */
#define FORBID_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&) = delete; \
	TypeName& operator=(const TypeName&) = delete

// Defining a simple alias for buffers.
typedef vector<BYTE> Buffer;

// Available debug levels.
enum class DEBUG_LEVEL {
	// Must-read messages.
	CRITICAL,
	// General information messages.
	INFO,
	// Extended information messages.
	VERBOSE
};

// Current debug level of the program.
static const DEBUG_LEVEL CURRENT_DEBUG_LEVEL = DEBUG_LEVEL::VERBOSE;

#ifdef _DEBUG
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crtdbg.h>

// Extracting file name from a full path.
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

// Defining tracing functionality.
#define TRACE(debugLevel, format, ...) \
	do { \
		if (debugLevel <= CURRENT_DEBUG_LEVEL) { \
			printf("[%s:%d @ %s] ", __FILENAME__, __LINE__, __FUNCTION__); \
			printf((format), ##__VA_ARGS__); \
			printf("\n"); \
		} \
	} while(0)
#define TRACE_WITH_ERROR_CODE(debugLevel, errorCode, format, ...) \
	do { \
		if (debugLevel <= CURRENT_DEBUG_LEVEL) { \
			printf("[%s:%d @ %s] ", __FILENAME__, __LINE__, __FUNCTION__); \
			printf((format), ##__VA_ARGS__); \
			printf(" (Error: %d)", (errorCode)); \
			printf("\n"); \
		} \
	} while(0)

// Defining memory debugging functionality.
#define CRTDBG_MAP_ALLOC
#define SETUP_MEM_LEAK_CHECK() \
	do { \
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE ); \
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT); \
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE); \
		_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT); \
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE); \
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT); \
	} while(0)

#define MEM_LEAK_CHECK() \
	do { \
		SETUP_MEM_LEAK_CHECK(); \
		_CrtDumpMemoryLeaks(); \
	} while(0)

#define MEM_LEAK_CHECK_FROM_CHECKPOINT(checkpoint) \
	do { \
		SETUP_MEM_LEAK_CHECK(); \
		_CrtMemDumpAllObjectsSince(&(checkpoint)); \
	} while(0)

#define MEM_CHECKPOINT(checkpoint) _CrtMemCheckpoint(&(checkpoint))

typedef _CrtMemState MemoryCheckpoint;
#else
#define TRACE(format, ...)
#define TRACE_WITH_ERROR_CODE(format, ...)
#define SETUP_MEM_LEAK_CHECK()
#define MEM_LEAK_CHECK()
#define MEM_LEAK_CHECK_FROM_CHECKPOINT(checkpoint)
#define MEM_CHECKPOINT(checkpoint)

typedef struct {
	char dummy;
} MemoryCheckpoint;
#endif

#endif // _NTFSLIB_DEFS_H
