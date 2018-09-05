//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Add setup item "SATA Frozen" for enable/disable SATA freeze lock security feature. 
//    Reason:   
//    Auditor:  Gino Yang
//    Date:     Apr/1/2016
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

/** @file AhciBus.c
    Provides AHCI Block IO protocol

**/

//---------------------------------------------------------------------------

#include "AhciBus.h"
#include <AcpiRes.h>
#include "Protocol/LegacyAhci.h"
#include <Protocol/StorageSecurityCommand.h>
//---------------------------------------------------------------------------

#if SBIDE_SUPPORT
#include "SBIDE.h" 
#endif
#if SMCPKG_SUPPORT
#include <Setup.h>					
#include <Guid/SetupVariable.h>	
#endif // #if SMCPKG_SUPPORT
#if BOOT_SECTOR_WRITE_PROTECT
#include <Protocol/AmiBlockIoWriteProtection.h>
AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL *AmiBlkWriteProtection = NULL;
#endif

#if defined(ACOUSTIC_MANAGEMENT_DRIVER_SUPPORT) && (ACOUSTIC_MANAGEMENT_DRIVER_SUPPORT != 0)
#include <Protocol/AcousticProtocol.h>
EFI_GUID gHddAcousticInitProtocolGuid = AMI_HDD_ACOUSTIC_INIT_PROTOCOL_GUID;
AMI_HDD_ACOUSTIC_INIT_PROTOCOL *gHddAcousticInitProtocol = NULL;
#endif

#ifndef EFI_COMPONENT_NAME2_PROTOCOL_GUID
EFI_GUID gComponentNameProtocolGuid = EFI_COMPONENT_NAME_PROTOCOL_GUID;
#else
EFI_GUID gComponentNameProtocolGuid = EFI_COMPONENT_NAME2_PROTOCOL_GUID;
#endif

extern  EFI_GUID    gAmiGlobalVariableGuid;
static EFI_GUID gOnboardRaidControllerGuid    = AMI_ONBOARD_RAID_CONTROLLER_GUID;
static EFI_GUID gHddPasswordVerifiedGuid      = AMI_HDD_PASSWORD_VERIFIED_GUID;

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

#if SBIDE_SUPPORT
EFI_GUID gIdeSetupProtocolguid                  = IDE_SETUP_PROTOCOL_GUID;
VOID InitMiscConfig(IN SATA_DEVICE_INTERFACE    *SataDevInterface);
#endif


#if INDEX_DATA_PORT_ACCESS
extern      InitilizeIndexDataPortAddress();
#endif

extern
EFI_STATUS
ExecutePacketCommand (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface, 
    IN COMMAND_STRUCTURE        *CommandStructure,
    IN BOOLEAN                  READWRITE
);

extern EFI_GUID gAmiIdeBusInitProtocolGuid;
extern EFI_COMPONENT_NAME2_PROTOCOL gAhciBusControllerDriverName;

AHCI_CONTOLLER_LINKED_LIST AhciControllerLinkedList;

EFI_DRIVER_BINDING_PROTOCOL gAhciBusDriverBinding = {
  AhciBusSupported, 
  AhciBusStart,     
  AhciBusStop,      
  AHCI_BUS_DRIVER_VERSION,  // version
  NULL,                     // ImageHandle
  NULL                      // DriverBindingHandle
};

AMI_HDD_SECURITY_INIT_PROTOCOL      *HddSecurityInitProtocol = NULL;  
AMI_HDD_SECURITY_INIT_PROTOCOL      *TcgStorageSecurityInitProtocol = NULL;
VOID                                *TempHddSecurityProtocolPtr;
AMI_HDD_SMART_INIT_PROTOCOL         *HddSmartInitProtocol = NULL;     
AMI_HDD_OPAL_SEC_INIT_PROTOCOL      *OpalSecInitProtocol = NULL;
AMI_ATA_PASS_THRU_INIT_PROTOCOL     *AtaPassThruInitProtocol = NULL;  
AMI_SCSI_PASS_THRU_INIT_PROTOCOL    *gScsiPassThruInitProtocol = NULL; 
AHCI_PLATFORM_POLICY_PROTOCOL       *AhciPlatformPolicy = NULL;

EFI_EVENT                           AhciReadyToBootEvent=NULL;

VOID 
EFIAPI
AhciFreezeLockDevice (
    IN EFI_EVENT    Event,
    IN VOID         *Context
);

// Instantiate AHCI_PLATFORM_POLICY_PROTOCOL with default values 
AHCI_PLATFORM_POLICY_PROTOCOL        gDefaultAhciPlatformPolicy = {
    FALSE,                              // Legacy Raid option selected 
    TRUE,                               // AhciBus driver handles the ATAPI devices
    FALSE,                              // Drive LED on ATAPI Enable (DLAE)
#ifdef POWERUP_IN_STANDBY_SUPPORT
    POWERUP_IN_STANDBY_SUPPORT,         // PowerUpInStandby feature is supported or not
#else
    FALSE,
#endif
#ifdef POWERUP_IN_STANDBY_MODE
    POWERUP_IN_STANDBY_MODE,            // PowerUpInStandby mode
#else
    FALSE,
#endif
#ifdef DiPM_SUPPORT
    DiPM_SUPPORT,                        // Device Initiated power management
#else
    FALSE,
#endif
#ifdef ENABLE_DIPM
    ENABLE_DIPM,
#else
    FALSE,
#endif
#ifdef DEVICE_SLEEP_SUPPORT
    DEVICE_SLEEP_SUPPORT,
#else
    FALSE,
#endif
#ifdef ENABLE_DEVICE_SLEEP
    ENABLE_DEVICE_SLEEP,
#else
    FALSE,
#endif
#ifdef USE_PCIIO_MAP_ADDRESS_FOR_DATA_TRANSFER
    USE_PCIIO_MAP_ADDRESS_FOR_DATA_TRANSFER
#else
    FALSE
#endif
};

#ifndef READ_LOG_EXT
#define     READ_LOG_EXT                    0x2F
#endif

#ifndef IDENTIFY_DEVICE_DATA_LOG
#define     IDENTIFY_DEVICE_DATA_LOG        0x30
#endif

#ifndef SERIAL_ATA_SETTINGS_PAGE
#define     SERIAL_ATA_SETTINGS_PAGE        0x08
#endif

#ifndef DEVSLP_TIMING_VARIABLES_OFFSET
#define     DEVSLP_TIMING_VARIABLES_OFFSET  0x30
#endif

#ifndef DEVSLEEP_EXIT_TIMEOUT
#define     DEVSLEEP_EXIT_TIMEOUT           20
#endif

#ifndef MINIMUM_DEVSLP_ASSERTION_TIME
#define     MINIMUM_DEVSLP_ASSERTION_TIME   10
#endif

#ifndef IDENTIFY_DIPM_ENABLED
#define     IDENTIFY_DIPM_ENABLED                       0x08  // Identify Data Word 79 Bit 3 : Device initiating interface power management Enabled
#endif

///  Equates used for DevSleep Support
#ifndef DEVSLEEP_SUB_COMMAND
#define     DEVSLEEP_SUB_COMMAND                        0x09  // Count value in SetFeature identification : 09h  Device Sleep 
#endif

#ifndef DEVSLEEP_ENABLE
#define     DEVSLEEP_ENABLE                             0x10
#endif

#ifndef DEVSLEEP_DISABLE
#define     DEVSLEEP_DISABLE                            0x90
#endif

#ifndef IDENTIFY_DEVSLEEP_ENABLED
#define     IDENTIFY_DEVSLEEP_ENABLED                   0x100 // Identify Data Word 79 Bit 8 : Device Sleep Enabled
#endif

#ifndef IDENTIFY_DEVSLEEP_SUPPORT
#define     IDENTIFY_DEVSLEEP_SUPPORT                   0x100 // Identify Data Word 78 Bit 8 : Supports Device Sleep
#endif

#ifndef IDENTIFY_DEVSLP_TO_REDUCED_PWRSTATE_CAPABLE
#define     IDENTIFY_DEVSLP_TO_REDUCED_PWRSTATE_CAPABLE 0x80  // Identify Data Word 77 Bit 7 : Device Sleep in reduced Power state capable
#endif
/**
    Installs gAhciBusDriverBinding protocol

    @param    ImageHandle 
    @param    SystemTable 

    @retval   EFI_STATUS

    @note  
          Here is the control flow of this function:
          1. Initialize AmiLib.
          2. Install Driver Binding Protocol
          3. Return EFI_SUCCESS.
**/

EFI_STATUS 
EFIAPI 
AhciBusEntryPoint (
    IN EFI_HANDLE          ImageHandle,
    IN EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS  Status;

    gAhciBusDriverBinding.DriverBindingHandle=NULL;
    gAhciBusDriverBinding.ImageHandle=ImageHandle;

    InitAmiLib(ImageHandle, SystemTable);
    DListInit(&(AhciControllerLinkedList.AhciControllerList));
    Status = pBS->InstallMultipleProtocolInterfaces(
                &gAhciBusDriverBinding.DriverBindingHandle,
                &gEfiDriverBindingProtocolGuid,&gAhciBusDriverBinding,
                &gComponentNameProtocolGuid, &gAhciBusControllerDriverName,
                NULL
                );

    return Status;
}

/**
    Checks whether EFI_IDE_CONTROLLER_INIT_PROTOCOL_GUID 
    is installed on the controller. If 'yes', return SUCCESS else ERROR

        
    @param    This 
    @param    Controller 
    @param    RemainingDevicePath 

    @retval   EFI_STATUS

    @note  
      Here is the control flow of this function:
     1. If Devicepath is NULL, check "gEfiIdeControllerInitProtocolGuid"
        is installed by IdeController device driver,if yes, it is the 
        IDE controller that this Bus will manage. Then return Success.
     2. If Devicepath is valid, check if it is a SATA device Path. See 
        if gAmiAhciBusProtocolGuid is installed on the device.
     3. make sure the the Controller class code is AHCI

**/

EFI_STATUS 
EFIAPI 
AhciBusSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     Controller,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath 
)
{

    EFI_STATUS                       Status;
    EFI_IDE_CONTROLLER_INIT_PROTOCOL IdeControllerInterface;
    SATA_DEVICE_PATH                 *SataRemainingDevicePath = (SATA_DEVICE_PATH *) RemainingDevicePath;
    AMI_AHCI_BUS_PROTOCOL            *AhciBusInterface;
    SATA_DEVICE_INTERFACE            *SataDevInterface = NULL;
    EFI_PCI_IO_PROTOCOL              *PciIO;
    UINT8                            PciConfig[256];

    // Check for Valid SATA Device Path. If no return UNSUPPORTED
    if (!(SataRemainingDevicePath == NULL)) {
        // Check if the SataRemainingDevicePath is valid 8.3.4.1 
        if (SataRemainingDevicePath->Header.Type != MESSAGING_DEVICE_PATH ||
            SataRemainingDevicePath->Header.SubType != MSG_USB_SATA_DP &&
            NODE_LENGTH(&SataRemainingDevicePath->Header) != SATA_DEVICE_PATH_LENGTH) {
            return EFI_UNSUPPORTED;
        }

        // Now check whether it is OK to enumerate the specified device.
        Status = pBS->OpenProtocol(Controller,
                                   &gAmiAhciBusProtocolGuid,
                                   (VOID **)&AhciBusInterface,
                                   This->DriverBindingHandle,
                                   Controller,
                                   EFI_OPEN_PROTOCOL_BY_DRIVER );
        if (Status == EFI_SUCCESS || Status == EFI_ALREADY_STARTED) {
            pBS->CloseProtocol (Controller,
                                &gAmiAhciBusProtocolGuid,
                                This->DriverBindingHandle,
                                Controller);

            SataDevInterface = GetSataDevInterface(
                                AhciBusInterface, 
                                (UINT8)SataRemainingDevicePath->HBAPortNumber,
                                (UINT8)SataRemainingDevicePath->PortMultiplierPortNumber
                               );

            // If the device in the remaining device path Port already detected and configured 
            // return as EFI_ALREADY_STARTED. If the device is not yet detected in the 
            // remaining device path port, proceed for the device detection and configuration
            if (SataDevInterface && (SataDevInterface->DeviceState >= DEVICE_DETECTION_FAILED)) {
                return EFI_ALREADY_STARTED;
            }
            else {
                return EFI_SUCCESS;
            }
        }
    }

    // Check if the IDEBUS installed on the controller. If it is installed 
    // IdeBus driver already handling the Controller. So AHCIBUS driver should not handle
    // the controller 
    Status = pBS->OpenProtocol(Controller,
                               &gAmiIdeBusInitProtocolGuid,
                               NULL,
                               This->DriverBindingHandle,
                               Controller,
                               EFI_OPEN_PROTOCOL_TEST_PROTOCOL );

    if (Status == EFI_SUCCESS) {
        // IdeBus handling the controller. Return with Error.
        return EFI_UNSUPPORTED;
    }

    // Check whether IDE_CONTROLLER_PROTOCOL  has been installed on 
    // this controller  
    Status = pBS->OpenProtocol(Controller,
                               &gEfiIdeControllerInitProtocolGuid,
                               (VOID **)&IdeControllerInterface,
                               This->DriverBindingHandle,
                               Controller,
                               EFI_OPEN_PROTOCOL_BY_DRIVER);

    // IDE_CONTROLLER_PROTOCOL will be opened by each device. So 
    // EFI_ALREADY_STARTED is not an error.
    if  ( !(Status == EFI_SUCCESS || Status == EFI_ALREADY_STARTED)) {
        return EFI_UNSUPPORTED;
    }

    // Close "gEfiIdeControllerInitProtocolGuid" Protocol only if Open protocol is "success"
    // Dont Close the Protoocl if Return Status is "EFI_ALREADY_STARTED" (As Device will not
    // be detected again if it was detected and configured in RAID mode already)
    if  ( Status == EFI_SUCCESS ) {
        //
        // Close IDE_CONTROLLER_PROTOCOL
        //
        pBS->CloseProtocol (
            Controller,
            &gEfiIdeControllerInitProtocolGuid,
            This->DriverBindingHandle,
            Controller
        );
    }

    // Check if Controller is in AHCI mode or not?
    Status = pBS->OpenProtocol(Controller,
                               &gEfiPciIoProtocolGuid,
                               (VOID **)&PciIO,
                               This->DriverBindingHandle,     
                               Controller,   
                               EFI_OPEN_PROTOCOL_GET_PROTOCOL);
     if (EFI_ERROR(Status)) {
         return EFI_UNSUPPORTED;
     }

     Status = PciIO->Pci.Read (PciIO,
                               EfiPciIoWidthUint8,
                               0,
                               sizeof (PciConfig),
                               PciConfig);
    ASSERT_EFI_ERROR(Status);

    if (PciConfig [IDE_SUB_CLASS_CODE] == SCC_AHCI_CONTROLLER) {
#ifdef AHCI_COMPATIBLE_MODE
    #if !(AHCI_COMPATIBLE_MODE)
        return EFI_SUCCESS;
    #endif
#else
    return EFI_SUCCESS;
#endif
    }

#ifdef SUPPORT_ATAPI_IN_RAID_MODE
#ifdef HDD_PASSWORD_SUPPORT_UNDER_RAIDMODE
    #if SUPPORT_ATAPI_IN_RAID_MODE || HDD_PASSWORD_SUPPORT_UNDER_RAIDMODE
    if (PciConfig [IDE_SUB_CLASS_CODE] == SCC_RAID_CONTROLLER) {

        // Under Raid mode, don't detect the devices again 
        // Check if AMI_AHCI_BUS_PROTOCOL installed status. If already installed
        // AhciBus started already and device detection done
        Status = pBS->OpenProtocol(Controller,
                                   &gAmiAhciBusProtocolGuid,
                                   NULL,
                                   This->DriverBindingHandle,
                                   Controller,
                                   EFI_OPEN_PROTOCOL_TEST_PROTOCOL);


        if  ( EFI_ERROR(Status)) {
            return EFI_SUCCESS;
        } else {
            return EFI_ALREADY_STARTED;
        }
    }
    #endif
#endif
#endif

    return EFI_UNSUPPORTED; 
}


/**
    Installs AHCI Block IO Protocol

    @param    This 
    @param    Controller 
    @param    RemainingDevicePath 

    @retval   EFI_STATUS

    @note  
         1. Collect the info about the number of devices to detect and configure.
         2. Configure the AHCI controller if it is not done yet.
         3. Detect the device connected to the port
         4. If the device is a Port Multiplier, detect & configure all the 
            devices behind it, else configure the device directly connected
            to the port.
         5. Continue step 3 and 4 for all the ports on the controller.

**/

EFI_STATUS
EFIAPI 
AhciBusStart (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     Controller,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath 
)
{

    EFI_STATUS                        Status;
    EFI_PCI_IO_PROTOCOL               *PciIO;
    EFI_IDE_CONTROLLER_INIT_PROTOCOL  *IdeControllerInterface;
    SATA_DEVICE_INTERFACE             *SataDevInterface = NULL;
    AMI_AHCI_BUS_PROTOCOL             *AhciBusInterface;        
    SATA_DEVICE_PATH                  *SataRemainingDevicePath = (SATA_DEVICE_PATH *)RemainingDevicePath;
    UINT8                             PortEnumeration = 0xFF, PMPortEnumeration = 0xFF; // Bit Map
    UINT8                             CurrentPort = 0, CurrentPMPort = 0xFF; 
    BOOLEAN                           Enabled = TRUE;
    UINT8                             MaxDevices = 0; 
    UINT8                             Data8;    
    VOID                              *TempProtocolPtr;
#if HDD_PASSWORD_SUPPORT_UNDER_RAIDMODE
    EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
    BOOLEAN                           RaidDriverBlocked=FALSE;
#endif
    PROGRESS_CODE(DXE_IDE_BEGIN);

#if BOOT_SECTOR_WRITE_PROTECT
    if(AmiBlkWriteProtection == NULL) {
        Status = pBS->LocateProtocol( &gAmiBlockIoWriteProtectionProtocolGuid, 
                                      NULL, 
                                      &AmiBlkWriteProtection ); 
        if(EFI_ERROR(Status)) {
            AmiBlkWriteProtection = NULL;
        }
    }
#endif

    // Open IDE_CONTROLLER_PROTOCOL. If success or Already opened, It is OK to proceed.
    Status = pBS->OpenProtocol( Controller,
                                &gEfiIdeControllerInitProtocolGuid,
                                (VOID **)&IdeControllerInterface,
                                This->DriverBindingHandle,     
                                Controller,   
                                EFI_OPEN_PROTOCOL_BY_DRIVER );

    if  ( !(Status == EFI_SUCCESS || Status == EFI_ALREADY_STARTED)) {
        return EFI_DEVICE_ERROR;
    }

    // Get the PciIO interface
    Status = pBS->OpenProtocol( Controller,
                                &gEfiPciIoProtocolGuid,
                                (VOID **)&PciIO,
                                This->DriverBindingHandle,     
                                Controller,   
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }

    // Get the Ahci Platform Policy Protocol
    Status=pBS->LocateProtocol( &gAmiAhciPlatformPolicyProtocolGuid,
                                NULL,
                                (VOID **)&AhciPlatformPolicy );

    if(EFI_ERROR(Status)) {
        // If the Ahci Platform policy protocol not found, initilize with default value
        AhciPlatformPolicy=(AHCI_PLATFORM_POLICY_PROTOCOL *)&gDefaultAhciPlatformPolicy;
    }

    // Check if AMI_AHCI_BUS_PROTOCOL installed.
    Status = pBS->OpenProtocol( Controller,
                                &gAmiAhciBusProtocolGuid,
                                (VOID **)&AhciBusInterface,
                                This->DriverBindingHandle,     
                                Controller,   
                                EFI_OPEN_PROTOCOL_BY_DRIVER);

    if  ( !(Status == EFI_SUCCESS || Status == EFI_ALREADY_STARTED)) {

        Status = pBS->AllocatePool ( EfiBootServicesData,
                                     sizeof(AMI_AHCI_BUS_PROTOCOL),
                                     (VOID**)&AhciBusInterface
                                     );

        if (EFI_ERROR(Status)) {
            pBS->CloseProtocol (Controller,
                                &gEfiIdeControllerInitProtocolGuid,
                                This->DriverBindingHandle,
                                Controller);
            return EFI_OUT_OF_RESOURCES; //No need to close IDE_CONTROLLER_PROTOCOL
        }

        Status = InstallAhciBusProtocol (Controller,
                                         AhciBusInterface,
                                         IdeControllerInterface,
                                         PciIO);
        if (EFI_ERROR(Status)) { 
            pBS->CloseProtocol (Controller,
                                &gEfiIdeControllerInitProtocolGuid,
                                This->DriverBindingHandle,
                                Controller);
            return EFI_DEVICE_ERROR;    
        }
    }

    if (!IdeControllerInterface->EnumAll) {
    // Check if sataRemainingDevicePath is valid or not
        if (!(SataRemainingDevicePath == NULL)) {
         // Check if the SataRemainingDevicePath is valid 8.3.4.1 
            if (SataRemainingDevicePath->Header.Type != MESSAGING_DEVICE_PATH ||
                SataRemainingDevicePath->Header.SubType != MSG_USB_SATA_DP &&
                NODE_LENGTH(&SataRemainingDevicePath->Header) != SATA_DEVICE_PATH_LENGTH) { 
                    return EFI_DEVICE_ERROR;
            }
            // Get the Port# that needs to be processed.
            PortEnumeration = 1 << SataRemainingDevicePath->HBAPortNumber;     //Bit Map
            PMPortEnumeration = 1 << SataRemainingDevicePath->PortMultiplierPortNumber;      // Bit Map
            CurrentPMPort =  (UINT8) SataRemainingDevicePath->PortMultiplierPortNumber;
        }
    }
    else {
        PortEnumeration = AhciBusInterface->HBAPortImplemented;
    }
    
    TRACE_AHCI((-1,"\nAHCI Driver Detection and Configuration starts\n"));

    //--------------------------------------------------------------------------
    //  Detection and Configuration starts
    //--------------------------------------------------------------------------
    for (  ; PortEnumeration != 0 ; PortEnumeration >>= 1, CurrentPort++, CurrentPMPort = 0xFF) {

        if(!(PortEnumeration & 1)) {
            continue;
        }    

        // Check if the current port is implemented or not?
        Status = CheckPortImplemented(AhciBusInterface, CurrentPort);       
        if (EFI_ERROR(Status)) { continue;}

        Status = IdeControllerInterface->NotifyPhase (IdeControllerInterface,
                                                      EfiIdeBeforeChannelEnumeration,
                                                      CurrentPort);
        if (EFI_ERROR(Status)) { continue;}

        Status = IdeControllerInterface->GetChannelInfo(IdeControllerInterface,
                                                        CurrentPort,
                                                        &Enabled,
                                                        &MaxDevices);
        if (EFI_ERROR(Status) || !Enabled) { goto NextDevice; }

        Status = DetectAndConfigureDevice(This,
                                          Controller,
                                          RemainingDevicePath,
                                          AhciBusInterface,
                                          IdeControllerInterface,
                                          CurrentPort,
                                          CurrentPMPort);

        SataDevInterface = GetSataDevInterface(AhciBusInterface,
                                               CurrentPort,
                                               CurrentPMPort);

        // Check whether the device detected is PM. Also check whether PM is supported by the Controller
        // and also MaxDevices should be more than 1 if PM is Supported.
        if (!EFI_ERROR(Status) && SataDevInterface && SataDevInterface->NumPMPorts &&
                (SataDevInterface->DeviceType == PMPORT) && MaxDevices > 1){

            Data8 = SataDevInterface->NumPMPorts > MaxDevices  ? MaxDevices : SataDevInterface->NumPMPorts;
            PMPortEnumeration = 1;
            for (Data8-- ;Data8; Data8-- ){
                PMPortEnumeration = (PMPortEnumeration << 1) | 1;
            }
            
            // Port Multiplier loop
            for (CurrentPMPort = 0; PMPortEnumeration & 1 ; PMPortEnumeration >>= 1, CurrentPMPort++ ){
                DetectAndConfigureDevice(This,
                                         Controller,
                                         RemainingDevicePath,
                                         AhciBusInterface,
                                         IdeControllerInterface,
                                         CurrentPort,
                                         CurrentPMPort);
            }
        }

NextDevice:
        IdeControllerInterface->NotifyPhase(IdeControllerInterface,
                                            EfiIdeAfterChannelEnumeration,
                                            CurrentPort);

    }


    TRACE_AHCI((-1," AHCI Driver Detection and Configuration Ends\n"));

    Status = pBS->LocateProtocol (&gAmiAtaPassThruInitProtocolGuid,
                                  NULL,
                                  (VOID **)&AtaPassThruInitProtocol);

    if(!EFI_ERROR(Status)) {
        if(AtaPassThruInitProtocol != NULL) {
            AtaPassThruInitProtocol->InstallAtaPassThru(Controller, TRUE);
        }
    }

    // SCSIPassThruAtapi install
    Status = pBS->LocateProtocol (&gAmiScsiPassThruInitProtocolGuid,
                                  NULL,
                                  (VOID **)&gScsiPassThruInitProtocol);

    if(!EFI_ERROR(Status)) {
        if(gScsiPassThruInitProtocol != NULL) {
            gScsiPassThruInitProtocol->InstallScsiPassThruAtapi(Controller, TRUE);
        }
    }

    InstallOtherOptionalFeatures(AhciBusInterface);

    // Handle the Onboard Raid controller Password Verification
    Status = pBS->HandleProtocol(Controller, 
                                    &gAmiHddSecurityEndProtocolGuid, 
                                    &TempProtocolPtr);

    if(Status == EFI_SUCCESS) {
        //
        // Protocol already installed on the Controller handle.
        // UnInstall and Install back the protocol interface to Notify the Password verification 
        //
        Status = pBS->UninstallProtocolInterface(
                                Controller, 
                                &gAmiHddSecurityEndProtocolGuid, 
                                NULL
                                );
        
        ASSERT_EFI_ERROR(Status);
        
    }
    
    // This will notify AMITSE to invoke the HDD password Screen
    Status = pBS->InstallProtocolInterface(
                                &Controller, 
                                &gAmiHddSecurityEndProtocolGuid, 
                                EFI_NATIVE_INTERFACE,
                                NULL
                                );
        
    ASSERT_EFI_ERROR(Status);
#if HDD_PASSWORD_SUPPORT_UNDER_RAIDMODE
    if(!EFI_ERROR(Status) && TempHddSecurityProtocolPtr != NULL) {

        // Handle the Onboard Raid controller Password Verification
        Status = pBS->HandleProtocol(Controller, 
                                    &gOnboardRaidControllerGuid, 
                                    (VOID**)&TempProtocolPtr);
         if(!EFI_ERROR(Status)) {

            // Check the Hdd Password verification done. If the password 
            // Verification done, proceed for RAID driver launch. Otherwise
            // Hold the Raid driver until Password verification finished.
            Status = pBS->HandleProtocol(Controller, 
                                         &gHddPasswordVerifiedGuid, 
                                         (VOID**)&TempProtocolPtr);

            if(EFI_ERROR(Status)) {
                if(!CheckForLockedDrives(AhciBusInterface)) {
                    // If No Drive is locked under this controller
                    // Install gAmiHddPasswordVerifiedGuid.This is to
                    // Allow CsmBlockIo to launch RAID option ROM.
                           
                    Status = pBS->InstallProtocolInterface( &Controller,
                                                            &gHddPasswordVerifiedGuid,
                                                            EFI_NATIVE_INTERFACE,
                                                            NULL);
                    ASSERT_EFI_ERROR(Status);
                    return Status;
                }
                // Don't launch the Raid Option rom until password verified
                Status = pBS->OpenProtocol (Controller,
                                            &gEfiDevicePathProtocolGuid,
                                            (VOID *) &DevicePath,
                                            This->DriverBindingHandle,
                                            Controller,
                                            EFI_OPEN_PROTOCOL_BY_DRIVER);
                ASSERT_EFI_ERROR(Status);
                if(Status == EFI_SUCCESS) {
                    RaidDriverBlocked=TRUE;
                    Status = pRS->SetVariable(L"RaidDriverBlockingStatus",
                                        &gAmiGlobalVariableGuid,
                                        EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                        sizeof(RaidDriverBlocked),
                                        &RaidDriverBlocked );
                    ASSERT_EFI_ERROR(Status);
                }
            }
        }
    }
#endif
    return EFI_SUCCESS;
}

/**
    Detects and Configures Sata Device

        
    @param    This 
    @param    Controller 
    @param    RemainingDevicePath 
    @param    AMI_AHCI_BUS_PROTOCOL             *AhciBusInterface,
    @param    EFI_IDE_CONTROLLER_INIT_PROTOCOL  *IdeControllerInterface,
    @param    UINT8                             Port,
    @param    UINT8                             PMPort

    @retval   EFI_STATUS

    @note  
         1. Detect whether device is connected to the port. If no device exit.
         2. Install SataDevInterface. If PMPort, Configure PMPort and Exit.
         3. Configure the SATA device and the controller.
         4. Install DevicePath, BlockIO and DiskInfo protocol.

**/

EFI_STATUS
DetectAndConfigureDevice (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     Controller,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath,
    AMI_AHCI_BUS_PROTOCOL             *AhciBusInterface,
    EFI_IDE_CONTROLLER_INIT_PROTOCOL  *IdeControllerInterface,
    UINT8                             Port,
    UINT8                             PMPort
)
{
    EFI_STATUS                 Status;
    SATA_DEVICE_INTERFACE      *SataDevInterface = NULL;
    EFI_ATA_COLLECTIVE_MODE    *SupportedModes = NULL;
    UINT16                     SecurityStatus = 0;

    SataDevInterface = GetSataDevInterface(AhciBusInterface, Port, PMPort);

    if (SataDevInterface && ((SataDevInterface->DeviceState == DEVICE_DETECTION_FAILED)||
        (SataDevInterface->DeviceState == DEVICE_CONFIGURED_SUCCESSFULLY))){ 
        return EFI_SUCCESS;
    }
    
    Status = AhciDetectDevice(AhciBusInterface, IdeControllerInterface, Port, PMPort);
    SataDevInterface = GetSataDevInterface(AhciBusInterface, Port, PMPort);
    if (EFI_ERROR(Status)) { 
        if (SataDevInterface) SataDevInterface->DeviceState = DEVICE_DETECTION_FAILED;
        return EFI_DEVICE_ERROR;
    }

    if (!SataDevInterface) { 
        return EFI_DEVICE_ERROR;
    }

    SataDevInterface->DeviceState = DEVICE_DETECTED_SUCCESSFULLY;

    // if this is a Port Multiplier skip the rest
    if (SataDevInterface->DeviceType == PMPORT) {
        SataDevInterface->DeviceState = DEVICE_CONFIGURED_SUCCESSFULLY;

        //Update Port Multiplier Data
        Status = ConfigurePMPort(SataDevInterface);
        if (!EFI_ERROR(Status)) {
            TRACE_AHCI((-1,"AHCI: SATA Device type %x detected at Port Number : %x, PM Port Number : %x\n",
                SataDevInterface->DeviceType, SataDevInterface->PortNumber, SataDevInterface->PMPortNumber));
        }
        return Status;
    }

    Status = ConfigureDevice(SataDevInterface, &SupportedModes);
    if (EFI_ERROR(Status)) { 
        ERROR_CODE(DXE_IDE_DEVICE_FAILURE, EFI_ERROR_MAJOR);
        SataDevInterface->DeviceState = DEVICE_DETECTION_FAILED;
        return EFI_DEVICE_ERROR;
    }

    Status = ConfigureController(SataDevInterface, SupportedModes);
    if (EFI_ERROR(Status)) { 
        ERROR_CODE(DXE_IDE_DEVICE_FAILURE, EFI_ERROR_MAJOR);
        SataDevInterface->DeviceState = DEVICE_DETECTION_FAILED;
        return EFI_DEVICE_ERROR;
    }

    SataDevInterface->DeviceState = DEVICE_CONFIGURED_SUCCESSFULLY;

    Status = ConfigureSataPort(SataDevInterface);

    if (EFI_ERROR(Status)) { 
        return EFI_DEVICE_ERROR; 
    }

    // Create the Device path
    Status = CreateSataDevicePath (This, Controller, SataDevInterface, RemainingDevicePath);
    if (EFI_ERROR(Status)) { return EFI_DEVICE_ERROR; }

    // Initialize Block_IO Protocol
    Status = InitSataBlockIO (SataDevInterface);
    if (EFI_ERROR(Status)){
        return EFI_DEVICE_ERROR;
    }

    // Initialize IDE EFI_DISK_INFO_PROTOCOL
    Status = InitSataDiskInfo (SataDevInterface);
    if (EFI_ERROR(Status)){
        return EFI_DEVICE_ERROR;
    }

    // Install Device path
    Status = pBS->InstallMultipleProtocolInterfaces (
            &(SataDevInterface->IdeDeviceHandle),
            &gEfiDevicePathProtocolGuid,
            SataDevInterface->DevicePathProtocol,
            NULL);

    if(EFI_ERROR(Status)) {
        SataDevInterface->DeviceState = DEVICE_DETECTION_FAILED;
        if (EFI_ERROR(Status)){
            return EFI_DEVICE_ERROR;
        }
    }

    // Open IdeControllerProtocol 
    Status = pBS->OpenProtocol(Controller,
                &gEfiIdeControllerInitProtocolGuid,
                (VOID **)&IdeControllerInterface,
                This->DriverBindingHandle,     
                SataDevInterface->IdeDeviceHandle,   
                EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);

    ASSERT_EFI_ERROR(Status);
    
    // Check BlockIO has been installed or not.
    Status = pBS->OpenProtocol( SataDevInterface->IdeDeviceHandle,
                                &gEfiBlockIoProtocolGuid,
                                NULL,
                                This->DriverBindingHandle,
                                SataDevInterface->IdeDeviceHandle,
                                EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

    if (EFI_ERROR(Status)) {

        // BLOCKIO not installed and device has been configured successfully
        Status = EFI_UNSUPPORTED;
        SecurityStatus = 0; 
    }

    if ((Status == EFI_UNSUPPORTED) || (!(SecurityStatus & 4))){
        // Either the device doesn't support Security Mode OR   Device is not locked
        Status = pBS->InstallMultipleProtocolInterfaces (
                    &(SataDevInterface->IdeDeviceHandle),
                    &gEfiDiskInfoProtocolGuid,
                    (EFI_DISK_INFO_PROTOCOL *)(SataDevInterface->SataDiskInfo),
                    NULL);

    #if HDD_PASSWORD_SUPPORT_UNDER_RAIDMODE
        if (!(!(AhciBusInterface->AHCIRAIDMODE) && (SataDevInterface->DeviceType == ATA))) {
    #endif

        // If it's Raid mode and AHCIBUS handles the ATAPI devices, install the BlockIo
        // for the ATAPI devices. BlockIo Will be installed for all the ATA and ATAPI device under AHCI mode
        if (!(!(AhciBusInterface->AHCIRAIDMODE) && (AhciPlatformPolicy->AhciBusAtapiSupport == FALSE ))) {

            // Either the device doesn't support Security Mode OR Device is not locked
            Status = pBS->InstallMultipleProtocolInterfaces (
                        &(SataDevInterface->IdeDeviceHandle),
                        &gEfiBlockIoProtocolGuid,
                        (EFI_BLOCK_IO_PROTOCOL *)(SataDevInterface->SataBlkIo),
                        NULL);
            ASSERT_EFI_ERROR(Status);
        }

    #if HDD_PASSWORD_SUPPORT_UNDER_RAIDMODE
        }
    #endif

     }                                                  // Install BLOCKIO

    TRACE_AHCI((-1,"AHCI: SATA Device type %x detected at Port Number : %x, PM Port Number : %x\n",
            SataDevInterface->DeviceType, SataDevInterface->PortNumber, SataDevInterface->PMPortNumber));

    return EFI_SUCCESS;
}

/**
    Install other features like HDD Security, Opal Security, HDD SMART 
    etc. on Device.

    @param    This 
    @param    AMI_AHCI_BUS_PROTOCOL             *AhciBusInterface,

    @retval   EFI_STATUS

**/

EFI_STATUS
InstallOtherOptionalFeatures(
    IN AMI_AHCI_BUS_PROTOCOL          *AhciBusInterface
)
{
    EFI_STATUS                 Status;
    DLINK                      *dlink;
    SATA_DEVICE_INTERFACE      *SataDevInterface = NULL;

    TempHddSecurityProtocolPtr = NULL;  // Reset global variable
    
    if ( HddSecurityInitProtocol == NULL) {
        pBS->LocateProtocol (&gAmiHddSecurityInitProtocolGuid,
                             NULL,
                             (VOID **) &HddSecurityInitProtocol);
    }
    
    if(OpalSecInitProtocol == NULL) {
        pBS->LocateProtocol (&gAmiHddOpalSecInitProtocolGuid,
                             NULL,
                             (VOID **) &OpalSecInitProtocol);
    }
    
    if ( HddSmartInitProtocol == NULL ) {
        pBS->LocateProtocol (&gAmiHddSmartInitProtocolGuid,
                             NULL,
                             (VOID **)&HddSmartInitProtocol);
    }

#if defined(ACOUSTIC_MANAGEMENT_DRIVER_SUPPORT) && (ACOUSTIC_MANAGEMENT_DRIVER_SUPPORT != 0)
    if ( gHddAcousticInitProtocol == NULL ) {
	 pBS->LocateProtocol (&gHddAcousticInitProtocolGuid,
                                   NULL,
                                   (VOID **) &gHddAcousticInitProtocol);
    }
#endif
    
    for (dlink = AhciBusInterface->SataDeviceList.pHead; dlink; dlink = dlink->pNext) {

        SataDevInterface = OUTTER(dlink, SataDeviceLink, SATA_DEVICE_INTERFACE);

        if(SataDevInterface->DeviceState != DEVICE_CONFIGURED_SUCCESSFULLY || SataDevInterface->IsDeviceFeatureDone ) {
            continue;
        }

        SataDevInterface->IsDeviceFeatureDone = TRUE;
        Status = EFI_UNSUPPORTED;
        
        if(OpalSecInitProtocol != NULL) {
           OpalSecInitProtocol->InstallOpalSecurityInterface(SataDevInterface, AmiStorageInterfaceAhci);
        }

        if(HddSecurityInitProtocol != NULL) {
            Status = HddSecurityInitProtocol->InstallSecurityInterface(SataDevInterface, AmiStorageInterfaceAhci);
            if(TempHddSecurityProtocolPtr == NULL) {
                 // Verify that Security interface has been installed
                 // on at least one device
                pBS->HandleProtocol(SataDevInterface->IdeDeviceHandle, 
                                     &gAmiHddSecurityProtocolGuid, 
                                     (VOID**)&TempHddSecurityProtocolPtr
                                     );
            }
        }
         
        if(EFI_ERROR(Status)) {
            // Install TcgStorageSecurity protocol for the device that supports TCG security Protocol
            if(TcgStorageSecurityInitProtocol == NULL) {
                pBS->LocateProtocol( &gAmiTcgStorageSecurityInitProtocolGuid,
                                                  NULL,
                                                  (VOID **) &TcgStorageSecurityInitProtocol);
            }

            if(TcgStorageSecurityInitProtocol != NULL) {
                Status = TcgStorageSecurityInitProtocol->InstallSecurityInterface(SataDevInterface, AmiStorageInterfaceAhci );
            }
        }
        
        // If NO Security protocol installed then send freeze lock command 
        if(EFI_ERROR(Status)) {
            if(AhciReadyToBootEvent == NULL ) {
                CreateReadyToBootEvent(
                                       TPL_CALLBACK,
                                       AhciFreezeLockDevice,
                                       NULL,
                                       &AhciReadyToBootEvent
                                       );
            }
        }

#if defined(ACOUSTIC_MANAGEMENT_DRIVER_SUPPORT) && (ACOUSTIC_MANAGEMENT_DRIVER_SUPPORT != 0)
         if(gHddAcousticInitProtocol != NULL) {
              gHddAcousticInitProtocol->InitHddAcoustic(SataDevInterface, AmiStorageInterfaceAhci);
         }
#endif

        if(HddSmartInitProtocol != NULL) {
            HddSmartInitProtocol->InitSmartSupport(SataDevInterface, AmiStorageInterfaceAhci);
            // Update the Identify Data.
            GetIdentifyData(SataDevInterface);
            if(HddSmartInitProtocol->SmartDiagonasticFlag) {
                HddSmartInitProtocol->InstallSmartInterface(SataDevInterface, AmiStorageInterfaceAhci);
            }
        }

    } // end of for loop

    return EFI_SUCCESS;

}
/** 
 * Checks for the presence of the Locked Drives Under this Controller.
 * If No drive is Locked, installs gHddPasswordVerifiedGuid
 * 
 * @param    ControllerHandle 
   @param    *AhciBusInterface
 * 
 * @retval   BOOLEAN
 *       
 * @notes    TRUE  - If drives present under this controller are locked.
 *                   else returns FALSE.
 *           
 */
BOOLEAN
CheckForLockedDrives(
    IN  AMI_AHCI_BUS_PROTOCOL         *AhciBusInterface
)
{
    DLINK                      *dlink;
    SATA_DEVICE_INTERFACE      *SataDevInterface = NULL;
    UINT16                     SecurityStatus;
    BOOLEAN                    DevLocked = FALSE;
    UINT16                     SecurityLockedMask = 0x04;
    
    for (dlink = AhciBusInterface->SataDeviceList.pHead; dlink; dlink = dlink->pNext) {
        SataDevInterface = OUTTER(dlink, SataDeviceLink, SATA_DEVICE_INTERFACE);
        //Check whether Security Feature is supported for this SATA Device
        if(!(SataDevInterface->IdentifyData.Command_Set_Supported_82 & 0x2)) {
            continue;
        }
        SecurityStatus = SataDevInterface->IdentifyData.Security_Status_128;
		// Check whether the Drive is Password Enabled or not.
        DevLocked = (BOOLEAN)((SecurityStatus & SecurityLockedMask) ? TRUE : FALSE );
        if(DevLocked) {
            return TRUE;
        }
    }
    return FALSE;
}


/**
    Uninstall all devices installed in start procedure.

    @param    This 
    @param    Controller 
    @param    NumberOfChildren 
    @param    ChildHandleBuffer 

    @retval   EFI_STATUS

    @note  
 1. Check whether "gAmiAhciBusProtocolGuid" is installed on this 
     controller. If not exit with error.
 2. If "NumberOfChildren" is zero, check whether all child devices 
    have been stopped. If not exit with error.If all child devices 
    have been stopped, then close "gAmiAhciBusProtocolGuid" and 
    "gEfiIdeControllerInitProtocolGuid",uninstall 
    "gEfiIdeControllerInitProtocolGuid" and then exit with success.
 3. If "NumberOfChildren" is non-zero,  close 
    "gEfiIdeControllerInitProtocolGuid" opened by the child device 
    in start function.Uninstall all protocols installed on this 
    child device in start function,free up all resources allocated 
    in start function. Repeat step 3 for all child devices and
    return success at the end.

**/

EFI_STATUS 
EFIAPI 
AhciBusStop (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     Controller,
    IN UINTN                          NumberOfChildren,
    IN EFI_HANDLE                     *ChildHandleBuffer
 )
{

    AMI_AHCI_BUS_PROTOCOL       *AhciBusInterface;
    SATA_DEVICE_INTERFACE       *SataDeviceInterface;
    EFI_STATUS                          Status;
    EFI_IDE_CONTROLLER_INIT_PROTOCOL    *IdeControllerInterface;
    UINT16                              Index = 0;
    UINT16                              Port=0;
    UINT16                              PMPort=0;
    EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
    DLINK                               *dlink;

    //
    // Check if AMI_AHCI_BUS_PROTOCOL is installed on the Controller.
    //
    Status = pBS->OpenProtocol( Controller,
                                &gAmiAhciBusProtocolGuid,
                                (VOID **)&AhciBusInterface,
                                This->DriverBindingHandle,
                                Controller,
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(Status)) { 
        return EFI_DEVICE_ERROR;
    }

    // Check if ChildHandleBuffer is valid
    if (NumberOfChildren) {
        while (NumberOfChildren) {

            Status = pBS->CloseProtocol ( Controller,
                                &gEfiIdeControllerInitProtocolGuid,
                                This->DriverBindingHandle,
                                ChildHandleBuffer[Index]);
            ASSERT_EFI_ERROR(Status);

            Status = pBS->OpenProtocol(ChildHandleBuffer[Index],
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **)&DevicePath,
                                   This->DriverBindingHandle,     
                                   Controller,
                                   EFI_OPEN_PROTOCOL_GET_PROTOCOL);

            // Lookout for SATA device path ACPI_DEVICE path, PCI 
            // Device path and then ATAPI device path will be the sequence
            do {
                if ((DevicePath->Type == MESSAGING_DEVICE_PATH) && (DevicePath->SubType == MSG_USB_SATA_DP)) {
                    Port = ((SATA_DEVICE_PATH *)DevicePath)->HBAPortNumber;
                    PMPort = ((SATA_DEVICE_PATH *)DevicePath)->PortMultiplierPortNumber;
                    break;
                }
                else {
                    DevicePath = NEXT_NODE(DevicePath);
                }
            } while (DevicePath->Type != END_DEVICE_PATH);

            if(DevicePath->Type == END_DEVICE_PATH) {
                //Unable to find the Messaging device path node.
                ASSERT(FALSE);  
                return EFI_DEVICE_ERROR;
            }

            SataDeviceInterface = GetSataDevInterface(AhciBusInterface, (UINT8)Port, (UINT8)PMPort);
            if (!SataDeviceInterface) return EFI_DEVICE_ERROR;

            // Before un-installing DiskInfo check whether it is installed or not
            Status = pBS->OpenProtocol( ChildHandleBuffer[Index],
                                &gEfiDiskInfoProtocolGuid,
                                NULL,
                                This->DriverBindingHandle,     
                                ChildHandleBuffer[Index],   
                                EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

            if (Status == EFI_SUCCESS) {
                Status = pBS->UninstallMultipleProtocolInterfaces (
                                ChildHandleBuffer[Index],
                                &gEfiDiskInfoProtocolGuid, 
                                (EFI_DISK_INFO_PROTOCOL *)(SataDeviceInterface->SataDiskInfo),
                                NULL);
            }

            // Before un-installing BLOCKIO check whether it is installed or not
            Status = pBS->OpenProtocol( ChildHandleBuffer[Index],
                                        &gEfiBlockIoProtocolGuid,
                                        NULL,
                                        This->DriverBindingHandle,     
                                        ChildHandleBuffer[Index],   
                                        EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

            if (Status == EFI_SUCCESS) {
                Status = pBS->UninstallMultipleProtocolInterfaces (
                                ChildHandleBuffer[Index],
                                &gEfiBlockIoProtocolGuid,
                                (EFI_BLOCK_IO_PROTOCOL *)(SataDeviceInterface->SataBlkIo),
                                NULL);
            }

            Status = pBS->UninstallMultipleProtocolInterfaces (
                            ChildHandleBuffer[Index],
                            &gEfiDevicePathProtocolGuid,
                            SataDeviceInterface->DevicePathProtocol,
                            NULL);

            if (EFI_ERROR(Status)) {
                return EFI_DEVICE_ERROR;
            }else {
                // Now free up all resources allocated.
                if (SataDeviceInterface->AtapiDevice != NULL){
                    pBS->FreePool(SataDeviceInterface->AtapiDevice->PacketBuffer);
                    pBS->FreePool(SataDeviceInterface->AtapiDevice->InquiryData);
                    pBS->FreePool(SataDeviceInterface->AtapiDevice);
                }
                // Free up resources allocated for component names
                if (SataDeviceInterface->UDeviceName != NULL) {
                    pBS->FreePool(SataDeviceInterface->UDeviceName->Language);
                    pBS->FreePool(SataDeviceInterface->UDeviceName->UnicodeString);
                    pBS->FreePool(SataDeviceInterface->UDeviceName);
                }
                
                // Before un-installing HDD security check whether it is installed or not
                Status = pBS->OpenProtocol( ChildHandleBuffer[Index],
                            &gAmiHddSecurityProtocolGuid,
                            NULL,
                            This->DriverBindingHandle,     
                            ChildHandleBuffer[Index], 
                            EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

                if (Status == EFI_SUCCESS) {
                        if(HddSecurityInitProtocol != NULL) {
                            HddSecurityInitProtocol->StopSecurityModeSupport(SataDeviceInterface, AmiStorageInterfaceAhci);
                        }
                }
                
                // Before un-installing TCGStoragesecurity check whether it is installed or not
                Status = pBS->OpenProtocol( ChildHandleBuffer[Index],
                                            &gAmiTcgStorageSecurityProtocolGuid,
                                            NULL,
                                            This->DriverBindingHandle,
                                            ChildHandleBuffer[Index],
                                            EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

                if ( Status == EFI_SUCCESS ) {
                     if(TcgStorageSecurityInitProtocol != NULL) {
                    	 TcgStorageSecurityInitProtocol->StopSecurityModeSupport(SataDeviceInterface, AmiStorageInterfaceAhci);
                     }
                }
                
                // Before un-installing Hdd Smart check whether it is installed or not
                Status = pBS->OpenProtocol( ChildHandleBuffer[Index],
                            &gAmiHddSmartProtocolGuid,
                            NULL,
                            This->DriverBindingHandle, 
                            ChildHandleBuffer[Index],
                            EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

                if (Status == EFI_SUCCESS) {
                    if(HddSmartInitProtocol != NULL) {
                        if(HddSmartInitProtocol->SmartDiagonasticFlag) {
                            HddSmartInitProtocol->UnInstallSmartInterface(SataDeviceInterface, AmiStorageInterfaceAhci);
                        }
                    }
                }
                // Before un-installing OPAL security interface check whether it is installed or not.
                Status = pBS->OpenProtocol( ChildHandleBuffer[Index],
                            &gEfiStorageSecurityCommandProtocolGuid,
                            NULL,
                            This->DriverBindingHandle,
                            ChildHandleBuffer[Index], 
                            EFI_OPEN_PROTOCOL_TEST_PROTOCOL);

                if (Status == EFI_SUCCESS) {
                        if(OpalSecInitProtocol != NULL) {
                            OpalSecInitProtocol->UnInstallOpalSecurityInterface(SataDeviceInterface, AmiStorageInterfaceAhci);
                        }
                }

                pBS->FreePool(SataDeviceInterface->SataBlkIo->BlkIo.Media);
                pBS->FreePool(SataDeviceInterface->SataBlkIo);
                pBS->FreePool(SataDeviceInterface->SataDiskInfo);
                pBS->FreePool (SataDeviceInterface->DevicePathProtocol);
                DListDelete(&(AhciBusInterface->SataDeviceList), &(SataDeviceInterface->SataDeviceLink));
                pBS->FreePool (SataDeviceInterface);
            }
            NumberOfChildren--;
            Index++;
        }
    } else {

        // Check if AMI_AHCI_BUS_PROTOCOL can be removed. No device other 
        // than Port Multiplier can be present.
        dlink = AhciBusInterface->SataDeviceList.pHead; 
        Status = EFI_SUCCESS;
        if (dlink){ 
            do {
                SataDeviceInterface = OUTTER(dlink, SataDeviceLink, SATA_DEVICE_INTERFACE);
                if (SataDeviceInterface->DeviceType != PMPORT ||
                    SataDeviceInterface->DeviceState == DEVICE_CONFIGURED_SUCCESSFULLY) {
                    Status = EFI_DEVICE_ERROR;
                    break;                
                }
                dlink = dlink-> pNext;
            }while (dlink);    
        }

        if (EFI_ERROR(Status)) { 
            return Status;
        }
        // Free PM resources
        dlink = AhciBusInterface->SataDeviceList.pHead; 
        Status = EFI_SUCCESS;
        if (dlink){ 
            do {
                SataDeviceInterface = OUTTER(dlink, SataDeviceLink, SATA_DEVICE_INTERFACE);
                // Now free up all resources allocated.
                if (SataDeviceInterface->AtapiDevice != NULL){
                    pBS->FreePool(SataDeviceInterface->AtapiDevice->PacketBuffer);
                    pBS->FreePool(SataDeviceInterface->AtapiDevice->InquiryData);
                    pBS->FreePool(SataDeviceInterface->AtapiDevice);
                }
                // Free up resources allocated for component names
                if (SataDeviceInterface->UDeviceName != NULL) {
                    pBS->FreePool(SataDeviceInterface->UDeviceName->Language);
                    pBS->FreePool(SataDeviceInterface->UDeviceName->UnicodeString);
                    pBS->FreePool(SataDeviceInterface->UDeviceName);
                }
                DListDelete(&(AhciBusInterface->SataDeviceList), &(SataDeviceInterface->SataDeviceLink));
                dlink = dlink-> pNext;
                pBS->FreePool (SataDeviceInterface);
            }while (dlink);    
        }    
            // Close all the protocols opened in Start Function
            Status = pBS->CloseProtocol ( Controller,
                                &gEfiIdeControllerInitProtocolGuid,
                                This->DriverBindingHandle,
                                Controller);
            ASSERT_EFI_ERROR(Status);
            // AtaPass Thru uninstall
            Status = pBS->LocateProtocol (
                                &gAmiAtaPassThruInitProtocolGuid,
                                NULL,
                                (VOID **) &AtaPassThruInitProtocol
                        );

             if(!EFI_ERROR(Status)) {
                 if(AtaPassThruInitProtocol != NULL) {
                     AtaPassThruInitProtocol->StopAtaPassThruSupport(Controller, TRUE);
                 }
             }
            // SCSIPassThruAtapi uninstall
            Status = pBS->LocateProtocol (&gAmiScsiPassThruInitProtocolGuid,
                                          NULL,
                                          (VOID **)&gScsiPassThruInitProtocol);

             if(!EFI_ERROR(Status)) {
                 if(gScsiPassThruInitProtocol != NULL) {
                     gScsiPassThruInitProtocol->StopScsiPassThruAtapiSupport(Controller, TRUE);
                 }
             }

            Status = pBS->CloseProtocol(Controller,
                                        &gAmiAhciBusProtocolGuid,
                                        This->DriverBindingHandle,
                                        Controller);
            ASSERT_EFI_ERROR(Status);

            Status = pBS->UninstallProtocolInterface (Controller,
                                                      &gAmiAhciBusProtocolGuid,
                                                      AhciBusInterface);

            if (EFI_ERROR(Status)) {

                Status = pBS->OpenProtocol( Controller,
                                            &gAmiAhciBusProtocolGuid,
                                            (VOID **)&AhciBusInterface,
                                            This->DriverBindingHandle,
                                            Controller,
                                            EFI_OPEN_PROTOCOL_BY_DRIVER);
                ASSERT_EFI_ERROR(Status);

                Status = pBS->OpenProtocol( Controller,
                                            &gEfiIdeControllerInitProtocolGuid,
                                            (VOID **)&IdeControllerInterface,
                                            This->DriverBindingHandle,     
                                            Controller,   
                                            EFI_OPEN_PROTOCOL_BY_DRIVER );
                ASSERT_EFI_ERROR(Status);

                return EFI_DEVICE_ERROR;
            }

            // Free the Pages allocated for the FIS and Command List
            if (AhciBusInterface->PortFISBaseAddress) {
                pBS->FreePages(AhciBusInterface->PortFISBaseAddress,
                               EFI_SIZE_TO_PAGES(AhciBusInterface->NumberofPortsImplemented * RECEIVED_FIS_SIZE + 0x100 ));
            }

            if (AhciBusInterface->PortCommandListBaseAddress) {
                pBS->FreePages((AhciBusInterface->PortCommandListBaseAddress),EFI_SIZE_TO_PAGES(COMMAND_LIST_SIZE_PORT * 2));
            }
    }
    return EFI_SUCCESS;
}


/**
    Installs BUS Init Protocol on the IDE controller Handle

        
    @param    Controller 
    @param    IDE_BUS_INIT_PROTOCOL           *IdeBusInitInterface,
    @param    IDE_CONTROLLER_PROTOCOL         *IdeControllerInterface,
    @param    EFI_PCI_IO_PROTOCOL             *PciIO

    @retval   EFI_STATUS

    @note  
   1. Call AhciInitController
   2. Install gAmiAhciBusProtocolGuid protocol

**/

EFI_STATUS
InstallAhciBusProtocol (
    IN EFI_HANDLE                          Controller,
    IN OUT AMI_AHCI_BUS_PROTOCOL           *AhciBusInterface,
    IN EFI_IDE_CONTROLLER_INIT_PROTOCOL    *IdeControllerInterface,
    IN EFI_PCI_IO_PROTOCOL                 *PciIO
)
{
    EFI_STATUS                  Status;
    UINT8                       PciConfig[16];
#if SBIDE_SUPPORT
    IDE_SETUP_PROTOCOL        *gIdeSetupProtocol;
#endif

    // Initialize the default Values
    //
    ZeroMemory (AhciBusInterface, sizeof(AMI_AHCI_BUS_PROTOCOL));

    AhciBusInterface->ControllerHandle = Controller;
    AhciBusInterface->IdeControllerInterface = IdeControllerInterface;
    AhciBusInterface->PciIO = PciIO;
    DListInit(&(AhciBusInterface->SataDeviceList));

    AhciBusInterface->SataReadWritePio          = SataReadWritePio;
    AhciBusInterface->SataPioDataOut            = SataPioDataOut;
    AhciBusInterface->ExecutePioDataCommand     = ExecutePioDataCommand;
    AhciBusInterface->ExecuteNonDataCommand     = ExecuteNonDataCommand;
    AhciBusInterface->ExecuteDmaDataCommand     = ExecuteDmaDataCommand;
    AhciBusInterface->WaitforCommandComplete    = WaitforCommandComplete;
    AhciBusInterface->GeneratePortReset         = GeneratePortReset;
    AhciBusInterface->GeneratePortSoftReset     = GenerateSoftReset;
    AhciBusInterface->ExecutePacketCommand      = ExecutePacketCommand;
    AhciBusInterface->AHCIRAIDMODE    = TRUE;       // Set TRUE when in AHCI mode

    Status = PciIO->Pci.Read ( PciIO,
                                EfiPciIoWidthUint8,
                                0,
                                sizeof (PciConfig),
                                PciConfig
                                );

    if (PciConfig [IDE_SUB_CLASS_CODE]== SCC_RAID_CONTROLLER ){
        AhciBusInterface->AHCIRAIDMODE    = FALSE;
    }

    // Using setup question if needed, set Bit 0 to enable/Disable
    // Acoustic Power Management.
    // Set bit1 only if HDD Losses power in S3 state. HDD freeze lock 
    // command will be issued during S3 resume when this bit is set 
    // and also if Password is enabled for HDD, it will be unlocked 
    // during S3 resume.

    AhciBusInterface->Acoustic_Enable = 0; // ACOUSTIC_SUPPORT_DISABLE

#if SBIDE_SUPPORT
    Status = pBS->LocateProtocol(&gIdeSetupProtocolguid, NULL, (VOID**)&gIdeSetupProtocol);
    ASSERT_EFI_ERROR(Status);
    #if ACOUSTIC_MANAGEMENT_SUPPORT
        if (!EFI_ERROR(Status)) {
            AhciBusInterface->Acoustic_Enable = gIdeSetupProtocol->AcousticPwrMgmt;
            AhciBusInterface->Acoustic_Management_Level   = gIdeSetupProtocol->AcousticLevel;
        } else {
            AhciBusInterface->Acoustic_Enable = ACOUSTIC_SUPPORT_DISABLE;
            AhciBusInterface->Acoustic_Management_Level   = ACOUSTIC_LEVEL_BYPASS;
        }       
    #endif //End of ACOUSTIC_MANAGEMENT_SUPPORT
#endif // end of if SBIDE_SUPPORT


    // Initialize PrevPortNum and PrevPortMultiplierPortNum for AtaPassThru to 0xFFFF
    AhciBusInterface->PrevPortNum = 0xffff;
    AhciBusInterface->PrevPortMultiplierPortNum = 0xffff;

    // Init AHCI Controller
    Status = AhciInitController(AhciBusInterface);
    if (EFI_ERROR(Status)) return Status;

    Status = pBS->InstallProtocolInterface(
                        &Controller,
                        &gAmiAhciBusProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        AhciBusInterface);
    return Status;
}

/**
    Allocates PCIIO system Memory for Received FIS, Command List Base address
    and Command Table using PCI IO protocol

    @param    AHCI_BUS_PROTOCOL     *AhciBusInterface

    @retval   EFI_STATUS

**/

EFI_STATUS
AllocateMemoryForCommandListAndFis( 
  IN OUT AMI_AHCI_BUS_PROTOCOL     *AhciBusInterface 
  )
{
    EFI_STATUS      Status;
    UINTN           ReceiveFisSize;
    UINTN           CommandListSize;
    UINT64          FisReceiveBaseAddr = 0;
    UINT64          CommandListBaseAddr = 0;
    UINT64          FisReceiveMappedBaseAddr = 0;
    UINT64          CommandListMappedBaseAddr = 0;
    void            *FisMapedAddress = NULL;
    void            *CommandListMapedAddress = NULL;

    EFI_PCI_IO_PROTOCOL *PciIo = AhciBusInterface->PciIO;

    // Calculate Received FIS size for the implemented ports
    ReceiveFisSize = AhciBusInterface->NumberofPortsImplemented * RECEIVED_FIS_SIZE + 0x100;

    // Map PCIIO system memory for the FIS structure
    Status = PciIo->AllocateBuffer ( PciIo,
                                     AllocateAnyPages,
                                     EfiBootServicesData,
                                     EFI_SIZE_TO_PAGES (ReceiveFisSize), 
                                     (VOID*)&FisReceiveBaseAddr,
                                     0 );

    if (EFI_ERROR (Status)) {
        return Status;
    }

    ZeroMemory ((VOID *) FisReceiveBaseAddr, ReceiveFisSize);
    
    CommandListSize = COMMAND_LIST_SIZE_PORT * 2;
    Status = PciIo->AllocateBuffer ( PciIo,
                                     AllocateAnyPages,
                                     EfiBootServicesData,
                                     EFI_SIZE_TO_PAGES ((UINTN) CommandListSize),
                                     (VOID*)&CommandListBaseAddr,
                                     0 );

    if (EFI_ERROR (Status)) {
        goto CommandListMemoryAllocationError;
    }

    ZeroMemory ((VOID *)CommandListBaseAddr, (UINTN)CommandListSize);

    // Will be used to free the memory later
    AhciBusInterface->PortFISBaseAddress = FisReceiveBaseAddr;
    AhciBusInterface->PortCommandListBaseAddress  = CommandListBaseAddr;

    
    if(AhciPlatformPolicy->PciMapAddressForDataTransfer) {

        // Map the System Memory for the PCIIO allocated Buffer 
        Status = PciIo->Map ( PciIo,
                              EfiPciIoOperationBusMasterCommonBuffer,
                              (VOID *)FisReceiveBaseAddr,
                              &ReceiveFisSize,
                              (EFI_PHYSICAL_ADDRESS *)&FisReceiveMappedBaseAddr,
                              &FisMapedAddress
                              );

        if (EFI_ERROR (Status) || 
            ( ReceiveFisSize != AhciBusInterface->NumberofPortsImplemented * RECEIVED_FIS_SIZE + 0x100)) {
            goto FisMapError;
        }

        Status = PciIo->Map ( PciIo,
                              EfiPciIoOperationBusMasterCommonBuffer,
                              (VOID *)CommandListBaseAddr,
                              &CommandListSize,
                              (EFI_PHYSICAL_ADDRESS *)&CommandListMappedBaseAddr,
                              &CommandListMapedAddress
                              );
                
        if (EFI_ERROR (Status) || (CommandListSize != COMMAND_LIST_SIZE_PORT * 2)) {
            goto CommandListMapError;
        }
        
    } else {
        FisReceiveMappedBaseAddr=FisReceiveBaseAddr;
        CommandListMappedBaseAddr = CommandListBaseAddr;
    }

    AhciBusInterface->PortFISBaseAddr    = FisReceiveMappedBaseAddr;
    
    AhciBusInterface->PortFISBaseAddrEnd = AhciBusInterface->PortFISBaseAddr + ReceiveFisSize;
    AhciBusInterface->PortFISBaseAddr    = (AhciBusInterface->PortFISBaseAddr & (~0xFF))+ 0x100; 

    // Allocate memory for Command List (1KB aligned) and Command Table (128KB aligned).
    // All the ports in the controller will share Command List and Command table data Area.
    
    AhciBusInterface->PortCommandListBaseAddr  = CommandListMappedBaseAddr;
    AhciBusInterface->PortCommandListBaseAddr  = (AhciBusInterface->PortCommandListBaseAddr & (~0x3FF)) + 0x400;
    AhciBusInterface->PortCommandListLength    = 0x20;

    AhciBusInterface->PortCommandTableBaseAddr = AhciBusInterface->PortCommandListBaseAddr + 0x80;
    AhciBusInterface->PortCommandTableLength   = COMMAND_LIST_SIZE_PORT - 0x80;

    return EFI_SUCCESS;

CommandListMapError:

    // Unmap the mapped memory for the Received FIS
    PciIo->Unmap ( PciIo,
                   FisMapedAddress
                   );

FisMapError:

    // Buffer Allocation Error. Free the memory and Unmap it.
    // Free the Buffer allocated for Command List and Command table
    PciIo->FreeBuffer ( PciIo,
                        EFI_SIZE_TO_PAGES ((UINTN) CommandListSize),
                        (void *)CommandListBaseAddr
                        );

CommandListMemoryAllocationError:

    // Free the Buffer allocated for Received FIS
    PciIo->FreeBuffer ( PciIo,
                        EFI_SIZE_TO_PAGES ((UINTN) ReceiveFisSize),
                        (void *)FisReceiveBaseAddr
                        );

    return EFI_OUT_OF_RESOURCES;
}

/**
    Initializes AHCI Controller

    @param    AhciBusInterface 

    @retval   EFI_STATUS

    @note  
          1. Update internal Data area about the AHCI controller Capabilities.
          2. Allocate memory for FIS and CommandList
          3. Enable AHCI mode
          3. Disable all the ports

**/

EFI_STATUS
AhciInitController (
    IN OUT AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface
)
{
    EFI_STATUS    Status;
    UINT32        PortsImplemented;
    UINT8         i, PortNumber;
    UINT32        Data32;
    UINT64          Supports=0;
    ASLR_QWORD_ASD  *Resources=NULL;

    // Make sure AHCI Base address is programmed Properly
    Status = AhciBusInterface->PciIO->GetBarAttributes (AhciBusInterface->PciIO,
                                        PCI_ABAR_INDEX,
                                        &Supports,
                                        &Resources
                                        );

    if (EFI_ERROR(Status)) { return Status;}

    AhciBusInterface->AhciBaseAddress = Resources->_MIN;

    if (!AhciBusInterface->AhciBaseAddress) return EFI_DEVICE_ERROR;   

#if INDEX_DATA_PORT_ACCESS
    Status = InitilizeIndexDataPortAddress (AhciBusInterface->PciIO);
#endif

    // Get AHCI Capability
    AhciBusInterface->HBACapability = HBA_REG32(AhciBusInterface, HBA_CAP);
    if (AhciBusInterface->HBACapability == 0xFFFFFFFF) return EFI_DEVICE_ERROR; // Not decoded properly

    // Get # of Ports Implemented (bit map)
    AhciBusInterface->HBAPortImplemented = HBA_REG32(AhciBusInterface, HBA_PI);
    if (!AhciBusInterface->HBAPortImplemented)  return EFI_SUCCESS;

    // Cross check whether # of Ports implemented is less or equal to
    // Max. # of ports supported by the silicon
    PortsImplemented = AhciBusInterface->HBAPortImplemented;
    AhciBusInterface->NumberofPortsImplemented = 0;
    for ( ;PortsImplemented; PortsImplemented >>= 1){ 
        if  (PortsImplemented & 1) AhciBusInterface->NumberofPortsImplemented++;
    }
    if (((AhciBusInterface->HBACapability & HBA_CAP_NP_MASK) + 1) < AhciBusInterface->NumberofPortsImplemented)
            { return EFI_DEVICE_ERROR; }

    //  Get the HBA version #
    AhciBusInterface->AhciVersion = HBA_REG32(AhciBusInterface, HBA_VS);    

    //  Set AE bit
    HBA_REG32_OR(AhciBusInterface, HBA_GHC, HBA_GHC_AE);

    // Allocate Memory for received FIS, Command List Base address and Command table
    Status = AllocateMemoryForCommandListAndFis(AhciBusInterface);
    if(EFI_ERROR(Status)) {
        return Status;
    }

    // Make sure controller is not running
    PortsImplemented = AhciBusInterface->HBAPortImplemented;
    PortNumber = 0;
    for (i=0; PortsImplemented; PortsImplemented>>=1, PortNumber++){
        if (PortsImplemented & 1) { 
            // Program PxCLB and PxFB
            HBA_PORT_WRITE_REG64 (AhciBusInterface,
                                  PortNumber,
                                  HBA_PORTS_CLB,
                                  AhciBusInterface->PortCommandListBaseAddr);

            HBA_PORT_WRITE_REG64 (AhciBusInterface,
                                  PortNumber,
                                  HBA_PORTS_FB,
                                  AhciBusInterface->PortFISBaseAddr +(i * RECEIVED_FIS_SIZE));

            // Clear Start
            HBA_PORT_REG32_AND(AhciBusInterface,
                               PortNumber,
                               HBA_PORTS_CMD,
                               ~(HBA_PORTS_CMD_ST));

            // Make sure CR is 0 with in 500msec
            Status = WaitForMemClear(AhciBusInterface,
                                     PortNumber,
                                     HBA_PORTS_CMD,
                                     HBA_PORTS_CMD_CR,
                                     HBA_CR_CLEAR_TIMEOUT);

            if (EFI_ERROR(Status)) { 
                // Get the Port Speed allowed and Interface Power Management Transitions Allowed
                // Pass the values for PortReset. 
                Data32 = HBA_PORT_REG32 (AhciBusInterface,
                                         PortNumber,
                                         HBA_PORTS_SCTL);
                Data32 &= 0xFF0;          

                Status = GeneratePortReset(AhciBusInterface,
                                           NULL,
                                           PortNumber,
                                           0xFF,
                                           (UINT8)((Data32 & 0xF0) >> 4),
                                           (UINT8)(Data32 >> 8));
            }

            if (EFI_ERROR(Status)) { 
                HostReset(AhciBusInterface);
            }

            // Clear FIS receive enable.
            HBA_PORT_REG32_AND (AhciBusInterface, PortNumber, 
                                HBA_PORTS_CMD, ~(HBA_PORTS_CMD_FRE));
            // Make sure FR is 0 with in 500msec
            Status = WaitForMemClear(AhciBusInterface, PortNumber, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_FR,
                                    HBA_FR_CLEAR_TIMEOUT);

            if (EFI_ERROR(Status)) { 
                continue;
            }

            HBA_PORT_REG32_OR (AhciBusInterface, PortNumber,
                                  HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); // Clear Status register

            i++;        
        }
    }
    return EFI_SUCCESS;
}

/**
    Check if the port is implemented in the AHCI Controller

    @param    AhciBusInterface 
    @param    Port 

    @retval  EFI_STATUS

    @note  
      1. Check Port Implemented register whether the PORT is 
         implemented in the Controller or not.

**/ 

EFI_STATUS
CheckPortImplemented (
    IN AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface, 
    IN UINT8                    Port
)
{

    if (AhciBusInterface->HBAPortImplemented & (1<< Port)) return EFI_SUCCESS;

    return EFI_NOT_FOUND;
}

/**
    Detects a SATA device connected to given Port and PMPort

    @param    AhciBusInterface 
    @param    IdeControllerInterface 
    @param    Port 
    @param    PMPort 

    @retval   EFI_STATUS

    @note  
          1. if CheckDevicePresence fails exit.
          2. If Controller supports PM, issue Soft reset
          3. Check the Device Signature.

**/ 

EFI_STATUS
AhciDetectDevice (
    IN AMI_AHCI_BUS_PROTOCOL               *AhciBusInterface, 
    IN EFI_IDE_CONTROLLER_INIT_PROTOCOL    *IdeControllerInterface, 
    IN UINT8                               Port, 
    IN UINT8                               PMPort
)
{
    EFI_STATUS              Status;
    SATA_DEVICE_INTERFACE   *SataDevInterface = NULL;

    PROGRESS_CODE(DXE_IDE_DETECT);

    IdeControllerInterface->NotifyPhase (IdeControllerInterface, EfiIdeBusBeforeDevicePresenceDetection, Port);

    SataDevInterface = GetSataDevInterface(AhciBusInterface, Port, PMPort);
    
    if (!SataDevInterface){

        // A device is present.
        Status = pBS->AllocatePool (EfiBootServicesData,
                    sizeof(SATA_DEVICE_INTERFACE),
                    (VOID**)&SataDevInterface);
        if (EFI_ERROR(Status)) return Status;

        ZeroMemory (SataDevInterface, sizeof(SATA_DEVICE_INTERFACE));

        SataDevInterface->PortNumber = Port;
        SataDevInterface->PMPortNumber = PMPort;
        SataDevInterface->AhciBusInterface = AhciBusInterface;
        SataDevInterface->DeviceState = DEVICE_IN_RESET_STATE;

        // Update Base addresses
        SataDevInterface->PortCommandListBaseAddr = HBA_PORT_REG64 (AhciBusInterface, Port, HBA_PORTS_CLB);
        SataDevInterface->PortFISBaseAddr = HBA_PORT_REG64 (AhciBusInterface, Port, HBA_PORTS_FB);

        // Add to the AhciBusInterface
        DListAdd(&(AhciBusInterface->SataDeviceList), &(SataDevInterface->SataDeviceLink));   
    }

    if (PMPort == 0xFF) {
        Status = CheckDevicePresence (SataDevInterface, IdeControllerInterface, Port, PMPort);
    }
    else {
        Status = CheckPMDevicePresence (SataDevInterface, IdeControllerInterface, Port, PMPort);
    }

    if (EFI_ERROR(Status)) {
        IdeControllerInterface->NotifyPhase (IdeControllerInterface, EfiIdeBusAfterDevicePresenceDetection, Port);
        if (SataDevInterface->DeviceState == DEVICE_IN_RESET_STATE) {
            DListDelete(&(AhciBusInterface->SataDeviceList), &(SataDevInterface->SataDeviceLink));
            pBS->FreePool(SataDevInterface);
        }
        return EFI_DEVICE_ERROR;
    }

    #if PORT_MULTIPLIER_SUPPORT
    // Check if PM support is present
    if (AhciBusInterface->HBACapability & HBA_CAP_SPM) {
        Status = GenerateSoftReset(SataDevInterface, PMPort == 0xFF ? CONTROL_PORT : PMPort);
        if (EFI_ERROR(Status)) {

            // We know link has been established, meaning device is 
            // present. Maybe we need delay before giving a Soft reset.
            Status = ReadytoAcceptCmd(SataDevInterface);
            if (!EFI_ERROR(Status)){
                pBS->Stall(3000000);        // 3sec delay
                Status = GenerateSoftReset(SataDevInterface, PMPort == 0xFF ? CONTROL_PORT : PMPort);
            }
        }

        if (EFI_ERROR(Status)) {
            IdeControllerInterface->NotifyPhase (IdeControllerInterface, EfiIdeBusAfterDevicePresenceDetection, Port);
            if (SataDevInterface->DeviceState == DEVICE_IN_RESET_STATE) {
                DListDelete(&(AhciBusInterface->SataDeviceList), &(SataDevInterface->SataDeviceLink));
                pBS->FreePool(SataDevInterface);
            }
            return EFI_DEVICE_ERROR;
        }
    }
    #endif

    if (!(SataDevInterface->PortCommandListBaseAddr) || !(SataDevInterface->PortFISBaseAddr)) {
        ASSERT_EFI_ERROR(EFI_DEVICE_ERROR);
    }

    // Save the Signature
    SataDevInterface->Signature = HBA_PORT_REG32(AhciBusInterface, Port, HBA_PORTS_SIG);
    switch (SataDevInterface->Signature) {
        case ATA_SIGNATURE_32:
            SataDevInterface->DeviceType = ATA;
            break;
        case ATAPI_SIGNATURE_32:
            SataDevInterface->DeviceType = ATAPI;
            break;
        case PMPORT_SIGNATURE:
            SataDevInterface->DeviceType = PMPORT;
            // 1 sec Delay needed for the next device to be discovered from PM.
            pBS->Stall(1000000);        
            break;
        default:
        Status =  EFI_DEVICE_ERROR;
    }

#if !HDD_PASSWORD_SUPPORT_UNDER_RAIDMODE
    if ((!AhciBusInterface->AHCIRAIDMODE) && (SataDevInterface->DeviceType == ATA)) {
        DListDelete(&(AhciBusInterface->SataDeviceList), &(SataDevInterface->SataDeviceLink));
        pBS->FreePool(SataDevInterface);
        Status =  EFI_DEVICE_ERROR;
    }
#endif

#if !SUPPORT_ATAPI_IN_RAID_MODE

    // If the Atapi devices are handled by Raid option rom, then 
    // don't configure the Atapi devices.
    if ((!AhciBusInterface->AHCIRAIDMODE) && (SataDevInterface->DeviceType == ATAPI)) {
        DListDelete(&(AhciBusInterface->SataDeviceList), &(SataDevInterface->SataDeviceLink));
        pBS->FreePool(SataDevInterface);
        Status =  EFI_DEVICE_ERROR;
    }
#endif
    IdeControllerInterface->NotifyPhase (IdeControllerInterface, EfiIdeBusAfterDevicePresenceDetection, Port);

    return Status;
}

/**
    Check if any device is connected to the port

    @param    SataDevInterface 
    @param    IdeControllerInterface 
    @param    Port 
    @param    PMPort 

    @retval   EFI_STATUS

    @note  
          1. If Staggered spin-up is supported, power-up the device.
          2. Call CheckValidDevice if success exit. Else generate soft reset.

**/ 

EFI_STATUS
CheckDevicePresence (
    IN SATA_DEVICE_INTERFACE                *SataDevInterface,
    IN EFI_IDE_CONTROLLER_INIT_PROTOCOL     *IdeControllerInterface, 
    IN UINT8                                Port, 
    IN UINT8                                PMPort
)
{
    EFI_STATUS                  Status;
    AMI_AHCI_BUS_PROTOCOL       *AhciBusInterface = SataDevInterface->AhciBusInterface; 
    UINT8                       CapIss;  
    UINT8                       PortSpeed=0;
    UINT8                       CurrentPortSpeed=0;

    // PM disabled
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_SCTL, HBA_PORTS_SCTL_IPM_PSD_SSD);       

    // Clear Status register
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); 
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR); 

    // Get the Interface Speed Support( Maximum Speed supported)
    CapIss = (UINT8)((HBA_REG32(AhciBusInterface, HBA_CAP) & HBA_CAP_ISS_MASK)>>20);    

    // Get the Speed Allowed (SPD) for the Port. Maximum speed allowed for the Port
    PortSpeed = (UINT8)((HBA_PORT_REG32(AhciBusInterface, Port, HBA_PORTS_SCTL) 
                            & HBA_PORTS_SCTL_SPD_MASK)>>4); 

    // If the Maximum speed allowed is programmed for the port, use the Port Speed allowed value 
    if(PortSpeed != 0 ) {
        if(PortSpeed > CapIss) {
            // Port Speed allowed can't be more than Interface Speed. So limit Port speed to Interface Speed 
            PortSpeed = CapIss;
        }
    } else {
        // If there is no Maximum speed allowed for the port, use the Interface Speed
        PortSpeed = CapIss;
    }   

    // Check if Link is already established
    if ((HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_SSTS) 
            & HBA_PORTS_SSTS_DET_MASK) == HBA_PORTS_SSTS_DET_PCE) {

        // As the Link is already established, get the negotiated interface
        // communication speed
        CurrentPortSpeed = (UINT8)((HBA_PORT_REG32(AhciBusInterface, Port, HBA_PORTS_SSTS) 
                                & HBA_PORTS_SSTS_SPD_MASK)>>4); 

        // Check the Current Interface Speed with Speed Allowed. If current interface speed is more than 
        // Speed allowed set, then set the port speed according to the speed allowed 
        if( CurrentPortSpeed > PortSpeed) {

            Status = GeneratePortReset(AhciBusInterface,
                                       SataDevInterface,
                                       Port,
                                       PMPort,
                                       PortSpeed,
                                       HBA_PORTS_SCTL_IPM_PSSD);
        }
    } else {
        // Link Not Established. Set SPD by PortSpeed 
        HBA_PORT_REG32_AND_OR (AhciBusInterface, Port, HBA_PORTS_SCTL, ~HBA_PORTS_SCTL_SPD_MASK,PortSpeed<<4 );
    }

    //  Check if Staggered Spin-up is supported
    if (HBA_REG32 (AhciBusInterface,  HBA_CAP) & HBA_CAP_SSS) {

        // Check if Link is already established, if yes dodn't expect a COMRESET
        if ((HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_SSTS) 
            & HBA_PORTS_SSTS_DET_MASK) != HBA_PORTS_SSTS_DET_PCE) {
            // Enable FIS Receive Enable
            HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_FRE); 

            // Wait till FIS is running
            WaitForMemSet(AhciBusInterface, Port, HBA_PORTS_CMD,
                                    HBA_PORTS_CMD_FR,
                                    HBA_PORTS_CMD_FR,
                                    HBA_FR_CLEAR_TIMEOUT);

            HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_SUD); // Spin up the device

            // Is delay necessary here after power up?
            Status = HandlePortComReset(AhciBusInterface, NULL, Port, 0xFF);

            //  Disable FIS Receive Enable
            HBA_PORT_REG32_AND (AhciBusInterface, Port, HBA_PORTS_CMD, ~HBA_PORTS_CMD_FRE);

            IdeControllerInterface->NotifyPhase (IdeControllerInterface, EfiIdeBusAfterDevicePresenceDetection, Port);
        } else {
            HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_SUD); // Spin up the device
        }
    }

    // Check if Device detected. And check if Cold Presence logic 
    // is enabled. If yes enable POD
    if (((HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_SSTS) & HBA_PORTS_SSTS_DET_MASK) == HBA_PORTS_SSTS_DET_PCE) && 
            (HBA_PORT_REG32(AhciBusInterface, Port, HBA_PORTS_CMD) & HBA_PORTS_CMD_CPD)) {
        HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_CMD, HBA_PORTS_CMD_POD);
    } 

    Status = CheckValidDevice(AhciBusInterface, Port, PMPort);

#if PORT_MULTIPLIER_SUPPORT
    // If PORT Multiplier support is enabled, SoftReset generated later will get the signature. 
    // No need for this additional Port Reset here
    if ((HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_SSTS) & HBA_PORTS_SSTS_DET_MASK) == HBA_PORTS_SSTS_DET_PCE) {
        return EFI_SUCCESS;
    }
#else
    if (EFI_ERROR(Status)) {
        UINT32    Data32;
        IdeControllerInterface->NotifyPhase (IdeControllerInterface,
                                             EfiIdeBeforeChannelReset,
                                             (UINT8)Port);

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

        IdeControllerInterface->NotifyPhase (IdeControllerInterface,
                                             EfiIdeAfterChannelReset,
                                             (UINT8)Port);
    }
#endif
    return Status;
}

/**
    Checks for the presence device behind a Port Multiplier.

    @param    SATA_DEVICE_INTERFACE               *SataDevInterface, 
    @param    EFI_IDE_CONTROLLER_INIT_PROTOCOL    *IdeControllerInterface, 
    @param    UINT8                               Port, 
    @param    UINT8                               PMPort

    @retval   EFI_STATUS

    @note  
  1. Check whether communication is established?
  2. If yes exit else issues Port Reset

**/ 

EFI_STATUS
CheckPMDevicePresence (
    IN SATA_DEVICE_INTERFACE               *SataDevInterface, 
    IN EFI_IDE_CONTROLLER_INIT_PROTOCOL    *IdeControllerInterface, 
    IN UINT8                               Port, 
    IN UINT8                               PMPort
)
{
    EFI_STATUS               Status = EFI_DEVICE_ERROR;
    AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface = SataDevInterface->AhciBusInterface; 
    UINT32                   Data32 = 0, Init_SStatus = 0;

    ReadWritePMPort (SataDevInterface, PMPort, PSCR_0_SSTATUS, &Init_SStatus, FALSE);

    SataDevInterface->SControl = HBA_PORTS_SCTL_IPM_PSD_SSD;
    if ((Init_SStatus & HBA_PORTS_SSTS_DET_MASK) == HBA_PORTS_SSTS_DET_PCE) { 
        Data32 = HBA_PORTS_SCTL_IPM_PSD_SSD;
        ReadWritePMPort (SataDevInterface, PMPort, PSCR_2_SCONTROL, &Data32, TRUE);
    } else {

        // Perform Port Reset to bring the communication back
        IdeControllerInterface->NotifyPhase (IdeControllerInterface,
                                             EfiIdeBeforeChannelReset,
                                             (UINT8)Port);

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
        
        IdeControllerInterface->NotifyPhase (IdeControllerInterface,
                                             EfiIdeAfterChannelReset,
                                             (UINT8)Port);

        // Giving a soft reset immediately after Port Reset doesn't help to detect the 
        // devices behind PM quickly. Add a delay here before soft reset is generated.
        // Add 1Sec delay
        pBS->Stall(1000000);  
    }

    // Clear Status register
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_SERR, HBA_PORTS_ERR_CLEAR); 
    HBA_PORT_REG32_OR (AhciBusInterface, Port, HBA_PORTS_IS, HBA_PORTS_IS_CLEAR); 

    Data32 = HBA_PORTS_ERR_CLEAR;
    ReadWritePMPort (SataDevInterface, PMPort, PSCR_1_SERROR, &Data32, TRUE);

    Data32 = 0;
    ReadWritePMPort (SataDevInterface, PMPort, PSCR_0_SSTATUS, &Data32, FALSE);

    if ((Data32 & HBA_PORTS_SSTS_DET_MASK) == HBA_PORTS_SSTS_DET_PCE) Status = EFI_SUCCESS;

    return Status;
}

/**
    Configure Sata Port settings
                   
    @param    SataDevInterface 

    @retval   EFI_SUCCESS

**/ 

EFI_STATUS
ConfigureSataPort (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{
    // Check for the ATAPI device
    if (SataDevInterface->DeviceType == ATAPI) {

        // Set Device is ATAPI Bit in CMD register
        HBA_PORT_REG32_OR (SataDevInterface->AhciBusInterface,
                           SataDevInterface->PortNumber,
                           HBA_PORTS_CMD,
                           HBA_PORTS_CMD_ATAPI);
    }

    // Check the AHCI platform policy protocol to set the 
    // Drive LED on ATAPI Enable (DLAE) bit
    if(AhciPlatformPolicy->DriverLedOnAtapiEnable) {

        // Set Drive LED on ATAPI Enable (DLAE) Bit in CMD register
        HBA_PORT_REG32_OR (SataDevInterface->AhciBusInterface,
                           SataDevInterface->PortNumber,
                           HBA_PORTS_CMD,
                           HBA_PORTS_CMD_DLAE);
    }
    return EFI_SUCCESS;
}

/**
    Configure Port Multiplier

    @param     SataDevInterface 

    @retval 

    @note  
      1. Read the number of Ports implemented in the Port Multiplier
      2. Update PM attached bit in the AHCI controller.

**/ 

EFI_STATUS
ConfigurePMPort (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{
    EFI_STATUS  Status;
    UINT32      Data;

    // Read the number of Ports preset in PM
    Status = ReadWritePMPort (SataDevInterface, CONTROL_PORT, GSCR_2, &Data, FALSE);
    SataDevInterface->NumPMPorts = (UINT8)Data;
    
    // Set PM Attached bit in CMD register
    HBA_PORT_REG32_OR (SataDevInterface->AhciBusInterface,
                       SataDevInterface->PortNumber,
                       HBA_PORTS_CMD,
                       HBA_PORTS_CMD_PMA);
    return Status;
}

/**
    Configure the SATA device

                   
    @param    SataDevInterface 
    @param    SupportedModes 
    @param    Modes collection supported by the device 

    @retval   EFI_STATUS

    @note  
      1. Get the Identify data command.
      2. From the IdeControllerInit protocol, get the DMA & PIO supported
      3. Issue Set feature command to set PIO, DMA and multiple mode
      4. Initialize Acoustic, SMART, Power Management features.

**/ 
EFI_STATUS
ConfigureDevice (
    IN SATA_DEVICE_INTERFACE          *SataDevInterface,
    IN OUT EFI_ATA_COLLECTIVE_MODE    **SupportedModes 
)
{

    EFI_STATUS                          Status;
    AMI_AHCI_BUS_PROTOCOL               *AhciBusInterface = SataDevInterface->AhciBusInterface; 
    EFI_IDE_CONTROLLER_INIT_PROTOCOL    *IdeControllerInterface = AhciBusInterface->IdeControllerInterface; 
    UINT8                               Port = SataDevInterface->PortNumber;
    UINT8                               PMPort = SataDevInterface->PMPortNumber;
    COMMAND_STRUCTURE                   CommandStructure;
    UINT8                               Data8;
    UINT32                              Data32;
    UINT8                               Index;
    UINT16                              DeviceName[41];
    CHAR8                               Language[] = "Eng";
    EFI_UNICODE_STRING_TABLE            *tempUnicodeTable;

    if (SataDevInterface->DeviceType == PMPORT) return EFI_SUCCESS; // This is a Port Multiplier

    Status = GetIdentifyData(SataDevInterface);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

#if !DISABLE_SOFT_SET_PREV
#if FORCE_HDD_PASSWORD_PROMPT
    if ((SataDevInterface->DeviceType == ATA) &&
        (SataDevInterface->IdentifyData.Reserved_76_79[0] != 0xFFFF )&&
        (SataDevInterface->IdentifyData.Reserved_76_79[2] & 0x0040)&&           // Software Preservation support
        (SataDevInterface->IdentifyData.Reserved_76_79[3] & 0x0040)){ // Software Preservation Enabled

        ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 

        CommandStructure.Features = DISABLE_SATA2_SOFTPREV;                     // Disable Software Preservation
        CommandStructure.SectorCount = 6;
        CommandStructure.Command = SET_FEATURE_COMMAND;
        ExecuteNonDataCommand (SataDevInterface, CommandStructure);

        // Get the Port Speed allowed and Interface Power Management Transitions Allowed
        // Pass the values for PortReset. 
        Data32 = HBA_PORT_REG32 (AhciBusInterface, Port, HBA_PORTS_SCTL);
        Data32 &= 0xFF0;          

        GeneratePortReset(AhciBusInterface,
                          SataDevInterface,
                          Port,
                          PMPort,
                          (UINT8)((Data32 & 0xF0) >> 4),
                          (UINT8)(Data32 >> 8));

        CommandStructure.Features = 0x10;                                       // Enable Software Preservation
        CommandStructure.SectorCount = 6;
        CommandStructure.Command = SET_FEATURE_COMMAND;
        ExecuteNonDataCommand (SataDevInterface, CommandStructure);
    }
#endif
#endif

    // Check if Device need spin-up
     if ((SataDevInterface->IdentifyData.General_Config_0 & 4) &&
           (SataDevInterface->IdentifyData.Special_Config_2 == SPIN_UP_REQUIRED1 ||
            SataDevInterface->IdentifyData.Special_Config_2 == SPIN_UP_REQUIRED2 )){

        ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
        CommandStructure.Features = SET_DEVICE_SPINUP;
        CommandStructure.Command = SET_FEATURE_COMMAND;
        Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);

        ASSERT_EFI_ERROR(Status);

        // Get the Identify Command once more
        Status = GetIdentifyData(SataDevInterface);
        if (EFI_ERROR(Status)){
            return EFI_DEVICE_ERROR;
        }
    }

    Status = IdeControllerInterface->SubmitData(IdeControllerInterface,
                                                Port, 
                                                PMPort == 0xFF ? 0 : PMPort, 
                                                (EFI_IDENTIFY_DATA *) &(SataDevInterface->IdentifyData));
    if (EFI_ERROR(Status))
        return Status;

    Status = IdeControllerInterface->CalculateMode(IdeControllerInterface, 
                                                   Port, 
                                                   PMPort == 0xFF ? 0 : PMPort, 
                                                   SupportedModes);

    if (EFI_ERROR(Status) || (*SupportedModes == NULL) )
        return EFI_INVALID_PARAMETER;

    // Check ExtMode
    if ((*SupportedModes)->ExtMode[0].TransferProtocol) {      // Not Auto speed
        Status = GeneratePortReset(AhciBusInterface, SataDevInterface, Port, PMPort,
                    (*SupportedModes)->ExtMode[0].TransferProtocol, HBA_PORTS_SCTL_IPM_PSSD);
        if (PMPort != 0xFF) {
            Data32 = HBA_PORTS_ERR_CLEAR;
            ReadWritePMPort (SataDevInterface, PMPort, PSCR_1_SERROR, &Data32, TRUE);
        }
    }

    Status = IdeControllerInterface->SetTiming(IdeControllerInterface, 
                                               Port, 
                                               PMPort == 0xFF ? 0 : PMPort, 
                                               (*SupportedModes));
    if (EFI_ERROR(Status)) return Status;

    SataDevInterface->PIOMode = 0xff;
    SataDevInterface->SWDma = 0xff;
    SataDevInterface->MWDma = 0xff;
    SataDevInterface->UDma = 0xff;    

    if ((*SupportedModes)->PioMode.Valid)
        SataDevInterface->PIOMode = (*SupportedModes)->PioMode.Mode;

    if ((*SupportedModes)->SingleWordDmaMode.Valid)
        SataDevInterface->SWDma = (*SupportedModes)->SingleWordDmaMode.Mode;

    if ((*SupportedModes)->MultiWordDmaMode.Valid)
        SataDevInterface->MWDma = (*SupportedModes)->MultiWordDmaMode.Mode;

    if ((*SupportedModes)->UdmaMode.Valid)
        SataDevInterface->UDma = (*SupportedModes)->UdmaMode.Mode;

    SataDevInterface->IORdy = (((EFI_IDENTIFY_DATA *)&(SataDevInterface->IdentifyData))->AtaData.capabilities_49 & 0x800) >> 11;

    if  ((SataDevInterface->IdentifyData.Valid_Bits_53 & 0x2) && ((*SupportedModes)->PioMode.Valid)){
            ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
            CommandStructure.Features = SET_TRANSFER_MODE;
            CommandStructure.Command = SET_FEATURE_COMMAND;
            CommandStructure.SectorCount = PIO_FLOW_CONTROL | SataDevInterface->PIOMode;
            Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);
    }

    if (EFI_ERROR(Status))
        return EFI_DEVICE_ERROR;

    // Issue Set Multiple Mode Command only for ATA device
    if  (SataDevInterface->DeviceType == ATA){
        Data8 = SataDevInterface->IdentifyData.Maximum_Sector_Multiple_Command_47 & 0xff;
        if (Data8 & 0x2)  Data8 = 2;
        if (Data8 & 0x4)  Data8 = 0x4;
        if (Data8 & 0x8)  Data8 = 0x8;
        if (Data8 & 0x10) Data8 = 0x10;
        if (Data8 & 0x20) Data8 = 0x20;
        if (Data8 & 0x40) Data8 = 0x40;
        if (Data8 & 0x80) Data8 = 0x80;
        
        if (Data8 > 1) {
            ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
            CommandStructure.Command = SET_MULTIPLE_MODE;
            CommandStructure.SectorCount = Data8;
            Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);
        }
    }

    // Check if  UDMA is supported
    if  (SataDevInterface->UDma != 0xff){
        ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
        CommandStructure.Features = SET_TRANSFER_MODE;
        CommandStructure.Command = SET_FEATURE_COMMAND;
        CommandStructure.SectorCount = UDMA_MODE | SataDevInterface->UDma;
        Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);
    } else {
        if (SataDevInterface->MWDma != 0xff){
            ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
            CommandStructure.Features = SET_TRANSFER_MODE;
            CommandStructure.Command = SET_FEATURE_COMMAND;
            CommandStructure.SectorCount = MWDMA_MODE | SataDevInterface->MWDma;
            Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);
        }
    }

    // Convert the Device string from English to Unicode
    SataDevInterface->UDeviceName = NULL;
    for (Index = 0; Index < 40; Index += 2) {
        DeviceName[Index] = ((UINT8 *)SataDevInterface->IdentifyData.Model_Number_27)[Index + 1];
        DeviceName[Index + 1] = ((UINT8 *)SataDevInterface->IdentifyData.Model_Number_27)[Index];
    }
    DeviceName[40] = 0;                 // Word

    // Remove the spaces from the end of the device name
    for (Index = 39; Index > 0; Index-- ) {
        if (DeviceName[Index] != 0x20) { 
        	break;
        }
        DeviceName[Index] = 0;
    }
    
    tempUnicodeTable = MallocZ(sizeof (EFI_UNICODE_STRING_TABLE) * 2);
    Status = pBS->AllocatePool (EfiBootServicesData,
                                sizeof (Language),
                                (VOID**)&tempUnicodeTable[0].Language);
    ASSERT_EFI_ERROR(Status);
    Status = pBS->AllocatePool (EfiBootServicesData,
                                sizeof (DeviceName),
                                (VOID**)&tempUnicodeTable[0].UnicodeString);
    ASSERT_EFI_ERROR(Status);
    pBS->CopyMem(tempUnicodeTable[0].Language, &Language, sizeof(Language)); 
    pBS->CopyMem(tempUnicodeTable[0].UnicodeString, DeviceName, sizeof (DeviceName)); 
    tempUnicodeTable[1].Language = NULL;
    tempUnicodeTable[1].UnicodeString = NULL;
    SataDevInterface->UDeviceName = tempUnicodeTable;

    #if SBIDE_SUPPORT
        InitMiscConfig(SataDevInterface);
    #endif  
    
    if( AhciPlatformPolicy->DipmSupport) {        
        // Initialize and Enable Device initiated Power management
        InitializeDipm(SataDevInterface);
    }
       
    if( AhciPlatformPolicy->DeviceSleepSupport) {
        // Initialize and Enable Device Sleep Support
        InitializeDeviceSleep(SataDevInterface);
    }

    ConfigurePowerUpInStandby(SataDevInterface);
    
    Status = GetIdentifyData(SataDevInterface);
    if (EFI_ERROR(Status)) return EFI_DEVICE_ERROR;

    return EFI_SUCCESS;
}

/**
    Configure the AHCI Controller

    @param    SataDevInterface 
    @param    SupportedModes 
    @param    Modes collection supported by the device 

    @retval   EFI_STATUS

    @note  
  1. Issue IdeControllerInterface->SetTiming for setting  uDMA 
     and PIO mode timings in the controller.
  2. Update the Read/Write command for the device
  3. Update the Device name used in Component Name protocol

**/ 
EFI_STATUS
ConfigureController (
    IN SATA_DEVICE_INTERFACE      *SataDevInterface,
    IN EFI_ATA_COLLECTIVE_MODE    *SupportedModes
)
{
    AMI_AHCI_BUS_PROTOCOL               *AhciBusInterface = SataDevInterface->AhciBusInterface; 
    EFI_IDE_CONTROLLER_INIT_PROTOCOL    *IdeControllerInterface = AhciBusInterface->IdeControllerInterface; 
    UINT8                               Port = SataDevInterface->PortNumber;
    UINT8                               PMPort = SataDevInterface->PMPortNumber;

    // Check if UDMA and MWDMA are programmed successfully
    if (SataDevInterface->UDma != 0xff) {
        if (!(ReturnMSBset((SataDevInterface->IdentifyData.UDMA_Mode_88 >> 8)) == SataDevInterface->UDma)) {
            SataDevInterface->UDma = ReturnMSBset((SataDevInterface->IdentifyData.UDMA_Mode_88 >> 8));
            SupportedModes->UdmaMode.Mode = SataDevInterface->UDma;
            IdeControllerInterface->SetTiming (IdeControllerInterface, Port, 
                                                PMPort == 0xFF ? 0 : PMPort, SupportedModes);
        }
    }
    else {
        if (SataDevInterface->MWDma != 0xff) {
            if (!(ReturnMSBset(SataDevInterface->IdentifyData.MultiWord_DMA_63 >> 8) == SataDevInterface->MWDma)) {
                SataDevInterface->MWDma = ReturnMSBset((SataDevInterface->IdentifyData.MultiWord_DMA_63 >> 8));
                SupportedModes->MultiWordDmaMode.Mode = SataDevInterface->MWDma;
                IdeControllerInterface->SetTiming (IdeControllerInterface, Port, 
                                                    PMPort == 0xFF ? 0 : PMPort, SupportedModes);
            }
        }
    }

    // Check for ATA
    if (SataDevInterface->DeviceType == ATA) {

        //  Update IDE Read/Write Command
        if  ((SataDevInterface->IdentifyData.Valid_Bits_59 & 0x100) && 
                (SataDevInterface->IdentifyData.Valid_Bits_59 & 0xff)) {    // Check if Multiple Read/Write
            if (SataDevInterface->IdentifyData.Command_Set_Supported_83 & 0x400) {

                    // 48Bit LBA supported
                    SataDevInterface->ReadCommand = READ_MULTIPLE_EXT;  
                    SataDevInterface->WriteCommand = WRITE_MULTIPLE_EXT;
                } else {
                    SataDevInterface->ReadCommand = READ_MULTIPLE;
                    SataDevInterface->WriteCommand = WRITE_MULTIPLE;
                }
            } else {                      // End of Multiple. 1 Block = 1 Sector
                if (SataDevInterface->IdentifyData.Command_Set_Supported_83 & 0x400) {

                    // 48Bit LBA supported
                    SataDevInterface->ReadCommand = READ_SECTORS_EXT;
                    SataDevInterface->WriteCommand = WRITE_SECTORS_EXT;
                } else {
                    SataDevInterface->ReadCommand = READ_SECTORS;
                    SataDevInterface->WriteCommand = WRITE_SECTORS;
                }
            }
        if (DMACapable(SataDevInterface)) {
            #if IDEBUSMASTER_SUPPORT
            SataDevInterface->ReadCommand = READ_DMA;
            SataDevInterface->WriteCommand = WRITE_DMA;
            if (SataDevInterface->IdentifyData.Command_Set_Supported_83 & 0x400) {

                // 48Bit LBA supported
                SataDevInterface->ReadCommand = READ_DMA_EXT;
                SataDevInterface->WriteCommand = WRITE_DMA_EXT;
            }
            #endif
        }
    }                                               // end of ATA

#if DISABLE_SOFT_SET_PREV

    // Software settings preserved
    if ((SataDevInterface->DeviceType == ATA) &&
        (SataDevInterface->IdentifyData.Reserved_76_79[0] != 0xFFFF )&&
        (SataDevInterface->IdentifyData.Reserved_76_79[2] & 0x0040)) {

            COMMAND_STRUCTURE           CommandStructure;
            EFI_STATUS                  Status;
            ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
            CommandStructure.Command = SET_FEATURE_COMMAND;
            CommandStructure.Features = DISABLE_SATA2_SOFTPREV;
            CommandStructure.SectorCount = 6;
            ExecuteNonDataCommand (SataDevInterface, CommandStructure);

            // Update the Identify device buffer
            Status = GetIdentifyData(SataDevInterface);
            if (EFI_ERROR(Status)){
                return EFI_DEVICE_ERROR;
            }
    }
#endif

    return EFI_SUCCESS;
}

/**
    Initialize Dipm in the device

    @param SataDevInterface 

    @retval VOID

**/ 

VOID
InitializeDipm (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{
    COMMAND_STRUCTURE       CommandStructure;
 
    ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
    CommandStructure.Command        = SET_FEATURE_COMMAND;
    CommandStructure.SectorCount    = DIPM_SUB_COMMAND; // 0x03 : DiPM

    // Check Host Supports Aggressive Link Power Management 
    // and Check DiPM supported by device
    if((SataDevInterface->AhciBusInterface->HBACapability & HBA_CAP_SALP) && 
       (SataDevInterface->IdentifyData.Reserved_76_79[2] & IDENTIFY_DIPM_SUPPORT)){ 
        
        // Check DiPM needs to be enabled from Platform Policy
        if(AhciPlatformPolicy->DipmEnable) {
            // Check DiPM is already enabled via Identify Data Word.
            if (!(SataDevInterface->IdentifyData.Reserved_76_79[3] & IDENTIFY_DIPM_ENABLED)) {
                // Enable DiPM and Issue Set Feature command.
                CommandStructure.Features = DIPM_ENABLE; 
                ExecuteNonDataCommand (SataDevInterface, CommandStructure);
            } 
        } else {
            if (SataDevInterface->IdentifyData.Reserved_76_79[3] & IDENTIFY_DIPM_ENABLED) {
                
                // Disable DiPM and Issue Set Feature command only if it is enabled already.
                CommandStructure.Features = DIPM_DISABLE; 
                ExecuteNonDataCommand (SataDevInterface, CommandStructure);
            }
        }     
    }

    return;
}

/**
    Initialize DevSleep in the device and controller

    @param SataDevInterface 

    @retval VOID

**/ 

VOID
InitializeDeviceSleep (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{

    AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface = SataDevInterface->AhciBusInterface; 
    UINT8                   PortNumber = SataDevInterface->PortNumber;
    COMMAND_STRUCTURE       CommandStructure;
    EFI_STATUS              Status;
    UINT8                   DevSleep_Exit_TimeOut = DEVSLEEP_EXIT_TIMEOUT;        
    UINT8                   Minimum_DevSleep_Assertion_Time = MINIMUM_DEVSLP_ASSERTION_TIME;        
    UINT8                   DITO_Multiplier = 0xF;     
    UINT8                   *Buffer = NULL;
    UINT32                  PxDevSlp;
    UINT8                   SectorCount = 1;

    // If Aggressive DelSlp is supported then DevSlp also supported. Also check if the port supports DevSlp or not           
    if ((HBA_REG32(AhciBusInterface, HBA_CAP2) & HBA_CAP2_SADM)  &&   
            (HBA_PORT_REG32(AhciBusInterface, PortNumber, HBA_PORTS_PxDEVSLP) & HBA_PORTS_PxDEVSLP_DSP)) { 

        //Clear ADSE
        HBA_PORT_REG32_AND (AhciBusInterface, PortNumber, HBA_PORTS_PxDEVSLP, ~(HBA_PORTS_PxDEVSLP_ADSE)); 

        // Check Word 78 Bit8 and Word 77 Bit7 of Identify Data.
        if ((SataDevInterface->IdentifyData.Reserved_76_79[2] & IDENTIFY_DEVSLEEP_SUPPORT) && 
                (SataDevInterface->IdentifyData.Reserved_76_79[1] & IDENTIFY_DEVSLP_TO_REDUCED_PWRSTATE_CAPABLE)) {
            
            if(AhciPlatformPolicy->DeviceSleepEnable) {

                // DevSlp not enabled in device. Issue Set Feature command. 
                if (!(SataDevInterface->IdentifyData.Reserved_76_79[3] & IDENTIFY_DEVSLEEP_ENABLED)) {
                
                    ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
                    CommandStructure.Command        = SET_FEATURE_COMMAND;
                    CommandStructure.SectorCount    = DEVSLEEP_SUB_COMMAND; 
                    CommandStructure.Features       = DEVSLEEP_ENABLE; 
                    ExecuteNonDataCommand (SataDevInterface, CommandStructure);
                } 
            
                // Allocate Memory for Identify Device Data
                pBS->AllocatePool(EfiBootServicesData, ATA_SECTOR_BYTES, (VOID**)&Buffer);
                ZeroMemory(Buffer, ATA_SECTOR_BYTES); 
    
                // Get Identify Device Data Log (log 30h Page 8)
                ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
                CommandStructure.Command        = READ_LOG_EXT;
                CommandStructure.SectorCount    = SectorCount;
                CommandStructure.LBAMid         = SERIAL_ATA_SETTINGS_PAGE;
                CommandStructure.LBALow         = IDENTIFY_DEVICE_DATA_LOG;
                CommandStructure.ByteCount      = SectorCount * ATA_SECTOR_BYTES;;
                CommandStructure.Buffer         = Buffer;
    
                Status = ExecutePioDataCommand (SataDevInterface, &CommandStructure, FALSE);
                if (!EFI_ERROR(Status)){
                    
                    // Update the Time out values
                    ///BYTE 30h..37h DEVSLP Timing Variables (Qword) of Identify Device Data log (log 30h page 08h) 
                            ///63 Valid 
                            ///62:16 Reserved 
                            ///15:8 DevSleep Exit Timeout, in ms (DETO) 
                            ///5:7 Reserved 
                            ///4:0 Minimum DEVSLP Assertion Time, in ms (MDAT) 
                            
                    DevSleep_Exit_TimeOut = Buffer[DEVSLP_TIMING_VARIABLES_OFFSET + 1] ?  Buffer[DEVSLP_TIMING_VARIABLES_OFFSET + 1] : DEVSLEEP_EXIT_TIMEOUT;
                    Minimum_DevSleep_Assertion_Time = (Buffer[DEVSLP_TIMING_VARIABLES_OFFSET] & 0x1F) ? (Buffer[DEVSLP_TIMING_VARIABLES_OFFSET] & 0x1F) : MINIMUM_DEVSLP_ASSERTION_TIME;
                }
    
                // Program the Timeouts and Multiplier value in PxDEVSLP Registers
                PxDevSlp = HBA_PORT_REG32 (AhciBusInterface, PortNumber, HBA_PORTS_PxDEVSLP);
                PxDevSlp &= ~(HBA_PORTS_PxDEVSLP_DETO_MASK | HBA_PORTS_PxDEVSLP_DMDAT_MASK | HBA_PORTS_PxDEVSLP_DM_MASK);
                PxDevSlp |= ((DevSleep_Exit_TimeOut << 2) +  (Minimum_DevSleep_Assertion_Time << 10) + (DITO_Multiplier << 25));
                HBA_PORT_REG32_OR (AhciBusInterface, PortNumber, HBA_PORTS_PxDEVSLP, PxDevSlp);
    
                // Enable PxDEVSLP.ADSE
                HBA_PORT_REG32_OR (AhciBusInterface, PortNumber, HBA_PORTS_PxDEVSLP, HBA_PORTS_PxDEVSLP_ADSE);
                pBS->FreePool(Buffer);
            } else {
                if (SataDevInterface->IdentifyData.Reserved_76_79[3] & IDENTIFY_DEVSLEEP_ENABLED) {
                    // Disable Device Sleep and Issue Set Feature command only if it is enabled already.
                    ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
                    CommandStructure.Command        = SET_FEATURE_COMMAND;
                    CommandStructure.SectorCount    = DEVSLEEP_SUB_COMMAND; 
                    CommandStructure.Features       = DEVSLEEP_DISABLE; 
                    ExecuteNonDataCommand (SataDevInterface, CommandStructure);
                }
            }
        }
    }
    
    return;
}

/**

    @param    AhciBusInterface 
    @param    Port 
    @param    PMPort 

    @retval 
        SATA_DEVICE_INTERFACE*

    @note  
      1. Return the Pointer to the SATA_DEVICE_INTERFACE for the given
         Port and PM Port

**/ 
SATA_DEVICE_INTERFACE *
GetSataDevInterface(
    IN AMI_AHCI_BUS_PROTOCOL    *AhciBusInterface, 
    IN UINT8                    Port,
    IN UINT8                    PMPort
)
{
    DLINK                   *dlink = AhciBusInterface->SataDeviceList.pHead; 
    SATA_DEVICE_INTERFACE   *SataDevInterface = NULL;

    if (!dlink) return NULL;
    do {
        SataDevInterface = OUTTER(dlink, SataDeviceLink, SATA_DEVICE_INTERFACE);
        if (SataDevInterface->PortNumber == Port && 
                SataDevInterface->PMPortNumber == PMPort ) break;
        dlink = dlink-> pNext;
        SataDevInterface = NULL;
    }while (dlink);    

    return SataDevInterface;
}

/**
    Wait for memory to be set to the test value.

    @param    MemTestAddr      - The memory address to test
    @param    MaskValue        - The mask value of memory
    @param    TestValue        - The test value of memory
    @param    WaitTimeInMs     - The time out value for wait memory set

    @retval    EFI_SUCCESS HBA reset successfully.
    @retval    EFI_DEVICE_ERROR HBA failed to complete hardware reset.

**/ 

EFI_STATUS 
WaitForMemSet (
    IN AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface,
    IN UINT8    Port,
    IN UINT8    Register,
    IN UINT32   AndMask,
    IN UINT32   TestValue,
    IN UINT32   WaitTimeInMs
)
{
    UINT8  Delay;
    while(WaitTimeInMs!=0){ 
        for ( Delay = 10; Delay > 0; Delay--) {

            if(((HBA_PORT_REG32(AhciBusInterface, Port, Register)) & AndMask) == TestValue){
                return EFI_SUCCESS;
            }

            pBS->Stall (100);  // 100 usec * 10 = 1Msec
        }
        WaitTimeInMs--;
   }
   return EFI_DEVICE_ERROR;
}

/**
    Wait for memory to be set to the test value.

    @param  MemTestAddr      - The memory address to test
    @param  MaskValue        - The mask value of memory
    @param  WaitTimeInMs     - The time out value for wait memory set

    @retval EFI_SUCCESS HBA reset successfully.
    @retval EFI_DEVICE_ERROR HBA failed to complete hardware reset.

**/ 
EFI_STATUS 
WaitForMemClear (
    IN AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface,
    IN UINT8    Port,
    IN UINT8    Register,
    IN UINT32   AndMask,
    IN UINT32   WaitTimeInMs
)
{
    UINT8  Delay;
    while(WaitTimeInMs!=0){ 
        for ( Delay = 10; Delay > 0; Delay--) {
            if(!((HBA_PORT_REG32(AhciBusInterface, Port, Register)) & AndMask)){
                return EFI_SUCCESS;
            }
            pBS->Stall (100);  // 100 usec * 10 = 1Msec
        }
        WaitTimeInMs--;
   }
   return EFI_DEVICE_ERROR;
}

/**
    Returns the MOST significant Bit set.

    @param    Data 

    @retval   UINT8

**/

UINT8
ReturnMSBset (
    IN UINT32   Data
)
{
    UINT8    Index;
    UINT8    Value = 0xFF;

    for (Index = 0; Index < 32; Index++) {
        if ( Data & 1) { 
            Value = Index;
        }
            Data >>= 1;
    }

    return Value;
}


/**
    Initializes SATA Block IO interface

    @param  SataDevInterface 

    @retval EFI_STATUS

    @note  
  Here is the control flow of this function:
  1. Initialize EFI_BLOCK_IO_PROTOCOL Protocol.
  2. In case of Removable devices, detect Media presence.

**/

EFI_STATUS
InitSataBlockIO (
    IN SATA_DEVICE_INTERFACE            *SataDevInterface
)
{
    EFI_STATUS               Status;
    EFI_BLOCK_IO_PROTOCOL    *BlkIo;
    SATA_BLOCK_IO            *SataBlkIo;
    EFI_BLOCK_IO_MEDIA       *BlkMedia;
    ATAPI_DEVICE             *AtapiDevice;
    UINT8                    *Data, *InquiryData;
    UINT16                   InquiryDataSize;
    UINT16                   OddType=0;
    UINT8                    OddLoadingType=0xFF;
    UINT32                   SectorSize = ATA_SECTOR_BYTES;

    Status = pBS->AllocatePool (EfiBootServicesData,
                sizeof(SATA_BLOCK_IO),
                (VOID**)&SataBlkIo);
    if (EFI_ERROR(Status)) return Status;

    BlkMedia = MallocZ(sizeof(EFI_BLOCK_IO_MEDIA));
    if (!BlkMedia) {
        pBS->FreePool (SataBlkIo);
        return EFI_OUT_OF_RESOURCES;
    }

    // Initialize the IdeBlkIo pointer in IDE_BUS_PROTOCOL (IdeBusInterface)
    SataDevInterface->SataBlkIo = SataBlkIo;

    // Initialize the fields in IdeBlkIo (SATA_BLOCK_IO)
    SataBlkIo->SataDevInterface = SataDevInterface;

    if (SataDevInterface->DeviceType == ATA){
        // ATA
        BlkIo = &(SataBlkIo->BlkIo);

#if defined CORE_COMBINED_VERSION && CORE_COMBINED_VERSION > 0x4028a
        if(pST->Hdr.Revision >= 0x0002001F) {
            BlkIo->Revision    = EFI_BLOCK_IO_PROTOCOL_REVISION3;
        } else {
            BlkIo->Revision    = BLKIO_REVISION;
        }
#else 
        BlkIo->Revision    = BLKIO_REVISION;
#endif
        BlkIo->Media = BlkMedia;
        BlkIo->Reset = SataReset;
        BlkIo->ReadBlocks = SataBlkRead;
        BlkIo->WriteBlocks = SataBlkWrite;
        BlkIo->FlushBlocks = SataBlkFlush;

        BlkMedia->MediaId = 0;
        BlkMedia->RemovableMedia = (SataDevInterface->IdentifyData.General_Config_0 & 0x80) == 0x80 ? TRUE : FALSE;

        BlkMedia->MediaPresent = TRUE;
        BlkMedia->LogicalPartition = FALSE;
        BlkMedia->ReadOnly = FALSE;
        BlkMedia->WriteCaching = FALSE;

        if((SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT14) && // WORD 106 valid? - BIT 14 - 1
           (!(SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT15)) && // WORD 106 valid? - BIT 15 - 0
           (SataDevInterface->IdentifyData.Reserved_104_126[2] & BIT12)) { // WORD 106 bit 12 - Sectorsize > 256 words
            // The sector size is in words 117-118.
            SectorSize = (UINT32)(SataDevInterface->IdentifyData.Reserved_104_126[13] + \
                                  (SataDevInterface->IdentifyData.Reserved_104_126[14] << 16)) * 2;
        }

        BlkMedia->BlockSize = SectorSize;
        BlkMedia->IoAlign = 4;

        if (SataDevInterface->IdentifyData.Command_Set_Supported_83 & 0x400) {
            BlkMedia->LastBlock = SataDevInterface->IdentifyData.LBA_48 - 1;
        }
        else {
            BlkMedia->LastBlock = SataDevInterface->IdentifyData.Addressable_Sector_60 - 1;
        }

#if defined CORE_COMBINED_VERSION && CORE_COMBINED_VERSION > 0x4028a
        if(pST->Hdr.Revision >= 0x0002001F) {

            BlkMedia->OptimalTransferLengthGranularity=BlkMedia->BlockSize;
            // bit 14 of word 106 is set to one and bit 15 of word 106 is cleared to zero, 
            // then the contents of word 106 contain valid information . 
            // Otherwise, information is not valid in this word.
            if ( (SataDevInterface->IdentifyData.Reserved_104_126[2] & 0xC000) == 0x4000 ) {
                // If bit 13 of word 106 is set to one, then the device has more than one 
                // logical sector per physical sector and bits (3:0) of word 106 are valid
                if ( SataDevInterface->IdentifyData.Reserved_104_126[2] & 0x2000 ) {
                    BlkMedia->LogicalBlocksPerPhysicalBlock = 1 << 
                                    (SataDevInterface->IdentifyData.Reserved_104_126[2] & 0xF);
                    // Bits 13:0 of word 209 indicate the Logical sector offset within the first physical 
                    // sector where the first logical sector is placed
                    BlkMedia->LowestAlignedLba =SataDevInterface->IdentifyData.Reserved_206_254[3] & 0x3FFF;
                } else {
                    // Default set the 1 logical blocks per PhysicalBlock
                    BlkMedia->LogicalBlocksPerPhysicalBlock=1;
                    // Default value set to 0 for Lowest Aligned LBA
                    BlkMedia->LowestAlignedLba=0;
                }
            } else {
                    // Default set the 1 logical blocks per PhysicalBlock
                    BlkMedia->LogicalBlocksPerPhysicalBlock=1;
                    // Default value set to 0 for Lowest Aligned LBA
                    BlkMedia->LowestAlignedLba=0;
            }
        }
#endif

    } else {

        // If it is an ATAPI device, check whether it is a CDROM or not. 
        // Currently only CDROM/Direct access Devices are supported.
        if ((SataDevInterface->IdentifyData.General_Config_0 & 0x1f00) == (CDROM_DEVICE << 8) ||
            (SataDevInterface->IdentifyData.General_Config_0 & 0x1f00) == (DIRECT_ACCESS_DEVICE << 8) ||
            (SataDevInterface->IdentifyData.General_Config_0 & 0x1f00) == (OPTICAL_MEMORY_DEVICE << 8)){

            AtapiDevice = MallocZ(sizeof (ATAPI_DEVICE));

            if (!AtapiDevice)   return EFI_OUT_OF_RESOURCES;

            SataDevInterface->AtapiDevice = AtapiDevice;

            Status = pBS->AllocatePool (EfiBootServicesData,
                                        16,
                                        (VOID**)&Data);

            if (EFI_ERROR(Status))  return Status;

            AtapiDevice->PacketBuffer = Data;
            AtapiDevice->DeviceType = (SataDevInterface->IdentifyData.General_Config_0 & 0x1f00) >> 8; 
            AtapiDevice->PacketSize = (SataDevInterface->IdentifyData.General_Config_0 & 3) == 1 ? 16 : 12; 

            BlkIo = &(SataBlkIo->BlkIo);
#if defined CORE_COMBINED_VERSION && CORE_COMBINED_VERSION > 0x4028a
            if(pST->Hdr.Revision >= 0x0002001F) {
                BlkIo->Revision    = EFI_BLOCK_IO_PROTOCOL_REVISION3;
            } else {
                BlkIo->Revision    = BLKIO_REVISION;
            }
#else 
            BlkIo->Revision    = BLKIO_REVISION;
#endif
            BlkIo->Media = BlkMedia;
            BlkIo->Reset = SataReset;
            BlkIo->ReadBlocks = SataAtapiBlkRead;
            BlkIo->WriteBlocks = SataAtapiBlkWrite;
            BlkIo->FlushBlocks = SataBlkFlush;
            BlkMedia->BlockSize = CDROM_BLOCK_SIZE;

            // Update Inquiry Data
            Status = pBS->AllocatePool (EfiBootServicesData,
                                        INQUIRY_DATA_LENGTH,
                                        (VOID**)&InquiryData);
            if (EFI_ERROR(Status))  return Status;
            InquiryDataSize = INQUIRY_DATA_LENGTH;
            SataAtapiInquiryData(SataDevInterface, InquiryData, &InquiryDataSize);
            AtapiDevice->InquiryData = InquiryData;
            AtapiDevice->InquiryDataSize = InquiryDataSize;

            //
            // Get the ATAPI drive type information and save it ATAPI_DEVICE
            //
            Status=SataGetOddType(SataDevInterface, &OddType);
            if(!EFI_ERROR(Status)) {
                AtapiDevice->OddType=GetEnumOddType(OddType);
            }  

            //
            // Get the ATAPI drive Loading information and save it ATAPI_DEVICE
            //
            Status=SataGetOddLoadingType(SataDevInterface, &OddLoadingType);

            if ( !EFI_ERROR( Status )) {
                AtapiDevice->OddLoadingType = (ODD_LOADING_TYPE)OddLoadingType;
            } else {
                AtapiDevice->OddLoadingType = (ODD_LOADING_TYPE)0xFF;
            }

            BlkMedia->RemovableMedia = (SataDevInterface->IdentifyData.General_Config_0 & 0x80) == 0x80 ? TRUE : FALSE;
            BlkMedia->LogicalPartition = FALSE;
            BlkMedia->WriteCaching = FALSE;
            BlkMedia->IoAlign = 4;
#if defined CORE_COMBINED_VERSION && CORE_COMBINED_VERSION > 0x4028a
            if(pST->Hdr.Revision >= 0x0002001F) {

                // For Atapi Devices, Default set the 1 for logical blocks per PhysicalBlock
                BlkMedia->LogicalBlocksPerPhysicalBlock=1;
    
                // For Atapi Devices, Default value set to 0 for Lowest Aligned LBA
                BlkMedia->LowestAlignedLba=0;
    
                BlkMedia->OptimalTransferLengthGranularity=BlkMedia->BlockSize;
            }
#endif
        }
    }
    return EFI_SUCCESS;
}

/**
    Get the Enum value for ODD type found on profile

    @param  UINT16      Oddtype

    @retval ODD_TYPE    EnumValue

**/

ODD_TYPE 
GetEnumOddType (
    UINT16  OddType
)
{

    switch(OddType) {
        case 1:
        case 3:
        case 4:
        case 5:
                return Obsolete;

        case 2:
                return Removabledisk;

        case 8:
                return CDROM;

        case 9:
                return CDR;

        case 0xa:
                return CDRW;

        case 0x10:
                return DVDROM;

        case 0x11:
                return DVDRSequentialrecording;

        case 0x12:
                return DVDRAM;

        case 0x13:
                return DVDRWRestrictedOverwrite;

        case 0x15:
                return DVDRWSequentialrecording;

        case 0x16:
                return DVDRDualLayerJumprecording;

        case 0x17:
                return DVDRWDualLayer;

        case 0x18:
                return DVDDownloaddiscrecording;

        case 0x1a:
                return DVDRW;

        case 0x1b:
                return DVDR;

        case 0x40:
                return BDROM;

        case 0x41:
                return BDRSequentialRecording;

        case 0x42:
                return BDRRandomRecordingMode;

        case 0x43:
                return BDRE;

        case 0x50:
                return HDDVDROM;

        case 0x51:
                return HDDVDR;

        case 0x52:
                return HDDVDRAM;

        case 0x53:
                return HDDVDRW;

        case 0x58:
                return HDDVDRDualLayer;

        case 0x5a:
                return HDDVDRWDualLayer;

        default:
                return UnknownType;
    }
}

/**
    Initializes SATA DiskInfo Interface

    @param  SataDevInterface 

    @retval EFI_STATUS

**/

EFI_STATUS
InitSataDiskInfo (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{
    EFI_STATUS        Status;
    SATA_DISK_INFO    *SataDiskInfo;

    Status = pBS->AllocatePool (EfiBootServicesData,
                sizeof(SATA_DISK_INFO),
                (VOID**)&SataDiskInfo);
    if (EFI_ERROR(Status)){
        return Status;
    }

    // Initialize the SataBlkIo pointer in SATA_DEVICE_INTERFACE
    SataDevInterface->SataDiskInfo = SataDiskInfo;

    // Initialize the fields in SataDiskInfo (SATA_DISK_INFO)
    SataDiskInfo->SataDevInterface = SataDevInterface;

    pBS->CopyMem (&(SataDiskInfo->DiskInfo.Interface), &gEfiDiskInfoAhciInterfaceGuid, sizeof (EFI_GUID));
    SataDiskInfo->DiskInfo.Inquiry = DiskInfoInquiry;
    SataDiskInfo->DiskInfo.Identify = DiskInfoIdentify;
    SataDiskInfo->DiskInfo.SenseData = DiskInfoSenseData;
    SataDiskInfo->DiskInfo.WhichIde = DiskInfoWhichIDE;
    return EFI_SUCCESS;

}


/**
    Creates a SATA device device path and adds it to SataDevInterface

        
    @param This 
    @param Controller 
    @param SataDevInterface 
    @param RemainingDevicePath 

    @retval EFI_STATUS

    @note  
  Here is the control flow of this function:
  1. If Remaining Device path is not NULL, we have already verified that it is a 
          valid Atapi device path in IdeBusStart. So nothing to do. Just exit.  
  2. Build a Atapi device path and a End device path. 
  3. Get the Device path for the IDE controller.
  4. Append Atapi device path to  IDE controller device path.

**/

EFI_STATUS
CreateSataDevicePath (
    IN EFI_DRIVER_BINDING_PROTOCOL      *This,
    IN EFI_HANDLE                       Controller,
    IN SATA_DEVICE_INTERFACE            *SataDevInterface,
    IN OUT EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
)
{

    EFI_STATUS                  Status ;
    SATA_DEVICE_PATH            NewDevicePath;
    EFI_DEVICE_PATH_PROTOCOL    *TempDevicePath;

    NewDevicePath.Header.Type = MESSAGING_DEVICE_PATH;
    NewDevicePath.Header.SubType = MSG_USB_SATA_DP;
    SET_NODE_LENGTH(&NewDevicePath.Header,SATA_DEVICE_PATH_LENGTH);
    NewDevicePath.HBAPortNumber = SataDevInterface->PortNumber;
    NewDevicePath.PortMultiplierPortNumber = SataDevInterface->PMPortNumber == 0xFF ? 0xFFFF : SataDevInterface->PMPortNumber;
    NewDevicePath.Lun = 0;

    // Append the Device Path
    Status = pBS->OpenProtocol( Controller,
                                &gEfiDevicePathProtocolGuid,
                                (VOID **)&TempDevicePath,
                                This->DriverBindingHandle,     
                                Controller,   
                                EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    SataDevInterface->DevicePathProtocol = DPAddNode(TempDevicePath, &NewDevicePath.Header);
    return Status;

}

#if SBIDE_SUPPORT

/**
    Initialize misc IDE configurations.

    @param SataDevInterface 

    @retval VOID

**/

VOID InitMiscConfig (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{
    IDE_SETUP_PROTOCOL              *gIdeSetupProtocol;
    EFI_STATUS                      Status;

    Status = pBS->LocateProtocol(&gIdeSetupProtocolguid, NULL, (VOID**)&gIdeSetupProtocol);
    if(EFI_ERROR(Status)){
        return;
    }

    #if ACOUSTIC_MANAGEMENT_SUPPORT
        InitAcousticSupport (SataDevInterface);
    #endif

}
#endif


/**
    Configure PowerUpInStandby

    @param  SataDevInterface 

    @retval EFI_STATUS

    @note  
     1. Check if the device support PowerUp In Standby.
     2. Check the desired state Vs the current state.
     3. If both are equal nothing to do, exit else program the desired level

**/

EFI_STATUS
ConfigurePowerUpInStandby (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{
    COMMAND_STRUCTURE   CommandStructure;
    EFI_STATUS          Status = EFI_SUCCESS;

    if(AhciPlatformPolicy->PowerUpInStandbySupport) { 

        // Check if the device supports PowerUpInStandby feature
        if  (SataDevInterface->IdentifyData.Command_Set_Supported_83 & 0x20){
            //Check if the desired state is already present or not
            if  (!((SataDevInterface->IdentifyData.Command_Set_Enabled_86 & 0x20) == 
                    (AhciPlatformPolicy->PowerUpInStandbyMode))){

                ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
                CommandStructure.Command     = SET_FEATURE_COMMAND;
                CommandStructure.Features    = (AhciPlatformPolicy->PowerUpInStandbyMode) == 0 ? DISABLE_POWERUP_IN_STANDBY : ENABLE_POWERUP_IN_STANDBY;
                Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);
            }
        }
    }

    return Status;
}

#if ACOUSTIC_MANAGEMENT_SUPPORT

/**
    Initialize Acoustic functionality

    @param SataDevInterface 

    @retval EFI_STATUS

    @note  
     1. Check if the device support Acoustic management.
     2. Check the desired state Vs the current state.
     3. If both are equal nothing to do, exit else program the desired level

**/

EFI_STATUS
InitAcousticSupport (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{

    UINT8               Data8;
    COMMAND_STRUCTURE   CommandStructure;
    EFI_STATUS          Status;

    ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
    CommandStructure.Command = SET_FEATURE_COMMAND;

    // Check if the device supports Acoustic Management
    if  (SataDevInterface->IdentifyData.Command_Set_Supported_83 & 0x200){

        // Check if Acoustic Level need to be enabled or Disabled
        if (SataDevInterface->AhciBusInterface->Acoustic_Enable & ACOUSTIC_SUPPORT_ENABLE) {
            Data8 = SataDevInterface->AhciBusInterface->Acoustic_Management_Level;

            // Do we need to program the recommended value
            if (Data8 == ACOUSTIC_LEVEL_BYPASS) {

                // Get the recommended value
                Data8 = (UINT8)(SataDevInterface->IdentifyData.Acoustic_Level_94 >> 8);
            }

            CommandStructure.Features       = ACOUSTIC_MANAGEMENT_ENABLE;
            CommandStructure.SectorCount    = Data8;
            Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);

        } else {

            // If already disabled, nothing to do
            if  (SataDevInterface->IdentifyData.Command_Set_Enabled_86 & 0x200) {
                CommandStructure.Features       = ACOUSTIC_MANAGEMENT_DISABLE;
                CommandStructure.SectorCount    = 0;
                Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);
                ASSERT_EFI_ERROR(Status);
            }
        }
    }
    return EFI_SUCCESS;
}

#endif                          // ACOUSTIC_MANAGEMENT_SUPPORT ends


/**
    Check if DMA is supported

    @param SataDevInterface 

    @retval TRUE : DMA Capable

    @note  
      1. Check the Identify Data to check if device supports DMA

**/

BOOLEAN
DMACapable (
    IN SATA_DEVICE_INTERFACE    *SataDevInterface
)
{

    if (SataDevInterface->DeviceType == ATAPI){
        //For Atapi Devices check Bit 8 in Word 49 =  DMA   Supported or not
        if((SataDevInterface->IdentifyData.Capabilities_49 & 0x100) == 0) return FALSE;
    }

    if ( (SataDevInterface->UDma != 0xff) ||
        (SataDevInterface->MWDma != 0xff))
        return TRUE;
    else
    return FALSE;

}

/**
    Return ATAPI Inquiry data

    @param  This 
    @param  InquiryData 
    @param  InquiryDataSize 

    @retval EFI_STATUS

    @note  
  1. Check for Atapi Device. If not exit 
  2. COpy the Inquiry Data from AtapiDevice->InquiryData to the input pointer.

**/

EFI_STATUS
EFIAPI 
DiskInfoInquiry (
    IN EFI_DISK_INFO_PROTOCOL    *This,
    IN OUT VOID                  *InquiryData,
    IN OUT UINT32                *InquiryDataSize
)
{

    SATA_DEVICE_INTERFACE       *SataDevInterface = ((SATA_DISK_INFO *)This)->SataDevInterface;
    ATAPI_DEVICE                *AtapiDevice = SataDevInterface->AtapiDevice;

    // Check for ATAPI device. If not return EFI_NOT_FOUND
    if (SataDevInterface->DeviceType == ATA){
        return EFI_NOT_FOUND;               // ATA device
    } else {
        if (*InquiryDataSize < AtapiDevice->InquiryDataSize) {
            *InquiryDataSize = AtapiDevice->InquiryDataSize;
            return EFI_BUFFER_TOO_SMALL;
        }
        if (AtapiDevice->InquiryData != NULL) {
            pBS->CopyMem (InquiryData, AtapiDevice->InquiryData, AtapiDevice->InquiryDataSize);  
            *InquiryDataSize = AtapiDevice->InquiryDataSize;
            return  EFI_SUCCESS;
        }
        else    return EFI_NOT_FOUND;
    }
}

/**
    Return Identify Data

        
    @param EFI_DISK_INFO_PROTOCOL          *This,
    @param IdentifyData 
    @param IdentifyDataSize 

    @retval EFI_STATUS

    @note  
      1. Return the Identify command data.

**/
EFI_STATUS
EFIAPI 
DiskInfoIdentify (
    EFI_DISK_INFO_PROTOCOL    *This,
    IN OUT VOID               *IdentifyData,
    IN OUT UINT32             *IdentifyDataSize
)
{

    SATA_DEVICE_INTERFACE    *SataDevInterface = ((SATA_DISK_INFO *)This)->SataDevInterface;

    if (*IdentifyDataSize < sizeof (IDENTIFY_DATA)) {
        *IdentifyDataSize = sizeof (IDENTIFY_DATA);
        return EFI_BUFFER_TOO_SMALL;
    }

    // ATA devices identify data might be changed because of the SetFeature command, 
    // So read the data from the device again by sending identify command.
    if(SataDevInterface->DeviceType == ATA){
        GetIdentifyData(SataDevInterface);
    }

    pBS->CopyMem (IdentifyData, &(SataDevInterface->IdentifyData), sizeof (IDENTIFY_DATA)); 
    *IdentifyDataSize =  sizeof (IDENTIFY_DATA);
    return EFI_SUCCESS;
}

/**
    Return InfoSenseData.

    @param  EFI_DISK_INFO_PROTOCOL      *This,
    @param  VOID                        *SenseData,
    @param  UINT32                      *SenseDataSize,
    @param  UINT8                       *SenseDataNumber

    @retval EFI_STATUS

    @note  
      1. Return the Sense data for the Atapi device.

**/

EFI_STATUS
EFIAPI 
DiskInfoSenseData (
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
DiskInfoWhichIDE (
    IN EFI_DISK_INFO_PROTOCOL    *This,
    OUT UINT32                   *IdeChannel,
    OUT UINT32                   *IdeDevice
)
{

    SATA_DEVICE_INTERFACE       *SataDevInterface = ((SATA_DISK_INFO *)This)->SataDevInterface;

    *IdeChannel = SataDevInterface->PortNumber; 
    *IdeDevice = SataDevInterface->PMPortNumber == 0xFF ? 0 : SataDevInterface->PMPortNumber;
    return EFI_SUCCESS;
}


/**
    Send the Freeze Lock and device config Freeze Lock command to 
    all the Sata-ATA Devices connected 

    @param Event signalled event
    @param Context calling context

    @retval VOID

**/
VOID
EFIAPI
AhciFreezeLockDevice (
    IN EFI_EVENT    Event,
    IN VOID         *Context
        
)
{
    UINTN                    HandleCount;
    EFI_HANDLE              *HandleBuffer;
    UINT8                   i;
    AMI_AHCI_BUS_PROTOCOL   *AhciBusInterface;
    SATA_DEVICE_INTERFACE   *SataDevInterface;
    DLINK                   *dlink;
    COMMAND_STRUCTURE       CommandStructure = {0};
    EFI_STATUS              Status;    
#if SMCPKG_SUPPORT
    EFI_GUID SetupGuid 		    = SETUP_GUID;		
    SYSTEM_CONFIGURATION		SetupData;		
    UINTN VariableSize 		    = sizeof(SYSTEM_CONFIGURATION);
#endif 

    Status = pBS->LocateHandleBuffer (
                                      ByProtocol,
                                      &gAmiAhciBusProtocolGuid,
                                      NULL,
                                      &HandleCount,
                                      &HandleBuffer
                                      );

    if(EFI_ERROR(Status) || HandleCount == 0 ) {
        return;
    }

    //
    // Issue Freeze Lock command all the Sata-ATA Devices connected.
    //
    for (i = 0; i < HandleCount; i++) {
        Status = pBS->HandleProtocol (HandleBuffer[i], &gAmiAhciBusProtocolGuid, (VOID**)&AhciBusInterface);
        if (EFI_ERROR(Status)) {
            continue;
        }
        
        dlink = AhciBusInterface->SataDeviceList.pHead; 
        if (dlink){ 
            do {
                SataDevInterface = OUTTER(dlink, SataDeviceLink, SATA_DEVICE_INTERFACE);

                    
                // Update the Identify device buffer
                GetIdentifyData(SataDevInterface);
                    
                // If Security Feature support and HDD is not in frozen state or Locked state , send the 
                // Device freeze Lock command to frozen the device 
                    
                if (SataDevInterface->DeviceType == ATA &&
                        SataDevInterface->IdentifyData.Command_Set_Supported_82 & 0x2 &&
                        !(SataDevInterface->IdentifyData.Security_Status_128 & 0xC) ) {
                        
                    ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
#if SMCPKG_SUPPORT
                    Status = pRS->GetVariable(
                                      L"IntelSetup",
                                      &SetupGuid,
                                      NULL,
                                      &VariableSize,
                                      &SetupData); 

                    if(!Status){
                        if(SetupData.SMCSataFrozen){            
                            CommandStructure.Command = SECURITY_FREEZE_LOCK;
                            Status = ExecuteNonDataCommand(SataDevInterface, CommandStructure);
                         }
                     } else { 
                            CommandStructure.Command = SECURITY_FREEZE_LOCK;
                            Status = ExecuteNonDataCommand(SataDevInterface, CommandStructure);
                     }	
                     //
                     // if Device Configuration Overlay feature set supported then issue the
                     // Dev config Free lock command.
                     //
                     if (SataDevInterface->IdentifyData.Command_Set_Supported_83 & 0x800) {
                         Status = pRS->GetVariable(
                                           L"IntelSetup",
                                           &SetupGuid,
                                           NULL,
                                           &VariableSize,
                                           &SetupData);

                         if(!Status){
                             if(SetupData.SMCSataFrozen){                
                                 CommandStructure.Command = DEV_CONFIG_FREEZE_LOCK;
                                 CommandStructure.Features = DEV_CONFIG_FREEZE_LOCK_FEATURES;
                                 Status = ExecuteNonDataCommand(SataDevInterface, CommandStructure);
                             }
                         } else {
                                 CommandStructure.Command = DEV_CONFIG_FREEZE_LOCK;
                                 CommandStructure.Features = DEV_CONFIG_FREEZE_LOCK_FEATURES;
                                 Status = ExecuteNonDataCommand(SataDevInterface, CommandStructure);
                         }
                     }
#else
                    // Send the Freeze Lock command 
                    CommandStructure.Command = SECURITY_FREEZE_LOCK;
                    Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);
                        
                    // if Device Configuration Overlay feature set supported then issue the
                    // Dev configure Free lock command.
                    if (SataDevInterface->IdentifyData.Command_Set_Supported_83 & 0x800) {
                        ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE)); 
                        CommandStructure.Command = DEV_CONFIG_FREEZE_LOCK;
                        CommandStructure.Features = DEV_CONFIG_FREEZE_LOCK_FEATURES;
                        Status = ExecuteNonDataCommand (SataDevInterface, CommandStructure);
                    }
#endif //#if SMCPKG_SUPPORT 
                    // Update the Identify device buffer
                    GetIdentifyData(SataDevInterface);
                }
                
                dlink = dlink-> pNext;
            } while (dlink);    
        }
     }
    
    // Freeze Lock commands send to the device. Close the event as no more it's needed 
    pBS->CloseEvent(Event);
    
    return;
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
