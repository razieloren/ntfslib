#include <gtest\gtest.h>

#include "..\..\NTFSLib\Misc\Win32\VolumeFile.h"
#include "..\..\NTFSLib\NTFSLib.h"

// Opening a valid volume.
TEST(Win32VolumeFileTest, GoodVolumeFileConstructor) {
	try {
		VolumeFile volume(L'C');
	}
	catch (...) {
		FAIL();
	}
}

// Opening an invalid volume letter.
TEST(Win32VolumeFileTest, BadCharacterVolumeFileConstructor) {
	try {
		VolumeFile volume(L'3');
		FAIL();
	}
	catch (BadVolumeCharacterError&) {
		 // Good!
	}
	catch (...) {
		FAIL();
	}
}

// opening a valid volume character, but it does not exist on this machine.
TEST(Win32VolumeFileTest, BadPathVolumeFileConstructor) {
	try {
		VolumeFile volume(L'O');
		FAIL();
	}
	catch (Win32Error&) {
		// Good!
	}
	catch (...) {
		FAIL();
	}
}