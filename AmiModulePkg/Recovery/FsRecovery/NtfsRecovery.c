//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 1985-2014, American Megatrends, Inc.        **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file NtfsRecovery.c

**/

//----------------------------------------------------------------------

#include <Token.h>
#include <AmiPeiLib.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Ppi/BlockIo.h>
#include <Guid/AmiRecoveryDevice.h>

#include "NtfsRecovery.h"
#include "FsRecovery.h"

//----------------------------------------------------------------------

INDEX_ENTRY         *NtfsRecoveryFiles[10];
BOOLEAN             ResidentPresent;
UINT32              ResidentSize;

extern UINTN               PartCount;
extern BOOLEAN             IsMbr;
extern UINT32              GpeCount;
extern UINT32              PartSector;
extern UINT32              ExtOffset;
extern UINT8               *ReadBuffer;
extern UINT8               *RootBuffer;
extern UINTN               RootBufferSize ;
extern UINT32              RootSize;
extern MEMORY_BLOCK        *MemBlk;
extern BOOLEAN             RootAllocated;
RC_VOL_INFO                *gVolume;
BOOT_SECTOR                *gBs;


/**
    Checks if given data block describes NTFS structure


    @param pBpb - pointer to data block to check

         
    @retval TRUE - data block is a NTFS structure
    @retval FALSE - data block is not a NTFS structure

**/
BOOLEAN 
IsNtfs (
    IN BOOT_SECTOR  *pBpb 
)
{
    return pBpb->NumFATs == 0
           && pBpb->TotSec16 == 0
           && pBpb->TotSec32 == 0
           && pBpb->Fat.Ntfs.TotSec64 != 0
           && pBpb->OEMName[0] == 0x4E // Name must be "NTFS"
           && pBpb->OEMName[1] == 0x54
           && pBpb->OEMName[2] == 0x46
           && pBpb->OEMName[3] == 0x53
           && pBpb->Signature == 0xAA55
           && (pBpb->jmp[0] == 0xEB || pBpb->jmp[0] == 0xE9);
}

/**
    Applies fixup on a File or Index record if necessary


    @param Vi - Volume Info Structure
    @param Buffer - Buffer containing File or Index record


    @retval EFI_STATUS - Status (EFI_SUCCESS or EFI_VOLUME_CORRUPTED)
    
    
    @note
        The "Fixup" is a method NTFS uses to protect important
        records from bad sectors.
        For more information, see "NTFS Documentation" by
        Richard Russon and Yuval Fledel, chapter 14,
        "Concept - Fixup".
        For a record that requires fixup, the last two bytes
        of each sector are stored in a "Fixup array", and are
        replaced with sequence numbers. This function compares
        those bytes with a table, replaces them with the
        stored originals, and reports a corrupt volume if
        there is a comparison mis-match.    

**/
EFI_STATUS 
ApplyFixup (
    RC_VOL_INFO  *Volume,
    UINT8        *Buffer
)
{
    UINT16      *FixupPtr;
    UINT16      *ArrayPtr;
    UINT16      *FixupArray;
    UINT16      FixupCount;

    //
    // Check for signature 'FILE' or 'INDX'
    //
    if ( ((Buffer[0] != 'F') || \
          (Buffer[1] != 'I') || \
          (Buffer[2] != 'L') || \
          (Buffer[3] != 'E')) &&
         ((Buffer[0] != 'I') || \
          (Buffer[1] != 'N') || \
          (Buffer[2] != 'D') || \
          (Buffer[3] != 'X')) ) {
        return EFI_NOT_FOUND;
    }

    FixupArray = (UINT16*)(Buffer + \
        ( (MFT_FILE_RECORD*)Buffer)->FR_UpdateSeqOffset );
    FixupCount = (( (MFT_FILE_RECORD*)Buffer)->FR_UpdateSeqSize ) - 1;

    //
    // Check if fixup needed to be applied
    //
    if ( *FixupArray == 0 ) {
        return EFI_SUCCESS;
    }

    ArrayPtr = FixupArray + 1; // Array starts after the check word.
    //
    // Point FixpuPtr to the last word of a sector or 512-byte piece
    //
    FixupPtr = (UINT16*)( Buffer + 510 );

    //
    // Apply fixup
    //
    do {
        if ( *FixupPtr != *FixupArray ) {
            return EFI_VOLUME_CORRUPTED;
        }

        *FixupPtr = *ArrayPtr;
        ArrayPtr++;
        FixupPtr += ( Volume->BlockSize )/sizeof( *FixupPtr );
    } while ( --FixupCount );

    *FixupArray = 0;

    return EFI_SUCCESS;
}

/**
    (NTFS) Retrieves the count and cluster of a run from a run list element


    @param pRunList - Pointer to the run list, updated if UpdateList is TRUE.
    @param ClusterCount - Length of this run in clusters.
    @param Cluster - Starting cluster of this run.
    @param UpdateList - Update list pointer to next element if TRUE.


    @retval EFI_STATUS - Status (EFI_SUCCESS or EFI_END_OF_FILE)
    
    
    @note
        A run list element has 3 parts -- a size byte, a Cluster
        count, and a Cluster Number.
        The low nibble of the size byte is the size of the Count
        in bytes. The high nibble is the size of the Offset in
        bytes. The element is therefore 1 + (low nibble) + (high
        nibble) bytes long.
        The cluster number is a signed number. The new cluster is
        added to the old one to get the result. So if the new
        cluster lies before the old one on the disk, it will be
        a negative number.  

**/
EFI_STATUS
GetRunListElementData (
    UINT8    **pRunList,
    UINT64   *ClusterCount,
    UINT64   *Cluster,
    BOOLEAN  UpdateList
)
{
    UINT64 TempCount = 0;
    UINT64 TempCluster = 0;
    UINT64 LeftFill = 0;
    UINT8  LowNibble;
    UINT8  HighNibble;
    UINT8  i, HighByte;
    UINT8  *RunListPtr;

    //
    // If the size byte is 0, we have reached the end of the file.
    //    
    RunListPtr = *pRunList;
    if ( RunListPtr[0] == 0 )
    {
        return EFI_END_OF_FILE;
    }

    LowNibble = RunListPtr[0] & 0xF;
    HighNibble = RunListPtr[0] >> 4;
    RunListPtr++;
    //
    // Get run length.
    //    
    for ( i=LowNibble; i>0; i-- )
    {
        TempCount = Shl64( TempCount, 8 );
        TempCount += RunListPtr[i-1];
    }
    RunListPtr += LowNibble;
    //
    // Get the run offset.
    //    
    HighByte = RunListPtr[HighNibble-1];
    for ( i=HighNibble; i>0; i-- )
    {
        TempCluster = Shl64(TempCluster, 8);
        TempCluster += RunListPtr[i-1];
    }
    RunListPtr += HighNibble;
    //
    // If the offset is negative, left-fill the empty bytes with 0xFF.
    //    
    if ( ( HighByte & 0x80 ) && ( HighNibble < 8 ) ) 
    {
        for ( i=8; i>HighNibble; i-- )
        {
            LeftFill = Shr64( LeftFill, 8 );
            LeftFill |= 0xFF00000000000000;
        }
        TempCluster |= LeftFill;
    }

    *Cluster += TempCluster;
    *ClusterCount = TempCount;
    if ( UpdateList ) *pRunList = RunListPtr;
    return EFI_SUCCESS;
}

/**
    (NTFS) Retrieves a File Record Attribute by it's number from a
    File Record.


    @param BufferIn - Pointer to a buffer containing a file record
    @param AttributeNumber - Number of the attribute to retrieve
    @param BufferOut - Points to the attribute in the buffer


    @retval EFI_STATUS - Status (EFI_SUCCESS or EFI_NOT_FOUND)


    @note
        Attributes are in sequential order, so, for example,
        if we're looking for 30, and we find 10 and then 40,
         we know there is no 30 in the record.  
**/
EFI_STATUS 
GetFrAttribute (
    UINT8  *BufferIn,
    UINT8  AttributeNumber,
    UINT8  **BufferOut
) 
{
    UINT8   *TempBuffer;

    TempBuffer = BufferIn;

    //    
    // Point to 1st attribute.
    //    
    TempBuffer += ((MFT_FILE_RECORD*)TempBuffer)->FR_AttributeOffset; 
    //
    // Search for the attribute.
    //    
    while ( TempBuffer[0] != AttributeNumber )
    {
        if ( TempBuffer[0] > AttributeNumber ) return EFI_NOT_FOUND;
        if ( TempBuffer[0] == 0xFF ) return EFI_NOT_FOUND;
        if ( ((FR_ATTR_HEADER_RES*)TempBuffer)->AHR_Length == 0 )
            return EFI_NOT_FOUND;
        TempBuffer += ((FR_ATTR_HEADER_RES*)TempBuffer)->AHR_Length;
    }

    *BufferOut = TempBuffer;
    return EFI_SUCCESS;
}

/**
    (NTFS) Returns the file record specified by MFTRecordNo in a buffer.


    @param Vi - Volume Info Structure
    @param Bs - Boot sector structure
    @param MFTRecordNo - MFT Record number to get
    @param Buffer - Buffer to read record into
    @param MFTSector - Sector where record found


    @retval EFI_STATUS - Status (EFI_SUCCESS or EFI_NOT_FOUND)
    
    
    @note
        The File Records in the Master File Table are numbered
        sequentially. We just have to count our way through the
        MFT's run list until we find it.    

**/
EFI_STATUS 
GetFileRecord (
    EFI_PEI_SERVICES  **PeiServices,
    RC_VOL_INFO       *Volume,
    BOOT_SECTOR       *Bs,
    UINT64            MFTRecordNo,
    UINT8             *FrBuffer,
    UINT64            *MFTSector OPTIONAL
)
{
    EFI_STATUS Status;
    UINT8      *pRunList;
    UINT64     Cluster;
    UINT64     Sector;
    UINT64     Count;
    UINT64     ByteCount;
    UINT32     SecPerRecord = 1;
    UINT64     RecordCount;
    UINT32     BytesPerCluster;
    UINT64     Offset;
    UINT32     RecordSize;

    Cluster = 0;
    pRunList = &MemBlk->MFTRunList[0];

    MFTRecordNo &= MAXIMUM_RECORD_NUMBER; // Isolate number part

    Status = GetRunListElementData( &pRunList, &Count, &Cluster, TRUE );
    BytesPerCluster = Bs->BytsPerSec * Bs->SecPerClus;
    ByteCount = Mul64( Count, BytesPerCluster );
    if ( Bs->BytsPerSec <= FILE_RECORD_SIZE ) {
        SecPerRecord = FILE_RECORD_SIZE / Bs->BytsPerSec;
        RecordSize = FILE_RECORD_SIZE;
    } else { // Special case for 4k sectors
        SecPerRecord = 1;
        RecordSize = (UINT32)Bs->BytsPerSec;
    }
    // ###DEBUG CHANGE NEEDED LATER ////
    // In NTFS, the cluster size can be 512 bytes to 4096 bytes.
    // File records are 1024 bytes
    // For now, we're going to assume a cluster size of 1024 bytes or more.
    ////////////////////////////

    Sector = Mul64( Cluster, Bs->SecPerClus );
    RecordCount = 0;
    do {
        if ( ByteCount > 0 )
        {
            Sector += SecPerRecord; 
            ByteCount -= RecordSize;
        } else { // We've used up a run, read from the next one.
            Status = GetRunListElementData( &pRunList, 
                                            &Count, 
                                            &Cluster, 
                                            TRUE );
            if ( EFI_ERROR(Status) ) return EFI_NOT_FOUND;
            ByteCount = Mul64( Count, BytesPerCluster );
            Sector = Mul64( Cluster, Bs->SecPerClus );
            continue;
        }
        RecordCount++;
    } while ( RecordCount < MFTRecordNo ); // Record numbers are 0-based.
    //
    // We found the sector of the file record wanted. Now read it.
    //
    Offset = Mul64( Sector, Bs->BytsPerSec );
    Status = ReadDevice( PeiServices, Volume, Offset, RecordSize, FrBuffer );
    if (EFI_ERROR(Status)) return EFI_NOT_FOUND;
    //
    // A File recored begins with "FILE". Check it.
    //
    if ( ( FrBuffer[0] != 0x46 ) || \
         ( FrBuffer[1] != 0x49 ) || \
         ( FrBuffer[2] != 0x4C ) || \
         ( FrBuffer[3] != 0x45 ) ) return EFI_NOT_FOUND;

    *MFTSector = Sector; // Return sector where the record was found
    //
    // Check if fixup is needed, and apply it if so
    //
    Status = ApplyFixup ( Volume, FrBuffer );

    if ( EFI_ERROR(Status) )
        return EFI_NOT_FOUND;

    return EFI_SUCCESS;
}

/**
    Reads a file from a device formatted in NTFS.


    @param Vi - Volume Info Structure
    @param Bs - Boot sector structure
    @param RunList - Run List of file to read
    @param Buffer - Buffer to read into
    @param Size - Size of file to read

               
    @retval EFI_STATUS - possible return values 

**/
EFI_STATUS 
ReadNTFSFile ( 
    IN EFI_PEI_SERVICES  **PeiServices,
    RC_VOL_INFO          *Volume, 
    BOOT_SECTOR          *Bs, 
    UINT8                *RunList, 
    VOID                 *Buffer, 
    UINT64               *Size 
) 
{
    UINT64      TotalContiguousBytes;
    UINT64      TotalBytesRead = 0;
    UINT64      AbsByte;
    UINT64      AccessBytes;
    UINT64      ClusterCount;
    UINT64      Cluster = 0;
    EFI_STATUS  Status;

    Status = GetRunListElementData( &RunList, &ClusterCount,
                                       &Cluster, TRUE );
    if ( EFI_ERROR(Status) ) {
        return Status;
    }
    do {
        TotalContiguousBytes = Mul64( ClusterCount,
                                     Bs->SecPerClus );
        TotalContiguousBytes = Mul64( TotalContiguousBytes,
                                     Bs->BytsPerSec );
        if ( TotalContiguousBytes > *Size ) AccessBytes = *Size;

        else AccessBytes = TotalContiguousBytes;

        AbsByte = Mul64( Cluster, Bs->SecPerClus );
        AbsByte = Mul64( AbsByte, Bs->BytsPerSec );

        if ( AccessBytes == 0 ) {
            return EFI_VOLUME_CORRUPTED; // Will happen if early EOF.
        }

        Status = ReadDevice( PeiServices, 
                             Volume, 
                             AbsByte, 
                             (UINTN)AccessBytes, 
                             Buffer );

        if ( EFI_ERROR(Status) ) {
            break;
        }

        Buffer = (UINT8 *)Buffer + AccessBytes;
        TotalBytesRead +=AccessBytes;

        *Size   -= AccessBytes; 

        if ( AccessBytes == TotalContiguousBytes )
        {
            Status = GetRunListElementData ( &RunList, &ClusterCount,
                                            &Cluster, TRUE  );
            if (EFI_ERROR(Status)) break; // Error here means EOF.
        }

    } while (*Size);

    *Size = (UINT32)TotalBytesRead;
    return EFI_SUCCESS;
}

/**
    Checks the attribute list to see if the specified attribute
    is present. If it is, reads in it's record if it is not the
    current record.
    
    @param  PeiServices - pointer to PEI services
    @param  Volume - pointer to volume description structure
    @param  Bs - pointer to MBR
    @param  Buffer - Pointer to record buffer
    @param  Attribute - Attribute to check against list
    @param  ListSize - Attribute List size
    @param  MFTRecord - Current MFT record
**/
VOID
GetAttributeRecord (
    IN EFI_PEI_SERVICES **PeiServices,
    IN OUT RC_VOL_INFO  *Volume,
    IN BOOT_SECTOR      *Bs,
    IN UINT8            *Buffer,
    IN UINT32           Attribute,
    IN UINT32           ListSize,
    IN OUT UINT64       *MFTRecord
)
{
    UINT64      TmpRecord;
    UINT32      TmpSize;
    UINT8       *TmpBuffer;
    EFI_STATUS  Status;
    
    TmpRecord = *MFTRecord;
    TmpSize = ListSize;
    TmpBuffer = MemBlk->AttributeList;
    
    do {
        if (Attribute == ((FR_LIST_ATTRIBUTE_ENTRY*)TmpBuffer)->ALE_Type) {
            TmpRecord = ((FR_LIST_ATTRIBUTE_ENTRY*)TmpBuffer)->ALE_MFTRecord;
            if (TmpRecord != *MFTRecord) {
                Status = GetFileRecord( PeiServices, 
                                        Volume, 
                                        Bs, 
                                        TmpRecord, 
                                        Buffer, 
                                        NULL ); 
                if (Status == EFI_SUCCESS) {
                    *MFTRecord = TmpRecord;
                    PEI_TRACE(( -1, PeiServices, "\nRecord %x loaded from attribute list\n", TmpRecord ));
                }
            }
            break;
        }
        TmpBuffer += ((FR_LIST_ATTRIBUTE_ENTRY*)TmpBuffer)->ALE_Length;
        TmpSize -= ((FR_LIST_ATTRIBUTE_ENTRY*)TmpBuffer)->ALE_Length;
        if (((FR_LIST_ATTRIBUTE_ENTRY*)TmpBuffer)->ALE_Type == 0) break; // end of list
    } while (TmpSize);
}

/**
    Reads an NTFS directory, specified by MFT record number.

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param Bs - pointer to MBR
    @param MFTRecord - MFT record number of the directory to read
    @param FrBuffer - Buffer for reading MFT record number.

               
    @retval EFI_STATUS - possible return values 


    @note  
        The root buffer is re-used for any subsequent directories.
**/
EFI_STATUS 
ReadNTFSDirectory (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN BOOT_SECTOR       *Bs,
    IN UINT64            MFTRecord,
    IN UINT8             *FrBuffer 
)
{
    UINT8       *Buffer;
    UINT8       *Buffer2;
    UINT8       *pRunList;
    EFI_STATUS  Status;
    UINT32      Temp32;
    UINT16      Temp16;
    UINT32      IndexSize;
    UINT64      Cluster = 0;
    UINT64      ClusterCount;
    UINT64      TmpDirSize;
    UINTN       DirPages;
    EFI_PHYSICAL_ADDRESS Allocate;
    BOOLEAN     AttributeListPresent;
    UINT32      AttributeListSize;
    UINT64      TmpRecord = MFTRecord;

    Buffer = &FrBuffer[0];

    Status = GetFileRecord( PeiServices, 
                            Volume, 
                            Bs, 
                            MFTRecord, 
                            Buffer, 
                            NULL ); 
    if ( EFI_ERROR( Status )) {
        PEI_TRACE(( -1, PeiServices, "\nRead directory: record not found\n" ));
        return Status;
    }
    //
    // Check for an Attribute List Attribute. If present, it could mean that
    // the Index Root and/or Index Allocation attribute is not in the current
    // MFT record.
    //
    AttributeListPresent = FALSE;
    Buffer2 = Buffer;
    Status = GetFrAttribute( Buffer, FR_ATTRIBUTE_LIST, &Buffer2 );
    if ( (Status == EFI_SUCCESS) && (((FR_ATTR_HEADER_RES*)Buffer2)->AHR_NonResFlag == 0) ){
        AttributeListPresent = TRUE;
        Temp16 = ((FR_ATTR_HEADER_RES*)Buffer2)->AHR_InfoOffset; // List location
        AttributeListSize = ((FR_ATTR_HEADER_RES*)Buffer2)->AHR_Length - Temp16;
        Buffer2 += Temp16; // Point to the list
        MemCpy( MemBlk->AttributeList, Buffer2, AttributeListSize );
        PEI_TRACE(( -1, PeiServices, "\nAttribute list found, size is %x\n", AttributeListSize ));
    }
    //
    // Check for a resident index. It will be in the Index Root Attribute.
    // If one if found, it will be saved for searching later.
    //
    ResidentPresent = FALSE;
    if (AttributeListPresent) {
        GetAttributeRecord( PeiServices,
                            Volume,
                            Bs,
                            Buffer, 
                            FR_ATTRIBUTE_INDEX_ROOT, 
                            AttributeListSize, 
                            &TmpRecord);
    }
    Buffer2 = Buffer;
    Status = GetFrAttribute( Buffer, FR_ATTRIBUTE_INDEX_ROOT, &Buffer2 );
    if ( Status == EFI_SUCCESS) { // Root Attribute found
        Temp16 = ((FR_ATTR_HEADER_RES*)Buffer2)->AHR_InfoOffset;
        Buffer2 += Temp16;
        IndexSize = (( FR_INDEX_ROOT_ATTRIBUTE*)Buffer2 )->IRA_TotalSize;
        Temp32 = (( FR_INDEX_ROOT_ATTRIBUTE*)Buffer2 )->IRA_Offset;
        Buffer2 += Temp32 + EFI_FIELD_OFFSET(FR_INDEX_ROOT_ATTRIBUTE, IRA_Offset);
        if ( IndexSize >= MINIMUM_ENTRY_SIZE ) { // Resident index is not empty
            PEI_TRACE((-1, PeiServices, "\nRead directory: resident part found\n"));
            MemCpy ( MemBlk->ResidentIndex, Buffer2, IndexSize );
            ResidentPresent = TRUE;
            ResidentSize = IndexSize;
        }
    }
    //
    // Now, check for a non-resident index.
    //
    if (AttributeListPresent) {
        GetAttributeRecord( PeiServices,
                            Volume,
                            Bs,
                            Buffer, 
                            FR_ATTRIBUTE_INDEX_ALLOC, 
                            AttributeListSize, 
                            &TmpRecord);
    }
    Status = GetFrAttribute( Buffer, FR_ATTRIBUTE_INDEX_ALLOC, &Buffer );
    if ( EFI_ERROR( Status )) {
        PEI_TRACE((-1, PeiServices, "\nRead directory: no index allocation attribute\n"));
        if ( ResidentPresent ) return EFI_SUCCESS; else return Status;
    }
    Buffer += (( FR_ATTR_HEADER_NONRES* )Buffer )->AHNR_RunOffset; // Point to run list
    MemCpy( MemBlk->RootRunList, Buffer, 128 ); // Copy Root run list
    //
    // Calculate root directory size by running its run list.
    //
    pRunList = &MemBlk->RootRunList[0];
    TmpDirSize = 0;
    Cluster = 0;
    do {
        Status = GetRunListElementData( &pRunList, &ClusterCount, &Cluster, TRUE );
        if ( Status == EFI_SUCCESS ) TmpDirSize += ClusterCount;
    } while ( Status == EFI_SUCCESS );
    TmpDirSize = Mul64 ( TmpDirSize, Bs->SecPerClus );
    TmpDirSize = Mul64 ( TmpDirSize, Bs->BytsPerSec );

    if (!RootAllocated) {
        DirPages = (UINTN) (Shr64( TmpDirSize, EFI_PAGE_SHIFT ) + ((TmpDirSize & EFI_PAGE_MASK) ? 1 : 0) );
        Status = (*PeiServices)->AllocatePages( PeiServices, EfiBootServicesData, DirPages, &Allocate );
        if ( EFI_ERROR( Status )) {
            PEI_TRACE((-1, PeiServices, "\nRead directory: can't allocate memory\n"));
            return Status;
        }
        RootBuffer     = (UINT8*)((UINTN)Allocate);
        RootBufferSize = EFI_PAGES_TO_SIZE( DirPages );
        MemSet( RootBuffer, RootBufferSize, 0 );
        RootAllocated = TRUE;
    } else {
        if ( TmpDirSize > RootBufferSize ) TmpDirSize = RootBufferSize;
    }
    
    pRunList = &MemBlk->RootRunList[0];
    Status = ReadNTFSFile( PeiServices, Volume, Bs, pRunList, RootBuffer, &TmpDirSize );
    if ( EFI_ERROR(Status) ) {
        PEI_TRACE((-1, PeiServices, "\nRead directory: error reading dir file\n"));
    }
    RootSize = (UINT32)TmpDirSize;

    return Status;
}

#if MATCH_VOLUME_NAME
/**
    Reads the volume name of the current NTFS volume and checks it against token

        
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param Bs - pointer to MBR
    @param FrBuffer - File Record Buffer

               
    @retval EFI_STATUS - possible return values 

**/
EFI_STATUS 
CheckNTFSVolumeName (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN RC_VOL_INFO       *Volume,
    IN BOOT_SECTOR       *Bs,
    IN UINT8             *FrBuffer 
)
{
    EFI_STATUS Status;
    UINTN Length;
    UINT8 i;
    UINT8 *TmpBuffer = &FrBuffer[0];
    UINT8 NameBuffer[128];
    CHAR8 *VolumeName = CONVERT_TO_STRING(VOLUME_NAME);

    //
    // The volume name is record no. 3 in the MFT.
    //    
    Status = GetFileRecord( PeiServices, Volume, Bs, 3, TmpBuffer, NULL );
    if (EFI_ERROR(Status) ) 
    {
        PEI_TRACE((-1, PeiServices, "\nVolume name record not found\n"));
        return Status;
    }
    //
    // Find the Volume Name Attribute, then get the name length and
    // point to the name.
    //        
    Status = GetFrAttribute ( TmpBuffer, FR_ATTRIBUTE_VOLUME_NAME, &TmpBuffer );
    if ( Status == EFI_SUCCESS )
    {
        Length = (UINTN)((FR_ATTR_HEADER_RES*)TmpBuffer)->AHR_InfoLength;
        if( Length == 0 ) return EFI_NOT_FOUND; // No volume name
        TmpBuffer += ((FR_ATTR_HEADER_RES*)TmpBuffer)->AHR_InfoOffset;
        //
        // Copy the name to the supplied buffer
        // The name is CHAR16, so we reduce it to CHAR8
        //    
        Length = Length / 2;
        for ( i=0; i<Length; i++ ) {
            NameBuffer[i] = TmpBuffer[i*2];
        }
        NameBuffer[i] = 0; // Terminate name
        Status = EFI_NOT_FOUND;
        Length = Strlen( VolumeName ); // Use supplied name length for comparison
        if( FileCompare( (CHAR8*)NameBuffer, VolumeName, FALSE, Length) ) {
            PEI_TRACE(( -1, PeiServices, "\nVolume found: %s\n", NameBuffer ));
            Status = EFI_SUCCESS;
        }
    }
    return Status;
}
#endif

/**
    Prepares given volume for read operations. Reads NTFS root directory.

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param Bs - pointer to MBR

               
    @retval EFI_STATUS - possible return values 

**/
EFI_STATUS 
ReadNTFSRoot (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN BOOT_SECTOR       *Bs 
)
{
    UINT8       FrBuffer[FILE_RECORD_SIZE]; // This needs to be 4096 for 4k secs.
    UINT8       *Buffer2;
    EFI_STATUS  Status;
    UINT64      Temp64;

    Temp64 = Mul64( Bs->Fat.Ntfs.MFTClus, Bs->SecPerClus );
    //
    // For NTFS, FatOffset is MFT Offset
    //
    Volume->FatOffset = Mul64( Temp64, Bs->BytsPerSec ); 

    //
    // Read the first file record of the MFT, to get the MFT run list.
    //
    Status = ReadDevice( PeiServices, 
                         Volume, 
                         Volume->FatOffset, 
                         FILE_RECORD_SIZE, 
                         FrBuffer );
    if ( EFI_ERROR( Status )) {
        return Status;
    }
    Buffer2 = &FrBuffer[0];
    //
    // Get data attribute
    //
    Status = GetFrAttribute( Buffer2, FR_ATTRIBUTE_DATA, &Buffer2 ); 
    if ( EFI_ERROR( Status )) {
        return Status;
    }
    //
    // Point to run list
    //
    Buffer2 += ((FR_ATTR_HEADER_NONRES*)Buffer2)->AHNR_RunOffset; 
    MemCpy( MemBlk->MFTRunList, Buffer2, 256 ); // Copy MFT run list

#if MATCH_VOLUME_NAME
    Status = CheckNTFSVolumeName( PeiServices, Volume, Bs, &FrBuffer[0] );
    if ( EFI_ERROR( Status )) {
        return Status;
    }
#endif
    //
    // Get the root directory file record, to get its run list.
    //
    Buffer2 = &FrBuffer[0];

    //
    // Read in ROOT directory
    //
    RootAllocated = FALSE; // First directory read,so space not allocated yet
    //
    // Root is always record no. 5
    //
    Status = ReadNTFSDirectory( PeiServices, Volume, Bs, 5, Buffer2 ); 

    return Status;
}

/**
    Reads recovery capsule from NTFS volume

        
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FileName - recovery capsule file name
    @param FileSize - pointer to size of provided buffer
    @param Buffer - pointer to buffer to store data


    @retval EFI_STATUS

**/
EFI_STATUS 
ProcessNTFSVolume (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN OUT UINTN         *FileSize,
    OUT VOID             *Buffer 
)
{
    EFI_STATUS           Status;
    UINT32               i;
    UINTN               NumberOfFiles;
    UINT64              MFTRecord;
    UINT8               *TmpBuffer;
    UINT8               *pRunList;
    UINT64              TmpFileSize;

    Status = ReadDevice( PeiServices, Volume, 0, 512, &MemBlk->Bs );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

    if (!IsNtfs( &MemBlk->Bs )) {
        PEI_TRACE(( -1, PeiServices, "\nIsNTFS failed\n" ));
        return EFI_NOT_FOUND;
    }

    Status = ReadNTFSRoot( PeiServices, Volume, &MemBlk->Bs );

    if ( EFI_ERROR( Status )) {
        PEI_TRACE(( -1, PeiServices, "\nRead root failed\n" ));
        return Status;
    }

    gVolume = Volume;
    gBs = &MemBlk->Bs;

    AmiGetFileListFromNtfsVolume( PeiServices, 
                                  (UINT8*)RootBuffer, 
                                  RootSize, 
                                  &NumberOfFiles, 
                                  NtfsRecoveryFiles );

    if ( NumberOfFiles == 0 )
        return EFI_NOT_FOUND;

    for(i = 0; i < NumberOfFiles; i++) {
        if ( *FileSize < NtfsRecoveryFiles[i]->INDE_RealSize )
            continue;

        TmpBuffer = (UINT8*)Buffer;
        TmpFileSize = NtfsRecoveryFiles[i]->INDE_RealSize;
        //
        // Get the file's MFT record number, and from that it's run list
        //
        MFTRecord = NtfsRecoveryFiles[i]->INDE_MFTRecord & MAXIMUM_RECORD_NUMBER;
        Status = GetFileRecord( PeiServices, Volume, &MemBlk->Bs, MFTRecord, TmpBuffer, NULL );
        if ( EFI_ERROR( Status )) {
            return Status;
        }
        Status = GetFrAttribute( TmpBuffer, FR_ATTRIBUTE_DATA, &TmpBuffer );
        if ( EFI_ERROR( Status )) {
            return Status;
        }
        TmpBuffer += (( FR_ATTR_HEADER_NONRES*)TmpBuffer )->AHNR_RunOffset; // Point to run list
        MemCpy( MemBlk->RootRunList, TmpBuffer, 128 ); // Copy the file's run list
        //
        // Read the file into the provided buffer
        //
        pRunList = &MemBlk->RootRunList[0];
        Status = ReadNTFSFile( PeiServices, 
                               Volume, 
                               &MemBlk->Bs, 
                               pRunList, 
                               Buffer, 
                               &TmpFileSize );
        if ( EFI_ERROR( Status )) {
            return Status;
        }

        if( AmiIsValidFile(Buffer, (UINTN)TmpFileSize) ) {
            *FileSize = (UINTN)TmpFileSize;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}


#if NTFS_NAME_CORRECTION
/**
    Copies a name from one location to another, but skips
    over any characters less than 0x20.


    @param Name1
    @param Name2
    @param ShortName


    @retval VOID

**/
VOID
FixName (
    CHAR16   *Name1,
    CHAR16   *Name2,
    BOOLEAN  ShortName
)
{
    if ( ShortName ) {
    while ( *Name1 ) {
        if (( *Name2 >= 0x20 ) && ( *Name2 < 0x60 ))
        {
            *Name1++ = *Name2++;
        } else {
            Name1++;
            Name2++;
        }
    }
    } else {
    while ( *Name1 ) {
        if ( *Name2 >= 0x20 )
        {
            *Name1++ = *Name2++;
        } else {
            Name1++;
            Name2++;
        }
    }
    }
}
#endif

/**
    Finds a specified file in an NTFS directory.


    @param DirPtr - Pointer to a buffer containing the directory
    @param DirSize - Size of the directory
    @param FileName - Name of the file to find
    @param IndxPtr - Pointer to buffer containing index entry of the file that was found.

         
    @retval TRUE - File found. FALSE - File not found

**/
BOOLEAN 
FindFileInNtfsDirectory (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN  UINT8               *DirPtr,
    IN  UINT32              DirSize,
    IN  VOID                *FileName,
    OUT INDEX_ENTRY         **IndxPtr
)
{
    UINT8       i;
    UINT32      IndexSize;
    UINT32      IndexSize2;
    UINTN       Offset;
    UINT8       *TmpPtr;
    CHAR16      *NamePtr;
    CHAR8       TmpFileName[256];
    UINT8       LfnSize;
    UINT16      EntrySize;
    EFI_STATUS  Status;
#if NTFS_NAME_CORRECTION
    UINT8       TmpBuf[FILE_RECORD_SIZE];
    CHAR16      TmpName16[256];
    UINT8       *TmpPtr2;
    UINT8       *TmpPtr3;
    BOOLEAN     DosName;
#endif

    if (ResidentPresent) { // If resident index found...
        PEI_TRACE((-1, PeiServices, "\nEntering resident search\n"));
        TmpPtr = &MemBlk->ResidentIndex[0];
        IndexSize = ResidentSize;

        do { // loop inside the index
            EntrySize = ( (INDEX_ENTRY*)TmpPtr )->INDE_EntrySize;
            LfnSize = ( (INDEX_ENTRY*)TmpPtr )->INDE_NameLength;
            NamePtr = &( (INDEX_ENTRY*)TmpPtr )->INDE_Name[0];
            for ( i=0; i<LfnSize; i++ )
            {
                TmpFileName[i] = (CHAR8)(CHAR16)NamePtr[i];
            }
            TmpFileName[i] = 0; // Zero-terminate name

            if(!EFI_ERROR( FileSearch( (CHAR8*)FileName, TmpFileName, FALSE, LfnSize ))) {
                *IndxPtr = (INDEX_ENTRY*)&TmpPtr[0];
                PEI_TRACE(( -1, PeiServices, "\nResident file found: %s\n", FileName ));
                return TRUE;
            }

            TmpPtr += EntrySize;
            IndexSize -= EntrySize;
            if ( IndexSize < MINIMUM_ENTRY_SIZE ) break;

        } while ( IndexSize );
    }

    PEI_TRACE((-1, PeiServices, "\nBeginning non-resident search\n"));
    do { // do loop handling indexes in the directory
        Offset = 0;
        //
        // Look for "INDX", start of index record
        //
        if ( ( DirPtr[0] == 0x49 ) && \
             ( DirPtr[1] == 0x4E ) && \
             ( DirPtr[2] == 0x44 ) && \
             ( DirPtr[3] == 0x58 ) ) 
        {
            //
            // Check if fixup is needed, and apply it if so
            //
            Status = ApplyFixup ( gVolume, DirPtr );
            if ( EFI_ERROR(Status) ) {
                return FALSE;
            }

            IndexSize = ( (INDEX_RECORD*)DirPtr )->INDR_IndxEntrySize;
            IndexSize2 = IndexSize;
            Offset += ((INDEX_RECORD*)DirPtr)->INDR_IndxEntryOff + \
                        EFI_FIELD_OFFSET(INDEX_RECORD, INDR_IndxEntryOff);
            TmpPtr = DirPtr;
            TmpPtr += Offset; // Point to first entry in index
            if (IndexSize < MINIMUM_ENTRY_SIZE) { // Empty index
                return FALSE;
            }
        } else return FALSE; // no index found

        do { // loop inside the index
            EntrySize = ((INDEX_ENTRY*)TmpPtr)->INDE_EntrySize;
            if (EntrySize == 0) {
                return FALSE; // This would cause endless loop
            }
            LfnSize = ((INDEX_ENTRY*)TmpPtr)->INDE_NameLength;

            NamePtr = &((INDEX_ENTRY*)TmpPtr)->INDE_Name[0];
// Insert name correction here
#if NTFS_NAME_CORRECTION
            MemCpy( TmpName16, NamePtr, (LfnSize*2) ); // Get a copy of the name
            Status = GetFileRecord (PeiServices, gVolume, gBs,
                        ((INDEX_ENTRY*)TmpPtr)->INDE_MFTRecord,
                        TmpBuf, NULL);
            if ( EFI_ERROR(Status) )
            {
                goto UseIndexName;
            }
            Status = GetFrAttribute (TmpBuf, 
                                     FR_ATTRIBUTE_NAME,
                                     &TmpPtr2);
            if ( EFI_ERROR(Status) )
            {
                goto UseIndexName;
            }
            TmpPtr3 = TmpPtr2;
            TmpPtr2 += \
                ((FR_ATTR_HEADER_RES*)TmpPtr2)->AHR_InfoOffset;
            //
            // If the original name was a long name, get the long name from the MFT.
            //
            DosName = FALSE;
            if ( (((INDEX_ENTRY*)TmpPtr)->INDE_NameType != 2) && \
                 (((FR_NAME_ATTRIBUTE*)TmpPtr2)->NA_NameType == 2) )
            { // First name found was DOS name, get next one.
                TmpPtr3 += \
                    ((FR_ATTR_HEADER_RES*)TmpPtr3)->AHR_Length;
                if (TmpPtr3[0] != FR_ATTRIBUTE_NAME)
                { // No long name is there, use short one.
                    TmpPtr3 = TmpPtr2;
                    DosName = TRUE;
                }
                TmpPtr2 = TmpPtr3;
                TmpPtr2 += \
                    ((FR_ATTR_HEADER_RES*)TmpPtr2)->AHR_InfoOffset;
            }
            LfnSize = \
                ((FR_NAME_ATTRIBUTE*)TmpPtr2)->NA_NameLength;
            NamePtr = \
                &((FR_NAME_ATTRIBUTE*)TmpPtr2)->NA_Name[0];
            TmpName16[LfnSize*2] = 0; // Zero-terminate name
            FixName (TmpName16, NamePtr, DosName);
UseIndexName:
#endif
            for ( i=0; i<LfnSize; i++ )
            {
#if NTFS_NAME_CORRECTION
                TmpFileName[i] = (CHAR8)(CHAR16)TmpName16[i];
#else
                TmpFileName[i] = (CHAR8)(CHAR16)NamePtr[i];
#endif
            }
            TmpFileName[i] = 0; // Zero-terminate name

            if(!EFI_ERROR(FileSearch((CHAR8*)FileName, TmpFileName, FALSE, LfnSize))) {
                *IndxPtr = (INDEX_ENTRY*)&TmpPtr[0];
                PEI_TRACE((-1, PeiServices, "\nNon-resident file found: %s\n", FileName));
                return TRUE;
            }

            TmpPtr += EntrySize;
            IndexSize -= EntrySize;
            if ( IndexSize < MINIMUM_ENTRY_SIZE ) break;

        } while (IndexSize);
        if ( IndexSize2 < 0x1000 ) {
            IndexSize2 = 0x1000;
        } else {
            IndexSize2 = (IndexSize2 + 0x100) & 0xffffff00 ; // Round off
        }
        DirPtr += IndexSize2;
        DirSize -= IndexSize2;

    } while (DirSize);
    return FALSE;
}

/**
    Gets a list of valid recovery files from an NTFS volume.
    As currently written, gets only one file.


    @param Root - Pointer to a buffer containing the root directory
    @param RootSize - Size of the root directory
    @param NumberOfFiles - Pointer to number of files found
    @param Buffer - Pointer to buffer containing index entry of the file that was found.

         
    @retval None - returned in variables.


    @note  
        This is an e-linked function, which can be replaced.

**/
VOID 
AmiGetFileListFromNtfsVolume (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN  UINT8               *Root,
    IN  UINT32              RootSize,
    OUT UINTN               *NumberOfFiles,
    OUT INDEX_ENTRY         **Buffer
)
{
    INDEX_ENTRY *IndxPtr = NULL;
    VOID *FileName;
    UINTN FileSize;
    EFI_STATUS Status;
    CHAR8 *FilePath;
    CHAR8 *DirPtr;
    CHAR8 LocalPath[256];
#if SEARCH_PATH
    CHAR8 *NextName;
    UINT64 MFTRecord;
#endif

    *NumberOfFiles = 0;     //no files found yet
    Status = GetRecoveryFileInfo(PeiServices, &FileName, &FileSize, NULL);
    if( EFI_ERROR(Status) ) {
        PEI_TRACE((-1, PeiServices, "\nGet info error\n"));
        return;
    }
#if SEARCH_PATH
    AddRecoveryPath( &FileName );
#endif

    MemCpy( LocalPath, FileName, Strlen( FileName )+1 );
    FilePath = &LocalPath[0];
    DirPtr = Root;

#if SEARCH_PATH
    NextName = FilePath;

    PEI_TRACE((-1, PeiServices, "\nTotal path is %s\n", FilePath));
    while ( IsolateName (&FilePath, &NextName) == FALSE ) {
        PEI_TRACE((-1, PeiServices, "\nLooking for %s\n", FilePath));
        //
        //Find path name in directory
        //
        if ( FindFileInNtfsDirectory( PeiServices, DirPtr, RootSize, FilePath, &IndxPtr )) {
            MFTRecord = ((INDEX_ENTRY*)IndxPtr)->INDE_MFTRecord;
            PEI_TRACE((-1, PeiServices, "\nFound directory in path, record no. %lx\n", MFTRecord));
            Status = ReadNTFSDirectory( PeiServices, gVolume, gBs, MFTRecord, DirPtr );
            if ( EFI_ERROR(Status )) {
                PEI_TRACE((-1, PeiServices, "\nDirectory not read\n"));
                return; // not found
            }
            FilePath = NextName;
        } else return; // not found
    } // end while
    //
    // End of path found, now find the file.
    //
    if ( FindFileInNtfsDirectory( PeiServices, DirPtr, RootSize, FilePath, &IndxPtr )) {
        Buffer[*NumberOfFiles] = IndxPtr; // Save pointer to this entry
        *NumberOfFiles = 1;
        PEI_TRACE((-1, PeiServices, "\nRecovery file found\n"));
        return;
    }
    PEI_TRACE((-1, PeiServices, "\nRecovery file not found\n"));
    return;
#else
    if ( FindFileInNtfsDirectory( PeiServices, DirPtr, RootSize, FilePath, &IndxPtr )) {
        Buffer[*NumberOfFiles] = IndxPtr; // Save pointer to this entry
        *NumberOfFiles = 1;
        PEI_TRACE((-1, PeiServices, "\nRecovery file found\n"));
        return;
    }
    PEI_TRACE((-1, PeiServices, "\nRecovery file not found\n"));
    return;
#endif
}

/**
    Reads recovery capsule from NTFS device. Tries to discover primary partitions
    and search for capsule there.

        
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FileName - recovery capsule file name
    @param FileSize - pointer to size of provided buffer
    @param Buffer - pointer to buffer to store data


    @retval EFI_STATUS

**/
EFI_STATUS 
ProcessNTFSDevice (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN OUT UINTN         *FileSize,
    OUT VOID             *Buffer 
)
{
    EFI_STATUS          Status;
    EFI_STATUS          Status2;

    //
    // Assume the volume is floppy-formatted.
    //
    Volume->PartitionOffset = 0; // Reset this to zero
    Status = ProcessNTFSVolume( PeiServices, Volume, FileSize, Buffer );

    if ( !EFI_ERROR( Status )) {
        return Status;
    }

    //
    // Not floppy formatted, look for partitions. Read sector 0 (MBR).
    //
    Status = ReadDevice( PeiServices, Volume, 0, 512, &MemBlk->Mbr );

    if ( EFI_ERROR( Status )) {
        PEI_TRACE((-1, PeiServices, "\nRead MBR failed\n"));
        return Status;
    }

    if ( MemBlk->Mbr.Sig != 0xaa55 ) {
        PEI_TRACE((-1, PeiServices, "\naa55 not found\n"));
        return EFI_NOT_FOUND;
    }

    PartCount = 0;
    PartSector = 0;
    ExtOffset = 0;
    IsMbr = TRUE;

    //
    // Locate all partitions. Check each one for the recovery file.
    // The recovery file will be loaded if it is found, and this
    // function will return EFI_SUCCESS.
    //
    do {
        Status = FindNextPartition( PeiServices, Volume );
        if ( !EFI_ERROR(Status) ) {
            Status2 = ProcessNTFSVolume( PeiServices, Volume, FileSize, Buffer );
            if ( !EFI_ERROR(Status2) ) {
                return Status2;
            }
        }
    } while ( Status == EFI_SUCCESS );

    return EFI_NOT_FOUND;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
