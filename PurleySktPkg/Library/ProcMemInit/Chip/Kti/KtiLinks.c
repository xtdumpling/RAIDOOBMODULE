/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      KTI Reference Code                                                *
// *                                                                        *
// *      ESS - Enterprise Silicon Software                                 *
// *                                                                        *
// *      Copyright (c) 2004 - 2016 Intel Corp.                             *
// *                                                                        *
// *      This program has been developed by Intel Corporation.             *
// *      Licensee has Intel's permission to incorporate this source code   *
// *      into their product, royalty free.  This source code may NOT be    *
// *      redistributed to anyone without Intel's written permission.       *
// *                                                                        *
// *      Intel specifically disclaims all warranties, express or           *
// *      implied, and all liability, including consequential and other     *
// *      indirect damages, for the use of this code, including liability   *
// *      for infringement of any proprietary rights, and including the     *
// *      warranties of merchantability and fitness for a particular        *
// *      purpose.  Intel does not assume any responsibility for any        *
// *      errors which may appear in this code nor any responsibility to    *
// *      update it.                                                        *
// *                                                                        *
// **************************************************************************
// **************************************************************************
// *                                                                        *
// *  PURPOSE:                                                              *
// *                                                                        *
// *      This file contains PHY/LL portion of KTIRC.                       *
// *                                                                        *
// **************************************************************************
**/

#include "DataTypes.h"
#include "PlatformHost.h"
#include "SysHost.h"
#include "SysFunc.h"
#include "KtiLib.h"
#include "KtiMisc.h"
#include "KtiCoh.h"
#include "RcRegs.h"
#include "KtiSetupDefinitions.h"
#include "SysHostChip.h"


KTI_STATUS
IssuePhyResetOnCpuLink(
  struct sysHost      *host,
  UINT8               Cpu,
  UINT8               Link,
  UINT8               ResetType
  );

STATIC
KTI_STATUS
ProgramLateAction (
  struct sysHost      *host,
  UINT32              Group,               //0: KTI
  UINT32              Value
  );


KTI_STATUS
ProgramPhyLayer (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  UINT8               Group
  );

STATIC
KTI_STATUS
DisableLinkForceToLowPower(
  struct sysHost      *host,
  UINT8               Socket,
  UINT8               Link
  );

STATIC
KTI_STATUS
DisableForceToLowPowerAllUnusedLinks (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
ConfigSocketLinkEparams (
  struct sysHost *host,
  UINT8          Socket,
  UINT8          Link,
  VOID           *LinkSpeedParameter
  );

KTI_STATUS
ProgramLinkLayerControlRegisters (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    Index,
  UINT8                    LinkIndex,
  UINT8                    Group
  );

KTI_STATUS
ProgramLinkLayerOnM3Kti (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    Index,
  UINT8                    M3Kti,
  UINT32                   VnaCredit
  );

STATIC
KTI_STATUS
ProgramLinkLayer (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    Group
  );

KTI_STATUS
GatherCommonKtiCapabilities (
  struct sysHost           *host,
  KTILCP_KTI_LLPMON_STRUCT *KtiLcp,
  KTIREUT_PH_CPR_KTI_REUT_STRUCT *KtiReutPhCpr
  );

KTI_STATUS
SelectSupportedKtiLinkSpeed (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

UINT8
GetMaxSupportedLinkSpeed (
  struct sysHost      *host
  );

KTI_STATUS
CheckLinkSpeedSupported (
   struct sysHost      *host,
   UINT8               RequestedLinkSpeed
  );

KTI_STATUS
NormalizeLinkOptions (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
SendMailBoxCmdSetKtiFreq (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
ProgramLateActionOnCpuLink (
   struct sysHost           *host,
   UINT8                    Cpu,
   UINT8                    Link,
   UINT32                   Value
  );

KTI_STATUS
ProgramPhyLayerControlRegisters (
  struct sysHost         *host,
  KTI_SOCKET_DATA        *SocketData,
  UINT8                  Index,
  UINT8                  LinkIndex,
  UINT8                  Group
  );

KTI_STATUS
ProgramEparams (
  struct sysHost         *host,
  UINT8                  Index,
  UINT8                  LinkIndex,
  UINT8                  Group
  );

KTI_STATUS
ClearOrSetKtiDfxLckForFullSpeedTransition (
  struct sysHost *host,
  UINT8          Value
  );

UINT8 SpeedTable[MAX_KTI_LINK_SPEED] = {96, 104}; // 0: 9.6G, 1:10.4G



/**

  The RxDCC value should be overwritten to a 'safe' value to ensure reliable startup of the RxDCC clock when transitioning from SLOW to FAST.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

**/
VOID
SlowModeRxDccOverride (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  CC_DCC_OVRD_KTI_LLDFX_STRUCT       DccOvrdReg;
  CC_DFX_MON0_KTI_LLDFX_STRUCT       DfxMon0Reg;
  UINT8                              Socket, Port, LinkInst;
  IOVB_AFE_CTL0_0_0_KTI_PHY_IOVB_STRUCT AfeCtl0Reg;

  if (host->var.kti.KtiCurrentLinkSpeedMode == KTI_LINK_SLOW_SPEED_MODE){
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if ((KtiInternalGlobal->CpuList & (1 << Socket)) != 0) {
        for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
          if (SocketData->Cpu[Socket].LinkData[Port].Valid == TRUE) {
            //Port 0 and Port 1 share KTI_LLDFX_REG registers
            LinkInst = (UINT8)((Port/2)*2);
            DccOvrdReg.Data = KtiReadPciCfg (host, Socket, LinkInst, CC_DCC_OVRD_KTI_LLDFX_REG);
            DfxMon0Reg.Data = KtiReadPciCfg (host, Socket, LinkInst, CC_DFX_MON0_KTI_LLDFX_REG);
            
            //clear dfx_bdl_sel
            AfeCtl0Reg.Data = KtiReadIar (host, Socket, Port, 0, IOVB_AFE_CTL0_0_0_KTI_PHY_IOVB_REG);
            AfeCtl0Reg.Bits.dfx_bdl_sel  = 0;
            KtiWriteIar (host, Socket, Port, KTILIB_IAR_BCAST, IOVB_AFE_CTL0_0_0_KTI_PHY_IOVB_REG, AfeCtl0Reg.Data);

            //Set rxdcc_extval to 46/2, half of median value from HVM for fast mode
            DccOvrdReg.Bits.rxdcc_extval =23;
            KtiWritePciCfg (host, Socket, LinkInst, CC_DCC_OVRD_KTI_LLDFX_REG, DccOvrdReg.Data);
    
            //Select all bundles for parallel override
            AfeCtl0Reg.Data = KtiReadIar (host, Socket, Port, 0, IOVB_AFE_CTL0_0_0_KTI_PHY_IOVB_REG);
            AfeCtl0Reg.Bits.dfx_bdl_sel  = 1;
            KtiWriteIar (host, Socket, Port, KTILIB_IAR_BCAST, IOVB_AFE_CTL0_0_0_KTI_PHY_IOVB_REG, AfeCtl0Reg.Data);

            //select odd lanes
            DfxMon0Reg.Bits.dfx_lane_sel = 1;
            KtiWritePciCfg (host, Socket, LinkInst, CC_DFX_MON0_KTI_LLDFX_REG, DfxMon0Reg.Data);

            //enable override
            DccOvrdReg.Bits.rxdcc_extovrden =1;
            KtiWritePciCfg (host, Socket, LinkInst, CC_DCC_OVRD_KTI_LLDFX_REG, DccOvrdReg.Data);
    
            //stall 1 microsecond
            KtiFixedDelay(host, 1);

            //Select even lanes
            DfxMon0Reg.Bits.dfx_lane_sel = 0;
            KtiWritePciCfg (host, Socket, LinkInst, CC_DFX_MON0_KTI_LLDFX_REG, DfxMon0Reg.Data);
    
            //stall 1 microsecond
            KtiFixedDelay(host, 1);

            //Remove override enable
            DccOvrdReg.Bits.rxdcc_extovrden = 0;
            KtiWritePciCfg (host, Socket, LinkInst, CC_DCC_OVRD_KTI_LLDFX_REG, DccOvrdReg.Data);

            //Deselect all bundles
            AfeCtl0Reg.Bits.dfx_bdl_sel  = 0;
            KtiWriteIar (host, Socket, Port, KTILIB_IAR_BCAST, IOVB_AFE_CTL0_0_0_KTI_PHY_IOVB_REG, AfeCtl0Reg.Data);
          }
        }
      }
    }
  }
}



/**

   Transition the KTI links to full speed operation.

   To transition the link to full speed, for each link we need to:
      1. Validate the phy/link layer input parameters; force safe values if needed
      2. Ensure that no partial loading of parameters by setting LA Disable
      3. Program the Link & Phy layer parameters
      4. Clear LA Disable

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.

**/
KTI_STATUS
KtiTransitionFullSpeed (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  )
{

  KtiCheckPoint (0xFF, 0xFF, 0xFF, STS_FULL_SPEED_TRANSITION, MINOR_STS_FULL_SPEED_START, host);

  //
  // Transition to FULL speed?
  //
  if ((host->setup.kti.KtiLinkSpeedMode == KTI_LINK_FULL_SPEED_MODE) && ((KtiInternalGlobal->ProgramPhyLink == TRUE) || (host->setup.kti.KtiLbEn))) {
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  Clearing KTI DFX Locks\n"));
    ClearOrSetKtiDfxLckForFullSpeedTransition (host, 0);

    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  Setting Late Action Disable bit for each socket/link\n"));
    ProgramLateAction (host, KTI_GROUP, 1);

    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  ******       Phy init      - START ******\n"));
    KtiCheckPoint (0xFF, 0xFF, 0xFF, STS_FULL_SPEED_TRANSITION, MINOR_STS_PHY_LAYER_SETTING, host);
    // s5370513: RxDCC override in slow mode
    SlowModeRxDccOverride(host, SocketData, KtiInternalGlobal);
    ProgramPhyLayer (host, SocketData, KTI_GROUP);
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  ******       Phy init      - END   ******"));

    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  ******       LL init       - START ******\n"));
    KtiCheckPoint (0xFF, 0xFF, 0xFF, STS_FULL_SPEED_TRANSITION, MINOR_STS_LINK_LAYER_SETTING, host);
    ProgramLinkLayer (host, KtiInternalGlobal, KTI_GROUP);
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  ******       LL init       - END   ******\n"));

    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n  Clearing Late Action Disable bit for each socket/link\n"));
    ProgramLateAction (host, KTI_GROUP, 0);

    host->var.common.resetRequired |= POST_RESET_WARM;

  } else {

    //
    // Indicate not transitioning to FS mode and why.
    //
    KtiDebugPrintInfo0 ((host, KTI_DEBUG_INFO0, "\n  %s",
       (KtiInternalGlobal->TotCpu == 1) ? "Single Socket, no KTI Links to transition" : "Full speed transition not requested"));

    //
    // Force unused ports to low power mode
    //
    DisableForceToLowPowerAllUnusedLinks (host, SocketData, KtiInternalGlobal);
  }

  //
  // Execute this for any topology: multi-socket or 1S-hotplug-enabled.
  //
  SendMailBoxCmdSetKtiFreq (host, SocketData, KtiInternalGlobal);

  return KTI_SUCCESS;
}

/**

  KTI Physical Layer programming performed after warm reset.

  @param host              - Pointer to the system host (root) structure
  @param SocketData        - Socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.

  @retval KTI_SUCCESS - on successful procedure execution

**/
KTI_STATUS
KtiPhyLinkAfterWarmReset (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  )
{
  KTIPHYPWRCTRL_KTI_REUT_STRUCT        KtiPhyPwrCtrl;
  DFX_LCK_CTL_CFG_KTI_CIOPHYDFX_STRUCT DfxLckCtlCfg;
  UINT8                                Index, LinkIndex;

  //
  // For unconnected links, need to set fake_L1.
  // No need to set it on firmware-disabled links, since it is done before WR.
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
      for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
        if ((SocketData->Cpu[Index].LinkData[LinkIndex].Valid == FALSE)  && (host->setup.kti.KtiLbEn == KTI_DISABLE)) {
          KtiPhyPwrCtrl.Data = KtiReadPciCfg (host, Index, LinkIndex, KTIPHYPWRCTRL_KTI_REUT_REG);
          if (KtiPhyPwrCtrl.Bits.fake_l1 != 1) {
            KtiPhyPwrCtrl.Bits.fake_l1 = 1;
            KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n  Socket %u Port %u : KTIPHYPWRCTRL write 0x%x\n", Index, LinkIndex, KtiPhyPwrCtrl.Data));
            KtiWritePciCfg (host, Index, LinkIndex, KTIPHYPWRCTRL_KTI_REUT_REG, KtiPhyPwrCtrl.Data);
          }
        }

        //
        // Lock KTI DFX after warm reset or unlock for EV first level debug
        //
        if ((SocketData->Cpu[Index].LinkData[LinkIndex].Valid == TRUE) || (host->setup.kti.KtiLbEn == KTI_ENABLE)) {
          DfxLckCtlCfg.Data = KtiReadPciCfg(host, Index, LinkIndex, DFX_LCK_CTL_CFG_KTI_CIOPHYDFX_REG);
          if(host->var.common.EVMode == KTI_ENABLE) {
            DfxLckCtlCfg.Bits.reutenglck = 0;
            DfxLckCtlCfg.Bits.privdbg_phy_l0_sync_lck = 0;
          } else {
            DfxLckCtlCfg.Bits.reutenglck = 1;
            DfxLckCtlCfg.Bits.privdbg_phy_l0_sync_lck = 1;
          }
          KtiWritePciCfg(host, Index, LinkIndex, DFX_LCK_CTL_CFG_KTI_CIOPHYDFX_REG, DfxLckCtlCfg.Data);
        }
      }
    }
  }

  return KTI_SUCCESS;
}

/**

    Send a pCode mailbox command to set KTI link frequency.

    @param host       - pointer to the system host root structure
    @param SocketData - pointer to the socket data structure

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
SendMailBoxCmdSetKtiFreq (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  )
{

  UINT8  Index, LinkIndex;
  UINT32 LinkSpeed = 0;

  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
      for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
        if (SocketData->Cpu[Index].LinkData[LinkIndex].Valid == TRUE || host->var.kti.OutKtiCpuSktHotPlugEn == TRUE){
          switch (KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[LinkIndex].KtiLinkSpeed) {
            case (SPEED_REC_96GT):
              LinkSpeed = 12;
              break;
            case (SPEED_REC_104GT):
              LinkSpeed = 13;
              break;
            default:
              LinkSpeed = 12;
              break;
          }
          if (LinkIndex < SI_MAX_KTI_PORTS - 1) {
            //
            // Send common command/link speed for ports 0/1
            //
            SendMailBoxCmdToPcode (host, Index, MAILBOX_BIOS_CMD_SET_KTI_FREQ, LinkSpeed);
            LinkIndex = 1;
          } else {
            //
            // Send separate command/link speed for port 2
            //
            SendMailBoxCmdToPcode (host, Index, MAILBOX_BIOS_CMD_SET_KTI_FREQ | (0x2 << 8), LinkSpeed);
          }
        } // valid link?
      } // for LinkIndex loop
    } // valid socket?
  } // for socket

  return KTI_SUCCESS;
}

/**

    Program late action registers.

    @param host  - pointer to the system host root structure
    @param Group - 0: KTI
    @param Value - Value to program

    @retval KTI_SUCCESS - on successful completion

**/
STATIC
KTI_STATUS
ProgramLateAction (
  struct sysHost      *host,
  UINT32              Group,
  UINT32              Value
  )
{
  UINT8 Index;
  UINT8 LinkIndex;

  if(Group == KTI_GROUP) {               //for KTI links
    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if ((host->var.common.socketPresentBitMap & (BIT0 << Index)) != 0) {

        for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
          //
          // Program lateactionloaddisable bit
          //
          if ((host->var.kti.CpuInfo[Index].LepInfo[LinkIndex].Valid == TRUE) || (host->setup.kti.KtiLbEn)) {
            ProgramLateActionOnCpuLink(host, Index, LinkIndex, Value);
          }
        }
      }
    }
  }
  return KTI_SUCCESS;
}

/**

   Programs PHY layer registers

   @param host  - Pointer to the system host (root) structure
   @param SocketData - Socket specific data structure

**/
KTI_STATUS
ProgramPhyLayer (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  UINT8               Group
  )
{
  UINT8 Index;
  UINT8 LinkIndex;

  if( Group == KTI_GROUP) {
    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if ((host->var.common.socketPresentBitMap & (BIT0 << Index)) != 0) {
        for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
          if ((host->var.kti.CpuInfo[Index].LepInfo[LinkIndex].Valid == TRUE) || (host->setup.kti.KtiLbEn)) {
            //
            // Program Electrical Parameters (board-layout-dependent)
            //
            KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  ****** S%dp%d Program Eparams - START ******\n", Index, LinkIndex));
            ProgramEparams (host, Index, LinkIndex, KTI_GROUP);
            KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  ****** S%dp%d Program Eparams - END ******\n", Index, LinkIndex));
            //
            // Program CTR1 and LDC registers
            //
            KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  ****** S%dp%d Program Phy Control Regs - START ******\n", Index, LinkIndex));
            ProgramPhyLayerControlRegisters (host, SocketData, Index, LinkIndex, KTI_GROUP);
            KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  ****** S%dp%d Program Phy Control Regs - END ******\n", Index, LinkIndex));
            //
            // Program the UniPhy recipe (independent of board layout)
            //
            KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  ****** S%dp%d Program UniPhy Recipe - START ******\n", Index, LinkIndex));
            KtiEvAutoRecipe (host, Index, LinkIndex);
            KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  ****** S%dp%d Program UniPhy Recipe - END ******\n", Index, LinkIndex));
          } else {
            //
            // Put unused links into low power/disable state
            //
            KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  ****** S%dp%d Disable and Force Link to Low Power - START ******\n", Index, LinkIndex));
            DisableLinkForceToLowPower (host, Index, LinkIndex);
            KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  ****** S%dp%d Disable and Force Link to Low Power - END ******\n", Index, LinkIndex));
          }

        } // For LinkIndex (2nd instance - Valid links only)
      } // If Socket valid
    } // For Socket
  }
  return KTI_SUCCESS;
}



/**

  Disables the KTI port.
  Accounts for two types of disabled KTI ports: unconnected ports and
  FW disabled ports. To determine the type of the disabled port, we
  check the PcSts.ll_status value. For connected, but FW-disabled links,
  we will see the LINK_SM_INIT_DONE value there.

  @param host  - Pointer to the system host (root) structure
  @param Socket - SocId we are looking up
  @param Link - Link# on socket (0 or 1)

**/
STATIC
KTI_STATUS
DisableLinkForceToLowPower(
  struct sysHost      *host,
  UINT8               Socket,
  UINT8               Link
  )
{
  KTIREUT_PH_CTR1_KTI_REUT_STRUCT KtiReutPhCtr1;
  KTIPHYPWRCTRL_KTI_REUT_STRUCT   KtiPhyPwrCtrl;
  KTIPCSTS_KTI_LLPMON_STRUCT      PcSts;

  PcSts.Data = KtiReadPciCfg (host, Socket, Link, KTIPCSTS_KTI_LLPMON_REG);
  KtiReutPhCtr1.Data = KtiReadPciCfg (host, Socket, Link,  KTIREUT_PH_CTR1_KTI_REUT_REG);

  if (PcSts.Bits.ll_status == LINK_SM_INIT_DONE) {
    //
    // Firmware-disabled KTI link since it trained in slow mode
    //
    KtiReutPhCtr1.Bits.c_init_begin = 0;
    KtiReutPhCtr1.Bits.cp_reset = 1;
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n  Socket %u Port %u : KTIREUT_PH_CTR1 write 0x%x\n", Socket, Link, KtiReutPhCtr1.Data));
    KtiWritePciCfg (host, Socket, Link,  KTIREUT_PH_CTR1_KTI_REUT_REG, KtiReutPhCtr1.Data);
      //
      // Tell the link to report L1 state to the PCU
      //
      KtiPhyPwrCtrl.Data = KtiReadPciCfg (host, Socket, Link, KTIPHYPWRCTRL_KTI_REUT_REG);
      KtiPhyPwrCtrl.Bits.fake_l1 = 1;
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n  Socket %u Port %u : KTIPHYPWRCTRL write 0x%x\n", Socket, Link, KtiPhyPwrCtrl.Data));
      KtiWritePciCfg (host, Socket, Link, KTIPHYPWRCTRL_KTI_REUT_REG, KtiPhyPwrCtrl.Data);
  } else {
    //
    // Unconnected link
    //
    KtiReutPhCtr1.Bits.c_init_begin = 0;
    KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n  Socket %u Port %u : KTIREUT_PH_CTR1 write 0x%x\n", Socket, Link, KtiReutPhCtr1.Data));
    KtiWritePciCfg (host, Socket, Link,  KTIREUT_PH_CTR1_KTI_REUT_REG, KtiReutPhCtr1.Data);

    //
    // If hot plug is enabled, mask FW_AGENT through pcode mailbox command
    //
    if (host->var.kti.OutKtiCpuSktHotPlugEn == TRUE) {
       SendMailBoxCmdToPcode (host, Socket, MAILBOX_BIOS_CMD_MASK_FW_AGENT |(Link << 8), 0);
    }
  }

  return KTI_SUCCESS;
}



/**

  Disables the link forces it to low power

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.

**/
STATIC
KTI_STATUS
DisableForceToLowPowerAllUnusedLinks(
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  )
{

  UINT8 Index;
  UINT8 LinkIndex;

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Force unused links to disabled/low power state."));
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((SocketData->Cpu[Index].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
      for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
        if ((SocketData->Cpu[Index].LinkData[LinkIndex].Valid == FALSE)  && !(host->setup.kti.KtiLbEn)) {
          DisableLinkForceToLowPower(host, Index, LinkIndex);
        }
      }
    }
  }

  return KTI_SUCCESS;

}

/**

   Programs the values from the EPARAMS entry for specified link

   @param host  - Pointer to the system host (root) structure
   @param SocketData - Socket specific data structure
   @param Socket - SocId we are looking up
   @param Link - Link# on socket
   @param *LinkSpeedParameter - ptr to eparam data

   @retval KTI_SUCCESS - on successful completion
**/
KTI_STATUS
ConfigSocketLinkEparams (
  struct sysHost *host,
  UINT8          Socket,
  UINT8          Link,
  VOID           *LinkSpeedParameter
  )
{
  UINT8 Data8;
  UINT8 Level, Lane;
  UINT8 TxeqLvlReg[4]   = {IOVB_TXEQ_LVL0_0_0_KTI_PHY_IOVB_REG,
                           IOVB_TXEQ_LVL1_0_0_KTI_PHY_IOVB_REG,
                           IOVB_TXEQ_LVL2_0_0_KTI_PHY_IOVB_REG,
                           IOVB_TXEQ_LVL3_0_0_KTI_PHY_IOVB_REG};

  PER_LANE_EPARAM_LINK_INFO     *Ptr = NULL;

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Socket %u Port %u : EPARAM entry programmed\n", Socket, Link));
  Ptr = (PER_LANE_EPARAM_LINK_INFO *) LinkSpeedParameter;
  if (Ptr->AllLanesUseSameTxeq == ALL_LANES_TXEQ_ENABLED) {
    //
    // All lanes use same txeq value, use broadcast as unit ID
    //
    for (Level = 0; Level < 4; Level++) {
      Data8 = (UINT8) ((Ptr->TXEQL[0] >> (Level *8)) & 0x3f);
      KtiWriteIar (host, Socket, Link, KTILIB_IAR_BCAST, TxeqLvlReg[Level], Data8);
    }
    //
    // Only ADAPTIVE_CTLE is supported, assert if Eparam Table indicates otherwise
    //
    KTI_ASSERT ((Ptr->CTLEPEAK[0] == ADAPTIVE_CTLE), ERR_FULL_SPEED_TRANSITION, MINOR_ERR_KTI_ELEC_PARAM_INVALID);

  } else {

    //
    // Each lanes has different txeq value, use lane # as unit ID
    //
    for (Lane= 0; Lane < MAX_KTI_LANES; Lane++) {
      for (Level = 0; Level < 4; Level++) {
        Data8 = (UINT8) ((Ptr->TXEQL[Lane] >> (Level *8)) & 0x3f);
        KtiWriteIar (host, Socket, Link, Lane, TxeqLvlReg[Level], Data8);
      }
    }
    //
    // Check ADAPTIVE_CTLE per lane (only ADAPTIVE_CTLE is supported)
    //
    for (Lane= 0; Lane < MAX_KTI_LANES; Lane++) {
      Data8 = ((UINT8) (Ptr->CTLEPEAK[Lane/4] >> ((Lane%4) * 8)) & 0xFF);
      KTI_ASSERT ((Data8 == ADAPTIVE_CTLE), ERR_FULL_SPEED_TRANSITION, MINOR_ERR_KTI_ELEC_PARAM_INVALID);
    }
  }

  return KTI_SUCCESS;
}

/**

    Misc Link layer configuration

    @param host              - pointer to the system host root structure
    @param KtiInternalGlobal - KTIRC internal variables.
    @param Index             - Socket ID
    @param LinkIndex         - Link number
    @param Group             - KTI

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramLinkLayerControlRegisters (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    Index,
  UINT8                    LinkIndex,
  UINT8                    Group
  )
{
  KTILCCL_KTI_LLPMON_STRUCT    KtiLccl;
  KTILCL_KTI_LLPMON_STRUCT     KtiLcl;

  //
  // Setup CRC mode based on the output variable and check the per-link DFX override.
  //
  KtiLcl.Data = KtiReadPciCfg (host, Index, LinkIndex, KTILCL_KTI_LLPMON_REG);
  KtiLcl.Bits.next_crc_mode = host->var.kti.OutKtiCrcMode;
  if (host->setup.kti.DfxCpuCfg[Index].Link[LinkIndex].DfxCrcMode < CRC_MODE_AUTO) {
    KtiLcl.Bits.next_crc_mode = host->setup.kti.DfxCpuCfg[Index].Link[LinkIndex].DfxCrcMode;
  }
  KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n  Socket %u Port %u : KTILCL write 0x%x\n", Index, LinkIndex, KtiLcl.Data));
  KtiWritePciCfg (host, Index, LinkIndex, KTILCL_KTI_LLPMON_REG, KtiLcl.Data);

  //
  // Setup LinkCreditControl
  //
  KtiLccl.Data = KtiReadPciCfg (host, Index, LinkIndex, KTILCCL_KTI_LLPMON_REG);
  switch (KtiInternalGlobal->PhyLinkPerPortSetting[Index].Link[LinkIndex].KtiLinkVnaOverride) {
    case KTI_LL_VNA_SETUP_MAX:
      //
      // Program normal values
      //
      if (host->var.kti.OutVn1En == TRUE) {
        KtiLccl.Bits.vna_credits = KTI_LL_VNA_WITH_VN1_SKX;
      } else {
        KtiLccl.Bits.vna_credits = KTI_LL_VNA_NORMAL_SKX;
      }
      break;
    case KTI_LL_VNA_SETUP_MIN:
      //
      // Program architectural minimum
      //
      KtiLccl.Bits.vna_credits = KTI_LL_VNA_MIN;
      break;
    default:
      //
      // Check if user-selected value is valid, if so program it
      //
      if ((KtiInternalGlobal->PhyLinkPerPortSetting[Index].Link[LinkIndex].KtiLinkVnaOverride > KTI_LL_VNA_WITH_VN1_SKX) &&
          (host->var.kti.OutVn1En == TRUE)) {
        KtiLccl.Bits.vna_credits = KTI_LL_VNA_WITH_VN1_SKX;
      } else if ((KtiInternalGlobal->PhyLinkPerPortSetting[Index].Link[LinkIndex].KtiLinkVnaOverride > KTI_LL_VNA_NORMAL_SKX) &&
                 (host->var.kti.OutVn1En == FALSE)) {
        KtiLccl.Bits.vna_credits = KTI_LL_VNA_NORMAL_SKX;
      } else {
        KtiLccl.Bits.vna_credits = KtiInternalGlobal->PhyLinkPerPortSetting[Index].Link[LinkIndex].KtiLinkVnaOverride;
      }
  }
  if (host->var.kti.OutVn1En == TRUE) {
    KtiLccl.Bits.vn1_enable = 1;
  }
  
  //
  // For the socket which has its UPI 2 connected with the FPGA
  // Program its VN0, only need Wb, Rsp & Req channels of VN0 for UPI port 2 which connects to the FPGA
  // Disable vn1, and maximize VNA credits.
  if (host->var.kti.OutKtiFpgaEnable[Index] == KTI_ENABLE && LinkIndex == XEON_FPGA_PORT) {
    KtiLccl.Data = KtiReadPciCfg (host, Index, LinkIndex, KTILCCL_KTI_LLPMON_REG);
    KtiLccl.Bits.vna_credits =  KTI_LL_VNA_FGPA_SKX;
    KtiLccl.Bits.vn0_snp_credits = 0;
    KtiLccl.Bits.vn0_ncb_credits = 0;
    KtiLccl.Bits.vn0_ncs_credits = 0;
    KtiLccl.Bits.vn1_enable = 0;    
  }
  
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Socket %u Port %u : KTILCCL write 0x%x\n", Index, LinkIndex, KtiLccl.Data));
  KtiWritePciCfg (host, Index, LinkIndex, KTILCCL_KTI_LLPMON_REG, KtiLccl.Data);

  return KTI_SUCCESS;
}

/**

    Program  M3INGDBGREMCRE_0/1_KTI0/1_M3KTI_MAIN_REG, the value need to match with LCCL setting on peer port of peer socket

    @param host      - pointer to the system host root structure
    @param SocId     - Socket ID
    @param M3Kti     - M3Kti number
    @param VnaCredit - Vna Credit

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramLinkLayerOnM3Kti (
  struct sysHost           *host,
  KTI_HOST_INTERNAL_GLOBAL *KtiInternalGlobal,
  UINT8                    SocId,
  UINT8                    M3Kti,
  UINT32                   VnaCredit
  )
{
  UINT32 Data32;
  M3INGERRMASK1_M3KTI_MAIN_STRUCT      M3IngerrMask1; 
  M3INGERRMASK0_M3KTI_MAIN_STRUCT      M3IngerrMask0;

  //
  // s5372538: Mask the remotevnacreditoverflow during programming vna/vn0 w/ VN1.
  //           remotevnacreditoverflowmask will be cleared after warm reset
  //
  if ((host->var.kti.OutVn1En) && (host->var.kti.SysConfig >= SYS_CONFIG_4S) &&
      (host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping >= B0_REV_SKX)) {
    M3IngerrMask0.Data = KtiReadPciCfg (host, SocId, M3Kti, M3INGERRMASK0_M3KTI_MAIN_REG);
    M3IngerrMask0.Bits.remotevnacreditoverflowmask = 1;
    KtiWritePciCfg (host, SocId, M3Kti, M3INGERRMASK0_M3KTI_MAIN_REG, M3IngerrMask0.Data);
 
    M3IngerrMask1.Data = KtiReadPciCfg (host, SocId, M3Kti, M3INGERRMASK1_M3KTI_MAIN_REG);
    M3IngerrMask1.Bits.remotevnacreditoverflowmask = 1;
    KtiWritePciCfg (host, SocId, M3Kti, M3INGERRMASK1_M3KTI_MAIN_REG, M3IngerrMask1.Data);
  }

  Data32 = 0x99999900 | (1 << 7) | VnaCredit;                                    //program KTI0 VNA & VN0 value and set Load bit
  KtiWritePciCfg (host, SocId, M3Kti, M3INGDBGREMCRE0KTI0_M3KTI_MAIN_REG, Data32);

  Data32 = 0x11111100 | VnaCredit;                                               //clear Load bit
  KtiWritePciCfg (host, SocId, M3Kti, M3INGDBGREMCRE0KTI0_M3KTI_MAIN_REG, Data32);

  Data32 = 0x99999900;                                                             //program KTI0 VN1 value and set Load bit
  KtiWritePciCfg (host, SocId, M3Kti, M3INGDBGREMCRE1KTI0_M3KTI_MAIN_REG, Data32);

  Data32 = 0x11111100;                                                             //clear Load bit
  KtiWritePciCfg (host, SocId, M3Kti, M3INGDBGREMCRE1KTI0_M3KTI_MAIN_REG, Data32);

  Data32 = 0x99999900 | (1 << 7) | VnaCredit;                                    //program KTI1 VNA & VN0 value and set Load bit
  KtiWritePciCfg (host, SocId, M3Kti, M3INGDBGREMCRE0KTI1_M3KTI_MAIN_REG, Data32);

  Data32 = 0x11111100 | VnaCredit;                                               //clear Load bit
  KtiWritePciCfg (host, SocId, M3Kti, M3INGDBGREMCRE0KTI1_M3KTI_MAIN_REG, Data32);

  Data32 = 0x99999900;                                                             //program KTI1 VN1 value and set Load bit
  KtiWritePciCfg (host, SocId, M3Kti, M3INGDBGREMCRE1KTI1_M3KTI_MAIN_REG, Data32);

  Data32 = 0x11111100;                                                             //clear Load bit
  KtiWritePciCfg (host, SocId, M3Kti, M3INGDBGREMCRE1KTI1_M3KTI_MAIN_REG, Data32);

  return KTI_SUCCESS;
}

/**

   Program Link layer registers

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Socket specific data structure

**/
STATIC
KTI_STATUS
ProgramLinkLayer (
  struct sysHost            *host,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal,
  UINT8                     Group
  )
{

  UINT8 Index, LinkIndex, Ctr;
  UINT32 VnaCredit;

  if (host->var.kti.OutVn1En == TRUE) {
    VnaCredit = KTI_LL_VNA_WITH_VN1_SKX;
  } else {
    VnaCredit = KTI_LL_VNA_NORMAL_SKX;
  }

  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((host->var.common.socketPresentBitMap & (BIT0 << Index)) != 0) {
      for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
        if (host->var.kti.CpuInfo[Index].LepInfo[LinkIndex].Valid == TRUE) {
          ProgramLinkLayerControlRegisters (host, KtiInternalGlobal, Index, LinkIndex, Group);
        }  // link valid
      } // for link

      //
      // Program  M3INGDBGREMCRE_0/1_KTI0/1_M3KTI_MAIN_REG, the value need to match with LCCL setting on peer port of peer socket
      //
      for (Ctr = 0; Ctr < host->var.kti.CpuInfo[Index].TotM3Kti; Ctr++) {
        ProgramLinkLayerOnM3Kti (host, KtiInternalGlobal, Index, Ctr, VnaCredit);
      }

    } // socket valid
  } // for socket

  return KTI_SUCCESS;
}

/**

  Prime the KTIRC data structure based on input option and system capabilities.
  Also normalize DFX per-link options.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Pointer to socket specific data
  @param KtiInternalGlobal - KTIRC Internal Global data

  @retval 0 - Successful
  @retval Other - failure

**/
KTI_STATUS
PrimeHostStructurePhyLinkLayer (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  )
{
  KTILCP_KTI_LLPMON_STRUCT        KtiLcp;
  KTIREUT_PH_CPR_KTI_REUT_STRUCT  KtiReutPhCpr;
  KTIMISCSTAT_KTI_LLDFX_STRUCT    KtiMiscSts;
  KTILS_KTI_LLPMON_STRUCT         KtiLs;
  KTIREUT_PH_CTR1_KTI_REUT_STRUCT KtiReutPhCtr1;
  UINT8                           Index, LinkIndex, LinkSpeed;
  BOOLEAN                         PerLinkSpeed;

  if (KtiInternalGlobal->ProgramPhyLink == TRUE) {
    //
    // Gather common link and phy layer capabilites of all valid sockets
    //
    GatherCommonKtiCapabilities (host, &KtiLcp, &KtiReutPhCpr);

    if (host->var.kti.SysConfig == SYS_CONFIG_8S) {
      //
      // 5330183: UPI Spec change to add VN1 capability bit to KTILCP for B0+
      //
      if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping >= B0_REV_SKX)) {
        if (KtiLcp.Bits.vn1_supported == 0){
          host->var.kti.OutVn1En = FALSE;
        }
      }
    }

    if (KtiInternalGlobal->CurrentReset == POST_RESET_POWERGOOD) {
      //
      // Cold reset path: prime setup options against capabilities
      //
      if ((KtiLcp.Bits.crc_mode_supported == 0) || (host->var.kti.OutKtiCrcMode == CRC_MODE_16BIT) || (host->var.kti.OutKtiCrcMode == CRC_MODE_AUTO)) {
        //
        // Rolling 32bit is not supported, or user specifically asked for 16bit mode
        //
        host->var.kti.OutKtiCrcMode = CRC_MODE_16BIT;
      } else {
        //
        // Rolling 32bit is supported and user didn't specifically request 16bit mode
        //
        host->var.kti.OutKtiCrcMode = CRC_MODE_ROLLING_32BIT;
      }

      if ((KtiReutPhCpr.Bits.ccl0p == 0) || (host->var.kti.OutKtiLinkL0pEn == KTI_DISABLE)) {
        //
        // L0p is not supported or user specifically asked to disable L0p
        //
        host->var.kti.OutKtiLinkL0pEn = KTI_DISABLE;
      } else {
        //
        // L0p is supported and user didn't specifically request to disable L0p
        //
        host->var.kti.OutKtiLinkL0pEn = KTI_ENABLE;
      }

      if ((KtiReutPhCpr.Bits.ccl1 == 0) || (host->var.kti.OutKtiLinkL1En == KTI_DISABLE)) {
        //
        // L1 is not supported or user specifically asked to disable L1
        //
        host->var.kti.OutKtiLinkL1En = KTI_DISABLE;
      } else {
        //
        // L1 is supported and user didn't specifically request to disable L1
        //
        host->var.kti.OutKtiLinkL1En = KTI_ENABLE;
      }

      //
      // s5332313 - Disable KTI L1 on A0/A1
      //
      if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < A2_REV_SKX)) {
        host->var.kti.OutKtiLinkL1En = KTI_DISABLE;
      }

      if ((KtiReutPhCpr.Bits.ccfailover == 0) || (host->var.kti.OutKtiFailoverEn == KTI_DISABLE)) {
        //
        // Failover is not supported or user specifically asked to disable it
        //
        host->var.kti.OutKtiFailoverEn = KTI_DISABLE;
      } else {
        //
        // Failover is supported and user didn't specifically request to disable it
        //
        host->var.kti.OutKtiFailoverEn = KTI_ENABLE;
      }

      //
      // This updates the OutKtiLinkSpeed and per-link frequency variables
      //
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  ****** Selecting KTI freq. - START ******"));
      SelectSupportedKtiLinkSpeed (host, SocketData, KtiInternalGlobal);
      KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  ****** Selecting KTI freq. - END   ******"));

      //
      // Normalize per-port knobs
      //
      NormalizeLinkOptions (host, SocketData, KtiInternalGlobal);
    } else {
      //
      // After warm reset, check status bits and populate output variables
      //

      //
      // We are in warm reset path; Log a warning if the links are in slow mode && not the 1S-FPGA topology with FPGA enabled case.
      //
      if ((host->var.kti.KtiCurrentLinkSpeedMode == KTI_LINK_SLOW_SPEED_MODE) && 
          (!((host->var.kti.SysConfig == SYS_CONFIG_1S) && (host->var.kti.OutKtiFpgaPresent[KtiInternalGlobal->SbspSoc] == 1) && 
             (host->var.kti.OutKtiFpgaEnable[KtiInternalGlobal->SbspSoc] == 1)))) {
        KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_LINK_SLOW_SPEED_MODE);
        KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n WARNING: System is operating in KTI Link Slow Speed Mode. "));
      } else {
        //
        // Need to select supported speed for hot-adds later
        //
        if (host->var.kti.OutKtiCpuSktHotPlugEn == TRUE) {
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  ****** Selecting KTI freq. - START ******"));
          SelectSupportedKtiLinkSpeed (host, SocketData, KtiInternalGlobal);
          KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n\n  ****** Selecting KTI freq. - END   ******"));
        }

        //
        // Identify the per link speed
        //
        LinkSpeed = 0xFF;
        PerLinkSpeed = FALSE;
        for (Index = 0; Index < MAX_SOCKET; Index++) {
          if (SocketData->Cpu[Index].Valid == TRUE) {
            for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
              if ((SocketData->Cpu[Index].LinkData[LinkIndex].Valid == TRUE) && (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU)) {
                //
                // Display the link's fast-mode speed.
                //
                KtiMiscSts.Data = KtiReadPciCfg (host, Index, (UINT8)((LinkIndex/2)*2), KTIMISCSTAT_KTI_LLDFX_REG); // Only ports 0 & 2 have this register
                host->var.kti.OutPerLinkSpeed[Index][LinkIndex] = (UINT8) (KtiMiscSts.Bits.kti_rate - 4);
                // We cannot access KtiMiscSts on port 2 until we have B0 parts
                if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX) && (LinkIndex == 2)) {
                  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Socket %u KTI Link %u Freq not available, Unable to read KtiMiscSts on Port 2 until next silicon stepping", Index, LinkIndex));
                }
                else {
                  //Link 0, 1, or 2 on B0 or later, print the speed based on KtiMiscSts
                  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Socket %u KTI Link %u Freq is currently %u.", Index, LinkIndex, SpeedTable[KtiMiscSts.Bits.kti_rate - 4]));
                }

                //
                // Keep track of the flag to indicate if two links have different speed (which implies per link speed)
                //
                if (LinkSpeed == 0xFF) {
                  LinkSpeed = host->var.kti.OutPerLinkSpeed[Index][LinkIndex];
                } else if (LinkSpeed != host->var.kti.OutPerLinkSpeed[Index][LinkIndex]){
                  PerLinkSpeed = TRUE;
                }
              }
            }
          }
        }

        //
        // Identify the system wide speed
        //
        host->var.kti.OutKtiLinkSpeed = MAX_KTI_LINK_SPEED;
        if (PerLinkSpeed == TRUE) {
          host->var.kti.OutKtiLinkSpeed = FREQ_PER_LINK;
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n  Per Link speed is used."));
        } else {
          KTI_ASSERT ((LinkSpeed != 0xFF), ERR_SW_CHECK, MINOR_ERR_SW_CHECK_1);
          host->var.kti.OutKtiLinkSpeed = LinkSpeed;
          KtiDebugPrintInfo0((host, KTI_DEBUG_INFO0, "\n  System wide KTI Freq is currently %u.", SpeedTable[host->var.kti.OutKtiLinkSpeed]));
        }
      }

      //
      // Identify the current L0p/L1/CrcMode settings.These settings are system wide, so just read it for only one valid link.
      //
      for (Index = 0; Index < MAX_SOCKET; Index++) {
        if (SocketData->Cpu[Index].Valid == TRUE) {
          for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
            if (SocketData->Cpu[Index].LinkData[LinkIndex].Valid == TRUE){
              KtiLs.Data = KtiReadPciCfg (host, Index, LinkIndex, KTILS_KTI_LLPMON_REG);
              KtiReutPhCtr1.Data = KtiReadPciCfg (host, Index, LinkIndex, KTIREUT_PH_CTR1_KTI_REUT_REG);
              host->var.kti.OutKtiLinkL0pEn = (UINT8) KtiReutPhCtr1.Bits.c_l0p_en;
              host->var.kti.OutKtiLinkL1En = (UINT8) KtiReutPhCtr1.Bits.c_l1_en;
              host->var.kti.OutKtiFailoverEn = (UINT8) KtiReutPhCtr1.Bits.c_failover_en;
              host->var.kti.OutKtiCrcMode = (UINT8) KtiLs.Bits.current_crc_mode;
              Index = MAX_SOCKET;
              break;
            }
          }
        }
      }
    }
  }

  return KTI_SUCCESS;
}

/**
   Routine to determine and select the maximum KTI link speed supported by CPUs. It is determined by
   Min(Max link speed of all populated CPU sockets).  A valid User Selected freq will be returned
   if enabled. If PER_LINK_SPEED is selected, then the links speeds will be normalized for both ends
   of the link. Note that the normalization will be done only for the sockets & links that are valid.
   So that the per link option of the unpopulated/unconnected links will not influence the normalization
   outcome. For O*L operation, this routine should be called after the socket/link valid information is
   collected so that the per link options can be normalized properly for the new topology scenario.

   If adaptation is scheduled to run, the speed selected will be the one decided by adaptation code.
   Also, the PER_LINK_SPEED request will not be fullfilled since the adaptation can support only
   system wide common speed, not per link speed adaptation.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.

   @retval Speed to transition to via host->var.kti.OutKtiLinkSpeed

**/
KTI_STATUS
SelectSupportedKtiLinkSpeed (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  )
{
  UINT8                    LinkSpeed, MaxSpeed;
  UINT8                    Speed0, Speed1, Speed2;
  UINT8                    Index, LinkIndex;
  BOOLEAN                  LinkDependency;
  KTI_LINK_DATA            *pPeerInfo;


  // Get the Max KTI link speed supported by all populated sockets
  MaxSpeed = GetMaxSupportedLinkSpeed (host);

  LinkSpeed = SPEED_REC_96GT;

  if (host->var.kti.OutKtiLinkSpeed == MAX_KTI_LINK_SPEED) {
    //
    // Max speed is requested; so set the speed to maximum speed supported by all populated sockets.
    //
    LinkSpeed = MaxSpeed;
    KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Max supported KTI Speed requested: %u", SpeedTable[LinkSpeed]));
  } else if (host->var.kti.OutKtiLinkSpeed < MAX_KTI_LINK_SPEED) {
    //
    // See if requested freq is supported
    //
    KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Request: KTI Speed of %u", SpeedTable[host->var.kti.OutKtiLinkSpeed]));
    if (CheckLinkSpeedSupported(host, (UINT8) host->var.kti.OutKtiLinkSpeed) == KTI_SUCCESS) {
      LinkSpeed = host->var.kti.OutKtiLinkSpeed;
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Requested speed is valid"));
    } else {
      //
      // Requested speed unsupported, default to 9.6GT
      //
      KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_UNSUPPORTED_LINK_SPEED);
      KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n WARNING:  Requested Link Speed %u is not supported. Defaulting to 9.6GT \n",
                         SpeedTable[host->var.kti.OutKtiLinkSpeed]));
      LinkSpeed = SPEED_REC_96GT;
    }
  } else if (host->var.kti.OutKtiLinkSpeed == FREQ_PER_LINK) {
    //
    // Normalize per link speed option.  Link 0 & 1 share same PLL while Link 2 has separate PLL. As a result link 0 & 1 must
    // operate at same speed while link 2 can be at different speed than link 0/1. Also, both sides of a given link must operate
    // at same speed. So if  link 2 of each populated socket is connected to link 0 or 1 at the peer side, then all links in the
    // system will be forced to operate same speed. So to take advantage of the per link Si feature, the platform should have
    // link 2 connected to link 2 at the peer side.
    //

    //
    // First if any of the links request for MAX_KTI_LINK_SPEED, or speed > MaxSpeed then set the speed for those links
    // to max allowed speed. Note that max allowed speed is system wide, not per link or per socket. Because Si
    // doesn't support per link max allowed speed, and we don't allow mixing of SKUs.
    // Note: The assumption is MaxSpeed and all speeds below it are assumed to be supported.
    //
    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if (SocketData->Cpu[Index].Valid == TRUE) {
        for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
          if (SocketData->Cpu[Index].LinkData[LinkIndex].Valid == TRUE) {
            if (KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[LinkIndex].KtiLinkSpeed > MaxSpeed) {
              KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[LinkIndex].KtiLinkSpeed = MaxSpeed;
            }
          }
        }
      }
    }

    //
    // Normalize speeds by:
    //       1. Making sure link 0 & 1 have same link speed, otherwise force the speed to minmum of the two.
    //       2. Making sure the peers have same link speed; otherwise select the minimum of the two.
    // Note that step 2 can cuase "chain reaction". For example, if there is a S0 link1- S1 link2 connection,
    // step will 2 can cause S0 link1's speed to change. That requires S0 link0 speed to be changed as well.
    // So we have to repeat step 1. That could cause further dependencies. So we need  to repeat step 1 & 2
    // until step they don't change any link's speed. Since during each occurance of mismatched speed the
    // minimum speed is selected, the "chain reaction" is guranteed to stop and the loop will break.
    //
    do {
      LinkDependency = FALSE;

      //
      // Make sure link 0 & 1 have same link speed; otherwise force the speed to minmum of the two.
      //
      for (Index = 0; Index < MAX_SOCKET; Index++) {
        if (SocketData->Cpu[Index].Valid == TRUE) {
          if (SocketData->Cpu[Index].LinkData[0].Valid == TRUE && SocketData->Cpu[Index].LinkData[1].Valid == TRUE) {
            Speed0 = (UINT8)KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[0].KtiLinkSpeed;
            Speed1 = (UINT8)KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[1].KtiLinkSpeed;
            if (Speed0 != Speed1) {
              (Speed0 < Speed1) ? (Speed2 = Speed0) : (Speed2 = Speed1);
              KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[0].KtiLinkSpeed = Speed2;
              KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[1].KtiLinkSpeed = Speed2;
              LinkDependency = TRUE;
            }
          }
        }
      }

      //
      // Make sure the peers have same link speed; otherwise select the minimum of the two.
      //
      for (Index = 0; Index < MAX_SOCKET; Index++) {
        if (SocketData->Cpu[Index].Valid == TRUE) {
          for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
            if (SocketData->Cpu[Index].LinkData[LinkIndex].Valid == TRUE) {
              pPeerInfo = &(SocketData->Cpu[Index].LinkData[LinkIndex]);
              Speed0 = (UINT8)KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[LinkIndex].KtiLinkSpeed;
              Speed1 = (UINT8)KtiInternalGlobal->PhyLinkPerPortSetting[pPeerInfo->PeerSocId].Phy[pPeerInfo->PeerPort].KtiLinkSpeed;
              if (Speed0 != Speed1) {
                (Speed0 < Speed1) ? (Speed2 = Speed0) : (Speed2 = Speed1);
                KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[LinkIndex].KtiLinkSpeed = Speed2;
                KtiInternalGlobal->PhyLinkPerPortSetting[pPeerInfo->PeerSocId].Phy[pPeerInfo->PeerPort].KtiLinkSpeed = Speed2;
                LinkDependency = TRUE;
              }
            }
          }
        }
      }
    }while (LinkDependency == TRUE);

    //
    // Print the normalized per link speed.
    //
    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if (SocketData->Cpu[Index].Valid == TRUE) {
         for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
           if (SocketData->Cpu[Index].LinkData[LinkIndex].Valid == TRUE) {
             KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  %s %u Link %u KTI Freq is %u GT.",
             (SocketData->Cpu[Index].SocType == SOCKET_TYPE_CPU) ? "Socket" : "FPGA",
             Index, LinkIndex,   SpeedTable[KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[LinkIndex].KtiLinkSpeed]));
           }
         }
      }
    }
  }

  //
  // If the requested speed is not per link, copy the selected speed in to the output structure and replicate it into the per link setup structure.
  //
  if (host->var.kti.OutKtiLinkSpeed != FREQ_PER_LINK) {
    KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Selected KTI Freq is %u GT", SpeedTable[LinkSpeed]));
    host->var.kti.OutKtiLinkSpeed = LinkSpeed;
    for (Index = 0; Index < MAX_SOCKET; Index++) {
      if (SocketData->Cpu[Index].Valid == TRUE || host->var.kti.OutKtiCpuSktHotPlugEn == TRUE) {
        for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
          KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[LinkIndex].KtiLinkSpeed = LinkSpeed;
        }
      }
    }
  }

  //
  // Update the host output per link speed entry with the selected speed
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if (SocketData->Cpu[Index].Valid == TRUE) {
      for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
        host->var.kti.OutPerLinkSpeed[Index][LinkIndex] = (UINT8)KtiInternalGlobal->PhyLinkPerPortSetting[Index].Phy[LinkIndex].KtiLinkSpeed;
      }
    }
  }

  return KTI_SUCCESS;
}


/**
   Routine to determine and select the maximum KTI link speed supported by all populated CPUs. It is determined by
   Min(Max link speed of all populated CPU sockets).

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Socket specific data structure

   @retval Max Link speed supported by all populated sockets

**/
UINT8
GetMaxSupportedLinkSpeed (
  struct sysHost      *host
  )
{
  UINT8                    SupportedLinkSpeeds = 0;
  UINT8                    LinkSpeed = SPEED_REC_96GT;
  UINT8                    Index;
  CAPID2_PCU_FUN3_STRUCT   Capid2;

  //
  // Select the highest common supported freq
  //
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((host->var.common.socketPresentBitMap & (BIT0 << Index)) != 0) {
      //
      // Get common supported freqs by ORing all DISABLED freqs together.   Zero bits indicate supported freqs
      //
      Capid2.Data = KtiReadPciCfg (host, Index, 0, CAPID2_PCU_FUN3_REG);
      SupportedLinkSpeeds |= Capid2.Bits.kti_allowed_cfclk_ratio_dis;
    }
  }

  if ((SupportedLinkSpeeds & BIT0) == 0) {
    LinkSpeed = SPEED_REC_104GT;
  } else {
    //
    // 9.6GT/s is always supported
    //
    LinkSpeed = SPEED_REC_96GT;
  }

  return LinkSpeed;
}

/**

   Verifies the selected KTI freq is valid for the platform
   If valid, return == KTI_SUCCESS


  @param host  - Pointer to the system host (root) structure
  @param SocketData - Socket specific data structure
  @param RequestedLinkSpeed - reuqested speed for KTI links

   @retval KTI_SUCCESS if valid speed
   @retval KTI_FAILURE if invalid speed

**/
KTI_STATUS
CheckLinkSpeedSupported (
  struct sysHost      *host,
  UINT8               RequestedLinkSpeed
  )
{
  UINT8                    SupportedLinkSpeeds = 0;
  UINT8                    Index;
  CAPID2_PCU_FUN3_STRUCT   Capid2;

  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((host->var.common.socketPresentBitMap & (BIT0 << Index)) != 0) {
      //
      // Get common supported freqs by ORing all DISABLED freqs together. Set bits indicate unsupported freqs
      //
      Capid2.Data = KtiReadPciCfg (host, Index, 0, CAPID2_PCU_FUN3_REG);
      KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  qpi_allowed_cfclk_ratio_dis of Processor %u == 0x%x", Index, Capid2.Bits.kti_allowed_cfclk_ratio_dis));
      SupportedLinkSpeeds |= Capid2.Bits.kti_allowed_cfclk_ratio_dis;

    }
  }

  //
  // See if requested freq is supported
  //
  Index = (RequestedLinkSpeed == SPEED_REC_104GT) ? 0 : 0xff;

  if ((RequestedLinkSpeed == SPEED_REC_96GT) || ((Index != 0xFF) && (((SupportedLinkSpeeds >> Index) & 1) == 0))) {
    //
    // Selected freq is valid
    //
    return KTI_SUCCESS;
  } else {
    //
    // Selected freq is invalid
    //
    return KTI_FAILURE;
  }
}

/**
   Routine to normalize the phy/link layer options so that the values to be programmed at both ends of
   link is agreeable.

  @param host  - Pointer to the system host (root) structure
  @param SocketData - Socket specific data structure
  @param KtiInternalGlobal - KTIRC internal variables.

**/
KTI_STATUS
NormalizeLinkOptions (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  )
{
  BOOLEAN                  Override;
  UINT8                    Index, LinkIndex;
  KTI_LINK_DATA            *pPeerInfo;
  struct sysSetup          *setup;

  setup = (struct sysSetup *) &host->setup;

  //
  // Normalize per link options. If the peers don't match their options, they will be normalized as follows:
  //   Disable - Force both the ends to disable.
  //   DfxCrcMode -   force the 16b CRC mode
  //   DfxL0pEnable - force L0p disabled
  //   DfxL1Enable -  force L1 disabled
  //   DfxKtiFailoverEn - force failover disabled
  //

  Override = FALSE;
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if (SocketData->Cpu[Index].Valid == TRUE) {
      for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
        if (SocketData->Cpu[Index].LinkData[LinkIndex].Valid == TRUE) {
          pPeerInfo = &(SocketData->Cpu[Index].LinkData[LinkIndex]);
          if (KtiInternalGlobal->PhyLinkPerPortSetting[Index].Link[LinkIndex].KtiPortDisable !=
              KtiInternalGlobal->PhyLinkPerPortSetting[pPeerInfo->PeerSocId].Link[pPeerInfo->PeerPort].KtiPortDisable) {
            KtiInternalGlobal->PhyLinkPerPortSetting[Index].Link[LinkIndex].KtiPortDisable = TRUE;
            KtiInternalGlobal->PhyLinkPerPortSetting[pPeerInfo->PeerSocId].Link[pPeerInfo->PeerPort].KtiPortDisable = TRUE;
            Override = TRUE;
          }
          if (setup->kti.DfxCpuCfg[Index].Link[LinkIndex].DfxCrcMode != setup->kti.DfxCpuCfg[pPeerInfo->PeerSocId].Link[pPeerInfo->PeerPort].DfxCrcMode) {
            setup->kti.DfxCpuCfg[Index].Link[LinkIndex].DfxCrcMode = CRC_MODE_16BIT;
            setup->kti.DfxCpuCfg[pPeerInfo->PeerSocId].Link[pPeerInfo->PeerPort].DfxCrcMode = CRC_MODE_16BIT;
            Override = TRUE;
          }
          if (setup->kti.DfxCpuCfg[Index].Link[LinkIndex].DfxL0pEnable != setup->kti.DfxCpuCfg[pPeerInfo->PeerSocId].Link[pPeerInfo->PeerPort].DfxL0pEnable) {
            setup->kti.DfxCpuCfg[Index].Link[LinkIndex].DfxL0pEnable = KTI_DISABLE;
            setup->kti.DfxCpuCfg[pPeerInfo->PeerSocId].Link[pPeerInfo->PeerPort].DfxL0pEnable = KTI_DISABLE;
            Override = TRUE;
          }
          if (setup->kti.DfxCpuCfg[Index].Link[LinkIndex].DfxL1Enable != setup->kti.DfxCpuCfg[pPeerInfo->PeerSocId].Link[pPeerInfo->PeerPort].DfxL1Enable) {
            setup->kti.DfxCpuCfg[Index].Link[LinkIndex].DfxL1Enable = KTI_DISABLE;
            setup->kti.DfxCpuCfg[pPeerInfo->PeerSocId].Link[pPeerInfo->PeerPort].DfxL1Enable = KTI_DISABLE;
            Override = TRUE;
          }
          if (setup->kti.DfxCpuCfg[Index].Phy[LinkIndex].DfxKtiFailoverEn != setup->kti.DfxCpuCfg[pPeerInfo->PeerSocId].Phy[pPeerInfo->PeerPort].DfxKtiFailoverEn) {
            setup->kti.DfxCpuCfg[Index].Phy[LinkIndex].DfxKtiFailoverEn = KTI_DISABLE;
            setup->kti.DfxCpuCfg[pPeerInfo->PeerSocId].Phy[pPeerInfo->PeerPort].DfxKtiFailoverEn = KTI_DISABLE;
            Override = TRUE;
          }
        }
      }
    }
  }

  if (Override == TRUE) {
    KtiLogWarning(host, 0xFF, 0xFF, 0xFF, WARN_PER_LINK_OPTION_MISMATCH);
    KtiDebugPrintWarn((host, KTI_DEBUG_WARN, "\n WARNING:   One or more per Link option mismatch detected. Forcing to common setting.  \n"));
  }


  return KTI_SUCCESS;
}

/**

    Issue physical layer reset on KTI link.

    @param host      - pointer to the system host root structure
    @param Cpu       - Socket ID
    @param Link      - Port number
    @param ResetType -

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
IssuePhyResetOnCpuLink(
  struct sysHost      *host,
  UINT8               Cpu,
  UINT8               Link,
  UINT8               ResetType
  )
{

   KTIREUT_PH_CTR1_KTI_REUT_STRUCT  KtiReutPhCtr1;
   KTIREUT_PH_CIS_KTI_REUT_STRUCT  KtiReutPhCis;

   UINT8                         WaitTryCount;
   KTI_STATUS                    Status;

   // Issue the phy layer reset with the reset modifier set to the given type
   KtiReutPhCtr1.Data = KtiReadPciCfg (host, Cpu,  Link,  KTIREUT_PH_CTR1_KTI_REUT_REG);
   KtiReutPhCtr1.Bits.cp_reset = 1;
   KtiWritePciCfg (host, Cpu,  Link,  KTIREUT_PH_CTR1_KTI_REUT_REG, KtiReutPhCtr1.Data);

   // Wait for the link to train
   for (WaitTryCount = 0; WaitTryCount <= 5; WaitTryCount++) {
     KtiReutPhCis.Data = KtiReadPciCfg (host, Cpu,  Link,  KTIREUT_PH_CIS_KTI_REUT_REG);
     if (KtiReutPhCis.Bits.s_link_up == 3) {
       break;
     }
     KtiFixedDelay(host, 100);
   }

   // Check if the link trained or not
   if (WaitTryCount > 5) {
     Status = KTI_FAILURE;
     KtiDebugPrintInfo1 ((host, KTI_DEBUG_INFO1, "\n  Link faiked to train after PhyReset."));
   } else {
     Status = KTI_SUCCESS;
   }

   return Status;
}

/**

    Program late action register per CPU per link

    @param host  - pointer to the system host root structure
    @param Cpu   - Socket ID
    @param Link  - Port number
    @param Value - Value to program

    @retval KTI_SUCCESS - on successful completion

**/
KTI_STATUS
ProgramLateActionOnCpuLink (
   struct sysHost           *host,
   UINT8                    Cpu,
   UINT8                    Link,
   UINT32                   Value
)
{
  KTIREUT_PH_CTR1_KTI_REUT_STRUCT  KtiReutPhCtr1;

  KtiReutPhCtr1.Data = KtiReadPciCfg (host, Cpu,  Link,  KTIREUT_PH_CTR1_KTI_REUT_REG);
  KtiReutPhCtr1.Bits.c_la_load_disable = Value;
  KtiWritePciCfg (host, Cpu,  Link,  KTIREUT_PH_CTR1_KTI_REUT_REG, KtiReutPhCtr1.Data);
  return KTI_SUCCESS;
}

/**

   Programs Phy layer Electrical Parameters

   @param host       - Pointer to the system host (root) structure
   @param SocketData - Socket specific data structure
   @param Index      - socket Index
   @param LinkIndex  - qpi port number

**/
KTI_STATUS
ProgramEparams (
  struct sysHost         *host,
  UINT8                  Index,
  UINT8                  LinkIndex,
  UINT8                  Group
  )
{
  UINT8      LinkSpeedParameter[128];
  UINT8      LinkSpeed = 0;
  KTI_STATUS Status;

  if (Group == KTI_GROUP) {
    LinkSpeed = (UINT8)host->var.kti.OutPerLinkSpeed[Index][LinkIndex];
  }

  //
  // OEM call to get Eparamtable. OEM call handles returning the correct EPARAM based on
  // Socket, Link, Freq, Platform
  //
  Status = OemKtiGetEParams (host, Index, LinkIndex, LinkSpeed, &LinkSpeedParameter);

  if (Status != KTI_SUCCESS) {
    //
    // No entry found at all!!
    //
    KtiCheckPoint ((UINT8)(1 << Index), SOCKET_TYPE_CPU, (UINT8)(1 << LinkIndex), ERR_FULL_SPEED_TRANSITION, MINOR_ERR_KTI_ELEC_PARAM_NOT_FOUND, host);
    KtiDebugPrintFatal ((host, KTI_DEBUG_ERROR, "\n\n***** KTI Electrical Parameters for CPU%u  Link %u is not found**** \n", Index, LinkIndex));
    KTI_ASSERT(FALSE, ERR_FULL_SPEED_TRANSITION, MINOR_ERR_KTI_ELEC_PARAM_NOT_FOUND);
  }

  ConfigSocketLinkEparams (host, Index, LinkIndex, &LinkSpeedParameter);

  return KTI_SUCCESS;
}

/**

   Programs PHY layer CTR1 and LDC registers

   @param host       - Pointer to the system host (root) structure
   @param Socket     - socket Index
   @param LinkIndex  - qpi port number

**/
KTI_STATUS
ProgramPhyLayerControlRegisters (
  struct sysHost         *host,
  KTI_SOCKET_DATA        *SocketData,
  UINT8                  Socket,
  UINT8                  LinkIndex,
  UINT8                  Group
  )
{
  KTIREUT_PH_CTR1_KTI_REUT_STRUCT          KtiReutPhCtr1;
  KTIREUT_PH_LDC_KTI_REUT_STRUCT           KtiReutPhLdc;
  KTIREUT_PH_CIS_KTI_REUT_STRUCT           KtiReutPhCis;
  KTIREUT_PH_TL0PEXITRX_KTI_REUT_STRUCT    KtiReutPhTL0pExitRx;
  KTIREUT_PH_TL0PEXITTXIDL_KTI_REUT_STRUCT KtiReutPhTL0pExitTxIdl;
  KTIREUT_PH_TL0PEXITTXACT_KTI_REUT_STRUCT KtiReutPhTL0pExitTxAct;
  KTI_DFX_CPU_LINK_SETTING                 *pCpuLinkSetting;

  pCpuLinkSetting = (KTI_DFX_CPU_LINK_SETTING *) &(host->setup.kti.DfxCpuCfg[Socket].Link[LinkIndex]);

  //
  // Get control register
  //
  KtiReutPhCtr1.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTIREUT_PH_CTR1_KTI_REUT_REG);

  //
  // Enable phy layer init begin
  //
  KtiReutPhCtr1.Bits.c_init_begin = 1;

  //
  // Enable fast mode
  //
  KtiReutPhCtr1.Bits.c_op_speed = 1;

  //
  // Setup L0p, L1 and failover based on output variables and DFX per-link overrides
  //
  KtiReutPhCtr1.Bits.c_l0p_en = host->var.kti.OutKtiLinkL0pEn;
  if (pCpuLinkSetting->DfxL0pEnable < KTI_AUTO) {
    KtiReutPhCtr1.Bits.c_l0p_en = pCpuLinkSetting->DfxL0pEnable;
  }

  //
  // Link PM workarounds need to be applied regardless if the feature is enabled
  // For details can see s4929889
  //

  //
  // Permanent SKX workaround: s4929026
  // Override L0p exit timer
  //
  KtiReutPhTL0pExitRx.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTIREUT_PH_TL0PEXITRX_KTI_REUT_REG);
  KtiReutPhTL0pExitRx.Bits.t_l0p_est = 0x27;

  if (host->var.common.cpuType == CPU_SKX && host->var.common.stepping < B0_REV_SKX) {
    //
    // A0 stepping workaround: s4929188
    // Delay the full width switchover beyond the stall cycle
    // RTL fix is pending approval in B0
    //
    KtiReutPhTL0pExitTxIdl.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTIREUT_PH_TL0PEXITTXIDL_KTI_REUT_REG);
    KtiReutPhTL0pExitRx.Bits.t_l0p_rx_sds2widthchg = 0xC;
    KtiReutPhTL0pExitTxIdl.Bits.t_l0p_tx_sds2widthchg = 0xC;
    KtiWritePciCfg (host, Socket, LinkIndex, KTIREUT_PH_TL0PEXITTXIDL_KTI_REUT_REG, KtiReutPhTL0pExitTxIdl.Data);

    KtiReutPhTL0pExitTxAct.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTIREUT_PH_TL0PEXITTXACT_KTI_REUT_REG);
    KtiReutPhTL0pExitTxAct.Bits.t_l0p_tx_exitend2l0c = 0xA0;
    KtiWritePciCfg (host, Socket, LinkIndex, KTIREUT_PH_TL0PEXITTXACT_KTI_REUT_REG, KtiReutPhTL0pExitTxAct.Data);
  } else {
    //
    // s5370781: phy resets occur with L0p enabled
    //
    KtiReutPhTL0pExitTxAct.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTIREUT_PH_TL0PEXITTXACT_KTI_REUT_REG);
    KtiReutPhTL0pExitTxAct.Bits.t_l0p_tx_exitend2l0c = 0x79;
    KtiWritePciCfg (host, Socket, LinkIndex, KTIREUT_PH_TL0PEXITTXACT_KTI_REUT_REG, KtiReutPhTL0pExitTxAct.Data);
  }

  KtiWritePciCfg (host, Socket, LinkIndex, KTIREUT_PH_TL0PEXITRX_KTI_REUT_REG, KtiReutPhTL0pExitRx.Data);

  KtiReutPhCtr1.Bits.c_l1_en = host->var.kti.OutKtiLinkL1En;
  if (pCpuLinkSetting->DfxL1Enable < KTI_AUTO) {
    KtiReutPhCtr1.Bits.c_l1_en = pCpuLinkSetting->DfxL1Enable;
  }
  host->var.kti.OutKtiPerLinkL1En[Socket][LinkIndex] = (BOOLEAN)KtiReutPhCtr1.Bits.c_l1_en;

  KtiReutPhCtr1.Bits.c_failover_en = host->var.kti.OutKtiFailoverEn;
  if (host->setup.kti.DfxCpuCfg[Socket].Phy[LinkIndex].DfxKtiFailoverEn < KTI_AUTO) {
    KtiReutPhCtr1.Bits.c_failover_en = host->setup.kti.DfxCpuCfg[Socket].Phy[LinkIndex].DfxKtiFailoverEn;
  }

  //
  // 5331578: WA for UPI L1 residency results in poor PkgC6 residency
  //
  if ((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX)) {
    KtiReutPhCtr1.Bits.c_failover_en= 0;
  }


  if (host->setup.kti.DfxParm.DfxKtiMaxInitAbort < KTI_AUTO) {
    KtiReutPhCtr1.Bits.c_numinit = host->setup.kti.DfxParm.DfxKtiMaxInitAbort;
  }

  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Socket %u Port %u : KTIREUT_PH_CTR1 write 0x%x\n", Socket, LinkIndex, KtiReutPhCtr1.Data));
  KtiWritePciCfg (host, Socket, LinkIndex, KTIREUT_PH_CTR1_KTI_REUT_REG, KtiReutPhCtr1.Data);

  //
  // Physical Determinism Control
  //
  KtiReutPhLdc.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTIREUT_PH_LDC_KTI_REUT_REG);
  KtiReutPhLdc.Bits.targetlinklatency   = 0x0;
  KtiReutPhLdc.Bits.inidriftbufflatency = 0x8;
  KtiReutPhLdc.Bits.driftbufalarmthr    = 0x4;
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Socket %u Port %u : KTIREUT_PH_LDC write 0x%x\n", Socket, LinkIndex, KtiReutPhLdc.Data));
  KtiWritePciCfg (host, Socket, LinkIndex, KTIREUT_PH_LDC_KTI_REUT_REG, KtiReutPhLdc.Data);

  //
  // Force Phy Layer readapt before full-speed transition
  //
  KtiReutPhCis.Data = KtiReadPciCfg (host, Socket, LinkIndex, KTIREUT_PH_CIS_KTI_REUT_REG);
  KtiReutPhCis.Bits.sp_rx_calib = 1;
  KtiReutPhCis.Bits.sp_tx_calib = 1;
  KtiDebugPrintInfo1((host, KTI_DEBUG_INFO1, "\n  Socket %u Port %u : KTIREUT_PH_CIS write 0x%x\n", Socket, LinkIndex, KtiReutPhCis.Data));
  KtiWritePciCfg (host, Socket, LinkIndex, KTIREUT_PH_CIS_KTI_REUT_REG, KtiReutPhCis.Data);


  return KTI_SUCCESS;
}



/**

  Gathers common Link and Phy Layer Capabilities Registers for all valid links.

  @param host   - Pointer to the system host (root) structure
  @param KtiLcp - Pointer to the KTILCP struct

**/
KTI_STATUS
GatherCommonKtiCapabilities (
  struct sysHost           *host,
  KTILCP_KTI_LLPMON_STRUCT *KtiLcp,
  KTIREUT_PH_CPR_KTI_REUT_STRUCT *KtiReutPhCpr
  )
{
  UINT8 Index, LinkIndex;
  KtiLcp->Data       = 0xffffffff;
  KtiReutPhCpr->Data = 0xffffffff;

  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if ((host->var.common.socketPresentBitMap & (BIT0 << Index)) != 0) {
      for (LinkIndex = 0; LinkIndex < host->var.common.KtiPortCnt; LinkIndex++) {
        if ((host->var.kti.CpuInfo[Index].LepInfo[LinkIndex].Valid == TRUE) || (host->setup.kti.KtiLbEn)) {
          KtiLcp->Data &= KtiReadPciCfg (host, Index, LinkIndex, KTILCP_KTI_LLPMON_REG);
          KtiReutPhCpr->Data &= KtiReadPciCfg (host, Index, LinkIndex, KTIREUT_PH_CPR_KTI_REUT_REG);
        }
      }
    }
  }

  return KTI_SUCCESS;
}

KTI_STATUS
ClearOrSetKtiDfxLckForFullSpeedTransition (
  struct sysHost *host,
  UINT8          Value
  )
{
  UINT8 Socket, Port;
  DFX_LCK_CTL_CFG_KTI_CIOPHYDFX_STRUCT DfxLckCtlCfg;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if ((host->var.common.socketPresentBitMap & (BIT0 << Socket)) != 0) {
      for (Port = 0; Port < host->var.common.KtiPortCnt; Port++) {
        if ((host->var.kti.CpuInfo[Socket].LepInfo[Port].Valid == TRUE) || (host->setup.kti.KtiLbEn)) {
          DfxLckCtlCfg.Data = KtiReadPciCfg (host, Socket, Port, DFX_LCK_CTL_CFG_KTI_CIOPHYDFX_REG);
          DfxLckCtlCfg.Bits.reutenglck = Value;
          DfxLckCtlCfg.Bits.privdbg_phy_l0_sync_lck = Value;
          KtiWritePciCfg (host, Socket, Port, DFX_LCK_CTL_CFG_KTI_CIOPHYDFX_REG, DfxLckCtlCfg.Data);
        }
      }
    }
  }

  return KTI_SUCCESS;
}
