//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
 FPGA Fv Data library

 Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 **/

#include "FpgaFvDataLibInternal.h"
#include <Library/FpgaFvDataLib.h>


// Internal Functions

/**
  Get a Fv BitStream from the FV

  @param[in] FpgaFvLibBitStreamIndex     Index of FPGA GUID of Bit Stream to retrieve.
  @param[out] FpgaFvLibBitStreamSize     Pointer to save the Bit Stream Size to
  @param[out] FpgaFvLibBitStreamPtr      Pointer to save the BitStream to, Caller responsible for FreePool of pointer

  @retval EFI_SUCCESS                    The function completed successfully
  @retval EFI_NOT_FOUND                  The function could not locate the FPGA data
  @retval EFI_INVALID_PARAMETER          The function found FPGA Fv, but it was not valid.

**/
EFI_STATUS
EFIAPI
FpgaFvLibBitStreamGet (
  IN UINTN                  FpgaFvLibBitStreamIndex,
  OUT UINTN                 *FpgaFvLibBitStreamSize,
  OUT FPGA_PAYLOAD_HEADER   **FpgaFvLibBitStreamPtr
  )
{
  EFI_STATUS           Status;

  Status = EFI_SUCCESS;
  switch (FpgaFvLibBitStreamIndex) {
    case FPGA_NULL_GUID_INDEX:
      Status = EFI_UNSUPPORTED;
      break;
    case FPGA_GBE_GUID_INDEX:
      Status = FpgaGetRawImage (&gFpgaSocketBbsGbeGuid, FpgaFvLibBitStreamPtr, FpgaFvLibBitStreamSize);
      break;
    case FPGA_PCIE_GUID_INDEX:
      Status = FpgaGetRawImage (&gFpgaSocketBbsPcieGuid, FpgaFvLibBitStreamPtr, FpgaFvLibBitStreamSize);
      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  return Status;
}

/**
  Get a Fv N4PE Loader binary blob to pass to FPGA

  @param[out] FpgaFvLibLoaderSize          Pointer to save the Loader Size to
  @param[out] FpgaFvLibLoaderPtr           Pointer to save the Loader to, Caller responsible for FreePool of pointer

  @retval EFI_SUCCESS                      The function completed successfully
  @retval EFI_NOT_FOUND                    The function could not locate the FPGA data
  @retval EFI_INVALID_PARAMETER            The function found FPGA Fv, but it was not valid.

**/
EFI_STATUS
EFIAPI
FpgaFvLibN4PeGet (
  OUT UINTN                 *FpgaFvLibLoaderSize,
  OUT FPGA_LOADER_HEADER    **FpgaFvLibLoaderPtr
  )
{
  EFI_STATUS           Status;

  Status = FpgaGetRawImage (&gFpgaSocketN4PeGuid, FpgaFvLibLoaderPtr, FpgaFvLibLoaderSize);
  return Status;
}
