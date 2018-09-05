//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
    UBA INIT DXE Driver.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_INIT_DXE_H_
#define _UBA_INIT_DXE_H_

#include <Base.h>
#include <Uefi.h>
#include <PiDxe.h>  // For Hob

#include <Protocol/UbaCfgDb.h>
#include <Protocol/UbaMakerProtocol.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Platform.h>
#include <BoardTypes.h>

#endif // _UBA_INIT_DXE_H_
