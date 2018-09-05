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

/** @file IPMITransportProtocol.h
    IPMI Transport protocol definitions

**/

#ifndef _IPMI_TRANSPORT_H_
#define _IPMI_TRANSPORT_H_

//----------------------------------------------------------------------

#include <Token.h>
#include <Include/IpmiNetFnTransportDefinitions.h>

//----------------------------------------------------------------------

//----------------------------------------------------------------------

#define EFI_DXE_IPMI_TRANSPORT_PROTOCOL_GUID \
  { \
    0x4a1d0e66, 0x5271, 0x4e22, { 0x83, 0xfe, 0x90, 0x92, 0x1b, 0x74, 0x82, 0x13 } \
  }

#define EFI_SMM_IPMI_TRANSPORT_PROTOCOL_GUID \
  { \
    0x1dbd1503, 0xa60, 0x4230, { 0xaa, 0xa3, 0x80, 0x16, 0xd8, 0xc3, 0xde, 0x2f } \
  }

//
// Common Defines
//
typedef UINT32  EFI_BMC_STATUS;

#define EFI_BMC_OK                    0
#define EFI_BMC_SOFTFAIL              1
#define EFI_BMC_HARDFAIL              2
#define EFI_BMC_UPDATE_IN_PROGRESS    3

#define MAX_SOFT_COUNT              10

typedef struct _EFI_IPMI_TRANSPORT EFI_IPMI_TRANSPORT;

//
//  IPMI Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_IPMI_SEND_COMMAND) (
  IN EFI_IPMI_TRANSPORT                * This,
  IN UINT8                             NetFunction,
  IN UINT8                             Lun,
  IN UINT8                             Command,
  IN UINT8                             *CommandData,
  IN UINT8                             CommandDataSize,
  OUT UINT8                            *ResponseData,
  OUT UINT8                            *ResponseDataSize
);

typedef
EFI_STATUS
(EFIAPI *EFI_IPMI_GET_CHANNEL_STATUS) (
  IN EFI_IPMI_TRANSPORT               * This,
  OUT EFI_BMC_STATUS                  * BmcStatus,
  OUT EFI_SM_COM_ADDRESS              * ComAddress
);

typedef
EFI_STATUS
(EFIAPI *EFI_IPMI_SEND_COMMAND_EX) (
  IN EFI_IPMI_TRANSPORT                * This,
  IN  UINT8                            NetFunction,
  IN  UINT8                            Lun,
  IN  UINT8                            Command,
  IN  UINT8                            *CommandData,
  IN  UINT8                            CommandDataSize,
  OUT UINT8                            *ResponseData,
  OUT UINT8                            *ResponseDataSize,
  OUT UINT8                            *CompletionCode
);

/**
   IPMI TRANSPORT PROTOCOL
*/
typedef struct _EFI_IPMI_TRANSPORT {
  UINT64                      Revision;
  UINT8                       CommandCompletionCode;
  EFI_IPMI_SEND_COMMAND       SendIpmiCommand;
  EFI_IPMI_GET_CHANNEL_STATUS GetBmcStatus;
  EFI_IPMI_SEND_COMMAND_EX    SendIpmiCommandEx;
} EFI_IPMI_TRANSPORT;

extern EFI_GUID gEfiDxeIpmiTransportProtocolGuid;
extern EFI_GUID gEfiSmmIpmiTransportProtocolGuid;

//----------------------------------------------------------------------

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
