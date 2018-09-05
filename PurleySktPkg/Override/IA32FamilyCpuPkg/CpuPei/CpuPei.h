//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/** @file
Implementation of CPU driver for PEI phase.

This PEIM is to expose the CPUIO ppi, Cache Ppi and BIST hob build notification

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _PEI_CPUPEIM_H_
#define _PEI_CPUPEIM_H_


#include <PiPei.h>

#include <Ppi/Cache.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/SecPlatformInformation.h>
#include <Ppi/SecPlatformInformation2.h>
#include <Ppi/PostBootScriptTable.h>  // OVERIDE_HSD_5370237
#include <Guid/HtBistHob.h>

#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/MtrrLib.h>
#include <Library/PcdLib.h>
#include <Library/SocketLga775Lib.h>
#include <Library/ReportStatusCodeLib.h>
// OVERIDE_HSD_5370237_BEGIN
#include <Library/CpuS3MsrLib.h>


/**

  Call into Cpu library to restore the necessary MSR settings late in S3 resume.


  @param PeiServices   - Pointer to PEI Services Table.
  @param NotifyDesc    - Pointer to the descriptor for the Notification
                         event that caused this function to execute.
  @param Ppi           - Pointer the the Ppi data associated with this function.


  @retval EFI_SUCCESS  - S3 Cpu MSR restore call completed successfully.

**/
EFI_STATUS
EFIAPI
S3RestoreMsrCallback (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
);

/**

  Call into Cpu library to perform any PEI CPU Initialization needed
  in PEI phase that requires multi-threaded execution. This is executed
  on normal boots and S3 resume.

  @param PeiServices   - Pointer to PEI Services Table.
  @param NotifyDesc    - Pointer to the descriptor for the Notification
                         event that caused this function to execute.
  @param Ppi           - Pointer the the Ppi data associated with this function.


  @retval EFI_SUCCESS  - PEI MP Services callback completed successfully.

**/
EFI_STATUS
EFIAPI
PeiMpServicesReadyCallback (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
);
// OVERIDE_HSD_5370237_END
#endif
