//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Re-write SmcOutBand module.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/02/2016
//
//  Rev. 1.00
//    Bug Fix:  Add Full SMBIOS module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/27/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        OobLibrary.h
//
// Description: OOB library header file.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef	_H_OobLibrary_
#define	_H_OobLibrary_

#include "Token.h"
#include "SmcLib.h"

EFI_STATUS
CalculateMd5(
    IN		VOID	*HashData,
    IN		UINTN	DataSize,
    IN OUT	UINT8	*Digest
);

//BOOLEAN CompareBIOSInfo(
//    IN SMC_BOARD_INFO* BoardInfo
//);

UINT8
OobGetCmosTokensValue(
    IN OUT	UINT32	tokenID
);

VOID
OobSetCmosTokensValue(
    IN	UINT32	tokenID,
    IN	UINT8	value
);

#endif // _H_OobLibrary_
