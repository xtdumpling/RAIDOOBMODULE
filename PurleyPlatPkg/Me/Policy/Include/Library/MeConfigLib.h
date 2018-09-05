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

@file:

  MeConfigLib.h

@brief:

  ME Config Library

**/
#ifndef _ME_CONFIG_LIB_H_
#define _ME_CONFIG_LIB_H_

#include "MeChipset.h"

typedef enum {
  ME_AUTO_CONFIG_HEC1         = HECI_FUNCTION_NUMBER,
  ME_AUTO_CONFIG_HEC2         = HECI2_FUNCTION_NUMBER,
  ME_AUTO_CONFIG_HEC3         = HECI3_FUNCTION_NUMBER,
  ME_AUTO_CONFIG_SOL          = SOL_FUNCTION_NUMBER,
  ME_AUTO_CONFIG_IDER         = IDER_FUNCTION_NUMBER
} ME_AUTO_CONFIG;


/**
  Configure Intel ME default settings.

  @param[io] pMeConfig  The pointer to get ME config structure

  @retval EFI_SUCCESS   The defaults initialized.
**/
EFI_STATUS EFIAPI
MeConfigDefaults(
  OUT  ME_CONFIG    *pMeConfig);


/**
  Print whole ME_CONFIG to serial out.

  @param[in] MeConfig                 The RC ME Config structure
**/
VOID EFIAPI
MeConfigPrint(
  IN     ME_CONFIG  *pMeConfig);

/**

  @brief
  Function resolves AutoSettings for particular configuration entry

  @param[in] Device               Device to resolve auto-configuration
  @param[in] ConfigToResolve      Auto-configuration to be resolved
  @param[in] MeIsCorporateAmt     True if it is corporate AMT running in ME

  @retval resolved auto-configuration

**/
UINT32
MeConfigDeviceAutoResolve(
  IN     ME_AUTO_CONFIG Device,
  IN     UINT32         ConfigToResolve,
  IN     BOOLEAN        MeIsCorporateAmt);

#endif // _ME_CONFIG_LIB_H_

