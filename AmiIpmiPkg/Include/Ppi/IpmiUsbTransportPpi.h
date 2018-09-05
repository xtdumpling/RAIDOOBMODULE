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

/** @file IpmiUsbTransportPpi.h
    IPMI USB transport PPI definitions

**/

#ifndef _IPMI_USB_TRANSPORT_PPI_H_
#define _IPMI_USB_TRANSPORT_PPI_H_

#define PEI_IPMI_USB_TRANSPORT_PPI_GUID \
  { \
    0xcbdd2397, 0xd2c5, 0x415b, { 0xbd, 0x77, 0x96, 0x30, 0xa1, 0xb7, 0x85, 0x3d } \
  }

//
// Forward declaration
//
typedef struct _PEI_IPMI_USB_TRANSPORT_PPI PEI_IPMI_USB_TRANSPORT_PPI;

//
// Defined to use in common header file
//
typedef PEI_IPMI_USB_TRANSPORT_PPI  IPMI_USB_TRANSPORT;

//
// Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_IPMI_USB_SEND_COMMAND) (
  IN  PEI_IPMI_USB_TRANSPORT_PPI        *This,
  IN  UINT8                             NetFunction,
  IN  UINT8                             Lun,
  IN  UINT8                             Command,
  IN  UINT8                             *CommandData,
  IN  UINT8                             CommandDataSize,
  OUT UINT8                             *ResponseData,
  OUT UINT8                             *ResponseDataSize );

typedef
EFI_STATUS
(EFIAPI *EFI_IPMI_USB_OEM_SEND_COMMAND) (
  IN  PEI_IPMI_USB_TRANSPORT_PPI         *This,
  IN  UINT8                              NetFunction,
  IN  UINT8                              Lun,
  IN  UINT8                              Command,
  IN  UINT8                              *CommandData,
  IN  UINT32                             CommandDataSize,
  OUT UINT8                              *ResponseData,
  OUT UINT32                             *ResponseDataSize );


#pragma pack(1)

/**
  IPMI Usb Transport PPI
*/
typedef struct _PEI_IPMI_USB_TRANSPORT_PPI {
  UINT8                         CommandCompletionCode;
  EFI_IPMI_USB_SEND_COMMAND     SendIpmiCommand;
  EFI_IPMI_USB_OEM_SEND_COMMAND SendOemIpmiCommand;
} PEI_IPMI_USB_TRANSPORT_PPI;

#pragma pack()

extern EFI_GUID gEfiPeiIpmiUsbTransportPpiGuid;

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
