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
  Filesystem headers from specifications:
    Intel Platform Innovation Framework for EFI Firmware Volume Specification 
    Intel Platform Innovation Framework for EFI Firmware File System Specification
*/

#ifndef __FFS__H__
#define __FFS__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Efi.h>

// Include files from EDKII
// MdePkg:
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Guid/FirmwareFileSystem.h>
#include <Guid/FirmwareFileSystem2.h>


#pragma pack(push,1)

//*************************************************
// EFI_FVB_ATTRIBUTES
//*************************************************
typedef UINT32 EFI_FVB_ATTRIBUTES;
// typo in AMI definition:
#define EFI_FVB2_ALIGNMNET_512K EFI_FVB2_ALIGNMENT_512K

typedef struct {
  EFI_FIRMWARE_VOLUME_EXT_ENTRY   Hdr;
  UINT32                          TypeMask;
  EFI_GUID                        Types[1];
} EFI_FIRMWARE_VOLUME_EXT_HEADER_OEM_TYPE;

// Pi/PiFirmwareVolume.h
typedef EFI_FV_BLOCK_MAP_ENTRY FvBlockMapEntry;
#define FV_SIGNATURE EFI_FVH_SIGNATURE

//**********************************************************************
//<AMI_SHDR_START>
//
// Name:        EFI_COMPRESSION_SECTION_HEADER
//
// Description: T
//
// Fields:     Name        Type        Description
//        ------------------------------------------------------------
//
//**********************************************************************
typedef struct {
    UINT32  UncompressedLength;
    UINT8   CompressionType;
} EFI_COMPRESSION_SECTION_HEADER;

#define EFI_CUSTOMIZED_COMPRESSION  0x02


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
