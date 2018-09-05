//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file FgpaPlatformEarlyInit.h

  Platform init file for Fpga

**/

#ifndef _EFI_FPGA_PLATFORM_EARLY_INIT_H_
#define _EFI_FPGA_PLATFORM_EARLY_INIT_H_

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Ppi/BaseMemoryTest.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Ppi/Spi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/PrintLib.h>
#include <Library/ReadFfsLib.h>
#include <Library/SetupLib.h>
#include <IndustryStandard/Pci22.h>
#include <Guid/FirmwareFileSystem3.h>
#include <Guid/SocketVariable.h>
#include <Guid/FpgaSocketVariable.h>
#include <Library/FpgaConfigurationLib.h>
#include <Library/GpioLib.h>
#include <GpioPinsSklH.h>
#include <Library/FpgaPlatformHooklib.h>
#include <SysHost.h>
#include <Ppi/MpServices.h>
#include <RcRegs.h>

extern EFI_GUID gFpgaInitPpiGuid;

/**
   Initialize FPGA BBS binaries file

  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS   FPGA BBS binaries file initialize successfully.

**/
EFI_STATUS
EFIAPI
FpgaBbsInit (
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );


#endif
