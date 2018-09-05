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
  UEFI capsule data structure definition
*/
#ifndef __CAPSULE__H__
#define __CAPSULE__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Token.h>
#include <Efi.h>

//Capsule vendor-specific GUID and variable name
#define EFI_CAPSULE_AMI_GUID \
    { 0xfac2efad, 0x8511, 0x4e34, 0x9c, 0xae, 0x16, 0xa2, 0x57, 0xba, 0x94, 0x88 }

#ifndef APTIO_FW_CAPSULE_GUID   //AMI flash update capsule
#define APTIO_FW_CAPSULE_GUID \
    { 0x4A3CA68B, 0x7723, 0x48FB, 0x80, 0x3d, 0x57, 0x8c, 0xc1, 0xfe, 0xc4, 0x4d }
#endif

#define W8_SCREEN_IMAGE_CAPSULE_GUID \
    { 0x3b8c8162, 0x188c, 0x46a4, 0xae, 0xc9, 0xbe, 0x43, 0xf1, 0xd6, 0x56, 0x97 }

#define INVALID_GUID \
    { 0x7039436b, 0x6acf, 0x433b, 0x86, 0xa1, 0x36, 0x8e, 0xc2, 0xef, 0x7e, 0x1f }

#ifdef OEM_ESRT_FIRMWARE_GUID
#define W8_FW_UPDATE_IMAGE_CAPSULE_GUID OEM_ESRT_FIRMWARE_GUID
#else
#if defined(MDEPKG_NDEBUG)
#pragma message ("!!!!! Firmware Guid for ESRT table is not defined !!!!!")
#pragma message ("!!!!! ESRT-based firmware update will not work !!!!!")
#pragma message ("Please define OEM_ESRT_FIRMWARE_GUID macro with valid value in C-source structure format")
#else
#error ("Firmware Guid for ESRT table is not defined. Please define OEM_ESRT_FIRMWARE_GUID macro with valid value in C-source structure format")
#endif
#define W8_FW_UPDATE_IMAGE_CAPSULE_GUID INVALID_GUID
#endif

#define CAPSULE_UPDATE_VAR          L"AmiCapUp"


#define MAX_SUPPORT_CAPSULE_NUM               50

typedef struct {
    EFI_EVENT CapsuleExitBootServiceEvent;
    BOOLEAN IsRuntimeMode;
} EFI_CAPSULE_RUNTIME_DATA;

#pragma pack(push, 1)
typedef struct {
    EFI_CAPSULE_HEADER Header;
    UINT8 Version;
    UINT8 Checksum;
    UINT8 ImageType;
    UINT8 Reserved;
    UINT32 Mode;
    UINT32 ImageOffsetX;
    UINT32 ImageOffsetY;
    UINT8 Image[1];
} W8_IMAGE_CAPSULE;

#pragma pack(pop)



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
