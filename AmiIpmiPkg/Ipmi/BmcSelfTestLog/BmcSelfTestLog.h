//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file BmcSelfTestLog.h
    Header file for BmcSeltTestLog module

**/

#ifndef __BMC_SELF_TEST_LOG_PAGE_H__
#define __BMC_SELF_TEST_LOG_PAGE_H__
#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------

#include "Token.h"
#include <ServerMgmtSetupVariable.h>
#include <ServerMgmtSetupStrDefs.h>
#include <ServerMgmtSetup.h>
#include <Protocol/IPMISelfTestLogProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>

//----------------------------------------------------------------------

//
// Macro definitions
//
typedef UINT16  STRING_REF;

#define STRING_TOKEN(t)             t
#define VAR_FLAG_ATTRIBUTES         EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                                    EFI_VARIABLE_NON_VOLATILE

//
// Function Prototype
//
VOID InitString(EFI_HII_HANDLE HiiHandle, STRING_REF StrRef, CHAR16 *sFormat, ...);

//
// Default values for setup data
//
#define ERASE_ON_EVERY_RESET    0x01
#define CLEAR_LOG               0x01
#define DONOT_LOG_ANY_MORE      0x00

#define STRING_BUFFER_LENGTH    4000
//
// Self Test Log structure related definitions
//
#define BST_LOG_SIZE 20

#pragma pack(1)

#if TSE_MULTILINE_CONTROLS == 1
typedef struct {
  UINT16  Year;
  UINT8   Month;
  UINT8   Day;
}DATE;

typedef struct {
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;
}TIME;

typedef struct {
  UINT8 logSize;
  UINT8 log[BST_LOG_SIZE];
  DATE  date[BST_LOG_SIZE];
  TIME  time[BST_LOG_SIZE];
} BMC_SELF_TEST_LOG_DATA;

#else //#if TSE_MULTILINE_CONTROLS == 1

typedef struct {
    UINT8 logSize;
    UINT8 log[BST_LOG_SIZE];
} BMC_SELF_TEST_LOG_DATA;

#endif

#pragma pack()

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
