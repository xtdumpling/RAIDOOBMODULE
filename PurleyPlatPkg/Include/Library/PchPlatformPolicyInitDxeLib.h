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

  @file PchPlatformPolicyUpdateDxeLib.h

  Header file for DXE PchPlatformPolicyUpdate Library.

**/
#ifndef _PCH_PLATFORM_POLICY_UPDATE_DXE_LIB_H_
#define _PCH_PLATFORM_POLICY_UPDATE_DXE_LIB_H_

#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/PlatformType.h>
#include <Protocol/SystemBoard.h>
#include <Guid/SetupVariable.h>
#include <Guid/SocketVariable.h>

/**

  @brief
  Initilize Intel PCH DXE Platform Policy

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
InitDxePchPlatformPolicy (
  IN SYSTEM_CONFIGURATION                   *SetupVariables,
  IN SOCKET_IIO_CONFIGURATION     *SocketIioVariables,
//
// PCH_VARIABLE
// Add the PCH Variable till we move to PCH pkg
//
  IN PCH_RC_CONFIGURATION         *PchVaraibles,
  IN SYSTEM_BOARD_PROTOCOL  *mSystemBoard
  )
;

/**

  This function updates Dxe Pch Platform Policy

  @param SetupVariables                 Setup configuration values
  @param mPlatformType                  Platformtype Info

  @retval EFI_SUCCESS                     Initialization complete.
  @retval EFI_UNSUPPORTED                 The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES            Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR                Device error, driver exits abnormally.

**/
EFI_STATUS
EFIAPI
UpdateDxePchPlatformPolicy (
      IN SYSTEM_CONFIGURATION                       *SetupVariables,
  //
  // PCH_VARIABLE
  // Add the PCH Variable till we move to PCH pkg
  //
      IN PCH_RC_CONFIGURATION                       *PchVariables,
      IN SOCKET_IIO_CONFIGURATION                   *SocketIioVariables,
      IN SYSTEM_BOARD_PROTOCOL                      *mSystemBoard,
      IN OUT DXE_PCH_PLATFORM_POLICY_PROTOCOL       *DxePlatformPchPolicy
      )
;
#endif
