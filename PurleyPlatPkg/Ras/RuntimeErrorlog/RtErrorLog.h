//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//
//*****************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		RtErrorLog.h
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _RT_ERRORLOG_H
#define _RT_ERRORLOG_H

// This RUNTIME_ERROR_DEBUG_MESSAGE define is only for internal code testing purposes.
// Please do not change define to 1
// Intel EDK DEBUG Macro is not supported at runtime.
// When we enable this, observed problems while doing UEFI OS Boot.

#define RUNTIME_ERROR_DEBUG_MESSAGE 0

EFI_STATUS
CommonErrorHandling (
  VOID  *ErrStruc
  );

#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
