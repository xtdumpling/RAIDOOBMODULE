//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.02
//    Bug Fix:  Send HideItem and RiserCard data to sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/23/2016
//
//  Rev. 1.01
//      Bug Fixed:  Add riser card name to IIO and OPROM control items.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/21/2016
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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MmPciBaseLib.h>
#include <AmiDxeLib.h>
#if DEBUG_MODE
#include <..\..\..\Build\NeonCity\DEBUG_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#else
#include <..\..\..\Build\NeonCity\RELEASE_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#endif
#include "SmcSetupModify.h"
#include "SmcLib.h"

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  X11DPUZHideSetupItem
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

VOID X11DPUZHideSetupItem (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    SMC_SETUP_MODIFY_PROTOCOL *SetupModifyProtocol;
    UINT8       i, TempBus;
    UINT16      TempBDF, TempDID;
    UINTN       LanPcdNumTbl[] = {PcdToken(PcdSmcOBLan1BDF), PcdToken(PcdSmcOBLan2BDF),
                    PcdToken(PcdSmcOBLan3BDF), PcdToken(PcdSmcOBLan4BDF)};

    DEBUG((-1, "X11DPUZHideSetupItem Entry.\n"));

    gBS->CloseEvent(Event);

    Status = gBS->LocateProtocol (&gSetupModifyProtocolGuid, NULL, &SetupModifyProtocol);
    if (EFI_ERROR (Status)) {
        DEBUG((-1, "ERROR : X11DPUZHideSetupItem - Can't locate gSetupModifyProtocolGuid.\n"));
        return;
    }

    for(i = 0; i < (sizeof(LanPcdNumTbl)/sizeof(UINTN)); i++){
        TempBDF = SmcBDFTransfer(LibPcdGet32(LanPcdNumTbl[i]));
        if((TempBDF == 0x8888) || (!TempBDF)){  //no any onboard lan
            SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_1_10G_SELECT);
            break;
        }
        TempBus = *(UINT8*)MmPciAddress(0, TempBDF >> 8, (TempBDF >> 3) & 0x1F, TempBDF & 0x07, 0x1A);
        TempDID = *(UINT16*)MmPciAddress(0, TempBus, 0, 0, 0x02);
        if(TempDID == 0x1521){  //not support FCoE
            SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_1_10G_SELECT);
            break;
        } else {
            SetupModifyProtocol->Add2HideList("Setup", STR_SMC_ONBOARD_LAN_1_SELECT);
            break;
        }
    }

    DEBUG((-1, "X11DPUZHideSetupItem end.\n"));
}

