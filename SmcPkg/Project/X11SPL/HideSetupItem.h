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
// Name:        HideSetupItem.h
//
// Description: Add hide setup items for the project.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef __SMC_HIDESETUPITEM__H__
#define __SMC_HIDESETUPITEM__H__

VOID X11SPLHideSetupItem (
    IN EFI_EVENT Event,
    IN VOID *Context
);

#endif // __SMC_HIDESETUPITEM__H__

