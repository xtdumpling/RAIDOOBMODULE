/** @file
  Contains protocol information for the Platform Ras Policy Protocol.

  Copyright (c) 2009-2015 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/

#ifndef _PLATFORM_RAS_POLICY_PROTOCOL_H_
#define _PLATFORM_RAS_POLICY_PROTOCOL_H_
#include <UncoreCommonIncludes.h>
#include <MemPlatform.h>

//
// Forward reference for pure ANSI compatability
//

#define EFI_PLATFORM_RAS_POLICY_PROTOCOL_GUID \
   { \
      0x86b091ed, 0x1463, 0x43b5, 0x82, 0xa1, 0x2c, 0x8b, 0x83, 0xcb, 0x89, 0x17 \
   }

// RAS Structures

#define MAX_TRANSACTION_TYPE           12
#define MAX_ALLOWED_TRANSACTION_TYPE   64 //This is the maximum possible AEP transaction types allowed by AEP DDRT Alert Policy (Reserved types included)
#define MAX_THREAD_ID (1 << 6)

typedef struct _CPU_INFO{
  UINT8                   Valid;
  UINT16                  CpuNum;
  UINT8                   ApicId;
  UINT8                   ApicVer;
  UINT8                   BspFlag;
  UINT8                   BusyFlag;
} CPU_INFO;

typedef struct _QPI_INFO {
  UINT8                   LinkValid;
} QPI_INFO;

typedef struct _DIMM_INFO {
  UINT8                   Valid;
  UINT8                   X4present;
  UINT8                   MemSize;
  UINT8                   NumOfRanks;         // Number of Ranks on this DIMM
  UINT16                  SmbiosType17Handle;
} DIMM_INFO;

typedef struct _CHANNEL_INFO {
  UINT8                   Valid;
  UINT8                   ChannelFailed;
  UINT8                   NgnChannelFailed;
  UINT8                   NumRanks;
  UINT8                   MaxDimms;
  DIMM_INFO               DimmInfo[MAX_DIMM];
} CHANNEL_INFO;

typedef struct _PCI_PORT_INFO {
  UINT16                  BusDevFun;
  UINT8                   NmiSciFlag;
  UINT8                   UrMask;    // unsupp req mask
  UINT8                   Bus;
  UINT8                   Device;
  UINT8                   Function;
  UINT8                   StackPerPort;
  UINT64                  HdrLog1;            
} PCI_PORT_INFO;

typedef struct _IIO_STACK_INFO {
  UINT8                   Valid;
  UINT8                   BusBase;
  UINT8                   BusLimit;
} IIO_STACK_INFO;

typedef struct _UNCORE_INFO {
  UINT8                   M3UpiCount;
  UINT8                   M2PcieCount;
} UNCORE_INFO;

typedef struct _UNCORE_IIO_INFO {
  UINT8                   Valid;
  UINT8                   SocketId;            // Socket ID of the IIO (0..3)
  UINT8                   IIOBusBase;
  UINT8                   FpgaStack;           //IIO Stack which connects to FPGA
  PCI_PORT_INFO           PciPortInfo[NUMBER_PORTS_PER_SOCKET];
  IIO_STACK_INFO          IioStackInfo[MAX_IIO_STACK];
} UNCORE_IIO_INFO;

typedef struct _SOCKET_INFO {
  UINT8                   Valid;
  UINT8                   CoreCount;    //MCC,HCC, LCC
  UINT8                   imcEnabled[MAX_IMC];
  CPU_INFO                CpuInfo[MAX_CORE][MAX_THREAD_ID];
  UINT16                  NumQpiLinks;
  QPI_INFO                QpiInfo[MAX_KTI_PORTS];
  CHANNEL_INFO            ChannelInfo[MAX_CH];
  UNCORE_INFO             UncoreInfo;
  UNCORE_IIO_INFO         UncoreIioInfo;
} SOCKET_INFO;

typedef struct _SYSTEM_INFO {
  UINT8                   BoardId;
  UINT16                  NumOfSocket;
  UINT8                   NumMemNode;
  UINT8                   NumChanEnabled;
  UINT16                  SocketBitMap;
  UINT16                  CpuType;
  UINT16                  CpuSubType;
  UINT8                   SystemRasType;
  UINT16                  CpuStepping;
  UINT16                  MmCfgBase;
  UINT8                   MemType;
  UINT8                   RasModesEnabled;
  UINT16                  SmbiosType16Handle;
  UINT8                   FpgaSktActive;
  SOCKET_INFO             SocketInfo[MAX_SOCKET];
  UINT8                   ProcessorValid[MAX_CPU_NUM];
//  UNCORE_PCH_INFO         UncorePchInfo[NumPch];  // NOT YET DEFINED
//  SYSTEM_PCI_INFO         SystemPciInfo;          // NOT YET DEFINED
  // APTIOV_SERVER_OVERRIDE_RC_START
  BOOLEAN                 LastBootErrorFlag;  //Added support for Last Boot Error Logging
  BOOLEAN                 SkipFillOtherMcBankInfoFlag;
  // APTIOV_SERVER_OVERRIDE_RC_END
} SYSTEM_INFO;

//
// RAS System Topology Structure – Contains all necessary system topology information
//

typedef struct _EFI_RAS_SYSTEM_TOPOLOGY {
  SYSTEM_INFO             SystemInfo;
} EFI_RAS_SYSTEM_TOPOLOGY;

////////////////////////////////////////////////

typedef struct {
  BOOLEAN                 PoisonCap;
  BOOLEAN                 ViralCap;
  BOOLEAN                 CloakingCap;
  BOOLEAN                 EmcaGen1Cap;
  BOOLEAN                 EmcaGen2Cap;
  BOOLEAN                 QpiLinkLvlRetryCap;
  BOOLEAN                 QpiLinkWidthReduCap;
  BOOLEAN                 MemDevTagCap;
  BOOLEAN                 MemDddcCap;
  BOOLEAN                 MemB2BDddcCap;
  BOOLEAN                 MemRankSpareCap;
  BOOLEAN                 MemMulRankSpareCap;
  BOOLEAN                 IoMcaCap;
  BOOLEAN                 EnhanRpErrRepCap;
  BOOLEAN                 LerCap;
  BOOLEAN                 VmseCap;
  BOOLEAN                 LmceCap;
} SILICON_RAS_CAPABILITY;

typedef  struct {
  BOOLEAN                 DengateCap;
  BOOLEAN                 WheaLogCap;
  BOOLEAN                 WheaProcLogCap;
  BOOLEAN                 WheaMemLogCap;
  BOOLEAN                 WheaPciLogCap;
  BOOLEAN                 EmcaLogCap;
  BOOLEAN                 EmcaMemLogCap;
  BOOLEAN                 EmcaProcLogCap;
  BOOLEAN                 WheaErrInjCap;
  BOOLEAN                 WheaProcErrInjCap;
  BOOLEAN                 WheaMemErrInjCap;
  BOOLEAN                 WheaPcieErrInjCap;
  BOOLEAN                 McaBankErrInjCap;
} PLATFORM_RAS_CAPABILITY;

//
//  RAS Capability Structures – Contains all silicon and platform RAS capabilities
//

typedef struct {
  PLATFORM_RAS_CAPABILITY PlatformRasCapability;
  SILICON_RAS_CAPABILITY  SiliconRasCapability;
} SYSTEM_RAS_CAPABILITY;

////////////////////////////////////////////////

typedef struct {
  UINT8                   EmcaEn;
  UINT8                   EmcaIgnOptin;
  UINT8                   EmcaCsmiEn;
  UINT8                   EmcaMsmiEn;
  UINT8                   ElogCorrErrEn;
  UINT8                   ElogMemErrEn;
  UINT8                   ElogProcErrEn;
  UINT8                   LmceEn;
} EMCA;

typedef struct {
  UINT8                   WheaSupportEn;
  UINT8                   WheaLogMemoryEn;
  UINT8                   WheaLogProcEn;
  UINT8                   WheaLogPciEn;
} WHEA;

typedef struct {
  UINT8                   WheaErrorInjSupportEn;
  UINT8                   McaBankErrInjEn;
  UINT8                   WheaErrInjEn;
  UINT8                   WheaPcieErrInjEn;
  UINT8                   MeSegErrorInjEn;
  UINT8                   PcieErrInjActionTable;
  UINT8                    ParityCheckEn;
} ERR_INJ;

typedef struct {
  UINT8                   QpiCpuSktHotPlugEn;
  UINT8                   DirectoryModeEn;
} QPI;

typedef struct {
  UINT8                   MemErrEn;
  UINT8                   CorrMemErrEn;
  UINT32                  LeakyBktHiLeakyBktLo;
  UINT8                   SpareIntSelect;
  UINT8                   FnvErrorEn;                 // 0 - Disable, 1 - Enable
  UINT8                   FnvErrorLowPrioritySignal;  // 0 - No Log, 1 - SMI, 2 - ERR0#
  UINT8                   FnvErrorHighPrioritySignal; // 0 - No Log, 1 - SMI, 2 - ERR0#
  UINT16                  HostAlerts[MAX_ALLOWED_TRANSACTION_TYPE]; // For FNV MAX_ALLOWED_TRANSACTION_TYPE = 64
  //UINT8                   DieSparing;                   // To Be used after restructure of PEI phase, for now these options are got from host struct
  //UINT8                   DieSparingAggressivenessLevel;// To Be used after restructure of PEI phase, for now these options are got from host struct
  UINT8                   NgnAddressRangeScrub;
  UINT8                   NgnHostAlertAit;
  UINT8                   NgnHostAlertDpa;
  UINT8                   pprType;                // PPR type - Hard / Soft / Disabed
  UINT8                   pprErrInjTest;          // Enable/disable support for Error injection test using Cscripts
  UINT8                   multiSparingRanks;
  UINT8                   PatrolScrubAddrMode;
} MEMORY;

typedef struct {
  UINT8                   IioErrorEn;
  UINT8                   IoMcaEn;
  UINT8                   IioErrRegistersClearEn;
  UINT8                   IioErrorPinEn;
  UINT8                   LerEn;
  UINT8                   DisableLerMAerrorLogging;
  UINT8                   IioCoreErrorEn;
  UINT32                  IioCoreErrorBitMap;          // IIOERRCTL
  UINT32                  IioCoreErrorSevMap;          // IIOERRSV
  UINT8                   IioIrpErrorEn;
  UINT32                  IioIrpp0ErrCtlBitMap;
  UINT64                  IioCohSevBitMap;             // IRPPERRSV
  UINT8                   IioMiscErrorEn;
  UINT32                  IioMiscErrorBitMap;          // MIERRCTL
  UINT32                  IioMiscErrorSevMap;          // MIERRSV
  UINT8                   IioVtdErrorEn;
  UINT32                  IioVtdBitMap;                // VTUNCERRMSK
  UINT32                  IioVtdSevBitMap;             // VTUNCERRSEV
  UINT32                  IioItcErrSevBitMap;          // ITCERRSEV
  UINT32                  IioOtcErrSevBitMap;          // OTCERRSEV
  UINT8                   IioDmaErrorEn;
  UINT8                   IioDmiErrorEn;
  UINT32                  IioDmaBitMap;                // CHANERRMSK
  UINT32                  IioDmaSevBitMap;             // CHANERRSEV
  UINT8                   IioPcieAddCorrErrorEn;
  UINT8                   IioPcieAddUnCorrEn;
  UINT32                  IioPcieAddUnCorrBitMap;      // XPUNCEDMASK, XPUNCERRMSK
  UINT32                  IioPcieAddUnCorrSevBitMap;   // XPUNCERRSEV
  UINT8                   IioPcieAerSpecCompEn;
  UINT8                   IioPciePortEcrcEn[MAX_TOTAL_PORTS]; //From Setup
//SGI+
  UINT8                   MaskPcieCorrError;
//SGI-  
} IIO;

typedef struct {
  UINT8                   PcieErrEn;
  UINT8                   PcieCorrErrEn;
  UINT8                   PcieUncorrErrEn;
  UINT8                   PcieFatalErrEn;
  UINT8                   PcieCorErrCntr;
  UINT32                  PcieCorErrMaskBitMap;
  UINT16                  PcieCorErrThres;
  UINT8                   PcieAerCorrErrEn;
  UINT32                  PcieAerCorrErrBitMap;     // COREDMASK, CORERRMSK
  UINT8                   PcieAerAdNfatErrEn;
  UINT32                  PcieAerAdNfatErrBitMap;   // UNCERRMSK, UNCEDMASK, UNCERRSEV
  UINT8                   PcieAerNfatErrEn;
  UINT32                  PcieAerNfatErrBitMap;     // UNCERRMSK, UNCEDMASK, UNCERRSEV
  UINT8                   PcieAerFatErrEn;
  UINT32                  PcieAerFatErrBitMap;      // UNCERRMSK, UNCEDMASK, UNCERRSEV
  UINT8                   SerrPropEn;
  UINT8                   PerrPropEn;
  UINT8                   OsSigOnSerrEn;
  UINT8                   OsSigOnPerrEn;
} PCIE;

typedef struct {
  UINT8                   CaterrGpioSmiEn;
} PLATFORM;

//
//  RAS Setup Structure – Contains all RAS Setup options
//

typedef struct {
  UINT8                       SystemErrorEn;
  UINT8                       PoisonEn;
  UINT8                       ViralEn;
  UINT8                       CloakingEn;
  UINT8                       ClearViralStatus;
  UINT8                       UboxToPcuMcaEn;
  UINT8                       FatalErrSpinLoopEn;
  // APTIOV_SERVER_OVERRIDE_RC_START : Unlocking the MSR 0x790 for the ErrorInjection to work from RAS tools
  UINT8                       UnlockMsr;
  // APTIOV_SERVER_OVERRIDE_RC_END : Unlocking the MSR 0x790 for the ErrorInjection to work from RAS tools
  EMCA                        Emca;
  WHEA                        Whea;
  ERR_INJ                     ErrInj;
  QPI                         Qpi;
  MEMORY                      Memory;
  IIO                         Iio;
  PCIE                        Pcie;
  PLATFORM                    Platform;
} SYSTEM_RAS_SETUP;

// Protocol Interfaces

typedef struct _EFI_PLATFORM_RAS_POLICY_PROTOCOL {
  SYSTEM_RAS_SETUP        *SystemRasSetup;
  SYSTEM_RAS_CAPABILITY   *SystemRasCapability;
  EFI_RAS_SYSTEM_TOPOLOGY *EfiRasSystemTopology;
} EFI_PLATFORM_RAS_POLICY_PROTOCOL;

extern EFI_GUID gEfiPlatformRasPolicyProtocolGuid;

#endif
