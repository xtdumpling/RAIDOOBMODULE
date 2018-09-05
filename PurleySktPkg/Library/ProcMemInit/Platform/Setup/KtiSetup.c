/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * KTI Reference Code
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

#include "DataTypes.h"
#include "PlatformHost.h"
#include "KtiLib.h"
#include "SysHost.h"
#include "SysFunc.h"
#include "KtiMisc.h"
#include "KtiSetupDefinitions.h"





KTI_DFX_PARM DfxParmDefaults = {
  0,             // DfxSystemWideParmStart
  KTI_AUTO,      // DfxHaltLinkFailReset
  KTI_AUTO,      // DfxKtiMaxInitAbort
  KTI_AUTO,      // DfxLlcShareDrdCrd
  BIAS_FWD_AUTO, // DfxBiasFwdMode
  KTI_AUTO,      // DfxSnoopFanoutEn
  KTI_AUTO,      // DfxHitMeEn
  KTI_AUTO,      // DfxFrcfwdinv
  KTI_AUTO,      // DfxDbpEnable
  KTI_AUTO,      // DfxOsbEn
  KTI_AUTO,      // DfxHitMeRfoDirsEn
  KTI_AUTO,      // DfxGateOsbIodcAllocEn
  KTI_AUTO,      // DfxDualLinksInterleavingMode
  DEGRADE_SUPPORTED_TOPOLOGY, // DfxSystemDegradeMode
  KTI_AUTO,      // DfxVn1En
  KTI_AUTO,      // DfxD2cEn
  KTI_AUTO,      // DfxD2kEn
  0              // DfxSystemWideParmEnd
};


/**

  KTI specific host structure setup options

  @param setup - pointer to setup area of host structure

  @retval VOID

**/
void
GetKtiSetupOptions (
  struct sysSetup *setup
  )
{
    UINT8     Ctr1, Ctr2;

    //
    // Fill-in the system wide options
    //
    for (Ctr1 = 0; Ctr1 < MAX_SOCKET; Ctr1++) {
      //
      // Assign resources equally among all IIOs
      //
      setup->kti.BusRatio[Ctr1] = BUS_RATIO_DFLT;
      //
      // Default enable discovered Fpgas
      //
      setup->kti.KtiFpgaEnable[Ctr1] = KTI_AUTO;
    }

    setup->kti.LegacyVgaSoc      = LEGACY_VGA_SOCKET_DFLT;   // Legacy VGA range target socket
    setup->kti.LegacyVgaStack    = LEGACY_VGA_STACK_DFLT;    // Legacy VGA range target stack
    setup->kti.MmioP2pDis        = KTI_DISABLE;              // Enable MMIOL P2P traffic across Sockets
    setup->kti.DebugPrintLevel   = KTI_DEBUG_PRINT_ALL;      // Enable all levels of debug messages
    setup->kti.IoDcMode          = IODC_AUTO;                // Auto by default, depends on topology
    setup->kti.DegradePrecedence = TOPOLOGY_PRECEDENCE;
    setup->kti.DirectoryModeEn   = KTI_ENABLE;
    setup->kti.SncEn             = KTI_ENABLE;               // Snc Enabled
    setup->kti.XptPrefetchEn     = KTI_ENABLE;               // XptPrefetch enabled
    setup->kti.KtiPrefetchEn     = KTI_ENABLE;               // KtiPrefetch enabled
    setup->kti.RdCurForXptPrefetchEn = KTI_AUTO;
    setup->kti.TscSyncEn         = KTI_AUTO;
    setup->kti.StaleAtoSOptEn    = KTI_DISABLE;
    setup->kti.LLCDeadLineAlloc  = KTI_ENABLE;
    //
    // Phy/LL system wide settings
    //
    setup->kti.KtiLinkSpeedMode = KTI_LINK_FULL_SPEED_MODE;
    setup->kti.KtiLinkSpeed     = MAX_KTI_LINK_SPEED;
    setup->kti.KtiLinkL0pEn     = KTI_AUTO;
    setup->kti.KtiLinkL1En      = KTI_AUTO;
    setup->kti.KtiFailoverEn    = KTI_AUTO;
    setup->kti.KtiLbEn          = KTI_DISABLE;
    setup->kti.KtiCrcMode       = CRC_MODE_AUTO;
    setup->kti.KtiCpuSktHotPlugEn       = KTI_DISABLE;
    setup->kti.KtiCpuSktHotPlugTopology = HOT_PLUG_TOPOLOGY_4S;
    setup->kti.KtiSkuMismatchCheck      = KTI_ENABLE;

    //
    // Phy/LL per link settings
    //
    for (Ctr1 = 0; Ctr1 < MAX_SOCKET; Ctr1++) {
      for (Ctr2 = 0; Ctr2 < MAX_KTI_PORTS; Ctr2++){
        setup->kti.PhyLinkPerPortSetting[Ctr1].Link[Ctr2].KtiPortDisable = KTI_DISABLE; //Assume all ports are enabled

        if (setup->kti.KtiLinkSpeed < FREQ_PER_LINK) {
          setup->kti.PhyLinkPerPortSetting[Ctr1].Phy[Ctr2].KtiLinkSpeed = setup->kti.KtiLinkSpeed;
        } else {
          setup->kti.PhyLinkPerPortSetting[Ctr1].Phy[Ctr2].KtiLinkSpeed = SPEED_REC_96GT;
        }

        setup->kti.PhyLinkPerPortSetting[Ctr1].Link[Ctr2].KtiLinkVnaOverride = KTI_LL_VNA_SETUP_MAX;
      }
    }

    //
    // Copy system-wide Dfx options.  These should not be changed
    //
    MemCopy ((UINT8*)&setup->kti.DfxParm, (UINT8*)&DfxParmDefaults, sizeof (KTI_DFX_PARM));


    //
    // Copy DFX Phy/LL per link settings
    //
    for (Ctr1 = 0; Ctr1 < MAX_SOCKET; Ctr1++) {
      for (Ctr2 = 0; Ctr2 < MAX_KTI_PORTS; Ctr2++){
        setup->kti.DfxCpuCfg[Ctr1].Link[Ctr2].DfxCrcMode = CRC_MODE_AUTO;
        setup->kti.DfxCpuCfg[Ctr1].Link[Ctr2].DfxL0pEnable = KTI_AUTO;
        setup->kti.DfxCpuCfg[Ctr1].Link[Ctr2].DfxL1Enable = KTI_AUTO;
        setup->kti.DfxCpuCfg[Ctr1].Phy[Ctr2].DfxKtiFailoverEn = KTI_AUTO;
      }
    }

  return;
}

