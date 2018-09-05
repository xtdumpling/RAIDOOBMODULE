/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

Copyright (c)  1999 - 2004 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file Stall.c
   
  Produce Stall Ppi.
  
**/

#include <Platform.h>
#include <Ppi/Stall.h>
#include <Library/IoLib.h>
#include <Library/PlatformHooksLib.h>


/**

  Waits for at least the given number of microseconds.

  @param PeiServices     General purpose services available to every PEIM.
  @param This            PPI instance structure.
  @param Microseconds    Desired length of time to wait.

  @retval EFI_SUCCESS     If the desired amount of time was passed.

**/
EFI_STATUS
EFIAPI
Stall (
  IN EFI_PEI_SERVICES   **PeiServices,
  IN EFI_PEI_STALL_PPI  *This,
  IN UINTN              Microseconds
  )
{
  UINTN                 Ticks;
  UINTN                 Counts;
  UINT32                CurrentTick;
  UINT32                OriginalTick;
  UINT32                RemainingTick;


  if (Microseconds == 0) {
    return EFI_SUCCESS;
  }

  OriginalTick = IoRead32 (PCH_ACPI_TIMER_ADDRESS);
  OriginalTick &= (PCH_ACPI_TIMER_MAX_VALUE - 1);
  CurrentTick = OriginalTick;
  
  //
  // The timer frequency is 3.579545MHz, so 1 ms corresponds to 3.58 clocks
  //
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;
  
  //
  // The loops needed for timer overflow
  //
  Counts = Ticks / PCH_ACPI_TIMER_MAX_VALUE;
  
  //
  // Remaining clocks within one loop
  //
  RemainingTick = (UINT32)Ticks % PCH_ACPI_TIMER_MAX_VALUE;  
  
  //
  // Do not intend to use TMROF_STS bit of register PM1_STS, because this add extra
  // one I/O operation, and may generate SMI
  //
  
  while (Counts != 0) {
    CurrentTick = IoRead32 (PCH_ACPI_TIMER_ADDRESS);
    CurrentTick &= (PCH_ACPI_TIMER_MAX_VALUE - 1);
    if (CurrentTick <= OriginalTick) {
      Counts--;
    }
    OriginalTick = CurrentTick;
  }

  while ((RemainingTick > CurrentTick) && (OriginalTick <= CurrentTick) ) {
    OriginalTick = CurrentTick;
    CurrentTick = IoRead32 (PCH_ACPI_TIMER_ADDRESS);
    CurrentTick &= (PCH_ACPI_TIMER_MAX_VALUE - 1);
  }        
        
  return EFI_SUCCESS;
}
