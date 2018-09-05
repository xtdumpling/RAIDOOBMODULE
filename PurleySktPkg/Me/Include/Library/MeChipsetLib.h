/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2010 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MeChipsetLib.h

  Header file for Me Chipset Lib

**/
#ifndef _ME_CHIPSET_LIB_H_
#define _ME_CHIPSET_LIB_H_

typedef enum {
  HECI1 = 0,
  HECI2,
  IDER,
  SOL,
  HECI3,
  FDSWL,
} ME_DEVICE;

typedef enum {
  Disabled= 0,
  Enabled,
} ME_DEVICE_FUNC_CTRL;

/**
  Put ME device into D0I3

  @param[in] Function          Select of Me device

**/
VOID
SetD0I3Bit (
  IN  UINT32   Function
  )
;

/**
  Put ME device out of D0I3

  @param[in] Function  ME function where DOI3 is to be changed

**/
VOID
ClearD0I3Bit (
   IN  UINT32   Function
  )
;

/**

 Enable/Disable Me devices

  @param WhichDevice    - Select of Me device
  @param DeviceFuncCtrl - Function control

  @retval None

**/
VOID
MeDeviceControl (
  IN  ME_DEVICE             WhichDevice,
  IN  ME_DEVICE_FUNC_CTRL   DeviceFuncCtrl
  )
;

#endif
