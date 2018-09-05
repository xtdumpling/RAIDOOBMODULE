//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.           **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header:  $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
// Revision History
// ----------------
// $Log: $
// 
//*************************************************************************
//<AMI_FHDR_START>
//
//  Name:  EXTRecovery.h
//
//  Description: EXT File system recovery data types definition
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef __EXT_RECOVERY__H__
#define __EXT_RECOVERY__H__
#ifdef __cplusplus
extern "C"
{
#endif

#pragma pack(push, 1)


// EXTx STRUCTURES
//
//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:        DIR_ENTRY_EXT
//
// Description: Structure of a EXT directory entry
//
// Referrals:
//
// Notes:       The directory entry length must be a multiple of 4 bytes
//              Padding will be added after the name if necessary.
//              Note that the directory does not contain any file info
//              other than the name. That info is in the inode for the file.
//
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct _DIR_ENTRY_EXT {
    UINT32 DIR_Inode;               // 0x00 - Inode no. of file
    UINT16 DIR_EntryLength;         // 0x04 - Length of dir. entry
    UINT8  DIR_NameLength;          // 0x06 - Length of name
    UINT8  DIR_FileType;            // 0x07 - Type of file
                                    //        1 = regular file, 2 = directory
    CHAR8  DIR_Name[1];             // 0x08 - File name starts here
} DIR_ENTRY_EXT;


//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:        VOLUME_SB
//
// Description: Structure of the Volume SuperBlock for EXT volumes.
//
// Referrals:
//
// Notes:       The Superblock is equivalent to the BPB of a FAT or NTFS
//              volume, containing information about the volume. However,
//              it does not contain any boot code.
//
//----------------------------------------------------------------------------
//<AMI_SHDR_END>

typedef struct  _VOLUME_SB {
    UINT32 SB_TotalInodes;      // 0x00 - Total Inodes
    UINT32 SB_TotalBlocks;      // 0x04 - Total size in blocks
    UINT32 SB_ReservedBlocks;   // 0x08 - Reserved blocks
    UINT32 SB_FreeBlocks;       // 0x0C - Free blocks
    UINT32 SB_FreeInodes;       // 0x10 - Free inodes
    UINT32 SB_FirstBlock;       // 0x14 - First data block (always 1 or 0)
    UINT32 SB_BlockSize;        // 0x18 - Block size shift value
                                //        The actual block size is calculated
                                //        as 1024 << SB_BlockSize
    UINT32 SB_FragSize;         // 0x0C - Fragment size
    UINT32 SB_BlocksPerGroup;   // 0x20 - Blocks per group
    UINT32 SB_FragsPerGroup;    // 0x24 - Fragments per group
    UINT32 SB_InodesPerGroup;   // 0x28 - Inodes per group
    UINT32 SB_LastMountTime;    // 0x2C - Time of last mount
    UINT32 SB_LastWriteTime;    // 0x30 - Time of last write
    UINT16 SB_MountCount;       // 0x34 - Count of mounts since verification
    UINT16 SB_MaxMountCount;    // 0x36 - Max mounts allowed before check
    UINT16 SB_Magic;            // 0x38 - Magic number (0xEF53)
    UINT16 SB_StatusFlag;       // 0x3A - 1 = clean, 2 = error
    UINT16 SB_ErrorDo;          // 0x3C - What to do if error
    UINT16 SB_MinorRev;         // 0x3E - Minor revision level
    UINT32 SB_LastCheckTime;    // 0x40
    UINT32 SB_CheckInterval;    // 0x44
    UINT32 SB_CreatorOS;        // 0x48 - OS that created the volume (0=Linux)
    UINT32 SB_Revision;         // 0x4C
    UINT16 SB_DefaultUID;       // 0x50 - Default User ID
    UINT16 SB_DefaultGID;       // 0x52 - Default Group ID
    UINT32 SB_FirstInode;       // 0x54 - First non-reserved inode
    UINT16 SB_InodeStrucSize;   // 0x58 - Inode structure size
    UINT16 SB_BlockGroupNo;     // 0x5A - Block Group Number
    UINT32 SB_CompFeatMap;      // 0x5C - Compatible feature bitmap
    UINT32 SB_IncompFeatMap;    // 0x60 - Incompatible feature bitmap
    UINT32 SB_ROCompFeatMap;    // 0x64 - Read-only comp. feature bitmap
                                //        If bit one of this is set, redundant
                                //        superblocks and group descriptors will
                                //        only be in groups that are a power of
                                //        3, 5, or 7.
    EFI_GUID SB_UUID;           // 0x68 - Volume Unique ID
    CHAR8  SB_VolumeName[16];   // 0x78 - Volume name
    CHAR8  SB_LastPath[64];     // 0x88 - Last mount path (normally not used)
    UINT32 SB_AlgoBitmap;       // 0xC8 - Compression algorithm bitmap
    UINT8  SB_PreAllocBlocks;   // 0xCC - Blocks to pre-allocate for new file
    UINT8  SB_PreAllocBlocksDir;// 0xCD - Blocks to pre-allocate for new directory
    UINT8  SB_Padding[512-0xCE];// 0xCE - Padding to 1024 starts here
                                //        We're loading this into an MBR buffer,
                                //        so we only pad to 512.
} VOLUME_SB;


//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:        VOLUME_BGDT
//
// Description: Structure of the Volume Block Group Descriptor Table.
//
// Referrals:
//
// Notes:       In EXT, volumes are arranged as groups of blocks. The size
//              of each group is defined in SB_BlocksPerGroup in the
//              Superblock. The descriptor table gives info about the group.
//
//----------------------------------------------------------------------------
//<AMI_SHDR_END>

typedef struct  _VOLUME_BGDT {
    UINT32 BGDT_BlockBitmapBlk; // 0x00 - Block bitmap block number
    UINT32 BGDT_InodeBitmapBlk; // 0x04 - Inode bitmap block number
    UINT32 BGDT_InodeTableBlk;  // 0x08 - Inode table block number
    UINT16 BGDT_FreeBlocks;     // 0x0C - Free blocks in the group
    UINT16 BGDT_FreeInodes;     // 0x0E - Free inodes in the group
    UINT16 BGDT_UsedDirs;       // 0x10 - Inodes used for directories in the group
    UINT16 BGDT_Padding;        // 0x12 - Padding
    UINT8  BGDT_Reserved[12];   // 0x14 - Reserved
} VOLUME_BGDT;


//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name:        VOLUME_IT
//
// Description: Structure of an Inode table
//
// Referrals:
//
// Notes:       The inode table table contains all the information about each
//              file EXCEPT the file's name, which is in the directory.
//              The directory contains the file's name and inode number.
//              The inodes are numbered sequentially in the table, and the
//              table occupies consecutive blocks.
//
//              EXT2, EXT3 File allocation method:
//              The block map of the file in IT_Blocks works as follows:
//              The first 12 entries are direct block numbers of the file.
//              The 13th entry is the number of a block containing block
//              numbers of the file (single-indirect blocks).
//              The 14th entry is the number of a block containing block
//              numbers of blocks containing file block numbers (double-
//              indirect blocks).
//              Similarly, the 15th entry is triple-indirect blocks.
//              Therefore, the maximum file size with 1024-byte blocks
//              would be 12+256+65536+16777216 1024-byte blocks.
//
//              EXT4 File allocation method:
//              Instead of a block map, EXT4 has a header followed by up
//              to 4 extents of contiguous blocks. If the entire file is
//              contiguous, there will be only one extent. Each extent
//              consists of a 32-bit logical block no. (relative to the
//              start of the file), a 16-bit  block count, and a 48-bit
//              physical block number, expressed as a 16-bit high value
//              and a 32-bit low value. If a file requires more than 4
//              extents, the value of EH_Depth in the header will be
//              non-zero, and the first extent will point to a block of
//              extents indtead of the file.
//
//              The file dates in VOLUME_IT are the number of seconds since
//              January 1, 1970.
//
//----------------------------------------------------------------------------
//<AMI_SHDR_END>

typedef struct _EXT_HDR {
    UINT16 EH_Magic;            // 0x00 - Magic number (0xF30A)
    UINT16 EH_Extents;          // 0x02 - Number of extents
    UINT16 EH_Max;              // 0x04 - Maximum no. of extents
    UINT16 EH_Depth;            // 0x06 - Depth of tree (0 normally)
    UINT32 EH_Generation;       // 0x08 - Generation ID (currently 0)
} EXT_HDR;
typedef struct _EXT_EXT {
    UINT32 EE_LogBlk;          // 0x00 - Logical block no.
    UINT16 EE_Length;          // 0x04 - Length of extent in blocks
    UINT16 EE_BlockHi;         // 0x06 - Uppper 16 bits of block no.
    UINT32 EE_BlockLo;         // 0x08 - Lower 32 bits of block no.
} EXT_EXT;

typedef struct _VOLUME_IT {
    UINT16 IT_Type;             // 0x00 - File type and access rights
                                //        0x8??? = regular file
                                //        0x4??? = directory
    UINT16 IT_UID;              // 0x02 - User ID
    UINT32 IT_SizeLo;           // 0x04 - File size (low 32 bits)
    UINT32 IT_ATime;            // 0x08 - Last access time
    UINT32 IT_CTime;            // 0x0C - File creation time
    UINT32 IT_MTime;            // 0x10 - File modified time
    UINT32 IT_DTime;            // 0x14 - File deleted time
    UINT16 IT_GID;              // 0x18 - Group ID
    UINT16 IT_Links;            // 0x1A - Links to this file (usually 1)
    UINT32 IT_ListSize;         // 0x1C - Block list size in 512-byte units.
    UINT32 IT_Flags;            // 0x20 - File flags
    UINT32 IT_OSD1;             // 0x24 - OS Dependant value no. 1
    union
    {
        struct
        {
        UINT32 Blocks[15];      // 0x28 - Blocks occupied by file (see notes)
        } Ext2;
        struct
        {
        EXT_HDR Header;         // 0x28 - Extent header
        EXT_EXT Extent[4];      // 0x34 - Extents of file (see notes)
        } Ext4;
    } Alloc;
    UINT32 IT_Version;          // 0x64 - File version
    UINT32 IT_FileACL;          // 0x68 - File Access Control List
    UINT32 IT_SizeHi;           // 0x6C - File size (high 32 bits)
    UINT32 IT_Fragment;         // 0x70 - Not used in Linux
    UINT32 IT_OSD2[3];          // 0x74 - OS Dependant value no. 2
} VOLUME_IT;


#pragma pack(pop)

VOID AmiGetFileListFromExtVolume(
    IN EFI_PEI_SERVICES 	**PeiServices,
    IN  UINT8               *Root,
    IN  UINT32              RootSize,
    OUT UINTN               *NumberOfFiles,
    OUT DIR_ENTRY_EXT       **Buffer
);


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**         (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
