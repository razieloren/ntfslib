#ifndef _NTFSLIB_NTFS_TYPES_H
#define _NTFSLIB_NTFS_TYPES_H

#include <cstdint>
#include <vector>
#include <string>

#include "..\Misc\Win32\Win32.h"

using std::vector;
using std::wstring;

// b[*] indicates the number of bytes in the number.
typedef uint8_t b1;
typedef uint16_t b2;
typedef uint32_t b4;
typedef uint64_t b8;

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
// System defined data structures                                       //
//////////////////////////////////////////////////////////////////////////

/**
 * The standard NTFS_BOOT_SECTOR is found on sector 0 of the partition.
 * On NT4 and above, there is one backup copy of the boot sector
 * in the last sector of the partition. On NT3.51 and earlier,
 * the backup copy was located at sectors / 2, in other words,
 * the middle of the volume.
 */

/**
 * BIOS parameter block (BPB) structure.
 */
typedef struct {
	// Size of 1 sector in bytes.
	b2 BytesPerSector;
	// Size of 1 cluster in sectors.
	b1 SectorsPerCluster;
	// 0.
	b2 ReservedSectors;
	// 0.
	b1 Fats;
	// 0.
	b2 RootEntries;
	// 0.
	b2 Sectors;
	// 0xf8 = hard disk.
	b1 MediaDescriptor;
	// 0.
	b2 SectorsPerFat;
	// Irrelevant.
	b2 SectorsPerTrack;
	// Irrelevant.
	b2 NumberOfHeads;
	// 0.
	b4 HiddenSectors;
	// 0.
	b4 LargeSectors;
	// 0.
	b4 Unused;
	// Number of sectors in the volume.
	b8 TotalSectors;
	// Logical cluster number of the MFT.
	b8 MFTLCN;
	// Logical cluster number of the MFT mirror.
	b8 MFTMirrLCN;
	// Size of 1 MFT Record in clusters.
	b1 ClustersPerMFTRecord;
	// 0.
	b1 Reserved0[3];
	// Size of 1 Index Record in clusters.
	b1 ClustersPerIndexRecord;
	// 0.
	b1 Reserved1[3];
	// Volume's serial number.
	b8 VolumeSerialNumber;
	// Boot sector checksum.
	b4 Checksum;
} NTFS_BPB, *PNTFS_BPB;

/**
 * NTFS boot sector structure.
 */
typedef struct {
	// Jump instruction to boot code.
	b1 JumpInstruction[3];
	// NTFS volume magic.
	b8 OEMID;
	// See NTFS_BPB.
	NTFS_BPB BPB;
	// Boot up code.
	b1 Bootstrap[426];
	// End of sector marker, always 0xaaff, little endian.
	b2 EndOfSector;
} NTFS_BOOT_SECTOR, *PNTFS_BOOT_SECTOR;

/**
 * System files MFT record numbers. All these files
 * are always marked as used in the
 * $BITMAP attribute of the MFT.
 */
enum class NTFS_SYSTEM_FILES {
	// Master file table.
	FILE_MFT,
	// Master file table copy.
	FILE_MFTMirr,
	// Journaling log in data attribute.
	FILE_LogFile,
	// Volume name & information.
	FILE_Volume,
	// Attribute definitions.
	FILE_AttrDef,
	// Volume's root directory.
	FILE_Root,
	// Allocation bitmap of all clusters in the data attribute.
	FILE_Bitmap,
	// Boot sector in the data attribute.
	FILE_Boot,
	// Contains all the bad clusters in the non-resident data attribute.
	FILE_BadClus,
	// Shared security descriptors in data attribute.
	FILE_Secure,
	// Uppercase equivalents of all 65536 Unicode characters in data attribute.
	FILE_UpCase,
	// Directory containing other system files ($ObjId, $Quota, $Reparse & $UsnJournal), new to NTFS 3.0+.
	FILE_Extend,
	// Reserved for future use.
	FILE_Reserved12,
	FILE_Reserved13,
	FILE_Reserved14,
	FILE_Reserved15,
	// First USER file. used for limit tests.
	FILE_FirstUser
};

/**
 * Information about the current state of he MFT record.
 */
enum class MFT_RECORD_FLAGS {
	// MFT record is pointing to a directory, and not a file.
	MFT_RECORD_IS_DIRECTORY = 2,
	// MFT record is currently being used.
	MFT_RECORD_IN_USE = 1
};

/**
 * MFT references are used whenever a structure needs to refer to a record in the MFT.
 * A reference consists of a 48-bit index number in the MFT, and a 16-bit sequence number
 * used to detect stale references.
 * We'll define 2 macros for unpacking these values, MFT_REF and MFT_SEQNO.
 */
#define MFT_REF_MASK 0x0000ffffffffffffULL
#define MFT_REF(x) ((b8)((x) & MFT_REF_MASK))
#define MFT_SEQNO(x) ((b2)(((x) >> 48) & 0xffff))

/**
 * Defines a basic NTFS record.
 * The update sequence array (USA) is an array of WORD values
 * which belong to the end of each sector protected by it.
 * The first entry of the USA is the update sequence number (USN),
 * a cyclic counter of how many times the protected record has been
 * written to the disk. All the last WORD's of each sector must be equal
 * to the USN (during the reading process). The actual last WORD is saved
 * in the USA. In this way, NTFS can locate corrupted sectors.
 */
typedef struct {
	// Record's magic.
	b4 Magic;
	// Offset of the Update Sequence Array from the start of the NTFS record.
	b2 USAOffset;
	// Number of WORD-sized entries in the USA, including the Update Sequence Number (USN). Thus, the number of fix-ups is USACount - 1.
	b2 USACount;
} NTFS_RECORD, *PNTFS_RECORD;

/**
 * NTFS MFT record structure.
 * Contains basic info the the record's attributes.
 */
typedef struct {
	// See NTFS_RECORD.
	NTFS_RECORD RecordHeader;
	// $LogFile sequence number, Changes every time the record is modified.
	b8 LSN;
	// Number of times this record has been reused.
	b2 SequenceNumber;
	// Number of hard links - The number of directory entires referencing this record.
	b2 HardLinksCount;
	// Offset in bytes from the start of the record to the first attribute.
	b2 AttributeOffset;
	// Bit field of MFT_RECORD FLAGS.
	b2 Flags;
	// Actual number of bytes used by this file record.
	b4 BytesInUse;
	// Number of bytes allocated for this MFT record (Should be equal to MFT Record size).
	b4 BytesAllocated;
	// Reference to the base MFT Record which this record belongs (0 for base MFT Records).
	b8 BaseFileRecord;
	// The instance number that will be assigned to the next attribute added to this MFT Record.
	b2 NextAttributeInstance;
	// Reserved / Alignment.
	b2 Alignment;
	// Number of this MFT Record.
	b4 RecordNumber;
} MFT_RECORD, *PMFT_RECORD;

/**
 * System defined attributes, each attribute type
 * has a corresponding attribute name.
 */
enum class ATTR_TYPE {
	AT_UNUSED = 0x0,
	AT_STANDARD_INFORMATION = 0x10,
	AT_ATTRIBUTE_LIST = 0x20,
	AT_FILE_NAME = 0x30,
	AT_OBJECT_ID = 0x40,
	AT_SECURITY_DESCRIPTOR = 0x50,
	AT_VOLUME_NAME = 0x60,
	AT_VOLUME_INFORMATION = 0x70,
	AT_DATA = 0x80,
	AT_INDEX_ROOT = 0x90,
	AT_INDEX_ALLOCATION = 0xa0,
	AT_BITMAP = 0xb0,
	AT_REPARSE_POINT = 0xc0,
	AT_EA_INFORMATION = 0xd0,
	AT_EA = 0xe0,
	AT_PROPERTY_SET = 0xf0,
	AT_LOGGED_UTILITY_STREAM = 0x100,
	AT_FIRST_USER_DEFINED_ATTRIBUTE	= 0x1000,
};
#define END_OF_ATTRIBUTES 0xffffffff

/**
 * Attribute flags.
 */
enum class ATTR_FLAGS {
	ATTR_IS_COMPRESSED = 0x1,
	ATTR_IS_ENCRYPTED = 0x4000,
	ATTR_IS_SPARSE = 0x8000
};

typedef struct {
	// Type of the attribute.
	b4 Type;
	// byte size of the resident part of the attribute, used to get to the next attribute.
	b4 Length;
	// If 0 - Attribute is resident, if 1 - Attribute is non-resident.
	b1 NonResident;
	// Unicode character size of the attribute name, 0 id unnamed.
	b1 NameLength;
	// Byte offset to the beginning of the name, from the beginning of the attribute.
	b2 NameOffset;
	// Bit field of ATTR_FLAGS.
	b2 Flags;
	// The instance of this attribute. Unique within the file record.
	b2 Instance;
} COMMON_ATTR_RECORD, *PCOMMON_ATTR_RECORD;

typedef struct {
	// See COMMON_ATTR_RECORD.
	COMMON_ATTR_RECORD CommonRecord;
	/* Resident attributes */
	// Byte size of the attribute value.
	b4 ValueLength;
	// Byte offset to the attribute value, from the beginning of the attribute.
	b2 ValueOffset;
	// Irrelevant for parsing (only for modifying).
	b1 IndexedFlag;
	// Reserved / Alignment.
	b1 Alignment;
} RESIDENT_ATTR_RECORD, *PRESIDENT_ATTR_RECORD;

typedef struct {
	// See COMMON_ATTR_RECORD.
	COMMON_ATTR_RECORD CommonRecord;
	/* Non-resident attributes */
	// Lowest valid virtual cluster number for this portion of the attribute value or 0 if this is the only extent.
	b8 LowestLCN;
	// Highest valid virtual cluster number of this extent of the attribute value. Usually there is only one portion, so it mostly equals to the attribute value size in clusters minus 1.
	b8 HighestVCN;
	// Byte offset to the data runs from the beginning of the attribute.
	b2 DataRunOffset;
	// Compression unit. 0 - no compression, WinNT4 only uses 4.
	b2 CompressionUnit;
	// Alignment.
	b4 Alignment;
	// Bytes size of the disk space allocated to hold the attribute. Always a multiple of cluster.
	b8 AllocatedSize;
	// Byte size of the attribute value. Can be larger than the allocated size if the attribute is compressed or sparse.
	b8 DataSize;
	// byte size of the initialized portion of the attribute value, usually equals to DataSize.
	b8 InitializedSize;		
} NONRESIDENT_ATTR_RECORD, *PNONRESIDENT_ATTR_RECORD;


/**
 * File attribute flags.
 */
enum class FILE_ATTR {
	ATTR_READ_ONLY = 0x1,
	ATTR_HIDDEN = 0x2,
	ATTR_SYSTEM = 0x4,
	ATTR_ARCHIVE = 0x20,
	ATTR_DEVICE = 0x40,
	ATTR_NORMAL = 0x80,
	ATTR_TEMP = 0x100,
	ATTR_SPARSE = 0x200,
	ATTR_REPARSE = 0x400,
	ATTR_COMPRESSED = 0x800,
	ATTR_OFFLINE = 0x1000,
	ATTR_NCI = 0x2000,
	ATTR_ENCRYPTED = 0x4000
};

/**
 * Times in NTFS: All times are in MS standard time format.
 * i.e. The number of 100-nanoseconds intervals since 1st January 1601, 00:00:00 UTC.
 */

/**
 * Attribute: Standard Information (0x10)
 * Always resident, present in all base file records on a volume.
 */
typedef struct {
	// Creation time of the file. Updated when filename is changed.
	b8 CreationTime;
	// Last modification time of $DATA attribute.
	b8 LastDataChangeTime;
	// Last modification time of this File record.
	b8 LastMFTChangeTime;
	// Approximate time this file was last accessed. This can be disabled.
	b8 LastAccessTime;
	// Bit field of FILE_ATTR.
	b4 Flags;
	// Maximum allowed versions for the file, 0 if version numbering is disabled.
	b4 MaximumVersions;
	// This file's version (if any), this is 0 if MaximumVersions is 0.
	b4 VersionNumber;
	// Class ID?
	b4 ClassId;
	// ID of the user owning the file. 0 if quota is disabled. Can be translated with $Q index in FileExtend/$Quota.
	b4 OwnerId;
	// Security ID which can be translated to security descriptor using $SDS data stream.
	b4 SecurityId;
	// Size in bytes of the charge to the quota for all streams, 0 if quota is disabled.
	b8 QuotaCharged;
	// Last update sequence number of the file, this is a direct index into the transaction log file ($UsnJournal). 0 if USN journal is disabled.
	b8 USN;
} STANDARD_INFORMATION, *PSTANDARD_INFORMATION;

/**
* Attribute: Attribute List (0x20)
* Either resident or non-resident, value consists of variable length ATTR_LIST_ENTRY.
* The list is not terminated by anything. We know we reached the end by comparing
* the current offset to the attribute size.
*/
typedef struct {
	// Type of referenced attribute.
	b4 AttributeType;
	// Size in bytes of this entry.
	b2 RecordSize;
	// Size of Unicode characters of the name, 0 if unnamed.
	b1 NameLength;
	// Byte offset from the beginning of the attribute the the attribute name.
	b1 NameOffset;
	// Lowest virtual cluster number of this portion of the attribute value, usually 0.
	b8 LowestVCN;
	// Reference to the File Record holding the Attribute Record.
	b8 MFTReference;
	// If LowestVCN = 0, this is the instance of the attribute being referenced, otherwise, it's 0.
	b2 Instance;
} ATTRIBUTE_LIST, *PATTRIBUTE_LIST;

/**
 * Possible name-spaces for filenames in NTFS.
 */
enum class FILE_NAME_NAMESPACE {
	// Case-sensitive and allows all Unicode characters except: '\0', '/'.
	NAMESPACE_POSIX,
	// Standard WinNT/2K NTFS long filenames. Case insensitive, accepts all Unicode characters except: '\0', '"', '*', '/', ':', '<', '>', '?', '\', and '|', cannot end with a '.' or a space.
	NAMESPACE_WIN32,
	// Standard DOS filenames. Uppercase only, accepts all ASCII characters except: '"', '*', '+', ',', '/', ':', ';', '<', '=', '>', '?', and '\'
	NAMESPACE_DOS,
	// Means that both Win32 and DOS filenames are identical and saved a single filename record.
	NAMESPACE_WIN32_AND_DOS
};

/**
* Attribute: Filename (0x30)
* Always resident, all fields expect ParentMFTReference are only updates
* when the file name is changed. until then, they become out of sync with reality.
*/
typedef struct {
	// Directory this filename is referenced from.
	b8 ParentMFTReference;
	// Time was the file was created - updated only on filename change, might be out of sync.
	b8 CreationTime;
	// Last modification time of $DATA attribute - updated only on filename change, might be out of sync.
	b8 LastDataChangeTime;
	// Last modification time of this File Record - updated only on filename change, might be out of sync.
	b8 LastMFTChangeTime;
	// Approximate time this file was last accessed. This can be disabled - updated only on filename change, might be out of sync.
	b8 LastAccessTime;
	// Allocated size in bytes for the unnamed $DATA attribute. For a directory, this is always 0. NOTE: this is a multiple of the cluster size.
	b8 AllocatedSize;
	// Actual size in bytes of the unnamed $DATA attribute, for a directory, it's always 0.
	b8 RealSize;
	// Flags describing the file.
	b4 Flags;
	// Used for extended attributes and re-parse point.
	b4 ER;
	// Length of filename in Unicode characters.
	b1 NameLength;
	// Namespace of the filename.
	b1 Namespace;
	// (pointer to the) Filename in Unicode.
	b2 Name[1];
} FILE_NAME, *PFILE_NAME;

/**
 * Possible flags for volume.
 */
enum class VOLUME_FLAGS {
	VOLUME_DIRTY = 0x1,
	VOLUME_RESIZE_LOG_FILE = 0x2,
	VOLUME_UPGRADE_ON_MOUNT = 0x4,
	VOLUME_MOUNTED_ON_NT4 = 0x8,
	VOLUME_DELETE_USN_UNDERWAY = 0x10,
	VOLUME_REPAIR_OBJECT_ID = 0x20,
	VOLUME_CHKDSK_UNDERWAY = 0x4000,
	VOLUME_MODIFIED_BY_CHKDSK = 0x8000
};

typedef struct {
	// Reserved.
	b8 Reserved;
	// Major version of the NTFS format.
	b1 MajorVersion;
	// Minor version of the NTFS format.
	b1 MinorVersion;
	// Bit field of VOLUME_FLAGS.
	b2 Flags;
} VOLUME_INFORMATION, *PVOLUME_INFORMATION;

/**
 * Index header flags.
 */
enum class INDEX_HEADER_FLAGS {
	/* When index header is in an index root attribute */
	// Index is small enough to fit inside the index root attribute, and there's no need for index allocation attribute.
	SMALL_INDEX	= 0,
	// The index is too large to fit in the index root attribute, and there's a need for index allocation attribute.
	LARGE_INDEX	= 1,

	/* When index header is in an index block (part of index allocation attribute) */
	// There are no more nodes branching from this node.
	LEAF_NODE = 0,
	// This node indexes other nodes.
	INDEX_NODE = 0
};

/**
 * This is the header for indexes, describing the INDEXD_ENTRY records,
 * which follow the INDEX_HEADER.
 * Important: All the offsets are relative to the INDEX_HEADER, and not
 * to the entries.
 */
typedef struct {
	// offset in bytes to the first INDEX_ENTRY.
	b4 EntriesOffset;
	// Total size in bytes of this index.
	b4 IndexLength;
	// Size in bytes of this index, multiple of 8 bytes.
	b4 AllocatedSize;
	// Bit field of INDEX_HEADER_FLAGS.
	b1 Flags;
	// Reserved / Alignment.
	b1 Reserved[3];
} INDEX_HEADER, *PINDEX_HEADER;

/**
* Attribute: Index root (0x90)
* Always resident, this is followed by a sequence of index entries (INDEX_ENTRY structures)
* as described by the index header.
* Note: The root directory (File_Root) contains an entry for itself, other
* directories do not contains entries for themselves. 
*/
typedef struct {
	// Type of indexed attribute, $FILE_NAME for directories, 0 for view indexes.
	b4 AttributeType;
	// Collation rule used to sort the index entries.
	b4 CollationRule;
	// Size of each index block in bytes (in the index allocation attribute).
	b4 IndexBlockSize;
	// Cluster size of each index block (in index allocation attribute).
	b1 ClusterPerIndexBlock;
	// Reserved / Alignment.
	b1 Reserved[3];
	// Index header describing the following entries.
	INDEX_HEADER Index;
} INDEX_ROOT, *PINDEX_ROOT;

/**
* Attribute: Index allocation (0xa0)
* Always non-resident. This is an array of index blocks. Each index block starts
* with an INDEX_BLOCK structure containing an index header, followed by a sequence
* of index entries (INDEX_ENTRY)
*/
typedef struct {
	// See NTFS_RECORD.
	NTFS_RECORD RecordHeader;
	// $LogFile sequence number of the last modification of this index block.
	b8 LSN;
	// Virtual cluster number of the index record.
	b8 VCN;
	// Describes the following index entries.
	INDEX_HEADER Index;
} INDEX_RECORD, *PINDEX_RECORD;

/**
 * Index entry flags.
 */
enum class INDEX_ENTRY_FLAGS {
	// This entry contains a sub node (reference to an index block in form of a virtual cluster number).
	INDEX_ENTRY_NODE = 1,
	// This signifies the last entry of an index block. This entry does no represent a file, but it can point to a sub node.
	INDEX_ENTRY_END = 2
};

/**
 * Index entry flags.
 */
typedef struct {
	// MFT record referenced by this entry.
	b8 MFTReference;
	// Size in bytes of this entry.
	b2 Size;
	// Size in bytes of the stream.
	b2 StreamSize;
	// Bit field of INDEX_ENTRY_FLAGS flags.
	b1 Flags;
	// Reserved / Alignment.
	b1 Reserved[3];
	// Entry's stream (only present when the last entry flag is NOT set).
	b1 Stream[1];
	// Size - 8 -> VCN of sub node. Present only when sub node flag is set.
} INDEX_ENTRY, *PINDEX_ENTRY;

#pragma pack()

//////////////////////////////////////////////////////////////////////////
// User defined data structures                                         //
//////////////////////////////////////////////////////////////////////////

/**
 * A data run is a chunk of data of a non-resident attribute.
 * Each data run header contains:
 * First byte:
 *		- Most significant nibble: Number of bytes describing the LCN (logical cluster number) offset.
 *		- Least significant nibble: Number of bytes describing the number of clusters.
 * Sparse runs (chunks of 0's) will not have an offset (since they are not really saved on the disk), and
 * scrambled runs might contain negative offsets.
 */
typedef struct {
	bool IsSparse;
	LONGLONG StartLCN;
	ULONGLONG StartVCN;
	ULONGLONG NumOfClusters;
} DataRun;
typedef vector<DataRun> DataRunList;
#define DATA_RUN_EIGHT_BYTE_MASK 0xffffffffffffffff

/**
* Available media descriptors.
*/
enum class VOLUME_TYPE {
	OTHER = 0,
	FLOPPY = 0xf0,
	FIXED = 0xf8
};

/**
* Stores current volume's properties.
*/
typedef struct {
	BYTE MediaDescriptor;
	BYTE SectorsPerCluster;
	BYTE ClustersPerMFTRecord;
	BYTE ClustersPerIndexRecord;
	WORD SectorSize;
	WORD ClusterSize;
	WORD MFTRecordSize;
	WORD IndexRecordSize;
	ULONGLONG SerialNumber;
	ULONGLONG TotalClusters;
	ULONGLONG TotalSectors;
	ULONGLONG MFTAddr;
	ULONGLONG MFTLCN;
	ULONGLONG MFTMirrLCN;
} VolumeProperties;

/**
 * Stores basic volume information.
 */
typedef struct {
	BYTE MajorVersion;
	BYTE MinorVersion;
	ULONGLONG SerialNumber;
	wstring Name;
} VolumeAttributes;

#endif // _NTFSLIB_NTFS_TYPES_H
