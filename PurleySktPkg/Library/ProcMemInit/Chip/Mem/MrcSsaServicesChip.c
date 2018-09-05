/** @file
  This file contains the implementation of the BIOS version of the SSA services PPI.

  This version contains code for both client and server implementations.

  For Client:
    - Delete include of BiosSsaMemoryServerConfig.h and BiosSsaMemoryDdrtConfig.h.
    - Delete definitions of SSA_REVISION_MEMORY_SERVER and SSA_REVISION_MEMORY_DDRT.
    - Delete declarations of SsaMemoryServerConfig and SsaMemoryDdrtConfig.
    - Replace SsaMemoryServerConfig and SsaMemoryDdrtConfig field values in SsaBiosServicesPpi with NULL.

  For Server:
    - Delete include of BiosSsaMemoryClientConfig.h.
    - Delete definition of SSA_REVISION_MEMORY_CLIENT.
    - Delete declaration of SsaMemoryClientConfig.
    - Replace SsaMemoryClientConfig field value in SsaBiosServicesPpi with NULL.

@copyright
  Copyright (c) 2012 - 2016 Intel Corporation. All rights
  reserved. This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#include "SysFunc.h"
#include "SysHostChip.h"
#include "RcRegs.h"
#include "BiosSsaChipFunc.h"

#ifdef SSA_FLAG

#ifdef SSA_LOADER_FLAG
//
//This BSSA file contains subsets of all BSSA function calls made from the Core files; all CSR accesses are made here
//

/**
Set CSR required by SsaBiosLoadStatus
**/
VOID
SsaBiosLoadStatusChip (
  PSYSHOST           host,
  UINT32             agentReg
  )
{
#ifndef MINIBIOS_BUILD
  WriteCpuPciCfgEx (host, 0, 0, SR_BIOS_SERIAL_DEBUG_CSR, agentReg);
#endif //MINIBIOS_BUILD
}
#endif //SSA_LOADER_FLAG

/**
Save the PEI address location of each of the SSA struct's into SSA_BIOS_SERVICES_PPI
**/
VOID
SsaStructAddressLocationsChip (
  PSYSHOST                         host,
  SSA_BIOS_SERVICES_PPI            *SsaBiosServicesPpi,
  SSA_COMMON_CONFIG                *SsaCommonConfig,
  SSA_RESULTS_CONFIG               *SsaResultsConfig,
  SSA_MEMORY_CONFIG                *SsaMemoryConfig,
  SSA_MEMORY_CLIENT_CONFIG         *SsaMemoryClientConfig,
  SSA_MEMORY_SERVER_CONFIG         *SsaMemoryServerConfig,
  SSA_MEMORY_DDRT_CONFIG           *SsaMemoryDdrtConfig
  )
{
#ifndef MINIBIOS_BUILD 
  SsaBiosServicesPpi->SsaCommonConfig = SsaCommonConfig;
  SsaBiosServicesPpi->SsaResultsConfig = SsaResultsConfig;
  SsaBiosServicesPpi->SsaMemoryConfig = SsaMemoryConfig;
  SsaBiosServicesPpi->SsaMemoryClientConfig = NULL;
  SsaBiosServicesPpi->SsaMemoryServerConfig = SsaMemoryServerConfig;
  SsaBiosServicesPpi->SsaMemoryDdrtConfig = SsaMemoryDdrtConfig;
  SsaBiosServicesPpi->SsaCommonConfig->BiosData = (void*) host;
  SsaBiosServicesPpi->SsaMemoryConfig->MrcData  = (void*) host;
#endif //MINIBIOS_BUILD
}

/**
SsaStichedModeRMTChip: Chip layer function call for setting up the SSA Structs needed for StichedModeSsaRMT
**/
#ifdef EV_STITCHED_CONTENT_ENBL
VOID
SsaStichedModeRMTChip (
  PSYSHOST                         host,
  SSA_BIOS_SERVICES_PPI            *SsaBiosServicesPpi
  )
{
  SSA_STATUS                    RetStatus;

  //Call EV test entry for the 'stitched code'
  RetStatus = EvTestEntry(SsaBiosServicesPpi);

  if (RetStatus != Success) {
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "Error running EV content.\n"));
  }
}
#endif

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
