//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//  Rev. 1.01
//   Bug Fix:   add CPLD version support
//   Reason:	Get version from BMC
//   Auditor:   Sunny Yang
//   Date:      Sep/01/2016
//
//  Rev. 1.00
//    Bug Fix:  Sync SMBIOS type 0/2 data to BIOS setup.
//    Reason:   
//    Auditor:  Matthew You
//    Date:     May/19/2016
//
//***************************************************************************
//***************************************************************************
//***************************************************************************
#include "token.h"
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>

#include <Guid/MemoryMapData.h>
#include <TimeStamp.h>
#include "AmiDxeLib.h"

#include <Setup.h>
#include "SmcPkg\SmcLib\SmcLib.h"

VOID InitString(EFI_HII_HANDLE HiiHandle, EFI_STRING_ID StrRef, CHAR16 *sFormat, ...);
//=======================================================================
// MACROs
#define STR CONVERT_TO_WSTRING

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//InitMainPageStrings
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
InitSMCStrings(
  IN EFI_HII_HANDLE HiiHandle,
  IN UINT16         Class
  )
{
    CHAR8    TempAStr[100];
    CHAR8    BiosVersion[20];
    CHAR8    BiosDate[10];
    CHAR8    Manufacturer[30];
    CHAR8    Product[20];
    UINT32 CPLDRevision=0;
    
    MemSet(TempAStr, sizeof(TempAStr), 0);
    MemSet(BiosVersion, sizeof(BiosVersion), 0);
    MemSet(BiosDate, sizeof(BiosDate), 0);
    MemSet(Manufacturer, sizeof(Manufacturer), 0);
    MemSet(Product, sizeof(Product), 0);    
    
    SmcLibGetBiosInforInSmbios(TempAStr, BiosVersion, BiosDate);
    SmcLibGetBoardInforInSmbios(Manufacturer, Product);
    
    InitString(HiiHandle,
        	   STRING_TOKEN(STR_SMC_BIOS_DATE_VALUE),
               L"%a",
               BiosDate);

    InitString(HiiHandle,
        	   STRING_TOKEN(STR_SMC_BOARD_NAME),
               L"%a %a",
               Manufacturer,
               Product);
    
#if OFFICIAL_RELEASE
    InitString(HiiHandle,
        	   STRING_TOKEN(STR_SMC_BIOS_VERSION_VALUE), 
               L"%a",
               BiosVersion);
#else
    InitString(HiiHandle,
               STRING_TOKEN(STR_SMC_BIOS_VERSION_VALUE),
               L"T%s%s%s%s%s%s", 
               L_FOUR_DIGIT_YEAR,
               L_TWO_DIGIT_MONTH,
               L_TWO_DIGIT_DAY,
               L_TWO_DIGIT_HOUR,
               L_TWO_DIGIT_MINUTE,
               L_TWO_DIGIT_SECOND);
#endif
    CPLDRevision = PcdGet32(PcdSmcCPLDRevision);
    InitString(
        HiiHandle,STRING_TOKEN(STR_SMC_CPLD_VERSION_VALUE), 
        L"%02x.%02x.%02x",(UINT8)(CPLDRevision >> 16) & 0xFF,(UINT8)(CPLDRevision >> 8) & 0xFF,(UINT8) (CPLDRevision) & 0xFF);
    
    return;
    
}
