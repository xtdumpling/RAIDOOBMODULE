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

/** @file NtfsRecovery.h
    File system recovery data types definition

**/

#ifndef __NTFS_RECOVERY__H__
#define __NTFS_RECOVERY__H__
#ifdef __cplusplus
extern "C"
{
#endif


//
// Some NTFS definitions
//
#define     FILE_RECORD_SIZE            1024 // Size of an NTFS file record
#define     MAXIMUM_RECORD_NUMBER       0x0000FFFFFFFFFFFF // NTFS max record no.
#define     FR_ATTRIBUTE_LIST           0x00000020
#define     FR_ATTRIBUTE_NAME           0x00000030
#define     FR_ATTRIBUTE_VOLUME_NAME    0x00000060
#define     FR_ATTRIBUTE_DATA           0x00000080
#define     FR_ATTRIBUTE_INDEX_ROOT     0x00000090
#define     FR_ATTRIBUTE_INDEX_ALLOC    0x000000A0
#define     MINIMUM_ENTRY_SIZE          0x58

#pragma pack(push, 1)


//
// NTFS Structures
//

/**
    Structure of a Master File Table file record.

**/

typedef struct _MFT_FILE_RECORD
{
    CHAR8 FR_Name[4];               // 0x00 - Always "FILE"
    UINT16 FR_UpdateSeqOffset;      // 0x04
    UINT16 FR_UpdateSeqSize;        // 0x06
    UINT64 FR_LogSeqNumber;         // 0x08
    UINT16 FR_SequenceNumber;       // 0x10
    UINT16 FR_HardLinkCount;        // 0x12
    UINT16 FR_AttributeOffset;      // 0x14 - Offset to Attributes
    UINT16 FR_Flags;                // 0x16 - File Record Flags
                                    //        0x01 - Record is in use
                                    //        0x02 - Record is a directory
    UINT32 FR_RealSize;             // 0x18 - Real size of record
    UINT32 FR_AllocatedSize;        // 0x1C - Allocated size of record
    UINT64 FR_BaseRecordNumber;     // 0x20 - Zero if this is a base record
} MFT_FILE_RECORD;


/**
    Structure of a File Record resident attribute header.

**/

typedef struct _FR_ATTR_HEADER_RES
{
    UINT32 AHR_Type;                // 0x00 - Attribute Type
                                    //        0x30 - File Name
                                    //        0x80 - Data
    UINT32 AHR_Length;              // 0x04 - Attribute Length
    UINT8 AHR_NonResFlag;           // 0x08 - Non-Resident Flag
    UINT8 AHR_NameLength;           // 0x09 - Name Length (if named)
    UINT16 AHR_NameOffset;          // 0x0A - Offset to Name (if named)
    UINT16 AHR_Flags;               // 0x0C - Attribute Hdr. Flags 
                                    //        0x0001 - Compressed
                                    //        0x4000 - Encrypted
                                    //        0x8000 - Sparse
    UINT16 AHR_AttributeId;         // 0x0E
    UINT32 AHR_InfoLength;          // 0x10 - Attribute Information Length
    UINT16 AHR_InfoOffset;          // 0x14 - Offset to this Attribute's 
                                    //        Information
    UINT16 AHR_IndexFlag;           // 0x16
} FR_ATTR_HEADER_RES;


/**
    Structure of a File Record non-resident attribute header.

**/

typedef struct _FR_ATTR_HEADER_NONRES
{
    UINT32 AHNR_Type;               // 0x00 - Attribute Type
                                    //        0x30 - File Name
                                    //        0x80 - Data
    UINT32 AHNR_Length;             // 0x04 - Attribute Length
    UINT8 AHNR_NonResFlag;          // 0x08 - Non-Resident Flag
    UINT8 AHNR_NameLength;          // 0x09 - Name Length (if named)
    UINT16 AHNR_NameOffset;         // 0x0A - Offset to Name (if named)
    UINT16 AHNR_Flags;              // 0x0C - Flags
                                    //        0x0001 - Compressed
                                    //        0x4000 - Encrypted
                                    //        0x8000 - Sparse
    UINT16 AHNR_AttributeId;        // 0x0E
    UINT64 AHNR_StartingVCN;        // 0x10 - Staeting Virtual Cluster Number
    UINT64 AHNR_LastVCN;            // 0x18 - Last Virtual Cluster Number
    UINT16 AHNR_RunOffset;          // 0x20 - Offset to data runs
                                    //        (Usually 0x40 for a 
                                    //        Data attribute)
} FR_ATTR_HEADER_NONRES;

/**
    Structure of a Master File Table Attribute List Entry
    
    @note     When there are lots of attributes and space in the MFT record 
              is short, all those attributes that can be made non-resident 
              are moved out of the MFT. If there is still not enough room, 
              then an ATTRIBUTE_LIST attribute is needed. This attribute
              contains a list of structures that tell where each attribute is
              located. Those that are in the current MFT record will have
              the same number (ALE_MFTRecord) as the current MFT. Those that
              don't fit will have another record number.
    
**/
typedef struct _FR_LIST_ATTRIBUTE_ENTRY
{
    UINT32 ALE_Type;                // 0x00 - Attribute Type
    UINT16 ALE_Length;              // 0x04 - Record Length
    UINT8  ALE_NameLength;          // 0x06 - Name Length (if named)
    UINT8  ALE_NameOffset;          // 0x07 - Offset to Name (if named)
    UINT64 ALE_StartingVCN;         // 0x08 - Starting Virtual Cluster Number
    UINT64 ALE_MFTRecord;           // 0x10 - MFT record containing the attribute
    UINT16 ALE_AttributeId;         // 0x18 - Attribute Id
} FR_LIST_ATTRIBUTE_ENTRY;


/**
    Structure of a Master File Table Name Attribute.

    @note     Time stamps in NTFS are the number of 100ns units since
              January 1, 1601.
              This attribute is preceded by a resident attribute header.
**/

typedef struct _FR_NAME_ATTRIBUTE
{
    UINT64 NA_PDReference;          // 0x00 - Parent Directory Reference 
    UINT64 NA_CreationTime;         // 0x08 - File Creation Time
    UINT64 NA_AlteredTime;          // 0x10 - File Altered Time
    UINT64 NA_MFTChangedTime;       // 0x18 - MFT Record Changed Time
    UINT64 NA_ReadTime;             // 0x20 - File Read Time
    UINT64 NA_AllocatedSize;        // 0x28 - Allocated Size of File
    UINT64 NA_RealSize;             // 0x30 - Real size of File
    UINT32 NA_Flags;                // 0x38 - File Flags
                                    //        Note: These flags correspond to 
                                    //        DOS flags up to 0x20 (Archive).
                                    //        If other flags are set, reject
                                    //        the file.
    UINT32 NA_Reparse;              // 0x3C
    UINT8 NA_NameLength;            // 0x40 - Length of file name in Words
    UINT8 NA_NameType;              // 0x41 - File Name type
                                    //        0x00 - POSIX
                                    //        0x01 - Unicode
                                    //        0x02 - DOS
                                    //        0x03 - Both
    CHAR16 NA_Name[1];              // 0x42 - File Name (unicode)
} FR_NAME_ATTRIBUTE;

/**
    Structure of a Master File Table Data Attribute.

    @note     This attribute can be preceded by a resident or non-resident
              attribute header. If resident, the data will be the actual
              file data. If non-resident, the data will be a run list

**/

typedef struct _FR_DATA_ATTRIBUTE
{
    UINT8 DA_Data[1];   // 0x00 - Run List or data
} FR_DATA_ATTRIBUTE;

/**
    Structure of a Master File Table Index Root Attribute.

    @note     A small index will have it's entries inside this attribute.
              This attribute is preceded by a resident attribute header.

**/
typedef struct _FR_INDEX_ROOT_ATTRIBUTE
{
    UINT32 IRA_Type;
    UINT32 IRA_Collation;
    UINT32 IRA_Size;
    UINT8 IRA_ClustersPerIndexRec;
    UINT8 Padding1[3];
    UINT32 IRA_Offset;      // 0x10 - Offset to Index entries (if small index)
                            // The above offset is from here, 
                            // not from beginning.
    UINT32 IRA_TotalSize;
    UINT32 IRA_AllocSize;
    UINT8 IRA_Flags;        // 0x1C - 0 = small index, 1 = large index
    UINT8 Padding2[3];
} FR_INDEX_ROOT_ATTRIBUTE;

/**
    Structure of a Master File Table Index Allocation Attribute.

    @note     A large index will be in a separate index file, and the 
              run list in this attribute will indicate where it is.
              This attribute is preceded by a non-resident attribute header

**/
typedef struct _FR_INDEX_ALLOC_ATTRIBUTE
{
    UINT8 IAA_RunList[1];   // 0x00 - Run list of index record
} FR_INDEX_ALLOC_ATTRIBUTE;

/**
    Structure of an Index record.

    @note     In NTFS, the Index is equivalent to a directory in FAT. A large
              index will be contained in an Index Record, while a small index
              will be contained within the Master File Table File Record for
              the index, in the Index Root Attribute. Index Records are 
              files, the way FAT directories are files, but not every 
              "directory" will have a file. See the structure INDEX_ENTRY 
              for the actual "directory entry" format.

**/
typedef struct _INDEX_RECORD
{
    CHAR8 INDR_Name[4];              // 0x00 - Always "INDX"
    UINT16 INDR_UpSeqOff;            // 0x04 - Offset to update sequence
    UINT16 INDR_UpSeqSize;           // 0x06 - Size of update sequence
    UINT64 INDR_LogSeqNo;            // 0x08 - Log sequence number
    UINT64 INDR_VCN;                 // 0x10 - Virtual Cluster Number
    UINT32 INDR_IndxEntryOff;        // 0x18 - Offset to Index Entries
                                     //        (from here, not from start)
    UINT32 INDR_IndxEntrySize;       // 0x1C - Size of Index Entries
    UINT32 INDR_IndxEntryAlloc;      // 0x20 - Allocated size of Entries
    UINT8 INDR_LeafNode;             // 0x24 - 1 if not leaf node
    UINT8 INDR_Padding[3];           // 0x25 - Padding
    UINT16 INDR_UpdateSequence;      // 0x28 - Update Sequence
    UINT16 INDR_SeqenceArray[1];     // 0x2A - Sequence Array
} INDEX_RECORD;

/**
    Structure of an Index record entry.

    @note     In NTFS, the index is the equivalent of a directory in FAT,
              so the index entry is the equivalent of a directory entry.
              The lower 4 bytes of the MFT reference is the number of the
              file's File Record in the MFT. The lower 4 bytes of the MFT
              parent reference is the number of the File Record for the
              Index that this entry is in.
**/
typedef struct _INDEX_ENTRY
{
    UINT64 INDE_MFTRecord;      // 0x00 - MFT reference of this entry's file
    UINT16 INDE_EntrySize;      // 0x08 - Size of this entry
    UINT16 INDE_NameOffset;     // 0x0a - Offset to file name
    UINT16 INDE_IndexFlags;     // 0x0C - Index Flags
    UINT16 INDE_Padding;
    UINT64 INDE_MFTParentRec;   // 0x10 - MFT refernece of this entry's parent
    UINT64 INDE_CreationTime;   // 0x18 - File Creation Time
    UINT64 INDE_AlteredTime;    // 0x20 - File Altered Time
    UINT64 INDE_MFTChangedTime; // 0x28 - MFT Record Changed Time
    UINT64 INDE_ReadTime;       // 0x30 - File Read Time
    UINT64 INDE_AllocSize;      // 0x38 - Allocated File Size
    UINT64 INDE_RealSize;       // 0x40 - Real File Size
    UINT64 INDE_FileFlags;      // 0x48 - File Flags (like DOS flags + more)
    UINT8 INDE_NameLength;      // 0x50 - Length of File name
    UINT8 INDE_NameType;        // 0x51 - File Name type
                                //        0x00 - POSIX
                                //        0x01 - Unicode
                                //        0x02 - DOS
                                //        0x03 - Both
    CHAR16 INDE_Name[1];        // 0x52 - File Name (Unicode, not terminated)
} INDEX_ENTRY;


#pragma pack(pop)


VOID 
AmiGetFileListFromNtfsVolume(
    IN EFI_PEI_SERVICES **PeiServices,
    IN  UINT8               *Root,
    IN  UINT32              RootSize,
    OUT UINTN               *NumberOfFiles,
    OUT INDEX_ENTRY         **Buffer
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
