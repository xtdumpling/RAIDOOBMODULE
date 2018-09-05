/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2014, Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MeDummy.c

  This file implements dummy ME library used in case of no ME support.

**/
// APTIOV_SERVER_OVERRIDE_RC_START : Resolve build error with WorkStation Support
#if AMT_SUPPORT         //Fix SSC enable failed with SPS support 
#include <Uefi/UefiBaseType.h>
#include <Library/SpsPeiLib.h>
#include <Library/MeUmaEinjLib.h>

/**
  It's a dummy function.

  @param[out] ClockingMode        Clocking mode read via HECI from ME

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_NOT_READY           Function called before DID
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_UNSUPPORTED         Unsupported Clocking Mode has been returned
**/
EFI_STATUS
PeiHeciGetCurrenClockingMode (
   OUT CLOCKING_MODES *ClockingMode
  )
{
  return EFI_UNSUPPORTED;
}

/**
  It's a dummy function.

  @param[in] UseAlternate         Set Alternate SSC
                                  TRUE:  turn off SSC for BCLK PLL
                                  FALSE: turn on SSC 0.5% downspread for BCLK PLL

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_NOT_READY           Function called before DID
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_UNSUPPORTED         Unsupported Clocking Mode has been returned
**/
EFI_STATUS
PeiHeciSetSSCAlternate (
   IN BOOLEAN UseAlternate
  )
{
  return EFI_UNSUPPORTED;
}
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Resolve build error with WorkStation Support