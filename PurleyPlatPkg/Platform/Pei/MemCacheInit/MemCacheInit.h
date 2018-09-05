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


  @file MemCacheInit.h

  Header file for setting up Memory Type Range Registers for the platform

**/

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/SetupLib.h>
#include <Library/MtrrLib.h>
#include <Library/PlatformHooksLib.h>

#include <Guid/PlatformInfo.h>
#include <Guid/SetupVariable.h>
#include <Guid/MemoryMapData.h>

#include <Ppi/MpServices.h>

#include <Protocol/IioUds.h>

#include <Cpu/CpuRegs.h>
#include <Cpu/CpuDataStruct.h>

//   EDK2_TODO
#define EFI_MAX_ADDRESS   0xFFFFFFFF        /* Definition in EfiBind.h */

/**

  End of PEI Callback to set up the MTRRs for system boot.

  @param PeiServices       -  General purpose services available to every PEIM.
  @param NotifyDescriptor  -  Not used.
  @param Ppi               -  Not used.

  @retval EFI_SUCCESS   - MTRRs successfully programmed
          EFI_NOT_FOUND - Required HOB not found

**/
EFI_STATUS
MemCacheEndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );
