//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2005 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file OemProcMemInit.h

  Memory Initialization Module.

**/

#ifndef _OEMPROCMEM_INIT_H_
#define _OEMPROCMEM_INIT_H_

#include "ProcMemInit.h"

#define size_t UINT32
void *memcpy(void * dst, void * src, size_t cnt);
void *memset(void *dst, char value, size_t cnt);

#define MMCFGBASE_GRANT       0x4000000
#define PCA9534_INPORT        0  // Input port
#define PCA9534_OUTPORT       1  // Output port
#define PCA9534_POLARITY      2  // Polarity inversion
#define PCA9534_CONFIG        3  // Configuration

#define PCA9535_INPORT0       0  // Input port 0
#define PCA9535_INPORT1       1  // Input port 1
#define PCA9535_OUTPORT0      2  // Output port 0
#define PCA9535_OUTPORT1      3  // Output port 1
#define PCA9535_POLARITY0     4  // Polarity inversion 0
#define PCA9535_POLARITY1     5  // Polarity inversion 1
#define PCA9535_CONFIG0       6  // Configuration 0
#define PCA9535_CONFIG1       7  // Configuration 1

//OC defines
#define DEFAULT_BCLK      0x00

typedef struct _OC_DATA_HOB{
	UINT8  DmiPeg;
}OC_DATA_HOB;

UINT8 PlatformVRsSVID (PSYSHOST  host, UINT8     socket, UINT8     mcId);

#endif // _OEMPROCMEM_INIT_H_
