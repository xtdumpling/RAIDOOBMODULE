//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file FpgaSocketVariable.h

  Data format for Universal Data Structure

**/


#ifndef   __FPGA_SOCKET_CONFIGURATION_DATA_H__
#define   __FPGA_SOCKET_CONFIGURATION_DATA_H__

#include <Fpga.h>

extern EFI_GUID gFpgaSocketVariableGuid;
#define FPGA_SOCKET_CONFIGURATION_NAME L"FpgaSocketConfig"

#pragma pack(1)

typedef struct {
  //
  // User Bitmap to enable the FPGA socket.
  //
  UINT8    FpgaSetupEnabled;

  //
  // For each socket enabled, use this Bit stream GUID Index
  // Note: variable is Index+ 1 for unused default to be 0
  //
  UINT8    FpgaSocketGuid[FPGA_MAX_SOCKET];

  //
  // FPGA Temperature Threshold 1/2: Max value clamped at 100 C;
  // i.e. if the SW tries to write value greater than 100 C, HW will automatically default to 100 C.
  //
  UINT8    FpgaThermalTH1[FPGA_MAX_SOCKET];
  UINT8    FpgaThermalTH2[FPGA_MAX_SOCKET];

  //
  // Enable/Disable FPGA HSSI EQ Tuning
  //
  UINT8    HssiEqTuningEn;

  //
  // FPGA reserved data
  //
  UINT8    FpgaReserved[13];
} FPGA_SOCKET_CONFIGURATION;

#pragma pack()

#endif
