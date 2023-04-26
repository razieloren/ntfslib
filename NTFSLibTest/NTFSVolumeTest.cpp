#include <gtest\gtest.h>

#include "..\NTFSLib\NTFSLib.h"
#include "..\NTFSLib\NTFSVolume.h"

using std::wstring;

// Valid volume constructor.
TEST(NTFSVolumeTest, NTFSVolumeConstructor) {
	try {
		NTFSVolume volume('C');
		ASSERT_EQ(volume.getVolumeType(), VOLUME_TYPE::FIXED);

		wstring volPrefix = volume.getVolumePrefix();
		ASSERT_STREQ(volPrefix.c_str(), L"C:\\");
		ASSERT_TRUE(volume.isChangeJournalAvailable());
	}
	catch (...) {
		FAIL();
	}
}

// General boot sector properties.
TEST(NTFSVolumeTest, ValidBootSector) {
	try {
		NTFSVolume volume('C');
		ASSERT_EQ(volume.getIndexRecordSize(), 4096);
		ASSERT_EQ(volume.getMFTRecordSize(), 1024);
	}
	catch (...) {
		FAIL();
	}
}

#ifndef LIGHT_TESTS
TEST(NTFSVolumeTest, ReadChnageJournal) {
	try {
		NTFSVolume volume('C');
		for (size_t i = 0; i < 5; ++i) {
			ChangeJournalRecordList changes = volume.readChangeJournal(0xffffffff);
			Sleep(1000 * 5);
		}

	}
	catch (...) {
		FAIL();
	}
}
#endif // LIGHT_TESTS