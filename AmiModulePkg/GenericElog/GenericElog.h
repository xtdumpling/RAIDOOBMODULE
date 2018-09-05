//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header:  $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  GenericElog.h
//
// Description:	Baseboard Event Log functions.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef _EFI_GENERICELOG_H_
#define _EFI_GENERICELOG_H_

#include "Token.h"

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/GenericElogProtocol.h>

//
// BMC Elog instance data
//
#pragma pack(1)
typedef struct {
  EFI_SM_ELOG_PROTOCOL		  GenericElogProtocol;
  UINT8				          NumberOfRedirects;
  EFI_SM_ELOG_REDIR_PROTOCOL* RedirectProtocolList[EfiSmElogMax];
} GENERIC_ELOG_LOCAL_STRUCT;
#pragma pack()

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
