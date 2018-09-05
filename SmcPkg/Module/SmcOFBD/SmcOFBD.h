//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  History
//
//  Rev. 1.00
//    Bug Fix : Renew.
//    Reason  : Move from Greenlow
//    Auditor : Kasber Chen
//    Date    : Aug/17/2016
//
//****************************************************************************

#ifndef _SMC_OFBD_SUPPORT_H_
#define _SMC_OFBD_SUPPORT_H_
#ifdef __cplusplus
extern "C" {
#endif

VOID PreserveSmcRomHoleData (VOID);
VOID RestoreSmcRomHoleData (VOID);
VOID SmcOfbdDisableWdt (VOID);
VOID SmcOfbdRestoreWdt (VOID);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************
