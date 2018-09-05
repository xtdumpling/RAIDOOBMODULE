//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  CpuRegs.h

Abstract:

--*/

#ifndef _CPU_CSR_H
#define _CPU_CSR_H

#include "CommonIncludes.h"


//
// Device 0,  Function 0
//
#define CPU_D0F0_PCIE_ADDR(Bus, Offset)      PCI_EXPRESS_LIB_ADDRESS(Bus, 0, 0, Offset)

#define CSR_DESIRED_CORES                    0x0080
#define   B_CSR_DESIRED_CORES_CORE_CNT        (BIT0 | BIT1)
#define   B_CSR_DESIRED_CORES_SMT_DIS          BIT8
#define   B_CSR_DESIRED_CORES_LOCK             BIT16


//
// Device 0,  Function 1
//
#define CPU_D0F1_PCIE_ADDR(Bus, Offset)      PCI_EXPRESS_LIB_ADDRESS(Bus, 0, 1, Offset)

#define GQ1_CR_PCIEXBAR                      0x0050

#endif
