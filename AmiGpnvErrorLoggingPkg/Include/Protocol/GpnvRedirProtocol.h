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


/** @file GpnvRedirProtocol.h
    Redir Elog Protocol definitions

**/

#ifndef _EFI_REDIR_ELOG_PROTOCOL_H_
#define _EFI_REDIR_ELOG_PROTOCOL_H_

#define EFI_ELOG_REDIR_PROTOCOL_GUID \
  { \
    0x16d11030, 0x71ba, 0x4e5e, { 0xa9, 0xf9, 0xb4, 0x75, 0xa5, 0x49, 0x4, 0x8a }\
  }

typedef struct _EFI_SM_ELOG_REDIR_PROTOCOL EFI_SM_ELOG_REDIR_PROTOCOL;

// Common Defines

typedef enum {
  EfiElogRedirSmSMBIOS,
  EfiElogRedirSmIPMI,
  EfiElogRedirSmMachineCritical,
  EfiElogRedirSmASF,
  EfiElogRedirSmOEM,
  EfiSmElogRedirMax
} EFI_SM_ELOG_REDIR_TYPE;

// Generic ELOG Redir Function Prototypes

typedef
EFI_STATUS
(EFIAPI *EFI_SET_ELOG_REDIR_DATA) (
  IN  EFI_SM_ELOG_REDIR_PROTOCOL        *This,
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_REDIR_TYPE            DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             DataSize,
  OUT UINT64                            *RecordId );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_ELOG_REDIR_DATA) (
  IN  EFI_SM_ELOG_REDIR_PROTOCOL        *This,
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_REDIR_TYPE            DataType,
  IN  OUT UINTN                         *DataSize,
  IN OUT UINT64                         *RecordId );

typedef
EFI_STATUS
(EFIAPI *EFI_ERASE_ELOG_REDIR_DATA) (
  IN  EFI_SM_ELOG_REDIR_PROTOCOL        *This,
  IN EFI_SM_ELOG_REDIR_TYPE             DataType,
  IN OUT UINT64                         *RecordId );

typedef
EFI_STATUS
(EFIAPI *EFI_ACTIVATE_ELOG_REDIR) (
  IN  EFI_SM_ELOG_REDIR_PROTOCOL        *This,
  IN EFI_SM_ELOG_REDIR_TYPE             DataType,
  IN BOOLEAN                            *EnableElog,
  OUT BOOLEAN                           *ElogStatus );

typedef struct _EFI_SM_ELOG_REDIR_PROTOCOL {
EFI_SET_ELOG_REDIR_DATA         SetEventLogData;
EFI_GET_ELOG_REDIR_DATA         GetEventLogData;
EFI_ERASE_ELOG_REDIR_DATA       EraseEventLogData;
EFI_ACTIVATE_ELOG_REDIR         ActivateEventLog;
} EFI_SM_ELOG_REDIR_PROTOCOL;

EFI_GUID gEfiRedirElogProtocolGuid;
EFI_GUID gAmiSmmDummyProtocolRedirGuid;

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
