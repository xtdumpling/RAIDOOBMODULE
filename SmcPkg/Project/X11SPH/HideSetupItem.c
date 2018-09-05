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
//
//  Rev. 1.01
//    Bug Fix:  Support riser card "RSC-RR1U-E16"
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Sep/09/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Kasber Chen
//    Date:     Jun/24/2016
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
// Name:  X11SPHHideSetupItem
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

VOID X11SPHHideSetupItem(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    UINT32	X11SPH_GPIO[] = SLOT6_GPIO_Tbl, temp;
    UINT8	index, temp_ID = 0;
    EFI_STATUS Status = EFI_SUCCESS;
    SMC_SETUP_MODIFY_PROTOCOL *SetupModifyProtocol;

    DEBUG((-1, "X11SPHHideSetupItem entry.\n"));
    
    gBS->CloseEvent(Event);

    Status = gBS->LocateProtocol(
		    &gSetupModifyProtocolGuid,
		    NULL,
		    &SetupModifyProtocol);

    if(EFI_ERROR(Status))	return;

    for(index = 0; index < 4; index++){
    	GpioGetInputValue(X11SPH_GPIO[index], &temp);
    	temp_ID |= temp << index;
    }

    if((temp_ID & 0x0f) == 0x0e)                                        //E16 exist
        SetupModifyProtocol->Add2HideList("Setup", STR_RSC_RR1U_E8);	
    else if((temp_ID & 0x0f) == 0x0c)                                   //E8 exist
        SetupModifyProtocol->Add2HideList("Setup", STR_RSC_RR1U_E16);	
    else {  //no riser 
	SetupModifyProtocol->Add2HideList("Setup", STR_RSC_RR1U_E16);	//ID = 0x0E
	SetupModifyProtocol->Add2HideList("Setup", STR_RSC_RR1U_E8);	//ID = 0x0C   
    }
    
    DEBUG((-1, "X11SPHHideSetupItem end.\n"));
    return;
}

