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

  XlateFunctions.c

Abstract:

  Implementation for functions handling DPA/SPA conversions.

--*/

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

#include "CrystalRidge.h"

#include "XlateFunctions/XlateFunctions.h"

extern CR_INFO mCrInfo;

extern CONST UINT32 Imc1Way4k_2ChWay4k_LineMultipliers[2][2];
extern CONST UINT32 Imc1Way4k_2ChWay256_LineMultipliers[2][8];
extern CONST UINT32 Imc2Way4k_2ChWay4k_LineMultipliers[4][2];
extern CONST UINT32 Imc2Way4k_2ChWay256_LineMultipliers[4][8];
extern CONST UINT32 Imc4Way4k_2ChWay4k_LineMultipliers[8][2];
extern CONST UINT32 Imc4Way4k_2ChWay256_LineMultipliers[8][8];
extern CONST UINT32 Imc8Way4k_2ChWay256_LineMultipliers[16][8];
extern CONST UINT32 Imc1Way4k_3ChWay4k_LineMultipliers[3][2];
extern CONST UINT32 Imc1Way4k_3ChWay256_LineMultipliers[3][16];
extern CONST UINT32 Imc2Way4k_3ChWay4k_LineMultipliers[6][2];
extern CONST UINT32 Imc2Way4k_3ChWay256_LineMultipliers[6][16];
extern CONST UINT32 Imc4Way4k_3ChWay4k_LineMultipliers[12][2];
extern CONST UINT32 Imc4Way4k_3ChWay256_LineMultipliers[12][16];
extern CONST UINT32 Imc8Way4k_3ChWay4k_LineMultipliers[24][2];
extern CONST UINT32 Imc8Way4k_3ChWay256_LineMultipliers[24][16];

/**

Routine Description: This function returns number of lines in interleaving

  @param[in] ChWays      - channel interleaving ways
  @param[in] ImcGran     - memory controller granularity
  @param[in] ChGran      - channel granularity

  @return UINT8      - number of lines in given interleaving configuration

**/
UINT8
GetNumLinesInRotation (
  UINT8  ChWays,
  UINT32 ImcGran,
  UINT32 ChGran
  )
{
  if (ImcGran == ChGran) {
    if (ChWays == CH_1WAY) {
      return 1;
    }
    else {
      return 2;
    }
  } else {
    switch (ChWays) {
    case CH_1WAY:
      return 16;
      break;
    case CH_2WAY:
      return 8;
      break;
    case CH_3WAY:
      return 16;
      break;
    default:
      return 0;
    }
  }
}

/**

Routine Description: This function returns line multiplier for given dimm and
  line numbers taking part in interleaving.

  @param[in] ImcSize     - memory controller granularity
  @param[in] LineSize    - channel granularity
  @param[in] ImcWays     - Imc interleaving ways
  @param[in] ChWays      - channel interleaving ways
  @param[in] DimmNum     - dimm number in interleaving set (starting from 0)
  @param[in] LineNum     - line number

  @return UINT32     - line multiplier for given arguments

**/
UINT32
GetLineMultiplier (
  UINT32 ImcSize,
  UINT32 LineSize,
  UINT8  ImcWays,
  UINT8  ChWays,
  UINT8  DimmNum,
  UINT8  LineNum)
{
  UINT32 LineMultiplier = 0;
  UINT32 NumLinesInRotation;

  NumLinesInRotation = GetNumLinesInRotation (ChWays, ImcSize, LineSize);

  DimmNum %= ImcWays * ChWays;
  LineNum %= NumLinesInRotation;

  switch (ChWays) {
  case CH_1WAY:
    LineMultiplier = LineNum + (DimmNum * NumLinesInRotation);
    break;
  case CH_2WAY:
    switch (ImcWays) {
    case ONE_IMC_WAY:
      if (ImcSize == LineSize) {
        LineMultiplier = Imc1Way4k_2ChWay4k_LineMultipliers[DimmNum][LineNum];
      } else {
        LineMultiplier = Imc1Way4k_2ChWay256_LineMultipliers[DimmNum][LineNum];
      }
      break;
    case TWO_IMC_WAY:
      if (ImcSize == LineSize) {
        LineMultiplier = Imc2Way4k_2ChWay4k_LineMultipliers[DimmNum][LineNum];
      } else {
        LineMultiplier = Imc2Way4k_2ChWay256_LineMultipliers[DimmNum][LineNum];
      }
      break;
    case FOUR_IMC_WAY:
      if (ImcSize == LineSize) {
        LineMultiplier = Imc4Way4k_2ChWay4k_LineMultipliers[DimmNum][LineNum];
      } else {
        LineMultiplier = Imc4Way4k_2ChWay256_LineMultipliers[DimmNum][LineNum];
      }
      break;
    case EIGHT_IMC_WAY:
      // 8 Imc Way not supported for 4k/4k
      LineMultiplier = Imc8Way4k_2ChWay256_LineMultipliers[DimmNum][LineNum];
      break;
    default:
      LineMultiplier = 0;
      break;
    }
    break;
  case CH_3WAY:
    switch (ImcWays) {
    case ONE_IMC_WAY:
      if (ImcSize == LineSize) {
        LineMultiplier = Imc1Way4k_3ChWay4k_LineMultipliers[DimmNum][LineNum];
      } else {
        LineMultiplier = Imc1Way4k_3ChWay256_LineMultipliers[DimmNum][LineNum];
      }
      break;
    case TWO_IMC_WAY:
      if (ImcSize == LineSize) {
        LineMultiplier = Imc2Way4k_3ChWay4k_LineMultipliers[DimmNum][LineNum];
      } else {
        LineMultiplier = Imc2Way4k_3ChWay256_LineMultipliers[DimmNum][LineNum];
      }
      break;
    case FOUR_IMC_WAY:
      if (ImcSize == LineSize) {
        LineMultiplier = Imc4Way4k_3ChWay4k_LineMultipliers[DimmNum][LineNum];
      } else {
        LineMultiplier = Imc4Way4k_3ChWay256_LineMultipliers[DimmNum][LineNum];
      }
      break;
    case EIGHT_IMC_WAY:
      if (ImcSize == LineSize) {
        LineMultiplier = Imc8Way4k_3ChWay4k_LineMultipliers[DimmNum][LineNum];
      } else {
        LineMultiplier = Imc8Way4k_3ChWay256_LineMultipliers[DimmNum][LineNum];
      }
      break;
    default:
      LineMultiplier = 0;
      break;
    }
    break;
  default:
    LineMultiplier = 0;
    break;
  }

  return LineMultiplier;
}

/**

Routine Description: This function converts given SPA to DPA

  @param[in] SpaOffset   - Spa for which DPA is computed
  @param[in] ImcWays     - Imc interleaving ways
  @param[in] ChWays      - channel interleaving ways
  @param[in] LineSize    - channel granularity
  @param[in] ImcSize     - memory controller granularity

  @return DPA_ADDRESS - full DPA address and offset from given SPA offset

**/
DPA_ADDRESS
GetDpaOffsetFromSpaOffset (
   UINT64 SpaOffset,
   UINT8  ImcWays,
   UINT8  ChWays,
   UINT32 LineSize,
   UINT32 ImcSize
   )
{
  UINT64           SpaIndex;
  UINT64           RotationNum;
  UINT8            NumLinesInRotation;
  UINT8            IntWays;
  UINT32           RotationSize;
  UINT8            LineNum = 0;
  UINT8            IntDimmNum = 0;
  BOOLEAN          IndexFound = FALSE;
  DPA_ADDRESS      Dpa;

  NumLinesInRotation = GetNumLinesInRotation (ChWays, ImcSize, LineSize);

  IntWays      = ImcWays * ChWays;
  RotationSize = LineSize * NumLinesInRotation;
  RotationNum  = SpaOffset / (RotationSize * IntWays);
  SpaIndex     = SpaOffset % (RotationSize * IntWays) / LineSize;

  for (IntDimmNum = 0; IntDimmNum < IntWays; IntDimmNum++ ) {
    for (LineNum = 0; LineNum < NumLinesInRotation; LineNum++) {
      if (SpaIndex == GetLineMultiplier (ImcSize, LineSize, ImcWays, ChWays, IntDimmNum, LineNum)) {
        IndexFound = TRUE;
        break;
      }
    }
    if (IndexFound) {
      break;
    }
  }

  // Now we have everything to compute the Dpa
  Dpa.Offset     = (RotationNum * RotationSize) + (LineNum * LineSize) + (SpaOffset % LineSize);
  Dpa.IntDimmNum = IntDimmNum;
  Dpa.Ch         = SpaIndex % ChWays;
  Dpa.Imc        = IntDimmNum / ChWays;
  Dpa.Skt        = Dpa.Imc / MAX_IMC;

  return Dpa;
}

/**

Routine Description: This function converts given DPA to SPA

  @param[in] Dpa         - DPA offset for which SPA is computed
  @param[in] DimmNum     - DIMM number in interleave set
  @param[in] ImcWays     - Imc interleaving ways
  @param[in] ChWays      - channel interleaving ways
  @param[in] LineSize    - channel granularity
  @param[in] ImcSize     - memory controller granularity

  @return UINT64     - computed SPA address offset

**/
UINT64
GetSpaOffsetFromDpaOffset (
  UINT64 Dpa,
  UINT8  DimmNum,
  UINT8  ImcWays,
  UINT8  ChWays,
  UINT32 LineSize,
  UINT32 ImcSize
  )
{

  UINT8  LineNum;
  UINT8  NumLinesInRotation;
  UINT64 RotationNum;
  UINT64 RotationSize;
  INT32  LineMultiplier;
  UINT64 Spa;

  NumLinesInRotation = GetNumLinesInRotation (ChWays, ImcSize, LineSize);

  RotationSize  = LineSize * NumLinesInRotation;
  RotationNum   = Dpa / RotationSize;
  LineNum       = (UINT8)((Dpa % RotationSize) / LineSize);

  LineMultiplier = GetLineMultiplier (ImcSize, LineSize, ImcWays, ChWays, DimmNum, LineNum);

  Spa = (RotationNum * RotationSize * ImcWays * ChWays) + (LineMultiplier * LineSize) + (Dpa % LineSize);

  return Spa;
}

/**

Routine Description: XlateSpaToDpa - This function converts
given Spa to Dpa in the case of the Dimm involved in all Channel
ways and up to 8 Imc Ways. Converts only PMEM and CTRL regions.

  @param[in] *NvmDimmPtr - Pointer to Dimm structure
  @param[in] SpaBaseAddr - SAD Base Address
  @param[in] Spa         - Spa for which we need the DPA
  @param[out] *Dpa       - Pointer to the Dpa that this function
                           returns.

  @return EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
XlateSpaToDpaOffset (
  IN  AEP_DIMM  *NvmDimmPtr,
  IN  UINT64    SpaBaseAddr,
  IN  UINT64    Spa,
  OUT UINT64    *Dpa
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       ImcWays;
  UINT8       ChWays;
  UINT32      LineSize;
  UINT32      ImcSize;
  INT32       Index;

  if (Spa < SpaBaseAddr || NvmDimmPtr == NULL || Dpa == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsPmemRgn (SpaBaseAddr)) {
    Index = GetPmemIndexBasedOnSocCh (SpaBaseAddr, (UINT8)NvmDimmPtr->SocketId, NvmDimmPtr->Ch);
    if (Index == -1) {
      return EFI_INVALID_PARAMETER;
    }

    ImcWays  = mCrInfo.PMEMInfo[Index].iMCWays;
    ChWays   = mCrInfo.PMEMInfo[Index].ChWays;
    LineSize = GetChGranularityFromSadBaseAddr (SpaBaseAddr);
  } else {
    // Control Region only !
    SpaBaseAddr = NvmDimmPtr->SADSpaBase;
    ImcWays     = NvmDimmPtr->iMCWays;
    ChWays      = NvmDimmPtr->ChWays;
    LineSize    = mCrInfo.ChGran;
  }

  ASSERT (LineSize != 0);

  ImcSize = mCrInfo.SktGran;

  //
  // function expects SPA offset
  Spa = Spa - SpaBaseAddr;

  *Dpa = GetDpaOffsetFromSpaOffset (Spa, ImcWays, ChWays, LineSize, ImcSize).Offset;

  return Status;
}

/**

Routine Description: XlateDpaToSpa - This function converts given Dpa
to Spa in the case of the Dimm involved in all Channel ways and
up to 8 Imc Ways. Converts only PMEM and CTRL regions.

  @param[in] *NvmDimmPtr - Pointer to Dimm structure
  @param[in] SpaBaseAddr - Start Address of the Spa for Dpa Translation
  @param[in] Dpa         - Dpa offset for which we need the SPA
  @param[out] *Spa       - Pointer to the Spa.

  @return EFI_STATUS - status of the conversion effort

**/
EFI_STATUS
XlateDpaOffsetToSpa (
  IN  AEP_DIMM  *NvmDimmPtr,
  IN  UINT64    SpaBaseAddr,
  IN  UINT64    Dpa,
  OUT UINT64    *Spa
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       ChWays;
  UINT8       ImcWays;
  UINT8       DimmNum;
  UINT32      LineSize;
  UINT32      ImcSize;
  INT32       Index;

  if (NvmDimmPtr == NULL || Spa == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsPmemRgn (SpaBaseAddr)) {
    Index = GetPmemIndexBasedOnSocCh (SpaBaseAddr, (UINT8)NvmDimmPtr->SocketId, NvmDimmPtr->Ch);
    if (Index == -1) {
      return EFI_INVALID_PARAMETER;
    }

    DimmNum  = mCrInfo.PMEMInfo[Index].IntDimmNum;
    ImcWays  = mCrInfo.PMEMInfo[Index].iMCWays;
    ChWays   = mCrInfo.PMEMInfo[Index].ChWays;
    LineSize = GetChGranularityFromSadBaseAddr (SpaBaseAddr);
  } else {
    // Control Region only !
    SpaBaseAddr = NvmDimmPtr->SADSpaBase;
    DimmNum     = NvmDimmPtr->IntDimmNum;
    ImcWays     = NvmDimmPtr->iMCWays;
    ChWays      = NvmDimmPtr->ChWays;
    LineSize    = mCrInfo.ChGran;
  }

  ASSERT (LineSize != 0);

  ImcSize = mCrInfo.SktGran;

  *Spa = GetSpaOffsetFromDpaOffset (Dpa, DimmNum, ImcWays, ChWays, LineSize, ImcSize);
  *Spa += SpaBaseAddr;

  return Status;
}
