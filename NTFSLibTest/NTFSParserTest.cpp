#include <gtest\gtest.h>

#include "..\NTFSLib\NTFSLib.h"
#include "..\NTFSLib\Misc\Defs.h"
#include "Misc\NTFSFileWriter.h"

using std::wstring;
using std::string;

#define MY_VOLUME_NAME L"Destiny"

// Assuming 64-bit windows.
#define DOS_NAME_DIR L"C:\\PROGRA~1"
#define ENV_NAME_DIR L"%SystemRoot%\\Boot"

#define TEST_DIR L"C:\\NTFSLibTestingGrounds"
#define CONTENT_FILE L"42.txt"
#define HIDDEN_FILE L"Hidden.txt"
#define SYSTEM_FILE L"System.txt"
#define READ_ONLY_FILE L"ReadOnly.txt"
#define STREAMS_FILE L"Streams.txt"
#define NOT_ARCHIVED_FILE L"NotArchived.txt"
#define LONG_DIRECTORY_NAME L"LongDirectoryName"
#define EMPTY_DIRECTORY L"Empty Folder"

#define DUMP_DIR L"C:\\NTFSLibTestingGrounds\\Dumps"
#define BIG_FILE_NAME L"BigFile.bin"
#define TEMP_OUTPUT L"Temp.bin"

#define BAD_FILE L"C:\\If\\You\\Create\\This\\File\\You\\Ruin\\The\\Tests\\Think\\About\\The\\Unicorns.please"

// Checking volume attributes.
TEST(NTFSParserTest, ReadingVolumeAtrtributes) {
	try {
		NTFSParser ntfsParser('C');
		VolumeAttributes volAttrs = ntfsParser.getVolumeAttributes();
		ASSERT_EQ(volAttrs.MajorVersion, 3);
		ASSERT_EQ(volAttrs.MinorVersion, 1);
		ASSERT_STREQ(volAttrs.Name.c_str(), MY_VOLUME_NAME);
	}
	catch (...) {
		FAIL();
	}
}

// Regular "Dir" operation.
TEST(NTFSParserTest, RegularListFiles) {
	try {
		NTFSParser ntfsParser('C');
		Dir files = ntfsParser.listFiles(TEST_DIR, true, 5);
		ASSERT_EQ(files.size(), 9);
		for (const auto file : files) {
			if (file->Root->getFriendlyFileName() == HIDDEN_FILE) {
				ASSERT_TRUE(file->Root->getExtendedInfo()->isHidden());
			}
			else if (file->Root->getFriendlyFileName() == SYSTEM_FILE) {
				ASSERT_TRUE(file->Root->getExtendedInfo()->isSystemFile());
			}
			else if (file->Root->getFriendlyFileName() == READ_ONLY_FILE) {
				ASSERT_TRUE(file->Root->getExtendedInfo()->isReadOnly());
			}
			else if (file->Root->getFriendlyFileName() == NOT_ARCHIVED_FILE) {
				ASSERT_FALSE(file->Root->getExtendedInfo()->isArchived());
			}
			else if (file->Root->getFriendlyFileName() == STREAMS_FILE) {
				ASSERT_EQ(file->Root->getSize(), 22);
				ASSERT_EQ(file->Root->getTotalSize(), 70);
			}
			else if (file->Root->getFriendlyFileName() == LONG_DIRECTORY_NAME) {
				ASSERT_TRUE(file->Root->isDirectory());
				ASSERT_EQ(file->Children.size(), 2);
				for (const auto& innerFile : file->Children) {
					if (file->Root->getFriendlyFileName() == EMPTY_DIRECTORY) {
						ASSERT_TRUE(innerFile->Root->isDirectory());
						ASSERT_EQ(innerFile->Children.size(), 0);
					}
				}
			}
		}
	}
	catch (...) {
		FAIL();
	}
}

// "Dir" operation with DOS name (e.g. PROGRA~1).
TEST(NTFSParserTest, DOSNameListFiles) {
	try {
		NTFSParser ntfsParser('C');
		ntfsParser.listFiles(DOS_NAME_DIR);
	}
	catch (...) {
		FAIL();
	}
}

// "Dir" operation with environment variable (e.g. %SystemRoot%).
TEST(NTFSParserTest, EnvironmentVariableListFiles) {
	try {
		NTFSParser ntfsParser('C');
		ntfsParser.listFiles(ENV_NAME_DIR);
	}
	catch (...) {
		FAIL();
	}
}

// Trying to list files in a fake directory.
TEST(NTFSParserTest, BadListFiles) {
	try {
		NTFSParser ntfsParser('C');
		ntfsParser.listFiles(BAD_FILE);
	}
	catch (MFTRecordNotFoundError&) {
		// Good!
	}
	catch (...) {
		FAIL();
	}
}

// Dumps a small file.
TEST(NTFSParserTest, DumpFile) {
	try {
		string realContent = "424242424242 \r\n";
		NTFSParser ntfsParser('C');
		NTFSFileWriter fileWriter(wstring(DUMP_DIR) + L"\\" + wstring(TEMP_OUTPUT));
		ntfsParser.dumpFile(fileWriter, 3, wstring(TEST_DIR) + L"\\" + wstring(CONTENT_FILE));
		ASSERT_EQ(fileWriter.getFileSize(), realContent.length());
	}
	catch (...) {
		FAIL();
	}
}

// Dumps part of a small file.
TEST(NTFSParserTest, DumpPartOfFile) {
	try {
		string realContent = "42424";
		NTFSParser ntfsParser('C');
		NTFSFileWriter fileWriter(wstring(DUMP_DIR) + L"\\" + wstring(TEMP_OUTPUT));
		ntfsParser.dumpFile(fileWriter, 3, wstring(TEST_DIR) + L"\\" + wstring(CONTENT_FILE), L"", 2, 5);
		ASSERT_EQ(fileWriter.getFileSize(), realContent.length());
	}
	catch (...) {
		FAIL();
	}
}

#ifndef LIGHT_TESTS
// Dumps a big file.
TEST(NTFSParserTest, DumpBigFile) {
	try {
		NTFSParser ntfsParser('C');
		NTFSFileWriter fileWriter(wstring(DUMP_DIR) + L"\\" + wstring(TEMP_OUTPUT));
		ntfsParser.dumpFile(fileWriter, 8192, wstring(DUMP_DIR) + L"\\" + wstring(BIG_FILE_NAME), L"", 0, 0);
		ASSERT_EQ(fileWriter.getFileSize(), 1024 * 1024 * 256);
	}
	catch (...) {
		FAIL();
	}
}

// Lists volume changes over time.
TEST(NTFSParserTest, ListDiffs) {
	try {
		NTFSParser ntfsParser('C');
		for (size_t i = 0; i < 5; ++i) {
			Sleep(1000 * 5);
			DiffList changes = ntfsParser.listDiffs();
			// Not asserting changes size, since it can actually be 0.
		}
	}
	catch (...) {
		FAIL();
	}
}

// Dumps full "Dir" into a product file.
TEST(NTFSParserTest, DumpFullDir) {
	try {
		NTFSParser ntfsParser('C');
		NTFSFileWriter fileWriter(wstring(DUMP_DIR) + L"\\" + wstring(TEMP_OUTPUT));
		ntfsParser.dumpFullDir(fileWriter, 2000);
	}
	catch (...) {
		FAIL();
	}
}
#endif