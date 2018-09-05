//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/16/2016
//
//***************************************************************************
//***************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcCspLibSb.h
//
// Description: Supermicro sourth bridge library header file.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef __SMC_CSP_LIB_SB__H__
#define __SMC_CSP_LIB_SB__H__
#ifdef __cplusplus
extern "C" {
#endif


#pragma pack(push,1)
#pragma pack(pop)

VOID SMC_Enable_WOL(VOID);

VOID SMC_Enable_WOR(VOID);

VOID SMC_Enable_PME(VOID);

UINT8 SMC_Get_SATA_Mode();

EFI_STATUS
AfterG3Setting(
    IN	BOOLEAN	LastState
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************
