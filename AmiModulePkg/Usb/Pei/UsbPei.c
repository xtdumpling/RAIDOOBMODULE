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

/** @file UsbPei.c
    PEI USB initialization code

**/

#include "Token.h"
#include "UsbPei.h"
#include "UsbBotPeim.h"
#include <PiPei.h>
#include <Ppi/AmiKeyCodePpi.h>
#include <Ppi/Stall.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Library/DebugLib.h>

//this funciton is created from InitList.c template file during build process
VOID InitParts(IN EFI_FFS_FILE_HEADER *FfsHeader,IN EFI_PEI_SERVICES **PeiServices);

EFI_GUID gPeiAmikeycodeInputPpiGuid = EFI_PEI_AMI_KEYCODE_PPI_GUID;
EFI_GUID gPeiChipUsbRecoveryInitPpiGuid = PEI_USB_CHIP_INIT_PPI_GUID;
EFI_GUID gAmiPeiUsbHostControllerPpiGuid = AMI_PEI_USB_HOST_CONTROLLER_PPI_GUID;
EFI_GUID gAmiPeiUsbIoPpiGuid = AMI_PEI_USB_IO_PPI_GUID;

#if USB_PEI_KEYBOARD_SUPPORT 
// USB keyboard buffer, its head and tail pointers
EFI_PEI_AMIKEYCODE_DATA gKeys[PEI_MAX_USB_KEYS];
EFI_PEI_AMIKEYCODE_DATA *gKeysHead;
EFI_PEI_AMIKEYCODE_DATA *gKeysTail;
UINT8   gNumLockOn  = PEI_USB_KEYBOARD_NUMLOCK_ON;
UINT8   gScrlLockOn = 0;
UINT8   gCapsLockOn = 0;
#endif

BOOLEAN gUsbBootInRecoveryNotifyFlag = FALSE;
BOOLEAN gUsbKeybaordBootNotifyFlag = FALSE;

EFI_PEI_NOTIFY_DESCRIPTOR gUsbPeiNotifyList = {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiEndOfPeiSignalPpiGuid,
    NotifyOnRecoveryCapsuleLoaded};

EFI_PEI_NOTIFY_DESCRIPTOR gUsbBootInRecoveryNotifyList = {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiBootInRecoveryModePpiGuid,
    InitializeUsbHostDevice};

EFI_PEI_NOTIFY_DESCRIPTOR gUsbKeybaordBootInRecoveryNotifyList = {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiBootInRecoveryModePpiGuid,
    InitializeUsbMassDevice};


/**
    This function enables USB controllers on the PCI bus, programs BARs,
    starts controllers and installs EFI_PEI_USBINIT_PPI.

**/

EFI_STATUS
EFIAPI
UsbPeiEntryPoint (
    IN       EFI_PEI_FILE_HANDLE    FileHandle,
    IN CONST EFI_PEI_SERVICES       **PeiServices
)
{
#if USB_PEI_KEYBOARD_SUPPORT
    UINTN                       PpiInstance;
    EFI_PEI_PPI_DESCRIPTOR      *TempPpiDescriptor;
    PEI_USB_IO_PPI              *UsbIoPpi;
    EFI_STATUS                  Status;
    
#if !USBPEI_IN_S3_SUPPORT
	EFI_BOOT_MODE	 BootMode;
	
	(**PeiServices).GetBootMode(PeiServices, &BootMode);
	if (BootMode == BOOT_ON_S3_RESUME) {
		return EFI_UNSUPPORTED;
	}
#endif

    InitializeUsbHostDevice((EFI_PEI_SERVICES **)PeiServices, NULL, NULL);

#if PEI_USB_MASS_STORAGE_SUPPORT_POLICY == 0
    (**PeiServices).NotifyPpi(PeiServices, &gUsbKeybaordBootInRecoveryNotifyList);
#else
    InitializeUsbMassDevice(PeiServices, NULL, NULL);
#endif

    for (PpiInstance = 0; PpiInstance < PEI_MAX_USB_IO_PPI; PpiInstance++) {
        Status = (**PeiServices).LocatePpi(PeiServices,
            &gAmiPeiUsbIoPpiGuid,
            PpiInstance,
            &TempPpiDescriptor,
            &UsbIoPpi);

        if (EFI_ERROR(Status)) {
            break;
        }

        InitUsbKbdPPI((EFI_PEI_SERVICES **)PeiServices, UsbIoPpi);
    }
    
#else
	(**PeiServices).NotifyPpi(PeiServices, &gUsbBootInRecoveryNotifyList);
#endif
	
    return EFI_SUCCESS;
}


/**
    Gets the keyboard data from TD if available and returns the next
    keystroke from input buffer.

         
    @retval EFI_NOT_READY There was no keystroke data available.
    @retval EFI_SUCCESS KeyData is filled with the most up-to-date keypress

**/

#if USB_PEI_KEYBOARD_SUPPORT 

EFI_STATUS
EFIAPI
PeiUsbReadKey(
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_AMIKEYCODE_PPI   *This,
    OUT EFI_PEI_AMIKEYCODE_DATA *KeyData
)
{
    UINTN          UsbIoPpiInstance;
    EFI_PEI_PPI_DESCRIPTOR *TempPpiDescriptor;
    PEI_USB_IO_PPI *UsbIoPpi;
    PEI_USB_DEVICE *PeiUsbDev;
    static UINT8    Data[8];
    static UINTN    DataLength = 8;
    EFI_STATUS      Status;

    // Check the keypress for every available USB keyboard.
    for (UsbIoPpiInstance = 0;
         UsbIoPpiInstance < PEI_MAX_USB_IO_PPI;
         UsbIoPpiInstance++)
    {
        Status = (**PeiServices).LocatePpi( PeiServices,
            &gAmiPeiUsbIoPpiGuid,
            UsbIoPpiInstance,
            &TempPpiDescriptor,
            &UsbIoPpi);

        if (EFI_ERROR(Status)) break;

        PeiUsbDev = PEI_USB_DEVICE_FROM_THIS(UsbIoPpi);
        if (PeiUsbDev->DeviceType != BIOS_DEV_TYPE_KEYBOARD) continue;

        Status = UsbIoPpi->UsbSyncInterruptTransfer(
            PeiServices,
            UsbIoPpi,
            PeiUsbDev->IntEndpoint,
            Data,
            &DataLength,
            PEI_USB_RECOVERYREQUEST_TIMEOUT  // timeout, ms
        );
        if (!EFI_ERROR(Status)) {
            PeiUsbLibProcessKeypress(PeiServices, UsbIoPpi, Data);    // This will update gKeys
        }
    }
    
    if (gKeysHead == gKeysTail) return EFI_NOT_READY;

    // Get the data and adjust the tail
    *KeyData = *gKeysTail++;
    if (gKeysTail == &gKeys[PEI_MAX_USB_KEYS])
    {
        gKeysTail = gKeys;
    }

    return EFI_SUCCESS;
}

/**
  Set certain state for the input device.

  @param  PeiServices            Describes the list of possible PEI Services.
  @param  This                   Protocol instance pointer.
  @param  KeyToggleState         The EFI_KEY_TOGGLE_STATE to set the
                                  state for the input device.

  @retval EFI_SUCCESS             The device state was set appropriately.
  @retval EFI_DEVICE_ERROR        The device is not functioning correctly and could
                                  not have the setting adjusted.
  @retval EFI_UNSUPPORTED         The device does not support the ability to have its state set.

**/

EFI_STATUS
EFIAPI
PeiUsbSetLedState(
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_AMIKEYCODE_PPI      *This,
    IN  EFI_KEY_TOGGLE_STATE        KeyToggleState
)
{
    UINTN                       Index;
    EFI_STATUS                  Status;
    EFI_PEI_PPI_DESCRIPTOR      *TempPpiDescriptor;
    PEI_USB_IO_PPI              *UsbIoPpi;
    
	if (!(KeyToggleState & TOGGLE_STATE_VALID)) {
		return EFI_UNSUPPORTED;
	}

    if (KeyToggleState & SCROLL_LOCK_ACTIVE) {
        gScrlLockOn = 1;
    } else {
        gScrlLockOn = 0;
    }

    if (KeyToggleState & NUM_LOCK_ACTIVE) {
        gNumLockOn = 1;
    } else {
        gNumLockOn = 0;
    }

    if (KeyToggleState & CAPS_LOCK_ACTIVE) {
        gCapsLockOn = 1;
    } else {
        gCapsLockOn = 0;
    }

    for (Index = 0;Index < PEI_MAX_USB_IO_PPI; Index++) {
        Status = (**PeiServices).LocatePpi( PeiServices,
            &gAmiPeiUsbIoPpiGuid,
            Index,
            &TempPpiDescriptor,
            &UsbIoPpi);

        if (EFI_ERROR(Status)) {
            break;
        }

        PeiUsbLibLightenKbLeds(PeiServices, UsbIoPpi);
    }
    
    return EFI_SUCCESS;
}

/**
    This function gets the key code and analyzes if it matches the recovery
    request pre-defined key sequence.

    @param 
        EFI_PEI_AMIKEYCODE_DATA *KeyData - key press information

         
    @retval TRUE recovery request key combination found
    @retval FALSE recovery is not requested

**/

BOOLEAN
IsUsbKbdRecovery (
    EFI_PEI_AMIKEYCODE_DATA *KeyData
)
{
    static UINT8 ModifierBits[] = {PEI_USB_RECOVERYREQUEST_KEY_MODIIFIER_VALUES 0};
    static UINT8 ModifierBitsCount = sizeof(ModifierBits) - 1;
    UINT8 i;

    // Check for modifier match
    if (ModifierBits[0] != 0) {
        for (i = 0; i < ModifierBitsCount; i++) {
            if ((KeyData->KeyState.KeyShiftState & ModifierBits[i]) != 0) {
                break;
            }
        }
        if ((i > 0) && (i == ModifierBitsCount)) {
            return FALSE;
        }
    }

    // Check for the EFI scan code
    // Note: in future there might be a need to support UniCode, Efi key and PS2 scan codes
    if (KeyData->Key.ScanCode == PEI_USB_RECOVERYREQUEST_KEY_EFISCANCODE) {
        return TRUE;
    }

    return FALSE;
}

/**
    Initialize USB keyboard input PPI.

**/

EFI_STATUS
InitUsbKbdPPI(
    EFI_PEI_SERVICES    **PeiServices,
    PEI_USB_IO_PPI      *UsbIoPpi
)
{
    EFI_USB_ENDPOINT_DESCRIPTOR  *EndpointDescriptor;
    PEI_USB_DEVICE  *PeiUsbDev = PEI_USB_DEVICE_FROM_THIS(UsbIoPpi);
    UINT8           EpIndx;
    EFI_STATUS      Status;
    UINTN           Count;
    UINT32          UsbStatus;

    static BOOLEAN UsbKeyboardPpiInstalled = FALSE;

    static EFI_PEI_AMIKEYCODE_PPI UsbKeyboardInputPpi = {
        PeiUsbReadKey,
        PeiUsbSetLedState
    };

    static EFI_PEI_PPI_DESCRIPTOR UsbKeyboardInputPpiList = {
        (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
        &gPeiAmikeycodeInputPpiGuid,
        NULL
    };

    static EFI_PEI_AMIKEYCODE_DATA KeyData = {0};

    if (PeiUsbDev->DeviceType != BIOS_DEV_TYPE_KEYBOARD) {
        return EFI_NOT_FOUND;
    }

    DEBUG((DEBUG_INFO, "PEI USB Keyboard found.\n"));

    // Initialize IntEndpoint and IntMaxPkt fields
    for (EpIndx = 0; EpIndx < MAX_ENDPOINT; EpIndx++) {
        Status = PeiUsbGetEndpointDescriptor( PeiServices,
            UsbIoPpi, EpIndx, &EndpointDescriptor );
        if (EFI_ERROR(Status)) {
            return Status;
        }

        // See if it is Interrupt endpoint and it is IN, not OUT
        if (((EndpointDescriptor->Attributes & EP_DESC_FLAG_TYPE_BITS) == EP_DESC_FLAG_TYPE_INT) 
            && (EndpointDescriptor->EndpointAddress & EP_DESC_ADDR_DIR_BIT)) {
            PeiUsbDev->IntEndpoint = EndpointDescriptor->EndpointAddress;
            PeiUsbDev->IntMaxPkt = EndpointDescriptor->MaxPacketSize;
            break;
        }
    }

    ASSERT(EpIndx < MAX_ENDPOINT);

    if (EpIndx == MAX_ENDPOINT) {
        return EFI_NOT_FOUND;   // Interrupt endpoint is not found
    }

    // Send the HID SET_IDLE request w/ wValue = 0 (Indefinite)
    PeiUsbIoControlTransfer(PeiServices, UsbIoPpi,
            HID_RQ_SET_IDLE, 0, 0, 0, 0, &UsbStatus);

    // Send the set protocol command w/ wValue = 0 (Boot protocol)
    PeiUsbIoControlTransfer(PeiServices, UsbIoPpi,
            HID_RQ_SET_PROTOCOL, 0, 0, 0, 0, &UsbStatus);

    PeiUsbLibLightenKbLeds(PeiServices, UsbIoPpi);

    // Attach periodic schedule
    PeiUsbDev->UsbHcPpi->ActivatePolling(PeiServices, (VOID*)PeiUsbDev);

    // Install input PPI, only once
    if (UsbKeyboardPpiInstalled == FALSE) {
        UsbKeyboardInputPpiList.Ppi = &UsbKeyboardInputPpi;
        Status = (**PeiServices).InstallPpi(PeiServices, &UsbKeyboardInputPpiList);
    
        ASSERT(Status == EFI_SUCCESS);
        if (!EFI_ERROR(Status)) {
            UsbKeyboardPpiInstalled = TRUE;
        }
    }

    for (Count = 0; Count < PEI_USB_RECOVERYREQUEST_RETRIES; Count++) {
        // See if recovery request is active
        Status = PeiUsbReadKey(PeiServices, &UsbKeyboardInputPpi, &KeyData);
    
        if (!EFI_ERROR(Status)) {
            DEBUG((DEBUG_INFO, " key: sc %x, uc %x, shft %x, tggl %x, efi %x, ps2 %x, Count %d\n",
                KeyData.Key.ScanCode, KeyData.Key.UnicodeChar,
                KeyData.KeyState.KeyShiftState, KeyData.KeyState.KeyToggleState,
                KeyData.EfiKey, KeyData.PS2ScanCode, Count));
        }

        if (IsUsbKbdRecovery(&KeyData)) {
            static EFI_PEI_PPI_DESCRIPTOR RecoveryModePpi = {
          	    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
                &gEfiPeiBootInRecoveryModePpiGuid, NULL
            };
    
            (*PeiServices)->SetBootMode(PeiServices, BOOT_IN_RECOVERY_MODE);
            (*PeiServices)->InstallPpi(PeiServices, &RecoveryModePpi);

            break;
        }
    }

    return EFI_SUCCESS;
}

#endif

/**
    This routine initializes usb mass devices.

    @param PeiServices 
    @param NotifyDesc 
    @param InvokePpi 
    
**/

EFI_STATUS
EFIAPI
InitializeUsbMassDevice(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *InvokePpi
)
{
    EFI_STATUS                  Status;
    EFI_PEI_PPI_DESCRIPTOR      *TempPpiDescriptor;
    PEI_USB_IO_PPI              *UsbIoPpi;
    UINTN                       PpiInstance;
    EFI_PEI_STALL_PPI           *StallPpi;
    UINT8                       DevConnectTimeout = PEI_USB_DEVICE_CONNECT_TIMEOUT * 2 + 1;
    PEI_USB_DEVICE              *PeiUsbDev;
    PEI_USB_HOST_CONTROLLER_PPI *UsbHcPpi;

    if (gUsbKeybaordBootNotifyFlag == TRUE) {
        return EFI_SUCCESS;
    }

    gUsbKeybaordBootNotifyFlag = TRUE;
    
    Status = (**PeiServices).LocatePpi( PeiServices, &gEfiPeiStallPpiGuid,
                    0, NULL, &StallPpi);

    if (EFI_ERROR(Status)) {
        return Status;
    }

    //Some devices may connect lately, polling process to wait for usb devices.
    for (;;) {
        PeimInitializeUsb(NULL, PeiServices); 
        for (PpiInstance = 0; PpiInstance < PEI_FAT_MAX_USB_IO_PPI; PpiInstance++) {
            Status = (**PeiServices).LocatePpi( PeiServices,
                                &gAmiPeiUsbIoPpiGuid,
                                PpiInstance,
                                &TempPpiDescriptor,
                                &UsbIoPpi);
            if (EFI_ERROR(Status)) {
                break;
            }
            
            PeiUsbDev = PEI_USB_DEVICE_FROM_THIS(UsbIoPpi);
            UsbHcPpi = PeiUsbDev->UsbHcPpi;
            
            if (PeiUsbDev->InterfaceDesc->InterfaceClass == BASE_CLASS_HUB) {
                PeiHubEnumeration(PeiServices, PeiUsbDev, 
                                 &UsbHcPpi->CurrentAddress, TRUE);
            }
        }
        
        DevConnectTimeout--;
        
        if (DevConnectTimeout == 0) {
            break;
        }
        
        StallPpi->Stall(PeiServices, StallPpi, 500 * 1000);
    }
    
    for (PpiInstance = 0; PpiInstance < PEI_MAX_USB_IO_PPI; PpiInstance++) {
        Status = (**PeiServices).LocatePpi(PeiServices,
            &gAmiPeiUsbIoPpiGuid,
            PpiInstance,
            &TempPpiDescriptor,
            &UsbIoPpi);

        if (EFI_ERROR(Status)) {
            break;
        }
        InitUsbBot(PeiServices, UsbIoPpi);
    }

    return EFI_SUCCESS;
}

/**
    This routine initializes usb host controllers and devices.

    @param PeiServices 
    @param NotifyDesc 
    @param InvokePpi 
    
**/

EFI_STATUS
EFIAPI
InitializeUsbHostDevice(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *InvokePpi
)
{
    EFI_STATUS                  Status;
    UINTN                       PpiInstance;
    PEI_USB_CHIP_INIT_PPI       *UsbChipsetRecoveryInitPpi;
    EFI_PEI_STALL_PPI           *StallPpi = NULL;
    PEI_USB_IO_PPI              *UsbIoPpi = NULL;
    PEI_USB_DEVICE              *PeiUsbDev = NULL;
    EFI_PEI_PPI_DESCRIPTOR      *TempPpiDescriptor = NULL;
    PEI_USB_HOST_CONTROLLER_PPI *UsbHcPpi = NULL;
#if USB_PEI_KEYBOARD_SUPPORT 
    UINT32                      KbConnectDelay = PEI_USB_KEYBOARD_CONNECT_DELAY_MS;
#endif

    if (gUsbBootInRecoveryNotifyFlag == TRUE) {
        return EFI_SUCCESS;
    }

    gUsbBootInRecoveryNotifyFlag = TRUE;

    for (PpiInstance = 0; PpiInstance < PEI_MAX_USB_RECOVERY_INIT_PPI; PpiInstance++) {
        Status = (**PeiServices).LocatePpi( PeiServices, &gPeiChipUsbRecoveryInitPpiGuid,
            PpiInstance, NULL, &UsbChipsetRecoveryInitPpi);
        if (EFI_ERROR(Status)) {
            break;
        }

        UsbChipsetRecoveryInitPpi->EnableChipUsbRecovery(PeiServices);
    }	

#if EHCI_PEI_SUPPORT
    EhciPeiUsbEntryPoint(NULL, PeiServices);
#endif

#if UHCI_PEI_SUPPORT
    UhciPeiUsbEntryPoint(NULL, PeiServices);
    UhcPeimEntry(NULL, PeiServices );
#endif

#if OHCI_PEI_SUPPORT
    OhciPeiUsbEntryPoint(NULL, PeiServices);
#endif

#if XHCI_PEI_SUPPORT
    XhciPeiUsbEntryPoint(NULL, PeiServices);
#endif
    
#if USB_PEI_KEYBOARD_SUPPORT     
    if (KbConnectDelay != 0) {
        Status = (**PeiServices).LocatePpi( PeiServices, &gEfiPeiStallPpiGuid,
                        0, NULL, &StallPpi);
 	  if (!EFI_ERROR(Status)) {
		  StallPpi->Stall(PeiServices, StallPpi, KbConnectDelay * 1000);
	  }
    }
#endif
    
    PeimInitializeUsb(NULL, PeiServices);
    
#if USB_PEI_KEYBOARD_SUPPORT  
    // Initialize head and tail of the input buffer
    gKeysHead = gKeys;
    gKeysTail = gKeys;
    PeimInitializeUsb(NULL, PeiServices); 
    for (PpiInstance = 0; PpiInstance < PEI_FAT_MAX_USB_IO_PPI; PpiInstance++) {
        Status = (**PeiServices).LocatePpi( PeiServices,
                            &gAmiPeiUsbIoPpiGuid,
                            PpiInstance,
                            &TempPpiDescriptor,
                            &UsbIoPpi);
        if (EFI_ERROR(Status)) {
            break;
        }
            
        PeiUsbDev = PEI_USB_DEVICE_FROM_THIS(UsbIoPpi);
        UsbHcPpi = PeiUsbDev->UsbHcPpi;
            
        if (PeiUsbDev->InterfaceDesc->InterfaceClass == BASE_CLASS_HUB) {
            PeiHubEnumeration(PeiServices, PeiUsbDev, 
                                 &UsbHcPpi->CurrentAddress, TRUE);
        }
    }
#endif
    
#if !USB_PEI_KEYBOARD_SUPPORT
    InitializeUsbMassDevice(PeiServices, NULL, NULL);
#endif

	(**PeiServices).NotifyPpi(PeiServices, &gUsbPeiNotifyList);
	
    return EFI_SUCCESS;
}

/**
    This routine halts all available host controllers at end of PEI

    @param PeiServices 
    @param NotifyDesc 
    @param InvokePpi 

         
    @retval EFI_STATUS this function returns EFI_SUCCESS if the
        host controller is reset successfully. Otherwise, returns
        any type of error it encountered during the reset operation.
              
**/

EFI_STATUS
EFIAPI
NotifyOnRecoveryCapsuleLoaded(
    IN EFI_PEI_SERVICES          **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
    IN VOID                      *InvokePpi
)
{
    EFI_STATUS                      Status;
    UINT8                           ControllerIndex = 0;
    PEI_USB_HOST_CONTROLLER_PPI     *UsbHcPpi;
	UINT8				 			NumOfRootPort;
	UINT8							PortNum;

    while (TRUE) {
        
        Status = (**PeiServices).LocatePpi (
                    PeiServices,
                    &gAmiPeiUsbHostControllerPpiGuid,
                    ControllerIndex++,
                    NULL,
                    &UsbHcPpi
                    );

        if (EFI_ERROR (Status)) break;

		UsbHcPpi->GetRootHubPortNumber(
	  			PeiServices,
	  			UsbHcPpi,
	  			&NumOfRootPort
  				);
		for (PortNum = 1; PortNum <= NumOfRootPort; PortNum++) {
			UsbHcPpi->ClearRootHubPortFeature(
							PeiServices,
							UsbHcPpi,
							PortNum,
							EfiUsbPortEnable
							);
		}

        if (UsbHcPpi->Reset != NULL ){
            Status = UsbHcPpi->Reset ( PeiServices, 
										UsbHcPpi, 
										EFI_USB_HC_RESET_GLOBAL );
        }
    }    

    return EFI_SUCCESS;
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
