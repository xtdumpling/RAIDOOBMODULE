//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  uba config database head file

  Copyright (c) 2012 - 2014, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_CONFIG_DATABASE_HOB_H_
#define _UBA_CONFIG_DATABASE_HOB_H_

// {E4B2025B-C7DB-4e5d-A65E-2B257EB1058E}
#define UBA_CURRENT_CONFIG_HOB_GUID \
{ 0xe4b2025b, 0xc7db, 0x4e5d, { 0xa6, 0x5e, 0x2b, 0x25, 0x7e, 0xb1, 0x5, 0x8e } }


#define UBA_CONFIG_HOB_SIGNATURE    SIGNATURE_32('U', 'B', 'A', 'H')
#define UBA_CONFIG_HOB_VERSION      0x01

#define UBA_BOARD_SIGNATURE         SIGNATURE_32('S', 'K', 'U', 'D')
#define UBA_BOARD_VERSION           0x01

//
// Interface data between PEI & DXE
// Should keep same align
//
#pragma pack (1)

typedef struct _UBA_CONFIG_HOB_FIELD {
  UINT32                  Signature;
  UINT32                  Version;
  EFI_GUID                ResId;
  UINT64                  DataOffset;
  UINT32                  Size;
} UBA_CONFIG_HOB_FIELD;

typedef struct _UBA_CONFIG_HOB_HEADER {
  UINT32                  Signature;
  UINT32                  Version;
  EFI_GUID                DataGuid;
  UINT32                  HobLength;
  UINT32                  BoardId;
  EFI_GUID                BoardGuid;
  CHAR8                   BoardName[16];
  UINT32                  DataCount;
  UBA_CONFIG_HOB_FIELD    HobField[1];
}UBA_CONFIG_HOB_HEADER;


typedef struct _UBA_BOARD_NODE {
  UINT32                  Signature;
  UINT32                  Version;  
  LIST_ENTRY              DataLinkHead;
  
  UINT32                  BoardId;
  EFI_GUID                BoardGuid;
  CHAR8                   BoardName[16];
  UINT32                  DataCount;
}UBA_BOARD_NODE;

typedef struct _UBA_CONFIG_NODE {
  UINT32                  Signature;
  UINT32                  Version;
  LIST_ENTRY              DataLink;

  EFI_HANDLE              Handle;
  EFI_GUID                ResId;
  UINT32                  Size;
  VOID                    *Data;
}UBA_CONFIG_NODE;

#pragma pack ()

#define BOARD_NODE_INSTANCE_FROM_THIS(p)       CR(p, UBA_BOARD_NODE, DataLinkHead, UBA_BOARD_SIGNATURE)
#define CONFIG_NODE_INSTANCE_FROM_THIS(p)      CR(p, UBA_CONFIG_NODE, DataLink, UBA_BOARD_SIGNATURE)

#endif // _UBA_CONFIG_DATABASE_HOB_H_
