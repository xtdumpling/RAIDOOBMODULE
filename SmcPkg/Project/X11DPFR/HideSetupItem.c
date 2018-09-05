//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Unhide OPROM selection. 
//    Reason:   
//    Auditor:  Joshua Fan
//    Date:     Feb/06/2017
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Joshua Fan
//    Date:     Feb/04/2016
//
//****************************************************************************
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        HideSetupItem.c
//
// Description: Add hide setup items for the project.
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Token.h>

#include "EFI.h"
#include <Library\UefiBootServicesTableLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/PciLib.h>
#include <AmiDxeLib.h>
//#include <Library/PciExpressLib.h> //AptioV server override: dynamic MMCFG base change support.
#if DEBUG_MODE
#include <..\..\..\Build\NeonCity\DEBUG_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#else
#include <..\..\..\Build\NeonCity\RELEASE_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#endif
#include "SmcSetupModify.h"
#include "SmcLib.h"
#include <PciBus.h>
#include <Library\GpioLib.h>

//#define PCIE_CFG_ADDR(bus,dev,func,reg) \
//  ((UINTN) GetPciExpressBaseAddress() + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg)//AptioV server overrride: Dynamic mmcfg base address change

#define PCI_SID_OFFSET                              0x2E ///< SubSystem ID
#define PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET    0x19


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  X11DPFRHideSetupItem
//
// Description: 
//  Get SuperM Setup menu Hidden Item list.
//
// Input:
//      None
//
// Output:
//      UINT16 *HiddenItemList - Hidden item list for output
//      EFI_STATUS (return) -
//          EFI_SUCCESS : Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID X11DPFRHideSetupItem(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    SMC_SETUP_MODIFY_PROTOCOL *SetupModifyProtocol;

    UINT16	UpdatedLAN1BDF;
    UINT32 Data32, BusNum, Temp;
    UINT8 LanBus;
    UINT32 ReturnVal0 = 0, ReturnVal1 = 0, ReturnVal2 = 0, ReturnVal3 = 0, ReturnVal4 = 0, ReturnVal5 = 0, SIOM_ID = 0, SXB3_ID = 0, SXB4_ID = 0, SXB5_ID = 0;

    gBS->CloseEvent(Event);
    
    Status = gBS->LocateProtocol (&gSetupModifyProtocolGuid, NULL, &SetupModifyProtocol);
    if (EFI_ERROR (Status)) {
        DEBUG((-1, "ERROR : X11DPFRHideSetupItem - Can't locate gSetupModifyProtocolGuid.\n"));
        return;
    }
//Riser
//Check Riser card GPIO for different bifurcation.
	GpioGetInputValue(JPCIE1_ID0, &ReturnVal0);  // SXB1
	GpioGetInputValue(JPCIE1_ID1, &ReturnVal1);
	GpioGetInputValue(JPCIE2_ID0, &ReturnVal2);  // SXB3
	GpioGetInputValue(JPCIE2_ID1, &ReturnVal3);  
	GpioGetInputValue(SASSLOT_ID1, &ReturnVal4); // SXB2
	GpioGetInputValue(SASSLOT_ID2, &ReturnVal5);

	SXB3_ID = (ReturnVal1 << 1) | ReturnVal0; // JPCIE1_1 (SXB1)
	SXB4_ID = (ReturnVal3 << 1) | ReturnVal2; // JPCIE2   (SXB3)
	SXB5_ID = (ReturnVal5 << 1) | ReturnVal4; // SAS SLOT (SXB2)
        DEBUG((-1, "SXB3_ID = %x, SXB4_ID = %x\n", SXB3_ID, SXB4_ID));        
        
//    if(SXB3_ID != 0)//RSC-R1UTP-E16R installed
//        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_1_OPROM);

//    if(SXB4_ID != 2)//RSC-R2UT-E8E16R installed
//        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_2_OPROM);
        
//    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_1_SELECT);
//    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_2_SELECT);
//    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_3_SELECT);
//    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_4_SELECT);
    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_5_SELECT);
    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_6_SELECT);   
    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_7_SELECT);
    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_8_SELECT);
    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_9_SELECT);
    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_10_SELECT);
    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_11_SELECT);
    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_12_SELECT);
//	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_3_OPROM);
//	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_4_OPROM);
//	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_6_OPROM);
//	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_7_OPROM);
	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_8_OPROM);		
	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_9_OPROM);
	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_10_OPROM);
	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_11_OPROM);

    UpdatedLAN1BDF = SmcBDFTransfer(PcdGet32(PcdSmcOBLan1BDF));

	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_1_10G_SELECT);//Always hide onboard 10G option.
//Check Sub System ID to hide i350 SIOM card
	Temp = PciRead32(PCI_LIB_ADDRESS((UpdatedLAN1BDF >> 8), 0, 0, 0x18));//Save

//Allocate temp BUS number to read SIOM card
    PciWrite8(PCI_LIB_ADDRESS((UpdatedLAN1BDF >> 8), 0, 0, 0x18), (UpdatedLAN1BDF >> 8)); 
    PciWrite8(PCI_LIB_ADDRESS((UpdatedLAN1BDF >> 8), 0, 0, 0x19), (UpdatedLAN1BDF >> 8)+1);
    PciWrite8(PCI_LIB_ADDRESS((UpdatedLAN1BDF >> 8), 0, 0, 0x1A), (UpdatedLAN1BDF >> 8)+1);
//Read Data
    BusNum = PciRead32(PCI_LIB_ADDRESS((UpdatedLAN1BDF >> 8), 0, 0, 0x18));
    LanBus = (BusNum >> 8) & 0xFF;
    Data32 = PciRead32(PCI_LIB_ADDRESS(LanBus, 0, 0, 0));
    DEBUG((EFI_D_INFO, "X11DPFRHideSetupItem BusNum = %x, LanBus = %x, Data32 = %x UpdatedLAN1BDF = %x End.\n",BusNum, LanBus, Data32, (UpdatedLAN1BDF >> 8)));
    
	if (*(UINT16*)MmPciAddress(0, *(UINT8*)MmPciAddress(0, (UpdatedLAN1BDF >> 8), 0, 0, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ), 0, 0, 2) == 0x1521){
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_5_OPROM);//Hide SIOM slot control
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_6_OPROM);//Hide SIOM slot control
		if (*(UINT16*)MmPciAddress(0, *(UINT8*)MmPciAddress(0, (UpdatedLAN1BDF >> 8), 0, 0, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ), 0, 0,PCI_SID_OFFSET) == 0x0912){//This sku only 2 port
			SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_3_SELECT);
			SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_4_SELECT);
		}
	} else if (*(UINT16*)MmPciAddress(0, *(UINT8*)MmPciAddress(0, (UpdatedLAN1BDF >> 8), 0, 0, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ), 0, 0, 2) == 0x1533) {
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_5_OPROM);//Hide SIOM slot control
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_2_SELECT);
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_3_SELECT);
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_4_SELECT);
	} else {
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_1_SELECT);
		SetupModifyProtocol->Add2HideList("Setup", STR_ONBOARD_LAN_OPROM_TYPE_PROMPT);
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_2_SELECT);
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_3_SELECT);
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_4_SELECT);		
	}
	
	PciWrite32(PCI_LIB_ADDRESS((UpdatedLAN1BDF >> 8), 0, 0, 0x18), Temp);//Restore

}

