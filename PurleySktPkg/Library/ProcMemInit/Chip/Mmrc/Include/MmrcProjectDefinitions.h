/**

Copyright (c) 2005 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file File name:  MmrcProjectDefinitions.h

  Includes all hard defines specific to a project which are NOT 
  outputted by the MMRC tool. Things like the maximum number of 
  channels, ranks, DIMMs, etc, should be included here. This file 
  needs to be included by most MMRC components, including Mmrc.h.

**/

#ifndef _MMRCPROJECTDEFINITIONS_H
#define _MMRCPROJECTDEFINITIONS_H

#ifdef _MSC_VER
#pragma warning (disable: 4214)
#endif //_MSC_VER
#include "MmrcProjectDefinitionsGenerated.h"

//
// [31:24] = Major revision 
// [23:16] = Minor revision 
// [15:8]  = Release Candidate 
// [7:0]   = Internal revision 
//
#define MAX_ELEMENT_TABLE     40
#define MAX_CMDS              2
#define MAX_RMT_ELEMENTS      4

#ifndef PCIEX_BASE_ADDRESS
//
// PCIe Base Address
//
#ifndef SIM
#define PCIEX_BASE_ADDRESS  0xE0000000
#else
#define PCIEX_BASE_ADDRESS  0xC0000000
#endif
//
// PMC Memory Space
//
#define PBASE               0xFED03000
#endif

//
// Extended Configuration Base Address.  Port to value enabled by calling code, if necessary.
// If the EC space is not enabled by code calling the MRC, then enable the
// EC space to this value
//
#ifndef EC_BASE
#define EC_BASE   PCIEX_BASE_ADDRESS
#endif

#define BROADCAST_SUP               0x00        // Support for Broadcasting during phyinit's when looping on modules.
//
// EarlyMprTraining Parameters
//
#define EARLYTRAIN_MIDPOINTVREF    0x20
#define EARLYTRAIN_MARGINSTEP      0x01
#define EARLYTRAIN_LOWBOUNDARY     0x00
#define EARLYTRAIN_UPBOUNDARY      0x48
//
// RdVefTraining Parameters
//
#define RDVREFTRAIN_MIDPOINTVREF    0x14
#define RDVREFTRAIN_MARGINSTEP      0x02
#define RDVREFTRAIN_LOWBOUNDARY     0x00
#define RDVREFTRAIN_UPBOUNDARY      (2*RDVREFTRAIN_MIDPOINTVREF - 1)
//
// WrVefTraining Parameters
//
#define WRVREFTRAIN_MIDPOINTVREF    0x20
#define WRVREFTRAIN_MARGINSTEP      0x02
#define WRVREFTRAIN_LOWBOUNDARY     0x00
#define WRVREFTRAIN_UPBOUNDARY      0x3f
// 
// CMD Training Parameters
//
#define CMDTRAIN_MIDPOINTVREF       0x1ff  
#define CMDTRAIN_MARGINSTEP         0x02
#define CMDTRAIN_LOWBOUNDARY        0x00
#define CMDTRAIN_UPBOUNDARY         0x3ff 
// 
// RdTraining Parameters
//
#define RDTRAIN_MIDPOINTVREF        0x20
#define RDTRAIN_MARGINSTEP          0x02
#define RDTRAIN_LOWBOUNDARY         0x00
#define RDTRAIN_UPBOUNDARY          0x3f
// 
// WrTraining Parameters
//
#define WRTRAIN_MIDPOINTVREF        0x20
#define WRTRAIN_MARGINSTEP          0x02
#define WRTRAIN_LOWBOUNDARY         0x00
#define WRTRAIN_UPBOUNDARY          0x3f

#define PHYENTERWRLVL               0x1
#define PHYEXITWRLVL                0x0
//
// Pattern Creations
//
#define NUMBERPATTERNS              0x10  // Number of Victim/Aggressor Patterns * (repeat +1)= 10 * (16 + 1) = 10*17 = 170  Defined by tool
#define VICTIMPATTERNS              { 0xAAAAAAAA,  0xDB6DB6DB,  0x92492492,  0xEEEEEEEE,  0xCCCCCCCC,  0x88888888,  0xF7BDEF7B,  0xE739CE73,  0xC6318C63,  0x84210842}
#define AGGRESSORPATTERNS           {~0xAAAAAAAA, ~0xDB6DB6DB, ~0x92492492, ~0xEEEEEEEE, ~0xCCCCCCCC, ~0x88888888, ~0xF7BDEF7B, ~0xE739CE73, ~0xC6318C63, ~0x84210842}
#define PATTERN_SIZE                256   //  4 CL for Cedarview = 256 bytes , 2 CL for Berryville
#define FLYBY_SHIFT                 {0, 0, 0, 0, 0, 0, 0, 0}

#define FIFO_RESET_DISABLE          0x01
#define FIFO_RESET_ENABLE           0x00

#define BUSWIDTH                    0x8   // 1=x8, 2=x16, 4=x32, 8=x64
#define CHANNEL_BYTEOFFSET          0x1000
#define CHANNEL_BITOFFSET           0

#define MAX_DIMM_CONFIGS           144

#define DQ_BURSTLEN_8   0x00
#define DQ_BURSTLEN_DYN 0x01;
#define DQ_BURSTLEN_4   0x02

//
// Various portions of the MMRC that can be run by calling MmrcEntry().
//
typedef enum {
  EntireMmrc,
  DdrioPhyInit,
  MemoryTraining,
  PostTraining,
  HandlePeriodicComp,
  ExecuteSpecificFunction,
  CapsuleInit,
} MMRC_STAGE;
//
// Message Bus Commands
//
#define CMD_READ_REG          0x10    // Read Message Bus Register Command 
#define CMD_WRITE_REG         0x11    // Write Message Bus Register Command 
#define CMD_DRAM_INIT         0x68    // JEDEC Command 
#define CMD_WAKE              0xCA    // Wake Command 
#define CMD_SUSPEND           0xCC    // Suspend Command 

//
// Allowed vref gap between channels
//
#define VREF_THRESHOLD_GAP    5
//
// Layout definitions  of the elements array.
//
#define ALGO_REG_INDEX          0xc0                                // Starting index in the TYPE field for Signal Group Modules.
#define SGTI_INDEX              0xe0                                // Starting index for the Signal Group Type Index.
#define INDIV_ELEMENT_INDEX     (NUM_ALGOS * NUM_ELEMENTS_PER_ALGO) // Starting index of non-linear delay elements.

// 
// Index of individual elements supported by the Get/set
//
#define REG_UNDEFINED     0xFF

#define CMD_SIGNALTYPE          1
#define CMD_SIGNALGROUP         0

#define MIN_RANK_BIT                RANK_SHIFT_BIT
#define MAX_DUNIT_ROW_ADDRESS_BIT   (MIN_RANK_BIT - 1)
#define OFFSET_HIGH_LOW_SHIFT       (MAX_DUNIT_ROW_ADDRESS_BIT - 6 - 15)

#define PERCENT_MAX_MARGIN(x)     ((4*x)/10)

#define EW_PERCENTAGE(x)          ((6*x)/10)

#define EW_PERCENTAGE_VREF(x)     ((9*x)/10)
#define WEIGHT_CONDITION_VREF(x)  ((4*x)/10)

#define EW_PERCENTAGE_DELAY(x)    ((9*x)/10)
#define WEIGHT_CONDITION_DELAY(x) ((5*x)/10)

#define PASS_MARGIN_AXIS_X(x)     ((7*x)/10)
#define PASS_MARGIN_AXIS_Y(y)     ((1*y)/10)

typedef enum {
  ReceiveEnableStep,
  FineWriteLevelingStep,
  CoarseWriteLevelingStep,
  ReadVrefStep,
  ReadDelayStep,
  WriteDelayStep,
  WriteVrefStep,
  EarlyMprReadStep,
  CmdDelayStep
} MARGIN_TYPE;

// 
// Definition of the HALF/QTR/ONE Clock lengths.
// These are all frequency dependent.
//
#define HALF_CLK_(DigitalDllEn, FreqIndex)      (HalfClk[ADdll[DigitalDllEn]][FreqIndex])
#define QTR_CLK_(DigitalDllEn, FreqIndex)       (HalfClk[ADdll[DigitalDllEn]][FreqIndex] / 2)
#define ONE_CLK_(DigitalDllEn, FreqIndex)       (HalfClk[ADdll[DigitalDllEn]][FreqIndex] * 2)
#define MAXPI_VAL_(DigitalDllEn, FreqIndex)     (HalfClk[ADdll[DigitalDllEn]][FreqIndex] - 1)
// 
// This is the first byte in the assignment which provides the libraries the Type and amount of conditional values.
//
#define xxxxx                   0x00
#define xxxxT                   0x01
#define xxxCx                   0x02
#define xxxCT                   0x03
#define xxFxx                   0x04
#define xxFxT                   0x05
#define xxFCx                   0x06
#define xxFCT                   0x07
#define xPxxx                   0x08
#define xPxxT                   0x09
#define xPxCx                   0x0a
#define xPxCT                   0x0b
#define xPFxx                   0x0c
#define xPFxT                   0x0d
#define xPFCx                   0x0e
#define xPFCT                   0x0f
#define dxxxx                   0x80
#define dxxxT                   0x81
#define dxxCx                   0x82
#define dxxCT                   0x83
#define dxFxx                   0x84
#define dxFxT                   0x85
#define dxFCx                   0x86
#define dxFCT                   0x87
#define dPxxx                   0x88
#define dPxxT                   0x89
#define dPxCx                   0x8a
#define dPxCT                   0x8b
#define dPFxx                   0x8c
#define dPFxT                   0x8d
#define dPFCx                   0x8e
#define dPFCT                   0x8f

#define MILLI_DEL             0
#define MICRO_DEL             1
#define NANO_DEL              2

#define RD_REG                0x01
#define RD_ONLY               0x02
#define WR_OFF                0x04
#define FC_WR                 0x08
#define UPD_CACHE             0x10
#define FC_WR_PRINT           0x20
#define FC_PRINT              0x40

#define VLV_CPU 0x670
#define AVN_CPU 0x6D0

#define CPGC_MEMTEST_NUM 2
#define CPGC_LFSR_VICTIM_SEED       0xF294BA21 // Random seed for victim.
#define CPGC_LFSR_AGGRESSOR_SEED    0xEBA7492D // Random seed for aggressor.

#define AUTO_OPTION       15
#define DDR_CHANNELS_AUTO  0

typedef enum {
  Channel0Interposer = 0,
  Channel1Interposer
} CHANNEL_ENABLED_BY_INTERPOSER;

//
// Favor
//
#define DDR_FAVOR_DISABLED     0
#define DDR_FAVOR_PERFORMANCE  1
#define DDR_FAVOR_POWER_SAVING 2
#define DDR_FAVOR_CUSTOM       3

#endif // _MMRCPROJECTDEFINITIONS_H
