//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This file contains routines that get PCI Express Address

  Copyright (c) 2011 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SiliconRegAccess.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>

USRA_PROTOCOL     *mPciUsra   = NULL;

/**
  The constructor function initialize UsraProtocol.
  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
SmmMmPciLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  if (mPciUsra == NULL) 
  {
    //
    // USRA protocol need to be installed before the module access USRA.
    // 
    Status = gSmst->SmmLocateProtocol (&gUsraProtocolGuid, NULL, &mPciUsra);
    ASSERT_EFI_ERROR (Status);
    ASSERT (mPciUsra != NULL);
  }
  return EFI_SUCCESS;
}

/**
  This procedure will get PCIE address
  
  @param[in] Bus                  Pci Bus Number
  @param[in] Device               Pci Device Number
  @param[in] Function             Pci Function Number

  @retval PCIE address
**/
UINTN
MmPciBase (
  IN UINT32                       Bus,
  IN UINT32                       Device,
  IN UINT32                       Function
)
{
  USRA_ADDRESS Address;
  USRA_PCIE_ADDRESS(Address, UsraWidth32, Bus, Device, Function, 0);
  return mPciUsra->GetRegAddr (&Address);
}

/**
  This procedure will get PCIE address
  
  @param[in] Seg                  Pcie Segment Number
  @param[in] Bus                  Pcie Bus Number
  @param[in] Device               Pcie Device Number
  @param[in] Function             Pcie Function Number

  @retval PCIE address
**/
UINTN
MmPciAddress(
IN UINT32                       Seg,
IN UINT32                       Bus,
IN UINT32                       Device,
IN UINT32                       Function,
IN UINT32                       Register
)
{
  USRA_ADDRESS Address;
  USRA_PCIE_SEG_ADDRESS(Address, UsraWidth32, Seg, Bus, Device, Function, Register);
  return mPciUsra->GetRegAddr (&Address);
}
