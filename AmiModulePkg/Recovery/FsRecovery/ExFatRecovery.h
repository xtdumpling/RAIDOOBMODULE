//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.           **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************


/** @file ExFatRecovery.h
    EXT File system recovery data types definition

**/

#ifndef __EXFAT_RECOVERY__H__
#define __EXFAT_RECOVERY__H__
#ifdef __cplusplus
extern "C"
{
#endif

#pragma pack(push, 1)

//
// EXTFAT STRUCTURES
//

/**
    Structure of exFAT boot sector

**/
typedef struct
{
    UINT8  BS_Jmp[3];               // 0 
    CHAR8  BS_OEMName[8];           // 3   Name is always "EXFAT   "
    UINT8  BS_MBZ[53];              // 11  Must be zero
    UINT64 BS_PartOff;              // 64  Offset to partition (sector)
    UINT64 BS_VolSize;              // 72  Volume size in sectors
    UINT32 BS_FATOff;               // 80  Offset to FAT (sector)
    UINT32 BS_FATSize;              // 84  FAT size in sectors
    UINT32 BS_DataOff;              // 88  Offset to data (sector)
    UINT32 BS_DataSize;             // 92  Data size in clusters
    UINT32 BS_RootCluster;          // 96  Root dir. first cluster
    UINT32 BS_VSN;                  // 100 Volume Serial Number
    UINT16 BS_Revision;             // 104 File system revision
    UINT16 BS_VolFlags;             // 106 Volume flags
    UINT8  BS_BPS;                  // 108 Bytes Per Sector (power of 2)
    UINT8  BS_SPC;                  // 109 Sectors Per Cluster (power of 2)
    UINT8  BS_NumFAT;               // 110 Number of FATs
    UINT8  BS_DrvSel;               // 111 Drive Select (used by Int13)
    UINT8  BS_DataPct;              // 112 Percent of Data space in use
    UINT8  BS_Reserved[7];          // 113 Reserved
    UINT8  BS_BootCode[390];        // 120 Boot Code
    UINT16 BS_Signature;            // 510 Signature (0xAA55)
} BOOT_SECTOR_EX;

/**
    Structure of exFAT directory entry

    @note  
        A directory entry in exFAT consists of at least 3 32-bit sections.
        The first section always begins with 0x85, and contains the file
        dates and the number of "extensions" (additional 32-bit sections).
        The next section begins with 0xC0, and contains the file location and size.
        The next section(s) begin with 0xC1, and contain the file name in
        unicode, in 15-character pieces.
        There are also special entries for the volume name, bitmap table,
        and "upcase" table.

**/

typedef struct
{
    UINT8  DE_Type;                 // 0   Entry type flags
    // The bites of this byte are defined as follows:
    // Bits 0-4 - Type (1 = Allocation Bitmap table entry)
    //                 (2 = UpCase table entry)
    //                 (3 = Volume name entry)
    //                 (5 = Normal file entry)
    //  Secondary types (0 = File info (location and size))
    //                  (1 = File name)
    //  Bit 5 - Importance (0 = Can be ignored)
    //  Bit 6 - Category (0 = Primanry, 1 = Secondary)
    //  Bit 7 - In Use (0 = Erased, 1 = In use)
    //  Taken together, these bits make...
    //  0x81 = Bitmap table entry
    //  0x82 = UpCase table entry
    //  0x83 = Volume name entry
    //  0x85 = Normal file entry
    //  0xC0 = Normal file info
    //  0xC1 = Normal file name
    union 
    {
        //
        // Structure of the Allocation Bitmap table entry (0x81)
        //
        struct
        {
            UINT8  DE_BmNo;         // 1   Bitmap table number (0 or 1)
            UINT8  DE_Reserved[18]; // 2
            UINT32 DE_Cluster;      // 20  Bitmap table starting cluster
            UINT64 DE_Size;         // 24  Bitmap table size
        } BitMap;
        
        //
        // Structure of the UpCase table entry (0x82)
        //
        struct
        {
            UINT8  DE_Reserved1[3];  // 1
            UINT32 DE_Checksum;      // 4   UpCase table checksum
            UINT8  DE_Reserved2[12]; // 8
            UINT32 DE_Cluster;       // 20  UpCase table starting cluster
            UINT64 DE_Size;          // 24  UpCase table size
        } UpCase;
        
        //
        // Structure of the Volume Name entry (0x83)
        //
        struct
        {
            UINT8  DE_NameLength;   // 1   Volume name length
            CHAR16 DE_VolName[11];  // 2   Volume name (max 11 characters)
            UINT8  DE_Reserved[8];  // 24
        } VName;
        
        //
        // Structure of a normal file entry (0x85)
        //
        struct
        {
            UINT8  DE_SecCount;     // 1   Secondary element count
                                    //     This is the number of additional
                                    //     32-bit elements in the entry
            UINT16 DE_Checksum;     // 2   Checksum
            UINT16 DE_Attr;         // 4   Attributes
                                    //       Read Only = 0x01
                                    //       Hidden    = 0x02
                                    //       System    = 0x04
                                    //       Directory = 0x10
                                    //       Archive   = 0x20
            UINT16 DE_Reserved1;    // 6
            UINT32 DE_Created;      // 8   File created timestamp
            UINT32 DE_Modified;     // 12  File modified timestamp
            UINT32 DE_Accessed;     // 16  File accessed timestamp
            UINT8  DE_Create10ms;   // 20  Created 10ms increment
            UINT8  DE_Mod10ms;      // 21  Modified 10ms increment
            UINT8  DE_CreateTZ;     // 22  Created date time zone
                                    //     Expressed as difference to UTC in
                                    //     15-minute increments
            UINT8  DE_ModTZ;        // 23  Modified date time zone
            UINT8  DE_AccessTZ;     // 24  Accessed date time zone
            UINT8  DE_Reserved2[7]; // 25
        } File;
        
        // Structure of secondary element containing file location (0xC0)
        struct
        {
            UINT8  DE_Flags;        // 1   Secondary element flags
                                    //     Bit 0 - Set for file locator
                                    //     Bit 1 - Set if contiguous file
            UINT8  DE_Reserved1;    // 2
            UINT8  DE_NameLength;   // 3   Length of file name
            UINT16 DE_NameHash;     // 4   Name hash (used for searching)
            UINT16 DE_Reserved2;    // 6
            UINT64 DE_VSize;        // 8   Valid file size (?)
            UINT32 DE_Reserved3;    // 16
            UINT32 DE_Cluster;      // 20  First cluster of file
            UINT64 DE_Size;         // 24  Size of file
        } Info;
        
        //
        // Structure of secondary element containing file name (0xC1)
        //
        struct
        {
            UINT8  DE_Flags;        // 1  Secondary element flags
                                    //    Bit 0 - Reset for file name
            CHAR16 DE_Name[15];     // 2  Part (or all) of file name
                                    //    Name is in unicode
        } Name;
    } EType;
} DIR_ENTRY_EX;


#pragma pack(pop)

VOID 
AmiGetFileListFromExFatVolume (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN  DIR_ENTRY_EX        *Root,
    IN  UINT32              RootEntries,
    OUT UINTN               *NumberOfFiles,
    OUT DIR_ENTRY_EX        **Buffer
);


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**         (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
