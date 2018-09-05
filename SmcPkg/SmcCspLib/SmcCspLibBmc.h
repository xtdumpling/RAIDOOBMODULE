//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2015 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/01/2014
//
//***************************************************************************
//***************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcCspLibBmc.h
//
// Description: Supermicro BMC library header file.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef __SMC_CSP_LIB_BMC__H__
#define __SMC_CSP_LIB_BMC__H__
#ifdef __cplusplus
extern "C" {
#endif

VOID
SmcCspLibBmcPostCpl(
    IN	BOOLEAN	BmcPostCpl
);

UINT8
EFIAPI 
CheckBMCPresent(VOID);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************

