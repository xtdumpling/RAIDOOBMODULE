//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.07
//      Bug Fixed:  Support 24NVMe LED
//      Reason:     Before RP PCI config switched to MMIO space -
//					For direct connect, we set CPU RP; for PLX bridge connect, we need to set PLX RP
//      Auditor:    Donald Han
//      Date:       Jun/08/2017
//
//  Rev. 1.06
//      Bug Fixed:  Fix 2nd CPU VMD LED malfunction
//      Reason:     PortIndex value got reset when starting next CPU
//      Auditor:    Donald Han
//      Date:       Mar/03/2017
//
//  Rev. 1.05
//      Bug Fixed:  Mask out link down error in LER_UNCERRMSK_IIO_PCIE_REG register
//      Reason:     For NVMe hotplug
//      Auditor:    Donald Han
//      Date:       Feb/17/2017
//
//  Rev. 1.04
//      Bug Fixed:  Mask out Surprised Down Error
//      Reason:     Make system no hang when removing NVMe
//      Auditor:    Donald Han
//      Date:       Jan/18/2017
//
//  Rev. 1.03
//      Bug Fixed:  Sync with branch SVN_743.
//      Reason:     
//      Auditor:    Jacker Yeh
//      Date:       Dec/30/2016
//
//  Rev. 1.02
//      Bug Fixed:  Sync with branch SVN_689 for NVMe legacy mode hotplug.
//      Reason:     
//      Auditor:    Jacker Yeh
//      Date:       Dec/20/2016
//
//  Rev. 1.01
//      Bug Fixed:  Fixed NVMe hot plug function fail issue.(Legacy Mode)
//      Reason:     SMC standard HW design.
//      Auditor:    Jacker Yeh
//      Date:       Dec/13/2016
//
//  Rev. 1.00
//      Bug Fixed:  Update SMC SVID/SDID to CPU PCIe root ports.
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Aug/17/2016
//
//*****************************************************************************

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


  @file IioPortInit.c

  Implement north bridge PCI express port initialization  

**/

#include <Token.h> //SMCPKG_SUPPORT
#include <SysFunc.h>
#include <IioSetupDefinitions.h>
#include <IioUtilityLib.h>
#include <RcRegs.h>
#include "IioPortInit.h"



/**

    Enable Slot power: Reset power_control_bit on Slot control register.

    @param IioGlobalData - Pointer to IIO globals.
    @param IioIndex      - Socket being initialized.
    @param PortIndex     - Port number being initialized.

    @retval None

**/
void
EnableSlotPower (
  IN  IIO_GLOBALS                                   *IioGlobalData,
  IN  UINT8                                          IioIndex,
  IN  UINT8                                          PortIndex
  )
{
  SLTCON_IIO_PCIE_STRUCT      SlotControl;

  //
  // Enable Power to Slot: PWRCTRL on SLOTCTRL
  //
  // HSD 4166825: HSX: Polarity for PCIe Slot Power is Wrong When HotPlug is Enabled
  //
  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Enabling PCIE Skt:%d Port:%d Bus:%x Dev:%d Func:%d Slot Power...\n", IioIndex,PortIndex, \
                                                                      IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex], \
                                                                      IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device, \
                                                                      IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function);
  SlotControl.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTCON_IIO_PCIE_REG);
#if SMCPKG_SUPPORT
  SlotControl.Bits.power_indicator_control = 1;
  if(IioGlobalData->SetupData.VMDPortEnable[IioIndex*12+PortIndex - 1])
  {
	  SlotControl.Bits.power_indicator_control = 3;
#if ULIO_SUPPORT
	  if(((PcdGet32(PcdSmcRiserID)&0x1f0f)==0x0b08)&&
		 (((IioIndex*12+PortIndex)==9)||((IioIndex*12+PortIndex)==17)))	  //24NVMe?
	  {
		  UINT8 i, data8=IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex];
		  
		  IioPciExpressWrite32(IioGlobalData, IioIndex, data8, 0, 0, 0x18, ((data8+2)<<16)+((data8+1)<<8)+data8);
		  IioPciExpressWrite32(IioGlobalData, IioIndex, data8+1, 0, 0, 0x18, ((data8+2)<<16)+((data8+2)<<8)+data8+1);
		  for(i=4; i<=0xf; i++)
			  IioPciExpressWrite8(IioGlobalData, IioIndex, data8+2, i, 0, 0x81, 3);
		  IioPciExpressWrite32(IioGlobalData, IioIndex, data8+1, 0, 0, 0x18, 0);
		  IioPciExpressWrite32(IioGlobalData, IioIndex, data8, 0, 0, 0x18, 0);
	  }
#endif //ULIO_SUPPORT    
  }
#endif
  SlotControl.Bits.power_controller_control = 0;
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTCON_IIO_PCIE_REG, SlotControl.Data);
  IioStall (IioGlobalData, LINK_INIT_POLL_TIMEOUT); //10ms delay
}

/**

    Configure SLOT power control register.

    @param IioGlobalData - Pointer to IIO Globals
    @param IioIndex      - Socket number of the port being configured.
    @param PortIndex     - Port number being configured.

    @retval None

**/
VOID
SlotPowerControl ( 
  IN  IIO_GLOBALS                                   *IioGlobalData,
  IN  UINT8                                          IioIndex,
  IN  UINT8                                          PortIndex
  )
{
  UINT16                        LoopCount;
  SLTCON_IIO_PCIE_STRUCT        SlotControl;
  SLTSTS_IIO_PCIE_STRUCT        SlotStatus;

  //
  // HSD 4166825: HSX: Polarity for PCIe Slot Power is Wrong When HotPlug is Enabled
  //
  // Check PCIE slot status
  //
  LoopCount = 0;
  SlotStatus.Data = 0;
  while (LoopCount < 50) {
    SlotStatus.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTSTS_IIO_PCIE_REG);
    if (SlotStatus.Bits.presence_detect_changed)
      break;
    IioStall (IioGlobalData, SLOT_POWER_DELAY); //1ms delay
    LoopCount++;
  }
  
  if (SlotStatus.Bits.presence_detect_state == 1) {
    if (SlotStatus.Bits.mrl_sensor_state == 0) {
      //
      // Enable Power to Slot: PWRCTRL on SLOTCTRL, turn PWD indicator ON
      //
      IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Enabling PCIE Skt:%d Port:%d Bus:%d Dev:%d Func:%d Slot Power...\n", IioIndex,PortIndex, \
                                                                      IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex], \
                                                                      IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device, \
                                                                      IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function);
      SlotControl.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTCON_IIO_PCIE_REG);
      if (IioGlobalData->IioVar.IioOutData.PciePortOwnership[PortIndex] == VMD_OWNERSHIP) {
        SlotControl.Bits.power_indicator_control = 3;           // Indicator OFF (2 bit field)
        SlotControl.Bits.attention_indicator_control = 3;       // Attention Indicator OFF (2 bit field)
      } else {
        SlotControl.Bits.power_indicator_control = 1;           // Indicator ON (2 bit field)
      }
      SlotControl.Bits.power_controller_control = 0;          // Power ON
      IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTCON_IIO_PCIE_REG, SlotControl.Data);
    } else {
      //
      // Card Present and MRL Open - Error Condition
      // Just Blink Attn Indicator without powering card
      //
      IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Disabling PCIE Skt:%d Port:%d Bus:%d Dev:%d Func:%d  (MRL Open)...\n", IioIndex,PortIndex, \
                                                                      IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex], \
                                                                      IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device, \
                                                                      IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function);

      if (IioGlobalData->IioVar.IioOutData.PciePortOwnership[PortIndex] == VMD_OWNERSHIP){
        SlotControl.Bits.power_indicator_control = 3;           // Indicator OFF (2 bit field)
        SlotControl.Bits.attention_indicator_control = 3;       // Attention Indicator OFF (2 bit field)
      } else {
        SlotControl.Bits.power_indicator_control = 3;           // Indicator OFF (2 bit field)
        SlotControl.Bits.attention_indicator_control = 2;       // Attention Indicator BLINK (2 bit field)
      }
      SlotControl.Bits.power_controller_control = 1;          // Power OFF
      IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTCON_IIO_PCIE_REG, SlotControl.Data);
    }
  } else {
    //
    // Empty slot    
    // Remove Power to Slot: PWRCTRL on SLOTCTRL
    //
    IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Disabling PCIE Skt:%d Port:%d Bus:%d Dev:%d Func:%d (No Device)...\n", IioIndex,PortIndex, \
                                                                      IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex], \
                                                                      IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device, \
                                                                      IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function);
    SlotControl.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTCON_IIO_PCIE_REG);

    SlotControl.Bits.power_indicator_control = 3;           // Indicator OFF (2 bit field)
    SlotControl.Bits.power_controller_control = 1;          // Power OFF
    if (SlotStatus.Bits.mrl_sensor_state == 0){
        SlotControl.Bits.attention_indicator_control = 3;     // Attention Indicator OFF (2 bit field)
    } else if (IioGlobalData->IioVar.IioOutData.PciePortOwnership[PortIndex] == VMD_OWNERSHIP){
        SlotControl.Bits.attention_indicator_control = 3;       // Attention Indicator OFF (2 bit field)
    }
    IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTCON_IIO_PCIE_REG, SlotControl.Data);
  }
  
  //
  // wait on slot control command to complete (max. 100ms)
  //
  LoopCount = 0;
  while (LoopCount < 100) {
    SlotStatus.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTSTS_IIO_PCIE_REG);
    if (SlotStatus.Bits.command_completed)
      break;
    IioStall (IioGlobalData, 1000); //1ms delay
    LoopCount++;
  }
  
  //
  // Clear all the pending status  
  //
  SlotStatus.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTSTS_IIO_PCIE_REG);
  SlotStatus.Bits.command_completed = 1;
  SlotStatus.Bits.presence_detect_changed = 1;
  SlotStatus.Bits.mrl_sensor_changed = 1;
  SlotStatus.Bits.power_fault_detected = 1;
  SlotStatus.Bits.attention_button_pressed = 1;
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTSTS_IIO_PCIE_REG, SlotStatus.Data);

  //
  // Enable all Hot Plug Interrupts regardless of card presence
  // except Hot Plug Interrupt HPINTEN bit-5 reg 84
  //
  SlotControl.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTCON_IIO_PCIE_REG);
  SlotControl.Bits.hot_plug_interrupt_enable = 0;
  SlotControl.Bits.command_completed_interrupt_enable = 0;
  SlotControl.Bits.presence_detect_changed_enable = 1;
  SlotControl.Bits.mrl_sensor_changed_enable = 1;
  SlotControl.Bits.power_fault_detected_enable = 1;
  SlotControl.Bits.attention_button_pressed_enable = 1;
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTCON_IIO_PCIE_REG, SlotControl.Data);
}

/**

    PCIe device initialization.

    @param IioGlobalData - Pointer to IIO globals.
    @param IioIndex      - Socket number of the PCIe device being initialized.
    @param PortIndex     - Port number of the PCIe device being initialized.

    @retval None

**/

VOID
PcieDeviceInit ( 
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex
  )
{

  UINT8   *Array;
  DEVCTRL_IIO_PCIE_STRUCT     DevCtrl;
  INTPIN_IIO_PCIE_STRUCT      IntPinReg;

  DevCtrl.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, DEVCTRL_IIO_PCIE_REG);

  Array = &(IioGlobalData->SetupData.PcieUnsupportedRequests[0]);
  if (Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
    DevCtrl.Bits.unsupported_request_reporting_enable = 1;
  } else {
    DevCtrl.Bits.unsupported_request_reporting_enable = 0;
  }

  //s3427199: register name below was incorrect - start
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, DEVCTRL_IIO_PCIE_REG, DevCtrl.Data);
  //s3427199 - end

  //
  // Read and Write back INTPIN register to lock RW-O INTPIN
  //
  IntPinReg.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, INTPIN_IIO_PCIE_REG);
  IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, INTPIN_IIO_PCIE_REG, IntPinReg.Data);
}

/**

    PCIe Link initialization.

    @param IioGlobalData - Pointer to IIO globals.
    @param IioIndex      - Socket number of the port being initialized.
    @param Port          - Port number being initialized.

    @retval None

**/

VOID
PcieLinkInit (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex
  )
{

  LNKCAP_IIO_PCIE_STRUCT    LNKCAPSet;
  LNKCAP2_IIO_PCIE_STRUCT   LNKCAP2Set;
  LNKCON_IIO_PCIE_STRUCT    LnkCon;
  LNKCON2_IIO_PCIE_STRUCT   LnkCon2;
  BOOLEAN        LinkSpeed;
  UINT16         Aspm;
  BOOLEAN        DisableLink;



  //
  // Program PCI Express Link Capabilities - Port# (31:24),
  //                                         L1 exit Latecy (17:15),
  //                                         LOs exit Latecy (14:12),
  //                                         ASPM(11:10)
  //                                         MaxLnkWidth (9:4),
  //                                         Link Speed Supported (3:0)
  //
  LNKCAPSet.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LNKCAP_IIO_PCIE_REG);
  LNKCAP2Set.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LNKCAP2_IIO_PCIE_REG);
  //
  // Port# (31:24)
  //
  LNKCAPSet.Bits.port_number = 0;                                               // clear bits 31:24
  LNKCAPSet.Bits.port_number = PortIndex;

  //
  // HSD: 5332673 [SKX_P] FPGA does not support L1 for PCIe. Only L0 supported.
  //
  if(CheckFPGAPort(IioGlobalData,IioIndex,PortIndex) == FALSE){
    //
    // L1 exit Latecy (17:15)
    //
    LNKCAPSet.Bits.l1_exit_latency = IioGlobalData->SetupData.PcieL1Latency[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex];
  }
  if (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) {
    //
    // L0s exit Latency (14:12)
    //
    LNKCAPSet.Bits.l0s_exit_latency = IioGlobalData->SetupData.PcieL0sLatency[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex];
  }

  //
  // ASPM(11:10): 00=disabled, 10=L1 only, 01 = L0s entry support, 11=Both L1 & L0s support
  //
  LNKCAPSet.Bits.active_state_link_pm_support = 0;                     // clear ASPM bits 11:10
  if(CheckFPGAPort(IioGlobalData,IioIndex,PortIndex) == FALSE){
    //
    // as per the 4030522/4030608, overriding the WA for the below ECO (to keep
    // the ASPM disabled for A0/1), since L1 is functional.
    // ivt_eco: s4570709: PUSH from jaketown: Disable PCIe L0s with unCore patch
    //  [cloned to s4030134] - (WA to keep the ASPM disabled for A0 - is abondoned)
    //
    //
    // HSD 4165822:PCIE: Global PCIE ASPM option needed
    //             Code added to check a global setup option first to see if the SV team just wants all the ports
    //             configured for L1.  If this option is set, then all ports get the L1 setting, but if the option is
    //             DISABLED, then the individual port setting uses the older setup option.
    //
    // 4168647: BIOS to enable ASPM L1 on DMI and PCIe ports by default
    LNKCAPSet.Bits.active_state_link_pm_support = 2; // Set ASPM L1 cap by default.
  } else {
    LNKCAPSet.Bits.active_state_link_pm_support = 0; // Disabled by default.
  }
  //
  // MaxLnkWidth (9:4)
  //
  LNKCAPSet.Bits.maximum_link_width = 0;                     // clear Max Link width 9:4

  // If Override Max Link Width was not specified in MaxPXPMap, then copy CurrentPXPMap (bifurcated max width) to MaxPXPMap (override max width)
  if(IioGlobalData->IioVar.IioOutData.MaxPXPMap[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex] == 0) {
    IioGlobalData->IioVar.IioOutData.MaxPXPMap[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex] = IioGlobalData->IioVar.IioOutData.CurrentPXPMap[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex];
  }
  // get max port link width
  LNKCAPSet.Bits.maximum_link_width =  IioGlobalData->IioVar.IioOutData.MaxPXPMap[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex];

  //
  // HSD 4166781: HSX,  PCIE - Mismatch in advertising link capability between lnkcap and lnkcap2
  //                    can cause legacy hardware(prior to 3.0 base PCIE spec) to train only to Gen1.
  //
  LNKCAPSet.Bits.maxlnkspd = 0;

  switch (IioGlobalData->SetupData.PciePortLinkSpeed[(IioIndex)*NUMBER_PORTS_PER_SOCKET + PortIndex]) {
    case PCIE_LINK_SPEED_AUTO:                                  // if AUTO we leave HW defaults except for port index 0
      //4167004: ESCALATE from hexaii_hsx_mock:PCIE: Link capabilities set wrong by BIOS
      LNKCAPSet.Bits.maxlnkspd = V_PCIE_PORT_PXPLNKCAP_LS_GEN3;
      LNKCAP2Set.Bits.lnkspdvec = (V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_3G_TOP  >> 1);
      break;
    case PCIE_LINK_SPEED_GEN1:
      LNKCAPSet.Bits.maxlnkspd = V_PCIE_PORT_PXPLNKCAP_LS_GEN1;
      LNKCAP2Set.Bits.lnkspdvec = (V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_1G_TOP >> 1);
      break;
    case PCIE_LINK_SPEED_GEN2:
      LNKCAPSet.Bits.maxlnkspd = V_PCIE_PORT_PXPLNKCAP_LS_GEN2;
      LNKCAP2Set.Bits.lnkspdvec = (V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_2G_TOP >> 1);
      break;
    case PCIE_LINK_SPEED_GEN3:
    default:
      LNKCAPSet.Bits.maxlnkspd = V_PCIE_PORT_PXPLNKCAP_LS_GEN3;
      LNKCAP2Set.Bits.lnkspdvec = (V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_3G_TOP >> 1);
      break;
  }

  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LNKCAP_IIO_PCIE_REG, LNKCAPSet.Data);
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LNKCAP2_IIO_PCIE_REG, LNKCAP2Set.Data);

  //
  // Program Link Control 2
  //
  LnkCon2.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON2_IIO_PCIE_REG);

  LnkCon2.Bits.enter_compliance = 0;
  LnkCon2.Bits.selectable_de_emphasis = 0;

  if(IioGlobalData->SetupData.PciePortLinkSpeed[(IioIndex)*NUMBER_PORTS_PER_SOCKET + PortIndex] != PCIE_LINK_SPEED_AUTO) {
    //
    // If link speed not set to Auto, set link speed to user-specified speed
    //
    LnkCon2.Bits.target_link_speed = 0;
    // Program Link Speed Supported (3:0)
    switch(IioGlobalData->SetupData.PciePortLinkSpeed[(IioIndex)*NUMBER_PORTS_PER_SOCKET + PortIndex]){
      case PCIE_LINK_SPEED_GEN1:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "  Program Gen1!\n");
        LnkCon2.Bits.target_link_speed = V_PCIE_PORT_PXPLNKCTRL2_LS_GEN1;
        break;
      case PCIE_LINK_SPEED_GEN2:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "  Program Gen2!\n");
        LnkCon2.Bits.target_link_speed =  V_PCIE_PORT_PXPLNKCTRL2_LS_GEN2;
        break;
      case PCIE_LINK_SPEED_GEN3:
      default:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "  Program Gen3!\n");
        LnkCon2.Bits.target_link_speed = V_PCIE_PORT_PXPLNKCTRL2_LS_GEN3;
        break;
    }
  } else {      //if desired speed is set to auto
    //
    // overwrite special port specific settings in case of auto otherwise leave to its
    // default HW state.....
    //
    LinkSpeed = CheckVarPortLinkSpeed(IioGlobalData, IioIndex, PortIndex);
    if (LnkCon2.Bits.target_link_speed != LinkSpeed){
      LnkCon2.Bits.target_link_speed = LinkSpeed;
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "  Program Gen%d!\n", LnkCon2.Bits.target_link_speed );
    } else {
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "  (auto mode) No speed change required!\n");
    }
  }

  // Set Compliance Mode
  if (IioGlobalData->SetupData.ComplianceMode[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
    LnkCon2.Bits.enter_compliance = 1;
  }

  //
  // Program De-Emphasis select (bit 6)
  //
  if(IioGlobalData->SetupData.DeEmphasis[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
    LnkCon2.Bits.selectable_de_emphasis = 1;
  }

  //
  // Update PCI Express Link Control Register 2
  //
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON2_IIO_PCIE_REG, LnkCon2.Data);

  if (CheckVarPortEnable(IioGlobalData, IioIndex, PortIndex) == PCIE_PORT_DISABLE){
    DisableLink = TRUE;
  } else {
    DisableLink = FALSE;
  }

  //
  // User need to disable link
  //
  if (IioGlobalData->SetupData.PcieLinkDis[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]){
    DisableLink = TRUE;
  }

  LnkCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIE_REG);

  if (DisableLink) {
    LnkCon.Bits.link_disable = 1;
  } else {
    //s3427267 - start*/
    LnkCon.Bits.active_state_link_pm_control = 0;
    if(CheckFPGAPort(IioGlobalData,IioIndex,PortIndex) == FALSE){
      //
      // 4168647: BIOS to enable ASPM L1 on DMI and PCIe ports by default
      //
      // ASPM(11:10): 00=disabled, 10=L1 only, 01 = L0s entry support, 11=Both L1 & L0s support
      //
      if (IioGlobalData->SetupData.PcieGlobalAspm != 1) {
        Aspm = IioGlobalData->SetupData.PcieGlobalAspm;
      } else {
        Aspm = CheckVarPortASPM(IioGlobalData, IioIndex, PortIndex);
      }
      LnkCon.Bits.active_state_link_pm_control = Aspm;
    } else{
      LnkCon.Bits.active_state_link_pm_control = 0; // Disabled  by default.
    }
    //s3427267 - end
  }

  if (IioGlobalData->SetupData.ExtendedSync[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
    LnkCon.Bits.extended_synch = 1;
  }
  
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIE_REG, LnkCon.Data);
}
/**

    PCIe slot pre init: Slot initialization

    @param IioGlobalData - Pointer to IIO globals.
    @param IioIndex      - Socket number
    @param PortIndex     - Port number

    @retval None

**/
VOID
PcieSlotPreInit (
  IN  IIO_GLOBALS                                    *IioGlobalData,
  IN  UINT8                                          IioIndex,
  IN  UINT8                                          PortIndex
  )
{
  UINT16         SlotConfig;
  UINT8          *Array;
  UINT8          *Array2;
  UINT8          *Array3;
  UINT16         *Array4;

  VPP_INVERTS_PCU_FUN5_STRUCT   VppInverts;
  VPP_INDEX_IIO_PCIE_STRUCT     VppIndex;
  VPPCTL_PCU_FUN5_STRUCT        VppPcu;
  PXPCAP_IIO_PCIE_STRUCT        PxpCap;
  SLTCAP_IIO_PCIE_STRUCT        SltCap;
  MISCCTRLSTS_1_IIO_PCIE_STRUCT MiscCtrlSts1;
  VPPFREQ_PCU_FUN5_STRUCT       VppFreq;

    Array = &(IioGlobalData->SetupData.SLOTIMP[0]);
    SlotConfig =(Array[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]);
    //
    // Set slot implemented bit
    //
    PxpCap.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, PXPCAP_IIO_PCIE_REG);
    PxpCap.Bits.slot_implemented = (SlotConfig? 1:0);
    IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, PXPCAP_IIO_PCIE_REG, PxpCap.Data);

    //
    // HSD 4166776: PCIE slot power loss on warm reset
    //              BIOS needs to ensure that SLOTCAP is zero if HP disabled
    //              Also MRLSP, the code below uses these two to decide which
    //              .. slot power control function to use
    //
    if (!IioGlobalData->SetupData.PcieHotPlugEnable) {
      Array3 = &(IioGlobalData->SetupData.SLOTHPCAP[0]);
      Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex] = 0;
      Array3 = &(IioGlobalData->SetupData.SLOTMRLSP[0]);
      Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex] = 0;
    }

    //
    // Program VPP, enable VPP and misc
    //
    VppIndex.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, VPP_INDEX_IIO_PCIE_REG);
    VppIndex.Bits.vpp_enable = 0;
    VppIndex.Bits.index = 0;
    Array  = &(IioGlobalData->SetupData.VppEnable[0]);
    Array2 = &(IioGlobalData->SetupData.VppPort[0]);
    Array3 = &(IioGlobalData->SetupData.VppAddress[0]);

    //
    // HSD 4166776: PCIE slot power loss on warm reset
    //              BIOS needs to ensure that VPP address and enable are zero if HP disabled
    //
    if (IioGlobalData->SetupData.PcieHotPlugEnable) {
      if (Array[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]) { // If zero, VPP for this port is disabled...
        VppIndex.Bits.index  = (UINT32)(Array2[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex] & 1);
        VppIndex.Bits.index |= ((Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]) & 0xf);      // mask off the upper unused SMBus address bits

        VppIndex.Bits.vpp_enable = 1;

        //
        // 4927785:IIO: F2F feedback for message channel enabling, vpp_mc_enable should be set according to PCIe hot-plug capability
        //
        VppPcu.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, 0, VPPCTL_PCU_FUN5_REG);  //PCU always use instance 0
        //
        // 5370188: Cloned From SKX Si Bug Eco: CLONE SKX Sighting: IIO RAS: VPP port hangs in Neon City for the warm reset test and vpp reset type 0
        //
        VppPcu.Bits.vpp_reset_mode = 1;
        if(VppPcu.Bits.vpp_mc_enable == 0) {
          VppPcu.Bits.vpp_mc_enable = 1;
        }
        IioWriteCpuCsr32(IioGlobalData, IioIndex, 0, VPPCTL_PCU_FUN5_REG, VppPcu.Data);
      } // VppEnable
    } // PcieHotPlugEnable

    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, VPP_INDEX_IIO_PCIE_REG, VppIndex.Data);

    //
    //4928254: IIO: VPPFREQ values should reflect 100 Mhz local clock frequency
    //
    VppFreq.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, 0, VPPFREQ_PCU_FUN5_REG);
    VppFreq.Bits.vpp_tsu_thd = 0x1F3; //to get a value of 10us
    IioWriteCpuCsr32(IioGlobalData, IioIndex, 0, VPPFREQ_PCU_FUN5_REG, VppFreq.Data);

    //
    //  HSD 4167027: PCIE: BIOS needs to program PCA9555 for correct polarity
    //
    if (IioGlobalData->SetupData.PcieHotPlugEnable) {
      VppInverts.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, 0, VPP_INVERTS_PCU_FUN5_REG);
#if SMCPKG_SUPPORT
      VppInverts.Bits.dfr_inv_pwren = 0;
#else
      VppInverts.Bits.dfr_inv_pwren = 1;
#endif
      IioWriteCpuCsr32(IioGlobalData, IioIndex, 0, VPP_INVERTS_PCU_FUN5_REG, VppInverts.Data);
    }

    // intended fix for s4030382:Pcie cards are not powering up after
    //                  a CLDRST with Serial Debug Msgs: Disabled and no PCIE_ON jumper
    if (Array[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]){
      IioStall (IioGlobalData, VPP_WAIT_DELAY);  //500us delay to sync with PCA9555
    }

    //
    // Update PCIE Slot Capabilities Register
    //
    Array  = &(IioGlobalData->SetupData.PcieLinkDis[0]);
    Array2 = &(IioGlobalData->SetupData.SLOTIMP[0]);
    Array3 = &(IioGlobalData->SetupData.SLOTHPCAP[0]);

    if (!Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {             // Valid PCIE Port?
      if (Array2[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {           // Slot Implemented?
        SltCap.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTCAP_IIO_PCIE_REG);
        MiscCtrlSts1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_1_IIO_PCIE_REG);

        //
        // set physical slot number
        //
        // HSD: 4168942: PCIe physical slot number register is 13 bit,
        // but reference code interface can only program 8 bit
        //
        Array4 = &(IioGlobalData->SetupData.SLOTPSP[0]);
        SltCap.Bits.physical_slot_number = (Array4[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex] & 0x1FFF);

        //
        // set Electromechanical Interlock Present
        //
        Array3 = &(IioGlobalData->SetupData.SLOTEIP[0]);
        if (Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
          // Set "electromechanical_interlock_present" based on "formfactor" for SIOM topologies.
          if (MiscCtrlSts1.Bits.formfactor) {
            SltCap.Bits.electromechanical_interlock_present = 1;
          }
        }
        //
        // set Slot Power Limit Scale
        //
        Array3 = &(IioGlobalData->SetupData.SLOTSPLS[0]);
        SltCap.Bits.slot_power_limit_scale = Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex];

        //
        // set slot power limit value
        //
        Array3 = &(IioGlobalData->SetupData.SLOTSPLV[0]);
        SltCap.Bits.slot_power_limit_value = Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex];

        //
        // set Hot Plug Capable
        // If slot supports HP then enable set the correct bit in
        // in the SLOTCAp register & call slot power control
        //
        Array3 = &(IioGlobalData->SetupData.SLOTHPCAP[0]);
        if (Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]){
          SltCap.Bits.hot_plug_capable = 1;
        }

        //
        // set Hot Plug Surprise
        //
        Array3 = &(IioGlobalData->SetupData.SLOTHPSUP[0]);
        if (Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]){
          SltCap.Bits.hot_plug_surprise = 1;
        }

        //
        // set Power Indicator Present
        //
        Array3 = &(IioGlobalData->SetupData.SLOTPIP[0]);
        if (Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]){
          SltCap.Bits.power_indicator_present = 1;
        }

        //
        // set Attention Indicator Present
        //
        Array3 = &(IioGlobalData->SetupData.SLOTAIP[0]);
        if (Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
          // Set "mrl_sensor_present" based on "formfactor" for SIOM topologies.
          if (MiscCtrlSts1.Bits.formfactor == 0) {
            SltCap.Bits.attention_indicator_present = 1;
          }
        }

        //
        // set MRL Sensor Present
        //
        Array3 = &(IioGlobalData->SetupData.SLOTMRLSP[0]);
        if (Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]){
          SltCap.Bits.mrl_sensor_present = 1;
        }

        //
        // set Power Controller Present
        //
        Array3 = &(IioGlobalData->SetupData.SLOTPCP[0]);
        if (Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]){
          SltCap.Bits.power_controller_present = 1;
        }

        //
        // set Attention Button Present
        //
        Array3 = &(IioGlobalData->SetupData.SLOTABP[0]);
        if (Array3[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]){
          SltCap.Bits.attention_button_present = 1;
        }

        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, SLTCAP_IIO_PCIE_REG, SltCap.Data);

      } // Slot Implemented?
    } // Valid PCIe Port?
}

/**

    PCIe slot init: Slot initialization (Power parameters, clock, etc)

    @param IioGlobalData - Pointer to IIO globals.
    @param IioIndex      - Socket number
    @param PortIndex     - Port number

    @retval None

**/
VOID
PcieSlotInit (
  IN  IIO_GLOBALS                                    *IioGlobalData,
  IN  UINT8                                          IioIndex,
  IN  UINT8                                          PortIndex
  )
{
  UINT8          *Array;
  UINT8          *Array2;
  SLTCAP_IIO_PCIE_STRUCT        SltCap;
  
  Array  = &(IioGlobalData->SetupData.PcieLinkDis[0]);
  Array2 = &(IioGlobalData->SetupData.SLOTIMP[0]);
  if (!Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {             // Valid PCIE Port?
    if (Array2[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {           // Slot Implemented?
      //
      // Previously we were not calling the SlotPower Control routine for HP init so added the call if
      // the slot is HP capable.  This CALL MUST occur after programming HC Capability and MRL present capable bits
      //
      SltCap.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SLTCAP_IIO_PCIE_REG);

      if ((SltCap.Bits.hot_plug_capable == 1) && (SltCap.Bits.mrl_sensor_present == 1)) {
        SlotPowerControl (IioGlobalData, IioIndex, PortIndex);
      } else {
        EnableSlotPower (IioGlobalData, IioIndex, PortIndex);
      }
    } // Slot Implemented?
  } // Valid PCIe Port?
}

/**

    PCIe miscelaneous initializations.

    @param IioGlobalData - Pointer to global Data
    @param IioIndex      - Socket number
    @param Port          - Port number.

    @retval None

**/
VOID
PcieMiscInit (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex
  )
{
  UINT8   *Array;
  MSIMSGCTL_IIO_PCIE_STRUCT          MsiMsgCtl;
  PERFCTRLSTS_1_IIO_PCIE_STRUCT      PerfCtrlSts1;
  PERFCTRLSTS_0_IIO_PCIE_STRUCT      PerfCtrlSts0;
  RPERRCMD_IIO_PCIE_STRUCT           RpErrCmd;
  ERRCAP_IIO_PCIE_STRUCT             ErrCap;
  XPUNCEDMASK_IIO_PCIE_STRUCT        XpUnceDMask;
  UNCERRSEV_IIO_PCIE_STRUCT          UnCerrSev;
 
  Array = &(IioGlobalData->SetupData.MsiEn[0]);
  MsiMsgCtl.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, MSIMSGCTL_IIO_PCIE_REG);
  MsiMsgCtl.Bits.msien = Array[((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex]? 1:0;
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, MSIMSGCTL_IIO_PCIE_REG, MsiMsgCtl.Data);

  //
  // PCIE PERFCTRLSTS 
  //

   
  // apply the PERFCTRL related BIOS setup options
  PerfCtrlSts0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PERFCTRLSTS_0_IIO_PCIE_REG);
  if (IioGlobalData->SetupData.PcieAllocatingFlow) {
     PerfCtrlSts0.Bits.use_allocating_flow_wr = 1;
  } else{
     PerfCtrlSts0.Bits.use_allocating_flow_wr = 0;
  }
  //
  // 4987818: IIO:Update NoSnoop BIOS Knob to only Enable/Disable options.
  // 4927752: PERFCTRLSTS_0_IIO_PCIE_EG.nosnoopoprden=0.
  //
  if(IioGlobalData->SetupData.NoSnoopRdCfg) {
    PerfCtrlSts0.Bits.nosnoopoprden = 1;
  } else {
    PerfCtrlSts0.Bits.nosnoopoprden = 0;
  }

  if(IioGlobalData->SetupData.NoSnoopWrCfg) {
    PerfCtrlSts0.Bits.nosnoopopwren = 1;
  } else {
    PerfCtrlSts0.Bits.nosnoopopwren = 0;
  }

  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PERFCTRLSTS_0_IIO_PCIE_REG, PerfCtrlSts0.Data);

  PerfCtrlSts1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PERFCTRLSTS_1_IIO_PCIE_REG);

  //
  // HSD 4165824: PCIE: BIOS option for PCIRdCurrent/DRd.UC mode select (CIPCTRL[0])
  //
  PerfCtrlSts1.Bits.full_cohrd_op = IioGlobalData->SetupData.CoherentReadFull;
  PerfCtrlSts1.Bits.partial_cohrd_op = IioGlobalData->SetupData.CoherentReadPart;

  //enable/disable TPH
  if(IsPciePortTPHCapable(IioGlobalData, IioIndex, PortIndex) == TRUE){
    PerfCtrlSts1.Bits.tphdis = IioGlobalData->SetupData.DisableTPH;
    if(IioGlobalData->SetupData.DisableTPH == FALSE){ // Enable TPH
      PerfCtrlSts1.Bits.prioritize_tph = IioGlobalData->SetupData.PrioritizeTPH;
    }
  }

  if(IioGlobalData->SetupData.MaxReadCompCombSize) {
    PerfCtrlSts1.Bits.max_read_completion_combine_size = 1;
  }
  
  //
  // 5332283: Fix IODC modes control in BIOS menu
  //
  // Check IODC

  if((IioGlobalData->SetupData.IODC[((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex]) == IODC_GLOBAL_KTI_OPTION) {
 // Apply KTI global option
    switch (IioGlobalData->IioVar.IioVData.IODC){
     case IODC_AUTO:
     case IODC_EN_REM_INVITOM_AND_WCILF:
       PerfCtrlSts1.Bits.implicit_tph = 1;
       PerfCtrlSts1.Bits.implicit_nosnp = 0;
       break;
     case IODC_EN_REM_INVITOM_PUSH:
        PerfCtrlSts1.Bits.implicit_tph = 1;
        PerfCtrlSts1.Bits.implicit_nosnp = 0;
        break;
     case IODC_EN_REM_INVITOM_ALLOCFLOW:
      PerfCtrlSts1.Bits.implicit_tph = 0;
      PerfCtrlSts1.Bits.implicit_nosnp = 0;
      break;
     case IODC_EN_REM_INVITOM_ALLOC_NONALLOC:
      PerfCtrlSts1.Bits.implicit_tph = 0;
      PerfCtrlSts1.Bits.implicit_nosnp = 1;
       break;
     case IODC_DISABLE:
       default:
       //Do nothing
       break;
    }
  } else {
    switch (IioGlobalData->SetupData.IODC[((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex]){
      // Apply particular port configuration
      case IODC_AUTO:
      case IODC_EN_REM_INVITOM_AND_WCILF:
        PerfCtrlSts1.Bits.implicit_tph = 1;
        PerfCtrlSts1.Bits.implicit_nosnp = 0;
        break;
      case IODC_EN_REM_INVITOM_PUSH:
        PerfCtrlSts1.Bits.implicit_tph = 1;
        PerfCtrlSts1.Bits.implicit_nosnp = 0;
        break;
      case IODC_EN_REM_INVITOM_ALLOCFLOW:
        PerfCtrlSts1.Bits.implicit_tph = 0;
        PerfCtrlSts1.Bits.implicit_nosnp = 0;
        break;
      case IODC_EN_REM_INVITOM_ALLOC_NONALLOC:
        PerfCtrlSts1.Bits.implicit_tph = 0;
        PerfCtrlSts1.Bits.implicit_nosnp = 1;
        break;
      case IODC_DISABLE:
        default:
        //Do nothing
        break;
     }
  }
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PERFCTRLSTS_1_IIO_PCIE_REG, PerfCtrlSts1.Data);
     
  //
  // Program Root Port Error Escalation enable/disable
  //
  RpErrCmd.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, RPERRCMD_IIO_PCIE_REG);
  if (IioGlobalData->SetupData.RpCorrectableErrorEsc[IioIndex]){
    RpErrCmd.Bits.correctable_error_reporting_enable = 1;
  }
  if (IioGlobalData->SetupData.RpUncorrectableNonFatalErrorEsc[IioIndex]){
    RpErrCmd.Bits.non_fatal_error_reporting_enable = 1;
  }
  if (IioGlobalData->SetupData.RpUncorrectableFatalErrorEsc[IioIndex]){
    RpErrCmd.Bits.fatal_error_reporting_enable = 1;
  }
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, RPERRCMD_IIO_PCIE_REG, RpErrCmd.Data);
  //
  // 4928991: BIOS needs to program ECRC Capability bits on PCIe Root Ports
  //
  ErrCap.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, ERRCAP_IIO_PCIE_REG);
  ErrCap.Bits.ecrc_check_capable = 1;
  //
  // 5370514: ECRC capability correction
  //
  ErrCap.Bits.ecrc_generation_capable = 1;
  // s5370514 - end
  
  UnCerrSev.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCERRSEV_IIO_PCIE_REG);

  //
  //5370541: ECRC capability enable options
  //
  if (IioGlobalData->SetupData.ECRC[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]){
    ErrCap.Bits.ecrc_generation_enable = 1;
    ErrCap.Bits.ecrc_check_enable = 1;    
    UnCerrSev.Bits.ecrc_error_severity = 1;
  } else {
    ErrCap.Bits.ecrc_generation_enable = 0;
    ErrCap.Bits.ecrc_check_enable = 0;    
    UnCerrSev.Bits.ecrc_error_severity = 0;
  }
  // s5370541 - end

  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, ERRCAP_IIO_PCIE_REG, ErrCap.Data);
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCERRSEV_IIO_PCIE_REG,UnCerrSev.Data);

  //  
  // 5371377: {TPP Beta} Issues while connecting switches (backplane, Futondale x8) into slots connected to 2'nd CPU on Neon City
  // Set ur_detect_mask mask for VMD ports sin cIIO RAS code is not able to seetig since 
  // Pcie Prrto Port are hidden so VMD device an take ownership
  if(IioGlobalData->IioVar.IioOutData.PciePortOwnership[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex] == VMD_OWNERSHIP) {
    XpUnceDMask.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPUNCEDMASK_IIO_PCIE_REG);
    XpUnceDMask.Bits.received_pcie_completion_with_ur_detect_mask = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPUNCEDMASK_IIO_PCIE_REG, XpUnceDMask.Data);
  }
}

/**

    PCI Pre initialization procedures.

    @param IioGlobalData - Pointer to global data structure
    @param IioIndex      - Socket number being initialized
    @param Port          - Port number being initialized

    @retval None

**/
VOID
PciePreInit (
  IN  IIO_GLOBALS                                    *IioGlobalData,
  IN  UINT8                                          IioIndex,
  IN  UINT8                                          PortIndex
  )
{
  //
  // set/reset any RWO features of the PCIe port capability register here, which
  // can be used to initialize any features on the PCIe port control registers later
  //
  DEVCAP_IIO_PCIE_STRUCT   DevCap;
  DEVCAP2_IIO_PCIE_STRUCT  DevCap2;
  //
  // init & Lock DEVCAP register
  //

  //
  // SKX eco[300732] Incorrect default values are loaded by hardware for devcap::extended_tag_field_supported and rootcap7::crs_software_visibility
  // 4928293: BIOS workaround to write the value 1'b1 to devcap dev0 fun0 bit 5 extended tag field supported
  // only if IIO ports in P-stacks that are in PCIE mode 
  //
  if (!CheckNtbPortConfSetting(IioGlobalData, IioIndex, PortIndex)){
    DevCap.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, DEVCAP_IIO_PCIE_REG);
    DevCap.Bits.extended_tag_field_supported = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DEVCAP_IIO_PCIE_REG, DevCap.Data);
  }

  //
  // init & Lock DEVCAP2 register
  //
  DevCap2.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, DEVCAP2_IIO_PCIE_REG);
                              
  //Set the TPH completer supported capability
  DevCap2.Bits.tph_completer_supported = 1;

  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DEVCAP2_IIO_PCIE_REG, DevCap2.Data);
}

/**

    Post initialization methods for PCIe

    @param PciAddress    - Address of the PCIe root port
    @param IioGlobalData - Pointer to IIO Globals.
    @param IioIndex      - Socket of the initialized port.
    @param Port          - Number of the initialized port.

    @retval None

**/
VOID
PciePostInit (
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  UINT8                          PortIndex
  )
{

    UINT16  PortWidth;
    BOOLEAN PortLinked;
    UINT8   PortSpeed;
    UINT8   Stack;
    UINT8   NtbPortIndex;
    PCICMD_IIO_PCIE_STRUCT        PciCmd;
    UNCERRSTS_IIO_PCIE_STRUCT     UncErrSts;
    LNKSTS_IIO_PCIE_STRUCT        LnkSts;
    XPCORERRSTS_IIO_PCIE_STRUCT   XpCoreErrSts;
    MISCCTRLSTS_1_IIO_PCIE_STRUCT MiscCtrlSts1;
    SLTCAP_IIO_PCIE_STRUCT        Sltcap;
    LNKCAP_IIO_PCIE_STRUCT        Lnkcap;
    MISCCTRLSTS_0_IIO_PCIE_STRUCT Miscctrlsts;
    PXPCAP_IIO_PCIE_STRUCT        Pxpcap;
    PXPNXTPTR_IIO_PCIE_STRUCT     Pxpnxtptr;
    SDID_IIO_PCIE_STRUCT          Sdid;
    SVID_IIO_PCIE_STRUCT          Svid;
    SVID_IIO_PCIENTB_STRUCT       Svid_pcientb;
    RID_IIO_PCIE_STRUCT           Rid;


    // skip DMI port
    if (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == TRUE){
     return;
    }
  
    Stack = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
    if(Stack == MAX_IIO_STACK){ // Return if stack not found
      return;
    }

    if (((IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex) >= MAX_TOTAL_PORTS) return;
    
    //
    // Command register has to be set after PCI bus enumeration
    // PCI bus driver will change the command register during the enumeration
    //
    PciCmd.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, PCICMD_IIO_PCIE_REG);

    if (IioGlobalData->SetupData.Serr){
      PciCmd.Bits.serre = 1;
    }     
    if (IioGlobalData->SetupData.Perr){
      PciCmd.Bits.perre = 1;
    }
    //
    // Program PCI SERR/PERR
    //

    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PCICMD_IIO_PCIE_REG, PciCmd.Data);

    //
    // Update for PCI Express Port 1-10 (PciAddress.Device)
    //
    LnkSts.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIE_REG);

    Sltcap.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, SLTCAP_IIO_PCIE_REG);
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, SLTCAP_IIO_PCIE_REG, Sltcap.Data);

    Lnkcap.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LNKCAP_IIO_PCIE_REG);
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LNKCAP_IIO_PCIE_REG, Lnkcap.Data);

    Miscctrlsts.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_0_IIO_PCIE_REG);
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_0_IIO_PCIE_REG, Miscctrlsts.Data);

    Pxpcap.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, PXPCAP_IIO_PCIE_REG);
    IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, PXPCAP_IIO_PCIE_REG, Pxpcap.Data);

    // Gather and store data about linked ports.
    PortWidth = LnkSts.Bits.negotiated_link_width;
    PortSpeed = (UINT8)LnkSts.Bits.current_link_speed;
    PortLinked = (LnkSts.Bits.data_link_layer_link_active == 1);

    // Can't trust Negotiated Link Width to be zero if not linked, so need to check port-linked bit.
    if(PortLinked) {
      IioGlobalData->IioVar.IioOutData.LinkedPXPMap[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex] = (UINT8) PortWidth;
    } else {
      IioGlobalData->IioVar.IioOutData.LinkedPXPMap[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex] = 0;
    }
    IioGlobalData->IioVar.IioOutData.SpeedPXPMap[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex] = PortSpeed;

    //
    // Handle programming of RWO and RWOG registers for Iio Port functions
    //

    //
    // Lock Revision ID
    //
    Rid.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, RID_IIO_PCIE_REG);
    IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, RID_IIO_PCIE_REG, Rid.Data);

#ifndef MINIBIOS_BUILD
    if (FeaturePcdGet (PcdLockCsrSsidSvidRegister)) {
#endif // MINIBIOS_BUILD
      // check if this is NTB port
      if (CheckNtbPortConfSetting(IioGlobalData, IioIndex, PortIndex)){
        NtbPortIndex = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort;
        Svid_pcientb.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, NtbPortIndex, SVID_IIO_PCIENTB_REG);
        IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, SVID_IIO_PCIENTB_REG, Svid_pcientb.Data);
      } else {
        // PCIe mode
        //
        // Lock Other Subsystem Vendor ID
        //
        Svid.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SVID_IIO_PCIE_REG);
#if SMCPKG_SUPPORT
        Svid.Bits.subsystem_vendor_id = (UINT16)(CRB1_SSID & 0x0000FFFF);
#endif
        IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, SVID_IIO_PCIE_REG, Svid.Data);
      }
      //
      // Lock Other Subsystem Device ID
      //
      Sdid.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SDID_IIO_PCIE_REG);
#if SMCPKG_SUPPORT
      Sdid.Bits.subsystem_device_id = (UINT16)(CRB1_SSID >> 16);
#endif
      IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, SDID_IIO_PCIE_REG, Sdid.Data);
#ifndef MINIBIOS_BUILD
    }
#endif //MINIBIOS_BUILD
    //
    // Lock PXPNXTPTR register
    //
    Pxpnxtptr.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, PXPNXTPTR_IIO_PCIE_REG);
    IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, PXPNXTPTR_IIO_PCIE_REG, Pxpnxtptr.Data);

    //
    // Clear Expected PCIe Errors (3612591, 3875328)
    //  [permanent programming consideration for all steppings]
    UncErrSts.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCERRSTS_IIO_PCIE_REG);
    UncErrSts.Bits.received_an_unsupported_request = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCERRSTS_IIO_PCIE_REG, UncErrSts.Data);

    LnkSts.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIE_REG);
    LnkSts.Bits.link_autonomous_bandwidth_status = 1;
    LnkSts.Bits.link_bandwidth_management_status = 1;
    IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIE_REG, LnkSts.Data);

    XpCoreErrSts.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPCORERRSTS_IIO_PCIE_REG);
    XpCoreErrSts.Bits.pci_link_bandwidth_changed_status = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPCORERRSTS_IIO_PCIE_REG, XpCoreErrSts.Data);

    if (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) {
      MiscCtrlSts1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_1_IIO_PCIE_REG);
      if (IioGlobalData->SetupData.MctpEn){
        MiscCtrlSts1.Bits.disable_mctp_broadcast_to_this_link = 0;
      } else{
        MiscCtrlSts1.Bits.disable_mctp_broadcast_to_this_link = 1;
      }
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_1_IIO_PCIE_REG, MiscCtrlSts1.Data);
    }
}

/**

    Early PCIe initializations.

    @param PciAddress    - PCIe Address of the current port being initialized.
    @param Phase         - Current phase of initialization (pre/init/post)
    @param IioGlobalData - Pointer to IIO global data /methods
    @param IioIndex      - Socket number
    @param PortIndex     - Port number

    @retval None

**/
VOID
PcieEarlyInit (  
  IN  IIO_INIT_PHASE                                 Phase,
  IN  IIO_GLOBALS                                    *IioGlobalData,
  IN  UINT8                                          IioIndex,
  IN  UINT8                                          PortIndex
  )
{
    //
    // The purpose of this routine is to do any {CIE Port programming prior to actually training the PCIE links
    //

    MISCCTRLSTS_0_IIO_PCIE_STRUCT MiscCtrlSts0;
    MISCCTRLSTS_1_IIO_PCIE_STRUCT MiscCtrlSts1;
    UINT8   *Array;

    IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "PciEarlyInit at device scanning...\n");

    // Program MISCCTRL 
    // This was moved here because there are RWO registers that must be programmed.  
    //
    MiscCtrlSts0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_0_IIO_PCIE_REG);
    MiscCtrlSts1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_1_IIO_PCIE_REG);
    //
    // clear all configurable bits and update accordingly 
    //

    MiscCtrlSts1.Bits.formfactor = 0;
    MiscCtrlSts1.Bits.override_system_error_on_pcie_fatal_error_enable = 0;
    MiscCtrlSts1.Bits.override_system_error_on_pcie_non_fatal_error_enable = 0;
    MiscCtrlSts1.Bits.override_system_error_on_pcie_correctable_error_enable = 0;
    MiscCtrlSts1.Bits.acpi_pme_inten = 0;
    MiscCtrlSts1.Bits.received_pme_to_ack = 0;

    MiscCtrlSts0.Bits.disable_l0s_on_transmitter = 0;
    MiscCtrlSts0.Bits.system_interrupt_only_on_link_bw_management_status = 0;
    MiscCtrlSts0.Bits.eoifd = 0;
    // Peer 2 Peer memory write disable not available on SKX
    MiscCtrlSts0.Bits.peer2peer_memory_read_disable = 0;
    MiscCtrlSts0.Bits.phold_disable = 0;
    MiscCtrlSts0.Bits.pme2acktoctrl = 0;
    MiscCtrlSts0.Bits.send_pme_turn_off_message = 0;
    MiscCtrlSts0.Bits.enable_system_error_only_for_aer = 0;
    MiscCtrlSts0.Bits.enable_acpi_mode_for_hotplug = 0;
    MiscCtrlSts0.Bits.enable_acpi_mode_for_pm = 0;

    //5332038: We need to set this bit to 0 to enable OB parity checking
    // (This apply just for SKX; CNX will have it in pcode or ucode)
    MiscCtrlSts0.Bits.disable_ob_parity_check = 0;
    // 
    // Avoid set EOIFD bit if NTB mode will be configured since EOIFD bit is set automatically if NTB mode 
    //
    if (CheckNtbPortConfSetting(IioGlobalData, IioIndex, PortIndex) == FALSE){
      if (IioGlobalData->IioVar.IioOutData.PciePortOwnership[PortIndex] == REGULAR_PCIE_OWNERSHIP)  {
         //
         // Adding this for SV PRD requirement
         //
         Array = &(IioGlobalData->SetupData.EOI[0]);

        if (Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
            MiscCtrlSts0.Bits.eoifd = 1;
        } 
      } else {
         MiscCtrlSts0.Bits.eoifd = 1;
         MiscCtrlSts1.Bits.disable_mctp_broadcast_to_this_link = 1;
      }
    } else {
        MiscCtrlSts1.Bits.disable_mctp_broadcast_to_this_link = 1;
    }

    // Override System Error on PCIE Fatal Errors  - Bit 35 of MISCCTRLSTS
	Array = &(IioGlobalData->SetupData.MSIFATEN[0]);
  if (Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
      MiscCtrlSts1.Bits.override_system_error_on_pcie_fatal_error_enable = 1;
    } 

    // Override System Error on PCIE Non-Fatal Errors  - Bit 34 of MISCCTRLSTS
	Array = &(IioGlobalData->SetupData.MSINFATEN[0]);
    if (Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
      MiscCtrlSts1.Bits.override_system_error_on_pcie_non_fatal_error_enable = 1;
    }

    // Override System Error on PCIE Correctable  Errors  - Bit 33 of MISCCTRLSTS
	Array = &(IioGlobalData->SetupData.MSICOREN[0]);
    if (Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
      MiscCtrlSts1.Bits.override_system_error_on_pcie_correctable_error_enable = 1;
    }

    // ACPI PME Interrupt Enable  - Bit 32 of MISCCTRLSTS
	Array = &(IioGlobalData->SetupData.ACPIPMEn[0]);
    if (Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
      MiscCtrlSts1.Bits.acpi_pme_inten = 1;
    }

    //
    // 4166584: L0s Feature Is Still Partially Present in the HSX BIOS
    //
    if (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) {
      MiscCtrlSts0.Bits.disable_l0s_on_transmitter = 1;
    }

    // P2P Memory Read Disable  - Bit 24 of MISCCTRLSTS
    Array = &(IioGlobalData->SetupData.P2PRdDis[0]);
    if (Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
      MiscCtrlSts0.Bits.peer2peer_memory_read_disable = 1;
    }

    // Disable Timeout 4 Rcv PME TO ACK  - Bit 6 of MISCCTRLSTS
    Array = &(IioGlobalData->SetupData.DisPMETOAck[0]);
    if (!Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
      MiscCtrlSts0.Bits.enable_timeout_for_receiving_pme_to_ack = 1;
      //
      // if receiving of PME to ACK TO is enabled than use global
      // option to set the timeout value in control bits 8:7
      MiscCtrlSts0.Bits.pme2acktoctrl = (IioGlobalData->SetupData.GlobalPme2AckTOCtrl & 3);
    } else{
      MiscCtrlSts0.Bits.enable_timeout_for_receiving_pme_to_ack = 0;
    }

    //
    // Enable ACPI mode HOTPLUG  - Bit 3 of MISCCTRLSTS
    //
    // 4168626 : SCI for Hot Plug is not working
    //
    if (IioGlobalData->SetupData.PcieAcpiHotPlugEnable == 1) {
         MiscCtrlSts0.Bits.enable_acpi_mode_for_hotplug = 1;
    } else if (IioGlobalData->SetupData.PcieAcpiHotPlugEnable == 2) {
        Array = &(IioGlobalData->SetupData.ACPIHP[0]);
        if (Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
          MiscCtrlSts0.Bits.enable_acpi_mode_for_hotplug = 1;
        }
    }

    //
    // Enable ACPI PM  - Bit 2 of MISCCTRLSTS
    //
    Array = &(IioGlobalData->SetupData.ACPIPM[0]);
    if (Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
      MiscCtrlSts0.Bits.enable_acpi_mode_for_pm = 1;
    }

    //
    // 5332335 - [LBG Val Critical] New option for inbound configuration enable
    //
    Array = &(IioGlobalData->SetupData.InboundConfiguration[0]);
    if (Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
      MiscCtrlSts0.Bits.inbound_configuration_enable = 1;
    }

    //
    //  5385724 : Hot Plug PCIe late Device Present Detection and command complete interrupt
    //
    if (IioGlobalData->SetupData.SLOTPCP[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex] && IioGlobalData->SetupData.SLOTHPCAP[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex] ) {
      MiscCtrlSts0.Bits.dispdspolling = 1;
    }

  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_0_IIO_PCIE_REG, MiscCtrlSts0.Data);
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_1_IIO_PCIE_REG, MiscCtrlSts1.Data);

}

/**

    Per port Early initialization before link trainig is done.

    @param IioGlobalData - Pointer to IIO global data/methods
    @param IioIndex      - Current socket.
    @param PortIndex     - Port number being initialized.

    @retval None

**/
VOID
PciePortEarlyInit (
  IN IIO_GLOBALS                    *IioGlobalData,
  IN UINT8                          IioIndex,
  IN UINT8                          PortIndex
  )
{
  //
  // If the port designated for initialization is hidden, do not attempt to initialize
  //
  if (IioGlobalData->SetupData.PEXPHIDE[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex])
    return;

  if (IioGlobalData->IioVar.IioOutData.CurrentPXPMap[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]) {
    //
    // Only initialize the present port per the port map
    //
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Initialize IIO[%x] PCIE PortIndex:%x...\n", IioIndex, PortIndex);
    PcieLinkInit   (IioGlobalData, IioIndex, PortIndex);
    PcieSlotPreInit (IioGlobalData, IioIndex, PortIndex);
  } else {
    //
    // Put links into low power mode which are not connected to slot.
    //
    IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "  Spare port --> [%s(%d)]\n", IIO_PORT_LABEL(PortIndex), PortIndex);
      // TBD: Need to be revisited
      // This may not be applicable if we consider the HP functionality.
      // For HP slots even if the card is connected when the system is powered up will show the link status bit as down until in phase 3
      // where the driver wiil power up the slots and retrains, until that point the endpoint device will not be visible in the slots
      // which are HP capable, and if we try to pull down the power level in phase 1, the cards connected to the HP slots will fail to train,
      // so this is not applicable as per the current flow
  }
}

/**

    Per port initialization.

    @param Phase         - Initialization phase (pre, init, post).
    @param IioGlobalData - Pointer to IIO global data/methods
    @param IioIndex      - Current socket.
    @param PortIndex     - Port number being initialized.

    @retval None

**/
VOID
PciePortInit (  
  IN  IIO_INIT_PHASE                 Phase,
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  UINT8                          PortIndex
  )
{
  UINT8   Var8=0;
  UINT8   *Array;
  PCI_ROOT_BRIDGE_PCI_ADDRESS                 PciAddress;
  //
  // If the port designated for initialization is hidden, do not attempt to initialize
  //
  Array = &(IioGlobalData->SetupData.PEXPHIDE[0]);
  if (Array[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex]) return;
  Var8 = IioGlobalData->IioVar.IioOutData.CurrentPXPMap[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex];

  if (!Var8) {
    //
    // Put links into low power mode which are not connected to slot.
    //
    if ( Phase == IIOInitPhase1 ) {
      IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "  Spare port --> [%s(%d)]", IIO_PORT_LABEL(PortIndex), PortIndex);
      // TBD: Need to be revisited
      // This may not be applicable if we consider the HP functionality.
      // For HP slots even if the card is connected when the system is powered up will show the link status bit as down until in phase 3
      // where the driver wiil power up the slots and retrains, until that point the endpoint device will not be visible in the slots
      // which are HP capable, and if we try to pull down the power level in phase 1, the cards connected to the HP slots will fail to train,
      // so this is not applicable as per the current flow
      //ForceUnusedPortsToL1 (PciAddress, IioGlobalData,0,Port);
    }
    return;
  } else {
    //
    // Only initialize the present port per the port map
    //
    PciAddress.Bus = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex];
    PciAddress.Device   = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device;
    PciAddress.Function = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function;
    switch (Phase) {
      case IIOInitPhase1:
        IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Initialize IIO PCIE Bus = %X Dev:%X Func:%X...\n", PciAddress.Bus, PciAddress.Device,PciAddress.Function);
        PcieEarlyInit (Phase, IioGlobalData, IioIndex, PortIndex);
        PciePreInit    (IioGlobalData, IioIndex, PortIndex);
        PcieDeviceInit (IioGlobalData, IioIndex, PortIndex);
        PcieSlotInit   (IioGlobalData, IioIndex, PortIndex);
        break;

      case IIOInitPhase2:
        PcieMiscInit   (IioGlobalData, IioIndex, PortIndex);
        break;

      case IIOInitPhase3:
        IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "PciPostInit Bus:%X Dev:%X Func%X...\n", PciAddress.Bus, PciAddress.Device,PciAddress.Function);
        PciePostInit   (IioGlobalData, IioIndex, PortIndex);
        break;

      default:
        break;
    }
  }
}


/**

  IIO Port post initialization settings after the other IIO Port resources are assigned.

  @param IioGlobalData        Pointer to the Internal Data Storage (UDS) for this driver
  @param IioIndex             Socket Index 

  @retval VOID              All other error conditions encountered result in an ASSERT

**/
VOID
PciePortPostInit (
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT8           IioIndex
  )
{
  UINT8 PortIndex;
  UINT8 NtbPortIndex;
  UINT8 PowerState;
  UINT8   *Array;
  PMCSR_IIO_PCIE_STRUCT      Pmcsr;
  PMCSR_IIO_PCIENTB_STRUCT   PmcsrNtb;
#if SMCPKG_SUPPORT  
  UNCERRMSK_IIO_PCIE_STRUCT	Uncerrmsk;
#endif  
  MSINXTPTR_IIO_PCIE_STRUCT Msinxtptr;


  for ( PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
      if (!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex))
         continue;

      IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "PciePortPostInit IIO[%x], Port = %x\n", IioIndex, PortIndex);
      //
      // Set D0 or D3Hot state based on setup option
      //
      Array = &(IioGlobalData->SetupData.PcieDState[0]);
      if (Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex] == 1) {
        PowerState = V_PCIE_PORT_PXPPMCSR_PWR_STATE_D3HOT;
      } else {
        PowerState = V_PCIE_PORT_PXPPMCSR_PWR_STATE_D0;
      }

      //
      // 4927900: IIO: NTB setting change for acsctrl and pmcsr.power_state
      //
      // 2. if NTB, setup pmcsr.power_state after NTB or move pmcsr.power_state setting after NTB setup.
      // 4. Msinxtptr moved to after NTB
      // 5. PMCSR_IIOCB, PMCSR_IIO_IOAPIC, PMCSR_IIO_PCIE, all 3 should be programmed after NTB.

      // Check if this is NTB port
      if (CheckNtbPortConfSetting(IioGlobalData, IioIndex, PortIndex)){
         //
         // Update PMCSR and lock the register
         //
         NtbPortIndex = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort;
         PmcsrNtb.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, NtbPortIndex, PMCSR_IIO_PCIENTB_REG);
         PmcsrNtb.Bits.power_state = PowerState;
         IioWriteCpuCsr32(IioGlobalData, IioIndex, NtbPortIndex, PMCSR_IIO_PCIENTB_REG, PmcsrNtb.Data);
      } else {
        // if PCIE Mode
        //
        // Update PMCSR and lock the register
        //
        Pmcsr.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PMCSR_IIO_PCIE_REG);
        Pmcsr.Bits.power_state = PowerState;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PMCSR_IIO_PCIE_REG, Pmcsr.Data);
        //
        // Lock MSINXTPTR register
        //
        Msinxtptr.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, MSINXTPTR_IIO_PCIE_REG);
        IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, MSINXTPTR_IIO_PCIE_REG, Msinxtptr.Data);
      }
#if SMCPKG_SUPPORT
      Uncerrmsk.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCERRMSK_IIO_PCIE_REG);
      Uncerrmsk.Bits.surprise_down_error_mask = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCERRMSK_IIO_PCIE_REG, Uncerrmsk.Data);
      
      Uncerrmsk.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCEDMASK_IIO_PCIE_REG);
      Uncerrmsk.Bits.surprise_down_error_mask = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCEDMASK_IIO_PCIE_REG, Uncerrmsk.Data);

      Uncerrmsk.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LER_UNCERRMSK_IIO_PCIE_REG);
      Uncerrmsk.Bits.surprise_down_error_mask = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LER_UNCERRMSK_IIO_PCIE_REG, Uncerrmsk.Data);
#endif
  } // End PortIndex

}
