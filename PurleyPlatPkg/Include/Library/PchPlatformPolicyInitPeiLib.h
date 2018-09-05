/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c) 2011-2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file PchPlatformPolicyUpdatePeiLib.h

  Header file for PEI PchPlatformPolicyUpdate Library.

**/
#ifndef _PCH_PLATFORM_POLICY_UPDATE_PEI_LIB_H_
#define _PCH_PLATFORM_POLICY_UPDATE_PEI_LIB_H_

#include <Ppi/PchPlatformPolicy.h>
#include <Ppi/SystemBoard.h>
#include <Guid/SetupVariable.h>
#include <Guid/PchRcVariable.h>
#include <Guid/MeRcVariable.h>
#define PCH_HPET_BASE_ADDRESS             0xFED00000
#define PCH_TEMP_MEM_BASE_ADDRESS         0xFED08000

EFI_STATUS
EFIAPI
InitPchPolicyPei (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN SYSTEM_CONFIGURATION      *SystemConfiguration,
  //
  // PCH_VARIABLE
  // Add the PCH Variable till we move to PCH pkg
  //
  IN PCH_RC_CONFIGURATION      *PchConfiguration,
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  IN ME_RC_CONFIGURATION       *MeRcConfiguration,
#endif //  ME_SUPPORT_FLAG
  IN SYSTEM_BOARD_PPI          *SystemBoard
  );
/**

  @brief
  This PEIM performs PCH PEI Platform Policy initialzation.

  @param[in] FfsHeader            Pointer to Firmware File System file header.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             The PPI is installed and initialized.
  @retval EFI ERRORS              The PPI is not successfully installed.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

**/


/**

  This function performs PCH PEI Platform Policy initialzation.

  @param PeiServices             General purpose services available to every PEIM.
  @param PchPlatformPolicyPpi    The PCH Platform Policy PPI instance

  @retval EFI_SUCCESS             The PPI is installed and initialized.
  @retval EFI ERRORS              The PPI is not successfully installed.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

**/
EFI_STATUS
EFIAPI
UpdatePeiPchPlatformPolicy (
  IN      EFI_PEI_SERVICES        **PeiServices,
  IN OUT  PCH_PLATFORM_POLICY_PPI *PchPlatformPolicyPpi,
  IN SYSTEM_CONFIGURATION         *SystemConfiguration,
  //
  // PCH_VARIABLE
  // Add the PCH Variable till we move to PCH pkg
  //
  IN PCH_RC_CONFIGURATION        *PchConfiguration,
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  IN ME_RC_CONFIGURATION         *MeRcConfiguration,
#endif // ME_SUPPORT_FLAG
  IN SYSTEM_BOARD_PPI             *SystemBoard
  )
;
#endif
