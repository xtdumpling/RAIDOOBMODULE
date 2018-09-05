//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2011, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Inc/TseCommon.h $
//
// $Author: Rajashakerg $
//
// $Revision: 5 $
//
// $Date: 11/20/11 6:38a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file TseCommon.h
    Tse Common Helper header file

**/

#ifndef _TSECOMMON_H_
#define _TSECOMMON_H_

#include "HiiLib.h"
#include "mem.h"
#include "variable.h"
#include "PwdLib.h"

EFI_STATUS
EfiLibReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     Type,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId OPTIONAL,
  IN EFI_STATUS_CODE_DATA     *Data     OPTIONAL  
  );

VOID *
EfiLibAllocateZeroPool (
  IN  UINTN   AllocationSize
  );

extern INTN
EfiCompareMem (
  IN VOID     *MemOne,
  IN VOID     *MemTwo,
  IN UINTN    Len
  );

UINTN
SPrint (
  OUT CHAR16        *Buffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR16  *Format,
  ...
  );

EFI_STATUS ShowPostMsgBox(IN CHAR16  *MsgBoxTitle,IN CHAR16  *Message,IN UINT8  MsgBoxType, UINT8 *pSelection);

VOID    CheckForKeyHook( EFI_EVENT Event, VOID *Context );
VOID CheckForClickHook( EFI_EVENT Event, VOID *Context );
VOID    ClearScreen( UINT8 Attrib );
VOID    *SaveScreen( VOID );
VOID ClearFlushBuffer(UINT8 Attrib);

extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;

//extern EFI_GUID  gEfiDevicePathProtocolGuid;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 2011, American Megatrends, Inc.             **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093       **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
