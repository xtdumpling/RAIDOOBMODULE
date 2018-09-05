//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UbaConfigDatabase Peim head file.

  Copyright (c) 2013 - 2014, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_CONFIG_DATABASE_PEIM_H_
#define _UBA_CONFIG_DATABASE_PEIM_H_

#include <PiPei.h>
#include <Uefi/UefiSpec.h>
#include <Ppi/UbaCfgDb.h>
#include <Guid/UbaCfgHob.h>

typedef struct _UBA_PEIM_PRIVATE_DATA {
  UINT32                          Signature;
  UINT32                          Version;
  
  UINTN                           ConfigDataCount;      //for AllConfigDataSize
  UINTN                           HandleCount;
  UBA_BOARD_NODE                  *CurrentSku;
  UINTN                           ThisAddress;
  
  UBA_CONFIG_DATABASE_PPI         UbaCfgDbPpi;
  EFI_PEI_PPI_DESCRIPTOR          UbaPeimPpiList;
}UBA_PEIM_PRIVATE_DATA;

#define PRIVATE_DATA_FROM_PPI(p)    CR(p, UBA_PEIM_PRIVATE_DATA, UbaCfgDbPpi, UBA_BOARD_SIGNATURE)

#endif // _UBA_CONFIG_DATABASE_PEIM_H_