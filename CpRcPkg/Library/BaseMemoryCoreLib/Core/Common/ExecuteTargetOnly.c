/** @file
  BIOS SSA loader Target-Only implementation.

@copyright
  Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
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
#include "MrcSsaServices.h"
#include "ExecuteTargetOnly.h"
#include "BssaDebug.h"
#include "EvLoaderPeim.h"

#ifdef SSA_FLAG
#ifdef SSA_LOADER_FLAG
#ifndef MINIBIOS_BUILD
extern EFI_GUID BssaVarGuid;
extern EFI_GUID BssaResultsGuid;
extern EFI_GUID gSsaBiosServicesPpiGuid;

static
VOID *
EvCopyToGlobalMemory (
  VOID *                    Data,
  UINTN                     DataSize
  );

static
VOID 
EvLoadTool (
  PSYSHOST                  host,
  SSA_BIOS_SYS_CONFIG_HEADER *sysConfigHeaderPtr,
  UINT32                    *TotalBufferPtr,
  UINT32                    *ImageHandlePtr
  );

static
VOID 
EvLoadConfig (
  PSYSHOST                  host,
  SSA_BIOS_SYS_CONFIG_HEADER *sysConfigHeaderPtr,
  UINT32                    ConfigCount,
  UINT32                    ConfigIndex,
  UINT32                    *ConfigHandlePtr
  );

static
VOID 
CopyVariableListToCache (
  PSYSHOST                  host,
  UINT32                    DestinationHandle,
  EFI_GUID                  *SourceGuidPtr,
  CHAR16                    *SourceName,
  UINT32                    DestinationSize
  );

static
SSA_STATUS 
SaveResultsToBdat (
  PSYSHOST                  host
  );

extern
UINT32
flushBios (
  void
  );

VOID 
ExecuteTargetOnlyCmd(
  PSYSHOST host
  )
{
  SSA_STATUS                      retStatus;
  EFI_PEI_SERVICES                **PeiServices = NULL;
  SSA_BIOS_SERVICES_PPI           *SsaServicesPpi  = NULL;

  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VarAccessPpiPtr = NULL;

  UINT8                           DataBuffer[VAR_BUFFER_SIZE];
  UINTN                           DataSize = sizeof(DataBuffer);
  SSA_BIOS_SYS_CONFIG_HEADER *    sysConfigHeaderPtr = NULL;
  UINT32                          TotalBuffer = 0;
  UINT32                          ImageHandle = 0;
  UINT32                          ConfigCount = 0;
  UINT32                          ConfigBuffer = 0;
  UINT32                          ConfigVersionHandle = 0;
  UINT32                          ConfigPointerFixupCountHandle = 0;
  UINT32                          ConfigPointerFixupCount = 0;
  UINT32                          PointerIndex = 0;
  UINT32                          ConfigBlockHandle = 0;
  UINT32                          configIndex = 0;
  UINT32                          entryPoint = 0;
  UINT32                          PointerOffset = 0;
  UINT32                          StringPointerLocation = 0;

#ifdef DEBUG
      rcPrintf((host, "Inside ExecuteTargetOnlyCmd() \n"));
#endif // DEBUG

  DEBUG ((
     DEBUG_ERROR,
     "Inside ExecuteTargetOnlyCmd() \n"
     ));

  // Get PEI and BSSA Servcies
  PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  retStatus = (*PeiServices)->LocatePpi(PeiServices, &gSsaBiosServicesPpiGuid, 0, NULL, &SsaServicesPpi);
  ASSERT_EFI_ERROR (retStatus);
  retStatus = (*PeiServices)->LocatePpi(PeiServices, 


                                      &gEfiPeiReadOnlyVariable2PpiGuid,

  0,
  NULL,
  (VOID **)&VarAccessPpiPtr);
  ASSERT_EFI_ERROR (retStatus);

  // Get the BSSA sys configuration EFI variable
  
  ZeroMem (DataBuffer, sizeof(DataBuffer));
  retStatus = VarAccessPpiPtr->GetVariable(VarAccessPpiPtr, SYS_CONFIG_NAME, &BssaVarGuid, NULL, &DataSize, (VOID *)DataBuffer);
  ASSERT_EFI_ERROR (retStatus);

  // Pull in system config
  
  sysConfigHeaderPtr = EvCopyToGlobalMemory(DataBuffer, DataSize);
  if (sysConfigHeaderPtr == NULL) {
    DEBUG ((DEBUG_ERROR, "\nEFI_OUT_OF_RESOURCES!!! AllocatePool() returned NULL pointer.\n"));
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    return;
  }
  ConfigCount = sysConfigHeaderPtr->ConfigCount;

  // Load tool
  EvLoadTool(host, sysConfigHeaderPtr, &TotalBuffer, &ImageHandle);

  DEBUG ((
    DEBUG_ERROR, "Returned from EvLoadTool() \n"
    ));

  // Iterate over each config
  
  if(ConfigCount) { // execute for each config
    for(configIndex = 0; configIndex < ConfigCount; configIndex++) {
      DEBUG ((
        DEBUG_ERROR, "configIndex = %d \n", configIndex
        ));

      // load config
      EvLoadConfig(host, sysConfigHeaderPtr, ConfigCount, configIndex, &ConfigBuffer);
      ConfigVersionHandle = ConfigBuffer;
      ConfigPointerFixupCountHandle = ConfigBuffer + sizeof(UINT32);
      ConfigPointerFixupCount = *(UINT32*)ConfigPointerFixupCountHandle;
      ConfigBlockHandle = ConfigPointerFixupCountHandle + sizeof(UINT32) + (ConfigPointerFixupCount * sizeof(UINT32));

      // Fixup string pointers
      for (PointerIndex = 0; PointerIndex < ConfigPointerFixupCount; PointerIndex++) {
        PointerOffset = *((UINT32*)((ConfigPointerFixupCountHandle + sizeof(UINT32)) + (PointerIndex * sizeof(UINT32))));
        StringPointerLocation = ConfigBlockHandle + PointerOffset;
        *((UINT32*)StringPointerLocation) = *((UINT32*)StringPointerLocation) + ConfigBlockHandle;
      }

      // run tool
      DEBUG ((
      DEBUG_ERROR, "About to run tool... \n"
      ));

      entryPoint = GetPeEntrypoint (host, (UINT8 *)ImageHandle);
      ((EvToolEntryPoint*)entryPoint) (SsaServicesPpi, (UINT32 *)ConfigBlockHandle);

      DEBUG ((
      DEBUG_ERROR, "The tool has finished execution. \n"
      ));
      DEBUG ((
      DEBUG_ERROR, "SsaServicesPpi->SsaResultsConfig->ResultsData =  0x%08X \n", SsaServicesPpi->SsaResultsConfig->ResultsData
      ));

    // return results?
    SaveResultsToBdat(host);
    }
  }
  else { // execute a test w/o a config
    // run tool
    DEBUG ((
      DEBUG_ERROR, "About to run tool... \n"
      ));

    entryPoint = GetPeEntrypoint (host, (UINT8 *)ImageHandle);
    ((EvToolEntryPoint*)entryPoint) (SsaServicesPpi, NULL /*(UINT32 *)inputBufferForITP*/);

    // return result?
    SaveResultsToBdat(host);
  }
}

static
VOID *
EvCopyToGlobalMemory(
  VOID * Data,
  UINTN  DataSize)
{
  VOID * NewBlockPtr = AllocatePool(DataSize);

  CopyMem(NewBlockPtr, Data, DataSize);

  return NewBlockPtr;
}


static
VOID 
EvLoadTool(
  PSYSHOST                  host,
  SSA_BIOS_SYS_CONFIG_HEADER *sysConfigHeaderPtr,
  UINT32                    *TotalBufferPtr,
  UINT32                    *ImageHandlePtr
  )
{
  EFI_PEI_SERVICES                **PeiServices = NULL;
  SSA_BIOS_SERVICES_PPI           *SsaServicesPpi  = NULL;


  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *VarAccessPpiPtr = NULL;
  EFI_STATUS  Status;
  UINT32      ToolSize    = 0;
  UINT32      imageHandle = 0;
  UINT32      TotalBuffer = 0;

#ifdef DEBUG
      rcPrintf((host, "EvLoadTool() \n"));
#endif // DEBUG

  // Get PEI and BSSA Servcies

  PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  Status = (*PeiServices)->LocatePpi(PeiServices, &gSsaBiosServicesPpiGuid, 0, NULL, &SsaServicesPpi);
  ASSERT_EFI_ERROR (Status);
  // reassemble tool

  ASSERT(sysConfigHeaderPtr != NULL);
  if(sysConfigHeaderPtr == NULL){
    DEBUG ((DEBUG_ERROR, "\nERROR: EvLoadTool () - EFI INVALID PARAMETER.\n"));
    return;
  }

  ToolSize    = sysConfigHeaderPtr->ToolSize;

  Status = (*PeiServices)->LocatePpi(PeiServices, 
                                      &gEfiPeiReadOnlyVariable2PpiGuid,
  0,
  NULL,
  (VOID **)&VarAccessPpiPtr);
  ASSERT_EFI_ERROR (Status);

  //
  // Reserve 4KB buffers below and above the tool image
  //
  TotalBuffer = (UINT32) SsaServicesPpi->SsaCommonConfig->Malloc (SsaServicesPpi, (1024 * 4) + ToolSize + (1024 * 4));
  imageHandle = TotalBuffer + (1024 * 4);

  if (!TotalBuffer)
  {
    DEBUG ((
      DEBUG_ERROR, "EvLoadTool()  !TotalBuffer ... \n"
      ));
  }

  DEBUG ((
      DEBUG_ERROR, "About to call CopyVariableListToCache() \n"
      ));
  CopyVariableListToCache(host, imageHandle, &BssaVarGuid, TOOL_BINARY_NAME, ToolSize);
  DEBUG ((
      DEBUG_ERROR, "Just returned from CopyVariableListToCache() \n"
      ));
  
  DEBUG ((
      DEBUG_ERROR, "About to call PlacePeImage((UINT8 *)imageHandle) \n"
      ));
  PlacePeImage(host, (UINT8 *)imageHandle);
  flushBios ();

  DEBUG ((
      DEBUG_ERROR, "Just called PlacePeImage((UINT8 *)imageHandle) \n"
      ));

  // return the buffers and tool image addresses
  *TotalBufferPtr     = TotalBuffer;
  *ImageHandlePtr   = imageHandle;
}


static
VOID 
EvLoadConfig(
  PSYSHOST                  host,
  SSA_BIOS_SYS_CONFIG_HEADER *sysConfigHeaderPtr,
  UINT32                    ConfigCount,
  UINT32                    ConfigIndex,
  UINT32                    *ConfigHandlePtr
  )
{
  SSA_STATUS                      Status;
  EFI_PEI_SERVICES                **PeiServices = NULL;
  SSA_BIOS_SERVICES_PPI           *SsaServicesPpi  = NULL;
  UINT32                          ConfigBuffer = 0;
  UINT32                          SysConfigHandle = 0;
  VOID                            * VarNameHeaderPtr = NULL;
  UINT32                          *VarSizePtr = 0;
  CHAR16                          HeadVarName[VAR_NULL_NAME_LENGTH];

#ifdef DEBUG
      rcPrintf((host, "EvLoadConfig() \n"));
#endif // DEBUG

  // Get PEI and BSSA Servcies
  PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  Status = (*PeiServices)->LocatePpi(PeiServices, &gSsaBiosServicesPpiGuid, 0, NULL, &SsaServicesPpi);
  ASSERT_EFI_ERROR (Status);

  SysConfigHandle = (UINT32)sysConfigHeaderPtr;

  // Lookup config variable name address
  DEBUG ((
      DEBUG_ERROR, "Lookup config variable name... \n"
      ));
  VarNameHeaderPtr = (VOID*)((SysConfigHandle + sizeof(SSA_BIOS_SYS_CONFIG_HEADER)) + (ConfigIndex * VAR_NAME_SIZE));

  ZeroMem(HeadVarName, VAR_NULL_NAME_SIZE);

  CopyMem(HeadVarName, VarNameHeaderPtr, VAR_NAME_SIZE);

  DEBUG ((
      DEBUG_ERROR, "HeadVarName = %s \n", HeadVarName
      ));

  // Lookup config variable size address
  DEBUG ((
      DEBUG_ERROR, "Lookup config variable size... \n"
      ));

  // *****************************************************************************************
  // *****************************************************************************************

  VarSizePtr = (UINT32 *)((SysConfigHandle + sizeof(SSA_BIOS_SYS_CONFIG_HEADER)) + (ConfigCount * VAR_NAME_SIZE) + ((ConfigIndex - 2) * sizeof(UINT32)));

  DEBUG ((
      DEBUG_ERROR, "VarSize (-2) = %d (0x%08X) \n", *VarSizePtr, *VarSizePtr
      ));
  VarSizePtr = (UINT32 *)((SysConfigHandle + sizeof(SSA_BIOS_SYS_CONFIG_HEADER)) + (ConfigCount * VAR_NAME_SIZE) + ((ConfigIndex - 1) * sizeof(UINT32)));

  DEBUG ((
      DEBUG_ERROR, "VarSize (-1) = %d (0x%08X) \n", *VarSizePtr, *VarSizePtr
      ));
  VarSizePtr = (UINT32 *)((SysConfigHandle + sizeof(SSA_BIOS_SYS_CONFIG_HEADER)) + (ConfigCount * VAR_NAME_SIZE) + (ConfigIndex * sizeof(UINT32)));

  DEBUG ((
      DEBUG_ERROR, "VarSize = %d (0x%08X) \n", *VarSizePtr, *VarSizePtr
      ));
  VarSizePtr = (UINT32 *)((SysConfigHandle + sizeof(SSA_BIOS_SYS_CONFIG_HEADER)) + (ConfigCount * VAR_NAME_SIZE) + ((ConfigIndex + 1) * sizeof(UINT32)));

  DEBUG ((
      DEBUG_ERROR, "VarSize (+1) = %d (0x%08X) \n", *VarSizePtr, *VarSizePtr
      ));
  VarSizePtr = (UINT32 *)((SysConfigHandle + sizeof(SSA_BIOS_SYS_CONFIG_HEADER)) + (ConfigCount * VAR_NAME_SIZE) + ((ConfigIndex + 2) * sizeof(UINT32)));

  DEBUG ((
      DEBUG_ERROR, "VarSize (+2) = %d (0x%08X) \n", *VarSizePtr, *VarSizePtr
      ));

  // ***************************************************************************************** 
  // *****************************************************************************************

  VarSizePtr = (UINT32 *)((SysConfigHandle + sizeof(SSA_BIOS_SYS_CONFIG_HEADER)) + (ConfigCount * VAR_NAME_SIZE) + (ConfigIndex * sizeof(UINT32)));

  DEBUG ((
      DEBUG_ERROR, "VarSize = %d \n", *VarSizePtr
      ));

  // Allocate the config cache
  ConfigBuffer = (UINT32)SsaServicesPpi->SsaCommonConfig->Malloc(SsaServicesPpi, *VarSizePtr);

  if (!ConfigBuffer) {
    DEBUG ((
      DEBUG_ERROR, "EvLoadConfig()  !ConfigBuffer ... \n"
      ));
  }

  DEBUG ((
      DEBUG_ERROR, "About to call CopyVariableListToCache() \n"
      ));
  CopyVariableListToCache(host, ConfigBuffer, &BssaVarGuid, HeadVarName, *VarSizePtr);
  DEBUG ((
      DEBUG_ERROR, "Just returned from CopyVariableListToCache() \n"
      ));

  *ConfigHandlePtr = ConfigBuffer;
}


static
VOID 
CopyVariableListToCache(
  PSYSHOST                  host,
  UINT32                    DestinationHandle,
  EFI_GUID                  *SourceVarGuidPtr,
  CHAR16                    *SourceVarName,
  UINT32                    DestinationSize
  )
{
  EFI_PEI_SERVICES                **PeiServices = NULL;
  SSA_BIOS_SERVICES_PPI           *SsaServicesPpi  = NULL;

  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *VarAccessPpiPtr = NULL;
  EFI_STATUS  Status;
  UINT8       DataBuffer[VAR_BUFFER_SIZE];
  UINT8 *     DataBufferPtr = DataBuffer;
  UINTN       DataSize = sizeof(DataBuffer);
  UINT32      BytesRead = 0;
  VOID *      DestinationFillPtr = NULL;
  CHAR16      NextVarName[VAR_NULL_NAME_LENGTH];
  UINT32      GetVarIter = 0;

  DEBUG ((
      DEBUG_ERROR, "Entered CopyVariableListToCache() \n"
      ));
  BSSA_DEBUG_ERROR(host, DEBUG_ERROR, "SourceVarName: %s\n", SourceVarName);
  BSSA_ASSERT(host, (SourceVarGuidPtr != NULL));
  BSSA_ASSERT(host, (SourceVarName != NULL));
  BSSA_ASSERT(host, (DestinationSize != 0));

  // Get PEI and BSSA Servcies

  PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  Status = (*PeiServices)->LocatePpi(PeiServices, &gSsaBiosServicesPpiGuid, 0, NULL, &SsaServicesPpi);
  ASSERT_EFI_ERROR (Status);
  Status = (*PeiServices)->LocatePpi(PeiServices, 

  &gEfiPeiReadOnlyVariable2PpiGuid,

  0,
  NULL,
  (VOID **)&VarAccessPpiPtr);
  ASSERT_EFI_ERROR (Status);
  Status = VarAccessPpiPtr->GetVariable(VarAccessPpiPtr, SourceVarName, SourceVarGuidPtr, NULL, &DataSize, DataBufferPtr);

  DEBUG ((
      DEBUG_ERROR, "Just called VarAccessPpiPtr->PeiGetVariable() \n"
      ));
  ASSERT_EFI_ERROR(Status);
  BSSA_DEBUG_ERROR(host, DEBUG_ERROR, "VAR_NAME_SIZE: %d\n", VAR_NAME_SIZE);
  BSSA_DEBUG_ERROR(host, DEBUG_ERROR, "DataSize: %d\n", DataSize);
  BSSA_DEBUG_ERROR(host, DEBUG_ERROR, "VAR_BUFFER_SIZE: %d\n", VAR_BUFFER_SIZE);


  ZeroMem(NextVarName, VAR_NULL_NAME_SIZE);
  CopyMem(NextVarName, DataBufferPtr, VAR_NAME_SIZE);
  BytesRead += (DataSize - VAR_NAME_SIZE);

  DestinationFillPtr = (VOID *)DestinationHandle;  // start at the front of the image block
  CopyMem(DestinationFillPtr, (DataBufferPtr + VAR_NAME_SIZE), (DataSize - VAR_NAME_SIZE));    // copy all but the first name bytes

  while(StrCmp(NextVarName, NULL_STRING) != 0)
  {
    UINT32 ActualDataSize = 0;

    ASSERT((UINT32)DestinationFillPtr < ((UINT32)DestinationHandle + DestinationSize));            // is it inside of the image block?

    DestinationFillPtr = (VOID *)((UINTN)DestinationFillPtr + (DataSize - VAR_NAME_SIZE));  // increment where in image to copy to
    // *******************************************************************
    // *******************************************************************
    GetVarIter++;
    DEBUG ((
      DEBUG_ERROR, "while(EfiStrCmp(NextVarName, NULL_STRING) != 0) iteration: %d \n", GetVarIter
      ));
    DEBUG ((
      DEBUG_ERROR, "(UINT32)DestinationFillPtr = 0x%08X \n", (UINT32)DestinationFillPtr
      ));
    DEBUG ((
      DEBUG_ERROR, "(UINT32)DestinationHandle = 0x%08X \n", (UINT32)DestinationHandle
      ));
    DEBUG ((
      DEBUG_ERROR, "DestinationSize = 0x%08X \n", DestinationSize
      ));
    DEBUG ((
      DEBUG_ERROR, "((UINT32)DestinationHandle + DestinationSize) = 0x%08X \n", ((UINT32)DestinationHandle + DestinationSize)
      ));
    DEBUG ((
      DEBUG_ERROR, "(((UINT32)DestinationHandle + DestinationSize) - (UINT32)DestinationFillPtr) = 0x%08X \n", (((UINT32)DestinationHandle + DestinationSize) - (UINT32)DestinationFillPtr)
      ));
    DEBUG ((
      DEBUG_ERROR, "Is 0x%08X < 0x%08X ? \n", (UINT32)DestinationFillPtr, ((UINT32)DestinationHandle + DestinationSize)
      ));
    // *******************************************************************
    // *******************************************************************

    Status = VarAccessPpiPtr->GetVariable(VarAccessPpiPtr, NextVarName, SourceVarGuidPtr, NULL, &DataSize, DataBufferPtr);
    ASSERT_EFI_ERROR(Status);
    DEBUG ((
      DEBUG_ERROR, "About to call EfiCommonLibZeroMem(NextVarName, VAR_NULL_NAME_LENGTH)... \n"
      ));

    ZeroMem(NextVarName, VAR_NULL_NAME_SIZE);
    DEBUG ((
      DEBUG_ERROR, "EfiCommonLibCopyMem(NextVarName, DataBufferPtr, VAR_NAME_LENGTH)... \n"
      ));

    CopyMem(NextVarName, DataBufferPtr, VAR_NAME_SIZE);
    DEBUG ((
      DEBUG_ERROR, "NextVarName = %s \n", NextVarName
      ));
    ActualDataSize = (DataSize - VAR_NAME_SIZE);
    DEBUG ((
      DEBUG_ERROR, "ActualDataSize = %d \n", ActualDataSize
      ));
    BytesRead += ActualDataSize;

    if(ActualDataSize > 0)
    {
      ASSERT((UINT32)DestinationFillPtr >= DestinationHandle );                                         // Is it above the bottom?
      ASSERT(((UINT32)DestinationFillPtr + ActualDataSize) <=  (DestinationHandle + DestinationSize) ); // Is it below the expected top?
      CopyMem(DestinationFillPtr, (DataBufferPtr + VAR_NAME_SIZE), ActualDataSize);          // copy all but the first name bytes
      flushBios();
    }
  };

  DEBUG ((
      DEBUG_ERROR, "ASSERT(%d == %d) \n", BytesRead, DestinationSize
      ));
  ASSERT(BytesRead == DestinationSize);
  
}


static
SSA_STATUS 
SaveResultsToBdat (
  PSYSHOST host
  )
{
  SSA_STATUS                      RetStatus;
  EFI_PEI_SERVICES                **PeiServices = NULL;
  SSA_BIOS_SERVICES_PPI           *SsaServicesPpi  = NULL;
  UINT8 *                         ResultsData = NULL;
  RESULTS_DATA_HDR *              ResultsDataHdr = NULL;
  UINT32                          ResultsDataSize = 0;
  
  DEBUG ((
      DEBUG_ERROR,
      "Sending results via SaveToBdat()... \n"
      ));

  // Get PEI and BSSA Services
  PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  RetStatus = (*PeiServices)->LocatePpi(PeiServices, &gSsaBiosServicesPpiGuid, 0, NULL, &SsaServicesPpi);
  ASSERT_EFI_ERROR (RetStatus);
  ResultsData = (UINT8*)SsaServicesPpi->SsaResultsConfig->ResultsData;
  ResultsDataHdr = (RESULTS_DATA_HDR *) ResultsData;
  //Trim the unused result entries.
  ResultsDataHdr->RsBlock.ResultCapacity = ResultsDataHdr->RsBlock.ResultElementCount;
  
  //ResultsDataSize should be less than HOB size
  ResultsDataSize = sizeof (RESULTS_DATA_HDR) + ResultsDataHdr->MdBlock.MetadataSize + (ResultsDataHdr->RsBlock.ResultElementSize * ResultsDataHdr->RsBlock.ResultCapacity);
  RetStatus = SsaServicesPpi->SsaCommonConfig->SaveToBdat (SsaServicesPpi, &BssaResultsGuid, ResultsData, ResultsDataSize);

  //Freeing up the temporary memory range in the heap that is used to save of results from the 'Loader SSA' method
  SsaServicesPpi->SsaCommonConfig->Free(SsaServicesPpi, (VOID*)ResultsData);
  SsaServicesPpi->SsaResultsConfig->ResultsData = NULL;

  return RetStatus;
}
#endif //MINIBIOS_BUILD
#endif //SSA_LOADER_FLAG

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
