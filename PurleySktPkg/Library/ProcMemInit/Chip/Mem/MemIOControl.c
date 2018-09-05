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
 * Copyright 2006 - 2017 Intel Corporation All Rights Reserved.
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

#include "SysFunc.h"
#include "SysFuncChip.h"
#include "SysHostChip.h"
#include "MemFuncChip.h"
#include "CpuPciAccess.h"
#include "FnvAccess.h"
#include "MemProjectSpecific.h"
#include "MemHostChip.h"

#ifdef _MSC_VER
#pragma warning(disable : 4701)
#endif

//
// Internal data types

//
//SKX change
//
// New swizzled table
const UINT32 ddrioOffsetTable[MAX_STROBE] = {0x200, 0x100, 0xc00, 0xb00, 0x700, 0x400, 0x300, 0x000, 0x800,
                                             0x300, 0x000, 0xd00, 0xa00, 0x600, 0x500, 0x200, 0x100, 0x900};

extern UINT32 ClkAddressConvertTable[MAX_CH];

//
// Local Prototypes
//
UINT32 ProgramIOCompValues(PSYSHOST host, UINT8 socket);
MRC_STATUS GetSetTxDelayBit(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 strobe, UINT8 bit, GSM_LT level,
             GSM_GT group, UINT8 mode, INT16 *value);
MRC_STATUS GetSetTxVrefDDR4(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 strobe, UINT8 bit, GSM_LT level,
             GSM_GT group, UINT8 mode, INT16 *value);
MRC_STATUS GetSetTxVrefFnv(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 strobe, UINT8 bit, GSM_LT level,
             GSM_GT group, UINT8 mode, INT16 *value);
MRC_STATUS GetSetImode (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 strobe, UINT8 mode, INT16 *value);
MRC_STATUS GetSetCTLE (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 strobe, UINT8 bit, GSM_LT level,
             GSM_GT group, UINT8 mode, INT16 *value);
MRC_STATUS GetSetCPUODT (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 strobe, UINT8 bit, GSM_LT level,
             GSM_GT group, UINT8 mode, INT16 *value);
MRC_STATUS GetSetDIMMODT (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 strobe, UINT8 bit, GSM_LT level,
             GSM_GT group, UINT8 mode, INT16 *value);
UINT32 CacheDDRIO(PSYSHOST host, UINT8 socket);
UINT32 MemWriteDimmVref(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 rank, UINT8 strobe, GSM_LT level, UINT8 vref);
UINT32 UpdateIoRegister2 (PSYSHOST host, UINT8 strobe, UINT32 regOffset);
void PanicUpDnMultiplier(PSYSHOST host, UINT8 socket);
void UpdatePanicUpDownCodes(PSYSHOST host, UINT8 socket);
MRC_STATUS GetSetRxEq (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 strobe, GSM_GT group, UINT8 mode, INT16 *value);
static void   DoComp(PSYSHOST host, UINT8 socket); 
MRC_STATUS GetSetMarginParameterCheck (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, GSM_LT level, GSM_GT group);

#ifdef SERIAL_DBG_MSG
char   *GetGroupStr(GSM_GT group, char *strBuf);
#endif  // SERIAL_DBG_MSG

/**

  Initialize the DDRIO interface

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
InitDdrioInterface (
                   PSYSHOST  host
                   )
{
  UINT8                                       socket;
  UINT8                                       imc;
  UINT8                                       ch;
  UINT8                                       dimm;
  UINT8                                       rank;
  UINT8                                       strobe;
  INT16                                       rxVref;
  INT16                                       rxDqDelay;
  struct imcNvram                             (*imcNvList)[MAX_IMC];
  struct channelNvram                         (*channelNvList)[MAX_CH];
  struct dimmNvram                            (*dimmNvList)[MAX_DIMM];
  struct ddrRank                              (*rankList)[MAX_RANK_DIMM];
  struct ddr4OdtValueStruct                   *ddr4OdtValuePtr = NULL;
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT          dataControl0;
  DATACONTROL1N0_0_MCIO_DDRIO_STRUCT          dataControl1;
  DATACONTROL2N0_0_MCIO_DDRIO_STRUCT          dataControl2;
  DATACONTROL4N0_0_MCIO_DDRIO_STRUCT          dataControl4;
  DDRCRCOMPOVR_MCIO_DDRIOEXT_STRUCT           ddrCrCompOvr;
  COMPDATA0N0_0_MCIO_DDRIO_STRUCT             compData0;
  COMPDATA1N0_0_MCIO_DDRIO_STRUCT             compData1;
  DATACONTROL3N0_0_MCIO_DDRIO_STRUCT          dataControl3;
  RXOFFSETN0RANK0_0_MCIO_DDRIO_STRUCT         rxOffset;
  VSSHIORVREFCONTROLN0_0_MCIO_DDRIO_STRUCT    vssControl;
  DDRCRCLKCONTROLS_MCIO_DDRIOEXT_STRUCT       ddrCRClkControls;
  DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_STRUCT  ddrCRCmdControls;
  DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_STRUCT ddrCRCmdControls3;
  DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_STRUCT ddrCRCmdControls3CmdN;
  DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_STRUCT ddrCRCmdControls3CmdS;
  DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_STRUCT  ddrCRCmdControls3Cke;
  DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_STRUCT  ddrCRCmdControls3Ctl;
  DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_STRUCT   ddrCRCtlControls;
  DDRCRCMDCONTROLS1_CTL_MCIO_DDRIOEXT_STRUCT  ddrCRCtlControls1;
  DDRCRCMDCONTROLS1_CMDN_MCIO_DDRIOEXT_STRUCT ddrCRCmdControls1CmdN;
  DDRCRCMDCONTROLS1_CMDS_MCIO_DDRIOEXT_STRUCT ddrCRCmdControls1CmdS;
  DDRCRCOMPCTL0_MCIO_DDRIOEXT_STRUCT          ddrCRCompCtl0;
  DDRCRCOMPCTL1_MCIO_DDRIOEXT_STRUCT          ddrCRCompCtl1;
  DDRCRCOMPCTL2_MCIO_DDRIOEXT_STRUCT          ddrCRCompCtl2;
  DDRCRCOMPCTL3_MCIO_DDRIOEXT_STRUCT          ddrCRCompCtl3;
  DDRCRCTLRANKSUSED_CTL_MCIO_DDRIOEXT_STRUCT  ddrCRCtlRanksUsed;
  DDRCRCLKCOMP_CLK_MCIO_DDRIOEXT_STRUCT       ddrCRClkComp;
  DDRCRCMDCOMP_CMDN_MCIO_DDRIOEXT_STRUCT      ddrCRCmdComp;
  DDRCRCTLCOMP_CKE_MCIO_DDRIOEXT_STRUCT       ddrCRCtlComp;
  DDRCRCOMPVSSHI_MCIO_DDRIOEXT_STRUCT         ddrCRCompVssHi;
  DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_STRUCT  ddrCrDimmVrefControl1;
  DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_STRUCT  ddrCrDimmVrefControlFnv1;
  DDRCRDIMMVREFCONTROL2_MCIO_DDRIOEXT_STRUCT    ddrCrDimmVrefControl2;
  DDRCRCOMPVSSHICONTROL_MCIO_DDRIOEXT_STRUCT    ddrCrCompVssHiControl;
  RXGROUP0N0RANK0_0_MCIO_DDRIO_STRUCT         rxGroup0;
  TXGROUP0N0RANK0_0_MCIO_DDRIO_STRUCT         txGroup0;
  DDRCRSPDCFG1_MCIO_DDRIOEXT_STRUCT           ddrCRSPDCfg1;
  DDRCRSPDCFG2_MCIO_DDRIOEXT_STRUCT           ddrCRSPDCfg2;
  DDRCRCMDCOMPOFFSET_CMDN_MCIO_DDRIOEXT_STRUCT  ddrCRCmdCompOffsetCmdN;
  DDRCRCMDCOMPOFFSET_CMDS_MCIO_DDRIOEXT_STRUCT  ddrCRCmdCompOffsetCmdS;
  DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_STRUCT        ddrCRClkRanksUsed;
  MCDDRTCFG_MC_MAIN_STRUCT            mcDdrtCfg;
  DDRCRCTLCOMPOFFSET_CMDN_MCIO_DDRIOEXT_STRUCT  ddrCRCtlCompOffsetCmdN;
  DDRCRCLKTRAINING_MCIO_DDRIOEXT_STRUCT        ddrCRClkTraining;
  DDRCRCTLCOMPOFFSET_CMDS_MCIO_DDRIOEXT_STRUCT ddrCRCtlCompOffsetCmdS;
  DDRCRCTLCOMPOFFSET_CKE_MCIO_DDRIOEXT_STRUCT  ddrCRCtlCompOffsetCke;
  DDRCRCTLCOMPOFFSET_CTL_MCIO_DDRIOEXT_STRUCT  ddrCRCtlCompOffsetCtl;

  socket = host->var.mem.currentSocket;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "InitDdrioInterface Starts\n"));
#endif

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  channelNvList = GetChannelNvList(host, socket);
  imcNvList     = GetImcNvList(host, socket);

  rxDqDelay = 1;

  //
  // clear IO direction valid bit, before rx_enables and tx_enables
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    ddrCRCtlCompOffsetCmdN.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG);
    ddrCRClkTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompOffsetCmdS.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompOffsetCke.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CKE_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompOffsetCtl.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CTL_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompOffsetCmdN.Bits.iodirectionvalid = 0;
    ddrCRClkTraining.Bits.iodirectionvalid       = 0;
    ddrCRCtlCompOffsetCmdS.Bits.iodirectionvalid = 0;
    ddrCRCtlCompOffsetCke.Bits.iodirectionvalid  = 0;
    ddrCRCtlCompOffsetCtl.Bits.iodirectionvalid  = 0;
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCmdN.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG, ddrCRClkTraining.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCmdS.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CKE_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCke.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CTL_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCtl.Data);

    //
    // clear the scramble config bits for simulation
    //
  }

  //
  // Misc DDRIO programming
  //

  rxOffset.Data = 0;
  rxOffset.Bits.saoffset0 = 15;
  rxOffset.Bits.saoffset1 = 15;
  rxOffset.Bits.saoffset2 = 15;
  rxOffset.Bits.saoffset3 = 15;

  vssControl.Data = 0;
  switch (host->nvram.mem.socket[socket].ddrVoltage) {
  case SPD_VDD_150:
    vssControl.Data = 54; // codetarget
    break;
  case SPD_VDD_135:
    vssControl.Data = 44; // codetarget
    break;
  case SPD_VDD_120:
    vssControl.Data = 30; // codetarget
    break;
  }
  vssControl.Data |= 0 << 6; // HiBWDivider
  vssControl.Data |= 0 << 8; // LoBWDivider
  vssControl.Data |= 0 << 10; // SampleDivider
  vssControl.Data |= 0 << 13; // OpenLoop
  vssControl.Data |= 0 << 14; // BWError
  vssControl.Data |= 1 << 16; // PanicEn
  vssControl.Data |= 1 << 17; // CloseLoop
  vssControl.Data |= 5 << 18; // PanicVoltage
  vssControl.Data |= 1 << 22; // GainBoost
  vssControl.Data |= 0 << 23; // SelCode

  dataControl1.Data = 0;
  dataControl2.Data = 0;
  dataControl3.Data = 0;
  dataControl4.Data = 0;
  ddrCRClkControls.Data = 0;
  ddrCRClkComp.Data = 0;
  ddrCRCmdComp.Data = 0;
  ddrCRCtlComp.Data = 0;
  ddrCRCompVssHi.Data = 0;
  ddrCRCtlControls.Data = 0;
  ddrCrCompOvr.Data = 0;

  ddrCrCompOvr.Bits.dqtco  = 1;
  ddrCrCompOvr.Bits.cmdtco = 1;
  ddrCrCompOvr.Bits.ctltco = 1;
  ddrCrCompOvr.Bits.clktco = 1;

  //dataControl1.Bits.dllmask           = 1;  // 2 QCLK DLL mask
  // TODO: dataControl1.Bits.burstendodtdelay will need to get set based on DDR frequency
  dataControl1.Bits.odtdelay          = 1; //KV3 (UINT32)-2; //Need atleast 17 for now
  dataControl1.Bits.senseampdelay     = 14; // the 2's complement of -2 = 14; (UINT32)-2;
  dataControl1.Bits.odtduration       = 7;  // 18 QCLK
  dataControl1.Bits.senseampduration  = 7;  // 18 QCLK
  dataControl1.Bits.rxbiasctl         = 4;  // 1.33Idll

  compData0.Data = 0;
  compData0.Bits.vtcomp       = 5;
  compData0.Bits.rcompdrvup   = 0x13;
  compData0.Bits.rcompdrvdown = 0x13;
  compData0.Bits.slewratecomp = 9;

  compData1.Data = 0;
  compData1.Bits.panicdrvdn       = 1;
  compData1.Bits.panicdrvup       = 1;
  compData1.Bits.rcompodtup       = 0x10;
  compData1.Bits.rcompodtdown     = 0x10;

  dataControl0.Data = 0;
  dataControl0.Bits.rfon            = 1;
  dataControl0.Bits.rxpion            = 0;
  dataControl0.Bits.txlong            = 0; //KV3 1;
  dataControl0.Bits.internalclockson  = 0;
  //dataControl0.Bits.rxdisable

  //If Write 2tCK Preamble set DDRIO to issue long preamble in datacontrol0
  if ((host->setup.mem.writePreamble > 0) && (host->nvram.mem.socket[socket].ddrFreq >= DDR_2400)){
    dataControl0.Bits.longpreambleenable = 1;
  }

  //dataControl0.Bits.dcdetectmode      = 0;
  dataControl0.Bits.saoffsettraintxon = 0;
  //dataControl2.Bits.imodebiasen   = 1;
  dataControl2.Bits.rxdqssaoffset = 0x10;
  //dataControl2.Bits.imodeenable   = 1;

  //
  // Enable 12 legs
  //
  dataControl3.Bits.drvstaticlegcfg       = 3;
  dataControl3.Bits.odtstaticlegcfg       = 3;

  dataControl3.Bits.longodtr2w = 0;
  dataControl3.Bits.odtsegmentenable    = 3;
  dataControl3.Bits.datasegmentenable     = 7;
  // dataControl3.Bits.imodebiashighcm       = 0; // Removed in 14ww11d CSR XML
  dataControl3.Bits.imodebiasvrefen       = 0;
  // dataControl3.Bits.imodebiasdfxddr3legup = 0; // Removed in 14ww11d CSR XML
  dataControl3.Bits.imodebiasdfxddr4legup = 0;
  dataControl3.Bits.imodebiasdfxlegdn     = 0;
  dataControl3.Bits.ddrcrctleresen        = 0;
  dataControl3.Bits.pcasbiasclosedloopen  = 1;
  dataControl3.Bits.rxbiassel             = 0;
  dataControl3.Bits.rxbiasfoldedlegtrim   = 0;
  dataControl3.Bits.rxbiasgcncomp         = 2;
  dataControl3.Bits.imodebiasrxbiastrim   = 1;

  ddrCRClkControls.Bits.statlegen   = 3;
  ddrCRClkControls.Bits.xoveropmode = 1;
  ddrCRClkControls.Bits.rxvref      = 0x0F;
  if (host->var.common.bootMode != S3Resume){
    ddrCRClkControls.Bits.intclkon    = 1;
  }

  if (host->nvram.mem.dimmTypePresent != UDIMM) {
    ddrCRClkControls.Bits.is_rdimm = 1;
  }

  ddrCRClkComp.Bits.scomp         = 9;
  ddrCRClkComp.Bits.rcompdrvup    = 19;
  ddrCRClkComp.Bits.rcompdrvdown  = 0x13;
  ddrCRClkComp.Bits.lscomp        = 0x1;

  ddrCRCmdComp.Bits.rcompdrvup = 0x0C;
  ddrCRCmdComp.Bits.rcompdrvdown = 0x0C;
  //ddrCRCmdComp.Bits.lscomp = 0x1;

  ddrCRCtlComp.Bits.rcompdrvup = 0x13;
  ddrCRCtlComp.Bits.rcompdrvdown = 0x13;
  ddrCRCtlComp.Bits.lscomp = 0x1;

  // Time 1: lvmode=0, lvmodevalide = 0
  // SKX change
  ddrCRCompCtl0.Data = MemReadPciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG);
  switch (host->nvram.mem.socket[socket].ddrVoltage) {
  case SPD_VDD_120:
    ddrCRCompCtl0.Bits.lvmode = 3;
    break;
  }

  ddrCRCompCtl0.Bits.dqdrvpdnvref = 32;
  ddrCRCompCtl0.Bits.dqodtpupvref = 0x26;

  // Time 2: lvmode = target value, lvmodevalid = 0
  ddrCRCompCtl0.Bits.lvmodevalid  = 0;
  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG, ddrCRCompCtl0.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG, ddrCRCompCtl0.Data);

  // Time 3: lvmode = target value, lvmodevalid = 1
  ddrCRCompCtl0.Bits.lvmodevalid  = 1;
  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG, ddrCRCompCtl0.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG, ddrCRCompCtl0.Data);

  // Time 4: lvmode = target value, lvmodevalid = 0
  ddrCRCompCtl0.Bits.lvmodevalid  = 0;
  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG, ddrCRCompCtl0.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG, ddrCRCompCtl0.Data);

  // Time 5: lvmode = target value, lvmodevalid = 1
  ddrCRCompCtl0.Bits.lvmodevalid  = 1;
  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG, ddrCRCompCtl0.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG, ddrCRCompCtl0.Data);

  // SKX change
  ddrCRCompCtl1.Data = MemReadPciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPCTL1_MCIO_DDRIOEXT_REG);
  ddrCRCompCtl1.Bits.dqscomppc      = 1;
  ddrCRCompCtl1.Bits.cmdscomppc     = 1;
  ddrCRCompCtl1.Bits.ctlscomppc     = 1;


  switch (GetClosestFreq(host, socket)) {
    case(DDR_800):
      ddrCRCompCtl1.Bits.dqscompcells   = 15;   //  the field "dqscompcells" can have max value of 15; ref hsd 4927190, so changed value from 16 to dummy value of 15 for now, later needs to be revised
      ddrCRCompCtl1.Bits.cmdscompcells  = 5;
      ddrCRCompCtl1.Bits.ctlscompcells  = 15;   //  the field "ctlscompcells" can have max value of 15; ref hsd 4927190, so changed value from 16 to dummy value of 15 for now, later needs to be revised
      break;
    case(DDR_1066):
      ddrCRCompCtl1.Bits.dqscompcells   = 12;
      ddrCRCompCtl1.Bits.cmdscompcells  = 6;
      ddrCRCompCtl1.Bits.ctlscompcells  = 12;
      break;
    case(DDR_1333):
      ddrCRCompCtl1.Bits.dqscompcells   = 10;
      ddrCRCompCtl1.Bits.cmdscompcells  = 6;
      ddrCRCompCtl1.Bits.ctlscompcells  = 10;
      break;
    case(DDR_1600):
      ddrCRCompCtl1.Bits.dqscompcells   = 8;
      ddrCRCompCtl1.Bits.cmdscompcells  = 5;
      ddrCRCompCtl1.Bits.ctlscompcells  = 8;
      break;
    case(DDR_1866):
      ddrCRCompCtl1.Bits.dqscompcells   = 7;
      ddrCRCompCtl1.Bits.cmdscompcells  = 4;
      ddrCRCompCtl1.Bits.ctlscompcells  = 7;
      break;
    case(DDR_2133):
      ddrCRCompCtl1.Bits.dqscompcells   = 6;
      ddrCRCompCtl1.Bits.cmdscompcells  = 4;
      ddrCRCompCtl1.Bits.ctlscompcells  = 6;
      break;
    case(DDR_2400):
      ddrCRCompCtl1.Bits.dqscompcells   = 5;
      ddrCRCompCtl1.Bits.cmdscompcells  = 3;
      ddrCRCompCtl1.Bits.ctlscompcells  = 5;
      break;
    case(DDR_2666):
      ddrCRCompCtl1.Bits.dqscompcells   = 4;
      ddrCRCompCtl1.Bits.cmdscompcells  = 2;
      ddrCRCompCtl1.Bits.ctlscompcells  = 4;
      break;
    default:
      ddrCRCompCtl1.Bits.dqscompcells   = 8;
      ddrCRCompCtl1.Bits.cmdscompcells  = 5;
      ddrCRCompCtl1.Bits.ctlscompcells  = 8;
      break;
  }


  ddrCRCompCtl1.Bits.tcovref        = 64;
  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPCTL1_MCIO_DDRIOEXT_REG, ddrCRCompCtl1.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRCOMPCTL1_MCIO_DDRIOEXT_REG, ddrCRCompCtl1.Data);

  // SKX change
  ddrCRCompCtl2.Data = MemReadPciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPCTL2_MCIO_DDRIOEXT_REG);
  ddrCRCompCtl2.Bits.odtstatlegen   = 3;
  ddrCRCompCtl2.Bits.txdqstatlegen  = 3;
  ddrCRCompCtl2.Bits.cmdstatlegen   = 3;
  ddrCRCompCtl2.Bits.dqodtpdnvref   = 32;
  ddrCRCompCtl2.Bits.clkdrvpupvref  = 44;
  ddrCRCompCtl2.Bits.ddr3nren       = 0;
  ddrCRCompCtl2.Bits.clkdrvpdnvref  = 32;
  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPCTL2_MCIO_DDRIOEXT_REG, ddrCRCompCtl2.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRCOMPCTL2_MCIO_DDRIOEXT_REG, ddrCRCompCtl2.Data);

  //SKX change
  ddrCRCompCtl3.Data = MemReadPciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPCTL3_MCIO_DDRIOEXT_REG);
  ddrCRCompCtl3.Bits.swcapcmpclksel = 2;
  ddrCRCompCtl3.Bits.ctldrvpdnvref  = 32;
  ddrCRCompCtl3.Bits.ctldrvpupvref  = 44;
  if (host->nvram.mem.dimmTypePresent == UDIMM) {
    ddrCRCompCtl3.Bits.cmddrvpdnvref  = 49;
  } else {
    ddrCRCompCtl3.Bits.cmddrvpdnvref  = 31;
  }
  if (host->nvram.mem.dimmTypePresent == UDIMM) {
    ddrCRCompCtl3.Bits.cmddrvpupvref  = 43;
  } else {
    ddrCRCompCtl3.Bits.cmddrvpupvref  = 28;
  }
  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPCTL3_MCIO_DDRIOEXT_REG, ddrCRCompCtl3.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRCOMPCTL3_MCIO_DDRIOEXT_REG, ddrCRCompCtl3.Data);

  ddrCRCompVssHi.Bits.panicdrvdnvref = 32;
  ddrCRCompVssHi.Bits.panicdrvupvref = 32;
  ddrCRCompVssHi.Bits.ddrdmvrfs3localpwrgoodoverride = 1;
  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPVSSHI_MCIO_DDRIOEXT_REG, ddrCRCompVssHi.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRCOMPVSSHI_MCIO_DDRIOEXT_REG, ddrCRCompVssHi.Data);

  ddrCrDimmVrefControl1.Data = 0;
  ddrCrDimmVrefControl1.Bits.ch0cavrefctl    = 64;
  ddrCrDimmVrefControl1.Bits.ch2cavrefctl    = 64;
  ddrCrDimmVrefControl1.Bits.ch1cavrefctl    = 64;
  ddrCrDimmVrefControl1.Bits.ch0caendimmvref = 1;
  ddrCrDimmVrefControl1.Bits.ch2caendimmvref = 1;
  ddrCrDimmVrefControl1.Bits.ch1caendimmvref = 1;
  ddrCrDimmVrefControl1.Bits.hiztimerctrl    = 2;
  ddrCrDimmVrefControlFnv1.Data = 0;
  ddrCrDimmVrefControlFnv1.Bits.rxvref = 64;
  for (imc = 0; imc < host->var.mem.maxIMC; imc++) {
    if (host->var.mem.socket[socket].imcEnabled[imc] == 0) continue;
    (*imcNvList)[imc].dimmVrefControl1 = ddrCrDimmVrefControl1.Data;
    MemWritePciCfgMC (host, socket, imc, DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_REG, ddrCrDimmVrefControl1.Data);
  }
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    (*channelNvList)[ch].dimmVrefControlFnv1 = ddrCrDimmVrefControlFnv1.Data;
  }

  // SKX change
  ddrCrDimmVrefControl2.Data = MemReadPciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRDIMMVREFCONTROL2_MCIO_DDRIOEXT_REG);
  ddrCrDimmVrefControl2.Data |= 2 << 8;
  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRDIMMVREFCONTROL2_MCIO_DDRIOEXT_REG, ddrCrDimmVrefControl2.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRDIMMVREFCONTROL2_MCIO_DDRIOEXT_REG, ddrCrDimmVrefControl2.Data);

  ddrCrCompVssHiControl.Data = vssControl.Data;
  if (((host->var.common.bootMode == S3Resume) || (host->var.mem.subBootMode == WarmBootFast))) { 
    ddrCrCompVssHiControl.Data |= BIT13;
    ddrCrCompVssHiControl.Data &= ~BIT17;
  }
  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPVSSHICONTROL_MCIO_DDRIOEXT_REG, ddrCrCompVssHiControl.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRCOMPVSSHICONTROL_MCIO_DDRIOEXT_REG, ddrCrCompVssHiControl.Data);

  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRCOMPOVR_MCIO_DDRIOEXT_REG, ddrCrCompOvr.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRCOMPOVR_MCIO_DDRIOEXT_REG, ddrCrCompOvr.Data);

  // SKX change
  ddrCRSPDCfg1.Data = MemReadPciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRSPDCFG1_MCIO_DDRIOEXT_REG);
  ddrCRSPDCfg1.Bits.crspdcfg1 = 0x83F8;
  ddrCRSPDCfg1.Bits.odtsegovrd = 1;
  ddrCRSPDCfg1.Bits.odtupsegen = 1;
  ddrCRSPDCfg1.Bits.odtdnsegen = 2;

  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRSPDCFG1_MCIO_DDRIOEXT_REG, ddrCRSPDCfg1.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRSPDCFG1_MCIO_DDRIOEXT_REG, ddrCRSPDCfg1.Data);

  // SKX change
  ddrCRSPDCfg2.Data = MemReadPciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRSPDCFG2_MCIO_DDRIOEXT_REG);
  ddrCRSPDCfg2.Bits.ctlsegovrd = 1;
  ddrCRSPDCfg2.Bits.clksegovrd = 1;
  ddrCRSPDCfg2.Bits.dqsegovrd = 1;
  ddrCRSPDCfg2.Bits.ctldnsegen = 3;
  ddrCRSPDCfg2.Bits.ctlupsegen = 3;
  ddrCRSPDCfg2.Bits.clkdnsegen = 3;
  ddrCRSPDCfg2.Bits.clkupsegen = 3;
  ddrCRSPDCfg2.Bits.dqdnsegen = 3;
  ddrCRSPDCfg2.Bits.dqupsegen = 3;


  ddrCRSPDCfg2.Bits.crspdcfg2 = 0x44F0;
  // SKX change
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(0), DDRCRSPDCFG2_MCIO_DDRIOEXT_REG, ddrCRSPDCfg2.Data);
  MemWritePciCfgEp (host, socket, DDRIOEXT2_CH(1), DDRCRSPDCFG2_MCIO_DDRIOEXT_REG, ddrCRSPDCfg2.Data);

  for (ch = 0; ch < MAX_CH; ch++) {
    //SKX change for upper nibble
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, VSSHIORVREFCONTROLN0_0_MCIO_DDRIO_REG), vssControl.Data);
    } // strobe loop
  } // ch loop

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)) {

      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, RXVREFCTRLN0_0_MCIO_DDRIO_REG), 0);
      } // strobe loop

      //
      // Set all Rx Vref per bit
      //

      // Lookup nominal RxVref setting
      ddr4OdtValuePtr = LookupDdr4OdtValue(host, socket, ch);
      RC_ASSERT(ddr4OdtValuePtr != NULL, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_51);

      // Selects the Vref voltage value coming out of internal Vref generator.
      // The absolute Vref values are calculated as below:
      rxVref = ((ddr4OdtValuePtr->mcVref - 50) * 127) / 50;


      GetSetDataGroup (host, socket, ch, 0, 0, ALL_STROBES, ALL_BITS, DdrLevel, RxVref, GSM_READ_CSR | GSM_FORCE_WRITE | GSM_UPDATE_CACHE, &rxVref);
    }

    ddrCRCmdControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG);
    ddrCRCmdControls1CmdN.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS1_CMDN_MCIO_DDRIOEXT_REG);
    ddrCRCmdControls1CmdS.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS1_CMDS_MCIO_DDRIOEXT_REG);
    ddrCRCmdCompOffsetCmdN.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG);
    ddrCRCmdCompOffsetCmdS.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG);
    ddrCRClkRanksUsed.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG);
    mcDdrtCfg.Data = MemReadPciCfgEp (host, socket, ch, MCDDRTCFG_MC_MAIN_REG);

    //SKX change
    ddrCRCmdControls3.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG); //JAD:skx:4/4
    ddrCRCmdControls3.Bits.bufdrvsegen    = 0x1f;
    ddrCRCmdControls3.Bits.divby2alignctl  = 0;     //SKX change HSD 4927799
    ddrCRCmdControls3CmdN.Data = ddrCRCmdControls3.Data;
    ddrCRCmdControls3CmdS.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG);
    ddrCRCmdControls3Cke.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG);
    ddrCRCmdControls3Ctl.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG);

    ddrCRCmdControls.Bits.ddrphasextalkenableqnnnh = 0;
    //ddrCRCmdControls.Bits.gqclkdivenable  = 0;                      //SKX change
    ddrCRCmdControls.Bits.xoveropmode     = 1;
    ddrCRCmdControls.Bits.txon            = 0;
    ddrCRCmdControls.Bits.intclkon        = 1;
    ddrCRCmdControls.Bits.iolbctl         = 0;
    ddrCRCmdControls.Bits.odtmode         = 0;
    ddrCRCmdControls.Bits.earlyweakdrive  = 0;
    ddrCRCmdControls.Bits.lvlshft_holden  = 0;
    ddrCRCmdControls.Bits.rxvref          = 0x0F;
    ddrCRCmdControls.Bits.xovercal        = 0;
    ddrCRCmdControls.Bits.ddr4modeenable = 0;

    //
    // Enable ODT static legs
    //
    ddrCRCmdControls1CmdN.Bits.odtstatdflt  = 3;
    ddrCRCmdControls1CmdN.Bits.dqstatdflt   = 3;
    ddrCRCmdControls1CmdS.Bits.odtstatdflt  = 3;
    ddrCRCmdControls1CmdS.Bits.dqstatdflt   = 3;
//    ddrCRCmdControls1.Bits.ddrcrimodeen = 0;

    ddrCRCmdControls1CmdN.Bits.cmdoutputensel = 0xFFF;
    ddrCRCmdControls1CmdS.Bits.cmdoutputensel = 0xFFF;

    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      if ((*dimmNvList)[dimm].aepDimmPresent) {

        // Configure NVMDIMM signal direction
        /*
        CASE: NVMDIMM DIMM in SLOT0
        Pins to be enabled as receivers: CKE1, ODT1, CK1

        CASE: NVMDIMM DIMM in SLOT1
        Pins to be enabled as receivers: CKE3, ODT3, CK3
        */
        if (dimm == 0) {
          ddrCRCmdCompOffsetCmdN.Bits.ddrcmdctlrx_en |= BIT8;   // ODT1
          ddrCRCmdControls3CmdN.Bits.odten  |= BIT8;
          ddrCRCmdControls1CmdN.Bits.cmdoutputensel &= ~BIT8;
          ddrCRCmdCompOffsetCmdS.Bits.ddrcmdctlrx_en |= BIT11;  // CKE1
          ddrCRCmdControls3CmdS.Bits.odten  |= BIT11;
          ddrCRCmdControls1CmdS.Bits.cmdoutputensel &= ~BIT11;
          ddrCRClkRanksUsed.Bits.ddrtclk1en = 1;                // CK1
          mcDdrtCfg.Bits.slot0 = 1;

        } else if (dimm == 1) {
          ddrCRCmdCompOffsetCmdN.Bits.ddrcmdctlrx_en |= BIT0;   // ODT3
          ddrCRCmdControls3CmdN.Bits.odten  |= BIT0;
          ddrCRCmdControls1CmdN.Bits.cmdoutputensel &= ~BIT0;
          ddrCRCmdCompOffsetCmdS.Bits.ddrcmdctlrx_en |= BIT7;   // CKE3
          ddrCRCmdControls3CmdS.Bits.odten  |= BIT7;
          ddrCRCmdControls1CmdS.Bits.cmdoutputensel &= ~BIT7;
          ddrCRClkRanksUsed.Bits.ddrtclk3en = 1;                // CK3
          mcDdrtCfg.Bits.slot1 = 1;
        }
      } else {
        ddrCRCmdCompOffsetCmdS.Bits.ddrcmdctlrx_en |= BIT6;     // ALERT_N
        ddrCRCmdControls3CmdS.Bits.odten  |= BIT6;
        ddrCRCmdControls1CmdS.Bits.cmdoutputensel &= ~BIT6;
      }
    }

    if (host->var.common.bootMode == S3Resume){
      ddrCRCmdControls.Bits.drvpupdis = 0;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);
      ddrCRCmdControls.Bits.drvpupdis = 1;
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);
      MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);
      ddrCRCmdControls.Bits.drvpupdis = 0;
    }

    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCOMP_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdComp.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCOMP_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdComp.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdControls.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS1_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdControls1CmdN.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS1_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdControls1CmdS.Data);
    //SKX change
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdControls3CmdN.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdControls3CmdS.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG, ddrCRCmdControls3Cke.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG, ddrCRCmdControls3Ctl.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG, ddrCRCmdCompOffsetCmdN.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG, ddrCRCmdCompOffsetCmdS.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, ddrCRClkRanksUsed.Data);
    MemWritePciCfgEp (host, socket, ch, MCDDRTCFG_MC_MAIN_REG, mcDdrtCfg.Data);

    IO_Reset(host, socket);

    ddrCRCtlControls.Bits.xoveropmode = 1;
    ddrCRCtlControls.Bits.intclkon    = 1;
    ddrCRCtlControls.Bits.rxvref      = 0x0F;

    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CTL_MCIO_DDRIOEXT_REG, ddrCRCtlControls.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS_CKE_MCIO_DDRIOEXT_REG, ddrCRCtlControls.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMP_CKE_MCIO_DDRIOEXT_REG, ddrCRCtlComp.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMP_CTL_MCIO_DDRIOEXT_REG, ddrCRCtlComp.Data);

    ddrCRCtlRanksUsed.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLRANKSUSED_CKE_MCIO_DDRIOEXT_REG);

    MemWritePciCfgEp (host, socket, ch, DDRCRCTLRANKSUSED_CKE_MCIO_DDRIOEXT_REG, ddrCRCtlRanksUsed.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLRANKSUSED_CTL_MCIO_DDRIOEXT_REG, ddrCRCtlRanksUsed.Data);

    ddrCRCtlControls1.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCMDCONTROLS1_CKE_MCIO_DDRIOEXT_REG);

    ddrCRCtlControls1.Bits.odtstatdflt  = 3;
    ddrCRCtlControls1.Bits.dqstatdflt   = 3;
    ddrCRCtlControls1.Bits.cmdoutputensel = 0xfff;
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS1_CKE_MCIO_DDRIOEXT_REG, ddrCRCtlControls1.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCMDCONTROLS1_CTL_MCIO_DDRIOEXT_REG, ddrCRCtlControls1.Data);

    if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)) {
      (*channelNvList)[ch].dataControl0 = dataControl0.Data;
      (*channelNvList)[ch].ddrCRClkControls = ddrCRClkControls.Data;
    } else if ((host->var.common.bootMode == S3Resume) || (host->var.mem.subBootMode == WarmBootFast) || (host->var.mem.subBootMode == ColdBootFast)) {
      dataControl0.Data = (*channelNvList)[ch].dataControl0;
      ddrCRClkControls.Data = (*channelNvList)[ch].ddrCRClkControls;
      if (host->var.common.bootMode == S3Resume){
        ddrCRClkControls.Bits.intclkon = 0;
      }
    }

    MemWritePciCfgEp (host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG, ddrCRClkControls.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCLKCOMP_CLK_MCIO_DDRIOEXT_REG, ddrCRClkComp.Data);

    // SKX change for upper nibble
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)) {
        (*channelNvList)[ch].dataControl2[strobe] = dataControl2.Data;
        (*channelNvList)[ch].dataControl1[strobe] = dataControl1.Data;  // save initial values to cache
        (*channelNvList)[ch].dataControl3[strobe] = dataControl3.Data;

        dataControl4.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG));
        dataControl4.Bits.rxvrefsel = RX_VREF_SETTING;
        dataControl4.Bits.vreftonblbus = RX_VREF_TO_NIBBLE_BUS;
        dataControl4.Bits.fnvcrvisaen = 0; //KV1
        dataControl4.Bits.fnvcrdllbypassen = 0; //KV1
        dataControl4.Bits.fnvcrsdlbypassen = 0; //KV1
        (*channelNvList)[ch].dataControl4[strobe] = dataControl4.Data;

      } else if ((host->var.common.bootMode == S3Resume) || (host->var.mem.subBootMode == WarmBootFast) || (host->var.mem.subBootMode == ColdBootFast)) {
        dataControl1.Data = (*channelNvList)[ch].dataControl1[strobe];  // load cached values for other flows
        dataControl2.Data = (*channelNvList)[ch].dataControl2[strobe];
        dataControl4.Data = (*channelNvList)[ch].dataControl4[strobe];
        dataControl3.Data = (*channelNvList)[ch].dataControl3[strobe];
      }
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL1N0_0_MCIO_DDRIO_REG), dataControl1.Data);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG), dataControl2.Data);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL3N0_0_MCIO_DDRIO_REG), dataControl3.Data);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG), dataControl4.Data);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, COMPDATA0N0_0_MCIO_DDRIO_REG), compData0.Data);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, COMPDATA1N0_0_MCIO_DDRIO_REG), compData1.Data);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, TXXTALKN0_0_MCIO_DDRIO_REG), 0);
    } // strobe loop

    if (host->var.common.bootMode == NormalBoot && host->var.mem.subBootMode == ColdBoot){

      dimmNvList  = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        rankList = GetRankNvList(host, socket, ch, dimm);

        for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

            for (strobe = 0; strobe < MAX_STROBE; strobe++) {

              {
                rxGroup0.Data = 0;
                txGroup0.Data = 0;
                if ((*dimmNvList)[dimm].x4Present == 0) {
                  // Set for x8 mode
                  if (strobe >= MAX_STROBE / 2) {
                    rxGroup0.Bits.readx8modeen = 1;
                  txGroup0.Bits.x8writeen = 1;
                  }
                }
                (*channelNvList)[ch].rxOffset[(*rankList)[rank].rankIndex][strobe] |= rxOffset.Data;
                MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, (*rankList)[rank].logicalRank, strobe, RXOFFSETN0RANK0_0_MCIO_DDRIO_REG),
                                  (*channelNvList)[ch].rxOffset[(*rankList)[rank].rankIndex][strobe]);
                //SKX change
                MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, (*rankList)[rank].logicalRank, strobe, RXGROUP0N0RANK0_0_MCIO_DDRIO_REG), rxGroup0.Data);
                MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, (*rankList)[rank].logicalRank, strobe, TXGROUP0N0RANK0_0_MCIO_DDRIO_REG), txGroup0.Data);
                MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, (*rankList)[rank].logicalRank, strobe, RXGROUP1N0RANK0_0_MCIO_DDRIO_REG), 0);
                MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, (*rankList)[rank].logicalRank, strobe, TXGROUP1N0RANK0_0_MCIO_DDRIO_REG), 0x00208208);
              }
            } // strobe loop

            GetSetDataGroup (host, socket, ch, dimm, rank, ALL_STROBES, 0, DdrLevel, RxDqDelay, GSM_READ_CSR | GSM_UPDATE_CACHE | GSM_FORCE_WRITE, &rxDqDelay);
        } // rank loop
      } // dimm loop
    }
  } // ch loop


  //
  // SKX TODO: Set SKX indicator from Jack
  //

  rcPrintf ((host, "JAD: START_COMP_FLOW\n"));
  //
  // Start a COMP cycle
  //
  //
  // set IO direction valid bit, after rx_enables and tx_enables
  //
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    ddrCRCtlCompOffsetCmdN.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG);
    ddrCRClkTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompOffsetCmdS.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompOffsetCke.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CKE_MCIO_DDRIOEXT_REG);
    ddrCRCtlCompOffsetCtl.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CTL_MCIO_DDRIOEXT_REG);

    ddrCRCtlCompOffsetCmdN.Bits.iodirectionvalid = 1;
    ddrCRClkTraining.Bits.iodirectionvalid = 1;
    ddrCRCtlCompOffsetCmdS.Bits.iodirectionvalid = 1;
    ddrCRCtlCompOffsetCke.Bits.iodirectionvalid = 1;
    ddrCRCtlCompOffsetCtl.Bits.iodirectionvalid = 1;

    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDN_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCmdN.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG, ddrCRClkTraining.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CMDS_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCmdS.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CKE_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCke.Data);
    MemWritePciCfgEp (host, socket, ch, DDRCRCTLCOMPOFFSET_CTL_MCIO_DDRIOEXT_REG, ddrCRCtlCompOffsetCtl.Data);
  }

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "InitDdrioInterface Ends\n"));
#endif

  return SUCCESS;
} // InitDdrioInterface

void
RestoreDDRIO (
           PSYSHOST  host,
           UINT8     socket
           )
{
  UINT8               ch;
  UINT8               imc;
  UINT8               strobe;

  struct imcNvram     (*imcNvList)[MAX_IMC];
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);
  imcNvList     = GetImcNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    MemWritePciCfgEp (host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRClkControls);
    MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, 0, DATACONTROL0N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl0);

    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL1N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl1[strobe]);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl2[strobe]);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl4[strobe]);
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL3N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl3[strobe]);
    }
    //
    // Restore roundtrip, IO latencies, etc, for fast boot.
    //
    RestoreTimings(host, socket, ch);

  } // Ch

  for (imc = 0; imc < host->var.mem.maxIMC; imc++) {
    if (host->var.mem.socket[socket].imcEnabled[imc] == 0) continue;
    MemWritePciCfgMC (host, socket, imc, DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_REG, (*imcNvList)[imc].dimmVrefControl1);
    MemWritePciCfgMC (host, socket, imc, DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG, (*imcNvList)[imc].ddrCRCompCtl0);
  }

}
/**

  Initialize the DDRIO interface

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
UINT32
InitDdrioInterfaceLate (
                   PSYSHOST  host
                   )
{
  UINT8                                       socket;
  UINT8                                       ch;
  UINT8                                       dimm;
  UINT8                                       rank;
  UINT8                                       strobe;
  UINT8                                       txVref = 0;
  UINT8                                       imc;
  struct channelNvram                         (*channelNvList)[MAX_CH];
  struct dimmNvram                            (*dimmNvList)[MAX_DIMM];
  struct ddrRank                              (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                           (*rankStruct)[MAX_RANK_DIMM];
  struct ddr4OdtValueStruct                   *ddr4OdtValuePtr = NULL;
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT          dataControl0;
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT          dataControl0_nonecc;
  DDRCRCLKCOMP_CLK_MCIO_DDRIOEXT_STRUCT       ddrCRClkComp;
  MC_INIT_STATE_G_MC_MAIN_STRUCT              mcInitStateG;
  DDRCRCLKCONTROLS_MCIO_DDRIOEXT_STRUCT       ddrCRClkControls;
  DDRCRCLKTRAINING_MCIO_DDRIOEXT_STRUCT       ddrCRClkTraining;
  DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_STRUCT      ddrCRClkRanksEnabled;
  DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_STRUCT      save_ddrCRClkRanksUsed[MAX_CH];
  DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_STRUCT  ddrCrDimmVrefControl1;
  MCMAIN_CHKN_BITS_MC_MAIN_STRUCT             mcChknBits;
  DATACONTROL0N0_0_FNV_DDRIO_COMP_STRUCT      dataControl0fnv;
  REVISION_MFG_ID_FNV_DFX_MISC_0_STRUCT       revisionmfg;
  SREF_STATIC2_MCDDC_CTL_STRUCT               srefStatic2;
  RXGROUP0N0RANK0_0_FNV_DDRIO_COMP_STRUCT     rxGroup0;
  MCMTR_MC_MAIN_STRUCT                        mcMtr;
  INT16                                       piDelayfnv = 64;
  UINT16                                      minVal;
  UINT16                                      maxVal;
  INT16                                       zeroOffset = 0;
  RXVREFCTRLN0_0_FNV_DDRIO_COMP_STRUCT        txVrefCtrl;
  socket = host->var.mem.currentSocket;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "InitDdrioInterfaceLate Starts\n"));
#endif

  //
  // Return if this socket is disabled
  //
  if ((host->nvram.mem.socket[socket].enabled == 0) || (host->nvram.mem.socket[socket].maxDimmPop == 0)) return SUCCESS;

  channelNvList = GetChannelNvList(host, socket);

  if ((host->var.common.bootMode == S3Resume)) {
      for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;

          MemWritePciCfgEp(host, socket, ch, DDRCRCMDTRAINING_CMDN_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdTrainingCmdN);
          MemWritePciCfgEp(host, socket, ch, DDRCRCMDTRAINING_CMDS_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdTrainingCmdS);
          MemWritePciCfgEp(host, socket, ch, DDRCRCMDTRAINING_CTL_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCtlTraining);
          MemWritePciCfgEp(host, socket, ch, DDRCRCMDTRAINING_CKE_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCkeTraining);
          MemWritePciCfgEp(host, socket, ch, DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRClkTraining);

          MemWritePciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CMDN_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdControls3CmdN);
          MemWritePciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CMDS_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdControls3CmdS);
          MemWritePciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CTL_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdControls3Ctl);
          MemWritePciCfgEp(host, socket, ch, DDRCRCMDCONTROLS3_CKE_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRCmdControls3Cke);
          MemWritePciCfgEp(host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, (*channelNvList)[ch].ddrCRClkRanksUsed);

          // SKX change for upper nibble
          for (strobe = 0; strobe < MAX_STROBE; strobe++) {

              MemWritePciCfgEp(host, socket, ch, UpdateIoRegisterCh(host, strobe, DATAOFFSETTRAINN0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataOffsetTrain[strobe]);
              MemWritePciCfgEp(host, socket, ch, UpdateIoRegisterCh(host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG), (*channelNvList)[ch].dataControl4[strobe]);
          } // strobe loop
      }

      for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;
          OutputCheckpoint(host, 0xb3, 0x11, 0);
          ddrCRClkRanksEnabled.Data = MemReadPciCfgEp(host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG);
          ddrCRClkRanksEnabled.Bits.ranken = 0xF;
          MemWritePciCfgEp(host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, ddrCRClkRanksEnabled.Data);
          IO_Reset(host, socket);
      }

      FnvPollingBootStatusRegister(host, socket, FNV_DT_DONE);
  }

  if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)) {
    
    // HSD 533094
    // Intialize CAVREF for B0
    if (CheckSteppingGreaterThan(host, CPU_SKX, A2_REV_SKX)) {
      for (imc = 0; imc < host->var.mem.maxIMC; imc++) {
        ddrCrDimmVrefControl1.Data = MemReadPciCfgMC (host, socket, imc, DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_REG);
        ddrCrDimmVrefControl1.Data &= 0xf8000000; // zero out VREF enable and values
        for (ch = 0; ch < MAX_MC_CH; ch++) {
          if ((*channelNvList)[ch + (MAX_MC_CH * imc)].enabled == 0) continue;
          switch (ch) {
            case 0:
              ddrCrDimmVrefControl1.Bits.ch0caendimmvref = 1;
              ddrCrDimmVrefControl1.Bits.ch0cavrefctl = CAVREF;
              break;
            case 1:
              ddrCrDimmVrefControl1.Bits.ch1caendimmvref = 1;
              ddrCrDimmVrefControl1.Bits.ch1cavrefctl = CAVREF;
              break;
            case 2:
              ddrCrDimmVrefControl1.Bits.ch2caendimmvref = 1;
              ddrCrDimmVrefControl1.Bits.ch2cavrefctl = CAVREF;
              break;
          }
        }        
        MemWritePciCfgMC (host, socket, imc, DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_REG, ddrCrDimmVrefControl1.Data);
      }
    }
    
    //
    // Initialize DDRIO cached values
    //
    CacheDDRIO(host, socket);

    //SKX HSD: 4929993 : ddrclk comp codes not valid in ODT mode
    if (CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        ddrCRClkComp.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKCOMP_CLK_MCIO_DDRIOEXT_REG);
        ddrCRClkTraining.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG);
        ddrCRClkTraining.Bits.spare2 = ddrCRClkComp.Bits.rcompdrvup | (ddrCRClkComp.Bits.rcompdrvdown << 6);
        MemWritePciCfgEp (host, socket, ch, DDRCRCLKTRAINING_MCIO_DDRIOEXT_REG, ddrCRClkTraining.Data);
      }
    }

      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
          dataControl0.Data = (*channelNvList)[ch].dataControl0;

        // 2 TCLK preamble
        if ((host->setup.mem.writePreamble > 0) && (host->nvram.mem.socket[socket].ddrFreq >= DDR_2400)){
          dataControl0.Bits.longpreambleenable = 1;
        } else {
          dataControl0.Bits.longpreambleenable = 0;
        }
        (*channelNvList)[ch].dataControl0 = dataControl0.Data; // cache value

        // setup no ecc version
        dataControl0_nonecc.Data = dataControl0.Data;
        dataControl0_nonecc.Bits.rxdisable = 1;
        dataControl0_nonecc.Bits.txdisable = 1;

        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))){
            MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0_nonecc.Data);
          } else {
            MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
          }
      }
    }

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      ddr4OdtValuePtr = LookupDdr4OdtValue(host, socket, ch);
      RC_ASSERT(ddr4OdtValuePtr != NULL, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_52);
      if (ddr4OdtValuePtr == NULL) continue;

      //
      // Construct CKE mask
      //
      dimmNvList  = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        rankList = GetRankNvList(host, socket, ch, dimm);
        rankStruct  = GetRankStruct(host, socket, ch, dimm);

        for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

          // Assume DRAM Vref range 1 (60% - 92.5%)
          txVref = ((((ddr4OdtValuePtr->dramVref - 60) * 100) / 65) & 0x3F);
          (*channelNvList)[ch].txVrefSafe[(*rankList)[rank].rankIndex] = txVref + DDR4_VREF_RANGE1_OFFSET;

          //
          // Save Tx Vref
          //
          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
#ifdef  LRDIMM_SUPPORT
            if (IsLrdimmPresent(host, socket, ch, dimm)) {
              if ((*dimmNvList)[dimm].aepDimmPresent == 1) {
                // save backside rxVref here
                (*rankList)[rank].lrbufRxVref[strobe] = (*dimmNvList)[dimm].SPDLrbufDbVrefdq;
                (*rankStruct)[rank].lrbufRxVrefCache[strobe] = (*rankList)[rank].lrbufRxVref[strobe];
                //MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT, "DB RxVref to DRAM = %d\n", (*rankList)[rank].lrbufRxVref[strobe]));
              } else { //DDR4 path
                if ((*dimmNvList)[dimm].SPDSpecRev > 7) {
                  // For SPD rev > 7, get the TxVref value from SPD
                  switch (rank) {
                    case 0:
                      (*rankList)[rank].lrbufTxVref[strobe] = (*dimmNvList)[dimm].SPDLrbufDramVrefdqR0;
                      break;

                    case 1:
                      (*rankList)[rank].lrbufTxVref[strobe] = (*dimmNvList)[dimm].SPDLrbufDramVrefdqR1;
                      break;

                    case 2:
                      (*rankList)[rank].lrbufTxVref[strobe] = (*dimmNvList)[dimm].SPDLrbufDramVrefdqR2;
                      break;

                    case 3:
                      (*rankList)[rank].lrbufTxVref[strobe] = (*dimmNvList)[dimm].SPDLrbufDramVrefdqR3;
                      break;
                    }

                   (*rankStruct)[rank].lrbufTxVrefCache[strobe] = (*rankList)[rank].lrbufTxVref[strobe];
                   // Set MR6 value with training mode enabled
                   (*rankStruct)[rank].MR6[strobe] = (UINT8) ((*rankList)[rank].lrbufTxVref[strobe] | BIT7);
                   //also save backside rxVref here
                   (*rankList)[rank].lrbufRxVref[strobe] = (*dimmNvList)[dimm].SPDLrbufDbVrefdq;
                   (*rankStruct)[rank].lrbufRxVrefCache[strobe] = (*rankList)[rank].lrbufRxVref[strobe];
                  } else {
                    if ((*dimmNvList)[dimm].numDramRanks == 4) {
                      //Backside DRAM txVref, hard code MR6 value for 78.2% = 0x1C with training mode enabled
                      (*rankStruct)[rank].MR6[strobe] = (0x1C | BIT7);
                      (*rankList)[rank].lrbufTxVref[strobe] = 0x1C;
                      (*rankStruct)[rank].lrbufTxVrefCache[strobe] = 0x1C;
                      //also save backside rxVref here
                      //quad rank buffer backside vref: 82.1% = 0x22 (range 1)
                      (*rankList)[rank].lrbufRxVref[strobe] = 0x22+DB_DRAM_VREF_RANGE2_OFFSET;
                      (*rankStruct)[rank].lrbufRxVrefCache[strobe] = 0x22+DB_DRAM_VREF_RANGE2_OFFSET;
                    } else {
                      //Backside DRAM txVref, hard code MR6 value for 74% = 0x16 with training mode enabled
                      (*rankStruct)[rank].MR6[strobe] = (0x16 | BIT7);
                      (*rankList)[rank].lrbufTxVref[strobe] = 0x16;
                      (*rankStruct)[rank].lrbufTxVrefCache[strobe] = 0x16;
                      //also save backside rxVref here
                      //dual rank buffer backside vref: 76.9% = 0x1A (range 1)
                      (*rankList)[rank].lrbufRxVref[strobe] = 0x1A+DB_DRAM_VREF_RANGE2_OFFSET;
                      (*rankStruct)[rank].lrbufRxVrefCache[strobe] = 0x1A+DB_DRAM_VREF_RANGE2_OFFSET;
                    }
                  } //SPD rev
                } // DDR4 present
              } else // LRDIMM
#endif
            {
              (*rankStruct)[rank].MR6[strobe] = (txVref | BIT7);
            }
            (*channelNvList)[ch].txVref[(*rankList)[rank].rankIndex][strobe] = (*channelNvList)[ch].txVrefSafe[(*rankList)[rank].rankIndex];
            (*channelNvList)[ch].txVrefCache[(*rankList)[rank].rankIndex][strobe] = (*channelNvList)[ch].txVrefSafe[(*rankList)[rank].rankIndex];

            // QR RDIMM support
            if ((IsLrdimmPresent(host, socket, ch, 0) == 0) && ((*channelNvList)[ch].numQuadRanks > 0)) {
              INT16 TxEqValue = 10;
              if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;
              if (strobe > 8) continue;
              GetSetDataGroup(host, socket, ch, dimm, rank, strobe, ALL_BITS, DdrLevel, TxEq, GSM_UPDATE_CACHE | GSM_FORCE_WRITE, &TxEqValue);
            }
          } // strobe loop

          (*channelNvList)[ch].ckeMask |= 1 << (*rankList)[rank].CKEIndex;
        } // rank loop
      } // dimm loop
    } // ch loop

#ifdef  LRDIMM_SUPPORT
    // Update safe TXVREF for LRDIMMS on backside
    if (host->nvram.mem.socket[socket].lrDimmPresent) {
      UpdateSafeTxVref(host, socket, LrbufLevel);
    }
#endif
  } //boot mode

  //
  // Restore DDRIO values
  //
  if ((host->var.common.bootMode == S3Resume) || (host->var.mem.subBootMode == WarmBootFast) || (host->var.mem.subBootMode == ColdBootFast)){
    RestoreDDRIO (host, socket);
  }

  // HSD 5331856 disable closed page mode during training
  for (imc = 0; imc < host->var.mem.maxIMC; imc++) {
    if (host->var.mem.socket[socket].imcEnabled[imc] == 0) continue;

    //
    // Clear to set open page mode
    //
    mcMtr.Data = MemReadPciCfgMC (host, socket, imc, MCMTR_MC_MAIN_REG);
    mcMtr.Bits.close_pg = 0;
    MemWritePciCfgMC (host, socket, imc, MCMTR_MC_MAIN_REG, mcMtr.Data);
  } // imc loop

  //
  // Hard coded COMP settings
  //
  ProgramIOCompValues(host, socket);

  if (host->var.common.bootMode == S3Resume){
    mcInitStateG.Data = MemReadPciCfgMain (host, socket, MC_INIT_STATE_G_MC_MAIN_REG);
    mcInitStateG.Bits.dclk_enable = 1;
    MemWritePciCfgMain (host, socket, MC_INIT_STATE_G_MC_MAIN_REG, mcInitStateG.Data);

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      ddrCRClkControls.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG);
      ddrCRClkControls.Bits.intclkon = 1;
      MemWritePciCfgEp (host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG, ddrCRClkControls.Data);
     }
  }

  
  if ((host->var.common.bootMode == S3Resume) || (host->var.mem.subBootMode == WarmBootFast)) {

    if (host->nvram.mem.dimmTypePresent == RDIMM) {
      if ((host->var.common.cpuType == CPU_SKX) && CheckSteppingLessThan (host, CPU_SKX, B0_REV_SKX)) {
       //s4930298 SKX HSD/s4987926/s5001711 WA starts
        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;

          // Save original value
          ddrCRClkRanksEnabled.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG);
          save_ddrCRClkRanksUsed[ch].Data = ddrCRClkRanksEnabled.Data;

          // Set drvpupdis bit to pull CK low
          ddrCRClkRanksEnabled.Bits.drvpupdis = 1;

          // Apply setting to each channel
          MemWritePciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, ddrCRClkRanksEnabled.Data);
        } //ch loop

        // Delay for Register to recognize CK stop mode
        FixedDelay(host, 10);

        for (ch = 0; ch < MAX_CH; ch++) {
          if ((*channelNvList)[ch].enabled == 0) continue;

          // Restore drvpupdis setting on each channel
          MemWritePciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, save_ddrCRClkRanksUsed[ch].Data);
        } //ch loop
        //s4930298 SKX HSD/s4987926/s5001711 WA ends
      }
    } //if RDIMM / LRDIMM

  }//if S3Resume || WarmBootFast
  
  //
  // Add PanicUp/Dn Multiplier Calculation
  //
  PanicUpDnMultiplier(host, socket);

  //
  // Start a COMP cycle, to have correct final Panic Up/Dn codes.
  //
//#ifdef YAM_ENV
  DoComp (host, socket);
//#endif

  //
  // s4929988: Cloned From SKX Si Bug Eco: ddrcmd panic comp up/down broken
  //
  if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX)) {
    UpdatePanicUpDownCodes(host, socket);

    //
    // Start a COMP cycle, to have correct final Panic Up/Dn codes.
    //
//#ifdef YAM_ENV
    DoComp (host, socket);
//#endif

  }  

  if ((host->var.common.bootMode == NormalBoot) && ((host->var.mem.subBootMode == ColdBoot) || (host->var.mem.subBootMode == ColdBootFast))){

    // HSD 5331110  Disable Periodic Rcomp during memory training
    for (imc = 0; imc < host->var.mem.maxIMC; imc++) {
      if (host->var.mem.socket[socket].imcEnabled[imc] == 0) continue;
        //comp
      mcChknBits.Data = MemReadPciCfgMC (host, socket, imc, MCMAIN_CHKN_BITS_MC_MAIN_REG);
      mcChknBits.Bits.dis_rcomp = 1;
      MemWritePciCfgMC (host, socket, imc, MCMAIN_CHKN_BITS_MC_MAIN_REG, mcChknBits.Data);
    }


    if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode != WarmBootFast)) {
      FnvDdrtIoInit(host, socket, PLL_LOCK);
      WaitForMailboxReady(host, socket);
    }
  }

  if ((host->var.common.bootMode != S3Resume)) {
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        ddrCRClkRanksEnabled.Data = MemReadPciCfgEp(host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG);
        ddrCRClkRanksEnabled.Bits.ranken = 0xF;
        MemWritePciCfgEp(host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, ddrCRClkRanksEnabled.Data);
        IO_Reset(host, socket);
      }
  }

  CrossoverCalib(host);

  if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode == ColdBoot)){
      //
      // Initialize CMD/CTL/CLK pi delays
      //
          SetStartingCCC(host, socket);
  }

  if ((host->var.common.bootMode == NormalBoot) && ((host->var.mem.subBootMode == ColdBoot) || (host->var.mem.subBootMode == ColdBootFast))){
    //
    // Execute NVMDIMM IO init in NVMCTLR
    //
    ResetAllDdrChannels (host, socket, CH_BITMASK);

    if ((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode != WarmBootFast)) {
      FixedDelay(host, 100000);
      FnvDdrtIoInit (host, socket, DDRT_FREQ);
    }

  }
  host->var.mem.ioInitdone[socket] = 1;

  //
  // Update backside txVref cached values
  //
  if (((host->var.common.bootMode == NormalBoot) && ((host->var.mem.subBootMode == ColdBoot) || (host->var.mem.subBootMode == ColdBootFast)))){
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < MAX_DIMM; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
        GetSetCmdVref(host, socket, ch, dimm, DdrLevel, GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &zeroOffset);
        GetSetCmdVref(host, socket, ch, dimm, LrbufLevel, GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &zeroOffset);
        rankList = GetRankNvList(host, socket, ch, dimm);
        rankStruct = GetRankStruct(host, socket, ch, dimm);
        for (rank = 0; rank < host->var.mem.socket[socket].maxRankDimm; rank++) {
          if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            GetSetFnvIO(host, socket, ch, dimm, UpdateIoRegisterCh(host, strobe, RXVREFCTRLN0_0_FNV_DDRIO_COMP_REG), &txVrefCtrl.Data, 0, FNV_IO_READ, 0xF);
            (*rankList)[rank].lrbufTxVref[strobe] = txVrefCtrl.Data;
            (*rankStruct)[rank].lrbufTxVrefCache[strobe] = (*rankList)[rank].lrbufTxVref[strobe];
          } // strobe
        } // rank
      } // dimm
    } // ch
  }
  OutputCheckpoint (host, STS_DDRIO_INIT, SUB_DDRT_IO_INIT, 0);

  if (((host->var.common.bootMode == NormalBoot) && ((host->var.mem.subBootMode == ColdBoot) || (host->var.mem.subBootMode == ColdBootFast)))){ 

    if (((host->var.common.bootMode == NormalBoot) && (host->var.mem.subBootMode != WarmBootFast))) {       //
      // Initiate JEDEC init
      //
      JedecInitDdrAll (host, socket, CH_BITMASK);
    }

    if (host->var.mem.subBootMode == ColdBoot){
      SenseAmpOffset(host);
      SetAepTrainingMode (host,socket, ENABLE_TRAINING_MODE);
    }

    rxGroup0.Data = 0;
    rxGroup0.Bits.rcvendelay = 0x40;

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      dimmNvList = GetDimmNvList(host, socket, ch);
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        // This caching must be done before any GetSetFnvIO call
        (*dimmNvList)[dimm].fnvioControl = ReadFnvCfg(host, socket, ch, dimm, MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_REG);
        // 4928807: MRC: For NVMCTLR WRCRC, modify the NVMCTLR settings for DDR_IODAT_DataControl1.OdtDuration
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          GetSetFnvIO(host, socket, ch, dimm, UpdateIoRegisterCh(host, strobe, RXGROUP0N0RANK0_0_FNV_DDRIO_COMP_REG), &rxGroup0.Data, 0, FNV_IO_WRITE, 0xF);
        } // strobe

        WriteFnvCfg(host, socket, ch, dimm, D_RST_MASK_FNV_D_UNIT_0_REG, 1);
        //
        // Program Register Command Latency Addr
        //
        WriteFnvCfg(host, socket, ch, dimm, DA_DDRT_TRAINING_RC0F_FNV_DA_UNIT_0_REG, (*channelNvList)[ch].commandLatencyAddr + COMMAND_LATENCY_DEFAULT);
      } // dimm
    } // ch

    if (host->setup.mem.enableNgnBcomMargining) {
      if (host->nvram.mem.aepDimmPresent) {
        CrossoverCalibFnv(host);
      }
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = GetDimmNvList(host, socket, ch);
        for (dimm = 0; dimm < MAX_DIMM; dimm++) {
          if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
          if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

          GetSetClkDelayCore(host, socket, ch, dimm, LrbufLevel, 0, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelayfnv);
          GetSetCtlGroupDelayCore(host, socket, ch, dimm, LrbufLevel, CtlAll, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelayfnv, &minVal, &maxVal);
          GetSetCmdGroupDelayCore(host, socket, ch, dimm, LrbufLevel, CmdAll, GSM_FORCE_WRITE | GSM_WRITE_OFFSET | GSM_UPDATE_CACHE, &piDelayfnv, &minVal, &maxVal);
        } // dimm
      } // ch
    } // IsBacksideCmdMarginEnabled
  }

  //
  // Disable unused output clocks for ddr4 and NVMDIMM dimms
  //
  //
  //        DIMM1              DIMM0             Ranken -- binary(slot0 - bit 0 and bit 2; slot1 - bit 1 and bit 3)
  //  (L)RDIMM (SR/DR)   (L)RDIMM (SR/DR)          0011
  //       UDIMM (SR)        UDIMM (SR)            0011
  //       UDIMM (DR)        UDIMM (DR)            1111
  //       UDIMM (DR)        UDIMM (SR)            1011
  //       UDIMM (SR)        UDIMM (DR)            0111
  //       DDRT          (L)RDIMM (SR/DR)          1011
  //  (L)RDIMM (SR/DR)       DDRT                  0111
  //
  // HSD 4929953
  //
  if (host->var.common.cpuType == CPU_SKX) {
    host->var.mem.socket[socket].clkSwapFixDis = 0;
  } else {
    host->var.mem.socket[socket].clkSwapFixDis = 1;
  }
 

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);
    ddrCRClkRanksEnabled.Data = MemReadPciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG);    

    ddrCRClkRanksEnabled.Bits.ranken = 0;
    for (dimm = 0; dimm < host->var.mem.socket[socket].channelList[ch].numDimmSlots; dimm++) {      
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      if ((host->var.mem.socket[socket].clkSwapFixDis == 0)) {  // fixed clk mapping for SKX
        if (((*dimmNvList)[dimm].aepDimmPresent) || (((*dimmNvList)[dimm].numRanks == 2) && ((host->nvram.mem.dimmTypePresent == UDIMM) || (host->nvram.mem.dimmTypePresent == SODIMM)))) {
          ddrCRClkRanksEnabled.Bits.ranken |= (0x3 << dimm*2);
        } else {
          ddrCRClkRanksEnabled.Bits.ranken |= (0x1 << dimm*2);
        }
      } else {                                      // original CLK mapping, for SKX - DDRT not supported
        if (((*dimmNvList)[dimm].aepDimmPresent) || (((*dimmNvList)[dimm].numRanks == 2) && ((host->nvram.mem.dimmTypePresent == UDIMM) || (host->nvram.mem.dimmTypePresent == SODIMM)))) {
          ddrCRClkRanksEnabled.Bits.ranken |= (0x5 << dimm);
        } else {
          ddrCRClkRanksEnabled.Bits.ranken |= (0x1 << dimm);
        }

      }
    }
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "Clocks enabled(ranken): 0x%x\n", ddrCRClkRanksEnabled.Bits.ranken));
    MemWritePciCfgEp (host, socket, ch, DDRCRCLKRANKSUSED_MCIO_DDRIOEXT_REG, ddrCRClkRanksEnabled.Data);

    // program idle_cmd_to_cke_drops_timer value provided by design
    srefStatic2.Data = MemReadPciCfgEp(host, socket, ch, SREF_STATIC2_MCDDC_CTL_REG);    
    srefStatic2.Bits.idle_cmd_to_cke_drops_timer = IDLE_CMD_CKE_DROPS_TIMER;
    MemWritePciCfgEp(host, socket, ch, SREF_STATIC2_MCDDC_CTL_REG, srefStatic2.Data);  
    MemDebugPrint((host, SDBG_MAX, socket, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "idle_cmd_to_cke_drops_timer programmed to: 0x%x\n", srefStatic2.Bits.idle_cmd_to_cke_drops_timer));

    //HSD 5332703-EKV S0 IO power optimization requires DDRT training update
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      revisionmfg.Data = ReadFnvCfg(host, socket, ch, dimm, REVISION_MFG_ID_FNV_DFX_MISC_0_REG);
      if (revisionmfg.Bits.revision_id == 0x10) {
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          GetSetFnvIO(host, socket, ch, dimm, DATACONTROL0N0_0_FNV_DDRIO_COMP_REG, &dataControl0fnv.Data, 0, FNV_IO_READ, 0xF);
          dataControl0fnv.Bits.txpion = 1;
          dataControl0fnv.Bits.rxpion = 1;
          GetSetFnvIO(host, socket, ch, dimm, UpdateIoRegisterCh(host, strobe, DATACONTROL0N0_0_FNV_DDRIO_COMP_REG), &dataControl0fnv.Data, 0, FNV_IO_WRITE, 0xF);
        } // strobe loop
      }
    }
  } // ch loop
 
#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MINMAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "InitDdrioInterfaceLate Ends\n"));
#endif

  return SUCCESS;
} // InitDdrioInterfaceLate

/**

  Programs IO delays

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)
  @param strobe  - Strobe number (0-based)
  @param bit     - Bit number
  @param level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  @param group   - CSM_GT - Parameter to program
  @param mode    - Bit-field of different modes
  @param value   - Pointer to delay value or offset based on mode

  @retval MRC_STATUS

**/
MRC_STATUS
GetSetDataGroup (
                PSYSHOST  host,
                UINT8     socket,
                UINT8     ch,
                UINT8     dimm,
                UINT8     rank,
                UINT8     strobe,
                UINT8     bit,
                GSM_LT    level,
                GSM_GT    group,
                UINT8     mode,
                INT16     *value
                )
{
  MRC_STATUS  status = MRC_STATUS_SUCCESS;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);

  /*
   * Checks are needed to determine whether a margin parameter can be operated on with the specified
   * arguments based on the memory technology (e.g. HBM vs. DDR4), DIMM population, and CPU stepping
   * in the event of known issues affecting the usability of a given margin parameter, and failing these
   * changes should result in a different enum value being returned.
  */
  status = GetSetMarginParameterCheck(host, socket, ch, dimm, level, group);
  if (status == MRC_STATUS_LEVEL_NOT_SUPPORTED) {
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, strobe, bit,
                   "Level = %d, Group = %d,  MRC Margin Group Not Supported\n", level, group));
    return MRC_STATUS_LEVEL_NOT_SUPPORTED;
  }

  status = MRC_STATUS_SUCCESS;
  switch (group) {
    case TxDqDelay:
    case TxDqsDelay:
    case TxDqsDelayCycle:
    case TxEq:
    case WrLvlDelay:
      status = GetSetTxDelay (host, socket, ch, dimm, rank, strobe, bit, level, group, mode, value);
      break;

    case TxDqBitDelay:
      status = GetSetTxDelayBit (host, socket, ch, dimm, rank, strobe, bit, level, group, mode, value);
      break;

    case RxDqsBitDelay:
    case RxDqsPBitDelay:
    case RxDqsNBitDelay:
      status = GetSetRxDelayBit (host, socket, ch, dimm, rank, strobe, bit, level, group, mode, value);
      break;

    case RecEnDelay:
    case RecEnDelayCycle:
    case RxDqsPDelay:
    case RxDqsNDelay:
    case RxDqsDelay:
    case RxDqDelay:
    case RxDqDqsDelay:
      status = GetSetRxDelay (host, socket, ch, dimm, rank, strobe, bit, level, group, mode, value);
      break;

    case RxVref:
      status = GetSetRxVref (host, socket, ch, dimm, rank, strobe, bit, level, group, mode, value);
      break;

    case TxVref:
        // Check for NVMDIMM Present and LrbufLevel, then call GetSetTxVrefFnv
      if ((*dimmNvList)[dimm].aepDimmPresent && level == LrbufLevel){
        status = GetSetTxVrefFnv (host, socket, ch, dimm, rank, strobe, bit, level, group, mode, value);
      } else {
        status = GetSetTxVrefDDR4 (host, socket, ch, dimm, rank, strobe, bit, level, group, mode, value);
      }
      break;

    case TxImode:
      status = GetSetImode (host, socket, ch, strobe, mode, value);
      break;
      
    case RxEq:
      status = GetSetRxEq (host, socket, ch, strobe, group, mode, value);
      break;
    case RxCtleC:
    case RxCtleR:
      status = GetSetCTLE (host, socket, ch, dimm, rank, strobe, bit, level, group, mode, value);
      break;
    case RxOdt:
    case TxRon:
      status = GetSetCPUODT (host, socket, ch, dimm, rank, strobe, bit, level, group, mode, value);
      break;
    case DramDrvStr:
    case ParkOdt:
    case NomOdt:
    case WrOdt:
      status = GetSetDIMMODT (host, socket, ch, dimm, rank, strobe, bit, level, group, mode, value);
      break;
    default:
      MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                     "GetSetDataGroup called unknown group!\n"));
      RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_53);
      break;

  }

  if (status != MRC_STATUS_SUCCESS) {
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, rank, strobe, bit,
                   "Level = %d, Group = %d,  MRC Margin Group training failed.\n", level, group));
    RC_ASSERT (FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_85);
  }

  return status;
} // GetSetDataGroup

/** HSD 5332196
  GetSetMarginParameterCheck - To check if a margin parameter can be operated on with the specified
  arguments based on the memory technology (e.g. HBM vs. DDR4), DIMM population, and CPU stepping
  in the event of known issues affecting the usability of a given margin parameter wrt a platform,
  and failing these checks should result in a different enum value being returned.
  @retval MRC_STATUS
**/
MRC_STATUS
GetSetMarginParameterCheck (
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     dimm,
              GSM_LT    level,
              GSM_GT    group
              )
{
  //Backside CMD/CTL/CLK margining is only available for modules with REV 2 Dimms
  if ((level == LrbufLevel) && ((group == CmdAll) || (group == CtlAll) || (group == CmdVref))) {
    if (IsDdr4RegisterRev2 (host, socket, ch, dimm)) {
      return MRC_STATUS_SUCCESS;
    } else {
      return MRC_STATUS_LEVEL_NOT_SUPPORTED;
    }
  }

  return MRC_STATUS_SUCCESS;
}//GetSetMarginParameterCheck

/**

  Get TxDelay

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)
  @param strobe  - Strobe number (0-based)
  @param bit     - Bit number
  @param level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  @param group   - CSM_GT - Parameter to program
  @param mode    - Bit-field of different modes
  @param value   - Pointer to delay value or offset based on mode

  @retval IO delay

**/
MRC_STATUS
GetSetTxDelay (
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     dimm,
              UINT8     rank,
              UINT8     strobe,
              UINT8     bit,
              GSM_LT    level,
              GSM_GT    group,
              UINT8     mode,
              INT16     *value
              )
{
  UINT8                                     logRank;
  UINT8                                     maxStrobe;
  UINT16                                    logicVal = 0;
  UINT16                                    piVal = 0;
  UINT16                                    curVal = 0;
  UINT16                                    curVal2 = 0;
  INT16                                     delta0 = 0;
  INT16                                     delta1 = 0;
  INT16                                     delta2 = 0;
  INT16                                     delta3 = 0;
  UINT16                                    piDelay;
  INT16                                     tmp;
  UINT16                                    piDelay2;
  INT16                                     tmp2;
  UINT16                                    maxLimit = 0;
  UINT16                                    minLimit = 0;
  UINT16                                    usDelay = 0;
#ifdef SERIAL_DBG_MSG
  char                                      strBuf0[128];
#endif  // SERIAL_DBG_MSG
  struct channelNvram                       (*channelNvList)[MAX_CH];
  TXGROUP0N0RANK0_0_MCIO_DDRIO_STRUCT       txGroup0;
  TXGROUP0N0RANK0_0_MCIO_DDRIO_STRUCT       txGroup0n1;
  TXGROUP1N0RANK0_0_MCIO_DDRIO_STRUCT       txGroup1;
//  DATAOFFSETTRAINN0_0_MCIO_DDRIO_STRUCT    dataOffsetTrain;
#ifdef RC_SIM_EYE_DATA_FEEDBACK
  UINT8                                     feedbackStrobe;
  INT16                                     feedbackValue;
  UINT16                                    latestPiDelay;
#endif // RC_SIM_EYE_DATA_FEEDBACK
#ifdef LRDIMM_SUPPORT
  UINT8                                     updateNeeded;
  UINT8                                     tmpStrobe;
  UINT8                                     *controlWordDataPtr;
  UINT8                                     controlWordAddr;
  struct dimmNvram                          (*dimmNvList)[MAX_DIMM];
  struct rankDevice                         (*rankStruct)[MAX_RANK_DIMM];
#endif //LRDIMM_SUPPORT
  channelNvList = GetChannelNvList(host, socket);

#ifdef LRDIMM_SUPPORT
  rankStruct  = GetRankStruct(host, socket, ch, dimm);
  dimmNvList = GetDimmNvList(host, socket, ch);
#endif //LRDIMM_SUPPORT

  txGroup0.Data = 0;
  txGroup1.Data = 0;

  //
  // Get the logical rank #
  //
  logRank = GetLogicalRank(host, socket, ch, dimm, rank);

  //
  // Get minimum and maximum value
  //
  GetDataGroupLimits(host, level, group, &minLimit, &maxLimit, &usDelay);

  if (strobe == ALL_STROBES) {
    strobe      = 0;
    maxStrobe   = MAX_STROBE;
  } else {
    maxStrobe = strobe + 1;
  }
  
  if (maxStrobe > MAX_STROBE) {
    return MRC_STATUS_STROBE_NOT_PRESENT;
  }

#ifdef LRDIMM_SUPPORT
  updateNeeded = 0;
  if (((*dimmNvList)[dimm].lrBuf_BC1x & LRDIMM_ONE_RANK_TIMING_MODE) && (level == LrbufLevel)) {
    rank = 0;
    logRank = GetLogicalRank(host, socket, ch, dimm, 0);
  }
#endif //LRDIMM_SUPPORT

  for ( ; strobe < maxStrobe; strobe++) {

#ifdef LRDIMM_SUPPORT
    if (level == LrbufLevel) {
      if (mode & GSM_READ_CSR) {
        //
        // Read from "actual setting" cache
        //
        switch (group) {
        case TxDqDelay:
          if ((*dimmNvList)[dimm].rankList[rank].lrBuf_FxBC8x9x[strobe] < 16) {
            curVal = (*dimmNvList)[dimm].rankList[rank].lrBuf_FxBC8x9x[strobe] + 15;
          } else {
            curVal = (*dimmNvList)[dimm].rankList[rank].lrBuf_FxBC8x9x[strobe] ^ 31;
          }

          break;
        case TxDqsDelay:
          curVal = (*dimmNvList)[dimm].rankList[rank].lrBuf_FxBCAxBx[strobe];
          break;
        case TxDqsDelayCycle:
          curVal = (((*dimmNvList)[dimm].rankList[rank].lrBuf_FxBCDxFx[strobe % 9] >> (4 * (strobe / 9))) & 0xF);
          break;
        default:
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                         "GetSetTxDelay called unsupported group!\n"));
          RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_54);
          break;
        } //switch group
      } else {
        //
        // Read from setting cache
        //
        switch (group) {
        case TxDqDelay:
          if ((*rankStruct)[rank].cachedLrBuf_FxBC8x9x[strobe] < 16) {
            curVal = (*rankStruct)[rank].cachedLrBuf_FxBC8x9x[strobe] + 15;
          } else {
            curVal = (*rankStruct)[rank].cachedLrBuf_FxBC8x9x[strobe] ^ 31;
          }
          break;
        case TxDqsDelay:
          curVal = (*rankStruct)[rank].cachedLrBuf_FxBCAxBx[strobe];
          break;
        case TxDqsDelayCycle:
          curVal = (((*rankStruct)[rank].cachedLrBuf_FxBCDxFx[strobe % 9] >> (4 * (strobe / 9))) & 0xF);
          break;
        default:
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                         "GetSetTxDelay called unsupported group!\n"));
         RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_55);
          break;
        } //switch group
      }
    } else
#endif //LRDIMM_SUPPORT
    {
      if (mode & GSM_READ_CSR) {
        //
        // Read from PCI config space
        //
        txGroup0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, TXGROUP0N0RANK0_0_MCIO_DDRIO_REG));
        txGroup1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, TXGROUP1N0RANK0_0_MCIO_DDRIO_REG));
        if (group == TxEq) {
          if ((strobe+9) >= MAX_STROBE) {
            return MRC_STATUS_STROBE_NOT_PRESENT;
          }
          txGroup0n1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, (strobe+9), TXGROUP0N0RANK0_0_MCIO_DDRIO_REG));
        }
      } else {
        //
        // Read from cache
        //
        txGroup0.Data = (*channelNvList)[ch].txGroup0[logRank][strobe];
        txGroup1.Data = (*channelNvList)[ch].txGroup1[logRank][strobe];
        if (group == TxEq) {
          if ((strobe+9) >= MAX_STROBE) {
            return MRC_STATUS_STROBE_NOT_PRESENT;
          }
          txGroup0n1.Data = (*channelNvList)[ch].txGroup0[logRank][(strobe+9)];
        }
      }

      switch (group) {
      case WrLvlDelay:
      case TxDqDelay:

        logicVal = (UINT16)(txGroup0.Bits.txdqdelay & 0x1C0);   // SKX design only uses bits[8:6]

        //Use mux select to determine largest delay setting
        switch(txGroup0.Bits.txdqpiclksel) {
        case 0:
          piVal = (UINT16)txGroup1.Bits.txdqpicode0;
          break;
        case 1:
          piVal = (UINT16)txGroup1.Bits.txdqpicode1;
          break;
        case 2:
          piVal = (UINT16)txGroup1.Bits.txdqpicode2;
          break;
        case 3:
          piVal = (UINT16)txGroup1.Bits.txdqpicode3;
          break;
        }
        // Get current largest delay value
        curVal = logicVal + piVal;

        // Relative per-bit offsets to maintain
        delta0 = (UINT16)(txGroup1.Bits.txdqpicode0 - piVal);
        delta1 = (UINT16)(txGroup1.Bits.txdqpicode1 - piVal);
        delta2 = (UINT16)(txGroup1.Bits.txdqpicode2 - piVal);
        delta3 = (UINT16)(txGroup1.Bits.txdqpicode3 - piVal);
        // get the strobe PI
        if (group == WrLvlDelay){
          curVal2 = (UINT16)txGroup0.Bits.txdqsdelay;
        }
        break;

      case TxDqsDelay:
        curVal = (UINT16)txGroup0.Bits.txdqsdelay;
        break;

      case TxEq:
        curVal = (UINT16)txGroup0.Bits.txeq;
        break;
      default:
        break;
      } // switch
    }

    //
    // Check if read only
    //
    if (mode & GSM_READ_ONLY) {
      if (group == WrLvlDelay){ 
        *value = curVal2; // return DQS value for wrlvlDelay
      } else {
        *value = curVal;
      } 
    } else {
      //
      // Write
      //
      // Adjust the current CMD delay value by offset
      if (mode & GSM_WRITE_OFFSET) {
        tmp = curVal + *value;
      } else {
        tmp = *value;
      }

      if (tmp >= minLimit) {
        piDelay = tmp;
      } else {
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                       "%s Out of range!! tmp = 0x%x, minLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), tmp, minLimit));
        piDelay = minLimit;
      }
      // Ensure we do not exceed maximums
      if (piDelay > maxLimit) {
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                       "%s Out of range!! piDelay = 0x%x, maxLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), piDelay, maxLimit));
        piDelay = maxLimit;
      }
      if (group == WrLvlDelay){
        // Adjust the current CMD delay value by offset
        if (mode & GSM_WRITE_OFFSET) {
          tmp2 = curVal2 + *value;
        } else {
          tmp2 = *value;
        }

        if (tmp2 >= minLimit) {
          piDelay2 = tmp2;
        } else {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                         "%s Out of range!! tmp2 = 0x%x, minLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), tmp2, minLimit));
          piDelay2 = minLimit;
        }
        // Ensure we do not exceed maximums
        if (piDelay2 > maxLimit) {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                         "%s Out of range!! piDelay2 = 0x%x, maxLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), piDelay2, maxLimit));
          piDelay2 = maxLimit;
        }
      }

#ifdef LRDIMM_SUPPORT
      if (level == LrbufLevel) {
        curVal = piDelay;
        switch (group) {
          case TxDqDelay:
            if (curVal < 16) {
              if ((*dimmNvList)[dimm].rankList[rank].lrBuf_FxBC8x9x[strobe] != (UINT8) (curVal ^ 31)) {
                updateNeeded = 1;
                (*dimmNvList)[dimm].rankList[rank].lrBuf_FxBC8x9x[strobe] = (UINT8) (curVal ^ 31);
              }
            } else {
              if ((*dimmNvList)[dimm].rankList[rank].lrBuf_FxBC8x9x[strobe] != (UINT8) (curVal - 15)) {
                updateNeeded = 1;
                (*dimmNvList)[dimm].rankList[rank].lrBuf_FxBC8x9x[strobe] = (UINT8) (curVal - 15);
              }
            }
            break;
          case TxDqsDelay:
            if ((*dimmNvList)[dimm].rankList[rank].lrBuf_FxBCAxBx[strobe] != (UINT8) curVal) {
              updateNeeded = 1;
              (*dimmNvList)[dimm].rankList[rank].lrBuf_FxBC8x9x[strobe] = (UINT8) curVal;
            }
            break;
          case TxDqsDelayCycle:
            if ((((*dimmNvList)[dimm].rankList[rank].lrBuf_FxBCDxFx[strobe % 9]>>(4 * (strobe / 9))) & 0xF) != (UINT8) curVal) {
              updateNeeded = 1;
              (*dimmNvList)[dimm].rankList[rank].lrBuf_FxBCDxFx[strobe % 9] &= 0xF0 >> (4 * (strobe / 9));
              (*dimmNvList)[dimm].rankList[rank].lrBuf_FxBCDxFx[strobe % 9] |= (UINT8) curVal<<(4 * (strobe / 9));
            }
            break;
          default:
            break;
        }
      } else
#endif  //LRDIMM
      {
        switch (group) {
        case WrLvlDelay:
        case TxDqDelay:

          txGroup0.Bits.txdqdelay = piDelay & 0x1C0;      // SKX design only uses bits[8:6]
          txGroup1.Bits.txdqpicode0 = (piDelay + delta0) % 64;
          txGroup1.Bits.txdqpicode1 = (piDelay + delta1) % 64;
          txGroup1.Bits.txdqpicode2 = (piDelay + delta2) % 64;
          txGroup1.Bits.txdqpicode3 = (piDelay + delta3) % 64;
          // update txdqs also 
          if (group == WrLvlDelay){
            txGroup0.Bits.txdqsdelay = piDelay2;
          }
          break;

        case TxDqsDelay:
          txGroup0.Bits.txdqsdelay = piDelay;
          break;

        case TxEq:
          txGroup0.Bits.txeq = piDelay;
          txGroup0n1.Bits.txeq = piDelay;
          break;
        default:
          break;
        } // switch

        //
        // Write it back if the current data does not equal the cache value or if in force write mode
        //
        if ((txGroup0.Data != (*channelNvList)[ch].txGroup0[logRank][strobe]) || (mode & GSM_FORCE_WRITE)) {
          MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, TXGROUP0N0RANK0_0_MCIO_DDRIO_REG), txGroup0.Data);
          if (group == TxEq) { 
            if ((strobe+9) >= MAX_STROBE) {
              return MRC_STATUS_STROBE_NOT_PRESENT;
            }
            MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, (strobe+9), TXGROUP0N0RANK0_0_MCIO_DDRIO_REG), txGroup0n1.Data);
          } 
        }
        if ((txGroup1.Data != (*channelNvList)[ch].txGroup1[logRank][strobe]) || (mode & GSM_FORCE_WRITE)) {
          MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, TXGROUP1N0RANK0_0_MCIO_DDRIO_REG), txGroup1.Data);
        }
        //
        // Wait for the new value to settle
        //
        FixedDelay(host, usDelay);
      }
    } // if read only

    //
    // Save to cache
    //
    if (mode & GSM_UPDATE_CACHE) {
      if (level == DdrLevel) {
        (*channelNvList)[ch].txGroup0[logRank][strobe] = txGroup0.Data;
        if (group == TxEq) {
          if ((strobe+9) >= MAX_STROBE) {
            return MRC_STATUS_STROBE_NOT_PRESENT;
          }
          (*channelNvList)[ch].txGroup0[logRank][(strobe+9)] = txGroup0n1.Data;
        }
        if (group == TxDqDelay) {
          (*channelNvList)[ch].txGroup1[logRank][strobe] = txGroup1.Data;
        }
      } //level
    }
  } // strobe loop

#ifdef LRDIMM_SUPPORT
  if (mode & GSM_UPDATE_CACHE) {
    if (level == LrbufLevel) {
      switch (group) {
        case TxDqDelay:
          for (tmpStrobe = 0; tmpStrobe < MAX_STROBE; tmpStrobe++) {
            (*rankStruct)[rank].cachedLrBuf_FxBC8x9x[tmpStrobe] =  (*dimmNvList)[dimm].rankList[rank].lrBuf_FxBC8x9x[tmpStrobe];
          }
          break;
        case TxDqsDelay:
          for (tmpStrobe = 0; tmpStrobe < MAX_STROBE; tmpStrobe++) {
            (*rankStruct)[rank].cachedLrBuf_FxBCAxBx[tmpStrobe] =  (*dimmNvList)[dimm].rankList[rank].lrBuf_FxBCAxBx[tmpStrobe];
          }
          break;
        case TxDqsDelayCycle:
          for (tmpStrobe = 0; tmpStrobe < MAX_STROBE / 2; tmpStrobe++) {
            (*rankStruct)[rank].cachedLrBuf_FxBCDxFx[tmpStrobe] =  (*dimmNvList)[dimm].rankList[rank].lrBuf_FxBCDxFx[tmpStrobe];
          }
          break;
        default:
          break;
      } // switch
    } //level
  }

  if ((level == LrbufLevel) && !(mode & GSM_READ_ONLY) && ((updateNeeded) || (mode & GSM_FORCE_WRITE))) {
    switch (group) {
      case TxDqDelay:
        controlWordDataPtr = &((*dimmNvList)[dimm].rankList[rank].lrBuf_FxBC8x9x[0]);
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, rank, LRDIMM_BC8x, ALL_DATABUFFERS);
        controlWordDataPtr = &((*dimmNvList)[dimm].rankList[rank].lrBuf_FxBC8x9x[9]);
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, rank, LRDIMM_BC9x, ALL_DATABUFFERS);
        break;
      case TxDqsDelay:
        controlWordDataPtr = &((*dimmNvList)[dimm].rankList[rank].lrBuf_FxBCAxBx[0]);
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, rank, LRDIMM_BCAx, ALL_DATABUFFERS);
        controlWordDataPtr = &((*dimmNvList)[dimm].rankList[rank].lrBuf_FxBCAxBx[9]);
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, rank, LRDIMM_BCBx, ALL_DATABUFFERS);
        break;
      case TxDqsDelayCycle:
        controlWordDataPtr = &((*dimmNvList)[dimm].rankList[rank].lrBuf_FxBCDxFx[0]);
        controlWordAddr = LRDIMM_BCDx + ((rank >> 1) * 0x20);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, (rank & BIT0) , controlWordAddr, ALL_DATABUFFERS);
        break;
      default:
      break;
    } // switch
  }
#endif  //LRDIMM

  return MRC_STATUS_SUCCESS;
} // GetSetTxDelay

/**

  Get RxDelay

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)
  @param strobe  - Strobe number (0-based)
  @param bit     - Bit number
  @param level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  @param group   - CSM_GT - Parameter to program
  @param mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  @param value   - Data to program

  @retval IO delay

**/
MRC_STATUS
GetSetTxDelayBit (
                 PSYSHOST  host,
                 UINT8     socket,
                 UINT8     ch,
                 UINT8     dimm,
                 UINT8     rank,
                 UINT8     strobe,
                 UINT8     bit,
                 GSM_LT    level,
                 GSM_GT    group,
                 UINT8     mode,
                 INT16     *value
                 )
{
  UINT8                               logRank;
  UINT8                               maxStrobe;
  UINT8                               i;
  UINT16                              clkSel;
  UINT16                              logicVal;
  UINT16                              curVal = 0;
  UINT16                              curValBit[4];
  UINT16                              piVal;
  UINT16                              piValBit[4];
  UINT16                              piDelay;
  INT16                               tmp;
  INT16                               bitNibbleMin;
  UINT16                              maxLimit = 0;
  UINT16                              minLimit = 0;
  UINT16                              usDelay = 0;
#ifdef SERIAL_DBG_MSG
  char                                strBuf0[128];
#endif  // SERIAL_DBG_MSG
  struct channelNvram                 (*channelNvList)[MAX_CH];
  TXGROUP0N0RANK0_0_MCIO_DDRIO_STRUCT       txGroup0;
  TXGROUP1N0RANK0_0_MCIO_DDRIO_STRUCT txGroup1;

  channelNvList = GetChannelNvList(host, socket);

  txGroup0.Data = 0;
  txGroup1.Data = 0;

  //
  // Get the logical rank #
  //
  logRank = GetLogicalRank(host, socket, ch, dimm, rank);

  //
  // Get minimum and maximum value
  //
  GetDataGroupLimits(host, level, group, &minLimit, &maxLimit, &usDelay);

  if (strobe == ALL_STROBES) {
    strobe    = 0;
    maxStrobe = MAX_STROBE;
  } else {
    maxStrobe = strobe + 1;
  }

  if (maxStrobe > MAX_STROBE) {
    return MRC_STATUS_STROBE_NOT_PRESENT;
  }

  //Assert on error; should never get here!
  RC_ASSERT(bit < 4, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_56);

  for ( ; strobe < maxStrobe; strobe++) {

    if (mode & GSM_READ_CSR) {
      //
      // Read from PCI config space
      //
      txGroup0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, TXGROUP0N0RANK0_0_MCIO_DDRIO_REG));
      txGroup1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, TXGROUP1N0RANK0_0_MCIO_DDRIO_REG));

    } else {
      //
      // Read from cache
      //
      txGroup0.Data = (*channelNvList)[ch].txGroup0[logRank][strobe];
      txGroup1.Data = (*channelNvList)[ch].txGroup1[logRank][strobe];
    }

    logicVal = (UINT16)(txGroup0.Bits.txdqdelay & 0x1C0);   // SKX design only uses bits[8:6]

    // Use mux select to determine largest per-bit setting
    
    if (strobe>=9){
      piValBit[0] = (UINT16)txGroup1.Bits.txdqpicode2;
      piValBit[1] = (UINT16)txGroup1.Bits.txdqpicode3;
      piValBit[2] = (UINT16)txGroup1.Bits.txdqpicode0;
      piValBit[3] = (UINT16)txGroup1.Bits.txdqpicode1;
      piVal = piValBit[(txGroup0.Bits.txdqpiclksel +2)%4];
    } else{
      piValBit[0] = (UINT16)txGroup1.Bits.txdqpicode0;
      piValBit[1] = (UINT16)txGroup1.Bits.txdqpicode1;
      piValBit[2] = (UINT16)txGroup1.Bits.txdqpicode2;
      piValBit[3] = (UINT16)txGroup1.Bits.txdqpicode3;
      piVal = piValBit[txGroup0.Bits.txdqpiclksel];
    }

    // Calculate total delay values
    for (i = 0; i < 4; i++) {

      // Handle wrap
      if (piValBit[i] > piVal) {

        //Assert that sufficient range exists
        RC_ASSERT(logicVal, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_57);

        curValBit[i] = logicVal + piValBit[i] - 64;
      } else {
        curValBit[i] = logicVal + piValBit[i];
      }
    }

    // Get current delay value
    curVal = curValBit[bit];

    //
    // Check if read only
    //
    if (mode & GSM_READ_ONLY) {
      //
      // Save the value read into the output parameter
      //
      *value = curVal;

    } else {
      //
      // Write
      //
      // Adjust the current delay value by offset
      if (mode & GSM_WRITE_OFFSET) {
        //
        // Add the offset to the current value
        //
        tmp = curVal + *value;

      } else {
        //
        // Write absolute value
        //
        tmp = *value;
      }

      //
      // Make sure we do not exeed the limits
      //
      if (tmp >= minLimit) {
        piDelay = tmp;
      } else {
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, bit,
                       "%s Out of range!! tmp = 0x%x, minLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), tmp, minLimit));
        piDelay = minLimit;
      }
      // Ensure we do not exceed maximums
      if (piDelay > maxLimit) {
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, bit,
                       "%s Out of range!! piDelay = 0x%x, maxLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), piDelay, maxLimit));
        piDelay = maxLimit;
      }

      // Update current delay value
      curValBit[bit] = piDelay;

      // Determine minumum delay
      bitNibbleMin = 512;   // Set to largest possible
      for (i = 0; i < BITS_PER_NIBBLE; i++) {
        if (curValBit[i] < bitNibbleMin) {
          bitNibbleMin = curValBit[i];
        }
      }
      clkSel = 0;
      piDelay = 0;
      for (i = 0; i < BITS_PER_NIBBLE; i++) {

        // Enforce maximum per-bit skew
        if(curValBit[i] - bitNibbleMin > 15) {
          curValBit[i] = bitNibbleMin + 15;
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                       "Skew exceeds 15 ticks. Limiting skew to 15 tick this sounds good\n"));
        }

        // Determine largest per-bit delay
        if (curValBit[i] > piDelay) {
          clkSel = i;
          piDelay = curValBit[i];
        }
      }
      // Update CSR settings
      txGroup0.Bits.txdqdelay = piDelay & 0x1C0;      // SKX design only uses bits[8:6]
      if (strobe>=9){
        txGroup0.Bits.txdqpiclksel = (clkSel + 2)%4;
        txGroup1.Bits.txdqpicode0 = curValBit[2] % 64;
        txGroup1.Bits.txdqpicode1 = curValBit[3] % 64;
        txGroup1.Bits.txdqpicode2 = curValBit[0] % 64;
        txGroup1.Bits.txdqpicode3 = curValBit[1] % 64;
      } else{
        txGroup0.Bits.txdqpiclksel = clkSel;
        txGroup1.Bits.txdqpicode0 = curValBit[0] % 64;
        txGroup1.Bits.txdqpicode1 = curValBit[1] % 64;
        txGroup1.Bits.txdqpicode2 = curValBit[2] % 64;
        txGroup1.Bits.txdqpicode3 = curValBit[3] % 64;
      }

      //
      // Write it back if the current data does not equal the cache value or if in force write mode
      //
      if ((txGroup0.Data != (*channelNvList)[ch].txGroup0[logRank][strobe]) || (mode & GSM_FORCE_WRITE)) {
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, TXGROUP0N0RANK0_0_MCIO_DDRIO_REG), txGroup0.Data);
      }
      if ((txGroup1.Data != (*channelNvList)[ch].txGroup1[logRank][strobe]) || (mode & GSM_FORCE_WRITE)) {
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, TXGROUP1N0RANK0_0_MCIO_DDRIO_REG), txGroup1.Data);
      }
      //
      // Wait for the new value to settle
      //
      FixedDelay(host, usDelay);

    } // if read only

    //
    // Save to cache
    //
    if (mode & GSM_UPDATE_CACHE) {
      (*channelNvList)[ch].txGroup0[logRank][strobe] = txGroup0.Data;
      (*channelNvList)[ch].txGroup1[logRank][strobe] = txGroup1.Data;
    }
  } // strobe loop

  return MRC_STATUS_SUCCESS;
} // GetSetTxDelayBit

/**

  Get RxDelay

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)
  @param strobe  - Strobe number (0-based)
  @param bit     - Bit number
  @param level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  @param group   - CSM_GT - Parameter to program
  @param mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  @param value   - Data to program

  @retval IO delay

**/
MRC_STATUS
GetSetRxDelay (
              PSYSHOST  host,
              UINT8     socket,
              UINT8     ch,
              UINT8     dimm,
              UINT8     rank,
              UINT8     strobe,
              UINT8     bit,
              GSM_LT    level,
              GSM_GT    group,
              UINT8     mode,
              INT16     *value
              )
{
  UINT8                               logRank;
  UINT8                               maxStrobe;
  UINT16                              curVal  = 0;
  UINT16                              curVal2 = 0;
  UINT16                              piDelay;
  UINT16                              piDelay2 = 0;
  INT16                               tmp;
  INT16                               tmp2 = 0;
  UINT16                              maxLimit = 0;
  UINT16                              minLimit = 0;
  UINT16                              usDelay = 0;
#ifdef SERIAL_DBG_MSG
  char                                strBuf0[128];
#endif  // SERIAL_DBG_MSG
  struct channelNvram                 (*channelNvList)[MAX_CH];
  RXGROUP0N0RANK0_0_MCIO_DDRIO_STRUCT rxGroup0;
#ifdef LRDIMM_SUPPORT
  UINT8                               tmpStrobe;
  UINT8                               *controlWordDataPtr;
  UINT8                               controlWordAddr;
  struct ddrRank                      (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                   (*rankStruct)[MAX_RANK_DIMM];
  UINT8                               updateNeeded = 0;
  struct dimmNvram                    (*dimmNvList)[MAX_DIMM];
#endif  //LRDIMM
  channelNvList = GetChannelNvList(host, socket);
#ifdef LRDIMM_SUPPORT
  rankStruct  = GetRankStruct(host, socket, ch, dimm);
  rankList = GetRankNvList(host, socket, ch, dimm);
  dimmNvList = GetDimmNvList(host, socket, ch);
#endif  //LRDIMM

  rxGroup0.Data = 0;

  //
  // Get the logical rank #
  //
  logRank = GetLogicalRank(host, socket, ch, dimm, rank);

  //
  // Get minimum and maximum value
  //
  GetDataGroupLimits(host, level, group, &minLimit, &maxLimit, &usDelay);

  if (strobe == ALL_STROBES) {
    strobe    = 0;
    maxStrobe = MAX_STROBE;
  } else {
    maxStrobe = strobe + 1;
  }

  if (maxStrobe > MAX_STROBE) {
    return MRC_STATUS_STROBE_NOT_PRESENT;
  }

#ifdef LRDIMM_SUPPORT
  if (((*dimmNvList)[dimm].lrBuf_BC1x & LRDIMM_ONE_RANK_TIMING_MODE) && (level == LrbufLevel)) {
    rank = 0;
    logRank = GetLogicalRank(host, socket, ch, dimm, 0);
  }
#endif //LRDIMM_SUPPORT
  for (; strobe < maxStrobe; strobe++) {
#ifdef LRDIMM_SUPPORT
    if (level == LrbufLevel) {

      if (mode & GSM_READ_CSR) {
        //
        // Read from actual setting cache
        //
        switch (group) {
          case RxDqsDelay:
            if ((*rankList)[rank].lrBuf_FxBC4x5x[strobe] < 16) {
              curVal = (*rankList)[rank].lrBuf_FxBC4x5x[strobe] + 15;
            } else {
              curVal = (*rankList)[rank].lrBuf_FxBC4x5x[strobe] ^ 31;
            }
            break;
          case RecEnDelay:
              curVal = (*rankList)[rank].lrBuf_FxBC2x3x[strobe];
            break;
          case RecEnDelayCycle:
              curVal = (((*rankList)[rank].lrBuf_FxBCCxEx[strobe % 9] >> (4 * (strobe / 9))) & 0xF);
            break;
          default:
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                           "GetSetRxDelay called unsupported group!\n"));
            RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_58);
            break;
        } //switch group
      } else {
        //
        // Read from setting cache
        //
        switch (group) {
        case RxDqsDelay:
          if ((*rankStruct)[rank].cachedLrBuf_FxBC4x5x[strobe] < 16) {
            curVal = (*rankStruct)[rank].cachedLrBuf_FxBC4x5x[strobe] + 15;
          } else {
            curVal = (*rankStruct)[rank].cachedLrBuf_FxBC4x5x[strobe] ^ 31;
          }
          break;
        case RecEnDelay:
          curVal = (*rankStruct)[rank].cachedLrBuf_FxBC2x3x[strobe];
          break;
        case RecEnDelayCycle:
          curVal = (((*rankStruct)[rank].cachedLrBuf_FxBCCxEx[strobe % 9]>>(4 * (strobe / 9))) & 0xF);
          break;
        default:
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                         "GetSetRxDelay called unsupported group!\n"));
          RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_59);
          break;
        } //switch group
      }
    } else
#endif //LRDIMM
    {
      if (mode & GSM_READ_CSR) {
        //
        // Read from PCI config space
        //
        rxGroup0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, RXGROUP0N0RANK0_0_MCIO_DDRIO_REG));

      } else {
        //
        // Read from cache
        //
        rxGroup0.Data = (*channelNvList)[ch].rxGroup0[logRank][strobe];
      }

      switch (group) {
      case RecEnDelay:
        curVal = (UINT16)rxGroup0.Bits.rcvendelay;
        break;

      case RxDqsPDelay:
        curVal = (UINT16)rxGroup0.Bits.rxdqspdelay;
        break;

      case RxDqsNDelay:
        curVal = (UINT16)rxGroup0.Bits.rxdqsndelay;
        break;

      case RxDqsDelay:
        curVal  = (UINT16)rxGroup0.Bits.rxdqspdelay;
        curVal2 = (UINT16)rxGroup0.Bits.rxdqsndelay;
        break;

      case RxDqDelay:
        curVal = (UINT16)rxGroup0.Bits.rxdqdelay;
        break;

      case RxDqDqsDelay:
        curVal = (UINT16)rxGroup0.Bits.rxdqspdelay + (5-(UINT16)rxGroup0.Bits.rxdqdelay)*8;
        break;

      case RxEq:
        curVal = (UINT16)rxGroup0.Bits.rxeq;
        break;
      default:
        break;
      } // switch
    }

    //
    // Check if read only
    //
    if (mode & GSM_READ_ONLY) {
      //
      // Save the value read into the output parameter
      //
      *value = curVal;
    } else {
      //
      // Write
      //
      // Adjust the current CMD delay value by offset
      if (mode & GSM_WRITE_OFFSET) {
        //
        // Add the offset to the current value
        //
        tmp = curVal + *value;
        if(group == RxDqsDelay) {
          tmp2 = curVal2 + *value;
        }

      } else {
        //
        // Write absolute value
        //
        tmp = *value;
        if(group == RxDqsDelay) {
          tmp2 = *value;
        }
      }

      //
      // Make sure we do not exeed the limits
      //
      if (tmp >= minLimit) {
        piDelay = tmp;
      } else {
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                       "%s Out of range!! tmp = 0x%x, minLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), tmp, minLimit));
        piDelay = minLimit;
      }
      // Ensure we do not exceed maximums
      if (piDelay > maxLimit) {
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                       "%s Out of range!! piDelay = 0x%x, maxLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), piDelay, maxLimit));
        piDelay = maxLimit;
      }

      if ((group == RxDqsDelay) && (level == DdrLevel)) {
        if (tmp2 >= minLimit) {
          piDelay2 = tmp2;
        } else {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                       "%s Out of range!! tmp2 = 0x%x, minLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), tmp2, minLimit));
          piDelay2 = minLimit;
        }
        // Ensure we do not exceed maximums
        if (piDelay2 > maxLimit) {
          MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                       "%s Out of range!! piDelay2 = 0x%x, maxLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), piDelay2, maxLimit));
          piDelay2 = maxLimit;
        }
      }

      if (level == DdrLevel) {
        switch (group) {
        case RecEnDelay:
          rxGroup0.Bits.rcvendelay = piDelay;
          break;

        case RxDqsPDelay:
          rxGroup0.Bits.rxdqspdelay = piDelay;
          break;

        case RxDqsNDelay:
          rxGroup0.Bits.rxdqsndelay = piDelay;
          break;

        case RxDqsDelay:
          rxGroup0.Bits.rxdqspdelay = piDelay;
          rxGroup0.Bits.rxdqsndelay = piDelay2;
          break;

        case RxDqDelay:
          rxGroup0.Bits.rxdqdelay = piDelay;
          break;

        case RxDqDqsDelay:
          if ( piDelay>71 ) {
            rxGroup0.Bits.rxdqdelay = 0;
            rxGroup0.Bits.rxdqspdelay = piDelay-40;
            rxGroup0.Bits.rxdqsndelay = piDelay-40;
          } else if (piDelay>63) {
            rxGroup0.Bits.rxdqdelay = 1;
            rxGroup0.Bits.rxdqspdelay = piDelay-32;
            rxGroup0.Bits.rxdqsndelay = piDelay-32;
          } else if (piDelay>55) {
            rxGroup0.Bits.rxdqdelay = 2;
            rxGroup0.Bits.rxdqspdelay = piDelay-24;
            rxGroup0.Bits.rxdqsndelay = piDelay-24;
          } else if (piDelay>47) {
            rxGroup0.Bits.rxdqdelay = 3;
            rxGroup0.Bits.rxdqspdelay = piDelay-16;
            rxGroup0.Bits.rxdqsndelay = piDelay-16;
          } else if (piDelay>39) {
            rxGroup0.Bits.rxdqdelay = 4;
            rxGroup0.Bits.rxdqspdelay = piDelay-8;
            rxGroup0.Bits.rxdqsndelay = piDelay-8;
          } else {
            rxGroup0.Bits.rxdqdelay = 5;
            rxGroup0.Bits.rxdqspdelay = piDelay;
            rxGroup0.Bits.rxdqsndelay = piDelay;
          }
          break;

        case RxEq:
          rxGroup0.Bits.rxeq = piDelay;
        default:
          break;
        } // switch
      }

#ifdef LRDIMM_SUPPORT
      if (level == LrbufLevel) {
        //
        // first translate encoded value to register value
        //
        //curVal = piDelay >> 1;
        curVal = piDelay;

        switch (group) {
          case RxDqsDelay:
            if (curVal < 16) {
              if ((*rankList)[rank].lrBuf_FxBC4x5x[strobe] != (UINT8) (curVal ^ 31)) {
                updateNeeded = 1;
                (*rankList)[rank].lrBuf_FxBC4x5x[strobe] = (UINT8) (curVal ^ 31);
              }
            } else {
              if ((*rankList)[rank].lrBuf_FxBC4x5x[strobe] != (UINT8) (curVal - 15)) {
                updateNeeded = 1;
                (*rankList)[rank].lrBuf_FxBC4x5x[strobe] = (UINT8) (curVal - 15);
              }
            }
            break;
          case RecEnDelay:
            if ((*rankList)[rank].lrBuf_FxBC2x3x[strobe] != (UINT8) curVal) {
              updateNeeded = 1;
              (*rankList)[rank].lrBuf_FxBC2x3x[strobe] = (UINT8) curVal;
            }
            break;
          case RecEnDelayCycle:
            if ((((*rankList)[rank].lrBuf_FxBCCxEx[strobe % 9]>>(4 * (strobe / 9))) & 0xF) != (UINT8) curVal) {
              updateNeeded = 1;
              (*rankList)[rank].lrBuf_FxBCCxEx[strobe % 9] &= 0xF0 >> (4 * (strobe / 9));
              (*rankList)[rank].lrBuf_FxBCCxEx[strobe % 9] |= (UINT8) curVal<<(4 * (strobe / 9));
            }
            break;
          default:
            MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, NO_BIT,
                           "GetSetRxDelay called unsupported group!\n"));
            RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_60);
            break;
        } //switch group
      } else
#endif  //LRDIMM
      {
        //
        // Write it back if the current data does not equal the cache value or if in force write mode
        //
        if ((rxGroup0.Data != (*channelNvList)[ch].rxGroup0[logRank][strobe]) || (mode & GSM_FORCE_WRITE)) {

          MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, RXGROUP0N0RANK0_0_MCIO_DDRIO_REG), rxGroup0.Data);

          //
          // Wait for the new value to settle
          //
          FixedDelay(host, usDelay);
        }
      }
    } // if read only

    //
    // Save to cache
    //
    if (mode & GSM_UPDATE_CACHE) {
      if (level == DdrLevel) {
        (*channelNvList)[ch].rxGroup0[logRank][strobe] = rxGroup0.Data;
      }
    }

  } // strobe loop

#ifdef LRDIMM_SUPPORT
  if ((level == LrbufLevel) && !(mode & GSM_READ_ONLY) && ((updateNeeded) || (mode & GSM_FORCE_WRITE))) {
    switch (group) {
      case RxDqsDelay:
        controlWordDataPtr = &((*rankList)[rank].lrBuf_FxBC4x5x[0]);
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, rank, LRDIMM_BC4x, ALL_DATABUFFERS);
        controlWordDataPtr = &((*rankList)[rank].lrBuf_FxBC4x5x[9]);
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, rank, LRDIMM_BC5x, ALL_DATABUFFERS);
        break;
      case RecEnDelay:
        controlWordDataPtr = &((*rankList)[rank].lrBuf_FxBC2x3x[0]);
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, rank, LRDIMM_BC2x, ALL_DATABUFFERS);
        controlWordDataPtr = &((*rankList)[rank].lrBuf_FxBC2x3x[9]);
        WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, rank, LRDIMM_BC3x, ALL_DATABUFFERS);
        break;
      case RecEnDelayCycle:
        controlWordDataPtr = &((*rankList)[rank].lrBuf_FxBCCxEx[0]);
        controlWordAddr = LRDIMM_BCCx + ((rank >> 1) * 0x20);
        WriteLrbufPBA(host, socket, ch, dimm, 0, controlWordDataPtr, (rank & BIT0) , controlWordAddr, ALL_DATABUFFERS);
        break;
      default:
        break;
    } // end switch
    //
    // Wait for the new value to settle
    //
    FixedDelay(host, usDelay);
  }

  if (mode & GSM_UPDATE_CACHE) {
    if (level == LrbufLevel) {
      switch (group) {
        case RxDqsDelay:
          for (tmpStrobe = 0; tmpStrobe < MAX_STROBE; tmpStrobe++) {
            (*rankStruct)[rank].cachedLrBuf_FxBC4x5x[tmpStrobe] =  (*rankList)[rank].lrBuf_FxBC4x5x[tmpStrobe];
          }
          break;
        case RecEnDelay:
          for (tmpStrobe = 0; tmpStrobe < MAX_STROBE; tmpStrobe++) {
            (*rankStruct)[rank].cachedLrBuf_FxBC2x3x[tmpStrobe] =  (*rankList)[rank].lrBuf_FxBC2x3x[tmpStrobe];
          }
          break;
        case RecEnDelayCycle:
          for (tmpStrobe = 0; tmpStrobe < MAX_STROBE/2; tmpStrobe++) {
            (*rankStruct)[rank].cachedLrBuf_FxBCCxEx[tmpStrobe] =  (*rankList)[rank].lrBuf_FxBCCxEx[tmpStrobe];
          }
        break;
        default:
          break;
      }
    }
  }
#endif //LRDIMM

  return MRC_STATUS_SUCCESS;
} // GetSetRxDelay

/**

  Get RxDelay

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)
  @param strobe  - Strobe number (0-based)
  @param bit     - Bit number
  @param level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  @param group   - CSM_GT - Parameter to program
  @param mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  @param value   - Data to program

  @retval IO delay

**/
MRC_STATUS
GetSetRxDelayBit (
                 PSYSHOST  host,
                 UINT8     socket,
                 UINT8     ch,
                 UINT8     dimm,
                 UINT8     rank,
                 UINT8     strobe,
                 UINT8     bit,
                 GSM_LT    level,
                 GSM_GT    group,
                 UINT8     mode,
                 INT16     *value
                 )
{
  UINT8                               logRank;
  UINT8                               maxStrobe;
  UINT8                               i;
  UINT16                              curVal = 0;
  UINT16                              piDelayNibble;
  UINT16                              piDelay;
  UINT16                              piDelayMin;
  UINT16                              piDelayMax;
  UINT16                              delta;
  UINT16                              piDelayBit[4];
  UINT8                               clkSel;
  INT16                               tmp;
  UINT16                              maxLimit = 0;
  UINT16                              minLimit = 0;
  UINT16                              usDelay = 0;
#ifdef SERIAL_DBG_MSG
  char                                strBuf0[128];
#endif  // SERIAL_DBG_MSG
  struct channelNvram                 (*channelNvList)[MAX_CH];
  RXGROUP0N0RANK0_0_MCIO_DDRIO_STRUCT rxGroup0;
  RXGROUP1N0RANK0_0_MCIO_DDRIO_STRUCT rxGroup1;
  RXOFFSETN0RANK0_0_MCIO_DDRIO_STRUCT rxOffset;

  channelNvList = GetChannelNvList(host, socket);

  rxGroup1.Data = 0;

  //
  // Get the logical rank #
  //
  logRank = GetLogicalRank(host, socket, ch, dimm, rank);

  //
  // Get minimum and maximum value
  //
  GetDataGroupLimits(host, level, group, &minLimit, &maxLimit, &usDelay);

  if (strobe == ALL_STROBES) {
    strobe    = 0;
    maxStrobe = MAX_STROBE;
  } else {
    maxStrobe = strobe + 1;
  }

  if (maxStrobe > MAX_STROBE) {
    return MRC_STATUS_STROBE_NOT_PRESENT;
  }

  //Assert on error; should never get here!
  RC_ASSERT(bit < 4, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_61);

  for (; strobe < maxStrobe; strobe++) {

    if (mode & GSM_READ_CSR) {
      //
      // Read from PCI config space
      //
      rxGroup0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, RXGROUP0N0RANK0_0_MCIO_DDRIO_REG));
      rxGroup1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, RXGROUP1N0RANK0_0_MCIO_DDRIO_REG));
      rxOffset.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, RXOFFSETN0RANK0_0_MCIO_DDRIO_REG));

    } else {
      //
      // Read from cache
      //
      rxGroup0.Data = (*channelNvList)[ch].rxGroup0[logRank][strobe];
      rxGroup1.Data = (*channelNvList)[ch].rxGroup1[logRank][strobe];
      rxOffset.Data = (*channelNvList)[ch].rxOffset[logRank][strobe];
    }
    
    if (strobe>=9){
        switch (group) {
        case RxDqsBitDelay:
        case RxDqsPBitDelay:
          piDelayNibble = (UINT16)rxGroup0.Bits.rxdqspdelay;
          piDelayBit[0] = (UINT16)rxGroup1.Bits.rxdqspdelay2;
          piDelayBit[1] = (UINT16)rxGroup1.Bits.rxdqspdelay3;
          piDelayBit[2] = (UINT16)rxGroup1.Bits.rxdqspdelay0;
          piDelayBit[3] = (UINT16)rxGroup1.Bits.rxdqspdelay1;
          break;
        case RxDqsNBitDelay:
          piDelayNibble = (UINT16)rxGroup0.Bits.rxdqsndelay;
          piDelayBit[0] = (UINT16)rxGroup1.Bits.rxdqsndelay2;
          piDelayBit[1] = (UINT16)rxGroup1.Bits.rxdqsndelay3;
          piDelayBit[2] = (UINT16)rxGroup1.Bits.rxdqsndelay0;
          piDelayBit[3] = (UINT16)rxGroup1.Bits.rxdqsndelay1;
          break;
        default:
          piDelayNibble = 0;
          piDelayBit[0] = 0;
          piDelayBit[1] = 0;
          piDelayBit[2] = 0;
          piDelayBit[3] = 0;
          break;
        }
        for (i = 0; i < 4; i++) {
          piDelayBit[i] += piDelayNibble;
        }
    } else {
        switch (group) {
        case RxDqsBitDelay:
        case RxDqsPBitDelay:
          piDelayNibble = (UINT16)rxGroup0.Bits.rxdqspdelay;
          piDelayBit[0] = (UINT16)rxGroup1.Bits.rxdqspdelay0;
          piDelayBit[1] = (UINT16)rxGroup1.Bits.rxdqspdelay1;
          piDelayBit[2] = (UINT16)rxGroup1.Bits.rxdqspdelay2;
          piDelayBit[3] = (UINT16)rxGroup1.Bits.rxdqspdelay3;
          break;
        case RxDqsNBitDelay:
          piDelayNibble = (UINT16)rxGroup0.Bits.rxdqsndelay;
          piDelayBit[0] = (UINT16)rxGroup1.Bits.rxdqsndelay0;
          piDelayBit[1] = (UINT16)rxGroup1.Bits.rxdqsndelay1;
          piDelayBit[2] = (UINT16)rxGroup1.Bits.rxdqsndelay2;
          piDelayBit[3] = (UINT16)rxGroup1.Bits.rxdqsndelay3;
          break;
        default:
          piDelayNibble = 0;
          piDelayBit[0] = 0;
          piDelayBit[1] = 0;
          piDelayBit[2] = 0;
          piDelayBit[3] = 0;
          break;
        }
        for (i = 0; i < 4; i++) {
          piDelayBit[i] += piDelayNibble;
        }
    }
    curVal = piDelayBit[bit];

    //
    // Check if read only
    //
    if (mode & GSM_READ_ONLY) {
      //
      // Save the value read into the output parameter
      //
      *value = curVal;
    } else {
      //
      // Write
      //
      // Adjust the current CMD delay value by offset
      if (mode & GSM_WRITE_OFFSET) {
        //
        // Add the offset to the current value
        //
        tmp = curVal + *value;

      } else {
        //
        // Write absolute value
        //
        tmp = *value;
      }

      //
      // Make sure we do not exeed the limits
      //
      if (tmp >= minLimit) {
        piDelay = tmp;
      } else {
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, bit,
          "%s Out of range!! tmp = 0x%x, minLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), tmp, minLimit));
        piDelay = minLimit;
      }
      // Ensure we do not exceed maximums
      if (piDelay > maxLimit) {
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, strobe, bit,
          "%s Out of range!! piDelay = 0x%x, maxLimit = 0x%x\n", GetGroupStr(group, (char *)&strBuf0), piDelay, maxLimit));
        piDelay = maxLimit;
      }

      // Adjust target bit delay
      piDelayBit[bit] = piDelay;

      // Normalize smallest per bit delay to 0
      piDelayMax = 0;
      piDelayMin = 0xFF;
      clkSel = 0;
      for (i = 0; i < 4; i++) {
        if (piDelayBit[i] > piDelayMax) {
          piDelayMax = piDelayBit[i];
          clkSel = i;
        }
        if (piDelayBit[i] < piDelayMin) {
          piDelayMin = piDelayBit[i];
        }
      }
      delta = piDelayMin - piDelayNibble;
      piDelayNibble += delta;
      for (i = 0; i < 4; i++) {
        piDelayBit[i] = piDelayBit[i] - piDelayNibble;
        if (piDelayBit[i] > 15) piDelayBit[i] = 15;
      }

      for (i = 0; i < 4; i++) {
        piDelay = piDelayBit[i];
        if (strobe>=9){
          piDelay = piDelayBit[(i+2)%4];
        }
        
        switch (group) {
        case RxDqsBitDelay:
          rxGroup0.Bits.rxdqspdelay = piDelayNibble;
          rxGroup0.Bits.rxdqsndelay = piDelayNibble;
          switch (i) {
          case 0:
            rxGroup1.Bits.rxdqspdelay0 = piDelay;
            rxGroup1.Bits.rxdqsndelay0 = piDelay;
            break;
          case 1:
            rxGroup1.Bits.rxdqspdelay1 = piDelay;
            rxGroup1.Bits.rxdqsndelay1 = piDelay;
            break;
          case 2:
            rxGroup1.Bits.rxdqspdelay2 = piDelay;
            rxGroup1.Bits.rxdqsndelay2 = piDelay;
            break;
          case 3:
            rxGroup1.Bits.rxdqspdelay3 = piDelay;
            rxGroup1.Bits.rxdqsndelay3 = piDelay;
            break;
          } // switch i
          break;
        case RxDqsPBitDelay:
          rxGroup0.Bits.rxdqspdelay = piDelayNibble;
          switch (i) {
          case 0:
            rxGroup1.Bits.rxdqspdelay0 = piDelay;
            break;
          case 1:
            rxGroup1.Bits.rxdqspdelay1 = piDelay;
            break;
          case 2:
            rxGroup1.Bits.rxdqspdelay2 = piDelay;
            break;
          case 3:
            rxGroup1.Bits.rxdqspdelay3 = piDelay;
            break;
          } // switch i
          break;
        case RxDqsNBitDelay:
          rxGroup0.Bits.rxdqsndelay = piDelayNibble;
          switch (i) {
          case 0:
            rxGroup1.Bits.rxdqsndelay0 = piDelay;
            break;
          case 1:
            rxGroup1.Bits.rxdqsndelay1 = piDelay;
            break;
          case 2:
            rxGroup1.Bits.rxdqsndelay2 = piDelay;
            break;
          case 3:
            rxGroup1.Bits.rxdqsndelay3 = piDelay;
            break;
          } // switch i
          break;
        default:
          break;
        } //switch group
      } //for i

      // Update mux select to reflect largest per bit offset

        if (strobe>=9){
          clkSel = (clkSel+2)%4;
        }
      switch (group) {
      case RxDqsBitDelay:
        rxOffset.Bits.rxdqspsel = clkSel;
        rxOffset.Bits.rxdqsnsel = clkSel;
        break;
      case RxDqsPBitDelay:
        rxOffset.Bits.rxdqspsel = clkSel;
        break;
      case RxDqsNBitDelay:
        rxOffset.Bits.rxdqsnsel = clkSel;
        break;
      default:
        break;
      }

      //
      // Write it back if the current data does not equal the cache value or if in force write mode
      //
      if ((rxGroup0.Data != (*channelNvList)[ch].rxGroup0[logRank][strobe]) || (mode & GSM_FORCE_WRITE)) {
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, RXGROUP0N0RANK0_0_MCIO_DDRIO_REG), rxGroup0.Data);
      }
      if ((rxGroup1.Data != (*channelNvList)[ch].rxGroup1[logRank][strobe]) || (mode & GSM_FORCE_WRITE)) {
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, RXGROUP1N0RANK0_0_MCIO_DDRIO_REG), rxGroup1.Data);
      }
      if ((rxOffset.Data != (*channelNvList)[ch].rxOffset[logRank][strobe]) || (mode & GSM_FORCE_WRITE)) {
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, RXOFFSETN0RANK0_0_MCIO_DDRIO_REG), rxOffset.Data);
      }

      //
      // Wait for the new value to settle
      //
      FixedDelay(host, usDelay);

    } // if read only

    //
    // Save to cache
    //
    if (mode & GSM_UPDATE_CACHE) {
      (*channelNvList)[ch].rxGroup0[logRank][strobe] = rxGroup0.Data;
      (*channelNvList)[ch].rxGroup1[logRank][strobe] = rxGroup1.Data;
      (*channelNvList)[ch].rxOffset[logRank][strobe] = rxOffset.Data;
    }
  } // strobe loop

  return MRC_STATUS_SUCCESS;
} // GetSetRxDelayBit

/**

  Get RxDelay

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)
  @param strobe  - Strobe number (0-based)
  @param bit     - Bit number
  @param level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  @param group   - CSM_GT - Parameter to program
  @param mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  @param value   - Data to program

  @retval IO delay

**/
MRC_STATUS
GetSetRxVref (
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             UINT8     dimm,
             UINT8     rank,
             UINT8     strobe,
             UINT8     bit,
             GSM_LT    level,
             GSM_GT    group,
             UINT8     mode,
             INT16     *value
             )
{
  UINT8                             maxStrobe;
  UINT8                             bit2;
  UINT16                            curVal = 0;
  UINT16                            piDelay;
  INT16                             tmp;
  UINT16                            maxLimit = 127;
  UINT16                            minLimit = 0;
  UINT16                            usDelay = 1;
  struct channelNvram               (*channelNvList)[MAX_CH];
  RXVREFCTRLN0_0_MCIO_DDRIO_STRUCT  rxVrefCtrl;
  RXVREFCTRLN0_0_MCIO_DDRIO_STRUCT  rxVrefCtrln1;
  UINT8                             vrefValue[MAX_STROBE/2];
  UINT8                             vrefSelect[MAX_STROBE/2];
#ifdef LRDIMM_SUPPORT
  UINT8                             *controlWordDataPtr;
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  UINT8                             updateNeeded = 0;
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
#endif //LRDIMM
#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  countTrackingData(host, VREF_MOVE, 1);
#endif
#endif  //  DEBUG_PERFORMANCE_STATS

  channelNvList = GetChannelNvList(host, socket);
#ifdef LRDIMM_SUPPORT
  rankStruct  = GetRankStruct(host, socket, ch, dimm);
  rankList = GetRankNvList(host, socket, ch, dimm);
#endif //LRDIMM

  rxVrefCtrl.Data = 0;
  rxVrefCtrln1.Data = 0;

  //
  // Get the logical rank #
  //

  //
  // Get minimum and maximum value
  //
  GetDataGroupLimits(host, level, group, &minLimit, &maxLimit, &usDelay);

  if (strobe == ALL_STROBES) {
    strobe    = 0;
    maxStrobe = MAX_STROBE;
  } else {
    maxStrobe = strobe + 1;
    if (bit == ALL_BITS) {
      MemCheckIndex(host, strobe, MAX_STROBE/2);
    } else {
      MemCheckIndex(host, strobe, MAX_STROBE);
    }
  }

  if (maxStrobe > MAX_STROBE) {
    return MRC_STATUS_STROBE_NOT_PRESENT;
  }

  for (; strobe < maxStrobe; strobe++) {
    // HSD 5330942 
    bit2 = bit;
    // if ((bit != ALL_BITS) && (strobe>=9)){
      // bit2 = (bit+2)%4; //bits 0,1 and 2,3 swapped in upper nibble for each byte
    // } 

    if (mode & GSM_READ_CSR) {
#ifdef LRDIMM_SUPPORT
      if (level == LrbufLevel) {
        curVal = (*rankList)[rank].lrbufRxVref[strobe];
      } else
#endif //LRDIMM
      {
        //
        // Read from PCI config space
        //
        rxVrefCtrl.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe%9, RXVREFCTRLN0_0_MCIO_DDRIO_REG));
        rxVrefCtrln1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, (strobe%9)+9, RXVREFCTRLN0_0_MCIO_DDRIO_REG));
      }
    } else {
      //
      // Read from cache
      //
#ifdef LRDIMM_SUPPORT
      if (level == LrbufLevel) {
        curVal = (*rankStruct)[rank].lrbufRxVrefCache[strobe];
      } else
#endif //LRDIMM
      {
        rxVrefCtrl.Data = (*channelNvList)[ch].rxVrefCtrl[strobe%9];
        rxVrefCtrln1.Data = (*channelNvList)[ch].rxVrefCtrl[(strobe%9)+9];
      }
    }

    if (level == DdrLevel) {
      if (bit2 == ALL_BITS) {
        // real BIT 0 Nibble0
        curVal = (UINT16)rxVrefCtrln1.Bits.vrefperbit0;

      } else {
        if (strobe < 9) {
          switch (bit2) {
            case 0:
              curVal = (UINT16)rxVrefCtrln1.Bits.vrefperbit0;
              break;

            case 1:
              curVal = (UINT16)rxVrefCtrln1.Bits.vrefperbit1;
              break;

            case 2:
              curVal = (UINT16)rxVrefCtrl.Bits.vrefperbit2;
              break;

            case 3:
              curVal = (UINT16)rxVrefCtrl.Bits.vrefperbit3;
              break;
          } // switch (bit2)
        } else {
          switch (bit2) {
            case 0:
              curVal = (UINT16)rxVrefCtrln1.Bits.vrefperbit2;
              break;

            case 1:
              curVal = (UINT16)rxVrefCtrln1.Bits.vrefperbit3;
              break;

            case 2:
              curVal = (UINT16)rxVrefCtrl.Bits.vrefperbit0;
              break;

            case 3:
              curVal = (UINT16)rxVrefCtrl.Bits.vrefperbit1;
              break;
          } // switch (bit2)
        }
      }
    } // DdrLevel

    //
    // Check if read only
    //
    if (mode & GSM_READ_ONLY) {
      //
      // Save the value read into the output parameter
      //
      *value = curVal;
    } else {
      //
      // Write
      //
      // Adjust the current CMD delay value by offset
      if (mode & GSM_WRITE_OFFSET) {
        //
        // Add the offset to the current value
        //
        tmp = curVal + *value;

        //
        // Make sure we do not exeed the limits
        //
        if (tmp > minLimit) {
          piDelay = tmp;
        } else {
          piDelay = minLimit;
        }
        // Ensure we do not exceed maximums
        if (piDelay > maxLimit) {
          piDelay = maxLimit;
        }
      } else {
        //
        // Write absolute value
        //
        piDelay = *value;
      }

      if (bit2 == ALL_BITS) {
          rxVrefCtrln1.Bits.vrefperbit0 = piDelay;
          rxVrefCtrln1.Bits.vrefperbit1 = piDelay;
          rxVrefCtrl.Bits.vrefperbit2 = piDelay;
          rxVrefCtrl.Bits.vrefperbit3 = piDelay;
          rxVrefCtrln1.Bits.vrefperbit2 = piDelay;
          rxVrefCtrln1.Bits.vrefperbit3 = piDelay;
          rxVrefCtrl.Bits.vrefperbit0 = piDelay;
          rxVrefCtrl.Bits.vrefperbit1 = piDelay;
        
      } else {
        if (strobe < 9) {
          switch (bit2) {
            case 0:
              rxVrefCtrln1.Bits.vrefperbit0 = piDelay;
              break;

            case 1:
              rxVrefCtrln1.Bits.vrefperbit1 = piDelay;
              break;

            case 2:
              rxVrefCtrl.Bits.vrefperbit2 = piDelay;
              break;

            case 3:
              rxVrefCtrl.Bits.vrefperbit3 = piDelay;
              break;
          } // switch (bit2)
        } else {
          switch (bit2) {
            case 0:
              rxVrefCtrln1.Bits.vrefperbit2 = piDelay;
              break;

            case 1:
              rxVrefCtrln1.Bits.vrefperbit3 = piDelay;
              break;

            case 2:
              rxVrefCtrl.Bits.vrefperbit0 = piDelay;
              break;

            case 3:
              rxVrefCtrl.Bits.vrefperbit1 = piDelay;
              break;
          } // switch (bit2)
        }
      }

#ifdef LRDIMM_SUPPORT
      if (level == LrbufLevel) {
        //
        // Write it back if the current data does not equal the cache value or if in force write mode
        //
        if ((piDelay != (*rankList)[rank].lrbufRxVref[strobe]) || (mode & GSM_FORCE_WRITE)) {
          //(*channelNvList)[ch].lrbufRxVref[rank][strobe] = (UINT8) piDelay;
          (*rankList)[rank].lrbufRxVref[strobe] = (UINT8) piDelay;
          updateNeeded = 1;
        }
      } else
#endif //LRDIMM
      {
        //
        // Write it back if the current data does not equal the cache value or if in force write mode
        //
        if ((rxVrefCtrl.Data != (*channelNvList)[ch].rxVrefCtrl[strobe%9]) || 
        (rxVrefCtrln1.Data != (*channelNvList)[ch].rxVrefCtrl[(strobe%9)+9]) || (mode & GSM_FORCE_WRITE)) {

          MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe%9, RXVREFCTRLN0_0_MCIO_DDRIO_REG), rxVrefCtrl.Data);
          MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, (strobe%9)+9, RXVREFCTRLN0_0_MCIO_DDRIO_REG), rxVrefCtrln1.Data);
          //
          // Wait for the new value to settle
          //
          FixedDelay(host, usDelay);
        }
      }
    } // if read only

    //
    // Save to cache
    //
    if (mode & GSM_UPDATE_CACHE) {
#ifdef LRDIMM_SUPPORT
      if (level == LrbufLevel) {
        (*rankStruct)[rank].lrbufRxVrefCache[strobe] = (*rankList)[rank].lrbufRxVref[strobe];
      } else
#endif //LRDIMM
      {
        (*channelNvList)[ch].rxVrefCtrl[strobe%9] = rxVrefCtrl.Data;
        (*channelNvList)[ch].rxVrefCtrl[(strobe%9)+9] = rxVrefCtrln1.Data;
      }
    }
  } // strobe loop

#ifdef LRDIMM_SUPPORT
  if ((level == LrbufLevel) && !(mode & GSM_READ_ONLY) && ((updateNeeded) || (mode & GSM_FORCE_WRITE))) {
    //
    //Note: because Rx vref is shared between upper and lower nibbles on the backside,
    // we have to either pick 1 or do some average.  For now, just picking the lower nibble
    // TODO: check this
    //
    MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT, "DB RxVref to DRAM = %d\n", (*rankList)[rank].lrbufRxVref[0]));
    for (strobe = 0; strobe < MAX_STROBE / 2; strobe++) {
      if ((*rankList)[rank].lrbufRxVref[strobe] < DB_DRAM_VREF_RANGE2_OFFSET) {
        //Select range 2
        vrefSelect[strobe] = 0x5;
        vrefValue[strobe] = (*rankList)[rank].lrbufRxVref[strobe];
      }
      else {
        //Select range 1 and adjust encoding
        vrefSelect[strobe] = 0x1;
        vrefValue[strobe] = (*rankList)[rank].lrbufRxVref[strobe] - DB_DRAM_VREF_RANGE2_OFFSET;
      }
    }
    controlWordDataPtr = &vrefSelect[0];
    WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, LRDIMM_F6, LRDIMM_BC4x, ALL_DATABUFFERS);
    controlWordDataPtr = &vrefValue[0];
    WriteLrbufPBA(host, socket, ch, dimm, rank, controlWordDataPtr, LRDIMM_F5, LRDIMM_BC6x, ALL_DATABUFFERS);

    //
    // Wait for the new value to settle
    //
    FixedDelay(host, usDelay);
  }
#endif //LRDIMM

  return MRC_STATUS_SUCCESS;
} // GetSetRxVref

/**

  GetSetTxVrefFnv

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)
  @param strobe  - Strobe number (0-based)
  @param bit     - Bit number
  @param level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  @param group   - CSM_GT - Parameter to program
  @param mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  @param value   - Data to program

  @retval status

**/
MRC_STATUS
GetSetTxVrefFnv (
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             UINT8     dimm,
             UINT8     rank,
             UINT8     strobe,
             UINT8     bit,
             GSM_LT    level,
             GSM_GT    group,
             UINT8     mode,
             INT16     *value
             )
{
  UINT8                             maxStrobe;
  UINT16                            curVal = 0;
  UINT16                            newVref;
  INT16                             tmp;
  UINT16                            maxLimit = 127;
  UINT16                            minLimit = 0;
  UINT16                            usDelay = 1;
  UINT8                             mask = 0;
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct rankDevice                 (*rankStruct)[MAX_RANK_DIMM];
  RXVREFCTRLN0_0_FNV_DDRIO_COMP_STRUCT  txVrefCtrl;
#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  countTrackingData(host, VREF_MOVE, 1);
#endif
#endif  //  DEBUG_PERFORMANCE_STATS

  rankList = GetRankNvList(host, socket, ch, dimm);
  rankStruct  = GetRankStruct(host, socket, ch, dimm);
  txVrefCtrl.Data = 0;

  if (bit == 0xFF) {
    mask = 0xF;
  } else {
    mask = 1 << bit;
  }
  //
  // Get minimum and maximum value
  //
  GetDataGroupLimits(host, level, group, &minLimit, &maxLimit, &usDelay);
  UpdateDdrtGroupLimits(host, socket, ch, dimm, level, group, &minLimit, &maxLimit);

  if (strobe == ALL_STROBES) {
    strobe    = 0;
    maxStrobe = MAX_STROBE;
  } else {
    maxStrobe = strobe + 1;
  }

  if (maxStrobe > MAX_STROBE) {
    return MRC_STATUS_STROBE_NOT_PRESENT;
  }

  for (; strobe < maxStrobe; strobe++) {

    if (mode & GSM_READ_CSR) {
      {
        //
        // Read from PCI config space
        //
        // GetSetFnvIO
        GetSetFnvIO (host, socket, ch, dimm, UpdateIoRegisterCh (host, strobe, RXVREFCTRLN0_0_FNV_DDRIO_COMP_REG), &txVrefCtrl.Data ,0, FNV_IO_READ, mask);

      }
    } else {
      //
      // Read from cache
      //
      {
        txVrefCtrl.Data = (*rankStruct)[rank].lrbufTxVrefCache[strobe];
      }
    }

      if (bit == ALL_BITS) {
        curVal = (UINT16)txVrefCtrl.Bits.vrefperbit0;
      } else {
        switch (bit) {
        case 0:
          curVal = (UINT16)txVrefCtrl.Bits.vrefperbit0;
          break;

        case 1:
          curVal = (UINT16)txVrefCtrl.Bits.vrefperbit1;
          break;

        case 2:
          curVal = (UINT16)txVrefCtrl.Bits.vrefperbit2;
          break;

        case 3:
          curVal = (UINT16)txVrefCtrl.Bits.vrefperbit3;
          break;
        } // switch (bit)
      }

    //
    // Check if read only
    //
    if (mode & GSM_READ_ONLY) {
      //
      // Save the value read into the output parameter
      //
      *value = curVal;
    } else {
      //
      // Write
      //
      // Adjust the current CMD delay value by offset
      if (mode & GSM_WRITE_OFFSET) {
        //
        // Add the offset to the current value
        //
        tmp = curVal + *value;

        //
        // Make sure we do not exeed the limits
        //
        if (tmp > minLimit) {
          newVref = tmp;
        } else {
          newVref = minLimit;
        }
        // Ensure we do not exceed maximums
        if (newVref > maxLimit) {
          newVref = maxLimit;
        }
      } else {
        //
        // Write absolute value
        //
        newVref = *value;
      }

      if (bit == ALL_BITS) {
        txVrefCtrl.Bits.vrefperbit0 = newVref;
        txVrefCtrl.Bits.vrefperbit1 = newVref;
        txVrefCtrl.Bits.vrefperbit2 = newVref;
        txVrefCtrl.Bits.vrefperbit3 = newVref;
      } else {
        switch (bit) {
        case 0:
          txVrefCtrl.Bits.vrefperbit0 = newVref;
          break;

        case 1:
          txVrefCtrl.Bits.vrefperbit1 = newVref;
          break;

        case 2:
          txVrefCtrl.Bits.vrefperbit2 = newVref;
          break;

        case 3:
          txVrefCtrl.Bits.vrefperbit3 = newVref;
          break;
        } // switch (bit)
      }


      //
      // Write it back if the current data does not equal the cache value or if in force write mode
      //
      if ((txVrefCtrl.Data != (*rankList)[rank].lrbufTxVref[strobe]) || (mode & GSM_FORCE_WRITE)) {
        GetSetFnvIO (host, socket, ch, dimm, UpdateIoRegisterCh (host, strobe, RXVREFCTRLN0_0_FNV_DDRIO_COMP_REG), &txVrefCtrl.Data ,0, FNV_IO_WRITE, mask);
        //
        // Wait for the new value to settle
        //
        (*rankList)[rank].lrbufTxVref[strobe] = txVrefCtrl.Data;

        FixedDelay(host, usDelay);
      }
    } // if read only

    //
    // Save to cache
    //
    if (mode & GSM_UPDATE_CACHE) {
       (*rankStruct)[rank].lrbufTxVrefCache[strobe] = (*rankList)[rank].lrbufTxVref[strobe];
    }

  } // strobe loop


  return MRC_STATUS_SUCCESS;
} // GetSetTxVrefFnv
/**

  Get Tx Vref

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)
  @param strobe  - Strobe number (0-based)
  @param bit     - Bit number
  @param level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  @param group   - CSM_GT - Parameter to program
  @param mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  @param value   - Data to program

  @retval IO delay

**/
MRC_STATUS
GetSetTxVrefDDR4 (
                 PSYSHOST  host,
                 UINT8     socket,
                 UINT8     ch,
                 UINT8     dimm,
                 UINT8     rank,
                 UINT8     strobe,
                 UINT8     bit,
                 GSM_LT    level,
                 GSM_GT    group,
                 UINT8     mode,
                 INT16     *value
                 )
{
  UINT8               maxStrobe;
  UINT16              curVal = 0;
  INT16               newVref;
  INT16               tmp;
  UINT16              maxLimit = 0;
  UINT16              minLimit = 0;
  UINT16              usDelay = 0;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
#ifdef LRDIMM_SUPPORT
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];
  UINT8               tmpStrobe;
#endif //LRDIMM

  channelNvList = GetChannelNvList(host, socket);
  rankList = GetRankNvList(host, socket, ch, dimm);

#ifdef LRDIMM_SUPPORT
  rankStruct  = GetRankStruct(host, socket, ch, dimm);
#endif //LRDIMM

#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  countTrackingData(host, VREF_MOVE, 1);
#endif
#endif  //  DEBUG_PERFORMANCE_STATS

#ifdef LRDIMM_SUPPORT
  if (IsLrdimmPresent(host, socket, ch, dimm) && (rank > 0) && (level == DdrLevel)){
    rank=0;
  }
#endif //LRDIMM_SUPPORT

  //
  // Get minimum and maximum value
  //
  GetDataGroupLimits(host, level, group, &minLimit, &maxLimit, &usDelay);

  if ((host->setup.mem.optionsExt & PDA_EN) && ((host->nvram.mem.txVrefDone) || (level == LrbufLevel))) {

    if (strobe == ALL_STROBES) {
      strobe    = 0;
      maxStrobe = GetMaxStrobe(host, socket, ch, dimm, group, level);
    } else {
      maxStrobe = strobe + 1;
      if (maxStrobe > MAX_STROBE) {
        return MRC_STATUS_STROBE_NOT_PRESENT;
      }
    }

    for (; strobe < maxStrobe; strobe++) {
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

      if (mode & GSM_READ_CSR) {
#ifdef LRDIMM_SUPPORT
        if (level == LrbufLevel) {
          curVal = (UINT8) (*rankList)[rank].lrbufTxVref[strobe];
        } else
#endif  //LRDIMM
        {
          curVal = (*channelNvList)[ch].txVref[(*rankList)[rank].rankIndex][strobe];
        }
      } else {
#ifdef LRDIMM_SUPPORT
        if (level == LrbufLevel) {
          curVal = (UINT8) (*rankStruct)[rank].lrbufTxVrefCache[strobe];
        } else
#endif  //LRDIMM
        {
          curVal = (*channelNvList)[ch].txVrefCache[(*rankList)[rank].rankIndex][strobe];
        }
      } // if cached

      //
      // Check if read only
      //
      if (mode & GSM_READ_ONLY) {
        //
        // Save the value read into the output parameter
        //
        *value = curVal;
      } else {
        //
        // Write
        //
        if (mode & GSM_WRITE_OFFSET) {
          //
          // Add the offset to the current value
          //
          tmp = curVal + *value;

        } else {
          //
          // Write absolute value
          //
          tmp = *value;
        }

        //
        // Make sure we do not exeed the limits
        //
        if (tmp > minLimit) {
          newVref = tmp;
        } else {
          newVref = minLimit;
        }
        // Ensure we do not exceed maximums
        if (newVref > maxLimit) {
          newVref = maxLimit;
        }

#ifdef LRDIMM_SUPPORT
        if (level == LrbufLevel) {
          //
          // Write it back if the current data does not equal the current value or if in force write mode
          //
          if ((newVref != (UINT8) (*rankList)[rank].lrbufTxVref[strobe]) || (mode & GSM_FORCE_WRITE)) {

            (*rankList)[rank].lrbufTxVref[strobe] = (UINT8)newVref;

            //
            // Write the new vref
            //
            MemWriteDimmVref(host, socket, ch, dimm, rank, strobe, level, (UINT8)newVref);

          }
        } else
#endif  //LRDIMM
        {
          //
          // Write it back if the current data does not equal the current value or if in force write mode
          //
          if ((newVref != (*channelNvList)[ch].txVref[(*rankList)[rank].rankIndex][strobe]) || (mode & GSM_FORCE_WRITE)) {

            (*channelNvList)[ch].txVref[(*rankList)[rank].rankIndex][strobe] = (UINT8)newVref;

            //
            // Write the new vref
            //
            MemWriteDimmVref(host, socket, ch, dimm, rank, strobe, level, (UINT8)newVref);
          }
        }
      } // if read only

      //
      // Save to cache
      //
      if (mode & GSM_UPDATE_CACHE) {
#ifdef LRDIMM_SUPPORT
        if (level == LrbufLevel) {
          (*rankStruct)[rank].lrbufTxVrefCache[strobe] = (*rankList)[rank].lrbufTxVref[strobe];
        } else
#endif //LRDIMM
        {
          (*channelNvList)[ch].txVrefCache[(*rankList)[rank].rankIndex][strobe] = (*channelNvList)[ch].txVref[(*rankList)[rank].rankIndex][strobe];
        }
      }

      // Wait for Wr Vref circuit to settle
      FixedDelay(host, usDelay);
    } // strobe loop

    //
    // Make sure Vref settles
    //
    FixedDelay (host, 1);

  } else {
    if (mode & GSM_READ_CSR) {
#ifdef LRDIMM_SUPPORT
      if (level == LrbufLevel) {
        curVal = (UINT8) (*rankList)[rank].lrbufTxVref[0];
      } else
#endif //LRDIMM
      {
        curVal = (*channelNvList)[ch].txVref[(*rankList)[rank].rankIndex][0];
      }
    } else {
#ifdef LRDIMM_SUPPORT
      if (level == LrbufLevel) {
        curVal = (UINT8) (*rankStruct)[rank].lrbufTxVrefCache[0];
      } else
#endif //LRDIMM
      {
        curVal = (*channelNvList)[ch].txVrefCache[(*rankList)[rank].rankIndex][0];
      }
    } // if cached

    //
    // Check if read only
    //
    if (mode & GSM_READ_ONLY) {
      //
      // Save the value read into the output parameter
      //
      *value = curVal;
    } else {
      //
      // Write
      //
      if (mode & GSM_WRITE_OFFSET) {
        //
        // Add the offset to the current value
        //
        tmp = curVal + *value;

      } else {
        //
        // Write absolute value
        //
        tmp = *value;
      }

      //
      // Make sure we do not exeed the limits
      //
      if (tmp > minLimit) {
        newVref = tmp;
      } else {
        newVref = minLimit;
      }
      // Ensure we do not exceed maximums
      if (newVref > maxLimit) {
        newVref = maxLimit;
      }

#ifdef LRDIMM_SUPPORT
      if (level == LrbufLevel) {
        //
        // Write it back if the current data does not equal the current value or if in force write mode
        //
        if ((newVref != (UINT8) (*rankList)[rank].lrbufTxVref[0]) || (mode & GSM_FORCE_WRITE)) {

          for (tmpStrobe = 0; tmpStrobe < MAX_STROBE; tmpStrobe++) {
            (*rankList)[rank].lrbufTxVref[tmpStrobe] = (UINT8)newVref;
          } // strobe loop
          //
          // Write the new vref
          //
          MemWriteDimmVref(host, socket, ch, dimm, rank, 0, level, (UINT8)newVref);
        }
      } else
#endif //LRDIMM
      {
        //
        // Write it back if the current data does not equal the current value or if in force write mode
        //
        if ((newVref != (*channelNvList)[ch].txVref[(*rankList)[rank].rankIndex][0]) || (mode & GSM_FORCE_WRITE)) {

          for (strobe = 0; strobe < MAX_STROBE; strobe++) {
            (*channelNvList)[ch].txVref[(*rankList)[rank].rankIndex][strobe] = (UINT8)newVref;
          } // strobe loop

          //
          // Write the new vref
          //
          MemWriteDimmVref(host, socket, ch, dimm, rank, 0, level, (UINT8)newVref);

        }
      }
    } // if read only

    //
    // Save to cache
    //
    if (mode & GSM_UPDATE_CACHE) {
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
#ifdef LRDIMM_SUPPORT
        if (level == LrbufLevel) {
          (*rankStruct)[rank].lrbufTxVrefCache[strobe] = (*rankList)[rank].lrbufTxVref[strobe];
        } else
#endif //LRDIMM
        {
          (*channelNvList)[ch].txVrefCache[(*rankList)[rank].rankIndex][strobe] = (*channelNvList)[ch].txVref[(*rankList)[rank].rankIndex][strobe];
        }
      } // strobe loop
    }

    // Wait for Wr Vref circuit to settle
    FixedDelay(host, usDelay);
  }

  return MRC_STATUS_SUCCESS;
} // GetSetTxVrefDDR4

/**

  Get Imode

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param ch      - Channel number (0-based)
  @param strobe  - Strobe number (0-based)
  @param mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  @param value   - Data to program

  @retval IO delay

**/
MRC_STATUS
GetSetImode (
             PSYSHOST  host,
             UINT8     socket,
             UINT8     ch,
             UINT8     strobe,
             UINT8     mode,
             INT16     *value
             )
{
  UINT8                             maxStrobe;
  UINT16                            curVal = 0;
  UINT16                            piDelay;
  INT16                             tmp;
  UINT16                            maxLimit = 15;
  UINT16                            minLimit = 0;

  struct channelNvram               (*channelNvList)[MAX_CH];
  DATACONTROL2N0_0_MCIO_DDRIO_STRUCT    dataControl2;
  DATACONTROL3N0_0_MCIO_DDRIO_STRUCT    dataControl3;
  DATACONTROL2N0_0_MCIO_DDRIO_STRUCT    dataControl2n1;
  DATACONTROL3N0_0_MCIO_DDRIO_STRUCT    dataControl3n1;
  
  channelNvList = GetChannelNvList(host, socket);

  MemCheckIndex(host, strobe, MAX_STROBE/2);
  
  if (strobe == ALL_STROBES) {
    strobe    = 0;
    maxStrobe = MAX_STROBE/2;
  } else {
    maxStrobe = strobe + 1;
    if (maxStrobe > MAX_STROBE/2) {
      return MRC_STATUS_STROBE_NOT_PRESENT;
    }
  }

  for (; strobe < maxStrobe; strobe++) {
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

    if ((strobe+9) >= MAX_STROBE) {
      return MRC_STATUS_STROBE_NOT_PRESENT;
    }

    if(mode & GSM_READ_CSR) {
      //
      // Read from PCI config space
      //
      dataControl2.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG));
      dataControl2n1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, (strobe+9), DATACONTROL2N0_0_MCIO_DDRIO_REG));
      dataControl3.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL3N0_0_MCIO_DDRIO_REG));
      dataControl3n1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, (strobe+9), DATACONTROL3N0_0_MCIO_DDRIO_REG));
    } else {
      //
      // Read from cache
      //
      dataControl2.Data = (*channelNvList)[ch].dataControl2[strobe];
      dataControl2n1.Data = (*channelNvList)[ch].dataControl2[(strobe+9)];
      dataControl3.Data = (*channelNvList)[ch].dataControl3[strobe];
      dataControl3n1.Data = (*channelNvList)[ch].dataControl3[strobe+9];
    }

    curVal = (UINT16)dataControl2.Bits.imodeeqcode;
    //
    // Check if read only
    //
    if (mode & GSM_READ_ONLY) {
      //
      // Save the value read into the output parameter
      //
      *value = curVal;
    } else {
      //
      // Write
      //
      // Adjust the current CMD delay value by offset
      if (mode & GSM_WRITE_OFFSET) {
        //
        // Add the offset to the current value
        //
        tmp = curVal + *value;

        //
        // Make sure we do not exeed the limits
        //
        if (tmp > minLimit) {
          piDelay = tmp;
        } else {
          piDelay = minLimit;
        }
        // Ensure we do not exceed maximums
        if (piDelay > maxLimit) {
          piDelay = maxLimit;
        }
      } else {
        //
        // Write absolute value
        //
        piDelay = *value;
      }

      dataControl2.Bits.imodeeqcode = piDelay;
      dataControl2n1.Bits.imodeeqcode = piDelay;
      
      //
      // Write it back if the current data does not equal the cache value or if in force write mode
      //
      if ((dataControl2.Data != (*channelNvList)[ch].dataControl2[strobe]) || (mode & GSM_FORCE_WRITE)) {

        if (dataControl2.Bits.imodebiasen == 0 && *value != 0) {
          //
          // Enable Imode registers per byte (put this in GetSet? file BIOS sighting and justification is power savings)
          //
          dataControl2.Bits.imodebiasen = 1;
          dataControl2.Bits.imodeenable = 1;
          dataControl2n1.Bits.imodebiasen = 1;
          dataControl2n1.Bits.imodeenable = 1;
          dataControl3.Bits.imodebiasvrefen = 1;
          dataControl3n1.Bits.imodebiasvrefen = 1;
        }

        if (dataControl2.Bits.imodebiasen == 1 && *value == 0) {
          dataControl2.Bits.imodebiasen = 0;
          dataControl2.Bits.imodeenable = 0;
          dataControl2n1.Bits.imodebiasen = 0;
          dataControl2n1.Bits.imodeenable = 0;
          dataControl3.Bits.imodebiasvrefen = 0;
          dataControl3n1.Bits.imodebiasvrefen = 0;
        }
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG), dataControl2.Data);
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, (strobe+9), DATACONTROL2N0_0_MCIO_DDRIO_REG), dataControl2n1.Data);
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL3N0_0_MCIO_DDRIO_REG), dataControl3.Data);
        MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, (strobe+9), DATACONTROL3N0_0_MCIO_DDRIO_REG), dataControl3n1.Data);
      }
    } // if read only

    //
    // Save to cache
    //
    if (mode & GSM_UPDATE_CACHE) {
      (*channelNvList)[ch].dataControl2[strobe] = dataControl2.Data;
      (*channelNvList)[ch].dataControl2[(strobe+9)] = dataControl2n1.Data;
      (*channelNvList)[ch].dataControl3[strobe] = dataControl3.Data;
      (*channelNvList)[ch].dataControl3[(strobe+9)] = dataControl3n1.Data;
    }
  } // strobe loop

  return MRC_STATUS_SUCCESS;
} // GetSetImode

MRC_STATUS
GetSetCTLE (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     strobe,
  UINT8     bit,
  GSM_LT    level,
  GSM_GT    group,
  UINT8     mode,
  INT16     *value
  )
/*++

Routine Description:

  Get CTLE

Arguments:

  host    - Pointer to sysHost
  socket  - Socket number
  ch      - Channel number (0-based)
  dimm    - DIMM number (0-based)
  rank    - Rank number (0-based)
  strobe  - Strobe number (0-based)
  bit     - Bit number
  level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  group   - CSM_GT - Parameter to program
  mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  value   - Data to program

Returns:

  IO delay

--*/
{
  UINT8                             maxStrobe;
  UINT16                            curVal = 0;
  UINT16                            piDelay;
  INT16                             tmp;
  UINT16                            maxLimit = 3;
  UINT16                            minLimit = 0;
  struct channelNvram               (*channelNvList)[MAX_CH];
  DATACONTROL3N0_0_MCIO_DDRIO_STRUCT dataControl3;
  DATACONTROL3N0_0_MCIO_DDRIO_STRUCT dataControl3n1;

  channelNvList = GetChannelNvList(host, socket);

  //
  // Get minimum and maximum value
  //
  //GetDataGroupLimits(host, level, group, &minLimit, &maxLimit, &usDelay);
  MemCheckIndex(host, strobe, MAX_STROBE/2);

  if (strobe == ALL_STROBES) {
    strobe    = 0;
    maxStrobe = MAX_STROBE/2;
  } else {
    maxStrobe = strobe + 1;
    if (maxStrobe > MAX_STROBE/2) {
      return MRC_STATUS_STROBE_NOT_PRESENT;
    }
  }

  for (; strobe < maxStrobe; strobe++) {
    if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

    if ((strobe+9) >= MAX_STROBE) {
      return MRC_STATUS_STROBE_NOT_PRESENT;
    }

    if(mode & GSM_READ_CSR) {
      //
      // Read from PCI config space
      //
      dataControl3.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL3N0_0_MCIO_DDRIO_REG));
      dataControl3n1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, (strobe+9), DATACONTROL3N0_0_MCIO_DDRIO_REG));
   } else {
      //
      // Read from cache
      //
      dataControl3.Data = (*channelNvList)[ch].dataControl3[strobe];
      dataControl3n1.Data = (*channelNvList)[ch].dataControl3[(strobe+9)];
    } // DDR_LEVEL

    switch (group) {
      case RxCtleC:
        curVal = (UINT16)dataControl3.Bits.ddrcrctlecapen;
        break;
      case RxCtleR:
        curVal = (UINT16)dataControl3.Bits.ddrcrctleresen;
        break;
      default:
        break;
    }

    //
    // Check if read only
    //
    if (mode & GSM_READ_ONLY) {
      //
      // Save the value read into the output parameter
      //
      *value = curVal;
    } else {
      //
      // Write
      //
      // Adjust the current CMD delay value by offset
      if (mode & GSM_WRITE_OFFSET) {
        //
        // Add the offset to the current value
        //
        tmp = curVal + *value;

        //
        // Make sure we do not exeed the limits
        //
        if (tmp > minLimit) {
          piDelay = tmp;
        } else {
          piDelay = minLimit;
        }
        // Ensure we do not exceed maximums
        if (piDelay > maxLimit) {
          piDelay = maxLimit;
        }
      } else {
        //
        // Write absolute value
        //
        piDelay = *value;
      }

      switch (group) {
        case RxCtleC:
          dataControl3.Bits.ddrcrctlecapen = piDelay;
          dataControl3n1.Bits.ddrcrctlecapen = piDelay;
          break;
        case RxCtleR:
          dataControl3.Bits.ddrcrctleresen = piDelay;
          dataControl3n1.Bits.ddrcrctleresen = piDelay;
          break;
        default:
          break;
      }

      {
        //
        // Write it back if the current data does not equal the cache value or if in force write mode
        //
        if ((dataControl3.Data != (*channelNvList)[ch].dataControl3[strobe]) || (mode & GSM_FORCE_WRITE)) {

          MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL3N0_0_MCIO_DDRIO_REG), dataControl3.Data);
          MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, (strobe+9), DATACONTROL3N0_0_MCIO_DDRIO_REG), dataControl3n1.Data);
        }
      }
    } // if read only

    //
    // Save to cache
    //
    if (mode & GSM_UPDATE_CACHE) {
      (*channelNvList)[ch].dataControl3[strobe] = dataControl3.Data;
      (*channelNvList)[ch].dataControl3[(strobe+9)] = dataControl3n1.Data;
    }
  } // strobe loop

  return MRC_STATUS_SUCCESS;
} // GetSetCTLE

/*++

Routine Description:

  Get RxEq, control this on a per byte basis even though its per rank per nibble

Arguments:

  host    - Pointer to sysHost
  socket  - Socket number
  ch      - Channel number (0-based)
  strobe  - Strobe number (0-based)
  group   - CSM_GT - Parameter to program
  mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  value   - Data to program

Returns:

  IO delay

--*/
MRC_STATUS
GetSetRxEq (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     strobe,
  GSM_GT    group,
  UINT8     mode,
  INT16     *value
  )
{
  UINT8                             dimm,rank;
  UINT8                             maxStrobe;
  UINT8                             minStrobe;
  UINT16                            curVal = 0;
  UINT16                            piDelay;
  INT16                             tmp;
  UINT16                            maxLimit = 3;
  UINT16                            minLimit = 0;
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  UINT8                               logRank;
  RXGROUP0N0RANK0_0_MCIO_DDRIO_STRUCT rxGroup0;
  RXGROUP0N0RANK0_0_MCIO_DDRIO_STRUCT rxGroup0n1;

  channelNvList = GetChannelNvList(host, socket);
    
  dimmNvList = GetDimmNvList(host, socket, ch);
  
  MemCheckIndex(host, strobe, MAX_STROBE/2);
   
  if (strobe == ALL_STROBES) {
    minStrobe    = 0;
    maxStrobe = MAX_STROBE/2;
  } else {
    minStrobe = strobe;
    maxStrobe = strobe + 1;
  }

  if (maxStrobe > MAX_STROBE) {
    return MRC_STATUS_STROBE_NOT_PRESENT;
  }
  
  for (dimm = 0; dimm < MAX_DIMM; dimm++) {
    //
    // Skip if no DIMM present
    //
    if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
    for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
      //
      // Skip if no rank
      //
      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
      //
      // Get the logical rank #
      //
      logRank = GetLogicalRank(host, socket, ch, dimm, rank);

      //
      // Get minimum and maximum value
      //
      //GetDataGroupLimits(host, level, group, &minLimit, &maxLimit, &usDelay);

      for (strobe=minStrobe; strobe < maxStrobe; strobe++) {
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

        if ((strobe+9) >= MAX_STROBE) {
          continue;
        }

        if(mode & GSM_READ_CSR) {
          //
          // Read from PCI config space
          //
          rxGroup0.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, (strobe), RXGROUP0N0RANK0_0_MCIO_DDRIO_REG));
          rxGroup0n1.Data = MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, (strobe+9), RXGROUP0N0RANK0_0_MCIO_DDRIO_REG));
          
       } else {
          //
          // Read from cache
          //
          rxGroup0.Data = (*channelNvList)[ch].rxGroup0[logRank][strobe];
          rxGroup0n1.Data = (*channelNvList)[ch].rxGroup0[logRank][(strobe+9)];
        
        } // DDR_LEVEL

        curVal = (UINT16)rxGroup0.Bits.rxeq;

        //
        // Check if read only
        //
        if (mode & GSM_READ_ONLY) {
          //
          // Save the value read into the output parameter
          //
          *value = curVal;
        } else {
          //
          // Write
          //
          // Adjust the current CMD delay value by offset
          if (mode & GSM_WRITE_OFFSET) {
            //
            // Add the offset to the current value
            //
            tmp = curVal + *value;

            //
            // Make sure we do not exeed the limits
            //
            if (tmp > minLimit) {
              piDelay = tmp;
            } else {
              piDelay = minLimit;
            }
            // Ensure we do not exceed maximums
            if (piDelay > maxLimit) {
              piDelay = maxLimit;
            }
          } else {
            //
            // Write absolute value
            //
            piDelay = *value;
          }

          rxGroup0.Bits.rxeq = piDelay;
          rxGroup0n1.Bits.rxeq = piDelay;

          //
          // Write it back if the current data does not equal the cache value or if in force write mode
          //
          if ((rxGroup0.Data != (*channelNvList)[ch].rxGroup0[logRank][strobe]) || (mode & GSM_FORCE_WRITE)) {
            MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, strobe, RXGROUP0N0RANK0_0_MCIO_DDRIO_REG), rxGroup0.Data);
            MemWritePciCfgEp (host, socket, ch, UpdateIoRegister (host, logRank, (strobe+9), RXGROUP0N0RANK0_0_MCIO_DDRIO_REG), rxGroup0n1.Data);
          }

        } // if read only

        //
        // Save to cache
        //
        if (mode & GSM_UPDATE_CACHE) {
          (*channelNvList)[ch].rxGroup0[logRank][strobe] = rxGroup0.Data;
          (*channelNvList)[ch].rxGroup0[logRank][(strobe+9)] = rxGroup0n1.Data;
        }
      } // strobe loop
    } // Rank
  } //DIMM
  return MRC_STATUS_SUCCESS;
} // GetSetRxEq


UINT16
CodeDeCodeRxOdt (
  UINT16 value,
  UINT8  code
)
{

  UINT16 convertODT = 0;
  UINT16 target      = 0;
  UINT16 extResistor = 100;
  UINT16 vrefsteps   = 256;
  UINT16 codeMulti   = 1250;
  UINT32 vrefCode    = 0;
  UINT32 partialRes  = 0;
  UINT32 numerator   = 0;
  UINT32 denominator = 0;
  UINT16 roundUp     = 0;

  if (CODE_ODT == code) {
    // get Register value and convert it to ODT value
    vrefCode    = 32 * 10000;                        // scale factor avoid decimals
    partialRes  = (vrefCode/vrefsteps) + codeMulti;  // partial = (value/256) + 0.125
    numerator   = 10000 * (extResistor);
    denominator = numerator/(partialRes*2);          // ((Target) + 90)) = 90 / (partial * 2)
    target      = (UINT16)((denominator - extResistor));
    convertODT  = target / 2;
  } else {
    // get ODT value and convert it to Register value
    target      = value * 2;                               // target ohm per segment is 3*X
    numerator   = 10000 * (extResistor);                    // scale factor avoid decimals
    denominator = (target)+ extResistor;
    partialRes  = (numerator/denominator)/2;               // (90 / ((Target) + 90)) /2
    vrefCode    = (vrefsteps * (partialRes - codeMulti));  // 256 (multiplier - 0.125)
    roundUp     = (vrefCode)%10000;
    convertODT  = (UINT16) ((vrefCode/10000) + (roundUp/5000));
  } // if code
  return convertODT;
} // CodeDeCodeRxOdt

UINT16
CodeDeCodeTxRon (
  UINT16 value,
  UINT8  code
)
{
  UINT16 convertODT  = 0;
  UINT16 tgtDq       = 0;
  UINT16 extResistor = 90;
  UINT16 vrefsteps   = 256;
  UINT16 codeMulti   = 125;
  UINT32 vrefCode    = 0;
  UINT32 partialRes  = 0;
  UINT32 numerator   = 0;
  UINT32 denominator = 0;
  UINT16 roundUp     = 0;

  if (CODE_ODT == code) {
    // get Register value and convert it to ODT value
    convertODT  = value * 1000;                        // scale factor avoid decimals
    partialRes  = (convertODT/vrefsteps) + codeMulti;  // partial = (value/256) + 0.125
    numerator   = 1000 * (extResistor);
    denominator = numerator/(partialRes*2);            // ((Target) + 90)) = 90 / (partial * 2)
    tgtDq       = (UINT16)((denominator - extResistor));
    convertODT  = tgtDq / 3;
  } else {
    // get ODT value and convert it to Register value
    tgtDq       = value * 3;                               // target ohm per segment is 3*X
    numerator   = 1000 * (extResistor);                    // scale factor avoid decimals
    denominator = (tgtDq)+ extResistor;
    partialRes  = (numerator/denominator)/2;               // (90 / ((Target) + 90)) /2
    vrefCode    = (vrefsteps * (partialRes - codeMulti));  // 256 (multiplier - 0.125)
    roundUp     = (vrefCode)%1000;
    convertODT  = (UINT16) ((vrefCode/1000) + (roundUp/500));
  } // if code

  return convertODT;
} // CodeDeCodeTxRon

MRC_STATUS
GetSetCPUODT (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     strobe,
  UINT8     bit,
  GSM_LT    level,
  GSM_GT    group,
  UINT8     mode,
  INT16     *value
  )
/*++

Routine Description:

  Get ODT

Arguments:

  host    - Pointer to sysHost
  socket  - Socket number
  ch      - Channel number (0-based)
  dimm    - DIMM number (0-based)
  rank    - Rank number (0-based)
  strobe  - Strobe number (0-based)
  bit     - Bit number
  level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  group   - CSM_GT - Parameter to program
  mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  value   - Data to program

Returns:

  IO delay

--*/
{
  UINT16                              curVal = 0;
  UINT16                              tempVal = 0;
  UINT16                              piDelay;
  INT16                               tmp;
  UINT16                              maxLimit = 3;
  UINT16                              minLimit = 0;
  UINT8                               imc;
  DDRCRCOMPCTL0_MCIO_DDRIOEXT_STRUCT  ddrCRCompCtl0;
  struct imcNvram     (*imcNvList)[MAX_IMC];
  imcNvList     = GetImcNvList(host, socket);
  imc = GetMCID(host, socket, ch);

  //if (ch & 1) return SUCCESS;

  //
  // Get minimum and maximum value
  //
  // GetDataGroupLimits(host, level, group, &minLimit, &maxLimit, &usDelay);

  if(mode & GSM_READ_CSR) {

    //
    // Read from PCI config space
    //
    ddrCRCompCtl0.Data = MemReadPciCfgMC (host, socket, imc, DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG);

  } else {
    // Read from cache
    //
    ddrCRCompCtl0.Data = (*imcNvList)[imc].ddrCRCompCtl0;

  } // DDR_LEVEL

  switch (group) {
    case RxOdt:
      tempVal = (UINT16)ddrCRCompCtl0.Bits.dqodtpupvref;
      curVal  = ConvertOdtValue (host, group, tempVal, CODE_ODT, RDIMM);
      break;
    case TxRon:
      tempVal = (UINT16)ddrCRCompCtl0.Bits.dqdrvpupvref;
      curVal  = ConvertOdtValue (host, group, tempVal, CODE_ODT, RDIMM);
      break;
    default:
      break;
  }

  //
  // Check if read only
  //
  if (mode & GSM_READ_ONLY) {
    //
    // Save the value read into the output parameter
    //
    *value = curVal;
  } else {
    //
    // Write
    //
    // Adjust the current CMD delay value by offset
    if (mode & GSM_WRITE_OFFSET) {
      //
      // Add the offset to the current value
      //
      tmp = tempVal + *value;

      //
      // Make sure we do not exeed the limits
      //
      if (tmp > minLimit) {
        piDelay = tmp;
      } else {
        piDelay = minLimit;
      }
      // Ensure we do not exceed maximums
      if (piDelay > maxLimit) {
        piDelay = maxLimit;
      }
    } else {
      //
      // Write absolute value
      //
      tempVal = *value;
      piDelay = ConvertOdtValue (host, group, tempVal, DECODE_ODT, RDIMM);
    }

    switch (group) {
      case RxOdt:
        ddrCRCompCtl0.Bits.dqodtpupvref = piDelay;
        break;
      case TxRon:
        ddrCRCompCtl0.Bits.dqdrvpupvref = piDelay;
        break;
      default:
        break;
    }

    {
      //
      // Write it back if the current data does not equal the cache value or if in force write mode
      //
      // no cache
      if ((ddrCRCompCtl0.Data != (*imcNvList)[imc].ddrCRCompCtl0) || (mode & GSM_FORCE_WRITE)) {
        MemWritePciCfgMC (host, socket, imc, DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG, ddrCRCompCtl0.Data);
        DoComp(host,socket);
      }
    }
  } // if read only

  //
  // Save to cache
  //
  if (mode & GSM_UPDATE_CACHE) {
    {
      // no cache
      (*imcNvList)[imc].ddrCRCompCtl0 = ddrCRCompCtl0.Data;
    }
  }

  return MRC_STATUS_SUCCESS;
} // GetSetCPUODT

MRC_STATUS
GetSetDIMMODT (
  PSYSHOST  host,
  UINT8     socket,
  UINT8     ch,
  UINT8     dimm,
  UINT8     rank,
  UINT8     strobe,
  UINT8     bit,
  GSM_LT    level,
  GSM_GT    group,
  UINT8     mode,
  INT16     *value
  )
/*++

Routine Description:

  Get ODT

Arguments:

  host    - Pointer to sysHost
  socket  - Socket number
  ch      - Channel number (0-based)
  dimm    - DIMM number (0-based)
  rank    - Rank number (0-based)
  strobe  - Strobe number (0-based)
  bit     - Bit number
  level   - CSM_LT - Level to program (DDR, VMSE, Buffer)
  group   - CSM_GT - Parameter to program
  mode    - 0 - do not read from cache, 1 - read from cache, 2 - use cache to program register (used for fast boot or S3)
  value   - Data to program

Returns:

  IO delay

--*/
{
  UINT16              curVal = 0;
  UINT16              tempVal = 0;
  UINT16              piDelay = 0;
  INT16               tmp;
  UINT16              maxLimit = 15;
  UINT16              minLimit = 0;
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];
  struct rankDevice   (*rankStruct)[MAX_RANK_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  rankList = GetRankNvList(host, socket, ch, dimm);
  rankStruct = GetRankStruct(host, socket, ch, dimm);
#ifdef  LRDIMM_SUPPORT
  channelNvList = GetChannelNvList(host, socket);
#endif

  //
  // Get minimum and maximum value
  //
  //GetDataGroupLimits(host, level, group, &minLimit, &maxLimit, &usDelay);

  //
  // Read from cache
  //
#ifdef  LRDIMM_SUPPORT
  if (IsLrdimmPresent(host, socket, ch, dimm)) {
    switch (group) {
    case DramDrvStr:
      tempVal = (*channelNvList)[ch].dimmList[dimm].lrBuf_BC03;
      curVal  = ConvertOdtValue (host, group, tempVal, CODE_ODT, LRDIMM);
      break;
    case WrOdt:
      tempVal = (*channelNvList)[ch].dimmList[dimm].lrBuf_BC01;
      curVal  = ConvertOdtValue (host, group, tempVal, CODE_ODT, LRDIMM);
      break;
    case ParkOdt:
      tempVal = (*channelNvList)[ch].dimmList[dimm].lrBuf_BC02;
      curVal  = ConvertOdtValue (host, group, tempVal, CODE_ODT, LRDIMM);
      break;
    case NomOdt:
      tempVal = (*channelNvList)[ch].dimmList[dimm].lrBuf_BC00;
      curVal  = ConvertOdtValue (host, group, tempVal, CODE_ODT, LRDIMM);
      break;
    default:
      break;
    }
  } else
#endif
  {
    switch (group) {
    case DramDrvStr:
      tempVal = ((*rankStruct)[rank].MR1 & BIT1) >> 1;
      curVal  = ConvertOdtValue (host, group, tempVal, CODE_ODT, RDIMM);
      break;
    case WrOdt:
      tempVal = (*rankList)[rank].RttWr >> DDR4_RTT_WR_ODT_SHIFT;
      curVal  = ConvertOdtValue (host, group, tempVal, CODE_ODT, RDIMM);
      break;
    case ParkOdt:
      tempVal = (*rankList)[rank].RttPrk >> DDR4_RTT_PRK_ODT_SHIFT;
      curVal  = ConvertOdtValue (host, group, tempVal, CODE_ODT, RDIMM);
      break;
    case NomOdt:
      tempVal = (*rankList)[rank].RttNom >> DDR4_RTT_NOM_ODT_SHIFT;
      curVal  = ConvertOdtValue (host, group, tempVal, CODE_ODT, RDIMM);
      break;
    default:
      break;
    }
  }

  //
  // Check if read only
  //
  if (mode & GSM_READ_ONLY) {
    //
    // Save the value read into the output parameter
    //
    *value = curVal;
  } else {
    //
    // Write
    //
    // Adjust the current CMD delay value by offset
    if (mode & GSM_WRITE_OFFSET) {
      //
      // Add the offset to the current value
      //
      tmp = tempVal + *value;

      //
      // Make sure we do not exceed the limits
      //
      if (tmp > minLimit) {
        piDelay = tmp;
      } else {
        piDelay = minLimit;
      }
      // Ensure we do not exceed maximums
      if (piDelay > maxLimit) {
        piDelay = maxLimit;
      }
    } else {
      //
      // Write absolute value
      //
      tempVal = *value;
#ifdef  LRDIMM_SUPPORT
      if (IsLrdimmPresent(host, socket, ch, dimm)) {
        piDelay = ConvertOdtValue (host, group, tempVal, DECODE_ODT, LRDIMM);
      } else
#endif
      {
        if ((tempVal == 60) && (group == WrOdt)){ // Invalid RTT_WRT setting for 
          RC_ASSERT(FALSE, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_62);
        }
        piDelay = ConvertOdtValue (host, group, tempVal, DECODE_ODT, RDIMM); 
      }
    }

    {
      //
      // Write it back if the current data does not equal the cache value or if in force write mode
      //
#ifdef  LRDIMM_SUPPORT
      if (IsLrdimmPresent(host, socket, ch, dimm)) {
        switch (group) {
        case DramDrvStr:
          (*channelNvList)[ch].dimmList[dimm].lrBuf_BC03 = (UINT8) piDelay;
          WriteLrbuf(host, socket, ch, dimm, 0, (UINT8) piDelay, LRDIMM_F0, LRDIMM_BC03);
          break;
        case WrOdt:
          (*channelNvList)[ch].dimmList[dimm].lrBuf_BC01 = (UINT8) piDelay;
          WriteLrbuf(host, socket, ch, dimm, 0, (UINT8) piDelay, LRDIMM_F0, LRDIMM_BC01);
          break;
        case ParkOdt:
          (*channelNvList)[ch].dimmList[dimm].lrBuf_BC02 = (UINT8) piDelay;
          WriteLrbuf(host, socket, ch, dimm, 0, (UINT8) piDelay, LRDIMM_F0, LRDIMM_BC02);
          break;
        case NomOdt:
          (*channelNvList)[ch].dimmList[dimm].lrBuf_BC00 = (UINT8) piDelay;
          WriteLrbuf(host, socket, ch, dimm, 0, (UINT8) piDelay, LRDIMM_F0, LRDIMM_BC00);
          break;
        default:
          break;
        }
      } else
#endif
      {
          switch (group) {
          case DramDrvStr:
            (*rankStruct)[rank].MR1 &= ~BIT1;
            //*(UINT32 *)&host->setup.mem.svHooks.svOptions &= ~DIMM_WEAK_MODE;
            (*rankStruct)[rank].MR1 |= (piDelay << 1);
            // *(UINT32 *)&host->setup.mem.svHooks.svOptions |= piDelay << 31;
            WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR1, BANK1);
            break;
          case WrOdt:
            (*rankList)[rank].RttWr = piDelay << DDR4_RTT_WR_ODT_SHIFT;
            (*rankStruct)[rank].MR2 &= ~(0x3 << DDR4_RTT_WR_ODT_SHIFT); // clear RTTWR from MR2
            (*rankStruct)[rank].MR2 |= (*rankList)[rank].RttWr;
            WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR2, BANK2);
            break;
          case ParkOdt:
            (*rankList)[rank].RttPrk = piDelay << DDR4_RTT_PRK_ODT_SHIFT;
            (*rankStruct)[rank].MR5 &= ~(0x7 << DDR4_RTT_PRK_ODT_SHIFT);
            (*rankStruct)[rank].MR5 |= (*rankList)[rank].RttPrk;
            WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR5, BANK5);
            break;
          case NomOdt:
            (*rankList)[rank].RttNom = piDelay << DDR4_RTT_NOM_ODT_SHIFT ;
            (*rankStruct)[rank].MR1 &= ~(0x7 << DDR4_RTT_NOM_ODT_SHIFT); // clear RTTNOM from MR1
            (*rankStruct)[rank].MR1 |= (*rankList)[rank].RttNom;
            WriteMRS (host, socket, ch, dimm, rank, (*rankStruct)[rank].MR1, BANK1);
            break;
          default:
            break;
          }
      }
    }
  } // if read only

  //
  // Save to cache
  //
  if (mode & GSM_UPDATE_CACHE) {
#ifdef  LRDIMM_SUPPORT
    if ((*channelNvList)[ch].lrDimmPresent) {
    } else
#endif
    {
      switch (group) {
      case DramDrvStr:
        (*rankStruct)[rank].MR1 &= ~BIT1;
        //*(UINT32 *)&host->setup.mem.svHooks.svOptions &= ~DIMM_WEAK_MODE;
        (*rankStruct)[rank].MR1 |= (piDelay << 1);
        //*(UINT32 *)&host->setup.mem.svHooks.svOptions |= piDelay << 31;
        break;
      case WrOdt:
        break;
      case ParkOdt:
        break;
      case NomOdt:
        break;
      default:
        break;
      }
    }
  }

  return MRC_STATUS_SUCCESS;
} // GetSetDIMMODT

/**

  PlatformWriteDimmVref: Procedure to write the DIMM Vref setting to the given target

  @param host - Pointer to sysHost struct
  @param socket - processor number
  @param vref - vref value to write

  @retval SUCCESS if the Vref circuit is present and can be written
  @retval FAILURE if the Vref circuit is not present or cannot be written

**/
UINT32
MemWriteDimmVref (
                 PSYSHOST  host,
                 UINT8     socket,
                 UINT8     ch,
                 UINT8     dimm,
                 UINT8     rank,
                 UINT8     strobe,
                 GSM_LT    level,
                 UINT8     vref
                 )
{
  UINT8                 vrefRange;
  UINT8                 dram;
  struct channelNvram   (*channelNvList)[MAX_CH];
  struct rankDevice     (*rankStruct)[MAX_RANK_DIMM];
  struct dimmNvram      (*dimmNvList)[MAX_DIMM];
#ifdef LRDIMM_SUPPORT
  struct ddrRank        (*rankList)[MAX_RANK_DIMM];
  UINT8                 HostSideBufferTxVref[MAX_STROBE / 2];
  UINT8                 BufferIndex;
#endif

  channelNvList = GetChannelNvList(host, socket);
  rankStruct    = GetRankStruct(host, socket, ch, dimm);

  dimmNvList = GetDimmNvList(host, socket, ch);

  //check to make sure that strobe is within the range of strobes (used as array index in function)
  if(strobe < 0 || strobe >= MAX_STROBE){
    return FAILURE;
  }

#ifdef LRDIMM_SUPPORT
  if (level == LrbufLevel) {
    // for LR buffer side, only support RANGE1 (TODO: see if RANGE 2 is needed)
    vrefRange = vref;
    // Enforce upper limit per JEDEC spec
    if (vrefRange > 50) {
      vrefRange = 50;
    }
  } else
#endif //LRDIMM
  {
    //
    // Select the Vref range
    //
    vrefRange = GetVrefRange (host, vref);
  }
#ifdef LRDIMM_SUPPORT
    if (IsLrdimmPresent(host, socket, ch, dimm)) {

      if (level == DdrLevel) {

      rankList = GetRankNvList(host, socket, ch, dimm);
      //
      // Get per strobe range settings into an array
      //
      // (take vref range select bit 6 and put it in the LRDIMM buffer register bit 1)
      //NOTE: the backside DRAM range is written to range 1...ok for now, but may need to change later
      for (BufferIndex = 0; BufferIndex < (MAX_STROBE / 2); BufferIndex++) {
        HostSideBufferTxVref[BufferIndex] = ((((GetVrefRange(host, (*channelNvList)[ch].txVref[(*rankList)[rank].rankIndex][BufferIndex])) & BIT6) >> 5) | BIT0);
      }
      HostSideBufferTxVref[strobe % 9] = ((vrefRange & BIT6) >> 5) | BIT0;
      if ((host->setup.mem.optionsExt & PDA_EN) && (host->nvram.mem.txVrefDone)) {
      //NOTE: the backside DRAM range is written to range 1...ok for now, but may need to change later
        WriteLrbufPBA(host, socket, ch, dimm, rank, HostSideBufferTxVref, LRDIMM_F6, LRDIMM_BC4x, (strobe % 9));
      } else {
        WriteLrbuf(host, socket, ch, dimm, 0, HostSideBufferTxVref[0], LRDIMM_F6, LRDIMM_BC4x);
      }
      //
      // Get per strobe txVref settings into an array
      //
      for (BufferIndex = 0; BufferIndex < (MAX_STROBE / 2); BufferIndex++) {
        HostSideBufferTxVref[BufferIndex] = ((GetVrefRange(host, (*channelNvList)[ch].txVref[(*rankList)[rank].rankIndex][BufferIndex])) & 0x3F);
      }
      HostSideBufferTxVref[strobe % 9] = vrefRange & 0x3F;
      //
      // program LRDIMM Host Interface VREF Control Word
      //
      if ((host->setup.mem.optionsExt & PDA_EN) && (host->nvram.mem.txVrefDone)){
        WriteLrbufPBA(host, socket, ch, dimm, rank, HostSideBufferTxVref, LRDIMM_F5, LRDIMM_BC5x, (strobe % 9));
      } else {
        WriteLrbuf(host, socket, ch, dimm, 0, HostSideBufferTxVref[0], LRDIMM_F5, LRDIMM_BC5x);
      }
    } else {
      //LEVEL = LrbufLevel

      // Update MR6 value with new vref setting mode value
      (*rankStruct)[rank].MR6[strobe] &= ~0x7F;
      (*rankStruct)[rank].MR6[strobe] |= (vrefRange & 0x7F);

      MRSCmdStacking(host, socket, ch, dimm, LOAD_MRS);
      // Write the new DRAM Vref value and enable VrefDQ training mode
      WriteMRS(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR6[strobe] | BIT7, BANK6);
      WriteMRS(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR6[strobe] | BIT7, BANK6);
      WriteMRS(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR6[strobe], BANK6);
      MRSCmdStacking(host, socket, ch, dimm, EXECUTE_MRS);

    }
    WriteBC(host, socket, ch, dimm, 0, 0, LRDIMM_F0, LRDIMM_BC7x);
    (*channelNvList)[ch].dimmList[dimm].rcLrFunc = 0;

    } else
#endif
    {
      // Update MR6 value with new vref setting mode value
      (*rankStruct)[rank].MR6[strobe] &= ~0x7F;
      (*rankStruct)[rank].MR6[strobe] |= (vrefRange & 0x7F);

      // Write the new DRAM Vref value and enable VrefDQ training mode
      if ((host->setup.mem.optionsExt & PDA_EN) && (host->nvram.mem.txVrefDone)) {
        if ((*dimmNvList)[dimm].x4Present) {
          if (strobe >= 9) {
            dram = ((strobe - 9) * 2) + 1;
          } else {
            dram = strobe * 2;
          }
        } else {
          dram = strobe % 9;
        }
        WriteMRSPDA(host, socket, ch, dimm, rank, dram, (*rankStruct)[rank].MR6[strobe], BANK6);
      } else {
        WriteMR6(host, socket, ch, dimm, rank, (*rankStruct)[rank].MR6[strobe], BANK6);
      }
    }
  return SUCCESS;
} // MemWriteDimmVref

/**

  Converts the Vref setting to be written into MR6

  @param host - Pointer to sysHost struct
  @param vref - vref value to write

**/
UINT8
GetVrefRange (
             PSYSHOST  host,
             UINT8     vref
             )
{
  UINT8 vrefRange;

  //
  // Select the Vref range
  //
  if (vref < DDR4_VREF_RANGE1_OFFSET) {

    // Use range 2 and enforce lower limit
    if (vref > DDR4_VREF_RANGE2_OFFSET) {
      vrefRange = (vref - DDR4_VREF_RANGE2_OFFSET) | BIT6;
    } else {
      vrefRange = BIT6;
    }

  } else {

    // Use range 1
    vrefRange = vref - DDR4_VREF_RANGE1_OFFSET;

    // Enforce upper limit per JEDEC spec
    if (vrefRange > 50) {
      vrefRange = 50;
    }
  }

  return vrefRange;
} // GetVrefRange

/**

  Converts the Vref setting to be written into MR6

  @param host    - Pointer to sysHost struct
  @param socket  - processor number

**/
void
UpdateSafeTxVref (
  PSYSHOST  host,
  UINT8     socket,
  GSM_LT    level
                 )
{
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT8               maxStrobe;
  UINT8               hiVref;
  UINT8               loVref;
  UINT8               safelevel;
  INT16               vref;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      // skip NVMDIMM backside
      if (((*dimmNvList)[dimm].aepDimmPresent) && (level == LrbufLevel)) continue;
      // skip if lrdimm level but no LRDIMM found
      if ((level == LrbufLevel) && (IsLrdimmPresent(host, socket, ch, dimm) == 0)) continue;
      maxStrobe = GetMaxStrobe(host, socket, ch, dimm, TxVref, level);

      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;
        if (IsLrdimmPresent(host, socket, ch, dimm) && (rank > 0)) continue;
        loVref = 0xFF;
        hiVref = 0;

        for (strobe = 0; strobe < maxStrobe; strobe++) {
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          GetSetDataGroup (host, socket, ch, dimm, rank, strobe, ALL_BITS, level, TxVref, GSM_READ_ONLY, &vref);

          if (loVref > vref) {
            loVref = (UINT8)vref;
          }
          if (hiVref < vref) {
            hiVref = (UINT8)vref;
          }
        } // strobe loop

        safelevel = (UINT8)((loVref + hiVref) / 2);

        if (level ==  DdrLevel) {
          (*channelNvList)[ch].txVrefSafe[(*rankList)[rank].rankIndex] = safelevel;
        } else  if (level ==  LrbufLevel){
          (*channelNvList)[ch].txVrefSafeLrbufLevel[(*rankList)[rank].rankIndex] = safelevel;
        }
        MemDebugPrint((host, SDBG_MAX, socket, ch, dimm, rank, NO_STROBE, NO_BIT,
              "txVrefSafe = 0x%x\n", safelevel));
      } // rank loop
    } // dimm loop
  } // ch loop
} // UpdateSafeTxVref

/**

  Get the limits for the group or level requested

  @param host      - Pointer to sysHost
  @param level     - CSM_LT - Level to program (DDR, VMSE, Buffer)
  @param group     - CSM_GT - Parameter to program
  @param minLimit  - Minimum delay value allowed
  @param maxLimit  - Maximum absolute value allowed
  @param usdelay   - Time in usec required for new setting to take effect

  @retval MRC_STATUS

**/
MRC_STATUS
GetDataGroupLimits (
                   PSYSHOST  host,
                   GSM_LT    level,
                   GSM_GT    group,
                   UINT16    *minLimit,
                   UINT16    *maxLimit,
                   UINT16    *usDelay
                   )
{
  MRC_STATUS  status = MRC_STATUS_SUCCESS;

  if (level == DdrLevel) {
    switch (group) {
    case RecEnDelay:
    case RxDqsDelay:
    case RxDqsPDelay:
    case RxDqsNDelay:
    case RxEq:
    case WrLvlDelay:
    case TxDqsDelay:
    case TxDqDelay:
    case TxEq:
    case TxDqBitDelay:
      *minLimit = 0;
      *maxLimit = 511;
      *usDelay  = 0;
      break;
    case RxDqsBitDelay:
    case RxDqsPBitDelay:
    case RxDqsNBitDelay:
      *minLimit = 0;
      *maxLimit = 127;
      *usDelay  = 0;
      break;
      //case RxDqDelay:
    case EridVref:
    case RxVref:
      //*minLimit = -MAX_RD_VREF_OFFSET;
      *minLimit = 0;
      *maxLimit = 127;
      *usDelay  = 1;
      break;
    case TxVref:
      // SKX TODO: look at NVMCTLR vref range
      //*minLimit = -MAX_WR_VREF_OFFSET;
      *maxLimit = DDR4_VREF_RANGE1_OFFSET + 50;
      *minLimit = DDR4_VREF_RANGE2_OFFSET;
      *usDelay  = 1;
      break;
    case RxDqDelay:
      *minLimit = 0;
      *maxLimit = 7;
      *usDelay  = 0;
    case RxDqDqsDelay:
      *minLimit = 0;
      *maxLimit = 119;
      *usDelay  = 0;
    default:
      status = MRC_STATUS_GROUP_NOT_SUPPORTED;
      break;
    } // switch group
#ifdef LRDIMM_SUPPORT
  } else if (level == LrbufLevel) {
    switch (group) {
    case TxDqDelay:
    case RxDqsPDelay:
    case RxDqsNDelay:
    case RxDqsDelay:
      *minLimit = 0;
      *maxLimit = 30;
      *usDelay  = 0;
      break;
    case RxVref:
      //NOTE...Assume range 1 only, 0 to 50
      *maxLimit = 73;
      *minLimit = 0;
      *usDelay  = 1;
      break;

    case TxVref:
      *maxLimit = 50;
      *minLimit = 0;
      *usDelay  = 1;
      break;
    default:
      status = MRC_STATUS_GROUP_NOT_SUPPORTED;
      break;
    } // switch group
#endif
  } else {
    status = MRC_STATUS_LEVEL_NOT_SUPPORTED;
  }

  return status;
} // GetDataGroupLimits

/**

  Cache all DDRIO registers

  @param host    - Pointer to sysHost
  @param socket  - Socket number

**/
UINT32
CacheDDRIO (
           PSYSHOST  host,
           UINT8     socket
           )
{
  UINT8               imc;
  UINT8               ch;
  UINT8               dimm;
  UINT8               rank;
  UINT8               strobe;
  UINT16              minVal = 0xFF;
  UINT16              maxVal = 0x0;
  UINT16              zeroValue = 0;
  INT16               tempVal = 0;
  struct imcNvram     (*imcNvList)[MAX_IMC];
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct ddrRank      (*rankList)[MAX_RANK_DIMM];

  channelNvList = GetChannelNvList(host, socket);
  imcNvList     = GetImcNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    (*channelNvList)[ch].dataControl0 = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, 0, DATACONTROL0N0_0_MCIO_DDRIO_REG));
    (*channelNvList)[ch].ddrCRClkControls = MemReadPciCfgEp (host, socket, ch, DDRCRCLKCONTROLS_MCIO_DDRIOEXT_REG);

    // SKX change for upper nibble
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      (*channelNvList)[ch].dataControl1[strobe] = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL1N0_0_MCIO_DDRIO_REG));
      (*channelNvList)[ch].dataControl2[strobe] = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG));
      (*channelNvList)[ch].dataControl4[strobe] = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL4N0_0_MCIO_DDRIO_REG));
      (*channelNvList)[ch].dataControl3[strobe] = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL3N0_0_MCIO_DDRIO_REG));
    } // strobe loop

    GetSetCmdGroupDelay (host, socket, ch, CmdAll, GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &tempVal, &minVal, &maxVal);

    (*channelNvList)[ch].roundtrip0 = MemReadPciCfgEp (host, socket, ch, ROUNDTRIP0_MCDDC_DP_REG);
    (*channelNvList)[ch].roundtrip1 = MemReadPciCfgEp (host, socket, ch, ROUNDTRIP1_MCDDC_DP_REG);

    //
    // Loop for each dimm and each rank
    //
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      //
      // Skip if no DIMM present
      //
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);
      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        //
        // Skip if no rank
        //
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        GetSetCtlGroupDelay (host, socket, ch, (*rankList)[rank].ctlIndex + CtlGrp0, GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &tempVal, &minVal, &maxVal);
        GetSetClkDelay (host, socket, ch, (*rankList)[rank].ckIndex, GSM_READ_CSR | GSM_READ_ONLY | GSM_UPDATE_CACHE, &tempVal);

        //SKX change for upper nibble
        for (strobe = 0; strobe < MAX_STROBE; strobe++) {
          if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

          (*channelNvList)[ch].rxOffset[(*rankList)[rank].rankIndex][strobe] =
            MemReadPciCfgEp (host, socket, ch, UpdateIoRegister (host, (*rankList)[rank].logicalRank, strobe, RXOFFSETN0RANK0_0_MCIO_DDRIO_REG));
          (*channelNvList)[ch].dataOffsetTrain[strobe] = MemReadPciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATAOFFSETTRAINN0_0_MCIO_DDRIO_REG));

          GetSetTxDelay (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, TxDqDelay, GSM_READ_CSR | GSM_UPDATE_CACHE | GSM_READ_ONLY, (INT16 *)&zeroValue);
          GetSetRxDelay (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RxDqDelay, GSM_READ_CSR | GSM_UPDATE_CACHE | GSM_READ_ONLY, (INT16 *)&zeroValue);
          GetSetRxVref (host, socket, ch, dimm, rank, strobe, 0, DdrLevel, RxVref, GSM_READ_CSR | GSM_UPDATE_CACHE | GSM_READ_ONLY, (INT16 *)&zeroValue);

        } // strobe loop
      } // rank loop
    } // dimm loop
  } // ch loop

  for (imc = 0; imc < host->var.mem.maxIMC; imc++) {
    if (host->var.mem.socket[socket].imcEnabled[imc] == 0) continue;
    (*imcNvList)[imc].dimmVrefControl1 = MemReadPciCfgMC (host, socket, imc, DDRCRDIMMVREFCONTROL1_MCIO_DDRIOEXT_REG);
    (*imcNvList)[imc].ddrCRCompCtl0 = MemReadPciCfgMC (host, socket, imc, DDRCRCOMPCTL0_MCIO_DDRIOEXT_REG);
  }

  return SUCCESS;
} // CacheDDRIO



/**

  This resets the MC IO module

  @param host    - Pointer to sysHost
  @param socket    - Socket number

  @retval N/A

**/
void
IO_Reset (
         PSYSHOST host,
         UINT8    socket
         )
{
  MC_INIT_STATE_G_MC_MAIN_STRUCT mcInitStateG[MAX_IMC];
  UINT8 i;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "IO_Reset Starts\n"));
#endif

  for (i = 0; i < host->var.mem.maxIMC; i++) {
    if (host->var.mem.socket[socket].imcEnabled[i]) {
      mcInitStateG[i].Data = MemReadPciCfgMC(host, socket, i, MC_INIT_STATE_G_MC_MAIN_REG);
    }
  }

  //
  // Reset DDR IO
  //
  for (i = 0; i < host->var.mem.maxIMC; i++) {
    if (host->var.mem.socket[socket].imcEnabled[i]) {
      mcInitStateG[i].Bits.reset_io = 1;
      MemWritePciCfgMC(host, socket, i, MC_INIT_STATE_G_MC_MAIN_REG, mcInitStateG[i].Data);
    }
  }

  //
  // Wait 20 QCLK with reset high.
  //
  FixedQClkDelay (host, 20);

  //
  // Clear DDR IO Reset
  //
  for (i = 0; i < host->var.mem.maxIMC; i++) {
    if (host->var.mem.socket[socket].imcEnabled[i]) {
      mcInitStateG[i].Bits.reset_io = 0;
      MemWritePciCfgMC(host, socket, i, MC_INIT_STATE_G_MC_MAIN_REG, mcInitStateG[i].Data);
    }
  }

  //
  // Wait again 20 QCLK for stable IO.
  //
  FixedQClkDelay (host, 20);

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "IO_Reset Ends\n"));
#endif

} // IO_Reset


/**

  This function updates the function block number

  @param strobe            - Strobe number (0-based)
  @param RegisterGroupBase - Address of the the register group

  @retval IO Address

**/
UINT32
UpdateIoRegister (
                 PSYSHOST    host,
                 UINT8       logRank,
                 UINT8       strobe,
                 UINT32      regOffset
                 )
{
  CSR_OFFSET  csrReg;

  csrReg.Data = regOffset;

  //check to make sure that strobe is within the range of strobes (used as array index in function)
  MemCheckIndex(host, strobe, MAX_STROBE);  

  //SKX change
  // Bytes 0 and 1 are in the DDRIO_EXT function
  if ((strobe == 0) || (strobe == 1) || (strobe == 9) || (strobe == 10)) {
    csrReg.Bits.funcblk = csrReg.Bits.funcblk + 1;
  }

  //
  // Add in offset for data group
  //
  csrReg.Bits.offset = csrReg.Bits.offset + ddrioOffsetTable[strobe];

  //
  // Add in offset for rank
  //
  csrReg.Bits.offset = csrReg.Bits.offset + (logRank * 4);

  return csrReg.Data;
} // UpdateIoRegister


/**

  This function updates the function block number

  @param strobe            - Strobe number (0-based)
  @param RegisterGroupBase - Address of the the register group

  @retval IO Address

**/
UINT32
UpdateIoRegisterCh (
                   PSYSHOST    host,
                   UINT8       strobe,
                   UINT32      regOffset
                   )
{
  CSR_OFFSET  csrReg;

  csrReg.Data = regOffset;

  //check to make sure that strobe is within the range of strobes (used as array index in function)
  MemCheckIndex(host, strobe, MAX_STROBE);

  //SKX change
  // Bytes 0 and 1 are in the DDRIO_EXT function
  if ((strobe == 0) || (strobe == 1) || (strobe == 9) || (strobe == 10)) {
    csrReg.Bits.funcblk = csrReg.Bits.funcblk + 1;
  }

  //
  // Add in offset for data group
  //
  csrReg.Bits.offset = csrReg.Bits.offset + ddrioOffsetTable[strobe];

  return csrReg.Data;
} // UpdateIoRegisterCh

/**

  Initializes the data control 0 register

  @param host    - Pointer to sysHost
  @param socket  - Socket number
  @param dimm    - DIMM number (0-based)
  @param rank    - Rank number (0-based)

  @retval N/A

**/
void
InitDataControl0 (
                 PSYSHOST host,
                 UINT8    socket
                 )
{
  UINT8                                     ch;
  UINT8                                     strobe;
  struct channelNvram                       (*channelNvList)[MAX_CH];
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT        dataControl0;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dataControl0.Data = 0;

    (*channelNvList)[ch].dataControl0 = dataControl0.Data;

    // SKX change for upper nibble
    for (strobe = 0; strobe < MAX_STROBE; strobe++) {
      //
      // Skip if this is an ECC strobe when ECC is disabled
      //
      if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

      //
      // Set training mode and rank to all channels
      //
      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data);
    } // strobe loop
  } // ch loop
} // InitDataControl0

/**

  Set the training mode

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param dimm  - DIMM number (0-based)
  @param rank  - Rank number (0-based)
  @param mode  - Training mode

  @retval N/A

**/
void
SetTrainingMode (
                PSYSHOST host,
                UINT8    socket,
                UINT8    dimm,
                UINT8    rank,
                UINT8    mode
                )
{
  UINT8                                 ch;
  UINT8                                 strobe;
  struct channelNvram                   (*channelNvList)[MAX_CH];
  DATACONTROL0N0_0_MCIO_DDRIO_STRUCT    dataControl0;
  DATACONTROL2N0_0_MCIO_DDRIO_STRUCT    dataControl2;

  channelNvList = GetChannelNvList(host, socket);

  dataControl0.Data = 0;
  dataControl2.Data = 0;

    switch (mode) {
    case RECEIVE_ENABLE_BASIC:
      dataControl0.Bits.rltrainingmode  = 1;
      dataControl0.Bits.forceodton      = 1;
      dataControl2.Bits.forcerxon       = 1;
      break;

    case RX_DQ_DQS_BASIC:
      dataControl0.Bits.rxtrainingmode  = 1;
      break;

    case WRITE_LEVELING_BASIC:
      dataControl0.Bits.wltrainingmode  = 1;
      dataControl0.Bits.txpion          = 1;
      dataControl2.Bits.forcebiason     = 1;
      break;

    case SENSE_AMP:
      dataControl0.Bits.forceodton            = 1;
      dataControl0.Bits.ddrcrforceodton       = 1;
      dataControl0.Bits.senseamptrainingmode  = 1;
      dataControl2.Bits.forcebiason     = 1;
      dataControl2.Bits.forcerxon       = 1;
      break;

    case DISABLE_SENSE_AMP:
      //set back to default...
      //dataControl2.Bits.forcebiason     = 1;
      //dataControl2.Bits.forcerxon       = 1;
      break;

    default:
      break;
    } // switch mode

    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

      // SKX change for upper nibble
      for (strobe = 0; strobe < MAX_STROBE; strobe++) {
        //
        // Skip if this is an ECC strobe when ECC is disabled
        //
        if ((!host->nvram.mem.eccEn) && ((strobe == 8) || (strobe == 17))) continue;

        //
        // Set training mode and rank to all channels
        //
        // If enabling, program datacontrol2 first, if disabling program 0 first.
        if ((dataControl0.Bits.rltrainingmode == 1) ||
            (dataControl0.Bits.rxtrainingmode == 1) ||
            (dataControl0.Bits.wltrainingmode == 1) ||
            (dataControl0.Bits.senseamptrainingmode == 1)) {
          MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG), dataControl2.Data | (*channelNvList)[ch].dataControl2[strobe]);
          MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data | (*channelNvList)[ch].dataControl0);
        } else {
          MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL0N0_0_MCIO_DDRIO_REG), dataControl0.Data | (*channelNvList)[ch].dataControl0);
		      MemWritePciCfgEp (host, socket, ch, UpdateIoRegisterCh (host, strobe, DATACONTROL2N0_0_MCIO_DDRIO_REG), dataControl2.Data | (*channelNvList)[ch].dataControl2[strobe]);
        }
      } // strobe loop
    } // ch loop
} // SetTrainingMode


/**

  Initialize IO Comp settings and issue a comp cycle

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
UINT32
ProgramIOCompValues (
                    PSYSHOST  host,
                    UINT8     socket
                    )
{
#if defined(HW_EMULATION) || defined(HYBRID_VP)
  UINT8                             ch;
  UINT8                             dimm;
  UINT8                             rank;
  UINT8                             logRank;
  UINT8                             rtDelay;
  struct ddrRank                    (*rankList)[MAX_RANK_DIMM];
  struct channelNvram               (*channelNvList)[MAX_CH];
  struct dimmNvram                  (*dimmNvList)[MAX_DIMM];
  struct RankCh                     (*rankPerCh)[MAX_RANK_CH];
  TCOTHP_MCDDC_CTL_STRUCT           tcothp;
  IOLATENCY1_MCDDC_DP_STRUCT        ioLatency1;
  UINT8                             nAL, nPL;
  UINT8                             cmd_stretch;

  channelNvList = GetChannelNvList(host, socket);

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;

    dimmNvList = GetDimmNvList(host, socket, ch);

    for (dimm = 0; dimm < MAX_DIMM; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      rankList = GetRankNvList(host, socket, ch, dimm);

      for (rank = 0; rank < (*dimmNvList)[dimm].numRanks; rank++) {
        if (CheckRank(host, socket, ch, dimm, rank, DONT_CHECK_MAPOUT)) continue;

        rankPerCh = &(*channelNvList)[ch].rankPerCh;

        //
        // Get the logical rank #
        //
        logRank = GetLogicalRank(host, socket, ch, dimm, rank);

        if (host->nvram.mem.dimmTypePresent == RDIMM) {
          nAL = 1;
        } else {
          nAL = 0;
        }
        nPL = 0;
        rtDelay = (2 * ((*channelNvList)[ch].common.nCL + nAL + nPL)) + 7; // validation picker equation

        cmd_stretch = (*channelNvList)[ch].timingMode;
        if (cmd_stretch == TIMING_3N) {
          rtDelay = rtDelay + 6; // emulation offset.
        } else if (cmd_stretch == TIMING_2N) {
          rtDelay = rtDelay + 4; // emulation offset.
        } else if (cmd_stretch == TIMING_1N) {
          rtDelay = rtDelay + 2; // emulation offset.
        }


        (*rankPerCh)[(*rankList)[rank].rankIndex].Roundtrip = rtDelay;
        SetRoundTrip (host, socket, ch, logRank, rtDelay);

        (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency = 0;
        SetIOLatency (host, socket, ch, logRank, (*rankPerCh)[(*rankList)[rank].rankIndex].IOLatency);

      } // rank loop
    } // dimm loop

    //
    // per Celeste Brown, for RDIMM +1 to t_cwl_adj.
    //
    if (host->nvram.mem.dimmTypePresent == RDIMM) {
      tcothp.Data = MemReadPciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG);
      tcothp.Bits.t_cwl_adj = 1;
      MemWritePciCfgEp (host, socket, ch, TCOTHP_MCDDC_CTL_REG, tcothp.Data);
    }

    // IO Latency Compensation starting point
    ioLatency1.Data = MemReadPciCfgEp (host, socket, ch, IOLATENCY1_MCDDC_DP_REG);
    ioLatency1.Bits.io_lat_io_comp = MRC_ROUND_TRIP_IO_COMPENSATION_EMU;
    MemWritePciCfgEp (host, socket, ch, IOLATENCY1_MCDDC_DP_REG, ioLatency1.Data);
    (*channelNvList)[ch].ioLatency1 = ioLatency1.Data;
  } // ch loop


#endif
  return SUCCESS;
} // ProgramIOCompValues

/**

  Issue a comp cycle

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
void
DoComp (
       PSYSHOST host,
       UINT8    socket
       )
{
  UINT8                             mcId;
  RCOMP_TIMER_MC_MAIN_STRUCT        rcompTimer;
  MCMAIN_CHKN_BITS_MC_MAIN_STRUCT   mcChknBits;
  MCMAIN_CHKN_BITS_MC_MAIN_STRUCT   mcChknBitsOrg;

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "DoComp Starts\n"));
#endif

  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

    // if periodic RCOMP is disabled, re-enable 
    mcChknBitsOrg.Data = mcChknBits.Data = MemReadPciCfgMC (host, socket, mcId, MCMAIN_CHKN_BITS_MC_MAIN_REG);
    mcChknBits.Bits.dis_rcomp = 0;
    MemWritePciCfgMC (host, socket, mcId, MCMAIN_CHKN_BITS_MC_MAIN_REG, mcChknBits.Data);
      
    //
    // Read RCOMP_TIMER
    //
    rcompTimer.Data = MemReadPciCfgMC(host, socket, mcId,RCOMP_TIMER_MC_MAIN_REG);
    //
    // Make sure rcomp bit is 0 to begin with
    //
    rcompTimer.Bits.count = 0xED0;
    rcompTimer.Bits.rcomp = 0;
    //
    // Write value back
    //
    MemWritePciCfgMC(host, socket, mcId, RCOMP_TIMER_MC_MAIN_REG, rcompTimer.Data);

    //
    // Set rcomp
    //
    rcompTimer.Bits.rcomp = 1;
    //
    // Write value back
    //
    MemWritePciCfgMC(host, socket, mcId, RCOMP_TIMER_MC_MAIN_REG, rcompTimer.Data);

    //
    // Wait for the COMP cycle to complete
    //
    while ((rcompTimer.Bits.rcomp_in_progress == 1) || (rcompTimer.Bits.first_rcomp_done == 0)) {
      rcompTimer.Data = MemReadPciCfgMC (host, socket, mcId, RCOMP_TIMER_MC_MAIN_REG);

      if (host->var.common.emulation & SIMICS_FLAG) {
        break;
      }
    }

    //
    //Clear rcomp bit
    //
    rcompTimer.Bits.rcomp = 0;
    MemWritePciCfgMC(host, socket, mcId, RCOMP_TIMER_MC_MAIN_REG, rcompTimer.Data);
    
    // restore original comp setting
    MemWritePciCfgMC (host, socket, mcId, MCMAIN_CHKN_BITS_MC_MAIN_REG, mcChknBitsOrg.Data);
  } // mcId loop

#ifdef PRINT_FUNC
  MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "DoComp Ends\n"));
#endif

} // DoComp

/**

  Issue a ZQ cycle

  @param host            - sysHost data structure
  @param socket          - Processor socket
  @param ch              - Channel to issue the ZQ to
  @param zqType          - Type of ZQ Calibration: Long or Short

  @retval N/A

**/
void
DoZQ (
     PSYSHOST  host,
     UINT8     socket,
     UINT8     ch,
     UINT8     zqType
     )
{
  CpgcIssueZQ(host, socket, ch, zqType);
} // DoZQ

VOID
GetOriginalRtlChip(
  PSYSHOST    host,                             // Pointer to the system host (root) structure
  UINT8       socket,                           // Socket Id
  UINT8       ch,                               // Channel number (0-based)
  UINT8       dimm,                             // Current dimm
  UINT8       rank,                             // Rank number (0-based)
  UINT8       RtlInitializationMode,            // RTL Initialization Mode: RTL_INITIALIZE_ONCE or RTL_RE_INITIALIZE
  UINT8       *roundTripLatency                 // Pointer to the round trip latency
  )
{
  //
  // Get original RTL
  //
  if (RtlInitializationMode == RTL_INITIALIZE_ONCE) {
    *roundTripLatency = GetRoundTrip(host, socket, ch, GetLogicalRank(host, socket, ch, dimm, rank));
  }
}

/**

  Set the IO latency register value

  @param host            - Pointer to sysHost
  @param socket            - Socket numer
  @param ch              - Channel number (0-based)
  @param rank            - Rank number (0-based)
  @param IOLatencyValue  - New IO Latency value

  @retval N/A

**/
void
SetIOLatency (
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch,
             UINT8    rank,
             UINT32   IOLatencyValue
             )
{
  IOLATENCY0_MCDDC_DP_STRUCT  ioLatency;
  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  ioLatency.Data = MemReadPciCfgEp (host, socket, ch, IOLATENCY0_MCDDC_DP_REG);

  ioLatency.Data &= ~(0xF << (rank * 4));
  ioLatency.Data |= IOLatencyValue << (rank * 4);

  MemWritePciCfgEp (host, socket, ch, IOLATENCY0_MCDDC_DP_REG, ioLatency.Data);
  (*channelNvList)[ch].ioLatency0 = ioLatency.Data;
} // SetIOLatency

/**

  Gets the IO latency register value

  @param host  - Pointer to sysHost
  @param socket  - Socket numer
  @param ch    - Channel number (0-based)
  @param rank  - Rank number (0-based)

  @retval IO Latency

**/
UINT8
GetIOLatency (
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch,
             UINT8    rank
             )
{
  IOLATENCY0_MCDDC_DP_STRUCT    ioLatency;
  UINT8                         Value;

  ioLatency.Data  = MemReadPciCfgEp (host, socket, ch, IOLATENCY0_MCDDC_DP_REG);
  Value           = (UINT8) ((ioLatency.Data >> (rank * 4)) & 0xF);

  return Value;
} // GetIOLatency

/**

  Set the round trip register value

  @param host            - Pointer to sysHost
  @param socket          - Socket number
  @param ch              - Channel number (0-based)
  @param rank            - Rank number (0-based)
  @param RoundTripValue  - New Round Trip Latency value

  @retval N/A

**/
UINT8
SetRoundTrip (
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch,
             UINT8    rank,
             UINT8    RoundTripValue
             )
{
  UINT8                       rk;
  ROUNDTRIP0_MCDDC_DP_STRUCT  roundTrip;
  struct channelNvram         (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  if (rank < 4) {
    roundTrip.Data  = MemReadPciCfgEp (host, socket, ch, ROUNDTRIP0_MCDDC_DP_REG);
    rk              = rank;
    roundTrip.Data &= ~(0xFF << (rk * 8));
    roundTrip.Data |= RoundTripValue << (rk * 8);
  } else {
    roundTrip.Data  = MemReadPciCfgEp (host, socket, ch, ROUNDTRIP1_MCDDC_DP_REG);
    rk              = rank - 4;
    roundTrip.Data &= ~(0xFF << (rk * 8));
    roundTrip.Data |= RoundTripValue << (rk * 8);
  }

  if (rank < 4) {
    MemWritePciCfgEp (host, socket, ch, ROUNDTRIP0_MCDDC_DP_REG, roundTrip.Data);
    (*channelNvList)[ch].roundtrip0 = roundTrip.Data;
  } else {
    MemWritePciCfgEp (host, socket, ch, ROUNDTRIP1_MCDDC_DP_REG, roundTrip.Data);
    (*channelNvList)[ch].roundtrip1 = roundTrip.Data;
  }
  return RoundTripValue;
} // SetRoundTrip

/**

  Get the round trip register value

  @param host  - Pointer to sysHost
  @param socket  - Socket numer
  @param ch    - Channel number (0-based)
  @param rank  - Rank number (0-based)

  @retval N/A

**/
UINT8
GetRoundTrip (
             PSYSHOST host,
             UINT8    socket,
             UINT8    ch,
             UINT8    rank
             )
{
  UINT8                       rk;
  UINT8                       Value;
  ROUNDTRIP0_MCDDC_DP_STRUCT  roundTrip;

  if (rank < 4) {
    roundTrip.Data  = MemReadPciCfgEp (host, socket, ch, ROUNDTRIP0_MCDDC_DP_REG);
    rk              = rank;
  } else {
    roundTrip.Data  = MemReadPciCfgEp (host, socket, ch, ROUNDTRIP1_MCDDC_DP_REG);
    rk              = rank - 4;
  }

  Value = (UINT8) ((roundTrip.Data >> (rk * 8)) & 0xFF);

  return Value;
} // GetRoundTrip

/**

  Clears Bit-wise error status registers per channel

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param ch    - Channel number (0-based)

  @retval N/A

**/
void
ClearBWErrorStatus (
                   PSYSHOST host,
                   UINT8    socket,
                   UINT8    ch
                   )
{
} // ClearBWErrorStatus

/**

  Calculate Panic Up/Down multiplier value

 for 2133Mhz the value of ddrFreqMHz = 1066, so need to multiply by 2 to get the actual speed value. (sampledivider = 1; (1/speed)*2; sampledivider = 0; (1/speed);)

 for panicUpMultiplier:
 
 MIN_CAP (nF) * CHARGED_VOLTAGE (mV) * RDQ (r) * 1000 
 ------------------------------------------------------
 VSSHI (V)* 1000 * tPeriod (ps)

 MIN_CAP (nF) * CHARGED_VOLTAGE (mV) * RDQ (r) * 1000       2E-9 F * 24E-3 V * 90 r * 1000     2 F * 24 V * 90 r * 1000     
 ------------------------------------------------------  = -------------------------------- = -------------------------- 
 VSSHI (V)* 1000 * tPeriod (ps)                             0.356 V * 1000 * 937E-12s          356 V * 937 s
 
 nF * mV * r     F * V * r       F * r
 ------------ = ----------- =  -------- = constant; bcz (second/ohm = farad)
 V * ps           V *s             s

 similarly for panicDownMultiplier
 
  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
void PanicUpDnMultiplier(
                         PSYSHOST host, 
                         UINT8 socket
                         )
{
  UINT8                                     mcId;
  UINT8                                     chNum;
  UINT8                                     ch;
  UINT16                                    tPeriod = 0;
  UINT16                                    panicUpMultiplier = 0;
  UINT16                                    panicDownMultiplier = 0;
  UINT16                                    vccp = 0;
  UINT32                                    tempValue = 0;
  UINT32                                    mailboxStatus;
  UINT32                                    mailboxData;

  VSSHIORVREFCONTROLN0_0_MCIO_DDRIO_STRUCT  vsshiorvrefctl;
  DDRCRCOMPVSSHI1_MCIO_DDRIOEXT_STRUCT      ddrcrcompvsshi1;

  struct channelNvram (*channelNvList)[MAX_CH];

  channelNvList = GetChannelNvList(host, socket);

  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

    for (ch = 0; ch < MAX_MC_CH; ch++) {
      chNum = mcId * MAX_MC_CH + ch;    // Change channel format from 0-2 to 0-5
      if ((*channelNvList)[chNum].enabled == 0) continue;

      // as the value will be same across nibble and byte
      vsshiorvrefctl.Data = MemReadPciCfgEp (host, socket, chNum, UpdateIoRegisterCh (host, 0, VSSHIORVREFCONTROLN0_0_MCIO_DDRIO_REG));
      // SampleDivider == 0
      if (((vsshiorvrefctl.Bits.vsshiorvrefctl & (BIT12|BIT11|BIT10)) >> 0xA) == 0x0) {
        // Tperiod = 1 / Speed
        tPeriod = (UINT16) ((UINT32)1000000/(host->nvram.mem.socket[socket].ddrFreqMHz * 2)); // in ps
      } else if (((vsshiorvrefctl.Bits.vsshiorvrefctl & (BIT12|BIT11|BIT10)) >> 0xA) == 0x1) {
        // Tperiod = (1 / Speed) * 2
        tPeriod = (UINT16) ((UINT32)1000000/(host->nvram.mem.socket[socket].ddrFreqMHz * 2)) * 2; // in ps      
      } else {
        // throw an error mesg
          MemDebugPrint((host, SDBG_MINMAX, socket, chNum, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                        "Warning!!! Invalid setting for VSSHIORVREFCONTROLNx_y.VssHiOrVrefCtl, (SampleDivider) should be either 0x0 or 0x1\n"));
        // for some reason if there is no valid value set to SamplerDivider = VssHiOrVrefControlN*_*.VssHiOrVrefCtl[12:10]; use a default value, as Tperiod = 0, leads to divide by 0
        // As we know for Speed greater than and equal to 2400, SampleDivider = 0x1 and for speed below 2400, SampleDivider = 0x0, thus for default Tperiod value
        if ((host->nvram.mem.socket[socket].ddrFreqMHz * 2) >= 2400) {
          // Tperiod = (1 / Speed) * 2
          tPeriod = (UINT16) ((UINT32)1000000/(host->nvram.mem.socket[socket].ddrFreqMHz * 2)) * 2; // in ps      
        } else if ((host->nvram.mem.socket[socket].ddrFreqMHz * 2) < 2400) {
          // Tperiod = 1 / Speed
          tPeriod = (UINT16) ((UINT32)1000000/(host->nvram.mem.socket[socket].ddrFreqMHz * 2)); // in ps
        }
      }
      // iTarget = Cdie*deltaV/Tperiod (this has been changed to get better value)
      // tempValue = Cdie*deltaV * Rdq * 1000(multiply and divide by constant)
      tempValue = (MIN_CAP * CHARGED_VOLTAGE * RDQ * 1000);      

      // panicUpMultiplier (refer comments in function header)  
      panicUpMultiplier = (UINT16)(tempValue/(VSSHI * tPeriod));

      // init vccp to VCCP( default/fixed value of 1.3V)
      vccp = VCCP;
      // initiate a B2P mailbox command to read vccp voltage value
      mailboxStatus = WriteBios2PcuMailboxCommand(host, socket, MAILBOX_BIOS_CMD_READ_MC_VOLTAGE, mcId);
      // no error
      if (mailboxStatus == MAILBOX_BIOS_ERROR_CODE_NO_ERROR) {
        // B2P mailbox data to read vccp voltage value
        mailboxData = ReadCpuPciCfgEx (host, socket, 0, BIOS_MAILBOX_DATA);

        // vccp is 16bit value
        vccp = (UINT16)(mailboxData & 0xFFFF);

        // convert vccp to actual voltage value
        vccp = ((UINT32)vccp * 1000)/VCCP_CONVERSION_FACTOR;

        // when simics support is added to MAILBOX_BIOS_CMD_READ_MC_VOLTAGE, it will return mailboxStatus = MAILBOX_BIOS_ERROR_CODE_NO_ERROR; so vccp will become 0, due to mailboxData=0
        //  so force it a default value
        if (host->var.common.emulation & SIMICS_FLAG) {
          // init vccp to VCCP( default/fixed value of 1.3V)
          vccp = VCCP;
        }
        MemDebugPrint((host, SDBG_MAX, socket, chNum, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Panic Up/Down multiplier for MC:%d; vccp read from FIVR via PCODE:%d; (vccp value/1000 = value in volts)\n", mcId, vccp));
      } else {
        MemDebugPrint((host, SDBG_MAX, socket, chNum, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "Panic Up/Down multiplier for MC:%d; vccp is fixed value of 1.3V \n", mcId));
      }

      // panicDownMultiplier (refer comments in function header)
      panicDownMultiplier = (UINT16) (tempValue/((vccp-VSSHI) * tPeriod));

      // as the SampleDivider(tPeriod) value is same across channels
      break; // out of ch loop
    } // ch loop

    // write to comp 
    ddrcrcompvsshi1.Data = 0;
    ddrcrcompvsshi1.Bits.paniccompupmult = MIN(panicUpMultiplier, PANICCOMPUPMULT);
    ddrcrcompvsshi1.Bits.paniccompdnmult = MIN(panicDownMultiplier, PANICCOMPDNMULT);

    MemWritePciCfgMC(host, socket, mcId, DDRCRCOMPVSSHI1_MCIO_DDRIOEXT_REG, ddrcrcompvsshi1.Data);    

    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "Panic Up/Down multiplier for MC:%d; paniccompupmult:%d; paniccompdnmult:%d \n", mcId, ddrcrcompvsshi1.Bits.paniccompupmult, ddrcrcompvsshi1.Bits.paniccompdnmult));
  } // mcId loop
} // PanicUpDnMultiplier

/**

  Update Panic Up/Down code values

  @param host  - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
void UpdatePanicUpDownCodes(
                            PSYSHOST host, 
                            UINT8 socket
                           )
{
  UINT8                                     mcId;
  UINT16                                    compValue;

  DDRCRCOMPOVR_MCIO_DDRIOEXT_STRUCT         ddrcrcompovr;
  DDRCRDATACOMP1_MCIO_DDRIOEXT_STRUCT       ddrcrdatacomp1;
  DDRCRCMDCOMP1_CKE_MCIO_DDRIOEXT_STRUCT    ddrcrcmdcomp1_cke;
  DDRCRCMDCOMP1_CTL_MCIO_DDRIOEXT_STRUCT    ddrcrcmdcomp1_ctl;

  for (mcId = 0; mcId < host->var.mem.maxIMC; mcId++) {
    if (host->var.mem.socket[socket].imcEnabled[mcId] == 0) continue;

    // read comp fub values for panic up/down 
    ddrcrdatacomp1.Data = MemReadPciCfgMC(host, socket, mcId, DDRCRDATACOMP1_MCIO_DDRIOEXT_REG);

    // PanicDrvUp[9:0] = {PanicUp[9:4], PanicDn[9:6]}
    compValue = ((ddrcrdatacomp1.Bits.panicdrvup & (BIT9|BIT8|BIT7|BIT6|BIT5|BIT4)) | ((ddrcrdatacomp1.Bits.panicdrvdn & (BIT9|BIT8|BIT7|BIT6))>>6));

    // override panic up/down drive value
    ddrcrcompovr.Data = MemReadPciCfgMC(host, socket, mcId, DDRCRCOMPOVR_MCIO_DDRIOEXT_REG);
    ddrcrcompovr.Bits.panicdrvup = 1;
    ddrcrcompovr.Bits.panicdrvdn = 1;
    MemWritePciCfgMC(host, socket, mcId, DDRCRCOMPOVR_MCIO_DDRIOEXT_REG, ddrcrcompovr.Data);

    // assign comp fub values to cmd fubs
    ddrcrcmdcomp1_cke.Data = MemReadPciCfgMC(host, socket, mcId, DDRCRCMDCOMP1_CKE_MCIO_DDRIOEXT_REG);    
    ddrcrcmdcomp1_ctl.Data = MemReadPciCfgMC(host, socket, mcId, DDRCRCMDCOMP1_CTL_MCIO_DDRIOEXT_REG);    
    ddrcrcmdcomp1_cke.Bits.panicdrvdn = ddrcrdatacomp1.Bits.panicdrvdn;
    ddrcrcmdcomp1_cke.Bits.panicdrvup = compValue;
    ddrcrcmdcomp1_ctl.Bits.panicdrvdn = ddrcrdatacomp1.Bits.panicdrvdn;
    ddrcrcmdcomp1_ctl.Bits.panicdrvup = compValue;
    MemWritePciCfgMC(host, socket, mcId, DDRCRCMDCOMP1_CKE_MCIO_DDRIOEXT_REG, ddrcrcmdcomp1_cke.Data);    
    MemWritePciCfgMC(host, socket, mcId, DDRCRCMDCOMP1_CTL_MCIO_DDRIOEXT_REG, ddrcrcmdcomp1_ctl.Data);  

    MemDebugPrint((host, SDBG_MAX, socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                  "Updated Panic Up/Down codes for MC:%d \n", mcId));
  } // mcId loop
} // UpdatePanicUpDownCodes

#if SERIAL_DBG_MSG
char *GetGroupStr(GSM_GT group, char *strBuf) {

  switch (group) {
  case RecEnDelay:
    StrCpyLocal(strBuf, "RecEnDelay");
    break;
  case RxDqsDelay:
    StrCpyLocal(strBuf, "RxDqsDelay");
    break;
  case RxDqsPDelay:
    StrCpyLocal(strBuf, "RxDqsPDelay");
    break;
  case RxDqsNDelay:
    StrCpyLocal(strBuf, "RxDqsNDelay");
    break;
  case RxDqsBitDelay:
    StrCpyLocal(strBuf, "RxDqsBitDelay");
    break;
  case RxDqsPBitDelay:
    StrCpyLocal(strBuf, "RxDqsPBitDelay");
    break;
  case RxDqsNBitDelay:
    StrCpyLocal(strBuf, "RxDqsNBitDelay");
    break;
  case TxDqBitDelay:
    StrCpyLocal(strBuf, "TxDqBitDelay");
    break;
  case RxDqDelay:
    StrCpyLocal(strBuf, "RxDqDelay");
    break;
  case WrLvlDelay:
    StrCpyLocal(strBuf, "WrLvlDelay");
    break;
  case TxDqsDelay:
    StrCpyLocal(strBuf, "TxDqsDelay");
    break;
  case TxDqDelay:
    StrCpyLocal(strBuf, "TxDqDelay");
    break;
  case RxVref:
    StrCpyLocal(strBuf, "RxVref");
    break;
  case TxVref:
    StrCpyLocal(strBuf, "TxVref");
    break;
  default:
    StrCpyLocal(strBuf, "Unknown group");
    break;
  }
  return strBuf;
} // GetGroupStr
#endif  // SERIAL_DBG_MSG
