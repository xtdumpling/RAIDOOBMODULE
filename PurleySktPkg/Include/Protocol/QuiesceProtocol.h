//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/**

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


    @file QuiesceProtocol.h
    
  GUIDs used for Quiesce support

**/

#ifndef _QUIESCE_SUPPORT_PROTOCOL_H_
#define _QUIESCE_SUPPORT_PROTOCOL_H_

//
// Includes
//

//#include "Tiano.h"

//
// Forward reference for pure ANSI compatability
//
//EFI_FORWARD_DECLARATION (EFI_QUIESCE_SUPPORT_PROTOCOL);

#define QUIESCE_SUPPORT_GUID \
  { \
    0x20d6e759, 0x4c4a, 0x40c0, 0x95, 0x33, 0x2b, 0xf0, 0x6, 0x68, 0x50, 0xfd \
  }


#define SYSTEM_QUIESCE    1
#define SYSTEM_UNQUIESCE  0

         
#pragma pack(1)

typedef enum {
  EnumQuiesceWrite = 1,
  EnumQuiescePoll,
  EnumQuiesceRunSvQuiesceCode,
  EnumQuiesceOperationEnd = 0xFF,
} QUIESCE_OPERATION;

typedef struct {
  UINT8                 Operation;
  UINT8                 AccessWidth;
  UINT64                CsrAddress;
  UINT32                AndMask;
  union {
    UINT32              Data;
    UINT32              OrMask;
  } DataMask;
} QUIESCE_DATA_TABLE_ENTRY;


typedef struct {
  UINT32                        GetQuiesceTable;
  UINT32                        Entries;
  QUIESCE_DATA_TABLE_ENTRY      *DataBuffer;
} QUIESCE_TABLE_ENTRY; 

#pragma pack()

//
// Protocol Data Structures
//
typedef 
EFI_STATUS 
(EFIAPI *EFI_GET_QUIESCE_CODE_MEMORY_INFO) (
  IN OUT UINTN *QuieceCodeBaseAddress,
  IN OUT UINTN *QuiesceCodeSize
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_GET_QUIESCE_DATA_MEMORY_INFO) (
  IN OUT UINTN *QuieceDataBaseAddress,
  IN OUT UINTN *QuiesceDataSize
);


typedef 
EFI_STATUS 
(EFIAPI *EFI_SET_BUILD_DATA_MEMORY_INFO) (
  IN UINTN BuildDataBaseAddress,
  IN UINTN BuildDataSize
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_QUIESCE_AP_PROC) (
  VOID
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_QUIESCE_AQUIRE_DATA_BUFFER) (
  IN OUT EFI_PHYSICAL_ADDRESS   *BufferAddress
);


typedef 
EFI_STATUS 
(EFIAPI *EFI_QUIESCE_RELEASE_DATA_BUFFER) (
  IN EFI_PHYSICAL_ADDRESS  BaseAddress
);


typedef 
EFI_STATUS 
(EFIAPI *EFI_QUIESCE_UNQUIESCE) (
  UINT8 QuiesceFlag
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_QUIESCE_MONARCHAP_CHECKIN) (
  IN BOOLEAN  CheckIn
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_QUIESCE_MAIN) (
  VOID
);


typedef struct _EFI_QUIESCE_SUPPORT_PROTOCOL {

  EFI_GET_QUIESCE_CODE_MEMORY_INFO  GetQuiesceCodeMemoryInfo;
  EFI_GET_QUIESCE_DATA_MEMORY_INFO  GetQuiesceDataMemoryInfo;
  EFI_QUIESCE_AP_PROC               QuiesceApProc;
  EFI_QUIESCE_AQUIRE_DATA_BUFFER    AquireDataBuffer;
  EFI_QUIESCE_RELEASE_DATA_BUFFER   ReleaseDataBuffer;
  EFI_QUIESCE_MONARCHAP_CHECKIN     MonarchAPCheckIn;
  EFI_QUIESCE_MAIN                  QuiesceMain;
  EFI_QUIESCE_UNQUIESCE             SystemQuiesceUnQuiesce;


} EFI_QUIESCE_SUPPORT_PROTOCOL;

extern EFI_GUID gEfiQuiesceProtocolGuid;

#endif
