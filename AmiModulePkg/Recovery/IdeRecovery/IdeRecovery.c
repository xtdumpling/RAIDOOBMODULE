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

/** @file IdeRecovery.c
    File implements IDE Recovery Functions
**/

//----------------------------------------------------------------------

#include "IdeRecovery.h"

//----------------------------------------------------------------------

EFI_GUID gIdeRecoveryNativeModePpiGuid = \
PEI_IDE_RECOVERY_NATIVE_MODE_PPI_GUID;

//
// IDE registers' fixed address
//
IDE_BASE_REGISTERS PrimaryChannelLegacyIdeIoPortRegisters = {
    0x1f0, 0x1f1, 0x1f2, 0x1f3, 0x1f4, 0x1f5, 0x1f6, 0x1f7, 0x3f6, 0x3f7
};

IDE_BASE_REGISTERS SecondaryChannelLegacyIdeIoPortRegisters = {
    0x170, 0x171, 0x172, 0x173, 0x174, 0x175, 0x176, 0x177, 0x376, 0x377
};

//
// AtaAtapiCommonInit should only be called once
//
BOOLEAN            gHaveCalledAtaAtapiCommonInit = FALSE;

BOOLEAN            gIdeRecoveryNativeModeSupport = FALSE;

IDE_BASE_REGISTERS PrimaryChannelNativeIdeIoPortRegisters = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1
};

IDE_BASE_REGISTERS SecondaryChannelNativeIdeIoPortRegisters = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1
};

IDE_BASE_REGISTERS *gNativeModePortRegsiters;
UINT8              gNativeIdeCount = 0;


EFI_PEI_PPI_DESCRIPTOR   Atapi_PpiDescriptor = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiVirtualBlockIoPpiGuid,
    NULL
};

EFI_PEI_PPI_DESCRIPTOR   Ata_PpiDescriptor = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiVirtualBlockIoPpiGuid,
    NULL
};

static EFI_PEI_SERVICES  **gPeiServices = NULL;
static EFI_PEI_STALL_PPI *gStallPpi     = NULL;
EFI_PEI_SERVICES    **ThisPeiServices;
/**
    Function to install EFI_PEI_RECOVERY_BLOCK_IO_PPI for ATAPI devices.

    @param FileHandle
    @param PeiServices

    @retval EFI_STATUS
**/
EFI_STATUS 
Atapi_RecoveryPeimEntry (
    IN  EFI_PEI_FILE_HANDLE FileHandle,
    IN  EFI_PEI_SERVICES    **PeiServices
)
{
    EFI_STATUS  Status  =   EFI_SUCCESS;
    IDE_RECOVERY_BLK_IO_DEV *Atapi_BlkIoDev =   NULL;

    if ( !gPeiServices ) {
        gPeiServices = PeiServices;
    }

    Status = (**PeiServices).AllocatePool( PeiServices, 
                                           sizeof(IDE_RECOVERY_BLK_IO_DEV), 
                                           (VOID**)&Atapi_BlkIoDev );

    if ( EFI_ERROR( Status )) {
        return EFI_OUT_OF_RESOURCES;
    }

    Atapi_BlkIoDev->HaveEnumeratedDevices = FALSE;
    Atapi_BlkIoDev->DeviceCount           = 0;

    Atapi_BlkIoDev->RecoveryBlkIo.GetNumberOfBlockDevices = Atapi_GetNumberOfBlockDevices;
    Atapi_BlkIoDev->RecoveryBlkIo.GetBlockDeviceMediaInfo = Atapi_GetBlockDeviceMediaInfo;
    Atapi_BlkIoDev->RecoveryBlkIo.ReadBlocks              = Atapi_ReadBlocks;
    Atapi_PpiDescriptor.Ppi = &Atapi_BlkIoDev->RecoveryBlkIo;

    Status = (**PeiServices).InstallPpi( PeiServices, &Atapi_PpiDescriptor );

    if ( EFI_ERROR( Status )) {
        return EFI_OUT_OF_RESOURCES;
    }

    return EFI_SUCCESS;
}

/**
    Function to install EFI_PEI_RECOVERY_BLOCK_IO_PPI for ATA devices.

    @param FfsHeader
    @param PeiServices

    @retval EFI_STATUS
**/
EFI_STATUS 
Ata_RecoveryPeimEntry (
    IN  EFI_PEI_FILE_HANDLE FileHandle,
    IN  EFI_PEI_SERVICES    **PeiServices
)
{
    EFI_STATUS  Status  =   EFI_SUCCESS;
    IDE_RECOVERY_BLK_IO_DEV *Ata_BlkIoDev   =   NULL;

    if ( !gPeiServices ) {
        gPeiServices = PeiServices;
    }

    Status = (**PeiServices).AllocatePool( PeiServices, 
                                           sizeof(IDE_RECOVERY_BLK_IO_DEV), 
                                           (VOID**)&Ata_BlkIoDev );

    if ( EFI_ERROR( Status )) {
        return EFI_OUT_OF_RESOURCES;
    }

    Ata_BlkIoDev->HaveEnumeratedDevices = FALSE;
    Ata_BlkIoDev->DeviceCount           = 0;

    Ata_BlkIoDev->RecoveryBlkIo.GetNumberOfBlockDevices = Ata_GetNumberOfBlockDevices;
    Ata_BlkIoDev->RecoveryBlkIo.GetBlockDeviceMediaInfo = Ata_GetBlockDeviceMediaInfo;
    Ata_BlkIoDev->RecoveryBlkIo.ReadBlocks              = Ata_ReadBlocks;
    Ata_PpiDescriptor.Ppi = &Ata_BlkIoDev->RecoveryBlkIo;

    Status = (**PeiServices).InstallPpi( PeiServices, &Ata_PpiDescriptor );

    if ( EFI_ERROR( Status )) {
        return EFI_OUT_OF_RESOURCES;
    }

    return EFI_SUCCESS;
}

/**
    Function to install EFI_PEI_RECOVERY_BLOCK_IO_PPI for ATAPI devices.

    @param FileHandle
    @param PeiServices

    @retval EFI_STATUS
**/
EFI_STATUS 
EFIAPI
IdeRecoveryPeimEntry (
  IN    EFI_PEI_FILE_HANDLE FileHandle,
  IN    CONST   EFI_PEI_SERVICES    **PeiServices
)
{
    #if defined(ATAPI_RECOVERY_SUPPORT) && ATAPI_RECOVERY_SUPPORT
        Atapi_RecoveryPeimEntry(FileHandle,PeiServices);
    #endif
    #if defined(ATA_RECOVERY_SUPPORT) && ATA_RECOVERY_SUPPORT
        Ata_RecoveryPeimEntry(FileHandle,PeiServices);
    #endif
    return EFI_SUCCESS;
}

/**
    This function called by Atapi_RecoveryPeimEntry and Ata_RecoveryPeimEntry
    functions. Initializes stuff, including gIdeRecoveryNativeModeSupport,
    PrimaryChannelNativeIdeIoPortRegisters, 
    SecondaryChannelNativeIdeIoPortRegisters.
 
    @param PeiServices

    @retval EFI_STATUS
**/
EFI_STATUS 
AtaAtapiCommonInit (
    IN  EFI_PEI_SERVICES    **PeiServices
)
{
    EFI_STATUS  Status;
    EFI_STATUS  LegacyStatus    =   EFI_NOT_FOUND;
    PEI_ATA_CONTROLLER_PPI  *AtaControllerPpi   =   NULL;
    PEI_IDE_RECOVERY_NATIVE_MODE_PPI    *IdeRecoveryNativeModePpi = NULL;
    UINT8 i     = 0;
    UINT8 Index = 0;

    Status = (**PeiServices).LocatePpi( PeiServices, 
                                        &gEfiPeiStallPpiGuid, 
                                        0, 
                                        NULL, 
                                        (VOID**)&gStallPpi );

    if ( EFI_ERROR( Status )) {
        return EFI_UNSUPPORTED;
    }

    Status = (**PeiServices).LocatePpi( PeiServices, 
                                        &gPeiAtaControllerPpiGuid, 
                                        0, 
                                        NULL, 
                                        (VOID**)&AtaControllerPpi );

    if ( !(EFI_ERROR( Status ))) {
        LegacyStatus = AtaControllerPpi->EnableAtaChannel( PeiServices, 
                                                           AtaControllerPpi, 
                                                           PEI_ICH_IDE_PRIMARY | PEI_ICH_IDE_SECONDARY );
    }

    //
    // Find Number of Native mode IDE ppi installed
    //
    Index = 0;

    do
    {
        Status = (**PeiServices).LocatePpi( PeiServices, 
                                            &gIdeRecoveryNativeModePpiGuid, 
                                            Index, 
                                            NULL, 
                                            (VOID**)&IdeRecoveryNativeModePpi );

        if ( !(EFI_ERROR( Status ))) {
            Index++;
            gNativeIdeCount++;
        }
    } while ( Status == EFI_SUCCESS );

    //
    // If Legacy mode init fails and if there is no native PPI return 
    // with error.
    //
    if ((gNativeIdeCount == 00) && (LegacyStatus != EFI_SUCCESS)) {
        return EFI_UNSUPPORTED;
    }

    Status = (**PeiServices).AllocatePool( PeiServices, 
                                           sizeof(IDE_BASE_REGISTERS) * 2 * gNativeIdeCount, 
                                           (VOID**)&gNativeModePortRegsiters );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    //
    // Fill the base address values in gNativeModePortRegsiters
    //
    Index = 0;

    do
    {
        Status = (**PeiServices).LocatePpi( PeiServices, 
                                            &gIdeRecoveryNativeModePpiGuid, 
                                            Index, 
                                            NULL, 
                                            (VOID**)&IdeRecoveryNativeModePpi );

        if ( !(EFI_ERROR( Status ))) {
            gIdeRecoveryNativeModeSupport             = TRUE;
            gNativeModePortRegsiters[i].Data          = PrimaryChannelNativeIdeIoPortRegisters.Data + IdeRecoveryNativeModePpi->PCMDBarAddress;
            gNativeModePortRegsiters[i].Reg1.Error    = PrimaryChannelNativeIdeIoPortRegisters.Reg1.Error + IdeRecoveryNativeModePpi->PCMDBarAddress;
            gNativeModePortRegsiters[i].SectorCount   = PrimaryChannelNativeIdeIoPortRegisters.SectorCount + IdeRecoveryNativeModePpi->PCMDBarAddress;
            gNativeModePortRegsiters[i].SectorNumber  = PrimaryChannelNativeIdeIoPortRegisters.SectorNumber + IdeRecoveryNativeModePpi->PCMDBarAddress;
            gNativeModePortRegsiters[i].CylinderLsb   = PrimaryChannelNativeIdeIoPortRegisters.CylinderLsb + IdeRecoveryNativeModePpi->PCMDBarAddress;
            gNativeModePortRegsiters[i].CylinderMsb   = PrimaryChannelNativeIdeIoPortRegisters.CylinderMsb + IdeRecoveryNativeModePpi->PCMDBarAddress;
            gNativeModePortRegsiters[i].Head          = PrimaryChannelNativeIdeIoPortRegisters.Head + IdeRecoveryNativeModePpi->PCMDBarAddress;
            gNativeModePortRegsiters[i].Reg.Command   = PrimaryChannelNativeIdeIoPortRegisters.Reg.Command + IdeRecoveryNativeModePpi->PCMDBarAddress;
            gNativeModePortRegsiters[i].Alt.AltStatus = PrimaryChannelNativeIdeIoPortRegisters.Alt.AltStatus + IdeRecoveryNativeModePpi->PCNLBarAddress;
            gNativeModePortRegsiters[i].DriveAddress  = PrimaryChannelNativeIdeIoPortRegisters.DriveAddress + IdeRecoveryNativeModePpi->PCNLBarAddress;

            gNativeModePortRegsiters[i + 1].Data          = SecondaryChannelNativeIdeIoPortRegisters.Data + IdeRecoveryNativeModePpi->SCMDBarAddress;
            gNativeModePortRegsiters[i + 1].Reg1.Error    = SecondaryChannelNativeIdeIoPortRegisters.Reg1.Error + IdeRecoveryNativeModePpi->SCMDBarAddress;
            gNativeModePortRegsiters[i + 1].SectorCount   = SecondaryChannelNativeIdeIoPortRegisters.SectorCount + IdeRecoveryNativeModePpi->SCMDBarAddress;
            gNativeModePortRegsiters[i + 1].SectorNumber  = SecondaryChannelNativeIdeIoPortRegisters.SectorNumber += IdeRecoveryNativeModePpi->SCMDBarAddress;
            gNativeModePortRegsiters[i + 1].CylinderLsb   = SecondaryChannelNativeIdeIoPortRegisters.CylinderLsb + IdeRecoveryNativeModePpi->SCMDBarAddress;
            gNativeModePortRegsiters[i + 1].CylinderMsb   = SecondaryChannelNativeIdeIoPortRegisters.CylinderMsb + IdeRecoveryNativeModePpi->SCMDBarAddress;
            gNativeModePortRegsiters[i + 1].Head          = SecondaryChannelNativeIdeIoPortRegisters.Head + IdeRecoveryNativeModePpi->SCMDBarAddress;
            gNativeModePortRegsiters[i + 1].Reg.Command   = SecondaryChannelNativeIdeIoPortRegisters.Reg.Command + IdeRecoveryNativeModePpi->SCMDBarAddress;
            gNativeModePortRegsiters[i + 1].Alt.AltStatus = SecondaryChannelNativeIdeIoPortRegisters.Alt.AltStatus + IdeRecoveryNativeModePpi->SCNLBarAddress;
            gNativeModePortRegsiters[i + 1].DriveAddress  = SecondaryChannelNativeIdeIoPortRegisters.DriveAddress + IdeRecoveryNativeModePpi->SCNLBarAddress;
            i = i + 2;
            Index++;
        }
    } while ( Status == EFI_SUCCESS );

    return EFI_SUCCESS;
}

/**
    GetNumberOfBlockDevices function of EFI_PEI_RECOVERY_BLOCK_IO_PPI.

    @param PeiServices
    @param This
    @param NumberBlockDevices

    @retval EFI_STATUS
**/
EFI_STATUS 
EFIAPI
Atapi_GetNumberOfBlockDevices (
    IN  EFI_PEI_SERVICES    **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    OUT UINTN   *NumberBlockDevices
)
{
    IDE_RECOVERY_BLK_IO_DEV *Atapi_BlkIoDev = NULL;

    ThisPeiServices = PeiServices;
    if ( This == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    Atapi_BlkIoDev = (IDE_RECOVERY_BLK_IO_DEV*)This;

    if ( !Atapi_BlkIoDev->HaveEnumeratedDevices ) {
        EnumerateDevices( Atapi_BlkIoDev, EnumerateAtapi );
        Atapi_BlkIoDev->HaveEnumeratedDevices = TRUE;
    }
    *NumberBlockDevices = Atapi_BlkIoDev->DeviceCount;

    return EFI_SUCCESS;
}

/**
    GetBlockDeviceMediaInfo function of EFI_PEI_RECOVERY_BLOCK_IO_PPI.

    @param PeiServices
    @param This
    @param DeviceIndex
    @param MediaInfo

    @retval EFI_STATUS
**/
EFI_STATUS 
EFIAPI
Atapi_GetBlockDeviceMediaInfo (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    IN  UINTN                           DeviceIndex,
    OUT EFI_PEI_BLOCK_IO_MEDIA          *MediaInfo
)
{
    IDE_RECOVERY_BLK_IO_DEV *Atapi_BlkIoDev = NULL;
    EFI_STATUS              Status          = EFI_SUCCESS;

    if ((This == NULL) || (MediaInfo == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    Atapi_BlkIoDev = (IDE_RECOVERY_BLK_IO_DEV*)This;

    if ( !Atapi_BlkIoDev->HaveEnumeratedDevices ) {
        EnumerateDevices( Atapi_BlkIoDev, EnumerateAtapi );
        Atapi_BlkIoDev->HaveEnumeratedDevices = TRUE;
    }

    if ( DeviceIndex > (Atapi_BlkIoDev->DeviceCount - 1)) {
        return EFI_INVALID_PARAMETER;
    }

    if ( !Atapi_BlkIoDev->DeviceInfo[DeviceIndex]->LookedForMedia ) {
        Status = Atapi_DetectMedia(Atapi_BlkIoDev->DeviceInfo[DeviceIndex]);
        if ( Status != EFI_SUCCESS ) {
            return EFI_DEVICE_ERROR;
        }
		
        Atapi_BlkIoDev->DeviceInfo[DeviceIndex]->LookedForMedia = TRUE;
    }

    *MediaInfo = Atapi_BlkIoDev->DeviceInfo[DeviceIndex]->MediaInfo;

    return EFI_SUCCESS;
}

/**
    ReadBlocks function of EFI_PEI_RECOVERY_BLOCK_IO_PPI.

    @param PeiServices
    @param This
    @param DeviceIndex
    @param StartLba
    @param BufferSize
    @param Buffer

    @retval EFI_STATUS
**/
EFI_STATUS 
EFIAPI
Atapi_ReadBlocks (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    IN  UINTN                           DeviceIndex,
    IN  EFI_PEI_LBA                     StartLba,
    IN  UINTN                           BufferSize,
    OUT VOID                            *Buffer
)
{
    EFI_PEI_BLOCK_IO_MEDIA  MediaInfo;
    EFI_STATUS              Status          = EFI_SUCCESS;
    UINTN                   NumberOfBlocks  = 0;
    UINTN                   BlockSize       = 0;
    IDE_RECOVERY_BLK_IO_DEV *Atapi_BlkIoDev = NULL;

    if ( This == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    Atapi_BlkIoDev = (IDE_RECOVERY_BLK_IO_DEV*)This;

    if ( Buffer == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    if ( BufferSize == 0 ) {
        return EFI_SUCCESS;
    }

    if ( !Atapi_BlkIoDev->HaveEnumeratedDevices ) {
        EnumerateDevices( Atapi_BlkIoDev, EnumerateAtapi );
        Atapi_BlkIoDev->HaveEnumeratedDevices = TRUE;
    }

    if ( !Atapi_BlkIoDev->DeviceInfo[DeviceIndex]->LookedForMedia ) {
        Status = Atapi_GetBlockDeviceMediaInfo( PeiServices, 
                                                This, 
                                                DeviceIndex, 
                                                &MediaInfo );

        if ( Status != EFI_SUCCESS ) {
            return Status;
        }
    } else {
        MediaInfo = Atapi_BlkIoDev->DeviceInfo[DeviceIndex]->MediaInfo;
    }

    BlockSize = MediaInfo.BlockSize;

    if ((BufferSize % BlockSize) != 0 ) {
        return EFI_INVALID_PARAMETER;
    }

    if ( !MediaInfo.MediaPresent ) {
        return EFI_NO_MEDIA;
    }

    NumberOfBlocks = BufferSize / BlockSize;

    Status = Atapi_ReadSectors(
        Atapi_BlkIoDev->DeviceInfo[DeviceIndex],
        Buffer,
        StartLba,
        NumberOfBlocks,
        BlockSize
        );

    return Status;
}

/**
    GetNumberOfBlockDevices function of EFI_PEI_RECOVERY_BLOCK_IO_PPI.

    @param PeiServices
    @param This
    @param NumberBlockDevices

    @retval EFI_STATUS
**/
EFI_STATUS 
EFIAPI
Ata_GetNumberOfBlockDevices (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    OUT UINTN                           *NumberBlockDevices
)
{
    IDE_RECOVERY_BLK_IO_DEV *Ata_BlkIoDev = NULL;

    if ( This == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    Ata_BlkIoDev = (IDE_RECOVERY_BLK_IO_DEV*)This;

    if ( !Ata_BlkIoDev->HaveEnumeratedDevices ) {
        EnumerateDevices( Ata_BlkIoDev, EnumerateAta );
        Ata_BlkIoDev->HaveEnumeratedDevices = TRUE;
    }
    *NumberBlockDevices = Ata_BlkIoDev->DeviceCount;

    return EFI_SUCCESS;
}

/**
    GetBlockDeviceMediaInfo function of EFI_PEI_RECOVERY_BLOCK_IO_PPI.

    @param PeiServices
    @param This
    @param DeviceIndex
    @param MediaInfo

    @retval EFI_STATUS
**/
EFI_STATUS 
EFIAPI
Ata_GetBlockDeviceMediaInfo (
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI   *This,
    IN  UINTN                           DeviceIndex,
    OUT EFI_PEI_BLOCK_IO_MEDIA          *MediaInfo
)
{
    IDE_RECOVERY_BLK_IO_DEV *Ata_BlkIoDev          = NULL;

    if ((This == NULL) || (MediaInfo == NULL)) {
        return EFI_INVALID_PARAMETER;
    }

    Ata_BlkIoDev = (IDE_RECOVERY_BLK_IO_DEV*)This;

    if ( !Ata_BlkIoDev->HaveEnumeratedDevices ) {
        EnumerateDevices( Ata_BlkIoDev, EnumerateAta );
        Ata_BlkIoDev->HaveEnumeratedDevices = TRUE;
    }

    if ( DeviceIndex > (Ata_BlkIoDev->DeviceCount - 1)) {
        return EFI_INVALID_PARAMETER;
    }

    if ( !Ata_BlkIoDev->DeviceInfo[DeviceIndex]->LookedForMedia ) {
        Ata_BlkIoDev->DeviceInfo[DeviceIndex]->LookedForMedia = TRUE;

    }

    *MediaInfo = Ata_BlkIoDev->DeviceInfo[DeviceIndex]->MediaInfo;

    return EFI_SUCCESS;
}

/**
    ReadBlocks function of EFI_PEI_RECOVERY_BLOCK_IO_PPI.

    @param PeiServices
    @param This
    @param DeviceIndex
    @param StartLba
    @param BufferSize
    @param Buffer

    @retval EFI_STATUS
**/
EFI_STATUS 
EFIAPI
Ata_ReadBlocks (
    IN  EFI_PEI_SERVICES              **PeiServices,
    IN  EFI_PEI_RECOVERY_BLOCK_IO_PPI *This,
    IN  UINTN                         DeviceIndex,
    IN  EFI_PEI_LBA                   StartLba,
    IN  UINTN                         BufferSize,
    OUT VOID                         *Buffer
)
{
    EFI_PEI_BLOCK_IO_MEDIA  MediaInfo;
    EFI_STATUS              Status         = EFI_SUCCESS;
    UINTN                   NumberOfBlocks = 0;
    UINTN                   BlockSize      = 0;
    IDE_RECOVERY_BLK_IO_DEV *Ata_BlkIoDev  = NULL;

    if ( This == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    Ata_BlkIoDev = (IDE_RECOVERY_BLK_IO_DEV*)This;

    if ( Buffer == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    if ( BufferSize == 0 ) {
        return EFI_SUCCESS;
    }

    if ( !Ata_BlkIoDev->HaveEnumeratedDevices ) {
        EnumerateDevices( Ata_BlkIoDev, EnumerateAta );
        Ata_BlkIoDev->HaveEnumeratedDevices = TRUE;
    }

    if ( !Ata_BlkIoDev->DeviceInfo[DeviceIndex]->LookedForMedia ) {
        Status = Ata_GetBlockDeviceMediaInfo(
                                    PeiServices,
                                    This,
                                    DeviceIndex,
                                    &MediaInfo
                                    );

        if ( Status != EFI_SUCCESS ) {
            return Status;
        }
    }  else {
        MediaInfo = Ata_BlkIoDev->DeviceInfo[DeviceIndex]->MediaInfo;
    }

    BlockSize = MediaInfo.BlockSize;

    if ( BufferSize % BlockSize != 0 ) {
        return EFI_INVALID_PARAMETER;
    }

    if ( !MediaInfo.MediaPresent ) {
        return EFI_NO_MEDIA;
    }

    NumberOfBlocks = BufferSize / BlockSize;

    if ( StartLba > MediaInfo.LastBlock ) {
        return EFI_INVALID_PARAMETER;
    }

    if ((StartLba + NumberOfBlocks) > (MediaInfo.LastBlock + 1)) {
        return EFI_INVALID_PARAMETER;
    }

    Status = Ata_ReadSectors(
                        Ata_BlkIoDev->DeviceInfo[DeviceIndex],
                        Buffer,
                        StartLba,
                        NumberOfBlocks,
                        BlockSize
                        );
    return Status;
}

/**
    This function enumerates Ata or Atapi devices.  Called by
    Ata_GetNumberOfBlockDevices or Atapi_GetNumberOfBlockDevices.

    @param IdeRecoveryBlkIoDev
    @param EnumerateType

    @retval VOID
**/
VOID 
EnumerateDevices (
    IN  IDE_RECOVERY_BLK_IO_DEV *IdeRecoveryBlkIoDev,
    ENUMERATE_TYPE              EnumerateType
)
{
    EFI_STATUS               Status      = EFI_SUCCESS;
    UINT8                    MaxModes    = IdeMaxMode;
    EFI_IDE_MODE             Mode        = 0;
    UINT8                    Channel     = 0;
    UINT8                    Device      = 0;
    UINTN                    DeviceCount = 0;
    IDE_RECOVERY_DEVICE_INFO DeviceInfo;
    UINT8                    NativeIdeModeCount = 0;


    if ( !gHaveCalledAtaAtapiCommonInit ) {
        AtaAtapiCommonInit( gPeiServices );
        gHaveCalledAtaAtapiCommonInit = TRUE;
    }

    if ( gIdeRecoveryNativeModeSupport ) {
        //
        // Based on the number of Nativemode PPI maximum mode support is adjusted
        //
        MaxModes = IdeMaxMode + gNativeIdeCount - 1;
    }  else {
        MaxModes = IdeNative;
    }

    for ( Mode = IdeLegacy; Mode < MaxModes; Mode++ ) {
        for ( Channel = IdePrimary; Channel < IdeMaxChannel; Channel++ ) {
            for ( Device = IdeMaster; Device < IdeMaxDevice; Device++ ) {

                DeviceInfo.Device = Device;
                if ( Mode == IdeLegacy ) {
                    if ( Channel == IdePrimary ) {
                        DeviceInfo.IdeIoPortRegisters = &PrimaryChannelLegacyIdeIoPortRegisters;
                    } else { // IdeSecondary
                        DeviceInfo.IdeIoPortRegisters = &SecondaryChannelLegacyIdeIoPortRegisters;
                    }
                } else { // IdeNative
                    if ( Channel == IdePrimary ) {
                        DeviceInfo.IdeIoPortRegisters = &gNativeModePortRegsiters[NativeIdeModeCount];
                    } else { // IdeSecondary
                        DeviceInfo.IdeIoPortRegisters = &gNativeModePortRegsiters[NativeIdeModeCount + 1];
                    }
                }

                if ( EnumerateType == EnumerateAtapi ) {
                    if ( Atapi_DiscoverDevice( &DeviceInfo )) {
                        Status = (**gPeiServices).AllocatePool( gPeiServices, 
                                                                sizeof(IDE_RECOVERY_DEVICE_INFO), 
                                                                (VOID**)&(IdeRecoveryBlkIoDev->DeviceInfo[DeviceCount]));

                        if ( EFI_ERROR( Status )) {
                            return;
                        }
                        DeviceInfo.MediaInfo.MediaPresent               = FALSE;
                        DeviceInfo.MediaInfo.LastBlock                  = 0;
                        DeviceInfo.LookedForMedia                       = FALSE;
                        *(IdeRecoveryBlkIoDev->DeviceInfo[DeviceCount]) = DeviceInfo;
                        DeviceCount++;
                    }
                } else { 
                    //
                    // EnumerateAta
                    //
                    if ( Ata_DiscoverDevice( &DeviceInfo )) {
                        Status = (**gPeiServices).AllocatePool( gPeiServices, 
                                                                sizeof(IDE_RECOVERY_DEVICE_INFO), 
                                                                (VOID**)&(IdeRecoveryBlkIoDev->DeviceInfo[DeviceCount]));

                        if ( EFI_ERROR( Status )) {
                            return;
                        }
                        DeviceInfo.MediaInfo.MediaPresent               = TRUE;
                        DeviceInfo.MediaInfo.BlockSize                  = 0x200;
                        DeviceInfo.LookedForMedia                       = FALSE;
                        *(IdeRecoveryBlkIoDev->DeviceInfo[DeviceCount]) = DeviceInfo;
                        DeviceCount++;
                    }
                }
            }
        }

        if ( Mode == IdeNative ) {
            //
            // Index to the Next native mode PPI base address
            //
            NativeIdeModeCount = NativeIdeModeCount + 2;
        }
    }

    IdeRecoveryBlkIoDev->DeviceCount = DeviceCount;
}

/**
    This function called by EnumerateDevices. Looks for ATAPI_SIGNATURE.

    @param DeviceInfo

    @retval BOOLEAN
**/
BOOLEAN 
Atapi_DiscoverDevice (
    IDE_RECOVERY_DEVICE_INFO    *DeviceInfo
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8      Device      = DeviceInfo->Device;
    UINT16     HeadReg     = DeviceInfo->IdeIoPortRegisters->Head;
    UINT16     StatusReg   = DeviceInfo->IdeIoPortRegisters->Reg.Status;
    UINT16     CylinderLsb = DeviceInfo->IdeIoPortRegisters->CylinderLsb;
    UINT16     CylinderMsb = DeviceInfo->IdeIoPortRegisters->CylinderMsb;
    UINT8      StatusValue = 0;
    UINT8      Data8       = 0;

    //
    // Select Device
    //
    IoWrite8( HeadReg, Device << 4 );
    StatusValue = IoRead8( StatusReg );

    if ((StatusValue == 0x7f) || (StatusValue == 0xff)) {
        return FALSE;
    }

    Status = WaitForBSYClear( DeviceInfo->IdeIoPortRegisters, 31000 );
    IoRead8( StatusReg );

    if ( EFI_ERROR( Status )) {
        return FALSE;
    }

    Data8 = IoRead8( CylinderLsb );

    if ( Data8 == (ATAPI_SIGNATURE & 0xff)) {
        Data8 = IoRead8( CylinderMsb );

        if ( Data8 == (ATAPI_SIGNATURE >> 8)) {
            Status = Inquiry( DeviceInfo );

            if ( !EFI_ERROR( Status )) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

/**
    This function called by EnumerateDevices. Looks at IDENTIFY data.

    @param DeviceInfo

    @retval BOOLEAN
**/
BOOLEAN 
Ata_DiscoverDevice (
    IDE_RECOVERY_DEVICE_INFO    *DeviceInfo
)
{
    EFI_STATUS     Status = EFI_SUCCESS;
    UINT8          Device      = DeviceInfo->Device;
    UINT16         HeadReg     = DeviceInfo->IdeIoPortRegisters->Head;
    UINT16         StatusReg   = DeviceInfo->IdeIoPortRegisters->Reg.Status;
    UINT16         CylinderLsb = DeviceInfo->IdeIoPortRegisters->CylinderLsb;
    UINT16         CylinderMsb = DeviceInfo->IdeIoPortRegisters->CylinderMsb;
    UINT8          StatusValue = 0;
    UINT8          Data8       = 0;

    ATA_IDENTIFY_DATA AtaIdentifyData;


    //
    // Select Device
    //
    IoWrite8( HeadReg, Device << 4 );
    StatusValue = IoRead8( StatusReg );

    if ((StatusValue == 0x7f) || (StatusValue == 0xff)) {
        return FALSE;
    }

    Status = WaitForBSYClear( DeviceInfo->IdeIoPortRegisters, 31000 );
    IoRead8( StatusReg );

    if ( EFI_ERROR( Status )) {
        return FALSE;
    }

    Data8 = IoRead8( CylinderLsb );

    if ( Data8 == (ATAPI_SIGNATURE & 0xff)) {
        Data8 = IoRead8( CylinderMsb );

        if ( Data8 == (ATAPI_SIGNATURE >> 8)) {
            return FALSE;
        }
    }

    if ( AtaIdentify( DeviceInfo, &AtaIdentifyData ) == EFI_SUCCESS ) {
        if ((AtaIdentifyData.GeneralConfiguration_0 & 0x8000) == 0 ) {
            DeviceInfo->MediaInfo.DeviceType = MaxDeviceType;
            DeviceInfo->MediaInfo.LastBlock  = AtaIdentifyData.TotalUserAddressableSectors_60;
#if LBA48_SUPPORT
            DeviceInfo->Lba48Bit = FALSE;
            if ( AtaIdentifyData.Command_Set_Supported_83 & 0x400 ) {
                DeviceInfo->MediaInfo.LastBlock = AtaIdentifyData.LBA_48;
                DeviceInfo->Lba48Bit = TRUE;
                PEI_TRACE((-1, ThisPeiServices, "\n48-bit LBA detected\n"));
            }
#endif
            return TRUE;
        }
    }

    return FALSE;
}

/**
    This function executes various ATAPI packet commands
    (TEST_UNIT_READY, INQUIRY, REQUEST_SENSE, READ_CAPACITY, etc).

    @param DeviceInfo
    @param Packet
    @param Buffer
    @param ByteCount
    @param TimeoutInMilliSeconds

    @retval EFI_STATUS
**/
EFI_STATUS 
AtapiPacketCommandIn (
    IN  IDE_RECOVERY_DEVICE_INFO    *DeviceInfo,
    IN  ATAPI_PACKET_COMMAND        *Packet,
    IN  UINT16                      *Buffer,
    IN  UINT32                      ByteCount,
    IN  UINTN                       TimeoutInMilliSeconds
)
{
    IDE_BASE_REGISTERS *IdeIoPortRegisters = DeviceInfo->IdeIoPortRegisters;

    UINT8           Device           = DeviceInfo->Device;
    UINT16          StatusReg        = IdeIoPortRegisters->Reg.Status;
    UINT16          HeadReg          = IdeIoPortRegisters->Head;
    UINT16          CommandReg       = IdeIoPortRegisters->Reg.Command;
    UINT16          FeatureReg       = IdeIoPortRegisters->Reg1.Feature;
    UINT16          CylinderLsbReg   = IdeIoPortRegisters->CylinderLsb;
    UINT16          CylinderMsbReg   = IdeIoPortRegisters->CylinderMsb;
    UINT16          DeviceControlReg = IdeIoPortRegisters->Alt.DeviceControl;
    UINT16          DataReg          = IdeIoPortRegisters->Data;
    EFI_STATUS      Status           = EFI_SUCCESS;
    UINT32          Count            = 0;
    UINT16          *CommandIndex    = NULL;
    UINT16          *ptrBuffer       = Buffer;
    UINT32          Index            = 0;
    UINT8           StatusValue      = 0;
    UINT32          WordCount        = 0;

    //
    // Required transfer data in word unit.
    //
    UINT32  RequiredWordCount = 0;

    //
    // Actual transfer data in word unit.
    //
    UINT32  ActualWordCount = 0;

    //
    // Select device via Device/Head Register.
    // DEFAULT_CMD: 0xa0 (1010,0000)
    //
    IoWrite8( HeadReg, (UINT8) ((Device << 4) | DEFAULT_CMD ));

    //
    // Set all the command parameters by fill related registers.
    // Before write to all the following registers, BSY and DRQ must be 0.
    //
    if ( DRQClear2 (
             DeviceInfo->IdeIoPortRegisters,
             TIMEOUT
             ) != EFI_SUCCESS ) {
        return EFI_DEVICE_ERROR;
    }


    //
    // No OVL; No DMA
    //
    IoWrite8( FeatureReg, 0x00 );

    //
    // Set the transfersize to MAX_ATAPI_BYTE_COUNT to let the device
    // determine how many data should be transfered.
    //
    IoWrite8( CylinderLsbReg, (UINT8) ( MAX_ATAPI_BYTE_COUNT & 0x00ff ));
    IoWrite8( CylinderMsbReg, (UINT8) ( MAX_ATAPI_BYTE_COUNT >> 8 ));

    //
    //  DEFAULT_CTL:0x0a (0000,1010)
    //  Disable interrupt
    //
    IoWrite8( DeviceControlReg, DEFAULT_CTL );

    //
    // Send Packet command to inform device
    // that the following data bytes are command packet.
    //
    IoWrite8( CommandReg, PACKET_CMD );

    Status = DRQReady( IdeIoPortRegisters, TimeoutInMilliSeconds );

    if ( Status != EFI_SUCCESS ) {
        return Status;
    }
    //
    // Send out command packet
    //
    CommandIndex = Packet->Data16;

    for ( Count = 0; Count < 6; Count++, CommandIndex++ ) {
        IoWrite16( DataReg, *CommandIndex );
        gStallPpi->Stall( gPeiServices, gStallPpi, 10 );
    }

    IoRead8( StatusReg );
    Status = WaitForBSYClear( IdeIoPortRegisters, LONGTIMEOUT );
    if(EFI_ERROR(Status)) {
        return Status;
    }
    StatusValue = IoRead8( StatusReg );

    if ((StatusValue & ERR) == ERR ) {
        //
        // Trouble! Something's wrong here... Wait some time and return. 
        // 3 second is supposed to be long enough for a device reset latency 
        // or error recovery
        //
        gStallPpi->Stall( gPeiServices, gStallPpi, 100000 );

        return EFI_DEVICE_ERROR;
    }

    if ( Buffer == NULL || ByteCount == 0 ) {
        return EFI_SUCCESS;
    }
    //
    // Call PioReadWriteData() function to get
    // requested transfer data form device.
    //
    ptrBuffer         = Buffer;
    RequiredWordCount = ByteCount / 2;
    //
    // ActuralWordCount means the word count of data really transfered.
    //
    ActualWordCount = 0;

    while (ActualWordCount < RequiredWordCount)
    {
        //
        // Before each data transfer stream, the host should poll DRQ bit ready,
        // which informs device is ready to transfer data.
        //
        if ( DRQReady2( IdeIoPortRegisters, TimeoutInMilliSeconds ) != EFI_SUCCESS ) {
            return CheckErrorStatus( StatusReg );
        }
        //
        // Read Status Register will clear interrupt
        //
        IoRead8( StatusReg );

        //
        // Get current data transfer size from Cylinder Registers.
        //
        WordCount = (( IoRead8( CylinderMsbReg ) << 8) | IoRead8( CylinderLsbReg )) & 0xffff;
        WordCount /= 2;

        //
        // Perform a series data In/Out.
        //
        for ( Index = 0; (Index < WordCount) && (ActualWordCount < RequiredWordCount); Index++, ActualWordCount++ ) {
            *ptrBuffer = IoRead16( DataReg );

            ptrBuffer++;
        }

        if (((GENERIC_CMD*) Packet)->OperationCode_0 == REQUEST_SENSE && ActualWordCount >= 4 ) {
            RequiredWordCount = EFI_MIN( RequiredWordCount,
                                (UINT32) ( 4 + (((REQUEST_SENSE_DATA*) Buffer)->AdditionalSenseLength_7 / 2))
                                );
        }
    }
    //
    // After data transfer is completed, normally, DRQ bit should clear.
    //
    Status = DRQClear2( IdeIoPortRegisters, TimeoutInMilliSeconds );

    if ( Status != EFI_SUCCESS ) {
        return EFI_DEVICE_ERROR;
    }
    //
    // Read status register to check whether error happens.
    //
    Status = CheckErrorStatus( StatusReg );
    return Status;
}

/**
    ATAPI packet command INQUIRY. Uses AtapiPacketCommandIn.

    @param DeviceInfo

    @retval EFI_STATUS
**/
EFI_STATUS 
Inquiry (
    IN  OUT IDE_RECOVERY_DEVICE_INFO    *DeviceInfo
)
{
    ATAPI_PACKET_COMMAND Packet;
    EFI_STATUS           Status;
    INQUIRY_DATA         Idata;

    ZeroMem( &Packet, sizeof (ATAPI_PACKET_COMMAND));

    Packet.Cmd.OperationCode_0    = INQUIRY;
    Packet.Cmd.AllocationLength_4 = sizeof (INQUIRY_DATA);

    Status = AtapiPacketCommandIn(
        DeviceInfo,
        &Packet,
        (UINT16*)(&Idata),
        sizeof (INQUIRY_DATA),
        TIMEOUT
        );

    if ( Status != EFI_SUCCESS ) {
        return EFI_DEVICE_ERROR;
    }

    if ( Idata.PeripheralDeviceType_0 == CD_ROM_DEVICE ) {
        DeviceInfo->MediaInfo.DeviceType   = IdeCDROM;
        DeviceInfo->MediaInfo.MediaPresent = FALSE;
        DeviceInfo->MediaInfo.LastBlock    = 0;
        DeviceInfo->MediaInfo.BlockSize    = 0x800;
        return EFI_SUCCESS;
    } else {
        return EFI_UNSUPPORTED;
    }
}

/**
    -Loop: Issues ATAPI packet command TEST_UNIT_READY.
    If this is not successful, then look at SENSE data.
    If you get the sense data for (1) becoming ready, (2) media
    changed, or (3) power on reset, then stall for some time and
    continue looping with TEST_UNIT_READY and REQUEST_SENSE.
    -If TEST_UNIT_READY is successful, then issue READ_CAPACITY
    packet command, which should get device info.
    -If TEST_UNIT_READY is not succesful or if the SENSE data
    contains some other error (not one of the three listed above),
    then return an error.

    @param DeviceInfo

    @retval EFI_STATUS
**/
EFI_STATUS 
Atapi_DetectMedia (
    IN  OUT IDE_RECOVERY_DEVICE_INFO *DeviceInfo
)
{
    UINTN              Index = 0;
    REQUEST_SENSE_DATA SenseBuffers[MAX_SENSE_KEY_COUNT];
    EFI_STATUS         Status        = EFI_SUCCESS;
    UINT8              SenseCounts   = 0;
    BOOLEAN            DeviceIsReady;

    DeviceInfo->MediaInfo.MediaPresent = FALSE;
    DeviceInfo->MediaInfo.LastBlock    = 0;

    DeviceIsReady = FALSE;

    for ( Index = 0; Index < 50; Index++ ) {
        Status = TestUnitReady( DeviceInfo );

        if ( Status == EFI_SUCCESS ) {
            DeviceIsReady = TRUE;
            break;
        }

        SenseCounts = MAX_SENSE_KEY_COUNT;
        Status      = RequestSense( DeviceInfo, SenseBuffers, &SenseCounts );

        if ( SenseCounts == 0 ) {
            gStallPpi->Stall( gPeiServices, gStallPpi, 250000 );
            continue;
        }

        if (((SenseBuffers[0].SenseKey_2 == SK_NOT_READY) && (SenseBuffers[0].AdditionalSenseCode_12 == 4) && (SenseBuffers[0].AdditionalSenseCodeQualifier_13 == 1))     // Becoming ready
            || ((SenseBuffers[0].SenseKey_2 == SK_UNIT_ATTENTION) && (SenseBuffers[0].AdditionalSenseCode_12 == 0x28) && (SenseBuffers[0].AdditionalSenseCodeQualifier_13 == 0))     // Media Changed
            || ((SenseBuffers[0].SenseKey_2 == SK_UNIT_ATTENTION) && (SenseBuffers[0].AdditionalSenseCode_12 == 0x29) && (SenseBuffers[0].AdditionalSenseCodeQualifier_13 == 0))) {   // Power on Reset
            gStallPpi->Stall( gPeiServices, gStallPpi, 250000 );
        } else {
            break;
        }
    }

    if ( DeviceIsReady ) {
        Status = ReadCapacity( DeviceInfo );
        return Status;
    }

    return EFI_DEVICE_ERROR;
}

/**
    ATAPI command TEST_UINT_READY. Uses AtapiPacketCommandIn.

    @param DeviceInfo

    @retval EFI_STATUS
**/
EFI_STATUS 
TestUnitReady (
    IN  IDE_RECOVERY_DEVICE_INFO    *DeviceInfo
)
{
    ATAPI_PACKET_COMMAND Packet;
    EFI_STATUS           Status;

    ZeroMem( &Packet, sizeof (ATAPI_PACKET_COMMAND));
    Packet.Cmd.OperationCode_0 = TEST_UNIT_READY;

    Status = AtapiPacketCommandIn( DeviceInfo, &Packet, NULL, 0, TIMEOUT );
    return Status;
}

/**
    Function to send Request sense command.

    @param DeviceInfo
    @param SenseBuffers
    @param SenseCounts

    @retval EFI_STATUS
**/
EFI_STATUS 
RequestSense (
    IN  OUT IDE_RECOVERY_DEVICE_INFO    *DeviceInfo,
    IN  REQUEST_SENSE_DATA              *SenseBuffers,
    IN  OUT UINT8                       *SenseCounts
)
{
    EFI_STATUS           Status = EFI_SUCCESS;
    REQUEST_SENSE_DATA   *Sense = SenseBuffers;
    ATAPI_PACKET_COMMAND Packet;

    ZeroMem( SenseBuffers, sizeof (REQUEST_SENSE_DATA) * (*SenseCounts));

    ZeroMem( &Packet,      sizeof (ATAPI_PACKET_COMMAND));
    Packet.Cmd.OperationCode_0    = REQUEST_SENSE;
    Packet.Cmd.AllocationLength_4 = sizeof (REQUEST_SENSE_DATA);

    *SenseCounts = 0;

    while ( TRUE ) {
        Status = AtapiPacketCommandIn(
            DeviceInfo,
            &Packet,
            (UINT16*) Sense,
            sizeof (REQUEST_SENSE_DATA),
            TIMEOUT
            );

        if ( Status != EFI_SUCCESS ) {
            if ( *SenseCounts == 0 ) {
                return EFI_DEVICE_ERROR;
            } else {
                return EFI_SUCCESS;
            }
        }

        (*SenseCounts)++;

        if ( *SenseCounts == MAX_SENSE_KEY_COUNT ) {
            return EFI_SUCCESS;
        }

        //
        // We limit MAX sense data count to 20 in order to avoid dead loop. 
        // Some incompatible ATAPI devices don't retrive NO_SENSE when 
        // there is no media. In this case, dead loop occurs if we don't 
        // have a gatekeeper. 20 is supposed to be large enough for 
        // any ATAPI device. (Actually 6, not 20).
        //
        if ( Sense->SenseKey_2 != SK_NO_SENSE ) {
            Sense++;
        } else {
            return EFI_SUCCESS;
        }
    }
}

/**
    ATAPI packet command READ_CAPACITY. Uses AtapiPacketCommandIn.

    @param DeviceInfo

    @retval EFI_STATUS
**/
EFI_STATUS 
ReadCapacity (
    IN  OUT IDE_RECOVERY_DEVICE_INFO    *DeviceInfo
)
{
    EFI_STATUS           Status;
    ATAPI_PACKET_COMMAND Packet;
    READ_CAPACITY_DATA   Data;

    ZeroMem( &Data,   sizeof (Data));

    if ( DeviceInfo->MediaInfo.DeviceType == IdeCDROM ) {
        ZeroMem( &Packet, sizeof (ATAPI_PACKET_COMMAND));
        Packet.Cmd.OperationCode_0 = READ_CAPACITY;
        Status                     = AtapiPacketCommandIn(
                                                    DeviceInfo,
                                                    &Packet,
                                                    (UINT16*)(&Data),
                                                    sizeof (READ_CAPACITY_DATA),
                                                    LONGTIMEOUT
                                                    );

        if ( Status == EFI_SUCCESS ) {
            if ( DeviceInfo->MediaInfo.DeviceType == IdeCDROM ) {
                DeviceInfo->MediaInfo.LastBlock    = (Data.LastLba0 << 24) | (Data.LastLba1 << 16) | (Data.LastLba2 << 8) | Data.LastLba3;
                DeviceInfo->MediaInfo.MediaPresent = TRUE;
                DeviceInfo->MediaInfo.BlockSize    = 0x800;
            }
            return EFI_SUCCESS;
        } else {
            return EFI_DEVICE_ERROR;
        }
    } else {
        return EFI_UNSUPPORTED;
    }
}

/**
    Helper function called by Atapi_ReadBlocks.

    @param DeviceInfo
    @param Buffer
    @param StartLba
    @param NumberOfBlocks
    @param BlockSize

    @retval EFI_STATUS
**/
EFI_STATUS 
Atapi_ReadSectors (
    IN  OUT IDE_RECOVERY_DEVICE_INFO    *DeviceInfo,
    IN  VOID                            *Buffer,
    IN  EFI_PEI_LBA                     StartLba,
    IN  UINTN                           NumberOfBlocks,
    IN  UINTN                           BlockSize
)
{
    ATAPI_PACKET_COMMAND Packet;
    READ10_CMD           *Read10Packet   = NULL;
    EFI_STATUS           Status;
    UINTN                BlocksRemaining = 0;
    UINT32               Lba32           = 0;
    UINT32               ByteCount       = 0;
    UINT16               SectorCount     = 0;
    VOID                 *ptrBuffer      = NULL;
    UINT16               MaxBlock        = 0;

    ZeroMem( &Packet, sizeof (ATAPI_PACKET_COMMAND));

    Read10Packet = &Packet.Read10;
    Lba32        = (UINT32) StartLba;
    ptrBuffer    = Buffer;

    //
    // Limit the data bytes that can be transfered by one Read(10) Command
    //
    MaxBlock = (UINT16) ( 0x10000 / BlockSize );
    //
    // (64K bytes)
    //
    BlocksRemaining = NumberOfBlocks;

    Status = EFI_SUCCESS;
    while ( BlocksRemaining > 0 ) {
        if ( BlocksRemaining <= MaxBlock ) {
            SectorCount = (UINT16) BlocksRemaining;
        } else {
            SectorCount = MaxBlock;
        }

        Read10Packet->OperationCode_0 = READ_10;

        //
        // Lba0 ~ Lba3 specify the start logical block address of the data transfer.
        // Lba0 is MSB, Lba3 is LSB
        //
        Read10Packet->Lba3 = (UINT8) ( Lba32 & 0xff );
        Read10Packet->Lba2 = (UINT8) ( Lba32 >> 8 );
        Read10Packet->Lba1 = (UINT8) ( Lba32 >> 16 );
        Read10Packet->Lba0 = (UINT8) ( Lba32 >> 24 );

        //
        // TransferLengthMSB ~ TransferLengthLSB specify the transfer length in block unit.
        //
        Read10Packet->TransferLengthLSB = (UINT8) ( SectorCount & 0xff );
        Read10Packet->TransferLengthMSB = (UINT8) ( SectorCount >> 8 );

        ByteCount = (UINT32) ( SectorCount * BlockSize );

        Status = AtapiPacketCommandIn(
            DeviceInfo,
            &Packet,
            (UINT16*) ptrBuffer,
            ByteCount,
            LONGTIMEOUT
            );

        if ( Status != EFI_SUCCESS ) {
            return Status;
        }

        Lba32           += SectorCount;
        ptrBuffer        = (UINT8*) ptrBuffer + SectorCount * BlockSize;
        BlocksRemaining -= SectorCount;
    }

    return Status;
}

/**
    Does ATA protocol Pio In with data.

    @param IdeIoPortRegisters
    @param Buffer
    @param ByteCount
    @param SectorCount
    @param SectorNumber
    @param CylinderLsb
    @param CylinderMsb
    @param Device
    @param Command

    @retval EFI_STATUS
**/
EFI_STATUS 
AtaPioDataIn (
    IN  IDE_BASE_REGISTERS  *IdeIoPortRegisters,
    OUT VOID                *Buffer,
    IN  UINT32              ByteCount,
    IN  UINT8               SectorCount,
    IN  UINT8               SectorNumber,
    IN  UINT8               CylinderLsb,
    IN  UINT8               CylinderMsb,
    IN  UINT8               Device,
    IN  UINT8               Command
)
{
    EFI_STATUS Status;

    UINT16     HeadReg           = IdeIoPortRegisters->Head;
    UINT16     SectorCountReg    = IdeIoPortRegisters->SectorCount;
    UINT16     SectorNumberReg   = IdeIoPortRegisters->SectorNumber;
    UINT16     CylinderLsbReg    = IdeIoPortRegisters->CylinderLsb;
    UINT16     CylinderMsbReg    = IdeIoPortRegisters->CylinderMsb;
    UINT16     CommandReg        = IdeIoPortRegisters->Reg.Command;
    UINT16     AltStatusRegister = IdeIoPortRegisters->Alt.AltStatus;
    UINT16     StatusReg         = IdeIoPortRegisters->Reg.Status;
    UINT16     DataReg           = IdeIoPortRegisters->Data;

    UINT32     WordCount = 0;
    UINT32     Increment = 0;
    UINT32     Index     = 0;
    UINT16     *Buffer16 = (UINT16*)Buffer;

    IoWrite8( HeadReg, Device );

    Status = WaitForBSYClear( IdeIoPortRegisters, TIMEOUT );

    if ( Status != EFI_SUCCESS ) {
        return EFI_DEVICE_ERROR;
    }

    Status = DRQClear2( IdeIoPortRegisters, TIMEOUT );

    if ( Status != EFI_SUCCESS ) {
        return EFI_DEVICE_ERROR;
    }

    Status = DRQClear2( IdeIoPortRegisters, TIMEOUT );

    if ( Status != EFI_SUCCESS ) {
        return EFI_DEVICE_ERROR;
    }

    IoWrite8( SectorCountReg,  SectorCount );
    IoWrite8( SectorNumberReg, SectorNumber );
    IoWrite8( CylinderLsbReg,  CylinderLsb );
    IoWrite8( CylinderMsbReg,  CylinderMsb );

    IoWrite8( CommandReg,      Command );

    IoRead8( AltStatusRegister );

    Increment = 256;
    WordCount = 0;

    while ( WordCount < ByteCount / 2 ) {
        Status = DRQReady2( IdeIoPortRegisters, COMMAND_COMPLETE_TIMEOUT );

        if ( Status != EFI_SUCCESS ) {
            return Status;
        }

        if ( CheckErrorStatus( StatusReg ) != EFI_SUCCESS ) {
            return EFI_DEVICE_ERROR;
        }

        if ((WordCount + Increment) > ByteCount / 2 ) {
            Increment = ByteCount / 2 - WordCount;
        }

        for ( Index = 0; Index < Increment; Index++ )
        {
            *Buffer16++ = IoRead16( DataReg );
        }

        WordCount += Increment;
    }

    Status = DRQClear( IdeIoPortRegisters, TIMEOUT );

    if ( Status != EFI_SUCCESS ) {
        return CheckErrorStatus( StatusReg );
    }

    return EFI_SUCCESS;
}

#if LBA48_SUPPORT
/**
    Does ATA protocol Pio In with data (48-bit).

    @param IdeIoPortRegisters
    @param Buffer
    @param ByteCount
    @param SectorCount
    @param LbaLow
    @param LbaMid
    @param LbaHigh
    @param Lba48Low
    @param Lba48Mid
    @param Lba49High
    @param Device
    @param Command

    @retval EFI_STATUS
**/
EFI_STATUS 
AtaPioDataIn48 (
    IN IDE_BASE_REGISTERS *IdeIoPortRegisters,
    OUT VOID              *Buffer,
    IN UINT32             ByteCount,
    IN UINT16             SectorCount,
    IN UINT8              LbaLow,
    IN UINT8              LbaMid,
    IN UINT8              LbaHigh,
    IN UINT8              Lba48Low,
    IN UINT8              Lba48Mid,
    IN UINT8              Lba48High,
    IN UINT8              Device,
    IN UINT8              Command 
)
{
    EFI_STATUS Status;
    UINT16     HeadReg           = IdeIoPortRegisters->Head;
    UINT16     SectorCountReg    = IdeIoPortRegisters->SectorCount;
    UINT16     SectorNumberReg   = IdeIoPortRegisters->SectorNumber;
    UINT16     CylinderLsbReg    = IdeIoPortRegisters->CylinderLsb;
    UINT16     CylinderMsbReg    = IdeIoPortRegisters->CylinderMsb;
    UINT16     CommandReg        = IdeIoPortRegisters->Reg.Command;
    UINT16     AltStatusRegister = IdeIoPortRegisters->Alt.AltStatus;
    UINT16     StatusReg         = IdeIoPortRegisters->Reg.Status;
    UINT16     DataReg           = IdeIoPortRegisters->Data;
    UINT32     WordCount = 0;
    UINT32     Increment = 0;
    UINT32     Index     = 0;
    UINT16     *Buffer16 = (UINT16*)Buffer;
    
    IoWrite8( HeadReg, (Device & 0x50) ); // Remove all bits but DRV and LBA

    Status = WaitForBSYClear( IdeIoPortRegisters, TIMEOUT );

    if ( Status != EFI_SUCCESS ) {
        return EFI_DEVICE_ERROR;
    }

    Status = DRQClear2( IdeIoPortRegisters, TIMEOUT );

    if ( Status != EFI_SUCCESS ) {
        return EFI_DEVICE_ERROR;
    }

    Status = DRQClear2( IdeIoPortRegisters, TIMEOUT );

    if ( Status != EFI_SUCCESS ) {
        return EFI_DEVICE_ERROR;
    }

    // Send upper three parts of LBA, then lower three parts
    IoWrite8( SectorCountReg,  (UINT8)(SectorCount >> 8) );
    IoWrite8( SectorNumberReg, Lba48Low );
    IoWrite8( CylinderLsbReg,  Lba48Mid );
    IoWrite8( CylinderMsbReg,  Lba48High );
    IoWrite8( SectorCountReg,  (UINT8) (SectorCount & 0xFF) );
    IoWrite8( SectorNumberReg, LbaLow );
    IoWrite8( CylinderLsbReg,  LbaMid );
    IoWrite8( CylinderMsbReg,  LbaHigh );

    IoWrite8( CommandReg,      Command );

    IoRead8( AltStatusRegister );

    Increment = 256;
    WordCount = 0;

    while ( WordCount < ByteCount / 2 ) {
        Status = DRQReady2( IdeIoPortRegisters, LONGTIMEOUT );

        if ( Status != EFI_SUCCESS ) {
            return Status;
        }

        if ( CheckErrorStatus( StatusReg ) != EFI_SUCCESS ) {
            return EFI_DEVICE_ERROR;
        }

        if ((WordCount + Increment) > ByteCount / 2 ) {
            Increment = ByteCount / 2 - WordCount;
        }

        for ( Index = 0; Index < Increment; Index++ )
        {
            *Buffer16++ = IoRead16( DataReg );
        }

        WordCount += Increment;
    }

    Status = DRQClear( IdeIoPortRegisters, TIMEOUT );

    if ( Status != EFI_SUCCESS ) {
        return CheckErrorStatus( StatusReg );
    }

    return EFI_SUCCESS;
}
#endif

/**
    ATA Identify command. Uses AtaPioDataIn

    @param DeviceInfo
    @param AtaIdentifyData

    @retval EFI_STATUS
**/
EFI_STATUS 
AtaIdentify (
    IN  IDE_RECOVERY_DEVICE_INFO    *DeviceInfo,
    OUT ATA_IDENTIFY_DATA           *AtaIdentifyData
)
{
    return       AtaPioDataIn( DeviceInfo->IdeIoPortRegisters,
                               AtaIdentifyData,
                               sizeof (ATA_IDENTIFY_DATA),
                               0,
                               0,
                               0,
                               0,
                               DeviceInfo->Device << 4,
                               ATA_IDENTIFY_DEVICE_CMD
                               );
}

/**
    Helper function called by Ata_ReadBlocks.

    @param DeviceInfo
    @param Buffer
    @param StartLba
    @param NumberOfBlocks
    @param BlockSize

    @retval EFI_STATUS
**/
EFI_STATUS 
Ata_ReadSectors (
    IN  IDE_RECOVERY_DEVICE_INFO    *DeviceInfo,
    IN  OUT VOID                    *Buffer,
    IN  EFI_PEI_LBA                 StartLba,
    IN  UINTN                       NumberOfBlocks,
    IN  UINTN                       BlockSize
)
{
    EFI_STATUS Status;
    UINTN      BlocksRemaining = 0;
    UINT32     Lba32           = 0;
    UINT8      LbaLow          = 0;
    UINT8      LbaMid          = 0;
    UINT8      LbaHigh         = 0;
    UINT32     ByteCount       = 0;
    UINT32     SectorCount     = 0;
    VOID       *ptrBuffer      = NULL;
    UINT32     MaxBlock        = 0;
    UINT8      Dev             = 0;
#if LBA48_SUPPORT
    UINT32     Lba32Hi         = 0;
    UINT64     Lba48           = 0;
    UINT8      Lba48Low        = 0;
    UINT8      Lba48Mid        = 0;
    UINT8      Lba48High       = 0;
#endif

    Lba32     = (UINT32) StartLba;
    ptrBuffer = Buffer;

//    MaxBlock = (UINT16) ( 0x20000 / BlockSize );
    MaxBlock = (UINT32) MAX_SECTOR_COUNT_PIO;

    BlocksRemaining = NumberOfBlocks;

    Status = EFI_SUCCESS;
    Dev    = (DeviceInfo->Device << 4) | LBA_MODE;

#if LBA48_SUPPORT
    if( DeviceInfo->Lba48Bit ) {
        MaxBlock = (UINT32) MAX_SECTOR_COUNT_PIO_48BIT;
        Lba48 = StartLba;
        while ( BlocksRemaining > 0 ) {
            
            if ( BlocksRemaining < MaxBlock ) {
                SectorCount = BlocksRemaining;
            } else {
                SectorCount = MaxBlock;
            }
            
            Lba32Hi = (UINT32) Shr64 ( Lba48, 32 );
            LbaLow  = (UINT8) ( Lba32 & 0xff );
            LbaMid  = (UINT8) ( Lba32 >> 8 );
            LbaHigh = (UINT8) ( Lba32 >> 16 );
            Lba48Low = (UINT8) ( Lba32 >> 24 );
            Lba48Mid = (UINT8) ( Lba32Hi & 0xff );
            Lba48High = (UINT8) ( Lba32Hi >> 8 );

            ByteCount = (UINT32) ( SectorCount * BlockSize );

            Status = AtaPioDataIn48( DeviceInfo->IdeIoPortRegisters,
                                   ptrBuffer,
                                   ByteCount,
                                   (UINT16) SectorCount, 
                                   LbaLow, 
                                   LbaMid, 
                                   LbaHigh, 
                                   Lba48Low,
                                   Lba48Mid,
                                   Lba48High,
                                   Dev,
                                   ATA_READ_SECTOR_EXT_CMD
                                   );

            if ( Status != EFI_SUCCESS ) {
                return Status;
            }

            Lba48            += SectorCount;
            Lba32            = (UINT32) Lba48;
            ptrBuffer        = (UINT8*) ptrBuffer + SectorCount * BlockSize;
            BlocksRemaining  -= SectorCount;
        }

    } else {
#endif
        while ( BlocksRemaining > 0 ) {
            if ( BlocksRemaining < MaxBlock ) {
                SectorCount = BlocksRemaining;
            } else {
                SectorCount = MaxBlock;
            }
            LbaLow  = (UINT8) ( Lba32 & 0xff );
            LbaMid  = (UINT8) ( Lba32 >> 8 );
            LbaHigh = (UINT8) ( Lba32 >> 16 );

            ByteCount = (UINT32) ( SectorCount * BlockSize );

            Dev   |= (Lba32 >> 24);
            Status = AtaPioDataIn( DeviceInfo->IdeIoPortRegisters,
                                   ptrBuffer,
                                   ByteCount,
                                   (UINT8) SectorCount, // Sector count
                                   LbaLow,              // Sector number
                                   LbaMid,              // LSB
                                   LbaHigh,             // MSB
                                   Dev,
                                   ATA_READ_SECTOR_CMD
                                   );

            if ( Status != EFI_SUCCESS ) {
                return Status;
            }

            Lba32           += SectorCount;
            ptrBuffer        = (UINT8*) ptrBuffer + SectorCount * BlockSize;
            BlocksRemaining -= SectorCount;
        }
#if LBA48_SUPPORT
    }
#endif
    return Status;
}

/**
    Helper functin used by WaitForBSYClear, DRQClear, DRQClear2.

    @param Register
    @param Bits
    @param TimeoutInMilliSeconds

    @retval EFI_STATUS
**/
EFI_STATUS 
WaitForBitsToClear (
    UINT16 Register,
    UINT8  Bits,
    UINTN  TimeoutInMilliSeconds
)
{
    UINTN Delay       = ((TimeoutInMilliSeconds * STALL_1_MILLI_SECOND) / 250) + 1;
    UINT8 StatusValue = 0;

    do {
        StatusValue = IoRead8( Register );

        if ((StatusValue & Bits) == 0x00 ) {
            break;
        }

        gStallPpi->Stall( gPeiServices, gStallPpi, 250 );

        Delay--;
    } while ( Delay );

    if ( Delay == 0 ) {
        return EFI_TIMEOUT;
    }

    return EFI_SUCCESS;
}

/**
    Wait for busy bit to clear

    @param IdeIoRegisters
    @param TimeoutInMilliSeconds

    @retval EFI_STATUS
**/
EFI_STATUS 
WaitForBSYClear (
    IN  IDE_BASE_REGISTERS  *IdeIoRegisters,
    IN  UINTN               TimeoutInMilliSeconds
)
{
    return WaitForBitsToClear( IdeIoRegisters->Reg.Status, 
                               BSY, 
                               TimeoutInMilliSeconds );
}

/**
    Wait for the DRQ bit clear

    @param IdeIoRegisters
    @param TimeoutInMilliSeconds

    @retval EFI_STATUS
**/
EFI_STATUS 
DRQClear (
    IN  IDE_BASE_REGISTERS  *IdeIoRegisters,
    IN  UINTN               TimeoutInMilliSeconds
)
{
    return WaitForBitsToClear( IdeIoRegisters->Reg.Status, 
                               DRQ | BSY, 
                               TimeoutInMilliSeconds );
}

/**
    Wait for the DRQ bit clear in Alternate Status Reg

    @param IdeIoRegisters
    @param TimeoutInMilliSeconds

    @retval EFI_STATUS
**/
EFI_STATUS 
DRQClear2 (
    IN  IDE_BASE_REGISTERS  *IdeIoRegisters,
    IN  UINTN               TimeoutInMilliSeconds
)
{
    return WaitForBitsToClear( IdeIoRegisters->Alt.AltStatus, 
                               DRQ | BSY, 
                               TimeoutInMilliSeconds );
}

/**
    Helper function called by DRQReady.

    @param StatusRegister
    @param ErrorRegister
    @param TimeoutInMilliSeconds

    @retval EFI_STATUS
**/
EFI_STATUS 
DRQReadyHelper (
    UINT16      StatusRegister,
    UINT16      ErrorRegister,
    IN  UINTN   TimeoutInMilliSeconds
)
{
    UINTN Delay       = ((TimeoutInMilliSeconds * STALL_1_MILLI_SECOND) / 250) + 1;
    UINT8 StatusValue = 0;
    UINT8 ErrValue    = 0;

    do
    {
        //
        //  Read Status Register will clear interrupt
        //
        StatusValue =  IoRead8( StatusRegister );

        //
        //  BSY==0,DRQ==1
        //
        if ((StatusValue & (BSY | DRQ)) == DRQ ) {
            break;
        }

        if ((StatusValue & (BSY | ERR)) == ERR ) {
            ErrValue = IoRead8( ErrorRegister );

            if ((ErrValue & ABRT_ERR) == ABRT_ERR ) {
                return EFI_ABORTED;
            }
        }

        gStallPpi->Stall( gPeiServices, gStallPpi, 250 );

        Delay--;
    } while ( Delay );

    if ( Delay == 0 ) {
        return EFI_TIMEOUT;
    }

    return EFI_SUCCESS;
}

/**
    Wait for the DRQ ready.

    @param IdeIoRegisters
    @param TimeoutInMilliSeconds

    @retval EFI_STATUS
**/
EFI_STATUS 
DRQReady (
    IN  IDE_BASE_REGISTERS  *IdeIoRegisters,
    IN  UINTN               TimeoutInMilliSeconds
)
{
    return DRQReadyHelper( IdeIoRegisters->Reg.Status, 
                           IdeIoRegisters->Reg1.Error, 
                           TimeoutInMilliSeconds );
}

/**
    Wait for the DRQ Ready in Alternate Status Reg

    @param IdeIoRegisters
    @param TimeoutInMilliSeconds

    @retval EFI_STATUS
**/
EFI_STATUS 
DRQReady2 (
    IN  IDE_BASE_REGISTERS  *IdeIoRegisters,
    IN  UINTN               TimeoutInMilliSeconds
)
{
    return DRQReadyHelper( IdeIoRegisters->Alt.AltStatus, 
                           IdeIoRegisters->Reg1.Error, 
                           TimeoutInMilliSeconds );
}

/**
    Check for the Errros.

    @param StatusReg

    @retval EFI_STATUS
**/
EFI_STATUS 
CheckErrorStatus (
    IN  UINT16  StatusReg
)
{
    UINT8 StatusValue = IoRead8( StatusReg );

    if ((StatusValue & (ERR | DWF | CORR)) == 0 ) {
        return EFI_SUCCESS;
    }

    return EFI_DEVICE_ERROR;
}

/**
    Clear the Memory.

    @param Buffer
    @param Size

    @retval VOID
**/
static VOID 
ZeroMem (
    IN  VOID    *Buffer,
    IN  UINTN   Size
)
{
    UINT8 *ptr;

    ptr = Buffer;
    while ( Size-- )
    {
        *(ptr++) = 0;
    }
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
