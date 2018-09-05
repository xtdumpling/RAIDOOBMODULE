/** @file
  Header file for PEI SiPolicyUpdate Library.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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

#ifndef _PEI_SI_POLICY_UPDATE_LIB_H_
#define _PEI_SI_POLICY_UPDATE_LIB_H_

/**
  This function performs Silicon PEI Policy initialzation.

  @param[in] SiPolicy     The Silicon Policy PPI instance

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
EFIAPI
UpdatePeiSiPolicy (
  IN OUT SI_POLICY_PPI *SiPolicy
  );
#endif
