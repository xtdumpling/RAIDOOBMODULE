/** @file
  Header file for Pch Init Lib Pei Phase

@copyright
  Copyright (c) 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PCH_INIT_LIB_H_
#define _PCH_INIT_LIB_H_

#include <Ppi/PchPolicy.h>

/**
  PCH Init Pre Policy Entry Point
  Performing Pch init in before Poilcy init
**/
VOID
EFIAPI
PchInitPrePolicy (
  VOID
  );

/**
  Check some PCH policies are valid for debugging unexpected problem if these 
  values are not initialized or assigned incorrect resource.

  @param[in] PchPolicy    The PCH Policy PPI instance

**/
VOID
PchValidatePolicy (
  IN  PCH_POLICY_PPI *PchPolicy
  );

/**
  This function performs basic initialization for PCH in PEI phase after Policy produced.
  If any of the base address arguments is zero, this function will disable the corresponding
  decoding, otherwise this function will enable the decoding.
  This function locks down the AcpiBase.

  @param[in] SiPolicyPpi   The Silicon Policy PPI instance
  @param[in] PchPolicyPpi  The PCH Policy PPI instance
**/
VOID
EFIAPI
PchOnPolicyInstalled (
  IN  PCH_POLICY_PPI  *PchPolicyPpi
  );

/**
  Pch init after memory PEI module
  
  @param[in] SiPolicy     The Silicon Policy PPI instance
  @param[in] PchPolicy    The PCH Policy PPI instance

  @retval None
**/
VOID
EFIAPI
PchInit (
  IN  PCH_POLICY_PPI *PchPolicy
  );

/**
  Pch End of PEI callback function. This is the last change before entering DXE and OS when S3 resume.
**/
VOID
PchOnEndOfPei (
  VOID
  );

/**
   Configure PSF registers for MCTP support.
**/
VOID
PchMctpConfigure (
  VOID
  );

#endif // _PEI_PCH_INIT_LIB_H_