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
/** @file
  Interface with the input devices in PEI
*/

#ifndef __AMI_AMIKEYCODE_PPI_H__
#define __AMI_AMIKEYCODE_PPI_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "KeyboardCommonDefinitions.h"

//
//Guid for Normal AmiKeycodePPI ( Ex: Ps2 and USB)
//
// {20B0F1C2-B0D8-4c5d-AAD9-F44580DFDF8B}
#define EFI_PEI_AMI_KEYCODE_PPI_GUID \
    {0x20b0f1c2, 0xb0d8, 0x4c5d, 0xaa, 0xd9, 0xf4, 0x45, 0x80, 0xdf, 0xdf, 0x8b}

//
// Guid for PEI Consplitter AmiKeyCodePPI
//
// {73DDB5E1-5FB4-4751-AF1E-83CF75BECBB6}
#define EFI_PEI_CONSPLIT_AMI_KEYCODE_PPI_GUID \
    {0x73ddb5e1, 0x5fb4, 0x4751, 0xaf, 0x1e, 0x83, 0xcf, 0x75, 0xbe, 0xcb, 0xb6}

GUID_VARIABLE_DECLARATION(gPeiAmikeycodeInputPpiGuid,EFI_PEI_AMI_KEYCODE_PPI_GUID);

GUID_VARIABLE_DECLARATION(gPeiConsplitAmikeycodeInputPpiGuid,EFI_PEI_CONSPLIT_AMI_KEYCODE_PPI_GUID);

typedef struct {
    EFI_INPUT_KEY       Key;
    EFI_KEY_STATE       KeyState;
    EFI_KEY             EfiKey;
    UINT8               PS2ScanCode;
} EFI_PEI_AMIKEYCODE_DATA;

typedef struct _EFI_PEI_AMIKEYCODE_PPI EFI_PEI_AMIKEYCODE_PPI;

typedef EFI_STATUS (EFIAPI * EFI_PEI_AMIKEYCODE_READKEY) (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_AMIKEYCODE_PPI      *This,
    OUT EFI_PEI_AMIKEYCODE_DATA     *KeyData
);

typedef EFI_STATUS (EFIAPI * EFI_PEI_AMIKEYCODE_SETLEDSTATE) (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_AMIKEYCODE_PPI      *This,
    IN  EFI_KEY_TOGGLE_STATE        KeyToggleState
);

struct _EFI_PEI_AMIKEYCODE_PPI {
    EFI_PEI_AMIKEYCODE_READKEY      ReadKey;
    EFI_PEI_AMIKEYCODE_SETLEDSTATE  SetLedState;
};

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
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
