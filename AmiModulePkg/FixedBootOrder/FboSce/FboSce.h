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

/** @file FboSce.h
    Header file for FboSec module.
**/

#ifndef __FBO_SCE__H__
#define __FBO_SCE__H__
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

typedef struct {
    UINT32 Type;
    UINT16 Length;
} FBO_SCE_HEADER;

typedef struct {
    FBO_SCE_HEADER Header;
    UINT16 Index[1];
} FBO_SCE_ORDER;

#pragma pack()

typedef struct {
    FIXED_BOOT_SETUP*  FixedBoot;
    UINTN              FixedBootSize;
    FBO_SCE_ORDER*     SceOrder;
    UINTN              SceOrderSize;
    UINT16             IsUefi;
} FBO_SCE_VAR;

#define SCEORDER_COUNT(Order) ((Order->Header.Length - sizeof(FBO_SCE_HEADER)) / sizeof(Order->Index))


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
