/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    OemIioInitCommon.h

Abstract:

    Oem Hooks file for IioInit Module

--*/


#ifndef _OEMIIOINITCOMMON_H_
#define _OEMIIOINITCOMMON_H_

#include <Platform.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PlatformHooksLib.h>
#include <Library/SetupLib.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SetupVariable.h>
#include <Guid/SocketVariable.h>

#ifndef IA32
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/PcieCommonInitLib.h>
#include <Library/SetupLib.h>
#include <Library/PlatformHooksLib.h>
#include <Guid/PlatformInfo.h>
#include <Protocol/SystemBoard.h>
#include <Protocol/PciCallback.h>
#include <Protocol/UbaDevsUpdateProtocol.h>


#endif // IA32

#endif //_OEMIIOINITCOMMON_H_

