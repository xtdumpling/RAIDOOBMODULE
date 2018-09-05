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

/** @file AmiUsbSmartCardReader.h
    Header file contains Smart Card reader Protocol API function declaration

**/

#ifndef _USB_SMART_CARD_READER_H_
#define _USB_SMART_CARD_READER_H_

#if defined(MDE_PKG_VERSION) && (MDE_PKG_VERSION>=10)
    #include "Protocol/SmartCardReader.h"
#else
    #include "Protocol/UsbSmartCardReaderProtocol.h"
#endif

typedef struct {
    EFI_SMART_CARD_READER_PROTOCOL   EfiSmartCardReaderProtocol;
    VOID                             *DevInfo;
    UINT8                            Slot;
    UINT8                            AccessMode;
    EFI_HANDLE                       ControllerHandle;
    EFI_HANDLE                       ChildHandle;
} USB_SCARD_DEV;

EFI_STATUS
EFIAPI
USBSCardReaderAPIConnect(
    IN EFI_SMART_CARD_READER_PROTOCOL *This,
    IN UINT32                         AccessMode,
    IN UINT32                         CardAction,
    IN UINT32                         PreferredProtocol,
    OUT UINT32                        *ActiveProtocol
);

EFI_STATUS
EFIAPI
USBSCardReaderAPIDisconnect(
    IN EFI_SMART_CARD_READER_PROTOCOL *This,
    IN UINT32                         CardAction
);

EFI_STATUS
EFIAPI
USBSCardReaderAPIStatus(
    IN EFI_SMART_CARD_READER_PROTOCOL *This,
    OUT CHAR16                        *ReaderName,
    IN OUT UINTN                      *ReaderNameLength,
    OUT UINT32                        *State,
    OUT UINT32                        *CardProtocol,
    OUT UINT8                         *Atr,
    IN OUT UINTN                      *AtrLength
);

EFI_STATUS
EFIAPI
USBSCardReaderAPITransmit(
    IN EFI_SMART_CARD_READER_PROTOCOL *This,
    IN UINT8                          *CAPDU,
    IN UINTN                          CAPDULength,
    OUT UINT8                         *RAPDU,
    IN OUT UINTN                      *RAPDULength
);

EFI_STATUS
EFIAPI
USBSCardReaderAPIControl(
    IN EFI_SMART_CARD_READER_PROTOCOL *This,
    IN UINT32                         ControlCode,
    IN UINT8                          *InBuffer,
    IN UINTN                          *InBufferLength,
    OUT UINT8                         *OutBuffer,
    IN OUT UINTN                      *OutBufferLength
);

EFI_STATUS
EFIAPI
USBSCardReaderAPIGetAttrib(
    IN EFI_SMART_CARD_READER_PROTOCOL *This,
    IN UINT32                         Attrib,
    OUT UINT8                         *OutBuffer,
    IN OUT UINTN                      *OutBufferLength
);

EFI_STATUS
GetSmartCardReaderName (
    EFI_HANDLE    ControllerHandle,
    CHAR16        *ReaderName,
    UINTN         *ReaderNameLength
);

#endif // _USB_SMART_CARD_READER_H_

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
