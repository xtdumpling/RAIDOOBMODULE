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

/** @file Fru.h
    Header file for Fru Area usage

**/

#ifndef __FRU__H__
#define __FRU__H__

//
// Macro definitions
//
#define  MAX_FRU_SLOT                   20

#pragma pack(1)
typedef struct {
  BOOLEAN               Valid;
  IPMI_FRU_DATA_INFO    FruDevice;
  UINT16                ControllerInstance;
} EFI_FRU_DEVICE_INFO;

typedef struct {
  UINTN                       Signature;
  UINT8                       MaxFruSlots;
  UINT8                       NumSlots;
  EFI_FRU_DEVICE_INFO         FruDeviceInfo[MAX_FRU_SLOT];
  EFI_SM_FRU_REDIR_PROTOCOL   IpmiRedirFruProtocol;
} EFI_IPMI_FRU_GLOBAL;

typedef struct {
  UINT8       FormatVersionNumber:4;
  UINT8       Reserved:4;
  UINT8       InternalUseStartingOffset;
  UINT8       ChassisInfoStartingOffset;
  UINT8       BoardAreaStartingOffset;
  UINT8       ProductInfoStartingOffset;
  UINT8       MultiRecInfoStartingOffset;
  UINT8       Pad;
  UINT8       Checksum;
} IPMI_FRU_COMMON_HEADER;

typedef struct {
  UINT8       RecordTypeId;
  UINT8       RecordFormatVersion:4;
  UINT8       Reserved:3;
  UINT8       EndofList:1;
  UINT8       RecordLength;
  UINT8       RecordChecksum;
  UINT8       HeaderChecksum;
} IPMI_FRU_MULTI_RECORD_HEADER;

typedef struct {
  UINT8       RecordCheckSum;
  UINT8       SubRecordId;
  EFI_GUID    Uuid;
} IPMI_SYSTEM_UUID_SUB_RECORD_WITH_CHECKSUM;
#pragma pack()

#define INSTANCE_FROM_EFI_SM_IPMI_FRU_THIS(a) \
  CR(a, EFI_IPMI_FRU_GLOBAL, IpmiRedirFruProtocol, EFI_SM_FRU_REDIR_SIGNATURE)

#endif
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
