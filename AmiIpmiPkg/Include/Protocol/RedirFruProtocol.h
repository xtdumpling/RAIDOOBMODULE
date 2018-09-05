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

/** @file RedirFruProtocol.h
    REDIR FRU Protocol definitions

**/

#ifndef _REDIR_FRU_H_
#define _REDIR_FRU_H_

#define EFI_SM_FRU_REDIR_PROTOCOL_GUID \
  { \
    0x28638cfa, 0xea88, 0x456c, { 0x92, 0xa5, 0xf2, 0x49, 0xca, 0x48, 0x85, 0x35 } \
  }

#define EFI_SM_FRU_REDIR_SIGNATURE  SIGNATURE_32 ('f', 'r', 'r', 'x')

typedef struct _EFI_SM_FRU_REDIR_PROTOCOL EFI_SM_FRU_REDIR_PROTOCOL;

//
//  Redir FRU Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_REDIR_INFO) (
  IN  EFI_SM_FRU_REDIR_PROTOCOL           *This,
  IN  UINTN                               FruSlotNumber,
  OUT EFI_GUID                            *FruFormatGuid,
  OUT UINTN                               *DataAccessGranularity,
  OUT CHAR16                              **FruInformationString
);

typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_SLOT_INFO) (
  IN  EFI_SM_FRU_REDIR_PROTOCOL           *This,
  OUT EFI_GUID                            *FruTypeGuid,
  OUT UINTN                               *StartFruSlotNumber,
  OUT UINTN                               *NumSlots
);

typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_REDIR_DATA) (
  IN EFI_SM_FRU_REDIR_PROTOCOL           *This,
  IN UINTN                               FruSlotNumber,
  IN UINTN                               FruDataOffset,
  IN UINTN                               FruDataSize,
  IN UINT8                               *FruData
);

typedef
EFI_STATUS
(EFIAPI *EFI_SET_FRU_REDIR_DATA) (
  IN  EFI_SM_FRU_REDIR_PROTOCOL           * This,
  IN  UINTN                               FruSlotNumber,
  IN  UINTN                               FruDataOffset,
  IN  UINTN                               FruDataSize,
  IN  UINT8                               *FruData
);

/**
    REDIR FRU PROTOCOL
*/
typedef struct _EFI_SM_FRU_REDIR_PROTOCOL {
  EFI_GET_FRU_REDIR_INFO  GetFruRedirInfo;
  EFI_GET_FRU_SLOT_INFO   GetFruSlotInfo;
  EFI_GET_FRU_REDIR_DATA  GetFruRedirData;
  EFI_SET_FRU_REDIR_DATA  SetFruRedirData;
} EFI_SM_FRU_REDIR_PROTOCOL;

extern EFI_GUID gEfiRedirFruProtocolGuid;
extern EFI_GUID gEfiPreFruSmbiosDataGuid;

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
