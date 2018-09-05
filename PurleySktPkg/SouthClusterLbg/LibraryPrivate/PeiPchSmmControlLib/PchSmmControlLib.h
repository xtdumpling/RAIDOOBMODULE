/** @file
  Header file for SMM Control PEI Library.

@copyright
  Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _PCH_SMM_CONTROL_LIB_H_
#define _PCH_SMM_CONTROL_LIB_H_

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Ppi/SmmControl.h>
#include <PchAccess.h>

//
// Prototypes
//
/**
  This routine generates an SMI

  @param[in] PeiServices                General purpose services available to every PEIM.
  @param[in] This                       The EFI SMM Control ppi instance
  @param[in, out] ArgumentBuffer        The buffer of argument
  @param[in, out] ArgumentBufferSize    The size of the argument buffer
  @param[in] Periodic                   Periodic or not
  @param[in] ActivationInterval         Interval of periodic SMI

  @retval EFI Status                    Describing the result of the operation
  @retval EFI_INVALID_PARAMETER         Some parameter value passed is not supported
**/
EFI_STATUS
EFIAPI
PeiActivate (
  IN     EFI_PEI_SERVICES    **PeiServices,
  IN     PEI_SMM_CONTROL_PPI *This,
  IN OUT INT8                *ArgumentBuffer OPTIONAL,
  IN OUT UINTN               *ArgumentBufferSize OPTIONAL,
  IN     BOOLEAN             Periodic OPTIONAL,
  IN     UINTN               ActivationInterval OPTIONAL
  );

/**
  This routine clears an SMI

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The EFI SMM Control ppi instance
  @param[in] Periodic             Periodic or not

  @retval EFI Status              Describing the result of the operation
  @retval EFI_INVALID_PARAMETER   Some parameter value passed is not supported
**/
EFI_STATUS
EFIAPI
PeiDeactivate (
  IN EFI_PEI_SERVICES    **PeiServices,
  IN PEI_SMM_CONTROL_PPI *This,
  IN BOOLEAN             Periodic OPTIONAL
  );

#endif
