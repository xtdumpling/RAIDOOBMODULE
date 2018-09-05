/** @file

@copyright
 Copyright (c) 2011 - 2014 Intel Corporation. All rights reserved
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

#ifndef _PEI_PCH_POLICY_UPDATE_LIB_H_
#define _PEI_PCH_POLICY_UPDATE_LIB_H_
#include <Guid/SetupVariable.h>
#include <Guid/PchRcVariable.h>

EFI_STATUS
EFIAPI
UpdatePeiPchPolicy (
    IN OUT PCH_POLICY_PPI        *PchPolicy,
    IN SYSTEM_CONFIGURATION      *SetupVariables,
    IN PCH_RC_CONFIGURATION      *PchRcVariables
  );
/*++

Routine Description:

  This function performs PCH PEI Policy initialzation.

Arguments:

  PchPolicy               The PCH Policy PPI instance

Returns:

  EFI_SUCCESS             The PPI is installed and initialized.
  EFI ERRORS              The PPI is not successfully installed.
  EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

--*/
;
#endif
