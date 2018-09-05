/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioDfxInit.c

  Houses all code related to Initialize DFX

Revision History:

**/
#include <SysFunc.h>
#include <OemIioInit.h>
#include <IioUtilityLib.h>
#include <EvAutoRecipe.h>
#include <RcRegs.h>
#include <SysHostChip.h>
#include "IioUniphy.h"
#include "CpuPciAccess.h"

//
// The meRequestedSize == 1 means non-zero value
// 4929890: To enable ME UMA region vc1_prim_en must be disabled when MEseg mode is enabled
//
static ISOC_VC_TABLE_STRUCT IsocVc_Table_SKX[] = {
// Isoc  meRequestedSize  Vc1_pri_en Isoc_enable
  { 0,      0,               0,         0},      // 0
  { 0,      1,               0,         0},      // 1
  { 1,      0,               1,         1},      // 2
  { 1,      1,               0,         1}       // 3
  };

/**

    IioPreUniphyInitRxRecalibration: IIO Rx Recalibration programming, before UNIPHY Rx recipe settings (if any)

    @param IioGlobalData      - Pointer to IIO Universal Data Structure (UDS)
    @param IioIndex           - Index of CPU/IIO

    @retval None

**/
VOID IioPreUniphyInitRxRecalibration (
    IIO_GLOBALS                        *IioGlobalData,
    UINT8                              IioIndex
    )
{

}

/**

    IioPostUniphyInitRxRecalibration: IIO Rx Recalibration programming, after UNIPHY Rx recipe settings (if any)

    @param IioGlobalData      - Pointer to IIO Universal Data Structure (UDS)
    @param IioIndex           - Index of CPU/IIO

    @retval None

**/
VOID
IioPostUniphyInitRxRecalibration (
    IIO_GLOBALS                         *IioGlobalData,
    UINT8                               IioIndex
    )
{

}

/**

    DfxPcieInit: Contains functions for programming Rx, Tx, Isoc parameters and some WA.

    @param IioGlobalData      - Pointer to IIO Universal Data Structure (UDS)
    @param IioIndex           - Index of IIO/CPU

    @retval None

**/
VOID
DfxPcieInit (
  IN  IIO_GLOBALS                         *IioGlobalData,
  IN  UINT8                               IioIndex
  )
{
  UINT8                                 Stack;
  UINT8                                 PortIndex;
  UINT8                                 BusNumber;
  UINT8                                 ClusterPort;
  XPDFXSPAREREG_IIO_DFX_STRUCT          XpDfxSpareReg;
  XPDFXSPAREREG_IIO_DFX_STRUCT          XpDfxSpareRegDmi;
  UINT32                                Ler_uncerrmsk;
  IIOMISCCTRL_N1_IIO_VTD_STRUCT         IioMiscCtrlN1;
  LER_XPUNCERRMSK_IIO_PCIE_STRUCT       Ler_Xpuncerrmsk;
  LER_RPERRMSK_IIO_PCIE_STRUCT          Ler_Rperrmsk;
  LER_CTRLSTS_IIO_PCIE_STRUCT           Ler_Ctrlsts;
  TXEQREUTPRIV_IIO_DFX_STRUCT           TxEqReutPriv;
  CC_SPARE_ADDL_IIO_DFX_GLOBAL_STRUCT   CcSpareAddl;
  SPARE_SWWEST2_IIO_DFX_GLOBAL_STRUCT   SpareSwwest2;
  XPDFXSPAREREG2_IIO_DFX_STRUCT         XpDfxSpareReg2;

  //
  // IIO Rx Recalibration programming, before UNIPHY Rx recipe settings (if any)
  //
  IioPreUniphyInitRxRecalibration(IioGlobalData, IioIndex);
  //
  // Apply IIO RX Recipe settings here
  //
  IioRxRecipeSettingsAuto(IioGlobalData, IioIndex);
  //
  //IIO Rx Recalibration programming, after UNIPHY Rx recipe settings (if any)
  //
  IioPostUniphyInitRxRecalibration(IioGlobalData, IioIndex);
  //
  // Allow OEMs to do an override of any of the default IIO RX Recipe settings
  //
  OemOverrideIioRxRecipeSettings (IioGlobalData, IioIndex);

  for(Stack =0; Stack < MAX_IIO_STACK; Stack++) {
    if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1<<Stack))){
      continue;
    }

    if ( IioGlobalData->SetupData.StopAndScream ){
      if ((IioIndex != 0) && (Stack != IIO_CSTACK)) {
        //
        // 4166876: Configure Stop & scream if enabled
        //
        IioMiscCtrlN1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG);
        IioMiscCtrlN1.Bits.poisfen = 0x01;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG, IioMiscCtrlN1.Data);
      }
    }

    //
    // BIOS Enhancement for the UniPhy RxRst issue...
    // 4928075: IIO: CC_SPARE_ADDL[1:0] must be be set for all PCIe/DMI ports
    //
    CcSpareAddl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, CC_SPARE_ADDL_IIO_DFX_GLOBAL_REG);
    CcSpareAddl.Bits.cc_spare_addl_bits |= 3;    //set CC_SPARE_ADDL[0], CC_SPARE_ADDL[1]
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CC_SPARE_ADDL_IIO_DFX_GLOBAL_REG, CcSpareAddl.Data);

    //
    // 5371685: {TPP Beta} PCIE: Enable bug fix for Phase2 EQ
    //
    BusNumber = IioGlobalData->IioVar.IioVData.SocketStackBus[IioIndex][Stack];
    XpDfxSpareReg2.Data = IioPciExpressRead32(IioGlobalData, IioIndex, BusNumber, PCIE_PORT_GLOBAL_DFX_DEV, 0,  XPDFXSPAREREG2_IIO_DFX_REG);
    XpDfxSpareReg2.Bits. xpdfxsparereg2_csr_en_ph2_presetfix = 1;
    IioPciExpressWrite32 (IioGlobalData, IioIndex, BusNumber, PCIE_PORT_GLOBAL_DFX_DEV, 0, XPDFXSPAREREG2_IIO_DFX_REG, XpDfxSpareReg2.Data);

    //
    // 5370667: IIO/PCIe Relax Ordering override incorrectly programmed by BIOS
    //
    SpareSwwest2.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, SPARE_SWWEST2_IIO_DFX_GLOBAL_REG);
    if (IioGlobalData->SetupData.PcieRelaxedOrdering){
      SpareSwwest2.Bits.disable_ro_override = 0; // Enable the RelaxOrdering
    } else {
      SpareSwwest2.Bits.disable_ro_override = 1;
    }

    //
    // 5372533: [SKX H0 VV] CLONE SKX Sighting: <@RC.w/a>CATERR with Lock stuck in TOR while running Rocket
    //
    if ( (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) &&  (IioGlobalData->IioVar.IioVData.CpuStepping >= H0_REV_SKX)) {
      SpareSwwest2.Bits.spare_csr = BIT15;
    }
    // WorkAround
    // s5372403: Cloned From SKX Si Bug Eco: CLONE SKX Sighting: PCIE: SKX responds with UR instead of CA for non posted transactions that incur on an ACS violation
    //
    SpareSwwest2.Bits.spare_csr |= BIT16;
    // end s5372403

    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, SPARE_SWWEST2_IIO_DFX_GLOBAL_REG, SpareSwwest2.Data);
  }

  for(PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
    if (!IioGlobalData->IioVar.IioOutData.PciePortConfig[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]){
      continue;
    }

    if ((IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE) && (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX)) {

      ClusterPort = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort;

      if (PortIndex == 0) {
        // DMI in non-legacy socket
        XpDfxSpareRegDmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG_IIO_DFX_DMI_REG);
        //
        // HSD:5371804: [Workaround] CLONE SKX Sighting: EP PCIe Timout Completion Synthesis results in TOR Timeout -- From BDX HSD
        //
        XpDfxSpareRegDmi.Bits.xpdfxsparereg_dis_incorrect_addr5_2_check = 0;
        XpDfxSpareRegDmi.Bits.xpdfxsparereg_dis_incorrect_cmpl_returned = 0;
        XpDfxSpareRegDmi.Bits.xpdfxsparereg_dis_cfgwr_cmplt_check = 0;

        //
        // 5370831: IIO_PCIE: Fix request for BIOS HSD 4930200 (Disable MSGD length check should be set to match BDX handling of VDM > 16DW)
        // 4930200: Disable MSGD length check should be set to match BDX handling of VDM > 16DW
        // hsx_performance improvement: 4168476: IIO Perfboost Pkt Gen CSR Field Needs to Be Enabled By Default
        //
        XpDfxSpareRegDmi.Bits.dis_msgd_len_chk = 1;
        XpDfxSpareRegDmi.Bits.xpdfxsparereg_dis_perfboost_pktgen = 0;

        IioWriteCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG_IIO_DFX_DMI_REG, XpDfxSpareRegDmi.Data);

      } else {
        XpDfxSpareReg.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG_IIO_DFX_REG);
        //
        // HSD:5371804: [Workaround] CLONE SKX Sighting: EP PCIe Timout Completion Synthesis results in TOR Timeout -- From BDX HSD
        //
        XpDfxSpareReg.Bits.xpdfxsparereg_dis_incorrect_addr5_2_check = 0;
        XpDfxSpareReg.Bits.xpdfxsparereg_dis_incorrect_cmpl_returned = 0;
        XpDfxSpareReg.Bits.xpdfxsparereg_dis_cfgwr_cmplt_check = 0;

        //
        // 5370831: IIO_PCIE: Fix request for BIOS HSD 4930200 (Disable MSGD length check should be set to match BDX handling of VDM > 16DW)
        // 4930200: Disable MSGD length check should be set to match BDX handling of VDM > 16DW
        // hsx_performance improvement: 4168476: IIO Perfboost Pkt Gen CSR Field Needs to Be Enabled By Default
        //
        XpDfxSpareReg.Bits.dis_msgd_len_chk = 1;
        XpDfxSpareReg.Bits.xpdfxsparereg_dis_perfboost_pktgen = 0;

        IioWriteCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG_IIO_DFX_REG, XpDfxSpareReg.Data);
      }
    }

    //
    // bdx_bugeco s280626 : CLONE from broadwell_server: PUSH from haswell_server: PUSH from ivytown: CLONE from ivytown: PCIe - Gen3 header bits inserted in TX data
    // stream when RX not frame locked with in-band slave loop
    // [cloned to s4986752 ] - Permanent
    //
    if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_HSX) || (IioGlobalData->IioVar.IioVData.CpuType == CPU_BDX) ||
        (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX)) {
      TxEqReutPriv.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, TXEQREUTPRIV_IIO_DFX_REG);
      TxEqReutPriv.Bits.wait4_2tsinlpbkentryslave = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, TXEQREUTPRIV_IIO_DFX_REG, TxEqReutPriv.Data);
    }

    //
    // 4166876: Configure Stop & scream if enabled
    //
    if (( IioGlobalData->SetupData.StopAndScream ) && (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE)){
      Ler_Xpuncerrmsk.Data = 0x0ff;
      Ler_uncerrmsk = 0xffffffff;
      // HSD: 5371239, Design team requested to clear Poisoned TLP from uncorrectable error status.
      Ler_uncerrmsk &= ~BIT12;
      Ler_Rperrmsk.Data = 0x0ff;
      //
      // 5371239: {!TPP Beta} BIOS is not correctly configuring registers when stop and scream is enabled
      //
      Ler_Ctrlsts.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LER_CTRLSTS_IIO_PCIE_REG);
      Ler_Ctrlsts.Bits.ler_ss_enable = 0x01;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LER_CTRLSTS_IIO_PCIE_REG, Ler_Ctrlsts.Data);

      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LER_XPUNCERRMSK_IIO_PCIE_REG, Ler_Xpuncerrmsk.Data);

      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LER_UNCERRMSK_IIO_PCIE_REG, Ler_uncerrmsk);

      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LER_RPERRMSK_IIO_PCIE_REG, Ler_Rperrmsk.Data);

    }
  } //end of per port DFX init settings

}

/**

    IioIsocConfiguration: Configures ISOC parameters in order to enable it

    @param IioGlobalData      - Pointer to IIO Universal Data Structure (UDS)
    @param IioIndex           - Index of CPU/IIO

    @retval None

**/
VOID
IioIsocConfiguration (
  IN  IIO_GLOBALS                         *IioGlobalData,
  IN  UINT8                               IioIndex
  )
{
  UINT8                                 Stack;
  UINT8                                 j;
  IRPEGCREDITS_IIO_DFX_VTD_STRUCT       IrpEgCredits;
  CSIPISOCRES_IIO_DFX_GLOBAL_STRUCT     CsiPiSocres;
  IRP_MISC_DFX2_IIO_DFX_VTD_STRUCT      IrpMiscDfx2;
  ISOC_VC_TABLE_STRUCT                  *IsocVc_Tbl;

  //
  // The following is common for Processors
  // 3614823, 3876167: isoc_enabled bit in CSIPISOCRES needs to be set
  //                   when isoc is enabled
  //  [Non-Si, permanent programming for all steppings]
  //
  // 4167166: [BSX_B0_PO] CLONE from HSX: OVL Error: cache allocation exceeds max cache!
  //

  for(Stack =0; Stack < MAX_IIO_STACK; Stack++) {
    if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1<<Stack))){
      continue;
    }

    CsiPiSocres.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, CSIPISOCRES_IIO_DFX_GLOBAL_REG);

    IrpEgCredits.Data  = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IRPEGCREDITS_IIO_DFX_VTD_REG);

    if (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) {
      if (IioIndex == 0) {
        //
        // 4983629 Set merge_vc0_cnt unconditionally for C0 stepping.
        // 4986112: ESCALATE from hexaii_hsx_mock:C0 BIOS issues for isoc/ME
        //
        IrpMiscDfx2.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IRP_MISC_DFX2_IIO_DFX_VTD_REG);

        IsocVc_Tbl = IsocVc_Table_SKX;
        for (j = 0; j < (sizeof(IsocVc_Table_SKX)/sizeof(ISOC_VC_TABLE_STRUCT)); j++) {
          if (IioGlobalData->IioVar.IioVData.IsocEnable ==  IsocVc_Tbl[j].Isoc) {
            if ((IioGlobalData->IioVar.IioVData.meRequestedSize > 0) == ((UINT8)IsocVc_Tbl[j].meRequestedSize)) {
              CsiPiSocres.Bits.isoc_enabled = IsocVc_Tbl[j].Isoc_Enable;
              IrpMiscDfx2.Bits.enable_vc1 = IsocVc_Tbl[j].Vc1_pri_en;
              //
              // 4929890: Workstation/HEDT should be enable_vc1 bit = 1
              // 5371199: CLONE SKX Sighting: <ResetDungeon>UMA Integrity check failures with SPS FW
              if ((IioGlobalData->IioVar.IioVData.meRequestedSize != 0) && (Stack == IIO_CSTACK)) {
                IrpMiscDfx2.Bits.enable_vc1 = 0;
              }
              break;
            }
          }
        }
        IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IRP_MISC_DFX2_IIO_DFX_VTD_REG, IrpMiscDfx2.Data);
      } else {
        //
        // 4986476: ESCALATE from hexaii_hsx_mock:C0: BIOS needs to set isoc registers differently on sockets 1..N
        //
        CsiPiSocres.Bits.merge_vc0_cnt = 0x00;
        CsiPiSocres.Bits.isoc_enabled = 0;
      }

      //
      // 4986258: ESCALATE from hexaii_hsx_mock:C0 BIOS/Isoc issues with 18d15
      // 4927841(SKX): IrpEgCredits.Bits.ad_shared_cdt_threshold = 0xC;
      //
      IrpEgCredits.Bits.ad_shared_cdt_threshold = 0xc;
    }

    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CSIPISOCRES_IIO_DFX_GLOBAL_REG, CsiPiSocres.Data);
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IRPEGCREDITS_IIO_DFX_VTD_REG, IrpEgCredits.Data);
  }
}
