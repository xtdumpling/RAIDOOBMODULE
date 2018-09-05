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

/** @file
    USB 3.0 Interface Association Descriptor(IAD) protocol definitions.
**/

#ifndef __AMI_USB_IAD_PROTOCOL__H__
#define __AMI_USB_IAD_PROTOCOL__H__

#define AMI_USB_IAD_PROTOCOL_GUID \
  { 0xa18976f, 0x6ca6, 0x405b, { 0xaf, 0x4b, 0x27, 0xb8, 0xf7, 0xf6, 0xdb, 0x98 } }

typedef struct _AMI_USB_INTERFACE_ASSOCIATION_DESCRIPTOR {
    UINT8   bLength;
    UINT8   bDescriptorType;
    UINT8   bFirstInterface;
    UINT8   bInterfaceCount;
    UINT8   bFunctionClass;
    UINT8   bFunctionSubclass;
    UINT8   bFunctionProtocol;
    UINT8   iFunction;
} AMI_USB_INTERFACE_ASSOCIATION_DESCRIPTOR;

typedef struct _AMI_USB_IAD_PROTOCOL AMI_USB_IAD_PROTOCOL;

/**
  Returns Interface Association Descriptor(IAD).

  @param This   A pointer to the protocol instance.
  @param Iad    A pointer to the caller allocated buffer where IAD is returned.

  @return EFI_STATUS
**/
typedef
EFI_STATUS
(EFIAPI *AMI_USB_IAD_GET_IAD)(
  IN AMI_USB_IAD_PROTOCOL                       *This,
  OUT AMI_USB_INTERFACE_ASSOCIATION_DESCRIPTOR  *Iad
  );

/**
  Returns USB I/O handle associated with the IAD.

  @param This          A pointer to the protocol instance.
  @param UsbIoIndex    Index of the handle to return. Pass zero to get the first handle.
  @param Handle        A pointer to a handle to return.

  @retval EFI_SUCCESS    The handle has been returned.
  @retval EFI_NOT_FOUND  UsbIoIndex exceeds the number of handles associated with the IAD.
**/
typedef
EFI_STATUS
(EFIAPI *AMI_USB_IAD_GET_USB_HANDLE)(
  IN AMI_USB_IAD_PROTOCOL   *This,
  IN UINT8                  UsbIoIndex,
  OUT EFI_HANDLE            *Handle
  );

/**
  Returns IAD interface data.

  @param This          A pointer to the protocol instance.
  @param UsbIoIndex    Index of the USB I/O handle to return the interface data for. Pass zero to get the first handle.
  @param DataSize      A pointer to a data size. On input, size of the passed in Data buffer. On output, size of the returned data.
  @param Data          A caller allocated returned data buffer.

  @retval EFI_SUCCESS           The handle has been returned.
  @retval EFI_NOT_FOUND         UsbIoIndex exceeds the number of handles associated with the IAD.
  @retval EFI_BUFFER_TOO_SMALL  Passed in buffer is not large enough to accommodate the data. 
                                DataSize is updated with the required buffer size.
**/
typedef
EFI_STATUS
(EFIAPI *AMI_USB_IAD_GET_INTERFACE_DATA)(
  IN AMI_USB_IAD_PROTOCOL   *This,
  IN UINT8                  UsbIoIndex,
  IN OUT UINTN              *DataSize,
  OUT VOID                  *Data
  );

struct _AMI_USB_IAD_PROTOCOL {
    AMI_USB_IAD_GET_IAD             GetIad;
    AMI_USB_IAD_GET_USB_HANDLE      GetUsbHandle;
    AMI_USB_IAD_GET_INTERFACE_DATA  GetInterfaceData;
};

extern EFI_GUID gAmiUsbIadProtocolGuid;

#endif // __AMI_USB_IAD_PROTOCOL__H__

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
