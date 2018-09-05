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

/** @file Ehci.c
    AMI USB EHCI support

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "AmiUsb.h"
#include "UsbKbd.h"
#include <Library/BaseMemoryLib.h>
#if !USB_RT_DXE_DRIVER
#include <Library/AmiBufferValidationLib.h>
#endif
#include <UsbDevDriverElinks.h>

#if EHCI_SUPPORT

UINT8   EHCI_Start (HC_STRUC*);
UINT8   EHCI_Stop (HC_STRUC*);
UINT8   EHCI_EnumeratePorts (HC_STRUC*);
UINT8   EHCI_DisableInterrupts (HC_STRUC*);
UINT8   EHCI_EnableInterrupts (HC_STRUC*);
UINT8   EHCI_ProcessInterrupt(HC_STRUC*);
UINT32  EHCI_GetRootHubStatus (HC_STRUC*, UINT8, BOOLEAN);
UINT8   EHCI_DisableRootHub (HC_STRUC*,UINT8);
UINT8   EHCI_EnableRootHub (HC_STRUC*,UINT8);
UINT16  EHCI_ControlTransfer (HC_STRUC*,DEV_INFO*,UINT16,UINT16,UINT16,UINT8*,UINT16);
UINT32  EHCI_BulkTransfer (HC_STRUC*,DEV_INFO*,UINT8,UINT8*,UINT32);
UINT32  EHCI_IsocTransfer (HC_STRUC*,DEV_INFO*,UINT8,UINT8*,UINT32, UINT8*);
UINT16  EHCI_InterruptTransfer (HC_STRUC*, DEV_INFO*, UINT8, UINT16, UINT8*, UINT16);
UINT8   EHCI_DeactivatePolling (HC_STRUC*,DEV_INFO*);
UINT8   EHCI_ActivatePolling (HC_STRUC*,DEV_INFO*);
UINT8   EHCI_DisableKeyRepeat (HC_STRUC*);
UINT8   EHCI_EnableKeyRepeat (HC_STRUC*);
UINT8   EHCI_ResetRootHub (HC_STRUC*,UINT8);
UINT8   EHCI_GlobalSuspend (HC_STRUC*);	//(EIP54018+)
UINT8   EhciSmiControl(HC_STRUC*, BOOLEAN);

UINT8   EHCIResetHC(HC_STRUC*);
UINT8   EHCIInitializePeriodicSchedule(HC_STRUC*, UINT32);
UINT8   EHCIProgramLegacyRegisters(HC_STRUC*, UINT8);
UINT8   EHCIStartAsyncSchedule(HC_STRUC*);
UINT8   EHCIStopAsyncSchedule(HC_STRUC*);
UINT8   EHCIStartPeriodicSchedule(HC_STRUC*);
UINT8   EHCIStopPeriodicSchedule(HC_STRUC*);
UINT8   EHCIProcessQH(HC_STRUC*, EHCI_QH*);
VOID    EHCIProcessPeriodicList(HC_STRUC*);
VOID    EHCIInitializeQueueHead (EHCI_QH*);
VOID    EHCISetQTDBufferPointers(EHCI_QTD*, UINT8*, UINT32);
UINT16  EHCIWaitForTransferComplete(HC_STRUC*, EHCI_QH*,DEV_INFO* );
UINT8   EhciAddPeriodicQh(HC_STRUC*,EHCI_QH*);
VOID    EhciAddIsochTDs(HC_STRUC*);
UINT8   EhciRemovePeriodicQh(HC_STRUC*,EHCI_QH*);
VOID    ProcessOwnerShipChangeSMI(HC_STRUC*);
VOID    ProcessSmiChangeToEHCD(HC_STRUC*);
VOID    ProcessSmiChangeToBIOS(HC_STRUC*);
UINT8   EHCIGetLegacySupportOffset(HC_STRUC*, UINT16);
//VOID    EHCIRemoveQHFromAsyncList(HC_STRUC*, EHCI_QH*);
UINT8	EhciPollingTDCallback(HC_STRUC*, DEV_INFO*, UINT8*, UINT8*, UINT16);
UINT8	EhciRepeatTDCallback(HC_STRUC*, DEV_INFO*, UINT8*, UINT8*, UINT16);

UINT32  DwordReadMem(UINT32, UINT16);

UINT32	HcReadPciReg(HC_STRUC*, UINT32);
VOID	HcWritePciReg(HC_STRUC*, UINT32, UINT32);
UINT32	HcReadHcMem(HC_STRUC*, UINT32);
VOID	HcWriteHcMem(HC_STRUC*, UINT32, UINT32);
UINT32	HcReadOpReg(HC_STRUC*, UINT32);
VOID	HcWriteOpReg(HC_STRUC*, UINT32, UINT32);
VOID	HcClearOpReg(HC_STRUC*, UINT32, UINT32);
VOID	HcSetOpReg(HC_STRUC*, UINT32, UINT32);
UINT32	EhciReadDebugReg(HC_STRUC*, UINT8, UINT32);
VOID*   EhciMemGetPhyAddr(HC_STRUC*, VOID*);
VOID*   EhciMemGetHostAddr(HC_STRUC*, VOID*);
VOID*	EhciMemAlloc(HC_STRUC*, UINT16);
VOID	EhciMemFree(HC_STRUC*, VOID*, UINT16);
UINT8	HcDmaMap(HC_STRUC*, UINT8, UINT8*, UINT32, UINT8**, VOID**);
UINT8	HcDmaUnmap(HC_STRUC*, VOID*);
BOOLEAN EhciIsHalted(HC_STRUC*);
UINT16  EhciTranslateInterval(UINT8, UINT8);

UINT8   USBCheckPortChange (HC_STRUC*, UINT8, UINT8);
UINT8   USBLogError(UINT16);
UINT8	UsbGetDataToggle(DEV_INFO*,UINT8);
VOID	UsbUpdateDataToggle(DEV_INFO*, UINT8, UINT8);

VOID    USB_InitFrameList (HC_STRUC*, UINT32);
VOID    FixedDelay(UINTN);

VOID*   USB_MemAlloc (UINT16);
UINT8   USB_MemFree (VOID _FAR_ *, UINT16);
UINT8   USB_DisconnectDevice (HC_STRUC*, UINT8, UINT8);
DEV_INFO*   USB_GetDeviceInfoStruc(UINT8, DEV_INFO*, UINT8, HC_STRUC*);
UINT8   USB_StopDevice (HC_STRUC*,  UINT8, UINT8);
UINT32  USB_GetHubPortStatus(HC_STRUC*, UINT8, UINT8, BOOLEAN);
UINT8	USB_InstallCallBackFunction (CALLBACK_FUNC);
VOID	USBKeyRepeat(HC_STRUC*, UINT8);

#if USB_DEV_KBD
VOID    USBKBDPeriodicInterruptHandler(HC_STRUC*);
#endif


extern  USB_GLOBAL_DATA     *gUsbData;
extern  BOOLEAN gCheckUsbApiParameter;

typedef	VOID (EHCI_GET_ROOT_HUB_PORT_SPEED_FUNS)( 
    HC_STRUC    *HcStruc,
    UINT8       PortNum,
    UINT8       *PortSpeed
);
extern EHCI_GET_ROOT_HUB_PORT_SPEED_FUNS EHCI_GET_ROOT_HUB_PORT_SPEED_ELINK_LIST EndOfGetRootHubPortSpeedList;
EHCI_GET_ROOT_HUB_PORT_SPEED_FUNS*EhciGetRootHubPortSpeedFunctions[] = {
    EHCI_GET_ROOT_HUB_PORT_SPEED_ELINK_LIST NULL};

/**
    This function fills the host controller driver
    routine pointers

    @param fpHCDHeader     Ptr to the host controller header structure

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
EHCI_FillHCDEntries(
    HCD_HEADER *HcdHeader
)
{
    HcdHeader->pfnHCDStart                = EHCI_Start;
    HcdHeader->pfnHCDStop                 = EHCI_Stop;
    HcdHeader->pfnHCDEnumeratePorts       = EHCI_EnumeratePorts;
    HcdHeader->pfnHCDDisableInterrupts    = EHCI_DisableInterrupts;
    HcdHeader->pfnHCDEnableInterrupts     = EHCI_EnableInterrupts;
    HcdHeader->pfnHCDProcessInterrupt     = EHCI_ProcessInterrupt;
    HcdHeader->pfnHCDGetRootHubStatus     = EHCI_GetRootHubStatus;
    HcdHeader->pfnHCDDisableRootHub       = EHCI_DisableRootHub;
    HcdHeader->pfnHCDEnableRootHub        = EHCI_EnableRootHub;
    HcdHeader->pfnHCDControlTransfer      = EHCI_ControlTransfer;
    HcdHeader->pfnHCDBulkTransfer         = EHCI_BulkTransfer;
    HcdHeader->pfnHCDIsocTransfer         = EHCI_IsocTransfer;
    HcdHeader->pfnHCDInterruptTransfer    = EHCI_InterruptTransfer;
    HcdHeader->pfnHCDDeactivatePolling    = EHCI_DeactivatePolling;
    HcdHeader->pfnHCDActivatePolling      = EHCI_ActivatePolling;
    HcdHeader->pfnHCDDisableKeyRepeat     = EHCI_DisableKeyRepeat;
    HcdHeader->pfnHCDEnableKeyRepeat      = EHCI_EnableKeyRepeat;
    HcdHeader->pfnHCDEnableEndpoints      = USB_EnableEndpointsDummy;
    HcdHeader->pfnHCDInitDeviceData       = USB_InitDeviceDataDummy;
    HcdHeader->pfnHCDDeinitDeviceData     = USB_DeinitDeviceDataDummy;
    HcdHeader->pfnHCDResetRootHub         = EHCI_ResetRootHub;
    HcdHeader->pfnHCDClearEndpointState	  = 0;	//(EIP54283+)
    HcdHeader->pfnHCDGlobalSuspend        = EHCI_GlobalSuspend;	//(EIP54018+)
    HcdHeader->pfnHCDSmiControl           = EhciSmiControl;

    USB_InstallCallBackFunction(EhciRepeatTDCallback);
    USB_InstallCallBackFunction(EhciPollingTDCallback);

    return  USB_SUCCESS;
}


/**
    This routine locates EHCI debug port and determines whether
    or not the debug port is initialized and being used by other
    agents. If so, the global flag will be set to instruct the
    EHCI runtime routines about debug port presence and prevent
    any unwanted reset/reconfiguration of this port.

    @param HcStruc   Ptr to the host controller structure

    @retval fpHCStruc->DebugPort is updated if Debug Port is active on
        this controller; otherwise it will remain 0.

**/

VOID
EhciIsolateDebugPort(
    HC_STRUC *HcStruc
)
{
    UINT32 HcsParams = HcStruc->dHCSParams;   // Host Controller Structural Parameters
    UINT8  DebugPortNo;
    UINT32 NextCap;
	UINT8  DebugPortBarIndex;
    UINT16 DebugPortOffset;

    //
    // Locate debug port by looking at the PCI capabilities
    //
    DebugPortNo = (UINT8)((HcsParams & (EHCI_DEBUG_N)) >> 20);

    //ASSERT(DebugPortNo); // No debug port implemented
    HcStruc->DebugPort = 0;
    if (DebugPortNo == 0) return;

    ASSERT(DebugPortNo <= (UINT8)(HcsParams & (EHCI_N_PORTS)));    // Invalid debug port number
    if (DebugPortNo > (UINT8)(HcsParams & (EHCI_N_PORTS))) return;

    //
    // Check whether device implements Capability list that starts at register 0x34
    //
    if (!(HcReadPciReg(HcStruc, 4) & BIT20)) {
        //ASSERT(FALSE);  // Capabilities list is not implemented
        return;
    }

    //
    // Find the beginning of Debug Port registers block
    //
    for (NextCap = HcReadPciReg(HcStruc, 0x34);
            (UINT8)NextCap > 0;
    )
    {
        NextCap = HcReadPciReg(HcStruc, (UINT8)NextCap);
        if ((UINT8)NextCap == 0xA) break;   // Debug port capability found
        NextCap >>= 8;
    }
    if ((UINT8)NextCap == 0) {
        //ASSERT(FALSE);  // Debug capabilities not found
        return;
    }
	DebugPortBarIndex = (UINT8)((NextCap >> 29) - 1);
    DebugPortOffset = (UINT16)((NextCap >> 16) & 0x1FFF);
    ASSERT(DebugPortBarIndex >= 0 && DebugPortBarIndex <= 5); // Wrong BAR
    if (!(DebugPortBarIndex >= 0 && DebugPortBarIndex <= 5)) return;
    //
    // See whether Debug Port is acquired by other software
    //
	if (EhciReadDebugReg(HcStruc, DebugPortBarIndex, DebugPortOffset) & BIT28) {
	    HcStruc->DebugPort = DebugPortNo;
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "EHCI HC Debug Port #%d enabled.\n", DebugPortNo);
	}
}


/**
    This API function is called to start a EHCI host controller.
    The input to the routine is the pointer to the HC structure
    that defines this host controller

    @param HcStruc   Ptr to the host controller structure

    @retval Status: USB_SUCCESS = Success
        USB_ERROR = Failure

**/

UINT8
EHCI_Start(
    HC_STRUC* HcStruc
)
{
    UINT32  Temp;
//#if EHCI_ASYNC_BELL_SUPPORT
//    EHCI_QH *QhAsyncXfer;
//#endif
	EHCI_QH     *QhRepeat = NULL;
	EHCI_QTD    *QtdRepeat = NULL;
	UINT32	i;							//(EIP55960+)
	BOOLEAN	SetPortPower = FALSE;
    UINT16  PortReg;
    EHCI_DESC_PTRS      *DescPtr = NULL;
    EFI_STATUS  EfiStatus = EFI_SUCCESS;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

/*
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Enabling MEM/BM for EHCI HC %02X\n", HcStruc->wBusDevFuncNum);

    //
    // Enable IO access and Bus Mastering
    //
    WordWritePCIConfig((UINT16)HcStruc->wBusDevFuncNum, 4, BIT1 + BIT2);
*/
    //
    // Get memory base address of the HC and store it in the HCStruc
    //
    HcStruc->BaseAddress = HcReadPciReg(HcStruc, USB_MEM_BASE_ADDRESS) & 0xFFFFFFF0;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "EHCI HC Mem Addr: %X\n", HcStruc->BaseAddress);
    
#if !USB_RT_DXE_DRIVER
    EfiStatus = AmiValidateMmioBuffer((VOID*)HcStruc->BaseAddress, HcStruc->BaseAddressSize);
    if (EFI_ERROR(EfiStatus)) {
        USB_DEBUG(DEBUG_ERROR, 3, "Usb Mmio address is invalid, it is in SMRAM\n");
        return USB_ERROR;
    }
    EfiStatus = AmiValidateMmioBuffer((VOID*)HcStruc->fpFrameList, 0x1000);
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }
#endif

    //
    // Get the number of ports supported by the host controller (Offset 4)
    // and store it in HCStruc
    //
    HcStruc->dHCSParams = HcReadHcMem(HcStruc, EHCI_HCSPARAMS);
    HcStruc->bNumPorts = (UINT8)(HcStruc->dHCSParams & EHCI_N_PORTS);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "EHCI HC Number of ports: %d\n", HcStruc->bNumPorts);

	EhciIsolateDebugPort(HcStruc);

    //
    // Read the Capability Registers Length to find the Offset address for the
    // beginning of the operational registers
    //
    HcStruc->bOpRegOffset = (UINT8)HcReadHcMem(HcStruc, EHCI_VERCAPLENGTH);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "EHCI HC Operational Registers Offset: %d\n", HcStruc->bOpRegOffset);

    //
    // Read and store the HCCPARAMS value
    //
    HcStruc->dHCCParams = HcReadHcMem(HcStruc, EHCI_HCCPARAMS);
    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "EHCI HC HCPARAMS: %x\n", gUsbData->dHCCParams);

    //
    // Get PCI register offset for the legacy support in EHCI controller
    // and store it in HC_STRUC
    //
    HcStruc->bExtCapPtr = EHCIGetLegacySupportOffset(
                        HcStruc,
                        HcStruc->wBusDevFuncNum);

#if EHCI_64BIT_DATA_STRUCTURE == 0
    //
    // 64bit data structures are not enabled. So check whether this host controller
    // needs 64bit data structure or not.
    //
    if (HcStruc->dHCCParams & EHCI_64BIT_CAP) {
        //
        // Engineer has to enable the 64bit capability. Post an error message
        //
        USBLogError(ERRUSB_EHCI_64BIT_DATA_STRUC);
        ASSERT(FALSE);

        //
        // Connect all ports to the classic host controller
        //
        HcClearOpReg(HcStruc, EHCI_CONFIGFLAG, BIT0);
        return  USB_ERROR;
    }
#endif

#if	HIDE_USB_HISPEED_SUPPORT_SETUP_QUESTION == 0
	if ((gUsbData->UsbHiSpeedSupport == 0) && ((HcStruc->dHCSParams & EHCI_N_CC) != 0)) {
		HcClearOpReg(HcStruc, EHCI_CONFIGFLAG, BIT0);
		return USB_ERROR;
	}
#endif

//----------------------------------------------------------------------------
// Note: after this point any access to the operational registers is through
// the macros EHCI_DWORD_READ_MEM and EHCI_DWORD_WRITE_MEM; access to the
// capability registers is through the macro USBPORT_DWORD_READ_MEM and
// there is no macro to write to the registers
//----------------------------------------------------------------------------
										//(EIP55960)>
	if ((HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_HCHALTED) == 0) {
	    // Turn HC off and wait for the Halted bit to get set
	    HcClearOpReg(HcStruc, EHCI_USBCMD, EHCI_RUNSTOP);

		// The Host Controller must halt within 16 micro-frames after 
		// software clears the Run bit. 
		for (i = 0; i < 16; i++) {
			if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_HCHALTED) {
				break;
			}
            FixedDelay(125);	  // 125 us delay
		}
		//while ((DwordReadMem(dMemAddr, EHCI_USBSTS) & EHCI_HCHALTED) == 0) {};
	}
										//<(EIP55960)
// /* EIP#23479
    //
    // Reset the host controller (HC must be halted)
    //
    if (EHCIResetHC(HcStruc) == USB_ERROR) {
        return  USB_ERROR;  // HC reset error, error log is updated
    }
//*/
    //
    // Get the frame list size from the EHCI command register
    //
    Temp = HcReadOpReg(HcStruc, EHCI_USBCMD);
    Temp = (Temp & (BIT2 + BIT3)) >> 2;

    //
    // Calculate number of elements in the asynchronous list
    // and store the value in the HCStruc
    //
    switch (Temp) {
        case 0: 
            HcStruc->wAsyncListSize   = 1024;
            break;
        case 1: 
            HcStruc->wAsyncListSize   = 512;
            break;
        case 2: 
            HcStruc->wAsyncListSize   = 256;
            break;
        case 3: 
        default:
            return USB_ERROR;

    }

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "EHCI AsyncListSize: %d\n", HcStruc->wAsyncListSize);

    //
    // Set the max bulk data size
    //
    HcStruc->dMaxBulkDataSize = MAX_EHCI_DATA_SIZE;

    //
    // Initialize the frame list pointers
    //
    USB_InitFrameList(HcStruc, EHCI_TERMINATE);

    //
    // Write the base address of the Periodic Frame List to the PERIODIC BASE
    // register
    //
#if !USB_RUNTIME_DRIVER_IN_SMM
    if (HcStruc->FrameListPhyAddr) {
        HcWriteOpReg(HcStruc, EHCI_PERIODICLISTBASE, (UINT32)(UINTN)HcStruc->FrameListPhyAddr);
    } else {
        HcWriteOpReg(HcStruc, EHCI_PERIODICLISTBASE, (UINT32)(UINTN)HcStruc->fpFrameList);
    }
#else
    HcWriteOpReg(HcStruc, EHCI_PERIODICLISTBASE, (UINT32)(UINTN)HcStruc->fpFrameList);
#endif

    //
    // Initialize the periodic schedule
    //
    EHCIInitializePeriodicSchedule(HcStruc, (UINT32)HcStruc->BaseAddress);
/*
#if EHCI_ASYNC_BELL_SUPPORT
    //
    // Allocate and initialize an queue head for Async transfer
    // Set the QHDummy as Async list head
    //
    QhAsyncXfer = EhciMemAlloc(HcStruc, GET_MEM_BLK_COUNT_STRUC(EHCI_QH));

    if (!QhAsyncXfer) {
        return  USB_ERROR;
    }

    gUsbData->fpQHAsyncXfer        = QhAsyncXfer;

    QhAsyncXfer->dEndPntCap       = QH_ONE_XFER;
    QhAsyncXfer->fpFirstqTD       = 0;
    QhAsyncXfer->dAltNextqTDPtr   = EHCI_TERMINATE;
    QhAsyncXfer->dNextqTDPtr      = EHCI_TERMINATE;

    //
    // Assume as a high speed device
    //
    Temp = QH_HIGH_SPEED;  // 10b - High speed

    //
    // Use data toggle from qTD and this QH is the head of the queue
    //
    Temp |= (QH_USE_QTD_DT | QH_HEAD_OF_LIST | DUMMY_DEVICE_ADDR); // Endpoint is 0

    //
    // dTemp[6:0] = Dev. Addr, dTemp[7] = I bit(0) & dTemp[11:8] = Endpoint (0)
    //
    QhAsyncXfer->dEndPntCharac    = Temp;

    //
    // Set the ASYNCLISTADDR register to point to the QHDummy
    //
    HcWriteOpReg(HcStruc, EHCI_ASYNCLISTADDR, (UINT32)(UINTN)QhAsyncXfer);

    //
    // Set next QH pointer to itself (circular link)
    //
    QhAsyncXfer->dLinkPointer = (UINT32)(UINTN)QhAsyncXfer | EHCI_QUEUE_HEAD;
    QhAsyncXfer->bActive = TRUE;
#endif  //  EHCI_ASYNC_BELL_SUPPORT
*/

#if !USB_RT_DXE_DRIVER
	// Check whether no companion host controllers
	if (!(HcStruc->dHCFlag & HC_STATE_EXTERNAL) &&
		(HcStruc->dHCSParams & EHCI_N_CC) == 0) {
		//
		// Allocate a QH/qTD for QHRepeat/qTDRepeat
		//
	    QhRepeat = EhciMemAlloc(HcStruc,
						GET_MEM_BLK_COUNT(sizeof(EHCI_QH)+sizeof(EHCI_QTD)));

		if (!QhRepeat) {
			return	USB_ERROR;	// Memory allocation error
		}
        DescPtr = HcStruc->stDescPtrs.fpEHCIDescPtrs;
		DescPtr->fpQHRepeat = QhRepeat;
		QtdRepeat = (EHCI_QTD*)((UINT32)QhRepeat + sizeof(EHCI_QH));
		DescPtr->fpqTDRepeat = QtdRepeat;
//
// Setup QHRepeat and qTDRepeat.  It will run a interrupt transaction to a
// nonexistant dummy device.  This will have the effect of generating
// a periodic interrupt used to generate keyboard repeat.  This QH/qTD
// is normally inactive,  and is only activated when a key is pressed.
//
		//
		// Set the first qTD pointer
		//
		QhRepeat->fpFirstqTD = QtdRepeat;

		//QhRepeat->fpDevInfoPtr = (UINT8*)fpDevInfo;
		QhRepeat->dNextqTDPtr = (UINT32)QtdRepeat;

		//
		// Intialize the queue head
		//
		QhRepeat->dAltNextqTDPtr = EHCI_TERMINATE;
		QhRepeat->dLinkPointer = EHCI_TERMINATE;

		//
		// Set max packet size, address, endpoint and high speed
		// Update the AH's endpoint characteristcs field with the data formed
		//
		QhRepeat->dEndPntCharac |= ((0x40 << 16) | DUMMY_DEVICE_ADDR |
									QH_HIGH_SPEED);

		//
		// Set a bit in interrupt mask
		//
		QhRepeat->dEndPntCap	= (BIT0 | QH_ONE_XFER);
		QhRepeat->Interval = REPEAT_INTERVAL;

//
// Fill the repeat qTD with relevant information
// The token field will be set so
//	 Direction PID = QTD_IN_TOKEN,
//	 Size = size of the data,
//	 Data Toggle = QTD_DATA0_TOGGLE,
//	 Error Count = QTD_NO_ERRORS,
//	 Status code = QTD_ACTIVE
// The buffer pointers field will point to the fpBuffer buffer
//	 which was before initialized to contain a DeviceRequest struc.
// The dNextqTDPtr field will point to the qTDControlSetup
// The dAltNextqTDPtr field will be set to EHCI_TERMINATE
//
		QhRepeat->dTokenReload = ((UINT32)8 << 16) | QTD_IN_TOKEN | QTD_ONE_ERROR;
		QtdRepeat->dToken = ((UINT32)8 << 16) | QTD_IN_TOKEN | QTD_ONE_ERROR;

		EHCISetQTDBufferPointers(QtdRepeat,
			&QhRepeat->aDataBuffer[0], 8);

		//
		// Update next & alternate next qTD pointers
		//
		QtdRepeat->dNextqTDPtr = EHCI_TERMINATE;
		QtdRepeat->dAltNextqTDPtr = EHCI_TERMINATE;

		//
		// Schedule the QHRepeat to 8ms schedule
		//
        EhciAddPeriodicQh(HcStruc,QhRepeat);

        QhRepeat->bCallBackIndex = USB_InstallCallBackFunction(EhciRepeatTDCallback);
        QhRepeat->bActive = FALSE;

		USBKeyRepeat(HcStruc, 0);
	}
#endif

    //
    // Clear status register - all R/WC bits
    //
    HcWriteOpReg(HcStruc, EHCI_USBSTS,
            EHCI_USB_INTERRUPT |        // Interrupt
            EHCI_USB_ERROR_INTERRUPT |  // Error interrupt
            EHCI_PORT_CHANGE_DETECT |   // Port Change Detect
            EHCI_FRAME_LIST_ROLLOVER |  // Frame List Rollover
            EHCI_HOST_SYSTEM_ERROR |    // Host System Error
            EHCI_INT_ASYNC_ADVANCE);    // Interrupt on Async Advance
    //
    // Program the HC BIOS owned bit and return the legacy support register offset
    //
    if (HcStruc->bExtCapPtr) {
		EHCIProgramLegacyRegisters(HcStruc, 1);  // Set HC BIOS owned semaphore

        //
        // Enable USB SMI, SMI on port change and SMI on ownership change
        //
		Temp = EHCI_SMI + EHCI_PORT_CHANGE_SMI + EHCI_OWNERSHIP_CHANGE_SMI;

		HcWritePciReg(HcStruc, HcStruc->bExtCapPtr + EHCI_LEGACY_CTRL_STS_REG, Temp);
    }

    //
    // Turn HC on
    //
    HcSetOpReg(HcStruc, EHCI_USBCMD, \
        (EHCI_RUNSTOP | EHCI_PER_SCHED_ENABLE));

    // Wait for halt bit get cleared
    for (i = 0; i < 20; i++) {
        if (!(HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_HCHALTED)) {
            break;
        }
        FixedDelay(100);	  // 100 us delay
    }

    //
    // If the port has the power switch then enable the port. Otherwise 
    // Power for the port is already present. So don't need to enable the power.
    // ( Refer EHCI Spec 2.2.3 HCSPARAMS Structural Parameters Bit 4 (PPC) )
    if (HcStruc->dHCSParams & EHCI_PPC) {
        //
        // Enable port power
        //
        for (i = 1, PortReg = EHCI_PORTSC; i <= HcStruc->bNumPorts; i++, PortReg += 4) {
            //
            // Skip enabling DebugPort
            //
            if (HcStruc->DebugPort && HcStruc->DebugPort == i) continue;
    
            if (HcReadOpReg(HcStruc, PortReg) & EHCI_PORTPOWER) {
                continue;
            }
            
            HcSetOpReg(HcStruc, PortReg, EHCI_PORTPOWER);
            SetPortPower = TRUE;
        }
        //
        // Delay till the ports power is stabilised
        //
        if (SetPortPower) {
            FixedDelay(20 * 1000);        // 20 msec delay
        }
    }

    // Set HC flag as running
    HcStruc->dHCFlag |= HC_STATE_RUNNING;

    // Set USB_FLAG_DRIVER_STARTED flag when HC is running.
    if (!(gUsbData->dUSBStateFlag & USB_FLAG_DRIVER_STARTED)) {
        gUsbData->dUSBStateFlag |= USB_FLAG_DRIVER_STARTED;
    }

    //
    // Disconnect all ports from companion HC (if any) and route them to EHCI
    //
    HcSetOpReg(HcStruc, EHCI_CONFIGFLAG, BIT0);		//(EIP59663-) //(EIP80307+)

    if (HcStruc->dHCFlag & HC_STATE_CONTROLLER_WITH_RMH) {
        // Wait for port change detect bit set
        for (i = 0; i < 50; i++) {
            if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_PORT_CHANGE_DETECT) {
                break;
            }
            FixedDelay(100);    // 100 us delay
        }
    } else {
        FixedDelay(100);    // 100 us delay
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

    return  USB_SUCCESS;

}


/**
    This function returns the PCI register offset for the legacy
    support in EHCI controller

    @param HcStruc   - HCStruc pointer
        wPciAddr    - PCI address of the EHCI host controller

    @retval 0   If the feature is not present
        <>0 Legacy support capability offset

**/

UINT8
EHCIGetLegacySupportOffset(
    HC_STRUC*   HcStruc,
    UINT16      PciAddr
)
{
    UINT8   Ptr = 0;
    UINT32  Data = 0;

    if (HcStruc->dHCFlag & HC_STATE_IRQ) {
        return 0;
    }

#if !USB_RT_DXE_DRIVER

    if (HcStruc->dHCFlag & HC_STATE_EXTERNAL) {
        return 0;
    }

    //
    // Get EHCI Extended Capabilities Pointer
    //
    Ptr = (UINT8)((HcStruc->dHCCParams >> 8) & 0xFF);

    if (!Ptr) {
        return 0;   // No extended capabilities are implemented.
    }

    Data = HcReadPciReg(HcStruc, Ptr);
    if (!((UINT8)Data & 1)) {
        return 0;
    }
#endif
    return Ptr;

}


/**
    This function programs the EHCI legacy registers as per the
    input. Also this routine returns the PCI register offset
    for the legacy support in EHCI controller

    @param HcStruc   HCStruc pointer
           SetReset:
              0   Reset HC BIOS owned bit
              1   Set HC BIOS owned bit

    @retval 0   If the feature is not present
        <>0 Legacy support capability offset

**/

UINT8
EHCIProgramLegacyRegisters(
    HC_STRUC*   HcStruc,
    UINT8       SetReset
)
{
    UINT32  Temp;

    //
    // Check whether EHCI extended capabilities  pointer is present
    //
    if (!HcStruc->bExtCapPtr) {
        return 0;  // No extended capabilities are implemented.
    }

    //
    // Program 'HC BIOS owned semaphore bit'
    //
    Temp = HcReadPciReg(HcStruc, HcStruc->bExtCapPtr);
    Temp &= ~BIT16;

    if (SetReset) {
        Temp |= BIT16;
    }

                                        // (USB_S4_RESUME_ISSUE, EIP#20084)>
    if (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) {
        Temp &= ~BIT24;
    }
                                        // <(USB_S4_RESUME_ISSUE, EIP#20084)

    HcWritePciReg(HcStruc, HcStruc->bExtCapPtr, Temp);

    //
    // Reset all enable bits and clear the status
    //
    Temp = 0xE0000000 | EHCI_OWNERSHIP_CHANGE_SMI;

    HcWritePciReg(HcStruc, HcStruc->bExtCapPtr + EHCI_LEGACY_CTRL_STS_REG, Temp);

    return HcStruc->bExtCapPtr;

}


/**
    This procedure clear EHCI legacy support status.

    @param HcStruc   - HCStruc pointer
           Status    - Legacy status to clear

    @retval VOID

**/
VOID
ClearEECPstatus(
    HC_STRUC*   HcStruc,
    UINT16      Status
)
{
    UINT32  Temp;

    if (!HcStruc->bExtCapPtr) {
        return; // No extended capabilities are implemented.
    }

    //
    // Read control and status register
    //
    Temp = HcReadPciReg(HcStruc,
                HcStruc->bExtCapPtr + EHCI_LEGACY_CTRL_STS_REG);

    //
    // Keep enable bits and set clear status bit
    //
    Temp = (Temp & 0xFFFF) | ((UINT32)Status << 16);
    HcWritePciReg(HcStruc, HcStruc->bExtCapPtr + EHCI_LEGACY_CTRL_STS_REG, Temp);
    
}


/**
    This routine return USBLEGSUP register content. It could be
    used to check EHCI semaphore owened by BIOS or OS.

    @param HcStruc   HCStruc pointer

    @retval UINT32  Legacy support extended capability register content.
        -1 if no extended capabilities are implemented.

**/

UINT32
GetEhciUSBLEGSUP(
    HC_STRUC* HcStruc
)
{
    UINT32  Temp;

    //
    // Check whether EHCI extended capabilities  pointer is present
    //
    if (!HcStruc->bExtCapPtr) {
        return 0xFFFFFFFF;  // No extended capabilities are implemented.
    }

    //
    // Read Legacy support register
    //
    Temp = HcReadPciReg(HcStruc, HcStruc->bExtCapPtr);

    return Temp;
}


/**
    This function enumerates the HC ports for devices

    @param HcStruc   Host controller's HCStruc structure

    @retval VOID

**/

UINT8
EHCI_EnumeratePorts(
    HC_STRUC* HcStruc
)
{
    UINT16  PortCtl = EHCI_PORTSC;  // Port Status and Control Register (44h)
    UINT8   HcNumber;
    UINT8   PortNum;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }

	if (EhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}

    HcNumber = (UINT8)(HcStruc->bHCNumber | BIT7);
    

    //
    // Enable port power so that new devices can be detected.
    //
    // Check whether enumeration flag is set by us or by somebody else by checking
    // local enum flag.
    //
    if (gUsbData->bEnumFlag == FALSE) {
        gUsbData->bIgnoreConnectStsChng    = TRUE;
        gUsbData->bEnumFlag                = TRUE;
                                        //(EIP122174+)>
        do {
            //
            // Clear the EHCI_PCD bit of the interrupt status register EHCI_USBSTS
            //
            HcWriteOpReg(HcStruc, EHCI_USBSTS, EHCI_PORT_CHANGE_DETECT);

            //
            // Check the root hub ports to see if a device is connected.  If so, then
            // call USBCheckPortChange to handle the attachment of a new device.
            //
            for (PortNum = 1; PortNum <= HcStruc->bNumPorts; PortNum++) {
                //
                // Skip DebugPort enumeration
                //
                if (HcStruc->DebugPort && HcStruc->DebugPort == PortNum) {
                    continue;
                }

                //
                // Process device connect/disconnect
                //
                USBCheckPortChange(HcStruc, HcNumber, PortNum);
            }
        } while ((HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_PORT_CHANGE_DETECT));
                                        //<(EIP122174+)
        gUsbData->bIgnoreConnectStsChng = FALSE;

        //
        // Reset enumeration flag and enable hub enumeration
        //
        gUsbData->bEnumFlag = FALSE;
    }

    //
    // Enable appropriate interrupts
    //
    HcWriteOpReg(HcStruc, EHCI_USBINTR, EHCI_USBINT_EN | EHCI_PCDINT_EN);
    return  USB_SUCCESS;
}


/**
    This function checks whether the host controller is still
    under BIOS

    @param HcStruc   - Host controller's HCStruc structure

    @retval USB_SUCCESS If the control is with the BIOS
    @retval USB_ERROR If the control is not with the BIOS

**/

UINT8
EHCICheckHCStatus(
    HC_STRUC* HcStruc
)
{
    UINT32  Cmd;
    UINT32  Sts;

    Cmd = HcReadOpReg(HcStruc, EHCI_USBCMD);
    Sts = HcReadOpReg(HcStruc, EHCI_USBSTS);

    // Don't read Periodic Frame List Base Address Register if the controller 
    // doesn't support periodic schedule.
    if (Cmd & EHCI_PER_SCHED_ENABLE) {
        if (!(Sts & EHCI_PER_SCHED_STATUS)) {
            return USB_SUCCESS;
        }
    }
    //
    // Check whether the controller is still under BIOS control
    // Read the base address of the Periodic Frame List to the PERIODIC BASE
    // register and compare with stored value
    //
#if !USB_RUNTIME_DRIVER_IN_SMM
    if (HcStruc->FrameListPhyAddr) {
        if ((UINTN)HcStruc->FrameListPhyAddr ==
            (HcReadOpReg(HcStruc, EHCI_PERIODICLISTBASE) & 0xFFFFF000)) {
            return USB_SUCCESS;    // Control is with BIOS
        }
    } else {
        if ((UINTN)HcStruc->fpFrameList ==
            (HcReadOpReg(HcStruc, EHCI_PERIODICLISTBASE) & 0xFFFFF000)) {
            return USB_SUCCESS;    // Control is with BIOS
        }
    }
#else
    if ((UINTN)HcStruc->fpFrameList == 
        (HcReadOpReg(HcStruc, EHCI_PERIODICLISTBASE) & 0xFFFFF000)) {
        return USB_SUCCESS;    // Control is with BIOS
    }
#endif
    return USB_ERROR;  // HC is controlled by someone else
}


/**
    This function stops the EHCI controller.

    @param HcStruc   Host controller's HCStruc structure

    @retval VOID

**/

UINT8
EHCI_Stop(
    HC_STRUC* HcStruc
)
{
    UINT8   PortNum;					//(EIP26685+)
    UINT8   Status;
	UINT8	i;							//(EIP55960+)
    EHCI_DESC_PTRS  *DescPtr;
    EFI_STATUS  EfiStatus;
    UINT8       *MemBlockEnd = gUsbData->fpMemBlockStart + (gUsbData->MemPages << 12);

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }


    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }

    DescPtr = HcStruc->stDescPtrs.fpEHCIDescPtrs;

#if USB_RUNTIME_DRIVER_IN_SMM
    if (((UINT8*)DescPtr < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(DescPtr + sizeof(EHCI_DESC_PTRS)) > MemBlockEnd)) {
        return USB_ERROR;
    }
#endif
    
    //
    // Check whether the control is with BIOS or not
    //
    if (EHCICheckHCStatus(HcStruc) == USB_SUCCESS)    // Controlled by BIOS
    {
#if PCH_EHCI_OWNERSHIP_CHANGE_MECHANISM
		if (HcStruc->dHCFlag & HC_STATE_OWNERSHIP_CHANGE_IN_PROGRESS) {
			UINT16  PortReg;
			UINT32	PortSts;
			UINT32	Data32;

			// Disconnect all the devices connected to its ports
			for (PortNum = 1; PortNum <= HcStruc->bNumPorts; PortNum++) {
				USB_StopDevice(HcStruc, (UINT8)(HcStruc->bHCNumber | BIT7), PortNum);
			}
	
			// Stop the asynchronous schedule
			EHCIStopAsyncSchedule(HcStruc);

			// Stop the periodic schedule
			EHCIStopPeriodicSchedule(HcStruc);
	
			for (PortNum = 1; PortNum <= HcStruc->bNumPorts; PortNum++) {
				PortReg = (UINT16)((PortNum - 1) * 4 + EHCI_PORTSC);
				PortSts = HcReadOpReg(HcStruc, PortReg);
	
				if (!(PortSts & EHCI_PORTENABLE)) {
					continue;
				}
				HcWriteOpReg(HcStruc, PortReg, PortSts | EHCI_SUSPEND);
			}
			FixedDelay(250);      // 250 us delay
	
			// Stop the host controller (Reset bit 0)
			HcClearOpReg(HcStruc, EHCI_USBCMD, EHCI_RUNSTOP);
	
			// The Host Controller must halt within 16 micro-frames after 
			// software clears the Run bit. 
			for (i = 0; i < 16; i++) {
				if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_HCHALTED) {
					break;
				}
                FixedDelay(125);	  // 125 us delay
			}
	
			// Clear the SMI enable bits
			if (HcStruc->bExtCapPtr) {
				Data32 = HcReadPciReg(HcStruc, 
							HcStruc->bExtCapPtr + EHCI_LEGACY_CTRL_STS_REG);
	
				HcWritePciReg(HcStruc,
					HcStruc->bExtCapPtr + EHCI_LEGACY_CTRL_STS_REG, Data32 & ~(0x3F));
			}
	
			// Clear the USBSTS register bits
			HcWriteOpReg(HcStruc, EHCI_USBSTS, HcReadOpReg(HcStruc, EHCI_USBSTS));
	
			// Clear the Configure Flag bit
			HcClearOpReg(HcStruc, EHCI_CONFIGFLAG, BIT0);
		} else 
#endif
		{
											//(EIP26685+)>
	        //
	        // Disconnect all the devices connected to its ports
	        //
	        for (PortNum = 1; PortNum <= HcStruc->bNumPorts; PortNum++) {
	            USB_DisconnectDevice(HcStruc,
	                    (UINT8)(HcStruc->bHCNumber | BIT7), PortNum);
	        }
											//<(EIP26685+)

	        if (HcStruc->DebugPort == 0) {
	            //
	            // Stop the host controller (Reset bit 0)
	            //
	            HcClearOpReg(HcStruc, EHCI_USBCMD, EHCI_RUNSTOP);
												//(EIP55960)>
				// The Host Controller must halt within 16 micro-frames after 
				// software clears the Run bit. 
				for (i = 0; i < 16; i++) {
	            	if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_HCHALTED) {
						break;
	            	}
                    FixedDelay(125);      // 125 us delay
				}
												//<(EIP55960)
	            //
	            // Reset the host controller
	            //
//  EIP#23479          EHCIResetHC(HcStruc); // ERROR CONDITION RETURNED IS NOT TAKEN CARE
	            Status = EHCIResetHC(HcStruc);
	            ASSERT(Status == USB_SUCCESS);
	        }
		}
        //
        // Program the HC BIOS owned bit and return the legacy
        // support register offset
        //
        EHCIProgramLegacyRegisters(HcStruc, 0);   // Reset HC BIOS owned semaphore
                                                    // ERROR CONDITION IS NOT HANDLED

        //
        // Clear the frame list pointers
        //
        USB_InitFrameList(HcStruc, EHCI_TERMINATE);

        //
        // Disable TD schedule and free the data structures
        //
		if (DescPtr->fpQHRepeat) {
			EhciMemFree(HcStruc, DescPtr->fpQHRepeat,
							GET_MEM_BLK_COUNT(sizeof(EHCI_QH) + sizeof(EHCI_QTD) ));
		}

        //
        // Free the scheduling QHs
        //
        EhciMemFree(HcStruc, DescPtr->PeriodicQh,        
                        GET_MEM_BLK_COUNT(1 * sizeof(EHCI_QH)));

        //
        // Free descriptor structure
        //
        EhciMemFree(HcStruc, DescPtr,
                        GET_MEM_BLK_COUNT_STRUC(EHCI_DESC_PTRS));

//#if EHCI_ASYNC_BELL_SUPPORT
        //
        // Free the Async transfer QH
        //
//        EhciMemFree(HcStruc, gUsbData->fpQHAsyncXfer, GET_MEM_BLK_COUNT_STRUC(EHCI_QH));
//#endif

		USBKeyRepeat(HcStruc, 3);
    }
    else    // not controlled by BIOS
    {
        //
        // Program the HC BIOS owned bit and return the legacy
        // support register offset
        //
        EHCIProgramLegacyRegisters(HcStruc, 0);   // Reset HC BIOS owned semaphore
    }

    //
    // Set the HC state to stopped
    //
    HcStruc->dHCFlag  &= ~(HC_STATE_RUNNING);

    CheckBiosOwnedHc();

    return USB_SUCCESS;
}


/**
    This function disables the HC interrupts

    @param HcStruc   Pointer to the HCStruc structure

    @retval USB_ERROR   On error
        USB_SUCCESS On success

**/

UINT8
EHCI_DisableInterrupts (
    HC_STRUC*   HcStruc
)
{
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }
    //
    // Disable interrupt generation
    //
    HcClearOpReg(HcStruc, EHCI_USBINTR, EHCI_USBINT_EN | EHCI_PCDINT_EN);

    //
    // Stop periodic and asynchoronous schedule
    //
    EHCIStopAsyncSchedule(HcStruc);
    EHCIStopPeriodicSchedule(HcStruc);

    return  USB_SUCCESS;
}


/**
    This function enables the HC interrupts

    @param HcStruc   Pointer to the HCStruc structure

    @retval USB_ERROR   On error
        USB_SUCCESS On success

**/

UINT8
EHCI_EnableInterrupts (
    HC_STRUC* HcStruc
)
{
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }
    //
    // Start periodic and asynchoronous schedule
    //
    EHCIStartAsyncSchedule(HcStruc);
    EHCIStartPeriodicSchedule(HcStruc);

    //
    // Enable interrupt generation
    //
    HcSetOpReg(HcStruc, EHCI_USBINTR, EHCI_USBINT_EN | EHCI_PCDINT_EN);

    return USB_SUCCESS;
}


/**
    Root hub change processing code

    @param HcStruc   Pointer to the HCStruc structure


**/

UINT8
ProcessRootHubChanges(
    HC_STRUC* HcStruc
)
{
    UINT8   PortNum;

    //
    // Check bEnumFlag before enumerating devices behind root hub
    //
    if ((gUsbData->bEnumFlag) == TRUE) {
        return USB_ERROR;
    }

    // The interval with a minimum duration of 100 ms when 
    // the USB System Software is notified of a connection detection.
    FixedDelay(USB_PORT_CONNECT_STABLE_DELAY_MS * 1000);

    //
    // Clear the port change bit of the interrupt status register EHCI_USBSTS
    //
    HcWriteOpReg(HcStruc, EHCI_USBSTS, EHCI_PORT_CHANGE_DETECT);

    //
    // Check all the ports on the root hub for any change in connect status.
    // If the connect status has been changed on either or both of these ports,
    // then call the  routine UsbHubPortChange for each changed port.
    //
    // Set enumeration flag and avoid hub port enumeration
    //
    gUsbData->bEnumFlag = TRUE;

    for (PortNum = 1; PortNum <= HcStruc->bNumPorts; PortNum++) {
        if (HcStruc->DebugPort && HcStruc->DebugPort == PortNum) {
            continue;
        }
        //
        // Process device connect/disconnect
        // Note: port connect status is cleared while processing
        // connect/disconnect (EHCIGetRootHubStatus)
        //
        USBCheckPortChange(HcStruc, (UINT8)(HcStruc->bHCNumber | BIT7), PortNum);
    }

    //
    // Reset enumeration flag and enable hub enumeration
    //
    gUsbData->bEnumFlag = FALSE;
    return USB_SUCCESS;
}

/**
    This function is called when the USB interrupt bit is
    set. This function will parse through the TDs and QHs to
    find out completed TDs and call their respective call
    back functions

    @param HcStruc   Pointer to the HCStruc structure

    @retval USB_ERROR Need more Interrupt processing
    @retval USB_SUCCESS No interrupts pending

**/

UINT8
EHCI_ProcessInterrupt(
    HC_STRUC* HcStruc
)
{
    UINT32  Sts;
    UINT32  Temp;
    UINT16  Status;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }
                                        //(EIP71067-)>
//#if (EHCI_ASYNC_BELL_SUPPORT==0)
//    EHCI_QH *fpQH;
//#endif
                                        //<(EIP71067-)

    //
    // If EHCI extended capabilities  pointer is present,
    // then service OwnerShipChange SMI
    //
    if (HcStruc->bExtCapPtr) {
        //
        // Read control and status register
        //
        Temp = HcReadPciReg(
                HcStruc,
                HcStruc->bExtCapPtr + EHCI_LEGACY_CTRL_STS_REG);
        Status = (UINT16)Temp;
        Status &= (UINT16)(Temp >> 16);   // "And" enable and status bits
        if (Status & EHCI_OWNERSHIP_CHANGE_SMI_STS) {
            ClearEECPstatus(HcStruc, Status);
            ProcessOwnerShipChangeSMI(HcStruc);
            return USB_SUCCESS; // Break from Interrupt process loop
        }
    }

    //
    // Check whether the controller is still under BIOS control
    // Read the base address of the Periodic Frame List to the PERIODIC BASE
    // register and compare with stored value
    //
    if (EHCICheckHCStatus(HcStruc) == USB_ERROR) {
        //
        // Control is not with us anymore, we should disable SMI generation
        // and come out.
        //
        if (HcStruc->bExtCapPtr) {
            //
            // Read control and status register
            //
            Temp = HcReadPciReg(
                        HcStruc,
                        HcStruc->bExtCapPtr + EHCI_LEGACY_CTRL_STS_REG);
        
            //
            // Leave only Ownership SMI active.
            //
            Temp &= 0xE0002000; 
            HcWritePciReg(HcStruc, HcStruc->bExtCapPtr + EHCI_LEGACY_CTRL_STS_REG,
                            Temp);
        }
        return USB_SUCCESS;
    }

	if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
		return USB_SUCCESS;
	}

    while (TRUE) {
        //
        // Get the interrupt status
        //
        Sts = HcReadOpReg(HcStruc, EHCI_USBSTS);

        //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "-->> %x <<--\n", Sts);

		if (Sts & EHCI_HOST_SYSTEM_ERROR) {
			gUsbData->dUSBStateFlag &= (~USB_FLAG_ENABLE_BEEP_MESSAGE);
			EHCI_Start(HcStruc);
			EHCI_EnumeratePorts(HcStruc);
			gUsbData->dUSBStateFlag |= USB_FLAG_ENABLE_BEEP_MESSAGE;
			continue;
		}

        if (Sts & EHCI_HCHALTED) {
			// Clear the USBSTS register bits
			HcWriteOpReg(HcStruc, EHCI_USBSTS, HcReadOpReg(HcStruc, EHCI_USBSTS));
            break;
        }
        
        //
        // Check for transaction complete
        //
        
        if ((gUsbData->ProcessingPeriodicList == TRUE) && (Sts & EHCI_USB_INTERRUPT)) {

            //
            // Clear the interrupt status
            //
            HcWriteOpReg(HcStruc, EHCI_USBSTS, EHCI_USB_INTERRUPT);

            //Section 4.4 Schedule traversal rules.
            //if the periodic schedule is enabled (see Section 4.6) then the host controller must 
            //execute from the periodic schedule before executing from the asynchronous schedule. 
            //It will only execute from the asynchronous schedule after it encounters the end of
            //the periodic schedule.

            //
            // Check and process periodic schedule
            //
            if (Sts & EHCI_PER_SCHED_STATUS) {
                //
                // Check the command register for Async status
                //
                Temp = HcReadOpReg(HcStruc, EHCI_USBCMD);

                if (Temp & EHCI_PER_SCHED_ENABLE) {
                    EHCIProcessPeriodicList(HcStruc);
                }
            }

            //
            // Check for Asynchronous schedule completion
            //
/*                                        //(EIP71067-)>
            if (dSts & EHCI_ASYNC_SCHED_STATUS) {
                dTmp = DwordReadMem(dMemAddr, EHCI_USBCMD);
                if (dTmp & EHCI_ASYNC_SCHED_ENABLE) {
                    //
                    // Check and process Async. QH
                    //
#if EHCI_ASYNC_BELL_SUPPORT
                    EHCIProcessQH(HcStruc, HcStruc->stDescPtrs.fpEHCIDescPtrs->fpQHControl);
                    EHCIProcessQH(HcStruc, HcStruc->stDescPtrs.fpEHCIDescPtrs->fpQHBulk);
#else
                    //
                    // Get the Async list address
                    //
                    fpQH = (EHCI_QH*)(UINTN)DwordReadMem(dMemAddr, EHCI_ASYNCLISTADDR);
                    if (EHCIProcessQH(HcStruc, fpQH) == USB_ERROR) {
                        //continue;
                        //return    USB_SUCCESS;
                    } else {
                        //
                        // Async list processed; stop the Async transfer
                        //
                        EHCIStopAsyncSchedule(HcStruc);
                    }
#endif
                }
            }
*/                                      //<(EIP71067-)
            continue;
        }

        //
        // Check PORT_CHANGE_DETECT bit
        //
        if ((Sts & EHCI_PORT_CHANGE_DETECT )) {
            if (ProcessRootHubChanges(HcStruc) == USB_SUCCESS) {
                continue;
            }
        }
        break;  // No more statuses to process
    }
    return  USB_SUCCESS;    // Set as interrupt processed
}


/**
    This procedure process EHCI OwnerShipChange SMI.

    @param HcStruc   HCStruc pointer

    @retval VOID

**/
VOID
ProcessOwnerShipChangeSMI(
    HC_STRUC* HcStruc
)
{
    UINT32 Temp = GetEhciUSBLEGSUP(HcStruc);
    
	HcStruc->dHCFlag |= HC_STATE_OWNERSHIP_CHANGE_IN_PROGRESS;
    if (Temp & EHCI_HC_OS) {
        gUsbData->dUSBStateFlag &= (~USB_FLAG_ENABLE_BEEP_MESSAGE);
        ProcessSmiChangeToEHCD(HcStruc);
    }
    else {
        gUsbData->dUSBStateFlag |= USB_FLAG_ENABLE_BEEP_MESSAGE;
        ProcessSmiChangeToBIOS(HcStruc);
    }
	HcStruc->dHCFlag &= ~(HC_STATE_OWNERSHIP_CHANGE_IN_PROGRESS);
}


/**
    This procedure process OwnerShipChange for BIOS -> EHCD.

    @param HcStruc   HCStruc pointer

    @retval VOID

**/
VOID
ProcessSmiChangeToEHCD(
    HC_STRUC* HcStruc
)
{
    EHCI_Stop(HcStruc);    // Stop EHCI legacy
}


/**
    This procedure process OwnerShipChange for EHCD -> BIOS.

    @param SI  HCStruc pointer

    @retval VOID

**/

VOID
ProcessSmiChangeToBIOS (
    HC_STRUC    *HcStruc
)
{
    HC_STRUC    *Hc;
    UINT8       Count;
    DEV_INFO    *Device;

    // Stop UHCI devices connected to the companions
    // Core8 executes this under MKF_PCCHECK_PATCH==1 condition, EIP10272
    for (Count = 1; Count < MAX_DEVICES; Count++) {
        Device = &gUsbData->aDevInfoTable[Count];
        
        if ((Device->Flag & DEV_INFO_VALIDPRESENT) != DEV_INFO_VALIDPRESENT) {
			continue;
		}
        
        Hc = gUsbData->HcTable[Device->bHCNumber - 1];

        if (Hc->bHCType != USB_HC_UHCI) {
            continue;   // Not UHCI
        }

        if ((Device->Flag & DEV_INFO_VALID_STRUC)==0) {
            continue;    // Not valid
        }

        if ((Hc->wBusDevFuncNum & 0xfff8) !=
            (HcStruc->wBusDevFuncNum & 0xfff8)) {
            continue; // Not a companion
        }

        // Found a device connected to UHCI companion controller. Stop it.
        USB_StopDevice(Hc, Device->bHubDeviceNumber, Device->bHubPortNumber);
    }

    EHCI_Start(HcStruc);  // Reinitialize EHCI host controller
}


/**

    @param HcStruc   - Pointer to HCStruc of the host controller
        PortNum   - Port in the HC whose status is requested

    @retval 

**/

UINT8
EHCI_ReleasePortOwner(
    HC_STRUC*	HcStruc,
    UINT8		PortNum
)
{
    UINT16  PortReg = (UINT16)((PortNum - 1) * 4 + EHCI_PORTSC);
    UINT16  i;

	if ((HcStruc->dHCSParams & EHCI_N_CC) == 0) {
		return USB_SUCCESS;
	}

    if (!(HcReadOpReg(HcStruc, PortReg) & EHCI_CURRENTCONNECTSTATUS)) {
        return USB_ERROR;
    }

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Release EHCI port %d\n", PortNum);
    HcSetOpReg(HcStruc, PortReg, EHCI_PORTOWNER);

    // Loop until Full speed device disconnect event process done.
    // This change is done in sync with Core8 except the extra 400mS delay
    for (i = 0; i < 200; i++) {
        if (HcReadOpReg(HcStruc, PortReg) & EHCI_CONNECTSTATUSCHANGE) {
            break;
        }
        FixedDelay(100); 
    }

    HcSetOpReg(HcStruc, PortReg, EHCI_CONNECTSTATUSCHANGE);

	return USB_SUCCESS;	
}

/**
    This function returns the port connect status for the
    root hub port

    @param HcStruc   - Pointer to HCStruc of the host controller
        bPortNum    - Port in the HC whose status is requested

    @retval Port status flags (see USB_PORT_STAT_XX equates)

**/

UINT32
EHCI_GetRootHubStatus(
    HC_STRUC*   HcStruc,
    UINT8       PortNum,
    BOOLEAN     ClearChangeBits
)
{
    UINT32  Tmp;
    UINT32  Status = USB_PORT_STAT_DEV_OWNER;
    UINT16  PortReg = (UINT16)((PortNum - 1) * 4 + EHCI_PORTSC);
    EFI_STATUS  EfiStatus;
    UINT8   Index;
    UINT8   PortSpeed;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (HcStruc->DebugPort && HcStruc->DebugPort == PortNum) {
        return 0;
    }

    //
    // Read the status of the port
    //
    Tmp = HcReadOpReg(HcStruc, PortReg);
	USB_DEBUG(DEBUG_INFO, 3, "Ehci port[%d] status: %08x\n", PortNum, Tmp);

	// Detect the high-speed device.
	// In case of low-speed or full-speed change the ownership to a
	// companion 1.1 controller (if any)
	if (Tmp & EHCI_CURRENTCONNECTSTATUS) {
    	// Analyze Line Status
    	if ((Tmp & EHCI_LINE_STATUS) == EHCI_DMINUSBIT) {	// Low speed device connected
    		EHCI_ReleasePortOwner(HcStruc, PortNum);
    		Tmp = HcReadOpReg(HcStruc, PortReg);
    	}
	}

    //
    // Check the connect status change bit
    //
    if (Tmp & EHCI_CONNECTSTATUSCHANGE) {
        //
        // Set connect status change flag
        //
        Status |= USB_PORT_STAT_DEV_CONNECT_CHANGED;

        //
        // Wait 20ms for host controller could report accurate port status properly.
        //
        //FixedDelay(gUsbData->UsbTimingPolicy.EhciPortConnect * 1000);   // 20ms delay

        //
        // Read the status of the port
        //
        //dTmp = HcReadOpReg(HcStruc, wPortReg);

		// Clear connect status change
        if (ClearChangeBits == TRUE) {
            HcSetOpReg(HcStruc, PortReg, EHCI_CONNECTSTATUSCHANGE);		//(EIP61030+)
        }
    }

    if (Tmp & EHCI_CURRENTCONNECTSTATUS) {
        Status |= USB_PORT_STAT_DEV_CONNECTED;

		if (Tmp & EHCI_PORTENABLE) {
            PortSpeed = AMI_USB_PORT_STAT_HIGH_SPEED;
            for (Index = 0; EhciGetRootHubPortSpeedFunctions[Index]; Index++) {
                EhciGetRootHubPortSpeedFunctions[Index](HcStruc, PortNum, &PortSpeed);
            }
            switch (PortSpeed) {
                case AMI_USB_PORT_STAT_LOW_SPEED:
                    Status |= USB_PORT_STAT_DEV_LOWSPEED;
                    break;
                case AMI_USB_PORT_STAT_FULL_SPEED:
                    Status |= USB_PORT_STAT_DEV_FULLSPEED;
                    break;
                case AMI_USB_PORT_STAT_HIGH_SPEED:
                    Status |= USB_PORT_STAT_DEV_HISPEED;
                    break;
                default:
                    USB_DEBUG(DEBUG_ERROR, 3, "Get the undefined speed of the root hub port %x\n", PortSpeed);
                    break;
            }
            Status |= USB_PORT_STAT_DEV_ENABLED;
		} else {
			if (gUsbData->bIgnoreConnectStsChng == TRUE) {
				if (!(Tmp & EHCI_CONNECTSTATUSCHANGE)) {
				    Status |= USB_PORT_STAT_DEV_CONNECT_CHANGED;
				}
			}
		}
	}

    if (Tmp & EHCI_PORTOWNER) {
        Status &= ~USB_PORT_STAT_DEV_OWNER;
    }
    if (Tmp & EHCI_PORTPOWER) {
        Status |= USB_PORT_STAT_DEV_POWER;
    }
    if (Tmp & EHCI_OVERCURRENTACTIVE) {
        Status |= USB_PORT_STAT_DEV_OVERCURRENT;
    }
    if (Tmp & EHCI_SUSPEND) {
        Status |= USB_PORT_STAT_DEV_SUSPEND;
    }
    if (Tmp & EHCI_PORTRESET) {
        Status |= USB_PORT_STAT_DEV_RESET;
    }
    if (Tmp & EHCI_PORTENABLESTATUSCHANGE) {
        Status |= USB_PORT_STAT_DEV_ENABLE_CHANGED;
    }
    if (Tmp & EHCI_OVERCURRENTCAHGE) {
        Status |= USB_PORT_STAT_DEV_OVERCURRENT_CHANGED;
    }

	return Status;
}


/**
    This function disables the EHCI HC Ruoot hub port.

    @param HcStruc   - Pointer to HCStruc of the host controller
        bPortNum    - Port in the HC to disable

    @retval USB_SUCCESS on success
        USB_ERROR   on error

**/

UINT8
EHCI_DisableRootHub(
    HC_STRUC*   HcStruc,
    UINT8       PortNum
)
{
										//(EIP58108+)>
    UINT16  PortReg = (UINT16)((PortNum - 1) * 4 + EHCI_PORTSC);
	UINT32	i;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (HcStruc->DebugPort && HcStruc->DebugPort == PortNum) {
        return USB_SUCCESS;
    }

	if (!(HcReadOpReg(HcStruc, PortReg) & EHCI_PORTENABLE)) {
		return USB_SUCCESS;
	}
    HcClearOpReg(HcStruc, PortReg, EHCI_PORTENABLE);

	for (i = 0; i < 100; i++) {
		if ((HcReadOpReg(HcStruc, PortReg) & EHCI_PORTENABLE) == 0) {
			break;
		}
		FixedDelay(100);
	}
										//<(EIP58108+)
    return USB_SUCCESS;
}


/**
    This function enables the EHCI HC Root hub port.

    @param HcStruc   - Pointer to HCStruc of the host controller
        bPortNum    - Port in the HC to enable

    @retval USB_SUCCESS on success
        USB_ERROR   on error

**/

UINT8
EHCI_EnableRootHub(
    HC_STRUC*   HcStruc,
    UINT8       PortNum
)
{
//
// Software can only enable the EHCI root hub ports by port RESET.  HC will
// enable the port only if it is a high speed device
//
    return USB_SUCCESS;
}

/**
    This function resets the EHCI HC Root hub port.

    @param HcStruc   - Pointer to HCStruc of the host controller
        PortNum    - Port in the HC to enable

    @retval USB_SUCCESS on success
        USB_ERROR   on error

**/

UINT8
EHCI_ResetRootHub(
    HC_STRUC*	HcStruc,
    UINT8		PortNum
)
{
    UINT16  PortReg = (UINT16)((PortNum - 1) * 4 + EHCI_PORTSC);
	UINT32	i;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

	// Disable the port if it is enabled
	if (HcReadOpReg(HcStruc, PortReg) & EHCI_PORTENABLE) {
		HcClearOpReg(HcStruc, PortReg, EHCI_PORTENABLE);
	
		// There may be a delay in disabling or enabling a port due to other 
		// host controller and bus events.
		for (i = 0; i < 100; i++) {
			if ((HcReadOpReg(HcStruc, PortReg) & EHCI_PORTENABLE) == 0) {
				break;
			}
			FixedDelay(100);    // 100 us delay
		}
	}

	// Reset the port
	HcSetOpReg(HcStruc, PortReg, EHCI_PORTRESET);

    if ((HcStruc->dHCFlag & HC_STATE_CONTROLLER_WITH_RMH) && (PortNum == 1)) {
        FixedDelay(3 * 1000);	 // 3 ms delay
    } else {
    	// Wait til port disable is complete (Tdrstr=50ms Ref 7.1.7.5 of USB Spec 2.0)
    	FixedDelay(50 * 1000);	 // 50 ms delay        
    }

	HcClearOpReg(HcStruc, PortReg, EHCI_PORTRESET);	// Terminate reset

    if (!(HcReadOpReg(HcStruc, PortReg) & EHCI_CURRENTCONNECTSTATUS)) {
        return USB_ERROR;
    }

	// if the port detects that the attached device is high-speed during reset, 
	// then the host controller must have the port in the enabled state within 2ms 
	// of software writing this bit to a zero.
	for (i = 0; i < 20; i++) {
		if ((HcReadOpReg(HcStruc, PortReg) & (EHCI_PORTRESET | 
			EHCI_PORTENABLE)) == EHCI_PORTENABLE) {
			break;
		}
        FixedDelay(100);	 // 100 us delay
	}

	if (HcReadOpReg(HcStruc, PortReg) & EHCI_PORTRESET) {	 // Reset failed
		USBLogError(USB_ERR_PORT_RESET_FAILED);
		return USB_ERROR;
	}

	if (!(HcReadOpReg(HcStruc, PortReg) & EHCI_PORTENABLE)) {
        if (!(HcReadOpReg(HcStruc, PortReg) & EHCI_CURRENTCONNECTSTATUS)) {
            return USB_ERROR;
        } else {
		    EHCI_ReleasePortOwner(HcStruc, PortNum);
		    return USB_ERROR;
        }
	}

	FixedDelay(1 * 1000);	 // 1 ms delay

    return USB_SUCCESS;
}

UINT8
EhciSmiControl(
    HC_STRUC* HcStruc,
    BOOLEAN Enable
)
{
    UINT32 LegCtlStsReg;
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

    if (Enable == TRUE) {
        LegCtlStsReg = EHCI_SMI + EHCI_PORT_CHANGE_SMI + EHCI_OWNERSHIP_CHANGE_SMI;

		HcWritePciReg(HcStruc, HcStruc->bExtCapPtr + EHCI_LEGACY_CTRL_STS_REG, LegCtlStsReg);
    } else {
        //
        // Reset all enable bits and clear the status
        //
        LegCtlStsReg = 0xE0000000;

        HcWritePciReg(HcStruc, HcStruc->bExtCapPtr + EHCI_LEGACY_CTRL_STS_REG, LegCtlStsReg);
    }

    return USB_SUCCESS;
}

                                        //(EIP54018+)>
/**
    This function suspend the EHCI HC.

**/

UINT8
EHCI_GlobalSuspend(
    HC_STRUC*	HcStruc
)
{
    UINT16  PortReg;
	UINT32	PortSts;
    UINT8   PortNum;
    UINT8   i;
    EFI_STATUS  EfiStatus;

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }
    
	if (EhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}

    for (PortNum = 1; PortNum <= HcStruc->bNumPorts; PortNum++) {
        PortReg = (UINT16)(EHCI_PORTSC + (PortNum - 1) * 4 );
        PortSts = HcReadOpReg(HcStruc, PortReg );
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3,"EHCI PortSts[%x] %x \n",
                        PortNum, PortSts); 
        // Check if port is disabled or suspended.
        if ((PortSts & EHCI_PORTENABLE) && (!(PortSts & EHCI_SUSPEND))) {
            // Suspend if necessary.
            HcClearOpReg(HcStruc, PortReg,
                            EHCI_WKOC_E | EHCI_WKDSCNNT_E | EHCI_WKCNNT_E);
            HcSetOpReg(HcStruc, PortReg, EHCI_SUSPEND);
            // Read PortSc until port shows suspended. 
     	    for (i = 0; i < 100; i++) {
                if (HcReadOpReg(HcStruc, PortReg) & EHCI_SUSPEND) {
                    break;
                }
                FixedDelay(100);      // 100 us delay
	        } 
        }
    }

    // Turn HC off and wait for the Halted bit to get set
    HcClearOpReg(HcStruc, EHCI_USBCMD, EHCI_RUNSTOP);
    // The Host Controller must halt within 16 micro-frames after 
    // software clears the Run bit. 
	for (i = 0; i < 16; i++) {
        if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_HCHALTED) {
            break;
        }
        FixedDelay(125);      // 125 us delay
	}

    EHCIProgramLegacyRegisters(HcStruc, 0); // Reset HC BIOS owned semaphore

    HcStruc->dHCFlag &= ~(HC_STATE_RUNNING);
    HcStruc->dHCFlag |= HC_STATE_SUSPEND;

    return USB_SUCCESS;
}
                                        //<(EIP54018+)

/**
    This function gets the hi-speed hub's device and port number
    to which this low speed device is connected.  It parses
    through its parents until it finds the correct device. This
    information is used for split transaction

    @param fpDevInfo   - Device info pointer of the device

    @retval UINT16 Device/port number of the hi-speed hub

    @note  This low/full speed device may be behind different hubs as
          shown below. In any case this routine will get the device
          address of the hub number HISP_A :
      Notations used:
          MBPortX     Motherboard USB port
          HISP_X      Hi-speed hub number X
          FUSP_X      Full-speed hub number X
          Device      Low/Full speed device
      Config 1:
        MBPortX --> HISP_A --> Device
      Config 2:
        MBPortX --> HISP_A --> FUSP_1 --> Device
      Config 3:
        MBPortX --> HISP_B --> HISP_A --> Device
      Config 4:
        MBPortX --> HISP_A --> FUSP_1 --> HISP_B --> Device
        In the above configuration the HISP_B will be operated in
      full speed rather than hi-speed since it is connected to a
      full speed hub
**/

UINT16
EHCI_GetHiSpeedHubPortNumber(
    DEV_INFO* DevInfo
)
{
    DEV_INFO*   HubDev = DevInfo;
    DEV_INFO*   ParentHubDev;
    UINT16      RetCode;

    if (!VALID_DEVINFO(DevInfo)) {
        return 0;
    }

//
// Get the device info structure for the matching device address
//
    //
    // Get the device number of the immediate hub, then get the device
    // info structure for this device number
    //
    for(;;) {
        ParentHubDev = USB_GetDeviceInfoStruc(USB_SRCH_DEV_ADDR,
                                0, HubDev->bHubDeviceNumber, 0);
        if (!ParentHubDev) {
            return 0; // Error. Exit !
        }
        if (((ParentHubDev->bEndpointSpeed << USB_PORT_STAT_DEV_SPEED_MASK_SHIFT)
             & USB_PORT_STAT_DEV_SPEED_MASK) == 0) {
             break;
        }
        HubDev = ParentHubDev;
    }
    //
    // The first USB 2.0 hub found as fpHubDev to which the low/full speed
    // device is connected
    //
    RetCode = (UINT16)((HubDev->bHubPortNumber << 7) |
                                HubDev->bHubDeviceNumber);

    return RetCode;
}

/**
    This function insert the requested QH to asynchronous schedule
    and waits until the QH completes or the transaction time-out.

    @param HcStruc   - Pointer to HCStruc of the host controller
        XferQh    - Pointer to the QH which has to be completed

    @retval USB_ERROR   On error
        USB_SUCCESS On success

**/

UINT16
EhciExexuteAsyncSchedule(
	HC_STRUC	*HcStruc,
	EHCI_QH 	*XferQh
)
{
	UINT16	Status = USB_SUCCESS;
	UINT32	Count;
    UINT32  TimeOut = gUsbData->wTimeOutValue * 100; // in 10 microsecond unit
    EHCI_QH *XferQhPhyAddr;
/*
#if EHCI_ASYNC_BELL_SUPPORT
	UINT32	Tmp;

	XferQh->dLinkPointer = EHCI_TERMINATE;
	XferQh->bActive = TRUE;

	//
	// Insert the Control/Bulk QH into the Async list
	//
	Tmp = gUsbData->fpQHAsyncXfer->dLinkPointer;
	gUsbData->fpQHAsyncXfer->dLinkPointer = (UINT32)XferQh | EHCI_QUEUE_HEAD;
	XferQh->dLinkPointer = Tmp;
#else
*/
	//
	// Set the ASYNCLISTADDR register to point to the Control/Bulk QH
	//
	XferQhPhyAddr = EhciMemGetPhyAddr(HcStruc, XferQh);
    HcWriteOpReg(HcStruc, EHCI_ASYNCLISTADDR, (UINT32)(UINTN)XferQhPhyAddr);

	//
	// Set next QH pointer to itself (circular link)
	//
	XferQh->dLinkPointer = (UINT32)((UINTN)XferQhPhyAddr | EHCI_QUEUE_HEAD);
	XferQh->bActive = TRUE;
//#endif

	//
    // Now put the Control/Bulk QH into the HC's schedule by
    // setting the Async. schedule enabled field of USBCMD register
    // This will cause the HC to execute the transaction in the next active frame.
    //
    Status = EHCIStartAsyncSchedule(HcStruc);
    
    if (Status == USB_ERROR) {
        return Status;
    }

	// Wait for tansfer complete
	for (Count = 0; !TimeOut || Count < TimeOut; Count++) {
		EHCIProcessQH(HcStruc, XferQh);
		if (XferQh->bActive == FALSE) {
			break;
		}
		FixedDelay(10);  // 10 microsec
	}

//#if EHCI_ASYNC_BELL_SUPPORT
    //
    // Disconnect Control/Bulk QH from the Async list
    //
//    EHCIRemoveQHFromAsyncList(HcStruc, XferQh);
//#else
	//
	// Stop the Async transfer
	//
	EHCIStopAsyncSchedule(HcStruc);
//#endif

	if (XferQh->bActive == TRUE) {
		XferQh->bActive = FALSE;
		Status = USB_ERROR;
		USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "EHCI Time-Out\n");
	}
	
	// Service all interrupts
	EHCI_ProcessInterrupt(HcStruc);

	return Status;
}

/**
    This function executes a device request command transaction
    on the USB. One setup packet is generated containing the
    device request parameters supplied by the caller.  The setup
    packet may be followed by data in or data out packets
    containing data sent from the host to the device
    or vice-versa. This function will not return until the
    request either completes successfully or completes in error
    (due to time out, etc.)

    @param HcStruc   Pointer to HCStruc of the host controller
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


    @retval Number of bytes actually transferred

**/

UINT16
EHCI_ControlTransfer(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT16      Request,
    UINT16      Index,
    UINT16      Value,
    UINT8       *Buffer,
    UINT16      Length
)
{
    UINT32      Tmp;
    UINT32      Tmp1;
    UINT16      RetCode = 0;   // Initialize with error
    EHCI_QH     *QhCtl;
    EHCI_QTD    *QtdCtlSetup;
    EHCI_QTD    *QtdCtlData;
    EHCI_QTD    *QtdCtlStatus;
    DEV_REQ     *DevRequest = NULL;
    UINT8       EndpointSpeed;
    UINT8       *BufPhyAddr = NULL;
    VOID        *BufferMapping = NULL;
    UINT8       *DevRequestPhyAddr = NULL;
    UINT8       *QhCtlPhyAddr = NULL;
    UINT8       *QtdCtlSetupPhyAddr = NULL;
    UINT8       *QtdCtlDataPhyAddr = NULL;
    UINT8       *QtdCtlStatusPhyAddr = NULL;
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
                USB_DEBUG(DEBUG_ERROR, 3, "Ehci ControlTransfer Invalid Pointer, Buffer is in SMRAM.\n");
                return 0;
            }
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return 0;
    }

	if (EhciIsHalted(HcStruc)) {
		return 0;
	}

    if (!VALID_DEVINFO(DevInfo)) {
        return 0;
    }
    
    gUsbData->dLastCommandStatusExtended = 0;	//(EIP84790+)

    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "EHCI_ControlTransfer..\n");
    //
    // Build the device request in the data area of the control setup qTD
    //
    DevRequest = EhciMemAlloc(HcStruc, GET_MEM_BLK_COUNT(sizeof(DEV_REQ)));
    
	ASSERT(DevRequest);
    
    if (DevRequest == NULL) {
        return 0;
    }

    DevRequestPhyAddr = EhciMemGetPhyAddr(HcStruc, DevRequest);

    DevRequest->wRequestType = Request;
    DevRequest->wIndex       = Index;
    DevRequest->wValue       = Value;
    DevRequest->wDataLength  = Length;

//
// The QH endpoint characteristic field will be set so
//   Function address & Endpoint number = From DeviceInfo structure,
//   Direction = From TD,
//   Speed = DeviceInfo.bEndpointSpeed,
//   Skip = 1, Format = 0,
//   Max packet size  = DeviceInfo.wEndp0MaxPacket
// The dNextqTDPtr field will be set to qTDControlSetup
// The dAltNextqTDPtr field will be set to EHCI_TERMINATE
// The dCurrentqTDPtr field will be set to 0
//
    //
    // Intialize the queue head with null pointers
    //
    QhCtl = EhciMemAlloc(HcStruc, GET_MEM_BLK_COUNT((1 * sizeof(EHCI_QH))+
                                             			(3 * sizeof(EHCI_QTD))));

    if (!QhCtl) {
        return 0;
    }

    QtdCtlSetup = (EHCI_QTD*)((UINTN)QhCtl + sizeof(EHCI_QH));
    QtdCtlData = (EHCI_QTD*)((UINTN)QtdCtlSetup + sizeof(EHCI_QTD));
    QtdCtlStatus = (EHCI_QTD*)((UINTN)QtdCtlData + sizeof(EHCI_QTD));

    QhCtlPhyAddr = EhciMemGetPhyAddr(HcStruc, QhCtl);
    QtdCtlSetupPhyAddr = EhciMemGetPhyAddr(HcStruc, QtdCtlSetup);
    QtdCtlDataPhyAddr = EhciMemGetPhyAddr(HcStruc, QtdCtlData);
    QtdCtlStatusPhyAddr = EhciMemGetPhyAddr(HcStruc, QtdCtlStatus);

    EHCIInitializeQueueHead(QhCtl);

    EndpointSpeed = DevInfo->bEndpointSpeed; // 00/01/10 for HI/LO/FULL

    //
    // Assume as a high speed device
    //
    Tmp = QH_HIGH_SPEED;   // 10b - High speed

    //
    // Check for high speed
    //
    if (EndpointSpeed) { // Low/Full speed device
        Tmp = ((UINT32)EndpointSpeed & 1) << 12;  // Bit 12 = full/low speed flag
        Tmp |= QH_CONTROL_ENDPOINT;
        //
        // Set the hub address and port number
        // Get the Hispeed hub port number & device number
        //
        Tmp1 = (UINT32)EHCI_GetHiSpeedHubPortNumber(DevInfo);
        Tmp1 = (Tmp1 << 16);		// Split complete Xaction
        QhCtl->dEndPntCap |= Tmp1;
    }

    //USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Tmp1..%x\n", dTmp);

    //
    // Use data toggle from qTD and this QH is the head of the queue
    //
//#if EHCI_ASYNC_BELL_SUPPORT
//    Tmp |= QH_USE_QTD_DT;
//#else
    Tmp |= QH_USE_QTD_DT;
    // Do not set QH_HEAD_OF_LIST bit on VIA controller
    if (HcStruc->Vid != 0x1106) {
        Tmp |= QH_HEAD_OF_LIST;
    }
//#endif
    Tmp |= (UINT32)DevInfo->bDeviceAddress;
    //
    // dTmp[Bits 6:0] = Dev. Addr
    // dTmp[Bit7] = I bit(0)
    // dTmp[Bits11:8] = Endpoint (0)
    //

    Tmp1 = (UINT32)DevInfo->wEndp0MaxPacket;
    Tmp |= (Tmp1 << 16);  // Tmp[Bits26:16] = device's packet size
    QhCtl->dEndPntCharac  = Tmp;

    //
    // Fill in various fields in the qTDControlSetup.
    //
    //fpQTDCtlSetup = HcStruc->stDescPtrs.fpEHCIDescPtrs->fpqTDControlSetup;	//(EIP71067-)

    //
    // The token field will be set so
    //   Direction PID = QTD_SETUP_TOKEN,
    //   Size = size of the data,
    //   Data Toggle = QTD_SETUP_TOGGLE,
    //   Error Count = QTD_THREE_ERRORS,
    //   Status code = QTD_DO_OUT + QTD_ACTIVE
    // The buffer pointers field will point to the aControlSetupData buffer
    //   which was before initialized to contain a DeviceRequest struc.
    // The dNextqTDPtr field will point to the qTDControlData if data will
    //   be sent/received or to the qTDControlStatus if no data is expected.
    // The dAltNextqTDPtr field will be set to EHCI_TERMINATE
    //
    QtdCtlSetup->dToken = QTD_SETUP_TOKEN |
                QTD_SETUP_TOGGLE | QTD_IOC_BIT |
                QTD_THREE_ERRORS | QTD_DO_OUT | QTD_ACTIVE |
                (8 << 16);  // Data size

    //
    // Update buffer pointers
    //
    EHCISetQTDBufferPointers(QtdCtlSetup, (UINT8*)DevRequestPhyAddr, 8);
    //fpQTDCtlData = HcStruc->stDescPtrs.fpEHCIDescPtrs->fpqTDControlData;	//(EIP71067-)

    if (Length) {      // br if no data to transfer
        //
        // Fill in various fields in the qTDControlData
        //
        // The token field will be set so
        //   Direction PID = QTD_OUT_TOKEN/QTD_IN_TOKEN,
        //   Size = size of the data,
        //   Data Toggle = QTD_DATA1_TOGGLE,
        //   Error Count = QTD_THREE_ERRORS,
        //   Status code = QTD_DO_OUT(if it is out) + QTD_ACTIVE
        // The buffer pointers field will point to the fpBuffer buffer
        //   which was before initialized to contain a DeviceRequest struc.
        // The dNextqTDPtr field will point to the qTDControlSetup
        // The dAltNextqTDPtr field will be set to EHCI_TERMINATE
        //
        QtdCtlData->dToken = QTD_IN_TOKEN |
                QTD_DATA1_TOGGLE | QTD_IOC_BIT |
                QTD_THREE_ERRORS | QTD_ACTIVE;
        if (!(Request & BIT7)) // Br if host sending data to device (OUT)
        {
            QtdCtlData->dToken    = QTD_OUT_TOKEN |
                QTD_DATA1_TOGGLE | QTD_IOC_BIT |
                QTD_THREE_ERRORS | QTD_DO_OUT | QTD_ACTIVE;
        }

        //
        // Set length
        //
        QtdCtlData->dToken |= ((UINT32)Length << 16);

		HcDmaMap(HcStruc, (UINT8)(Request & BIT7), Buffer, Length, 
			&BufPhyAddr, &BufferMapping);
        
        //
        // Update buffer pointers
        //
        EHCISetQTDBufferPointers(QtdCtlData,
                (UINT8*)BufPhyAddr,
                (UINT32)Length);
    }

    //
    // Fill in various fields in the qTDControlStatus
    //
    //fpQTDCtlStatus = HcStruc->stDescPtrs.fpEHCIDescPtrs->fpqTDControlStatus;	//(EIP71067-)

    //
    // The token field will be set so
    //   Direction PID = QTD_OUT_TOKEN/QTD_IN_TOKEN,
    //   Size = 0,
    //   Data Toggle = QTD_DATA1_TOGGLE,
    //   Error Count = QTD_THREE_ERRORS,
    //   Status code = QTD_DO_OUT(if it is out) + QTD_ACTIVE
    // The buffer pointers field will be 0
    // The dNextqTDPtr field will set to EHCI_TERMINATE
    // The dAltNextqTDPtr field will be set to EHCI_TERMINATE
    //
    // For OUT control transfer status should be IN and
    // for IN cotrol transfer, status should be OUT
    //
    QtdCtlStatus->dToken = QTD_IN_TOKEN |
                QTD_DATA1_TOGGLE | QTD_IOC_BIT |
                QTD_THREE_ERRORS | QTD_ACTIVE;
    if (Request & BIT7) {
        QtdCtlStatus->dToken  = QTD_OUT_TOKEN |
                QTD_DATA1_TOGGLE | QTD_IOC_BIT |
                QTD_THREE_ERRORS | QTD_DO_OUT | QTD_ACTIVE;
    }

    EHCISetQTDBufferPointers(QtdCtlStatus, NULL, 0);

    //
    // Link the qTD formed now and connect them with the control queue head
    //
    QhCtl->fpFirstqTD     = QtdCtlSetup;
    QhCtl->dNextqTDPtr    = (UINT32)(UINTN)QtdCtlSetupPhyAddr;

    if(Length) {
        QtdCtlSetup->dNextqTDPtr  = (UINT32)(UINTN)QtdCtlDataPhyAddr;
        QtdCtlData->dNextqTDPtr   = (UINT32)(UINTN)QtdCtlStatusPhyAddr;
    } else {
        QtdCtlSetup->dNextqTDPtr  = (UINT32)(UINTN)QtdCtlStatusPhyAddr;
    }

    QtdCtlStatus->dNextqTDPtr = EHCI_TERMINATE;

	RetCode = EhciExexuteAsyncSchedule(HcStruc, QhCtl);
	QhCtl->fpFirstqTD     = 0;
	QhCtl->dNextqTDPtr    = EHCI_TERMINATE;

	if (Length) {
		HcDmaUnmap(HcStruc, BufferMapping);
        Length = Length - (UINT16)((QtdCtlData->dToken & ~(QTD_DATA_TOGGLE)) >> 16);
	}

    //
    // Clear the stalled condition flag
    //
    gUsbData->bLastCommandStatus &= ~USB_CONTROL_STALLED;

    //
    // Check whether the QH stopped or timed out
    //
    if (RetCode != USB_SUCCESS) {
        gUsbData->dLastCommandStatusExtended |= USB_TRNSFR_TIMEOUT;	//(EIP84790+)
		Length = 0;                    //(EIP71067)
	}

	if (QhCtl->bErrorStatus & QTD_HALTED) {
        //
        // Command stalled set the error bit appropriately
        //
        gUsbData->bLastCommandStatus   |= USB_CONTROL_STALLED;
        gUsbData->dLastCommandStatusExtended  |= USB_CONTROL_STALLED;
		Length = 0;                    //(EIP71067)
    }
                                        //(EIP71067+)>
    EhciMemFree(HcStruc, QhCtl, GET_MEM_BLK_COUNT((1 * sizeof(EHCI_QH))+
                                           (3 * sizeof(EHCI_QTD))));
                                        //<(EIP71067+)
	EhciMemFree(HcStruc, DevRequest, GET_MEM_BLK_COUNT(sizeof(DEV_REQ)));

	return Length;
}


/**
    This function executes a bulk transaction on the USB. The
    transfer may be either DATA_IN or DATA_OUT packets containing
    data sent from the host to the device or vice-versa. This
    function wil not return until the request either completes
    successfully or completes with error (due to time out, etc.)
    @note  Make sure that amount of bytes to transfer should not
              exceed MAX_EHCI_DATA_SIZE

    @param pHCStruc    Pointer to HCStruc of the host controller
        pDevInfo    DeviceInfo structure (if available else 0)
        bXferDir    Transfer direction
        Bit 7: Data direction
        0 Host sending data to device
        1 Device sending data to host
        Bit 6-0 : Reserved
        fpBuffer    Buffer containing data to be sent to the
        device or buffer to be used to receive data
        value in Segment:Offset format
        dwLength    dwLength request parameter, number of bytes
        of data to be transferred in or out
        of the host controller

    @retval Amount of data transferred

**/

UINT32
EHCI_BulkTransfer(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       XferDir,
    UINT8       *Buffer,
    UINT32      Length
)
{
    UINT16      MaxPkt;
    UINT8       Endp;
    UINT8       DatToggle;
    EHCI_QH     *QhBulk;
    EHCI_QTD    *QtdBulkData;
    UINT32      Tmp;
    UINT32      Tmp1;
    UINT16      Status;
    UINT32      BytesToTransfer;
    UINT32      BytesRemaining;
    UINT32      BytesTransferred;
    UINT8       *BufPhyAddr = NULL;
    VOID        *BufferMapping = NULL;
    UINT8       *TempBuffer = NULL;
    UINT8       *QhBulkPhyAddr;
    UINT8       *QtdBulkDataPhyAddr;
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
            USB_DEBUG(DEBUG_ERROR, 3, "Ehci BulkTransfer Invalid Pointer, Buffer is in SMRAM.\n");
            return 0;
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return 0;
    }

	if (EhciIsHalted(HcStruc)) {
		return 0;
	}

    if (!VALID_DEVINFO(DevInfo)) {
        return 0;
    }

    // Realtek 8111EP EHCI controller workaround
    // The controller doesn't work if the buffer address isn't DWORD alignment 
    // (current offset of qTD). Provide the workaround to locate DWORD alignment buffer.
    
    if ((HcStruc->Vid == 0x10EC) && (HcStruc->Did == 0x816D)) {
        if ((UINTN)Buffer & (BIT0 | BIT1)) {
            if (Length < HIGHSPEED_MAX_BULK_DATA_SIZE) {
                TempBuffer = EhciMemAlloc(HcStruc, GET_MEM_BLK_COUNT((UINT16)Length));
            } else {
                TempBuffer = EhciMemAlloc(HcStruc, GET_MEM_BLK_COUNT(HIGHSPEED_MAX_BULK_DATA_SIZE));  
            }
        }
    }

    //clear HW source of error
    gUsbData->dLastCommandStatusExtended = 0;


    BytesRemaining     = Length;
    BytesTransferred   = 0;
		
	//
	// Get Bulk IN/OUT enpoint number, data sync value & max packet size
	//
	if (XferDir & BIT7) {
        MaxPkt = DevInfo->wBulkInMaxPkt;
        Endp   = DevInfo->bBulkInEndpoint;
    } else {
        MaxPkt = DevInfo->wBulkOutMaxPkt;
        Endp   = DevInfo->bBulkOutEndpoint;
    }
    if (MaxPkt == 0) {
        return 0;
    }

    QhBulk = EhciMemAlloc(HcStruc, GET_MEM_BLK_COUNT((1 * sizeof(EHCI_QH))+
                                              		(1 * sizeof(EHCI_QTD))));

    if (!QhBulk) {
        return 0;
    }

    QtdBulkData = (EHCI_QTD*)((UINTN)QhBulk + sizeof(EHCI_QH));

    QhBulkPhyAddr = EhciMemGetPhyAddr(HcStruc, QhBulk);
    QtdBulkDataPhyAddr = EhciMemGetPhyAddr(HcStruc, QtdBulkData);

	HcDmaMap(HcStruc, XferDir, Buffer, Length, &BufPhyAddr, &BufferMapping);

    while (BytesRemaining) {
        BytesToTransfer = 
             (BytesRemaining < HIGHSPEED_MAX_BULK_DATA_SIZE)?
                    BytesRemaining : HIGHSPEED_MAX_BULK_DATA_SIZE;
		
		//
		// Get data toggle value
		//
    	DatToggle = UsbGetDataToggle(DevInfo, Endp | XferDir);

        //
        // Set the QH's dNextqTDPtr field to bulk data qTD and dAltqTDPtr field to
        // EHCI_TERMINATE. Also set QH's link pointer to itself
        //
                                        //(EIP71067-)>
        //fpQHBulk        = HcStruc->stDescPtrs.fpEHCIDescPtrs->fpQHBulk;
        //fpQTDBulkData   = HcStruc->stDescPtrs.fpEHCIDescPtrs->fpqTDBulkData;
                                        //<(EIP71067-)
        //
        // Intialize the queue head
        //
        EHCIInitializeQueueHead(QhBulk);
    
        //
        // Set the first qTD pointer
        //
        QhBulk->fpFirstqTD    = QtdBulkData;
        QhBulk->dNextqTDPtr   = (UINT32)(UINTN)QtdBulkDataPhyAddr;
        QhBulk->dLinkPointer  = (UINT32)((UINTN)QhBulkPhyAddr | EHCI_QUEUE_HEAD);
    
        //
        // Device address & Endpoint
        //
        Tmp = (UINT32)(DevInfo->bDeviceAddress | (Endp << 8));
    
        //
        // Set max packet size
        //
        Tmp = Tmp | ((UINT32)MaxPkt << 16);
    
        //
        // Set the data toggle control
        //
//    #if EHCI_ASYNC_BELL_SUPPORT
//        Tmp |= QH_USE_QTD_DT;
//    #else
        Tmp |= QH_USE_QTD_DT;
        // Do not set QH_HEAD_OF_LIST bit on VIA controller
        if (HcStruc->Vid != 0x1106) {
            Tmp |= QH_HEAD_OF_LIST;
        }
//    #endif
    
        //
        // Set the device speed
        // Reset the device speed bits
        //
        Tmp1 = (UINT32)DevInfo->bEndpointSpeed;    // 00/01/10 for HI/LO/FULL
    
        //
        // Assume as a high speed device
        //
        Tmp |= QH_HIGH_SPEED;   // 10b - High speed
    
        //
        // Check for high speed
        //
        if (Tmp1) {
            Tmp1   = (Tmp1 & 1) << 12;    // Bit 12 = full/low speed flag
            Tmp    &= ~(QH_ENDPOINT_SPEED);
            Tmp    |= Tmp1;
            //
            // Set the hub address and port number
            //
            Tmp1   = (UINT32)EHCI_GetHiSpeedHubPortNumber(DevInfo);
            Tmp1   = (Tmp1 << 16);    	// Hispeed hub port number & device number
            QhBulk->dEndPntCap    |= Tmp1;   // Split complete Xaction
        }
    
        //
        // Update the endpoint characteristcs field with the data formed
        //
        QhBulk->dEndPntCharac = Tmp;
    
        //
        // Fill the bulk data qTD with relevant information
        // The token field will be set so
        //   Direction PID = QTD_OUT_TOKEN/QTD_IN_TOKEN,
        //   Size = size of the data,
        //   Data Toggle = bDatToggle,
        //   Error Count = QTD_THREE_ERRORS,
        //   Status code = QTD_DO_OUT(if it is out) + QTD_ACTIVE
        // The buffer pointers field will point to the fpBuffer buffer
        //   which was before initialized to contain a DeviceRequest struc.
        // The dNextqTDPtr field will point to the qTDControlSetup
        // The dAltNextqTDPtr field will be set to EHCI_TERMINATE
        //
        if (XferDir & BIT7) {
            QtdBulkData->dToken   = QTD_IN_TOKEN |
                    QTD_IOC_BIT |
                    QTD_THREE_ERRORS | QTD_ACTIVE;
        } else {
            QtdBulkData->dToken   = QTD_OUT_TOKEN |
                    QTD_IOC_BIT |
                    QTD_THREE_ERRORS | QTD_DO_OUT | QTD_ACTIVE;
            if (TempBuffer != NULL) {
                CopyMem(TempBuffer, BufPhyAddr, BytesToTransfer);
            }            
        }
    
        //
        // Set the data toggle depending on the bDatToggle value
        //
        QtdBulkData->dToken   |= (UINT32)DatToggle << 31;
    
        //
        // Set length
        //
        QtdBulkData->dToken   |= (BytesToTransfer << 16);
    
        //
        // Update buffer pointers
        //
        if (TempBuffer != NULL) {
            EHCISetQTDBufferPointers(QtdBulkData, TempBuffer, BytesToTransfer);
        } else {
            EHCISetQTDBufferPointers(QtdBulkData, BufPhyAddr, BytesToTransfer);
        }
    
        //
        // Update next & alternate next qTD pointers
        //
        QtdBulkData->dNextqTDPtr      = EHCI_TERMINATE;
        QtdBulkData->dAltNextqTDPtr   = EHCI_TERMINATE;
    
        QhBulk->bActive   = TRUE;
    
    
        //
        // Set bulk condition as not stalled
        //
        gUsbData->bLastCommandStatus &= ~(USB_BULK_STALLED + USB_BULK_TIMEDOUT);
    
        //
        // Now wait for bulk transaction to be complete
        // the EHCIProcessInterrupt will set its active flag to FALSE.
        // Now wait for the bulk transfer to complete
        //
		Status = EhciExexuteAsyncSchedule(HcStruc, QhBulk);

		QhBulk->fpFirstqTD	= 0;
		QhBulk->dNextqTDPtr	= EHCI_TERMINATE;

		if (Status != USB_SUCCESS) {
			//
            // Set time out status
            //
            gUsbData->bLastCommandStatus |= USB_BULK_TIMEDOUT;
            gUsbData->dLastCommandStatusExtended  |= USB_TRNSFR_TIMEOUT;
			break;
		}

		if (QhBulk->bErrorStatus & QTD_HALTED) {
			//
			// Stall condition
			//
            gUsbData->bLastCommandStatus   &= ~(USB_BULK_TIMEDOUT);
            gUsbData->bLastCommandStatus   |= USB_BULK_STALLED;
            gUsbData->dLastCommandStatusExtended  |= USB_TRSFR_STALLED;
			break;
		}

        //
        // Update the data toggle value into the mass info structure
        //
        DatToggle =
            (UINT8)(((QhBulk->dToken & QH_DATA_TOGGLE) >> 31) & 1);
		UsbUpdateDataToggle(DevInfo, Endp | XferDir, DatToggle);

        //
        // Get the size of data transferred
        //
        Tmp = (QtdBulkData->dToken & ~(QTD_DATA_TOGGLE)) >> 16;
        Tmp = (Tmp)? BytesToTransfer - Tmp : BytesToTransfer;

        if (!Tmp) {
            break;
        }

        if (TempBuffer != NULL) {
            if (XferDir & BIT7) {
                CopyMem(BufPhyAddr, TempBuffer, Tmp);
            }
        }

        //
        // Adjust loop variables
        //
        BytesRemaining = BytesRemaining - Tmp;
        BytesTransferred += Tmp;

        if (Tmp < BytesToTransfer) {
            break;
        }

        BufPhyAddr += Tmp;
    }

	HcDmaUnmap(HcStruc, BufferMapping);
                                        //(EIP71067+)>
    EhciMemFree(HcStruc, QhBulk, GET_MEM_BLK_COUNT( (1 * sizeof(EHCI_QH)) +
                                             	    (1 * sizeof(EHCI_QTD))));
                                        //<(EIP71067+)
    if (TempBuffer != NULL) {
        if (Length < HIGHSPEED_MAX_BULK_DATA_SIZE) {
            EhciMemFree(HcStruc, TempBuffer, GET_MEM_BLK_COUNT((UINT16)Length));     
        } else {
            EhciMemFree(HcStruc, TempBuffer, GET_MEM_BLK_COUNT(HIGHSPEED_MAX_BULK_DATA_SIZE));
        }
    }
    
    return  BytesTransferred;

}

/**
 *  This function calculates Isochronous TD buffer addresses
 */
typedef struct _ITD_PG_ADDRESS {
    UINT32 TrBuffer;
    BOOLEAN NewPgNeeded;
} ITD_PG_ADDRESS;

EFI_STATUS CalculateItdBuffers(ITD_PG_ADDRESS* ItdAddress, UINT32* Address)
{
    UINT8 i;
    UINT32 ItdTrOffset;
    BOOLEAN NewPgNeeded;

    for (i = 0; i < 8; i++)
    {
        ItdAddress[i].TrBuffer = *Address;
        ItdTrOffset = *Address & 0xfff;
        NewPgNeeded = ItdTrOffset >= EHCI_ITD_PAGE_MAX_OFFSET;
        ItdAddress[i].NewPgNeeded = NewPgNeeded;
        *Address += USB_ISOC_XFER_MEM_LENGTH;
    }
    // can't have a new page on 7th TR record; however if the record can hold the data we also return success
    return (NewPgNeeded && ItdTrOffset != EHCI_ITD_PAGE_MAX_OFFSET)? EFI_INVALID_PARAMETER : EFI_SUCCESS;
}

/**
 * This function creates ITDs for the isochronous transfer
 */
// Note: this function uses current frame index as input. Any delays in this
// function (debug messages, stalls, etc.) affect async transfer since "current"
// frame will not be current.
UINT16 CreateITDs(
    HC_STRUC    *Hc,
    DEV_INFO    *Device,
    UINT16      Index,
    UINT8       *Buffer,
    UINT32      Length,
    UINT8       XferDir,
    UINT8       *AsyncStatus
)
{
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    UINT16      Count;
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
    UINT8       Frame;
    UINT16      FrameCount = 1;
    volatile EHCI_ITD    *Itd;
    UINT32      Address = (UINT32)(UINTN)Buffer;
    UINT32      Page;
    ITD_PG_ADDRESS ItdAddress[8];
    UINT16      CurrentIndex = Index;
    UINT8       Endpoint = Device->IsocDetails.Endpoint & 0xf;
    UINT32      XferRecLength = Device->IsocDetails.EpMaxPkt * Device->IsocDetails.EpMult;
    UINT32      ScheduledDataLength = 0;
    EFI_STATUS  Status;

// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
    for (Count = 0; Count < Hc->wAsyncListSize; Count++)
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
    {
        Status = CalculateItdBuffers(ItdAddress, &Address);
        if (EFI_ERROR(Status))
        {
            // can't have a new page on 7th TR record, adjust address and redo this iTD
            Address = (ItdAddress[0].TrBuffer & 0xfffff000) + 0x1000;
            Status = CalculateItdBuffers(ItdAddress, &Address);
            ASSERT_EFI_ERROR(Status);   // second call must be successful
        }

        // Fill the iTD
        Page = 0;
        Itd = (EHCI_ITD*)(UINTN)Hc->fpFrameList[CurrentIndex++];
        if (CurrentIndex == Hc->wAsyncListSize) CurrentIndex = 0;

        ASSERT_EFI_ERROR(Status);
        Itd->BufferPointer[0] = ItdAddress[0].TrBuffer & 0xfffff000;

        for (Frame = 0; Frame < 8; Frame++, FrameCount++)
        {
            Itd->ControlStatus[Frame] = (EHCI_ISOC_MAX_REC_DATA_SIZE << 16);    // transaction length
            Itd->ControlStatus[Frame] |= ItdAddress[Frame].TrBuffer & 0xfff;   // transaction offset
            Itd->ControlStatus[Frame] |= (Page << 12);        // page
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
            if(AsyncStatus) {
                Itd->ControlStatus[Frame] |= BIT15;   // Interrupt on completion
            }
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
            if (ItdAddress[Frame].NewPgNeeded && (Frame != 7))
            {
                Page++;
                Itd->BufferPointer[Page] = ItdAddress[Frame+1].TrBuffer & 0xfffff000;
            }
            ScheduledDataLength += XferRecLength;
            if (ScheduledDataLength >= Length) break;
        }

        Itd->BufferPointer[0] |= (Device->bDeviceAddress | ((UINT16)Endpoint << 8));
        Itd->BufferPointer[1] |= Device->IsocDetails.EpMaxPkt;
        Itd->BufferPointer[2] |= Device->IsocDetails.EpMult;    // # of transaction per microframe
        if (XferDir != 0)
        {
            Itd->BufferPointer[1] |= BIT11;             // BIT11 0 - out, 1 - in
        }

        if (ScheduledDataLength >= Length) break;
    }

    Device->IsocDetails.AsyncStatus = AsyncStatus;
    if (AsyncStatus)
    {
// APTIOV_SERVER_OVERRIDE_START : To fix Coverity tool Issue.
//        Itd->ControlStatus[Frame] |= BIT15;   // Interrupt on completion
// APTIOV_SERVER_OVERRIDE_END : To fix Coverity tool Issue.
        Device->IsocDetails.XferStart = (UINTN)Index;
        Device->IsocDetails.XferKey = (UINTN)Itd;
    }    
    return FrameCount;
}

/**
 * This function activates or deactivates the given range of ITDs
 */
EHCI_ITD* ActivateDeactivateITDs(
    HC_STRUC    *HcStruc,
    UINT16      FrmNum,
    UINT16      FrameCount,
    UINT8       *Frame,
    BOOLEAN     Activate
)
{
    UINT16  FrameTotal = 0;
    UINT16  FrameNum = FrmNum;
    EHCI_ITD *Itd;
    UINT8   i;

    while (1)
    {
        Itd = (EHCI_ITD*)(UINTN)HcStruc->fpFrameList[FrameNum++];
        if (FrameNum == HcStruc->wAsyncListSize) FrameNum = 0;

        for (i = 0; i < 8; i++)
        {
            if (Activate)
                Itd->ControlStatus[i] |= 0x80000000;
            else
                Itd->ControlStatus[i] &= 0x7fffffff;
            
            FrameTotal++;
            if (FrameTotal == FrameCount)
            {
                if (Frame != NULL)
                    *Frame = i;
                return Itd;
            }
        }
    }
}

/**
 * This function returns the current frame index
 */
UINT16 GetCurrentFIndex(HC_STRUC* Hc)
{
    UINT32  Data = HcReadOpReg(Hc, EHCI_FRINDEX);
    return (UINT16)((Data >> 3) & (Hc->wAsyncListSize - 1));
}


/**
    For a given range of micro-frames update the map of transfer size with
    the individual micro-frame transfer length. Return the total transfer
    data amount.
**/
UINT32 FinalizeIsocXfer(
    HC_STRUC    *HcStruc,
    UINT16      Index,
    UINT16      FrameCount,
    UINT32      *FrameData
)
{
    UINT16  FrameTotal = 0;
    UINT16  Frame = Index;
    volatile EHCI_ITD *Itd;
    UINT8   ControlStatusIndex;
    UINT32  TotalAmount = 0;
    UINT32  FrameXferBytes;
    UINT16  i;

    for (i = 0; i < HcStruc->wAsyncListSize; i++) {
        Itd = (EHCI_ITD*)(UINTN)HcStruc->fpFrameList[Frame++];
        if (Frame == HcStruc->wAsyncListSize) Frame = 0;

        for (ControlStatusIndex = 0; ControlStatusIndex < 8; ControlStatusIndex++) {
            FrameXferBytes = 0;
            if ((Itd->ControlStatus[ControlStatusIndex] & 0x80000000) == 0) {
                FrameXferBytes = ((Itd->ControlStatus[ControlStatusIndex] >> 16) & 0xfff);
                TotalAmount += FrameXferBytes;
            } else {
                Itd->ControlStatus[ControlStatusIndex] &= 0x7fffffff;  // deactivate microframe that has not been processed
            }

            FrameTotal++;
            if (FrameData != NULL) {
                *FrameData++ = FrameXferBytes;
            }

            if (FrameTotal == FrameCount) {
                return TotalAmount;
            }
        }
    }
    ASSERT(FALSE);
    return 0;
}

/**
    This function executes a isochronous transaction on the USB. The transfer may be
    either DATA_IN or DATA_OUT packets containing data sent from the host to the
    device or vice-versa. Depending on the value passed in AsyncIndicator this transfer
    would either be a blocking (synchronous) or non-blocking (asynchronous).

    @param[in]  HcStruc    Pointer to HCStruc of the host controller
    @param[in]  DevInfo    DeviceInfo structure (if available else 0)
    @param[in]  XferDir    Transfer direction
                  Bit 7: Data direction
                          0 Host sending data to device
                          1 Device sending data to host
                  Bit 6-0 : Reserved
    @param[in]  Buffer    32-bit buffer containing data to be sent to the device or buffer
                      to be used to receive data
    @param[in]  Length    Number of bytes of data to be transferred in or out
    @param[in]  AsyncIndicator  If NULL, the call is not returned until all data is transferred
                                if not NULL, it points to the UINT8 data which will be updated
                                when transfer is complete.

    @param[out] For blocking transfer - amount of data transferred, for non-blocking transfer 0
    @param[out] Transfer status is updated in gUsbData->bLastCommandStatus and in 
                gUsbData->dLastCommandStatusExtended

    @note       Isochronous transfer implies no checking for the data transmission
          errors, i.e. transfer completes successfully when the last iTD becomes inactive.
**/
UINT32
EHCI_IsocTransfer(
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       XferDir,
    UINT8       *Buffer,
    UINT32      Length,
    UINT8       *AsyncIndicator
)
{
    UINT16      CurrentFIndex;
    UINT32      MaxXferLength;
    UINT16      FrameCount;
    UINT16      TransferTimeout15us;
    UINT8       Frame;
    volatile    EHCI_ITD    *Itd;
    UINT16      Count;
    UINT32      TotalAmount;
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
            USB_DEBUG(DEBUG_ERROR, 3, "Ehci IsocTransfer Invalid Pointer, Buffer is in SMRAM.\n");
            return 0;
        }
        if (AsyncIndicator) {
            EfiStatus = AmiValidateMemoryBuffer((VOID*)AsyncIndicator, sizeof(UINT8));
            if (EFI_ERROR(EfiStatus)) {
                return 0;
            }
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return 0;
    }

	if (EhciIsHalted(HcStruc)) {
		return 0;
	}

    if (HcStruc->IsocTds == NULL) {
        return 0;
    }

    // see if the requested length is acceptable
    MaxXferLength = DevInfo->IsocDetails.EpMaxPkt * DevInfo->IsocDetails.EpMult * 8 * HcStruc->wAsyncListSize;

//    USB_DEBUG(DEBUG_INFO, 3, "EHCI_IsochXfer: buffer %x length %x maxxferlength %x = isocmax(%x)*isocmult(%x)*asynclistsize(%x)\n", Buffer, Length, MaxXferLength,
//            DevInfo->IsocDetails.EpMaxPkt, DevInfo->IsocDetails.EpMult, HcStruc->wAsyncListSize);
    
    if (Length > MaxXferLength)
    {
        ASSERT(FALSE);
        return 0;   // too much to ask
    }

    gUsbData->dLastCommandStatusExtended = 0;

    // initialize and activate the necessary number of iTDs

    CurrentFIndex = GetCurrentFIndex(HcStruc);
    // advance by, say, one 1 ms frame to avoid losing first TD
    CurrentFIndex += 1;
    if (CurrentFIndex == HcStruc->wAsyncListSize)
    {
        CurrentFIndex = 0;
    }

    FrameCount = CreateITDs(HcStruc, DevInfo, CurrentFIndex, Buffer, Length, XferDir, AsyncIndicator);
    Itd = ActivateDeactivateITDs(HcStruc, CurrentFIndex, FrameCount, &Frame, TRUE);

    if (AsyncIndicator != NULL)
    {
        *AsyncIndicator = 0;    // clear indicator, to be set by ProcessInterrupt upon transfer completion
        return 0;
    }
    
    // calculate the timeout
    TransferTimeout15us = (FrameCount << 3) + 1;  // 125 us / 8 = 15 us

    // wait for transfer completion
    for (Count = 0; Count < TransferTimeout15us; Count++)
    {
        if ((Itd->ControlStatus[Frame] & 0x80000000) == 0) break;
        FixedDelay(15);  // 15 microsec
    }
#if !USB_RT_DXE_DRIVER
    EfiStatus = AmiValidateMemoryBuffer((VOID*)DevInfo->IsocDetails.XferDetails, FrameCount * sizeof(UINT32));
    if (EFI_ERROR(EfiStatus)) {

        return 0;
    }
#endif
    TotalAmount = FinalizeIsocXfer(HcStruc, CurrentFIndex, FrameCount, DevInfo->IsocDetails.XferDetails);
    //USB_DEBUG(DEBUG_INFO, 3, "EHCI ISOC: %d uFrames %d Bytes\n", uFrameCount, TotalAmount);
    if (Count == TransferTimeout15us)
    {
        gUsbData->dLastCommandStatusExtended |= USB_TRNSFR_TIMEOUT;
    }

    return TotalAmount;
}


/**
    This function executes an interrupt transaction on the USB.
    The data transfer direction is always DATA_IN. This
    function will not return until the request either completes
    successfully or completes in error (due to time out, etc.)

    @param HcStruc   Pointer to HCStruc of the host controller
        DevInfo   DeviceInfo structure (if available else 0)
        EndpointAddress The destination USB device endpoint to which the device request 
                    is being sent.
        MaxPktSize  Indicates the maximum packet size the target endpoint is capable 
        of sending or receiving.
        fpBuffer    Buffer containing data to be sent to the
        device or buffer to be used to receive data
        wLength     wLength request parameter, number of bytes
        of data to be transferred in

    @retval Number of bytes transferred


    @note  DO NOT TOUCH THE LINK POINTER OF THE TDInterruptData. It is
              statically allocated and linked with other items in the
              1ms schedule

**/

UINT16
EHCI_InterruptTransfer (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       EndpointAddress,
    UINT16      MaxPktSize,
    UINT8       *Buffer,
    UINT16      Length
)
{

    UINT8       DatToggle;
    EHCI_QH     *QhInt;
    UINT32      Tmp;
    UINT32      Tmp1;
    EHCI_QTD    *QtdIntData;
	UINT32		Count;
	UINT32		Timeout;
	UINT32		BytesTransferred;
	UINT8		*BufPhyAddr = NULL;
	VOID		*BufferMapping = NULL;
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
            USB_DEBUG(DEBUG_ERROR, 3, "Ehci InterruptTransfer Invalid Pointer, Buffer is in SMRAM.\n");
            return 0;
        }
        gCheckUsbApiParameter = FALSE;
    }
#endif

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return 0;
    }

	if (EhciIsHalted(HcStruc)) {
		return 0;
	}

	gUsbData->dLastCommandStatusExtended = 0;

	DatToggle = UsbGetDataToggle(DevInfo, EndpointAddress);

    //
    // Get the QHInterrupt pointer
    //
    QhInt = EhciMemAlloc(HcStruc, GET_MEM_BLK_COUNT((1 * sizeof(EHCI_QH))+
                                             		(1 * sizeof(EHCI_QTD))));
	if (QhInt == NULL) {
		return 0;
	}

	QtdIntData = (EHCI_QTD*)((UINTN)QhInt + sizeof(EHCI_QH));

	//
	// Intialize the queue head
	//
	EHCIInitializeQueueHead(QhInt);

	//
	// Set the first qTD pointer
	//
	QhInt->fpFirstqTD	= QtdIntData;
	QhInt->dNextqTDPtr	= (UINT32)(UINTN)QtdIntData;
	QhInt->dLinkPointer	= EHCI_TERMINATE;

    //
    // Get Device address & Endpoint
    //
    Tmp = (UINT32)DevInfo->bDeviceAddress;
    Tmp |= (UINT32)(EndpointAddress & 0xF) << 8;

    //
    // Set max packet size
    //
    Tmp |= (UINT32)(MaxPktSize) << 16;

    //
    // Set the device speed, reset the device speed bits
    //
    Tmp1 = (UINT32)DevInfo->bEndpointSpeed;  // 00/01/10 for HI/LO/FULL

    //
    // Assume as a high speed device
    //
    Tmp |= QH_HIGH_SPEED;  // 10b - High speed

    //
    // Check for high speed
    //
    if (Tmp1) {
        Tmp1 = (Tmp1 & 1) << 12;  // Bit 12 = full/low speed flag
        Tmp &= ~(QH_ENDPOINT_SPEED);
        Tmp |= Tmp1;
        //
        // Set the hub address and port number
        //
        Tmp1 = (UINT32)EHCI_GetHiSpeedHubPortNumber(DevInfo);
        Tmp1 = (Tmp1 << 16) | BIT10 | BIT11 | BIT12;
        QhInt->dEndPntCap |= Tmp1;   // Split complete Xaction
    }
    //
    // Update the endpoint characteristcs field with the data formed
    //
    QhInt->dEndPntCharac = Tmp;
    QhInt->dEndPntCap |= (BIT0 | QH_ONE_XFER);    // Interrupt schedule mask
    QhInt->Interval = EhciTranslateInterval(DevInfo->bEndpointSpeed, 
                            DevInfo->bPollInterval);

    //
    // Set the data toggle depending on the bDatToggle value
    //
    QhInt->dToken |= (UINT32)DatToggle << 31;

//
// Fill the interrupt data qTD with relevant information
// The token field will be set so
//   Direction PID = QTD_IN_TOKEN,
//   Size = size of the data,
//   Data Toggle = bDatToggle,
//   Error Count = QTD_THREE_ERRORS,
//   Status code = QTD_ACTIVE
// The buffer pointers field will point to the EDX
//   which was before initialized to contain a DeviceRequest struc.
// The dNextqTDPtr field will point to the qTDControlSetup
// The dAltNextqTDPtr field will be set to EHCI_TERMINATE
//
    QtdIntData->dToken = QTD_IOC_BIT | QTD_THREE_ERRORS | QTD_ACTIVE;
	if (EndpointAddress & BIT7) {
	    QtdIntData->dToken |= QTD_IN_TOKEN;
	} else {
	    QtdIntData->dToken |= QTD_OUT_TOKEN;
	}

    //
    // Set length
    //
	QtdIntData->dToken |= (UINT32)Length << 16;
	
	HcDmaMap(HcStruc, EndpointAddress & BIT7, Buffer, Length, 
		&BufPhyAddr, &BufferMapping);
    //
    // Update buffer pointers
    //
    EHCISetQTDBufferPointers(QtdIntData, BufPhyAddr, (UINT32)Length);

    //
    // Update next & alternate next qTD pointers
    //
    QtdIntData->dNextqTDPtr = EHCI_TERMINATE;
    QtdIntData->dAltNextqTDPtr = EHCI_TERMINATE;

    //
    // Schedule the QHInterrupt to 1msec schedule
    //
    EhciAddPeriodicQh(HcStruc, QhInt);

	// Set the QH as active
	QhInt->bActive = TRUE;

    //
    // Now wait for interrupt transaction to be complete;
    // the EHCIProcessInterrupt will set its active flag to FALSE.
    //
	Timeout = gUsbData->wTimeOutValue * 100; // makes it number of 10 microsecond units

	for (Count = 0; Timeout == 0 || Count < Timeout; Count++) {
		if (!(QtdIntData->dToken & QTD_ACTIVE)) {
			break;
		}
		FixedDelay(10);  // 60 microsec
	}
    //Status = EHCIWaitForTransferComplete(HcStruc, fpQHInt, fpDevInfo);

	// Remove the QH from periodic schedule
    EhciRemovePeriodicQh(HcStruc,(EHCI_QH*)QhInt);

    //
    // Check whether the QH stopped or timed out
    //
    BytesTransferred = 0;
	if (QtdIntData->dToken & QTD_ACTIVE) {
	    USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "EHCI Time-Out\n");
		gUsbData->dLastCommandStatusExtended |= USB_TRNSFR_TIMEOUT;
	} else if (QtdIntData->dToken & QTD_HALTED) {
		gUsbData->dLastCommandStatusExtended  |= USB_TRSFR_STALLED;
	} else {
		BytesTransferred = (UINT16)(Length - ((QtdIntData->dToken &
								~(QTD_DATA_TOGGLE)) >> 16));
		//
		// Update the data toggle value into the mass info structure
		//
		DatToggle = (UINT8)(((QhInt->dToken & QH_DATA_TOGGLE) >> 31) & 1);
		UsbUpdateDataToggle(DevInfo, EndpointAddress, DatToggle);
	}

	HcDmaUnmap(HcStruc, BufferMapping);

	// Free the allocated QH and qTD
    EhciMemFree(HcStruc, QhInt, GET_MEM_BLK_COUNT((1 * sizeof(EHCI_QH)) +
                                            	  (1 * sizeof(EHCI_QTD))));

	// Service all interrupts
	EHCI_ProcessInterrupt(HcStruc);

    return (UINT16)BytesTransferred;
}


/**
    This function de-activates the polling QH for the requested
    device. The device may be a USB keyboard or USB hub

    @param HcStruc   - Pointer to the HC structure
        fpDevInfo   - Pointer to the device information structure

    @retval USB_ERROR on error, USB_SUCCESS on success

**/

UINT8
EHCI_DeactivatePolling(
    HC_STRUC*       HcStruc,
    DEV_INFO*       DevInfo
)
{
    UINT8   *PollEd;
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

	if (EhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}

	// Deactivate iTDs
	if ((DevInfo->IsocDetails.Endpoint) && (HcStruc->IsocTds != NULL)) {
	    ActivateDeactivateITDs(HcStruc, 0, HcStruc->wAsyncListSize * 8, NULL, FALSE);
	}

    //
    // Get a pointer to the device's QH from the poll QH pointer and remove
    // the polling ED from the schedule
    //
    PollEd = DevInfo->fpPollEDPtr;
    if (!PollEd) {
        return USB_ERROR;
    }

	((EHCI_QH*)PollEd)->bActive = FALSE;

    EhciRemovePeriodicQh(HcStruc,(EHCI_QH*)PollEd); 

	UsbUpdateDataToggle(DevInfo, DevInfo->IntInEndpoint, 
		(UINT8)((((EHCI_QH*)PollEd)->dToken & QH_DATA_TOGGLE) >> 31));

    EhciMemFree(HcStruc, PollEd, GET_MEM_BLK_COUNT(sizeof(EHCI_QH) + sizeof(EHCI_QTD)));
    DevInfo->fpPollEDPtr  = NULL;
	DevInfo->fpPollTDPtr	= NULL;

	if (DevInfo->fpPollDataBuffer) {
		EhciMemFree(HcStruc, DevInfo->fpPollDataBuffer, 
						GET_MEM_BLK_COUNT(DevInfo->PollingLength)); 
    	DevInfo->fpPollDataBuffer = 0;
	}

    return USB_SUCCESS;
}


/**
    This function activates the polling QH for the requested
    device. The device may be a USB keyboard or USB hub

    @param HcStruc   - Pointer to the HC structure
        fpDevInfo   - Pointer to the device information structure

    @retval USB_ERROR on error, USB_SUCCESS on success

    @note  For the keyboard device this routine allocates TDRepeat
              also, if it is not already allocated. This routine allocate
              a polling TD and schedule it to 8ms schedule for keyboards
              and to 1024ms schedule for hubs.

**/

UINT8
EHCI_ActivatePolling(
    HC_STRUC* HcStruc,
    DEV_INFO* DevInfo
)
{
    EHCI_QH     *PollEd; 
    EHCI_QTD    *PollTd;
    UINT32      Tmp;
    UINT32      Tmp1;
    EFI_STATUS  EfiStatus;
    UINT8       *PolllEdPhyAddr;
    UINT8       *PollTdPhyAddr;
    UINT8       *PollDataBufferPhyAddr;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "EHCI_AP dev type %d, adr %d, ep %x, maxp %x, speed %x, interval %d\n",
        DevInfo->bDeviceType, DevInfo->bDeviceAddress, DevInfo->IntInEndpoint, DevInfo->IntInMaxPkt, DevInfo->bEndpointSpeed, DevInfo->bPollInterval);

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
    
	if (EhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}

    //
    // Allocate a QH/qTD for polling QH & qTD
    //
	PollEd = EhciMemAlloc(HcStruc, GET_MEM_BLK_COUNT(
                                  sizeof(EHCI_QH)+sizeof(EHCI_QTD)));
    if (!PollEd)
    {
        return  USB_ERROR;  // Memory allocation error
    }

    //
    // Save the pointers in DeviceInfo structure
    //
    DevInfo->fpPollEDPtr  = (UINT8*)PollEd;
    PollTd    = (EHCI_QTD*)((UINTN)PollEd + sizeof(EHCI_QH));
    DevInfo->fpPollTDPtr  = (UINT8*)PollTd;

    PolllEdPhyAddr = EhciMemGetPhyAddr(HcStruc, PollEd);
    PollTdPhyAddr = EhciMemGetPhyAddr(HcStruc, PollTd);

//
// Setup the polling QH
// Set the QH's dNextqTDPtr field to polling qTD and dAltqTDPtr field to
// EHCI_TERMINATE
//

    PollEd->fpFirstqTD    = PollTd;
    PollEd->dNextqTDPtr   = (UINT32)(UINTN)PollTdPhyAddr;

    //
    // Intialize the queue head
    //
    PollEd->dAltNextqTDPtr    = EHCI_TERMINATE;
    PollEd->dLinkPointer      = EHCI_TERMINATE;

    //
    // Set the device info pointer in the QH
    //
    PollEd->fpDevInfoPtr  = (UINT8*)DevInfo;

    //
    // Get Device address & Endpoint
    //
    Tmp = ((UINT32)DevInfo->bDeviceAddress)  |
                    ((UINT32)(DevInfo->IntInEndpoint & 0xF) << 8);

    Tmp |= ((UINT32)DevInfo->IntInMaxPkt) << 16; // Set max packet size	//(EIP54782)

    Tmp1   = (UINT32)DevInfo->bEndpointSpeed;    // 00/01/10 for HI/LO/FULL

    //
    // Assume as a high speed device
    //
    Tmp |= QH_HIGH_SPEED;  // 10b - High speed

    //
    // Check for high speed
    //
    if (Tmp1)
    {
        Tmp1 = (Tmp1 & 1) << 12;  // Bit 12 = full/low speed flag
        Tmp &= ~(QH_ENDPOINT_SPEED);
        Tmp |= Tmp1;
        //
        // Set the hub address and port number
        //
        Tmp1 = (UINT32)EHCI_GetHiSpeedHubPortNumber(DevInfo);
        Tmp1 = (Tmp1 << 16) | (BIT10 + BIT11 + BIT12);    // Split complete Xaction
        PollEd->dEndPntCap    |= Tmp1;
    }

    //
    // Update the endpoint characteristcs field with the data formed
    //
    PollEd->dEndPntCharac = Tmp;

    //
    // Set a bit in interrupt mask
    //
    PollEd->dEndPntCap    |= (BIT0 + QH_ONE_XFER);
    PollEd->Interval = EhciTranslateInterval(DevInfo->bEndpointSpeed, 
                                DevInfo->bPollInterval);

    //
    // Set the data toggle
    //
    PollEd->dToken |= (UINT32)(UsbGetDataToggle(DevInfo, DevInfo->IntInEndpoint) << 31);

//
// Fill the polling qTD with relevant information
// The token field will be set so
//   Direction PID = QTD_IN_TOKEN,
//   Size = size of the data,
//   Data Toggle = QTD_DATA0_TOGGLE,
//   Error Count = QTD_THREE_ERRORS,
//   Status code = QTD_ACTIVE
// The buffer pointers field will point to the fpBuffer buffer
//   which was before initialized to contain a DeviceRequest struc.
// The dNextqTDPtr field will point to the qTDControlSetup
// The dAltNextqTDPtr field will be set to EHCI_TERMINATE
//
    PollTd->dToken = QTD_IN_TOKEN |
                    QTD_IOC_BIT |
                    QTD_THREE_ERRORS |
                    QTD_ACTIVE;
    //
    // Set length
    //
    PollTd->dToken |= (UINT32)DevInfo->PollingLength << 16;
    DevInfo->fpPollDataBuffer = EhciMemAlloc(HcStruc, 
									GET_MEM_BLK_COUNT(DevInfo->PollingLength)); 
	ASSERT(DevInfo->fpPollDataBuffer);

    PollDataBufferPhyAddr = EhciMemGetPhyAddr(HcStruc, DevInfo->fpPollDataBuffer);

    //
    // Update buffer pointers
    //
    EHCISetQTDBufferPointers(PollTd,
                PollDataBufferPhyAddr, DevInfo->PollingLength);

    //
    // Update next & alternate next qTD pointers
    //
    PollTd->dNextqTDPtr = EHCI_TERMINATE;
    PollTd->dAltNextqTDPtr = EHCI_TERMINATE;

    EhciAddPeriodicQh(HcStruc,PollEd); 

    PollEd->bCallBackIndex = USB_InstallCallBackFunction(EhciPollingTDCallback);
    PollEd->bActive = TRUE;

    return  USB_SUCCESS;
}


/**
    This function disables the keyboard repeat rate logic by
    enabling the repeat TD

    @param HcStruc   - Pointer to the HCStruc structure

    @retval VOID

**/

UINT8
EHCI_DisableKeyRepeat (
	HC_STRUC	*HcStruc
)
{
    EHCI_DESC_PTRS  *DescPtr;
	EHCI_QH		    *RepeatQh;
    EFI_STATUS      EfiStatus;
    UINT8           *MemBlockEnd = gUsbData->fpMemBlockStart + (gUsbData->MemPages << 12);

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }
    
	if (EhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}

    DescPtr = HcStruc->stDescPtrs.fpEHCIDescPtrs;

    if (DescPtr == NULL) {
        return USB_ERROR;
    }

#if USB_RUNTIME_DRIVER_IN_SMM
    if (((UINT8*)DescPtr < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(DescPtr + sizeof(EHCI_DESC_PTRS)) > MemBlockEnd)) {
        return USB_ERROR;
    }
#endif
    
    RepeatQh = DescPtr->fpQHRepeat;

    if (RepeatQh == NULL) {
        return USB_ERROR;
    }
    
#if USB_RUNTIME_DRIVER_IN_SMM
    if (((UINT8*)RepeatQh < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(RepeatQh + sizeof(EHCI_QH)) > MemBlockEnd)) {
        return USB_ERROR;
    }
#endif

    RepeatQh->dTokenReload = ((UINT32)8 << 16) | QTD_IN_TOKEN | QTD_ONE_ERROR;
    RepeatQh->bActive = FALSE;

    return  USB_SUCCESS;
}

/**
    This function disables the keyboard repeat rate logic by
    enabling the repeat TD

    @param HcStruc   Pointer to the HCStruc structure

    @retval USB_SUCCESS or USB_ERROR

**/

UINT8
EHCI_EnableKeyRepeat (
	HC_STRUC* HcStruc
)
{
    EHCI_DESC_PTRS      *DescPtr;
	EHCI_QH		        *RepeatQh;
	EHCI_QTD            *RepeatQtd;
    EFI_STATUS          EfiStatus;
    UINT8               *MemBlockEnd = gUsbData->fpMemBlockStart + (gUsbData->MemPages << 12);

    EfiStatus = UsbHcStrucValidation(HcStruc);
    
    if (EFI_ERROR(EfiStatus)) {
        return USB_ERROR;
    }

    if (!(HcStruc->dHCFlag & HC_STATE_RUNNING)) {
        return USB_ERROR;
    }
    
	if (EhciIsHalted(HcStruc)) {
		return USB_ERROR;
	}

    DescPtr = HcStruc->stDescPtrs.fpEHCIDescPtrs;

    if (DescPtr == NULL) {
        return USB_ERROR;
    }

#if USB_RUNTIME_DRIVER_IN_SMM
    if (((UINT8*)DescPtr < gUsbData->fpMemBlockStart) ||
        ((UINT8*)(DescPtr + sizeof(EHCI_DESC_PTRS)) > MemBlockEnd)) {
        return USB_ERROR;
    }
#endif

    RepeatQh = DescPtr->fpQHRepeat;

	if (RepeatQh == NULL) {
		return USB_ERROR;
	}
    
#if USB_RUNTIME_DRIVER_IN_SMM
    if (((UINT8*)RepeatQh < gUsbData->fpMemBlockStart) ||
        (((UINT8*)RepeatQh + sizeof(EHCI_QH)) > MemBlockEnd)) {
        return USB_ERROR;
    }
#endif

	if ((RepeatQh->dTokenReload & QTD_ACTIVE) == 0) {
		RepeatQtd = DescPtr->fpqTDRepeat;
        if (RepeatQtd == NULL) {
            return USB_ERROR;
        }
#if USB_RUNTIME_DRIVER_IN_SMM
        if (((UINT8*)RepeatQtd < gUsbData->fpMemBlockStart) ||
            (((UINT8*)RepeatQtd + sizeof(EHCI_QTD)) > MemBlockEnd)) {
            return USB_ERROR;
        }
#endif
		RepeatQh->dTokenReload = ((UINT32)8 << 16) | QTD_IN_TOKEN | 
								QTD_ONE_ERROR | QTD_IOC_BIT | QTD_ACTIVE;

		// Update buffer pointers
		EHCISetQTDBufferPointers(RepeatQtd,
						&RepeatQh->aDataBuffer[0], 8);
		
		// Re-init the QH pointers
		RepeatQh->dCurqTDPtr = 0;
		RepeatQh->dAltNextqTDPtr = EHCI_TERMINATE;
		RepeatQh->dNextqTDPtr = (UINT32)(UINTN)RepeatQtd;
		
		//
		// Restart the qTD
		//
		RepeatQh->dToken = 0;
		RepeatQtd->dToken = RepeatQh->dTokenReload;

		RepeatQh->bActive = TRUE;
	}
    return USB_SUCCESS;
}


/**
    This function resets the EHCI controller

    @param Pointer to the HCStruc structure

    @retval USB_SUCCESS     HC successfully reset
        USB_ERROR       Error, error log is updated

**/

UINT8
EHCIResetHC(
    HC_STRUC* HcStruc
)
{
    UINT16   Count;

    if (HcStruc->DebugPort) {
        return USB_SUCCESS;
    }
    //
    // Check HC is halted: attempting to reset an actively running HC will
    // result in undefined behavior.
    //
    if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_HCHALTED) {
        //
        // Issue reset
        //
        HcSetOpReg(HcStruc, EHCI_USBCMD, EHCI_HCRESET);

        //
        // EHCI_HCRESET bit is set to zero by the Host Controller when the reset
        // process is complete.
        //
        for (Count = 0; Count < 500; Count++) {
            if (!(HcReadOpReg(HcStruc, EHCI_USBCMD) & EHCI_HCRESET)) {
                return  USB_SUCCESS;
            }
            FixedDelay(100);     // 100 us delay
        }
    }

    //
    // Error - HC reset failed
    //
    USBLogError(USB_ERR_HC_RESET_FAILED);

    return  USB_ERROR;
}

/*
void printlist(HC_STRUC *HcStruc)
{
    UINT16      Index;
    UINT32      *PrevPtr;
    UINT32      LinkPtr;
    EHCI_QH     *Qh;

    for (Index = 0; Index < HcStruc->wAsyncListSize; Index++) {
        PrevPtr = &HcStruc->fpFrameList[Index];
        LinkPtr = *PrevPtr;

        USB_DEBUG(DEBUG_INFO, 3, "%04d: %x->", Index, PrevPtr);

        while (!(LinkPtr & EHCI_TERMINATE)){
            USB_DEBUG(DEBUG_INFO, 3, "%x->", LinkPtr);
            Qh = (EHCI_QH*)(LinkPtr & EHCI_POINTER_MASK);
            PrevPtr = &Qh->dLinkPointer;
            LinkPtr = *PrevPtr; 
        }
        USB_DEBUG(DEBUG_INFO, 3, "%x\n", LinkPtr);
    }
} 
*/

/**
    This function initializes the periodic schedules for the
    EHCI host controller

    @param HcStruc   - HCStruc for the controller
        dMemAddr    - Membase address

    @retval USB_ERROR   On error
        USB_SUCCESS On success

    @note  This routine creates 8ms and 32ms schedules

**/

UINT8
EHCIInitializePeriodicSchedule(
    HC_STRUC*   HcStruc,
    UINT32      MemBase
)
{
    UINT8           *Ptr;
    EHCI_DESC_PTRS  *DescPtr;
    //
    // Allocate descriptor structure and fill it in HCStruc
    //
    DescPtr = EhciMemAlloc(HcStruc, GET_MEM_BLK_COUNT_STRUC(EHCI_DESC_PTRS));
    if (DescPtr == NULL) {
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_4, "EHCI Descriptor struc alloc failed. %d \n",
                                    GET_MEM_BLK_COUNT_STRUC(EHCI_DESC_PTRS));
        return USB_ERROR;
    }

    //
    // Save the value in the HC struc
    //
    HcStruc->stDescPtrs.fpEHCIDescPtrs = DescPtr;

    EhciAddIsochTDs(HcStruc);    // This must be called before any other periodic scheduling

    // Allocate QH/qTD for PeriodicQh
    Ptr = EhciMemAlloc(HcStruc, GET_MEM_BLK_COUNT((1 * sizeof(EHCI_QH))+
													(0 * sizeof(EHCI_QTD))));
    if (Ptr == NULL) {
        USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_4, "Schedule of EHCI QH alloc failed.\n");
        return USB_ERROR;
    }
	
    // Save the 1 QH in appropriate location
    DescPtr->PeriodicQh = (EHCI_QH*)Ptr;

    EHCIInitializeQueueHead(DescPtr->PeriodicQh);
    DescPtr->PeriodicQh->dNextqTDPtr = EHCI_TERMINATE;
    DescPtr->PeriodicQh->dAltNextqTDPtr = EHCI_TERMINATE;
    DescPtr->PeriodicQh->dEndPntCharac = ((UINT32)0x40 << 16) + QH_HIGH_SPEED;
    DescPtr->PeriodicQh->dLinkPointer      = EHCI_TERMINATE;
    DescPtr->PeriodicQh->dToken = QTD_HALTED;
    DescPtr->PeriodicQh->Interval = 1;

    EhciAddPeriodicQh(HcStruc, DescPtr->PeriodicQh);

    return  USB_SUCCESS;
}



/**
    This function initializes the queue head with default values

    @param fpQH    Pointer to queue head

    @retval VOID

**/

VOID
EHCIInitializeQueueHead(
    EHCI_QH *Qh
)
{
    Qh->dNextqTDPtr       = 1;
    Qh->dAltNextqTDPtr    = 1;
    Qh->dCurqTDPtr        = 0;

    Qh->dEndPntCap        = QH_ONE_XFER;
    Qh->dToken            = 0;
    Qh->dEndPntCharac     = 0;
    Qh->dBufferPtr0       = 0;
    Qh->dBufferPtr1       = 0;
    Qh->dBufferPtr2       = 0;
    Qh->dBufferPtr3       = 0;
    Qh->dBufferPtr4       = 0;
    Qh->bErrorStatus      = 0;
}


/**
    This function starts the periodic schedule for the
    EHCI host controller

    @param Pointer to HcStruc

    @retval USB_ERROR   On error
            USB_SUCCESS On success

**/

UINT8
EHCIStartPeriodicSchedule(
    HC_STRUC* HcStruc
)
{
    UINT16  i;
    //
    // Start periodic schedule
    //
    HcSetOpReg(HcStruc, EHCI_USBCMD, EHCI_PER_SCHED_ENABLE);

    //
    // Make sure the HC started the schedules
    //
    for (i = 0; i < 1000; i++) {
        if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_PER_SCHED_STATUS) {
            break;
        }
        FixedDelay(10);
    }
    if (!(HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_PER_SCHED_STATUS)) {
        return USB_ERROR;
    }
    return USB_SUCCESS;
}


/**
    This function stops the periodic schedule for the
    EHCI USB host controller

    @param HcStruc for the controller

    @retval USB_ERROR   On error
            USB_SUCCESS On success

**/

UINT8
EHCIStopPeriodicSchedule(
    HC_STRUC* HcStruc
)
{
    UINT16  i;
    //
    // Stop periodic schedule
    //
    HcClearOpReg(HcStruc, EHCI_USBCMD, EHCI_PER_SCHED_ENABLE);

    //
    // Make sure the HC stopped the schedules
    //
    for (i = 0; i < 1000; i++) {
        if (!(HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_PER_SCHED_STATUS)) {
            break;
        }
        FixedDelay(10);
    }
    if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_PER_SCHED_STATUS) {
        return USB_ERROR;
    }
    return USB_SUCCESS;
}


/**
    This function starts the asynchronous schedule

    @param Pointer to HcStruc

    @retval USB_ERROR   On error
            USB_SUCCESS On success

**/

UINT8
EHCIStartAsyncSchedule(
    HC_STRUC* HcStruc
)
{
    UINT16  i;
    //
    // Start the Async schedule
    //
    HcSetOpReg(HcStruc, EHCI_USBCMD, EHCI_ASYNC_SCHED_ENABLE);

    //
    // Make sure the HC started the async. execution
    //
    for (i = 0; i < 1000; i++) {
        if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_ASYNC_SCHED_STATUS) {
            break;
        }
        FixedDelay(10);
    }
    if (!(HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_ASYNC_SCHED_STATUS)) {
        //
        // Stop the host controller (Reset bit 0)
        //
        HcClearOpReg(HcStruc, EHCI_USBCMD, EHCI_RUNSTOP);
        // The Host Controller must halt within 16 micro-frames after 
        // software clears the Run bit. 
        for (i = 0; i < 16; i++) {
            if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_HCHALTED) {
                break;
            }
            FixedDelay(125);      // 125 us delay
        }
        return USB_ERROR;
    }
    
    return USB_SUCCESS;
}


/**
    This function stops the asynchronous transfer and sets the
    asynchronous pointer to null

    @param Pointer to HcStruc

    @retval USB_ERROR   On error
            USB_SUCCESS On success

**/

UINT8
EHCIStopAsyncSchedule(
    HC_STRUC* HcStruc
)
{
    UINT16  i;
    
	if (!(HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_ASYNC_SCHED_STATUS)) {
	    return USB_SUCCESS;
	}

    //
    // Stop periodic schedule
    //
    HcClearOpReg(HcStruc, EHCI_USBCMD, EHCI_ASYNC_SCHED_ENABLE);

    //
    // Make sure the HC stopped the async. execution
    //
    for (i = 0; i < 1000; i++) {
        if (!(HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_ASYNC_SCHED_STATUS)) {
            break;
        }
        FixedDelay(10);
    }
    if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_ASYNC_SCHED_STATUS) {
        //
        // Stop the host controller (Reset bit 0)
        //
        HcClearOpReg(HcStruc, EHCI_USBCMD, EHCI_RUNSTOP);
        // The Host Controller must halt within 16 micro-frames after 
        // software clears the Run bit. 
        for (i = 0; i < 16; i++) {
            if (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_HCHALTED) {
                break;
            }
            FixedDelay(125);      // 125 us delay
        }
        return USB_ERROR;
    }
    
    return USB_SUCCESS;
}


//#if EHCI_ASYNC_BELL_SUPPORT
/**
    This function stops the asynchronous transfer and sets the
    asynchronous pointer to null

    @param Pointer to HCStruc
        Pointer to the Queue head that has to be removed
        from the asynchronous schedule

    @retval VOID

**/
/*
VOID
EHCIRemoveQHFromAsyncList(
    HC_STRUC    *HcStruc,
    EHCI_QH     *Qh
)
{
    //
    // Stop the Async transfer
    //
    EHCIStopAsyncSchedule(HcStruc);

    //
    // Remove the queue head from the Async list
    //
    gUsbData->fpQHAsyncXfer->dLinkPointer = Qh->dLinkPointer;

    //
    // Pointer is advanced. The queue head is totally removed from the list!
    //
}
*/
//#endif


/**
    This function adds Isochronous TDs to the frame list

    @param Host Controller data structure

    @retval VOID

**/
VOID EhciAddIsochTDs(
    HC_STRUC *HcStruc
)
{
    UINT16  Index;
    UINT8   *Ptr;
    EHCI_ITD *Itd;

    if (HcStruc->IsocTds == NULL) {
        return;
    }

    Itd = (EHCI_ITD*)HcStruc->IsocTds;

    USB_DEBUG(DEBUG_INFO, 3, "EHCI add ISOC TDs: ITD %x\n", Itd);

    for (Index = 0; Index < HcStruc->wAsyncListSize; Index++)
    {
        Ptr = (UINT8*)&HcStruc->fpFrameList[Index];
        Itd->NextLinkPointer = *((UINT32*)(UINTN)Ptr);
        *((UINT32*)(UINTN)Ptr) = (UINT32)(UINTN)Itd;
        Itd++;
    }
}

/**
    This function adds a QH to the frame list

    @param Pointer to the QH to be added
        Absolute pointer to the frame list

    @retval VOID

**/

UINT8
EhciAddPeriodicQh(
    HC_STRUC    *HcStruc,
    EHCI_QH     *NewQh
)
{
    UINT16      Index;
    UINT32      *PrevPtr;
    UINT32      LinkPtr;
    EHCI_QH     *Qh;
    EHCI_QH     *NewQhPhyAddr;
    EHCI_QH     *QhHostAddr;
    EFI_STATUS  Status = EFI_SUCCESS;

    if (NewQh == NULL || NewQh->Interval == 0) {
        return USB_ERROR;
    }

    NewQhPhyAddr = EhciMemGetPhyAddr(HcStruc, NewQh);
 
    for (Index = HcStruc->SplitPeriodicIndex; Index < HcStruc->wAsyncListSize; Index += NewQh->Interval) {
        PrevPtr = &HcStruc->fpFrameList[Index];
        if (HcStruc->IsocTds != NULL) {
            PrevPtr = (UINT32*)(UINTN)*PrevPtr; // leave ITD on the top of the list
        }
        LinkPtr = *PrevPtr; 
        while (!(LinkPtr & EHCI_TERMINATE)){
            Qh = (EHCI_QH*)(LinkPtr & EHCI_POINTER_MASK);
#if !USB_RT_DXE_DRIVER
            Status = AmiValidateMemoryBuffer((VOID*)Qh, sizeof(EHCI_QH));
            if (EFI_ERROR(Status)) {
                return USB_ERROR;
            }
#endif
            QhHostAddr = EhciMemGetHostAddr(HcStruc, Qh);
            if (QhHostAddr->Interval <= NewQh->Interval) {
                break;
            }
            PrevPtr = &QhHostAddr->dLinkPointer;
            LinkPtr = *PrevPtr; 
        }

        if (!(LinkPtr & EHCI_TERMINATE) && (QhHostAddr == NewQh)) {
            continue;
        }
#if !USB_RT_DXE_DRIVER
        Status = AmiValidateMemoryBuffer((VOID*)PrevPtr, sizeof(UINT32));
        if (EFI_ERROR(Status)) {
            return USB_ERROR;
        }
#endif
        NewQh->dLinkPointer = *PrevPtr;
        *PrevPtr = (UINT32)((UINTN)NewQhPhyAddr | EHCI_QUEUE_HEAD);
    }

    if (HcStruc->SplitPeriodicIndex == MAX_SPLIT_PERIODIC_NUMBER) {
        HcStruc->SplitPeriodicIndex = 0;
    } else {
        HcStruc->SplitPeriodicIndex++;
    }

    return USB_SUCCESS;
} 


/**
    This function removes a QH from the frame list

    @param Pointer to the QH to be added
        Absolute pointer to the frame list

    @retval VOID

**/

UINT8
EhciRemovePeriodicQh (
    HC_STRUC    *HcStruc,
    EHCI_QH     *RetiredQh
)
{
    UINT16      Index;
    UINT32      *PrevPtr;
    UINT32      LinkPtr;
    EHCI_QH     *Qh;
    EFI_STATUS  Status = EFI_SUCCESS;
    EHCI_QH     *QhHostAddr;

    if (RetiredQh == NULL || RetiredQh->Interval == 0) {
        return USB_ERROR;
    }

    EHCIStopPeriodicSchedule(HcStruc);

    for (Index = 0; Index < HcStruc->wAsyncListSize; Index++) {
        PrevPtr = &HcStruc->fpFrameList[Index];
        LinkPtr = *PrevPtr; 
 
        while (!(LinkPtr & EHCI_TERMINATE)){
            Qh = (EHCI_QH*)(LinkPtr & EHCI_POINTER_MASK);
#if !USB_RT_DXE_DRIVER
            Status = AmiValidateMemoryBuffer((VOID*)Qh, sizeof(EHCI_QH));
            if (EFI_ERROR(Status)) {
                return USB_ERROR;
            }
#endif
            QhHostAddr = EhciMemGetHostAddr(HcStruc, Qh);
            if (QhHostAddr == RetiredQh) {
                break;
            }
            PrevPtr = &QhHostAddr->dLinkPointer;
            LinkPtr = *PrevPtr; 
        }

        if (LinkPtr & EHCI_TERMINATE) {
            continue;
        }
#if !USB_RT_DXE_DRIVER
        Status = AmiValidateMemoryBuffer((VOID*)PrevPtr, sizeof(UINT32));
        if (EFI_ERROR(Status)) {
            return USB_ERROR;
        }
#endif
        *PrevPtr = RetiredQh->dLinkPointer;
    }

    RetiredQh->dLinkPointer = EHCI_TERMINATE;

    EHCIStartPeriodicSchedule(HcStruc);

    return USB_SUCCESS;
}


/**
    This function will set the 5 buffer pointer in the qTD
    appropriately depending upon the input size

    @param fpQtd   - Pointer to the qTD
        fpBuf   - 32bit absolute buffer pointer
        wSize   - Amount of data to be transferred

    @retval VOID

**/

VOID
EHCISetQTDBufferPointers(
    EHCI_QTD    *Qtd,
    UINT8       *Buf,
    UINT32      Size
)
{
    UINT16      BufSize;
    UINT8       *Buffer   = Buf;
    UINT32      *BufferPtr;
    UINT16      W4KRemainder;

    //
    // Fill the buffer pointers with 0s
    //
    Qtd->dBufferPtr0      = 0;
    Qtd->dBufferPtr1      = 0;
    Qtd->dBufferPtr2      = 0;
    Qtd->dBufferPtr3      = 0;
    Qtd->dBufferPtr4      = 0;
    Qtd->dAltNextqTDPtr   = 1;

    //
    // If size to transfer is 0 skip updating pointers
    //
    if (!Size)
    {
        return;
    }

    //
    // Make sure the amount of data to be xferred is 16K or less
    //
    BufSize = (UINT16)((Size > MAX_EHCI_DATA_SIZE) ? MAX_EHCI_DATA_SIZE : Size);

    BufferPtr = &Qtd->dBufferPtr0;

    for (;;)
    {
        *BufferPtr = (UINT32)(UINTN)Buffer;
        //
        // Calculate the number of bytes that can be transferred using current
        // buffer pointer
        //
        W4KRemainder = (UINT16)((((UINT32)((UINTN)Buffer+0x1000)) & 0xFFFFF000) -
                                        (UINT32)(UINTN)Buffer);

        //
        // Check whether all the bytes can be accomadated in the current buffer
        //
        if (W4KRemainder >= BufSize)
        {
            break;  // Yes. Current Buffer is sufficient for the rest of data
        }

        //
        // We have more data to transfer - adjust data and store it in the next pointer
        //
        BufSize = (UINT16)(BufSize - W4KRemainder);   // Amount of data remaining
        Buffer = Buffer + W4KRemainder;     // Adjust buffer (4K bound)
        BufferPtr++;                          // Next buffer pointer
    }
}


/**
    This function waits until the requested QH completes or
    the transaction time-out

    @param HcStruc   - Pointer to HCStruc of the host controller
        fpQH        - Pointer to the QH which has to be completed

    @retval USB_ERROR   On error
        USB_SUCCESS On success

**/

UINT16
EHCIWaitForTransferComplete(
    HC_STRUC    *HcStruc,
    EHCI_QH     *Qh,
    DEV_INFO*   DevInfo

)
{
    UINT32 Count ;
    UINT32 CountLimit = gUsbData->wTimeOutValue * 100; // makes it number of microsecond units
	UINT16 Status = USB_ERROR;

    //
    // Check status change loop iteration
    //
    for(Count = 0; !CountLimit || Count < CountLimit; Count++)
    {
        EHCIProcessQH(HcStruc, Qh);
        if (Qh->bActive == FALSE) {
            Status = USB_SUCCESS;
			break;
        }
        FixedDelay(10);  // 60 microsec
    }

	if(Qh->bActive == TRUE) {
		// Set the QH as in-active
	    Qh->bActive = FALSE;
		Status = USB_ERROR;
    	USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, "EHCI Time-Out\n");
	}

	// Service all interrupts
	EHCI_ProcessInterrupt(HcStruc);
	
    return Status;
}


/**
    This function whether all the TD's in the QH is completed

    @param fpQH    - Pointer to the QH which has to be completed

    @retval USB_ERROR   On error
        USB_SUCCESS On success

**/

UINT8
EHCIProcessQH(
	HC_STRUC	*HcStruc,
	EHCI_QH		*Qh
)
{
    EHCI_QTD *Qtd = Qh->fpFirstqTD;
    EFI_STATUS  Status = EFI_SUCCESS;
    EHCI_QTD    *QtdHostAddr;

    for (;;) {
        if (Qtd == NULL) {
            return USB_ERROR;
        }
#if !USB_RT_DXE_DRIVER
        Status = AmiValidateMemoryBuffer((VOID*)Qtd, sizeof(EHCI_QTD));
        if (EFI_ERROR(Status)) {
            return USB_ERROR;
        }
#endif
        //
        // Check whether the qTD is active, if so. Exit!
        //
        if (Qtd->dToken & QTD_ACTIVE) {
            return  USB_ERROR;
        }

        //
        // Check for halt condition, if halted - exit
        //
        if (Qtd->dToken & QTD_HALTED) {
            //
            // Set the QH halted status
            //
            Qh->bErrorStatus = QTD_HALTED;
            //
            // Set the QH as in-active
            //
            Qh->bActive = FALSE;
			break;
        }
        //
        // qTD is not active and not halted. That is it is completed successfully
        // Check whether this qTD is the last one in the list
        //
        if (Qtd->dNextqTDPtr & EHCI_TERMINATE) {
            //
            // Set the QH as in-active
            //
            Qh->bActive = FALSE;
            break;
        }

        //
        // More qTDs are in the list. Process next qTD
        //
        Qtd = (EHCI_QTD*)(UINTN)Qtd->dNextqTDPtr;
        QtdHostAddr = EhciMemGetHostAddr(HcStruc, Qtd);
        Qtd = QtdHostAddr;
    }

	if ((Qh->bCallBackIndex) && (Qh->bCallBackIndex <= MAX_CALLBACK_FUNCTION)) {
		if (gUsbData->aCallBackFunctionTable[Qh->bCallBackIndex - 1 + CALLBACK_FUNCTION_START]) {
            if ((gUsbData->aCallBackFunctionTable[Qh->bCallBackIndex - 1 + CALLBACK_FUNCTION_START]) 
                != EhciRepeatTDCallback) {
                Status = UsbDevInfoValidation((DEV_INFO*)Qh->fpDevInfoPtr);

                if (EFI_ERROR(Status)) {
                    return USB_ERROR;
                }
            }
			(*gUsbData->aCallBackFunctionTable[Qh->bCallBackIndex - 1 + CALLBACK_FUNCTION_START])
							(HcStruc, (DEV_INFO*)Qh->fpDevInfoPtr, 
							(UINT8*)Qh, NULL, 0);
		}
	}

	return  USB_SUCCESS;
}

UINT8 EHCI_FindIOCuFrame(EHCI_ITD *Itd)
{
    UINT8 i;

    for (i = 0; i < 8; i++)
    {
        if (Itd->ControlStatus[i] & BIT15)
        {
            break;
        }
    }
    return i;
}

VOID EHCIProcessCurrentItd(
    HC_STRUC *HcStruc
)
{
    UINT32      LinkPtr;
    UINT8       i;
    UINT32      j;
    UINT16      CurrentFIndex;
    EHCI_ITD    *Itd;
    UINT16      FrameCount;
    UINT32      TotalAmount;
    DEV_INFO    *DevInfo;
    EFI_STATUS  Status = EFI_SUCCESS;

    // Check current and previous iTD for IOC
    CurrentFIndex = GetCurrentFIndex(HcStruc);
    LinkPtr = HcStruc->fpFrameList[CurrentFIndex];
    Itd = (EHCI_ITD*)(LinkPtr & EHCI_POINTER_MASK);
    i = EHCI_FindIOCuFrame(Itd);
    if (i == 8)
    {
        if (CurrentFIndex == 0) CurrentFIndex = HcStruc->wAsyncListSize;
        CurrentFIndex--;
        LinkPtr = HcStruc->fpFrameList[CurrentFIndex];
        Itd = (EHCI_ITD*)(LinkPtr & EHCI_POINTER_MASK);
        i = EHCI_FindIOCuFrame(Itd);
        if (i == 8) return; // iTD was not the reason we are here
    }

#if !USB_RT_DXE_DRIVER
    Status = AmiValidateMemoryBuffer((VOID*)Itd, sizeof(EHCI_ITD));
    if (EFI_ERROR(Status)) {
        return;
    }
#endif
    
    // Locate the device by matching iTD with DevInfo->IsocDetails.XferKey
    for (j = 1; j < MAX_DEVICES; j++) {
        DevInfo = &gUsbData->aDevInfoTable[j];
        if ((DevInfo->Flag & DEV_INFO_VALIDPRESENT) != DEV_INFO_VALIDPRESENT) {
            continue;
        }
        if (DevInfo->IsocDetails.XferKey == (UINTN)Itd) break;
    }
    //ASSERT(j < MAX_DEVICES);
    if (j == MAX_DEVICES) return;   // device not found
    
    Itd->ControlStatus[i] &= ~BIT15;    // Clear IOC
    // Count uFrames
    if (CurrentFIndex < (UINT16)DevInfo->IsocDetails.XferStart)
        CurrentFIndex += HcStruc->wAsyncListSize;
    ASSERT(CurrentFIndex >= (UINT16)DevInfo->IsocDetails.XferStart);
    FrameCount = ((CurrentFIndex - (UINT16)DevInfo->IsocDetails.XferStart) << 3) + i + 1; // i is 0-based
#if !USB_RT_DXE_DRIVER
    Status = AmiValidateMemoryBuffer((VOID*)DevInfo->IsocDetails.XferDetails, FrameCount * sizeof(UINT32));
    if (EFI_ERROR(Status)) {

        return ;
    }
#endif    
    TotalAmount = FinalizeIsocXfer(HcStruc, (UINT16)DevInfo->IsocDetails.XferStart,
            FrameCount, DevInfo->IsocDetails.XferDetails);
    //USB_DEBUG(DEBUG_INFO, 3, "EHCI ISOC: frame #%d, %d uFrames, details ptr: %x, total %d Bytes\n",
    //  DevInfo->IsocDetails.XferStart, uFrameCount, DevInfo->IsocDetails.XferDetails, TotalAmount);

    // Indicate the completion of the transfer
#if !USB_RT_DXE_DRIVER
    Status = AmiValidateMemoryBuffer((VOID*)DevInfo->IsocDetails.AsyncStatus, sizeof(UINT8));
    if (EFI_ERROR(Status)) {
        return;
    }
#endif
    *DevInfo->IsocDetails.AsyncStatus = 1;
}

/**
    This function parses through the periodic list to find
    completed qTD.  If a qTD is done it will call its associated
    call back function (in device info structure) and restarts
    the polling qTD

    @param HcStruc   - Pointer to HCStruc of the host controller

    @retval VOID

**/

VOID
EHCIProcessPeriodicList(
	HC_STRUC	*HcStruc
)
{
    UINT16      Index;
    UINT32      *PrevPtr;
    UINT32      LinkPtr;
    EHCI_QH     *Qh;
    UINT32      OrgQhLinkPointer;
    EHCI_QH     *QhHostAddr;
    EFI_STATUS  Status = EFI_SUCCESS;


    if (HcStruc->IsocTds != NULL) {
        EHCIProcessCurrentItd(HcStruc);
    }
    

    //
    // Get the first entry in the periodic list. This QH list will link to all
    // the periodic QH's
    //

    for (Index = 0; Index <= MAX_SPLIT_PERIODIC_NUMBER; Index ++) {
        PrevPtr = &HcStruc->fpFrameList[Index];
        LinkPtr = *PrevPtr; 

	    while (!(LinkPtr & EHCI_TERMINATE)) {
		    Qh = (EHCI_QH*)(LinkPtr & EHCI_POINTER_MASK);

#if !USB_RT_DXE_DRIVER
            Status = AmiValidateMemoryBuffer((VOID*)Qh, sizeof(EHCI_QH));
            if (EFI_ERROR(Status)) {
                return;
            }
#endif
            QhHostAddr = EhciMemGetHostAddr(HcStruc, Qh);

	        // Process only QHeads, skip the other types
	        if ((QhHostAddr->dLinkPointer & 6) == EHCI_QUEUE_HEAD) {

	            // Check whether this QH is actived
                if (QhHostAddr->bActive == TRUE) {
                    OrgQhLinkPointer = QhHostAddr->dLinkPointer;
                    EHCIProcessQH(HcStruc, QhHostAddr);
                    // Qh is changed, we re-parses through the list.
                    if (QhHostAddr->dLinkPointer != OrgQhLinkPointer) {
                        LinkPtr = *PrevPtr;
                        continue;
                    }
                }
	        }

		    LinkPtr = QhHostAddr->dLinkPointer;
	    }

    }
/*
	UINT32		LinkPtr;
    EHCI_QH     *Qh = NULL;

    EHCIProcessCurrentItd(HcStruc);

    //
    // Get the first entry in the periodic list. This QH list will link to all
    // the periodic QH's
    //

	LinkPtr = HcStruc->fpFrameList[0];
	while (!(LinkPtr & EHCI_TERMINATE)) {
		Qh = (EHCI_QH*)(LinkPtr & EHCI_POINTER_MASK);

		// Process only QHeads, skip the other types
		if ((Qh->dLinkPointer & 6) == EHCI_QUEUE_HEAD) {
    
            // Check whether this QH is actived
            if (Qh->bActive == TRUE) {			
                EHCIProcessQH(HcStruc, Qh);
            }
		}
		
		LinkPtr = Qh->dLinkPointer;
	}
*/
}

/**
    This function is called when a polling TD from the TD pool 
    completes an interrupt transaction to its assigned device.
    This routine should process any data in the TD's data buffer, 
    handle any errors, and then copy the TD's CSReloadValue field 
    into its control status field to put the TD back into service.

    @param HcStruc   Pointer to the HCStruc structure
        DevInfo   NULL (pDevInfo is not valid)
        QueueHead Pointer to the QH that completed
        Buffer    Not used

    @retval VOID

**/

UINT8
EhciPollingTDCallback(
    HC_STRUC	*HcStruc,
    DEV_INFO	*DevInfo,
    UINT8		*QueueHead,
    UINT8		*Buffer,
    UINT16      DataLength
)
{
	EHCI_QH*	PollQh = (EHCI_QH*)QueueHead;
	EHCI_QTD    *PollQtd = PollQh->fpFirstqTD;
    UINT16      BytesTransferred;
    UINT32      PortStatus;
    UINT8       *PollDataBufferPhyAddr;
    UINT8       *PollQtdPhyAddr;
    EFI_STATUS  Status = EFI_SUCCESS;

#if !USB_RT_DXE_DRIVER
    Status = AmiValidateMemoryBuffer((VOID*)PollQtd, sizeof(EHCI_QTD));
    if (EFI_ERROR(Status)) {
        return USB_ERROR;
    }
#endif

	// Update datat toggle value
	UsbUpdateDataToggle(DevInfo, DevInfo->IntInEndpoint, 
		(UINT8)((PollQh->dToken & QH_DATA_TOGGLE) >> 31));

	if (!(PollQh->dToken & QTD_STATUS_FIELD)) {
        if ((DevInfo->bCallBackIndex) && (DevInfo->bCallBackIndex <= MAX_CALLBACK_FUNCTION)) {
            if (gUsbData->aCallBackFunctionTable[DevInfo->bCallBackIndex - 1 + CALLBACK_FUNCTION_START]) {
                //
                // Get the size of data transferred
                //
                BytesTransferred = DevInfo->PollingLength - (UINT16)((PollQtd->dToken & ~(QTD_DATA_TOGGLE)) >> 16);
                (*gUsbData->aCallBackFunctionTable[DevInfo->bCallBackIndex - 1 + CALLBACK_FUNCTION_START])
                        (HcStruc, DevInfo, (UINT8*)PollQtd, DevInfo->fpPollDataBuffer, BytesTransferred);
            }
		}
	} else {
        if (PollQh->dToken & QTD_XACT_ERROR) {
            // When the device is disconnecting, the transaction may be error, 
            // check if the port status is enabled.
            PortStatus = USB_GetHubPortStatus(HcStruc, DevInfo->bHubDeviceNumber, DevInfo->bHubPortNumber, FALSE);
            if ((PortStatus == USB_ERROR) || (!(PortStatus & USB_PORT_STAT_DEV_ENABLED))) {
                return USB_SUCCESS;
            }
        }
    }

    // Keep the PID code bit in the qTD
    PollQtd->dToken &= QTD_DIRECTION_PID;
    
    //
    // Set length
    //
	PollQtd->dToken |= (UINT32)DevInfo->PollingLength << 16;

    //
    // Update buffer pointers
    //
    PollDataBufferPhyAddr = EhciMemGetPhyAddr(HcStruc, DevInfo->fpPollDataBuffer);
    
	EHCISetQTDBufferPointers(PollQtd,
					PollDataBufferPhyAddr, DevInfo->PollingLength);

    //
    // Re-init the QH pointers
    //
    PollQh->dToken &= QH_DATA_TOGGLE; 
    PollQh->dCurqTDPtr = 0;
    PollQh->dAltNextqTDPtr = EHCI_TERMINATE;
    PollQh->dBufferPtr0 = 0;
    PollQh->dBufferPtr1 = 0;
    PollQh->dBufferPtr2 = 0;
    PollQh->dBufferPtr3 = 0;
    PollQh->dBufferPtr4 = 0;
    //
    // Update next & alternate next qTD pointers
    //
    PollQtd->dNextqTDPtr = EHCI_TERMINATE;
    PollQtd->dAltNextqTDPtr = EHCI_TERMINATE;
    PollQtdPhyAddr = EhciMemGetPhyAddr(HcStruc, PollQtd);
    PollQh->dNextqTDPtr = (UINT32)(UINTN)PollQtdPhyAddr;

    //
    // Restart the qTD
    //
    PollQtd->dToken |= (QTD_IOC_BIT | QTD_THREE_ERRORS | QTD_ACTIVE);
	PollQh->bActive = TRUE;
	
    return  USB_SUCCESS;
}

/**
    This function is called when qTdRepeat completes
    a transaction.  This qTD runs a dummy interrupt transaction
    to a non-existant device address for the purpose of
    generating a periodic timeout interrupt which in turn
    is used to generate keyboard repeat or update LED status.

    @param HcStruc   Pointer to the HCStruc structure
        DevInfo   NULL (pDevInfo is not valid)
        QueueHead Pointer to the QH that completed
        Buffer    Not used

    @retval VOID

**/

UINT8
EhciRepeatTDCallback(
	HC_STRUC	*HcStruc,
	DEV_INFO	*DevInfo,
	UINT8		*QueueHead,
	UINT8		*Buffer,
	UINT16      DataLength
)
{
	EHCI_QH		*RepeatQh = (EHCI_QH*)QueueHead;
	EHCI_QTD	*RepeatQtd = RepeatQh->fpFirstqTD;
    EFI_STATUS  Status = EFI_SUCCESS;

#if !USB_RT_DXE_DRIVER
    Status = AmiValidateMemoryBuffer((VOID*)RepeatQtd, sizeof(EHCI_QTD));
    if (EFI_ERROR(Status)) {
        return USB_ERROR;
    }
#endif

#if USB_DEV_KBD
	USBKBDPeriodicInterruptHandler(HcStruc);
#endif

	if (RepeatQh->dTokenReload & QTD_ACTIVE) {
		//
		// Update buffer pointers
		//
		EHCISetQTDBufferPointers(RepeatQtd,
						&RepeatQh->aDataBuffer[0], 8);

		//
		// Re-init the QH pointers
		//
        RepeatQh->dToken &= QH_DATA_TOGGLE;
        RepeatQh->dCurqTDPtr = 0;
        RepeatQh->dAltNextqTDPtr = EHCI_TERMINATE;
        RepeatQh->dBufferPtr0 = 0;
        RepeatQh->dBufferPtr1 = 0;
        RepeatQh->dBufferPtr2 = 0;
        RepeatQh->dBufferPtr3 = 0;
        RepeatQh->dBufferPtr4 = 0;
		RepeatQh->dNextqTDPtr = (UINT32)(UINTN)RepeatQtd;

	    //
	    // Restart the qTD
	    //
		RepeatQh->dToken = 0;
		RepeatQtd->dToken = RepeatQh->dTokenReload;
		
		RepeatQh->bActive = TRUE;
	}

    return  USB_SUCCESS;
}

/**
    This function check whether HC is halted.

**/

BOOLEAN
EhciIsHalted (
	HC_STRUC	*HcStruc
)
{
	return (HcReadOpReg(HcStruc, EHCI_USBSTS) & EHCI_HCHALTED) == EHCI_HCHALTED;
}

/**
    This function calculates the polling rate in frames unit.

**/

UINT16
EhciTranslateInterval(
    UINT8   Speed,
    UINT8   Interval
)
{
    UINT8   BitCount = 0;
    UINT16  PollingRate = 0;

    if (Speed == USB_PORT_STAT_DEV_HISPEED) {
        // For high-speed interrupt endpoints, the Interval value must be 
        // from 1 to 16
        ASSERT(Interval >= 1 && Interval <= 16);
 
        PollingRate = (1 << (Interval - 1)) >> 3;
        return PollingRate != 0 ? PollingRate : 1;
    }

    // For full-/low-speed interrupt endpoints, the Interval value should 
    // be from 1 to 255
    ASSERT(Interval >= 1 && Interval <= 255);

    for (BitCount = 0; Interval != 0; BitCount++) {
        Interval >>= 1;
    }
    return (1 << (BitCount - 1));
}

UINT32
EhciReadDebugReg(
	HC_STRUC	*HcStruc,
	UINT8		BarIndex,
	UINT32		Offset
)
{
#if !USB_RT_DXE_DRIVER
	UINT8  BarOffset[6] = {0x10, 0x14, 0x18, 0x1C, 0x20, 0x24};
	UINT32 DebugPortsMem;

    DebugPortsMem = HcReadPciReg(HcStruc, BarOffset[BarIndex]);
	return DwordReadMem(DebugPortsMem, Offset);
#else
	EFI_STATUS	Status;
	UINT32		Data = 0;
	EFI_PCI_IO_PROTOCOL	*PciIo = HcStruc->PciIo;
	
	Status = PciIo->Mem.Read(
			 PciIo,
			 EfiPciIoWidthUint32,
			 BarIndex,
			 Offset,
			 1,
			 &Data);
	ASSERT_EFI_ERROR(Status);
	return Data;
#endif
}

VOID*
EhciMemGetPhyAddr(
	HC_STRUC	*HcStruc,
	VOID*		MemPtr
)
{
#if USB_RUNTIME_DRIVER_IN_SMM
	return MemPtr;
#else
    return (VOID*)(HcStruc->MemPoolPhyAddr + ((UINT8*)MemPtr - HcStruc->MemPool));
#endif
}

VOID*
EhciMemGetHostAddr(
	HC_STRUC	*HcStruc,
	VOID*		MemPtr
)
{
#if USB_RUNTIME_DRIVER_IN_SMM
	return MemPtr;
#else
    return (VOID*)(HcStruc->MemPool + ((UINT8*)MemPtr - HcStruc->MemPoolPhyAddr));
#endif
}

VOID*
EhciMemAlloc(
	HC_STRUC	*HcStruc,
	UINT16		NumBlks
)
{
#if USB_RUNTIME_DRIVER_IN_SMM
	return USB_MemAlloc(NumBlks);
#else
	UINT32	SavedMemPages = gUsbData->MemPages;
	UINT8	*SavedMemPool = gUsbData->fpMemBlockStart;
	UINT32	SavedMemBlkStsBytes = gUsbData->MemBlkStsBytes;
	UINT32	*SavedMemBlsSts = gUsbData->aMemBlkSts;
	VOID*	MemPtr;

	gUsbData->MemPages = HcStruc->MemPoolPages;
	gUsbData->fpMemBlockStart = HcStruc->MemPool;
	gUsbData->MemBlkStsBytes = HcStruc->MemBlkStsBytes;
	gUsbData->aMemBlkSts = HcStruc->MemBlkSts;

	MemPtr = USB_MemAlloc(NumBlks);

	gUsbData->MemPages = SavedMemPages;
	gUsbData->fpMemBlockStart = SavedMemPool;
	gUsbData->MemBlkStsBytes = SavedMemBlkStsBytes;
	gUsbData->aMemBlkSts = SavedMemBlsSts;

	return MemPtr;
#endif
}

VOID
EhciMemFree(
	HC_STRUC	*HcStruc,
	VOID*		MemPtr,
	UINT16		NumBlks
)
{
#if USB_RUNTIME_DRIVER_IN_SMM
	USB_MemFree(MemPtr, NumBlks);
#else
	UINT32	SavedMemPages = gUsbData->MemPages;
	UINT8	*SavedMemPool = gUsbData->fpMemBlockStart;
	UINT32	SavedMemBlkStsBytes = gUsbData->MemBlkStsBytes;
	UINT32	*SavedMemBlsSts = gUsbData->aMemBlkSts;

	gUsbData->MemPages = HcStruc->MemPoolPages;
	gUsbData->fpMemBlockStart = HcStruc->MemPool;
	gUsbData->MemBlkStsBytes = HcStruc->MemBlkStsBytes;
	gUsbData->aMemBlkSts = HcStruc->MemBlkSts;

	USB_MemFree(MemPtr, NumBlks);

	gUsbData->MemPages = SavedMemPages;
	gUsbData->fpMemBlockStart = SavedMemPool;
	gUsbData->MemBlkStsBytes = SavedMemBlkStsBytes;
	gUsbData->aMemBlkSts = SavedMemBlsSts;

	return;
#endif
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
