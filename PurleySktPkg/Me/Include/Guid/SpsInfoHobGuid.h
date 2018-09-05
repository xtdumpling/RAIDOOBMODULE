//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  1999-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SpsInfoHobGuid.h

  SPS info HOB definitions.

**/

#ifndef _SPS_INFO_HOB_GUID_H_
#define _SPS_INFO_HOB_GUID_H_

#include <SpsMisc.h>

//
// Definition of ME FW SPS Features Set - according to the
// Intel ME-BIOS Interface Specification (revision 1.0.3) - chapter 4.5.1
//
typedef union
{
  struct
  {
    UINT32 Set1;
    UINT32 Set2;
  } Data;
  struct
  {
    UINT32 NodeManager                 :1,    // 0
           PeciProxy                   :1,    // 1
           ICC                         :1,    // 2
           MeStorageServices           :1,    // 3
           BootGuard                   :1,    // 4
           PTT                         :1,    // 5
           OemDefinedCpuDebugPolicy    :1,    // 6
           ResetSuppression            :1,    // 7
           PmBusProxy                  :1,    // 8
           CpuHotPlug                  :1,    // 9
           MicProxy                    :1,    //10
           MctpProxy                   :1,    //11
           ThermalReportVolumeAirflow  :1,    //12
           SocThermalReporting         :1,    //13
           DualBIOSSupport             :1,    //14
           MPHYSurvivabilityProgramming:1,    //15
           PECIOverDMI                 :1,    //16
           PCHDebug                    :1,    //17
           PowerThermalUtilitySupport  :1,    //18
           FiaMuxConfiguration         :1,    //19
           PchThermalSensorInit        :1,    //20
           DeepSx                      :1,    //21
           DualMeFwImage               :1,    //22
           DirectFwUpdate              :1,    //23
           MctpInfrastructure          :1,    //24
           CUPS                        :1,    //25
           FlashDescRegionVerification :1,    //26
           Hotham                      :1,    //27
           TurboStateLimiting          :1,    //28
           TelemetryHub                :1,    //29
           MeShutdown                  :1,    //30
           ASA                         :1;    //31
    UINT32 WarmResetNotificationSubFlow:1;    //0
  } Bits;
} SPS_FEATURE_SET;

typedef struct _SPS_INFO_HOB_
{
  SPS_FEATURE_SET   FeatureSet;
  UINT8             WorkFlow;
  BOOLEAN           PwrOptBoot;    // NM may request power reduction at boot
  UINT8             Cores2Disable; // NM may request disabling processor cores
  BOOLEAN           NmPowerMsmtSupport;
  BOOLEAN           NmHwChangeStatus;
} SPS_INFO_HOB;


extern EFI_GUID gSpsInfoHobGuid;

#endif // _SPS_INFO_HOB_GUID_H_

