//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AhciController.c
    Provides Access to AHCI Controller

**/

//---------------------------------------------------------------------------

#include "AhciBus.h"

//---------------------------------------------------------------------------

#if BOOT_SECTOR_WRITE_PROTECT
#include <Protocol/AmiBlockIoWriteProtection.h>
extern AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL *AmiBlkWriteProtection;
#endif

BOOLEAN gPortReset = FALSE;             // Avoid Re-entry
BOOLEAN gSoftReset = FALSE;             // Avoid Re-entry
UINT64  gCommandListBaseAddress = 0;
UINT64  gFisBaseAddress;
extern  AHCI_PLATFORM_POLICY_PROTOCOL   *AhciPlatformPolicy;;

/**
    Read from the Sata ATA Device

    @param This 
    @param MediaId 
    @param IN EFI_LBA                      LBA,
    @param BufferSize 
    @param Buffer 

    @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI 
SataBlkRead (
    IN EFI_BLOCK_IO_PROTOCOL    *This,
    IN UINT32                   MediaId,
    IN EFI_LBA                  LBA,
    IN UINTN                    BufferSize,
    OUT VOID                    *Buffer
)
{
    return SataAtaBlkReadWrite(This, MediaId, LBA, BufferSize, Buffer, 0);
}

/**
    Write to Sata ATA Device

    @param This 
    @param MediaId 
    @param IN EFI_LBA                      LBA,
    @param BufferSize 
    @param Buffer 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI 
SataBlkWrite (
    IN EFI_BLOCK_IO_PROTOCOL    *This,
    IN UINT32                   MediaId,
    IN EFI_LBA                  LBA,
    IN UINTN                    BufferSize,
    OUT VOID                    *Buffer
)
{
#if BOOT_SECTOR_WRITE_PROTECT
    EFI_STATUS  Status;
    
    if(AmiBlkWriteProtection != NULL) {
        // Get user input
        Status = AmiBlkWriteProtection->BlockIoWriteProtectionCheck ( 
                                                    AmiBlkWriteProtection,
                                                    This,
                                                    LBA,
                                                    BufferSize
                                                    );
        // Abort operation if denied
        if(Status == EFI_ACCESS_DENIED) {
            return Status;
        }
    }
#endif

    return SataAtaBlkReadWrite(This, MediaId, LBA, BufferSize, Buffer, 1);
}

/**
    Read/Write to/from Sata ATA Device

    @param This 
    @param MediaId 
    @param IN EFI_LBA                      LBA,
    @param BufferSize 
    @param Buffer 
    @param BOOLEAN                         READWRITE

    @retval EFI_STATUS

    @note  
      1. Check for validity of the input
      2. Issue DMA or PIO Read/Write call.

**/

EFI_STATUS
SataAtaBlkReadWrite (
    IN EFI_BLOCK_IO_PROTOCOL    *This,
    IN UINT32                   MediaId,
    IN EFI_LBA                  LBA,
    IN UINTN                    BufferSize,
    OUT VOID                    *Buffer,
    BOOLEAN                     READWRITE
)
{
    EFI_STATUS               Status = EFI_DEVICE_ERROR;
    SATA_DEVICE_INTERFACE    *SataDevInterface = ((SATA_BLOCK_IO *)This)->SataDevInterface;
    EFI_BLOCK_IO_MEDIA       *BlkMedia = This->Media;
    UINTN                    DataN;
    UINTN                    BufferAddress;

    // Check if Media ID matches
    if (BlkMedia->MediaId != MediaId) return EFI_MEDIA_CHANGED;

    if (BufferSize == 0) return EFI_SUCCESS;

    // If IoAlign values is 0 or 1, means that the buffer can be placed 
    // anywhere in memory or else IoAlign value should be power of 2. To be
    // properly aligned the buffer address should be divisible by IoAlign  
    // with no remainder. 
    BufferAddress = (UINTN)Buffer;
    if((BlkMedia->IoAlign > 1 ) && (BufferAddress % BlkMedia->IoAlign)) {
        return EFI_INVALID_PARAMETER;
    }

    // Check whether the block size is multiple of BlkMedia->BlockSize
    DataN = BufferSize % BlkMedia->BlockSize;
    if (DataN) return EFI_BAD_BUFFER_SIZE;

    // Check for Valid start LBA #
    if (LBA > BlkMedia->LastBlock) return EFI_INVALID_PARAMETER;

    // Check for Valid End LBA #
    DataN = BufferSize / BlkMedia->BlockSize;
    if (LBA + DataN > BlkMedia->LastBlock + 1) return EFI_INVALID_PARAMETER;

    #if IDEBUSMASTER_SUPPORT
    if (DMACapable(SataDevInterface)) {     
    Status = SataReadWriteBusMaster (SataDevInterface, Buffer, BufferSize, LBA, 
                        READWRITE ? SataDevInterface->WriteCommand : SataDevInterface->ReadCommand, 
                        READWRITE);
        return Status;
    }
    #endif

    Status = SataReadWritePio (SataDevInterface, Buffer, BufferSize, LBA, 
                        READWRITE ? SataDevInterface->WriteCommand : SataDevInterface->ReadCommand, 
                        READWRITE);

    return Status;
}

/**
    Read from the Sata ATAPI Device

    @param This 
    @param MediaId 
    @param IN EFI_LBA                      LBA,
    @param BufferSize 
    @param Buffer 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI 
SataAtapiBlkRead(
    IN EFI_BLOCK_IO_PROTOCOL    *This,
    IN UINT32                   MediaId,
    IN EFI_LBA                  LBA,
    IN UINTN                    BufferSize,
    OUT VOID                    *Buffer
 )
{
    return SataAtapiBlkReadWrite(This, MediaId, LBA, BufferSize, Buffer, 0);
}

/**
    Write to Sata ATAPI Device

    @param This 
    @param MediaId 
    @param IN EFI_LBA                      LBA,
    @param BufferSize 
    @param Buffer 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI 
SataAtapiBlkWrite (
    IN EFI_BLOCK_IO_PROTOCOL    *This,
    IN UINT32                   MediaId,
    IN EFI_LBA                  LBA,
    IN UINTN                    BufferSize,
    OUT VOID                    *Buffer
)
{
    return SataAtapiBlkReadWrite(This, MediaId, LBA, BufferSize, Buffer, 1);
}

/**
    Read/Write to/from Sata ATAPI Device

        
    @param This 
    @param MediaId 
    @param IN EFI_LBA                      LBA,
    @param BufferSize 
    @param Buffer 

    @retval EFI_STATUS

    @note  
      1. Check for validity of Inputs
      2. Check whether Media is present or not
      3. Issue ATAPi Packet command

**/
EFI_STATUS
SataAtapiBlkReadWrite(
    IN EFI_BLOCK_IO_PROTOCOL    *This,
    IN UINT32                   MediaId,
    IN EFI_LBA                  LBA,
    IN UINTN                    BufferSize,
    OUT VOID                    *Buffer,
    BOOLEAN                     READWRITE
 )
{
    EFI_STATUS               Status = EFI_DEVICE_ERROR;
    SATA_DEVICE_INTERFACE    *SataDevInterface = ((SATA_BLOCK_IO *)This)->SataDevInterface;
    EFI_BLOCK_IO_MEDIA       *BlkMedia = This->Media;
    UINTN                    DataN;
    ATAPI_DEVICE             *AtapiDevice = SataDevInterface->AtapiDevice;
    COMMAND_STRUCTURE        CommandStructure;
    INTN                     TotalNumberofBlocks;
    INTN                     TransferLength;
    UINTN                    BytesRemainingTobeRead;
    VOID                     *TempBuffer = Buffer;
    UINTN                    BufferAddress;


    //  Check if Media Present
    if (BlkMedia->MediaPresent == FALSE) {
        Status = DetectAtapiMedia(SataDevInterface);
        if (Status == EFI_SUCCESS) return EFI_MEDIA_CHANGED;
        if (Status == EFI_NO_MEDIA) return Status;
        return EFI_DEVICE_ERROR;
    }

    //  Check if Media ID matches
    if (BlkMedia->MediaId != MediaId) return EFI_MEDIA_CHANGED;

    if (BufferSize == 0) return EFI_SUCCESS;

    // If IoAlign values is 0 or 1, means that the buffer can be placed
    // anywhere in memory or else IoAlign value should be power of 2. To be
    // properly aligned the buffer address should be divisible by IoAlign
    // with no remainder.
    BufferAddress = (UINTN)Buffer;
    if((BlkMedia->IoAlign > 1 ) && (BufferAddress % BlkMedia->IoAlign)) {
        return EFI_INVALID_PARAMETER;
    }

    //  Check whether the block size is multiple of BlkMedia->BlockSize
    DataN = BufferSize % BlkMedia->BlockSize;
    if (DataN) return EFI_BAD_BUFFER_SIZE;

    //  Check for Valid start LBA #

    if (LBA > BlkMedia->LastBlock) return EFI_INVALID_PARAMETER;

    //  Check for Valid End LBA #8
    DataN = BufferSize / BlkMedia->BlockSize;
    if (LBA + DataN > BlkMedia->LastBlock + 1) return EFI_INVALID_PARAMETER;


    TotalNumberofBlocks = BufferSize / AtapiDevice->BlockSize;

    for (; TotalNumberofBlocks >  0; TotalNumberofBlocks -= TransferLength) {  

        ZeroMemory (&CommandStructure, sizeof(CommandStructure));

        // Calculate # of blocks to be transferred
        if (TotalNumberofBlocks > 0xffff) 
            TransferLength = 0xffff;
        else
            TransferLength = TotalNumberofBlocks;

        BytesRemainingTobeRead = TransferLength * AtapiDevice->BlockSize;

        if (READWRITE) CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = SataDevInterface->WriteCommand;
        else CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = SataDevInterface->ReadCommand;
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[1] = AtapiDevice->Lun << 5;
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[2] = (UINT8)(((UINT32) LBA) >>  24);
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[3] = (UINT8)(((UINT32) LBA) >> 16);
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[4] = (UINT8)(((UINT16) LBA) >> 8);
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[5] = (UINT8)(((UINT8) LBA) & 0xff);
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[7] = (UINT8) (TransferLength >> 8);        // MSB
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[8] = (UINT8) (TransferLength & 0xff);      // LSB
        CommandStructure.Buffer = TempBuffer;
        CommandStructure.ByteCount = (UINT32)BytesRemainingTobeRead;

        Status = ExecutePacketCommand(SataDevInterface, &CommandStructure, READWRITE);

        if (Status != EFI_SUCCESS) {

            // Some error has occurred
            // Check if Device is getting ready. If yes, wait till it gets ready
            if (AtapiDevice->Atapi_Status == BECOMING_READY) {
                Status = TestUnitReady(SataDevInterface);
            }

            if (Status == EFI_MEDIA_CHANGED ) {
                Status = DetectAtapiMedia(SataDevInterface);
                if (Status == EFI_SUCCESS) return EFI_MEDIA_CHANGED;    // Return Media Change
                if (Status == EFI_NO_MEDIA) {
                    SataDevInterface->SataBlkIo->BlkIo.Media->MediaPresent = FALSE;
                    return Status;
                }
            }
            return Status;
        }

        if (CommandStructure.ByteCount != BytesRemainingTobeRead) return EFI_DEVICE_ERROR;

        // Update pointer
         TempBuffer =  (UINT8*)TempBuffer +BytesRemainingTobeRead;
        LBA += TransferLength;
    }

    return Status;
}

/**
    Reset ATA device

    @param This 
    @param ExtendedVerification 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI 
SataReset (
    IN EFI_BLOCK_IO_PROTOCOL    *This,
    IN BOOLEAN                  ExtendedVerification
)
{
    return EFI_SUCCESS;
}

/**
    Flush the cache
        
    @param This 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI 
SataBlkFlush (
    IN EFI_BLOCK_IO_PROTOCOL    *This
)
{
    EFI_STATUS             Status = EFI_SUCCESS;
    COMMAND_STRUCTURE      CommandStructure;
    SATA_DEVICE_INTERFACE  *SataDevInterface = NULL;
    
    SataDevInterface = ((SATA_BLOCK_IO *)This)->SataDevInterface;
    
    ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
    
    if(SataDevInterface->DeviceType == ATA) {
        if (SataDevInterface->IdentifyData.Command_Set_Supported_83 & 0x400) {
            // 48Bit LBA Supported
            CommandStructure.Command = FLUSH_CACHE_EXT;
        } else {
            CommandStructure.Command = FLUSH_CACHE;
        }
        Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);
    } else if(SataDevInterface->DeviceType == ATAPI) {
        if (SataDevInterface->IdentifyData.Command_Set_Supported_83 & 0x1000) {
            // Flush Cache command supported
            CommandStructure.Command = FLUSH_CACHE;
            Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);
        }
    }
    
    return Status;
}


/**
    Issues Read/Write Command and Read/Write the data from/to the ATA device

        
    @param IdeBusInterface 
    @param Buffer 
    @param UINTN                           ByteCount,
    @param UINT64                          LBA
    @param ReadWriteCommand 
    @param ReadWrite Read/Write = 0/1

    @retval *Buffer, EFI_STATUS

    @note  
  1. Get the Max. number of sectors that can be Read/written in one Read/Write PIO command
  2. Update the Command Structure
  3. Issue ExecutePioDataCommand.
  4. If all the bytes are read exit else goto step 2 

**/

EFI_STATUS
EFIAPI 
SataReadWritePio (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN OUT VOID                 *Buffer,
    IN UINTN                    ByteCount,
    IN UINT64                   LBA,
    IN UINT8                    ReadWriteCommand,
    IN BOOLEAN                  READWRITE
) 
{
    EFI_STATUS           Status;
    UINT32               SectorCount;
    INT64                MaxSectorCount;
    INT64                Total_Number_Of_Sectors;
    COMMAND_STRUCTURE    CommandStructure;
    UINT32               SectorSize = ATA_SECTOR_BYTES;
    UINT8                *TempBuffer = Buffer;
    if (Check48BitCommand(ReadWriteCommand)) 
        MaxSectorCount = MAX_SECTOR_COUNT_PIO_48BIT;
    else 
        MaxSectorCount = MAX_SECTOR_COUNT_PIO;

    // Calculate Sector Size
    if((SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT14) && // WORD 106 valid? - BIT 14 - 1
       (!(SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT15)) && // WORD 106 valid? - BIT 15 - 0
       (SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT12)) { // WORD 106 bit 12 - Sectorsize > 256 words
        // The sector size is in words 117-118.
        SectorSize = (UINT32)(SataDevInterface->IdentifyData.Reserved_104_126[13] + \
                              (SataDevInterface->IdentifyData.Reserved_104_126[14] << 16)) * 2;
    }

    if (SectorSize > ByteCount) SectorSize = (UINT32)ByteCount;

    // Calculate the total number of Sectors to be transferred
    Total_Number_Of_Sectors = ByteCount / SectorSize ;

    for ( ;Total_Number_Of_Sectors > 0; Total_Number_Of_Sectors -= MaxSectorCount) {

        if (Total_Number_Of_Sectors > MaxSectorCount) SectorCount = 0;
        else SectorCount = (UINT32) Total_Number_Of_Sectors;

        ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

        if (Check48BitCommand (ReadWriteCommand)) {  
            CommandStructure.LBALowExp = (UINT8)Shr64(LBA,24);
            CommandStructure.LBAMidExp = (UINT8)Shr64(LBA,32);
            CommandStructure.LBAHighExp = (UINT8)Shr64(LBA,40);
            CommandStructure.Device = 0x40;                                 // 48Bit LBA
        }
        else {                                                              // 28 Bit LBA
            CommandStructure.Device = ((UINT8) ((UINT32) LBA >> 24) & 0x0f) | 0x40;   
        }
        
        CommandStructure.SectorCount = (UINT16) SectorCount;
        CommandStructure.LBALow = (UINT8)LBA;
        CommandStructure.LBAMid = (UINT8)(((UINT32)LBA >>8) & 0xff);
        CommandStructure.LBAHigh = (UINT8)(((UINT32)LBA >>16) & 0xff);
        CommandStructure.Command = ReadWriteCommand;

        CommandStructure.Buffer = TempBuffer;
        CommandStructure.ByteCount = (UINT32)(SectorCount == 0 ? MaxSectorCount : Total_Number_Of_Sectors) * SectorSize ;

        Status = ExecutePioDataCommand (SataDevInterface, &CommandStructure, READWRITE); // Returns # of bytes read

        if (EFI_ERROR(Status)) { 
            return EFI_DEVICE_ERROR; 
        }

        TempBuffer += CommandStructure.ByteCount;
        SectorCount = CommandStructure.ByteCount / SectorSize ;
        LBA += SectorCount;
    }
    return EFI_SUCCESS;
}

/**
    Issues Read/Write Command with SubCommand feature
    and Reads/Writes data to the ATA device.

        
    @param SataDevInterface 
    @param CommandStructure
    @param IN BOOLEAN                      READWRITE

    @retval EFI_STATUS

    @note  
  1. Get the Max. number of sectors that can be transferred in one Read/Write PIO command
  2. Update the Command Structure
  3. Issue ExecutePioDataCommand.

**/

EFI_STATUS 
EFIAPI 
SataPioDataOut (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN COMMAND_STRUCTURE        CommandStructure,
    IN BOOLEAN                  READWRITE
)
{
    EFI_STATUS           Status;
    UINT32               SectorCount;
    INT64                MaxSectorCount;
    INT64                Total_Number_Of_Sectors;
    UINT32               SectorSize = ATA_SECTOR_BYTES;
    UINT64               LBAHighDword = 0;
    
    if (Check48BitCommand(CommandStructure.Command)) { 
        MaxSectorCount = MAX_SECTOR_COUNT_PIO_48BIT;
        //	if 48 Bit LBA form Upper Dword
        LBAHighDword |= CommandStructure.LBAHighExp;
        LBAHighDword = ( Shl64(( Shl64( LBAHighDword, 8)| CommandStructure.LBAMidExp), 8)| CommandStructure.LBALowExp);
    } else { 
        MaxSectorCount = MAX_SECTOR_COUNT_PIO;
    }
    
    // Calculate Sector Size
    if((SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT14) && // WORD 106 valid? - BIT 14 - 1
       (!(SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT15)) && // WORD 106 valid? - BIT 15 - 0
       (SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT12)) { // WORD 106 bit 12 - Sectorsize > 256 words

        // The sector size is in words 117-118.
        SectorSize = (UINT32)(SataDevInterface->IdentifyData.Reserved_104_126[13] + \
                              (SataDevInterface->IdentifyData.Reserved_104_126[14] << 16)) * 2;
    }

    if (SectorSize > CommandStructure.ByteCount) {
         SectorSize = (UINT32)CommandStructure.ByteCount;
    }

    // Calculate the total number of Sectors to be transferred
    Total_Number_Of_Sectors = CommandStructure.ByteCount / SectorSize ;

    for ( ;Total_Number_Of_Sectors > 0; Total_Number_Of_Sectors -= MaxSectorCount) {

        if ( Total_Number_Of_Sectors > MaxSectorCount ) {
            SectorCount = 0;
        } else {
            SectorCount = (UINT32) Total_Number_Of_Sectors;
        }

        if (Check48BitCommand (CommandStructure.Command)) {
            // If 48Bit LBA then form Upper DWord  
            CommandStructure.Device     = 0x40;
        } else {      
            // 28 Bit LBA
            CommandStructure.Device = ((UINT8) CommandStructure.LBAHigh & 0x0f) | 0x40;   
        }

        CommandStructure.SectorCount = (UINT16) SectorCount;
        CommandStructure.ByteCount = (UINT32)(SectorCount == 0 ? MaxSectorCount : Total_Number_Of_Sectors) * SectorSize ;
  
        Status = ExecutePioDataCommand (SataDevInterface, &CommandStructure, READWRITE); // Returns # of bytes read
        if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

        SectorCount = CommandStructure.ByteCount / SectorSize ;
    }
    return EFI_SUCCESS;
}

/**
    Issues Read/Write Command and Read/Write the data from/to the ATA device
    using BusMaster

        
    @param SATA_DEVICE_INTERFACE           *SataDevInterface,
    @param Buffer 
    @param ByteCount 
    @param IN UINT64                       LBA,
    @param ReadWriteCommand 
    @param IN BOOLEAN                      READWRITE

    @retval EFI_STATUS

    @note  
  1. Get the Max. number of sectors that can be Read/written in one Read/Write Bus master command
  2. Update the Command Structure
  3. Issue ExecutePioDataCommand.
  4. If all the bytes are read exit else goto step 2 

**/

EFI_STATUS
SataReadWriteBusMaster (
    SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN OUT VOID              *Buffer,
    IN UINTN                 ByteCount,
    IN UINT64                LBA,
    IN UINT8                 ReadWriteCommand,
    IN BOOLEAN               READWRITE
) 
{
    EFI_STATUS           Status;
    UINTN                Total_Number_Of_Sectors,MaxSectorCount;
    UINTN                CurrentSectorCount,CurrentByteCount;
    UINT8                *TempBuffer = Buffer;
    COMMAND_STRUCTURE    CommandStructure;
    UINT32               SectorSize = ATA_SECTOR_BYTES;

    if (Check48BitCommand(ReadWriteCommand)) 
        MaxSectorCount = MAX_SECTOR_COUNT_PIO_48BIT;
    else 
        MaxSectorCount = MAX_SECTOR_COUNT_PIO;

    // Calculate Sector Size
    if((SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT14) && // WORD 106 valid? - BIT 14 - 1
       (!(SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT15)) && // WORD 106 valid? - BIT 15 - 0
       (SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT12)) { // WORD 106 bit 12 - Sectorsize > 256 words
        // The sector size is in words 117-118.
        SectorSize = (UINT32)(SataDevInterface->IdentifyData.Reserved_104_126[13] + \
                              (SataDevInterface->IdentifyData.Reserved_104_126[14] << 16)) * 2;
    }
    if (SectorSize > ByteCount) SectorSize = (UINT32)ByteCount;

    Total_Number_Of_Sectors = ByteCount / SectorSize ;

    do {
        if (Total_Number_Of_Sectors > MaxSectorCount) CurrentSectorCount = 0;
            else CurrentSectorCount = Total_Number_Of_Sectors;
        CurrentByteCount = (CurrentSectorCount == 0 ? MaxSectorCount : CurrentSectorCount) * SectorSize ;

        ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

        CommandStructure.Buffer = TempBuffer;
        CommandStructure.ByteCount = (UINT32)CurrentByteCount;
        CommandStructure.Command = ReadWriteCommand;

        if (Check48BitCommand (ReadWriteCommand)) {  
            // 48 Bit LBA   
            // Write the Upper LBA DWORD and Upper byte of Sector Count
            CommandStructure.LBALowExp = (UINT8)Shr64(LBA,24);
            CommandStructure.LBAMidExp = (UINT8) Shr64(LBA,32);
            CommandStructure.LBAHighExp = (UINT8) Shr64(LBA,40);
            CommandStructure.Device = 0x40;     // 48Bit LBA
        } else {                                                  // 28 Bit LBA
            CommandStructure.Device = ((UINT8) ((UINT32) LBA >> 24) & 0x0f) | 0x40;
        }

        CommandStructure.SectorCount = (UINT16) CurrentSectorCount;
        CommandStructure.LBALow = (UINT8)LBA;
        CommandStructure.LBAMid = (UINT8) (((UINT32)LBA >>8) & 0xff);
        CommandStructure.LBAHigh = (UINT8) (((UINT32)LBA >>16) & 0xff);

        Status = ExecuteDmaDataCommand (SataDevInterface, &CommandStructure, READWRITE); // Returns # of bytes read
        if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

        CurrentByteCount = CommandStructure.ByteCount;
        CurrentSectorCount = CurrentByteCount / SectorSize;

        TempBuffer += CurrentByteCount;
        Total_Number_Of_Sectors -= (CurrentSectorCount == 0 ? MaxSectorCount : CurrentSectorCount);
        LBA += (CurrentSectorCount == 0 ? MaxSectorCount : CurrentSectorCount);

    } while (Total_Number_Of_Sectors);

    return EFI_SUCCESS;

}

/**
    Checks if the command is for 48-bit LBA

    @param Command 

    @retval TRUE/FLASE

**/

BOOLEAN
Check48BitCommand (
    IN UINT8    Command
)
{
    if ( Command == READ_SECTORS_EXT || 
         Command == READ_MULTIPLE_EXT || 
         Command == WRITE_SECTORS_EXT || 
         Command == WRITE_MULTIPLE_EXT ||
         Command == READ_DMA_EXT    ||
         Command == WRITE_DMA_EXT ) 
        return TRUE;
    else
        return FALSE; 
}   

/**
    Issue a  Non-Data command to the SATA Device

    @param    SataDevInterface 
    @param    CommandStructure 

    @retval   EFI_STATUS     

    @note  
  1. Stop the Controller
  2. Check if the device is ready to accept a Command. 
  3. Build Command list
  4. Start the Controller.
  5. Wait till command completes. Check for errors.

**/ 

EFI_STATUS
EFIAPI 
ExecuteNonDataCommand (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN COMMAND_STRUCTURE        CommandStructure
)
{
    EFI_STATUS           Status;
    AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface = SataDevInterface->AhciBusInterface; 
    AHCI_COMMAND_LIST    *CommandList = (AHCI_COMMAND_LIST *)(UINTN) SataDevInterface->PortCommandListBaseAddr;
    AHCI_COMMAND_TABLE   *Commandtable = (AHCI_COMMAND_TABLE *)(UINTN)AhciBusInterface->PortCommandTableBaseAddr;
    UINT64               Timeout = 0;

    Status = StopController(AhciBusInterface, SataDevInterface,TRUE);
    if (EFI_ERROR(Status)) return Status;

    Status = ReadytoAcceptCmd(SataDevInterface);
    if (EFI_ERROR(Status)) {
        StopController(AhciBusInterface, SataDevInterface,FALSE);
        return Status;
    }

    BuildCommandList(SataDevInterface, CommandList, AhciBusInterface->PortCommandTableBaseAddr);
    BuildCommandFIS(SataDevInterface, CommandStructure, CommandList, Commandtable);

    // Data-in
    CommandList->Ahci_Cmd_W = 0; 
       
    // Update of Command Register
    Commandtable->CFis.Ahci_CFis_C = 1;

    StartController(AhciBusInterface, SataDevInterface, BIT00);

    if(CommandStructure.Timeout == 0) {
        Timeout = ATAPI_BUSY_CLEAR_TIMEOUT;
    } else {
		Timeout = CommandStructure.Timeout;
	}
    
    Status = WaitforCommandComplete(SataDevInterface, NON_DATA_CMD, (UINTN)Timeout);
    
    //  Stop Controller
    StopController(AhciBusInterface, SataDevInterface,FALSE);

    return Status;    
}

/**
    Map DMA BusMaster Buffer for the Input Buffer

    @param EFI_PCI_IO_PROTOCOL          *PciIO
    @param IN OUT VOID                  *CommandStructureBufferAddr
    @param IN OUT UINTN                 *ByteCountPtr
    @param IN BOOLEAN                   READWRITE
    @param EFI_PHYSICAL_ADDRESS         *MappedAddrPtr
    @param IN VOID                      **Mapping 

    @retval EFI_STATUS

**/ 

EFI_STATUS
MapBusMasterDMAAddress (
    EFI_PCI_IO_PROTOCOL          *PciIO, 
    IN OUT VOID                  *CommandStructureBufferAddr,
    IN OUT UINTN                 *ByteCountPtr,
    IN BOOLEAN                   READWRITE,
    EFI_PHYSICAL_ADDRESS         *MappedAddrPtr,
    IN VOID                      **Mapping
)
{
    // Find DMA BusMaster Buffer for the Input Buffer using PCIIO Protocol Mapping
    return (  PciIO->Map (
                          PciIO,
                          (READWRITE ? EfiPciIoOperationBusMasterRead : EfiPciIoOperationBusMasterWrite),
                          (void*)(CommandStructureBufferAddr),
                          ByteCountPtr,
                          (void*)MappedAddrPtr,
                          Mapping
                          ) ) ;

}

/**
    Execute PIO Data In/Out command

    @param SataDevInterface 
    @param CommandStructure 
    @param IN BOOLEAN                             READWRITE

    @retval EFI_STATUS, CommandStructure->ByteCount 

    @note  
      1. Stop the Controller
      2. Check if the device is ready to accept a Command. 
      3. Build Command list
      4. Start the Controller.
      5. Wait till command completes. Check for errors.

**/ 
EFI_STATUS
EFIAPI 
ExecutePioDataCommand (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN OUT COMMAND_STRUCTURE    *CommandStructure,
    IN BOOLEAN                  READWRITE
)
{
    EFI_STATUS               Status;
    AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface = SataDevInterface->AhciBusInterface;
    AHCI_COMMAND_LIST        *CommandList = (AHCI_COMMAND_LIST *)(UINTN) SataDevInterface->PortCommandListBaseAddr;
    AHCI_COMMAND_TABLE       *Commandtable = (AHCI_COMMAND_TABLE *)(UINTN)AhciBusInterface->PortCommandTableBaseAddr;
    VOID                     *Mapping=NULL;
    UINTN                    Bytes = CommandStructure->ByteCount;
    EFI_PHYSICAL_ADDRESS     MappedAddr;
    void                     *InputBuffer;
    UINT64                   Timeout = 0;

    if(AhciPlatformPolicy->PciMapAddressForDataTransfer) {
        // Find DMA BusMaster Buffer for the Input Buffer using PCIIO Protocol Mapping
        // if the Input Buffer is not NULL and Bytecount is not Zero
        if( (CommandStructure->Buffer != NULL) && (CommandStructure->ByteCount != 0) ) {
            Status = MapBusMasterDMAAddress ( SataDevInterface->AhciBusInterface->PciIO, 
                                              CommandStructure->Buffer,
                                              &Bytes,
                                              READWRITE, 
                                              &MappedAddr,
                                              &Mapping ); 
            if (EFI_ERROR(Status) || Bytes != CommandStructure->ByteCount) {
                if(Status == EFI_OUT_OF_RESOURCES) {
                    return EFI_BAD_BUFFER_SIZE;
                }
                return EFI_NOT_FOUND;
            }
            InputBuffer = (void *)CommandStructure->Buffer;
            CommandStructure->Buffer = (VOID *)MappedAddr;
        }
    }

    Status = StopController(AhciBusInterface, SataDevInterface,TRUE);

    if (EFI_ERROR(Status)) {
        goto PioError;
    }

    Status = ReadytoAcceptCmd(SataDevInterface);
    if (EFI_ERROR(Status)) {
        StopController(AhciBusInterface, SataDevInterface,FALSE);
        goto PioError;
    }

    BuildCommandList(SataDevInterface, CommandList, AhciBusInterface->PortCommandTableBaseAddr);
    BuildCommandFIS(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildAtapiCMD(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildPRDT(SataDevInterface, *CommandStructure, CommandList, Commandtable);

    if (READWRITE) {
        CommandList->Ahci_Cmd_W = 1;
    } else {
        CommandList->Ahci_Cmd_W = 0;
    }
    Commandtable->CFis.Ahci_CFis_C = 1;

    StartController(AhciBusInterface, SataDevInterface, BIT00);
    // For Security Erase command the time out value comes from Identify Data.    
    if(CommandStructure->Command == SECURITY_ERASE_UNIT ) {
        UINT32  EraseCommandTimeout;
        UINT32  EraseCommandTimeoutMax;
        UINT16  IdentifyDataEraseModeTimeoutValue;
        UINT8   *Buffer=(UINT8*)CommandStructure->Buffer;
        
        if(SataDevInterface->IdentifyData.Major_Revision_80 & BIT10){ // If supports ACS-3
            
            // BIT:1 of the Buffer[0] refers Enhanced Security Erase Mode.
            // If this Bit is set, Identify Data Word 90 is used for Erase command timeout
            // else Word 89 will be used for the timeout.
			
			if(Buffer[0] & 2 ) {
                IdentifyDataEraseModeTimeoutValue = SataDevInterface->IdentifyData.Time_Esecurity_Earse_90;
            } else {
                IdentifyDataEraseModeTimeoutValue = SataDevInterface->IdentifyData.Time_security_Earse_89;
            }
            
            // The definition of Identify Data Word 90 and Word 89 in ACS-3
            // Bit 15 is 1=Extended Time is reported in bits 14:0
            //        is 0=Time is reported in bits 7:0
            
            // ACS-3 Table A.40 & Table A.41
            // Table A.40 - Short format ENHANCED SECURITY ERASE TIME field
            // Value    Time
            // 0        Value not specified
            // 1..254   (Value*2) minutes
            // 255      >508 minutes
            //
            // Table A.41 - Extended format ENHANCED SECURITY ERASE TIME field
            // Value    Time
            // 0        Value not specified
            // 1..32766 (Value*2) minutes
            // 32767    >65532 minutes
                      
            if(IdentifyDataEraseModeTimeoutValue & BIT15){
                EraseCommandTimeout = (UINT32)(IdentifyDataEraseModeTimeoutValue & 0x7FFF);
                EraseCommandTimeoutMax = 32766;
            } else {
                EraseCommandTimeout = (UINT32)(IdentifyDataEraseModeTimeoutValue & 0xFF);
                EraseCommandTimeoutMax = 254;
            }  
        } else { // Older ATA/ATAPI Spec
            // BIT:1 of the Buffer[0] refers Enhanced Security Erase Mode.
            // If this Bit is set, Identify Data Word 90 is used for Erase command timeout
            // else Word 89 will be used for the timeout.
            if(Buffer[0] & 2 ) {
                EraseCommandTimeout = (UINT32)(SataDevInterface->IdentifyData.Time_Esecurity_Earse_90);
            } else {
                EraseCommandTimeout = (UINT32)(SataDevInterface->IdentifyData.Time_security_Earse_89);
            }
            EraseCommandTimeoutMax = 254;            
        }
        
        if(EraseCommandTimeout <= EraseCommandTimeoutMax) {
            EraseCommandTimeout = EraseCommandTimeout * 2 * 1000 * 60; //(Value * 2)Minutes
        } else {
            EraseCommandTimeout = 0; // Infinite Timeout
        } 
        
        Status = WaitforCommandComplete(SataDevInterface, PIO_DATA_IN_CMD, EraseCommandTimeout);
    } else {
        if(CommandStructure->Timeout == 0) {
            Timeout = SataDevInterface->DeviceType == ATA ? DMA_ATA_COMMAND_COMPLETE_TIMEOUT 
                                                        : DMA_ATAPI_COMMAND_COMPLETE_TIMEOUT;
        } else {
			Timeout = CommandStructure->Timeout;
		}
        Status = WaitforCommandComplete(SataDevInterface, PIO_DATA_IN_CMD, (UINTN)Timeout);
    }

    CommandStructure->ByteCount = CommandList->Ahci_Cmd_PRDBC;

    //  Stop Controller
    StopController(AhciBusInterface, SataDevInterface,FALSE);

PioError:

    // Unmap the Mapped Memory, Restore the Inputbuffer Address 
    if(AhciPlatformPolicy->PciMapAddressForDataTransfer) {
        if( (CommandStructure->Buffer != NULL) && (CommandStructure->ByteCount != 0) ) {
            SataDevInterface->AhciBusInterface->PciIO->Unmap ( 
                    SataDevInterface->AhciBusInterface->PciIO,
                    Mapping
                    );
                
            CommandStructure->Buffer = (void *)InputBuffer;
        }
    }

    return Status;    

}

/**

                   
    @param SataDevInterface 
    @param CommandStructure 
    @param IN BOOLEAN                              READWRITE

    @retval EFI_STATUS

    @note  
      1. Stop the Controller
      2. Check if the device is ready to accept a Command. 
      3. Build Command list
      4. Start the Controller.
      5. Wait till command completes. Check for errors.

**/ 

EFI_STATUS
EFIAPI 
ExecuteDmaDataCommand (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN COMMAND_STRUCTURE        *CommandStructure,
    IN BOOLEAN                  READWRITE
)
{
    EFI_STATUS               Status;
    AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface = SataDevInterface->AhciBusInterface;
    AHCI_COMMAND_LIST        *CommandList = (AHCI_COMMAND_LIST *)(UINTN) SataDevInterface->PortCommandListBaseAddr;
    AHCI_COMMAND_TABLE       *Commandtable = (AHCI_COMMAND_TABLE *)(UINTN)AhciBusInterface->PortCommandTableBaseAddr;
    VOID                     *Mapping=NULL;
    UINTN                    Bytes = CommandStructure->ByteCount;
    EFI_PHYSICAL_ADDRESS     MappedAddr;
    void                     *InputBuffer;
    UINT64                   Timeout = 0;
   
    
    if(AhciPlatformPolicy->PciMapAddressForDataTransfer) {
        // Find DMA BusMaster Buffer for the Input Buffer using PCIIO Protocol Mapping
        // if the Input Buffer is not NULL and Bytecount is not Zero
        if( (CommandStructure->Buffer != NULL) && (CommandStructure->ByteCount != 0) ) {
            Status = MapBusMasterDMAAddress (SataDevInterface->AhciBusInterface->PciIO, 
                                             CommandStructure->Buffer,
                                             &Bytes,
                                             READWRITE, 
                                             &MappedAddr,
                                             &Mapping); 
            if (EFI_ERROR(Status) || Bytes != CommandStructure->ByteCount) {
                if(Status == EFI_OUT_OF_RESOURCES) {
                    return EFI_BAD_BUFFER_SIZE;
                }
                return EFI_NOT_FOUND;
            }
            InputBuffer = (void *)CommandStructure->Buffer;
            CommandStructure->Buffer = (VOID *)MappedAddr;
        }
    }

    Status = StopController(AhciBusInterface, SataDevInterface,TRUE);
    if (EFI_ERROR(Status)) { 
        goto DmaDataError;
    }

    Status = ReadytoAcceptCmd(SataDevInterface);
    if (EFI_ERROR(Status)) {
        StopController(AhciBusInterface, SataDevInterface,FALSE);
        goto DmaDataError;
    }
    BuildCommandList(SataDevInterface, CommandList, AhciBusInterface->PortCommandTableBaseAddr);
    BuildCommandFIS(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildAtapiCMD(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildPRDT(SataDevInterface, *CommandStructure, CommandList, Commandtable);

    // Data-in
    if (READWRITE) {
        CommandList->Ahci_Cmd_W = 1;        
    }
    else {
        CommandList->Ahci_Cmd_W = 0  ;        
    }
    Commandtable->CFis.Ahci_CFis_C = 1;

    StartController(AhciBusInterface, SataDevInterface, BIT00);
    
    if(CommandStructure->Timeout == 0) {
           Timeout = SataDevInterface->DeviceType == ATA? DMA_ATA_COMMAND_COMPLETE_TIMEOUT \
                                                    : DMA_ATAPI_COMMAND_COMPLETE_TIMEOUT;
    } else {
 		Timeout = CommandStructure->Timeout;
	}

    Status = WaitforCommandComplete(SataDevInterface, DMA_DATA_IN_CMD, (UINTN)Timeout);
    
    if (!EFI_ERROR(Status)){

        //Check if the required BYTES have been received
        if (CommandList->Ahci_Cmd_PRDBC != CommandStructure->ByteCount){

            Status = EFI_DEVICE_ERROR;
        }
    }
    //  Stop Controller
    StopController(AhciBusInterface, SataDevInterface,FALSE);

    CommandStructure->ByteCount = CommandList->Ahci_Cmd_PRDBC;

DmaDataError:

    // Unmap the Mapped Memory, Restore the Inputbuffer Address 
    if(AhciPlatformPolicy->PciMapAddressForDataTransfer) {
        if( (CommandStructure->Buffer != NULL) && (CommandStructure->ByteCount != 0) ) {
            SataDevInterface->AhciBusInterface->PciIO->Unmap ( 
                    SataDevInterface->AhciBusInterface->PciIO,
                    Mapping
                    );
                    
            CommandStructure->Buffer = (void *)InputBuffer;
        }
    }
    return Status;
}

/**
    Execute a Atapi Packet command

    @param SataDevInterface 
    @param CommandStructure 
    @param IN BOOLEAN                              READWRITE

    @retval EFI_STATUS

    @note  
      1. Stop the Controller
      2. Check if the device is ready to accept a Command. 
      3. Build Command list
      4. Start the Controller.
      5. Wait till command completes. Check for errors.

**/ 

EFI_STATUS 
EFIAPI 
ExecutePacketCommand (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN COMMAND_STRUCTURE        *CommandStructure,
    IN BOOLEAN                  READWRITE
)
{
    EFI_STATUS               Status;
    AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface = SataDevInterface->AhciBusInterface;
    AHCI_COMMAND_LIST        *CommandList = (AHCI_COMMAND_LIST *)(UINTN) SataDevInterface->PortCommandListBaseAddr;
    AHCI_COMMAND_TABLE       *Commandtable = (AHCI_COMMAND_TABLE *)(UINTN)AhciBusInterface->PortCommandTableBaseAddr;
    ATAPI_DEVICE             *AtapiDevice = SataDevInterface->AtapiDevice;
    UINT8                    Port = SataDevInterface->PortNumber;
    UINT8                    Data8;
    VOID                     *Mapping=NULL;
    UINTN                    Bytes = CommandStructure->ByteCount;
    EFI_PHYSICAL_ADDRESS     MappedAddr;
    void                     *InputBuffer;
    UINT64                   Timeout = 0;

    if(AhciPlatformPolicy->PciMapAddressForDataTransfer) {
        // Find DMA BusMaster Buffer for the Input Buffer using PCIIO Protocol Mapping
        // if the Input Buffer is not NULL and Bytecount is not Zero
        if( (CommandStructure->Buffer != NULL) && (CommandStructure->ByteCount != 0) ) {
            Status = MapBusMasterDMAAddress (SataDevInterface->AhciBusInterface->PciIO, 
                                             CommandStructure->Buffer,
                                             &Bytes,
                                             READWRITE, 
                                             &MappedAddr,
                                             &Mapping); 

            if (EFI_ERROR(Status) || Bytes != CommandStructure->ByteCount) {
                // return Bad buffer size for SCT to pass in case of insufficient resources.
                if(Status == EFI_OUT_OF_RESOURCES) {
                    return EFI_BAD_BUFFER_SIZE;
                }
                return EFI_NOT_FOUND;
            }
            InputBuffer = (void *)CommandStructure->Buffer;
            CommandStructure->Buffer = (VOID *)MappedAddr;
        }
    }
    CommandStructure->LBAMid = (UINT8)(CommandStructure->ByteCount);
    CommandStructure->LBAHigh = (UINT8)(CommandStructure->ByteCount >> 8);
    CommandStructure->Command = PACKET_COMMAND;

    Status = StopController(AhciBusInterface, SataDevInterface,TRUE);
    if (EFI_ERROR(Status)) {
        goto PacketError;
    }

    Status = ReadytoAcceptCmd(SataDevInterface);
    if (EFI_ERROR(Status)) {
        StopController(AhciBusInterface, SataDevInterface,FALSE);
        goto PacketError;
    }
    BuildCommandList(SataDevInterface, CommandList, AhciBusInterface->PortCommandTableBaseAddr);
    BuildCommandFIS(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildAtapiCMD(SataDevInterface, *CommandStructure, CommandList, Commandtable);
    BuildPRDT(SataDevInterface, *CommandStructure, CommandList, Commandtable);

    if (READWRITE) {
        CommandList->Ahci_Cmd_W = 1;        
    }
    else {
        CommandList->Ahci_Cmd_W = 0;        
    }
    Commandtable->CFis.Ahci_CFis_C = 1;

    StartController(AhciBusInterface, SataDevInterface, BIT00);

    if(CommandStructure->Timeout == 0) {
        Timeout = SataDevInterface->DeviceType == ATA? DMA_ATA_COMMAND_COMPLETE_TIMEOUT \
                                                    : DMA_ATAPI_COMMAND_COMPLETE_TIMEOUT;
    } else {
		Timeout = CommandStructure->Timeout;
	}
    Status = WaitforCommandComplete(SataDevInterface, PIO_DATA_IN_CMD, (UINTN)Timeout);

    // Handle ATAPI device error
    if (EFI_ERROR(Status) && SataDevInterface->DeviceType == ATAPI) {
        Data8 = HBA_PORT_REG8 (AhciBusInterface, Port, HBA_PORTS_TFD);
        if (Data8 & IDE_CHK ){
            // Avoid requesting sense data for sense data command failure.
            if (CommandStructure->AtapiCmd.Ahci_Atapi_Command[0] != ATAPI_REQUEST_SENSE) {
                Status = HandleAtapiError(SataDevInterface);
            }
            goto PacketError;
        }
    }
    AtapiDevice->Atapi_Status = EFI_SUCCESS;

    CommandStructure->ByteCount = CommandList->Ahci_Cmd_PRDBC;

    //  Stop Controller
    StopController(AhciBusInterface, SataDevInterface,FALSE);

PacketError:

    // Unmap the Mapped Memory, Restore the Inputbuffer Address 
    if(AhciPlatformPolicy->PciMapAddressForDataTransfer) {
        if( (CommandStructure->Buffer != NULL) && (CommandStructure->ByteCount != 0) ) {
            SataDevInterface->AhciBusInterface->PciIO->Unmap ( 
                SataDevInterface->AhciBusInterface->PciIO,
                Mapping
                );

            CommandStructure->Buffer = (void *)InputBuffer;
        }
    }
    return Status;
}

/**
    Check for ATAPI Errors

    @param SataDevInterface 

    @retval EFI_STATUS  

    @note  
  1. Execute ATAPI Request Sense command.
  2. Check for Device getting ready, Media Change, No Media and other errors. Update AtapiDevice->Atapi_Status

**/
EFI_STATUS
HandleAtapiError (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{
    EFI_STATUS               Status;
    UINT8                    Data8 = 0;
    UINT8                    SenseData[256];
    COMMAND_STRUCTURE        CommandStructure;
    ATAPI_DEVICE             *AtapiDevice = SataDevInterface->AtapiDevice;
    AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface = SataDevInterface->AhciBusInterface;
    UINT8                    Port = SataDevInterface->PortNumber;

    AtapiDevice->Atapi_Status = DEVICE_ERROR;

    ZeroMemory (SenseData, 256);
    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

    CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = ATAPI_REQUEST_SENSE;
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[4] = 0xff;

    CommandStructure.ByteCount =  256;
    CommandStructure.Buffer = SenseData;

    Status = ExecutePacketCommand(SataDevInterface, &CommandStructure, 0);

    if (EFI_ERROR(Status)) {
        Data8 = HBA_PORT_REG8 (AhciBusInterface, Port, HBA_PORTS_TFD);
    }

    SataDevInterface->AtapiSenseDataLength = 0;

    // Check for DF and CHK
    if (Data8 & (IDE_DF | IDE_CHK)) { 
        goto exit_HandleAtapiError_with_Reset;
    }    

    if (!EFI_ERROR(Status)){
        // Store the SenseData which would be used by ScsiPassThruAtapi PassThru Interface.
        pBS->CopyMem( SataDevInterface->AtapiSenseData, SenseData, 256);
        SataDevInterface->AtapiSenseDataLength = CommandStructure.ByteCount;

        AtapiDevice->Atapi_Status = DEVICE_ERROR;
        Status = EFI_DEVICE_ERROR;              // Default Value    

        if (((SenseData[2] & 0xf) == 2) && (SenseData[12] == 0x3a)) {
            Status = EFI_NO_MEDIA;
            AtapiDevice->Atapi_Status = MEDIUM_NOT_PRESENT;
        }
        if (((SenseData[2] & 0xf) == 2) && (SenseData[12] == 0x04) && (SenseData[13] == 0x01)) {
            Status = EFI_MEDIA_CHANGED;
            AtapiDevice->Atapi_Status = BECOMING_READY;
        }

        if (((SenseData[2] & 0xf) == 6) && (SenseData[12] == 0x28)){
                Status = EFI_MEDIA_CHANGED;
                AtapiDevice->Atapi_Status = MEDIA_CHANGED;
        }

        if (((SenseData[2] & 0xf) == 7) && (SenseData[12] == 0x27)){
                Status = EFI_WRITE_PROTECTED;
                AtapiDevice->Atapi_Status = WRITE_PROTECTED_MEDIA;
        }

        if (((SenseData[2] & 0xf) == 6) && (SenseData[12] == 0x29)){
                AtapiDevice->Atapi_Status = POWER_ON_OR_DEVICE_RESET;
        }

        if (((SenseData[2] & 0xf) == 5) && (SenseData[0] == 0x70)){
                AtapiDevice->Atapi_Status = ILLEGAL_REQUEST;
        }
    }    

exit_HandleAtapiError_with_Reset:
        return Status;
}

/**
    Read/Write routine to PM ports

                   
    @param SataDevInterface 
    @param Port 
    @param RegNum 
    @param Data 
    @param IN BOOLEAN                      READWRITE       // TRUE for Write

    @retval EFI_STATUS

    @note  
      1. Update Command Structure for READ/Write Port Multiplier command
      2. Issue command
      3. Check for errors.
      4. Read the out data in case of READ.

**/
EFI_STATUS
ReadWritePMPort (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN UINT8                    Port,
    IN UINT8                    RegNum,
    IN OUT UINT32               *Data,
    IN BOOLEAN                  READWRITE
)
{
    EFI_STATUS               Status;
    COMMAND_STRUCTURE        CommandStructure;
    AHCI_RECEIVED_FIS        *PortFISBaseAddr = (AHCI_RECEIVED_FIS *)(UINTN)(SataDevInterface->PortFISBaseAddr);
    AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface = SataDevInterface->AhciBusInterface; 
    AHCI_COMMAND_LIST        *CommandList = (AHCI_COMMAND_LIST *)(UINTN) SataDevInterface->PortCommandListBaseAddr;
    AHCI_COMMAND_TABLE       *Commandtable = (AHCI_COMMAND_TABLE *)(UINTN)AhciBusInterface->PortCommandTableBaseAddr;

    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

    CommandStructure.Command = READ_PORT_MULTIPLIER;

    if (READWRITE) {
        CommandStructure.SectorCount = (UINT16) (*Data & 0xFF);
        CommandStructure.LBALow = (UINT8) (*Data >> 8);
        CommandStructure.LBAMid = (UINT8)(*Data >> 16);
        CommandStructure.LBAHigh = (UINT8)(*Data >> 24);
        CommandStructure.Command = WRITE_PORT_MULTIPLIER;
    }

    CommandStructure.Device = Port;
    CommandStructure.Features = RegNum;

    Status = StopController(AhciBusInterface, SataDevInterface,TRUE);
    if (EFI_ERROR(Status)) return Status;

    Status = ReadytoAcceptCmd(SataDevInterface);
    if (EFI_ERROR(Status)) {
        StopController(AhciBusInterface, SataDevInterface,FALSE);
        return Status;
    }

    BuildCommandList(SataDevInterface, CommandList, AhciBusInterface->PortCommandTableBaseAddr);
    BuildCommandFIS(SataDevInterface, CommandStructure, CommandList, Commandtable);

    // Data-in
    CommandList->Ahci_Cmd_W = 0; 
       
    // Update of Command Register
    Commandtable->CFis.Ahci_CFis_C = 1;

    // Update the Port Address
    CommandList->Ahci_Cmd_PMP = CONTROL_PORT;
    Commandtable->CFis.AHci_CFis_PmPort = CONTROL_PORT; 

    StartController(AhciBusInterface, SataDevInterface, BIT00);

    Status = WaitforCommandComplete(SataDevInterface, NON_DATA_CMD, TIMEOUT_1SEC);

    //  Stop Controller
    StopController(AhciBusInterface, SataDevInterface,FALSE);

    if (!READWRITE) {
        *Data = 0;
        if (!EFI_ERROR(Status)) {
            *Data = PortFISBaseAddr->Ahci_Rfis[12] |    
                    (PortFISBaseAddr->Ahci_Rfis[4] << 8) |  
                    (PortFISBaseAddr->Ahci_Rfis[5] << 16) |     
                    (PortFISBaseAddr->Ahci_Rfis[6] << 24);  
        }
    }

    return Status;
}

/**
    Return Identify data from SATA device

    @param SataDevInterface 

    @retval gIdentifyDataBuffer
    @retval EFI_STATUS
    @note  
      1. Build CommandStructure.
      2. Issue ExecutePioDataCommand

**/ 

EFI_STATUS
GetIdentifyData (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface 
)
{
    EFI_STATUS           Status;
    COMMAND_STRUCTURE    CommandStructure;
    IDENTIFY_DATA        tIdentifyData;

    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

    //  Read Identifydata
    CommandStructure.Buffer = &tIdentifyData;
    CommandStructure.ByteCount = sizeof(IDENTIFY_DATA);
    CommandStructure.Device = 0;
    CommandStructure.Command = SataDevInterface->DeviceType == ATA ? IDENTIFY_COMMAND : IDENTIFY_PACKET_COMMAND;

    Status = ExecutePioDataCommand (SataDevInterface, &CommandStructure, FALSE);
    
    if (CommandStructure.ByteCount != sizeof(IDENTIFY_DATA)) { Status = EFI_DEVICE_ERROR; }

    if(!EFI_ERROR(Status))
        pBS->CopyMem(&(SataDevInterface->IdentifyData), &tIdentifyData, sizeof(IDENTIFY_DATA)); 

    return Status;
}

/**
    Return ATAPI Inquiry data

    @param SataDevInterface 
    @param InquiryData 
    @param InquiryDataSize 

    @retval EFI_STATUS

    @note  
      1. Update CommandStructure
      2. Issue ExecutePioDataCommand

**/

EFI_STATUS
SataAtapiInquiryData (
    IN SATA_DEVICE_INTERFACE     *SataDevInterface,
    OUT UINT8                    *InquiryData,
    IN OUT UINT16                *InquiryDataSize
)
{
    EFI_STATUS           Status;
    COMMAND_STRUCTURE    CommandStructure;
    
    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.Buffer = InquiryData;
    CommandStructure.ByteCount = *InquiryDataSize;
    CommandStructure.Command = PACKET_COMMAND;
    CommandStructure.LBAMid = (UINT8)*InquiryDataSize;
    CommandStructure.LBAHigh = (UINT8)(*InquiryDataSize >> 8);

    CommandStructure.AtapiCmd.Ahci_Atapi_Command[0]=ATAPI_INQUIRY;
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[4]=(UINT8)*InquiryDataSize;

    Status = ExecutePioDataCommand (SataDevInterface, &CommandStructure, 0); 

    if (!EFI_ERROR(Status)) {
        *InquiryDataSize = (UINT16)CommandStructure.ByteCount;
    }

    return Status;
}

/**
    Detects whether a Media is present in the ATAPI Removable device or not.

    @param SataDevInterface 

    @retval EFI_STATUS

    @note  
      1. Issue Read Capacity command for CDROM or Read Format command for other ATAPI devices.
      2. If step 1  is successful, update last LBA, Block Size, Read/Write capable, Media ID

**/

EFI_STATUS
DetectAtapiMedia (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{
    UINT8                *InputData, LoopCount;
    ATAPI_DEVICE         *AtapiDevice = SataDevInterface->AtapiDevice;
    EFI_BLOCK_IO_MEDIA   *BlkMedia = SataDevInterface->SataBlkIo->BlkIo.Media;
    EFI_STATUS           Status ;
    UINT16               ByteCount = 256, Data16;
    COMMAND_STRUCTURE    CommandStructure;
    BOOLEAN              ReadCapacity=FALSE;
    
    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

    // Default Values
    BlkMedia->MediaPresent = FALSE;
    BlkMedia->LastBlock = 0x100;                            // Dummy value
    SataDevInterface->ReadCommand = ATAPI_READ_10;  
    SataDevInterface->WriteCommand = ATAPI_WRITE_10;

    Status = TestUnitReady(SataDevInterface);
    if ((Status != EFI_MEDIA_CHANGED) && (Status != EFI_SUCCESS)) {
        return Status;
    }

    // Issue Read Capacity command
    Status = pBS->AllocatePool (EfiBootServicesData, ByteCount, (VOID**)&InputData);
    if (EFI_ERROR(Status)) return Status;

    // For CDROM use Read Capacity command else use Read Format Command
    if (AtapiDevice->DeviceType == CDROM_DEVICE){
        BlkMedia->BlockSize = CDROM_BLOCK_SIZE;         // Default size
        AtapiDevice->BlockSize = (UINT16)(BlkMedia->BlockSize);
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = ATAPI_READ_CAPACITY;
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[1] = AtapiDevice->Lun << 5;
        Data16 = 8;
    }
    else {
        BlkMedia->BlockSize = LS120_BLOCK_SIZE;         // Default Size
        AtapiDevice->BlockSize = (UINT16)(BlkMedia->BlockSize);
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = ATAPI_READ_FORMAT_CAPACITIES;
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[1] = AtapiDevice->Lun << 5;
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[7] = (UINT8)(ByteCount >> 8);
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[8] = (UINT8)(ByteCount & 0xff);
        Data16 = ByteCount;
    }

    for (LoopCount  = 0; LoopCount < 5; LoopCount++) {          // 5sec loop
        ByteCount = Data16;
        ZeroMemory (InputData, ByteCount);
        CommandStructure.Buffer  = InputData;
        CommandStructure.ByteCount = ByteCount;
        Status = ExecutePacketCommand(SataDevInterface, &CommandStructure, 0);
        if(CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] == ATAPI_READ_FORMAT_CAPACITIES && 
            AtapiDevice->Atapi_Status == ILLEGAL_REQUEST) {
            //
            //If the Read Format Capacities not supported by device, try 
            //ReadCapacity command
            //
            ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
            BlkMedia->BlockSize = CDROM_BLOCK_SIZE;         // Default size
            AtapiDevice->BlockSize = (UINT16)(BlkMedia->BlockSize);
            CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = ATAPI_READ_CAPACITY;
            CommandStructure.AtapiCmd.Ahci_Atapi_Command[1] = AtapiDevice->Lun << 5;
            Data16 = 8;
            ReadCapacity=TRUE;
        }
        if (AtapiDevice->Atapi_Status == EFI_SUCCESS) break;
        if (AtapiDevice->Atapi_Status == MEDIUM_NOT_PRESENT) break;
    } 

    if (Status == EFI_SUCCESS) {
        if(ReadCapacity == TRUE) {
            BlkMedia->LastBlock = InputData[0] << 24 | InputData[1] << 16 | InputData[2] << 8 | InputData[3];
            BlkMedia->LastBlock--; 
            BlkMedia->MediaPresent = TRUE;
            BlkMedia->MediaId ++;
            BlkMedia->BlockSize = InputData[4] << 24 | InputData[5] << 16 | InputData[6] << 8 | InputData[7];
            AtapiDevice->BlockSize = BlkMedia->BlockSize;
            BlkMedia->ReadOnly = FALSE;
        } else if (AtapiDevice->DeviceType == CDROM_DEVICE) {
            BlkMedia->LastBlock = InputData[0] << 24 | InputData[1] << 16 | InputData[2] << 8 | InputData[3];
            BlkMedia->LastBlock--;
            BlkMedia->BlockSize = CDROM_BLOCK_SIZE;
            AtapiDevice->BlockSize = (UINT16)(BlkMedia->BlockSize);
            BlkMedia->MediaPresent = TRUE;
            BlkMedia->MediaId ++;
            BlkMedia->ReadOnly = TRUE;
        } else if (InputData[8] != 3) {         // No media present
            BlkMedia->LastBlock = InputData[4] << 24 | InputData[5] << 16 | InputData[6] << 8 | InputData[7];
            BlkMedia->LastBlock--; 
            BlkMedia->MediaPresent = TRUE;
            BlkMedia->MediaId ++;
            BlkMedia->BlockSize = InputData[9] << 16 | InputData[10] << 8 | InputData[11];
            BlkMedia->ReadOnly = FALSE;
            AtapiDevice->BlockSize = (UINT16)(BlkMedia->BlockSize);
        }

        // Update ReadOnly Status
        if (AtapiDevice->DeviceType != CDROM_DEVICE) {
            ByteCount = 256;
            ZeroMemory (InputData, ByteCount);
            ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
            CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = ATAPI_MODE_SENSE;
            CommandStructure.AtapiCmd.Ahci_Atapi_Command[2] = RETURN_ALL_PAGES;
            CommandStructure.AtapiCmd.Ahci_Atapi_Command[7] = (UINT8)(ByteCount >> 8);
            CommandStructure.AtapiCmd.Ahci_Atapi_Command[8] = (UINT8)(ByteCount & 0xff);
            CommandStructure.Buffer  = InputData;
            CommandStructure.ByteCount= ByteCount;
            Status = ExecutePacketCommand(SataDevInterface, &CommandStructure, 0);
            if ((Status == EFI_SUCCESS) && (ByteCount > 8)) {
                BlkMedia->ReadOnly = (InputData[3] & 0x80) != 0 ? TRUE : FALSE;
            }
        }
    }

    pBS->FreePool(InputData);
    return Status;

}

/**
    Return ODD type

    @param SataDevInterface 

    @retval EFI_STATUS

**/

EFI_STATUS
SataGetOddType (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN OUT UINT16               *OddType
)
{
    EFI_STATUS           Status;
    COMMAND_STRUCTURE    CommandStructure;
    UINT8                *ProfileData;    

    Status = pBS->AllocatePool (EfiBootServicesData,16,(VOID**)&ProfileData);

    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.Buffer = ProfileData;
    CommandStructure.ByteCount = 16;
    CommandStructure.Command = PACKET_COMMAND;
    CommandStructure.LBAMid = 16;
    CommandStructure.LBAHigh = 0;

    CommandStructure.AtapiCmd.Ahci_Atapi_Command[0]= ATAPI_GET_CONFIGURATION;
    // Get the Feature Descriptor.
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[1] = FEATURE_DISCRIPTOR;
    // Get the Profile list
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[3] = GET_PROFILE_LIST;
    // Response Data Size
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[8] = 0x10;

    Status = ExecutePioDataCommand (SataDevInterface, &CommandStructure, 0); 

    if (!EFI_ERROR(Status)) {
        // Get the Profile Number
        *OddType=(UINT16 )(((ProfileData[sizeof(GET_CONFIGURATION_HEADER)+4]) << 8) + ProfileData[sizeof(GET_CONFIGURATION_HEADER)+5]);
    }
    pBS->FreePool(ProfileData);
    return Status;
}

/**
    Return ODD Loading type information

        
    @param SataDevInterface 

    @retval 
        EFI_STATUS

**/
EFI_STATUS
SataGetOddLoadingType (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN OUT UINT8                *OddLoadingType
)
{
    EFI_STATUS           Status;
    COMMAND_STRUCTURE    CommandStructure;
    UINT8                *ProfileData;    

    Status = pBS->AllocatePool (EfiBootServicesData,
                                16,
                                (VOID**)&ProfileData);

    if (EFI_ERROR(Status)) {
        return Status;
    }

    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.Buffer = ProfileData;
    CommandStructure.ByteCount = 16;
    CommandStructure.Command = PACKET_COMMAND;
    CommandStructure.LBAMid = 16;
    CommandStructure.LBAHigh = 0;

    CommandStructure.AtapiCmd.Ahci_Atapi_Command[0]= ATAPI_GET_CONFIGURATION;
    // Get the Feature Descriptor.
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[1] = FEATURE_DISCRIPTOR;
    // Get the Removable Medium feature
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[3] = GET_REMOVEABLE_MEDIUM_FEATURE;
    // Response Data Size
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[8] = 0x10;

    Status = ExecutePioDataCommand (SataDevInterface, &CommandStructure, 0); 

    if (!EFI_ERROR(Status)) {
        // Get the ODD Loading Type
        *OddLoadingType=(UINT8 )(((ProfileData[sizeof(GET_CONFIGURATION_HEADER)+4]) & 0xE0) >> 5);
    }

    pBS->FreePool(ProfileData);
    return Status;
}

/**
    Issues Start/Stop unit Command

        
    @param SataDevInterface 

    @retval EFI_STATUS          EFI_SUCCESS           : If Media is accessible
    @retval EFI_NO_MEDIA
    @retval EFI_MEDIA_CHANGED
    @retval EFI_DEVICE_ERROR

    @note  
  1. Update CommandStructure for ATAPI_TEST_UNIT_READY command
  2. Issue ExecutePacketCommand
  3. Check if the device is ready to accept command, whether Media is present or not.

**/

EFI_STATUS
TestUnitReady (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{

    EFI_STATUS           Status = EFI_SUCCESS;
    ATAPI_DEVICE         *AtapiDevice = SataDevInterface->AtapiDevice;
    UINT16               LoopCount;
    COMMAND_STRUCTURE    CommandStructure;

    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = ATAPI_TEST_UNIT_READY;
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[1] = AtapiDevice->Lun << 5;
    CommandStructure.Buffer  = NULL;
    CommandStructure.ByteCount  =  0x100;

    for (LoopCount  = 0; LoopCount < 1000; LoopCount++) {           // 10sec loop ( 1000 * 10 msec = 10Sec)
        Status = ExecutePacketCommand(SataDevInterface, &CommandStructure, 0);
        if (Status == EFI_SUCCESS) break;
        if (AtapiDevice->Atapi_Status == MEDIUM_NOT_PRESENT) break;
        if (AtapiDevice->Atapi_Status == MEDIA_CHANGED) break;
        pBS->Stall(10000); // 10msec
    } 

    return Status;
}

/**
    Builds command list

    @param SataDevInterface 
    @param CommandList 
    @param CommandTableBaseAddr 

    @retval EFI_STATUS
  
    @note  
      1. Update CommandList bits
      2. Not all fields like Ahci_Cmd_A are updated.
      3. Port number is set to 0xF (Control port) if PM Port number is 0xFF.  

**/ 
EFI_STATUS
BuildCommandList (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN AHCI_COMMAND_LIST        *CommandList,
    IN UINT64                   CommandTableBaseAddr
)
{
    ZeroMemory (CommandList, sizeof(AHCI_COMMAND_LIST));
    // CommandList->Ahci_Cmd_A = SataDevInterface->DeviceType == ATAPI ? 1 : 0;      // set elsewhere 
    CommandList->Ahci_Cmd_P = 0;       
    CommandList->Ahci_Cmd_R = 0;       
    CommandList->Ahci_Cmd_B = 0;       
    CommandList->Ahci_Cmd_Rsvd1 = 0;       
    CommandList->Ahci_Cmd_PMP = SataDevInterface->PMPortNumber == 0xFF ? 0x0 : SataDevInterface->PMPortNumber;       
    CommandList->Ahci_Cmd_PRDTL = 0;       
    CommandList->Ahci_Cmd_PRDBC = 0;       
    CommandList->Ahci_Cmd_CTBA = (UINT32)CommandTableBaseAddr;       
    CommandList->Ahci_Cmd_CTBAU = (UINT32)(Shr64(CommandTableBaseAddr,32));   // Store Upper 32 bit value of CommandTableAddr

    return EFI_SUCCESS;
}

/**
    Build Command FIS

    @param SataDevInterface 
    @param CommandStructure 
    @param CommandList 
    @param Commandtable 

    @retval EFI_STATUS

    @note  
  1. Update Command FIS data area.
  2. Update the Command FIS length in Command List table  

**/ 
EFI_STATUS
BuildCommandFIS (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN COMMAND_STRUCTURE        CommandStructure,
    IN AHCI_COMMAND_LIST        *CommandList,
    IN AHCI_COMMAND_TABLE       *Commandtable
)
{
    ZeroMemory (Commandtable, sizeof(AHCI_COMMAND_TABLE));

    Commandtable->CFis.Ahci_CFis_Type = FIS_REGISTER_H2D;
    Commandtable->CFis.AHci_CFis_PmPort = SataDevInterface->PMPortNumber == 0xFF ? 0x0 : SataDevInterface->PMPortNumber;
    // Commandtable->CFis.Ahci_CFis_C = 1;          // Set elsewhere
    Commandtable->CFis.Ahci_CFis_Cmd = CommandStructure.Command;

    Commandtable->CFis.Ahci_CFis_Features = CommandStructure.Features;
    Commandtable->CFis.Ahci_CFis_FeaturesExp = CommandStructure.FeaturesExp;

    Commandtable->CFis.Ahci_CFis_SecNum = CommandStructure.LBALow;
    Commandtable->CFis.Ahci_CFis_SecNumExp = CommandStructure.LBALowExp;

    Commandtable->CFis.Ahci_CFis_ClyLow = CommandStructure.LBAMid;
    Commandtable->CFis.Ahci_CFis_ClyLowExp = CommandStructure.LBAMidExp;

    Commandtable->CFis.Ahci_CFis_ClyHigh = CommandStructure.LBAHigh;
    Commandtable->CFis.Ahci_CFis_ClyHighExp = CommandStructure.LBAHighExp;

    Commandtable->CFis.Ahci_CFis_SecCount = (UINT8)(CommandStructure.SectorCount);
    Commandtable->CFis.Ahci_CFis_SecCountExp = (UINT8)(CommandStructure.SectorCount >> 8);

    Commandtable->CFis.Ahci_CFis_DevHead = CommandStructure.Device;
    Commandtable->CFis.Ahci_CFis_Control = CommandStructure.Control;    

    CommandList->Ahci_Cmd_CFL = FIS_REGISTER_H2D_LENGTH / 4;

    return EFI_SUCCESS;
}

/**

    @param SataDevInterface 
    @param CommandStructure 
    @param CommandList 
    @param Commandtable 

    @retval EFI_STATUS

    @note  
      1. Copy Packet data to command table
      2. Set Atapi bit in Command List    

**/ 
EFI_STATUS
BuildAtapiCMD (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN COMMAND_STRUCTURE        CommandStructure,
    IN AHCI_COMMAND_LIST        *CommandList,
    IN AHCI_COMMAND_TABLE       *Commandtable
)
{
    pBS->CopyMem(&(Commandtable->AtapiCmd),&(CommandStructure.AtapiCmd),sizeof(AHCI_ATAPI_COMMAND));

    if (Commandtable->CFis.Ahci_CFis_Cmd == PACKET_COMMAND){ // Is it a packet command?         
        CommandList->Ahci_Cmd_A = 1;
    }
    return EFI_SUCCESS;
}

/**
    Build PRDT table

    @param SataDevInterface 
    @param CommandStructure 
    @param CommandList 
    @param Commandtable 

    @retval EFI_STATUS

    @note  
      1. Build as many PRDT table entries based on ByteCount.
      2. Set the I flag for the lasr PRDT table.
      3. Update PRDT table length in CommandList  

**/ 

EFI_STATUS
BuildPRDT (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN COMMAND_STRUCTURE        CommandStructure,
    IN AHCI_COMMAND_LIST        *CommandList,
    IN AHCI_COMMAND_TABLE       *Commandtable
)
{

    AMI_AHCI_BUS_PROTOCOL  *AhciBusInterface = SataDevInterface->AhciBusInterface; 
    UINT32                  ByteCount = CommandStructure.ByteCount;
    UINT16                  Prdtlength = 0;
    AHCI_COMMAND_PRDT       *PrdtTable = &(Commandtable->PrdtTable);

    for (;ByteCount; PrdtTable++ ){
        PrdtTable->Ahci_Prdt_DBA = (UINT32)(UINTN)CommandStructure.Buffer;
        PrdtTable->Ahci_Prdt_DBAU = (UINT32)Shr64((UINTN)CommandStructure.Buffer, 32);
        PrdtTable->Ahci_Prdt_DBC = ByteCount >= PRD_MAX_DATA_COUNT ? (PRD_MAX_DATA_COUNT - 1) : (ByteCount - 1);
        ByteCount -= (PrdtTable->Ahci_Prdt_DBC + 1);
        PrdtTable->Ahci_Prdt_I = 0;
        Prdtlength+= sizeof(AHCI_COMMAND_PRDT);
        if ((UINT32)(Prdtlength + 0x80) >= AhciBusInterface->PortCommandTableLength) {
            //ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
            break;
        }
        CommandStructure.Buffer = (UINT8 *)CommandStructure.Buffer + PrdtTable->Ahci_Prdt_DBC + 1;
    }
    //  Set I flag only for the last entry.
    PrdtTable -- ;
    PrdtTable->Ahci_Prdt_I = 1;
    CommandList->Ahci_Cmd_PRDTL = Prdtlength / sizeof(AHCI_COMMAND_PRDT);

    return EFI_SUCCESS;
}

/**

    @param AhciBusInterface 
    @param SataDevInterface 
    @param CIBitMask 

    @retval EFI_STATUS

    @note  
      1. Clear Status register
      2. Enable FIS and CR running bit
      3. Enable Start bit
      4. Update CI bit mask

**/ 

EFI_STATUS
StartController (
    IN AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface, 
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN UINT32                   CIBitMask
)
{
    UINT8       Port = SataDevInterface->PortNumber;

    // Clear Status
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); 
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR); 

    // Clear FIS Receive area
    ZeroMemory ((VOID *)(UINTN)SataDevInterface->PortFISBaseAddr, RECEIVED_FIS_SIZE);

    // Enable ST
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_ST);

    // Enable Command Issued
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_CI, CIBitMask);

    return EFI_SUCCESS;
}

/**
    Wait till command completes

    @param SataDevInterface 
    @param CommandType 
    @param TimeOut 

    @retval EFI_STATUS

    @note  
      1. Check for SError bits. If set return error.
      2. For PIO IN/Out and Packet IN/OUT command wait till PIO Setup FIS is received
      3. If D2H register FIS is received, exit the loop.
      4. Check for SError and TFD bits.

**/

EFI_STATUS
EFIAPI 
WaitforCommandComplete (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN COMMAND_TYPE             CommandType,
    IN UINTN                    TimeOut    
)
{

    AMI_AHCI_BUS_PROTOCOL       *AhciBusInterface = SataDevInterface->AhciBusInterface; 
    UINT8                       Port = SataDevInterface->PortNumber;
    UINT32                      Data32_SERR, Data32_IS, i;
    BOOLEAN                     PxSERR_ERROR = FALSE, PIO_SETUP_FIS = FALSE;
    volatile AHCI_RECEIVED_FIS  *FISReceiveAddress = (AHCI_RECEIVED_FIS *)(UINTN)SataDevInterface->PortFISBaseAddr;
    UINTN                       TimeOutCount = TimeOut;
    EFI_STATUS                  Status;

    i=1;
    do {
        pBS->Stall(500);

        //  Check for Error bits
        Data32_SERR = HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_SERR);
        if (Data32_SERR & HBA_PORTS_ERR_CHK) {
            PxSERR_ERROR = TRUE;
            break;
        }

        //  Check for Error bits
        Data32_IS = HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_IS);
        if (Data32_IS & HBA_PORTS_IS_ERR_CHK) {
            PxSERR_ERROR = TRUE;
            break;
        }

        switch (CommandType) {

            case PIO_DATA_IN_CMD:
            case PIO_DATA_OUT_CMD:
            case PACKET_PIO_DATA_IN_CMD:
            case PACKET_PIO_DATA_OUT_CMD:
                // check if PIO setup received
                if(FISReceiveAddress->Ahci_Psfis[0] == FIS_PIO_SETUP) {
                    FISReceiveAddress->Ahci_Psfis[0] = 0;
                    TimeOutCount = TimeOut;
                    PIO_SETUP_FIS = TRUE;                         
                }
                break;
            default: 
                break;
        }
        // check if D2H register FIS is received
        if(FISReceiveAddress->Ahci_Rfis[0] == FIS_REGISTER_D2H) break;

        // For PIO Data in D2H register FIS is not received. So rely on BSY bit
        if ((CommandType == PIO_DATA_IN_CMD) &&  PIO_SETUP_FIS &&
                    !((HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_TFD) & 
                    (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_DRQ)))){
            break;
        }
        //If the Timeout is 0, Then there is no timeout for command processing  
        if(TimeOut==0) {
            continue;
        }
        i++;
    } while(i <= TimeOutCount * 2);

    if (PxSERR_ERROR) {
        // clear the status and return error
        HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); 
        HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR);         
        return EFI_DEVICE_ERROR;    
    }

    // Wait and check for CI clear
    Status = WaitForMemClear(AhciBusInterface, Port, 
	                                           HBA_PORTS_CI,
                                               0xFFFF,
                                               HBA_CI_CLEAR_TIMEOUT);
    
    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;                
    }

    // check for status bits
    if (HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_TFD) & (HBA_PORTS_TFD_ERR | HBA_PORTS_TFD_DRQ)){
        return EFI_DEVICE_ERROR;                
    }

    return EFI_SUCCESS;
}

/**
    Stop FIS and CR

    @param AhciBusInterface 
    @param SataDevInterface 

    @retval EFI_STATUS

    @note  
      1. clear ST bit and wait till CR bits gets reset
      2. if not generate Port reset
      3. Clear FIS running bit.
      4. Clear status register

**/ 
EFI_STATUS
StopController (
    IN AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface, 
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN BOOLEAN                  StartOrStop
) 
{
    UINT8         Port = SataDevInterface->PortNumber;
    UINT8         PMPort = SataDevInterface->PMPortNumber;
    EFI_STATUS    Status;
    UINT64        PortFISBaseAddr = SataDevInterface->PortFISBaseAddr;
    UINT64        CommandListBaseAddress = SataDevInterface->PortCommandListBaseAddr;
    UINT32        Data32;

    if(StartOrStop && (HBA_PORT_REG64(AhciBusInterface,Port,HBA_PORTS_CLB) != CommandListBaseAddress)) {
        gCommandListBaseAddress=HBA_PORT_REG64(AhciBusInterface,Port,HBA_PORTS_CLB);
        gFisBaseAddress=HBA_PORT_REG64(AhciBusInterface,Port,HBA_PORTS_FB);

        // Saving FIS and Command List Registers
        HBA_PORT_WRITE_REG64(AhciBusInterface,Port,HBA_PORTS_CLB,CommandListBaseAddress);
        HBA_PORT_WRITE_REG64(AhciBusInterface,Port,HBA_PORTS_FB,PortFISBaseAddr);
    }

    // Clear Start
    HBA_PORT_REG32_AND (AhciBusInterface, Port, HBA_PORTS_CMD, ~(HBA_PORTS_CMD_ST));
    // Make sure CR is 0 with in 500msec
    Status = WaitForMemClear(AhciBusInterface, Port, HBA_PORTS_CMD,
                            HBA_PORTS_CMD_CR,
                            HBA_CR_CLEAR_TIMEOUT);

    if (EFI_ERROR(Status)) { 

        // Get the Port Speed allowed and Interface Power Management Transitions Allowed
        // Pass the values for PortReset. 
        Data32 = HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_SCTL);
        Data32 &= 0xFF0;          

        Status = GeneratePortReset(AhciBusInterface,
                                   SataDevInterface,
                                   Port,
                                   PMPort,
                                   (UINT8)((Data32 & 0xF0) >> 4),
                                   (UINT8)(Data32 >> 8));
    };

    // Clear Status register
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); 
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR); 

    if(!StartOrStop  && gCommandListBaseAddress) {

        // Restoring FIS and Command List Registers
        HBA_PORT_WRITE_REG64(AhciBusInterface,Port,HBA_PORTS_CLB,gCommandListBaseAddress);
        HBA_PORT_WRITE_REG64(AhciBusInterface,Port,HBA_PORTS_FB,gFisBaseAddress);

        gCommandListBaseAddress = 0;
    }

    return Status;
}

/**
    Check if the device is ready to accept command.

    @param SataDevInterface 

    @retval EFI_STATUS      

    @note  
  1. Check the device is ready to accept the command. BSY and DRQ should be de-asserted.  
  2. If set, generate Port reset
  3. In case Port Multiplier is connected to the port, enable all the ports of the Port Multiplier.

**/ 
EFI_STATUS
ReadytoAcceptCmd (
    IN SATA_DEVICE_INTERFACE               *SataDevInterface
)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface = SataDevInterface->AhciBusInterface; 
    UINT8                   Port = SataDevInterface->PortNumber;
    UINT8                   PMPort = SataDevInterface->PMPortNumber;
    SATA_DEVICE_INTERFACE   *SataPMDevInterface, *SataPMPortDevInterface;
    UINT32                  Data32 = 0, Init_SStatus = 0;
    UINT8                   PowerManagement, Speed;

    // Is the Device ready to accept the command
    if (HBA_PORT_REG8 (AhciBusInterface, Port, HBA_PORTS_TFD) & (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_DRQ)){
        Data32 = HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_SCTL);
        Data32 &= 0xFF0;          
        // make sure the status we read is for the right port
        Status = GeneratePortReset(AhciBusInterface, SataDevInterface, Port, 0xFF,
                          (UINT8)((Data32 & 0xF0) >> 4), (UINT8)(Data32 >> 8));
        if (EFI_ERROR(Status)) return Status;

        // If it is a PMPort, Make sure all the Ports are in enabled state.
        SataPMDevInterface = GetSataDevInterface(AhciBusInterface, Port, 0xFF);
        if (!SataPMDevInterface) { 
            return EFI_DEVICE_ERROR;
        }
        if (SataPMDevInterface->DeviceType == PMPORT) {
            for (PMPort = 0; PMPort < SataPMDevInterface->NumPMPorts; PMPort++){
                SataPMPortDevInterface = GetSataDevInterface(AhciBusInterface, Port, PMPort);
                if (!SataPMPortDevInterface) continue;
                    ReadWritePMPort (SataDevInterface, PMPort, PSCR_0_SSTATUS, &Init_SStatus, FALSE);
                    if ((Init_SStatus & HBA_PORTS_SSTS_DET_MASK) == HBA_PORTS_SSTS_DET_PCE) { 
                        Data32 = SataPMPortDevInterface->SControl;
                        ReadWritePMPort (SataDevInterface, PMPort, PSCR_2_SCONTROL, &Data32, TRUE);
                    }
                    else {
                        Speed = (UINT8)((SataDevInterface->SControl >> 4) & 0xF);
                        PowerManagement = (UINT8)((SataDevInterface->SControl >> 8) & 0xF);
                        GeneratePortReset(AhciBusInterface, SataDevInterface, Port, PMPort, 
                                            Speed, PowerManagement);
                    }
                    Data32 = HBA_PORTS_ERR_CLEAR;
                    ReadWritePMPort (SataDevInterface, PMPort, PSCR_1_SERROR, &Data32, TRUE);
                }
        }
    } 
    return Status;
}

/**

    @param AhciBusInterface 

    @retval 

**/ 

EFI_STATUS
HostReset (
    IN AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface 
)
{

    return EFI_SUCCESS;
}

/**
    Issue a Port Reset

    @param AhciBusInterface 
    @param SataDevInterface 
    @param CurrentPort 
    @param Speed 
    @param PowerManagement 

    @retval EFI_STATUS

    @note  
      1. Issue port reset by setting DET bit in SControl register
      2. Call HandlePortComReset to check the status of the reset.

**/ 

EFI_STATUS
EFIAPI 
GeneratePortReset (
    AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface, 
    SATA_DEVICE_INTERFACE   *SataDevInterface, 
    UINT8                   Port,
    UINT8                   PMPort,
    UINT8                   Speed,
    UINT8                   PowerManagement
)
{
    EFI_STATUS  Status;
    volatile    AHCI_RECEIVED_FIS  *FISAddress =  (AHCI_RECEIVED_FIS *)( (UINT64)(HBA_PORT_REG64(AhciBusInterface, Port, HBA_PORTS_FB)) );
    UINT32      Data32;

    TRACE_AHCI_LEVEL2((-1,"AHCI: PortReset on Port : %x PMPort : %x", Port, PMPort));

    if (!FISAddress) return EFI_DEVICE_ERROR;   // FIS receive address is not programmed.

    if (gPortReset) return EFI_SUCCESS;
    gPortReset = TRUE;

    // Disable Start bit
    HBA_PORT_REG32_AND (AhciBusInterface, Port, HBA_PORTS_CMD, ~HBA_PORTS_CMD_ST);

    // Wait till CR is cleared    
    Status = WaitForMemClear(AhciBusInterface, Port, HBA_PORTS_CMD,
                            HBA_PORTS_CMD_CR,
                            HBA_CR_CLEAR_TIMEOUT);

    // Clear Status register
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); 
    if (PMPort != 0xFF) {
        Data32 = HBA_PORTS_ERR_CLEAR;
        ReadWritePMPort (SataDevInterface, PMPort, PSCR_1_SERROR, &Data32, TRUE);
    }
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR); 
    
    //  Enable FIS Receive Enable
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_FRE);     

    // Wait till FIS is running and then clear the data area
    WaitForMemSet(AhciBusInterface, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_FR,
                                    HBA_PORTS_CMD_FR,
                                    HBA_FR_CLEAR_TIMEOUT);

    FISAddress->Ahci_Rfis[0] = 0;

    if (PMPort == 0xFF) {
        // Issue Port COMRESET
       HBA_PORT_REG32_AND_OR (AhciBusInterface, Port, HBA_PORTS_SCTL, 0xFFFFF000, 
                    HBA_PORTS_SCTL_DET_INIT + (Speed << 4) + (PowerManagement << 8));     
        pBS->Stall (1000);                               // 1msec
        HBA_PORT_REG32_AND (AhciBusInterface, Port, HBA_PORTS_SCTL, ~HBA_PORTS_SCTL_DET_MASK);
    } else {
        Data32 = HBA_PORTS_SCTL_DET_INIT + (Speed << 4) + (PowerManagement << 8);
        ReadWritePMPort (SataDevInterface, PMPort, PSCR_2_SCONTROL, &Data32, TRUE);
        pBS->Stall (1000);                               // 1msec
        Data32 = (Speed << 4) + (PowerManagement << 8);
        ReadWritePMPort (SataDevInterface, PMPort, PSCR_2_SCONTROL, &Data32, TRUE);
    }

    Status = HandlePortComReset(AhciBusInterface, SataDevInterface, Port, PMPort);

    SataDevInterface->SControl = (Speed << 4) + (PowerManagement << 8);

    gPortReset = FALSE;     

    if (EFI_ERROR(Status)) {
        TRACE_AHCI_LEVEL2((-1," Status : %r\n", Status));
        return EFI_DEVICE_ERROR;
    }

    TRACE_AHCI_LEVEL2((-1," Status : %r\n", Status));
    return EFI_SUCCESS;    

}

/**
    Generate Soft Reset

    @param SataDevInterface 
    @param In UINT8                                PMPort

    @retval EFI_STATUS

    @note  
  1. Issue a Control register update, H2D register FIS with reset bit set.
  2. Wait for 100usec
  3. Issue a Control register update, H2D register FIS with reset bit reset.

**/ 

EFI_STATUS
EFIAPI 
GenerateSoftReset (
    IN SATA_DEVICE_INTERFACE                *SataDevInterface,
    IN UINT8                                PMPort
)
{

    EFI_STATUS           Status;
    AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface = SataDevInterface->AhciBusInterface; 
    AHCI_COMMAND_LIST    *CommandList = (AHCI_COMMAND_LIST *)(UINTN) SataDevInterface->PortCommandListBaseAddr;
    AHCI_COMMAND_TABLE   *Commandtable = (AHCI_COMMAND_TABLE *)(UINTN)AhciBusInterface->PortCommandTableBaseAddr;
    COMMAND_STRUCTURE     CommandStructure;
    UINT32                Data32;
    UINT8                 Port = SataDevInterface->PortNumber;

    TRACE_AHCI_LEVEL2((-1,"AHCI: SoftReset on Port : %x PMPort : %x", Port, PMPort));

    PROGRESS_CODE(DXE_IDE_RESET);

    if (gSoftReset) return EFI_SUCCESS;
    
    gSoftReset = TRUE;

    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

    Status = StopController(AhciBusInterface, SataDevInterface,TRUE);
    if (EFI_ERROR(Status)) {
        goto GenerateSoftReset_Exit;
    }

    // if Command list Override is supported, set CLO bit
    Data32 = HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_TFD) & (HBA_PORTS_TFD_DRQ | HBA_PORTS_TFD_BSY);
    if ((AhciBusInterface->HBACapability & HBA_CAP_SCLO) && Data32){
        HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_CLO);
        Status = WaitForMemClear(AhciBusInterface, Port, HBA_PORTS_CMD,
                            HBA_PORTS_CMD_CLO,
                            BUSY_CLEAR_TIMEOUT);
    }
    CommandStructure.Control = 4;
    BuildCommandList(SataDevInterface, CommandList, (UINT64)Commandtable);
    BuildCommandFIS(SataDevInterface, CommandStructure, CommandList, Commandtable);

    CommandList->Ahci_Cmd_W = 0; 
    // Update of Control Register
    Commandtable->CFis.Ahci_CFis_C = 0;
    CommandList->Ahci_Cmd_R = 1;
    CommandList->Ahci_Cmd_C= 1;

    if (PMPort != 0xFF) Commandtable->CFis.AHci_CFis_PmPort = PMPort;

    StartController(AhciBusInterface, SataDevInterface, BIT00);
    // Wait till command is processed
    Status = WaitForMemClear(AhciBusInterface, Port, HBA_PORTS_CI,
                            BIT00,
                            ONE_MILLISECOND * 5);

    // Is the command complete?
    if (EFI_ERROR(Status)){
        goto GenerateSoftReset_Exit;
    }
    pBS->Stall (100);               // 100 usec

    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
    BuildCommandList(SataDevInterface, CommandList, (UINT64)Commandtable);
    BuildCommandFIS(SataDevInterface, CommandStructure, CommandList, Commandtable);

    CommandList->Ahci_Cmd_W = 0; 
    // Update of Control Register
    Commandtable->CFis.Ahci_CFis_C = 0;
    if (PMPort != 0xFF) Commandtable->CFis.AHci_CFis_PmPort = PMPort;

    StartController(AhciBusInterface, SataDevInterface, BIT00);    
    Status = WaitforCommandComplete(SataDevInterface, NON_DATA_CMD, ATAPI_BUSY_CLEAR_TIMEOUT);

    //  Stop Controller
    StopController(AhciBusInterface, SataDevInterface,FALSE);

GenerateSoftReset_Exit:

    gSoftReset = FALSE;
    TRACE_AHCI_LEVEL2((-1," Status : %r\n", Status));
    return Status;
}

/**
    Check if COM Reset is successful or not

    @param AhciBusInterface 
    @param SataDevInterface 
    @param Port 
    @param PMPort 
  
    @retval EFI_STATUS      

    @note  
  1. Check if Link is active. If not return error.
  2. If Link is present, wait for PhyRdy Change bit to be set.
  3. Clear SError register
  4. Wait for D2H register FIS
  5. Check the Status register for errors.
  6. If COMRESET is success wait for sometime if the device is ATAPI or GEN1

**/ 
EFI_STATUS
HandlePortComReset (
    IN AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface, 
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN UINT8                    Port,
    IN UINT8                    PMPort
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    BOOLEAN     DeviceDetected = FALSE;
    UINT32      Data32, i, SStatusData;
    volatile    AHCI_RECEIVED_FIS  *FISAddress;
    UINT32      SError = 0;
    //  Check if detection is complete
    for (i = 0; i < HBA_PRESENCE_DETECT_TIMEOUT; i++){   // Total delay 10msec
        SStatusData = ReadSCRRegister (AhciBusInterface, SataDevInterface, Port, PMPort, 0); // SStatus
        SStatusData &= HBA_PORTS_SSTS_DET_MASK;            
        if ((SStatusData == HBA_PORTS_SSTS_DET_PCE) || (SStatusData == HBA_PORTS_SSTS_DET)) { 
            DeviceDetected = TRUE;
            break; 
        }
        pBS->Stall (1000);                               // 1msec
    }

    if (DeviceDetected) {
        // Wait till PhyRdy Change bit is set
        if (PMPort == 0xFF) {
            Status = WaitForMemSet(AhciBusInterface, Port, HBA_PORTS_SERR,
                            HBA_PORTS_SERR_EX,
                            HBA_PORTS_SERR_EX,
                            ATAPI_BUSY_CLEAR_TIMEOUT);
        } else {
            Status = WaitforPMMemSet (SataDevInterface, PMPort, PSCR_1_SERROR, 
                    HBA_PORTS_SERR_EX, HBA_PORTS_SERR_EX, ATAPI_BUSY_CLEAR_TIMEOUT);
        }

        FISAddress = (AHCI_RECEIVED_FIS*)((UINT64)(HBA_PORT_REG64(AhciBusInterface, Port, HBA_PORTS_FB)));

        for (i = 0; i < ATAPI_BUSY_CLEAR_TIMEOUT; ) {
            SError = ReadSCRRegister (AhciBusInterface, SataDevInterface, Port, PMPort, 2); // SError
            if (SError & HBA_PORTS_ERR_CHK) {
              WriteSCRRegister (AhciBusInterface, SataDevInterface, Port, PMPort, 1, HBA_PORTS_ERR_CLEAR ); //SError
            }
            if(FISAddress->Ahci_Rfis[0] == FIS_REGISTER_D2H) {break;}
            pBS->Stall (1000);              // 1msec Strange. Delay is needed for read to succeed. 
                if (PMPort != 0xFF) {
                    i+= 100;  // For device behind PM Port, there is a delay in writing to the register. So count can be decreased.
                } else {
                    i++;
                }
        } 

        // Wait till PxTFD gets updated from D2H FIS
        for (i = 0; i < 100; i++){   // Total delay 10msec        
            WriteSCRRegister (AhciBusInterface, SataDevInterface, Port, PMPort, 1, HBA_PORTS_ERR_CLEAR); //SError
            if((FISAddress->Ahci_Rfis[2] & HBA_PORTS_TFD_MASK) == (HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_TFD) & HBA_PORTS_TFD_MASK)) break;
            pBS->Stall (100);                               // 100usec
        }

        // check for errors
        if (FISAddress->Ahci_Rfis[2] & (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_ERR)) Status = EFI_DEVICE_ERROR;

        Data32 = HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_IS); 
        if (Data32 & (BIT30 + BIT29 + BIT28 + BIT27 + BIT26)) Status = EFI_DEVICE_ERROR;

        // Clear the status
        WriteSCRRegister (AhciBusInterface, SataDevInterface, Port, PMPort, 1, HBA_PORTS_ERR_CLEAR); //SError
        HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR); 

    } else {
        Status = EFI_DEVICE_ERROR;
    }

    return Status;   
}

/**

    @param AhciBusInterface 
    @param SataDevInterface 
    @param Port 
    @param PMPort 
    @param Register (0 : SStatus 1: SError 2: SControl)

    @retval UINT32
    @note  
  1. Check if the device is connected directly to the port
  2. if yes, read to the AHCI Controller else write to the Port Multiplier register. 

**/ 
UINT32
ReadSCRRegister (
    IN AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface, 
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN UINT8                     Port, 
    IN UINT8                     PMPort, 
    IN UINT8                     Register
)
{
    UINT32  Data32 = 0;
    UINT32  Reg = HBA_PORTS_SSTS;

    if (PMPort != 0xFF) {
        ReadWritePMPort (SataDevInterface, PMPort, Register, &Data32, FALSE);
    } else {
        if (Register == 1) Reg = HBA_PORTS_SCTL;
        if (Register == 2) Reg = HBA_PORTS_SERR;
        Data32 = HBA_PORT_REG32 (AhciBusInterface, Port, Reg);
    }

    return Data32;
}

/**
    Write to SCONTROL/Serror/SStatus register

    @param AhciBusInterface 
    @param SataDevInterface 
    @param Port 
    @param PMPort 
    @param Register (0 : SStatus 1: SError 2: SControl)
    @param Data32 
  
    @retval EFI_STATUS

    @note  
  1. Check if the device is connected directly to the port
  2. if yes, write to the AHCI Controller else write to the Port Multiplier register
  
**/ 

EFI_STATUS
WriteSCRRegister (
    IN AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface, 
    IN SATA_DEVICE_INTERFACE   *SataDevInterface,
    IN UINT8                    Port, 
    IN UINT8                    PMPort, 
    IN UINT8                    Register,
    IN UINT32                   Data32
)
{
    UINT32  Reg = HBA_PORTS_SSTS;

    if (PMPort != 0xFF) {
        ReadWritePMPort (SataDevInterface, PMPort, Register, &Data32, TRUE);
    } else {
        if (Register == 2) Reg = HBA_PORTS_SCTL;
        if (Register == 1) Reg = HBA_PORTS_SERR;
        HBA_PORT_REG32_OR (AhciBusInterface, Port, Reg, Data32); 
    }
    return EFI_SUCCESS;
}

/**
    Wait for memory to be set to the test value.

    @param  SATA_DEVICE_INTERFACE   *SataDevInterface,
    @param  PMPort
    @param  Register
    @param  MaskValue        - The mask value of memory
    @param  TestValue        - The test value of memory
    @param  WaitTimeInMs     - The time out value for wait memory set

    @retval EFI_SUCCESS HBA reset successfully.
    @retval EFI_DEVICE_ERROR HBA failed to complete hardware reset.

**/ 
EFI_STATUS 
WaitforPMMemSet (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface,
    IN UINT8                    PMPort,
    IN UINT8                    Register,
    IN UINT32                   AndMask,
    IN UINT32                   TestValue,
    IN UINT32                   WaitTimeInMs
)
{
    UINT32  Data32;

    while(WaitTimeInMs!=0){ 
        ReadWritePMPort (SataDevInterface, PMPort, Register, &Data32, FALSE);
        if((Data32 & AndMask) == TestValue) {return EFI_SUCCESS;}
        pBS->Stall (1000);  //  1Msec
        WaitTimeInMs--;
   }
   return EFI_DEVICE_ERROR;
}

/**
    Check for valid ATA/ATAPI/PMPORT signature 

                   
    @param AhciBusInterface 
    @param Port 
    @param PMPort 

    @retval EFI_STATUS

    @note  
      1. Check if Link is active
      2. Enable FIS and Command list run bits
      3. Check for valid signature    

**/ 

EFI_STATUS 
CheckValidDevice (
    IN AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface, 
    IN UINT8                    Port,
    IN UINT8                    PMPort
)
{
    UINT8   Data8;
    UINT32  Data32;

    // Check if Link is active
    Data8 = (UINT8)(HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_SSTS) & HBA_PORTS_SSTS_DET_MASK);
    if (Data8 != HBA_PORTS_SSTS_DET_PCE)  return EFI_DEVICE_ERROR;

    // Enable FIS receive and CI so that TFD gets updated properly
    // Clear out the command slot
    HBA_PORT_REG32_AND (AhciBusInterface, Port, HBA_PORTS_CI, 0);

    // Enable FIS Receive
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_FRE | HBA_PORTS_CMD_ST);

    // Wait till FIS is running
    WaitForMemSet(AhciBusInterface, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_FR,
                                    HBA_PORTS_CMD_FR,
                                    HBA_FR_CLEAR_TIMEOUT);
    
    // Wait till CR list is running
    WaitForMemSet(AhciBusInterface, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_CR,
                                    HBA_PORTS_CMD_CR,
                                    HBA_FR_CLEAR_TIMEOUT);

    // Clear Start Bit
    HBA_PORT_REG32_AND (AhciBusInterface, Port, HBA_PORTS_CMD, ~(HBA_PORTS_CMD_ST));
    WaitForMemClear(AhciBusInterface, Port, HBA_PORTS_CMD,
                            HBA_PORTS_CMD_CR,
                            HBA_CR_CLEAR_TIMEOUT);

    // Check if valid signature is present
    Data32 = HBA_PORT_REG32(AhciBusInterface, Port, HBA_PORTS_SIG);
    if (Data32 != ATA_SIGNATURE_32 && Data32 != ATAPI_SIGNATURE_32 && Data32 != PMPORT_SIGNATURE)
        return EFI_DEVICE_ERROR;

    Data8 = HBA_PORT_REG8 (AhciBusInterface, Port, HBA_PORTS_TFD);
    if (Data8 & (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_DRQ)) return EFI_DEVICE_ERROR;

    return EFI_SUCCESS;

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
