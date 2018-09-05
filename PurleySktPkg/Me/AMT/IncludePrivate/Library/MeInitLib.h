/** @file
  The PEI Library Implements ME Init.

@copyright
 Copyright (c) 2008 - 2016 Intel Corporation. All rights reserved
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
#ifndef _ME_INIT_LIB_H_
#define _ME_INIT_LIB_H_

#include <Ppi/MePolicyPpi.h>

/**
  Check some ME policies are valid for debugging unexpected problem if these
  values are not initialized or assigned incorrect resource.

  @param[in] MePolicyPpi    The Me Policy protocol instance

**/
VOID
MeValidatePolicy (
  IN  ME_POLICY_PPI  *MePolicyPpi
  );

/**
  This function performs basic initialization for ME in PEI phase after Policy produced.
**/
VOID
EFIAPI
MeOnPolicyInstalled (
  VOID
  );

/**
  Configure HECI devices on End Of Pei
**/
VOID
MeOnEndOfPei (
  IN VOID
  );

/**
  Internal function performing Heci PPIs init needed in PEI phase

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        ME FPT is bad
**/
EFI_STATUS
InstallHeciPpi (
  VOID
  );

/**
  This function performs basic initialization for ME in PEI phase after memory is initiliazed.
**/
VOID
EFIAPI
MePostMemInit (
  VOID
  );

//
// Function Prototype(s) for PchMeUma.c
//

/**
  This procedure will read and return the amount of ME UMA requested
  by ME ROM from the HECI device.

  @param[in] FfsHeader            Pointer to the FFS file header

  @retval UINT32                  Return ME UMA Size
  @retval EFI_SUCCESS             Do not check for ME UMA
**/
UINT32
MeSendUmaSize (
  IN VOID                         *FfsHeader
  );

/**
  This procedure will determine whether or not the CPU was replaced
  during system power loss or via dynamic fusing.
  Calling this procedure could result in a warm reset (if ME FW is requesting one).

  @param[in] FfsHeader            Not used.
  @param[out] ForceFullTraining   When set = 0x1, MRC will be forced to perform a full
                                  memory training cycle.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
CpuReplacementCheck (
  IN VOID                         *FfsHeader,
  OUT UINT8                       *ForceFullTraining
  );

/**
  This procedure will configure the ME Host General Status register,
  indicating that DRAM Initialization is complete and ME FW may
  begin using the allocated ME UMA space.

  @param[in] FfsHeader            Pointer to the FFS file header
  @param[in] InitStat             H_GS[27:24] Status
  @param[in] PttStolenBase        The base of PTT
  @param[in] MeUmaSize            The UMA size FW requested
  @retval EFI_SUCCESS
**/
EFI_STATUS
MeConfigDidReg (
  IN VOID                         *FfsHeader,
  IN UINT8                        InitStat,
  IN UINT32                       PttStolenBase,
  IN UINT32                       MeUmaSize
  );

#endif
