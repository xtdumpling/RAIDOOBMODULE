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

  Fpga.h

Abstract:

  This file provides required FPGA Limits.

--*/

#ifndef _FPGA_DATA_H_
#define _FPGA_DATA_H_

#include <MaxSocket.h>

// Max Sockets for FPGA's, for all arrays, index loops,...
// Note: all bit mask arrays are defined as "UINT8", so if this increases
//       Those will have to be refactored to hold the new data.
#define FPGA_MAX_SOCKET                   MAX_SOCKET

// Number of times to retry loading on fail
#define FPGA_MAX_LOADER_RETRIES           3

// Entries in BS GUID index array
#define FPGA_GBE_GUID_INDEX               0x00
#define FPGA_PCIE_GUID_INDEX              0x01
#define FPGA_NULL_GUID_INDEX              0xff

// Default Invalid Thermal Threshold
#define FPGA_INVALID_TH                   0xff

// Resouce Allocation
#define FPGA_PREAllOCATE_BUS_NUM          0x02
#define FPGA_PREALLOCATE_MEM_SIZE         0x400000
// On FPGA, the VTD BAR is under FPGA device and require root bridge to decode. Root bridge decode is 1M Alignment.
#define FPGA_PREALLOCATE_VTD_SIZE         0x100000

//
// Major Checkpoint progress indicators written to debug port
//
#define STS_FPGA_LOADER_INIT              0x90
#define STS_FPGA_LOADER_PROGRAM_START     0x91
#define STS_FPGA_LOADER_PROGRAM_FAIL      0x92
#define STS_FPGA_DXE_ENTRY                0x93
#define STS_FPGA_DXE_CALLBACK             0x94
#define STS_FPGA_HIDE_PCIE_ROOT_PORT      0x95

#endif
