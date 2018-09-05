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

/** @file PeiIpmiCmosClear.h
    Header file for PeiIpmiCmosClear PEIM

**/

#ifndef __PEI_IPMI_CMOS_CLEAR__H__
#define __PEI_IPMI_CMOS_CLEAR__H__

//----------------------------------------------------------------------

#include "Token.h"
#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>

//----------------------------------------------------------------------

extern EFI_GUID gEfiIpmiCmosClearVariableGuid;

BOOLEAN
EFIAPI
IsIpmiCmosClear ( 
  IN       EFI_PEI_SERVICES **PeiServices,
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
  );

#endif
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
