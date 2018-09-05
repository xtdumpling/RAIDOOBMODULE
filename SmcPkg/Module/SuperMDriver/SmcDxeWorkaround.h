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
//  Rev. 1.01
//    Bug Fix : Add OnBoard LAN drop check function.
//    Reason  : Remove LAN DID check.
//    Auditor : Kasber Chen
//    Date    : Jul/31/2017
//
//  Rev 1.00
//    Bug Fix : Added B2 workaround and disabled it by default..
//    Reason  :
//    Auditor : Joe Jhang
//    Date    : Jun/01/2017
//
//****************************************************************************

#ifndef _SMC_DXE_WORKAROUND_H_
#define _SMC_DXE_WORKAROUND_H_

//
// Function prototypes.
//
VOID
SmcDxeB2Workaround(void);

VOID
SmcDxeOnboardLanWorkaround(void);

#endif
