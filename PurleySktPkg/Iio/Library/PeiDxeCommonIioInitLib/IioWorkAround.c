/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2017 Intel Corporation. All rights
reserved This software and associated documentation (if any) is 
furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by 
such license, no part of this software or documentation may be 
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioWorkAround.c

  Houses all code related to IIO workarounds

Revision History:

**/
#include <SysFunc.h>
#include <OemIioInit.h>
#include <IioUtilityLib.h>
#include <RcRegs.h>
#include <SysHostChip.h>
#ifndef MINIBIOS_BUILD
#include <Library/IoLib.h>
#endif //MINIBIOS_BUILD
#include "IioWorkAround.h"


UINT64 CdrPpmBifurcationMap[5][4] = { {0x0000000000000081, 0x0000000000204000, 0x0000008100000000, 0x0020400000000000},
                                      {0x0000000000204081, 0x0000000000000000, 0x0000008100000000, 0x0020400000000000},
                                      {0x0000000000000081, 0x0000000000204000, 0x0020408100000000, 0x0000000000000000},
                                      {0x0000000000204081, 0x0000000000000000, 0x0020408100000000, 0x0000000000000000},
                                      {0x0020408100204081, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000} };

static UINTN BifurcationMap[5][4] = {{ 4, 4, 4, 4},
                                     { 8, 0, 4, 4},
                                     { 4, 4, 8, 0},
                                     { 8, 0, 8, 0},
                                     {16, 0, 0, 0} };

/**
=========================================================================================
===================== Subroutines should be defined starting here    ====================
=========================================================================================
**/

/**
    This routine contains permanent workarounds
 
    @param IioGlobalData - IIO global data structure
    @param IioIndex      - IIO index

    @retval None

 */
VOID
IioGlobalPermanentWorkarounds (
  IN  IIO_GLOBALS              *IioGlobalData,
  IN  UINT8                    IioIndex
  )
{
  OTCNUMMINCRDT_N0_IIO_VTD_STRUCT    OtcNumMinCrdtN0;
  OTCCTRL01_N0_IIO_VTD_STRUCT        OtcCtrl01_N0;
  OTCCTRL01_N1_IIO_VTD_STRUCT        OtcCtrl01_N1;
  OTCCTRL23_N0_IIO_VTD_STRUCT        OtcCtrl23N0;
  OTCNUMMINCRDT_N1_IIO_VTD_STRUCT    OtcNumMinCrdt;
  OTCNUMMAXCRDT_N1_IIO_VTD_STRUCT    OtcNumMaxCrdt;
  UINT8 Stack;

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    // Check for a valid stack
    if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
      continue;
    }
    //
    // 4929364: Cloned From SKX Si Bug Eco: Credit starvation for broadcast messages in IIO OTC can cause hangs
    //
    OtcNumMinCrdtN0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, OTCNUMMINCRDT_N0_IIO_VTD_REG);
    OtcNumMinCrdtN0.Bits.p_pool = 0xA;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, OTCNUMMINCRDT_N0_IIO_VTD_REG, OtcNumMinCrdtN0.Data);

    //
    // 5385166: CLONE SKX Sighting: <BIOSw/a>[HPE] IOU bandwidth drops when runing stress with x4/x8 concurrently
    //
    OtcNumMinCrdt.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, OTCNUMMINCRDT_N1_IIO_VTD_REG);
    OtcNumMinCrdt.Bits.x4_c = 0xD;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, OTCNUMMINCRDT_N1_IIO_VTD_REG, OtcNumMinCrdt.Data);

    //
    // 5370485: [SKX H0 TI] CLONE SKX Sighting: Write failures with UPI throttling on B0
    //
    OtcCtrl01_N0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, OTCCTRL01_N0_IIO_VTD_REG);
    OtcCtrl01_N0.Bits.mabort_req_during_lck = 0;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, OTCCTRL01_N0_IIO_VTD_REG, OtcCtrl01_N0.Data);

    //
    //  5372955 : [SKX H0 VV] CLONE SKX Sighting: Supercollider Arden Data Verify Failures (due to IO CTO / System Slowdown)
    //
    if  ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) &&
        (IioGlobalData->IioVar.IioVData.CpuStepping >= H0_REV_SKX)) {
    OtcCtrl01_N1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, OTCCTRL01_N1_IIO_VTD_REG);
    OtcCtrl01_N1.Bits.np_max_crdt_use = 64;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, OTCCTRL01_N1_IIO_VTD_REG, OtcCtrl01_N1.Data);
    }

  }

  if(IioGlobalData->SetupData.ConfigIOU0[IioIndex] == IIO_BIFURCATE_x4x4x4x4){
    OtcCtrl23N0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK0, OTCCTRL23_N0_IIO_VTD_REG);
    OtcCtrl23N0.Bits.dis_p2p_np_borrow = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK0, OTCCTRL23_N0_IIO_VTD_REG, OtcCtrl23N0.Data);

    OtcNumMinCrdt.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK0, OTCNUMMINCRDT_N1_IIO_VTD_REG);
    OtcNumMinCrdt.Bits.x4_np = 10;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK0, OTCNUMMINCRDT_N1_IIO_VTD_REG, OtcNumMinCrdt.Data);

    OtcNumMaxCrdt.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK0, OTCNUMMAXCRDT_N1_IIO_VTD_REG);
    OtcNumMaxCrdt.Bits.x4_np = 8;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK0, OTCNUMMAXCRDT_N1_IIO_VTD_REG, OtcNumMaxCrdt.Data);
  }

  if(IioGlobalData->SetupData.ConfigIOU1[IioIndex] == IIO_BIFURCATE_x4x4x4x4){
    OtcCtrl23N0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK1, OTCCTRL23_N0_IIO_VTD_REG);
    OtcCtrl23N0.Bits.dis_p2p_np_borrow = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK1, OTCCTRL23_N0_IIO_VTD_REG, OtcCtrl23N0.Data);

    OtcNumMinCrdt.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK1, OTCNUMMINCRDT_N1_IIO_VTD_REG);
    OtcNumMinCrdt.Bits.x4_np = 10;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK1, OTCNUMMINCRDT_N1_IIO_VTD_REG, OtcNumMinCrdt.Data);

    OtcNumMaxCrdt.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK1, OTCNUMMAXCRDT_N1_IIO_VTD_REG);
    OtcNumMaxCrdt.Bits.x4_np = 8;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK1, OTCNUMMAXCRDT_N1_IIO_VTD_REG, OtcNumMaxCrdt.Data);
  }

  if(IioGlobalData->SetupData.ConfigIOU2[IioIndex] == IIO_BIFURCATE_x4x4x4x4){
    OtcCtrl23N0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK2, OTCCTRL23_N0_IIO_VTD_REG);
    OtcCtrl23N0.Bits.dis_p2p_np_borrow = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK2, OTCCTRL23_N0_IIO_VTD_REG, OtcCtrl23N0.Data);

    OtcNumMinCrdt.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK2, OTCNUMMINCRDT_N1_IIO_VTD_REG);
    OtcNumMinCrdt.Bits.x4_np = 10;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK2, OTCNUMMINCRDT_N1_IIO_VTD_REG, OtcNumMinCrdt.Data);

    OtcNumMaxCrdt.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK2, OTCNUMMAXCRDT_N1_IIO_VTD_REG);
    OtcNumMaxCrdt.Bits.x4_np = 8;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK2, OTCNUMMAXCRDT_N1_IIO_VTD_REG, OtcNumMaxCrdt.Data);
  }
}

#ifndef MINIBIOS_BUILD
/**
  Routine name: PciePhyTestMode
  Description: This routine sets the PCIe PHY test mode

    @param IioGlobalData - IIO global data structure
    @param IioIndex      - IIO index

    @retval None

**/
VOID
PciePhyLTSSM (
    IN  IIO_GLOBALS     *IioGlobalData,
    IN  UINT8           IioIndex,
    IN  UINT8           PortIndex
    )
{
  UINT8     Bifurcation;
  UINT8     MaxFunc = 4;
  UINT8     FuncIndex = 0;
  UINT32    TmpLaneMask, ClearMask;
  UINT32    LtsSmState, FelbVal, FelbReg;
  static UINT16 LaneMask[5][4] = {{ 0x000f, 0x00f0, 0x0f00, 0xf000},    // port Bifurcated x4x4x4x4
                                  { 0x00ff,    0x0, 0x0f00, 0xf000},    // port Bifurcated x4x4x8
                                  { 0x000f, 0x00f0, 0xff00,    0x0},    // port bifurcated 8x4x4
                                  { 0x00ff,    0x0, 0xff00,    0x0},    // port bifurcated x8x8
                                  { 0xffff,    0x0,    0x0,    0x0} };  // port bifurcated x16

  Bifurcation = CheckBifurcationMapPort(IioGlobalData, IioIndex, PortIndex);
  // loop through all bifurcates ports
  for (FuncIndex = 0; FuncIndex < MaxFunc; FuncIndex++ ) {

    TmpLaneMask = (UINT32)LaneMask[Bifurcation][FuncIndex];
    if (TmpLaneMask == 0) {
      continue;
    }

    FelbReg = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, FAREND_LPBK_EN_IIO_DFX_REG);
    FelbVal = FelbReg & TmpLaneMask;

    LtsSmState = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex + FuncIndex, DBG1STAT_IIO_DFX_REG);
    LtsSmState &= 0xFF;


    if (LtsSmState == 0x15) {
      if (FelbVal != TmpLaneMask) {
        FelbReg |= TmpLaneMask;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, FAREND_LPBK_EN_IIO_DFX_REG, FelbReg);
      }
    } else {
      if (FelbVal != 0){
        ClearMask = TmpLaneMask ^ (0xFFFFFFFF);
        FelbReg &= ClearMask;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, FAREND_LPBK_EN_IIO_DFX_REG, FelbReg);
      }
    }

  } // end for
}


/**
  Routine name: PciePhyTestMode
  Description: This routine sets the PCIe PHY test mode

    @param IioGlobalData - IIO global data structure
    @param IioIndex      - IIO index

    @retval None

**/
VOID
PciePhyTestMode (
  IN  IIO_GLOBALS              *IioGlobalData
  )
{
  volatile int                      key = 0;
  UINT8                             Data;
  UINT8                             IioIndex;


  if (!IioGlobalData->SetupData.PciePhyTestMode) {
      return;
    }

  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "PCIe PHY test mode enabled (Press [ENTER] to continue) ...\n");

  while(key != 0x0d){
    // loop through all sockets
    for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++){

      if(!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex])
        continue;
      // PStack 1
      PciePhyLTSSM  (IioGlobalData, IioIndex, PORT_1A_INDEX);
      // PStack 2
      PciePhyLTSSM  (IioGlobalData, IioIndex, PORT_2A_INDEX);
      // PStack 3
      PciePhyLTSSM  (IioGlobalData, IioIndex, PORT_3A_INDEX);
    }

   IioStall (IioGlobalData, 1000); //1ms delay
   Data = IoRead8 (0x03F8 + 0x05);
   if((Data & 0x01) != 0){
   key = IoRead8 ( 0x03F8);
   }
 }
}
#endif //MINIBIOS_BUILD


/**
  Routine name: IioOtherMiscellaneousWorkarounds
  Description: This routine may hold other miscellaneous workarounds which may
  be temporary or require only for testing purpose
 
    @param IioGlobalData - IIO global data structure
    @param IioIndex      - IIO index

    @retval None

**/
VOID
IioOtherMiscellaneousWorkarounds (
  IN  IIO_GLOBALS              *IioGlobalData,
  IN  UINT8                    IioIndex
  )
{
  LTSSMDBG1_IIO_DFX_STRUCT                  ltssmdbg1;
  IRPRINGERRCTL_IIO_RAS_STRUCT              IrpRingErrCtl;
  IRPRINGERRSV_N0_IIO_RAS_STRUCT            IrpRingErrSvN0;
  TCERRCTL_IIO_RAS_STRUCT                   TcErrCtl;
  ITCERRSEV_IIO_RAS_STRUCT                  ItcErrSev;
  OTCERRSEV_IIO_RAS_STRUCT                  OtcErrSev;
  IRP_MISC_DFX0_IIO_DFX_VTD_STRUCT          IrpMiscDfx0;
  IRP_MISC_DFX1_IIO_DFX_VTD_STRUCT          IrpMiscDfx1;
  IIOERRCTL_IIO_RAS_STRUCT                  IioErrCtl;
  OTCCTRL23_N0_IIO_VTD_STRUCT               OtcCtrl23;
  ITCPIPEHAZARD_IIO_DFX_GLOBAL_STRUCT       ItcPipeHazrd;
  CSIPSUBSAD_IIO_DFX_GLOBAL_STRUCT          Csipsubsad;
  DFXSPAREREG0_IIO_DFX_STRUCT               DfxSpareReg0;
  OTCMISCARBBLOCK_IIO_DFX_GLOBAL_DMI_STRUCT OtcmiscarbblockDmi;
  OTCMISCARBBLOCK_IIO_DFX_GLOBAL_STRUCT     Otcmiscarbblock;
  UINT8   PortIndex;
  UINT8   Stack;

  //
  // Following are the ECOs which may get change later as stepping specific
  //

  //
  // 4986612 HSX Clone: PCIE: RocketBase CATERR with MCTP
  // WA for C0 Set max_cache and vc0_max_cache to 0x40. 
  // SKX discard CSIPOOLDFX0_IIO_DFX_GLOBAL_REG and CSIPOOLDFX1_IIO_DFX_GLOBAL_REG


    //
    // 4928144: IIO: Change the default value of ltssmdbg1.disablesqinl1 to 0 (BIOS verson)
    //
    for( PortIndex=0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
      // Check whether the PCIe port can be configured
      if(!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex)){
          continue;
      }

      ltssmdbg1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMDBG1_IIO_DFX_REG);
      ltssmdbg1.Bits.disablesqinl1 = 0;
      ltssmdbg1.Bits.disablesqinpkgc6 = 0;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LTSSMDBG1_IIO_DFX_REG, ltssmdbg1.Data);
      //
      // 5371317: Cloned From SKX Si Bug Eco: Cloned Bug: CLONE SKX Sighting: IIO: PCIE - Correctable errors with DLW/ASPM L1 and Laguna/PLX
      //
      if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) &&
          (IioGlobalData->IioVar.IioVData.CpuStepping >= H0_REV_SKX)) {
         if (IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort == PortIndex){
          DfxSpareReg0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, DFXSPAREREG0_IIO_DFX_REG);
          DfxSpareReg0.Data |= BIT15; // Set snapshot_deskew_ptr_skp_w_sds
          IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DFXSPAREREG0_IIO_DFX_REG, DfxSpareReg0.Data);
        }
      }
    }


  //
  // hsx_eco s256827: iio tlb forward progress mechanism is broken in presence of locks
  //  [cloned to s4032241] -  (from EP:- read tor timeout when under lock with oversubscribed tlb)
  // Removed due to SXK using HSX-C0 skew
  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    // Check for a valid stack
    if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
      continue;
    }
    //
    // 4987823: Grantley RAS code failed to enable c6/c5/c4 bits of IIOERRCTL_IIO_RAS_REG
    // for SKX c4/c5/6 bits were moed to TCERRCTRL and c4 outbound/inbound renamed in IIOERRCTL.
    //
    if (IioGlobalData->SetupData.IioErrorEn) {
      IioErrCtl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOERRCTL_IIO_RAS_REG);
      IioErrCtl.Bits.inbound_ler_ma_mca_disable = 0;
      IioErrCtl.Bits.outbound_ler_ma_mca_disable = 0;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIOERRCTL_IIO_RAS_REG, IioErrCtl.Data);

      TcErrCtl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, TCERRCTL_IIO_RAS_REG);
      TcErrCtl.Bits.itc_cabort = 1;
      TcErrCtl.Bits.itc_mabort = 1;
      TcErrCtl.Bits.itc_irp_cred_uf = 1;
      TcErrCtl.Bits.itc_irp_cred_of = 1;
      TcErrCtl.Bits.otc_cabort = 1;
      TcErrCtl.Bits.otc_mabort = 1;
      TcErrCtl.Bits.otc_ob_cred_uf = 1;
      TcErrCtl.Bits.otc_ob_cred_of = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, TCERRCTL_IIO_RAS_REG, TcErrCtl.Data);
    }

    //w/a: 5331846
    if (IioGlobalData->SetupData.IioErrorEn == 1)  {
      IrpRingErrCtl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IRPRINGERRCTL_IIO_RAS_REG);
      IrpRingErrCtl.Bits.rxdata0_parity_error = 1;
      IrpRingErrCtl.Bits.rxdata1_parity_error = 1;
      IrpRingErrCtl.Bits.bl_parity_error = 1;
      IrpRingErrCtl.Bits.blq_parity_error = 1;
      IrpRingErrCtl.Bits.snp_addr_parity_error = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IRPRINGERRCTL_IIO_RAS_REG, IrpRingErrCtl.Data);

      IrpRingErrSvN0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IRPRINGERRSV_N0_IIO_RAS_REG);
      IrpRingErrSvN0.Bits.rxdata0_parity_error = 2;
      IrpRingErrSvN0.Bits.rxdata1_parity_error = 2;
      IrpRingErrSvN0.Bits.bl_parity_error = 2;
      IrpRingErrSvN0.Bits.blq_parity_error = 2;
      IrpRingErrSvN0.Bits.snp_addr_parity_error = 2;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IRPRINGERRSV_N0_IIO_RAS_REG, IrpRingErrSvN0.Data);

      IrpMiscDfx0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IRP_MISC_DFX0_IIO_DFX_VTD_REG);
      IrpMiscDfx0.Bits.enable_parity_err_checking = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IRP_MISC_DFX0_IIO_DFX_VTD_REG, IrpMiscDfx0.Data);

      IrpMiscDfx1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IRP_MISC_DFX1_IIO_DFX_VTD_REG);
      IrpMiscDfx1.Bits.en_poison_mmio_read_cmpl_for_ca = 0;
      IrpMiscDfx1.Bits.en_poison_mmio_read_cmpl_for_ma = 0;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IRP_MISC_DFX1_IIO_DFX_VTD_REG, IrpMiscDfx1.Data);

      TcErrCtl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, TCERRCTL_IIO_RAS_REG);
      TcErrCtl.Bits.itc_hw_assert = 1;
      TcErrCtl.Bits.itc_misc_prh_overflow = 1;
      TcErrCtl.Bits.itc_enq_overflow = 1;
      TcErrCtl.Bits.itc_mtc_tgt_err = 1;
      TcErrCtl.Bits.itc_mabort = 0;
      TcErrCtl.Bits.itc_cabort = 0;
      TcErrCtl.Bits.itc_ecc_uncor_rf = 1;
      TcErrCtl.Bits.itc_ecc_cor_rf = 1;
      TcErrCtl.Bits.itc_par_addr_rf = 1;
      TcErrCtl.Bits.itc_vtmisc_hdr_rf = 1;
      TcErrCtl.Bits.itc_par_hdr_rf = 1;
      TcErrCtl.Bits.itc_par_pcie_dat = 1;
      TcErrCtl.Bits.itc_irp_cred_of = 1;
      TcErrCtl.Bits.itc_irp_cred_uf = 1;
      TcErrCtl.Bits.otc_hw_assert = 1;
      TcErrCtl.Bits.otc_irp_addr_par = 1;
      TcErrCtl.Bits.otc_irp_dat_par = 1;
      TcErrCtl.Bits.otc_par_rte = 1;
      TcErrCtl.Bits.otc_mctp_bcast_to_dmi = 1;
      TcErrCtl.Bits.otc_ecc_cor_rf = 1;
      TcErrCtl.Bits.otc_mtc_tgt_err = 1;
      TcErrCtl.Bits.otc_mabort = 0;
      TcErrCtl.Bits.otc_cabort = 0;
      TcErrCtl.Bits.otc_ecc_uncor_rf = 1;
      TcErrCtl.Bits.otc_par_addr_rf = 1;
      TcErrCtl.Bits.otc_par_hdr_rf = 1;
      TcErrCtl.Bits.otc_ob_cred_of = 1;
      TcErrCtl.Bits.otc_ob_cred_uf = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, TCERRCTL_IIO_RAS_REG, TcErrCtl.Data);

      ItcErrSev.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, ITCERRSEV_IIO_RAS_REG);
      ItcErrSev.Bits.itc_hw_assert = 2;
      ItcErrSev.Bits.itc_misc_prh_overflow = 2;
      ItcErrSev.Bits.itc_enq_overflow = 2;
      ItcErrSev.Bits.itc_mtc_tgt_err = 2;
      ItcErrSev.Bits.itc_mabort = 0;
      ItcErrSev.Bits.itc_cabort = 0;
      ItcErrSev.Bits.itc_ecc_uncor_rf = 2;
      ItcErrSev.Bits.itc_ecc_cor_rf = 0;
      ItcErrSev.Bits.itc_par_addr_rf = 2;
      ItcErrSev.Bits.itc_vtmisc_hdr_rf = 2;
      ItcErrSev.Bits.itc_par_hdr_rf = 2;
      ItcErrSev.Bits.itc_par_pcie_dat = 2;
      ItcErrSev.Bits.itc_irp_cred_of = 2;
      ItcErrSev.Bits.itc_irp_cred_uf = 2;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, ITCERRSEV_IIO_RAS_REG, ItcErrSev.Data);

      OtcErrSev.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, OTCERRSEV_IIO_RAS_REG);
      OtcErrSev.Bits.otc_hw_assert = 2;
      OtcErrSev.Bits.otc_irp_addr_par = 2;
      OtcErrSev.Bits.otc_irp_dat_par = 2;
      OtcErrSev.Bits.otc_par_rte = 2;
      OtcErrSev.Bits.otc_mctp_bcast_to_dmi = 2;
      OtcErrSev.Bits.otc_ecc_cor_rf = 0;
      OtcErrSev.Bits.otc_mtc_tgt_err = 2;
      OtcErrSev.Bits.otc_mabort = 0;
      OtcErrSev.Bits.otc_cabort = 0;
      OtcErrSev.Bits.otc_ecc_uncor_rf = 2;
      OtcErrSev.Bits.otc_par_addr_rf = 2;
      OtcErrSev.Bits.otc_par_hdr_rf = 2;
      OtcErrSev.Bits.otc_ob_cred_of = 2;
      OtcErrSev.Bits.otc_ob_cred_uf = 2;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, OTCERRSEV_IIO_RAS_REG, OtcErrSev.Data);
    }

    if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) && (IioGlobalData->IioVar.IioVData.CpuStepping < B0_REV_SKX)) {
      if(CheckVMDEnablePerStack(IioGlobalData,IioIndex,Stack)) {
        OtcCtrl23.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, OTCCTRL23_N0_IIO_VTD_REG);
        OtcCtrl23.Bits.dis_np_pool_borrow = 1;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, OTCCTRL23_N0_IIO_VTD_REG, OtcCtrl23.Data);
      }
      //
      // 5330537: Cloned From SKX Si Bug Eco: Cloned Bug: CLONE SKX Sighting: PCIE:  Inbound Posteds hang because of incorrect credit management in ITC
      //
      ItcPipeHazrd.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, ITCPIPEHAZARD_IIO_DFX_GLOBAL_REG);
      ItcPipeHazrd.Bits.enable = 1;
      ItcPipeHazrd.Bits.mode = 0xf;
      ItcPipeHazrd.Bits.thresh = 0x3;
      ItcPipeHazrd.Bits.block= 0x40;
      ItcPipeHazrd.Bits.trigger=0x40;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, ITCPIPEHAZARD_IIO_DFX_GLOBAL_REG,ItcPipeHazrd.Data );

    }
    //
    // 5331230: csipsubsad.valid programming in IIO stacks
    //
    Csipsubsad.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, CSIPSUBSAD_IIO_DFX_GLOBAL_REG);
    Csipsubsad.Bits.valid = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CSIPSUBSAD_IIO_DFX_GLOBAL_REG, Csipsubsad.Data);

    //
    // 5371539:[SKX B1 PO] Cloned From SKX Si Bug Eco: CLONE SKX Sighting: [SKX B0 PO] Supercollider split lock hanging with outstanding Ubox lock to IIO
    //
    if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) &&
            (IioGlobalData->IioVar.IioVData.CpuStepping == B1_REV_SKX)) {

          if (Stack == IIO_CSTACK) {
              OtcmiscarbblockDmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, OTCMISCARBBLOCK_IIO_DFX_GLOBAL_DMI_REG);
              OtcmiscarbblockDmi.Bits.mask = 0x200;
              OtcmiscarbblockDmi.Bits.enable = 4;
              IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, OTCMISCARBBLOCK_IIO_DFX_GLOBAL_DMI_REG, OtcmiscarbblockDmi.Data);
          } else {
              Otcmiscarbblock.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, OTCMISCARBBLOCK_IIO_DFX_GLOBAL_REG);
              Otcmiscarbblock.Bits.mask = 0x200;
              Otcmiscarbblock.Bits.enable = 4;
              IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, OTCMISCARBBLOCK_IIO_DFX_GLOBAL_REG, Otcmiscarbblock.Data);
          }
    }
    // end s5371539

  }

}

//HSD 3613071 ClearXPUncErrStsBit6
/**

    Clear error status bit6 in XPUNCERRSTS register

    @param IioGlobalData - IIO global data structure
    @param IioIndex      - IIO index

    @retval None

**/
VOID
ClearXPUncErrStsBit6(
  IN  IIO_GLOBALS          *IioGlobalData,
  IN  UINT8                IioIndex
  )
{
    UINT8   PortIndex;
    XPUNCERRSTS_IIO_PCIEDMI_STRUCT   PcieXpUncerrSts;
    XPUNCERRSTS_IIO_PCIE_STRUCT      DmiXpUncerrSts;

    for(PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
      // Check whether the PCIe port can be configured
      if (!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex)){
        continue;
      }

      if (PortIndex == PORT_0_INDEX) {
        DmiXpUncerrSts.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPUNCERRSTS_IIO_PCIEDMI_REG);
        // This bit is R/W1CS = write 1 to clear
        DmiXpUncerrSts.Bits.received_pcie_completion_with_ur_status = 1;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPUNCERRSTS_IIO_PCIEDMI_REG, DmiXpUncerrSts.Data);
      } else {
        PcieXpUncerrSts.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPUNCERRSTS_IIO_PCIE_REG);
        // This bit is R/W1CS = write 1 to clear
        PcieXpUncerrSts.Bits.received_pcie_completion_with_ur_status = 1;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPUNCERRSTS_IIO_PCIE_REG, PcieXpUncerrSts.Data);
      }
    }
    return;
}

/*
 * Routine name: IioMiscellaneousCommonWorkarounds()
 * Description: Routine contains common cross processor workarounds

   @param IioGlobalData - IIO Global data structure
   @param IioIndex      - IIO index

   @retval None
 */
VOID
IioMiscellaneousCommonWorkarounds (
  IN  IIO_GLOBALS             *IioGlobalData,
  IN  UINT8                   IioIndex
  )
{
    UINT8   BusBase;
    UINT8   StackIndex;
    IIO_ERRPINDAT_UBOX_CFG_STRUCT       iioErrPinDatReg;
    CC_TX_CTL_IIO_DFX_GLOBAL_STRUCT     CC_Tx_Ctl_Iio_Dfx;
    DFX_HVM_IIO_DFX_GLOBAL_STRUCT       Dfx_Hvm_Iio_Dfx;
    IRP_MISC_DFX0_IIO_DFX_VTD_STRUCT    IrpMiscDfx0;
    ITCCTRL23_N0_IIO_VTD_STRUCT         ItcCtrl23N0;

    BusBase = IioGlobalData->IioVar.IioVData.SocketBaseBusNumber[IioIndex];
    //
    // 4986248: CLONE from HSX: Malicious SW could negatively impact servers availability
    // due to incorrect default value of IIO_CR_ERRPINDAT_0_5_2_CFG
    //
    if (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) {
      iioErrPinDatReg.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, 0, IIO_ERRPINDAT_UBOX_CFG_REG);
      iioErrPinDatReg.Bits.pin0 = 1;
      iioErrPinDatReg.Bits.pin1 = 1;
      iioErrPinDatReg.Bits.pin2 = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, (UINT8)0, IIO_ERRPINDAT_UBOX_CFG_REG, iioErrPinDatReg.Data);
    }
 
    // The following is common for IVT, HSX
    // s3612991: Workaround for IIO is supposed to handle unimplemented functions.
    //            Chicken bit must be set for all steppings.
    for( StackIndex=0; StackIndex<MAX_IIO_STACK ; StackIndex++) {
      // Check for a valid stack 
      if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << StackIndex))){
        continue;
      }

      Dfx_Hvm_Iio_Dfx.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex, DFX_HVM_IIO_DFX_GLOBAL_REG);
      //
      // 4168945: PCIE:  L0 20-lane reduced part hides Device 1 function 0
      //
      Dfx_Hvm_Iio_Dfx.Bits.cfg_invalid_dev_mmio_chk_dis = 1;
      //
      // 5372246: [SKX H0 PO] Chicken bit to enable hot plug command complete fix
      //
     if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) &&
         (IioGlobalData->IioVar.IioVData.CpuStepping >= H0_REV_SKX)) {
        Dfx_Hvm_Iio_Dfx.Bits.spare_cfg1 = 1;
      }
      IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex, DFX_HVM_IIO_DFX_GLOBAL_REG, Dfx_Hvm_Iio_Dfx.Data);
      //
      // 4032862: HSX Clone : SC: Completion starved at head of switch queue, many MWr to loopback address with ACS bits cleared
      // 4168495: HSX Clone: SC: Completion starved at head of switch queue, many MWr to loopback address with ACS bits cleared
      //
      if (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) {
        IrpMiscDfx0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex, IRP_MISC_DFX0_IIO_DFX_VTD_REG);
        IrpMiscDfx0.Bits.p_pf_snp_rsp_holdoff_cnt = IioGlobalData->SetupData.SnoopResponseHoldOff; // Set bits 3:0:  p_pf_snp_rsp_holdoff_cnt based on setup option
        IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex, IRP_MISC_DFX0_IIO_DFX_VTD_REG, IrpMiscDfx0.Data);

        //
        // 4168851: HSX Clone: SC: PCIe Test card (Laguna) Completion TimeOut with HA_OSB_EDR_OPTIMIZATION on P2P
        //
        // SKX discard FWDPROGRESS0_IIO_DFX_GLOBAL_REG
      }

      //
      //  s3613216, 3246475: Workaround for CBDMA quiesce flow is broken (M2M and ROL)
      //  s4167150: CBDMA M2M+ROL traffic causes 3 strike (Ubox Lock FSM hang#3)
      //
      // ivt_eco s4947426: CLONE from ivytown: Copy from HSX : CBDMA M2M+ROL traffic causes 3 strike (IIO Hang)
      //  [cloned to s4032101]
      // SKX discard TSWCTL1_IIO_DFX_GLOBAL_REG, will remove this from XML at some point

      //
      // 3614137: CLONE from jaketown: PCIE: Request to change TxRxDetect from P leg only to alternating P/N Legs
      //  (4031655: also required for CRB build)
      //
      CC_Tx_Ctl_Iio_Dfx.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex, CC_TX_CTL_IIO_DFX_GLOBAL_REG);
      CC_Tx_Ctl_Iio_Dfx.Bits.tx_rxdet_ctl = 2;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex, CC_TX_CTL_IIO_DFX_GLOBAL_REG, CC_Tx_Ctl_Iio_Dfx.Data);

      //
      // 4167723: HSX Clone: SC: Rand_Stress13b - IRP hang with Rd/Wr conflicts from multiple devices
      // undo the change for BIOS HSD 4166930
      //
      // SKX discard TSWCTL1_IIO_DFX_GLOBAL_REG, will remove this from XML at some point
      // SKX discard TSWCTL0_IIO_DFX_GLOBAL_REG, will remove this from XML at some point

      //
      //4167204 :HSX Clone: Rckt: read tor timeout when under lock with oversubscribed tlb
      // B0 stepping below

      if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) && (IioGlobalData->IioVar.IioVData.CpuStepping < B0_REV_SKX)){
        //
        // 4928638: Cloned From SKX Si Bug Eco: Serialization P2P credit miscalculation in ITC
        //
        ItcCtrl23N0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex, ITCCTRL23_N0_IIO_VTD_REG);
        ItcCtrl23N0.Bits.en_gbl_serialize_irp_p2p_p = 0;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex, ITCCTRL23_N0_IIO_VTD_REG, ItcCtrl23N0.Data);
      }

    } // end of for(StackIndex=0;...
}

/**

 * Routine name: IioEarlyWABeforeBif()
 * Description: Routine contains early workarounds before rootport bifurcation

    @param IioGlobalData - Pointer to IioGlobalData structure.
    @param IioIndex      - IIO index

    @retval None

**/
VOID
IioWABeforeBif (
  IN  IIO_GLOBALS             *IioGlobalData,
  IN  UINT8                   IioIndex
  )
{
  UINT8                           PortIndex;

  UNIPHYPMCTL_IIO_DFX_STRUCT      UniPhyPmCtl;
  PXPPMCTRL_IIO_DFX_STRUCT        PxpPmCtrl;
  CLSPHYCTL6_IIO_DFX_STRUCT       sClsPhyCtrl6;
  LER_CTRLSTS_IIO_PCIEDMI_STRUCT  LerCtrlSts;
  CLSPHYCTL8_IIO_DFX_STRUCT       sClsPhyCtrl8;
  XPUNCERRSTS_IIO_PCIEDMI_STRUCT  XpUncErrSts;
  CLSPHYCTL1_IIO_DFX_STRUCT       ClsPhyCtrl1;
  SPARE_IIO_DFX_STRUCT            Spare_Iio_Dfx;
  CAPPTR_IIO_PCIE_STRUCT          Capptr_Iio_Pcie;
  PRTPHYCTL_IIO_DFX_STRUCT        Prtphyctl_Iio_Dfx;
  DFXCHICKENBIT1_IIO_DFX_STRUCT   DfxChickenBit1_Iio_Dfx;
  DBGCLST0_IIO_DFX_STRUCT         DbgClst0_Iio_Dfx;
  DFXCHICKENBIT0_IIO_DFX_STRUCT   DfxChickenBit0;
  RX_RESET_IIO_DFX_STRUCT         Rx_Reset;
  LER_CTRLSTS_IIO_PCIE_STRUCT     Ler_Ctrlsts;
  XPDFXSPAREREG2_IIO_DFX_STRUCT   XpdFxSparereg2;
  DFXUPCFGEXITEI_IIO_DFX_STRUCT   DfxUpcFgeXitei;
  //
  // apply IIO global permanent workarounds
  //
  IioGlobalPermanentWorkarounds(IioGlobalData, IioIndex);
  //
  // Other miscellaneous ECOs
  //
  IioOtherMiscellaneousWorkarounds(IioGlobalData, IioIndex);

  //
  //Apply miscellaneous workarounds which are common among cross-processor generations
  //
  IioMiscellaneousCommonWorkarounds(IioGlobalData, IioIndex);

  // EXPBERR1_IIO_DFX_STRUCT and EXPBERR3_IIO_DFX_STRUCT not existed on SKX, code removed

  //
  //Enable Non-Posted Prefetch for CBDMA traffic
  //
  //
  // s4031444: Enable Non-Posted Prefetch for CBDMA traffic in EX
  //              Reset Bit 2
  //
  // SKX discard TSWCTL1_IIO_DFX_GLOBAL_REG, will remove this from XML at some point

  //
  // Following are the Workarounds/ECOs which gets applied for each PCIe port
  //
  for(PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {

    if (!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex)){
      continue;
    }

    if (IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort == PortIndex){

      //
      //ivt_eco: s4540515: PCIe Link equalization for Gen3 is mixed with PCIe
      //                   automatic BW change notification.
      //  [cloned to s4031085] - disable logging PCIE Link Equalization Request
      //                         from being logged in XPCOERRSTS, on all Gen3 ports
      if(PortIndex != NONE_GEN3_PCIE_PORT){
        DfxChickenBit1_Iio_Dfx.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, DFXCHICKENBIT1_IIO_DFX_REG);
        DfxChickenBit1_Iio_Dfx.Bits.en_txn_eq_bw_only = 1;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DFXCHICKENBIT1_IIO_DFX_REG, DfxChickenBit1_Iio_Dfx.Data);
      }

      //
      //4168289:PCIE: HSX reported receiver (framing) errors in DLW tests.
      //
      sClsPhyCtrl8.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, CLSPHYCTL8_IIO_DFX_REG);
      sClsPhyCtrl8.Bits.en_ltidlestate2rxl0s = 0;
      // 3614327: CLONE from jaketown: PCIE LTSSM gets stuck in the CONFIG.LANENUM_* state
      sClsPhyCtrl8.Bits.lnknumtightcheck = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, CLSPHYCTL8_IIO_DFX_REG, sClsPhyCtrl8.Data);

      // 3614123: CLONE from jaketown: Spurious receiver error logged when BFM aggresively goes into L0s
      DbgClst0_Iio_Dfx.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, DBGCLST0_IIO_DFX_REG);
      DbgClst0_Iio_Dfx.Bits.extend_eiosseen_cnt_ing3 = 3;
      DbgClst0_Iio_Dfx.Bits.extend_eiosseen_cnt_ing12 =3;
      // 3614824: Data errors with Gen2 x16 nVidia cards (Once infered electrical idle in rec.cfg state it times out to detect in some cases Legaccy bug)
      DbgClst0_Iio_Dfx.Bits.dis_infereifix4reccfgstate = 0;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DBGCLST0_IIO_DFX_REG, DbgClst0_Iio_Dfx.Data);

      // 3615404: PCIE: Endpoint Receiver Errors in SV testing
      // 3615505: PCIE: Staggering interval needs to be set to 4
      // 3615781: PCIE: Test BIOS to remove disable-rxreset-on-detect work-around (enable rxreset on detect)
      //
      UniPhyPmCtl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNIPHYPMCTL_IIO_DFX_REG);
      UniPhyPmCtl.Bits.dis_rxrst4inferredei = 0;
      UniPhyPmCtl.Bits.dis_txl0sentrystagger = 0;


      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNIPHYPMCTL_IIO_DFX_REG, UniPhyPmCtl.Data);

      DfxChickenBit0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, DFXCHICKENBIT0_IIO_DFX_REG);
      DfxChickenBit0.Bits.en_l0s_tx_txval_cntr = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DFXCHICKENBIT0_IIO_DFX_REG, DfxChickenBit0.Data);

      if ((IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE) && ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX))){
        //4167573: HSX Clone: PCIe - Rx termination not disabled during link_disable state
        DfxChickenBit0.Bits.dis_send_ts1_pol_state = 0;
        //
        // 5385473: CLONE SKX Sighting: [Dell] DPN-MRT0D Mellanox 25Gb NIC failed PCIe LTSSM LED TEst w/ Riser 2A/Slot 4, Riser3A/Slot8
        //
        DfxUpcFgeXitei.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, DFXUPCFGEXITEI_IIO_DFX_REG);
        DfxUpcFgeXitei.Bits.pollact_exit_ei_timeout = 4;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DFXUPCFGEXITEI_IIO_DFX_REG, DfxUpcFgeXitei.Data);
      }

      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DFXCHICKENBIT0_IIO_DFX_REG, DfxChickenBit0.Data);

      // 3615143: PCIE: GEN3: issue reported by Broadcom appears to lock up data link layer
      sClsPhyCtrl6.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, CLSPHYCTL6_IIO_DFX_REG);
      sClsPhyCtrl6.Bits.dskindexmsk = 0x1FFFF;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, CLSPHYCTL6_IIO_DFX_REG, sClsPhyCtrl6.Data);


      // The following is approved for IVT as well
      // Update timer for data rate dependent calibration for ports that support Gen3 speeds
      // Note: As per the BSU 0.8.15: 3875429, 3613529, 3614291, 3246634: Data-Sampler VOC Values are
      // not Accurate
      //
      //
      // for IOUx
      //
      if( PortIndex != PORT_0_INDEX) {    // exclude PORT_0 per sighting
        ClsPhyCtrl1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, CLSPHYCTL1_IIO_DFX_REG);
        ClsPhyCtrl1.Bits.speedchangetime_constin4us = 0x28;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, CLSPHYCTL1_IIO_DFX_REG, ClsPhyCtrl1.Data);

        //
        //  5372154: [SKX H0 PO] PCIE: Enable bug fix for NAK sent on L1 entry
        //
        XpdFxSparereg2.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPDFXSPAREREG2_IIO_DFX_REG);
        XpdFxSparereg2.Bits.xpdfxsparereg2_bdx_eios_l1_fix = 1;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPDFXSPAREREG2_IIO_DFX_REG, XpdFxSparereg2.Data);
      }

    } // end of if supercluster
    //
    // 4031204: Legacy Pcie Txn layer bug with enabling csr_disable_ok2startnxttlp performance feature
    //
    // 4985746:  CLONE from HSX: OVL ERROR: OB TL sent data, but insufficient data, !! This sighting exclude PORT_1A, SKX should not have this limitation
    Spare_Iio_Dfx.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, SPARE_IIO_DFX_REG);
    Spare_Iio_Dfx.Bits.cgtt_sparebits = 0x2; //cgtt_sparebits[1] = 1, rest of the bits in the vector should be 0
    // 5370248: [SKX B0 PO] BIOS needs to set all pxp_<bdf>_spare.enable_gc2controll1 bits when PCIE ASPM L1 is enabled
    // 5385181: CLONE SKX Sighting: <@RC> TOR TO w/ PC6 and External Network Card <Dynamic L1>
    Spare_Iio_Dfx.Bits.enable_gc2controll1 = 0;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, SPARE_IIO_DFX_REG, Spare_Iio_Dfx.Data);
    
    // 3613833 PXT CCB: Remove PCIe L0s from the POR
    PxpPmCtrl.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, PXPPMCTRL_IIO_DFX_REG);
    PxpPmCtrl.Bits.use_cr_l0s_idle_timer = 0;
    PxpPmCtrl.Bits.l0s_idle_timer = 0;
    IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, PXPPMCTRL_IIO_DFX_REG, PxpPmCtrl.Data);

    //3614964, 3876204: PCIe: LER enable bit is sticky
    LerCtrlSts.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LER_CTRLSTS_IIO_PCIE_REG);
    LerCtrlSts.Bits.ler_ss_status = 0x01;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LER_CTRLSTS_IIO_PCIE_REG, LerCtrlSts.Data);

    XpUncErrSts.Data = 0;
    XpUncErrSts.Bits.outbound_poisoned_data = 0x01;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPUNCERRSTS_IIO_PCIE_REG, XpUncErrSts.Data);
    //
    // HSD 4030421: IVT PCIE Link Layer not returning posted data credits for MSI transactions
    //  Resolution: Remove 2nd part of W/A for s4030322
    //  Removed setting of XPDFXSPAREREG.disable_msi_gt_1dw to 1

    //
    // ivt_eco: b3247660: CLONE from jaketown: Bad Replay Packets during
    //                        PCIe NAK Test in Gen3 X16 config with 256B MPS
    //  [cloned to s4030491] - common for IVT EP,EX,
    //
    //  4167964: PCIe HSX UniPhy Recipe Updated to v5.00 (PCIe changes only)
    //  4985815: UniPhy Recipe Not Being Set On Port 0 Of Non-Legacy Sockets
    //
    //  Skip DMI but do it for Port 0 in Non-Legacy Sockets.
    // SKX discard PXPRETRYCTRL3_IIO_DFX_REG
    
    //
    // 4032294 W/A for HSX B0,
    // iio_cr_xpdfxsparereg_0_(6_0|6_3|7_0)_cfg::disable_msi_gt_1dw = 1
    // HSX B0 stepping code is removed from SKX

    //
    // HSD 4166558 (HSX): HSX Clone: PCIE: Pcode is setting the ltssmdbg0.forcereverse bit in IOU0 (Dev6 / Fun3)
    // HSX Ax stepping code removed from SKX

    //
    // ivt_eco: 4677272: PCIE: In Gen1 X1 L1 test IVT sometimes drops EIOS
    //  [cloned to 4031304] - permanent WA for IVT
    Prtphyctl_Iio_Dfx.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PRTPHYCTL_IIO_DFX_REG);
    Prtphyctl_Iio_Dfx.Bits.kalign_mode = 0x2;//kalign_mode = 2 for "Dynamic up to L0"
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PRTPHYCTL_IIO_DFX_REG, Prtphyctl_Iio_Dfx.Data);

    //
    // as per the s4031981, the WA for PM L2/3 failures as per the IVT s4677468
    // has to be removed..
    //

    //
    // 4985836: PCIe NTB: NTB settings set in DEVCAP and CAPPTR even in PCIe mode
    //

    if(IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == REGULAR_PCIE_OWNERSHIP) {
       Capptr_Iio_Pcie.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, CAPPTR_IIO_PCIE_REG);
       Capptr_Iio_Pcie.Bits.capability_pointer = 0x40;	// set 0x40 as CAPPTR for each NTBPort if RP configuration is selected. Otherwise keep 0x60 for NTB configuration
       IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, CAPPTR_IIO_PCIE_REG, Capptr_Iio_Pcie.Data);
    }

    //
    //  5370983: CLONE SKX Sighting: PCIe: TCRH cal enable causing dual polling pass after warm reset
    //
    if( (PortIndex != PORT_0_INDEX) && (PortIndex < PORT_4A_INDEX)) {
      Rx_Reset.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, RX_RESET_IIO_DFX_REG);
      Rx_Reset.Data = 0x8000ffff;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, RX_RESET_IIO_DFX_REG,Rx_Reset.Data);
      // wait for 1us before reore the lines again
      IioStall(IioGlobalData, RX_RESET_DELAY);

      Rx_Reset.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, RX_RESET_IIO_DFX_REG);
      // HSD - 5371772: PCIE: Fix mistake on TCRH w/a
      Rx_Reset.Data = 0x0;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, RX_RESET_IIO_DFX_REG,Rx_Reset.Data);
    }

    //
    //  5370983: CLONE SKX Sighting: PCIe: TCRH cal enable causing dual polling pass after warm reset
    //

    if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) &&
        (IsIioPciePortsPopulated(IioGlobalData, IioIndex) == TRUE) &&
        (IioGlobalData->SetupData.LerEn == 1)) {
      Ler_Ctrlsts.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LER_CTRLSTS_IIO_PCIE_REG);
      Ler_Ctrlsts.Bits.ler_ss_enable = 0x0;
      // To exit LER mode, the status bit must be cleared by software.
      Ler_Ctrlsts.Bits.ler_ss_status = 0x0;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LER_CTRLSTS_IIO_PCIE_REG, Ler_Ctrlsts.Data);
    }


  } //end of for port

  return;
}

/**

  This routine is to address the post RWO settings of the IIOMISCCTRL global register

    @param IioGlobalData  - Pointer to IIO_GLOBALS
    @param IioIndex       - Index to CPU/IIO

    @retval None

++*/
/**

  This routine is to address the post RWO settings of the IIOMISCCTRL global register

    @param IioGlobalData  - Pointer to IIO_GLOBALS
    @param IioIndex       - Index to CPU/IIO

    @retval None

++*/
VOID
IioMiscCtrlWorkAround(
  IN  IIO_GLOBALS                 *IioGlobalData,
  IN  UINT8                       IioIndex
  )
{
  UINT8                   j;
  IIOMISCCTRL_N0_IIO_VTD_STRUCT       IioMiscCtrlN0;
  IOBAS_IIO_PCIE_STRUCT               IoBas;
  IIOMISCCTRL_N1_IIO_VTD_STRUCT       IioMiscCtrlN1;
  UINT8                   MaxPortNumber;
  UINT8                   Stack;
  UINT8                   PortIndex;

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
      if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
          continue;
      }
      IioMiscCtrlN0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N0_IIO_VTD_REG);

      // jkt_eco # 3615392: Relaxed Order disable bits in iiomiscctrl need to be set to 1
      //
      // ivt_eco: s4538144: PCIE Ordering: Strongly ordered posted issued prior to
      //                    the completion of previous relaxed ordered posted in some cases
      //  [cloned to s4029926]
      IioMiscCtrlN0.Bits.pipeline_ns_writes_on_csi = 0;
      IioMiscCtrlN0.Bits.disable_inbound_ro_for_vc0 = 0;
      IioMiscCtrlN0.Bits.disable_ro_on_writes_from_cb_dma = 0;
      IioMiscCtrlN0.Bits.dmi_vc1_write_ordering = 3;

      //
      //s3615786 CLONE from jaketown: ESCALATE from hexa_jaketown_c0:CATERR with a stuck Lock when azalia is using VCp channel
      //
      IioMiscCtrlN0.Bits.azalia_on_vcp = 0;

      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N0_IIO_VTD_REG, IioMiscCtrlN0.Data);

      //
      // 4929480: Enable for IIO READs to bypass IRP/Write Cache
      //
      IioMiscCtrlN1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG);
      IioMiscCtrlN1.Bits.enable_read_bypass_irp = 1;
      IioWriteCpuCsr32(IioGlobalData,IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG, IioMiscCtrlN1.Data);

      MaxPortNumber = GetMaxPortPerStack(Stack);
      PortIndex = 0;
      for(j = 0; j < MaxPortNumber ; j++) {  
        
        PCIESTACKPORTINDEX(j, Stack, PortIndex);
        if( IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex) == FALSE){  //check whether the PCIe port can be configured
          continue;
        }
        //
        //Invalid 'I/O addressing Capability' implemented in II-O chipset (3614969, 3876211, 3247349)
        //
        if(IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == TRUE){  //Skip DMI port
          continue;
        }
        //setting EN1K bit
        IioMiscCtrlN0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N0_IIO_VTD_REG);
        IioMiscCtrlN0.Bits.en1k = 1;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N0_IIO_VTD_REG,IioMiscCtrlN0.Data);

        IoBas.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, IOBAS_IIO_PCIE_REG);
        IoBas.Bits.more_i_o_base_address = 0;
        IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, IOBAS_IIO_PCIE_REG, IoBas.Data);
        //clear EN1K bit
        IioMiscCtrlN0.Bits.en1k = 0;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N0_IIO_VTD_REG, IioMiscCtrlN0.Data);

        if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) && (IioGlobalData->IioVar.IioVData.CpuStepping < B0_REV_SKX)){
          //
          // 4928999: Cloned From SKX Si Bug Eco: ReqId check for GPE messages incorrect for VMD mode
          //
          if (IioGlobalData->SetupData.VMDEnabled[(IioIndex*VMD_STACK_PER_SOCKET)+Stack]) {
            IioMiscCtrlN1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG);
            IioMiscCtrlN1.Bits.disable_reqid_filter = 1;
            IioWriteCpuCsr32(IioGlobalData,IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG, IioMiscCtrlN1.Data);
          }
        }
     } // End of for(j = 0; j < MaxPortNumber ; j++)
  }// End of for (Stack = 0; Stack < MAX_IIO_STACK; Stack++)
}

/**

    This function will disable unpopulated PCIe Root Port for P-STACK0. This is a WA
    to avoid P-STACK0 Pcie unpopulated ports enter a compliance mode that causes multiple
    failure signatures when crossed with PKGC6 (s5370839)

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
IioDisableUnpopulatePciePorts(
  IN IIO_GLOBALS      *IioGlobalData,
  IN UINT8            IioIndex
  )
{
  REUTPHTDC_IIO_DFX_STRUCT Reutphtdc;
  UINT32 DisableMask;

  DisableMask = 0;
  Reutphtdc.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK0, REUTPHTDC_IIO_DFX_REG);
  switch(IioGlobalData->SetupData.ConfigIOU0[IioIndex]){
    case IIO_BIFURCATE_xxxxxx16:
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1A_INDEX) == FALSE){
        DisableMask = DISABLE_ALL_PORTS;
      }
      break;
    case IIO_BIFURCATE_xxx8xxx8:
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1A_INDEX) == FALSE){
        DisableMask =  DISABLE_A_PORT | DISABLE_B_PORT;
      }
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1C_INDEX) == FALSE){
        DisableMask =  DisableMask | DISABLE_C_PORT | DISABLE_D_PORT;
      }
     break;
    case IIO_BIFURCATE_xxx8x4x4:
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1A_INDEX) == FALSE){
        DisableMask = DISABLE_A_PORT;
      }
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1B_INDEX) == FALSE){
        DisableMask = DisableMask | DISABLE_B_PORT;
      }
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1C_INDEX) == FALSE){
        DisableMask = DisableMask | DISABLE_C_PORT | DISABLE_D_PORT;
      }
     break;
    case IIO_BIFURCATE_x4x4xxx8:
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1A_INDEX) == FALSE){
        DisableMask = DISABLE_A_PORT | DISABLE_B_PORT;
      }
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1C_INDEX) == FALSE){
        DisableMask =  DisableMask | DISABLE_C_PORT;
      }
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1D_INDEX) == FALSE){
        DisableMask = DisableMask | DISABLE_D_PORT;
      }
      break;
    case IIO_BIFURCATE_x4x4x4x4:
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1A_INDEX) == FALSE){
        DisableMask = DISABLE_A_PORT;
      }
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1B_INDEX) == FALSE){
        DisableMask = DisableMask | DISABLE_B_PORT;
      }
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1C_INDEX) == FALSE){
        DisableMask = DisableMask | DISABLE_C_PORT;
      }
      if (CheckPciePortEnable(IioGlobalData, IioIndex, PORT_1D_INDEX) == FALSE){
        DisableMask = DisableMask | DISABLE_D_PORT;
      }
     break;
    default:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "ERROR: Bifurcation value not found %x\n", IioGlobalData->SetupData.ConfigIOU0[IioIndex]);
      break;
  }
  Reutphtdc.Bits.txdatalanedis = DisableMask;
  IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_PSTACK0, REUTPHTDC_IIO_DFX_REG, Reutphtdc.Data);
}

/**

  This routine is to any IIO RX recipie workarounds which would be done post IIO init

    @param IioGlobalData  - Pointer to IIO_GLOBALS
    @param IioIndex       - Index to CPU/IIO

    @retval None

++*/
VOID
IioRxRecipeWorkAround (
    IIO_GLOBALS                     *IioGlobalData,
    UINT8                           IioIndex
  )
{
  LNKCON_IIO_PCIEDMI_STRUCT        LnkConDmi;
  LNKCON_IIO_PCIE_STRUCT           LnkCon;
  UINT8 PortIndex;
  UINT8 ClusterPort;
  UINT8 Bifurcation;
  UINTN CtrEnd;
  UINT8 Ctr;

  for(PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {

    if(!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex)){  //check whether the PCIe port can be configured
      continue;
    }

    if (IioGlobalData->SetupData.SRIS[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == TRUE)
      continue;

    ClusterPort = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort;
   
    if (IioGlobalData->SetupData.PcieCommonClock[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == TRUE)
      continue;

    if (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == TRUE){
        LnkConDmi.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG);
        LnkConDmi.Bits.common_clock_configuration = 0;
        IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG, LnkConDmi.Data);
   } else {

        Bifurcation = CheckBifurcationMapPort(IioGlobalData, IioIndex, PortIndex);
        if ((PortIndex - ClusterPort) < 0 || (PortIndex - ClusterPort) > 3){
          IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "\nERROR!!! IioRxRecipeWorkAround() - BifurcationMap[Bifurcation] out of bound.\n");
          return;
        }
        CtrEnd = (BifurcationMap[Bifurcation][PortIndex - ClusterPort] / 4);

        for (Ctr = 0; Ctr < CtrEnd; Ctr++) {
          LnkCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex + Ctr, LNKCON_IIO_PCIE_REG);
          LnkCon.Bits.common_clock_configuration = 0;
          IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex + Ctr, LNKCON_IIO_PCIE_REG, LnkCon.Data);
        }
    }
  }
}

/**

  This routine is to any IIO LER workarounds which would be done post IIO init

    @param IioGlobalData  - Pointer to IIO_GLOBALS
    @param IioIndex       - Index to CPU/IIO

    @retval None

++*/
VOID
IioLerWorkaround(
  IN IIO_GLOBALS      *IioGlobalData,
  IN UINT8            IioIndex
    )
{
  UINT8 PortIndex;
  UINT8 ClusterPort;
  XPDFXSPAREREG1_IIO_DFX_STRUCT XpDfxSpareReg1;
  XPDFXSPAREREG_IIO_DFX_STRUCT XpDfxSpareReg;
  XPDFXSPAREREG_IIO_DFX_STRUCT XpDfxSpareRegDmi;

  if (IioGlobalData->SetupData.LerEn == 0)
    return;
  
  for (PortIndex = 0 ; PortIndex < PORT_3D_INDEX; PortIndex++) {

    if (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == TRUE)
      continue;

    ClusterPort = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort;
    if (PortIndex != ClusterPort)
      continue;

    //
    // HSD:5371253 :Clear xpdfxsparereg1[dis_msk_uncerrsts_cto_in_ler] to keep synthetic CTO from getting logged during LER
    //
    XpDfxSpareReg1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG1_IIO_DFX_REG);
    XpDfxSpareReg1.Bits.dis_msk_uncerrsts_cto_in_ler = 0;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG1_IIO_DFX_REG, XpDfxSpareReg1.Data);
    // end s5371253

    //
    // HSD:5371452 :Cloned from Brickland/IVT - Poisoned Memory Write Packet That Triggers LER Is Not Dropped
    // HSD:5371457 :BIOS to Enable LER containment of subsequent packets from the LER triggering packet.
   
    XpDfxSpareReg.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG_IIO_DFX_REG);
    XpDfxSpareReg.Bits.en_ib_poison_ma = 1;
    XpDfxSpareReg.Bits.ler_ob_contain = 1;
    //
    // 5371757: BIOS to set xpdfxsparereg[drop_poison_cmpl] when enabling LER
    //
    XpDfxSpareReg.Bits.xpdfxsparereg_drop_poison_cmpl = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG_IIO_DFX_REG, XpDfxSpareReg.Data);

    // HSD:5371329 :Cloned from Brickland - Need to enable LER x16 specific data containment related fixes from IVT B0 step onwards
    if(PortIndex == 0){
      XpDfxSpareRegDmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG_IIO_DFX_DMI_REG);
      XpDfxSpareRegDmi.Bits.xpdfxsparereg_en_convert_invcmpl_urcmpl = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG_IIO_DFX_DMI_REG, XpDfxSpareRegDmi.Data);
    } else {
      XpDfxSpareReg.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG_IIO_DFX_REG);
      XpDfxSpareReg.Bits.xpdfxsparereg_en_x16_ler_check = 1;
      XpDfxSpareReg.Bits.xpdfxsparereg_en_x16_ler_dump = 1;
      XpDfxSpareReg.Bits.xpdfxsparereg_en_convert_invcmpl_urcmpl = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, ClusterPort, XPDFXSPAREREG_IIO_DFX_REG, XpDfxSpareReg.Data);
    }
  }
}

/**

  This routine is to any IIO workarounds which would be done post IIO init

    @param IioGlobalData  - Pointer to IIO_GLOBALS
    @param IioIndex       - Index to CPU/IIO

    @retval None

++*/
VOID
IioPostInitWorkaround(
  IN  IIO_GLOBALS                                         *IioGlobalData,
  IN  UINT8                                               IioIndex
  )
{

  //
  // 5370839: [SKX H0 TI] CLONE SKX Sighting: IIO0 LTSSM enters POLLING/L0_EXT.COMP_G1/POL_COMPLIANCE causing PKGC faliures
  //
  if (IioGlobalData->IioVar.IioVData.CpuStepping < H0_REV_SKX) {
    IioDisableUnpopulatePciePorts(IioGlobalData, IioIndex);
  }
}
/**
=========================================================================================
============ Subroutine Definition ends here, Main control routine starts here ==========
=========================================================================================
**/
/**

 * Routine name: IioWorkAround()
 * Description: Routine contains switching control to early workaround routines

    @param IioGlobalData          - Pointer to IioGlobalData structure.
    @param IioIndex               - IIO index
    @param IIO_INIT_ENUMERATION   - Calling Phase used as switch control

    @retval None

**/
VOID
IioWorkAround (
  IN  IIO_GLOBALS                                     *IioGlobalData,
  IN  UINT8                                           IioIndex,
  IN  IIO_INIT_ENUMERATION                            Phase
  )
{

  switch(Phase) {
  case IioBeforeBifurcation:
    ClearXPUncErrStsBit6(IioGlobalData, IioIndex);
    IioWABeforeBif(IioGlobalData, IioIndex);
    break;

  case IioPortEnumProgramMISCCTRL:
    IioMiscCtrlWorkAround(IioGlobalData, IioIndex);
   break;
  case IioPostInitEnd:
    IioPostInitWorkaround(IioGlobalData, IioIndex);
    break;
  default:
    break;

  }
  // 600015278: Routine "OemIioEarlyWorkAround" is missing in Purley BIOS
  // OEM hook to override the DFX WA settings
  //
  OemIioEarlyWorkAround(IioGlobalData, IioIndex, Phase);

  return;
}
