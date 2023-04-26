#ifndef _NTFSLIB_INDEX_ENTRY_H
#define _NTFSLIB_INDEX_ENTRY_H

#include <string>

#include "Defs.h"
#include "..\Types\NTFSTypes.h"

using std::wstring;

/**
 * A single entry under IndexRoot / IndexRoot index. It contains enough data
 * to read the full related MFT record if desired.
 */
class IndexEntry {
public:
	IndexEntry(const PINDEX_ENTRY indexEntry);

	/**
	 * Returns the MFT record referenced by this entry.
	 */
	ULONGLONG getMFTReference() const;

	/**
	 * Returns the sub-node VCN (if available).
	 */
	ULONGLONG getSubNodeVCN() const;

	/**
	 * Returns true if this entry owns sub-nodes, false otherwise.
	 */
	bool isSubNode() const;

	/**
	 * Returns true if this entry points to a user created record (including the OS, everything but the NTFS system files).
	 */
	bool isUserEntry() const;

	/**
	 * Compared two file names, case-insensitive.
	 */
	bool compareFileName(const wstring& otherFileName) const;

private:
	// Copy and assign is allowed here.
	/* FORBID_COPY_AND_ASSIGN(IndexEntry); */

	bool m_subNode;

	wstring m_fileName;

	ULONGLONG m_fileReference;

	ULONGLONG m_subNodeVCN;
};

#endif // _NTFSLIB_INDEX_ENTRY_H
