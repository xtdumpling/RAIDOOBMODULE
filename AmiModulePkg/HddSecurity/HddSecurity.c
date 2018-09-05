//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file HddSecurity.c
    HDD Security Support

**/

//---------------------------------------------------------------------------

#include "HddSecurity.h"
#include <Protocol/AmiHddSecurityInit.h>

//---------------------------------------------------------------------------

static EFI_GUID     gIdeSecurityPwNvguid = IDE_SECURITY_PWNV_GUID;

extern EFI_DRIVER_BINDING_PROTOCOL gIdeBusDriverBinding;
extern EFI_EVENT                   gIDEBusEvtMiscSmmFeatures;
EFI_EVENT                          BootEvent = NULL;
EFI_EVENT                          gHddEfiBootScriptEvt = NULL;
EFI_EVENT                          gHddLegacyBootScriptEvt = NULL;

const CHAR16                       *sOemMasterPasswordTag
    = CONVERT_TO_WSTRING( OEM_MASTER_PASSWORD_TAG );
const CHAR16                       *sDefaultMasterPasswordTag
    = CONVERT_TO_WSTRING( DEFAULT_MASTER_PASSWORD_TAG );

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
EFI_SMM_CONTROL2_PROTOCOL          *mSmmCtl = NULL;
#else
EFI_SMM_CONTROL_PROTOCOL           *mSmmCtl = NULL;
#endif

#if SMMHDDSECURITY_SUPPORT
HDD_PASSWORD                       *HDDPasswordBuffer = NULL;
#endif

AMI_HDD_SECURITY_INIT_PROTOCOL *gHddSecurityInitProtocol;

// Global variable
EFI_SMM_COMMUNICATION_PROTOCOL  *gSmmCommunication = NULL;

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )

EFI_EVENT gNvmeEvtBootScript = NULL;
EFI_S3_SAVE_STATE_PROTOCOL      *NvmeBootScriptSave=NULL;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo=NULL;

BOOT_SCRIPT_NVME_SECURITY_SAVE_ENTRY    gNvmeSaveEntry[MAX_NVME_DEVICES];
UINT32                                  gNvmeSaveEntryCount = 0;
BOOT_SCRIPT_NVME_SECURITY_SAVE_ENTRY    *gNvmeSmmTransferBuffer = NULL;

// PciCfg register values for PCI root bridge
BOOT_SCRIPT_NVME_REG_SAVE gPciRootBridgeRegistersSave[] = {
   PCI_ROOT_BRIDGE_BUS, EfiBootScriptWidthUint32, 0xffffffff, // 0x18
   PCI_ROOT_BRIDGE_MEM, EfiBootScriptWidthUint32, 0xffffffff, // 0x20
   PCI_ROOT_BRIDGE_CMD, EfiBootScriptWidthUint8,  0xff,       // 0x04
};

// PciCfg register values for NVMe device
BOOT_SCRIPT_NVME_REG_SAVE gNvmeControllerRegistersSave[] = {
   NVME_CONTROLLER_BAR0, EfiBootScriptWidthUint32, 0xffffffff, // 0x10
   NVME_CONTROLLER_BAR1, EfiBootScriptWidthUint32, 0xffffffff, // 0x14
   NVME_CONTROLLER_BAR2, EfiBootScriptWidthUint32, 0xffffffff, // 0x18
   NVME_CONTROLLER_BAR3, EfiBootScriptWidthUint32, 0xffffffff, // 0x1C
   NVME_CONTROLLER_BAR4, EfiBootScriptWidthUint32, 0xffffffff, // 0x20
   NVME_CONTROLLER_BAR5, EfiBootScriptWidthUint32, 0xffffffff, // 0x24
   NVME_CONTROLLER_CMD,  EfiBootScriptWidthUint8,  0xff,       // 0x04
};

/**
    It uses EFI_SMM_COMMUNICATION_PROTOCOL API to transfer data from Non-SMM
    mode NVME BootScript Data to SMM mode.

    @param pData - Pointer to data to be transfered
    @param DataSize - Size of the data
    @param pGuid  - Pointer to GUID identifier for the data
        
    @retval EFI_STATUS

**/
EFI_STATUS
TransferNvmeBootScriptDataToSmm ( 
    IN  VOID    *pData, 
    IN  UINTN   DataSize, 
    IN  EFI_GUID *pGuid 
)
{
    EFI_SMM_COMMUNICATION_PROTOCOL *gSmmCommunication = NULL;
    EFI_SMM_COMMUNICATE_HEADER     *SmmCommunicateHeader;
    UINT8                          *CommunicateBuffer = NULL;
    UINTN                          CommunicateBufferSize;
    EFI_STATUS                     Status;

    if ( pData == NULL || DataSize == 0 || pGuid == NULL ) {
        return EFI_INVALID_PARAMETER;
    }

    // Calculate Size of Communication buffer
    CommunicateBufferSize  = (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data)) + DataSize; // Header size (without data) + data size

    // Allocate memory for Communication Buffer.   
    Status = pBS->AllocatePool( EfiBootServicesData,
                                CommunicateBufferSize,
                                (VOID**)&CommunicateBuffer );
    if ( EFI_ERROR( Status )) {  
        ASSERT_EFI_ERROR(Status);
        return Status;
    }    

    // Copy SMM Communicate Header Here
    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)CommunicateBuffer;

    // Copy data GUID
    pBS->CopyMem( &SmmCommunicateHeader->HeaderGuid, pGuid, sizeof( EFI_GUID ) );

    // Updated data length
    SmmCommunicateHeader->MessageLength  = DataSize;

    // Copy Data Here
    pBS->CopyMem( &SmmCommunicateHeader->Data, &pData, sizeof(pData) );

    // Locate EFI_SMM_COMMUNICATION_PROTOCOL
    Status = pBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &gSmmCommunication);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;  
    }

    // Send data to SMM using protocol API
    Status = gSmmCommunication->Communicate (gSmmCommunication, CommunicateBuffer, &CommunicateBufferSize);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;  
    }

    // Free memory allocated for Communication Buffer.
    Status = pBS->FreePool(CommunicateBuffer);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;  
    }
    
    return EFI_SUCCESS;
}


/**
    Copies PciRegisters from NvmeController or PCI Root Bridge devices 
    to S3 Buffer Structure for saving Data in Bootscript for S3 Resume.

    @param PciAddress - Physical Pci Address of register to be stored
    @param Width - Width of the data to be stored
    @param Value  - Value to be saved for Bootscript
        
    @retval EFI_STATUS

**/
EFI_STATUS
CopyPciDataToS3BufferStructure ( 
  IN    UINTN                           PciAddr,
  IN    EFI_BOOT_SCRIPT_WIDTH           Width,
  IN    UINTN                           Value
){

    TRACE((-1,"\n CopyPciDataToS3BufferStructure - %x %x %x %x", gNvmeSaveEntryCount, PciAddr, Width, Value ));
    
    if( gNvmeSaveEntryCount >= MAX_NVME_DEVICES ) {
        ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
        return EFI_OUT_OF_RESOURCES;
    }

    // Save the bootScript save entries to the array list
    gNvmeSaveEntry[gNvmeSaveEntryCount].PciAddress = PciAddr;
    gNvmeSaveEntry[gNvmeSaveEntryCount].Width = Width;
    gNvmeSaveEntry[gNvmeSaveEntryCount].Value = Value;
    gNvmeSaveEntryCount++;
    
    return EFI_SUCCESS;
}

#if SAVE_PCI_ROOTBRIDGE_REGS
/**
    Function stores PCI root bridge BAR,CMD, BUS PCI Configuration values

    @param BusNum
    @param DevNum
    @param FuncNum

    @retval EFI_STATUS

**/

EFI_STATUS
StorePciRootBridgePciCfgRegs (
    UINTN             BusNum,
    UINTN             DevNum,
    UINTN             FuncNum
)
{
    EFI_STATUS    Status;
    UINTN         Data32;
    UINTN         i;
    UINTN         RbPciAddress;
    
    EFI_GUID gPciRootBridgeIoProtocolGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;

    if( !PciRootBridgeIo ) {
        Status = pBS->LocateProtocol(
                      &gPciRootBridgeIoProtocolGuid,
                      NULL,
                      (void**) &PciRootBridgeIo
                      );

        if (EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR (Status);
            return Status;
        }
    }

    for (i = 0; i < sizeof(gPciRootBridgeRegistersSave)/sizeof(BOOT_SCRIPT_NVME_REG_SAVE); ++i ) {

        RbPciAddress = EFI_PCI_CFG_ADDRESS (BusNum, DevNum, FuncNum, gPciRootBridgeRegistersSave[i].RegNum);


        PciRootBridgeIo->Pci.Read(
                PciRootBridgeIo,
                gPciRootBridgeRegistersSave[i].Width,
                RbPciAddress,
                1,
                &Data32 );
        
        Data32 &= gPciRootBridgeRegistersSave[i].Mask;

        // Copy Root Bridge Pci Data to be saved in BootScript to entry list one by one to transfer to Smm
        Status = CopyPciDataToS3BufferStructure(RbPciAddress, gPciRootBridgeRegistersSave[i].Width, Data32);
        
        if (EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR (Status);
            return Status;
        }
    }

    return EFI_SUCCESS;
}
#endif

/**
    Callback Funtion to register Boot script for Nvme devices in order initialize Nvme controller
    in S3 resume path
    
    @param Event
    @param Context
    
    @retval VOID
**/

VOID
NvmeCallbackBootScript (
    IN  EFI_EVENT Event,
    IN  VOID      *Context
)
{
    EFI_STATUS                      Status;
    UINTN                           PciSeg, PciBus, PciDev, PciFunc;
    EFI_PCI_IO_PROTOCOL             *PciIO;
    UINTN                           Data32 = 0;
    UINT32                          i;
    UINT32                          Index;
    UINTN                           HandleCount;
    EFI_HANDLE                      *HandleBuffer;
    UINTN                           PciAddress;
#if SAVE_PCI_ROOTBRIDGE_REGS
    EFI_HANDLE                      MatchedDevicePathHandle;
    EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
    EFI_DEVICE_PATH_PROTOCOL        *DevicePathNode;
    EFI_DEVICE_PATH_PROTOCOL        *HeadDevicePath = NULL;
    EFI_DEVICE_PATH_PROTOCOL        *TempHeadDevicePath;
#endif

    pBS->CloseEvent( Event );
        
    Status = pBS->LocateProtocol(
                  &gEfiS3SaveStateProtocolGuid,
                  NULL,
                  (VOID**)&NvmeBootScriptSave
                  );

    if ( EFI_ERROR( Status )) {
        ASSERT_EFI_ERROR (Status);
        return;
    }

    Status = pBS->LocateHandleBuffer(
                  ByProtocol,
                  &gAmiNvmeControllerProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                 );

    if ( EFI_ERROR( Status )) {
        return;
    }

    for(Index = 0; Index < HandleCount; Index++) {
        
#if SAVE_PCI_ROOTBRIDGE_REGS
        // Get the Device path Protocol interface
        Status = pBS->HandleProtocol( HandleBuffer[Index],
                                      &gEfiDevicePathProtocolGuid,
                                      (void**) &DevicePath
                                      );
    
        if(!EFI_ERROR(Status)) {
        
            DevicePathNode = DevicePath;
    
            for ( ;!isEndNode(DevicePathNode); DevicePathNode = NEXT_NODE (DevicePathNode)) {
    
                TempHeadDevicePath = HeadDevicePath;
                HeadDevicePath = AppendDevicePathNode(HeadDevicePath,DevicePathNode);
    
                if(TempHeadDevicePath != NULL) {
                    pBS->FreePool(TempHeadDevicePath);
                }
    
                // Check to identify the PCI device Path
                if ((DevicePathNode->Type == HARDWARE_DEVICE_PATH) &&
                            (DevicePathNode->SubType == HW_PCI_DP)) {
    
                    TempHeadDevicePath = HeadDevicePath;
    
                    Status = pBS->LocateDevicePath( &gEfiDevicePathProtocolGuid,
                                                    &TempHeadDevicePath,
                                                    &MatchedDevicePathHandle
                                                    );
                    if ( EFI_ERROR( Status )) {
                        ASSERT_EFI_ERROR (Status);
                        continue;
                    }
    
                    Status = pBS->HandleProtocol( MatchedDevicePathHandle,
                                                  &gEfiPciIoProtocolGuid,
                                                  (void **) &PciIO
                                                  );
                    if ( EFI_ERROR( Status )) {
                        ASSERT_EFI_ERROR (Status);
                        continue;
                    }
    
                    Status = PciIO->GetLocation(PciIO, &PciSeg, &PciBus, &PciDev, &PciFunc);
                    if ( EFI_ERROR( Status )) {
                        ASSERT_EFI_ERROR (Status);
                        continue;
                    }
                    
                    Status = PciIO->Pci.Read(PciIO, 
                                             EfiPciIoWidthUint32, 
                                             PCI_RID, 
                                             1, 
                                             &Data32);
                    if(EFI_ERROR(Status)) {
                        ASSERT_EFI_ERROR (Status);
                        continue;
                    }
    
                    // Check Class code to identify whether it is a PCI root bridge device or not
                    if((Data32>>24) == PCI_CL_BRIDGE) {
                        // Store PCI root bridge device PCI configuration values
                        Status = StorePciRootBridgePciCfgRegs(PciBus,PciDev,PciFunc);

                        if ( EFI_ERROR( Status )) {
                            ASSERT_EFI_ERROR (Status);
                            return;
                        }
                    }
                }
            }
        } else {
            ASSERT_EFI_ERROR (Status);
            continue;
        }
#endif
        
        // Get the PCIIO protocol installed on the handle
        Status = pBS->HandleProtocol( HandleBuffer[Index],
                                      &gEfiPciIoProtocolGuid,
                                      (void**) &PciIO
                                      );
    
        if ( EFI_ERROR( Status )) {
            ASSERT_EFI_ERROR (Status);
            continue;
        }
    
        Status = PciIO->GetLocation(PciIO, &PciSeg, &PciBus, &PciDev, &PciFunc);
        if ( EFI_ERROR( Status )) {
            ASSERT_EFI_ERROR (Status);
            continue;
        }
        
        Status = PciIO->Pci.Read( \
                        PciIO, \
                        EfiBootScriptWidthUint32, \
                        PCI_VID, \
                        1, \
                        &Data32);
    
        if ( EFI_ERROR( Status )) {
            ASSERT_EFI_ERROR (Status);
            continue;
        }
        
        if (Data32 != 0xffffffff) {
            for (i = 0; i < sizeof(gNvmeControllerRegistersSave)/sizeof(BOOT_SCRIPT_NVME_REG_SAVE); ++i) {
                Status = PciIO->Pci.Read( \
                         PciIO, \
                         gNvmeControllerRegistersSave[i].Width, \
                         gNvmeControllerRegistersSave[i].RegNum, \
                         1, \
                         &Data32);
                if ( EFI_ERROR( Status )) {
                    ASSERT_EFI_ERROR (Status);
                    continue;
                }
                
                Data32 &= gNvmeControllerRegistersSave[i].Mask;
                
                PciAddress =   EFI_PCI_CFG_ADDRESS( PciBus, PciDev, PciFunc, gNvmeControllerRegistersSave[i].RegNum);

                // Copy Nvme Pci Data to be saved in BootScript to entry list one by one to transfer to Smm
                Status = CopyPciDataToS3BufferStructure(PciAddress, gNvmeControllerRegistersSave[i].Width, Data32);

                if (EFI_ERROR(Status)) {
                    ASSERT_EFI_ERROR (Status);
                    return;
                }
            }
        }
    }

    // Allocate a buffer that will hold the entry list of BootScript saved along with the Nvme BootScript Signature
    // and Total bootscript entry count @ the end
    //          Buffer will be as of below
    
    //  ===============================     ======================================== 
    //          Buffer entries                              Size  
    //  ===============================     ========================================  
    //       NvmeSaveEntry[0]                   sizeof(BOOT_SCRIPT_NVME_REG_SAVE)
    //       NvmeSaveEntry[1]                   sizeof(BOOT_SCRIPT_NVME_REG_SAVE)
    //       NvmeSaveEntry[2]                   sizeof(BOOT_SCRIPT_NVME_REG_SAVE)
    //          .
    //          .
    //          .
    //       NvmeSaveEntry[n]                   sizeof(BOOT_SCRIPT_NVME_REG_SAVE)  where n is Total Entry Count
    //       NVME Boot Script Signature         UINT32   ('n','s','b','s')
    //       gNvmeSaveEntryCount                UINT32
    //
    Status = pBS->AllocatePool( EfiBootServicesData,
                                sizeof(BOOT_SCRIPT_NVME_SECURITY_SAVE_ENTRY)*(gNvmeSaveEntryCount) + 
                                2*sizeof(UINT32),
                                (VOID**)&gNvmeSmmTransferBuffer );
    
    if ( EFI_ERROR( Status )) {
        ASSERT_EFI_ERROR (Status);
        return;
    }

    // Copy the saved entry list to the buffer allocated
    pBS->CopyMem( gNvmeSmmTransferBuffer, gNvmeSaveEntry, sizeof(BOOT_SCRIPT_NVME_SECURITY_SAVE_ENTRY)*(gNvmeSaveEntryCount));

    // Append the Signature and Bootscript Entry Count to the buffer.
    *(UINT32*)(&gNvmeSmmTransferBuffer[gNvmeSaveEntryCount]) = NVME_SECURITY_SAVE_SIGNATURE;
    *(UINT32*)(((UINT8*)&gNvmeSmmTransferBuffer[gNvmeSaveEntryCount]) + sizeof(UINT32)) = gNvmeSaveEntryCount;

    // Transfer NVMe boot script buffer with size to communication buffer to trasfer to SMM 
    TransferNvmeBootScriptDataToSmm ( gNvmeSmmTransferBuffer,
                                      sizeof(BOOT_SCRIPT_NVME_SECURITY_SAVE_ENTRY)*(gNvmeSaveEntryCount) + 
                                      2*sizeof(UINT32), 
                                      &gNvmeSecurityBootScriptSaveTriggerGuid );

    // Free the unused memory
    pBS->FreePool(gNvmeSmmTransferBuffer);
    pBS->FreePool(HandleBuffer);
    return;

}
#endif // End of NVMe_SUPPORT

/**
    Checks whether the device supports Security mode or not.
    If yes, installs SecurityProtocol.

    @param BusInterface 
    @param BusInterfaceType 

    @retval EFI_SUCCESS                      : Security Mode supported.
    @retval EFI_DEVICE_ERROR/EFI_UNSUPPORTED : Not supported.

**/

EFI_STATUS
EFIAPI
InstallSecurityInterface (
    IN  VOID                       *BusInterface,
    IN  AMI_STORAGE_BUS_INTERFACE  BusInterfaceType
)
{
    EFI_STATUS                      Status;
    AMI_HDD_SECURITY_INTERFACE      *SecurityInterface;
    EFI_HANDLE                      DeviceHandle;
    IDENTIFY_DATA                   IdentifyData;
    BOOLEAN                         IsSAT3Supported = FALSE;
    BOOLEAN                         IsTCGSupported = FALSE;
    
#if MASTER_PASSWORD_ENABLE
    UINT32                          IdePasswordFlags = 0;
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL            *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE           *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    AMI_NVME_CONTROLLER_PROTOCOL    *NvmeDevInterface = (AMI_NVME_CONTROLLER_PROTOCOL*)BusInterface;
    IDENTIFY_CONTROLLER_DATA        *NvmeIdentifyData;
    UINT16                          NvmeSecurityStatus=0;
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        IdentifyData    = IdeBusInterface->IdeDevice.IdentifyData;
        DeviceHandle = IdeBusInterface->IdeDeviceHandle;
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        IdentifyData    = SataDevInterface->IdentifyData;
        DeviceHandle = SataDevInterface->IdeDeviceHandle;
    }
#endif
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceNvme ) {
        NvmeIdentifyData  = NvmeDevInterface->IdentifyControllerData;
        DeviceHandle = NvmeDevInterface->ControllerHandle;
    }
#endif

    if  (BusInterfaceType <= AmiStorageInterfaceAhci) {
	    if(IdentifyData.Command_Set_Supported_82 & 0x2) {
            IsSAT3Supported = TRUE;  
			if  (BusInterfaceType == AmiStorageInterfaceAhci) { 
		        //Check whether the device supports TCG security protocol         
                Status = IsSecurityProtocolSupported(BusInterface, BusInterfaceType, &IsSAT3Supported, &IsTCGSupported );
			}
	    } else {
	        return EFI_UNSUPPORTED;
	    }	    
    } else if(BusInterfaceType == AmiStorageInterfaceNvme ) {
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )

    	// Check the Security Send and Receive command Support
        if( (NvmeIdentifyData->OACS & 0x1) == 0) {
            return EFI_UNSUPPORTED;
        }
        Status = IsNvmeSecurityProtocolSupported(NvmeDevInterface->EfiNvmExpressPassThru, &IsSAT3Supported, &IsTCGSupported );
#endif 
    }

    // Some devices returns error for the SECURITY_PROTOCOL_INFORMATION command once the device gets locked
    // In case of error assuming that device supports SAT3 security command and proceed.
    if (!(EFI_ERROR(Status))) {
        
    	// if device does not support SAT security protocol then return.
        if(!IsSAT3Supported ) {
        	return EFI_UNSUPPORTED;
        }

// if device supports both SAT and TCG security protocol
// Then proceed based on SELECT_SAT_OPAL_SECURITY_PROTOCOL SDL token
#if (defined(SELECT_SAT3_OPAL_SECURITY_PROTOCOL) \
      && !SELECT_SAT3_OPAL_SECURITY_PROTOCOL) 
        if( IsTCGSupported ) {
            return EFI_UNSUPPORTED;
        }
#endif
    }
    
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
   if(BusInterfaceType == AmiStorageInterfaceNvme ) {
        //  Update the Identify device buffer
        Status= UpdateIdentifyData( NvmeDevInterface->EfiNvmExpressPassThru, BusInterfaceType );
        
        // If the device doesn't support getting the Security Protocol 0xEF information
        // Return with not supported
        if(EFI_ERROR(Status)) {
            return EFI_UNSUPPORTED;
        }        
        // Return Nvme device security status
        NvmeSecurityStatus = GET_NVME_SEC_RECEIVE_BUFFER_ELEMENT( NvmeDevInterface->EfiNvmExpressPassThru,\
                                                                  NvmeSecStatus );
        
        // Check if the Security is supported
        if((NvmeSecurityStatus & 01) == 0) {
        	return EFI_UNSUPPORTED;
        }
   }
#endif

    // Return error if gAmiHddSecurityProtocolGuid is already installed on the Device Handle
    Status = pBS->OpenProtocol ( DeviceHandle, 
                                &gAmiHddSecurityProtocolGuid, 
                                NULL, 
                                NULL, 
                                NULL, 
                                EFI_OPEN_PROTOCOL_TEST_PROTOCOL );
    if(!EFI_ERROR(Status)) {
        return EFI_ALREADY_STARTED;
    }

    Status = pBS->AllocatePool( EfiBootServicesData,
                                sizeof(AMI_HDD_SECURITY_INTERFACE),
                                (VOID**)&SecurityInterface );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde )
    {
        IdeBusInterface->IdeSecurityInterface = (AMI_HDD_SECURITY_PROTOCOL*)SecurityInterface;
        SecurityInterface->BusInterface    = IdeBusInterface;
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        SataDevInterface->IdeSecurityInterface = (AMI_HDD_SECURITY_PROTOCOL*)SecurityInterface;
        SecurityInterface->BusInterface     = SataDevInterface;
    }
#endif

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceNvme ) {
        NvmeDevInterface->SecurityInterface = SecurityInterface;
        SecurityInterface->BusInterface     = NvmeDevInterface->EfiNvmExpressPassThru;
    }
#endif

    SecurityInterface->ReturnSecurityStatus    = ReturnSecurityStatus;
    SecurityInterface->SecuritySetPassword     = SecuritySetPassword;
    SecurityInterface->SecurityUnlockPassword  = SecurityUnlockPassword;
    SecurityInterface->SecurityDisablePassword = SecurityDisablePassword;
    SecurityInterface->SecurityFreezeLock      = SecurityFreezeLock;
    SecurityInterface->SecurityEraseUnit       = SecurityEraseUnit;
    SecurityInterface->ReturnIdePasswordFlags  = ReturnIdePasswordFlags;

    // Set BusInterfaceType to FALSE if IdeBus else TRUE for Ahci Bus.
    SecurityInterface->BusInterfaceType = BusInterfaceType;  // Ide Bus

    // Update the Identify device buffer before installing the protocol as 
    // Other driver can use the Protocol interface on a callback.
    UpdateIdentifyData( SecurityInterface->BusInterface, BusInterfaceType );

    //  Install the protocol on the device
    Status = pBS->InstallMultipleProtocolInterfaces(
        &DeviceHandle,
        &gAmiHddSecurityProtocolGuid, SecurityInterface,
        NULL );
    ASSERT_EFI_ERROR( Status );
    

#if SMMHDDSECURITY_SUPPORT

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    if ( (gNvmeEvtBootScript == NULL) && (BusInterfaceType == AmiStorageInterfaceNvme) ) {
        // Create event for NVMe boot script
        Status = CreateReadyToBootEvent(\
                 TPL_NOTIFY,\
                 NvmeCallbackBootScript,\
                 NULL,\
                 &gNvmeEvtBootScript\
                 );
        ASSERT_EFI_ERROR(Status);
    }
#endif


    if ( gHddEfiBootScriptEvt == NULL )
    {
        // Create ExitBootServices event for boot script
        Status = pBS->CreateEvent (
                EFI_EVENT_SIGNAL_EXIT_BOOT_SERVICES,
                TPL_NOTIFY,
                CallbackFuncToStoreBootScriptForS3Resume,
                NULL,
                &gHddEfiBootScriptEvt
                );
        ASSERT_EFI_ERROR( Status );
    }

#if CSM_SUPPORT
    if ( gHddLegacyBootScriptEvt == NULL )
    {
        // Create legacy boot event for boot script
        Status = CreateLegacyBootEvent(
            TPL_NOTIFY,
            CallbackFuncToStoreBootScriptForS3Resume,
            NULL,
            &gHddLegacyBootScriptEvt
            );
        ASSERT_EFI_ERROR( Status );
    }
#endif

    // Send data to SMM.
    if( !gSmmCommunication ) {
        Status = pBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &gSmmCommunication);
        if (EFI_ERROR(Status)) {
            gSmmCommunication=NULL;
        }
    }

    // Install the Readytoboot event to locate Smm Protocol for HddSecurity devices
    if ((BootEvent == NULL) && (BusInterfaceType)) {
        
        //
        // Create event for AhciSmm Services
        //
        Status = CreateReadyToBootEvent(
            TPL_CALLBACK,
            LocateAhciSmmServiceEvent,
            NULL,
            &BootEvent
            );
        ASSERT_EFI_ERROR( Status );
    }
#endif

#if MASTER_PASSWORD_ENABLE
    // Check if OEM_DEFAULT_MASTER_PASSWORD is enabled and
    // DISPLAY_MASTER_PASSWORD is Disabled
    Status = ReturnIdePasswordFlags(
        (AMI_HDD_SECURITY_PROTOCOL*)SecurityInterface,
        &IdePasswordFlags
        );

    ASSERT_EFI_ERROR( Status );

    if ((IdePasswordFlags & 0x0800) && (!(IdePasswordFlags & 0x0008)))
    {
        Status = OEMSetMasterPassword(
            (AMI_HDD_SECURITY_PROTOCOL*)SecurityInterface );
        ASSERT_EFI_ERROR( Status );
    }
#endif

    return EFI_SUCCESS;
}

/**
    Stops the security mode support by uninstalling the
    security interface.

        
    @param BusInterface 
    @param BusInterfaceType 

    @retval 
        EFI_SUCCESS         : Security Mode supported
        EFI_DEVICE_ERROR    : Not supported

**/

EFI_STATUS
EFIAPI
StopSecurityModeSupport (
    IN  VOID                       *BusInterface,
    IN  AMI_STORAGE_BUS_INTERFACE  BusInterfaceType
)
{
    EFI_STATUS                  Status;
    EFI_HANDLE                  DeviceHandle;
    AMI_HDD_SECURITY_INTERFACE  *SecurityInterface = NULL;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL        *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE       *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    AMI_NVME_CONTROLLER_PROTOCOL    *NvmeDevInterface = (AMI_NVME_CONTROLLER_PROTOCOL*)BusInterface;
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        SecurityInterface = (AMI_HDD_SECURITY_INTERFACE*)IdeBusInterface->IdeSecurityInterface;
        DeviceHandle = IdeBusInterface->IdeDeviceHandle;
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        SecurityInterface = (AMI_HDD_SECURITY_INTERFACE*)SataDevInterface->IdeSecurityInterface;
        DeviceHandle = SataDevInterface->IdeDeviceHandle;
    }
#endif
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceNvme ) {
        SecurityInterface = (AMI_HDD_SECURITY_INTERFACE*)NvmeDevInterface->SecurityInterface;
        DeviceHandle = NvmeDevInterface->ControllerHandle;
    }
#endif

    if ( SecurityInterface == NULL ) {
        return EFI_SUCCESS;
    }

    //  Install Devicepath and BLOCK_IO on a new handle
    Status = pBS->UninstallMultipleProtocolInterfaces(
        DeviceHandle,
        &gAmiHddSecurityProtocolGuid,
        SecurityInterface,
        NULL );

    if ( Status == EFI_SUCCESS ) {
        pBS->FreePool( SecurityInterface );
        SecurityInterface = NULL;
    }

    return Status;
}
#if SMMHDDSECURITY_SUPPORT
/**
    Triggers Smm Communication event in ReadytoBoot Event callback that will 
    save SwSmi port and value for S3 Resume Hdd Password Unlock

        
    @param Event 
    @param Context 

         
    @retval VOID

    @note  Will be called only if HDD loses power in S3 state. A SW SMI will
        be created during S3 resume, to unlock the HDD and put it in
        Freeze state.

**/
VOID
EFIAPI
CallbackFuncToStoreBootScriptForS3Resume (
    IN  EFI_EVENT Event,
    IN  VOID      *Context
)
{
    EFI_SMM_COMMUNICATE_HEADER  *SmmCommunicateHeader=NULL;
    UINT8                       *CommunicateBuffer = NULL;
    UINTN                       CommunicateBufferSize;
    EFI_STATUS                  Status;
    static BOOLEAN              BootScriptStored = FALSE;

    if( BootScriptStored ) {
        //Kill the Event
        pBS->CloseEvent( Event );
        return;
    }

    // Calculate Size of Communication buffer
    CommunicateBufferSize  = SMM_COMMUNICATE_HEADER_SIZE + sizeof (UINT8 *);

    //
    // Allocate Memory for Communication buffer.   
    //
    Status = pBS->AllocatePool( EfiBootServicesData,
                                CommunicateBufferSize,
                                (VOID**)&CommunicateBuffer );
    if ( EFI_ERROR( Status )) {
        return ;
    }
    pBS->SetMem( CommunicateBuffer, CommunicateBufferSize, 0 );

    //
    // Copy Smm Communicate Header Here
    //
    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)CommunicateBuffer;
    pBS->CopyMem( &SmmCommunicateHeader->HeaderGuid, &gHddSecurityBootScriptSaveTriggerGuid, sizeof( EFI_GUID ) );
    SmmCommunicateHeader->MessageLength  = sizeof(UINT8 *);

    if(gSmmCommunication == NULL) {
        Status = pBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &gSmmCommunication);
        if (EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            return ;  
        }
    }

    //
    // Send data to SMM.
    //
    Status = gSmmCommunication->Communicate (gSmmCommunication, CommunicateBuffer, &CommunicateBufferSize);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return ;  
    }

    // Freeing the Memory Allocated for Communication Buffer.
    Status = pBS->FreePool(CommunicateBuffer);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
    }

    BootScriptStored = TRUE;

    //Kill the Event
    pBS->CloseEvent( Event );

    return;

}
#endif

/**
    This function is the entry point for IDE SECURITY Driver.

    @param 
        EFI_HANDLE          ImageHandle     Image handle.
        EFI_SYSTEM_TABLE    *SystemTable    Pointer to the EFI system table.

    @retval 
        EFI_SUCCESS         Security interface installed.
        EFI_ERROR           Security interface not installed or unable to locate
        some protocol.

    @note  
  Entry Points are used to locate or install protocol interfaces and
  notification events.

**/

EFI_STATUS
EFIAPI
HddSecurityEntryPoint (
    IN  EFI_HANDLE        ImageHandle,
    IN  EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_HANDLE Handle = NULL;
    EFI_STATUS Status;

    InitAmiLib( ImageHandle, SystemTable );

    Status = pBS->AllocatePool( EfiBootServicesData,
                                sizeof(AMI_HDD_SECURITY_INIT_PROTOCOL),
                                (VOID**)&gHddSecurityInitProtocol );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    Status = pBS->LocateProtocol(&gEfiSmmControl2ProtocolGuid, NULL, (VOID**)&mSmmCtl);
#else
    Status = pBS->LocateProtocol(&gEfiSmmControlProtocolGuid, NULL, (VOID**)&mSmmCtl);
#endif

    if ( EFI_ERROR( Status )) {
        return EFI_UNSUPPORTED;
    }

    gHddSecurityInitProtocol->InstallSecurityInterface = InstallSecurityInterface;
    gHddSecurityInitProtocol->StopSecurityModeSupport  = StopSecurityModeSupport;

    Status = pBS->InstallProtocolInterface(
                        &Handle,
                        &gAmiHddSecurityInitProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        gHddSecurityInitProtocol
                        );
    
    ASSERT_EFI_ERROR( Status );

    return Status;
}

/**
    Returns whether the Device supports Security Mode or not.
    If supported, the current status of the device is returned.
    Security status information will be retrieved from buffer
    updated by IDENTIFY DEVICE(Command Code : 0xEC) command.
        
    @param This 
    @param UINT16       *SecurityStatus

    @retval EFI_SUCCESS : Security Mode supported
    @retval EFI_UNSUPPORTED : Not supported

    @retval UINT16      *SecurityStatus
    @retval             Bit 0 : Security Supported
    @retval             Bit 1 : Security Enabled
    @retval             Bit 2 : Security Locked
    @retval             Bit 3 : Security Frozen
    @retval             Bit 4 : Security Count Expired
    @retval             Bit 5 : Enhanced Security Erase supported
    @retval             Bit 6 : Enhanced Security Erase supported
    @retval             Bit 8 : Security Level 0:High, 1:Maximum

**/

EFI_STATUS
EFIAPI
ReturnSecurityStatus (
    IN  AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT16                          *SecurityStatus
)
{
    IDENTIFY_DATA         IdentifyData;
    VOID                  *BusInterface
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE    BusInterfaceType = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterfaceType;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL  *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde )
    {
        IdentifyData = IdeBusInterface->IdeDevice.IdentifyData;
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        IdentifyData = SataDevInterface->IdentifyData;
    }
#endif

    //  Check if Word 82 Bit 1 is set
    if ( (BusInterfaceType<=AmiStorageInterfaceAhci) && (IdentifyData.Command_Set_Supported_82 & 0x2) ) {
        *SecurityStatus = IdentifyData.Security_Status_128;
    } else if ( BusInterfaceType==AmiStorageInterfaceNvme ) {
        // Return Nvme device security status
        *SecurityStatus = GET_NVME_SEC_RECEIVE_BUFFER_ELEMENT( BusInterface,\
                                                               NvmeSecStatus );
    }

    return EFI_SUCCESS;
}

/**
    Sets User/Master Password using SECURITY_SET_PASSWORD
    (Command Code : 0xF1) security command.

        
    @param This 
    @param UINT16   Control,
    @param UINT8    *Buffer (32 Bytes)
    @param UINT8    RevisionCode (Valid when Control
    @param          Bit 0 = 1 of Master Password)

    @param Control : Bit 0 : 0/1 User/Master Password
    @param Bit 8 : 0/1 High/Maximum Security level

    @retval EFI_SUCCESS : Password set.
    @retval EFI_ERROR   : Error in setting Password.

    @note  
      1. Buffer should be 32 bytes long (Mandatory). Even if Password is not 32
     bytes long,pad it with zeros to create a buffer of 32 bytes.
      2. If a MasterPassword with a valid Revision code(0x0001 through 0xFFFE)
     already exists, a new MasterPassword with a different revision code
     cannot be installed.MasterPassword needs to be unlocked, disabled and
     then a new password can be installed.

**/
EFI_STATUS
EFIAPI
SecuritySetPassword (
    IN  AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT16                          Control,
    UINT8                           *Buffer,
    UINT16                          RevisionCode
)
{
    EFI_STATUS            Status;

#if (!DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT ) || HDD_PASSWORD_ON_NVRAM
    UINT32                HddDataId  = 0;
#endif
#if MASTER_PASSWORD_REVISION_SWITCH == 2
    UINT16                Temp;
    IDENTIFY_DATA         IdentifyData = {0,};
#endif
    VOID                  *BusInterface
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE BusInterfaceType = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterfaceType;
#if MASTER_PASSWORD_REVISION_SWITCH == 2
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL  *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;;
#endif
#endif

    Control &= 0x0101;          // Bit0 and Bit 8 are valid

    //  Update the Identify device buffer
    UpdateIdentifyData( BusInterface, BusInterfaceType );

#if MASTER_PASSWORD_REVISION_SWITCH == 2

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde )
    {
        IdentifyData = IdeBusInterface->IdeDevice.IdentifyData;
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        IdentifyData = SataDevInterface->IdentifyData;
    }
#endif
#endif

    if ( (Control & 1) && (RevisionCode == 0))
    {
#if MASTER_PASSWORD_REVISION_SWITCH == 1
        RevisionCode = CHANGED_MASTER_PASSWORD_REVISION;
#else
        RevisionCode = DEFAULT_MASTER_PASSWORD_REVISION;
#endif

#if MASTER_PASSWORD_REVISION_SWITCH == 2
        
        if( BusInterfaceType <= AmiStorageInterfaceAhci ) {
            Temp = ++IdentifyData.Master_Password_Rev_92;
        } else {
            Temp = GET_NVME_SEC_RECEIVE_BUFFER_ELEMENT( BusInterface, MasterPwdIdentifier );
            Temp++;
        }

        if ((Temp == 0) || (Temp == 0xFFFF))
        {
            Temp++;

            if ( Temp == 0xFFFF )
            {
                Temp++;
            }
        }
        RevisionCode = Temp;
#endif

        if ( RevisionCode == DISABLED_MASTER_PASSWORD_REVISION )
        {
            RevisionCode++;

            if ((RevisionCode == 0) || (RevisionCode == 0xFFFF))
            {
                RevisionCode++;

                if ( RevisionCode == 0xFFFF )
                {
                    RevisionCode++;
                }
            }
        }
    }

    if( BusInterfaceType <= AmiStorageInterfaceAhci ) {
        Status = SecurityCommonRoutine(
            BusInterface,
            Control,
            Buffer,
            SECURITY_SET_PASSWORD,
            RevisionCode,
            BusInterfaceType
            );
    } else {
        Status = NvmeSecuritySendCommandCommonRoutine (
            BusInterface,
            Control,
            Buffer,
            NVME_ATA_SECURITY_SET_PASSWORD,
            RevisionCode,
            BusInterfaceType
            );
    }

    //  Update the Identify device buffer
    UpdateIdentifyData( BusInterface, BusInterfaceType );

    //  If Password is set successfully, save it.
    if ( !EFI_ERROR( Status )) {
#if SMMHDDSECURITY_SUPPORT
        if(!(Control & 1)) {
            SmmHDDPasswordInterface( This,
                                    Control,
                                    Buffer,
                                    SW_SMI_HDD_PASSWORD_SAVE );
        }
#endif
#if (!DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT ) || HDD_PASSWORD_ON_NVRAM
        Status = GatherIdeInfo( BusInterface, BusInterfaceType, &HddDataId );
        ASSERT_EFI_ERROR( Status );

        Status = SetHddSecDataInNvram( BusInterface,
                                       &HddDataId,
                                       Control,
                                       Buffer );
        ASSERT_EFI_ERROR( Status );
#endif
    }

    return Status;
}

/**
    Unlocks the devices for media access using
    SECURITY_UNLOCK(Command Code : 0xF2) security command.

        
    @param This 
    @param UINT16   Control,
    @param UINT8    *Buffer (32 Bytes).

    @param Control : Bit 0 : 0/1 User/Master Password
    @param Buffer  : Pointer to password buffer which will be used to unlock the
            drive.

    @retval EFI_SUCCESS         : Hard disk is unlocked.
    @retval EFI_DEVICE_ERROR    : Failed to unlock the drive.

    @note  
      1. Buffer should be 32 bytes long (Mandatory). Even if Password is not
     32 bytes long, pad it with zeros to create a buffer of 32 bytes.
      2. If the password compare fails then the device returns command aborted
     to the host and decrements the unlock counter. This counter is initially
     set to five and is decremented for each password mismatch when SECURITY
     UNLOCK is issued and the device is locked. When this counter reaches
     zero then SECURITY UNLOCK and SECURITY ERASE UNIT commands are command
     aborted until a power-on reset or a hardware reset.SECURITY UNLOCK
     commands issued when the device is unlocked have no effect on the unlock
     counter.
      3. SECURITY_DISABLE_PASSWORD and SECURITY_SET_PASSWORD commands used when
     security password is set and HDD is in unlocked state. This is done to
     have consistency of output from this function.
**/

EFI_STATUS
EFIAPI
SecurityUnlockPassword (
    AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT16                      Control,
    UINT8                       *Buffer
)
{
    EFI_STATUS            Status;

#if (!DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT ) || HDD_PASSWORD_ON_NVRAM
    UINT32                HddDataId  = 0;
    UINT8                 TempPassWordBuffer[IDE_PASSWORD_LENGTH]= {0};
    EFI_STATUS            TempPassWordStatus = EFI_SUCCESS;
#endif
    UINT16                RevisionCode = 0;
    VOID                  *BusInterface   \
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE    BusInterfaceType   \
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterfaceType;
    UINT16                SecurityStatus = 0;
#if (defined(PORTRESET_ON_HDD_PWD_VERIFICATION) && (PORTRESET_ON_HDD_PWD_VERIFICATION != 0))
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    COMMAND_STRUCTURE     CommandStructure;
    SATA_DEVICE_INTERFACE *SataDevInterface 
        =(SATA_DEVICE_INTERFACE*)BusInterface;
#endif
#endif

#if MASTER_PASSWORD_ENABLE
    UINT32                IdePasswordFlags = 0;
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL  *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
    EFI_BLOCK_IO_PROTOCOL *BlockIo;
    EFI_STATUS            Status1;
#endif

    // Update the Identify device buffer
    UpdateIdentifyData( BusInterface, BusInterfaceType );

    Status = ReturnSecurityStatus( This, &SecurityStatus );

    Control &= 0x1;         // Bit0 is valid

    // When in Setup, Current Password is prompted for consistency when Setting a New Password
    // Or Disabling the Existing Password. Follow a Special Path only in this case.
    //*****This Case Applies on only when (1)Password is Set & Security Enabled
    //                                    (2)Unlocked
    if ((SecurityStatus & BIT01) && (!(SecurityStatus & BIT02))) {
        
        if ( !(Control & 1)) {
#if (defined (PORTRESET_ON_HDD_PWD_VERIFICATION) && (PORTRESET_ON_HDD_PWD_VERIFICATION != 0))
            if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )

                //
                // AHCI MODE:  Changing or Clearing User Password.
                //  1. Send the Disable Software Preservation if the HDD is unlocked state 
                //  2. Issue Port reset to put back the HDD into Locked State 
                //  3. Send the Unlock command to verify the current password 
                //
                if ((SataDevInterface->IdentifyData.Reserved_76_79[0] != 0xFFFF )&&
                    (SataDevInterface->IdentifyData.Reserved_76_79[2] & 0x0040)&&           // Software Preservation support
                    (SataDevInterface->IdentifyData.Security_Status_128 & 0x0002)&&         // Security enabled
                    (!(SataDevInterface->IdentifyData.Security_Status_128 & 0x0004))) {     // Security not locked

                    //
                    // Send Disable Software Preservation command.
                    //
                    CommandStructure.Features = DISABLE_SATA2_SOFTPREV;
                    CommandStructure.SectorCount = 6;
                    CommandStructure.Command = SET_FEATURE_COMMAND;
                    Status= SataDevInterface->AhciBusInterface-> \
                                 ExecuteNonDataCommand(SataDevInterface, 
                                                       CommandStructure
                                                       );
                }
                // Generate the port Reset to put the HDD into Locked State
                Status= SataDevInterface-> \
                         AhciBusInterface->GeneratePortReset( 
                                      SataDevInterface->AhciBusInterface, 
                                      SataDevInterface,
                                      (UINT8)SataDevInterface->PortNumber, 
                                      (UINT8)SataDevInterface->PMPortNumber,
                                      0x00, 
                                      0x00
                                      );
                Status = SecurityCommonRoutine(
                                    BusInterface,
                                    Control,
                                    Buffer,
                                    SECURITY_UNLOCK,
                                    0,
                                    BusInterfaceType
                                    );
#if (!DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT ) || HDD_PASSWORD_ON_NVRAM
                if(EFI_ERROR(Status)) {

                    TempPassWordStatus = GetHddSecDataFromNvram( BusInterface,
                                                                 BusInterfaceType,
                                                                 TempPassWordBuffer );
                    if(!EFI_ERROR(TempPassWordStatus)) {
                        SecurityCommonRoutine(
                                    BusInterface,
                                    Control,
                                    TempPassWordBuffer,
                                    SECURITY_UNLOCK,
                                    0,
                                    BusInterfaceType
                                    );
                    }
                } 
#endif
#endif 
                return Status;
            }
#endif
            if( BusInterfaceType <= AmiStorageInterfaceAhci) {

                // 
                // (a)For Changing or Clearing User Password.
                //  1.Send a disable Password Command from the String Obtained from user.
                //  2.If Success Set the Password Same Password.
                //  3.Suitable if Logged in as Both User or Master
                //
                Status = SecurityCommonRoutine(
                        BusInterface,
                        Control,
                        Buffer,
                        SECURITY_DISABLE_PASSWORD,
                        0,
                        BusInterfaceType
                        );

                if ( !EFI_ERROR( Status ))
                {
                    SecurityCommonRoutine(
                            BusInterface,
                            Control,
                            Buffer,
                            SECURITY_SET_PASSWORD,
                            RevisionCode,
                            BusInterfaceType
                            );
                }
            } else if( BusInterfaceType == AmiStorageInterfaceNvme ) {
                Status = NvmeSecuritySendCommandCommonRoutine (
                        BusInterface,
                        Control,
                        Buffer,
                        NVME_ATA_SECURITY_DISABLE_PASSWORD,
                        0,
                        BusInterfaceType
                        );
                if ( !EFI_ERROR( Status ))
                {
                    NvmeSecuritySendCommandCommonRoutine(
                            BusInterface,
                            Control,
                            Buffer,
                            NVME_ATA_SECURITY_SET_PASSWORD,
                            RevisionCode,
                            BusInterfaceType
                            );
                }
            }
            return Status;
        }
    }

    if ( BusInterfaceType <= AmiStorageInterfaceAhci) {
        Status = SecurityCommonRoutine(
            BusInterface,
            Control,
            Buffer,
            SECURITY_UNLOCK,
            0,
            BusInterfaceType
            );
    } else if( BusInterfaceType == AmiStorageInterfaceNvme ) {
        Status = NvmeSecuritySendCommandCommonRoutine(
            BusInterface,
            Control,
            Buffer,
            NVME_ATA_SECURITY_UNLOCK,
            0,
            BusInterfaceType
            );
    }

    // Update the Identify device buffer
    UpdateIdentifyData( BusInterface, BusInterfaceType );

    if ( !EFI_ERROR( Status )) {
#if SMMHDDSECURITY_SUPPORT
        // Save the Password if unlocked Successfully.
        SmmHDDPasswordInterface( This,
                                 Control,
                                 Buffer,
                                 SW_SMI_HDD_PASSWORD_SAVE );
#endif
#if (!DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT ) || HDD_PASSWORD_ON_NVRAM
        Status = GatherIdeInfo( BusInterface, BusInterfaceType, &HddDataId );
        ASSERT_EFI_ERROR( Status );

        Status = SetHddSecDataInNvram( BusInterface,
                                       &HddDataId,
                                       Control,
                                       Buffer );
        ASSERT_EFI_ERROR( Status );
#endif

        // After unlock the HDD set the OEM default master password
        // Check if OEM_DEFAULT_MASTER_PASSWORD is enabled and
        // DISPLAY_MASTER_PASSWORD is Disabled.
#if MASTER_PASSWORD_ENABLE
        Status = ReturnIdePasswordFlags( This, &IdePasswordFlags );
        ASSERT_EFI_ERROR( Status );

        if ((IdePasswordFlags & 0x0800) && (!(IdePasswordFlags & 0x0008)))
        {
            Status = OEMSetMasterPassword( This );
            ASSERT_EFI_ERROR( Status );
        }
#endif
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
        if ( BusInterfaceType == AmiStorageInterfaceIde ) {
            // Check if BlockIO Protocol is already present on this device.
            Status1 = pBS->HandleProtocol(IdeBusInterface->IdeDeviceHandle,
                                          &gEfiBlockIoProtocolGuid,
                                          (VOID**)&BlockIo
                                          );

            // Connect only if BloCkIO protocol is not installed on the device.
            if ( EFI_ERROR( Status1 )) {
                pBS->ConnectController(
                    IdeBusInterface->IdeControllerInterfaceHandle,
                    NULL,
                    NULL,
                    TRUE
                    );
            }
        }
#endif
    }

    return Status;
}

/**
    Disables Password authentication before media access.
    SECURITY_DISABLE_PASSWORD(Command Code : 0xF6) security
    command is used to remove the user password.

    @param This 
    @param UINT16   Control,
    @param UINT8    *Buffer (32 Bytes)

    @param Control : Bit 0 : 0/1 User/Master Password

    @param Buffer  : Pointer to password buffer which will be used to when sending
    @param SECURITY_DISABLE_PASSWORD command.


    @retval EFI_SUCCESS      : Security mode feature disabled.
    @retval EFI_DEVICE_ERROR : Security command failed.

    @note  
      1. Buffer should be 32 bytes long (Mandatory). Even if Password is not
     32 bytes long,pad it with zeros to create a buffer of 32 bytes.
      2. Password should be unlocked before disabling it.
      3. SECURITY_DISABLE_PASSWORD command does not change the Master password
     that may be reactivated later by setting a User password.
      4. Normal erase mode is selected, the SECURITY ERASE UNIT command writes
     binary zeroes to all user data areas.

**/

EFI_STATUS
EFIAPI
SecurityDisablePassword (
    AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT16                      Control,
    UINT8                       *Buffer
)
{
    EFI_STATUS Status;
    VOID       *BusInterface   \
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE   BusInterfaceType  \
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterfaceType;

#if (!DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT ) || HDD_PASSWORD_ON_NVRAM
    UINT32     HddDataId = 0;
    UINT8      EmptyBuffer[IDE_PASSWORD_LENGTH];
#endif

    Control &= 0x1;             // Bit0 is valid
    
#if (!DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT ) || HDD_PASSWORD_ON_NVRAM
    //
    //Update the Nvram buffer with NULL string.
    //
    pBS->SetMem( EmptyBuffer, IDE_PASSWORD_LENGTH, 0 );

    Status = GatherIdeInfo( BusInterface, BusInterfaceType, &HddDataId );
    ASSERT_EFI_ERROR( Status );

    Status = SetHddSecDataInNvram( BusInterface, &HddDataId, Control, EmptyBuffer );
    ASSERT_EFI_ERROR( Status );
#endif

    if( BusInterfaceType <= AmiStorageInterfaceAhci ) {
        Status = SecurityCommonRoutine( BusInterface,
                                        Control,
                                        Buffer,
                                        SECURITY_DISABLE_PASSWORD,
                                        0,
                                        BusInterfaceType
                                        );
    } else if( BusInterfaceType == AmiStorageInterfaceNvme ) {

        // Nvme Password disable command
        Status = NvmeSecuritySendCommandCommonRoutine( BusInterface,
                                        Control,
                                        Buffer,
                                        NVME_ATA_SECURITY_DISABLE_PASSWORD,
                                        0,
                                        BusInterfaceType
                                        );
    }

    // Update the Identify device buffer
    UpdateIdentifyData( BusInterface, BusInterfaceType );

#if SMMHDDSECURITY_SUPPORT
    if ( !EFI_ERROR( Status )) {
        SmmHDDPasswordInterface( This,
                                 Control,
                                 Buffer,
                                 SW_SMI_HDD_PASSWORD_REMOVE );
    }
#endif

    // If the Empty string entered for the master password, Set the OEM
    // disable password with Disabled revision code.
    if ( Control & 01 )
    {
        EFI_STATUS    DefaultStatus;
        DefaultStatus = SetDefaultMasterPassword( This );
        ASSERT_EFI_ERROR( DefaultStatus );
    }
    return Status;
}

/**
    Sets the Default Password String provided by OEM as
    Master Password.

    @param This 

    @retval EFI_SUCCESS : Default master password is set.
    @retval EFI_ERROR   : Default master password is not set.

    @note  
      DEFAULT_MASTER_PASSWORD_TAG token string will be set as master password.
**/

EFI_STATUS
SetDefaultMasterPassword (
    IN  AMI_HDD_SECURITY_PROTOCOL   *This
)
{
    EFI_STATUS Status;
    UINT8      Buffer[IDE_PASSWORD_LENGTH];
    UINT16     Control      = 1;
    UINT16     RevisionCode = DISABLED_MASTER_PASSWORD_REVISION;
    UINTN      i;
    UINT16     SecurityStatus = 0;

    MemSet( &Buffer, IDE_PASSWORD_LENGTH, 0 );

    for ( i = 0; i < IDE_PASSWORD_LENGTH; i++ )
    {
        Buffer[i] = (UINT8)sDefaultMasterPasswordTag[i];

        if ( sDefaultMasterPasswordTag[i] == L'\0' )
        {
            break;
        }
    }

    // Set the OEM master password if the HDD is not in Frozen or locked state.
    Status = ReturnSecurityStatus( This, &SecurityStatus );

    if ( !((SecurityStatus & 0x04) || (SecurityStatus & 0x08)))
    {
        Status = SecuritySetDefaultMasterPassword( This,
                                                   Control,
                                                   Buffer,
                                                   RevisionCode );
    }
    return Status;
}

/**
    Sets default Master Password with disabled revision code.
    SECURITY_SET_PASSWORD(Command code : 0xF1) security command
    used to set default master password.

    @param This 
    @param UINT16                          Control,
    @param UINT8                           *Buffer (32 Bytes)
    @param UINT8                           RevisionCode (Valid when
    @param Control Bit 0 = 1 of Master Password)

    @param Control :       Bit 0 : 0/1 User/Master Password
    @param Bit 8 : 0/1 High/Maximum Security level

    @retval EFI_SUCCESS : Password set
    @retval EFI_ERROR   : Error in setting Password

    @note  
      1. Buffer should be 32 bytes long (Mandatory). Even if Password is not
     32 bytes long,pad it with zeros to create a buffer of 32 bytes.

      2. If a MasterPassword with a valid Revision code already exists, a new
     MasterPassword with a different revision code cannot be installed.
     MasterPassword needs to be unlocked, disabled and then a new password
     can be installed.

**/
EFI_STATUS
SecuritySetDefaultMasterPassword (
    AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT16                      Control,
    UINT8                       *Buffer,
    UINT16                      RevisionCode
)
{
    VOID       *BusInterface = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE BusInterfaceType = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterfaceType;
    EFI_STATUS Status;

#if (!DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT ) || HDD_PASSWORD_ON_NVRAM
    UINT32     HddDataId  = 0;
#endif

    Control &= 0x0101;          // Bit0 and Bit 8 are valid


    // Update the Identify device buffer
    UpdateIdentifyData( BusInterface, BusInterfaceType );

    if( BusInterfaceType <= AmiStorageInterfaceAhci ) {
        Status = SecurityCommonRoutine(
                    BusInterface,
                    Control,
                    Buffer,
                    SECURITY_SET_PASSWORD,
                    RevisionCode,
                    BusInterfaceType
                    );
    } else {
        Status = NvmeSecuritySendCommandCommonRoutine(
                    BusInterface,
                    Control,
                    Buffer,
                    NVME_ATA_SECURITY_SET_PASSWORD,
                    RevisionCode,
                    BusInterfaceType
                    );
    }

    // Update the Identify device buffer
    UpdateIdentifyData( BusInterface, BusInterfaceType );

    // If Password is set successfully, save it.
    if ( !EFI_ERROR( Status )) {
#if SMMHDDSECURITY_SUPPORT
        SmmHDDPasswordInterface(
            This,
            Control,
            Buffer,
            SW_SMI_HDD_PASSWORD_SAVE
            );
#endif
#if (!DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT ) || HDD_PASSWORD_ON_NVRAM
        Status = GatherIdeInfo( BusInterface, BusInterfaceType, &HddDataId );
        ASSERT_EFI_ERROR( Status );

        Status = SetHddSecDataInNvram( BusInterface,
                                       &HddDataId,
                                       Control,
                                       Buffer );
        ASSERT_EFI_ERROR( Status );
#endif
    }

    return Status;
}

/**
    Prevents any further alteration of security status on the
    device leaving the hard disk in Frozen mode. Frozen mode is
    disabled by power-off or hardware reset. SECURITY_FREEZE_LOCK
    (Command Code : 0xF5)security command is used to freeze hard disk.

        
    @param This 

    @retval EFI_SUCCESS : Hard disk is successfully kept in FROZEN state.
    @retval EFI_ERROR   : Security Freeze command failed.

    @note  
  Commands disabled by SECURITY FREEZE LOCK are:
      - SECURITY SET PASSWORD
      - SECURITY UNLOCK
      - SECURITY DISABLE PASSWORD
      - SECURITY ERASE PREPARE
      - SECURITY ERASE UNIT
**/

EFI_STATUS
EFIAPI
SecurityFreezeLock (
    IN  AMI_HDD_SECURITY_PROTOCOL *This
)
{
    EFI_STATUS            Status;
    VOID                  *BusInterface
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterfaceType;
    UINT16                SecurityStatus = 0;

    COMMAND_STRUCTURE     CommandStructure;
#if !DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT && SMMHDDSECURITY_SUPPORT
    IDENTIFY_DATA         IdentifyData;
#endif
    
    Status=ReturnSecurityStatus( This, &SecurityStatus );
    if(EFI_ERROR(Status)) {
        return Status;
    }

#if !DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT && SMMHDDSECURITY_SUPPORT
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        IdentifyData = ((AMI_IDE_BUS_PROTOCOL*)BusInterface)->IdeDevice.IdentifyData;
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        IdentifyData = ((SATA_DEVICE_INTERFACE*)BusInterface)->IdentifyData;
    }
#endif

    if ( SecurityStatus & 2 ) {
        if ( ((BusInterfaceType<=AmiStorageInterfaceAhci) && (IdentifyData.Reserved_76_79[3] & 0x0040))\
                || BusInterfaceType == AmiStorageInterfaceNvme ) {
            Status = TransferPwDataFromNvramToSmram( This );
            ASSERT_EFI_ERROR( Status );
        }
    }
#endif

    // Freeze Lock command will not work with the HDD is in Locked State.
    // So return with Success if the HDD is either in Locked State or Frozen state.
    if ( SecurityStatus & 0xC ) {
        return EFI_SUCCESS;
    }
    
    if ( BusInterfaceType <= AmiStorageInterfaceAhci ) {
        ZeroMemory( &CommandStructure, sizeof(COMMAND_STRUCTURE));
        CommandStructure.Command = SECURITY_FREEZE_LOCK;

        Status =  CommonNonDataHook( BusInterface,
                                    CommandStructure,
                                    BusInterfaceType );
        ASSERT_EFI_ERROR (Status);
        CommandStructure.Features = DEV_CONFIG_FREEZE_LOCK_FEATURES;
        CommandStructure.Command  = DEV_CONFIG_FREEZE_LOCK;
        Status                    = CommonNonDataHook( BusInterface,
                                                   CommandStructure,
                                                   BusInterfaceType );
    } else if ( BusInterfaceType == AmiStorageInterfaceNvme ) {
        Status = NvmeSecuritySendCommandCommonRoutine(
                    BusInterface,
                    0,
                    0,
                    NVME_ATA_SECURITY_FREEZE_LOCK,
                    0,
                    BusInterfaceType
                    );
    }

    //  Update the Identify device buffer
    UpdateIdentifyData( BusInterface, BusInterfaceType );

    return Status;
}

/**
    Erases the Device content.SECURITY_ERASE_UNIT
    (Command code : 0xF4) security command is used for device
    erasing.

        
    @param This 
    @param UINT16   Control,
    @param UINT8    *Buffer

    @param Control :       Bit 0 : 0/1 User/Master Password
    @param Bit 1 : 0/1 Normal Erase/ Enhanced Erase
    @param Buffer  : Pointer to password buffer which will be used to when sending
    @param SECURITY_ERASE_UNIT command.

    @retval EFI_SUCCESS      : Successful completion of Erase command.
    @retval EFI_DEVICE_ERROR : EraseUnit command failed to complete.

    @note  
      If a device is locked with USER PASSWORD and Maximum security level set
      then only way to unlock the device with MASTER password is to erase the
      contents of the device.
**/
EFI_STATUS
EFIAPI
SecurityEraseUnit (
    AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT16                      Control,
    UINT8                       *Buffer
)
{
    EFI_STATUS        Status = EFI_UNSUPPORTED;

    VOID              *BusInterface   \
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE   BusInterfaceType   \
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterfaceType;    
    COMMAND_STRUCTURE CommandStructure;

    // Issue Security Erase Unit
    Control &= 0x3;  // Bit0 and Bit 1 is valid

    // Update the Identify device buffer
    UpdateIdentifyData( BusInterface, BusInterfaceType );
    
    if( BusInterfaceType <= AmiStorageInterfaceAhci ) {
        ZeroMemory( &CommandStructure, sizeof(COMMAND_STRUCTURE));
        CommandStructure.Command = SECURITY_ERASE_PREPARE;

        Status = CommonNonDataHook( BusInterface, CommandStructure, BusInterfaceType );
        ASSERT_EFI_ERROR (Status);

        Status   = SecurityCommonRoutine(
                      BusInterface,
                      Control,
                      Buffer,
                      SECURITY_ERASE_UNIT,
                      0,
                      BusInterfaceType
                      );

    } else if (BusInterfaceType == AmiStorageInterfaceNvme ) {

        // Send Nvme Security Erase prepare command
        Status = NvmeSecuritySendCommandCommonRoutine(
                    BusInterface,
                    0,
                    0,
                    NVME_ATA_ECURITY_ERASE_PREPARE,
                    0,
                    BusInterfaceType
                    );

        ASSERT_EFI_ERROR (Status);

        // Send Nvme Security Erase command
        Status = NvmeSecuritySendCommandCommonRoutine(
                    BusInterface,
                    Control,
                    Buffer,
                    NVME_ATA_SECURITY_ERASE_UNIT,
                    0,
                    BusInterfaceType
                    );
    }

    // Update the Identify device buffer
    UpdateIdentifyData( BusInterface, BusInterfaceType );

    return Status;
}

/**
    Returns Ide Password Variables and Flags.

    @param This 
    @param UINT32   *IdePasswordFlags

    @retval EFI_SUCCESS : IDEPassword Status Flag is returned in IdePasswordFlags.
    @retval EFI_ERROR   : Error while retrieving the SecurityStatus.


    @note IdePasswordFlags:
     **TBD Only Bit0, Bit03, Bit11, Bit16, Bit17 are Supported Currently

         BIT0                             User Password Support;
         BIT1                             Master Password Support;
         BIT2                             Display User Password;
         BIT3                             Display Master Password;
         BIT4                             Display User Password Status;
         BIT5                             Display Master Password Status;
         BIT6                             Freeze Lock Hdds;
         BIT7                             Single User Password;
         BIT8                             Single Master Password;
         BIT9                             System User Password as Hdd User Password;
         BIT10                            System Admin Password as Hdd Master Password;
         BIT11                            OEM Default Master Password
         BIT12                            Prompt for User Password in Post
         BIT13                            Prompt for Master Password in Post
         BIT14                            Display Security Status in Setup
         BIT15                            DUMMY Security Token.
         BIT16                            MasterPassword Set Status.
         BIT17                            UserPassword Set Status.

**/

EFI_STATUS
EFIAPI
ReturnIdePasswordFlags (
    AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT32                      *IdePasswordFlags
)
{
    EFI_STATUS            Status         = EFI_SUCCESS;
    UINT32                Temp           = 0;
    UINT16                MasterPwdRev   = 0;
    UINT16                SecurityStatus = 0;
    VOID                  *BusInterface
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterfaceType;
    IDENTIFY_DATA         IdentifyData;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL  *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde )
    {
        IdentifyData = IdeBusInterface->IdeDevice.IdentifyData;
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        IdentifyData = SataDevInterface->IdentifyData;
    }
#endif

#if MASTER_PASSWORD_ENABLE
    Temp |= BIT01;
#endif


#if DISPLAY_MASTER_PASSWORD
    Temp |= BIT03;
#endif

#if OEM_DEFAULT_MASTER_PASSWORD
    Temp |= BIT11;
#endif

    if( BusInterfaceType <=AmiStorageInterfaceAhci ) {
        MasterPwdRev = IdentifyData.Master_Password_Rev_92;
    } else if ( BusInterfaceType == AmiStorageInterfaceNvme ) {
        MasterPwdRev = GET_NVME_SEC_RECEIVE_BUFFER_ELEMENT( BusInterface, MasterPwdIdentifier );
    }
    
    if ((MasterPwdRev != 0x0000) && (MasterPwdRev != 0xFFFF)    \
            && (MasterPwdRev != DISABLED_MASTER_PASSWORD_REVISION)) {
        Temp |= BIT16;
    }

    Status = ReturnSecurityStatus( This, &SecurityStatus );

    if ( EFI_ERROR( Status ))
    {
        return Status;
    }

    if ( SecurityStatus & BIT01 )
    {
        Temp |= BIT17;
    }

    *IdePasswordFlags = (UINT32)Temp;

    return EFI_SUCCESS;
}

/**
    Checks if device supports TCG Security protocol or not.

    @param BusInterface  - Pointer to current bus interface
    @param BusInterfaceType - Pointer to bus interface type AHCI

    @retval EFI_SUCCESS                      : Security Mode supported.
    @retval EFI_DEVICE_ERROR/EFI_UNSUPPORTED : Not supported.

**/

EFI_STATUS
IsSecurityProtocolSupported (
   IN     VOID                    *BusInterface,
   IN    AMI_STORAGE_BUS_INTERFACE  BusInterfaceType,
   OUT    BOOLEAN                 *SAT3Supported,
   OUT    BOOLEAN                 *TcgSupported  
){
    EFI_STATUS                            Status;
    SP0_TRUSTED_RECEIVE_PARAMETER_DATA    QueryBuffer;
    UINTN                                 QueryBufferSize = sizeof(SP0_TRUSTED_RECEIVE_PARAMETER_DATA);
    UINTN                                 SpByte;
    EFI_STORAGE_SECURITY_COMMAND_PROTOCOL *StorageSecurityProtocol = NULL;
    UINT16                                ListLength = 0;
    UINT32			                      MediaId;
    
    IDENTIFY_DATA                         IdentifyData;
	 
	MediaId = ((SATA_DEVICE_INTERFACE*)BusInterface)->SataBlkIo->BlkIo.Media->MediaId;
    StorageSecurityProtocol = ((SATA_DEVICE_INTERFACE*)BusInterface)->StorageSecurityInterface;
    IdentifyData = ((SATA_DEVICE_INTERFACE*)BusInterface)->IdentifyData;

    
    if(!(IdentifyData.Trusted_Computing_Support & 0x01)) {
	    *TcgSupported = FALSE ;
	    return EFI_SUCCESS;
     }
    
    gBS->SetMem(&QueryBuffer, QueryBufferSize, 0);
    // SPC Spec Section 7.7.1 has details on decoding the data
    Status = StorageSecurityProtocol->ReceiveData( StorageSecurityProtocol,
                                                   MediaId, // Media ID
                                                   SATA_COMMAND_TIMEOUT * 10000000,    // 5 sec 
                                                   SECURITY_PROTOCOL_INFORMATION,   // Security Protocol ID
                                                   0x00,                            // SP specific id
                                                   0x200,                           // Transfer Length
                                                   &QueryBuffer,
                                                   &QueryBufferSize                 // Transferred Length
                                                   );
    
    if (EFI_ERROR(Status)) {
        // If GetSecurityProtocolInformation returns error Status return Unsupported Security Protocol.
        return EFI_UNSUPPORTED;
    }

    // The List Length field indicates the total length, in bytes, of the supported security protocol list.
    //
    ListLength = QueryBuffer.ListLengthHigh << 8 | QueryBuffer.ListLengthLow;
    if (ListLength == 0) {
        return EFI_UNSUPPORTED;
    }

    // Check Security supported Protocol List for Protocol ID 1
    for (SpByte = 0; SpByte < ListLength; SpByte++) {
        if (QueryBuffer.SupportedSPList[SpByte] == SECURITY_PROTOCOL_1) {
            *TcgSupported = TRUE;
        }
      
        if(*SAT3Supported && *TcgSupported ) break;
    }
    
    return EFI_SUCCESS;   
}

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )

/**
     Check for NVME Security Protocol Supported

        IN     VOID     *BusInterface,
        OUT    BOOLEAN  *SATSupported,
        OUT    BOOLEAN  *TcgSupported
        
        @retval EFI_STATUS

 **/

EFI_STATUS 
IsNvmeSecurityProtocolSupported (
        IN     VOID     *BusInterface,
        OUT    BOOLEAN  *SAT3Supported,
        OUT    BOOLEAN  *TcgSupported
)
{
    EFI_STATUS      Status;
    UINT16          ListLength = 0;
    UINT16          SpByte = 0;
    SP0_TRUSTED_RECEIVE_PARAMETER_DATA  QueryBuffer;
     
    UINT32   QueryBufferSize = sizeof(SP0_TRUSTED_RECEIVE_PARAMETER_DATA);

    *SAT3Supported = FALSE;
    *TcgSupported = FALSE;
    
    pBS->SetMem( &QueryBuffer, QueryBufferSize, 0 );
    
    Status = GetNvmeSecurityProtocolInformation(BusInterface, (UINT8*)&QueryBuffer, QueryBufferSize);
    if (EFI_ERROR(Status)) {
        // If GetNvmeSecurityProtocolInformation returns error Status return Unsupported Security Protocol.
        return EFI_UNSUPPORTED;
    }

    // The List Length field indicates the total length, in bytes, of the supported security protocol list.
    //
    ListLength = QueryBuffer.ListLengthHigh << 8 | QueryBuffer.ListLengthLow;
    if (ListLength == 0) {
        return EFI_UNSUPPORTED;
    }
    
    TRACE((-1, "ListLength =: %x\n", ListLength));
    
    // Check the security supported Protocol List
    for (SpByte = 0; SpByte < ListLength; SpByte++) {
        TRACE((-1, "SpByte =: %x, SupportedProtocolList = %x\n", SpByte, QueryBuffer.SupportedSPList[SpByte]));
        if (QueryBuffer.SupportedSPList[SpByte] == NVME_SECUIRTY_SECP) {
            *SAT3Supported = TRUE;
        }
        
        if (QueryBuffer.SupportedSPList[SpByte] == SECURITY_PROTOCOL_1) {
            *TcgSupported = TRUE;
        }
        
        if(*SAT3Supported && *TcgSupported ) {
            return EFI_SUCCESS;
        }
    }

    return EFI_SUCCESS;
}

/**
    Get Nvme Security Protocol Information

    @param BusInterface 
    @param Buffer 
    @param BufferLength 
      

    @retval EFI_STATUS

**/
EFI_STATUS 
GetNvmeSecurityProtocolInformation (
    IN     VOID     *BusInterface,
    IN OUT VOID     *Buffer,
    IN OUT UINT32    BufferLength 
)
{

    
    UINT32 CDW10_Value = SECURITY_PROTOCOL_INFORMATION << 24; // SP = 0X00, SPSP=0x0000

    return ( NvmeSecurityRecieveRoutine(
                   BusInterface,
                   Buffer,
                   BufferLength,
                   CDW10_Value ) );
    
}

/**
    Gets Nvme Security Status information

    @param BusInterface 
    @param Buffer
    @param Bufferlength

    @retval EFI_STATUS

**/
EFI_STATUS
GetNvmeDeviceSecurityData (
        VOID        *BusInterface,
        UINT8       *Buffer,
        UINT32       Bufferlength
)
{
    UINT32 CDW10_Value = NVME_SECUIRTY_SECP << 24; // SECP = 0XEF, SPSP=0

    return ( NvmeSecurityRecieveRoutine(
                   BusInterface,
                   Buffer,
                   Bufferlength,
                   CDW10_Value ) );
}
#endif

/**
    Get Sends Security Receive command to nvme and stores the 
    returned data in Buffer

    @param BusInterface 
    @param Buffer
    @param Bufferlength
    @param CDW10_Value

    @retval EFI_STATUS

**/
EFI_STATUS
NvmeSecurityRecieveRoutine(
    VOID        *BusInterface,
    UINT8       *Buffer,
    UINT32       Bufferlength,
    UINT32      CDW10_Value
)
{
    EFI_STATUS                               Status =  EFI_SUCCESS;
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET NvmePacket;
    EFI_NVM_EXPRESS_COMMAND                  NvmeCommand;
    EFI_NVM_EXPRESS_COMPLETION               NvmeCompletion;
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL       *EfiNvmePassThru =\
            (EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL*)BusInterface;
    
    pBS->SetMem( &NvmePacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET), 0 );
    pBS->SetMem( &NvmeCommand, sizeof(EFI_NVM_EXPRESS_COMMAND), 0);
    pBS->SetMem( &NvmeCompletion, sizeof(EFI_NVM_EXPRESS_COMPLETION), 0);    
    
    NvmeCommand.Nsid = 0;
#if defined(USE_MDE_PKG_NVME_PASSTHRU_CHANGES)
    NvmeCommand.Cdw0.Opcode = SECURITY_RECEIVE;
#else
    NvmeCommand.Cdw0.OpCode = SECURITY_RECEIVE;
#endif
    NvmeCommand.Cdw0.FusedOperation = 0;
    NvmeCommand.Cdw10 = CDW10_Value;
    NvmeCommand.Cdw11 = Bufferlength;
    
    NvmeCommand.Flags = ( CDW10_VALID | CDW11_VALID );
    
    NvmePacket.CommandTimeout = MultU64x32(NVME_COMMAND_TIMEOUT, 10000000);    // 1Sec
    NvmePacket.NvmeCmd = &NvmeCommand;   
    NvmePacket.QueueType = 0;
    NvmePacket.NvmeCompletion = &NvmeCompletion;
    NvmePacket.TransferBuffer = Buffer;
    NvmePacket.TransferLength = Bufferlength;
    
    // Send Command through Passthru API
    Status = EfiNvmePassThru->PassThru ( EfiNvmePassThru,
                                         0,
                                         &NvmePacket,
                                         NULL
                                         );
#endif
    return Status;
}

/**
    Builds Nvme Passwords buffer from user entered password in order
    to unlock Master or User password

    @param SecurityCommand 
    @param Control
    @param PasswrdBuffer
    @param Buffer
    @param RevisionCode

    @retval EFI_STATUS

**/
VOID
BuildNvmeCommandBuffer (
    UINT8    SecurityCommand,
    UINT16   Control,
    UINT8    *PasswrdBuffer,
    UINT8    *Buffer,
    UINT16   RevisionCode
)
{
    // Bit0 - 0/1 Indicates User/Master password    
    // Bit8 - 0/1 High/Maximum Security level
    Control &=0x103;
    Buffer[0] |= Control>>8;
    Buffer[1] |= (UINT8)Control&BIT0;
    
    // Security Erase command
    // BIT1 - 0/1 Normal Erase/ Enhanced Erase
    if( SecurityCommand==NVME_ATA_SECURITY_ERASE_UNIT ) {
        Buffer[0] |= Control>>1;
    }

    if( (SecurityCommand!=NVME_ATA_SECURITY_DISABLE_PASSWORD) &&\
            (Control&0x1) && (RevisionCode != 0) && (RevisionCode != 0xFFFF) ) {
        Buffer[34] = RevisionCode>>8;
        Buffer[35] = (UINT8)RevisionCode;
    }

    pBS->CopyMem( &Buffer[2], &PasswrdBuffer[0], IDE_PASSWORD_LENGTH );

    return;
}

/**
    Function to send nvme Security command

    @param BusInterface 
    @param Control
    @param PasswrdBuffer
    @param SecurityCommand
    @param RevisionCode
    @param BusInterfaceType

    @retval EFI_STATUS

**/
EFI_STATUS
NvmeSecuritySendCommandCommonRoutine (
    IN  VOID                    *BusInterface,
    UINT16                      Control,
    UINT8                       *PasswrdBuffer,
    UINT8                       SecurityCommand,
    UINT16                      RevisionCode,
    AMI_STORAGE_BUS_INTERFACE   BusInterfaceType
)
{
    EFI_STATUS    Status =  EFI_SUCCESS;
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL  *EfiNvmePassThru= \
            (EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL*)BusInterface;
    EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET  NvmePacket;
    EFI_NVM_EXPRESS_COMMAND                   NvmeCommand;
    EFI_NVM_EXPRESS_COMPLETION                NvmeCompletion;
    UINT32                                    CommandLength=0;
    UINT8         NvmeSecurityCmdBuffer[NVME_SECURITY_SEND_BUFFER_SIZE];

    pBS->SetMem( NvmeSecurityCmdBuffer, NVME_SECURITY_SEND_BUFFER_SIZE, 0);
    pBS->SetMem( &NvmePacket, sizeof(EFI_NVM_EXPRESS_PASS_THRU_COMMAND_PACKET), 0 );
    pBS->SetMem( &NvmeCommand, sizeof(EFI_NVM_EXPRESS_COMMAND), 0);
    pBS->SetMem( &NvmeCompletion, sizeof(EFI_NVM_EXPRESS_COMPLETION), 0);
    
    if( (SecurityCommand !=NVME_ATA_ECURITY_ERASE_PREPARE) && 
            (SecurityCommand !=NVME_ATA_SECURITY_FREEZE_LOCK) ) {

        BuildNvmeCommandBuffer(
                    SecurityCommand,
                    Control,
                    PasswrdBuffer,
                    NvmeSecurityCmdBuffer,
                    RevisionCode
                    );
        CommandLength = NVME_SECURITY_SEND_BUFFER_SIZE;
    }

    NvmeCommand.Nsid = 0;

#if defined (USE_MDE_PKG_NVME_PASSTHRU_CHANGES)
    NvmeCommand.Cdw0.Opcode = SECURITY_SEND;
#else
    NvmeCommand.Cdw0.OpCode = SECURITY_SEND;
#endif

    NvmeCommand.Cdw0.FusedOperation = 0;
    NvmeCommand.Cdw10 = (((UINT32)NVME_SECUIRTY_SECP)<<24) | (((UINT32)SecurityCommand)<<8);
    NvmeCommand.Cdw11 = CommandLength;
    
    NvmeCommand.Flags = ( CDW10_VALID | CDW11_VALID );
    
    // Update the Timeout value based on the Command send to controller.
    // The Timeout value for the Secure Erase Unit should be coming from device
    if(SecurityCommand == NVME_ATA_SECURITY_ERASE_UNIT) {
        // Is Enhanced Security Erase requested
        if(Control & 2) {
            // Enhanced Security Erase Timeout
            NvmePacket.CommandTimeout = (UINT64)GET_NVME_SEC_RECEIVE_BUFFER_ELEMENT(EfiNvmePassThru, EnhancedSecEraseT);
        } else {
            // Normal Security Erase Timeout
            NvmePacket.CommandTimeout = (UINT64)GET_NVME_SEC_RECEIVE_BUFFER_ELEMENT(EfiNvmePassThru, SecEraseTime);
        }
        
        if(NvmePacket.CommandTimeout <= 254 ) {
            // Convert the Time out values to 1ms unit
            NvmePacket.CommandTimeout = MultU64x32(NVME_COMMAND_TIMEOUT, 120000);      // Value * 2 Minutes(2 * 1000 * 60) 
        } else {
            NvmePacket.CommandTimeout = 0;
        }
    } else {
        // For all other command the timeout is 1 Sec 
   			NvmePacket.CommandTimeout = MultU64x32(NVME_COMMAND_TIMEOUT, 10000000);    
    }

    NvmePacket.NvmeCmd = &NvmeCommand;   
    NvmePacket.QueueType = 0;
    NvmePacket.NvmeCompletion = &NvmeCompletion;
    NvmePacket.TransferBuffer = NvmeSecurityCmdBuffer;
    NvmePacket.TransferLength = NVME_SECURITY_SEND_BUFFER_SIZE;

    // Send Command through Passthru API
    Status = EfiNvmePassThru->PassThru ( EfiNvmePassThru,
                                         0,
                                         &NvmePacket,
                                         NULL
                                         );
#endif
    return Status;
}

/**
    This is common security routine which will be called from
    security routines. It is from this routine most of the security
    commands are sent to the drive.

        
    @param BusInterface 
    @param UINT16   Control,
    @param UINT8    *Buffer (32 Bytes),
    @param UINT8    SecurityCommand
    @param UINT8    RevisionCode
                    Valid for Master password)

    @param Control : Bit 0 : 0/1 User/Master Password
    @param Bit 1 : 0/1 Normal/Enhanced Erase
    @param Bit 8 : 0/1 High/Maximum Security level

    @param Buffer  : Pointer to password buffer which will be used to when sending
        SecurityCommand.


    @retval EFI_STATUS : EFI_SUCCESS/EFI_DEVICE_ERROR

    @note  
      1. Buffer should be 32 bytes long (Mandatory). Even if Password is not
     32 bytes long, pad it with zeros to create a buffer of 32 bytes.

**/
EFI_STATUS
SecurityCommonRoutine (
    IN  VOID                  *BusInterface,
    UINT16                     Control,
    UINT8                      *Buffer,
    UINT8                      SecurityCommand,
    UINT16                     RevisionCode,
    AMI_STORAGE_BUS_INTERFACE  BusInterfaceType
)
{
    EFI_STATUS Status;
    UINT16     *SecurityBuffer;

    // Allocate 512 Bytes
    Status = pBS->AllocatePool( EfiBootServicesData,
                                SECURITY_BUFFER_LENGTH,
                                (VOID**)&SecurityBuffer );

    if ( EFI_ERROR( Status )) {
        return Status;
    }

    //
    // Clear the Buffer
    //
    ZeroMemory( SecurityBuffer, SECURITY_BUFFER_LENGTH );

    SecurityBuffer[0] = Control;

    //
    // Copy 32 Bytes
    //
    pBS->CopyMem( &(SecurityBuffer[1]), Buffer, IDE_PASSWORD_LENGTH );

    //
    // If Security Set password command is for Master, then update the
    // revision code.
    //
    if ((SecurityCommand == SECURITY_SET_PASSWORD) & (Control & 1))
    {
        if ((RevisionCode != 0) && (RevisionCode != 0xFFFF))
        {
            SecurityBuffer[17] = RevisionCode;
        }
    }

    //
    // Write 512 Bytes
    //
    Status = CommonReadWritePioHook(
        BusInterface,
        SecurityBuffer,
        SECURITY_BUFFER_LENGTH,
        0,
        SecurityCommand,
        TRUE,
        BusInterfaceType
        );

    pBS->FreePool( SecurityBuffer );
    return Status;
}
#if SMMHDDSECURITY_SUPPORT
/**
    Manages HDD Password in SMM area.

    @param AMI_HDD_SECURITY_PROTOCOL   *This,
    @param UINT16                      Control,
    @param UINT8                       *Buffer
    @param UINT8                       Action

    @param Control : Bit 0 : 0/1 User/Master Password

    @param Buffer  : Pointer to password buffer which will be used to when sending
        SecurityCommand.

    @retval EFI_SUCCESS      : Password saved which will be used to unlock the drive
        during S3 resume.
    @retval EFI_DEVICE_ERROR : Error in saving Password.

    @note  
      1. Buffer should be 32 bytes long (Mandatory). Even if Password is not
     32 bytes long, pad it with zeros to create a buffer of 32 bytes.

**/

EFI_STATUS
SmmHDDPasswordInterface (
    AMI_HDD_SECURITY_PROTOCOL   *This,
    UINT16                      Control,
    UINT8                       *Buffer,
    UINT8                       Action
)
{
    UINT8                      SwSmiValue = Action;
    UINTN                      i;
    EFI_SMM_COMMUNICATE_HEADER *SmmCommunicateHeader;
    UINT8                      *CommunicateBuffer = NULL;
    UINTN                      CommunicateBufferSize;
    VOID                       *BusInterface   \
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE    BusInterfaceType   \
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterfaceType;
    EFI_PCI_IO_PROTOCOL *PciIo = NULL;
    UINTN   SegNum;
    UINTN   BusNum;
    UINTN   DevNum;
    UINTN   FuncNum;
    EFI_STATUS  Status;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL  *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL  *EfiNvmExpressPassThru=\
            (EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL*)BusInterface;
#endif

    // HDDPasswordBuffer is allocated only once. The same buffer will be used for all operations.
    if ( HDDPasswordBuffer == NULL )
    {
        Status = pBS->AllocatePool( EfiBootServicesData,
                                    sizeof(HDD_PASSWORD),
                                    (VOID**)&HDDPasswordBuffer );

        if ( EFI_ERROR( Status )) {
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
    }

    ZeroMemory( HDDPasswordBuffer, sizeof(HDD_PASSWORD) );

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde )
    {

        //  If HDD is not going to lose power in S3, no need to unlock password and put it in
        //  Freeze state during S3 resume.
        if ( !(IdeBusInterface->IdeBusInitInterface->Flags   \
               & S3_RESUME_UNLOCK_HDD_PASSWORD))
        {
            return EFI_SUCCESS;
        }

        HDDPasswordBuffer->BaseAddress
            = IdeBusInterface->IdeDevice.Regs.CommandBlock.DataReg;
        HDDPasswordBuffer->Device = IdeBusInterface->IdeDevice.Device;
        HDDPasswordBuffer->DeviceControlReg
            = IdeBusInterface->IdeDevice.Regs.ControlBlock.DeviceControlReg;
        PciIo = IdeBusInterface->PciIO;

    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {

        PciIo=SataDevInterface->AhciBusInterface->PciIO;
        HDDPasswordBuffer->PortNumber   = SataDevInterface->PortNumber;
        HDDPasswordBuffer->PMPortNumber = SataDevInterface->PMPortNumber;
    }
#endif
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceNvme ) {
        PciIo= ((NVM_EXPRESS_PASS_THRU_INSTANCE*)EfiNvmExpressPassThru)->NvmeControllerProtocol->PciIO;
        HDDPasswordBuffer->DevHandle = ((NVM_EXPRESS_PASS_THRU_INSTANCE*)EfiNvmExpressPassThru)->ControllerHandle;
    }
#endif

    // Get PCI Bus/Device/Function number
    Status = PciIo->GetLocation ( PciIo,
                                  &SegNum,
                                  &BusNum,
                                  &DevNum,
                                  &FuncNum
                                  );
    ASSERT_EFI_ERROR (Status);

    // Save it for IdeSmm to initialize the controller.
    HDDPasswordBuffer->BusNo=(UINT8)BusNum;
    HDDPasswordBuffer->DeviceNo=(UINT8)DevNum;
    HDDPasswordBuffer->FunctionNo=(UINT8)FuncNum;

    HDDPasswordBuffer->Signature = '$HDD'; // 0x44444824; //
    HDDPasswordBuffer->Control   = Control;
    HDDPasswordBuffer->BusInterfaceType  = BusInterfaceType;

    //      HDDPasswordBuffer->RevisionCode = RevisionCode;
    if ( Control & 0x01 ) {
        for ( i = 0; i < IDE_PASSWORD_LENGTH; i++ )
        {
            HDDPasswordBuffer->MasterPassword[i] = Buffer[i];
        } // end of for
    }
    else {
        for ( i = 0; i < IDE_PASSWORD_LENGTH; i++ )
        {
            HDDPasswordBuffer->UserPassword[i] = Buffer[i];
        } // end of for
    }

    // Calculate Size of Communication buffer
    CommunicateBufferSize  = SMM_COMMUNICATE_HEADER_SIZE + HDD_PASSWORD_SIZE;


    // Allocate Memory for Communication buffer.   
    Status = pBS->AllocatePool( EfiBootServicesData,
                                    CommunicateBufferSize,
                                    (VOID**)&CommunicateBuffer );
    if ( EFI_ERROR( Status )) {    	
        return Status;
    }    

    // Copy Smm Communicate Header Here
    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)CommunicateBuffer;

    if(SwSmiValue == SW_SMI_HDD_PASSWORD_SAVE) {
        pBS->CopyMem( &SmmCommunicateHeader->HeaderGuid, &gSaveHddPasswordGuid, sizeof( EFI_GUID ) );
    } else if(SwSmiValue == SW_SMI_HDD_PASSWORD_REMOVE) {
        pBS->CopyMem( &SmmCommunicateHeader->HeaderGuid, &gRemoveHddPasswordGuid, sizeof( EFI_GUID ) );
    } else {
        ASSERT(FALSE);
        return EFI_NOT_FOUND;
    }

    SmmCommunicateHeader->MessageLength  = HDD_PASSWORD_SIZE;

    // Copy HDD Password Data Here
    pBS->CopyMem( &SmmCommunicateHeader->Data, HDDPasswordBuffer, sizeof(HDD_PASSWORD) );

    if(gSmmCommunication == NULL) {
        Status = pBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &gSmmCommunication);
        if (EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
    }

    // Send data to SMM.
    Status = gSmmCommunication->Communicate (gSmmCommunication, CommunicateBuffer, &CommunicateBufferSize);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    // Freeing the Memory Allocated for Communication Buffer.
    Status = pBS->FreePool(CommunicateBuffer);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return Status;
    }
    return EFI_SUCCESS;
}
#endif

/**
    Updates Identify command data.IDENTIFY_DATA(Command Code: 0xEC)
    command used to update the Identify data buffer.

        
    @param IdeBusInterface 
    @param BusInterfaceType 

    @retval EFI_SUCCESS : Identify data updated successfully.
    @retval EFI_ERROR   : Failed to update the identify data.

    @note  ATA/ATAPI device type should have been known already.

**/
EFI_STATUS
UpdateIdentifyData (
    IN  VOID                       *BusInterface,
    IN  AMI_STORAGE_BUS_INTERFACE  BusInterfaceType
)
{
    EFI_STATUS                     Status=EFI_SUCCESS;
    COMMAND_STRUCTURE              CommandStructure;
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    NVME_SECURITY_RECEIVE_BUFFER   SecReceiveBuffer;
#endif

    if( BusInterfaceType <= AmiStorageInterfaceAhci ) {
        ZeroMemory( &CommandStructure, sizeof(COMMAND_STRUCTURE));
        CommandStructure.ByteCount = sizeof(IDENTIFY_DATA);
        CommandStructure.Command   = IDENTIFY_COMMAND;
        Status = CommonPioDataHook( BusInterface, CommandStructure, BusInterfaceType );
    } else {  // BusInterfaceType = AmiStorageInterfaceNvme
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
        pBS->SetMem( &SecReceiveBuffer, NVME_SECURITY_RECEIVE_BUFFER_SIZE , 0 );
        Status = GetNvmeDeviceSecurityData (
                        BusInterface,
                        (UINT8*)&SecReceiveBuffer,
                        NVME_SECURITY_RECEIVE_BUFFER_SIZE
                        );

        if( !EFI_ERROR(Status) ) {
            ((NVM_EXPRESS_PASS_THRU_INSTANCE*)BusInterface)->\
                    NvmeControllerProtocol->SecReceiveBuffer = SecReceiveBuffer;
        }
#endif
    }

    ASSERT_EFI_ERROR(Status);
    return Status;
}

#if MASTER_PASSWORD_ENABLE
/**
    Sets the Default Password String provided by OEM as
    Master Password. SECURITY_SET_PASSWORD(Command Code : 0xF1)
    security command is used to set the master password.

    @param AMI_HDD_SECURITY_PROTOCOL        *This,

    @retval EFI_SUCCESS : OEM Master password set successfully.
    @retval EFI_ERROR   : Failed to set OEM Master password to the hard disk.

    @note OEM_MASTER_PASSWORD_TAG token string will be set as OEM master password.

**/

EFI_STATUS
OEMSetMasterPassword (
    IN  AMI_HDD_SECURITY_PROTOCOL   *This
)
{
    EFI_STATUS Status = 0;
    UINT8      Buffer[IDE_PASSWORD_LENGTH];
    UINT16     Control      = 1;
    UINT16     RevisionCode = 0xFFFE;
    UINTN      i;
    UINT16     SecurityStatus = 0;

    MemSet( &Buffer, IDE_PASSWORD_LENGTH, 0 );

    for ( i = 0; i < IDE_PASSWORD_LENGTH; i++ )
    {
        Buffer[i] = (UINT8)sOemMasterPasswordTag[i];

#if defined HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE && HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE 
        Buffer[i] = ((Buffer[i]>='a')&&(Buffer[i]<='z'))?(Buffer[i]+'A'-'a'):Buffer[i];
#endif
        if ( sOemMasterPasswordTag[i] == L'\0' )
        {
            break;
        }
    }

    //
    //Set the OEM master password if the HDD is not in Frozen or locked state.
    //
    ReturnSecurityStatus( This, &SecurityStatus );

    if ( !((SecurityStatus & 0x04) || (SecurityStatus & 0x08)))
    {
        Status = SecuritySetPassword( This, Control, Buffer, RevisionCode );
    }

    return Status;
}

#endif

#if (!DISABLE_SOFT_SET_PREV && !FORCE_HDD_PASSWORD_PROMPT ) || HDD_PASSWORD_ON_NVRAM
/**
    Gathers Ide Device's Bus,Func,Device & PM/PS/SM/SS Info.

    @param BusInterface 
    @param HddDataId 

    @retval EFI_SUCCESS : IDE info retrieved successfully.
    @retval EFI_ERROR   : Failed to get IDE info.

**/
EFI_STATUS
GatherIdeInfo (
    IN  VOID                     *BusInterface,
    AMI_STORAGE_BUS_INTERFACE    BusInterfaceType,
    OUT UINT32                   *HddDataId
)
{
    EFI_STATUS            Status;
    UINTN                 SegNum;
    UINTN                 BusNum;
    UINTN                 DevNum;
    UINTN                 FuncNum;
    UINT16                wBDF;
    UINT16                wPortPMModeFlag=0;
    UINT8                 ChannelorPort;
    UINT8                 DeviceorPMPort;
    EFI_PCI_IO_PROTOCOL   *PciIo;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL  *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif
#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL *EfiNvmExpressPassThru = (EFI_NVM_EXPRESS_PASS_THRU_PROTOCOL*)BusInterface;
#endif
    
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde ) {
        PciIo          = IdeBusInterface->PciIO;
        ChannelorPort  = IdeBusInterface->IdeDevice.Channel;
        DeviceorPMPort =  IdeBusInterface->IdeDevice.Device;
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        PciIo          = SataDevInterface->AhciBusInterface->PciIO;
        ChannelorPort  = SataDevInterface->PortNumber;
        DeviceorPMPort =  SataDevInterface->PMPortNumber;
    }
#endif

#if ( defined(NVMe_SUPPORT) && (NVMe_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceNvme ) {
        PciIo = ((NVM_EXPRESS_PASS_THRU_INSTANCE*)EfiNvmExpressPassThru)->NvmeControllerProtocol->PciIO;
    }
#endif

    Status = PciIo->GetLocation( PciIo, &SegNum, &BusNum, &DevNum, &FuncNum );

    if(EFI_ERROR (Status) ) {
        return Status;
    }

    wBDF = (UINT16)((BusNum << 8) | (DevNum << 3) | FuncNum );

    // wPortPMModeFlag=0 for Nvme
    if ( BusInterfaceType <= AmiStorageInterfaceAhci ) {
        wPortPMModeFlag = (UINT16)((ChannelorPort << 12)   \
                                   | (DeviceorPMPort << 4)   \
                                   | ((UINT8)BusInterfaceType));
    }

    *HddDataId = (UINT32)(((UINT16)wBDF << 16) + ((UINT16)wPortPMModeFlag));

    return EFI_SUCCESS;
}

/**
   Gets Hdd Security Data in NVRAM Area.
  
   @param *BusInterface
   @param *HddDataId
   @param Control
   @param *Buffer
  
   @retval EFI_STATUS
**/
EFI_STATUS 
GetHddSecDataFromNvram(
    IN VOID                      *BusInterface,
    AMI_STORAGE_BUS_INTERFACE    BusInterfaceType,
    UINT8                        *Buffer 
)
{

    UINT32       HddDataId       = 0;
    CHAR16       *HddSecutiyData = NULL;
    HDDSECDATA   *HddSecData;
    EFI_STATUS   Status;
    UINTN        i;
    UINTN        HddSecDataSize = sizeof (HDDSECDATA);
    UINT8        CryptBuffer[IDE_PASSWORD_LENGTH];
    UINT16       Control;

    Status = GatherIdeInfo( BusInterface, BusInterfaceType, &HddDataId );
    ASSERT_EFI_ERROR( Status );

    pBS->AllocatePool( EfiBootServicesData,
                       IDE_PASSWORD_LENGTH,
                       (void**) &HddSecutiyData );

    pBS->SetMem( HddSecutiyData, IDE_PASSWORD_LENGTH, 0 );
    ConvertHddDataIdToString( HddDataId, HddSecutiyData );

    pBS->AllocatePool( EfiBootServicesData, sizeof (HDDSECDATA), (void**) &HddSecData );
    pBS->SetMem( HddSecData, sizeof(HDDSECDATA),  0 );

    pBS->SetMem( Buffer,  IDE_PASSWORD_LENGTH, 0 );

    Status = pRS->GetVariable(
        HddSecutiyData,
        &gIdeSecurityPwNvguid,
        NULL,
        &HddSecDataSize,
        HddSecData
        );

    ASSERT_EFI_ERROR( Status );
    if(EFI_ERROR(Status)) {
        return Status;
    }

    Control = HddSecData->UserMaster;

    if ( Control & 0x01 )
    {
        for ( i = 0; i < sizeof (HddSecData->HddMasterPassword); i++ )
        {
            CryptBuffer[i] = HddSecData->HddMasterPassword[i];
        }

        EncodeDecodePassword( CryptBuffer, Buffer, HddSecData->PasswordLength );
    }
    else {
        for ( i = 0; i < sizeof (HddSecData->HddUserPassword); i++ )
        {
            CryptBuffer[i] = HddSecData->HddUserPassword[i];
        }

        EncodeDecodePassword( CryptBuffer, Buffer, HddSecData->PasswordLength );
    }

    pBS->FreePool( HddSecData );

    return Status;
}


/**
    Sets Hdd Security Data in NVRAM Area.

    @param BusInterface 
    @param UINT32   *HddDataId
    @param UINT16   Control,
    @param UINT8    *Buffer

    @param Control : Bit 0 : 0/1 User/Master Password

    @param Buffer  : Pointer to password buffer which will be used to when sending
        SecurityCommand.

    @retval EFI_SUCCESS : HDDPassword saved in NVram which later will be transfered to
        SMRAM.
    @retval EFI_ERROR   : Failed to save the HDD password in Nvram.
**/
EFI_STATUS
SetHddSecDataInNvram (
    IN  VOID    *BusInterface,
    UINT32      *HddDataId,
    UINT16      Control,
    UINT8       *Buffer
)
{
    EFI_STATUS Status;
    HDDSECDATA *HddSecData;
    UINTN      i;
    CHAR16     *HddSecutiyData = NULL;
    UINT8      CryptBuffer[IDE_PASSWORD_LENGTH];
    UINTN      HddSecDataSize = sizeof (HDDSECDATA);
    UINT32     ParseIndex;
    UINT32     Attribute = 0;

    pBS->AllocatePool(EfiBootServicesData,
                      sizeof (HDDSECDATA),
                      (VOID**)&HddSecData );
    
    pBS->SetMem( HddSecData, sizeof(HDDSECDATA), 0 );
    
    pBS->AllocatePool( EfiBootServicesData,
                       IDE_PASSWORD_LENGTH,
                       (VOID**)&HddSecutiyData );
    
    pBS->SetMem( HddSecutiyData, IDE_PASSWORD_LENGTH, 0 );
    ConvertHddDataIdToString( *HddDataId, HddSecutiyData );

    //
    // Get the password variable so that other password will be
    // maintained in the nvram variable.
    //
    Status = pRS->GetVariable(
        HddSecutiyData,
        &gIdeSecurityPwNvguid,
        &Attribute,
        &HddSecDataSize,
        HddSecData
        );

    if ( EFI_ERROR( Status )) {
        pBS->SetMem( HddSecData, sizeof(HDDSECDATA), 0 );
        if(Attribute == 0) {
            Attribute = (UINT32)(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS);
        }
    }


    for ( ParseIndex = 0; ParseIndex < IDE_PASSWORD_LENGTH; ParseIndex++ )
    {
        if ( Buffer[ParseIndex] == 0 )
        {
            break;
        }
    }

    HddSecData->UserMaster     = Control;
    HddSecData->PasswordLength = ParseIndex;

    if ( Control & 0x01 )
    {
        EncodeDecodePassword( Buffer, CryptBuffer, ParseIndex );

        for ( i = 0; i < ParseIndex; i++ )
        {
            HddSecData->HddMasterPassword[i] = CryptBuffer[i];
        }

        for ( i = ParseIndex; i < sizeof (HddSecData->HddMasterPassword); i++ )
        {
            HddSecData->HddMasterPassword[i] = 00;
        }
    }
    else {
        EncodeDecodePassword( Buffer, CryptBuffer, ParseIndex );

        for ( i = 0; i < ParseIndex; i++ )
        {
            HddSecData->HddUserPassword[i] = CryptBuffer[i];
        }

        for ( i = ParseIndex; i < sizeof (HddSecData->HddUserPassword); i++ )
        {
            HddSecData->HddUserPassword[i] = 00;
        }
    }

    Status = pRS->SetVariable(
        HddSecutiyData,
        &gIdeSecurityPwNvguid,
        Attribute,
        HddSecDataSize,
        HddSecData
        );

    ASSERT_EFI_ERROR( Status );
    pBS->FreePool( HddSecData );

    return Status;
}
#if SMMHDDSECURITY_SUPPORT
/**
    Transfers Stored Data from NVRAM to SMRAM.

    @param AMI_HDD_SECURITY_PROTOCOL    *This,

    @retval EFI_SUCCESS      : Password saved in SMRAM.
    @retval EFI_DEVICE_ERROR : Password not saved in SMRAM.

**/
EFI_STATUS
TransferPwDataFromNvramToSmram (
    AMI_HDD_SECURITY_PROTOCOL   *This
)
{
    UINT32      HddDataId       = 0;
    CHAR16      *HddSecutiyData = NULL;
    HDDSECDATA  *HddSecData;
    VOID        *BusInterface   \
        = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterface;
    AMI_STORAGE_BUS_INTERFACE   BusInterfaceType = ((AMI_HDD_SECURITY_INTERFACE* )This)->BusInterfaceType;
    EFI_STATUS  Status;
    UINT16      Control;
    UINTN       i;
    UINTN       HddSecDataSize = sizeof (HDDSECDATA);
    UINT8       CryptBuffer[IDE_PASSWORD_LENGTH];
    UINT8       Buffer[IDE_PASSWORD_LENGTH];
    UINT16      SecurityStatus = 0;
    
    Status=ReturnSecurityStatus( This, &SecurityStatus );
    
    if(EFI_ERROR(Status)) {
        return Status;
    }

    // Hdd Password Nvram Variable will not be available if the HDD is in locked state. 
    // So return without copying the password to Smram as the password 
    // data is not available
    
    if ( SecurityStatus & 4 ) {
        return EFI_SUCCESS;
    }

    Status = GatherIdeInfo( BusInterface, BusInterfaceType, &HddDataId );
    ASSERT_EFI_ERROR( Status );

    pBS->AllocatePool( EfiBootServicesData,
                       IDE_PASSWORD_LENGTH,
                       (VOID**)&HddSecutiyData );
    pBS->SetMem( HddSecutiyData, IDE_PASSWORD_LENGTH, 0 );
    ConvertHddDataIdToString( HddDataId, HddSecutiyData );

    pBS->AllocatePool( EfiBootServicesData, sizeof (HDDSECDATA), (VOID**)&HddSecData );
    pBS->SetMem( HddSecData, sizeof(HDDSECDATA),  0 );

    pBS->SetMem( Buffer, IDE_PASSWORD_LENGTH, 0 );

    Status = pRS->GetVariable(
        HddSecutiyData,
        &gIdeSecurityPwNvguid,
        NULL,
        &HddSecDataSize,
        HddSecData
        );

    ASSERT_EFI_ERROR( Status );

    Control = HddSecData->UserMaster;

    if ( Control & 0x01 ) {
        for ( i = 0; i < sizeof (HddSecData->HddMasterPassword); i++ ) {
            CryptBuffer[i] = HddSecData->HddMasterPassword[i];
        }
        
        EncodeDecodePassword( CryptBuffer, Buffer, HddSecData->PasswordLength );
    } else {
        for ( i = 0; i < sizeof (HddSecData->HddUserPassword); i++ ) {
            CryptBuffer[i] = HddSecData->HddUserPassword[i];
        }

        EncodeDecodePassword( CryptBuffer, Buffer, HddSecData->PasswordLength );
    }

    SmmHDDPasswordInterface( This,
                             Control,
                             Buffer,
                             SW_SMI_HDD_PASSWORD_SAVE );

    pBS->FreePool( HddSecData );

    return Status;
}
#endif
/**
    Converts Integer HDD Data Id to String.

    @param DataId 
    @param String 
         
    @retval VOID

**/
VOID
ConvertHddDataIdToString (
    UINT32  DataId,
    OUT CHAR16  *String
)
{
    Swprintf( String, L"%d",DataId );
    return;
}

/**
    Encodes/Decodes Password.

    @param InputString 
    @param OutputString 
    @param StringLen 

    @retval VOID

**/
VOID
EncodeDecodePassword (
    IN  UINT8   *InputString,
    OUT UINT8   *OutputString,
    IN  UINT32  StringLength
)
{
    UINT8  *Key;
    UINT32 i;
    UINT32 KeyIndex;
    UINT32 KeyLength;

    if ((InputString == NULL) || (StringLength == 0))
    {
        return;
    }

    KeyLength = (UINT32)( Strlen( HDD_PWD_ENCRYPTION_KEY ));
    pBS->AllocatePool( EfiBootServicesData, KeyLength, (VOID**)&Key );
    pBS->SetMem( Key, KeyLength, 0 );
    pBS->CopyMem( Key, HDD_PWD_ENCRYPTION_KEY, KeyLength );
    ASSERT( Key != NULL );

    for ( i = 0, KeyIndex = 0; i < StringLength; i++, KeyIndex++ )
    {
        if ( KeyIndex == (KeyLength - 1))
        {
            KeyIndex = 0;
        }
        OutputString[i] = (UINT8) ( InputString[i] ^ Key[KeyIndex] );
    }

    pBS->FreePool( Key );
    return;
}
  
#endif


/**
    Common Hook which Calls Non Data Command.

    @param BusInterface 
    @param CommandStructure 
    @param BusInterfaceType 

    @retval EFI_SUCCESS                      : Non Data command completed successfully.
    @retval EFI_DEVICE_ERROR/EFI_UNSUPPORTED : Not supported.

**/
EFI_STATUS
CommonNonDataHook (
    IN  VOID                        *BusInterface,
    IN  COMMAND_STRUCTURE           CommandStructure,
    IN  AMI_STORAGE_BUS_INTERFACE   BusInterfaceType
)
{
    EFI_STATUS            Status = EFI_UNSUPPORTED;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL  *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde )
    {
        CommandStructure.Device = IdeBusInterface->IdeDevice.Device << 4;

        //
        // IdeBus API changed from Core 4.6.5.2. Added Core Version check for the OLD 
        // Core support.
        //
#if defined CORE_COMBINED_VERSION && (CORE_COMBINED_VERSION > 0x4028b) 
        Status = IdeBusInterface->IdeNonDataCommand(
            IdeBusInterface,
            CommandStructure.Features,
            (UINT8)CommandStructure.SectorCount,
            (UINT8)(CommandStructure.SectorCount >> 8),
            CommandStructure.LBALow,
            CommandStructure.LBALowExp,
            CommandStructure.LBAMid,
            CommandStructure.LBAMidExp,
            CommandStructure.LBAHigh,
            CommandStructure.LBAHighExp,
            CommandStructure.Device,
            CommandStructure.Command
            );
#else 
        Status = IdeBusInterface->IdeNonDataCommand(
            IdeBusInterface,
            CommandStructure.Features,
            (UINT8)CommandStructure.SectorCount,
            CommandStructure.LBALow,
            CommandStructure.LBAMid,
            CommandStructure.LBAHigh,
            CommandStructure.Device,
            CommandStructure.Command
            );

#endif
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        Status = SataDevInterface->AhciBusInterface->ExecuteNonDataCommand(
            SataDevInterface,
            CommandStructure
            );
    }
#endif

    return Status;
}

/**
    Common Hook which Calls Pio Data Command.

    @param BusInterface 
    @param CommandStructure 
    @param BusInterfaceType 

    @retval EFI_SUCCESS                      : Pio data command completed.
    @retval EFI_DEVICE_ERROR/EFI_UNSUPPORTED : Not supported.

**/
EFI_STATUS
CommonPioDataHook (
    IN  VOID                         *BusInterface,
    IN  COMMAND_STRUCTURE            CommandStructure,
    IN  AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
)
{
    EFI_STATUS            Status;
    VOID                  *IdentifyBuffer = NULL;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL  *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

    Status = pBS->AllocatePool( EfiBootServicesData,
                                sizeof(IDENTIFY_DATA),
                                (VOID**)&IdentifyBuffer
                                );
    //
    //  Read Identify data
    //
    CommandStructure.Buffer = IdentifyBuffer;

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde )
    {
        CommandStructure.Device = IdeBusInterface->IdeDevice.Device << 4;
        Status                  = IdeBusInterface->AtaPioDataIn(
            IdeBusInterface,
            CommandStructure
                .Buffer,
            CommandStructure
                .ByteCount,
            CommandStructure
                .Features,
            (UINT8)
            CommandStructure.SectorCount,
            CommandStructure
                .LBALow,
            CommandStructure
                .LBAMid,
            CommandStructure
                .LBAHigh,
            CommandStructure
                .Device,
            CommandStructure
                .Command,
            FALSE
            );
        //
        // Copy Identify Data
        //
        pBS->CopyMem( &(IdeBusInterface->IdeDevice.IdentifyData),
                      IdentifyBuffer,
                      sizeof(IDENTIFY_DATA)
                      );
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        Status = SataDevInterface->AhciBusInterface->ExecutePioDataCommand(
            SataDevInterface,
            &CommandStructure,
            FALSE
            );

        //
        // Copy Identify Data
        //
        pBS->CopyMem( &(SataDevInterface->IdentifyData),
                      IdentifyBuffer,
                      sizeof(IDENTIFY_DATA)
                      );
    }
#endif
    
    pBS->FreePool( IdentifyBuffer );

    return Status;
}

/**
    Common Hook which Calls Read Write PIO.

    @param BusInterface 
    @param VOID                            *Buffer,
    @param UINTN                           ByteCount,
    @param UINT64                          LBA
    @param ReadWriteCommand 
    @param ReadWrite Read/Write = 0/1
    @param BusInterfaceType 

    @retval *Buffer updated from PIO command.

**/
EFI_STATUS CommonReadWritePioHook(
    IN VOID                         *BusInterface,
    IN OUT VOID                     *Buffer,
    IN UINTN                        ByteCount,
    IN UINT64                       LBA,
    IN UINT8                        ReadWriteCommand,
    IN BOOLEAN                      ReadWrite,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
)
{
    EFI_STATUS            Status = EFI_UNSUPPORTED;
#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    AMI_IDE_BUS_PROTOCOL  *IdeBusInterface = (AMI_IDE_BUS_PROTOCOL*)BusInterface;
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    SATA_DEVICE_INTERFACE *SataDevInterface = (SATA_DEVICE_INTERFACE*)BusInterface;
#endif

#if ( defined(IdeBusSrc_SUPPORT) && (IdeBusSrc_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceIde )
    {
        Status = IdeBusInterface->AtaReadWritePio(
            IdeBusInterface,
            Buffer,
            ByteCount,
            0,
            ReadWriteCommand,
            TRUE
            );
    }
#endif
#if ( defined(AHCI_SUPPORT) && (AHCI_SUPPORT != 0) )
    if ( BusInterfaceType == AmiStorageInterfaceAhci ) {
        Status = SataDevInterface->AhciBusInterface->SataReadWritePio(
            SataDevInterface,
            Buffer,
            ByteCount,
            0,
            ReadWriteCommand,
            TRUE
            );
    }
#endif

    return Status;
}
#if SMMHDDSECURITY_SUPPORT
/**
    Generate the SMI to Locate the AhciSMM Protocol that is used by IdeSMM 
    and pass the buffer for Unlock command

    @param EFI_EVENT     Event,
    @param VOID          *Context

    @retval VOID
**/
VOID
EFIAPI
LocateAhciSmmServiceEvent (
    EFI_EVENT   Event,
    VOID        *Context
)
{
    EFI_SMM_COMMUNICATE_HEADER  *SmmCommunicateHeader;
    UINT8                       *CommunicateBuffer = NULL;
    UINTN                       CommunicateBufferSize;
    EFI_STATUS                  Status;

    //
    // Calculate Size of Communication buffer
    //
    CommunicateBufferSize  = SMM_COMMUNICATE_HEADER_SIZE + sizeof (UINT8 *);

    //
    // Allocate Memory for Communication buffer.   
    //
    Status = pBS->AllocatePool( EfiBootServicesData,
                                CommunicateBufferSize,
                                (VOID**)&CommunicateBuffer );
    if ( EFI_ERROR( Status )) {     
        return ;
    }
    
    pBS->SetMem( CommunicateBuffer, CommunicateBufferSize, 0 );

    //
    // Copy Smm Communicate Header Here
    //
    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)CommunicateBuffer;
    pBS->CopyMem( &SmmCommunicateHeader->HeaderGuid, &gLocateIdeAhciSmmProtocolGuid, sizeof( EFI_GUID ) );
    SmmCommunicateHeader->MessageLength  = sizeof(UINT8 *);

    // Allocate the Buffer for the Passing the Password Data to 
    // Unlock command in SMM 
    Status = pBS->AllocatePool( EfiReservedMemoryType,
                                512,
                                (VOID**)&SmmCommunicateHeader->Data );

    if(gSmmCommunication == NULL) {
        Status = pBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &gSmmCommunication);
        if (EFI_ERROR(Status)) {
            ASSERT_EFI_ERROR(Status);
            return ;  
        }
    }

    //
    // Send data to SMM.
    //
    Status = gSmmCommunication->Communicate (gSmmCommunication, CommunicateBuffer, &CommunicateBufferSize);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
        return ;  
    }

    // Freeing the Memory Allocated for Communication Buffer.
    Status = pBS->FreePool(CommunicateBuffer);
    if (EFI_ERROR(Status)) {
        ASSERT_EFI_ERROR(Status);
    }
    //APTIOV_SERVER_OVERRIDE_START: Security vulnerability fix.
    //Kill the Event
    pBS->CloseEvent(Event);
	//APTIOV_SERVER_OVERRIDE_END: Security vulnerability fix.
    return;
}
#endif

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
