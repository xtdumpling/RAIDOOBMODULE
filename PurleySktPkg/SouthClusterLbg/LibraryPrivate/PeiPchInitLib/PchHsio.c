/** @file
  PCH HSIO Initialization file

@copyright
 Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/

#include <Library/HeciMsgLib.h>
#include <Library/PlatformHooksLib.h>
#include <Library/PchPcrLib.h>
#if ME_SUPPORT_FLAG
#include <Library/SpsPeiLib.h>
#if SPS_SUPPORT
#include <Library/MeTypeLib.h>
#endif // SPS_SUPPORT
#endif //ME_SUPPORT_FLAG
#include <BupMsgs.h>
#include "PchInitPei.h"
#include "PchHsioLaneLib.h"
#include <Library/PeiServicesTablePointerLib.h>
#include <IncludePrivate/PchHsio.h>
#include <IncludePrivate/PchChipsetInfoHob.h>
#include <Library/PchHsioLib.h>
#include <Library/SpsPeiLib.h>

/**
  The function is used to detemine if a ChipsetInitSync with ME is required and syncs with ME if required.

  @param[in] MeSync               Sync ME Settings

  @retval EFI_SUCCESS             BIOS and ME ChipsetInit settings are in sync
  @retval EFI_UNSUPPORTED         BIOS and ME ChipsetInit settings are not in sync
**/
EFI_STATUS
PchHsioChipsetInitProg (
  IN  BOOLEAN                      MeSync
  )
{
  EFI_STATUS                  Status;
#ifdef ME_SUPPORT_FLAG //(SERVER_BIOS_FLAG)
  UINT16                      BiosChipInitCrc;
  ME_POLICY_PPI               *PeiMePolicyPpi;
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#if SPS_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
  CLOCKING_MODES              ClockingMode;
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
  CHIPSET_INIT_INFO_HOB       *ChipsetInitHob;
#endif // ME_SUPPORT_FLAG (SERVER_BIOS_FLAG)
  EFI_BOOT_MODE               BootMode;
  UINT32                      PchChipsetInitTableLength;
  UINT8                       *PchChipsetInitTable;

  ///
  /// Step 1
  /// GetBoodMode, do not perform ChipsetInit Sync check on S3 RESUME
  ///
  Status = PeiServicesGetBootMode (&BootMode);
  if(BootMode == BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }

#ifdef ME_SUPPORT_FLAG
  if (MeSync == TRUE) {
    ///
    /// Get Policy settings through the MePolicy PPI
    ///
    Status = PeiServicesLocatePpi (
                              &gMePolicyPpiGuid,
                              0,
                              NULL,
                              (VOID **) &PeiMePolicyPpi
                              );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[ME] ERROR: MePolicyPpi not located! Error: %r\n", Status));
      Status = EFI_SUCCESS;
      PeiMePolicyPpi = NULL;
    } else if (PeiMePolicyPpi->MeConfig.HsioMessaging == 0) {
      DEBUG ((DEBUG_INFO, "PeiMePolicyPpi->HsioMessaging = 0, do not perform HSIO sync. \n"));
      return EFI_SUCCESS;
    }
  }

#endif // ME_SUPPORT_FLAG

  ///
  /// Step 2
  /// Assign appropriate ChipsetInit table
  ///
  switch (PchStepping ()) {
    //
    // LBG Chipset Init tables
    //
    case LbgA0:
      PchChipsetInitTable = PchLbgChipsetInitTable_Ax;
      PchChipsetInitTableLength   = sizeof (PchLbgChipsetInitTable_Ax);
      break;
    case LbgB0:
    case LbgB1:
    case LbgB2:
#if ME_SUPPORT_FLAG
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#if SPS_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
      ClockingMode = InternalStandard;
      PeiGetCurrenClockingMode(&ClockingMode);
      if (ClockingMode == External) {
        PchChipsetInitTable = PchLbgChipsetInitTable_Bx_Ext;
        PchChipsetInitTableLength   = sizeof (PchLbgChipsetInitTable_Bx_Ext);
      }
      else
#endif
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#endif // ME_SUPPORT_FLAG
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
      {
        PchChipsetInitTable = PchLbgChipsetInitTable_Bx;
        PchChipsetInitTableLength   = sizeof (PchLbgChipsetInitTable_Bx);
      }
      break;
#ifdef SKXD_EN
    case LbgB1_D:
#if ME_SUPPORT_FLAG
      ClockingMode = InternalStandard;
      PeiGetCurrenClockingMode(&ClockingMode);
      if (ClockingMode == External) {
        //
        // SKX-D
        //
        DEBUG ((DEBUG_INFO, "Apply PchChipsetInitTable for LBG-D.\n"));
        PchChipsetInitTable = PchLbgChipsetInitTable_BxD_Ext;
        PchChipsetInitTableLength   = sizeof (PchLbgChipsetInitTable_BxD_Ext);
      }
      else
#endif // ME_SUPPORT_FLAG
      {
        //
        // SKX-D
        //
        DEBUG ((DEBUG_INFO, "Apply PchChipsetInitTable for LBG-D.\n"));
        PchChipsetInitTable = PchLbgChipsetInitTable_BxD;
        PchChipsetInitTableLength   = sizeof (PchLbgChipsetInitTable_BxD);
      }
      break;
#endif // SKXD_EN
    case LbgS0:
    case LbgS1:
#if ME_SUPPORT_FLAG
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#if SPS_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
      ClockingMode = InternalStandard;
      PeiGetCurrenClockingMode(&ClockingMode);
      if (ClockingMode == External) {
        PchChipsetInitTable = PchLbgChipsetInitTable_Sx_Ext;
        PchChipsetInitTableLength   = sizeof (PchLbgChipsetInitTable_Sx_Ext);
      }
      else
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
#endif // ME_SUPPORT_FLAG
      {
        PchChipsetInitTable = PchLbgChipsetInitTable_Sx;
        PchChipsetInitTableLength   = sizeof (PchLbgChipsetInitTable_Sx);
      }
      break;      
    default:
      PchChipsetInitTable = NULL;
      PchChipsetInitTableLength   = 0;
      DEBUG ((DEBUG_ERROR, "Cannot find HSIO configuration table for PCH stepping %d\n", PchStepping()));
      ASSERT (FALSE);
  }

  ///
  /// Step 3
  /// Send the HECI HSIO Message
  ///

#ifdef ME_SUPPORT_FLAG //(SERVER_BIOS_FLAG)
  Status         = EFI_SUCCESS;
  ChipsetInitHob = GetFirstGuidHob (&gPchChipsetInitHobGuid);
  if (ChipsetInitHob == NULL) {

    UINT16  MeChipInitCrc = 0;

    DEBUG ((DEBUG_INFO, "(Hsio) ChipsetInitHob not found\n"));
    Status = PeiHeciHsioMsg (HSIO_REPORT_VERSION_REQ, &MeChipInitCrc);
    if (!EFI_ERROR(Status)) {
      DEBUG ((DEBUG_INFO, "(Hsio) Creating HOB to adjust Hsio settings in PchInit, if required\n"));
      Status = PeiServicesCreateHob (
                  EFI_HOB_TYPE_GUID_EXTENSION,
                  sizeof (CHIPSET_INIT_INFO_HOB),
                  (VOID**) &ChipsetInitHob
                  );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "(Hsio) ChipsetInitHob could not be created\n"));
        ASSERT_EFI_ERROR (Status);
        return EFI_ABORTED;
      }
      //
      // Initialize ChipsetInitHob
      //
      ChipsetInitHob->MeChipInitCrc = MeChipInitCrc;
      ChipsetInitHob->Header.Name   = gPchChipsetInitHobGuid;
    }
  }

  if (Status == EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "(Hsio) ME Reported CRC=0x%04X\n", ChipsetInitHob->MeChipInitCrc));
    //
    // Get ChipsetInit table indentifier from the one found in the code
    //
    if(PchChipsetInitTable != NULL) {
      BiosChipInitCrc = *((UINT16*)PchChipsetInitTable);
      DEBUG ((DEBUG_INFO, "(Hsio) BIOS Hsio CRC=0x%04X\n", BiosChipInitCrc));

      if (IsSimicsPlatform() == TRUE) {
        DEBUG ((DEBUG_INFO, "(Hsio) Skip HSIO config update when running on Simics\n"));
        BiosChipInitCrc = ChipsetInitHob->MeChipInitCrc;
      }
      if (ChipsetInitHob->MeChipInitCrc != BiosChipInitCrc) {
        if(MeSync == TRUE) {
          DEBUG ((DEBUG_INFO, "(Hsio) Sending HSIO as MKHI message\n"));
          Status = PeiHeciChipsetInitSyncMsg (PchChipsetInitTable, PchChipsetInitTableLength);
          if (EFI_ERROR (Status)) {
            DEBUG ((DEBUG_ERROR, "ChipsetInit Sync Error: %r\n", Status));
            if (Status == EFI_UNSUPPORTED || Status == EFI_DEVICE_ERROR) {
              DEBUG ((DEBUG_ERROR, "Current Me Bios boot path doesn't support Chipset Init Sync message, continue to boot.\n", Status));
            } else {
              ASSERT_EFI_ERROR (Status);
              return EFI_UNSUPPORTED;
            }
          }
        } else {

          return EFI_UNSUPPORTED;
        }
      }
    }
  } else {
    DEBUG ((DEBUG_INFO, "(Hsio) Syncing ChipsetInit with ME failed! Error: %r\n", Status));
  }
#endif //ME_SUPPORT_FLAG //(SERVER_BIOS_FLAG)
  return EFI_SUCCESS;
}


/**
  The function indicates if at least one of the data lanes attached
  to a specific common lane is of a specific phymode.

  @param[in] Pid               Common Lane End Point ID
  @param[in] PhyMode           Phymode that needs to be checked
  @param[in] PchSeries         Indicates the Pch Series
  @param[in] Los1              Lane Owner Status 1 Value
  @param[in] Los2              Lane Owner Status 2 Value
  @param[in] Los3              Lane Owner Status 3 Value
  @param[in] Los4              Lane Owner Status 4 Value

  @retval EFI_SUCCESS          Phymode exists.
          EFI_UNSUPPORTED      Phymode does not exist.
**/
EFI_STATUS
PchLosPhyModeCheck (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT8                             PhyMode,
  IN PCH_SERIES                         PchSeries,
  IN UINT32                             Los1,
  IN UINT32                             Los2,
  IN UINT32                             Los3,
  IN UINT32                             Los4
  )
{
  UINT8                       Shift;

  switch (Pid) {
  case PID_MODPHY1:
    for(Shift = PCH_MODPHY1_LOS1_LANE_START; Shift <= PCH_MODPHY1_LOS1_LANE_END; Shift++) {
      if(((Los1 >> (Shift * 4)) & 0xF) == PhyMode) {
        return EFI_SUCCESS;
      }
    }
    for(Shift = PCH_MODPHY1_LOS2_LANE_START; Shift <= PCH_MODPHY1_LOS2_LANE_END; Shift++) {
      if(((Los2 >> (Shift * 4)) & 0xF) == PhyMode) {
        return EFI_SUCCESS;
      }
    }
    break;
  case PID_MODPHY2:
    for(Shift = PCH_MODPHY2_LOS2_LANE_START; Shift <= PCH_MODPHY2_LOS2_LANE_END; Shift++) {
      if(((Los2 >> (Shift * 4)) & 0xF) == PhyMode) {
        return EFI_SUCCESS;
      }
    }
    for(Shift = PCH_MODPHY2_LOS3_LANE_START; Shift <= PCH_MODPHY2_LOS3_LANE_END; Shift++) {
      if(((Los3 >> (Shift * 4)) & 0xF) == PhyMode) {
        return EFI_SUCCESS;
      }
    }
    for(Shift = PCH_MODPHY2_LOS4_LANE_START; Shift <= PCH_MODPHY2_LOS4_LANE_END; Shift++) {
      if(((Los4 >> (Shift * 4)) & 0xF) == PhyMode) {
        return EFI_SUCCESS;
      }
    }
    break;
    default:
      return EFI_SUCCESS;
      break;
  }
  return EFI_UNSUPPORTED;
}

/**
  The function programs HSIO Rx\Tx Eq policy registers.

  @param[in]  PchPolicyPpi        The PCH Policy PPI instance
**/
VOID
PchHsioRxTxEqPolicyProg (
  IN  PCH_POLICY_PPI      *PchPolicyPpi
  )
{
  HSIO_LANE                   HsioLane;
  UINT16                      Offset;
  UINT8                       Index;
  UINT8                       PortId;
  UINT8                       LaneOwner;
  UINT8                       MaxSataPorts;
  UINT8                       MaxPciePorts;
  UINT8                       LaneNum;
  UINTN                       RpBase;
  UINT32                      Data32;
  UINT32                      Data32And;
  UINT32                      Data32Or;
  UINT32                      RpToRetrain;
  UINT32                      TimeSpent;
  UINT8                       SataCtrlIndex;
  PCH_HSIO_SATA_CONFIG        *HsioSataConfig;

  for (SataCtrlIndex= 0; SataCtrlIndex < 2; SataCtrlIndex++) {
    if (SataCtrlIndex == 0) {
      MaxSataPorts = GetPchMaxSataPortNum ();
      HsioSataConfig = &PchPolicyPpi->HsioSataConfig;
    } else {
      MaxSataPorts = GetPchMaxsSataPortNum ();
      HsioSataConfig = &PchPolicyPpi->HsiosSataConfig;
    }
    for (Index = 0; Index < MaxSataPorts; Index++) {
      if (SataCtrlIndex == 0) {
        PchGetSataLaneNum (Index, &LaneNum);
      } else {
        PchGetsSataLaneNum (Index, &LaneNum);
      }
      PchGetLaneInfo (LaneNum, &PortId, &LaneOwner);

      if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_SATA) {
        HsioGetLane (LaneNum, &HsioLane);
        if ((HsioSataConfig->PortLane[Index].HsioRxGen1EqBoostMagEnable == TRUE) ||
            (HsioSataConfig->PortLane[Index].HsioRxGen2EqBoostMagEnable == TRUE)){
          Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD21;
          Data32Or  = 0;
          Data32And = 0xFFFFFFFF;
          if (HsioSataConfig->PortLane[Index].HsioRxGen1EqBoostMagEnable == TRUE) {
            Data32Or |= (UINT32) (HsioSataConfig->PortLane[Index].HsioRxGen1EqBoostMag << N_PCH_HSIO_RX_DWORD21_ICFGCTLEDATATAP_QUATRATE_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_RX_DWORD21_ICFGCTLEDATATAP_QUATRATE_5_0;
          }
  
          if (HsioSataConfig->PortLane[Index].HsioRxGen2EqBoostMagEnable == TRUE) {
            Data32Or |= (UINT32) (HsioSataConfig->PortLane[Index].HsioRxGen2EqBoostMag << N_PCH_HSIO_RX_DWORD21_ICFGCTLEDATATAP_HALFRATE_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_RX_DWORD21_ICFGCTLEDATATAP_HALFRATE_5_0;
          }
  
          PchPcrAndThenOr32 (
            (PCH_SBI_PID) PortId,
            (UINT16) Offset,
            Data32And,
            Data32Or);
          PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
          DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
        }
  
        if (HsioSataConfig->PortLane[Index].HsioRxGen3EqBoostMagEnable == TRUE){
          Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD26;
          PchPcrAndThenOr32 (
            (PCH_SBI_PID) PortId,
            (UINT16) Offset,
            (UINT32) ~0,
            (UINT32) B_PCH_HSIO_RX_DWORD26_SATA_EQ_DIS);
          PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
          DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
  
          Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD20;
          PchPcrAndThenOr32 (
            (PCH_SBI_PID) PortId,
            (UINT16) Offset,
            (UINT32) ~B_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0,
            (UINT32) (HsioSataConfig->PortLane[Index].HsioRxGen3EqBoostMag << N_PCH_HSIO_RX_DWORD20_ICFGCTLEDATATAP_FULLRATE_5_0));
          PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
          DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
          }
  
        if ((HsioSataConfig->PortLane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) ||
            (HsioSataConfig->PortLane[Index].HsioTxGen2DownscaleAmpEnable == TRUE) ||
            (HsioSataConfig->PortLane[Index].HsioTxGen3DownscaleAmpEnable == TRUE)) {
  
          Data32Or  = 0;
          Data32And = 0xFFFFFFFF;
          if (HsioSataConfig->PortLane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) {
            Data32Or |= (UINT32) (HsioSataConfig->PortLane[Index].HsioTxGen1DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
          }
  
          if (HsioSataConfig->PortLane[Index].HsioTxGen2DownscaleAmpEnable == TRUE) {
            Data32Or |= (UINT32) (HsioSataConfig->PortLane[Index].HsioTxGen2DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
          }
  
          if (HsioSataConfig->PortLane[Index].HsioTxGen3DownscaleAmpEnable == TRUE) {
            Data32Or |= (UINT32) (HsioSataConfig->PortLane[Index].HsioTxGen3DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE10MARGIN_5_0);
            Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE10MARGIN_5_0;
          }
  
          Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD8;
          PchPcrAndThenOr32 (
            (PCH_SBI_PID) PortId,
            (UINT16) Offset,
            Data32And,
            Data32Or);
          PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
          DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
        }
  
        if ((HsioSataConfig->PortLane[Index].HsioTxGen1DeEmphEnable == TRUE) ||
            (HsioSataConfig->PortLane[Index].HsioTxGen2DeEmphEnable == TRUE) ||
            (HsioSataConfig->PortLane[Index].HsioTxGen3DeEmphEnable == TRUE)) {
          Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD6;
          Data32Or  = 0;
          Data32And = 0xFFFFFFFF;
          if (HsioSataConfig->PortLane[Index].HsioTxGen1DeEmphEnable == TRUE) {
            Data32Or |= (UINT32) (HsioSataConfig->PortLane[Index].HsioTxGen1DeEmph);
            Data32And &= (UINT32) ~B_PCH_HSIO_TX_DWORD6_OW2TAPGEN1DEEMPH6P0_5_0;
          }
  
          if (HsioSataConfig->PortLane[Index].HsioTxGen2DeEmphEnable == TRUE) {
            Data32Or |= (UINT32) (HsioSataConfig->PortLane[Index].HsioTxGen2DeEmph << N_PCH_HSIO_TX_DWORD6_OW2TAPGEN2DEEMPH6P0_5_0);
            Data32And &= (UINT32) ~B_PCH_HSIO_TX_DWORD6_OW2TAPGEN2DEEMPH6P0_5_0;
          }
  
          if (HsioSataConfig->PortLane[Index].HsioTxGen3DeEmphEnable == TRUE) {
            Data32Or |= (UINT32) (HsioSataConfig->PortLane[Index].HsioTxGen3DeEmph << N_PCH_HSIO_TX_DWORD6_OW2TAPGEN3DEEMPH6P0_5_0);
            Data32And &= (UINT32) ~B_PCH_HSIO_TX_DWORD6_OW2TAPGEN3DEEMPH6P0_5_0;
          }
  
          PchPcrAndThenOr32 (
            (PCH_SBI_PID) PortId,
            (UINT16) Offset,
            Data32And,
            Data32Or);
          PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
          DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
        }
      }
    }
  }

  MaxPciePorts = GetPchMaxPciePortNum ();

  RpToRetrain = 0;

  for (Index = 0; Index < MaxPciePorts; Index++) {
    RpBase = PchPcieBase (Index);
    PchGetPcieLaneNum (Index, &LaneNum);
    PchGetLaneInfo (LaneNum, &PortId, &LaneOwner);
    if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
      HsioGetLane (LaneNum, &HsioLane);
      if (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioRxSetCtleEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD25;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0,
          (UINT32) (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioRxSetCtle << N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }
      if (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioIcfgAdjLimitLoEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD39;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0,
          (UINT32) (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioIcfgAdjLimitLo << N_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }
      if (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioSampOffstEvenErrSpEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD40;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP,
          (UINT32) (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioSampOffstEvenErrSp << N_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }
      if (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioRemainingSamplerOffEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD41;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS,
          (UINT32) (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioRemainingSamplerOff << N_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }
      if (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioVgaGainCalEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD7;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL,
          (UINT32) (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioVgaGainCal << N_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }

      if ((PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) || (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DeEmphEnable == TRUE)) {
        //
        // Remember which ports were populated before temporarily disabling them
        //
        if ((MmioRead32 (RpBase) != 0xFFFFFFFF) && (MmioRead16 (RpBase + R_PCH_PCIE_LSTS) & B_PCIE_LSTS_LA) ) {
          RpToRetrain |= (BIT0 << Index);
          DEBUG ((DEBUG_INFO, "Warning! During PTSS programming Root Port %d needs to be retrained!", Index));
        }
        MmioOr8 (RpBase + R_PCH_PCIE_LCTL, B_PCIE_LCTL_LD);
      }

      if ((PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) ||
          (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen2DownscaleAmpEnable == TRUE) ||
          (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen3DownscaleAmpEnable == TRUE)) {

        Data32Or  = 0;
        Data32And = 0xFFFFFFFF;
        if (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) {
          Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0);
          Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
        }

        if (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen2DownscaleAmpEnable == TRUE) {
          Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen2DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0);
          Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
        }

        if (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen3DownscaleAmpEnable == TRUE) {
          Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen3DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE10MARGIN_5_0);
          Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE10MARGIN_5_0;
        }

        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD8;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          Data32And,
          Data32Or);
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }

      if ((PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DeEmphEnable == TRUE) ||
          (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen2DeEmph3p5Enable == TRUE)) {

        Data32Or  = 0;
        Data32And = 0xFFFFFFFF;
        if (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DeEmphEnable == TRUE) {
          Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DeEmph << N_PCH_HSIO_TX_DWORD5_OW2TAPGEN1DEEMPH3P5_5_0);
          Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD5_OW2TAPGEN1DEEMPH3P5_5_0;
        }

        if (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen2DeEmph3p5Enable == TRUE) {
          Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen2DeEmph3p5 << N_PCH_HSIO_TX_DWORD5_OW2TAPGEN2DEEMPH3P5_5_0);
          Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD5_OW2TAPGEN2DEEMPH3P5_5_0;
        }

        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD5;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          Data32And,
          Data32Or);
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }

      if (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen2DeEmph6p0Enable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD6;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_TX_DWORD6_OW2TAPGEN2DEEMPH6P0_5_0,
          (UINT32) (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen2DeEmph6p0 << N_PCH_HSIO_TX_DWORD6_OW2TAPGEN2DEEMPH6P0_5_0));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }

      if ((PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) || (PchPolicyPpi->HsioPcieConfig.Lane[Index].HsioTxGen1DeEmphEnable == TRUE)) {
        MmioAnd8 (RpBase + R_PCH_PCIE_LCTL, (UINT8) ~(B_PCIE_LCTL_LD));
      }
    }
  }

  //
  // PTSS programming for WM20 FIA PCIe Lanes
  //
  for (Index = 0; Index < PCH_MAX_WM20_LANES_NUMBER; Index++) {
    LaneNum = Index;
    PchGetLaneInfoWM20 (LaneNum, &PortId, &LaneOwner);
    if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_PCIEDMI) {
      HsioGetLaneWM20 (LaneNum, &HsioLane);
      if (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioRxSetCtleEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD25;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0,
          (UINT32) (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioRxSetCtle << N_PCH_HSIO_RX_DWORD25_CTLE_ADAPT_OFFSET_CFG_4_0));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }
      if (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioIcfgAdjLimitLoEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD39;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0,
          (UINT32) (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioIcfgAdjLimitLo << N_PCH_HSIO_RX_DWORD39_ICFG_ADJ_LIMITL0));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }
      if (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioSampOffstEvenErrSpEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD40;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP,
          (UINT32) (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioSampOffstEvenErrSp << N_PCH_HSIO_RX_DWORD40_SAMP_OFFST_EVEN_ERRSP));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }
      if (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioRemainingSamplerOffEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD41;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS,
          (UINT32) (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioRemainingSamplerOff << N_PCH_HSIO_RX_DWORD41_REMAINING_SAMP_OFFSTS));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }
      if (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioVgaGainCalEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_RX_DWORD7;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL,
          (UINT32) (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioVgaGainCal << N_PCH_HSIO_RX_DWORD7_VGA_GAIN_CAL));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }

      if ((PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) ||
          (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen2DownscaleAmpEnable == TRUE) ||
          (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen3DownscaleAmpEnable == TRUE)) {

        Data32Or  = 0;
        Data32And = 0xFFFFFFFF;
        if (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen1DownscaleAmpEnable == TRUE) {
          Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen1DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0);
          Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE00MARGIN_5_0;
        }

        if (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen2DownscaleAmpEnable == TRUE) {
          Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen2DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0);
          Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0;
        }

        if (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen3DownscaleAmpEnable == TRUE) {
          Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen3DownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE10MARGIN_5_0);
          Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD8_ORATE10MARGIN_5_0;
        }

        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD8;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          Data32And,
          Data32Or);
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }

      if ((PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen1DeEmphEnable == TRUE) ||
          (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen2DeEmph3p5Enable == TRUE)) {

        Data32Or  = 0;
        Data32And = 0xFFFFFFFF;
        if (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen1DeEmphEnable == TRUE) {
          Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen1DeEmph << N_PCH_HSIO_TX_DWORD5_OW2TAPGEN1DEEMPH3P5_5_0);
          Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD5_OW2TAPGEN1DEEMPH3P5_5_0;
        }

        if (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen2DeEmph3p5Enable == TRUE) {
          Data32Or |= (UINT32) (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen2DeEmph3p5 << N_PCH_HSIO_TX_DWORD5_OW2TAPGEN2DEEMPH3P5_5_0);
          Data32And &= (UINT32)~B_PCH_HSIO_TX_DWORD5_OW2TAPGEN2DEEMPH3P5_5_0;
        }

        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD5;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          Data32And,
          Data32Or);
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }

      if (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen2DeEmph6p0Enable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD6;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_TX_DWORD6_OW2TAPGEN2DEEMPH6P0_5_0,
          (UINT32) (PchPolicyPpi->HsioPcieConfigFIAWM20.Lane[Index].HsioTxGen2DeEmph6p0 << N_PCH_HSIO_TX_DWORD6_OW2TAPGEN2DEEMPH6P0_5_0));
        PchPcrRead32 ((PCH_SBI_PID) PortId, Offset, &Data32);
        DEBUG ((DEBUG_INFO, "HSIO : PortID = 0x%02x, Offset = 0x%04x, Value = 0x%08x\n", (PCH_SBI_PID) PortId, (UINT16) Offset, (UINT32) Data32));
      }
    }
  }

  //
  // According to PCIE spec, following Link Disable PCIE device may take up to 100ms to become operational.
  // Wait until all ports are operational again or until 100ms passed
  //
  TimeSpent = 0;
  while ((TimeSpent < 100) && (RpToRetrain != 0)) {
	for (Index = 0; Index < MaxPciePorts; Index++) {
      if ((RpToRetrain & (BIT0 << Index)) == 0) {
        continue;
      }
      RpBase = PchPcieBase (Index);
      if (MmioRead16 (RpBase + R_PCH_PCIE_LSTS) & B_PCIE_LSTS_LA) {
        RpToRetrain &= ~(BIT0 << Index);
      }
    }
    MicroSecondDelay (1000);
    TimeSpent++;
  }
}

VOID
PchHsioEnablePcieComplianceMode (
  VOID
  )
{
  UINT32     MaxPcieLanes;
  UINT32     PcieIndex;
  UINT8      HsioLaneNo;
  HSIO_LANE  HsioLane;



  MaxPcieLanes = GetPchMaxPciePortNum ();

  for (PcieIndex = 0; PcieIndex < MaxPcieLanes; ++PcieIndex) {
    if (PchGetPcieLaneNum (PcieIndex, &HsioLaneNo) == EFI_SUCCESS) {
      DEBUG ((DEBUG_INFO, "PCIe compliance mode for PCIe%d, HSIO%d\n", PcieIndex + 1, HsioLaneNo));
      HsioGetLane (HsioLaneNo, &HsioLane);
      HsioLaneAndThenOr32 (&HsioLane, R_PCH_HSIO_PCS_DWORD4, (UINT32) ~BIT23, 0);
    }
  }


}

/**
  The function programs common HSIO registers.

  @param[in]  PchPolicyPpi        The PCH Policy PPI instance

  @retval EFI_SUCCESS             THe HSIO settings have been programmed correctly
**/
EFI_STATUS
PchHsioCommonProg (
  IN  PCH_POLICY_PPI      *PchPolicyPpi
  )
{
  PCH_SBI_HSIO_TABLE_STRUCT   *PchHsioTable;
  UINT8                       PhyModeNum;
  PCH_SERIES                  PchSeries;
  UINT16                      HsioSize;
  UINT8                       Entry;
  UINT32                      Los1;
  UINT32                      Los2;
  UINT32                      Los3;
  UINT32                      Los4;
#if ME_SUPPORT_FLAG
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#if SPS_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
  CLOCKING_MODES              ClockingMode;
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
#endif // ME_SUPPORT_FLAG

  PchHsioTable = NULL;
  PhyModeNum = 0;
  HsioSize = 0;
  Entry = 0;
  Los3 = 0;
  Los4 = 0;

  PchSeries = GetPchSeries ();

  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS1_REG_BASE, &Los1);
  DEBUG ((DEBUG_INFO, "FIA LOS1 = %08x\n", Los1));
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS2_REG_BASE, &Los2);
  DEBUG ((DEBUG_INFO, "FIA LOS2 = %08x\n", Los2));
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS3_REG_BASE, &Los3);
  DEBUG ((DEBUG_INFO, "FIA LOS3 = %08x\n", Los3));
  PchPcrRead32 (PID_FIAWM26, R_PCH_PCR_FIA_LOS4_REG_BASE, &Los4);
  DEBUG ((DEBUG_INFO, "FIA LOS4 = %08x\n", Los4));


  ///
  /// Step 2
  /// Assign the appropriate BIOS HSIO table
  ///
  switch (PchStepping ()) {
    //
    // LBG HSIO tables
    //
    case LbgA0:
      PchHsioTable = PchLbgHsio_Ax;
      HsioSize = (UINT16) (sizeof (PchLbgHsio_Ax) / sizeof (PCH_SBI_HSIO_TABLE_STRUCT));
      break;
    case LbgB0:
    case LbgB1:
    case LbgB2:
#if ME_SUPPORT_FLAG
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#if SPS_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
      ClockingMode = InternalStandard;
      PeiGetCurrenClockingMode(&ClockingMode);
      if (ClockingMode == External) {
        PchHsioTable = PchLbgHsio_Bx_Ext_Ptr;
        HsioSize = PchLbgHsio_Bx_Ext_Size;
      }
      else
#endif
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#endif // ME_SUPPORT_FLAG
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
      {
        PchHsioTable = PchLbgHsio_Bx_Ptr;
        HsioSize = PchLbgHsio_Bx_Size;
      }
      break;
#ifdef SKXD_EN
    case LbgB1_D:
#if ME_SUPPORT_FLAG
      ClockingMode = InternalStandard;
      PeiGetCurrenClockingMode(&ClockingMode);
      if (ClockingMode == External) {
        PchHsioTable = PchLbgHsio_BxD_Ext_Ptr;
        HsioSize = PchLbgHsio_BxD_Ext_Size;
      }
      else
#endif // ME_SUPPORT_FLAG
      {
        PchHsioTable = PchLbgHsio_BxD_Ptr;
        HsioSize = PchLbgHsio_BxD_Size;
      }
      break;
#endif // SKXD_EN
    case LbgS0:
    case LbgS1:
#if ME_SUPPORT_FLAG
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#if SPS_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
      ClockingMode = InternalStandard;
      PeiGetCurrenClockingMode(&ClockingMode);
      if (ClockingMode == External) {
        PchHsioTable = PchLbgHsio_Sx_Ext_Ptr;
        HsioSize = PchLbgHsio_Sx_Ext_Size;
      }
      else
// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#endif
// APTIOV_SERVER_OVERRIDE_RC_END: Fixed build error for WorkStation.
#endif // ME_SUPPORT_FLAG
      {
        PchHsioTable = PchLbgHsio_Sx_Ptr;
        HsioSize = PchLbgHsio_Sx_Size;
      }
      break;      
    default:
      PchHsioTable = NULL;
      HsioSize = 0;
      DEBUG ((DEBUG_ERROR, "Cannot find HSIO configuration table for PCH stepping %d\n", PchStepping()));
      ASSERT (FALSE);

  }

  ///
  /// Step 3
  /// Iterate through the HSIO table for WM26 entries
  ///
  for (Entry = 0; Entry < HsioSize; Entry++){
    if (((PchHsioTable[Entry].Offset & B_PCH_HSIO_ACCESS_TYPE) == V_PCH_HSIO_ACCESS_TYPE_MULCAST) &&
      ((PchHsioTable[Entry].Offset & B_PCH_HSIO_LANE_GROUP_NO) != PCH_HSIO_LANE_GROUP_COMMON_LANE)) {
      if ((PchHsioTable[Entry].PortId == PID_MODPHY4) || (PchHsioTable[Entry].PortId == PID_MODPHY5) ||
        PchLosPhyModeCheck ((PCH_SBI_PID) PchHsioTable[Entry].PortId, PchHsioTable[Entry].LanePhyMode, PchSeries, Los1, Los2, Los3, Los4) == EFI_SUCCESS){

        ///
        /// Step 3a
        /// If the HSIO offset is targeted to all the phymode specific data lanes attached to the common lane (MULCAST)
        /// then program the values only if the PhyModes actually exist on the platform based on FIA LOS
        ///
        DEBUG ((DEBUG_INFO, "Programming HSIO MULCAST : PortID = %02x, Offset = %04x, Value = %08x, LanePhyMode = %02x\n", (PCH_SBI_PID) PchHsioTable[Entry].PortId, (UINT16) PchHsioTable[Entry].Offset, (UINT32) PchHsioTable[Entry].Value, PchHsioTable[Entry].LanePhyMode));
        PchPcrWrite32 ((PCH_SBI_PID) PchHsioTable[Entry].PortId, (UINT16) PchHsioTable[Entry].Offset, (UINT32) PchHsioTable[Entry].Value);
      } else {
        DEBUG ((DEBUG_INFO, "Skipping HSIO MULCAST : PortID = %02x, Offset = %04x, Value = %08x, LanePhyMode = %02x\n", (PCH_SBI_PID) PchHsioTable[Entry].PortId, (UINT16) PchHsioTable[Entry].Offset, (UINT32) PchHsioTable[Entry].Value, PchHsioTable[Entry].LanePhyMode));
      }
    } else {
      ///
      /// Step 3b
      /// If the HSIO offset is targeted to all the data lanes attached to the common lane (BDCAST)
      /// or the offset target is the common lane then program without any checks
      ///
      DEBUG ((DEBUG_INFO, "HSIO : PortID = %02x, Offset = %04x, Value = %08x\n", (PCH_SBI_PID) PchHsioTable[Entry].PortId, (UINT16) PchHsioTable[Entry].Offset, (UINT32) PchHsioTable[Entry].Value));
      PchPcrWrite32 ((PCH_SBI_PID) PchHsioTable[Entry].PortId, (UINT16) PchHsioTable[Entry].Offset, (UINT32) PchHsioTable[Entry].Value);
    }
  }

  if (PchPolicyPpi->PcieConfig.ComplianceTestMode) {
    PchHsioEnablePcieComplianceMode ();
  }

  return EFI_SUCCESS;
}


/**
  The function programs the Pcie Pll SSC registers.

  @param[in]  PchPolicyPpi        The PCH Policy PPI instance
**/
VOID
PchPciePllSscProg (VOID)
{
  EFI_STATUS                            Status;
  CLOCKING_MODES                        ClockingMode;

// APTIOV_SERVER_OVERRIDE_RC_START : Fixed build error for WorkStation.
#if SPS_SUPPORT
//#if ME_SUPPORT_FLAG
// APTIOV_SERVER_OVERRIDE_RC_END : Fixed build error for WorkStation.
  Status = PeiGetCurrenClockingMode(&ClockingMode);
#else
  if (PchStepping () == LbgA0) {
    ClockingMode = InternalStandard;
  } else {
    ClockingMode = InternalAlternate;
  }
  Status = EFI_SUCCESS;
#endif // ME_SUPPORT_FLAG
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "PchConfigureSsc. Can't read clock mode! EFI_STATUS= 0x%x\n", Status));
    return;
  }

  switch (ClockingMode) {
    case InternalStandard:
    {
      DEBUG ((DEBUG_INFO, "PchConfigureSsc. Clock Mode: Internal Standard\n"));

      PchConfigureSscWm20Gen2Pll();
      break;
    }
    case HybridStandard:
    {
      DEBUG ((DEBUG_INFO, "PchConfigureSsc. Clock Mode: Hybrid Standard\n"));

      PchConfigureSscWm20Gen2Pll();
      break;
    }
    case InternalAlternate:
    {
      DEBUG ((DEBUG_INFO, "PchConfigureSsc. Clock Mode: Internal Alternate\n"));
#if SPS_SUPPORT
      if (MeTypeIsSps())
      {
        PchConfigureSscAlternate ();
      } else {
        DEBUG ((DEBUG_ERROR, "PchConfigureSsc. Non SPS firmware. No SSC support\n"));
      }
#else
      DEBUG ((DEBUG_ERROR, "PchConfigureSsc. Non SPS firmware. No SSC support\n"));
#endif // SPS_SUPPORT
      break;
    }
    case HybridAlternate:
    {
      DEBUG ((DEBUG_INFO, "PchConfigureSsc. Clock Mode: Hybrid Alternate\n"));
#if SPS_SUPPORT
      if (MeTypeIsSps())
      {
        PchConfigureSscAlternate();
      } else {
        DEBUG ((DEBUG_ERROR, "PchConfigureSsc. Non SPS firmware. No SSC support\n"));
      }
#else
      DEBUG ((DEBUG_ERROR, "PchConfigureSsc. Non SPS firmware. No SSC support\n"));
#endif // SPS_SUPPORT
      break;
    }
  }
}

/**
  The function program HSIO registers.

  @param[in] PchPolicyPpi         The PCH Policy PPI instance

  @retval EFI_SUCCESS             THe HSIO settings have been programmed correctly
**/
EFI_STATUS
PchHsioBiosProg (
  IN  PCH_POLICY_PPI    *PchPolicyPpi
  )
{
  DEBUG ((DEBUG_INFO, "PchHsioBiosProg() Start\n"));
  PchHsioCommonProg (PchPolicyPpi);
  PchHsioRxTxEqPolicyProg (PchPolicyPpi);
  PchPciePllSscProg ();
  DEBUG ((DEBUG_INFO, "PchHsioBiosProg() End\n"));
  return EFI_SUCCESS;
}
