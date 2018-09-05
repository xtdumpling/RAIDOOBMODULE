/** @file
  PEI Library Functions. Initialize GPIOs

@copyright
 Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/


#include <Uefi.h>
#include <Guid/PlatformInfo.h>
#include <Library/DebugLib.h>
#include <Library/UbaGpioInitLib.h>
#include <Library/PeiPlatformHooklib.h>

/**
  Configure GPIO

  @param[in]  PlatformInfo
**/
VOID
GpioInit (
  IN EFI_PLATFORM_INFO             *PlatformInfo
)
{
  EFI_STATUS                  Status;
  Status = PlatformInitGpios();
}


/**
  Configure GPIO and SIO

  @retval  EFI_SUCCESS   Operation success.
**/
EFI_STATUS
BoardInit (
  IN EFI_PLATFORM_INFO            *PlatformInfo
  )
{

  ASSERT (PlatformInfo);
  
  DEBUG((DEBUG_INFO, "BoardInit Board ID: %x\n",PlatformInfo->BoardId));
  
  GpioInit(PlatformInfo);

  return EFI_SUCCESS;
}
