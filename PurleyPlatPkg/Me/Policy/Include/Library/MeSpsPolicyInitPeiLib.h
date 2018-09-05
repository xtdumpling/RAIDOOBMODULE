/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  MeSpsPolicyInitPeiLib.h

@brief:

  Header file for the MeSpsPolicyInitPei Driver.

**/
#ifndef _PEI_ME_SPS_POLICY_INIT_LIB_H_
#define _PEI_ME_SPS_POLICY_INIT_LIB_H_

#include <Ppi/MeSpsPolicyPei.h>
#include <Ppi/MePolicyPpi.h>

/**

  @brief
  Initialize Intel ME PEI SPS Policy

  @param[in] PeiServices          Pei Services pointer

  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures.

**/
EFI_STATUS
SpsPolicyInitPei (
  IN CONST EFI_PEI_SERVICES   **PeiServices
  )
;


#endif // _PEI_ME_SPS_POLICY_INIT_LIB_H_
