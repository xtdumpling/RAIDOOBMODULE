/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2011 - 2016 Intel Corporation. All rights
  reserved This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.

@file
  BiosGuardInit.h

@brief
  Describes the functions visible to the rest of the BiosGuardInit.
**/
#ifndef _BIOSGUARD_INIT_H_
#define _BIOSGUARD_INIT_H_

#include "BiosGuardDefinitions.h"
#include "SysFunc.h"
#include <Cpu/CpuBaseLib.h>
#include <Guid/PlatformInfo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/ProcMemInitLib.h>
#include <Library/DebugLib.h>
#include <Ppi/Reset.h>
#include <Ppi/SystemBoard.h>
#include <PchAccess.h>
#include <Library/MmPciBaseLib.h>

/**
  Platform policies for BIOS Guard Configuration for all processor security features configuration.
  Platform code can pass relevant configuration data through this structure.
  @note The policies are marked are either <b>(Required)</b> or <b>(Optional)</b>
  - <b>(Required)</b> : This policy is recommended to be properly configured for proper functioning of reference code and silicon initialization
  - <b>(Optional)</b> : This policy is recommended for validation purpose only.
**/
typedef struct {
  BGUP_HEADER          BgupHeader;          ///< BIOS Guard update package header that will be packaged along with BIOS Guard script and update data.
  BGPDT                Bgpdt;               ///< BIOS Guard Platform Data Table contains all the platform data that will be parsed by BIOS Guard module.
  UINT64               BgpdtHash[4];        ///< Hash of the BGPDT that will be programmed to PLAT_FRMW_PROT_HASH_0/1/2/3 MSR.
  UINT8                BiosGuardMemSize;    ///< BIOS Guard memory size.
  UINT8                EcCmdDiscovery;      ///< EC Command discovery.
  UINT8                EcCmdProvisionEav;   ///< EC Command Provision Eav.
  UINT8                EcCmdLock;           ///< EC Command Lock.
  BIOSGUARD_LOG        BiosGuardLog;        ///< BIOS Guard log.
  EFI_PHYSICAL_ADDRESS BiosGuardModulePtr;  ///< Pointer to the BIOS Guard Module.
  UINT32               TotalFlashSize;      ///< Size of entire SPI flash region
} BIOSGUARD_CONFIG;

///
/// Function Prototypes
///
/**
  Execute Early-Post initialization of BIOS Guard specific MSRs

  @param[in] PeiServices                  - Indirect reference to the PEI Services Table.
  @param[in] TotalSystemConfigurationPtr  - Pointer to Setup Configuration structure
  @param[in] host                         - SysHost structure with configuration values
**/
VOID
BiosGuardInit (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN SYSTEM_CONFIGURATION       *TotalSystemConfigurationPtr,
  IN struct sysHost             *host
  );

#endif
