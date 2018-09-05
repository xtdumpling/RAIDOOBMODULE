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


  @file FpgaSktSetup.h

  Header file for Platform Initialization Driver.

Revision History:

**/

#ifndef _FPGA_SOCKET_SETUP_H_
#define _FPGA_SOCKET_SETUP_H_

#include <Uefi.h>
#include "FpgaSocketSetupStrDefs.h"
#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/FpgaFvDataLib.h>
#include <Library/FpgaConfigurationLib.h>

#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
// APTIOV_SERVER_OVERRIDE_RC_START
#include <Protocol/HiiPackageList.h>
// APTIOV_SERVER_OVERRIDE_RC_END
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/IioSystem.h>

#include <Guid/HobList.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/FpgaSocketVariable.h>
#include <FpgaConfiguration.h>
#include <RcRegs.h>
#include <PciAccess.h>

#define NEWSTRING_SIZE         0x200
typedef UINT16                 STRING_REF;

#endif
