/*++
Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  CmdCtlDelay.h

Abstract:

  Data structures for thermal and power parameters for platform(s).

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

              DO NOT MODIFY - TABLE IS AUTO-GENERATED

        This table is generated with CreateCmdCtlDelay.py

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

--*/

#ifndef _CmdCtlDelay_h
#define _CmdCtlDelay_h

#ifdef SKXD_EN
const struct IoGroupDelayStruct igCtl[] = {

   {0,  3,  SIDE_A,  {   3,   3,   3, 0xFF }},     //CS0_N, ODT0
   {1,  3,  SIDE_A,  {   0,   0,   0, 0xFF }},     //CS0_N, ODT0
   {3,  3,  SIDE_A,  {  29,  29,  29, 0xFF }},     //CS0_N, ODT0
   {4,  3,  SIDE_A,  {  46,  46,  46, 0xFF }},     //CS0_N, ODT0
   {0, 12,  SIDE_A,  {  24,  24,  24, 0xFF }},     //CKE0
   {1, 12,  SIDE_A,  {  23,  23,  23, 0xFF }},     //CKE0
   {3, 12,  SIDE_A,  {  19,  19,  19, 0xFF }},     //CKE0
   {4, 12,  SIDE_A,  {  64,  64,  64, 0xFF }},     //CKE0
   {0,  5,  SIDE_A,  {   0,   0,   0, 0xFF }},     //CS2_N
   {1,  5,  SIDE_A,  {  17,  17,  17, 0xFF }},     //CS2_N
   {3,  5,  SIDE_A,  {  19,  19,  19, 0xFF }},     //CS2_N
   {4,  5,  SIDE_A,  {  33,  33,  33, 0xFF }},     //CS2_N
   {0,  6,  SIDE_A,  {  13,  13,  13, 0xFF }},     //CS3_N
   {1,  6,  SIDE_A,  {  25,  25,  25, 0xFF }},     //CS3_N
   {3,  6,  SIDE_A,  {  23,  23,  23, 0xFF }},     //CS3_N
   {4,  6,  SIDE_A,  {  47,  47,  47, 0xFF }},     //CS3_N
   {0,  4,  SIDE_A,  {   8,   8,   8, 0xFF }},     //CS1_N, ODT1
   {1,  4,  SIDE_A,  {  24,  24,  24, 0xFF }},     //CS1_N, ODT1
   {3,  4,  SIDE_A,  {  29,  29,  29, 0xFF }},     //CS1_N, ODT1
   {4,  4,  SIDE_A,  {  31,  31,  31, 0xFF }},     //CS1_N, ODT1
   {0, 13,  SIDE_A,  {  34,  34,  34, 0xFF }},     //CKE1
   {1, 13,  SIDE_A,  {  44,  44,  44, 0xFF }},     //CKE1
   {3, 13,  SIDE_A,  {  25,  25,  25, 0xFF }},     //CKE1
   {4, 13,  SIDE_A,  {  70,  70,  70, 0xFF }},     //CKE1
   {0,  4,  SIDE_B,  {  11,  11,  11, 0xFF }},     //CS4_N, ODT2
   {1,  4,  SIDE_B,  {  27,  27,  27, 0xFF }},     //CS4_N, ODT2
   {3,  4,  SIDE_B,  {  19,  19,  19, 0xFF }},     //CS4_N, ODT2
   {4,  4,  SIDE_B,  {  51,  51,  51, 0xFF }},     //CS4_N, ODT2
   {0, 11,  SIDE_A,  {  35,  35,  35, 0xFF }},     //CKE2
   {1, 11,  SIDE_A,  {  25,  25,  25, 0xFF }},     //CKE2
   {3, 11,  SIDE_A,  {  24,  24,  24, 0xFF }},     //CKE2
   {4, 11,  SIDE_A,  {  55,  55,  55, 0xFF }},     //CKE2
   {0,  2,  SIDE_A,  {   3,   3,   3, 0xFF }},     //CS6_N, CS7_N
   {1,  2,  SIDE_A,  {   5,   5,   5, 0xFF }},     //CS6_N, CS7_N
   {3,  2,  SIDE_A,  {  11,  11,  11, 0xFF }},     //CS6_N, CS7_N
   {4,  2,  SIDE_A,  {   9,   9,   9, 0xFF }},     //CS6_N, CS7_N
   {0,  0,  SIDE_A,  {   3,   3,   3, 0xFF }},     //CS5_N, ODT3
   {1,  0,  SIDE_A,  {   6,   6,   6, 0xFF }},     //CS5_N, ODT3
   {3,  0,  SIDE_A,  {  12,  12,  12, 0xFF }},     //CS5_N, ODT3
   {4,  0,  SIDE_A,  {  46,  46,  46, 0xFF }},     //CS5_N, ODT3
   {0, 10,  SIDE_A,  {  28,  28,  28, 0xFF }},     //CKE3
   {1, 10,  SIDE_A,  {  46,  46,  46, 0xFF }},     //CKE3
   {3, 10,  SIDE_A,  {  22,  22,  22, 0xFF }},     //CKE3
   {4, 10,  SIDE_A,  {  55,  55,  55, 0xFF }},     //CKE3
};


struct IoGroupClkDelayStruct {
  UINT8   ch;                         // Channel
  UINT8   clk;                        // Clock number (4 clocks per channel)
  UINT16   delay[SKX_PCKG_TYPE];       // Delay in Pico Seconds SKX 28core/?/?
};

const struct IoGroupClkDelayStruct igClk[] = {

   {0,  0, { 19,  19,  19, 0xFF}},
   {1,  0, { 22,  22,  22, 0xFF}},
   {3,  0, {  8,   8,   8, 0xFF}},
   {4,  0, { 52,  52,  52, 0xFF}},
   {0,  1, { 15,  15,  15, 0xFF}},
   {1,  1, { 30,  30,  30, 0xFF}},
   {3,  1, {  0,   0,   0, 0xFF}},
   {4,  1, { 45,  45,  45, 0xFF}},
   {0,  2, { 14,  14,  14, 0xFF}},
   {1,  2, { 23,  23,  23, 0xFF}},
   {3,  2, { 23,  23,  23, 0xFF}},
   {4,  2, { 31,  31,  31, 0xFF}},
   {0,  3, { 23,  23,  23, 0xFF}},
   {1,  3, { 32,  32,  32, 0xFF}},
   {3,  3, { 14,  14,  14, 0xFF}},
   {4,  3, { 65,  65,  65, 0xFF}},
};


const struct IoGroupDelayStruct igCmd[] = {

   {0,  1,  SIDE_A,  {  11,  11,  11, 0xFF }},     //A17, C2
   {1,  1,  SIDE_A,  {  17,  17,  17, 0xFF }},     //A17, C2
   {3,  1,  SIDE_A,  {  16,  16,  16, 0xFF }},     //A17, C2
   {4,  1,  SIDE_A,  {   0,   0,   0, 0xFF }},     //A17, C2
   {0,  3,  SIDE_B,  {  12,  12,  12, 0xFF }},     //A13, A15
   {1,  3,  SIDE_B,  {  13,  13,  13, 0xFF }},     //A13, A15
   {3,  3,  SIDE_B,  {  19,  19,  19, 0xFF }},     //A13, A15
   {4,  3,  SIDE_B,  {  21,  21,  21, 0xFF }},     //A13, A15
   {0,  2,  SIDE_B,  {  17,  17,  17, 0xFF }},     //A14, A16
   {1,  2,  SIDE_B,  {  12,  12,  12, 0xFF }},     //A14, A16
   {3,  2,  SIDE_B,  {  21,  21,  21, 0xFF }},     //A14, A16
   {4,  2,  SIDE_B,  {  14,  14,  14, 0xFF }},     //A14, A16
   {0,  1,  SIDE_B,  {  26,  26,  26, 0xFF }},     //BA1, A10
   {1,  1,  SIDE_B,  {  33,  33,  33, 0xFF }},     //BA1, A10
   {3,  1,  SIDE_B,  {  22,  22,  22, 0xFF }},     //BA1, A10
   {4,  1,  SIDE_B,  {  23,  23,  23, 0xFF }},     //BA1, A10
   {0,  0,  SIDE_B,  {  19,  19,  19, 0xFF }},     //A0, BA0
   {1,  0,  SIDE_B,  {  37,  37,  37, 0xFF }},     //A0, BA0
   {3,  0,  SIDE_B,  {  21,  21,  21, 0xFF }},     //A0, BA0
   {4,  0,  SIDE_B,  {   8,   8,   8, 0xFF }},     //A0, BA0
   {0,  7,  SIDE_B,  {  15,  15,  15, 0xFF }},     //PAR
   {1,  7,  SIDE_B,  {  44,  44,  44, 0xFF }},     //PAR
   {3,  7,  SIDE_B,  {   8,   8,   8, 0xFF }},     //PAR
   {4,  7,  SIDE_B,  {  55,  55,  55, 0xFF }},     //PAR
   {0,  8,  SIDE_B,  {  19,  19,  19, 0xFF }},     //A1, A3
   {1,  8,  SIDE_B,  {  33,  33,  33, 0xFF }},     //A1, A3
   {3,  8,  SIDE_B,  {  19,  19,  19, 0xFF }},     //A1, A3
   {4,  8,  SIDE_B,  {  57,  57,  57, 0xFF }},     //A1, A3
   {0,  9,  SIDE_B,  {  20,  20,  20, 0xFF }},     //A2, A4
   {1,  9,  SIDE_B,  {  52,  52,  52, 0xFF }},     //A2, A4
   {3,  9,  SIDE_B,  {  15,  15,  15, 0xFF }},     //A2, A4
   {4,  9,  SIDE_B,  {  52,  52,  52, 0xFF }},     //A2, A4
   {0, 10,  SIDE_B,  {  17,  17,  17, 0xFF }},     //A5, A6
   {1, 10,  SIDE_B,  {  34,  34,  34, 0xFF }},     //A5, A6
   {3, 10,  SIDE_B,  {  16,  16,  16, 0xFF }},     //A5, A6
   {4, 10,  SIDE_B,  {  52,  52,  52, 0xFF }},     //A5, A6
   {0, 11,  SIDE_B,  {  24,  24,  24, 0xFF }},     //A7, A8
   {1, 11,  SIDE_B,  {  39,  39,  39, 0xFF }},     //A7, A8
   {3, 11,  SIDE_B,  {  15,  15,  15, 0xFF }},     //A7, A8
   {4, 11,  SIDE_B,  {  58,  58,  58, 0xFF }},     //A7, A8
   {0,  7,  SIDE_A,  {  19,  19,  19, 0xFF }},     //A9, A12
   {1,  7,  SIDE_A,  {  31,  31,  31, 0xFF }},     //A9, A12
   {3,  7,  SIDE_A,  {  13,  13,  13, 0xFF }},     //A9, A12
   {4,  7,  SIDE_A,  {  58,  58,  58, 0xFF }},     //A9, A12
   {0,  8,  SIDE_A,  {  25,  25,  25, 0xFF }},     //BG1, A11
   {1,  8,  SIDE_A,  {  31,  31,  31, 0xFF }},     //BG1, A11
   {3,  8,  SIDE_A,  {  20,  20,  20, 0xFF }},     //BG1, A11
   {4,  8,  SIDE_A,  {  61,  61,  61, 0xFF }},     //BG1, A11
   {0,  9,  SIDE_A,  {  32,  32,  32, 0xFF }},     //BG0, ACT_N
   {1,  9,  SIDE_A,  {  41,  41,  41, 0xFF }},     //BG0, ACT_N
   {3,  9,  SIDE_A,  {  23,  23,  23, 0xFF }},     //BG0, ACT_N
   {4,  9,  SIDE_A,  {  48,  48,  48, 0xFF }},     //BG0, ACT_N
};
#else
const struct IoGroupDelayStruct igCtl[] = {

   {0,  3,  SIDE_A,  {  53,  73,  75, 0xFF }},     //CS0_N, ODT0
   {1,  3,  SIDE_A,  {  52,  64,  30, 0xFF }},     //CS0_N, ODT0
   {2,  3,  SIDE_A,  {  56,  71,  96, 0xFF }},     //CS0_N, ODT0
   {3,  3,  SIDE_A,  {  65, 112, 102, 0xFF }},     //CS0_N, ODT0
   {4,  3,  SIDE_A,  {  54,  70,  39, 0xFF }},     //CS0_N, ODT0
   {5,  3,  SIDE_A,  {  36,  61,  96, 0xFF }},     //CS0_N, ODT0
   {0, 12,  SIDE_A,  {  19,  56,  96, 0xFF }},     //CKE0
   {1, 12,  SIDE_A,  {  46,  74,  59, 0xFF }},     //CKE0
   {2, 12,  SIDE_A,  {  19,  52,  78, 0xFF }},     //CKE0
   {3, 12,  SIDE_A,  {  46, 112, 110, 0xFF }},     //CKE0
   {4, 12,  SIDE_A,  {  46,  82,  52, 0xFF }},     //CKE0
   {5, 12,  SIDE_A,  {  22,  64,  83, 0xFF }},     //CKE0
   {0,  5,  SIDE_A,  {  65,  99, 106, 0xFF }},     //CS2_N
   {1,  5,  SIDE_A,  {  83, 108,  78, 0xFF }},     //CS2_N
   {2,  5,  SIDE_A,  {  81, 109,  98, 0xFF }},     //CS2_N
   {3,  5,  SIDE_A,  {  59, 119, 121, 0xFF }},     //CS2_N
   {4,  5,  SIDE_A,  {  74, 104,  79, 0xFF }},     //CS2_N
   {5,  5,  SIDE_A,  {  65, 102,  88, 0xFF }},     //CS2_N
   {0,  6,  SIDE_A,  {  73, 103,  77, 0xFF }},     //CS3_N
   {1,  6,  SIDE_A,  {  92, 114,  79, 0xFF }},     //CS3_N
   {2,  6,  SIDE_A,  {  76, 101,  92, 0xFF }},     //CS3_N
   {3,  6,  SIDE_A,  {  97, 153,  92, 0xFF }},     //CS3_N
   {4,  6,  SIDE_A,  {  79, 106,  80, 0xFF }},     //CS3_N
   {5,  6,  SIDE_A,  {  59,  93,  91, 0xFF }},     //CS3_N
   {0,  4,  SIDE_A,  {  58,  88,  95, 0xFF }},     //CS1_N, ODT1
   {1,  4,  SIDE_A,  {  61,  84,  72, 0xFF }},     //CS1_N, ODT1
   {2,  4,  SIDE_A,  {  72,  97,  91, 0xFF }},     //CS1_N, ODT1
   {3,  4,  SIDE_A,  {  77, 132, 117, 0xFF }},     //CS1_N, ODT1
   {4,  4,  SIDE_A,  {  49,  77,  71, 0xFF }},     //CS1_N, ODT1
   {5,  4,  SIDE_A,  {  62,  98,  81, 0xFF }},     //CS1_N, ODT1
   {0, 13,  SIDE_A,  {  18,  58,  85, 0xFF }},     //CKE1
   {1, 13,  SIDE_A,  {  37,  73,  82, 0xFF }},     //CKE1
   {2, 13,  SIDE_A,  {  19,  59,  51, 0xFF }},     //CKE1
   {3, 13,  SIDE_A,  {  42, 108, 101, 0xFF }},     //CKE1
   {4, 13,  SIDE_A,  {  39,  74,  75, 0xFF }},     //CKE1
   {5, 13,  SIDE_A,  {  26,  66,  62, 0xFF }},     //CKE1
   {0,  4,  SIDE_B,  {  49,  82, 107, 0xFF }},     //CS4_N, ODT2
   {1,  4,  SIDE_B,  {  52,  74,  51, 0xFF }},     //CS4_N, ODT2
   {2,  4,  SIDE_B,  {  65,  91,  90, 0xFF }},     //CS4_N, ODT2
   {3,  4,  SIDE_B,  {  90, 149, 114, 0xFF }},     //CS4_N, ODT2
   {4,  4,  SIDE_B,  {  41,  69,  42, 0xFF }},     //CS4_N, ODT2
   {5,  4,  SIDE_B,  {  42,  77,  96, 0xFF }},     //CS4_N, ODT2
   {0, 11,  SIDE_A,  {   8,  41,  91, 0xFF }},     //CKE2
   {1, 11,  SIDE_A,  {   3,  29,  68, 0xFF }},     //CKE2
   {2, 11,  SIDE_A,  {  28,  56,  95, 0xFF }},     //CKE2
   {3, 11,  SIDE_A,  {  48, 107,  81, 0xFF }},     //CKE2
   {4, 11,  SIDE_A,  {  33,  66,  45, 0xFF }},     //CKE2
   {5, 11,  SIDE_A,  {   0,  36, 103, 0xFF }},     //CKE2
   {0,  2,  SIDE_A,  {  83, 125, 100, 0xFF }},     //CS6_N, CS7_N
   {1,  2,  SIDE_A,  {  73, 107,  60, 0xFF }},     //CS6_N, CS7_N
   {2,  2,  SIDE_A,  {  73, 108, 121, 0xFF }},     //CS6_N, CS7_N
   {3,  2,  SIDE_A,  {  79, 144,  94, 0xFF }},     //CS6_N, CS7_N
   {4,  2,  SIDE_A,  {  79, 115,  57, 0xFF }},     //CS6_N, CS7_N
   {5,  2,  SIDE_A,  {  56, 104, 118, 0xFF }},     //CS6_N, CS7_N
   {0,  0,  SIDE_A,  {  63,  87, 111, 0xFF }},     //CS5_N, ODT3
   {1,  0,  SIDE_A,  {  41,  57,  54, 0xFF }},     //CS5_N, ODT3
   {2,  0,  SIDE_A,  {  89, 108, 115, 0xFF }},     //CS5_N, ODT3
   {3,  0,  SIDE_A,  {  91, 142, 124, 0xFF }},     //CS5_N, ODT3
   {4,  0,  SIDE_A,  {  71,  92,  51, 0xFF }},     //CS5_N, ODT3
   {5,  0,  SIDE_A,  {  60,  88, 103, 0xFF }},     //CS5_N, ODT3
   {0, 10,  SIDE_A,  {  22,  51,  66, 0xFF }},     //CKE3
   {1, 10,  SIDE_A,  {   0,  23,  56, 0xFF }},     //CKE3
   {2, 10,  SIDE_A,  {  21,  43,  69, 0xFF }},     //CKE3
   {3, 10,  SIDE_A,  {  45, 101,  68, 0xFF }},     //CKE3
   {4, 10,  SIDE_A,  {  25,  52,  60, 0xFF }},     //CKE3
   {5, 10,  SIDE_A,  {   4,  31,  73, 0xFF }},     //CKE3
};


struct IoGroupClkDelayStruct {
  UINT8   ch;                         // Channel
  UINT8   clk;                        // Clock number (4 clocks per channel)
  UINT16   delay[SKX_PCKG_TYPE];       // Delay in Pico Seconds SKX 28core/?/?
};

const struct IoGroupClkDelayStruct igClk[] = {

   {0,  0, { 42, 277, 279, 0xFF}},
   {1,  0, { 59, 285, 246, 0xFF}},
   {2,  0, { 48, 277, 275, 0xFF}},
   {3,  0, { 64, 326, 285, 0xFF}},
   {4,  0, { 61, 292, 236, 0xFF}},
   {5,  0, { 30, 269, 265, 0xFF}},
   {0,  1, { 35, 269, 256, 0xFF}},
   {1,  1, { 72, 292, 229, 0xFF}},
   {2,  1, { 55, 283, 266, 0xFF}},
   {3,  1, { 41, 299, 268, 0xFF}},
   {4,  1, { 70, 300, 227, 0xFF}},
   {5,  1, { 50, 288, 254, 0xFF}},
   {0,  2, { 44, 267, 263, 0xFF}},
   {1,  2, { 40, 254, 209, 0xFF}},
   {2,  2, { 49, 266, 291, 0xFF}},
   {3,  2, { 71, 320, 277, 0xFF}},
   {4,  2, { 56, 275, 224, 0xFF}},
   {5,  2, { 25, 252, 284, 0xFF}},
   {0,  3, { 28, 250, 267, 0xFF}},
   {1,  3, { 58, 270, 235, 0xFF}},
   {2,  3, { 42, 259, 305, 0xFF}},
   {3,  3, { 42, 291, 273, 0xFF}},
   {4,  3, { 49, 267, 242, 0xFF}},
   {5,  3, { 34, 260, 298, 0xFF}},
};


const struct IoGroupDelayStruct igCmd[] = {

   {0,  1,  SIDE_A,  {  84,  85,  63, 0xFF }},     //A17, C2
   {1,  1,  SIDE_A,  {  69,  61,  40, 0xFF }},     //A17, C2
   {2,  1,  SIDE_A,  {  93,  88,  81, 0xFF }},     //A17, C2
   {3,  1,  SIDE_A,  {  71, 104,  67, 0xFF }},     //A17, C2
   {4,  1,  SIDE_A,  {  68,  65,  42, 0xFF }},     //A17, C2
   {5,  1,  SIDE_A,  {  71,  76,  69, 0xFF }},     //A17, C2
   {0,  3,  SIDE_B,  {  86,  81,  54, 0xFF }},     //A13, A15
   {1,  3,  SIDE_B,  {  43,  28,  37, 0xFF }},     //A13, A15
   {2,  3,  SIDE_B,  {  64,  56,  14, 0xFF }},     //A13, A15
   {3,  3,  SIDE_B,  {  74, 101,  71, 0xFF }},     //A13, A15
   {4,  3,  SIDE_B,  {  36,  27,  36, 0xFF }},     //A13, A15
   {5,  3,  SIDE_B,  {  51,  53,  26, 0xFF }},     //A13, A15
   {0,  2,  SIDE_B,  {  41,  57,  62, 0xFF }},     //A14, A16
   {1,  2,  SIDE_B,  {  55,  62,  65, 0xFF }},     //A14, A16
   {2,  2,  SIDE_B,  {  74,  84,  89, 0xFF }},     //A14, A16
   {3,  2,  SIDE_B,  {  71, 111,  49, 0xFF }},     //A14, A16
   {4,  2,  SIDE_B,  {  54,  64,  64, 0xFF }},     //A14, A16
   {5,  2,  SIDE_B,  {  53,  70,  80, 0xFF }},     //A14, A16
   {0,  1,  SIDE_B,  {  42,  52,  52, 0xFF }},     //BA1, A10
   {1,  1,  SIDE_B,  {  40,  40,  41, 0xFF }},     //BA1, A10
   {2,  1,  SIDE_B,  {  47,  47,  37, 0xFF }},     //BA1, A10
   {3,  1,  SIDE_B,  {  68, 104,  64, 0xFF }},     //BA1, A10
   {4,  1,  SIDE_B,  {  37,  43,  42, 0xFF }},     //BA1, A10
   {5,  1,  SIDE_B,  {   8,  18,  50, 0xFF }},     //BA1, A10
   {0,  0,  SIDE_B,  {  46,  44,  60, 0xFF }},     //A0, BA0
   {1,  0,  SIDE_B,  {  52,  40,  25, 0xFF }},     //A0, BA0
   {2,  0,  SIDE_B,  {  70,  62,   0, 0xFF }},     //A0, BA0
   {3,  0,  SIDE_B,  {  59,  83,  70, 0xFF }},     //A0, BA0
   {4,  0,  SIDE_B,  {  45,  39,  23, 0xFF }},     //A0, BA0
   {5,  0,  SIDE_B,  {  57,  58,  61, 0xFF }},     //A0, BA0
   {0,  7,  SIDE_B,  {  61,  53,  73, 0xFF }},     //PAR
   {1,  7,  SIDE_B,  {  60,  43,  50, 0xFF }},     //PAR
   {2,  7,  SIDE_B,  {  60,  46,  81, 0xFF }},     //PAR
   {3,  7,  SIDE_B,  {  90, 109,  88, 0xFF }},     //PAR
   {4,  7,  SIDE_B,  {  57,  66,  88, 0xFF }},     //PAR
   {5,  7,  SIDE_B,  {  47,  42,  69, 0xFF }},     //PAR
   {0,  8,  SIDE_B,  {  31,  22,  33, 0xFF }},     //A1, A3
   {1,  8,  SIDE_B,  {  43,  24,  15, 0xFF }},     //A1, A3
   {2,  8,  SIDE_B,  {  36,  21,  42, 0xFF }},     //A1, A3
   {3,  8,  SIDE_B,  {  52,  70,  43, 0xFF }},     //A1, A3
   {4,  8,  SIDE_B,  {  39,  26,  10, 0xFF }},     //A1, A3
   {5,  8,  SIDE_B,  {   6,   5,  53, 0xFF }},     //A1, A3
   {0,  9,  SIDE_B,  {  44,  48,  47, 0xFF }},     //A2, A4
   {1,  9,  SIDE_B,  {  13,  10,  24, 0xFF }},     //A2, A4
   {2,  9,  SIDE_B,  {  53,  51,  66, 0xFF }},     //A2, A4
   {3,  9,  SIDE_B,  {  49,  79,  67, 0xFF }},     //A2, A4
   {4,  9,  SIDE_B,  {   0,   0,  24, 0xFF }},     //A2, A4
   {5,  9,  SIDE_B,  {  26,  34,  68, 0xFF }},     //A2, A4
   {0, 10,  SIDE_B,  {  37,  25,  28, 0xFF }},     //A5, A6
   {1, 10,  SIDE_B,  {  58,  38,   0, 0xFF }},     //A5, A6
   {2, 10,  SIDE_B,  {  36,  19,  59, 0xFF }},     //A5, A6
   {3, 10,  SIDE_B,  {  59,  73,  38, 0xFF }},     //A5, A6
   {4, 10,  SIDE_B,  {  58,  42,   0, 0xFF }},     //A5, A6
   {5, 10,  SIDE_B,  {   5,   0,  62, 0xFF }},     //A5, A6
   {0, 11,  SIDE_B,  {  30,  30,  52, 0xFF }},     //A7, A8
   {1, 11,  SIDE_B,  {  50,  43,  13, 0xFF }},     //A7, A8
   {2, 11,  SIDE_B,  {  38,  32,  20, 0xFF }},     //A7, A8
   {3, 11,  SIDE_B,  {  63,  91,  53, 0xFF }},     //A7, A8
   {4, 11,  SIDE_B,  {  17,  14,  18, 0xFF }},     //A7, A8
   {5, 11,  SIDE_B,  {  28,  31,  22, 0xFF }},     //A7, A8
   {0,  7,  SIDE_A,  {   2,   1,  12, 0xFF }},     //A9, A12
   {1,  7,  SIDE_A,  {  10,   0,  11, 0xFF }},     //A9, A12
   {2,  7,  SIDE_A,  {  11,   3,  38, 0xFF }},     //A9, A12
   {3,  7,  SIDE_A,  {   2,   0,  11, 0xFF }},     //A9, A12
   {4,  7,  SIDE_A,  {  38,  33,  36, 0xFF }},     //A9, A12
   {5,  7,  SIDE_A,  {  28,  28,  68, 0xFF }},     //A9, A12
   {0,  8,  SIDE_A,  {   0,   0,   0, 0xFF }},     //BG1, A11
   {1,  8,  SIDE_A,  {  36,  26,  15, 0xFF }},     //BG1, A11
   {2,  8,  SIDE_A,  {  28,  18,  26, 0xFF }},     //BG1, A11
   {3,  8,  SIDE_A,  {  47,  67,   0, 0xFF }},     //BG1, A11
   {4,  8,  SIDE_A,  {  51,  49,  22, 0xFF }},     //BG1, A11
   {5,  8,  SIDE_A,  {   8,   2,  37, 0xFF }},     //BG1, A11
   {0,  9,  SIDE_A,  {  26,  32,  41, 0xFF }},     //BG0, ACT_N
   {1,  9,  SIDE_A,  {  27,  29,  19, 0xFF }},     //BG0, ACT_N
   {2,  9,  SIDE_A,  {   3,   0,  11, 0xFF }},     //BG0, ACT_N
   {3,  9,  SIDE_A,  {  20,  53,  55, 0xFF }},     //BG0, ACT_N
   {4,  9,  SIDE_A,  {  24,  23,  21, 0xFF }},     //BG0, ACT_N
   {5,  9,  SIDE_A,  {  26,  35,   0, 0xFF }},     //BG0, ACT_N
};
#endif

#endif