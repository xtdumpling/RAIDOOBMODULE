 
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
#include "Include/ChipApi.h"
#include "KtiLib.h"
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include "Ppi/MrcHooksChipServicesPpi.h"
  
static MRC_HOOKS_CHIP_SERVICES_PPI mMrcHooksChipServicesPpi = {
  MRC_HOOKS_CHIP_SERVICES_PPI_REVISION,
  ReadCpuPciCfgEx,                     // READ_CPU_PCI_CFG_EX                                    
  WriteCpuPciCfgEx,                    // WRITE_CPU_PCI_CFG_EX                                       
  SendMailBoxCmdToPcode,               // SEND_MAIL_BOX_CMD_TO_PCODE                                                      
  FatalError,                          // FATAL_ERROR                                
  debugPrintMemFunction                // DEBUG_PRINT_MEM_FUNCTION
};
  
static EFI_PEI_PPI_DESCRIPTOR mInstallMrcHooksChipServicesPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gMrcHooksChipServicesPpiGuid,
  &mMrcHooksChipServicesPpi
};

//
// Below are the MrcHooksChipServicesPpi installation
//
UINT32
InstallMrcHooksChipServicesPpi (
  PSYSHOST Host
  )
{
 
  EFI_STATUS                       Status;
  EFI_PEI_SERVICES                 **PeiServices;

  PeiServices  = (EFI_PEI_SERVICES**) Host->var.common.oemVariable;

  //
  // Initialize MrcHooksChipServices PPI
  //
  Status = PeiServicesInstallPpi (&mInstallMrcHooksChipServicesPpi);
  ASSERT_EFI_ERROR (Status);
  rcPrintf ((Host, "InstallPpi MrcHooksChipServicesPpiDesc Status = %08x\n", Status));
  if (EFI_ERROR (Status)) {
    Host->MrcHooksChipServicesPpi = NULL;
    Status = EFI_NOT_READY;
  } else {
    Host->MrcHooksChipServicesPpi = &mMrcHooksChipServicesPpi;
    rcPrintf ((Host, "CacheMrcChipHooksServicesPpi in HOST: Host->MrcHooksChipServicesPpi = %08x\n", Host->MrcHooksChipServicesPpi));
    Status = EFI_SUCCESS;
  }
  return (UINT32) Status;
}
#endif // MINIBIOS_BUILD
