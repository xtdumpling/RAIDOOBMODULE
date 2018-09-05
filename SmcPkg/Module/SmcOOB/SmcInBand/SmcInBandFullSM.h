//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.04
//    Bug Fix:  Add smbios version for SUM.
//    Reason:   SUM required
//    Auditor:  Kasber Chen
//    Date:     Aug/04/2017
//
//  Rev. 1.03
//    Bug Fix:  Add Full SmBios InBand support.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/05/2016
//
//  Rev. 1.02
//    Bug Fix:  Add Full SMBIOS module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/27/2016
//
//  Rev. 1.01
//    Bug Fix:  Update Feature support Flags code for new spec.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/30/2015
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Mar/31/2015
//
//****************************************************************************

//<AMI_FHDR_START>
//
// Name:  SmcFullSM.h
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef	_H_SmcFullSM_
#define	_H_SmcFullSM_

#include <AmiDxeLib.h>
#include <AmiCspLib.h>


#define SM_SIGNATURE 0x5F4D535F

#pragma pack(1)

typedef struct {
    UINT8	Signature[4];
    UINT16	Version;
    UINT32	FileSize;	//excluding header
    UINT8	Checksum;	//excluding header
    UINT8	Flag;
    UINT8       SMBIOSMajorVer;
    UINT8       SMBIOSMinorVer;
    UINT8       SMBIOSDocrev;
    UINT8	Reserve[3];
} InBand_FullSmBios_Header;

#pragma pack()

VOID CombineFullSmBios (UINT8* BuffAddress, UINT32* BuffSize);

#endif // _H_SmcFullSM_

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2013, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
