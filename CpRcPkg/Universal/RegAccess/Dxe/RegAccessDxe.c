//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  USRA DXE driver manage all USRA entry initialized in PEI phase and DXE phase, and
  produce the implementation of native USRA protocol defined

  Copyright (c) 2014 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmBase2.h>
#include <Library/BaseMemoryLib/MemLibInternals.h>
#include <Library/PcdLib.h>

#include <Library/PcieAddress.h>
#include <Protocol/SiliconRegAccess.h>
#include <Library/UsraAccessApi.h>
#include <Library/DebugLib.h>

// #include <Library/BaseMemoryLib/MemLibInternals.h>
// #pragma optimize ("",off)

EFI_HANDLE                gUsraHandle = NULL;

EFI_SMM_BASE2_PROTOCOL    *mSmmBase = NULL;
EFI_SMM_SYSTEM_TABLE2     *mSmst = NULL;
UINTN                     *mHostPtr = NULL;

///
/// USRA_PROTOCOL the EDKII native implementation which support Unified 
/// Silicon Register (CSR, PCIE) Access
///
USRA_PROTOCOL mUsraInstance = {
  DxeRegRead,
  DxeRegWrite,
  DxeRegModify,
  DxeGetRegAddr
};


/**
  Main entry for USRA DXE driver.
  This routine initialize the USRA driver and install USRA_PROTOCOL.
  
  @param[in] ImageHandle          Image handle for USRA DXE driver
  @param[in] SystemTable          Pointer to SystemTable

  @retval Status                  Status of gBS->InstallProtocolInterface()
**/
EFI_STATUS
EFIAPI
UsraDxeInit (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;
  UINTN                   MmcfgTableSize;
  UINT32                  *HobMmcfgTable;
  PHYSICAL_ADDRESS        BaseAddressMem;
  USRA_ADDRESS            PcieAddress;
  //
  // Install USRA_PROTOCOL to handle Unified Reg access in Dxe
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gUsraHandle,
                  &gUsraProtocolGuid,
                  &mUsraInstance,
                  NULL
                  );

  ASSERT_EFI_ERROR (Status);

  HobMmcfgTable = (UINT32 *) PcdGetPtr (PcdPcieMmcfgTablePtr); 

// APTIOV_SERVER_OVERRIDE_RC_START : For DynamicEx, PCD should use the Method of PcdGetSize not LibPcdGetSize.
#if 0
  MmcfgTableSize = LibPcdGetSize (PcdToken (PcdPcieMmcfgTablePtr));
#else
  MmcfgTableSize = PcdGetSize (PcdPcieMmcfgTablePtr);
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : For DynamicEx, PCD should use the Method of PcdGetSize not LibPcdGetSize.
  if(HobMmcfgTable[0] != 0x4746434d)
  {
    //
    // if it is not valid MMCFG Table (signature is not "MCFG"), 
    //   Update it wiht predefined default MMCFG Table
    //
    USRA_ZERO_ADDRESS_TYPE(PcieAddress, AddrTypePCIE);
    GetRegisterAddress(&PcieAddress);
  }

  BaseAddressMem = 0xFFFFFFFF;
  Status = gBS->AllocatePages (
                AllocateMaxAddress,
                EfiReservedMemoryType,
                EFI_SIZE_TO_PAGES (MmcfgTableSize),
                &BaseAddressMem
                );
  ASSERT (BaseAddressMem <= 0xffffffff);  
  mHostPtr = (UINTN*)BaseAddressMem;

  InternalMemCopyMem(mHostPtr, HobMmcfgTable, MmcfgTableSize);

  return Status;

}


