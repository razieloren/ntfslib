#include <gtest\gtest.h>
#include <iostream>

#include "..\NTFSLib\Misc\Defs.h"

using std::cout;
using std::endl;

GTEST_API_ int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);

#ifdef LIGHT_TESTS
	cout << "*** Heavy tests are disabled for this run! ***" << endl;
#endif // LIGHT_TESTS
	MemoryCheckpoint memoryCheckpoint = { 0 };
	MEM_CHECKPOINT(memoryCheckpoint);
	int returnValue = RUN_ALL_TESTS();
	// Expecting 1 pointer to leak (gtest's UnitTest instance).
	MEM_LEAK_CHECK_FROM_CHECKPOINT(memoryCheckpoint);
	return returnValue;
}