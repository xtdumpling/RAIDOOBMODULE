//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Add type 16 and type 17, define type token.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jul/20/2016
//
//  Rev. 1.00
//    Bug Fix:  Renew the SMC ROM Hole.
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/04/2016
//
//****************************************************************************
//****************************************************************************

#ifndef __SMC_SMBIOS__H__
#define __SMC_SMBIOS__H__

#define SkipStructure	0;
#define WriteStructure	1;
#define DeleteStructure	2;

#pragma pack(push,1)
typedef struct {
    UINT32      Signature;
    UINT8       Reserved1;
    UINT8       Count;
    UINT16      Size;
    UINT16      CHK;
    UINT8       Flags;  //Flags = 1 -> restore DMI from SMCROMHOLE
    UINT8       Reserved5[5];
}SMC_PRESERVE_TABLE;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************
