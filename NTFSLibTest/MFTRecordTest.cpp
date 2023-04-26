#include <gtest\gtest.h>
#include <memory>
#include <vector>
#include <string>

#include "..\NTFSLib\NTFSLib.h"

using std::shared_ptr;
using std::string;
using std::wstring;
using std::vector;
using std::sort;

#define TEST_DIR L"C:\\NTFSLibTestingGrounds"
#define CONTENT_FILE L"42.txt"
#define STREAMS_FILE L"Streams.txt"
#define LONG_DIRECTORY_NAME L"LongDirectoryName"
#define DUMP_DIR L"C:\\NTFSLibTestingGrounds\\Dumps"
#define BIG_FILE_NAME L"BigFile.bin"

#define BAD_FILE L"C:\\If\\You\\Create\\This\\File\\You\\Ruin\\The\\Tests\\Think\\About\\The\\Unicorns.please"

// Lists file named streams.
TEST(MFTRecordTest, ListFileStreams) {
	try {
		vector<wstring> expectedNames = { L"stream1", L"stream2", L"stream9001" };
		NTFSParser ntfsParser('C');
		shared_ptr<MFTRecord> record = ntfsParser.findMFTRecord(wstring(TEST_DIR) + L"\\" + wstring(STREAMS_FILE));
		auto streams = record->listStreams();
		ASSERT_EQ(streams.size(), 3);
		sort(expectedNames.begin(), expectedNames.end());
		sort(streams.begin(), streams.end());
		ASSERT_EQ(expectedNames, streams);
	}
	catch (...) {
		FAIL();
	}
}

// Searches for all record names.
TEST(MFTRecordTest, ListAllFileNames) {
	try {
		vector<wstring> expectedNames = { L"LongDirectoryName", L"LONGDI~1" };
		NTFSParser ntfsParser('C');
		shared_ptr<MFTRecord> record = ntfsParser.findMFTRecord(wstring(TEST_DIR) + L"\\" + wstring(LONG_DIRECTORY_NAME));
		auto names = record->getFileNames();
		ASSERT_EQ(names.size(), 2);
		sort(expectedNames.begin(), expectedNames.end());
		sort(names.begin(), names.end());
		ASSERT_EQ(expectedNames, names);
	}
	catch (...) {
		FAIL();
	}
}

// Reads a small file to a buffer.
TEST(MFTRecordTest, GoodReadFileToBuffer) {
	try {
		string realContent = "424242424242 \r\n";
		NTFSParser ntfsParser('C');
		shared_ptr<MFTRecord> record = ntfsParser.findMFTRecord(wstring(TEST_DIR) + L"\\" + std::wstring(CONTENT_FILE));
		BYTE buffer[64] = { 0 };
		record->read(buffer);
		string readContent((PCHAR)buffer);
		ASSERT_STREQ(realContent.c_str(), readContent.c_str());
	}
	catch (...) {
		FAIL();
	}
}

// Reads a fake file.
TEST(MFTRecordTest, BadReadFileToBuffer) {
	try {
		NTFSParser ntfsParser('C');
		shared_ptr<MFTRecord> record = ntfsParser.findMFTRecord(BAD_FILE);
		BYTE buffer[64] = { 0 };
		record->read(buffer);
		FAIL();
	}
	catch (MFTRecordNotFoundError&) {
		// Good!
	}
	catch (...) {
		FAIL();
	}
}

// Reads part of a small file.
TEST(MFTRecordTest, ReadPartOfFileToBuffer) {
	try {
		std::string realContent = "4242";
		NTFSParser ntfsParser('C');
		shared_ptr<MFTRecord> record = ntfsParser.findMFTRecord(std::wstring(TEST_DIR) + L"\\" + std::wstring(CONTENT_FILE));
		BYTE buffer[64] = { 0 };
		record->read(buffer, L"", 2, 4);
		std::string readContent((PCHAR)buffer);
		ASSERT_STREQ(realContent.c_str(), readContent.c_str());
	}
	catch (...) {
		FAIL();
	}
}

#ifndef LIGHT_TESTS
// Reads a bug file to buffer.
TEST(MFTRecordTest, ReadBigFileToBuffer) {
	try {
		// Only last 20 bytes.
		string realContent = "01234567890123456789";
		NTFSParser ntfsParser('C');
		shared_ptr<MFTRecord> record = ntfsParser.findMFTRecord(std::wstring(DUMP_DIR) + L"\\" + std::wstring(BIG_FILE_NAME));
		Buffer buffer(1024 * 1024 * 257, 0);
		record->read(buffer.data(), L"", 0, 0);
		ASSERT_EQ(memcmp(realContent.c_str(), buffer.data() + buffer.size() - realContent.length(), realContent.length()), 0);
	}
	catch (...) {
		FAIL();
	}
}
#endif // LIGHT_TESTS