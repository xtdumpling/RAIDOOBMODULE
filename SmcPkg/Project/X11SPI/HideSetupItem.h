//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Send HideItem and RiserCard data to sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/23/2016
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
// Name:        HideSetupItem.h
//
// Description: Add hide setup items for the project.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef __SMC_HIDESETUPITEM__H__
#define __SMC_HIDESETUPITEM__H__

VOID X11SPIHideSetupItem (
    IN EFI_EVENT Event,
    IN VOID *Context
);

#endif // __SMC_HIDESETUPITEM__H__

