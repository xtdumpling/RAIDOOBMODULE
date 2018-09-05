//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file PeiVideoInit.c
    LIB driver for initializing the Video in PEI 

**/

#include <Include/AmiLib.h>
#include <Library/DebugLib.h>

/**
    Initializes video controller with VGA standard init. 

    @param   None 

    @retval  EFI_NOT_FOUND
**/
EFI_STATUS 
AmiVideoInit(
    VOID
)
{
    // Vga Init Code
    // Porting Required: Call the Video Binary Init code Entry Point from here.
    //
    return EFI_NOT_FOUND;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
