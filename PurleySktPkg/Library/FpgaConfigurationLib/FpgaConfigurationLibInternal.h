//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

 FpgaConfigurationLibInternal.h

Abstract:

  This file provides intermal function headers for FpgaConfigurationLib

--*/

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/FpgaSocketVariable.h>
#include <Library/FpgaConfigurationLib.h>

#ifndef PURLEYSKTPKG_LIBRARY_FPGACONFIGURATIONLIB_FPGACONFIGURATIONLIBINTERNAL_H_
#define PURLEYSKTPKG_LIBRARY_FPGACONFIGURATIONLIB_FPGACONFIGURATIONLIBINTERNAL_H_

extern EFI_GUID gFpgaSocketHobGuid;

#pragma pack(1)

typedef struct {

  //
  // These are all flags on global state of the FPGA system, used to return teh global flag to teh rest of the system
  // Basically the global flag is a or'd version of all of these.
  // Also is in part a result of the Socket bit position flags, i.e. if no FPGA are present in any socket, global value will be FALSE.
  //

  BOOLEAN      FpgaPcdValue;
  BOOLEAN      FpgaFvFound;


  //
  // These are all socket bit position flags, with Bit 0 being Socket 0
  //

  // These are runtime status entries
  UINT8          FpgaSktPresent;         // FPGA Is present in the socket
  UINT8          FpgaSktActive;          // FPGA Is active in the socket
  UINT8          FpgaSktError;           // FPGA has error in the socket, FpgaBitStreamError is valid
  UINT8          FpgaSktProgrammed;      // FPGA has been successful programmed with BitStream

  // These are Platform inputs
  UINT8          FpgaPlatformEnabled; // FPGA in socket is requested enabled by platform

  //
  // Array of values with FPGA Socket as index
  //

  //
  // FPGA BitStram GUID from Platform setup, 0's for invalid GUID's
  // Must have valid entry for every bit enabled in FpgaPlatformEnabled.
  //
  UINT8          FpgaBitStreamGuid[FPGA_MAX_SOCKET];

  //
  // FPGA HSSI Card ID
  //
  UINT8          FpgaHssiCardID[FPGA_MAX_SOCKET];

  //
  // Status of BitStream load for error reporting if needed.
  //
  UINT8          FpgaBitStreamStatus[FPGA_MAX_SOCKET]; // FPGA BitStream load Status value
  //
  // Loader error return, only valid if the FpgaBitStreamStatus == FPGA_LOAD_STATUS_DEVICE_RETURNED_ERROR
  //
  UINT8          FpgaSocketErrorReturn[FPGA_MAX_SOCKET];

  // FPGA Temperature Threshold 1/2: Max value clamped at 100 C;
  // i.e. if the SW tries to write value greater than 100 C, HW will automatically default to 100 C.
  UINT8          FpgaSktThermalTH1[FPGA_MAX_SOCKET];
  UINT8          FpgaSktThermalTH2[FPGA_MAX_SOCKET];

  //
  // Enable/Disable FPGA HSSI EQ Tuning
  //
  UINT8          HssiEqTuningEn;
} FPGA_CONFIGURATION_HOB;

#pragma pack()

//
// Internal get HOB function
//
VOID * InternalCreateHob(VOID);

FPGA_SOCKET_CONFIGURATION * EFIAPI FpgaGetVariable(VOID);


#endif /* PURLEYSKTPKG_LIBRARY_FPGACONFIGURATIONLIB_FPGACONFIGURATIONLIBINTERNAL_H_ */
