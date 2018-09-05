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

#include <Library/MmPciBaseLib.h>

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

  if (!FeaturePcdGet (PcdSingleSegFixMmcfg)) 
  {
    return GetRegisterAddress(&Address);
  }
  //
  // If the PcdSingleSegFixMmcfg is true, do the following with static PcdPciExpressBaseAddress
  //
  return ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + (UINTN) (Address.Attribute.RawData32[0] & 0x00ffffff));
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

  if (!FeaturePcdGet(PcdSingleSegFixMmcfg))
  {
    return GetRegisterAddress(&Address);
  }
  //
  // If the PcdSingleSegFixMmcfg is true, do the following with static PcdPciExpressBaseAddress
  //
  return ((UINTN)(PcdGet64(PcdPciExpressBaseAddress)) + (UINTN)(Address.Attribute.RawData32[0] & 0x00ffffff));
}
