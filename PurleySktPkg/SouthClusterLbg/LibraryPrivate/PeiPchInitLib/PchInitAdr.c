/** @file
  Initializes PCH-side ADR.

@copyright
  Copyright (c) 2012 - 2016 Intel Corporation. All rights reserved
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
#include "PchInitAdr.h"

/**
  Initializes PCH-side ADR

  @param[in]  PchPolicy           The PCH Policy PPI instance

  @retval None
**/
EFI_STATUS
InitAdrPreMem (
  IN PCH_POLICY_PPI           *PchPolicy
  )
{
///
/// Lewisburg related settings.
///
  UINTN EvaMmIo;
  UINTN PmcBase;
  UINT32 AdrTimer;
  UINT32 PchPwrmBase;
  EFI_STATUS Status;

  DEBUG(( EFI_D_INFO, "PCH ADR config: PchAdrEn: %d, AdrTimerEn: %d, AdrTimerVal: %d\n",
      PchPolicy->AdrConfig.PchAdrEn,
      PchPolicy->AdrConfig.AdrTimerEn,
      PchPolicy->AdrConfig.AdrTimerVal
  ));
  DEBUG(( EFI_D_INFO, "PCH ADR config: AdrMultiplierVal: %d, AdrGpioSel: %d, AdrHostPartitionReset: %d\n",
      PchPolicy->AdrConfig.AdrMultiplierVal,
      PchPolicy->AdrConfig.AdrGpioSel,
      PchPolicy->AdrConfig.AdrHostPartitionReset
  ));
  ASSERT (PchPolicy->AdrConfig.PchAdrEn != PLATFORM_POR);
  ASSERT (PchPolicy->AdrConfig.AdrTimerEn != PLATFORM_POR);
  ASSERT (PchPolicy->AdrConfig.AdrHostPartitionReset != PLATFORM_POR);
  ASSERT (PchPolicy->AdrConfig.AdrTimerVal <= V_PCH_LBG_MROM_ADRTIMERCTRL_ADR_TMR_MAX);
  ASSERT (PchPolicy->AdrConfig.AdrMultiplierVal <= V_PCH_LBG_MROM_ADRTIMERCTRL_ADR_MULT_MAX);
  if (PchPolicy->AdrConfig.PchAdrEn == 1) {
    /*
      1) One option to choose value of ADR_TMR (BDF 0/17/0 Off 180h bits 31:30).  Valid values:
         25us  - 0b
         50us  - 1b
         100us - 10b
         0us   - 11b
      2) One option to choose multiplier ADR_MULT (BDF 0/17/0 Off 180h bits 27:24). Valid values:
         x1  - 0b
         x8  - 1b
         x24 - 10b
         x40 - 11b
         x56 - 100b
         x64 - 101b
         x72 - 110b
         x80 - 111b
         x88 - 1000b
         x96 - 1001b
               Other values are RESERVED.
      3) One option to choose GPIO_B or GPIO_C (mutually exclusive)
         When choosing GPIO_B:
            /// 1. Configure PM_SYNC in serial mode 0/31/2 A8h bit 9 = 1
            /// 2. Program PWRMBASE +D4h [13] = 1b, for GPIO_B to PM_SYNC Enable
            /// 3. Program PWRMBASE +F4h [13] = 1b, for GPIO_B to PM_SYNC Enable in C0
            /// 4. Program PWRMBASE +C8h [10:9] = 1b, to map CPU_GP_0 to GPIO_B
            /// 5. Program PWRMBASE +F0h [31:30] to 01b to select GPIO_B as ADR GPIO (Disable timer if requested by SV)
            /// 6.  Program CPU_GP_0 as GPIO
            /// 7. Program PWRMBASE +F0h [0] = 1b ADR feature enable

         When choosing GPIO_C:
            /// 1. Configure PM_SYNC in serial mode 0/31/2 A8h bit 9 = 1
            /// 2. Program PWRMBASE +D4h [14] = 2b, for GPIO_C to PM_SYNC Enable
            /// 3. Program PWRMBASE +F4h [14] = 2b, for GPIO_C to PM_SYNC Enable in C0
            /// 4. Program PWRMBASE +C8h [10:9] = 0b, to map CPU_GP_0 to GPIO_C
            /// 5. Program PWRMBASE +F0h [31:30] to 10b to select GPIO_C as ADR GPIO (Disable timer if requested by SV)
            /// 6.  Program CPU_GP_0 as GPIO
            /// 7. Program PWRMBASE +F0h [0] = 1b ADR feature enable
    */

    //
    // Get Pmc & PchPwrm Base adresses
    //
    PmcBase = MmPciBase (
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_PMC,
            PCI_FUNCTION_NUMBER_PCH_PMC
            );
    if (MmioRead16 (PmcBase) == 0xFFFF) {
      ASSERT (FALSE);
      return EFI_UNSUPPORTED;
    }

    Status = PchPwrmBaseGet(&PchPwrmBase);
    if (Status != EFI_SUCCESS) {
      return Status;
    }

    /// 1. Configure PM_SYNC in serial mode 0/31/2 A8h bit 9 = 1
    MmioOr32(
      PmcBase + R_PCH_PMC_BM_CX_CNF,
      B_PCH_PMC_BM_CX_CNF_PM_SYNC_MSG_MODE
    );
    if (PchPolicy->AdrConfig.AdrGpioSel == PM_SYNC_GPIO_B) {
      /// 2. Program PWRMBASE +D4h [13] = 1b for GPIO_B to PM_SYNC Enable
      MmioOr32(
        PchPwrmBase + R_PCH_PWRM_PM_SYNC_MODE,
        B_PCH_PWRM_PM_SYNC_MODE_GPIO_B
      );

      /// 3. Program PWRMBASE +F4h [13] =1b for GPIO_B to PM_SYNC Enable in C0
      MmioOr32(
        PchPwrmBase + R_PCH_PWRM_PM_SYNC_MODE_C0,
        (B_PCH_PWRM_PM_SYNC_MODE_GPIO_B)
      );

      /// 4. Program PWRMBASE +C8h [10:9] = 1b, to map CPU_GP_0 to GPIO_B
      MmioAndThenOr32(
        PchPwrmBase + R_PCH_PWRM_PMSYNC_MISC_CFG,
        ~(UINT32)(B_PCH_PWRM_PMSYNC_GPIO_C_SEL),
        B_PCH_PWRM_PMSYNC_GPIO_B_SEL
      );

      /// 5. Program PWRMBASE +F0h [31:30] to 01b to select GPIO_B as ADR GPIO (Disable timer if requested by SV)
      MmioAndThenOr32(
        PchPwrmBase + R_PCH_PWRM_ADR_EN,
        ~(B_PCH_PWRM_ADR_EN_ADR_GPIO_SEL_MASK),
        B_PCH_PWRM_ADR_EN_ADR_GPIO_SEL_GPIO_B
      );

    } else if (PchPolicy->AdrConfig.AdrGpioSel == PM_SYNC_GPIO_C) {
      /// 2. Program PWRMBASE +D4h [14] = 1b for GPIO_C to PM_SYNC Enable
      MmioOr32(
        PchPwrmBase + R_PCH_PWRM_PM_SYNC_MODE,
        B_PCH_PWRM_PM_SYNC_MODE_GPIO_C
      );

      /// 3. Program PWRMBASE +F4h [14] =1b for GPIO_C to PM_SYNC Enable in C0
      MmioOr32(
        PchPwrmBase + R_PCH_PWRM_PM_SYNC_MODE_C0,
        (B_PCH_PWRM_PM_SYNC_MODE_GPIO_C)
      );

      /// 4. Program PWRMBASE +C8h [10:9] = 0b, to map CPU_GP_0 to GPIO_C
      MmioAnd32(
        PchPwrmBase + R_PCH_PWRM_PMSYNC_MISC_CFG,
        ~(UINT32)(B_PCH_PWRM_PMSYNC_GPIO_C_SEL | B_PCH_PWRM_PMSYNC_GPIO_B_SEL)
      );

      /// 5. Program PWRMBASE +F0h [31:30] to 10b to select GPIO_C as ADR GPIO (Disable timer if requested by SV)
      MmioAndThenOr32(
        PchPwrmBase + R_PCH_PWRM_ADR_EN,
        ~(B_PCH_PWRM_ADR_EN_ADR_GPIO_SEL_MASK),
        B_PCH_PWRM_ADR_EN_ADR_GPIO_SEL_GPIO_C
      );
    }

    if (PchPolicy->AdrConfig.AdrHostPartitionReset == FORCE_DISABLE) {
      MmioAnd32(
        PchPwrmBase + R_PCH_PWRM_ADR_EN,
        ~(UINT32)(B_PCH_PWRM_ADR_EN_HPR_ADR_EN)
      );
    }

    /// 6.  Program CPU_GP_0 as GPIO
    /// GPIO is being programmed via BoardInit->GpioInit(), by adding "PchHGpio_GPP_E, V_GPIO_PAD3" row to platform GPIO configuration table

    EvaMmIo = MmPciBase( DEFAULT_PCI_BUS_NUMBER_PCH,
                         PCI_DEVICE_NUMBER_EVA,
                         PCI_FUNCTION_NUMBER_EVA_MROM0);
    AdrTimer = MmioRead32 (EvaMmIo + R_PCH_LBG_MROM_ADRTIMERCTRL);
    //
    // Only disable timer if ADR_DBG_DIS is 1
    //
    if ((AdrTimer & B_PCH_LBG_MROM_ADRTIMERCTRL_ADR_DBG_DIS) == 0 && PchPolicy->AdrConfig.AdrTimerEn == ADR_TMR_HELD_OFF_SETUP_OPTION) {
      DEBUG (( EFI_D_ERROR, "PCH ADR config: WARNING! ADR timer HELD OFF INDEFINITELY! FOR DEBUG PURPOSES ONLY!\n" ));
      MmioOr32(
        EvaMmIo + R_PCH_LBG_MROM_ADRTIMERCTRL,
        B_PCH_LBG_MROM_ADRTIMERCTRL_ADR_TMR_DIS
      );
    } else {
      // Set the selected timer value
      MmioAndThenOr32(
        EvaMmIo + R_PCH_LBG_MROM_ADRTIMERCTRL,
        ~B_PCH_LBG_MROM_ADRTIMERCTRL_ADR_TMR_MASK,
        (PchPolicy->AdrConfig.AdrTimerVal << N_PCH_LBG_MROM_ADRTIMERCTRL_ADR_TMR)
      );
      // Set the selected timer multiplier
      MmioAndThenOr32(
        EvaMmIo + R_PCH_LBG_MROM_ADRTIMERCTRL,
        ~(UINT32)B_PCH_LBG_MROM_ADRTIMERCTRL_ADR_MULT_MASK,
        (PchPolicy->AdrConfig.AdrMultiplierVal << N_PCH_LBG_MROM_ADRTIMERCTRL_ADR_MULT)
      );
    }

    /// 7. Program PWRMBASE +F0h [0] = 1b ADR feature enable
    /// GPIO BASE is done in platform code.
    MmioOr32(
      PchPwrmBase + R_PCH_PWRM_ADR_EN,
      B_PCH_PWRM_ADR_EN_ADR_FEAT_EN
    );
  }
  return EFI_SUCCESS;
}
