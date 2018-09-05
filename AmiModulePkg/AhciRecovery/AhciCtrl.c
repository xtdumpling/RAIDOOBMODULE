//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file AhciCtrl.c
    Provides the Access to AHCI Controller

**/

//---------------------------------------------------------------------------

#include    "AhciRecovery.h"
#include    "AhciCtrl.h"

//---------------------------------------------------------------------------

UINT32      gCommandListBaseAddress;
UINT32      gFisBaseAddress;
BOOLEAN     gPortReset = FALSE;             // Avoid Re-entry

extern      EFI_PEI_SERVICES        **gPeiServices;
extern      EFI_PEI_STALL_PPI       *gStallPpi;

VOID
ZeroMemory (
    IN  VOID  *Buffer,
    IN  UINTN Size
);
/**
    Issues Read Command to the ATA device

    @param  AhciRecDevInfo 
    @param  Buffer 
    @param  ByteCount 
    @param  LBA
    @param  BlockSize 

    @retval EFI_STATUS
    @note  
    1. Get the Max. number of sectors that can be Read/written in one Read/Write PIO command
    2. Update the Command Structure
    3. Issue ExecutePioDataCommand.
    4. If all the bytes are read exit else goto step 2

**/
EFI_STATUS
Ahci_AtaReadSectors (
    IN  AHCI_RECOVERY_DEVICE_INFO     *AhciRecDevInfo,
    IN  OUT VOID                      *Buffer,
    IN  UINTN                         ByteCount,
    IN  UINT64                        LBA,
    IN  UINT32                        BlockSize
)
{
    EFI_STATUS                      Status;
    UINT32                          SectorCount;
    INT64                           MaxSectorCount;
    INT64                           Total_Number_Of_Sectors;
    COMMAND_STRUCTURE               CommandStructure;
#if LBA48_SUPPORT
    UINT32                          Lba32Lo;
    UINT32                          Lba32Hi;
    BOOLEAN                         Use48Bit;
    Use48Bit = AhciRecDevInfo->Lba48Bit;
#endif
    MaxSectorCount = MAX_SECTOR_COUNT_PIO;
#if LBA48_SUPPORT
    if (Use48Bit) MaxSectorCount = 8192;
#endif
    if (BlockSize > ByteCount) BlockSize = (UINT32)ByteCount;
    //
    // Calculate the total number of Sectors to be transferred
    //
    Total_Number_Of_Sectors = ByteCount / BlockSize ;

#if LBA48_SUPPORT
    if (Use48Bit) {
        for ( ;Total_Number_Of_Sectors > 0; Total_Number_Of_Sectors -= MaxSectorCount) {

            if (Total_Number_Of_Sectors > MaxSectorCount) {
                SectorCount = (UINT32)MaxSectorCount;
            } else {
                SectorCount = (UINT32)Total_Number_Of_Sectors;
            }

            ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

            CommandStructure.Device = 0x40;

            CommandStructure.SectorCount = (UINT16) SectorCount;
            Lba32Lo = (UINT32) ( LBA & 0xffffffff );
            Lba32Hi = (UINT32) Shr64 ( LBA, 32 );
            CommandStructure.LBALow = (UINT8) ( Lba32Lo & 0xff );
            CommandStructure.LBAMid = (UINT8) ( Lba32Lo >> 8 );
            CommandStructure.LBAHigh = (UINT8) ( Lba32Lo >> 16 );
            CommandStructure.LBALowExp = (UINT8) ( Lba32Lo >> 24 );
            CommandStructure.LBAMidExp = (UINT8) ( Lba32Hi & 0xff );
            CommandStructure.LBAHighExp = (UINT8) ( Lba32Hi >> 8 );
            
            CommandStructure.Command = 0x24;

            CommandStructure.Buffer = Buffer;
            CommandStructure.ByteCount = (UINT32)(SectorCount * BlockSize);
            
            Status = ExecutePioDataCommand (AhciRecDevInfo, &CommandStructure, FALSE); // Returns # of bytes read
            if (EFI_ERROR(Status)) {
                return EFI_DEVICE_ERROR;
            }

            Buffer=(VOID*)((UINT8*)Buffer + (UINT32)(SectorCount * BlockSize));
            SectorCount = CommandStructure.ByteCount / BlockSize ;
            LBA += SectorCount;
        }
    } else {
#endif
        for ( ;Total_Number_Of_Sectors > 0; Total_Number_Of_Sectors -= MaxSectorCount) {

            if (Total_Number_Of_Sectors > MaxSectorCount) {
                SectorCount = 0;
            } else {
                SectorCount = (UINT32) Total_Number_Of_Sectors;
            }

            ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

            CommandStructure.Device = ((UINT8) ((UINT32) LBA >> 24) & 0x0f) | 0x40;

            CommandStructure.SectorCount = (UINT16) SectorCount;
            CommandStructure.LBALow = (UINT8)LBA;
            CommandStructure.LBAMid = (UINT8)(((UINT32)LBA >>8) & 0xff);
            CommandStructure.LBAHigh = (UINT8)(((UINT32)LBA >>16) & 0xff);
            CommandStructure.Command = 0x20;

            CommandStructure.Buffer = Buffer;
            CommandStructure.ByteCount = (UINT32)(SectorCount == 0 ? MaxSectorCount : Total_Number_Of_Sectors) * BlockSize ;

            Status = ExecutePioDataCommand (AhciRecDevInfo, &CommandStructure, FALSE); // Returns # of bytes read
            if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

            Buffer=(VOID*)((UINT8*)Buffer +((UINT32)(SectorCount == 0 ? MaxSectorCount : Total_Number_Of_Sectors) * BlockSize ));
            SectorCount = CommandStructure.ByteCount / BlockSize ;
            LBA += SectorCount;

        }
#if LBA48_SUPPORT
    }
#endif
    return EFI_SUCCESS;

}

/**
    Issues Read command to ATAPI device.

        
    @param  AhciRecDevInfo 
    @param  Buffer 
    @param  BufferSize 
    @param  LBA
    @param  BlockSize 

    @retval Buffer
    @retval EFI_STATUS

    @note  
    1. Check for validity of Inputs
    2. Check whether Media is present or not
    3. Issue ATAPi Packet command

**/
EFI_STATUS
Ahci_AtapiReadSectors (
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDevInfo,
    IN  OUT VOID                     *Buffer,
    IN  UINTN                        BufferSize,
    IN  UINT64                       LBA,
    IN  UINT32                       BlockSize
)
{

    EFI_STATUS              Status = EFI_DEVICE_ERROR;
    COMMAND_STRUCTURE       CommandStructure;
    INTN                    TotalNumberofBlocks;
    INTN                    TransferLength;
    UINTN                   BytesRemainingTobeRead;
    VOID                    *TempBuffer = Buffer;

    //
    //  Check if Media Present
    //
    if (AhciRecDevInfo->MediaInfo.MediaPresent == FALSE) {
        Status = DetectAtapiMedia(NULL, AhciRecDevInfo);
        if (Status == EFI_SUCCESS) return EFI_MEDIA_CHANGED;
        if (Status == EFI_NO_MEDIA) return Status;
        return EFI_DEVICE_ERROR;
    }

    TotalNumberofBlocks = BufferSize / AhciRecDevInfo->MediaInfo.BlockSize;

    for (; TotalNumberofBlocks >  0; TotalNumberofBlocks -= TransferLength) {

        ZeroMemory (&CommandStructure, sizeof(CommandStructure));
        //
        // Calculate # of blocks to be transferred
        //
        if (TotalNumberofBlocks > 0xffff)
            TransferLength = 0xffff;
        else
            TransferLength = TotalNumberofBlocks;

        BytesRemainingTobeRead = TransferLength * AhciRecDevInfo->MediaInfo.BlockSize;

        CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = ATAPI_READ_10;
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[1] = AhciRecDevInfo->Lun << 5;
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[2] = (UINT8)(((UINT32) LBA) >>  24);
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[3] = (UINT8)(((UINT32) LBA) >> 16);
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[4] = (UINT8)(((UINT16) LBA) >> 8);
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[5] = (UINT8)(((UINT8) LBA) & 0xff);
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[7] = (UINT8) (TransferLength >> 8);        // MSB
        CommandStructure.AtapiCmd.Ahci_Atapi_Command[8] = (UINT8) (TransferLength & 0xff);      // LSB
        CommandStructure.Buffer = TempBuffer;
        CommandStructure.ByteCount = (UINT32)BytesRemainingTobeRead;

        Status = ExecutePacketCommand(AhciRecDevInfo, &CommandStructure, 0);

        if (Status != EFI_SUCCESS) {
            //
            // Some error has occurred
            //

            //
            // Check if Device is getting ready. If yes, wait till it gets ready
            //
            if (AhciRecDevInfo->Atapi_Status == BECOMING_READY) {
                Status = AhciTestUnitReady(AhciRecDevInfo);
            }

            if (Status == EFI_MEDIA_CHANGED ) {
                Status = DetectAtapiMedia(NULL, AhciRecDevInfo);
                if (Status == EFI_SUCCESS) return EFI_MEDIA_CHANGED;    // Return Media Change
                if (Status == EFI_NO_MEDIA) {
                    AhciRecDevInfo->MediaInfo.MediaPresent = FALSE;
                    return Status;
                }
            }

            //return EFI_DEVICE_ERROR;
            return Status;
        }

        if (CommandStructure.ByteCount != BytesRemainingTobeRead) {  
            return EFI_DEVICE_ERROR;
        }

        //
        // Update pointer
        //
        TempBuffer = (UINT8 *)TempBuffer + BytesRemainingTobeRead;
        LBA += TransferLength;
    }


    return Status;

}

/**
    Detects and Configures Sata Device

    @param  PeiServices
    @param  AhciRecBlkIo
	@param  AhciControllerInfo
    @param  Port
    @param  PMPort

    @retval EFI_STATUS

**/

EFI_STATUS
DetectAndConfigureDevice (
    IN  EFI_PEI_SERVICES               **PeiServices,
    IN  AHCI_RECOVERY_BLK_IO_DEV       *AhciRecBlkIo,
    IN  AHCI_RECOVERY_CONTROLLER_INFO  *AhciControllerInfo,
    IN  UINT8                          Port,
    IN  UINT8                          PMPort
)
{

    EFI_STATUS  Status;

    Status = AhciDetectDevice(PeiServices, 
								AhciRecBlkIo,
								AhciControllerInfo,
								Port, 
								PMPort
								);
    
    return Status;
}


/**
    Detects a SATA device connected to given Port and PMPort

    @param  PeiServices
	@param  AhciRecBlkIo
    @param  AhciControllerInfo 
    @param  Port 
    @param  PMPort  

    @retval EFI_STATUS

    @note  
    1. if CheckDevicePresence fails exit.
    2. If Controller supports PM, issue Soft reset
    3. Check the Device Signature.

**/
EFI_STATUS
AhciDetectDevice (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  AHCI_RECOVERY_BLK_IO_DEV        *AhciRecBlkIo,
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort
)
{
    EFI_STATUS                  Status;
    UINTN                       AhciBaseAddr = (UINTN)(AhciControllerInfo->AhciBaseAddress);
    UINT32                      Signature;
    ATA_IDENTIFY_DATA           AtaAtapiIdentifyData;
    AHCI_RECOVERY_DEVICE_INFO   *AhciRecDeviceInfo;
    
    if( AhciRecBlkIo->DeviceCount >= MAX_DEVICE_COUNT) {
        return EFI_OUT_OF_RESOURCES;
    }

    if (PMPort == 0xFF) {
        Status = CheckDevicePresence (PeiServices, AhciControllerInfo, Port, PMPort);
#if ENABLE_AHCI_RECOVERY_TRACES
        PEI_TRACE((-1, gPeiServices, "\n CheckDevicePresence at port :%x and PMPort: %x is :%r",Port,PMPort,Status));
#endif
        if(EFI_ERROR(Status)) {
            return Status;
        }
    }
    else {
        return EFI_DEVICE_ERROR;
    }
        
    // Allocate memory for AHCI_RECOVERY_DEVICE_INFO 
    Status = (**gPeiServices).AllocatePool( (CONST EFI_PEI_SERVICES**)gPeiServices,
                                              sizeof(AHCI_RECOVERY_DEVICE_INFO),
                                              (VOID**)&AhciRecBlkIo->DeviceInfo[AhciRecBlkIo->DeviceCount] 
                                           );
	ASSERT_PEI_ERROR(gPeiServices, Status); 
	
    if(EFI_ERROR(Status)) {
        return Status;
    }
    
    AhciRecDeviceInfo = AhciRecBlkIo->DeviceInfo[AhciRecBlkIo->DeviceCount];
    (**PeiServices).SetMem( AhciRecDeviceInfo,
                             sizeof(AHCI_RECOVERY_DEVICE_INFO),
                             0 );
    AhciRecDeviceInfo->PortNo = Port;
    AhciRecDeviceInfo->PMPortNo=0xFF;

    //
    // Save the Signature
    //
    Signature = HBA_PORT_REG32(AhciBaseAddr, Port, HBA_PORTS_SIG);
    
    if(Signature == ATA_SIGNATURE_32) {
            AhciRecBlkIo->DeviceCount++;
            AhciRecDeviceInfo->RecoveryDeviceType = ATA;
            AhciRecDeviceInfo->DevicePresent = TRUE;
            AhciRecDeviceInfo->MediaInfo.DeviceType=MaxDeviceType;
            AhciRecDeviceInfo->MediaInfo.MediaPresent=TRUE;
            AhciRecDeviceInfo->MediaInfo.BlockSize=0x200;
            AhciRecDeviceInfo->AhciControllerInfo = AhciControllerInfo;
            
            //
            // Update the Identify device buffer
            //
            Status = GetIdentifyData(PeiServices,AhciRecDeviceInfo,Port,PMPort,&AtaAtapiIdentifyData);
            if (!EFI_ERROR(Status) && ((AtaAtapiIdentifyData.GeneralConfiguration_0 & 0x8000) == 0) ) {
                AhciRecDeviceInfo->MediaInfo.LastBlock  = (UINTN)(UINT32)AtaAtapiIdentifyData.TotalUserAddressableSectors_60;
#if LBA48_SUPPORT
                AhciRecDeviceInfo->.Lba48Bit = FALSE;
                if ( AtaAtapiIdentifyData.Command_Set_Supported_83 & 0x400 ) {
                    AhciRecDeviceInfo->MediaInfo.LastBlock = AtaAtapiIdentifyData.LBA_48;
                    AhciRecDeviceInfo->Lba48Bit = TRUE;
                }
#endif
            }
     }
     if(Signature == ATAPI_SIGNATURE_32) {
            AhciRecDeviceInfo->RecoveryDeviceType = ATAPI;
            AhciRecDeviceInfo->AhciControllerInfo = AhciControllerInfo;
            //
            // Update the Identify device buffer
            //
            Status = GetIdentifyData(PeiServices,AhciRecDeviceInfo,Port,PMPort,&AtaAtapiIdentifyData);
            //
            // If it is an ATAPI device, check whether it is a CDROM or not.
            // Currently only CDROM/Direct access Devices are supported.
            //
            if ((AtaAtapiIdentifyData.GeneralConfiguration_0 & 0x1f00) == (CDROM_DEVICE << 8) ||
                (AtaAtapiIdentifyData.GeneralConfiguration_0 & 0x1f00) == (DIRECT_ACCESS_DEVICE << 8) ||
                (AtaAtapiIdentifyData.GeneralConfiguration_0 & 0x1f00) == (OPTICAL_MEMORY_DEVICE << 8)) {
                AhciRecBlkIo->DeviceCount++;
                AhciRecDeviceInfo->DevicePresent = TRUE;
                AhciRecDeviceInfo->MediaInfo.DeviceType=IdeCDROM;
                AhciRecDeviceInfo->MediaInfo.MediaPresent=FALSE;
                AhciRecDeviceInfo->MediaInfo.BlockSize=0x800;
           }
      }
    return Status;

}

/**
    Issues Start/Stop unit Command

    @param AhciRecDevInfo 

    @retval EFI_STATUS  EFI_SUCCESS     : If Media is accessible
    @retval EFI_NO_MEDIA                : If No Media is available
    @retval EFI_MEDIA_CHANGED           : If Media is Changed
    @retval EFI_DEVICE_ERROR            : If Device not found

    @note  
    1. Update CommandStructure for ATAPI_TEST_UNIT_READY command
    2. Issue ExecutePacketCommand
    3. Check if the device is ready to accept command, whether Media is present or not.

**/
EFI_STATUS
AhciTestUnitReady (
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDevInfo
)
{

    EFI_STATUS              Status = EFI_SUCCESS;
    UINT16                  LoopCount;
    COMMAND_STRUCTURE       CommandStructure;
    

    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = ATAPI_TEST_UNIT_READY;
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[1] = AhciRecDevInfo->Lun << 5;
    CommandStructure.Buffer  = NULL;
    CommandStructure.ByteCount  =  0x100;

    for (LoopCount  = 0; LoopCount < 1000; LoopCount++) {           // 10sec loop ( 1000 * 10 msec = 10Sec)
        Status = ExecutePacketCommand(AhciRecDevInfo, &CommandStructure, 0);
        if (Status == EFI_SUCCESS) break;
        if (AhciRecDevInfo->Atapi_Status == MEDIUM_NOT_PRESENT) break;
        if (AhciRecDevInfo->Atapi_Status == MEDIA_CHANGED) break;
        gStallPpi->Stall( gPeiServices, gStallPpi, 100000 );

    }

    return Status;

}

/**
    Detects whether a Media is present in the ATAPI Removable 
    device or not.

    @param PeiServices 
    @param AhciRecDeviceInfo 

    @retval EFI_STATUS

    @note  
    1. Issue Read Capacity command for CDROM or Read Format command for other
     ATAPI devices.
    2. If step 1  is successful, update last LBA, Block Size, Read/Write 
     capable, Media ID.

**/
EFI_STATUS
DetectAtapiMedia (
    IN  EFI_PEI_SERVICES             **PeiServices,
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDeviceInfo
 )
{
    UINT8               *InputData, LoopCount;
    EFI_STATUS          Status;
    UINT16              ByteCount = 256, Data16;
    COMMAND_STRUCTURE   CommandStructure;

    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

    Status = AhciTestUnitReady(AhciRecDeviceInfo);

    if ((Status != EFI_MEDIA_CHANGED) && (Status != EFI_SUCCESS)) {
        return Status;
    }
    //
    //  Issue Read Capacity command
    //
    Status = (**PeiServices).AllocatePool( PeiServices, ByteCount, (VOID**)&InputData);

    if (EFI_ERROR(Status)) {
        return Status;
    }

    CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = ATAPI_READ_CAPACITY;
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[1] = AhciRecDeviceInfo->Lun << 5;
    Data16 = 8;

    for (LoopCount  = 0; LoopCount < 5; LoopCount++) {          // 5sec loop
        ByteCount = Data16;
        ZeroMemory (InputData, ByteCount);
        CommandStructure.Buffer  = InputData;
        CommandStructure.ByteCount = ByteCount;
        Status = ExecutePacketCommand(AhciRecDeviceInfo, &CommandStructure, 0);
        if (AhciRecDeviceInfo->Atapi_Status == EFI_SUCCESS) break;
        if (AhciRecDeviceInfo->Atapi_Status == MEDIUM_NOT_PRESENT) break;

    }

    if (Status == EFI_SUCCESS) {
        AhciRecDeviceInfo->MediaInfo.LastBlock = InputData[0] << 24 | InputData[1] << 16 | InputData[2] << 8 | InputData[3];
        AhciRecDeviceInfo->MediaInfo.LastBlock--;
        AhciRecDeviceInfo->MediaInfo.BlockSize = CDROM_BLOCK_SIZE;
        AhciRecDeviceInfo->MediaInfo.MediaPresent=TRUE;

    } else if (InputData[8] != 3) {
        AhciRecDeviceInfo->MediaInfo.LastBlock = InputData[4] << 24 | InputData[5] << 16 | InputData[6] << 8 | InputData[7];
        AhciRecDeviceInfo->MediaInfo.LastBlock--;
        AhciRecDeviceInfo->MediaInfo.BlockSize = InputData[9] << 16 | InputData[10] << 8 | InputData[11];
        AhciRecDeviceInfo->MediaInfo.MediaPresent=TRUE;
    }
    return Status;
}

/**
    Check for ATAPI Errors

    @param AhciRecDevInfo 

    @retval EFI_STATUS

    @note  
      1. Execute ATAPI Request Sense command.
      2. Check for Device getting ready, Media Change, No Media and other errors.
         Update AtapiDevice->Atapi_Status

**/
EFI_STATUS
HandleAtapiError (
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDevInfo
)
{

    EFI_STATUS              Status;
    UINT8                   Data8 = 0;
    UINT8                   SenseData[256];
    COMMAND_STRUCTURE       CommandStructure;
    UINTN                   AhciBaseAddr = (UINTN)AhciRecDevInfo->AhciControllerInfo->AhciBaseAddress;
    UINT8                   Port = AhciRecDevInfo->PortNo;
    

    AhciRecDevInfo->Atapi_Status = DEVICE_ERROR;

    ZeroMemory (SenseData, 256);
    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));

    CommandStructure.AtapiCmd.Ahci_Atapi_Command[0] = ATAPI_REQUEST_SENSE;
    CommandStructure.AtapiCmd.Ahci_Atapi_Command[4] = 0xff;

    CommandStructure.ByteCount =  256;
    CommandStructure.Buffer = SenseData;

    Status = ExecutePacketCommand(AhciRecDevInfo, &CommandStructure, 0);

    if (EFI_ERROR(Status)) {
        Data8 = HBA_PORT_REG8 (AhciBaseAddr, Port, HBA_PORTS_TFD);
    }
    //
    // Check for DF and CHK
    //
    if (Data8 & (DF | CHK)) {
        goto exit_HandleAtapiError_with_Reset;
    }

    if (!EFI_ERROR(Status)){

        AhciRecDevInfo->Atapi_Status = DEVICE_ERROR;
        Status = EFI_DEVICE_ERROR;              // Default Value

        if (((SenseData[2] & 0xf) == 2) && (SenseData[12] == 0x3a)) {
            Status = EFI_NO_MEDIA;
            AhciRecDevInfo->Atapi_Status = MEDIUM_NOT_PRESENT;
        }
        if (((SenseData[2] & 0xf) == 2) && (SenseData[12] == 0x04) && (SenseData[13] == 0x01)) {
            Status = EFI_MEDIA_CHANGED;
            AhciRecDevInfo->Atapi_Status = BECOMING_READY;
        }

        if (((SenseData[2] & 0xf) == 6) && (SenseData[12] == 0x28)){
            Status = EFI_MEDIA_CHANGED;
            AhciRecDevInfo->Atapi_Status = MEDIA_CHANGED;
        }

        if (((SenseData[2] & 0xf) == 7) && (SenseData[12] == 0x27)){
            Status = EFI_WRITE_PROTECTED;
            AhciRecDevInfo->Atapi_Status = WRITE_PROTECTED_MEDIA;
        }

        if (((SenseData[2] & 0xf) == 6) && (SenseData[12] == 0x29)){
            AhciRecDevInfo->Atapi_Status = POWER_ON_OR_DEVICE_RESET;
        }

        if (((SenseData[2] & 0xf) == 5) && (SenseData[0] == 0x70)){
            AhciRecDevInfo->Atapi_Status = ILLEGAL_REQUEST;
        }
    }

exit_HandleAtapiError_with_Reset:
    return Status;

}

/**
    Execute a Atapi Packet command

    @param  AhciRecDeviceInfo 
    @param  CommandStructure 
    @param  READWRITE

    @retval  EFI_STATUS

    @note  
        1. Stop the Controller
        2. Check if the device is ready to accept a Command.
        3. Build Command list
        4. Start the Controller.
        5. Wait till command completes. Check for errors.

**/
EFI_STATUS
ExecutePacketCommand (
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDeviceInfo,
    IN  COMMAND_STRUCTURE            *CommandStructure,
    IN  BOOLEAN                      READWRITE
)
{

    UINT8                            Data8;
    EFI_STATUS                       Status;
    AHCI_COMMAND_LIST                *CommandList=NULL;
    AHCI_RECOVERY_CONTROLLER_INFO    *AhciControllerInfo = AhciRecDeviceInfo->AhciControllerInfo;
    AHCI_COMMAND_TABLE               *Commandtable = (AHCI_COMMAND_TABLE *)(UINTN)AhciControllerInfo->PortCommandTableBaseAddr;
    UINTN                            AhciBaseAddr = (UINTN)AhciControllerInfo->AhciBaseAddress;
    UINT8                            Port = AhciRecDeviceInfo->PortNo;
   
    //
    // Update Base addresses
    //
    AhciRecDeviceInfo->PortCommandListBaseAddr = HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_CLB);
    AhciRecDeviceInfo->PortFISBaseAddr = HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_FB);

    CommandList = (AHCI_COMMAND_LIST *)(UINTN) AhciRecDeviceInfo->PortCommandListBaseAddr;

    CommandStructure->LBAMid = (UINT8)(CommandStructure->ByteCount);
    CommandStructure->LBAHigh = (UINT8)(CommandStructure->ByteCount >> 8);
    CommandStructure->Command = PACKET_COMMAND;

    Status = StopController(AhciRecDeviceInfo,TRUE);
    if (EFI_ERROR(Status)) return Status;

    Status = ReadytoAcceptCmd(AhciRecDeviceInfo);
    if (EFI_ERROR(Status)) {
        StopController(AhciRecDeviceInfo,FALSE);
        return Status;
    }
    BuildCommandList(AhciRecDeviceInfo, CommandList, AhciControllerInfo->PortCommandTableBaseAddr);
    BuildCommandFIS(AhciRecDeviceInfo, *CommandStructure, CommandList, Commandtable);
    BuildAtapiCMD(AhciRecDeviceInfo, *CommandStructure, CommandList, Commandtable);
    BuildPRDT(AhciRecDeviceInfo, *CommandStructure, CommandList, Commandtable);

    if (READWRITE) {
        CommandList->Ahci_Cmd_W = 1;
    }
    else {
        CommandList->Ahci_Cmd_W = 0;
    }
    Commandtable->CFis.Ahci_CFis_C = 1;

    Status = StartController(AhciRecDeviceInfo, BIT00);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = WaitforCommandComplete(AhciRecDeviceInfo, PIO_DATA_IN_CMD,
                    AhciRecDeviceInfo->RecoveryDeviceType == ATA? DMA_ATA_COMMAND_COMPLETE_TIMEOUT \
                    : DMA_ATAPI_COMMAND_COMPLETE_TIMEOUT );

    //
    // Handle ATAPI device error
    //
    if (EFI_ERROR(Status) && AhciRecDeviceInfo->RecoveryDeviceType == ATAPI) {
        Data8 = HBA_PORT_REG8 (AhciBaseAddr, Port, HBA_PORTS_TFD);
        if (Data8 & CHK ){
            return HandleAtapiError(AhciRecDeviceInfo);

        }
    }

    AhciRecDeviceInfo->Atapi_Status = EFI_SUCCESS;

    CommandStructure->ByteCount = CommandList->Ahci_Cmd_PRDBC;
    //
    //  Stop Controller
    //
    StopController(AhciRecDeviceInfo,FALSE);

    return Status;

}

/**
    Return Identify data from SATA device

    @param PeiServices
    @param AhciRecDeviceInfo
    @param Port
    @param PMPort
    @param AtaIdentifyData

    @retval  EFI_STATUS

    @note
    1. Build CommandStructure.
    2. Issue ExecutePioDataCommand

**/
EFI_STATUS
GetIdentifyData (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  AHCI_RECOVERY_DEVICE_INFO       *AhciRecDeviceInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort,
    IN  ATA_IDENTIFY_DATA               *AtaIdentifyData
)
{
    EFI_STATUS                          Status;
    COMMAND_STRUCTURE                   CommandStructure;

    ZeroMemory (&CommandStructure, sizeof(COMMAND_STRUCTURE));
    //
    //  Read Identify data
    //
    CommandStructure.Buffer = AtaIdentifyData;
    CommandStructure.ByteCount = sizeof(ATA_IDENTIFY_DATA);
    CommandStructure.Device = 0;
    CommandStructure.Command = AhciRecDeviceInfo->RecoveryDeviceType == ATA ? IDENTIFY_COMMAND : IDENTIFY_PACKET_COMMAND;

    Status = ExecutePioDataCommand (AhciRecDeviceInfo, &CommandStructure, FALSE);
    if (CommandStructure.ByteCount != sizeof(ATA_IDENTIFY_DATA)) { Status = EFI_DEVICE_ERROR; }

    return Status;

}

/**
    Check if any device is connected to the port

    @param PeiServices 
    @param AhciControllerInfo 
    @param Port 
    @param PMPort 

    @retval EFI_STATUS

    @note  
    1. If Staggered spin-up is supported, power-up the device.
    2. Call CheckValidDevice if success exit. Else generate Soft reset.

**/
EFI_STATUS
CheckDevicePresence (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort
)
{

    EFI_STATUS      Status;
    UINTN           AhciBaseAddr = (UINTN)(AhciControllerInfo->AhciBaseAddress);
    UINT32          Data32;
    UINTN           Delay;

    //
    // PM disabled
    //
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_SCTL, HBA_PORTS_SCTL_IPM_PSD_SSD);

    //
    // Clear Status register
    //
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR);
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR);

    //
    //  Check if Staggered Spin up is supported
    //
    if (HBA_REG32 (AhciBaseAddr,  HBA_CAP) & HBA_CAP_SSS) {

        //
        // Check if Link is already established, if yes dodn't expect a COMRESET
        //
        if ((HBA_PORT_REG32 (AhciControllerInfo->AhciBaseAddress, Port, HBA_PORTS_SSTS)
            & HBA_PORTS_SSTS_DET_MASK) != HBA_PORTS_SSTS_DET_PCE) {
            //
            // Enable FIS Receive Enable
            //
            HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_FRE);

            //
            // Wait till FIS is running
            //
            WaitForMemSet(AhciBaseAddr, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_FR,
                                    HBA_PORTS_CMD_FR,
                                    HBA_FR_CLEAR_TIMEOUT);


            HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_SUD); // Spin up the device

            //
            // Is delay necessary here after power up?
            //
            Status = HandlePortComReset(AhciControllerInfo, Port, 0xFF);

            //
            //  Disable FIS Receive Enable
            //
            HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_CMD, ~HBA_PORTS_CMD_FRE);
 			if (EFI_ERROR(Status)) {
                return EFI_DEVICE_ERROR;
            }

        } else {
            HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_SUD); // Spin up the device
        }
    }

    //
    // Check if Device detected. And check if Cold Presence logic
    // is enabled. If yes enable POD
    //
    if (((HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_SSTS) & HBA_PORTS_SSTS_DET_MASK) == HBA_PORTS_SSTS_DET_PCE) &&
            (HBA_PORT_REG32(AhciBaseAddr, Port, HBA_PORTS_CMD) & HBA_PORTS_CMD_CPD)) {
        HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_POD);
    }


    //
    // Is the Device ready to accept the command
    //
    if (HBA_PORT_REG8 (AhciBaseAddr, Port, HBA_PORTS_TFD) & (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_DRQ)) {

        Data32 = HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_SCTL);
        Data32 &= 0xFF0;

        //
        // make sure the status we read is for the right port
        //
        Status = GeneratePortReset(AhciControllerInfo, Port, 0xFF,
                          (UINT8)((Data32 & 0xF0) >> 4), (UINT8)(Data32 >> 8));

        if ( EFI_ERROR(Status) ) 
            return Status;
			
        // if Command list Override is supported, set CLO bit which clears the BSY and DRQ.
        if (HBA_PORT_REG8 (AhciBaseAddr, Port, HBA_PORTS_TFD) & (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_DRQ)) {
            if (HBA_REG32 (AhciBaseAddr, HBA_CAP) & HBA_CAP_SCLO) {
                HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_CLO);
                Status = WaitForMemClear(AhciBaseAddr, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_CLO,
                                    HBA_DRQ_BSY_CLEAR_TIMEOUT);
            }
        }

        for ( Delay = 0; Delay < HBA_DRQ_BSY_CLEAR_TIMEOUT; Delay++ ) {

            if (HBA_PORT_REG8 (AhciBaseAddr, Port, HBA_PORTS_TFD) & ( HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_DRQ))
                gStallPpi->Stall( PeiServices, gStallPpi, 1000 );
            else
                break;
        }      

        if ( Delay == HBA_DRQ_BSY_CLEAR_TIMEOUT ) {
            return EFI_DEVICE_ERROR;
        }
    }

    Status = CheckValidDevice(AhciControllerInfo, Port, PMPort);

    return Status;
}

/**
    Check for valid ATA/ATAPI/PMPORT signature

    @param AhciControllerInfo
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
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort
)
{

    UINT8       Data8;
    UINT32      Data32;
    UINTN       AhciBaseAddr = (UINTN)(AhciControllerInfo->AhciBaseAddress);
    EFI_STATUS  Status;

    //
    // Check if Link is active
    //
    Data8 = (UINT8)(HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_SSTS) & HBA_PORTS_SSTS_DET_MASK);
    if (Data8 != HBA_PORTS_SSTS_DET_PCE)  return EFI_DEVICE_ERROR;
    //
    // Enable FIS receive and CI so that TFD gets updated properly
    // Clear out the command slot
    //
    HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_CI, 0);
    //
    // Enable FIS Receive
    //
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_FRE | HBA_PORTS_CMD_ST);
    //
    // Wait till FIS is running
    //
    Status=WaitForMemSet(AhciBaseAddr, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_FR,
                                    HBA_PORTS_CMD_FR,
                                    HBA_FR_CLEAR_TIMEOUT);

    if(EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Wait till CR list is running
    //
    Status=WaitForMemSet(AhciBaseAddr, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_CR,
                                    HBA_PORTS_CMD_CR,
                                    HBA_FR_CLEAR_TIMEOUT);

    if(EFI_ERROR(Status)) {
        return Status;
    }
    //
    // Clear Start Bit
    //
    HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_CMD, ~(HBA_PORTS_CMD_ST));
    Status=WaitForMemClear(AhciBaseAddr, Port, HBA_PORTS_CMD,
                            HBA_PORTS_CMD_CR,
                            HBA_CR_CLEAR_TIMEOUT);

    if(EFI_ERROR(Status)) {
        return Status;
    }
    //
    //Clear FIS Receive enable bit
    //
    HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_CMD, ~(HBA_PORTS_CMD_FRE));
    Status=WaitForMemClear(AhciBaseAddr, Port, HBA_PORTS_CMD,
                            HBA_PORTS_CMD_FR,
                            HBA_FR_CLEAR_TIMEOUT);

    if(EFI_ERROR(Status)) {
        return Status;
    }
    //
    // Check if valid signature is present
    //
    Data32 = HBA_PORT_REG32(AhciBaseAddr, Port, HBA_PORTS_SIG);


    if (Data32 != ATA_SIGNATURE_32 && Data32 != ATAPI_SIGNATURE_32 && Data32 != PMPORT_SIGNATURE)
        return EFI_DEVICE_ERROR;

    Data8 = HBA_PORT_REG8 (AhciBaseAddr, Port, HBA_PORTS_TFD);
    if (Data8 & (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_DRQ)) return EFI_DEVICE_ERROR;

    return EFI_SUCCESS;

}

/**
    Initializes AHCI Controller

    @param PeiService 
    @param AhciControllerInfo

    @retval EFI_STATUS

    @note  
    1. Update internal Data area about the AHCI controller Capabilities.
    2. Allocate memory for FIS and CommandList
    3. Enable AHCI mode
    3. Disable all the ports
**/

EFI_STATUS
AhciInitController (
    IN  EFI_PEI_SERVICES                  **PeiServices,
    IN  OUT AHCI_RECOVERY_CONTROLLER_INFO *AhciControllerInfo
)
{
    EFI_STATUS  Status;
    UINT32      PortsImplemented;
    UINT8       i, PortNumber;
    UINTN       AhciBaseAddr;

    if (!AhciControllerInfo->AhciBaseAddress) {
        return EFI_DEVICE_ERROR;
    }

    AhciBaseAddr = (UINTN) (AhciControllerInfo->AhciBaseAddress);

    //
    // Get AHCI Capability
    //
    AhciControllerInfo->HBACapability = HBA_REG32(AhciBaseAddr, HBA_CAP);
    if (AhciControllerInfo->HBACapability == 0xFFFFFFFF)   { 
        return EFI_DEVICE_ERROR; // Not decoded properly
    }

    //
    // Get # of Ports Implemented (bit map)
    //
    AhciControllerInfo->HBAPortImplemented = HBA_REG32(AhciBaseAddr, HBA_PI);
    if (!AhciControllerInfo->HBAPortImplemented) {
        return EFI_DEVICE_ERROR;
    }

    //
    // Cross check whether # of Ports implemented is less or equal to
    // Max. # of ports supported by the silicon
    //
    PortsImplemented = AhciControllerInfo->HBAPortImplemented;
    AhciControllerInfo->NumberofPortsImplemented = 0;
    for ( ;PortsImplemented; PortsImplemented >>= 1) {
        if  (PortsImplemented & 1) AhciControllerInfo->NumberofPortsImplemented++;
    }
    if (((AhciControllerInfo->HBACapability & HBA_CAP_NP_MASK) + 1) < AhciControllerInfo->NumberofPortsImplemented) { 
        return EFI_DEVICE_ERROR; 
    }

    //
    //  Set AE bit
    //
    HBA_REG32_OR(AhciBaseAddr, HBA_GHC, HBA_GHC_AE);

    //
    // Allocate memory for FIS. Should be aligned on 256 Bytes. Each
    // Port will have it own FIS data area.
    //-

    Status = (**PeiServices).AllocatePool( PeiServices,
                                           AhciControllerInfo->NumberofPortsImplemented * RECEIVED_FIS_SIZE + 0x100,
                                           (VOID**)&(AhciControllerInfo->PortFISBaseAddr));
    ASSERT_PEI_ERROR(PeiServices, Status);
    if( EFI_ERROR(Status) ) {
        return EFI_OUT_OF_RESOURCES;
    }

    ZeroMemory ((VOID *) AhciControllerInfo->PortFISBaseAddr,
            AhciControllerInfo->NumberofPortsImplemented * RECEIVED_FIS_SIZE + 0x100);

    AhciControllerInfo->PortFISBaseAddrEnd = AhciControllerInfo->PortFISBaseAddr +
            AhciControllerInfo->NumberofPortsImplemented * RECEIVED_FIS_SIZE;
    AhciControllerInfo->Address1 = AhciControllerInfo->PortFISBaseAddr; // Will be used to free the memory later
    AhciControllerInfo->PortFISBaseAddr = (AhciControllerInfo->PortFISBaseAddr & (~0xFF))+ 0x100;

    //
    // Allocate memory for Command List (1KB aligned) and Command Table (128KB aligned).
    // All the ports in the controller will share Command List and Command table data Area.
    //

    Status = (**PeiServices).AllocatePool( PeiServices,
                                           COMMAND_LIST_SIZE_PORT * 2,
                                           (VOID**)&(AhciControllerInfo->PortCommandListBaseAddr));

    ASSERT_PEI_ERROR(PeiServices, Status);
    if( EFI_ERROR(Status) ) {
        return EFI_OUT_OF_RESOURCES;
    }

    ZeroMemory ((VOID *)AhciControllerInfo->PortCommandListBaseAddr, COMMAND_LIST_SIZE_PORT * 2);
    AhciControllerInfo->Address2 = AhciControllerInfo->PortCommandListBaseAddr;

    AhciControllerInfo->PortCommandListBaseAddr = (AhciControllerInfo->PortCommandListBaseAddr & (~0x3FF)) + 0x400;
    AhciControllerInfo->PortCommandListLength = 0x20;
    AhciControllerInfo->PortCommandTableBaseAddr = AhciControllerInfo->PortCommandListBaseAddr + 0x80;
    AhciControllerInfo->PortCommandTableLength = COMMAND_LIST_SIZE_PORT - 0x80;

    //
    // Make sure controller is not running
    //
    PortsImplemented = AhciControllerInfo->HBAPortImplemented;
    PortNumber = 0;
    for (i=0; PortsImplemented; PortsImplemented>>=1, PortNumber++){
        if (PortsImplemented & 1) {

            //
            // Clear Start
            //
            HBA_PORT_REG32_AND(AhciBaseAddr, PortNumber, HBA_PORTS_CMD, ~(HBA_PORTS_CMD_ST));

            //
            // Make sure CR is 0 with in 500msec
            //
            Status = WaitForMemClear(AhciBaseAddr, PortNumber, HBA_PORTS_CMD,
                            HBA_PORTS_CMD_CR,
                            HBA_CR_CLEAR_TIMEOUT
            );

            if (EFI_ERROR(Status)) {
                Status = GeneratePortReset(AhciControllerInfo, PortNumber, 0xFF,
                                        HBA_PORTS_SCTL_SPD_NSNR, HBA_PORTS_SCTL_IPM_PSSD);
            }
            // if (EFI_ERROR(Status)) { HostReset(AhciRecInterface);}

            //
            // Clear FIS receive enable.
            //
            HBA_PORT_REG32_AND (AhciBaseAddr, PortNumber,
                                HBA_PORTS_CMD, ~(HBA_PORTS_CMD_FRE));
            //
            // Make sure FR is 0 with in 500msec
            //
            Status = WaitForMemClear(AhciBaseAddr, PortNumber, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_FR,
                                    HBA_FR_CLEAR_TIMEOUT);
            if (EFI_ERROR(Status)) continue;

            HBA_PORT_REG32_OR (AhciBaseAddr, PortNumber,
                                  HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); // Clear Status register

            //
            // Program PxCLB and PxFB
            //
            HBA_PORT_WRITE_REG32 (AhciBaseAddr, PortNumber, HBA_PORTS_CLB,AhciControllerInfo->PortCommandListBaseAddr);

            HBA_PORT_WRITE_REG32 (AhciBaseAddr, PortNumber, 0x0008, AhciControllerInfo->PortFISBaseAddr +(i * RECEIVED_FIS_SIZE));

            i++;
        }
    }
    
    return EFI_SUCCESS;
}

/**
    Check if the port is implemented in the AHCI Controller

    @param AhciControllerInfo 
    @param Port 

    @retval EFI_STATUS

    @note
    1. Check Port Implemented register whether the PORT is
     implemented in the Controller or not.

**/
EFI_STATUS
CheckPortImplemented (
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port
)
{

    if (AhciControllerInfo->HBAPortImplemented & (1<< Port)) return EFI_SUCCESS;

    return EFI_NOT_FOUND;

}

/**
    Execute PIO Data In/Out command

    @param  AhciRecDeviceInfo 
    @param  CommandStructure 
    @param  READWRITE

    @retval EFI_STATUS, CommandStructure->ByteCount

    @note
    1. Stop the Controller
    2. Check if the device is ready to accept a Command.
    3. Build Command list
    4. Start the Controller.
    5. Wait till command completes. Check for errors.

**/
EFI_STATUS
ExecutePioDataCommand (
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDeviceInfo,
    IN  OUT COMMAND_STRUCTURE        *CommandStructure,
    IN  BOOLEAN                      READWRITE
)
{
    EFI_STATUS                      Status;
    AHCI_COMMAND_LIST               *CommandList=NULL;
    AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo = AhciRecDeviceInfo->AhciControllerInfo;
    AHCI_COMMAND_TABLE              *Commandtable = (AHCI_COMMAND_TABLE *)(UINTN)AhciControllerInfo->PortCommandTableBaseAddr;
    UINTN                           AhciBaseAddr = (UINTN)AhciControllerInfo->AhciBaseAddress;
    UINT8                           Port = AhciRecDeviceInfo->PortNo;

    //
    // Update Base addresses
    //
    AhciRecDeviceInfo->PortCommandListBaseAddr = HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_CLB);
    AhciRecDeviceInfo->PortFISBaseAddr = HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_FB);

    CommandList = (AHCI_COMMAND_LIST *)(UINTN) AhciControllerInfo->PortCommandListBaseAddr;

    Status = StopController(AhciRecDeviceInfo,TRUE);
    if (EFI_ERROR(Status)) return Status;

    Status = ReadytoAcceptCmd(AhciRecDeviceInfo);
    if (EFI_ERROR(Status)) {
        StopController(AhciRecDeviceInfo,FALSE);
        return Status;
    }
    BuildCommandList(AhciRecDeviceInfo, CommandList, AhciControllerInfo->PortCommandTableBaseAddr);
    BuildCommandFIS(AhciRecDeviceInfo, *CommandStructure, CommandList, Commandtable);
    BuildAtapiCMD(AhciRecDeviceInfo, *CommandStructure, CommandList, Commandtable);
    BuildPRDT(AhciRecDeviceInfo, *CommandStructure, CommandList, Commandtable);

    if (READWRITE) {
        CommandList->Ahci_Cmd_W = 1;
    }
    else {
        CommandList->Ahci_Cmd_W = 0;
    }
    Commandtable->CFis.Ahci_CFis_C = 1;

    Status = StartController(AhciRecDeviceInfo, BIT00);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = WaitforCommandComplete(AhciRecDeviceInfo, PIO_DATA_IN_CMD,
                    AhciRecDeviceInfo->RecoveryDeviceType == ATA? DMA_ATA_COMMAND_COMPLETE_TIMEOUT \
                    : DMA_ATAPI_COMMAND_COMPLETE_TIMEOUT );
    CommandStructure->ByteCount = CommandList->Ahci_Cmd_PRDBC;
    //
    //  Stop Controller
    //
    StopController(AhciRecDeviceInfo, FALSE);

    return Status;

}

/**
    Wait till cmd completes

    @param AhciRecDeviceInfo 
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
WaitforCommandComplete (
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDeviceInfo,
    IN  COMMAND_TYPE                 CommandType,
    IN  UINTN                        TimeOut
)
{

    UINTN       AhciBaseAddr = (UINTN)AhciRecDeviceInfo->AhciControllerInfo->AhciBaseAddress;
    UINT8       Port = AhciRecDeviceInfo->PortNo;
    UINT32      Data32_SERR, Data32_IS, i;
    BOOLEAN     PxSERR_ERROR = FALSE, PIO_SETUP_FIS = FALSE;
    volatile AHCI_RECEIVED_FIS   *FISReceiveAddress = (AHCI_RECEIVED_FIS   *)AhciRecDeviceInfo->PortFISBaseAddr;
    UINTN       TimeOutCount = TimeOut;

    for(i = 0; i < TimeOutCount * 2; i++, gStallPpi->Stall( gPeiServices, gStallPpi, 500 )) { // 500usec

        //
        //  Check for Error bits
        //
        Data32_SERR = HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_SERR);
        if (Data32_SERR & HBA_PORTS_ERR_CHK) {
            PxSERR_ERROR = TRUE;
            break;
        }

        //
        //  Check for Error bits
        //
        Data32_IS = HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_IS);
        if (Data32_IS & HBA_PORTS_IS_ERR_CHK) {
            PxSERR_ERROR = TRUE;
            break;
        }

        switch (CommandType) {

            case PIO_DATA_IN_CMD:
            case PIO_DATA_OUT_CMD:
            case PACKET_PIO_DATA_IN_CMD:
            case PACKET_PIO_DATA_OUT_CMD:

                //
                // check if PIO setup received
                //
                if(FISReceiveAddress->Ahci_Psfis[0] == FIS_PIO_SETUP) {
                    FISReceiveAddress->Ahci_Psfis[0] = 0;
                    TimeOutCount = TimeOut;
                    PIO_SETUP_FIS = TRUE;
                }
                break;
            default:
                break;

        }

        //
        // check if D2H register FIS is received
        //
        if(FISReceiveAddress->Ahci_Rfis[0] == FIS_REGISTER_D2H) break;

        //
        // For PIO Data in D2H register FIS is not received. So rely on BSY bit
        //
        if ((CommandType == PIO_DATA_IN_CMD) &&  PIO_SETUP_FIS &&
                    !((HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_TFD) &
                    (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_DRQ)))){
            break;
        }
    }

    if (PxSERR_ERROR) {

        //
        // clear the status and return error
        //
        HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR);
        HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR);
        return EFI_DEVICE_ERROR;
    }

    //
    // check if CI register is zero
    //
    if (HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_CI)){
        return EFI_DEVICE_ERROR;
    }

    //
    // check for status bits
    //
    if (HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_TFD) & (HBA_PORTS_TFD_ERR | HBA_PORTS_TFD_DRQ)){
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;

}

/**
    Build Command FIS

    @param AhciRecDeviceInfo
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
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDeviceInfo,
    IN  COMMAND_STRUCTURE            CommandStructure,
    IN  AHCI_COMMAND_LIST            *CommandList,
    IN  AHCI_COMMAND_TABLE           *Commandtable
)
{


    ZeroMemory (Commandtable, sizeof(AHCI_COMMAND_TABLE));

    Commandtable->CFis.Ahci_CFis_Type = FIS_REGISTER_H2D;
    Commandtable->CFis.AHci_CFis_PmPort = AhciRecDeviceInfo->PMPortNo == 0xFF ? 0x0 : AhciRecDeviceInfo->PMPortNo;
//  Commandtable->CFis.Ahci_CFis_C = 1;          // Set elsewhere
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
    Build ATAPI CMD.

    @param  AhciRecDevInfo 
    @param  CommandStructure 
    @param  CommandList 
    @param  Commandtable 

    @retval EFI_STATUS

    @note
    1. Copy Packet data to command table
    2. Set Atapi bit in Command List

**/
EFI_STATUS
BuildAtapiCMD (
    IN AHCI_RECOVERY_DEVICE_INFO           *AhciRecDevInfo,
    IN COMMAND_STRUCTURE                   CommandStructure,
    IN AHCI_COMMAND_LIST                   *CommandList,
    IN AHCI_COMMAND_TABLE                  *Commandtable
)
{


    CopyMemPei(&(Commandtable->AtapiCmd),&(CommandStructure.AtapiCmd),sizeof(AHCI_ATAPI_COMMAND));

    if (Commandtable->CFis.Ahci_CFis_Cmd == PACKET_COMMAND){ // Is it a packet command?
        CommandList->Ahci_Cmd_A = 1;
    }

    return EFI_SUCCESS;

}

/**
    Build PRDT table

    @param AhciRecDevInfo 
    @param CommandStructure 
    @param CommandList 
    @param Commandtable 

    @retval  EFI_STATUS

    @note
    1. Build as many PRDT table entries based on ByteCount.
    2. Set the I flag for the lasr PRDT table.
    3. Update PRDT table length in CommandList


**/
EFI_STATUS
BuildPRDT (
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDevInfo,
    IN  COMMAND_STRUCTURE            CommandStructure,
    IN  AHCI_COMMAND_LIST            *CommandList,
    IN  AHCI_COMMAND_TABLE           *Commandtable
)
{

    UINT32                          ByteCount = CommandStructure.ByteCount;
    UINT16                          Prdtlength = 0;
    AHCI_COMMAND_PRDT               *PrdtTable = &(Commandtable->PrdtTable);
    AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo = AhciRecDevInfo->AhciControllerInfo;
    for (;ByteCount; PrdtTable++){ 
        PrdtTable->Ahci_Prdt_DBA = (UINT32)(UINTN)CommandStructure.Buffer;
        PrdtTable->Ahci_Prdt_DBAU = (UINT32)Shr64((UINTN)CommandStructure.Buffer, 32);
        PrdtTable->Ahci_Prdt_DBC = ByteCount >= PRD_MAX_DATA_COUNT ? (PRD_MAX_DATA_COUNT - 1) : (ByteCount - 1);
        ByteCount -= (PrdtTable->Ahci_Prdt_DBC + 1);
        PrdtTable->Ahci_Prdt_I = 0;
        Prdtlength+= sizeof(AHCI_COMMAND_PRDT);
        if ((UINT32)(Prdtlength + 0x80) >= AhciControllerInfo->PortCommandTableLength) {
            break;
        }
        CommandStructure.Buffer = (UINT8 *) CommandStructure.Buffer + PrdtTable->Ahci_Prdt_DBC + 1;
    }
    //
    //  Set I flag only for the last entry.
    //
//    (UINT8 *)PrdtTable -= sizeof(AHCI_COMMAND_PRDT);
    PrdtTable -- ;
    PrdtTable->Ahci_Prdt_I = 1;
    CommandList->Ahci_Cmd_PRDTL = Prdtlength / sizeof(AHCI_COMMAND_PRDT);

    return EFI_SUCCESS;

}

/**
    Starts SATA controller.

    @param  AhciRecDeviceInfo 
    @param  CIBitMask 

    @retval EFI_STATUS

    @note
    1. Clear Status register
    2. Enable FIS and CR running bit
    3. Enable Start bit
    4. Update CI bit mask

**/
EFI_STATUS
StartController (
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDeviceInfo,
    IN  UINT32                       CIBitMask
)
{

    UINTN   AhciBaseAddr = (UINTN)(AhciRecDeviceInfo->AhciControllerInfo->AhciBaseAddress);
    UINT8   Port = AhciRecDeviceInfo->PortNo;
    EFI_STATUS  Status;
    //
    // Clear Status
    //
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR);
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR);
    //
    // Enable FIS Receive
    //
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_FRE);
    //
    // Wait till FIS is running
    //
    Status = WaitForMemSet(AhciBaseAddr, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_FR,
                                    HBA_PORTS_CMD_FR,
                                    HBA_FR_CLEAR_TIMEOUT);
    if(EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Clear FIS Receive area
    //
    ZeroMemory ((VOID *)(UINTN)AhciRecDeviceInfo->PortFISBaseAddr, RECEIVED_FIS_SIZE);
    //
    // Enable ST
    //
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_ST);
    //
    // Enable Command Issued
    //
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CI, CIBitMask);

    return EFI_SUCCESS;

}

/**
    Builds command list

    @param AhciRecDevInfo
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
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDevInfo,
    IN  AHCI_COMMAND_LIST            *CommandList,
    IN  UINT32                       CommandTableBaseAddr
)
{


    ZeroMemory (CommandList, sizeof(AHCI_COMMAND_LIST));
    // CommandList->Ahci_Cmd_A = SataDevInterface->DeviceType == ATAPI ? 1 : 0;      // set elsewhere
    CommandList->Ahci_Cmd_P = 0;
    CommandList->Ahci_Cmd_R = 0;
    CommandList->Ahci_Cmd_B = 0;
    CommandList->Ahci_Cmd_Rsvd1 = 0;
    CommandList->Ahci_Cmd_PMP = AhciRecDevInfo->PMPortNo == 0xFF ? 0x0 : AhciRecDevInfo->PMPortNo;
    CommandList->Ahci_Cmd_PRDTL = 0;
    CommandList->Ahci_Cmd_PRDBC = 0;
    CommandList->Ahci_Cmd_CTBA = CommandTableBaseAddr;
    CommandList->Ahci_Cmd_CTBAU = 0;

    return EFI_SUCCESS;

}

/**
    Check if the device is ready to accept cmd.

    @param  AhciRecDeviceInfo 

    @retval EFI_STATUS

    @note  
    1. Check the device is ready to accept the command. BSY and DRQ
     should be de-asserted.
    2. If set, generate Port reset
    3. In case Port Multiplier is connected to the port, enable all
     the ports of the Port Multiplier.

**/
EFI_STATUS
ReadytoAcceptCmd (
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDeviceInfo
)
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo = AhciRecDeviceInfo->AhciControllerInfo;
    UINTN                           AhciBaseAddr = (UINTN)(AhciControllerInfo->AhciBaseAddress);
    UINT8                           Port = AhciRecDeviceInfo->PortNo;
    UINT32                          Data32 = 0;

    //
    // Is the Device ready to accept the command
    //
    if (HBA_PORT_REG8 (AhciBaseAddr, Port, HBA_PORTS_TFD) & (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_DRQ)){
        Data32 = HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_SCTL);
        Data32 &= 0xFF0;

        //
        // make sure the status we read is for the right port
        //
        Status = GeneratePortReset(AhciControllerInfo, Port, 0xFF,
                          (UINT8)((Data32 & 0xF0) >> 4), (UINT8)(Data32 >> 8));
        if (EFI_ERROR(Status)) return Status;

    }
    return Status;
}

/**
    Stop FIS and CR

    @param  AhciRecDeviceInfo 
    @param  StartOrStop 

    @retval EFI_STATUS

    @note
    1. clear ST bit and wait till CR bits gets reset
    2. if not generate Port reset
    3. Clear FIS running bit.
    4. Clear status register

**/
EFI_STATUS
StopController (
    IN  AHCI_RECOVERY_DEVICE_INFO    *AhciRecDeviceInfo,
    IN  BOOLEAN                      StartOrStop
)
{

    UINT8   Port = AhciRecDeviceInfo->PortNo;
    AHCI_RECOVERY_CONTROLLER_INFO  *AhciControllerInfo = AhciRecDeviceInfo->AhciControllerInfo;
    UINTN  AhciBaseAddr = (UINTN)(AhciControllerInfo->AhciBaseAddress);
    EFI_STATUS  Status;
    UINT32      PortFISBaseAddr = AhciRecDeviceInfo->PortFISBaseAddr;
    UINT32      CommandListBaseAddress = AhciRecDeviceInfo->PortCommandListBaseAddr;

    if(StartOrStop) {
        gCommandListBaseAddress=HBA_PORT_REG32(AhciBaseAddr,Port,HBA_PORTS_CLB);
        gFisBaseAddress=HBA_PORT_REG32(AhciBaseAddr,Port,HBA_PORTS_FB);
        HBA_PORT_WRITE_REG32(AhciBaseAddr,Port,HBA_PORTS_CLB,CommandListBaseAddress);
        HBA_PORT_WRITE_REG32(AhciBaseAddr,Port,HBA_PORTS_FB,PortFISBaseAddr);
    }
    //
    // Clear Start
    //
    HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_CMD, ~(HBA_PORTS_CMD_ST));
    //
    // Make sure CR is 0 with in 500msec
    //
    Status = WaitForMemClear(AhciBaseAddr, Port, HBA_PORTS_CMD,
                            HBA_PORTS_CMD_CR,
                            HBA_CR_CLEAR_TIMEOUT);
    if (EFI_ERROR(Status)) {
        Status = GeneratePortReset(AhciControllerInfo, Port, 0xFF,
                                        HBA_PORTS_SCTL_SPD_NSNR, HBA_PORTS_SCTL_IPM_DIS);
    };

    if (EFI_ERROR(Status)) {
        goto StopController_ErrorExit;
    }

    if (EFI_ERROR(Status)) return Status;
    //
    //  Clear FIS receive enable.
    //
    HBA_PORT_REG32_AND (AhciBaseAddr, Port,
                                    HBA_PORTS_CMD, ~(HBA_PORTS_CMD_FRE));
    //
    //  Make sure FR is 0 with in 500msec
    //
    Status = WaitForMemClear(AhciBaseAddr, Port, HBA_PORTS_CMD,
                            HBA_PORTS_CMD_FR,
                            HBA_FR_CLEAR_TIMEOUT);
    if (EFI_ERROR(Status)) {
        goto StopController_ErrorExit;
    }
    //
    // Clear Status register
    //
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR);
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR);

    if(!StartOrStop) {
        HBA_PORT_WRITE_REG32(AhciBaseAddr,Port,HBA_PORTS_CLB,gCommandListBaseAddress);
        HBA_PORT_WRITE_REG32(AhciBaseAddr,Port,HBA_PORTS_FB,gFisBaseAddress);
    }

    return  EFI_SUCCESS;

StopController_ErrorExit:

    HBA_PORT_WRITE_REG32(AhciBaseAddr,Port,HBA_PORTS_CLB,gCommandListBaseAddress);
    HBA_PORT_WRITE_REG32(AhciBaseAddr,Port,HBA_PORTS_FB,gFisBaseAddress);
    return Status;

}


/**
    Wait for memory to be set to the test value.

    @param  BaseAddr 
    @param  Port 
    @param  Register 
    @param  AndMask 
    @param  WaitTimeInMs 

    @retval EFI_STATUS

**/
EFI_STATUS
WaitForMemClear (
    IN  UINTN     BaseAddr,
    IN  UINT8     Port,
    IN  UINT8     Register,
    IN  UINT32    AndMask,
    IN  UINT32    WaitTimeInMs
)
{
    UINT8  Delay;
    while(WaitTimeInMs!=0){
        for ( Delay = 10; Delay > 0; Delay--) {

            if(!((HBA_PORT_REG32(BaseAddr, Port, Register)) & AndMask)){
                return EFI_SUCCESS;
            }

            gStallPpi->Stall( gPeiServices, gStallPpi, 100 );  // 100 usec * 10 = 1msec
        }
        WaitTimeInMs--;
   }
   return EFI_DEVICE_ERROR;
}

/**
    Wait for memory to be set to the test value.

    @param BaseAddr 
    @param Port 
    @param Register 
    @param AndMask 
    @param TestValue 
    @param WaitTimeInMs 

    @retval EFI_STATUS

**/
EFI_STATUS
WaitForMemSet (
    IN  UINTN     BaseAddr,
    IN  UINT8     Port,
    IN  UINT8     Register,
    IN  UINT32    AndMask,
    IN  UINT32    TestValue,
    IN  UINT32    WaitTimeInMs
)
{
    UINT8  Delay;
    while(WaitTimeInMs!=0){
        for ( Delay = 10; Delay > 0; Delay--) {

            if(((HBA_PORT_REG32(BaseAddr, Port, Register)) & AndMask) == TestValue){
                return EFI_SUCCESS;
            }

            gStallPpi->Stall( gPeiServices, gStallPpi, 100 );    // 100 usec * 10 = 1msec

        }
        WaitTimeInMs--;
   }
   return EFI_DEVICE_ERROR;
}

/**
    Issue a Port Reset

    @param  AhciControllerInfo  
    @param  Port
    @param  PMPort 
    @param  Speed 
    @param  PowerManagement
	 

    @retval EFI_STATUS

    @note
    1. Issue port reset by setting DET bit in SControl register
    2. Call HandlePortComReset to check the status of the reset.

**/
EFI_STATUS
GeneratePortReset (
    IN  AHCI_RECOVERY_CONTROLLER_INFO *AhciControllerInfo,
    UINT8                             Port,
    UINT8                             PMPort,
    UINT8                             Speed,
    UINT8                             PowerManagement
)
{

    EFI_STATUS  Status;
    UINTN       AhciBaseAddr = (UINTN) AhciControllerInfo->AhciBaseAddress;
    volatile    AHCI_RECEIVED_FIS  *FISAddress = *((AHCI_RECEIVED_FIS  **)(HBA_PORT_REG_BASE(Port) + AhciBaseAddr + HBA_PORTS_FB));


    if (!FISAddress) return EFI_DEVICE_ERROR;   // FIS receive address is not programmed.

    if (gPortReset) return EFI_SUCCESS;
    gPortReset = TRUE;


    // Disable Start bit
    HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_CMD, ~HBA_PORTS_CMD_ST);

    // Wait till CR is cleared    
    Status = WaitForMemClear(AhciBaseAddr, Port, HBA_PORTS_CMD,
                            HBA_PORTS_CMD_CR,
                            HBA_CR_CLEAR_TIMEOUT);
    if(EFI_ERROR(Status)) {
           return Status;
       }
    
    // Clear Status register
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR);
 
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR);

    //  Enable FIS Receive Enable
    HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_FRE);

    // Wait till FIS is running and then clear the data area
    Status = WaitForMemSet(AhciBaseAddr, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_FR,
                                    HBA_PORTS_CMD_FR,
                                    HBA_FR_CLEAR_TIMEOUT);
    if(EFI_ERROR(Status)) {
        return Status;
    }
        
    FISAddress->Ahci_Rfis[0] = 0;

    if (PMPort == 0xFF) {
        // Issue Port COMRESET
       HBA_PORT_REG32_AND_OR (AhciBaseAddr, Port, HBA_PORTS_SCTL, 0xFFFFF000, 
                    HBA_PORTS_SCTL_DET_INIT + (Speed << 4) + (PowerManagement << 8));
        gStallPpi->Stall( gPeiServices, gStallPpi, 1000 ); // 1msec

        HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_SCTL, ~HBA_PORTS_SCTL_DET_MASK);
    }
    else {
        gStallPpi->Stall( gPeiServices, gStallPpi, 1000 ); //1msec
    }
    
    Status = HandlePortComReset(AhciControllerInfo, Port, PMPort);

    //  Disable FIS Receive Enable
    HBA_PORT_REG32_AND (AhciBaseAddr, Port, HBA_PORTS_CMD, ~HBA_PORTS_CMD_FRE);

    gPortReset = FALSE;

    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;

}

/**
    Check if COM Reset is successful or not

    @param  AhciControllerInfo 
    @param  Port 
    @param  PMPort 

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
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort
)
{

    EFI_STATUS  Status = EFI_SUCCESS;
    BOOLEAN     DeviceDetected = FALSE;
    UINT32      Data32, i, SStatusData;
    UINTN       AhciBaseAddr = (UINTN)(AhciControllerInfo->AhciBaseAddress);
    volatile    AHCI_RECEIVED_FIS  *FISAddress;
    UINT32      SError = 0;
    //
    //  Check if detection is complete
    //
    for (i = 0; i < HBA_PRESENCE_DETECT_TIMEOUT; i++){   // Total delay 10msec
        SStatusData = ReadSCRRegister (AhciControllerInfo, Port, PMPort, 0); // SStatus
        SStatusData &= HBA_PORTS_SSTS_DET_MASK;
        if ((SStatusData == HBA_PORTS_SSTS_DET_PCE) || (SStatusData == HBA_PORTS_SSTS_DET)) {
            DeviceDetected = TRUE;
            break;
        }

        gStallPpi->Stall( gPeiServices, gStallPpi, 1000 );

    }

    if (DeviceDetected) {
        //
        // Wait till PhyRdy Change bit is set
        //
        if (PMPort == 0xFF) {
            Status = WaitForMemSet(AhciBaseAddr, Port, HBA_PORTS_SERR,
                            HBA_PORTS_SERR_EX,
                            HBA_PORTS_SERR_EX,
                            ATAPI_BUSY_CLEAR_TIMEOUT);
        }
        else {
//            Status = WaitforPMMemSet (SataDevInterface, PMPort, PSCR_1_SERROR,
//                    HBA_PORTS_SERR_EX, HBA_PORTS_SERR_EX, ATAPI_BUSY_CLEAR_TIMEOUT);
        }

        FISAddress = (AHCI_RECEIVED_FIS *)HBA_PORT_REG32(AhciBaseAddr, Port, HBA_PORTS_FB);


        for (i = 0; i < ATAPI_BUSY_CLEAR_TIMEOUT; ) {
            SError = ReadSCRRegister (AhciControllerInfo,  Port, PMPort, 2); // SError
            if (SError & HBA_PORTS_ERR_CHK) {
              WriteSCRRegister (AhciControllerInfo,  Port, PMPort, 1, HBA_PORTS_ERR_CLEAR ); //SError
            }
            if(FISAddress->Ahci_Rfis[0] == FIS_REGISTER_D2H) {break;}
            gStallPpi->Stall( gPeiServices, gStallPpi, 1000 );
                //
                // For device behind PM Port, there is a delay in writing
                // to the register. So count can be decreased.
                if (PMPort != 0xFF) {i+= 100;}
                    else { i++; }
        }
        //
        // Wait till PxTFD gets updated from D2H FIS
        //
        for (i = 0; i < 100; i++){   // Total delay 10msec
            WriteSCRRegister (AhciControllerInfo,  Port, PMPort, 1, HBA_PORTS_ERR_CLEAR); //SError
            if((FISAddress->Ahci_Rfis[2] & HBA_PORTS_TFD_MASK) == (HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_TFD) & HBA_PORTS_TFD_MASK)) {
                break;
            }

            gStallPpi->Stall( gPeiServices, gStallPpi, 100 );
        }
        //
        // check for errors.
        //
        if (FISAddress->Ahci_Rfis[2] & (HBA_PORTS_TFD_BSY | HBA_PORTS_TFD_ERR)) Status = EFI_DEVICE_ERROR;

        Data32 = HBA_PORT_REG32 (AhciBaseAddr, Port, HBA_PORTS_IS);
        if (Data32 & (BIT30 + BIT29 + BIT28 + BIT27 + BIT26)) Status = EFI_DEVICE_ERROR;
        //
        // Clear the status
        //
        WriteSCRRegister (AhciControllerInfo, Port, PMPort, 1, HBA_PORTS_ERR_CLEAR); //SError
        HBA_PORT_REG32_OR (AhciBaseAddr, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR);

    }
    else {
        Status = EFI_DEVICE_ERROR;
    }

    return Status;

}

/**
    Read from SCONTROL/Serror/SStatus register.

    @param  AhciControllerInfo
    @param  Port
    @param  PMPort
    @param  Register (0 : SStatus, 1: SError, 2: SControl)

    @retval UINT32
    @note  
    1. Check if the device is connected directly to the port
    2. if yes, read to the AHCI Controller else write to the Port Multiplier register.

**/
UINT32
ReadSCRRegister (
    IN  AHCI_RECOVERY_CONTROLLER_INFO   *AhciControllerInfo,
    IN  UINT8                           Port,
    IN  UINT8                           PMPort,
    IN  UINT8                           Register
)
{

    UINT32  Data32 = 0;
    UINT32  Reg = HBA_PORTS_SSTS;

    if (PMPort != 0xFF) {
//        ReadWritePMPort (SataDevInterface, PMPort, Register, &Data32, FALSE);
    }
    else {
        if (Register == 1) Reg = HBA_PORTS_SCTL;
        if (Register == 2) Reg = HBA_PORTS_SERR;
        Data32 = HBA_PORT_REG32 (AhciControllerInfo->AhciBaseAddress, Port, Reg);
    }

    return Data32;
}

/**
    Write to SCONTROL/Serror/SStatus register

    @param  AhciControllerInfo 
    @param  Port 
    @param  PMPort 
    @param  Register 
    @param  Data32 

    @retval EFI_STATUS

    @note
    1. Check if the device is connected directly to the port
    2. if yes, write to the AHCI Controller else write to the Port Multiplier register

**/
EFI_STATUS
WriteSCRRegister (
    IN  AHCI_RECOVERY_CONTROLLER_INFO *AhciControllerInfo,
    IN  UINT8                         Port,
    IN  UINT8                         PMPort,
    IN  UINT8                         Register,
    IN  UINT32                        Data32
)
{

    UINT32  Reg = HBA_PORTS_SSTS;

    if (PMPort != 0xFF) {
//        ReadWritePMPort (SataDevInterface, PMPort, Register, &Data32, TRUE);
    }
    else {
        if (Register == 2) Reg = HBA_PORTS_SCTL;
        if (Register == 1) Reg = HBA_PORTS_SERR;
        HBA_PORT_REG32_OR (AhciControllerInfo->AhciBaseAddress, Port, Reg, Data32);
    }

    return EFI_SUCCESS;
}

/**
    Clear the Memory

    @param Buffer
    @param Size

    @retval None
**/
VOID ZeroMemory (
    IN  VOID    *Buffer,
    IN  UINTN   Size
)
{
    UINT8 *ptr;
    ptr = Buffer;
    while ( Size-- ){
        *(ptr++) = 0;
    }
}

/**
    Copy Length bytes from Source to Destination.

    @param  DestinationBuffer 
    @param  SourceBuffer 
    @param  Length 

    @retval VOID

**/
VOID
CopyMemPei (
    IN  VOID    *DestinationBuffer,
    IN  VOID    *SourceBuffer,
    IN  UINTN   Length
)
{
    //
    // Declare the local variables that actually move the data elements as
    // volatile to prevent the optimizer from replacing this function with
    // the intrinsic memcpy()
    //
    volatile UINT8                    *Destination8;
    CONST UINT8                       *Source8;

    if (SourceBuffer > DestinationBuffer) {
        Destination8 = (UINT8*)DestinationBuffer;
        Source8 = (CONST UINT8*)SourceBuffer;
        while (Length-- != 0) {
            *(Destination8++) = *(Source8++);
        }
    } else if (SourceBuffer < DestinationBuffer) {
    Destination8 = (UINT8*)DestinationBuffer + Length;
    Source8 = (CONST UINT8*)SourceBuffer + Length;
        while (Length-- != 0) {
            *(--Destination8) = *(--Source8);
        }
    }
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
