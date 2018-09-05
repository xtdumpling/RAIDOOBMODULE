/** @file
  Header file for the PolicyInitPei Library.

@copyright
 Copyright (c) 2013 - 2014 Intel Corporation. All rights reserved
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
#ifndef _POLICY_INIT_PEI_LIB_H_
#define _POLICY_INIT_PEI_LIB_H_

/**
  Initilize Intel PEI Platform Policy

  @param[in]  FirmwareConfiguration It uses to skip specific policy init that depends
                                    on the 'FirmwareConfiguration' varaible.

  @retval     EFI_OUT_OF_RESOURCES  Unable to allocate necessary data structures.
**/
VOID
EFIAPI
PeiPolicyInitPreMem (
  IN UINT8                     FirmwareConfiguration,
  IN SYSTEM_CONFIGURATION      *SystemConfiguration,
  IN PCH_RC_CONFIGURATION      *PchRcConfiguration
  );

/**
  Initilize Intel PEI Platform Policy

  @param[in]  PeiServices          General purpose services available to every PEIM.

  @retval     EFI_OUT_OF_RESOURCES Unable to allocate necessary data structures.
**/
VOID
EFIAPI
PeiPolicyInit (
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );
#endif
