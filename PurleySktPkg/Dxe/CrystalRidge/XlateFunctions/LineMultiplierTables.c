/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++
Copyright (c) 2016, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


Module Name:

  LineMultipliersTables.c

Abstract:

  Line multiplier tables used for DPA/SPA conversions.

--*/

#include <Library/UefiLib.h>

// For 4k/256 interleave granularity, the SAD interleave allows for 1, 2, 3, 4, 6, 8, 12, 16, and 24 way channel interleave
// For 3ChWay, this allows Imc1Way, Imc2Way, Imc4Way, and Imc8Way
// For 2ChWay, this allows Imc1Way, Imc2Way, Imc4Way, and Imc8Way

// For 4k/4k interleave granularity, the SAD interleave allows for 1, 2, 3, 4, 6, 8, 12, and 24 way channel interleave
// For 3ChWay, this allows Imc1Way, Imc2Way, Imc4Way, and Imc8Way
// For 2ChWay, this allows Imc1Way, Imc2Way, and Imc4Way

//
// The following table is used for the translation of Dpa to Spa based for the given ImcWays and 3 channel ways that the dimms participate in the
// interleave set.
//
CONST UINT32  Imc1Way4k_3ChWay256_LineMultipliers[3][16] = {{ 0, 3, 6, 9,  12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45 },                 // Imc0/Ch0
                                                            { 1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46 },                 // Imc0/Ch1
                                                            { 2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35, 38, 41, 44, 47 }};                // Imc0/Ch2

CONST UINT32  Imc2Way4k_3ChWay256_LineMultipliers[6][16] = {{ 0,  3,  6,  9,  12, 15, 66, 69, 72, 75, 78, 33, 36, 39, 42, 45 },              // Imc0/Ch0
                                                            { 1,  4,  7,  10, 13, 64, 67, 70, 73, 76, 79, 34, 37, 40, 43, 46 },              // Imc0/Ch1
                                                            { 2,  5,  8,  11, 14, 65, 68, 71, 74, 77, 32, 35, 38, 41, 44, 47 },              // Imc0/Ch2
                                                            { 48, 51, 54, 57, 60, 63, 18, 21, 24, 27, 30, 81, 84, 87, 90, 93 },               // Imc1/Ch0
                                                            { 49, 52, 55, 58, 61, 16, 19, 22, 25, 28, 31, 82, 85, 88, 91, 94 },               // Imc1/Ch1
                                                            { 50, 53, 56, 59, 62, 17, 20, 23, 26, 29, 80, 83, 86, 89, 92, 95 }};              // Imc1/Ch2

CONST UINT32  Imc4Way4k_3ChWay256_LineMultipliers[12][16] = {{ 0,   3,   6,   9,   12,  15,  66,  69,  72,  75,  78,  129, 132, 135, 138, 141 },  // Imc0/Ch0
                                                             { 1,   4,   7,   10,  13,  64,  67,  70,  73,  76,  79,  130, 133, 136, 139, 142 },  // Imc0/Ch1
                                                             { 2,   5,   8,   11,  14,  65,  68,  71,  74,  77,  128, 131, 134, 137, 140, 143 },  // Imc0/Ch2
                                                             { 144, 147, 150, 153, 156, 159, 18,  21,  24,  27,  30,  81,  84,  87,  90,  93 },    // Imc1/Ch0
                                                             { 145, 148, 151, 154, 157, 16,  19,  22,  25,  28,  31,  82,  85,  88,  91,  94 },    // Imc1/Ch1
                                                             { 146, 149, 152, 155, 158, 17,  20,  23,  26,  29,  80,  83,  86,  89,  92,  95 },    // Imc1/Ch2
                                                             { 96,  99,  102, 105, 108, 111, 162, 165, 168, 171, 174, 33,  36,  39,  42,  45 },   // Imc2/Ch0
                                                             { 97,  100, 103, 106, 109, 160, 163, 166, 169, 172, 175, 34,  37,  40,  43,  46 },   // Imc2/Ch1
                                                             { 98,  101, 104, 107, 110, 161, 164, 167, 170, 173, 32,  35,  38,  41,  44,  47 },   // Imc2/Ch2
                                                             { 48,  51,  54,  57,  60,  63,  114, 117, 120, 123, 126, 177, 180, 183, 186, 189 },   // Imc3/Ch0
                                                             { 49,  52,  55,  58,  61,  112, 115, 118, 121, 124, 127, 178, 181, 184, 187, 190 },   // Imc3/Ch1
                                                             { 50,  53,  56,  59,  62,  113, 116, 119, 122, 125, 176, 179, 182, 185, 188, 191 }};  // Imc3/Ch2

CONST UINT32  Imc8Way4k_3ChWay256_LineMultipliers[24][16]  = {{ 0,   3,   6,   9,   12,  15,  258, 261, 264, 267, 270, 129, 132, 135, 138, 141 }, // Imc0/Ch0
                                                              { 1,   4,   7,   10,  13,  256, 259, 262, 265, 268, 271, 130, 133, 136, 139, 142 }, // Imc0/Ch1
                                                              { 2,   5,   8,   11,  14,  257, 260, 263, 266, 269, 128, 131, 134, 137, 140, 143 }, // Imc0/Ch2
                                                              { 144, 147, 150, 153, 156, 159, 18,  21,  24,  27,  30,  273, 276, 279, 282, 285 },  // Imc1/Ch0
                                                              { 145, 148, 151, 154, 157, 16,  19,  22,  25,  28,  31,  274, 277, 280, 283, 286 },  // Imc1/Ch1
                                                              { 146, 149, 152, 155, 158, 17,  20,  23,  26,  29,  272, 275, 278, 281, 284, 287 },  // Imc1/Ch2
                                                              { 288, 291, 294, 297, 300, 303, 162, 165, 168, 171, 174, 33,  36,  39,  42,  45 },  // Imc2/Ch0
                                                              { 289, 292, 295, 298, 301, 160, 163, 166, 169, 172, 175, 34,  37,  40,  43,  46 },  // Imc2/Ch1
                                                              { 290, 293, 296, 299, 302, 161, 164, 167, 170, 173, 32,  35,  38,  41,  44,  47 },  // Imc2/Ch2
                                                              { 48,  51,  54,  57,  60,  63,  306, 309, 312, 315, 318, 177, 180, 183, 186, 189 },  // Imc3/Ch0
                                                              { 49,  52,  55,  58,  61,  304, 307, 310, 313, 316, 319, 178, 181, 184, 187, 190 },  // Imc3/Ch1
                                                              { 50,  53,  56,  59,  62,  305, 308, 311, 314, 317, 176, 179, 182, 185, 188, 191 },  // Imc3/Ch2
                                                              { 192, 195, 198, 201, 204, 207, 66,  69,  72,  75,  78,  321, 324, 327, 330, 333 }, // Imc4/Ch0
                                                              { 193, 196, 199, 202, 205, 64,  67,  70,  73,  76,  79,  322, 325, 328, 331, 334 }, // Imc4/Ch1
                                                              { 194, 197, 200, 203, 206, 65,  68,  71,  74,  77,  320, 323, 326, 329, 332, 335 }, // Imc4/Ch2
                                                              { 336, 339, 342, 345, 348, 351, 210, 213, 216, 219, 222, 81,  84,  87,  90,  93 },   // Imc5/Ch0
                                                              { 337, 340, 343, 346, 349, 208, 211, 214, 217, 220, 223, 82,  85,  88,  91,  94 },   // Imc5/Ch1
                                                              { 338, 341, 344, 347, 350, 209, 212, 215, 218, 221, 80,  83,  86,  89,  92,  95 },   // Imc5/Ch2
                                                              { 96,  99,  102, 105, 108, 111, 354, 357, 360, 363, 366, 225, 228, 231, 234, 237 }, // Imc6/Ch0
                                                              { 97,  100, 103, 106, 109, 352, 355, 358, 361, 364, 367, 226, 229, 232, 235, 238 }, // Imc6/Ch1
                                                              { 98,  101, 104, 107, 110, 353, 356, 359, 362, 365, 224, 227, 230, 233, 236, 239 }, // Imc6/Ch2
                                                              { 240, 243, 246, 249, 252, 255, 114, 117, 120, 123, 126, 369, 372, 375, 378, 381 },  // Imc7/Ch0
                                                              { 241, 244, 247, 250, 253, 112, 115, 118, 121, 124, 127, 370, 373, 376, 379, 382 },  // Imc7/Ch1
                                                              { 242, 245, 248, 251, 254, 113, 116, 119, 122, 125, 368, 371, 374, 377, 380, 383 }}; // Imc7/Ch2

// 4k Channel granularity multipliers
CONST UINT32  Imc1Way4k_3ChWay4k_LineMultipliers[3][2] = {{ 0, 3 }, { 1, 4 }, { 2, 5 }};  // Imc0

CONST UINT32  Imc2Way4k_3ChWay4k_LineMultipliers[6][2] = {{ 0, 6 }, { 4, 10 }, { 2, 8 },    // Imc0
                                                          { 3, 9 }, { 1, 7 },  { 5, 11 }};  // Imc1

CONST UINT32   Imc4Way4k_3ChWay4k_LineMultipliers[12][2] = {{ 0, 12 }, { 4, 16 },  { 8, 20 }, // Imc0
                                                            { 9, 21 }, { 1, 13 },  { 5, 17 }, // Imc1
                                                            { 6, 18 }, { 10, 22 }, { 2, 14 },
                                                            { 3, 15 }, { 7, 19 },  { 11, 23 }};

CONST UINT32  Imc8Way4k_3ChWay4k_LineMultipliers[24][2]  = {{ 0, 24 },  { 16, 40 }, { 8, 32 },  // Imc0
                                                            { 9, 33 },  { 1, 25 },  { 17, 41 }, // Imc1
                                                            { 18, 42 }, { 10, 34 }, { 2, 26 },
                                                            { 3, 27 },  { 19, 43 }, { 11, 35 },
                                                            { 12, 36 }, { 4, 28 },  { 20, 44 },
                                                            { 21, 45 }, { 13, 37 }, { 5, 29 },
                                                            { 6, 30 },  { 22, 46 }, { 14, 38 },
                                                            { 15, 39 }, { 7, 31 },  { 23, 47 }};

//
// The following table is used for the translation of Dpa to Spa based for the given ImcWays and 2 channel ways that the dimms participate in the
// interleave set.
//
CONST UINT32  Imc1Way4k_2ChWay256_LineMultipliers[2][8] = {{ 0, 2, 4, 6, 8, 10, 12, 14 }, // Imc0/Ch0
                                                           { 1, 3, 5, 7, 9, 11, 13, 15 }};

CONST UINT32  Imc2Way4k_2ChWay256_LineMultipliers[4][8] = {{ 0,  2,  4,  6,  8,  10, 12, 14 },  // Imc0/Ch0
                                                           { 1,  3,  5,  7,  9,  11, 13, 15 },
                                                           { 16, 18, 20, 22, 24, 26, 28, 30 },
                                                           { 17, 19, 21, 23, 25, 27, 29, 31 }};

CONST UINT32  Imc4Way4k_2ChWay256_LineMultipliers[8][8] = {{ 0,  2,  4,  6,  8,  10, 12, 14 },  // Imc0/Ch0
                                                           { 1,  3,  5,  7,  9,  11, 13, 15 },
                                                           { 16, 18, 20, 22, 24, 26, 28, 30 },
                                                           { 17, 19, 21, 23, 25, 27, 29, 31 },
                                                           { 32, 34, 36, 38, 40, 42, 44, 46 },
                                                           { 33, 35, 37, 39, 41, 43, 45, 47 },
                                                           { 48, 50, 52, 54, 56, 58, 60, 62 },
                                                           { 49, 51, 53, 55, 57, 59, 61, 63 }};

CONST UINT32  Imc8Way4k_2ChWay256_LineMultipliers[16][8] = {{ 0,   2,   4,   6,   8,   10,  12,  14 },  // Imc0/Ch0
                                                            { 1,   3,   5,   7,   9,   11,  13,  15 },
                                                            { 16,  18,  20,  22,  24,  26,  28,  30 },
                                                            { 17,  19,  21,  23,  25,  27,  29,  31 },
                                                            { 32,  34,  36,  38,  40,  42,  44,  46 },
                                                            { 33,  35,  37,  39,  41,  43,  45,  47 },
                                                            { 48,  50,  52,  54,  56,  58,  60,  62 },
                                                            { 49,  51,  53,  55,  57,  59,  61,  63 },
                                                            { 64,  66,  68,  70,  72,  74,  76,  78 },
                                                            { 65,  67,  69,  71,  73,  75,  77,  79 },
                                                            { 80,  82,  84,  86,  88,  90,  92,  94 },
                                                            { 81,  83,  85,  87,  89,  91,  93,  95 },
                                                            { 96,  98,  100, 102, 104, 106, 108, 110 },
                                                            { 97,  99,  101, 103, 105, 107, 109, 111 },
                                                            { 112, 114, 116, 118, 120, 122, 124, 126 },
                                                            { 113, 115, 117, 119, 121, 123, 125, 127 }};

// 4k Channel granularity multipliers
CONST UINT32  Imc1Way4k_2ChWay4k_LineMultipliers[2][2] = {{ 0, 2 }, { 1, 3 }};  // Imc0

CONST UINT32  Imc2Way4k_2ChWay4k_LineMultipliers[4][2] = {{ 0, 4 }, { 1, 5 },   // Imc0
                                                          { 2, 6 }, { 3, 7 }};  // Imc1

CONST UINT32  Imc4Way4k_2ChWay4k_LineMultipliers[8][2] = {{ 0, 8 },  { 1, 9 },  // Imc0
                                                          { 2, 10 }, { 3, 11 }, // Imc1
                                                          { 4, 12 }, { 5, 13 },
                                                          { 6, 14 }, { 7, 15 }};

