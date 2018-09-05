/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/** @file

Copyright (c) 2007 - 2008, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IdeBus.h

  System reset Library Services.  This library class provides a set of
  methods to reset whole system with manipulate ICH.

**/


#include <Base.h>


#include <Library/ResetSystemLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsLpc.h>
#include <PchAccess.h>
#include <Library/MmPciBaseLib.h>

/**
  Calling this function causes a system-wide reset. This sets
  all circuitry within the system to its initial state. This type of reset
  is asynchronous to system operation and operates without regard to
  cycle boundaries.

  System reset should not return, if it returns, it means the system does
  not support cold reset.
**/
VOID
EFIAPI
ResetCold (
  VOID
  )
{
  IoWrite8 (R_PCH_RST_CNT, 0x2);
  IoWrite8 (R_PCH_RST_CNT, 0x6);
}

/**
  Calling this function causes a system-wide initialization. The processors
  are set to their initial state, and pending cycles are not corrupted.

  System reset should not return, if it returns, it means the system does
  not support warm reset.
**/
VOID
EFIAPI
ResetWarm (
  VOID
  )
{
  IoWrite8 (R_PCH_RST_CNT, 0x0);
  IoWrite8 (R_PCH_RST_CNT, 0x4);
}

/**
  Calling this function causes the system to enter a power state equivalent
  to the ACPI G2/S5 or G3 states.

  System shutdown should not return, if it returns, it means the system does
  not support shut down reset.
**/
VOID
EFIAPI
ResetShutdown (
  VOID
  )
{
  UINT16  IchPmioBase;
  UINT16  Data16;
  UINT32  Data32;

  //
  // Firstly, ACPI decode must be enabled
  //
  MmioOr8 ((
    MmPciBase(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_ACPI_CNT),
    (UINT8) (B_PCH_PMC_ACPI_CNT_ACPI_EN)
    );
  IchPmioBase = (UINT16)(
                  (MmPciBase(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_PMC) +
                  R_PCH_PMC_ACPI_BASE) & B_PCH_PMC_ACPI_BASE_BAR);
  //
  // Then, GPE0_EN should be disabled to avoid any GPI waking up the system from S5
  // 
  Data16 = 0;
  IoWrite16 (
    (UINTN)(IchPmioBase + R_PCH_ACPI_GPE0_EN_127_96), 
    (UINT16)Data16
    );

  //
  // Secondly, PwrSts register must be cleared
  //
  // Write a "1" to bit[8] of power button status register at 
  // (PM_BASE + PM1_STS_OFFSET) to clear this bit
  //  
  Data16 = B_PCH_ACPI_PM1_STS_PWRBTN;
  IoWrite16 (
    (UINTN)(IchPmioBase + R_PCH_ACPI_PM1_STS), 
    (UINT16)Data16
    );
      
  //
  // Finally, transform system into S5 sleep state
  //
  Data32 = IoRead32 ((UINTN)(IchPmioBase + R_PCH_ACPI_PM1_CNT));

  Data32  = (UINT32) ((Data32 & ~(B_PCH_ACPI_PM1_CNT_SLP_TYP + B_PCH_ACPI_PM1_CNT_SLP_EN)) | V_PCH_ACPI_PM1_CNT_S5);

  IoWrite32 (
    (UINTN) (IchPmioBase + R_PCH_ACPI_PM1_CNT), 
    (UINT32)Data32
    );

  Data32 = Data32 | B_PCH_ACPI_PM1_CNT_SLP_EN;

  IoWrite32 (
    (UINTN) (IchPmioBase + R_PCH_ACPI_PM1_CNT), 
    (UINT32)Data32
    );

  return;
}

/**
  Calling this function causes the system to enter a power state for capsule
  update.

  Reset update should not return, if it returns, it means the system does
  not support capsule update.

**/
VOID
EFIAPI
EnterS3WithImmediateWake (
  VOID
  )
{
  ASSERT (FALSE);
}

