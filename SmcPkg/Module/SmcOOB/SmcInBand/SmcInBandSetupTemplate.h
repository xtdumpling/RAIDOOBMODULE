//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Hide some forms for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/30/2016
//
//  Rev. 1.00
//    Bug Fix:  Add setup template Json code in HII for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/23/2016
//
//****************************************************************************

//<AMI_FHDR_START>
//
// Name:  SmcInBandSetupTemplate.h
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef	_H_SMCINBANDSETUPTEMPLATE_
#define	_H_SMCINBANDSETUPTEMPLATE_

#include "Uefi\UefiInternalFormRepresentation.h"

#define MAX_GUID_HANDLE_COUNT 12 // Max HII package list count

#pragma pack(1)

typedef struct _SMC_FORM_DATA {
    EFI_GUID Guid;
    UINT16 FormId;
    EFI_IFR_OP_HEADER* OpPointer;
} SMC_FORM_DATA;

typedef struct _SMC_GUID_HANDLE {
    EFI_GUID Guid;
    EFI_HII_HANDLE Handle;
} SMC_GUID_HANDLE;

typedef struct _SMC_OP_FLAG {
    UINT8* Pointer;
    UINT8 Flag;
} SMC_OP_FLAG;

#pragma pack()

CHAR8* ToGuidString (CHAR8* StringBuffer, EFI_GUID* Guid);
EFI_STATUS SmcGetString (EFI_HII_HANDLE HiiHandle, EFI_STRING_ID StrId, CHAR8* StrBuff);
VOID AddGuidHandle (EFI_GUID* Guid, EFI_HII_HANDLE Handle);
EFI_HII_HANDLE GetGuidHandle (EFI_GUID* Guid);


EFI_STATUS GenFormLayout (
    IN UINT8* HiiData,
    IN UINT32 HiiDataSize,
    OUT UINT8* BuffAddress,
    OUT UINT32* BuffSize);

#endif // _H_SMCINBANDSETUPTEMPLATE_

