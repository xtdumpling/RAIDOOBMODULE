//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  EFI Platform Pirq Data Definition Header File.

  Copyright (c) 1999 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _EFI_PLATF_PIRQ_DATA_H_
#define _EFI_PLATF_PIRQ_DATA_H_

#include <Protocol/LegacyBiosPlatform.h>

#define EFI_PIRQ_TABLE_SIGNATURE  0x52495024
#define EFI_PIRQ_TABLE_VERSION    0x100

#define MAX_IRQ_ROUTING_ENTRIES   623
#define MAX_IRQ_PRIORITY_ENTRIES  7

//
// Common path types.
//
typedef struct {
  EFI_LEGACY_PIRQ_TABLE_HEADER  PirqTable;
  EFI_LEGACY_IRQ_ROUTING_ENTRY  IrqRoutingEntry[MAX_IRQ_ROUTING_ENTRIES];
} EFI_LEGACY_PIRQ_TABLE;

//
// This is the module global containing platform PIRQ data.
//
typedef struct {
  EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY *PriorityTable;  
  UINTN                                PriorityTableSize;
  EFI_LEGACY_PIRQ_TABLE               *TableHead;
  UINTN                                TableHeadSize;
  UINT16                              *BusTranslationTbl;
  UINTN                                BusTranslationTblSize;
} PLATFORM_PIRQ_DATA;

#endif  //_EFI_PLATF_PIRQ_DATA_H_

