/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IePeiPolicyAccessLib.c

  Do Platform Stage IE initialization.

**/
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Ppi/Spi.h>
#include "Library/DebugLib.h"

#define R_PCH_SPI_STRP_IE_DISABLE       121

/**
  Read soft-strap to determine if IE device is enabled

  param                     None

  retval TRUE               IE device is enabled
  retval FALSE              IE device is disabled
 **/
BOOLEAN IeIsEnabled( VOID )
{
  EFI_STATUS        Status;
  UINT32            SoftStrapValue;
  EFI_SPI_PROTOCOL  *SpiPpi;

  Status = PeiServicesLocatePpi( &gPeiSpiPpiGuid,
                                  0,
                                  NULL,
                                  (VOID **) &SpiPpi
                                );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[IE Policy] : Cannot locate SpiPPI (%r)\n", Status));
    return 0;
  }

  Status = SpiPpi->ReadPchSoftStrap (SpiPpi, R_PCH_SPI_STRP_IE_DISABLE * sizeof(UINT32), sizeof (SoftStrapValue),(UINT8*)&SoftStrapValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[IE Policy] : Cannot read soft-strap %d (%r)\n", R_PCH_SPI_STRP_IE_DISABLE, Status));
    return 0;
  }
  DEBUG ((DEBUG_INFO, "[IE Policy] : Read soft-strap %d (0x%08x). Soft-strap bit (17) is ", R_PCH_SPI_STRP_IE_DISABLE, SoftStrapValue));
  if (SoftStrapValue & BIT17) {
    DEBUG ((DEBUG_INFO, " set, IE Disabled\n"));
    return FALSE;
  } else {
    DEBUG ((DEBUG_INFO, "not set, IE Enabled\n"));
    return TRUE;
  } 
}