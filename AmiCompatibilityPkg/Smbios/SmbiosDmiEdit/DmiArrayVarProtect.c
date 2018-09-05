//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

/** @file DmiArrayVarProtect.c
    This file provides worker functions to support DmiArrayVar protection feature

**/

#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/AmiSmbios.h>

EFI_GUID AmiSmbiosNvramGuid = { 0x4b3082a3, 0x80c6, 0x4d7e, { 0x9c, 0xd0, 0x58, 0x39, 0x17, 0x26, 0x5d, 0xf1 } };

EFI_STATUS
DmiArrayVarCheck(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
  if ((!StrCmp (VariableName, L"DmiArray")) &&
      (!guidcmp (VendorGuid, &AmiSmbiosNvramGuid)) &&
      DataSize != (DMI_ARRAY_COUNT * sizeof(DMI_VAR)))
  {
    return EFI_WRITE_PROTECTED;
  }

  return EFI_UNSUPPORTED;
}

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
