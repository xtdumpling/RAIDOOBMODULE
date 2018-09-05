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

/** @file ExtRecovery.c

**/

//----------------------------------------------------------------------

#include <Token.h>
#include <AmiPeiLib.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Ppi/BlockIo.h>
#include <Guid/AmiRecoveryDevice.h>

#include "ExtRecovery.h"
#include "FsRecovery.h"

//----------------------------------------------------------------------


DIR_ENTRY_EXT       *ExtRecoveryFiles[10];
UINT32              InodeBlock;     // First block of inode table
UINT8               Indirect;
UINT32              *BlockList;
UINT32              *BlockList2;
#if SEARCH_PATH
RC_VOL_INFO         *gExtVolume;
VOLUME_SB           *gSb;
#endif

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
extern BOOLEAN             RootAllocated;

/**
    Checks if given data block describes EXT Superblock structure


    @param pSb - pointer to data block to check

               
    @retval TRUE - data block is a EXT Superblock structure
    @retval FALSE - data block is not a EXT Superblock structure

**/
BOOLEAN 
IsExt (
    IN VOLUME_SB *pSb 
)
{
    return pSb->SB_Magic == 0xEF53
           && pSb->SB_BlockSize < 4
           && pSb->SB_FirstBlock < 2
           && pSb->SB_FreeBlocks < pSb->SB_TotalBlocks
           && pSb->SB_FreeInodes < pSb->SB_TotalInodes;
}

/**
    (EXT) Retrieves the next block number from an Inode
    block list.


    @param Vi - Volume Info Structure
    @param Sb - Superblock structure
    @param Inode - Inode table structure
    @param BlockNo - Sequential number of the block
    @param Block - Next block of the file
    @param UpdateList - Update block no. to next block if TRUE.


    @retval EFI_STATUS - Status (EFI_SUCCESS or EFI_END_OF_FILE)

**/
EFI_STATUS 
GetNextBlock (
    EFI_PEI_SERVICES  **PeiServices,
    RC_VOL_INFO       *Volume,
    VOLUME_SB         *Sb,
    VOLUME_IT         *Inode,
    UINT32            *BlockNo,
    UINT32            *Block,
    BOOLEAN           UpdateList 
)
{
    UINT32      *BlockListPtr;
    UINT32      TmpBlock;
    UINT32      TmpBlock2;
    UINT32      TmpBlkNo;
    UINT32      IBlkCnt;
    UINT64      Offset;
    UINT32      BlockSize;
    UINT32      NosPerBlock;
    UINTN       BlockPages;
    EFI_STATUS  Status;
    EFI_PHYSICAL_ADDRESS Allocate;

    BlockSize = 1024 << Sb->SB_BlockSize;
    NosPerBlock = BlockSize / 4;
    TmpBlkNo = *BlockNo;

    //
    // Process direct blocks (0-11)
    //
    if ( (TmpBlkNo < 12) && (Indirect == 0) )
    {
        BlockListPtr = &Inode->Alloc.Ext2.Blocks[0];
        *Block = BlockListPtr[TmpBlkNo];
        goto Exit;
    }

    //
    // Process single indirect blocks (12-(256+11))
    //
    if ( (TmpBlkNo >= 12) && (TmpBlkNo < NosPerBlock+12) && (Indirect != 1) )
    {
        Indirect = 1;
        BlockPages = EFI_SIZE_TO_PAGES( BlockSize );
        Status = (*PeiServices)->AllocatePages( PeiServices, 
                                                EfiBootServicesData, 
                                                BlockPages, 
                                                &Allocate );
        if ( EFI_ERROR( Status )) {
            PEI_TRACE((-1, PeiServices, "\nCan't allocate block pages 1\n"));
            return EFI_END_OF_FILE;
        }
        BlockList = (UINT32*)( (UINTN)Allocate );
    
        BlockListPtr = &Inode->Alloc.Ext2.Blocks[0];
        TmpBlock = BlockListPtr[12];
        Offset = Mul64(TmpBlock, BlockSize);
        Status = ReadDevice (PeiServices, Volume, Offset, BlockSize, &BlockList[0]);
        if ( EFI_ERROR( Status )) {
            return Status;
        }
    }

    if ( (TmpBlkNo >= 12) && (TmpBlkNo < NosPerBlock+12) && (Indirect == 1) )
    {
        BlockListPtr = &BlockList[0];
        TmpBlock = TmpBlkNo - 12;
        *Block = BlockListPtr[TmpBlock];
        goto Exit;
    }

    //
    // Process double indirect blocks ((256+12)-(65536+256+11))
    //
    if ( (TmpBlkNo >= (NosPerBlock+12)) && (Indirect != 2) )
    {
        Indirect = 2;
        BlockPages = EFI_SIZE_TO_PAGES( BlockSize );
        Status = (*PeiServices)->AllocatePages( PeiServices, 
                                                EfiBootServicesData, 
                                                BlockPages, 
                                                &Allocate );
        if ( EFI_ERROR( Status )) {
            PEI_TRACE((-1, PeiServices, "\nCan't allocate block pages 2\n"));
            return EFI_END_OF_FILE;
        }
        BlockList2 = (UINT32*)( (UINTN)Allocate );
    
        TmpBlock = Inode->Alloc.Ext2.Blocks[13];
        Offset = Mul64(TmpBlock, BlockSize);
        Status = ReadDevice ( PeiServices, Volume, Offset, BlockSize, &BlockList[0] );
        if ( EFI_ERROR( Status )) {
            return Status;
        }
    }

    if ((TmpBlkNo >= (NosPerBlock+12)) && (Indirect == 2))
    {
        TmpBlock = TmpBlkNo - 12;
        IBlkCnt = TmpBlock / NosPerBlock - 1;
        if (TmpBlock % NosPerBlock == 0)
        {
            //
            // Read another set of nos. into BlockList2
            //
            TmpBlock2 = BlockList[IBlkCnt];
            Offset = Mul64(TmpBlock2, BlockSize);
            Status = ReadDevice (PeiServices, Volume, Offset, BlockSize, &BlockList2[0]);
            if ( EFI_ERROR( Status )) {
                return Status;
            }
        }
        BlockListPtr = &BlockList2[0];
        TmpBlock -= ( NosPerBlock * (IBlkCnt+1) );
        *Block = BlockListPtr[TmpBlock];
    }

Exit:
    if (UpdateList)
    {
        TmpBlkNo++;
        *BlockNo = TmpBlkNo;
    }

    if (*Block == 0)
    {
        return EFI_END_OF_FILE;
    } else {
        return EFI_SUCCESS;
    }
}

/**
    Reads a file from a device formatted in Ext(n).


    @param Vi - Volume Info Structure
    @param Sb - Superblock structure
    @param Inode - Inode table structure
    @param Buffer - Buffer to read into
    @param Size - Size of file to read

               
    @retval EFI_STATUS - possible return values 

**/
EFI_STATUS 
ReadExtFile ( 
    IN EFI_PEI_SERVICES  **PeiServices,
    RC_VOL_INFO          *Volume, 
    VOLUME_SB            *Sb, 
    VOLUME_IT            *Inode, 
    VOID                 *Buffer, 
    UINT64               *Size 
)
{
    EFI_STATUS  Status;
    UINT32      BlockSize;
    UINT32      Block;
    UINT32      BlockNo;
    UINT64      Offset;
    UINT64      TotalBytesRead = 0;
    UINT16      ExtentCount;
    UINT64      BigBlock;
    UINT32      ReadSize;
    UINT16      i;

    BlockSize = 1024 << Sb->SB_BlockSize;
    BlockNo = 0;
    Indirect = 0;

    //
    // Check for which allocation method to use for reading the file
    //
    if ( (Inode->Alloc.Ext4.Header.EH_Magic == 0xF30A) && \
        (Inode->Alloc.Ext4.Header.EH_Max == 4) )
    {
    //
    // Use the EXT4 allocation method
    //
        ExtentCount = Inode->Alloc.Ext4.Header.EH_Extents;

        for (i=0; i<ExtentCount; i++)
        {
            BigBlock = Inode->Alloc.Ext4.Extent[i].EE_BlockLo + \
                       Shl64(Inode->Alloc.Ext4.Extent[i].EE_BlockHi, 0x20);
            Offset = Mul64(BigBlock, BlockSize);
            ReadSize = BlockSize * Inode->Alloc.Ext4.Extent[i].EE_Length;
            if (*Size <= ReadSize)
            {
                Status = ReadDevice (PeiServices, Volume, Offset, (UINTN)*Size, Buffer);
                if ( EFI_ERROR(Status) ) {
                    *Size = TotalBytesRead;
                    return Status;
                } else {
                    TotalBytesRead += *Size;
                    *Size = TotalBytesRead;
                    return EFI_SUCCESS;
                }
            }

            Status = ReadDevice ( PeiServices, Volume, Offset, ReadSize, Buffer );
            if ( EFI_ERROR(Status) ) return Status;
            *Size -= ReadSize;
            TotalBytesRead += ReadSize;
        } // End of read loop

        return EFI_VOLUME_CORRUPTED; // Shouldn't get here

    } else {
    //
    // Use the EXT2, EXT3 allocation method
    //
        Status = GetNextBlock ( PeiServices,
                                Volume,
                                Sb,
                                Inode,
                                &BlockNo,
                                &Block,
                                TRUE );
        if ( EFI_ERROR(Status) ) // Zero-length file
        {
            *Size = 0;
            PEI_TRACE((-1, PeiServices, "\nFirst GetNextBlock failed\n"));
            return Status;
        }

        do
        {
            Offset = Mul64 (BlockSize, Block);

            if (*Size <= BlockSize)
            {
                Status = ReadDevice ( PeiServices, Volume, Offset, (UINTN)*Size, Buffer );
                if ( EFI_ERROR(Status) ) {
                    PEI_TRACE((-1, PeiServices, "\nReading partial block failed\n"));
                    *Size = TotalBytesRead;
                    return Status;
                } else {
                    TotalBytesRead += *Size;
                    *Size = TotalBytesRead;
                    return EFI_SUCCESS;
                }
            }

            Status = ReadDevice (PeiServices, Volume, Offset, BlockSize, Buffer);
            if ( EFI_ERROR(Status) ) {
                PEI_TRACE((-1, PeiServices, "\nReading block failed, status = %r\n", Status));
                PEI_TRACE((-1, PeiServices, "\nOffset %lx, BlockSize %x, Block %x\n", Offset, BlockSize, Block));
                return Status;
            }

            *Size -= BlockSize;
            TotalBytesRead += BlockSize;
            Buffer = (UINT8*)Buffer + BlockSize;

            Status = GetNextBlock ( PeiServices,
                                    Volume,
                                    Sb,
                                    Inode,
                                    &BlockNo,
                                    &Block,
                                    TRUE );
            if ( EFI_ERROR(Status) ) // EOF found
            {
                PEI_TRACE((-1, PeiServices, "\nGetNextBlock failed\n"));
                *Size = TotalBytesRead;
                return EFI_SUCCESS;
            }
        }
        while ( *Size );

        return EFI_VOLUME_CORRUPTED; // Shouldn't get here
    }
}

/**
    Prepares given volume for read operations. Reads Ext(n) root directory.

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param Sb - pointer to Superblock
    @param Inode - Inode number of directory (root is 2)

               
    @retval EFI_STATUS - possible return values 

**/
EFI_STATUS 
ReadExtDirectory (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN VOLUME_SB         *Sb,
    IN UINT32            Inode 
)
{
    EFI_STATUS          Status;
    UINT32              BlockSize;
    UINT16              InodeSize;
    UINT32              BgdtBlock;
    VOLUME_BGDT         Bgdt;
    UINT64              Offset;
    UINT64              TmpRootSize;
    UINTN               RootPages;
    EFI_PHYSICAL_ADDRESS Allocate;
    UINT32              InodesPerGroup;
    UINT32              GroupNumber = 0;
    UINT32              InodePtr;
    UINT32              BGDTOffset = 0;

    BlockSize = 1024 << Sb->SB_BlockSize;
    if (BlockSize == 1024)
    {
        BgdtBlock = 2;
    } else {
        BgdtBlock = 1;
    }

    //
    // Read in the Block Group Descriptor Table
    //
    Offset = Mul64(BlockSize, BgdtBlock);

    InodePtr = Inode - 1; // Work with a 0-based Inode pointer
    InodesPerGroup = Sb->SB_InodesPerGroup;
    //
    // If the Inode is far out on the volume, it will not be in the first group
    // Find out which group it is in, and get the offset to it's descriptor in
    // the Block Group Descriptor Table
    //
    if ( InodePtr > InodesPerGroup ) {
        GroupNumber = InodePtr / InodesPerGroup;
        InodePtr = InodePtr % InodesPerGroup;
        BGDTOffset = GroupNumber * sizeof( VOLUME_BGDT );
    }
    Offset += BGDTOffset;

    Status = ReadDevice ( PeiServices, 
                         Volume, 
                         Offset, 
                         sizeof(VOLUME_BGDT), 
                         &Bgdt );
    if ( EFI_ERROR(Status) ) 
    {
        PEI_TRACE((-1, PeiServices, "\nRead BGBT error\n"));
        return EFI_NOT_FOUND;
    }

    InodeBlock = Bgdt.BGDT_InodeTableBlk;
    InodeSize = Sb->SB_InodeStrucSize;

    //
    // Read in the directory's inode.
    //
    Offset = Mul64( BlockSize, InodeBlock ) + \
             Mul64( InodePtr, InodeSize );
    Status = ReadDevice (PeiServices, 
                         Volume, 
                         Offset, 
                         sizeof(VOLUME_IT), 
                         &MemBlk->RootInode);
    if ( EFI_ERROR(Status) )
    {
        PEI_TRACE((-1, PeiServices, "\nRead dir inode error. Inode is %x\n", Inode));
        return EFI_NOT_FOUND;
    }

    TmpRootSize = MemBlk->RootInode.IT_SizeLo + Shl64( MemBlk->RootInode.IT_SizeHi, 0x20 );

    if (!RootAllocated) {
        RootPages = (UINTN) (Shr64( TmpRootSize, EFI_PAGE_SHIFT ) + ((TmpRootSize & EFI_PAGE_MASK) ? 1 : 0) );
        Status = (*PeiServices)->AllocatePages( PeiServices, EfiBootServicesData, RootPages, &Allocate );
        if ( EFI_ERROR( Status )) {
            PEI_TRACE((-1, PeiServices, "\nRead EXT dir allocate error\n"));
            return Status;
        }
        RootBuffer     = (UINT8*)( (UINTN)Allocate );
        RootBufferSize = EFI_PAGES_TO_SIZE( RootPages );
        MemSet( RootBuffer, RootBufferSize, 0 );
        RootAllocated = TRUE;
    } else {
        if ( TmpRootSize > RootBufferSize ) TmpRootSize = RootBufferSize;
    }
    
    Status = ReadExtFile( PeiServices, 
                          Volume, 
                          Sb, 
                          &MemBlk->RootInode, 
                          RootBuffer, 
                          &TmpRootSize );
    RootSize = (UINT32)TmpRootSize;

    return Status;
}

/**
    Reads recovery capsule from Ext(n) volume

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FileName - recovery capsule file name
    @param FileSize - pointer to size of provided buffer
    @param Buffer - pointer to buffer to store data


    @retval EFI_STATUS

**/
EFI_STATUS
ProcessExtVolume (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN OUT UINTN         *FileSize,
    OUT VOID             *Buffer 
)
{

    EFI_STATUS          Status;
    UINT32              i;
    UINT32              Inode;
    UINT64              TmpFileSize;
    UINTN               NumberOfFiles;
    UINT64              Offset;
    UINT32              BlockSize;
    UINT16              InodeSize;
    UINT8               *TmpPtr;
    UINT32              InodesPerGroup;
    UINT32              GroupNumber;
    UINT32              BGDTOffset;
    UINT32              BgdtBlock;
    VOLUME_BGDT         Bgdt;
#if MATCH_VOLUME_NAME
    CHAR8 *VolumeName = CONVERT_TO_STRING(VOLUME_NAME);
#endif
    
    Status = ReadDevice( PeiServices, Volume, 0, 512, &MemBlk->Sb );

    if ( EFI_ERROR( Status )) {
        PEI_TRACE((-1, PeiServices, "\nRead first sector failed\n"));
        return Status;
    }

    //
    // On an EXT(n) volume, the first sector will be all zeros
    //
    TmpPtr = (UINT8*)&MemBlk->Sb.SB_TotalInodes;
    for ( i=0; i<512; i++ )
    {
        if ( (UINT8)TmpPtr[i] != 0 )
        {
            PEI_TRACE((-1, PeiServices, "\nFirst sector not blank\n"));
            return EFI_NOT_FOUND; // Not an EXT(n) volume
        }
    }

    //
    // The Superblock is always 1024 bytes in on the volume
    //
    Status = ReadDevice( PeiServices, Volume, 1024, 512, &MemBlk->Sb );

    if ( !IsExt( &MemBlk->Sb ) ) {
        PEI_TRACE((-1, PeiServices, "\nIsExt failed\n"));
        return EFI_NOT_FOUND;
    }

#if MATCH_VOLUME_NAME
    if( Strlen(&MemBlk->Sb.SB_VolumeName[0]) == 0 ) return EFI_NOT_FOUND; // No vol name
    PEI_TRACE((-1, PeiServices, "\nVolume name is %s\n", &MemBlk->Sb.SB_VolumeName[0]));
    if( !(FileCompare(&MemBlk->Sb.SB_VolumeName[0], VolumeName, FALSE, Strlen(VolumeName))) ) {
        PEI_TRACE((-1, PeiServices, "\nName did not match\n"));
        return EFI_NOT_FOUND;
    }
#endif

    RootAllocated = FALSE;
    //
    // The root is always Inode 2.
    //
    Status = ReadExtDirectory( PeiServices, Volume, &MemBlk->Sb, 2 );

    if ( EFI_ERROR( Status ) ) {
        PEI_TRACE((-1, PeiServices, "\nRead EXT root failed\n"));
        return Status;
    }

#if SEARCH_PATH
    gExtVolume = Volume;
    gSb = &MemBlk->Sb;
#endif    

    AmiGetFileListFromExtVolume(PeiServices, (UINT8*)RootBuffer, RootSize, &NumberOfFiles, ExtRecoveryFiles);

    if ( NumberOfFiles == 0 )
        return EFI_NOT_FOUND;

    BlockSize = 1024 << MemBlk->Sb.SB_BlockSize;
    InodeSize = MemBlk->Sb.SB_InodeStrucSize;
    if (BlockSize == 1024)
    {
        BgdtBlock = 2;
    } else {
        BgdtBlock = 1;
    }

    //
    // Read in the Block Group Descriptor Table
    //
    Offset = Mul64(BlockSize, BgdtBlock);

    for(i = 0; i < NumberOfFiles; i++) {
        //
        // An EXT(n) directory  entry only contains the name and inode, so we have to
        // read the inode to get the size.
        //
        Inode = ExtRecoveryFiles[i]->DIR_Inode;

        Inode = Inode - 1; // Work with a 0-based Inodes
        InodesPerGroup = MemBlk->Sb.SB_InodesPerGroup;
        GroupNumber = 0;
        BGDTOffset = 0;
        //
        // If the Inode is far out on the volume, it will not be in the first group
        // Find out which group it is in, and get the offset to it's descriptor in
        // the Block Group Descriptor Table
        //  
        if ( Inode > InodesPerGroup ) {
            GroupNumber = Inode / InodesPerGroup;
            Inode = Inode % InodesPerGroup;
            BGDTOffset = GroupNumber * sizeof( VOLUME_BGDT );
        }

        Offset += BGDTOffset;

        Status = ReadDevice ( PeiServices, 
                             Volume, 
                             Offset, 
                             sizeof(VOLUME_BGDT), 
                             &Bgdt );
        if ( EFI_ERROR(Status) )
        {
            PEI_TRACE((-1, PeiServices, "\nRead BGBT error\n"));
            return EFI_NOT_FOUND;
        }
        InodeBlock = Bgdt.BGDT_InodeTableBlk; // Block where this file's Inode is

        Offset = Mul64(BlockSize, InodeBlock) + \
                 Mul64((Inode), InodeSize);
        Status = ReadDevice (PeiServices,  // Read in the file's Inode
                             Volume, 
                             Offset, 
                             sizeof(VOLUME_IT), 
                             &MemBlk->FileInode);
        if ( EFI_ERROR(Status) ) continue;

        TmpFileSize = MemBlk->FileInode.IT_SizeLo + Shl64(MemBlk->FileInode.IT_SizeHi, 0x20);
        PEI_TRACE((-1, PeiServices, "\nSize of file found is %lx\n", TmpFileSize));
        if ( *FileSize < (UINTN)TmpFileSize )
            continue;

        Status = ReadExtFile( PeiServices, 
                              Volume, 
                              &MemBlk->Sb, 
                              &MemBlk->FileInode, 
                              Buffer, 
                              &TmpFileSize );
        if ( EFI_ERROR( Status )) {
            PEI_TRACE((-1, PeiServices, "\nRead of file returned %r\n", Status));
            return Status;
        }

        if(AmiIsValidFile( Buffer, (UINTN)TmpFileSize) ) {
            *FileSize = (UINTN)TmpFileSize;
            return EFI_SUCCESS;
        }
    }
    
    return EFI_NOT_FOUND;
}

/**
    Reads recovery capsule from Ext(n) device. Tries to discover primary partitions
    and search for capsule there.

               
    @param PeiServices - pointer to PEI services
    @param Volume - pointer to volume description structure
    @param FileName - recovery capsule file name
    @param FileSize - pointer to size of provided buffer
    @param Buffer - pointer to buffer to store data


    @retval EFI_STATUS

**/
EFI_STATUS 
ProcessExtDevice (
    IN EFI_PEI_SERVICES  **PeiServices,
    IN OUT RC_VOL_INFO   *Volume,
    IN OUT UINTN         *FileSize,
    OUT VOID             *Buffer 
)
{
    EFI_STATUS          Status = 0;
    EFI_STATUS          Status2 = 0;

    //
    // Assume the volume is floppy-formatted.
    //
    Volume->PartitionOffset = 0; // Reset this to zero
    Status = ProcessExtVolume( PeiServices, Volume, FileSize, Buffer );

    if ( !EFI_ERROR( Status )) {
        return Status;
    }

    //
    // Not floppy formatted, look for partitions. Read sector 0 (MBR).
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
        if ( !EFI_ERROR(Status) ) {
            Status2 = ProcessExtVolume( PeiServices, Volume, FileSize, Buffer );
            if ( !EFI_ERROR(Status2) ) {
                return Status2;
            }
        }
    } while (Status == EFI_SUCCESS);

    return EFI_NOT_FOUND;
}

/**
    Find the specified file in the EXT directory at DirPtr


    @param DirPtr - Pointer to a buffer containing the directory
    @param DirSize - Size of the directory
    @param FileName - Name of the file to find
    @param EntryPtr - Pointer to buffer containing directory entry of the file that was found.

         
    @retval TRUE - File found 
            FALSE - File not found

**/
BOOLEAN 
FindFileInExtDirectory (
    IN  UINT8               *DirPtr,
    IN  UINT32              DirSize,
    IN  VOID                *FileName,
    OUT DIR_ENTRY_EXT       **EntryPtr
)
{
    DIR_ENTRY_EXT           *TmpPtr;
    UINT16                  EntryLength;
    UINT8                   NameLength;
    UINT8                   i;
    CHAR8                   TmpFileName[128];

    do { // do loop handling entries in the directory

        TmpPtr = ( DIR_ENTRY_EXT* )&DirPtr[0];
        EntryLength = TmpPtr->DIR_EntryLength;
        if ( EntryLength == 0 ) break; // End of directory, file not found
        NameLength = TmpPtr->DIR_NameLength;

        for ( i=0; i<NameLength; i++ )
        {
            TmpFileName[i] = TmpPtr->DIR_Name[i];
        }
        TmpFileName[i] = 0; // Zero-terminate name

        if(!EFI_ERROR(FileSearch( (CHAR8*)FileName, 
                                 TmpFileName, 
                                 FALSE, 
                                 NameLength)) ) {
            *EntryPtr = TmpPtr; // Save pointer to this entry
            return TRUE;
        }

        DirPtr += EntryLength;
        DirSize -= EntryLength;

    } while (DirSize);

    return FALSE;
}

/**
    Gets a list of valid recovery files from an EXT(n) volume.
    As currently written, gets only one file.


    @param Root - Pointer to a buffer containing the root directory
    @param RootSize - Size of the root directory
    @param NumberOfFiles - Pointer to number of files found
    @param Buffer - Pointer to buffer containing index entry ofthe file that was found.

         
    @retval None returned in variables.


    @note  
        This is an e-linked function, which can be replaced.

**/
VOID 
AmiGetFileListFromExtVolume (
    EFI_PEI_SERVICES        **PeiServices,
    IN  UINT8               *Root,
    IN  UINT32              RootSize,
    OUT UINTN               *NumberOfFiles,
    OUT DIR_ENTRY_EXT       **Buffer
)
{
    DIR_ENTRY_EXT *EntryPtr = NULL;
    VOID *FileName;
    UINTN FileSize;
    EFI_STATUS Status;
    CHAR8 *FilePath;
    CHAR8 *DirPtr;
    CHAR8 LocalPath[256];
#if SEARCH_PATH
    CHAR8 *NextName;
    UINT32 Inode;
#endif

    *NumberOfFiles = 0;     // no files found yet

    Status = GetRecoveryFileInfo(PeiServices, &FileName, &FileSize, NULL);
    if( EFI_ERROR(Status) )
        return;
#if SEARCH_PATH
    AddRecoveryPath(&FileName);
#endif

    MemCpy( LocalPath, FileName, Strlen(FileName)+1 );
    FilePath = &LocalPath[0];
    DirPtr = Root;

#if SEARCH_PATH
    NextName = FilePath;

    while ( IsolateName (&FilePath, &NextName) == FALSE ) {
        //
        // Find path name in directory
        //
        if ( FindFileInExtDirectory(DirPtr, RootSize, FilePath, &EntryPtr) ) {
            //
            // An EXT(n) directory  entry only contains the name and inode, so we have to
            // read the inode to get the size.
            //
            Inode = EntryPtr->DIR_Inode;
            Status = ReadExtDirectory( PeiServices, gExtVolume, gSb, Inode );
            if ( EFI_ERROR(Status) ) {
                PEI_TRACE((-1, PeiServices, "\nDirectory not read\n"));
                return; // not found
            }
            FilePath = NextName;
        } else return; // not found
    } // end while

    if ( FindFileInExtDirectory( DirPtr, RootSize, FilePath, &EntryPtr )) {
        Buffer[*NumberOfFiles] = EntryPtr; // Save pointer to this entry
        *NumberOfFiles = 1;
    }
    return;
#else
    if ( FindFileInExtDirectory( DirPtr, RootSize, FilePath, &EntryPtr )) {
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
