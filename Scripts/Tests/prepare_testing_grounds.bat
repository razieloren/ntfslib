@echo OFF
rem Preparing testing grounds for NTFSLib.
rem Usage: prepare_testing_grounds.bat
rem This script was not set as "*-Build Script" for the testing projects to reduce build time, just run it once, manually.

md C:\NTFSLibTestingGrounds

rem For Utils tests.
IF DEFINED SystemRoot (ECHO SystemRoot is defined.) ELSE (SETX SystemRoot C:\WINDOWS /M)

rem For NTFSParser tests.
echo Setting C: volume name to "Destiny"
label C: Destiny
echo Creating testing folders
md "C:\NTFSLibTestingGrounds\LongDirectoryName\Inner Directory"
md "C:\NTFSLibTestingGrounds\LongDirectoryName\Empty Folder"
md "C:\NTFSLibTestingGrounds\Dumps"
echo Creating big file
python -c "f = open(\"C:\\NTFSLibTestingGrounds\\Dumps\\BigFile.bin\", \"w\"); f.write(\"\".join([i % 10 for i in 1024 * 1024 * 256])); f.close()"
echo Creating Other test files
echo 424242424242 > "C:\NTFSLibTestingGrounds\42.txt"
echo 133713371337 > "C:\NTFSLibTestingGrounds\1337.txt"
echo Awesome Main Stream > "C:\NTFSLibTestingGrounds\Streams.txt"
echo Super Stream 1 > "C:\NTFSLibTestingGrounds\Streams.txt:stream1"
echo Super Stream 2 > "C:\NTFSLibTestingGrounds\Streams.txt:stream2"
echo Power Level > "C:\NTFSLibTestingGrounds\Streams.txt:stream9001"
echo Garen > "C:\NTFSLibTestingGrounds\Hidden.txt"
attrib +H "C:\NTFSLibTestingGrounds\Hidden.txt"
echo Root > "C:\NTFSLibTestingGrounds\System.txt"
attrib +S "C:\NTFSLibTestingGrounds\System.txt"
echo Edit me > "C:\NTFSLibTestingGrounds\ReadOnly.txt"
attrib +R "C:\NTFSLibTestingGrounds\ReadOnly.txt"
echo Archmage > "C:\NTFSLibTestingGrounds\NotArchived.txt"
attrib -A "C:\NTFSLibTestingGrounds\NotArchived.txt"
echo Done
