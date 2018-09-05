//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
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
// $Header: /AptioV/BIN/AMIDebugRxPkg/Include/Protocol/AMIDebugger.h 1     11/02/12 10:15a Sudhirv $
//
// $Revision: 1 $
//
// $Date: 11/02/12 10:15a $
//**********************************************************************
//*****************************************************************


#ifndef _AMI_DEBUGGER_H_
#define _AMI_DEBUGGER_H_

//
// AMIDebugger protocol {35C64E50-405B-4485-B93C-8D4181E78A00}
//
#define AMI_DEBUGGER_PROTOCOL_GUID \
  { 0x165D94BD, 0xB8B5, 0x42AC, 0xA2, 0x61, 0x44, 0x58, 0xE4, 0xFC, 0x15, 0xE0 }


//
// AMI_DEBUGGER_PROTOCOL forward reference
//
typedef
struct _AMI_DEBUGGER_PROTOCOL AMI_DEBUGGER_PROTOCOL;

//
// AMIDebugger member functions
//
typedef
EFI_STATUS
(EFIAPI *AMI_DEBUGGER_SEND_FILE_INFO) (
  IN AMI_DEBUGGER_PROTOCOL		*This,
  IN UINT64				LoadAddres,
  IN CHAR16				*FileName
  );

typedef
EFI_STATUS
(EFIAPI *AMI_DEBUGGER_SET_BREAKPOINT) (
  IN AMI_DEBUGGER_PROTOCOL		*This,
  IN UINTN				Type,
  IN UINTN				Address
  );

typedef
EFI_STATUS
(EFIAPI *AMI_DEBUGGER_SEND_DEBUG_MESSAGE_IN_ASCII) (
  IN AMI_DEBUGGER_PROTOCOL		*This,
  IN CHAR8 				*TextBuffer
  );

//
// AMIDebugger protocol definition
//
typedef
struct _AMI_DEBUGGER_PROTOCOL {
  AMI_DEBUGGER_SEND_FILE_INFO				SendFileInfo;				//Not to be used by other external drivers
  AMI_DEBUGGER_SET_BREAKPOINT				SetBreakpoint;				//Not to be used by other external drivers
  AMI_DEBUGGER_SEND_DEBUG_MESSAGE_IN_ASCII	SendDebugMessageInAscii;	//Can only be use by StatusCode driver for debug message redirection
} AMI_DEBUGGER_PROTOCOL;


#endif /* _AMI_DEBUGGER_H_ */

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
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
