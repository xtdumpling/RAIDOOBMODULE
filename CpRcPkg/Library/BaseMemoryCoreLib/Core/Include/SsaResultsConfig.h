/**
@copyright
  Copyright (c) 2014-2015 Intel Corporation. All rights reserved.
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
  SsaResultsConfig.h

@brief
  This file contains the definition of the BIOS-SSA Results Configuration API.

  NOTE: This file is only intended to be included from MrcSsaServices.h.  It
  cannot be included separately.

**/
#ifndef _SsaResultsConfig_h_
#define _SsaResultsConfig_h_

#ifdef SSA_FLAG

// BIOS-SSA Result Configuration API revision
#define SSA_REVISION_RESULT ((0x01 << 24) | (0x00 << 16) | (0x00 << 8) | 0x00)

#pragma pack (push, 1)

///
/// SSA results buffer header.
///
typedef struct {
  UINT32  Revision;
  BOOLEAN TransferMode;
  struct {
    VOID     *MetadataStart;
    UINT32   MetadataSize;
    EFI_GUID MetadataType;
  } MdBlock;
  struct {
    VOID     *ResultStart;
    EFI_GUID ResultType;
    UINT32   ResultElementSize;
    INT32    ResultCapacity;
    INT32    ResultElementCount;
  } RsBlock;
} RESULTS_DATA_HDR;

/**
  Function allocates a buffer to collect results data.
  It is made up of a block of metadata of MetadataSize and a block of results
  elements which is ResultsElementSize * ResultsCapacity in size.

  @param[in, out] This              - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      MetadataSize      - Declares the size of the result buffers metadata block.
  @param[in]      ResultElementSize - Declares the size of an individual result element.
  @param[in]      ResultsCapacity   - Declares the number of elements to store results in.

  @retval SUCCESS
  @retval NotAvailable if there is insufficient memory to create the result buffer.
**/
typedef
SSA_STATUS
(EFIAPI * CREATE_RESULTS_BUFFER) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT32                    MetadataSize,
  IN UINT32                    ResultElementSize,
  IN UINT32                    ResultsCapacity
  );

/**
  Function sets the type of the metadata.

  @param[in, out] This     - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Revision - Incremented when a backwards compatible binary change is made to the results format.

  @retval SUCCESS
  @retval NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * SET_REVISION) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN UINT32                    Revision
  );

/**
  Function gets the type of the metadata.

  @param[in, out] This     - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     Revision - Incremented when a backwards compatible binary change is made to the results format.

  @retval SUCCESS
  @retval UnsupportedValue if Revision is NULL.  Else NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * GET_REVISION) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT UINT32                   **Revision
  );

/**
  Function sets the GUID associated with the metadata.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      MetadataType - Address to an EFI_GUID representing a specific metadata type.

  @retval SUCCESS
  @retval UnsupportedValue if MetadataType is NULL or invalid.  Else NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * SET_METADATA_TYPE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN EFI_GUID                  *MetadataType
  );

/**
  Function gets the GUID associated with the metadata.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     MetadataType - Address to an EFI_GUID representing a specific metadata type.

  @retval SUCCESS
  @retval UnsupportedValue if MetadataType is NULL.  Else NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * GET_METADATA_TYPE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT EFI_GUID                 **MetadataType
  );

/**
  Function returns the size of the metadata block.

  @param[in, out] This         - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     MetadataSize - The address to the UINT32 in the results data buffer that holds the size.

  @retval SUCCESS
  @retval UnsupportedValue if MetadataSize is NULL.  Else NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * GET_METADATA_SIZE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT UINT32                   **MetadataSize
  );

/**
  Function returns the address to the metadata contained in the results data buffer.

  @param[in, out] This     - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     Metadata - The address to the metadata contained in the results data buffer.

  @retval SUCCESS
  @retval UnsupportedValue if Metadata is NULL.  Else NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * GET_METADATA) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT VOID                     **Metadata
  );

/**
  Function sets the GUID associated with result elements.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      ResultType - The address to an EFI_GUID representing a specific result type.

  @retval SUCCESS
  @retval UnsupportedValue if ResultType is NULL or invalid.  Else NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * SET_RESULT_ELEMENT_TYPE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN EFI_GUID                  *ResultType
  );

/**
  Function gets the GUID associated with result elements.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     ResultType - The address to an EFI_GUID representing a specific result type.

  @retval SUCCESS
  @retval UnsupportedValue if ResultType is NULL.  Else NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * GET_RESULT_ELEMENT_TYPE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT EFI_GUID                 **ResultType
  );

/**
  Function returns the size a result element.

  @param[in, out] This       - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     ResultSize - Returns the address to the UINT32 in the results data buffer that holds the size.

  @retval SUCCESS
  @retval UnsupportedValue if ResultSize is NULL.  Else NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * GET_RESULT_ELEMENT_SIZE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT UINT32                   **ResultSize
  );

/**
  Function returns how many elements can be stored in the results data buffer.

  @param[in, out] This           - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     ResultCapacity - Returns the address to the UINT32 in the results data buffer that holds the capacity.

  @retval SUCCESS
  @retval UnsupportedValue if ResultCapacity is NULL.  Else NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * GET_RESULT_ELEMENT_CAPACITY) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT UINT32                   **ResultCapacity
  );

/**
  Function returns how many elements are currently stored in the results data buffer.

  @param[in, out] This        - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     ResultCount - Returns the address to the UINT32 in the results data buffer that holds the current element count.

  @retval SUCCESS
  @retval UnsupportedValue if ResultCount is NULL.  Else NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * GET_RESULT_ELEMENT_COUNT) (
  IN  OUT SSA_BIOS_SERVICES_PPI *This,
  OUT UINT32                    **ResultCount
  );

/**
  Function returns the next available result element in the results data buffer.
  While in automatic transfer mode every time a new element is requested the current element count is checked.
  If the buffer is full the entire block is transferred back to the host and the buffer count is reset.
  Note: results remaining in the buffer will be finalized (transferred, etc) either by the loader when the test completes
  or by CreateResultsBuffer() before it frees any preexisting buffer.

  @param[in, out] This          - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     ResultElement - Returns the address to next available results element in the results data buffer.

  @retval SUCCESS
  @retval UnsupportedValue if ResultElement is NULL.  Else NotAvailable if a results buffer has not been created or there are no results elements available.
**/
typedef
SSA_STATUS
(EFIAPI * GET_NEXT_RESULT_ELEMENT) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT VOID                     **ResultElement
  );

/**
  The function toggles the explicit mode either on or off.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[in]      Mode - Toggle explicit mode on (TRUE) or off (FALSE).

  @retval SUCCESS
  @retval NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * SET_EXPLICIT_TRANSFER_MODE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  IN BOOLEAN                   Mode
  );

/**
  The function returns the current explicit mode value.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.
  @param[out]     Mode - Returns the current explicit mode, on (TRUE) or off (FALSE).

  @retval SUCCESS
  @retval UnsupportedValue if Mode is NULL.  Else NotAvailable if a results buffer has not been created.
**/
typedef
SSA_STATUS
(EFIAPI * GET_EXPLICIT_TRANSFER_MODE) (
  IN OUT SSA_BIOS_SERVICES_PPI *This,
  OUT BOOLEAN                  *Mode
  );

/**
  The function flushes the used portion of the results buffer and resets the results data index.
  (Used in explicit transfer mode only.) Note: results remaining in the buffer will be finalized
  (transferred, etc) either by the loader when the test completes or by CreateResultsBuffer()
  before it frees any pre-existing buffer.

  @param[in, out] This - Interface pointer that implements the particular SSA_BIOS_SERVICES_PPI instance.

  @retval SUCCESS
  @retval NotAvailable if a results buffer has not been created or the explicit transfer mode is FALSE.
**/
typedef
SSA_STATUS
(EFIAPI * PUSH_RESULTS) (
  IN OUT SSA_BIOS_SERVICES_PPI *This
  );

///
/// BIOS-SSA Results Configuration
///
typedef struct _SSA_RESULTS_CONFIG {
  UINT32                      Revision;                 ///< Incremented when a backwards compatible binary change is made to the PPI.
  VOID                        *ResultsData;             ///< Pointer to the Results Data buffer.
  CREATE_RESULTS_BUFFER       CreateResultBuffer;       ///< Creates a new result buffer comprised of a header, metadata and an array of result elements.
  SET_REVISION                SetRevision;              ///< Function sets the type of the metadata.
  GET_REVISION                GetRevision;              ///< Function gets the type of the metadata.
  SET_METADATA_TYPE           SetMetadataType;          ///< Sets the type of the metadata.
  GET_METADATA_TYPE           GetMetadataType;          ///< Gets the type of the metadata.
  GET_METADATA_SIZE           GetMetadataSize;          ///< Gets the size of the metadata block.
  GET_METADATA                GetMetadata;              ///< Gets a pointer to the metadata block.
  SET_RESULT_ELEMENT_TYPE     SetResultElementType;     ///< Sets the element type of the result buffer.
  GET_RESULT_ELEMENT_TYPE     GetResultElementType;     ///< Gets the element type of the result buffer.
  GET_RESULT_ELEMENT_SIZE     GetResultElementSize;     ///< Gets the size of a result element.
  GET_RESULT_ELEMENT_CAPACITY GetResultElementCapacity; ///< Gets the total number of result element slots.
  GET_RESULT_ELEMENT_COUNT    GetResultElementCount;    ///< Gets the number of populated result elements.
  GET_NEXT_RESULT_ELEMENT     GetNextResultElement;     ///< Gets a pointer to the next result element in an iteration over the result block.
  SET_EXPLICIT_TRANSFER_MODE  SetExplicitTransferMode;  ///< Set the explicit transfer mode to true or false.
  GET_EXPLICIT_TRANSFER_MODE  GetExplicitTransferMode;  ///< Get the explicit transfer mode.
  PUSH_RESULTS                PushResults;              ///< Flush data, fill in any scratch pad information and reset element index.
} SSA_RESULTS_CONFIG, *PSSA_RESULTS_CONFIG;

#pragma pack (pop)
#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

#endif // _SsaResultsConfig_h_
