/**
@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved.
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

@file
  BiosSsaResultsConfig.h

@brief
  This file contains the definition of the BIOS implementation of the BIOS-SSA Results Configuration API.

**/
#ifndef __BIOS_SSA_RESULTS_CONFIG__
#define __BIOS_SSA_RESULTS_CONFIG__

#ifdef SSA_FLAG
#pragma pack (push, 1)

/**
  Function allocates a buffer to collect results data.
  It is made up of a block of metadata of MetadataSize and a block of results
  elements which is ResultsElementSize * ResultsCapacity in size.

  @param[in, out] This              - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      MetadataSize      - Declares the size of the result buffers metadata block.
  @param[in]      ResultElementSize - Declares the size of an individual result element.
  @param[in]      ResultsCapacity   - Declares the number of elements to store results in.

  @retval NotAvailable - There is insufficient memory to create the result buffer.
  @retval Success - Buffer is valid
**/
SSA_STATUS
(EFIAPI BiosCreateResultBuffer) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                MetadataSize,
  UINT32                ResultElementSize,
  UINT32                ResultsCapacity
  );

/**
  Function sets the type of the metadata.

  @param[in, out] This     - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Revision - Incremented when a backwards compatible binary change is made to the results format.

  @retval NotAvailable -  A results buffer has not been created.
  @retval Success - The results buffer has been created.
**/
SSA_STATUS
(EFIAPI BiosSetRevision) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                Revision
  );

/**
  Function gets the type of the metadata.

  @param[in, out] This     - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     Revision - Incremented when a backwards compatible binary change is made to the results format.

  @retval UnsupportedValue - Revision is NULL.
  @retval NotAvailable - A results buffer has not been created.
  @retval Success - metadata is valid.
**/
SSA_STATUS
(EFIAPI BiosGetRevision) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                **Revision
  );

/**
  Function sets the GUID associated with the metadata.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      MetadataType - Address to an EFI_GUID representing a specific metadata type.

  @retval UnsupportedValue -MetadataType is NULL or invalid.
  @retval NotAvailable - A results buffer has not been created.
  @retval Success - GUID set.
**/
SSA_STATUS
(EFIAPI BiosSetMetadataType) (
  SSA_BIOS_SERVICES_PPI *This,
  EFI_GUID              *MetadataType
  );

/**
  Function gets the GUID associated with the metadata.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     MetadataType - Address to an EFI_GUID representing a specific metadata type.

  @retval UnsupportedValue - MetadataType is NULL.
  @retval NotAvailable - A results buffer has not been created.
  @retval Success - GUID associated with the metadata is valid.
**/
SSA_STATUS
(EFIAPI BiosGetMetadataType) (
  SSA_BIOS_SERVICES_PPI *This,
  EFI_GUID              **MetadataType
  );

/**
  Function returns the size of the metadata block.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     MetadataSize - The address to the UINT32 in the results data buffer that holds the size.

  @retval UnsupportedValue - MetadataSize is NULL.
  @retval NotAvailable - a results buffer has not been created.
  @retval Success - MetadataSize is valid.
**/
SSA_STATUS
(EFIAPI BiosGetMetadataSize) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                **MetadataSize
  );

/**
  Function returns the address to the metadata contained in the results data buffer.

  @param[in, out] This     - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     Metadata - The address to the metadata contained in the results data buffer.

  @retval UnsupportedValue - Metadata is NULL.
  @retval NotAvailable - a results buffer has not been created.
  @retval Success - Metadata is valid.
**/
SSA_STATUS
(EFIAPI BiosGetMetadata) (
  SSA_BIOS_SERVICES_PPI *This,
  VOID                  **Metadata
  );

/**
  Function sets the GUID associated with result elements.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      ResultType - The address to an EFI_GUID representing a specific result type.

  @retval UnsupportedValue - ResultType is NULL or invalid.
  @retval NotAvailable - a results buffer has not been created.
  @retval Success - ResultType is set.
**/
SSA_STATUS
(EFIAPI BiosSetResultElementType) (
  SSA_BIOS_SERVICES_PPI *This,
  EFI_GUID              *ResultType
  );

/**
  Function gets the GUID associated with result elements.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     ResultType - The address to an EFI_GUID representing a specific result type.

  @retval UnsupportedValue - ResultType is NULL.
  @retval NotAvailable - a results buffer has not been created.
  @retval Success - ResultType is valid.
**/
SSA_STATUS
(EFIAPI BiosGetResultElementType) (
  SSA_BIOS_SERVICES_PPI *This,
  EFI_GUID              **ResultType
  );

/**
  Function returns the size a result element.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     ResultSize - Returns the address to the UINT32 in the results data buffer that holds the size.

  @retval UnsupportedValue - ResultSize is NULL.
  @retval NotAvailable - a results buffer has not been created.
  @retval Success - ResultSize if valid
**/
SSA_STATUS
(EFIAPI BiosGetResultElementSize) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                **ResultSize
  );

/**
  Function returns how many elements can be stored in the results data buffer.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     ResultCapacity - Returns the address to the UINT32 in the results data buffer that holds the capacity.

  @retval UnsupportedValue - ResultCapacity is NULL.
  @retval NotAvailable - a results buffer has not been created.
  @retval Success - ResultCapacity is valid.
**/
SSA_STATUS
(EFIAPI BiosGetResultElementCapacity) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                **ResultCapacity
  );

/**
  Function returns how many elements are currently stored in the results data buffer.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     ResultCount - Returns the address to the UINT32 in the results data buffer that holds the current element count.

  @retval UnsupportedValue - ResultCount is NULL.
  @retval NotAvailable - a results buffer has not been created.
  @retval Success - ResultCount is valid.
**/
SSA_STATUS
(EFIAPI BiosGetResultElementCount) (
  SSA_BIOS_SERVICES_PPI *This,
  UINT32                **ResultCount
  );

/**
  Function returns the next available result element in the results data buffer.
  While in automatic transfer mode every time a new element is requested the current element count is checked.
  If the buffer is full the entire block is transferred back to the host and the buffer count is reset.
  Note: results remaining in the buffer will be finalized (transferred, etc) either by the loader when the test completes
  or by CreateResultsBuffer() before it frees any preexisting buffer.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     ResultElement - Returns the address to next available results element in the results data buffer.

  @retval UnsupportedValue - ResultElement is NULL.
  @retval NotAvailable - a results buffer has not been created or there are no results elements available.
  @retval Success - ResultElement is valid and has data.
**/
SSA_STATUS
(EFIAPI BiosGetNextResultElement) (
  SSA_BIOS_SERVICES_PPI *This,
  void                  **ResultElement
  );

/**
  The function toggles the explicit mode either on or off.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Mode - Toggle explicit mode on (TRUE) or off (FALSE).

  @retval NotAvailable - a results buffer has not been created.
  @retval Success - Explicit mode set according to the Mode parameter.
**/
SSA_STATUS
(EFIAPI BiosSetExplicitTransferMode) (
  SSA_BIOS_SERVICES_PPI *This,
  BOOLEAN               Mode
  );

/**
  The function returns the current explicit mode value.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in, out] Mode - Returns the current explicit mode, on (TRUE) or off (FALSE).

  @retval UnsupportedValue - Mode is NULL.
  @retval NotAvailable - a results buffer has not been created.
  @retval Success - Mode is valid.
**/
SSA_STATUS
(EFIAPI BiosGetExplicitTransferMode) (
  SSA_BIOS_SERVICES_PPI *This,
  BOOLEAN               *Mode
  );

/**
  The function flushes the used portion of the results buffer and resets the results data index.
  (Used in explicit transfer mode only.) Note: results remaining in the buffer will be finalized
  (transferred, etc) either by the loader when the test completes or by CreateResultsBuffer()
  before it frees any pre-existing buffer.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.

  @retval NotAvailable - a results buffer has not been created or the explicit transfer mode is FALSE.
  @retval Success - Push completed successfully.
**/
SSA_STATUS
(EFIAPI BiosPushResults) (
  SSA_BIOS_SERVICES_PPI *This
  );

#pragma pack (pop)
#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
#endif // __BIOS_SSA_RESULTS_CONFIG__

// end file BiosSsaResultsConfig.h
