#include "IndexEntry.h"
#include "StringResource.h"
#include "..\NTFSUtils.h"

IndexEntry::IndexEntry(const PINDEX_ENTRY indexEntry):
	m_subNode((indexEntry->Flags & (b1)INDEX_ENTRY_FLAGS::INDEX_ENTRY_NODE) != 0),
	m_fileReference(MFT_REF(indexEntry->MFTReference)),
	// The sub-node VCN is located in the last 8 bytes of the entry, only if it's a sub-node (of course...).
	m_subNodeVCN(m_subNode ? 0 : *((PULONGLONG)((PBYTE)indexEntry + indexEntry->Size - 8))) {
	// Streams (== MFT references) are only available for non-end entry nodes. However, end-nodes can contain sub-nodes.
	if ((indexEntry->Flags & (b1)INDEX_ENTRY_FLAGS::INDEX_ENTRY_END) == 0) {
		PFILE_NAME fileName = (PFILE_NAME)indexEntry->Stream;
		m_fileName = wstring((PWCHAR)fileName->Name, fileName->NameLength);
	}
	else {
		m_fileName = L"";
	}
}

ULONGLONG IndexEntry::getMFTReference() const {
	return m_fileReference;
}

ULONGLONG IndexEntry::getSubNodeVCN() const {
	return m_subNodeVCN;
}

bool IndexEntry::isSubNode() const {
	return m_subNode;
}

bool IndexEntry::isUserEntry() const {
	return m_fileReference >= (ULONGLONG)NTFS_SYSTEM_FILES::FILE_FirstUser;
}

bool IndexEntry::compareFileName(const wstring& otherFileName) const {
	size_t nameLength = m_fileName.length();
	return nameLength == otherFileName.length() &&
		CMP_IWSTR(m_fileName.c_str(), otherFileName.c_str());
}
