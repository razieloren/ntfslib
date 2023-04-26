#include <gtest\gtest.h>

#include "..\..\NTFSLib\Misc\Win32\Event.h"
#include "..\..\NTFSLib\NTFSLib.h"

// Creates an event.
TEST(Win32EventTest, EventConstructor) {
	try {
		Event myEvent(L"NTFSLibTestEvent");
	}
	catch (...) {
		FAIL();
	}
}

#ifndef LIGHT_TESTS
// Signals a given event.
DWORD WINAPI signalEvent(PVOID myEvent) {
	Sleep(2 * 1000);
	((Event*)myEvent)->signal();
	return 0;
}

// Tries to signal an event with a short timeout.
TEST(Win32EventTest, SignalEventWithTimeout) {
	try {
		Event myEvent(L"NTFSLibTestEvent");
		HANDLE myThread = CreateThread(NULL, 0, signalEvent, &myEvent, 0, NULL);
		bool success = myEvent.isSignaled(4 * 1000);
		WaitForSingleObject(myThread, 5 * 1000);
		CloseHandle(myThread);
		ASSERT_TRUE(success);
	}
	catch (...) {
		FAIL();
	}
}

// Sampling an event (no timeout at all).
TEST(Win32EventTest, SignalEventWithoutTimeout) {
	try {
		Event myEvent(L"NTFSLibTestEvent");
		bool success = false;
		HANDLE myThread = CreateThread(NULL, 0, signalEvent, &myEvent, 0, NULL);
		for (size_t i = 0; i < 20; ++i) {
			if (myEvent.isSignaled()) {
				success = true;
				break;
			}
			Sleep(400);
		}
		
		WaitForSingleObject(myThread, 5 * 1000);
		CloseHandle(myThread);
		ASSERT_TRUE(success);
	}
	catch (...) {
		FAIL();
	}
}
#endif