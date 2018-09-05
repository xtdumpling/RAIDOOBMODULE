//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Update SMC SVID/SDID to CPU DMI root port.
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Oct/04/2016
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


 @file IioDmiInit.c

 This file initialize the DMI device on IIO

**/

#include <Token.h> //SMCPKG_SUPPORT
// APTIOV_SERVER_OVERRIDE_RC_START : Force Gen2 Speed for DMI Link to avoid reset while DMI link retraining in A1/A2 CPU
#include <SysHostChip.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Force Gen2 Speed for DMI Link to avoid reset while DMI link retraining in A1/A2 CPU
#include <IioPlatformData.h>
#include <OemIioInit.h>
#include <IioSetupDefinitions.h>
#include <IioUtilityLib.h>
#include <IioDmiInit.h>
#include <IioRegs.h>
#include <RcRegs.h>

#ifndef MINIBIOS_BUILD
#include <DataTypes.h>
#include <Uefi.h>
#include <Library/PchPcieRpLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/IoLib.h>
#include <Library/PchInfoLib.h>
#endif //MINIBIOS_BUILD

/**

 This function will issue a link re-train process in DMI if required

 @param IioGlobalData           Pointer to the Internal Data Storage (UDS) for this driver
 @param IioIndex                Socket Index
  @param PortIndex               Index port that will be used   

 @retval None                   All other error conditions encountered result in an ASSERT

**/
VOID 
DmiLinkReTrain (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex,
  IN  UINT8               PortIndex
  )
{
  UINT8  DmiPortNegWidth;
  UINT8  DmiPortNegSpeed;
  UINT8  PortWidth;
  UINT8  PortSpeed;
  LNKSTS_IIO_PCIEDMI_STRUCT LnkSts;
  LNKCON_IIO_PCIEDMI_STRUCT LnkCon;
  UINT32   WaitTime;
 IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "DMI Link Retrain()\n");

  //
  // Assumption: the DMI link speed control bits are already set to desired value, thus
  // this routine will only take care of DMI link retrain and status
  //

  WaitTime = LINKTRAINING_TIME_OUT;
  do{
    IioStall(IioGlobalData, LINKTRAINING_DELAY);
    LnkSts.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIEDMI_REG);
    WaitTime--;
  } while((LnkSts.Bits.link_training == 1) && (WaitTime > 0) );

  if (WaitTime == 0){
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Link Training Error: DMI link training bit has not be cleared\n");
    IIO_ASSERT(IioGlobalData, IIO_ERR_GENERAL_FAILURE, IIO_MINOR_ERR_TIMEOUT);
  }
  //
  // Now order the link to retrain.
  //
  LnkCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG);
  LnkCon.Bits.retrain_link = 1;
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG, LnkCon.Data);

  //
  // retrain again.
  //
  LnkCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG);
  LnkCon.Bits.retrain_link = 1;
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG, LnkCon.Data);

  //
  // Clear Error
  //
  LnkSts.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIEDMI_REG);
  LnkSts.Bits.link_autonomous_bandwidth_status = 1;
  LnkSts.Bits.link_bandwidth_management_status = 1;
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIEDMI_REG, LnkSts.Data);

  WaitTime = LINKTRAINING_TIME_OUT;
  do{
    IioStall(IioGlobalData, LINKTRAINING_DELAY);
    LnkSts.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIEDMI_REG);
    WaitTime--;
  } while((LnkSts.Bits.link_training == 1) && (WaitTime > 0) );

  LnkSts.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIEDMI_REG);

  PortSpeed = (UINT8)LnkSts.Bits.current_link_speed;
  PortWidth = (UINT8)LnkSts.Bits.negotiated_link_width;

  IioGlobalData->IioVar.IioOutData.LinkedPXPMap[0] = PortWidth;
  IioGlobalData->IioVar.IioOutData.SpeedPXPMap[0]  = PortSpeed;

  switch(PortSpeed) {
    case 2:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "DMI speed is 5GT/s (Gen2)\n");
        break;
    case 1:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "DMI speed is 2.5GT/s (Gen1)\n");
        break;
    case 3:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "DMI speed is 8GT/s (Gen3)\n");
        break;
    default:
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "DMI speed unknown\n");
        break;
  }

  //
  // Determine Negotiated link width and speed for DMI port.
  // (LnkSts) = get LNKSTS mode DMI register data

  if (LnkSts.Bits.data_link_layer_link_active){                  // is this link active?
    DmiPortNegWidth = (UINT8)LnkSts.Bits.negotiated_link_width;  // if so, update Negotiated width value.
    DmiPortNegSpeed = (UINT8)LnkSts.Bits.current_link_speed;     // update Negotiated speed value.
  } else {
    DmiPortNegWidth = 0;                                         // if not, clear negotiated width value.
    DmiPortNegSpeed = 0;                                         // Clear negotiated speed value.
  }

  //
  // Degraded DMI link width is not POR. So error and halt if DMI negotiated width is reduced!
  //
  if (DmiPortNegWidth != 0x04) {
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "DMI is in a degraded state!  Should be 0x4 width, but is currently running at 0x%x!\n", DmiPortNegWidth);
    // HSD 3614405: BIOS PBG CPV requires a setup option to avoid BIOS hanging on DMI not training to x4 Gen2
    if(IioGlobalData->SetupData.HaltOnDmiDegraded) {  // Is "halt on DMI degraded" setup option  enabled?
      IIO_ASSERT(IioGlobalData, IIO_ERROR, IIO_MINOR_ERR_GENERAL);                              // if so, halt.
    }
  }

  //
  // Degraded DMI link speed is not POR. So error and halt if DMI link speed is reduced!
  //
  if (IioGlobalData->SetupData.PciePortLinkSpeed[0] != PCIE_LINK_SPEED_AUTO){ //ignore if it is auto
    if (DmiPortNegSpeed != IioGlobalData->SetupData.PciePortLinkSpeed[0]) {
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "DMI is in a degraded state!  Link Status current_link_speed field should %x, but is currently %x!\n", IioGlobalData->SetupData.PciePortLinkSpeed[0], DmiPortNegSpeed);
      // HSD 3614405: BIOS PBG CPV requires a setup option to avoid BIOS hanging on DMI not training to x4 Gen2
      if(IioGlobalData->SetupData.HaltOnDmiDegraded) {  // Is "halt on DMI degraded" setup option  enabled?
        IIO_ASSERT(IioGlobalData, IIO_ERROR, IIO_MINOR_ERR_GENERAL);                              // if so, halt.
      }
    }
  }
}

/**

  DMI link specific initialization

  @param IioGlobalData           Pointer to the Internal Data Storage (UDS) for this driver

  @retval VOID              All other error conditions encountered result in an ASSERT

**/
VOID
DmiLinkInit (
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  UINT8                          PortIndex
  )
{
  LNKCAP_IIO_PCIEDMI_STRUCT   DmiLnkCap;
  LNKCAP2_IIO_PCIEDMI_STRUCT  DmiLnkCap2;
  LNKCON2_IIO_PCIEDMI_STRUCT  DmiLnkCon2;
  LNKCON_IIO_PCIEDMI_STRUCT   DmiLnkCon;
  LNKCON3_IIO_PCIEDMI_STRUCT  DmiLnkCon3;
  DBG2STAT_IIO_DFX_STRUCT     Dbg2Stat;

  //
  // Setup the DMI port Link capability features (which have RW-O attribute)
  //
  DmiLnkCap.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LNKCAP_IIO_PCIEDMI_REG);
  //
  // L1 exit Latecy (17:15)
  //
  DmiLnkCap.Bits.l1_exit_latency = IioGlobalData->SetupData.PcieL1Latency[0];
  //
  // L0s exit Latecy (14:12)
  //
  DmiLnkCap.Bits.l0s_exit_latency = IioGlobalData->SetupData.PcieL0sLatency[0];
  
  // 4165822: PCIE: Global PCIE ASPM option needed
  //             Code added to check a global setup option first to see if the user wants all the ports
  //             configured for L1.  If this option is set, then all ports get the L1 setting, if the option is
  //             DISABLED L1 will be disabled for all ports, if Per-Port option is selected then the individual port setting 
  //             will be used to configure ASPM
  //  
  //             ASPM(11:10): 00=disabled, 10=L1 only, 01 = L0s entry support, 11=Both L1 & L0s support
  //  
  if (IioGlobalData->SetupData.PcieGlobalAspm != 1) {
       DmiLnkCap.Bits.active_state_link_pm_support = IioGlobalData->SetupData.PcieGlobalAspm;
  } else {
      if(IioGlobalData->SetupData.PcieAspm[0] != PCIE_ASPM_AUTO){  //if auto than disable
          DmiLnkCap.Bits.active_state_link_pm_support = IioGlobalData->SetupData.PcieAspm[0];
      } else {
          DmiLnkCap.Bits.active_state_link_pm_support = PCIE_ASPM_DISABLE;;
      }
  }

  //
  // MaxLnkWidth (9:4)
  //
  // If Override Max Link Width was not specified in MaxPXPMap, then copy CurrentPXPMap (bifurcated max width) to MaxPXPMap (override max width)
  if(IioGlobalData->IioVar.IioOutData.MaxPXPMap[0] == 0){
    IioGlobalData->IioVar.IioOutData.MaxPXPMap[0] = IioGlobalData->IioVar.IioOutData.CurrentPXPMap[0];
  }
  DmiLnkCap.Bits.maximum_link_width = IioGlobalData->IioVar.IioOutData.MaxPXPMap[0];
  //
  // ivt_eco s4677507: PCIE - Mismatch in advertising link capability between
  //                   lnkcap and lnkcap2 can cause legacy hardware(prior to 3.0
  //                   base PCIE spec) to train only to Gen1.
  //  [cloned to s4031727] - applicable to IVX, HSX, BDX
  // 
  // 4925605: BIOS to support DMI Gen3
  //
  DmiLnkCap.Bits.maxlnkspd = V_PCIE_PORT_PXPLNKCAP_LS_GEN3; 

  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LNKCAP_IIO_PCIEDMI_REG, DmiLnkCap.Data);

  //
  // Program Link Capabilities 2
  //
  DmiLnkCap2.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, LNKCAP2_IIO_PCIEDMI_REG);

  // APTIOV_SERVER_OVERRIDE_RC_START : Force Gen2 Speed for DMI Link to avoid reset while DMI link retraining in A1/A2 CPU
  if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) && (IioGlobalData->IioVar.IioVData.CpuStepping < B0_REV_SKX)) {
      IioGlobalData->SetupData.PciePortLinkSpeed[0] = PCIE_LINK_SPEED_GEN2;
  }
  // APTIOV_SERVER_OVERRIDE_RC_END : Force Gen2 Speed for DMI Link to avoid reset while DMI link retraining in A1/A2 CPU
  
  DmiLnkCap2.Bits.lnkspdvec = 0;
  switch (IioGlobalData->SetupData.PciePortLinkSpeed[(IioIndex) * NUMBER_PORTS_PER_SOCKET + PortIndex]) {
    case 0:
      DmiLnkCap2.Data |= V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_3G_TOP;
      break;
    case 1:
      DmiLnkCap2.Data |= V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_1G_TOP;
      break;
    case 2:
      DmiLnkCap2.Data |= V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_2G_TOP;
      break;
    case 3:
    default:
      DmiLnkCap2.Data |= V_PCIE_PORT_PXPLNKCAP2_LNKSPDVEC_3G_TOP;
      break;
  }

  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, LNKCAP2_IIO_PCIEDMI_REG, DmiLnkCap2.Data);


  //
  // 4928760: Disable gen 2 override needs to be set for DMI to train to GEN3
  //
  Dbg2Stat.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, DBG2STAT_IIO_DFX_REG);
  Dbg2Stat.Bits.disablegen2over = 1;
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DBG2STAT_IIO_DFX_REG, Dbg2Stat.Data);
  
  //
  // 5332537: [LBG Val Critical] LBG DMI Tx co-efficient update
  //  Program Link Control 3 for DMI for PCH Equalization settings.
  //
  DmiLnkCon3.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, 0, LNKCON3_IIO_PCIEDMI_REG);
  DmiLnkCon3.Bits.perfeq = 1;
  IioWriteCpuCsr32(IioGlobalData, IioIndex, 0, LNKCON3_IIO_PCIEDMI_REG, DmiLnkCon3.Data);
  //
  // Program Link Control 2
  // 
  DmiLnkCon2.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON2_IIO_PCIEDMI_REG);
  //
  // Set Compliance Mode (bit 4)
  //
  DmiLnkCon2.Bits.enter_compliance = IioGlobalData->SetupData.ComplianceMode[0];
  //
  // Program De-Emphasis select (bit 6)
  //
  DmiLnkCon2.Bits.selectable_de_emphasis = IioGlobalData->SetupData.DeEmphasis[0];

  //
  // Target link speed
  //
  // Assumption: PCH DMI is always program to Gen2 by PCH driver, therefore set the DMI to Gen2 as default,
  // if user knob indicates Gen1 or Gen3, than first program the PCH DMI to Gen1/Gen3 than CPU root complex to Gen1/Gen3
  //
  // 4925605: BIOS to support DMI Gen3
  // 
  switch (IioGlobalData->SetupData.PciePortLinkSpeed[0]) {
      case 1: //Gen1
        if(SetPchDmiLinkSpeed(IioGlobalData, V_DMI_PORT_PXPLNKCTRL2_LS_GEN1) == TRUE)
          DmiLnkCon2.Bits.target_link_speed = V_DMI_PORT_PXPLNKCTRL2_LS_GEN1;
          break;
      case 2: //Gen2
          if(SetPchDmiLinkSpeed(IioGlobalData, V_DMI_PORT_PXPLNKCTRL2_LS_GEN2) == TRUE){
            DmiLnkCon2.Bits.target_link_speed = V_DMI_PORT_PXPLNKCTRL2_LS_GEN2;
          }
         break;
      case 0: //Auto
      default:
      case 3: //Gen3
        if(SetPchDmiLinkSpeed(IioGlobalData, V_DMI_PORT_PXPLNKCTRL2_LS_GEN3) == TRUE){
            DmiLnkCon2.Bits.target_link_speed = V_DMI_PORT_PXPLNKCTRL2_LS_GEN3;
        }
        break;
  }
  
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON2_IIO_PCIEDMI_REG, DmiLnkCon2.Data);

  //
  // Program DMI other link control features
  //
  DmiLnkCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG);
  //
  // ivt_eco: s4570709: PUSH from jaketown: Disable PCIe L0s with unCore patch
  //  [cloned to s4030134] - WA is to remove the L0s from the knob defn., L1 as capability will be indicated clearly in B0 spec
  //
  // 4165822: PCIE: Global PCIE ASPM option needed
  //             Code added to check a global setup option first to see if the user wants all the ports
  //             configured for L1.  If this option is set, then all ports get the L1 setting, if the option is
  //             DISABLED L1 will be disabled for all ports, if Per-Port option is selected then the individual port setting 
  //             will be used to configure ASPM
  //  
  //             ASPM(11:10): 00=disabled, 10=L1 only, 01 = L0s entry support, 11=Both L1 & L0s support
  //  
  if (IioGlobalData->SetupData.PcieGlobalAspm != 1) {
       DmiLnkCon.Bits.active_state_link_pm_control = IioGlobalData->SetupData.PcieGlobalAspm;
  } else {
    if (IioGlobalData->SetupData.PcieAspm[0] != PCIE_ASPM_AUTO){  //if auto than disable
        DmiLnkCon.Bits.active_state_link_pm_control = IioGlobalData->SetupData.PcieAspm[0];
    } else {
        DmiLnkCon.Bits.active_state_link_pm_control =  PCIE_ASPM_DISABLE;
    }  
  }

  //
  // Program Extended Sync
  //
  DmiLnkCon.Bits.extended_synch = IioGlobalData->SetupData.ExtendedSync[0];
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG, DmiLnkCon.Data);
}


/**

  DMI initialization before Bus number is/are assigned

  @param IioGlobalData           Pointer to the Internal Data Storage (UDS) for this driver

  @retval VOID              All other error conditions encountered result in an ASSERT

**/
VOID
DmiPreInit (
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  UINT8                          PortIndex 
  )
{
  INTPIN_IIO_PCIE_STRUCT     IntPin;
  PCICMD_IIO_PCIEDMI_STRUCT  PciCmd;
  DEVCAP_IIO_PCIEDMI_STRUCT  DevCap;
  DEVCAP2_IIO_PCIEDMI_STRUCT DevCap2;
  MISCCTRLSTS_0_IIO_PCIEDMI_STRUCT MiscCtrlSts0;
  MISCCTRLSTS_1_IIO_PCIEDMI_STRUCT MiscCtrlSts1;

  //
  // Disable interrupts on legacy host bridge (Legacy DMI).
  //
  PciCmd.Data = IioReadCpuCsr16(IioGlobalData,IioIndex, PortIndex, PCICMD_IIO_PCIEDMI_REG);
  PciCmd.Bits.interrupt_disable = 1;
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, PCICMD_IIO_PCIEDMI_REG, PciCmd.Data);

  // write back read value to lock RW-O INTPIN for legacy socket DMI
  IntPin.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, INTPIN_IIO_PCIEDMI_REG);
  IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, INTPIN_IIO_PCIEDMI_REG, IntPin.Data);

  // #5331490: PCIE:  DMI Max Payload incorrectly set
  DevCap.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, DEVCAP_IIO_PCIEDMI_REG);
  DevCap.Bits.max_payload_size_supported = 1;
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DEVCAP_IIO_PCIEDMI_REG, DevCap.Data);

  //
  // SA BSU 0.8.11 
  // #3614765, #3247235: devcap2.ari_en default is "1" for DMI port and it
  //                     should be set to 0 by BIOS
  // This is a permanent workaround
  //
  DevCap2.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, DEVCAP2_IIO_PCIEDMI_REG);
  DevCap2.Bits.ari_en = 0;
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DEVCAP2_IIO_PCIEDMI_REG, DevCap2.Data);

  // Program MISCCTRL
  // This was moved here because there are RWO registers that must be programmed.
  //

  MiscCtrlSts0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_0_IIO_PCIEDMI_REG);
  MiscCtrlSts1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_1_IIO_PCIEDMI_REG);
  MiscCtrlSts0.Bits.eoifd = 0;
  MiscCtrlSts0.Bits.disable_l0s_on_transmitter = 0;
  MiscCtrlSts0.Bits.peer2peer_memory_read_disable = 0;
  MiscCtrlSts1.Bits.override_system_error_on_pcie_fatal_error_enable = 0;
  MiscCtrlSts1.Bits.override_system_error_on_pcie_non_fatal_error_enable = 0;
  MiscCtrlSts1.Bits.override_system_error_on_pcie_correctable_error_enable =0;
  MiscCtrlSts1.Bits.acpi_pme_inten = 0;
  if(IioGlobalData->SetupData.EOI[0]) {
     MiscCtrlSts0.Bits.eoifd = 1;
  }
  // Override System Error on PCIE Fatal Errors  - Bit 35 of MISCCTRLSTS
  if(IioGlobalData->SetupData.MSIFATEN[0]) {
     MiscCtrlSts1.Bits.override_system_error_on_pcie_fatal_error_enable = 1;
  }
  // Override System Error on PCIE Non-Fatal Errors  - Bit 34 of MISCCTRLSTS
  if(IioGlobalData->SetupData.MSINFATEN[0]) {
     MiscCtrlSts1.Bits.override_system_error_on_pcie_non_fatal_error_enable = 1;
  }
  // Override System Error on PCIE Correctable  Errors  - Bit 33 of MISCCTRLSTS
  if(IioGlobalData->SetupData.MSICOREN[0]) {
      MiscCtrlSts1.Bits.override_system_error_on_pcie_correctable_error_enable = 1;
  }


  // ACPI PME Interrupt Enable  - Bit 32 of MISCCTRLSTS
  if(IioGlobalData->SetupData.ACPIPMEn[0]) {
     MiscCtrlSts1.Bits.acpi_pme_inten = 1;
  }
  // Disable L0s on transmitter  - Bit 31 of MISCCTRLSTS
  MiscCtrlSts0.Bits.disable_l0s_on_transmitter = 1;

  // P2P Memory Read Disable  - Bit 24 of MISCCTRLSTS
  if(IioGlobalData->SetupData.P2PRdDis[0]) {
     MiscCtrlSts0.Bits.peer2peer_memory_read_disable = 1;
  }
  // Disable Timeout 4 Rcv PME TO ACK  - Bit 6 of MISCCTRLSTS
  if (!IioGlobalData->SetupData.DisPMETOAck[0]) {
      MiscCtrlSts0.Bits.enable_timeout_for_receiving_pme_to_ack = 1;
      //
      // if receiving of PME to ACK TO is enabled than use global
      // option to set the timeout value in control bits 8:7
      MiscCtrlSts0.Bits.pme2acktoctrl = IioGlobalData->SetupData.GlobalPme2AckTOCtrl & 3;
  } else {
     MiscCtrlSts0.Bits.enable_timeout_for_receiving_pme_to_ack = 0;
  }

  //5332038: We need to set this bit to 0 to enable OB parity checking
  // (This apply just for SKX; CNX will have it in pcode or ucode)
  MiscCtrlSts0.Bits.disable_ob_parity_check = 0;


  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_0_IIO_PCIEDMI_REG, MiscCtrlSts0.Data);
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_1_IIO_PCIEDMI_REG, MiscCtrlSts1.Data);
}

/**

  DMI initialization after Bus number is/are assigned but other DMI resources are 
  not assigned.
  
   @param IioGlobalData           Pointer to the Universal Data Storage (UDS) for this driver

  @retval VOID              All other error conditions encountered result in an ASSERT

**/
VOID
DmiDeviceInit (
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  UINT8                          PortIndex
  )
{
#ifndef MINIBIOS_BUILD
  UINT8                        RpIndex, MaxPchPciePortNum;
  UINT16                       Data16, DeviceFound, MaxPayLoadSize;
  UINT8                        Data8;
  UINTN                        RpDevice, RpFunction, RpBase;
#endif  // MINIBIOS_BUILD
  DEVCTRL_IIO_PCIEDMI_STRUCT   DmiDevCtrl;
  PMCSR_IIO_PCIEDMI_STRUCT     DmiPmCsr;
  DEVCTRL2_IIO_PCIEDMI_STRUCT  DmiDevCtrl2;
  
  //
  // Program the Device Control features
  //
  DmiDevCtrl.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, DEVCTRL_IIO_PCIEDMI_REG);

  DmiDevCtrl.Bits.unsupported_request_reporting_enable = IioGlobalData->SetupData.PcieUnsupportedRequests[0];

  //
  // 4925638 Support Max Payload of 256 B (Legacy)
  //
  if (IioGlobalData->SetupData.PcieMaxPayload[0] != 2) {
      if (IioGlobalData->SetupData.PcieMaxPayload[0]) {
        DmiDevCtrl.Bits.max_payload_size = MAX_PAYLOAD_SIZE_256B; //256B
      } else {
        DmiDevCtrl.Bits.max_payload_size = MAX_PAYLOAD_SIZE_128B; //128B
      }
  } else {
    //
    // 5372153:PCIe Max Payload Size determination broken
    //
#ifndef MINIBIOS_BUILD
   MaxPchPciePortNum   = GetPchMaxPciePortNum ();
   MaxPayLoadSize = MAX_PAYLOAD_SIZE_256B;

   //
   // PCH: PCI EXPRESS PORT 1-20
   //
   for (RpIndex = 0; RpIndex < MaxPchPciePortNum; RpIndex++) {

     GetPchPcieRpDevFun (RpIndex, &RpDevice, &RpFunction);
     RpBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32)RpDevice, (UINT32)RpFunction);
     DeviceFound = MmioRead16 (RpBase);

     if(DeviceFound == 0xFFFF){
       continue;
     }

     Data16 = MmioRead16 (RpBase + R_PCH_PCIE_DCAP);
     Data16 &= B_PCIE_DCAP_MPS;

     if (Data16 < MaxPayLoadSize) {
       MaxPayLoadSize = Data16;
     }
   }

   //
   //PCH: cAVS(Audio, Voice, Speec) BDF 0x0:0x1F:0x3
   //
   RpBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, 0x1F, 0x03);
   DeviceFound = MmioRead16 (RpBase);
   if(DeviceFound != 0xFFFF){
     Data8 =  MmioRead8 (RpBase + R_PCIE_PORT_STATUS);
     if ((Data8 & BIT4) != 0) {
       Data16 = MmioRead16 (RpBase  + 0x74);
       Data16 &= B_PCIE_DCAP_MPS;
       if (Data16 < MaxPayLoadSize) {
         MaxPayLoadSize = Data16;
       }
     }
   }

   DmiDevCtrl.Bits.max_payload_size = MaxPayLoadSize;
#else
   DmiDevCtrl.Bits.max_payload_size = MAX_PAYLOAD_SIZE_128B;
#endif  // MINIBIOS_BUILD
   // end 5372153
  }

  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, DEVCTRL_IIO_PCIEDMI_REG, DmiDevCtrl.Data);
  //
  // Set D0 or D3Hot state based on setup option
  //
  DmiPmCsr.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PMCSR_IIO_PCIEDMI_REG);
  if (IioGlobalData->SetupData.PcieDState[0] == 1){
     DmiPmCsr.Bits.power_state = 3;    // bit1:0 = 11: D3_hot
  } else {
     DmiPmCsr.Bits.power_state = 0;    // bit1:0 = 00: D0
  }
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PMCSR_IIO_PCIEDMI_REG, DmiPmCsr.Data);

  //
  // Program the DEVCTRL2 Completion Timeout features
  //
  DmiDevCtrl2.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, DEVCTRL2_IIO_PCIEDMI_REG);
  DmiDevCtrl2.Data &= ~(B_PCIE_PORT_PXPDEVCTRL2_ARI | B_PCIE_PORT_PXPDEVCTRL2_CTPE | B_PCIE_PORT_PXPDEVCTRL2_CTV);
  if (IioGlobalData->SetupData.CompletionTimeoutGlobal != 2) {
    if (IioGlobalData->SetupData.CompletionTimeoutGlobal == 1 ) {
      DmiDevCtrl2.Bits.compltodis = 1;
      DmiDevCtrl2.Bits.compltoval = 0;
    } else {
      DmiDevCtrl2.Bits.compltodis = 0;
      DmiDevCtrl2.Bits.compltoval = IioGlobalData->SetupData.CompletionTimeoutGlobalValue;
    }
  } else {
      if (IioGlobalData->SetupData.CompletionTimeoutValue[0]) {
          DmiDevCtrl2.Bits.compltodis = 0;
          DmiDevCtrl2.Bits.compltoval = IioGlobalData->SetupData.CompletionTimeoutValue[0];
      } else {
          DmiDevCtrl2.Bits.compltodis = 1;
          DmiDevCtrl2.Bits.compltoval = 0;
      }
    }
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, DEVCTRL2_IIO_PCIEDMI_REG, DmiDevCtrl2.Data);

}

/**

  DMI initialization all miscellaneous CSRs are initialized
  
  @param IioGlobalData           Pointer to the Universal Data Storage (UDS) for this driver

  @retval VOID              All other error conditions encountered result in an ASSERT

**/
      
VOID
DmiMiscInit (
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  UINT8                          PortIndex
  )
{
  RPERRCMD_IIO_PCIEDMI_STRUCT        RpErrCmd;
  MSIMSGCTL_IIO_PCIEDMI_STRUCT       MsiMsgCtl;
  PERFCTRLSTS_1_IIO_PCIEDMI_STRUCT   PerfCtrlSts1Dmi;
  PERFCTRLSTS_0_IIO_PCIEDMI_STRUCT   PerfCtrlSts0Dmi;
  TSWCTL5_IIO_DFX_GLOBAL_DMI_STRUCT  TswCtl5;
  UINT8   *Array;

  Array = &(IioGlobalData->SetupData.MsiEn[0]);

  MsiMsgCtl.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, MSIMSGCTL_IIO_PCIEDMI_REG);
  MsiMsgCtl.Bits.msien=(Array[((IioIndex)*NUMBER_PORTS_PER_SOCKET)+PortIndex]?1:0);
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, MSIMSGCTL_IIO_PCIEDMI_REG, MsiMsgCtl.Data);

  //
  // 4986124: HSX Clone: [C0 PO] Isoc: Vcm appears to be the only traffic getting priority at the HA
  // open bits definition to setup Vc1 correctly and get priority on the ring
  //
  // 4927708: IIO: vc1m_nosnoopopdis changed to vc1m_ns_disable
  // PCIEIPERFCTRLSTS.vc1m_nosnoopopdis has been removed for SKX and replaced by TSWCTL5.vc1m_ns_disable
  TswCtl5.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, TSWCTL5_IIO_DFX_GLOBAL_DMI_REG);
  TswCtl5.Bits.vc1m_ns_disable = 0;
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, TSWCTL5_IIO_DFX_GLOBAL_DMI_REG, TswCtl5.Data);

  //
  // PCIE PERFCTRLSTS
  //
  PerfCtrlSts0Dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PERFCTRLSTS_0_IIO_PCIEDMI_REG);
  if(IioGlobalData->SetupData.DmiAllocatingFlow){
    PerfCtrlSts0Dmi.Bits.use_allocating_flow_wr = 1;
  } else {
    PerfCtrlSts0Dmi.Bits.use_allocating_flow_wr = 0;
  }

  //
  // 4927752: PERFCTRLSTS_0_IIO_PCIE_EG.nosnoopoprden=0.
  //
  if(IioGlobalData->SetupData.NoSnoopRdCfg) {
    PerfCtrlSts0Dmi.Bits.nosnoopoprden = 1;
  } else {
    PerfCtrlSts0Dmi.Bits.nosnoopoprden = 0;
  }


  if(IioGlobalData->SetupData.NoSnoopWrCfg) {
    PerfCtrlSts0Dmi.Bits.nosnoopopwren = 1;
  } else {
    PerfCtrlSts0Dmi.Bits.nosnoopopwren = 0;
  }
  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PERFCTRLSTS_0_IIO_PCIE_REG, PerfCtrlSts0Dmi.Data);

  PerfCtrlSts1Dmi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PERFCTRLSTS_1_IIO_PCIEDMI_REG);

  //enable/disable TPH
  if (IsPciePortTPHCapable(IioGlobalData, IioIndex, PortIndex) == TRUE){
    PerfCtrlSts1Dmi.Bits.tphdis = IioGlobalData->SetupData.DisableTPH;
    if(IioGlobalData->SetupData.DisableTPH == FALSE){ // Enable TPH
      PerfCtrlSts1Dmi.Bits.prioritize_tph = IioGlobalData->SetupData.PrioritizeTPH;
    }
  }

  if(IioGlobalData->SetupData.MaxReadCompCombSize) {
    PerfCtrlSts1Dmi.Bits.max_read_completion_combine_size = 1;
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
  		PerfCtrlSts1Dmi.Bits.implicit_tph = 1;
  		PerfCtrlSts1Dmi.Bits.implicit_nosnp = 0;
  	    break;
  	  case IODC_EN_REM_INVITOM_PUSH:
  		PerfCtrlSts1Dmi.Bits.implicit_tph = 1;
  		PerfCtrlSts1Dmi.Bits.implicit_nosnp = 0;
  	    break;
  	  case IODC_EN_REM_INVITOM_ALLOCFLOW:
  		PerfCtrlSts1Dmi.Bits.implicit_tph = 0;
  		PerfCtrlSts1Dmi.Bits.implicit_nosnp = 0;
  	    break;
  	  case IODC_EN_REM_INVITOM_ALLOC_NONALLOC:
  		PerfCtrlSts1Dmi.Bits.implicit_tph = 0;
  		PerfCtrlSts1Dmi.Bits.implicit_nosnp = 1;
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
  		PerfCtrlSts1Dmi.Bits.implicit_tph = 1;
  		PerfCtrlSts1Dmi.Bits.implicit_nosnp = 0;
  	    break;
  	  case IODC_EN_REM_INVITOM_PUSH:
  		PerfCtrlSts1Dmi.Bits.implicit_tph = 1;
  		PerfCtrlSts1Dmi.Bits.implicit_nosnp = 0;
  	    break;
  	  case IODC_EN_REM_INVITOM_ALLOCFLOW:
  		PerfCtrlSts1Dmi.Bits.implicit_tph = 0;
  		PerfCtrlSts1Dmi.Bits.implicit_nosnp = 0;
  	    break;
  	  case IODC_EN_REM_INVITOM_ALLOC_NONALLOC:
  		PerfCtrlSts1Dmi.Bits.implicit_tph = 0;
  		PerfCtrlSts1Dmi.Bits.implicit_nosnp = 1;
  	    break;
  	  case IODC_DISABLE:
  	  default:
  	    //Do nothing
  	    break;
  	  }
    }


  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PERFCTRLSTS_1_IIO_PCIEDMI_REG, PerfCtrlSts1Dmi.Data);

  //
  // Program Root Port Error Escalation enable/disable
  //
  RpErrCmd.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, RPERRCMD_IIO_PCIEDMI_REG);
  if (IioGlobalData->SetupData.RpCorrectableErrorEsc[IioIndex]){
     RpErrCmd.Bits.correctable_error_reporting_enable = 1;
  }

  if (IioGlobalData->SetupData.RpUncorrectableNonFatalErrorEsc[IioIndex]){
     RpErrCmd.Bits.non_fatal_error_reporting_enable = 1;
  }

  if (IioGlobalData->SetupData.RpUncorrectableFatalErrorEsc[IioIndex]){
     RpErrCmd.Bits.fatal_error_reporting_enable = 1 ;
  }

  IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, RPERRCMD_IIO_PCIEDMI_REG, RpErrCmd.Data);
}
      

/**

  DMI initialization all DMI resources (bus and other resources) are assigned.
  
  @param IioGlobalData           Pointer to the Internal Data Storage (UDS) for this driver

  @retval VOID              All other error conditions encountered result in an ASSERT

**/
VOID
DmiResourceAssignedInit (
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  UINT8                          PortIndex
  )
{
    UINT32  Var32;
    UINT8   Iio;
    SDID_IIO_PCIEDMI_STRUCT        Sdid_Iio_PcieDmi;
    SVID_IIO_PCIEDMI_STRUCT        Svid_Iio_PcieDmi;
    MSINXTPTR_IIO_PCIEDMI_STRUCT   Msinxtptr_Iio_PcieDmi;
    PXPNXTPTR_IIO_PCIEDMI_STRUCT   Pxpnxtptr_Iio_PcieDmi;
    PMCSR_IIO_PCIEDMI_STRUCT       Pmcsr_Iio_PcieDmi;
    DMIRCBAR_IIO_PCIEDMI_STRUCT    DmiRcBar;
    UNCEDMASK_IIO_PCIEDMI_STRUCT   UncedMask;
    UNCERRSTS_IIO_PCIEDMI_STRUCT   UncErrSts;
    LNKSTS_IIO_PCIEDMI_STRUCT      LnkSts;
    XPCORERRSTS_IIO_PCIEDMI_STRUCT XpCorErrSts;
    RID_IIO_PCIE_STRUCT            Rid_Iio_Pcie;
    Iio = IioIndex;
    //Program the DMIRCBAR
    if(IioGlobalData->IioVar.IioVData.RcBaseAddress) {
      DmiRcBar.Data = IioGlobalData->IioVar.IioVData.RcBaseAddress;
      DmiRcBar.Bits.dmircbaren = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, DMIRCBAR_IIO_PCIEDMI_REG, DmiRcBar.Data);
    }

    // Handle programming of RWO and RWOG registers for IIO Port functions
    // Lock Revision ID
    Rid_Iio_Pcie.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, RID_IIO_PCIE_REG);  // !!use PCIE CSR on DMI, functional ok. SKXTODO: wait for new header file
    IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, RID_IIO_PCIE_REG, Rid_Iio_Pcie.Data);  // !!use PCIE CSR on DMI, functional ok. SKXTODO: wait for new header file

#ifndef MINIBIOS_BUILD
    if (FeaturePcdGet (PcdLockCsrSsidSvidRegister)) {
#endif //MINIBIOS_BUILD
      // Lock Subsystem Vendor ID
      Svid_Iio_PcieDmi.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SVID_IIO_PCIEDMI_REG);
#if SMCPKG_SUPPORT
      Svid_Iio_PcieDmi.Bits.subsystem_vendor_id = (UINT16)(CRB1_SSID & 0x0000FFFF);
#endif
      IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, SVID_IIO_PCIEDMI_REG, Svid_Iio_PcieDmi.Data);

      // Lock Subsystem Device ID
      Sdid_Iio_PcieDmi.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, SDID_IIO_PCIEDMI_REG);
#if SMCPKG_SUPPORT
      Sdid_Iio_PcieDmi.Bits.subsystem_device_id = (UINT16)(CRB1_SSID >> 16);
#endif
      IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, SDID_IIO_PCIEDMI_REG, Sdid_Iio_PcieDmi.Data);
#ifndef MINIBIOS_BUILD
    }
#endif //MINIBIOS_BUILD

    // Lock MSINXTPTR register
    Msinxtptr_Iio_PcieDmi.Data = IioReadCpuCsr8(IioGlobalData,IioIndex, PortIndex, MSINXTPTR_IIO_PCIEDMI_REG);
    IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, MSINXTPTR_IIO_PCIEDMI_REG, Msinxtptr_Iio_PcieDmi.Data);

    // Lock PXPNXTPTR register
    Pxpnxtptr_Iio_PcieDmi.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, PXPNXTPTR_IIO_PCIEDMI_REG);
    IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, PXPNXTPTR_IIO_PCIEDMI_REG, Pxpnxtptr_Iio_PcieDmi.Data);

    // Lock PMCSR register
    Pmcsr_Iio_PcieDmi.Data = IioReadCpuCsr32(IioGlobalData,IioIndex, PortIndex, PMCSR_IIO_PCIEDMI_REG);
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PMCSR_IIO_PCIEDMI_REG, Pmcsr_Iio_PcieDmi.Data);
    
    //
    // DMI masks should not be set, so clear them out
    //
    UncedMask.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCEDMASK_IIO_PCIEDMI_REG);
    UncedMask.Bits.poisoned_tlp_detect_mask = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCEDMASK_IIO_PCIEDMI_REG, UncedMask.Data);
    Var32 = 0x00000000;

    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, COREDMASK_IIO_PCIEDMI_REG, Var32);                                     
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, RPEDMASK_IIO_PCIEDMI_REG, Var32);                                     
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPUNCEDMASK_IIO_PCIEDMI_REG, Var32);
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPCOREDMASK_IIO_PCIEDMI_REG, Var32);
    //
    // Clear Expected PCIe Errors (3612591, 3875328)
    //  [permanent programming consideration for all steppings]
    UncErrSts.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCERRSTS_IIO_PCIEDMI_REG);
    UncErrSts.Bits.received_an_unsupported_request = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, UNCERRSTS_IIO_PCIEDMI_REG, UncErrSts.Data);

    LnkSts.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIEDMI_REG);
    LnkSts.Bits.link_autonomous_bandwidth_status = 1;
    LnkSts.Bits.link_bandwidth_management_status = 1;
    IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIEDMI_REG, LnkSts.Data);

    XpCorErrSts.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPCORERRSTS_IIO_PCIEDMI_REG);
    XpCorErrSts.Bits.pci_link_bandwidth_changed_status = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, XPCORERRSTS_IIO_PCIEDMI_REG, XpCorErrSts.Data);
}

/**

    Initialize DMI IIO function

  @param DmiPciAddress     - PCI address information of the device being enumerated/initialized
  @param Phase             - Defines the phase of PCI initialization that the PCI driver is executing. 
                             Either: 
                               EfiPciEnumerationDeviceScanning
                               EfiPciEnumerationBusNumberAssigned
                               EfiPciEnumerationResourceAssigned

  @retval Status            -
  @retval EFI_SUCCESS       - Installed successfully
  @retval Others            - Failed to install this protocol

**/
VOID
DmiInit (
  IN  IIO_INIT_PHASE                 Phase,
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex
  )
{
  UINT8  PortIndex = PORT_0_INDEX;

  switch (Phase) {
    case IIOInitPhase1:
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "DMI IIOInitPhase1...\n");
      DmiPreInit (IioGlobalData, IioIndex, PortIndex);
      OemDmiPreInit (IioGlobalData,IioIndex, PortIndex);
      break;

    case IIOInitPhase2:
      IioDebugPrintInfo(IioGlobalData,IIO_DEBUG_INFO, "DMI IIOInitPhase2...\n");
      DmiDeviceInit ( IioGlobalData, IioIndex, PortIndex);
      OemDmiDeviceInit ( IioGlobalData, IioIndex, PortIndex);
      break;

    case IIOInitPhase3:
      IioDebugPrintInfo(IioGlobalData,IIO_DEBUG_INFO, "DMI IIOInitPhase3...\n");
    //  DmiLinkInit ( IioGlobalData, IioIndex, PortIndex);
      DmiMiscInit ( IioGlobalData, IioIndex, PortIndex);
      DmiResourceAssignedInit (IioGlobalData, IioIndex, PortIndex);
      OemDmiResourceAssignedInit ( IioGlobalData,IioIndex, PortIndex);
      break;

    default:
      break;
  }
}

/**

 DMI post initialization settings after the other DMI resources are assigned.

 @param IioGlobalData           Pointer to the Internal Data Storage (UDS) for this driver

 @retval VOID              All other error conditions encountered result in an ASSERT

**/
VOID
DmiPostInit (
    IIO_GLOBALS             *IioGlobalData,
    UINT8                   IioIndex
  )
{

}
