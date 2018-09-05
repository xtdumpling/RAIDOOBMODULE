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
  @brief ROM Layout GUID Definitions.

  The file contains definitions of the GUID associated with the ROM Layout infrastructure.
**/

#ifndef __AMI_ROM_LAYOUT_GUID__H__
#define __AMI_ROM_LAYOUT_GUID__H__
#include <AmiRomLayout.h>

/// The GUID of the FFS file containing AMI ROM Layout.
#define AMI_ROM_LAYOUT_FFS_FILE_GUID \
    { 0xc91c3c17, 0xfc74, 0x46e5, { 0xbd, 0xbe, 0x6f, 0x48, 0x6a, 0x5a, 0x9f, 0x3c } }

/// @brief This GUID is used to identify ROM Layout data.
///
/// The GUID is conjunction with the following data structure to identify ROM Layout data:
///   - FFS Section GUID (section of type EFI_SECTION_FREEFORM_SUBTYPE_GUID)
///   - ROM Layout HOB GUID
///   - ROM Layout SMM Configuration Table GUID
#define AMI_ROM_LAYOUT_GUID \
    { 0x00781ca1, 0x5de3, 0x405f, { 0xab, 0xb8, 0x37, 0x9c, 0x3c, 0x7, 0x69, 0x84 } }

typedef struct {
	EFI_HOB_GUID_TYPE Header;
    UINT32 Version;             //< Version from the AMI_ROM_LAYOUT_HEADER structure
    UINT32 DescriptorSize;      //< DescriptorSize from the AMI_ROM_LAYOUT_HEADER structure
    UINT32 HobEntrySize;        //< Total size of each AMI_ROM_LAYOUT_HOB_ENTRY structure
    UINT32 Reserved;            //< Reserved for future use
//  AMI_ROM_LAYOUT_HOB_ENTRY Descriptors[...];
} AMI_ROM_LAYOUT_HOB;

typedef struct {
    UINT32 Offset;              //< Offset from the start of the AMI_ROM_LAYOUT_HOB
    UINT32 Reserved;            //< Reserved for future use
    AMI_ROM_AREA Area;          //< Rom Information retrieved from firmware volume
} AMI_ROM_LAYOUT_HOB_ENTRY;

typedef struct {
	UINT32 Size;
    UINT32 Version;
    UINT32 DescriptorSize;
    UINT32 Reserved;
//  AMI_ROM_AREA Descriptors[...];
} AMI_ROM_LAYOUT_CONFIG_TABLE;

/// see ::AMI_ROM_LAYOUT_FFS_FILE_GUID
extern EFI_GUID gAmiRomLayoutFfsFileGuid;
/// see ::AMI_ROM_LAYOUT_GUID
extern EFI_GUID gAmiRomLayoutGuid;

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
