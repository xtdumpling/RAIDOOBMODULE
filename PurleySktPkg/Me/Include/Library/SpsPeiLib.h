/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file SpsPeiLib.h

  Definitions for Sps Pei Library

**/
#ifndef _SPS_PEI_LIB_H
#define _SPS_PEI_LIB_H

// Available clocking modes
typedef enum _CLOCKING_MODES {
  InternalStandard        = 0,
  InternalAlternate,
  HybridStandard,
  HybridAlternate,
  External
} CLOCKING_MODES;

/**
  Get current clocking mode via HECI.
  In case non SPS ME on board read it from soft straps

  @param[out] ClockingMode        Clocking mode read

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_NOT_READY           Function called before DID
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_UNSUPPORTED         Unsupported Clocking Mode has been returned
**/
EFI_STATUS
PeiGetCurrenClockingMode (
   OUT CLOCKING_MODES *ClockingMode
  )
;

/**
  Function sets SSC on/off

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
;

#endif // _SPS_PEI_LIB_H
