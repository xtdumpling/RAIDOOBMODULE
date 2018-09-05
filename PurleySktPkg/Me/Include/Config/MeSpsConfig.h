/** @file
  ME config block.

@copyright
  Copyright (c) 2016 Intel Corporation. All rights reserved
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
#ifndef _ME_SPS_CONFIG_H_
#define _ME_SPS_CONFIG_H_

#define ME_SPS_CONFIG_REVISION     1


#define ME_SPS_ALTITUDE_NONE       0x8000
#define ME_SPS_MCTP_BUS_OWNER_NONE 0
#define ME_SPS_INIT_TIMEOUT        2    ///< Max SPS firmware initialization time

#define ME_SPS_HECI_IN_ME_OFF      0    ///< Do not request anything from ME regarding HECI visibility
#define ME_SPS_HECI_IN_ME_HIDE     1    ///< Request ME to hide HECI PCI config space
#define ME_SPS_HECI_IN_ME_DISABLE  2    ///< Request ME to disable HECI on PCI


#ifndef VFRCOMPILE

typedef struct
{
  UINT32 Revision;                      ///< Revision for the config structure
  
  // dword 0
  UINT16 SpsAltitude;
  UINT16 SpsMctpBusOwner;
  
  // dword 1
  UINT32 PreDidMeResetEnabled     : 1,  ///< 0: disabled, 1: enabled
         Heci1HideInMe            : 2,  ///< 0: do not set, 1 - hide, 2 disable
         Heci2HideInMe            : 2,
         Heci3HideInMe            : 2,
         DWord1Reserved           :25;
  
  // dword 2
  UINT32 NmPwrOptBoot             : 1,
         NmCores2Disable          : 7,
         DWord2Reserved           :24;
  
#if ME_TESTMENU_FLAG
  // dword 3
  UINT32 MeTimeout                : 8,  ///< ME initializaton timeout
         
         MeHmrfpoLockEnabled      : 1,  ///< Host ME Region Flash Protection Override test options
         MeHmrfpoEnableEnabled    : 1,
         
         MeGrLockEnabled          : 1,  ///< Supress global reset locking for testing purpose
         MeGrPromotionEnabled     : 1,  ///< Promote upcoming reset to global reset
         BreakRtcEnabled          : 1,  ///< Test option for breaking RTC configuration
         
         SpsIccClkOverride        : 1,  ///< Enable overriding default SPS ICC Clock configuration
         SpsIccClkSscSetting      : 1,  ///< Enable/disable spread spectrum
         
         NmPwrOptBootOverride     : 1,  ///< NM boot time policy test options
         NmCores2DisableOverride  : 1,
         
         NmPowerMsmtOverride      : 1,  ///< NM PTU power measurement test options
         NmPowerMsmtSupport       : 1,
         NmHwChangeOverride       : 1,
         NmHwChangeStatus         : 1,
         NmPtuLoadOverride        : 1,  ///< For MROM-less systems only
         
         DWord3Reserved           :10;
#else
  UINT32 DWord3Reserved;
#endif // ME_TESTMENU_FLAG
  
} ME_SPS_CONFIG;

#endif // VFRCOMPILE


#endif // _ME_SPS_CONFIG_H_

