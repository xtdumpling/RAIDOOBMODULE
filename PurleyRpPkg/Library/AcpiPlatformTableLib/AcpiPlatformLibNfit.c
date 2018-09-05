/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
Copyright (c) 1996 - 2016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file AcpiPlatformLibNfit.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatformLibLocal.h"

extern EFI_CRYSTAL_RIDGE_PROTOCOL *mCrystalRidgeProtocol;
extern EFI_JEDEC_NVDIMM_PROTOCOL *mJedecNvDimmProtocol;


/**

    This function  locates the CrystalRidge Protocol and calls
    into one of its interface function
    (UpdateCrystalRidgeAcpiTable) to update/build the Nvdimm F/W
    Interface Table (Nfit) which builds the Nfit table which
    gets published in Acpi Xsdt.

    @param *Table       - Pointer to Nfit which will be build in
           CR Protocol and will be publised in Acpi Xsdt.

    @retval Status      - Return Status

**/
EFI_STATUS
UpdateNfitTable(
   IN OUT   EFI_ACPI_COMMON_HEADER  *Table
   )
{
  EFI_STATUS    Status;

  if (mCrystalRidgeProtocol == NULL) {
    Status = gBS->LocateProtocol(&gEfiCrystalRidgeGuid, NULL, &mCrystalRidgeProtocol);
    ASSERT_EFI_ERROR(Status);
  }
  mCrystalRidgeProtocol->UpdateNvmAcpiTables((UINT64 *)Table);
  
  if (mJedecNvDimmProtocol == NULL) {
    Status = gBS->LocateProtocol(&gEfiJedecNvDimmGuid, NULL, &mJedecNvDimmProtocol); 
    ASSERT_EFI_ERROR(Status); 
  }
  mJedecNvDimmProtocol->UpdateJedecNvDimmAcpiTable((UINT64 *)Table);

  return EFI_SUCCESS;
}