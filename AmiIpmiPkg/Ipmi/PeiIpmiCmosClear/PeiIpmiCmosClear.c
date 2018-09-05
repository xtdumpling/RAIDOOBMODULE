//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PeiIpmiCmosClear.c
    Implement ELINK for IsResetConfigMode

**/

//----------------------------------------------------------------------

#include "PeiIpmiCmosClear.h"

//----------------------------------------------------------------------

/**
    Read NVRAM variable. Based on read status return boolean value.

    @param PeiServices PEI Services Variable
    @param ReadVariablePpi PPI to access NVRAM

    @return BOOLEAN
    @retval TRUE if Variable found
    @retval FALSE if variable not found

**/

BOOLEAN
EFIAPI
IsIpmiCmosClear ( 
  IN       EFI_PEI_SERVICES                **PeiServices,
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi )
{
    EFI_STATUS       Status;
    UINTN            Size;
    UINT8            Value;

    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry...\n", __FUNCTION__));
    Size = sizeof (UINT8);
    Status = ReadVariablePpi->GetVariable (
                                    ReadVariablePpi,
                                    L"IpmiCmosClear",
                                    &gEfiIpmiCmosClearVariableGuid,
                                    NULL,
                                    &Size,
                                    &Value );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "ReadVariablePpi->GetVariable Status %r  \n", Status));
    if ( EFI_ERROR (Status)) {
        return FALSE;
    }
    SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Exiting...\n", __FUNCTION__));
    return TRUE;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
