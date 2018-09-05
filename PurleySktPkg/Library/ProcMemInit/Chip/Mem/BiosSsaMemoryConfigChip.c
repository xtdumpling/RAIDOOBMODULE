/** @file
  This file contains the BIOS implementation of the BIOS-SSA Memory Configuration API.

@copyright
  Copyright (c) 2015 - 2016 Intel Corporation. All rights
  reserved. This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by such license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include "SysFunc.h"
#include "RcRegs.h"
#include "SysHostChip.h"
#include "CpuPciAccess.h"
#include "MemApiSkx.h"

#ifdef SSA_FLAG

/**
  Find if there is a LRDIMM present on the platform

  @retval TRUE/FALSE     TRUE: LRDIMM is present
**/
BOOLEAN
SSAIsLrdimmPresent(
  PSYSHOST         host
  )
{
  UINT8                     dimm, ch, socket;
  struct dimmNvram          (*dimmNvList)[MAX_DIMM];
  struct channelNvram       (*channelNvList)[MAX_CH];
  struct socketNvram        (*socketNvram)[MAX_SOCKET];

  socketNvram = &host->nvram.mem.socket;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if ((*socketNvram)[socket].enabled == 0) continue;
    channelNvList = GetChannelNvList(host, socket);
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        if (IsLrdimmPresent(host, socket, ch, dimm)) {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

/**
  Find if there is a DDRT(AEP) DIMM present on the platform

  @retval TRUE/FALSE     TRUE: DDRT(AEP) DIMM is present
**/
BOOLEAN
SSAIsDdrtDimmPresent (
  PSYSHOST         host
  )
{
  if (host->nvram.mem.aepDimmPresent) {
    return TRUE;
  }

  return FALSE;
}

/**
  Find if there is any RegisterRev2 Dimm is present on the platform

  @retval TRUE/FALSE     TRUE: RegisterRev2 Dimm is present
**/
BOOLEAN
SSAIsRegRev2DimmPresent(
  PSYSHOST         host
  )
{
  UINT8                     dimm, ch, socket;
  struct dimmNvram          (*dimmNvList)[MAX_DIMM];
  struct channelNvram       (*channelNvList)[MAX_CH];
  struct socketNvram        (*socketNvram)[MAX_SOCKET];

  socketNvram = &host->nvram.mem.socket;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if ((*socketNvram)[socket].enabled == 0) continue;
    channelNvList = GetChannelNvList(host, socket);
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if (IsDdr4RegisterRev2(host, socket, ch, dimm)) {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

//
//This BSSA file contains subsets of all BSSA function calls made from the Core files; all CSR accesses are made here
//

/**

  This routine gets the CSRs required by BiosGetMemTemp

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Dimm                       - Zero based Dimm number (0-based)
  @param Temperature                - Pointer to where the DIMM's temperature will be written

  @retval N/A

**/
VOID
BiosGetMemTempChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Dimm,
  INT32                 *Temperature
  )
{
  UINT8                                mcId;
  UINT16                               mstData;
  UINT32                               smbCmdReg;
  SMB_CMD_CFG_1_PCU_FUN5_STRUCT        smbCmd;
  DIMMTEMPSTAT_0_MCDDC_CTL_STRUCT      DimmTempStat;
  struct smbDevice                     tsod;

  mstData = 0;
  mcId    = GetMCID(host, Socket, ChannelInSocket);

  tsod.compId = MTS;
  tsod.address.controller   = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
  tsod.address.deviceType   = DTI_TSOD;
  tsod.address.busSegment   = ChannelInSocket/MAX_MC_CH;  //Determines the MC number
  tsod.address.strapAddress = Dimm + ((ChannelInSocket % MAX_MC_CH) * MAX_DIMM);  //Val: 0,1,2,3,4,5 per MC
  tsod.mcId = mcId;

  if (tsod.address.busSegment == 0) {
    smbCmdReg   = SMB_CMD_CFG_0_PCU_FUN5_REG;
  } else {
    smbCmdReg   = SMB_CMD_CFG_1_PCU_FUN5_REG;
  }

  smbCmd.Data = ReadCpuPciCfgEx (host, Socket, PCU_INSTANCE_0, smbCmdReg);
  if (smbCmd.Bits.smb_tsod_poll_en == 1) {
    DimmTempStat.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, (DIMMTEMPSTAT_0_MCDDC_CTL_REG + (Dimm * 4)));
    *Temperature = DimmTempStat.Bits.dimm_temp;
  } else {
    ReadSmb (host, Socket, tsod, MTS_TEMP, (UINT8 *)&mstData); //Get data from device = TSOD for offset = MTS_TEMP register 5
    mstData = ((mstData & 0x0FF0) >> 4); //Taking only bits 11:4 for the dimm temp
    *Temperature = mstData;
  }
}

/**

  This routine sets CSRs required by BiosIoReset

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param Controller                 - Controller number (0-based)
  @param resetIo                    - Reset/Clear DDRIO (0-based)

  @retval N/A

**/
VOID
BiosIoResetChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 resetIo
  )
{
  MC_INIT_STATE_G_MC_MAIN_STRUCT           mcInitStateG;

  mcInitStateG.Data = MemReadPciCfgMC (host, Socket, Controller, MC_INIT_STATE_G_MC_MAIN_REG);
  mcInitStateG.Bits.reset_io = resetIo;
  MemWritePciCfgMC (host, Socket, Controller, MC_INIT_STATE_G_MC_MAIN_REG, mcInitStateG.Data);
}

/**

  This routine sets required by BiosInitMarginParam

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - ChannelInSocket number (0-based)
  @param cmdOeVal                   - Set/Clear cmdOeVal (0-based)

  @retval N/A

**/
VOID
BiosInitMarginParamChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 cmdOeVal
  )
{
  TCDBP_MCDDC_CTL_STRUCT                 tcdbp;

  tcdbp.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, TCDBP_MCDDC_CTL_REG);
  tcdbp.Bits.cmd_oe_on = cmdOeVal;
  MemWritePciCfgEp (host, Socket, ChannelInSocket, TCDBP_MCDDC_CTL_REG, tcdbp.Data);
}

/**

  This routine gets/sets CSRs required by BiosSetZQCalConfig

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param Controller                 - Controller number (0-based)
  @param Enable                     - Specifies whether ZQCal is enabled.  TRUE enables ZQCal; FALSE disables it
  @param PrevEnable                 - Pointer to where the previous ZQCal enable setting will be stored. This value may be NULL

  @retval N/A

**/
VOID
BiosSetZQCalConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable,
  BOOLEAN               *PrevEnable
  )
{
  UINT8                                     channelInSocket = 0;
  UINT8                                     maxChannel = 0;
  UINT8                                     iteration = 0;
  struct channelNvram                       (*channelNvList)[MAX_CH];
  CPGC_MISCZQCTL_MCDDC_CTL_STRUCT           miscZqCtl;

  channelNvList = GetChannelNvList(host, Socket);

  if (Controller == 1) {
    channelInSocket += MAX_CH_IMC;  //If Controller = 1, Channels = 2,3 for BDX and 3,4,5 for SKX
    maxChannel = MAX_CH;
  } else {
    maxChannel = MAX_CH_IMC;        //2 for BDX, 3 for SKX
  }

  for (channelInSocket; channelInSocket < maxChannel; channelInSocket++) {
    if ((*channelNvList)[channelInSocket].enabled == 0) continue;

    miscZqCtl.Data  = MemReadPciCfgEp (host, Socket, channelInSocket, CPGC_MISCZQCTL_MCDDC_CTL_REG);

    if ((PrevEnable != NULL) && (iteration == 0)) {  //Get Case. Save Previous value for only the First Enabled Channel
      if (miscZqCtl.Bits.zq_rankmask == 0) {  //Enable ZQ Cal
        *PrevEnable = TRUE;
      } else {
        *PrevEnable = FALSE;
      }
    }

     miscZqCtl.Bits.zq_rankmask = (Enable) ? 0 : 0xFF;  // 0xFF --> disable ZQ to all the Ranks on the Channel
     MemWritePciCfgEp (host, Socket, channelInSocket, CPGC_MISCZQCTL_MCDDC_CTL_REG, miscZqCtl.Data);

     iteration = 1;
  } //for-loop
}

/**

  This routine gets/sets CSRs required by BiosSetRCompUpdateConfig

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param Enable                     - Specifies whether RComp updates are enabled.  TRUE enables RComp updates; FALSE disables them
  @param PrevEnable                 - Pointer to where the previous RComp update enable setting will be stored.  This value may be NULL

  @retval N/A

**/
VOID
BiosSetRCompUpdateConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  BOOLEAN               Enable,
  BOOLEAN               *PrevEnable
  )
{
  UINT8                                     originalRcompVal;
  UINT32                                    PcodeMailboxStatus = 0;
  UINT32                                    PCUData = 0;

  // Read PCU_MISC_CONFIG data
  PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, Socket, (UINT32) MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG, 0);
  if (PcodeMailboxStatus == 0) {
    PCUData = ReadCpuPciCfgEx (host, Socket, 0, BIOS_MAILBOX_DATA);

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
    MemDebugPrint((host, SDBG_MAX, Socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "MboxStatus: %d PCU_MISC_CONFIG = 0x%x\n", PcodeMailboxStatus, PCUData));
#endif
  }

  //Set bit 24 to disable periodic RCOMP .. HSD s4167626
  //Set bit 23 to disable pkg exit RCOMP

  if (PrevEnable != NULL) {  //Get Case
    if (((PCUData >> 24) & 1) == 1) {
      originalRcompVal = 1;
    } else {
      originalRcompVal = 0;
    }
    *PrevEnable = originalRcompVal;  //Save original Bit 24 value -> periodic RCOMP
  }

  if (Enable == FALSE) {
    PCUData |= (BIT0 << 24);           // Set bit 24 to disable periodic RCOMP
  } else {
    PCUData &= (0xFEFFFFFF);           // Set bit 24 to 0 to enable periodic RCOMP
  }

  //
  // HSD s4986822: PKGC RCOMP disable WA
  //
  PCUData |= (BIT0 << 23);

  // Write back PCU_MISC_CONFIG data
  PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, Socket, (UINT32) MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG, PCUData);
  PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, Socket, MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG, 0);

  if (PcodeMailboxStatus == 0) {
    PCUData = ReadCpuPciCfgEx (host, Socket, 0, BIOS_MAILBOX_DATA);

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
    MemDebugPrint((host, SDBG_MAX, Socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "MBoxStatus: %d PCU_MISC_CONFIG = 0x%x\n", PcodeMailboxStatus, PCUData));
#endif
  }
}

/**

  This routine gets/sets CSRs required by BiosSetPageOpenConfig

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param Controller                 - Controller number (0-based)
  @param Enable                     - Specifies whether page-open is enabled.  TRUE enables page-open; FALSE disables it
  @param PrevEnable                 - Pointer to where the previous page-open enable setting will be stored.  This value may be NULL

  @retval N/A

**/
VOID
BiosSetPageOpenConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable,
  BOOLEAN               *PrevEnable
  )
{
  MCMTR_MC_MAIN_STRUCT                       mcmtrReg;

  mcmtrReg.Data = MemReadPciCfgMC (host, Socket, Controller, MCMTR_MC_MAIN_REG);

  //
  // Set to Open Page Mode
  //
  if (PrevEnable != NULL) {  //Get Case
    *PrevEnable = (mcmtrReg.Bits.close_pg & 1);
  }

  if (Enable == TRUE) {
    mcmtrReg.Bits.close_pg = CLOSED_PAGE_DIS;
  } else {
    mcmtrReg.Bits.close_pg = CLOSED_PAGE_EN;  //Enabling Closed Page Mode
  }
  MemWritePciCfgMC (host, Socket, Controller, MCMTR_MC_MAIN_REG, mcmtrReg.Data);
}

/**

  This routine sets CSRs required by BiosInitCpgc

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param Controller                 - Controller number (0-based)
  @param SetupCleanup               - Specifies setup or cleanup action

  @retval N/A

**/
VOID
BiosInitCpgcChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  SETUP_CLEANUP         SetupCleanup
  )
{
  MCMTR_MC_MAIN_STRUCT                    mcMtr;
  //Enter or exit CPGC mode
  mcMtr.Data = MemReadPciCfgMC (host, Socket, Controller, MCMTR_MC_MAIN_REG);
  mcMtr.Bits.trng_mode = (SetupCleanup == Setup)? 1 : 0;
  //At the postcode where the BSSA runs, the "normal" mode bit is still 0
  //mcMtr.Bits.normal = 0;
  MemWritePciCfgMC (host, Socket, Controller, MCMTR_MC_MAIN_REG, mcMtr.Data);
}

/**

  BiosGetMarginParamSpecificityChip: This routine gets the specificity of a given margin parameter.

  @param host                       - Pointer to sysHost
  @param IoLevel                    - I/O level.
  @param MarginGroup                - Margin group.
  @param Specificity                - Pointer to where the margin parameter specificity mask will be stored.

  @retval UnsupportedValue if the IoLevel or MarginGroup parameter value is not supported.  Else Success.

**/
SSA_STATUS
BiosGetMarginParamSpecificityChip (
  PSYSHOST                 host,
  GSM_LT                   IoLevel,
  GSM_GT                   MarginGroup,
  MARGIN_PARAM_SPECIFICITY *Specificity
  )
{
  SSA_STATUS                     Status = Success;
  *Specificity = ChannelSpecific|ControllerSpecific|SocketSpecific;  //Default value

  switch (MarginGroup) {
    case CtlAll:
    case CtlGrp0:
    case CtlGrp1:
    case CtlGrp2:
    case CtlGrp3:
    case CmdAll:
    case CmdGrp0:
    case CmdGrp1:
    case CmdGrp2:
    case CmdGrp3:
    case CmdGrp4:
    case CmdVref:
      if (IoLevel == DdrLevel && ((MarginGroup != CmdGrp3) && (MarginGroup != CmdGrp4))) {
        *Specificity = ChannelSpecific|ControllerSpecific|SocketSpecific;
      } else { //For 'LrbufLevel' && Rev2 RDIMM & LRDIMM - backside CmdAll/CtlAll/CmdVref is supported
        if ((MarginGroup == CmdVref) && (SSAIsRegRev2DimmPresent(host) || (SSAIsDdrtDimmPresent(host)))) {
          *Specificity = RankSpecific | ChannelSpecific | ControllerSpecific | SocketSpecific;
        } else if (((MarginGroup == CmdAll) || (MarginGroup == CtlAll)) && (SSAIsRegRev2DimmPresent(host) || (SSAIsDdrtDimmPresent(host) && host->setup.mem.enableNgnBcomMargining))) {
          *Specificity = RankSpecific | ChannelSpecific | ControllerSpecific | SocketSpecific;
        } else if (((MarginGroup == CmdGrp3) || (MarginGroup == CmdGrp4)) && (SSAIsDdrtDimmPresent(host)) && (host->setup.mem.enableNgnBcomMargining)) {
          *Specificity = RankSpecific | ChannelSpecific | ControllerSpecific | SocketSpecific;
        } else {
          Status = UnsupportedValue;
        }
      }
      break;

    case RxDqsDelay:
    case TxDqDelay:
    case RxVref:
    case TxVref:
      if ((IoLevel == DdrLevel) || ((IoLevel == LrbufLevel) && (SSAIsLrdimmPresent(host)))) {
        *Specificity = StrobeSpecific|RankSpecific|ChannelSpecific|ControllerSpecific|SocketSpecific;
      } else {
        Status = UnsupportedValue;
      }
      break;

    case RecEnDelay:
    case TxDqsDelay:
    case WrLvlDelay:
    case RxDqsPDelay:
    case RxDqsNDelay:
    case RxDqDelay:
    case RxEq:
    case TxEq:
      if (IoLevel == DdrLevel) {
        *Specificity = StrobeSpecific|RankSpecific|ChannelSpecific|ControllerSpecific|SocketSpecific;
      } else {
        Status = UnsupportedValue;
      }
      break;

    case RxDqsBitDelay:
    case TxDqBitDelay:
      if (IoLevel == DdrLevel) {
        *Specificity = LaneSpecific|StrobeSpecific|RankSpecific|ChannelSpecific|ControllerSpecific|SocketSpecific;
      } else {
        Status = UnsupportedValue;
      }
      break;

    case EridDelay:
      if ((IoLevel == DdrLevel) && (SSAIsDdrtDimmPresent(host))) {
        *Specificity = RankSpecific|ChannelSpecific|ControllerSpecific|SocketSpecific;
      } else {
        Status = UnsupportedValue;
      }
      break;

    case EridVref:
      if ((IoLevel == DdrLevel) && (SSAIsDdrtDimmPresent(host))) {
        *Specificity = ChannelSpecific|ControllerSpecific|SocketSpecific;
      } else {
        Status = UnsupportedValue;
      }
      break;

    default:
      Status = UnsupportedValue;
      break;
  }

  return Status;
}

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

// end file BiosSsaMemoryConfig.c
