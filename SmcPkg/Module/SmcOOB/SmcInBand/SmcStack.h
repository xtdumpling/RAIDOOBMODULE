//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Fix some errors for suppressif expression calculation.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/02/2016
//
//****************************************************************************


#ifndef __SMCSTACK_H__
#define __SMCSTACK_H__

#define STACK_SIZE 32
#define STACK_EMPTY -1

VOID InitSmcStack ();
UINT32 SmcStackSize ();
VOID SmcPush64 (UINT64 Item, UINT8* Status);
UINT64 SmcPop64 (UINT8* Status);

#endif // __SMCSTACK_H__
