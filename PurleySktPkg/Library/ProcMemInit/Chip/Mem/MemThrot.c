//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  ADD TOKEN to control the 2x refresh low/mid temperature.
//                  2x refresh enabled low temperature : SMC_CLTT_TEMP_LO_DOUBLE_REFRESH_DEFAULT
//                  2x refresh enabled mid temperature : SMC_CLTT_TEMP_MID_DOUBLE_REFRESH_DEFAULT
//                  2x refresh disabled low temperature : SMC_CLTT_TEMP_LO_SINGLE_REFRESH_DEFAULT
//                  2x refresh disabled mid temperature : SMC_CLTT_TEMP_MID_SINGLE_REFRESH_DEFAULT
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Mar/29/2017
//
//****************************************************************************
/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Memory Reference Code
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
 *      This file contains memory detection and initialization for
 *      IMC and DDR3 modules compliant with JEDEC specification.
 *
 ************************************************************************/

#include "Token.h" //SMCPKG_SUPPORT
#include "SysFunc.h"
#include "SysFuncChip.h"
#include "MemThrot.h"
#include "MemWeight.h"
#include "SysHost.h"
#include "RcRegs.h"
#include "OemProcMemInit.h"
#include "FnvAccess.h"
#include "SysHostChip.h"
#include "MemApiSkx.h"

#define MemDebugPrintLocal(dbgInfo)

//
// Local Prototypes
//
static UINT32 DramRaplProg(PSYSHOST host, UINT8 socket, DramRaplDataStruct *RAPLData);
static void   DramRaplProgCalc(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, DramRaplDataStruct *RAPLData, DramRaplLUTDataStruct *RAPLLUTData);
static void   UpdateTsodPopulation(PSYSHOST host, UINT8 socket, UINT8 mcId);
static UINT32 ControlDramPMThermalViaPcode (PSYSHOST host, UINT8 flag);
static UINT32 ControlRasViaPcode (PSYSHOST host, UINT8 flag);

//
// Disable warning for unsued input parameters
//
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif

static UINT16
RefreshScaledPwr(
                PSYSHOST host,
                UINT8    socket,
                UINT32   slope,
                UINT16   powerInput
                );

/**

  Initialize memory throttling

  @param host  - Point to sysHost

  @retval SUCCESS

**/
UINT32
InitThrottlingEarly (
                    PSYSHOST host
                    )
{
  UINT8                                           socket;
  UINT8                                           ch;
  UINT8                                           mcId;
  UINT8                                           dimm;
  DIMM_TEMP_TH_0_MCDDC_CTL_STRUCT                 dimmTempTh;
  DIMM_TEMP_THRT_LMT_0_MCDDC_CTL_STRUCT           dimmTempThrtLmt;
  DIMM_TEMP_EV_OFST_0_MCDDC_CTL_STRUCT            dimmTempEvOfst;
  SMB_CMD_CFG_0_PCU_FUN5_STRUCT                   smbCmd;
  THRT_PWR_DIMM_0_MCDDC_CTL_STRUCT                thrtPwrDimm;
  SMB_TSOD_POLL_RATE_CFG_0_PCU_FUN5_STRUCT        smbTsodPollRate;
  CHN_TEMP_CFG_MCDDC_CTL_STRUCT                   ChnTempCfgReg;

  socket = host->var.mem.currentSocket;

  smbTsodPollRate.Data = MemReadPciCfgEp (host, socket, PCU_INSTANCE_0, SMB_TSOD_POLL_RATE_CFG_0_PCU_FUN5_REG);
  smbTsodPollRate.Bits.smb_tsod_poll_rate = 0x3e800;
  MemWritePciCfgEp (host, socket, PCU_INSTANCE_0, SMB_TSOD_POLL_RATE_CFG_0_PCU_FUN5_REG, smbTsodPollRate.Data);

  for (mcId = 0; mcId < MAX_IMC; mcId++) {
    if(host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

    if (mcId == 0) {
      smbCmd.Data = ReadCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_CMD_CFG_0_PCU_FUN5_REG);
      smbCmd.Bits.smb_tsod_poll_en = 0;
      WriteCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_CMD_CFG_0_PCU_FUN5_REG, smbCmd.Data);
    } else {
      smbCmd.Data = ReadCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_CMD_CFG_1_PCU_FUN5_REG);
      smbCmd.Bits.smb_tsod_poll_en = 0;
      WriteCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_CMD_CFG_1_PCU_FUN5_REG, smbCmd.Data);
    }
  } // mcId loop

  for (ch = 0; ch < MAX_CH; ch++) {
    //
    // Disable throttling during MRC training
    //
    ChnTempCfgReg.Data = MemReadPciCfgEp (host, socket, ch, CHN_TEMP_CFG_MCDDC_CTL_REG);
    ChnTempCfgReg.Bits.oltt_en = 0;
    ChnTempCfgReg.Bits.bw_limit_thrt_en = 0;
    ChnTempCfgReg.Bits.thrt_allow_isoch = 1;
    MemWritePciCfgEp (host, socket, ch, CHN_TEMP_CFG_MCDDC_CTL_REG, ChnTempCfgReg.Data);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      //
      // Initialize the temp threshold to safe defaults
      //
      dimmTempTh.Data =  MemReadPciCfgEp (host, socket, ch, (DIMM_TEMP_TH_0_MCDDC_CTL_REG + (dimm * 4)));
      dimmTempTh.Bits.temp_lo = 0x55;
      dimmTempTh.Bits.temp_mid = 0x5a;
      dimmTempTh.Bits.temp_hi = 0x5f;
      MemWritePciCfgEp (host, socket, ch, (DIMM_TEMP_TH_0_MCDDC_CTL_REG + (dimm * 4)), dimmTempTh.Data);

      //
      // Initialize threshold limits to maximum settings, effectively disabling bandwidth throttling
      //
      dimmTempThrtLmt.Data =  MemReadPciCfgEp (host, socket, ch, (DIMM_TEMP_THRT_LMT_0_MCDDC_CTL_REG + (dimm * 4)));
      dimmTempThrtLmt.Bits.thrt_mid = 0xFF;
      dimmTempThrtLmt.Bits.thrt_hi = 0xF;
      dimmTempThrtLmt.Bits.thrt_crit = 0;
      MemWritePciCfgEp (host, socket, ch, (DIMM_TEMP_THRT_LMT_0_MCDDC_CTL_REG + (dimm * 4)), dimmTempThrtLmt.Data);

      //
      // Disable DIMM based power throttling
      //
      thrtPwrDimm.Data = (UINT16)MemReadPciCfgEp (host, socket, ch, (THRT_PWR_DIMM_0_MCDDC_CTL_REG + (dimm * 2)));
      thrtPwrDimm.Bits.thrt_pwr_en = 0;
      MemWritePciCfgEp (host, socket, ch, (THRT_PWR_DIMM_0_MCDDC_CTL_REG + (dimm * 2)), (UINT32)thrtPwrDimm.Data);

      //
      // SKX BIOS is programming these based on thermal table which has non zero values based on internal thermal characterization
      //
      dimmTempEvOfst.Data = MemReadPciCfgEp (host, socket, ch, (DIMM_TEMP_EV_OFST_0_MCDDC_CTL_REG + (dimm * 4)));
      dimmTempEvOfst.Bits.dimm_temp_offset = 0;
      dimmTempEvOfst.Bits.ev_2x_ref_templo_en = 0;
      MemWritePciCfgEp (host, socket, ch, (DIMM_TEMP_EV_OFST_0_MCDDC_CTL_REG + (dimm * 4)), dimmTempEvOfst.Data);

    } // dimm loop
  } // ch loop

  return SUCCESS;
}

/**

  Control RAS using Pcode

    @param host  - Pointer to sysHost

    @retval VOID

**/
static UINT32
ControlRasViaPcode (
  PSYSHOST host,
  UINT8    flag
  )
{
  UINT32 PcodeMailboxData = 0;
  UINT32 PcodeMailboxStatus = 0;
  UINT8  socket;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if ((host->var.common.socketPresentBitMap & (BIT0 << socket)) == 0) continue;
    if (flag == 0) {
      //
      // BIOS issues the RAS_START(MemAdd, ID Vector) mailbox command [11:4] - Memory - 0, Socket - 1, [3:0] Add - 00, Remove - 01, ReInit - 02
      // Check If RAS start happened
      //
      PcodeMailboxData = 0;
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_RAS_START), PcodeMailboxData);
      if (PcodeMailboxStatus == 0) ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
#if 0
      PcodeMailboxData = 0;
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_RAS_START_CHECK), PcodeMailboxData);
      if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
      if ((TempData & BIT0) == 0) {
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "ControlRasViaPcode: RAS Start failed\n"));
        return FAILURE;
      }
#endif
    } else {
      //
      // BIOS issues the RAS_EXIT(MemAdd, ID Vector) mailbox command
      //
      PcodeMailboxData = 0;
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_RAS_EXIT), PcodeMailboxData);
      if (PcodeMailboxStatus == 0) ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
    }
  }
  return SUCCESS;
}

/**

  Control PM and thermal enable using Pcode

    @param host  - Pointer to sysHost

    @retval VOID

**/
static UINT32
ControlDramPMThermalViaPcode (
  PSYSHOST  host,
  UINT8     flag
  )
{
  UINT8               socket;
  UINT8               ch;
  UINT8               mcId;
  UINT8               dimm;
  UINT32              PcodeMailboxData = 0;
  UINT32              PcodeMailboxStatus = 0;
  UINT32              TempData = 0;
  BOOLEAN             DDR4PresentFlag[MAX_IMC]={0};
  BOOLEAN             DDRTPresentFlag[MAX_IMC]={0};
  BOOLEAN             ThreeDSPresentFlag = 0;
  BOOLEAN             LRDIMMPresentFlag = 0;
  UINT8               DDR4DimmMask[MAX_CH]={0};
  UINT8               DDRTDimmMask[MAX_CH]={0};
  UINT8               VrSvidMC0 = 0;
  UINT8               VrSvidMC1 = 0;
  UINT8               imcEnMask = 0;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];

  socket = host->var.mem.currentSocket;

  //Set MC Bit Mask to indicate populated MCs on Skt
  for (mcId = 0; mcId < MAX_IMC; mcId++) {
    if(host->var.mem.socket[socket].imcEnabled[mcId] == 1) {
      imcEnMask |= (1 << mcId);
    }
  } // mcId loop

  //
  // Collect the Dimm type information present in the Memory controller
  //
  channelNvList = &host->nvram.mem.socket[socket].channelList;

  if (flag == DISABLE_DRAM_MPM) {
    //
    // BIOS issues Disable_DRAM_PM
    // BIOS polls the Disable_Check
    // (MC0 – [0]; MC1 – [1] RGT0 – [2] ; RGT1 [3] ---till RGT7 – [9]) BitMask - 0x1FF
    //
    PcodeMailboxData = (UINT32)imcEnMask;
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_PM_DISABLE), PcodeMailboxData);
    if (PcodeMailboxStatus == 0) {
      TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
    }

    PcodeMailboxData = 0;
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_PM_ENABLED_MASK), PcodeMailboxData);
    if (PcodeMailboxStatus == 0) {
      TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
    }
    if ((TempData & (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) != (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) {
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "ControlDramPMThermalViaPcode: Cannot disable DRAM PM\n"));
      return FAILURE;
    }

    //
    // BIOS issues Disable_DRAM_THERMAL
    // BIOS polls the Disable_Check
    // MC0 – [0]; MC1 – [1]  BitMask - 0x03
    //
    PcodeMailboxData = (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK);
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_THERMAL_DISABLE), PcodeMailboxData);
    if (PcodeMailboxStatus == 0) {
      TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
    }

    PcodeMailboxData = 0;
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_THERMAL_ENABLED_MASK), PcodeMailboxData);
    if (PcodeMailboxStatus == 0) {
      TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
    }
    if ((TempData & (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) != 0) {
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "ControlDramPMThermalViaPcode: Cannot disable DRAM Thermal\n"));
      return FAILURE;
    }

  } else {

    //Collect the Dimm type information present in the Memory controller .
    for (mcId = 0; mcId < MAX_IMC; mcId++) {
      if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
      channelNvList = &host->nvram.mem.socket[socket].channelList;

      for (ch = 0; ch < MAX_MC_CH; ch++) {
        if ((*channelNvList)[ch + (MAX_MC_CH * mcId)].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch + (MAX_MC_CH * mcId));

        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

          if((*dimmNvList)[dimm].aepDimmPresent){
            DDRTDimmMask[(mcId * MAX_MC_CH) + ch] |= (1 << dimm); //Collect DIMM Mask of NVMDIMM DIMMs
            DDRTPresentFlag[mcId] = 1;
          } else {
            DDR4PresentFlag[mcId] = 1;
            DDR4DimmMask[(mcId * MAX_MC_CH) + ch] |= (1 << dimm); //Collect DIMM Mask of DDR4 DIMMs
          }
        } // dimm loop
      } // ch loop
    } // mcId loop

    //Pcode will set BIT0(MC0 VR) and BIT1(MC1 VR). 0x1 for only MC0 VR and 0x3 for both VRs present.
    //If bit 0 is set program 0x10 to which ever MC is enabled.
    //If bit 1 and bit zero are set program 0x10 to MC0 and 0x12 to MC1.
    //If only bit 1 is set program 0x12 to which ever MC is enabled.
    //0x10 is the address for MC0 VR; 0x12 is the address for MC1 VR
    PcodeMailboxData = 0;
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_PROBE_DRAM_VR), PcodeMailboxData);
    if (PcodeMailboxStatus == 0) {
      TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
    } else {
      TempData = 0;
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "MAILBOX read failed: Setting default VR values\n"));
    }

    if ((TempData & 0xF) == 0x1) { //BIT0 only set: MC0 VR
      VrSvidMC0 = 0x10;
      VrSvidMC1 = 0x10;
    } else if ((TempData & 0xF) == 0x3) { // BIT0 & 1 set: MC0 & MC1 VR
      VrSvidMC0 = 0x10;
      VrSvidMC1 = 0x12;
    } else if ((TempData & 0xF) == 0x2) { // BIT1 only set: MC1 VR
      VrSvidMC0 = 0x12;
      VrSvidMC1 = 0x12;
    } else { //If MailBox read fails(PcodeMailboxStatus != 0) hardcode default VR address values
      VrSvidMC0 = 0x10;
      VrSvidMC1 = 0x12;
    }

    for (mcId = 0; mcId < MAX_IMC; mcId++) {
      if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

      if(mcId == 0){
        if((DDR4PresentFlag[mcId]) || (DDRTPresentFlag[mcId])){
          PcodeMailboxData = (VrSvidMC0 << 8) | 0; //VR SVID address for MC0 is 0x10
          PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_VR_MC_MAPPING), PcodeMailboxData);
          if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
          if (TempData!=0){
            MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "ControlDramPMThermalViaPcode: BAD VR ADDRESS\n"));
            return FAILURE;
          }
        }

        if(DDRTPresentFlag[mcId]){
          PcodeMailboxData = (VrSvidMC0 << 8) | 0;
          PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_VR_SENSOR_MAPPING), PcodeMailboxData);
          if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
          if (TempData!=0){
            MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "ControlDramPMThermalViaPcode: BAD VR ADDRESS\n"));
            return FAILURE;
          }
        }
      }

      if(mcId == 1){
       if((DDR4PresentFlag[mcId]) || (DDRTPresentFlag[mcId])) {
          PcodeMailboxData = (VrSvidMC1 << 8) | 1; //VR SVID address for MC0 is 0x12 (12:8)
          PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_VR_MC_MAPPING), PcodeMailboxData);
          if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
          if (TempData!=0){
            MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "ControlDramPMThermalViaPcode: BAD VR ADDRESS\n"));
            return FAILURE;
          }
        }
        if(DDRTPresentFlag[mcId]){
          PcodeMailboxData = (VrSvidMC1 << 8) | 1;
          PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_VR_SENSOR_MAPPING), PcodeMailboxData);
          if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
          if (TempData!=0){
            MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "ControlDramPMThermalViaPcode: BAD VR ADDRESS\n"));
            return FAILURE;
          }
        }
      }
      // BITS 25:24 - 12V sensor dimm mask and BITS 17:16 - DDR4 DIMM MASK. BITS 9:8 - channel id and BIT 0 -MCID
      for (ch = 0; ch < host->var.mem.numChPerMC; ch++) {
        //if(channel is enabled) check should be added here?- Needs to be checked with Nikhil.
        PcodeMailboxData = ((DDRTDimmMask[(mcId*MAX_MC_CH)+ ch]) << 24) | ((DDR4DimmMask[(mcId*MAX_MC_CH)+ ch]) << 16) | (ch << 8) | mcId;
        PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_DIMM_VR_MAPPING), PcodeMailboxData);
        if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
          if (TempData!=0){
            MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "ControlDramPMThermalViaPcode: BAD CHANNEL INDEX\n"));
            return FAILURE;
          }
      } // ch loop
    } // mcId loop
/*
    //
    // BIOS issues ENABLE DRAM_PM - move code after SPARE2 program
    // MC0 – [0]; MC1 – [1]  BitMask - 0x03
    //
    PcodeMailboxData = (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK);
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_PM_ENABLE), PcodeMailboxData);
    if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
#ifndef HW_EMULATION
    PcodeMailboxData = 0;
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_PM_ENABLED_MASK), PcodeMailboxData);
    if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
    if ((TempData & (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) != (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) {
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "ControlDramPMThermalViaPcode: Cannot enable DRAM PM\n"));
      return FAILURE;
    }
#endif
    //
    // BIOS issues ENABLE DRAM_THERMAL - move code after CLTT enable path done
    //
    PcodeMailboxData = (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK);
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_THERMAL_ENABLE), PcodeMailboxData);
    if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
#ifndef HW_EMULATION
    PcodeMailboxData = 0;
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_THERMAL_ENABLED_MASK), PcodeMailboxData);
    if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
    if ((TempData & (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) != (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) {
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "ControlDramPMThermalViaPcode: Cannot enable DRAM Thermal\n"));
      return FAILURE;
    }
#endif
*/
  }

  for (ch = 0; ch < MAX_MC; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    dimmNvList = GetDimmNvList(host, socket, ch);

    if ((*channelNvList)[ch].encodedCSMode == 2) {
      ThreeDSPresentFlag = 1;
    }

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      if(IsLrdimmPresent(host, socket, ch, dimm)){
        LRDIMMPresentFlag = 1;
      }
    } // dimm loop
  } // ch loop

  //
  // HSD 5372718: Rasie min_ops to 0x1B0 for LRDIMM and 3DS configurations
  //
  if (ThreeDSPresentFlag | LRDIMMPresentFlag) {
    PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_MIN_OPS) | (0x1B0 << 8), 0);
    if (PcodeMailboxStatus == 0) {
      ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
    }
  }

  return SUCCESS;
}

/**

  Initialize memory throttling

  @param host  - Point to sysHost

  @retval SUCCESS

**/
UINT32
InitThrottling (
               PSYSHOST host
               )
{
  UINT8                                       B2PFlag;
  UINT8                                       i;
  UINT8                                       socket;
  UINT8                                       ch;
  UINT8                                       mcId, imcEnMask = 0;
  UINT8                                       dimm;
  UINT8                                       dimmInTTMODETable, dimmInTTTable, dimmInWTTable;
  UINT16                                      TTMRow = 0;
  UINT16                                      TTRow = 0;
  UINT16                                      WTRow = 0;
  UINT32                                      bwlimittfdata = 0;
  UINT32                                      CsrReg = 0;
  UINT8                                       n = 0;
  const struct DimmTTRowEntryStruct           *ptrDimmTT = NULL;
  struct DimmTTRowEntryStruct                 TTcurrentDimm;
  const struct DimmWTRowEntryStruct           *ptrDimmWT = NULL;
  struct DimmWTRowEntryStruct                 WTcurrentDimm;
  const struct DimmTThrottlingStruct           *ptrDimmTTMODE = NULL;
  struct DimmTThrottlingStruct                 TTMODEcurrentDimm;
  struct dimmNvram                            (*dimmNvList)[MAX_DIMM];
  struct channelNvram                         (*channelNvList)[MAX_CH];
  DramRaplDataStruct                          RAPLData;
  DramRaplLUTDataStruct                       RAPLLUTData;
  CHN_TEMP_CFG_MCDDC_CTL_STRUCT               ChnTempCfgReg;
  ET_CFG_MCDDC_CTL_STRUCT                     etCfgReg;
  PM_CMD_PWR_0_MCDDC_CTL_STRUCT               pmCmdPwrReg;
  PMSUM_DDRT_WEIGHT_MCDDC_CTL_STRUCT          pmsumDdrtWeightReg;
  MH_MAINCTL_PCU_FUN5_STRUCT                  mhMainCntl;
  MH_SENSE_500NS_PCU_FUN5_STRUCT              mhSense500nsReg;
  DIMM_TEMP_TH_0_MCDDC_CTL_STRUCT             dimmTempTh;
  DIMM_TEMP_OEM_0_MCDDC_CTL_STRUCT            dimmTempOem;
  DIMM_TEMP_EV_OFST_0_MCDDC_CTL_STRUCT        dimmTempEvOfst;
  DIMM_TEMP_THRT_LMT_0_MCDDC_CTL_STRUCT       dimmTempThrtLmt;
  PCU_BIOS_SPARE2_PCU_FUN3_STRUCT             pcuBiosSpare2Reg;
  MSC_TEMP_CRIT_CSR_FNV_DFX_MISC_0_STRUCT     fnvTempCrit;
  THERMTRIP_CONFIG_PCU_FUN2_STRUCT            ThermTripCfgReg;

  struct smbDevice  tsod;
  UINT16 mstData;
  UINT8  varThermalThrottlingOptions;
  UINT16 perChMaxMemPwr[MAX_CH] = {0};
  UINT16 perChSelfRefreshPwr[MAX_CH] = {0};

  UINT32 PcodeMailboxData = 0;
  UINT32 PcodeMailboxStatus = 0;
  UINT32 TempData = 0;


  socket = host->var.mem.currentSocket;

/* SKX TODO: Aaron fix this

//4986014 HSX Clone: Crashdump 0x91 PECI completion code after an IERR injected with a real PCIe card
  dynamicPerfPowerCtl.Data = ReadCpuPciCfgEx (host, socket, 0, DYNAMIC_PERF_POWER_CTL_PCU_FUN2_REG);
  VcnCrFeaturesCFG.Data = ReadCpuPciCfgEx (host, socket, 0, FEATURES_VCU_FUN0_REG);

  dynamicPerfPowerCtl.Bits.allow_peci_pcode_error_rsp = 1;
  WriteCpuPciCfgEx (host, socket, 0, DYNAMIC_PERF_POWER_CTL_PCU_FUN2_REG, dynamicPerfPowerCtl.Data);

  VcnCrFeaturesCFG.Bits.allow_peci_vcode_error_rsp = 1;
  WriteCpuPciCfgEx (host, socket, 0, FEATURES_VCU_FUN0_REG, VcnCrFeaturesCFG.Data);
*/

  RAPLLUTData.DramRaplDataDramPmWritePowerCoeffCval = 0;
  RAPLLUTData.DramRaplDataDramMaxPwr = 0;
  RAPLLUTData.DramRaplDataDramTdp = 0;
  RAPLLUTData.DramRaplDataDramMinPwrClosedPage = 0;
  RAPLLUTData.DramRaplDataDramWritePwrScaleClosedPage = 0;
  RAPLLUTData.DramRaplDataDramMaxPwrOpenPage = 0;
  RAPLLUTData.DramRaplDataDramTdpOpenPage = 0;
  RAPLLUTData.DramRaplDataDramMinPwrOpenPage = 0;
  RAPLLUTData.DramRaplDataDramWritePwrScaleOpenPage = 0;
  RAPLLUTData.DramRaplDataDramRefreshRateSlope = 0;
  RAPLLUTData.DramRaplDataDramSelfRefreshPower = 0;

#ifdef SERIAL_DBG_MSG
  getPrintFControl(host);
  MemDebugPrint((host, SDBG_MAX , socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "Initialize Throttling\n\n"));
#endif // SERIAL_DBG_MSG
  channelNvList = GetChannelNvList(host, socket);

  // Initialize data variables used later
  for (i = 0; i < 0xF; i++) {
    if (i < MAX_CH) {
      RAPLData.RAPLRanksPresentData[i] = 0x00;
    }
    RAPLData.RAPLWritePwrScaleSval[i] = 0x0000;
    RAPLData.RAPLDRAMPmWrPwrCOEFFValConst[i] = 0x0000;
    RAPLData.RAPLDRAMPmWrPwrCOEFFValPower[i] = 0x0000;
  } // i loop

  RAPLData.RAPL3SigmaPolicy = 0x00;
  RAPLData.RAPLMode = 0x00;
  RAPLData.RAPLDramAdjVal = 0x00;
  RAPLData.RAPLDramPwrInfoMaxVal = 0x0000;
  RAPLData.RAPLDramPwrInfoTdpVal = 0x0000;
  RAPLData.RAPLDramPwrInfoMinVal = 0x0000;
  RAPLData.RAPLWritePwrScaleMval = 0x0000;


  varThermalThrottlingOptions = host->setup.mem.thermalThrottlingOptions;
  host->nvram.mem.socket[socket].DimmWithoutThermalSensorFound = 0;

  // Flag Value (i) came from BIOS knob:
  // 0 - Disable both DRAM RAPL and DRAM Thermal Throttling
  // 1 - Enable DRAM Thermal Throttling
  // 2 - Enable DRAM RAPL
  B2PFlag = DISABLE_DRAM_MPM; //init flag value

  if (varThermalThrottlingOptions & (CLTT_EN|CLTT_PECI_EN)) {
    B2PFlag |= ENABLE_DRAM_THERMAL;
  }

  if (host->setup.mem.CmsEnableDramPm!=0) {
    B2PFlag |= ENABLE_DRAM_PM;
  }

  MemDebugPrint((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "ControlDramPMThermalViaPcode (%d)\n\n", B2PFlag));
  ControlDramPMThermalViaPcode(host, B2PFlag);

  for (mcId = 0; mcId < MAX_IMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

    //
    // Initialize MH_SENSE_500NS register to recommended values
    //
    mhSense500nsReg.Data = ReadCpuPciCfgEx (host, socket, PCU_INSTANCE_0, MH_SENSE_500NS_PCU_FUN5_REG);
    mhSense500nsReg.Bits.cnfg_500_nanosec = 50; //Floor (BCLK/2) = Floor(100/2) = 50
    WriteCpuPciCfgEx (host, socket, PCU_INSTANCE_0, MH_SENSE_500NS_PCU_FUN5_REG, mhSense500nsReg.Data);

    //
    // Configure MEMHOT
    //
    mhMainCntl.Data = ReadCpuPciCfgEx (host, socket, PCU_INSTANCE_0, MH_MAINCTL_PCU_FUN5_REG);

    if (host->setup.mem.thermalThrottlingOptions & MH_OUTPUT_EN) {
      mhMainCntl.Bits.mh_output_en = 1; //When 1, MEM_HOT output generation logic is enabled.
      for (ch = 0; ch < MAX_CH; ch++) {
       if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          dimmTempEvOfst.Data = MemReadPciCfgEp (host, socket, ch, (DIMM_TEMP_EV_OFST_0_MCDDC_CTL_REG + (dimm * 4)));

          if (host->setup.mem.memhotOutputOnlyOpt == 1) {
            dimmTempEvOfst.Bits.ev_mh_temphi_en = 1;
          } else if (host->setup.mem.memhotOutputOnlyOpt == 2) {
            dimmTempEvOfst.Bits.ev_mh_temphi_en = 1;
            dimmTempEvOfst.Bits.ev_mh_tempmid_en = 1;
          } else if (host->setup.mem.memhotOutputOnlyOpt == 3) {
            dimmTempEvOfst.Bits.ev_mh_temphi_en = 1;
            dimmTempEvOfst.Bits.ev_mh_tempmid_en = 1;
            dimmTempEvOfst.Bits.ev_mh_templo_en = 1;
          } else if (host->setup.mem.memhotOutputOnlyOpt == 0) {
            dimmTempEvOfst.Bits.ev_mh_temphi_en = 0;
            dimmTempEvOfst.Bits.ev_mh_tempmid_en = 0;
            dimmTempEvOfst.Bits.ev_mh_templo_en = 0;
          } else {
            dimmTempEvOfst.Bits.ev_mh_temphi_en = 1; //high by default
          }
          MemWritePciCfgEp (host, socket, ch, (DIMM_TEMP_EV_OFST_0_MCDDC_CTL_REG + (dimm * 4)), dimmTempEvOfst.Data);
        }//dimm
      }//ch
    } else {
      mhMainCntl.Bits.mh_output_en = 0;
    }

    if (host->setup.mem.thermalThrottlingOptions & MH_SENSE_EN) {
      mhMainCntl.Bits.mh_sense_en = 1;
    } else {
      mhMainCntl.Bits.mh_sense_en = 0;
    }
    WriteCpuPciCfgEx (host, socket, PCU_INSTANCE_0, MH_MAINCTL_PCU_FUN5_REG, mhMainCntl.Data);

    //
    // Configure MEMTRIP
    //
    ThermTripCfgReg.Data = ReadCpuPciCfgEx (host, socket, PCU_INSTANCE_0, THERMTRIP_CONFIG_PCU_FUN2_REG);

    ThermTripCfgReg.Bits.en_memtrip = 0;
    if ((varThermalThrottlingOptions & MEMTRIP_EN) && (varThermalThrottlingOptions & (OLTT_EN|CLTT_EN|CLTT_PECI_EN))){
      ThermTripCfgReg.Bits.en_memtrip = 1;
    }
    WriteCpuPciCfgEx (host, socket, PCU_INSTANCE_0, THERMTRIP_CONFIG_PCU_FUN2_REG, ThermTripCfgReg.Data);

  } // mcId

  for (ch = 0; ch < MAX_CH; ch++) {
    OutputExtendedCheckpoint((host, STS_INIT_THROTTLING, STS_RMT, (UINT16)((SUB_INIT_THROTTLING << 8)|(ch << 4)|socket)));

    mcId = GetMCID(host, socket, ch);

    ChnTempCfgReg.Data = MemReadPciCfgEp (host, socket, ch, CHN_TEMP_CFG_MCDDC_CTL_REG);

    if (varThermalThrottlingOptions & (CLTT_EN | CLTT_PECI_EN)) {
      ChnTempCfgReg.Bits.bw_limit_thrt_en = 1;
    }

    ChnTempCfgReg.Bits.cltt_debug_disable_lock = 1;
    ChnTempCfgReg.Bits.cltt_or_pcode_temp_mux_sel = 0;

    MemDebugPrintLocal((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
         ":: (*channelNvList)[%d].maxDimm = %d\n ", ch, (*channelNvList)[ch].maxDimm ));

    dimmInTTMODETable = 0;
    if (varThermalThrottlingOptions & (OLTT_EN|CLTT_EN|CLTT_PECI_EN)) {
       TTMODEcurrentDimm.TTMode = 0;
       if ((varThermalThrottlingOptions & (CLTT_EN|CLTT_PECI_EN)) && ((varThermalThrottlingOptions & OLTT_EN) == 0))  {
         TTMODEcurrentDimm.TTMode = 1;
       }

       TTMODEcurrentDimm.TTFREQ = host->nvram.mem.socket[socket].ddrFreq;
       TTMODEcurrentDimm.TTDIMMPC = (*channelNvList)[ch].maxDimm;

       if (dimmInTTMODETable == 0) {
          for (TTMRow = 0; TTMRow <= MAX_TTM_ROW; TTMRow++ ) {
            ptrDimmTTMODE = &DimmTTMODE[TTMRow];
            if ((ptrDimmTTMODE->TTMode == TTMODEcurrentDimm.TTMode) &&
                (ptrDimmTTMODE->TTDIMMPC == TTMODEcurrentDimm.TTDIMMPC) &&
                (ptrDimmTTMODE->TTFREQ == TTMODEcurrentDimm.TTFREQ)) {
              dimmInTTMODETable = 1;
              MemDebugPrintLocal((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 ":: Found TTMRow: %d,   ", TTMRow ));
              MemDebugPrintLocal((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "TTMode: %d,  TTDIMMPC: %d,  TTFREQ: %x,  TTBW_THROTTLE_TF: %x,  TTTHRT_HI: %x\n ",
                 ptrDimmTTMODE->TTMode,
                 ptrDimmTTMODE->TTDIMMPC,
                 ptrDimmTTMODE->TTFREQ,
                 ptrDimmTTMODE->TTBW_THROTTLE_TF,
                 ptrDimmTTMODE->TTTHRT_HI
               ));
              break;
            }
          } // TTMRow loop
        }
     }

    if ((*channelNvList)[ch].enabled) {

      // Override bw_limit_tf setting based on user configuration
      n = host->setup.mem.dramraplbwlimittf;     // Get user bw_limit_tf multipler
      CsrReg = (ReadCpuPciCfgEx (host, socket, 0, MC_BIOS_REQ_PCU_FUN1_REG)) & 0x0000003f;  // only preserve bits 5:0

      if ((dimmInTTMODETable == 0) && (n == 0)) {
        n = 1;
      }

      switch (CsrReg) {
        case 6:
          // DCLK = 400 MHz
          bwlimittfdata = (n * (400/8)) & 0x7FF;
          break;

        case 8:
          // DCLK = 533 MHz
          bwlimittfdata = (n * (533/8)) & 0x7FF ;
          break;

        case 10:
          // DCLK = 667 MHz
          bwlimittfdata = (n * (667/8)) & 0x7FF;
          break;

        case 12:
          // DCLK = 800 MHz
          bwlimittfdata = (n * (800/8)) & 0x7FF;
          break;

        case 14:
          // DCLK = 933.5 MHz
          bwlimittfdata = (n * (934/8)) & 0x7FF;
          break;

        case 16:
          // DCLK = 1066.5 MHz
          bwlimittfdata = (n * (1066/8)) & 0x7FF;
          break;

        case 18:
          // DCLK = 1200 MHz
          bwlimittfdata = (n * (1200/8)) & 0x7FF;
          break;

        case 20:
          // DCLK = 1333 MHz
          bwlimittfdata = (n * (1333/8)) & 0x7FF;
          break;

        case 22:
          // DCLK = 1466 MHz
          bwlimittfdata = (n * (1466/8)) & 0x7FF;
          break;

        case 24:
          // DCLK = 1600 MHz
          bwlimittfdata = (n * (1600/8)) & 0x7FF;
          break;

        default:
          bwlimittfdata = ChnTempCfgReg.Bits.bw_limit_tf; // Just use the existing value
        } // switch

      if (n > 0) {
        ChnTempCfgReg.Bits.bw_limit_tf = bwlimittfdata;
      } else {
        ChnTempCfgReg.Bits.bw_limit_tf = ptrDimmTTMODE->TTBW_THROTTLE_TF;
      }

      if ((*channelNvList)[ch].ddrtEnabled) { //if DDRT present on Ch set bit
        ChnTempCfgReg.Bits.bw_limit_tf = BW_LIMIT_TF_DDRT_DEFAULT;
      }

      MemDebugPrintLocal((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                    "\n:: ChnTempCfgReg.Bits.bw_limit_tf = 0x%04X\n ",ChnTempCfgReg.Bits.bw_limit_tf ));

      ChnTempCfgReg.Bits.oltt_en = 0;
      ChnTempCfgReg.Bits.cltt_or_pcode_temp_mux_sel = 0;
      //Enable oltt_en
      ChnTempCfgReg.Bits.oltt_en = 1;

      if (varThermalThrottlingOptions & OLTT_EN)  {
        if (dimmInTTMODETable == 1) {
          ChnTempCfgReg.Bits.thrt_ext = ptrDimmTTMODE->TTTHRT_HI; //HSD 4987488
        } else {
          ChnTempCfgReg.Bits.thrt_ext = OLTT_THRT_HI_DEFAULT;     //HSD 4987488
        }
      }

      if ((varThermalThrottlingOptions & (CLTT_EN|CLTT_PECI_EN)) && ((varThermalThrottlingOptions & OLTT_EN) == 0)) {
        if (dimmInTTMODETable == 1) {
          ChnTempCfgReg.Bits.thrt_ext = ptrDimmTTMODE->TTTHRT_HI; //HSD 4987488
        } else {
          ChnTempCfgReg.Bits.thrt_ext = CLTT_THRT_HI_DEFAULT;     //HSD 4987488
        }
      }

      if ((*channelNvList)[ch].ddrtEnabled) { //if DDRT present on Ch set bit
        ChnTempCfgReg.Bits.thrt_ext = CLTT_THRT_HI_DDRT_DEFAULT;
      }

      MemDebugPrintLocal((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
         "\n:: ChnTempCfgReg.Bits.thrt_ext = %x\n ", ChnTempCfgReg.Bits.thrt_ext));

      //
      // Clear imcM_cC_chn_temp_cfg.thrt_allow_isoch as default
      //
      ChnTempCfgReg.Bits.thrt_allow_isoch = 0;

      MemWritePciCfgEp (host, socket, ch, CHN_TEMP_CFG_MCDDC_CTL_REG, ChnTempCfgReg.Data);

      //
      // Initialize per channel electrical throttling parameters
      //
      etCfgReg.Data = MemReadPciCfgEp (host, socket, ch, ET_CFG_MCDDC_CTL_REG);
      etCfgReg.Bits.et_en = 0;
      MemWritePciCfgEp (host, socket, ch, ET_CFG_MCDDC_CTL_REG, etCfgReg.Data);
      //
      // Initialize per dimm throttling and power related registers
      //
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        //
        // set TSOD present bit if a TSOD is present on the DIMM
        //
        if ((((*dimmNvList)[dimm].dimmTs & BIT7) == 0) && varThermalThrottlingOptions & (CLTT_EN|CLTT_PECI_EN)) {
          //
          // here CLTT is enabled on input but a dimm is present with no thermal sensor...so
          // disable CLTT and print a warning message
          //
          varThermalThrottlingOptions &= ~(CLTT_EN|CLTT_PECI_EN);
          host->setup.mem.thermalThrottlingOptions = varThermalThrottlingOptions;
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                "\n:MPM: CLTT is enabled on input but a dimm is present with no thermal sensor. So, disable CLTT.\n" ));
        }

        //
        // Look up DIMM command weight value in the fixed table
        // First get the data needed to match a table entry for this dimm
        //
        dimmInWTTable = 0;

        if ((*dimmNvList)[dimm].aepDimmPresent) {
          WTcurrentDimm.WTDimmType = (0xF0 | SPD_TYPE_AEP);
        } else {
          WTcurrentDimm.WTDimmType = (*dimmNvList)[dimm].actKeyByte2;
        }

        if ((*dimmNvList)[dimm].keyByte == SPD_TYPE_DDR4) {
          if (WTcurrentDimm.WTDimmType == SPD_LRDIMM_DDR4) WTcurrentDimm.WTDimmType = SPD_LRDIMM;

          if (!((WTcurrentDimm.WTDimmType == SPD_RDIMM) ||
              (WTcurrentDimm.WTDimmType == SPD_UDIMM) ||
              (WTcurrentDimm.WTDimmType == SPD_UDIMM_ECC) ||
              (WTcurrentDimm.WTDimmType == SPD_LRDIMM))) {
            dimmInWTTable = 0xFF;
          }


          if ((WTcurrentDimm.WTDimmType == SPD_UDIMM) && ((*dimmNvList)[dimm].SPDMemBusWidth & BIT3)) {
            WTcurrentDimm.WTDimmType |= 0x10;
          }
        }

        if ((*dimmNvList)[dimm].keyByte == SPD_TYPE_DDR4) {
          WTcurrentDimm.WTLvAndDramDensity = (*dimmNvList)[dimm].sdramCapacity;
        } else {
          WTcurrentDimm.WTLvAndDramDensity = (*dimmNvList)[dimm].aepTechIndex + 5;
        }

        if ((*dimmNvList)[dimm].keyByte == SPD_TYPE_DDR4) {
          if ((WTcurrentDimm.WTLvAndDramDensity < SPD_1Gb) || (WTcurrentDimm.WTLvAndDramDensity > SPD_8Gb)) {
            dimmInWTTable = 0xFF;
          }
        }

        WTcurrentDimm.WTLvAndDramDensity |= 0x40;
        MemDebugPrintLocal((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                            "\n::MPM:: 2> dimmInWTTable = %x, WTcurrentDimm.WTLvAndDramDensity = %x,   ddrVoltage = %x\n\n", dimmInWTTable, WTcurrentDimm.WTLvAndDramDensity, host->nvram.mem.socket[socket].ddrVoltage));

        if ((*dimmNvList)[dimm].keyByte == SPD_TYPE_DDR4) {
          WTcurrentDimm.WTSpdByte7 = (*dimmNvList)[dimm].SPDModuleOrg;
        } else {
          WTcurrentDimm.WTSpdByte7 = 0;
        }

        WTcurrentDimm.WTFrequency = host->nvram.mem.socket[socket].ddrFreq;

        WTcurrentDimm.WTSignalLoading = ((*dimmNvList)[dimm].SPDDeviceType & SPD_SIGNAL_LOADING);
        WTcurrentDimm.WTDieCount = (((*dimmNvList)[dimm].SPDDeviceType & SPD_DIE_COUNT) >> 4); //DieCount value has a +1 to make it 1 based

        if (WTcurrentDimm.WTFrequency > DDR_2666) {
          dimmInWTTable = 0xFF;
        }

        if (dimm == 2) {
          WTcurrentDimm.WTDimmPos = 1;
        } else {
          WTcurrentDimm.WTDimmPos = dimm;
        }

        if (dimmInWTTable == 0) {

          MemDebugPrintLocal((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
             "\nChecking...    WTcurrentDimm.WTDimmType: %x, WTLvAndDramDensity: %x, WTSpdByte7: %x, WTDimmPos: %x, WTFrequency: %x\n\n",
             WTcurrentDimm.WTDimmType, WTcurrentDimm.WTLvAndDramDensity, WTcurrentDimm.WTSpdByte7, WTcurrentDimm.WTDimmPos, WTcurrentDimm.WTFrequency));

          //
          // DDR4 DIMM Check
          //
          if (((*dimmNvList)[dimm].keyByte == SPD_TYPE_DDR4)) {
            MemDebugPrintLocal((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n::MEM DDR4:: DDR4 Memory is detected in system. Look at DDR4 Power table now...\n"));

            for (WTRow = 0; WTRow <= (sizeof(DimmWT4)/sizeof(WTcurrentDimm)); WTRow++) {
              ptrDimmWT = &DimmWT4[WTRow];
              if ((ptrDimmWT->WTDimmType == WTcurrentDimm.WTDimmType) &&
                (ptrDimmWT->WTLvAndDramDensity == WTcurrentDimm.WTLvAndDramDensity) &&
                (ptrDimmWT->WTSpdByte7 == WTcurrentDimm.WTSpdByte7) &&
                (ptrDimmWT->WTFrequency == WTcurrentDimm.WTFrequency) &&
                (ptrDimmWT->WTDimmPos == WTcurrentDimm.WTDimmPos) &&
                (ptrDimmWT->WTSignalLoading == WTcurrentDimm.WTSignalLoading) &&
                (ptrDimmWT->WTDieCount == WTcurrentDimm.WTDieCount)) {
                dimmInWTTable = 1;
                MemDebugPrintLocal((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "\n::MEM DDR4:: DDR4 Table is available to use. dimmInWTTable = %x\n\n", dimmInWTTable));
                MemDebugPrintLocal((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "::MEM DDR4:: (%d)        %x, %x, %x, %x, %x, %x, %x, ", WTRow, ptrDimmWT->WTDimmType, ptrDimmWT->WTLvAndDramDensity, ptrDimmWT->WTSpdByte7, ptrDimmWT->WTSignalLoading, ptrDimmWT->WTDieCount, ptrDimmWT->WTDimmPos, ptrDimmWT->WTFrequency));
                MemDebugPrintLocal((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "%d,  %d,  %d,  %d,  %d,  %d,  ", ptrDimmWT->WTPmCmdPwrData, ptrDimmWT->DramPmWritePowerCoeff, ptrDimmWT->DramMaxPwr, ptrDimmWT->DramTdp, ptrDimmWT->DramMinPwrClosedPage, ptrDimmWT->DramWritePwrScaleClosedPage));
                MemDebugPrintLocal((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "%d,  %d,  %d,  %d,  %d,  %d\n\n", ptrDimmWT->DramMaxPwrOpenPage, ptrDimmWT->DramTdpOpenPage, ptrDimmWT->DramMinPwrOpenPage, ptrDimmWT->DramWritePwrScaleOpenPage, ptrDimmWT->DramRefreshRateSlope, ptrDimmWT->DramSelfRefreshPower));
                break;
              }
            } // DDR4 WTRow loop
          } else {
            MemDebugPrintLocal((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "\n::MEM NVMDIMM:: NVMDIMM Memory is detected in system. Look at NVMDIMM Power table now...\n"));
            //
            // NVDIMM Check
            //
            for (WTRow = 0; WTRow <= (sizeof(DimmWTAEP)/sizeof(WTcurrentDimm)); WTRow++ ) {
              ptrDimmWT = &DimmWTAEP[WTRow];

              if ((ptrDimmWT->WTDimmType == WTcurrentDimm.WTDimmType) &&
                (ptrDimmWT->WTLvAndDramDensity == WTcurrentDimm.WTLvAndDramDensity) &&
                (ptrDimmWT->WTDimmPos == WTcurrentDimm.WTDimmPos)) {
                dimmInWTTable = 1;
                MemDebugPrint((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "::MEM NVMDIMM:: (%d)        %x, %x, %x, %x, %x, ", WTRow, ptrDimmWT->WTDimmType, ptrDimmWT->WTLvAndDramDensity, ptrDimmWT->WTSpdByte7, ptrDimmWT->WTDimmPos, ptrDimmWT->WTFrequency));
                MemDebugPrint((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "%d,  %d,  %d,  %d,  %d,  %d,  %d,  ", ptrDimmWT->WTPmCmdPwrData, ptrDimmWT->DramPmWritePowerCoeff, ptrDimmWT->DramMaxPwr, ptrDimmWT->DramTdp, ptrDimmWT->DramMinPwrClosedPage, ptrDimmWT->DramWritePwrScaleClosedPage));
                MemDebugPrint((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "%d,  %d,  %d,  %d,  %d,  %d,  %d\n\n", ptrDimmWT->DramMaxPwrOpenPage, ptrDimmWT->DramTdpOpenPage, ptrDimmWT->DramMinPwrOpenPage, ptrDimmWT->DramWritePwrScaleOpenPage, ptrDimmWT->DramRefreshRateSlope, ptrDimmWT->DramSelfRefreshPower));
                break;
              }
            } // NVMDIMM WTRow loop
          }
        }

        pmCmdPwrReg.Data = MemReadPciCfgEp (host, socket, ch, (PM_CMD_PWR_0_MCDDC_CTL_REG + (dimm * 4)));

        if (dimmInWTTable == 1) {
          if ( ((*dimmNvList)[dimm].keyByte == SPD_TYPE_AEP) ) {
            //
            // look up PMSUM_DDRT_WEIGHT value from the fixed table
            //
            if (dimm == 0) {
              pmsumDdrtWeightReg.Bits.wr_cmd_0 = ptrDimmWT->WTPmCmdPwrData & 0xF;  // get PwrcasrDimmVal
              pmsumDdrtWeightReg.Bits.rd_cmd_0 = (ptrDimmWT->WTPmCmdPwrData >> 4) & 0xF;  // get PwrcaswDimmVal
            } else {
              pmsumDdrtWeightReg.Bits.wr_cmd_1 = ptrDimmWT->WTPmCmdPwrData & 0xF;  // get PwrcasrDimmVal
              pmsumDdrtWeightReg.Bits.rd_cmd_1 = (ptrDimmWT->WTPmCmdPwrData >> 4) & 0xF;  // get PwrcaswDimmVal
            }

            MemDebugPrint((host, SDBG_MAX , socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                            "::MPM::  get NVMDIMM table value pmsumDdrtWeightReg.Data = %x\n\n", pmsumDdrtWeightReg.Data));

            //
            // look up pmCmdPwrReg value from the fixed table
            //
            pmCmdPwrReg.Bits.active_idle_dimm = (ptrDimmWT->WTPmCmdPwrData >> 22) & 0x1F;  // get ActiveIdleDimmVal
          } else {
            //
            // look up pmCmdPwrReg value from the fixed table
            //
            pmCmdPwrReg.Data = ptrDimmWT->WTPmCmdPwrData;
          }

          MemDebugPrintLocal((host, SDBG_MAX , socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                            "::MPM::  get table value pmCmdPwrReg.Data = %x\n\n", pmCmdPwrReg.Data));
        } else {
          //
          // default DRAM Power setting
          //
          if ( ((*dimmNvList)[dimm].keyByte == SPD_TYPE_AEP) ) {
            pmCmdPwrReg.Bits.active_idle_dimm = 5;  // get ActiveIdleDimmVal

            if (dimm == 0) {
              pmsumDdrtWeightReg.Bits.wr_cmd_0 = 15;  // get PwrcasrDimmVal
              pmsumDdrtWeightReg.Bits.rd_cmd_0 = 15;  // get PwrcaswDimmVal
            } else {
              pmsumDdrtWeightReg.Bits.wr_cmd_1 = 15;  // get PwrcasrDimmVal
              pmsumDdrtWeightReg.Bits.rd_cmd_1 = 15;  // get PwrcaswDimmVal
            }
          } else {
            pmCmdPwrReg.Bits.pwrodt_cnt_dimm = 16;
            if ((*dimmNvList)[dimm].keyByte2 == SPD_UDIMM) {
              pmCmdPwrReg.Bits.active_idle_dimm = 8;
              pmCmdPwrReg.Bits.pwrref_dimm = 133;
              pmCmdPwrReg.Bits.pwract_dimm = 29;
            } else if ((*dimmNvList)[dimm].actKeyByte2 == SPD_LRDIMM) {
              pmCmdPwrReg.Bits.active_idle_dimm = 8;
              pmCmdPwrReg.Bits.pwrref_dimm = 106;
              pmCmdPwrReg.Bits.pwract_dimm = 31;
            } else {  // assume RDIMM
              pmCmdPwrReg.Bits.active_idle_dimm = 8;
              pmCmdPwrReg.Bits.pwrref_dimm = 167;
              pmCmdPwrReg.Bits.pwract_dimm = 11;
            }
            pmCmdPwrReg.Bits.pwrcasw_dimm = 15;
            pmCmdPwrReg.Bits.pwrcasr_dimm = 15;
          }
        }
        //HSD 5331227 CLONE SKX Sighting: Enabling lowest DRAM RAPL limit causes TOR TO
        pmCmdPwrReg.Bits.pwrref_dimm = 0;
        MemWritePciCfgEp (host, socket, ch, (PM_CMD_PWR_0_MCDDC_CTL_REG + (dimm * 4)), pmCmdPwrReg.Data);

        if (((*dimmNvList)[dimm].keyByte == SPD_TYPE_AEP)) {
          MemWritePciCfgEp (host, socket, ch, (PMSUM_DDRT_WEIGHT_MCDDC_CTL_REG + (dimm * 4)), pmsumDdrtWeightReg.Data);
          //
          // On any MC channel that has DDRT, set imcM_cC_chn_temp_cfg.thrt_allow_isoch=0x1
          //
          ChnTempCfgReg.Data = MemReadPciCfgEp (host, socket, ch, CHN_TEMP_CFG_MCDDC_CTL_REG);
          if (CheckSteppingLessThan(host, CPU_SKX, H0_REV_SKX)) { //HSD 5373008
            ChnTempCfgReg.Bits.thrt_allow_isoch = 1;
          } else {
            ChnTempCfgReg.Bits.thrt_allow_isoch = 0;
          }
          MemWritePciCfgEp (host, socket, ch, CHN_TEMP_CFG_MCDDC_CTL_REG, ChnTempCfgReg.Data);
        }

        //
        // Initialize socket DRAM TDP structure for use by the DRAM VR Phase Shedding initialization code
        //
        if (dimmInWTTable == 1) {
          // Read the TDP from the table
          perChMaxMemPwr[ch] = perChMaxMemPwr[ch] + (ptrDimmWT->DramMaxPwr);
          perChSelfRefreshPwr[ch] = perChSelfRefreshPwr[ch] + (ptrDimmWT->DramSelfRefreshPower);
        } else {
          //load default value
          switch ((*dimmNvList)[dimm].keyByte2) {
          case SPD_UDIMM:
            perChMaxMemPwr[ch] = perChMaxMemPwr[ch] + 126;
            perChSelfRefreshPwr[ch] = perChSelfRefreshPwr[ch] + 2679;
            break;
          case SPD_UDIMM_ECC:
            perChMaxMemPwr[ch] = perChMaxMemPwr[ch] + 114;
            perChSelfRefreshPwr[ch] = perChSelfRefreshPwr[ch] + 2679;
            break;
          case SPD_LRDIMM:
            perChMaxMemPwr[ch] = perChMaxMemPwr[ch] + 120;
            perChSelfRefreshPwr[ch] = perChSelfRefreshPwr[ch] + 2679;
            break;
          case SPD_RDIMM:
          default:
            perChMaxMemPwr[ch] = perChMaxMemPwr[ch] + 113;
            perChSelfRefreshPwr[ch] = perChSelfRefreshPwr[ch] + 1782;
          } // end switch
        }
        //
        // Init DRAM RAPL Settings here
        //
//        if (host->setup.mem.dramraplen != 0x00) {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                         "Initialize DRAM RAPL\n\n"));
          if (dimmInWTTable  != 1) {
            //
            // Initialize DRAM RAPL with defult values
            //

            if ((*dimmNvList)[dimm].keyByte2 == SPD_UDIMM) {
              RAPLLUTData.DramRaplDataDramPmWritePowerCoeffCval = 113;
              RAPLLUTData.DramRaplDataDramMaxPwr = UDIMM_MAX_POWER_DEFAULT;
              RAPLLUTData.DramRaplDataDramTdp = UDIMM_DRAM_TDP_DEFAULT;
              RAPLLUTData.DramRaplDataDramMinPwrClosedPage = UDIMM_MIN_PWR_CLOSED_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramWritePwrScaleClosedPage = UDIMM_WRITE_PWR_SCALE_CLOSED_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramMaxPwrOpenPage = UDIMM_MAX_PWR_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramTdpOpenPage = UDIMM_TDP_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramMinPwrOpenPage = UDIMM_MIN_PWR_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramWritePwrScaleOpenPage = UDIMM_WRITE_PWR_SCALE_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramRefreshRateSlope = UDIMM_REFRESH_RATE_SLOPE_DEFAULT;
              RAPLLUTData.DramRaplDataDramSelfRefreshPower = UDIMM_SELF_REFRESH_POWER_DEFAULT;
            } else if ((*dimmNvList)[dimm].actKeyByte2 == SPD_LRDIMM) {
              RAPLLUTData.DramRaplDataDramPmWritePowerCoeffCval = 336;
              RAPLLUTData.DramRaplDataDramMaxPwr = LRDIMM_MAX_POWER_DEFAULT;
              RAPLLUTData.DramRaplDataDramTdp = LRDIMM_DRAM_TDP_DEFAULT;
              RAPLLUTData.DramRaplDataDramMinPwrClosedPage = LRDIMM_MIN_PWR_CLOSED_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramWritePwrScaleClosedPage = LRDIMM_WRITE_PWR_SCALE_CLOSED_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramMaxPwrOpenPage = LRDIMM_MAX_PWR_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramTdpOpenPage = LRDIMM_TDP_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramMinPwrOpenPage = LRDIMM_MIN_PWR_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramWritePwrScaleOpenPage = LRDIMM_WRITE_PWR_SCALE_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramRefreshRateSlope = LRDIMM_REFRESH_RATE_SLOPE_DEFAULT;
              RAPLLUTData.DramRaplDataDramSelfRefreshPower = LRDIMM_SELF_REFRESH_POWER_DEFAULT;
            } else {  // assume RDIMM
              RAPLLUTData.DramRaplDataDramPmWritePowerCoeffCval = 251;
              RAPLLUTData.DramRaplDataDramMaxPwr = RDIMM_MAX_POWER_DEFAULT;
              RAPLLUTData.DramRaplDataDramTdp = RDIMM_DRAM_TDP_DEFAULT;
              RAPLLUTData.DramRaplDataDramMinPwrClosedPage = RDIMM_MIN_PWR_CLOSED_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramWritePwrScaleClosedPage = RDIMM_WRITE_PWR_SCALE_CLOSED_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramMaxPwrOpenPage = RDIMM_MAX_PWR_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramTdpOpenPage = RDIMM_TDP_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramMinPwrOpenPage = RDIMM_MIN_PWR_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramWritePwrScaleOpenPage = RDIMM_WRITE_PWR_SCALE_OPEN_PAGE_DEFAULT;
              RAPLLUTData.DramRaplDataDramRefreshRateSlope = RDIMM_REFRESH_RATE_SLOPE_DEFAULT;
              RAPLLUTData.DramRaplDataDramSelfRefreshPower = RDIMM_SELF_REFRESH_POWER_DEFAULT;
            }
          }

          if (dimmInWTTable == 1) {
            //
            // Initialize DRAM RAPL with values from lookup table
            //
            RAPLLUTData.DramRaplDataDramPmWritePowerCoeffCval = ptrDimmWT->DramPmWritePowerCoeff;
            RAPLLUTData.DramRaplDataDramMaxPwr = ptrDimmWT->DramMaxPwr;
            RAPLLUTData.DramRaplDataDramTdp = ptrDimmWT->DramTdp;
            RAPLLUTData.DramRaplDataDramMinPwrClosedPage = ptrDimmWT->DramMinPwrClosedPage;
            RAPLLUTData.DramRaplDataDramWritePwrScaleClosedPage = ptrDimmWT->DramWritePwrScaleClosedPage;
            RAPLLUTData.DramRaplDataDramMaxPwrOpenPage = ptrDimmWT->DramMaxPwrOpenPage;
            RAPLLUTData.DramRaplDataDramTdpOpenPage = ptrDimmWT->DramTdpOpenPage;
            RAPLLUTData.DramRaplDataDramMinPwrOpenPage = ptrDimmWT->DramMinPwrOpenPage;
            RAPLLUTData.DramRaplDataDramWritePwrScaleOpenPage = ptrDimmWT->DramWritePwrScaleOpenPage;
            RAPLLUTData.DramRaplDataDramRefreshRateSlope = ptrDimmWT->DramRefreshRateSlope;
            RAPLLUTData.DramRaplDataDramSelfRefreshPower = ptrDimmWT->DramSelfRefreshPower;
            MemDebugPrintLocal((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                "::MPM::  DRAM RAPL is enabled, using table value in RAPLLUTData structure.\n\n"));
          }

//APTIOV_SERVER_OVERRIDE_RC_START
          //RSD type 197 Memory Ext Info
          (*dimmNvList)[dimm].MaxTdp = RAPLLUTData.DramRaplDataDramTdp;
//APTIOV_SERVER_OVERRIDE_RC_END          
          
          // Print debug info we used to look of the 2sigma values
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "Found DIMM Type(RDIMM=1, LRDIMM=b, UDIMM=2, EccUDIMM=12) = 0x%02X\n ",WTcurrentDimm.WTDimmType ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "Found DRAM Density (Bits 7:4 = 1 LV DIMM, bits 3:0 1GB=2, 2GB=3, 4GB=4, 8GB=5) = 0x%02X\n ",WTcurrentDimm.WTLvAndDramDensity ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "#ranks/width (Bits 7:3(SR=0, DR=0x08, QR=0x18) Bis2:0(x4=0, x8=1, x16=2)) = 0x%02X\n ",WTcurrentDimm.WTSpdByte7 ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "DIMM Position (1=0, 2/3=1) = 0x%02X\n ",WTcurrentDimm.WTDimmPos ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "Frequency (800=0, 1066=1, 1333=2, 1600=3, 1866=4) = 0x%02X\n ",WTcurrentDimm.WTFrequency ));

          // Print info on rather we are using 3sigma defaults or lookup values
          if (dimmInWTTable == 1) {
            MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                                "For the Current DIMM we are using values from the 3sigma table\n "));
          } else {
            MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                                "For the Current DIMM we are using default values provided by the 3sigma table\n "));
          }

          // We are going to dump the data that is used for the DRAM RAPL calculations
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "Write Power Coeff (Constant) = %04d\n ",RAPLLUTData.DramRaplDataDramPmWritePowerCoeffCval ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "DRAM Power Info DRAMMaxPwr (closed page) = %04d\n ",RAPLLUTData.DramRaplDataDramMaxPwr ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "DRAM Power Info DRAMTDP (closed page) = %04d\n ",RAPLLUTData.DramRaplDataDramTdp ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "DRAM Power Info DRAMMinPwr (closed page) = %04d\n ",RAPLLUTData.DramRaplDataDramMinPwrClosedPage ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "Write PWR BW Scale (s va.lue) [closed page]) = %04d\n ",RAPLLUTData.DramRaplDataDramWritePwrScaleClosedPage ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "DRAM Power Info DRAMMaxPwr (open page) = %04d\n ",RAPLLUTData.DramRaplDataDramMaxPwrOpenPage ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "DRAM Power Info DRAMTDP (open page) = %04d\n ",RAPLLUTData.DramRaplDataDramTdpOpenPage ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "DRAM Power Info DRAMMinPwr (open page) = %04d\n ",RAPLLUTData.DramRaplDataDramMinPwrOpenPage ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "Write PWR BW Scale (s va.lue) [open page]) = %04d\n ",RAPLLUTData.DramRaplDataDramWritePwrScaleOpenPage ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "DRAM Refresh Rate Slope = %04d\n ",RAPLLUTData.DramRaplDataDramRefreshRateSlope ));
          MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                              "DRAM SelfRefresh Power [Need to be updated!] = %04d\n ",RAPLLUTData.DramRaplDataDramSelfRefreshPower ));

          DramRaplProgCalc(host, socket, ch, dimm, &RAPLData, &RAPLLUTData);

//        } // end DRAM RAPL Init enable

        //
        // Initialize the temp thresholds and bandwidth limits
        //
        dimmTempThrtLmt.Data = MemReadPciCfgEp (host, socket, ch, (DIMM_TEMP_THRT_LMT_0_MCDDC_CTL_REG + (dimm * 4)));
        dimmTempTh.Data =  MemReadPciCfgEp (host, socket, ch, (DIMM_TEMP_TH_0_MCDDC_CTL_REG + (dimm * 4)));
        ThermTripCfgReg.Data = ReadCpuPciCfgEx (host, socket, PCU_INSTANCE_0, THERMTRIP_CONFIG_PCU_FUN2_REG);

        if (varThermalThrottlingOptions & (OLTT_EN|CLTT_EN|CLTT_PECI_EN)) {

          if (ThermTripCfgReg.Bits.en_memtrip == 1) {
            dimmTempTh.Bits.temp_hi = TEMP_HI_EN_MEMTRIP_DEFAULT;
          } else {
            dimmTempTh.Bits.temp_hi = TEMP_HI_DIS_MEMTRIP_DEFAULT;
          }

        }

        dimmTempThrtLmt.Bits.thrt_crit = DIMMTEMP_THRT_CRIT_DEFAULT;

        if (varThermalThrottlingOptions & (CLTT_EN|CLTT_PECI_EN)) {
          //
          // set the throttling register values for CLTT based on the Romley Thermal Throttling Whitepaper rev0.95
          dimmTempThrtLmt.Bits.thrt_mid = CLTT_THRT_MID_DEFAULT;

          if (dimmInTTMODETable == 1) {
            dimmTempThrtLmt.Bits.thrt_hi = ptrDimmTTMODE->TTTHRT_HI; //HSD 4987559
          } else {
            dimmTempThrtLmt.Bits.thrt_hi = CLTT_THRT_HI_DEFAULT / (*channelNvList)[ch].maxDimm;  //devide by #DIMMs installed per channel
          }

          if (host->setup.mem.options & ALLOW2XREF_EN) {
            dimmTempTh.Bits.temp_lo = CLTT_TEMP_LO_DOUBLE_REFRESH_DEFAULT;
            dimmTempTh.Bits.temp_mid = CLTT_TEMP_MID_DOUBLE_REFRESH_DEFAULT;
#if SMCPKG_SUPPORT
            dimmTempTh.Bits.temp_lo = SMC_CLTT_TEMP_LO_DOUBLE_REFRESH_DEFAULT;
            dimmTempTh.Bits.temp_mid = SMC_CLTT_TEMP_MID_DOUBLE_REFRESH_DEFAULT;
#endif
          } else {
            dimmTempTh.Bits.temp_lo = CLTT_TEMP_LO_SINGLE_REFRESH_DEFAULT;
            dimmTempTh.Bits.temp_mid = CLTT_TEMP_MID_SINGLE_REFRESH_DEFAULT;
#if SMCPKG_SUPPORT
            dimmTempTh.Bits.temp_lo = SMC_CLTT_TEMP_LO_SINGLE_REFRESH_DEFAULT;
            dimmTempTh.Bits.temp_mid = SMC_CLTT_TEMP_MID_SINGLE_REFRESH_DEFAULT;
#endif
          }

        } else if (varThermalThrottlingOptions & OLTT_EN) {
          dimmTempThrtLmt.Bits.thrt_mid = OLTT_THRT_MID_DEFAULT;

          if (dimmInTTMODETable == 1) {
            dimmTempThrtLmt.Bits.thrt_hi = ptrDimmTTMODE->TTTHRT_HI; //HSD 4987559
          } else {
            dimmTempThrtLmt.Bits.thrt_hi = ((host->setup.mem.olttPeakBWLIMITPercent * 255) / 100) / (*channelNvList)[ch].maxDimm;  //devide by #DIMMs installed per channel
          }

          dimmTempTh.Bits.temp_lo = OLTT_TEMP_LO_DEFAULT;
          dimmTempTh.Bits.temp_mid = OLTT_TEMP_MID_DEFAULT;
        }

        if ((host->setup.mem.customRefreshRate >= 20) && (host->setup.mem.customRefreshRate <= 40)) {
          dimmTempTh.Bits.temp_lo = 0;
          dimmTempThrtLmt.Bits.thrt_mid = 0xFF;
        }

        MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
           "\n:MPM: dimmTempThrtLmt  thrt_hi:%d,  thrt_mid:%d,  thrt_crit:%d\n ", dimmTempThrtLmt.Bits.thrt_hi, dimmTempThrtLmt.Bits.thrt_mid, dimmTempThrtLmt.Bits.thrt_crit ));
        MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
           "\n:MPM: dimmTempTh  temp_hi:%d,  temp_mid:%d,  temp_lo:%d\n ", dimmTempTh.Bits.temp_hi, dimmTempTh.Bits.temp_mid, dimmTempTh.Bits.temp_lo ));

        MemWritePciCfgEp (host, socket, ch, (DIMM_TEMP_THRT_LMT_0_MCDDC_CTL_REG + (dimm * 4)), dimmTempThrtLmt.Data);
        MemWritePciCfgEp (host, socket, ch, (DIMM_TEMP_TH_0_MCDDC_CTL_REG + (dimm * 4)), dimmTempTh.Data);

        //
        // Initialize the DIMM_TEMP_OEM registers based on the v0.95 Romley Thermal Throttling whitepaper
        //
        dimmTempOem.Data = MemReadPciCfgEp (host, socket, ch, (DIMM_TEMP_OEM_0_MCDDC_CTL_REG + (dimm * 4)));
        dimmTempOem.Bits.temp_oem_hi = 0x50;
        dimmTempOem.Bits.temp_oem_lo = 0x4B;
        MemWritePciCfgEp (host, socket, ch, (DIMM_TEMP_OEM_0_MCDDC_CTL_REG + (dimm * 4)), dimmTempOem.Data);

        //
        // Look up DIMM thermal threshold offset in the fixed table
        // First get the data needed to match a table entry for this dimm
        //
        dimmInTTTable = 0;

        TTcurrentDimm.TTDimmType = (*dimmNvList)[dimm].actKeyByte2;
        if (TTcurrentDimm.TTDimmType == SPD_LRDIMM_DDR4) TTcurrentDimm.TTDimmType = SPD_LRDIMM;

        if (!((TTcurrentDimm.TTDimmType == SPD_RDIMM) ||
              (TTcurrentDimm.TTDimmType == SPD_LRDIMM))) {
          TTcurrentDimm.TTDimmType = SPD_RDIMM;
        }

        TTcurrentDimm.TTDramDensity = ((*dimmNvList)[dimm].actSPDSDRAMBanks & 0x0F);
        // Substitute 16 Gb for anything other than 16 Gb
        if (TTcurrentDimm.TTDramDensity != SPD_16Gb) TTcurrentDimm.TTDramDensity = SPD_16Gb;

        TTcurrentDimm.TTSpdByte7 = (*dimmNvList)[dimm].actSPDModuleOrg;

        // For x16, substitute x8
        if ((TTcurrentDimm.TTSpdByte7 & 0x7) == DEVICE_WIDTH_X16) {
          TTcurrentDimm.TTSpdByte7 &= 0xF8;
          TTcurrentDimm.TTSpdByte7 |= DEVICE_WIDTH_X8;
        }

        if ((((TTcurrentDimm.TTSpdByte7 & 0x7) > DEVICE_WIDTH_X8) ||
             ((TTcurrentDimm.TTSpdByte7 & 0x38) > (SPD_NUM_RANKS_8 << 3))) && (TTcurrentDimm.TTDimmType == SPD_LRDIMM_DDR4)) {
          dimmInTTTable = 0xFF;
        }

        TTcurrentDimm.TTFrequency = host->nvram.mem.socket[socket].ddrFreq;
        if (TTcurrentDimm.TTFrequency < DDR_1600) TTcurrentDimm.TTFrequency = DDR_1600;
        if (TTcurrentDimm.TTFrequency > DDR_2933) TTcurrentDimm.TTFrequency = DDR_2933;

        TTcurrentDimm.TTDPC = dimm + 1;

        // UDIMM substitute RDIMM
        if ( (TTcurrentDimm.TTDimmType == SPD_UDIMM) || (TTcurrentDimm.TTDimmType == SPD_UDIMM_ECC) ) {
          TTcurrentDimm.TTDimmType = SPD_RDIMM;
        }

        if (dimmInTTTable == 0) {
          for (TTRow = 0; TTRow <= MAX_TT_ROW; TTRow++ ) {
            ptrDimmTT = &DimmTT[TTRow];
            if ((ptrDimmTT->TTDimmType == TTcurrentDimm.TTDimmType) &&
                (ptrDimmTT->TTDramDensity == TTcurrentDimm.TTDramDensity) &&
                (ptrDimmTT->TTSpdByte7 == TTcurrentDimm.TTSpdByte7) &&
                (ptrDimmTT->TTFrequency == TTcurrentDimm.TTFrequency) &&
                (ptrDimmTT->TTDPC == TTcurrentDimm.TTDPC)) {
              dimmInTTTable = 1;
              MemDebugPrintLocal((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "\nMPM: Found TTStaticData = 0x%x, TTPower = 0x%x in Table DimmTT Row (%d)\n", ptrDimmTT->TTStaticData, ptrDimmTT->TTPower, TTRow));
              break;
            }
          } // TTRow loop
        }

        dimmTempEvOfst.Data = MemReadPciCfgEp (host, socket, ch, (DIMM_TEMP_EV_OFST_0_MCDDC_CTL_REG + (dimm * 4)));

        if (dimmInTTTable == 1) {
          dimmTempEvOfst.Bits.dimm_temp_offset = ptrDimmTT->TTStaticData;
        } else {
          dimmTempEvOfst.Bits.dimm_temp_offset = TT_MAX_WORST_CASE_DIMM_TEMP_OFFSET;
        }

        dimmTempEvOfst.Bits.ev_2x_ref_templo_en = 1;

        MemWritePciCfgEp (host, socket, ch, (DIMM_TEMP_EV_OFST_0_MCDDC_CTL_REG + (dimm * 4)), dimmTempEvOfst.Data);
        //
        // if CLTT is enabled, initialize the TSOD device
        //
        if (varThermalThrottlingOptions & CLTT_EN) {

          //
          // Initialize Tcrit to 105 - temp offset
          //
          if ((*dimmNvList)[dimm].aepDimmPresent) {
            fnvTempCrit.Data = 0;
            fnvTempCrit.Bits.tempcrit = (((105 - (UINT16)dimmTempEvOfst.Bits.dimm_temp_offset) << 4) & 0x0FFF);
            WriteFnvCfg(host, socket, ch, dimm, MSC_TEMP_CRIT_CSR_FNV_DFX_MISC_0_REG, fnvTempCrit.Data);
          } else {

            tsod.compId = MTS;
            tsod.address.controller = PLATFORM_SMBUS_CONTROLLER_PROCESSOR;
            tsod.address.deviceType = DTI_TSOD;
            GetSmbAddress(host, socket, ch, dimm, &tsod);

#ifdef SERIAL_DBG_MSG
            ReadSmb (host, socket, tsod, MTS_MFGID, (UINT8 *)&mstData);
            ReadSmb (host, socket, tsod, MTS_DID_RID, (UINT8 *)&mstData);
            ReadSmb (host, socket, tsod, MTS_CAPABILITY, (UINT8 *)&mstData);
            ReadSmb (host, socket, tsod, MTS_TEMP, (UINT8 *)&mstData);
#endif
            mstData = (((105 - (UINT16)dimmTempEvOfst.Bits.dimm_temp_offset) << 4) & 0x0FFF);
            WriteSmb (host, socket, tsod, MTS_CRITICAL, (UINT8 *)&mstData);

            //
            // Initialize the config register
            //
            mstData = OEM_MTS_CONFIG_VALUE;
            WriteSmb (host, socket, tsod, MTS_CONFIG, (UINT8 *)&mstData);
            // Added OEM hook for customer to do override
#if !defined(SIM_BUILD) && !defined(MINIBIOS_BUILD)
            PlatformHookMst (host, socket, tsod, MTS_CONFIG, (UINT8 *)&mstData);
#endif
          }
        }
      } // dimm loop
    } // ch enabled
  } // ch loop
  //
  // Program DRAM RAPL parameters host->setup.mem.dramraplen == 00 - dram rapl disabled, 01 - set DRAM rapl mode 0, 02 - set DRAM rapl mode 1, else invalid
  //
  RAPLData.RAPLMode = 1;
  DramRaplProg (host, socket, &RAPLData);

  pcuBiosSpare2Reg.Data = ReadCpuPciCfgEx (host, socket, 0, PCU_BIOS_SPARE2_PCU_FUN3_REG);
  pcuBiosSpare2Reg.Data = pcuBiosSpare2Reg.Data & ~(BIT15 + BIT14 + BIT13 + BIT12);
  if (host->nvram.mem.dimmTypePresent == UDIMM) {
    pcuBiosSpare2Reg.Data = pcuBiosSpare2Reg.Data | PCU_BIOS_SPARE2_UDIMM;
  } else if ((host->nvram.mem.dimmTypePresent == RDIMM) && (host->nvram.mem.socket[socket].lrDimmPresent == 1)) {
    pcuBiosSpare2Reg.Data =  pcuBiosSpare2Reg.Data | PCU_BIOS_SPARE2_LRDIMM;
  } else {
    // default to RDIMM
    pcuBiosSpare2Reg.Data =  pcuBiosSpare2Reg.Data | PCU_BIOS_SPARE2_RDIMM;
  }

  pcuBiosSpare2Reg.Data =  pcuBiosSpare2Reg.Data | PCU_BIOS_SPARE2_125V_DIMM;

  MemDebugPrintLocal((host, SDBG_MAX , NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "\n::MPM:: host->nvram.mem.dimmTypePresent = %x,    pcuBiosSpare2Reg.Data = %x\n\n", host->nvram.mem.dimmTypePresent, pcuBiosSpare2Reg.Data));

  WriteCpuPciCfgEx (host, socket, 0, PCU_BIOS_SPARE2_PCU_FUN3_REG, pcuBiosSpare2Reg.Data);

  //Set MC Bit Mask to indicate populated MCs on Skt
  for (mcId = 0; mcId < MAX_IMC; mcId++) {
    if(host->var.mem.socket[socket].imcEnabled[mcId] == 1) {
      imcEnMask |= (1 << mcId);
    }
  }

  if (B2PFlag & ENABLE_DRAM_PM)
  {
      //
      // BIOS issues ENABLE DRAM_PM
      // MC0 – [0]; MC1 – [1]  BitMask - 0x03
      //
      PcodeMailboxData = (UINT32)imcEnMask;
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_PM_ENABLE), PcodeMailboxData);
      if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
      PcodeMailboxData = 0;
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_PM_ENABLED_MASK), PcodeMailboxData);
      if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
      if ((TempData & (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) != (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) {
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "BIOS failed to issue B2P ENABLE_DRAM_PM, return data = 0x%x\n", TempData));
      }
  }

  if (varThermalThrottlingOptions & CLTT_EN)
  {
      //
      // BIOS issues ENABLE DRAM_THERMAL
      //
      PcodeMailboxData = (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK);
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\nCLTT, write 0x%x to B2P DRAM_THERMAL_ENABLE.\n", TempData));
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_THERMAL_ENABLE), PcodeMailboxData);
      if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
      PcodeMailboxData = 0;
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_THERMAL_ENABLED_MASK), PcodeMailboxData);
      if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
      if ((TempData & (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) != (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) {
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "BIOS failed to issue B2P DRAM_THERMAL_ENABLE for CLTT return data = 0x%x\n", TempData));
      }
  }

  if (varThermalThrottlingOptions & CLTT_PECI_EN)
  {
      //
      // BIOS issues ENABLE DRAM_THERMAL for CLTT w/ PECI
      //
      PcodeMailboxData = ((MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK) << 8);
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_THERMAL_ENABLE), PcodeMailboxData);
      MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\nCLTT w/PECI, write 0x%x to B2P DRAM_THERMAL_ENABLE.\n", TempData));
      if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
      PcodeMailboxData = 0;
      PcodeMailboxStatus = WriteBios2PcuMailboxCommand (host, socket, ((UINT32)MAILBOX_BIOS_CMD_DRAM_THERMAL_ENABLED_MASK), PcodeMailboxData);
      if (PcodeMailboxStatus == 0) TempData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
      if (( (TempData >> 8) & (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) != (MAILBOX_BIOS_MC_0_MASK | MAILBOX_BIOS_MC_1_MASK)) {
        MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "BIOS failed to issue B2P DRAM_THERMAL_ENABLE for CLTT w/ PECI, return data = 0x%x\n", TempData));
      }
  }

#ifdef SERIAL_DBG_MSG
  releasePrintFControl (host);
#endif // SERIAL_DBG_MSG
  return SUCCESS;
}

/**

  This outine will progam registers for Initializing DRAM RAPL

  @param host          - Point to sysHost
  @param socket          - Socket number
  @param pointer DRamRaplDataStruct  - Strucutre containing the values to be should be Programmed

  @retval SUCCESS

**/
static UINT32
DramRaplProg (
             PSYSHOST host,
             UINT8    socket,
             DramRaplDataStruct *RAPLData
             )
{

  UINT8                               ch = 0;
  UINT8                               dimm = 0;
  UINT8 numDimmsPerSocket = 0;
  UINT8                               RAPLDimmIndex = 0;
  UINT8 DimmTableIndex = 0;
  UINT32                              PcodeMailboxStatus = 0;
  DRAM_POWER_INFO_N0_PCU_FUN2_STRUCT  dramPowerInfoLow;
  DRAM_POWER_INFO_N1_PCU_FUN2_STRUCT  dramPowerInfoHigh;
  CMD_WRITE_PWR_BW_SCALE_DATA         BwScaleData;
  CMD_DRAM_PM_WRITE_PWR_COEFF         WritePwrCoeff;
  CMD_DRAM_PM_RAPL_MODE               RaplMode;
  CMD_WRITE_DRAM_PBM_THRT_ADJUST_DATA PbmthrtAdjData;
  struct dimmNvram                    (*dimmNvList)[MAX_DIMM];
  struct channelNvram                 (*channelNvList)[MAX_CH];
  struct socketNvram                  *socketNv;

  channelNvList = GetChannelNvList(host, socket);
  socketNv = &host->nvram.mem.socket[socket];

  // Set DDR Ranks Present
  for (ch = 0; ch < MAX_CH; ch++) {

    if ((*channelNvList)[ch].enabled == 0) continue;

//if DIMMTYPE=DDR4,
//        DRAM_VR_MC_MAPPING (0xA6)
//        DRAM_DIMM_VR_MAPPING (0xA4)- Mask of dimms attached to VR [17:16]
//
//if DIMMTYPE=NVMDIMM,
//        DRAM_VR_SENSOR_MAPPING  (0x97)
//        DRAM_DIMM_VR_MAPPING (0xA4) - Mask of dimms attached to 12V [25:24]

    // Set DDR_PM_RAPL_MODE
    RaplMode.Data = 0x00000000;
    RaplMode.Bits.raplmode = RAPLData->RAPLMode;
    PcodeMailboxStatus = (UINT8) WriteBios2PcuMailboxCommand (host, socket, MAILBOX_BIOS_CMD_DRAM_RAPL_MODE, RaplMode.Data);
    // Added PcodeMailboxStatus usage
    if (PcodeMailboxStatus == 0) ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);
    MemDebugPrintLocal((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "DRAM RAPL DEBUG: Issued DRAM RAPL MBX CMD DRAM_RAPL_MODE with CMD data = 0x%08x\n ", RaplMode.Data ));
    MemDebugPrintLocal((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "DRAM RAPL DEBUG: DRAM RAPL MXB DRAM_RAPL_MODE CMD status = 0x%02X\n ",PcodeMailboxStatus ));

    // ....Program BW power Scale factor (s)
    BwScaleData.Data = 0x00000000;
    BwScaleData.Bits.channelindex = ch;
    BwScaleData.Bits.selector = 0;            // indicate scale factor being programmed
    BwScaleData.Bits.cmdval = RAPLData->RAPLWritePwrScaleSval[ch];
    WriteBios2PcuMailboxCommand (host, socket, MAILBOX_BIOS_CMD_WRITE_PWR_BW_SCALE, BwScaleData.Data);

    // ....Program BW power Scale Minimum Power (m)
    BwScaleData.Data = 0x00000000;
    BwScaleData.Bits.channelindex = ch;
    BwScaleData.Bits.selector = 1;            // indicate minimum power being programmed
    BwScaleData.Bits.cmdval = RAPLData->RAPLWritePwrScaleMval;
    WriteBios2PcuMailboxCommand (host, socket, (UINT32) MAILBOX_BIOS_CMD_WRITE_PWR_BW_SCALE, BwScaleData.Data);

    //
    // Use MAILBOX_BIOS_WRITE_DRAM_PBM_THRT_ADJUST command code (0x8B)
    //
    PbmthrtAdjData.Data = 0x00000000;
    PbmthrtAdjData.Bits.channelindex = ch;
    PbmthrtAdjData.Bits.adjustFactor = RAPLData->RAPLDramAdjVal;
    WriteBios2PcuMailboxCommand (host, socket, (UINT32) MAILBOX_BIOS_WRITE_DRAM_PBM_THRT_ADJUST, PbmthrtAdjData.Data);

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      numDimmsPerSocket++;

      // Calculate DIMMindex based on CH & DIMM - 0=ch0:dimm0, 1=ch0:dimm1, 2=ch0:dimm2, 3=ch1:dimm0, ..., 6=ch2:dimm0,..., 9=ch3:dimm0, ..., 0xc=ch0:dimm3, 0xb=ch1:dimm3....
      RAPLDimmIndex = dimm + (ch * MAX_DIMM);

      DimmTableIndex = RAPLDimmIndex;

      // Set DDR_PM_WRITE_POWER_COEFF (0x86)- Conversion factor
      WritePwrCoeff.Data = 0x00000000;
      WritePwrCoeff.Bits.selector = 1;        // Indicate we providing a constant (i.e. Conversion factor)
      WritePwrCoeff.Bits.dimmindex = RAPLDimmIndex;
      WritePwrCoeff.Bits.datavalue = RAPLData->RAPLDRAMPmWrPwrCOEFFValConst[DimmTableIndex];
      WriteBios2PcuMailboxCommand (host, socket, MAILBOX_BIOS_CMD_DRAM_PM_WRITE_POWER_COEFF, WritePwrCoeff.Data);
      MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
          "Issued DRAM RAPL MBX CMD DRAM_PM_WRITE_POWER_COEFF (Constant) with CMD data = 0x%08x\n ", WritePwrCoeff.Data ));

      // Set DDR_PM_WRITE_POWER_COEFF - Idle Power
      WritePwrCoeff.Data = 0x00000000;
      WritePwrCoeff.Bits.selector = 0;        // Idle Power
      WritePwrCoeff.Bits.dimmindex = RAPLDimmIndex;
      WritePwrCoeff.Bits.datavalue = RAPLData->RAPLDRAMPmWrPwrCOEFFValPower[DimmTableIndex];
      WriteBios2PcuMailboxCommand (host, socket, MAILBOX_BIOS_CMD_DRAM_PM_WRITE_POWER_COEFF, WritePwrCoeff.Data);
      MemDebugPrintLocal((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
          "Issued DRAM RAPL MBX CMD DRAM_PM_WRITE_POWER_COEFF (Idel Power) with CMD data = 0x%08x\n ", WritePwrCoeff.Data ));
    }
  } //ch

  // Program only if socket is enabled
  if (socketNv->enabled) {

    // Set DDR_PM_WRITE_POWER_COEFF - Idle Power

    //
    // Program DRAM_POWER_INFO
    //
    //if (numDimmsPerSocket == 1) RAPLData->RAPLDramPwrInfoMinVal = ((RAPLData->RAPLDramPwrInfoMinVal / 2) * 3);

    if (host->setup.mem.DramRaplExtendedRange) {
      RAPLData->RAPLDramPwrInfoMinVal = RAPLData->RAPLDramPwrInfoMinVal / 2;
    }

    //
    //... Program lower dword
    //
    dramPowerInfoLow.Data = ReadCpuPciCfgEx (host, socket, 0, DRAM_POWER_INFO_N0_PCU_FUN2_REG); // TODO: How DRAM power info is handled for 2 controllers; update if needed
    dramPowerInfoLow.Bits.dram_tdp = RAPLData->RAPLDramPwrInfoTdpVal;
    dramPowerInfoLow.Bits.dram_min_pwr = (RAPLData->RAPLDramPwrInfoMinVal / 10);
    WriteCpuPciCfgEx (host, socket, 0, DRAM_POWER_INFO_N0_PCU_FUN2_REG, dramPowerInfoLow.Data); // TODO: How DRAM power info is handled for 2 controllers; update if needed

    //
    //... Program upper dword
    //
    dramPowerInfoHigh.Data = ReadCpuPciCfgEx (host, socket, 0, DRAM_POWER_INFO_N1_PCU_FUN2_REG); // TODO: How DRAM power info is handled for 2 controllers; update if needed
    dramPowerInfoHigh.Bits.dram_max_pwr = RAPLData->RAPLDramPwrInfoMaxVal;
    WriteCpuPciCfgEx (host, socket, 0, DRAM_POWER_INFO_N1_PCU_FUN2_REG, dramPowerInfoHigh.Data); // TODO: How DRAM power info is handled for 2 controllers; update if needed
  }


  return SUCCESS;
}

/**

  This outine will calculate the values to be programmed for DRAM RAPL based upon the looked up values

  @param host          - Point to sysHost
  @param socket          - Socket number
  @param ch          - channel
  @param dimm          - dimm number
  @param pointer DRamRaplDataStruct  - Strucutre containing the values to be should be Programmed
  @param pointer DramRaplLUTData     - Strucutre containing looked up which the calculations will be based upon

  @retval SUCCESS

**/
static VOID
DramRaplProgCalc (
                 PSYSHOST host,
                 UINT8    socket,
                 UINT8    ch,
                 UINT8    dimm,
                 DramRaplDataStruct *RAPLData,
                 DramRaplLUTDataStruct *RAPLLUTData
                 )
{
  UINT8                           RankPresentShift = 0x00;
  UINT16                          RankPresentCalc = 0x0000;
  UINT8                           RAPLDimmIndex = 0;
  UINT32                          x = 0;

  TCMR0SHADOW_MCDDC_CTL_STRUCT    mr0shadow;
  MCMTR_MC_MAIN_STRUCT            mcMtr;
  CHN_TEMP_CFG_MCDDC_CTL_STRUCT   ChnTempCfgReg;
  struct dimmNvram                            (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);
  ChnTempCfgReg.Data = MemReadPciCfgEp (host, socket, ch, CHN_TEMP_CFG_MCDDC_CTL_REG);

  // ...calculate DIMM index to be used for Bios2PcuMailboxCommands
  // Calculate DIMMindex based on CH & DIMM - 0=ch0:dimm0, 1=ch0:dimm1, 2=ch0:dimm2, 3=ch1:dimm0, ..., 6=ch2:dimm0,..., 9=ch3:dimm0, ..., 0xc=ch0:dimm3, 0xb=ch1:dimm3....
  RAPLDimmIndex = dimm + (ch * MAX_DIMM);

  // ...calculate DDR Ranks Present Data for Bios2PcuMailboxCommands
  if (dimm == 0) {
    RankPresentShift = 0x0;
  } else {
    RankPresentShift = 1 << (dimm + 1);
  }
  x = (*dimmNvList)[dimm].numDramRanks;

  if (x > 0) {
    RankPresentCalc = (UINT16) (((x * 2) * (x * 2)) - 1);
  } else {
    RankPresentCalc = 0x1;
  }

  RAPLData->RAPLRanksPresentData[ch] |= (RankPresentCalc << RankPresentShift);

  // ... Calculate CKE policy mode

  //
  // Shadow MR0 but do not set DLL_RESET
  mr0shadow.Data = MemReadPciCfgEp (host, socket, ch, TCMR0SHADOW_MCDDC_CTL_REG);
  if (mr0shadow.Bits.mr0_shadow & BIT12) {
    RAPLData->RAPL3SigmaPolicy &= ~BIT0;  // for debug purposes weare going to force CKE policy to fast for now
  } else {
    RAPLData->RAPL3SigmaPolicy |= BIT0;   // Indicate CKE slow
  }

  // ... Set Page mode policy
  mcMtr.Data = MemReadPciCfgMC (host, socket, GetMCID(host, socket, ch), MCMTR_MC_MAIN_REG);
  if (mcMtr.Bits.close_pg == 1) {
    RAPLData->RAPL3SigmaPolicy|= BIT1;    // Indicate close page (i.e. BIT1=1)
  } else {
    RAPLData->RAPL3SigmaPolicy &= ~BIT1;    // Indicate open page (i.e. BIT1=0)
  }

  // Set DRAM Adj Value
  RAPLData->RAPLDramAdjVal = 0x80;

  // Set Write Power Scale (m) value
  RAPLData->RAPLWritePwrScaleMval = 0x01;

  //
  // Initialize DRAM RAPL with values from lookup table
  //

  // Get DramMaxPwr & DramTdp based upon page mode policy open/closed
  if (RAPLData->RAPL3SigmaPolicy & BIT1) {
    // Init DramMaxPwr
    RAPLData->RAPLDramPwrInfoMaxVal = RAPLData->RAPLDramPwrInfoMaxVal + RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramMaxPwr);
    // Init DramTdp
    RAPLData->RAPLDramPwrInfoTdpVal = RAPLData->RAPLDramPwrInfoTdpVal + RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramTdp);
  } else {
    // Init DramMaxPwr
    RAPLData->RAPLDramPwrInfoMaxVal = RAPLData->RAPLDramPwrInfoMaxVal + RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramMaxPwrOpenPage);
    // Init DramTdp
    RAPLData->RAPLDramPwrInfoTdpVal = RAPLData->RAPLDramPwrInfoTdpVal + RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramTdpOpenPage);
  }

  // Get Min pwr based upon page... NOTE: the end result is cumlative for all the DIMMS in a channel
  if (RAPLData->RAPL3SigmaPolicy & BIT1) {
    if (ChnTempCfgReg.Bits.thrt_allow_isoch == 1) {
      if ((*dimmNvList)[dimm].keyByte == SPD_TYPE_DDR4) {
        RAPLData->RAPLDramPwrInfoMinVal = RAPLData->RAPLDramPwrInfoMinVal + (RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramMinPwrClosedPage) * 13);
      } else {
        RAPLData->RAPLDramPwrInfoMinVal = RAPLData->RAPLDramPwrInfoMinVal + 24 + (RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramMinPwrClosedPage) * 12);
      }
    } else {
      RAPLData->RAPLDramPwrInfoMinVal = RAPLData->RAPLDramPwrInfoMinVal + (RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramMinPwrClosedPage) * 10);
    }
  } else {
    if (ChnTempCfgReg.Bits.thrt_allow_isoch == 1) {
      if ((*dimmNvList)[dimm].keyByte == SPD_TYPE_DDR4) {
        RAPLData->RAPLDramPwrInfoMinVal = RAPLData->RAPLDramPwrInfoMinVal + (RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramMinPwrOpenPage) * 13);
      } else {
        RAPLData->RAPLDramPwrInfoMinVal = RAPLData->RAPLDramPwrInfoMinVal + 24 + (RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramMinPwrOpenPage) * 12);
      }
    } else {
      RAPLData->RAPLDramPwrInfoMinVal = RAPLData->RAPLDramPwrInfoMinVal + (RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramMinPwrOpenPage) * 10);
    }
  }

  // Get power scale (Write Power Scale (S) val) value based upon page mode
  if (RAPLData->RAPL3SigmaPolicy & BIT1) {
    RAPLData->RAPLWritePwrScaleSval[ch] = RAPLLUTData->DramRaplDataDramWritePwrScaleClosedPage;
  } else {
    RAPLData->RAPLWritePwrScaleSval[ch] = RAPLLUTData->DramRaplDataDramWritePwrScaleOpenPage;
  }

  // Get DDR_PM_WRITE_POWER_COEFF Constant value
  RAPLData->RAPLDRAMPmWrPwrCOEFFValConst[RAPLDimmIndex] = RAPLLUTData->DramRaplDataDramPmWritePowerCoeffCval;

  // Calculate DDR_PM_WRITE_POWER_COEFF min Idle power value based on page policy
  if (RAPLData->RAPL3SigmaPolicy & BIT1) {
    RAPLData->RAPLDRAMPmWrPwrCOEFFValPower[RAPLDimmIndex] = RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramMinPwrClosedPage);
  } else {
    RAPLData->RAPLDRAMPmWrPwrCOEFFValPower[RAPLDimmIndex] = RefreshScaledPwr(host, socket, RAPLLUTData->DramRaplDataDramRefreshRateSlope, RAPLLUTData->DramRaplDataDramMinPwrOpenPage);
  }
}

/**

  This outine will scale the calculated power values to be programmed for DRAM RAPL based upon the looked up values
  based on the refresh rate and a linear scaling factor from looked up values.

  @param host          - Point to sysHost
  @param powerInput    - Unscaled power value

  @retval Scaled power value

**/
static UINT16
RefreshScaledPwr(
                PSYSHOST host,
                UINT8    socket,
                UINT32   slope,
                UINT16   powerInput
                )
{
  UINT16 scaledPower = 0;
  UINT32 RefreshRate;
  UINT32 RefreshAdder = 0;

  //
  // If refresh rate is >1x, then scale DRAM power calculations
  //
  RefreshRate = (UINT32) host->nvram.mem.socket[socket].refreshRate;

  if ((host->setup.mem.dramraplRefreshBase >= 10) && (RefreshRate > host->setup.mem.dramraplRefreshBase) ) {
    RefreshAdder = (((UINT32)slope * (RefreshRate - host->setup.mem.dramraplRefreshBase)) / 10000);
  }
  scaledPower = powerInput + (UINT16)RefreshAdder;

  return scaledPower;
}

void
EnableTsod (
            PSYSHOST host,
            UINT8    socket
)
{
  UINT8                                      mcId;
  SMB_TSOD_POLL_RATE_CFG_0_PCU_FUN5_STRUCT   smbTsodPollRate;
  SMB_CMD_CFG_0_PCU_FUN5_STRUCT              smbCmd;
  UINT8                                      FPGALoadAttempted;

  smbTsodPollRate.Data = MemReadPciCfgEp (host, socket, PCU_INSTANCE_0, SMB_TSOD_POLL_RATE_CFG_0_PCU_FUN5_REG);
  smbTsodPollRate.Bits.smb_tsod_poll_rate = 0x3F7A0;
  //
  // Initialize the Smbus TSOD Polling rate to 10ms
  //
  MemWritePciCfgEp (host, socket, PCU_INSTANCE_0, SMB_TSOD_POLL_RATE_CFG_0_PCU_FUN5_REG, smbTsodPollRate.Data);

  if (host->nvram.mem.socket[socket].mcpPresent == 1) {
    MemWritePciCfgEp (host, socket, PCU_INSTANCE_0, SMB_TSOD_POLL_RATE_CFG_1_PCU_FUN5_REG, smbTsodPollRate.Data);
  }

  smbCmd.Data = 0;
  FPGALoadAttempted = (UINT8)(BIT5 & ReadCpuPciCfgEx (host, host->nvram.common.sbspSocketId, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG));

  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;
    //
    // configure the SMBCNTL register TSOD present bits
    //

    if (mcId == 0) {
      smbCmd.Data = ReadCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_CMD_CFG_0_PCU_FUN5_REG);
    } else {
      smbCmd.Data = ReadCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_CMD_CFG_1_PCU_FUN5_REG);
    }

    if (host->setup.mem.thermalThrottlingOptions & CLTT_EN) {
      if (host->var.kti.OutKtiFpgaEnable[socket] == 1 && FPGALoadAttempted == 0){
        smbCmd.Bits.smb_tsod_poll_en = 0;
      } else {
        smbCmd.Bits.smb_tsod_poll_en = 1;
      }
      smbCmd.Bits.smb_dti = DTI_TSOD;
    }
    UpdateTsodPopulation(host, socket, mcId);

    if (mcId == 0) {
      WriteCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_CMD_CFG_0_PCU_FUN5_REG, smbCmd.Data);
    } else {
      WriteCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_CMD_CFG_1_PCU_FUN5_REG, smbCmd.Data);
    }
  }

  if (host->nvram.mem.socket[socket].mcpPresent == 1) {
    if (CheckSteppingLessThan(host, CPU_SKX, B0_REV_SKX)) {
      WriteCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_CMD_CFG_2_PCU_FUN5_REG, smbCmd.Data);
    } else {
      // Pcode
    }
  }
}

static void
UpdateTsodPopulation (
                      PSYSHOST host,
                      UINT8    socket,
                      UINT8    mcId
)
{
  UINT8                                 ch;
  UINT8                                 dimm;
  UINT8                                 chEnable[MAX_CH];
  struct channelNvram                   (*channelNvList)[MAX_CH];
  struct dimmNvram                      (*dimmNvList)[MAX_DIMM];
  SMB_TSOD_CONFIG_CFG_0_PCU_FUN5_STRUCT smbCntl;

  channelNvList = GetChannelNvList(host, socket);

  if (mcId == 0) {
    smbCntl.Data = ReadCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_TSOD_CONFIG_CFG_0_PCU_FUN5_REG);
  } else {
    smbCntl.Data = ReadCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_TSOD_CONFIG_CFG_1_PCU_FUN5_REG);
  }

  //
  // Need to enable TSOD polling for AEP dimms even if the channel has been disabled
  //
  for (ch = 0; ch < MAX_MC_CH; ch++) {
    chEnable[ch + (MAX_MC_CH * mcId)] = 0;
    if ((*channelNvList)[ch + (MAX_MC_CH * mcId)].enabled == 1) {
      chEnable[ch + (MAX_MC_CH * mcId)] = 1;
      continue;
    } else {
      dimmNvList = GetDimmNvList(host, socket, ch + (MAX_MC_CH * mcId));
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].aepDimmPresent) {
          chEnable[ch + (MAX_MC_CH * mcId)] = 1;
          break;
        }
      } // dimm loop
    }
  } // ch loop

  for (ch = 0; ch < MAX_MC_CH; ch++) {
    if (chEnable[ch + (MAX_MC_CH * mcId)] == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch + (MAX_MC_CH * mcId));
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if (((*dimmNvList)[dimm].dimmPresent == 0) && ((*dimmNvList)[dimm].aepDimmPresent == 0)) continue;

      smbCntl.Bits.group0_dimm_present |= (1 << (((ch % MAX_MC_CH) * 2) + dimm));
    } // dimm loop
  } // ch loop

  if (mcId == 0) {
    WriteCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_TSOD_CONFIG_CFG_0_PCU_FUN5_REG, smbCntl.Data);
  } else {
    WriteCpuPciCfgEx (host, socket, PCU_INSTANCE_0, SMB_TSOD_CONFIG_CFG_1_PCU_FUN5_REG, smbCntl.Data);
  }
}
