/**

@copyright
  Copyright (c) 2015 Intel Corporation. All rights reserved.
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

@file
  MrcCommonTypes.h

@brief
  This file contains the definitions common to the MRC API and other APIs.

**/
#ifndef _MrcCommonTypes_h_
#define _MrcCommonTypes_h_

#include "DataTypes.h"

#define INT32_MIN                       (0x80000000)
#ifndef INT32_MAX  //INT32_MAX->Already defined
#define INT32_MAX                       (0x7FFFFFFF)
#endif
#define INT16_MIN                       (0x8000)
#define INT16_MAX                       (0x7FFF)

///
/// System boot mode.
///
typedef enum {
  bmCold,                                 ///< Cold boot
  bmWarm,                                 ///< Warm boot
  bmS3,                                   ///< S3 resume
  bmFast,                                 ///< Fast boot
  MrcBootModeMax,                         ///< MRC_BOOT_MODE enumeration maximum value.
  MrcBootModeDelim = INT32_MAX            ///< This value ensures the enum size is consistent on both sides of the PPI.
} MrcBootMode;

///
/// DIMM memory package
///
typedef enum {
  RDimmMemoryPackage      = 1,
  UDimmMemoryPackage      = 2,
  SoDimmMemoryPackage     = 3,
  MicroDimmMemoryPackage  = 4,
  MiniRDimmMemoryPackage  = 5,
  MiniUDimmMemoryPackage  = 6,
  MiniCDimmMemoryPackage  = 7,
  SoUDimmEccMemoryPackage = 8,
  SoRDimmEccMemoryPackage = 9,
  SoCDimmEccMemoryPackage = 10,
  LrDimmMemoryPackage     = 11,
  SoDimm16bMemoryPackage  = 12,
  SoDimm32bMemoryPackage  = 13,
  MemoryPackageMax,                       ///< MEMORY_PACKAGE enumeration maximum value.
  MemoryPackageDelim = INT32_MAX          ///< This value ensures the enum size is consistent on both sides of the PPI.
} MEMORY_PACKAGE;

///
/// Memory training I/O levels.
///
typedef enum {
  DdrLevel   = 0,                         ///< Refers to frontside of DIMM
  LrbufLevel = 1,                         ///< Refers to data level at backside of LRDIMM or NVMDIMM buffer
  RegALevel  = 2,                         ///< Refers to cmd level at backside of register, side A
  RegBLevel  = 3,                         ///< Refers to cmd level at backside of register, side B
  HbmLevel   = 4,                         ///< Refers to HBM
  GsmLtMax,                               ///< GSM_LT enumeration maximum value.
  GsmLtDelim = INT32_MAX                  ///< This value ensures the enum size is consistent on both sides of the PPI.
} GSM_LT;

///
/// Memory training margin group selectors.
///
typedef enum {
  RecEnDelay       = 0,                   ///< Linear delay (PI ticks), where the positive increment moves the RCVEN sampling window later in time relative to the RX DQS strobes.
  RxDqsDelay       = 1,                   ///< Linear delay (PI ticks), where the positive increment moves the RX DQS strobe later in time relative to the RX DQ signal (i.e. toward the hold side of the eye).
  RxDqDelay        = 2,                   ///< Linear delay (PI ticks), where the positive increment moves the RX DQ byte/nibble/bitlane later in time relative to the RX DQS signal (i.e.closing the gap between DQ and DQS in the setup side of the eye).
  RxDqsPDelay      = 3,                   ///< Linear delay (PI ticks), where the positive increment moves the RX DQS strobe for "even" chunks later in time relative to the RX DQ signal. Even chunks are 0, 2, 4, 6 within the 0 to 7 chunks of an 8 burst length cacheline, for example.
  RxDqsNDelay      = 4,                   ///< Linear delay (PI ticks), where the positive increment moves the RX DQS strobe for "odd" chunks later in time relative to the RX DQ signal. Odd chunks are 1, 3, 5, 7 within the 0 to 7 chunks of an 8 burst length cacheline, for example.
  RxVref           = 5,                   ///< Linear increment (Vref ticks), where the positive increment moves the byte/nibble/bitlane RX Vref to a higher voltage.
  RxEq             = 6,                   ///< RX CTLE setting indicating a set of possible resistances, capacitance, current steering, etc. values, which may be a different set of values per product. The setting combinations are indexed by integer values.
  RxDqBitDelay     = 7,                   ///< Linear delay (PI ticks), where the positive increment moves the RX DQ bitlane later in time relative to the RX DQS signal (i.e.closing the gap between DQ and DQS in the setup side of the eye).
  RxVoc            = 8,                   ///< Monotonic increment (Sense Amp setting), where the positive increment moves the byte/nibble/bitlane's effective switching point to a lower Vref value.
  RxOdt            = 9,                   ///< Resistance setting within a set of possible resistances, which may be a different set of values per product. Indexed by integer values.
  RxOdtUp          = 10,                  ///< Resistance setting within a set of possible resistances, which may be a different set of values per product. Indexed by integer values.
  RxOdtDn          = 11,                  ///< Resistance setting within a set of possible resistances, which may be a different set of values per product. Indexed by integer values.
  DramDrvStr       = 12,                  ///< Drive strength setting resistance setting within a set of possible resistances (or currents), which may be a different set of values per product. Indexed by integer values.
  McOdtDelay       = 13,                  ///<
  McOdtDuration    = 14,                  ///<
  SenseAmpDelay    = 15,                  ///<
  SenseAmpDuration = 16,                  ///<
  RoundTripDelay   = 17,                  ///< Relative time from CMD to data valid in RX FIFO, in increments of 1 DQ UI (logic delays).
  RxDqsBitDelay    = 18,                  ///< Linear delay (PI ticks), where the positive increment moves the RX DQS within the bitlane later in time relative to the RX DQ signal (i.e.closing the gap between DQ and DQS in the hold side of the eye).
  RxDqDqsDelay     = 19,                  ///< Linear delay (PI ticks), where the positive increment moves the RX DQS per strobe later in time relative to the RX DQ signal (i.e. closing the gap between DQS and DQ in the hold side of the eye. The difference between this parameter and RxDqsDelay is that both the DQ and DQS timings may be moved in order to increase the total range of DQDQS timings.

  WrLvlDelay       = 20,                  ///< Linear delay (PI ticks), where the positive increment moves both the TX DQS and TX DQ signals later in time relative to all other bus signals.
  TxDqsDelay       = 21,                  ///< Linear delay (PI ticks), where the positive increment moves the TX DQS strobe later in time relative to all other bus signals.
  TxDqDelay        = 22,                  ///< Linear delay (PI ticks), where the positive increment moves the TX DQ byte/nibble/bitlane later in time relative to all other bus signals.
  TxVref           = 23,                  ///< Linear increment (Vref ticks), where the positive increment moves the byte/nibble/bitlane TX Vref to a higher voltage. (Assuming this will abstract away from the range specifics for DDR4, for example.)
  TxEq             = 24,                  ///< TX EQ setting indicating a set of possible equalization levels, which may be a different set of values per product. The setting combinations are indexed by integer values.
  TxDqBitDelay     = 25,                  ///< Linear delay (PI ticks), where the positive increment moves the TX DQ bitlane later in time relative to all other bus signals.
  TxRon            = 26,                  ///< Resistance setting within a set of possible resistances, which may be a different set of values per product. Indexed by integer values.
  TxRonUp          = 27,                  ///< Resistance setting within a set of possible resistances, which may be a different set of values per product. Indexed by integer values.
  TxRonDn          = 28,                  ///< Resistance setting within a set of possible resistances, which may be a different set of values per product. Indexed by integer values.
  TxSlewRate       = 29,                  ///< Monotonic increment, where the positive increment moves the byte/nibble/bitlane's effective slew rate to a higher slope.
  TxImode          = 30,                  ///< TX I-Mode Boost setting indicating a set of possible current boost levels, which may be a different set of values per product. The setting combinations are indexed by integer values.
  WrOdt            = 31,                  ///< Resistance setting within a set of possible resistances, which may be a different set of values per product. Indexed by integer values.
  NomOdt           = 32,                  ///< Resistance setting within a set of possible resistances, which may be a different set of values per product. Indexed by integer values.
  ParkOdt          = 33,                  ///< Resistance setting within a set of possible resistances, which may be a different set of values per product. Indexed by integer values.
  TxTco            = 34,                  ///<

  RxCtleR          = 36,                  ///< RX CTLE setting indicating a set of possible resistance values, which may be a different set of values per product. The setting combinations are indexed by integer values.
  RxCtleC          = 37,                  ///< RX CTLE setting indicating a set of possible capacitance values, which may be a different set of values per product. The setting combinations are indexed by integer values.
  RxDqsPBitDelay   = 38,                  ///< Linear delay (PI ticks), where the positive increment moves the RX DQS bitlane timing for "even" chunks later in time relative to the RX DQ bitlane signal. Even chunks are 0, 2, 4, 6 within the 0 to 7 chunks of an 8 burst length cacheline, for example.
  RxDqsNBitDelay   = 39,                  ///< Linear delay (PI ticks), where the positive increment moves the RX DQS bitlane timing for "odd" chunks later in time relative to the RX DQ bitlane signal. Odd chunks are 1, 3, 5, 7 within the 0 to 7 chunks of an 8 burst length cacheline, for example.
  CmdAll           = 40,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdAll category later in time relative to all other signals on the bus.

  CtlAll           = 44,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CTL_ALL category later in time relative to all other signals on the bus.
  CtlGrp0          = 45,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CTL_GRP0 category later in time relative to all other signals on the bus.
  CtlGrp1          = 46,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CTL_GRP1 category later in time relative to all other signals on the bus.
  CtlGrp2          = 47,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CTL_GRP2 category later in time relative to all other signals on the bus.
  CtlGrp3          = 48,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CTL_GRP3 category later in time relative to all other signals on the bus.
  CtlGrp4          = 49,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CTL_GRP4 category later in time relative to all other signals on the bus.
  CtlGrp5          = 50,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CTL_GRP5 category later in time relative to all other signals on the bus.
  CmdCtlAll        = 51,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CMD_CTL_ALL category later in time relative to all other signals on the bus.
  CkAll            = 52,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CK_ALL category later in time relative to all other signals on the bus.
  CmdVref          = 53,                  ///< Linear increment (Vref ticks), where the positive increment moves the CMD Vref to a higher voltage.
  AlertVref        = 54,                  ///< Linear increment (Vref ticks), where the positive increment moves the ALERT Vref to a higher voltage.
  CmdRon           = 55,                  ///< Resistance setting within a set of possible resistances, which may be a different set of values per product. Indexed by integer values.

  EridDelay        = 60,                  ///< Linear delay (PI ticks), where the positive increment moves the ERID signals later in time relative to the internal sampling clock (i.e.closing the gap between ERID and internal sampling clock in the setup side of the eye). This group is applicable for DDRT DIMMs.
  EridVref         = 61,                  ///< Linear increment (Vref ticks), where the positive increment moves the ERID Vref to a higher voltage. This group is applicable for DDRT DIMMs.
  ErrorVref        = 62,                  ///< Linear increment (Vref ticks), where the positive increment moves the ERROR Vref to a higher voltage. This group is applicable for DDRT DIMMs.
  ReqVref          = 63,                  ///< Linear increment (Vref ticks), where the positive increment moves the REQ Vref to a higher voltage. This group is applicable for DDRT DIMMs.

  RecEnDelayCycle  = 67,
  TxDqsDelayCycle  = 68,

  CmdGrp0          = 71,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp0 category later in time relative to all other signals on the bus.
  CmdGrp1          = 72,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp1 category later in time relative to all other signals on the bus.
  CmdGrp2          = 73,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp3          = 74,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp4          = 75,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp5          = 76,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp6          = 77,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp7          = 78,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp8          = 79,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp9          = 80,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp10         = 81,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp11         = 82,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp12         = 83,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp13         = 84,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp14         = 85,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp15         = 86,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp16         = 87,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp17         = 88,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp18         = 89,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp19         = 90,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
  CmdGrp20         = 91,                  ///< Linear delay (PI ticks), where the positive increment moves all signals assigned to the CmdGrp2 category later in time relative to all other signals on the bus.
 // HBM
  TxDsPu           = 92,                  ///< Drive strength setting for pull up legs that are turned on.
  TxDsPd           = 93,                  ///< Drive strength setting for pull down legs that are turned on.
  CmdDsPu          = 94,                  ///< Drive strength setting for pull up legs that are turned on.
  CmdDsPd          = 95,                  ///< Drive strength setting for pull down legs that are turned on.
  CmdTco           = 96,                  ///< TX Duty Cycle correction
  CkTco            = 97,                  ///< TX Duty Cycle correction
  GsmGtMax,                               ///< SSA_GSM_GT enumeration maximum value.
  GsmGtDelim = INT32_MAX                  ///< This value ensures the enum size is consistent on both sides of the PPI.
} GSM_GT;

typedef struct {
  UINT8   odtRdLeading;
  UINT8   odtRdTrailing;
  UINT8   odtWrLeading;
  UINT8   odtWrTrailing;
  } ODT_TIMING, *PODT_TIMING;

#endif // _MrcCommonTypes_h_
