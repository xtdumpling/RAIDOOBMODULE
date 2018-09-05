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
// Name:        ProjectHideSetupItem.h
//
// Description: To hide project common setup item.
//
//<AMI_FHDR_END>
//**********************************************************************
#ifndef __SMC_PROJECT_HIDESETUPITEM__H__
#define __SMC_PROJECT_HIDESETUPITEM__H__

/**
  To hide project common setup item.

  This function is to hide project common setup item.

  @param  Event    Event triggered.
  @param  Context  Parameter passed with the event.

**/
VOID ProjectHideSetupItem (
    IN EFI_EVENT Event,
    IN VOID *Context
);

#endif // __SMC_PROJECT_HIDESETUPITEM__H__

