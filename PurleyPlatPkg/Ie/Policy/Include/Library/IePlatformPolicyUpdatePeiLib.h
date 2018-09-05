/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file IePlatformPolicyUpdatePeiLib.h

  Header file for PEI IePlatformPolicyUpdate Library.

**/
#ifndef _IE_PLATFORM_POLICY_UPDATE_PEI_LIB_H_
#define _IE_PLATFORM_POLICY_UPDATE_PEI_LIB_H_

#include "Ppi/IePlatformPolicyPei.h"
/**

  Update the Ie Platform Policy Library

  @param PeiIePlatformPolicyPpi  Ie Platform Policy Ppi

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.

**/
EFI_STATUS
UpdatePeiIePlatformPolicy (
  IN OUT  PEI_IE_PLATFORM_POLICY_PPI  *PeiIePlatformPolicyPpi
  )
;

#endif
