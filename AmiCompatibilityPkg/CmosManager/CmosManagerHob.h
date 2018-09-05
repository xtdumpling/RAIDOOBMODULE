//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************


/** @file CmosManagerHob.h
    Contains declarations to support the CMOS Manager
    CMOS_MANAGER_HOB.

**/

#ifndef  _CMOS_MANAGER_HOB_H
#define  _CMOS_MANAGER_HOB_H

#include <hob.h>
#include <SspData.h>
#include "CmosTypes.h"

// Currently the manager's HOB size is calculated during the build process.

// Cannot define arrays of size 0
#if ((LAST_CMOS_REGISTER - FIRST_CMOS_REGISTER) < 1)
    #define MANAGED_CMOS_SIZE 1
#else
    #define MANAGED_CMOS_SIZE LAST_CMOS_REGISTER - FIRST_CMOS_REGISTER + 2
#endif

#if (TOTAL_CMOS_TOKENS < 1)
    #define TOKEN_COUNT 1
#else
    #define TOKEN_COUNT TOTAL_CMOS_TOKENS + 1
#endif

#if (TOTAL_NOT_CHECKSUMMED < 1)
    #define NO_CHECKSUM_COUNT 1
#else
    #define NO_CHECKSUM_COUNT TOTAL_NOT_CHECKSUMMED + 1
#endif


// CMOS_MANAGER_HOB GUID
//--------------------------------------------------------------
// {D5367802-B873-4c0f-B544-31B7CCF5C555}
#define CMOS_MANAGER_HOB_GUID \
{0xd5367802, 0xb873, 0x4c0f, 0xb5, 0x44, 0x31, 0xb7, 0xcc, 0xf5, 0xc5, 0x55}


// PPI notification that CMOS_MANAGER_HOB is installed
//--------------------------------------------------------------
// {5A6A93F4-2907-4a34-BD11-6CA8A0959E09}
#define EFI_CMOS_DATA_HOB_INSTALLED_GUID \
{ 0x5a6a93f4, 0x2907, 0x4a34, 0xbd, 0x11, 0x6c, 0xa8, 0xa0, 0x95, 0x9e, 0x9 }


// Table indexes - For all tables, the first valid index is 1
//
// OptimalDefaultTable - OptimalDefaultTable[1] =  FIRST_CMOS_REGISTER
//
typedef struct _CMOS_MANAGER_HOB {
    EFI_HOB_GUID_TYPE           Header;
    CMOS_MANAGER_STATUS         ManagerStatus;
    CMOS_CHECKSUM               Checksum;
    UINT16                      FirstManagedRegister;
    UINT16                      LastManagedRegister;
    UINT16                      TokenCount;
    CMOS_TOKEN                  TokenTable[TOKEN_COUNT];  // NULL at index 0
    UINT16                      OptimalDefaultCount;
    CMOS_REGISTER               OptimalDefaultTable[MANAGED_CMOS_SIZE];
    UINT16                      NoChecksumCount;
    CMOS_REGISTER               NoChecksumTable[NO_CHECKSUM_COUNT];
    UINT16                      UnmanagedTableCount;
    CMOS_REGISTER               UnmanagedTable[TOKEN_COUNT];       // DEBUG change when AmiSsp2.exe is updated
} CMOS_MANAGER_HOB;

#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
