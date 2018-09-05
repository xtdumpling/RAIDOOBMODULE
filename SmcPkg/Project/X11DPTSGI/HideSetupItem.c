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
#include <Library\UefiBootServicesTableLib.h>
#include <AmiDxeLib.h>
#if DEBUG_MODE
#include <..\..\..\Build\NeonCity\DEBUG_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#else
#include <..\..\..\Build\NeonCity\RELEASE_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#endif
//#if DEBUG_MODE
//#include <..\..\Build\NeonCity\DEBUG_MYTOOLS\X64\PurleyPlatPkg\Platform\Dxe\SocketSetup\SocketSetup\DEBUG\SocketSetupStrDefs.h>
//#else
//#include <..\..\Build\NeonCity\RELEASE_MYTOOLS\X64\PurleyPlatPkg\Platform\Dxe\SocketSetup\SocketSetup\DEBUG\SocketSetupStrDefs.h>
//#endif
#include "SmcSetupModify.h"


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  X11DPTSGIHideSetupItem
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

VOID X11DPTSGIHideSetupItem(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    SMC_SETUP_MODIFY_PROTOCOL *SetupModifyProtocol;

    DEBUG((-1, "X11DPTSGIHideSetupItem Entry.\n"));

    gBS->CloseEvent(Event);

    Status = gBS->LocateProtocol (&gSetupModifyProtocolGuid, NULL, &SetupModifyProtocol);
    if (EFI_ERROR (Status)) {
        DEBUG((-1, "ERROR : X11DPTSGIHideSetupItem - Can't locate gSetupModifyProtocolGuid.\n"));
        return;
    }
    if ( PcdGetBool(PcdSGINodeIsA1B1) ){
       //hide A0 node 
       SetupModifyProtocol->Add2HideList ("Setup", STR_SMC_PCI_SLOT_1_OPROM);
    } else {
       //hide A1 node 
       SetupModifyProtocol->Add2HideList ("Setup", STR_SMC_PCI_SLOT_2_OPROM);       
    }
//    SetupModifyProtocol->Add2HideList ("Socket Configuration", STR_KTI_CPU2_TITLE);
//    SetupModifyProtocol->Add2HideList ("Socket Configuration", STR_KTI_CPU3_TITLE);
//    SetupModifyProtocol->Add2HideList ("Socket Configuration", STR_SOCKET_CONFIG_FORM_TITLE_2);
//    SetupModifyProtocol->Add2HideList ("Socket Configuration", STR_SOCKET_CONFIG_FORM_TITLE_3);
//
//    SetupModifyProtocol->Add2HideList ("Socket Configuration", STR_KTI_CPU0_LINK2);
//    SetupModifyProtocol->Add2HideList ("Socket Configuration", STR_KTI_CPU1_LINK2);
//    SetupModifyProtocol->Add2HideList ("Socket Configuration", STR_KTI_DFX_CPU0_LINK2);
//    SetupModifyProtocol->Add2HideList ("Socket Configuration", STR_KTI_DFX_CPU1_LINK2);    
    
/*-----------------12/19/2014 11:50AM---------------    
 * Below is X11DPTSGI
 * --------------------------------------------------*/
/*
    //SetupModifyProtocol->Add2HideList ("Setup", STR_SMC_WATCH_DOG_FN);
*/
     DEBUG((-1, "X11DPTSGIHideSetupItem end.\n"));
}

