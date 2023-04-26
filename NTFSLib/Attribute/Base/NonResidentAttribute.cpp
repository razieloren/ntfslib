#include <algorithm>

#include "NonResidentAttribute.h"
#include "..\..\Misc\NTFSLibError.h"

using std::min;

NonResidentAttribute::NonResidentAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute) :
	CommonAttribute(ntfsVolume, attribute),
	m_nonResident((PNONRESIDENT_ATTR_RECORD)attribute){
	// Parsing non-resident data runs. For an explanation about the data run structure, see DataRun structure in NTFSTypes.h
	PBYTE dataRunBuffer = (PBYTE)m_nonResident + m_nonResident->DataRunOffset;
	// VCN = FILE offset, LCN = DISK offset.
	ULONGLONG VCN = 0, LCN = 0;
	while (*dataRunBuffer) {
		DataRun dataRunEntry = { 0 };

		BYTE numOfLengthBytes = *dataRunBuffer & 0x0f;
		BYTE numOfOffsetBytes = *dataRunBuffer >> 4;

		memcpy(&dataRunEntry.NumOfClusters, dataRunBuffer + 1, numOfLengthBytes);
		// numOfOffsetBytes == 0 means no relative offset, which means a sparse data run (that is not actually saved on the disk).
		if (numOfOffsetBytes > 0) {
			dataRunEntry.IsSparse = false;
			LONGLONG currentLCN = 0;
			memcpy(&currentLCN, dataRunBuffer + numOfLengthBytes + 1, numOfOffsetBytes);
			// Figuring out if we're dealing with a scrambled data run (negative relative offset).
			if (currentLCN >> (numOfOffsetBytes * 8 - 1) == 1) {
				currentLCN ^= (DATA_RUN_EIGHT_BYTE_MASK << (numOfOffsetBytes * 8));
			}
			LCN += currentLCN;
			dataRunEntry.StartLCN = LCN;
		}
		else {
			dataRunEntry.StartLCN = 0;
			dataRunEntry.IsSparse = true;
		}
		dataRunEntry.StartVCN = VCN;
		VCN += dataRunEntry.NumOfClusters;
		m_dataRuns.push_back(dataRunEntry);

		dataRunBuffer += numOfLengthBytes + numOfOffsetBytes + 1;
	}

	NTFSLIB_ASSERT(
		!m_dataRuns.empty(),
		EmptyDataRunError
	);
}

void NonResidentAttribute::getData(PVOID buffer, ULONGLONG offset, DWORD length) {
	NTFSLIB_ASSERT(
		offset + length <= getDataSize(), 
		OutOfBoundsError
	);
	
	WORD clusterSize = m_ntfsVolume.getClusterSize();
	// First cluster we're going to read, relative to the file (NOT to the actual disk).
	ULONGLONG currentVC = offset / clusterSize;
	// By calculating the last cluster we're going like to read, we can figure out how many clusters we're going to read.
	ULONGLONG clustersLeftToRead = (offset + length + clusterSize - 1) / clusterSize - currentVC;
	// Initializing a buffer big enough for all the data we're going to read.
	Buffer fullBuffer((DWORD)clustersLeftToRead * clusterSize);
	size_t bufferIndex = 0;
	for (const DataRun& dataRun : m_dataRuns) {
		// If the current file-relative cluster is found between this data run, lets read from it.
		if (currentVC >= dataRun.StartVCN && currentVC < dataRun.StartVCN + dataRun.NumOfClusters) {
			// The difference between the current file cluster to the starting file cluster of this data run.
			ULONGLONG currentOffset = currentVC - dataRun.StartVCN;
			// Figuring out if this data run is enough for our needs, or we'll have more clusters to read afterwards.
			ULONGLONG clustersToRead = min<ULONGLONG>(clustersLeftToRead, dataRun.NumOfClusters - currentOffset);
			// Reading the clusters with the same relative offset we've already calculated.
			DWORD bytesRead = m_ntfsVolume.readClusters(fullBuffer.data() + bufferIndex, dataRun.StartLCN + currentOffset, (DWORD)clustersToRead, dataRun.IsSparse);
			NTFSLIB_ASSERT(
				bytesRead == clustersToRead * clusterSize,
				BadSizeError
			);
			clustersLeftToRead -= clustersToRead;
			if (clustersLeftToRead == 0) {
				break;
			}
			currentVC += clustersToRead;
			bufferIndex += bytesRead;
		}
	}
	NTFSLIB_ASSERT(
		clustersLeftToRead == 0, 
		BadSizeError
	);

	// We read full clusters, but the user asked for a specific offset and length.
	WORD startIndex = offset % clusterSize;
	memcpy(buffer, fullBuffer.data() + startIndex, length);
}

ULONGLONG NonResidentAttribute::getDataSize() const {
	return m_nonResident->DataSize;
}

PBYTE NonResidentAttribute::getDataPointer() const {
	// There is no really a "data pointer" for non-resident attributes. Their data is stored in data runs.
	NTFSLIB_ERROR(
		UnexpectedActionError, 
		NTFSLIB_DEFAULT_ERROR_CODE, 
		"No data pointer for NonResdientAttribute"
	);
}

