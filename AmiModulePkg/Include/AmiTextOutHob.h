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

/** @file AmiVideoCtrlHob.h
    AMI defined header file for Text Out HOB.

**/

#ifndef __AMI_TEXT_OUT_HOB__H__
#define __AMI_TEXT_OUT_HOB__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <Token.h>

#define AMI_TEXT_OUT_HOB_GUID \
{0xc50cee43, 0x1926, 0x465b, 0x8a, 0x91, 0xb8, 0x8, 0x5c, 0xb0, 0xa1, 0xd7}

typedef
struct _AMI_TEXT_OUT_HOB {
    EFI_HOB_GUID_TYPE   Header;
    UINT8               UpperCursorRow;
    UINT8               UpperCursorCol;
    UINT8               LowerCursorRow;
    UINT8               LowerCursorCol;
    BOOLEAN             NextLine;
} AMI_TEXT_OUT_HOB;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
