//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.03
//      Bug Fixed:  Add tRWSR w/a setup option for Samsung DIMMs.
//      Reason:     Improve memory RX margin
//      Auditor:    Stephen Chen
//      Date:       Aug/18/2017
//
//  Rev. 1.02
//      Bug Fixed:  Add tCCD_L Relaxation setup option for specific DIMMs.
//      Reason:     Requested by Micron, code reference from Jian.
//      Auditor:    Jacker Yeh
//      Date:       Mar/17/2017
//
//  Rev. 1.01
//      Bug Fixed:  Add NVDIMM function
//      Reason:     
//      Auditor:    Leon Xu
//      Date:       Feb/17/2017
//
//  Rev. 1.00
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/07/2016
//
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioMemoryConfigData.h

  Data format for Ioh Memory Config Data Structure

**/

#ifndef   __SOCKET_MEMORY_CONFIG_DATA_H__
#define   __SOCKET_MEMORY_CONFIG_DATA_H__


#include <UncoreCommonIncludes.h>
#include "SocketConfiguration.h"
#include <MemDefaults.h>
#include "Token.h"	//SMCPKG_SUPPORT

extern EFI_GUID gEfiSocketMemoryVariableGuid;
#define SOCKET_MEMORY_CONFIGURATION_NAME L"SocketMemoryConfig"

#pragma pack(1)
typedef struct {
    UINT8                   dfxMaxNodeInterleave;
    UINT8                   MemoryHotPlugBase;
    UINT8                   MemoryHotPlugLen;
    UINT8                   Srat;
    UINT8                   SratMemoryHotPlug;
    UINT8                   SratCpuHotPlug;
    UINT8                   PagePolicy;
    UINT8                   PatrolScrub;
    UINT8                   PatrolScrubDuration;
    UINT8                   PatrolScrubAddrMode;
    UINT8                   partialmirror;
    UINT8                   partialmirrorsad0;
    UINT8                   PartialMirrorUefi;
    UINT16                  PartialMirrorUefiPercent;
    UINT16                  partialmirrorsize[MAX_PARTIAL_MIRROR]; // Array of sizes of different partial mirrors
    UINT8                   DemandScrubMode;
    UINT8                   SddcPlusOneEn;
    UINT16                  spareErrTh;
    UINT8                   DieSparing;
    UINT8                   DieSparingAggressivenessLevel;
    UINT8                   ADDDCEn;
    UINT8                   AdddcErrInjEn;
    UINT8                   leakyBktLo;
    UINT8                   leakyBktHi;
    UINT8                   DutyCycleTraining;
    UINT8                   refreshMode;
    UINT8                   dllResetTestLoops;
    UINT8                   DdrMemoryType;
    UINT8                   HwMemTest;
    UINT16                  MemTestLoops;
    UINT8                   EccSupport;
    UINT8                   SocketInterleaveBelow4GB;
    UINT8                   dfxDimmManagement;
    UINT8                   dfxPartitionDDRTDimm;
    UINT8                   dfxPartitionRatio[16];    //[MAX_SOCKET * MAX_IMC] = [8]
    UINT8                   volMemMode;
    UINT8                   dfxPerMemMode;
    UINT8                   memInterleaveGran1LM;
    UINT8                   dfxMemInterleaveGranPMemUMA;
    UINT8                   dfxCfgMask2LM;
    UINT8                   ImcInterleaving;
    UINT8                   ChannelInterleaving;
    UINT8                   RankInterleaving;
    UINT8                   CkeProgramming;
    UINT8                   SrefProgramming;
    UINT8                   PkgcSrefEn;
    UINT8                   CkeIdleTimer;
    UINT8                   ApdEn;
    UINT8                   PpdEn;
    UINT8                   DdrtCkeEn;
    UINT8                   OppSrefEn;
    UINT8                   DdrtSrefEn;
    UINT8                   MdllOffEn;
    UINT8                   CkMode;
    UINT8                   MemTestOnFastBoot;
    UINT8                   AttemptFastBoot;
    UINT8                   AttemptFastBootCold;
    UINT8                   bdatEn;
    UINT8                   ScrambleEnDDRT;
    UINT8                   ScrambleEn;  // for ddr4
    UINT8                   allowCorrectableError;
    UINT16                  ScrambleSeedLow;
    UINT16                  ScrambleSeedHigh;
    UINT8                   CustomRefreshRateEn;
    UINT8                   CustomRefreshRate;
    UINT8                   mcBgfThreshold;
    UINT8                   readVrefCenter;
    UINT8                   wrVrefCenter;
    UINT8                   haltOnMemErr;
    UINT8                   thermalthrottlingsupport;
    UINT8                   thermalmemtrip;
    UINT8                   DimmTempStatValue;
    UINT8                   XMPProfilesSup;
    UINT8                   XMPMode;
    UINT8                   tCAS;
    UINT8                   tRP;
    UINT8                   tRCD;
    UINT8                   tRAS;
    UINT8                   tWR;
    UINT16                  tRFC;
    UINT8                   tRRD;
    UINT8                   tRTP;
    UINT8                   tWTR;
    UINT8                   tFAW;
    UINT8                   tCWL;
    UINT8                   tRC;
    UINT8                   commandTiming;
    UINT16                  tREFI;
    UINT8                   DdrFreqLimit;
    UINT16                  Vdd;
    UINT8                   lrdimmModuleDelay;
    UINT32                  rmtPatternLength;
    UINT32                  rmtPatternLengthExt;
    UINT8                   check_pm_sts;
    UINT8                   check_platform_detect;
    UINT8                   MemPwrSave;
    UINT8                   ElectricalThrottlingMode;
    UINT8                   MultiThreaded;
    UINT8                   promoteMrcWarnings;
    UINT8                   promoteWarnings;
    UINT8                   oppReadInWmm;
    UINT16                  normOppInterval;
    UINT8                   sck0ch0;
    UINT8                   sck0ch1;
    UINT8                   sck0ch2;
    UINT8                   sck0ch3;
    UINT8                   sck0ch4;
    UINT8                   sck0ch5;
    UINT8                   sck1ch0;
    UINT8                   sck1ch1;
    UINT8                   sck1ch2;
    UINT8                   sck1ch3;
    UINT8                   sck1ch4;
    UINT8                   sck1ch5;
    UINT8                   sck2ch0;
    UINT8                   sck2ch1;
    UINT8                   sck2ch2;
    UINT8                   sck2ch3;
    UINT8                   sck2ch4;
    UINT8                   sck2ch5;
    UINT8                   sck3ch0;
    UINT8                   sck3ch1;
    UINT8                   sck3ch2;
    UINT8                   sck3ch3;
    UINT8                   sck3ch4;
    UINT8                   sck3ch5;
    UINT8                   mdllSden;
    UINT8                   memhotSupport;
    UINT8                   MemhotOutputOnlyOpt;
    UINT8                   ADREn;
#if SMCPKG_SUPPORT	
    UINT8                 SmcGlobalResetEn;
    UINT8                 SmcGracefulShutdownTriggerADR;
    UINT8                 SmcADRCompleteTimeout;
    UINT8                 SmcHideADR;
    UINT8                 SmcSetNVDIMM2WT;
#endif
    UINT8                   RankMargin;
    UINT8                   EnableBacksideRMT;
    UINT8                   EnableBacksideCMDRMT;
    UINT8                   EnableNgnBcomMargining;
    UINT8                   rankMaskEn;
    UINT8                   RankSparing;
    UINT8                   multiSparingRanks;
    UINT8                   caParity;
    UINT8                   dimmIsolation;
    UINT8                   smbSpeed;
    UINT8                   EnforcePOR;
    UINT8                   pda;
    UINT8                   turnaroundOpt;
    UINT8                   oneRankTimingMode;
    UINT8                   eyeDiagram;
#ifdef SSA_FLAG
    UINT8                   EnableBiosSsaLoader;
    UINT8                   EnableBiosSsaRMT;
    UINT8                   EnableBiosSsaRMTonFCB;
    UINT8                   BiosSsaPerBitMargining;
    UINT8                   BiosSsaDisplayTables;
    UINT8                   BiosSsaPerDisplayPlots;
    UINT8                   BiosSsaLoopCount;
    UINT8                   BiosSsaBacksideMargining;
    UINT8                   BiosSsaEarlyReadIdMargining;
    UINT8                   BiosSsaStepSizeOverride;
    UINT8                   BiosSsaRxDqs;
    UINT8                   BiosSsaRxVref;
    UINT8                   BiosSsaTxDq;
    UINT8                   BiosSsaTxVref;
    UINT8                   BiosSsaCmdAll;
    UINT8                   BiosSsaCmdVref;
    UINT8                   BiosSsaCtlAll;
    UINT8                   BiosSsaEridDelay;
    UINT8                   BiosSsaEridVref;
    UINT8                   BiosSsaDebugMessages;
#endif //SSA_FLAG
    UINT8                   DramRaplInit;
    UINT8                   BwLimitTfOvrd;
    UINT8                   perbitmargin;
    UINT8                   DramRaplExtendedRange;
    UINT8                   CmsEnableDramPm;
    UINT8                   logParsing;
    UINT8                   WritePreamble;
    UINT8                   ReadPreamble;
    UINT8                   WrCRC;
    // NGN options
    UINT8                   LockNgnCsr;
    UINT8                   NgnCmdTime;
    UINT8                   NgnEccCorr;
    UINT8                   NgnEccWrChk;
    UINT8                   NgnEccRdChk;
    UINT8                   CrMonetization;
    UINT8                   NgnDebugLock;
    UINT8                   NgnArsOnBoot;
    UINT8                   RmtOnColdFastBoot;
    UINT8                   mrcRepeatTest;
    UINT8                   dfxLowMemChannel;
    UINT8                   dfxHighAddressStartBitPosition;
    UINT8                   staggerref;
    UINT32                  memFlows;
    UINT32                  memFlowsExt;
    UINT8                   Blockgnt2cmd1cyc;
    UINT8                   Disddrtopprd;
    UINT8                   setTDPDIMMPower;
    UINT8                   setSecureEraseAllDIMMs;
    UINT8                   setSecureEraseSktCh[MAX_AEP_DIMM_SETUP];
    //
    // PPR related
    //
    UINT8                   pprType;
    UINT8                   pprErrInjTest;
    // CR QoS Configuration Profiles
    UINT8                   crQosConfig;
    // load NGN dimm mgmt driver
    UINT8                   dfxLoadDimmMgmtDriver;
    UINT8                   LegacyADRModeEn;
    // mixed NGN dimms SKU
    UINT8                   crMixedSKU;
#ifdef NVMEM_FEATURE_EN
    UINT8                   ADRDataSaveMode;
#endif
#ifdef MEM_NVDIMM_EN
    UINT8                   eraseArmNVDIMMS;
    UINT8                   restoreNVDIMMS;
    UINT8                   interNVDIMMS;
#endif
    UINT8                   imcBclk;
    UINT8                   spdCrcCheck;
#if SMCPKG_SUPPORT
#if MemoryMapOut_SUPPORT
    UINT8                   MemMapOut;
#endif//MemoryMapOut_SUPPORT
    UINT8                   tCCDLRelaxation;
    UINT8                   tRWSREqualization;
#endif//SMCPKG_SUPPORT
} SOCKET_MEMORY_CONFIGURATION;

#pragma pack()

#endif

