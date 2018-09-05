/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

  Copyright (c) 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


    @file PlatformCpu.c

**/


#include <Uefi.h>

#include <Library/PlatformCpuLib.h>
#include <Library/SetupLib.h>


/**
  Library call to get the enabled CPU features from Setup variables and
  pass back these values as a bitmap.

  @param  VOID

  @retval UINT32         Bitmap of enabled CPU features.

**/
UINT32
EFIAPI
GetPlatformEnabledCpuFeatures (
  VOID
  )
{
  EFI_STATUS            Status;
  UINT8                 ProcessorMsrLockControl;
  UINT8                 ProcessorAutonomousCstateEnable;
  UINT8                 LlcPrefetchEnable;
  UINT32                EnabledCpuFeatures = 0;

  Status = GetOptionData (&gEfiSocketProcessorCoreVarGuid, OFFSET_OF(SOCKET_PROCESSORCORE_CONFIGURATION, ProcessorMsrLockControl), &ProcessorMsrLockControl, sizeof(ProcessorMsrLockControl));
  Status = GetOptionData (&gEfiSocketPowermanagementVarGuid, OFFSET_OF(SOCKET_POWERMANAGEMENT_CONFIGURATION, ProcessorAutonomousCstateEnable), &ProcessorAutonomousCstateEnable, sizeof(ProcessorAutonomousCstateEnable));
  Status = GetOptionData (&gEfiSocketProcessorCoreVarGuid, OFFSET_OF(SOCKET_PROCESSORCORE_CONFIGURATION, LlcPrefetchEnable), &LlcPrefetchEnable, sizeof(LlcPrefetchEnable));

  //
  // Default values if we fail to get the Setup values
  //
  if (EFI_ERROR(Status)){
    ProcessorMsrLockControl = TRUE;
    ProcessorAutonomousCstateEnable = FALSE;
    LlcPrefetchEnable = TRUE;
  }

  EnabledCpuFeatures = (ProcessorMsrLockControl ? B_PROC_MSR_LOCK_CTRL : 0) |
                       (ProcessorAutonomousCstateEnable ? B_PROC_MSR_ACC_EN : 0) |
                       (LlcPrefetchEnable ? B_PROC_LLC_PREFETCH_EN : 0);

  return EnabledCpuFeatures;
}