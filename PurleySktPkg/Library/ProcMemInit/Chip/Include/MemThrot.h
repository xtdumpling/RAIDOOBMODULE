//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//

/*************************************************************************
 *
 * Reference Code
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
 ************************************************************************
 *
 *  PURPOSE:
 *
 *      This file contains data structures for thermal parameters
 *
 ************************************************************************/
#ifndef _memthrot_h
#define _memthrot_h

#pragma pack(1)
// Base on Purley DDR4 Thermal Offset Table 112015-06.xls and
// Purley Platform Memory Subsystem OLTT CLTT BIOS MRC Rev. 1.0.pdf
//
// the tables below assume the following:
//
// DDR Type
//      DDR3 = 0
//      DDR4 = 1
//
// Type
//      RDIMM = 1
//      LRDIMM = 11d
//
// DRAM Density
//      1Gb = 2
//      2Gb = 3
//      4Gb = 4
//      8Gb = 5
//      16Gb = 6
//      32Gb = 7
//      64Gb = 8
//      128Gb = 9
//
//#Ranks and Width combined into 1 byte
// #Ranks
//      1R = 0
//      2R = 0x8
//      4R = 0x18
//      8R = 0x38
//
// Width
//      x4 = 0
//      x8 = 1
//
// Frequency
//      800 = 0 
//      1000 = 1
//      1066 = 2
//      1200 = 3
//      1333 = 4
//      1400 = 5
//      1600 = 6
//      1800 = 7
//      1866 = 8
//      2000 = 9
//      2133 = 0xA
//      2200 = 0xB
//      2400 = 0xC
//      2600 = 0xD
//      2666 = 0xE
//      2800 = 0xF
//      2933 = 0x10
//      3000 = 0x11
//      3200 = 0x12
//      3400 = 0x13
//      3466 = 0x14
//      3600 = 0x15
//      3733 = 0x16
//      3800 = 0x17
//      4000 = 0x18
//      4200 = 0x19
//
// Adjacent DIMM installed
//      y = 1, n = 0
//
// DPC (Maximum DIMMs per channel on the platform)
//      1, 2, 3
//
// Static (new offset 1.5m/s)
//
// Power [Actual value * 10]
//
//struct DimmTTRowEntryStruct {
//  UINT8  TTDDRType;
//  UINT8  TTDimmType;
//  UINT8  TTDramDensity;
//  UINT8  TTSpdByte7;
//  UINT8  TTFrequency;
//  UINT8  TTAdjDimmInstalled;
//  UINT8  TTDPC;
//  UINT8  TTStaticData;
//  UINT8  TTPower;
//};


#define MAX_TT_ROW 144

const struct DimmTTRowEntryStruct DimmTT[] = {
 {1,  1,  6, 0x1, 6, 0, 1, 7, 32},
 {1,  1,  6, 0x1, 8, 0, 1, 8, 35},
 {1,  1,  6, 0x1, 10, 0, 1, 9, 38},
 {1,  1,  6, 0x1, 12, 0, 1, 10, 41},
 {1,  1,  6, 0x1, 14, 0, 1, 11, 44},
 {1,  1,  6, 0x1, 16, 0, 1, 11, 47},
 {1,  1,  6, 0x1, 6, 1, 1, 7, 32},
 {1,  1,  6, 0x1, 8, 1, 1, 8, 35},
 {1,  1,  6, 0x1, 10, 1, 1, 9, 38},
 {1,  1,  6, 0x1, 12, 1, 1, 10, 41},
 {1,  1,  6, 0x1, 14, 1, 1, 11, 44},
 {1,  1,  6, 0x1, 16, 1, 1, 11, 47},
 {1,  1,  6, 0x1, 6, 1, 2, 4, 23},
 {1,  1,  6, 0x1, 8, 1, 2, 5, 24},
 {1,  1,  6, 0x1, 10, 1, 2, 5, 26},
 {1,  1,  6, 0x1, 12, 1, 2, 6, 28},
 {1,  1,  6, 0x1, 14, 1, 2, 6, 29},
 {1,  1,  6, 0x1, 16, 1, 2, 7, 31},
 {1,  1,  6, 0x9, 6, 0, 1, 7, 40},
 {1,  1,  6, 0x9, 8, 0, 1, 8, 43},
 {1,  1,  6, 0x9, 10, 0, 1, 9, 46},
 {1,  1,  6, 0x9, 12, 0, 1, 10, 49},
 {1,  1,  6, 0x9, 14, 0, 1, 11, 52},
 {1,  1,  6, 0x9, 16, 0, 1, 12, 56},
 {1,  1,  6, 0x9, 6, 1, 1, 7, 40},
 {1,  1,  6, 0x9, 8, 1, 1, 8, 43},
 {1,  1,  6, 0x9, 10, 1, 1, 9, 46},
 {1,  1,  6, 0x9, 12, 1, 1, 10, 49},
 {1,  1,  6, 0x9, 14, 1, 1, 11, 52},
 {1,  1,  6, 0x9, 16, 1, 1, 12, 56},
 {1,  1,  6, 0x9, 6, 1, 2, 4, 30},
 {1,  1,  6, 0x9, 8, 1, 2, 5, 32},
 {1,  1,  6, 0x9, 10, 1, 2, 6, 34},
 {1,  1,  6, 0x9, 12, 1, 2, 6, 35},
 {1,  1,  6, 0x9, 14, 1, 2, 6, 37},
 {1,  1,  6, 0x9, 16, 1, 2, 7, 39},
 {1,  1,  6, 0x0, 6, 0, 1, 7, 40},
 {1,  1,  6, 0x0, 8, 0, 1, 8, 43},
 {1,  1,  6, 0x0, 10, 0, 1, 9, 47},
 {1,  1,  6, 0x0, 12, 0, 1, 10, 50},
 {1,  1,  6, 0x0, 14, 0, 1, 11, 53},
 {1,  1,  6, 0x0, 16, 0, 1, 12, 56},
 {1,  1,  6, 0x0, 6, 1, 1, 7, 40},
 {1,  1,  6, 0x0, 8, 1, 1, 8, 43},
 {1,  1,  6, 0x0, 10, 1, 1, 9, 47},
 {1,  1,  6, 0x0, 12, 1, 1, 10, 50},
 {1,  1,  6, 0x0, 14, 1, 1, 11, 53},
 {1,  1,  6, 0x0, 16, 1, 1, 12, 56},
 {1,  1,  6, 0x0, 6, 1, 2, 5, 31},
 {1,  1,  6, 0x0, 8, 1, 2, 5, 33},
 {1,  1,  6, 0x0, 10, 1, 2, 6, 34},
 {1,  1,  6, 0x0, 12, 1, 2, 6, 36},
 {1,  1,  6, 0x0, 14, 1, 2, 7, 38},
 {1,  1,  6, 0x0, 16, 1, 2, 7, 40},
 {1,  1,  6, 0x8, 6, 0, 1, 3, 56},
 {1,  1,  6, 0x8, 8, 0, 1, 3, 60},
 {1,  1,  6, 0x8, 10, 0, 1, 3, 63},
 {1,  1,  6, 0x8, 12, 0, 1, 4, 66},
 {1,  1,  6, 0x8, 14, 0, 1, 4, 70},
 {1,  1,  6, 0x8, 16, 0, 1, 4, 73},
 {1,  1,  6, 0x8, 6, 1, 1, 3, 56},
 {1,  1,  6, 0x8, 8, 1, 1, 3, 60},
 {1,  1,  6, 0x8, 10, 1, 1, 3, 63},
 {1,  1,  6, 0x8, 12, 1, 1, 4, 66},
 {1,  1,  6, 0x8, 14, 1, 1, 4, 70},
 {1,  1,  6, 0x8, 16, 1, 1, 4, 73},
 {1,  1,  6, 0x8, 6, 1, 2, 2, 46},
 {1,  1,  6, 0x8, 8, 1, 2, 2, 48},
 {1,  1,  6, 0x8, 10, 1, 2, 2, 50},
 {1,  1,  6, 0x8, 12, 1, 2, 2, 52},
 {1,  1,  6, 0x8, 14, 1, 2, 3, 54},
 {1,  1,  6, 0x8, 16, 1, 2, 3, 55},
 {1,  0xB,  6, 0x18, 6, 0, 1, 12, 99},
 {1,  0xB,  6, 0x18, 8, 0, 1, 13, 104},
 {1,  0xB,  6, 0x18, 10, 0, 1, 14, 109},
 {1,  0xB,  6, 0x18, 12, 0, 1, 15, 114},
 {1,  0xB,  6, 0x18, 14, 0, 1, 15, 119},
 {1,  0xB,  6, 0x18, 16, 0, 1, 15, 124},
 {1,  0xB,  6, 0x18, 6, 1, 1, 12, 99},
 {1,  0xB,  6, 0x18, 8, 1, 1, 13, 104},
 {1,  0xB,  6, 0x18, 10, 1, 1, 14, 109},
 {1,  0xB,  6, 0x18, 12, 1, 1, 15, 114},
 {1,  0xB,  6, 0x18, 14, 1, 1, 15, 119},
 {1,  0xB,  6, 0x18, 16, 1, 1, 15, 124},
 {1,  0xB,  6, 0x18, 6, 1, 2, 9, 85},
 {1,  0xB,  6, 0x18, 8, 1, 2, 9, 88},
 {1,  0xB,  6, 0x18, 10, 1, 2, 10, 90},
 {1,  0xB,  6, 0x18, 12, 1, 2, 10, 93},
 {1,  0xB,  6, 0x18, 14, 1, 2, 11, 96},
 {1,  0xB,  6, 0x18, 16, 1, 2, 12, 99},
 {1,  0xB,  6, 0x18, 6, 0, 1, 0, 99},
 {1,  0xB,  6, 0x18, 8, 0, 1, 0, 104},
 {1,  0xB,  6, 0x18, 10, 0, 1, 0, 109},
 {1,  0xB,  6, 0x18, 12, 0, 1, 0, 114},
 {1,  0xB,  6, 0x18, 14, 0, 1, 0, 119},
 {1,  0xB,  6, 0x18, 16, 0, 1, 0, 124},
 {1,  0xB,  6, 0x18, 6, 1, 1, 0, 99},
 {1,  0xB,  6, 0x18, 8, 1, 1, 0, 104},
 {1,  0xB,  6, 0x18, 10, 1, 1, 0, 109},
 {1,  0xB,  6, 0x18, 12, 1, 1, 0, 114},
 {1,  0xB,  6, 0x18, 14, 1, 1, 0, 119},
 {1,  0xB,  6, 0x18, 16, 1, 1, 0, 124},
 {1,  0xB,  6, 0x18, 6, 1, 2, 0, 85},
 {1,  0xB,  6, 0x18, 8, 1, 2, 0, 88},
 {1,  0xB,  6, 0x18, 10, 1, 2, 0, 90},
 {1,  0xB,  6, 0x18, 12, 1, 2, 0, 93},
 {1,  0xB,  6, 0x18, 14, 1, 2, 0, 96},
 {1,  0xB,  6, 0x18, 16, 1, 2, 0, 99},
 {1,  0xB,  6, 0x38, 6, 0, 1, 15, 140},
 {1,  0xB,  6, 0x38, 8, 0, 1, 15, 145},
 {1,  0xB,  6, 0x38, 10, 0, 1, 15, 149},
 {1,  0xB,  6, 0x38, 12, 0, 1, 15, 154},
 {1,  0xB,  6, 0x38, 14, 0, 1, 15, 159},
 {1,  0xB,  6, 0x38, 16, 0, 1, 15, 163},
 {1,  0xB,  6, 0x38, 6, 1, 1, 15, 140},
 {1,  0xB,  6, 0x38, 8, 1, 1, 15, 145},
 {1,  0xB,  6, 0x38, 10, 1, 1, 15, 149},
 {1,  0xB,  6, 0x38, 12, 1, 1, 15, 154},
 {1,  0xB,  6, 0x38, 14, 1, 1, 15, 159},
 {1,  0xB,  6, 0x38, 16, 1, 1, 15, 163},
 {1,  0xB,  6, 0x38, 6, 1, 2, 15, 127},
 {1,  0xB,  6, 0x38, 8, 1, 2, 15, 129},
 {1,  0xB,  6, 0x38, 10, 1, 2, 15, 132},
 {1,  0xB,  6, 0x38, 12, 1, 2, 15, 135},
 {1,  0xB,  6, 0x38, 14, 1, 2, 15, 137},
 {1,  0xB,  6, 0x38, 16, 1, 2, 15, 140},
 {1,  0xB,  6, 0x38, 6, 0, 1, 0, 140},
 {1,  0xB,  6, 0x38, 8, 0, 1, 0, 145},
 {1,  0xB,  6, 0x38, 10, 0, 1, 0, 149},
 {1,  0xB,  6, 0x38, 12, 0, 1, 0, 154},
 {1,  0xB,  6, 0x38, 14, 0, 1, 0, 159},
 {1,  0xB,  6, 0x38, 16, 0, 1, 0, 163},
 {1,  0xB,  6, 0x38, 6, 1, 1, 0, 140},
 {1,  0xB,  6, 0x38, 8, 1, 1, 0, 145},
 {1,  0xB,  6, 0x38, 10, 1, 1, 0, 149},
 {1,  0xB,  6, 0x38, 12, 1, 1, 0, 154},
 {1,  0xB,  6, 0x38, 14, 1, 1, 0, 159},
 {1,  0xB,  6, 0x38, 16, 1, 1, 0, 163},
 {1,  0xB,  6, 0x38, 6, 1, 2, 0, 127},
 {1,  0xB,  6, 0x38, 8, 1, 2, 0, 129},
 {1,  0xB,  6, 0x38, 10, 1, 2, 0, 132},
 {1,  0xB,  6, 0x38, 12, 1, 2, 0, 135},
 {1,  0xB,  6, 0x38, 14, 1, 2, 0, 137},
 {1,  0xB,  6, 0x38, 16, 1, 2, 0, 140}
};

#define MAX_TTM_ROW 54

//
// Based on Platform_OLTT/CLTT_Implementation_Whitepaper(563306) Revision 1.0 (NOV 2015)
// the DimmTThrottlingStruct table below assumes the following:
//
// TTMode = Throttling Mode
//      OLTT = 0
//      CLTT = 1
//
// TTDIMMPC = Num of DIMM Per Channel
//      1DPC = 1
//      2DPC = 2
//      3DPC = 3
//
// TTFREQ = Frequency
//      800 = 0
//      1066 = 2
//      1333 = 4
//      1600 = 6
//      1866 = 8
//      2133 = 0xA
//      2400 = 0xC
//      2666 = 0xE
//      2933 = 0x10
//
// TTWINDOWS = Windows (us)
//      1us = 1
//      2us = 2
//      ...
//
// TTBW_THROTTLE_TF = BW_THROTTLE_TF (hex)
//
// TTTHRT_HI = THRT_HI (hex)
//
// TTBWPercent = Throttle BW (per DIMM)
//      [value is ??% * 10]
//
struct DimmTThrottlingStruct {
  UINT8   TTMode;
  UINT8   TTDIMMPC;
  UINT8   TTFREQ;
  UINT8   TTWINDOWS;
  UINT16  TTBW_THROTTLE_TF;
  UINT8   TTTHRT_HI;
  UINT16  TTBWPercent;
};

const struct DimmTThrottlingStruct DimmTTMODE[] = { //OLTT(0) and CLTT(1) values Performance Values ddr4/ddr3
//OLTT 1DPC
  { 0, 1, 0, 1, 0x32, 0x5, 800 },
  { 0, 1, 2, 1, 0x43, 0x7, 836 },
  { 0, 1, 4, 1, 0x53, 0x8, 771 },
  { 0, 1, 6, 1, 0x64, 0x10, 800 },
  { 0, 1, 8, 1, 0x75, 0x12, 821 },
  { 0, 1, 0xA, 1, 0x85, 0x13, 782 },
  { 0, 1, 0xC, 1, 0x96, 0x15, 800 },
  { 0, 1, 0xE, 1, 0xA7, 0x17, 814 },
  { 0, 1, 0x10, 1, 0xB7, 0x18, 787 },
//OLTT 2DPC
  { 0, 2, 0, 2, 0x64, 0x5, 400 },
  { 0, 2, 2, 2, 0x85, 0x7, 421 },
  { 0, 2, 4, 2, 0xA7, 0x8, 383 },
  { 0, 2, 6, 2, 0xC8, 0x10, 400 },
  { 0, 2, 8, 2, 0xE9, 0x12, 412 },
  { 0, 2, 0xA, 2, 0x10B, 0x13, 390 },
  { 0, 2, 0xC, 2, 0x12C, 0x15, 400 },
  { 0, 2, 0xE, 2, 0x14D, 0x17, 408 },
  { 0, 2, 0x10, 2, 0x16F, 0x18, 392 },
//OLTT 3DPC
  { 0, 3, 0, 3, 0x96, 0x5, 267 },
  { 0, 3, 2, 3, 0xC8, 0x7, 280 },
  { 0, 3, 4, 3, 0xFA, 0x8, 256 },
  { 0, 3, 6, 3, 0x12C, 0x10, 267 },
  { 0, 3, 8, 3, 0x15E, 0x12, 274 },
  { 0, 3, 0xA, 3, 0x190, 0x13, 260 },
  { 0, 3, 0xC, 3, 0x1C2, 0x15, 267 },
  { 0, 3, 0xE, 3, 0x1F4, 0x17, 272 },
  { 0, 3, 0x10, 3, 0x226, 0x18, 262 },
//CLTT 1DPC
  { 1, 1, 0, 3, 0x96, 0x2, 107 },
  { 1, 1, 2, 4, 0x10B, 0x3, 90 },
  { 1, 1, 4, 3, 0xFA, 0x3, 96 },
  { 1, 1, 6, 3, 0x12C, 0x4, 107 },
  { 1, 1, 8, 4, 0x1D3, 0x6, 103 },
  { 1, 1, 0xA, 4, 0x215, 0x7, 105 },
  { 1, 1, 0xC, 4, 0x258, 0x8, 107 },
  { 1, 1, 0xE, 3, 0x1F4, 0x6, 96 },
  { 1, 1, 0x10, 3, 0x226, 0x7, 102 },
//CLTT 2DPC
  { 1, 2, 0, 3, 0x96, 0x1, 53 },
  { 1, 2, 2, 4, 0x10B, 0x2, 60 },
  { 1, 2, 4, 4, 0x14D, 0x2, 48 },
  { 1, 2, 6, 3, 0x12C, 0x2, 53 },
  { 1, 2, 8, 4, 0x1D3, 0x3, 51 },
  { 1, 2, 0xA, 4, 0x215, 0x3, 45 },
  { 1, 2, 0xC, 4, 0x258, 0x4, 53 },
  { 1, 2, 0xE, 4, 0x29B, 0x4, 48 },
  { 1, 2, 0x10, 4, 0x2DD, 0x5, 55 },
//CLTT 3DPC
  { 1, 3, 0, 4, 0xC8, 0x1, 40 },
  { 1, 3, 2, 3, 0xC8, 0x1, 40 },
  { 1, 3, 4, 3, 0xFA, 0x1, 32 },
  { 1, 3, 6, 4, 0x190, 0x2, 40 },
  { 1, 3, 8, 4, 0x1D3, 0x2, 34 },
  { 1, 3, 0xA, 4, 0x215, 0x2, 30 },
  { 1, 3, 0xC, 3, 0x1C2, 0x2, 36 },
  { 1, 3, 0xE, 3, 0x1F4, 0x2, 32 },
  { 1, 3, 0x10, 4, 0x2DD, 0x3, 33 },
 };

#pragma pack()
#endif  // _memThrot_h
