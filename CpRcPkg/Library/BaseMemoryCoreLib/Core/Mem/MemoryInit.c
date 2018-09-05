/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MemoryInit.c

  Memory Initialization Module.

**/

#include "MemFunc.h"
#include "KtiApi.h"
// APTIOV_SERVER_OVERRIDE_RC_START : AMI hook
#include  <Library/PeiServicesTablePointerLib.h>

#define AMI_PEI_AFTER_MRC_GUID \
 {0x64c96700, 0x6b4c, 0x480c, 0xa3, 0xe1, 0xb8, 0xbd, 0xe8, 0xf6, 0x2, 0xb2}
EFI_GUID gAmiPeiAfterMrcGuid            = AMI_PEI_AFTER_MRC_GUID;

static EFI_PEI_PPI_DESCRIPTOR mAmiPeiCompelteMrcDesc[] = {
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), \
      &gAmiPeiAfterMrcGuid, \
      NULL }
};
// APTIOV_SERVER_OVERRIDE_RC_END : AMI hook

/**

  MRC wrapper code.

    @param host  - Pointer to sysHost

    @retval N/A

**/
VOID
MemoryInit (
  struct sysHost             *host
  )
{

  UINT32  MrcStatus;
  
  // APTIOV_SERVER_OVERRIDE_RC_START
  EFI_STATUS             Status;
  EFI_BOOT_MODE          BootMode;
  CONST EFI_PEI_SERVICES **PeiServices;

  PeiServices = GetPeiServicesTablePointer ();
  // APTIOV_SERVER_OVERRIDE_RC_END

  //
  // OemHook Before Memory Init
  //
  MrcStatus = OemPreMemoryInit (host);

  //
  // Invoke MRC
  //
  MrcStatus = MemStart (host);
  if (MrcStatus) {
    host->var.common.rcStatus = MrcStatus;
  }
  
  // APTIOV_SERVER_OVERRIDE_RC_START
  // Install AMI hook guid
  Status = (*PeiServices)->InstallPpi(PeiServices, &mAmiPeiCompelteMrcDesc[0]);
  ASSERT_EFI_ERROR (Status);

  // Determine if change boot mode
  Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
  ASSERT_EFI_ERROR(Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    host->var.common.bootMode = S3Resume;
  } else {
    host->var.common.bootMode = NormalBoot;
  }
  // APTIOV_SERVER_OVERRIDE_RC_END

#ifdef  BDAT_SUPPORT
  //
  // Compatible BIOS Data Structure
  //
  if (host->var.common.resetRequired == 0) {
    FillBDATStructure(host);
  }
#endif  //  BDAT_SUPPORT

  UncoreEnableMeshMode (host);

  OemPostMemoryInit (host, MrcStatus);

}
