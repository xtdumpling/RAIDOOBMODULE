/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
@copyright
  Copyright (c) 2014-2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  MarginParam.c

@brief
  This file contains product specific functions for margin parameters.
**/
#ifdef SSA_FLAG

#include "ssabios.h"
#include "MarginParam.h"

/**
  This function is used to get a string associated with the given I/O level.

  @param[in]   IoLevel   I/O level.

  @retval   String describing the I/O level.
**/
CONST char* GetIoLevelStr(
  GSM_LT IoLevel)
{
  switch (IoLevel) {
    case DdrLevel:
      return "DdrLevel";
    case LrbufLevel:
      return "LrbufLevel";
    default:
      return "unknown";
  }
} // end function GetIoLevelStr

/**
  This function is used to get a string associated with the given margin group.

  @param[in]   MarginGroup   Margin group.

  @retval   String describing the margin group.
**/
CONST char* GetMarginGroupStr(
  GSM_GT MarginGroup)
{
  switch (MarginGroup) {
    case RecEnDelay:
      return "RecEnDelay";
    case RxDqDelay:
      return "RxDqDelay";
    case RxDqsDelay:
      return "RxDqsDelay";
    case RxDqsPDelay:
      return "RxDqsPDelay";
    case RxDqsNDelay:
      return "RxDqsNDelay";
    case RxVref:
      return "RxVref";
    case RxEq:
      return "RxEq";
    case RxDqBitDelay:
      return "RxDqBitDelay";
    case WrLvlDelay:
      return "WrLvlDelay";
    case TxDqsDelay:
      return "TxDqsDelay";
    case TxDqDelay:
      return "TxDqDelay";
    case TxVref:
      return "TxVref";
    case TxEq:
      return "TxEq";
    case TxDqBitDelay:
      return "TxDqBitDelay";
    case CmdAll:
      return "CmdAll";
    case CtlAll:
      return "CtlAll";
    case CkAll:
      return "CkAll";
    //case CmdCtlAll:
    //  return "CmdCtlAll";
    case CmdVref:
      return "CmdVref";
#if SUPPORT_FOR_DDRT
    case EridDelay:
      return "EridDelay";
    case EridVref:
      return "EridVref";
#endif
    default:
      return "unknown";
  }
} // end function GetMarginGroupStr

/**
  This function is used to determine if the given margin parameter requires
  incremental stepping.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       IoLevel            I/O level.
  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   Incremental stepping is required.
  @retval  FALSE  Incremental stepping is not required.
**/
BOOLEAN IsIncrementalSteppingRequired(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN GSM_LT IoLevel,
  IN GSM_GT MarginGroup)
{
  switch (MarginGroup) {
    case CmdAll:
    case CmdGrp0:
    case CmdGrp1:
    case CmdGrp2:
    case CtlAll:
    case CtlGrp0:
    case CtlGrp1:
    case CtlGrp2:
    case CtlGrp3:
    case CtlGrp4:
    case CtlGrp5:
    case CmdCtlAll:
    case CkAll:
    case CmdVref:
    case AlertVref:
    case CmdRon:
    case ReqVref:
      return TRUE;
      break;
    default:
      return FALSE;
  }
} // end function IsIncrementalSteppingRequired

/**
  This function is used to determine if the given margin parameter requires
  an I/O reset after an error is detected.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       IoLevel            I/O level.
  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   I/O reset is required.
  @retval  FALSE  I/O reset is not required.
**/
BOOLEAN IsIoResetAfterErrRequired(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN GSM_LT IoLevel,
  IN GSM_GT MarginGroup)
{
  BOOLEAN RetVal = FALSE;

  // JEDEC initialization should be preceded by I/O reset
  if (IsJedecInitAfterErrRequired(SsaServicesHandle, pSystemInfo, pMemCfg,
    pMemPointTestCfg, IoLevel, MarginGroup))
  {
    RetVal = TRUE;
  }
  else {
    switch (MarginGroup) {
      case RecEnDelay:
        RetVal = TRUE;
        break;
      default:
        break;
    }
  }

  return RetVal;
} // end function IsIoResetAfterErrRequired

/**
  This function is used to determine if the given margin parameter requires
  a JEDEC initialization after an error is detected.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       pMemPointTestCfg   Pointer to generic memory point test configuration structure.
  @param[in]       IoLevel            I/O level.
  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   Incremental stepping is required.
  @retval  FALSE  Incremental stepping is not required.
**/
BOOLEAN IsJedecInitAfterErrRequired(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg,
  IN CONST MEM_POINT_TEST_CONFIG *pMemPointTestCfg,
  IN GSM_LT IoLevel,
  IN GSM_GT MarginGroup)
{
  BOOLEAN RetVal = FALSE;
  UINT8 Socket, Controller, Channel, Dimm;
  MEMORY_TECHNOLOGY MemoryTech;

  switch (MarginGroup) {
    case CmdAll:
    case CmdGrp0:
    case CmdGrp1:
    case CmdGrp2:
    case CtlAll:
    case CtlGrp0:
    case CtlGrp1:
    case CtlGrp2:
    case CtlGrp3:
    case CtlGrp4:
    case CtlGrp5:
    case CmdCtlAll:
    case CkAll:
    case CmdVref:
    case AlertVref:
    case CmdRon:
    case ReqVref:
      RetVal = TRUE;
      break;
    case TxVref:
      // IF this is for the frontside
      if (IoLevel == DdrLevel) {
        // FOR each socket:
        for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
          // FOR each controller:
          for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
            // FOR each channel:
            for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
              // IF this channel has no lanes enabled for validation THEN skip it
              if (!IsChannelValEnabled(SsaServicesHandle, pSystemInfo, pMemCfg,
                pMemPointTestCfg, Socket, Controller, Channel)) {
                  continue;
              }

              for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
                // IF this DIMM is not enabled for testing THEN skip it
#if SUPPORT_FOR_TURNAROUNDS
                if (!IsDimmATestingEnabled(SsaServicesHandle, pSystemInfo,
                  pMemCfg, pMemPointTestCfg, Socket, Controller, Channel,
                  Dimm) &&
                  ((pMemPointTestCfg->RankTestModes[Socket][Controller][Channel] != Turnarounds) ||
                  !IsDimmBTestingEnabled(SsaServicesHandle, pSystemInfo,
                  pMemCfg, pMemPointTestCfg, Socket, Controller, Channel,
                  Dimm))) {
                  continue;
                }
#else
                if (!IsDimmATestingEnabled(SsaServicesHandle, pSystemInfo,
                  pMemCfg, pMemPointTestCfg, Socket, Controller, Channel,
                  Dimm)) {
                  continue;
                }
#endif // !SUPPORT_FOR_TURNAROUNDS

                MemoryTech = pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm];

                // IF the DIMM is not DDR3
                if (MemoryTech != SsaMemoryDdr3) {
                  RetVal = TRUE;
                  break;
                }
              } // end for each DIMM

              if (RetVal) {
                break;
              }
            } // end for each channel

            if (RetVal) {
              break;
            }
          } // end for each controller

          if (RetVal) {
            break;
          }
        } // end for each socket
      } // end if this is for the frontside
      break;
    default:
      break;
  }

  return RetVal;
} // end function IsJedecInitAfterErrRequired

/**
  This function is used to get strobe width for the given margin parameter and
  DIMM.

  Note that the logic used in this function is derived from the GetMaxStrobe
  function in the MemProjectSpecific.c file in the SKX BIOS code.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
  @param[in]       IoLevel            I/O level.
  @param[in]       MarginGroup        Margin group.
  @param[in]       Socket             Zero-based Socket.
  @param[in]       Controller         Zero-based Controller.
  @param[in]       Channel            Zero-based Channel.
  @param[in]       Dimm               Zero-based DIMM.

  @retval  Strobe width.
**/
UINT8 GetMarginParamStrobeWidth(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MEM_CFG *pMemCfg,
  IN GSM_LT IoLevel,
  IN GSM_GT MarginGroup,
  IN UINT8 Socket,
  IN UINT8 Controller,
  IN UINT8 Channel,
  IN UINT8 Dimm)
{
  UINT8 StrobeWidth = 8;

#if SUPPORT_FOR_LRDIMM || SUPPORT_FOR_DDRT
  // IF the device is an LRDIMM and the margin parameter is frontside TxVref or backside RxVref
  // pMemCfg->IsLrDimms is a platform level inidcation. 
  if (pMemCfg->IsLrDimms && (pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] != SsaMemoryDdrT)) {
    if (((IoLevel == DdrLevel) && (MarginGroup == TxVref)) ||
      ((IoLevel == LrbufLevel) && (MarginGroup == RxVref))) {
      StrobeWidth = 8;
    }
    else if ((IoLevel == LrbufLevel) && (MarginGroup == TxVref)) {
      StrobeWidth = 4;
    }
  }
  else
#endif // SUPPORT_FOR_LRDIMM || SUPPORT_FOR_DDRT
  {
    MRC_DIMM_INFO DimmInfo;
    UINT8 DeviceWidth;

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  GetDimmInfo(Socket=%u Controller=%u Channel=%u Dimm=%u)\n",
      Socket, Controller, Channel, Dimm);
#endif
    SsaServicesHandle->SsaMemoryConfig->GetDimmInfo(SsaServicesHandle,
      Socket, Controller, Channel, Dimm, &DimmInfo);

    DeviceWidth = (DimmInfo.DeviceWidth == 0) ? 4 : DimmInfo.DeviceWidth;

    if ((MarginGroup == TxEq) || (MarginGroup == TxImode) ||
      (MarginGroup == RxEq) || (MarginGroup == RxCtleC) ||
      (MarginGroup == RxCtleR) ||
      ((MarginGroup == RxVref) && (IoLevel == DdrLevel))) {
      StrobeWidth = 8;
    }
    else if ((DeviceWidth == 4) ||
      (MarginGroup == RxDqsPDelay) || (MarginGroup == RxDqsNDelay) ||
      (MarginGroup == RxDqDqsDelay) || (MarginGroup == RxDqDelay) ||
      (MarginGroup == RxDqsDelay) || (MarginGroup == RecEnDelay) ||
      (MarginGroup == TxDqDelay) || (MarginGroup == RxVref) ||
      (MarginGroup == RxEq) || (MarginGroup == TxEq) ||
      ((MarginGroup == TxVref) &&
      (pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] == SsaMemoryDdr3))) {
      StrobeWidth = 4;
    }
    else {
      StrobeWidth = 8;
    }
  }

  return StrobeWidth;
} // end function GetMarginParamStrobeWidth

/**
  This function is used to determine if the given margin group is specific to
  DDR-T.

  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   The margin group is specific to DDR-T.
  @retval  FALSE  The margin group is not specific to DDR-T.
**/
BOOLEAN IsMarginGroupDdrTSpecific(
  IN GSM_GT MarginGroup)
{
  BOOLEAN IsDdrtSpecific = FALSE;

  if ((MarginGroup == EridDelay) ||
    (MarginGroup == EridVref) ||
    (MarginGroup == ErrorVref) ||
    (MarginGroup == ReqVref)) {
    IsDdrtSpecific = TRUE;
  }

  return IsDdrtSpecific;
} // end function IsMarginGroupDdrTSpecific

/**
  This function is used to determine if the given margin group is for
  Early-Read-ID.

  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   The margin group is for Early-Read-ID.
  @retval  FALSE  The margin group is not for Early-Read-ID.
**/
BOOLEAN IsMarginGroupForErid(
  IN GSM_GT MarginGroup)
{
  BOOLEAN IsForErid = FALSE;

  if ((MarginGroup == EridDelay) ||
    (MarginGroup == EridVref)) {
    IsForErid = TRUE;
  }

  return IsForErid;
} // end function IsMarginGroupForErid

/**
  This function is used to determine if the given margin parameter requires
  the point test timeout feature to be enabled.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       IoLevel            I/O level.
  @param[in]       MarginGroup        Margin group.

  @retval  TRUE   Point test timeout is required.
  @retval  FALSE  Point test timeout is not required.
**/
BOOLEAN IsPointTestTimeoutRequired(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN GSM_LT IoLevel,
  IN GSM_GT MarginGroup)
{
  BOOLEAN RetVal = FALSE;

  if ((IoLevel == DdrLevel) && (MarginGroup == ReqVref)) {
    RetVal = TRUE;
  }

  return RetVal;
} // end function IsPointTestTimeoutRequired

#endif // SSA_FLAG

// end file MarginParam.c
