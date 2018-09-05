//*************************************************************************
//*************************************************************************
//**                                                                     **
//**         (C)Copyright 1985-2014, American Megatrends, Inc.           **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file FsRecovery.h
    File system recovery data types definition

**/

#ifndef __FS_RECOVERY__H__
#define __FS_RECOVERY__H__
#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------

#include <Ppi/BlockIo.h>

#include "NtfsRecovery.h"
#include "ExtRecovery.h"
#include "ExFatRecovery.h"

//----------------------------------------------------------------------

#define FAT12 0
#define FAT16 1
#define FAT32 2
#define NTFS  3
#define EXTx  4
#define exFAT 5

#define     ATTR_VOLUME_ID              0x08
#define     ATTR_DIRECTORY              0x10

#pragma pack(push, 1)

//
// GUID Partition structures
//
typedef struct _GUID_PARTITION
{
    UINT8  TypeGuid[16];
    UINT8  UniqueGuid[16];
    UINT64 FirstLba;
    UINT64 LastLba;
    UINT64 Flags;
    CHAR16 Name[36];
} GUID_PARTITION;

typedef struct _GUID_BOOT_RECORD
{
    GUID_PARTITION GuidPart[4];
} GUID_BOOT_RECORD;

typedef struct _GUID_TABLE_HEADER
{
    UINT8  Signature[8];
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 HeaderCRC;
    UINT32 Reserved;
    UINT64 CurrentLBA;
    UINT64 BackupLBA;
    UINT64 FirstLBA;
    UINT64 LastLBA;
    UINT8  DiskGuid[16];
    UINT64 StartLBA;
    UINT32 EntryCount;
    UINT32 EntrySize;
    UINT32 ArraySize;
} GUID_TABLE_HEADER;

typedef struct
{
    CHAR8  FileName[11];
    UINT8  DirAttr;
    UINT8  Unused[2];        //NTRes, CrtTimeTenth;
    UINT16 Unused1[3];       //CrtTime, CrtDate, LstAccDate;
    UINT16 FirstClusterHi;
    UINT16 Unused2[2];       //WrtTime, WrtDate;
    UINT16 FirstClusterLo;
    UINT32 FileSize;
} DIR_ENTRY;

typedef struct
{
    UINT8  jmp[3];               // 0
    CHAR8  OEMName[8];           // 3
    UINT16 BytsPerSec;           // 11
    UINT8  SecPerClus;           // 13
    UINT16 RsvdSecCnt;           // 14
    UINT8  NumFATs;              // 16
    UINT16 RootEntCnt;           // 17
    UINT16 TotSec16;             // 19
    UINT8  Media;                // 21
    UINT16 FATSz16;              // 22
    UINT16 SecPerTrk;            // 24
    UINT16 NumHeads;             // 26
    UINT32 HiddSec;              // 28
    UINT32 TotSec32;             // 32
    union
    {
        struct
        {
            UINT8 Padding[510 - 36];
        } Fat16;
        struct
        {
            UINT32 FATSz32;      // 36
            UINT8  Padding1[4];  // 40
            UINT32 RootClus;     // 44
            UINT8  Padding2[510 - 48];
        } Fat32;
        struct
        {
            UINT32 FATSz32u;    // 36
            UINT64 TotSec64;    // 40
            UINT64 MFTClus;     // 48
            UINT8  Padding3[510 - 56];
        } Ntfs;
    } Fat;
    UINT16 Signature;
} BOOT_SECTOR;

typedef struct _RC_VOL_INFO
{
    UINT32                        BlockSize;
    EFI_PEI_RECOVERY_BLOCK_IO_PPI *BlkIo;
    UINTN                         Device;
    UINT8                         FatType;
    UINT8                         *FatPointer;
    DIR_ENTRY                     *RootPointer;
    UINT64                        PartitionOffset;
    UINT64                        FatOffset;
    UINT64                        RootOffset;
    UINT64                        DataOffset;
    UINT32                        BytesPerCluster;
} RC_VOL_INFO;

typedef struct
{
    UINT8  Length, ExtAttrLength;   // 1,2
    UINT32 ExtentOffset, Unused1;   // 3..10
    UINT32 DataLength, Unused2;     // 11..18
    UINT8  DataTime[7];             // 19..25
    UINT8  Flags;                   // 26
    UINT8  Unused3[32 - 26];        // 27..32
    UINT8  LengthOfFileId;          // 33
} DIR_RECORD;

typedef struct
{
    UINT8      Type;                // 1
    CHAR8      StandardId[5];       // 2..6
    UINT8      Version;             //7
    UINT8      Unused1[156 - 7];    //8..156
    DIR_RECORD RootDir;             //157..190
    UINT8      Padding;
    CHAR8      Unused2[2048 - 190]; //191..2048
} PRIMARY_VOLUME_DESCRIPTOR;

typedef struct
{
    UINT8  BootRecordIndicator;
    CHAR8  ISO9660Identifier[0x5];
    UINT8  DescriptorVersion;
    CHAR8  BootSystemIdentifier[0x20];
    CHAR8  Unused0[0x20];
    UINT32 BootCatalogFirstSector;
    UINT8  Unused1[0x7B6];
} BOOT_RECORD_VOLUME_DESCRIPTOR;

typedef struct
{
    UINT8  HeaderID;
    UINT8  PlatformID;
    UINT8  Reserved0[0x2];
    CHAR8  ManufacturerID[0x18];
    UINT16 Checksum;
    UINT16 Key;
} VALIDATION_ENTRY;

typedef struct
{
    UINT8  BootIndicator;
    UINT8  BootMediaType;
    UINT16 LoadSegment;
    UINT8  SystemType;
    UINT8  Unused0;
    UINT16 SectorCount;
    UINT32 LoadRBA;
    UINT8  Unused1[0x14];
} INITIAL_DEFAULT_ENTRY;

typedef struct _MBR_PARTITION
{
    UINT8  BootIndicator;
    UINT8  StartHead;
    UINT8  StartSector;
    UINT8  StartTrack;
    UINT8  OSType;
    UINT8  EndHead;
    UINT8  EndSector;
    UINT8  EndTrack;
    UINT32 StartingLba;
    UINT32 SizeInLba;
} MBR_PARTITION;

typedef struct _MASTER_BOOT_RECORD
{
    UINT8         BootCode[440];
    UINT32        UniqueMbrSig;
    UINT16        Unknown;
    MBR_PARTITION PartRec[4];
    UINT16        Sig;
} MASTER_BOOT_RECORD;

typedef struct _MEMORY_BLOCK {
    MASTER_BOOT_RECORD  Mbr;                // Master boot record (all file systems)
    BOOT_SECTOR         Bs;                 // Boot sector (FAT and NTFS)
    UINT8               MFTRunList[256];    // Master File Table run list (NTFS)
    UINT8               RootRunList[128];   // Root dir run list (NTFS)
    UINT8               ResidentIndex[256]; // Stores resident index (NTFS)
    UINT8               AttributeList[256]; // Stores attribute list
    VOLUME_SB           Sb;                 // Superblock (EXT(n))
    VOLUME_IT           RootInode;          // Root directory inode (EXT(n))
    VOLUME_IT           FileInode;          // File inode (EXT(n))
} MEMORY_BLOCK;

#pragma pack(pop)

//**************** eLink override definitions *******************************
//This functions may be overriden by eLinks

typedef 
EFI_STATUS 
(OEM_RECOVERY_VOLUME_INTEGRITY_CALLBACK)(
  IN  UINT8    PartCount,
  IN  BOOLEAN  IsMbr,
  IN  UINT8    RecoveryDeviceType,
  IN  MEMORY_BLOCK *MemBlk 
);

typedef  
EFI_STATUS 
(FsRecovery_Devices) (
    IN EFI_PEI_SERVICES **PeiServices,
    IN OUT RC_VOL_INFO  *Volume,
    IN OUT UINTN        *FileSize,
    OUT VOID            *Buffer 
);

VOID 
GetFileListFromPrimaryVolume (
    IN  DIR_RECORD          *Root,
    IN  UINT32              RootSize,
    OUT UINTN               *NumberOfFiles,
    OUT DIR_RECORD          **Buffer
);

VOID 
GetFileListFromFatVolume (
    IN  DIR_ENTRY            *Root,
    IN  UINT32               RootEntries,
    OUT UINTN                *NumberOfFiles,
    OUT DIR_ENTRY            **Buffer
);

BOOLEAN 
AmiIsValidFile (
    IN VOID   *FileData,
    IN UINTN  FileSize
);

EFI_STATUS 
FindNextPartition (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume 
);

EFI_STATUS 
GetRecoveryFileInfo (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT VOID          **pCapsuleName,
    IN OUT UINTN         *pCapsuleSize,
    OUT    BOOLEAN       *ExtendedVerification
);

EFI_STATUS 
FileSearch (
    IN CHAR8    *RecoveryFileName, 
    IN CHAR8    *FsFilename, 
    IN BOOLEAN  IgnoreSpacesInFilename,
    IN UINT32   FileNameLength 
);

EFI_STATUS 
ReadDevice (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN RC_VOL_INFO       *Volume,
    IN UINT64            Start,
    IN UINTN             Size,
    OUT VOID             *Buffer 
);

UINT32 
GetClustersCount (
    IN UINT8   FatType,
    IN UINT32  CurrentCluster,
    OUT UINT32 *NextCluster,
    IN BOOLEAN Continuous 
);

EFI_STATUS 
InitFat (
    IN EFI_PEI_SERVICES **PeiServices,
    IN RC_VOL_INFO      *Volume,
    IN UINT32           FatSize 
);

VOID 
AmiGetFileListFromPrimaryVolume (
    IN  DIR_RECORD          *Root,
    IN  UINT32              RootSize,
    OUT UINTN               *NumberOfFiles,
    OUT DIR_RECORD          **Buffer
);

VOID 
AmiGetFileListFromFatVolume (
    IN  DIR_ENTRY           *Root,
    IN  UINT32              RootEntries,
    OUT UINTN               *NumberOfFiles,
    OUT DIR_ENTRY           **Buffer
);

BOOLEAN 
FileCompare (
    IN CHAR8 *RecoveryFileNamePattern, 
    IN CHAR8 *FsFilename, 
    IN BOOLEAN IgnoreSpacesInFilename,
    IN UINT32 FileNameLength
);

#if SEARCH_PATH
VOID 
AddRecoveryPath (
    VOID IN OUT  **FileName 
);

BOOLEAN 
IsolateName (
    IN  CHAR8       **FilePath,
    OUT CHAR8       **NextName
);
#endif

VOID ReportCapsuleSize (
        EFI_PEI_SERVICES **PeiServices,
        UINT32 Size
        );

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**         (C)Copyright 1985-2014, American Megatrends, Inc.           **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
