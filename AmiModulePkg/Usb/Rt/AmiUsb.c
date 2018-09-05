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

/** @file AmiUsb.c
    AMI USB API implementation. The following code will be
    copied to SMM; only RT functions can be used. gUsbData
    is obtained from AMIUHCD in the entry point and can be
    used afterwards.

**/

#include "AmiDef.h"
#include "AmiUsb.h"
#include <UsbDevDriverElinks.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#if !USB_RT_DXE_DRIVER
#include <Library/AmiBufferValidationLib.h>
#include <Library/AmiUsbSmmGlobalDataValidationLib.h>
#endif

#if USB_DEV_KBD
#include "UsbKbd.h"
#endif
#include "UsbMass.h"
#include <Protocol/AmiUsbHid.h>

//#pragma warning (disable: 4152)

EFI_EMUL6064MSINPUT_PROTOCOL* gMsInput = 0;
EFI_EMUL6064KBDINPUT_PROTOCOL* gKbdInput = 0;
EFI_EMUL6064TRAP_PROTOCOL* gEmulationTrap = 0;

USB_GLOBAL_DATA     *gUsbData;
//USB_BADDEV_STRUC    *gUsbBadDeviceTable;			//(EIP60706-)

AMI_USB_SMM_PROTOCOL	gUsbSmmProtocol = {0};

BOOLEAN gLockSmiHandler = FALSE;
BOOLEAN gLockHwSmiHandler = FALSE;
BOOLEAN gCheckUsbApiParameter = FALSE;

VOID    StopControllerType(UINT8);
VOID    StartControllerType(UINT8);
UINT8   USB_StopDevice (HC_STRUC*,  UINT8, UINT8);
UINT8   USB_EnumerateRootHubPorts(UINT8);   //(EIP57521+)              
VOID    StopControllerBdf(UINT16);			//(EIP74876+)

VOID	FillHcdEntries();
										//(EIP71750+)>
typedef VOID USB_DEV_DELAYED_DRIVER_CHECK (DEV_DRIVER*);
extern USB_DEV_DELAYED_DRIVER_CHECK USB_DEV_DELAYED_DRIVER EndOfUsbDevDelayedDriverList;
USB_DEV_DELAYED_DRIVER_CHECK* UsbDevDelayedDrivers[]= {USB_DEV_DELAYED_DRIVER NULL};

typedef VOID USB_DEV_DRIVER_CHECK (DEV_DRIVER*);
extern USB_DEV_DRIVER_CHECK USB_DEV_DRIVER EndOfUsbDevDriverList;
USB_DEV_DRIVER_CHECK* UsbDevDrivers[]= {USB_DEV_DRIVER NULL};
										//<(EIP71750+)

extern	UINT8	UHCI_FillHCDEntries(HCD_HEADER*);
extern	UINT8	OHCI_FillHCDEntries(HCD_HEADER*);
extern	UINT8	EHCI_FillHCDEntries(HCD_HEADER*);
extern	UINT8	XHCI_FillHCDEntries(HCD_HEADER*);

void FillHcdEntries()
{
#if UHCI_SUPPORT
	UHCI_FillHCDEntries (&gUsbData->aHCDriverTable[USB_INDEX_UHCI]);
#endif
#if OHCI_SUPPORT
	OHCI_FillHCDEntries (&gUsbData->aHCDriverTable[USB_INDEX_OHCI]);
#endif
#if EHCI_SUPPORT
	EHCI_FillHCDEntries (&gUsbData->aHCDriverTable[USB_INDEX_EHCI]);
#endif
#if XHCI_SUPPORT
	XHCI_FillHCDEntries (&gUsbData->aHCDriverTable[USB_INDEX_XHCI]);
#endif
}

/**
 Type:        Function Dispatch Table

    This is the table of functions used by USB API

    @note  This functions are invoked via software SMI

**/

API_FUNC aUsbApiTable[] = {
    USBAPI_CheckPresence,
    USBAPI_Start,
    USBAPI_Stop,
    USBAPI_DisableInterrupts,
    USBAPI_EnableInterrupts,
    USBAPI_MoveDataArea,
    USBAPI_GetDeviceInfo,
    USBAPI_CheckDevicePresence,
    USBAPI_MassDeviceRequest,
    USBAPI_PowerManageUSB,
    USBAPI_PrepareForOS,
    USBAPI_SecureInterface,
    USBAPI_LightenKeyboardLEDs,
    USBAPI_ChangeOwner,
    USBAPI_HC_Proc,
    USBAPI_Core_Proc,
    USBAPI_LightenKeyboardLEDs_Compatible,
    USBAPI_KbcAccessControl,
    USBAPI_LegacyControl,
    USBAPI_GetDeviceAddress,
    USBAPI_ExtDriverRequest,
    USBAPI_CCIDRequest,
    USBAPI_UsbStopController,				//(EIP74876+)
    USBAPI_HcStartStop
};

/**
 Type:        Function Dispatch Table

    This is the table of functions used by USB Mass Storage API

**/

API_FUNC aUsbMassApiTable[] = {
    USBMassAPIGetDeviceInformation, // USB Mass API Sub-Func 00h
    USBMassAPIGetDeviceGeometry,    // USB Mass API Sub-Func 01h
    USBMassAPIResetDevice,          // USB Mass API Sub-Func 02h
    USBMassAPIReadDevice,           // USB Mass API Sub-Func 03h
    USBMassAPIWriteDevice,          // USB Mass API Sub-Func 04h
    USBMassAPIVerifyDevice,         // USB Mass API Sub-Func 05h
    USBMassAPIFormatDevice,         // USB Mass API Sub-Func 06h
    USBMassAPICommandPassThru,      // USB Mass API Sub-Func 07h
    USBMassAPIAssignDriveNumber,    // USB BIOS API function 08h
    USBMassAPICheckDevStatus,       // USB BIOS API function 09h
    USBMassAPIGetDevStatus,         // USB BIOS API function 0Ah
    USBMassAPIGetDeviceParameters,  // USB BIOS API function 0Bh
    USBMassAPIEfiReadDevice,        // USB Mass API Sub-Func 0Ch
    USBMassAPIEfiWriteDevice,       // USB Mass API Sub-Func 0Dh
    USBMassAPIEfiVerifyDevice       // USB Mass API Sub-Func 0Eh
};

EFI_DRIVER_ENTRY_POINT(USBDriverEntryPoint)

/**
    USB Driver entry point

**/

EFI_STATUS
EFIAPI
USBDriverEntryPoint(
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS Status;

    InitAmiLib(ImageHandle, SystemTable);

#if !USB_RT_DXE_DRIVER
    Status = InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
#endif
    
#if USB_RT_DXE_DRIVER
    Status = InstallUsbProtocols(); 
    InitializeUsbGlobalData();
#endif
	
    ASSERT_EFI_ERROR(Status);

    return Status;
}

/**
    This function initializes the USB global data.

**/

EFI_STATUS
InitializeUsbGlobalData(
	VOID
)
{
	EFI_STATUS	Status;
    UINTN   	DriverIndex;
    UINTN   	DelayedIndex;
    UINTN       Index;
    UINTN       MaxIndex;
    EFI_PHYSICAL_ADDRESS    MemAddress;

    //
    // Initialize host controller drivers
    //
    FillHcdEntries();

    //
    // Initialize the device driver pointers
    //
#if !USB_RT_DXE_DRIVER
    DriverIndex = 0;
    DelayedIndex = 0;
    MaxIndex = MAX_DEVICE_TYPES;
#else
    DriverIndex = MAX_DEVICE_TYPES;
    DelayedIndex = MAX_DEVICE_TYPES;
    MaxIndex = MAX_DEVICE_TYPES * 2;
#endif

    Index = 0;

    while (UsbDevDelayedDrivers[Index]) {
        if (DelayedIndex == MaxIndex) {
            break;
        }
        UsbDevDelayedDrivers[Index](&gUsbData->aDelayedDrivers[DelayedIndex]);
		if (gUsbData->aDelayedDrivers[DelayedIndex].pfnDeviceInit) {
            (*gUsbData->aDelayedDrivers[DelayedIndex].pfnDeviceInit)();
        }
        if (gUsbData->aDelayedDrivers[DelayedIndex].bDevType) {
            Index++;
            DelayedIndex++;
        }
    }

    ASSERT(DelayedIndex != MaxIndex);

    Index = 0;

    while (UsbDevDrivers[Index]) {
        if (DriverIndex == MaxIndex) {
            break;
        }
        UsbDevDrivers[Index](&gUsbData->aDevDriverTable[DriverIndex]);
		if (gUsbData->aDevDriverTable[DriverIndex].pfnDeviceInit) {
            (*gUsbData->aDevDriverTable[DriverIndex].pfnDeviceInit)();
        }
        if (gUsbData->aDevDriverTable[DriverIndex].bDevType) {
            Index++;
            DriverIndex++;
        }
    }

    ASSERT(DriverIndex != MaxIndex);

	//
	// Allocate a block of memory to be used as a temporary
	// buffer for  USB mass transfer
	//
	if (gUsbData->fpUSBMassConsumeBuffer == NULL) {
    	MemAddress = 0xFFFFFFFF;
    	Status = gBS->AllocatePages(AllocateMaxAddress, EfiRuntimeServicesData,
    					EFI_SIZE_TO_PAGES(MAX_CONSUME_BUFFER_SIZE), 
    					&MemAddress);

        if (EFI_ERROR(Status)) {
    	    Status = gBS->AllocatePages(AllocateAnyPages, EfiRuntimeServicesData,
    					    EFI_SIZE_TO_PAGES(MAX_CONSUME_BUFFER_SIZE), 
    					    &MemAddress);
        }

    	ASSERT_EFI_ERROR(Status);
        gUsbData->fpUSBMassConsumeBuffer = (VOID*)(UINTN)MemAddress;
    	pBS->SetMem(gUsbData->fpUSBMassConsumeBuffer, MAX_CONSUME_BUFFER_SIZE, 0);
    }

	//
	// Allocate a block of memory for the temporary buffer
	//
	if (gUsbData->fpUSBTempBuffer == NULL) {
        MemAddress = 0xFFFFFFFF;
    	Status = gBS->AllocatePages(AllocateMaxAddress, EfiRuntimeServicesData,
    					EFI_SIZE_TO_PAGES(MAX_TEMP_BUFFER_SIZE), 
    					&MemAddress);
        if (EFI_ERROR(Status)) {
        	Status = gBS->AllocatePages(AllocateAnyPages, EfiRuntimeServicesData,
        					EFI_SIZE_TO_PAGES(MAX_TEMP_BUFFER_SIZE), 
        					&MemAddress);
        }

    	ASSERT_EFI_ERROR(Status);
        gUsbData->fpUSBTempBuffer = (VOID*)(UINTN)MemAddress;
    	pBS->SetMem(gUsbData->fpUSBTempBuffer, MAX_TEMP_BUFFER_SIZE, 0);
    }

    //
    // Allow to enumerate ports
    //
    gUsbData->bEnumFlag = FALSE;

	return USB_SUCCESS;
}


/**

**/

VOID
EFIAPI
UsbApiHandler(
    VOID* Param
)
{
    URP_STRUC   *UsbUrp = (URP_STRUC*)Param;
    UINT8       FuncIndex;
    UINT8       NumberOfFunctions;

	if (UsbUrp == NULL) {
		return;
	}

    FuncIndex = UsbUrp->bFuncNumber;
    NumberOfFunctions = sizeof(aUsbApiTable) / sizeof (API_FUNC *);

    //
    // Make sure function number is valid; if function number is not zero
    // check for valid extended USB API function
    //
    if (FuncIndex && ((FuncIndex < USB_NEW_API_START_FUNC ) ||
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
            FuncIndex >= (NumberOfFunctions + USB_NEW_API_START_FUNC - 1))) { // aUsbApiTable 0-based
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
        USB_DEBUG(DEBUG_ERROR, 3, "UsbApiHandler Invalid function#%x\n", FuncIndex);
        return;
    }

    if (FuncIndex) {
        FuncIndex = (UINT8)(FuncIndex - USB_NEW_API_START_FUNC + 1);
    }

    aUsbApiTable[FuncIndex](UsbUrp);    // Call the appropriate function

}

/**
    This function initializes the USB global data.

**/

EFI_STATUS
InstallUsbProtocols(
	VOID
)
{
	EFI_STATUS	Status;
	EFI_USB_PROTOCOL	*UsbProtocol;

    Status = pBS->LocateProtocol(&gEfiUsbProtocolGuid, NULL, &UsbProtocol);
	if (EFI_ERROR(Status)) {
		return Status;
	}

    gUsbData = UsbProtocol->USBDataPtr;

#if !USB_RT_DXE_DRIVER
	UsbProtocol->UsbRtKbcAccessControl = UsbKbcAccessControl;
	//Hook USB legacy control function for shutdown/init USB legacy support
	UsbProtocol->UsbLegacyControl = USBRT_LegacyControl;
	UsbProtocol->UsbStopUnsupportedHc = USB_StopUnsupportedHc;
#else
    if (USB_RUNTIME_DRIVER_IN_SMM == 0) {
    	UsbProtocol->UsbRtKbcAccessControl = UsbKbcAccessControl;
    	//Hook USB legacy control function for shutdown/init USB legacy support
    	UsbProtocol->UsbLegacyControl = USBRT_LegacyControl;
    	UsbProtocol->UsbStopUnsupportedHc = USB_StopUnsupportedHc;
    }
	UsbProtocol->UsbInvokeApi = UsbApiHandler;
#endif

	return Status;
}

#if !USB_RT_DXE_DRIVER
/**
    SMM entry point of AMIUSB driver

**/

EFI_STATUS
InSmmFunction(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS                      Status;
	EFI_HANDLE                      SwSmiHandle = NULL;
    EFI_SMM_SW_REGISTER_CONTEXT     SwSmiContext;
	EFI_SMM_SW_DISPATCH2_PROTOCOL    *SwSmiDispatch;
    UINT32                          KbcEmulFeature = 0;
    VOID	                        *ProtocolNotifyRegistration;
    EFI_EVENT                       Emul6064Event = NULL;
	EFI_HANDLE	UsbSmmProtocolHandle = NULL;

	Status = InitAmiSmmLib( ImageHandle, SystemTable );
	if (EFI_ERROR(Status)) return Status;

	InstallUsbProtocols();
	InitializeUsbGlobalData();

    Status = pSmst->SmmLocateProtocol(
                    &gEmul6064TrapProtocolGuid,
                    NULL,
                    &gEmulationTrap);

    if (EFI_ERROR(Status)) {
        Status = pSmst->SmmRegisterProtocolNotify(
                    &gEmul6064TrapProtocolGuid,
                    Emul6064TrapCallback,
                    &ProtocolNotifyRegistration
                    );
    }
    if (!gUsbData->kbc_support) {
        Status = pSmst->SmmLocateProtocol(&gEmul6064MsInputProtocolGuid, 
                                    NULL, &gMsInput);

        Status = pSmst->SmmLocateProtocol(&gEmul6064KbdInputProtocolGuid,
                                    NULL, &gKbdInput);

        if (Status == EFI_SUCCESS) {
            gUsbData->dUSBStateFlag |= USB_FLAG_6064EMULATION_ON;
            if (gEmulationTrap) {
                KbcEmulFeature = gEmulationTrap->FeatureSupported(gEmulationTrap);
            }
            if (KbcEmulFeature & IRQ_SUPPORTED) {
                gUsbData->dUSBStateFlag |= USB_FLAG_6064EMULATION_IRQ_SUPPORT;
            }
        } else {
            InitSysKbc( &gKbdInput, &gMsInput );
        }
    } else {
        //
        //Init Fake Emulation interface
        //
        InitSysKbc( &gKbdInput, &gMsInput );
    }
    
#if !USB_IRQ_SUPPORT
    Status = USBSB_InstallSmiEventHandlers();
#endif

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "AMIUSB global data at 0x%x\n", gUsbData);

    //
    // Register the USB SW SMI handler
    //
   Status = pSmst->SmmLocateProtocol(&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwSmiDispatch);

   if (EFI_ERROR (Status)) {
       USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_0, "SmmSwDispatch protocol: %r\n", Status);
       return Status;
   }

    SwSmiContext.SwSmiInputValue = USB_SWSMI;
    Status = SwSmiDispatch->Register(SwSmiDispatch, USBSWSMIHandler, &SwSmiContext, &SwSmiHandle);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "AMIUSB SW SMI registration:: %r\n", Status);

	gUsbSmmProtocol.UsbStopUnsupportedHc = USB_StopUnsupportedHc;
    gUsbSmmProtocol.UsbApiTable = aUsbApiTable;
    gUsbSmmProtocol.UsbMassApiTable = aUsbMassApiTable;
    gUsbSmmProtocol.GlobalDataValidation.ConstantDataCrc32 = 0;
    gUsbSmmProtocol.GlobalDataValidation.Crc32Hash = (UINT32)GetPerformanceCounter();
  
    Status = pSmst->SmmInstallProtocolInterface(
                    &UsbSmmProtocolHandle,
                    &gAmiUsbSmmProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gUsbSmmProtocol
                    );
	
	USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "AMIUSB SMM protocol: %r\n", Status);

    UpdateAmiUsbSmmGlobalDataCrc32(gUsbData);

    return Status;
}


/**
    Invoked on reads from SW SMI port with value USB_SWSMI. This
    function dispatches the USB Request Packets (URP) to the
    appropriate functions.

    @param EBDA:USB_DATA_EBDA_OFFSET - Pointer to the URP (USB Request
        Packet structure)
        DispatchHandle  - EFI Handle
        DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

        bRetValue   Zero on successfull completion
    @retval Non zero on error

**/
EFI_STATUS
EFIAPI
USBSWSMIHandler (
    EFI_HANDLE	DispatchHandle,
    CONST VOID	*Context OPTIONAL,
    VOID		*CommBuffer OPTIONAL,
    UINTN	    *CommBufferSize OPTIONAL
)
{
    URP_STRUC   *UsbUrp;
    UINT16      EbdaSeg;
    EFI_STATUS  Status;

    if (gLockSmiHandler == TRUE) {
        return EFI_SUCCESS;
    }

    Status = AmiUsbSmmGlobalDataValidation(gUsbData);

    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) {
        gLockHwSmiHandler = TRUE;
        gLockSmiHandler = TRUE;
        return EFI_SUCCESS;
    }

	if (gUsbData->fpURP) {			// Call from AMIUSB C area
        Status = AmiValidateMemoryBuffer((VOID*)gUsbData->fpURP, sizeof(URP_STRUC));
        if (EFI_ERROR(Status)) {
            USB_DEBUG(DEBUG_ERROR, 3, "UsbApiHandler Invalid Pointer, the address is in SMRAM.\n");
            return EFI_ACCESS_DENIED;
        }
		UsbUrp = gUsbData->fpURP;
		gUsbData->fpURP = 0;   		// Clear the switch
	} else {
	    if (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_OS) {
            return EFI_SUCCESS;
        }
        //
        // Get the UsbUrp pointer from EBDA
        //
        EbdaSeg = *((UINT16*)0x40E);
        UsbUrp = *(URP_STRUC**)(UINTN)(((UINT32)EbdaSeg << 4) + USB_DATA_EBDA_OFFSET);
        UsbUrp = (URP_STRUC*)((UINTN)UsbUrp & 0xFFFFFFFF);
        Status = AmiValidateMemoryBuffer((VOID*)UsbUrp, sizeof(URP_STRUC));
        if (EFI_ERROR(Status)) {
            USB_DEBUG(DEBUG_ERROR, 3, "UsbApiHandler Invalid Pointer, the address is in SMRAM.\n");
            return EFI_SUCCESS;
        }
	}

	if (UsbUrp == NULL) {
        return EFI_SUCCESS;
	}

	gCheckUsbApiParameter = TRUE;

	UsbApiHandler(UsbUrp);

	gCheckUsbApiParameter = FALSE;

    return EFI_SUCCESS;
}

/**
    USB Hardware SMI handler.

    @param Host controller type.

**/

VOID
UsbHwSmiHandler (UINT8 HcType)
{
    UINT8       Index;
    HC_STRUC    *HcStruc;
    EFI_STATUS  Status;

    if (gLockHwSmiHandler == TRUE) {
        return;
    }

    Status = AmiUsbSmmGlobalDataValidation(gUsbData);

    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) {
        gLockHwSmiHandler = TRUE;
        gLockSmiHandler = TRUE;
        return;
    }

    for (Index = 0; Index < gUsbData->HcTableCount; Index++) {
        HcStruc = gUsbData->HcTable[Index];
        if (HcStruc == NULL) {
            continue;
        }
        if (!(HcStruc->dHCFlag & HC_STATE_USED)) {
        	continue;
        }
        if (HcStruc->bHCType == HcType) { // Process appropriate interrupt
            (*gUsbData->aHCDriverTable
				[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDProcessInterrupt)(HcStruc);
        }
    }

}

/**
    USB Hardware SMI handler.

    @param DispatchHandle  - EFI Handle
        DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

    @retval EFI_SUCCESS

**/

EFI_STATUS
EFIAPI
UhciHWSMIHandler (
	EFI_HANDLE	DispatchHandle,
	CONST VOID	*Context,
	VOID		*CommBuffer,
	UINTN		*CommBufferSize
)
{
    UsbHwSmiHandler(USB_HC_UHCI);
    return EFI_SUCCESS;
}

/**
    USB Hardware SMI handler.

    @param DispatchHandle  - EFI Handle
        DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

    @retval EFI_SUCCESS

**/

EFI_STATUS
EFIAPI
OhciHWSMIHandler (
    EFI_HANDLE	DispatchHandle,
    CONST VOID	*Context,
    VOID		*CommBuffer,
    UINTN		*CommBufferSize
)
{
    UsbHwSmiHandler(USB_HC_OHCI);
	return EFI_SUCCESS;
}

/**
    USB Hardware SMI handler.

    @param DispatchHandle  - EFI Handle
        DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

    @retval EFI_SUCCESS

**/

EFI_STATUS
EFIAPI
EhciHWSMIHandler (
    EFI_HANDLE	DispatchHandle,
    CONST VOID	*Context,
    VOID		*CommBuffer,
    UINTN		*CommBufferSize
)
{
    UsbHwSmiHandler(USB_HC_EHCI);
    return EFI_SUCCESS;
}

/**
    USB Hardware SMI handler.

    @param DispatchHandle  - EFI Handle
        DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

    @retval EFI_SUCCESS

**/

EFI_STATUS
EFIAPI
XhciHwSmiHandler (
    EFI_HANDLE	DispatchHandle,
    CONST VOID	*Context,
    VOID		*CommBuffer,
    UINTN		*CommBufferSize
)
{
    UsbHwSmiHandler(USB_HC_XHCI);
    return EFI_SUCCESS;
}

/**
    This function registers USB hardware SMI callback function.

**/

EFI_STATUS
UsbInstallHwSmiHandler(
	HC_STRUC    *HcStruc
)
{
	EFI_STATUS                      Status;
	EFI_SMM_USB_REGISTER_CONTEXT    UsbContext;
    EFI_SMM_USB_DISPATCH2_PROTOCOL  *UsbDispatch;
    EFI_SMM_HANDLER_ENTRY_POINT2	UsbCallback;
    EFI_HANDLE                      Handle = NULL;

    if (HcStruc->HwSmiHandle != NULL) {
        return EFI_SUCCESS;
    }
	

    Status = pSmst->SmmLocateProtocol(
            &gEfiSmmUsbDispatch2ProtocolGuid,
            NULL,
            &UsbDispatch);

    ASSERT_EFI_ERROR(Status);
    
	if (EFI_ERROR(Status)) {
		return Status;
	}

	switch (HcStruc->bHCType) {
		case USB_HC_UHCI:
			UsbCallback = UhciHWSMIHandler;
			break;

		case USB_HC_OHCI:
			UsbCallback = OhciHWSMIHandler;
			break;

		case USB_HC_EHCI:
			UsbCallback = EhciHWSMIHandler;
			break;

		case USB_HC_XHCI:
			UsbCallback = XhciHwSmiHandler;
			break;

		default:
			return EFI_UNSUPPORTED;
	}

    UsbContext.Type = UsbLegacy;
    UsbContext.Device = HcStruc->pHCdp;

    Status = UsbDispatch->Register(
                UsbDispatch,
                UsbCallback,
                &UsbContext,
                &Handle);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "AMIUSB HC type %x HW SMI registation status:: %r\n", HcStruc->bHCType, Status);
    
	if (!EFI_ERROR(Status)) {
		HcStruc->HwSmiHandle = Handle;
	}
    
	return Status;
}

/**
    Update the KbcEmul feature when the Emul6064Trap Protocol becomes available.

**/
EFI_STATUS
EFIAPI
Emul6064TrapCallback (
    CONST EFI_GUID  *Protocol,
    VOID            *Interface,
    EFI_HANDLE      Handle
)
{
    EFI_STATUS                      Status;
    UINT32                          KbcEmulFeature = 0;
    
    Status = pSmst->SmmLocateProtocol(
                    &gEmul6064TrapProtocolGuid,
                    NULL,
                    &gEmulationTrap);
    if (!gUsbData->kbc_support) {
        Status = pSmst->SmmLocateProtocol(&gEmul6064MsInputProtocolGuid,
                                        NULL, &gMsInput);

        Status = pSmst->SmmLocateProtocol(&gEmul6064KbdInputProtocolGuid,
                                        NULL, &gKbdInput);

        if (Status == EFI_SUCCESS) {
            gUsbData->dUSBStateFlag |= USB_FLAG_6064EMULATION_ON;
            if (gEmulationTrap) {
                KbcEmulFeature = gEmulationTrap->FeatureSupported(gEmulationTrap);
            }
            if (KbcEmulFeature & IRQ_SUPPORTED) {
                gUsbData->dUSBStateFlag |= USB_FLAG_6064EMULATION_IRQ_SUPPORT;
            }
        } else {
            InitSysKbc( &gKbdInput, &gMsInput );
        }
    } else {
        //
        //Init Fake Emulation interface
        //
        InitSysKbc( &gKbdInput, &gMsInput );
    }
    
    return EFI_SUCCESS;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
// USB API Functions
//
//////////////////////////////////////////////////////////////////////////////

/**
    This routine services the USB API function number 27h.  It
    handles all the mass storage related calls from the higher
    layer. Different sub-functions are invoked depending on
    the sub-function number

    @param fpURPPointer    Pointer to the URP structure
        fpURPPointer.bSubFunc   Subfunction number
        00  Get Device Information
        01  Get Device Parameter
        02  Reset Device
        03  Read Device
        04  Write Device
        05  Verify Device
        06  Format Device
        07  Request Sense
        08  Test Unit Ready
        09  Start Stop Unit
        0A  Read Capacity
        0B  Mode Sense
        0C  Device Inquiry
        0D  Send Command
        0E  Assign drive number

    @retval URP structure is updated with the relevant information


**/

VOID
USBAPI_MassDeviceRequest(
    URP_STRUC *UsbUrp
)
{
    UINT8 MassFuncIndex = UsbUrp->bSubFunc;
    UINT8 NumberOfMassFunctions = sizeof(aUsbMassApiTable) / sizeof(API_FUNC *);

    //
    // Make sure function number is valid
    //
    if (MassFuncIndex >= NumberOfMassFunctions) {
        USB_DEBUG(DEBUG_ERROR, 3, "UsbApi MassDeviceRequet Invalid function#%x\n", MassFuncIndex);
        //UsbUrp->bRetValue = USBAPI_INVALID_FUNCTION;
        return;
    }
    gUsbData->bUSBKBC_MassStorage = 01;
    //
    // Function number is valid - call it
    //
    aUsbMassApiTable[MassFuncIndex](UsbUrp);
    gUsbData->bUSBKBC_MassStorage = 00;
}


/**
    This routine services the USB API function number 0.  It
    reports the USB BIOS presence, its version number and
    its current status information

    @param fpURPPointer - Pointer to the URP structure

    @retval URP structure is updated with the following information
        CkPresence.wBiosRev       USB BIOS revision (0210h means r2.10)
        CkPresence.bBiosActive    0 - if USB BIOS is not running
        CkPresence.bNumBootDev    Number of USB boot devices found
        CkPresence.bNumHC         Number of host controller present
        CkPresence.bNumPorts      Number of root hub ports
        CkPresence.dUsbDataArea   Current USB data area

**/

VOID
USBAPI_CheckPresence (URP_STRUC *fpURP)
{
    fpURP->bRetValue                        = USB_SUCCESS;
    fpURP->ApiData.CkPresence.bBiosActive   = 0;

    fpURP->ApiData.CkPresence.bNumBootDev   = 0;    // Number of USB boot devices found
    fpURP->ApiData.CkPresence.bNumKeyboards = 0;    // Number of USB keyboards present
    fpURP->ApiData.CkPresence.bNumMice      = 0;    // Number of USB mice present
    fpURP->ApiData.CkPresence.bNumPoint		= 0;    // Number of USB Point present    //(EIP38434+)
    fpURP->ApiData.CkPresence.bNumHubs      = 0;    // Number of USB hubs present
    fpURP->ApiData.CkPresence.bNumStorage   = 0;    // Number of USB storage devices present

    fpURP->ApiData.CkPresence.wBiosRev = USB_DRIVER_MAJOR_VER;
    fpURP->ApiData.CkPresence.bBiosActive = USB_ACTIVE; // Set USB BIOS as active
    if (!(gUsbData->dUSBStateFlag & USB_FLAG_DISABLE_LEGACY_SUPPORT)) {
        fpURP->ApiData.CkPresence.bBiosActive |= USB_LEGACY_ENABLE;
    }
    if (gUsbData->dUSBStateFlag & USB_FLAG_6064EMULATION_ON) {
        fpURP->ApiData.CkPresence.bBiosActive |= USB_6064_ENABLE;
    }
    USBWrap_GetDeviceCount(fpURP);  // Get active USB devices
}


/**
    This API routine configures the USB host controllers and
    enumerate the devices

    @param fpURPPointer  URP structure with input parameters
        StartHc.wDataAreaFlag Indicates which data area to use

        StartHc.wDataAreaFlag Returns current data area pointer
    @retval bRetValue USB_SUCCESS on success, USB_ERROR on error.

**/

VOID USBAPI_Start (URP_STRUC *fpURP)
{
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBSMI: Start\n");
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "\tUSBAPI_HC_Proc:%x\n", &USBAPI_HC_Proc);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "\tUSBAPI_Core_Proc:%x\n", &USBAPI_Core_Proc);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "\tUSB_ReConfigDevice:%x\n", &USB_ReConfigDevice);
    fpURP->bRetValue = USB_StartHostControllers (gUsbData);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USB_StartHostControllers returns %d\n", fpURP->bRetValue);
}


/**
    This routine stops the USB host controllers

    @param fpURPPointer    Pointer to the URP structure

    @retval bRetValue   USB_SUCCESS on success
        USB_ERROR on error

**/

VOID USBAPI_Stop (URP_STRUC *fpURP)
{
    gCheckUsbApiParameter = FALSE;
    fpURP->bRetValue = USB_StopHostControllers (gUsbData);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USB_StopHostControllers returns %d\n", fpURP->bRetValue);
    gUsbData->dUSBStateFlag &= ~(USB_FLAG_DRIVER_STARTED);
}


/**
    This routine suspends the USB host controllers

    @param fpURPPointer    Pointer to the URP structure

    @retval VOID

**/

VOID USBAPI_PowerManageUSB (URP_STRUC *fpURP)
{
    fpURP->bRetValue = USB_NOT_SUPPORTED;
}


/**
    This routine updates data structures to reflect that
    POST is completed

    @param fpURPPointer    Pointer to the URP structure

    @retval VOID

**/

VOID USBAPI_PrepareForOS (URP_STRUC *fpURP)
{
    fpURP->bRetValue = USB_NOT_SUPPORTED;
}


/**
    This routine handles the calls related to security device

    @param fpURPPointer    Pointer to the URP structure

    @retval VOID

**/

VOID USBAPI_SecureInterface (URP_STRUC *fpURP)
{
    fpURP->bRetValue = USB_NOT_SUPPORTED;
}


/**
    This routine stops the USB host controllers interrupts

    @param fpURPPointer    Pointer to the URP structure

    @retval bRetValue   USB_SUCCESS on success
        USB_ERROR on error (Like data area not found)

**/

VOID USBAPI_DisableInterrupts (URP_STRUC *fpURP)
{
    fpURP->bRetValue = USB_NOT_SUPPORTED;
}


/**
    This routine re-enable the USB host controller interrupts

    @param fpURPPointer    Pointer to the URP structure

    @retval bRetValue   USB_SUCCESS on success
        USB_ERROR on error (Like data area not found)

**/

VOID USBAPI_EnableInterrupts (URP_STRUC *fpURP)
{
    fpURP->bRetValue = USB_NOT_SUPPORTED;
}


/**
    This routine stops the USB host controllers and moves
    the data area used by host controllers to a new area.
    The host controller is started from the new place.

    @param fpURPPointer    URP structure with input parameters
        StartHc.wDataAreaFlag   Indicates which data area to use

    @retval bRetValue   USB_SUCCESS

**/

VOID USBAPI_MoveDataArea(URP_STRUC *fpURP)
{
    fpURP->bRetValue = USB_NOT_SUPPORTED;
}


/**
    This routine returns the information regarding
    a USB device like keyboard, mouse, floppy drive etc

    @param fpURPPointer            URP structure with input parameters
        GetDevInfo.bDevNumber   Device number (1-based) whose
        information is requested

    @retval URP structure is updated with the following information
        GetDevInfo.bHCNumber - HC number in which the device is found
        GetDevInfo.bDevType  - Type of the device
        bRetValue will be one of the following value
        USB_SUCCESS         on successfull completion
        USB_PARAMETER_ERROR if bDevNumber is invalid
        USB_ERROR           on error

**/

VOID USBAPI_GetDeviceInfo (URP_STRUC *fpURP)
{
    DEV_INFO* fpDevInfo;

    //
    // Initialize the return values
    //
    fpURP->ApiData.GetDevInfo.bHCNumber = 0;
    fpURP->ApiData.GetDevInfo.bDevType  = 0;
    fpURP->bRetValue                    = USB_ERROR;

    //
    // Check for parameter validity
    //
    if ( !fpURP->ApiData.GetDevInfo.bDevNumber ) return;

    fpURP->bRetValue = USB_PARAMETER_ERROR;

    //
    // Get the device information structure for the 'n'th device
    //
    fpDevInfo = USBWrap_GetnthDeviceInfoStructure(fpURP->ApiData.GetDevInfo.bDevNumber);
//  if (!wRetCode) return;  // USB_PARAMETER_ERROR

    //
    // Return value
    //
    fpURP->ApiData.GetDevInfo.bDevType  = fpDevInfo->bDeviceType;
    fpURP->ApiData.GetDevInfo.bHCNumber = fpDevInfo->bHCNumber;
    fpURP->bRetValue                    = USB_SUCCESS;

}


/**
    This routine checks whether a particular type of USB device
    is installed in the system or not.

    @param fpURPPointer            URP structure with input parameters
        ChkDevPrsnc.bDevType    Device type to find
        ChkDevPrsnc.fpHCStruc   Pointer to HC being checked for device
        connection; if NULL then the total number of devices
        connected to ANY controller is returned.
        ChkDevPrsnc.bNumber     Number of devices connected

    @retval bRetValue will be one of the following value
        USB_SUCCESS     if device type present, ChkDevPrsnc.bNumber <> 0
        USB_ERROR       if device type absent, ChkDevPrsnc.bNumber returns 0

**/

VOID
USBAPI_CheckDevicePresence (URP_STRUC *fpURP)
{
    UINT8 bSearchFlag;
    UINTN dData;

    bSearchFlag = USB_SRCH_DEV_NUM;
    if (fpURP->bSubFunc == 1)
    {
        bSearchFlag = USB_SRCH_DEVBASECLASS_NUM;
    }
    //
    // The total number of devices connected to ANY controller has been requested
    //
    dData = (UINTN) USB_GetDeviceInfoStruc( bSearchFlag,
            0, fpURP->ApiData.ChkDevPrsnc.bDevType, fpURP->ApiData.ChkDevPrsnc.fpHCStruc);

    fpURP->ApiData.ChkDevPrsnc.bNumber = (UINT8)dData;
    fpURP->bRetValue = (UINT8)((fpURP->ApiData.ChkDevPrsnc.bNumber)?
                                            USB_SUCCESS : USB_ERROR);
}


/**
    This function is part of the USB BIOS MASS API. This function
    returns the device information of the mass storage device

    @param fpURPPointer    Pointer to the URP structure
        bDevAddr    USB device address of the device

    @retval bRetValue   Return value
        fpURPPointer    Pointer to the URP structure
        dSenseData  Sense data of the last command
        bDevType    Device type byte (HDD, CD, Removable)
        bEmuType    Emulation type used
        fpDevId     Far pointer to the device ID
        dInt13Entry INT 13h entry point

    @note  Initially the bDevAddr should be set to 0 as input. This
      function returns the information regarding the first mass
      storage device (if no device found it returns bDevAddr as
      0FFh) and also updates bDevAddr to the device address of
      the current mass storage device. If no other mass storage
      device is found then the routine sets the bit7 to 1
      indicating current information is valid but no more mass
      device found in the system. The caller can get the next
      device info if bDevAddr is not 0FFh and bit7 is not set

**/

VOID
USBMassAPIGetDeviceInformation (URP_STRUC *fpURP)
{
    fpURP->bRetValue = USBMassGetDeviceInfo (&fpURP->ApiData.MassGetDevInfo);
}

VOID
USBMassAPIGetDeviceParameters (URP_STRUC *fpURP)
{
    DEV_INFO    *DevInfo;
    EFI_STATUS  Status;

    DevInfo = fpURP->ApiData.MassGetDevParms.fpDevInfo;

    Status = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(Status)) {
        return;
    }

#if !USB_RT_DXE_DRIVER
    if (gCheckUsbApiParameter) {
        Status = AmiValidateMemoryBuffer((VOID*)fpURP->ApiData.MassGetDevParms.fpInqData, sizeof(MASS_INQUIRY));
        if (EFI_ERROR(Status)) {
            return;
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif
    
    fpURP->ApiData.MassGetDevParms.fpInqData = USBMassGetDeviceParameters(DevInfo);
    fpURP->bRetValue = (fpURP->ApiData.MassGetDevParms.fpInqData == NULL)? USB_ERROR : USB_SUCCESS;
}

/**
    This function returns the drive status and media presence
    status

    @param fpURPPointer    Pointer to the URP structure
        fpURP->ApiData.fpDevInfo - pointer to USB device that is
        requested to be checked

    @retval Return code USB_ERROR - Failure, USB_SUCCESS - Success

**/

VOID
USBMassAPIGetDevStatus(URP_STRUC *fpURP)
{
#if USB_DEV_MASS
    fpURP->bRetValue    = USBMassGetDeviceStatus (&fpURP->ApiData.MassGetDevSts);
    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBMassAPIGetDevStatus ... check function call correct?\n");
#endif
}



/**
    This function is part of the USB BIOS MASS API.

    @param fpURPPointer    Pointer to the URP structure
        bDevAddr    USB device address of the device

    @retval bRetValue   Return value
        fpURPPointer    Pointer to the URP structure
        dSenseData  Sense data of the last command
        bNumHeads   Number of heads
        wNumCylinders   Number of cylinders
        bNumSectors Number of sectors
        wBytesPerSector Number of bytes per sector
        bMediaType  Media type

**/

VOID
USBMassAPIGetDeviceGeometry(URP_STRUC *fpURP)
{
    fpURP->bRetValue = USBMassGetDeviceGeometry (&fpURP->ApiData.MassGetDevGeo);
}


/**
    This function is part of the USB BIOS MASS API.

    @param fpURPPointer    Pointer to the URP structure

    @retval bRetValue   Return value

**/

VOID
USBMassAPIResetDevice (URP_STRUC *fpURP)
{
    UINT8       bDevAddr;
    DEV_INFO    *fpDevInfo;
    UINT16      wResult;

    bDevAddr = fpURP->ApiData.MassReset.bDevAddr;

    //
    // Get the device info structure for the matching device address
    //
    fpDevInfo   = USB_GetDeviceInfoStruc(USB_SRCH_DEV_INDX, 0, bDevAddr, 0);
    if((fpDevInfo == NULL)|| (!(fpDevInfo->Flag & DEV_INFO_DEV_PRESENT))) {
		fpURP->bRetValue = USB_ATA_TIME_OUT_ERR;
        return;
    }
    //
    // Send Start/Stop Unit command to UFI class device only
    //
    fpURP->bRetValue    = USB_SUCCESS;
    if(fpDevInfo->bSubClass ==  SUB_CLASS_UFI) {
        wResult = USBMassStartUnitCommand (fpDevInfo);
        if (wResult) {
            fpURP->bRetValue  = USBWrapGetATAErrorCode(fpURP->ApiData.MassReset.dSenseData);
        }
    }
}

/**
    This function is part of the USB BIOS MASS API.

    @param fpURPPointer    Pointer to the URP structure, it contains the following:
        bDevAddr      USB device address of the device
        dStartLBA     Starting LBA address
        wNumBlks      Number of blocks to read
        wPreSkipSize  Number of bytes to skip before
        wPostSkipSize Number of bytes to skip after
        fpBufferPtr   Far buffer pointer

    @retval fpURPPointer Pointer to the URP structure
        bRetValue    Return value
        dSenseData   Sense data of the last command

**/
VOID
USBMassAPIReadDevice (
    URP_STRUC *Urp
)
{
    URP_STRUC               Parameters;

    Parameters.bRetValue = USB_ERROR;
    Parameters.ApiData.EfiMassRead.DevAddr = Urp->ApiData.MassRead.DevAddr;
    Parameters.ApiData.EfiMassRead.StartLba = Urp->ApiData.MassRead.StartLba;
    Parameters.ApiData.EfiMassRead.NumBlks = Urp->ApiData.MassRead.NumBlks;
    Parameters.ApiData.EfiMassRead.PreSkipSize = Urp->ApiData.MassRead.PreSkipSize;
    Parameters.ApiData.EfiMassRead.PostSkipSize = Urp->ApiData.MassRead.PostSkipSize;
    Parameters.ApiData.EfiMassRead.BufferPtr = Urp->ApiData.MassRead.BufferPtr;

    USBMassAPIEfiReadDevice(&Parameters);

    Urp->bRetValue = Parameters.bRetValue;

    return;
}

/**
    This function is part of the USB BIOS MASS API.

    @param fpURPPointer    Pointer to the URP structure, it contains the following:
        DevAddr      USB device address of the device
        StartLBA     Starting LBA address
        NumBlks      Number of blocks to read
        PreSkipSize  Number of bytes to skip before
        PostSkipSize Number of bytes to skip after
        BufferPtr   Far buffer pointer

    @retval fpURPPointer Pointer to the URP structure
        bRetValue    Return value
        dSenseData   Sense data of the last command

**/

VOID
USBMassAPIEfiReadDevice (
    URP_STRUC *Urp
)
{
    DEV_INFO    *DevInfo;
    UINT8       DevAddr;
    UINT16      Result;
    UINT32		Data = 0;
    UINT8       OpCode;

    DevAddr = Urp->ApiData.EfiMassRead.DevAddr;

    if (((DevAddr == USB_HOTPLUG_FDD_ADDRESS) &&
            ((gUsbData->dUSBStateFlag & USB_HOTPLUG_FDD_ENABLED) == FALSE)) ||
        ((DevAddr == USB_HOTPLUG_HDD_ADDRESS) &&
            ((gUsbData->dUSBStateFlag & USB_HOTPLUG_HDD_ENABLED) == FALSE)) ||
        ((DevAddr == USB_HOTPLUG_CDROM_ADDRESS) &&
            ((gUsbData->dUSBStateFlag & USB_HOTPLUG_CDROM_ENABLED) == FALSE))) {
        Urp->bRetValue = USB_ATA_DRIVE_NOT_READY_ERR;
        return;
    }

    //
    // Get the device info structure for the matching device address
    //
    DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_INDX, 0, DevAddr, 0);
    if ((DevInfo == NULL)|| (!(DevInfo->Flag & DEV_INFO_DEV_PRESENT))) {
		Urp->bRetValue = USB_ATA_TIME_OUT_ERR;
        return;
    }

											//(EIP15037+)>
#if EXTRA_CHECK_DEVICE_READY
    //
    // Check device ready
    //
    Data = USBMassCheckDeviceReady(DevInfo);
	if (Data) {
        Urp->ApiData.EfiMassRead.SenseData = Data;
        Urp->bRetValue = USBWrapGetATAErrorCode(Urp->ApiData.EfiMassRead.SenseData);	//(EIP31535)
		return;
	}
#endif
											//<(EIP15037+)

    //
    // Issue read command
    //
    if (RShiftU64(DevInfo->MaxLba, 32)) {
        OpCode = COMMON_READ_16_OPCODE;
    } else {
        OpCode = COMMON_READ_10_OPCODE;
    }
    Result = USBMassRWVCommand(DevInfo, OpCode, &Urp->ApiData.EfiMassRead);
    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, " wr(%x):%x %x", DevAddr, Result, Urp->ApiData.EfiMassRead.SenseData);
    if (Result) {
        Urp->bRetValue = USB_SUCCESS;
        return;
    }
    Urp->bRetValue = USBWrapGetATAErrorCode(Urp->ApiData.EfiMassRead.SenseData);
    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, " er(%x):%x", DevAddr, Urp->bRetValue);
}

/**
    This function is part of the USB BIOS MASS API.

    @param fpURPPointer    Pointer to the URP structure
        bDevAddr    USB device address of the device
        dStartLBA   Starting LBA address
        wNumBlks    Number of blocks to write
        wPreSkipSize    Number of bytes to skip before
        wPostSkipSize   Number of bytes to skip after
        fpBufferPtr Far buffer pointer

    @retval fpURPPointer    Pointer to the URP structure
        bRetValue   Return value
        dSenseData  Sense data of the last command

**/

VOID
USBMassAPIWriteDevice(
    URP_STRUC *Urp
)
{
    URP_STRUC               Parameters;

    Parameters.bRetValue = USB_ERROR;
    Parameters.ApiData.EfiMassWrite.DevAddr = Urp->ApiData.MassWrite.DevAddr;
    Parameters.ApiData.EfiMassWrite.StartLba = Urp->ApiData.MassWrite.StartLba;
    Parameters.ApiData.EfiMassWrite.NumBlks = Urp->ApiData.MassWrite.NumBlks;
    Parameters.ApiData.EfiMassWrite.PreSkipSize = Urp->ApiData.MassWrite.PreSkipSize;
    Parameters.ApiData.EfiMassWrite.PostSkipSize = Urp->ApiData.MassWrite.PostSkipSize;
    Parameters.ApiData.EfiMassWrite.BufferPtr = Urp->ApiData.MassWrite.BufferPtr;

    USBMassAPIEfiWriteDevice(&Parameters);

    Urp->bRetValue = Parameters.bRetValue;

    return;
}

/**
    This function is part of the USB BIOS MASS API.

    @param fpURPPointer    Pointer to the URP structure
        DevAddr    USB device address of the device
        StartLBA   Starting LBA address
        NumBlks    Number of blocks to write
        PreSkipSize    Number of bytes to skip before
        PostSkipSize   Number of bytes to skip after
        BufferPtr Far buffer pointer

    @retval fpURPPointer    Pointer to the URP structure
        bRetValue   Return value
        dSenseData  Sense data of the last command

**/

VOID
USBMassAPIEfiWriteDevice(
    URP_STRUC *Urp
)
{
    DEV_INFO    *DevInfo;
    UINT8       DevAddr;
    UINT16      Result;
    UINT32		Data = 0;
    UINT8       OpCode;

    DevAddr = Urp->ApiData.EfiMassWrite.DevAddr;

    if (((DevAddr == USB_HOTPLUG_FDD_ADDRESS) &&
            ((gUsbData->dUSBStateFlag & USB_HOTPLUG_FDD_ENABLED) == FALSE)) ||
        ((DevAddr == USB_HOTPLUG_HDD_ADDRESS) &&
            ((gUsbData->dUSBStateFlag & USB_HOTPLUG_HDD_ENABLED) == FALSE)) ||
        ((DevAddr == USB_HOTPLUG_CDROM_ADDRESS) &&
            ((gUsbData->dUSBStateFlag & USB_HOTPLUG_CDROM_ENABLED) == FALSE))) {
        Urp->bRetValue = USB_ATA_DRIVE_NOT_READY_ERR;
        return;
    }

    //
    // Get the device info structure for the matching device address
    //
    DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_INDX, 0, DevAddr, 0);
    if((DevInfo == NULL)|| (!(DevInfo->Flag & DEV_INFO_DEV_PRESENT))) {
		Urp->bRetValue = USB_ATA_TIME_OUT_ERR;
        return;
    }
/*
    if (!(DevInfo->bLastStatus & USB_MASS_MEDIA_PRESENT)) {
        Urp->bRetValue = USB_ATA_NO_MEDIA_ERR;
        return;
    }
*/
											//(EIP15037+)>
#if EXTRA_CHECK_DEVICE_READY
    //
    // Check device ready
    //
    Data = USBMassCheckDeviceReady(DevInfo);
	if (Data) {
        Urp->ApiData.EfiMassWrite.SenseData = Data;
        Urp->bRetValue = USBWrapGetATAErrorCode(Urp->ApiData.EfiMassWrite.SenseData);	//(EIP31535)
		return;
	}
#endif
											//<(EIP15037+)

    //
    // Issue write command
    //
    if (RShiftU64(DevInfo->MaxLba, 32)) {
        OpCode = COMMON_WRITE_16_OPCODE;
    } else {
        OpCode = COMMON_WRITE_10_OPCODE;
    }
    Result = USBMassRWVCommand(DevInfo, OpCode, &Urp->ApiData.EfiMassWrite);
    if (Result) {
        Urp->bRetValue = USB_SUCCESS;
        return;
    }

    Urp->bRetValue = USBWrapGetATAErrorCode(Urp->ApiData.EfiMassWrite.SenseData);
}

/**
    This function is part of the USB BIOS MASS API.

    @param fpURPPointer    Pointer to the URP structure
        DevAddr    USB device address of the device
        StartLBA   Starting LBA address
        NumBlks    Number of blocks to write
        PreSkipSize    Number of bytes to skip before
        PostSkipSize   Number of bytes to skip after
        BufferPtr Far buffer pointer

    @retval fpURPPointer    Pointer to the URP structure
        bRetValue   Return value
        dSenseData  Sense data of the last command

**/

VOID
USBMassAPIVerifyDevice(
    URP_STRUC *Urp
)
{
    URP_STRUC               Parameters;

    Parameters.bRetValue = USB_ERROR;
    Parameters.ApiData.EfiMassVerify.DevAddr = Urp->ApiData.MassVerify.DevAddr;
    Parameters.ApiData.EfiMassVerify.StartLba = Urp->ApiData.MassVerify.StartLba;
    Parameters.ApiData.EfiMassVerify.NumBlks = Urp->ApiData.MassVerify.NumBlks;
    Parameters.ApiData.EfiMassVerify.PreSkipSize = Urp->ApiData.MassVerify.PreSkipSize;
    Parameters.ApiData.EfiMassVerify.PostSkipSize = Urp->ApiData.MassVerify.PostSkipSize;
    Parameters.ApiData.EfiMassVerify.BufferPtr = Urp->ApiData.MassVerify.BufferPtr;

    USBMassAPIEfiVerifyDevice(&Parameters);

    Urp->bRetValue = Parameters.bRetValue;

    return;
}

/**
    This function is part of the USB BIOS MASS API.

    @param fpURPPointer    Pointer to the URP structure
        DevAddr    USB device address of the device
        StartLBA   Starting LBA address
        NumBlks    Number of blocks to write
        PreSkipSize    Number of bytes to skip before
        PostSkipSize   Number of bytes to skip after
        BufferPtr Far buffer pointer

    @retval fpURPPointer    Pointer to the URP structure
        bRetValue   Return value
        dSenseData  Sense data of the last command

**/

VOID
USBMassAPIEfiVerifyDevice(
    URP_STRUC *Urp
)
{
    DEV_INFO    *DevInfo;
    UINT8       DevAddr;
    UINT16      Result;
    UINT32		Data = 0;
    UINT8       OpCode;

    DevAddr = Urp->ApiData.EfiMassVerify.DevAddr;

    if (((DevAddr == USB_HOTPLUG_FDD_ADDRESS) &&
            ((gUsbData->dUSBStateFlag & USB_HOTPLUG_FDD_ENABLED) == FALSE)) ||
        ((DevAddr == USB_HOTPLUG_HDD_ADDRESS) &&
            ((gUsbData->dUSBStateFlag & USB_HOTPLUG_HDD_ENABLED) == FALSE)) ||
        ((DevAddr == USB_HOTPLUG_CDROM_ADDRESS) &&
            ((gUsbData->dUSBStateFlag & USB_HOTPLUG_CDROM_ENABLED) == FALSE))) {
        Urp->bRetValue = USB_ATA_DRIVE_NOT_READY_ERR;
        return;
    }

    //
    // Get the device info structure for the matching device address
    //
    DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_INDX, 0, DevAddr, 0);
    if ((DevInfo == NULL)|| (!(DevInfo->Flag & DEV_INFO_DEV_PRESENT))) {
		Urp->bRetValue = USB_ATA_TIME_OUT_ERR;
        return;
    }
											//(EIP15037+)>
#if EXTRA_CHECK_DEVICE_READY
    //
    // Check device ready
    //
    Data = USBMassCheckDeviceReady(DevInfo);
	if (Data) {
        Urp->ApiData.EfiMassVerify.SenseData = Data;
        Urp->bRetValue = USBWrapGetATAErrorCode(Urp->ApiData.EfiMassVerify.SenseData);	//(EIP31535)
		return;
	}
#endif
											//<(EIP15037+)

    //
    // Issue write command
    //
    if (RShiftU64(DevInfo->MaxLba, 32)) {
        OpCode = COMMON_VERIFY_16_OPCODE;
    } else {
        OpCode = COMMON_VERIFY_10_OPCODE;
    }
    Result = USBMassRWVCommand(DevInfo, OpCode, &Urp->ApiData.EfiMassVerify);
    if (Result) {
        Urp->bRetValue = USB_SUCCESS;
        return;
    }

    Urp->bRetValue = USBWrapGetATAErrorCode(Urp->ApiData.EfiMassVerify.SenseData);

}

/**
    This function is part of the USB BIOS MASS API.

    @param fpURPPointer    Pointer to the URP structure

    @retval bRetValue   Return value

**/

VOID
USBMassAPIFormatDevice(URP_STRUC *fpURP)
{
    fpURP->bRetValue = USB_SUCCESS;
}


/**
    This function is part of the USB BIOS MASS API. This
    function can be used to pass raw command/data sequence to
    the USB mass storage device

    @param fpURPPointer    Pointer to the URP structure

    @retval bRetValue   Return value

**/

VOID
USBMassAPICommandPassThru (URP_STRUC *fpURP)
{
    UINT8                   Result;
    MASS_CMD_PASS_THRU      *MassCmdPassThru;
    EFI_STATUS              Status = EFI_SUCCESS;

    MassCmdPassThru = &fpURP->ApiData.MassCmdPassThru;

#if !USB_RT_DXE_DRIVER
    if (gCheckUsbApiParameter) {
        Status = AmiValidateMemoryBuffer((VOID*)MassCmdPassThru->fpCmdBuffer, MassCmdPassThru->wCmdLength);
        if (EFI_ERROR(Status)) {
            return;
        }
        Status = AmiValidateMemoryBuffer((VOID*)MassCmdPassThru->fpDataBuffer, MassCmdPassThru->wDataLength);
        if (EFI_ERROR(Status)) {
            return;
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif

    Result = USBMassCmdPassThru(MassCmdPassThru);
    
    fpURP->bRetValue = Result;
}


/**
    This function is part of the USB BIOS MASS API. This function
    assigns the logical drive device according to the information of the
    mass storage device

    @param fpURPPointer    Pointer to the URP structure
        bDevAddr    USB device address of the device
        bLogDevNum  Logical Drive Number to assign to the device

    @retval Return code USB_ERROR - Failure, USB_SUCCESS - Success

**/

VOID
USBMassAPIAssignDriveNumber (URP_STRUC *fpURP)
{
    fpURP->bRetValue = USB_SUCCESS; // No errors expected after this point
}


/**
    This function is part of the USB BIOS MASS API. This function
    invokes USB Mass Storage API handler to check whether device
    is ready. If called for the first time, this function retrieves
    the mass storage device geometry and fills the corresponding
    fpDevInfo fields.

    @param fpURPPointer    Pointer to the URP structure
        fpURP->ApiData.fpDevInfo - pointer to USB device that is
        requested to be checked

    @retval Return code USB_ERROR - Failure, USB_SUCCESS - Success

**/

VOID
USBMassAPICheckDevStatus(URP_STRUC *fpURP)
{
#if USB_DEV_MASS
    UINT32      Result;
    DEV_INFO    *DevInfo;
    EFI_STATUS  Status;

    DevInfo = fpURP->ApiData.MassChkDevReady.fpDevInfo;

    Status = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(Status)) {
        return;
    }
	
	gCheckUsbApiParameter = FALSE;

    Result = USBMassCheckDeviceReady(fpURP->ApiData.MassChkDevReady.fpDevInfo);
    fpURP->bRetValue = (UINT8)Result;
#endif
}


/**
    This function is part of the USB BIOS API. This function
    controls LED state on the connected USB keyboards

    @param fpURP   Pointer to the URP structure

    @retval Return code USB_ERROR - Failure, USB_SUCCESS - Success

**/

VOID
USBAPI_LightenKeyboardLEDs(URP_STRUC *fpURP)
{
#if USB_DEV_KBD
    EFI_STATUS  Status;
    DEV_INFO    *DevInfo = (DEV_INFO*)fpURP->ApiData.EfiKbLedsData.DevInfoPtr;

    if (DevInfo) {
        Status = UsbDevInfoValidation(DevInfo);
        if (EFI_ERROR(Status)) {
            fpURP->bRetValue = USB_PARAMETER_ERROR;
            return;
        }
    }

    gCheckUsbApiParameter = FALSE;
    
	if (fpURP->ApiData.EfiKbLedsData.LedMapPtr) {
		gUsbData->bUSBKBShiftKeyStatus = 0;
		if(((LED_MAP*)fpURP->ApiData.EfiKbLedsData.LedMapPtr)->NumLock) {
			gUsbData->bUSBKBShiftKeyStatus |= KB_NUM_LOCK_BIT_MASK;
		}
		if(((LED_MAP*)fpURP->ApiData.EfiKbLedsData.LedMapPtr)->CapsLock) {
			gUsbData->bUSBKBShiftKeyStatus |= KB_CAPS_LOCK_BIT_MASK;
		}
		if(((LED_MAP*)fpURP->ApiData.EfiKbLedsData.LedMapPtr)->ScrLock) {
			gUsbData->bUSBKBShiftKeyStatus |= KB_SCROLL_LOCK_BIT_MASK;
		}
	}

	if (DevInfo) {
		UsbKbdSetLed(DevInfo, ((gUsbData->bUSBKBShiftKeyStatus) >> 4) & 0x07);
	}

	fpURP->bRetValue	= USB_SUCCESS;
	return;
#else
	fpURP->bRetValue	= USB_NOT_SUPPORTED;
#endif
}

VOID
USBAPI_LightenKeyboardLEDs_Compatible(URP_STRUC *fpURP)
{
#if USB_DEV_KBD
	if (fpURP->ApiData.KbLedsData.LedMapPtr) {
		gUsbData->bUSBKBShiftKeyStatus = 0;
		if(((LED_MAP*)fpURP->ApiData.KbLedsData.LedMapPtr)->NumLock) {
			gUsbData->bUSBKBShiftKeyStatus |= KB_NUM_LOCK_BIT_MASK;
		}
		if(((LED_MAP*)fpURP->ApiData.KbLedsData.LedMapPtr)->CapsLock) {
			gUsbData->bUSBKBShiftKeyStatus |= KB_CAPS_LOCK_BIT_MASK;
		}
		if(((LED_MAP*)fpURP->ApiData.KbLedsData.LedMapPtr)->ScrLock) {
			gUsbData->bUSBKBShiftKeyStatus |= KB_SCROLL_LOCK_BIT_MASK;
		}
	}

	//USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3," LEDs: %d\n", gUsbData->bUSBKBShiftKeyStatus);
	USBKB_LEDOn();

	fpURP->bRetValue	= USB_SUCCESS;
	return;
#else
	fpURP->bRetValue	= USB_NOT_SUPPORTED;
#endif
}

					                        //(EIP29733+)>
/**
    This function is part of the USB BIOS API. This function
    is used to control whether KBC access in USB module 
    should be blocked or not.

    @param fpURP   Pointer to the URP structure

    @retval Return code USB_ERROR - Failure, USB_SUCCESS - Success

**/


VOID
EFIAPI
UsbKbcAccessControl(
    UINT8 ControlSwitch
)
{
    UINT8       Index;
    HC_STRUC    *HcStruc;
#if !USB_RT_DXE_DRIVER
    EFI_STATUS Status;
    
    Status = AmiUsbSmmGlobalDataValidation(gUsbData);

    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) {
        gLockHwSmiHandler = TRUE;
        gLockSmiHandler = TRUE;
        return;
    }
#endif

    gCheckUsbApiParameter = FALSE;
  
    gUsbData->IsKbcAccessBlocked = (ControlSwitch != 0)? TRUE : FALSE;

    for (Index = 0; Index < gUsbData->HcTableCount; Index++) {
        HcStruc = gUsbData->HcTable[Index];
        if (HcStruc == NULL) {
            continue;
        }
        if (HcStruc->dHCFlag & HC_STATE_RUNNING) {
            break;
        }
    }

    if (Index == gUsbData->HcTableCount) {
        return;
    }

/*
    //
    // Check if the USB access in Legacy mode. If it's legacy mode enable/disable
    // the Kbcemulation based on the ControlSwitch  
    //
    if(!(gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI)) {

        if(IsKbcAccessBlocked) {
            if(gEmulationTrap) { 
                //
                // Keyboard access blocked. Disable the Emulation
                //
                gEmulationTrap->TrapDisable(gEmulationTrap);
            }
        } else {
            if(gEmulationTrap) { 
                //
                // Keyboard access enabled. Enable the KbcEmulation
                //
                gEmulationTrap->TrapEnable(gEmulationTrap);
            }
        }
    }
*/
					//(EIP48323+)>
    //Reflush USB data buffer if intend to disable usb keyboard data throughput.
    if (gUsbData->IsKbcAccessBlocked) {
        USBKeyRepeat(NULL, 1);  // Disable Key repeat
        gUsbData->RepeatKey = 0;

		// Clear Legacy USB keyboard buffer
        ZeroMem(gUsbData->aKBCCharacterBufferStart, sizeof(gUsbData->aKBCCharacterBufferStart));
		gUsbData->fpKBCCharacterBufferHead = gUsbData->aKBCCharacterBufferStart;
		gUsbData->fpKBCCharacterBufferTail = gUsbData->aKBCCharacterBufferStart;
		
		ZeroMem(gUsbData->aKBCScanCodeBufferStart, sizeof(gUsbData->aKBCScanCodeBufferStart));
		gUsbData->fpKBCScanCodeBufferPtr = gUsbData->aKBCScanCodeBufferStart;
											
		ZeroMem(gUsbData->aKBCDeviceIDBufferStart, sizeof(gUsbData->aKBCDeviceIDBufferStart));
		ZeroMem(gUsbData->aKBCShiftKeyStatusBufferStart, sizeof(gUsbData->aKBCShiftKeyStatusBufferStart));		
		ZeroMem(gUsbData->aKBInputBuffer, sizeof(gUsbData->aKBInputBuffer));
    }
					//<(EIP48323+)
}


VOID
USBAPI_KbcAccessControl(URP_STRUC *fpURP)
{
    UsbKbcAccessControl(fpURP->ApiData.KbcControlCode);
}
					                        //<(EIP29733+)
/**
    This function is part of the USB BIOS API. This function init USB 
    legacy support.

    @param fpURP   Pointer to the URP structure

    @retval VOID

**/
VOID
USB_StopLegacy(URP_STRUC *fpURP)
{
    //shutdown device first
    UINT8       bIndex;
    DEV_INFO    *fpDevInfo;
    HC_STRUC    *fpHCStruc;
    
    for (bIndex = 1; bIndex < MAX_DEVICES; bIndex ++){
        fpDevInfo = gUsbData->aDevInfoTable +bIndex;
        if ((fpDevInfo->Flag & 
            (DEV_INFO_VALID_STRUC |DEV_INFO_DEV_PRESENT)    ) ==   
            (DEV_INFO_VALID_STRUC |DEV_INFO_DEV_PRESENT)    ){
            //
            fpHCStruc = gUsbData->HcTable[fpDevInfo->bHCNumber - 1];
            //
            USB_StopDevice (fpHCStruc, fpDevInfo->bHubDeviceNumber, fpDevInfo->bHubPortNumber);
        }
    }

    StopControllerType(USB_HC_XHCI);    //(EIP57521+)
    StopControllerType(USB_HC_EHCI);
    StopControllerType(USB_HC_UHCI);
    StopControllerType(USB_HC_OHCI);
    
    //return as success
    fpURP->bRetValue    = USB_SUCCESS;
    
}

/**
    This function is part of the USB BIOS API. This function init USB 
    legacy support.

    @param fpURP   Pointer to the URP structure

    @retval VOID

**/
VOID
USB_StartLegacy(URP_STRUC *fpURP)
{
                                        //(EIP57521)>
    gUsbData->bHandOverInProgress = FALSE;
    //Start XHCI
    StartControllerType(USB_HC_XHCI);
    USB_EnumerateRootHubPorts(USB_HC_XHCI);
    //Start EHCI
    StartControllerType(USB_HC_EHCI);
    USB_EnumerateRootHubPorts(USB_HC_EHCI);
    //Start UHCI
    StartControllerType(USB_HC_UHCI);
    USB_EnumerateRootHubPorts(USB_HC_UHCI);
    //Start OHCI
    StartControllerType(USB_HC_OHCI);
    USB_EnumerateRootHubPorts(USB_HC_OHCI);
                                        //<(EIP57521)
    //return as success
  fpURP->bRetValue    = USB_SUCCESS;
}

/**
    This function is part of the USB BIOS API. This function
    is used to shutdown/init USB legacy support.

    @param fpURP   Pointer to the URP structure

    @retval VOID

**/
VOID USBAPI_LegacyControl (URP_STRUC *fpURP)
{
    UINT8 bSubLegacyFunc = fpURP->bSubFunc,i;       //(EIP102150+)
    UINT8 Count = (UINT8)(gUsbData->fpKBCScanCodeBufferPtr - 
                  (UINT8*)gUsbData->aKBCScanCodeBufferStart);   //(EIP102150+) 

    gCheckUsbApiParameter = FALSE;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBAPI_LegacyControl %d\n", fpURP->bSubFunc);
    if(bSubLegacyFunc==STOP_USB_CONTROLLER){ 		//(EIP43475+)>  	
        USB_StopLegacy (fpURP);
                                        //(EIP102150+)>
    for(i = Count; i > 0; i--)
        USBKB_DiscardCharacter(&gUsbData->aKBCShiftKeyStatusBufferStart[i-1]); 
                                        //<(EIP102150+)
        if(gEmulationTrap) 
            gEmulationTrap->TrapDisable(gEmulationTrap);
    }

    if(bSubLegacyFunc==START_USB_CONTROLLER){
        USB_StartLegacy (fpURP);
        if(gEmulationTrap)
            gEmulationTrap->TrapEnable(gEmulationTrap);
    }												//<(EIP43475+)
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Result %d\n", fpURP->bRetValue);
}
										//(EIP74876+)>
/**
    This function is part of the USB BIOS API. This function stops 
    the USB host controller.

    @param fpURP   Pointer to the URP structure

    @retval VOID

**/
VOID USBAPI_UsbStopController (URP_STRUC *fpURP)
{
    gCheckUsbApiParameter = FALSE;
	StopControllerBdf(fpURP->ApiData.HcBusDevFuncNum);
}
										//<(EIP74876+)
//-----------------------------------------------------
//
//-----------------------------------------------------
EFI_STATUS
EFIAPI
USBRT_LegacyControl(
    VOID *fpURP
)
{
#if !USB_RT_DXE_DRIVER
    EFI_STATUS Status;
    
    Status = AmiUsbSmmGlobalDataValidation(gUsbData);

    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) {
        gLockHwSmiHandler = TRUE;
        gLockSmiHandler = TRUE;
        return EFI_SUCCESS;
    }
#endif
  //
  USBAPI_LegacyControl ((URP_STRUC *)fpURP);
  //
  return((EFI_STATUS)(((URP_STRUC *)fpURP)->bRetValue));
}


/**

    @param 

    @retval 

**/

VOID
USBAPI_GetDeviceAddress(
    URP_STRUC *Urp
)
{
	UINT8		i;
	DEV_INFO	*DevInfo = NULL;

	for (i = 1; i < MAX_DEVICES; i++) {
		if (!(gUsbData->aDevInfoTable[i].Flag & DEV_INFO_VALID_STRUC)) {
			continue;
		}
		if ((gUsbData->aDevInfoTable[i].wVendorId == Urp->ApiData.GetDevAddr.Vid) && 
				(gUsbData->aDevInfoTable[i].wDeviceId == Urp->ApiData.GetDevAddr.Did)) {
			DevInfo = &gUsbData->aDevInfoTable[i];
		}
	}
	if (DevInfo == NULL) {
		Urp->bRetValue = USB_ERROR;
		return;
	}

	Urp->ApiData.GetDevAddr.DevAddr = DevInfo->bDeviceAddress;
	Urp->bRetValue = USB_SUCCESS;
}

/**

    @param 

    @retval 

**/

VOID
USBAPI_ExtDriverRequest (
    URP_STRUC *Urp
)
{
    DEV_INFO	*DevInfo = NULL;
    DEV_DRIVER	*DevDriver;

	DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_ADDR, 0, Urp->ApiData.DevAddr, 0);
	if (DevInfo == NULL) {
		Urp->bRetValue = USB_ERROR;
		return;
	}
	
	DevDriver = UsbFindDeviceDriverEntry(DevInfo->fpDeviceDriver);

	if (DevDriver != NULL) {
		DevDriver->pfnDriverRequest(DevInfo, Urp);
	}
}

/**
    This routine is called, from host controllers that supports
    OS handover functionality, when OS wants the BIOS to hand-over 
    the host controllers to the OS.  This routine will stop HC that 
    does not support this functionality.

    @param VOID

    @retval VOID

**/

VOID
EFIAPI
USB_StopUnsupportedHc(
    VOID
)
{

#if !USB_RT_DXE_DRIVER
    UINT8       Index;
    HC_STRUC*   HcStruc;
    EFI_STATUS Status;
    
    Status = AmiUsbSmmGlobalDataValidation(gUsbData);

    ASSERT_EFI_ERROR(Status);
    
    if (EFI_ERROR(Status)) {
        gLockHwSmiHandler = TRUE;
        gLockSmiHandler = TRUE;
        return;
    }
    
	//USBSB_UninstallTimerHandlers();

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Stopping all external HCs");
    for (Index = 0; Index < gUsbData->HcTableCount; Index++) {
        HcStruc = gUsbData->HcTable[Index];
        if (HcStruc == NULL) {
            continue;
        }
        if (HcStruc->HwSmiHandle != NULL) {
            continue;
        }
        if (HcStruc->bHCType == USB_HC_XHCI) {
            if (!(HcStruc->dHCFlag & HC_STATE_EXTERNAL)) {
                continue;
            }
        }
        if (HcStruc->dHCFlag & HC_STATE_RUNNING) {
            (*gUsbData->aHCDriverTable[GET_HCD_INDEX(HcStruc->bHCType)].pfnHCDStop)(HcStruc);
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, ".");
        }
    }
    
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "\n");

#endif

	if(gUsbData->UsbXhciHandoff) {
		StopControllerType(USB_HC_XHCI);
	}
	if(gUsbData->UsbEhciHandoff) {
		gUsbData->bHandOverInProgress = TRUE;
		StopControllerType(USB_HC_EHCI);
	}
	if(gUsbData->UsbOhciHandoff) {
		StopControllerType(USB_HC_OHCI);
	}
    
#if !USB_RT_DXE_DRIVER

    // Disable kbc emulation trap if we don't own any xhci controller.
    for (Index = 0; Index < gUsbData->HcTableCount; Index++) {
        HcStruc = gUsbData->HcTable[Index];
        if (HcStruc == NULL) {
            continue;
        }
        if ((HcStruc->bHCType == USB_HC_XHCI) && (HcStruc->dHCFlag & HC_STATE_RUNNING)) {
            break;
        }
    }

    if (Index == gUsbData->HcTableCount) {
        if (gEmulationTrap) {
            gEmulationTrap->TrapDisable(gEmulationTrap);
        }
    }
    
#endif   
}

/**
    This function is part of the USB BIOS API. This function
    updates the global variables according to the new owner

    @param fpURP   Pointer to the URP structure

    @retval Return code USB_ERROR - Failure, USB_SUCCESS - Success

    @note  It is a caller responsibility to release the keyboard only if it
              was previously acquired.

**/

VOID
USBAPI_ChangeOwner(URP_STRUC *fpURP)
{
    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBAPI_ChangeOwner..");

    if(fpURP->ApiData.Owner) {  // Changing to Efi driver
    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "fpURP->ApiData.Owner=%d\n", fpURP->ApiData.Owner);
        if(gEmulationTrap) {
            gEmulationTrap->TrapDisable(gEmulationTrap);
        }
        gUsbData->dUSBStateFlag |= USB_FLAG_RUNNING_UNDER_EFI;
    } else {    // Acquiring - check the current condition first
        //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "fpURP->ApiData.Owner=%d...", fpURP->ApiData.Owner);
        if(gEmulationTrap) {
            gEmulationTrap->TrapEnable(gEmulationTrap);
        }

        if (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) {
            //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USB_FLAG_RUNNING_UNDER_EFI\n");
            gUsbData->dUSBStateFlag &= ~USB_FLAG_RUNNING_UNDER_EFI;
        } else {
            //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "not USB_FLAG_RUNNING_UNDER_EFI\n");
        }
    }
    fpURP->bRetValue = USB_SUCCESS;
}

/**
    This function is part of the USB BIOS API. This function 
    starts/stops the USB host controller.

    @param fpURP   Pointer to the URP structure

    @retval VOID

**/

VOID
USBAPI_HcStartStop(URP_STRUC *Urp)
{
    HC_STRUC*   HcStruc;
    EFI_STATUS  Status;

    HcStruc = Urp->ApiData.HcStartStop.HcStruc;

    Status = UsbHcStrucValidation(HcStruc);

    if (EFI_ERROR(Status)) {
        return;
    }

    if ((HcStruc->bHCType != USB_HC_UHCI) && (HcStruc->bHCType != USB_HC_OHCI) &&
        (HcStruc->bHCType != USB_HC_EHCI) && (HcStruc->bHCType != USB_HC_XHCI)) {
        return;
    }

    gCheckUsbApiParameter = FALSE;

    if (Urp->ApiData.HcStartStop.Start) {
        Urp->bRetValue = UsbHcStart(HcStruc);
    } else {
        Urp->bRetValue = UsbHcStop(HcStruc);
    }
}

/**
    Invokes procedure passing parameters supplied in the buffer
    It replicates the stack frame so that target procedure can
    see the parameters passed to the stub.

    @retval Returns result of invoked proc

**/

//
// The following typedef corresponds to the min width type that can be passed
// into function call as a parameter without padding
//
typedef UINTN STACKWORD;

UINTN 
USBAPI_invoke_in_frame(
    VOID* pProc,
    VOID* buffer,
    UINT32 paramSize )
{
    STACKWORD* params = (STACKWORD*)buffer;

    switch(paramSize/sizeof(STACKWORD)){
    case 0: return ((STACKWORD (*)())pProc)();
    case 1: return ((STACKWORD (*)(STACKWORD))pProc)(params[0]);
    case 2: return ((STACKWORD (*)(STACKWORD,STACKWORD))pProc)(params[0],
                params[1]);
    case 3: return ((STACKWORD (*)(STACKWORD,STACKWORD,STACKWORD))pProc)(
                params[0],params[1],params[2]);
    case 4: return ((STACKWORD (*)(STACKWORD,STACKWORD,STACKWORD,
                STACKWORD))pProc)(
                params[0],params[1],params[2],params[3]);
    case 5: return ((STACKWORD (*)(STACKWORD,STACKWORD,STACKWORD,STACKWORD,
                STACKWORD))pProc)(
                params[0],params[1],params[2],params[3],params[4]);
    case 6: return ((STACKWORD (*)(STACKWORD,STACKWORD,STACKWORD,STACKWORD,
                STACKWORD,STACKWORD))pProc)(
                params[0],params[1],params[2],params[3],params[4],params[5]);
    case 7: return ((STACKWORD (*)(STACKWORD,STACKWORD,STACKWORD,STACKWORD,
                STACKWORD,STACKWORD,STACKWORD))pProc)(
                params[0],params[1],params[2],params[3],params[4],params[5],
                params[6]);
    default:
        ASSERT(paramSize/sizeof(STACKWORD) < 4);
        return 0;
    }
/*  kept for reference
    __asm {
        push    ecx
        push    esi
        pushf
                        //Copy stak frame
        std
        mov     esi, buffer
        mov     ecx, paramSize
        add     esi, ecx
        sub     esi, 4
        shr     ecx, 2
loop1:
        lodsd   //DWORD PTR ds:edi
        push    eax
        loop    loop1
                        //Call proc
        mov     eax, pProc
        cld
        call    eax
                        //Read return value
        mov     retVal, eax

                        //Restore stack and registers
        add     esp, paramSize
        popf
        pop     esi
        pop     ecx
    }
    return retVal;*/
}

/**
    Bridge to a number of procedures supplied by HC driver


    @param fpURP   Pointer to the URP structure

    @retval Return code USB_ERROR - Failure, USB_SUCCESS - Success)

    @note  
      Assumes that buffer has a correct image of the stack that
      corresponding function reads argument from
      Size of the buffer can be biger than actually used.

      Following code copies the buffer (some stack frame) into new
      stack frame such that invoked dirver proc can read parametes
      supplied by buffer
**/

VOID USBAPI_HC_Proc(URP_STRUC *fpURP)
{
    VOID* buffer = fpURP->ApiData.HcProc.paramBuffer;
    UINT32 paramSize = // align size on DWORD
        (fpURP->ApiData.HcProc.paramSize + 3) & ~0x3;
    UN_HCD_HEADER* pHdr;

    ASSERT( GET_HCD_INDEX(fpURP->ApiData.HcProc.bHCType) <
        sizeof(gUsbData->aHCDriverTable)/sizeof(HCD_HEADER));
    ASSERT( fpURP->bSubFunc < sizeof(pHdr->asArray.proc)/sizeof(VOID*));

    pHdr = (UN_HCD_HEADER*)(gUsbData->aHCDriverTable +
                GET_HCD_INDEX(fpURP->ApiData.HcProc.bHCType));
    fpURP->ApiData.HcProc.retVal = USBAPI_invoke_in_frame(
        pHdr->asArray.proc[fpURP->bSubFunc],buffer,paramSize);
    fpURP->bRetValue = USB_SUCCESS;
}


/**
    Bridge to a number of procedures supplied by Core proc table

    @param fpURP   Pointer to the URP structure

    @retval Return code USB_ERROR - Failure, USB_SUCCESS - Success

    @note  
      Assumes that buffer has a correct image of the stack that
      corresponding function reads argument from
      Size of the buffer can be biger than actually used.

      Following code copies the buffer (some stack frame) into new
      stack frame such that invoked proc can read parametes
      supplied by buffer
**/


VOID* core_proc_table[] = {
        USB_GetDescriptor,
        USB_ReConfigDevice,
        USB_ReConfigDevice2,
        UsbAllocDevInfo,
        PrepareForLegacyOs,
        USB_ResetAndReconfigDev,
        USB_DevDriverDisconnect,
//        USB_GetDataPtr,
    };

VOID USBAPI_Core_Proc(URP_STRUC *fpURP)
{
    VOID* buffer = fpURP->ApiData.CoreProc.paramBuffer;
    UINT32 paramSize = // align size on DWORD
        (fpURP->ApiData.CoreProc.paramSize + 3) & ~0x3;

    ASSERT( fpURP->bSubFunc < COUNTOF(core_proc_table));

    fpURP->ApiData.CoreProc.retVal = USBAPI_invoke_in_frame(
        core_proc_table[fpURP->bSubFunc],buffer,paramSize);

    fpURP->bRetValue = USB_SUCCESS;
}


//----------------------------------------------------------------------------
//          USB API Procedures Ends
//----------------------------------------------------------------------------


/**
    This routine converts the sense data information into
    ATAPI error code

    @param dSenseData  Sense data obtained from the device

    @retval BYTE ATAPI error code

**/

UINT8 USBWrapGetATAErrorCode (UINT32 dSenseData)
{
    UINT8   sc = (UINT8)dSenseData;             // Sense code
    UINT8   asc = (UINT8)(dSenseData >> 16);    // Additional Sense Code (ASC)
    UINT8   ascq = (UINT8)(dSenseData >> 8);    // Additional Sense Code Qualifier (ASCQ)

    if (ascq == 0x28) return USB_ATA_DRIVE_NOT_READY_ERR;
    if (sc == 7) return USB_ATA_WRITE_PROTECT_ERR;
    if ((asc == 0x80) && (ascq == 0x80)) return USB_ATA_TIME_OUT_ERR;
    if (ascq == 0x18) return USB_ATA_DATA_CORRECTED_ERR;
    if ((ascq==6) && (asc == 0)) return USB_ATA_MARK_NOT_FOUND_ERR;
    if ((ascq==0x3a) && (asc == 0)) return USB_ATA_NO_MEDIA_ERR;
    if ((ascq==0x11) && (asc == 0)) return USB_ATA_READ_ERR;
    if ((ascq==0x11) && (asc == 6)) return USB_ATA_UNCORRECTABLE_ERR;
    if (ascq==0x30) return USB_ATA_BAD_SECTOR_ERR;
    if ((ascq<0x20) || (ascq>0x26)) return USB_ATA_GENERAL_FAILURE;

    return USB_ATA_PARAMETER_FAILED;
}


/**
    This routine finds the 'n'th device's DeviceInfo entry.

    @param bDevNumber  Device number (1-based)

    @retval DeviceInfo structure

**/

DEV_INFO*
USBWrap_GetnthDeviceInfoStructure(UINT8 bDevNumber)
{
    return &gUsbData->aDevInfoTable[bDevNumber];
}


/**
    This routine searches through the device entry table
    and returns number of active USB devices configured
    by the BIOS.

    @param fpURPPointer    Pointer to the URP

    @retval Following fields in the URP are modified
        CkPresence.bNumBootDev      Number of USB boot devices found
        CkPresence.bNumKeyboards    Number of USB keyboards present
        CkPresence.bNumMice         Number of USB mice present
        CkPresence.bNumHubs         Number of USB hubs present
        CkPresence.bNumStorage      Number of USB storage devices present

**/

VOID
USBWrap_GetDeviceCount(URP_STRUC *fpURP)
{
    DEV_INFO    *fpDevInfo;
    UINT8       i;

    for (i=1; i<MAX_DEVICES; i++) {
        fpDevInfo   = &gUsbData->aDevInfoTable[i];

        if ( (fpDevInfo->Flag & DEV_INFO_VALID_STRUC) &&
            (fpDevInfo->Flag & DEV_INFO_DEV_PRESENT)) {
            fpURP->ApiData.CkPresence.bNumBootDev++;

            switch (fpDevInfo->bDeviceType) {
				case BIOS_DEV_TYPE_HID:
                    if (fpDevInfo->HidDevType & HID_DEV_TYPE_KEYBOARD) {
                        fpURP->ApiData.CkPresence.bNumKeyboards++;
                    }
                    if (fpDevInfo->HidDevType & HID_DEV_TYPE_MOUSE) {
                        fpURP->ApiData.CkPresence.bNumMice++;
                    }
                    if (fpDevInfo->HidDevType & HID_DEV_TYPE_POINT) {
                        fpURP->ApiData.CkPresence.bNumPoint++;
                    }
					break;
										//<(EIP84455+)
                case  BIOS_DEV_TYPE_HUB:
                            fpURP->ApiData.CkPresence.bNumHubs++;
                            break;
                case  BIOS_DEV_TYPE_STORAGE:
                            fpURP->ApiData.CkPresence.bNumStorage++;
                            break;
                case  BIOS_DEV_TYPE_SECURITY:
                            break;
            }
        }
    }
}

UINTN DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
    EFI_DEVICE_PATH_PROTOCOL     *Start;

    if (DevicePath == NULL) {
        return 0;
    }

    //
    // Search for the end of the device path structure
    //
    Start = DevicePath;
    while (!EfiIsDevicePathEnd (DevicePath)) {
        DevicePath = EfiNextDevicePathNode (DevicePath);
    }

    //
    // Compute the size and add back in the size of the end device path structure
    //
    return ((UINTN)DevicePath - (UINTN)Start) + sizeof(EFI_DEVICE_PATH_PROTOCOL);
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
