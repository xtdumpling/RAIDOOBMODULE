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
  Copyright (c) 2013-2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  MemCfg.c

@brief
  This file contains the implementation of the GetMemCfg function.
**/
#ifdef SSA_FLAG

#include "MemCfg.h"
#include "ChkRetStat.h"

// flag to enable BIOS-SSA debug messages
#define ENBL_BIOS_SSA_API_DEBUG_MSGS (0)

/**
  This function fills in the given memory configuration structure.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[out]      pMemCfg            Pointer to memory configuration structure.
  @param[out]      pTestStat          Pointer to test status in results metadata structure.

  @retval  0  success
  @retval  1  error occurred
**/
UINT8 GetMemCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  OUT MEM_CFG *pMemCfg,
  OUT TEST_STATUS *pTestStat)
{
  UINT8 Socket, Controller, Channel, Dimm;
  MRC_DIMM_INFO DimmInfo;

  bssa_memset(pMemCfg, 0, sizeof(*pMemCfg));

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // IF this socket is not populated THEN skip it
    if ((pSystemInfo->SocketBitMask & (1 << Socket)) == 0) {
      continue;
    }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "GetControllerBitMask(Socket=%u)\n", Socket);
#endif
    SsaServicesHandle->SsaMemoryConfig->GetControllerBitMask(SsaServicesHandle,
      Socket, &pMemCfg->ControllerBitmasks[Socket]);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  ControllerBitmask=0x%X\n", pMemCfg->ControllerBitmasks[Socket]);
#endif
    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // IF this controller is not populated THEN skip it
      if ((pMemCfg->ControllerBitmasks[Socket] & (1 << Controller)) == 0) {
        continue;
      }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "  GetChannelBitMask(Socket=%u Controller=%u)\n",
        Socket, Controller);
#endif
      SsaServicesHandle->SsaMemoryConfig->GetChannelBitMask(SsaServicesHandle,
        Socket, Controller, &pMemCfg->ChannelBitmasks[Socket][Controller]);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "    ChannelBitmask=0x%X\n",
        pMemCfg->ChannelBitmasks[Socket][Controller]);
#endif
      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not populated THEN skip it
        if ((pMemCfg->ChannelBitmasks[Socket][Controller] & (1 << Channel)) == 0) {
          continue;
        }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "    GetDimmBitMask(Socket=%u Controller=%u Channel=%u)\n",
          Socket, Controller, Channel);
#endif
        SsaServicesHandle->SsaMemoryConfig->GetDimmBitMask(
          SsaServicesHandle, Socket, Controller, Channel,
          &pMemCfg->DimmBitmasks[Socket][Controller][Channel]);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "      DimmBitmask=0x%X\n",
          pMemCfg->DimmBitmasks[Socket][Controller][Channel]);
#endif
        // FOR each DIMM:
        for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
          // IF this DIMM is not populated THEN skip it
          if ((pMemCfg->DimmBitmasks[Socket][Controller][Channel] &
            (1 << Dimm)) == 0) {
            continue;
          }

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "      GetRankInDimm(Socket=%u Controller=%u Channel=%u Dimm=%u)\n",
            Socket, Controller, Channel, Dimm);
#endif
          SsaServicesHandle->SsaMemoryConfig->GetRankInDimm(
            SsaServicesHandle, Socket, Controller, Channel, Dimm,
            &pMemCfg->RankCounts[Socket][Controller][Channel][Dimm]);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "        RankCount=%u\n",
            pMemCfg->RankCounts[Socket][Controller][Channel][Dimm]);
#endif

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "      GetDimmInfo(Socket=%u Controller=%u Channel=%u Dimm=%u)\n",
            Socket, Controller, Channel, Dimm);
#endif
          SsaServicesHandle->SsaMemoryConfig->GetDimmInfo(
            SsaServicesHandle, Socket, Controller, Channel, Dimm,
            &DimmInfo);

#if ENBL_BIOS_SSA_API_DEBUG_MSGS
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "        EccSupport=%u DimmCapacity=%u BankCount=%u RowSize=%u ColumnSize=%u\n",
            DimmInfo.EccSupport, DimmInfo.DimmCapacity, DimmInfo.BankCount,
            DimmInfo.RowSize, DimmInfo.ColumnSize);
          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "        DeviceWidth=%u MemoryTech=%u MemoryPackage=%u\n",
            DimmInfo.DeviceWidth, DimmInfo.MemoryTech, DimmInfo.MemoryPackage);
#endif
          pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] = DimmInfo.MemoryTech;

          if ((DimmInfo.MemoryPackage == LrDimmMemoryPackage) && \
            (DimmInfo.MemoryTech != SsaMemoryDdrT)) {
            pMemCfg->IsLrDimms = TRUE;
          }
        } // end for each DIMM
      } // end for each channel
    } // end for each controller
  } // end for each socket

  return 0;
} // end function GetMemCfg

/**
  This function logs the contents of the given memory configuration structure.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.
**/
VOID LogMemCfg(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN MEM_CFG *pMemCfg)
{
  UINT8 Socket, Controller, Channel, Dimm;

  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "\n"
    "Memory Configuration:\n");
  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // IF this socket is not populated THEN skip it
    if ((pSystemInfo->SocketBitMask & (1 << Socket)) == 0) {
      continue;
    }

    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
      "  Socket=%u\n", Socket);

    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // IF this controller is not populated THEN skip it
      if ((pMemCfg->ControllerBitmasks[Socket] & (1 << Controller)) == 0) {
        continue;
      }

      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
        "    Controller=%u\n", Controller);

      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not populated THEN skip it
        if ((pMemCfg->ChannelBitmasks[Socket][Controller] &
          (1 << Channel)) == 0) {
          continue;
        }

        NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
          "      Channel=%u\n", Channel);

        // FOR each DIMM:
        for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
          // IF this DIMM is not populated THEN skip it
          if ((pMemCfg->DimmBitmasks[Socket][Controller][Channel] &
            (1 << Dimm)) == 0) {
            continue;
          }

          NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
            "        Dimm=%u RankCount=%u MemoryTech=0x%X\n", Dimm,
            pMemCfg->RankCounts[Socket][Controller][Channel][Dimm],
            pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm]);
        } // end for each DIMM
      } // end for each channel
    } // end for each controller
  } // end for each socket

  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "  IsLrDimms=%u\n", pMemCfg->IsLrDimms);
  NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR,
    "\n");
} // end function LogMemCfg

#if SUPPORT_FOR_DDRT
/**
  This function determines whether the given memory configuration contains any
  DDR-T devices.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       pSystemInfo        Pointer to system information structure.
  @param[in]       pMemCfg            Pointer to memory configuration structure.

  @retval  TRUE   The memory configuration has at least one DDR-T device.
  @retval  FALSE  The memory configuration has no DDR-T devices.
**/
BOOLEAN GetHasDdrtDevices(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN CONST MRC_SYSTEM_INFO *pSystemInfo,
  IN CONST MEM_CFG *pMemCfg)
{
  UINT8 Socket, Controller, Channel, Dimm;
  BOOLEAN HasDdrtDevices = FALSE;

  // FOR each socket:
  for (Socket = 0; Socket < MAX_SOCKET_CNT; Socket++) {
    // IF this socket is not populated THEN skip it
    if ((pSystemInfo->SocketBitMask & (1 << Socket)) == 0) {
      continue;
    }

    // FOR each controller:
    for (Controller = 0; Controller < MAX_CONTROLLER_CNT; Controller++) {
      // IF this controller is not populated THEN skip it
      if ((pMemCfg->ControllerBitmasks[Socket] & (1 << Controller)) == 0) {
        continue;
      }

      // FOR each channel:
      for (Channel = 0; Channel < MAX_CHANNEL_CNT; Channel++) {
        // IF this channel is not populated THEN skip it
        if ((pMemCfg->ChannelBitmasks[Socket][Controller] &
          (1 << Channel)) == 0) {
          continue;
        }

        // FOR each DIMM:
        for (Dimm = 0; Dimm < MAX_DIMM_CNT; Dimm++) {
          // IF this DIMM is not populated THEN skip it
          if ((pMemCfg->DimmBitmasks[Socket][Controller][Channel] &
            (1 << Dimm)) == 0) {
            continue;
          }

          if (pMemCfg->MemoryTech[Socket][Controller][Channel][Dimm] ==
            SsaMemoryDdrT) {
            HasDdrtDevices = TRUE;
            break;
          }
        } // end for each DIMM

        if (HasDdrtDevices) {
          break;
        }
      } // end for each channel

      if (HasDdrtDevices) {
        break;
      }
    } // end for each controller

    if (HasDdrtDevices) {
      break;
    }
  } // end for each socket

  return HasDdrtDevices;
} // end function GetHasDdrtDevices
#endif // SUPPORT_FOR_DDRT

#endif  // SSA_FLAG

// end file MemCfg.c
