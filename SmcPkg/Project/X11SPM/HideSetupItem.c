//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Hide LAN OPROM Control options according to SKU.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Nov/18/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Isaac Hsu
//    Date:     Nov/18/2016
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

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  X11SPMHideSetupItem
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

VOID X11SPMHideSetupItem(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    SMC_SETUP_MODIFY_PROTOCOL *SetupModifyProtocol;
    UINT32	X11SPM_GPIO[] = SLOT6_GPIO_Tbl, temp;
    UINT8	index, temp_ID = 0;
    UINT32  X11SPM_SKU_GPIO[] = SKU_TBL_GPIO_Tbl;


    Status = gBS->LocateProtocol(&gSetupModifyProtocolGuid, NULL, &SetupModifyProtocol);
    if(EFI_ERROR(Status))
        return;

    // Hide RSC-RR1U-E16 if absent
    for(index=0; index<3; index++) {
    	GpioGetInputValue(X11SPM_GPIO[index], &temp);
    	temp_ID |= temp << index;
    }

    if(temp_ID != 0x06)
        SetupModifyProtocol->Add2HideList("Setup", STR_RSC_RR1U_E16);

    // SKU        GPP_G5  GPP_G4  LAN
    // X11SPM-F        0       0  88E1512
    // X11SPM-TF       0       1  X557
    // X11SPM-TPF      1       0  CS4227
    // X11SPM-T4F      1       1  X557+88E1512
    temp_ID = 0;
    for(index=0; index<2; index++){
        GpioGetInputValue(X11SPM_SKU_GPIO[index], &temp);
        temp_ID |= temp << index;
    }

    switch(temp_ID) {
        case 0:
        case 1:
        case 2:
            SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_3_SELECT);
            SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_4_SELECT);
            break;
        default:
            break;
    }

}

