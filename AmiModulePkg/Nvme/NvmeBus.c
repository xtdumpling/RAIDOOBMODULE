//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//      Bug fixed:  Support Legacy NVMe and controlled by item.
//      Reason:   
//      Auditor:    Jimmy Chiu (Refer Grantley - Kasber Chen)
//      Date:       Jan/12/2017
//
//  Rev. 1.00
//      Bug Fixed:  Skip AMI NVMe driver handle if NVMe driver is in device.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/21/2016
//
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

/** @file NvmeBus.c
    Provides Nvme Block IO protocol

**/

//---------------------------------------------------------------------------

#include "NvmeIncludes.h"
#include "NvmeBus.h"
#include "NvmePassthru.h"
#include "Token.h"	//SMCPKG_SUPPORT
#include "Protocol/AmiHddSecurityInit.h"
#include "Protocol/AmiHddSecurity.h"
#include "Protocol/AmiHddSmartInit.h"
#include "Protocol/AmiHddSmart.h"
#include <Guid/AmiResetSystemEvent.h>
#include <Setup.h> //SMCPKG_SUPPORT

//---------------------------------------------------------------------------

#ifndef EFI_COMPONENT_NAME2_PROTOCOL_GUID
EFI_GUID gComponentNameProtocolGuid = EFI_COMPONENT_NAME_PROTOCOL_GUID;
#else
EFI_GUID gComponentNameProtocolGuid = EFI_COMPONENT_NAME2_PROTOCOL_GUID;
#endif

#if (EFI_SPECIFICATION_VERSION > 0x00020000)
extern EFI_COMPONENT_NAME2_PROTOCOL gNvmeBusControllerDriverName;
#else
extern EFI_COMPONENT_NAME_PROTOCOL gNvmeBusControllerDriverName;
#endif

// Once gEfiNvmExpressPassThruProtocolGuid is added in MdePkg, we have to remove
// the below gEfiNvmExpressPassThruProtocolGuid initialization
EFI_GUID gEfiNvmExpressPassThruProtocolGuid = EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL_GUID;

// Below code will be removed when gAmiTcgStorageSecurityInitProtocolGuid definition is added in AmiModulePkg
#ifndef AMI_TCG_STORAGE_SECURITY_INIT_PROTOCOL_GUID 
#define AMI_TCG_STORAGE_SECURITY_INIT_PROTOCOL_GUID \
        { 0x734aa01d, 0x95ec, 0x45b7, { 0xa2, 0x3a, 0x2d, 0x86, 0xd8, 0xfd, 0xeb, 0xb6 } }
#endif

// Below code will be removed when gAmiTcgStorageSecurityProtocolGuid definition is added in AmiModulePkg
#ifndef AMI_TCG_STORAGE_SECURITY_PROTOCOL_GUID 
#define AMI_TCG_STORAGE_SECURITY_PROTOCOL_GUID \
        { 0xb396da3a, 0x52b2, 0x4cd6, { 0xa8, 0x9a, 0x13, 0xe7, 0xc4, 0xae, 0x97, 0x90 } }
#endif

EFI_GUID gAmiTcgStorageSecurityInitProtocolGuid = AMI_TCG_STORAGE_SECURITY_INIT_PROTOCOL_GUID;
EFI_GUID gAmiTcgStorageSecurityProtocolGuid = AMI_TCG_STORAGE_SECURITY_PROTOCOL_GUID;

AMI_HDD_SECURITY_INIT_PROTOCOL      *gNvmeHddSecurityInitProtocol = NULL;
AMI_HDD_SECURITY_INIT_PROTOCOL      *gAmiTcgStorageSecurityInitProtocol = NULL;

EFI_EVENT                              NvmeShutdownEvent = NULL;
EFI_EVENT                              NvmeReadyToBootEvent = NULL;
AMI_HDD_SMART_INIT_PROTOCOL            *HddSmartInitProtocol = NULL;
AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL  *AmiBlkWriteProtection = NULL;

// Used by NVMe legacy boot support
UINT8       gInt13DeviceAddress = 1;
VOID        *gLegacyNvmeBuffer = NULL;


EFI_DRIVER_BINDING_PROTOCOL gNvmeBusDriverBinding = {
    NvmeBusSupported, 
    NvmeBusStart,     
    NvmeBusStop,      
    NVME_BUS_DRIVER_VERSION,  // version
    NULL,                    // ImageHandle
    NULL                     // DriverBindingHandle
};

/**
    Installs gNvmeBusDriverBinding protocol

        
    @param ImageHandle 
    @param SystemTable 

    @retval 
        EFI_STATUS

    @note  
  Here is the control flow of this function:
  1. Initialize Ami Lib.
  2. Install Driver Binding Protocol
  3. Return EFI_SUCCESS.

**/

EFI_STATUS
EFIAPI
NvmeBusEntryPoint (
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS  Status;
#if SMCPKG_SUPPORT
#if NVMEINT13_SUPPORT
    EFI_GUID	SetupGuid = SETUP_GUID;
    UINTN	VarSize = sizeof(SETUP_DATA);
    SETUP_DATA	SetupData;
        
    Status = SystemTable->RuntimeServices->GetVariable(
    		    L"Setup",
    		    &SetupGuid,
    		    NULL,
    		    &VarSize,
    		    &SetupData);
        
    if(!Status && !SetupData.NvmeFWSource)	return Status;
#endif
#endif
   
    gNvmeBusDriverBinding.DriverBindingHandle=NULL;
    gNvmeBusDriverBinding.ImageHandle=ImageHandle;

    Status = gBS->InstallMultipleProtocolInterfaces(
                    &gNvmeBusDriverBinding.DriverBindingHandle,
                    &gEfiDriverBindingProtocolGuid,&gNvmeBusDriverBinding,
                    &gEfiComponentName2ProtocolGuid, &gNvmeBusControllerDriverName,
                    NULL
                    );

    return Status;
    
}

/**
    Checks whether it is a Nvme controller or not. 
    If 'yes', return SUCCESS else ERROR

    @param This 
    @param Controller 
    @param RemainingDevicePath 

    @retval EFI_STATUS

**/

EFI_STATUS 
EFIAPI
NvmeBusSupported (
    IN  EFI_DRIVER_BINDING_PROTOCOL *This,
    IN  EFI_HANDLE                  Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
)
{

    EFI_STATUS                  Status;
    EFI_PCI_IO_PROTOCOL         *PciIO;
    PCI_TYPE00                  PciConfig;
//APTIOV_SERVER_OVERRIDE_START
    EFI_DEVICE_PATH_PROTOCOL    *ParentDevicePath;

    
    // Check whether DevicePath Protocol has been installed on this controller
    Status = gBS->OpenProtocol( Controller,
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&ParentDevicePath,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_BY_DRIVER );

    if (EFI_ERROR (Status)) {
        return Status;
    }

    gBS->CloseProtocol (
                    Controller,
                    &gEfiDevicePathProtocolGuid,
                    This->DriverBindingHandle,
                    Controller);
//APTIOV_SERVER_OVERRIDE_END
    // Check for Valid SATA Device Path. If no return UNSUPPORTED
    // Check if Controller is Nvme or not?

    Status = gBS->OpenProtocol( Controller,
                        &gEfiPciIoProtocolGuid,
                        (VOID **)&PciIO,
                        This->DriverBindingHandle,     
                        Controller,   
						//APTIOV_SERVER_OVERRIDE_START
						//EFI_OPEN_PROTOCOL_GET_PROTOCOL);
                        EFI_OPEN_PROTOCOL_BY_DRIVER);
						//APTIOV_SERVER_OVERRIDE_END
    
     if (EFI_ERROR(Status)) {
//APTIOV_SERVER_OVERRIDE_START
         return Status;
//APTIOV_SERVER_OVERRIDE_END
     }
//APTIOV_SERVER_OVERRIDE_START
     gBS->CloseProtocol( Controller,
    		 	 	 	 &gEfiPciIoProtocolGuid,
    		 	 	 	 This->DriverBindingHandle,
    		 	 	 	 Controller );
//APTIOV_SERVER_OVERRIDE_END

     Status = PciIO->Pci.Read ( PciIO,
                                EfiPciIoWidthUint8,
                                0,
                                sizeof (PCI_TYPE00),
                                &PciConfig
                                );
     
     if (EFI_ERROR(Status)) {
         return EFI_UNSUPPORTED;
     }

     // Check for MASS Storage controller, Non-Volatile and NVMHCI interface
     if (IS_CLASS3 (&PciConfig, 
                 PCI_CLASS_MASS_STORAGE, 
                 PCI_CLASS_MASS_STORAGE_SOLID_STATE, 
                 PCI_IF_MASS_STORAGE_SOLID_STATE_ENTERPRISE_NVMHCI)) {
#if SMCPKG_SUPPORT && NVMEINT13_SUPPORT == 0
         if(!(PciIO->RomSize))
             return EFI_SUCCESS;
         else
             return EFI_UNSUPPORTED;
#else
         return EFI_SUCCESS;
#endif
     }
     
    return EFI_UNSUPPORTED; 
}

/**
    Installs Nvme Block IO Protocol

        
    @param This 
    @param Controller 
    @param RemainingDevicePath 

    @retval 
        EFI_STATUS

**/

EFI_STATUS 
EFIAPI
NvmeBusStart (
    IN  EFI_DRIVER_BINDING_PROTOCOL *This,
    IN  EFI_HANDLE                  Controller,
    IN  EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
)
{

    EFI_STATUS                         Status;
    EFI_PCI_IO_PROTOCOL                *PciIO;
    AMI_NVME_CONTROLLER_PROTOCOL       *NvmeController = NULL;
    IDENTIFY_CONTROLLER_DATA           *IdentifyData = NULL;
    UINT32                             *ActiveNameSpaceIDs = NULL;
    UINT32                             i;
    AMI_NVME_PASS_THRU_PROTOCOL        *AmiNvmePassThru;
    NVM_EXPRESS_PASS_THRU_INSTANCE     *NvmePassthruInstance = NULL;
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL *EfiNvmExpressPassThru;
    VOID                               *AmiHddSecurityEndProtocol=NULL;
//APTIOV_SERVER_OVERRIDE_START
    EFI_DEVICE_PATH_PROTOCOL  		   *PciDevPath;
//APTIOV_SERVER_OVERRIDE_END
    REPORT_STATUS_CODE(EFI_PROGRESS_CODE,EFI_IO_BUS_ATA_ATAPI | EFI_IOB_PC_INIT); 
//APTIOV_SERVER_OVERRIDE_START 
    Status = gBS->OpenProtocol( Controller,
                    			&gEfiDevicePathProtocolGuid,
                    			(VOID**)&PciDevPath,
                    			This->DriverBindingHandle,
                    			Controller,
                    			EFI_OPEN_PROTOCOL_BY_DRIVER );

    if (EFI_ERROR (Status)) {
        return Status;
    }
//APTIOV_SERVER_OVERRIDE_END

    // Get the PciIO interface
    Status = gBS->OpenProtocol( Controller,
                        &gEfiPciIoProtocolGuid,
                        (VOID **)&PciIO,
                        This->DriverBindingHandle,     
                        Controller,
                        EFI_OPEN_PROTOCOL_BY_DRIVER);

    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }
    
    DEBUG((EFI_D_ERROR,"\nNVMe Driver Detection and Configuration starts\n"));

    // Do Controller Init
    Status = InitializeNvmeController(Controller, This->DriverBindingHandle, &NvmeController);
    
    if (EFI_ERROR(Status)) {
        goto Error_Path;
    }
    
    // Get the IdentifyData
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof (IDENTIFY_CONTROLLER_DATA),
                    (VOID**)&IdentifyData
                    );
    
    if (EFI_ERROR(Status)) {
        goto Error_Path_1;
    }
    
    gBS->SetMem(IdentifyData, sizeof(IDENTIFY_CONTROLLER_DATA), 0);
    NvmeController->IdentifyControllerData = IdentifyData;
    
    // Get IdentifyController Data Structure    
    Status = GetIdentifyData (NvmeController, (UINT8 *)IdentifyData, 1, 0);
    if (EFI_ERROR(Status)) {
        goto Error_Path_1;
    }

    GetNvmeUnicodeStringName(NvmeController);

    // Get the list of Active Name space IDs
    Status = gBS->AllocatePages (
                        AllocateAnyPages,
                        EfiRuntimeServicesData,
                        EFI_SIZE_TO_PAGES(4096),
                        (EFI_PHYSICAL_ADDRESS*)&(ActiveNameSpaceIDs));

    if (EFI_ERROR(Status)) {
        goto Error_Path_1;
    }
    
    gBS->SetMem(ActiveNameSpaceIDs, 4096, 0);
    
    NvmeController->ActiveNameSpaceIDs = ActiveNameSpaceIDs;
    
    // Since "Get list of Active NameSpace IDs" isn't working 
    // use alternate method to get the active name space. Zero is reserved
    for (i=1; i < IdentifyData->NN + 1; i++ ) {
        ActiveNameSpaceIDs[i] = i;
    }
    
    Status = EnumerateActiveNameSpace (NvmeController);
    if (EFI_ERROR(Status)) {
        goto Error_Path_1;
    }

    // Is there any active name space discovered
    if(IsListEmpty(&NvmeController->ActiveNameSpaceList)) {
        goto    exit_NvmeBusStart;
    }

    // Installing EfiNvmExpressPassThru protocol
    Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiNvmExpressPassThruProtocolGuid,
                  (VOID **)&EfiNvmExpressPassThru,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
    
    if ( EFI_ERROR(Status) ) {
        // Allocate memory for NVM_EXPRESS_PASS_THRU_INSTANCE structure
        Status = gBS->AllocatePool (
                      EfiBootServicesData,
                      sizeof(NVM_EXPRESS_PASS_THRU_INSTANCE),
                      (VOID**)&NvmePassthruInstance
                      );

        if( !EFI_ERROR(Status) ) {

            Status = gBS->AllocatePool (
                          EfiBootServicesData,
                          sizeof(EFI_NVM_EXPRESS_PASS_THRU_MODE),
                          (VOID**)&NvmePassthruInstance->EfiNvmExpressPassThru.Mode
                          );
            if(EFI_ERROR(Status)) {
                gBS->FreePool(NvmePassthruInstance);
            }
        }

        if (!EFI_ERROR(Status)) {

            EfiNvmExpressPassThru = (EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL*)&(NvmePassthruInstance->EfiNvmExpressPassThru);

            NvmePassthruInstance->NvmeControllerProtocol = NvmeController;
            NvmeController->EfiNvmExpressPassThru = EfiNvmExpressPassThru;
            NvmePassthruInstance->ControllerHandle = Controller;

            EfiNvmExpressPassThru->Mode->Attributes = EFI_NVM_EXPRESS_PASS_THRU_ATTRIBUTES_PHYSICAL |
                                                       EFI_NVM_EXPRESS_PASS_THRU_ATTRIBUTES_CMD_SET_NVM;
            EfiNvmExpressPassThru->Mode->IoAlign = 2; // DWORD aligned
            EfiNvmExpressPassThru->Mode->NvmeVersion = 0;
            EfiNvmExpressPassThru->PassThru = NvmePassThru;
            EfiNvmExpressPassThru->GetNextNamespace = GetNextNamespace;
            EfiNvmExpressPassThru->BuildDevicePath = BuildDevicePath;

            #if defined(USE_MDE_PKG_NVME_PASSTHRU_CHANGES)
                EfiNvmExpressPassThru->GetNamespace = GetNamespace;
            #else
                EfiNvmExpressPassThru->GetNameSpace = GetNamespace;
            #endif

            gBS->InstallMultipleProtocolInterfaces( &NvmePassthruInstance->ControllerHandle,
                                                    &gEfiNvmExpressPassThruProtocolGuid, 
                                                    &NvmePassthruInstance->EfiNvmExpressPassThru,
                                                    NULL
                                                   );
        }
    }

    // Install EFI Security Protocol
    InstallEfiSecurityStorageProtocol (This, NvmeController);

    // Installing Smart Interface for Nvme device

    // Locate HddSmartInitProtocol to provide Nvme HddSmart support
    if ( HddSmartInitProtocol == NULL ) {
        gBS->LocateProtocol (&gAmiHddSmartInitProtocolGuid,
                             NULL,
                             (VOID **)&HddSmartInitProtocol);
    }

    if( HddSmartInitProtocol != NULL ) {
        // Routine will check SmartHealth status of Nvme device
        HddSmartInitProtocol->InitSmartSupport( NvmeController, AmiStorageInterfaceNvme );

        if(HddSmartInitProtocol->SmartDiagonasticFlag) {
            HddSmartInitProtocol->InstallSmartInterface( NvmeController, AmiStorageInterfaceNvme );
        }
    }

    Status = EFI_UNSUPPORTED;

    // Install HDDSecurity protocol for NVMe device that supports ATA security Protocol
    if(gNvmeHddSecurityInitProtocol == NULL) {
        gBS->LocateProtocol( &gAmiHddSecurityInitProtocolGuid,
                             NULL,
                             (VOID **) &gNvmeHddSecurityInitProtocol);
    }

    if( gNvmeHddSecurityInitProtocol != NULL) {
        Status = gNvmeHddSecurityInitProtocol->InstallSecurityInterface(NvmeController, AmiStorageInterfaceNvme );
    }
    // Install TcgStorageSecurity protocol for NVMe device that supports TCG security Protocol
    if (EFI_ERROR(Status)) {
        if(gAmiTcgStorageSecurityInitProtocol == NULL) {
            gBS->LocateProtocol( &gAmiTcgStorageSecurityInitProtocolGuid,
                                 NULL,
                                 (VOID **) &gAmiTcgStorageSecurityInitProtocol);
        }

        if( gAmiTcgStorageSecurityInitProtocol != NULL ) {
            Status = gAmiTcgStorageSecurityInitProtocol->InstallSecurityInterface(NvmeController, AmiStorageInterfaceNvme );
        }
    }

    // If No Security protocol installed then send freeze lock command to the device at ReadyToBoot event.
    if (EFI_ERROR(Status)) {
	    if(NvmeReadyToBootEvent == NULL ) {
            gBS->CreateEventEx(
		        EVT_NOTIFY_SIGNAL, TPL_CALLBACK, 
		        NvmeFreezeLockDevice,
		        NULL,
		        &gEfiEventReadyToBootGuid,
		        &NvmeReadyToBootEvent);
	    }
    }

    Status = gBS->HandleProtocol( Controller, 
                                  &gAmiHddSecurityEndProtocolGuid, 
                                  &AmiHddSecurityEndProtocol
                                  );

    if( Status == EFI_SUCCESS ) {
        // Protocol already installed on the Controller handle.
        // UnInstall and Install back the protocol interface to Notify the Password verification 
        Status = gBS->UninstallProtocolInterface(
                                    Controller, 
                                    &gAmiHddSecurityEndProtocolGuid, 
                                    NULL
                                    );

        ASSERT_EFI_ERROR(Status);
    }

    // This will notify AMITSE to invoke the HDD password Screen
    Status = gBS->InstallProtocolInterface(
                                    &Controller, 
                                    &gAmiHddSecurityEndProtocolGuid, 
                                    EFI_NATIVE_INTERFACE,
                                    NULL
                                    );

    ASSERT_EFI_ERROR(Status);
    
    // Locate AmiBlockIoWriteProtectionProtocol to provide BootSectorWriteProtection support
    if (AmiBlkWriteProtection == NULL) {
        gBS->LocateProtocol( &gAmiBlockIoWriteProtectionProtocolGuid, 
                             NULL, 
                             &AmiBlkWriteProtection ); 
    }
     //Check whether the device is locked.
    if ((GetNvmeSecurityStatus( EfiNvmExpressPassThru ) & NVME_SECURITY_LOCK_MASK)) {
        goto exit_NvmeBusStart;
    }

    // Create Submission and Completion Queue1
    NvmeController->NVMQueueNumber = 1;
    Status  = CreateAdditionalSubmissionCompletionQueue(NvmeController, 
                                                NvmeController->NvmeCmdWrapper, 
                                                NvmeController->NVMQueueNumber, 
                                                NvmeController->MaxQueueEntrySupported >= QUEUE1_SIZE ? QUEUE1_SIZE : NvmeController->MaxQueueEntrySupported
                                                );
    
    if (EFI_ERROR(Status)) {
        goto Error_Path;
    }
    
    Status = InstallBlockIoDiskInfo(This, NvmeController);
    
    if (EFI_ERROR(Status)) {
        goto Error_Path_1;
    }

    
    // Install NvmePassThru protocol
    Status = gBS->OpenProtocol( 
                  Controller,
                  &gAmiNvmePassThruProtocolGuid,
                  (VOID **)&AmiNvmePassThru,
                  This->DriverBindingHandle,     
                  Controller,   
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
    
    if (EFI_ERROR(Status)) {
        Status = gBS->AllocatePool (
                      EfiBootServicesData,
                      sizeof(AMI_NVME_PASS_THRU_PROTOCOL),
                      (VOID**)&AmiNvmePassThru
                      );
        if (!EFI_ERROR(Status)) {
            AmiNvmePassThru->ExecuteNvmeCmd = ExecuteNvmeCmd;
            gBS->InstallMultipleProtocolInterfaces(
                        &Controller,
                        &gAmiNvmePassThruProtocolGuid, 
                        AmiNvmePassThru,
                        NULL
                        );
        }
        
    }

    if(NvmeShutdownEvent == NULL) {
        Status = gBS->CreateEventEx(
                         EVT_NOTIFY_SIGNAL,
                         TPL_CALLBACK,
                         NvmeShutdown,
                         NULL,
                         &gAmiResetSystemEventGuid,
                         &NvmeShutdownEvent);
    }
    
exit_NvmeBusStart:
    DEBUG((EFI_D_ERROR,"\nNVMe Driver Detection and Configuration Ends with Status =  EFI_SUCCESS\n"));  
    
    return EFI_SUCCESS;
    
Error_Path_1:
    // Uninstall NvmeController
    gBS->UninstallMultipleProtocolInterfaces(
                        Controller,
                        &gAmiNvmeControllerProtocolGuid, 
                        NvmeController,
                        NULL
                        );
Error_Path:
    gBS->CloseProtocol ( Controller,
                  &gEfiPciIoProtocolGuid,
                  This->DriverBindingHandle,
                  Controller);
//APTIOV_SERVER_OVERRIDE_START
    gBS->CloseProtocol (
                    Controller,
                    &gEfiDevicePathProtocolGuid,
                    This->DriverBindingHandle,
                    Controller);
//APTIOV_SERVER_OVERRIDE_END
    DEBUG((EFI_D_ERROR,"\nNVMe Driver Detection and Configuration Ends with Status %r\n", Status));
    return Status;

}

/**
    Uninstall all devices installed in start procedure.

    @param This 
    @param Controller 
    @param NumberOfChildren 
    @param ChildHandleBuffer 

    @retval EFI_STATUS

**/

EFI_STATUS 
EFIAPI
NvmeBusStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL *This,
    IN  EFI_HANDLE                  Controller,
    IN  UINTN                       NumberOfChildren,
    IN  EFI_HANDLE                  *ChildHandleBuffer
 )
{

    EFI_STATUS                         Status;
    AMI_NVME_CONTROLLER_PROTOCOL       *NvmeController;
    ACTIVE_NAMESPACE_DATA              *ActiveNameSpace;
    UINTN                              Index = 0;
    EFI_DEVICE_PATH_PROTOCOL           *DevicePath;
    NVME_DEVICE_PATH                   *NvmeDevicePath;
    LIST_ENTRY                         *LinkData;
    AMI_NVME_PASS_THRU_PROTOCOL        *AmiNvmePassThru;
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL *NvmExpressPassthruProtocol;
    NVM_EXPRESS_PASS_THRU_INSTANCE     *NvmePassthruInstance = NULL;

    // Check if gAmiNvmeControllerProtocolGuid is installed on the device
    Status = gBS->OpenProtocol( Controller,
                        &gAmiNvmeControllerProtocolGuid,
                        (VOID **)&NvmeController,
                        This->DriverBindingHandle,
                        Controller,
                        EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    
    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }
    
    //
    // Check if ChildHandleBuffer is valid
    //
    if (NumberOfChildren) {
        while (NumberOfChildren) {
            // Does the child handle have the correct devicepath
            Status = gBS->OpenProtocol(
                            ChildHandleBuffer[Index],
                            &gEfiDevicePathProtocolGuid,
                            (VOID **)&DevicePath,
                            This->DriverBindingHandle,     
                            Controller,
                            EFI_OPEN_PROTOCOL_GET_PROTOCOL);
            
            // Locate Nvme devicepath
            NvmeDevicePath = (NVME_DEVICE_PATH *) DevicePath;
            
            while (!IsDevicePathEnd ((VOID *)NvmeDevicePath)) {
                
                NvmeDevicePath = (NVME_DEVICE_PATH *)NextDevicePathNode (NvmeDevicePath);
                if (NvmeDevicePath && NvmeDevicePath->Header.Type == MESSAGING_DEVICE_PATH && \
                                NvmeDevicePath->Header.SubType == MSG_NVME_DP){
                    
                    for (LinkData = NvmeController->ActiveNameSpaceList.ForwardLink; \
                            LinkData != &NvmeController->ActiveNameSpaceList; 
                            ) {
                        
                        ActiveNameSpace = BASE_CR(LinkData ,ACTIVE_NAMESPACE_DATA, Link);
                        
                        // Is it the correct ActiveNameSpace
                        if (ActiveNameSpace->IdentifyNamespaceData.EUI64 == NvmeDevicePath->EUI64 && \
                                    ActiveNameSpace->ActiveNameSpaceID == NvmeDevicePath->Nsid ) {
                            
                            // Uninstall and close the protocols
                            Status = gBS->CloseProtocol ( 
                                                Controller,
                                                &gEfiPciIoProtocolGuid,
                                                This->DriverBindingHandle,
                                                ChildHandleBuffer[Index]);
                            
                            if (EFI_ERROR(Status)) {
                                continue;
                            }
                            
                            // Check if BlockIO is installed
                            Status = gBS->OpenProtocol( ChildHandleBuffer[Index],
                                                &gEfiBlockIoProtocolGuid,
                                                NULL,
                                                This->DriverBindingHandle,     
                                                ChildHandleBuffer[Index],   
                                                EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

                            if (Status == EFI_SUCCESS) {
                                Status = gBS->UninstallMultipleProtocolInterfaces (
                                                ChildHandleBuffer[Index],
                                                &gEfiBlockIoProtocolGuid,
                                                &(ActiveNameSpace->NvmeBlockIO),
                                                NULL);
                            }

                            // Check if DiskInfo is installed
                            Status = gBS->OpenProtocol( ChildHandleBuffer[Index],
                                                        &gEfiDiskInfoProtocolGuid,
                                                        NULL,
                                                        This->DriverBindingHandle,
                                                        ChildHandleBuffer[Index],
                                                        EFI_OPEN_PROTOCOL_TEST_PROTOCOL );

                            if (Status == EFI_SUCCESS) {
                                Status = gBS->UninstallMultipleProtocolInterfaces (
                                                ChildHandleBuffer[Index],
                                                &gEfiDiskInfoProtocolGuid,
                                                &(ActiveNameSpace->NvmeDiskInfo),
                                                NULL);
                            }
                          
                            Status = gBS->UninstallMultipleProtocolInterfaces (
                                            ChildHandleBuffer[Index],
                                            &gEfiDevicePathProtocolGuid,
                                            ActiveNameSpace->EfiDevicePath,
                                            NULL);
                            
                            if (EFI_ERROR(Status)) {
                                return EFI_DEVICE_ERROR;
                            } else {
                                // Delete from the list
//                                RemoveEntryList(&(ActiveNameSpace->Link));
                                
                                // Free up all the memory 
                                NvmeController->PciIO->Unmap(NvmeController->PciIO, 
                                                            ActiveNameSpace->PRP2ListUnMap
                                                            );
                                
                                  if (ActiveNameSpace->PRP2List) { 
                                      NvmeController->PciIO->FreeBuffer (NvmeController->PciIO,
                                                                      EFI_SIZE_TO_PAGES(NvmeController->MemoryPageSize),\
                                                                      ActiveNameSpace->PRP2List
                                                                      );
                                  }
                                  
                                  gBS->FreePool (ActiveNameSpace->NvmeBlockIO.Media);
                                  gBS->FreePool (ActiveNameSpace->EfiDevicePath);

                                  LinkData = RemoveEntryList (LinkData);
                                  gBS->FreePool (ActiveNameSpace);
                            }
                        }
                        
                    }

                    break;
                }
                
                NvmeDevicePath = (NVME_DEVICE_PATH *)NextDevicePathNode (NvmeDevicePath);
            } 
            Index++;
            NumberOfChildren --;
        }
    } else {
          // Check if all Active name space has been stopped
          if(!IsListEmpty(&NvmeController->ActiveNameSpaceList)) {
              return EFI_DEVICE_ERROR;
          } 

          // Before un-installing HDD security check whether it is installed or not
          Status = gBS->OpenProtocol( Controller,
                      &gAmiHddSecurityProtocolGuid,
                      NULL,
                      This->DriverBindingHandle,
                      Controller, 
                      EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

          if ( Status == EFI_SUCCESS ) {
               if(gNvmeHddSecurityInitProtocol != NULL) {
                   gNvmeHddSecurityInitProtocol->StopSecurityModeSupport(NvmeController, AmiStorageInterfaceNvme);
               }
          }

          // Before un-installing TCGStoragesecurity check whether it is installed or not
          Status = gBS->OpenProtocol( Controller,
                                      &gAmiTcgStorageSecurityProtocolGuid,
                                      NULL,
                                      This->DriverBindingHandle,
                                      Controller,
                                      EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

          if ( Status == EFI_SUCCESS ) {
               if(gAmiTcgStorageSecurityInitProtocol != NULL) {
                   gAmiTcgStorageSecurityInitProtocol->StopSecurityModeSupport(NvmeController, AmiStorageInterfaceNvme);
               }
          }

          // Before un-installing HDDSmart check whether it is installed or not
          Status = gBS->OpenProtocol( Controller,
                      &gAmiHddSmartProtocolGuid,
                      NULL,
                      This->DriverBindingHandle,
                      Controller, 
                      EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

          if (Status == EFI_SUCCESS) {
               if( HddSmartInitProtocol != NULL && HddSmartInitProtocol->SmartDiagonasticFlag ) {
                   HddSmartInitProtocol->UnInstallSmartInterface( NvmeController, AmiStorageInterfaceNvme);
               }
          }

          // Optional protocol. Don't check for error.   
          gBS->UninstallMultipleProtocolInterfaces (
                         Controller,
                         &gEfiStorageSecurityCommandProtocolGuid,
                         &(NvmeController->StorageSecurityInterface),
                         NULL);

          // No child is active. 
          // Stop the controller. This will delete all the Queues.
          CONTROLLER_REG32_AND(NvmeController, Offset_CC, 0xFFFFFFFE);
          
          Status = gBS->UninstallMultipleProtocolInterfaces (
                         Controller,
                         &gAmiNvmeControllerProtocolGuid,
                         NvmeController,
                         NULL);
          ASSERT_EFI_ERROR(Status);
          Status = gBS->OpenProtocol( Controller,
                                &gAmiNvmePassThruProtocolGuid,
                                (VOID **)&AmiNvmePassThru,
                                This->DriverBindingHandle,     
                                Controller,   
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        
          if (!EFI_ERROR(Status)) {
              Status = gBS->UninstallMultipleProtocolInterfaces(
                              Controller,
                              &gAmiNvmePassThruProtocolGuid, 
                              AmiNvmePassThru,
                              NULL
                              );
        
              ASSERT_EFI_ERROR(Status);
          }

          Status = gBS->OpenProtocol( Controller,
                                &gEfiNvmExpressPassThruProtocolGuid,
                                (VOID **)&NvmExpressPassthruProtocol,
                                This->DriverBindingHandle,
                                Controller,   
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
        
          if (!EFI_ERROR(Status)) {

              Status = gBS->UninstallMultipleProtocolInterfaces(
                              Controller,
                              &gEfiNvmExpressPassThruProtocolGuid, 
                              NvmExpressPassthruProtocol,
                              NULL
                              );
        
              ASSERT_EFI_ERROR(Status);

              NvmePassthruInstance = BASE_CR( NvmExpressPassthruProtocol ,NVM_EXPRESS_PASS_THRU_INSTANCE, EfiNvmExpressPassThru);
              gBS->FreePool (NvmePassthruInstance->EfiNvmExpressPassThru.Mode);
              gBS->FreePool (NvmePassthruInstance);
          }

          // Uninstall and close the protocols
          Status = gBS->CloseProtocol ( 
                            Controller,
                            &gEfiPciIoProtocolGuid,
                            This->DriverBindingHandle,
                            Controller);
        
          ASSERT_EFI_ERROR(Status);
//APTIOV_SERVER_OVERRIDE_START  
          Status = gBS->CloseProtocol (
                           Controller,
                           &gEfiDevicePathProtocolGuid,
                           This->DriverBindingHandle,
                           Controller);
          
          ASSERT_EFI_ERROR(Status);
//APTIOV_SERVER_OVERRIDE_END
          // Free up all the memory allocated
          if (NvmeController->IdentifyControllerData) { 
              gBS->FreePool (NvmeController->IdentifyControllerData);
          }
        
          if (NvmeController->IdentifyControllerData) { 
              gBS->FreePages ((EFI_PHYSICAL_ADDRESS)NvmeController->ActiveNameSpaceIDs, 
                          EFI_SIZE_TO_PAGES(4096)
                          );
          }

          if (NvmeController->AdminSubmissionQueueUnMap) { 
              NvmeController->PciIO->Unmap (NvmeController->PciIO, 
                                    NvmeController->AdminSubmissionQueueUnMap
                                    );
          }
          if (NvmeController->AdminSubmissionUnAligned) { 
              NvmeController->PciIO->FreeBuffer (NvmeController->PciIO, \
                                            EFI_SIZE_TO_PAGES(NvmeController->AdminSubmissionUnAlignedSize), \
                                           (VOID *)NvmeController->AdminSubmissionUnAligned
                                           );
        }
        
          if (NvmeController->AdminCompletionQueueUnMap) {
              NvmeController->PciIO->Unmap (NvmeController->PciIO, 
                                        NvmeController->AdminCompletionQueueUnMap
                                        );
          }
          if (NvmeController->AdminCompletionUnAligned) { 
              NvmeController->PciIO->FreeBuffer (NvmeController->PciIO, \
                                            EFI_SIZE_TO_PAGES(NvmeController->AdminCompletionUnAlignedSize), \
                                            (VOID *)NvmeController->AdminCompletionUnAligned
                                            );
          }
        
          if (NvmeController->Queue1SubmissionQueueUnMap) { 
              NvmeController->PciIO->Unmap (NvmeController->PciIO, 
                                        NvmeController->Queue1SubmissionQueueUnMap
                                        );
          }
          if (NvmeController->Queue1SubmissionUnAligned) { 
              NvmeController->PciIO->FreeBuffer (NvmeController->PciIO, \
                                             EFI_SIZE_TO_PAGES(NvmeController->Queue1SubmissionUnAlignedSize), \
                                             (VOID *)NvmeController->Queue1SubmissionUnAligned
                                             );
          }
        
          if (NvmeController->Queue1CompletionQueueUnMap) {  
              NvmeController->PciIO->Unmap (NvmeController->PciIO, 
                                    NvmeController->Queue1CompletionQueueUnMap
                                    ); 
          }
          if (NvmeController->Queue1CompletionUnAligned) { 
              NvmeController->PciIO->FreeBuffer (NvmeController->PciIO, \
                                             EFI_SIZE_TO_PAGES(NvmeController->Queue1CompletionUnAlignedSize), \
                                             (VOID *)NvmeController->Queue1CompletionUnAligned
                                             );
          }
          
          if (NvmeController->NvmeCmdWrapper) { 
              gBS->FreePool (NvmeController->NvmeCmdWrapper); 
          }
        
          if (NvmeController->UDeviceName) {
              gBS->FreePool (NvmeController->UDeviceName->Language);
              gBS->FreePool (NvmeController->UDeviceName->UnicodeString);
              gBS->FreePool (NvmeController->UDeviceName);
          }

          if (NvmeController) { 
              gBS->FreePool (NvmeController);
          }
          
          if(NvmeShutdownEvent != NULL) {
              gBS->CloseEvent(NvmeShutdownEvent);
              NvmeShutdownEvent = NULL;              
          }
        
    }
    
    return EFI_SUCCESS;

}

/**
    Initialize Nvme controller and ADMIN submission and Completion 
    queues

        
    @param  Controller 
    @param  DriverBindingHandle 
    @param  NvmeControllerReturnAddress 

    @retval EFI_STATUS

**/

EFI_STATUS
InitializeNvmeController (
    IN  EFI_HANDLE                      Controller,
    IN  EFI_HANDLE                      DriverBindingHandle,
    OUT AMI_NVME_CONTROLLER_PROTOCOL    **NvmeControllerReturnAddress
)
{
    
    EFI_STATUS                      Status;
    EFI_PCI_IO_PROTOCOL             *PciIO;
    PCI_TYPE00                      PciConfig;
    UINT64                          ControllerCapabilities;
    UINT32                          Delay;
    UINT32                          ProgramCC = 0;
    EFI_PHYSICAL_ADDRESS            AdminQueueAddress = 0;
    UINTN                           AllocatePageSize;
    UINTN                           AdminQueueSize;
    UINTN                           OrgAdminQueueSize;
    AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController = NULL;
    UINT64                          PciAttributes;
    NVME_DXE_SMM_DATA_AREA          *NvmeDxeSmmDataArea = 0;
    
    Status = gBS->AllocatePool (EfiBootServicesData,
                    sizeof(AMI_NVME_CONTROLLER_PROTOCOL),
                    (VOID**)&NvmeController
                    );
    
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    *NvmeControllerReturnAddress = NvmeController;
    
    // Clear memory
    gBS->SetMem(NvmeController, sizeof(AMI_NVME_CONTROLLER_PROTOCOL), 0);

    Status = gBS->AllocatePool ( EfiReservedMemoryType,
                                 sizeof(NVME_DXE_SMM_DATA_AREA),
                                 (VOID**)&NvmeDxeSmmDataArea
                                 );

    if ( EFI_ERROR(Status) ) {
        return Status;
    }

    // Clear memory
    gBS->SetMem( NvmeDxeSmmDataArea, sizeof(NVME_DXE_SMM_DATA_AREA), 0 );
    NvmeController->NvmeDxeSmmDataArea = NvmeDxeSmmDataArea;
    
    InitializeListHead (&NvmeController->ActiveNameSpaceList);
    
    // Get the PciIO interface
    Status = gBS->OpenProtocol( Controller,
                        &gEfiPciIoProtocolGuid,
                        (VOID **)&PciIO,
                        DriverBindingHandle,     
                        Controller,   
                        EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    // Enable PCI Command register
    Status = PciIO->Attributes(PciIO, EfiPciIoAttributeOperationSupported, 0, &PciAttributes);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    Status = PciIO->Attributes(PciIO, EfiPciIoAttributeOperationEnable, PciAttributes & EFI_PCI_DEVICE_ENABLE, NULL);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    Status = PciIO->Pci.Read ( PciIO,
                               EfiPciIoWidthUint8,
                               0,
                               sizeof (PCI_TYPE00),
                               &PciConfig
                               );
    
    if (EFI_ERROR(Status)) {
        return Status;
    }    
    NvmeController->Signature = NVME_CONTROLLER_SIG;
    NvmeController->NvmeBarOffset = LShiftU64(PciConfig.Device.Bar[1], 32) + (PciConfig.Device.Bar[0] & ~(0x07));
    NvmeController->PciIO = PciIO;
    NvmeController->ControllerHandle = Controller;
    
    ControllerCapabilities = LShiftU64(CONTROLLER_REG32(NvmeController, 4), 32) + 
                                        CONTROLLER_REG32(NvmeController, 0);
    
    // Initialize various capability values
    
    NvmeController->RawControllerCapabilities = ControllerCapabilities;
    NvmeController->MaxQueueEntrySupported = (ControllerCapabilities & 0xFFFF) + 1;
    NvmeController->ContiguousQueueRequired = (BOOLEAN)(RShiftU64((ControllerCapabilities & 0x10000), 16));
    NvmeController->ArbitrationMechanismSupport = (BOOLEAN)(RShiftU64((ControllerCapabilities & 0x60000), 17));
    NvmeController->TimeOut = (UINT8)RShiftU64((ControllerCapabilities & 0xFF000000), 24);  // 500msec units
    NvmeController->DoorBellStride = (UINT8)(RShiftU64((ControllerCapabilities & 0xF00000000), 32));
    NvmeController->NVMResetSupport = (BOOLEAN) (RShiftU64((ControllerCapabilities & 0x1000000000), 36));
    NvmeController->CmdSetsSupported = (UINT8)(RShiftU64((ControllerCapabilities & 0x1FE000000000), 37));
    NvmeController->MemoryPageSizeMin = (UINT32) LShiftU64(1, (UINTN)(RShiftU64((ControllerCapabilities & 0xF000000000000), 48) + 12));  // In Bytes
    NvmeController->MemoryPageSizeMax = (UINT32) LShiftU64(1, (UINTN)(RShiftU64((ControllerCapabilities & 0xF0000000000000), 52) + 12)); // In Bytes
    
    PrintNvmeCapability(NvmeController);
    
    // Is NVM command set supported
    if (!(NvmeController->CmdSetsSupported & 0x1)) {
        return EFI_UNSUPPORTED;
    }
    
    // Check if the controller is already running. If yes stop it.
    Delay = NvmeController->TimeOut * 500;
    if (CONTROLLER_REG32(NvmeController, Offset_CC) & 0x1) {
        
        //  Disable Enable bit
        CONTROLLER_REG32_AND (NvmeController, Offset_CC, ~0x01);
        do {
            if (!(CONTROLLER_REG32(NvmeController, Offset_CSTS) & 0x1)) {
                break;
            }
            gBS->Stall(1000); // 1msec delay
        }while (--Delay);
    }
    
    if (!Delay) {
        return EFI_DEVICE_ERROR;
    }
    
    // Initialize Controller configuration register. 
    // Select Round Robin and NVM Command Set (both values are zero)
    // Max Page Size
    NvmeController->MemoryPageSize = NvmeController->MemoryPageSizeMin;
    ProgramCC |= (UINT32) LShiftU64((UINTN)RShiftU64(NvmeController->MemoryPageSize, 13), 7);
    
    // Initialize with default value. Later it can be modified
    ProgramCC |= (6 << 16);     // I/O Submission Queue Entry Size
    ProgramCC |= (4 << 20);     // I/O Completion Queue Entry Size
    
    CONTROLLER_WRITE_REG32(NvmeController, Offset_CC, ProgramCC);

    // Allocate memory for Admin Queue. Each entry is 64bytes long and queue should be aligned on MemoryPageSize
    OrgAdminQueueSize = AdminQueueSize = (NvmeController->MaxQueueEntrySupported >= ADMIN_QUEUE_SIZE ? 
                                              ADMIN_QUEUE_SIZE : NvmeController->MaxQueueEntrySupported) * sizeof (NVME_ADMIN_COMMAND);

    AllocatePageSize = NvmeController->MemoryPageSize + AdminQueueSize; 

#if  NVME_VERBOSE_PRINT    
    DEBUG ((EFI_D_ERROR, "Value programmed in Control register  : %08X\n", ProgramCC));
#endif
    
    Status = NvmeController->PciIO->AllocateBuffer (NvmeController->PciIO,
                                             AllocateAnyPages,
                                             EfiRuntimeServicesData,
                                             EFI_SIZE_TO_PAGES(AllocatePageSize),
                                             (VOID **)&(AdminQueueAddress),
                                             EFI_PCI_IO_ATTRIBUTE_MEMORY_CACHED | EFI_PCI_IO_ATTRIBUTE_MEMORY_WRITE_COMBINE
                                             );
    
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    NvmeController->AdminSubmissionUnAligned = AdminQueueAddress;
    NvmeController->AdminSubmissionUnAlignedSize = EFI_SIZE_TO_PAGES(AllocatePageSize);
    NvmeController->AdminSubmissionQueue = (AdminQueueAddress & ~((UINT64)NvmeController->MemoryPageSize - 1)) + 
                                            NvmeController->MemoryPageSize;

    Status = NvmeController->PciIO->Map ( NvmeController->PciIO, 
                                      EfiPciIoOperationBusMasterCommonBuffer,
                                      (VOID *)NvmeController->AdminSubmissionQueue,
                                      &AdminQueueSize,
                                      &NvmeController->AdminSubmissionQueueMappedAddr,
                                      &NvmeController->AdminSubmissionQueueUnMap
                                      );
    
    // Check PCIIO mapped memory is allocated for AdminSubmissionQueue
    if (EFI_ERROR(Status) || (AdminQueueSize < OrgAdminQueueSize) ) {
        return Status;
    }
    
    Status = NvmeController->PciIO->AllocateBuffer (NvmeController->PciIO,
                                              AllocateAnyPages,
                                              EfiRuntimeServicesData,
                                              EFI_SIZE_TO_PAGES(AllocatePageSize),
                                              (VOID **)&(AdminQueueAddress),
                                              EFI_PCI_IO_ATTRIBUTE_MEMORY_CACHED | EFI_PCI_IO_ATTRIBUTE_MEMORY_WRITE_COMBINE
                                              );
    
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    NvmeController->AdminCompletionUnAligned = AdminQueueAddress;
    NvmeController->AdminCompletionUnAlignedSize = EFI_SIZE_TO_PAGES(AllocatePageSize);
    NvmeController->AdminCompletionQueue = (AdminQueueAddress & ~((UINT64)NvmeController->MemoryPageSize - 1)) + 
                                            NvmeController->MemoryPageSize;

    Status = NvmeController->PciIO->Map ( NvmeController->PciIO, 
                                       EfiPciIoOperationBusMasterCommonBuffer,
                                       (VOID *)NvmeController->AdminCompletionQueue,
                                       &AdminQueueSize,
                                       &NvmeController->AdminCompletionQueueMappedAddr,
                                       &NvmeController->AdminCompletionQueueUnMap
                                       );
    
    // Check PCIIO mapped memory is allocated for AdminCompletionQueue
    if ( EFI_ERROR(Status) || (AdminQueueSize < OrgAdminQueueSize) ) {
        return Status;
    }
    
    if ( NvmeController->MaxQueueEntrySupported >= ADMIN_QUEUE_SIZE ) {
        NvmeController->AdminSubmissionQueueSize =  NvmeController->AdminCompletionQueueSize = ADMIN_QUEUE_SIZE;
    } else {
        NvmeController->AdminSubmissionQueueSize =  NvmeController->AdminCompletionQueueSize = NvmeController->MaxQueueEntrySupported;
    }

    // Clear memory
    gBS->SetMem((VOID *)NvmeController->AdminSubmissionQueue, AdminQueueSize, 0);
    gBS->SetMem((VOID *)NvmeController->AdminCompletionQueue, AdminQueueSize, 0);
    
    // Program Admin Queue Size and Base Address
    CONTROLLER_WRITE_REG32(NvmeController, Offset_Aqa, ((NvmeController->AdminCompletionQueueSize - 1) << 16) + 
                                                (NvmeController->AdminSubmissionQueueSize - 1) );
    
    CONTROLLER_WRITE_REG32(NvmeController, Offset_Asq, 
                                                (UINT32) NvmeController->AdminSubmissionQueueMappedAddr);
    
    CONTROLLER_WRITE_REG32(NvmeController, Offset_Asq + 4,  
                                                (UINT32) RShiftU64(NvmeController->AdminSubmissionQueueMappedAddr, 32) );
    
    CONTROLLER_WRITE_REG32(NvmeController, Offset_Acq, 
                                                (UINT32)NvmeController->AdminCompletionQueueMappedAddr);
    
    CONTROLLER_WRITE_REG32(NvmeController, Offset_Acq + 4, 
                                                (UINT32) RShiftU64(NvmeController->AdminCompletionQueueMappedAddr, 32) );

#if  NVME_VERBOSE_PRINT
    DEBUG ((EFI_D_VERBOSE, "Admin Submission Queue Size              : %08X\n", NvmeController->AdminSubmissionQueueSize));
    DEBUG ((EFI_D_VERBOSE, "Admin Completion Queue Size              : %08X\n", NvmeController->AdminCompletionQueueSize));
    DEBUG ((EFI_D_VERBOSE, "Admin Submission Queue Offset            : %08X\n", NvmeController->AdminSubmissionQueue));
    DEBUG ((EFI_D_VERBOSE, "Admin Completion Queue Offset            : %08X\n", NvmeController->AdminCompletionQueue));
#endif
    
    NvmeController->NvmeDxeSmmDataArea->AdminPhaseTag = FALSE;
    
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof(NVME_COMMAND_WRAPPER),
                    (VOID**)&(NvmeController->NvmeCmdWrapper)
                    );
    
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    // Enable Controller
    CONTROLLER_REG32_OR(NvmeController, Offset_CC, 0x1);
    
    // Wait for the controller to get ready
    // Check if the controller is already running. If yes stop it.
    Delay = NvmeController->TimeOut * 500;
    do {
        if ((CONTROLLER_REG32(NvmeController, Offset_CSTS) & 0x1)) {
            break;
        }
        gBS->Stall(1000); // 1msec delay
    } while (--Delay);
        
    if (!Delay) {
        return EFI_DEVICE_ERROR;
    }
    
    Status = SetNumberOfQueues (NvmeController);
    if (EFI_ERROR(Status)) {
        return Status;
    }    
    
    // Install NvmeController
    Status = gBS->InstallMultipleProtocolInterfaces(
                            &Controller,
                            &gAmiNvmeControllerProtocolGuid, 
                            NvmeController,
                            NULL
                            );
    
    
    return Status;
    
}

/**
    Issue Nvme Admin Identify command

    @param NvmeController 
    @param IdentifyData 
    @param ControllerNameSpaceStructure 
    @param NameSpaceID 

    @retval EFI_STATUS

    @note  
    ControllerNameSpaceStructure can take 0/1/2 : 
    See Figure 81 NVM Express 1.1 Spec.
    NameSpaceID can be 0 or specific NameSpace ID. 
    See Figure 38 NVM Express 1.1 Spec.

**/

EFI_STATUS  
GetIdentifyData (
    IN  AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController, 
    OUT UINT8                           *IdentifyData,
    IN  UINT8                           ControllerNameSpaceStructure, 
    IN  UINT32                          NameSpaceID
)
{
    
    EFI_STATUS              Status;
    NVME_COMMAND_WRAPPER    *NvmeCmdWrapper = NULL;
    NVME_DXE_SMM_DATA_AREA  *NvmeDxeSmmDataArea = NvmeController->NvmeDxeSmmDataArea;
    COMPLETION_QUEUE_ENTRY  CompletionData;

    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof(NVME_COMMAND_WRAPPER),
                    (VOID**)&(NvmeCmdWrapper)
                    );
    
    if (EFI_ERROR(Status)) {
        return Status;
    }
    
    // Clear memory
    gBS->SetMem(NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER), 0);
    
    // Build NVME command
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = IDENTIFY;
    NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.PSDT = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeDxeSmmDataArea->CommandIdentifierAdmin;
    NvmeCmdWrapper->NvmCmd.NSID = NameSpaceID;
            
    NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64)IdentifyData;
    
    // If PRP1 isn't aligned on MemoryPageSize, then PRP2 will also be used.
    NvmeCmdWrapper->NvmCmd.PRP2 = ((UINT64)IdentifyData & ~((UINT64)NvmeController->MemoryPageSize-1)) + 
                                   NvmeController->MemoryPageSize; 
    
    NvmeCmdWrapper->NvmCmd.CDW10 = ControllerNameSpaceStructure;
    
    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;           // Queue 0 for Admin cmds
    NvmeCmdWrapper->CmdTimeOut = NVME_COMMAND_TIMEOUT * 1000;		
    
    Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, &CompletionData);
    
    if (!EFI_ERROR(Status)) {
        PrintIdentifyDataStructure (IdentifyData, ControllerNameSpaceStructure);
    }
    
    gBS->FreePool(NvmeCmdWrapper);
    return Status;
}

/**
    Issue Set feature command to set the # if queues to 1

    @param  NvmeController 

    @retval EFI_STATUS

**/

EFI_STATUS
SetNumberOfQueues (
    IN  AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController
)
{

    EFI_STATUS              Status;
    NVME_COMMAND_WRAPPER    *NvmeCmdWrapper = NvmeController->NvmeCmdWrapper;
    NVME_DXE_SMM_DATA_AREA  *NvmeDxeSmmDataArea = NvmeController->NvmeDxeSmmDataArea;
    COMPLETION_QUEUE_ENTRY  CompletionData;
        
    // Issue Set Feature cmd to initialize # of queues to 1
    gBS->SetMem(NvmeController->NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER), 0);
    
    // Build NVME command
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = SET_FEATURES;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeDxeSmmDataArea->CommandIdentifierAdmin;
            
    NvmeCmdWrapper->NvmCmd.CDW10 = 0x7;
    NvmeCmdWrapper->NvmCmd.CDW11 = 0x10001;
    
    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;           // Queue 0 for Admin cmds
    NvmeCmdWrapper->CmdTimeOut = NVME_COMMAND_TIMEOUT * 1000;
    
    Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, &CompletionData);
    
    return Status;
    
}

/**
    Detects active Namespace and adds it into a linked list

    @param NvmeController 

    @retval EFI_STATUS

**/

EFI_STATUS
EnumerateActiveNameSpace (
    IN  OUT AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController
)
{
    
    EFI_STATUS                  Status;
    ACTIVE_NAMESPACE_DATA       *ActiveNameSpaceData=0;
    UINT32                      i;

    NvmeController->ActiveNameSpaceList.ForwardLink = &(NvmeController->ActiveNameSpaceList);
    NvmeController->ActiveNameSpaceList.BackLink = &(NvmeController->ActiveNameSpaceList);
 
    // Get Identify NameSpace Data for each Namespace 
    for (i = 1; (i < 1024) && NvmeController->ActiveNameSpaceIDs[i]; i++) {
        
        // Get the list of Active Namespace IDs
        Status = gBS->AllocatePool (
                        EfiBootServicesData,
                        sizeof (ACTIVE_NAMESPACE_DATA),
                        (VOID**)&ActiveNameSpaceData
                        );
        if (EFI_ERROR(Status)) {
            return Status;
        }
        
        gBS->SetMem(ActiveNameSpaceData, sizeof(ACTIVE_NAMESPACE_DATA), 0);
        
        ActiveNameSpaceData->Link.ForwardLink = &(ActiveNameSpaceData->Link);
        ActiveNameSpaceData->Link.BackLink = &(ActiveNameSpaceData->Link);
        
        ActiveNameSpaceData->Signature = ACTIVE_NAME_SPACE_SIG;
        ActiveNameSpaceData->NvmeController = NvmeController;
        ActiveNameSpaceData->ActiveNameSpaceID = NvmeController->ActiveNameSpaceIDs[i];
        
        Status = DetectActiveNameSpace (NvmeController, ActiveNameSpaceData);
        
        if (!EFI_ERROR(Status)) {
            
            DEBUG((EFI_D_ERROR,"NameSpace %02X detected %lx\n", ActiveNameSpaceData->ActiveNameSpaceID, ActiveNameSpaceData));
            InsertTailList (&NvmeController->ActiveNameSpaceList, &ActiveNameSpaceData->Link);
            
        } else {
            
              gBS->FreePool(ActiveNameSpaceData);
              // First instance of zero marks the end of valid Namespace IDs
              if (!NvmeController->ActiveNameSpaceIDs[i+1]) {
                  break;
              }
              continue;
        }

        // First instance of zero marks the end of valid namespace IDs
        if (!NvmeController->ActiveNameSpaceIDs[i+1]) {
            break;
        }
    }

    return EFI_SUCCESS;
}

/**
    Install BlockIo, DiskInfo and DevicePath protocol for Active namespace

    @param This 
    @param NvmeController 

    @retval EFI_STATUS

**/

EFI_STATUS
InstallBlockIoDiskInfo (
    IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
    IN  AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController
)
{
    
    EFI_STATUS                  Status =  EFI_SUCCESS;
    ACTIVE_NAMESPACE_DATA       *ActiveNameSpace;
    LIST_ENTRY                  *LinkData;
    EFI_BLOCK_IO_MEDIA          *BlkMedia;
    EFI_PCI_IO_PROTOCOL         *PciIO;
    UINT32                      BlockSize;
    UINTN                       AllocatePageSize;
    UINT8                       *Buffer;
    BOOLEAN                     LegacyNvmeInfo=FALSE;
    
    for (LinkData = NvmeController->ActiveNameSpaceList.ForwardLink; 
            LinkData != &NvmeController->ActiveNameSpaceList; 
            LinkData = LinkData->ForwardLink) {
        
        ActiveNameSpace = BASE_CR(LinkData ,ACTIVE_NAMESPACE_DATA, Link);

        Status = gBS->AllocatePool (
                        EfiBootServicesData,
                        sizeof (EFI_BLOCK_IO_MEDIA),
                        (VOID**)&BlkMedia
                        );
        
        if (EFI_ERROR(Status)) return Status;
        
        gBS->SetMem(BlkMedia, sizeof(EFI_BLOCK_IO_MEDIA), 0);
        
        // PRP2List will be cleared when used
        AllocatePageSize = NvmeController->MemoryPageSize;
        Status = NvmeController->PciIO->AllocateBuffer (NvmeController->PciIO,
                                             AllocateAnyPages,
                                             EfiRuntimeServicesData,
                                             EFI_SIZE_TO_PAGES(AllocatePageSize),
                                             (VOID**)&ActiveNameSpace->PRP2List,
                                             EFI_PCI_IO_ATTRIBUTE_MEMORY_CACHED | EFI_PCI_IO_ATTRIBUTE_MEMORY_WRITE_COMBINE
                                             );
        
        if (EFI_ERROR(Status)) {
            return Status;
        }
        
        Status = NvmeController->PciIO->Map ( NvmeController->PciIO, 
                                        EfiPciIoOperationBusMasterCommonBuffer,
                                        (VOID *)ActiveNameSpace->PRP2List,
                                        &AllocatePageSize,
                                        &ActiveNameSpace->PRP2ListMappedAddr,
                                        &ActiveNameSpace->PRP2ListUnMap
                                        );
          
        if (EFI_ERROR(Status)) {
            return Status;
        }
        
        
        BlkMedia->MediaId = 0;
        BlkMedia->RemovableMedia = FALSE;
        BlkMedia->MediaPresent = TRUE;
        BlkMedia->LogicalPartition = FALSE;
        BlkMedia->ReadOnly = FALSE;
        BlkMedia->WriteCaching = FALSE;
        BlockSize =  ActiveNameSpace->IdentifyNamespaceData.LBAF[ActiveNameSpace->IdentifyNamespaceData.FLBAS & 0xF].LBADS;
        BlkMedia->BlockSize = (UINT32) LShiftU64(1, BlockSize);  
        BlkMedia->IoAlign = 2;      // DWORD aligned PRP Entry register BITS 1:0 are reserved
        BlkMedia->LastBlock = (EFI_LBA) (ActiveNameSpace->IdentifyNamespaceData.NSIZE - 1);      // Name space Size
        BlkMedia->LowestAlignedLba = 0;
        BlkMedia->LogicalBlocksPerPhysicalBlock = 1;
        BlkMedia->OptimalTransferLengthGranularity = BlkMedia->BlockSize;
        
        ActiveNameSpace->NvmeBlockIO.Revision = EFI_BLOCK_IO_PROTOCOL_REVISION3;
        ActiveNameSpace->NvmeBlockIO.Media = BlkMedia;
        ActiveNameSpace->NvmeBlockIO.Reset = NvmeReset;
        ActiveNameSpace->NvmeBlockIO.ReadBlocks = NvmeReadBlocks;
        ActiveNameSpace->NvmeBlockIO.WriteBlocks = NvmeWriteBlocks;
        ActiveNameSpace->NvmeBlockIO.FlushBlocks = NvmeFlushBlocks;

        // DiskInfo Protocol 
        gBS->CopyMem (&(ActiveNameSpace->NvmeDiskInfo.Interface), &gEfiDiskInfoNvmeInterfaceGuid, sizeof (EFI_GUID));

        ActiveNameSpace->NvmeDiskInfo.Inquiry = NvmeDiskInfoInquiry;
        ActiveNameSpace->NvmeDiskInfo.Identify = NvmeDiskInfoIdentify;
        ActiveNameSpace->NvmeDiskInfo.SenseData = NvmeDiskInfoSenseData;
        ActiveNameSpace->NvmeDiskInfo.WhichIde = NvmeDiskInfoWhichIde;

        // Create DevicePath Protocol
        Status = CreateNvmeDevicePath (This, ActiveNameSpace);
        if (EFI_ERROR(Status)) {
            continue;
        }

        // Install Devicepath and Block IO protocol
        Status = gBS->InstallMultipleProtocolInterfaces (
                        &(ActiveNameSpace->NvmeDeviceHandle),              
                        &gEfiDevicePathProtocolGuid, ActiveNameSpace->EfiDevicePath,
                        &gEfiBlockIoProtocolGuid, &(ActiveNameSpace->NvmeBlockIO),
                        &gEfiDiskInfoProtocolGuid, &(ActiveNameSpace->NvmeDiskInfo),
                        NULL);
        
        if (EFI_ERROR(Status)) {
            continue;
        }
        
        // Open PCI IO protocol by CHILD
        Status = gBS->OpenProtocol (
                    NvmeController->ControllerHandle,
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIO,
                    This->DriverBindingHandle,     
                    ActiveNameSpace->NvmeDeviceHandle,   
                    EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);
        
        ASSERT_EFI_ERROR(Status);
        
        ActiveNameSpace->Configured = TRUE;
        
        // Check if the drive ready to return data
        Status = gBS->AllocatePages (
                            AllocateAnyPages,
                            EfiBootServicesData,
                            EFI_SIZE_TO_PAGES(ActiveNameSpace->NvmeBlockIO.Media->BlockSize),  
                            (EFI_PHYSICAL_ADDRESS*)&(Buffer));
        
        if (EFI_ERROR(Status)) {
            return Status;
        }
        
        Status = ReadBlock (NvmeController, ActiveNameSpace, Buffer);
        
        if (!EFI_ERROR(Status) && (gInt13DeviceAddress <= NVMEDEVS_MAX_ENTRIES)) {
            // No error check for Int13 installation
            InstallNvmeLegacyDevice(NvmeController, ActiveNameSpace, Buffer);
            //Flag to indicate Int13 installation for ActiveNameSpace 
            LegacyNvmeInfo = TRUE;
        }
        
        gBS->FreePages((EFI_PHYSICAL_ADDRESS)Buffer, EFI_SIZE_TO_PAGES(ActiveNameSpace->NvmeBlockIO.Media->BlockSize));
    }

    if(gLegacyNvmeBuffer == NULL ) {
        Status = gBS->AllocatePages (
                        AllocateAnyPages,
                        EfiReservedMemoryType,
                        EFI_SIZE_TO_PAGES(ActiveNameSpace->NvmeBlockIO.Media->BlockSize),  
                        (EFI_PHYSICAL_ADDRESS*)&(gLegacyNvmeBuffer));
    }
    NvmeController->LegacyNvmeBuffer=gLegacyNvmeBuffer;

    if( !EFI_ERROR(Status) && LegacyNvmeInfo ) {
        // Move the Controller and ActiveName Space data into SMM also. 
        // This will done for each controller in the system
        TransferNvmeDataToSmram(NvmeController);
    }

    return Status;
}



/**
    Install EFI_STORAGE_SECUIRTY_COMMAND_PROTOCOL

    @param This 
    @param NvmeController 

    @retval EFI_STATUS

**/
EFI_STATUS
InstallEfiSecurityStorageProtocol (
    IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
    IN  AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController
)
{

    EFI_STATUS                              Status =  EFI_SUCCESS;
    
    // Check if Security Send/receive is supported
    if (!(NvmeController->IdentifyControllerData->OACS & BIT0)) {
        DEBUG ((EFI_D_VERBOSE, "Security Send/Receive cmds not supported\n"));
        return EFI_UNSUPPORTED;     
    }

    NvmeController->StorageSecurityInterface.SendData = NvmeSendData;
    NvmeController->StorageSecurityInterface.ReceiveData = NvmeReceiveData;

    // Install EFI_STORAGE_SECURITY_COMMAND_PROTOCOL on controller handle
    Status = gBS->InstallMultipleProtocolInterfaces (
                                         &(NvmeController->ControllerHandle),
                                         &gEfiStorageSecurityCommandProtocolGuid, 
                                         &(NvmeController->StorageSecurityInterface),
                                         NULL);
    ASSERT_EFI_ERROR(Status);
    return Status;
}

/**
    Transfer the Nvme Controller information to SMRAM area that would be used
    by the Nvme SMM driver 

    @param NvmeController 

    @retval None

**/

VOID
TransferNvmeDataToSmram (
    IN  AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController
)
{

    EFI_STATUS                          SmmStatus;
    EFI_SMM_COMMUNICATION_PROTOCOL     *SmmCommunication;
    EFI_SMM_COMMUNICATE_HEADER         *CommHeader;
    UINT8                              CommBuffer[sizeof(EFI_GUID) + sizeof(UINTN) + sizeof(NvmeController)];
    UINTN                              CommSize;

    // Get needed resource
    SmmStatus = gBS->LocateProtocol (
                    &gEfiSmmCommunicationProtocolGuid,
                    NULL,
                    (VOID **)&SmmCommunication
                    );
    if (EFI_ERROR (SmmStatus)) {
        return;
    }

    CommHeader = (EFI_SMM_COMMUNICATE_HEADER *)&CommBuffer[0];
    gBS->CopyMem (&CommHeader->HeaderGuid, &gAmiSmmNvmeCommunicationGuid, sizeof(gAmiSmmNvmeCommunicationGuid));
    CommHeader->MessageLength = sizeof(NvmeController);
    gBS->CopyMem (&(CommHeader->Data[0]), &NvmeController, sizeof(NvmeController));
    
    // Send command
    CommSize = sizeof(CommBuffer);
    SmmCommunication->Communicate (
                                 SmmCommunication,
                                 &CommBuffer[0],
                                 &CommSize
                                 );
    return;

}

/**
    Allocate memory and create a NVMe devicepath

    @param This 
    @param NvmeController 

    @retval EFI_STATUS

**/

EFI_STATUS
CreateNvmeDevicePath (
    IN  EFI_DRIVER_BINDING_PROTOCOL *This,
    IN  ACTIVE_NAMESPACE_DATA       *ActiveNameSpace
)
{

    EFI_STATUS                          Status;
    IN AMI_NVME_CONTROLLER_PROTOCOL     *NvmeController = ActiveNameSpace->NvmeController;
    NVME_DEVICE_PATH                    NewDevicePath;
    EFI_DEVICE_PATH_PROTOCOL            *TempDevicePath;

    NewDevicePath.Header.Type = MESSAGING_DEVICE_PATH;
    NewDevicePath.Header.SubType = MSG_NVME_DP;
    SetDevicePathNodeLength(&NewDevicePath, sizeof(NVME_DEVICE_PATH));
    NewDevicePath.Nsid = ActiveNameSpace->ActiveNameSpaceID;
    NewDevicePath.EUI64 = ActiveNameSpace->IdentifyNamespaceData.EUI64;
    
    //
    // Append the Device Path
    //
    Status = gBS->OpenProtocol ( 
                    NvmeController->ControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&TempDevicePath,
                    This->DriverBindingHandle,     
                    NvmeController->ControllerHandle,   
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    ActiveNameSpace->EfiDevicePath = AppendDevicePathNode(TempDevicePath, &NewDevicePath.Header);
    
    return Status;
}

/**
    Issue GetIdentifyData cmd to get the list of active name space. 

    @param NvmeController 
    @param ActiveNameSpaceData 

    @retval EFI_STATUS

**/

EFI_STATUS
DetectActiveNameSpace (
    IN  AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController,
    OUT ACTIVE_NAMESPACE_DATA           *ActiveNameSpaceData
)
{

    EFI_STATUS  Status;
    
    // Get list of Active NameSpace Data
    Status = GetIdentifyData (NvmeController, 
                        (UINT8*)&(ActiveNameSpaceData->IdentifyNamespaceData),
                        0, 
                        ActiveNameSpaceData->ActiveNameSpaceID
                        );
    return Status;

}


/**
    Add the NVMe active partition as a legacy device 

    @param NvmeController 

    @retval None

**/

VOID
InstallNvmeLegacyDevice (
    IN  AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController,
    IN  ACTIVE_NAMESPACE_DATA           *ActiveNameSpace,
    IN  UINT8                           *Buffer
)
{
    EFI_STATUS                      Status;
    AMI_NVME_LEGACY_PROTOCOL        *AmiNvmeLegacyProtocol = NULL;
    NVME_LEGACY_MASS_DEVICE         *NvmeLegacyMassDevice;
    NVME_MASS_DEV_INFO              *NvmeMassDevInfo;
    CHAR8                           *NewString;
    UINTN                           PciSeg, PciBus, PciDev, PciFunc;
    
    //If block size is not 512 do install INT13
    if (ActiveNameSpace->NvmeBlockIO.Media->BlockSize != 0x200) {
        return;
    }
    
    Status=gBS->LocateProtocol(&gAmiNvmeLegacyProtocolGuid, NULL, (VOID**)&AmiNvmeLegacyProtocol);
    // Legacy Support disabled. 
    if(EFI_ERROR(Status) || AmiNvmeLegacyProtocol == NULL ) {
        return;
    }
        
    Status = gBS->AllocatePool (
                        EfiBootServicesData,
                        sizeof (NVME_LEGACY_MASS_DEVICE),
                        (VOID**)&NvmeLegacyMassDevice
                        );

    if (EFI_ERROR(Status)){
        return;
    }

    Status = gBS->AllocatePool (
                        EfiBootServicesData,
                        sizeof (NVME_MASS_DEV_INFO),
                        (VOID**)&NvmeMassDevInfo
                        );

    if (EFI_ERROR(Status)){
        gBS->FreePool (NvmeLegacyMassDevice);
        return;
    }

    Status = gBS->AllocatePool(
                      EfiBootServicesData, 
                      256,
                      (VOID *) &NewString);
    ASSERT_EFI_ERROR (Status);

    // Read the first sector of the device
    ActiveNameSpace->dMaxLBA = ActiveNameSpace->NvmeBlockIO.Media->LastBlock;
    GetNvmeGeometryData(ActiveNameSpace, NvmeMassDevInfo, Buffer);

    NvmeMassDevInfo->wBlockSize=ActiveNameSpace->NvmeBlockIO.Media->BlockSize;
    NvmeMassDevInfo->dMaxLba=ActiveNameSpace->dMaxLBA;     
    NvmeMassDevInfo->bHeads=ActiveNameSpace->NumHeads;
    NvmeMassDevInfo->bSectors=ActiveNameSpace->NumSectors;
    NvmeMassDevInfo->wCylinders=ActiveNameSpace->NumCylinders;
    NvmeMassDevInfo->bNonLBAHeads=ActiveNameSpace->LBANumHeads;
    NvmeMassDevInfo->bNonLBASectors=ActiveNameSpace->LBANumSectors;
    NvmeMassDevInfo->wNonLBACylinders=ActiveNameSpace->LBANumCyls;

    PrintNvmeMassDevInfo (NvmeMassDevInfo);

    // Get the NVMe controller Bus,Dev and Fun
    NvmeController->PciIO->GetLocation (NvmeController->PciIO, &PciSeg, &PciBus, &PciDev, &PciFunc);
    UnicodeStrToAsciiStr (NvmeController->UDeviceName->UnicodeString, NewString);

    NvmeLegacyMassDevice->PciBDF=(UINT16)((PciBus << 8)+(PciDev << 3) + PciFunc);
    NvmeLegacyMassDevice->DevString=NewString;
    NvmeLegacyMassDevice->DevInfo=(VOID*)NvmeMassDevInfo;
    ActiveNameSpace->Int13DeviceAddress = gInt13DeviceAddress;
    NvmeLegacyMassDevice->LogicalAddress=ActiveNameSpace->Int13DeviceAddress;
    gInt13DeviceAddress++;
    NvmeLegacyMassDevice->StorageType = NVME_MASS_DEV_HDD;
    NvmeLegacyMassDevice->Handle=ActiveNameSpace->NvmeDeviceHandle;

    AmiNvmeLegacyProtocol->AddNvmeLegacyDevice(NvmeLegacyMassDevice);
    return;
}

/**
    Get drive geometry data for INT13 support

    @param Buffer, 
    @param NvmeMassDevInfo
    
    @retval VOID

    @note  
    
**/

EFI_STATUS
GetNvmeGeometryData (
    IN  OUT ACTIVE_NAMESPACE_DATA   *ActiveNameSpace,
    IN  OUT NVME_MASS_DEV_INFO      *NvmeMassDevInfo,
    IN  UINT8                       *Buffer
)
{
    
    EFI_STATUS  Status;
    UINT8       *ActPartAddr;
    UINT32      dHS;
    
    
    ActiveNameSpace->bStorageType = NVME_MASS_DEV_HDD;
    ActiveNameSpace->bEmuType = 0;
    
    ActiveNameSpace->bHiddenSectors = 0;
    //
    // Check for validity of Boot Record
    //
    if ( *(UINT16*)(Buffer + 0x1FE) != 0xAA55 ) {
        NvmeSetDefaultGeometry(ActiveNameSpace);
        return EFI_NOT_FOUND;
    }

    //
    // Check for validity of the partition table
    //
    if ( NvmeValidatePartitionTable( Buffer, ActiveNameSpace->dMaxLBA, &ActPartAddr) == EFI_SUCCESS ) {

        //
        // Read boot sector, set the LBA number to boot record LBA number
        //
        dHS = *((UINT32*)(ActPartAddr + 8));
        ActiveNameSpace->bHiddenSectors = (UINT8)dHS; // Save hidden sector value

        Status = NvmeReadWriteBlocks (ActiveNameSpace, ActiveNameSpace->NvmeBlockIO.Media->MediaId, (EFI_LBA)dHS, 512, Buffer, NULL, NVME_READ);

        if ( EFI_ERROR(Status) ) {  
            return EFI_NOT_FOUND;
        }

        if ( NvmeUpdateCHSFromBootRecord(ActiveNameSpace, Buffer) == EFI_SUCCESS) {
             return EFI_SUCCESS;
        } else {
            NvmeSetDefaultGeometry(ActiveNameSpace);
            ActiveNameSpace->bHiddenSectors = 0;      // Reset hidden sector value
            return EFI_SUCCESS;
        }
    }

    return EFI_SUCCESS;
}
    
/**
    This procedure sets the  default geometry for mass storage devices

    @param ActiveNameSpace 

    @retval EFI_STATUS

    @note

**/

EFI_STATUS 
NvmeSetDefaultGeometry ( 
    IN  OUT ACTIVE_NAMESPACE_DATA   *ActiveNameSpace
)
{
    ActiveNameSpace->NumHeads = 0xFF;
    ActiveNameSpace->LBANumHeads = 0xFF;
    ActiveNameSpace->NumSectors = 0x3F;
    ActiveNameSpace->LBANumSectors = 0x3F;

    NvmeMassUpdateCylinderInfo(ActiveNameSpace);
    return EFI_SUCCESS;
}

/**
    This procedure updates cylinder parameter for device geometry.
    head and sector parameters are required before invoking this
    function.

    @param ActiveNameSpace 
    
    @retval VOID

    @note

**/

VOID
NvmeMassUpdateCylinderInfo (
    IN  OUT ACTIVE_NAMESPACE_DATA   *ActiveNameSpace
)
{
    UINT32 data = (UINT32)(DivU64x64Remainder(ActiveNameSpace->dMaxLBA,(ActiveNameSpace->NumSectors * ActiveNameSpace->NumHeads), NULL));

    if (data <= 1) data++;
    if (data > 0x3FF) data = 0x3FF;   // DOS workaround

    ActiveNameSpace->NumCylinders = (UINT16)data;
    ActiveNameSpace->LBANumCyls = (UINT16)data;
    return;
}

/**
    This procedure checks whether the partition table is valid

    @param Buffer 
    @param dMaxLBA 
    @param ActPartAddr 

    @retval VOID

    @note

**/

EFI_STATUS
NvmeValidatePartitionTable (
    IN  UINT8   *Buffer,
    IN  UINT64  dMaxLBA,
    OUT UINT8   **ActPartAddr
)
{

    UINT8   *PartPtr;
    UINT8   PartNo = 0;
    UINT8   *ActPart = NULL;
    UINT8   ValidEntryCount = 0;

    //
    // Drive has a partition table, start from 1st bootable partition
    //
    PartPtr = Buffer + 0x1BE;

    for (; PartNo<4; PartNo++, PartPtr+=0x10 ) {

        if (*PartPtr & 0x7F ) { 
            return EFI_NOT_FOUND; //BootFlag should be 0x0 or 0x80
        }
        //
        // Check whether beginning LBA is reasonable
        //
        if (*(UINT32*)(PartPtr + 8) > dMaxLBA) { 
            return EFI_NOT_FOUND;
        }
        
        ValidEntryCount++;                      // Update valid entry count
        //
        // Update active entry offset
        //
        if (!(*PartPtr & 0x80)) continue;
        if (ActPart) continue;
        ActPart = PartPtr;
    }

    if (ValidEntryCount < 1) return EFI_NOT_FOUND; // At least one valid partition is found
    //
    // If no active partition table entry found use first entry
    //
    if (ActPart == NULL) ActPart = Buffer + 0x1BE;

    *ActPartAddr = ActPart;

    return EFI_SUCCESS;

}

/**
    This function parses the boot record and extract the CHS
    information of the formatted media from the boot record.
    This routine checks for DOS & NTFS formats only

    @param IdentifyData, 
    @param ControllerNameSpaceStructure
    
    @retval VOID

    @note  The amount of data that will get printed can be controlled 
    using DEBUG_ERROR_LEVEL_MASK SDL token. 
    Make sure PcdDebugPrintErrorLevel is properly cloned and set to 
    PcdsPatchableInModule in the project.

**/

EFI_STATUS
NvmeUpdateCHSFromBootRecord (
    IN  OUT ACTIVE_NAMESPACE_DATA   *ActiveNameSpace,
    IN  UINT8*                      BootRecord
)
{
    UINT32      OemName;
    UINT8       Heads;
    UINT8       SecPerTrack;
    UINT16      SecTimesHeads;
    UINT16      TotalSect;

    if (*((UINT16*)(BootRecord + 0x1FE)) != 0xAA55) { 
        return EFI_NOT_FOUND;
    }

    //
    // Read succeeded so the drive is formatted
    // Check for valid MSDOS/MSWIN/NTFS boot record
    //
    OemName = *(UINT32*)(BootRecord + 3);
    
    if ((OemName != 0x4F44534D) &&   // 'ODSM' for MSDO
        (OemName != 0x4957534D) &&   // 'IWSM' for MSWI
        (OemName != 0x5346544E)) {   // 'SFTN' for NTFS
        //
        // Check for valid FAT,FAT16,FAT32 boot records
        //
        *(BootRecord + 0x36 + 3) = 0x20;              // Ignore the 4th byte and fill it with space
        if ((*(UINT32*)(BootRecord + 0x36) != 0x20544146) &&      // " TAF" for FATx
            (*(UINT32*)(BootRecord + 0x52) != 0x33544146)) {      // "3TAF" for FAT3
            //
            // Boot Record is invalid. Return with error
            //
            return EFI_NOT_FOUND;
        }
    }

    Heads = *(BootRecord + 0x1A);         // Number of heads
    SecPerTrack = *(BootRecord + 0x18);   // Sectors/track
    SecTimesHeads = Heads * SecPerTrack;

    // Zero check added to prevent invalid sector/head information in BPB
    if (SecTimesHeads == 0) {
        return EFI_NOT_FOUND;
    }

    TotalSect = *(UINT16*)(BootRecord + 0x13);
    if ( TotalSect ) {
        ActiveNameSpace->dMaxLBA = TotalSect;
    }

    ActiveNameSpace->NumHeads = Heads;
    ActiveNameSpace->LBANumHeads = Heads;
    ActiveNameSpace->NumSectors = SecPerTrack;
    ActiveNameSpace->LBANumSectors = SecPerTrack;


    NvmeMassUpdateCylinderInfo(ActiveNameSpace);

    return  EFI_SUCCESS;
}

/**
    Read the Data from device 

    @param NvmeController,
    @param ActiveNameSpace

    @retval EFI_STATUS

**/

EFI_STATUS
ReadBlock (
    IN AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController,
    IN ACTIVE_NAMESPACE_DATA           *ActiveNameSpace,
    OUT UINT8                           *Buffer
)
{
    
    EFI_STATUS              Status;
    NVME_DXE_SMM_DATA_AREA  *NvmeDxeSmmDataArea = NvmeController->NvmeDxeSmmDataArea;
    NVME_COMMAND_WRAPPER    *NvmeCmdWrapper = NvmeController->NvmeCmdWrapper;
    COMPLETION_QUEUE_ENTRY  CompletionData;
    UINT64                  DataAddress = (UINTN)Buffer;
    UINT32                  LBANumber = 0;
    UINT32                  Retries = MAX_NVME_READ_FAIL_RETRY_COUNT;
    
    do { 
    
        // Clear memory
        gBS->SetMem(NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER), 0);
        
        // Build NVME command
        NvmeCmdWrapper->NvmCmd.CMD0.Opcode = NVME_READ;
        NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = 0;
        NvmeCmdWrapper->NvmCmd.CMD0.PSDT = 0;
        NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeDxeSmmDataArea->CommandIdentifierQueue1;
        NvmeCmdWrapper->NvmCmd.NSID = 1;
                
        NvmeCmdWrapper->NvmCmd.MPTR = 0;
        NvmeCmdWrapper->NvmCmd.PRP1 = DataAddress;
        NvmeCmdWrapper->NvmCmd.PRP2 = 0;
        NvmeCmdWrapper->NvmCmd.CDW10 = LBANumber;       // LBA 0
        NvmeCmdWrapper->NvmCmd.CDW11 = 0;           
        NvmeCmdWrapper->NvmCmd.CDW12 = 0x80000000;           
        NvmeCmdWrapper->NvmCmd.CDW13 = 0;           
        NvmeCmdWrapper->NvmCmd.CDW14 = 0;           
        
        NvmeCmdWrapper->AdminOrNVMCmdSet = FALSE;
        NvmeCmdWrapper->SQIdentifier = 1;           // Cmd issued in Queue0
        NvmeCmdWrapper->CmdTimeOut = NVME_COMMAND_TIMEOUT * 1000;
        
        Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, &CompletionData);
        
    } while (Retries-- && Status != EFI_SUCCESS);
    
    return Status;
    
}

/**
    Reads data from the given LBA address

        
    @param This 
    @param MediaId 
    @param LBA
    @param BufferSize 
    @param Buffer 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
NvmeReadBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL   *This,
    IN  UINT32                  MediaId,
    IN  EFI_LBA                 LBA,
    IN  UINTN                   BufferSize,
    OUT VOID                    *Buffer
)
{
    
    ACTIVE_NAMESPACE_DATA           *ActiveNameSpace = ACTIVE_NAMESPACE_DATA_FROM_THIS(This);
    
    return NvmeReadWriteBlocks (ActiveNameSpace, MediaId, LBA, BufferSize, Buffer, NULL, NVME_READ);
    
}

/**
    Write data from the given LBA address

    @param This 
    @param MediaId 
    @param LBA
    @param BufferSize 
    @param Buffer 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
NvmeWriteBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL   *This,
    IN  UINT32                  MediaId,
    IN  EFI_LBA                 LBA,
    IN  UINTN                   BufferSize,
    OUT VOID                    *Buffer
)
{
   
    EFI_STATUS                 Status;
    ACTIVE_NAMESPACE_DATA      *ActiveNameSpace = ACTIVE_NAMESPACE_DATA_FROM_THIS(This);

    if (AmiBlkWriteProtection != NULL) {
        Status = AmiBlkWriteProtection->BlockIoWriteProtectionCheck ( 
                                                    AmiBlkWriteProtection,
                                                    &(ActiveNameSpace->NvmeBlockIO),
                                                    LBA,
                                                    BufferSize
                                                    );
        
        // Abort operation if denied
        if(Status == EFI_ACCESS_DENIED) {
            return Status;
        }
    }
    
    return NvmeReadWriteBlocks (ActiveNameSpace, MediaId, LBA, BufferSize, Buffer, NULL, NVME_WRITE);
    
}

/**
    Resets Nvme Controller

    @param This 
    @param ExtendedVerification 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
NvmeReset (
    IN  EFI_BLOCK_IO_PROTOCOL   *This,
    IN  BOOLEAN                 ExtendedVerification
)
{
    
    EFI_STATUS  Status = EFI_SUCCESS;
    
    return Status;
}

/**
    Flushes the data

    @param This 

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
NvmeFlushBlocks (
    IN  EFI_BLOCK_IO_PROTOCOL   *This
)
{
    
    EFI_STATUS                      Status;
    ACTIVE_NAMESPACE_DATA           *ActiveNameSpace = ACTIVE_NAMESPACE_DATA_FROM_THIS(This);
    AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController = ActiveNameSpace->NvmeController;
    NVME_DXE_SMM_DATA_AREA          *NvmeDxeSmmDataArea = NvmeController->NvmeDxeSmmDataArea;
    NVME_COMMAND_WRAPPER            *NvmeCmdWrapper = NvmeController->NvmeCmdWrapper;
    COMPLETION_QUEUE_ENTRY          CompletionData;
    
    DEBUG((EFI_D_VERBOSE,"NvmeFlushBlocks \n"));

    gBS->SetMem(NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER), 0); 
    
    // Build NVME command
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = NVME_FLUSH;
    NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.PSDT = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeDxeSmmDataArea->CommandIdentifierQueue1;
    NvmeCmdWrapper->NvmCmd.NSID = ActiveNameSpace->ActiveNameSpaceID;
        
    NvmeCmdWrapper->AdminOrNVMCmdSet = FALSE;
    NvmeCmdWrapper->SQIdentifier = NvmeController->NVMQueueNumber;
    NvmeCmdWrapper->CmdTimeOut = NVME_COMMAND_TIMEOUT * 1000;      
    
    Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, &CompletionData);
    
    DEBUG((EFI_D_VERBOSE,"NvmeFlushBlocks Status %r \n", Status));
    
    return Status;
    
}

/**
    Return Nvme device Inquiry data

    @param  This 
    @param  InquiryData 
    @param  InquiryDataSize 

    @retval EFI_STATUS /EFI_NOT_FOUND

**/

EFI_STATUS
EFIAPI
NvmeDiskInfoInquiry (
    IN EFI_DISK_INFO_PROTOCOL    *This,
    IN OUT VOID                  *InquiryData,
    IN OUT UINT32                *InquiryDataSize
)
{

    return EFI_NOT_FOUND;
}

/**
    Return Identify Data

        
    @param This,
    @param IdentifyData 
    @param IdentifyDataSize 

    @retval EFI_STATUS

    @note  
      1. Return the Nvme device Identify command data.

**/
EFI_STATUS
EFIAPI
NvmeDiskInfoIdentify (
    EFI_DISK_INFO_PROTOCOL    *This,
    IN OUT VOID               *IdentifyData,
    IN OUT UINT32             *IdentifyDataSize
)
{
    ACTIVE_NAMESPACE_DATA       *ActiveNameSpace = ACTIVE_NAMESPACE_DATA_FROM_THIS_FOR_DISKINFO(This);

    if (*IdentifyDataSize < sizeof (IDENTIFY_NAMESPACE_DATA)) {
        *IdentifyDataSize = sizeof (IDENTIFY_NAMESPACE_DATA);
        return EFI_BUFFER_TOO_SMALL;
    }

    gBS->CopyMem (IdentifyData, &(ActiveNameSpace->IdentifyNamespaceData), sizeof (IDENTIFY_NAMESPACE_DATA)); 
    *IdentifyDataSize =  sizeof (IDENTIFY_NAMESPACE_DATA);
    return EFI_SUCCESS;
}

/**
    Return InfoSenseData.

    @param  This,
    @param  SenseData,
    @param  SenseDataSize,
    @param  SenseDataNumber

    @retval EFI_STATUS

    @note  
      1. Return the Sense data for the Nvme device.

**/

EFI_STATUS
EFIAPI
NvmeDiskInfoSenseData (
    IN EFI_DISK_INFO_PROTOCOL    *This,
    OUT VOID                     *SenseData,
    OUT UINT32                   *SenseDataSize,
    OUT UINT8                    *SenseDataNumber
)
{
    return EFI_NOT_FOUND;
}

/**
    Returns info about where the device is connected.

    @param This 
    @param IdeChannel 
    @param IdeDevice 

    @retval EFI_STATUS

    @note  
  1. Return Port and PMPort

**/
EFI_STATUS
EFIAPI
NvmeDiskInfoWhichIde (
    IN EFI_DISK_INFO_PROTOCOL    *This,
    OUT UINT32                   *IdeChannel,
    OUT UINT32                   *IdeDevice
)
{

    return EFI_UNSUPPORTED;
}

/**
    Function coverts Nvme Model number into Unicodestring.

    @param NvmeController

    @retval EFI_STATUS

**/
EFI_STATUS
GetNvmeUnicodeStringName (
    AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController
)
{
    EFI_STATUS                  Status=EFI_SUCCESS;
    UINT16                      DeviceName[41];
    CHAR8                       Language[] = LANGUAGE_CODE_ENGLISH;
    EFI_UNICODE_STRING_TABLE    *tempUnicodeTable;
    UINT32                      Index;

    gBS->SetMem(DeviceName, sizeof(DeviceName), 0);

    for (Index = 0; Index < 40; Index ++) {
        DeviceName[Index] = ((UINT8 *)NvmeController->IdentifyControllerData->ModelNumber)[Index];
    }

    DeviceName[40] = 0; // Word
    
    // Remove the spaces from the end of the device name
    for (Index = 39; Index > 0; Index-- ) {
        if (DeviceName[Index] != 0x20) { 
             break;
        }
        DeviceName[Index] = 0;
    }

    Status = gBS->AllocatePool ( EfiBootServicesData,
                                 sizeof (EFI_UNICODE_STRING_TABLE) * 2,
                                 (VOID**)&tempUnicodeTable
                                 );

    if (EFI_ERROR(Status)) {
        return Status;
    }

    gBS->SetMem(tempUnicodeTable, sizeof(EFI_UNICODE_STRING_TABLE) * 2, 0);

    Status = gBS->AllocatePool ( EfiBootServicesData,
                                 sizeof (Language),
                                (VOID**)&tempUnicodeTable[0].Language
                                );

    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBS->AllocatePool ( EfiBootServicesData,
                                 sizeof (DeviceName),
                                 (VOID**)&tempUnicodeTable[0].UnicodeString
                                 );

    if (EFI_ERROR(Status)) {
        return Status;
    }

    gBS->CopyMem(tempUnicodeTable[0].Language, &Language, sizeof(Language)); 
    gBS->CopyMem(tempUnicodeTable[0].UnicodeString, DeviceName, sizeof (DeviceName)); 
    tempUnicodeTable[1].Language = NULL;
    tempUnicodeTable[1].UnicodeString = NULL;
    NvmeController->UDeviceName = tempUnicodeTable;

    return Status;
}

/**
    Function returns the security status of Nvme device.

    @param EfiNvmExpressPassThru

    @retval BOOLEAN

**/
UINT16
GetNvmeSecurityStatus(
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL  *EfiNvmExpressPassThru
)
{
    EFI_STATUS                                Status;
    EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET  NvmePacket;
    EFI_NVM_EXPRESS_COMMAND                   NvmeCommand;
    EFI_NVM_EXPRESS_COMPLETION                NvmeCompletion;
    NVME_SECURITY_RECEIVE_BUFFER              SecReceiveBuffer;
    AMI_NVME_CONTROLLER_PROTOCOL              *NvmeController =\
       ((NVM_EXPRESS_PASS_THRU_INSTANCE*)EfiNvmExpressPassThru)->NvmeControllerProtocol;

    // Check Nvme device support Security commands
    if( !(NvmeController->IdentifyControllerData->OACS & 0x1) ) {
        return FALSE;
    }

    gBS->SetMem( &SecReceiveBuffer, sizeof(NVME_SECURITY_RECEIVE_BUFFER), 0 );
    gBS->SetMem( &NvmePacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET), 0 );
    gBS->SetMem( &NvmeCommand, sizeof(EFI_NVM_EXPRESS_COMMAND), 0);
    gBS->SetMem( &NvmeCompletion, sizeof(EFI_NVM_EXPRESS_COMPLETION), 0);

    NvmeCommand.Nsid = 0;
#if defined(USE_MDE_PKG_NVME_PASSTHRU_CHANGES)
    NvmeCommand.Cdw0.Opcode = SECURITY_RECEIVE;
#else
    NvmeCommand.Cdw0.OpCode = SECURITY_RECEIVE;
#endif

    NvmeCommand.Cdw0.FusedOperation = 0;
    NvmeCommand.Cdw10 = (((UINT32)NVME_SECUIRTY_SECP)<<24);
    NvmeCommand.Cdw11 = 0x10;

    NvmeCommand.Flags = ( CDW10_VALID | CDW11_VALID );

    NvmePacket.CommandTimeout = NVME_COMMAND_TIMEOUT * 10000000;    
    NvmePacket.NvmeCmd = &NvmeCommand;   
    NvmePacket.QueueType = 0;
    NvmePacket.NvmeCompletion = &NvmeCompletion;
    NvmePacket.TransferBuffer = &SecReceiveBuffer;
    NvmePacket.TransferLength = sizeof(NVME_SECURITY_RECEIVE_BUFFER);

    // Send Command through Passthru API
    Status = EfiNvmExpressPassThru->PassThru ( EfiNvmExpressPassThru,
                                               0,
                                               &NvmePacket,
                                               NULL
                                               );
    
    if(!EFI_ERROR(Status)){
        // Return the Security status of Nvme Device.
        return SecReceiveBuffer.NvmeSecStatus;
    } else {
	    return 0;
    }

}

/**
    Function sends freeze lock command to Nvme controller

    @param EfiNvmExpressPassThru

    @retval BOOLEAN

**/
EFI_STATUS
NvmeSendSecurityFreezeLock (
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL  *EfiNvmExpressPassThru
)
{
    EFI_STATUS                                Status;
    EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET  NvmePacket;
    EFI_NVM_EXPRESS_COMMAND                   NvmeCommand;
    EFI_NVM_EXPRESS_COMPLETION                NvmeCompletion;
    AMI_NVME_CONTROLLER_PROTOCOL              *NvmeController =\
       ((NVM_EXPRESS_PASS_THRU_INSTANCE*)EfiNvmExpressPassThru)->NvmeControllerProtocol;
    UINT16                                    SecurityStatus;
    // Check Nvme device support Security commands
    if( !(NvmeController->IdentifyControllerData->OACS & 0x1) ) {
        return EFI_SUCCESS;
    }
    // Get the security status of Nvme device
    SecurityStatus = GetNvmeSecurityStatus( EfiNvmExpressPassThru );
    
    // Freeze Lock command will not work with the Nvme in Locked State.
    // So return with Success if the Nvme is either in Locked State or Frozen state.
    if ( SecurityStatus &  (NVME_SECURITY_LOCK_MASK | NVME_SECURITY_FREEZE_MASK) ) {
        return EFI_SUCCESS;
    }
    
    gBS->SetMem( &NvmePacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET), 0 );
    gBS->SetMem( &NvmeCommand, sizeof(EFI_NVM_EXPRESS_COMMAND), 0);
    gBS->SetMem( &NvmeCompletion, sizeof(EFI_NVM_EXPRESS_COMPLETION), 0);

    NvmeCommand.Nsid = 0;
#if defined (USE_MDE_PKG_NVME_PASSTHRU_CHANGES)
    NvmeCommand.Cdw0.Opcode = SECURITY_SEND;
#else
    NvmeCommand.Cdw0.OpCode = SECURITY_SEND;
#endif
    NvmeCommand.Cdw0.FusedOperation = 0;
    NvmeCommand.Cdw10 = (((UINT32)NVME_SECUIRTY_SECP)<<24) | ((UINT32)0x5<<8);  // Nvme FreezeLock command
    NvmeCommand.Cdw11 = 0;

    NvmeCommand.Flags = ( CDW10_VALID | CDW11_VALID );

    NvmePacket.CommandTimeout = NVME_COMMAND_TIMEOUT * 10000000;   
    NvmePacket.NvmeCmd = &NvmeCommand;   
    NvmePacket.QueueType = 0;
    NvmePacket.NvmeCompletion = &NvmeCompletion;
    NvmePacket.TransferBuffer = 0;
    NvmePacket.TransferLength = 0;

    // Send Command through Passthru API
    Status = EfiNvmExpressPassThru->PassThru ( EfiNvmExpressPassThru,
                                               0,
                                               &NvmePacket,
                                               NULL
                                               );
    return Status;
}

/**
    Send the Freeze Lock and device config Freeze Lock command to 
    all the NVMe Devices connected 

    @param Event   - signaled event
    @param Context - pointer to event context

    @retval VOID

**/
VOID
EFIAPI
NvmeFreezeLockDevice (
    IN EFI_EVENT    Event,
    IN VOID         *Context
)
{
    UINTN                   HandleCount;
    EFI_HANDLE              *HandleBuffer;
    UINT8                   i;
    EFI_STATUS              Status;
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL *NvmExpressPassthruProtocol;
    AMI_NVME_CONTROLLER_PROTOCOL       *NvmeController = NULL;

    Status = gBS->LocateHandleBuffer (
                                      ByProtocol,
                                      &gEfiNvmExpressPassThruProtocolGuid,
                                      NULL,
                                      &HandleCount,
                                      &HandleBuffer
                                      );
    if(EFI_ERROR(Status) || HandleCount == 0 ) {
        return;
    }

    //
    // Issue Freeze Lock command all the Nvme Devices connected.
    //
    for (i = 0; i < HandleCount; i++) {
	 
	    // Avoid issuing freeze lock command, if Nvme Passthru protocol is not produced by Ami Nvme driver

	    Status = gBS->HandleProtocol (HandleBuffer[i], &gAmiNvmeControllerProtocolGuid, (VOID**)&NvmeController);
	    if (EFI_ERROR(Status)) {
	        continue;
	    }	
	    
        Status = gBS->HandleProtocol (HandleBuffer[i], &gEfiNvmExpressPassThruProtocolGuid, (VOID**)&NvmExpressPassthruProtocol);
        if (EFI_ERROR(Status)) {
            continue;
        }
        Status = NvmeSendSecurityFreezeLock( NvmExpressPassthruProtocol );
    }

    // Freeze Lock commands send to the device. Close the event as no more it's needed 
    gBS->CloseEvent(Event);

    return;
}

/**
    Send a security protocol command to a device.

    @param  This - Indicates a pointer to the calling context. Type
        EFI_STORAGE_SECURITY_COMMAND_PROTOCOL is defined in the
        EFI_STORAGE_SECURITY_COMMAND_PROTOCOL description.
    @param  MediaId - ID of the medium to send data to.
    @param  Timeout - The timeout, in 100ns units, to use for the execution of the 
        security protocol command. A Timeout value of 0 means that this function 
        will wait indefinitely for the security protocol command to execute.
    @param  If Timeout is greater than zero, then this function will return 
        EFI_TIMEOUT if the time required to execute the receive data command 
        is greater than Timeout.
    @param  SecurityProtocolId - Security protocol ID of the security protocol 
        command to be sent.
    @param  SecurityProtocolSpecificData - Security protocol specific portion of 
        the security protocol command.
    @param  PayloadBufferSize    - Size in bytes of the payload data buffer.
    @param  PayloadBuffer - A pointer to a buffer containing the security protocol 
        command specific payload data for the security protocol command.

         
    @retval EFI_SUCCESS The security protocol command completed successfully.
    @retval EFI_UNSUPPORTED The given MediaId does not support security protocol 
        commands.
    @retval EFI_DEVICE_ERROR The security protocol command completed with an error.
    @retval EFI_INVALID_PARAMETER The PayloadBuffer or PayloadTransferSize is 
        NULL and PayloadBufferSize is non-zero.
    @retval EFI_MEDIA_CHANGED The MediaId is not for the current media. 

**/

EFI_STATUS 
EFIAPI
NvmeSendData (
    IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
    IN UINT32   MediaId,
    IN UINT64   Timeout,
    IN UINT8    SecurityProtocolId,
    IN UINT16   SecurityProtocolSpecificData,
    IN UINTN    PayloadBufferSize,
    IN VOID     *PayloadBuffer  
)
{
    EFI_STATUS                      Status;
    AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController;
    NVME_COMMAND_WRAPPER            *NvmeCmdWrapper;
    NVME_DXE_SMM_DATA_AREA          *NvmeDxeSmmDataArea;
    COMPLETION_QUEUE_ENTRY          CompletionData;
    UINTN                           MappedBufferSize = PayloadBufferSize;
    EFI_PHYSICAL_ADDRESS            MappedBuffer;
    VOID                            *BufferUnMap;
    UINTN                           PRP1TransferSize;

    if (!This || !PayloadBuffer ) {
        return EFI_INVALID_PARAMETER;
    }

    NvmeController = NVME_CONTROLLER_FROM_SECURITY_PROTOCOL(This);
    NvmeCmdWrapper = NvmeController->NvmeCmdWrapper;
    NvmeDxeSmmDataArea = NvmeController->NvmeDxeSmmDataArea;

//  Skip MediaID check since controller command
//  Check if Media ID matches
//  if (BlkMedia->MediaId != MediaId) {
//     return EFI_MEDIA_CHANGED;
//  }
    
    gBS->SetMem(NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER), 0); 
    
    MappedBuffer = (EFI_PHYSICAL_ADDRESS) PayloadBuffer;
    Status = NvmeController->PciIO->Map ( NvmeController->PciIO, 
                                      EfiPciIoOperationBusMasterRead,
                                      PayloadBuffer,
                                      &MappedBufferSize,
                                      &MappedBuffer,
                                      &BufferUnMap
                                      );
      
    if (EFI_ERROR(Status)) {
        return Status;
    }
   
    NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64) MappedBuffer; 
    
    PRP1TransferSize = NvmeController->MemoryPageSize - 
                        (MappedBuffer & ((UINTN)(NvmeController->MemoryPageSize) - 1));
    
    // If all data can be transferred using only PRP1 then do that.
    if (PRP1TransferSize >= MappedBufferSize) {
        PRP1TransferSize = MappedBufferSize;
    }
    
    // Do we need PRP2
    if (MappedBufferSize - PRP1TransferSize) {
        NvmeCmdWrapper->NvmCmd.PRP2 = NvmeCmdWrapper->NvmCmd.PRP1 + PRP1TransferSize;
    }
    
    //Swap SecurityProtocolSpecificData bytes
    NvmeCmdWrapper->NvmCmd.CDW10 = (SecurityProtocolId << 24) + ((SecurityProtocolSpecificData & 0xFF) << 16) +\
                                    (SecurityProtocolSpecificData & 0xFF00);
    NvmeCmdWrapper->NvmCmd.CDW11 = (UINT32)PayloadBufferSize;
            
    // Build NVME command
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = SECURITY_SEND;
    NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.PSDT = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeDxeSmmDataArea->CommandIdentifierAdmin;
    NvmeCmdWrapper->NvmCmd.NSID = 0; 
        
    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;       // Admin Queue      
    NvmeCmdWrapper->CmdTimeOut = (UINT32)(Timeout < 10000 ? 1 : DivU64x32 (Timeout, 10000));  
    
    Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, &CompletionData);
    
    NvmeController->PciIO->Unmap(NvmeController->PciIO, BufferUnMap);
    
    return Status;
    
}

/**
    Send a security protocol command to a device that receives 
    data and/or the result of one or more commands sent by 
    SendData.

    @param  This -Indicates a pointer to the calling context. Type 
        EFI_STORAGE_SECURITY_COMMAND_PROTOCOL is defined in the
        EFI_STORAGE_SECURITY_COMMAND_PROTOCOL description.
    @param  MediaId - ID of the medium to receive data from. 
    @param  Timeout - The timeout, in 100ns units, to use for the execution of the
        security protocol command. A Timeout value of 0 means that this 
        function will wait indefinitely for the security protocol command
        to execute. If Timeout is greater than zero, then this function 
        will return.
    @param  SecurityProtocolId - Security protocol ID of the security protocol 
        command to be sent.
    @param  SecurityProtocolSpecificData - Security protocol specific portion of 
        the security protocol command.
    @param  PayloadBufferSize - Size in bytes of the payload data buffer.
    @param  PayloadBuffer - A pointer to a destination buffer to store the 
        security protocol command specific payload data for the 
        security protocol command. The caller is responsible 
        for either having implicit or explicit ownership of 
        the buffer.
    @param  PayloadTransferSize - A pointer to a buffer to store the size in bytes
        of the data written to the payload data buffer.

         
    @retval EFI_SUCCESS The security protocol command completed successfully.
    @retval EFI_UNSUPPORTED The given MediaId does not support security 
        protocol commands.
    @retval EFI_DEVICE_ERROR The security protocol command completed with an error.
    @retval EFI_INVALID_PARAMETER The PayloadBuffer or PayloadTransferSize is 
        NULL and PayloadBufferSize is non-zero.
    @retval EFI_MEDIA_CHANGED The MediaId is not for the current media. 
        
**/

EFI_STATUS
EFIAPI
NvmeReceiveData (
    IN EFI_STORAGE_SECURITY_COMMAND_PROTOCOL    *This,
    IN UINT32    MediaId,
    IN UINT64   Timeout,
    IN UINT8     SecurityProtocolId,
    IN UINT16   SecurityProtocolSpecificData,
    IN UINTN    PayloadBufferSize,
    OUT VOID    *PayloadBuffer,
    OUT UINTN   *PayloadTransferSize
)
{
    EFI_STATUS                      Status;
    AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController;
    NVME_COMMAND_WRAPPER            *NvmeCmdWrapper;
    NVME_DXE_SMM_DATA_AREA          *NvmeDxeSmmDataArea;
    COMPLETION_QUEUE_ENTRY          CompletionData;
    UINTN                           MappedBufferSize = PayloadBufferSize;
    EFI_PHYSICAL_ADDRESS            MappedBuffer;
    VOID                            *BufferUnMap;
    UINTN                           PRP1TransferSize;
    
    if (!This || !PayloadBuffer || !PayloadTransferSize ) {
        return EFI_INVALID_PARAMETER;
    }

    NvmeController = NVME_CONTROLLER_FROM_SECURITY_PROTOCOL(This);
    NvmeCmdWrapper = NvmeController->NvmeCmdWrapper;
    NvmeDxeSmmDataArea = NvmeController->NvmeDxeSmmDataArea;

//  Skip MediaID check since controller command
//  Check if Media ID matches
//  if (BlkMedia->MediaId != MediaId) {
//     return EFI_MEDIA_CHANGED;
//  }

    gBS->SetMem(NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER), 0); 
    
    MappedBuffer = (EFI_PHYSICAL_ADDRESS) PayloadBuffer;
    Status = NvmeController->PciIO->Map ( NvmeController->PciIO, 
                                      EfiPciIoOperationBusMasterWrite,
                                      PayloadBuffer,
                                      &MappedBufferSize,
                                      &MappedBuffer,
                                      &BufferUnMap
                                      );
      
    if (EFI_ERROR(Status)) {
        return Status;
    }

    NvmeCmdWrapper->NvmCmd.PRP1 = (UINT64) MappedBuffer; 
    
    PRP1TransferSize = NvmeController->MemoryPageSize - 
                        (MappedBuffer & ((UINTN)(NvmeController->MemoryPageSize) - 1));
    
    
    // If all data can be transferred using only PRP1 then do that.
    if (PRP1TransferSize >= MappedBufferSize) {
        PRP1TransferSize = MappedBufferSize;
    }
    
    // Do we need PRP2
    if (MappedBufferSize - PRP1TransferSize) {
        NvmeCmdWrapper->NvmCmd.PRP2 = NvmeCmdWrapper->NvmCmd.PRP1 + PRP1TransferSize;
    }
    
    //Swap SecurityProtocolSpecificData bytes
    NvmeCmdWrapper->NvmCmd.CDW10 = (SecurityProtocolId << 24) + ((SecurityProtocolSpecificData & 0xFF) << 16) +\
                                    (SecurityProtocolSpecificData & 0xFF00);
    NvmeCmdWrapper->NvmCmd.CDW11 = (UINT32)PayloadBufferSize;
            
    // Build NVME command
    NvmeCmdWrapper->NvmCmd.CMD0.Opcode = SECURITY_RECEIVE;
    NvmeCmdWrapper->NvmCmd.CMD0.FusedOperation = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.PSDT = 0;
    NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeDxeSmmDataArea->CommandIdentifierAdmin;
    NvmeCmdWrapper->NvmCmd.NSID = 0;  
        
    NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
    NvmeCmdWrapper->SQIdentifier = 0;       // Admin Queue
    NvmeCmdWrapper->CmdTimeOut = (UINT32)(Timeout < 10000 ? 1 : DivU64x32 (Timeout, 10000));  
    
    Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, &CompletionData);
    
    NvmeController->PciIO->Unmap(NvmeController->PciIO, BufferUnMap);
    
    if (!EFI_ERROR(Status)) {
        *PayloadTransferSize = MappedBufferSize;
    }
    
    return Status;
    
}

/**
    Perform the Shutdown Initialization Sequence for the Nvme Controller 

    1. Stop submitting any new I/O commands to the controller and allow any outstanding commands to complete.
    2. The host should delete all I/O Submission Queues, using the Delete I/O Submission Queue command. 
        A result of the successful completion of the Delete I/O Submission Queue command is that any 
        remaining commands outstanding are aborted.
    3. The host should delete all I/O Completion Queues, using the Delete 
        I/O Completion Queue command.
    4. The host should set the Shutdown Notification (CC.SHN) field to 01b to indicate a normal shutdown operation. 
        The controller indicates when shutdown processing is completed by updating 
        the Shutdown Status (CSTS.SHST) field to 10b.
        
    @param Event   - signaled event
    @param Context - pointer to event context

    @retval EFI_STATUS

**/
VOID
EFIAPI
NvmeShutdown (
    IN EFI_EVENT    Event,
    IN VOID         *Context 
)
{
 
    UINTN                           NvmeHandle;
    UINTN                           Index=0;
    EFI_HANDLE                      *NvmeControllerHandleBuffer;
    AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController = NULL;
    NVME_DXE_SMM_DATA_AREA          *NvmeDxeSmmDataArea = NULL;
    NVME_COMMAND_WRAPPER            *NvmeCmdWrapper;
    COMPLETION_QUEUE_ENTRY          CompletionData;
    EFI_STATUS                      Status;
    UINT32                          Delay;

   
    // Locate all the Nvme Controller Protocol
    Status = gBS->LocateHandleBuffer(
                         ByProtocol,
                         &gAmiNvmeControllerProtocolGuid,
                         NULL,
                         &NvmeHandle,
                         &NvmeControllerHandleBuffer
                         );
    if(EFI_ERROR(Status) || NvmeHandle == 0 ) {
        return ;
    }
    
    Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof(NVME_COMMAND_WRAPPER),
                  (VOID**)&NvmeCmdWrapper
                  );

    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) {
        return ;
    }
    

    for (Index = 0; Index < NvmeHandle; Index++) {
    
        // Get the Nvme Controller Protocol Interface
        Status = gBS->HandleProtocol(
                                NvmeControllerHandleBuffer[Index],
                                &gAmiNvmeControllerProtocolGuid,
                                (VOID *)&NvmeController
                                );
                
        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) {
            continue;
        }

        NvmeDxeSmmDataArea = NvmeController->NvmeDxeSmmDataArea;

        // Clear memory
        SetMem(NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER), 0);
        
        // Delete all I/O Submission Queues, using the Delete I/O Submission Queue command
        // Build NVME command to delete Submission queue
        NvmeCmdWrapper->NvmCmd.CMD0.Opcode = DELETE_IO_SUBMISSION_QUEUE;
        NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeDxeSmmDataArea->CommandIdentifierAdmin;
        NvmeCmdWrapper->NvmCmd.CDW10 = NvmeController->NVMQueueNumber;
        
        NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
        NvmeCmdWrapper->SQIdentifier = 0;           // Queue 0 for Admin cmds
        NvmeCmdWrapper->CmdTimeOut = NVME_COMMAND_TIMEOUT * 1000;
        
        Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, &CompletionData);
        if (EFI_ERROR(Status)) {
            continue;
        }

        // Clear memory
        SetMem(NvmeCmdWrapper, sizeof(NVME_COMMAND_WRAPPER), 0);

        // Delete all I/O Completion Queues, using the Delete I/O Completion Queue command
        // Build NVME command to delete Completion queue
        NvmeCmdWrapper->NvmCmd.CMD0.Opcode = DELETE_IO_COMPLETION_QUEUE;
        NvmeCmdWrapper->NvmCmd.CMD0.CommandIdentifier = NvmeDxeSmmDataArea->CommandIdentifierAdmin;
        NvmeCmdWrapper->NvmCmd.CDW10 = NvmeController->NVMQueueNumber;
        
        NvmeCmdWrapper->AdminOrNVMCmdSet = TRUE;
        NvmeCmdWrapper->SQIdentifier = 0;           // Queue 0 for Admin cmds
        NvmeCmdWrapper->CmdTimeOut = NVME_COMMAND_TIMEOUT * 1000;
        
        Status = ExecuteNvmeCmd (NvmeController, NvmeCmdWrapper, &CompletionData);
        if (EFI_ERROR(Status)) {
            continue;
        }
        
        // Set the Shutdown Notification (CC.SHN) field to 01b to indicate a normal shutdown operation
        CONTROLLER_REG32_OR(NvmeController, Offset_CC, 0x4000);
        
        Delay = NvmeController->TimeOut * 500;
        
        // Check if the shutdown process is completed  
        do {
         
            if ((CONTROLLER_REG32(NvmeController, Offset_CSTS) & 0xC) == 8) {
                break;
            }
            gBS->Stall(1000); // 1msec delay
            
        }while (--Delay);
        
        if (!Delay) {
            ASSERT (FALSE);
        }
    }
  
    // Stop the controller. This will delete all the Queues.
    CONTROLLER_REG32_AND(NvmeController, Offset_CC, 0xFFFFFFFE);
    
    return ;
}

/**
    Prints Identify data structure

    @param IdentifyData, 
    @param ControllerNameSpaceStructure
    
    @retval VOID

    @note  The amount of data that will get printed can be controlled 
    using DEBUG_ERROR_LEVEL_MASK SDL token. 
    Make sure PcdDebugPrintErrorLevel is properly cloned and set to 
    PcdsPatchableInModule in the project.

**/

VOID
PrintIdentifyDataStructure (
    IN  UINT8   *IdentifyData, 
    IN  UINT8   ControllerNameSpaceStructure
)
{

#if  NVME_VERBOSE_PRINT
    
    IDENTIFY_CONTROLLER_DATA    *IdentifyControllerData = (IDENTIFY_CONTROLLER_DATA *)IdentifyData;
    IDENTIFY_NAMESPACE_DATA     *IdentifyNameSpaceData = (IDENTIFY_NAMESPACE_DATA *)IdentifyData;
    UINT32                      *ActiveNameSapceID = (UINT32 *)IdentifyData;
    UINT32                      i;
    UINT64                      FirmwareVersion;
    
    switch (ControllerNameSpaceStructure){
    
        case 1:
            
            FirmwareVersion = *(UINT64 *)(IdentifyControllerData->FirmwareRevision);
            DEBUG((EFI_D_ERROR,"\nIdentify Controller Data Structure\n"));
            DEBUG((EFI_D_VERBOSE, "Vendor ID                                 : %08X\n", IdentifyControllerData->VID));
            DEBUG((EFI_D_VERBOSE, "SubSystem Vendor ID                       : %08X\n", IdentifyControllerData->SSVID));
            DEBUG((EFI_D_VERBOSE, "Firmware Version                          : %16lX\n", FirmwareVersion));
            DEBUG((EFI_D_VERBOSE, "NameSpace Sharing Capability              : %08X\n", IdentifyControllerData->CMIC));
            DEBUG((EFI_D_ERROR,   "Max. Data Transfer Size                   : %08X\n", IdentifyControllerData->MDTS));
            DEBUG((EFI_D_ERROR,   "Controller ID                             : %08X\n", IdentifyControllerData->ControllerID));
            DEBUG((EFI_D_VERBOSE, "Optional Admin Cmd Support                : %08X\n", IdentifyControllerData->OACS));
            DEBUG((EFI_D_VERBOSE, "Abort Command Limit                       : %08X\n", IdentifyControllerData->ACL));
            DEBUG((EFI_D_VERBOSE, "Asyn. Event Request Limit                 : %08X\n", IdentifyControllerData->AERL));
            DEBUG((EFI_D_VERBOSE, "Firmware Updates                          : %08X\n", IdentifyControllerData->FRMW));
            DEBUG((EFI_D_VERBOSE, "Log Page Attribute                        : %08X\n", IdentifyControllerData->LPA));
            DEBUG((EFI_D_VERBOSE, "# of Power state supported                : %08X\n", IdentifyControllerData->NPSS));
            DEBUG((EFI_D_VERBOSE, "Admin Vendor Specific cmd                 : %08X\n", IdentifyControllerData->AVSCC));
            DEBUG((EFI_D_VERBOSE, "Autonomous Power state attrib             : %08X\n", IdentifyControllerData->APSTA));
            DEBUG((EFI_D_ERROR,   "Submission queue Entry Size               : %08X\n", IdentifyControllerData->SQES));
            DEBUG((EFI_D_ERROR,   "Completion queue Entry Size               : %08X\n", IdentifyControllerData->CQES));
            DEBUG((EFI_D_ERROR,   "Number of NameSpace                       : %08X\n", IdentifyControllerData->NN));
            DEBUG((EFI_D_VERBOSE, "Optional NVM Command Support              : %08X\n", IdentifyControllerData->ONCS));
            DEBUG((EFI_D_VERBOSE, "Fused Operation Support                   : %08X\n", IdentifyControllerData->FUSES));
            DEBUG((EFI_D_VERBOSE, "Format NVM Attribute                      : %08X\n", IdentifyControllerData->FNA));
            DEBUG((EFI_D_VERBOSE, "Volatile Write Cache                      : %08X\n", IdentifyControllerData->VWC));
            DEBUG((EFI_D_VERBOSE, "Atomic Write Unit Normal                  : %08X\n", IdentifyControllerData->AWUN));
            DEBUG((EFI_D_VERBOSE, "Atomic Write Unit Power Fail              : %08X\n", IdentifyControllerData->AWUPF));
            DEBUG((EFI_D_VERBOSE, "NVM VS CMD Config                         : %08X\n", IdentifyControllerData->NVSCC));
            DEBUG((EFI_D_VERBOSE, "Atomic Compare & Write Unit               : %08X\n", IdentifyControllerData->ACWU));
            DEBUG((EFI_D_ERROR,   "SGL Support                               : %08X\n", IdentifyControllerData->SGLS));
            break;
            
        case 0:
            DEBUG((EFI_D_ERROR, "\nIdentify NameSpace Data Structure\n"));
            DEBUG((EFI_D_ERROR,  "NameSpace Size                             : %08X\n", IdentifyNameSpaceData->NSIZE));
            DEBUG((EFI_D_ERROR,  "NameSpace Capacity                         : %08X\n", IdentifyNameSpaceData->NCAP));
            DEBUG((EFI_D_ERROR,  "NameSpace Utilization                      : %08X\n", IdentifyNameSpaceData->NUSE));
            DEBUG((EFI_D_VERBOSE,"NameSpace Features                         : %08X\n", IdentifyNameSpaceData->NSFEAT));
            DEBUG((EFI_D_VERBOSE,"No. of LBA Formats (0 Based)               : %08X\n", IdentifyNameSpaceData->NLBAF));
            DEBUG((EFI_D_ERROR,  "Formatted LBA Size                         : %08X\n", IdentifyNameSpaceData->FLBAS));
            DEBUG((EFI_D_ERROR,  "MetaData Capabilities                      : %08X\n", IdentifyNameSpaceData->MC));
            DEBUG((EFI_D_ERROR,  "End-to-end data Protection cap             : %08X\n", IdentifyNameSpaceData->DPC));
            DEBUG((EFI_D_ERROR,  "End-to-end data Protection settings        : %08X\n", IdentifyNameSpaceData->DPS));
            DEBUG((EFI_D_VERBOSE,"NameSpace Sharing                          : %08X\n", IdentifyNameSpaceData->NMIC));
            DEBUG((EFI_D_VERBOSE,"Reservation Capabilities                   : %08X\n", IdentifyNameSpaceData->RESCAP));
            DEBUG((EFI_D_ERROR,  "IEEE Extended Unique Identifier            : %016lx\n", IdentifyNameSpaceData->EUI64));
            for (i=0; i<16; i++){
                DEBUG((EFI_D_VERBOSE, "LBA Format %02X Support                  : %08X\n", i, *(UINT32 *)&(IdentifyNameSpaceData->LBAF[i])));
            }
            break;
            
        case 2:
            DEBUG((EFI_D_ERROR,"\nActive NameSpace IDs\n"));
            for (i=0; i<1024 && ActiveNameSapceID[i]; i++){
                DEBUG((EFI_D_ERROR, "    %08X\n", i, ActiveNameSapceID[i]));
            }    
            
    }
#endif   
    return;
}

/**
    Prints Controller capabilities

        
    @param NvmeController 

    @retval VOID

    @note  The amount of data that will get printed can be controlled 
    using DEBUG_ERROR_LEVEL_MASK SDL token.
    Make sure PcdDebugPrintErrorLevel is properly cloned and set to 
    PcdsPatchableInModule in the project.

**/

VOID
PrintNvmeCapability (
    IN  AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController
)
{
    
#if  NVME_VERBOSE_PRINT
    DEBUG ((EFI_D_ERROR,   "Controller Capabilities Reg Value       : %16lx\n", NvmeController->RawControllerCapabilities));
    DEBUG ((EFI_D_VERBOSE, "Max. Queue Entrys Supported              : %08X\n", NvmeController->MaxQueueEntrySupported));
    DEBUG ((EFI_D_VERBOSE, "Contiguous Queue Required                : %08X\n", NvmeController->ContiguousQueueRequired));
    DEBUG ((EFI_D_VERBOSE, "Arbitration Mode Supported               : %08X\n", NvmeController->ArbitrationMechanismSupport));
    DEBUG ((EFI_D_VERBOSE, "TimeOut in 500msec unit                  : %08X\n", NvmeController->TimeOut));
    DEBUG ((EFI_D_VERBOSE, "Doorbell Stride                          : %08X\n", NvmeController->DoorBellStride));
    DEBUG ((EFI_D_VERBOSE, "NVM Subsystem Reset Support              : %08X\n", NvmeController->NVMResetSupport));
    DEBUG ((EFI_D_VERBOSE, "Command Sets Supported                   : %08X\n", NvmeController->CmdSetsSupported));
    DEBUG ((EFI_D_VERBOSE, "Memory Page Size Min.in Bytes            : %08X\n", NvmeController->MemoryPageSizeMin));
    DEBUG ((EFI_D_VERBOSE, "Memory Page Size Max.in Bytes            : %08X\n", NvmeController->MemoryPageSizeMax));
#endif

    return;
}

VOID
PrintNvmeMassDevInfo (
    NVME_MASS_DEV_INFO              *NvmeMassDevInfo
)
{
    
#if  NVME_VERBOSE_PRINT    
    DEBUG((EFI_D_VERBOSE,"********** NvmeMassDevInfo **********\n"));
    DEBUG((EFI_D_VERBOSE, "wBlockSize                                  : %08X\n", NvmeMassDevInfo->wBlockSize));
    DEBUG((EFI_D_VERBOSE, "dMaxLba                                     : %lX\n",  NvmeMassDevInfo->dMaxLba));
    DEBUG((EFI_D_VERBOSE, "bHeads                                      : %08X\n", NvmeMassDevInfo->bHeads));
    DEBUG((EFI_D_VERBOSE, "bSectors                                    : %08X\n", NvmeMassDevInfo->bSectors));
    DEBUG((EFI_D_VERBOSE, "wCylinders                                  : %08X\n", NvmeMassDevInfo->wCylinders));
    DEBUG((EFI_D_VERBOSE, "bNonLBAHeads                                : %08X\n", NvmeMassDevInfo->bNonLBAHeads));
    DEBUG((EFI_D_VERBOSE, "bNonLBASectors                              : %08X\n", NvmeMassDevInfo->bNonLBASectors));
    DEBUG((EFI_D_VERBOSE, "wNonLBACylinders                            : %08X\n", NvmeMassDevInfo->wNonLBACylinders));
#endif  
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
