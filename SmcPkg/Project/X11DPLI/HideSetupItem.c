//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
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
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#if DEBUG_MODE
#include <..\..\..\Build\NeonCity\DEBUG_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#else
#include <..\..\..\Build\NeonCity\RELEASE_MYTOOLS\X64\Build\Setup\DEBUG\SetupStrDefs.h>
#endif
#include "SmcSetupModify.h"
#include "SmcLib.h"
#include <PciBus.h>

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  X11DPTPSHideSetupItem
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

VOID X11DPLIHideSetupItem(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
	EFI_STATUS					Status = EFI_SUCCESS;
	SMC_SETUP_MODIFY_PROTOCOL	*SetupModifyProtocol;
	UINT32						X11DPLi_GPIO[] = SLOT6_GPIO_Tbl;
	UINT32						temp;
	UINT8						index;
	UINT8						temp_ID = 0;

	DEBUG((-1, "X11DPL-i HideSetupItem entry.\n"));
    
	gBS->CloseEvent(Event);

	Status = gBS->LocateProtocol(
						&gSetupModifyProtocolGuid,
						NULL,
						&SetupModifyProtocol
						);
	if(EFI_ERROR(Status))
		return;

	for(index = 0; index < 1; index++)
	{
		GpioGetInputValue(X11DPLi_GPIO[index], &temp);
		temp_ID |= temp << index;
    }

	if((temp_ID & 0x03) != 0)
		SetupModifyProtocol->Add2HideList("Setup", STR_RSC_RR1U_E8);	//ID = 0x0C   

    DEBUG((-1, "X11DPL-i HideSetupItem end.\n"));
    return;

}
