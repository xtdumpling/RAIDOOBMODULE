/** @file
  Header file for the PCH SPI PEI Library.

@copyright
 Copyright (c) 2004 - 2017 Intel Corporation. All rights reserved
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
#ifndef _PCH_SPI_H_
#define _PCH_SPI_H_

#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/TimerLib.h>
#include <PchAccess.h>
#include <Ppi/Spi.h>
#include <Ppi/PchPolicy.h>
#include <IncludePrivate/Library/PchSpiCommonLib.h>

typedef struct {
  EFI_PEI_PPI_DESCRIPTOR  PpiDescriptor;
  SPI_INSTANCE            SpiInstance;
} PEI_SPI_INSTANCE;

/**
  Configure SPI device after memory.

  @param[in] PchPolicyPpi  The PCH Policy PPI instance
**/
VOID
ConfigureSpiAfterMem (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  );

/**
  Installs PCH SPI PPI

  @retval EFI_SUCCESS             PCH SPI PPI is installed successfully
  @retval EFI_OUT_OF_RESOURCES    Can't allocate pool
**/
EFI_STATUS
EFIAPI
InstallPchSpi (
  VOID
  );
#endif
