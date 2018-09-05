
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file

  Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#ifndef MINIBIOS_BUILD
#include <Uefi.h>
#include "SysHost.h"
#include "Include/CoreApi.h"
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include "Ppi/MrcHooksServicesPpi.h"
  
static MRC_HOOKS_SERVICES_PPI mMrcHooksServicesPpi = {
  MRC_HOOKS_SERVICES_PPI_REVISION,   
  InitializeUsb2DebugPort,             // INITIALIZE_USB2_DEBUG_PORT
  DiscoverUsb2DebugPort,               // DISCOVER_USB2_DEBUG_PORT_API
  rcVprintf,                           // RC_VPRINTF
  WriteMsrPipe,                        // WRITE_MSR_PIPE
  SetPromoteWarningException,          // SET_PROMOTE_WARNING_EXCEPTION
  ReadSmb,                             // READ_SMB
  WriteSmb,                            // WRITE_SMB
  OutputCheckpoint,                    // OUTPUT_CHECKPOINT
  LogWarning,                          // LOG_WARNING
  OutputWarning,                       // OUTPUT_WARNING
  ReadSpd,                             // READ_SPD
  FixedDelayMem                        // FIXED_DELAY_MEM
};

static EFI_PEI_PPI_DESCRIPTOR mInstallMrcHooksServicesPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gMrcHooksServicesPpiGuid,
  &mMrcHooksServicesPpi
};

//
// Below are the MrcHooksServicesPpi installation
//
UINT32
InstallMrcHooksServicesPpi (
  PSYSHOST Host
  )
{
 
  EFI_STATUS                       Status;
  EFI_PEI_SERVICES                 **PeiServices;

  PeiServices  = (EFI_PEI_SERVICES**) Host->var.common.oemVariable;

  //
  // Initialize MrcHooksServices PPI
  //
  Status = PeiServicesInstallPpi (&mInstallMrcHooksServicesPpi);
  ASSERT_EFI_ERROR (Status);
  rcPrintf ((Host, "InstallPpi MrcHooksServicesPpiDesc Status = %08x\n", Status));
  if (EFI_ERROR (Status)) {
    Host->MrcHooksServicesPpi = NULL;
    Status = EFI_NOT_READY;
  } else {
    Host->MrcHooksServicesPpi = &mMrcHooksServicesPpi;
    rcPrintf ((Host, "CacheMrcHooksServicesPpi in HOST: Host->MrcHooksServicesPpi = %08x\n", Host->MrcHooksServicesPpi));
    Status = EFI_SUCCESS;
  }
  return (UINT32) Status;
}
#endif // MINIBIOS_BUILD
