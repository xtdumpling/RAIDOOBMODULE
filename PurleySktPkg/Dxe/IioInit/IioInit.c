/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

Copyright (c)  1999 - 2016   Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioInit.c

  Implement IIO initialization protocol to provide IIO initialization functions.
  This interfaces will be called by PciHostBridge driver in the PCI bus enumeration.

**/

#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <SysFunc.h>
#include <Library/OemIioInit.h>
#include <Library/IioInitLib.h>
#include <Library/IioInitDxeLib.h>
#include <Library/HobLib.h>
#include <Protocol/IioSystem.h>
#include <Protocol/IioUds.h>
#include <Protocol/CpuCsrAccess.h>
#include <Guid/IioPolicyHob.h>

EFI_IIO_SYSTEM_PROTOCOL       mIioSystemProtocol;

/**

  Install the IIO Initialization

  @param ImageHandle  - ImageHandle of the loaded driver
  @param SystemTable  - Pointer to the BIOS System Table

  @retval Status
    @retval EFI_SUCCESS           - Installed successfully
    @retval Others                - Failed to install this protocol

**/
EFI_STATUS
EFIAPI
IioInitEntry (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  IIO_GLOBALS                   *IioGlobalData;
  EFI_HANDLE                    IioSystemProtocolHandle = NULL;
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;
  EFI_PEI_HOB_POINTERS          HobPtr;

  DEBUG ((EFI_D_INFO, " IioInit Start, localize IioHob\n"));
  //
  // Get IIO Policy HOB.
  //
  HobPtr.Guid = GetFirstGuidHob (&gIioPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  IioGlobalData = GET_GUID_HOB_DATA (HobPtr.Guid);
  DEBUG ((EFI_D_INFO, " IioHob localized\n"));

  Status = gBS->LocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &CpuCsrAccess);
  ASSERT_EFI_ERROR (Status);
  IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit = (UINT64)CpuCsrAccess;


  Status = IioPostLateInitialize(IioGlobalData);
  ASSERT_EFI_ERROR (Status);

  Status = IioSecurePlatform(mIioSystemProtocol.IioGlobalData);
  ASSERT_EFI_ERROR (Status);

  //
  // Install Dummy protocols for VMD and HFI support
  //
  OemCheckForOtherSupport (IioGlobalData);
  OemUpdateHidePCIePortMenu(IioGlobalData);
  //
  // Build the IIO_SYSTEM_PROTOCOL driver instance for protocol publishing  
  //
  ZeroMem (&mIioSystemProtocol, sizeof (EFI_IIO_SYSTEM_PROTOCOL));
  mIioSystemProtocol.IioGlobalData = IioGlobalData;
  mIioSystemProtocol.IioGetCpuUplinkPort = IioGetCpuUplinkPort;

  //
  // Install IIO System Protocol
  //
  Status = gBS->InstallProtocolInterface (
                    &IioSystemProtocolHandle,
                    &gEfiIioSystemProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mIioSystemProtocol
                    );
  ASSERT_EFI_ERROR (Status);



  return Status;
}
