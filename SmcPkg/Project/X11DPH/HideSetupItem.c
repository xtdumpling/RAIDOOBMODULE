//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Option Rom control setup menu modify
//    Reason:   .
//    Auditor:  Max Mu
//    Date:     May/18/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Max Mu
//    Date:     Mar/02/2016
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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#if DEBUG_MODE
#include <..\..\..\Build\NeonCity\DEBUG_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#else
#include <..\..\..\Build\NeonCity\RELEASE_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#endif
#include "SmcSetupModify.h"
#include "SmcLib.h"

//
// Remove all PCI/PCIe option rom control items from SETUP menu, which are not defined
// in X11DPH.sdl by BDF token
// Copy from SmcRiserCard module.
//
//
#if !SmcRiserCard_SUPPORT
EFI_STATUS EFIAPI X11DPH_HiddenPCIeOptRomSetupItem(SMC_SETUP_MODIFY_PROTOCOL *SetupModifyProtocol)
{
    UINT8	i;
    UINTN	SlotPcdNumTbl[] = {
    		PcdToken(PcdSmcSLOT1BDF), PcdToken(PcdSmcSLOT2BDF), PcdToken(PcdSmcSLOT3BDF),
    		PcdToken(PcdSmcSLOT4BDF), PcdToken(PcdSmcSLOT5BDF), PcdToken(PcdSmcSLOT6BDF),
    		PcdToken(PcdSmcSLOT7BDF), PcdToken(PcdSmcSLOT8BDF), PcdToken(PcdSmcSLOT9BDF),
    		PcdToken(PcdSmcSLOT10BDF), PcdToken(PcdSmcSLOT11BDF)};

    UINTN	LanPcdNumTbl[] = {
    		PcdToken(PcdSmcOBLan1BDF), PcdToken(PcdSmcOBLan2BDF), PcdToken(PcdSmcOBLan3BDF),
    		PcdToken(PcdSmcOBLan4BDF)};

    UINTN	NVMePcdNumTbl[] = {
    		PcdToken(PcdSmcOBNVMe1BDF), PcdToken(PcdSmcOBNVMe2BDF), PcdToken(PcdSmcOBNVMe3BDF),
    		PcdToken(PcdSmcOBNVMe4BDF), PcdToken(PcdSmcOBNVMe5BDF), PcdToken(PcdSmcOBNVMe6BDF),
    		PcdToken(PcdSmcOBNVMe7BDF), PcdToken(PcdSmcOBNVMe8BDF), PcdToken(PcdSmcOBNVMe9BDF),
    		PcdToken(PcdSmcOBNVMe10BDF), PcdToken(PcdSmcOBNVMe11BDF), PcdToken(PcdSmcOBNVMe12BDF)};

    UINT8	SlotPcdNumTblSize = sizeof(SlotPcdNumTbl)/sizeof(UINTN);
    UINT8	LanPcdNumTblSize = sizeof(LanPcdNumTbl)/sizeof(UINTN);
    UINT8	NVMePcdNumTblSize = sizeof(NVMePcdNumTbl)/sizeof(UINTN);

    UINT16	STR_RC_OPROM_Table[] = {
            STR_SMC_PCI_SLOT_1_OPROM, STR_SMC_PCI_SLOT_2_OPROM,
            STR_SMC_PCI_SLOT_3_OPROM, STR_SMC_PCI_SLOT_4_OPROM,
            STR_SMC_PCI_SLOT_5_OPROM, STR_SMC_PCI_SLOT_6_OPROM,
            STR_SMC_PCI_SLOT_7_OPROM, STR_SMC_PCI_SLOT_8_OPROM,
            STR_SMC_PCI_SLOT_9_OPROM, STR_SMC_PCI_SLOT_10_OPROM,
            STR_SMC_PCI_SLOT_11_OPROM};

    UINT16	STR_RC_LAN_Table[] = {
            STR_SMC_ONBOARD_LAN_1_SELECT, STR_SMC_ONBOARD_LAN_2_SELECT,
            STR_SMC_ONBOARD_LAN_3_SELECT, STR_SMC_ONBOARD_LAN_4_SELECT};

    UINT16	STR_RC_NVMe_Table[] = {
            STR_SMC_ONBOARD_NVME_1_SELECT, STR_SMC_ONBOARD_NVME_2_SELECT,
            STR_SMC_ONBOARD_NVME_3_SELECT, STR_SMC_ONBOARD_NVME_4_SELECT,
            STR_SMC_ONBOARD_NVME_5_SELECT, STR_SMC_ONBOARD_NVME_6_SELECT,
            STR_SMC_ONBOARD_NVME_7_SELECT, STR_SMC_ONBOARD_NVME_8_SELECT,
            STR_SMC_ONBOARD_NVME_9_SELECT, STR_SMC_ONBOARD_NVME_10_SELECT,
            STR_SMC_ONBOARD_NVME_11_SELECT, STR_SMC_ONBOARD_NVME_12_SELECT};
            

    DEBUG((-1, "X11DPH_HiddenPCIeOptRomSetupItem entry.\n"));
// hide pcie slot root port
    for(i = 0; i < (sizeof(STR_RC_OPROM_Table)/sizeof(UINT16)); i++){
        if(!LibPcdGet32(SlotPcdNumTbl[i]) || (LibPcdGet32(SlotPcdNumTbl[i]) == 0x88888888))
            SetupModifyProtocol->Add2HideList("Setup", STR_RC_OPROM_Table[i]);
    }
// hide LAN port
    for(i = 0; i < (sizeof(STR_RC_LAN_Table)/sizeof(UINT16)); i++){
        if(!LibPcdGet32(LanPcdNumTbl[i]) || (LibPcdGet32(LanPcdNumTbl[i]) == 0x88888888))
            SetupModifyProtocol->Add2HideList("Setup", STR_RC_LAN_Table[i]);
    }
// hide NVMe port
    for(i = 0; i < (sizeof(STR_RC_NVMe_Table)/sizeof(UINT16)); i++){
        if(!LibPcdGet32(NVMePcdNumTbl[i]) || (LibPcdGet32(NVMePcdNumTbl[i]) == 0x88888888))
            SetupModifyProtocol->Add2HideList("Setup", STR_RC_NVMe_Table[i]);
    }
// hide SAS port
    if(!PcdGet32(PcdSmcOBSASBDF) || PcdGet32(PcdSmcOBSASBDF) == 0x88888888)
    	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_SAS_SELECT);

    DEBUG((-1, "X11DPH_HiddenPCIeOptRomSetupItem end.\n"));

    return EFI_SUCCESS;
}        

#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  X11DPHHideSetupItem
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

VOID X11DPHHideSetupItem(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    SMC_SETUP_MODIFY_PROTOCOL *SetupModifyProtocol;

    Status = gBS->LocateProtocol (&gSetupModifyProtocolGuid, NULL, &SetupModifyProtocol);
    if (EFI_ERROR (Status))        return;

/*-----------------12/19/2014 11:50AM---------------    
 * Below is X11DPU
 * --------------------------------------------------*/
#if !SmcRiserCard_SUPPORT
    X11DPH_HiddenPCIeOptRomSetupItem(SetupModifyProtocol);
#endif
    //SetupModifyProtocol->Add2HideList ("Setup", STR_SMC_WATCH_DOG_FN);

}

