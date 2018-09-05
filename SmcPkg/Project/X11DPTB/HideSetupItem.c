//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Jacker Yeh
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
// Name:  X11DPTBHideSetupItem
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

VOID X11DPTBHideSetupItem(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    SMC_SETUP_MODIFY_PROTOCOL *SetupModifyProtocol;

    UINT16 UpdatedBDF;
    UINT32 Data32, BusNum, OldBusNum;
    UINT8 LanBus;
    UINT32 ReturnVal0 = 0, ReturnVal1 = 0, ReturnVal2 = 0, ReturnVal3 = 0, SIOM_ID = 0;

    gBS->CloseEvent(Event);
    
    Status = gBS->LocateProtocol (&gSetupModifyProtocolGuid, NULL, &SetupModifyProtocol);
    if (EFI_ERROR (Status)) {
        DEBUG((-1, "ERROR : X11DPTB HideSetupItem - Can't locate gSetupModifyProtocolGuid.\n"));
        return;
    }
//Riser
    GpioGetInputValue(SXB3_ID0, &ReturnVal0);
    GpioGetInputValue(SXB3_ID1, &ReturnVal1);
    SIOM_ID = (ReturnVal1 << 1) | ReturnVal0;

    if(SIOM_ID != 0x00) {
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_1_OPROM);
    }

    GpioGetInputValue(SXB4_ID0, &ReturnVal0);
    GpioGetInputValue(SXB4_ID1, &ReturnVal1);
    SIOM_ID = (ReturnVal1 << 1) | ReturnVal0;

    if(SIOM_ID != 0x02) {
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_2_OPROM);
    }
    
//SXB1
    GpioGetInputValue(SXB1_ID0, &ReturnVal0);
    GpioGetInputValue(SXB1_ID1, &ReturnVal1);
    GpioGetInputValue(SXB1_ID2, &ReturnVal2);
    SIOM_ID = (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;
/*
    if (SIOM_ID == 0x06)
    {
        UpdatedBDF = SmcBDFTransfer(PcdGet32(PcdSmcSLOT3BDF));
        OldBusNum = PciRead32(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18));//Save
        PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18), (UpdatedBDF>>8));
        PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x19), (UpdatedBDF>>8)+1);
        PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x1A), (UpdatedBDF>>8)+1);
        if (*(UINT16*)MmPciAddress(0, *(UINT8*)MmPciAddress(0, UpdatedBDF>>8, (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ), 0, 0, 2) == 0xFFFF) {
          SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_3_OPROM);
        }
        PciWrite32(PCI_LIB_ADDRESS((UpdatedBDF >> 8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18), OldBusNum);//Restore

        UpdatedBDF = SmcBDFTransfer(PcdGet32(PcdSmcSLOT4BDF));
        OldBusNum = PciRead32(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18));//Save
        PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18), (UpdatedBDF>>8));
        PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x19), (UpdatedBDF>>8)+1);
        PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x1A), (UpdatedBDF>>8)+1);
        if (*(UINT16*)MmPciAddress(0, *(UINT8*)MmPciAddress(0, UpdatedBDF>>8, (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ), 0, 0, 2) == 0xFFFF) {
          SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_4_OPROM);
        }
        PciWrite32(PCI_LIB_ADDRESS((UpdatedBDF >> 8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18), OldBusNum);//Restore
    }
*/
//SXB2
    GpioGetInputValue(SXB2_ID0, &ReturnVal0);
    GpioGetInputValue(SXB2_ID1, &ReturnVal1);
    GpioGetInputValue(SXB2_ID2, &ReturnVal2);
    SIOM_ID = (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;

    switch(SIOM_ID)
    {
      case 0x04:	//BPN-ADP-12NVMe-2UB
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_SAS_SELECT);
        break;  
      case 0x06:	//BPN-ADP-6NVMe3-1UB
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_7_OPROM);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_8_OPROM);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_7_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_8_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_9_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_10_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_11_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_12_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_SAS_SELECT);
        break;
      case 0x01:	//BPN-ADP-4TNS6-1UB
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_SAS_SELECT);
      case 0x03:	//BPN-ADP-12S3216N4-2UB
      case 0x05:	//BPN-ADP-6S3008N4-1UB
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_5_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_6_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_7_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_8_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_9_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_10_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_11_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_12_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_7_OPROM);        
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_8_OPROM);
        break;
      default:
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_1_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_2_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_3_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_4_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_5_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_6_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_7_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_8_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_9_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_10_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_11_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_12_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_SAS_SELECT);
        break;
    }

//  SetupModifyProtocol->Add2HideList("Setup", STR_PCIE_PORT_13_PROMPT);//Always hide CPU1 MCP 0
//  SetupModifyProtocol->Add2HideList("Setup", STR_PCIE_PORT_17_PROMPT);//Always hide CPU1 MCP 0
//  SetupModifyProtocol->Add2HideList("Setup", STR_PCIE_PORT_34_PROMPT);//Always hide CPU2 MCP 0
//  SetupModifyProtocol->Add2HideList("Setup", STR_PCIE_PORT_38_PROMPT);//Always hide CPU2 MCP 1

    
    UpdatedBDF = SmcBDFTransfer(PcdGet32(PcdSmcOBLan1BDF));

    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_1_10G_SELECT);//Always hide onboard 10G option.
//Check Sub System ID to hide i350 SIOM card
    OldBusNum = PciRead32(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18));//Save

//Allocate temp BUS number to read SIOM card
    PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18), (UpdatedBDF>>8));
    PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x19), (UpdatedBDF>>8)+1);
    PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x1A), (UpdatedBDF>>8)+1);

//Read Data
    BusNum = PciRead32(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18));
    LanBus = (BusNum >> 8) & 0xFF;
    Data32 = PciRead32(PCI_LIB_ADDRESS(LanBus, 0, 0, 0));
    DEBUG((EFI_D_INFO, "X11DPTB HideSetupItem BusNum = %x, LanBus = %x, Data32 = %x UpdatedLAN1BDF = %x End.\n",BusNum, LanBus, Data32, (UpdatedBDF >> 8)));
    
	if (*(UINT16*)MmPciAddress(0, *(UINT8*)MmPciAddress(0, (UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ), 0, 0, 2) == 0x1521){
//      if (Data32 == 0x15218086){
		//These skus only 2 port
		if (*(UINT16*)MmPciAddress(0, *(UINT8*)MmPciAddress(0, (UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ), 0, 0,PCI_SID_OFFSET) == 0x0912)
		{
			SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_5_OPROM);//Hide SIOM slot control
			SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_3_SELECT);
			SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_4_SELECT);
		} else if ((*(UINT16*)MmPciAddress(0, *(UINT8*)MmPciAddress(0, (UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ), 0, 0,PCI_SID_OFFSET) == 0x0956) ||
		           (*(UINT16*)MmPciAddress(0, *(UINT8*)MmPciAddress(0, (UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ), 0, 0,PCI_SID_OFFSET) == 0x0957))
		{
			SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_6_OPROM);	//Hide SIOM slot control
			SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_3_SELECT);
			SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_4_SELECT);
		} else {
			SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_5_OPROM);//Hide SIOM slot control
		}
	} else if (*(UINT16*)MmPciAddress(0, *(UINT8*)MmPciAddress(0, (UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ), 0, 0, 2) == 0x1533) {
//	} else if (Data32 == 0x15338086) {
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_6_OPROM);		//Hide SIOM slot control
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
	
	PciWrite32(PCI_LIB_ADDRESS((UpdatedBDF >> 8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18), OldBusNum);//Restore

//Check SIOM GPIO for special SIOM card.
/*
    AOC-MTG-I4T/I2T   : 1100b
    AOC-MHIBF-2QG/1QG : 1010b
    AOC-MHIBF-m2Q2G
    AOC-MHIBF-m1Q2G
    AOC-MH25G-b2S2G   : 1001b
    AOC-MH25G-m2S2T   : 0111b
*/

    UpdatedBDF = SmcBDFTransfer(PcdGet32(PcdSmcSLOT5BDF));
    OldBusNum = PciRead32(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18));//Save
    PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18), (UpdatedBDF>>8));
    PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x19), (UpdatedBDF>>8)+1);
    PciWrite8(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x1A), (UpdatedBDF>>8)+1);
	
    GpioGetInputValue(SIOM_ID0, &ReturnVal0);
    GpioGetInputValue(SIOM_ID1, &ReturnVal1);
    GpioGetInputValue(SIOM_ID2, &ReturnVal2);
    GpioGetInputValue(SIOM_ID3, &ReturnVal3);
    SIOM_ID = (ReturnVal3 << 3) | (ReturnVal2 << 2) | (ReturnVal1 << 1) | ReturnVal0;

    if((SIOM_ID == 0x0C) || (SIOM_ID == 0x09) || (SIOM_ID == 0x07))
    {
      if (*(UINT16*)MmPciAddress(0, *(UINT8*)MmPciAddress(0, UpdatedBDF>>8, (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ), 0, 0, 2) == 0xFFFF) {
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_5_OPROM);//Hide SIOM slot control
      }
    } else {
		SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_6_OPROM);//Hide SIOM slot control
    }
    PciWrite32(PCI_LIB_ADDRESS((UpdatedBDF>>8), (UpdatedBDF&0xFF)>>3, (UpdatedBDF&0x07), 0x18), OldBusNum);//Restore

	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_9_OPROM);
	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_10_OPROM);
	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_PCI_SLOT_11_OPROM);

}

