//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FpgaFvDataLib.h

Abstract:

  This file provides required function headers and Data formats of FPGA FV Library.
  Use of this library will allow access to the Fpga Fv data.

--*/

#ifndef _FPGA_FV_DATA_LIB_H_
#define _FPGA_FV_DATA_LIB_H_

#include <Uefi.h>
#include <Fpga.h>
#include <FpgaConfiguration.h>

extern EFI_GUID gFpgaSocketBbsGbeGuid;
extern EFI_GUID gFpgaSocketBbsPcieGuid;
extern EFI_GUID gFpgaSocketN4PeGuid;

#define FPGA_PRAM_DIRECTORY_VERSION 0x0001

typedef struct {
  EFI_PHYSICAL_ADDRESS FpgaParmLoaderAddress;    //FPGA_LOADER_MODULE  0  8 FPGA Loader module address (256K-aligned)
  UINT16               FpgaParmStructureVersion; //VERSION             8  2 Structure version
  UINT16               FpgaParmPayloadVersion;   //PAYLOAD_VERSION    10  2 Payload version
  UINT32               FpgaParmPayloadSize;      //PAYLOAD_LENGTH     12  4 Payload length
  EFI_PHYSICAL_ADDRESS FpgaParmPayloadAddress;   //PAYLOAD            16  8 Physical address of payload
} FPGA_PARM_DIRECTORY;

// The N4PE Binary Blob header
typedef struct {
  UINT16             MinorHeaderVersion ;   // 0  2 Minor version of this header. .
  UINT16             MajorHeaderVersion;    // 2  2 Major version of this header.
  UINT16             MinorRevisionID ;      // 4  2 Minor version of the module (FPGA N4PE Module). .
  UINT16             MajorRevisionID;       // 6  2 Bits 14:0 - Major version of the Module.    Bit 15: 0 - Production signed module, 1 - debug signed module                                             
  UINT32             Date;                  // 8  4 Date of the module creation in the binary format mmddyyyy 
  UINT32             LoaderRevision;        // 12 4 Bits 31:16: version number of the loader program needed to load this module.
  UINT32             LoaderSize;            // 16 4 Size of the entire module Module (including this header)
  UINT32             NumOfTargetFamilies;   // 20 4 Number of valid values in the subsequent Processor Target array (1..8). This field may be 0.
  UINT32             TargetProcessors[8];   // 24 4*8 Processor Target array. 
  UINT16             ProductId;             // 56 2 The ProductId defines the domain of the BuildNumber.   0: SKX
  UINT16             BuildNumber;           // 58 2 This is the build number for the given ProductId domain.  
  UINT8              Reserved[68];          // 50 68 RFU
} FPGA_LOADER_HEADER;


// And the FPGA Binary Blob header
typedef struct {
  GUID               FpgaBinaryGuid;    //GUID    16 GUID uniquely identifying the data structure.
  UINT32             FpgaBinarySize;    //SIZE    4  Data structure size (including this header)
  UINT16             FpgaBinaryVersion; //VERSION 2  Data structure version.
  UINT16             FpgaBinaryFlags;   //FLAGS   2  0
  VOID               *FpgaBinaryData;   //DATA SIZE
} FPGA_PAYLOAD_HEADER;


//
// Functions implemented in the Library
//

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
  );

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
  );

#endif
