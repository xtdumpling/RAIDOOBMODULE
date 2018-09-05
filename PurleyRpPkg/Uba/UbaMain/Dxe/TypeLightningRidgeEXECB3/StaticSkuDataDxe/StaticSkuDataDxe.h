//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    UBA static sku data update dxe driver.

  Copyright (c) 2013 - 2014, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _STATIC_SKU_DATA_DXE_H_
#define _STATIC_SKU_DATA_DXE_H_

#include <Base.h>
#include <Uefi.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>

#include <Protocol/UbaCfgDb.h>

EFI_STATUS
InstallMpTableData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
) ;

EFI_STATUS
InstallPirqData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
);


EFI_STATUS
InstallAcpiFixupTableData (
  IN UBA_CONFIG_DATABASE_PROTOCOL    *UbaConfigProtocol
);


#endif // _STATIC_SKU_DATA_DXE_H_
