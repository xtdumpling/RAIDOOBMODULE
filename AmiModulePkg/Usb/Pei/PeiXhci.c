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

/** @file PeiXhci.c
    This file is the main source file for the xHCI PEI USB recovery module.
    Its entry point at XhciPeiUsbEntryPoint will be executed from the
    UsbRecoveryInitialize INIT_LIST.

**/
#pragma optimize("", off)

#include <Token.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <PiPei.h>
#include <IndustryStandard/Pci.h>
#include <Ppi/Stall.h>
#include "PeiXhci.h"
#include "UsbPei.h"

#if XHCI_PEI_SUPPORT

#define LOAD_XHC_FIRMWARE_PPI_GUID \
  { 0x563eafe8, 0xcfb3, 0x4082, 0xa9, 0x91, 0x11, 0xe, 0xa, 0x1, 0x72, 0xf4 }

typedef EFI_STATUS (EFIAPI *LOAD_XHC_FIRMWARE) (
	IN EFI_PEI_SERVICES **PeiServices,
	IN UINT64           PciCfgAddr
);

typedef struct {
    LOAD_XHC_FIRMWARE   LoadXhcFirmware;
} LOAD_XHC_FIRMWARE_PPI;

EFI_GUID gLoadXhcFirmwarePpiGuid = LOAD_XHC_FIRMWARE_PPI_GUID;

PCI_BUS_DEV_FUNCTION gXhciControllerPciTable[] = {PEI_XHCI_CONTROLLER_PCI_ADDRESS {0,0,0}};
UINT16 gXhciControllerCount = \
    sizeof(gXhciControllerPciTable) / sizeof(PCI_BUS_DEV_FUNCTION) - 1;

PCI_DEV_REGISTER_VALUE gPeiXhciInitPciTable[] = {PEI_XHCI_CONTROLLER_PCI_REGISTER_VALUES {0,0,0,0,0,0,0}};
UINT16 gPeiXhciInitPciTableCount = \
    sizeof(gPeiXhciInitPciTable) / sizeof(PCI_DEV_REGISTER_VALUE) - 1;

EFI_STATUS  XhciReadHcMem(USB3_CONTROLLER*, EFI_PEI_PCI_CFG_PPI_WIDTH, UINT32, UINTN, VOID*);
EFI_STATUS  XhciWriteHcMem(USB3_CONTROLLER*, EFI_PEI_PCI_CFG_PPI_WIDTH, UINT32, UINTN, VOID*);
VOID        XhciSetHcMem(USB3_CONTROLLER*, UINT32, UINT32);
VOID        XhciClearHcMem(USB3_CONTROLLER*, UINT32, UINT32);
UINT32      XhciReadOpReg(USB3_CONTROLLER*, UINT32);
VOID        XhciWriteOpReg(USB3_CONTROLLER*, UINT32, UINT32);
VOID        XhciSetOpReg(USB3_CONTROLLER*, UINT32, UINT32);
VOID        XhciClearOpReg(USB3_CONTROLLER*, UINT32, UINT32);

/**
    This is the entry point into the XHCI initialization.

        
    @param FfsHeader 
        --  EFI_FFS_FILE_HEADER pointer
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion or valid EFI error code

**/

EFI_STATUS XhciPeiUsbEntryPoint (
    IN EFI_FFS_FILE_HEADER *FfsHeader,
    IN EFI_PEI_SERVICES    **PeiServices )
{

    EFI_STATUS              Status;
    EFI_PEI_STALL_PPI       *StallPpi;
    UINTN                   XhciBaseAddress = PEI_XHCI_MMIOBASE;
    EFI_PHYSICAL_ADDRESS    TempPtr;
    USB3_CONTROLLER         *Usb3Hc;
    UINTN                   MemPages;
    UINT8                   ControllerIndex;
    UINT8                   PciDevIndex;
    UINT8                   CmdRegisterValue = 6;
    UINTN                   Instance = 0;
	LOAD_XHC_FIRMWARE_PPI   *LoadXhcFwPpi;
    EFI_PEI_PCI_CFG_PPI_WIDTH Width;
    UINT8                   PciAttributes;
    UINT8                   ClassCode[4];

    //-------------------------------------------
    // Initialize the EFI_PEI_STALL_PPI interface
    //-------------------------------------------
    Status = (**PeiServices).LocatePpi( PeiServices, &gEfiPeiStallPpiGuid,
        0, NULL, &StallPpi );
    if ( EFI_ERROR( Status ) ) {
        return EFI_UNSUPPORTED;
    }

    // Program PCI registers of the host controller and other relevant PCI
    // devices (e.g. bridges that enable this host).

    for (PciDevIndex = 0; PciDevIndex < gPeiXhciInitPciTableCount; PciDevIndex++) {
        
        switch (gPeiXhciInitPciTable[PciDevIndex].Size) {
            	case 8:     Width = EfiPeiPciCfgWidthUint8; break;
            	case 16:	Width = EfiPeiPciCfgWidthUint16; break;
            	case 32:	Width = EfiPeiPciCfgWidthUint32; break;
            	case 64:	Width = EfiPeiPciCfgWidthUint64; break;
            	default:    continue;
        }
 
        Status = (*PeiServices)->PciCfg->Modify(
                    PeiServices,
                    (*PeiServices)->PciCfg,
                    Width,
                    EFI_PEI_PCI_CFG_ADDRESS(
                        gPeiXhciInitPciTable[PciDevIndex].Bus,
                        gPeiXhciInitPciTable[PciDevIndex].Device,
                        gPeiXhciInitPciTable[PciDevIndex].Function,
                        gPeiXhciInitPciTable[PciDevIndex].Register
                    ),
                    &gPeiXhciInitPciTable[PciDevIndex].SetBits,
                    &gPeiXhciInitPciTable[PciDevIndex].ClearBits);
    }

    for (ControllerIndex = 0; ControllerIndex < gXhciControllerCount;
         ControllerIndex++, XhciBaseAddress += PEI_XHCI_MMIOSIZE) {
        UINT16 Vid;
        UINT16 Did;

        // Get VID/DID, see if controller is visible on PCI
        (*PeiServices)->PciCfg->Read(PeiServices,(*PeiServices)->PciCfg,
            EfiPeiPciCfgWidthUint16, XHCI_PCI_ADDRESS(ControllerIndex, PCI_VENDOR_ID_OFFSET),&Vid);
        
        if (Vid == 0xffff) {
            continue;    // Controller not present
        }
    
        (*PeiServices)->PciCfg->Read(PeiServices,(*PeiServices)->PciCfg,
            EfiPeiPciCfgWidthUint16, XHCI_PCI_ADDRESS(ControllerIndex, PCI_DEVICE_ID_OFFSET), &Did);

        (*PeiServices)->PciCfg->Read(PeiServices,(*PeiServices)->PciCfg,
            EfiPeiPciCfgWidthUint32, XHCI_PCI_ADDRESS(ControllerIndex, PCI_REVISION_ID_OFFSET), ClassCode);

        DEBUG((DEBUG_INFO, "USB XHCI#%d PI %x SubClassCode %x BaseCode %x\n", 
            ControllerIndex, ClassCode[1], ClassCode[2], ClassCode[3]));

        if ((ClassCode[1] != PCI_CLASSC_PI_XHCI) || 
            (ClassCode[2] != PCI_CLASSC_SUBCLASS_SERIAL_USB) ||
            (ClassCode[3] != PCI_CLASSC_BASE_CLASS_SERIAL)) {
            continue;   //This is not a xhci controller.
        }

        //----------------------------------------------------------
        // Allocate USB3_CONTROLLER object that holds all necessary
        // information for the Host Controller operational registers
        // for each controller.  Initialze the controller and setup
        // data structures, get it ready for operation.
        //----------------------------------------------------------
        MemPages = sizeof(USB3_CONTROLLER) / 0x1000 + 1;
        Status = (**PeiServices).AllocatePages(PeiServices, 
                EfiBootServicesData,
                MemPages,
                &TempPtr);
        if (EFI_ERROR(Status)) {
            return EFI_OUT_OF_RESOURCES;
        }

        ZeroMem((VOID*)TempPtr, MemPages * 4096); // Clear allocated memory
  
        Usb3Hc = (USB3_CONTROLLER*)(UINTN)TempPtr;
    
        //----------------------------------------------------------
        // USB3 controller data area is allocated, start stuff it in
        // with the useful filling in with the useful data.
        //----------------------------------------------------------
    
        (*PeiServices)->PciCfg->Read(PeiServices,(*PeiServices)->PciCfg,
            EfiPeiPciCfgWidthUint32,
            XHCI_PCI_ADDRESS(ControllerIndex, PCI_BASE_ADDRESSREG_OFFSET),
            &Usb3Hc->BaseAddress
        );

        PciAttributes = (UINT8)Usb3Hc->BaseAddress;
        
        if (((PciAttributes & (BIT1 |BIT2)) == BIT2) &&
                ((sizeof(VOID*) / sizeof(UINT32) == 2))) {            
            (*PeiServices)->PciCfg->Read(PeiServices,(*PeiServices)->PciCfg,
                EfiPeiPciCfgWidthUint64,
                XHCI_PCI_ADDRESS(ControllerIndex, PCI_BASE_ADDRESSREG_OFFSET),
                &Usb3Hc->BaseAddress
            );
        }
        
        Usb3Hc->BaseAddress &= ~(0x7F); // Clear attributes
        
        Usb3Hc->FreeResource = FALSE;
        
        if (Usb3Hc->BaseAddress == 0) {
            if (RShiftU64(XhciBaseAddress, 32)) {
                // XhciBaseAddress is 64-bit address, check if controller 
                // supports 64-bit address space.
                if ((PciAttributes & (BIT1 |BIT2)) != BIT2) {
                    continue;
                }
                Width = EfiPeiPciCfgWidthUint64;
            } else {
                Width = EfiPeiPciCfgWidthUint32;
            }
            // Program BAR
            (*PeiServices)->PciCfg->Write(PeiServices,(*PeiServices)->PciCfg,
                    Width,
                    XHCI_PCI_ADDRESS(ControllerIndex, PCI_BASE_ADDRESSREG_OFFSET),
                    &XhciBaseAddress
            );
            Usb3Hc->BaseAddress = XhciBaseAddress;
            Usb3Hc->FreeResource = TRUE;
        }
    
        // Enable MMIO access and BM
        (*PeiServices)->PciCfg->Write(PeiServices,(*PeiServices)->PciCfg,
            EfiPeiPciCfgWidthUint8,
            XHCI_PCI_ADDRESS(ControllerIndex, PCI_COMMAND_OFFSET),
            &CmdRegisterValue
        );

        do {
            Status = (*PeiServices)->LocatePpi(PeiServices, &gLoadXhcFirmwarePpiGuid,
                            Instance++, NULL, &LoadXhcFwPpi );
            if (!EFI_ERROR(Status)) {
                Status = LoadXhcFwPpi->LoadXhcFirmware(PeiServices, XHCI_PCI_ADDRESS(ControllerIndex, 0));
                if (!EFI_ERROR(Status)) {
                    break;
                }
            }
        } while (!EFI_ERROR(Status));

        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint8, XHCI_CAPLENGTH_OFFSET, 1, &Usb3Hc->CapRegs.CapLength);
        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint16, XHCI_HCIVERSION_OFFSET, 1, &Usb3Hc->CapRegs.HciVersion);
        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, XHCI_HCSPARAMS1_OFFSET, 1, &Usb3Hc->CapRegs.HcsParams1);
        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, XHCI_HCSPARAMS2_OFFSET, 1, &Usb3Hc->CapRegs.HcsParams2);
        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, XHCI_HCSPARAMS3_OFFSET, 1, &Usb3Hc->CapRegs.HcsParams3);
        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, XHCI_HCCPARAMS1_OFFSET, 1, &Usb3Hc->CapRegs.HccParams1);
        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, XHCI_DBOFF_OFFSET, 1, &Usb3Hc->CapRegs.DbOff);
        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, XHCI_RTSOFF_OFFSET, 1, &Usb3Hc->CapRegs.RtsOff);
        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, XHCI_HCCPARAMS2_OFFSET, 1, &Usb3Hc->CapRegs.HccParams2);
      
        Usb3Hc->Did = Did;
        Usb3Hc->Vid = Vid;
        Usb3Hc->OpRegs = (XHCI_HC_OP_REGS*)(Usb3Hc->BaseAddress + Usb3Hc->CapRegs.CapLength);
        Usb3Hc->PageSize4K = XhciReadOpReg(Usb3Hc, XHCI_PAGESIZE_OFFSET);
        Usb3Hc->ContextSize = 0x20 << Usb3Hc->CapRegs.HccParams1.Csz;
        Usb3Hc->MaxIntrs = Usb3Hc->CapRegs.HcsParams1.MaxIntrs;     // Get maximum number of interrupters
        Usb3Hc->MaxSlots = PEI_XHCI_MAX_SLOTS;
    
        Usb3Hc->InputContext = (VOID*)&Usb3Hc->InpCtx;
        Usb3Hc->XfrRings = Usb3Hc->XfrRing;
        Usb3Hc->XfrTrbs = (UINTN)Usb3Hc->XfrTrb;
        Usb3Hc->DeviceContext = (VOID*)Usb3Hc->DevCtx;
        Usb3Hc->ControllerIndex = ControllerIndex;
        
        XhciExtCapParser(PeiServices, Usb3Hc);
        
        Usb3Hc->DcbaaPtr = (XHCI_DCBAA*)Usb3Hc->Dcbaa;
        XhciAllocateScratchpadBuffers(PeiServices, Usb3Hc);

        DEBUG((DEBUG_INFO, "USB recovery xHCI[%d] controller initialization details:\n", ControllerIndex));
        DEBUG((DEBUG_INFO, "PCI location: B%x/D%x/F%x, VID:DID = %x:%x, BAR0 = %x\n",
            gXhciControllerPciTable[ControllerIndex].Bus,
            gXhciControllerPciTable[ControllerIndex].Device,
            gXhciControllerPciTable[ControllerIndex].Function,
            Usb3Hc->Vid,
            Usb3Hc->Did,
            Usb3Hc->BaseAddress
        ));
        DEBUG((DEBUG_INFO, " MaxSlots = %x, InputCtx %x, Device Ctx %x\n",
            Usb3Hc->MaxSlots, (UINT8*)Usb3Hc->InputContext, (UINT8*)Usb3Hc->DeviceContext));
    
        Usb3Hc->PeiServices = PeiServices;
        Usb3Hc->CpuIoPpi = (*PeiServices)->CpuIo;
        Usb3Hc->StallPpi = StallPpi;
        Usb3Hc->PciCfgPpi = (*PeiServices)->PciCfg;

        // Initialize the xHCI Controller for operation
        Status = XhciInitHC(PeiServices, Usb3Hc, ControllerIndex);
        ASSERT(Status == EFI_SUCCESS);
        if (EFI_ERROR(Status)) return Status;

        // Setup PPI entry point
        Usb3Hc->UsbHostControllerPpi.ControlTransfer = XhciHcControlTransfer;
        Usb3Hc->UsbHostControllerPpi.BulkTransfer = XhciHcBulkTransfer;
        Usb3Hc->UsbHostControllerPpi.SyncInterruptTransfer = XhciHcSyncInterruptTransfer;
        Usb3Hc->UsbHostControllerPpi.GetRootHubPortNumber = XhciHcGetRootHubPortNumber;
        Usb3Hc->UsbHostControllerPpi.GetRootHubPortStatus = XhciHcGetRootHubPortStatus;
        Usb3Hc->UsbHostControllerPpi.SetRootHubPortFeature = XhciHcSetRootHubPortFeature;
        Usb3Hc->UsbHostControllerPpi.ClearRootHubPortFeature = XhciHcClearRootHubPortFeature;
        Usb3Hc->UsbHostControllerPpi.PreConfigureDevice = XhciHcPreConfigureDevice;
        Usb3Hc->UsbHostControllerPpi.EnableEndpoints = XhciEnableEndpoints;
        Usb3Hc->UsbHostControllerPpi.ActivatePolling = XhciHcActivatePolling;
        Usb3Hc->UsbHostControllerPpi.Reset = XhciHcReset;
        Usb3Hc->UsbHostControllerPpi.CurrentAddress = 0;

        Usb3Hc->PpiDescriptor.Flags =(EFI_PEI_PPI_DESCRIPTOR_PPI |EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
        Usb3Hc->PpiDescriptor.Guid = &gAmiPeiUsbHostControllerPpiGuid;
        Usb3Hc->PpiDescriptor.Ppi = &Usb3Hc->UsbHostControllerPpi;

        Status = (**PeiServices).InstallPpi(PeiServices, &Usb3Hc->PpiDescriptor);
        if (EFI_ERROR(Status)) return EFI_NOT_FOUND;

    }

    return EFI_SUCCESS;
}

/**
    This function allocates XHCI scratchpad buffers.

    @note  
  Usb3Hc->DcbaaPtr points to the beginning of memory block first 2048 Bytes
  of which is used for DCBAA.

**/

EFI_STATUS
XhciAllocateScratchpadBuffers (
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc
)
{
    EFI_STATUS              Status;
    UINT16                  NumBufs = ((Usb3Hc->CapRegs.HcsParams2.MaxScratchPadBufsHi) << 5) + 
                                            Usb3Hc->CapRegs.HcsParams2.MaxScratchPadBufsLo;
    UINT16                  Count;
    EFI_PHYSICAL_ADDRESS    TempPtr;
    UINTN                   AlignmentMask;
    UINT64                  *ScratchBufEntry;

    if (NumBufs == 0) {
        return EFI_SUCCESS;
    }

    // Allcate a PAGESIZE boundary for Scratchpad Buffer Array Base Address
    // because bit[0..5] are reserved in  Device Context Base Address Array Element 0.
    Status = (**PeiServices).AllocatePages(PeiServices,
                                EfiBootServicesData,
                                EFI_SIZE_TO_PAGES(sizeof(UINT64) * NumBufs),
                                &TempPtr);

    if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
    }

    ZeroMem((VOID*)TempPtr, (sizeof(UINT64) * NumBufs));

    ScratchBufEntry = (UINT64*)(UINTN)TempPtr;
   
    // Allocate scratch pad buffer: PAGESIZE block located on
    // a PAGESIZE boundary. Section 4.20.
    Status = (**PeiServices).AllocatePages(PeiServices, 
                                EfiBootServicesData,
                                Usb3Hc->PageSize4K * (NumBufs + 1),
                                &TempPtr);
    
    if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
    }

    // Clear allocated memory
    ZeroMem((VOID*)TempPtr, Usb3Hc->PageSize4K * 4096 * (NumBufs + 1));

    AlignmentMask = (Usb3Hc->PageSize4K << 12) - 1;

    TempPtr = ((UINTN)TempPtr +AlignmentMask) & ~AlignmentMask;

    for (Count = 0; Count < NumBufs; Count++) {    
        // Update *ScratchBufArrayBase
        ScratchBufEntry[Count] = (UINTN)TempPtr + (Usb3Hc->PageSize4K << 12) * Count;
    }

    // Update scratch pad pointer
    *(UINTN*)Usb3Hc->DcbaaPtr = (UINTN)ScratchBufEntry;

    return EFI_SUCCESS;
}

/**
    This function initializes xHCI controller registers and starts it.

    @param 
        EFI_PEI_SERVICES    **PeiServices
        -- PEI_SERVICES pointer
        USB3_CONTROLLER     *Usb3Hc
        -- XHCI controller data structure pointer
        UINT8               ControllerIndex
        -- 0-based index of the controller to be initialized

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful initialization completion

**/

EFI_STATUS
XhciInitHC(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc,
    UINT8               ControllerIndex
)
{
    XHCI_INTERRUPTER_REGS   *Interrupter;
    XHCI_ER_SEGMENT_ENTRY   *Erst0Entry;
    BOOLEAN     PpSet = FALSE;
    UINT8       PortNumber;
    UINT32      CurrentPortOffset = 0;
    UINT32      i;
    UINT8	    Count;
    UINT32      Data;
    UINT32      PortStsOffset;
    UINT32      DbCapDcctrl = 0;

	// Wait controller ready
	for (i = 0; i < PEI_XHCI_READY_TIMEOUT_MS * 10; i++) {
        if (!(XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_CNR)) {
            break;
        }
		XHCI_FIXED_DELAY_US(Usb3Hc, 100);
    }
    ASSERT(!(XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_CNR));

    if (XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_CNR) {
        return EFI_DEVICE_ERROR;
    }

	// Check if the xHC is halted
	if (!(XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_HALTED)) {
        XhciClearOpReg(Usb3Hc, XHCI_USBCMD_OFFSET, XHCI_CMD_RS);
		// The xHC should halt within 16 ms. Section 5.4.1.1
		for (i = 0; i < PEI_XHCI_HALT_TIMEOUT_MS * 10; i++) {
            if (XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_HALTED) {
                break;
            }
            XHCI_FIXED_DELAY_US(Usb3Hc, 100);
		}
		ASSERT(XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_HALTED);
        if (!(XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_HALTED)) {
            return EFI_DEVICE_ERROR;
        }
	}

    // Reset controller
    if (Usb3Hc->DbCapOffset) {
        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, Usb3Hc->DbCapOffset + XHCI_DB_CAP_DCCTRL_REG, 1, &DbCapDcctrl);
    }
    if (!(DbCapDcctrl & XHCI_DB_CAP_DCE)) {
        XhciSetOpReg(Usb3Hc, XHCI_USBCMD_OFFSET, XHCI_CMD_HCRST);

        // Workaround for the issue in some silicons.
        // Add short delay to avoid race condition after write USBCMD.HCRST 
        // for issuing HC Reset and before accessing any MMIO register.
        XHCI_FIXED_DELAY_MS(Usb3Hc, PEI_XHCI_RESET_DELAY_MS);
        
        for (i = 0; i < PEI_XHCI_RESET_TIMEOUT_MS * 10; i++) {
            if (!(XhciReadOpReg(Usb3Hc, XHCI_USBCMD_OFFSET) & XHCI_CMD_HCRST)) {
                break;
            }
            XHCI_FIXED_DELAY_US(Usb3Hc, 100);
        }

        ASSERT(!(XhciReadOpReg(Usb3Hc, XHCI_USBCMD_OFFSET) & XHCI_CMD_HCRST));
        if (XhciReadOpReg(Usb3Hc, XHCI_USBCMD_OFFSET) & XHCI_CMD_HCRST) {
            return EFI_DEVICE_ERROR;  // Controller can not be reset
        }
    }

	// Wait controller ready
	for (i = 0; i < PEI_XHCI_READY_TIMEOUT_MS * 10; i++) {
        if (!(XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_CNR)) {
            break;
        }
		XHCI_FIXED_DELAY_US(Usb3Hc, 100);
    }
    
    ASSERT(!(XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_CNR));

    if (XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_CNR) {
        return EFI_DEVICE_ERROR;
    }

//APTIOV_SERVER_OVERRIDE_START: Set HSEE if it is disabled
#if PEI_USB_HOST_SYSTEM_ERRORS_SUPPORT
    if (!(XhciReadOpReg(Usb3Hc, XHCI_USBCMD_OFFSET) & XHCI_CMD_HSEE)) {
        XhciSetOpReg(Usb3Hc, XHCI_USBCMD_OFFSET, XHCI_CMD_HSEE);
    }
#endif
//APTIOV_SERVER_OVERRIDE_END: Set HSEE if it is disabled

    Usb3Hc->RtRegs = (XHCI_HC_RT_REGS*)(Usb3Hc->BaseAddress + Usb3Hc->CapRegs.RtsOff);
    DEBUG((DEBUG_INFO, "PEI_XHCI: RT registers are at %x\n", Usb3Hc->RtRegs));

    XhciWriteOpReg(Usb3Hc, XHCI_CONFIG_OFFSET, Usb3Hc->MaxSlots); // Max device slots enabled

    XhciMmio64Write(Usb3Hc, (UINTN)&Usb3Hc->OpRegs->DcbAap, (UINT64)(UINTN)Usb3Hc->DcbaaPtr);

    // Define the Command Ring Dequeue Pointer by programming the Command Ring
    // Control Register (5.4.5) with a 64-bit address pointing to the starting
    // address of the first TRB of the Command Ring.

    // Initialize Command Ring Segment: Size TRBS_PER_SEGMENT*16, 64 Bytes aligned
    XhciInitRing(&Usb3Hc->CmdRing, (UINTN)Usb3Hc->CommandRing, TRBS_PER_SEGMENT, TRUE);
    DEBUG((DEBUG_INFO, "CMD Ring is at %x\n", (UINTN)&Usb3Hc->CmdRing));

    // Write CRCR HC register with the allocated address. Set Ring Cycle State to 1.
    XhciMmio64Write(Usb3Hc, (UINTN)&Usb3Hc->OpRegs->Crcr,
            (UINT64)(UINTN)Usb3Hc->CmdRing.Base + CRCR_RING_CYCLE_STATE);

    // Initialize and assign Event Ring
    XhciInitRing(&Usb3Hc->EvtRing, (UINTN)Usb3Hc->EventRing, TRBS_PER_SEGMENT, FALSE);
    DEBUG((DEBUG_INFO, "EVT Ring is at %x\n", (UINTN)&Usb3Hc->EvtRing));

    // NOTE: This driver supports one Interrupter, hence it uses
    // one Event Ring segment with TRBS_PER_SEGMENT TRBs in it.

    // Initialize ERST[0]
    Erst0Entry = &Usb3Hc->Erst;
    Erst0Entry->RsBase = (UINT64)(UINTN)Usb3Hc->EvtRing.Base;
    Erst0Entry->RsSize = TRBS_PER_SEGMENT;

    Interrupter = Usb3Hc->RtRegs->IntRegs;

    // Initialize Interrupter fields
    Data = 1;
    XhciWriteHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32,
        (UINT32)((UINTN)&Usb3Hc->RtRegs->IntRegs->Erstz - Usb3Hc->BaseAddress), 1, &Data);
    // ER dequeue pointer
    XhciMmio64Write(Usb3Hc, (UINTN)&Interrupter->Erdp, (UINT64)(UINTN)Usb3Hc->EvtRing.QueuePtr);
    // Seg Table location
    XhciMmio64Write(Usb3Hc, (UINTN)&Interrupter->Erstba, (UINT64)(UINTN)Erst0Entry);
    Data = XHCI_IMODI;
    XhciWriteHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32,
        (UINT32)((UINTN)&Usb3Hc->RtRegs->IntRegs->IMod - Usb3Hc->BaseAddress), 1, &Data);
    XhciSetOpReg(Usb3Hc, XHCI_USBCMD_OFFSET, XHCI_CMD_INTE);
    // Enable interrupt
    XhciSetHcMem(Usb3Hc, 
        (UINT32)((UINTN)&Usb3Hc->RtRegs->IntRegs->IMan - Usb3Hc->BaseAddress), BIT1);

    DEBUG((DEBUG_INFO, "Transfer Rings structures start at %x\n", Usb3Hc->XfrRings));

    // Set PortPower unless PowerPortControl indicates otherwise
    if (Usb3Hc->CapRegs.HccParams1.Ppc != 0) {
        for (PortNumber = 0; PortNumber < Usb3Hc->CapRegs.HcsParams1.MaxPorts;
                PortNumber++, CurrentPortOffset+=0x10) {
            PortStsOffset = XHCI_PORTSC_OFFSET + (0x10 * (PortNumber - 1));
            if (!(XhciReadOpReg(Usb3Hc, PortStsOffset) & XHCI_PCS_PP)) {
                // Set port power
                XhciSetOpReg(Usb3Hc, PortStsOffset, XHCI_PCS_PP);
                PpSet = TRUE;
            }
        }
        if (PpSet) {
            XHCI_FIXED_DELAY_MS(Usb3Hc, 20);   // Wait for 20 ms, Section 5.4.8
        }
    }

    XhciSetOpReg(Usb3Hc, XHCI_USBCMD_OFFSET, XHCI_CMD_RS);

    for (i = 0; i < PEI_XHCI_HALT_TIMEOUT_MS * 10; i++) {
        if (!(XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_HALTED)) {
            break;
        }
        XHCI_FIXED_DELAY_US(Usb3Hc, 100);
    }
    ASSERT(!(XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_HALTED));
    
    XhciResetUsb2Port(PeiServices, Usb3Hc);    //(EIP75547+)

    if (Usb3Hc->Usb2Protocol.PortCount) {
		for (Count = 0; Count < Usb3Hc->Usb2Protocol.PortCount; Count++) {
			PortNumber = Count + Usb3Hc->Usb2Protocol.PortOffset;
            PortStsOffset = XHCI_PORTSC_OFFSET + + (0x10 * (PortNumber - 1));
            if (XhciReadOpReg(Usb3Hc, PortStsOffset) & XHCI_PCS_PED) {
                XhciWriteOpReg(Usb3Hc, PortStsOffset, XHCI_PCS_PED | XHCI_PCS_PP);
			    for (i = 0; i < PEI_XHCI_DISABLE_USB2_PORT_TIMEOUT_MS * 10; i++) {
				    if (!(XhciReadOpReg(Usb3Hc, PortStsOffset) & XHCI_PCS_PED)) {
					    break;
				    }
				    XHCI_FIXED_DELAY_US(Usb3Hc, 100);
			    }
            }
        }
    }
        
    return EFI_SUCCESS;
}

/**
    This function uses an Address Device Command to transition the state of 
    the USB device.
    BSR flag is 1, it transitions the state to Default.
    BSR flag is 0, it transitions the state to Address.

    @param 
        PeiServices - Pointer to the PEI services table
        Usb3Hc  - Pointer to the Usb3Hc structure
        PeiUsbDev - Pointer to the PeiUsbDev structure
        SlotId -The index of a Device Slot
        Port - The index of a port in the root hub port or the Usb hub port
        
    @retval EFI_SUCCESS This function transitions the state of the USB device.
    @retval EFI_DEVICE_ERROR This function doesn't transitions the state of the USB device.
  
**/

EFI_STATUS
XhciAddressDevice(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc,
    PEI_USB_DEVICE      *PeiUsbDev,
    UINT8		        SlotId,
    UINT8               Port
)
{
    EFI_STATUS                  Status;
    XHCI_INPUT_CONTROL_CONTEXT  *InputCtrl = NULL;
    XHCI_SLOT_CONTEXT           *InputSlot = NULL;
    XHCI_SLOT_CONTEXT           *OutputSlot = NULL;
    XHCI_SLOT_CONTEXT           *ParentHubSlotCtx = NULL;
    XHCI_EP_CONTEXT             *InputEp0 = NULL;
    XHCI_EP_CONTEXT             *OutputEp0 = NULL;
    VOID                        *DevCtx = XhciGetDeviceContext(Usb3Hc, SlotId);
    VOID                        *InputCtx = Usb3Hc->InputContext;
    TRB_RING                    *XfrRing = NULL;
    UINT16                      AddrDevParam = 0;
    UINT8                       Bsr = 0;
    UINT8                       i;
    UINT8	                    ParentHubSlotId;
    UINT8		                *ParentHubDevice;

    OutputSlot = XhciGetContextEntry(Usb3Hc, DevCtx, 0);
     
    if (OutputSlot->SlotState >= XHCI_SLOT_STATE_ADDRESSED) {
        return EFI_DEVICE_ERROR;
    }

    // Initialize data structures associated with the slot 4.3.3

    // Zero the InputContext
    ZeroMem(InputCtx, XHCI_INPUT_CONTEXT_ENTRIES * Usb3Hc->ContextSize);

    // Initialize the Input Control Context of the Input Context
    // by setting the A0 and A1 flags to 1

    InputCtrl = (XHCI_INPUT_CONTROL_CONTEXT*)XhciGetContextEntry(Usb3Hc, InputCtx, 0);
    InputCtrl->AddContextFlags = BIT0 + BIT1;

    // Initialize the Input Slot Context data structure
    InputSlot = (XHCI_SLOT_CONTEXT*)XhciGetContextEntry(Usb3Hc, InputCtx, 1);    
    InputSlot->RouteString = 0;
    InputSlot->ContextEntries = 1;
                                        
	if (PeiUsbDev->TransactionTranslator == 0) {
		InputSlot->RootHubPort = Port;
	} else {
		Status = GetSlotId(Usb3Hc->DeviceMap, &ParentHubSlotId, (UINT8)(PeiUsbDev->TransactionTranslator & 0x7F));
		ASSERT(Status == EFI_SUCCESS);
		ParentHubDevice = (UINT8*)XhciGetDeviceContext(Usb3Hc, ParentHubSlotId);
		ParentHubSlotCtx = (XHCI_SLOT_CONTEXT*)XhciGetContextEntry(Usb3Hc, ParentHubDevice, 0);
		InputSlot->RootHubPort = ParentHubSlotCtx->RootHubPort;
	}

    switch (PeiUsbDev->DeviceSpeed) {
        case USB_HIGH_SPEED_DEVICE: 
            InputSlot->Speed = XHCI_DEVSPEED_HIGH; 
            break;
        case USB_SLOW_SPEED_DEVICE:
            InputSlot->Speed = XHCI_DEVSPEED_LOW;
            break;
        case USB_FULL_SPEED_DEVICE:
            InputSlot->Speed = XHCI_DEVSPEED_FULL;
            break;
        case USB_SUPER_SPEED_DEVICE:
            InputSlot->Speed = XHCI_DEVSPEED_SUPER;
            break;
        case USB_SUPER_SPEED_PLUS_DEVICE:
            InputSlot->Speed = XHCI_DEVSPEED_SUPER_PLUS;
            break;
    }

    OutputEp0 = XhciGetContextEntry(Usb3Hc, DevCtx, 1);

    switch (OutputEp0->EpState) {
        case XHCI_EP_STATE_DISABLED:
            XfrRing = XhciInitXfrRing(Usb3Hc, SlotId, 0);
            break;
        case XHCI_EP_STATE_RUNNING:
        case XHCI_EP_STATE_STOPPED:
            XfrRing = XhciGetXfrRing(Usb3Hc, SlotId, 0);
            break;
        default:
            break;
    }

    // Initialize the Input default control Endpoint 0 Context

    InputEp0 = (XHCI_EP_CONTEXT*)XhciGetContextEntry(Usb3Hc, InputCtx, 2);
    InputEp0->EpType = XHCI_EPTYPE_CTL;
    InputEp0->MaxPacketSize = PeiUsbDev->MaxPacketSize0;
    InputEp0->TrDequeuePtr = (UINT64)(UINTN)XfrRing->QueuePtr | XfrRing->CycleBit;
	InputEp0->AvgTrbLength = 8;
    InputEp0->ErrorCount = 3;

	// Initialize Route String and TT fields
	if (PeiUsbDev->TransactionTranslator != 0) {
		if ((ParentHubSlotCtx->Speed == XHCI_DEVSPEED_SUPER) ||
		        (ParentHubSlotCtx->Speed == XHCI_DEVSPEED_SUPER_PLUS)) {	
			for (i = 0; i < 5; i++) {
				if (((ParentHubSlotCtx->RouteString >> (i << 2)) & 0xF) == 0) {
					break;
				}
			}
	        InputSlot->RouteString = ParentHubSlotCtx->RouteString | (Port << (i << 2));
		} else {
			// Update TT fields in the Slot context for LS/FS device connected to HS hub
	        if (InputSlot->Speed == XHCI_DEVSPEED_FULL || InputSlot->Speed == XHCI_DEVSPEED_LOW) {
	            if (ParentHubSlotCtx->Speed == XHCI_DEVSPEED_HIGH) {
					InputSlot->TtHubSlotId = ParentHubSlotId;
	                InputSlot->TtPortNumber = (UINT8)(PeiUsbDev->TransactionTranslator >> 7);
	                InputSlot->MultiTT = ParentHubSlotCtx->MultiTT;
	            } else {
					InputSlot->TtHubSlotId = ParentHubSlotCtx->TtHubSlotId;
	                InputSlot->TtPortNumber = ParentHubSlotCtx->TtPortNumber;
	                InputSlot->MultiTT = ParentHubSlotCtx->MultiTT;
	            }
	        }
		}
	}

    Bsr = (InputSlot->Speed != XHCI_DEVSPEED_SUPER &&
            InputSlot->Speed != XHCI_DEVSPEED_SUPER_PLUS && 
            OutputSlot->SlotState == XHCI_SLOT_STATE_DISABLED) ? 1 : 0;

    AddrDevParam = (UINT16)SlotId | (Bsr << 8);

    // Assign a new address 4.3.4, 4.6.5
    Status = XhciExecuteCommand(PeiServices, Usb3Hc, XhciTAddressDeviceCmd, &AddrDevParam);
	if (Status != EFI_SUCCESS) {
		XhciExecuteCommand(PeiServices, Usb3Hc, XhciTDisableSlotCmd, &SlotId);
		Usb3Hc->DcbaaPtr->DevCntxtAddr[SlotId - 1] = 0;
		return Status;
	}

    if (Bsr == 0) {
        DEBUG((DEBUG_INFO, "PEI_XHCI: new device address %d\n", OutputSlot->DevAddr));
    }

    return EFI_SUCCESS;
}


/**
    This function does preliminary device initialization: enables slot and
    sets the address.

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful pre-configuration completion
        = EFI_DEVICE_ERROR on error

**/

EFI_STATUS
EFIAPI
XhciHcPreConfigureDevice(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN VOID                        *UsbDev,
    IN UINT8                       Port
)
{
    PEI_USB_DEVICE  *PeiUsbDev = (PEI_USB_DEVICE*)UsbDev;
    USB3_CONTROLLER *Usb3Hc = PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS(PeiUsbDev->UsbHcPpi);
	UINT8		*Device;
    EFI_STATUS  Status;
    UINT8       SlotId;
    
    if ((PeiUsbDev->DeviceSpeed == USB_HIGH_SPEED_DEVICE) ||
        (PeiUsbDev->DeviceSpeed == USB_FULL_SPEED_DEVICE) ||
        (PeiUsbDev->DeviceSpeed == USB_SLOW_SPEED_DEVICE)) {
        XHCI_FIXED_DELAY_MS(Usb3Hc, 5);
    }

    // Obtain device slot using Enable Slot command, 4.3.2, 4.6.3
    Status = XhciExecuteCommand(PeiServices, Usb3Hc, XhciTEnableSlotCmd, &SlotId);
    //ASSERT(Status == EFI_SUCCESS);
    //ASSERT(SlotId != 0);
	if (Status != EFI_SUCCESS) return Status;

    PeiUsbDev->SlotId = SlotId;

    Device = (UINT8*)XhciGetDeviceContext(Usb3Hc, SlotId);
    ZeroMem(Device, XHCI_DEVICE_CONTEXT_ENTRIES * Usb3Hc->ContextSize);

    // Update DCBAA with the new device pointer (index = SlotId)
    Usb3Hc->DcbaaPtr->DevCntxtAddr[SlotId -1] = (UINT64)(UINTN)Device;
    DEBUG((DEBUG_INFO, "PEI_XHCI: Slot[%d] enabled, device context at %x\n", SlotId, Device));

    Status = XhciAddressDevice(PeiServices, Usb3Hc, PeiUsbDev, SlotId, Port);

    if (Status == EFI_SUCCESS) {
        UpdateDeviceMap(Usb3Hc->DeviceMap, SlotId, Port, PeiUsbDev, 0);
    }
    return Status;
}


/**
    This function updates SlotId<->Address mapping table.

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS if update is successful
        = EFI_NOT_FOUND if there is no room for a new entry in the map

**/

EFI_STATUS
UpdateDeviceMap(
    PEI_XHCI_SLOTADDR_MAP       *DeviceMap,
    UINT8                       SlotId,
    UINT8                       PortNumber,
    PEI_USB_DEVICE              *PeiUsbDev,
    UINT8                       DevAddr
)
{
    UINT8   i;
    PEI_XHCI_SLOTADDR_MAP   *Map = DeviceMap;

    if (DevAddr == 0) {
        Map->SlotId = SlotId;
        Map->PortNumber = PortNumber;
        Map->PeiUsbDev = PeiUsbDev;
        return EFI_SUCCESS;
    } else {
        for (i = 1; i < PEI_XHCI_MAX_SLOTS + 1; i++, Map++) {
            if (Map->SlotId == 0) {
                Map->SlotId = SlotId;
                Map->DevAddr = DevAddr;
                Map->PortNumber  = PortNumber;
                Map->PeiUsbDev = PeiUsbDev;
                return EFI_SUCCESS;
            }
        }
    }
    return EFI_NOT_FOUND;
}


/**
    This function retrieves SlotId from the Slot<->Address mapping table.

    @retval 
        SlotId variable is updated
        EFI_STATUS (Return Value)
        = EFI_SUCCESS if update is successful
        = EFI_NOT_FOUND if the requested Slot is not found in the mapping table

**/

EFI_STATUS
GetSlotId(
    PEI_XHCI_SLOTADDR_MAP   *DeviceMap,
    UINT8   *SlotId,
    UINT8   DevAddr
)
{
    UINT8   i;
    PEI_XHCI_SLOTADDR_MAP   *Map = DeviceMap;

    for (i = 1; i < PEI_XHCI_MAX_SLOTS + 1; i++, Map++) {
        if (Map->DevAddr == DevAddr) {
            *SlotId = Map->SlotId;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

/**
    This function is called to reset endpoint.

    @param 
        Stalled EP data - SlotId and DCI

**/

UINT8
XhciResetEndpoint(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc,
    UINT8       SlotId,
    UINT8       Dci
)
{
	UINT16      EpInfo;
    EFI_STATUS  Status = EFI_SUCCESS;
    XHCI_EP_CONTEXT *EpCtx;

    EpCtx = (XHCI_EP_CONTEXT*)XhciGetContextEntry(Usb3Hc, 
					XhciGetDeviceContext(Usb3Hc, SlotId), Dci);
    
    // The Reset Endpoint Command is issued by software to recover 
    // from a halted condition on an endpoint.
    if (EpCtx->EpState == XHCI_EP_STATE_HALTED) {  
        // Reset stalled endpoint
        EpInfo = (Dci << 8) + SlotId;
        Status = XhciExecuteCommand(PeiServices, Usb3Hc, XhciTResetEndpointCmd, &EpInfo);
    }
	return Status;
}


/**
    This function is called to restart endpoint. After Endpoint STALLs, it
    transitions from Halted to Stopped state. It is restored back to Running
    state by moving the endpoint ring dequeue pointer past the failed control
    transfer with a Set TR Dequeue Pointer. Then it is restarted by ringing the
    doorbell. Alternatively endpint is restarted using Configure Endpoint command.

    @param 
        Stalled EP data - SlotId and DCI

**/

EFI_STATUS
XhciClearStalledEp(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc,
    UINT8       SlotId,
    UINT8       Dci
)
{
    UINT16      EpInfo;
    TRB_RING    *XfrRing;
    EFI_STATUS  Status = EFI_SUCCESS;
    XHCI_SET_TRPTR_CMD_TRB  Trb;
    XHCI_EP_CONTEXT *EpCtx;

    EpCtx = (XHCI_EP_CONTEXT*)XhciGetContextEntry(Usb3Hc, 
					XhciGetDeviceContext(Usb3Hc, SlotId), Dci);

    // The Reset Endpoint Command is issued by software to recover 
    // from a halted condition on an endpoint.
    if (EpCtx->EpState == XHCI_EP_STATE_HALTED) {
        // Reset stalled endpoint
        EpInfo = (Dci << 8) + SlotId;
        Status = XhciExecuteCommand(PeiServices, Usb3Hc, XhciTResetEndpointCmd, &EpInfo);
    }

    // Set TR Dequeue Pointer command may be executed only if the target 
    // endpoint is in the Error or Stopped state.
    if ((EpCtx->EpState == XHCI_EP_STATE_STOPPED) || 
        (EpCtx->EpState == XHCI_EP_STATE_ERROR)) {
        
        XfrRing = XhciGetXfrRing(Usb3Hc, SlotId, Dci-1);
        Trb.TrPointer = (UINT64)((UINTN)XfrRing->QueuePtr + XfrRing->CycleBit); // Set up DCS
        Trb.EndpointId = Dci;
        Trb.SlotId = SlotId;
        Status = XhciExecuteCommand(PeiServices, Usb3Hc, XhciTSetTRDequeuePointerCmd, &Trb);
    }

    return Status;
}

/**

**/

EFI_STATUS
XhciClearEndpointState(
    IN EFI_PEI_SERVICES                     **PeiServices,
    USB3_CONTROLLER                         *Usb3Hc,
    UINT8                                   SlotId,
	IN UINT8		                        Endpoint
)
{
	UINT8		                Dci;
    TRB_RING                    *XfrRing;
    XHCI_SET_TRPTR_CMD_TRB      Trb;
    XHCI_EP_CONTEXT             *EpCtx;
    UINT16                      EpInfo;
    EFI_STATUS                  Status = EFI_SUCCESS;
    
	if (Endpoint != 0) {
		Dci = (Endpoint & 0xF) * 2 + (Endpoint >> 7);
	} else {
		Dci = 1;
	}

    EpCtx = (XHCI_EP_CONTEXT*)XhciGetContextEntry(Usb3Hc, 
                            XhciGetDeviceContext(Usb3Hc, SlotId), Dci);

    if (EpCtx->EpState == XHCI_EP_STATE_RUNNING) {
        EpInfo = (Dci << 8) + SlotId;
        Status = XhciExecuteCommand(PeiServices, Usb3Hc, XhciTStopEndpointCmd, &EpInfo);
    } 

    // Set TR Dequeue Pointer command may be executed only if the target 
    // endpoint is in the Error or Stopped state.
    if ((EpCtx->EpState == XHCI_EP_STATE_STOPPED) ||
        (EpCtx->EpState == XHCI_EP_STATE_ERROR)) {
        
    	XfrRing = XhciGetXfrRing(Usb3Hc, SlotId, Dci -1);
    	Trb.TrPointer = (UINT64)((UINTN)XfrRing->QueuePtr + XfrRing->CycleBit); // Set up DCS
    	Trb.EndpointId = Dci;
    	Trb.SlotId = SlotId;
    	Status = XhciExecuteCommand(PeiServices, Usb3Hc, XhciTSetTRDequeuePointerCmd, &Trb);
    }

	return Status;
}

/**
    This function verifies the MaxPacket size of the control pipe. If it does
    not match the one received as a part of GET_DESCRIPTOR, then this function
    updates the MaxPacket data in DeviceContext and HC is notified via
    EvaluateContext command.

    @param 
        Usb3Hc  Pointer to the HC structure
        Device  Evaluated device context pointer
        SlotId  Device context index in DCBAA
        Endp0MaxPacket  Max packet size obtained from the device

**/

VOID
XhciUpdateEp0MaxPacket(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc,
    UINT8               SlotId,
    UINT8               Endp0MaxPacket
)
{
    UINT8   Status;
	UINT8	*DevCtx;
	XHCI_INPUT_CONTROL_CONTEXT	*CtlCtx;
	XHCI_SLOT_CONTEXT			*SlotCtx;
	XHCI_EP_CONTEXT				*EpCtx;

	DevCtx = (UINT8*)XhciGetDeviceContext(Usb3Hc, SlotId);

	SlotCtx = (XHCI_SLOT_CONTEXT*)XhciGetContextEntry(Usb3Hc, DevCtx, 0);
	if (SlotCtx->Speed != XHCI_DEVSPEED_FULL) return;

	EpCtx = (XHCI_EP_CONTEXT*)XhciGetContextEntry(Usb3Hc, DevCtx, 1);
	if (EpCtx->MaxPacketSize == Endp0MaxPacket) return;

    // Prepare input context for EvaluateContext comand
    ZeroMem((UINT8*)Usb3Hc->InputContext, XHCI_INPUT_CONTEXT_ENTRIES * Usb3Hc->ContextSize);

    CtlCtx = (XHCI_INPUT_CONTROL_CONTEXT*)XhciGetContextEntry(Usb3Hc, (UINT8*)Usb3Hc->InputContext, 0);
    CtlCtx->AddContextFlags = BIT1;

	EpCtx = (XHCI_EP_CONTEXT*)XhciGetContextEntry(Usb3Hc, (UINT8*)Usb3Hc->InputContext, 2);
    EpCtx->MaxPacketSize = Endp0MaxPacket;

    Status = XhciExecuteCommand(PeiServices, Usb3Hc, XhciTEvaluateContextCmd, &SlotId);
    //ASSERT(Status == EFI_SUCCESS);
}

/**

**/

EFI_STATUS
XhciRingDoorbell(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc,
	UINT8               SlotId,
	UINT8               Dci
)
{
	UINT32 DoorbellOffset;
	XHCI_EP_CONTEXT *EpCtx = NULL;
	UINT32	Count;

    DoorbellOffset = XhciGetTheDoorbellOffset(Usb3Hc, SlotId);
    XhciWriteHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, DoorbellOffset, 1, &Dci);

	if (SlotId == 0) {
		return EFI_DEVICE_ERROR;
	}

	EpCtx = (XHCI_EP_CONTEXT*)XhciGetContextEntry(Usb3Hc, 
					XhciGetDeviceContext(Usb3Hc, SlotId), Dci);
	// Wait for the endpoint running
	for (Count = 0; Count < PEI_XHCI_EP_RUNNING_TIMEOUT_MS * 10; Count++) {
		if (EpCtx->EpState == XHCI_EP_STATE_RUNNING) {
			break;
		}
		XHCI_FIXED_DELAY_US(Usb3Hc, 100);    // 1 ms out of TimeOutMs
	}

    if (EpCtx->EpState != XHCI_EP_STATE_RUNNING) {
        return EFI_DEVICE_ERROR;
    }
    
	return EFI_SUCCESS;
}
/**
    This function intiates a USB control transfer and waits on it to 
    complete.

        
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer
    @param This 
        --  PEI_USB_HOST_CONTROLLER_PPI pointer
    @param bDeviceAddress 
        --  USB address of the device for which the control 
        transfer is to be issued
    @param DeviceType 
        --  Not used
    @param MaximumPacketLength 
        --  Maximum number of bytes that can be sent to or 
        received from the endpoint in a single data packet
    @param Request 
        --  EFI_USB_DEVICE_REQUEST pointer
    @param TransferDirection 
        --  Direction of transfer
    @param DataBuffer 
        --  Pointer to source or destination buffer
    @param DataLength 
        --  Length of buffer
    @param TimeOut 
        --  Not used
    @param TransferResult 
        --  Not used

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS 
EFIAPI
XhciHcControlTransfer(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       DeviceAddress,
    IN UINT8                       DeviceSpeed,
    IN UINT8                       MaximumPacketLength,
    IN UINT16                      TransactionTranslator    OPTIONAL,
    IN EFI_USB_DEVICE_REQUEST      *Request,
    IN EFI_USB_DATA_DIRECTION      TransferDirection,
    IN OUT VOID *Data              OPTIONAL,
    IN OUT UINTN *DataLength       OPTIONAL,
    IN UINTN                       TimeOut,
    OUT UINT32                     *TransferResult 
)
{
    USB3_CONTROLLER *Usb3Hc = PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS(This);
    XHCI_TRB    *Trb;
    UINT8       SlotId;
    UINT8       CompletionCode;
    EFI_STATUS  Status;
    TRB_RING    *XfrRing;
    UINT16      Rq = ((UINT16)Request->Request << 8) + Request->RequestType;
    XHCI_SLOT_CONTEXT	*SlotCtx = NULL;
	UINT8	            *DevCtx;

    if (DeviceAddress == 0) {
        SlotId = Usb3Hc->DeviceMap->SlotId;
    } else {
        Status = GetSlotId(Usb3Hc->DeviceMap, &SlotId, DeviceAddress);
        ASSERT(Status == EFI_SUCCESS);
    }
    
    if (SlotId == 0) {
        return EFI_DEVICE_ERROR;
    }
      
	DevCtx = (UINT8*)XhciGetDeviceContext(Usb3Hc, SlotId);

	SlotCtx = (XHCI_SLOT_CONTEXT*)XhciGetContextEntry(Usb3Hc, DevCtx, 0);

    // Skip SET_ADDRESS request
    if (Request->RequestType == USB_DEV_SET_ADDRESS_REQ_TYPE &&
        Request->Request == USB_DEV_SET_ADDRESS) {
        if (SlotCtx->SlotState == XHCI_SLOT_STATE_DEFAULT) {
            Status = XhciAddressDevice(PeiServices, Usb3Hc, 
                Usb3Hc->DeviceMap->PeiUsbDev, SlotId, Usb3Hc->DeviceMap->PortNumber);
        }
        Status = UpdateDeviceMap(Usb3Hc->DeviceMap, SlotId, 
            Usb3Hc->DeviceMap->PortNumber, 
            Usb3Hc->DeviceMap->PeiUsbDev, (UINT8)Request->Value);
        ASSERT(Status == EFI_SUCCESS);
        return EFI_SUCCESS;
    }

    *TransferResult = EFI_USB_NOERROR;

    // Insert Setup, Data(if needed), and Status TRBs into the transfer ring
    XfrRing = XhciGetXfrRing(Usb3Hc, SlotId, 0);

    // Setup TRB
    Trb = XhciAdvanceEnqueuePtr(XfrRing);
    Trb->TrbType = XhciTSetupStage;
    ((XHCI_SETUP_XFR_TRB*)Trb)->Idt = 1;
    *(UINT16*)&((XHCI_SETUP_XFR_TRB*)Trb)->bmRequestType = Rq;
    ((XHCI_SETUP_XFR_TRB*)Trb)->wValue = Request->Value;
    ((XHCI_SETUP_XFR_TRB*)Trb)->wIndex = Request->Index;
    ((XHCI_SETUP_XFR_TRB*)Trb)->wLength = (UINT16)(*DataLength);
    ((XHCI_SETUP_XFR_TRB*)Trb)->XferLength = 8;

    if (Usb3Hc->CapRegs.HciVersion >= 0x100) {
        if (*DataLength != 0) {
            if ((Rq & BIT7) != 0) {
                ((XHCI_SETUP_XFR_TRB*)Trb)->Trt = XHCI_XFER_TYPE_DATA_IN;
            } else {
                ((XHCI_SETUP_XFR_TRB*)Trb)->Trt = XHCI_XFER_TYPE_DATA_OUT;
            }
        } else {
            ((XHCI_SETUP_XFR_TRB*)Trb)->Trt = XHCI_XFER_TYPE_NO_DATA;
        }
    }
    
    // Data TRB
    if (*DataLength != 0) {
        Trb = XhciAdvanceEnqueuePtr(XfrRing);
        Trb->TrbType = XhciTDataStage;
        ((XHCI_DATA_XFR_TRB*)Trb)->Dir = ((Rq & BIT7) != 0)? 1 : 0;
        ((XHCI_DATA_XFR_TRB*)Trb)->XferLength = *DataLength;
        ((XHCI_DATA_XFR_TRB*)Trb)->DataBuffer = (UINT64)(UINTN)Data;
        ((XHCI_DATA_XFR_TRB*)Trb)->CycleBit = XfrRing->CycleBit;
    }

    // Status TRB
    Trb = XhciAdvanceEnqueuePtr(XfrRing);
    Trb->TrbType = XhciTStatusStage;
    ((XHCI_STATUS_XFR_TRB*)Trb)->Ioc = 1;
    if ((Rq & BIT7) == 0) {
        ((XHCI_STATUS_XFR_TRB*)Trb)->Dir = 1;   // Status is IN
    }

    ((XHCI_STATUS_XFR_TRB*)Trb)->CycleBit = XfrRing->CycleBit;

    // Ring the doorbell and see Event Ring update
    Status = XhciRingDoorbell(PeiServices, Usb3Hc, SlotId, 1);
    
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = XhciWaitForEvent(
                PeiServices, Usb3Hc, Trb, XhciTTransferEvt,
                &CompletionCode, XHCI_CTL_COMPLETE_TIMEOUT_MS, NULL);

    if (EFI_ERROR(Status)) {
        switch (CompletionCode) {
            case XHCI_TRB_DATABUF_ERROR:
                *TransferResult |= EFI_USB_ERR_BUFFER;
                break;
            case XHCI_TRB_BABBLE_ERROR:
                *TransferResult |= EFI_USB_ERR_BABBLE;
            case XHCI_TRB_TRANSACTION_ERROR:
                XhciClearStalledEp(PeiServices, Usb3Hc, SlotId, 1);
                break;
            case XHCI_TRB_STALL_ERROR:
                *TransferResult |= EFI_USB_ERR_STALL;
                XhciClearStalledEp(PeiServices, Usb3Hc, SlotId, 1);
                break;
            case XHCI_TRB_EXECUTION_TIMEOUT_ERROR:
                *TransferResult |= EFI_USB_ERR_TIMEOUT ;
                XhciClearEndpointState(PeiServices, Usb3Hc, SlotId, 0);
                break;
			default:
                break;
        }
        return Status;
    }

    if (Request->Request == USB_DEV_GET_DESCRIPTOR && *DataLength == 8) {
        // Full speed device requires the update of MaxPacket size
        XhciUpdateEp0MaxPacket(PeiServices, Usb3Hc, SlotId,
            ((EFI_USB_DEVICE_DESCRIPTOR*)Data)->MaxPacketSize0);
    }

    if ((Request->Request == USB_DEV_CLEAR_FEATURE) &&
        (Request->Value == EfiUsbEndpointHalt) && (Request->RequestType == 0x02) &&
        (UINT8)Request->Index != 0) {
        XhciClearEndpointState(PeiServices, Usb3Hc, SlotId, (UINT8)Request->Index);
    }

    return EFI_SUCCESS;
}


/**
    This function intiates a USB bulk transfer and waits on it to 
    complete.

        
    @param PeiServices 
        --  EFI_PEI_SERVICES pointer
    @param This 
        --  PEI_USB_HOST_CONTROLLER_PPI pointer
    @param DeviceAddress 
        --  USB address of the device for which the control 
        transfer is to be issued
    @param EndPointAddress 
        --  Particular endpoint for the device
    @param MaximumPacketLength 
        --  Maximum number of bytes that can be sent to or 
        received from the endpoint in a single data packet
    @param DataBuffer 
        --  Pointer to source or destination buffer
    @param DataLength 
        --  Length of buffer
    @param DataToggle 
        --  Used to update the control/status DataToggle field
        of the Transfer Descriptor
    @param TimeOut 
        --  Not used
    @param TransferResult 
        --  Not used

    @retval 
        EFI_STATUS (Return Value)
        = EFI_SUCCESS on successful completion
        or valid EFI error code

**/

EFI_STATUS
EFIAPI
XhciHcBulkTransfer(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       DeviceAddress,
    IN UINT8                       EndPointAddress,
    IN UINT8                       DeviceSpeed,
    IN UINT16                      MaximumPacketLength,
    IN UINT16                      TransactionTranslator    OPTIONAL,
    IN OUT VOID                    *Data,
    IN OUT UINTN                   *DataLength,
    IN OUT UINT8                   *DataToggle,
    IN UINTN                       TimeOut,
    OUT UINT32                     *TransferResult
)
{
    USB3_CONTROLLER *Usb3Hc = PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS(This);
    XHCI_TRB    *Trb;
    XHCI_TRB    *FirstTrb;
    UINT8       SlotId;
    UINT8       CompletionCode;
    EFI_STATUS  Status;
    TRB_RING    *XfrRing;
    UINT8       Endpoint;
    UINT8       Dci;
    UINT64      DataPointer;
    UINT32      ResidualData;       // Transferred amount return by Transfer Event
    UINT32      TransferredSize;    // Total transfer amount
    UINT32      RingDataSize;       // One TRB ring transfer amount
    UINT32      RemainingXfrSize;
    UINT32      RemainingDataSize;
    UINT32      XfrSize;
    UINT32      XfrTdSize;
	UINT32		TdPktCnt;
	UINT32		TdSize;

    Endpoint = EndPointAddress;
    Dci = (Endpoint & 0xf)* 2;
    if (Endpoint & BIT7) Dci++;

    GetSlotId(Usb3Hc->DeviceMap, &SlotId, DeviceAddress);

    XfrRing = XhciGetXfrRing(Usb3Hc, SlotId, Dci-1);

    // Make a chain of TDs to transfer the requested amount of data. If necessary,
    // make multiple transfers in a loop.

    DataPointer = (UINTN)Data;
    RemainingDataSize = *DataLength;

    *TransferResult = EFI_USB_NOERROR;

    // Two loops are executing the transfer:
    // The inner loop creates a transfer ring of chained TDs, XHCI_BOT_TD_MAXSIZE
    // bytes each. This makes a ring capable of transferring
    // XHCI_BOT_TD_MAXSIZE * (TRBS_PER_SEGMENT-1) bytes.
    // The outter loop repeats the transfer if the requested transfer size exceeds
    // XHCI_BOT_TD_MAXSIZE * (TRBS_PER_SEGMENT-1).

    for (TransferredSize = 0; TransferredSize < *DataLength;) {
        // Calculate the amount of data to transfer in the ring
        RingDataSize = (RemainingDataSize > XHCI_BOT_MAX_XFR_SIZE)?
            XHCI_BOT_MAX_XFR_SIZE : RemainingDataSize;

        RemainingXfrSize = RingDataSize;
		TdPktCnt = RingDataSize / MaximumPacketLength;

        for (Trb = NULL, XfrSize = 0, FirstTrb = 0; XfrSize < RingDataSize;) {
            Trb = XhciAdvanceEnqueuePtr(XfrRing);
            if (FirstTrb == NULL) FirstTrb = Trb;

            Trb->TrbType = XhciTNormal;
            ((XHCI_NORMAL_XFR_TRB*)Trb)->Isp = 1;
            ((XHCI_NORMAL_XFR_TRB*)Trb)->DataBuffer = DataPointer;

            // See if we need a TD chain. Note that we do not need to
            // place the chained TRB into Event Ring, since we will not be
            // looking for it anyway. Set IOC only for the last-in-chain TRB.
            if (RemainingXfrSize > XHCI_BOT_TD_MAXSIZE) {
                XfrTdSize = XHCI_BOT_TD_MAXSIZE;
                ((XHCI_NORMAL_XFR_TRB*)Trb)->Chain = 1;
            } else {
                ((XHCI_NORMAL_XFR_TRB*)Trb)->Ioc = 1;
                XfrTdSize = RemainingXfrSize;
            }
            // Data buffers referenced by Transfer TRBs shall not span 64KB boundaries. 
            // If a physical data buffer spans a 64KB boundary, software shall chain 
            // multiple TRBs to describe the buffer.
            if (XfrTdSize > (UINT32)(0x10000 - (DataPointer & (0x10000 - 1)))) {
                XfrTdSize = (UINT32)(0x10000 - (DataPointer & (0x10000 - 1)));
                ((XHCI_NORMAL_XFR_TRB*)Trb)->Chain = 1;
                ((XHCI_NORMAL_XFR_TRB*)Trb)->Ioc = 0;
            }

            ((XHCI_NORMAL_XFR_TRB*)Trb)->XferLength = XfrTdSize;

            XfrSize += XfrTdSize;
            DataPointer += XfrTdSize;
            RemainingXfrSize -= XfrTdSize;
            
            if(Usb3Hc->CapRegs.HciVersion >= 0x100) {
                TdSize = 0;
                if (RemainingXfrSize != 0) {
                    TdSize = RemainingXfrSize/MaximumPacketLength;
                    if (RemainingXfrSize % MaximumPacketLength) {
                        TdSize++;
                    }
                    TdSize = (TdSize > 31)? 31 : TdSize;
                }
            } else {
                TdSize = RemainingXfrSize + XfrTdSize;
                TdSize = (TdSize < 32768)? (TdSize >> 10) : 31;
            }
            
            ((XHCI_NORMAL_XFR_TRB*)Trb)->TdSize = TdSize;
            if (Trb != FirstTrb) {
                ((XHCI_NORMAL_XFR_TRB*)Trb)->CycleBit = XfrRing->CycleBit;
            }
            
        }

        // If transfer ring crossed Link TRB, set its Chain flag
        if (Trb < FirstTrb) {
            ((XHCI_NORMAL_XFR_TRB*)XfrRing->LastTrb)->Chain = 1;
            
        }

        ((XHCI_NORMAL_XFR_TRB*)FirstTrb)->CycleBit = XfrRing->CycleBit;
        if (Trb < FirstTrb) {
            ((XHCI_NORMAL_XFR_TRB*)FirstTrb)->CycleBit ^= 1;
        }

        // Ring the door bell and see Event Ring update
        Status = XhciRingDoorbell(PeiServices, Usb3Hc, SlotId, Dci);

        if (EFI_ERROR(Status)) {
            break;
        }

        Status = XhciWaitForEvent(
                PeiServices, Usb3Hc, Trb, XhciTTransferEvt,
                &CompletionCode, XHCI_BULK_COMPLETE_TIMEOUT_MS, &ResidualData);

        // Clear Link TRB chain flag
        ((XHCI_NORMAL_XFR_TRB*)XfrRing->LastTrb)->Chain = 0;
        if (EFI_ERROR(Status)) {
            switch (CompletionCode) {
                case XHCI_TRB_DATABUF_ERROR:
                    *TransferResult |= EFI_USB_ERR_BUFFER;
                    break;
                case XHCI_TRB_BABBLE_ERROR:
                    *TransferResult |= EFI_USB_ERR_BABBLE;
                case XHCI_TRB_TRANSACTION_ERROR:
                    XhciClearStalledEp(PeiServices, Usb3Hc, SlotId, Dci);
                    break;
                case XHCI_TRB_STALL_ERROR:
                    *TransferResult |= EFI_USB_ERR_STALL;
                    XhciResetEndpoint(PeiServices, Usb3Hc, SlotId, Dci);
                    break;
                case XHCI_TRB_EXECUTION_TIMEOUT_ERROR:
                    *TransferResult |= EFI_USB_ERR_TIMEOUT ;
                    XhciClearEndpointState(PeiServices, Usb3Hc, SlotId, Endpoint);
                    break;
                default:
                    break;
            }
        }
        TransferredSize += (RingDataSize - ResidualData);
        if (ResidualData != 0) break;   // Short packet detected, no more transfers
        RemainingDataSize -= RingDataSize;
    }

    *DataLength = TransferredSize;

    return Status;

}


/**
    This function processes a USB interrupt transfer within a given timeout.

    @retval 
        EFI_INVALID_PARAMETER   Invalid data, datalength, datatoggle
        EFI_NOT_FOUND   No endpoint found that matches the given parameters.
        EFI_TIMEOUT     No transfer during the given timeout.
        EFI_SUCCESS     Successful transfer, data is copied to the given data buffer.

**/

EFI_STATUS
EFIAPI
XhciHcSyncInterruptTransfer(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI  *This,
    IN     UINT8                    DeviceAddress,
    IN     UINT8                    EndPointAddress,
    IN     UINT8                    DeviceSpeed,
    IN     UINT8                    MaximumPacketLength,
    IN UINT16                       TransactionTranslator,
    IN OUT VOID                     *Data,
    IN OUT UINTN                    *DataLength,
    IN OUT UINT8                    *DataToggle,
    IN     UINTN                    TimeOut,
    OUT    UINT32                   *TransferResult
)
{
    USB3_CONTROLLER *Usb3Hc = PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS(This);
	UINT8		SlotId;
    UINT8       Dci;
    UINT8       *Src;
    UINT8       *Dst;
    UINTN       Count;
    EFI_STATUS  Status;

    if (Data == NULL || DataLength == NULL)
    {
        ASSERT(FALSE);
        return EFI_INVALID_PARAMETER;
    }

	GetSlotId(Usb3Hc->DeviceMap, &SlotId, DeviceAddress);
    Dci = (EndPointAddress & 0xf)* 2;
    if (EndPointAddress & BIT7) Dci++;

    if (Dci != Usb3Hc->HidDci || SlotId != Usb3Hc->HidSlotId)
    {
        return EFI_NOT_FOUND;
    }

    for (Count = 0, Status = EFI_NOT_READY; Count < TimeOut; Count++)
    {
        Status = XhciProcessInterrupt(PeiServices, Usb3Hc);

        if (Status == EFI_SUCCESS) break;

        XHCI_FIXED_DELAY_MS(Usb3Hc, 1);  // 1 msec
    }

    if (EFI_ERROR(Status))
    {
        return EFI_TIMEOUT;
    }

    // Get the data and return success
    for (Src = Usb3Hc->HidData,
         Dst = (UINT8*)Data,
         Count = 0; Count < 8; Count++)
    {
        *Dst++ = *Src++;
    }

    return EFI_SUCCESS;
}


/**
    This function returns number of root ports supported by the controller.

**/

EFI_STATUS
EFIAPI
XhciHcGetRootHubPortNumber(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    OUT UINT8                      *PortNumber
)
{
    USB3_CONTROLLER *Usb3Hc = PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS(This);

    *PortNumber = Usb3Hc->CapRegs.HcsParams1.MaxPorts;
    return EFI_SUCCESS;
}


/**
    This function converts XHCI speed definition into the terms
    of PEI_USB_HOST_CONTROLLER_PPI (namely XHCI_DEVSPEED_xyz is converted
    into USB_PORT_STAT_xyz).

**/

VOID
UpdatePortStatusSpeed(
    EFI_PEI_SERVICES    **PeiServices,
    UINT8               Speed,
    UINT16              *PortStatus
)
{
    switch (Speed) {
        case XHCI_DEVSPEED_UNDEFINED:
        case XHCI_DEVSPEED_FULL:
            break;
        case XHCI_DEVSPEED_LOW:
            *PortStatus |= USB_PORT_STAT_LOW_SPEED;
            break;
        case XHCI_DEVSPEED_HIGH:
            *PortStatus |= USB_PORT_STAT_HIGH_SPEED;
            break;
        case XHCI_DEVSPEED_SUPER:
            *PortStatus |= USB_PORT_STAT_SUPER_SPEED;
            break;
        case XHCI_DEVSPEED_SUPER_PLUS:
            *PortStatus |= USB_PORT_STAT_SUPER_SPEED_PLUS;
            break;
        default:
            DEBUG((DEBUG_ERROR, "XHCI ERROR: unknown device speed.\n"));
    }
}


/**
    Host controller API function; returns root hub port status in terms of
    PEI_USB_HOST_CONTROLLER_PPI definition.

**/

EFI_STATUS
EFIAPI
XhciHcGetRootHubPortStatus(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    OUT EFI_USB_PORT_STATUS        *PortStatus
)
{
    USB3_CONTROLLER *Usb3Hc = PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS(This);
    UINT32              i;
    UINT32  PortStsOffset = XHCI_PORTSC_OFFSET + (0x10 * (PortNumber - 1));
    UINT32  XhciPortSts;
    XHCI_PORTSC *PortSts = (XHCI_PORTSC*)(&XhciPortSts);

    // Find the proper MMIO access offset for a given port
    XhciPortSts = XhciReadOpReg(Usb3Hc, PortStsOffset);

    DEBUG((DEBUG_INFO, "XHCI port[%d] status: %08x\n", PortNumber, XhciPortSts));

    for (i = 0; i < PEI_XHCI_PORT_RESET_TIMEOUT_MS; i++) {
        if (PortSts->Field.Pr == 0) {
            break;
        }
		XHCI_FIXED_DELAY_MS(Usb3Hc, 1);  
        XhciPortSts = XhciReadOpReg(Usb3Hc, PortStsOffset);
    }
    
	switch (PortSts->Field.Pls) {
		case XHCI_PORT_LINK_U0:
		case XHCI_PORT_LINK_RXDETECT:
			break;
		case XHCI_PORT_LINK_RECOVERY:
			for (i = 0; i < PEI_XHCI_PORT_LINK_RECOVERY_TIMEOUT_MS; i++) {
				XHCI_FIXED_DELAY_MS(Usb3Hc, 1);  
                XhciPortSts = XhciReadOpReg(Usb3Hc, PortStsOffset);
				if (PortSts->Field.Pls != XHCI_PORT_LINK_RECOVERY) {
					break;
				}
			}
			break;
		case XHCI_PORT_LINK_POLLING:
			if (!XhciIsUsb3Port(Usb3Hc, PortNumber)) {
				break;
			}
			for (i = 0; i < PEI_XHCI_PORT_LINK_POLLING_TIMEOUT_MS; i++) {
				XHCI_FIXED_DELAY_MS(Usb3Hc, 1);  
                XhciPortSts = XhciReadOpReg(Usb3Hc, PortStsOffset);
				if (PortSts->Field.Pls != XHCI_PORT_LINK_POLLING) {
					break;
				}
			}
			if (PortSts->Field.Pls == XHCI_PORT_LINK_U0 || 
				PortSts->Field.Pls == XHCI_PORT_LINK_RXDETECT) {
				break;
			}
			break;
		case XHCI_PORT_LINK_INACTIVE:
			for (i = 0; i < PEI_XHCI_PORT_LINK_INACTIVE_TIMEOUT_MS; i++) {
				XHCI_FIXED_DELAY_MS(Usb3Hc, 1);  
                XhciPortSts = XhciReadOpReg(Usb3Hc, PortStsOffset);
				if (PortSts->Field.Pls != XHCI_PORT_LINK_INACTIVE) {
					break;
				}
			}
            if (PortSts->Field.Pls == XHCI_PORT_LINK_RXDETECT) {
                break;
			}
		case XHCI_PORT_LINK_COMPLIANCE_MODE:
			break;
		default:
			break;
	}

    XhciPortSts = XhciReadOpReg(Usb3Hc, PortStsOffset);
    
    *(UINT32*)PortStatus = 0;

    if (PortSts->Field.Ccs != 0) {
        PortStatus->PortStatus |= USB_PORT_STAT_CONNECTION;
    }
    if (PortSts->Field.Ped != 0) {
        PortStatus->PortStatus |= USB_PORT_STAT_ENABLE;
    }
    if (PortSts->Field.Oca != 0) {
        PortStatus->PortStatus |= USB_PORT_STAT_OVERCURRENT;
    }
    if (PortSts->Field.Pr != 0) {
        PortStatus->PortStatus |= USB_PORT_STAT_RESET;
    }
    if (PortSts->Field.Pp != 0) {
        PortStatus->PortStatus |= USB_PORT_STAT_POWER;
    }
    if (PortSts->Field.Csc != 0) {
        PortStatus->PortChangeStatus |= USB_PORT_STAT_C_CONNECTION;
    }
    if (PortSts->Field.Pec != 0) {
        PortStatus->PortChangeStatus |= USB_PORT_STAT_C_ENABLE;
    }
    if (PortSts->Field.Occ != 0) {
        PortStatus->PortChangeStatus |= USB_PORT_STAT_C_OVERCURRENT;
    }
    if ((PortSts->Field.Prc != 0) ||(PortSts->Field.Wrc != 0))  {
        PortStatus->PortChangeStatus |= USB_PORT_STAT_C_RESET;
        PortStatus->PortChangeStatus |= USB_PORT_STAT_C_CONNECTION;
    }

    UpdatePortStatusSpeed(PeiServices, PortSts->Field.PortSpeed, &PortStatus->PortStatus);

    return EFI_SUCCESS;
}


/**
    Host controller PEI_USB_HOST_CONTROLLER_PPI API function; sets a requested
    feature of a root hub port.

**/

EFI_STATUS
EFIAPI
XhciHcSetRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
)
{
    USB3_CONTROLLER *Usb3Hc = PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS(This);
    UINT32      XhciPortSts;
    UINT32      PortStsOffset = XHCI_PORTSC_OFFSET + (0x10 * (PortNumber - 1));
    
	if (PortNumber > Usb3Hc->CapRegs.HcsParams1.MaxPorts) {
		return EFI_INVALID_PARAMETER;
	}

    XhciPortSts = XhciReadOpReg(Usb3Hc, PortStsOffset);
    
	switch (PortFeature) {
		case EfiUsbPortEnable:
		case EfiUsbPortSuspend:
		    break;

		case EfiUsbPortReset:
            XhciPortSts = (XhciPortSts & XHCI_PCS_PP) | XHCI_PCS_PR;
            XhciWriteOpReg(Usb3Hc, PortStsOffset, XhciPortSts);
		    break;

		case EfiUsbPortPower:
            XhciWriteOpReg(Usb3Hc, PortStsOffset, XHCI_PCS_PP);
		    break; 

		default:
        	return EFI_INVALID_PARAMETER;
    }

    return EFI_SUCCESS;
}


/**
    Host controller PEI_USB_HOST_CONTROLLER_PPI API function; clears a requested
    feature of a root hub port.

**/

EFI_STATUS
EFIAPI
XhciHcClearRootHubPortFeature(
    IN EFI_PEI_SERVICES            **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI *This,
    IN UINT8                       PortNumber,
    IN EFI_USB_PORT_FEATURE        PortFeature
)
{
    USB3_CONTROLLER *Usb3Hc = PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS(This);
    UINT32      XhciPortSts;
    UINT32      PortStsOffset = XHCI_PORTSC_OFFSET + (0x10 * (PortNumber - 1));

    if (PortNumber > Usb3Hc->CapRegs.HcsParams1.MaxPorts) {
        return EFI_INVALID_PARAMETER;
    }

    XhciPortSts = XhciReadOpReg(Usb3Hc, PortStsOffset);

    switch (PortFeature) {
        case EfiUsbPortEnable:
		    if (XhciPortSts & XHCI_PCS_PED) {
                if (XhciIsUsb3Port(Usb3Hc, PortNumber)) {
                    XhciPortSts = (XhciPortSts & XHCI_PCS_PP) | XHCI_PCS_PR;
                } else {
                    XhciPortSts = (XhciPortSts & XHCI_PCS_PP) | XHCI_PCS_PED;
                }
                XhciWriteOpReg(Usb3Hc, PortStsOffset, XhciPortSts);
            }
            break;

		case EfiUsbPortSuspend:
		case EfiUsbPortReset:
		    break;

		case EfiUsbPortPower:
			XhciPortSts = (XhciPortSts & XHCI_PCS_PP) & ~(XHCI_PCS_PP);
            XhciWriteOpReg(Usb3Hc, PortStsOffset, XhciPortSts);
		    break; 

		case EfiUsbPortOwner:
			break;

	    case EfiUsbPortConnectChange:
			XhciPortSts = (XhciPortSts & XHCI_PCS_PP) | XHCI_PCS_CSC | XHCI_PCS_PRC |XHCI_PCS_WRC;
            XhciWriteOpReg(Usb3Hc, PortStsOffset, XhciPortSts);
	        break;

	    case EfiUsbPortEnableChange:
			XhciPortSts = (XhciPortSts & XHCI_PCS_PP) | XHCI_PCS_PEC;
            XhciWriteOpReg(Usb3Hc, PortStsOffset, XhciPortSts);
	        break;

	    case EfiUsbPortSuspendChange:
			break;

	    case EfiUsbPortOverCurrentChange:
			XhciPortSts = (XhciPortSts & XHCI_PCS_PP) | XHCI_PCS_OCC;
            XhciWriteOpReg(Usb3Hc, PortStsOffset, XhciPortSts);
	        break;

	    case EfiUsbPortResetChange:
			XhciPortSts = (XhciPortSts & XHCI_PCS_PP) | XHCI_PCS_PRC |XHCI_PCS_WRC;
            XhciWriteOpReg(Usb3Hc, PortStsOffset, XhciPortSts);
	        break;

	    default:
	        return EFI_INVALID_PARAMETER;
    }

    return EFI_SUCCESS;
}


/**
    This routine resets the Host Controler and its USB bus 
    according to the attributes
    @param EFI_PEI_SERVICES             **PeiServices,
        PEI_USB_HOST_CONTROLLER_PPI  *This,
        UINT16                         Attributes         

         
    @retval EFI_SUCCESS if the Xhci host controller is reset successfully.
    @retval EFI_UNSUPPORTED if unknow attribute is supplied

**/

EFI_STATUS
EFIAPI
XhciHcReset(
  IN EFI_PEI_SERVICES             **PeiServices,
  IN PEI_USB_HOST_CONTROLLER_PPI  *This,
  IN UINT16                       Attributes
)
{    
// APTIOV_SERVER_OVERRIDE_START : Xhci_pei_support to work along with USB3StatusCode_01.1 module
    UINT32                  Index;
// APTIOV_SERVER_OVERRIDE_END : Xhci_pei_support to work along with USB3StatusCode_01.1 module
    USB3_CONTROLLER         *Usb3Hc = 
                                PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS (This);
    UINT32                  DbCapDcctrl;
    EFI_PEI_PCI_CFG_PPI_WIDTH Width;
    UINT64                   Val64;
    UINT8                    PciDevIndex;
// APTIOV_SERVER_OVERRIDE_START : Xhci_pei_support to work along with USB3StatusCode_01.1 module
    XHCI_SLOT_CONTEXT	    *SlotCtx;
    UINT8                   SlotId;
    UINT8                   Dci;
    TRB_RING                *XfrRing;
    XHCI_EP_CONTEXT         *EpCtx;
    UINT16                  EpInfo;
// APTIOV_SERVER_OVERRIDE_END : Xhci_pei_support to work along with USB3StatusCode_01.1 module
 
    switch (Attributes) {
        case EFI_USB_HC_RESET_GLOBAL:
// APTIOV_SERVER_OVERRIDE_START : Xhci_pei_support to work along with USB3StatusCode_01.1 module
            // Disable the device slots before we stop the host controller.
            for (Index = 0; Index <= PEI_XHCI_MAX_SLOTS; Index++) {
                if (Usb3Hc->DcbaaPtr->DevCntxtAddr[Index]) {
                    SlotId = Index + 1;
                    SlotCtx = (XHCI_SLOT_CONTEXT*)XhciGetContextEntry(Usb3Hc, (VOID*)Usb3Hc->DcbaaPtr->DevCntxtAddr[Index], 0);
                    for (Dci = 1; Dci <= SlotCtx->ContextEntries; Dci++) {
                	    EpCtx = (XHCI_EP_CONTEXT*)XhciGetContextEntry(Usb3Hc, (VOID*)Usb3Hc->DcbaaPtr->DevCntxtAddr[Index], Dci);
                        if (EpCtx->TrDequeuePtr != 0) {
                			if (EpCtx->EpState == XHCI_EP_STATE_RUNNING) {
                				EpInfo = (Dci << 8) + SlotId;
                				XhciExecuteCommand(PeiServices, Usb3Hc, XhciTStopEndpointCmd, &EpInfo);
                        	}
                			// Clear transfer rings
                            XfrRing = XhciGetXfrRing(Usb3Hc, SlotId, Dci - 1);
                            ZeroMem((UINT8*)XfrRing->Base, RING_SIZE);
                        }
                    }
                    XhciExecuteCommand(PeiServices, Usb3Hc, XhciTDisableSlotCmd, &SlotId);
                	Usb3Hc->DcbaaPtr->DevCntxtAddr[SlotId - 1] = 0;
                }
            }
// APTIOV_SERVER_OVERRIDE_END : Xhci_pei_support to work along with USB3StatusCode_01.1 module
		
    		// Disable interrupt				
            XhciClearOpReg(Usb3Hc, XHCI_USBCMD_OFFSET, XHCI_CMD_INTE);
            XhciClearHcMem(Usb3Hc, 
                (UINT32)((UINTN)&Usb3Hc->RtRegs->IntRegs->IMan - Usb3Hc->BaseAddress), BIT1);

		    // Clear the Run/Stop bit
            XhciClearOpReg(Usb3Hc, XHCI_USBCMD_OFFSET, XHCI_CMD_RS);
		
		    // The xHC should halt within 16 ms. Section 5.4.1.1
// APTIOV_SERVER_OVERRIDE_START : Xhci_pei_support to work along with USB3StatusCode_01.1 module
            for (Index = 0; Index < PEI_XHCI_HALT_TIMEOUT_MS * 10; Index++) {			
// APTIOV_SERVER_OVERRIDE_END : Xhci_pei_support to work along with USB3StatusCode_01.1 module
                if (XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_HALTED) {
                    break;
                }
                XHCI_FIXED_DELAY_US(Usb3Hc, 100);    // 100us delay 
		    }
		    ASSERT(XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_HALTED);

            if (Usb3Hc->DbCapOffset) {
                XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, Usb3Hc->DbCapOffset + XHCI_DB_CAP_DCCTRL_REG, 1, &DbCapDcctrl);
                // Don't reset xhci controller if it's Debug Capability is enabled.
                if (DbCapDcctrl & XHCI_DB_CAP_DCE) {
                    break;
                }
            }
		    
            // Reset controller
            XhciSetOpReg(Usb3Hc, XHCI_USBCMD_OFFSET, XHCI_CMD_HCRST);

            // Workaround for the issue in some silicons.
            // Add short delay to avoid race condition after write USBCMD.HCRST 
            // for issuing HC Reset and before accessing any MMIO register.
            XHCI_FIXED_DELAY_MS(Usb3Hc, PEI_XHCI_RESET_DELAY_MS);
            
// APTIOV_SERVER_OVERRIDE_START : Xhci_pei_support to work along with USB3StatusCode_01.1 module
            for (Index = 0; Index < PEI_XHCI_RESET_TIMEOUT_MS * 10; Index++) {
// APTIOV_SERVER_OVERRIDE_END : Xhci_pei_support to work along with USB3StatusCode_01.1 module
                if (!(XhciReadOpReg(Usb3Hc, XHCI_USBCMD_OFFSET) & XHCI_CMD_HCRST)) {
                    break;
                }	
                XHCI_FIXED_DELAY_US(Usb3Hc, 100);    // 100us delay
            }
            ASSERT(!(XhciReadOpReg(Usb3Hc, XHCI_USBCMD_OFFSET) & XHCI_CMD_HCRST)); 
            
            Val64 = 0;
            
            //Check flag 'FreeResource' and clear resource
            if (Usb3Hc->FreeResource == TRUE) {

                //Clear PCI_BAR0
                (*PeiServices)->PciCfg->Write(PeiServices,(*PeiServices)->PciCfg,
                    EfiPeiPciCfgWidthUint32,
                    XHCI_PCI_ADDRESS(Usb3Hc->ControllerIndex, PCI_BASE_ADDRESSREG_OFFSET),
                    &Val64
                );

                //Clear PCI_CMD
                (*PeiServices)->PciCfg->Write(PeiServices,(*PeiServices)->PciCfg,
                    EfiPeiPciCfgWidthUint8,
                    XHCI_PCI_ADDRESS(Usb3Hc->ControllerIndex, PCI_COMMAND_OFFSET),
                    &Val64
                );
            }
            
            //If it is the last controller, clear PCI bridge resource.
            if (Usb3Hc->ControllerIndex == gXhciControllerCount - 1) {
                
                for (PciDevIndex = 0; PciDevIndex < gPeiXhciInitPciTableCount; PciDevIndex++) {
                    
                    switch (gPeiXhciInitPciTable[PciDevIndex].Size) {
                            case 8:     Width = EfiPeiPciCfgWidthUint8; break;
                            case 16:    Width = EfiPeiPciCfgWidthUint16; break;
                            case 32:    Width = EfiPeiPciCfgWidthUint32; break;
                            case 64:    Width = EfiPeiPciCfgWidthUint64; break;
                            default:    continue;
                    }
                    
                    (*PeiServices)->PciCfg->Write(PeiServices,(*PeiServices)->PciCfg,
                        Width,
                        EFI_PEI_PCI_CFG_ADDRESS(
                        gPeiXhciInitPciTable[PciDevIndex].Bus, 
                        gPeiXhciInitPciTable[PciDevIndex].Device, 
                        gPeiXhciInitPciTable[PciDevIndex].Function, 
                        gPeiXhciInitPciTable[PciDevIndex].Register),
                        &Val64
                    );
                }
            }
            
            break;                    
        default:
            return EFI_UNSUPPORTED;
    }

//APTIOV_SERVER_OVERRIDE_START: Set HSEE if it is disabled
#if PEI_USB_HOST_SYSTEM_ERRORS_SUPPORT
    if (!(XhciReadOpReg(Usb3Hc, XHCI_USBCMD_OFFSET) & XHCI_CMD_HSEE)) {
        XhciSetOpReg(Usb3Hc, XHCI_USBCMD_OFFSET, XHCI_CMD_HSEE);
    }
#endif
//APTIOV_SERVER_OVERRIDE_END: Set HSEE if it is disabled

    return EFI_SUCCESS;  
}

/**
    MMIO read; depending on 64-bit access availability executes either one
    64-bit read or two 32-bit writes.

**/

UINT64
XhciMmio64Read(
    USB3_CONTROLLER *Usb3Hc,
    UINTN   Address
)
{
    UINT64  Data;
    
    XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, (UINT32)(Address - Usb3Hc->BaseAddress), 2, &Data);

    return Data;
}

/**
    MMIO write; depending on 64-bit access availability executes either one
    64-bit write or two 32-bit writes.

**/

VOID
XhciMmio64Write(
    USB3_CONTROLLER *Usb3Hc,
    UINTN   Address,
    UINT64  Data
)
{
    XhciWriteHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, (UINT32)(Address - Usb3Hc->BaseAddress), 2, &Data);
}


/**
    Transfer ring initialization. There is an option to create a Link TRB in
    the end of the ring.

**/

EFI_STATUS
XhciInitRing (
    IN OUT TRB_RING *Ring,
    IN UINTN    RingBase,
    IN UINT32   RingSize,
    IN BOOLEAN  PlaceLinkTrb
)
{
    XHCI_LINK_TRB   *LinkTrb;

    Ring->Base = (XHCI_TRB*)RingBase;
    Ring->Size = RingSize;
    Ring->LastTrb = Ring->Base + RingSize - 1;
    Ring->CycleBit = 1;
    Ring->QueuePtr = (XHCI_TRB*)RingBase;

    // Initialize ring with zeroes
    {
        UINT8   *p = (UINT8*)RingBase;
        UINTN   i;
        for (i = 0; i < RingSize*sizeof(XHCI_TRB); i++, p++) *p = 0;
    }

    if (PlaceLinkTrb) {
        // Place a Link TRB in the end of the ring pointing to the beginning
        LinkTrb = (XHCI_LINK_TRB*)Ring->LastTrb;
        LinkTrb->NextSegPtr = (UINT64)(UINTN)RingBase;
        LinkTrb->ToggleCycle = 1;
        LinkTrb->TrbType = XhciTLink;
    }

    return EFI_SUCCESS;
}


/**
    This function advances returns the pointer to the current TRB and anvances
    dequeue pointer. If the advance pointer is Link TRB, then it: 1) activates
    Link TRB by updating its cycle bit, 2) updates dequeue pointer to the value
    pointed by Link TRB.

    @param 
        Ring - TRB ring to be updated

    @retval 
        TRB that can be used for command, transfer, etc.

**/

XHCI_TRB*
XhciAdvanceEnqueuePtr(
    TRB_RING    *Ring
)
{
    XHCI_TRB* Trb = Ring->QueuePtr;

    if (Trb->TrbType == XhciTLink) {
        Trb->CycleBit = Ring->CycleBit;
        Ring->CycleBit ^= 1;
        Ring->QueuePtr = Ring->Base;

        Trb = Ring->QueuePtr;
    }
    // Clear the TRB
    {
        UINT32 *p = (UINT32*)Trb;
        UINT8 i = 0;
        for (i=0; i<(sizeof(XHCI_TRB)/sizeof(UINT32)); i++) {
            *p++ = 0;
        }
    }

    Trb->CycleBit = Ring->CycleBit;
    Ring->QueuePtr++;

    return Trb;
}


/**
    This function processes a transfer event and gives control to the device
    specific routines.

**/

EFI_STATUS
XhciProcessXferEvt(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc,
    UINT64              TrbPtr,
    UINT8               SlotId,
    UINT8               Dci
)
{
    TRB_RING    *XfrRing;
    UINT32       DoorbellOffset;

    XHCI_TRB *Trb = (XHCI_TRB*)TrbPtr;
    // ((XHCI_NORMAL_XFR_TRB*)Trb)->DataBuffer should have the data

    if (Usb3Hc->HidDci != Dci || Usb3Hc->HidSlotId != SlotId) {
        return EFI_NOT_READY;
    }

    {
        UINT8 i;
        DEBUG((DEBUG_INFO, "buf: "));
        for (i = 0; i < 8; i++) {
            DEBUG((DEBUG_INFO, "%x", Usb3Hc->HidData[i]));
        }
        DEBUG((DEBUG_INFO, "\n"));
    }


    XfrRing = XhciGetXfrRing(Usb3Hc, SlotId, Dci-1);
    Trb = XhciAdvanceEnqueuePtr(XfrRing);
    ((XHCI_NORMAL_XFR_TRB*)Trb)->TrbType = XhciTNormal;
    ((XHCI_NORMAL_XFR_TRB*)Trb)->DataBuffer = (UINT64)(UINTN)Usb3Hc->HidData;
    ((XHCI_NORMAL_XFR_TRB*)Trb)->XferLength = 8;
	((XHCI_NORMAL_XFR_TRB*)Trb)->Isp = 1;
    ((XHCI_NORMAL_XFR_TRB*)Trb)->Ioc = 1;

    // Ring the door bell
    DoorbellOffset = XhciGetTheDoorbellOffset(Usb3Hc, SlotId);
    XhciWriteHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, DoorbellOffset, 1, &Dci);

    return EFI_SUCCESS;
}


/**
    This is the XHCI controller event handler. It walks through
    the Event Ring and executes the event associated code if needed. Updates
    the Event Ring Data Pointer in the xHC to let it know which events are
    completed.

         
    @retval EFI_NOT_READY Need more Interrupt processing
    @retval EFI_SUCCESS No interrupts pending

**/

EFI_STATUS
XhciProcessInterrupt(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc
)
{
    XHCI_TRB        *Trb;
    XHCI_EVENT_TRB  *EvTrb;
    EFI_STATUS      Status = EFI_NOT_READY;

    if (XhciMmio64Read(Usb3Hc, (UINTN)&Usb3Hc->OpRegs->DcbAap) != 
        (UINT64)(UINTN)Usb3Hc->DcbaaPtr) {
        return Status;
    }

/*
    if (Usb3Hc->OpRegs->UsbSts.Field.Pcd) {
        XHCI_EnumeratePorts(HcStruc);
        Usb3Hc->OpRegs->UsbSts.AllBits = XHCI_STS_PCD;    // Clear PortChangeDetect
    }
*/
	if (XhciReadOpReg(Usb3Hc, XHCI_USBSTS_OFFSET) & XHCI_STS_EVT_INTERRUPT) {
        XhciWriteOpReg(Usb3Hc, XHCI_USBSTS_OFFSET, XHCI_STS_EVT_INTERRUPT);  
        XhciSetHcMem(Usb3Hc, 
            (UINT32)((UINTN)&Usb3Hc->RtRegs->IntRegs[0].IMan - Usb3Hc->BaseAddress), BIT0);
	}
	
    // Check for pending interrupts:
    // check the USBSTS[3] and IMAN [0] to determine if any interrupt generated
    if (Usb3Hc->EvtRing.QueuePtr->CycleBit != Usb3Hc->EvtRing.CycleBit) {
        return Status;
    }

    Status = EFI_SUCCESS;

    // See if there are any TRBs waiting in the event ring
    //for (Count = 0; Count < Usb3Hc->EvtRing.Size; Count++) {
    for (;;) {
        Trb = Usb3Hc->EvtRing.QueuePtr;

        if (Trb->CycleBit != Usb3Hc->EvtRing.CycleBit) break;  // past the last

		if (Usb3Hc->EvtRing.QueuePtr == Usb3Hc->EvtRing.LastTrb) {
			// Reached the end of the ring, wrap around
			Usb3Hc->EvtRing.QueuePtr = Usb3Hc->EvtRing.Base;
			Usb3Hc->EvtRing.CycleBit ^= 1;
		} else {
			Usb3Hc->EvtRing.QueuePtr++;
		}
        // error manager
        if (Trb->CompletionCode == XHCI_TRB_SHORTPACKET) {
            DEBUG((DEBUG_INFO, "PEI_XHCI: short packet detected."));
        }

        if (Trb->CompletionCode == XHCI_TRB_STALL_ERROR) {
            DEBUG((DEBUG_ERROR, "PEI_XHCI: device STALLs."));
        }

        if (Trb->CompletionCode != XHCI_TRB_SUCCESS
                && Trb->CompletionCode != XHCI_TRB_STALL_ERROR
                && Trb->CompletionCode != XHCI_TRB_SHORTPACKET) {
            DEBUG((DEBUG_ERROR, "Trb completion code: %d\n", Trb->CompletionCode));
        }

        // Process TRB pointed by Usb3Hc->EvtRing->QueuePtr
        EvTrb = (XHCI_EVENT_TRB*)Trb;

        switch (Trb->TrbType) {
            case XhciTTransferEvt:
                Status = XhciProcessXferEvt(
                    PeiServices,
                    Usb3Hc,
                    EvTrb->TransferEvt.TrbPtr,
                    EvTrb->TransferEvt.SlotId,
                    EvTrb->TransferEvt.EndpointId);
                break;
            case XhciTCmdCompleteEvt:
                DEBUG((DEBUG_INFO, "CmdCompleteEvt\n"));
                break;
            case XhciTPortStatusChgEvt:
                DEBUG((DEBUG_INFO, "PortStatusChgEvt, port #%d\n", EvTrb->PortStsChgEvt.PortId));
                break;
            case XhciTDoorbellEvt:
                DEBUG((DEBUG_INFO, "DoorbellEvt\n"));
                break;
            case XhciTHostControllerEvt:
                DEBUG((DEBUG_INFO, "HostControllerEvt\n"));
                break;
            case XhciTDevNotificationEvt:
                DEBUG((DEBUG_INFO, "DevNotificationEvt\n"));
                break;
            case XhciTMfIndexWrapEvt:
                DEBUG((DEBUG_INFO, "MfIndexWrapEvt\n"));
                break;
            default:
                DEBUG((DEBUG_ERROR, "UNKNOWN EVENT\n"));
        }
    }
    //ASSERT(Count < Usb3Hc->EvtRing.Size);    // Event ring is full

	// Update ERDP to inform xHC that we have processed another TRB
    XhciMmio64Write(Usb3Hc,
        (UINTN)&Usb3Hc->RtRegs->IntRegs->Erdp, (UINT64)(UINTN)Usb3Hc->EvtRing.QueuePtr | BIT3);

    return  Status;
}


/**
    This function walks through the active TRBs in the event ring and looks for
    the command TRB to be complete. If found, returns SlotId and CompletionCode
    from the completed event TRB. In the end it processes the event ring,
    adjusting its Dequeue Pointer.

**/

EFI_STATUS
XhciWaitForEvent(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc,
    XHCI_TRB            *TrbToCheck,
    TRB_TYPE            EventType,
    UINT8               *CompletionCode,
    UINT16              TimeOutMs,
    VOID                *Data
)
{
    XHCI_TRB    *Trb;
    UINT32      TimeOut;
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       CycleBit;
//	UINT32		TimeOutValue = TimeOutMs << 6;	// *64, 15 us unit
	UINT32		TimeOutValue = TimeOutMs + 1;
    XHCI_NORMAL_XFR_TRB      *ResidualTrb; 

    for (TimeOut = 0; TimeOut < TimeOutValue; TimeOut++) {
        for (Trb = Usb3Hc->EvtRing.QueuePtr,
			CycleBit = Usb3Hc->EvtRing.CycleBit;;) {
            if (Trb->CycleBit != CycleBit) {
                // Command is not complete, break and retry
                break;
            }

            *CompletionCode = Trb->CompletionCode;
			if (Trb->CompletionCode == XHCI_TRB_STALL_ERROR || 
				Trb->CompletionCode == XHCI_TRB_TRANSACTION_ERROR) {
                Status = EFI_DEVICE_ERROR;
                goto DoneWaiting;
            }

            // Active TRB found
            if (Trb->TrbType == EventType) {
                if ((*(UINTN*)&Trb->Param1) == (UINTN)TrbToCheck) {

                    if (Trb->CompletionCode != XHCI_TRB_SUCCESS && Trb->CompletionCode != XHCI_TRB_SHORTPACKET) {
                        DEBUG((DEBUG_ERROR, "TRB Completion Error: %d\n", Trb->CompletionCode));
                    }

                    if (EventType == XhciTCmdCompleteEvt) {
                        *(UINT8*)Data = ((XHCI_CMDCOMPLETE_EVT_TRB*)Trb)->SlotId;
                    }
                    if (EventType == XhciTTransferEvt) {
                        if (Data != NULL) {
                            *(UINT32*)Data = ((XHCI_TRANSFER_EVT_TRB*)Trb)->TransferLength;
                            if (Trb->CompletionCode == XHCI_TRB_SHORTPACKET) {
                                ResidualTrb = (XHCI_NORMAL_XFR_TRB*)(UINTN)((XHCI_TRANSFER_EVT_TRB*)Trb)->TrbPtr;
                                 while (1) {
                                    ResidualTrb->Isp = 0;
                                    ResidualTrb->Ioc = 0;
                                    if (ResidualTrb->Chain != 1) {
                                        break;
                                    }
                                    ResidualTrb++;
                                    if (ResidualTrb->TrbType == XhciTLink) {
                                        ResidualTrb = (XHCI_NORMAL_XFR_TRB*)(UINTN)((XHCI_LINK_TRB*)ResidualTrb)->NextSegPtr;
                                    }
                                    *(UINT32*)Data += ResidualTrb->XferLength;
                                }
                             }
                        }
                    }

                    Status = (Trb->CompletionCode == XHCI_TRB_SUCCESS ||
                        Trb->CompletionCode == XHCI_TRB_SHORTPACKET)? EFI_SUCCESS:EFI_DEVICE_ERROR;
                    goto DoneWaiting;
                }
            }
            // Advance TRB pointer
            if (Trb == Usb3Hc->EvtRing.LastTrb) {
                Trb = Usb3Hc->EvtRing.Base;
                CycleBit ^= 1;
            } else {
                Trb++;
            }
            if (Trb == Usb3Hc->EvtRing.QueuePtr) {
                // Event ring is full, return error
                DEBUG((DEBUG_ERROR, "PEI_XHCI: Event Ring is full...\n"));
                ASSERT(FALSE);
                *CompletionCode = XHCI_TRB_EVENTRINGFULL_ERROR;
                Status = EFI_DEVICE_ERROR;
                break;
            }
        }
//        XHCI_FIXED_DELAY_15MCS(Usb3Hc, 1);    // 15 us out of TimeOutMs
        XHCI_FIXED_DELAY_MS(Usb3Hc, 1);    // 1 ms out of TimeOutMs
    }

    DEBUG((DEBUG_ERROR, "PEI_XHCI: execution time-out.\n"));

    *CompletionCode = XHCI_TRB_EXECUTION_TIMEOUT_ERROR;
    Status = EFI_DEVICE_ERROR;

DoneWaiting:
    XhciProcessInterrupt(PeiServices, Usb3Hc);

    return Status;
}


/**
    This function places a given command in the Command Ring, rings HC doorbell,
    and waits for the command completion.

         
        EFI_DEVICE_ERROR on execution failure, otherwise EFI_SUCCESS
    @retval Params pointer to the command specific data.

    @note  
  Caller is responsible for a data placeholder.

**/

EFI_STATUS
XhciExecuteCommand(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER     *Usb3Hc,
    TRB_TYPE            Cmd,
    VOID                *Params
)
{
    UINT32      Doorbell;
    UINT8       CompletionCode;
    UINT8       SlotId;
    EFI_STATUS  Status;
    XHCI_TRB    *Trb = XhciAdvanceEnqueuePtr(&Usb3Hc->CmdRing);

    Trb->TrbType = Cmd; // Set TRB type

    // Fill in the command TRB fields
    switch (Cmd) {
        case XhciTAddressDeviceCmd:
            ((XHCI_ADDRESSDEV_CMD_TRB*)Trb)->InpCtxAddress = (UINT64)(UINTN)Usb3Hc->InputContext;
            ((XHCI_ADDRESSDEV_CMD_TRB*)Trb)->SlotId = *((UINT8*)Params);
            ((XHCI_ADDRESSDEV_CMD_TRB*)Trb)->Bsr = *((UINT8*)Params + 1);
            break;
        case XhciTEvaluateContextCmd:
        case XhciTConfigureEndpointCmd:
            ((XHCI_CONFIGURE_EP_CMD_TRB*)Trb)->InpCtxAddress = (UINT64)(UINTN)Usb3Hc->InputContext;
            ((XHCI_CONFIGURE_EP_CMD_TRB*)Trb)->SlotId = *((UINT8*)Params);
            ((XHCI_CONFIGURE_EP_CMD_TRB*)Trb)->Dc = 0;
            break;
        case XhciTResetEndpointCmd:
			((XHCI_RESET_EP_CMD_TRB*)Trb)->Tsp = 0;
            ((XHCI_RESET_EP_CMD_TRB*)Trb)->SlotId = *((UINT8*)Params);
            ((XHCI_RESET_EP_CMD_TRB*)Trb)->EndpointId = *((UINT8*)Params+1);
            break;
        case XhciTSetTRDequeuePointerCmd:
            ((XHCI_SET_TRPTR_CMD_TRB*)Trb)->TrPointer = ((XHCI_SET_TRPTR_CMD_TRB*)Params)->TrPointer;
            ((XHCI_SET_TRPTR_CMD_TRB*)Trb)->EndpointId = ((XHCI_SET_TRPTR_CMD_TRB*)Params)->EndpointId;
            ((XHCI_SET_TRPTR_CMD_TRB*)Trb)->SlotId = ((XHCI_SET_TRPTR_CMD_TRB*)Params)->SlotId;
            break;
        case XhciTDisableSlotCmd:
            ((XHCI_DISABLESLOT_CMD_TRB*)Trb)->SlotId = *((UINT8*)Params);
            break;
// APTIOV_SERVER_OVERRIDE_START : Xhci_pei_support to work along with USB3StatusCode_01.1 module
        case XhciTStopEndpointCmd:
            ((XHCI_STOP_EP_CMD_TRB*)Trb)->SlotId = *((UINT8*)Params);
            ((XHCI_STOP_EP_CMD_TRB*)Trb)->EndpointId = *((UINT8*)Params+1);
            break;
// APTIOV_SERVER_OVERRIDE_END : Xhci_pei_support to work along with USB3StatusCode_01.1 module
    }

    Trb->CycleBit = Usb3Hc->CmdRing.CycleBit;

    // Ring the door bell and see Event Ring update
    Doorbell = 0;  // HC doorbell is #0
    XhciWriteHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, Usb3Hc->CapRegs.DbOff, 1, &Doorbell);
   
    Status = XhciWaitForEvent(
                PeiServices, Usb3Hc, Trb, XhciTCmdCompleteEvt,
                &CompletionCode, XHCI_CMD_COMPLETE_TIMEOUT_MS, &SlotId);

    if (Status == EFI_DEVICE_ERROR) {
        DEBUG((DEBUG_ERROR, "XHCI command[%d] completion error code: %d\n", Cmd, CompletionCode));
        //ASSERT(Status != EFI_DEVICE_ERROR);
        return Status;
    }

    switch (Cmd) {
        case XhciTEnableSlotCmd:
            DEBUG((DEBUG_INFO, "PEI_XHCI: Enable Slot command complete, SlotID %d\n", SlotId));
            *((UINT8*)Params) = SlotId;
            break;
        case XhciTEvaluateContextCmd:
            DEBUG((DEBUG_INFO, "PEI_XHCI: Evaluate Context command complete.\n"));
            break;
        case XhciTConfigureEndpointCmd:
            DEBUG((DEBUG_INFO, "PEI_XHCI: Configure Endpoint command complete.\n"));
            break;
        case XhciTResetEndpointCmd:
            DEBUG((DEBUG_INFO, "PEI_XHCI: Reset Endpoint command complete (slot#%x dci#%x).\n",
                *((UINT8*)Params), *((UINT8*)Params+1)));
            // Xhci speci 1.1 4.6.8 Reset Endpoint
            // Software shall be responsible for timing the Reset "recovery interval" required by USB.
            XHCI_FIXED_DELAY_MS(Usb3Hc, PEI_XHCI_RESET_EP_DELAY_MS);
            break;
        case XhciTSetTRDequeuePointerCmd:
            DEBUG((DEBUG_INFO, "PEI_XHCI: Set TR pointer command complete.\n"));
            break;
        case XhciTDisableSlotCmd:
            DEBUG((DEBUG_INFO, "PEI_XHCI: DisableSlot command complete.\n"));
            break;
// APTIOV_SERVER_OVERRIDE_START : Xhci_pei_support to work along with USB3StatusCode_01.1 module
        case XhciTStopEndpointCmd:
            DEBUG((DEBUG_INFO, "XHCI: Stop Endpoint command complete (slot#%x dci#%x).\n",
                *((UINT8*)Params), *((UINT8*)Params+1)));
            break;
// APTIOV_SERVER_OVERRIDE_END : Xhci_pei_support to work along with USB3StatusCode_01.1 module
    }

    return EFI_SUCCESS;
}


/**
    This function initializes transfer ring of given endpoint

    @retval 
        Pointer to the transfer ring

**/

TRB_RING*
XhciInitXfrRing(
    USB3_CONTROLLER* Usb3Hc,
    UINT8   Slot,
    UINT8   Ep
)
{
    TRB_RING    *XfrRing = Usb3Hc->XfrRings + (Slot-1)*32 + Ep;
    UINTN       Base = Usb3Hc->XfrTrbs + ((Slot-1)*32+Ep)*RING_SIZE;

    XhciInitRing(XfrRing, Base, TRBS_PER_SEGMENT, TRUE);

    return XfrRing;
}


/**
    This routine calculates the Interval field to be used in device's endpoint
    context. Interval is calculated using the following rules (Section 6.2.3.6):

    For SuperSpeed bulk and control endpoints, the Interval field shall not be
    used by the xHC. For all other endpoint types and speeds, system software
    shall translate the bInterval field in the USB Endpoint Descriptor to the
    appropriate value for this field.

    For high-speed and SuperSpeed Interrupt and Isoch endpoints the bInterval
    field the Endpoint Descriptor is computed as 125æs * 2^(bInterval-1), where
    bInterval = 1 to 16, therefore Interval = bInterval - 1.

    For low-speed Interrupt and full-speed Interrupt and Isoch endpoints the
    bInterval field declared by a Full or Low-speed device is computed as
    bInterval * 1ms., where bInterval = 1 to 255.

    For Full- and Low-speed devices software shall round the value of Endpoint
    Context Interval field down to the nearest base 2 multiple of bInterval * 8.

    @param 
        EpType      Endpoint type, see XHCI_EP_CONTEXT.DW1.EpType field definitions
        Speed       Endpoint speed, 1..4 for XHCI_DEVSPEED_FULL, _LOW, _HIGH, _SUPER
        Interval    Poll interval value from endpoint descriptor

    @retval 
        Interval value to be written to the endpoint context

**/

UINT8
XhciTranslateInterval(
    UINT8   EpType,
    UINT8   Speed,
    UINT8   Interval
)
{
    UINT8  TempData;
    UINT8  BitCount;

    if (EpType == XHCI_EPTYPE_CTL || 
        EpType == XHCI_EPTYPE_BULK_OUT || 
        EpType == XHCI_EPTYPE_BULK_IN) {

        if (Speed == XHCI_DEVSPEED_HIGH) {
            return Interval;
        } else {
            return 0;   // Interval field will not be used for LS, FS and SS
        }
    }

    // Control and Bulk end points are processed; translate intervals for Isoc and Interrupt
    // end points

    // Translate SS and HS end points
    if ((Speed == XHCI_DEVSPEED_SUPER_PLUS) || 
        (Speed == XHCI_DEVSPEED_SUPER) || 
        (Speed == XHCI_DEVSPEED_HIGH)) {
        return (Interval - 1);
    }

    for (TempData = Interval, BitCount = 0; TempData != 0; BitCount++) {
        TempData >>= 1;
    }
    return (BitCount + 2);  // return value, where Interval = 0.125*2^value
}


/**
    This function parses the device descriptor data and enables the endpoints
    by 1)assigning the Transfer TRB and 2)executing ConfigureEndpoint command
    for the slot. Section 4.3.5.

    @param 
        Desc    - Device Configuration Descriptor data pointer

    @retval 
        EFI_DEVICE_ERROR on error, EFI_SUCCESS on success

    @note  
  1) This call is executed before SET_CONFIGURATION control transfer
  2) Device slot is addressed by gSlotBeingConfigured
  3) EP0 information is valid in the Device

**/

EFI_STATUS
EFIAPI
XhciEnableEndpoints(
    IN EFI_PEI_SERVICES                 **PeiServices,
    IN PEI_USB_HOST_CONTROLLER_PPI      *This,
    IN UINT8                            DeviceAddress,
    IN UINT8                            *Desc
)
{
    UINT16          TotalLength;
    UINT16          CurPos;
    UINT8           Dci;
	EFI_USB_INTERFACE_DESCRIPTOR    *IntrfDesc;
    EFI_USB_ENDPOINT_DESCRIPTOR     *EpDesc;
    TRB_RING        *XfrRing;
    UINT8           EpType;
    UINT8           Status;
	UINT8			IsHub;
	UINT8			DevSpeed;
	XHCI_INPUT_CONTROL_CONTEXT	*CtlCtx;
	XHCI_SLOT_CONTEXT			*SlotCtx;
	XHCI_EP_CONTEXT 			*EpCtx;

    USB3_CONTROLLER *Usb3Hc = PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS(This);
    UINT8 SlotId;
    UINT8 *DevCtx;

    if (DeviceAddress == 0) {
        return EFI_INVALID_PARAMETER;
    }

    Status = GetSlotId(Usb3Hc->DeviceMap, &SlotId, DeviceAddress);
    
    if (EFI_ERROR(Status)) {
        return EFI_DEVICE_ERROR;
    }

    if (SlotId == 0) {
        return EFI_DEVICE_ERROR;
    }

    DevCtx = (UINT8*)XhciGetDeviceContext(Usb3Hc, SlotId);

    if (((EFI_USB_CONFIG_DESCRIPTOR*)Desc)->DescriptorType != USB_DT_CONFIG) {
        return EFI_DEVICE_ERROR;
    }

	SlotCtx = (XHCI_SLOT_CONTEXT*)XhciGetContextEntry(Usb3Hc, DevCtx, 0);
	DevSpeed = SlotCtx->Speed;

    // Note (From 4.6.6): The Add Context flag A1 and Drop Context flags D0 and D1
    // of the Input Control Context (in the Input Context) shall be cleared to 0.
    // Endpoint 0 Context does not apply to the Configure Endpoint Command and
    // shall be ignored by the xHC. A0 shall be set to 1.

    // Note (From 6.2.2.2): If Hub = 1 and Speed = High-Speed (3), then the
    // TT Think Time and Multi-TT (MTT) fields shall be initialized.
    // If Hub = 1, then the Number of Ports field shall be initialized, else
    // Number of Ports = 0.

    // Prepare input context for EvaluateContext comand
    ZeroMem((UINT8*)Usb3Hc->InputContext, XHCI_INPUT_CONTEXT_ENTRIES * Usb3Hc->ContextSize);

    CtlCtx = (XHCI_INPUT_CONTROL_CONTEXT*)XhciGetContextEntry(Usb3Hc, (UINT8*)Usb3Hc->InputContext, 0);
    CtlCtx->AddContextFlags = BIT0;    // EP0

	SlotCtx = (XHCI_SLOT_CONTEXT*)XhciGetContextEntry(Usb3Hc, (UINT8*)Usb3Hc->InputContext, 1);

    // Collect the endpoint information and update the Device Input Context
    TotalLength = ((EFI_USB_CONFIG_DESCRIPTOR*)Desc)->TotalLength;

    if (TotalLength > (MAX_CONTROL_DATA_SIZE - 1)) {
        TotalLength = MAX_CONTROL_DATA_SIZE - 1;
    }

    for (CurPos = 0; CurPos < TotalLength; CurPos += EpDesc->Length) {
		EpDesc = (EFI_USB_ENDPOINT_DESCRIPTOR*)(IntrfDesc = (EFI_USB_INTERFACE_DESCRIPTOR*)(Desc + CurPos));
	
		if (IntrfDesc->DescriptorType == USB_DT_INTERFACE) {
			IsHub = IntrfDesc->InterfaceClass == BASE_CLASS_HUB;
			continue;
		}
	
        if (EpDesc->DescriptorType != USB_DT_ENDPOINT) continue;

        // Found Endpoint, fill up the information in the InputContext

        // Calculate Device Context Index (DCI), Section 4.5.1.
        // 1) For Isoch, Interrupt, or Bulk type endpoints the DCI is calculated
        // from the Endpoint Number and Direction with the following formula:
        //  DCI = (Endpoint Number * 2) + Direction, where Direction = 0 for OUT
        // endpoints and 1 for IN endpoints.
        // 2) For Control type endpoints:
        //  DCI = (Endpoint Number * 2) + 1
        //
        // Also calculate XHCI EP type out of EpDesc->bEndpointFlags

        if ((EpDesc->Attributes & EP_DESC_FLAG_TYPE_BITS) == EP_DESC_FLAG_TYPE_CONT) {
            Dci = (EpDesc->EndpointAddress & 0xf) * 2 + 1;
            EpType = XHCI_EPTYPE_CTL;
        } else {
            // Isoc, Bulk or Interrupt endpoint
            Dci = (EpDesc->EndpointAddress & 0xf) * 2;
            EpType = EpDesc->Attributes & EP_DESC_FLAG_TYPE_BITS;   // 1, 2, or 3

            if (EpDesc->EndpointAddress & BIT7) {
                Dci++;          // IN
                EpType += 4;    // 5, 6, or 7
            }
        }

        // Update ContextEntries in the Slot context
        if (Dci > SlotCtx->ContextEntries) {
            SlotCtx->ContextEntries = Dci;
        }

        EpCtx = (XHCI_EP_CONTEXT*)XhciGetContextEntry(Usb3Hc, (UINT8*)Usb3Hc->InputContext, Dci + 1);

        EpCtx->EpType = EpType;
        // Only reserve bits 10..0
        EpCtx->MaxPacketSize = EpDesc->MaxPacketSize & 0x7ff;
        EpCtx->ErrorCount = 3;

        // Set Interval 
        EpCtx->Interval = XhciTranslateInterval(EpType, DevSpeed, EpDesc->Interval);

        XfrRing = XhciInitXfrRing(Usb3Hc, SlotId, Dci - 1);
        EpCtx->TrDequeuePtr = (UINT64)(UINTN)XfrRing->Base + 1;

        CtlCtx->AddContextFlags |= (1 << Dci);
    }

    // For a HUB update NumberOfPorts and TTT fields in the Slot context. For that get hub descriptor
    // and use bNbrPorts and TT Think time fields (11.23.2.1 of USB2 specification)
    // Notes:
    //  - Slot.Hub field is already updated
    //  - Do not set NumberOfPorts and TTT fields for 0.95 controllers

	if (IsHub) {

		EFI_STATUS	Status;
        EFI_USB_HUB_DESCRIPTOR  HubDesc;
        UINT8   Speed;
        EFI_USB_DEVICE_REQUEST  DevReq;
        UINT32  Timeout;
        UINTN   DataLength = sizeof(EFI_USB_HUB_DESCRIPTOR);
        UINT32  TransferResult;

        //
        // Fill Device request packet
        //
        ZeroMem((VOID*)&DevReq, sizeof(EFI_USB_DEVICE_REQUEST));
        DevReq.RequestType = USB_RT_HUB | 0x80;
        DevReq.Request = USB_DEV_GET_DESCRIPTOR;
        if ((DevSpeed == XHCI_DEVSPEED_SUPER) || (DevSpeed == XHCI_DEVSPEED_SUPER_PLUS)) {
            DevReq.Value = USB_DT_SS_HUB << 8;
        } else {
            DevReq.Value = USB_DT_HUB << 8;
        }
        DevReq.Index = 0;
        DevReq.Length = sizeof(EFI_USB_HUB_DESCRIPTOR);
    
        Timeout = 3000;

        switch (DevSpeed) {
            case XHCI_DEVSPEED_HIGH: 
                Speed = USB_HIGH_SPEED_DEVICE;
                break;
            case XHCI_DEVSPEED_LOW:
                Speed = USB_SLOW_SPEED_DEVICE;
                break;
            case XHCI_DEVSPEED_FULL:
                Speed = USB_FULL_SPEED_DEVICE;
                break;
            case XHCI_DEVSPEED_SUPER:
                Speed = USB_SUPER_SPEED_DEVICE;
                break;
            case XHCI_DEVSPEED_SUPER_PLUS:
                Speed = USB_SUPER_SPEED_PLUS_DEVICE;
                break;
        }

        EpCtx = (XHCI_EP_CONTEXT*)XhciGetContextEntry(Usb3Hc, DevCtx, 1);

        Status = XhciHcControlTransfer(PeiServices, This,
            0,  // Current address
            Speed,
            EpCtx->MaxPacketSize,
            0,  // Transaction translator
            &DevReq,
            EfiUsbDataIn,
            &HubDesc,
            &DataLength,
            Timeout,
            &TransferResult);

		if (!EFI_ERROR(Status)) {
			SlotCtx->Hub = 1;
			SlotCtx->PortsNum = HubDesc.NbrPorts;
		
			if (DevSpeed == XHCI_DEVSPEED_HIGH) {
				SlotCtx->TThinkTime = (HubDesc.HubCharacteristics >> 5) & 0x3;
			}
		}
	}

// check route string here
    // Input context is updated with the endpoint information. Execute ConfigureEndpoint command.
    Status = XhciExecuteCommand(PeiServices, Usb3Hc, XhciTConfigureEndpointCmd, &SlotId);
    //ASSERT(Status == EFI_SUCCESS);

    return Status;
}


/**
    This function starts the periodic schedule for the interrupt endpoint-based
    devices (keyboard).

**/

EFI_STATUS
EFIAPI
XhciHcActivatePolling(
    IN EFI_PEI_SERVICES **PeiServices,
    IN VOID *UsbDev
)
{
    PEI_USB_DEVICE  *PeiUsbDev = (PEI_USB_DEVICE*)UsbDev;
    USB3_CONTROLLER *Usb3Hc = PEI_RECOVERY_USB_XHCI_DEV_FROM_THIS(PeiUsbDev->UsbHcPpi);
    UINT8 Dci = (PeiUsbDev->IntEndpoint & 0xf) * 2 + 1;
    UINT32  DoorbellOffset;
    TRB_RING    *XfrRing;
    XHCI_TRB    *Trb;

 /*
    static XhciPollingStarted = FALSE;

    if (XhciPollingStarted) return EFI_ALREADY_STARTED;

    XhciPollingStarted = TRUE;
*/
    
    XfrRing = XhciGetXfrRing(Usb3Hc, PeiUsbDev->SlotId, Dci-1);

    Trb = XhciAdvanceEnqueuePtr(XfrRing);
    Trb->TrbType = XhciTNormal;
    ((XHCI_NORMAL_XFR_TRB*)Trb)->DataBuffer = (UINT64)(UINTN)Usb3Hc->HidData;
    ((XHCI_NORMAL_XFR_TRB*)Trb)->XferLength = 8;
	((XHCI_NORMAL_XFR_TRB*)Trb)->Isp = 1;
    ((XHCI_NORMAL_XFR_TRB*)Trb)->Ioc = 1;

    Usb3Hc->HidDci = Dci;
    Usb3Hc->HidSlotId = PeiUsbDev->SlotId;

    // Ring the door bell
    DoorbellOffset = XhciGetTheDoorbellOffset(Usb3Hc, PeiUsbDev->SlotId);
    XhciWriteHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, DoorbellOffset, 1, &Dci);

    return EFI_SUCCESS;
}


/**
    This routine calculates the address of the address ring of a particular
    Slot/Endpoint.

    @retval 
        Pointer to the transfer ring

**/

TRB_RING*
XhciGetXfrRing(
    USB3_CONTROLLER* Usb3Hc,
    UINT8   Slot,
    UINT8   Ep
)
{
    return Usb3Hc->XfrRings + (Slot-1)*32 + Ep;
}


/**
    This function calculates and returns the offset to a doorbell for a
    given Slot.

**/

UINT32
XhciGetTheDoorbellOffset(
    USB3_CONTROLLER    		*Usb3Hc,
    UINT8                   SlotId
)
{
    return (UINT32)(Usb3Hc->CapRegs.DbOff + sizeof(UINT32) * SlotId);
}

/**
    This function calculates and returns the pointer to a device context for
    a given Slot.

**/

VOID*
XhciGetDeviceContext(
	USB3_CONTROLLER	*Usb3Hc,
	UINT8			SlotId
)
{
	UINT32 DevCtxSize = XHCI_DEVICE_CONTEXT_ENTRIES * Usb3Hc->ContextSize;
	return (VOID*)((UINTN)Usb3Hc->DeviceContext + (SlotId - 1) * DevCtxSize);
}

/**
    This function calculates and returns the pointer to a context entry for
    a given index.

**/

VOID*
XhciGetContextEntry(
	USB3_CONTROLLER	*Usb3Hc,
	VOID			*Context,
	UINT8			Index
)
{
	return (VOID*)((UINTN)Context + Index * Usb3Hc->ContextSize);
}

/**

**/

BOOLEAN
XhciIsUsb3Port(
    USB3_CONTROLLER        *Usb3Hc,
    UINT8                       Port
)
{
	if ((Usb3Hc->Usb3Protocol.PortCount != 0) && (Port >= Usb3Hc->Usb3Protocol.PortOffset) &&
		(Port < Usb3Hc->Usb3Protocol.PortOffset + Usb3Hc->Usb3Protocol.PortCount)) {
		return TRUE;
	}

	if ((Usb3Hc->Usb31Protocol.PortCount != 0) && (Port >= Usb3Hc->Usb31Protocol.PortOffset) &&
		(Port < Usb3Hc->Usb31Protocol.PortOffset + Usb3Hc->Usb31Protocol.PortCount)) {
		return TRUE;
	}
    
	return FALSE;
}
                                        
/**

**/

VOID
XhciResetUsb2Port(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER *Usb3Hc
)
{
    UINT8	    Count;
    UINT8       PortNumber;
    UINT32      i;
    UINT32      PortStsOffset;
    UINT32      XhciPortSts;
    XHCI_PORTSC *PortSts = (XHCI_PORTSC*)(&XhciPortSts);
    
    if (Usb3Hc->Usb2Protocol.PortCount) {
		for(Count = 0; Count < Usb3Hc->Usb2Protocol.PortCount; Count++) {
			PortNumber = Count + Usb3Hc->Usb2Protocol.PortOffset;
            PortStsOffset = XHCI_PORTSC_OFFSET + (0x10 * (PortNumber - 1));
            XhciPortSts = XhciReadOpReg(Usb3Hc, PortStsOffset);
            if (PortSts->Field.Ccs) {
                if (!(PortSts->Field.Ped)) {
                    XhciWriteOpReg(Usb3Hc, PortStsOffset, XHCI_PCS_PR | XHCI_PCS_PP);
                    XhciPortSts = XhciReadOpReg(Usb3Hc, PortStsOffset);
                    for (i = 0; i < PEI_XHCI_RESET_USB2_PORT_TIMEOUT_MS * 10; i++) {
			            if (PortSts->Field.Prc) {
                            break;
                        }
                        XHCI_FIXED_DELAY_US(Usb3Hc, 100);
                        XhciPortSts = XhciReadOpReg(Usb3Hc, PortStsOffset);
		            }
                    XhciWriteOpReg(Usb3Hc, PortStsOffset, XHCI_PCS_WRC | XHCI_PCS_PRC | XHCI_PCS_PP);
                }
            }
		}
	}
    
 }
 
/**

**/

EFI_STATUS
XhciExtCapParser(
    EFI_PEI_SERVICES    **PeiServices,
    USB3_CONTROLLER *Usb3Hc
)
{
    UINT32                  CurPtrOffset;
    UINT32                  XhciLegSup;
    XHCI_EXT_CAP            *XhciExtCap = (XHCI_EXT_CAP*)&XhciLegSup;

    if (Usb3Hc->CapRegs.HccParams1.Xecp == 0) {
        return EFI_SUCCESS;
    }

    // Starts from first capability
    CurPtrOffset = Usb3Hc->CapRegs.HccParams1.Xecp << 2;

    // Traverse all capability structures
    for(;;) {
        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, CurPtrOffset, 1, XhciExtCap);
        switch (XhciExtCap->CapId) {
            case XHCI_EXT_CAP_USB_LEGACY:
                break;
            case XHCI_EXT_CAP_SUPPORTED_PROTOCOL:
                if (((XHCI_EXT_PROTOCOL*)XhciExtCap)->MajorRev == 0x02) {
                    XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, CurPtrOffset, 3, &Usb3Hc->Usb2Protocol);
                    DEBUG((DEBUG_INFO, "XHCI: USB2 Support Protocol %x, PortOffset %x PortCount %x\n", 
                        Usb3Hc->BaseAddress + CurPtrOffset, Usb3Hc->Usb2Protocol.PortOffset, Usb3Hc->Usb2Protocol.PortCount));
                } else if (((XHCI_EXT_PROTOCOL*)XhciExtCap)->MajorRev == 0x03) {
                    if (((XHCI_EXT_PROTOCOL*)XhciExtCap)->MinorRev == 0x00) {
                        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, CurPtrOffset, 3, &Usb3Hc->Usb3Protocol);
                        DEBUG((DEBUG_INFO, "XHCI: USB3.0 Support Protocol %x, PortOffset %x PortCount %x\n", 
                            Usb3Hc->BaseAddress + CurPtrOffset, Usb3Hc->Usb3Protocol.PortOffset, Usb3Hc->Usb3Protocol.PortCount));
                    } else if (((XHCI_EXT_PROTOCOL*)XhciExtCap)->MinorRev == 0x01) {
                        XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, CurPtrOffset, 3, &Usb3Hc->Usb31Protocol);
                        DEBUG((DEBUG_INFO, "XHCI: USB3.1 Support Protocol %x, PortOffset %x PortCount %x\n", 
                            Usb3Hc->BaseAddress + CurPtrOffset, Usb3Hc->Usb31Protocol.PortOffset, Usb3Hc->Usb31Protocol.PortCount));
                    }
                }
                break;

            case XHCI_EXT_CAP_POWERMANAGEMENT:
            case XHCI_EXT_CAP_IO_VIRTUALIZATION:
                break;
            case XHCI_EXT_CAP_USB_DEBUG_PORT:
                Usb3Hc->DbCapOffset = CurPtrOffset;
                DEBUG((DEBUG_INFO, "XHCI: USB Debug Capability Ptr %x\n", Usb3Hc->DbCapOffset));
                break;
        }
        if (XhciExtCap->NextCapPtr == 0) {
            break;
        }
        // Point to next capability
        CurPtrOffset += XhciExtCap->NextCapPtr << 2;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
XhciReadHcMem(
    USB3_CONTROLLER                 *Usb3Hc,
    EFI_PEI_PCI_CFG_PPI_WIDTH       Width,
    UINT32	                        Offset,
    UINTN                           Count,
    VOID                            *Buffer
)
{
    UINTN   i;
    if (Width < 0 || Width > EfiPeiPciCfgWidthUint64) {
        return EFI_INVALID_PARAMETER;
    }

    if (Count == 0) {
        return EFI_INVALID_PARAMETER;
    }

    if (Buffer == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    for (i = 0; i < Count; i++) {
        
        switch (Width) {
            case EfiPeiPciCfgWidthUint8:
                ((UINT8*)Buffer)[i] = MmioRead8(Usb3Hc->BaseAddress + Offset + i * sizeof(UINT8));
                break;
            case EfiPeiPciCfgWidthUint16:
                ((UINT16*)Buffer)[i] = MmioRead16(Usb3Hc->BaseAddress + Offset + i * sizeof(UINT16));
                break;
            case EfiPeiPciCfgWidthUint32:
                ((UINT32*)Buffer)[i] = MmioRead32(Usb3Hc->BaseAddress + Offset + i * sizeof(UINT32));
                break;
            case EfiPeiPciCfgWidthUint64:
                ((UINT64*)Buffer)[i] = MmioRead64(Usb3Hc->BaseAddress + Offset + i * sizeof(UINT64));
                break;
            default:
                return EFI_INVALID_PARAMETER;
        }
    }
    
    return EFI_SUCCESS;
}

EFI_STATUS
XhciWriteHcMem(
    USB3_CONTROLLER                 *Usb3Hc,
    EFI_PEI_PCI_CFG_PPI_WIDTH       Width,
    UINT32	                        Offset,
    UINTN                           Count,
    VOID                            *Buffer
)
{
    UINTN   i;
    if (Width < 0 || Width > EfiPeiPciCfgWidthUint64) {
        return EFI_INVALID_PARAMETER;
    }

    if (Count == 0) {
        return EFI_INVALID_PARAMETER;
    }

    if (Buffer == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    for (i = 0; i < Count; i++) {
        
        switch (Width) {
            case EfiPeiPciCfgWidthUint8:
                MmioWrite8(Usb3Hc->BaseAddress + Offset + i * sizeof(UINT8), ((UINT8*)Buffer)[i]);
                break;
            case EfiPeiPciCfgWidthUint16:
                MmioWrite16(Usb3Hc->BaseAddress + Offset + i * sizeof(UINT16), ((UINT16*)Buffer)[i]);
                break;
            case EfiPeiPciCfgWidthUint32:
                MmioWrite32(Usb3Hc->BaseAddress + Offset + i * sizeof(UINT32), ((UINT32*)Buffer)[i]);
                break;
            case EfiPeiPciCfgWidthUint64:
                MmioWrite64(Usb3Hc->BaseAddress + Offset + i * sizeof(UINT64), ((UINT64*)Buffer)[i]);
                break;
            default:
                return EFI_INVALID_PARAMETER;
        }
    }
    
    return EFI_SUCCESS;
}

VOID
XhciSetHcMem(
    USB3_CONTROLLER     *Usb3Hc,
    UINT32              Offset,
    UINT32              Bit
)
{
	UINT32  Data;

    XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, Offset, 1, &Data);
	Data |= Bit;
	XhciWriteHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, Offset, 1, &Data);
    return;
}


VOID
XhciClearHcMem(
    USB3_CONTROLLER     *Usb3Hc,
    UINT32      Offset,
    UINT32      Bit
)
{
    UINT32  Data;

    XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, Offset, 1, &Data);
    Data &= ~Bit;
    XhciWriteHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, Offset, 1, &Data);
    
	return;
}

UINT32
XhciReadOpReg(
    USB3_CONTROLLER     *Usb3Hc,
    UINT32              Offset
)
{
    UINT32  Data;
    
    XhciReadHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, Usb3Hc->CapRegs.CapLength + Offset, 1, &Data);
    
    return Data;
}

VOID
XhciWriteOpReg(
    USB3_CONTROLLER     *Usb3Hc,
    UINT32      Offset,
    UINT32      Data
)
{
    XhciWriteHcMem(Usb3Hc, EfiPeiPciCfgWidthUint32, Usb3Hc->CapRegs.CapLength + Offset, 1, &Data);  
    return;
}

VOID
XhciSetOpReg(
    USB3_CONTROLLER     *Usb3Hc,
    UINT32      Offset,
    UINT32      Bit
)
{
    UINT32  Data;

    Data = XhciReadOpReg(Usb3Hc, Offset) | Bit;
    XhciWriteOpReg(Usb3Hc, Offset, Data);
    return;
}

VOID
XhciClearOpReg(
    USB3_CONTROLLER     *Usb3Hc,
    UINT32      Offset,
    UINT32      Bit
)
{
    UINT32  Data;

    Data = XhciReadOpReg(Usb3Hc, Offset) & ~Bit;
    XhciWriteOpReg(Usb3Hc, Offset, Data);
	return;
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
