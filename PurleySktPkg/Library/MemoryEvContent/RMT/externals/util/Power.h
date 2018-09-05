/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
@copyright
  Copyright (c) 2013-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  Power.h

@brief
  This file contains definitions for the Power function.
**/
#ifndef __POWER_H__
#define __POWER_H__

#ifdef SSA_FLAG

#include "ssabios.h"

/**
@brief
  This function returns the value to power of exponent

  @param[in]   value        
  @param[in]   exponent
**/
UINT8 Power(
  UINT8 Value,
  UINT8 Exponent);

#endif  // SSA_FLAG
#endif  // __POWER_H__

// end file Power.h
