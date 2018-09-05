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
  ROM Layout structure type and related definitions
*/

#ifndef __ROM_LAYOUT__H__
#define __ROM_LAYOUT__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Efi.h>

#pragma pack(push, 1)

typedef enum {
    RomAreaTypeFv,
    RomAreaTypeRaw,
    RomAreaTypeMax
} ROM_AREA_TYPE;

typedef struct {
    EFI_PHYSICAL_ADDRESS Address;
    UINT32 Offset;
    UINT32 Size;
    ROM_AREA_TYPE Type;
    UINT32 Attributes;
} ROM_AREA;

#pragma pack(pop)

// The FV will be published early in PEI
#define ROM_AREA_FV_PEI               0x00000002
// The FV will be published in PEI after the memory detection
#define ROM_AREA_FV_PEI_MEM           0x00000004
// The FV will be published in DXE IPL Entry prior to transition to DXE. 
// The FV published in DXE IPL are not processed by the PEI dispatcher. 
// The FV is visible to DXE IPL, which means it can contain the DXE Core.
#define ROM_AREA_FV_DXE               0x00000008
// Reserved for future use
#define ROM_AREA_FV_BDS               0x00000010
// The FV will be published in PEI after the memory detection 
// (just like ROM_AREA_FV_PEI_MEM). 
// The FV will be copied to memory prior to the publishing.
#define ROM_AREA_FV_PEI_SHADOW        0x00000020

// The FV is processed by the PEI dispatcher if either 
//  ROM_AREA_FV_PEI, or ROM_AREA_FV_PEI_MEM, or ROM_AREA_FV_PEI_SHADOW 
//  attribute is defined.
// The FV is processed by the DXE dispatcher if ROM_AREA_FV_DXE attribute is defined.

#define ROM_AREA_FV_VERIFY            0x00000040
#define ROM_AREA_FV_NON_CRITICAL      0x00000080
#define ROM_AREA_FV_NFV_PRESENT       0x00000100
#define ROM_AREA_FV_SIGNED            0x00000200

// This area is required to perform a firmware recovery. 
// The firmware recovery will not work if this area is corrupted.
#define ROM_AREA_VITAL                0x00000400

#define ROM_AREA_FV_PEI_ACCESS (ROM_AREA_FV_PEI | ROM_AREA_FV_PEI_MEM | ROM_AREA_FV_PEI_SHADOW)

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
