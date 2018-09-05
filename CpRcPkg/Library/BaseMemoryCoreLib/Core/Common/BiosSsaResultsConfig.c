/** @file
  This file contains the BIOS implementation of the BIOS-SSA Results Configuration API.

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights
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

#include "SysFunc.h"  //MemSetLocal(), MemCopy(), SysHost-MRC_SSA_SERVICES_SET
#include "MrcSsaServices.h"


#ifndef MINIBIOS_BUILD
#include <Library/BaseMemoryLib.h>
#endif //MINIBIOS_BUILD


#ifdef SSA_FLAG

/**
Function checks the GUID.

@param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
@param[in]      Guid        - The address to the given EFI_GUID.

@retval Success, UnsupportedValue.
**/
static
SSA_STATUS
EfiGuidCheck (
     SSA_BIOS_SERVICES_PPI *This,
     EFI_GUID              *Guid
     )
{
  SSA_STATUS            Status;
  UINT8                 Check;
  UINT8                 Index;

  Status = UnsupportedValue;
  if (Guid != NULL) {
    for (Index = 0, Check = 0; Index < sizeof (EFI_GUID); Index++) {
      Check |= ((UINT8 *) Guid)[Index];
    }

    if (Check != 0) {
      Status = Success;
    }
  }

  return Status;
}

/**
Function checks and sets the GUID.

@param[in, out] PeiServices - An indirect pointer to the PEI Services Table published by the PEI Foundation.
@param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
@param[in]      Source      - The address to the given EFI_GUID.
@param[in, out] Destination - Where to save the given EFI_GUID if it is valid.

@retval Success, UnsupportedValue.
**/
static
SSA_STATUS
EfiGuidCheckAndSave (
     SSA_BIOS_SERVICES_PPI *This,
     EFI_GUID              *Source,
     EFI_GUID              *Destination
     )
{
  SSA_STATUS            Status;

  Status = UnsupportedValue;
  if (Destination != NULL) {
    Status = EfiGuidCheck (This, Source);
    if (Status == Success) {
#ifndef MINIBIOS_BUILD
      CopyMem((UINT8 *) Destination, (UINT8 *) Source, sizeof (EFI_GUID));
#endif //MINIBIOS_BUILD
    }
  }

  return Status;
}

/**
  Function allocates a buffer to collect results data.
  It is made up of a block of metadata of MetadataSize and a block of results
  elements which is ResultsElementSize * ResultsCapacity in size.

  @param[in, out] This              - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      MetadataSize      - Declares the size of the result buffers metadata block.
  @param[in]      ResultElementSize - Declares the size of an individual result element.
  @param[in]      ResultsCapacity   - Declares the number of elements to store results in.

  @retval Success, UnsupportedValue, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosCreateResultBuffer) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                MetadataSize,
  UINT32                ResultElementSize,
  UINT32                ResultsCapacity
  )
{
  SSA_STATUS                 Status = UnsupportedValue;
  struct sysHost             *host;
  RESULTS_DATA_HDR           *ResultsDataHdr;
  UINT32                     ResultsDataSize;

  host = NULL;

  if ((MetadataSize != 0) || (ResultElementSize != 0) || (ResultsCapacity != 0)) {
    if (This->SsaResultsConfig->ResultsData != NULL) {
      This->SsaCommonConfig->Free (This, This->SsaResultsConfig->ResultsData);
    }
    ResultsDataSize                    = sizeof (RESULTS_DATA_HDR) + MetadataSize + (ResultElementSize * ResultsCapacity);

	//If results for one BSSA test run are more than the max size of a HOB, we don't save to BDAT
	if (ResultsDataSize > MAX_HOB_ENTRY_SIZE) {
      This->SsaResultsConfig->ResultsData = NULL;
	} else {
      This->SsaResultsConfig->ResultsData = This->SsaCommonConfig->Malloc (This, ResultsDataSize);	
	}
	
    if (This->SsaResultsConfig->ResultsData != NULL) {
 #ifndef MINIBIOS_BUILD
      ZeroMem(This->SsaResultsConfig->ResultsData, ResultsDataSize);
 #endif //MINIBIOS_BUILD
      ResultsDataHdr                            = (RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData;
      ResultsDataHdr->MdBlock.MetadataStart     = (VOID *) ((UINT32) ResultsDataHdr + sizeof (RESULTS_DATA_HDR));
      ResultsDataHdr->MdBlock.MetadataSize      = MetadataSize;
      ResultsDataHdr->RsBlock.ResultStart       = (VOID *) ((UINT32) (ResultsDataHdr->MdBlock.MetadataStart) + MetadataSize);
      ResultsDataHdr->RsBlock.ResultElementSize = ResultElementSize;
      ResultsDataHdr->RsBlock.ResultCapacity    = ResultsCapacity;
      Status = Success;
    } else {
      Status = NotAvailable;
    }
  }

  return Status;
}

/**
  Function sets the type of the metadata.

  @param[in, out] This     - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Revision - Incremented when a backwards compatible binary change is made to the results format.

  @retval Success, NotYetAvailable.
**/
SSA_STATUS
(EFIAPI BiosSetRevision) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                Revision
  )
{
  RESULTS_DATA_HDR *ResultsDataHdr = NULL;

  if (This->SsaResultsConfig->ResultsData != NULL) {
    ResultsDataHdr = (RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData;
    ResultsDataHdr->Revision = Revision;
    return (Success);
  }

  return NotAvailable;
}

/**
  Function gets the type of the metadata.

  @param[in, out] This     - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Revision - Incremented when a backwards compatible binary change is made to the results format.

  @retval Success, NotYetAvailable.
**/
SSA_STATUS
(EFIAPI BiosGetRevision) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                **Revision
  )
{
  RESULTS_DATA_HDR *ResultsDataHdr = NULL;

  if (This->SsaResultsConfig->ResultsData != NULL) {
   ResultsDataHdr = (RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData;
   *Revision = &ResultsDataHdr->Revision;
   return (Success);
  }

  return NotAvailable;
}

/**
  Function sets the GUID associated with the metadata.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      MetadataType - Address to an EFI_GUID representing a specific metadata type.

  @retval Success, UnsupportedValue, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosSetMetadataType) (
  SSA_BIOS_SERVICES_PPI *This,
  EFI_GUID              *MetadataType
  )
{
  RESULTS_DATA_HDR *ResultsDataHdr = NULL;

  if (This->SsaResultsConfig->ResultsData != NULL) {
   ResultsDataHdr = (RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData;
   return (EfiGuidCheckAndSave (This, MetadataType, &ResultsDataHdr->MdBlock.MetadataType));
  }

  return NotAvailable;
}

/**
  Function gets the GUID associated with the metadata.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in, out] MetadataType - Address to an EFI_GUID representing a specific metadata type.

  @retval Success, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosGetMetadataType) (
  SSA_BIOS_SERVICES_PPI *This,
  EFI_GUID              **MetadataType
  )
{
  if (This->SsaResultsConfig->ResultsData != NULL) {
    *MetadataType = &(((RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData)->MdBlock.MetadataType);
   return (Success);
  }

  return NotAvailable;
}

/**
  Function returns the size of the metadata block.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in, out] MetadataType - The address to the UINT32 in the results data buffer that holds the size.

  @retval Success, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosGetMetadataSize) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                **MetadataSize
  )
{
  if (This->SsaResultsConfig->ResultsData != NULL) {
    *MetadataSize = &(((RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData)->MdBlock.MetadataSize);
    return (Success);
  }

  return NotAvailable;
}

/**
  Function returns the address to the metadata contained in the results data buffer.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in, out] Metadata    - The address to the metadata contained in the results data buffer.

  @retval Success, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosGetMetadata) (
  SSA_BIOS_SERVICES_PPI *This,
  VOID                  **Metadata
  )
{
  if (This->SsaResultsConfig->ResultsData != NULL) {
    *Metadata = ((RESULTS_DATA_HDR *) (This->SsaResultsConfig->ResultsData))->MdBlock.MetadataStart;
    return (Success);
  }

  return NotAvailable;
}

/**
  Function sets the GUID associated with result elements.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      ResultType - The address to an EFI_GUID representing a specific result type.

  @retval Success, UnsupportedValue, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosSetResultElementType) (
  SSA_BIOS_SERVICES_PPI *This,
  EFI_GUID              *ResultType
  )
{
  RESULTS_DATA_HDR *ResultsDataHdr;

  if (This->SsaResultsConfig->ResultsData != NULL) {
    ResultsDataHdr = (RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData;
    return (EfiGuidCheckAndSave (This, ResultType, &ResultsDataHdr->RsBlock.ResultType));
  }

  return NotAvailable;
}

/**
  Function gets the GUID associated with result elements.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in, out] ResultType - The address to an EFI_GUID representing a specific result type.

  @retval Success, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosGetResultElementType) (
  SSA_BIOS_SERVICES_PPI *This,
  EFI_GUID              **ResultType
  )
{
  if (This->SsaResultsConfig->ResultsData != NULL) {
    *ResultType = &(((RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData)->RsBlock.ResultType);
    return (Success);
  }

  return NotAvailable;
}

/**
  Function returns the size a result element.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in, out] ResultSize - Returns the address to the UINT32 in the results data buffer that holds the size.

  @retval Success, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosGetResultElementSize) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                **ResultSize
  )
{
  if (This->SsaResultsConfig->ResultsData != NULL) {
   *ResultSize = &(((RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData)->RsBlock.ResultElementSize);
   return (Success);
  }

  return NotAvailable;
}

/**
  Function returns how many elements can be stored in the results data buffer.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in, out] ResultCapacity - Returns the address to the UINT32 in the results data buffer that holds the capacity.

  @retval Success, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosGetResultElementCapacity) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                **ResultCapacity
  )
{
  if (This->SsaResultsConfig->ResultsData != NULL) {
    *ResultCapacity = (UINT32 *) &(((RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData)->RsBlock.ResultCapacity);
    return (Success);
  }

  return NotAvailable;
}

/**
  Function returns how many elements are currently stored in the results data buffer.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in, out] ResultCount - Returns the address to the UINT32 in the results data buffer that holds the current element count.

  @retval Success, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosGetResultElementCount) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                **ResultCount
  )
{
  if (This->SsaResultsConfig->ResultsData != NULL) {
    *ResultCount = (UINT32 *) &(((RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData)->RsBlock.ResultElementCount);
    return (Success);
  }

  return NotAvailable;
}

/**
  Function returns the next available result element in the results data buffer.
  While in automatic transfer mode every time a new element is requested the current element count is checked.
  If the buffer is full the entire block is transferred back to the host and the buffer count is reset.
  Note: results remaining in the buffer will be finalized (transferred, etc) either by the loader when the test completes
  or by CreateResultsBuffer() before it frees any preexisting buffer.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in, out] ResultElement - Returns the address to next available results element in the results data buffer.

  @retval Success, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosGetNextResultElement) (
  SSA_BIOS_SERVICES_PPI *This,
  void                  **ResultElement
  )
{
  UINT32                           ResultsStart;
  UINT32                           CurrentResult;
  RESULTS_DATA_HDR *ResultsDataHdr = NULL;

  if (This->SsaResultsConfig->ResultsData != NULL) {
    ResultsDataHdr = (RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData;
    if (ResultsDataHdr->RsBlock.ResultElementCount < ResultsDataHdr->RsBlock.ResultCapacity) {
      ResultsStart   = (UINT32) (ResultsDataHdr->RsBlock.ResultStart);
      CurrentResult  = ResultsDataHdr->RsBlock.ResultElementCount * ResultsDataHdr->RsBlock.ResultElementSize;
      *ResultElement = (VOID *) (ResultsStart + CurrentResult);
      ResultsDataHdr->RsBlock.ResultElementCount++;
      return (Success);
    } else {
      return (NotAvailable);
    }
  }

  return NotAvailable;
}

/**
  The function toggles the explicit mode either on or off.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Mode - Toggle explicit mode on (TRUE) or off (FALSE).

  @retval Success, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosSetExplicitTransferMode) (
  SSA_BIOS_SERVICES_PPI *This,
  BOOLEAN               Mode
  )
{
  if (This->SsaResultsConfig->ResultsData != NULL) {
    (((RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData)->TransferMode) = Mode ? 1 : 0;
    return (Success);
  }

  return NotAvailable;
}

/**
  The function returns the current explicit mode value.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in, out] Mode - Returns the current explicit mode, on (TRUE) or off (FALSE).

  @retval Success, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosGetExplicitTransferMode) (
  SSA_BIOS_SERVICES_PPI *This,
  BOOLEAN               *Mode
  )
{
  if (This->SsaResultsConfig->ResultsData != NULL) {
    *Mode = ((RESULTS_DATA_HDR *) (This->SsaResultsConfig->ResultsData))->TransferMode;
    return (Success);
  }

  return NotAvailable;
}

/**
  The function flushes the used portion of the results buffer and resets the results data index.
  (Used in explicit transfer mode only.) Note: results remaining in the buffer will be finalized
  (transferred, etc) either by the loader when the test completes or by CreateResultsBuffer()
  before it frees any pre-existing buffer.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.

  @retval Success, NotAvailable.
**/
SSA_STATUS
(EFIAPI BiosPushResults) (
  SSA_BIOS_SERVICES_PPI *This
  )
{
  RESULTS_DATA_HDR *ResultsDataHdr = NULL;

  if (This->SsaResultsConfig->ResultsData != NULL) {
    ResultsDataHdr = (RESULTS_DATA_HDR *) This->SsaResultsConfig->ResultsData;
    if (ResultsDataHdr->TransferMode) {
      ResultsDataHdr->RsBlock.ResultElementCount = 0;
      return (Success);
    }
  }

  return NotAvailable;
}

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
// End file BiosSsaResultsConfig.c
