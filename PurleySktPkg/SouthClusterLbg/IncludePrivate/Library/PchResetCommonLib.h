/** @file
  Header file for PCH RESET Common Library.

@copyright
 Copyright (c) 2011 - 2014 Intel Corporation. All rights reserved
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
#ifndef _PCH_RESET_COMMON_LIB_H_
#define _PCH_RESET_COMMON_LIB_H_
#include <Uefi/UefiSpec.h>
#include <Protocol/PchReset.h>
///
/// Private data structure definitions for the driver
///
#define PCH_RESET_SIGNATURE SIGNATURE_32 ('I', 'E', 'R', 'S')

typedef struct {
  UINT32              Signature;
  EFI_HANDLE          Handle;
  union {
    PCH_RESET_PPI       PchResetPpi;
    PCH_RESET_PROTOCOL  PchResetProtocol;
  }PchResetInterface;
  UINT32              PchPwrmBase;
  UINT16              PchAcpiBase;
  UINTN               PchPmcBase;
} PCH_RESET_INSTANCE;

//
// Function prototypes used by the Pch Reset ppi/protocol.
//
/**
  Initialize an Pch Reset ppi/protocol instance.

  @param[in] PchResetInstance     Pointer to PchResetInstance to initialize

  @retval EFI_SUCCESS             The protocol instance was properly initialized
  @exception EFI_UNSUPPORTED      The PCH is not supported by this module
**/
EFI_STATUS
PchResetConstructor (
  PCH_RESET_INSTANCE          *PchResetInstance
  );

/**
  Execute Pch Reset from the host controller.
  @param[in] PchResetInstance     Pointer to PchResetInstance to initialize
  @param[in] PchResetType         Pch Reset Types which includes ColdReset, WarmReset, ShutdownReset,
                                  PowerCycleReset, GlobalReset, GlobalResetWithEc

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_INVALID_PARAMETER   If ResetType is invalid.
**/
EFI_STATUS
PchReset (
  IN PCH_RESET_INSTANCE *PchResetInstance,
  IN PCH_RESET_TYPE     PchResetType
  );
#endif
