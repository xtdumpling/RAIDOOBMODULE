//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file FsRecovery.c
    

**/

//----------------------------------------------------------------------

#include <AmiPeiLib.h>
#include <Token.h>
#include "FsRecovery.h"

#include <Ppi/DeviceRecoveryModule.h>
#include <Ppi/BlockIo.h>

#include <Guid/AmiRecoveryDevice.h>
#include <FsRecoveryElinks.h>

//----------------------------------------------------------------------

#define FAT_FILE_NAME_SIZE  11
#define FAT_ENTIRE_SIZE     0x20000  // to fit FATA12 and FAT16 tables entirely
#define FAT_CACHE_SIZE      0x4000      

typedef struct {
    UINT8 *FatBuffer;
    UINT32 FatBufferSize;
    UINT32 FirstCluster;
    UINT32 ClusterCount;
    UINT32 MaxClusterCount;
} FAT_WINDOW;

static FAT_WINDOW *FatWindow = NULL;

UINT8               *ReadBuffer    = NULL;
UINTN               BufferSize     = 0;
UINT8               *FatBuffer     = NULL;
UINTN               FatBufferSize  = 0;
UINT8               *RootBuffer    = NULL;
UINTN               RootBufferSize = 0;
UINT32              RootEntries    = 0;
UINT32              RootSize       = 0;
UINTN               PartCount;
BOOLEAN             IsMbr;
UINT32              GpeCount;
UINT32              PartSector;
UINT32              ExtOffset;
MEMORY_BLOCK        *MemBlk;
DIR_ENTRY           *FatRecoveryFiles[10];
DIR_RECORD          *CdRecoveryFiles[10];
EFI_PEI_SERVICES    **ThisPeiServices;
RC_VOL_INFO         *ThisVolume;
BOOLEAN             RootAllocated = FALSE;
BOOLEAN             gIntegrityCheck = FALSE;

EFI_PEI_BLOCK_DEVICE_TYPE    gRecoveryDeviceType;

extern OEM_RECOVERY_VOLUME_INTEGRITY_CALLBACK OEM_RECOVERY_VOLUME_INTEGRITY_CALLBACK_LIST EndOfList;
OEM_RECOVERY_VOLUME_INTEGRITY_CALLBACK* OemRecoveryVolumeIntegrityCallback[] = {OEM_RECOVERY_VOLUME_INTEGRITY_CALLBACK_LIST NULL};

extern FsRecovery_Devices FSRECOVERY_LIST EndOfFsRecoverySupport;
FsRecovery_Devices *FsRecoverySupport[] = {
        FSRECOVERY_LIST NULL
};

#if SEARCH_PATH
const CHAR8 *RecoveryPath = CONVERT_TO_STRING(RECOVERY_PATH);
CHAR8 BuiltRecoveryName[256];
#endif
 
/**
    Converts lower case characters to upper case

               
    @param c - character to convert

               
    @retval CHAR8 - converted character value

**/
CHAR8 
toupper (
    IN CHAR8 c 
)
{
    return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}


/**
    This function takes a filename and a filename pattern and tries to make 
    a match.  It can account for Wild characters * and ? in the pattern. Also,
    the pattern can be multiple comma separated patterns.

               
    @param RecoveryFileName - recover file pattern string comma separated
    @param FsFilename - file name to check against the pattern string
    @param IgnoreSpacesInFilename - If true, ignore spaces in FsFilename when pattern string is a period

               
    @retval TRUE - Pattern matches filename
    @retval FALSE - Pattern doesn't match filename


    @note  
      RecoveryFileNamePattern is taken from the RECOVERY_ROM token and should look
      like this:   *.rom,ab??rt??.bio,123.bin  etc.
      The Parameter IgnoreSpacesInFilename is for use with file systems that pad
      spaces into filenames and don't use periods.  If TRUE, it matches a period 
      in the pattern to any number of spaces in the filename.
**/
BOOLEAN 
FileCompare (
    IN CHAR8    *RecoveryFileNamePattern, 
    IN CHAR8    *FsFilename, 
    IN BOOLEAN  IgnoreSpacesInFilename,
    IN UINT32   FileNameLength
)
{
    CHAR8  ch1, ch;
    UINTN len1, len2;

    for ( ; ; ) {
        //
        // Check length of string
        //
        len1 = Strlen(FsFilename);
        len2 = Strlen(RecoveryFileNamePattern);

        //
        // if len1 is 0 then at end of filename
        //
        if( !len1 || !FileNameLength )
        {
            //
            // if len2 is 0 then also at end of pattern so file name is 
            // equal to a pattern
            //
            if( !len2 )
                return TRUE;
            else
            {
                //
                // if len2 is a comma, then it is the same as len2 == 0 so 
                // file name is equal to a pattern
                //
                ch1 = *RecoveryFileNamePattern;
                if ( ch1 == ',' )
                {
                    return TRUE;
                }
                //
                // if not a comma or 0, then file does not fit the pattern
                //
                return FALSE;
            }
        }

        //
        // get next character of the pattern
        //
        ch1 = *RecoveryFileNamePattern;
        RecoveryFileNamePattern ++;

        switch ( ch1 ) 
        {
            case ',':
                return TRUE;
                break;

            case 0: // reached string terminator
                return TRUE;
                break;

            //
            // wild character, it must deal with any number of matching characters 
            // in the file name string
            //
            case '*':                               
                while ( *FsFilename ) 
                {
                    if ( FileCompare ( RecoveryFileNamePattern, FsFilename, IgnoreSpacesInFilename, FileNameLength ) ) 
                    {
                        return TRUE;
                    }        
                    FsFilename ++;
                    FileNameLength--;
                }
                return FileCompare ( RecoveryFileNamePattern, FsFilename, IgnoreSpacesInFilename, FileNameLength );
                break;

            //
            // wild character, it must deal with a matching character in the file name string
            //
            case '?':                               
                if ( !*FsFilename ) 
                {
                    return FALSE;
                }

                FsFilename ++;
                FileNameLength--;
                break;

            //
            // in this case statement the case '.' must be directly above the default case.
            // if IgnoreSpacesInFilename is FALSE, it is supposed to fall through into default.
            // If IgnoreSpacesInFilenameis TRUE, process the period as a check for spaces character.
            // then once we skip over all spaces, if there are any, then it moves to the 
            // next character in the pattern
            //
            case '.':
                //
                // FAT, spaces added to file name to make 8 3  -- no period in the filename either
                //
                if ( IgnoreSpacesInFilename == TRUE )  
                {
                    ch = *FsFilename;

                    while (( ch == ' ' ) && ( ch != 0 ))
                    {
                        FsFilename ++;
                        FileNameLength--;
                        ch = *FsFilename;
                    }
                    break;
                }
                //
                // CDFS, no spaces and there is a period.  Let it fall through to the default case
                //
            default:
                ch = *FsFilename;
                if ( toupper( ch ) != toupper( ch1 ) ) 
                {
                    return FALSE;
                }
                FsFilename ++;
                FileNameLength--;
                break;
        }
    }        
}


/**
    Wrapper function for parsing through a pattern string with multiple entries
    Each entry is comma separated and can include wild characters like * and ?
    The Function can handle CDFS and FAT file systems.  

               
    @param RecoveryFileName - recover file pattern string comma separated
    @param FsFilename - file name to check against the pattern string
    @param IgnoreSpacesInFilename - If true, ignore spaces in FsFilename when pattern string is a period

               
    @retval EFI_SUCCESS - File name fits one of the Pattern strings in RecoveryFileName
    @retval EFI_INVALID_PARAMETER - one of the strings is NULL or file name doesn't fit pattern string

**/
EFI_STATUS 
FileSearch (
    IN CHAR8    *RecoveryFileName, 
    IN CHAR8    *FsFilename, 
    IN BOOLEAN  IgnoreSpacesInFilename,
    IN UINT32   FileNameLength 
)
{
    CHAR8 *RecStrPtr = (CHAR8 *)RecoveryFileName;
    CHAR8 *FilenamePtr = FsFilename;

    if ( *RecStrPtr == 0 ) 
    {
        return EFI_INVALID_PARAMETER;
    }

    if ( *FsFilename == 0 )
    {
        return EFI_INVALID_PARAMETER;
    }   

    if ( FileNameLength == 0 ) 
    {
        return EFI_INVALID_PARAMETER;
    }   
   

    //
    // loop until all possibilities listed in the RecoveryFileName are exhausted
    //
    do {
        //
        // Now compare the current possiblity to the current filename
        //
        FilenamePtr = FsFilename;

        if ( *RecStrPtr == ',' ) 
            RecStrPtr++;

        if ( FileCompare( RecStrPtr, FsFilename, IgnoreSpacesInFilename, FileNameLength ) == TRUE )
            return EFI_SUCCESS;
        
        while ( *RecStrPtr != ',' && *RecStrPtr != 0 )
            RecStrPtr ++;

    } while ( *RecStrPtr != 0 );

    return EFI_INVALID_PARAMETER;
}


/**
    Converts file name from "XXXXX.XXX" form to FAT form

               
    @param inFileName - pointer to input file name
    @param outFileName - pointer to output file name


    @retval VOID

**/
VOID 
ConvertToFatFileName (
    IN CHAR8   *inFileName,
    OUT CHAR8  *outFileName 
)
{
    UINT32 i = 0;
    UINT32 j = 0;

    for ( i = 0; inFileName[i] && inFileName[i] != '.'; i++ ) {
        outFileName[i] =     toupper( inFileName[i] );
    }
    j = i;

    for ( ; i < 8; i++ ) {
        outFileName[i] = ' ';
    }

    if ( inFileName[j] == '.' ) {
        for ( j++; inFileName[j]; i++, j++ ) {
            outFileName[i] = toupper( inFileName[j] );
        }
    }

    for ( ; i < 11; i++ ) {
        outFileName[i] = ' ';
    }
    outFileName[i] = 0;
}

/**
    Reads data from Block device

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param Start - starting offset in bytes
    @param Size - size of the data to read
    @param Buffer - pointer to buffer to store data


    @retval EFI_SUCCESS

**/
EFI_STATUS 
ReadDevice (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN RC_VOL_INFO       *Volume,
    IN UINT64            Start,
    IN UINTN             Size,
    OUT VOID             *Buffer 
)
{
    UINT64               StartLba;
    UINTN                ActualSize;
    UINTN                ActualPages;
    UINT32               StartOffset;
    EFI_STATUS           Status;
    EFI_PHYSICAL_ADDRESS Allocate;

    Start += Volume->PartitionOffset;

    if ( Volume->BlockSize == 4096 )  {
        StartLba    = Shr64( Start, 12 );
        StartOffset = (UINT32)( Start & 0xfff );
    } else if ( Volume->BlockSize == 2048 )  {
        StartLba    = Shr64( Start, 11 );
        StartOffset = (UINT32)( Start & 0x7ff );
    } else {
        StartLba    = Shr64( Start, 9 );
        StartOffset = (UINT32)( Start & 0x1ff );
    }

    ActualSize  = ( ( StartOffset + Size + Volume->BlockSize - 1 ) / Volume->BlockSize ) * Volume->BlockSize;
    ActualPages = EFI_SIZE_TO_PAGES( ActualSize );

    if ( BufferSize < EFI_PAGES_TO_SIZE( ActualPages )) {
        Status = (*PeiServices)->AllocatePages( PeiServices, EfiBootServicesData, ActualPages, &Allocate );

        if ( EFI_ERROR( Status )) {
            return Status;
        }

        ReadBuffer = (UINT8*)( (UINTN)Allocate );
        BufferSize = EFI_PAGES_TO_SIZE( ActualPages );
    }

    Status = Volume->BlkIo->ReadBlocks( PeiServices,
                                        Volume->BlkIo,
                                        Volume->Device,
                                        StartLba,
                                        ActualSize,
                                        ReadBuffer );

    if ( !EFI_ERROR( Status )) {
        MemCpy( Buffer, ReadBuffer + StartOffset, Size );
    }

    return Status;
}

/**
    Checks if given data block describes FAT structure


    @param pBpb - pointer to data block to check

               
    @retval TRUE - data block is a FAT structure
    @retval FALSE - data block is not a FAT structure

**/
BOOLEAN 
IsFat (
    IN BOOT_SECTOR  *pBpb 
)
{
    return pBpb->BytsPerSec <= 4096
           && pBpb->SecPerClus && pBpb->SecPerClus <= 128
           && pBpb->RsvdSecCnt
           && pBpb->NumFATs
           && pBpb->Signature == 0xAA55
           && (pBpb->jmp[0] == 0xEB || pBpb->jmp[0] == 0xE9);
}

/**
    Re-reads FAT table so requested cluster is in cache

               
    @param Cluster - cluster number

               
    @retval EFI_SUCCESS - operation completed successfully

**/
EFI_STATUS 
ShiftFatWindow (
    IN UINT32  Cluster
)
{
    EFI_STATUS Status;

    if( Cluster + FatWindow->ClusterCount >= FatWindow->MaxClusterCount )
        Cluster = FatWindow->MaxClusterCount - FatWindow->ClusterCount;

    Status = ReadDevice(ThisPeiServices, ThisVolume, ThisVolume->FatOffset + (Cluster << 2), FatWindow->FatBufferSize, FatWindow->FatBuffer);
    if( EFI_ERROR( Status ) ) {
        return Status;
    }

    FatWindow->FirstCluster = Cluster;
    return EFI_SUCCESS;
}

/**
    Initializes FAT table cache

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FatSize - size of FAT table

               
    @retval EFI_SUCCESS - operation completed successfully

**/
EFI_STATUS 
InitFat (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN RC_VOL_INFO       *Volume,
    IN UINT32            FatSize
)
{
    EFI_STATUS Status;
    UINTN FatPages;
    EFI_PHYSICAL_ADDRESS Allocate;

    if(FatWindow == NULL) {
        Status = (*PeiServices)->AllocatePool( PeiServices, sizeof(FAT_WINDOW), (VOID**)&FatWindow );
        if(EFI_ERROR(Status)) {
            return Status;
        }

        FatPages = EFI_SIZE_TO_PAGES( FAT_ENTIRE_SIZE );
    
        Status = (*PeiServices)->AllocatePages( PeiServices, EfiBootServicesData, FatPages, &Allocate );
        if( EFI_ERROR(Status) ) {
            FatWindow = NULL;
            return Status;
        }
        FatWindow->FatBuffer     = (UINT8*)( (UINTN)Allocate );
    }

    FatWindow->FatBufferSize = (FatSize < FAT_ENTIRE_SIZE) ? FatSize : FAT_CACHE_SIZE;

    MemSet(FatWindow->FatBuffer, FatWindow->FatBufferSize, 0);
    Status = ReadDevice(PeiServices, Volume, Volume->FatOffset, FatWindow->FatBufferSize, FatWindow->FatBuffer);
    if( EFI_ERROR(Status) ) {
        return Status;
    }

    FatWindow->FirstCluster = 0;
    FatWindow->ClusterCount = ( FatSize < FAT_ENTIRE_SIZE ) ? 0xffffffff : FAT_CACHE_SIZE >> 2;
    FatWindow->MaxClusterCount = FatSize >> 2;      //used only for FAT32 cached tables
    return EFI_SUCCESS;
}

/**
    Returns number of clusters for given cluster chain

               
    @param FatType - FAT type (FAT12, FAT16 or FAT32)
    @param CurrentCluster - first cluster of cluster chain
    @param NextCluster - first cluster of next cluster chain if there is break
    @param Continuous - if TRUE, returns only number of subsequent clusters in chain
                      - if FALSE, returns total number of clusters in cluster chain

               
    @retval UINT32 - number of clusters

**/
UINT32 
GetClustersCount (
    IN UINT8    FatType,
    IN UINT32   CurrentCluster,
    OUT UINT32  *NextCluster,
    IN BOOLEAN  Continuous 
)
{
    UINT32 Count = 0;
    UINT32 WorkCluster;
    UINT32 Cluster = CurrentCluster;

    if ( FatType == FAT16 ) {
        UINT16 *Fat16 = (UINT16 *)(FatWindow->FatBuffer);
        while ( TRUE )
        {
            Count++;
            WorkCluster = Fat16[Cluster];

            if ( WorkCluster > 0xfff7 ) {
                *NextCluster = 0;
                break;
            }

            if ( WorkCluster != Cluster + 1 && Continuous ) {
                *NextCluster = WorkCluster;
                break;
            }
            Cluster = WorkCluster;
        }
    } else if ( FatType == FAT32 ) {
        UINT32 *Fat32;
        while ( TRUE )
        {
            Count++;
            if( Cluster < FatWindow->FirstCluster || Cluster >= FatWindow->FirstCluster + FatWindow->ClusterCount ) {
                EFI_STATUS Status;
                Status = ShiftFatWindow( Cluster );
                if( EFI_ERROR(Status) )
                    return 0;
            }
            Fat32 = (UINT32 *)( FatWindow->FatBuffer );
            WorkCluster = Fat32[Cluster - FatWindow->FirstCluster] & 0xfffffff;

            if ( WorkCluster > 0xffffff7 ) {
                *NextCluster = 0;
                break;
            }

            if ( WorkCluster != Cluster + 1 && Continuous ) {
                *NextCluster = WorkCluster;
                break;
            }
            Cluster = WorkCluster;
        }
#if EXFATRecovery_SUPPORT
    } else if ( FatType == exFAT ) {
        UINT32 *Fat32;
        while ( TRUE )
        {
            Count++;
            if(Cluster < FatWindow->FirstCluster || Cluster >= FatWindow->FirstCluster + FatWindow->ClusterCount) {
                EFI_STATUS Status;
                Status = ShiftFatWindow( Cluster );
                if(EFI_ERROR(Status)) {
                    return 0;
                }
            }
            Fat32 = (UINT32 *)( FatWindow->FatBuffer );
            WorkCluster = Fat32[Cluster - FatWindow->FirstCluster];

            if ( (WorkCluster > 0xfffffff7) || (WorkCluster == 0) ) {
                *NextCluster = 0;
                break;
            }

            if ( WorkCluster != Cluster + 1 && Continuous ) {
                *NextCluster = WorkCluster;
                break;
            }
            Cluster = WorkCluster;
        }
#endif
    } else {
        while ( TRUE ) {
            Count++;
            WorkCluster = *(UINT16 *)(FatWindow->FatBuffer + Cluster + Cluster / 2);
            WorkCluster = (Cluster & 1) ? WorkCluster >> 4 : WorkCluster & 0xfff;

            if ( WorkCluster > 0xff7 ) {
                *NextCluster = 0;
                break;
            }

            if ( WorkCluster != Cluster + 1 && Continuous ) {
                *NextCluster = WorkCluster;
                break;
            }
            Cluster = WorkCluster;
        }
    }
    return Count;
}

/**
    Reads data from FAT device

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FirstCluster - starting cluster
    @param Size - size of the data to read
    @param Buffer - pointer to buffer to store data

               
    @retval EFI_SUCCESS - correctly read all FAT data
    @retval EFI_ABORTED - should never get this.
    @retval Other any errors reported from ReadDevice function

**/
EFI_STATUS 
GetFatData (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN RC_VOL_INFO       *Volume,
    IN UINT32            FirstCluster,
    IN UINTN             Size,
    OUT VOID             *Buffer 
)
{
    EFI_STATUS Status;
    UINT32     Count;
    UINT32     NextCluster;
    UINT32     Cluster = FirstCluster;
    UINTN      SizeToRead;

    while ( TRUE )
    {
        SizeToRead = Size;
        Count      = GetClustersCount( Volume->FatType, Cluster, &NextCluster, TRUE );
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

/**
    Prepares given volume for read operations. Reads FAT table, root directory,
    determines FAT type

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param Bs - pointer to MBR or diskette FAT data

               
    @retval EFI_STATUS - possible return values from ReadDevice, AllocatePages, GetFatData

**/
EFI_STATUS 
GetFatType (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN BOOT_SECTOR       *Bs 
)
{
    EFI_STATUS           Status;
    UINT32               TotalSectors;
    UINT32               FatSectors;
    UINT32               RootSectors;
    UINT32               DataSectors;
    UINT32               RootSize;
    UINT32               FatSize;
    UINT32               DataClusters;
    UINT32               RootClusters;
    UINT32               DummyCluster;
    UINTN                RootPages;
    EFI_PHYSICAL_ADDRESS Allocate;

    FatSectors   = ( Bs->FATSz16 != 0 ) ? Bs->FATSz16 : Bs->Fat.Fat32.FATSz32;
    FatSize      = FatSectors * Bs->BytsPerSec;
    TotalSectors = ( Bs->TotSec16 != 0 ) ?  Bs->TotSec16 : Bs->TotSec32;
    RootSectors  = ( (Bs->RootEntCnt * 32) + (Bs->BytsPerSec - 1) ) / Bs->BytsPerSec;
    RootSize     = RootSectors * Bs->BytsPerSec;
    DataSectors  = TotalSectors - RootSectors - FatSectors * Bs->NumFATs - Bs->RsvdSecCnt;
    DataClusters = DataSectors / Bs->SecPerClus;

    Volume->FatOffset       = Bs->RsvdSecCnt * Bs->BytsPerSec;
    Volume->RootOffset      = Volume->FatOffset + FatSectors * Bs->NumFATs * Bs->BytsPerSec;
    Volume->DataOffset      = Volume->RootOffset + RootSize;
    Volume->BytesPerCluster = Bs->BytsPerSec * Bs->SecPerClus;
    Volume->FatType         = (DataClusters >= 65525) ? FAT32 : ((DataClusters < 4085) ? FAT12 : FAT16);

    RootEntries = Bs->RootEntCnt;

    //
    // Init FAT table
    //
    Status = InitFat( PeiServices, Volume, FatSize );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Read Root directory
    //
    if ( RootSize == 0 ) {      
        //    
        // in case of FAT32 it will be so at this time
        //
        RootClusters = GetClustersCount( FAT32, Bs->Fat.Fat32.RootClus, &DummyCluster, FALSE );
        RootSize    = RootClusters * Volume->BytesPerCluster;
        RootEntries = RootSize / 32;
    }

    RootPages = EFI_SIZE_TO_PAGES( RootSize );

    if ( RootBufferSize < EFI_PAGES_TO_SIZE( RootPages )) {
        Status = (*PeiServices)->AllocatePages( PeiServices, EfiBootServicesData, RootPages, &Allocate );
        if ( EFI_ERROR( Status )) {
            return Status;
        }

        RootBuffer     = (UINT8*)( (UINTN)Allocate );
        RootBufferSize = EFI_PAGES_TO_SIZE( RootPages );
    }
    MemSet( RootBuffer, RootBufferSize, 0 );

    if ( Volume->FatType == FAT32 ) {
        Status = GetFatData( PeiServices, Volume, Bs->Fat.Fat32.RootClus, RootSize, RootBuffer );
    }
    else {
        Status = ReadDevice( PeiServices, Volume, Volume->RootOffset, RootSize, RootBuffer );
    }

    return Status;
}

/**
    Reads recovery capsule from FAT volume

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FileName - recovery capsule file name
    @param FileSize - pointer to size of provided buffer
    @param Buffer - pointer to buffer to store data


    @retval EFI_STATUS

**/
EFI_STATUS 
ProcessFatVolume (
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

    Status = ReadDevice( PeiServices, Volume, 0, 512, &MemBlk->Bs );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

    if (!IsFat( &MemBlk->Bs )) {
        return EFI_NOT_FOUND;
    }

    ThisVolume = Volume;
    Status = GetFatType( PeiServices, Volume, &MemBlk->Bs );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

    AmiGetFileListFromFatVolume((DIR_ENTRY*)RootBuffer, RootEntries, &NumberOfFiles, FatRecoveryFiles);
    
    if ( NumberOfFiles == 0 )
        return EFI_NOT_FOUND;

    for(i = 0; i < NumberOfFiles; i++) {
        if ( *FileSize < FatRecoveryFiles[i]->FileSize )
            continue;

        FirstFileCluster = ( FatRecoveryFiles[i]->FirstClusterHi << 16 ) + FatRecoveryFiles[i]->FirstClusterLo;

        Status = GetFatData( PeiServices, Volume, FirstFileCluster, FatRecoveryFiles[i]->FileSize, Buffer );
        if( EFI_ERROR(Status) )
            continue;

        if( AmiIsValidFile( Buffer, FatRecoveryFiles[i]->FileSize ) ) {
            *FileSize = FatRecoveryFiles[i]->FileSize;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

/**
    Reads recovery capsule from FAT device. First treat device as 
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
ProcessFatDevice (
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
    Status = ProcessFatVolume( PeiServices, Volume, FileSize, Buffer );

    if ( !EFI_ERROR( Status )) {
        return Status;
    }

    //
    // Not floppy formatted, look for partitions. Rread sector 0 (MBR).
    //
    Status = ReadDevice( PeiServices, Volume, 0, 512, &MemBlk->Mbr );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

    if ( MemBlk->Mbr.Sig != 0xaa55 ) {
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
        if ( !EFI_ERROR(Status) && gIntegrityCheck ) {
            Status2 = ProcessFatVolume( PeiServices, Volume, FileSize, Buffer );
            if ( !EFI_ERROR( Status2 ) ) {
                gIntegrityCheck = FALSE;
                return Status2;
            }
        }
    } while ( Status == EFI_SUCCESS );

    return EFI_NOT_FOUND;
}

/**
    Reads recovery capsule from ISO9660 device

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FileName - recovery capsule file name
    @param FileSize - pointer to size of provided buffer
    @param Buffer - pointer to buffer to store data


    @retval EFI_STATUS

**/
EFI_STATUS 
ProcessPrimaryVolume (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN OUT UINTN         *FileSize,
    OUT VOID             *Buffer 
)
{
    EFI_STATUS                Status;
    PRIMARY_VOLUME_DESCRIPTOR PriVol;
    UINT32                    RootSize;
    UINTN                     RootPages;
    EFI_PHYSICAL_ADDRESS      Allocate;
    UINTN                     NumberOfFiles;
    UINT32                    i;

    Volume->PartitionOffset = 0;
    Status = ReadDevice( PeiServices, Volume, 16 * Volume->BlockSize, Volume->BlockSize, &PriVol );
    if ( EFI_ERROR( Status )) {
        return Status;
    }

    //
    // check that we read CD
    //
    if ( PriVol.Type != 1 || MemCmp( PriVol.StandardId, "CD001", 5 )) {
        return EFI_NOT_FOUND;
    }

    //
    // read root directory
    //
    RootSize  = PriVol.RootDir.DataLength;
    RootPages = EFI_SIZE_TO_PAGES( RootSize );

    if ( RootBufferSize < EFI_PAGES_TO_SIZE( RootPages )) {
        Status = (*PeiServices)->AllocatePages( PeiServices, EfiBootServicesData, RootPages, &Allocate );

        if ( EFI_ERROR( Status )) {
            return Status;
        }

        RootBuffer     = (UINT8*)((UINTN)Allocate);
        RootBufferSize = EFI_PAGES_TO_SIZE( RootPages );
    }
    MemSet( RootBuffer, RootBufferSize, 0 );

    Status = ReadDevice( PeiServices, Volume, PriVol.RootDir.ExtentOffset * Volume->BlockSize, RootSize, RootBuffer );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

    AmiGetFileListFromPrimaryVolume((DIR_RECORD*)RootBuffer, RootSize, &NumberOfFiles, CdRecoveryFiles);

    if(NumberOfFiles == 0)
        return EFI_NOT_FOUND;

    for( i = 0; i < NumberOfFiles; i++ ) {
        if ( *FileSize < CdRecoveryFiles[i]->DataLength )
            continue;

        Status = ReadDevice( PeiServices, Volume, CdRecoveryFiles[i]->ExtentOffset * Volume->BlockSize, CdRecoveryFiles[i]->DataLength, Buffer );
        if(EFI_ERROR(Status)) 
            continue;

        if( AmiIsValidFile( Buffer, CdRecoveryFiles[i]->DataLength ) ) {
            *FileSize = CdRecoveryFiles[i]->DataLength;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

/**
    Reads recovery capsule ATAPI device. First search for recovery capsule in
    primary volume. If not found tries to process Eltorito images

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FileName - recovery capsule file name
    @param FileSize - pointer to size of provided buffer
    @param Buffer - pointer to buffer to store data


    @retval EFI_STATUS

**/
EFI_STATUS 
ProcessCd (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN OUT UINTN         *FileSize,
    OUT VOID             *Buffer 
)
{
    EFI_STATUS                    Status;
    UINT8                         Data[2048];       // space for 1 block
    BOOT_RECORD_VOLUME_DESCRIPTOR *BootDesc;
    INITIAL_DEFAULT_ENTRY         *Entry;

    if ( !( Volume->BlockSize == 2048 && PcdGetBool( PcdCdRecoverySupport ) ) ){
        return EFI_UNSUPPORTED;
    }

    Status = ProcessPrimaryVolume( PeiServices, Volume, FileSize, Buffer );

    if ( !EFI_ERROR( Status )) {
        return Status;
    }

    //
    // file not found in primary volume, check Eltorito partitions
    //
    Status = ReadDevice( PeiServices, Volume, 17 * Volume->BlockSize, Volume->BlockSize, Data );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

    //
    // check if it is Eltorito
    //
    BootDesc = (BOOT_RECORD_VOLUME_DESCRIPTOR*)Data;

    if ( BootDesc->BootRecordIndicator != 0
         || MemCmp( &(BootDesc->ISO9660Identifier),    "CD001",                   5 )
         || BootDesc->DescriptorVersion != 1
         || MemCmp( &(BootDesc->BootSystemIdentifier), "EL TORITO SPECIFICATION", 23 )) {
        return EFI_NOT_FOUND;
    }

    //
    // it is Eltorito, read boot catalog
    //
    Status = ReadDevice( PeiServices, Volume, BootDesc->BootCatalogFirstSector * Volume->BlockSize, Volume->BlockSize, Data );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

    Entry                   = (INITIAL_DEFAULT_ENTRY*) &Data[32];
    Volume->PartitionOffset = Entry->LoadRBA * Volume->BlockSize;
    Status                  = ProcessFatDevice( PeiServices, Volume, FileSize, Buffer );
    return Status;
}

/**
    Search for recovery capsule file on all file system devices

               
    @param PeiServices - pointer to PEI services
    @param FileName - recovery capsule file name
    @param DeviceIndex - device index for given BlockIo PPI
    @param pBlockIo - pointer to BlockIo PPI
    @param FileSize - pointer to size of provided buffer
    @param Buffer - pointer to buffer to store data


    @retval EFI_STATUS

**/
EFI_STATUS 
FsRecoveryRead (
    IN EFI_PEI_SERVICES               **PeiServices,
    IN UINTN                          DeviceIndex,
    IN EFI_PEI_RECOVERY_BLOCK_IO_PPI  *pBlockIo,
    IN OUT UINTN                      *pSize,
    OUT VOID                          *pBuffer 
)
{
    EFI_STATUS             Status;
    RC_VOL_INFO            Volume;
    EFI_PEI_BLOCK_IO_MEDIA Media;
    UINTN                   BlockPages;
    EFI_PHYSICAL_ADDRESS    Allocate;
    UINT32                 Index;

    if ( !pBlockIo || !pSize || *pSize && !pBuffer ) {
        return EFI_INVALID_PARAMETER;
    }

    MemSet( &Volume, sizeof(RC_VOL_INFO), 0 );

    Status = pBlockIo->GetBlockDeviceMediaInfo( PeiServices, pBlockIo, DeviceIndex, &Media );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

    if ( !Media.MediaPresent ) {
        return EFI_NOT_FOUND;
    }

    Volume.BlkIo     = pBlockIo;
    Volume.Device    = DeviceIndex;
    Volume.BlockSize = Media.BlockSize;
    
    gRecoveryDeviceType = Media.DeviceType;

    //
    // Allocate memory for MBR, Boot Sector, etc.
    //
    BlockPages = EFI_SIZE_TO_PAGES( sizeof(MEMORY_BLOCK) );
    Status = (*PeiServices)->AllocatePages( PeiServices, EfiBootServicesData, BlockPages, &Allocate );
    if ( EFI_ERROR( Status )) {
        return EFI_NOT_FOUND;
    }
    MemBlk = ( MEMORY_BLOCK* )( ( UINTN )Allocate );

    for(Index = 0; FsRecoverySupport[Index]!=NULL; Index++){
        Status = FsRecoverySupport[Index](PeiServices, &Volume, pSize, pBuffer);
        if (Status == EFI_SUCCESS) break;
    }


    return Status;
}

/**
    Device Recovery Module PPI  


    @param PeiServices
    @param pCapsuleName
    @param pCapsuleSize
    @param ExtendedVerification


    @retval EFI_SUCCESS - number of recovery capsules returned
    @retval EFI_INVALID_PARAMETER - the pointer NumberRecoveryCapsules is NULL
**/
EFI_STATUS
FwCapsuleInfo (
  IN EFI_PEI_SERVICES  **PeiServices,
  IN OUT VOID          **pCapsuleName,
  IN OUT UINTN         *pCapsuleSize,
  OUT   BOOLEAN        *ExtendedVerification
)
#if defined(SecFlashUpd_SUPPORT) && SecFlashUpd_SUPPORT == 1
;
#else
{
    return EFI_SUCCESS;
}
#endif

/**
    Gets recovery file information  


    @param PeiServices
    @param pCapsuleName
    @param pCapsuleSize
    @param ExtendedVerification


    @retval EFI_STATUS
**/
EFI_STATUS 
GetRecoveryFileInfo (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT VOID          **pCapsuleName,
    IN OUT UINTN         *pCapsuleSize,
    OUT    BOOLEAN       *ExtendedVerification
)
{
   if( pCapsuleName != NULL )
        *pCapsuleName = ( CHAR8 * ) PcdGetPtr ( PcdRecoveryImageName );

    if ( pCapsuleSize != NULL )
        *pCapsuleSize = ( UINTN ) PcdGet32 ( PcdRecoveryImageSize );

    if( ExtendedVerification != NULL )
        *ExtendedVerification = FALSE;

    return FwCapsuleInfo( PeiServices, pCapsuleName, pCapsuleSize, ExtendedVerification );
}

/**
    GetNumberRecoveryCapsules function of ppi 
    EFI_PEI_DEVICE_RECOVERY_MODULE_PPI.


    @param PeiServices
    @param This
    @param NumberRecoveryCapsules


    @retval EFI_SUCCESS - number of recovery capsules returned
    @retval EFI_INVALID_PARAMETER - the pointer NumberRecoveryCapsules is NULL
**/
EFI_STATUS 
EFIAPI
GetNumberRecoveryCapsules (
    IN EFI_PEI_SERVICES                    **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI  *This,
    OUT UINTN                              *NumberRecoveryCapsules 
)
{
    if ( !NumberRecoveryCapsules ) {
        return EFI_INVALID_PARAMETER;
    }
    *NumberRecoveryCapsules = 1;
    return EFI_SUCCESS;
}

/**
    GetRecoveryCapsuleInfo function of ppi EFI_PEI_DEVICE_RECOVERY_MODULE_PPI 
    for any block devices including floppies, USB keys, CD-ROMs and HDDs.

        
    @param PeiServices - pointer to PeiServices Structure
    @param This - pointer to the PPI structure
    @param CapsuleInstance - value indicating the instance of the PPI
    @param Size - Size of the recovery capsule
    @param CapsuleType OPTIONAL - Type of recovery capsule

         
    @retval EFI_SUCCESS - Parameters are valid and output parameters are updated
    @retval EFI_INVALID_PARAMETER - Size pointer is NULL
    @retval EFI_NOT_FOUND - asking for a 1 or greater instance of the PPI
**/
EFI_STATUS 
EFIAPI
GetRecoveryCapsuleInfo (
    IN EFI_PEI_SERVICES                    **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI  *This,
    IN UINTN                               CapsuleInstance,
    OUT UINTN                              *Size,
    OUT EFI_GUID                           *CapsuleType 
)
{
    EFI_STATUS Status;

    if ( !Size ) {
        return EFI_INVALID_PARAMETER;
    }

    if ( CapsuleInstance > 0 ) {
        return EFI_NOT_FOUND;
    }

    Status = GetRecoveryFileInfo(PeiServices, NULL, Size, NULL);
    if( EFI_ERROR( Status ))
        return Status;

    if ( CapsuleType ) {
        *CapsuleType = gBlockDeviceCapsuleGuid;
    }
    return EFI_SUCCESS;
}

#define NUMBER_OF_RETRIES 3

/**
    Locates all EFI_PEI_RECOVERY_BLOCK_IO_PPI PPIs.  Calls function 
    GetNumberOfBlockDevices.  For each block device, calls the function 
    FsRecoveryRead, to find the recovery image named in var sAmiRomFile.

        
    @param PeiServices - pointer to PeiServices Structure 
    @param This - pointer to the PPI structure
    @param CapsuleInstance - value indicating the instance of the PPI
    @param Buffer - contains information read from the block device

         
    @retval EFI_SUCCESS-  File read from recovery media
    @retval EFI_INVALID_PARAMETER - Buffer is a NULL pointer
    @retval EFI_NOT_FOUND - asking for a 1 or greater instance of the PPI
    @retval Other return error values from LocatePpi or FsRecoveryRead
**/
EFI_STATUS 
EFIAPI
LoadRecoveryCapsule (
    IN EFI_PEI_SERVICES                    **PeiServices,
    IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI  *This,
    IN UINTN                               CapsuleInstance,
    OUT VOID                               *Buffer 
)
{
    EFI_STATUS             Status;
    EFI_PEI_PPI_DESCRIPTOR *pDummy;
    UINTN                  i;
    UINTN                  RecoveryCapsuleSize;
    BOOLEAN                ExtendedVerification;

    PEI_TRACE((-1, PeiServices, "..BLOCK DEVICE.."));

    if ( !Buffer ) {
        return EFI_INVALID_PARAMETER;
    }

    if ( CapsuleInstance > 0 ) {
        return EFI_NOT_FOUND;
    }

    Status = GetRecoveryFileInfo(PeiServices, NULL, &RecoveryCapsuleSize, &ExtendedVerification);
    if ( EFI_ERROR( Status ))
        return Status;

    i = 0;

    do
    {
        EFI_PEI_RECOVERY_BLOCK_IO_PPI *pBlockIo;
        UINTN                         NumberBlockDevices;
        UINTN                         j;
        UINTN                         Size;
        Status = (*PeiServices)->LocatePpi( PeiServices, &gEfiPeiVirtualBlockIoPpiGuid, i++, &pDummy, (VOID**)&pBlockIo );

        if ( EFI_ERROR( Status )) {
            break;
        }

        if (EFI_ERROR( Status = pBlockIo->GetNumberOfBlockDevices(
                                            PeiServices, pBlockIo, &NumberBlockDevices))) {
            continue;
        }

        for ( j = 0; j < NumberBlockDevices; j++ ) {
            UINTN k;
            Size = RecoveryCapsuleSize;

            for ( k = 0; k < NUMBER_OF_RETRIES; k++ )
            {
                Status = FsRecoveryRead(
                    PeiServices, j, pBlockIo,
                    &Size, Buffer
                    );

                if ( !EFI_ERROR(Status) ){
                    ReportCapsuleSize (PeiServices, (UINT32)Size);
                    if(ExtendedVerification || Size == RecoveryCapsuleSize )
                        return EFI_SUCCESS;
                }
            }
        }
    } while ( TRUE );
    return Status;
}

/************************************************************************/
/*              Entry Point                                             */
/************************************************************************/
EFI_PEI_DEVICE_RECOVERY_MODULE_PPI BlockDeviceRecoveryModule = {
    GetNumberRecoveryCapsules, GetRecoveryCapsuleInfo, LoadRecoveryCapsule
};

//
// PPI to be installed
//
static EFI_PEI_PPI_DESCRIPTOR      BlockDeviceRecoveryPpiList[] = {
    {
        EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gEfiPeiDeviceRecoveryModulePpiGuid, &BlockDeviceRecoveryModule
    }
};


/**
    Installs EFI_PEI_DEVICE_RECOVERY_MODULE_PPI for loading recovery 
    images from block devices such as floppies, USB keys, HDDs or CD-ROMs
    
    
    @param FfsHeader
    @param PeiServices - pointer to PeiServices Structure  
    
    
    @retval EFI_STATUS   

**/
EFI_STATUS 
EFIAPI
FsRecoveryPeimEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices 
)
{
    ThisPeiServices = PeiServices;
    return (*PeiServices)->InstallPpi( PeiServices, BlockDeviceRecoveryPpiList );
}

//
//************************** AMI custom eLink implementation *******************
//

VOID 
AmiGetFileListFromPrimaryVolume (
    IN  DIR_RECORD          *Root,
    IN  UINT32              RootSize,
    OUT UINTN               *NumberOfFiles,
    OUT DIR_RECORD          **Buffer
)
{
    UINT32 Count = 0;
    CHAR8  *DirFileName;
    UINT32 FileNameSize;

    VOID *FileName;
    UINTN FileSize;
    EFI_STATUS Status;


    *NumberOfFiles = 0;     // no files found yet

    Status = GetRecoveryFileInfo(ThisPeiServices, &FileName, &FileSize, NULL);
    if( EFI_ERROR( Status ))
        return;

    //
    // find file in root directory
    //
    while ( Count < RootSize ) {
        DirFileName = ( CHAR8* )( Root + 1 );

        if( Root->Length == 0 )
            return;

        //
        // Find the length of the file name.  The ISO9660 spec has the following structure
        // up to 8 characters then a '.' then up to 3 more characters then a ';' 
        // then the digits that make up a number between 0 and 32767
        // The filename search uses all characters up to the ';'
        //
        FileNameSize = 0;
        while( DirFileName[FileNameSize] != ';' && ( FileNameSize < Root->LengthOfFileId )) 
            FileNameSize++;

        if ( !EFI_ERROR( FileSearch( (CHAR8*)FileName, DirFileName, FALSE, FileNameSize ))) {
            Buffer[*NumberOfFiles] = Root;
            ( *NumberOfFiles )++;
            break;
        }

        Count += Root->Length;
        Root = (DIR_RECORD *)((UINT8 *)Root + Root->Length);
    }
}

#if SEARCH_PATH
/**
    If SEARCH_PATH is ON, adds specified path to the recovery file name


    @param FileName - Name to add path to


    @retval None - returned in variable.

**/
VOID 
AddRecoveryPath (
    VOID IN OUT  **FileName 
)
{
    UINTN i, j;
    CHAR8 *TmpBuiltName = &BuiltRecoveryName[0];
    CHAR8 *TmpRecPath = &(CHAR8)RecoveryPath[0];
    UINTN PathSize = Strlen( TmpRecPath );
    VOID  *TmpVoid;
    CHAR8 *TmpFileName;
    UINTN NameSize;
    UINTN TotalSize;

    TmpVoid = *FileName;
    TmpFileName = (CHAR8 *)TmpVoid;
    NameSize = Strlen(TmpFileName);
    TotalSize = NameSize + PathSize;
    //
    // Build up a path and name
    //
    MemCpy( TmpBuiltName, TmpRecPath, PathSize );
    for( i=PathSize, j=0; i<TotalSize; i++, j++ ) {
        TmpBuiltName[i] = TmpFileName[j];
    }
    TmpBuiltName[i] = 0; // Terminate name
    *FileName = (VOID *)TmpBuiltName;
}

/**
    If SEARCH_PATH is ON, isolates each name in a path


    @param FilePath - Path we are searching
    @param NextName - Points to next name in the path


    @retval TRUE - if isolated name found

**/
BOOLEAN 
IsolateName (
    IN  CHAR8       **FilePath,
    OUT CHAR8       **NextName
)
{
    UINTN   len;
    BOOLEAN GotName;
    UINTN   i;
    CHAR8   *TempPath;

    TempPath = *FilePath;
    len = Strlen(TempPath);
    GotName = TRUE;
    for ( i = 0; i < len; i++ ) {
        if ( TempPath[i] == 0x2F ) { // "/"
            GotName = FALSE;
            TempPath[i] = 0;
            *NextName = TempPath + i + 1;
            return GotName;
        }
    }
    return GotName;
}

#endif

/**
    Gets a list of valid recovery files from an FAT volume.
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
AmiGetFileListFromFatVolume (
    IN  DIR_ENTRY           *Root,
    IN  UINT32              RootEntries,
    OUT UINTN               *NumberOfFiles,
    OUT DIR_ENTRY           **Buffer
)
{
    UINT32 i;
    VOID *FileName;
    UINTN FileSize;
    EFI_STATUS Status;
    UINT32 DirEntries;
    DIR_ENTRY *DirPtr;
    CHAR8 *FilePath;
    CHAR8 FatName[12];
#if SEARCH_PATH
    CHAR8 *NextName;
    UINT32 DirSize;
    UINTN DirPages;
    EFI_PHYSICAL_ADDRESS Allocate;
    UINT32 Cluster;
    UINT32     NextCluster;
    UINT32     FirstCluster;
    UINT32     ClusterCount = 0;
#endif
#if MATCH_VOLUME_NAME
    CHAR8 *VolumeName = CONVERT_TO_STRING(VOLUME_NAME);
    UINT8 j;
    BOOLEAN VolNameFound = FALSE;
#endif

    *NumberOfFiles = 0;     //no files found yet

    Status = GetRecoveryFileInfo(ThisPeiServices, &FileName, &FileSize, NULL);
    if(EFI_ERROR(Status))
        return;
#if SEARCH_PATH
    AddRecoveryPath(&FileName);
#endif

    FilePath = FileName;
    DirEntries = RootEntries;
    DirPtr = Root;
#if SEARCH_PATH
    NextName = FilePath;

    while ( IsolateName (&FilePath, &NextName) == FALSE ) {
        //
        // Find path name in directory
        //
        ConvertToFatFileName( FilePath, FatName );
        for( i = 0; i < DirEntries; i++ ) {
            if( ( DirPtr[i].FileName[0] == (CHAR8)0xE5) || (DirPtr[i].FileName[0] == (CHAR8)0 ) ) 
                continue;

#if MATCH_VOLUME_NAME
            if ( ((DirPtr[i].DirAttr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_VOLUME_ID) && IsMbr ) {
                if ( DirPtr[i].FileName[0] ==(CHAR8)0x20 || DirPtr[i].FileName[0] == (CHAR8)0xE5 ) {
                    continue;
                }
                for ( j=0; j<11; j++ ) {
                    if ( DirPtr[i].FileName[j] ==(CHAR8) 0x20 ) {
                        DirPtr[i].FileName[j] = (CHAR8)0; // Terminate name
                    }
                }
                PEI_TRACE((-1, ThisPeiServices, "\nVolume name is %s\n", &DirPtr[i].FileName[0]));
                if ( (FileCompare(&DirPtr[i].FileName[0], VolumeName, FALSE, Strlen(VolumeName))) ) {
                    PEI_TRACE((-1, ThisPeiServices, "\nName matched\n"));
                    VolNameFound = TRUE;
                }
            }
#endif

            if(!EFI_ERROR( FileSearch ((CHAR8*)FatName, DirPtr[i].FileName, TRUE, FAT_FILE_NAME_SIZE ))) {
                //
                // A match was found...
                // Update FilePath to next name. 
                // Update DirPtr to this directory, and read it in
                // 
                // *REMOVED* DirSize = DirPtr[i].FileSize;
                //
                // The size of a directory is not stored in it's directory entry,
                // So we will only read in one cluster for now.
                //
                Cluster = DirPtr[i].FirstClusterLo + (DirPtr[i].FirstClusterHi << 16);
                FirstCluster = Cluster;
                ClusterCount      = GetClustersCount( ThisVolume->FatType, FirstCluster, &NextCluster, FALSE );
                DirSize = ClusterCount * ThisVolume->BytesPerCluster;
                DirEntries = DirSize / 32;
                Cluster = DirPtr[i].FirstClusterLo + (DirPtr[i].FirstClusterHi << 16);

                DirPages = EFI_SIZE_TO_PAGES( DirSize );

                if ( DirSize < EFI_PAGES_TO_SIZE( DirPages )) {
                    Status = (*ThisPeiServices)->AllocatePages( ThisPeiServices,
                                                 EfiBootServicesData, DirPages, &Allocate );
                    if ( EFI_ERROR( Status )) {
                        *NumberOfFiles = 0;
                        return;
                    }

                    DirPtr = (DIR_ENTRY*)((UINTN)Allocate);
                    DirSize = EFI_PAGES_TO_SIZE( DirPages );
                }
                MemSet( DirPtr, DirSize, 0 );

                Status = GetFatData( ThisPeiServices, ThisVolume, Cluster, DirSize, DirPtr );
                if ( EFI_ERROR( Status )) {
                    *NumberOfFiles = 0;
                    return;
                }
                FilePath = NextName;
                break;
            }
        }
    }
#endif
    //
    // Find file in directory
    //
    ConvertToFatFileName( FilePath, FatName );
    PEI_TRACE((-1, ThisPeiServices, "\nEntering name search\n"));
    for( i = 0; i < DirEntries; i++ ) {
        if((DirPtr[i].FileName[0] == (CHAR8)0xE5) || (DirPtr[i].FileName[0] == (CHAR8)0)) 
            continue;

#if MATCH_VOLUME_NAME && SEARCH_PATH == 0
        if ( ((DirPtr[i].DirAttr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_VOLUME_ID) && IsMbr ) {
            if ( DirPtr[i].FileName[0] == (CHAR8)0x20 || DirPtr[i].FileName[0] ==(CHAR8) 0xE5 ) {
                continue;
            }
            for ( j=0; j<12; j++ ) {
                if ( DirPtr[i].FileName[j] ==(CHAR8) 0x20 ) {
                    DirPtr[i].FileName[j] = (CHAR8)0; // Terminate name
                }
            }
            if ( (FileCompare(&DirPtr[i].FileName[0], VolumeName, FALSE, Strlen(VolumeName))) ) {
                PEI_TRACE((-1, ThisPeiServices, "\nName matched\n"));
                VolNameFound = TRUE;
            }
        }
#endif

        if(!EFI_ERROR(FileSearch((CHAR8*)FatName, &DirPtr[i].FileName[0], TRUE, FAT_FILE_NAME_SIZE))) {
            Buffer[*NumberOfFiles] = &DirPtr[i];
#if MATCH_VOLUME_NAME
            if ( VolNameFound == TRUE ) *NumberOfFiles = 1;
#else
            *NumberOfFiles = 1;
            PEI_TRACE((-1, ThisPeiServices, "\nFound recovery file\n"));
#endif
            break;
        }
    }
}

/**
    Checks file is valid or not.

        
    @param FileData
    @param FileSize


    @retval BOOLEAN
**/
BOOLEAN 
AmiIsValidFile (
    IN VOID   *FileData,
    IN UINTN  FileSize
)
{
    return TRUE;
}

/**
    E-Link to check the validity of the Partition used to find recovery image.

        
    @param PartCount          - Counter to keep track of search
    @param IsMbr              - True if looking for MBR partitions
	@param RecoveryDeviceType - Recovery Device Type
    @param *MemBlk            - pointer to Memory Block structure


    @retval EFI_STATUS

**/
EFI_STATUS
OemPartitionVolumeIntegrityCheck(
  IN     UINT8           PartCount,
  IN     BOOLEAN         IsMbr,
  IN     UINT8           RecoveryDeviceType,
  IN     MEMORY_BLOCK    *MemBlk  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN       i;

  for (i = 0; OemRecoveryVolumeIntegrityCallback[i] != NULL; i++)
    Status = OemRecoveryVolumeIntegrityCallback[i](PartCount,IsMbr,RecoveryDeviceType,MemBlk);

  return Status;
}

/**
    Finds the next partition on the volume, and sets the VolumeOffset in
    the RC_VOL_INFO structure.

        
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure


    @retval EFI_STATUS


    @note  
      This function uses the following global variables:
      UINTN PartCount - Counter to keep track of search
      BOOLEAN IsMbr - True if looking for MBR partitions
      UINT32 GpeCount - GUID Partition Entry count
      UINT32 PartSector - Starting sector of partition
      PartCount and PartSector must be seeded to 0, and IsMbr must be
      seeded to TRUE before the first call.

**/
EFI_STATUS 
FindNextPartition (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume 
)
{
    EFI_STATUS          Status;
    UINT64              Offset;
    GUID_BOOT_RECORD    *Gbr;
    GUID_TABLE_HEADER   *Gth;
    UINT32              GpeSize;
    UINT32              i;
    UINT32              TempSector;

    //
    // Check for MBR partitions
    //
    if ( IsMbr ) {
        while ( PartCount < 4 ) {
            if ( MemBlk->Mbr.PartRec[PartCount].OSType == 0xEE ) {
                IsMbr = FALSE; // Mark GUID partition found
                PartCount = 0; // Reset counter
                break;
            }

            if ( MemBlk->Mbr.PartRec[PartCount].OSType == 5
                 || MemBlk->Mbr.PartRec[PartCount].OSType == 15 ) { // Extended partition
                if (ExtOffset == 0) { // If the first extended partition
                    ExtOffset = MemBlk->Mbr.PartRec[PartCount].StartingLba;
                    PartSector = ExtOffset;
                } else {
                    PartSector = ExtOffset + MemBlk->Mbr.PartRec[PartCount].StartingLba;
                }
                Volume->PartitionOffset = Mul64( 512, PartSector );
                Status = ReadDevice( PeiServices, Volume, 0, 512, &MemBlk->Mbr );
                if ( EFI_ERROR( Status )) {
                    return Status;
                }
                PartCount = 0;
            }

            if ( MemBlk->Mbr.PartRec[PartCount].OSType == 0
                 || MemBlk->Mbr.PartRec[PartCount].SizeInLba == 0
                 || MemBlk->Mbr.PartRec[PartCount].StartingLba == 0 ) {
                PartCount++; // Check next partition
                continue;
            }

            TempSector = MemBlk->Mbr.PartRec[PartCount].StartingLba + PartSector;
            Volume->PartitionOffset = Mul64( 512, TempSector );
            PartCount++;

            Status  = OemPartitionVolumeIntegrityCheck(PartCount-1,IsMbr,(UINT8)gRecoveryDeviceType,MemBlk);
                if (!EFI_ERROR(Status))
                gIntegrityCheck = TRUE;
            return EFI_SUCCESS;
        }
    }
    if ( IsMbr ) return EFI_NOT_FOUND; // No MBR partitions were found

    //
    // Check for GUID partitions
    //
    if ( PartCount == 0 ) {
        Offset = Mul64( 1, Volume->BlockSize ); 
        Status = ReadDevice( PeiServices, Volume, Offset, 512, &MemBlk->Mbr );
        if ( EFI_ERROR( Status )) {
            return Status;
        }
        Gth = (GUID_TABLE_HEADER*)&MemBlk->Mbr.BootCode[0];
        if ( (Gth->Signature[0] == 0x45) && // Check for "EFI"
             (Gth->Signature[1] == 0x46) &&
             (Gth->Signature[2] == 0x49) )
        {
            GpeCount = Gth->EntryCount;
            GpeSize = Gth->EntrySize;
            //
            // We only support entry size of 128 for now.
            //
            if ( GpeSize != 128 ) return EFI_NOT_FOUND; //
            Offset = Mul64( 2, Volume->BlockSize );
            //
            // Read in the first entry in the partition table
            //
            Status = ReadDevice( PeiServices, Volume, Offset, 512, &MemBlk->Mbr );
            if ( EFI_ERROR( Status )) {
                return Status;
            }
        } else return EFI_NOT_FOUND; // Table header not found.
    }

    while ( PartCount < GpeCount ) {
        i = PartCount % 4;
        if ( (i == 0) && (PartCount != 0) ) {
            Offset = Mul64( 2+(PartCount/4), Volume->BlockSize );
            Volume->PartitionOffset = 0; // Reset this to zero
            Status = ReadDevice( PeiServices, Volume, Offset, 512, &MemBlk->Mbr );
            if ( EFI_ERROR( Status )) {
                return Status;
            }
        }
        PartCount++;
        Gbr = (GUID_BOOT_RECORD*)&MemBlk->Mbr.BootCode[0];
        Volume->PartitionOffset = Mul64( Gbr->GuidPart[i].FirstLba, Volume->BlockSize );
        //
        // The partition count is incremented by 4 for each new Guid Boot Record,
        // even if it does not contain 4 partition records. So we may find an empty
        // partition. Exit with EFI_NOT_FOUND if there is one.
        //
        if ( Volume->PartitionOffset == 0 ) {
            return EFI_NOT_FOUND;
        }

        Status  = OemPartitionVolumeIntegrityCheck(PartCount-1,IsMbr,(UINT8)gRecoveryDeviceType,MemBlk);
            if (!EFI_ERROR(Status))
            gIntegrityCheck = TRUE;
        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
