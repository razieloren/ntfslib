#ifndef _NTFSLIB_CHANGE_JOURNAL_DEFS_H
#define _NTFSLIB_CHANGE_JOURNAL_DEFS_H

#include <vector>

#include "..\Misc\Win32\Win32.h"
#include "..\NTFSUtils.h"

#define JOURNAL_READ_LENGTH 4096

typedef USN_JOURNAL_DATA_V0 JournalData;
typedef READ_USN_JOURNAL_DATA_V0 JournalReadDef;;

// Single Change Journal record.
struct ChangeJournalRecord {
	LARGE_INTEGER TimeStamp;
	ULONGLONG ReferenceNumber;
	DWORD RecordLength;
	DWORD ChangeReason;
	USN Usn;
};
typedef std::vector<ChangeJournalRecord> ChangeJournalRecordList;

// Actual data we save for each ChangeJournalRecord in the final DiffList.
struct DiffRecord {
	DWORD Reason;
	ULONGLONG TimeStamp;
	std::wstring Path;
};
typedef std::vector<DiffRecord> DiffList;

// Local cache for DiffRecords.
struct DiffLocalCache {
	ULONGLONG ParentReferenceNumber;
	std::wstring Name;
};

#endif // _NTFSLIB_CHANGE_JOURNAL_DEFS_H
