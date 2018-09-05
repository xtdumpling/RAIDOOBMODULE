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
  Hii basic types definition
*/

#ifndef __UEFI_HII__H__
#define __UEFI_HII__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Efi.h>
#include <KeyboardCommonDefinitions.h>

// Include files from EDKII
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/FormBrowser2.h>
// From MdePkg
#include <Protocol/HiiFont.h>
#include <Uefi/UefiInternalFormRepresentation.h>
#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/HiiKeyBoardLayout.h>


#pragma pack (push,1)
#define EFI_GLYPH_WIDE_WIDTH	16
#define EFI_GLYPH_BASE_LINE		14
// EFI_IFR_EQ_ID_LIST_OP has been renamed to EFI_IFR_EQ_ID_VAL_LIST_OP
// starting from 5.004_MdePkg_05
#ifndef EFI_IFR_EQ_ID_LIST_OP
#ifdef EFI_IFR_EQ_ID_VAL_LIST_OP
#define EFI_IFR_EQ_ID_LIST_OP EFI_IFR_EQ_ID_VAL_LIST_OP
#else
#define EFI_IFR_EQ_ID_LIST_OP          0x14
#endif
#endif
//***************************************************
// EFI_DESCRIPTOR_STRING
//***************************************************
typedef struct {
//    CHAR16 Language[3];
    CHAR16 Space;
//    CHAR16 *DescriptionString;
} EFI_DESCRIPTION_STRING;

//***************************************************
// EFI_DESCRIPTOR_STRING_BUNDLE
//
// Example: 2ENG English Keyboard<null>SPA Keyboard en ingles<null>
// <null> = U-0000
//***************************************************
typedef struct {
    UINT16                  DescriptionCount;
    EFI_DESCRIPTION_STRING  *DescriptionString;
} EFI_DESCRIPTION_STRING_BUNDLE;

#pragma pack (pop)
/**************** Compatibility declarations *******************************/
#ifndef __STRING_REF_DEFINED__
#define __STRING_REF_DEFINED__
typedef EFI_STRING_ID STRING_REF;
#endif //__STRING_REF_DEFINED__

#define EfiKeyRshift EfiKeyRShift
typedef EFI_IFR_EQ_ID_VAL_LIST EFI_IFR_EQ_ID_LIST;
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
