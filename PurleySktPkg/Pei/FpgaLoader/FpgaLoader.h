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

 FpgaLoader.h

Abstract:

  This file provides intermal function headers for FpgaLoader

--*/

#ifndef _FPGA_LOADER_H_
#define _FPGA_LOADER_H_

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Ppi/MpServices.h>
#include <Ppi/PchReset.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/FpgaPlatformHooklib.h>
#include <Library/FpgaConfigurationLib.h>
#include <Library/FpgaFvDataLib.h>
#include <Fpga.h>
#include <Cpu/CpuRegs.h>
#include <RcRegs.h>
#include <SysHost.h>


#endif
