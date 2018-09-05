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
//    Auditor:  Jimmy Chiu
//    Date:     Nov/22/2016
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
#include <Library/PciLib.h>
#if DEBUG_MODE
#include <..\..\..\Build\NeonCity\DEBUG_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#else
#include <..\..\..\Build\NeonCity\RELEASE_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#endif
#include "SmcSetupModify.h"

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  X11DDWHideSetupItem
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

VOID X11DDWHideSetupItem(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    SMC_SETUP_MODIFY_PROTOCOL *SetupModifyProtocol;

    Status = gBS->LocateProtocol (&gSetupModifyProtocolGuid, NULL, &SetupModifyProtocol);
    if (EFI_ERROR (Status))        return;

/*-----------------12/19/2014 11:50AM---------------    
 * Below is X11DDW
 * --------------------------------------------------*/
    //SetupModifyProtocol->Add2HideList ("Setup", STR_SMC_WATCH_DOG_FN);
    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_1_10G_SELECT);
    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_3_SELECT);
    SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_4_SELECT);
    
    if (PciRead32(PCI_LIB_ADDRESS(0x0,0x1F,0x00,0x00))==0xA1C18086) //check PCH ID, X11DDW-L no NVME support
    	{        
    	SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_1_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_2_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_3_SELECT);
        SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_NVME_4_SELECT);
    	}
}

