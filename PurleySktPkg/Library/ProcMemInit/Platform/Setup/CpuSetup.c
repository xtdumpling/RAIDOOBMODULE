/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * CPU Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#include "SysFunc.h"

/**

  Processor specific input setup

  @param setup - pointer to setup area of host structure

  @retval N/A

**/
void
GetCpuSetupOptions (
  struct sysSetup *setup
  )
{
   //
   // Init CPU feature related setup options to default safe values that ensure minimum boot.
   // A platform hook will be called later to override these values.
   // All data in the syshost structure default to 0 at this point
   //
   setup->cpu.PchTraceHubEn = 0;                      // Set NPK_STH_ACPIBAR_BASE Enable
   setup->cpu.debugInterfaceEn = 0;                   // CPU Debug Interface Enable
   setup->cpu.dcuModeSelect = 0;                      // CPU DCU Mode select
   setup->cpu.flexRatioEn = 0;                        // 0 = keep HW default ratio
   setup->cpu.flexRatioNext = 0;                      // 0 = keep HW default ratio
   setup->cpu.vrsOff = 0;                             // 1 = VRsOff default is 1
   setup->cpu.smtEnable = 1;                          // 0/1 Disable/Enable SMT(HT)
   setup->cpu.vtEnable = 0;                           // 0/1 Disable/Enable VT
   setup->cpu.ltEnable = 0;
   setup->cpu.ltsxEnable = 0;
   setup->cpu.EnableGv = 1;                           // 0/1 Disable/Enable GV3
   setup->cpu.ConfigTDPLevel = 0;                     // 0 - 2: 0 = Normal; 1 =  Level 1; 2 = Level 2
   setup->cpu.PpinControl = 0;                        // 0/1 Disable/Enable PPIN_CTL
   setup->cpu.UFSDisable = 0;                         // 0 = Keep UFS default as Enabled
   setup->cpu.CheckCpuBist = 1;                       // 0 - ignore BIST result, 1 - will disable BIST fail core
   setup->cpu.AllowMixedPowerOnCpuRatio = 0;          // 0 = Force common ratio among CPU sockets  1 = Allow mixed ration
   setup->WFRWAEnable = 2;
   setup->PmaxDisable = 0;                            // 0 default for Pmax detector disable


}

