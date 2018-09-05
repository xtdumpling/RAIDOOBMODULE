//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix : N/A
//    Reason  : Hidden "TXT Support" in BIOS Setup menu dynamically when CPU 
//              doesn't support it.
//    Auditor : Joe Jhang
//    Date    : Jun/30/2017
//
//****************************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        ProjectHideSetupItem.c
//
// Description: To hide project common setup item.
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Token.h>
#include "EFI.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Iio/IioRegs.h>
#include "SuperMSetupStrDefs.h"
#include "SmcSetupModify.h"

/**
  To hide project common setup item.

  This function is to hide project common setup item.

  @param  Event    Event triggered.
  @param  Context  Parameter passed with the event.

**/
VOID ProjectHideSetupItem(
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  SMC_SETUP_MODIFY_PROTOCOL *SetupModifyProtocol;
  
  DEBUG((DEBUG_INFO, "ProjectHideSetupItem Entry.\n"));

  Status = gBS->LocateProtocol (
                  &gSetupModifyProtocolGuid,
                  NULL,
                  &SetupModifyProtocol
                  );
  if (EFI_ERROR (Status)) return;

  //
  // Added project common hidden item here.
  //
#ifdef LT_FLAG
  if ((((*(volatile UINT32 *)(UINTN)LT_SPAD_EXIST)) & BIT0) == 0) {
    SetupModifyProtocol->Add2HideList("Setup", STR_LTSX);
  }
#endif //#ifdef LT_FLAG

  DEBUG((DEBUG_INFO, "ProjectHideSetupItem End.\n"));

  return;
}

