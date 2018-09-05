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

#include "SmcStack.h"

UINT64 ExpStack[STACK_SIZE] = {0};
UINT32 ExpStackTop = STACK_EMPTY;

VOID InitSmcStack ()
{
    UINT32 ii = 0;
    for (ii = 0; ii < STACK_SIZE; ii++) ExpStack[ii] = 0;
}

UINT32 SmcStackSize ()
{
    return ExpStackTop;
}

/*PUSH FUNCTION*/
//
// Status
//     0 : Failure
//     1 : Success
//
VOID SmcPush64 (UINT64 Item, UINT8* Status)
{   if (ExpStackTop == (STACK_SIZE - 1))
        *Status = 0;
    else
    {   *Status = 1;
        ++(ExpStackTop);
        ExpStack [ExpStackTop] = Item;
    }
}

/*POP FUNCTION*/
//
// Status
//     0 : Failure
//     1 : Success
//
UINT64 SmcPop64 (UINT8* Status)
{  
    UINT64 ret;
    if (ExpStackTop == -1)
    {   ret = 0;
        *Status = 0;
    }
    else
    {
        *Status = 1;
        ret = ExpStack [ExpStackTop];
        --(ExpStackTop);
    }
    return ret;
}


