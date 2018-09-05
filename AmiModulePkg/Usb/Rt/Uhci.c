//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**                 5555 Oakbrook Pkwy, Norcross, GA 30093                 **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

/** @file Uhci.c
    AMI USB UHCI driver

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "AmiUsb.h"
#include "UsbKbd.h"
#if !USB_RT_DXE_DRIVER
#include <Library/AmiBufferValidationLib.h>
#endif

#if UHCI_SUPPORT

extern  VOID*       USB_MemAlloc (UINT16);
extern  UINT8       USB_MemFree  (VOID _FAR_ *,  UINT16);
extern  UINT8       USBCheckPortChange (HC_STRUC*, UINT8, UINT8);
extern  UINT8       USB_InstallCallBackFunction (CALLBACK_FUNC);

extern  VOID        WordWritePCIConfig(UINT16, UINT8, UINT16);

UINT32  HcReadPciReg(HC_STRUC*, UINT32);
VOID    HcWritePciReg(HC_STRUC*, UINT32, UINT32);
VOID    HcWordWritePciReg(HC_STRUC*, UINT32, UINT16);
UINT32  HcReadHcMem(HC_STRUC*, UINT32);
VOID    HcWriteHcMem(HC_STRUC*, UINT32, UINT32);
VOID    HcClearHcMem(HC_STRUC*, UINT32, UINT32);
VOID    HcSetHcMem(HC_STRUC*, UINT32, UINT32);
UINT32  HcReadOpReg(HC_STRUC*, UINT32);
VOID    HcWriteOpReg(HC_STRUC*, UINT32, UINT32);
VOID    HcClearOpReg(HC_STRUC*, UINT32, UINT32);
VOID    HcSetOpReg(HC_STRUC*, UINT32, UINT32);
UINT8   HcByteReadHcIo(HC_STRUC*, UINT32);
VOID    HcByteWriteHcIo(HC_STRUC*, UINT32, UINT8);
UINT16  HcWordReadHcIo(HC_STRUC*, UINT32);
VOID    HcWordWriteHcIo(HC_STRUC*, UINT32, UINT16);
UINT32  HcDwordReadHcIo(HC_STRUC*, UINT32);
VOID    HcDwordWriteHcIo(HC_STRUC*, UINT32, UINT32);
UINT8	HcDmaMap(HC_STRUC*, UINT8, UINT8*, UINT32, UINT8**, VOID**);
UINT8	HcDmaUnmap(HC_STRUC*, VOID*);

extern  VOID        FixedDelay(UINTN);
extern  DEV_INFO*   USB_GetDeviceInfoStruc(UINT8, DEV_INFO*, UINT8, HC_STRUC*);
extern  UINT8       USBLogError(UINT16);
extern  VOID        USB_InitFrameList (HC_STRUC*, UINT32);
#if USB_DEV_KBD
extern  VOID        USBKBDPeriodicInterruptHandler(HC_STRUC*);
extern  VOID        USBKeyRepeat(HC_STRUC*, UINT8);
#endif

UINT8   USB_DisconnectDevice (HC_STRUC*, UINT8, UINT8);
UINT8	UsbGetDataToggle(DEV_INFO*,UINT8);
VOID	UsbUpdateDataToggle(DEV_INFO*, UINT8, UINT8);

//---------------------------------------------------------------------------

// Public function declaration
UINT8   UHCI_Start (HC_STRUC*);
UINT8   UHCI_Stop (HC_STRUC*);
UINT8   UHCI_EnumeratePorts (HC_STRUC*);
UINT8   UHCI_DisableInterrupts (HC_STRUC*);
UINT8   UHCI_EnableInterrupts (HC_STRUC*);
UINT8   UHCI_ProcessInterrupt(HC_STRUC*);
UINT32  UHCI_GetRootHubStatus (HC_STRUC*, UINT8, BOOLEAN);
UINT8   UHCI_DisableRootHub (HC_STRUC*,UINT8);
UINT8   UHCI_EnableRootHub (HC_STRUC*,UINT8);
UINT8   UHCI_ResetRootHub (HC_STRUC*,UINT8);
UINT16  UHCI_ControlTransfer (HC_STRUC*,DEV_INFO*,UINT16,UINT16,UINT16,UINT8*,UINT16);
UINT32  UHCI_BulkTransfer (HC_STRUC*,DEV_INFO*,UINT8,UINT8*,UINT32);
UINT16  UHCI_InterruptTransfer (HC_STRUC*, DEV_INFO*, UINT8, UINT16, UINT8*, UINT16);
UINT8   UHCI_DeactivatePolling (HC_STRUC*,DEV_INFO*);
UINT8   UHCI_ActivatePolling (HC_STRUC*,DEV_INFO*);
UINT8   UHCI_DisableKeyRepeat (HC_STRUC*);
UINT8   UHCI_EnableKeyRepeat (HC_STRUC*);
UINT8   UHCI_GlobalSuspend (HC_STRUC*);	//(EIP54018+)
UINT8   UhciSmiControl(HC_STRUC*, BOOLEAN);

UINT8   UhciProcessQh (HC_STRUC*, UHCI_QH*);
UINT8   UhciProcessTd (HC_STRUC*, UHCI_TD*);
UINT8   UhciProcessFrameList (HC_STRUC*);

UINT8   UHCI_DisableHCPorts (HC_STRUC*);
UINT8   UHCI_StartTDSchedule (HC_STRUC*);
UINT8   UHCI_StopTDSchedule (HC_STRUC*);
UINT8   UHCI_InterruptTDCallback (HC_STRUC*, DEV_INFO*, UINT8*, UINT8*);
UINT8   UhciAddQhToFrameList (HC_STRUC*, UHCI_QH*);
UINT8   UhciRemoveQhFromFrameList (HC_STRUC*, UHCI_QH*);
VOID    UhciInitQh (UHCI_QH*);
BOOLEAN UhciIsHalted(HC_STRUC*);
UINT8   UhciTranslateInterval(UINT8);

UHCI_TD*
UhciAllocGeneralTds (
	IN UINT8		DeviceAddr,
	IN BOOLEAN		LowSpeed,
	IN UINT8		PacketId,
	IN UINT8        EndpointAddr,
	IN UINT16		MaxPacket,
	IN BOOLEAN		ShortPacket,
    IN OUT UINTN    *BufferAddr,
    IN OUT UINT32   *Length,
    IN OUT UINT8    *DataToggle
);

VOID
UhciFreeTds (
	IN UHCI_TD	*FirstTd
);

VOID
UhciActivateTds (
	IN UHCI_TD	*FirstTd,
	IN UINT8	DataToggle
);

extern  USB_GLOBAL_DATA *gUsbData;
extern  BOOLEAN gCheckUsbApiParameter;

UINT8   UhciRootHubQhCallBack(HC_STRUC*, DEV_INFO*, UINT8*, UINT8*, UINT16);
UINT8   UhciRepeatQhCallback (HC_STRUC*, DEV_INFO*, UINT8*, UINT8*, UINT16);
UINT8   UhciPollingQhCallback (HC_STRUC*, DEV_INFO*, UINT8*, UINT8*, UINT16);

/**
    This function fills the host controller driver routine pointers

    @param 
        fpHCDHeader     Ptr to the host controller header structure

    @retval 
        USB_SUCCESS = Success, USB_ERROR = Failure

**/

UINT8
UHCI_FillHCDEntries(
    HCD_HEADER *HcdHeader
)
{
    //
    // Fill the routines here
    //
    HcdHeader->pfnHCDStart                = UHCI_Start;
    HcdHeader->pfnHCDStop                 = UHCI_Stop;
    HcdHeader->pfnHCDEnumeratePorts       = UHCI_EnumeratePorts;
    HcdHeader->pfnHCDDisableInterrupts    = UHCI_DisableInterrupts;
    HcdHeader->pfnHCDEnableInterrupts     = UHCI_EnableInterrupts;
    HcdHeader->pfnHCDProcessInterrupt     = UHCI_ProcessInterrupt;
    HcdHeader->pfnHCDGetRootHubStatus     = UHCI_GetRootHubStatus;
    HcdHeader->pfnHCDDisableRootHub       = UHCI_DisableRootHub;
    HcdHeader->pfnHCDEnableRootHub        = UHCI_EnableRootHub;
    HcdHeader->pfnHCDControlTransfer      = UHCI_ControlTransfer;
    HcdHeader->pfnHCDBulkTransfer         = UHCI_BulkTransfer;
    HcdHeader->pfnHCDInterruptTransfer    = UHCI_InterruptTransfer;
    HcdHeader->pfnHCDDeactivatePolling    = UHCI_DeactivatePolling;
    HcdHeader->pfnHCDActivatePolling      = UHCI_ActivatePolling;
    HcdHeader->pfnHCDDisableKeyRepeat     = UHCI_DisableKeyRepeat;
    HcdHeader->pfnHCDEnableKeyRepeat      = UHCI_EnableKeyRepeat;
    HcdHeader->pfnHCDEnableEndpoints      = USB_EnableEndpointsDummy;
    HcdHeader->pfnHCDInitDeviceData       = USB_InitDeviceDataDummy;
    HcdHeader->pfnHCDDeinitDeviceData     = USB_DeinitDeviceDataDummy;
    HcdHeader->pfnHCDResetRootHub         = UHCI_ResetRootHub;
    HcdHeader->pfnHCDClearEndpointState	= 0;	//(EIP54283+)
    HcdHeader->pfnHCDGlobalSuspend        = UHCI_GlobalSuspend;	//(EIP54018+)
    HcdHeader->pfnHCDSmiControl           = UhciSmiControl;

    USB_InstallCallBackFunction(UhciPollingQhCallback);
    USB_InstallCallBackFunction(UhciRepeatQhCallback);
    USB_InstallCallBackFunction(UhciRootHubQhCallBack);

    return  USB_SUCCESS;
}

/**
    This API function is called to start a UHCI host controller. The input to the
    routine is the pointer to the HC structure that defines this host controller

    @param 
        fpHCStruc   Ptr to the host controller structure

    @retval 
  Status: USB_SUCCESS = Success, USB_ERROR = Failure

**/

UINT8
UHCI_Start(
    HC_STRUC*   HcStruc
)
{
    UINT16  IoAddr;
    UINT16  LegSupReg;
    UINT16  Index;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }
    
#if !USB_RT_DXE_DRIVER
    EfiStatus = AmiValidateMmioBuffer((VOID*)HcStruc->fpFrameList, 0x1000);
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }
#endif

/*
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Enabling IO/BM for UHCI HC %02X\n", HcStruc->wBusDevFuncNum);
    //
    // Enable IO access and Bus Mastering
    //
    HcWordWritePciReg(HcStruc, 4, BIT0 + BIT2);
*/
    //
    // Set number of root hub ports present
    //
    HcStruc->bNumPorts = 2;

    //
    // Get the I/O base address for the host controller
    //
    IoAddr = (UINT16)HcReadPciReg(HcStruc, USB_IO_BASE_ADDRESS);

    //
    // Mask the low order two bits and store the value in HCStruc
    //
    IoAddr = (UINT16)(IoAddr & (~(BIT0+BIT1)));
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "HC I/O Address : %X\n", IoAddr);
    HcStruc->BaseAddress = IoAddr;

    HcStruc->wAsyncListSize = UHCI_FRAME_LIST_SIZE;

    //
    // Disable hardware interrupt generation by programming legacy registers
    //
    LegSupReg = (UINT16)HcReadPciReg(HcStruc, USB_UHCI_REG_LEGSUP);

    //
    // Disable generation of SMI/IRQ and clear status bits
    //
    LegSupReg = (UINT16)(LegSupReg & (~BIT4));
    HcWordWritePciReg(HcStruc, USB_UHCI_REG_LEGSUP, LegSupReg);

    //
    // Disable the interrupts (to aVOID spurious interrupts)
    //
    UHCI_DisableInterrupts(HcStruc);

    //
    // Disable the host controller root hub ports
    //
    UHCI_DisableHCPorts(HcStruc);

    //
    // Check whether HC is already stopped
    //
    if (!UhciIsHalted(HcStruc)) {
        //
        // HC is still running. Stop it by programming HC run bit
        //
        HcByteWriteHcIo(HcStruc, UHCI_COMMAND_REG,
            HcByteReadHcIo(HcStruc, UHCI_COMMAND_REG) & ~UHC_HOST_CONTROLLER_RUN);

        //
        // Check whether the host controller is halted (check for 50 ms)
        //
        for (Index = 0; Index < 500; Index++) {
            if ((HcByteReadHcIo(HcStruc, UHCI_STATUS_REG)) & UHC_HC_HALTED) {
                break;
            }
            FixedDelay(100);      // 100 us delay
        }
    }
    ASSERT((HcByteReadHcIo(HcStruc, UHCI_STATUS_REG)) & UHC_HC_HALTED);
    if (!((HcByteReadHcIo(HcStruc, UHCI_STATUS_REG)) & UHC_HC_HALTED)) {
        return USB_ERROR;
    }

    //
    // Reset the host controller
    //
    HcByteWriteHcIo(HcStruc, UHCI_COMMAND_REG, UHC_GLOBAL_RESET);
	
    FixedDelay(10 * 1000);    // Recommended 10msec delay, UHCI Spec, p.12, GRESET description
	
    HcByteWriteHcIo(HcStruc, UHCI_COMMAND_REG, 0);

    //
    // Memory has been allocated in AMIUHCD
    //
    if (!HcStruc->fpFrameList) {
        return USB_ERROR;
    }

    USB_InitFrameList(HcStruc, UHCI_TERMINATE);

    //
    // Program frame list pointer to the HC
    //
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "Frame list pointer : %x\n", HcStruc->fpFrameList);
    HcDwordWriteHcIo(HcStruc, UHCI_FRAME_LIST_BASE, (UINT32)(UINTN)HcStruc->fpFrameList);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_6, "UHCI_StartTDSchedule\n");

    //
    // Start the TD schedule
    //
    if (UHCI_StartTDSchedule(HcStruc) != USB_SUCCESS) {
        return USB_ERROR;
    }

#if !USB_RT_DXE_DRIVER
    //
    // Enable hardware interrupt generation by programming legacy registers
    //
    if (!(HcStruc->dHCFlag & HC_STATE_EXTERNAL)) {
        LegSupReg = (UINT16)HcReadPciReg(HcStruc, USB_UHCI_REG_LEGSUP);
        // Enable generation of SMI/IRQ
        LegSupReg = (UINT16)(LegSupReg | BIT4) & ~BIT13;
        HcWordWritePciReg(HcStruc, USB_UHCI_REG_LEGSUP, LegSupReg);
    }
#endif
    //
    // Start the host controller by setting the run and configure bit
    //
    HcWordWriteHcIo(HcStruc, UHCI_COMMAND_REG,
        UHC_HOST_CONTROLLER_RUN |
        UHC_CONFIGURE_FLAG |
        UHC_MAX_PACKET_64_BYTE);

    //
    // Enable interrupt generation
    //
//  WordWriteIO((UINT16)(wIOAddr + UHCI_INTERRUPT_ENABLE), (UHC_IOC_ENABLE | UHC_TIMEOUT_CRC_ENABLE));
    HcWordWriteHcIo(HcStruc, UHCI_INTERRUPT_ENABLE, UHC_IOC_ENABLE);

    HcStruc->dHCFlag |= HC_STATE_RUNNING;

    // Set USB_FLAG_DRIVER_STARTED flag when HC is running.
    if (!(gUsbData->dUSBStateFlag & USB_FLAG_DRIVER_STARTED)) {
        gUsbData->dUSBStateFlag |= USB_FLAG_DRIVER_STARTED;
    }

#if !USB_RT_DXE_DRIVER
    //
    // Register the USB HW SMI handler
    //
    if (!(HcStruc->dHCFlag & HC_STATE_IRQ)) {
        if (!(HcStruc->dHCFlag & HC_STATE_EXTERNAL)) {
            UsbInstallHwSmiHandler(HcStruc);
        } else {
            USBSB_InstallUsbIntTimerHandler();
    	}
    }
#endif

    return USB_SUCCESS;
}


/**
    This API function is called to stop the UHCI controller. The input to the
    routine is the pointer to the HC structure that defines this host controller.

    @param 
        fpHCStruc   Ptr to the host controller structure

    @retval 
  Status: USB_SUCCESS = Success, USB_ERROR = Failure

**/

UINT8
UHCI_Stop(
    HC_STRUC* HcStruc
)
{
    UINT16      LegSupReg;
    UINT16      Index;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }

    //
    // Disable hardware interrupt generation by programming legacy registers
    //
    if (!(HcStruc->dHCFlag & HC_STATE_EXTERNAL)) {
        LegSupReg = (UINT16)HcReadPciReg(HcStruc, USB_UHCI_REG_LEGSUP);

        //
        // Disable generation of SMI/IRQ and clear status bits
        //
        LegSupReg = (UINT16)(LegSupReg & ~(BIT4));
        HcWordWritePciReg(HcStruc, USB_UHCI_REG_LEGSUP, LegSupReg);
    }

    //
    // Disable the host controller interrupt generation
    //
    UHCI_DisableInterrupts(HcStruc);

	// Disconnect all devices	
	USB_DisconnectDevice(HcStruc, HcStruc->bHCNumber | BIT7, 1);
	USB_DisconnectDevice(HcStruc, HcStruc->bHCNumber | BIT7, 2);

    //
    // Stop the host controller
    //
    if (!UhciIsHalted(HcStruc)) {
        //
        // Clear HC run bit
        //
        HcByteWriteHcIo(HcStruc, UHCI_COMMAND_REG,
            HcByteReadHcIo(HcStruc, UHCI_COMMAND_REG) & ~(UHC_HOST_CONTROLLER_RUN));

        //
        // Check whether the host controller is halted (check for 50 ms)
        //
        for (Index = 0; Index < 500; Index++) {
            if ((HcByteReadHcIo(HcStruc, UHCI_STATUS_REG)) & UHC_HC_HALTED) {
                break;
            }
            FixedDelay(100);      // 100 us delay
        }
    }
    ASSERT((HcByteReadHcIo(HcStruc, UHCI_STATUS_REG)) & UHC_HC_HALTED);

    // Reset the host controller
    HcByteWriteHcIo(HcStruc, UHCI_COMMAND_REG, UHC_GLOBAL_RESET);
	// Recommended 10msec delay, UHCI Spec, p.12, GRESET description
    FixedDelay(10 * 1000);
    HcByteWriteHcIo(HcStruc, UHCI_COMMAND_REG, 0);

    //
    // Clear the frame list pointers
    //
    USB_InitFrameList(HcStruc, UHCI_TERMINATE);

    //
    // Disable and free the TD schedule data structures
    //
    UHCI_StopTDSchedule(HcStruc);

    //
    // Set the HC state to stopped
    //
    HcStruc->dHCFlag &= ~(HC_STATE_RUNNING);

    CheckBiosOwnedHc();

    return USB_SUCCESS;
}


/**
    This API function is called to enumerate the root hub ports in the UHCI
    controller. The input to the routine is the pointer to the HC structure
    that defines this host controller

    @param 
        fpHCStruc   Ptr to the host controller structure

    @retval 
  Status: USB_SUCCESS = Success, USB_ERROR = Failure

**/

UINT8
UHCI_EnumeratePorts(
    HC_STRUC* HcStruc
)
{
    UINT8       HcNumber;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }

	if (UhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}

//(USB0061+)>
    //
    // Check whether USB host controllers are accessible to aVOID system 
    // hang in ports enumeration.
    //
    if (HcByteReadHcIo(HcStruc, 0) == 0xFF) {
        return USB_ERROR;
    }
//<(USB0061+)
    //
    // Check whether enumeration is already began
    //
    if (gUsbData->bEnumFlag == TRUE) {
        return USB_SUCCESS;
    }

    gUsbData->bEnumFlag = TRUE;
    HcNumber = (UINT8)(HcStruc->bHCNumber | BIT7);
										//(EIP61385)>
    //
    // Process Port#1 and clear Port#1 status bit
    //
	if ((HcWordReadHcIo(HcStruc, UHCI_PORT1_CONTROL) & (UHC_CONNECT_STATUS_CHANGE | 
		UHC_CONNECT_STATUS)) == UHC_CONNECT_STATUS_CHANGE) {
		 HcWordWriteHcIo(HcStruc, UHCI_PORT1_CONTROL, UHC_CONNECT_STATUS_CHANGE);
	}
    
    USBCheckPortChange(HcStruc, HcNumber, 1);
    
    HcWordWriteHcIo(HcStruc, UHCI_PORT1_CONTROL, 
        HcWordReadHcIo(HcStruc, UHCI_PORT1_CONTROL));

    //
    // Process Port#2 and clear Port#2 status bit
    //
	if ((HcWordReadHcIo(HcStruc, UHCI_PORT2_CONTROL) & (UHC_CONNECT_STATUS_CHANGE | 
		UHC_CONNECT_STATUS)) == UHC_CONNECT_STATUS_CHANGE) {
		 HcWordWriteHcIo(HcStruc, UHCI_PORT2_CONTROL, UHC_CONNECT_STATUS_CHANGE);
	}
    
    USBCheckPortChange(HcStruc, HcNumber, 2);
    
    HcWordWriteHcIo(HcStruc, UHCI_PORT2_CONTROL, 
        HcWordReadHcIo(HcStruc, UHCI_PORT2_CONTROL));
										//<(EIP61385)
    gUsbData->bEnumFlag = FALSE;

    return USB_SUCCESS;
}


/**
    This API function is called to disable the interrupts generated by the UHCI
    host controller. The input to the routine is the pointer to the HC structure
    that defines this host controller.

    @param 
        fpHCStruc   Ptr to the host controller structure

    @retval 
  Status: USB_SUCCESS = Success, USB_ERROR = Failure

**/

UINT8
UHCI_DisableInterrupts(
    HC_STRUC* HcStruc
)
{
    UINT8   IntEnReg;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    IntEnReg = HcByteReadHcIo(HcStruc, UHCI_INTERRUPT_ENABLE);
    IntEnReg &= ~(UHC_IOC_ENABLE);
    HcByteWriteHcIo(HcStruc, UHCI_INTERRUPT_ENABLE, IntEnReg);

    return USB_SUCCESS;
}


/**
    This function enables the HC interrupts

    @param 
        fpHCStruc   Pointer to the HCStruc structure

    @retval 
        USB_SUCCESS of USB_ERROR

**/

UINT8
UHCI_EnableInterrupts(
    HC_STRUC* HcStruc
)
{
    return USB_SUCCESS;
}


/**
    This function is called when the USB interrupt bit is set. This function
    will parse through the TDs and QHs to find out completed TDs and call
    their respective callback functions.

    @param 
        fpHCStruc   Pointer to the HCStruc structure

         
    @retval USB_ERROR Need more Interrupt processing
    @retval USB_SUCCESS No interrupts pending

**/

UINT8
UHCI_ProcessInterrupt(
    HC_STRUC    *HcStruc
)
{
	UINT16  LegSupReg = 0;
    UINT16  UsbSts = 0;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

	if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
		return USB_ERROR;
	}

#if !USB_RT_DXE_DRIVER
	if (!(HcStruc->dHCFlag & HC_STATE_EXTERNAL)) {
		LegSupReg = (UINT16)HcReadPciReg(HcStruc, USB_UHCI_REG_LEGSUP);
		if (!(LegSupReg & BIT4)) {
			return USB_ERROR;
		}
	}
#endif

    //
    // Check whether the controller is still under BIOS control
    // Read the frame list base address and compare with stored value
    //
    if (((UINTN)HcDwordReadHcIo(HcStruc, UHCI_FRAME_LIST_BASE) & 0xFFFFF000) != 
        (UINTN)HcStruc->fpFrameList) {
#if !USB_RT_DXE_DRIVER
        if (!(HcStruc->dHCFlag & HC_STATE_EXTERNAL)) {
			// Disable the SMI on IRQ enable bit
			HcWordWritePciReg(HcStruc, USB_UHCI_REG_LEGSUP, LegSupReg & ~BIT4);
		}
#endif
        return USB_ERROR; // Control is not with us anymore
    }

    UsbSts = HcWordReadHcIo(HcStruc, UHCI_STATUS_REG);

	if (UsbSts & UHC_HC_HALTED) {
		return USB_ERROR;
	}

    if (UsbSts & UHC_USB_INTERRUPT) {
		HcWordWriteHcIo(HcStruc, UHCI_STATUS_REG, UsbSts);
        UhciProcessFrameList(HcStruc);
    }
    if (HcStruc->dHCFlag & HC_STATE_EXTERNAL) {
        UhciRootHubQhCallBack(HcStruc, NULL, NULL, NULL, 0);
    }

    return  USB_SUCCESS;
}


/**
    This function returns the port connect status for the root hub port

    @param 
        pHCStruc    Pointer to HCStruc of the host controller
        bPortNum    Port in the HC whose status is requested

    @retval 
        NewPortNum  Port in the HC that can possibly replace bPortNum

**/

UINT32
UHCI_GetRootHubStatus(
    HC_STRUC*   HcStruc,
    UINT8       PortNum,
    BOOLEAN     ClearChangeBits
)
{
    UINT32  PortStatus;
    UINT16  PortTemp;
    UINT16  PortStatusOffset = (PortNum << 1) + UHCI_PORT1_CONTROL - 2;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    PortStatus = USB_PORT_STAT_DEV_OWNER;

    //
    // Read the port status
    //
    PortTemp = HcWordReadHcIo(HcStruc, PortStatusOffset);
	USB_DEBUG(DEBUG_INFO, 3, "UHCI port[%d] status: %04x\n", PortNum, PortTemp);

    //
    // Check for port connect status
    //
    if (PortTemp & UHC_CONNECT_STATUS) {
        PortStatus |= USB_PORT_STAT_DEV_CONNECTED;

        //
        // Identify the speed of the device (full or low speed)
        //
        if (PortTemp & UHC_LOW_SPEED_ATTACHED) {
            PortStatus |= USB_PORT_STAT_DEV_LOWSPEED;
        }
        else {
            PortStatus |= USB_PORT_STAT_DEV_FULLSPEED;
        }

		if (PortTemp & UHC_PORT_ENABLE) {
			PortStatus |= USB_PORT_STAT_DEV_ENABLED;
		}
    }

    //
    // Check for connect status change
    //
    if (PortTemp & UHC_CONNECT_STATUS_CHANGE) {
        PortStatus |= USB_PORT_STAT_DEV_CONNECT_CHANGED;
        if (ClearChangeBits == TRUE) {
            HcWordWriteHcIo(HcStruc, PortStatusOffset, UHC_CONNECT_STATUS_CHANGE);     //(EIP61385+)
        }
    }
    if (PortTemp & UHC_PORT_RESET) {
        PortStatus |= USB_PORT_STAT_DEV_RESET;
    }
    if (PortTemp & UHC_PORT_SUSPEND) {
        PortStatus |= USB_PORT_STAT_DEV_SUSPEND;
    }
    if (PortTemp & UHC_PORT_ENABLE_CHANGE) {
        PortStatus |= USB_PORT_STAT_DEV_ENABLE_CHANGED;
    }

    return PortStatus;
}


/**
    This function disables the root hub of the UHCI controller.

    @param 
        fpHCStruc   Pointer to HCStruc of the host controller
        bPortNum    Root port to be disabled

    @retval 
        USB_SUCCESS or USB_ERROR

**/

UINT8
UHCI_DisableRootHub(
    HC_STRUC*   HcStruc,
    UINT8       PortNum
)
{
    UINT16  PortStatusOffset = (PortNum << 1) + UHCI_PORT1_CONTROL - 2;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }
    //
    // Reset the enable bit
    //
    HcWordWriteHcIo(HcStruc, PortStatusOffset,
        (UINT16)(HcWordReadHcIo(HcStruc, PortStatusOffset) & (~UHC_PORT_ENABLE)));

    return USB_SUCCESS;
}


/**
    This function enables the root hub port specified

    @param 
        fpHCStruc   Pointer to HCStruc of the host controller
        bPortNum    Root port to be enabled

    @retval 
        USB_SUCCESS or USB_ERROR

**/

UINT8
UHCI_EnableRootHub (
    HC_STRUC    *HcStruc,
    UINT8       PortNum)
{
	UINT16  PortStatus;
    UINT16  PortStatusOffset = (PortNum << 1) + UHCI_PORT1_CONTROL - 2;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }
    
	PortStatus = HcWordReadHcIo(HcStruc, PortStatusOffset);

    //
    // Set the enable & reset bit, preserve Connect Status Change bit
    //
	PortStatus &= ~(UHC_CONNECT_STATUS_CHANGE | UHC_PORT_ENABLE_CHANGE);
    HcWordWriteHcIo(HcStruc, PortStatusOffset, PortStatus | UHC_PORT_RESET);

    //
    // Wait for 10ms
    //
    FixedDelay(10 * 1000);  // 10msec delay

    //
    // Clear the reset bit and set the enable, preserve Connect Status Change bit
    //
	PortStatus = HcWordReadHcIo(HcStruc, PortStatusOffset);
	PortStatus &= ~(UHC_CONNECT_STATUS_CHANGE | UHC_PORT_ENABLE_CHANGE);
    HcWordWriteHcIo(HcStruc, PortStatusOffset, PortStatus & (~UHC_PORT_RESET));

    // Wait 1 ms for stabilize the port status
    FixedDelay(1 * 1000);        // 1 ms delay

    // Clear Connect Status Change bit and Port Enable Change bit
    HcWordWriteHcIo(HcStruc, PortStatusOffset, HcWordReadHcIo(HcStruc, PortStatusOffset));

    //
    // Set the enable bit
    //
    PortStatus = HcWordReadHcIo(HcStruc, PortStatusOffset);
    PortStatus &= ~(UHC_CONNECT_STATUS_CHANGE | UHC_PORT_ENABLE_CHANGE);
    HcWordWriteHcIo(HcStruc, PortStatusOffset, PortStatus | UHC_PORT_ENABLE);

    //
    // Wait for 100ms
    //
    //FixedDelay(gUsbData->UsbTimingPolicy.UhciPortEnable * 1000);        // 100msec delay

    return USB_SUCCESS;
}

/**
    This function resets the UHCI HC root hub port

    @param 
        HcStruc   Pointer to HCStruc of the host controller
        PortNum    Root port to be enabled

    @retval 
        USB_SUCCESS or USB_ERROR

**/

UINT8
UHCI_ResetRootHub (
    HC_STRUC*	HcStruc,
    UINT8		PortNum)
{
    return USB_SUCCESS;
}


/**
    This function suspend the UHCI HC.

**/

UINT8
UHCI_GlobalSuspend(
    HC_STRUC*	HcStruc
)
{

    UINT16      UhciCommand;
    UINT16      UhciStatus;
    UINT32      i;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }

	if (UhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}

    UhciCommand = HcWordReadHcIo(HcStruc, UHCI_COMMAND_REG);
    UhciCommand &= ~UHC_HOST_CONTROLLER_RUN;
    HcWordWriteHcIo(HcStruc, UHCI_COMMAND_REG, UhciCommand);
    for (i = 0; i < 1024; i++) {
        UhciStatus = HcWordReadHcIo(HcStruc, UHCI_STATUS_REG);
        if (UhciStatus & UHC_HC_HALTED) {
            break;
        }
        FixedDelay(1 * 1000);
    }
    
    HcWordWriteHcIo(HcStruc, UHCI_INTERRUPT_ENABLE, UHC_RESUME_ENABLE);
    
    UhciStatus = HcWordReadHcIo(HcStruc, UHCI_STATUS_REG);
    UhciStatus |= 0x1F;
    HcWordWriteHcIo(HcStruc, UHCI_STATUS_REG, UhciStatus);
  
    UhciCommand = HcWordReadHcIo(HcStruc, UHCI_COMMAND_REG);
    UhciCommand |= UHC_ENTER_SUSPEND;
    HcWordWriteHcIo(HcStruc, UHCI_COMMAND_REG, UhciCommand);
    FixedDelay(50 * 1000);
    
    HcStruc->dHCFlag &= ~(HC_STATE_RUNNING);
    HcStruc->dHCFlag |= HC_STATE_SUSPEND;
    
    return  USB_SUCCESS;
}

UINT8
UhciSmiControl(
    HC_STRUC* HcStruc,
    BOOLEAN Enable
)
{
    UINT16      LegSupReg;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }
    
    if (HcStruc->HwSmiHandle == NULL) {
        return USB_SUCCESS;
    }

    LegSupReg = (UINT16)HcReadPciReg(HcStruc, USB_UHCI_REG_LEGSUP);

    if (Enable == TRUE) {
        //
        // Eisable generation of SMI/IRQ and clear status bits
        //
        LegSupReg = (UINT16)(LegSupReg | BIT4) & ~BIT13;
        HcWordWritePciReg(HcStruc, USB_UHCI_REG_LEGSUP, LegSupReg);
    } else {
        //
        // Disable generation of SMI/IRQ and clear status bits
        //
        LegSupReg = (UINT16)(LegSupReg & ~(BIT4));
        HcWordWritePciReg(HcStruc, USB_UHCI_REG_LEGSUP, LegSupReg);
    }

    return USB_SUCCESS;
}
                                        
/**
    This function execites a transfer and waits for the completion of 
    the transfer, and returns the transfer results.

    @param 
        Pointers to the first data TD and last data TD in the TD list

    @retval 
        Return value is the size of transferred data, Bytes

**/

UINT32
UhciExecuteTransfer (
    HC_STRUC    *HcStruc,
    UHCI_QH     *TransferQh
)
{
	UINT32	Timeout = gUsbData->wTimeOutValue * 100; // *100, makes it number of 10 usec units
	BOOLEAN InfiniteLoop = (Timeout == 0);

    TransferQh->ActiveFlag = TRUE;
    UhciAddQhToFrameList(HcStruc, TransferQh);

    while (InfiniteLoop || Timeout--) {
        UhciProcessQh(HcStruc, TransferQh);
        if (TransferQh->ActiveFlag == FALSE) {
            break;
        }

        FixedDelay(10);  // 10 microseconds
    }

    UhciRemoveQhFromFrameList(HcStruc, TransferQh);
    UhciProcessQh(HcStruc, TransferQh);
    
    if (TransferQh->ActiveFlag) {
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "UHCI Time-Out\n");
    }

    return TransferQh->BytesTransferred;
}
 
/**
    This function executes a device request command transaction on the USB. One
    setup packet is generated containing the device request parameters supplied
    by the caller.  The setup packet may be followed by data in or data out packets
    containing data sent from the host to the device or vice-versa. This function
    will not return until the request either completes successfully or completes in
    error (due to time out, etc.)

    @param 
        fpHCStruc   Pointer to HCStruc of the host controller
        pDevInfo    DeviceInfo structure (if available else 0)
        wRequest    Request type (low byte)
        Bit 7   : Data direction
        0 = Host sending data to device
        1 = Device sending data to host
        Bit 6-5 : Type
        00 = Standard USB request
        01 = Class specific
        10 = Vendor specific
        11 = Reserved
        Bit 4-0 : Recipient
        00000 = Device
        00001 = Interface
        00010 = Endpoint
        00100 - 11111 = Reserved
        Request code, a one byte code describing
        the actual device request to be executed
        (ex: Get Configuration, Set Address etc)
        wIndex      wIndex request parameter (meaning varies)
        wValue      wValue request parameter (meaning varies)
        fpBuffer    Buffer containing data to be sent to the
        device or buffer to be used to receive data
        wLength     wLength request parameter, number of bytes
        of data to be transferred in or out
        of the host controller


    @retval 
        Number of bytes transferred: 0 - Failure, <>0 - Success


    @note  
  Do not use USB_SUCCESS or USB_ERROR as returned values

**/

UINT16
UHCI_ControlTransfer (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT16      Request,
    UINT16      Index,
    UINT16      Value,
    UINT8       *Buffer,
    UINT16      Length
)
{
    UINT16      Packet;
    UINT32      Temp;
    UINT32      Data;

    DEV_REQ     *DevReq;
    UHCI_TD     *SetupTd = NULL;
    UHCI_TD     *DataTds = NULL;
    UHCI_TD     *StatusTd = NULL;
    UHCI_TD     *LastTd = NULL;
    UHCI_TD     *CurrentTd = NULL;
    UHCI_QH     *CtrlQh;
    UINT16      NumDataTds = 0;
    UINT16      BytesRemaining;
    UINT16      BytesTransferred;
    UINT8       DataToggle;
    UINT8       *BufPhyAddr = NULL;
    VOID        *BufferMapping = NULL;
    EFI_STATUS  EfiStatus = EFI_SUCCESS;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return 0;
    }

    EfiStatus = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(EfiStatus)) {
        return 0;
    }

#if !USB_RT_DXE_DRIVER
    if (gCheckUsbApiParameter) {
        if (Length != 0) {
            EfiStatus = AmiValidateMemoryBuffer((VOID*)Buffer, Length);
            if (EFI_ERROR(EfiStatus)) {
                USB_DEBUG(DEBUG_ERROR, 3, "Uhci ControlTransfer Invalid Pointer, Buffer is in SMRAM.\n");
                return 0;
            }
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return 0;
    }

	if (UhciIsHalted(HcStruc)) {
		return 0;
	}
    
    if (!VALID_DEVINFO(DevInfo)) {
        return 0;
    }

    gUsbData->bLastCommandStatus &= ~( USB_CONTROL_STALLED );
    gUsbData->dLastCommandStatusExtended = 0;

    //
    // Allocate TDs for control setup and control status packets
    //
    SetupTd = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(UHCI_TD));
    if (SetupTd == NULL) {
        return 0;
    }

    //
    // Build the device request in the data area of the control setup TD
    //
    DevReq = (DEV_REQ*)SetupTd->aDataArea;
    DevReq->wRequestType  = Request;
    DevReq->wValue        = Value;
    DevReq->wIndex        = Index;
    DevReq->wDataLength   = Length;

    //
    // dTemp will contain the device address and endpoint shifted and ready to go
    // into the TDs' token field.
    // 10:0] = Dev. Addr & Endpoint
    // [18:8] = Dev. Addr & Endpoint
    //
    Temp = ((UINT32)(DevInfo->bDeviceAddress)) << 8;

    //
    // Fill in various fields in the control setup TD.
    // The LinkPointer field will point to the control data TD if data will
    // be sent/received or to the control status TD if no data is expected.
    // The ControlStatus field will be set to active and interrupt on complete.
    // The Token field will contain the packet size (size of DeviceRequest
    // struc), the device address, endpoint, and a setup PID.
    // The BufferPointer field will point to the TD's DataArea buffer which
    // was just initialized to contain a DeviceRequest struc.
    // The CSReloadValue field will contain 0 because this is a "one shot" packet.
    // The pCallback will be set to point to the UHCI_ControlTDCallback routine.
    // The ActiveFlag field will be set to TRUE.
    //

    //
    // 11/01/10 for HI/LO/FULL
    //
    Data = (((UINT32)DevInfo->bEndpointSpeed) & 1) << 26;
										//(EIP34448)>
    Data |= (UINT32)(UHCI_TD_THREE_ERRORS | UHCI_TD_ACTIVE);
										//<(EIP34448)

    SetupTd->dControlStatus = Data;

    Data = Temp |
        ((UINT32)UHCI_TD_SETUP_PACKET |
        ((UINT32)(sizeof(DEV_REQ) - 1) << 21));

    //
    // Set PID=Setup, and MaxLen
    //
    SetupTd->dToken         = Data;
    SetupTd->pBufferPtr     = (UINT32)(UINTN)SetupTd->aDataArea;
    SetupTd->dCSReload      = 0;
    SetupTd->bActiveFlag   = 1;

    LastTd = SetupTd;
    //
    // Fill in various fields in the control data TD.
    // Enough control data TDs must be initialized to handle the amount of
    // data expected.  The length of the data transfer is currently in wLength.
    // LinkPointer field will be set to the next data TD or the status TD.
    // ControlStatus field will be se to active and interrupt on complete.
    // Token field will contain the data transfer size (still in wLength), device
    // address (in pDevInfo), endpoint (in dTemp), and an in or out PID
    // (in wReqType).
    // BufferPointer field will point to the data buffer passed in by the
    // caller (currently in fpBuffer).
    // CSReloadValue field will contain 0 because this is a "one shot" packet.
    // pCallback will be set to point to the UHCI_ControlTDCallback routine.
    // ActiveFlag field will be set to TRUE.
    //
    if (Length) {
        NumDataTds = Length / DevInfo->wEndp0MaxPacket;
        if (Length % DevInfo->wEndp0MaxPacket) {
            NumDataTds++;
        }

        DataTds = USB_MemAlloc(GET_MEM_BLK_COUNT(NumDataTds * sizeof(UHCI_TD)));
        if (DataTds == NULL) {
            USB_MemFree(SetupTd, GET_MEM_BLK_COUNT_STRUC(UHCI_TD));
            return 0;
        }

        CurrentTd = DataTds;

        DataToggle  = 1;
        BytesRemaining = Length;
 
        //
        // Allocate one more TD to be used either for more data or for TD Status
        //
        HcDmaMap(HcStruc, (UINT8)(Request & BIT7), Buffer, Length, 
			                &BufPhyAddr, &BufferMapping);
        do {
            //
            // 11/01/10 for HI/LO/FULL
            //
            Data = (((UINT32)(DevInfo->bEndpointSpeed) & 1) << 26);
            Data = Data |
                (UINT32)(UHCI_TD_THREE_ERRORS | UHCI_TD_ACTIVE);
            if(Request & BIT7) {
                Data |= UHCI_TD_SHORT_PACKET_DETECT;
            }
            CurrentTd->dControlStatus = Data;
            CurrentTd->pBufferPtr = (UINT32)(UINTN)BufPhyAddr;
            Packet = (UINT16)((BytesRemaining > (DevInfo->wEndp0MaxPacket)) ?
                                DevInfo->wEndp0MaxPacket : BytesRemaining);
            //
            // Packet size is valid
            //
            BytesRemaining = (UINT16)(BytesRemaining - Packet);
            BufPhyAddr = BufPhyAddr + Packet;
            --Packet;

            //
            // [18:8]=Dev. addr & endp
            //
            Data = Temp | (((UINT32)Packet) << 21);
            Data = (Data & 0xFFFFFF00) | UHCI_TD_OUT_PACKET;

            if (Request & BIT7) {
                Data = (Data & 0xFFFFFF00) | UHCI_TD_IN_PACKET;
            }
            if (DataToggle & 1) {
                Data = Data | UHCI_TD_DATA_TOGGLE;  // Make packet into a data 1
            }
            CurrentTd->dToken = Data;
            CurrentTd->dCSReload = 0;
            CurrentTd->bActiveFlag = 1;

            LastTd->pLinkPtr = (UINT32)((UINTN)CurrentTd | UHCI_VERTICAL_FLAG);
            LastTd = CurrentTd;
            CurrentTd++;
            
            DataToggle  ^=  1;
        } while (BytesRemaining);  // End the data TD list
    }
    //
    // Fill in various fields in the TD control status.
    // LinkPointer field will point to TERMINATE.
    // ControlStatus field will be set to active and interrupt on complete.
    // Token field will contain the packet size (0), the device address,
    // endpoint, and a setup PID with opposite data direction as that defined
    // in the request type (wReqType).
    // BufferPointer field will point to the TD's DataArea buffer even though
    // we are not expecting any data transfer.
    // CSReloadValue field will contain 0 because this is a "one shot" packet.
    // pCallback will be set to point to the UHCI_ControlTDCallback routine.
    // ActiveFlag field will be set to TRUE.
    //
    StatusTd = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(UHCI_TD));
    if (StatusTd == NULL) {
        return 0;
    }

    LastTd->pLinkPtr = (UINT32)((UINTN)StatusTd | UHCI_VERTICAL_FLAG);
    LastTd = StatusTd;
    StatusTd->pLinkPtr  = UHCI_TERMINATE;
    Data = (((UINT32)(DevInfo->bEndpointSpeed) & 1) << 26);
										//(EIP34448)>
    Data = Data | (UINT32)(UHCI_TD_THREE_ERRORS | UHCI_TD_ACTIVE);
										//<(EIP34448)
    StatusTd->dControlStatus = Data;
    Data = Temp;
    Data = (Data & 0xFFFFFF00) | (UINT32)UHCI_TD_OUT_PACKET;
    if ((Request & BIT7) == 0) {
        Data = (Data & 0xFFFFFF00) | (UINT32)UHCI_TD_IN_PACKET;
    }
    Data |= (UHCI_TD_DATA_TOGGLE | ((UINT32)UHCI_TD_ACTUAL_LENGTH << 21));
    StatusTd->dToken = Data;
    Data = (UINT32)(UINTN)StatusTd->aDataArea;
    StatusTd->pBufferPtr = Data;
    StatusTd->dCSReload = 0;
    StatusTd->bActiveFlag = 1;
    //
    // Now put the control setup, data and status into the HC's schedule by
    // pointing QhControl's link pointer to control setup TD.
    // This will cause the HC to execute the transaction in the next active frame.

    CtrlQh = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(UHCI_QH));
    if (CtrlQh == NULL) {
        return 0;
    }

    UhciInitQh(CtrlQh);
    CtrlQh->pElementPtr = (UINT32)(UINTN)SetupTd;
    CtrlQh->CurrentTd = SetupTd;
    CtrlQh->Type = Control;
    CtrlQh->FirstTd = SetupTd;

    // Wait till transfer complete
    BytesTransferred = UhciExecuteTransfer(HcStruc, CtrlQh);

    // Calculate the transferred length
    BytesTransferred -= (((SetupTd->dControlStatus & UHCI_TD_ACTUAL_LENGTH) + 1) & 0x7FF);
 
    // Save error information in global variable
    gUsbData->dLastCommandStatusExtended = 
        (CtrlQh->CurrentTd->dControlStatus & UHCI_TD_STATUS_FIELD) >> 17;

    if (CtrlQh->CurrentTd->dControlStatus & UHCI_TD_STALLED ){
        gUsbData->bLastCommandStatus |= USB_CONTROL_STALLED;
        BytesTransferred = 0;
    }

    if (Length) {
        HcDmaUnmap(HcStruc, BufferMapping);
    }

    USB_MemFree(SetupTd, GET_MEM_BLK_COUNT_STRUC(UHCI_TD));
    if (DataTds) {
        USB_MemFree(DataTds, GET_MEM_BLK_COUNT(NumDataTds * sizeof(UHCI_TD)));
    }
    USB_MemFree(StatusTd, GET_MEM_BLK_COUNT_STRUC(UHCI_TD));
    USB_MemFree(CtrlQh, GET_MEM_BLK_COUNT_STRUC(UHCI_QH));

    return BytesTransferred;
}   // UHCI_ControlTransfer


/**
    This function creates a chain of two TDs for bulk data transfer. It fills
    in the following fields in TD:
    pLinkPtr - NextTd address
    dToken   - All bits except length and data toggle

    @param 
        BulkDataTd0     1st TD in the chain
        BulkDataTd1     2nd TD in the chain
        TokenData       Data for dToken
        NumBulkTds      # of bulk TDs

**/

VOID
UHCI_InitBulkTdCommon (
    UHCI_TD *BulkDataTd0,
    UINT32  TokenData,
    UINT16  NumBulkTds
)
{
    UINT16  i;
    UHCI_TD *Td0 = BulkDataTd0;
    UHCI_TD *Td1 = Td0;
    UINT16  NumTd = NumBulkTds*2;

    for (i=0; i<NumTd; i++)
    {
        Td0 = Td1;
        Td0->dToken = TokenData;
        Td1 = (UHCI_TD*)((UINTN)Td0 + sizeof(UHCI_TD));
        Td0->pLinkPtr = (UINT32)(UINTN)Td1 | UHCI_VERTICAL_FLAG;
    }
    // Terminated later in UHCI_InitBulkDataTds
}


/**
    This function initializes the fields in bulk data TD list that remain after
  UHCI_InitBulkTdCommon:
                  - Data buffer pointer
                  - Data length
                  - Data toggle (DAT0/DAT1)

    @retval 
        Pointer to the last TD in the chain

**/

UHCI_TD*
UHCI_InitBulkDataTds(
    IN UHCI_TD      *BulkDataTd,
    IN UINT16       MaxPkt,
    IN UINT32       EndpointSpeed,
    IN OUT UINT32   *Address,
    IN OUT UINT8    *DatToggle,
    IN OUT UINT32   *BytesRemaining,
    IN UINT16       NumBulkTds
)
{
    UINT16  i;
    UINT32  Length = *BytesRemaining;
    UHCI_TD *Td = BulkDataTd;
    UINT32  Addr = *Address;
    UINT8   Toggle = *DatToggle;
    BOOLEAN TheLastTd = FALSE;

    for (i = 0; i < NumBulkTds; i++)
    {
        Length = *BytesRemaining;

        if (Length > (UINT32)MaxPkt)
        {
            Length = (UINT32)MaxPkt;
        }
        else
        {
            TheLastTd = TRUE;
        }
        Td->dToken &= 0x1FFFFF;
        Td->dToken |= (Length - 1) << 21;
    
        Td->dToken &= ~UHCI_TD_DATA_TOGGLE;      // Make packet go into DAT0
        if (Toggle == 1)
        {
            Td->dToken |= UHCI_TD_DATA_TOGGLE;   // Make packet go into DAT1
        }
    
        Td->dControlStatus = EndpointSpeed |
                    (UHCI_TD_THREE_ERRORS | UHCI_TD_ACTIVE);
        if ((Td->dToken & UHCI_TD_PACKET_ID) == UHCI_TD_IN_PACKET) {
            Td->dControlStatus |= UHCI_TD_SHORT_PACKET_DETECT;
        }

        Td->pBufferPtr = Addr;
        Addr += MaxPkt;
        Toggle ^= 1;
        Td = (UHCI_TD*)((UINTN)Td + sizeof(UHCI_TD));
        *BytesRemaining -= Length;

        if (TheLastTd) break;
    }

    Td = (UHCI_TD*)((UINTN)Td - sizeof(UHCI_TD));
    Td->pLinkPtr = UHCI_TERMINATE;

    *Address = Addr;
    *DatToggle = Toggle;

    return Td;
}


/**
    This function executes a bulk transaction on the USB. The transfer may be
    either DATA_IN or DATA_OUT packets containing data sent from the host to
    the device or vice-versa. This function wil not return until the request
    either completes successfully or completes with error (due to time out, etc.)

    @param 
        HcStruc     Pointer to HCStruc of the host controller
        DevInfo     DeviceInfo structure (if available else 0)
        XferDir     Transfer direction
        Bit 7: Data direction
        0 Host sending data to device
        1 Device sending data to host
        Bit 6-0 : Reserved
        Buffer      Buffer containing data to be sent to the device or buffer to be
        used to receive data value in Segment:Offset format
        Length      Length request parameter, number of bytes of data to be transferred
        in or out of the host controller

    @retval 
        Amount of data transferred

**/

UINT32
UHCI_BulkTransfer(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       XferDir,
    UINT8       *Buffer,
    UINT32      Length
)
{
    UINT16      MaxPkt;
    UINT8       Endp;
    UINT32      Data;
    UINT8       DatToggle;
    UINT32      TransferError;
    UHCI_QH     *BulkQh;
    UHCI_TD     *BulkDataTd;
    UHCI_TD     *NextBulkDataTd;
    UHCI_TD     *BulkDataTd0;
    UHCI_TD     *BulkDataTd1;
    UHCI_TD     *LastTd;
    UHCI_TD     *NextLastTd;
    UINT16      NumAllocTDs;
    UINT16      NumBulkTDs;
    UINT32      BytesRemaining;
    UINT32      BytesTransferred;
    UINT32      BytesTransferredNow;
    UINT32      Address;
    UINT32      Eps;
    UINT8       *BufPhyAddr = NULL;
    VOID        *BufferMapping = NULL;
    EFI_STATUS  EfiStatus = EFI_SUCCESS;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return 0;
    }

    EfiStatus = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(EfiStatus)) {
        return 0;
    }

#if !USB_RT_DXE_DRIVER
    if (gCheckUsbApiParameter) {
        EfiStatus = AmiValidateMemoryBuffer((VOID*)Buffer, Length);
        if (EFI_ERROR(EfiStatus)) {
            USB_DEBUG(DEBUG_ERROR, 3, "Uhci BulkTransfer Invalid Pointer, Buffer is in SMRAM.\n");
            return 0;
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return 0;
    }

	if (UhciIsHalted(HcStruc)) {
		return 0;
	}
    
    if (!VALID_DEVINFO( DevInfo)) {
        return 0;
    }

    if (Length == 0) {
        return 0;
    }

    // Clear HW source of error
    gUsbData->bLastCommandStatus &= ~(USB_BULK_STALLED | USB_BULK_TIMEDOUT );
    gUsbData->dLastCommandStatusExtended = 0;
    //
    // Get Bulk IN/OUT enpoint number, data sync value & max packet size
    // Store the appropriate max packet size and endpoint number
    // in the local variables
    //
    MaxPkt = (XferDir & BIT7)? DevInfo->wBulkInMaxPkt : DevInfo->wBulkOutMaxPkt;

    if (MaxPkt == 0) {
        return 0;
    }

    Endp = (XferDir & BIT7)? DevInfo->bBulkInEndpoint : DevInfo->bBulkOutEndpoint;

    //
    // For multiple LUN devices toggle is maintained by LUN0
    //
    DatToggle = UsbGetDataToggle(DevInfo, Endp | XferDir);

    //
    // Form TD token data, less the transfer length and toggle information
    //
    Data = (UINT32)Endp << 7;
    Data = (Data | DevInfo->bDeviceAddress) << 8;
    Data = (XferDir & BIT7)? Data | UHCI_TD_IN_PACKET : Data | UHCI_TD_OUT_PACKET;

    BulkQh = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(UHCI_QH));
    if (BulkQh == NULL) {
        return 0;
    }

    BulkQh->Type = Bulk;

    // Allocate data TDs.
    NumBulkTDs = FULLSPEED_MAX_BULK_DATA_SIZE_PER_FRAME/MaxPkt;
    ASSERT(NumBulkTDs != 0);

    NumAllocTDs = NumBulkTDs*2;

    BulkDataTd0 = (UHCI_TD*)USB_MemAlloc(GET_MEM_BLK_COUNT(NumAllocTDs * sizeof(UHCI_TD)));
    ASSERT(BulkDataTd0 != NULL);

    UHCI_InitBulkTdCommon (BulkDataTd0, Data, NumBulkTDs);

    HcDmaMap(HcStruc, XferDir, Buffer, Length, &BufPhyAddr, &BufferMapping);

    BulkDataTd1 = (UHCI_TD*)((UINTN)BulkDataTd0 + (NumBulkTDs * sizeof(UHCI_TD)));

    BulkDataTd = BulkDataTd0;
    NextBulkDataTd = BulkDataTd1;
    BytesRemaining = Length;
    BytesTransferred = 0;
    BytesTransferredNow = 0;
    Address = (UINT32)(UINTN)BufPhyAddr;
    TransferError = 0;
    Eps = ((UINT32)(DevInfo->bEndpointSpeed) & 1) << 26;

    LastTd = UHCI_InitBulkDataTds(
        BulkDataTd, MaxPkt, Eps, &Address, &DatToggle, &BytesRemaining, NumBulkTDs);
    NextLastTd = LastTd;

    do {
        // Start the transfer by adding TD in the bulk queue head
        UhciInitQh(BulkQh);
        BulkQh->pElementPtr = (UINT32)(UINTN)BulkDataTd;
        BulkQh->CurrentTd = BulkDataTd;
        BulkQh->FirstTd = BulkDataTd;

        // Initialize the next TD block and wait for the current one to complete.
        // In case MaxPkt is 64 Bytes, we have approx. NumBulkTDs*50mcs for this.
        if (BytesRemaining != 0) {
            NextLastTd = UHCI_InitBulkDataTds(
                NextBulkDataTd, MaxPkt, Eps, &Address, &DatToggle, &BytesRemaining, NumBulkTDs);
        }

        // Wait til BulkDataTd is complete, check for errors
        BytesTransferredNow = UhciExecuteTransfer(HcStruc, BulkQh);

		DatToggle = BulkQh->DataToggle;
        TransferError = (BulkQh->CurrentTd->dControlStatus & UHCI_TD_STATUS_FIELD) >> 17;
        if (TransferError) {
            break;
        }
        BytesTransferred += BytesTransferredNow;

        NextBulkDataTd = BulkDataTd;
        LastTd = NextLastTd;
        BulkDataTd = (BulkDataTd == BulkDataTd0)? BulkDataTd1 : BulkDataTd0;

    } while ((BytesTransferred < Length) && !BulkQh->ShortPacketDetected);

    UsbUpdateDataToggle(DevInfo, Endp | XferDir, DatToggle);
    gUsbData->dLastCommandStatusExtended = TransferError;
    if (BulkQh->CurrentTd->dControlStatus & UHCI_TD_STALLED){
        gUsbData->bLastCommandStatus |= USB_BULK_STALLED;
    }
    if (BulkQh->CurrentTd->dControlStatus & UHCI_TD_ACTIVE) {
        gUsbData->bLastCommandStatus |= USB_BULK_TIMEDOUT;
    }
    //
    // Deallocate memory and return the transferred data size
    //
    HcDmaUnmap(HcStruc, BufferMapping);
    USB_MemFree(BulkDataTd0, GET_MEM_BLK_COUNT(NumAllocTDs * sizeof(UHCI_TD)));
    USB_MemFree(BulkQh, GET_MEM_BLK_COUNT_STRUC(UHCI_QH));

    return BytesTransferred;
}



/**
    This function executes an interrupt transaction on the USB. The data transfer
    direction is always DATA_IN. This function wil not return until the request
    either completes successfully or completes in error (due to time out, etc.)

    @param 
        HcStruc   Pointer to HCStruc of the host controller
        DevInfo   DeviceInfo structure (if available else 0)
        EndpointAddress The destination USB device endpoint to which the device request 
                    is being sent.
        MaxPktSize  Indicates the maximum packet size the target endpoint is capable 
        of sending or receiving.
        fpBuffer    Buffer containing data to be sent to the device or buffer to be
        used to receive data
        wLength     wLength request parameter, number of bytes of data to be transferred

    @retval 
        Number of bytes transferred


    @note  
  DO NOT TOUCH THE LINK POINTER OF THE TDInterruptData. It is statically allocated
  and linked with other items in the 1ms schedule

**/

UINT16
UHCI_InterruptTransfer(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       EndpointAddress,
    UINT16      MaxPktSize,
    UINT8       *Buffer,
    UINT16      Length
)
{
    UINT8		Endp;
    UINT8       DataToggle;
    UINT32		Temp;
    UINT32      Value;
    UHCI_QH     *IntQh;
	UHCI_TD		*IntTd;
	UINT32		BytesTransferred;
    UINT8       *BufPhyAddr = NULL;
    VOID        *BufferMapping = NULL;
    EFI_STATUS  EfiStatus = EFI_SUCCESS;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return 0;
    }

    EfiStatus = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(EfiStatus)) {
        return 0;
    }

#if !USB_RT_DXE_DRIVER
    if (gCheckUsbApiParameter) {
        EfiStatus = AmiValidateMemoryBuffer((VOID*)Buffer, Length);
        if (EFI_ERROR(EfiStatus)) {
            USB_DEBUG(DEBUG_ERROR, 3, "Uhci InterruptTransfer Invalid Pointer, Buffer is in SMRAM.\n");
            return 0;
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return 0;
    }

	if (UhciIsHalted(HcStruc)) {
		return 0;
	}
    
    if (!VALID_DEVINFO(DevInfo)) {
        return 0;
    }

    gUsbData->dLastCommandStatusExtended = 0;

    //
    // Check for 0 length transfer (if so, exit)
    //
    if (Length == 0) {
        return 0;
    }

    //
    // Store the descriptor pointer in a local variable
    //
    IntTd = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(UHCI_TD));
	if (IntTd == NULL) {
		return 0;
	}

	IntTd->pLinkPtr  = UHCI_TERMINATE;

    //
    // It is an interrupt IN transaction get appropriate size
    //
    Endp = EndpointAddress & 0xF;
	DataToggle = UsbGetDataToggle(DevInfo, EndpointAddress);

    //
    // Form device address and endpoint in proper order and bit position
    //
    Temp = (UINT32)Endp << 7;
    Temp = (Temp | (DevInfo->bDeviceAddress)) << 8; //[10:0] = Dev. Addr & Endpoint
                                                        //[18:8] = Dev. Addr & Endpoint
    //
    // Fill in various fields in the interrupt data TD
    //
    IntTd->dControlStatus = (((UINT32)(DevInfo->bEndpointSpeed) & 1) << 26) | 
    							UHCI_TD_THREE_ERRORS | UHCI_TD_ACTIVE;
    //
    // Set the buffer pointer. Note that currently UHCI Interrupt Transfer
    // implementation assumes IN packet; the direction is not passed here as
    // parameter. Should this change in future, make a branch to use 
    // UHCI_TD_OUT_PACKET while constructing dToken.
    //
    HcDmaMap(HcStruc, EndpointAddress & BIT7, Buffer, Length, 
		&BufPhyAddr, &BufferMapping);
	IntTd->pBufferPtr = (UINT32)(UINTN)BufPhyAddr;
    Value = (UINT32)(Length - 1);
    Value = ((Value << 21) | Temp) & 0xffffff00;
	Value |= EndpointAddress & BIT7 ? UHCI_TD_IN_PACKET : 
		UHCI_TD_OUT_PACKET;

    if (DataToggle & 1) {
        Value |= UHCI_TD_DATA_TOGGLE;  // Make packet into a data 1
    }

    IntTd->dToken = Value;
    IntTd->dCSReload = 0;
    IntTd->bActiveFlag = 1;

    IntQh = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(UHCI_QH));
    if (IntQh == NULL) {
        return 0;
    }

    UhciInitQh(IntQh);
    IntQh->pElementPtr = (UINT32)(UINTN)IntTd;
    IntQh->CurrentTd = IntTd;
    IntQh->Type = Interrupt;
    IntQh->FirstTd = IntTd;
    IntQh->Interval = UhciTranslateInterval(DevInfo->bPollInterval);

    BytesTransferred = UhciExecuteTransfer(HcStruc, IntQh);

    gUsbData->dLastCommandStatusExtended =
        (IntQh->CurrentTd->dControlStatus & UHCI_TD_STATUS_FIELD) >> 17;

    UsbUpdateDataToggle(DevInfo, EndpointAddress, IntQh->DataToggle);

    HcDmaUnmap(HcStruc, BufferMapping);

    USB_MemFree(IntTd, GET_MEM_BLK_COUNT_STRUC(UHCI_TD));
    USB_MemFree(IntQh, GET_MEM_BLK_COUNT_STRUC(UHCI_QH));

    return (UINT16)BytesTransferred;
}


/**
    This function de-activates the polling TD for the requested device. The
    device may be a USB keyboard or USB hub

    @param 
        fpHCStruc   Pointer to the HC structure
        fpDevInfo   Pointer to the device information structure

    @retval 
        USB_SUCCESS or USB_ERROR

**/

UINT8
UHCI_DeactivatePolling (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo
)
{
    UHCI_QH     *PollQh;
    UINT8       DataToggle;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    EfiStatus = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }

	if (UhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}

    if (DevInfo->fpPollTDPtr == NULL) {
        return USB_ERROR;
    }

    PollQh = (UHCI_QH*)DevInfo->fpPollTDPtr;

    UhciRemoveQhFromFrameList(HcStruc, PollQh);

    DataToggle = (PollQh->FirstTd->dToken & UHCI_TD_DATA_TOGGLE)? 1 : 0;
    if (!(PollQh->FirstTd->dControlStatus & UHCI_TD_STATUS_FIELD)) {
        UsbUpdateDataToggle(DevInfo, DevInfo->IntInEndpoint, DataToggle ^ 1);
    }

    UhciFreeTds(PollQh->FirstTd);
    USB_MemFree(PollQh, GET_MEM_BLK_COUNT_STRUC(UHCI_QH));
    DevInfo->fpPollTDPtr = NULL;

	if (DevInfo->fpPollDataBuffer) {
		USB_MemFree(DevInfo->fpPollDataBuffer, GET_MEM_BLK_COUNT(DevInfo->PollingLength)); 
    	DevInfo->fpPollDataBuffer = NULL;
	}

    return USB_SUCCESS;
}


/**
    This function activates the polling TD for the requested device. The device
    may be a USB keyboard or USB hub

    @param 
        fpHCStruc   Pointer to the HC structure
        fpDevInfo   Pointer to the device information structure

    @retval 
        USB_SUCCESS or USB_ERROR

    @note  
  For the keyboard device this routine allocates TDRepeat also, if it is not
  already allocated. This routine allocate a polling TD and schedule it to 8ms
  schedule for keyboards and to 1024ms schedule for hubs.

**/

UINT8
UHCI_ActivatePolling (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo)
{
    UHCI_TD     *PollTd;
    UHCI_QH     *PollQh;
	BOOLEAN		LowSpeed;
	UINT8		PacketId;
	UINTN		BufferAddr;
	UINT32		DataLen;
	UINT8		DataToggle;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    EfiStatus = UsbDevInfoValidation(DevInfo);

    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }

	if (UhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}
    
    if( !VALID_DEVINFO(DevInfo) )
        return USB_ERROR;

	DevInfo->fpPollDataBuffer = USB_MemAlloc(GET_MEM_BLK_COUNT(DevInfo->PollingLength));
    if (DevInfo->fpPollDataBuffer == NULL) {
        return USB_ERROR;
    }

	LowSpeed = (DevInfo->bEndpointSpeed & 1) != 0;
	PacketId = DevInfo->IntInEndpoint & BIT7 ? UHCI_TD_IN_PACKET : UHCI_TD_OUT_PACKET;
	BufferAddr = (UINTN)DevInfo->fpPollDataBuffer;
	DataLen = DevInfo->PollingLength;
	DataToggle = UsbGetDataToggle(DevInfo, DevInfo->IntInEndpoint);

	PollTd = UhciAllocGeneralTds(DevInfo->bDeviceAddress, LowSpeed, PacketId, 
					DevInfo->IntInEndpoint & 0xF, DevInfo->IntInMaxPkt, TRUE, 
					&BufferAddr, &DataLen, &DataToggle);
	if (PollTd == NULL) {
		USB_MemFree(DevInfo->fpPollDataBuffer, GET_MEM_BLK_COUNT(DevInfo->PollingLength));
		return USB_ERROR;
	}

    PollQh = USB_MemAlloc(GET_MEM_BLK_COUNT_STRUC(UHCI_QH));
    if (PollQh == NULL) {
		USB_MemFree(DevInfo->fpPollDataBuffer, GET_MEM_BLK_COUNT(DevInfo->PollingLength));
        UhciFreeTds(PollTd);
        return USB_ERROR;
    }

    DevInfo->fpPollTDPtr = (UINT8*)PollQh;

    UhciInitQh(PollQh);
    PollQh->pElementPtr = (UINT32)(UINTN)PollTd;
    PollQh->CurrentTd = PollTd;
    PollQh->Type = Interrupt;
    PollQh->FirstTd = PollTd;
    PollQh->Interval = UhciTranslateInterval(DevInfo->bPollInterval);
    PollQh->CallBackIndex = USB_InstallCallBackFunction(UhciPollingQhCallback);
    PollQh->DevInfoPtr = DevInfo;
    PollQh->ActiveFlag = TRUE;

    UhciAddQhToFrameList(HcStruc, PollQh);

    return USB_SUCCESS;
}

/**
    This function disables the keyboard repeat rate logic

    @param 
        fpHCStruc   Pointer to the HCStruc structure

    @retval 
        USB_SUCCESS or USB_ERROR

**/

UINT8
UHCI_DisableKeyRepeat (
    HC_STRUC    *HcStruc
)
{
    UHCI_DESC_PTRS      *UhicDescPtrs;
    UHCI_QH             *Qh;
    EFI_STATUS          EfiStatus;
    UINT8               *MemBlockEnd = gUsbData->fpMemBlockStart + (gUsbData->MemPages << 12);

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }

	if (UhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}

    UhicDescPtrs = HcStruc->stDescPtrs.fpUHCIDescPtrs;

    if (UhicDescPtrs == NULL) {
        return USB_ERROR;
    }

    if (((UINT8*)UhicDescPtrs < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(UhicDescPtrs + sizeof(UHCI_DESC_PTRS)) > MemBlockEnd)) {
        return USB_ERROR;
    }

    Qh = UhicDescPtrs->RepeatQh;

    if (Qh == NULL) {
        return USB_ERROR;
    }

    if (((UINT8*)Qh < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(Qh + sizeof(UHCI_DESC_PTRS)) > MemBlockEnd)) {
        return USB_ERROR;
    }

    if (((UINT8*)Qh->FirstTd < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(Qh->FirstTd + sizeof(UHCI_TD)) > MemBlockEnd)) {
        return USB_ERROR;
    }

    Qh->FirstTd->dCSReload = UHCI_TD_ONE_ERROR;
    Qh->FirstTd->dControlStatus= UHCI_TD_ONE_ERROR;
    Qh->FirstTd->bActiveFlag = 0;
    Qh->ActiveFlag = FALSE;
    Qh->pElementPtr = UHCI_TERMINATE;

    return USB_SUCCESS;
}


/**
    This function enables the keyboard repeat rate logic

    @param 
        fpHCStruc   Pointer to the HCStruc structure

    @retval 
        USB_SUCCESS or USB_ERROR

**/

UINT8
UHCI_EnableKeyRepeat (
    HC_STRUC    *HcStruc
)
{
    UHCI_DESC_PTRS      *UhicDescPtrs;
    UHCI_QH             *Qh;
    EFI_STATUS          EfiStatus;
    UINT8               *MemBlockEnd = gUsbData->fpMemBlockStart + (gUsbData->MemPages << 12);

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }

	if (UhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}

    UhicDescPtrs = HcStruc->stDescPtrs.fpUHCIDescPtrs;

    if (UhicDescPtrs == NULL) {
        return USB_ERROR;
    }

    if (((UINT8*)UhicDescPtrs < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(UhicDescPtrs + sizeof(UHCI_DESC_PTRS)) > MemBlockEnd)) {
        return USB_ERROR;
    }

    Qh = UhicDescPtrs->RepeatQh;

    if (((UINT8*)Qh < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(Qh + sizeof(UHCI_DESC_PTRS)) > MemBlockEnd)) {
        return USB_ERROR;
    }

    if (((UINT8*)Qh->FirstTd < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(Qh->FirstTd + sizeof(UHCI_TD)) > MemBlockEnd)) {
        return USB_ERROR;
    }

    Qh->FirstTd->dCSReload = (UHCI_TD_INTERRUPT_ON_COMPLETE |
                            UHCI_TD_ONE_ERROR | UHCI_TD_ACTIVE);
    Qh->FirstTd->dControlStatus = Qh->FirstTd->dCSReload;
    Qh->FirstTd->bActiveFlag = 1;
    Qh->pElementPtr = (UINT32)(UINTN)Qh->FirstTd;
    Qh->CurrentTd = Qh->FirstTd;
    Qh->ActiveFlag = TRUE;

    return USB_SUCCESS;
}



/**
    This routine disables the UHCI HC root hub ports

    @param 
        fpHCStruc   Ptr to the host controller structure

    @retval 
  Status: USB_SUCCESS = Success, USB_ERROR = Failure

**/

UINT8
UHCI_DisableHCPorts(
    HC_STRUC* HcStruc
)
{

    //
    // Disable the root hub port 1
    //
    HcByteWriteHcIo(HcStruc, UHCI_PORT1_CONTROL,
                (UINT8)(HcByteReadHcIo(HcStruc, UHCI_PORT1_CONTROL) & (~UHC_PORT_ENABLE)));
    //
    // Disable the root hub port 2
    //
    HcByteWriteHcIo(HcStruc, UHCI_PORT2_CONTROL,
                (UINT8)(HcByteReadHcIo(HcStruc, UHCI_PORT2_CONTROL) & (~UHC_PORT_ENABLE)));

    return USB_SUCCESS;
}


/**
    This routine will add the particular QH into the frame list

    @param 
        HcStruc     Pointerr to the host controller structure
        NewQh       Address of the QH to be linked

    @retval 
        USB_SUCCESS/USB_ERROR

**/

UINT8
UhciAddQhToFrameList (
    HC_STRUC    *HcStruc,
    UHCI_QH     *NewQh
)
{
    UINT16      Index;
    UINT32      *PrevPtr;
    UINT32      LinkPtr;
    UHCI_QH     *Qh;
    BOOLEAN     ByInterval = FALSE;
    EFI_STATUS  Status = EFI_SUCCESS;

    if (NewQh == NULL) {
        return USB_ERROR;
    }

    switch (NewQh->Type) {
        case Control:
        case Bulk:
            ByInterval = FALSE;
            break;
        case Interrupt:
            ByInterval = TRUE;
            break;
        case Isochronous:
            ASSERT(FALSE);
        default:
            return USB_ERROR;
    }

    if (ByInterval && NewQh->Interval == 0) {
        return USB_ERROR;
    }

    for (Index = 0; Index < HcStruc->wAsyncListSize; 
        ByInterval ? Index += NewQh->Interval : Index++) {
        PrevPtr = &HcStruc->fpFrameList[Index];
        LinkPtr = *PrevPtr;

        while (!(LinkPtr & UHCI_TERMINATE)) {
            Qh = (UHCI_QH*)(LinkPtr & UHCI_POINTER_MASK);
#if !USB_RT_DXE_DRIVER
            Status = AmiValidateMemoryBuffer((VOID*)Qh, sizeof(UHCI_QH));
            if (EFI_ERROR(Status)) {
               return USB_ERROR;
            }
#endif
            if (Qh->Type <= NewQh->Type) {
                if (ByInterval == FALSE ||
                    Qh->Interval <= NewQh->Interval) {
                    break;
                }
            }
            PrevPtr = &Qh->pLinkPtr;
            LinkPtr = *PrevPtr; 
        }
        if (Qh == NewQh) {
            continue;
        }
#if !USB_RT_DXE_DRIVER
        Status = AmiValidateMemoryBuffer((VOID*)PrevPtr, sizeof(UINT32));
        if (EFI_ERROR(Status)) {
            return USB_ERROR;
        }
#endif
        NewQh->pLinkPtr = *PrevPtr;
        *PrevPtr = (UINT32)((UINTN)NewQh | UHCI_QUEUE_HEAD);
    }

    return USB_SUCCESS;
}

/**
    This routine will remove a QH from the the frame list

    @param 
        HcStruc     Pointerr to the host controller structure
        RetiredQh   Address of the QH to be removed

    @retval 
        USB_SUCCESS/USB_ERROR

**/

UINT8
UhciRemoveQhFromFrameList (
    HC_STRUC    *HcStruc,
    UHCI_QH     *RetiredQh
)
{
    UINT16      Index;
    UINT32      *PrevPtr;
    UINT32      LinkPtr;
    UHCI_QH     *Qh;
    BOOLEAN     ByInterval = FALSE;
    EFI_STATUS  Status = EFI_SUCCESS;

    if (RetiredQh == NULL) {
        return USB_ERROR;
    }

    switch (RetiredQh->Type) {
        case Control:
        case Bulk:
            ByInterval = FALSE;
            break;
        case Interrupt:
            ByInterval = TRUE;
            break;
        case Isochronous:
            ASSERT(FALSE);
        default:
            return USB_ERROR;
    }

    if (ByInterval && RetiredQh->Interval == 0) {
        return USB_ERROR;
    }

    RetiredQh->pElementPtr = UHCI_TERMINATE;

    for (Index = 0; Index < HcStruc->wAsyncListSize;
        ByInterval ? Index += RetiredQh->Interval : Index++) {
        PrevPtr = &HcStruc->fpFrameList[Index];
        LinkPtr = *PrevPtr; 
 
        while (!(LinkPtr & UHCI_TERMINATE)) {
            Qh = (UHCI_QH*)(LinkPtr & UHCI_POINTER_MASK);
#if !USB_RT_DXE_DRIVER
            Status = AmiValidateMemoryBuffer((VOID*)Qh, sizeof(UHCI_QH));
            if (EFI_ERROR(Status)) {
                return USB_ERROR;
            }
#endif
            if(Qh == RetiredQh) {
                break;
            }
            PrevPtr = &Qh->pLinkPtr;
            LinkPtr = *PrevPtr;
        }

        if (LinkPtr & UHCI_TERMINATE) {
            continue;
        }
#if !USB_RT_DXE_DRIVER
        Status = AmiValidateMemoryBuffer((VOID*)PrevPtr, sizeof(UINT32));
        if (EFI_ERROR(Status)) {
            return USB_ERROR;
        }
#endif
        *PrevPtr = RetiredQh->pLinkPtr;
    }

    RetiredQh->pLinkPtr = UHCI_TERMINATE;
    return USB_SUCCESS;
}

/**
    This routine will start the TD schedule for the UHCI controller. After this
    routine TD's can be scheduled for execution.

    @param 
        fpHCStruc   Pointer to the HC information structure

    @retval 
        USB_SUCCESS/USB_ERROR

**/

UINT8
UHCI_StartTDSchedule (
    HC_STRUC  *HcStruc
)
{
    UHCI_DESC_PTRS  *DescPtr;
    UHCI_TD         *Td;

    //
    // Allocate the UHCI descriptor pointer structure
    //
    DescPtr = (UHCI_DESC_PTRS*) USB_MemAlloc (GET_MEM_BLK_COUNT_STRUC(UHCI_DESC_PTRS));
    if (DescPtr == NULL) {
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_4, "UHCI Descriptor struc alloc failed.\n");
        return USB_ERROR;
    }

    //
    // Save the value in the HC struc
    //
    HcStruc->stDescPtrs.fpUHCIDescPtrs = DescPtr;

    DescPtr->StaticQh = USB_MemAlloc(GET_MEM_BLK_COUNT(1 * sizeof(UHCI_QH)));
    if (DescPtr->StaticQh == NULL) {
        return USB_ERROR;
    }

    UhciInitQh(DescPtr->StaticQh);
    DescPtr->StaticQh->Type = Interrupt;
    DescPtr->StaticQh->Interval = 1;
    UhciAddQhToFrameList(HcStruc, DescPtr->StaticQh);

	if (HcStruc->dHCFlag & HC_STATE_EXTERNAL) {
		return USB_SUCCESS;
	}

    DescPtr->RepeatQh = USB_MemAlloc(GET_MEM_BLK_COUNT(1 * sizeof(UHCI_QH) + 
                                         1 * sizeof(UHCI_TD)));
    if(DescPtr->RepeatQh == NULL) {
        return USB_ERROR;
    }

    Td = (UHCI_TD*)((UINTN)DescPtr->RepeatQh + sizeof (UHCI_QH));

    //
    // Initialize the body of TdRepeat. It will run a interrupt transaction
    // to a non-existant dummy device.  This will have the effect of generating
    // a periodic interrupt used to generate keyboard repeat.  This TD is normally
    // inactive, and is only activated when a key is pressed.  TdRepeat will be
    // set to timeout after two attempts.  Since the TD is in the schedule
    // at 16ms intervals, this will generate an interrupt at intervals of 32ms
    // (when the TD is active).  This 32ms periodic interrupt may then
    // approximate the fastest keyboard repeat rate of 30 characters per second.
    //
    Td->pLinkPtr = UHCI_TERMINATE;
    Td->dControlStatus = UHCI_TD_ONE_ERROR;
    Td->dToken = (UHCI_TD_IN_PACKET |
                ((UINT32)DUMMY_DEVICE_ADDR << 8) |
                ((UINT32)(DEFAULT_PACKET_LENGTH - 1) << 21));
    Td->pBufferPtr = (UINT32)(UINTN)Td->aDataArea;
    Td->dCSReload = UHCI_TD_ONE_ERROR;
    Td->bActiveFlag = 0;

    UhciInitQh(DescPtr->RepeatQh);
    DescPtr->RepeatQh->Type = Interrupt;
    DescPtr->RepeatQh->FirstTd = Td;
    DescPtr->RepeatQh->Interval = REPEAT_INTERVAL;
    DescPtr->RepeatQh->CallBackIndex = USB_InstallCallBackFunction(UhciRepeatQhCallback);
    DescPtr->RepeatQh->ActiveFlag = FALSE;

    //
    // Schedule the TDRepeat to 8ms schedule
    //
    UhciAddQhToFrameList(HcStruc, DescPtr->RepeatQh);

    //
    // Inform the common code that key repeat is implemented
    //
    USBKeyRepeat(HcStruc, 0);

    //
    // Initialize the body of root hub TD.  It will run a interrupt
    // transaction to a nonexistent dummy device.  This will have the effect
    // of generating a periodic interrupt for the purpose of checking for
    // attach/detach on the root hub's ports
    // This initialization is done only once for the first HC
    //
    DescPtr->RootHubQh = USB_MemAlloc(GET_MEM_BLK_COUNT(1 * sizeof(UHCI_QH) + 
                                         1 * sizeof(UHCI_TD)));
    ASSERT(DescPtr->RootHubQh != NULL);
    if (DescPtr->RootHubQh == NULL) {
        return USB_ERROR;
    }

    Td = (UHCI_TD*)((UINTN)DescPtr->RootHubQh + sizeof (UHCI_QH));

    Td->pLinkPtr = UHCI_TERMINATE;
    Td->dControlStatus  = 0;
    Td->dToken =
        (UHCI_TD_IN_PACKET | ((UINT32)DUMMY_DEVICE_ADDR << 8) |
        ((UINT32)(DEFAULT_PACKET_LENGTH - 1) << 21));
    Td->pBufferPtr = (UINT32)(UINTN)Td->aDataArea;
    Td->dCSReload = 0;
    Td->bActiveFlag = 0;

    UhciInitQh(DescPtr->RootHubQh);
    DescPtr->RootHubQh->Type = Interrupt;
    DescPtr->RootHubQh->FirstTd = Td;
    DescPtr->RootHubQh->Interval = 128;
    DescPtr->RootHubQh->CallBackIndex = USB_InstallCallBackFunction(UhciRootHubQhCallBack);
    DescPtr->RootHubQh->ActiveFlag = FALSE;

    //
    // Schedule the root hub TD to 256ms schedule
    //
    UhciAddQhToFrameList(HcStruc, DescPtr->RootHubQh);

	if (gUsbData->RootHubHcStruc == NULL) {
	    Td->dCSReload =
	            UHCI_TD_INTERRUPT_ON_COMPLETE |
	            UHCI_TD_ONE_ERROR |
	            UHCI_TD_ACTIVE;
	    Td->bActiveFlag = 1;
	    Td->dControlStatus = Td->dCSReload;
        DescPtr->RootHubQh->pElementPtr = (UINT32)(UINTN)Td;
        DescPtr->RootHubQh->CurrentTd = Td;
        DescPtr->RootHubQh->ActiveFlag = TRUE;;
		gUsbData->RootHubHcStruc = HcStruc;
	}

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_4, "TD's are scheduled\n");

    return USB_SUCCESS;
}

/**
    This routine will stop the TD schedules and frees the data structures

    @param 
        fpHCStruc   Pointer to the HC information structure

    @retval 
        USB_SUCCESS/USB_ERROR

**/

UINT8
UHCI_StopTDSchedule (
    HC_STRUC    *HcStruc
)
{
	UINT8			i;
	UHCI_DESC_PTRS	*DescPtrs = HcStruc->stDescPtrs.fpUHCIDescPtrs;
    UINT8            *MemBlockEnd = gUsbData->fpMemBlockStart + (gUsbData->MemPages << 12);
    //
    // Free all the TD/QH data structures
    //
    if (DescPtrs == NULL) {
        return USB_ERROR;
    }

    if (((UINT8*)DescPtrs < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(DescPtrs + sizeof(UHCI_DESC_PTRS)) > MemBlockEnd)) {
        return USB_ERROR;
    }

    if (((UINT8*)DescPtrs->StaticQh < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(DescPtrs->StaticQh + sizeof(UHCI_QH)) > MemBlockEnd)) {
        return USB_ERROR;
    }

    if (((UINT8*)DescPtrs->RootHubQh < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(DescPtrs->RootHubQh + sizeof(UHCI_QH)) > MemBlockEnd)) {
        return USB_ERROR;
    }

    if (((UINT8*)DescPtrs->RepeatQh < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(DescPtrs->RepeatQh + sizeof(UHCI_QH)) > MemBlockEnd)) {
        return USB_ERROR;
    }

    USB_MemFree (DescPtrs->StaticQh, GET_MEM_BLK_COUNT(sizeof(UHCI_QH)));
	if (DescPtrs->RootHubQh) {
    	USB_MemFree (DescPtrs->RootHubQh, 
        	GET_MEM_BLK_COUNT(sizeof(UHCI_QH) + sizeof(UHCI_TD)));
	}
	if (DescPtrs->RepeatQh) {
		USB_MemFree (DescPtrs->RepeatQh, 
        	GET_MEM_BLK_COUNT(sizeof(UHCI_QH) + sizeof(UHCI_TD)));
	}
 
    //
    // Finally free the descriptor pointer
    //
    USB_MemFree (DescPtrs, GET_MEM_BLK_COUNT_STRUC(UHCI_DESC_PTRS));

	USBKeyRepeat(HcStruc, 3);

	if (gUsbData->RootHubHcStruc == HcStruc) {
		gUsbData->RootHubHcStruc  = NULL;
		for (i = 0; i < gUsbData->HcTableCount; i++) {
            if (gUsbData->HcTable[i] == NULL) {
                continue;
            }
			if ((gUsbData->HcTable[i]->bHCNumber) && 
				(gUsbData->HcTable[i]->bHCType == USB_HC_UHCI) &&
				(gUsbData->HcTable[i]->dHCFlag & HC_STATE_RUNNING) &&
				(HcStruc != gUsbData->HcTable[i])) {

				DescPtrs = gUsbData->HcTable[i]->stDescPtrs.fpUHCIDescPtrs;
                
                if (((UINT8*)DescPtrs < gUsbData->fpMemBlockStart) ||
                    ((UINT8*)(DescPtrs + sizeof(UHCI_DESC_PTRS)) > MemBlockEnd)) {
                    return USB_ERROR;
                }
                if (((UINT8*)DescPtrs->RootHubQh < gUsbData->fpMemBlockStart) ||
                    ((UINT8*)(DescPtrs->RootHubQh + sizeof(UHCI_QH)) > MemBlockEnd)) {
                    return USB_ERROR;
                }

                if (((UINT8*)DescPtrs->RootHubQh->FirstTd < gUsbData->fpMemBlockStart) ||
                    ((UINT8*)(DescPtrs->RootHubQh->FirstTd + sizeof(UHCI_TD)) > MemBlockEnd)) {
                    return USB_ERROR;
                }

                if (((UINT8*)DescPtrs->RootHubQh->CurrentTd < gUsbData->fpMemBlockStart) ||
                    ((UINT8*)(DescPtrs->RootHubQh->CurrentTd + sizeof(UHCI_TD)) > MemBlockEnd)) {
                    return USB_ERROR;
                }

				DescPtrs->RootHubQh->FirstTd->dCSReload =
						UHCI_TD_INTERRUPT_ON_COMPLETE |
						UHCI_TD_ONE_ERROR |
						UHCI_TD_ACTIVE;
				DescPtrs->RootHubQh->FirstTd->bActiveFlag = 1;
				DescPtrs->RootHubQh->FirstTd->dControlStatus =
						DescPtrs->RootHubQh->FirstTd->dCSReload;
                DescPtrs->RootHubQh->pElementPtr = 
                        (UINT32)(UINTN)DescPtrs->RootHubQh->FirstTd;
                DescPtrs->RootHubQh->CurrentTd = 
                        DescPtrs->RootHubQh->FirstTd;
                DescPtrs->RootHubQh->ActiveFlag = TRUE;;
				gUsbData->RootHubHcStruc = gUsbData->HcTable[i];
			}
		}
	}

    return USB_SUCCESS;
}

/**
    This function will check whether the QH is completed if so, it will call
    the call back routine associated with the TDs present in the QH

    @param 
        HcStruc     HCStruc structure
        Qh          Pointer to the QH

    @retval 
        USB_SUCCESS or USB_ERROR

**/

UINT8
UhciProcessQh(
    HC_STRUC    *HcStruc,
    UHCI_QH     *Qh
)
{
    UHCI_TD     *Td;
    UINT16      Length;
    EFI_STATUS	Status;
 
    if (Qh == NULL) {
        return USB_ERROR;
    }

    if (Qh->ActiveFlag == FALSE) {
        return USB_SUCCESS;
    }

    if (Qh->FirstTd == NULL) {
        return USB_SUCCESS;
    }

    if (Qh->CurrentTd == NULL) {
        Qh->CurrentTd = Qh->FirstTd;
    }

    Td = Qh->CurrentTd;
    while (Td) {
        Qh->CurrentTd = Td;
        Qh->DataToggle = Td->dToken & UHCI_TD_DATA_TOGGLE ? 1 : 0;

        if (Td->dControlStatus & UHCI_TD_ACTIVE) {
            return USB_ERROR;
        }

        Length = (UINT16)((Td->dControlStatus + 1) & UHCI_TD_ACTUAL_LENGTH);
        Qh->BytesTransferred += Length;

        if (Td->dControlStatus & UHCI_TD_STATUS_FIELD) {
            break;
        }

        Qh->DataToggle ^= 1;
        Qh->ShortPacketDetected = (Length < (((Td->dToken >> 21) + 1) & 0x7FF));

        if (Qh->ShortPacketDetected) {
            if (Qh->Type == Control) {
                while (!(Td->pLinkPtr & UHCI_TERMINATE)) {
                    Td = (UHCI_TD*)((UINTN)Td->pLinkPtr & UHCI_POINTER_MASK);
                }
                Qh->pElementPtr = (UINT32)(UINTN)Td;
                continue;
            }
            break;
        }
 
        Td = Td->pLinkPtr & UHCI_TERMINATE ?
                NULL : (UHCI_TD*)(Td->pLinkPtr & UHCI_POINTER_MASK);
    }

    Qh->ActiveFlag = FALSE;
    
    if (Qh->CallBackIndex == 0) {
        return USB_SUCCESS;
    }
    
    if ((Qh->CallBackIndex) && (Qh->CallBackIndex <= MAX_CALLBACK_FUNCTION)) {
        if (gUsbData->aCallBackFunctionTable[Qh->CallBackIndex - 1 + CALLBACK_FUNCTION_START]) {
            if ((gUsbData->aCallBackFunctionTable[Qh->CallBackIndex - 1 + CALLBACK_FUNCTION_START] != UhciRepeatQhCallback) && 
                (gUsbData->aCallBackFunctionTable[Qh->CallBackIndex - 1 + CALLBACK_FUNCTION_START] != UhciRootHubQhCallBack)) {
                Status = UsbDevInfoValidation(Qh->DevInfoPtr);
                if (EFI_ERROR(Status)) {
                    return USB_ERROR;
                }
            }
            (*gUsbData->aCallBackFunctionTable[Qh->CallBackIndex - 1 + CALLBACK_FUNCTION_START])(
                    HcStruc,
                    (DEV_INFO*)Qh->DevInfoPtr,
                    (UINT8*)Qh,
                    0,
                    0);
        }
    }

    return USB_SUCCESS;
}

/**
    This function will check whether the TD is completed if so, it will call
    the call back routine associated with this TD

    @param 
        HcStruc     HCStruc structure
        Td          Pointer to the TD

    @retval 
        USB_SUCCESS or USB_ERROR

    @note  
  For any TD whose ActiveFlag is TRUE and its ControlStatus bit 23 is clear
  (completed), process the TD by calling its callback routine, if one is present.

**/

UINT8
UhciProcessTd(
    HC_STRUC    *HcStruc,
    UHCI_TD     *Td
)
{
    UINT8       DevAddr;
	DEV_INFO    *DevInfo;

    if (Td == NULL) {   // Check for NULL
        return USB_ERROR;
    }

    if (Td->bActiveFlag == 0) {
        return USB_SUCCESS;
    }

    if (Td->dControlStatus & UHCI_TD_ACTIVE) {
        return USB_ERROR;
    }

    Td->bActiveFlag = 0;
    if ((Td->bCallBackIndex) && (Td->bCallBackIndex <= MAX_CALLBACK_FUNCTION)) {
        //
        // Get the device address from the completed TD
        //
        DevAddr = (UINT8)(((Td->dToken) >> 8) & 0x7F);
        DevInfo = USB_GetDeviceInfoStruc(USB_SRCH_DEV_ADDR, 0,
                        DevAddr, HcStruc);

        if (gUsbData->aCallBackFunctionTable[Td->bCallBackIndex - 1 + CALLBACK_FUNCTION_START]) {
            (*gUsbData->aCallBackFunctionTable[Td->bCallBackIndex - 1 + CALLBACK_FUNCTION_START])(
                    HcStruc,
                    DevInfo,
                    (UINT8*)Td,
                    0,
                    0);
        }
    }

    return USB_SUCCESS;
}

/**
    This function will parse through frame list to find completed QH/TD
    and invoke corresponding call back routine

    @param 
        HcStruc     HCStruc structure

    @retval 
        USB_SUCCESS or USB_ERROR

    @note  
  For any TD whose ActiveFlag is TRUE and its ControlStatus bit 23 is clear
  (completed), process the TD by calling its call back routine, if one is present.

**/

UINT8
UhciProcessFrameList (
    HC_STRUC    *HcStruc
)
{
    UINT32  ListPtr;
    EFI_STATUS  Status = EFI_SUCCESS;

    ListPtr = HcStruc->fpFrameList[0];

    while (!(ListPtr & UHCI_TERMINATE)) {
        if (ListPtr & UHCI_QUEUE_HEAD) {
#if !USB_RT_DXE_DRIVER
            Status = AmiValidateMemoryBuffer((VOID*)ListPtr, sizeof(UHCI_QH));
            if (EFI_ERROR(Status)) {
                return USB_ERROR;
            }
#endif
            UhciProcessQh(HcStruc, (UHCI_QH*)(ListPtr & UHCI_POINTER_MASK));
            ListPtr = ((UHCI_QH*)(ListPtr & UHCI_POINTER_MASK))->pLinkPtr;
        } else {
#if !USB_RT_DXE_DRIVER
            Status = AmiValidateMemoryBuffer((VOID*)ListPtr, sizeof(UHCI_TD));
            if (EFI_ERROR(Status)) {
               return USB_ERROR;
            }
#endif
            UhciProcessTd(HcStruc, (UHCI_TD*)(ListPtr & UHCI_POINTER_MASK));
            ListPtr = ((UHCI_TD*)(ListPtr & UHCI_POINTER_MASK))->pLinkPtr;
        }
    }
	return USB_SUCCESS;
}

/**
    This function is called when TD256ms completes a transaction. This TD runs
    a dummy interrupt transaction to a non-existant device address for the
    purpose of generating a periodic timeout interrupt.  This periodic interrupt
    may be used to check for new devices on the root hub etc.

    @param 
        fpHCStruc   Pointer to the HCStruc structure
        fpDevInfo   NULL (pDevInfo is not valid)
        fpTD        Pointer to the TD that completed
        fpBuffer    Not used

    @retval 
        USB_SUCCESS or USB_ERROR

**/

UINT8
UhciRootHubQhCallBack(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       *Qh,
    UINT8       *Buffer,
    UINT16      DataLength
)
{
    UINT8       Index;
    HC_STRUC    *Hc;
    UHCI_QH     *RootHubQh;
    UINT16      PortSc;

	//
	// First deactivate the TDRootHub so this callback function will not get
	// reentered.
	//
	if (Qh != NULL) {
        RootHubQh = (UHCI_QH*)Qh;
        RootHubQh->FirstTd->bActiveFlag = 0;
    }

    for (Index = 0; Index < gUsbData->HcTableCount; Index++) {
        Hc = gUsbData->HcTable[Index];
        if (Hc == NULL) {
            continue;
        }

        if (Hc->bHCType != USB_HC_UHCI) {	// Process for UHCI only
			continue;
        }

		if (!(Hc->dHCFlag & HC_STATE_RUNNING)) {
			continue;
		}

		//
		// Check whether the controller is still under BIOS control
		// Read the frame list base address and compare with stored value
		//
		if ((HcDwordReadHcIo(HcStruc, UHCI_FRAME_LIST_BASE) & 0xFFFFF000) 
			!= (UINT32)Hc->fpFrameList) {
			continue;
		}

		//
		// Check whether USB host controllers are accessible to aVOID system 
		// hang in ports enumeration.
		//
		if (HcByteReadHcIo(HcStruc, 0) == 0xFF) {
			continue;
		}

		//
		// Check whether enumeration is already began
		//
		if (gUsbData->bEnumFlag == FALSE) {
			gUsbData->bEnumFlag = TRUE;

			//
			// Mask the Host Controller interrupt so the ISR does not get re-entered due
			// to an IOC interrupt from any TDs that complete in frames while we are
			// configuring a new device that has just been plugged in.
			//
			// Disable IOC, timeout & CRC interrupt
			//
			HcWordWriteHcIo(HcStruc, UHCI_INTERRUPT_ENABLE, 0);
		
			//
			// Process Port#1 and clear Port#1 status bit
			//
			PortSc = HcWordReadHcIo(HcStruc, UHCI_PORT1_CONTROL);
			if (PortSc & UHC_CONNECT_STATUS_CHANGE) {
                // The interval with a minimum duration of 100 ms when 
                // the USB System Software is notified of a connection detection.
                if (PortSc & UHC_CONNECT_STATUS) {
                    FixedDelay(USB_PORT_CONNECT_STABLE_DELAY_MS * 1000);
                }
                
				USBCheckPortChange(Hc, Hc->bHCNumber | BIT7, 1);
				HcWordWriteHcIo(HcStruc, UHCI_PORT1_CONTROL, 
                    HcWordReadHcIo(HcStruc, UHCI_PORT1_CONTROL));
			}
		
			//
			// Process Port#2 and clear Port#2 status bit
			//
			PortSc = HcWordReadHcIo(HcStruc, UHCI_PORT2_CONTROL);
			if (PortSc & UHC_CONNECT_STATUS_CHANGE) {
                // The interval with a minimum duration of 100 ms when 
                // the USB System Software is notified of a connection detection.
                if (PortSc & UHC_CONNECT_STATUS) {
                    FixedDelay(USB_PORT_CONNECT_STABLE_DELAY_MS * 1000);
                }
				USBCheckPortChange(Hc, Hc->bHCNumber | BIT7, 2);
				HcWordWriteHcIo(HcStruc, UHCI_PORT2_CONTROL, 
                    HcWordReadHcIo(HcStruc, UHCI_PORT2_CONTROL));
			}

			//
			// Renable interrupts from the host controller
			// Enable IOC, timeout & CRC interrupt
			//
			HcWordWriteHcIo(HcStruc, UHCI_INTERRUPT_ENABLE, (UINT16)(UHC_IOC_ENABLE));

			gUsbData->bEnumFlag = FALSE;
		}
    }
    if (Qh != NULL) {
        //
        // Reactivate the TdRootHub
        //
        RootHubQh->FirstTd->dControlStatus	= RootHubQh->FirstTd->dCSReload;
        RootHubQh->FirstTd->bActiveFlag = 1;
        RootHubQh->pElementPtr = (UINT32)(UINTN)RootHubQh->FirstTd;
        RootHubQh->CurrentTd = RootHubQh->FirstTd;
        RootHubQh->ActiveFlag = TRUE;
    }

    return  USB_SUCCESS;
}


/**
    This function is called when TdRepeat completes a transaction.  This TD
    runs a dummy interrupt transaction to a non-existant device address for
    the purpose of generating a periodic timeout interrupt which in turn is
    used to generate keyboard repeat.

    @param 
        fpHCStruc   Pointer to the HCStruc structure
        fpDevInfo   NULL (pDevInfo is not valid)
        fpTD        Pointer to the TD that completed
        fpBuffer    Not used

    @retval 
        USB_SUCCESS or USB_ERROR

**/

UINT8
UhciRepeatQhCallback(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       *Qh,
    UINT8       *Buffer,
    UINT16      DataLength
)
{
    UHCI_QH *RepeatQh = (UHCI_QH*)Qh;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_8, "Processing Repeat TD ...\n");
    //
    // First deactivate the TdRepeat so this callback function will not get
    // re-entered.
    //
    RepeatQh->FirstTd->bActiveFlag = 0;

#if USB_DEV_KBD
    USBKBDPeriodicInterruptHandler(HcStruc);
#endif
    //
    // Reactivate the TdRepeat
    //
    if (RepeatQh->FirstTd->dCSReload & UHCI_TD_ACTIVE) {
        RepeatQh->FirstTd->dControlStatus = RepeatQh->FirstTd->dCSReload;
        RepeatQh->FirstTd->bActiveFlag = 1;
        RepeatQh->pElementPtr = (UINT32)(UINTN)RepeatQh->FirstTd;
        RepeatQh->CurrentTd = RepeatQh->FirstTd;
        RepeatQh->ActiveFlag = TRUE;
	}

    return  USB_SUCCESS;
}


/**
    This function is called when a polling TD from the TD pool completes an
    interrupt transaction to its assigned device.
    This routine should process any data in the TD's data buffer, handle any
    errors, and then copy the TD's CSReloadValue field into its control status
    field to put the TD back into service.

    @param 
        fpHCStruc   Pointer to the HCStruc structure
        fpDevInfo   NULL (pDevInfo is not valid)
        fpTD        Pointer to the TD that completed

    @retval 
        USB_SUCCESS or USB_ERROR

**/

UINT8
UhciPollingQhCallback(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       *Qh,
    UINT8       *Buffer,
    UINT16      DataLength
)
{
    UHCI_QH     *PollQh = (UHCI_QH*)Qh;   

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_8, "Processing polling TD ...\n");

	UsbUpdateDataToggle(DevInfo, DevInfo->IntInEndpoint, 
			PollQh->DataToggle);

    if ((PollQh->CurrentTd->dControlStatus & UHCI_TD_STATUS_FIELD) == 0) {
        if ((DevInfo->bCallBackIndex) && (DevInfo->bCallBackIndex <= MAX_CALLBACK_FUNCTION)) {
            if (gUsbData->aCallBackFunctionTable[DevInfo->bCallBackIndex - 1 + CALLBACK_FUNCTION_START]) {
                (*gUsbData->aCallBackFunctionTable[DevInfo->bCallBackIndex - 1 + CALLBACK_FUNCTION_START])(
                        HcStruc,
                        DevInfo,
                        Qh,
                        DevInfo->fpPollDataBuffer,
                        PollQh->BytesTransferred);
            }
        }
    }

	UhciActivateTds(PollQh->FirstTd, PollQh->DataToggle);

    PollQh->pElementPtr = (UINT32)(UINTN)PollQh->FirstTd;
    PollQh->CurrentTd = PollQh->FirstTd;
    PollQh->BytesTransferred = 0;
    PollQh->ActiveFlag = TRUE;

    return  USB_SUCCESS;
}

/**
    This function check whether HC is halted.

**/

VOID
UhciInitQh (
    UHCI_QH *Qh
)
{
    Qh->pLinkPtr = UHCI_TERMINATE;
    Qh->pElementPtr = UHCI_TERMINATE;
    Qh->CurrentTd = NULL;
    Qh->DataToggle = 0;
    Qh->BytesTransferred = 0;
    Qh->ShortPacketDetected = FALSE;
    Qh->FirstTd = NULL;
    Qh->Interval = 0;
    Qh->CallBackIndex = 0;
    Qh->ActiveFlag = FALSE;
    Qh->DevInfoPtr = NULL;
}

/**
    This function check whether HC is halted.

**/

BOOLEAN
UhciIsHalted (
	HC_STRUC	*HcStruc
)
{
	return (HcByteReadHcIo(HcStruc, UHCI_STATUS_REG) & UHC_HC_HALTED) == UHC_HC_HALTED;
}

/**
    This function calculates the polling rate.

**/

UINT8
UhciTranslateInterval(
    UINT8   Interval
)
{
    UINT8  BitCount = 0;

    // The Interval value should be from 1 to 255
    ASSERT(Interval >= 1 && Interval <= 255);

    for (BitCount = 0; Interval != 0; BitCount++) {
        Interval >>= 1;
    }
    return (1 << (BitCount - 1));
}

/**

**/

UHCI_TD*
UhciAllocGeneralTds (
	IN UINT8		DeviceAddr,
	IN BOOLEAN		LowSpeed,
	IN UINT8		PacketId,
	IN UINT8        EndpointAddr,
	IN UINT16		MaxPacket,
	IN BOOLEAN		ShortPacket,
    IN OUT UINTN    *BufferAddr,
    IN OUT UINT32   *Length,
    IN OUT UINT8    *DataToggle
)
{
	UINT16		NumTds = 0;
	UHCI_TD		*FirstTd = NULL;
	UHCI_TD		*Td = NULL;
	UINTN		Address = *BufferAddr;
	UINT32		BytesRemaining = *Length;
	UINT8		Toggle = *DataToggle;
	UINT16		MaxLen = 0;

	if (BytesRemaining == 0) {
		return NULL;
	}

	NumTds = BytesRemaining / MaxPacket;
	if (BytesRemaining % MaxPacket) {
		NumTds++;
	}

	FirstTd = USB_MemAlloc(GET_MEM_BLK_COUNT(NumTds * sizeof(UHCI_TD)));
	if (FirstTd == NULL) {
		return NULL;
	}
	
	for (Td = FirstTd;;) {
		MaxLen = BytesRemaining > MaxPacket ? MaxPacket : BytesRemaining;

		Td->pLinkPtr = UHCI_TERMINATE;
		Td->dToken = (UINT32)PacketId | ((UINT32)DeviceAddr << 8) | 
						((UINT32)EndpointAddr << 15) | ((MaxLen - 1) << 21);
		if (Toggle) {
			Td->dToken |= UHCI_TD_DATA_TOGGLE;
		}

		Td->pBufferPtr = (UINT32)Address;
		Td->dCSReload = UHCI_TD_THREE_ERRORS | UHCI_TD_ACTIVE;
		if (LowSpeed) {
			Td->dCSReload |= UHCI_TD_LOW_SPEED_DEVICE;
		}
		if (ShortPacket) {
			Td->dCSReload |= UHCI_TD_SHORT_PACKET_DETECT;
		}
		Td->dControlStatus = Td->dCSReload;
		Td->bActiveFlag = 1;

		BytesRemaining -= MaxLen;
		Address += MaxLen;
		Toggle ^= 1;

		if (BytesRemaining == 0) {
			break;
		}

		Td->pLinkPtr = (UINT32)(((UINTN)Td + sizeof(UHCI_TD)) | UHCI_VERTICAL_FLAG);
		Td = (UHCI_TD*)((UINTN)Td->pLinkPtr & UHCI_POINTER_MASK);
	}

	Td->dCSReload |= UHCI_TD_INTERRUPT_ON_COMPLETE;
	Td->dControlStatus = Td->dCSReload;

	*Length = BytesRemaining;
	*BufferAddr = Address;
	*DataToggle = Toggle;
	
	return FirstTd;
}

/**

**/

VOID
UhciFreeTds (
	IN UHCI_TD	*FirstTd
)
{
	UHCI_TD	*Td = FirstTd;
	UINT16	NumTds = 0;

	if (FirstTd == NULL) {
		return;
	}

	while (Td) {
		NumTds++;
        Td = Td->pLinkPtr & UHCI_TERMINATE ?
                NULL : (UHCI_TD*)((UINTN)Td->pLinkPtr & UHCI_POINTER_MASK);
	}

	USB_MemFree(FirstTd, GET_MEM_BLK_COUNT(NumTds * sizeof(UHCI_TD)));
}

/**

**/

VOID
UhciActivateTds (
	IN UHCI_TD	*FirstTd,
	IN UINT8	DataToggle
)
{
	UHCI_TD	*Td = FirstTd;
	UINT8	Toogle = DataToggle;

	if (FirstTd == NULL) {
		return;
	}

	while (Td) {
		Td->dToken &= ~UHCI_TD_DATA_TOGGLE;
		if (Toogle) {
			Td->dToken |= UHCI_TD_DATA_TOGGLE;
		}
    	Td->dControlStatus = Td->dCSReload;
		Td->bActiveFlag = 1;

		Toogle ^= 1;
        Td = Td->pLinkPtr & UHCI_TERMINATE ?
                NULL : (UHCI_TD*)((UINTN)Td->pLinkPtr & UHCI_POINTER_MASK);
	}
}

#endif

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
