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
#include "BiosSsaChipFunc.h"
#include "MemApiSkx.h"

#ifdef SSA_FLAG

//
//This BSSA file contains subsets of all BSSA function calls made from the Core files; all CSR accesses are made here
//

/**

  This routine gets the CSRs required by BiosServerGet3dsRankPerCs

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Dimm                       - Zero based Dimm number (0-based)
  @param RankCount                  - Pointer to where the rank count per chip select signal will be stored

  @retval N/A

**/
VOID
BiosServerGet3dsRankPerCsChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Dimm,
  UINT8                 *RankCount
  )
{
  DIMMMTR_0_MC_MAIN_STRUCT           dimmMtr;

  dimmMtr.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, DIMMMTR_0_MC_MAIN_REG + (Dimm * 4));

  if (dimmMtr.Bits.ddr4_3dsnumranks_cs == 1) {
    *RankCount = 2;
  } else if (dimmMtr.Bits.ddr4_3dsnumranks_cs == 2) {
    *RankCount = 4;
  } else if (dimmMtr.Bits.ddr4_3dsnumranks_cs == 3) {
    *RankCount = 8;
  }
}

/**

  This routine sets the CSRs required by BiosServerSetCreditWaitConfig

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param Controller                 - Controller number (0-based)
  @param Enable                     - Specifies whether CPGC credit-wait feature is enabled.

  @retval N/A

**/
VOID
BiosServerSetCreditWaitConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  BOOLEAN               Enable
  )
{
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT     cpgcGlobalCtrlReg;

  cpgcGlobalCtrlReg.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_GLOBALCTRL_MC_MAIN_REG);
  cpgcGlobalCtrlReg.Bits.cpgc_crdt_wait = (Enable)? 1 : 0;
  MemWritePciCfgMC (host, Socket, Controller, CPGC_GLOBALCTRL_MC_MAIN_REG, cpgcGlobalCtrlReg.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetWpqInOrderConfig

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param Controller                 - Controller number (0-based)
  @param ChannelInSocket            - Channel number (0-based)
  @param Enable                     - Specifies whether the WPQ-in-order feature are enabled

  @retval N/A

**/
VOID
BiosServerSetWpqInOrderConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  BOOLEAN               Enable
  )
{
  CPGC_INORDER_MCDDC_CTL_STRUCT      cpgcInOrder;

  cpgcInOrder.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_INORDER_MCDDC_CTL_REG);
  cpgcInOrder.Bits.wpq_inorder_en = (Enable)? 1 : 0;
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_INORDER_MCDDC_CTL_REG, cpgcInOrder.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetMultipleWrCreditConfig

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param Controller                 - Controller number (0-based)
  @param ChannelInSocket            - Channel number (0-based)
  @param Enable                     - Specifies whether multiple write credits are enabled.

  @retval N/A

**/
VOID
BiosServerSetMultipleWrCreditConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  BOOLEAN               Enable
  )
{
  CPGC_SEQCTL0_MC_MAIN_STRUCT        cpgcSeqCtl0Reg;

  cpgcSeqCtl0Reg.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQCTL0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)));
  cpgcSeqCtl0Reg.Bits.link_wrtrn_enb = (Enable)? 1 : 0;
  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQCTL0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), cpgcSeqCtl0Reg.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetDqdbIndexes

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param Controller                 - Controller number (0-based)
  @param ChannelInSocket            - Channel number (0-based)
  @param StartIndex                 - Starting index in the DqDB.  The value must be less than or equal to the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.
  @param EndIndex                   - Ending index in the DqDB.  The value must be less than or equal to the StartIndex value and less than or equal to the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.
  @param IncRateMode                - Increment rate mode.
  @param IncRate                    - Increment rate value.  If the IncRateMode is ExponentialDqdbIncRateMode then the IncRate value must be less than or equal to the MaxDqdbIncRateExponentVal from the GetCpgcInfo() function.  If the IncRateMode is LinearDqdbIncRateMode then the IncRate value must be less than or equal to the MaxDqdbIncRateLinearVal from the GetCpgcInfo() function.

  @retval N/A

**/
VOID
BiosServerSetDqdbIndexesChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  UINT8                 StartIndex,
  UINT8                 EndIndex,
  DQDB_INC_RATE_MODE    IncRateMode,
  UINT8                 IncRate
  )
{
  CPGC_PATWDBCLCTL_MCDDC_CTL_STRUCT             patWDBCLCtl;

  //Program Write Data Buffer Related Entries
  patWDBCLCtl.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCLCTL_MCDDC_CTL_REG);
  patWDBCLCtl.Bits.incr_rate  = IncRate;
  patWDBCLCtl.Bits.start_pntr = StartIndex;
  patWDBCLCtl.Bits.end_pntr   = EndIndex;
  patWDBCLCtl.Bits.incr_scale = (IncRateMode == LinearDqdbIncRateMode) ? 1 : 0;
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCLCTL_MCDDC_CTL_REG, patWDBCLCtl.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetDqdbEccDataSource

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param EccDataSource              - Specifies the ECC signal data source

  @retval N/A

**/
VOID
BiosServerSetDqdbEccDataSourceChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  ECC_DATA_SOURCE       EccDataSource
  )
{
  CPGC_PATWDBCL_MUXCTL_MCDDC_DP_STRUCT            patWDBCLMuxCtl;

  patWDBCLMuxCtl.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG);
  patWDBCLMuxCtl.Bits.ecc_datasrc_sel = (EccDataSource == ByteGroup0)? 0 : 1;
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG, patWDBCLMuxCtl.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetDqdbUniseqMode

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Uniseq                     - Zero based DqDB unisequencer number(0-based)
  @param Mode                       - DqDB unisequencer mode.

  @retval N/A

**/
VOID
BiosServerSetDqdbUniseqModeChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Uniseq,
  DQDB_UNISEQ_MODE      Mode
  )
{
  CPGC_PATWDBCL_MUXCTL_MCDDC_DP_STRUCT          patWDBCLMuxCtl;

  patWDBCLMuxCtl.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG);

  switch (Uniseq) {
    case 0:
      patWDBCLMuxCtl.Bits.mux0 = Mode;
      break;
    case 1:
      patWDBCLMuxCtl.Bits.mux1 = Mode;
      break;
    case 2:
      patWDBCLMuxCtl.Bits.mux2 = Mode;
      break;
    default:
      break;
  }

  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG, patWDBCLMuxCtl.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetDqdbUniseqWrSeed

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Uniseq                     - Zero based DqDB unisequencer number(0-based)
  @param Seed                       - DqDB write unisequencer seed value.  The value must be less than or equal to the MaxDqdbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval N/A

**/
VOID
BiosServerSetDqdbUniseqWrSeedChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Uniseq,
  UINT32                Seed
  )
{
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCL_MUX0_PBWR_MCDDC_DP_REG + (Uniseq*4), Seed);  //Uniseq-> 0/1/2/3 for each of the 4 Mux Values
}

/**

  This routine sets the CSRs required by BiosServerSetDqdbUniseqRdSeed

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Uniseq                     - Zero based DqDB unisequencer number(0-based)
  @param Seed                       - DqDB write unisequencer seed value. The value must be less than or equal to the MaxDqdbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval N/A

**/
VOID
BiosServerSetDqdbUniseqRdSeedChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Uniseq,
  UINT32                Seed
  )
{
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCL_MUX0_PBRD_MCDDC_DP_REG + (Uniseq*4), Seed);  //Uniseq-> 0/1/2/3 for each of the 4 Mux Values
}

/**

  This routine gets the CSRs required by BiosServerGetDqdbUniseqWrSeed

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Uniseq                     - Zero based DqDB unisequencer number(0-based)
  @param Seed                       - Pointer to where the DqDB write unisequencer seed value will be stored.

  @retval N/A

**/
VOID
BiosServerGetDqdbUniseqWrSeedChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Uniseq,
  UINT32                *Seed
  )
{
  *Seed = MemReadPciCfgEp(host, Socket, ChannelInSocket, CPGC_PATWDBCLMUX0_PBSTS_WR_MCDDC_DP_REG + (Uniseq*4));  //Uniseq-> 0/1/2 for each of the 3 Mux Values
}

/**

  This routine gets the CSRs required by BiosServerGetDqdbUniseqRdSeed

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Uniseq                     - Zero based DqDB unisequencer number(0-based)
  @param Seed                       - Pointer to where the DqDB write unisequencer seed value will be stored.

  @retval N/A

**/
VOID
BiosServerGetDqdbUniseqRdSeedChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Uniseq,
  UINT32                *Seed
  )
{
  *Seed = MemReadPciCfgEp(host, Socket, ChannelInSocket, CPGC_PATWDBCLMUX0_PBSTS_RD_MCDDC_DP_REG + (Uniseq*4));  //Uniseq-> 0/1/2 for each of the 3 Mux Values
}

/**

  This routine sets the CSRs required by BiosServerSetDqdbUniseqLmn

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Uniseq                     - Zero based DqDB unisequencer number(0-based)
  @param L                          - DqDB unisequencer L counter value.  The value must be less than or equal to the MaxDqdbUniseqLVal value from the GetCpgcInfo() function.
  @param M                          - DqDB unisequencer M counter value.  The value must be less than or equal to the MaxDqdbUniseqMVal value from the GetCpgcInfo() function.
  @param N                          - DqDB unisequencer N counter value.  The value must be less than or equal to the MaxDqdbUniseqNVal value from the GetCpgcInfo() function.
  @param LDataSel                   - DqDB unisequencer L data select value.  The value must be 0 or 1.
  @param EnableFreqSweep            - Enables the Frequency Sweep feature.

  @retval N/A

**/
VOID
BiosServerSetDqdbUniseqLmnChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Uniseq,
  UINT16                L,
  UINT16                M,
  UINT16                N,
  UINT8                 LDataSel,
  BOOLEAN               EnableFreqSweep
  )
{
  CPGC_PATWDBCLMUX_LMN_MCDDC_DP_STRUCT       cpgcLmnMuxReg;

  cpgcLmnMuxReg.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCLMUX_LMN_MCDDC_DP_REG);
  cpgcLmnMuxReg.Bits.sweep_freq = (EnableFreqSweep)? 1 : 0;
  cpgcLmnMuxReg.Bits.l_cnt     = L;
  cpgcLmnMuxReg.Bits.m_cnt     = M;
  cpgcLmnMuxReg.Bits.n_cnt     = N;
  cpgcLmnMuxReg.Bits.l_datsel  = LDataSel;
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCLMUX_LMN_MCDDC_DP_REG, cpgcLmnMuxReg.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetDqdbUniseqLfsrConfig

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param ReloadRate                 - DqDB unisequencer LFSR seed reload rate.  The value must be less than or equal to the MaxDqdbUniseqSeedReloadRateVal value from the GetCpgcInfo() function.
  @param SaveRate                   - DqDB unisequencer LFSR seed save rate.  The value must be less than or equal to the MaxDqdbUniseqSeedSaveRateVal value from the GetCpgcInfo() function.

  @retval N/A

**/
VOID
BiosServerSetDqdbUniseqLfsrConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 ReloadRate,
  UINT8                 SaveRate
  )
{
  CPGC_PATWDBCL_MUXCTL_MCDDC_DP_STRUCT            cpgcPatWdbClMuxCtl;

  cpgcPatWdbClMuxCtl.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG);
  cpgcPatWdbClMuxCtl.Bits.save_lfsr_seedrate = SaveRate;
  cpgcPatWdbClMuxCtl.Bits.rld_lfsr_seedrate = ReloadRate;
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDBCL_MUXCTL_MCDDC_DP_REG, cpgcPatWdbClMuxCtl.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetDqdbInvertDcConfig

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Mode                       - Invert/DC mode.
  @param DcPolarity                 - DC polarity (when operating in the DC mode). 0 = DC low. 1 = DC high.
  @param ShiftRateExponent          - Exponent of the bitmask shift rate.  The value must be less than or equal to the MaxDqdbInvertDcShiftRateExponentVal value from the GetCpgcInfo() function.
  @param DqMaskLow                 - Lower 32 bits of the DQ Mask (0-based)
  @param DqMaskHigh                - Upper 32 bits of the DQ Mask (0-based)
  @param EccMask                    - Ecc Mask (0-based)

  @retval N/A

**/
VOID
BiosServerSetDqdbInvertDcConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  INVERT_DC_MODE        Mode,
  UINT8                 DcPolarity,
  UINT8                 ShiftRateExponent,
  UINT32                DqMaskLow,
  UINT32                DqMaskHigh,
  UINT8                 EccMask
  )
{
  CPGC_PATWDB_INV_MCDDC_DP_STRUCT       cpgcPatWdbInvReg;

  //Setting the DQ lane and ECC bits as per the bitMask for the Channel no. entered
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDB_INV0_MCDDC_DP_REG, DqMaskLow); //31:0
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDB_INV1_MCDDC_DP_REG, DqMaskHigh); // 63:32

  cpgcPatWdbInvReg.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDB_INV_MCDDC_DP_REG);
  cpgcPatWdbInvReg.Bits.eccinv_or_dcenb = EccMask;  //64:71 ECC lanes
  cpgcPatWdbInvReg.Bits.dc_polarity  = (DcPolarity == 0) ? 0 : 1;
  cpgcPatWdbInvReg.Bits.invordc_ctl  = (Mode == InvertMode) ? 0 : 1;
  cpgcPatWdbInvReg.Bits.invordc_shft_rate  = ShiftRateExponent;

  //if ((DqMaskLow == 0x0) && (DqMaskHigh == 0x0) && (EccMask == 0x0)) {
  //  cpgcPatWdbInvReg.Bits.datainv_or_dcshftenb  = 0;
  //} else {
  //  cpgcPatWdbInvReg.Bits.datainv_or_dcshftenb  = 1;
  //}

  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATWDB_INV_MCDDC_DP_REG, cpgcPatWdbInvReg.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetCadbConfig

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param EnableAlwaysOn             - Specifies whether CADB patterns will be driven out on all command and address pins every Dclk while a test is running.
  @param EnableOnDeselect           - Specifies whether CADB patterns will be driven out during deselect packets.
  @param EnableParityNTiming        - Specifies parity timing.  If this parameter is TRUE, CADB parity will be driven on same clock as CMD/Address (i.e., N timing for UDIMM DDR4 only).  If this parameter is FALSE, CADB parity will be driven on the clock after CMD/Address (i.e,. N+1 timing for all other devices).
  @param EnableOnePassAlwaysOn      - This parameter only applies if the EnableAlwaysOn parameter is TRUE.  If this parameter is TRUE, the test stops after all 16 entries have been issued out of CADB.
  @param EnablePdaDoubleLength      - If this parameter is TRUE, any PBA (Per Buffer Addressable) /PDA (Per DRAM addressable) writes issued from CADB will be double length (i.e. if lockstep is disabled, 16 UI instead of 8 UI or if lockstep is enabled, 8 UI instead of 4 UI).  This is to allow more setup up time for PBA accesses during training.  If this parameter is FALSE, PBA/PDA writes will look exactly the same as normal writes.

  @retval N/A

**/
VOID
BiosServerSetCadbConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  BOOLEAN               EnableAlwaysOn,
  BOOLEAN               EnableOnDeselect,
  BOOLEAN               EnableParityNTiming,
  BOOLEAN               EnableOnePassAlwaysOn,
  BOOLEAN               EnablePdaDoubleLength
  )
{
  CPGC_PATCADBCTL_MCDDC_CTL_STRUCT              cpgcPatCadbCtlReg;

  cpgcPatCadbCtlReg.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_PATCADBCTL_MCDDC_CTL_REG);

  cpgcPatCadbCtlReg.Bits.enable_cadb_always_on   = (EnableAlwaysOn) ? 1 : 0;
  cpgcPatCadbCtlReg.Bits.enable_cadb_on_deselect = (EnableOnDeselect)? 1 : 0;
  cpgcPatCadbCtlReg.Bits.enable_cadb_parityntiming = (EnableParityNTiming)? 1 : 0;
  cpgcPatCadbCtlReg.Bits.enable_onepasscadb_always_on = (EnableOnePassAlwaysOn)? 1 : 0;
  cpgcPatCadbCtlReg.Bits.enable_pda_doublelength = (EnablePdaDoubleLength)? 1 : 0;

  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATCADBCTL_MCDDC_CTL_REG, cpgcPatCadbCtlReg.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetCadbUniseqMode

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Uniseq                     - Zero based CADB unisequencer number.
  @param DeselectMode               - CADB deselect cycle unisequencer mode.  The value must be less than or equal to the MaxCadbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval N/A

**/
VOID
BiosServerSetCadbUniseqModeChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Uniseq,
  CADB_UNISEQ_MODE      DeselectMode
  )
{
  CPGC_PATCADBMUXCTL_MCDDC_CTL_STRUCT              cpgcPatCadbMuxReg;

  cpgcPatCadbMuxReg.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_PATCADBMUXCTL_MCDDC_CTL_REG);
  switch (Uniseq) {
    case 0:
      cpgcPatCadbMuxReg.Bits.mux0_control = DeselectMode;
      break;
    case 1:
      cpgcPatCadbMuxReg.Bits.mux1_control = DeselectMode;
      break;
    case 2:
      cpgcPatCadbMuxReg.Bits.mux2_control = DeselectMode;
      break;
    case 3:
      cpgcPatCadbMuxReg.Bits.mux3_control = DeselectMode;
      break;
    default:
      break;
  }
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATCADBMUXCTL_MCDDC_CTL_REG, cpgcPatCadbMuxReg.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetCadbUniseqSeed

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Uniseq                     - Zero based CADB unisequencer number.
  @param DeselectMode               - CADB deselect cycle unisequencer mode.  The value must be less than or equal to the MaxCadbUniseqSeedVal value from the GetCpgcInfo() function.

  @retval N/A

**/
VOID
BiosServerSetCadbUniseqSeedChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Uniseq,
  UINT32                DeselectSeed
  )
{
  CPGC_PATCADBMUX0PB_MCDDC_CTL_STRUCT    cpgcPatCadb;

  cpgcPatCadb.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_PATCADBMUX0PB_MCDDC_CTL_REG + (Uniseq*4));
  cpgcPatCadb.Bits.pattern_buffer = (UINT16)(DeselectSeed & 0x0000FFFF);
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_PATCADBMUX0PB_MCDDC_CTL_REG + (Uniseq*4), cpgcPatCadb.Data);  //Uniseq-> 0/1/2/3 for each of the 4 Mux Values
}

/**

  This routine sets the CSRs required by BiosServerSetCadbUniseqLmn

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param ChannelInSocket            - Channel number (0-based)
  @param Uniseq                     - Zero based DqDB unisequencer number(0-based)
  @param L                          - CADB unisequencer L counter value.  The value must be less than or equal to the MaxCadbUniseqLVal value from the GetCpgcInfo() function.
  @param M                          - CADB unisequencer M counter value.  The value must be less than or equal to the MaxCadbUniseqMVal value from the GetCpgcInfo() function.
  @param N                          - CADB unisequencer N counter value.  The value must be less than or equal to the MaxCadbUniseqNVal value from the GetCpgcInfo() function.
  @param LDataSel                   - CADB unisequencer L data select value.  The value must be 0 or 1.
  @param EnableFreqSweep            - Enables LMN frequency sweep feature.

  @retval N/A

**/
VOID
BiosServerSetCadbUniseqLmnChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Uniseq,
  UINT16                L,
  UINT16                M,
  UINT16                N,
  UINT8                 LDataSel,
  BOOLEAN               EnableFreqSweep
  )
{
  UINT32                                        Offset;
  CPGC_PATCADBCLMUX0LMN_MCDDC_CTL_STRUCT        cpgcPatCadbMuxLmnReg;

  Offset = (CPGC_PATCADBCLMUX0LMN_MCDDC_CTL_REG + (Uniseq*4));  //Uniseq-> 0/1/2/3 for each of the 4 Mux Values

  cpgcPatCadbMuxLmnReg.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, Offset);

  cpgcPatCadbMuxLmnReg.Bits.l_counter     = L;
  cpgcPatCadbMuxLmnReg.Bits.m_counter     = M;
  cpgcPatCadbMuxLmnReg.Bits.n_counter     = N;
  cpgcPatCadbMuxLmnReg.Bits.l_data_select = LDataSel;
  cpgcPatCadbMuxLmnReg.Bits.sweep_freq    =  (EnableFreqSweep)? 1 : 0;

  MemWritePciCfgEp (host, Socket, ChannelInSocket, Offset, cpgcPatCadbMuxLmnReg.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetDummyReadLmn

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param Controller                 - Controller (0-based)
  @param ChannelInSocket            - Channel number (0-based)
  @param L                          - Dummy Read unisequencer L counter value.  The value must be less than or equal to the MaxDummyReadLVal value from the GetCpgcInfo() function.
  @param M                          - Dummy Read unisequencer M counter value.  The value must be less than or equal to the MaxDummyReadMVal value from the GetCpgcInfo() function.
  @param N                          - Dummy Read unisequencer N counter value.  The value must be less than or equal to the MaxDummyReadNVal value from the GetCpgcInfo() function.
  @param LDataSel                   - Dummy Read unisequencer L data select value.  The value must be 0 or 1.
  @param EnableFreqSweep            - Enables LMN frequency sweep feature.

  @retval N/A

**/
VOID
BiosServerSetDummyReadLmnChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  UINT16                L,
  UINT16                M,
  UINT16                N,
  UINT8                 LDataSel,
  BOOLEAN               EnableFreqSweep
  )
{
  CPGC_SEQ_DMYRDCTL0_MC_MAIN_STRUCT              seqDmyRdCtl;

  seqDmyRdCtl.Data = 0;
  seqDmyRdCtl.Bits.l_dmyrd_select = LDataSel;
  seqDmyRdCtl.Bits.l_counter = L;
  seqDmyRdCtl.Bits.m_counter = M;
  seqDmyRdCtl.Bits.n_counter = N;
  seqDmyRdCtl.Bits.sweep_freq = (EnableFreqSweep)? 1 : 0;

  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_DMYRDCTL0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), seqDmyRdCtl.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetDummyReadBankMask

  @param host                       - Pointer to sysHost
  @param Socket                     - Socket Id
  @param Controller                 - Controller (0-based)
  @param ChannelInSocket            - Channel number (0-based)
  @param BankMask                   - Dummy Read bank mask.(0-based)

  @retval N/A

**/
VOID
BiosServerSetDummyReadBankMaskChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  UINT16                BankMask
  )
{
  CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_STRUCT        seqDmyRdAddrlo;
  CPGC_SEQ_DMYRDADDR_HI_MC_MAIN_STRUCT        seqDmyRdAddrhi;

  if (ChannelInSocket < MAX_CH_IMC) {
    seqDmyRdAddrlo.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_REG);
    if (ChannelInSocket == 0) {
      seqDmyRdAddrlo.Bits.bank_mask0 = BankMask;
    } else {
      seqDmyRdAddrlo.Bits.bank_mask1 = BankMask;
    }
    MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_DMYRDADDR_LO_MC_MAIN_REG, seqDmyRdAddrlo.Data);
  } else {
    seqDmyRdAddrhi.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_DMYRDADDR_HI_MC_MAIN_REG);
    if (ChannelInSocket == 2) {
      seqDmyRdAddrhi.Bits.bank_mask2 = BankMask;
    } else {
      seqDmyRdAddrhi.Bits.bank_mask3 = BankMask;
    }

    MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_DMYRDADDR_HI_MC_MAIN_REG, seqDmyRdAddrhi.Data);
  }
}

/**

  This routine sets the CSRs required by BiosServerSetSubseqConfig

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param Subseq                            - Zero based subsequence number.  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param BurstLengthMode,                  - Burst length mode.
  @param BurstLength                       - Burst length in cachelines.  If BurstLengthMode is ExponentialBurstLengthMode then the Burstlength value is an exponent and the value must be less than or equal to the MaxBurstLengthExponentVal value from the in GetCpgcInfo() function.  If BurstLengthMode is LinearBurstLengthMode then the Burstlength value is linear and the value must be less than or equal to the MaxBurstLengthLinearVal value from the in GetCpgcInfo() function.
  @param InterSubseqWait                   - Inter-subsequence wait in dclks.  The value must be less than or equal to the MaxInterSubseqWaitVal value from the GetCpgcinfo() function.
  @param SubseqType                        - Subsequence type.
  @param EnableSaveCurrentBaseAddrToStart  - Specifies whether the current base address is saved to the start address with this subsequence.
  @param EnableResetCurrentBaseAddrToStart - Specifies whether the current base address is reset to the start address with this subsequence.
  @param EnableAddrInversion               - Specifies whether address inversion enabled for this subsequence.
  @param EnableDataInversion               - Specifies whether data inversion enabled for this subsequence.

  @retval N/A

**/
VOID
BiosServerSetSubseqConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  UINT8                 Subseq,
  BURST_LENGTH_MODE     BurstLengthMode,
  UINT32                BurstLength,
  UINT32                InterSubseqWait,
  SUBSEQ_TYPE           SubseqType,
  BOOLEAN               EnableSaveCurrentBaseAddrToStart,
  BOOLEAN               EnableResetCurrentBaseAddrToStart,
  BOOLEAN               EnableAddrInversion,
  BOOLEAN               EnableDataInversion
  )
{
  UINT32                                            baseRegisterOffset = 0;
  CPGC_SUBSEQ0_CTL0_MC_MAIN_STRUCT                  cpgcSubseq0Ctl0Reg;

  //Each Channel has 10 of the below registers; baseRegisterOffset is the base register(0-9) depending on the Subseq Value entered (0-9)
  //Depending on whether its 2HA (convert ch 2/3 to 0/1) OR 1HA (ch 0/1/2/3) we then add the baseRegisterOffset to the calculation (0x50 * GetMCCh(Controller, Channel))
  //Which takes us to the next instance of the same register on the next channel (80bytes apart, as offset between these 10 registers/channel is 8bytes)
  //(0x50 * GetMCCh(Controller, Channel)) --> all this does is move to the correct channel specified (i.e. whether to add 80/160/240bytes to the baseRegister)
  //Subseq (range 0 - 10) -> used to get the register offset  CPGC_SUBSEQ0_CTL0_MC_MAINEXT_REG

  baseRegisterOffset = (CPGC_SUBSEQ0_CTL0_MC_MAIN_REG + 8*Subseq);

  cpgcSubseq0Ctl0Reg.Data = MemReadPciCfgMC (host, Socket, Controller, baseRegisterOffset + (0x50 * GetMCCh(Controller, ChannelInSocket)));

  if (BurstLengthMode == ExponentialBurstLengthMode) {
    cpgcSubseq0Ctl0Reg.Bits.num_cachelines_scale = 0;  //Set to 0 for Exponential
  } else if (BurstLengthMode == LinearBurstLengthMode) {
    cpgcSubseq0Ctl0Reg.Bits.num_cachelines_scale = 1;  //Set to 1 for linear
  }

  cpgcSubseq0Ctl0Reg.Bits.num_cachelines  = BurstLength;  //Set to 1 for linear
  cpgcSubseq0Ctl0Reg.Bits.subseq_wait     = InterSubseqWait;
  cpgcSubseq0Ctl0Reg.Bits.subseq_type     = SubseqType;
  cpgcSubseq0Ctl0Reg.Bits.savecurr_ba_start = (EnableSaveCurrentBaseAddrToStart)? 1 : 0;
  cpgcSubseq0Ctl0Reg.Bits.rstcurr_ba_start = (EnableResetCurrentBaseAddrToStart)? 1 : 0;
  cpgcSubseq0Ctl0Reg.Bits.dataecc_addrinv  = (EnableAddrInversion)? 1 : 0;
  cpgcSubseq0Ctl0Reg.Bits.inv_dataecc = (EnableDataInversion)? 1 : 0;

  MemWritePciCfgMC (host, Socket, Controller, baseRegisterOffset + (0x50 * GetMCCh(Controller, ChannelInSocket)), cpgcSubseq0Ctl0Reg.Data); //0x50 = 80bytes
}

/**

  This routine sets the CSRs required by BiosServerSetSubseqOffsetConfig

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param Subseq                            - Zero based subsequence number.  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param EnableBaseInvertData              - Specifies whether base inversion of the data is enabled.
  @param EnableRowIncrement                - Specifies whether row increment is enabled.
  @param RowIncrementOrder                 - Specifies row increment order.  The value must be 0 or 1.
  @param EnableColIncrement                - Specifies whether column increment is enabled.
  @param ColIncrementOrder                 - Specifies column increment order.  The value must be 0 or 1.
  @param BaseSubseqType                    - Every time the BaseSubseqRepeatRate is reached a Read or Write is performed for one command based on the following fields: BaseSubseqType indicates whether a read (BaseReadSubseqType) or write (BaseWriteSubseqType) operation will occur at the current base address.  EnableBaseInvertData indicates whether the read or write (BaseSubseqType) operation at the current base address is inverted (EnableBaseInvertData = TRUE) or not (EnableBaseInvertData = FALSE).
  @param BaseSubseqRepeatRate              - Specifies how often the BaseSubseqType operation occurs for 1 cacheline.  0 = No BaseSubseqType cacheline operations will occur during the Offset_Read or Offset_Write.  1 = Reserved value.  2 = Repeat the BaseSubseqType cacheline operation after every offset cacheline operation.  ...  31 = Repeat the BaseSubseqType cacheline operation after 30 offset cacheline operations.
  @param OffsetAddrUpdateRate              - Specifies the rate that the Current Offset Address is updated.  The value must be less than or equal to the MaxOffsetAddrUpdateRateVal value from the GetCpgcInfo() function.

  @retval N/A

**/
VOID
BiosServerSetSubseqOffsetConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  UINT8                 Subseq,
  BOOLEAN               EnableBaseInvertData,
  BOOLEAN               EnableRowIncrement,
  UINT8                 RowIncrementOrder,
  BOOLEAN               EnableColIncrement,
  UINT8                 ColIncrementOrder,
  SUBSEQ_TYPE           BaseSubseqType,
  UINT32                BaseSubseqRepeatRate,
  UINT32                OffsetAddrUpdateRate
  )
{
  UINT32                                            subseqRegisterOffset = 0;
  CPGC_SUBSEQ0_OFFSETCTL0_MC_MAIN_STRUCT            subseq0_offsetctl0;

  //See "baseRegisterOffset" explanation in BiosServerSetSubseqConfig
  subseqRegisterOffset = (CPGC_SUBSEQ0_OFFSETCTL0_MC_MAIN_REG + 8*Subseq);
  subseq0_offsetctl0.Data = MemReadPciCfgMC (host, Socket, Controller, subseqRegisterOffset + (0x50 * GetMCCh(Controller, ChannelInSocket)));

  subseq0_offsetctl0.Bits.base_inv_dataecc = (EnableBaseInvertData)? 1 : 0;
  subseq0_offsetctl0.Bits.row_inc_enb = (EnableRowIncrement)? 1 : 0;
  subseq0_offsetctl0.Bits.row_inc_order = RowIncrementOrder;
  subseq0_offsetctl0.Bits.col_inc_enb = (EnableColIncrement)? 1 : 0;
  subseq0_offsetctl0.Bits.col_inc_order = ColIncrementOrder;
  subseq0_offsetctl0.Bits.base_subseq_type = BaseSubseqType;
  subseq0_offsetctl0.Bits.base_subseq_rptrate = BaseSubseqRepeatRate;
  subseq0_offsetctl0.Bits.offadd_updrate = OffsetAddrUpdateRate;

  MemWritePciCfgMC (host, Socket, Controller, subseqRegisterOffset + (0x50 * GetMCCh(Controller, ChannelInSocket)), subseq0_offsetctl0.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetSeqConfig

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param StartDelay                        - Specifies the delay period (in dclks) between the start of the test and the actual entry to Loopback.Pattern where the first SubSequence is executed immediately.  The value must be less than or equal to the MaxStartDelayVal value from the GetCpgcInfo() function.
  @param SubseqStartIndex                  - Specifies the initial SubSequence pointer where a Sequence first enters Loopback.Pattern (start of test).  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param SubseqEndIndex                    - When the subsequence pointed to by the SubseqEndIndex is completed the loop count is incremented by 1 and the current subsequence index is reset to the SubseqStartIndex.  The SubseqEndIndex must be greater than or equal to the SubseqStartIndex value.  The value must be less than the MaxNumberSubseqs value from the GetCpgcInfo() function.
  @param LoopCount                         - Specifies how many iterations of the complete sequence loop takes place before a test stops, where a sequence loop is defined by the completion of all subsequences indicated by the SubseqStartIndex to SubseqEndIndex. The value must be less than or equal to the MaxLoopCountVal from the GetCpgcInfo() function.  If the IsLoopCountExponential value from the GetCpgcInfo() function is TRUE then the number of loops is 2^(LoopCount-1).  If the IsLoopCountExponential value from the GetCpgcInfo() function is FALSE then the number of loops is simply LoopCount.  In both the linear of exponential cases, 0 is a special value means that the loop count is infinite.
  @param InitMode                          - Initialization mode.
  @param EnableGlobalControl               - Specifies whether global control is enabled for the given channel.
  @param EnableConstantWriteStrobe         - Specifies whether constant write strobe is enabled.
  @param EnableDummyReads                  - Specifies whether dummy reads are enabled.
  @param AddrUpdateRateMode                - Address update rate mode.

  @retval N/A

**/
VOID
BiosServerSetSeqConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  UINT32                StartDelay,
  UINT8                 SubseqStartIndex,
  UINT8                 SubseqEndIndex,
  UINT32                LoopCount,
  CPGC_INIT_MODE        InitMode,
  BOOLEAN               EnableGlobalControl,
  BOOLEAN               EnableConstantWriteStrobe,
  BOOLEAN               EnableDummyReads,
  ADDR_UPDATE_RATE_MODE AddrUpdateRateMode
  )
{
  CPGC_SEQCTL0_MC_MAIN_STRUCT                    seqCtl0;

  seqCtl0.Data = MemReadPciCfgMC(host, Socket, Controller, CPGC_SEQCTL0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)));

  seqCtl0.Bits.start_test_delay     = StartDelay;
  seqCtl0.Bits.subseq_start_pointer = SubseqStartIndex;
  seqCtl0.Bits.subseq_end_pointer   = SubseqEndIndex;
  seqCtl0.Bits.loopcount            = LoopCount;
  seqCtl0.Bits.initialization_mode  = InitMode;
  seqCtl0.Bits.global_control = (EnableGlobalControl)? 1 : 0;
  seqCtl0.Bits.enable_dummy_reads = (EnableDummyReads)? 1 : 0;
  seqCtl0.Bits.address_update_rate_mode = AddrUpdateRateMode;

  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQCTL0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), seqCtl0.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetLoopCount

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param LoopCount                         - Specifies how many iterations of the complete sequence loop takes place before a test stops, where a sequence loop is defined by the completion of all subsequences indicated by the SubseqStartIndex to SubseqEndIndex. The value must be less than or equal to the MaxLoopCountVal from the GetCpgcInfo() function.  If the IsLoopCountExponential value from the GetCpgcInfo() function is TRUE then the number of loops is 2^(LoopCount-1).  If the IsLoopCountExponential value from the GetCpgcInfo() function is FALSE then the number of loops is simply LoopCount.  In both the linear of exponential cases, 0 is a special value means that the loop count is infinite.

  @retval N/A

**/
VOID
BiosServerSetLoopCountChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  UINT32                LoopCount
  )
{
  CPGC_SEQCTL0_MC_MAIN_STRUCT            seqCtl0;

  seqCtl0.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQCTL0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)));
  seqCtl0.Bits.loopcount = LoopCount;
  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQCTL0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), seqCtl0.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetSeqAddrConfig

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param EnableRankWrapCarry               - If a base rank address wrap occurs  and EnableRankWrapCarry is set to TRUE then a carry increment or decrement from the rank address (order N) will propagate to the N+1 order address field(s).
  @param EnableBankWrapCarry               - If a base bank address wrap occurs  and EnableBankWrapCarry is set to TRUE then a carry increment or decrement from the bank address (order N) will propagate to the N+1 order address field(s).
  @param EnableRowWrapCarry                - If a base row address wrap occurs and EnableRowWrapCarry is set to TRUE then a carry increment or decrement from the row address (order N) will propagate to the N+1 order address field(s).
  @param EnableColWrapCarry                - If a base column address wrap occurs  and EnableColWrapCarry is set to TRUE then a carry increment or decrement from the column address (order N) will propagate to the N+1 order address field(s).
  @param EnableRankAddrInvert              - Setting to TRUE will cause the current rank address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param EnableBankAddrInvert              - Setting to TRUE will cause the current bank address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param EnableRowAddrInvert               - Setting to TRUE will cause the current row address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param EnableColAddrInvert               - Setting to TRUE will cause the current column address to be inverted with the periodicity described by the AddrInvertRate parameter.
  @param AddrInvertRate                    - Base address inversion rate.  The SetSeqConfig() function’s AddrUpdateRateMode parameter specifies whether the base address inversion logic is based on number of cachelines written and read or on the number of sequences performed.  If AddrUpdateRateMode from the SetSeqConfig() function is CachelineAddrUpdateRate then the current base address is inverted based on 2^AddrInvertRate read and write cacheline transactions.  If AddrUpdateRateMode from the SetSeqConfig() function is LoopCountAddrUpdateRate then the current base address is inverted based on 2^AddrInvertRate loops through the sequence.  The value must be less than or equal to the MaxAddrInvertRateVal value from the GetCpgcInfo() function.
  @param RankAddrOrder                     - Specifies the relative ordering of the rank address update logic in the nested for loop in relation to the bank, row, and column address update logic.  Any address fields set to the same order that roll over will cause a distinct carry of +1 or -1 to the next higher order address field (multiple simultaneous carries will cause only one increment/decrement).  All fields can be arbitrarily placed in any order as long as all address order fields are set in a continuous order starting from 0 and may not skip over an order number.  Example: setting 0,1,2,1 in the 4 fields (Col,Row,Bank,Rank) is legal; setting 0,2,2,3 is illegal (not continuous).  The value must be less than or equal to 3.
  @param BankAddrOrder                     - Specifies the relative ordering of the bank address update logic in the nested for loop in relation to the rank, row, and column address update logic.  The value must be less than or equal to 3.
  @param RowAddrOrder                      - Specifies the relative ordering of the row address update logic in the nested for loop in relation to the rank, bank, and column address update logic.  The value must be less than or equal to 3.
  @param ColAddrOrder                      - Specifies the relative ordering of the column address update logic in the nested for loop in relation to the rank, bank, and row address update logic.  The value must be less than or equal to 3.

  @retval N/A

**/
VOID
BiosServerSetSeqAddrConfigChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  BOOLEAN               EnableRankWrapCarry,
  BOOLEAN               EnableBankWrapCarry,
  BOOLEAN               EnableRowWrapCarry,
  BOOLEAN               EnableColWrapCarry,
  BOOLEAN               EnableRankAddrInvert,
  BOOLEAN               EnableBankAddrInvert,
  BOOLEAN               EnableRowAddrInvert,
  BOOLEAN               EnableColAddrInvert,
  UINT8                 AddrInvertRate,
  UINT8                 RankAddrOrder,
  UINT8                 BankAddrOrder,
  UINT8                 RowAddrOrder,
  UINT8                 ColAddrOrder
  )
{
  CPGC_SEQ_BAOCIC0_MC_MAIN_STRUCT        cpgcSeqBaoCic0;

  cpgcSeqBaoCic0.Data  = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_BAOCIC0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)));

  cpgcSeqBaoCic0.Bits.rank_bw_cryen = (EnableRankWrapCarry)? 1 : 0;
  cpgcSeqBaoCic0.Bits.bank_bw_cryen = (EnableBankWrapCarry)? 1 : 0;
  cpgcSeqBaoCic0.Bits.row_bw_cryen = (EnableRowWrapCarry)? 1 : 0;
  cpgcSeqBaoCic0.Bits.column_bw_cryen = (EnableColWrapCarry)? 1 : 0;
  cpgcSeqBaoCic0.Bits.rank_ba_inven = (EnableRankAddrInvert)? 1 : 0;
  cpgcSeqBaoCic0.Bits.bank_ba_inven = (EnableBankAddrInvert)? 1 : 0;
  cpgcSeqBaoCic0.Bits.row_ba_inven = (EnableRowAddrInvert)? 1 : 0;
  cpgcSeqBaoCic0.Bits.column_ba_inven = (EnableColAddrInvert)? 1 : 0;
  cpgcSeqBaoCic0.Bits.addr_inv_rate = AddrInvertRate;
  //Order; only supported order is Col, Row, Bank, Rank
  cpgcSeqBaoCic0.Bits.column_addr_order = ColAddrOrder;
  cpgcSeqBaoCic0.Bits.row_addr_order    = RowAddrOrder;
  cpgcSeqBaoCic0.Bits.bank_addr_order   = BankAddrOrder;
  cpgcSeqBaoCic0.Bits.rank_addr_order   = RankAddrOrder;

  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_BAOCIC0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), cpgcSeqBaoCic0.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetSeqStartAddr

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param RankAddr                          - Rank start address.  This value is the index into the CPGC rank map (see the SetSeqRankMap() function).  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param BankAddr                          - Bank start address.  If the CPGC engine supports a bank map (i.e., the MaxNumberBankMapEntried value from the GetCpgcInfo() function is greater than 0) then this value is is the index into the CPGC bank map (see the SetSeqBankMap() function) and the value must be less than the MaxNumberBankMapEntried value from the GetCpgcInfo() function.  If the CPGC engine does not support a bank map then the value must be less than the BankCount value from the GetDimmInfo function for the DIMMs that will be tested.
  @param RowAddr                           - Row start address.  The value must be less than the RowSize value from the GetDimmInfo function for the DIMMs that will be tested.
  @param ColAddr                           - Column start address.  The value must be less than the ColumnSize value from the GetDimmInfo function for the DIMMs that will be tested.

  @retval N/A

**/
VOID
BiosServerSetSeqStartAddrChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  UINT8                 RankAddr,
  UINT8                 BankAddr,
  UINT32                RowAddr,
  UINT32                ColAddr
  )
{
  CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_STRUCT       seqBaseAddrStartLo0;
  CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_STRUCT       seqBaseAddrStartHi0;

  seqBaseAddrStartLo0.Data = 0;
  seqBaseAddrStartLo0.Bits.column_addr = ColAddr;
  seqBaseAddrStartLo0.Bits.bank_addr   = BankAddr;
  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_BASEADDRSTART_LO0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), seqBaseAddrStartLo0.Data);

  seqBaseAddrStartHi0.Data = 0;
  seqBaseAddrStartHi0.Bits.row_addr    = RowAddr;
  seqBaseAddrStartHi0.Bits.rank_addr   = RankAddr;
  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_BASEADDRSTART_HI0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), seqBaseAddrStartHi0.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetSeqWrapAddr

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param RankAddr                          - Rank start address.  This value is the index into the CPGC rank map (see the SetSeqRankMap() function).  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param BankAddr                          - Bank start address.  If the CPGC engine supports a bank map (i.e., the MaxNumberBankMapEntried value from the GetCpgcInfo() function is greater than 0) then this value is is the index into the CPGC bank map (see the SetSeqBankMap() function) and the value must be less than the MaxNumberBankMapEntried value from the GetCpgcInfo() function.  If the CPGC engine does not support a bank map then the value must be less than the BankCount value from the GetDimmInfo function for the DIMMs that will be tested.
  @param RowAddr                           - Row start address.  The value must be less than the RowSize value from the GetDimmInfo function for the DIMMs that will be tested.
  @param ColAddr                           - Column start address.  The value must be less than the ColumnSize value from the GetDimmInfo function for the DIMMs that will be tested.

  @retval N/A

**/
VOID
BiosServerSetSeqWrapAddrChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  UINT8                 RankAddr,
  UINT8                 BankAddr,
  UINT32                RowAddr,
  UINT32                ColAddr
)
{
  CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_STRUCT     seqBaseAddrWrapHi0;
  CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_STRUCT     seqBaseAddrWrapLo0;

  seqBaseAddrWrapHi0.Data = 0;
  seqBaseAddrWrapHi0.Bits.row_addr   = RowAddr;
  seqBaseAddrWrapHi0.Bits.rank_addr  = RankAddr;
  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_BASEADDRWRAP_HI0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), seqBaseAddrWrapHi0.Data);

  seqBaseAddrWrapLo0.Data = 0;
  seqBaseAddrWrapLo0.Bits.column_addr = ColAddr;
  seqBaseAddrWrapLo0.Bits.bank_addr   = BankAddr;
  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_BASEADDRWRAP_LO0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), seqBaseAddrWrapLo0.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetSeqAddrInc

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param RankAddrIncMode                   - Rank address increment rate mode.
  @param RankAddrIncRate                   - Rank address increment rate.  In linear mode (see RankAddrIncMode), RankAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the RankAddrIncVal.  In exponential mode, 2^RankAddrIncRate defines how many times a specific rank address is repeated before adding the RankAddrIncVal.  If RankAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxRankAddrIncRateExponentVal from the GetCpgcInfo() function.  If RankAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxRankAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param RankAddrIncVal                    - Rank start address increment value.  When dictated by RankAddrIncRate the current rank address will be modified by the RankAddrIncVal (which is a 2's compliment field).  A value of 0 means the rank address increment is effectively disabled since no offset will be applied.  Disabling the rank address increment by setting RankAddrIncVal to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinRankAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxRankAddrIncVal from the GetCpgcInfo() function.
  @param BankAddrIncMode                   - Bank address increment rate mode.
  @param BankAddrIncRate                   - Bank address increment rate.  In linear mode (see BankAddrIncMode), BankAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the BankAddrIncVal.  In exponential mode, 2^BankAddrIncRate defines how many times a specific rank address is repeated before adding the BankAddrIncVal.  If BankAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxBankAddrIncRateExponentVal from the GetCpgcInfo() function.  If BankAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxBankAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param BankAddrIncVal                    - Bank start address increment value.  When dictated by BankAddrIncRate the current bank address will be modified by the BankAddrIncVal (which is a 2's compliment field).  A value of 0 means the bank address increment is effectively disabled since no offset will be applied.  Disabling the bank address increment by setting BankAddrIncVal to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinBankAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxBankAddrIncVal from the GetCpgcInfo() function.
  @param RowAddrIncMode                    - Row address increment rate mode.
  @param RowAddrIncRate                    - Row address increment rate.  In linear mode (see RowAddrIncMode), RowAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the RowAddrIncVal.  In exponential mode, 2^RowAddrIncRate defines how many times a specific rank address is repeated before adding the RowAddrIncVal.  If RowAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxRowAddrIncRateExponentVal from the GetCpgcInfo() function.  If RowAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxRowAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param RowAddrIncVal                     - Row start address increment value.  When dictated by RowAddrIncRate the current row address will be modified by the RowAddrIncVal (which is a 2's compliment field.  A value of 0 means the row address increment is effectively disabled since no offset will be applied.  Disabling the row address increment by setting RowAddrIncVal to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinRowAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxRowAddrIncVal from the GetCpgcInfo() function.
  @param ColAddrIncMode                    - Column address increment rate mode.
  @param ColAddrIncRate                    - Column address increment rate.  If ColAddrIncMode is ExponentialAddrIncMode then 2^ColAddrIncRate defines how many times a specific rank address is repeated before adding the ColAddrIncVal.  If ColAddrIncMode is LinearAddrIncMode then ColAddrIncRate+1 specifies how many times a specific rank address is repeated before adding the ColAddrIncVal.  If ColAddrIncMode is ExponentialAddrIncMode then the value must be less than or equal to the MaxColAddrIncRateExponentVal from the GetCpgcInfo() function.  If ColAddrIncMode is LinearAddrIncMode then the value must be less than or equal to the MaxColAddrIncRateLinearVal from the GetCpgcInfo() function.
  @param ColAddrIncVal                     - Column start address increment value.  When dictated by ColAddrIncRate the current column address will be modified by the ColAddrIncVal (which is a 2's compliment field).  A value of 0 means the column address increment is effectively disabled since no offset will be applied.  Disabling the column address increment by setting column address increment to 0 does not affect a carry from a lower order field rolling over to a higher order field which will also result in an increment or decrement to the higher order field of +1 or -1.  Note that a positive value in this field will cause a +1 carry to propagate while a negative value will cause a -1 carry to propagate from order N to order N+1.  The value must be greater than or equal to the MinColAddrIncVal from the GetCpgcInfo() function.  The value must be less than or equal to the MaxColAddrIncVal from the GetCpgcInfo() function.

  @retval N/A

**/
VOID
BiosServerSetSeqAddrIncChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  ADDR_INC_MODE         RankAddrIncMode,
  UINT32                RankAddrIncRate,
  INT8                  RankAddrIncVal,
  ADDR_INC_MODE         BankAddrIncMode,
  UINT32                BankAddrIncRate,
  INT8                  BankAddrIncVal,
  ADDR_INC_MODE         RowAddrIncMode,
  UINT32                RowAddrIncRate,
  INT16                 RowAddrIncVal,
  ADDR_INC_MODE         ColAddrIncMode,
  UINT32                ColAddrIncRate,
  INT16                 ColAddrIncVal
)
{
  CPGC_SEQ_BAINCCTL00_MC_MAIN_STRUCT                 seqBAIncCtl00;
  CPGC_SEQ_BAINCCTL10_MC_MAIN_STRUCT                 seqBAIncCtl10;

  // Rank
  seqBAIncCtl10.Bits.rank_ba_inc = RankAddrIncVal;
  seqBAIncCtl10.Bits.rank_ba_updrate = RankAddrIncRate;
  seqBAIncCtl10.Bits.rank_ba_updscale = RankAddrIncMode;

  // Bank
  seqBAIncCtl10.Bits.bank_ba_inc = BankAddrIncVal;
  seqBAIncCtl10.Bits.bank_ba_updrate = BankAddrIncRate;
  seqBAIncCtl10.Bits.bank_ba_updscale = BankAddrIncMode;

  // Row
  seqBAIncCtl00.Bits.row_ba_inc = RowAddrIncVal;
  seqBAIncCtl00.Bits.row_ba_updrate = RowAddrIncRate;
  seqBAIncCtl00.Bits.row_ba_updscale = RowAddrIncMode;

  // Col
  seqBAIncCtl00.Bits.column_ba_inc = ColAddrIncVal;
  seqBAIncCtl00.Bits.column_ba_updrate = ColAddrIncRate;
  seqBAIncCtl00.Bits.column_ba_updscale = ColAddrIncMode;

  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_BAINCCTL10_MC_MAIN_REG + (8 * GetMCCh(Controller, ChannelInSocket)), seqBAIncCtl10.Data);
  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_BAINCCTL00_MC_MAIN_REG + (8 * GetMCCh(Controller, ChannelInSocket)), seqBAIncCtl00.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetSeqRankMap

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param PairCount                         - Number of entries in the DimmRankPairs array.  The minimum value is 1.  The value must be less than or equal to the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param DimmRankPairs                     - Pointer to array of DIMM and physical rank pair values.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.  Note that the same rank may appear more than once in the map.  Note also that all ranks do not need to appear in the map.

  @retval N/A

**/
VOID
BiosServerSetSeqRankMapChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  UINT8                 PairCount,
  CONST DIMM_RANK_PAIR  *DimmRankPairs
)
{
  UINT8                                                  i;
  UINT8                                                  logicalRankMapping[MAX_RANK_CH] = {0};
  UINT8                                                  logicalToSubRankMapping[MAX_RANK_CH] = {0};
  CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_STRUCT                 cpgcSeqRankLg2PhyMap0;  //Save the LogicalRanks of the DimmRankPairs
  CPGC_SEQ_LRRANKLG2PHYMAP0_MC_MAIN_STRUCT               seqLrRankLg2PhyMap0;    //Save the SubRanks of the LogicalRanks of the DimmRankPairs (when in encodedModeCS)
  struct channelNvram                                    (*channelNvList)[MAX_CH];
  struct dimmNvram                                       (*dimmNvList)[MAX_DIMM];

  channelNvList = GetChannelNvList(host, Socket);
  dimmNvList    = GetDimmNvList(host, Socket, ChannelInSocket);

  //DimmRankPairs -> Pointer to the base address of an array of type DIMM_RANK_PAIR, PairCount -> size of the array (8 locations since MAX_RANK_CH = 8)
  //The Array DimmRankPairs[i].. we use these Dimm/Rank pairs to calculate the logical rank for them and save it in logicalRankMapping[i]

  //Based on the DimmRankPair we calculate the list of LogicalRanks
  for (i = 0; i < PairCount; i++) {
    if ((((DimmRankPairs + i)->Dimm >= MAX_DIMM) || ((DimmRankPairs + i)->Rank >= MAX_RANK_DIMM)) == 0) {
      SSAGetLogicalRank(host, Socket, ChannelInSocket, (DimmRankPairs + i)->Dimm , (DimmRankPairs + i)->Rank, &logicalRankMapping[i]);  //Fill Array logicalRankMapping with LogicalRank values
    }
  }

  //We save all the LogicalRank value we have in the logicalRankMapping[] array into the respective 8 register bits below
  cpgcSeqRankLg2PhyMap0.Data = 0;
  cpgcSeqRankLg2PhyMap0.Bits.rank0_mapping = logicalRankMapping[0];
  cpgcSeqRankLg2PhyMap0.Bits.rank1_mapping = logicalRankMapping[1];
  cpgcSeqRankLg2PhyMap0.Bits.rank2_mapping = logicalRankMapping[2];
  cpgcSeqRankLg2PhyMap0.Bits.rank3_mapping = logicalRankMapping[3];
  cpgcSeqRankLg2PhyMap0.Bits.rank4_mapping = logicalRankMapping[4];
  cpgcSeqRankLg2PhyMap0.Bits.rank5_mapping = logicalRankMapping[5];
  cpgcSeqRankLg2PhyMap0.Bits.rank6_mapping = logicalRankMapping[6];
  cpgcSeqRankLg2PhyMap0.Bits.rank7_mapping = logicalRankMapping[7];
  MemWritePciCfgMC (host, Socket,Controller, CPGC_SEQ_RANKLG2PHYMAP0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), cpgcSeqRankLg2PhyMap0.Data);

  // if i < paircnt; cpgcSeqRankLg2PhyMap0.Bits.rank[i]_mapping = logicalRankMapping[i]
  // if i >= paricnt, cpgcSeqRankLg2PhyMap0.Bits.rank[i]_mapping = 0 //By Default all logicalRankMapping Array elements are set to 0
#ifdef LRDIMM_SUPPORT
  if ((*channelNvList)[ChannelInSocket].encodedCSMode != 0) {
    for (i = 0; i < PairCount; i++) {
      if ((IsLrdimmPresent(host, Socket, ChannelInSocket, (DimmRankPairs + i)->Dimm) && (*dimmNvList)[(DimmRankPairs + i)->Dimm].aepDimmPresent == 0) || 
        ((*channelNvList)[ChannelInSocket].encodedCSMode == 2)) {
        //Based on the Dimm/Rank pairs PhysicalRank value entered we calculate the Sub-Rank
        logicalToSubRankMapping[i] = ((DimmRankPairs + i)->Rank) >> 1;
      } 
    }

    //Saving the Sub-Rank values for the corresponding LogicalRank values
    seqLrRankLg2PhyMap0.Data = 0;
    seqLrRankLg2PhyMap0.Bits.lr0_mapping = logicalToSubRankMapping[0];
    seqLrRankLg2PhyMap0.Bits.lr1_mapping = logicalToSubRankMapping[1];
    seqLrRankLg2PhyMap0.Bits.lr2_mapping = logicalToSubRankMapping[2];
    seqLrRankLg2PhyMap0.Bits.lr3_mapping = logicalToSubRankMapping[3];
    seqLrRankLg2PhyMap0.Bits.lr4_mapping = logicalToSubRankMapping[4];
    seqLrRankLg2PhyMap0.Bits.lr5_mapping = logicalToSubRankMapping[5];
    seqLrRankLg2PhyMap0.Bits.lr6_mapping = logicalToSubRankMapping[6];
    seqLrRankLg2PhyMap0.Bits.lr7_mapping = logicalToSubRankMapping[7];

    //Set the lrdimm_rank_mapping bits to determine the Number of Sub-Ranks on the DIMMs
    //We get the "numRowBits" from the first Dimm in the array
    //If the dimm has 16 row address bits [0:15] start appending the subrank after a[15]
    if (host->nvram.mem.dramType == SPD_TYPE_DDR4) {
      if ((*dimmNvList)[(DimmRankPairs + 0)->Dimm].numRowBits == 17) {
        seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 4;
      } else if ((*dimmNvList)[(DimmRankPairs + 0)->Dimm].numRowBits == 16) {
        seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 2;
      } else if ((*dimmNvList)[(DimmRankPairs + 0)->Dimm].numRowBits == 15) {
        seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 1;
      }
      //add for 3DS support
      if ((*channelNvList)[ChannelInSocket].encodedCSMode == 2) {
        if ((*dimmNvList)[(DimmRankPairs + 0)->Dimm].numRowBits  == 16) {
          seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 2;
        }
      }
    } else {
      if (((*dimmNvList)[(DimmRankPairs + 0)->Dimm].numRowBits - ((*dimmNvList)[(DimmRankPairs + 0)->Dimm].lrRankMult >> 1)) == 16) {
        seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 2;
      } else if (((*dimmNvList)[(DimmRankPairs + 0)->Dimm].numRowBits - ((*dimmNvList)[(DimmRankPairs + 0)->Dimm].lrRankMult >> 1)) == 15) {
        seqLrRankLg2PhyMap0.Bits.lrdimm_rank_mapping = 1;
      }
    }
    //seqLrRankLg2PhyMap0.Bits.lr0_mapping = SubRankBitMask;
    MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_LRRANKLG2PHYMAP0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), seqLrRankLg2PhyMap0.Data);
  }    // encodedCSMode != 0

#endif //LRDIMM
}

/**

  This routine sets the CSRs required by BiosServerSetSeqRowAddrSwizzle

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param SwizzleCount                      - Number of entries in the Swizzles array.  The value must be less than or equal to the MaxNumberRowAddrSwizzleEntries value from the GetCpgcInfo() function.  If the SwizzleCount value is 0 (and the Swizzles value is NULL) then the row address swizzling will be configured so that all the lower row addresses are mapped to themselves; effectively disabling the swizzling.
  @param Swizzles                          - Pointer to array of lower row address swizzle values.  The values must be unique and less than or equal to the MaxRowAddrSwizzleVal value from the GetCpgcInfo() function.  The array position indicates the row address that the specified row address will be swapped with, e.g., a value of 10 at the array index 1 means that row addresses 10 and 1 will be swapped.

  @retval N/A

**/
VOID
BiosServerSetSeqRowAddrSwizzleChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  UINT8                 SwizzleCount,
  CONST UINT8           *Swizzles
)
{
  UINT8                                       i;
  CPGC_SEQ_RASL0_MC_MAIN_STRUCT               cpgcRaslReg;   //This register is used to swizzle the Lower Logical to Physical Row Bits.
  CPGC_SEQ_RASU00_MC_MAIN_STRUCT              cpgcRasu00Reg; //Part-0: This register is used to swizzle the Upper Logical to Physical Row Bits.
  CPGC_SEQ_RASU10_MC_MAIN_STRUCT              cpgcRasu01Reg; //Part-1: This register is used to swizzle the Upper Logical to Physical Row Bits.

  for (i = 0; i < LOWER_ROW_ADDR_SWIZZLE_CNT; i++) {
    if (i >= SwizzleCount) continue;  //Program only those bits until SwizzleCount is reached
    cpgcRaslReg.Data  = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_RASL0_MC_MAIN_REG + 4 * GetMCCh(Controller, ChannelInSocket));
    if (i == 0) cpgcRaslReg.Bits.log2phy_row0_swzl = *(Swizzles + 0);
    if (i == 1) cpgcRaslReg.Bits.log2phy_row1_swzl = *(Swizzles + 1);
    if (i == 2) cpgcRaslReg.Bits.log2phy_row2_swzl = *(Swizzles + 2);
    if (i == 3) cpgcRaslReg.Bits.log2phy_row3_swzl = *(Swizzles + 3);
    if (i == 4) cpgcRaslReg.Bits.log2phy_row4_swzl = *(Swizzles + 4);
    if (i == 5) cpgcRaslReg.Bits.log2phy_row5_swzl = *(Swizzles + 5);
    MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_RASL0_MC_MAIN_REG + 4 * GetMCCh(Controller, ChannelInSocket), cpgcRaslReg.Data);
  } // 4 reg.. each offset by 4bytes ..  GetCpgcRegOffset (host, Socket, ChannelInSocket

  for (i = 0; i < (UPPER_ROW_ADDR_SWIZZLE_CNT - LOWER_ROW_ADDR_SWIZZLE_CNT); i++) {  //12-6
    cpgcRasu00Reg.Data  = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_RASU00_MC_MAIN_REG + 8 * GetMCCh(Controller, ChannelInSocket));
    if (i >= SwizzleCount) continue;
    if (*(Swizzles + i) == 6) cpgcRasu00Reg.Bits.log2phy_row6_swzl = *(Swizzles + i);
    if (*(Swizzles + i) == 7) cpgcRasu00Reg.Bits.log2phy_row7_swzl = *(Swizzles + i);
    if (*(Swizzles + i) == 8) cpgcRasu00Reg.Bits.log2phy_row8_swzl = *(Swizzles + i);
    if (*(Swizzles + i) == 9) cpgcRasu00Reg.Bits.log2phy_row9_swzl = *(Swizzles + i);
    if (*(Swizzles + i) == 10) cpgcRasu00Reg.Bits.log2phy_row10_swzl = *(Swizzles + i);
    if (*(Swizzles + i) == 11) cpgcRasu00Reg.Bits.log2phy_row11_swzl = *(Swizzles + i);
    MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_RASU00_MC_MAIN_REG + 8 * GetMCCh(Controller, ChannelInSocket), cpgcRasu00Reg.Data);
  }//GetCpgcRegOffset (host, Socket, ChannelInSocket, CPGC_SEQ_RASU00_MC_MAINEXT_REG)

  for (i = 0; i <= (MAX_ROW_ADDR_SWIZZLE_VAL - UPPER_ROW_ADDR_SWIZZLE_CNT); i++) {  //16-12
    cpgcRasu01Reg.Data  = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_RASU10_MC_MAIN_REG + 8 * GetMCCh(Controller, ChannelInSocket));
    if (i >= SwizzleCount) continue;
    if (*(Swizzles + i) == 12) cpgcRasu01Reg.Bits.log2phy_row12_swzl = *(Swizzles + i);
    if (*(Swizzles + i) == 13) cpgcRasu01Reg.Bits.log2phy_row13_swzl = *(Swizzles + i);
    if (*(Swizzles + i) == 14) cpgcRasu01Reg.Bits.log2phy_row14_swzl = *(Swizzles + i);
    if (*(Swizzles + i) == 15) cpgcRasu01Reg.Bits.log2phy_row15_swzl = *(Swizzles + i);
    if (*(Swizzles + i) == 16) cpgcRasu01Reg.Bits.log2phy_row16_swzl = *(Swizzles + i);
    MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQ_RASU10_MC_MAIN_REG + 8 * GetMCCh(Controller, ChannelInSocket), cpgcRasu01Reg.Data);
  }
}

/**

  This routine gets the CSRs required by BiosServerGetSeqState

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param SeqState                          - Pointer to buffer where sequence state will be stored.

  @retval N/A

**/
VOID
BiosServerGetSeqStateChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 Controller,
  UINT8                 ChannelInSocket,
  SSA_SEQ_STATE         *SeqState
)
{
  CPGC_SEQ_LPCNT_STS0_MC_MAIN_STRUCT                  cpgcSeqLpcnt0;
  CPGC_SEQ_LPCNT_STS1_MC_MAIN_STRUCT                  cpgcSeqLpcnt1;
  CPGC_SEQ_LPCNT_STS2_MC_MAIN_STRUCT                  cpgcSeqLpcnt2;
  CPGC_SEQ_SUBSEQ_PNTR_MC_MAIN_STRUCT                 cpgcSeqSubseqPntr;
  CPGC_SEQ_CLSTS0_MC_MAIN_STRUCT                      cpgcSeqClst0;
  CPGC_SEQ_CLSTS1_MC_MAIN_STRUCT                      cpgcSeqClst1;
  CPGC_SEQ_CLSTS2_MC_MAIN_STRUCT                      cpgcSeqClst2;
  CPGC_SEQ_BASEADDRCURR_LO0_MC_MAIN_STRUCT            cpgcSeqBaseAddCurrL0;
  CPGC_SEQ_BASEADDRCURR_LO1_MC_MAIN_STRUCT            cpgcSeqBaseAddCurrL1;
  CPGC_SEQ_BASEADDRCURR_LO2_MC_MAIN_STRUCT            cpgcSeqBaseAddCurrL2;
  CPGC_SEQ_BASEADDRCURR_HI0_MC_MAIN_STRUCT            cpgcSeqBaseAddCurrHi0;
  CPGC_SEQ_BASEADDRCURR_HI1_MC_MAIN_STRUCT            cpgcSeqBaseAddCurrHi1;
  CPGC_SEQ_BASEADDRCURR_HI2_MC_MAIN_STRUCT            cpgcSeqBaseAddCurrHi2;
  CPGC_SEQ_OFFADDRCUR0_MC_MAIN_STRUCT                 cpgcSeqOffAddCur0;
  CPGC_SEQ_OFFADDRCUR1_MC_MAIN_STRUCT                 cpgcSeqOffAddCur1;
  CPGC_SEQ_OFFADDRCUR2_MC_MAIN_STRUCT                 cpgcSeqOffAddCur2;
  CPGC_SEQ_OFFADDRCUR0_ROW_MC_MAIN_STRUCT             cpgcSeqOffAddCur0Row;
  CPGC_SEQ_OFFADDRCUR1_ROW_MC_MAIN_STRUCT             cpgcSeqOffAddCur1Row;
  CPGC_SEQ_OFFADDRCUR2_ROW_MC_MAIN_STRUCT             cpgcSeqOffAddCur2Row;

  //MC0: CH 0/1/2... MC1: CH 0/1/2
  if (ChannelInSocket == 0) {

    cpgcSeqLpcnt0.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_LPCNT_STS0_MC_MAIN_REG);  //MC0(Ch2:0) & MC1(Ch5:3) CPGC_SEQ_LPCNT_STS3_MC_MAIN_REG --> not used
    SeqState->LoopCount = (UINT32) cpgcSeqLpcnt0.Bits.current_loopcount;

    cpgcSeqSubseqPntr.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_SUBSEQ_PNTR_MC_MAIN_REG);  //Controller-> sets up the correct register; 2 copies of this CSR: One for MC0(Ch2:0), MC1(Ch5:3) -> currentpntr3 not used
    SeqState->SubSeqIndex = (UINT8) cpgcSeqSubseqPntr.Bits.currentpntr0;  //currentpntr0 is used for both Ch0 and Ch3 (since, MC number changes)

    cpgcSeqClst0.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_CLSTS0_MC_MAIN_REG);
    SeqState->Cacheline = (UINT32) cpgcSeqClst0.Bits.current_cl;

    cpgcSeqBaseAddCurrL0.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_BASEADDRCURR_LO0_MC_MAIN_REG);
    SeqState->BaseCol = (UINT32) cpgcSeqBaseAddCurrL0.Bits.column_addr;
    SeqState->BaseBank = (UINT8) cpgcSeqBaseAddCurrL0.Bits.bank_addr;

    cpgcSeqBaseAddCurrHi0.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_BASEADDRCURR_HI0_MC_MAIN_REG);
    SeqState->BaseRank = (UINT8) cpgcSeqBaseAddCurrHi0.Bits.rank_addr;
    SeqState->BaseRow = (UINT32) cpgcSeqBaseAddCurrHi0.Bits.row_addr;

    cpgcSeqOffAddCur0.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_OFFADDRCUR0_MC_MAIN_REG);
    SeqState->OffsetCol = (UINT32) cpgcSeqOffAddCur0.Bits.col_addr;

    cpgcSeqOffAddCur0Row.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_OFFADDRCUR0_ROW_MC_MAIN_REG);
    SeqState->OffsetRow = (UINT32) cpgcSeqOffAddCur0Row.Bits.row_addr;

  } else if (ChannelInSocket == 1) {

    cpgcSeqLpcnt1.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_LPCNT_STS1_MC_MAIN_REG);
    SeqState->LoopCount = (UINT32) cpgcSeqLpcnt1.Bits.current_loopcount;

    cpgcSeqSubseqPntr.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_SUBSEQ_PNTR_MC_MAIN_REG);
    SeqState->SubSeqIndex = (UINT8) cpgcSeqSubseqPntr.Bits.currentpntr1;

    cpgcSeqClst1.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_CLSTS1_MC_MAIN_REG);
    SeqState->Cacheline = (UINT32) cpgcSeqClst1.Bits.current_cl;

    cpgcSeqBaseAddCurrL1.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_BASEADDRCURR_LO1_MC_MAIN_REG);
    SeqState->BaseCol = (UINT32) cpgcSeqBaseAddCurrL1.Bits.column_addr;
    SeqState->BaseBank = (UINT8) cpgcSeqBaseAddCurrL1.Bits.bank_addr;

    cpgcSeqBaseAddCurrHi1.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_BASEADDRCURR_HI1_MC_MAIN_REG);
    SeqState->BaseRank = (UINT8) cpgcSeqBaseAddCurrHi1.Bits.rank_addr;
    SeqState->BaseRow = (UINT32) cpgcSeqBaseAddCurrHi1.Bits.row_addr;

    cpgcSeqOffAddCur1.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_OFFADDRCUR1_MC_MAIN_REG);
    SeqState->OffsetCol = (UINT32) cpgcSeqOffAddCur1.Bits.col_addr;

    cpgcSeqOffAddCur1Row.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_OFFADDRCUR1_ROW_MC_MAIN_REG);
    SeqState->OffsetRow = (UINT32) cpgcSeqOffAddCur1Row.Bits.row_addr;

  } else if (ChannelInSocket == 2) {

    cpgcSeqLpcnt2.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_LPCNT_STS2_MC_MAIN_REG);
    SeqState->LoopCount = (UINT32) cpgcSeqLpcnt2.Bits.current_loopcount;

    cpgcSeqSubseqPntr.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_SUBSEQ_PNTR_MC_MAIN_REG);
    SeqState->SubSeqIndex = (UINT8) cpgcSeqSubseqPntr.Bits.currentpntr2;

    cpgcSeqClst2.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_CLSTS2_MC_MAIN_REG);
    SeqState->Cacheline = (UINT32) cpgcSeqClst2.Bits.current_cl;

    cpgcSeqBaseAddCurrL2.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_BASEADDRCURR_LO2_MC_MAIN_REG);
    SeqState->BaseCol = (UINT32) cpgcSeqBaseAddCurrL2.Bits.column_addr;
    SeqState->BaseBank = (UINT8) cpgcSeqBaseAddCurrL2.Bits.bank_addr;

    cpgcSeqBaseAddCurrHi2.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_BASEADDRCURR_HI2_MC_MAIN_REG);
    SeqState->BaseRank = (UINT8) cpgcSeqBaseAddCurrHi2.Bits.rank_addr;
    SeqState->BaseRow = (UINT32) cpgcSeqBaseAddCurrHi2.Bits.row_addr;

    cpgcSeqOffAddCur2.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_OFFADDRCUR2_MC_MAIN_REG);
    SeqState->OffsetCol = (UINT32) cpgcSeqOffAddCur2.Bits.col_addr;

    cpgcSeqOffAddCur2Row.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQ_OFFADDRCUR2_ROW_MC_MAIN_REG);
    SeqState->OffsetRow = (UINT32) cpgcSeqOffAddCur2Row.Bits.row_addr;

  }
}

/**

  This routine sets the CSRs required by BiosServerSetStopMode

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param StopMode                          - CPGC stop mode.
  @param StopOnNthErrorCount               - Error count for stop-on-nth-any-lane error mode.  Minimum value is 1.  The value must be less than or equal to the MaxStopOnNthErrorCountVal value from the GetCpgcInfo() function.

  @retval N/A

**/
VOID
BiosServerSetStopModeChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  CPGC_STOP_MODE        StopMode,
  UINT32                StopOnNthErrorCount
)
{
  CPGC_ERR_CTL_MCDDC_CTL_STRUCT            errCtl;

  errCtl.Data                         = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_CTL_MCDDC_CTL_REG);
  errCtl.Bits.stop_on_ntherr          = StopOnNthErrorCount;
  errCtl.Bits.stop_on_errcontrol      = StopMode;

  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_CTL_MCDDC_CTL_REG, errCtl.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetErrorCounterMode

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param Counter                           - Zero based error counter number.  The value must be less than or equal to the MaxNumberErrorCounters value from the GetCpgcInfo() function.
  @param Mode                              - Error counter mode.
  @param ModeIndex                         - Extra index used to provide additional information if needed by the mode selected. This indicates which lane, nibble, byte, or chunk has been selected.  If CounterMode value is LaneErrorCounterMode then the ModeIndex value must be less than the BusWidth value from the GetSystemInfo() function.  If CounterMode is NibbleErrorCounterMode then the ModeIndex value must be less than the BusWidth/4 value from the GetSystemInfo() function.  If CounterMode is ByteErrorCounterMode then the ModeIndex value must be less than the BusWidth/8 value from the GetSystemInfo() function.  If CounterMode is ChunkErrorCounterMode then the ModeIndex value must be less than 8.

  @retval N/A

**/
VOID
BiosServerSetErrorCounterModeChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 Counter,
  ERROR_COUNTER_MODE    Mode,
  UINT32                ModeIndex
)
{
  UINT32                                  Offset = 0;
  CPGC_ERR_COUNTER0_MCDDC_CTL_STRUCT      errCounterReg;

  // NotAvailable if the given Mode value is not supported - Available on HSX
  errCounterReg.Data = 0; // We are initializing to 0 all the error counter bits/registers as per the I/P
  errCounterReg.Bits.ctl = Mode;
  errCounterReg.Bits.pntr = ModeIndex; //Counter_Pointer, to indicate what lane or byte group is desired for tracking Errors.. This indicates which lane, nibble, byte, or chunk has been selected.
  Offset = (CPGC_ERR_COUNTER0_MCDDC_CTL_REG + (Counter * 8)); //*8 as there is one Status register in between

  MemWritePciCfgEp (host, Socket, ChannelInSocket, Offset, errCounterReg.Data); //31:0
}

/**

  This routine sets the CSRs required by BiosServerSetLaneValidationMask

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param Counter                           - Zero based error counter number.  The value must be less than or equal to the MaxNumberErrorCounters value from the GetCpgcInfo() function.
  @param DqMaskLow                         - Lower 32 bits of the DQ Mask (0-based)
  @param DqMaskHigh                        - Upper 32 bits of the DQ Mask (0-based)
  @param EccMask                           - Ecc Mask (0-based)


  @retval N/A

**/
VOID
BiosServerSetLaneValidationMaskChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT32                DqMaskLow,
  UINT32                DqMaskHigh,
  UINT32                EccMask32
)
{
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_DATA0_MCDDC_CTL_REG, DqMaskLow);  //31:0
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_DATA1_MCDDC_CTL_REG, DqMaskHigh);  // 63:32
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_ECC_MCDDC_CTL_REG, EccMask32);  //71:64 - ECC
}

/**

  This routine sets the CSRs required by BiosServerSetChunkValidationMask

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param ChunkPairsToCheck                 - One bit for each chunk of a cacheline. Used to identify each pair of chunks to be checked.

  @retval N/A

**/
VOID
BiosServerSetChunkValidationMaskChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 ChunkPairsToCheck
)
{
  CPGC_ERR_CTL_MCDDC_CTL_STRUCT           cpgcErrorCtlReg;

  //The register has only 4 bits set to track the 8 chunk's. So, each bit corresponds to a ChunkPair
  //Does 4 checks of Chunk Pairs, bits 0,1  2,3  4,5 and 6,7, sets ChunkPairsToCheck BIT0 to 1 if chunk 0,1 enabled,  BIT1 to 1 if chunk 2,3 enabled so on...
  cpgcErrorCtlReg.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_CTL_MCDDC_CTL_REG);
  cpgcErrorCtlReg.Bits.selective_err_enb_chunk = ChunkPairsToCheck;
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_CTL_MCDDC_CTL_REG, cpgcErrorCtlReg.Data);
  //if Input ChunkMask = 11001111, ChunkPairsToCheck = 1011
}

/**

  This routine sets the CSRs required by BiosServerSetCachelineValidationMask

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param CachelineMask                     - Mask for cachelines to be enabled.  Bit value 1 = perform validation; bit value 0 = do not perform validation.  Bit position 0 = cacheline 0; bit position 1 = cacheline 1, etc.  Note that this may not be the same logic as the hardware.

  @retval N/A

**/
VOID
BiosServerSetCachelineValidationMaskChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 CachelineMask
)
{
  CPGC_ERR_CTL_MCDDC_CTL_STRUCT         errCtl;

  errCtl.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_CTL_MCDDC_CTL_REG);
  errCtl.Bits.selective_err_enb_cl = CachelineMask; // sets these bits on the cacheline to be checked for errors
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_CTL_MCDDC_CTL_REG, errCtl.Data);
}

/**

  This routine gets the CSRs required by BiosServerGetLaneErrorStatus

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param ChunkMask                         - Mask for the chunks. One bit for each chunk.  This mask should set to be the same value as the mask being passed to the SetChunkValidationMask() function.  Products don’t have separate error status for even and odd UI should ignore this parameter.
  @param Status                            - Pointer to array where the lane error status values will be stored.  The number of array elements is the BusWidth value from the GetSystemInfo() function divided by 8.  Bit value 1 = at least one error occurred; bit value 0 = no errors occurred.

  @retval N/A

**/
VOID
BiosServerGetLaneErrorStatusChip (
  PSYSHOST              host,
  UINT8                 Socket,
  UINT8                 ChannelInSocket,
  UINT8                 ChunkMask,
  UINT8                 *Status
)
{
  UINT8                                i;
  UINT8                                even = 0;
  UINT8                                odd = 0;
  UINT8                                arrayElements = 0;
  UINT8                                Counter = 0;
  UINT8                                maxCounter = 0;
  UINT8                                EccErrorStatus = 0;
  UINT32                               cpgcErrEccS = 0;
  UINT32                               Offset;
  CPGC_ERR_DATA0_S_MCDDC_CTL_STRUCT    cpgcErrorData;
  UINT64_STRUCT                        DqLaneErStatus;

  if ((ChunkMask & 0x55) != 0) {
    even = 1;
  }

  if ((ChunkMask & 0xaa) != 0) {
    odd = 1;
  }

 // There are 4 CPGC_ERR_DATA(0-3) registers, Registers 1 and 3 hold the Upper 32bits for Even & Odd UI respectively
 // When ChunkMask has only Even bits set, take values from Reg. 0&1 only; for Odd bits set, take values from Reg. 2&3 only
 // For Even & Odd bits set, OR Reg. 0&2 and OR Reg. 1&3 resp.

  if ((even == 1) && (odd != 1)) {  //Only Even Chunks enabled in Chunk Mask
    Counter = 0;
    maxCounter = 2;
  } else if ((even != 1) && (odd == 1)) {  //Only Odd Chunks enabled in Chunk Mask
    Counter = 2;
    maxCounter = 4;
  } else if ((even == 1) && (odd == 1)) {  //Even and Odd Chunks enabled in Chunk Mask
    Counter = 0;
    maxCounter = 4;
  }

  // init the return value
  DqLaneErStatus.lo = 0x0;
  DqLaneErStatus.hi = 0x0;

  for (Counter; Counter < maxCounter; Counter++) {
    Offset = (CPGC_ERR_DATA0_S_MCDDC_CTL_REG + (Counter * 4));
    cpgcErrorData.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, Offset);

    if ((Counter == 0) || (Counter == 2)) {
      DqLaneErStatus.lo |= cpgcErrorData.Data;
    } else if ((Counter == 1) || (Counter == 3)) {
      DqLaneErStatus.hi |= cpgcErrorData.Data;
    }
  }

  //Getting the number of Array Elements
  if (host->nvram.mem.eccEn) { //BusWidth = 72(with Ecc), Array elements = 72/8
    arrayElements = 9;
    // need to read ECC status
    cpgcErrEccS = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_ECC_S_MCDDC_CTL_REG);
    if (even == 1) {
      EccErrorStatus |= (cpgcErrEccS & 0xFF);  // get bits 0:7  Even
    }
    if (odd == 1) {
      EccErrorStatus |= ((cpgcErrEccS & 0xFF00) >> 8);  // get bits 8:15 Odd
    }
  } else {
   arrayElements = 8;  //BusWidth = 64, Array elements = 64/8 = 8
  }

  //Saving DqLaneErStatus.lo/hi into the Array pointed to by the "UINT8 Status pointer"
  for (i = 0; i < arrayElements; i++) {
    if (i < 4) {
      *(Status + i) = (UINT8)((DqLaneErStatus.lo >> i*8) & 0xFF);  // Saving of the 32 bit DqLaneErStatus (lo) into the first 4 array elements pointed to by the "UINT8 Status pointer"
    } else if ((i >=4) && (i < 8)) {
      *(Status + i) = (UINT8)((DqLaneErStatus.hi >> (i-4)*8) & 0xFF);  // Saving of the 32 bit DqLaneErStatus (hi) into the last 4 array elements
    } else {
      *(Status + i) = EccErrorStatus;
    }
  }

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "BiosServerGetLaneErrorStatus Hi (DqLaneErStatus= 0x%x) Lo (DqLaneErStatus= 0x%x)\n", DqLaneErStatus.hi, DqLaneErStatus.lo));
#endif
}

/**

  This routine gets the CSRs required by BiosServerGetChunkErrorStatus

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param Status                            - Pointer to where the chunk error status will be stored.  Bit value 1 = at least one error occurred; bit value 0 = no errors occurred. Bit position 0 = chunk 0; bit position 1 = chunk 1, etc.

  @retval N/A

**/
VOID
BiosServerGetChunkErrorStatusChip (
  PSYSHOST                       host,
  UINT8                          Socket,
  UINT8                          ChannelInSocket,
  UINT8                          *Status
)
{
  CPGC_ERR_CHUNK_MCDDC_CTL_STRUCT        cpgcErrChunk;

  cpgcErrChunk.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_CHUNK_MCDDC_CTL_REG);
  *Status = (UINT8)cpgcErrChunk.Bits.status;  //Taking first 8 bits which contain the ChunkErrorStatus
}

/**

  This routine gets the CSRs required by BiosServerGetErrorAddress

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param Address                           - Pointer to buffer where error address will be stored.

  @retval N/A

**/
VOID
BiosServerGetErrorAddressChip (
  PSYSHOST                    host,
  UINT8                       Socket,
  UINT8                       ChannelInSocket,
  SSA_ADDRESS                 *Address
)
{
  CPGC_ERR_ADDR0_MCDDC_CTL_STRUCT                    errAddr0;
  CPGC_ERR_ADDR1_MCDDC_CTL_STRUCT                    errAddr1;

  errAddr0.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_ADDR0_MCDDC_CTL_REG);
  Address->Row = (UINT32) errAddr0.Bits.row;
  Address->Rank = (UINT8) errAddr0.Bits.rank;

  errAddr1.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_ADDR1_MCDDC_CTL_REG);
  Address->Col = (UINT16) errAddr1.Bits.column;
  Address->Bank = (UINT8) errAddr1.Bits.bank;
}

/**

  This routine gets the CSRs required by BiosServerGetErrorCount

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param Counter                           - Zero based counter number.  The value must be less than or equal to the MaxNumberErrorCounters value from the GetCpgcInfo() function.
  @param Count                             - Pointer to where the error counter value will be stored.
  @param Overflow                          - Pointer to where the error counter overflow status will be stored.

  @retval N/A

**/
VOID
BiosServerGetErrorCountChip (
  PSYSHOST                 host,
  UINT8                    Socket,
  UINT8                    ChannelInSocket,
  UINT8                    Counter,
  UINT32                   *Count,
  BOOLEAN                  *Overflow
)
{
  UINT32                                            Offset;
  CPGC_ERR_COUNTER0_S_MCDDC_CTL_STRUCT              errCounter;
  CPGC_ERR_COUNTER_OVERFLOW_MCDDC_CTL_STRUCT        errCounterOverflow;

  Offset = (CPGC_ERR_COUNTER0_S_MCDDC_CTL_REG + (Counter * 8));
  errCounter.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, Offset); //Counter = 0-9 for 9 registers
  *Count = errCounter.Bits.status;

  errCounterOverflow.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_COUNTER_OVERFLOW_MCDDC_CTL_REG);
  *Overflow = (errCounterOverflow.Bits.status >> Counter) & BIT0; //Overflow val. for each register, get only flag value for that counter 0 OR 1

#ifdef ENBL_BIOS_SSA_DEBUG_MSGS
  rcPrintf ((host, "BiosServerGetErrorCount (ErrCounterData= 0x%x, ErrCounterOverflow= 0x%x)\n", errCounter.Data, errCounterOverflow.Data));
#endif
}

/**

  This routine sets the CSRs required by BiosServerSetDqdbErrorCaptureConfig

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param Enable                            - If set to TRUE then cachelines for which errors are detected are written to the DqDB starting at the StartCacheline and continuing to the EndCacheline before wrapping around to the StartCacheline.  If set to FALSE then the DqDB error capture feature is disabled.
  @param StartCacheline                    - The starting cacheline in the DqDB where the error status will be captured.  The value must be less than the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.
  @param EndCacheline                      - The ending cacheline in the DqDB where the error status will be captured.  The value must be greater than or equal to the StartCacheline value.  The value must be less than the MaxNumberDqdbCachelines value from the GetCpgcInfo() function.

  @retval N/A

**/
VOID
BiosServerSetDqdbErrorCaptureConfigChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 ChannelInSocket,
  BOOLEAN                               Enable,
  UINT8                                 StartCacheline,
  UINT8                                 EndCacheline
  )
{
  CPGC_ERR_WDB_CAPTURE_CTL_MCDDC_CTL_STRUCT          errDqdbCapture;

  errDqdbCapture.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_WDB_CAPTURE_CTL_MCDDC_CTL_REG);
  errDqdbCapture.Bits.enable = (Enable)? 1 : 0;
  errDqdbCapture.Bits.start_pntr = StartCacheline;
  errDqdbCapture.Bits.end_pntr = EndCacheline;
  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_WDB_CAPTURE_CTL_MCDDC_CTL_REG, errDqdbCapture.Data);
}

/**

  This routine gets the CSRs required by BiosServerGetDqdbErrorCaptureStatus

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - Channel number (0-based)
  @param CurrentCacheline                  - Pointer to where the current DqDB error capture cacheline value will be stored.

  @retval N/A

**/
VOID
BiosServerGetDqdbErrorCaptureStatusChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 ChannelInSocket,
  UINT8                                 *CurrentCacheline
  )
{
  CPGC_ERR_WDB_CAPTURE_STATUS_MCDDC_CTL_STRUCT          cpgcErrWdbCapture;

  cpgcErrWdbCapture.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_ERR_WDB_CAPTURE_STATUS_MCDDC_CTL_REG);
  *CurrentCacheline = (UINT8) cpgcErrWdbCapture.Bits.err_pntr;  //Every cacheline that experiences an error will cause the WDB_Current_Err_Pointer to increment by 1
}

/**

  This routine sets the CSRs required by BiosServerSetLocalTestControl

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller Id (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param ClearErrors                       - Setting this parameter to TRUE causes all the channel’s error counters and status to be cleared.
  @param StartTest                         - Setting this parameter to TRUE starts the channel’s CPGC engine.
  @param StopTest                          - Setting this parameter to TRUE stops the channel’s CPGC engine.

  @retval N/A

**/
VOID
BiosServerSetLocalTestControlChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 Controller,
  UINT8                                 ChannelInSocket,
  BOOLEAN                               ClearErrors,
  BOOLEAN                               StartTest,
  BOOLEAN                               StopTest
  )
{
  CPGC_SEQCTL0_MC_MAIN_STRUCT               cpgcSeqCtl0Reg;

  cpgcSeqCtl0Reg.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_SEQCTL0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)));

  cpgcSeqCtl0Reg.Bits.local_clear_errs = (ClearErrors)? 1 : 0;
  cpgcSeqCtl0Reg.Bits.local_start_test = (StartTest)? 1 : 0;
  cpgcSeqCtl0Reg.Bits.local_stop_test  =  (StopTest)? 1 : 0;

  MemWritePciCfgMC (host, Socket, Controller, CPGC_SEQCTL0_MC_MAIN_REG + (4 * GetMCCh(Controller, ChannelInSocket)), cpgcSeqCtl0Reg.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetGlobalTestControl

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller Id (0-based)
  @param ClearErrors                       - Setting this parameter to TRUE causes all the channel’s error counters and status to be cleared.
  @param StartTest                         - Setting this parameter to TRUE starts the channel’s CPGC engine.
  @param StopTest                          - Setting this parameter to TRUE stops the channel’s CPGC engine.

  @retval N/A

**/
VOID
BiosServerSetGlobalTestControlChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 Controller,
  BOOLEAN                               ClearErrors,
  BOOLEAN                               StartTest,
  BOOLEAN                               StopTest
  )
{
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT            cpgcGlobalCtrlReg;

  cpgcGlobalCtrlReg.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_GLOBALCTRL_MC_MAIN_REG);

  cpgcGlobalCtrlReg.Bits.clear_errs = (ClearErrors)? 1 : 0;
  cpgcGlobalCtrlReg.Bits.start_test = (StartTest)? 1 : 0;
  cpgcGlobalCtrlReg.Bits.stop_test =  (StopTest)? 1 : 0;

  MemWritePciCfgMC (host, Socket, Controller, CPGC_GLOBALCTRL_MC_MAIN_REG, cpgcGlobalCtrlReg.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetAddrConfig

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller Id (0-based)
  @param ChannelInSocket                   - Channel number (0-based)
  @param EnableDdr4                        - Setting this parameter to TRUE causes the CPGC engine to be in the DDR4 test mode, otherwise it’s in the NVMDIMM test mode.

  @retval N/A

**/
VOID
BiosServerSetAddrConfigChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 Controller,
  UINT8                                 ChannelInSocket,
  BOOLEAN                               EnableDdr4
  )
{
  UINT8                                       ChannelInMC = 0;
  CPGC_GLOBALCTRL_MC_MAIN_STRUCT              cpgcGlblCntrl;
  MCMTR_MC_MAIN_STRUCT                        mcMtr;
  T_DDRT_MISC_DELAY_MC_2LM_STRUCT             ddrtMiscDelay;

  ChannelInMC = GetMCCh(Controller, ChannelInSocket);

  cpgcGlblCntrl.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_GLOBALCTRL_MC_MAIN_REG);
  mcMtr.Data = MemReadPciCfgMC (host, Socket, Controller, MCMTR_MC_MAIN_REG);
  ddrtMiscDelay.Data  = MemReadPciCfgEp (host, Socket, ChannelInSocket, T_DDRT_MISC_DELAY_MC_2LM_REG);

  if (EnableDdr4) {
    cpgcGlblCntrl.Bits.ddrt_addr_mode  &= ~(1 << ChannelInMC);
    mcMtr.Bits.trng_target  &= ~(1 << ChannelInMC);
    ddrtMiscDelay.Bits.force_ddr4_mode = 1;
    ddrtMiscDelay.Bits.force_ddrt_mode = 0;
  } else {
    cpgcGlblCntrl.Bits.ddrt_addr_mode  |= (1 << ChannelInMC);
    mcMtr.Bits.trng_target  |= (1 << ChannelInMC);
    ddrtMiscDelay.Bits.force_ddr4_mode = 0;
    ddrtMiscDelay.Bits.force_ddrt_mode = 1;
  }

  MemWritePciCfgMC (host, Socket, Controller, CPGC_GLOBALCTRL_MC_MAIN_REG, cpgcGlblCntrl.Data);
  MemWritePciCfgMC (host, Socket, Controller, MCMTR_MC_MAIN_REG, mcMtr.Data);
  MemWritePciCfgEp (host, Socket, ChannelInSocket, T_DDRT_MISC_DELAY_MC_2LM_REG, ddrtMiscDelay.Data);
}

/**

  This routine gets the CSRs required by BiosServerGetTestDone

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller Id (0-based)
  @param ChannelMask                       - Channel mask.  Each bit represents a channel to be checked.  Bit value 1 = perform check; bit value 0 = do not perform check.  Bit position 0 = channel 0; bit position 1 = channel 1, etc.
  @param TestDoneMask                      - Pointer to where mask of per-channel test done status will be placed.  Bit value 1 = test complete; bit value 0 = test not complete or check not performed.  Bit position 0 = channel 0; bit position 1 = channel 1, etc.

  @retval N/A

**/
VOID
BiosServerGetTestDoneChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 Controller,
  UINT8                                 ChannelMask,
  UINT8                                 *TestDoneMask
  )
{
  CPGC_GLOBALERR_MC_MAIN_STRUCT             cpgcGlobalErrReg;

  cpgcGlobalErrReg.Data = MemReadPciCfgMC (host, Socket, Controller, CPGC_GLOBALERR_MC_MAIN_REG);

  *TestDoneMask = 0;
  if (ChannelMask & 0x1) {                 // Checking if Channel 0 is enabled
    if (cpgcGlobalErrReg.Bits.channel_test_done_status_0  == 1) {
      *TestDoneMask |= 1 ;                 //If channel_test_done_status == 1, then test is done on that channel
    }
  }

  if ((ChannelMask >> 1) & 0x1) {          // Checking if Channel 1 is enabled
    if (cpgcGlobalErrReg.Bits.channel_test_done_status_1  == 1) {
      *TestDoneMask |= (1 << 1);
    }
  }

  if ((ChannelMask >> 2) & 0x1) {          // Checking if Channel 2 is enabled
    if (cpgcGlobalErrReg.Bits.channel_test_done_status_2  == 1) {
      *TestDoneMask |= (1 << 2) ;         //If channel_test_done_status == 1, then test is done on that channel
    }
  }

  if ((ChannelMask >> 3) & 0x1) {          // Checking if Channel 4 is enabled
    if (cpgcGlobalErrReg.Bits.channel_test_done_status_3  == 1)
      *TestDoneMask |= (1 << 3);
  }

}

/**

  This routine gets/sets the CSRs required by BiosServerSetScramblerConfig

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param Controller                        - Controller Id (0-based)
  @param Enable                            - Specifies whether the scrambler is enabled.
  @param PrevEnable                        - Pointer to where the previous scrambler enable setting will be stored.  This value may be NULL.

  @retval N/A

**/
VOID
BiosServerSetScramblerConfigChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 Controller,
  BOOLEAN                               Enable,
  BOOLEAN                               *PrevEnable
  )
{
  //This function perform Get/Set/Restore for the register bits
  UINT8                                ChannelInSocket = 0;
  UINT8                                maxChannel = 0;
  UINT8                                iteration = 0;
  struct channelNvram                  (*channelNvList)[MAX_CH];
  MCSCRAMBLECONFIG_MCDDC_DP_STRUCT     scrambleConfig;

  channelNvList = GetChannelNvList(host, Socket);

  if (Controller == 1) {
     ChannelInSocket += MAX_CH_IMC;  //ch = 2,3 for BDX and 3,4,5 for SKX
     maxChannel = MAX_CH;            //4 for BDX, 6 for SKX
   } else {
     maxChannel = MAX_CH_IMC;        //2 for BDX, 3 for SKX
   }

  for (ChannelInSocket; ChannelInSocket < maxChannel; ChannelInSocket++) {
    if ((*channelNvList)[ChannelInSocket].enabled == 0) continue;

    scrambleConfig.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, MCSCRAMBLECONFIG_MCDDC_DP_REG);

    if ((PrevEnable != NULL) && (iteration == 0)) {  //Get Case... Save Previous value for only the First Enabled Channel
      *PrevEnable = (scrambleConfig.Bits.ch_enable & 1);
    }
    //When trying to Restore *PrevEnable is passed in via "Enable".. In Set/Restore cases we do not care about *PrevEnable

    //Enable scrambling if TRUE
    //To operate on the bits only during the Set or Restore Cases
    if (Enable == TRUE) {
      scrambleConfig.Bits.rx_enable = 1;
      scrambleConfig.Bits.tx_enable = 1;
      scrambleConfig.Bits.ch_enable = 1;
    } else {
      scrambleConfig.Bits.rx_enable = 0;
      scrambleConfig.Bits.tx_enable = 0;
      scrambleConfig.Bits.ch_enable = 0;
    }

    MemWritePciCfgEp (host, Socket, ChannelInSocket, MCSCRAMBLECONFIG_MCDDC_DP_REG, scrambleConfig.Data);
    iteration = 1;
  }
}

/**

  This routine sets the CSRs required by BiosServerSetRefreshOverride

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - ChannelInSocket No. (0-based)
  @param EnablePanicRefreshOnly            - If set to TRUE then opportunistic and high priority auto refreshes will be disabled for all ranks and refreshes will only occur when the panic refresh watermark has been exceeded.  If set to FALSE then refreshes will occur using the existing MC functional refresh logic.
  @param PairCount                         - Number of entries in the DimmRankPairs array.  If you wish to have no rank’s refresh overridden then set PairCnt=0 and DimmRankPairs=NULL.  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param DimmRankPairs                     - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in the list, then the CPGC engine will disable executing any refresh to that rank while in REUT mode. If a DIMM/rank doesn’t appear in the list then the CPGC engine will enable executing a refresh (assuming the MC has refreshes enabled) to that rank while in REUT mode based on the EnablePanicRefreshOnly parameter value.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.

  @retval N/A

**/
VOID
BiosServerSetRefreshOverrideChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 ChannelInSocket,
  BOOLEAN                               EnablePanicRefreshOnly,
  UINT8                                 PairCount,
  CONST DIMM_RANK_PAIR                  *DimmRankPairs
  )
{
  UINT8                                     i;
  UINT8                                     logicalRankMapping[MAX_RANK_CH] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  CPGC_MISCREFCTL_MCDDC_CTL_STRUCT          cpgcReg;

  cpgcReg.Data  = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_MISCREFCTL_MCDDC_CTL_REG);

  if (PairCount != 0) {
    //Based on the DimmRankPair we calculate the list of LogicalRanks
    for (i = 0; i < PairCount; i++) {
      if (((DimmRankPairs + i)->Dimm < MAX_DIMM) && ((DimmRankPairs + i)->Rank < MAX_RANK_DIMM)) {
        SSAGetLogicalRank(host, Socket, ChannelInSocket, (DimmRankPairs + i)->Dimm , (DimmRankPairs + i)->Rank, &logicalRankMapping[i]);  //Fill Array logicalRankMapping with LogicalRank values
      }
    }

    for (i = 0; i < MAX_RANK_CH; i++) {  //Check all Array Elements
      if (logicalRankMapping[i] != 0xFF) {  //If a "Valid" DimmRankPair appears in the list, then the REUT engine will disable executing any refresh to that "rank" while in REUT mode
        cpgcReg.Bits.ref_rankmask |= (1 << logicalRankMapping[i]);  //8bits
        continue;
      }
    } //for-rank
  } else {
    cpgcReg.Bits.ref_rankmask = 0;
  }

  cpgcReg.Bits.panic_ref_only = (EnablePanicRefreshOnly)? 1 : 0;

  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_MISCREFCTL_MCDDC_CTL_REG, cpgcReg.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetZQCalOverride

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - ChannelInSocket No. (0-based)
  @param EnableZQCalAfterRefresh           - If set to TRUE then a ZQ operation will always take place after a refresh occurs as long as the DIMM/rank does not appear in the DimmRankPairs array.  If set to FALSE then a ZQ operation will take place at the normal programmed period after a refresh occurs as long as the DIMM/rank does not appear in the DimmRankPairs array.
  @param PairCount                         - Number of entries in the DimmRankPairs array.  If you wish to have no rank’s ZQCal overridden then set PairCnt=0 and DimmRankPairs=NULL.  The value must be less than the product of the MaxNumberDimms and MaxNumberRanks values from the GetSystemInfo() function.
  @param DimmRankPairs                     - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in the list, then the CPGC engine will disable executing any ZQCal to that rank while in REUT mode.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.

  @retval N/A

**/
VOID
BiosServerSetZQCalOverrideChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 ChannelInSocket,
  BOOLEAN                               EnableZQCalAfterRefresh,
  UINT8                                 PairCount,
  CONST DIMM_RANK_PAIR                  *DimmRankPairs
  )
{
  UINT8                                     i;
  UINT8                                     logicalRankMapping[MAX_RANK_CH] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  CPGC_MISCZQCTL_MCDDC_CTL_STRUCT           miscZqCtl;

  miscZqCtl.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_MISCZQCTL_MCDDC_CTL_REG);

  if (PairCount != 0) {
    //Based on the DimmRankPair we calculate the list of LogicalRanks
    for (i = 0; i < PairCount; i++) {
      if (((DimmRankPairs + i)->Dimm < MAX_DIMM) && ((DimmRankPairs + i)->Rank < MAX_RANK_DIMM)) {
        SSAGetLogicalRank(host, Socket, ChannelInSocket, (DimmRankPairs + i)->Dimm , (DimmRankPairs + i)->Rank, &logicalRankMapping[i]);
      }
    }

    for (i = 0; i < MAX_RANK_CH; i++) {  //Check all Array Elements
      if (logicalRankMapping[i] != 0xFF) {  //If a "Valid" DimmRankPair appears in the list, then the REUT engine will disable executing any ZQCal to that "rank" while in REUT mode
        miscZqCtl.Bits.zq_rankmask |= (1 << logicalRankMapping[i]);  //8bits
      }
    } //for-rank
  } else {
    miscZqCtl.Bits.zq_rankmask = 0;
  }

  miscZqCtl.Bits.always_do_zq = (EnableZQCalAfterRefresh)? 1 : 0;

  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_MISCZQCTL_MCDDC_CTL_REG, miscZqCtl.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetOdtOverride

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - ChannelInSocket No. (0-based)
  @param EnableMprDdrTrain                 - This parameter is set to TRUE during DDR training.
  @param ValCount                          - Number of entries in the DimmRankPairs and OverrideVals arrays.  If you wish to have no rank’s ODT overridden then set ValCnt=0, DimmRankPairs=NULL, and OverrideVals=NULL.  The value must be less than twice the MaxNumberDimms value from the GetSystemInfo() function.
  @param DimmRankPairs                     - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in this array then its ODT value will be overridden with the value from the corresponding index in the OverrideVals array.  If a DIMM/rank does not appear in this array then its ODT will not be overridden.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.
  @param OverrideVals                      - Pointer to array of override values.  Value 1 = enable ODT for that rank while in REUT mode; value 0 = disable ODT for that rank while in REUT mode.

  @retval N/A

**/
VOID
BiosServerSetOdtOverrideChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 ChannelInSocket,
  BOOLEAN                               EnableMprDdrTrain,
  UINT8                                 ValCount,
  CONST DIMM_RANK_PAIR                  *DimmRankPairs,
  CONST UINT8                           *OverrideVals
  )
{
  UINT8                                     i;
  UINT8                                     PhyRank = 0;
  UINT8                                     Dimm = 0;
  UINT8                                     ChipSelectIndex = 0;
  UINT8                                     odtBit = 0;
  struct dimmNvram                          (*dimmNvList)[MAX_DIMM];
  CPGC_MISCODTCTL_MCDDC_CTL_STRUCT          cpgcMiscODTCtl;

  dimmNvList  = GetDimmNvList(host, Socket, ChannelInSocket);

  // The ranks in *DimmRankPairs go to the “odt_override” field.
  // The value in *OverrideVals go to the “odt_on” field. The bit location should be based on the rank location in the OverrideDimmRankPairs.
  cpgcMiscODTCtl.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_MISCODTCTL_MCDDC_CTL_REG);

  if (ValCount != 0) {
    // At location 0 we save the logicalRank for Dimm/Rank pair 0, so on...
    // Based on the DimmRankPair we calculate the list of LogicalRanks
    for (i = 0; i < ValCount; i++) {
      if (((DimmRankPairs + i)->Dimm < MAX_DIMM) && ((DimmRankPairs + i)->Rank < MAX_RANK_DIMM)) {
        Dimm = (DimmRankPairs + i)->Dimm;
        PhyRank = (DimmRankPairs + i)->Rank;

        // Get ChipSelectIndex
        if ((*dimmNvList)[Dimm].lrRankMult > 1) {
          ChipSelectIndex = (Dimm << 2) | (PhyRank & 1);
        } else {
          ChipSelectIndex = (Dimm << 2) | PhyRank;
        }

        if ((Dimm == 0) && ((ChipSelectIndex == 0) || (ChipSelectIndex == 2))) {
          odtBit = 0;
        } else if ((Dimm == 0) && ((ChipSelectIndex == 1) || (ChipSelectIndex == 3))) {
          odtBit = 1;
        } else if ((Dimm == 1) && ((ChipSelectIndex == 4) || (ChipSelectIndex == 6))) {
         odtBit = 2;
        } else if ((Dimm == 1) && ((ChipSelectIndex == 5) || (ChipSelectIndex == 7))) {
         odtBit = 3;
        } else if ((Dimm == 2) && (ChipSelectIndex == 8)) {
          odtBit = 4;
        } else if ((Dimm == 2) && (ChipSelectIndex == 9)) {
          odtBit = 5;
        }

        //When odt_override is set to 1 for a rank, the value for ODT stored in "odt_on" will be used
        cpgcMiscODTCtl.Bits.odt_override |= (1 << odtBit);

        //For "odt_on" if bit set to 1, ODT will be ON for that Chip_Select else OFF
        cpgcMiscODTCtl.Bits.odt_on &= (~(1 << odtBit)) & 0x3F;
        if ((*(OverrideVals + i) & 1) == 1) { //*OverrideVal -> Array with the list of the cke_on values to be over-written
          cpgcMiscODTCtl.Bits.odt_on |= (1 << odtBit);
        }  // Dimm and Rank are valid
      }  //if-DimmRankPairs
    }  //for-i
  } else {  //if-ValCount
    cpgcMiscODTCtl.Bits.odt_override = 0;
  }

  cpgcMiscODTCtl.Bits.mpr_train_ddr_on = (EnableMprDdrTrain)? 1 : 0;

  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_MISCODTCTL_MCDDC_CTL_REG, cpgcMiscODTCtl.Data);
}

/**

  This routine sets the CSRs required by BiosServerSetCkeOverride

  @param host                              - Pointer to sysHost
  @param Socket                            - Socket Id
  @param ChannelInSocket                   - ChannelInSocket No. (0-based)
  @param OverrideVals                      - Pointer to array of override values.  Value 1 = enable ODT for that rank while in REUT mode; value 0 = disable ODT for that rank while in REUT mode.
  @param ValCount                          - Number of entries in the DimmRankPairs and OverrideVals arrays.  If you wish to have no rank’s CKE overridden then set ValCnt=0, DimmRankPairs=NULL, and OverrideVals=NULL.  The value must be less than the MaxNumberRankMapEntries value from the GetCpgcInfo() function.
  @param DimmRankPairs                     - Pointer to array of DIMM and physical rank pair values.  If a DIMM/rank appears in this array then its CKE value will be overridden with the value from the corresponding index in the OverrideVals array.  If a DIMM/rank does not appear in this array then its CKE will not be overridden.  The valid DIMM/rank pairs can be determined via the GetDimmBitMask() and GetRankInDimm() functions.
  @param OverrideVals                      - Pointer to array of override values.  Value 1 = enable CKE for that rank while in REUT mode; value 0 = disable CKE for that rank while in REUT mode.

  @retval N/A

**/
VOID
BiosServerSetCkeOverrideChip (
  PSYSHOST                              host,
  UINT8                                 Socket,
  UINT8                                 ChannelInSocket,
  UINT8                                 ValCount,
  CONST DIMM_RANK_PAIR                  *DimmRankPairs,
  CONST UINT8                           *OverrideVals
  )
{
  UINT8                                     i;
  UINT8                                     logicalRankOverrideDimmRankPairs[MAX_RANK_CH] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  CPGC_MISCCKECTL_MCDDC_CTL_STRUCT          miscCKECtl;

  miscCKECtl.Data = MemReadPciCfgEp (host, Socket, ChannelInSocket, CPGC_MISCCKECTL_MCDDC_CTL_REG);

  if (ValCount != 0) {
    //Based on the DimmRankPair we calculate the list of LogicalRanks
    for (i = 0; i < ValCount; i++) {
      if (((DimmRankPairs + i)->Dimm < MAX_DIMM) && ((DimmRankPairs + i)->Rank < MAX_RANK_DIMM)) {
        SSAGetLogicalRank(host, Socket, ChannelInSocket, (DimmRankPairs + i)->Dimm , (DimmRankPairs + i)->Rank, &logicalRankOverrideDimmRankPairs[i]);  //Fill Array logicalRankMapping with LogicalRank values
      }
    }

    for (i = 0; i < MAX_RANK_CH; i++) {  //Check all Array Elements
      if (logicalRankOverrideDimmRankPairs[i] != 0xFF) {  //If a "Valid" DimmRankPair appears in the list, then the REUT engine will override "CKE" for that rank
        //cke_override --> 8bits. If bit set to 1, CKE value will be overridden for that Rank and the value specified in "cke_on" for that "Rank" is used
        //cke_on --> 8bits. If bit set to 1, "cke_on" will be ON for that Rank else OFF
        //*OverrideVal -> Array with the list of the cke_on values to be over-written
        miscCKECtl.Bits.cke_override |= (1 << logicalRankOverrideDimmRankPairs[i]);

        miscCKECtl.Bits.cke_on &= ~(1 << logicalRankOverrideDimmRankPairs[i]);
        if ((*(OverrideVals + i) & 1) == 1) {
          miscCKECtl.Bits.cke_on |= (1 << logicalRankOverrideDimmRankPairs[i]);
        }
      }
    }  //for-rank
  } else {
    miscCKECtl.Bits.cke_override = 0;
  }

  MemWritePciCfgEp (host, Socket, ChannelInSocket, CPGC_MISCCKECTL_MCDDC_CTL_REG, miscCKECtl.Data);
}

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

// end file BiosSsaMemoryServerConfig.c
