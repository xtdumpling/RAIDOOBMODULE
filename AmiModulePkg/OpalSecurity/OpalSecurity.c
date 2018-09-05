//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file OpalSecurity.c
    Opal Security Support

**/
//---------------------------------------------------------------------------

#include "OpalSecurity.h"

//---------------------------------------------------------------------------

AMI_HDD_OPAL_SEC_INIT_PROTOCOL        *gOpalSecInitProtocol;
AMI_STORAGE_SECURITY_COMMAND_PROTOCOL *gStorageSecurityProtocol;

/**
    This function is the entry point for OpalSecurity 
    Driver.

    @param  EFI_HANDLE          ImageHandle     Image handle.
    @param  EFI_SYSTEM_TABLE    *SystemTable    Pointer to the EFI system table.

    @retval EFI_SUCCESS : OpalSecurity Interface successfully installed.

**/

EFI_STATUS 
EFIAPI OpalSecEntryPoint (
    IN EFI_HANDLE          ImageHandle,
    IN EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_HANDLE    Handle = NULL;
    EFI_STATUS    Status ;

    InitAmiLib( ImageHandle, SystemTable );

    Status = pBS->AllocatePool( EfiBootServicesData,
                                sizeof(AMI_HDD_OPAL_SEC_INIT_PROTOCOL),
                                (VOID**)&gOpalSecInitProtocol );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

    gOpalSecInitProtocol->InstallOpalSecurityInterface    = InstallOpalSecurityInterface;
    gOpalSecInitProtocol->UnInstallOpalSecurityInterface  = UnInstallOpalSecurityInterface;

    Status = pBS->InstallProtocolInterface(
                                        &Handle,
                                        &gAmiHddOpalSecInitProtocolGuid,
                                        EFI_NATIVE_INTERFACE,
                                        gOpalSecInitProtocol
                                        );

    return Status;
}

/**
    Installs StorageSecurityProtocol if the drive supports 
    Trusted commands

        
    @param BusInterface 
    @param BusInterfaceType 

    @retval EFI_SUCCESS    : StorageSecurity supported.
    @retval Others         : Not supported.

**/

EFI_STATUS 
InstallOpalSecurityInterface (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
)
{
    EFI_STATUS  Status ;
    IDENTIFY_DATA   IdentifyData = { 0 };
    EFI_HANDLE  IdeDeviceHandle = NULL;
    
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL            *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
    
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE           *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif
   
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        
        IdentifyData    = IdeBusInterface->IdeDevice.IdentifyData;
        IdeDeviceHandle = IdeBusInterface->IdeDeviceHandle;
    } 
#endif
    
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        
        IdentifyData    = SataDevInterface->IdentifyData;
        IdeDeviceHandle = SataDevInterface->IdeDeviceHandle;
    }
#endif

    if (!(IdentifyData.Trusted_Computing_Support & 0x01)) {
        return EFI_UNSUPPORTED;
    }

    // Return error if gEfiStorageSecurityCommandProtocolGuid is already installed on the Device Handle
    Status = pBS->OpenProtocol ( IdeDeviceHandle, 
                                &gEfiStorageSecurityCommandProtocolGuid, 
                                NULL, 
                                NULL, 
                                NULL, 
                                EFI_OPEN_PROTOCOL_TEST_PROTOCOL );
    if(!EFI_ERROR(Status)) {
        return EFI_ALREADY_STARTED;
    }

    //
    // StorageSecurity Protocol
    //
    Status = pBS->AllocatePool( EfiBootServicesData,
                           sizeof(AMI_STORAGE_SECURITY_COMMAND_PROTOCOL),
                           (VOID**)&gStorageSecurityProtocol );
    if(EFI_ERROR(Status)){
        return Status;
    }
    
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        gStorageSecurityProtocol->ReceiveData = IdeReceiveData;
        gStorageSecurityProtocol->SendData    = IdeSendData;
        gStorageSecurityProtocol->BusInterface = IdeBusInterface;
        IdeBusInterface->StorageSecurityInterface = (EFI_STORAGE_SECURITY_COMMAND_PROTOCOL*)gStorageSecurityProtocol;
    } 
#endif
    
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        gStorageSecurityProtocol->ReceiveData = AhciReceiveData;
        gStorageSecurityProtocol->SendData    = AhciSendData;
        gStorageSecurityProtocol->BusInterface = SataDevInterface;
        SataDevInterface->StorageSecurityInterface =(EFI_STORAGE_SECURITY_COMMAND_PROTOCOL*)gStorageSecurityProtocol;
    }
#endif

    Status = pBS->InstallMultipleProtocolInterfaces(
                      &IdeDeviceHandle,
                      &gEfiStorageSecurityCommandProtocolGuid, gStorageSecurityProtocol,
                      NULL );
    //
    // TPer Lock On Reset Feature Support
    //
#if TPer_LOCK_ON_RESET

    //
    // Below error status check is to determine if gStorageSecurityProtocol has been
    // installed successfully or not. In case if code inserted in between, this check
    // has to be handled properly.
    //

    if (!EFI_ERROR(Status)) { 
        GetAndStoreSecurityProtocolInformation(
            (EFI_STORAGE_SECURITY_COMMAND_PROTOCOL*)gStorageSecurityProtocol,
            BusInterface
            );
        IssueLockOnResetCommand(
            (EFI_STORAGE_SECURITY_COMMAND_PROTOCOL*)gStorageSecurityProtocol, 
            BusInterface, 
            BusInterfaceType
            );
    }
#endif
    
    return Status;
}

/**
    Uninstall the Opal Security Init Protocol.

    @param BusInterface 
    @param BusInterfaceType 

    @retval EFI_SUCCESS                      : Security Mode supported.
    @retval EFI_DEVICE_ERROR/EFI_UNSUPPORTED : Not supported.

**/

EFI_STATUS 
UnInstallOpalSecurityInterface (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
)
{
    EFI_STATUS  Status;
    EFI_HANDLE  IdeDeviceHandle = NULL;
    AMI_STORAGE_SECURITY_COMMAND_PROTOCOL    *StorageSecurityInterface = NULL;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL            *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
    
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE           *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        IdeDeviceHandle = IdeBusInterface->IdeDeviceHandle;
        StorageSecurityInterface = (AMI_STORAGE_SECURITY_COMMAND_PROTOCOL*)IdeBusInterface->StorageSecurityInterface;
    } 
#endif
    
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        IdeDeviceHandle = SataDevInterface->IdeDeviceHandle;
        StorageSecurityInterface = (AMI_STORAGE_SECURITY_COMMAND_PROTOCOL*)SataDevInterface->StorageSecurityInterface;
    }
#endif

    //
    //  UnInstalls StorageSecurityProtocol on the DeviceHandle
    //
    Status = pBS->UninstallMultipleProtocolInterfaces(
                                     IdeDeviceHandle,
                                     &gEfiStorageSecurityCommandProtocolGuid,
                                     StorageSecurityInterface,
                                     NULL 
                                     );
    
    if ( Status == EFI_SUCCESS )
        {
            pBS->FreePool( StorageSecurityInterface );
            StorageSecurityInterface = NULL;
        }
    return Status;
}

/**
    Get and store Security Protocol Information
    
    @param  This 
    @param  BusInterface 
    
    @retval VOID

**/
EFI_STATUS 
GetAndStoreSecurityProtocolInformation (
    IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
    IN VOID    *BusInterface
)
{
    EFI_STATUS  Status ;
    SP0_TRUSTED_RECEIVE_PARAMETER_DATA  *QueryBuffer = NULL;
     
    UINTN   QueryBufferSize = sizeof(SP0_TRUSTED_RECEIVE_PARAMETER_DATA);

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(UINT8)*QueryBufferSize, (VOID**)&QueryBuffer);
    if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
    }

    Status = GetSecurityProtocolInformation(This, QueryBuffer, &QueryBufferSize);
    //
    // GetSecurityProtocolInformation returns error Status. Unsupported Security Protocol.
    //

    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

   
    ((AMI_STORAGE_SECURITY_COMMAND_PROTOCOL*)This)->SupportedProtocolList = QueryBuffer;

    return EFI_SUCCESS;
}

/**
    Get Security Protocol Information

    @param This 
    @param QueryBuffer 
    @param QueryBufferSize 
      

    @retval VOID

**/
EFI_STATUS 
GetSecurityProtocolInformation (
    IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
    IN OUT VOID *QueryBuffer,
    IN OUT UINTN    *QueryBufferSize 
)
{
    EFI_STATUS  Status;

    Status = This->ReceiveData( This,
                        0,
                        0,
                        SECURITY_PROTOCOL_INFORMATION,   // Security Protocol ID
                        0x00,   // SP specific id
                        0x200,  // TranferLength
                        QueryBuffer,
                        QueryBufferSize
                        );
    return Status;
    
}

/**
    Issues LockOnReset Command.

    @param This
    @param BusInterface 
    @param BusInterfaceType 

    @retval VOID

**/
EFI_STATUS 
IssueLockOnResetCommand (
    IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
    IN VOID                                     *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE                BusInterfaceType
)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    IDENTIFY_DATA   IdentifyData ={0};
   
    UINT16    ListLength = 0;
    UINT8     SpByte = 0;
    SP0_TRUSTED_RECEIVE_PARAMETER_DATA    *SupportedProtocolList = ((AMI_STORAGE_SECURITY_COMMAND_PROTOCOL*)This)->SupportedProtocolList;

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        IdentifyData    = ((AMI_IDE_BUS_PROTOCOL*)BusInterface)->IdeDevice.IdentifyData;
    } 
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )   
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        IdentifyData    = ((SATA_DEVICE_INTERFACE*)BusInterface)->IdentifyData;
    }
#endif

    //
    // The List Length field indicates the total length, in bytes, of the supported security protocol list.
    //
    ListLength = SupportedProtocolList->ListLengthHigh << 8 | SupportedProtocolList->ListLengthLow;
    if (ListLength == 0) {
        return EFI_UNSUPPORTED;
    }

    //
    // Check the security supported Protocol List is 0xEE for IEEE 1667 or 0x02 for Native TCG
    //
    for (SpByte = 0; SpByte < ListLength; SpByte++) {
        if (SupportedProtocolList->SupportedSPList[SpByte] == SECURITY_PROTOCOL_2) {
            //
            // Found Supported Security Protocol list NativeTcg.Issue TCG TPer Reset
            //
            Status = NativeTcgTPerReset(This);
            break; 
        }

        if (SupportedProtocolList->SupportedSPList[SpByte] == SECURITY_PROTOCOL_IEEE1667) {
            //
            // Found Supported Security Protocol list IEEE 1667. Issue Silo TPer Reset
            // Check if IEEE 1667 bit (bit 7) set in the AdditionalSupported field to 
            // determine whether the device supports Trusted Send/Receive.
            //
            if (!(IdentifyData.Reserved_69_74[0] & 0x80)) {
                Status = IEEE67TPerReset(This);
                ASSERT_EFI_ERROR(Status);
                break; 
            }
        } 
    }

    return EFI_SUCCESS;
}

/**
    Native TCG TPer Reset

    @param This 

    @retval VOID

**/

EFI_STATUS 
NativeTcgTPerReset (
    IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This
)
{
    EFI_STATUS  Status;
    VOID    *TcgResetDataBuffer = NULL;
    UINT32  BufferSize = 0x200;

    Status = pBS->AllocatePool(EfiBootServicesData, BufferSize, (VOID**)&TcgResetDataBuffer);
    if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
    }

    pBS->SetMem(TcgResetDataBuffer, BufferSize, 0);

    //
    // Determined the device supports Native TCG Security Support Protocol 2. Issue TPer Reset.
    //
    Status = This->SendData( This,
                      0,
                      0,
                      SECURITY_PROTOCOL_2,   // Security Protocol 2 ID
                      0x0400, // SP specific id
                      0x200,  // TranferLength - Non zero Value
                      TcgResetDataBuffer
                      );
    pBS->FreePool(TcgResetDataBuffer);

    return Status;
}

/**
    IEEE67 TPer Reset 

    @param This 
      
    @retval VOID

**/

EFI_STATUS 
IEEE67TPerReset (
    IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This
)
{
    EFI_STATUS  Status ;
    UINT16  SiloTypeIdentifier = 0;
    UINT16  IeeeSilo = 0;
    TCG_SILO_TYPE_IDENTIFIER    *ProbeHostDataBuffer = NULL;
    TCG_SILO_TYPE_IDENTIFIER    *SupportedSilosArrayBuffer = NULL;

    UINTN    SupportedSilosArrayBufferSize = sizeof(TCG_SILO_TYPE_IDENTIFIER);
    UINTN    ProbeHostDataBufferSize = sizeof(TCG_SILO_TYPE_IDENTIFIER);
        
    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(UINT8)*ProbeHostDataBufferSize, (VOID**)&ProbeHostDataBuffer);
    if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Check if TPer has TCG silo by sending probe command to PROBE SILO
    //
    Status = This->SendData( This,
                          0,
                          0,
                          SECURITY_PROTOCOL_IEEE1667,   // Security Protocol ID
                          0x0100, // SP specific id
                          0x200,  // TranferLength
                          ProbeHostDataBuffer
                         );
    if (!EFI_ERROR(Status)) {
        Status = pBS->AllocatePool(EfiBootServicesData, sizeof(UINT8)*SupportedSilosArrayBufferSize, (VOID**)&SupportedSilosArrayBuffer);
        if (EFI_ERROR(Status)) {
            return EFI_OUT_OF_RESOURCES;
        }

        //
        // Followed by Trusted Receive Probe command to PROBE SILO
        //
        Status = This->ReceiveData( This,
                            0,
                            0,
                            SECURITY_PROTOCOL_IEEE1667,   // Security Protocol ID
                            0x0100, // SP specific id
                            0x200,  // TranferLength
                            SupportedSilosArrayBuffer,
                            &SupportedSilosArrayBufferSize
                            );

        if (!EFI_ERROR(Status)) {
             //
             // Scan for Silo Identifier(0x0104) available to send TPer Reset silo command payload to TCG Silo
             //
             SiloTypeIdentifier = SupportedSilosArrayBuffer->SiloTypeIdentifierHigh << 8 | SupportedSilosArrayBuffer->SiloTypeIdentifierLow;
             //
             // Search for IEEE std allocated identifiers only to find  0x0104 present: IEEE  - 0100h to 01FFh
             //
             for (IeeeSilo = 0x100; IeeeSilo < 0x200; IeeeSilo++) {
                 if (IeeeSilo == SiloTypeIdentifier) {
                     Status = IssueTPerResetTcgSilo(This);
                     return Status;
                 }
             }
        }
    }
 
    return Status;
}

/**
    Issue TPer Reset

    @param This 

    @retval VOID

**/

EFI_STATUS 
IssueTPerResetTcgSilo (
    IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This
)
{
    EFI_STATUS    Status;
    TCG_SILO_TYPE_IDENTIFIER    *IeeeResetDataBuffer = NULL;

    UINTN    IeeeResetDataBufferSize = sizeof(TCG_SILO_TYPE_IDENTIFIER);

    Status = pBS->AllocatePool(EfiBootServicesData, sizeof(UINT8)*IeeeResetDataBufferSize, (VOID**)&IeeeResetDataBuffer);
    if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
    }

    //
    // Determined the device supports TCG silo. Issue TPer Reset silo command payload to the TCG silo
    //
    Status = This->SendData( This,
                         0,
                         0,
                         SECURITY_PROTOCOL_IEEE1667,   // Security Protocol ID
                         0x0500, // SP specific id
                         0x200,  // TranferLength
                         IeeeResetDataBuffer
                         );
    pBS->FreePool(IeeeResetDataBuffer);

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

