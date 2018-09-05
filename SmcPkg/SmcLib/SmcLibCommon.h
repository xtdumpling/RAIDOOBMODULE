//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.02
//      Bug Fixed:  Add GetOnBoardLanBus function.
//      Reason:     
//      Auditor:    William Hsu
//      Date:       Jan/07/2017
//
//  Rev. 1.01
//      Bug Fixed:  Fix Bus dynamic change issue.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Sep/06/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision.
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/12/2016
//**************************************************************************//
//****************************************************************************

#ifndef __SMC_LIB_COMMON__H__
#define __SMC_LIB_COMMON__H__
#ifdef __cplusplus
extern "C" {
#endif

UINT8 CheckTPMPresent(VOID);

VOID
SetCmosTokensValue(
    IN	UINT32	tokenID,
    IN	UINT8	value
);

UINT8
GetCmosTokensValue(
    IN OUT	UINT32	tokenID
);

UINT16
SmcBDFTransfer(
    IN	UINT32	OldBDF
);

UINT8
GetOnBoardLanBus(
    IN	UINT32	TempBDF
);
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************
