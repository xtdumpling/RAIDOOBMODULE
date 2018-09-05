/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

  Copyright (c) 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


    @file LtPeiLib.c

  This has platform-specific code for LT.  It contains hooks used by
  ServerCommon code that must be run during PEI for enabling/disabling/examining
  LT/TXT functions and status.

**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/Stall.h>
#include <Ppi/MpService.h>
#include <Guid/SocketVariable.h>
#include <Guid/SocketProcessorCoreVariable.h>
#include <Guid/PlatformTxt.h>
#include <Universal/GetSec/Pei/TxtPeiLib.h>
#include <Library/SetupLib.h>

// External variables
//
extern EFI_GUID gPeiMpServicePpiGuid;
extern EFI_GUID gEfiPeiReadOnlyVariable2PpiGuid;

// External PROCs
//

// Helper PROCs for ServerCommonPkg/Universal/GetSec
//
EFI_STATUS
GetLockChipset (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN UINT8                            *LockChipsetBit
  )
{
  EFI_STATUS                          Status;
  UINT8                               LockChipset;

  Status = GetOptionData(&gEfiSocketProcessorCoreVarGuid, OFFSET_OF (SOCKET_PROCESSORCORE_CONFIGURATION, LockChipset), &LockChipset, sizeof(LockChipset));
  ASSERT_EFI_ERROR (Status);
  *LockChipsetBit = LockChipset;
  return (Status);
}

EFI_STATUS
GetEVMode (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN UINT8                            *EVModeBit
  )
{
  EFI_STATUS                          Status;
  UINT8                               EVMode;

  Status = GetOptionData(&gEfiSocketProcessorCoreVarGuid, OFFSET_OF (SOCKET_PROCESSORCORE_CONFIGURATION, EVMode), &EVMode, sizeof(EVMode));
  ASSERT_EFI_ERROR (Status);
  *EVModeBit = EVMode;
  return (Status);
}

EFI_STATUS
GetProcessorLtsxEnable (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN UINT8                            *ProcessorLtsxEnableBit
  )
{
  EFI_STATUS                          Status;
  UINT8                               ProcessorLtsxEnable;

  Status = GetOptionData(&gEfiSocketProcessorCoreVarGuid, OFFSET_OF (SOCKET_PROCESSORCORE_CONFIGURATION, ProcessorLtsxEnable), &ProcessorLtsxEnable, sizeof(ProcessorLtsxEnable));
  ASSERT_EFI_ERROR (Status);
  *ProcessorLtsxEnableBit = ProcessorLtsxEnable;
  return (Status);
}

EFI_STATUS
GetPeiMpServices (
  IN      CONST EFI_PEI_SERVICES  **PeiServices,
  OUT 	  PEI_MP_SERVICES_PPI 	 **PeiMpServices

  )
{
  //
  // Find PeiMpServices PPI, only need success locating at end of S3 resume
  //
  return ((*PeiServices)->LocatePpi(
                      PeiServices,
                      &gPeiMpServicePpiGuid,
                      0,
                      NULL,
                      PeiMpServices
                      ));
}
