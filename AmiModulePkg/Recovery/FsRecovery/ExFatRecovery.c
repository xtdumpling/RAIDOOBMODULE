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

/** @file ExFatRecovery.c

**/

//----------------------------------------------------------------------

#include <Token.h>
#include <AmiPeiLib.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Ppi/BlockIo.h>
#include <Guid/AmiRecoveryDevice.h>

#include "ExFatRecovery.h"
#include "FsRecovery.h"

//----------------------------------------------------------------------

DIR_ENTRY_EX        *ExFatRecoveryFiles[10];
UINT32              InodeBlock;     // First block of inode table
UINT8               Indirect;
UINT32              *BlockList;
UINT32              *BlockList2;

extern UINTN               PartCount;
extern BOOLEAN             IsMbr;
extern UINT32              GpeCount;
extern UINT32              PartSector;
extern UINT32              ExtOffset;
extern UINT8               *ReadBuffer;
extern UINT8               *RootBuffer;
extern UINTN               RootBufferSize ;
extern UINT32              RootEntries ;
extern UINT32              RootSize;
extern MEMORY_BLOCK        *MemBlk;
#if SEARCH_PATH
RC_VOL_INFO                *gVolume;
#endif

/**
    Checks if given data block describes exFAT structure

    @param pBS - pointer to data block to check

               
    @retval TRUE - data block is a exFAT structure
    @retval FALSE - data block is not a exFAT structure

**/
BOOLEAN 
IsExFat (
    IN BOOT_SECTOR_EX *pBS 
)
{
    return pBS->BS_BPS >= 9
           && pBS->BS_BPS <= 12
           && (UINT64)pBS->BS_MBZ[0] == 0
           && pBS->BS_OEMName[0] == 'E'
           && pBS->BS_OEMName[1] == 'X'
           && pBS->BS_OEMName[2] == 'F'
           && pBS->BS_OEMName[3] == 'A'
           && pBS->BS_OEMName[4] == 'T';
}

/**
    Reads a file from exFAT device

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FirstCluster - starting cluster
    @param Size - size of the data to read
    @param Contiguous - true if file is contiguous
    @param Buffer - pointer to buffer to store data

               
    @retval EFI_SUCCESS - correctly read all FAT data
    @retval EFI_ABORTED - should never get this.
    @retval Other any errors reported from ReadDevice function

**/
EFI_STATUS 
ReadExFatFile (
    IN EFI_PEI_SERVICES   **PeiServices,
    IN RC_VOL_INFO        *Volume,
    IN UINT32             FirstCluster,
    IN UINTN              Size,
    IN BOOLEAN            Contiguous,
    OUT VOID              *Buffer 
)
{
    EFI_STATUS  Status;
    UINT32      Count;
    UINT32      NextCluster;
    UINT32      Cluster = FirstCluster;
    UINTN       SizeToRead;

    //
    // In exFAT, a file will have a marker in it's directory entry indicating
    // whether it is contiguous or not. If it is contiguous, the FAT is not needed.
    //
    if ( Contiguous )
    {
        Status     = ReadDevice( PeiServices,
                                 Volume,
                                 Volume->DataOffset + Mul64((UINT64)(Cluster - 2), Volume->BytesPerCluster),
                                 Size,
                                 Buffer );
        if ( EFI_ERROR(Status) ) {
            PEI_TRACE((-1, PeiServices, "\nRead status is %r\n", Status));
        }      
        return Status;
    } else {
    //
    // File is not contiguous, use the FAT.
    //
        while ( TRUE )
        {
            SizeToRead = Size;
            Count      = GetClustersCount( exFAT, Cluster, &NextCluster, TRUE );
            SizeToRead = ( SizeToRead > Count * Volume->BytesPerCluster ) ? Count * Volume->BytesPerCluster : SizeToRead;
            Status     = ReadDevice( PeiServices,
                                     Volume,
                                     Volume->DataOffset + Mul64((UINT64)(Cluster - 2), Volume->BytesPerCluster),
                                     SizeToRead,
                                     Buffer );

            if ( EFI_ERROR( Status ) || NextCluster == 0 ) {
                return Status;
            }

            Cluster         = NextCluster;
            Buffer = (UINT8*)Buffer + SizeToRead;
            Size           -= SizeToRead;
        }
        return EFI_ABORTED;     // should never get here
    }
}

/**
    Prepares given volume for read operations. Reads exFAT root directory.

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param Bs - pointer to MBR

               
    @retval EFI_STATUS - possible return values 

**/
EFI_STATUS 
ReadExFatRoot (
    IN EFI_PEI_SERVICES **PeiServices,
    IN OUT RC_VOL_INFO  *Volume,
    IN BOOT_SECTOR_EX   *Bs 
)
{
    EFI_STATUS           Status;
    UINT32               FatSectors;
    UINT32               RootSize;
    UINT32               FatSize;
    UINT32               RootClusters;
    UINT32               DummyCluster;
    UINT32               BytesPerSector;
    UINT32               SectorsPerCluster;
    UINTN                RootPages;
    EFI_PHYSICAL_ADDRESS Allocate;

    DummyCluster = 0;
    BytesPerSector = 1 << Bs->BS_BPS;
    SectorsPerCluster = ( BytesPerSector << Bs->BS_SPC ) / BytesPerSector;

    FatSectors   = Bs->BS_FATSize;
    FatSize      = FatSectors * BytesPerSector;

    Volume->FatOffset       = Mul64( Bs->BS_FATOff, BytesPerSector );
    Volume->DataOffset      = Mul64( Bs->BS_DataOff, BytesPerSector );
    Volume->RootOffset      = Volume->DataOffset + Mul64( (Bs->BS_RootCluster - 2) * SectorsPerCluster, BytesPerSector );
    Volume->BytesPerCluster = BytesPerSector << Bs->BS_SPC;
    Volume->FatType         = exFAT;

    //
    // Init FAT table
    //
    Status = InitFat( PeiServices, Volume, FatSize );
    if( EFI_ERROR(Status) ) {
        return Status;
    }

    //
    // Read Root directory
    //
    RootClusters = GetClustersCount( exFAT, Bs->BS_RootCluster, &DummyCluster, FALSE );
    RootSize    = RootClusters * Volume->BytesPerCluster;
    RootEntries = RootSize / 32;

    RootPages = EFI_SIZE_TO_PAGES( RootSize );

    if ( RootBufferSize < EFI_PAGES_TO_SIZE( RootPages )) {
        Status = (*PeiServices)->AllocatePages( PeiServices, EfiBootServicesData, RootPages, &Allocate );
        if ( EFI_ERROR( Status )) {
            PEI_TRACE((-1, PeiServices, "\nAllocate for root pages failed\n"));
            return Status;
        }

        RootBuffer     = (UINT8*)((UINTN)Allocate);
        RootBufferSize = EFI_PAGES_TO_SIZE( RootPages );
    }
    MemSet( RootBuffer, RootBufferSize, 0 );

    //
    // There is no indicator of whether the root directory is contiguous or not, 
    // so we will assume it is not.
    //
    Status = ReadExFatFile( PeiServices, Volume, Bs->BS_RootCluster, RootSize, FALSE, RootBuffer );

    return Status;

}

/**
    Reads recovery capsule from exFAT volume

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FileName - recovery capsule file name
    @param FileSize - pointer to size of provided buffer
    @param Buffer - pointer to buffer to store data

    @retval EFI_STATUS

**/
EFI_STATUS 
ProcessExFatVolume (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN OUT UINTN         *FileSize,
    OUT VOID             *Buffer 
)
{
    EFI_STATUS           Status;
    UINT32               i;
    UINT32               FirstFileCluster;
    UINTN                NumberOfFiles;
    DIR_ENTRY_EX         *DirPtr;
    BOOLEAN              Contiguous;
    UINTN                LocalFileSize;
    BOOT_SECTOR_EX       *BsPtr;

    Status = ReadDevice( PeiServices, Volume, 0, 512, &MemBlk->Bs );

    if ( EFI_ERROR( Status )) {
        PEI_TRACE((-1, PeiServices, "\nRead boot sector failed\n"));
        return Status;
    }

    BsPtr = ( BOOT_SECTOR_EX* )&MemBlk->Bs;
    if (!IsExFat( BsPtr )) {
        return EFI_NOT_FOUND;
    }

#if SEARCH_PATH
    gVolume = Volume;
#endif
    
    Status = ReadExFatRoot( PeiServices, Volume, BsPtr );

    if ( EFI_ERROR( Status )) {
        PEI_TRACE((-1, PeiServices, "\nRead root failed\n"));
        return Status;
    }

    AmiGetFileListFromExFatVolume(PeiServices, (DIR_ENTRY_EX*)RootBuffer, RootEntries, &NumberOfFiles, ExFatRecoveryFiles);
    if ( NumberOfFiles == 0 )
        return EFI_NOT_FOUND;
    PEI_TRACE((-1, PeiServices, "\nFound a recovery file\n"));

    //
    // When files are saved in ExFatRecoveryFiles, only the Info part of
    // each directory entry is saved, so that this loop will work without
    // concern for the actual directory entry size (which is variable in
    // exFAT).
    //
    for( i = 0; i < (NumberOfFiles); i++ ) {
        DirPtr = (DIR_ENTRY_EX*)ExFatRecoveryFiles[i];
        LocalFileSize = (UINTN)DirPtr->EType.Info.DE_Size;
        if ( *FileSize < LocalFileSize ) {
            PEI_TRACE((-1, PeiServices, "\nFile size is wrong, %x\n", LocalFileSize));
            continue;
        }

        FirstFileCluster = DirPtr->EType.Info.DE_Cluster;
        //
        // Find out whether file is contiguous or not. Fat will not be used
        // if the file is contiguous.
        //
        Contiguous = (DirPtr->EType.Info.DE_Flags & 0x02) ? TRUE : FALSE;

        Status = ReadExFatFile( PeiServices, Volume, FirstFileCluster, LocalFileSize, Contiguous, Buffer );
        if(EFI_ERROR(Status)) {
            PEI_TRACE((-1, PeiServices, "\nError reading recovery file\n"));
            continue;
        }

        if(AmiIsValidFile(Buffer, LocalFileSize)) {
            *FileSize = LocalFileSize;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}


/**
    Reads recovery capsule from exFAT device. First treat device as 
    non-partitioned device. If failed tries to discover primary partitions and 
    search for capsule there.

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FileName - recovery capsule file name
    @param FileSize - pointer to size of provided buffer
    @param Buffer - pointer to buffer to store data

    @retval EFI_STATUS

**/
EFI_STATUS 
ProcessExFatDevice (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN OUT UINTN         *FileSize,
    OUT VOID             *Buffer 
)
{
    EFI_STATUS          Status;
    EFI_STATUS          Status2;

    //
    // Assume the volume is floppy-formatted
    //
    Volume->PartitionOffset = 0; // Reset this to zero
    Status = ProcessExFatVolume( PeiServices, Volume, FileSize, Buffer );

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
        return EFI_NOT_FOUND;
    }

    IsMbr = TRUE;
    PartCount = 0;
    PartSector = 0;
    ExtOffset = 0;

    //
    // Locate all partitions. Check each one for the recovery file.
    // The recovery file will be loaded if it is found, and this
    // function will return EFI_SUCCESS.
    //
    do {
        Status = FindNextPartition( PeiServices, Volume );
        if ( !EFI_ERROR(Status) ) {
            Status2 = ProcessExFatVolume( PeiServices, Volume, FileSize, Buffer );
            if ( !EFI_ERROR(Status2) ) {
                return Status2;
            }
        }
    } while (Status == EFI_SUCCESS);

    return EFI_NOT_FOUND;
}


/**
    Find the specified file in the exFAT directory at DirPtr

    @param DirPtr - Pointer to a buffer containing the directory
    @param DirSize - Size of the directory
    @param FileName - Name of the file to find
    @param EntryPtr - Pointer to buffer containing directory entry of the file that was found.

         
    @retval TRUE - File found. FALSE - File not found

**/
BOOLEAN 
FindFileInExFatDirectory (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN  UINT8               *DirPtr,
    IN  UINT32              DirEntries,
    IN  VOID                *FileName,
    OUT DIR_ENTRY_EX        **EntryPtr
)
{
    DIR_ENTRY_EX            *TmpPtr;
    DIR_ENTRY_EX            *InfoPtr;
    CHAR16                  *NamePtr;
    UINT8                   NameLength;
    UINT16                  Attributes;
    UINT32                  i, ii;
    UINT8                   j, k, nl;
    CHAR8                   TmpFileName[128];
#if MATCH_VOLUME_NAME
    CHAR8 *VolumeName = CONVERT_TO_STRING(VOLUME_NAME);
#endif

    TmpPtr = (DIR_ENTRY_EX*)&DirPtr[0];

    for( i = 0; i < DirEntries; i++ ) {

        if( TmpPtr[i].DE_Type == 0 ) {
            break; // End of directory
        }

        if(((TmpPtr[i].DE_Type) & 0x80) == 0) {
            continue; // Deleted file, skip it
        }

#if MATCH_VOLUME_NAME
        if ( TmpPtr[i].DE_Type == 0x83 ) { // If volume name found...
            NamePtr = &TmpPtr[ii].EType.VName.DE_VolName[0]; // Point to name
            NameLength = TmpPtr[ii].EType.VName.DE_NameLength;
            for ( j=0; j<NameLength; j++ ) {
                TmpFileName[j] = (UINT8)NamePtr[j];
            }
            TmpFileName[j] = 0; // Terminate name
            if( !(FileCompare(TmpFileName, VolumeName, FALSE, Strlen(VolumeName))) ) {
                PEI_TRACE((-1, PeiServices, "\nName did not match\n"));
                return FALSE;
            }
        }
#endif
        
        //
        // See notes in the definition of DIR_ENTRY_EX for how exFAT file
        // names are stored.
        //
        if ( TmpPtr[i].DE_Type == 0x85 ) { // If normal directory entry found
            ii = i;
            Attributes = TmpPtr[ii].EType.File.DE_Attr; // Save attributes
            ii++;
            if ( TmpPtr[ii].DE_Type == 0xC0 ) { // If Info section
                NameLength = TmpPtr[ii].EType.Info.DE_NameLength;
                //
                // We're only going to save the Info section, so put the
                // attributes in a reserved spot here
                //
                TmpPtr[ii].EType.Info.DE_Reserved2 = Attributes;
                nl = NameLength;
                InfoPtr = &TmpPtr[ii]; // Save ptr to this section
                k = 0;
                ii++;
            }
            while ( TmpPtr[ii].DE_Type == 0xC1 ) { // If Name section
                NamePtr = &TmpPtr[ii].EType.Name.DE_Name[0]; // Point to name
                ii++;
                //
                // exFAT names are unicode, so we have to build an 8-bit name
                // for matching.
                //
                for ( j=0; j<15; j++, k++ ) {
                    TmpFileName[k] = (UINT8)NamePtr[j];
                    nl--;
                    if ( nl == 0 ) { // If end of name...
                        j = 15; // Cause loop to drop out
                        TmpFileName[k+1] = 0;
                        //
                        // Complete name found, see if it matches
                        //
                        if( !EFI_ERROR(FileSearch((CHAR8*)FileName, 
                                                 TmpFileName, 
                                                 FALSE, 
                                                 NameLength)) ) {
                            *EntryPtr = InfoPtr; // Save pointer to this entry
                            return TRUE;
                        }
                    }
                }
            }
        }
    }

    return FALSE;
}


/**
    Gets a list of valid recovery files from an exFAT volume.
    As currently written, gets only one file.

    @param Root - Pointer to a buffer containing the root directory
    @param RootEntries - Number of root entries
    @param NumberOfFiles - Pointer to number of files found
    @param Buffer - Pointer to buffer containing entry of the file that was found.

         
    @retval None - returned in variables.

    @note  
        This is an e-linked function, which can be replaced.

**/
VOID 
AmiGetFileListFromExFatVolume (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN  DIR_ENTRY_EX        *Root,
    IN  UINT32              RootEntries,
    OUT UINTN               *NumberOfFiles,
    OUT DIR_ENTRY_EX        **Buffer
)
{
    DIR_ENTRY_EX *EntryPtr = NULL;
    VOID *FileName;
    UINTN FileSize;
    EFI_STATUS Status;
    CHAR8 *FilePath;
    UINT8 *DirPtr;
    UINT32 EntryCount = RootEntries;
    CHAR8 LocalPath[256];
#if SEARCH_PATH
    CHAR8 *NextName;
    UINT32 Cluster;
    BOOLEAN Contiguous;
#endif

    *NumberOfFiles = 0;     //no files found yet

    Status = GetRecoveryFileInfo( PeiServices, &FileName, &FileSize, NULL );
    if(EFI_ERROR(Status))
        return;
#if SEARCH_PATH
    AddRecoveryPath(&FileName);
#endif

    MemCpy( LocalPath, FileName, Strlen(FileName)+1 );
    FilePath = &LocalPath[0];
    DirPtr = (UINT8*)Root;

#if SEARCH_PATH
    NextName = FilePath;

    while ( IsolateName (&FilePath, &NextName) == FALSE ) {
        //
        // Find path name in directory
        //
        if ( FindFileInExFatDirectory(PeiServices, DirPtr, RootEntries, FilePath, &EntryPtr) ) {
            Cluster = EntryPtr->EType.Info.DE_Cluster;
            Contiguous = EntryPtr->EType.Info.DE_Flags ? TRUE : FALSE;
            FileSize = (UINTN)EntryPtr->EType.Info.DE_Size;
            if ( FileSize > RootBufferSize ) FileSize = RootBufferSize;
            EntryCount = FileSize / 32;

            Status = ReadExFatFile( PeiServices, gVolume, Cluster, FileSize, Contiguous, RootBuffer );

            if ( EFI_ERROR(Status) ) {
                PEI_TRACE((-1, PeiServices, "\nDirectory not read\n"));
                return; // not found
            }
            FilePath = NextName;
        } else return; // not found
    } // end while

    if ( FindFileInExFatDirectory(PeiServices, DirPtr, EntryCount, FilePath, &EntryPtr )) {
        Buffer[*NumberOfFiles] = EntryPtr; // Save pointer to this entry
        *NumberOfFiles = 1;
    }
    return;
#else
    if ( FindFileInExFatDirectory( PeiServices, DirPtr, EntryCount, FilePath, &EntryPtr )) {
        Buffer[*NumberOfFiles] = EntryPtr; // Save pointer to this entry
        *NumberOfFiles = 1;
    }
    return;
#endif

}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
