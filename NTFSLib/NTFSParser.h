#ifndef _NTFSLIB_NTFS_PARSER_H
#define _NTFSLIB_NTFS_PARSER_H

#include <memory>
#include <vector>
#include <map>

#include "NTFSVolume.h"
#include "NTFSOutStream.h"
#include "Record\MFTRecord.h"
#include "Record\IndexRecord.h"
#include "Types\ChangeJournalTypes.h"
#include "Misc\Win32\Event.h"

using std::shared_ptr;
using std::vector;
using std::map;

/**
* Dir product created with NTFSParser Dir methods.
*/
struct DirProduct;
typedef vector<shared_ptr<DirProduct>> Dir;
struct DirProduct {
	DirProduct(shared_ptr<MFTRecord> rootRecord) :
		Root(rootRecord) {
		// Left blank.
	}

	shared_ptr<MFTRecord> Root;
	Dir Children;
};

/**
 * Supplies a friendly API to deal with NTFS.
 */
class NTFSParser {
public:
	/**
	 * Creates a new parser for the specified volume.
	 */
	NTFSParser(WCHAR volumeLetter);

	/**
	 * Finds the MFT record with the given <path>.
	 * Throws MFTRecordNotFoundError if not found.
	 */
	shared_ptr<MFTRecord> findMFTRecord(const wstring& path);

	/**
	 * Lists all the files changed since the last time queried.
	 * The parser "starts" to count whenever it is initialized.
	 * You can specify a change reason mask: https://msdn.microsoft.com/en-us/library/windows/desktop/aa365722(v=vs.85).aspx
	 */
	DiffList listDiffs(DWORD reason = 0xffffffff);

	/**
	 * Lists files in a given directory.
	 */
	Dir listFiles(const wstring path = L"C:", bool recursive = false, int maxDepth = 1);

	/**
	 * Dumps a full file list of this computer into an NTFSOutStream.
	 * You can limit the flush size with maxFileRecordsPerFlush.
	 */
	void dumpFullDir(NTFSOutStream& outStream, WORD maxFileRecordsPerFlush);

	/**
	 * Dumps a specific file's data stream to NTFSOutStream.
	 * Use <length> = 0 to read the whole stream.
	 */
	void dumpFile(NTFSOutStream& outStream, DWORD maxBlockSizePerFlush, const wstring& filePath, const wstring& streamName = L"", ULONGLONG offset = 0, DWORD amount = 0);

	/**
	 * Signals dumpFullDir to stop (you can call this from another thread).
	 */
	void stopFullDir();

	/**
	* Signals dumpFile to stop (you can call this from another thread).
	*/
	void stopFileDump();

	/**
	 * Returns the current volume attributes (volume's version, serial number and name).
	 */
	const VolumeAttributes& getVolumeAttributes() const;

private:
	/**
	 * Finds <fileName> in a given <folder>.
	 */
	shared_ptr<MFTRecord> findMFTRecordInFolder(shared_ptr<MFTRecord> folder, const wstring& fileName);

	/**
	 * Finds <fileName> in a given sub node, derived from <subNodeVCN> (read from <rootRecord>).
	 */
	shared_ptr<MFTRecord> findMFTRecordInSubNode(shared_ptr<MFTRecord> rootRecord, const wstring& fileName, ULONGLONG subNodeVCN);

	/**
	 * Reads an MFT record from the disk.
	 */
	shared_ptr<MFTRecord> readMFTRecord(ULONGLONG recordIndex);

	/**
	 * Verifies the records magic, fixes USN and resolves external file records.
	 */
	shared_ptr<MFTRecord> finalizeMFTRecord(Buffer& mftRecordBuffer);

	/**
	 * Resolves the full path of s single record.
	 */
	wstring resolveFullFilePath(shared_ptr<MFTRecord> leaf, map<ULONGLONG, DiffLocalCache>& localCache);

	/**
	 * Lists all the files in a given directory.
	 */
	Dir listDirectoryFiles(shared_ptr<MFTRecord> root, bool recursive, int maxDepth);

	/**
	 * Lists all the records under a given sub-node.
	 */
	void listSubNodeRecords(Dir& subNodeRecords, shared_ptr<MFTRecord> folder, ULONGLONG subNodeVCN, bool recursive, int maxDepth);

	// Reference to the MFT (which is just another record).
	shared_ptr<MFTRecord> m_MFTRecord;

	// Volume attributes.
	VolumeAttributes m_volumeAttributes;

	// Actual volume handle.
	NTFSVolume m_volume;

	// Event used to stop FullDir method.
	Event m_stopFullDirEvent;

	// Event used to stop DumpFile method.
	Event m_stopFileDumpEvent;
};

#endif // _NTFSLIB_NTFS_PARSER_H
