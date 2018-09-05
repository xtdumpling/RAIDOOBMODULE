//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file IpmiUsbTransportProtocol.h
    IPMI USB Protocol definitions

**/

#ifndef _EFI_IPMI_USB_TRANSPORT_PROTOCOL_H
#define _EFI_IPMI_USB_TRANSPORT_PROTOCOL_H

#define EFI_IPMI_USB_TRANSPORT_PROTOCOL_GUID \
  { \
    0x42d353b0, 0x8c1e, 0x4aca, { 0xbe, 0xac, 0x28, 0xfb, 0x24, 0xf2, 0x0c, 0x42 } \
  }

typedef struct _EFI_IPMI_USB_TRANSPORT EFI_IPMI_USB_TRANSPORT;

//
//  IPMI USB Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_IPMI_USB_SEND_COMMAND) (
  IN  EFI_IPMI_USB_TRANSPORT          *This,
  IN  UINT8                           NetFunction,
  IN  UINT8                           Lun,
  IN  UINT8                           Command,
  IN  UINT8                           *CommandData,
  IN  UINT8                           CommandDataSize,
  OUT UINT8                           *ResponseData,
  OUT UINT8                           *ResponseDataSize
  );

typedef
EFI_STATUS
(EFIAPI *EFI_IPMI_USB_OEM_SEND_COMMAND) (
  IN  EFI_IPMI_USB_TRANSPORT            *This,
  IN  UINT8                             NetFunction,
  IN  UINT8                             Lun,
  IN  UINT8                             Command,
  IN  UINT8                             *CommandData,
  IN  UINT32                            CommandDataSize,
  OUT UINT8                             *ResponseData,
  OUT UINT32                            *ResponseDataSize
  );

typedef struct _EFI_IPMI_USB_TRANSPORT {
  UINT8                         CommandCompletionCode;
  EFI_IPMI_USB_SEND_COMMAND     SendIpmiCommand;
  EFI_IPMI_USB_OEM_SEND_COMMAND SendOemIpmiCommand;
} IPMI_USB_TRANSPORT;

extern EFI_GUID gEfiDxeIpmiUsbTransportProtocolGuid;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
