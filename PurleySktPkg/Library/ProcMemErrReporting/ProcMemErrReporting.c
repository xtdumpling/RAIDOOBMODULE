/**++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c) 2009 - 2017 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file ProcMemErrorReporting

    Contains Library file that supports processor and memory error handling

---------------------------------------------------------------------------*/

#include <Library/ProcMemErrReporting.h>
#include <UncoreCommonIncludes.h>
#include <CommonErrorHandlerDefs.h>
#include "CpuPciAccess.h"
#include "RcRegs.h"
#include <IndustryStandard/WheaDefs.h>
#include <Library/mpsyncdatalib.h>
#include <Library/emcaplatformhookslib.h>
#include <Protocol/ErrorHandlingProtocol.h>
#include <Protocol/VariableLock.h>
#include <Protocol/SmmVariable.h>
#include <Library/emcalib.h>
#include <Protocol/CrystalRidge.h>
#include <Guid/PprVariable.h>
// APTIOV_SERVER_OVERRIDE_RC_START : OEM Hooks Support
#include <OemRasLib\OemRasLib.h>
#include <emca.h>
#include <Library\AmiErrReportingLib\AmiErrReportingLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks Support

#define PATROL_DIS_MASK 0xF

EFI_CPU_CSR_ACCESS_PROTOCOL       *mCpuCsrAccess1 = NULL;
EFI_ERROR_HANDLING_PROTOCOL       *mErrorHandlingProtocol1 = NULL;
EFI_PLATFORM_RAS_POLICY_PROTOCOL  *mRasPolicy = NULL;
static EFI_RAS_SYSTEM_TOPOLOGY           *mRasTopology = NULL;
EFI_QUIESCE_SUPPORT_PROTOCOL      *mQuiesceSupport = NULL;
static SYSTEM_RAS_SETUP                  *mRasSetup = NULL;
static SYSTEM_RAS_CAPABILITY             *mRasCapability = NULL;
EFI_MEM_RAS_PROTOCOL              *mMemRasProc = NULL;
static EFI_CRYSTAL_RIDGE_PROTOCOL        *mCrystalRidge;
// APTIOV_SERVER_OVERRIDE_RC_START
UEFI_MEM_ERROR_RECORD             *gMemoryErrorRecord;
// APTIOV_SERVER_OVERRIDE_RC_END
static EFI_SMM_VARIABLE_PROTOCOL         *mSmmVariable = NULL;

REPORTER_PROTOCOL                 mReporter;

STATIC EMCA_PHLIB_PRIV mEmcaPh = { 0 };
STATIC UINT64 mMcgCap = 0;
STATIC UINT64 mL1Address = 0;
// bits[3:0] bit map of failed ch
UINT8  Ddr4FailOver[MAX_SOCKET][MAX_MC];
UINT8  DdrtFailOver[MAX_SOCKET][MAX_MC];
//
// ProcessorErrorHandler
//

/**
mRasEventHndlrTable[]
    This table is an array of memory RAS event handler
    functions.  The entry # in the table determines the
    priority of event handling. Entry 0 has the highest
    priority. The priority of event handling can be changed
    by re-ordering the entries in this table.
**/
#define END_OF_TABLE  ((MEM_RAS_EVENT_HNDLR)((UINTN)(-1)))

static
MEM_RAS_EVENT_HNDLR mRasEventHndlrTable[] = {
    HandleAdddcSparing,
    HandleRankSparing,
    HandleSddcPlusOneSparing,
    END_OF_TABLE
};

/**
  Clear all Core errors before enabling them.

  @retval None

**/
VOID
ClearCoreErrors (
  UINT8 Socket
)
{

  return;
}
/**
  Clear CBO error status.

  @retval None

**/
//VOID
//ClearCBOErrors (
//  VOID
//  )
//{
//  return;
//}

/**
  Clear QPI errors.

  @retval None

**/
//VOID
//ClearKTIErrors (
//  VOID
//  )
//{
//  return;
//}

/**
  Clear PCU uncorrected error status.

  @param[in] Socket   The socket number

  @retval None

**/
VOID
ClearPCUErrors (
  IN        UINT8   Socket)
{
 
}

/**
  This function clears the UBOX error status and misc error data

  @param[in] Socket   The socket number

  @retval None

**/
VOID
ClearUboxAndMiscErrorStatus (
  IN        UINT8             Socket
  )
{
  UBOXERRSTS_UBOX_CFG_STRUCT    uboxErrStsReg;
  UBOXERRMISC_UBOX_CFG_STRUCT   uboxErrMisc;
  UBOXERRMISC2_UBOX_CFG_STRUCT  uboxErrMisc2;
  // APTIOV_SERVER_OVERRIDE_RC_START : Clear IerrLoggingReg and MCerrLoggingReg
  IERRLOGGINGREG_UBOX_CFG_STRUCT                    IerrLoggingReg;
  MCERRLOGGINGREG_UBOX_CFG_STRUCT               McerrLoggingReg;
  // APTIOV_SERVER_OVERRIDE_RC_END : Clear IerrLoggingReg and MCerrLoggingReg

  //
  // Note: MCA bank status initially cleared by InitializeMckRegs()
  //       MCA bank status during runtime cleared by OS
  //

  //
  // Clear UBOX uncorrected error status
  //
  uboxErrStsReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, 0, UBOXERRSTS_UBOX_CFG_REG);
  uboxErrStsReg.Bits.poisonrsvd = 0;
  //uboxErrStsReg.Bits.smisrcumc = 0; //Do not clear umc
  uboxErrStsReg.Bits.smisrcimc = 0;
  uboxErrStsReg.Bits.unsupportedopcode = 0;
  uboxErrStsReg.Bits.smi_delivery_valid  = 0;
  mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, 0, UBOXERRSTS_UBOX_CFG_REG, uboxErrStsReg.Data);

  uboxErrMisc.Data = 0;
  mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, 0, UBOXERRMISC_UBOX_CFG_REG, uboxErrMisc.Data);

  uboxErrMisc2.Data = 0;
  mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, 0, UBOXERRMISC2_UBOX_CFG_REG, uboxErrMisc2.Data);
  // APTIOV_SERVER_OVERRIDE_RC_START : Clear IerrLoggingReg and MCerrLoggingReg
  IerrLoggingReg.Data = 0;
  mEmcaPh.CpuCsrAccess->WriteCpuCsr(Socket, 0, IERRLOGGINGREG_UBOX_CFG_REG, IerrLoggingReg.Data);

  McerrLoggingReg.Data = 0;
  mEmcaPh.CpuCsrAccess->WriteCpuCsr(Socket, 0, MCERRLOGGINGREG_UBOX_CFG_REG, McerrLoggingReg.Data);
  // APTIOV_SERVER_OVERRIDE_RC_END : Clear IerrLoggingReg and MCerrLoggingReg

  //
  // Clear PCU uncorrected error status
  //
  ClearPCUErrors (Socket);

  return;

}

VOID
ClearCAPError (
  IN        UINT8   Socket,
  IN        UINT8   Ch
)
{
  CORRERRORSTATUS_MCDDC_DP_STRUCT 		    CorrErrSts;
  LINK_MCA_CTL_MCDDC_DP_STRUCT              LinkMcaCtl;

  LinkMcaCtl.Data = mCpuCsrAccess1->ReadCpuCsr (Socket, Ch, LINK_MCA_CTL_MCDDC_DP_REG);
  LinkMcaCtl.Bits.err4_log = 1;
  mCpuCsrAccess1->WriteCpuCsr (Socket, Ch, LINK_MCA_CTL_MCDDC_DP_REG, LinkMcaCtl.Data);

  CorrErrSts.Data = mCpuCsrAccess1->ReadCpuCsr (Socket, Ch, CORRERRORSTATUS_MCDDC_DP_REG);
  CorrErrSts.Bits.dimm_alert = 0;
  mCpuCsrAccess1->WriteCpuCsr (Socket, Ch, CORRERRORSTATUS_MCDDC_DP_REG, CorrErrSts.Data);

}

/**
  Clears LINK error0 log bit.

  Note: corrected error threshold init is done by MRC

  @param[in]  socket    - socket number
  @param[in]  Ch        - Channel ID

  @retval None
**/
VOID
ClearLinkErr0Sts (
  IN        UINT8   Socket,
  IN        UINT8   Ch
)
{
  LINK_MCA_CTL_MCDDC_DP_STRUCT              LinkMcaCtl;

  LinkMcaCtl.Data = mCpuCsrAccess1->ReadCpuCsr (Socket, Ch, LINK_MCA_CTL_MCDDC_DP_REG);
  LinkMcaCtl.Bits.err0_log = 1;
  mCpuCsrAccess1->WriteCpuCsr (Socket, Ch, LINK_MCA_CTL_MCDDC_DP_REG, LinkMcaCtl.Data);

}

/**
  This function clears Shadow Registers.

  @param[in] Socket   The socket number
  @param[in] Mc   Mc Index

  @retval VOID

**/
VOID
ClearShadowReg (
  IN UINT8 Socket, 
  IN UINT8 Mc
  )
{
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_ADDR_SHADOW0_M2MEM_MAIN_REG, 0);
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_ADDR_SHADOW1_M2MEM_MAIN_REG, 0);
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG, 0);
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG, 0);
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_MISC_SHADOW_N0_M2MEM_MAIN_REG, 0);
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, MCI_MISC_SHADOW_N1_M2MEM_MAIN_REG, 0);
}



/**
  Clears corrected error status/counts for the channel

  Note: corrected error threshold init is done by MRC

  @param[in]  Node      - Memory Node ID
  @param[in]  Ch        - Channel ID

  @retval None
**/
VOID
ClearImcRankCntrs (
  IN        UINT8   Socket,
  IN        UINT8   Mc,
  IN        UINT8   Ch
  )
{
  UINT8                               Node;
  UINT8                               SktCh;
  UINT8                               rank;
  UINT8                               rasModesSupported;
  BOOLEAN                             CopyInProgress;

  CORRERRCNT_0_MCDDC_DP_STRUCT        corrErrCntReg;
  CORRERRORSTATUS_MCDDC_DP_STRUCT     corrErrorStatusReg;

  RASDEBUG((DEBUG_INFO, "<ClearImcRankCntrs(Socket=%d, Mc=%d, Ch=%d)>\n", Socket, Mc, Ch));

  Node = SKT_TO_NODE(Socket, Mc);
  SktCh = NODECH_TO_SKTCH(Node, Ch);

  //
  // Ensure channel is enabled
  //
// APTIOV_SERVER_OVERRIDE_RC_START : EIP269552 system hangs up after injecting ECC error 
 
//- if (mRasTopology->SystemInfo.SocketInfo[Socket].ChannelInfo[Ch].Valid != 0) {
if (mRasTopology->SystemInfo.SocketInfo[Socket].ChannelInfo[SktCh].Valid != 0) {
// APTIOV_SERVER_OVERRIDE_RC_END : EIP269552 system hangs up after injecting ECC error
 
    corrErrorStatusReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG);

    //
    // Selectively clear corrected error count/status for each rank that
    // doesn't have any Memory RAS Modes enabled
    //
    for (rank = 0; rank < MAX_RANK_CH; rank++) {
      rasModesSupported = mMemRasProc->GetSupportedMemRasModes (Node, Ch, rank); //read adddc copy in progress;
      CopyInProgress = mMemRasProc->GetSpareInProgressState(Node,Ch, rank);
      corrErrCntReg.Data  = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, SktCh, CORRERRCNT_0_MCDDC_DP_REG + (rank/2) * 4);

      //
      // Do not clear (by writing a 1) corr error status for ranks
      // that support memory RAS.  It will be handled by mem RAS handler
      //
      if ((rasModesSupported) && (CopyInProgress == FALSE)) {
        corrErrorStatusReg.Bits.err_overflow_stat &= ~(1 << rank);
        if ( (rank % 2) == 0 ) { //Handle fields for even ranks
          corrErrCntReg.Bits.overflow_0 = 0;
        } else { //Handle fields for odd ranks
          corrErrCntReg.Bits.overflow_1 = 0;
        }
      } else {
        RASDEBUG((DEBUG_INFO, "Clearing cor_err_cnt for rank=%d\n", rank));
        if ( (rank % 2) == 0 ) {  //Handle fields for even ranks
          corrErrCntReg.Bits.cor_err_cnt_0 = 0;
          corrErrCntReg.Bits.overflow_0 = 1;
        }
        else {  //Handle fields for odd ranks
          corrErrCntReg.Bits.cor_err_cnt_1 = 0;
          corrErrCntReg.Bits.overflow_1 = 1;
        }
      }

      mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, SktCh, CORRERRCNT_0_MCDDC_DP_REG + (rank/2) * 4, corrErrCntReg.Data);
    }

    //
    // Clear rank corrected error threshold status
    //
    mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG, corrErrorStatusReg.Data);
  }

  RASDEBUG((DEBUG_INFO, "</ClearImcRankCntrs: exit>\n"));
  return;
}

/**
  Enables the generation of SMI for a socket.

  @param[in] Socket   The socket number

  @retval None.

**/
VOID
EnableDisabledGlobalSMIGeneration (
  IN        UINT8                   Socket
  )
{


  SMICTRL_UBOX_MISC_STRUCT  uboxSMICtrlReg;

  // SMiDis3 bit controls SMi generation of UBOX errors. Refre the sighting 4968880.
  // SmiDis programming is moved to EnableUboxError(). based on UboxToPcuMcaEn bit.
  uboxSMICtrlReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, 0, SMICTRL_UBOX_MISC_REG);
  if(mRasSetup->SystemErrorEn != 0) {
    uboxSMICtrlReg.Bits.smidis3 = 0;
  }
  mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, 0, SMICTRL_UBOX_MISC_REG, uboxSMICtrlReg.Data);

  return;
}

/**
  This function returns whether the socket is in a viral state or not.

  @retval ViralState  True if in viral state, otherwise False

**/
BOOLEAN
SocketInKtiViralState (
  VOID
  )
{
  BOOLEAN                                 ViralState;

  KTIVIRAL_KTI_LLPMON_STRUCT              ktiviralReg;
  VIRAL_IIO_RAS_STRUCT                    viraliiorasReg;
  UINT8                                   Socket;
  UINT8                                   LinkIndex;

  ViralState = FALSE;
  viraliiorasReg.Data =0;
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    //
    // Ignore disabled nodes
    //
    if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == 0) {
      continue;
    }

    // Read the KTI Viral state in all the ports for the socket
    for (LinkIndex = 0; LinkIndex < MAX_KTI_PORTS; LinkIndex++) {
      if (mRasTopology->SystemInfo.SocketInfo[Socket].QpiInfo[LinkIndex].LinkValid == TRUE) {
        ktiviralReg.Data = 0;

        ktiviralReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, LinkIndex, KTIVIRAL_KTI_LLPMON_REG);

        if (ktiviralReg.Bits.kti_viral_state == 1) {
          RASDEBUG((DEBUG_INFO,"SocketInKtiViralState: KTI Port: %d Socket ID: %d \n", LinkIndex,Socket));
          RASDEBUG((DEBUG_INFO,"SocketInKtiViralState: kti_viral_state %d\n", ktiviralReg.Bits.kti_viral_state));
          ViralState = TRUE;
          break;
        }
      }
    }
  }

  return ViralState;
}


/**
  Check for fatal or viral conditions.

  @retval EFI_SUCCESS if no viral or fatal conditions

**/
EFI_STATUS
ProcessKtiFatalAndViralError (
  VOID
  )
{
  UBOXERRSTS_UBOX_CFG_STRUCT  UboxErrStsReg;
  UINT8                       Socket;

  RASDEBUG ((DEBUG_INFO,"ProcessKtiFatalAndViralError: In the function\n"));
  if (SocketInKtiViralState () == TRUE) {
    RASDEBUG ((DEBUG_INFO,"ProcessKtiFatalAndViralError: Socket in Viral State\n"));
    ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
    CpuDeadLoop ();  // RASCM_TODO - Do not halt here, instead LogReport() the fatal error and let OEM code decide to halt or not
  }

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    //
    // Skip sockets that are not present
    //
    if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == 0)
      continue;

    UboxErrStsReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, 0, UBOXERRSTS_UBOX_CFG_REG);
    if (UboxErrStsReg.Bits.unsupportedopcode == 1) {
      RASDEBUG ((DEBUG_INFO,"ProcessKtiFatalAndViralError: ubox unsupported opcode\n"));
      ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
      CpuDeadLoop (); // RASCM_TODO - Do not halt here, instead LogReport() the fatal error and let OEM code decide to halt or not
    }
  }
  return EFI_SUCCESS;
}

/**
  Function to enbale Ubox errors.

  @retval None

**/
VOID
EnableUboxError (
  UINT8 Socket
  )
{
  UBOXERRCTL_UBOX_CFG_STRUCT        uboxErrCtrl;
  SMICTRL_UBOX_MISC_STRUCT  uboxSMICtrlReg;

  if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == TRUE) {
    uboxErrCtrl.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, 0, UBOXERRCTL_UBOX_CFG_REG);
    // Sighting 4031463: CLONE from ivytown: need to enable MCA's in R3CSI/R2PCIE
    // Deatils : uboxerrctl.uboxerrortopcumask (bit 24) = 0 This bit will enable UBox propopagation of errors to the PCU.
    if (mRasSetup->SystemErrorEn == 0) {
      uboxErrCtrl.Bits.uboxerrortopcumask = 1;
      mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, 0, UBOXERRCTL_UBOX_CFG_REG, uboxErrCtrl.Data);
    } else {
      uboxSMICtrlReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, 0, SMICTRL_UBOX_MISC_REG);

      if (mRasSetup->UboxToPcuMcaEn == 1) {
        // Unmask ubox to pcu reporting
        uboxErrCtrl.Bits.uboxerrortopcumask = 0;
        // Disable SMI ctrl bits
        uboxSMICtrlReg.Bits.smidis = 1;
        uboxSMICtrlReg.Bits.smidis2 = 1;
      } else {
        // Mask ubox to pcu reporting
        uboxErrCtrl.Bits.uboxerrortopcumask = 1;
        // Enable SMI ctrl bits
        uboxSMICtrlReg.Bits.smidis = 0;
        uboxSMICtrlReg.Bits.smidis2 = 0;
      }
      mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, 0, UBOXERRCTL_UBOX_CFG_REG, uboxErrCtrl.Data);
      mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, 0, SMICTRL_UBOX_MISC_REG, uboxSMICtrlReg.Data);
    }
  }
}

/**
  Enables logging of QPI errors.

  @param	void

  @retval EFI_SUCCESS if the call succeeded.

**/
EFI_STATUS
EnableElogKTI (
  UINT8 Socket
  )
{

  R2PINGERRMSK0_M2UPCIE_MAIN_STRUCT r2pIngErrReg;
  KTIERRDIS0_KTI_LLPMON_STRUCT    ktierrdisReg;

  UINT8 LinkIndex;

  // This code currently only enables/disables support for Clock Data Failover.
  // All other errors are enabled (unmasked) by default, currently.  Need to add setup question support.
  
  LinkIndex = 0;


  if(mRasTopology->SystemInfo.SocketInfo[Socket].Valid == TRUE) {
    r2pIngErrReg.Data = 0;
    mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, 0, R2PINGERRMSK0_M2UPCIE_MAIN_REG, r2pIngErrReg.Data);
  }
  for (LinkIndex = 0; LinkIndex < MAX_KTI_PORTS; LinkIndex++) {
    if(mRasTopology->SystemInfo.SocketInfo[Socket].QpiInfo[LinkIndex].LinkValid == TRUE) {
      ktierrdisReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, LinkIndex, KTIERRDIS0_KTI_LLPMON_REG);
      //Clear smi enable bit (SMI support is not POR so always disable)
      ktierrdisReg.Bits.smi_en = 0;
      mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, LinkIndex, KTIERRDIS0_KTI_LLPMON_REG, ktierrdisReg.Data);
      }
    }

  return EFI_SUCCESS;
}

//
// Memory error Handling
//

/**
  This function enables or disables interrupt generation of memory ecc errors.

  @param[in]  Node    - The node to enable/disable interrupts for.

  @retval None
**/
VOID
EnableDisableIntForEccCorrectedMemErrors (
  IN        UINT8               Socket,
  IN        UINT8               Mc
  )
{
  SMISPARECTL_MC_MAIN_STRUCT    imcSMISpareCtlReg;
  //
  // Enable INT for rank sparing, SDDC and ADDDC sparing  or corrected memory error threshold
  //
  imcSMISpareCtlReg.Data = mEmcaPh.CpuCsrAccess->ReadMcCpuCsr (Socket, Mc, SMISPARECTL_MC_MAIN_REG);

  if (mRasSetup->Memory.SpareIntSelect & SPARE_INT_SMI) {
    imcSMISpareCtlReg.Bits.intrpt_sel_smi = 1;
  } else {
    imcSMISpareCtlReg.Bits.intrpt_sel_smi = 0;
  }
  if (mRasSetup->Memory.SpareIntSelect & SPARE_INT_PIN) {
    imcSMISpareCtlReg.Bits.intrpt_sel_pin = 1;
  } else {
    imcSMISpareCtlReg.Bits.intrpt_sel_pin = 0;
  }
  if (mRasSetup->Memory.SpareIntSelect & SPARE_INT_CMCI) {
    imcSMISpareCtlReg.Bits.intrpt_sel_cmci = 1;
  } else {
    imcSMISpareCtlReg.Bits.intrpt_sel_cmci = 0;
  }

  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, SMISPARECTL_MC_MAIN_REG, imcSMISpareCtlReg.Data);

  return;
}

/**
  This function enables or disables SMI generation of for Mirror failover.

  @param[in]  Node      - Node to init

  @retval None
**/
VOID
EnableDisableIntForMirrorFailover (
  IN        UINT8   Socket,
  IN        UINT8   Mc
  )
{
  SYSFEATURES0_M2MEM_MAIN_STRUCT    SysFeatures0;
  EXRAS_CONFIG_M2MEM_MAIN_STRUCT    ExRasCfg;
  DEFEATURES1_M2MEM_MAIN_STRUCT     Defeatures1M2M;

  /*If Mirroring is enabled and hardware failover mode is enabled then enable the generation of SMI on failover*/
  if ( (mRasTopology->SystemInfo.RasModesEnabled & FULL_MIRROR_1LM)  ||
         (mRasTopology->SystemInfo.RasModesEnabled & FULL_MIRROR_2LM)  ||
            (mRasTopology->SystemInfo.RasModesEnabled & PARTIAL_MIRROR_1LM) ||
              ( mRasTopology->SystemInfo.RasModesEnabled & PARTIAL_MIRROR_2LM) )  {
    SysFeatures0.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, SYSFEATURES0_M2MEM_MAIN_REG);

    if(SysFeatures0.Bits.immediatefailoveractionena == 0) {
      ExRasCfg.Data = mCpuCsrAccess1->ReadCpuCsr(Socket, Mc, EXRAS_CONFIG_M2MEM_MAIN_REG);
      ExRasCfg.Bits.cfgmcasmionfailover = 1;
      if(mRasSetup->Emca.EmcaCsmiEn == 0) {
        ExRasCfg.Bits.cfgmcacmcionfailover = 0;
      }
      mCpuCsrAccess1->WriteCpuCsr( Socket, Mc, EXRAS_CONFIG_M2MEM_MAIN_REG, ExRasCfg.Data);

      Defeatures1M2M.Data  = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, DEFEATURES1_M2MEM_MAIN_REG);
      Defeatures1M2M.Bits.cfgfailoveronscrubchkrdcorr = 0;
      mCpuCsrAccess1->WriteCpuCsr( Socket, Mc, DEFEATURES1_M2MEM_MAIN_REG, Defeatures1M2M.Data);
    }
  }
}

/**
  This function enables or disables SMI generation of memory and HA corrected memeory errors.

  @param[in]  Node      - Node to init

  @retval None
**/
VOID
EnableSmiForCorrectedMemoryErrors (
  IN        UINT8   Socket,
  IN        UINT8   Mc
  )
{

  EnableDisableIntForEccCorrectedMemErrors (Socket, Mc);

  EnableDisableIntForMirrorFailover (Socket, Mc);

}

/**
  Enables memory error reporting

  @retval EFI_SUCCESS if the call is succeed

**/
VOID
EnableErrRepMemory (
  UINT8 Socket
  )
{
  UINT8                             Mc;
  UINT8                             Ch;
  UINT8                             NodeCh;
  RETRY_RD_ERR_LOG_MCDDC_DP_STRUCT  RetryRdErrLog;

  for (Mc = 0; Mc < MAX_IMC; Mc++) {
    // Skip if Mc is not enabled
    if (mRasTopology->SystemInfo.SocketInfo[Socket].imcEnabled[Mc] == 0) continue;

    for (NodeCh = 0; NodeCh < MAX_MC_CH; NodeCh++) {
      Ch = NODECH_TO_SKTCH(SKTMC_TO_NODE(Socket, Mc), NodeCh);
      //Enable memory errors and sparing/patrol scrub errors in the RETRY_RD_ERROR_LOG CSR
      RetryRdErrLog.Data = 0;
      RetryRdErrLog.Bits.en = 1;
      RetryRdErrLog.Bits.en_patspr = 1;
      mCpuCsrAccess1->WriteCpuCsr (Socket, Ch, RETRY_RD_ERR_LOG_MCDDC_DP_REG, RetryRdErrLog.Data);
    } // NodeCh

    //
    // Init IMC correctable errors / Enable SMI routing
    //
    if (mRasSetup->Memory.CorrMemErrEn) {
      for (Ch = 0; Ch < MAX_MC_CH; Ch++) {
        ClearImcRankCntrs (Socket, Mc, Ch);
      }
    }
    EnableSmiForCorrectedMemoryErrors (Socket, Mc);
  } // Node
  return ;
}

/**
  This function enables the signalling and reporting of corrected processor errors.

  @retval EFI_SUCCESS if the call is succeed.

**/
EFI_STATUS
EnableUncoreErrRep (
  UINT8 Socket
  )
{
  //
  // Enable uncore error reporting
  //
  if (mRasSetup->Memory.MemErrEn)
      EnableErrRepMemory(Socket);
  EnableUboxError(Socket);
  EnableElogKTI(Socket);
  return EFI_SUCCESS;
}

/**
  This function enables the signaling and reporting of Command Address Parity(CAP) errors.

  @retval EFI_SUCCESS if the call is succeed.

**/
EFI_STATUS
EnableCAPError (
  UINT8 Socket
  )
{
  UINT8                                      Ch;
  LINK_MCA_CTL_MCDDC_DP_STRUCT               LinkMcaCtl;
  LINK_RETRY_ERR_LIMITS_MCDDC_DP_STRUCT      LinkRetryErrLimit;

  for (Ch = 0; Ch < MAX_CH; Ch++) {
    if (mRasTopology->SystemInfo.SocketInfo[Socket].ChannelInfo[Ch].Valid == 0)  continue;
    LinkMcaCtl.Data = mCpuCsrAccess1->ReadCpuCsr (Socket, Ch, LINK_MCA_CTL_MCDDC_DP_REG);
    LinkMcaCtl.Bits.err4_en = 1;
    LinkMcaCtl.Bits.err4_log = 1;
    LinkMcaCtl.Bits.err4_smi = 1;
    mCpuCsrAccess1->WriteCpuCsr (Socket, Ch, LINK_MCA_CTL_MCDDC_DP_REG, LinkMcaCtl.Data);

    LinkRetryErrLimit.Data = mCpuCsrAccess1->ReadCpuCsr (Socket, Ch, LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG);
    LinkRetryErrLimit.Bits.sb_err_enable = 1;
    mCpuCsrAccess1->WriteCpuCsr (Socket, Ch, LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG, LinkRetryErrLimit.Data);

  }

  return EFI_SUCCESS;
}

/**
  This function enables the SMI signaling when link goes in link fail state.

  @retval EFI_SUCCESS if the call is succeed.

**/
EFI_STATUS
EnableLinkFailError (
  UINT8 Socket
  )
{
  UINT8                                      Ch;
  LINK_MCA_CTL_MCDDC_DP_STRUCT               LinkMcaCtl;
  
  for (Ch = 0; Ch < MAX_CH; Ch++) {
    if (mRasTopology->SystemInfo.SocketInfo[Socket].ChannelInfo[Ch].Valid == 0)  continue;
    LinkMcaCtl.Data = mCpuCsrAccess1->ReadCpuCsr (Socket, Ch, LINK_MCA_CTL_MCDDC_DP_REG);
    LinkMcaCtl.Bits.err0_en = 1;
    LinkMcaCtl.Bits.err0_log = 1;
    LinkMcaCtl.Bits.err0_smi = 1;
    mCpuCsrAccess1->WriteCpuCsr (Socket, Ch, LINK_MCA_CTL_MCDDC_DP_REG, LinkMcaCtl.Data);
  }
  return EFI_SUCCESS;
}





/**
  Handle Mirror Failover2 on the given node.

  @param[in]  Node      - The node to handle

  @retval None
**/
VOID
MirrorFailoverHandler (
  IN        UINT8   Socket,
  IN        UINT8   Mc
  )
{

  UINT8                               TadIndex;
  BOOLEAN                             MirrorFailOver = FALSE;
  UINT8                               ChFailed = 0xF;
  UINT8                               SecondaryCh = 0xF;
  UINT8                               ChFailed_A, ChFailed_B, ChFailed_C;
  UINT8                               Mccmdmemregion_A,Mccmdmemregion_B,Mccmdmemregion_C;
  UINT8                               MirrorFailover_A,MirrorFailover_B,MirrorFailover_C;
  UINT8                               McCmdVld_A,McCmdVld_B,McCmdVld_C;
  UINT32                              MailBoxCommand;
  UINT32                              MailboxData;
  UINT32                              MailboxSts;
  UINT32                              ApicId;
  UINT32                              BankIdx;
  UINT32                              ScrubEnSave;
  UINT32                              LinkFailSave;

  EFI_CPU_PHYSICAL_LOCATION               Loc;
  MIRRORFAILOVER_M2MEM_MAIN_STRUCT        Mfo;
  TADBASE_0_MC_MAIN_STRUCT                TadBase;
  SYSFEATURES0_M2MEM_MAIN_STRUCT          SysFeatures0;
  AMAP_MC_MAIN_STRUCT                     AMAPReg;
  MCI_STATUS_SHADOW_N0_M2MEM_MAIN_STRUCT  MciStatusShadow0;
  MCI_STATUS_SHADOW_N1_M2MEM_MAIN_STRUCT  MciStatusShadow1;
  MCI_MISC_SHADOW_N1_M2MEM_MAIN_STRUCT    MciMiscShadow1;
  DDRT_RETRY_FSM_STATE_MC_2LM_STRUCT      ddrtRetryFsm;
  DEFEATURES0_M2MEM_MAIN_STRUCT           imcDefeatures;
  SCRUBCTL_MC_MAIN_STRUCT                 ScrubCtl;
  SCRUBMASK_MC_MAIN_STRUCT                ScrubMask;
  MIRRORCHNLS_M2MEM_MAIN_STRUCT           MirrorChannels;
  LINK_RETRY_ERR_LIMITS_MCDDC_DP_STRUCT   LinkRetryErrLimit;
  // APTIOV_SERVER_OVERRIDE_RC_START
  BOOLEAN                           MirrFailoverSkipEvent = FALSE;
  // APTIOV_SERVER_OVERRIDE_RC_END
  UINT32 TadBaseRegOffset[TAD_RULES] = {
    TADBASE_0_MC_MAIN_REG, TADBASE_1_MC_MAIN_REG, TADBASE_2_MC_MAIN_REG, TADBASE_3_MC_MAIN_REG,
    TADBASE_4_MC_MAIN_REG, TADBASE_5_MC_MAIN_REG, TADBASE_6_MC_MAIN_REG, TADBASE_7_MC_MAIN_REG 
    } ;
  ChFailed_A = 0;

  //Check if mirror enabled
  if ( (mRasTopology->SystemInfo.RasModesEnabled & CH_ALL_MIRROR))  {
    RASDEBUG ((DEBUG_ERROR, "MirrorFailoverHandler : MIRROR ENABLED \n"));
    // APTIOV_SERVER_OVERRIDE_RC_START : Fill MemoryRecord here as code in LogCorrectedMemError() for filling is commented.
    FillMemRecordBeforeHandling(SKT_TO_NODE(Socket, Mc),gMemoryErrorRecord, ERROR_EVENT_MIRR_FAILOVER);
    OemReadyToTakeAction(gMemoryErrorRecord, &MirrFailoverSkipEvent); 
    if (MirrFailoverSkipEvent == TRUE) {
        DEBUG((DEBUG_INFO,"OEM requests to Skip Mirror Failover Event!\n"));
        return;
    }
    // APTIOV_SERVER_OVERRIDE_RC_END : Fill MemoryRecord here as code in LogCorrectedMemError() for filling is commented.

    SysFeatures0.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, SYSFEATURES0_M2MEM_MAIN_REG);

    if(SysFeatures0.Bits.immediatefailoveractionena == 0) {
      RASDEBUG ((DEBUG_ERROR, "Bios failover handler Socket %d Mc %d \n", Socket, Mc));
      MciStatusShadow1.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, MCI_STATUS_SHADOW_N1_M2MEM_MAIN_REG);
      if (MciStatusShadow1.Bits.valid && MciStatusShadow1.Bits.addrv) {
        MciMiscShadow1.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, MCI_MISC_SHADOW_N1_M2MEM_MAIN_REG);
        if (MciMiscShadow1.Bits.mirrorfailover) {

          //Reading the register three times and ensuring that all three reads returned same value
          //this will reduce probability for any  other (e.g. ECC) corrected errors returning a channel and hence failing over the wrong channel
          MciStatusShadow0.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG);
          ChFailed_A        = ( MciStatusShadow0.Bits.mcacod & 0x3 );
          MciMiscShadow1.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, MCI_MISC_SHADOW_N1_M2MEM_MAIN_REG);
          Mccmdmemregion_A    =  (UINT8)MciMiscShadow1.Bits.mccmdmemregion;
          MirrorFailover_A    =  (UINT8)MciMiscShadow1.Bits.mirrorfailover;
          McCmdVld_A          =  (UINT8)MciMiscShadow1.Bits.mccmdvld;

          MciStatusShadow0.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG);
          ChFailed_B        = ( MciStatusShadow0.Bits.mcacod & 0x3 );
          MciMiscShadow1.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, MCI_MISC_SHADOW_N1_M2MEM_MAIN_REG);
          Mccmdmemregion_B    =  (UINT8)MciMiscShadow1.Bits.mccmdmemregion;
          MirrorFailover_B    =  (UINT8)MciMiscShadow1.Bits.mirrorfailover;
          McCmdVld_B          =  (UINT8)MciMiscShadow1.Bits.mccmdvld;

          MciStatusShadow0.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, MCI_STATUS_SHADOW_N0_M2MEM_MAIN_REG);
          ChFailed_C        = ( MciStatusShadow0.Bits.mcacod & 0x3 );
          MciMiscShadow1.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, MCI_MISC_SHADOW_N1_M2MEM_MAIN_REG);
          Mccmdmemregion_C    =  (UINT8)MciMiscShadow1.Bits.mccmdmemregion;
          MirrorFailover_C    =  (UINT8)MciMiscShadow1.Bits.mirrorfailover;
          McCmdVld_C          =  (UINT8)MciMiscShadow1.Bits.mccmdvld;

          if( ( (ChFailed_A == ChFailed_B) && (ChFailed_A == ChFailed_C) )  &&
              ( (MirrorFailover_A == MirrorFailover_B) && (MirrorFailover_A == MirrorFailover_C) ) &&
              ( (Mccmdmemregion_A == Mccmdmemregion_B) && (Mccmdmemregion_A == Mccmdmemregion_C) ) &&
              ( (McCmdVld_A == 1) && (McCmdVld_A  == McCmdVld_B) && (McCmdVld_A == McCmdVld_C) )
              ) {
            RASDEBUG ((DEBUG_ERROR, "\t 1LM DDR4 Error \n"));
            Mfo.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, MIRRORFAILOVER_M2MEM_MAIN_REG);
            ChFailed = 1 << ChFailed_A;
            /* Interface:
                Command[7:0]  = 0xb0
                Address[28:8] = 0x0

              Data:
                Workaround Index[7:0] = 0x2
                Param2[18:16] = failedCh, one hot encoded`
                Param2[19] = 0x1
                Param2[20] = 0x1 for DDR-T, 0x0 for DDR4
             */

            if (Mccmdmemregion_A >> 2 == 0) { //1LM DDR4 error
              if(Mfo.Bits.ddr4chnlfailed==0) { //No Channels failed over - Do SW failover
                //B2P Command to Quiesce and setup MfoDdr4Chfailed bit
                //the B2P command will also setup the TADBASE registers for chfailed.

                ScrubCtl.Data = mCpuCsrAccess1->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
                ScrubEnSave = ScrubCtl.Bits.scrub_en;
                ScrubCtl.Bits.scrub_en = 0;
                mCpuCsrAccess1->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtl.Data);

                ScrubMask.Data =  mCpuCsrAccess1->ReadMcCpuCsr(Socket, Mc, SCRUBMASK_MC_MAIN_REG);
                LinkFailSave = ScrubMask.Bits.link_fail;
                ScrubMask.Bits.link_fail = ChFailed;
                mCpuCsrAccess1->WriteMcCpuCsr(Socket, Mc, SCRUBMASK_MC_MAIN_REG, ScrubMask.Data);

                RASDEBUG ((DEBUG_ERROR, "\t B2P call One Hot encoded channel failed: %d\n",ChFailed));
                MailBoxCommand = (UINT32)MAILBOX_BIOS_CMD_MISC_WORKAROUND_ENABLE;
                MailboxData    = (0x2) | (Mc << 0x8) | BIT19 | (ChFailed << 16) ;
                MailboxSts     =  mCpuCsrAccess1->Bios2PcodeMailBoxWrite(Socket, MailBoxCommand, MailboxData);
                //if quiesce B2P call successful
                if (MailboxSts==0) {

                  RASDEBUG ((DEBUG_ERROR, "\t Mirror Failover successful\n"));

                  Mfo.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, MIRRORFAILOVER_M2MEM_MAIN_REG);
                  RASDEBUG ((DEBUG_ERROR, "\t Mfo.Bits.ddr4chnlfailed:%d\n",Mfo.Bits.ddr4chnlfailed));
                  for (TadIndex = 0; TadIndex < TAD_RULES; TadIndex++) {
                    TadBase.Data = mCpuCsrAccess1->ReadMcCpuCsr(Socket, Mc, TadBaseRegOffset[TadIndex]);
                    if (TadBase.Bits.mirror_en) {
                      RASDEBUG ((DEBUG_ERROR, "MirrorFailoverHandler2 :Tad[%d].en_failover:%d \n",TadIndex,TadBase.Bits.en_failover));
                      RASDEBUG ((DEBUG_ERROR, "MirrorFailoverHandler2 :Tad[%d].failed_ch:%d \n",TadIndex,TadBase.Bits.failed_ch));
                    }
                  }
                  MirrorFailOver = TRUE;
                  Ddr4FailOver[Socket][Mc] |= ChFailed;

                  //Disable Link fail on secondary channel after failover
                  MirrorChannels.Data = mCpuCsrAccess1->ReadMcCpuCsr(Socket, Mc, MIRRORCHNLS_M2MEM_MAIN_REG);
                  switch (ChFailed_A) {
                  case 0:
                    SecondaryCh = (UINT8)MirrorChannels.Bits.ddr4chnl0secondary;
                    break;
                  case 1:
                    SecondaryCh = (UINT8)MirrorChannels.Bits.ddr4chnl1secondary;
                    break;
                  case 2:
                    SecondaryCh = (UINT8)MirrorChannels.Bits.ddr4chnl2secondary;
                    break;
                  default:
                    RASDEBUG ((DEBUG_ERROR, "Incorrect failed Ch cannot disable link!!\n"));
                    break;
                  }
                  if (SecondaryCh != 0xF) {
                    LinkRetryErrLimit.Data = mCpuCsrAccess1->ReadCpuCsr(Socket, NODECH_TO_SKTCH(Mc,SecondaryCh), LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG);
                    LinkRetryErrLimit.Bits.sb_err_enable = 0;
                    mCpuCsrAccess1->WriteCpuCsr(Socket, NODECH_TO_SKTCH(Mc,SecondaryCh), LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG , LinkRetryErrLimit.Data);
                  }
                  // Log the Error
                  LogCorrectedMemError (COR_ERROR_TYPE, MEM_MIRROR_FAILOVER, SKT_TO_NODE(Socket, Mc), ChFailed_A, 0, 0, 0);
                  if(mRasSetup->Emca.EmcaCsmiEn == 0) {
                    BankIdx = M2M0_BANK + Mc;
                    Loc.Package = Socket;
                    Loc.Core = 0;
                    Loc.Thread= 0;
                    ApicId = GetApicIdByLoc(&Loc);
                    SignalCmciToOs(BankIdx,
                                   ApicId
                                   );
                  }
                } //B2P Successful

                /*Re-enable patrolscrub and Link-fail regardless of the status of mirror failover*/
                ScrubCtl.Data = mCpuCsrAccess1->ReadMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG);
                ScrubCtl.Bits.scrub_en = ScrubEnSave;
                mCpuCsrAccess1->WriteMcCpuCsr(Socket, Mc, SCRUBCTL_MC_MAIN_REG, ScrubCtl.Data);

                ScrubMask.Data =  mCpuCsrAccess1->ReadMcCpuCsr(Socket, Mc, SCRUBMASK_MC_MAIN_REG);
                ScrubMask.Bits.link_fail = LinkFailSave;
                mCpuCsrAccess1->WriteMcCpuCsr(Socket, Mc, SCRUBMASK_MC_MAIN_REG, ScrubMask.Data);

              } // DDR4chnlfailed = 0
            } //DDR4 Error Handler
            else if (Mccmdmemregion_A >> 2 == 1) { //DDRT error
              RASDEBUG ((DEBUG_ERROR, "\t DDRTLinkFailHandler : DDRT Failover Error \n"));
              if((Mfo.Bits.ddrtchnlfailed==0) && (DdrtFailOver[Socket][Mc] == 0) )  { //No Channels failed over - Do SW failover
                // link failover handler
                ddrtRetryFsm.Data =  mCpuCsrAccess1->ReadCpuCsr (Socket, NODECH_TO_SKTCH(Mc, ChFailed_A), DDRT_RETRY_FSM_STATE_MC_2LM_REG);
                if( ddrtRetryFsm.Bits.current_state == 0x08)  {
                  RASDEBUG ((DEBUG_ERROR, " Link Error : Skt = %x, Mc = %x, ChFailed = %x \n",Socket, Mc, ChFailed_A));
                  MailBoxCommand = (UINT32)MAILBOX_BIOS_CMD_MISC_WORKAROUND_ENABLE;
                  MailboxData    = (0x2) | (Mc << 0x8) | BIT19 | BIT20 | (ChFailed << 16) ;
                  MailboxSts     =  mCpuCsrAccess1->Bios2PcodeMailBoxWrite(Socket, MailBoxCommand, MailboxData);

                  if (MailboxSts==0) {
                    //Disable Link fail on secondary channel after failover
                    MirrorChannels.Data = mCpuCsrAccess1->ReadMcCpuCsr(Socket, Mc, MIRRORCHNLS_M2MEM_MAIN_REG);
                    switch (ChFailed_A) {
                    case 0:
                      SecondaryCh = (UINT8)MirrorChannels.Bits.ddrtchnl0secondary;
                      break;
                    case 1:
                      SecondaryCh = (UINT8)MirrorChannels.Bits.ddrtchnl1secondary;
                      break;
                    case 2:
                      SecondaryCh = (UINT8)MirrorChannels.Bits.ddrtchnl2secondary;
                      break;
                    default:
                      RASDEBUG ((DEBUG_ERROR, "Incorrect failed Ch cannot disable link!!\n"));
                      break;
                    }
                    if (SecondaryCh != 0xF) {
                      LinkRetryErrLimit.Data = mCpuCsrAccess1->ReadCpuCsr(Socket, NODECH_TO_SKTCH(Mc,SecondaryCh), LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG);
                      LinkRetryErrLimit.Bits.sb_err_enable = 0;
                      mCpuCsrAccess1->WriteCpuCsr(Socket, NODECH_TO_SKTCH(Mc,SecondaryCh), LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG , LinkRetryErrLimit.Data);
                    }
                    DdrtFailOver[Socket][Mc] |= ChFailed;
                    LogCorrectedMemError (COR_ERROR_TYPE, MEM_MIRROR_FAILOVER, SKT_TO_NODE(Socket, Mc), ChFailed_A, 0, 0, 0);
                  } //B2P Successful
                } //ddrtretryfsm state
              } //Check no channel failed over
            } //DDRT error
          } //Read the correct Channel
        } //misc.failover set
      }
    }

    if (MirrorFailOver) {

      //Skip all ranks in the channel from patrol scrub
      AMAPReg.Data= mCpuCsrAccess1->ReadCpuCsr(Socket, NODECH_TO_SKTCH(Mc, ChFailed_A), AMAP_MC_MAIN_REG);
      AMAPReg.Bits.dimm0_pat_rank_disable  |= PATROL_DIS_MASK;
      AMAPReg.Bits.dimm1_pat_rank_disable  |= PATROL_DIS_MASK;
      mCpuCsrAccess1->WriteCpuCsr(Socket, NODECH_TO_SKTCH(Mc, ChFailed_A), AMAP_MC_MAIN_REG, AMAPReg.Data);

      /*5371865: CLONE SKX Sighting: ADDDC retry_rd_err_log_address1.faildevice incorrect on 3rd Strike*/
      //Disable Scrub check read after failover
      imcDefeatures.Data = mCpuCsrAccess1->ReadMcCpuCsr (Socket, Mc, DEFEATURES0_M2MEM_MAIN_REG);
      imcDefeatures.Bits.scrubcheckrddis = 1;
      mCpuCsrAccess1->WriteMcCpuCsr (Socket, Mc, DEFEATURES0_M2MEM_MAIN_REG, imcDefeatures.Data);

      mMemRasProc->InitializeAdddcAfterMirror(Mc);

    }

  }//Mirror enabled
  return;
}


/**
  Enable Viral on each KTI link.

  @param[in] Skt      - The socket to enable

  @retval None

**/
VOID
EnableKtiViralPerSkt (
  IN        UINT8                   Skt
  )
{
  UINT8                             LinkIndex;
  UINT8                             Ctr;

  KTIVIRAL_KTI_LLPMON_STRUCT        ktiviralReg;
  KTIVIRAL_KTI_LLPMON_STRUCT        ktiViralMask;

  KTIERRDIS0_KTI_LLPMON_STRUCT      ktierrdisReg;

  R2GLERRCFG_M2UPCIE_MAIN_STRUCT    r2glerrcfgReg;

  M3GLERRCFG_M3KTI_MAIN_STRUCT      m3glerrcfgReg;


  if (mRasTopology->SystemInfo.SocketInfo[Skt].Valid == TRUE) {

      for (LinkIndex = 0; LinkIndex < MAX_KTI_PORTS; LinkIndex++) {
        if (mRasTopology->SystemInfo.SocketInfo[Skt].QpiInfo[LinkIndex].LinkValid == TRUE) {
          if (mRasSetup->ViralEn == TRUE) {
            //
            // Clear failover status
            //
            ktiViralMask.Data = 0;
            ktiviralReg.Data = mCpuCsrAccess1->ReadCpuCsr (Skt, LinkIndex, KTIVIRAL_KTI_LLPMON_REG);
            if ( ktiviralReg.Bits.kti_unc_err_set ||
                 ktiviralReg.Bits.kti_pkt_viral_set ||
                 ktiviralReg.Bits.kti_viral_status ||
                 ktiviralReg.Bits.kti_viral_state
                 ) {
              ktiviralReg.Bits.kti_unc_err_set = 1;
              ktiViralMask.Bits.kti_unc_err_set |= ktiviralReg.Bits.kti_unc_err_set;

              ktiviralReg.Bits.kti_pkt_viral_set = 1;
              ktiViralMask.Bits.kti_pkt_viral_set |= ktiviralReg.Bits.kti_pkt_viral_set;

              if (mRasSetup->ClearViralStatus == 1) {
                ktiviralReg.Bits.kti_viral_status = 1;
                ktiViralMask.Bits.kti_viral_status |= ktiviralReg.Bits.kti_viral_status;
              }
              else {
                ktiviralReg.Bits.kti_viral_status = 0;
                ktiViralMask.Bits.kti_viral_status |= ktiviralReg.Bits.kti_viral_status;
              }
              ktiviralReg.Bits.kti_viral_state = 1;
              ktiViralMask.Bits.kti_viral_state |= ktiviralReg.Bits.kti_viral_state;

              mCpuCsrAccess1->WriteCpuCsr (Skt, LinkIndex, KTIVIRAL_KTI_LLPMON_REG, ktiviralReg.Data);

              while (1) {
                ktiviralReg.Data = mCpuCsrAccess1->ReadCpuCsr (Skt, LinkIndex, KTIVIRAL_KTI_LLPMON_REG);
                if ( (ktiviralReg.Data & ktiViralMask.Data) == 0)
                  break;
                ktiviralReg.Data |= ktiViralMask.Data;
                mCpuCsrAccess1->WriteCpuCsr (Skt, LinkIndex, KTIVIRAL_KTI_LLPMON_REG, ktiviralReg.Data);
              }
              ktiviralReg.Data = mCpuCsrAccess1->ReadCpuCsr (Skt, LinkIndex, KTIVIRAL_KTI_LLPMON_REG); 

              if (ktiviralReg.Bits.kti_viral_status == 1) {
                RASDEBUG ((DEBUG_ERROR, "KTI viral status found in asserted state Socket no : %d  LinkIndex : %d \n", Skt, LinkIndex));
                ktiviralReg.Bits.kti_viral_status = 0;
              }
            }
            ktiviralReg.Bits.kti_viral_alert_enable = 1;
            ktiviralReg.Bits.kti_signal_global_fatal = 1;

            mCpuCsrAccess1->WriteCpuCsr (Skt, LinkIndex, KTIVIRAL_KTI_LLPMON_REG, ktiviralReg.Data);

            ktiviralReg.Bits.kti_global_viral_mask = 1;
            mCpuCsrAccess1->WriteCpuCsr (Skt, LinkIndex, KTIVIRAL_KTI_LLPMON_REG, ktiviralReg.Data);

            ktierrdisReg.Data = mCpuCsrAccess1->ReadCpuCsr (Skt, LinkIndex, KTIERRDIS0_KTI_LLPMON_REG);
            //Clear smi enable bit (SMI support is not POR so always disable)
            ktierrdisReg.Bits.smi_en = 0;
            mCpuCsrAccess1->WriteCpuCsr (Skt, LinkIndex, KTIERRDIS0_KTI_LLPMON_REG, ktierrdisReg.Data);
          } //if (mRasSetup->ViralEn == TRUE)

        } //if (mRasTopology->SystemInfo.SocketInfo[Skt].QpiInfo[LinkIndex].LinkValid == TRUE)
      } // for (LinkIndex = 0; LinkIndex < MAX_KTI_PORTS; LinkIndex++)

      //
      // Enable Viral in KTI and PCIe logic
      //
      if (mRasSetup->ViralEn == TRUE) {
        for (Ctr = 0; Ctr < mRasTopology->SystemInfo.SocketInfo[Skt].UncoreInfo.M2PcieCount; Ctr++) {
          r2glerrcfgReg.Data = mCpuCsrAccess1->ReadCpuCsr (Skt, Ctr, R2GLERRCFG_M2UPCIE_MAIN_REG);
          //
          // Unmask bit [0-17] in this register
          //
          r2glerrcfgReg.Bits.resetglobalfatalerror = 0;
          r2glerrcfgReg.Bits.resetglobalviral = 0 ;
          r2glerrcfgReg.Bits.maskuboxviralin = 0;
          r2glerrcfgReg.Bits.maskuboxfatalerrorin = 0;
          r2glerrcfgReg.Bits.maskiiofatalerrorin = 0;
          r2glerrcfgReg.Bits.maskiioviralout = 0; 
          r2glerrcfgReg.Bits.maskiioviralin = 0;
          r2glerrcfgReg.Bits.maskr2fatalerror = 0;
          r2glerrcfgReg.Bits.localerrorstatus = 0;

          mCpuCsrAccess1->WriteCpuCsr (Skt, Ctr, R2GLERRCFG_M2UPCIE_MAIN_REG,r2glerrcfgReg.Data);
        }

        //
        // Enable Viral in M3KTI
        //
        for (Ctr = 0; Ctr < mRasTopology->SystemInfo.SocketInfo[Skt].UncoreInfo.M3UpiCount; Ctr++) {
          m3glerrcfgReg.Data = mCpuCsrAccess1->ReadCpuCsr (Skt, Ctr, M3GLERRCFG_M3KTI_MAIN_REG);
          //
          // Unmask bit [0-17] in this register
          //
          m3glerrcfgReg.Bits.resetglobalfatalerror = 0;
          m3glerrcfgReg.Bits.resetglobalviral = 0;
          m3glerrcfgReg.Bits.maskm3fatalerror = 0;
          m3glerrcfgReg.Bits.maskktifatalerrorin = 0;
          m3glerrcfgReg.Bits.maskktiviralout = 0;
          m3glerrcfgReg.Bits.maskktiiviralin = 0;
          m3glerrcfgReg.Bits.maskpcufatalerrorout = 0;

          mCpuCsrAccess1->WriteCpuCsr (Skt, Ctr, M3GLERRCFG_M3KTI_MAIN_REG, m3glerrcfgReg.Data);
        }
      } //if (mRasSetup->ViralEn == TRUE)

  } //if (mRasTopology->SystemInfo.SocketInfo[Skt].Valid == TRUE) 
}



/**
  Enable Viral in each UBOX.

  @param[in] Skt      - The socket to enable

  @retval None

**/
VOID
EnableUBoxViralPerSkt(
  IN        UINT8   Skt
  )
{

  UBOX_GL_ERR_CFG_UBOX_CFG_STRUCT   uboxglerrcfgReg;

   // Enable Viral in ubox
  if(mRasTopology->SystemInfo.SocketInfo[Skt].Valid == TRUE) {
      uboxglerrcfgReg.Data = mCpuCsrAccess1->ReadCpuCsr (Skt, 0, UBOX_GL_ERR_CFG_UBOX_CFG_REG);
      uboxglerrcfgReg.Bits.resetuboxfatalerror = 0;
      uboxglerrcfgReg.Bits.resetuboxviral = 0;
      uboxglerrcfgReg.Bits.maskuboxumc  = 1;
      uboxglerrcfgReg.Bits.maskuboxierr = 1;
      uboxglerrcfgReg.Bits.maskunsupportopcode = 0;
      uboxglerrcfgReg.Bits.maskpoisonreception = 0;
      uboxglerrcfgReg.Bits.maskcfgrdaddrmisalign = 0;
      uboxglerrcfgReg.Bits.maskcfgwraddrmisalign = 0;
      uboxglerrcfgReg.Bits.masksmitimeout = 0;
      uboxglerrcfgReg.Bits.maskmasterlocktimeout = 0;
      uboxglerrcfgReg.Bits.maskpcuproxyierr = 2;
      uboxglerrcfgReg.Bits.maskpcuproxyumc = 2;
      uboxglerrcfgReg.Bits.maskpcuproxymsmi = 0;
      uboxglerrcfgReg.Bits.maskextviral = 1;
      mCpuCsrAccess1->WriteCpuCsr (Skt, 0, UBOX_GL_ERR_CFG_UBOX_CFG_REG, uboxglerrcfgReg.Data );
  }
}

/**
  Enable poison and viral

  @retval None.

**/
VOID
EnableDisableViralPoisonOnCore (
  IN        VOID  *Buffer
  )
{
  UINT64          Data;
  UINTN           CpuNumber;

  CpuNumber = GetAPCpuNumber();

  if ((mRasSetup->ViralEn == 1) && (mRasSetup->PoisonEn == 1) && (mRasSetup->SystemErrorEn == 1)) { // Enable viral & poison if Viral is enabled in Setup
  
    Data = AsmReadMsr64(MSR_MCG_CONTAIN);
    Data |= BIT1 + BIT0;
    AsmWriteMsr64 (MSR_MCG_CONTAIN, Data);
  } else if (mRasSetup->PoisonEn == 1 && (mRasSetup->SystemErrorEn == 1)) {
    // Write poison enable bit in MCG_CONTAIN MSR
    // Note: write will GP fault if poison not supported by core
    Data = AsmReadMsr64 (MSR_MCG_CONTAIN);
    Data |= BIT0; // just enable poison
    AsmWriteMsr64 (MSR_MCG_CONTAIN, Data);
  } else {
    // Disable Poison & viral bit
    Data = AsmReadMsr64 (MSR_MCG_CONTAIN);
    Data &= ~(03); // Clear bit0 & bit1 - poison & viral  bits
    AsmWriteMsr64 (MSR_MCG_CONTAIN, Data);
  }
  return;
}

/**
  Check each channel for correctable error threshold overflow.

  @param[in]  Node      - The node to handle

  @retval BOOLEAN       - True if there is an error, otherwise false
          DdrCh         - The channel with the error
          Dimm          - The dimm with the error
          Rank          - The dimm rank that had the error
**/
BOOLEAN
GetErrInfo (
  IN        UINT8   Socket,
  IN        UINT8   *DdrCh,
  IN        UINT8   *Dimm,
  IN        UINT8   *Rank
  )
{
  UINT8             Node;
  UINT8             NodeCh;
  UINT8             ch;
  UINT8             rank;
  UINT8             dimmSlot;
  UINT8             PhysicalRank;
  UINT8             Mc;

  CORRERRORSTATUS_MCDDC_DP_STRUCT corrErrStsReg;

  for (ch = 0; ch < MAX_CH; ch++) {
    if (mRasTopology->SystemInfo.SocketInfo[Socket].ChannelInfo[ch].Valid == 0)
      continue;

    // Check if there is any rank in this channel that has overflow bit set
    for (rank = 0; rank < MAX_RANK_CH ; rank++) {
      // Check error status in CPU for this rank
      corrErrStsReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, ch, CORRERRORSTATUS_MCDDC_DP_REG);
      if (corrErrStsReg.Bits.err_overflow_stat & (1 << rank)) {
        Mc = ch % MAX_MC_CH; // convert socketch to Mc
        Node = SKTMC_TO_NODE(Socket, Mc);
        NodeCh = SKTCH_TO_NODECH(ch);
        PhysicalRank = mMemRasProc->GetLogicalToPhysicalRankId (Node, NodeCh, rank);
        dimmSlot = PhysicalRank / MAX_RANK_DIMM;
        *Dimm = dimmSlot;
        *Rank = PhysicalRank % MAX_RANK_DIMM;
        *DdrCh = ch; // channel at socket level
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
  Disables the SMI generation of memory ECC errors.

  @param[in]  node number   - The node to disable SMI for.

  @retval None
**/
VOID
DisableSMIForCorrectedMemErrorThreshold (
  IN        UINT8   Socket,
  IN        UINT8   Mc
  )
{
  SMISPARECTL_MC_MAIN_STRUCT    imcSMISpareCtlReg;

  //
  // Disable SMI for sparing or corrected memory error threshold
  //

  imcSMISpareCtlReg.Data =  mEmcaPh.CpuCsrAccess->ReadMcCpuCsr (Socket, Mc, SMISPARECTL_MC_MAIN_REG);
  imcSMISpareCtlReg.Bits.intrpt_sel_smi = 0;
  mEmcaPh.CpuCsrAccess->WriteMcCpuCsr (Socket, Mc, SMISPARECTL_MC_MAIN_REG, imcSMISpareCtlReg.Data);

  return;
}



/**
  Check and Handle ADDDC sparing

  @param[in]      NodeID          - Memory controller ID
  @param[in]      EventType       - In progress / new events
  @param[in, out] *IsEventHandled - ptr to buffer (BOOLEAN) to get Event handled status

  @retval status - EFI_SUCCESS - no error
       non-zero code - if error
  IsEventHandled - Event handled status - TRUE/FALSE

**/
static
EFI_STATUS
HandleAdddcSparing (
  IN        UINT8       NodeId,
  IN        EVENT_TYPE  EventType,
  IN  OUT   BOOLEAN     *IsEventHandled
  )
{
    // APTIOV_SERVER_OVERRIDE_RC_START : OEM Hooks support
    BOOLEAN AdddcSkipEvent = FALSE;

    if (mMemRasProc->SystemMemInfo->ExRasModesEnabled & ADDDC_EN) {
        if (EventType == NEW_EVENTS ) {
            ZeroMem (gMemoryErrorRecord, sizeof (UEFI_MEM_ERROR_RECORD));
            FillMemRecordBeforeHandling(NodeId, gMemoryErrorRecord, ERROR_EVENT_ADDDC);
            OemReadyToTakeAction(gMemoryErrorRecord, &AdddcSkipEvent); // Oem Hook To decide the Error action based on the Error data
        }
    }
       
    if (AdddcSkipEvent == TRUE) {
        DEBUG((DEBUG_INFO,"OEM request..Skipping ADDDC Sparing Event!\n"));
        *IsEventHandled = FALSE;
        return EFI_SUCCESS;
    }
    // APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks support

    return mMemRasProc->CheckAndHandleAdddcSparing (NodeId, EventType, IsEventHandled);
}

/**
  Handle Sparing Events - Rank Sparing

  @param[in]      NodeID          - Memory controller ID
  @param[in]      EventType       - In progress / new events
  @param[in, out] *IsEventHandled - ptr to buffer (BOOLEAN) to get Event handled status

  @retval status - EFI_SUCCESS - no error
       non-zero code - if error
  IsEventHandled - Event handled status - TRUE/FALSE

**/
static
EFI_STATUS
HandleRankSparing (
  IN        UINT8       NodeId,
  IN        EVENT_TYPE  EventType,
  IN  OUT   BOOLEAN     *IsEventHandled
  )
{
    // APTIOV_SERVER_OVERRIDE_RC_START : OEM Hooks support
    BOOLEAN RankSparingSkipEvent = FALSE;

    if (mRasTopology->SystemInfo.RasModesEnabled & RK_SPARE) {
        if (EventType == NEW_EVENTS ) {
            ZeroMem (gMemoryErrorRecord, sizeof (UEFI_MEM_ERROR_RECORD));
            FillMemRecordBeforeHandling(NodeId, gMemoryErrorRecord, ERROR_EVENT_RANK_SPARING);
            OemReadyToTakeAction(gMemoryErrorRecord, &RankSparingSkipEvent); // Oem Hook To decide the Error action based on the Error data
        }
    }

    if (RankSparingSkipEvent == TRUE) {
        DEBUG((DEBUG_INFO,"OEM request..Skipping Rank Sparing Event!\n"));
        *IsEventHandled = FALSE;
        return EFI_SUCCESS;
    }
    // APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks support

    return mMemRasProc->CheckAndHandleRankSparing (NodeId, EventType, IsEventHandled);
}

/**
  Handle Sparing Events -  SDDC Plus one sparing

  @param[in]      NodeID          - Memory controller ID
  @param[in]      EventType       - In progress / new events
  @param[in, out] *IsEventHandled - ptr to buffer (BOOLEAN) to get Event handled status

  @retval status - EFI_SUCCESS - no error
       non-zero code - if error
  IsEventHandled - Event handled status - TRUE/FALSE

**/
static
EFI_STATUS
HandleSddcPlusOneSparing (
  IN        UINT8       NodeId,
  IN        EVENT_TYPE  EventType,
  IN  OUT   BOOLEAN     *IsEventHandled
  )
{
    // APTIOV_SERVER_OVERRIDE_RC_START : OEM Hooks support
    BOOLEAN SddcSkipEvent = FALSE;

    if (mMemRasProc->SystemMemInfo->ExRasModesEnabled & SDDC_EN) {
        if (EventType == NEW_EVENTS ) {
		    ZeroMem (gMemoryErrorRecord, sizeof (UEFI_MEM_ERROR_RECORD));
            FillMemRecordBeforeHandling(NodeId,gMemoryErrorRecord, ERROR_EVENT_SDDC);
            OemReadyToTakeAction(gMemoryErrorRecord, &SddcSkipEvent); // Oem Hook To decide the Error action based on the Error data
        }
    }

    if (SddcSkipEvent == TRUE) {
        DEBUG((DEBUG_INFO,"OEM request..Skipping SDDC Plus One Event!\n"));
        *IsEventHandled = FALSE;
        return EFI_SUCCESS;
    }
    // APTIOV_SERVER_OVERRIDE_RC_END : OEM Hooks support

    return mMemRasProc->CheckAndHandleSddcPlusOneSparing (NodeId, EventType, IsEventHandled);
}

UINT16 Crc16 (char *ptr, INTN count)
{
  INT16 crc, i;
  crc = 0;
  while (--count >= 0)
  {
    crc = crc ^ (INT16)(int)*ptr++ << 8;
    for (i = 0; i < 8; ++i)
    {
      if (crc & 0x8000)
      {
        crc = crc << 1 ^ 0x1021;
      }
      else
      {
        crc = crc << 1;
      }
    }
  }
  return (crc & 0xFFFF);
}


void
UpdatePprAddress(
    UINT8            NodeId,
    UINT8            ChannelId,
    FAILED_DIMM_INFO FailedDimmInfo,
	BOOLEAN          IsCorrectableError
  )
//
// IF PPR testing is enabled, update PPR err address
//
{
  UINT8             pprIndex;
  UINTN             varSize;
  UINT16            oldCrc;
  UINT16            curCrc;
  EFI_STATUS        status;
  PPR_ADDR_VARIABLE pprData;

  varSize = sizeof(PPR_ADDR_VARIABLE);
  if (mRasSetup->Memory.pprType && mRasSetup->Memory.pprErrInjTest) {
    status = mSmmVariable->SmmGetVariable (
                                PPR_VARIABLE_NAME,
                                &gEfiPprVariableGuid,
                                NULL,
                                &varSize,
                                &pprData
                                );
    oldCrc = pprData.crc16;
    pprData.crc16 = 0;
    curCrc = Crc16 ((char*)&pprData, (INTN)varSize);
    if (EFI_ERROR(status) || (oldCrc != curCrc)) {
      //
      // Variable is not present. Initialize the data structure to all 0
      //
      ZeroMem (&pprData, varSize);
      RASDEBUG ((EFI_D_ERROR | EFI_D_INFO, "[PPR] Var not found or crc mismatch! Zero-out struct - oldcrc:0x%0x, curCrc:0x%0x\n", oldCrc, curCrc));
    }

    for (pprIndex = 0; pprIndex < MAX_PPR_ADDR_ENTRIES; pprIndex++) {
      if (!(pprData.pprAddrSetup[pprIndex].pprAddrStatus & PPR_STS_ADDR_VALID)) {
        pprData.pprAddrSetup[pprIndex].pprAddrStatus |= PPR_STS_ADDR_VALID;
        pprData.pprAddrSetup[pprIndex].socket         = NODE_TO_SKT(NodeId);
        pprData.pprAddrSetup[pprIndex].mc             = NODE_TO_MC(NodeId);
        pprData.pprAddrSetup[pprIndex].ch             = ChannelId;
        pprData.pprAddrSetup[pprIndex].dimm           = FailedDimmInfo.FailedDimm;
        pprData.pprAddrSetup[pprIndex].rank           = FailedDimmInfo.FailedRank;
        pprData.pprAddrSetup[pprIndex].bank           = (FailedDimmInfo.FailedBG << 2) | FailedDimmInfo.FailedBank;
        pprData.pprAddrSetup[pprIndex].row            = FailedDimmInfo.FailedRow;
        pprData.pprAddrSetup[pprIndex].subRank        = FailedDimmInfo.FailedSubRank;
        pprData.pprAddrSetup[pprIndex].dramMask       = IsCorrectableError? (1 << FailedDimmInfo.FailedDev) : 0; //Fill in 0 for UE for MRC to detect failing device

        RASDEBUG ((EFI_D_ERROR | EFI_D_INFO, "[PPR] Error logged - Entry: %d \n", pprIndex));
        RASDEBUG ((EFI_D_ERROR | EFI_D_INFO, "[PPR] Address-Socket[%d].MC[%d].CH[%d].D[%d].R[%d].CID[%d].Bank[%d].Row[%d].FailDevMask = 0x%0x\n",
            pprData.pprAddrSetup[pprIndex].socket,
            pprData.pprAddrSetup[pprIndex].mc,
            pprData.pprAddrSetup[pprIndex].ch,
            pprData.pprAddrSetup[pprIndex].dimm,
            pprData.pprAddrSetup[pprIndex].rank,
            pprData.pprAddrSetup[pprIndex].subRank,
            pprData.pprAddrSetup[pprIndex].bank,
            pprData.pprAddrSetup[pprIndex].row,
            pprData.pprAddrSetup[pprIndex].dramMask));

        //
        // Update CRC and update the variable
        //
        pprData.crc16 = 0;
        pprData.crc16 = Crc16 ((char*)&pprData, varSize);

        status = mSmmVariable->SmmSetVariable (
                                PPR_VARIABLE_NAME,
                                &gEfiPprVariableGuid,
                                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                varSize,
                                &pprData
                                );
        if (EFI_ERROR(status)) {
          RASDEBUG ((EFI_D_ERROR | EFI_D_INFO, "PPR: Var Update for PPR new entry failed - status: %d\n",
              "SetVariable failed",
              status)
          );
        }
        break;
      }
    } // for loop
  } // if pprType and pprErrInjTest
}

/**
  Handle corrected memory errors.

  @param[in]  Socket              - The socket to check
  @param[in]  ImcEventsProgress   - Events in progress
  @param[in]  ImcNewEvents        - New events

  @retval None
**/
VOID
CorrectedMemoryErrorHandler (
  IN        UINT8   Socket,
  IN        UINT32  ImcEventsProgress,
  IN        UINT32  ImcNewEvents
  )
{
  BOOLEAN                             isEventHandled;
  UINT8                               rasEventHndlrEntry;
  UINT8                               ddrCh;
  UINT8                               dimm;
  UINT8                               rank;
  UINT8                               node;
  UINT8                               Mc;
  UINT8                               RasModesSupported;
  EFI_STATUS                          status;
  BOOLEAN                             isCAPE;
  LINK_MCA_CTL_MCDDC_DP_STRUCT        LinkMcaCtl;
  FAILED_DIMM_INFO                    FailedDimmInfo;
  // APTIOV_SERVER_OVERRIDE_RC_START
  BOOLEAN                             DefHandlerSkipEvent = FALSE;
  CORRERRORSTATUS_MCDDC_DP_STRUCT           corrErrStsReg; // fix for loop is now executing without checking the overflow status bit.
  // APTIOV_SERVER_OVERRIDE_RC_END

  isEventHandled = FALSE;

  ASSERT (Socket < MAX_SOCKET);
  RASDEBUG ((DEBUG_ERROR, "  CorrectedMemoryErrorHandler function Entry: \n" ));

  for (Mc = 0; Mc < MAX_IMC; Mc++) {
    node = SKTMC_TO_NODE(Socket, Mc);
    RASDEBUG ((DEBUG_ERROR, "  CorrectedMemoryErrorHandler: MC = %d \n",Mc));
    // Skip if memory controller is not enabled
    if (mRasTopology->SystemInfo.SocketInfo[Socket].imcEnabled[Mc] == 0)
      continue;

    MirrorFailoverHandler (Socket, Mc);

    //
    // Handle ADDDC / Rank Sparing / SDDC / Corrected error logging
    //     Check if there are any events in progress or new events
    //     and handle them. This SMI could be to handle
    //     completion of an event or new event
    //
    if (ImcEventsProgress != 0 || ImcNewEvents != 0) {
      if (ImcEventsProgress & (1 << node)) {
        for (rasEventHndlrEntry = 0; mRasEventHndlrTable[rasEventHndlrEntry] != END_OF_TABLE; rasEventHndlrEntry++) {
          status = mRasEventHndlrTable[rasEventHndlrEntry] (node, EVENTS_IN_PROGRESS, &isEventHandled);
          ASSERT (status == EFI_SUCCESS);
          if (isEventHandled == TRUE) {
            break;      // Event handled on this node. Check other nodes
          }
        } // for(rasEventHndlrEntry)
      } else if (ImcNewEvents & (1 << node)) {
        for (rasEventHndlrEntry = 0; mRasEventHndlrTable[rasEventHndlrEntry] != END_OF_TABLE; rasEventHndlrEntry++) {
          RASDEBUG((DEBUG_INFO,"Handling rasEventHndlrEntry number %d\n",rasEventHndlrEntry));
          status = mRasEventHndlrTable[rasEventHndlrEntry] (node, NEW_EVENTS, &isEventHandled);
          ASSERT (status == EFI_SUCCESS);
          if (isEventHandled == TRUE) {
            mMemRasProc->GetLastErrInfo (node, &ddrCh, &dimm, &rank);
            RASDEBUG ((DEBUG_ERROR, "CorrectedMemoryErrorHandler: call LogCorrectedMemoryError \n" ));
            //The Rank got from Last error info may be incorrect if Rank is in VLS. Use the channel information to look up the rank
            status = mMemRasProc->GetFailedDimmErrorInfo (node, ddrCh, 1, 0, &FailedDimmInfo);
            RASDEBUG ((DEBUG_ERROR, "CorrectedMemoryErrorHandler (ECC Error) : node = %d, ddrCh = %d, rank = %d, dimm = %d \n",node, ddrCh, FailedDimmInfo.FailedRank, FailedDimmInfo.FailedDimm));
            LogCorrectedMemError (COR_ERROR_TYPE, MEM_ECC_ERROR, node, ddrCh, FailedDimmInfo.FailedRank, FailedDimmInfo.FailedDimm, (UINT8)0);
            break;      // Event handled on this node. Check other nodes
          }
        }
      } // for(rasEventHndlrEntry)
    } // if( ImcEventsProgress != 0 || ImcNewEvents != 0 )
    // APTIOV_SERVER_OVERRIDE_RC_START
    //Call the default handler only when isEventHandled is False
    if (isEventHandled == FALSE) { 
    for(ddrCh = 0; ddrCh < MAX_MC_CH; ddrCh++) {
        DefHandlerSkipEvent = FALSE;
    // APTIOV_SERVER_OVERRIDE_RC_END
      //ensure ch is enabled
      if (mRasTopology->SystemInfo.SocketInfo[Socket].ChannelInfo[NODECH_TO_SKTCH(node,ddrCh)].Valid == 0)
        continue;
// APTIOV_SERVER_OVERRIDE_RC_START: fix for loop is now executing without checking the overflow status bit.
      corrErrStsReg.Data = mCpuCsrAccess->ReadCpuCsr(Socket, NODECH_TO_SKTCH(node,ddrCh), CORRERRORSTATUS_MCDDC_DP_REG);
      if (corrErrStsReg.Bits.err_overflow_stat == 0 ) continue;
// APTIOV_SERVER_OVERRIDE_RC_END: fix for loop is now executing without checking the overflow status bit.  
      LinkMcaCtl.Data = mCpuCsrAccess1->ReadCpuCsr (Socket, NODECH_TO_SKTCH(node,ddrCh), LINK_MCA_CTL_MCDDC_DP_REG);
      isCAPE = (BOOLEAN)(LinkMcaCtl.Bits.err4_smi & LinkMcaCtl.Bits.err4_log);

      RASDEBUG ((DEBUG_ERROR, "CorrectedMemoryErrorHandler: call GetFailedDimmErrorInfo  for node %d, channel %d\n", node,ddrCh));
      if (isCAPE) {
    	status = mMemRasProc->GetFailedDimmErrorInfo (node, ddrCh, 2, 0, &FailedDimmInfo);
      } else {
        status = mMemRasProc->GetFailedDimmErrorInfo (node, ddrCh, 1, 0, &FailedDimmInfo);
      }
      if (status == EFI_SUCCESS) {
        rank = FailedDimmInfo.FailedRank;
        dimm = FailedDimmInfo.FailedDimm;
        RasModesSupported = mMemRasProc->GetSupportedMemRasModes (node, ddrCh, rank);

        if (!isCAPE) {
          // PPR flow expects variables in MC and McCh instead of Socket and SktCh
          UpdatePprAddress(node, ddrCh, FailedDimmInfo, TRUE);
        }

        if ((RasModesSupported == 0) && ( Ddr4FailOver[Socket][Mc] != (1 << ddrCh))) {
          RASDEBUG ((DEBUG_ERROR, "CorrectedMemoryErrorHandler: call LogCorrectedMemoryError \n" ));
          RASDEBUG ((DEBUG_ERROR, "CorrectedMemoryErrorHandler (ECC Error) : node = %d, ddrCh = %d, rank = %d, dimm = %d \n",node, ddrCh, rank, dimm));
          // APTIOV_SERVER_OVERRIDE_RC_START : Fill MemoryRecord here as code in LogCorrectedMemError() for filling is commented.          
          FillMemRecordBeforeHandling(SKT_TO_NODE(Socket, Mc),gMemoryErrorRecord, DEF_HANDLER);
          OemReadyToTakeAction(gMemoryErrorRecord, &DefHandlerSkipEvent);  // Oem Hook To decide the Error action based on the Error data
          if (DefHandlerSkipEvent == TRUE) {
              DEBUG((DEBUG_INFO,"OEM requests to Skip Default handler!\n"));
              continue;
          }
          // APTIOV_SERVER_OVERRIDE_RC_END : Fill MemoryRecord here as code in LogCorrectedMemError() for filling is commented.
          LogCorrectedMemError (COR_ERROR_TYPE, MEM_ECC_ERROR, node, ddrCh, rank, (UINT8)dimm, (UINT8)0);
        }
      }
    } // channel loop
    // APTIOV_SERVER_OVERRIDE_RC_START : Call the default handler only when isEventHandled is False
    } 
    // APTIOV_SERVER_OVERRIDE_RC_END : Call the default handler only when isEventHandled is False
  } // Mc loop

  RASDEBUG ((DEBUG_ERROR, "  CorrectedMemoryErrorHandler function Exit: \n"));
  return;
}

/**
  Corrected memory error handler.

  @retval Status.

**/
EFI_STATUS
ElogProcessorErrorHandler ( // RASCM_TODO - common code !??
      OUT   UINT32            *Sev
  )
{
  UINT8                       Socket;
  UINT32                      UmcSockets;
  UINT32                      ImcSockets;
  UINT32                      ImcNodesEventProgress;
  UINT32                      ImcNodesNewEvents;
  // APTIOV_SERVER_OVERRIDE_RC_START
  BOOLEAN 		              AllHandlersSkipEvent = FALSE;
  UmcSockets = 0;
  ImcSockets = 0;
  // APTIOV_SERVER_OVERRIDE_RC_END
  ImcNodesEventProgress = 0;
  ImcNodesNewEvents = 0;

  //
  // This handler is installed directly with SMM Base. The last thing we want is to get lost in
  // a protracted search for errors, thereby consuming excess time in SMM mode. Therefore, we need
  // to first quickly determine if we have a pending CPU error at all, and then kick-start the remainder
  // of core error handling part only when there is indeed an error...
  //
  if (!PendingProcessorErrorExists (&UmcSockets, &ImcSockets, &ImcNodesEventProgress, &ImcNodesNewEvents)) {
    RASDEBUG ((DEBUG_INFO, "ElogProcessorErrorHandler : No Pending Errors!!! \n"));
    return EFI_SUCCESS;
  }

  if ((mRasSetup->Memory.CorrMemErrEn == 1) || (mRasSetup->Emca.EmcaCsmiEn != 0) || (mRasSetup->Memory.SpareIntSelect & SPARE_INT_SMI)) {
    // APTIOV_SERVER_OVERRIDE_RC_START : OEM hooks support
    gMemoryErrorRecord->Section.ErrorEvent = SKIP_CE_HANDLING;
    OemReadyToTakeAction(gMemoryErrorRecord, &AllHandlersSkipEvent); // Oem Hook To decide the Error action based on the Error data

    if (AllHandlersSkipEvent == TRUE) {
        DEBUG((DEBUG_INFO,"OEM request..Skipping all corrected memory error handlers!\n"));
        return EFI_SUCCESS;
    }
    // APTIOV_SERVER_OVERRIDE_RC_END : Oem Hooks support
    *Sev = EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED;
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      //
      // Ignore disabled nodes
      //
      if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == TRUE) {
        RASDEBUG ((DEBUG_INFO, "ElogProcessorErrorHandler : ImcSockets - %x \n",ImcSockets));
        if (ImcSockets & (1 << Socket)) {
          RASDEBUG ((DEBUG_INFO, "ElogProcessorErrorHandler : IMCNodesNewEvents     - %x \n",ImcNodesNewEvents));
          RASDEBUG ((DEBUG_INFO, "ElogProcessorErrorHandler : ImcNodesEventProgress - %x \n",ImcNodesEventProgress));
          CorrectedMemoryErrorHandler (Socket, ImcNodesEventProgress, ImcNodesNewEvents);
          }
      } else {
        RASDEBUG ((DEBUG_ERROR, "ElogProcessorErrorHandler : Socket %d is not valid skipping\n",Socket));
      }
    }
  }
  return EFI_SUCCESS;
}

/**
  This function executes boot(init) time Si workarounds for RAS

  @retval EFI_SUCCESS if the call is succeed

**/
//VOID
//SiWAForInit (
//  VOID
//  )
//{
//  // place holder for Silicon RAS Workarounds if any
//
//}


/**
  This function executes boot(init) time Si workarounds for RAS

  @retval EFI_SUCCESS if the call is succeed

**/
//VOID
//SiWAForRuntime (
//  VOID
//  )
//{
//  // place holder for Silicon RAS Workarounds if any
//
//}


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//
// Interfaces between silicon and common module
//

BOOLEAN
ProcMemIsSupported (
  VOID
  )
{
  UINT8 SystemErrorEn = mRasSetup->SystemErrorEn;
  UINT8 ProcMemErrorEn = mRasSetup->Memory.MemErrEn;
  RASDEBUG ((DEBUG_INFO, "<ProcMemIsSupported>\n"));
  return (SystemErrorEn && ProcMemErrorEn);
}

/**
  Enable ECC Corrected memory error reporting, Viral and Poison

  None

  @retval EFI_SUCCESS.

**/

EFI_STATUS
ProcMemEnableReporting (
  VOID
  )
{
  UINT8                             Socket;
  UINT8                             Channel;
  UINT8                             Dimm;
  UINT8                             IioStack;
  UINT32                            Data;
  EFI_STATUS                        Status;
  DDR4_VIRAL_CTL_MCDDC_DP_STRUCT    Ddr4ViralCtlStruct;
  DDRT_DEFEATURE_MC_2LM_STRUCT      DdrtDefeatureStruct;
  IIOERRCTL_IIO_RAS_STRUCT          IioErrCtl;
  MCDDRTCFG_MC_MAIN_STRUCT          McDdrtCfg;

  RASDEBUG ((DEBUG_INFO, "<ProcMemEnableReporting>\n"));

  //
  // The following code is the w/a to enable viral on SKX A-Stepping
  // s5331151 -BIOS Workaround For SKX Bug ECO 306492 Viral Enablement
  //
  if (mEmcaPh.IioUds->SystemStatus.MinimumCpuStepping <  B0_REV_SKX) {
    if ((mRasCapability->SiliconRasCapability.PoisonCap != 0) && (mRasSetup->ViralEn == 1) && (mRasSetup->PoisonEn == 1) && (mRasSetup->SystemErrorEn == 1)  ) { 
      for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
        if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == TRUE) {
          RASDEBUG ((DEBUG_INFO, "Applying the W/A for Viral on socket 0x%\n", Socket));
          for (Channel = 0; Channel < MAX_CH; Channel++) {
            Ddr4ViralCtlStruct.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, Channel, DDR4_VIRAL_CTL_MCDDC_DP_REG);
            Ddr4ViralCtlStruct.Bits.dis_fv_viral_bit = 0;
            Ddr4ViralCtlStruct.Bits.dis_wpq_wpa_par_err = 0;
            Ddr4ViralCtlStruct.Bits.dis_rpq0_rpa_par_err = 0;
            Ddr4ViralCtlStruct.Bits.dis_rpq1_rpa_par_err = 0;
            mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, Channel, DDR4_VIRAL_CTL_MCDDC_DP_REG, Ddr4ViralCtlStruct.Data);
            
          }
        }
      }
    }
  }

  //w/a 5331846:
  if ((mRasSetup->PoisonEn == 1) && (mRasSetup->SystemErrorEn == 1) && (mRasCapability->SiliconRasCapability.PoisonCap != 0)) {
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == TRUE) {
          for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
            if (mRasTopology->SystemInfo.SocketInfo[Socket].UncoreIioInfo.IioStackInfo[IioStack].Valid) {
          IioErrCtl.Data = 0;
              IioErrCtl.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, IioStack, IIOERRCTL_IIO_RAS_REG);
          IioErrCtl.Bits.c8_ib_header_parity_core_poison_en = 1;
          IioErrCtl.Bits.c8_ib_header_parity_p2p_poison_en = 1;
          IioErrCtl.Bits.c8_ib_header_parity_sticky_poison = 1;
              mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, IioStack, IIOERRCTL_IIO_RAS_REG, IioErrCtl.Data);
        }
      }
    }
  }
    } 

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == TRUE) {
      EnableUncoreErrRep(Socket);
      // Enable or disable Global SMI generation
      //EnableLinkFailError(Socket);  /* We don't want err0_en set*/
      //EnableCAPError(Socket); /* We can't check here if CAP is enabled*/
      EnableDisabledGlobalSMIGeneration(Socket);
      // Program Error Signals
      if (mRasSetup->ViralEn == 1) {
        EnableKtiViralPerSkt (Socket);
        EnableUBoxViralPerSkt (Socket);

        //5371945: CLONE SKX Sighting: Memory Controller Returning Poison when Viral is triggered and Link Error FSM indicates 'Idle
        for (Channel = 0; Channel < MAX_CH; Channel++) {

          McDdrtCfg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, Channel, MCDDRTCFG_MC_MAIN_REG);

          DdrtDefeatureStruct.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, Channel, DDRT_DEFEATURE_MC_2LM_REG);
          if ((McDdrtCfg.Bits.slot0 == 0) && (McDdrtCfg.Bits.slot1 == 0)) {
            DdrtDefeatureStruct.Bits.ignore_viral = 1;
          } else {
            DdrtDefeatureStruct.Bits.ignore_viral = 0;
          }
          mEmcaPh.CpuCsrAccess->WriteCpuCsr (Socket, Channel, DDRT_DEFEATURE_MC_2LM_REG, DdrtDefeatureStruct.Data);

          for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
            Status = mCrystalRidge->NvmCtlrGetCfgDataPolicy (Socket, Channel, Dimm, &Data);
            if (!EFI_ERROR (Status)) {
              Data |= 0x2; // Viral Policy Enable = 1
              Status = mCrystalRidge->NvmCtlrSetCfgDataPolicy (Socket, Channel, Dimm, Data);
              if (EFI_ERROR (Status)) {
                RASDEBUG ((DEBUG_INFO, "Viral config not set: Set Config fail!! \n"));
              }
            } else {
              RASDEBUG ((DEBUG_INFO, "Viral config not set: Get Config fail!! \n"));
            }
          }
        }
      }
      if (mRasCapability->SiliconRasCapability.PoisonCap != 0) {
        RASDEBUG ((DEBUG_INFO, "Executing EnableDisableViralPoisonOnCore on Socket 0x%x \n", Socket));
        SmmStartupCoreApBlocking (Socket, EnableDisableViralPoisonOnCore, NULL); // MSR_MCG_CONTAIN (178h) is a CORE scope MSR.
      }
    }
  }
  return EFI_SUCCESS;
}

/**

  Disables the SMI(Legacy Path) Generation of Correctable ECC Errors.
  Does not include support for disabling Mirror Failover SMI or CSMI

  @param DeviceType          - Device information
  @param EventType           - Event Type
  @param ExtData             - Information about Socket and MC

  @retval Status.

**/

EFI_STATUS
ProcMemDisableReporting (
  UINT32 DeviceType,
  UINT32 EventType,
  UINT32 *ExtData
  )
{
  UINT8   Socket;
  UINT8   Mc;

  RASDEBUG ((DEBUG_INFO, "<ProcMemDisableReporting>\n"));
  if (ExtData) {
    Socket = (UINT8) ExtData[0];
    Mc = (UINT8) ExtData[1];
    DisableSMIForCorrectedMemErrorThreshold(Socket, Mc);
  }
  return EFI_SUCCESS;
}

/**
  Checks to see if pending processor errors exist. 

  @param[out] UmcSockets            Sockets with UMC pending
  @param[out] ImcSockets            Sockets with IMC
  @param[out] ImcNodesEventProgress Bitmap of nodes with events in progress
  @param[out] ImcNodesNewEvents     Bitmap of nodes with new errors

  @retval Status.

**/
BOOLEAN
PendingProcessorErrorExists (
      OUT   UINT32            *UmcSockets,
      OUT   UINT32            *ImcSockets,
      OUT   UINT32            *ImcNodesEventProgress,
      OUT   UINT32            *ImcNodesNewEvents
  )
{
  UINT8                       Socket;
  UBOXERRSTS_UBOX_CFG_STRUCT  UboxErrStsReg;
  GET_SMI_SRC_ARGS_STRUCT Params;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {

    //
    // Ignore disabled nodes
    //
    if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == 0) {
      continue;
    }

    if (mRasCapability->SiliconRasCapability.EmcaGen2Cap && (mRasSetup->Emca.EmcaMsmiEn == 2 || mRasSetup->Emca.EmcaCsmiEn == 2) ) {
      Params.McBankBits = 0;
      Params.Mode = MODE_EMCA;

      SmmStartupPackageApBlocking(Socket,
                                  GetUncoreMcBankErrors,
                                  &Params);
      
      if (Params.McBankBits != 0) {
        *UmcSockets |= (1 << Socket);
      }

      if (GetMcBankMemErrors (Params.McBankBits) == TRUE) {
        *ImcSockets |= (1 << Socket);
      }

    } 
      
    //
    // Check for pending processor errors
    //
    UboxErrStsReg.Data = mCpuCsrAccess1->ReadCpuCsr (Socket, 0, UBOXERRSTS_UBOX_CFG_REG);

      //if (UboxErrStsReg.Bits.smisrcumc == 1) {
        // Track which sockets have umc pending
      //  *UmcSockets |= (1 << Socket);
      //}
      if (UboxErrStsReg.Bits.smisrcimc == 1) {
        // Track which sockets have imc pending
        *ImcSockets |= (1 << Socket);
      }
    //}
  }

  mMemRasProc->GetBitMapOfNodeWithErrors (ImcNodesNewEvents);
  mMemRasProc->GetBitMapOfNodeWithEventInProgress (ImcNodesEventProgress);

  //if((*UmcSockets != 0) || (*ImcNodesNewEvents != 0) ||(*ImcNodesEventProgress != 0) || (*ImcSockets != 0)) {
  if((*ImcNodesNewEvents != 0) ||(*ImcNodesEventProgress != 0) || (*ImcSockets != 0)) {
    return TRUE;
  } else {
    return FALSE;
  }
}



/**
  Checks for pending processor errors.

  @retval TRUE  - Pending error exists
          FALSE - No pending error exists

**/
BOOLEAN
ProcMemCheckStatus (
  VOID
  )
{
  UINT8                       Socket;
  UINTN                       Thread;
  UINT8                       SktCh;
  BOOLEAN                     ErrorFound;
  CORRERRORSTATUS_MCDDC_DP_STRUCT     corrErrorStatusReg;
  UBOXERRSTS_UBOX_CFG_STRUCT      uboxErrorStatusReg;

  //RASDEBUG ((DEBUG_INFO, "<ProcMemCheckStatus>\n"));

  if((mRasSetup->Emca.EmcaCsmiEn == 2 || mRasSetup->Emca.EmcaEn == 1) && (mRasCapability->SiliconRasCapability.EmcaGen2Cap == TRUE)) {
    ErrorFound = FALSE;
    for (Thread = 0; Thread < gSmst->NumberOfCpus; Thread++) {
      if (Thread == gSmst->CurrentlyExecutingCpu) {
        EmcaSmiSrc (&ErrorFound);
      } else {
        SmmBlockingStartupThisAp (EmcaSmiSrc, Thread, &ErrorFound);
      }
      if(ErrorFound == TRUE) {
        return TRUE;
      }
    }
  }
  // Check per rank error overflow status in legacy path.
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    // Ignore disabled nodes
    if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == 0) continue;
    if (mRasSetup->Memory.CorrMemErrEn) {
      uboxErrorStatusReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr( Socket, 0, UBOXERRSTS_UBOX_CFG_REG );
      if( uboxErrorStatusReg.Bits.smi_delivery_valid && uboxErrorStatusReg.Bits.smisrcimc)
        return TRUE;

      for (SktCh = 0; SktCh < MAX_CH; SktCh++) {
        if (mRasTopology->SystemInfo.SocketInfo[Socket].ChannelInfo[SktCh].Valid != 0) {
          corrErrorStatusReg.Data = mEmcaPh.CpuCsrAccess->ReadCpuCsr (Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG);
          if(corrErrorStatusReg.Bits.err_overflow_stat) {
            RASDEBUG ((DEBUG_INFO, "<ProcMemCheckStatus returning TRUE>\n"));
            return TRUE;
          }
        }
      }
    }
  }
  return FALSE;  //  returning true to clear the per rank error counters
}

EFI_STATUS
ProcMemDetectAndHandle (
  UINT32    *CurrentSev,
  ClearOption      Clear       // Flag. 0 - do not clear the detected/handled error status. 1 - clear it.
  ) {


  RASDEBUG ((DEBUG_INFO, "\n<ProcMemDetectAndHandle>\n"));

  //SiWAForRuntime();

  ProcessKtiFatalAndViralError ();

  ElogProcessorErrorHandler (CurrentSev);

  return EFI_SUCCESS;
}

EFI_STATUS
ProcMemClearStatus (
  VOID
  ) {
  UINT8 Socket;
  UINT8 Mc;
  UINT8 Ch;

  RASDEBUG ((DEBUG_INFO, "<ProcMemClearStatus>\n"));
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (mRasTopology->SystemInfo.SocketInfo[Socket].Valid == TRUE) {
      ClearCoreErrors (Socket);
      for (Mc = 0; Mc < mRasTopology->SystemInfo.NumMemNode; Mc++) {
        if (mRasTopology->SystemInfo.SocketInfo[Socket].imcEnabled[Mc] == 0) continue;
        ClearShadowReg(Socket,Mc);
        for (Ch = 0; Ch < MAX_MC_CH; Ch++) {
          //ensure ch is enabled
           if (mRasTopology->SystemInfo.SocketInfo[Socket].ChannelInfo[NODECH_TO_SKTCH(Mc,Ch)].Valid == 0)
             continue;
          ClearImcRankCntrs (Socket, Mc, Ch);
          ClearCAPError(Socket, NODECH_TO_SKTCH(Mc, Ch));
          ClearLinkErr0Sts(Socket, NODECH_TO_SKTCH(Mc, Ch));
        }
      }
      //ClearCBOErrors();
      //ClearKTIErrors();
      ClearUboxAndMiscErrorStatus(Socket);
    }
  }
  return EFI_SUCCESS;
}

VOID
InitializePPRData()
{
  EFI_STATUS                    Status;
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;
  PPR_ADDR_VARIABLE             pprData;
  UINTN                         varSize;

  varSize = sizeof(PPR_ADDR_VARIABLE);
  ZeroMem (&pprData, varSize);

  //
  // Clear PPR Variable from storage
  //
  Status = gRT->SetVariable (
      PPR_VARIABLE_NAME,
      &gEfiPprVariableGuid,
      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
      varSize,
      &pprData
  );
  //
  // Mark PPR_VARIABLE_NAME variable to read-only if the Variable Lock protocol exists
  // Still lock it even the variable cannot be saved to prevent it's set by 3rd party code.
  //
  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
  if (!EFI_ERROR (Status)) {
    Status = VariableLock->RequestToLock (VariableLock, PPR_VARIABLE_NAME, &gEfiPprVariableGuid);
    ASSERT_EFI_ERROR (Status);
  }
}

/**

    Allocates protocols and initialize internal structure required by the library.

    @param ImageHandle - refers to the image handle of the driver.
    @param SystemTable - points to the EFI System Table.

    @retval EFI_SUCCESS - Library constructor executed correctly

**/
EFI_STATUS
EFIAPI
ProcMemErrReportingLibConstructor (
IN EFI_HANDLE ImageHandle,
IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS        Status;
  BOOLEAN           InSmm;
  EFI_IIO_UDS_PROTOCOL    *iioUdsProtocol;

  RASDEBUG ((DEBUG_INFO, "<ProcMemErrReportingLibConstructor>\n"));
  if(mEmcaPh.SmmBase == NULL) {
    Status = gBS->LocateProtocol(
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **) &mEmcaPh.SmmBase
                  );
    ASSERT_EFI_ERROR(Status);
  }

  mEmcaPh.SmmBase->InSmm(mEmcaPh.SmmBase, &InSmm);

  if(mEmcaPh.IioUds == NULL) {
    //
    // Locate the IIO Protocol Interface
    //
    Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid,NULL, &iioUdsProtocol);
    ASSERT_EFI_ERROR (Status);
    mEmcaPh.IioUds = iioUdsProtocol->IioUdsPtr;

  }

  if(InSmm) {

    if(mEmcaPh.Smst == NULL) {
      Status = mEmcaPh.SmmBase->GetSmstLocation(
                                  mEmcaPh.SmmBase,
                                  &mEmcaPh.Smst
                                );
      ASSERT_EFI_ERROR(Status);

    }

    if(mEmcaPh.CpuCsrAccess == NULL) {
      Status = mEmcaPh.Smst->SmmLocateProtocol(
                               &gEfiCpuCsrAccessGuid,
                               NULL,
                               &mEmcaPh.CpuCsrAccess
                             );
      ASSERT_EFI_ERROR (Status);

      Status = mEmcaPh.Smst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess1);
      ASSERT_EFI_ERROR (Status);
    }
  }

// APTIOV_SERVER_OVERRIDE_RC_START
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (UEFI_MEM_ERROR_RECORD), &gMemoryErrorRecord);
  ASSERT_EFI_ERROR (Status);
  ZeroMem (gMemoryErrorRecord, sizeof (UEFI_MEM_ERROR_RECORD));
// APTIOV_SERVER_OVERRIDE_RC_END

  //
  // Memory RAS protocol
  //
  Status = gSmst->SmmLocateProtocol (&gEfiMemRasProtocolGuid, NULL, &mMemRasProc);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiErrorHandlingProtocolGuid, NULL, &mErrorHandlingProtocol1);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (&gEfiPlatformRasPolicyProtocolGuid, NULL, &mRasPolicy);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiQuiesceProtocolGuid, NULL, &mQuiesceSupport);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, &mSmmVariable);
  ASSERT_EFI_ERROR (Status);

  mRasSetup = mRasPolicy->SystemRasSetup;
  mRasTopology = mRasPolicy->EfiRasSystemTopology;
  mRasCapability = mRasPolicy->SystemRasCapability;

  Status = gSmst->SmmLocateProtocol (&gEfiCrystalRidgeSmmGuid, NULL, &mCrystalRidge);
  ASSERT_EFI_ERROR (Status);

  if (ProcMemIsSupported()) {

    ProcMemEnableReporting();
   // APTIOV_SERVER_OVERRIDE_RC_START : OEM Hook provided for Enabling/Disabling Memory Error Reporting chipset registers
    OemMemErrEnableDisableReporting(mRasPolicy);
   // APTIOV_SERVER_OVERRIDE_RC_END : OEM Hook provided for Enabling/Disabling Memory Error Reporting chipset registers

    //
    // Create the REPORTER_PROTOCOL packet
    //
    mReporter.IsSupported = ProcMemIsSupported;
    mReporter.EnableReporting = ProcMemEnableReporting;
    mReporter.DisableReporting = ProcMemDisableReporting;
    mReporter.CheckStatus = ProcMemCheckStatus;
    mReporter.ClearStatus = ProcMemClearStatus;
    mReporter.DetectAndHandle = ProcMemDetectAndHandle;

    // RASCM_TODO - publish the ERROR_REPORTER protocol, so that O*L code can use it

    // Register the ProcMem Error Handler
    mErrorHandlingProtocol1->RegisterHandler (&mReporter, HighPriority);
  }


 return EFI_SUCCESS;
}


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
