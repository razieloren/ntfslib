#include "Event.h"
#include "..\NTFSLibError.h"

Event::Event(const wstring& eventName):
	m_eventName(eventName),
	m_eventHandle(CreateEvent(
		NULL,				// Security attributes.
		FALSE,				// Manual reset
		FALSE,				// Initial state
		m_eventName.c_str() // Event name
	)) {
	WIN32_ASSERT(m_eventHandle != NULL);

	// Indicates whether we created a new event or opened an existing one.
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		TRACE(DEBUG_LEVEL::INFO, "Event %ws already exists, opening it", m_eventName.c_str());
	}
	else {
		TRACE(DEBUG_LEVEL::INFO, "Event %ws has been created", m_eventName.c_str());
	}
}

Event::~Event() {
	if (!CloseHandle(m_eventHandle)) {
		TRACE_WITH_ERROR_CODE(DEBUG_LEVEL::CRITICAL, GetLastError(), "Could not close event %ws", m_eventName.c_str());
	}
}

void Event::signal() {
	TRACE(DEBUG_LEVEL::VERBOSE, "Signaling event %ws", m_eventName.c_str());
	WIN32_ASSERT(SetEvent(m_eventHandle));
}

bool Event::isSignaled(DWORD timeout /* = EVENT_TIMEOUT_SAMPLE */) {
	DWORD retVal = WaitForSingleObject(m_eventHandle, timeout);
	WIN32_ASSERT(retVal != WAIT_FAILED);
	return retVal == WAIT_OBJECT_0;
}
