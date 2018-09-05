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

/** @file AmiUsbCcid.h
    AMI USB CCID Protocol definition

**/

#ifndef _USB_CCID_H
#define _USB_CCID_H

#include <Efi.h>

#ifndef GUID_VARIABLE_DEFINITION

// CCID definitions/Structures
typedef struct _AMI_CCID_IO_PROTOCOL AMI_CCID_IO_PROTOCOL;

#define AMI_CCID_IO_PROTOCOL_GUID \
  { 0xB167C2F5, 0xE26A, 0x4DFF, 0x8E, 0x1C, 0x08, 0x07, 0xC7, 0xF0, 0x2A, 0x88}

#define AMI_CCID_PRESENCE_GUID \
  { 0x5FDEE00D, 0xDA40, 0x405A, 0xB9, 0x2E, 0xCF, 0x4A, 0x80, 0xEA, 0x8F, 0x76}

typedef enum {
  I_BLOCK,
  S_IFS_REQUEST = 0xC1,
  S_ABORT_REQUEST,
  S_WTX_REQUEST    
} ISBlock;

typedef
EFI_STATUS
(EFIAPI *AMI_CCID_SMARTCLASSDESCRIPTOR) (
    IN AMI_CCID_IO_PROTOCOL             *This,
    OUT UINT8                           *ResponseBuffer
);

typedef
EFI_STATUS
(EFIAPI *AMI_CCID_GET_ATR) (
    IN AMI_CCID_IO_PROTOCOL             *This,
    IN UINT8                            Slot,
    OUT UINT8                           *ATRData
);

typedef
EFI_STATUS
(EFIAPI *AMI_CCID_POWERUP_SLOT) (
    IN AMI_CCID_IO_PROTOCOL             *This,
    OUT UINT8                           *bStatus,
    OUT UINT8                           *bError,
    OUT UINT8                           *ATRData
);

typedef
EFI_STATUS
(EFIAPI *AMI_CCID_POWERDOWN_SLOT) (
    IN AMI_CCID_IO_PROTOCOL             *This,
    OUT UINT8                           *bStatus,
    OUT UINT8                           *bError
);

typedef
EFI_STATUS
(EFIAPI *AMI_CCID_GET_SLOT_STATUS) (
    IN AMI_CCID_IO_PROTOCOL              *This,
    OUT UINT8                            *bStatus,
    OUT UINT8                            *bError,
    OUT UINT8                            *bClockStatus
);

typedef
EFI_STATUS
(EFIAPI *AMI_CCID_XFR_BLOCK) (
    IN AMI_CCID_IO_PROTOCOL             *This,
    IN UINTN                            CmdLength,
    IN UINT8                            *CmdBuffer,
    IN UINT8                            ISBlock,
    OUT UINT8                           *bStatus,
    OUT UINT8                           *bError,
    IN OUT UINTN                        *ResponseLength,
    OUT UINT8                           *ResponseBuffer
);

typedef
EFI_STATUS
(EFIAPI *AMI_CCID_GET_PARAMETERS) (
    IN AMI_CCID_IO_PROTOCOL              *This,
    OUT UINT8                            *bStatus,
    OUT UINT8                            *bError,
    OUT UINTN                            *ResponseLength,
    OUT UINT8                            *ResponseBuffer
);

typedef struct _AMI_CCID_IO_PROTOCOL {

    AMI_CCID_SMARTCLASSDESCRIPTOR       USBCCIDAPISmartClassDescriptor;
    AMI_CCID_GET_ATR                    USBCCIDAPIGetAtr;
    AMI_CCID_POWERUP_SLOT               USBCCIDAPIPowerupSlot;    
    AMI_CCID_POWERDOWN_SLOT             USBCCIDAPIPowerDownSlot;    
    AMI_CCID_GET_SLOT_STATUS            USBCCIDAPIGetSlotStatus;
    AMI_CCID_XFR_BLOCK                  USBCCIDAPIXfrBlock;
    AMI_CCID_GET_PARAMETERS             USBCCIDAPIGetParameters;    
        
};

typedef struct {
    AMI_CCID_IO_PROTOCOL        CCIDIoProtocol;
    VOID                        *DevInfo;
    UINT8                       Slot;
    EFI_HANDLE                  ControllerHandle;
    EFI_HANDLE                  ChildHandle;

} USB_ICC_DEV;

#endif
#endif // _USB_CCID_H

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
