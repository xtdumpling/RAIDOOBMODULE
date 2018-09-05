//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SmbiosDmiEditBoard.c
    This file contains code for OEM related code for DMIEdit

**/

//----------------------------------------------------------------------------
// Includes

#include <Token.h>
#include <Efi.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>

/*
//
//  Sample code for Weybridge CRB. Needs to be changed for other platforms.
//
UINT8   Reg90Byte;      // 0F000 PAM Reg
#if ((defined(ITK_SUPPORT) && (ITK_SUPPORT != 0)) || SMBIOS_TABLE_LOCATION)
UINT8   Reg95Byte;      // 0E000-0E400 PAM Reg
UINT8   Reg96Byte;      // 0E8000-0EC00 PAM Reg
#endif
*/

/**
    ***** PORTING REQUIRED *****
    (if SMBIOS module is part of the project)

    This function enables write to F000 shadow,
    and E000 shadow if ITK_SUPPORT token
    or SMBIOS_TABLE_LOCATION token is set.

    @param None

    @retval None

**/
VOID
EnableShadowWrite (VOID)
{
/*
    //
    //
    //  Sample code for Weybridge CRB. Needs to be changed for other platforms.
    //

    UINT8 bValue;

    // 0F000 shadow
    IoWrite32(0x0CF8, 0x80000090);
    Reg90Byte = bValue = IoRead8(0x0CFC);
    bValue |= 0x30;
    IoWrite8(0x0CFC, bValue);

#if ((defined(ITK_SUPPORT) && (ITK_SUPPORT != 0)) || SMBIOS_TABLE_LOCATION)
    // 0E000-0E400 shadow
    IoWrite32(0x0CF8, 0x80000095);
    Reg95Byte = bValue = IoRead8(0x0CFD);
    bValue |= 0x33;
    IoWrite8(0x0CFD, bValue);

    // 0E800-0EC00 shadow
    IoWrite32(0x0CF8, 0x80000096);
    Reg96Byte = bValue = IoRead8(0x0CFE);
    bValue |= 0x33;
    IoWrite8(0x0CFE, bValue);
#endif
*/

#if INTEL_ARCH_SUPPORT
// Next line should be commented out for ARM platform
	NbRuntimeShadowRamWrite(TRUE);
#endif
}

/**
                     ***** PORTING REQUIRED *****
               (if SMBIOS module is part of the project)

    This function makes F000 shadow read only,
    and E000 shadow read only if ITK_SUPPORT token
    or SMBIOS_TABLE_LOCATION token is set.

    @param None

    @retval None

**/
VOID
DisableShadowWrite (VOID)
{
/*
    //
    //  Sample code for Weybridge CRB. Needs to be changed for other platforms.
    //

    // 0F000 shadow
    IoWrite32(0x0CF8, 0x80000090);
    IoWrite8(0x0CFC, Reg90Byte);

#if ((defined(ITK_SUPPORT) && (ITK_SUPPORT != 0)) || SMBIOS_TABLE_LOCATION)
    // 0E000-0E400 shadow
    IoWrite32(0x0CF8, 0x80000095);
    IoWrite8(0x0CFD, Reg95Byte);

    // 0E800-0EC00 shadow
    IoWrite32(0x0CF8, 0x80000096);
    IoWrite8(0x0CFE, Reg96Byte);
#endif
*/

#if INTEL_ARCH_SUPPORT
// Next line should be commented out for ARM platform
	NbRuntimeShadowRamWrite(FALSE);
#endif
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
