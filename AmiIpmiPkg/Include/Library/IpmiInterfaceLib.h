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

/** @file IpmiInterfaceLib.h
    IpmiInterfaceLib definitions

**/

#ifndef _IPMI_INTERFACE_LIB_H_
#define _IPMI_INTERFACE_LIB_H_

//
// Type definitions for Interface type.
//
typedef enum _BULK_DATATRANSFER_INTERFACE_TYPE {
  UsbInterface = 1,
  NonUsbInterface = 2
} BULK_DATATRANSFER_INTERFACE_TYPE;

//
// Function Prototype
//
EFI_STATUS
SendIpmiCommand (
  IN  UINT8      NetFunction,
  IN  UINT8      Lun,
  IN  UINT8      Command,
  IN  UINT8      *CommandData,
  IN  UINT8      CommandDataSize,
  OUT UINT8      *ResponseData,
  OUT UINT8      *ResponseDataSize, 
  OUT UINT8      *CompletionCode
  );

EFI_STATUS
UsbSendIpmiCommand (
  IN  UINT8      NetFunction,
  IN  UINT8      Lun,
  IN  UINT8      Command,
  IN  UINT8      *CommandData,
  IN  UINT8      CommandDataSize,
  OUT UINT8      *ResponseData,
  OUT UINT8      *ResponseDataSize,
  OUT UINT8      *CompletionCode
  );

EFI_STATUS
NonUsbSendIpmiCommand (
  IN  UINT8      NetFunction,
  IN  UINT8      Lun,
  IN  UINT8      Command,
  IN  UINT8      *CommandData,
  IN  UINT8      CommandDataSize,
  OUT UINT8      *ResponseData,
  OUT UINT8      *ResponseDataSize,
  OUT UINT8      *CompletionCode
  );

EFI_STATUS
UsbSendOemIpmiCommand (
  IN  UINT8      NetFunction,
  IN  UINT8      Lun,
  IN  UINT8      Command,
  IN  UINT8      *CommandData,
  IN  UINT32     CommandDataSize,
  OUT UINT8      *ResponseData,
  OUT UINT32     *ResponseDataSize,
  OUT UINT8      *CompletionCode
  );

EFI_STATUS
SendOemIpmiCommand (
  IN  UINT8      NetFunction,
  IN  UINT8      Lun,
  IN  UINT8      Command,
  IN  UINT8      *CommandData,
  IN  UINT32     CommandDataSize,
  OUT UINT8      *ResponseData,
  OUT UINT32     *ResponseDataSize,
  OUT UINT8      *CompletionCode
  );

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
