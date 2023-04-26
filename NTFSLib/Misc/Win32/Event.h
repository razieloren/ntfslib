#ifndef _NTFSLIB_EVENT_H
#define _NTFSLIB_EVENT_H

#include <string>

#include "Win32.h"
#include "..\Defs.h"

using std::wstring;

// Only sample the event, do not wait at all.
#define EVENT_TIMEOUT_SAMPLE 0
// Busy-wait until the event is signaled.
#define EVENT_TIMEOUT_BUSY_WAIT INFINITE

/**
 * Simple Windows "Event" container.
 * See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa964766(v=vs.85).aspx
 */
class Event {
public:
	/**
	 * Tries to create an event with the specified name.
	 * If it already exists, opening and using it.
	 */
	Event(const wstring& eventName);

	/**
	 * Closing the handle to our event.
	 */
	~Event();

	/**
	 * Signals the event.
	 */
	void signal();
	
	/**
	 * Check whether the event has been signaled.
	 */
	bool isSignaled(DWORD timeout = EVENT_TIMEOUT_SAMPLE);

private:
	FORBID_COPY_AND_ASSIGN(Event);

	// Event's name.
	const wstring m_eventName;

	// Event's handle.
	HANDLE m_eventHandle;
};

#endif // _NTFSLIB_EVENT_H
