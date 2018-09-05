/** @file
  This file contains the entry point of the EV content library. It allows user to 
  customize the EV content configuration.

@copyright
  Copyright (c) 2016 Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include "SysFunc.h"

#ifdef SSA_FLAG

#include "EvTypes.h"
#include "EvTestEntry.h" 
#if defined(SIM_BUILD) || defined(MINIBIOS_BUILD)
#ifndef CCMRC 
#include "RMT.h"
#endif
#else
#include <Library/MemoryEvContentLib.h>
#endif

#include "BiosSsaResultsConfig.h"
#include "EvLoaderPeim.h"
#include "ScratchPadReg.h"


/**
  Initialize the SsaBiosServices data structure.

  @param[in] MrcData  - The MRC global data area.

  @retval Nothing
**/
SSA_STATUS
EvTestEntry (
  SSA_BIOS_SERVICES_PPI *SsaServicesHandle
  )
{
  UINT8 *                         ResultsData = NULL;
  RESULTS_DATA_HDR *              ResultsDataHdr = NULL;
  UINT32                          ResultsDataSize = 0;
  SSA_STATUS                      RetStatus = Success;
  struct sysHost                  *host;
  RMT_CONFIG                      TestConfiguration;
#ifdef SSA_LOADER_FLAG
#if !defined(RC_SIM) && !defined(MINIBIOS_BUILD)
  EFI_GUID                        ResultsGuid = BIOS_SSA_RESULTS_GUID;
#endif  //!defined(RC_SIM) && !defined(MINIBIOS_BUILD)
#endif //SSA_LOADER_FLAG

  host = (struct sysHost *) SsaServicesHandle->SsaMemoryConfig->MrcData;

  // enable/disable RMT
  // enable/disable per-bit RMT
  // enable/dis table to serail
  // en/dis plot to serial 
  // loopcnt
  // step size
  // en/dis backside 
  // en/dis turnaround
  // loopcnt_turnaround
  // Erid enable/disable

  if (host->setup.mem.biosSsaBacksideMargining) {
    TestConfiguration.IoLevelBitmask = 0x3;
  } else {
    TestConfiguration.IoLevelBitmask = 1;
  }

  if (host->setup.mem.biosSsaEarlyReadIdMargining) {
    TestConfiguration.EnableEridMargin = TRUE;
  } else {
    TestConfiguration.EnableEridMargin = FALSE;
  }

  if (host->setup.mem.biosSsaPerBitMargining) {
    TestConfiguration.EnableLaneMargin = TRUE;
  } else {
   TestConfiguration.EnableLaneMargin = FALSE;
  }

  if (host->setup.mem.biosSsaDisplayTables) {
   TestConfiguration.EnableTestResultLogging = TRUE;
  } else {
   TestConfiguration.EnableTestResultLogging = FALSE;
  }

  if (host->setup.mem.biosSsaPerDisplayPlots) {
    TestConfiguration.EnableLaneMarginPlot = TRUE;
  } else {
   TestConfiguration.EnableLaneMarginPlot = FALSE;
  }

  TestConfiguration.SocketBitmask = 0xFFFFFFFF;
  TestConfiguration.ControllerBitmask = 0xFF;
  TestConfiguration.EnableRankMargin = TRUE;
  TestConfiguration.EnableCtlAllMargin = TRUE;

  if (host->setup.mem.biosSsaStepSizeOverride) {
   TestConfiguration.RxDqsStepSize = host->setup.mem.biosSsaRxDqs;
   TestConfiguration.TxDqStepSize = host->setup.mem.biosSsaTxDq;
   TestConfiguration.RxVrefStepSize = host->setup.mem.biosSsaRxVref;
   TestConfiguration.TxVrefStepSize = host->setup.mem.biosSsaTxVref;
   TestConfiguration.CmdAllStepSize = host->setup.mem.biosSsaCmdAll;
   TestConfiguration.CmdVrefStepSize = host->setup.mem.biosSsaCmdVref;
   TestConfiguration.CtlAllStepSize = host->setup.mem.biosSsaCtlAll;
   TestConfiguration.EridDelayStepSize = host->setup.mem.biosSsaEridDelay;
   TestConfiguration.EridVrefStepSize = host->setup.mem.biosSsaEridVref;
  } else {
   TestConfiguration.RxDqsStepSize = BIOS_SSA_RMT_DEFAULT_VALUE;
   TestConfiguration.TxDqStepSize = BIOS_SSA_RMT_DEFAULT_VALUE;
   TestConfiguration.RxVrefStepSize = BIOS_SSA_RMT_DEFAULT_VALUE;
   TestConfiguration.TxVrefStepSize = BIOS_SSA_RMT_DEFAULT_VALUE;
   TestConfiguration.CmdAllStepSize = BIOS_SSA_RMT_DEFAULT_VALUE;
   TestConfiguration.CmdVrefStepSize = BIOS_SSA_RMT_DEFAULT_VALUE;
   TestConfiguration.CtlAllStepSize = BIOS_SSA_RMT_DEFAULT_VALUE;
   TestConfiguration.EridDelayStepSize = BIOS_SSA_RMT_DEFAULT_VALUE;
   TestConfiguration.EridVrefStepSize = BIOS_SSA_RMT_DEFAULT_VALUE;
  }

  TestConfiguration.TestStatusLogLevel = host->setup.mem.biosSsaDebugMessages;
  TestConfiguration.SinglesLoopCount = host->setup.mem.biosSsaLoopCount; //16

  TestConfiguration.SinglesBurstLength = 32;
  TestConfiguration.SinglesBinarySearchLoopCount = 7;
  TestConfiguration.OptimizedShmooBitmask = 0xFF;
  TestConfiguration.ScramblerOverrideMode = DisableScrambler;
  TestConfiguration.EnableExtendedRange = TRUE;

  // Reset the scratchpad registers that will contain the test result address and size. 
  writeScratchPad_DATA0 (host, 0x0);
  writeScratchPad_DATA1 (host, 0x0);

  // Call EV content entry point
#ifndef CCMRC 
  TestEntryPoint (SsaServicesHandle, (VOID*)&TestConfiguration);
#endif

  // Use the SSA result service
  ResultsData = (UINT8*)SsaServicesHandle->SsaResultsConfig->ResultsData;
  ResultsDataHdr = (RESULTS_DATA_HDR *) ResultsData;
  ResultsDataSize = sizeof (RESULTS_DATA_HDR) + ResultsDataHdr->MdBlock.MetadataSize + (ResultsDataHdr->RsBlock.ResultElementSize * ResultsDataHdr->RsBlock.ResultCapacity);

  // Results are copied to HOBs in PEI and eventually saved to BDAT in DXE.
#if defined(SSA_LOADER_FLAG) && (!defined(RC_SIM) && !defined(MINIBIOS_BUILD))

  RetStatus = SsaServicesHandle->SsaCommonConfig->SaveToBdat (SsaServicesHandle, &ResultsGuid, ResultsData, ResultsDataSize);

  // Save the HOB result data location and size to scratchpad registers, so that the RMT result can
  // be retrieved with ITP even before system boots to EFI.
  writeScratchPad_DATA0 (host, host->var.common.HobPtrSsaResults);
  writeScratchPad_DATA1 (host, ResultsDataSize);

  //Freeing up the temporary memory range in the heap that is used to save of results from the 'stitched SSA' method
  SsaServicesHandle->SsaCommonConfig->Free(SsaServicesHandle, (VOID*)ResultsData);
  SsaServicesHandle->SsaResultsConfig->ResultsData = NULL;
#endif

  return RetStatus;
}
#endif //SSA_FLAG 
