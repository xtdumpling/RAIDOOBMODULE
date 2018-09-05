/*++
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++
Copyright (c) 1996 - 2017, Intel Corporation.

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

  JedecNvDimm.c

Abstract:

  Implementation of JEDEC NVDIMM NFIT/DSM

--*/
//
// Include files
//
#include "JedecNvDimm.h"
#include "Nfit.h"

#pragma warning(disable : 4100)
#pragma optimize("", off)


//
// Globals
EFI_JEDEC_NVDIMM_PROTOCOL       mJedecNvDimmProtocol;
struct SystemMemoryMapHob       *mSystemMemoryMap;
CR_INFO                         CrInfo;
EFI_SMM_BASE2_PROTOCOL          *mSmmBase = NULL;
EFI_SMM_SYSTEM_TABLE2           *mSmst = NULL;
struct sysHost                  mHost;

UINT16                          NumSMBiosHandles;
SMBIOS_HANDLE_INFO              SMBiosHandleInfo[MAX_SOCKET * MAX_CH * MAX_DIMM];
EFI_CPU_CSR_ACCESS_PROTOCOL     *mCpuCsrAccess;
NGN_ACPI_SMM_INTERFACE          *mNgnAcpiSmmInterface;
EFI_NGN_ACPI_SMM_INTERFACE_PROTOCOL *mNgnAcpiSmmInterfaceArea;
EFI_NGN_ACPI_SMM_INTERFACE_PROTOCOL  NgnAcpiSmmInterfaceArea;
UINT8                           Skts[MAX_SOCKET];
UINT8                           Chs[MAX_MC_CH * MAX_IMC];
// Used for ARS Implementation
BOOLEAN                         mIsArs = FALSE;
BOOLEAN                         mHasArsPerformed = FALSE;
UINT64                          ArsStartSpa;
UINT64                          ArsLength;
UINT32                          mPmemIndex;
ARS_DSM_HEADER                  mArsDsmHeader;
ARS_SCRUB_DATA                  mArsScrubData;
MCMAIN_CHKN_BITS_MC_MAIN_STRUCT mMcMainChknRegSave[MC_MAX_NODE];
UINT64                          PMEMSADBaseAddresses[MAX_PMEM_INTERLEAVE_SET];
BOOLEAN                         IsAepDimmPresent;
BOOLEAN                         IsJedecNvDimmPresent = FALSE;
PMEM_REGION                     *mPmemRegions;
UINT32                          mScrubIntervalSave;
UINT32                          mClearUcErrLenUnit = 1; // TODO: in bytes, this is prob the minimum error record length (1 byte or rank?)
UINT32                          mArsTimeEstimate = 100; // TODO: in seconds, how do we determine estimated time?

UINT8   mMaxErrorsPerRank;
UINT8   mNumErrorsCurrRank;

typedef struct {
  AMAP_MC_MAIN_STRUCT                 AMAPReg;
} CH_AMAP_STRUCT2;

typedef enum {
  SCRUBBING_NON_SPARERANK,
  SCRUBBING_SPARERANK
} SSRSTATETYPE;

typedef struct {
  SCRUBCTL_MC_MAIN_STRUCT              ScrubCtlReg;
  SCRUBADDRESSLO_MC_MAIN_STRUCT        ScrubAddrLoReg;
  SCRUBADDRESSHI_MC_MAIN_STRUCT        ScrubAddrHiReg;
  SCRUBADDRESS2LO_MCMAIN_STRUCT        ScrubAddrLo2Reg;
  SCRUBADDRESS2HI_MCMAIN_STRUCT        ScrubAddrHi2Reg;
  UINT32                               ScrubInterval;
  CH_AMAP_STRUCT2                      AMAPReg[MAX_MC_CH];

  SSRSTATETYPE                         SSRState;
  UINT8                                ScrubSpareCh;
  UINT8                                ScrubSpareRank;
} PATROL_SCRUB_STATUS_STRUC;

static
PATROL_SCRUB_STATUS_STRUC mPatrolScrubStatus[MC_MAX_NODE];

extern UINT32 BuildNfitDevHandle(UINT8, UINT8);

// For 4k/256 interleave granularity, the SAD interleave allows for 1, 2, 3, 4, 6, 8, 12, 16, and 24 way channel interleave
// For 3ChWay, this allows iMC1Way, iMC2Way, iMC4Way, and iMC8Way
// For 2ChWay, this allows iMC1Way, iMC2Way, iMC4Way, and iMC8Way

// For 4k/4k interleave granularity, the SAD interleave allows for 1, 2, 3, 4, 6, 8, 12, and 24 way channel interleave
// For 3ChWay, this allows iMC1Way, iMC2Way, iMC4Way, and iMC8Way
// For 2ChWay, this allows iMC1Way, iMC2Way, and iMC4Way

//
// The following table is used for the translation of Dpa to Spa based for the given iMCWays and 3 channel ways that the dimms participate in the
// interleave set. These numbers for this table is derived from the TestNfit Tool which is available separately.
//
UINT32  iMC1Way4k_3ChWay256_LineMultipliers[3][16] = {{ 0, 3, 6, 9,  12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45 },                  // iMC0/Ch0
                                                      { 1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46 },                 // iMC0/Ch1
                                                      { 2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, 35, 38, 41, 44, 47 }};                // iMC0/Ch2

UINT32  iMC2Way4k_3ChWay256_LineMultipliers[6][16] = {{ 0,  3,  6,  9,  12, 15, 66, 69, 72, 75, 78, 33, 36, 39, 42, 45 },                  // iMC0/Ch0
                                                      { 1,  4,  7,  10, 13, 64, 67, 70, 73, 76, 79, 34, 37, 40, 43, 46 },                 // iMC0/Ch1
                                                      { 2,  5,  8,  11, 14, 65, 68, 71, 74, 77, 32, 35, 38, 41, 44, 47 },                 // iMC0/Ch2
                                                      { 48, 51, 54, 57, 60, 63, 18, 21, 24, 27, 30, 81, 84, 87, 90, 93 },              // iMC1/Ch0
                                                      { 49, 52, 55, 58, 61, 16, 19, 22, 25, 28, 31, 82, 85, 88, 91, 94 },              // iMC1/Ch1
                                                      { 50, 53, 56, 59, 62, 17, 20, 23, 26, 29, 80, 83, 86, 89, 92, 95 }};             // iMC1/Ch2

UINT32  iMC4Way4k_3ChWay256_LineMultipliers[12][16] = {{ 0,   3,   6,   9,   12,  15,  66,  69,  72,  75,  78,  129, 132, 135, 138, 141 },           // iMC0/Ch0
                                                       { 1,   4,   7,   10,  13,  64,  67,  70,  73,  76,  79,  130, 133, 136, 139, 142 },          // iMC0/Ch1
                                                       { 2,   5,   8,   11,  14,  65,  68,  71,  74,  77,  128, 131, 134, 137, 140, 143 },         // iMC0/Ch2
                                                       { 144, 147, 150, 153, 156, 159, 18,  21,  24,  27,  30,  81,  84,  87,  90,  93 },      // iMC1/Ch0
                                                       { 145, 148, 151, 154, 157, 16,  19,  22,  25,  28,  31,  82,  85,  88,  91,  94 },       // iMC1/Ch1
                                                       { 146, 149, 152, 155, 158, 17,  20,  23,  26,  29,  80,  83,  86,  89,  92,  95 },       // iMC1/Ch2
                                                       { 96,  99,  102, 105, 108, 111, 162, 165, 168, 171, 174, 33,  36,  39,  42,  45 },   // iMC2/Ch0
                                                       { 97,  100, 103, 106, 109, 160, 163, 166, 169, 172, 175, 34,  37,  40,  43,  46 },  // iMC2/Ch1
                                                       { 98,  101, 104, 107, 110, 161, 164, 167, 170, 173, 32,  35,  38,  41,  44,  47 },   // iMC2/Ch2
                                                       { 48,  51,  54,  57,  60,  63,  114, 117, 120, 123, 126, 177, 180, 183, 186, 189 },  // iMC3/Ch0
                                                       { 49,  52,  55,  58,  61,  112, 115, 118, 121, 124, 127, 178, 181, 184, 187, 190 }, // iMC3/Ch1
                                                       { 50,  53,  56,  59,  62,  113, 116, 119, 122, 125, 176, 179, 182, 185, 188, 191 }};// iMC3/Ch2

UINT32  iMC8Way4k_3ChWay256_LineMultipliers[24][16]  = {{ 0,   3,   6,   9,   12,  15,  258, 261, 264, 267, 270, 129, 132, 135, 138, 141 },           // iMC0/Ch0
                                                        { 1,   4,   7,   10,  13,  256, 259, 262, 265, 268, 271, 130, 133, 136, 139, 142 },         // iMC0/Ch1
                                                        { 2,   5,   8,   11,  14,  257, 260, 263, 266, 269, 128, 131, 134, 137, 140, 143 },         // iMC0/Ch2
                                                        { 144, 147, 150, 153, 156, 159, 18,  21,  24,  27,  30,  273, 276, 279, 282, 285 },      // iMC1/Ch0
                                                        { 145, 148, 151, 154, 157, 16,  19,  22,  25,  28,  31,  274, 277, 280, 283, 286 },       // iMC1/Ch1
                                                        { 146, 149, 152, 155, 158, 17,  20,  23,  26,  29,  272, 275, 278, 281, 284, 287 },      // iMC1/Ch2
                                                        { 288, 291, 294, 297, 300, 303, 162, 165, 168, 171, 174, 33,  36,  39,  42,  45 },      // iMC2/Ch0
                                                        { 289, 292, 295, 298, 301, 160, 163, 166, 169, 172, 175, 34,  37,  40,  43,  46 },      // iMC2/Ch1
                                                        { 290, 293, 296, 299, 302, 161, 164, 167, 170, 173, 32,  35,  38,  41,  44,  47 },       // iMC2/Ch2
                                                        { 48,  51,  54,  57,  60,  63,  306, 309, 312, 315, 318, 177, 180, 183, 186, 189 },       // iMC3/Ch0
                                                        { 49,  52,  55,  58,  61,  304, 307, 310, 313, 316, 319, 178, 181, 184, 187, 190 },      // iMC3/Ch1
                                                        { 50,  53,  56,  59,  62,  305, 308, 311, 314, 317, 176, 179, 182, 185, 188, 191 },      // iMC3/Ch2
                                                        { 192, 195, 198, 201, 204, 207, 66,  69,  72,  75,  78,  321, 324, 327, 330, 333 },      // iMC4/Ch0
                                                        { 193, 196, 199, 202, 205, 64,  67,  70,  73,  76,  79,  322, 325, 328, 331, 334 },       // iMC4/Ch1
                                                        { 194, 197, 200, 203, 206, 65,  68,  71,  74,  77,  320, 323, 326, 329, 332, 335 },      // iMC4/Ch2
                                                        { 336, 339, 342, 345, 348, 351, 210, 213, 216, 219, 222, 81,  84,  87,  90,  93 },      // iMC5/Ch0
                                                        { 337, 340, 343, 346, 349, 208, 211, 214, 217, 220, 223, 82,  85,  88,  91,  94 },      // iMC5/Ch1
                                                        { 338, 341, 344, 347, 350, 209, 212, 215, 218, 221, 80,  83,  86,  89,  92,  95 },       // iMC5/Ch2
                                                        { 96,  99,  102, 105, 108, 111, 354, 357, 360, 363, 366, 225, 228, 231, 234, 237 },   // iMC6/Ch0
                                                        { 97,  100, 103, 106, 109, 352, 355, 358, 361, 364, 367, 226, 229, 232, 235, 238 },  // iMC6/Ch1
                                                        { 98,  101, 104, 107, 110, 353, 356, 359, 362, 365, 224, 227, 230, 233, 236, 239 },  // iMC6/Ch2
                                                        { 240, 243, 246, 249, 252, 255, 114, 117, 120, 123, 126, 369, 372, 375, 378, 381 }, // iMC7/Ch0
                                                        { 241, 244, 247, 250, 253, 112, 115, 118, 121, 124, 127, 370, 373, 376, 379, 382 }, // iMC7/Ch1
                                                        { 242, 245, 248, 251, 254, 113, 116, 119, 122, 125, 368, 371, 374, 377, 380, 383 }};// iMC7/Ch2

// 4k Channel granularity multipliers
UINT32  iMC1Way4k_3ChWay4k_LineMultipliers[3][2] = {{ 0, 3 }, { 1, 4 }, { 2, 5 }};  // iMC0

UINT32  iMC2Way4k_3ChWay4k_LineMultipliers[6][2] = {{ 0, 6 }, { 4, 10 }, { 2, 8 },    // iMC0
                                                    { 3, 9 }, { 1, 7 },  { 5, 11 }};  // iMC1

UINT32   iMC4Way4k_3ChWay4k_LineMultipliers[12][2] = {{ 0, 12 }, { 4, 16 },  { 8, 20 }, // iMC0
                                                      { 9, 21 }, { 1, 13 },  { 5, 17 }, // iMC1
                                                      { 6, 18 }, { 10, 22 }, { 2, 14 },
                                                      { 3, 15 }, { 7, 19 },  { 11, 23 }};

UINT32  iMC8Way4k_3ChWay4k_LineMultipliers[24][2]  = {{ 0, 24 },  { 16, 40 }, { 8, 32 },  // iMC0
                                                      { 9, 33 },  { 1, 25 },  { 17, 41 }, // iMC1
                                                      { 18, 42 }, { 10, 34 }, { 2, 26 },
                                                      { 3, 27 },  { 19, 43 }, { 11, 35 },
                                                      { 12, 36 }, { 4, 28 },  { 20, 44 },
                                                      { 21, 45 }, { 13, 37 }, { 5, 29 },
                                                      { 6, 30 },  { 22, 46 }, { 14, 38 },
                                                      { 15, 39 }, { 7, 31 },  { 23, 47 }};

//
// The following table is used for the translation of Dpa to Spa based for the given iMCWays and 2 channel ways that the dimms participate in the
// interleave set.
//
UINT32  iMC1Way4k_2ChWay256_LineMultipliers[2][8] = {{ 0, 2, 4, 6, 8, 10, 12, 14 }, // iMC0/Ch0
                                                     { 1, 3, 5, 7, 9, 11, 13, 15 }};

UINT32  iMC2Way4k_2ChWay256_LineMultipliers[4][8] = {{ 0,  2,  4,  6,  8,  10, 12, 14 },  // iMC0/Ch0
                                                     { 1,  3,  5,  7,  9,  11, 13, 15 },
                                                     { 16, 18, 20, 22, 24, 26, 28, 30 },
                                                     { 17, 19, 21, 23, 25, 27, 29, 31 }};

UINT32  iMC4Way4k_2ChWay256_LineMultipliers[8][8] = {{ 0,  2,  4,  6,  8,  10, 12, 14 },  // iMC0/Ch0
                                                     { 1,  3,  5,  7,  9,  11, 13, 15 },
                                                     { 16, 18, 20, 22, 24, 26, 28, 30 },
                                                     { 17, 19, 21, 23, 25, 27, 29, 31 },
                                                     { 32, 34, 36, 38, 40, 42, 44, 46 },
                                                     { 33, 35, 37, 39, 41, 43, 45, 47 },
                                                     { 48, 50, 52, 54, 56, 58, 60, 62 },
                                                     { 49, 51, 53, 55, 57, 59, 61, 63 }};

UINT32  iMC8Way4k_2ChWay256_LineMultipliers[16][8] = {{ 0,   2,   4,   6,   8,   10,  12,  14 },  // iMC0/Ch0
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
UINT32  iMC1Way4k_2ChWay4k_LineMultipliers[2][2] = {{ 0, 2 }, { 1, 3 }};  // iMC0

UINT32  iMC2Way4k_2ChWay4k_LineMultipliers[4][2] = {{ 0, 4 }, { 1, 5 },   // iMC0
                                                    { 2, 6 }, { 3, 7 }};  // iMC1

UINT32  iMC4Way4k_2ChWay4k_LineMultipliers[8][2] = {{ 0, 8 },  { 1, 9 },  // iMC0
                                                    { 2, 10 }, { 3, 11 }, // iMC1
                                                    { 4, 12 }, { 5, 13 },
                                                    { 6, 14 }, { 7, 15 }};


/**

  Save Patrol scrub engine registers

  @param NodeId - Memory controller ID


  @retval EFI_SUCCESS - Always

  **/
EFI_STATUS
SavePatrolScrubEngine(
  IN UINT8 Node
)
{
  UINT8                        Socket = NODE_TO_SKT(Node);
  UINT8                        Mc     = NODE_TO_MC(Node);
  UINT8                        ChOnMc;
  SCRUBCTL_MC_MAIN_STRUCT      ScrubCtlReg;

  //
  // Save registers and disable scrubbing
  //
  mPatrolScrubStatus[Node].ScrubAddrLoReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG);
  mPatrolScrubStatus[Node].ScrubAddrHiReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG);
  mPatrolScrubStatus[Node].ScrubAddrLo2Reg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESS2LO_MCMAIN_REG);
  mPatrolScrubStatus[Node].ScrubAddrHi2Reg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESS2HI_MCMAIN_REG);
  ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
  mPatrolScrubStatus[Node].ScrubCtlReg.Data    =  ScrubCtlReg.Data;
  mPatrolScrubStatus[Node].ScrubInterval       = ScrubCtlReg.Bits.scrubinterval;

  for (ChOnMc = 0; ChOnMc < MAX_MC_CH; ChOnMc ++){
      mPatrolScrubStatus[Node].AMAPReg[ChOnMc].AMAPReg.Data =  mCpuCsrAccess->ReadCpuCsr(Socket,NODECH_TO_SKTCH(Mc,ChOnMc), AMAP_MC_MAIN_REG);
  }

  return EFI_SUCCESS;
}

/**

  Disable Patrol scrub engine and save patrol registers

  @param NodeId - Memory controller ID


  @retval EFI_SUCCESS - Always

  **/
EFI_STATUS
EFIAPI
DisablePatrolScrubEngine(
  IN UINT8 Node
)
{
  UINT8                        Socket = NODE_TO_SKT(Node);
  UINT8                        Mc     = NODE_TO_MC(Node);
  SCRUBCTL_MC_MAIN_STRUCT      ScrubCtlReg;

  ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
  if (ScrubCtlReg.Bits.scrub_en == 1) {

    //
    //  disable scrubbing
    //
    ScrubCtlReg.Bits.scrub_en = 0;
    mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtlReg.Data);
  }

  return EFI_SUCCESS;
}

/**

  Enable Patrol scrub engine to start from the last Address.

  @param NodeId - Memory controller ID

  @retval EFI_SUCCESS - always

  **/
EFI_STATUS
EFIAPI
ReEnablePatrolScrubEngine(
IN UINT8 Node
)
{
  UINT8 Socket = NODE_TO_SKT(Node);
  UINT8 Mc = NODE_TO_MC(Node);
  SCRUBCTL_MC_MAIN_STRUCT      ScrubCtlReg;

  //
  // Disable scrubbing bgefore enbaling the scrub address registers.
  //
  ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
  if (ScrubCtlReg.Bits.scrub_en == 1) {
    ScrubCtlReg.Bits.scrub_en = 0;
    mCpuCsrAccess->WriteMcCpuCsr (Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtlReg.Data);
  }
  //
  // Patrol scrub was enabled before. Re-enable it now and restore old register values
  //
  mCpuCsrAccess->WriteMcCpuCsr (Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG, mPatrolScrubStatus[Node].ScrubAddrLoReg.Data);
  mCpuCsrAccess->WriteMcCpuCsr (Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG, mPatrolScrubStatus[Node].ScrubAddrHiReg.Data);
  mCpuCsrAccess->WriteMcCpuCsr (Socket, Mc, SCRUBADDRESS2LO_MCMAIN_REG, (UINT32)mPatrolScrubStatus[Node].ScrubAddrLo2Reg.Data);
  mCpuCsrAccess->WriteMcCpuCsr (Socket, Mc, SCRUBADDRESS2HI_MCMAIN_REG, (UINT32)mPatrolScrubStatus[Node].ScrubAddrHi2Reg.Data);

  mCpuCsrAccess->WriteMcCpuCsr (Socket, Mc, SCRUBCTL_MC_MAIN_REG, mPatrolScrubStatus[Node].ScrubCtlReg.Data);

  return EFI_SUCCESS;
}

/**

Routine Description:AddNvDimmToPMem adds a found NVDIMM
to the global mPmemRegions structure

  @param PmemIndex   - Index to PMEM region
  @param NvDimmIndex - Index to NVDIMM
  @param XlatedAddr  - NVDIMM to add
  @retval Status     - Status

**/
EFI_STATUS
AddNvDimmToPMem(
   UINTN                PmemIndex,
   UINTN                NvDimmIndex,
   TRANSLATED_ADDRESS   XlatedAddr
   )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  NVDIMM      *pNvDimm;

  mPmemRegions[PmemIndex].NumNvDimms++;
  mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].ImcId = XlatedAddr.MemoryControllerId;
  mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].SocketId = XlatedAddr.SocketId;
  mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].Dimm = XlatedAddr.DimmSlot;
  mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].Ch = XlatedAddr.ChannelId;
  mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].SpaBase = XlatedAddr.SystemAddress;
  mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].EnergyType = mSystemMemoryMap->Socket[XlatedAddr.SocketId].ChannelInfo[(XlatedAddr.MemoryControllerId * 3) + XlatedAddr.ChannelId].DimmInfo[XlatedAddr.DimmSlot].EnergyType;
  mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].Size = (UINT64)mSystemMemoryMap->Socket[XlatedAddr.SocketId].ChannelInfo[(XlatedAddr.MemoryControllerId * 3) + XlatedAddr.ChannelId].DimmInfo[XlatedAddr.DimmSlot].NVmemSize  * (64 * 1024 * 1024);
  pNvDimm = &mPmemRegions[PmemIndex].NvDimms[NvDimmIndex];
  DEBUG ((EFI_D_ERROR, "NVDIMM Socket = %d\n", mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].SocketId));
  DEBUG ((EFI_D_ERROR, "NVDIMM ImcId = %d\n", mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].ImcId));
  DEBUG ((EFI_D_ERROR, "NVDIMM Channel = %d\n", mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].Ch));
  DEBUG ((EFI_D_ERROR, "NVDIMM DIMM # = %d\n", mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].Dimm));
  DEBUG ((EFI_D_ERROR, "NVDIMM Base Address = 0x%lX\n", mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].SpaBase));
  DEBUG ((EFI_D_ERROR, "NVDIMM EnergyType = 0x%lX\n", mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].EnergyType));
  DEBUG ((EFI_D_ERROR, "NVDIMM size = 0x%lX\n", mPmemRegions[PmemIndex].NvDimms[NvDimmIndex].Size));
  // Set DIMM ES Policy - Already enabled at boot during JEDEC ARM!
  // SetDimmESPolicy(&mPmemRegions[PmemIndex].NvDimms[NvDimmIndex]);

  // Init DSM attributes
  DEBUG ((EFI_D_ERROR, "NVDIMM Get Identification\n"));
  GetDimmId(pNvDimm);
  DEBUG ((EFI_D_ERROR, "NVDIMM Get Power Info\n"));
  GetDimmPwrInfo(pNvDimm);
  DEBUG ((EFI_D_ERROR, "NVDIMM Get Energy Storage Info\n"));
  GetDimmESInfo(pNvDimm);
  DEBUG ((EFI_D_ERROR, "NVDIMM Get Operation Statistics Info\n"));
  GetDimmOpStatistics(pNvDimm);
  DEBUG ((EFI_D_ERROR, "NVDIMM Get Module Info\n"));
  GetDimmIModuleInfo(pNvDimm);
  IsJedecNvDimmPresent = TRUE;

  return Status;
}

/**

Routine Description: FillPmemRegions searches the SAD table to find entries
that are contained with persistent memory regions in the memory map.  The global
variable mPmemRegions contains all PMEM regions and JEDEC NVDIMMs found.

  @param  VOID         - None
  @retval EFI_STATUS   - EFI_SUCCESS if found at least one NVDIMM

**/
EFI_STATUS
FillPmemRegions(
   VOID
   )
{
  EFI_STATUS            Status;
  struct SADTable       *pSAD;
  UINT8                 TmpMemoryMap[1];
  UINTN                 MapKey;
  UINTN                 DescriptorSize;
  UINT32                DescriptorVersion;
  UINTN                 MemoryMapSize;
  EFI_MEMORY_DESCRIPTOR *MemoryMap;
  EFI_MEMORY_DESCRIPTOR *MemoryMapPtr;
  UINTN                 Index;
  UINTN                 PmemIndex = 0;
  UINTN                 NvDimmIndex;
  UINT64                SadBaseAddress;
  UINT64                SadLimit;
  UINT64                NvDimmAddress;
  UINT8                 Socket;
  UINT8                 SadNum;
  UINT64                CurrentBase;
  UINT64                PmemLimit;
  UINT8                 ChInterBitmap;
  UINT8                 i;
  TRANSLATED_ADDRESS    XlatedAddr;
  UINT64                NvDimmSize;
  UINT64                NvDimmOffset;
  UINT8                 NvDimmsInSad;

  DEBUG((EFI_D_ERROR, "FillPmemRegions start\n"));
  //
  // Get System MemoryMapSize
  //
  MemoryMapSize = 1;
  Status = gBS->GetMemoryMap (
                  &MemoryMapSize,
                  (EFI_MEMORY_DESCRIPTOR *)TmpMemoryMap,
                  &MapKey,
                  &DescriptorSize,
                  &DescriptorVersion
                  );
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);
  //
  // Enlarge space here, because we will allocate pool now.
  //
  MemoryMapSize += EFI_PAGE_SIZE;
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  MemoryMapSize,
                  (VOID**)&MemoryMap
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Get System MemoryMap
  //
  Status = gBS->GetMemoryMap (
                  &MemoryMapSize,
                  MemoryMap,
                  &MapKey,
                  &DescriptorSize,
                  &DescriptorVersion
                  );
  ASSERT_EFI_ERROR (Status);

  MemoryMapPtr = MemoryMap;

  Status = EFI_NOT_FOUND;

  //
  // Search for the PMEM regions
  //
  for (Index = 0; Index < (MemoryMapSize / DescriptorSize); Index++) {
    switch (MemoryMap->Type){
      case EfiPersistentMemory:

        DEBUG((EFI_D_ERROR, "EfiPersistentMemory region found at %lx\n", MemoryMap->PhysicalStart));
        // Scan SAD Table for corresponding memory region
        SadBaseAddress = 0;
        CurrentBase = MemoryMap->PhysicalStart;
        PmemLimit = MemoryMap->PhysicalStart + (MemoryMap->NumberOfPages <<  EFI_PAGE_SHIFT);
        for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
          for (SadNum = 0; SadNum < SAD_RULES; SadNum++) {
            if (mSystemMemoryMap->Socket[Socket].SAD[SadNum].Enable == 0) continue;
            if (mSystemMemoryMap->Socket[Socket].SAD[SadNum].local == 0) continue;
            SadLimit = (UINT64)mSystemMemoryMap->Socket[Socket].SAD[SadNum].Limit << 26;
            ChInterBitmap = GetChInterBitmap(&mSystemMemoryMap->Socket[Socket].SAD[SadNum]);

            if ((SadBaseAddress == CurrentBase) && (SadBaseAddress != SadLimit)) {
              if (PmemIndex >= MAX_PMEM_REGIONS) {
                DEBUG ((EFI_D_ERROR, "WARNING: PMEM region number exceeds max (%d)...skipping.\n", MAX_PMEM_REGIONS));
                break;
              }
              pSAD = &mSystemMemoryMap->Socket[Socket].SAD[SadNum];
              mPmemRegions[PmemIndex].BaseAddress = CurrentBase;
              mPmemRegions[PmemIndex].Size = SadLimit - SadBaseAddress;
              mPmemRegions[PmemIndex].SadIndex = SadNum;
              mPmemRegions[PmemIndex].SocketNum = Socket;
              mPmemRegions[PmemIndex].NumNvDimms = 0;
              NvDimmIndex = 0;

              DEBUG((EFI_D_ERROR, "SAD[%x] matches\n", SadNum));
              DEBUG((EFI_D_ERROR, "SAD[%x] base = %lx\n", SadNum, SadBaseAddress));
              DEBUG((EFI_D_ERROR, "SAD[%x] limit = %lx\n", SadNum, SadLimit));
              DEBUG((EFI_D_ERROR, "SAD[%x] type = %x\n", SadNum, pSAD->type));
              DEBUG((EFI_D_ERROR, "SAD[%x] ChInterBitmap = %x\n", SadNum, ChInterBitmap));
              DEBUG((EFI_D_ERROR, "SAD[%x] imcInterBitmap = %x\n", SadNum, pSAD->imcInterBitmap));

              NvDimmAddress = SadBaseAddress;

              // Determine number of NVDIMMs in SAD by getting size of first one
              ZeroMem (&XlatedAddr, sizeof(TRANSLATED_ADDRESS));
              Status = SystemAddressToDimmAddress (NvDimmAddress, &XlatedAddr);
              if (EFI_ERROR(Status)) {
                ASSERT_EFI_ERROR (Status);
                return Status;
              }

              NvDimmSize = (UINT64)mSystemMemoryMap->Socket[XlatedAddr.SocketId].ChannelInfo[(XlatedAddr.MemoryControllerId * 3) + XlatedAddr.ChannelId].DimmInfo[XlatedAddr.DimmSlot].NVmemSize  * (64 * 1024 * 1024);
              if (NvDimmSize == 0) { 
                    DEBUG((EFI_D_ERROR, "NvDimmSize is 0 not a real NvDimm go to next DIMM\n"));
                    continue;
              }

              NvDimmsInSad = (UINT8)((SadLimit - SadBaseAddress) / NvDimmSize);
              DEBUG((EFI_D_ERROR, "Total NVDIMMS in SAD  = %d\n", NvDimmsInSad));

              // If interleaving is disabled then the next NVDIMM is located after NvDimmSize,
              // otherwise the next NVDIMM in interleave set is at offset of SAD granularity
              NvDimmOffset = NvDimmSize;
              if (GetiMCWays(Socket, SadNum) != 1) NvDimmOffset = GetSocketGranularity(pSAD);
              if (GetChWays(Socket, SadNum) != 1) NvDimmOffset = GetChannelGranularity(pSAD);

              // Find all NVDIMMs in SAD
              for (i = 0; i < NvDimmsInSad; i++) {
                ZeroMem (&XlatedAddr, sizeof(TRANSLATED_ADDRESS));
                Status = SystemAddressToDimmAddress (NvDimmAddress, &XlatedAddr);
                if (EFI_ERROR(Status)) {
                  ASSERT_EFI_ERROR (Status);
                  return Status;
                }

                Status = AddNvDimmToPMem (PmemIndex, NvDimmIndex, XlatedAddr);
                NvDimmAddress += NvDimmOffset;

                // This handles the case when interleaving is disabled and 2 NVDIMMs are
                // contained within 1 SAD entry.  If interleaving is disabled and 2 NVDIMMs
                // on one channel then create 2 PMEM regions each with 1 NVDIMM.
                if ((NvDimmsInSad > 1) && (NvDimmOffset == NvDimmSize)) {
                  // Update last PMEM region size to match size of NVDIMM
                  mPmemRegions[PmemIndex].Size = NvDimmSize;
                  DEBUG((EFI_D_ERROR, "PMEM region contains %d NVDIMMs\n", mPmemRegions[PmemIndex].NumNvDimms));

                  // Add new PMEM region
                  PmemIndex++;
                  mPmemRegions[PmemIndex].BaseAddress = NvDimmAddress;
                  mPmemRegions[PmemIndex].Size = NvDimmSize;
                  mPmemRegions[PmemIndex].SadIndex = SadNum;
                  mPmemRegions[PmemIndex].SocketNum = Socket;
                  mPmemRegions[PmemIndex].NumNvDimms = 0;
                  NvDimmIndex = 0;  // Reset to 0 for new PMEM region
                  i++;              // Increment index for this NVDIMM

                  // Add NVDIMM to new PMEM region
                  ZeroMem (&XlatedAddr, sizeof(TRANSLATED_ADDRESS));
                  Status = SystemAddressToDimmAddress (NvDimmAddress, &XlatedAddr);
                  if (EFI_ERROR(Status)) {
                    ASSERT_EFI_ERROR (Status);
                    return Status;
                  }
                  Status = AddNvDimmToPMem(PmemIndex, NvDimmIndex, XlatedAddr);
                  NvDimmAddress += NvDimmOffset;
                }
                else {
                  NvDimmIndex++;
                }
              }

              // PMEM region may contain multiple SAD regions
              if (PmemLimit > SadLimit) {
                CurrentBase = SadLimit;
              }
              DEBUG((EFI_D_ERROR, "PMEM region contains %d NVDIMMs\n", mPmemRegions[PmemIndex].NumNvDimms));
              PmemIndex++;
            }
            SadBaseAddress = SadLimit;
          }
        }
        break;
      default:
        break;
    }
    MemoryMap = (EFI_MEMORY_DESCRIPTOR *)((UINTN)MemoryMap + DescriptorSize);
  }

  gBS->FreePool (MemoryMapPtr);

  return Status;
}


/**

  Find the MC index to use for calculating channel ways

  @param imcInterBitmap : bitmap of IMCs in the interleave.
  imcInterBitmap must be a non-zero value in input. Routine assumes value has BIT0 and/or BIT1 set.

  @retval IMC number to use for calculating channel ways

**/
UINT8
GetMcIndexFromBitmap (
  UINT8 imcInterBitmap
  )
{
  UINT8 mc;

  // if this entry is IMC0/IMC1 interleaved then use MC0 since number of channels have to be the same > 1 IMC way
  if ((imcInterBitmap & BIT0) && (imcInterBitmap & BIT1)) {
    mc = 0;
  } else if (imcInterBitmap & BIT1) { // if this entry for MC1 only
    mc = 1;
  } else { // if this entry for MC0 only
    mc = 0;
  }
  return mc;
}


/**

Routine Description: GetChInterBitmap - This function gets
channelInterBitmap or FMchannelInterBitmap depeding on the type.

  @param pSAD       - Pointer to the SAD
  @retval ChInter   - Return channel interleave bitmap

**/
UINT8
GetChInterBitmap(
   struct SADTable  *pSAD
   )
{
  UINT8 ChInterBitmap = 0;
  UINT8 McIndex;

  McIndex = GetMcIndexFromBitmap (pSAD->imcInterBitmap);

  switch (pSAD->type)  {
    case MEM_TYPE_1LM:
      ChInterBitmap = pSAD->channelInterBitmap[McIndex];
      break;
    case MEM_TYPE_PMEM:
    case MEM_TYPE_2LM:
    case MEM_TYPE_PMEM_CACHE:
    case MEM_TYPE_BLK_WINDOW:
    case MEM_TYPE_CTRL:
      ChInterBitmap = pSAD->FMchannelInterBitmap[McIndex];
      break;
  }

  return ChInterBitmap;
}

/**

Routine Description: GetiMCWays - This function computes the
number of ways iMC has been interleaved for a particular SAD in
both the Numa and Uma cases.

  @param socket      - Socket Number
  @param SadInx      - SAD Index
  @retval iMCWays    - iMC Interleave Ways

**/
UINT8
GetiMCWays(
   UINT8       socket,
   UINT8       SadInx
   )
{
  UINT8        ImcWays, i;

  ImcWays = 0;
  for (i = 0; i < MC_MAX_NODE; i++) {
    if (mSystemMemoryMap->Socket[socket].SADintList[SadInx][i]) {
      ImcWays++;
    }
  }
  return ImcWays;
}

/**

Routine Description: CreateDimmPresentBitmaps - This function
basically creates 2 64-bit fields that are part of the
AcpiSMMInterface. The first 64-bit field is for socket 0-3 and
the second 64-bit field is for socket 3-7. This field contains
bit that are set in the corresponding position when an
NVMDIMM is present in that position. Least significant 16 bits
(of the 1st field) represents socket0 and the most signifFicant
16-bit is for socket3 and the other sockets in between. For
example, if the system has 2 sockets and there is an NVMDIMM in
Dimm0 of all channels, then the bitmap would look like below:
0000000000000000000000000000000000000101010101010000010101010101b

  @param VOID         - None
  @retval VOID        - None

**/
VOID
CreateDimmPresentBitmaps(
   VOID
   )
{
  UINT64       DimmBitmap1;
  UINT64       DimmBitmap2;
  UINT8        Socket, Ch, Dimm;

  DimmBitmap1 = DimmBitmap2 = 0;
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Ch = 0; Ch < MAX_CH; Ch++) {
      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
        if ((IsNvDimm(Socket, Ch, Dimm)) && (mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].Enabled)) {
          if (Socket < 4) {
            DimmBitmap1 |= ((UINT64)0x1 << ((Socket * 16) + (Ch * 2) + Dimm));
            mNgnAcpiSmmInterface->EnergySourcePolicy0 = (mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].EnergyType == 1) ? 0 : 1;
          }
          else {
            DimmBitmap2 |= ((UINT64)0x1 << ((Socket * 16) + (Ch * 2) + Dimm - 64));
            mNgnAcpiSmmInterface->EnergySourcePolicy1 = (mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].EnergyType == 1) ? 0 : 1;
          }
        }
      }
    }
  }
  mNgnAcpiSmmInterface->NvDimmConfigBitmap0 = DimmBitmap1;
  mNgnAcpiSmmInterface->NvDimmConfigBitmap1 = DimmBitmap2;
}

/**

Routine Description: GetDimmInfoFromDevHandle - This function
deconstructs Socket, Channel and Dimm from the Device Handle
that is passed in and returns them in their respective pointers
that are passed in.

  @param *Socket      - pointer to Socket that this function
         will return
  @param *Mc          - pointer to channel that this function
         will return
  @param *Ch          - pointer to channel that this function
         will return
  @param *Dimm        - Pointer to Dimm that this function will
         return
  @param DevHandle    - 32-bit device handle.
  @retval None - void

**/
VOID
GetDimmInfoFromDevHandle(
   UINT8       *Socket,
   UINT8       *Mc,
   UINT8       *Ch,
   UINT8       *Dimm,
   UINT32      DevHandle
   )
{
  // Bit 15:12 of DevHandle contains the socket
  *Socket = (UINT8)((DevHandle >> 12) & 0x0000000F);
  // Bit 11:8 contains the memory controller ID in Nfit DeviceHandle
  *Mc = (UINT8)((DevHandle >> 8) & 0x0000000F);
  // Bit 7:4 contains the channel number
  *Ch = (UINT8)((DevHandle >> 4) & 0x0000000F);
  // Bit 3:0 contains the dimm number
  *Dimm = (UINT8)(DevHandle & 0x0000000F);
}

/**

Routine Description: GetPMEMIntDimmNum - This helper function
returns Dimm Number in the Interleave set of the SAD Rule for a
PMEM Region.

  @param SADBaseAddr - SAD Rule Base Address
  @param Socket    - Socket
  @param Ch        - Channel number
  @retval Interleave Dimm Number

**/
UINT8
GetPMEMIntDimmNum(
   UINT64      SADBaseAddr,
   UINT8       Socket,
   UINT8       Ch
   )
{
  UINT8            i, j;
  PPMEM_INFO       pPMEM = NULL;

  for (i = 0; i < CrInfo.NumPMEMRgns; i++) {
    pPMEM =  &CrInfo.PMEMInfo[i];
    j = i;
    if (pPMEM->SADPMemBase == SADBaseAddr) {
      while (pPMEM->SADPMemBase == SADBaseAddr) {
        if ((pPMEM->Socket == Socket) && (pPMEM->Channel == Ch)) {
          return (pPMEM->IntDimmNum);
        }
        j++;
        pPMEM = &CrInfo.PMEMInfo[j];
      }
    }
  }
  return (0);
}

/**

Routine Description: GetNumDimmsForPMEMRgn - This helper
function returns the total number of Dimms in the Interleave set
of the SAD Rule for a PMEM Region.

  @param SpaStartAddr - Spa Start Address of the SAD Rule
  @retval Total number of dimms in the Interleave set.

**/
UINT8
GetNumDimmsForPMEMRgn(
   UINT64      SADStarAddr
   )
{
  UINT8            i = 0;
  UINT8            DimmCount = 0;
  PPMEM_INFO       pPMEM = NULL;

  for (i = 0; i < CrInfo.NumPMEMRgns; i++) {
    pPMEM =  &CrInfo.PMEMInfo[i];
    if (pPMEM->SADPMemBase == SADStarAddr)
      DimmCount++;
  }
  return (DimmCount);
}

/**

Routine Description: IsPMEMRgn - This helper function
checks to see if the given start address of a SAD rule belongs
to a PMEM Region.

  @param SpaStartAddr - Spa Start Address of the SAD Rule
  @retval TRUE if so, else FALSE

**/
BOOLEAN
IsPMEMRgn(
   UINT64      SpaStartAddr
   )
{
  UINT8            i;
  PPMEM_INFO       pPMEM = NULL;

  for (i = 0; i < CrInfo.NumPMEMRgns; i++) {
    pPMEM =  &CrInfo.PMEMInfo[i];
    if (pPMEM->SADPMemBase == SpaStartAddr)
      return (TRUE);
  }
  return (FALSE);
}

/**

Routine Description: GetSADLimit - This helper function returns
SAD Rule Limit based on the SAD Rule base address passed in.

  @param SADBaseAddr  - Spa Start Address of the SAD Rule
  @param *Limit       - Limit will contain the SAD Rule Limit
  @retval VOID

**/
VOID
GetSADLimit(
   UINT64       SADBaseAddr,
   UINT64       *Limit
   )
{
  UINT8            Socket;
  UINT8            PmemIndex;
  UINT8            i;
  struct SADTable  *pSAD;
  UINT64           SpaStart, SpaEnd;

  //
  // Traverse thru all SAD entries to check for the Spa being in the range of one
  for (PmemIndex = 0; PmemIndex < MAX_PMEM_REGIONS; PmemIndex++) {
    if (mPmemRegions[PmemIndex].Size == 0) continue;
    Socket = mPmemRegions[PmemIndex].SocketNum;
    i = mPmemRegions[PmemIndex].SadIndex;
    pSAD = &mSystemMemoryMap->Socket[Socket].SAD[i];
    if (pSAD->Enable == 0) continue;
    if (pSAD->local == 0) continue;
    if (i == 0) {
      SpaStart = 0;
    } else {
      SpaStart = ((UINT64)mSystemMemoryMap->Socket[Socket].SAD[i - 1].Limit << BITS_64MB_TO_BYTES);
    }
    SpaEnd = ((UINT64)mSystemMemoryMap->Socket[Socket].SAD[i].Limit << BITS_64MB_TO_BYTES);
    if (SADBaseAddr == SpaStart)
      *Limit = SpaEnd;
  }
}

/**

Routine Description: GetPMEMIndexBasedOnSocChDimm - This helper
function returns index of the PMEM Array based on the SAD Start
Address, Socket, Channel and Dimm Info passed in.

  @param SpaStartAddr - Spa Start Address of the SAD Rule
  @param Socket       - Socket Number
  @param Ch           - Channel
  @retval Index of the array, -1 if no match found

**/
INT32
GetPMEMIndexBasedOnSocChDimm(
   UINT64      SpaStartAddr,
   UINT8       Socket,
   UINT8       Ch,
   UINT8       Dimm
   )
{
  INT32            Index = -1;
  UINT32           i;

  for (i = 0; i < CrInfo.NumPMEMRgns; i++) {
    if ((CrInfo.PMEMInfo[i].SADPMemBase == SpaStartAddr) && (CrInfo.PMEMInfo[i].Socket == Socket) && \
        (CrInfo.PMEMInfo[i].Channel == Ch) && (CrInfo.PMEMInfo[i].Dimm == Dimm)) {
      Index = (INT32)i;
      return (Index);
    }
  }
  return (Index);
}



/**

Routine Description: GetPMEMIndex - This helper function returns
index of the PMEM Array based on the SAD Start Address passed
in.

  @param SpaStartAddr - Spa Start Address of the SAD Rule
  @retval Index of the array, -1 if no match found

**/
INT32
GetPMEMIndex(
   UINT64      SpaStartAddr
   )
{
  INT32            Index = -1;
  UINT32           i;

  for (i = 0; i < CrInfo.NumPMEMRgns; i++) {
    if (CrInfo.PMEMInfo[i].SADPMemBase == SpaStartAddr) {
      if (i < (MAX_SYS_DIMM * MAX_PMEM_RGN_PER_DIMM) ) {
        Index = (INT32)i;
        return (Index);
      }
    }
  }
  return (Index);
}

/**

Routine Description: GetChannelGranularity - This function
returns Channel Granularity for interleave as part of the setup
options for the memory configuration.

  @param pSAD      - pointer to the SAD
  @retval ChGran   - Channel Granularity

**/
UINT32
GetChannelGranularity(
   struct SADTable                 *pSAD
   )
{
  UINT32   ChGran;
  UINT8    GranEncoding = 2;

  if (pSAD->type & MEM_TYPE_1LM) {
    switch (pSAD->granularity) {
      case MEM_INT_GRAN_1LM_256B_256B:
        GranEncoding = 1;
        break;
      case MEM_INT_GRAN_1LM_64B_64B:
        GranEncoding = 0;
      break;
    }
  }
  else if (pSAD->type & MEM_TYPE_2LM) {
    switch (pSAD->granularity) {
      case MEM_INT_GRAN_2LM_NM_4KB_256B_FM_4KB:
        GranEncoding = 2;
        break;
      case MEM_INT_GRAN_2LM_NM_4KB_64B_FM_256B:
        GranEncoding = 1;
        break;
      case MEM_INT_GRAN_2LM_NM_4KB_64B_FM_4KB:
        GranEncoding = 2;
        break;
    }
  }

  switch (GranEncoding) {
  case 0:
    ChGran = 64;
    break;
  case 1:
    ChGran = 256;
    break;
  case 2:
    ChGran = 4096;
    break;
  default:
    ChGran = 256;
    break;
  }
  return ChGran;
}

/**

Routine Description: GetChGranularityFromSADBaseAddr - Given a
SAD Base Address, function returns Channel Granularity based on
the SAD Rule.

  @param pSAD      - pointer to the SAD
  @retval ChGran   - Channel Granularity

**/
UINT32
GetChGranularityFromSADBaseAddr(
   UINT64          SADSpaBase
   )
{
  UINT8           Soc, i;
  struct          SADTable *pSAD;
  UINT64          SpaBase;
  UINT32          ChGran = 0;

  for (Soc = 0; Soc < MAX_SOCKET; Soc++) {
    for (i = 0; i < SAD_RULES; i++) {
      pSAD = &mSystemMemoryMap->Socket[Soc].SAD[i];
      if (i == 0) {
        SpaBase = 0;
      }
      else {
        SpaBase = ((UINT64)mSystemMemoryMap->Socket[Soc].SAD[i - 1].Limit << BITS_64MB_TO_BYTES);
      }
      if ((SpaBase == SADSpaBase) && (mSystemMemoryMap->Socket[Soc].SAD[i].local)) {
        ChGran = GetChannelGranularity(pSAD);
        break;
      }
    }
  }
  return (ChGran);
}

/**

Routine Description: VirtualToPhysicalDimmInfo - This function
returns Socket and Channel Information of a dimm given its
virtual socket and channel information that the alogrithm uses.
This routine handles both Control Region and PMEM regions as
well.

  @param SpaStartAddr - Spa Start Address of the SAD Rule
  @param RgnType      - has to be either PMEM or Cntrl Region
  @param ChWays       - Interleave channel ways
  @param Skt          - Virtual Socket number
  @param iMC          - Virtual iMC number
  @param Ch           - Virtual Channel number
  @param *Socket      - This pointer will have socket number
  @param *Channel     - This pointer will hold channel number
  @retval Status      - Status

**/
EFI_STATUS
VirtualToPhysicalDimmInfo(
   UINT64      SpaStartAddr,
   UINT8       RgnType,
   UINT8       ChWays,
   UINT8       Skt,
   UINT8       iMC,
   UINT8       Ch,
   UINT8       *Socket,
   UINT8       *Channel,
   UINT8       *Dimm
   )
{
  UINT32           i;
  UINT64           BaseAddr;
  UINT32           Limit;
  PPMEM_INFO       pPMEM = NULL;
  BOOLEAN          PMem = TRUE;
  EFI_STATUS       Status = EFI_SUCCESS;
  PAEP_DIMM        pDimm = NULL;

  if (RgnType == MEM_TYPE_PMEM) {
    Limit = CrInfo.NumPMEMRgns;
  }
  else if (RgnType == MEM_TYPE_CTRL) {
    Limit = CrInfo.NumAepDimms;
    PMem = FALSE;
  }
  else
    return (EFI_INVALID_PARAMETER);

  for (i = 0; i < Limit; i++) {
    if (RgnType == MEM_TYPE_PMEM) {
      pPMEM =  &CrInfo.PMEMInfo[i];
      BaseAddr = pPMEM->SADPMemBase;
    }
    else {
      pDimm = &CrInfo.AepDimms[i];
      BaseAddr = pDimm->SADSpaBase;
    }
    if (BaseAddr == SpaStartAddr) {
      if ((Skt == 0) && (iMC == 0)) {
        if (ChWays == CH_1WAY) {
          // 1st dimm of the Interleave set
          goto Found;
        }
        else if (ChWays == CH_2WAY) {
          if (Ch == 0) {
            // 1st dimm of the Interleave set
            goto Found;
          }
          else if (Ch == 1) {
            // 2nd dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 1];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 1];
            }
            goto Found;
          }
        }
        else if (ChWays ==  CH_3WAY) {
          if (Ch == 0) {
            // 1st dimm of the Interleave set
            goto Found;
          }
          else if (Ch == 1) {
            // 2nd dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 1];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 1];
            }
            goto Found;
          }
          else if (Ch == 2) {
            // 3rd dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 2];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 2];
            }
            goto Found;
          }
        }
      }
      else if ((Skt == 0) && (iMC == 1)) {
        if (ChWays == CH_1WAY) {
          // 2nd dimm of the Interleave set
          if (PMem) {
            pPMEM = &CrInfo.PMEMInfo[i + 1];
          }
          else {
            pDimm = &CrInfo.AepDimms[i + 1];
          }
          goto Found;
        }
        else if (ChWays == CH_2WAY) {
          if (Ch == 0) {
            // 3rd dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 2];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 2];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 4th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 3];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 3];
            }
            goto Found;
          }
        }
        else if (ChWays ==  CH_3WAY) {
          if (Ch == 0) {
            // 4th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 3];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 3];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 5th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 4];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 4];
            }
            goto Found;
          }
          else if (Ch == 2) {
            // 6th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 5];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 5];
            }
            goto Found;
          }
        }
      }
      else if ((Skt == 1) && (iMC == 0)) {
        if (ChWays == CH_1WAY) {
          // 3rd dimm of the Interleave set
          if (PMem) {
            pPMEM = &CrInfo.PMEMInfo[i + 2];
          }
          else {
            pDimm = &CrInfo.AepDimms[i + 2];
          }
          goto Found;
        }
        else if (ChWays == CH_2WAY) {
          if (Ch == 0) {
            // 5th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 4];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 4];
            }
            goto Found;
          } else if (Ch == 1) {
            // 6th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 5];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 5];
            }
            goto Found;
          }
        }
        else if (ChWays ==  CH_3WAY) {
          if (Ch == 0) {
            // 7th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 6];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 6];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 8th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 7];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 7];
            }
            goto Found;
          }
          else if (Ch == 2) {
            // 9th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 8];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 8];
            }
            goto Found;
          }
        }
      }
      else if ((Skt == 1) && (iMC == 1)) {
        if (ChWays == CH_1WAY) {
          // 4th dimm of the Interleave set
          if (PMem) {
            pPMEM = &CrInfo.PMEMInfo[i + 3];
          }
          else {
            pDimm = &CrInfo.AepDimms[i + 3];
          }
          goto Found;
        }
        else if (ChWays == CH_2WAY) {
          if (Ch == 0) {
            // 7th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 6];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 6];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 8th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 7];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 7];
            }
            goto Found;
          }
        }
        else if (ChWays ==  CH_3WAY) {
          if (Ch == 0) {
            // 10th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 9];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 9];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 11th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 10];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 10];
            }
            goto Found;
          }
          else if (Ch == 2) {
            // 12th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 11];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 11];
            }
            goto Found;
          }
        }
      }
      else if ((Skt == 2) && (iMC == 0)) {
        if (ChWays == CH_1WAY) {
          // 5th dimm of the Interleave set
          if (PMem) {
            pPMEM = &CrInfo.PMEMInfo[i + 4];
          }
          else {
            pDimm = &CrInfo.AepDimms[i + 4];
          }
          goto Found;
        }
        else if (ChWays == CH_2WAY) {
          if (Ch == 0) {
            // 9th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 8];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 8];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 10th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 9];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 9];
            }
            goto Found;
          }
        }
        else if (ChWays ==  CH_3WAY) {
          if (Ch == 0) {
            // 13th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 12];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 12];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 14th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 13];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 13];
            }
            goto Found;
          }
          else if (Ch == 2) {
            // 15th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 14];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 14];
            }
            goto Found;
          }
        }
      }
      else if ((Skt == 2) && (iMC == 1)) {
        if (ChWays == CH_1WAY) {
          // 6th dimm of the Interleave set
          if (PMem) {
            pPMEM = &CrInfo.PMEMInfo[i + 5];
          }
          else {
            pDimm = &CrInfo.AepDimms[i + 5];
          }
          goto Found;
        }
        else if (ChWays == CH_2WAY) {
          if (Ch == 0) {
            // 11th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 10];
            } else {
              pDimm = &CrInfo.AepDimms[i + 10];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 12th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 11];
            } else {
              pDimm = &CrInfo.AepDimms[i + 11];
            }
            goto Found;
          }
        }
        else if (ChWays ==  CH_3WAY) {
          if (Ch == 0) {
            // 16th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 15];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 15];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 17th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 16];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 16];
            }
            goto Found;
          }
          else if (Ch == 2) {
            // 18th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 17];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 17];
            }
            goto Found;
          }
        }
      }
      else if ((Skt == 3) && (iMC == 0)) {
        if (ChWays == CH_1WAY) {
          // 7th dimm of the Interleave set
          if (PMem) {
            pPMEM = &CrInfo.PMEMInfo[i + 6];
          }
          else {
            pDimm = &CrInfo.AepDimms[i + 6];
          }
          goto Found;
        }
        else if (ChWays == CH_2WAY) {
          if (Ch == 0) {
            // 13th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 12];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 12];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 14th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 13];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 13];
            }
            goto Found;
          }
        }
        else if (ChWays ==  CH_3WAY) {
          if (Ch == 0) {
            // 19th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 18];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 18];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 20th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 19];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 19];
            }
            goto Found;
          }
          else if (Ch == 2) {
            // 21st dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 20];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 20];
            }
            goto Found;
          }
        }
      }
      else if ((Skt == 3) && (iMC == 1)) {
        if (ChWays == CH_1WAY) {
          // 8th dimm of the Interleave set
          if (PMem) {
            pPMEM = &CrInfo.PMEMInfo[i + 7];
          }
          else {
            pDimm = &CrInfo.AepDimms[i + 7];
          }
          goto Found;
        }
        else if (ChWays == CH_2WAY) {
          if (Ch == 0) {
            // 15th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 14];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 14];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 16th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 15];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 15];
            }
            goto Found;
          }
        }
        else if (ChWays ==  CH_3WAY) {
          if (Ch == 0) {
            // 22nd dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 21];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 21];
            }
            goto Found;
          }
          else if (Ch == 1) {
            // 23rd dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 22];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 22];
            }
            goto Found;
          }
          else if (Ch == 2) {
            // 24th dimm of the Interleave set
            if (PMem) {
              pPMEM = &CrInfo.PMEMInfo[i + 23];
            }
            else {
              pDimm = &CrInfo.AepDimms[i + 23];
            }
            goto Found;
          }
        }
      }
    }
  }
  //
  // If it gets here, then we'ven't found the details
  Status = EFI_INVALID_PARAMETER;
Found:
  if (PMem) {
    if(pPMEM != NULL) {
      *Socket = pPMEM->Socket;
      *Channel = pPMEM->Channel;
      *Dimm = pPMEM->Dimm;
    }
  }
  else {
    if(pDimm != NULL) {
      *Socket = (UINT8)pDimm->SocketId;
      *Channel = pDimm->Ch;
      *Dimm = pDimm->Dimm;
    }
  }
  return (Status);
}

/**

Routine Description: GetDimmInfoFromSpaFor3ChWays - This
function is responsible for getting Socket, Channel and Dimm
information based on the given System Physical Address when the
Channel Interleave Ways of 3.. That is, to figure out the Nvm
Dimm that is mapped to the passed in Spa. First part of the code
computes the logical Socket & Channel and all ifs and else ifs
are to figure out the actual Socket and the Channel that is in
th data structs.

  @param Spa          - Spa for which the socket, channel & Dimm
                      information.
  @param SpaStartAddr - The Start Address of the SAD Interleave
                      set.
  @param iMCWays      - Number of iMCs participating in the
                      interleave set of the SAD Rule
  @param *Soc         - pointer to Socket that this function
         will return
  @param *Channel     - pointer to channel that this function
         will return
  @param *Dimm        - Pointer to Dimm that this function will
         return
  @retval Status      - Return Status

**/
EFI_STATUS
GetDimmInfoFromSpaFor3ChWays(
   UINT64      Spa,
   UINT64      SpaStartAddr,
   UINT8       iMCWays,
   UINT8       *Soc,
   UINT8       *Channel,
   UINT8       *Dimm
   )
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER;
  UINT64           AddrDiff;
  UINT32           NumLines = 0; // one based
  UINT32           LineSize, iMCSize;
  UINT32           NumRotations, NumBytes;
  UINT32           i;
  UINT8            Socket = 0, Inx = 0, Ch;
  BOOLEAN          iMC, PMemRgn = FALSE;
  // These arrays below helps to determine the start channel when the rotation occurs.
  UINT8            TwoiMCWayCh[6] = { 0, 2, 1, 1, 0, 2 };
  UINT8            FouriMCWayCh[12] = { 0, 1, 2, 1, 2, 0, 2, 0, 1, 0, 1, 2 };
  UINT8            EightiMCWayCh[24] = { 0, 2, 1, 1, 0, 2, 2, 1, 0, 0, 2, 1, 1, 0, 2, 2, 1, 0, 0, 2, 1, 1, 0, 2 };
  //UINT8            SixteeniMCWayCh[48] = { 0, 1, 2, 1, 2, 0, 2, 0, 1, 0, 1, 2, 1, 2, 0, 2, 0, 1, 0, 1, 2, 1, 2, 0, \
  //                                         2, 0, 1, 0, 1, 2, 1, 2, 0, 2, 0, 1, 0, 1, 2, 1, 2, 0, 2, 0, 1, 0, 1, 2 };

  LineSize = GetChGranularityFromSADBaseAddr(SpaStartAddr);
  if (LineSize == 0) {
    DEBUG((EFI_D_ERROR, "JNP: GetDimmInfoFromSpaFor3ChWays - LineSize returned is 0, returning!\n"));
    return (Status);
  }

  iMCSize = CrInfo.SktGran;
  iMC = 0;

  //
  // How far off this Spa from the SAD Spa Start Address?
  AddrDiff = Spa - SpaStartAddr;

  //
  // How many rotations it is going to take get to this Spa from SAD Spa Start?
  NumRotations = (UINT32)(AddrDiff / iMCSize);

  NumLines = ((AddrDiff % iMCSize) / LineSize);

  // These are the number of bytes past the LineSize boundary
  NumBytes = (AddrDiff % LineSize);

  //
  // Does the SAD Start Address belong to PMEM Region?
  PMemRgn = IsPMEMRgn(SpaStartAddr);

  //
  // Now with this we should have the Socket information
  switch (iMCWays) {
  case ONE_IMC_WAY:
    // iMCWays is 1 and 3ch ways
    NumLines = (UINT32)(AddrDiff / LineSize);
    Ch = 0;
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 2)
        Ch = 0;
    }
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_3WAY, 0, 0, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_3WAY, 0, 0, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  case TWO_IMC_WAY:
    // If iMCWays = 2, then it is only one socket, socket 0.
    if ((NumRotations % iMCWays)) {
      // If so it is in iMC1
      iMC = TRUE;
    }
    Inx = ((NumRotations  % (CH_3WAY * iMCWays)) / iMCWays);
    if (iMC) Inx += 3;
    Ch = TwoiMCWayCh[Inx];
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 2)
        Ch = 0;
    }
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_3WAY, 0, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_3WAY, 0, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  case FOUR_IMC_WAY:
    // 4 iMC Ways interleave,
    if (((NumRotations % iMCWays) == 2) || ((NumRotations % iMCWays) == 3)) {
      Socket = 1;
      Inx = 6;
      if ((NumRotations % iMCWays) == 3) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 0) || ((NumRotations % iMCWays) == 1)) {
      Socket = 0;
      Inx = 0;
      if ((NumRotations % iMCWays) == 1) {
        iMC = TRUE;
      }
    }
    Inx += ((NumRotations  % (CH_3WAY * iMCWays)) / iMCWays);
    if (iMC) Inx += 3;
    Ch = FouriMCWayCh[Inx];
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 2)
        Ch = 0;
    }
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_3WAY, Socket, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_3WAY, Socket, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  case EIGHT_IMC_WAY:
    // 8 iMC Way interleave
    if (((NumRotations % iMCWays) == 6) || ((NumRotations % iMCWays) == 7)) {
      Inx = 18;
      Socket = 3;
      if ((NumRotations % iMCWays) == 7) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 4) || ((NumRotations % iMCWays) == 5)) {
      Socket = 2;
      Inx = 12;
      if ((NumRotations % iMCWays) == 5) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 2) || ((NumRotations % iMCWays) == 3)) {
      Socket = 1;
      Inx = 6;
      if ((NumRotations % iMCWays) == 3) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 0) || ((NumRotations % iMCWays) == 1)) {
      Socket = 0;
      Inx = 0;
      if ((NumRotations % iMCWays) == 1) {
        iMC = TRUE;
      }
    }
    Inx += ((NumRotations  % (CH_3WAY * iMCWays)) / iMCWays);
    if (iMC) Inx += 3;
    Ch = EightiMCWayCh[Inx];
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 2)
        Ch = 0;
    }
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_3WAY, Socket, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_3WAY, Socket, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  }
  //
  // If it gets here, return EFI_INVALID_PARAMETER Status.
  return (Status);
}

/**

Routine Description: GetDimmInfoFromSpaFor2ChWays - This
function is responsible for getting Socket, Channel and Dimm
information based on the given System Physical Address when the
Channel Interleave Ways of 2.. That is, to figure out the Nvm
Dimm that is mapped to the passed in Spa. First part of the code
computes the logical Socket & Channel and all ifs and else ifs
are to figure out the actual Socket and the Channel that is in
th data structs.

  @param Spa          - Spa for which the socket, channel & Dimm
                      information.
  @param SADSpaStart  - The Start Address of the SAD Interleave
                      set.
  @param iMCWays      - Number of iMCs participating in the
                      interleave set of the SAD Rule
  @param *Soc         - pointer to Socket that this function
         will return
  @param *Channel     - pointer to channel that this function
         will return
  @param *Dimm        - Pointer to Dimm that this function will
         return
  @retval Status      - Return Status

**/
EFI_STATUS
GetDimmInfoFromSpaFor2ChWays(
   UINT64      Spa,
   UINT64      SpaStartAddr,
   UINT8       iMCWays,
   UINT8       *Soc,
   UINT8       *Channel,
   UINT8       *Dimm
   )
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER;
  UINT64           AddrDiff;
  UINT32           NumLines = 0; // one based
  UINT32           LineSize, iMCSize;
  UINT32           NumRotations, NumBytes;
  UINT32           i;
  UINT8            Socket = 0, Ch;
  BOOLEAN          iMC, PMemRgn = FALSE;

  LineSize = GetChGranularityFromSADBaseAddr(SpaStartAddr);
  if (LineSize == 0) {
    DEBUG((EFI_D_ERROR, "JNP: GetDimmInfoFromSpaFor2ChWays - LineSize returned is 0, returning!\n"));
    return (Status);
  }

  iMCSize = CrInfo.SktGran;
  iMC = 0;

  //
  // How far off this Spa from the SAD Spa Start Address?
  AddrDiff = Spa - SpaStartAddr;

  //
  // How many rotations it is going to take get to this Spa from SAD Spa Start?
  NumRotations = (UINT32)(AddrDiff / iMCSize);

  NumLines = ((AddrDiff % iMCSize) / LineSize);

  // These are the number of bytes past the LineSize boundary
  NumBytes = (AddrDiff % LineSize);

  //
  // Does the SAD Start Address belong to PMEM Region?
  PMemRgn = IsPMEMRgn(SpaStartAddr);

  //
  // Now with this we should have the Socket information
  switch (iMCWays) {
  case ONE_IMC_WAY:
    NumLines = (UINT32)(AddrDiff / LineSize);
    Ch = 0;
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 1) Ch = 0;
    }
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_2WAY, 0, 0, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_2WAY, 0, 0, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  case TWO_IMC_WAY:
    if ((NumRotations % iMCWays)) {
      // If it so, then pertinent information is in the second 2 dimms of the set of 4 Dimm AEP_DIMM struct.
      iMC = TRUE;
    }
    Ch = 0;
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 1)
        Ch = 0;
    }
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_2WAY, 0, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_2WAY, 0, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  case FOUR_IMC_WAY:
    // 4 iMC Ways interleave,
    if (((NumRotations % iMCWays) == 2) || ((NumRotations % iMCWays) == 3)) {
      Socket = 1;
      if ((NumRotations % iMCWays) == 3) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 0) || ((NumRotations % iMCWays) == 1)) {
      Socket = 0;
      if ((NumRotations % iMCWays) == 1) {
        iMC = TRUE;
      }
    }
    Ch = 0;
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 1)
        Ch = 0;
    }
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_2WAY, Socket, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_2WAY, Socket, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  case EIGHT_IMC_WAY:
    // 8 iMC Way interleave
    if (((NumRotations % iMCWays) == 6) || ((NumRotations % iMCWays) == 7)) {
      Socket = 3;
      if ((NumRotations % iMCWays) == 7) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 4) || ((NumRotations % iMCWays) == 5)) {
      Socket = 2;
      if ((NumRotations % iMCWays) == 5) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 2) || ((NumRotations % iMCWays) == 3)) {
      Socket = 1;
      if ((NumRotations % iMCWays) == 3) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 0) || ((NumRotations % iMCWays) == 1)) {
      Socket = 0;
      if ((NumRotations % iMCWays) == 1) {
        iMC = TRUE;
      }
    }
    Ch = 0;
    for (i = 0; i < NumLines; i++) {
      Ch++;
      if (Ch > 1)
        Ch = 0;
    }
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_2WAY, Socket, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_2WAY, Socket, iMC, Ch, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  }
  //
  // If it gets here, return EFI_INVALID_PARAMETER Status.
  return (Status);
}

/**

Routine Description: GetDimmInfoFromSpaFor1ChWays - This
function is responsible for getting Socket, Channel and Dimm
information based on the given System Physical Address when the
Channel Interleave Ways of 1.. That is, to figure out the Nvm
Dimm that is mapped to the passed in Spa. First part of the code
computes the logical Socket & Channel and all ifs and else ifs
are to figure out the actual Socket and the Channel that is in
th data structs.

  @param Spa          - Spa for which the socket, channel & Dimm
                      information.
  @param SADSpaStart  - The Start Address of the SAD Interleave
                      set.
  @param iMCWays      - Number of iMCs participating in the
                      interleave set of the SAD Rule
  @param *Soc         - pointer to Socket that this function
         will return
  @param *Channel     - pointer to channel that this function
         will return
  @param *Dimm        - Pointer to Dimm that this function will
         return
  @retval Status      - Return Status

**/
EFI_STATUS
GetDimmInfoFromSpaFor1ChWays(
   UINT64      Spa,
   UINT64      SpaStartAddr,
   UINT8       iMCWays,
   UINT8       *Soc,
   UINT8       *Channel,
   UINT8       *Dimm
   )
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER;
  UINT64           AddrDiff;
  UINT32           NumLines = 0; // one based
  UINT32           LineSize, iMCSize;
  UINT32           NumRotations, NumBytes;
  UINT8            Socket = 0;
  BOOLEAN          iMC, PMemRgn = FALSE;

  LineSize = GetChGranularityFromSADBaseAddr(SpaStartAddr);
  if (LineSize == 0) {
    DEBUG((EFI_D_ERROR, "JNP: GetDimmInfoFromSpaFor1ChWays - LineSize returned is 0, returning!\n"));
    return (Status);
  }

  iMCSize = CrInfo.SktGran;
  iMC = 0;

  //
  // How far off this Spa from the SAD Spa Start Address?
  AddrDiff = Spa - SpaStartAddr;

  //
  // How many rotations it is going to take get to this Spa from SAD Spa Start?
  NumRotations = (UINT32)(AddrDiff / iMCSize);

  NumLines = ((AddrDiff % iMCSize) / LineSize);

  // These are the number of bytes past the LineSize boundary
  NumBytes = (AddrDiff % LineSize);

  //
  // Does the SAD Start Address belong to PMEM Region?
  PMemRgn = IsPMEMRgn(SpaStartAddr);

  //
  // Now with this we should have the Socket information
  switch (iMCWays) {
  case ONE_IMC_WAY:
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_1WAY, 0, 0, 0, Soc, Channel, Dimm);
      return (Status);
    }
    else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_1WAY, 0, 0, 0, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  case TWO_IMC_WAY:
    // 2 iMC Ways
    if ((NumRotations % iMCWays)) {
      // If so it is in iMC1
      iMC = TRUE;
    }
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_1WAY, 0, iMC, 0, Soc, Channel, Dimm);
      return (Status);
    }
    else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_1WAY, 0, iMC, 0, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  case FOUR_IMC_WAY:
    // Four iMC Ways
    if (((NumRotations % iMCWays) == 2) || ((NumRotations % iMCWays) == 3)) {
      Socket = 1;
      if ((NumRotations % iMCWays) == 3) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 0) || ((NumRotations % iMCWays) == 1)) {
      Socket = 0;
      if ((NumRotations % iMCWays) == 1) {
        iMC = TRUE;
      }
    }
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_1WAY, Socket, iMC, 0, Soc, Channel, Dimm);
      return (Status);
    }
    else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_1WAY, Socket, iMC, 0, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  case EIGHT_IMC_WAY:
    // 8 iMC Way interleave
    if (((NumRotations % iMCWays) == 6) || ((NumRotations % iMCWays) == 7)) {
      Socket = 3;
      if ((NumRotations % iMCWays) == 7) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 4) || ((NumRotations % iMCWays) == 5)) {
      Socket = 2;
      if ((NumRotations % iMCWays) == 5) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 2) || ((NumRotations % iMCWays) == 3)) {
      Socket = 1;
      if ((NumRotations % iMCWays) == 3) {
        iMC = TRUE;
      }
    }
    else if (((NumRotations % iMCWays) == 0) || ((NumRotations % iMCWays) == 1)) {
      Socket = 0;
      if ((NumRotations % iMCWays) == 1) {
        iMC = TRUE;
      }
    }
    // Need to get the actual mapped channel
    if (PMemRgn) {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_PMEM, CH_1WAY, Socket, iMC, 0, Soc, Channel, Dimm);
      return (Status);
    } else {
      Status = VirtualToPhysicalDimmInfo(SpaStartAddr, MEM_TYPE_CTRL, CH_1WAY, Socket, iMC, 0, Soc, Channel, Dimm);
      return (Status);
    }
    break;
  }
  //
  // If it gets here, return EFI_INVALID_PARAMETER Status.
  return (Status);
}

/**

Routine Description: GetDimmInfoFromSpa - This function is
responsible for getting Socket, Channel and Dimm information
based on the given System Physical Address. That is, to figure
out the Nvm Dimm that is mapped to the passed in Spa.

  @param Spa          - Spa
  @param ArsLength    -
  @param *Soc         - pointer to Socket that this function
         will return
  @param *Channel     - pointer to channel that this function
         will return
  @param *Dimm        - Pointer to Dimm that this function will
         return
  @retval Status      - Return Status

**/
EFI_STATUS
GetDimmInfoFromSpa(
   UINT64      Spa,
   UINT64      *SADSpaStart,
   UINT8       *Soc,
   UINT8       *Channel,
   UINT8       *Dimm
   )
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER;
  UINT8            Socket;
  UINT8            PmemIndex;
  UINT8            iMCWays, ChWays, i;
  UINT64           SpaStart, SpaEnd;
  struct SADTable  *pSAD;
  UINT8            ChInterBitmap;

  //
  // Traverse thru all SAD entries to check for the Spa being in the range of one
  for (PmemIndex = 0; PmemIndex < MAX_PMEM_REGIONS; PmemIndex++) {
    if (mPmemRegions[PmemIndex].Size == 0) continue;
    Socket = mPmemRegions[PmemIndex].SocketNum;
    i = mPmemRegions[PmemIndex].SadIndex;
    pSAD = &mSystemMemoryMap->Socket[Socket].SAD[i];
    ChInterBitmap = GetChInterBitmap(pSAD);
    if (pSAD->Enable == 0) continue;
    if (pSAD->local == 0) continue;
    if (i == 0) {
      SpaStart = 0;
    }
    else {
      SpaStart = ((UINT64)mSystemMemoryMap->Socket[Socket].SAD[i - 1].Limit << BITS_64MB_TO_BYTES);
    }
    SpaEnd = ((UINT64)mSystemMemoryMap->Socket[Socket].SAD[i].Limit << BITS_64MB_TO_BYTES);

    //
    // If the passed in SPA falls with in this SAD SPA Range
    if ((Spa >= SpaStart) && (Spa <= SpaEnd)) {
      //
      // Found the Sad rule with in which the passed in Spa has mapping, now next to figure out to which dimm
      // this Spa is mapped into.
      //
      // One of the return values is SADSpaStart address
      *SADSpaStart = SpaStart;
      iMCWays = GetiMCWays(Socket, i);
      // Get the Channel ways for this SAD
      if (ChInterBitmap == 0x7)
        ChWays = CH_3WAY;
      else if ((ChInterBitmap == 0x5) || (ChInterBitmap == 0x6) || (ChInterBitmap == 0x3)) {
        ChWays = CH_2WAY;
      }
      else
        ChWays = CH_1WAY;

      switch (ChWays) {
      case CH_3WAY:
        Status = GetDimmInfoFromSpaFor3ChWays(Spa, SpaStart, iMCWays, Soc, Channel, Dimm);
        break;
      case CH_2WAY:
        Status = GetDimmInfoFromSpaFor2ChWays(Spa, SpaStart, iMCWays, Soc, Channel, Dimm);
        break;
      case CH_1WAY:
        Status = GetDimmInfoFromSpaFor1ChWays(Spa, SpaStart, iMCWays, Soc, Channel, Dimm);
        break;
      }
    }
  }

  if (Status ==  EFI_SUCCESS) {
#if VERBOSE_DEBUG
    DEBUG((EFI_D_ERROR, "JNP: GetDimmInfoFromSpa - Found Dimm Info From Spa (0x%llx), Soc = %d, Channel = %d, Dimm = %d\n", Spa, *Soc, *Channel, *Dimm));
#endif
  }
  else {
    DEBUG((EFI_D_ERROR, "JNP: GetDimmInfoFromSpa - Failed\n"));

  }
  return (Status);
}

//
// Note: The code from all translation routines that converts Dpa to Spa (and vice-versa),
// have been tested with another tool and all known issues have been addressed with these
// functions.
//

/**

Routine Description: GetSocketGranularity - This function
returns Socket Granularity for interleave (which is really iMC
Granularity) as part of the setup options for the memory
configuration.

  @param pSAD      - pointer to the SAD
  @retval SktGran  - Socket Granularity

**/
UINT32
GetSocketGranularity(
   struct SADTable                 *pSAD
   )
{
  UINT32   SktGran;
  UINT8    GranEncoding = 1;

  if (pSAD->type & MEM_TYPE_1LM) {
    switch (pSAD->granularity) {
      case MEM_INT_GRAN_1LM_256B_256B:
        GranEncoding = 1;
        break;
      case MEM_INT_GRAN_1LM_64B_64B:
        GranEncoding = 0;
        break;
    }
  }
  else if (pSAD->type & MEM_TYPE_2LM) {
    switch (pSAD->granularity) {
      case MEM_INT_GRAN_2LM_NM_4KB_256B_FM_4KB:
        // Fall thru
      case MEM_INT_GRAN_2LM_NM_4KB_64B_FM_256B:
        // Fall thru
      case MEM_INT_GRAN_2LM_NM_4KB_64B_FM_4KB:
        GranEncoding = 2;
        break;
    }
  }

  switch (GranEncoding) {
  case 0:
    SktGran = 64;
    break;
  case 1:
    SktGran = 256;
    break;
  case 2:
    SktGran = 4096;
    break;
  default:
    SktGran = 256;
    break;
  }
  return SktGran;
}

UINT8
GetChWays(
    UINT8 socket,
    UINT8 SadIndex
    )
{
  UINT8  ChWays, i, McIndex;
  UINT8  ChInter = 0;

  McIndex = GetMcIndexFromBitmap (mSystemMemoryMap->Socket[socket].SAD[SadIndex].imcInterBitmap);

  switch (mSystemMemoryMap->Socket[socket].SAD[SadIndex].type)  {
    case MEM_TYPE_1LM:
      ChInter = mSystemMemoryMap->Socket[socket].SAD[SadIndex].channelInterBitmap[McIndex];
      break;
    case MEM_TYPE_PMEM:
    case MEM_TYPE_2LM:
    case MEM_TYPE_PMEM_CACHE:
    case MEM_TYPE_BLK_WINDOW:
    case MEM_TYPE_CTRL:
      ChInter = mSystemMemoryMap->Socket[socket].SAD[SadIndex].FMchannelInterBitmap[McIndex];
      break;
  }

  ChWays = 0;
  for (i = 0; i < MAX_MC_CH; i++) {
    if (((ChInter >> i) & 0x1) ==1) {
      ChWays++;
    }
  }
  return ChWays;
}

/**

Routine Description: GetDpaFor1WayChInt - This function converts
given Spa to Dpa in the case of the Dimm(s) involved in 1-way
Channel interleaved for up to 8 iMC Ways.

  @param pDimm       - Pointer to Dimm structure
  @param Spa         - Spa for which we need the DPA
  @param SADSpaStart - Start Address for SAD Rule interleave set
  @param *pDpa       - Pointer to the Dpa that this function
                       returns.
  @retval EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
GetDpaFor1WayChInt(
   AEP_DIMM       *pDimm,
   UINT64          Spa,
   UINT64          SADSpaStart,
   UINT64          *pDpa
   )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  UINT64           RotationSize, NumRotation, SpaStart;
  UINT32           SpaIndex, LineNum;
  UINT32           NumLinesDesc;
  UINT32           LineIndexSize = 0;
  UINT32           LineSize, iMCSize;
  INT32            Index;
  UINT8            i, iMCWays;

  if (SADSpaStart == pDimm->SADSpaBase) {
    // It is Control Region for translation
    iMCWays = pDimm->iMCWays;
    SpaStart = pDimm->SADSpaBase;
    i = pDimm->IntDimmNum;
  }
  else {
    Index = GetPMEMIndex(SADSpaStart);
    if (Index == -1) {
      return (EFI_INVALID_PARAMETER);
    }
    else {
      // 1st PMEM Region of this dimm
      iMCWays = CrInfo.PMEMInfo[Index].iMCWays;
      SpaStart = SADSpaStart;
      i = CrInfo.PMEMInfo[Index].IntDimmNum;
    }
  }

  // Check if start of the NVDIMM
  if ((Spa - SpaStart) == 0) {
    *pDpa = 0;
    return Status;
  }

  iMCSize = CrInfo.SktGran;
  LineSize = GetChGranularityFromSADBaseAddr(SADSpaStart);
  if (LineSize == 0)
    return (EFI_INVALID_PARAMETER);

  if(iMCSize == LineSize) {
    NumLinesDesc = 2;
  } else {
    NumLinesDesc = iMCSize / LineSize;
  }

  RotationSize = LineSize * NumLinesDesc;
  NumRotation = (UINT32)((Spa - SpaStart) / (RotationSize * iMCWays));
  SpaIndex = (UINT32)((Spa - SpaStart) % (Spa - SpaStart));
  //
  // Need the Line Number and for that need to locate SpaIndex in the table
  LineIndexSize = iMCSize * i;
  for (LineNum = 0; LineNum <  NumLinesDesc; LineNum++) {
    if (SpaIndex == LineIndexSize)
      break;
    LineIndexSize += LineSize;
  }

  // Now we have everything to compute the Dpa
  *pDpa = (LineNum * LineSize) + (NumRotation * RotationSize) + (Spa % LineSize);

  return (Status);
}

/**

Routine Description: GetDpaFor2WayChInt - This function converts
given Spa to Dpa in the case of the Dimm(s) involved in 2-way
Channel interleaved for up to 8 iMC Ways.

  @param pDimm       - Pointer to Dimm structure
  @param Spa         - Spa for which we need the DPA
  @param SADSpaStart - Start Address for SAD Rule interleave set
  @param *pDpa       - Pointer to the Dpa that this function
                       returns.
  @retval EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
GetDpaFor2WayChInt(
   AEP_DIMM       *pDimm,
   UINT64          Spa,
   UINT64          SADSpaStart,
   UINT64          *pDpa
   )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT64      RotationSize;
  UINT64      NumRotation;
  UINT64      SpaStart;
  UINT32      SpaIndex;
  UINT32      LineNum = 0;
  UINT32      NumLinesDesc;
  UINT32      LineIndexSize = 0;
  UINT32      LineSize;
  UINT32      iMCSize;
  UINT32      *LineMultiplier;
  INT32       Index;
  UINT8       i;
  UINT8       iMCWays;

  if (SADSpaStart == pDimm->SADSpaBase) {
    // It is Control Region for translation
    iMCWays = pDimm->iMCWays;
    SpaStart = pDimm->SADSpaBase;
    i = pDimm->IntDimmNum;
  }
  else {
    Index = GetPMEMIndex(SADSpaStart);
    if (Index == -1) {
      return (EFI_INVALID_PARAMETER);
    }
    else {
      // 1st PMEM Region of this dimm
      iMCWays = CrInfo.PMEMInfo[Index].iMCWays;
      SpaStart = SADSpaStart;
      i = CrInfo.PMEMInfo[Index].IntDimmNum;
    }
  }

  // Check if start of the NVDIMM
  if ((Spa - SpaStart) == 0) {
    *pDpa = 0;
    return Status;
  }

  iMCSize = CrInfo.SktGran;
  LineSize = GetChGranularityFromSADBaseAddr(SADSpaStart);
  if (LineSize == 0)
    return (EFI_INVALID_PARAMETER);

  if(iMCSize == LineSize) {
    NumLinesDesc = 2;
  } else {
    NumLinesDesc = iMCSize / (LineSize * CH_2WAY);
  }

  RotationSize = LineSize * NumLinesDesc;
  NumRotation = (UINT32)((Spa - SpaStart) / (RotationSize * iMCWays * CH_2WAY));
  SpaIndex = (UINT32)((Spa - SpaStart) % (RotationSize * iMCWays * CH_2WAY));

  switch (iMCWays) {
  case ONE_IMC_WAY:
    if(iMCSize == LineSize) {
      LineMultiplier = &iMC1Way4k_2ChWay4k_LineMultipliers[i][0];
    } else {
      LineMultiplier = &iMC1Way4k_2ChWay256_LineMultipliers[i][0];
    }
    break;
  case TWO_IMC_WAY:
    if(iMCSize == LineSize) {
      LineMultiplier = &iMC2Way4k_2ChWay4k_LineMultipliers[i][0];
    } else {
      LineMultiplier = &iMC2Way4k_2ChWay256_LineMultipliers[i][0];
    }
    break;
  case FOUR_IMC_WAY:
    if(iMCSize == LineSize) {
      LineMultiplier = &iMC4Way4k_2ChWay4k_LineMultipliers[i][0];
    } else {
      LineMultiplier = &iMC4Way4k_2ChWay256_LineMultipliers[i][0];
    }
    break;
  case EIGHT_IMC_WAY:
    // 8 iMC Way not supported for 4k/4k
    LineMultiplier = &iMC8Way4k_2ChWay256_LineMultipliers[i][0];
    break;
  default:
    DEBUG ((EFI_D_ERROR, "\n*** ERROR. Invalid 'iMCSize' value, current value is 0x%X ***\n", iMCSize));
    return EFI_UNSUPPORTED;
  }

  LineIndexSize = *LineMultiplier * LineSize;
  for (LineNum = 0; LineNum <  NumLinesDesc; LineNum++) {
    if (SpaIndex == LineIndexSize)
      break;
    LineIndexSize += (LineMultiplier[LineNum] * LineSize);
  }

    // Now we have everything to compute the Dpa
  *pDpa = (LineNum * LineSize) + (NumRotation * RotationSize) + (Spa % LineSize);

  return (Status);
}

/**

Routine Description: GetDpaFor3WayChInt - This function converts
given Spa to Dpa in the cases of the Dimms involved in 3-way
Channel interleaved for up to 8 iMC Ways.

  @param pDimm       - Pointer to Dimm structure
  @param Spa         - Spa for which we need the DPA
  @param SADSpaStart - Start Address for SAD Rule interleave set
  @param *pDpa       - Pointer to the Dpa that this function
                       returns.
  @retval EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
GetDpaFor3WayChInt(
   AEP_DIMM       *pDimm,
   UINT64          Spa,
   UINT64          SADSpaStart,
   UINT64          *pDpa
   )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT64      RotationSize;
  UINT64      NumRotation;
  UINT64      SpaStart;
  UINT32      SpaIndex;
  UINT32      LineNum = 0;
  UINT32      NumLinesDesc;
  UINT32      LineIndexSize = 0;
  UINT32      LineSize;
  UINT32      iMCSize;
  UINT32      *LineMultiplier;
  INT32       Index;
  UINT8       i;
  UINT8       iMCWays;

  if (SADSpaStart == pDimm->SADSpaBase) {
    // It is Control Region for translation
    iMCWays = pDimm->iMCWays;
    SpaStart = pDimm->SADSpaBase;
    i = pDimm->IntDimmNum;
  }
  else {
    Index = GetPMEMIndex(SADSpaStart);
    if (Index == -1) {
      return (EFI_INVALID_PARAMETER);
    }
    else {
      // 1st PMEM Region of this dimm
      iMCWays = CrInfo.PMEMInfo[Index].iMCWays;
      SpaStart = SADSpaStart;
      i = CrInfo.PMEMInfo[Index].IntDimmNum;
    }
  }

  // Check if start of the NVDIMM
  if ((Spa - SpaStart) == 0) {
    *pDpa = 0;
    return Status;
  }

  iMCSize = CrInfo.SktGran;
  LineSize = GetChGranularityFromSADBaseAddr(SADSpaStart);
  if (LineSize == 0)
      return (EFI_INVALID_PARAMETER);

  if(iMCSize == LineSize) {
    NumLinesDesc = 2;
  } else {
    NumLinesDesc = iMCSize / LineSize;
  }

  RotationSize = LineSize * NumLinesDesc;
  NumRotation = (UINT32)((Spa - SpaStart) / (RotationSize * iMCWays * CH_3WAY));
  SpaIndex = (UINT32)((Spa - SpaStart) % (RotationSize * iMCWays * CH_3WAY));

  switch (iMCWays) {
  case ONE_IMC_WAY:
    if(iMCSize == LineSize) {
      LineMultiplier = &iMC1Way4k_3ChWay4k_LineMultipliers[i][0];
    } else {
      LineMultiplier = &iMC1Way4k_3ChWay256_LineMultipliers[i][0];
    }
    break;
  case TWO_IMC_WAY:
    if(iMCSize == LineSize) {
      LineMultiplier = &iMC2Way4k_3ChWay4k_LineMultipliers[i][0];
    } else {
      LineMultiplier = &iMC2Way4k_3ChWay256_LineMultipliers[i][0];
    }
    break;
  case FOUR_IMC_WAY:
    if(iMCSize == LineSize) {
      LineMultiplier = &iMC4Way4k_3ChWay4k_LineMultipliers[i][0];
    } else {
      LineMultiplier = &iMC4Way4k_3ChWay256_LineMultipliers[i][0];
    }
    break;
  case EIGHT_IMC_WAY:
    if(iMCSize == LineSize) {
      LineMultiplier = &iMC8Way4k_3ChWay4k_LineMultipliers[i][0];
    } else {
      LineMultiplier = &iMC8Way4k_3ChWay256_LineMultipliers[i][0];
    }
    break;
  default:
    DEBUG ((EFI_D_ERROR, "\n*** ERROR. Invalid 'iMCSize' value, current value is 0x%X ***\n", iMCSize));
    return EFI_UNSUPPORTED;
  }

  LineIndexSize = *LineMultiplier * LineSize;
  for (LineNum = 0; LineNum <  NumLinesDesc; LineNum++) {
    if (SpaIndex == LineIndexSize)
      break;
    LineIndexSize += (LineMultiplier[LineNum] * LineSize);
  }

  // Now we have everything to compute the Dpa
  *pDpa = (LineNum * LineSize) + (NumRotation * RotationSize) + (Spa % LineSize);

  return (Status);
}

/**

Routine Description: XlateSpaToDpa - This function converts
given Spa to Dpa in the case of the Dimm involved in all Channel
ways and up to 8 iMC Ways.

  @param pDimm       - Pointer to Dimm structure
  @param Spa         - Spa for which we need the DPA
  @param SADSpaStart - SAD Base Address
  @param *pDpa       - Pointer to the Dpa that this function
                       returns.
  @retval EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
XlateSpaToDpa(
   PAEP_DIMM       pDimm,
   UINT64          Spa,
   UINT64          SADSpaStart,
   UINT64          *pDpa
   )
{
  EFI_STATUS       Status = EFI_SUCCESS;

  switch (pDimm->ChWays) {
  case CH_1WAY:
    Status = GetDpaFor1WayChInt(pDimm, Spa, SADSpaStart, pDpa);
    break;
  case CH_2WAY:
    Status = GetDpaFor2WayChInt(pDimm, Spa, SADSpaStart, pDpa);
    break;
  case CH_3WAY:
    Status = GetDpaFor3WayChInt(pDimm, Spa, SADSpaStart, pDpa);
    break;
  }
  return Status;
}

/**
 * Routine Description: SpaToDpa - This function converts given Dpa
to Spa and also populates the socket channel and dimm information with
only system phy address as the input

  @param Spa        - System Physical address SPA to be translated
  @param Skt        - socket number of the given SPA
  @param ch         - channel number of the given SPA
  @param Dimm       - dimm corresponding to the given SPA
  @param pDpa       - Device physical address
  @retval EFI_STATUS - status of the conversion effort
 */
EFI_STATUS
SpaToDpa(
   UINT64       Spa,
   UINT8        *Skt,
   UINT8        *Ch,
   UINT8        *Dimm,
   UINT64       *pDpa
   )
{
  UINT64        SpaStart;
  EFI_STATUS    Status = EFI_NOT_FOUND;
  PAEP_DIMM     pDimm = NULL;

  Status = GetDimmInfoFromSpa(Spa, &SpaStart, Skt, Ch, Dimm);
  if (Status == EFI_SUCCESS) {
    pDimm = GetDimm(*Skt, *Ch, *Dimm);
  }

  if (pDimm == NULL) {
    Status = EFI_NOT_FOUND;
    return Status;
  }

  Status = XlateSpaToDpa(pDimm, Spa, SpaStart, pDpa);
  return Status;
}

/**

Routine Description: GetSpaFor3WayChInt - This function converts
given Dpa to Spa in the case of 3 way Channel Interleaved and up
to 16 iMC Ways.

  @param pDimm       - Pointer to Dimm structure
  @param SADBaseAddr - SAD Rule Base Address
  @param Dpa         - Dpa for which we need the SPA
  @param *pSpa       - Pointer to the Spa.
  @retval EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
GetSpaFor3WayChInt(
   PAEP_DIMM   pDimm,
   UINT64      SADBaseAddr,
   UINT64      Dpa,
   UINT64      *pSpa
   )
{
  UINT32           NumLines;
  EFI_STATUS       Status = EFI_SUCCESS;
  UINT64           RotationSize, NumRotation;
  UINT32           NumLinesDesc;
  UINT32           LineIndexSize = 0;
  UINT32           LineSize, iMCSize;
  UINT8            i, iMCWays;
  BOOLEAN          PMEMRgn =  FALSE;

  if (IsPMEMRgn(SADBaseAddr)) {
    LineSize = GetChGranularityFromSADBaseAddr(SADBaseAddr);
    // If PMEM Region, need to get the Interleave Dimm Number from the PMEMInfo struct
    i = CrInfo.PMEMInfo[mPmemIndex].IntDimmNum;
    PMEMRgn = TRUE;
  }
  else {
    i = pDimm->IntDimmNum;
    LineSize = CrInfo.ChGran;
  }
  iMCSize = CrInfo.SktGran;
  iMCWays = pDimm->iMCWays;
  if (iMCSize == LineSize) {
    NumLinesDesc = 2;
  } else {
    NumLinesDesc = iMCSize / LineSize;
  }

#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "JNP: LineSize = %d, iMCSize = %d, iMCWays = %d\n", LineSize, iMCSize, iMCWays));
  DEBUG((EFI_D_ERROR, "JNP: DPA = 0x%llx\n", Dpa));
#endif

  RotationSize = LineSize * NumLinesDesc;
  NumRotation = Dpa / RotationSize;
  NumLines = (UINT32)((Dpa % RotationSize) / LineSize);

  switch (iMCWays) {
  case ONE_IMC_WAY:
    // For 1 iMCWay Interleave Interleave Dimm Number should be < 3.
    ASSERT(i < (ONE_IMC_WAY * MAX_MC_CH));
    if (iMCSize == LineSize) {
      LineIndexSize = (iMC1Way4k_3ChWay4k_LineMultipliers[i][NumLines] * LineSize);
    } else {
      LineIndexSize = (iMC1Way4k_3ChWay256_LineMultipliers[i][NumLines] * LineSize);
    }
    break;
  case TWO_IMC_WAY:
    // For 2 iMCWay Interleave Interleave Dimm Number should be < 6.
    ASSERT(i < (TWO_IMC_WAY * MAX_MC_CH));
    if (iMCSize == LineSize) {
      LineIndexSize = (iMC2Way4k_3ChWay4k_LineMultipliers[i][NumLines] * LineSize);
    } else {
      LineIndexSize = (iMC2Way4k_3ChWay256_LineMultipliers[i][NumLines] * LineSize);
    }
    break;
  case FOUR_IMC_WAY:
    // For 4 iMCWay Interleave Interleave Dimm Number should be < 12.
    ASSERT(i < (FOUR_IMC_WAY * MAX_MC_CH));
    if (iMCSize == LineSize) {
      LineIndexSize = (iMC4Way4k_3ChWay4k_LineMultipliers[i][NumLines] * LineSize);
    } else {
      LineIndexSize = (iMC4Way4k_3ChWay256_LineMultipliers[i][NumLines] * LineSize);
    }
    break;
  case EIGHT_IMC_WAY:
    // For  iMCWay Interleave Interleave Dimm Number should be < 24.
    ASSERT(i < (EIGHT_IMC_WAY * MAX_MC_CH));
    if (iMCSize == LineSize) {
      LineIndexSize = (iMC8Way4k_3ChWay4k_LineMultipliers[i][NumLines] * LineSize);
    } else {
      LineIndexSize = (iMC8Way4k_3ChWay256_LineMultipliers[i][NumLines] * LineSize);
    }
    break;
  }
  if (PMEMRgn) {
    *pSpa = (UINT64)(SADBaseAddr + (NumRotation * RotationSize * iMCWays * CH_3WAY) + LineIndexSize + (Dpa % LineSize));
  }
  else {
    *pSpa = (UINT64)(pDimm->SADSpaBase + (NumRotation * RotationSize * iMCWays * CH_3WAY) + LineIndexSize + (Dpa % LineSize));
  }

  return (Status);
}

/**

Routine Description: GetSpaFor1ChWay - This function converts
given Dpa to Spa in the case of 1 way Channel Interleaved ways
and up to 16 iMC Ways.

  @param pDimm       - Pointer to Dimm structure
  @param SADBaseAddr - SAD Rule Base Address
  @param Dpa         - Dpa for which we need the SPA
  @param *pSpa       - Pointer to the Spa.
  @retval EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
GetSpaFor1ChWay(
   PAEP_DIMM   pDimm,
   UINT64      SADBaseAddr,
   UINT64      Dpa,
   UINT64      *pSpa
   )
{
  UINT32           NumLines = 0; // one based
  EFI_STATUS       Status = EFI_SUCCESS;
  UINT64           RotationSize, NumRotation;
  UINT32           NumLinesDesc;
  UINT32           LineIndexSize = 0;
  UINT32           LineSize, iMCSize;
  UINT8            i, iMCWays;
  BOOLEAN          PMEMRgn =  FALSE;

  if (IsPMEMRgn(SADBaseAddr)) {
    LineSize = GetChGranularityFromSADBaseAddr(SADBaseAddr);
    // If PMEM Region, need to get the Interleave Dimm Number from the PMEMInfo struct
    i = CrInfo.PMEMInfo[mPmemIndex].IntDimmNum;
    PMEMRgn = TRUE;
  }
  else {
    i = pDimm->IntDimmNum;
    LineSize = CrInfo.ChGran;
  }
  iMCSize = CrInfo.SktGran;
  iMCWays =  pDimm->iMCWays;
  if (iMCSize == LineSize) {
    NumLinesDesc = 2;
  } else {
    NumLinesDesc = iMCSize / LineSize;
  }

#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "JNP: LineSize = %d, iMCSize = %d, iMCWays = %d\n", LineSize, iMCSize, iMCWays));
  DEBUG((EFI_D_ERROR, "JNP: DPA = 0x%llx\n", Dpa));
#endif

  switch (iMCWays) {
  case ONE_IMC_WAY:
    // For 1 iMCWay and 1ChWay Interleave Spa is the same as Dpa
    if (PMEMRgn) {
      *pSpa = SADBaseAddr + Dpa;
    }
    else {
      *pSpa = pDimm->SADSpaBase + Dpa;
    }
    break;
  // Intended fall thrus
  case TWO_IMC_WAY:
    ASSERT(i < TWO_IMC_WAY);
  case FOUR_IMC_WAY:
    ASSERT(i < FOUR_IMC_WAY);
  case EIGHT_IMC_WAY:
    ASSERT(i < EIGHT_IMC_WAY);
  case SIXTEEN_IMC_WAY:
    ASSERT(i < SIXTEEN_IMC_WAY);
    RotationSize = LineSize * NumLinesDesc;
    NumRotation = Dpa / RotationSize;
    NumLines = (UINT32)((Dpa % RotationSize) / LineSize);
    LineIndexSize = (i * iMCSize) + (LineSize * NumLines);
    if (PMEMRgn) {
      *pSpa = (UINT64)(SADBaseAddr + (NumRotation * RotationSize * iMCWays) + LineIndexSize + (Dpa % LineSize));
    }
    else {
      *pSpa = (UINT64)(pDimm->SADSpaBase + (NumRotation * RotationSize * iMCWays) + LineIndexSize + (Dpa % LineSize));
    }
    break;
  }

  return (Status);
}

/**

Routine Description: GetSpaFor2ChWays - This function converts
given Dpa to Spa in the case of 2 way Channel Interleaved ways
and up to 16 iMC Ways.

  @param pDimm       - Pointer to Dimm structure
  @param SADBaseAddr - SAD Rule Base Address
  @param Dpa         - Dpa for which we need the SPA
  @param *pSpa       - Pointer to the Spa.
  @retval EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
GetSpaFor2ChWays(
   PAEP_DIMM   pDimm,
   UINT64      SADBaseAddr,
   UINT64      Dpa,
   UINT64      *pSpa
   )
{
  UINT32           NumLines = 0; // one based
  EFI_STATUS       Status = EFI_SUCCESS;
  UINT64           RotationSize, NumRotation;
  UINT32           NumLinesDesc;
  UINT32           LineIndexSize = 0;
  UINT32           LineSize, iMCSize;
  UINT8            i, iMCWays;
  BOOLEAN          PMEMRgn =  FALSE;

  if (IsPMEMRgn(SADBaseAddr)) {
    LineSize = GetChGranularityFromSADBaseAddr(SADBaseAddr);
    // If PMEM Region, need to get the Interleave Dimm Number from the PMEMInfo struct
    i = CrInfo.PMEMInfo[mPmemIndex].IntDimmNum;
    PMEMRgn = TRUE;
  }
  else {
    i = pDimm->IntDimmNum;
    LineSize = CrInfo.ChGran;
  }
  iMCSize = CrInfo.SktGran;
  iMCWays =  pDimm->iMCWays;

  if (iMCSize == LineSize) {
    NumLinesDesc = 2;
  } else {
    NumLinesDesc = iMCSize / (LineSize * CH_2WAY);
  }

#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "JNP: LineSize = %d, iMCSize = %d, iMCWays = %d\n", LineSize, iMCSize, iMCWays));
  DEBUG((EFI_D_ERROR, "JNP: DPA = 0x%llx\n", Dpa));
#endif

  RotationSize = LineSize * NumLinesDesc;
  NumRotation = Dpa / RotationSize;
  NumLines = (UINT32)((Dpa % RotationSize) / LineSize);

  switch (iMCWays) {
  case ONE_IMC_WAY:
    // For 1 iMCWay Interleave Interleave Dimm Number should be < 2.
    ASSERT(i < (ONE_IMC_WAY * CH_2WAY));
    if (iMCSize == LineSize) {
      LineIndexSize = (iMC1Way4k_2ChWay4k_LineMultipliers[i][NumLines] * LineSize);
    } else {
      LineIndexSize = (iMC1Way4k_2ChWay256_LineMultipliers[i][NumLines] * LineSize);
    }
    break;
  case TWO_IMC_WAY:
    // For 2 iMCWay Interleave Interleave Dimm Number should be < 4.
    ASSERT(i < (TWO_IMC_WAY * CH_2WAY));
    if (iMCSize == LineSize) {
      LineIndexSize = (iMC2Way4k_2ChWay4k_LineMultipliers[i][NumLines] * LineSize);
    } else {
      LineIndexSize = (iMC2Way4k_2ChWay256_LineMultipliers[i][NumLines] * LineSize);
    }
    break;
  case FOUR_IMC_WAY:
    // For 4 iMCWay Interleave Interleave Dimm Number should be < 8.
    ASSERT(i < (FOUR_IMC_WAY * CH_2WAY));
    if (iMCSize == LineSize) {
      LineIndexSize = (iMC4Way4k_2ChWay4k_LineMultipliers[i][NumLines] * LineSize);
    } else {
      LineIndexSize = (iMC4Way4k_2ChWay256_LineMultipliers[i][NumLines] * LineSize);
    }
    break;
  case EIGHT_IMC_WAY:
    // For 8 iMCWay Interleave Interleave Dimm Number should be < 16.
    ASSERT(i < (EIGHT_IMC_WAY * CH_2WAY));
    // 8 iMC Way not supported for 4k/4k
    LineIndexSize = (iMC8Way4k_2ChWay256_LineMultipliers[i][NumLines] * LineSize);
    break;
  }

  if (PMEMRgn) {
    *pSpa = (UINT64)(SADBaseAddr + (NumRotation * RotationSize * iMCWays * CH_2WAY) + LineIndexSize + (Dpa % LineSize));
  }
  else {
    *pSpa = (UINT64)(pDimm->SADSpaBase + (NumRotation * RotationSize * iMCWays * CH_2WAY) + LineIndexSize + (Dpa % LineSize));
  }

  return (Status);
}

/**

Routine Description: XlateDpaToSpa - This function converts given Dpa
to Spa in the case of the Dimm involved in all Channel ways and
up to 8 iMC Ways.

  @param pDimm       - Pointer to Dimm structure
  @param SADBaseAddr - Start Address of the Spa for
                               Dpa Translation
  @param Dpa         - Dpa for which we need the SPA
  @param *pSpa       - Pointer to the Spa.
  @retval EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
XlateDpaToSpa(
   PAEP_DIMM       pDimm,
   UINT64          SADBaseAddr,
   UINT64          Dpa,
   UINT64          *pSpa
   )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  UINT8            ChWays;

  ChWays =  pDimm->ChWays;

  if (ChWays == CH_3WAY) {
    Status = GetSpaFor3WayChInt(pDimm, SADBaseAddr, Dpa, pSpa);
  }
  else if (ChWays == CH_2WAY) {
    Status = GetSpaFor2ChWays(pDimm, SADBaseAddr, Dpa, pSpa);
  }
  else if (ChWays == CH_1WAY) {
    Status = GetSpaFor1ChWay(pDimm, SADBaseAddr, Dpa, pSpa);
  }
  return (Status);
}

/**

Routine Description: GetDimmIndex will return the Index
number of the NVMDIMM for a given Socket and Channel.
based on socket, channel and dimm/iMC arguments.

  @param socket      - Socket Number
  @param iMC         - iMC number
  @param ch          - DDR Channel ID
  @retval Index      - Index

**/
UINT16
GetDimmIndex(
   UINT8     socket,
   UINT8     Ch
   )
{
  AEP_DIMM      *pDimm;
  UINT16        iMC;
  UINT16        i;

  if (Ch < 3) iMC = 0;
  else iMC = 1;

  for (i = 0; i < CrInfo.NumAepDimms; i++) {
    pDimm = &CrInfo.AepDimms[i];
    if ((pDimm->SocketId == socket) && (pDimm->Ch == Ch) && (pDimm->ImcId == iMC)) {
      return i;
    }
  }
  return (0);
}

/**

Routine Description: GetDimmByIndex will return the pointer to
the Dimm structure if found based the Index.

  @param Index       - Index Number
  @retval pDimm       - Pointer to AEP_DIMM structure, this will
                      have the Dimm struct.
**/
PAEP_DIMM
GetDimmByIndex(
   UINT8     Index
   )
{
  PAEP_DIMM     pDimm = NULL;

  if (Index < CrInfo.NumAepDimms) {
    pDimm = &CrInfo.AepDimms[Index];
  }

  return (pDimm);
}

/**

Routine Description: GetDimm will return the pointer to the Dimm
structure if found based on socket, channel and dimm arguments.

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @retval pDimm      - if the pDimm is found else NULL

**/
PAEP_DIMM
GetDimm(
   UINT8     socket,
   UINT8     ch,
   UINT8     dimm
   )
{
  PAEP_DIMM     pDimm = NULL;
  UINT8         i;

  for (i = 0; i < CrInfo.NumAepDimms; i++) {
    pDimm = &CrInfo.AepDimms[i];
#if VERBOSE_DEBUG
    DEBUG((DEBUG_ERROR, "GetDimm pDimm->SocketId = %d, pDimm->Ch = %d pDimm->Dimm = %d\n", 
           pDimm->SocketId, pDimm->Ch, pDimm->Dimm));
#endif
    if ((pDimm->SocketId == socket) && (pDimm->Ch == ch) && (pDimm->Dimm ==  dimm)) {
      return(pDimm);
    }
  }
  return (NULL);
}

/**

Routine Description: InitDIMMInfoFromHOB initializes DIMM
information from HOB.

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @retval EFI_STATUS - success if command sent

**/
EFI_STATUS
InitDIMMInfoFromHOB(
   UINT8     Socket,
   UINT8     Ch,
   UINT8     Dimm
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  PAEP_DIMM       pDimm = NULL;

  pDimm = GetDimm(Socket, Ch, Dimm);

  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

    pDimm->VendorID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].VendorID;
    pDimm->DeviceID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].DeviceID;
    pDimm->RevisionID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].RevisionID;
    pDimm->subSysVendorID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].subSysVendorID;
    pDimm->subSysDeviceID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].subSysDeviceID;
    pDimm->subSysRevisionID = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].subSysRevisionID;
    pDimm->ManufLocation = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].manufacturingLocation;
    pDimm->ManufDate = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].manufacturingDate;
    pDimm->FormatInterfaceCode = mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].funcdesc;

    pDimm->SerialNum = (((UINT32)mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].serialNumber[3]) << 24  |
                      ((UINT32)mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].serialNumber[2]) << 16  |
                      ((UINT32)mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].serialNumber[1]) << 8 |
                       mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].serialNumber[0]);

    DEBUG((EFI_D_ERROR, "JNP: NVDIMM Socket[%d] Channel[%d] Dimm[%d]:\n", pDimm->SocketId, pDimm->Ch, pDimm->Dimm));
    DEBUG((EFI_D_ERROR, "JNP: VendorID = 0x%X, DeviceID = 0x%X\n", pDimm->VendorID, pDimm->DeviceID));
    DEBUG((EFI_D_ERROR, "JNP: Dimm Serial Number = 0x%X\n", pDimm->SerialNum));
    DEBUG((EFI_D_ERROR, "JNP: FormatInterfaceCode = 0x%X\n", pDimm->FormatInterfaceCode));

  return (Status);
}

/**

Routine Description: InitAllDimmsFromHOB Initializes
all Aep Dimms installed in the system using the HOB,
basically to gather VendorID & DeviceID of all these dimms and
stores in the pDimm Struct.

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @retval EFI_STATUS - success if command sent

**/
VOID
InitAllDimmsFromHOB(
   VOID
   )
{
  UINT8           i;
  PAEP_DIMM       pDimm;

  for (i = 0; i < CrInfo.NumAepDimms; i++) {
    pDimm = &CrInfo.AepDimms[i];
    InitDIMMInfoFromHOB((UINT8)pDimm->SocketId, pDimm->Ch, pDimm->Dimm);
  }
}

/**

Routine Description: Update Nvm Tech ACPI table

  @retval EFI_STATUS     - Status of operation

**/
EFI_STATUS
UpdateJedecNvDimmAcpiTable(
  UINT64 *JedecNvDimmTablePtr
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;
  UINTN                JedecNvDimmTableSize = sizeof(NVDIMM_FW_INTERFACE_TABLE);

  // Exit if AEP DIMM is present
  if (IsAepDimmPresent) return EFI_SUCCESS;

  // Exit if JEDEC NVDIMM is absent
  if (!IsJedecNvDimmPresent) return EFI_SUCCESS;

  //
  // Build the actual NFIT table with needed sub-tables
  //
  Status = BuildNfitTable(JedecNvDimmTablePtr, JedecNvDimmTableSize, mPmemRegions);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Nfit: BuildNfitTable failed, status = %r\n", Status));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  return Status;
}

/**

Routine Description:GetAepDimmSMBiosType17Handles traces thru
all installed Aep Dimms and gets SMBios Type17 handles and saves
these handles in the global struct for later use by Nfit
creation code. updates Dimm info data struct of the NVMDIMM's
(found) Control Region and other info about the Dimm.

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param dimm        - DIMM number
  @retval void       - None

**/
UINT16
GetAepDimmSMBiosType17Handles (
   VOID
   )
{
  EFI_STATUS               Status;
  UINT8                    Socket, Ch, Dimm;
  UINT16                   i;
  EFI_SMBIOS_PROTOCOL      *SmbiosProtocol = NULL;
  EFI_SMBIOS_TABLE_HEADER  *SmbiosRecord = NULL;
  EFI_SMBIOS_HANDLE        SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&SmbiosProtocol);
  ASSERT_EFI_ERROR (Status);
  i = 0;
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    for (Ch = 0; Ch < MAX_CH; Ch++) {
      if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].Enabled == 0) continue;
      // Get the dimmNvList to see if any of them are NVDIMMs
      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
        if (mSystemMemoryMap->Socket[Socket].ChannelInfo[Ch].DimmInfo[Dimm].Present == 0) continue;
        Status = GetNextValidSmbiosHandle (SmbiosProtocol, &SmbiosHandle, &SmbiosRecord);
        if ((!(EFI_ERROR(Status))) && (IsNvDimm (Socket, Ch, Dimm)) && SmbiosRecord != NULL) {
          // Found an NVMDIMM, now get it's SMBios Type17 Handle and save it.
          // Save the handle in the SMBiosHandleInfo & other info
          SMBiosHandleInfo[i].Socket = Socket;
          SMBiosHandleInfo[i].Channel = Ch;
          SMBiosHandleInfo[i].Dimm = Dimm;
          SMBiosHandleInfo[i].SMBiosType17Handle = SmbiosRecord->Handle;
          i++;
        }
      }
    }
  }
  return (i);
}

/**

Routine Description:GetNextValidSmbiosHandle iterates through
the list of SmbiosRecords and returns the valid SMBIOS17 entry.

  @param SmbiosProtocol      - Handler to SMBIOS_PROTOCOL
  @param SmbiosHandle          pointer to returned valid SMBIOS handle
  @param SmbiosRecord        - pointer to SMBBIOSRECORD
  @retval Status             - Returned Status

**/

EFI_STATUS
GetNextValidSmbiosHandle (
  EFI_SMBIOS_PROTOCOL *SmbiosProtocol, 
  EFI_SMBIOS_HANDLE *SmbiosHandle, 
  EFI_SMBIOS_TABLE_HEADER  **SmbiosRecord
  )
{
  EFI_STATUS 	             Status = EFI_NOT_FOUND;
  EFI_SMBIOS_TYPE          SmbiosType;
  SMBIOS_TABLE_TYPE17      *SmbiosType17Record;
  UINT8 Recordfound = FALSE;
  SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  do {
    Status = SmbiosProtocol->GetNext (SmbiosProtocol, SmbiosHandle, &SmbiosType, SmbiosRecord, NULL);
    SmbiosType17Record = (SMBIOS_TABLE_TYPE17 *)*SmbiosRecord;
    if (SmbiosType17Record->Size != 0 &&
      SmbiosType17Record->Speed != 0 &&
      SmbiosType17Record->TotalWidth != 0 &&
      SmbiosType17Record->DataWidth != 0 &&
      SmbiosType17Record->MemoryType != MemoryTypeUnknown) {
      Recordfound = TRUE;
    }
  } while (!EFI_ERROR(Status) && !Recordfound);

  return Status;
}

/**

Routine Description: Is this Dimm NVMDIMM

  @retval BOOLEAN     - TRUE if NVMDIMM otherwise false

**/
BOOLEAN
IsNvDimm(
  UINT8 socket,
  UINT8 ch,
  UINT8 dimm
   )
{
  if (mSystemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].NVmemSize != 0)
    return TRUE;
  else
    return FALSE;
}

/**

Routine Description:CheckIfSameiMCCh - given a socket and
channel, this routine checks if the SAD we're checking has
matching channel and socket.

  @param socket      - Socket Number
  @param ch          - DDR Channel ID
  @param pSAD        - pointer of the SAD
  @retval Channel    -

**/
INT16
CheckIfSameiMCCh(
   UINT8     socket,
   UINT8     ch,
   struct SADTable *pSAD
   )
{
  INT16         Channel = -1;
  UINT8         iMC;
  UINT8         ChInterBitmap = GetChInterBitmap(pSAD);

  //
  // Channel in the dimmNvram is connected with iMC, meaning if channel is < 3, then
  // it is iMC0 and if the channel is between 3 & 5, then iMC1
  if((ch >= 0) && (ch < 3)) {
    iMC = 0;
  } else if((ch > 2) && (ch < 6)) {
    iMC = 1;
  } else
    return (Channel);
  if((iMC == 0) && (pSAD->imcInterBitmap & 0x01)) {
    // iMC matches, i.e, iMC0, check if channel matches.
    switch(ch) {
    case 0:
      if(ChInterBitmap & 0x01)
        Channel = 0;
      break;
    case 1:
      if (ChInterBitmap & 0x02)
        Channel = 1;
      break;
    case 2:
      if (ChInterBitmap & 0x04)
        Channel = 2;
      break;
    }
  }
  else if((iMC == 1) && (pSAD->imcInterBitmap & 0x02)) {
    switch(ch) {
    case 3:
      if (ChInterBitmap & 0x01)
        Channel = 0;
      break;
    case 4:
      if (ChInterBitmap & 0x02)
        Channel = 1;
      break;
    case 5:
      if (ChInterBitmap & 0x04)
        Channel = 2;
      break;
    }
  }
  return (Channel);
}

/**

Routine Description:PreInitDimmStruct pre initializes AEP_DIMM
struct that is being created with the values that are being
passed in of its members.

  @param SADSpaBase  - Spa Base Address of SAD Rule
  @param socket      - socket
  @param Ch          - Channel
  @param *dimm       - This pointer will hold dimm number
  @param iMCWays     - iMCWays this dimm is interleaved across
                     iMCs
  @param ChWays      - Channel Ways this dimm is interleaved.
  @param IntDimmNum  - Dimm number in the interleave set
  @param FMchannelInterBitmap - pSAD->FMchannelInterBitmap of
                              the sad rule

**/
VOID
PreInitDimmStruct(
   UINT64    SADSpaBase,
   UINT8     socket,
   UINT8     ch,
   UINT8     dimm,
   UINT8     iMCWays,
   UINT8     ChWays,
   UINT8     IntDimmNum,
   UINT8     FMchannelInterBitmap
   )
{
  PAEP_DIMM       pDimm;

  pDimm = &CrInfo.AepDimms[CrInfo.NumAepDimms];
  pDimm->SADSpaBase = SADSpaBase;
  pDimm->Ch = ch;
  pDimm->Dimm = dimm;
  pDimm->SocketId = (UINT16)socket;
  if (ch < MAX_MC_CH)
    pDimm->ImcId = 0; // iMC0
  else
    pDimm->ImcId = 1; // iMC1
  pDimm->NonceSet = 0;
  pDimm->iMCWays = iMCWays;
  pDimm->ChWays = ChWays;
  pDimm->IntDimmNum = IntDimmNum;
  pDimm->FMchannelInterBitmap = FMchannelInterBitmap;
}

/**

Routine Description:InitPMEMInfoStruct initializes the PMEMInfo
struct that is in the AEP_DIMM structure with the values
provided as arguments into this function. Basically these values
are used for the translation of Spa-To-Dpa implementation
routines.

  @param Socket      - Socket Number
  @param Ch          - Channel number
  @param *Dimm       - This pointer will hold dimm number
  @param SADSpaBase  - Base Address of the SAD Rule found
  @param iMCWays     - iMC Ways interleave
  @param ChWays      - Channel Ways it is interleaved.
  @param IntDimmNum  - Dimm Number in the SAD Interleave set
  @retval None (VOID)

**/
VOID
InitPMEMInfoStruct(
   UINT8           Socket,
   UINT8           Ch,
   UINT8           Dimm,
   UINT64          SADSpaBase,
   UINT8           iMCWays,
   UINT8           ChWays,
   UINT8           IntDimmNum
   )
{
  PPMEM_INFO       pPMemInfo;

  pPMemInfo = &CrInfo.PMEMInfo[CrInfo.NumPMEMRgns];

  pPMemInfo->SADPMemBase = SADSpaBase;
  pPMemInfo->Socket = Socket;
  pPMemInfo->Channel = Ch;
  pPMemInfo->Dimm = Dimm;
  pPMemInfo->ChWays = ChWays;
  pPMemInfo->iMCWays = iMCWays;
  pPMemInfo->IntDimmNum = IntDimmNum;
  CrInfo.NumPMEMRgns += 1;
}


/**

Routine Description:AddPMemInfo adds pertinent
information regarding PMEM Region that has been found

  @param iMCWays     - iMC Ways interleave
  @param ChWays      - Channel Ways involved in the SAD Rule
  @param SADSpaBase  - Base Address of the SAD Rule found
  @param PmemRegion  - Each PMEM region already found
  @retval Status     - Status

**/
EFI_STATUS
AddPMemInfo(
   UINT8          iMCWays,
   UINT8          ChWays,
   UINT64         SADSpaBase,
   PMEM_REGION    *PmemRegion
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           Soc, Ch, Dimm;
  UINT8           IntDimmNum = 0;
  UINT8           NvDimmIndex = 0;

  for (NvDimmIndex = 0; NvDimmIndex < PmemRegion->NumNvDimms; NvDimmIndex++) {
    Soc = PmemRegion->NvDimms[NvDimmIndex].SocketId;
    Ch = (PmemRegion->NvDimms[NvDimmIndex].ImcId * 3) + PmemRegion->NvDimms[NvDimmIndex].Ch;
    Dimm = PmemRegion->NvDimms[NvDimmIndex].Dimm;
    InitPMEMInfoStruct(Soc, Ch, Dimm, SADSpaBase, iMCWays, ChWays, IntDimmNum);
#if VERBOSE_DEBUG
    DEBUG((EFI_D_ERROR, "JNP: Found PMEM on, Socket = %d, Channel = %d, Dimm = %d\n", Soc, Ch, Dimm));
    DEBUG((EFI_D_ERROR, "JNP: iMCWays = %d, ChWays = %d\n", iMCWays, ChWays));
#endif
    IntDimmNum++;
  }

  return Status;
}

/**

Routine Description:BuildDimms builds Dimm info data
structs for each NVMDIMM's (found)

  @param iMCWays     - iMC Ways involved in the SAD Rule
  @param ChWays      - Channel Ways involved in the SAD Rule
  @param SADSpaBase  - Base Address associated with the SAD Rule
  @param PmemRegion  - Each PMEM region already found
  @retval Status     - Status

**/
EFI_STATUS
BuildDimmInfo(
   UINT8          iMCWays,
   UINT8          ChWays,
   UINT64         SADSpaBase,
   PMEM_REGION    *PmemRegion
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT8           NvDimmIndex;
  UINT8           Soc, Ch, Dimm;
  UINT8           IntDimmNum = 0;
  struct          SADTable *pSAD;
  UINT8           ChInterBitmap;

  pSAD = &mSystemMemoryMap->Socket[PmemRegion->SocketNum].SAD[PmemRegion->SadIndex];
  ChInterBitmap = GetChInterBitmap(pSAD);

  for (NvDimmIndex = 0; NvDimmIndex < PmemRegion->NumNvDimms; NvDimmIndex++) {
    Soc = PmemRegion->NvDimms[NvDimmIndex].SocketId;
    Ch = (PmemRegion->NvDimms[NvDimmIndex].ImcId * 3) + PmemRegion->NvDimms[NvDimmIndex].Ch;
    Dimm = PmemRegion->NvDimms[NvDimmIndex].Dimm;
    PreInitDimmStruct(SADSpaBase, Soc, Ch, Dimm, iMCWays, ChWays, IntDimmNum, ChInterBitmap);
#if VERBOSE_DEBUG
    DEBUG((EFI_D_ERROR, "JNP: Found An NvDimm on, Socket = %d, Channel = %d, Dimm = %d\n", Soc, Ch, Dimm));
    DEBUG((EFI_D_ERROR, "JNP: iMCWays = %d, ChWays = %d\n", iMCWays, ChWays));
    DEBUG((EFI_D_ERROR, "JNP: Dimm Num in the Interleave set = %d\n", IntDimmNum));
#endif
    CrInfo.NumAepDimms++;
    IntDimmNum++;
  }

  ASSERT(CrInfo.NumAepDimms <= MAX_SYS_DIMM);
#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "JNP: Number of NvDimms found so far = %d\n", CrInfo.NumAepDimms));
#endif
  return Status;
}

/**

Routine Description:UpdateDimmInfo updates Dimm info data struct
of the NVMDIMM's (found) Control Region and other info about
the Dimm.If there is more than one dimm in this sad rule, then
it is interleaved and need to account for each one, so creates a
separate dimminfo struct for each one.

  @param MemType       - Memory Type
  @retval VOID         - None

**/
VOID
UpdateDimmInfo()
{
  struct SADTable *pSAD;
  UINT8           ChInterBitmap;
  UINT64          SpaBase;
  UINT64          Size;
  UINT8           i, ChWays, iMCWays;
  UINT8           Socket;
  UINT8           PmemIndex;

  for (PmemIndex = 0; PmemIndex < MAX_PMEM_REGIONS; PmemIndex++) {
    if (mPmemRegions[PmemIndex].Size == 0) continue;
    Socket = mPmemRegions[PmemIndex].SocketNum;
    i = mPmemRegions[PmemIndex].SadIndex;
    pSAD = &mSystemMemoryMap->Socket[Socket].SAD[i];
    ChInterBitmap = GetChInterBitmap(pSAD);

    // Get iMC Interleave ways for this Cntrl Region SAD
    iMCWays = GetiMCWays(Socket, i);
    CrInfo.SktGran = GetSocketGranularity(pSAD);
    CrInfo.ChGran = GetChannelGranularity(pSAD);

    // Get the Channel ways for this SAD
    if (ChInterBitmap == 0x7) ChWays = CH_3WAY;
    else if ((ChInterBitmap == 0x5) || (ChInterBitmap == 0x6) || (ChInterBitmap == 0x3)) {
      ChWays = CH_2WAY;
    } else ChWays = CH_1WAY;
    if (i == 0) {
      SpaBase = 0;
      Size = ((UINT64)mSystemMemoryMap->Socket[Socket].SAD[i].Limit << 26);
    }
    else {
      SpaBase = ((UINT64)mSystemMemoryMap->Socket[Socket].SAD[i - 1].Limit << 26);
      Size = ((UINT64)(mSystemMemoryMap->Socket[Socket].SAD[i].Limit - mSystemMemoryMap->Socket[Socket].SAD[i - 1].Limit) << 26);
    }
#if VERBOSE_DEBUG
    DEBUG((EFI_D_ERROR, "JNP: UpdateDimmInfo - SAD Range Base = 0x%1lx   Size = 0x%1lx\n", SpaBase, Size));
#endif

    BuildDimmInfo(iMCWays, ChWays, SpaBase, &mPmemRegions[PmemIndex]);
    AddPMemInfo(iMCWays, ChWays, SpaBase, &mPmemRegions[PmemIndex]);
  }
}

/**

  This function returns the total number of Ngn Dimms installed
  in the system.

  @param VOID - none

  @return Number of Ngn Dimms installed in the system.

**/
UINT32
GetNumNgnDimms(
  VOID
  )
{
  UINT8       socket, ch, dimm;
  UINT32      NumNgnDimms = 0;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    for (ch = 0; ch < MAX_CH; ch++) {
      if (mSystemMemoryMap->Socket[socket].ChannelInfo[ch].Enabled == 0) continue;
      // Get the dimmNvList to see if any of them are Aep dimms
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if (mSystemMemoryMap->Socket[socket].ChannelInfo[ch].DimmInfo[dimm].Present == 0) continue;
        if (IsNvDimm(socket, ch, dimm))
          NumNgnDimms += 1;
      }
    }
  }
  return (NumNgnDimms);
}

VOID
SwitchAddressModeToLegacy(
   UINT8 Node
   )
{
  SCRUBADDRESSHI_MC_MAIN_STRUCT   ScrubAddrHi;
  UINT8                           Socket = NODE_TO_SKT(Node);
  UINT8                           Mc = NODE_TO_MC(Node);

  ScrubAddrHi.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG);

  //
  // If the patrol scrub mode is SPA mode, need to move to legacy mode to setup the registers
  // and then move back to SPA mode.
  //

  if (ScrubAddrHi.Bits.ptl_sa_mode == 1) {
    ScrubAddrHi.Bits.ptl_sa_mode = 0;
    mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHi.Data);
  }
}

VOID
SwitchAddressModeToSystem(
   UINT8 Node
   )
{
  SCRUBADDRESSHI_MC_MAIN_STRUCT   ScrubAddrHi;
  UINT8                           Socket = NODE_TO_SKT(Node);
  UINT8                           Mc = NODE_TO_MC(Node);
  if (mSystemMemoryMap->MemSetup.patrolScrubAddrMode & PATROL_SCRUB_SPA)
  {
    ScrubAddrHi.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG);

    if (ScrubAddrHi.Bits.ptl_sa_mode == 0) {
      ScrubAddrHi.Bits.ptl_sa_mode = 1;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHi.Data);
    }
  }
}

/**

  This routine an Address Range Scrub (ARS) using the global ARS data.

  @return Status

**/
EFI_STATUS
DoScrub()
{
  EFI_STATUS    Status;
  UINT8         Socket;
  UINT8         Mc;
  UINT8         Node;
  UINT64        SizeToScrub;
  UINT64        RankSize;
  UINT64        DimmSize;
  UINT64        NextDpaStart;
  UINT8         NumRanks;
  UINT32        PcodeMailboxData;
  PAEP_DIMM     pDimm = NULL;
  SCRUBCTL_MC_MAIN_STRUCT         ScrubCtlReg;
  SCRUBADDRESSLO_MC_MAIN_STRUCT   ScrubAddrLoReg;
  SCRUBADDRESSHI_MC_MAIN_STRUCT   ScrubAddrHiReg;
  MCMAIN_CHKN_BITS_MC_MAIN_STRUCT McMainChknReg;
  SMISPARECTL_MC_MAIN_STRUCT      ImcSmiSpareCtlReg;

  Status = SystemAddressToDimmAddress (mArsScrubData.NextBaseAddress, &mArsScrubData.Ta);
  if(EFI_ERROR(Status)) return ARS_START_FAILED;

  Socket = mArsScrubData.Ta.SocketId;
  Mc = mArsScrubData.Ta.MemoryControllerId;
  Node = SKTMC_TO_NODE(Socket, Mc);

  // Switch to legacy mode
  SwitchAddressModeToLegacy (Node);

  // Set patrol inteval to PCU
  PcodeMailboxData  = Mc; // BIT0 MC_INDEX (0 or 1)
  PcodeMailboxData  |= (UINT32)(ARS_SCRUB_INTERVAL << 8);
  Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(Socket, MAILBOX_BIOS_CMD_MC_PATROL_SCRUB_INTERVAL, PcodeMailboxData);
  ASSERT_EFI_ERROR(Status);

  //
  // Enable patrol scrub SMI, stop on complete, and error logging
  //
  ImcSmiSpareCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SMISPARECTL_MC_MAIN_REG);
  ImcSmiSpareCtlReg.Bits.intrpt_sel_smi  = 1;   // SMI Enable
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SMISPARECTL_MC_MAIN_REG, ImcSmiSpareCtlReg.Data);

  McMainChknReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MCMAIN_CHKN_BITS_MC_MAIN_REG);
  McMainChknReg.Bits.defeature_12 = 1;          // Enable patrol to stop after rank is scrubbed
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, MCMAIN_CHKN_BITS_MC_MAIN_REG, McMainChknReg.Data);

  DimmSize = mSystemMemoryMap->Socket[Socket].ChannelInfo[(Mc * 3) + mArsScrubData.Ta.ChannelId].DimmInfo[mArsScrubData.Ta.DimmSlot].NVmemSize;
  NumRanks = mSystemMemoryMap->Socket[Socket].ChannelInfo[(Mc * 3) + mArsScrubData.Ta.ChannelId].DimmInfo[mArsScrubData.Ta.DimmSlot].NumRanks;
  RankSize = (DimmSize / NumRanks) << 26;
  SizeToScrub = RankSize - mArsScrubData.Ta.RankAddress;
#if VERBOSE_DEBUG
  DEBUG ((DEBUG_ERROR, " DimmSize = 0x%lx, NumRanks = %lx, RankSize = 0x%lx\n", DimmSize, NumRanks, RankSize));
  DEBUG ((DEBUG_ERROR, " SizeToScrub = 0x%lx\n", SizeToScrub));
#endif

  mMaxErrorsPerRank = MAX_ERRORS_PER_DIMM / NumRanks;
  mNumErrorsCurrRank = 0;

  DEBUG ((DEBUG_ERROR, "ARS: DoScrub  - SocketId = 0x%x, McId = 0x%x\n", mArsScrubData.Ta.SocketId,
   mArsScrubData.Ta.MemoryControllerId));
  DEBUG ((DEBUG_ERROR, "                Ch = 0x%x, DimmSlot = 0x%x, ChipSelect = 0x%x DimmRank = 0x%x,\n",
   mArsScrubData.Ta.ChannelId, mArsScrubData.Ta.DimmSlot,mArsScrubData.Ta.ChipSelect, mArsScrubData.Ta.DimmRank));
  DEBUG ((DEBUG_ERROR, "                Row = 0x%x, Col = 0x%x, Bank = 0x%x\n", 
   mArsScrubData.Ta.Row, mArsScrubData.Ta.Col, mArsScrubData.Ta.Bank));
  DEBUG ((DEBUG_ERROR, "                Node = %x, PhyRank = %x, RankAddr = %lx\n", 
   Node, mArsScrubData.Ta.PhysicalRankId, mArsScrubData.Ta.RankAddress));
  DEBUG ((DEBUG_ERROR, "                ScrubAddr = %lx, SizeToScrub = %lx, RankSize = %lx\n", 
   mArsScrubData.NextBaseAddress, SizeToScrub, RankSize));

  ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
  mScrubIntervalSave = ScrubCtlReg.Bits.scrubinterval;
  ScrubCtlReg.Bits.scrub_en = 1;
  ScrubCtlReg.Bits.startscrub = 1;
  ScrubCtlReg.Bits.scrubinterval = ARS_SCRUB_INTERVAL;
  ScrubCtlReg.Bits.stop_on_cmpl = 1;
  ScrubCtlReg.Bits.stop_on_err = 1;
  ScrubCtlReg.Bits.ptl_cmpl = 0;

  ScrubAddrHiReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG);
  ScrubAddrHiReg.Bits.rank = mArsScrubData.Ta.ChipSelect; 
  ScrubAddrHiReg.Bits.chnl = mArsScrubData.Ta.ChannelId;
  ScrubAddrHiReg.Bits.mirr_pri = 0;
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHiReg.Data);

  ScrubAddrLoReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG);
  ScrubAddrLoReg.Bits.rankadd = (UINT32)(mArsScrubData.Ta.RankAddress >> 6);
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG, ScrubAddrLoReg.Data);

  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG, ScrubAddrLoReg.Data);
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG, ScrubAddrHiReg.Data);
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtlReg.Data);

  // Check if more area to scrub on this NVDIMM
  if (mArsScrubData.SizeLeftToScrub > SizeToScrub) {
    mArsScrubData.SizeLeftToScrub -= SizeToScrub;
    mPmemIndex = mArsScrubData.PmemIndex[mArsScrubData.CurrentPmemIndex];
    NextDpaStart = CrInfo.PMEMInfo[mPmemIndex].ARSDpaEnd - mArsScrubData.SizeLeftToScrub;
    pDimm = GetDimm(CrInfo.PMEMInfo[mPmemIndex].Socket, CrInfo.PMEMInfo[mPmemIndex].Channel, CrInfo.PMEMInfo[mPmemIndex].Dimm);
    ASSERT(pDimm != NULL);
    if (pDimm != NULL) {
      Status = XlateDpaToSpa(pDimm, CrInfo.PMEMInfo[mPmemIndex].SADPMemBase, NextDpaStart, &mArsScrubData.NextBaseAddress);
    }
  }
  // Check if another NVDIMM to scrub
  else if (mArsScrubData.NumDimmsLeft > 0) {
    mArsScrubData.NumDimmsLeft--;
    mArsScrubData.CurrentPmemIndex++;
    mPmemIndex = mArsScrubData.PmemIndex[mArsScrubData.CurrentPmemIndex];
    NextDpaStart = CrInfo.PMEMInfo[mPmemIndex].ARSDpaStart;
    mArsScrubData.SizeLeftToScrub = (CrInfo.PMEMInfo[mPmemIndex].ARSDpaEnd - CrInfo.PMEMInfo[mPmemIndex].ARSDpaStart);
    pDimm = GetDimm(CrInfo.PMEMInfo[mPmemIndex].Socket, CrInfo.PMEMInfo[mPmemIndex].Channel, CrInfo.PMEMInfo[mPmemIndex].Dimm);
    ASSERT(pDimm != NULL);
    if (pDimm != NULL) {
      Status = XlateDpaToSpa(pDimm, CrInfo.PMEMInfo[mPmemIndex].SADPMemBase, NextDpaStart, &mArsScrubData.NextBaseAddress);
    }
  }
  else {
    mArsScrubData.SizeLeftToScrub = 0;
  }

  return ARS_START_SUCCESS;
}

/**

  This routine is sent to start Address Range Scrub for a
  particular Address range of a Ngn Dimm from the NVMCTLR command
  and passes the information back to the caller thru Acpi and
  the SMM Handler.

  @param Spa       - System Physical Address
  @param ArsLength - ARS Length
  @return Status

**/
EFI_STATUS
StartAddressRangeScrub(
   IN UINT64 Spa,
   IN UINT64 ArsLength
   )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINT8                       Socket;
  UINT8                       Mc;
  UINT8                       Node;
  SCRUBCTL_MC_MAIN_STRUCT     ScrubCtlReg;
  INT32                       Index;

  // Make sure ARS is not already in progress
  if (mIsArs) return ARS_IN_PROGRESS;

  // Initialize ARS DSM Header
  mArsDsmHeader.NumErrorRecords = 0;
  mArsDsmHeader.StartSpa = Spa;
  mArsDsmHeader.Length = ArsLength;
  mArsDsmHeader.Type = PMEM_SCRUB_SUPPORTED;
  mArsDsmHeader.Flags = 0;
  // Init the Output length with that of header size & as and when we error records,
  // we will increment this appropriately.
  mArsDsmHeader.OutputLength = ARS_DSM_HEADER_SIZE;

  // Init global ARS data to start with first NVDIMM in SPA region
  mArsScrubData.CurrentPmemIndex = 0;
  Index = mArsScrubData.PmemIndex[mArsScrubData.CurrentPmemIndex];
  mArsScrubData.NextBaseAddress = Spa;
  mArsScrubData.SizeLeftToScrub = (CrInfo.PMEMInfo[Index].ARSDpaEnd - CrInfo.PMEMInfo[Index].ARSDpaStart);
  mArsScrubData.NumDimmsLeft--; // Decrement for first NVDIMM

  for (Socket = 0; Socket < MAX_SOCKET; Socket++){
    for (Mc = 0; Mc < MAX_IMC; Mc++) {
      if (!mSystemMemoryMap->Socket[Socket].imcEnabled[Mc]){
        continue;
      }
      Node = SKTMC_TO_NODE(Socket, Mc);
      ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
      if (ScrubCtlReg.Bits.scrub_en == 1) {
        // Save MCMAIN_CHKN_BITS_MC_MAIN_REG, save patrol scrub regs, and disable scrub for each node
        mMcMainChknRegSave[Node].Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, MCMAIN_CHKN_BITS_MC_MAIN_REG);
        SavePatrolScrubEngine(Node);
        DisablePatrolScrubEngine(Node);
        mArsScrubData.CurrentNodeScrub[Node] = 1;
      }
      else {
        mArsScrubData.CurrentNodeScrub[Node] = 0;
      }
    }
  }

  Status = DoScrub();

  if (Status == ARS_START_SUCCESS) {
    // Set to indicate ARS has performed at least once on current boot (for query sts)
    if (!mHasArsPerformed) mHasArsPerformed = TRUE;
    mIsArs = TRUE;
  }

  return Status;
}

/**

  Given a PMEM Base Address, Socket and Channel number of the
  dimm in this interleave set, CheckIfTwoPMEMInDimm scans thru
  the PMemInfo struct to see if there is another PMEM Region
  defined for this socket and channel (with a different SAD Base
  Address of course) and if it is then it is the second PMEM
  Region for the dimm. If so, it returns TRUE, else FALSE.

  @param SADBaseAddr - SAD Rule base address
  @param Socket      - Socket
  @param Ch          - Channel
  @param Index       - will return the Index of the PMemInfo
                     struct that has the information about 2nd
                     PMEM Region
  @return Boolena    -  TRUE if a second PMem is found, else
          FALSE

**/
BOOLEAN
CheckIfTwoPMEMInDimm(
   UINT64         SADBaseAddr,
   UINT8          Socket,
   UINT8          Ch,
   INT32          *Index
   )
{
  UINT8           i;

  for (i = 0; i < CrInfo.NumPMEMRgns; i++) {
    if ((CrInfo.PMEMInfo[i].SADPMemBase != SADBaseAddr) && (CrInfo.PMEMInfo[i].Socket == Socket) && \
        (CrInfo.PMEMInfo[i].Channel == Ch)) {
      *Index = (INT32)i;
      return (TRUE);
    }
  }
  return (FALSE);
}


/**

  Given a PMEM Base Address, for all the dimms in this
  interleave set, SetupDpasForPMEMBase checks to see if the
  second PMEM is mapped to any of the dimms in this set, and if
  so, adjust the ArsEnd address to that of the second PMEM
  region so that when Start ARS command is sent for this dimm,
  it will cover both PMEM regions.

  @param SADBaseAddr - SAD Rule base address
  @return VOID

**/
VOID
SetupDpasForPMEMBase(
   UINT64         SADBaseAddr
   )
{
  UINT8           Skt, Ch;
  INT32           Index, PMem2Index;

  Index = GetPMEMIndex(SADBaseAddr);
  if (Index == -1) return;

  Skt = CrInfo.PMEMInfo[Index].Socket;
  Ch = CrInfo.PMEMInfo[Index].Channel;
  Index++;
  // Make sure we don't index out of bounds of PMEMInfo in while loop
  if(Index >= (MAX_SYS_DIMM * MAX_PMEM_RGN_PER_DIMM)) return;

  while ((SADBaseAddr == CrInfo.PMEMInfo[Index].SADPMemBase) && (Index < (MAX_SYS_DIMM * MAX_PMEM_RGN_PER_DIMM))) {
    if (CheckIfTwoPMEMInDimm(SADBaseAddr, Skt, Ch, &PMem2Index)) {
      if (!CrInfo.PMEMInfo[PMem2Index].SecondPMemRgn) {
        if (CrInfo.PMEMInfo[PMem2Index].ARSDpaEnd > CrInfo.PMEMInfo[Index].ARSDpaEnd) {
          // Set the 1st PMem Region to that of actual DpaEnd which is in the second PMem Region
          CrInfo.PMEMInfo[Index].ARSDpaEnd = CrInfo.PMEMInfo[PMem2Index].ARSDpaEnd;
          // And set the flag to tell that it is a second PMem Region
          CrInfo.PMEMInfo[PMem2Index].SecondPMemRgn = TRUE;
        }
        if(CrInfo.PMEMInfo[PMem2Index].ARSDpaStart > CrInfo.PMEMInfo[Index].ARSDpaStart) {
          CrInfo.PMEMInfo[PMem2Index].ARSDpaStart = CrInfo.PMEMInfo[Index].ARSDpaStart;
          // And set the flag to tell that it is a second PMem Region
          CrInfo.PMEMInfo[PMem2Index].SecondPMemRgn = TRUE;
        }
      }
    }
    if(Index < ((MAX_SYS_DIMM * MAX_PMEM_RGN_PER_DIMM) - 1)) {
      Index++;
    }
    Skt = CrInfo.PMEMInfo[Index].Socket;
    Ch = CrInfo.PMEMInfo[Index].Channel;
  }
}

/**

  This routine SetupDpasForMultiplePMEMRgns is called when a
  ARSStart request comes in and the ARS Range is found to spawn
  over multiple SAD Rules. When this request comes in DpaStart
  and DpaEnd addresses are computed for all dimms in all the
  interleave sets. However, there could be dimms with 2 PMEM
  Regions that could be participating in the ARS Range that
  could be part of 2 interleave sets. This routine basically
  checks for these cases and computes the DpaEnd Addresses of
  these dimms so that only one NVMCTLR ARS Command is sent per dimm
  and their status is tracked.

  @param VOID
  @return VOID

**/
VOID
SetupDpasForMultiplePMEMRgns(
   VOID
   )
{
  UINT8           i;

  for (i = 0; i < MAX_PMEM_INTERLEAVE_SET; i++) {
    if (PMEMSADBaseAddresses[i] == 0)
      break;
    SetupDpasForPMEMBase(PMEMSADBaseAddresses[i]);
  }
}

/**

  This routine InitPMEMDpas is sent to reset the Dpa values
  stored in PMEMInfo Data struct before the start of the ARS
  processing.

  @param VOID

  @return VOID

**/
VOID
InitPMEMDpas(
   VOID
   )
{
  UINT8           i;

  for (i = 0; i < CrInfo.NumPMEMRgns; i++) {
    CrInfo.PMEMInfo[i].ARSDpaStart = 0;
    CrInfo.PMEMInfo[i].ARSDpaEnd = 0;
    CrInfo.PMEMInfo[i].SecondPMemRgn = 0;
  }
  //
  // Also reset the global PMEMSADBaseAddresses array
  for (i = 0; i < MAX_PMEM_INTERLEAVE_SET; i++) {
    PMEMSADBaseAddresses[i] = 0;
  }
}

/**

  This routine InitSktsAndChs is sent to initialize the global
  module variable arrays Skts and Chs (with 0xff).

  @param VOID

  @return VOID

**/
VOID
InitSktsAndChs(
   VOID
   )
{
  UINT8           i;

  for (i = 0; i < MAX_SOCKET; i++) {
    Skts[i] = 0xFF;
  }

  for (i = 0; i < MAX_MC_CH * MAX_IMC; i++) {
    Chs[i] = 0xFF;
  }
}

/**

  This routine IsUniqueSktOrCh returns a boolean when either of
  the input (Skt or Ch) is not found in the Skts and Chs global
  arrays.

  @param Skt       - Socket
  @param Ch        - Channel

  @return Boolean when either of the input is not founde in the
          array

**/
BOOLEAN
IsUniqueSktOrCh(
   UINT8           Skt,
   UINT8           Ch
   )
{
  UINT8           i;
  BOOLEAN         UniqueSkt = TRUE, UniqueCh = TRUE;

  for (i = 0; i < MAX_SOCKET; i++) {
    if (Skts[i] == Skt) {
      UniqueSkt = FALSE;
      break;
    }
    if (Skts[i] == 0xFF) {
      Skts[i] = Skt;
      break;
    }
  }

  for (i =  0; i < MAX_MC_CH * MAX_IMC; i++) {
    if (Chs[i] == Ch) {
      UniqueCh = FALSE;
      break;
    }
    if (Chs[i] == 0xFF) {
      Chs[i] = Ch;
      break;
    }
  }

  if ((UniqueSkt) || (UniqueCh))
    return (TRUE);
  else
    return (FALSE);

}

/**

  This routine ComputeDPAsForARS Computes the Dpa start and end
  addresses of the Dimms involved in the interleave set and the
  ARS Range. And saves these values in the data structs for soon
  to send ARS Commands. If there are multiple PMEM Regions in
  any dimm, the end address will be offset to the second PMEM
  region address.

  @param Spa       - System Physical Address that needs to for
              which it requires ARS processing
  @param SADBaseAddr - SAD Rule base address
  @param ArsLength - ARS Length
  @return Status

**/
EFI_STATUS
ComputeDPAsForARS(
   UINT64         Spa,
   UINT64         SADBaseAddr,
   UINT64         ArsLength
   )
{
  UINT8           Skt, Ch, Dimm;
  UINT8           i, TotalDimms;
  INT32           Index;
  UINT32          ChGran;
  UINT8           StartDimmNum;
  UINT64          SpaStart;
  PAEP_DIMM       pDimm = NULL;
  EFI_STATUS      Status;

  //
  // Given the Spa, lets get the Socket, Ch & Dimm that this Spa is mapped into.
  Status = GetDimmInfoFromSpa(Spa, &SADBaseAddr, &Skt, &Ch, &Dimm);
  if (Status != EFI_SUCCESS)
    return (Status);

  ChGran = GetChGranularityFromSADBaseAddr(SADBaseAddr);

  StartDimmNum = GetPMEMIntDimmNum(SADBaseAddr, Skt, Ch);
#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "ARS: CompDPAs - Starting Dimm Number is = %d\n", StartDimmNum));
#endif

  // Total Number of Dimms in this interleave set
  TotalDimms = GetNumDimmsForPMEMRgn(SADBaseAddr);

  Index =  GetPMEMIndexBasedOnSocChDimm(SADBaseAddr, Skt, Ch, Dimm);

  // GetPMEMIndexBasedOnSocCh returns -1 if it fails.
  if (Index < 0) {
	  return EFI_NOT_FOUND;
  } // Return an error if Index is bigger than array PMEMInfo
  else if (Index >= (MAX_SYS_DIMM * MAX_PMEM_RGN_PER_DIMM)) {
	  return EFI_BUFFER_TOO_SMALL;
  }

  mArsScrubData.PmemIndex[mArsScrubData.NumDimmsLeft] = Index;
  pDimm = GetDimm(Skt, Ch, Dimm);
  if (pDimm == NULL) {
    return (EFI_INVALID_PARAMETER);
  }

  // Get the starting dimm Spa translated to Dpa and save it in the PMEMInfo struct
  Status = XlateSpaToDpa(pDimm, Spa, SADBaseAddr, &CrInfo.PMEMInfo[Index].ARSDpaStart);
  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "ARS: CompDPAs - Xlation from Spa to Dpa failed! returning\n"));
    return (Status);
  }

  // Get the Dpa End based on iMCWays and Channel Ways
  CrInfo.PMEMInfo[Index].ARSDpaEnd = CrInfo.PMEMInfo[Index].ARSDpaStart + (ArsLength / (CrInfo.PMEMInfo[Index].iMCWays * CrInfo.PMEMInfo[Index].ChWays));

  //
  // Now get the Dpa starting address of all the dimms that cross into this given ARS Range
  // Done with starting 1st dimm, get the rest
  if (Spa % ChGran) {
    SpaStart = (Spa / ChGran) * ChGran;
  }
  else {
    SpaStart = Spa;
  }
  //
  // Now get the Dpa starting address of all the dimms that cross into this given ARS Range
  // Done with starting 1st dimm, get the rest
  for (i = 0; i < (TotalDimms - 1); i++) {
    SpaStart += ChGran;
    // Ensure that SpaStart is with in the ARS Range provided
    if (SpaStart > (Spa + ArsLength)) {
      goto AllFound;
    }
    Status = GetDimmInfoFromSpa(SpaStart, &SADBaseAddr, &Skt, &Ch, &Dimm);

    // Now, we should have the next Dimm that is part of the ARS Set.
    Index =  GetPMEMIndexBasedOnSocChDimm(SADBaseAddr, Skt, Ch, Dimm);
    if (Index == -1) return (EFI_INVALID_PARAMETER);
    mArsScrubData.PmemIndex[mArsScrubData.NumDimmsLeft+i+1] = Index;
    pDimm = GetDimm(Skt, Ch, Dimm);
    if (pDimm == NULL) {
      return (EFI_INVALID_PARAMETER);
    }

    // Get the starting dimm Spa translated to Dpa and save it in the PMEMInfo struct
    ASSERT(Index < (sizeof(CrInfo.PMEMInfo) / sizeof(CrInfo.PMEMInfo[0])));
    if (Index < (sizeof(CrInfo.PMEMInfo) / sizeof(CrInfo.PMEMInfo[0]))) {
      Status = XlateSpaToDpa(pDimm, SpaStart, SADBaseAddr, &CrInfo.PMEMInfo[Index].ARSDpaStart);
      // Get the Dpa End based on iMCWays and Channel Ways
      CrInfo.PMEMInfo[Index].ARSDpaEnd = CrInfo.PMEMInfo[Index].ARSDpaStart + (ArsLength / (CrInfo.PMEMInfo[Index].iMCWays * CrInfo.PMEMInfo[Index].ChWays));
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
  }
  mArsScrubData.NumDimmsLeft += TotalDimms;
AllFound:
  return (Status);
}

EFI_STATUS
SetDimmESPolicy(
   NVDIMM *NvDimm
)
{
  EFI_STATUS Status;
  UINT8 Data;
  UINT8 ESPolicy = 0;

  if ((Status = JedecSwitchPage(NvDimm, 0)) == EFI_SUCCESS) {
    // 1: Supercapacitor Battery
    if (NvDimm->EnergyType == 1) {
      // Device Managed Policy - Clear Host Managed Policy Bit
      Data = BIT0;
      ESPolicy = Data;
    }
    else {
      // Host Managed Policy - Clear Device Managed Policy Bit
      Data = BIT1;
      ESPolicy = Data;
    }
    Status = WriteSmb(NvDimm, (NVM_SET_ES_POLICY_CMD & 0xFF), &Data);
  }

  ReadSmb(NvDimm, (NVM_SET_ES_POLICY_STATUS & 0xFF), &Data);
  DEBUG((EFI_D_ERROR, "Set ES Policy Status  = %02x\n", Data));
  // Check for [1] : SET_ES_POLICY_ERROR
  if (Data & BIT1) {
    DEBUG((EFI_D_ERROR, "Failure to set ES Policy\n"));
    Status = 1;
  }
  // Check [2] : DEVICE_MANAGED_POLICY_ENABLED | [3] : HOST_MANAGED_POLICY_ENABLED
  else {
    Data >>= 2;
    if ((Data & 0x03) != ESPolicy) {
      DEBUG((EFI_D_ERROR, "Failure to set ES Policy\n"));
      Status = 1;
    }
    else {
      DEBUG((EFI_D_ERROR, "Successfully set ES Policy[%d]\n", ESPolicy));
    }
  }

  return Status;
}

VOID
GetDimmId(
   NVDIMM *NvDimm
)
{
  UINT8 Data;
  NVDIMM_IDENTIFICATION *DimmId = (NVDIMM_IDENTIFICATION *)&NvDimm->DimmId;

  JedecSwitchPage(NvDimm, 0);
  ReadSmb(NvDimm, (NVM_SPECIFICATION_REV & 0xFF), &Data);
  DimmId->SpecificationRevision = Data;
  ReadSmb(NvDimm, (NVM_NUM_STANDARD_PAGES & 0xFF), &Data);
  DimmId->StandardPageNum = Data;
  ReadSmb(NvDimm, (NVM_FIRST_VENDOR_PAGE & 0xFF), &Data);
  DimmId->FirstVendorPage = Data;
  ReadSmb(NvDimm, (NVM_NUM_VENDOR_PAGES & 0xFF), &Data);
  DimmId->VendorPageNum = Data;
  ReadSmb(NvDimm, (NVM_HARDWARE_REV & 0xFF), &Data);
  DimmId->HardwareRevision = Data;
  ReadSmb(NvDimm, (NVM_FW_REV_SLOT0 & 0xFF), &Data);
  DimmId->FirmwareRevision = Data;
  ReadSmb(NvDimm, ((NVM_FW_REV_SLOT0 + 1) & 0xFF), &Data);
  DimmId->FirmwareRevision |= Data << 4;
  ReadSmb(NvDimm, (NVM_FW_REV_SLOT1 & 0xFF), &Data);
  DimmId->FirmwareRevision |= Data << 8;
  ReadSmb(NvDimm, ((NVM_FW_REV_SLOT1 + 1) & 0xFF), &Data);
  DimmId->FirmwareRevision |= Data << 12;
  DimmId->FirmwareSlotCount = 2;    // Must be 2 for slot 0 and slot 1
  ReadSmb(NvDimm, (NVM_CAPABILITIES & 0xFF), &Data);
  DimmId->Capabilities = Data;
  ReadSmb(NvDimm, (NVM_SUPPORTED_BACKUP_TRIGGER & 0xFF), &Data);
  DimmId->SupportedBackupTriggers = Data;
  ReadSmb(NvDimm, (NVM_MAX_OP_RETRY_COUNT & 0xFF), &Data);
  DimmId->MaxOpRetryCount = Data;
  ReadSmb(NvDimm, (NVM_SUPPORTED_NOTIFY_EVENTS & 0xFF), &Data);
  DimmId->SupportedNotificationEvents = Data;
  ReadSmb(NvDimm, (NVM_SAVE_OP_TIMEOUT & 0xFF), &Data);
  DimmId->SaveOpTimeout = Data;
  ReadSmb(NvDimm, ((NVM_SAVE_OP_TIMEOUT + 1) & 0xFF), &Data);
  DimmId->SaveOpTimeout |= Data << 8;
  ReadSmb(NvDimm, (NVM_RESTORED_OP_TIMEOUT & 0xFF), &Data);
  DimmId->RestoreOpTimeout = Data;
  ReadSmb(NvDimm, ((NVM_RESTORED_OP_TIMEOUT + 1) & 0xFF), &Data);
  DimmId->RestoreOpTimeout |= Data << 8;
  ReadSmb(NvDimm, (NVM_ERASED_OP_TIMEOUT & 0xFF), &Data);
  DimmId->EraseOpTimeout = Data;
  ReadSmb(NvDimm, ((NVM_ERASED_OP_TIMEOUT + 1) & 0xFF), &Data);
  DimmId->EraseOpTimeout |= Data << 8;
  ReadSmb(NvDimm, (NVM_ARMED_OP_TIMEOUT & 0xFF), &Data);
  DimmId->ArmOpTimeout = Data;
  ReadSmb(NvDimm, ((NVM_ARMED_OP_TIMEOUT + 1) & 0xFF), &Data);
  DimmId->ArmOpTimeout |= Data << 8;
  ReadSmb(NvDimm, (NVM_FW_OP_TIMEOUT & 0xFF), &Data);
  DimmId->FwOpTimeout = Data;
  ReadSmb(NvDimm, ((NVM_FW_OP_TIMEOUT + 1) & 0xFF), &Data);
  DimmId->FwOpTimeout |= Data << 8;
  ReadSmb(NvDimm, (NVM_ABORT_OP_TIMEOUT & 0xFF), &Data);
  DimmId->AbortOpTimeout = Data;
  ReadSmb(NvDimm, (NVM_MIN_OP_TEMP & 0xFF), &Data);
  DimmId->MinOpTemp = Data;
  ReadSmb(NvDimm, (NVM_MAX_OP_TEMP & 0xFF), &Data);
  DimmId->MaxOpTemp = Data;
  ReadSmb(NvDimm, (NVM_REGION_BLOCK_SIZE & 0xFF), &Data);
  DimmId->RegionBlockSize[0] = Data;
  JedecSwitchPage(NvDimm, 3);
  ReadSmb(NvDimm, (NVM_CURRENT_FW_SLOT & 0xFF), &Data);
  DimmId->CurrentFirmwareSlot = Data;

#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_SPECIFICATION_REV = 0x%lX\n", DimmId->SpecificationRevision));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_NUM_STANDARD_PAGES = 0x%lX\n", DimmId->StandardPageNum));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_FIRST_VENDOR_PAGE = 0x%lX\n", DimmId->FirstVendorPage));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_NUM_VENDOR_PAGES = 0x%lX\n", DimmId->VendorPageNum));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_HARDWARE_REV = 0x%lX\n", DimmId->HardwareRevision));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_FW_REV_SLOT0 = 0x%lX\n", DimmId->FirmwareRevision));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_FW_REV_SLOT0 = 0x%lX\n", DimmId->FirmwareRevision));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_FW_REV_SLOT1 = 0x%lX\n", DimmId->FirmwareRevision));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_FW_REV_SLOT1 = 0x%lX\n", DimmId->FirmwareRevision));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_CAPABILITIES = 0x%lX\n", DimmId->Capabilities));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_SUPPORTED_BACKUP_TRIGGER = 0x%lX\n", DimmId->SupportedBackupTriggers));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_MAX_OP_RETRY_COUNT = 0x%lX\n", DimmId->MaxOpRetryCount));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_SUPPORTED_NOTIFY_EVENTS = 0x%lX\n", DimmId->SupportedNotificationEvents));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_SAVE_OP_TIMEOUT = 0x%lX\n", DimmId->SaveOpTimeout));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_RESTORED_OP_TIMEOUT = 0x%lX\n", DimmId->RestoreOpTimeout));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_ERASED_OP_TIMEOUT = 0x%lX\n", DimmId->EraseOpTimeout));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_ARMED_OP_TIMEOUT = 0x%lX\n", DimmId->ArmOpTimeout));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_FW_OP_TIMEOUT = 0x%lX\n", DimmId->FwOpTimeout));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_ABORT_OP_TIMEOUT = 0x%lX\n", DimmId->AbortOpTimeout));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_MIN_OP_TEMP = 0x%lX\n", DimmId->MinOpTemp));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_MAX_OP_TEMP = 0x%lX\n", DimmId->MaxOpTemp));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_REGION_BLOCK_SIZE = 0x%lX\n", DimmId->RegionBlockSize[0] ));
  DEBUG((EFI_D_ERROR, "NVDIMM NVM_CURRENT_FW_SLOT = 0x%lX\n", DimmId->CurrentFirmwareSlot));
#endif
}

VOID
GetDimmPwrInfo(
NVDIMM *NvDimm
)
{
  UINT8 Data;
  SAVE_OPERATION_REQ *SaveOpReg = (SAVE_OPERATION_REQ *)&NvDimm->SaveOpReg;

  JedecSwitchPage(NvDimm, 0);
  ReadSmb(NvDimm, (NVM_AVG_PWR_REQ & 0xFF), &Data);
  SaveOpReg->AveragePwrReq = Data;
  ReadSmb(NvDimm, ((NVM_AVG_PWR_REQ + 1) & 0xFF), &Data);
  SaveOpReg->AveragePwrReq |= Data << 8;
  ReadSmb(NvDimm, (NVM_IDLE_PWR_REQ & 0xFF), &Data);
  SaveOpReg->IdlePwrReg = Data;
  ReadSmb(NvDimm, ((NVM_IDLE_PWR_REQ + 1) & 0xFF), &Data);
  SaveOpReg->IdlePwrReg |= Data << 8;
  ReadSmb(NvDimm, (NVM_MIN_VOLT_REQ & 0xFF), &Data);
  SaveOpReg->MinVoltReq = Data;
  ReadSmb(NvDimm, ((NVM_MIN_VOLT_REQ + 1) & 0xFF), &Data);
  SaveOpReg->MinVoltReq |= Data << 8;
  ReadSmb(NvDimm, (NVM_MAX_VOLT_REQ & 0xFF), &Data);
  SaveOpReg->MaxVoltReg = Data;
  ReadSmb(NvDimm, ((NVM_MAX_VOLT_REQ + 1)& 0xFF), &Data);
  SaveOpReg->MaxVoltReg |= Data << 8;

#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "AveragePwrReq = %02x\n", SaveOpReg->AveragePwrReq));
  DEBUG((EFI_D_ERROR, "IdlePwrReg    = %02x\n", SaveOpReg->IdlePwrReg));
  DEBUG((EFI_D_ERROR, "MinVoltReq    = %02x\n", SaveOpReg->MinVoltReq));
  DEBUG((EFI_D_ERROR, "MaxVoltReg    = %02x\n", SaveOpReg->MaxVoltReg));
#endif
}

VOID
GetDimmESInfo(
NVDIMM *NvDimm
)
{
  UINT8 Data;
  UINT8 ESPolicy;
  ENERGY_SOURCE_IDENTIFICATION *ESInfo = (ENERGY_SOURCE_IDENTIFICATION *)&NvDimm->ESInfo;
  ES_LIFETIME_INFO             *ESLifeInfo = (ES_LIFETIME_INFO *)&NvDimm->ESLifeInfo;

  ZeroMem(ESInfo, sizeof(ENERGY_SOURCE_IDENTIFICATION));
  JedecSwitchPage(NvDimm, 0);
  ReadSmb(NvDimm, (NVM_ES_POLICY & 0xFF), &Data);
  DEBUG((EFI_D_ERROR, "ES Policy Supported  = %02x\n", Data));

  ReadSmb(NvDimm, (NVM_SET_ES_POLICY_STATUS & 0xFF), &Data);
  DEBUG((EFI_D_ERROR, "ES Policy Status  = %02x\n", Data));
  ESPolicy = Data >> 2;

  if (ESPolicy & BIT0) {
    ESInfo->ESPolicy = 0;
    DEBUG((EFI_D_ERROR, "Select device managed ES policy\n"));
  }else if (ESPolicy & BIT1) {
    ESInfo->ESPolicy = 1;
    DEBUG((EFI_D_ERROR, "Select host managed ES policy\n"));
  }else
    DEBUG((EFI_D_ERROR, "ERROR: ES Policy is not set!\n"));

  if (ESInfo->ESPolicy == 0) {
    ReadSmb(NvDimm, (NVM_ES_HEALTH_CHECK_FREQ & 0xFF), &Data);
    ESInfo->DevESId.ESHealthChkFreq = Data;
    JedecSwitchPage(NvDimm, 1);
    ReadSmb(NvDimm, (NVM_ES_ATTRIBUTES & 0xFF), &Data);
    ESInfo->DevESId.ESAttribute = Data;
    ReadSmb(NvDimm, (NVM_ES_TECHNOLOGY & 0xFF), &Data);
    ESInfo->DevESId.ESTechnology = Data;
    ReadSmb(NvDimm, (NVM_ES_HARDWARE_REV & 0xFF), &Data);
    ESInfo->DevESId.ESHardwareRev = Data;
    ReadSmb(NvDimm, ((NVM_ES_HARDWARE_REV + 1) & 0xFF), &Data);
    ESInfo->DevESId.ESHardwareRev |= Data << 8;
    ReadSmb(NvDimm, (NVM_ES_FW_REV & 0xFF), &Data);
    ESInfo->DevESId.ESFirmwareRev = Data;
    ReadSmb(NvDimm, ((NVM_ES_FW_REV + 1) & 0xFF), &Data);
    ESInfo->DevESId.ESFirmwareRev |= Data << 8;
    ReadSmb(NvDimm, (NVM_ES_CHARGE_TIMEOUT & 0xFF), &Data);
    ESInfo->DevESId.ESChargeTimeout0 = Data;
    ReadSmb(NvDimm, ((NVM_ES_CHARGE_TIMEOUT + 1) & 0xFF), &Data);
    ESInfo->DevESId.ESChargeTimeout1 = Data;
    ReadSmb(NvDimm, (NVM_ES_MIN_OP_TEMP & 0xFF), &Data);
    ESInfo->DevESId.ESMinOpTemp = Data;
    ReadSmb(NvDimm, (NVM_ES_MAX_OP_TEMP & 0xFF), &Data);
    ESInfo->DevESId.ESMaxOpTemp = Data;
#if VERBOSE_DEBUG
    DEBUG((EFI_D_ERROR, "ESHealthChkFreq  = %02x\n", ESInfo->DevESId.ESHealthChkFreq));
    DEBUG((EFI_D_ERROR, "ESAttribute      = %02x\n", ESInfo->DevESId.ESAttribute));
    DEBUG((EFI_D_ERROR, "ESTechnology     = %02x\n", ESInfo->DevESId.ESTechnology));
    DEBUG((EFI_D_ERROR, "ESHardwareRev    = %02x\n", ESInfo->DevESId.ESHardwareRev));
    DEBUG((EFI_D_ERROR, "ESFirmwareRev    = %02x\n", ESInfo->DevESId.ESFirmwareRev));
    DEBUG((EFI_D_ERROR, "ESChargeTimeout0 = %02x\n", ESInfo->DevESId.ESChargeTimeout0));
    DEBUG((EFI_D_ERROR, "ESChargeTimeout1 = %02x\n", ESInfo->DevESId.ESChargeTimeout1));
    DEBUG((EFI_D_ERROR, "ESMinOpTemp      = %02x\n", ESInfo->DevESId.ESMinOpTemp));
    DEBUG((EFI_D_ERROR, "ESMaxOpTemp      = %02x\n", ESInfo->DevESId.ESMaxOpTemp));
#endif
  } else {
    ReadSmb(NvDimm, (NVM_ES_HEALTH_CHECK_FREQ & 0xFF), &Data);
    ESInfo->HostESId.ESHealthChkFreq = Data;
    JedecSwitchPage(NvDimm, 1);
    ReadSmb(NvDimm, (NVM_ES_ATTRIBUTES & 0xFF), &Data);
    ESInfo->HostESId.ESAttribute = Data;
    ReadSmb(NvDimm, (NVM_ES_TECHNOLOGY & 0xFF), &Data);
    ESInfo->HostESId.ESTechnology = Data;
#if VERBOSE_DEBUG
    DEBUG((EFI_D_ERROR, "ESHealthChkFreq  = %02x\n", ESInfo->DevESId.ESHealthChkFreq));
    DEBUG((EFI_D_ERROR, "ESAttribute      = %02x\n", ESInfo->DevESId.ESAttribute));
    DEBUG((EFI_D_ERROR, "ESTechnology     = %02x\n", ESInfo->DevESId.ESTechnology));
#endif
  }

  JedecSwitchPage(NvDimm, 1);
  ReadSmb(NvDimm, (NVM_ES_LIFETIME_PERCENTAGE & 0xFF), &Data);
  ESLifeInfo->ESLifeTime = Data;
  ReadSmb(NvDimm, (NVM_ES_CURRENT_TEMP & 0xFF), &Data);
  ESLifeInfo->ESTemp0 = Data;
  ReadSmb(NvDimm, ((NVM_ES_CURRENT_TEMP + 1) & 0xFF), &Data);
  ESLifeInfo->ESTemp1 = Data;
  ReadSmb(NvDimm, (NVM_ES_TOTAL_RUNTIME & 0xFF), &Data);
  ESLifeInfo->ESRuntime0 = Data;
  ReadSmb(NvDimm, ((NVM_ES_TOTAL_RUNTIME + 1) & 0xFF), &Data);
  ESLifeInfo->ESRuntime1 = Data;
#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "NVM_ES_LIFETIME_PERCENTAGE   = %02x\n", ESLifeInfo->ESLifeTime));
  DEBUG((EFI_D_ERROR, "NVM_ES_CURRENT_TEMP0         = %02x\n", ESLifeInfo->ESTemp0));
  DEBUG((EFI_D_ERROR, "NVM_ES_CURRENT_TEMP1         = %02x\n", ESLifeInfo->ESTemp1));
  DEBUG((EFI_D_ERROR, "NVM_ES_TOTAL_RUNTIME         = %02x\n", ESLifeInfo->ESRuntime0));
  DEBUG((EFI_D_ERROR, "NVM_ES_TOTAL_RUNTIME1        = %02x\n", ESLifeInfo->ESRuntime1));
#endif
}

VOID
GetDimmOpStatistics(
NVDIMM *NvDimm
)
{
  UINT8 Data;
  OPERATION_STATISTIC_INFO *OpInfo = (OPERATION_STATISTIC_INFO *)&NvDimm->OpInfo;

  JedecSwitchPage(NvDimm, 2);
  ReadSmb(NvDimm, (NVM_LAST_SAVE_OP_DURATION & 0xFF), &Data);
  OpInfo->LastSaveDuration0 = Data;
  ReadSmb(NvDimm, ((NVM_LAST_SAVE_OP_DURATION + 1) & 0xFF), &Data);

  OpInfo->LastSaveDuration1 = Data;
  ReadSmb(NvDimm, (NVM_LAST_RESTORE_OP_DURATION & 0xFF), &Data);
  OpInfo->LastRestoreDuration0 = Data;
  ReadSmb(NvDimm, ((NVM_LAST_RESTORE_OP_DURATION + 1) & 0xFF), &Data);
  OpInfo->LastRestoreDuration1 = Data;

  ReadSmb(NvDimm, (NVM_LAST_ERASE_OP_DURATION & 0xFF), &Data);
  OpInfo->LastEraseDuration0 = Data;
  ReadSmb(NvDimm, ((NVM_LAST_ERASE_OP_DURATION + 1) & 0xFF), &Data);
  OpInfo->LastEraseDuration1 = Data;

  ReadSmb(NvDimm, (NVM_COMPLETED_SAVE_OP_NUMBER & 0xFF), &Data);
  OpInfo->NumSaveOpsCount0 = Data;
  ReadSmb(NvDimm, ((NVM_COMPLETED_SAVE_OP_NUMBER + 1) & 0xFF), &Data);
  OpInfo->NumSaveOpsCount1 = Data;

  ReadSmb(NvDimm, (NVM_COMPLETED_RESTORE_OP_NUMBER & 0xFF), &Data);
  OpInfo->NumRestoreOpsCount0 = Data;
  ReadSmb(NvDimm, ((NVM_COMPLETED_RESTORE_OP_NUMBER + 1) & 0xFF), &Data);
  OpInfo->NumRestoreOpsCount1 = Data;

  ReadSmb(NvDimm, (NVM_COMPLETED_ERASE_OP_NUMBER & 0xFF), &Data);
  OpInfo->NumEraseOpsCount0 = Data;
  ReadSmb(NvDimm, ((NVM_COMPLETED_ERASE_OP_NUMBER + 1) & 0xFF), &Data);
  OpInfo->NumEraseOpsCount1 = Data;

  ReadSmb(NvDimm, (NVM_MODULE_PWR_CYCLES & 0xFF), &Data);
  OpInfo->NumModulePwrCycle0 = Data;
  ReadSmb(NvDimm, ((NVM_MODULE_PWR_CYCLES + 1) & 0xFF), &Data);
  OpInfo->NumModulePwrCycle1 = Data;

#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "LastSaveDuration0    = %02x\n", OpInfo->LastSaveDuration0));
  DEBUG((EFI_D_ERROR, "LastSaveDuration1    = %02x\n", OpInfo->LastSaveDuration1));
  DEBUG((EFI_D_ERROR, "LastRestoreDuration0 = %02x\n", OpInfo->LastRestoreDuration0));
  DEBUG((EFI_D_ERROR, "LastRestoreDuration1 = %02x\n", OpInfo->LastRestoreDuration1));
  DEBUG((EFI_D_ERROR, "LastEraseDuration0   = %02x\n", OpInfo->LastEraseDuration0));
  DEBUG((EFI_D_ERROR, "LastEraseDuration1   = %02x\n", OpInfo->LastEraseDuration1));
  DEBUG((EFI_D_ERROR, "NumSaveOpsCount0     = %02x\n", OpInfo->NumSaveOpsCount0));
  DEBUG((EFI_D_ERROR, "NumSaveOpsCount1     = %02x\n", OpInfo->NumSaveOpsCount1));
  DEBUG((EFI_D_ERROR, "NumRestoreOpsCount0  = %02x\n", OpInfo->NumRestoreOpsCount0));
  DEBUG((EFI_D_ERROR, "NumRestoreOpsCount1  = %02x\n", OpInfo->NumRestoreOpsCount1));
  DEBUG((EFI_D_ERROR, "NumEraseOpsCount0    = %02x\n", OpInfo->NumEraseOpsCount0));
  DEBUG((EFI_D_ERROR, "NumEraseOpsCount1    = %02x\n", OpInfo->NumEraseOpsCount1));
  DEBUG((EFI_D_ERROR, "NumModulePwrCycle0   = %02x\n", OpInfo->NumModulePwrCycle0));
  DEBUG((EFI_D_ERROR, "NumModulePwrCycle1   = %02x\n", OpInfo->NumModulePwrCycle1));
#endif
}

VOID
GetDimmIModuleInfo(
NVDIMM *NvDimm
)
{
  UINT8 Data;
  MODULE_INFO *ModuleInfo = (MODULE_INFO *)&NvDimm->ModuleInfo;

  JedecSwitchPage(NvDimm, 0);
  ReadSmb(NvDimm, (NVM_MODULE_HEALTH & 0xFF), &Data);
  ModuleInfo->Health.ModuleHealthStatus = Data;
  ReadSmb(NvDimm, ((NVM_MODULE_HEALTH + 1) & 0xFF), &Data);
  ModuleInfo->Health.ModuleHealthStatus |= Data << 8;
  ModuleInfo->Health.ModuleTemp = 28;         // N/A I2C
  ReadSmb(NvDimm, (NVM_ERR_THRESHOLD_STATUS & 0xFF), &Data);
  ModuleInfo->Health.ErrorThresStatus = Data;
  ReadSmb(NvDimm, (NVM_WARN_THRESHOLD_STATUS & 0xFF), &Data);
  ModuleInfo->Health.ErrorThresWarn = Data;
  ReadSmb(NvDimm, (NVM_LIFETIME & 0xFF), &Data);
  ModuleInfo->Health.NVMLifetime = Data;
  JedecSwitchPage(NvDimm, 2);
  ReadSmb(NvDimm, (NVM_UNCORR_ECC_COUNTS & 0xFF), &Data);
  ModuleInfo->Health.UECount = Data;
  ReadSmb(NvDimm, (NVM_CORR_ECC_COUNTS & 0xFF), &Data);
  ModuleInfo->Health.CECount = Data;

#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "ModuleHealthStatus   = %02x\n", ModuleInfo->Health.ModuleHealthStatus));
  DEBUG((EFI_D_ERROR, "ModuleTemp           = %02x\n", ModuleInfo->Health.ModuleTemp));
  DEBUG((EFI_D_ERROR, "ErrorThresStatus     = %02x\n", ModuleInfo->Health.ErrorThresStatus));
  DEBUG((EFI_D_ERROR, "ErrorThresWarn       = %02x\n", ModuleInfo->Health.ErrorThresWarn));
  DEBUG((EFI_D_ERROR, "NVMLifetime          = %02x\n", ModuleInfo->Health.NVMLifetime));
  DEBUG((EFI_D_ERROR, "UECount              = %02x\n", ModuleInfo->Health.UECount));
  DEBUG((EFI_D_ERROR, "CECount              = %02x\n", ModuleInfo->Health.CECount));
#endif

  JedecSwitchPage(NvDimm, 0);
  ReadSmb(NvDimm, (NVM_CRITICAL_HEALTH_INFO & 0xFF), &Data);
  ModuleInfo->CriticalHealthInfo = Data;
  ReadSmb(NvDimm, (NVM_LIFETIME_PERCENT_WARN_THRESHOLD & 0xFF), &Data);
  ModuleInfo->LifetimePercentWarnTheshold = Data;
  ReadSmb(NvDimm, (NVM_LIFETIME_PERCENT_ERR_THRESHOLD & 0xFF), &Data);
  ModuleInfo->LifetimePercentErrorTheshold = Data;

  ReadSmb(NvDimm, (NVM_TRIGGER_INFO & 0xFF), &Data);
  ModuleInfo->TriggerInformation = Data;

  ReadSmb(NvDimm, (NVM_SAVE_FAILURE_INFO & 0xFF), &Data);
  ModuleInfo->SaveFailInformation = Data;

  ReadSmb(NvDimm, (NVM_VENDOR_PAGE_LOG_SIZE & 0xFF), &Data);
  ModuleInfo->VendorLogPageSize = Data;
  ModuleInfo->VendorLogPage = Data; //  - NOTE: Vendor Log Page not supported, return Page Size instead.
  ReadSmb(NvDimm, (NVM_ERR_INJ_ENABLE & 0xFF), &Data);
  ModuleInfo->ErrorInjEnable = ((Data & BIT2) == 0) ? 0 : 1;

#if VERBOSE_DEBUG
  DEBUG((EFI_D_ERROR, "CriticalHealthInfo           = %02x\n", ModuleInfo->CriticalHealthInfo));
  DEBUG((EFI_D_ERROR, "LifetimePercentWarnTheshold  = %02x\n", ModuleInfo->LifetimePercentWarnTheshold));
  DEBUG((EFI_D_ERROR, "LifetimePercentErrorTheshold = %02x\n", ModuleInfo->LifetimePercentErrorTheshold));
  DEBUG((EFI_D_ERROR, "TriggerInformation           = %02x\n", ModuleInfo->TriggerInformation));
  DEBUG((EFI_D_ERROR, "SaveFailInformation          = %02x\n", ModuleInfo->SaveFailInformation));
  DEBUG((EFI_D_ERROR, "VendorLogPageSize            = %02x\n", ModuleInfo->VendorLogPageSize));
  DEBUG((EFI_D_ERROR, "VendorLogPage                = %02x\n", ModuleInfo->VendorLogPage));
  DEBUG((EFI_D_ERROR, "ErrorInjEnable               = %02x\n", ModuleInfo->ErrorInjEnable));
#endif
}

/**

  HandleARSStart handles Start Address Range Scrub command from
  the OS. Given the Spa and ARS Length, this function figurs out
  all the Nvm Dimms in the interleave set and sends Start ARS
  commands to all these Dimms and also handles the results of
  these commands to these dimms.

  @param Spa       - System Physical Address that needs to for
              which it requires ARS processing
  @param ArsLength - ARS Length
  @return Status

**/
EFI_STATUS
HandleARSStart(
   UINT64         Spa,
   UINT64         ArsLength
   )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT64          SADBaseAddr, SADNextBase, Limit, NextLimit;
  UINT64          SpaStart;
  UINT64          SADSize;
  UINT64          ArsSize;
  UINT8           Skt, Ch, Dimm;
  UINT8           i;
  BOOLEAN         MultipleSADs = FALSE;

  //
  // Given the Spa, lets get the Socket, Ch & Dimm that this Spa is mapped into.
  DEBUG((EFI_D_ERROR, "ARS: ARSStart - SpaStart = 0x%1lx, Length = 0x%1lx\n", Spa, ArsLength));
  Status = GetDimmInfoFromSpa(Spa, &SADBaseAddr, &Skt, &Ch, &Dimm);
  if (Status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "ARS: GetDimmInfoFromSpa failed, returning!!!\n"));
    return (Status);
  }

  DEBUG((EFI_D_ERROR, "ARS: ARSStart - SADBaseAddr = 0x%1lx\n", SADBaseAddr));
  DEBUG((EFI_D_ERROR, "ARS: ARSStart - ARSStart is in Soc = %d, Ch = %d, Dimm = %d\n", Skt, Ch, Dimm));

  // Check and if the range is in the PMEM Region, if it isn't return error status
  if (!IsPMEMRgn(SADBaseAddr)) {
    DEBUG((EFI_D_ERROR, "ARS: ARSStart - Not a PMEM region, returning Invalid Param\n"));
    return (EFI_INVALID_PARAMETER);
  }

  GetSADLimit(SADBaseAddr, &Limit);

  mArsScrubData.NumDimmsLeft = 0;

  //
  // Check and see if this entire ARS Length falls with in one interleave set
  Status = GetDimmInfoFromSpa((Spa + ArsLength)-1, &SADNextBase, &Skt, &Ch, &Dimm);
  DEBUG((EFI_D_ERROR, "ARS: ARSStart - SADNextBase = 0x%1lx\n", SADNextBase));

  // if ((SADBaseAddr + Limit) < (Spa + ArsLength)) {
  if (SADBaseAddr != SADNextBase) {
    //
    // The given ARS range spawns over one SAD Rule.
    DEBUG((EFI_D_ERROR, "ARS: ARS Spawns over multiple SAD Rules\n"));
    MultipleSADs = TRUE;
  }

  if (MultipleSADs) {
    //
    // The first check is to make sure the entire ARS Range provided is in PMEM Region(s).
    SADSize = Limit - SADBaseAddr;
    SADNextBase = SADBaseAddr + SADSize;
    while (SADNextBase < (Spa + ArsLength)) {
      // Check and if the range is in the PMEM Region, if it isn't return error status
      if (!IsPMEMRgn(SADNextBase)) {
        DEBUG((EFI_D_ERROR, "ARS: ARS Spawns over multiple SAD Rules and not all SAD Rules are PMEM Regions\n"));
        return (EFI_INVALID_PARAMETER);
      }
      // Get the limit of this SAD base.
      GetSADLimit(SADNextBase, &NextLimit);
      SADSize = NextLimit - SADNextBase;
      // Get the next SAD Base
      SADNextBase += SADSize;
    }
    // Reset all DPAs in the PMEM Data Structs.
    InitPMEMDpas();

    //
    // Now that we've ensured all PMEM Regions in this ARS Range,
    // Start processing for ARS Start Commands starting with the 1st SAD Rule
    // First is to get all the DpaStart and DpaEnd for all the dimms participating in the
    // ARS Range.
    i = 0;
    PMEMSADBaseAddresses[i] = SADBaseAddr;
    GetSADLimit(SADBaseAddr, &NextLimit);
    SADSize = NextLimit - SADBaseAddr;
    SpaStart = Spa;
    ArsSize = SADSize;
    ArsSize -= (SADSize - (NextLimit - SpaStart));  // Adjust ArsSize to based on SPA
    while ((SpaStart + ArsSize) <= (Spa + ArsLength)) {
      if (ArsSize == 0) break;  // Exit if finished processing SPA range
      Status = ComputeDPAsForARS(SpaStart, PMEMSADBaseAddresses[i], ArsSize);
      if (Status != EFI_SUCCESS) return Status;
      //
      // Get the next one, current one becomes the start of the next one
      i++;
      PMEMSADBaseAddresses[i] = SpaStart + ArsSize;
      SpaStart = PMEMSADBaseAddresses[i];
      // Get the next limit
      GetSADLimit(PMEMSADBaseAddresses[i], &NextLimit);
      ArsSize = NextLimit - PMEMSADBaseAddresses[i];
      if ((PMEMSADBaseAddresses[i] + ArsSize) > (Spa + ArsLength)) {
        Limit = (PMEMSADBaseAddresses[i] + ArsSize) - (Spa + ArsLength);
        NextLimit -= Limit;
        ArsSize = NextLimit - PMEMSADBaseAddresses[i];
      }
    }
  }
  else {
    // ARS Range is covered in one single SAD Rule.
    Status = GetDimmInfoFromSpa(Spa, &SADBaseAddr, &Skt, &Ch, &Dimm);
    Status = ComputeDPAsForARS(Spa, SADBaseAddr, ArsLength);
    if (Status != EFI_SUCCESS) return Status;
    i = 0;
    PMEMSADBaseAddresses[i] = SADBaseAddr;
  }

  Status = StartAddressRangeScrub(Spa, ArsLength);

  return Status;
}

/**

  HandleARSQueryStatus gathers Status from the previous Start
  ARS commands to dimms that were participating in the Start
  Address Range Scrub command from the OS and returns Status as
  specified in the DSM Spec.

  @return Status

**/
EFI_STATUS
HandleARSQueryStatus(
   VOID
   )
{
  EFI_STATUS      Status = QUERY_ARS_STS_SUCCESS;

  // Check if ARS is in progress
  if (mIsArs) Status = EXT_STS_ARS_IN_PROGRESS;

  // Check if ARS has been performed during this boot
  if (!mHasArsPerformed) Status = EXT_STS_NO_ARS_PERFORMED;

  if (mArsDsmHeader.Flags == 1) Status = EXT_STS_ARS_STOPPED;

  if ((Status == EXT_STS_NO_ARS_PERFORMED) || (Status == EXT_STS_ARS_IN_PROGRESS)) {
    mNgnAcpiSmmInterface->OutputLength = 0;
  }
  else {
    CopyMem(&mNgnAcpiSmmInterface->OutputBuffer[0], &mArsDsmHeader, mArsDsmHeader.OutputLength);
    mNgnAcpiSmmInterface->OutputLength = mArsDsmHeader.OutputLength;
  }

  mNgnAcpiSmmInterface->Status = (UINT32)Status;
  return Status;
}

/**

  Clear uncorrectable error from the global list of error records

  @param ClearUcErrSpa - Starting location from which to clear the uncorrectable error
  @param ClearUcErrLen - Length of the region to clear the uncorrectable error from

  @return Status
  @return ActualLenCleared - The range of errors actually cleared by the platform, starting from the requested Clear Error SPA Base

**/
EFI_STATUS
ClearUncorrErr(
  IN  UINT64 ClearUcErrSpa,
  IN  UINT64 ClearUcErrLen,
  OUT UINT64 *ActualLenCleared
 )
{
  UINTN i = 0;
  UINTN j;

  // Make sure valid error records exist
  if (!mHasArsPerformed || (mArsDsmHeader.NumErrorRecords == 0)) {
    DEBUG ((DEBUG_ERROR, "[Clear Error] No error records exist!\n"));
    return CLR_UNCORR_ERR_STS_SUCCESS;
  }

  // Loop through error records and delete any that fall within the input range
  while(i < mArsDsmHeader.NumErrorRecords) {
    if ((mArsDsmHeader.ErrRecords[i].SpaOfErrLoc >= ClearUcErrSpa) &&
        (mArsDsmHeader.ErrRecords[i].SpaOfErrLoc + mArsDsmHeader.ErrRecords[i].Length) <= (ClearUcErrSpa + ClearUcErrLen)) {
      DEBUG ((DEBUG_ERROR, "[Clear Error] Clearing error record: SpaOfErrLoc = 0x%lX, Length = 0x%lX\n", mArsDsmHeader.ErrRecords[i].SpaOfErrLoc, mArsDsmHeader.ErrRecords[i].Length));
      *ActualLenCleared += mArsDsmHeader.ErrRecords[i].Length;
      for (j = i; j < (mArsDsmHeader.NumErrorRecords - 1); j++) mArsDsmHeader.ErrRecords[j] = mArsDsmHeader.ErrRecords[j + 1];
      mArsDsmHeader.NumErrorRecords--;
    }
    else {
      i++;
    }
  }

  return CLR_UNCORR_ERR_STS_SUCCESS;
}

/**

  HandleClearUncorrErr allows system software to clear uncorrectable errors
  from the NVDIMM based on System Physical Address (SPA). Uncorrectable
  errors reported by the Query ARS Status function can be cleared utilizing
  this mechanism.

  @return Status

**/
EFI_STATUS
HandleClearUncorrErr(
   IN UINT64    ClearUcErrSpa,
   IN UINT64    ClearUcErrLen
   )
{
  EFI_STATUS    Status;
  UINT64        ActualLenCleared = 0;   // This is the sum of all error record lengths cleared
  UINT64        PmemRegionStart = 0;
  UINT64        TotalPmemSize = 0;
  BOOLEAN       SpaOutOfRange = TRUE;
  UINT8         PmemIndex;
  UINT8         PmemIndex2;

  // Init return length to 0 since always valid regardless of status
  CopyMem(&mNgnAcpiSmmInterface->OutputBuffer[4], &ActualLenCleared, sizeof(UINT64));

  // Validate SPA range base is aligned to the Clear Uncorrectable Error Range Length Unit Size and
  // the length is an integer multiple of the Clear Uncorrectable Error Range Length Unit Size
  if (((ClearUcErrSpa % mClearUcErrLenUnit) != 0) || ((ClearUcErrLen % mClearUcErrLenUnit) != 0)) {
    return CLR_UNCORR_ERR_INVALID_PARAM;
  }

  // Validate SPA range falls within the PMEM regions
  for (PmemIndex = 0; PmemIndex < MAX_SOCKET; PmemIndex++) {
    if ((ClearUcErrSpa >= mPmemRegions[PmemIndex].BaseAddress) && (ClearUcErrSpa < (mPmemRegions[PmemIndex].BaseAddress + mPmemRegions[PmemIndex].Size))) {
      PmemRegionStart = mPmemRegions[PmemIndex].BaseAddress;
      TotalPmemSize = mPmemRegions[PmemIndex].Size;
      if (PmemIndex < (MAX_SOCKET - 1)) {
        for (PmemIndex2 = PmemIndex + 1; PmemIndex2 < MAX_SOCKET; PmemIndex2++) {
          if ((mPmemRegions[PmemIndex].BaseAddress + TotalPmemSize) == mPmemRegions[PmemIndex2].BaseAddress)
            TotalPmemSize += mPmemRegions[PmemIndex2].Size;
        }
      }
      if ((ClearUcErrSpa >= PmemRegionStart) && (ClearUcErrSpa + ClearUcErrLen) <= (PmemRegionStart + TotalPmemSize)) {
        SpaOutOfRange = FALSE;
      }
      break;
    }
  }
  if (SpaOutOfRange) return CLR_UNCORR_ERR_INVALID_PARAM;

  Status = ClearUncorrErr(ClearUcErrSpa, ClearUcErrLen, &ActualLenCleared);

  CopyMem(&mNgnAcpiSmmInterface->OutputBuffer[4], &ActualLenCleared, sizeof(UINT64));
  mNgnAcpiSmmInterface->OutputLength = sizeof(UINT64);
  mNgnAcpiSmmInterface->Status = (UINT32)Status;

  return EFI_SUCCESS;
}

/**

  Handles ARS complete event

  @param NodeId  - Memory controller ID

  @return Status

**/
EFI_STATUS
HandleArsCompleteEvent(
   IN  UINT8 NodeId
)
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  SCRUBCTL_MC_MAIN_STRUCT         ScrubCtlReg;
  SCRUBADDRESSLO_MC_MAIN_STRUCT   ScrubAddrLoReg;
  SCRUBADDRESSHI_MC_MAIN_STRUCT   ScrubAddrHiReg;
  UINT32                          PcodeMailboxData;
  UINT8                           Socket = NODE_TO_SKT(NodeId);
  UINT8                           Mc = NODE_TO_MC(NodeId);
  UINT8                           Node;

  ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
  DEBUG((DEBUG_ERROR, "[ARS Event] ScrubCtlReg on node %d, socket %d, iMC %d = %X\n", NodeId, Socket, Mc, ScrubCtlReg.Data));
  ScrubAddrLoReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG);
  DEBUG((DEBUG_ERROR, "[ARS Event] ScrubAddrLoReg on node, socket %d, iMC %d %d = %X\n", NodeId, Socket, Mc, ScrubAddrLoReg.Data));
  ScrubAddrHiReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG);
  DEBUG((DEBUG_ERROR, "[ARS Event] ScrubAddrHiReg on node, socket %d, iMC %d %d = %X\n", NodeId, Socket, Mc, ScrubAddrHiReg.Data));

  // Disable scrub
  ScrubCtlReg.Bits.ptl_cmpl = 0;
  ScrubCtlReg.Bits.stop_on_cmpl  = 0;
  ScrubCtlReg.Bits.stop_on_err = 0;
  ScrubCtlReg.Bits.scrub_en  = 0;
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtlReg.Data);

  if (mArsScrubData.SizeLeftToScrub) {
    Status = DoScrub();
  }
  else {
    // ARS complete, re-enable any previous scrubs
    DEBUG((DEBUG_ERROR, "[ARS Event] ARS has completed\n"));
    mIsArs = FALSE;

    // Restore PCU setting for patrol scrub
    SwitchAddressModeToSystem (NodeId);

    PcodeMailboxData  = Mc; // BIT0 MC_INDEX (0 or 1)
    PcodeMailboxData  |= mScrubIntervalSave << 8;
    Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(Socket, MAILBOX_BIOS_CMD_MC_PATROL_SCRUB_INTERVAL, PcodeMailboxData);
    ASSERT_EFI_ERROR(Status);

    for (Socket = 0; Socket < MAX_SOCKET; Socket++){
      for (Mc = 0; Mc < MAX_IMC; Mc++) {
        if (!mSystemMemoryMap->Socket[Socket].imcEnabled[Mc]){
          continue;
        }
        Node = SKTMC_TO_NODE(Socket, Mc);
        ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
        if (mArsScrubData.CurrentNodeScrub[Node]) {
          // Restore MCMAIN_CHKN_BITS_MC_MAIN_REG and re-enable scrub for each node it was running
          mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, MCMAIN_CHKN_BITS_MC_MAIN_REG, mMcMainChknRegSave[Node].Data);
          ReEnablePatrolScrubEngine(Node);
        }
      }
    }
  }

  return Status;
}

/**

  Handles ARS error event

  @param NodeId  - Memory controller ID

  @return Status

**/
EFI_STATUS
HandleArsErrorEvent(
    IN  UINT8 NodeId
)
{
  EFI_STATUS                                Status = EFI_SUCCESS;
  SCRUBCTL_MC_MAIN_STRUCT                   ScrubCtlReg;
  SCRUBADDRESSLO_MC_MAIN_STRUCT             ScrubAddrLoReg;
  SCRUBADDRESSHI_MC_MAIN_STRUCT             ScrubAddrHiReg;
  ERROR_RECORD                              *ErrorRecord;
  PAEP_DIMM                                 pDimm = NULL;
  UINT64                                    SystemAddress;
  UINT64                                    DpaStart;
  UINT32                                    PcodeMailboxData;
  UINT8                                     Node;
  UINTN                                     CurrRecord = 0;
  UINT8                                     Socket = NODE_TO_SKT(NodeId);
  UINT8                                     Mc = NODE_TO_MC(NodeId);
  UINT8                                     Channel;
  UINT8                                     Dimm;

  // Get current ARS address that triggered the error
  ScrubAddrLoReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG);
  ScrubAddrHiReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBADDRESSHI_MC_MAIN_REG);

  // TODO: figure out how to get Ch, Dimm, and DpaStart correctly without MemRas conversion funcs
  Channel = (UINT8)ScrubAddrHiReg.Bits.chnl;
  DpaStart = ScrubAddrLoReg.Bits.rankadd << 6;
  for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
#if VERBOSE_DEBUG
    DEBUG((DEBUG_ERROR, "[ARS Error] GetDimm socket=%d, ch %d dimm %d: \n", Socket, Channel, Dimm));
#endif
    pDimm = GetDimm(Socket, NODECH_TO_SKTCH(NodeId, Channel), Dimm);
    if (pDimm == NULL) {
      continue;
    }
    else break;
  }
  if (pDimm != NULL) {
#if VERBOSE_DEBUG
   DEBUG((DEBUG_ERROR, "[ARS Error] pDimm->SADSpaBase = 0x%lx, DpaStart = 0x%x\n", pDimm->SADSpaBase, DpaStart));
#endif
   Status = XlateDpaToSpa(pDimm, pDimm->SADSpaBase, DpaStart, &SystemAddress);
  } else {
    return EFI_INVALID_PARAMETER;
  }
  DEBUG((DEBUG_ERROR, "[ARS Error] Error at address: 0x%lX\n", SystemAddress));
  DEBUG((DEBUG_ERROR, "[ARS Error] Error at DIMM: Socket = %X, McId = %X, Ch = %X, Dimm = %X\n", Socket, Mc, Channel, Dimm));

  // Check for overflow condition
  if (mArsDsmHeader.NumErrorRecords >= (MAX_NVDIMMS * MAX_ERRORS_PER_DIMM)) {
    DEBUG((DEBUG_ERROR, "[ARS Error] Too many error records! Stopping scrub...\n"));
    mArsDsmHeader.Flags = 1;

    // Set restart SPA and length
    mArsDsmHeader.RestartArsStartSpa = SystemAddress;
    mArsDsmHeader.RestartArsLength = (mArsDsmHeader.StartSpa + mArsDsmHeader.Length) - SystemAddress;

    // Disable scrub
    ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
    ScrubCtlReg.Bits.ptl_cmpl = 0;
    ScrubCtlReg.Bits.stop_on_cmpl  = 0;
    ScrubCtlReg.Bits.stop_on_err = 0;
    ScrubCtlReg.Bits.scrub_en  = 0;
    mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtlReg.Data);

    // Stop ARS, re-enable any previous scrubs
    mIsArs = FALSE;

    // Restore PCU setting for patrol scrub
    SwitchAddressModeToSystem (NodeId);

    PcodeMailboxData  = Mc; // BIT0 MC_INDEX (0 or 1)
    PcodeMailboxData  |= mScrubIntervalSave << 8;
    Status = mCpuCsrAccess->Bios2PcodeMailBoxWrite(Socket, MAILBOX_BIOS_CMD_MC_PATROL_SCRUB_INTERVAL, PcodeMailboxData);
    ASSERT_EFI_ERROR(Status);

    for (Socket = 0; Socket < MAX_SOCKET; Socket++){
      for (Mc = 0; Mc < MAX_IMC; Mc++) {
        if (!mSystemMemoryMap->Socket[Socket].imcEnabled[Mc]){
          continue;
        }
        Node = SKTMC_TO_NODE(Socket, Mc);
        ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
        if (mArsScrubData.CurrentNodeScrub[Node]) {
          // Restore MCMAIN_CHKN_BITS_MC_MAIN_REG and re-enable scrub for each node it was running
          mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, MCMAIN_CHKN_BITS_MC_MAIN_REG, mMcMainChknRegSave[Node].Data);
          ReEnablePatrolScrubEngine(Node);
        }
      }
    }
    return EFI_INVALID_PARAMETER;
  }

  // Check for adjacent error
  while (CurrRecord < mArsDsmHeader.NumErrorRecords) {
    ErrorRecord = &mArsDsmHeader.ErrRecords[CurrRecord];
    if (SystemAddress == (ErrorRecord->SpaOfErrLoc + ErrorRecord->Length)) {
      break;
    }
    CurrRecord++;
  }

  // If no match found then create new error record
  if (CurrRecord == mArsDsmHeader.NumErrorRecords) {
    mNumErrorsCurrRank++;
    if (mNumErrorsCurrRank > mMaxErrorsPerRank) {
      // Reached max errors per rank, move to next rank
      return HandleArsCompleteEvent(NodeId);
    }

    ErrorRecord = &mArsDsmHeader.ErrRecords[mArsDsmHeader.NumErrorRecords];
    ErrorRecord->NfitHandle = (Socket << 12) | (Mc << 8) | (Channel << 4) | Dimm;
    ErrorRecord->Flags = 0;
    ErrorRecord->SpaOfErrLoc = SystemAddress;
    ErrorRecord->Length = 1; // TODO: is length 1 byte or whole rank?

    mArsDsmHeader.NumErrorRecords++;                      // Increment total error record count
    mArsDsmHeader.OutputLength += sizeof (ERROR_RECORD);  // Update the Ars Dsm struct length to account for the Error Record just added
  }
  // Adjacent error found, update previous error record
  else {
    ErrorRecord = &mArsDsmHeader.ErrRecords[CurrRecord];
    ErrorRecord->Length += 1; // TODO: is length 1 byte or whole rank?
  }

  // Restart scrub
  mCpuCsrAccess->WriteMcCpuCsr (Socket, Mc, SCRUBADDRESSLO_MC_MAIN_REG,(ScrubAddrLoReg.Data + 1));
  ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
  ScrubCtlReg.Bits.ptl_stopped = 0;
  ScrubCtlReg.Bits.startscrub = 1;
  mCpuCsrAccess->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtlReg.Data);

  return Status;
}

/**

  Checks and handles when ARS has completed or encountered and error

  @param DispatchHandle - Dispatch handle (unused for now!)
  @param Dispatch Context - Context information (unused for now!)
  @param CommBuffer     - Buffer used for communication between caller/callback function (unused for now!)
  @param CommBufferSize - size of communication buffer (unused for now!)

  @retval EFI_SUCCESS / Error code

**/
EFI_STATUS
EFIAPI
CheckAndHandleArsEvent(
   IN EFI_HANDLE        DispatchHandle,
   IN CONST VOID        *DispatchContext,
   IN OUT   VOID        *CommBuffer,
   IN OUT   UINTN       *CommBufferSize
   )
{
  UINT8                       Socket;
  UINT8                       Mc;
  UINT8                       Node;
  EFI_STATUS                  Status = EFI_SUCCESS;
  SCRUBCTL_MC_MAIN_STRUCT     ScrubCtlReg;

  if (!mIsArs) return Status;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++){
    for (Mc = 0; Mc < MAX_IMC; Mc++) {
      if (!mSystemMemoryMap->Socket[Socket].imcEnabled[Mc]){
        continue;
      }

      Node = SKTMC_TO_NODE(Socket, Mc);
      ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
      if(ScrubCtlReg.Bits.scrub_en) {
        if(ScrubCtlReg.Bits.ptl_cmpl){
          Status = HandleArsCompleteEvent(Node);
        }
        if(ScrubCtlReg.Bits.ptl_stopped){
          DEBUG((DEBUG_ERROR, "[ARS Event] ARS encountered an error\n"));
          Status = HandleArsErrorEvent(Node);
        }
      }
    }
  }
  return Status;
}

/**

  This routine handles Address Range Scrub related commands from
  OS.

  @param VOID - No direct input

  @return Status

**/
EFI_STATUS
HandleARSCommandsFromOS(
   VOID
   )
{
  UINT32          *pBuf;
  EFI_STATUS      Status = ARS_INVALID_PARAM;

  switch (mNgnAcpiSmmInterface->FunctionIndex) {
  case 1:
    // Query ARS Capabilities - This should return success status and the extended status bytes
    // should just have ARS Enabled for PMEM bit set and set the Output size and Bios SMM Buffer size in it.
    Status = PMEM_SCRUB_SUPPORTED;
    mNgnAcpiSmmInterface->Status = (UINT32)(Status << 16) | (EFI_SUCCESS);
    mNgnAcpiSmmInterface->OutputLength = sizeof(UINT64);
    pBuf = (UINT32 *)&mNgnAcpiSmmInterface->OutputBuffer[0];
    *pBuf = OUTPUT_BUF_SIZE;
    *(pBuf + 1) = mClearUcErrLenUnit;     // Clear Uncorrectable Error Range Length Unit Size
    break;

  case 2:
    // Start Address Range Scrubbing
    // We only PMEM Scrub, not volatile scrub
    if (mNgnAcpiSmmInterface->ArsType & VOLATILE_SCRUB_SUPPORTED) {
      Status = ARS_INVALID_PARAM;
    }
    else {
      ArsStartSpa = mNgnAcpiSmmInterface->ArsStartAddress;
      // Align the Spa to 256 byte boundary
      ArsStartSpa = (ArsStartSpa / ARS_ALIGNMENT_SIZE) * ARS_ALIGNMENT_SIZE;
      ArsLength = mNgnAcpiSmmInterface->ArsLength;
      Status = HandleARSStart(ArsStartSpa, mNgnAcpiSmmInterface->ArsLength);
    }
    mNgnAcpiSmmInterface->Status = (UINT32)Status;
    mNgnAcpiSmmInterface->OutputLength = sizeof(UINT32);
    pBuf = (UINT32 *)&mNgnAcpiSmmInterface->OutputBuffer[0];
    *pBuf = mArsTimeEstimate;
    break;

  case 3:
    // Query Address Range Scrubbing Status
    Status = HandleARSQueryStatus();
    // Update Specific Area in ACPI with data, is done in the above handler
    // Give the status back what was returned.
    break;

  case 4:
    // Clear Uncorrectable Error
    // TODO: use ArsStartAddress & ArsLength?? Might need to update _DSM to include this new function
    Status = HandleClearUncorrErr(mNgnAcpiSmmInterface->ArsStartAddress, mNgnAcpiSmmInterface->ArsLength);
    break;

  default:
    Status = ARS_INVALID_PARAM;
    break;
  }
  //
  // The ARS Handlers should be filling in the Ngn Output buffer with output data.
  if (Status != EFI_SUCCESS) {
    // Tell the that Scrub is not supported
    mNgnAcpiSmmInterface->Status = (UINT32)Status;
    mNgnAcpiSmmInterface->OutputLength = 0;
  }
  return Status;
}

EFI_STATUS
ServeNgnCommandsFromOS(
   IN  EFI_HANDLE            DispatchHandle,
   IN CONST EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
   IN OUT   VOID             *CommBuffer,     OPTIONAL
   IN OUT   UINTN            *CommBufferSize  OPTIONAL
   )
{
  EFI_STATUS      Status = ARS_INVALID_PARAM;
  UINT8           PmemIndex = 0;
  UINT8           Socket = 0;
  UINT8           ImcId = 0;
  UINT8           Ch = 0;
  UINT8           Dimm = 0, DimmIndex = 0;
  UINT8           Data = 0;
  UINT8           Index;
  NVDIMM          NvDimm;

  //
  // The new encoding is that the device handle of _DSM commands for the root (ARS Commands) will be
  // 0xFFFFFFFF. Current spec has 4 ARS related functions
  if ((mNgnAcpiSmmInterface->FunctionIndex > 0) && (mNgnAcpiSmmInterface->FunctionIndex < MAX_ROOT_DEVICE_FCNS) && \
         (mNgnAcpiSmmInterface->DeviceHandle == 0xFFFFFFFF)) {
    // ARS commands to use BIOS MB only.
    Status = HandleARSCommandsFromOS();
    return Status;
  }
  else {
    // _DSM Interface for Byte Addressable Energy Backed Function Class, Function Interface 1

    // Get socket ch and dimm from NgnCommandDeviceHandle in ACPI shared space
    GetDimmInfoFromDevHandle(&Socket, &ImcId, &Ch, &Dimm, mNgnAcpiSmmInterface->DeviceHandle);

    DEBUG((EFI_D_ERROR, "DSM Passing: Socket %d, ImcId %d, Ch%d, Dimm %d\n", Socket, ImcId, Ch, Dimm));;
    DEBUG((EFI_D_ERROR, "ENTER PERBYTE DSM\n"));;

    Status = ARS_INVALID_PARAM;

    for (PmemIndex = 0; PmemIndex < MAX_PMEM_REGIONS; PmemIndex++) {
      DEBUG((EFI_D_ERROR, "Size:0x%lX\n",(mPmemRegions[PmemIndex].Size)));
      if (mPmemRegions[PmemIndex].Size) {
        for (DimmIndex = 0; DimmIndex < mPmemRegions[PmemIndex].NumNvDimms; DimmIndex++) {
          DEBUG((EFI_D_ERROR, "Socket:%d ,IMCID: %d,Ch: %d,DIMM: %d\n",mPmemRegions[PmemIndex].NvDimms[DimmIndex].SocketId,mPmemRegions[PmemIndex].NvDimms[DimmIndex].ImcId,mPmemRegions[PmemIndex].NvDimms[DimmIndex].Ch,mPmemRegions[PmemIndex].NvDimms[DimmIndex].Dimm));
          if (mPmemRegions[PmemIndex].NvDimms[DimmIndex].SocketId == Socket &&
                  mPmemRegions[PmemIndex].NvDimms[DimmIndex].ImcId == ImcId &&
                  mPmemRegions[PmemIndex].NvDimms[DimmIndex].Ch == Ch &&
                  mPmemRegions[PmemIndex].NvDimms[DimmIndex].Dimm == Dimm) {
            NvDimm = mPmemRegions[PmemIndex].NvDimms[DimmIndex];
            Status = EFI_SUCCESS;
            break;
          }
        }
      }
      if(Status == EFI_SUCCESS)
        break;
    }
    DEBUG((EFI_D_ERROR, "DimmIndex, PmemIndex:%d ,%d\n", DimmIndex, PmemIndex));

    if(DimmIndex != 0 || PmemIndex != 0) {
      DimmIndex = PmemIndex = 0;
      DEBUG((EFI_D_ERROR, "Forcing DIMMINDEX AND PMEMINDEX TO ZERO\n"));
    }
    DEBUG((EFI_D_ERROR, "mNgnAcpiSmmInterface->FunctionIndex:%d\n", mNgnAcpiSmmInterface->FunctionIndex));

    if (Status == EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "mNgnAcpiSmmInterface->FunctionIndex:2:%d\n", mNgnAcpiSmmInterface->FunctionIndex));
      // Determine Function Index
      switch (mNgnAcpiSmmInterface->FunctionIndex) {
        case 1:   // Get NVDIMM-N Identification
          CopyMem(mNgnAcpiSmmInterface->OutputBuffer, &mPmemRegions[PmemIndex].NvDimms[DimmIndex].DimmId, sizeof(NVDIMM_IDENTIFICATION));
          mNgnAcpiSmmInterface->OutputLength = sizeof(NVDIMM_IDENTIFICATION);
          break;

        case 2:   // Get Save Operation Requirement
          CopyMem(mNgnAcpiSmmInterface->OutputBuffer, &mPmemRegions[PmemIndex].NvDimms[DimmIndex].SaveOpReg, sizeof(SAVE_OPERATION_REQ));
          mNgnAcpiSmmInterface->OutputLength = sizeof(SAVE_OPERATION_REQ);
          break;

        case 3:   // Get Energy Source Identification
          CopyMem(mNgnAcpiSmmInterface->OutputBuffer, &mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESInfo, sizeof(ENERGY_SOURCE_IDENTIFICATION));
          mNgnAcpiSmmInterface->OutputLength = sizeof(ENERGY_SOURCE_IDENTIFICATION);
          break;

        case 4:   // Get Last Backup Information
          ((NVDIMM_BACKUP_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->TriggerInformation = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.TriggerInformation;
          ((NVDIMM_BACKUP_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->SaveFailInformation = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.SaveFailInformation;
          mNgnAcpiSmmInterface->OutputLength = sizeof(NVDIMM_BACKUP_INFO);
          break;

        case 5:   // Get NVDIMM Thresholds
          ((NVDIMM_THRESHOLD *)mNgnAcpiSmmInterface->OutputBuffer)->NVDIMMLifetimePercentWarnTheshold = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.LifetimePercentWarnTheshold;
          ((NVDIMM_THRESHOLD *)mNgnAcpiSmmInterface->OutputBuffer)->NVDIMMLifetimePercentErrorTheshold = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.LifetimePercentErrorTheshold;
          mNgnAcpiSmmInterface->OutputLength = sizeof(NVDIMM_THRESHOLD);
          break;

        case 6:   // Set NVDIMM Lifetime Percentage Warning Threshold
          if (mNgnAcpiSmmInterface->InputLength != sizeof(NVDIMM_LIFETIME_WARN_THRESHOLD))
            Status = ARS_INVALID_PARAM;
          else {
            mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.LifetimePercentWarnTheshold = mNgnAcpiSmmInterface->InputBuffer[0];
            JedecSwitchPage(&NvDimm, 0);
            WriteSmb(&NvDimm, (NVM_LIFETIME_PERCENT_WARN_THRESHOLD & 0xFF), &Data);
          }
          break;

        case 7:   // Get Energy Source Threshold
          ((ENERGY_SOURCE_THRESHOLD *)mNgnAcpiSmmInterface->OutputBuffer)->ESLifetimePercentWarnTheshold = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESLifetimePercentWarnTheshold;
          ((ENERGY_SOURCE_THRESHOLD *)mNgnAcpiSmmInterface->OutputBuffer)->ESLifetimePercentErrorThreshold = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESLifetimePercentErrorThreshold;
          ((ENERGY_SOURCE_THRESHOLD *)mNgnAcpiSmmInterface->OutputBuffer)->ESTempWarnTheshold = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESLifetimeTempWarnTheshold;
          ((ENERGY_SOURCE_THRESHOLD *)mNgnAcpiSmmInterface->OutputBuffer)->ESTempErrorTheshold = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESLifetimeTempErrorTheshold;
          mNgnAcpiSmmInterface->OutputLength = sizeof(ENERGY_SOURCE_THRESHOLD);
          break;

        case 8:   // Set Energy Source Lifetime Warning Threshold
          if (mNgnAcpiSmmInterface->InputLength != sizeof(ENERGY_SOURCE_LIFETIME_WARN_THRESHOLD))
            Status = ARS_INVALID_PARAM;
          else {
            mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESLifetimePercentWarnTheshold = mNgnAcpiSmmInterface->InputBuffer[0];
            JedecSwitchPage(&NvDimm, 0);
            WriteSmb(&NvDimm, (NVM_ES_LIFETIME_PERCENT_WARN_THRESHOLD & 0xFF), &Data);
          }
          break;

        case 9:   // Set Energy Source Temperature Warning Threshold
          if (mNgnAcpiSmmInterface->InputLength != sizeof(ENERGY_SOURCE_TEMPERATURE_WARN_THRESHOLD))
            Status = ARS_INVALID_PARAM;
          else {
            mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESLifetimeTempWarnTheshold = mNgnAcpiSmmInterface->InputBuffer[0];
            JedecSwitchPage(&NvDimm, 0);
            WriteSmb(&NvDimm, (NVM_ES_LIFETIME_TEMP_WARN_THRESHOLD & 0xFF), &Data);
          }
          break;

        case 10:  // Get Critical Health Info
          ((CRITICAL_HEALTH_INFO *)&mNgnAcpiSmmInterface->OutputBuffer)->CriticalHealthInfo = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.CriticalHealthInfo;
          mNgnAcpiSmmInterface->OutputLength = sizeof(CRITICAL_HEALTH_INFO);
          break;

        case 11:  // Get NVDIMM Health Info
          mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.Health.ModuleHealthStatus = 0;
          mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.Health.ModuleTemp = 0x1C1C;
          mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.Health.ErrorThresStatus = 0x0A;
          mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.Health.ErrorThresWarn = 0x0A;
          mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.Health.NVMLifetime = 0x63;
          mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.Health.UECount = 0;
          mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.Health.CECount = 0;
          CopyMem(mNgnAcpiSmmInterface->OutputBuffer, &mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.Health, sizeof(NVDIMM_HEALTH_INFO));
          mNgnAcpiSmmInterface->OutputLength = sizeof(NVDIMM_HEALTH_INFO);
          break;

        case 12:  // Get Energy Source Health Info
          if(JedecSwitchPage(&NvDimm, 1) == EFI_SUCCESS) {
            ReadSmb(&NvDimm, (NVM_ES_LIFETIME_PERCENTAGE & 0xFF), &Data);
            DEBUG((EFI_D_ERROR, "1.NVM_ES_LIFETIME_PERCENTAGE = 0x%1lx\n",Data));
            mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESLifeTime = Data;
            ReadSmb(&NvDimm, (NVM_ES_CURRENT_TEMP & 0xFF), &Data);
            DEBUG((EFI_D_ERROR, "1.NVM_ES_CURRENT_TEMP = 0x%1lx\n",Data));
            mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESTemp0 = Data;
            ReadSmb(&NvDimm, ((NVM_ES_CURRENT_TEMP + 1) & 0xFF), &Data);
            DEBUG((EFI_D_ERROR, "1.NVM_ES_CURRENT_TEMP = 0x%1lx\n",Data));
            mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESTemp1 = Data;
            ReadSmb(&NvDimm, (NVM_ES_TOTAL_RUNTIME & 0xFF), &Data);
            DEBUG((EFI_D_ERROR, "1.NVM_ES_TOTAL_RUNTIME = 0x%1lx\n",Data));
            mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESRuntime0 = Data;
            ReadSmb(&NvDimm, ((NVM_ES_TOTAL_RUNTIME + 1) & 0xFF), &Data);
            DEBUG((EFI_D_ERROR, "1.NVM_ES_TOTAL_RUNTIME = 0x%1lx\n",Data));
          }

          ((ENERGY_STORAGE_HEALTH_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->ESLifeTime = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESLifeTime;
          ((ENERGY_STORAGE_HEALTH_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->ESTemp0 = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESTemp0;
          ((ENERGY_STORAGE_HEALTH_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->ESTemp1 = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESTemp1;
          ((ENERGY_STORAGE_HEALTH_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->ESRuntime0 = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESRuntime0;
          ((ENERGY_STORAGE_HEALTH_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->ESRuntime1 = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ESLifeInfo.ESRuntime1;
          ((ENERGY_STORAGE_HEALTH_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->Rsvd[0] = 0;
          ((ENERGY_STORAGE_HEALTH_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->Rsvd[1] = 0;

          mNgnAcpiSmmInterface->OutputLength = sizeof(ENERGY_STORAGE_HEALTH_INFO);
          break;

        case 13:  // Get Operational Statistics
          CopyMem(mNgnAcpiSmmInterface->OutputBuffer, &mPmemRegions[PmemIndex].NvDimms[DimmIndex].OpInfo, sizeof(OPERATION_STATISTIC_INFO));
          mNgnAcpiSmmInterface->OutputLength = sizeof(OPERATION_STATISTIC_INFO);
          break;

        case 14:  // VENDOR_LOG_PAGE_SIZE
          ((VENDOR_LOG_PAGE_SIZE *)mNgnAcpiSmmInterface->OutputBuffer)->VendorLogPageSize = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.VendorLogPageSize;
          mNgnAcpiSmmInterface->OutputLength = sizeof(VENDOR_LOG_PAGE_SIZE);
          break;

        case 15:  // VENDOR_LOG_PAGE_SIZE - NOTE: Vendor Log Page not supported, return Page Size instead.
          ((VENDOR_LOG_PAGE *)mNgnAcpiSmmInterface->OutputBuffer)->VendorLogPage = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.VendorLogPage;
          mNgnAcpiSmmInterface->OutputLength = sizeof(VENDOR_LOG_PAGE);
          break;

        case 16:  // QUERY ERROR INJ STATUS: error injection is disabled
          ((QUERY_ERROR_INJ_STATUS *)mNgnAcpiSmmInterface->OutputBuffer)->ErrorInjEnable = mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.ErrorInjEnable;
          mNgnAcpiSmmInterface->OutputLength = sizeof(QUERY_ERROR_INJ_STATUS);
          break;

        case 17:  // Inject Error
          if (mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.ErrorInjEnable == 0) {
            Status = ARS_INVALID_PARAM;
          }
          else {
            JedecSwitchPage(&NvDimm, 2);
            // Fill up what to inject!
            Data = ((INJECT_ERROR *)mNgnAcpiSmmInterface->InputBuffer)->InjectOpsFailures;  // INJECT_OPS_FAILURES
            WriteSmb(&NvDimm, (NVM_INJECT_OP_FAILURES & 0xFF), &Data);
            if (Data & BIT7) {
              // Byte 1 – If INJECT_BAD_BLOCKS is 1 (bit 7 of Byte 0), this is INJECT_BAD_BLOCK_CAP (2, 0x67). Otherwise, it shall be 0.
              Data = ((INJECT_ERROR *)mNgnAcpiSmmInterface->InputBuffer)->InjectBadBlocks;
              WriteSmb(&NvDimm, (NVM_INJECT_BAD_BLOCK_CAP & 0xFF), &Data);
            }

            Data = ((INJECT_ERROR *)mNgnAcpiSmmInterface->InputBuffer)->InjectESFailures;
            WriteSmb(&NvDimm, (NVM_INJECT_ES_FAILURES & 0xFF), &Data);
            Data = ((INJECT_ERROR *)mNgnAcpiSmmInterface->InputBuffer)->InjectFwFailures;
            WriteSmb(&NvDimm, (NVM_FW_UDPATE_FAILURES & 0xFF), &Data);
          }
          mNgnAcpiSmmInterface->OutputLength = 0;
          break;

        case 18:  // Get Injected Errors
          JedecSwitchPage(&NvDimm, 2);
          ReadSmb(&NvDimm, (NVM_INJECT_OP_FAILURES & 0xFF), &Data);
          ((INJECT_ERROR_FAILURE_STATUS *)mNgnAcpiSmmInterface->OutputBuffer)->InjectOpsFailures = Data;
          ReadSmb(&NvDimm, (NVM_INJECT_BAD_BLOCK_CAP & 0xFF), &Data);
          ((INJECT_ERROR_FAILURE_STATUS *)mNgnAcpiSmmInterface->OutputBuffer)->InjectBadBlocks = Data;
          ReadSmb(&NvDimm, (NVM_INJECT_ES_FAILURES & 0xFF), &Data);
          ((INJECT_ERROR_FAILURE_STATUS *)mNgnAcpiSmmInterface->OutputBuffer)->InjectESFailures = Data;
          ReadSmb(&NvDimm, (NVM_FW_UDPATE_FAILURES & 0xFF), &Data);
          ((INJECT_ERROR_FAILURE_STATUS *)mNgnAcpiSmmInterface->OutputBuffer)->InjectFwFailures = Data;
          mNgnAcpiSmmInterface->OutputLength = sizeof(INJECT_ERROR_FAILURE_STATUS);
          Status = EFI_SUCCESS;
          break;

        case 19:  // Erase NVDIMM STATUS (Bit[3]-Start Erase)
          JedecSwitchPage(&NvDimm, 0);
          Status = ReadSmb(&NvDimm, (NVM_ERASE_IMAGE & 0xFF), &Data);
          if (Status == SUCCESS) {
            Data |= (1 << 3);
            Status = WriteSmb(&NvDimm, (NVM_ERASE_IMAGE & 0xFF), &Data);
          }

          if (Status != SUCCESS) Status = 3; // I2C Error!
          mNgnAcpiSmmInterface->OutputLength = 0;
          break;

        case 20:  // Arm NVDIMM STATUS (Bit0 set)
          JedecSwitchPage(&NvDimm, 0);
          Status = ReadSmb(&NvDimm, (NVM_ARM & 0xFF), &Data);
          if (Status == SUCCESS) {
            Data |= 1;
            Status = WriteSmb(&NvDimm, (NVM_ARM & 0xFF), &Data);
          }

          if (Status != SUCCESS) Status = 3; // I2C Error!
          mNgnAcpiSmmInterface->OutputLength = 0;
          break;

        case 21:  // Reset Factory Default STATUS (Bit0 set)
          JedecSwitchPage(&NvDimm, 0);
          Status = ReadSmb(&NvDimm, (NVM_RESET_FACTORY_DEFAULT & 0xFF), &Data);
          if (Status == SUCCESS) {
            Data |= 1;
            Status = WriteSmb(&NvDimm, (NVM_RESET_FACTORY_DEFAULT & 0xFF), &Data);
          }

          if (Status != SUCCESS) Status = 3; // I2C Error!
          mNgnAcpiSmmInterface->OutputLength = 0;
          break;

        case 22:  // Start Firmware Update (Bit[3:0]-Select FW SLOT)
          // Get firmware slot
          JedecSwitchPage(&NvDimm, 3);
          Status = ReadSmb(&NvDimm, (NVM_FW_SLOT & 0xFF), &Data);
          if (Status == SUCCESS) {
            Data &= 0xF0;
            Data |= mNgnAcpiSmmInterface->InputBuffer[0] & 0x0F;
            if ((Status = WriteSmb(&NvDimm, (NVM_FW_SLOT & 0xFF), &Data)) == SUCCESS) {
              JedecSwitchPage(&NvDimm, 0);
              ReadSmb(&NvDimm, (NVM_START_FW_UPDATE & 0xFF), &Data);
              Data |= 1; // (Bit[0] = 1 enable fw update mode)
              Status = WriteSmb(&NvDimm, (NVM_START_FW_UPDATE & 0xFF), &Data);
            }
          }

          if (Status != SUCCESS) Status = 3; // I2C Error!
          mNgnAcpiSmmInterface->OutputLength = 0;
          break;

        case 23:  // Send Firmware Update Data
          if (((SEND_FW_UPDATE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->RegionLength > READ_TYPE_DATA_SIZE)
            Status = ARS_INVALID_PARAM;
          else {
            JedecSwitchPage(&NvDimm, 3);
            Data = ((SEND_FW_UPDATE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->RegionId & 0xFF;
            WriteSmb(&NvDimm, (NVM_REGION_ID & 0xFF), &Data);
            Data = (((SEND_FW_UPDATE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->RegionId >> 8) & 0xFF;
            WriteSmb(&NvDimm, ((NVM_REGION_ID + 1) & 0xFF), &Data);
            Data = ((SEND_FW_UPDATE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->BlockId;
            WriteSmb(&NvDimm, (NVM_BLOCK_ID & 0xFF), &Data);
            for (Index = 0; Index < ((SEND_FW_UPDATE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->RegionLength; Index++) {
              Data = ((SEND_FW_UPDATE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->FwData[Index];
              WriteSmb(&NvDimm, ((NVM_DATA_RD_WR + Index) & 0xFF), &Data);
            }
          }

          mNgnAcpiSmmInterface->OutputLength = 0;
          break;

        case 24:  // Finish Firmware Update STATUS
          JedecSwitchPage(&NvDimm, 3);
          ReadSmb(&NvDimm, (NVM_START_FW_UPDATE & 0xFF), &Data);
          Data &= 0xFE; // (Bit[0] = 0 - disable fw update mode)
          Status = WriteSmb(&NvDimm, (NVM_START_FW_UPDATE & 0xFF), &Data);
          mNgnAcpiSmmInterface->OutputLength = 0;
          break;

        case 25:  // Select Firmware Image Slot
          Data = ((START_FW_UDPATE *)mNgnAcpiSmmInterface->InputBuffer)->FwSlot;
          JedecSwitchPage(&NvDimm, 3);
          WriteSmb(&NvDimm, (NVM_FW_SLOT & 0xFF), &Data);
          mNgnAcpiSmmInterface->OutputLength = 0;
          break;

        case 26:  // Get Firmware Info
          Data = ((START_FW_UDPATE *)mNgnAcpiSmmInterface->InputBuffer)->FwSlot;
          JedecSwitchPage(&NvDimm, 3);
          WriteSmb(&NvDimm, (NVM_FW_SLOT & 0xFF), &Data);
          if (Data == 0) { // FW SLOT 0
            ReadSmb(&NvDimm, (NVM_FW_SLOT0_REV & 0xFF), &Data);
            ((FW_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->SlotFwVer0 = Data;
            ReadSmb(&NvDimm, ((NVM_FW_SLOT0_REV + 1) & 0xFF), &Data);
            ((FW_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->SlotFwVer1 = Data;
          }else {      // FW SLOT 1
            ReadSmb(&NvDimm, (NVM_FW_SLOT1_REV & 0xFF), &Data);
            ((FW_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->SlotFwVer0 = Data;
            ReadSmb(&NvDimm, ((NVM_FW_SLOT1_REV + 1) & 0xFF), &Data);
            ((FW_INFO *)mNgnAcpiSmmInterface->OutputBuffer)->SlotFwVer1 = Data;
          }

          mNgnAcpiSmmInterface->OutputLength = sizeof(FW_INFO);
          break;

        case 27:  // I2C Read
          Data = ((I2C_READ *)mNgnAcpiSmmInterface->InputBuffer)->Page;
          JedecSwitchPage(&NvDimm, Data);
          ReadSmb(&NvDimm, ((I2C_READ *)mNgnAcpiSmmInterface->InputBuffer)->Offset, &Data);
          mNgnAcpiSmmInterface->OutputBuffer[0] = Data;
          mNgnAcpiSmmInterface->OutputLength = 1;
          break;

        case 28:  // I2C Write
          Data = ((I2C_WRITE *)mNgnAcpiSmmInterface->InputBuffer)->Page;
          JedecSwitchPage(&NvDimm, Data);
          Data = ((I2C_WRITE *)mNgnAcpiSmmInterface->InputBuffer)->Data;
          Status = WriteSmb(&NvDimm, ((I2C_WRITE *)mNgnAcpiSmmInterface->InputBuffer)->Offset, &Data);
          if (Status != SUCCESS) Status = 3; // I2C Error!
          mNgnAcpiSmmInterface->OutputLength = 0;
          break;

        case 29:  // Read Typed Data
          JedecSwitchPage(&NvDimm, 3);
          Data = ((READ_TYPE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->Type;
          WriteSmb(&NvDimm, (NVM_DATA_TYPE & 0xFF), &Data);
          Data = ((READ_TYPE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->RegionId[0];
          WriteSmb(&NvDimm, (NVM_REGION_ID & 0xFF), &Data);
          Data |= ((READ_TYPE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->RegionId[1];
          WriteSmb(&NvDimm, ((NVM_REGION_ID + 1) & 0xFF), &Data);
          Data = ((READ_TYPE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->BlockId;
          WriteSmb(&NvDimm, (NVM_BLOCK_ID & 0xFF), &Data);
          for (Index = 0; Index < READ_TYPE_DATA_SIZE; Index++) {
            ReadSmb(&NvDimm, ((NVM_DATA_RD_WR + Index) & 0xFF), &Data);
            ((READ_TYPE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->Data[Index] = Data;
          }

          mNgnAcpiSmmInterface->OutputLength = READ_TYPE_DATA_SIZE;
          break;

        case 30:  // Write Typed Data
          JedecSwitchPage(&NvDimm, 3);
          Data = ((WRITE_TYPE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->Type;
          WriteSmb(&NvDimm, (NVM_DATA_TYPE & 0xFF), &Data);
          Data = ((WRITE_TYPE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->RegionId[0];
          WriteSmb(&NvDimm, (NVM_REGION_ID & 0xFF), &Data);
          Data |= ((WRITE_TYPE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->RegionId[1];
          WriteSmb(&NvDimm, ((NVM_REGION_ID + 1) & 0xFF), &Data);
          Data = ((WRITE_TYPE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->BlockId;
          WriteSmb(&NvDimm, (NVM_BLOCK_ID & 0xFF), &Data);
          for (Index = 0; Index < READ_TYPE_DATA_SIZE; Index++) {
            Data = ((READ_TYPE_DATA *)mNgnAcpiSmmInterface->InputBuffer)->Data[Index];
            WriteSmb(&NvDimm, ((NVM_DATA_RD_WR + Index) & 0xFF), &Data);
          }

          mNgnAcpiSmmInterface->OutputLength = 0;
          break;

        case 31:  // Set Memory Error Counters
          JedecSwitchPage(&NvDimm, 2);
          Data = ((SET_MEMORY_ERROR_COUNTER *)mNgnAcpiSmmInterface->InputBuffer)->UECounter;
          mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.Health.UECount = Data;
          WriteSmb(&NvDimm, (NVM_UCECC_COUNT_REG & 0xFF), &Data);
          Data = ((SET_MEMORY_ERROR_COUNTER *)mNgnAcpiSmmInterface->InputBuffer)->CEThresholdCounter;
          //  mPmemRegions[PmemIndex].NvDimms[DimmIndex].ModuleInfo.Health.CEThresholdCounter = Data; TODO
          WriteSmb(&NvDimm, (NVM_CECC_COUNT_REG & 0xFF), &Data);
          mNgnAcpiSmmInterface->OutputLength = 0;
          break;

        default:
          Status = ARS_INVALID_PARAM;
      } // case
    } // if success

    mNgnAcpiSmmInterface->Status = (UINT32)Status;
    if (Status != EFI_SUCCESS) {
      mNgnAcpiSmmInterface->OutputLength = 0;
    }
  }

  DEBUG((EFI_D_ERROR, " mNgnAcpiSmmInterface outputlength::%d ", mNgnAcpiSmmInterface->OutputLength));
  DEBUG((EFI_D_ERROR, "mNgnAcpiSmmInterface->STATUS :%d\n", mNgnAcpiSmmInterface->Status));
  for(Index = 0; Index < mNgnAcpiSmmInterface->OutputLength; Index++)
    DEBUG((EFI_D_ERROR, " %x ", mNgnAcpiSmmInterface->OutputBuffer[Index]));
  DEBUG((EFI_D_ERROR, " \n"));
  DEBUG((EFI_D_ERROR, "ServeNgnCommandsFromOS: EXIT\n"));

  return Status;
}

/**

Routine Description:JedecNvDimmStart gathers Control Region
Data from the SADs and initializes the internal data structs for
later enablement of the read/write of the NVDIMM registers thru the
SAD interface.

  @retval UINT32 - status

**/
EFI_STATUS
EFIAPI
JedecNvDimmStart (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  VOID                 *HobList;
  EFI_HOB_GUID_TYPE    *GuidHob;
  EFI_STATUS           Status = EFI_SUCCESS;
  EFI_HANDLE           Handle = NULL;
  BOOLEAN              InSmm = FALSE;
  EFI_IIO_UDS_PROTOCOL *mIioUds;
  struct SystemMemoryMapHob       *TempSystemMemoryMap;

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, &mSmmBase);
  if (mSmmBase == NULL) {
    InSmm = FALSE;
  } else {
    mSmmBase->InSmm (mSmmBase, &InSmm);
    mSmmBase->GetSmstLocation (mSmmBase, &mSmst);
  }

  ZeroMem (&mJedecNvDimmProtocol, sizeof (EFI_JEDEC_NVDIMM_PROTOCOL));
  ZeroMem (&CrInfo, sizeof (CR_INFO));
  mJedecNvDimmProtocol.UpdateJedecNvDimmAcpiTable    = UpdateJedecNvDimmAcpiTable;

  if (!InSmm) {
    //Allocate memory for SystemMemoryHob
    Status = gBS->AllocatePool (EfiBootServicesData, sizeof (struct SystemMemoryMapHob), (VOID **) &mSystemMemoryMap);

    Status = gBS->LocateProtocol(&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
    ASSERT_EFI_ERROR (Status);

    Status = gBS->AllocatePool( EfiRuntimeServicesData, sizeof(PMEM_REGION)*MAX_PMEM_REGIONS, &mPmemRegions);
    ASSERT_EFI_ERROR(Status);
    ZeroMem(mPmemRegions, sizeof(PMEM_REGION)*MAX_PMEM_REGIONS);
  }
  else if (InSmm) {
    //Allocate memory for SystemMemoryHob
    Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (struct SystemMemoryMapHob), (VOID **) &mSystemMemoryMap);

    Status = mSmst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
    ASSERT_EFI_ERROR (Status);

    Status = mSmst->SmmAllocatePool(EfiRuntimeServicesData, sizeof(PMEM_REGION)*MAX_PMEM_REGIONS, &mPmemRegions);
    ASSERT_EFI_ERROR(Status);
    ZeroMem(mPmemRegions, sizeof(PMEM_REGION)*MAX_PMEM_REGIONS);
  }

  //
  // Update HOB variable for PCI resource information
  // Get the HOB list.  If it is not present, then ASSERT.
  //
  Status = EfiGetSystemConfigurationTable(&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR(Status);

  //
  // Search for the Memory Map GUID HOB.  If it is not present, then
  // there's nothing we can do. It may not exist on the update path.
  //
  GuidHob    = GetFirstGuidHob(&gEfiMemoryMapGuid);
  ASSERT(GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  TempSystemMemoryMap = GET_GUID_HOB_DATA(GuidHob);
  CopyMem(mSystemMemoryMap, TempSystemMemoryMap, sizeof(struct SystemMemoryMapHob));

  if (InSmm) {
    Status = mSmst->SmmInstallProtocolInterface (
             &Handle,
             &gEfiJedecNvDimmSmmGuid,
             EFI_NATIVE_INTERFACE,
             &mJedecNvDimmProtocol
             );
  } else {
    Status = gBS->InstallProtocolInterface (
             &Handle,
             &gEfiJedecNvDimmGuid,
             EFI_NATIVE_INTERFACE,
             &mJedecNvDimmProtocol
             );
  }
  ASSERT_EFI_ERROR (Status);

  // Locate the buffer created in CrystalRidge since it's not used
  Status = gBS->LocateProtocol(&gEfiNgnAcpiSmmInterfaceProtocolGuid, NULL, &mNgnAcpiSmmInterfaceArea);
  mNgnAcpiSmmInterface = mNgnAcpiSmmInterfaceArea->Area;
  ASSERT(mNgnAcpiSmmInterface);
  DEBUG((EFI_D_ERROR, "JNP: Found NgnAcpiSmmInterface = 0x%1lx\n", mNgnAcpiSmmInterface));

  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &mIioUds);
  ASSERT_EFI_ERROR (Status);

  ZeroMem (&mHost, sizeof (struct sysHost));
  mHost.var.common.cpuType = mIioUds->IioUdsPtr->SystemStatus.cpuType;
  mHost.var.common.SystemRasType = mIioUds->IioUdsPtr->SystemStatus.SystemRasType;
  mHost.var.common.stepping = mIioUds->IioUdsPtr->SystemStatus.MinimumCpuStepping;
  mHost.var.common.socketPresentBitMap = mIioUds->IioUdsPtr->SystemStatus.socketPresentBitMap;
  mHost.var.common.mmCfgBase = (UINT32)mIioUds->IioUdsPtr->PlatformData.PciExpressBase;

  // If AEP DIMMs present then exit
  IsAepDimmPresent = FALSE;
  if (mSystemMemoryMap->AepDimmPresent) {
    IsAepDimmPresent = TRUE;
    mNgnAcpiSmmInterface->IsAepDimmPresent = 1;
    DEBUG((EFI_D_ERROR, "JNP: AEP Dimm present, exiting JEDEC NVDIMM driver.\n"));
    return EFI_SUCCESS;
  }

  if (!InSmm) ZeroMem(mNgnAcpiSmmInterface, sizeof(NGN_ACPI_SMM_INTERFACE));

  CrInfo.NumAepDimms = 0;
  CrInfo.NumPMEMRgns = 0;

  //
  // Get the SMBios Type17 Handles for all the Aep Dimms
  NumSMBiosHandles = GetAepDimmSMBiosType17Handles();

  // Find JEDEC NVDIMMs
  Status = FillPmemRegions();
  if(EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "JNP: No JEDEC NVDIMMs found, exiting JEDEC NVDIMM driver.\n"));
    return EFI_SUCCESS;
  }

  //
  // Get and update the NVDIMM and PMEM info structs
  UpdateDimmInfo();

  //
  // Create NVMDIMM Present Bitmaps based on ALL NVDIMMs installed in all sockets
  // for use by AcpiSmmInterface.
  CreateDimmPresentBitmaps();

  //
  // Get some details of these dimms using HOB.
  InitAllDimmsFromHOB();

  if (InSmm) {
    EFI_SMM_SW_REGISTER_CONTEXT       SwContext;
    EFI_SMM_SW_DISPATCH2_PROTOCOL     *SwDispatch  = 0;
    EFI_HANDLE                        SwHandle;
    Status = mSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
    ASSERT_EFI_ERROR (Status);
    SwContext.SwSmiInputValue = 0xFD;
    Status = SwDispatch->Register (SwDispatch, ServeNgnCommandsFromOS, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);

    //
    // Install handler for ARS
    //
    Handle = NULL;
    Status = mSmst->SmiHandlerRegister (
                        CheckAndHandleArsEvent,
                        NULL,
                        &Handle
                        );
  }


/**
/*  ARS testing code
/*

{
  UINT64 Spa = 0x180000000;
  UINT64 ArsLength = 0xC00000000;

  if (InSmm) {
    //
    // NVDIMM configuration: 1way = (S0:C1:D0) 2way = (S0:C4:D1, S0:C5:D1) 3way = (S0:C3:D0, S0:C4:D0, S0:C5:D0)
    //
    DEBUG((EFI_D_ERROR, "\n\nARS Test...test all persistent memory\n\n\n"));
    HandleARSStart(Spa, ArsLength);
    DEBUG((EFI_D_ERROR, "\n\nARS Test...test 1way only\n\n\n"));
    Spa = 0x180000000;
    ArsLength = 0x200000000;
    HandleARSStart(Spa, ArsLength);
    DEBUG((EFI_D_ERROR, "\n\nARS Test...test 3way only\n\n\n"));
    Spa = 0x380000000;
    ArsLength = 0x600000000;
    HandleARSStart(Spa, ArsLength);
    DEBUG((EFI_D_ERROR, "\n\nARS Test...test 2way only\n\n\n"));
    Spa = 0x980000000;
    ArsLength = 0x400000000;
    HandleARSStart(Spa, ArsLength);
    DEBUG((EFI_D_ERROR, "\n\nARS Test...test middle of 3way\n\n\n"));
    Spa = 0x580000000;
    ArsLength = 0x200000000;
    HandleARSStart(Spa, ArsLength);
    DEBUG((EFI_D_ERROR, "\n\nARS Test...test start in 1way and end in 3way\n\n\n"));
    Spa = 0x200000000;
    ArsLength = 0x400000000;
    HandleARSStart(Spa, ArsLength);
    DEBUG((EFI_D_ERROR, "\n\nARS Boundary Test...test before PMEM start\n\n\n"));
    Spa = 0x100000000;
    ArsLength = 0xC00000000;
    HandleARSStart(Spa, ArsLength);
    DEBUG((EFI_D_ERROR, "\n\nARS Boundary Test...test after PMEM end\n\n\n"));
    Spa = 0x200000000;
    ArsLength = 0xC00000000;
    HandleARSStart(Spa, ArsLength);
    DEBUG((EFI_D_ERROR, "\n\nARS Boundary Test...test larger than PMEM region\n\n\n"));
    Spa = 0x100000000;
    ArsLength = 0xD00000000;
    HandleARSStart(Spa, ArsLength);
  }
}

**/


  return EFI_SUCCESS;
}


