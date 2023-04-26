#include <gtest\gtest.h>
#include <vector>
#include <string>
#include <memory>

#include "..\NTFSLib\NTFSUtils.h"
#include "..\NTFSLib\Misc\NTFSLibError.h"

using std::vector;
using std::wstring;
using std::shared_ptr;

#define BUFFER_SIZE 256
#define MY_WIDE_STRING L"This is a wide String. Not a narrow one."
#define MY_GOOD_ENV_VARIABLE L"%SystemRoot%"
#define MY_BAD_ENV_VARIABLE L"%1337_42%"

// Splits a multi-byte path.
TEST(NTFSUtilsTest, RegularSplitWidePath) {
	try {
		vector<wstring> correct = { L"C:", L"Unicorns", L"And", L"Turtles" };
		wstring path = L"C:\\Unicorns\\And\\Turtles";
		vector<wstring> parts = NTFSUtils::splitWidePath(path);

		ASSERT_EQ(correct.size(), parts.size());
		for (size_t i = 0; i < parts.size(); ++i) {
			ASSERT_STREQ(parts[i].c_str(), correct[i].c_str());
		}
	}
	catch (...) {
		FAIL();
	}
}

// Splits a multi-byte path, with a trailing "\".
TEST(NTFSUtilsTest, TrailingSplitWidePath) {
	try {
		vector<wstring> correct = { L"C:", L"Unicorns", L"And", L"Turtles" };
		wstring path = L"C:\\Unicorns\\And\\Turtles\\";
		vector<wstring> parts = NTFSUtils::splitWidePath(path);

		ASSERT_EQ(correct.size(), parts.size());
		for (size_t i = 0; i < parts.size(); ++i) {
			ASSERT_STREQ(parts[i].c_str(), correct[i].c_str());
		}
	}
	catch (...) {
		FAIL();
	}
}

// Expands a system environment variable.
TEST(NTFSUtilsTest, ExpandEnvironmentVariable) {
	try {
		wstring expectedOutput = L"C:\\WINDOWS";
		wstring realOutput = NTFSUtils::expandEnvironmentVariable(MY_GOOD_ENV_VARIABLE);
		ASSERT_STREQ(realOutput.c_str(), expectedOutput.c_str());
	}
	catch (...) {
		FAIL();
	}
}