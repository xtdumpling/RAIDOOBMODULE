//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header:  $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  GenericElogProtocol.h
//
// Description:	Generic Elog Protocol definitions
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef _EFI_GENERIC_ELOG_PROTOCOL_H_
#define _EFI_GENERIC_ELOG_PROTOCOL_H_

#define EFI_SM_ELOG_PROTOCOL_GUID \
  { \
    0x59d02fcd, 0x9233, 0x4d34, 0xbc, 0xfe, 0x87, 0xca, 0x81, 0xd3, 0xdd, 0xa7 \
  }

#define EFI_SM_SMM_ELOG_PROTOCOL_INSTALLED_GUID \
  { \
    0xa4223cf1, 0x6b27, 0x4994, 0x8f, 0xcb, 0xa3, 0xa2, 0x79, 0xc0, 0x4e, 0x81 \
  }

#define EFI_SM_ELOG_REDIR_PROTOCOL_GUID \
  { \
    0x16d11030, 0x71ba, 0x4e5e, 0xa9, 0xf9, 0xb4, 0x75, 0xa5, 0x49, 0x4, 0x8a \
  }

typedef struct _EFI_SM_ELOG_PROTOCOL EFI_SM_ELOG_PROTOCOL;
typedef struct _EFI_SM_ELOG_PROTOCOL EFI_SM_ELOG_REDIR_PROTOCOL;

//
// Common Defines
//
typedef enum {
  EfiElogSmSMBIOS,
  EfiElogSmIPMI,
  EfiElogSmMachineCritical,
  EfiElogSmASF,
  EfiElogSmOEM,
  EfiSmElogMax
} EFI_SM_ELOG_TYPE;

//
//  Generic ELOG Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_SET_ELOG_DATA) (
  IN EFI_SM_ELOG_PROTOCOL               * This,
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             DataSize,
  OUT UINT64                            *RecordId
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_ELOG_DATA) (
  IN EFI_SM_ELOG_PROTOCOL               * This,
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  OUT UINTN                         *DataSize,
  IN OUT UINT64                         *RecordId
  );

typedef
EFI_STATUS
(EFIAPI *EFI_ERASE_ELOG_DATA) (
  IN EFI_SM_ELOG_PROTOCOL               * This,
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINT64                         *RecordId
  );

typedef
EFI_STATUS
(EFIAPI *EFI_ACTIVATE_ELOG) (
  IN EFI_SM_ELOG_PROTOCOL               * This,
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN BOOLEAN                            *EnableElog,
  OUT BOOLEAN                           *ElogStatus
  );

typedef struct _EFI_SM_ELOG_PROTOCOL {
EFI_SET_ELOG_DATA	SetEventLogData;
EFI_GET_ELOG_DATA	GetEventLogData;
EFI_ERASE_ELOG_DATA	EraseEventLogData;
EFI_ACTIVATE_ELOG	ActivateEventLog;
} EFI_SM_ELOG_PROTOCOL;

extern EFI_GUID gEfiGenericElogProtocolGuid;
extern EFI_GUID gEfiGenericElogSmmProtocolInstallled;
extern EFI_GUID gEfiRedirElogProtocolGuid;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
