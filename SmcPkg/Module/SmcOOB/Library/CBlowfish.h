//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Compress and encipher HII data for sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/25/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        CBlowfish.h
//
// Description: OOB library header file.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef	_CBLOWFISH_H_
#define	_CBLOWFISH_H_

#include "Token.h"


VOID CBlowFishInit (CHAR8* Key);
VOID CBlowFishEncipher (UINT32 *xl, UINT32 *xr);
UINT8 CBlowFishEncipherCharArray(UINT8* buffer, UINT32 BufferSize, UINT32 bSize);
UINT8 CBlowFishDecipherChararray(UINT8* buffer, UINT32 BufferSize, UINT32 bSize);


#endif // _CBLOWFISH_H_
