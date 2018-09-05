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
//    Auditor:  Isaac Hsu
//    Date:     Jan/04/2017
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
// Name:  X11SPLHideSetupItem
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

VOID X11SPLHideSetupItem(
    IN EFI_EVENT Event,
    IN VOID *Context
)
{

}

