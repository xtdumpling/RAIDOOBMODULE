/** @file
  Initializes PCH LPC Device in PEI

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

#include "PchInitPei.h"

/**
  Configure LPC device on early PEI.
**/
VOID
ConfigureLpcOnEarlyPei (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "ConfigureLpcOnEarlyPei() \n"));

  //
  // Enhance port 8xh LPC decoding.
  // The can be disable by policy EnhancePort8xhDecoding.
  //
  PchLpcGenIoRangeSet (0x80, 0x10, LPC_ESPI_FIRST_SLAVE);
}

/**
  Configure LPC device on Policy callback.

  @param[in] PchPolicyPpi  The PCH Policy PPI instance
**/
VOID
ConfigureLpcOnPolicy (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  )
{
  PCH_LPC_GEN_IO_RANGE_LIST             LpcGenIoRangeList;
  UINTN                                 LpcBase;
  UINTN                                 Index;

  DEBUG ((DEBUG_INFO, "ConfigureLpcOnPolicy()\n"));
  //
  // If EnhancePort8xhDecoding is disabled, clear LPC and DMI LPC general IO range register.
  //
  if (PchPolicyPpi->LpcConfig.EnhancePort8xhDecoding == 0) {
    DEBUG ((DEBUG_INFO, "Disable EnhancePort8xhDecoding\n"));
    SetMem (&LpcGenIoRangeList, sizeof (PCH_LPC_GEN_IO_RANGE_LIST), 0);
    PchLpcGenIoRangeGet (&LpcGenIoRangeList, LPC_ESPI_FIRST_SLAVE);
    for (Index = 0; Index < PCH_LPC_GEN_IO_RANGE_MAX; Index++) {
      if ((LpcGenIoRangeList.Range[Index].BaseAddr == 0x80) &&
          (LpcGenIoRangeList.Range[Index].Length   == 0x10))
      {
        //
        // Clear the LPC general IO range register and DMI LPC general IO range register.
        //
        LpcBase = MmPciBase (
                    DEFAULT_PCI_BUS_NUMBER_PCH,
                    PCI_DEVICE_NUMBER_PCH_LPC,
                    PCI_FUNCTION_NUMBER_PCH_LPC
                    );
        MmioWrite32 (
          LpcBase + R_PCH_LPC_GEN1_DEC + Index * 4,
          0
          );
        PchPcrWrite32 (
          PID_DMI, (UINT16) (R_PCH_PCR_DMI_LPCLGIR1 + Index * 4),
          0
          );
        break;
      }
    }
  }
}
