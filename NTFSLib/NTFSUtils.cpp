#include "NTFSUtils.h"
#include "Misc\NTFSLibError.h"
#include "Misc\StringResource.h"

wstring NTFSUtils::expandEnvironmentVariable(const wstring& env) {
	WCHAR outBuffer[_MAX_PATH + 1] = { 0 };
	WIN32_ASSERT(ExpandEnvironmentStringsW(env.c_str(), outBuffer, _MAX_PATH + 1));
	return wstring(outBuffer);
}

vector<wstring> NTFSUtils::splitWidePath(const wstring& path) {
	vector<wstring> parts;
	WCHAR tempString[_MAX_PATH] = { 0 };
	wcsncpy(tempString, path.c_str(), path.length());
	PWCHAR buffer;
	PWCHAR part = wcstok(tempString, StringResource::windowsPathSeperator, &buffer);
	while (part) {
		parts.push_back(part);
		part = wcstok(nullptr, StringResource::windowsPathSeperator, &buffer);
	}

	return parts;
}

void NTFSUtils::USARecordFixup(PNTFS_RECORD ntfsRecord, WORD sectorSize) {
	PWORD usa = (PWORD)((PBYTE)ntfsRecord + ntfsRecord->USAOffset);
	WORD usn = usa[0];

	for (WORD i = 1; i < ntfsRecord->USACount; ++i) {
		PWORD lastWordOfSector = ((PWORD)((PBYTE)ntfsRecord + (sectorSize * i)) - 1);
		NTFSLIB_ASSERT(
			*lastWordOfSector == usn,
			USAFixupError
		);
		*lastWordOfSector = usa[i];
	}
}
