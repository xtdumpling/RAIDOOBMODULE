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
  @brief The file contains definition of the ROM Layout protocol.
**/
#ifndef __AMI_ROM_LAYOUT_PROTOCOL__H__
#define __AMI_ROM_LAYOUT_PROTOCOL__H__
#include <AmiRomLayout.h>

#define AMI_ROM_LAYOUT_PROTOCOL_GUID \
    { 0xaabe9a45, 0xb345, 0x49d2, { 0x88, 0xef, 0x6a, 0xd0, 0xb8, 0x5e, 0xd7, 0xe6 } }

typedef struct _AMI_ROM_LAYOUT_PROTOCOL AMI_ROM_LAYOUT_PROTOCOL;

typedef EFI_STATUS (EFIAPI *AMI_ROM_LAYOUT_GET_ROM_LAYOUT)(
    IN AMI_ROM_LAYOUT_PROTOCOL *This,
    IN OUT UINT32 *BufferSize, IN OUT AMI_ROM_AREA *RomLayoutBuffer,
    OUT UINT32 *DescriptorSize, OUT UINT32 *DescriptorVersion OPTIONAL
);

typedef EFI_STATUS (EFIAPI *AMI_ROM_LAYOUT_PUBLISH_FV)(
    IN AMI_ROM_LAYOUT_PROTOCOL *This, IN EFI_GUID *FvName
);

typedef EFI_STATUS (EFIAPI *AMI_ROM_LAYOUT_PUBLISH_FV_AREA)(
    IN AMI_ROM_LAYOUT_PROTOCOL *This, IN AMI_ROM_AREA *FvArea
);

struct _AMI_ROM_LAYOUT_PROTOCOL{
	AMI_ROM_LAYOUT_GET_ROM_LAYOUT GetRomLayout;
	AMI_ROM_LAYOUT_PUBLISH_FV PublishFv;
	AMI_ROM_LAYOUT_PUBLISH_FV_AREA PublishFvArea;
};

extern EFI_GUID AmiRomLayoutProtocolGuid;
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
