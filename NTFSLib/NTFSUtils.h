#ifndef _NTFSLIB_UTILS_H
#define _NTFSLIB_UTILS_H

#include <vector>
#include <string>

#include "Misc\Win32\Win32.h"
#include "Types\NTFSTypes.h"

using std::vector;
using std::wstring;

/**
 * General utility functions.
 */
class NTFSUtils {
public:
	/**
	 * Expands an environment variable as specified in the command 'SET'.
	 * If the string does not represent any environment variable, it'll be returned as-is.
	 */
	static wstring expandEnvironmentVariable(const wstring& env);

	/**
	 * Splits a multi-byte path into its parts.
	 * Returns a vector of these parts.
	 * e.g. C:\path\to\file -> [ C:, path, to, file ]
	 */
	static vector<wstring> splitWidePath(const wstring& path);

	/**
	 * USA = Update Sequence Array.
	 * This is NTFS' way to validate sectors. Whenever a record (MFT / Index) is written
	 * to the disk, NTFS replaces the last word of each sector with a known number - 
	 * The "Update Sequence Number" (USN), which can be found in the record header.
	 * Then, saves the actual last word of the sector in the USA. Afterwards, when the
	 * record is read from the disk, we have to validate the USA and fix the end of sectors.
	 * (Revert the sectors back to normal). The USN is saved as the first word of the USA.
	 * Example:
	 * Before NTFS writes the record to the disk:
	 * USN: AB CD
	 * USA: 00 00 00 00 00 00 (Actual memory representation: AB CD 00 00 00 00 00 00)
	 * ...
	 * End of sector 1: 12 34 56 78 90
	 * End of sector 1: 90 78 56 34 12
	 * End of sector 3: AA BB CC DD EE
	 *
	 * After NTFS writes the record to the disk:
	 * USN: AB CD
	 * USA: 78 90 34 12 DD EE (Actual memory representation: AB CD 78 90 34 12 DD EE)
	 * ...
	 *                            USN
	 *                            \/
	 * End of sector 1: 12 34 56 AB CD
	 * End of sector 1: 90 78 56 AB CD
	 * End of sector 3: AA BB CC AB CD
	 */
	static void USARecordFixup(PNTFS_RECORD ntfsRecord, WORD sectorSize);

	/**
	 * Returns true if <element> is in <vec>, false otherwise.
	 */
	template <class T>
	static bool hasElement(const vector<T> vec, T element);
};

#include "NTFSUtils.inl"

#endif // _NTFSLIB_UTILS_H
