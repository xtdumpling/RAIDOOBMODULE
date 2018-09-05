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

#ifdef SSA_FLAG

#if !defined(RC_SIM) && !defined(MINIBIOS_BUILD)
#include <Library/BaseMemoryLib.h>
#endif //!defined(RC_SIM) && !defined(MINIBIOS_BUILD)

#pragma optimize ("", off)

#include "EvItpDownloadAgent.h"
#include "EvLoaderPeim.h"
#include "BssaDebug.h"
#include "MrcSsaServices.h"
#include "BiosSsaCommonConfig.h"
#include "BiosSsaResultsConfig.h"
#include "BiosSsaMemoryConfig.h"
#include "BiosSsaMemoryServerConfig.h"
#include "BiosSsaMemoryDdrtConfig.h"
#include "BiosSsaChipFunc.h"

extern EFI_GUID  gSsaBiosServicesPpiGuid; //  = SSA_BIOS_SERVICES_PPI_GUID;

#ifdef SSA_LOADER_FLAG
EFI_GUID BssaVarGuid              = BIOS_SSA_VARIABLES_GUID;
EFI_GUID BssaResultsGuid          = BIOS_SSA_RESULTS_GUID;
#endif //SSA_LOADER_FLAG

#pragma pack (push, 1)

/*****************************************************************************
SSA Api call table:
*****************************************************************************/

SSA_BIOS_SERVICES_PPI SsaBiosServicesConst = {
    SSA_REVISION_BIOS,
    NULL, //&SsaCommonConfig,
    NULL, //&SsaResultsConfig,
    NULL, //&SsaMemoryConfig,
    NULL, //&SsaMemoryClientConfig,
    NULL, //&SsaMemoryServerConfig,
    NULL  //&SsaMemoryDdrtConfig
  };

SSA_COMMON_CONFIG SsaCommonConfigConst = {
    SSA_REVISION_COMMON,
    NULL,                   // MrcData
    NULL,                   // HookBuffer
    BiosReadMem,
    BiosWriteMem,
    BiosReadIo,
    BiosWriteIo,
    BiosReadPci,
    BiosWritePci,
    BiosGetBaseAddress,
    BiosMalloc,
    BiosFree,
    SSA_D_ERROR,  //PRINT_LEVEL SSA_D_ERROR
    BiosSsaDebugPrint,
    BiosWait,
    BiosSaveToBdat,
    BiosRegisterHookHandler,
    BiosRunHook,
    BiosSaveDataPointer,
    BiosRestoreDataPointer,
    BiosDisableWatchdogTimer,
    BiosGetTimestamp
  };

SSA_RESULTS_CONFIG SsaResultsConfigConst = {
    SSA_REVISION_RESULT,
    NULL,                   // Results buffer
    BiosCreateResultBuffer,
    BiosSetRevision,
    BiosGetRevision,
    BiosSetMetadataType,
    BiosGetMetadataType,
    BiosGetMetadataSize,
    BiosGetMetadata,
    BiosSetResultElementType,
    BiosGetResultElementType,
    BiosGetResultElementSize,
    BiosGetResultElementCapacity,
    BiosGetResultElementCount,
    BiosGetNextResultElement,
    BiosSetExplicitTransferMode,
    BiosGetExplicitTransferMode,
    BiosPushResults
  };

SSA_MEMORY_CONFIG SsaMemoryConfigConst = {
    SSA_REVISION_MEMORY,
    NULL,                   // MrcData
    BiosGetSystemInfo,
    BiosGetMemVoltage,
    BiosSetMemVoltage,
    BiosGetMemTemp,
    BiosGetControllerBitMask,
    BiosGetChannelBitMask,
    BiosGetDimmBitMask,
    BiosGetRankInDimm,
    BiosGetLogicalRank,
    BiosGetDimmInfo,
    BiosGetDimmUniqueModuleId,
    BiosGetSpdData,
    BiosJedecReset,
    BiosIoReset,
    BiosGetMarginParamSpecificity,
    BiosInitMarginParam,
    BiosGetMarginParamLimits,
    BiosSetMarginParamOffset,
    BiosSetZQCalConfig,
    BiosSetRCompUpdateConfig,
    BiosSetPageOpenConfig,
    BiosScrubMemory,
    BiosInitCpgc
  };

SSA_MEMORY_SERVER_CONFIG SsaMemoryServerConfigConst = {
    SSA_REVISION_MEMORY_SERVER,
    BiosServerGetCpgcInfo,
    BiosServerGetCsEncoding,
    BiosServerGet3dsRankPerCs,
    BiosServerSetCreditWaitConfig,
    BiosServerSetWpqInOrderConfig,
    BiosServerSetMultipleWrCreditConfig,
    BiosServerSetCpgcLockState,
    BiosServerSetDqdbPattern,
    BiosServerGetDqdbPattern,
    BiosServerSetDqdbIndexes,
    BiosServerSetDqdbEccDataSource,
    BiosServerSetDqdbUniseqMode,
    BiosServerSetDqdbUniseqWrSeed,
    BiosServerSetDqdbUniseqRdSeed,
    BiosServerGetDqdbUniseqWrSeed,
    BiosServerGetDqdbUniseqRdSeed,
    BiosServerSetDqdbUniseqLmn,
    BiosServerSetDqdbUniseqLfsrConfig,
    BiosServerSetDqdbInvertDcConfig,
    BiosServerSetCadbConfig,
    BiosServerSetCadbPattern,
    BiosServerSetCadbUniseqMode,
    BiosServerSetCadbUniseqSeed,
    BiosServerSetCadbUniseqLmn,
    BiosServerSetDummyReadLmn,
    BiosServerSetDummyReadBankMask,
    BiosServerSetSubseqConfig,
    BiosServerSetSubseqOffsetConfig,
    BiosServerSetSeqConfig,
    BiosServerSetLoopCount,
    BiosServerSetSeqAddrConfig,
    BiosServerSetSeqStartAddr,
    BiosServerSetSeqWrapAddr,
    BiosServerSetSeqAddrInc,
    BiosServerSetSeqRankMap,
    BiosServerSetSeqRowAddrSwizzle,
    BiosServerGetSeqState,
    BiosServerSetStopMode,
    BiosServerSetErrorCounterMode,
    BiosServerSetLaneValidationMask,
    BiosServerSetChunkValidationMask,
    BiosServerSetCachelineValidationMask,
    BiosServerGetLaneErrorStatus,
    BiosServerGetChunkErrorStatus,
    BiosServerGetRankErrorStatus,
    BiosServerGetErrorAddress,
    BiosServerGetErrorCount,
    BiosServerSetDqdbErrorCaptureConfig,
    BiosServerGetDqdbErrorCaptureStatus,
    BiosServerSetLocalTestControl,
    BiosServerSetGlobalTestControl,
    BiosServerSetAddrConfig,
    BiosServerGetTestDone,
    BiosServerSetScramblerConfig,
    BiosServerSetSelfRefreshConfig,
    BiosServerSetRefreshOverride,
    BiosServerSetZQCalOverride,
    BiosServerSetOdtOverride,
    BiosServerSetCkeOverride
  };

SSA_MEMORY_DDRT_CONFIG SsaMemoryDdrtConfigConst = {
    SSA_REVISION_MEMORY_DDRT,
    BiosSetEridValidationMask,
    BiosGetEridErrorStatus,
    BiosClearEridErrorStatus,
    BiosSetEridPattern,
    BiosSetEridTrainingConfig,
    BiosSetAepDimmTrainingMode,
    BiosAepIoReset,
    BiosAepRpqDrain,
    BiosAepClrPrefetchCache,
    BiosAepFifoTrainReset
  };

/*****************************************************************************
SSA Api initialization functions:
*****************************************************************************/

#ifdef SSA_LOADER_FLAG
void
SsaBiosLoadStatus (
  PSYSHOST host
  );
#endif //SSA_LOADER_FLAG

/**

Check boot parameters before executing SSA RMT

@param host  - Pointer to sysHost

@retval TRUE/FALSE

**/
#ifdef EV_STITCHED_CONTENT_ENBL
static
BOOLEAN
  CheckSSARMTParameters (
  PSYSHOST host
  )
{
  if (host->var.common.bootMode == S3Resume || host->var.mem.subBootMode == WarmBootFast ||
      (host->var.mem.subBootMode == ColdBootFast && !host->setup.mem.enableBiosSsaRMTonFCB)) {
     return TRUE; //We will skip SSA RMT if TRUE
  } else {
    return FALSE;
  }
} //CheckSSARMTParameters
#endif EV_STITCHED_CONTENT_ENBL

/**
  Initialize the SsaBiosServices data structure.

  @param[in] MrcData  - The MRC global data area.

  @retval Nothing
**/
void
SsaBiosInitialize (
  PSYSHOST host
  )
{
#ifdef EV_STITCHED_CONTENT_ENBL  //Include 'stitched BSSA' in RC Sim & EFI BIOS; exclude from MiniBIOS
  SSA_BIOS_SERVICES_PPI        *SsaBiosServicesPpi;
  SSA_COMMON_CONFIG            *SsaCommonConfig;
  SSA_RESULTS_CONFIG           *SsaResultsConfig;
  SSA_MEMORY_CONFIG            *SsaMemoryConfig;
  SSA_MEMORY_SERVER_CONFIG     *SsaMemoryServerConfig;
  SSA_MEMORY_DDRT_CONFIG       *SsaMemoryDdrtConfig;
#endif //EV_STITCHED_CONTENT_ENBL

#if !defined(RC_SIM) && !defined(MINIBIOS_BUILD)  //Only for full BIOS
  EFI_PEI_PPI_DESCRIPTOR  *SsaBiosServicesPpiDesc;  //Pointer to store the Address of SSA_BIOS_SERVICES_PPI
  EFI_STATUS              Status;
  EFI_PEI_SERVICES        **PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
#endif

#ifdef EV_STITCHED_CONTENT_ENBL
  MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "BIOS SSA Initialization Started\n"));

  if ((host->setup.mem.enableBiosSsaRMT) && (CheckSSARMTParameters(host) != TRUE)) {
    //Allocate memory for the SSA structs of function pointers
    SsaBiosServicesPpi = (SSA_BIOS_SERVICES_PPI*)rcMalloc(host, sizeof(SSA_BIOS_SERVICES_PPI));
    if(SsaBiosServicesPpi == NULL){
#ifdef DEBUG
      rcPrintf((host, "\nERROR: SsaBiosInitialize () - EFI OUT OF RESOURCES.\n"));
#endif //DEBUG
      return;
    }

    SsaCommonConfig = (SSA_COMMON_CONFIG*) rcMalloc(host, sizeof(SSA_COMMON_CONFIG));
    if(SsaCommonConfig == NULL){
#ifdef DEBUG
      rcPrintf((host, "\nERROR: SsaBiosInitialize () - EFI OUT OF RESOURCES.\n"));
#endif //DEBUG
      return;
    }

    SsaResultsConfig = (SSA_RESULTS_CONFIG*)rcMalloc(host, sizeof(SSA_RESULTS_CONFIG));
    if(SsaResultsConfig == NULL){
#ifdef DEBUG
      rcPrintf((host, "\nERROR: SsaBiosInitialize () - EFI OUT OF RESOURCES.\n"));
#endif //DEBUG
      return;
    }

    SsaMemoryConfig = (SSA_MEMORY_CONFIG*)rcMalloc(host, sizeof(SSA_MEMORY_CONFIG));
    if(SsaMemoryConfig == NULL){
#ifdef DEBUG
      rcPrintf((host, "\nERROR: SsaBiosInitialize () - EFI OUT OF RESOURCES.\n"));
#endif //DEBUG
      return;
    }

    SsaMemoryServerConfig = (SSA_MEMORY_SERVER_CONFIG*)rcMalloc(host, sizeof(SSA_MEMORY_SERVER_CONFIG));
    if(SsaMemoryServerConfig == NULL){
#ifdef DEBUG
      rcPrintf((host, "\nERROR: SsaBiosInitialize () - EFI OUT OF RESOURCES.\n"));
#endif //DEBUG
      return;
    }

    SsaMemoryDdrtConfig = (SSA_MEMORY_DDRT_CONFIG*)rcMalloc(host, sizeof(SSA_MEMORY_DDRT_CONFIG));
    if(SsaMemoryDdrtConfig == NULL){
#ifdef DEBUG
      rcPrintf((host, "\nERROR: SsaBiosInitialize () - EFI OUT OF RESOURCES.\n"));
#endif //DEBUG
      return;
    }

    MemCopy((UINT8*) SsaCommonConfig, (UINT8*) &SsaCommonConfigConst, sizeof (SSA_COMMON_CONFIG));
    MemCopy((UINT8*) SsaResultsConfig, (UINT8*) &SsaResultsConfigConst, sizeof (SSA_RESULTS_CONFIG));
    MemCopy((UINT8*) SsaMemoryConfig, (UINT8*) &SsaMemoryConfigConst, sizeof (SSA_MEMORY_CONFIG));
    MemCopy((UINT8*) SsaMemoryServerConfig, (UINT8*) &SsaMemoryServerConfigConst, sizeof (SSA_MEMORY_SERVER_CONFIG));
    MemCopy((UINT8*) SsaMemoryDdrtConfig, (UINT8*) &SsaMemoryDdrtConfigConst, sizeof (SSA_MEMORY_DDRT_CONFIG));

    //Saving the location of the SSA Structs into SSA_BIOS_SERVICES_PPI
    SsaBiosServicesPpi->SsaCommonConfig = SsaCommonConfig;
    SsaBiosServicesPpi->SsaResultsConfig = SsaResultsConfig;
    SsaBiosServicesPpi->SsaMemoryConfig = SsaMemoryConfig;
    SsaBiosServicesPpi->SsaMemoryClientConfig = NULL;
    SsaBiosServicesPpi->SsaMemoryServerConfig = SsaMemoryServerConfig;
    SsaBiosServicesPpi->SsaMemoryDdrtConfig = SsaMemoryDdrtConfig;
    SsaBiosServicesPpi->SsaCommonConfig->BiosData = (void*) host;
    SsaBiosServicesPpi->SsaMemoryConfig->MrcData  = (void*) host;
    SsaBiosServicesPpi->Revision = SSA_REVISION_BIOS;

    //Call EV test entry function for the 'stitched code'
#ifndef CCMRC
    CHIP_FUNC_CALL(host, SsaStichedModeRMTChip(host, SsaBiosServicesPpi));
#endif

    //Free all the memory allocated on the heap
    rcFree(host, SsaCommonConfig);
    rcFree(host, SsaResultsConfig);
    rcFree(host, SsaMemoryConfig);
    rcFree(host, SsaMemoryServerConfig);
    rcFree(host, SsaMemoryDdrtConfig);
    rcFree(host, SsaBiosServicesPpi);
  } //enableBiosSsaRMT && S3Resume||WarmBootFast
#endif //EV_STITCHED_CONTENT_ENBL

  //Continue to publish the SSA PPI if EFI framework is available. Include 'publishing the PPI' only in the EFI BIOS.
#if !defined(RC_SIM) && !defined(MINIBIOS_BUILD)
  //Allocating PEI memory for SSA_BIOS_SERVICES_PPI and saving the base address of that memory region in *SsaBiosServicesPpi
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (SSA_BIOS_SERVICES_PPI), &SsaBiosServicesPpi);
  ASSERT_EFI_ERROR (Status);
  //Allocating PEI memory for each of the BSSA Structs
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (SSA_COMMON_CONFIG), &SsaCommonConfig);//Address of PEI region saved in SsaCommonConfig
  ASSERT_EFI_ERROR (Status);
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (SSA_RESULTS_CONFIG), &SsaResultsConfig);
  ASSERT_EFI_ERROR (Status);
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (SSA_MEMORY_CONFIG), &SsaMemoryConfig);
  ASSERT_EFI_ERROR (Status);
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (SSA_MEMORY_SERVER_CONFIG), &SsaMemoryServerConfig);
  ASSERT_EFI_ERROR (Status);
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (SSA_MEMORY_DDRT_CONFIG), &SsaMemoryDdrtConfig);
  ASSERT_EFI_ERROR (Status);
  //Allocating PEI memory for the DESCRIPTOR
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (EFI_PEI_PPI_DESCRIPTOR), &SsaBiosServicesPpiDesc);
  ASSERT_EFI_ERROR (Status);

  //Copying into the PEI Memory Region all the struct SSA_BIOS_SERVICES_PPI
  (*PeiServices)->CopyMem ((void*) SsaBiosServicesPpi, (void*) &SsaBiosServicesConst, sizeof (SSA_BIOS_SERVICES_PPI));
  //Copying into the PEI Memory Region all the BSSA Structs
  (*PeiServices)->CopyMem ((void*) SsaCommonConfig, (void*) &SsaCommonConfigConst, sizeof (SSA_COMMON_CONFIG));
  (*PeiServices)->CopyMem ((void*) SsaResultsConfig, (void*) &SsaResultsConfigConst, sizeof (SSA_RESULTS_CONFIG));
  (*PeiServices)->CopyMem ((void*) SsaMemoryConfig, (void*) &SsaMemoryConfigConst, sizeof (SSA_MEMORY_CONFIG));
  (*PeiServices)->CopyMem ((void*) SsaMemoryServerConfig, (void*) &SsaMemoryServerConfigConst, sizeof (SSA_MEMORY_SERVER_CONFIG));
  (*PeiServices)->CopyMem ((void*) SsaMemoryDdrtConfig, (void*) &SsaMemoryDdrtConfigConst, sizeof (SSA_MEMORY_DDRT_CONFIG));

  //The below Chip function saves off the address location of each of the SSA structs into SSA_BIOS_SERVICES_PPI
  CHIP_FUNC_CALL(host, SsaStructAddressLocationsChip (host, SsaBiosServicesPpi, SsaCommonConfig, SsaResultsConfig, SsaMemoryConfig, NULL, SsaMemoryServerConfig, SsaMemoryDdrtConfig));

  //Saving the gSsaBiosServices Flags, Ppi & Guid into the SSA Bios Descriptor
  ZeroMem (SsaBiosServicesPpiDesc, sizeof (EFI_PEI_PPI_DESCRIPTOR));
  SsaBiosServicesPpiDesc->Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  SsaBiosServicesPpiDesc->Guid   = &gSsaBiosServicesPpiGuid;
  SsaBiosServicesPpiDesc->Ppi    = SsaBiosServicesPpi;

  Status = (*PeiServices)->InstallPpi(PeiServices, SsaBiosServicesPpiDesc);
  ASSERT_EFI_ERROR (Status);

#ifdef SSA_LOADER_FLAG
#ifdef SSA_OEM_HOOK
  if ((OemDetectPhysicalPresenceSSA(host) == TRUE) && (host->setup.mem.enableBiosSsaLoader)) {
    SsaBiosLoadStatus(host);
  }
#endif //SSA_OEM_HOOK
#endif //SSA_LOADER_FLAG

  MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
    "BIOS SSA Initialization Completed\n"));
#endif //!defined(RC_SIM) && !defined(MINIBIOS_BUILD)
}

#ifdef SSA_LOADER_FLAG
/**
  Initialize the SsaBiosServices load status.

  @param[in] MrcData  - The MRC global data area.

  @retval Nothing
**/
void
SsaBiosLoadStatus (
  PSYSHOST host
  )
{
#if !defined(RC_SIM) && !defined(MINIBIOS_BUILD)

  EFI_STATUS                             Status;
  UINT8                                  DataBuffer[VAR_BUFFER_SIZE];
  UINT32                                 agentReg;
  UINTN                                  DataSize = sizeof(DataBuffer);
  EFI_PEI_READ_ONLY_VARIABLE2_PPI        *VarAccessPpiPtr = NULL;
  EFI_PEI_SERVICES  **PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;

#ifdef DEBUG
  rcPrintf((host, "Inside SsaBiosLoadStatus() ... \n"));
#endif //DEBUG

DEBUG ((
      DEBUG_ERROR,
      "Inside SsaBiosLoadStatus() ... \n"
      ));

  // Locate the EFI read only variable PPI
  Status = (*PeiServices)->LocatePpi(PeiServices,
    &gEfiPeiReadOnlyVariable2PpiGuid,
    0,
    NULL,
    (VOID **)&VarAccessPpiPtr);
  if(Status == EFI_NOT_FOUND) {
#ifdef DEBUG
      rcPrintf((host, "Read Only Variable PPI NOT found. \n"));
#endif // DEBUG
DEBUG ((
    DEBUG_ERROR, "Read Only Variable PPI NOT found. \n"
    ));//*/
  } else {
#ifdef DEBUG
      rcPrintf((host, "Read Only Variable PPI found! \n"));
#endif // DEBUG

DEBUG ((
    DEBUG_ERROR, "Read Only Variable PPI found! \n"
    ));

  }
  // Check for Target-only
  Status = VarAccessPpiPtr->GetVariable(VarAccessPpiPtr, SYS_CONFIG_NAME, &BssaVarGuid, NULL, &DataSize, (VOID *)DataBuffer);

#ifdef DEBUG
    rcPrintf((host, "SsaBiosLoadStatus() -- After call VarAccessPpiPtr->PeiGetVariable() \n"));
#endif // DEBUG

DEBUG ((
      DEBUG_ERROR, "EvLoaderEntryPoint() -- After call VarAccessPpiPtr->PeiGetVariable()\n"
      ));

  if(Status == EFI_NOT_FOUND) {  // Host-Target Hybrid
#ifdef DEBUG
      rcPrintf((host, "SsaBiosLoadStatus() -- Host-Target Hybrid \n"));
#endif // DEBUG

DEBUG ((
      DEBUG_ERROR, "EvLoaderEntryPoint() -- Host-Target Hybrid \n"
      ));
  // Do nothing
  } else if (Status == EFI_SUCCESS) { // Target-Only
    MRC_EV_AGENT_CMD_STRUCT       agentCmd;
    SSA_BIOS_SYS_CONFIG_HEADER * sysConfigHeaderPtr = NULL;

#ifdef DEBUG
      rcPrintf((host, "SsaBiosLoadStatus() -- Target-Only \n"));
#endif // DEBUG

DEBUG ((
      DEBUG_ERROR, "EvLoaderEntryPoint() -- Target-Only \n"
      ));

  // Setup for target only
  agentCmd.Data = 0;
  agentCmd.Bits.command_pending_execution = MRC_EVAGENT_PENDING_CMD;

  agentCmd.Bits.command_opcode = MRC_EVAGENT_CMD_START_TARGET_ONLY;

  // Extract postcode
  sysConfigHeaderPtr = (SSA_BIOS_SYS_CONFIG_HEADER *)DataBuffer;

  // Setup postcode
  agentCmd.Bits.reserved = 0x0000FFFF & sysConfigHeaderPtr->PostCode;

  // Setup register
  agentReg = agentCmd.Data;

  CHIP_FUNC_CALL(host, SsaBiosLoadStatusChip (host, agentReg));
  } else {
    ASSERT_EFI_ERROR(Status);
  }
#endif //!defined(RC_SIM) && !defined(MINIBIOS_BUILD)
}
#endif //SSA_LOADER_FLAG

#pragma pack (pop)

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
