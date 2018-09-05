//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix: Initial revision
//    Reason : Implement a protocol for printing strings from DXE on.
//    Auditor: Yenhsin Cho
//    Date   : May/27/2016
//
//****************************************************************************

#ifndef _SMC_SIMPLETEXTOUT_PROTOCOL_H_
#define _SMC_SIMPLETEXTOUT_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif
/****** START TO WRITE BELOW THIS LINE *******/

#ifndef __USE_EDK_LIB
#include <EFI.h>
#endif

// {20160517-8bc9-4920-a9b5-35205889b917}
#define SMC_SIMPLETEXTOUT_PROTOCOL_GUID \
  { 0x20160517, 0x8bc9, 0x4920, 0xa9, 0xb5, 0x35, 0x20, 0x58, 0x89, 0xb9, 0x17 }

static EFI_GUID  gSmcSimpleTextOutProtocolGuid = SMC_SIMPLETEXTOUT_PROTOCOL_GUID;
// GUID_VARIABLE_DECLARATION(gSmcSimpleTextOutProtocolGuid, SMC_SIMPLETEXTOUT_PROTOCOL_GUID);

//////////////////////////////////////////
// SMC SimpleTextOut protocol structure //
//////////////////////////////////////////

typedef struct _SMC_SIMPLETEXTOUT_PROTOCOL SMC_SIMPLETEXTOUT_PROTOCOL;

typedef
VOID
(*SMC_SIMPLETEXTOUT_PROTOCOL_OPENVGADECODE) (
  VOID
  );

typedef
VOID
(*SMC_SIMPLETEXTOUT_PROTOCOL_CLOSEVGADECODE) (
  VOID
  );

typedef
VOID
(*SMC_SIMPLETEXTOUT_PROTOCOL_ENABLECURSOR) (
  IN  BOOLEAN  Enabled
  );

typedef
VOID
(*SMC_SIMPLETEXTOUT_PROTOCOL_CLEARLINE) (
  IN  UINT8  Yaxis
  );

typedef
VOID
(*SMC_SIMPLETEXTOUT_PROTOCOL_CLEARSCREEN) (
  VOID
  );

typedef
VOID
(*SMC_SIMPLETEXTOUT_PROTOCOL_WRITECHAR) (
  IN  UINT8  Xaxis,
  IN  UINT8  Yaxis,
  IN  UINT8  Attrib,
  IN  UINT8  Char
  );

typedef
UINT8
(*SMC_SIMPLETEXTOUT_PROTOCOL_OUTPUTSTRING) (
  IN  UINT8  Xaxis,
  IN  UINT8  Yaxis,
  IN  UINT8  Attrib,
  IN  CHAR8  *OutString
  );

typedef
VOID
(*SMC_SIMPLETEXTOUT_PROTOCOL_POSTPROGRESSMESSAGE) (
  IN  UINT8  Yaxis,
  IN  CHAR8  *OutString
  );

typedef struct _SMC_SIMPLETEXTOUT_PROTOCOL
{
  SMC_SIMPLETEXTOUT_PROTOCOL_OPENVGADECODE        OpenVgaDecode;
  SMC_SIMPLETEXTOUT_PROTOCOL_CLOSEVGADECODE       CloseVgaDecode;
  SMC_SIMPLETEXTOUT_PROTOCOL_ENABLECURSOR         EnableCursor;
  SMC_SIMPLETEXTOUT_PROTOCOL_WRITECHAR            WriteChar;
  SMC_SIMPLETEXTOUT_PROTOCOL_CLEARLINE            ClearLine;
  SMC_SIMPLETEXTOUT_PROTOCOL_CLEARSCREEN          ClearScreen;
  SMC_SIMPLETEXTOUT_PROTOCOL_OUTPUTSTRING         OutputString;
  SMC_SIMPLETEXTOUT_PROTOCOL_POSTPROGRESSMESSAGE  PostProgressMessage;
} SMC_SIMPLETEXTOUT_PROTOCOL;


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif  // _SMC_SIMPLETEXTOUT_PROTOCOL_H_
