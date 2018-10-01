//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.08
//    Bug Fix : N/A
//    Reason  : Add Support SMC LSI OOB Module.
//    Auditor : Durant Lin
//    Date    : Sep/28/2018
//
//  Rev. 1.07
//    Bug Fix : N/A
//    Reason  : Modify HII export package lists method and DEBUG Message.
//    Auditor : Durant Lin
//    Date    : Jun/01/2018
//
//  Rev. 1.06
//    Bug Fix:  Fixed Uni String longer then 32.[Mehlow]
//    Reason:   
//    Auditor:  Durant Lin
//    Date:     Dec/21/2017
//
//  Rev. 1.05
//    Bug Fix:  Update HideItem data for sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Feb/14/2017
//
//  Rev. 1.04
//    Bug Fix:  Add Full SmBios InBand support.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/05/2016
//
//  Rev. 1.03
//    Bug Fix:  Re-write SmcOutBand module.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/02/2016
//
//  Rev. 1.02
//    Bug Fix:  Extend FileSize in InBand header.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/22/2016
//
//  Rev. 1.01
//    Bug Fix:  Add BIOS config InBand function for sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/18/2016
//
//****************************************************************************

#ifndef	_H_SMCINBANDHII_
#define	_H_SMCINBANDHII_

//#include "Token.h"
#include <AmiLib.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiPackageList.h>
#include <Protocol/HiiString.h>

#define MAX_TITLE_SIZE 512

#pragma pack(1)


typedef struct {
    UINT8	Signature[4];
    UINT16	Version;
    UINT32	FileSize;	//excluding header
    UINT8	Checksum;	//excluding header
    UINT8	Flag;
    UINT8	Reserve[6];
} INBAND_HII_FILE_HEADER;

typedef struct {
    CHAR8 Title[MAX_TITLE_SIZE];
    EFI_GUID Guid;
} SMC_HPK_DATA;

#pragma pack()

VOID CombineHiiData (UINT8* BuffAddress, UINT32* BuffSize);
VOID DisplayHiiPackageDebugMessage (EFI_HII_HANDLE *HiiHandle, EFI_HII_PACKAGE_HEADER *IfrPackagePtr);

#endif // _H_SMCINBANDHII_
