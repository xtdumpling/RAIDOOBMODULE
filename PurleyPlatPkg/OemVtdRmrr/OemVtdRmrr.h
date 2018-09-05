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

//**********************************************************************
// $Header:  $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  OemVtdRmrr.h
//
// Description: Oem Vtd Rmrr definitions
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef _OEM_VTD_RMRR_H_
#define _OEM_VTD_RMRR_H_

#include "OemVtdRmrrHook.h"

#define OEM_VTD_RMMR_DEBUG_SUPPORT  1

#if OEM_VTD_RMMR_DEBUG_SUPPORT
#define OEM_RMMR_DEBUG(Arguments)  DEBUG(Arguments)
#else
#define OEM_RMMR_DEBUG(Arguments)
#endif

typedef EFI_STATUS (OEM_VTD_RMRR_HOOK)(
  IN OEM_VTD_RMRR_FUNC_NUMBER FuncNumber,
  IN VOID                     *Ptr );

EFI_STATUS
InsertOemVtdRmrr (
  IN VOID *DmaRemap );

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
