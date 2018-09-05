/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2017   Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioInitialize.c

  Implement IIO initialization protocol to provide IIO initialization functions.
  This interfaces will be called by PciHostBridge driver in the PCI bus enumeration.

**/
#include <SysFunc.h>
#include <IioSetupDefinitions.h>
#include <IioUtilityLib.h>
#include <IioInitLib.h>
#include <RcRegs.h>
#include "IioEvDebug.h"
#include "IioWorkAround.h"
#include "IioVtdInit.h"
#include "IioIoApicInit.h"
#include "IioNtbInit.h"
#include "IioDmiInit.h"
#include "IioPortInit.h"
#include "IioEarlyInitialize.h"
#include "IioSecurePlatform.h"
#include "IioLateInitialize.h"
#ifndef IA32
#include <Library/BaseMemoryLib.h>
#endif
/**

    VMD - Volume Management Device, setup

    @param IioGlobalData - Pointer to IioGlobalData

    @retval None

**/
VOID
IioVMDLateSetup (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex
  )
{
  UINT8                     StackIndex;
  UINT8                     PortIndex;
  VMCONFIG_IIO_VMD_STRUCT   VMConfig;
  VMASSIGN_IIO_VMD_STRUCT   VMAssign;
  CFGBAR_N0_IIO_VMD_STRUCT  CfgBar;
  MEMBAR1_N0_IIO_VMD_STRUCT MemBar1;
  MEMBAR2_N0_IIO_VMD_STRUCT MemBar2;
  UINT8                     Attr;

  for ( StackIndex = 0; StackIndex < VMD_STACK_PER_SOCKET; StackIndex++) {    //only check p-stacks which is stack index 1-3
     if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << (StackIndex + IIO_PSTACK0)))){
       continue;
     }

     // Skip initialization if VMD is not enabled is this stack.
     if(IioGlobalData->SetupData.VMDEnabled[(IioIndex*VMD_STACK_PER_SOCKET)+StackIndex]== 0){
       continue;
     }

     VMAssign.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, VMASSIGN_IIO_VMD_REG);

     // Skip VMD configuration if Config space is not valid.
     if(VMAssign.Data == 0xFFFFFFFF){
       IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "VMD Stack %02d not present\n", VMAssign.Data);
       continue;
     }

     // program CfgBar attribute
     CfgBar.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, CFGBAR_N0_IIO_VMD_REG);
     Attr = IioGlobalData->SetupData.VMDCfgBarAttr[IioIndex*VMD_STACK_PER_SOCKET+StackIndex];
     switch(Attr) {
       case VMD_32BIT_NONPREFETCH:  //32-bit non-prefetchable
           CfgBar.Bits.prefetchable = 0;
           CfgBar.Bits.type = 0;
           break;
       case VMD_64BIT_NONPREFETCH: //64-bit non-prefetcheble
           CfgBar.Bits.prefetchable = 0;
           CfgBar.Bits.type = 2;
           break;
       case VMD_64BIT_PREFETCH:  //64-bit prefetchable
          CfgBar.Bits.prefetchable = 1;
          CfgBar.Bits.type = 2;
          break;
     } //switch
     IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, CFGBAR_N0_IIO_VMD_REG, CfgBar.Data);

     // program MemBar1 attribute
     MemBar1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, MEMBAR1_N0_IIO_VMD_REG);
     Attr = IioGlobalData->SetupData.VMDMemBar1Attr[IioIndex*VMD_STACK_PER_SOCKET+StackIndex];
     switch(Attr) {
       case VMD_32BIT_NONPREFETCH:  //32-bit non-prefetchable
          MemBar1.Bits.prefetchable = 0;
          MemBar1.Bits.type = 0;
          break;
       case VMD_64BIT_NONPREFETCH: //64-bit non-prefetchable
           MemBar1.Bits.prefetchable = 0;
           MemBar1.Bits.type = 2;
           break;
       case VMD_64BIT_PREFETCH:  //64-bit prefetchable
          MemBar1.Bits.prefetchable = 1;
          MemBar1.Bits.type = 2;
          break;
     } //switch
     IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, MEMBAR1_N0_IIO_VMD_REG, MemBar1.Data);

       // program MemBar2 attribute
       MemBar2.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, MEMBAR2_N0_IIO_VMD_REG);
       Attr = IioGlobalData->SetupData.VMDMemBar2Attr[IioIndex*VMD_STACK_PER_SOCKET+StackIndex];
       switch(Attr) {
         case VMD_32BIT_NONPREFETCH:  //32-bit non-prefetchable
            MemBar2.Bits.prefetchable = 0;
            MemBar2.Bits.type = 0;
            break;
         case VMD_64BIT_NONPREFETCH: // 64-bit non-prefetchable
           MemBar2.Bits.prefetchable = 0;
           MemBar2.Bits.type = 2;
           break;
         case VMD_64BIT_PREFETCH:  //64-bit prefetchable
            MemBar2.Bits.prefetchable = 1;
            MemBar2.Bits.type = 2;
            break;
       } //switch
       IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, MEMBAR2_N0_IIO_VMD_REG, MemBar2.Data);

       IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "VMD VMAssign:%02x CfgBar:%02d MemBar1:%02d MemBar2:%02d\n", VMAssign.Data, CfgBar.Data, MemBar1.Data, MemBar2.Data);

       // program VMConfig to lock VMD register on this rootport
       VMConfig.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, VMCONFIG_IIO_VMD_REG);
       VMConfig.Bits.vmdlock = 1;
       IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, VMCONFIG_IIO_VMD_REG, VMConfig.Data);
       //
       // Hide Root Ports configured as VMD since VMD driver will access them through VMD.CFGBAR
       //
       for ( PortIndex = 0; PortIndex < VMD_PORT_PER_STACK; PortIndex++) {      //only check root ports on this p-stacks
         if(IioGlobalData->SetupData.VMDPortEnable[(IioIndex*VMD_PORTS_PER_SOCKET) + (StackIndex * VMD_PORT_PER_STACK) +  PortIndex]){
           IioGlobalData->SetupData.PEXPHIDE[ (NUMBER_PORTS_PER_SOCKET * IioIndex) + (StackIndex * VMD_PORT_PER_STACK) +  (PortIndex + 1)] = 1;
           IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "PortIndex = %x is hidden since acting as VMD Port.\n", ((NUMBER_PORTS_PER_SOCKET * IioIndex) + (StackIndex * VMD_PORT_PER_STACK) +  (PortIndex + 1)));
         }
       } // for ( PortIndex = 0 ...
     } //for ( StackIndex = 0...

}



/**

  This function does DFX specific work


  @param Phase         - Defines the phase of PCI initialization that the PCI driver is executing.
                             Either:
                               EfiPciEnumerationDeviceScanning
                               EfiPciEnumerationBusNumberAssigned
                               EfiPciEnumerationResourceAssigned
  @param IioGlobalData - Pointer to IIO_GLOBALS
  @param IioIndex      - Index to Iio

  @retval None
**/
VOID
IioDfxInit(
    IIO_INIT_PHASE                                      Phase,
    IIO_GLOBALS                                         *IioGlobalData,
    UINT8                                               IioIndex
  )
{
  CSIPSUBSAD_IIO_DFX_GLOBAL_STRUCT   Csipsubsad;

  UINT8         Stack;

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
     if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack)))
          continue;
     //
     // SA BSU 0.8.11, 7.5.7
     // #3612865: Legacy GPE (HPGPE, PMEGPE, GPE) messages not routed to IRP when DMI is non-legacy
     //
      if (((IioIndex != 0) && (Stack != 0)) && (Phase == IIOInitPhase3)) {
        Csipsubsad.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, CSIPSUBSAD_IIO_DFX_GLOBAL_REG);
        Csipsubsad.Bits.valid = 1;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CSIPSUBSAD_IIO_DFX_GLOBAL_REG, Csipsubsad.Data);
        //DEBUG((EFI_D_INFO, "Legacy GPE to IRP for DMI/PCIE on Stack:x%02X\n", Stack));
      }
   } // End of for (Stack = 0; Stack < MAX_IIO_STACK; Stack++)
}

/**

  Call back routine used to do all IIO chipset initialization

  @param   IioGlobalData - Pointer to IIO_GLOBALS
  @param   IioIndex      - Socket Index

  @retval  None

**/
VOID
IioInit (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT8        IioIndex
  )
{
  UINT8                                Phase;
  UINT8                                PortIndex;

  for (Phase = 1; Phase <= IIOInitPhase3; Phase = Phase << 1) {

    //
    //Warning: If anyone should decide that changing order of silicon functional module calls,
    //make sure you examine all code to verify that the functions you are changing are
    //not order dependent!
    //

    if (Phase == IIOInitPhase1) {
       IioBreakAtCheckPoint (IioGlobalData, STS_IIO_PCIE_PORT_INIT, Phase, IioIndex);
    }
    //
    // Then do the DMI and PCIE ports
    //
    for ( PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
      if( IioVData_ConfigurePciePort(IioGlobalData,IioIndex, PortIndex) == FALSE){  //check whether the PCIe port can be configured
        continue;
      }

      if (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == TRUE){
        DmiInit (Phase, IioGlobalData, IioIndex);
      } else {
        PciePortInit (Phase, IioGlobalData, IioIndex, PortIndex);
      }
    } // End of for ( PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++)

  } // Phase for ..


  IioBreakAtCheckPoint (IioGlobalData, STS_IIO_IOAPIC_INIT, Phase, IioIndex);
  IioIoApicInit ( IioGlobalData, IioIndex);

  IioBreakAtCheckPoint (IioGlobalData, STS_IIO_VTD_INIT, Phase, IioIndex);
  VtdInitialization (IioGlobalData, IioIndex);

  IioBreakAtCheckPoint (IioGlobalData, STS_IIO_DFX_INIT, Phase, IioIndex);
  IioDfxInit(Phase, IioGlobalData, IioIndex);

  if(IioGlobalData->SetupData.DfxJitterLogger) {
      ShowRxStats(IioGlobalData);
  }
}


/**

  Call back routine used to do all IIO post PCie port configuration, initialization

  @param IioGlobalData  - Pointer to IIO_GLOBALS
  @param IioIndex       - Socket Index

  @retval None
**/
VOID
IioPostInit (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT8         IioIndex
  )
{

  IIO_DEVFUNHIDE_TABLE IioDevFuncHideBR[MaxIIO];

  //
  // init the IIO misc. control features (Dev 5, Fun 0)
  //
  IioWorkAround (IioGlobalData, IioIndex, IioPortEnumProgramMISCCTRL);
  IioMiscFeatureInit(IioIndex, IioGlobalData);

  // s5371329
  IioLerWorkaround(IioGlobalData, IioIndex);

  IioRxRecipeWorkAround(IioGlobalData, IioIndex);

  ProgProblematicPortConfig(IioGlobalData, IioIndex);

  //let's switch to Ntb mode here
  IioBreakAtCheckPoint (IioGlobalData, STS_IIO_NTB_INIT, STS_IIO_INIT, IioIndex);
  NtbInit(IioGlobalData, IioIndex);

  // Any Workaround for PCIe PortPostInit phase
  IioWorkAround (IioGlobalData, IioIndex, IioPostInitEnd);
  // any post PCIe init DMI settings are done below
  DmiPostInit(IioGlobalData, IioIndex);

  // any post PCIe init settings are done below
  PciePortPostInit(IioGlobalData, IioIndex);

  // VMD Post Configuration
  IioVMDLateSetup(IioGlobalData, IioIndex);

  //
  // Enable PECI
  //
  IioBreakAtCheckPoint (IioGlobalData, STS_IIO_SECURITY_INIT, STS_IIO_INIT, IioIndex);
  TxtTrustPeci(IioGlobalData, IioIndex);

  // Clean IioDevFuncHideBR before use it
#ifdef IA32
  MemSetLocal ((UINT8 *)&(IioDevFuncHideBR), 0x00, (sizeof(IIO_DEVFUNHIDE_TABLE)* MaxIIO));
#else
  ZeroMem(&(IioDevFuncHideBR), (sizeof(IIO_DEVFUNHIDE_TABLE)* MaxIIO));
#endif //IA32
  //
  // let hide the PCIe root ports which are forced by user, or whose lanes are
  // used by other ports, or the ports which are not present
  //
  IioPcieDeviceHide(IioGlobalData, IioIndex, IioDevFuncHideBR);
  // now initiate any post IO APIC init setting; like signal for LT SCHECK to be performed
  IoApicPostInit(IioGlobalData, IioIndex );
  // Now Hide IIO Devices before Pcie Resource Allocation phase
  IioMiscHide(IioGlobalData, IioIndex, IioBeforeResources, IioDevFuncHideBR);
}


/**

    This function update Subsystem Vendor Id in all Uncore devices

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
UpdateSsids (
  IN  IIO_GLOBALS *IioGlobalData
  )
{
  UINT8  IioIndex;
  UINT8  Stack;
  SVID_IIO_PERFMON_STRUCT PerMonSvid;
  SVID_IIO_VTD_STRUCT VtSvid;
  SDID_IIO_VTD_STRUCT VtSdid;

  for ( IioIndex = 0 ; IioIndex < MaxIIO ; IioIndex++ ){
    if (!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]){
    	continue;
    }

    for (Stack =0; Stack < MAX_IIO_STACK; Stack++) {
       if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
          continue;
       }
        //
        // 4928464: BIOS needs to update SVID for dev5 Func0/6 to 0x8086
        //
        PerMonSvid.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, Stack, SVID_IIO_PERFMON_REG);
        PerMonSvid.Bits.subsystem_vendor_identification_number = 0x8086;
        IioWriteCpuCsr16(IioGlobalData, IioIndex, Stack, SVID_IIO_PERFMON_REG, PerMonSvid.Data);

        //
	    //5371441:SVID/SID registers of IIO & PCH devices are not programmed depending on PcdLockCsrSsidSvidRegister PCD
	    //
#ifndef MINIBIOS_BUILD
    if (FeaturePcdGet (PcdLockCsrSsidSvidRegister)) {
#endif //MINIBIOS_BUILD
        VtSvid.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, Stack, SVID_IIO_VTD_REG);
        VtSvid.Bits.subsystem_vendor_identification_number = 0x8086;
        IioWriteCpuCsr16(IioGlobalData, IioIndex, Stack, SVID_IIO_VTD_REG, VtSvid.Data);

        VtSdid.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, Stack, SDID_IIO_VTD_REG);
       	VtSdid.Bits.subsystem_device_identification_number = 0x0;
       	IioWriteCpuCsr16(IioGlobalData, IioIndex, Stack, SDID_IIO_VTD_REG, VtSdid.Data);
#ifndef MINIBIOS_BUILD
    }
#endif // MINIBIOS_BUILD
       	//
       	// End s5371441
       	//
    }
  }

}

/**

    Enable Misc Featues by writing to IIOMISCCTRL

    @param IioIndex       - Index to CPU/IIO
    @param IioGlobalData  - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
IioMiscFeatureInit (
  IN  UINT8                                          IioIndex,
  IN  IIO_GLOBALS                                    *IioGlobalData
  )
{
  UINT8   Stack;
  IIOMISCCTRL_N0_IIO_VTD_STRUCT      IioMiscCtrl;
  TSWCTL3_IIO_DFX_GLOBAL_STRUCT  TswCtl3;

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
      if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
          continue;
      }
      //
      //Setup the EN1K feature of IIOMISCCTRL (Register offset 0x1C0, bit 2 )
      //
      IioMiscCtrl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N0_IIO_VTD_REG);
      IioMiscCtrl.Bits.en1k = 0;
      if(IioGlobalData->SetupData.EN1K){
          IioMiscCtrl.Bits.en1k = 1;
      }
      //
      // Remote VGA enable : default for non-legacy sockets
      //
      if((IioGlobalData->SetupData.LegacyVgaSoc == IioIndex ) &&
         (IioGlobalData->SetupData.LegacyVgaStack == Stack )){
        IioMiscCtrl.Bits.rvgaen = 1;
      }
      // NOTE: We are asking user to explicitly indicate on which socket its VGA card
      // would be connected for the IIO apart from other configuration settings to
      // select onboard video device or detect the VGA device from the PCIe port.....
      IioMiscCtrl.Bits.tocmvalid = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N0_IIO_VTD_REG, IioMiscCtrl.Data);

      if (IioGlobalData->SetupData.DualCvIoFlow) {
         TswCtl3.Data = IioReadCpuCsr32(IioGlobalData, IioIndex,Stack,TSWCTL3_IIO_DFX_GLOBAL_REG);
         TswCtl3.Bits.enable_irp_intlv_iou2 = 0;
         TswCtl3.Bits.enable_irp_intlv_cbdma = 0;
         IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, TSWCTL3_IIO_DFX_GLOBAL_REG, TswCtl3.Data);
      }
  }// End of for (Stack = 0; Stack < MAX_IIO_STACK; Stack++)

}

/**

    This routine enable Problematic Port feature

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
ProgProblematicPortConfig (
  IN  IIO_GLOBALS                *IioGlobalData,
  IN UINT8                      IioIndex
  )
{
  UINT8                                   PortIndex;
  UINT16                                  AcsCtrl;
  IIOMISCCTRL_N0_IIO_VTD_STRUCT           IioMiscCtrl;
  MISCCTRLSTS_0_IIO_PCIE_STRUCT           MiscCtrlSts0;
  MISCCTRLSTS_1_IIO_PCIE_STRUCT           MiscCtrlSts1;
  UINT8                                   Stack;


  //
  // If Problematic Port functionality is disabled, do nothing
  //
  if (!IioGlobalData->SetupData.ProblematicPort) return;

  for (Stack =0; Stack < MAX_IIO_STACK; Stack++) {

      if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
        continue;
      }

      if(IioGlobalData->SetupData.ProblematicPort == 0x01){
        //
        // If NP_PROBLEMATIC flow, enable Lock Thawing Mode
        //
           IioMiscCtrl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N0_IIO_VTD_REG);
           IioMiscCtrl.Bits.lock_thaw_mode = 1;// A and B step are the same bit offset
           IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N0_IIO_VTD_REG, IioMiscCtrl.Data);
       }
   }

   for (PortIndex = 0 ; PortIndex < NUMBER_PORTS_PER_SOCKET ; PortIndex++) {
      if (!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex)){
        continue;
      }

      if (IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, VID_IIO_PCIE_REG) != 0x8086){
          continue;
      }

      //
      // 4927900: IIO: NTB setting change for acsctrl and pmcsr.power_state
      //
      // 1. if NTB Conf, skip acsctrl setting before NTB mode set via PPD. Done in NtbInit().
      //
      if (!CheckNtbPortConfSetting(IioGlobalData, IioIndex, PortIndex)){
        //
        // Force ACS functionality to disabled
        //
        AcsCtrl = 0x00;
        IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, ACSCTRL_IIO_PCIE_REG, AcsCtrl);
      }
      //
      // Program Problematic_Port_for_Lock_flows bit
      //
      MiscCtrlSts1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_1_IIO_PCIE_REG);
      MiscCtrlSts1.Bits.problematic_port_for_lock_flows = 1;


      MiscCtrlSts0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_0_IIO_PCIE_REG);
      if(IioGlobalData->SetupData.ProblematicPort == 0x01){
        //
        // If NP_PROBLEMATIC flow,  Disable P2P memory reads
        //
        MiscCtrlSts0.Bits.peer2peer_memory_read_disable = 1;
      } else {
        //
        // P_PROBLEMATIC flow, Disable P2P memory writes
        MiscCtrlSts0.Bits.rsvd_25 = 1;
      }

      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_0_IIO_PCIE_REG, MiscCtrlSts0.Data);
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_1_IIO_PCIE_REG, MiscCtrlSts1.Data);
   }
   return;
}

/**

  Determines if the PCIe slots have devices connected or not
  and disabled the link

  @param IioGlobalData  - Pointer to UDS
  @param IioIndex     - index to CPU/IIO

  @retval NONE
**/
VOID
IioDisableLinkPorts(
  IN  IIO_GLOBALS            *IioGlobalData,
  IN  UINT8                  IioIndex
  )
{
  PBUS_IIO_PCIE_STRUCT      PBUS;
  SECBUS_IIO_PCIE_STRUCT    SECBUS;
  SUBBUS_IIO_PCIE_STRUCT    SUBBUS;
  UINT8                     Bus;
  UINT8                     PortIndex;
  UINT16                    Present;
  LNKCON_IIO_PCIE_STRUCT    LnkCon;

  for(PortIndex = 1; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
    //
    // 5333019: BIOS Disables PCIe Link for Slot 2 When Hot Plug Enabled and Slot Empty At G3 Exit
    //
    // Check whether the PCIe port can be configured
    if((IioVData_ConfigurePciePort(IioGlobalData, IioIndex, (UINT8)PortIndex) == FALSE) || 
       (CheckVarPortEnable(IioGlobalData, IioIndex, PortIndex) == PCIE_PORT_DISABLE) ||
       (IioGlobalData->SetupData.SLOTHPCAP[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == TRUE)) {
      continue;
    }

    PBUS.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, PBUS_IIO_PCIE_REG);
    SECBUS.Data = IioReadCpuCsr8(IioGlobalData,IioIndex, PortIndex, SECBUS_IIO_PCIE_REG);
    SUBBUS.Data = IioReadCpuCsr8(IioGlobalData,IioIndex, PortIndex, SUBBUS_IIO_PCIE_REG);
    //
    // Check if there are EndPoint connected in the downstream buses
    //
    if (SECBUS.Bits.sbn > PBUS.Bits.pbn ){
      for (Bus = SECBUS.Bits.sbn; Bus <= SUBBUS.Bits.subordinate_bus_number; Bus++){
         //
         // Check if there is someone in the Dev 0 Fun 0 if not then there is nobody
         //
         Present = IioPciExpressRead16(IioGlobalData, IioIndex, Bus, 0, 0, 0);
         if (Present == 0xFFFF){
           LnkCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIE_REG );
           LnkCon.Bits.link_disable = 1;
           IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIE_REG, LnkCon.Data );
           IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "  No PCIe devices connected to IIO%d, PortIndex = %X Disabling link\n",IioIndex, PortIndex);
        } else {
           break;
        }
      }
    }
  }
}

/**

    This function is used as interface to initialize IIO Si features
    after Pcie Link has been trained 

    @param IIO Global Data        -  Pointer to IIO_GLOBALS

    @retval Status
    @retval IIO_SUCCESS           - Installed successfully

**/
VOID
IioLatePostLinkTrainingPhase (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex
  )
{
  IioBreakAtCheckPoint (IioGlobalData, STS_IIO_LATE_INIT_ENTRY, 0, 0xFF);

  IioInit(IioGlobalData, IioIndex);
  IioPostInit(IioGlobalData, IioIndex);

  IioBreakAtCheckPoint (IioGlobalData, STS_IIO_LATE_INIT_EXIT, 0, 0xFF);
}

/**

    This function is used as interface to initialize IIO Si features
    for DXE/SMM drivers.

    @param IIO Global Data        -  Pointer to IIO_GLOBALS

    @retval Status
    @retval IIO_SUCCESS           - Installed successfully

**/
IIO_STATUS
IioLateInitializeEntry (
  IN IIO_GLOBALS                *IioGlobalData
  )
{
  IIO_STATUS    Status = IIO_SUCCESS;
  UINT8         IioIndex;


  for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++) {
    if(!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]){
      continue;
    }
    IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR, "IioLateInitialization for Socket = %x Start..\n", IioIndex);
    IioEarlyPostLinkTrainingPhase (IioGlobalData, IioIndex);

    IioLatePostLinkTrainingPhase (IioGlobalData, IioIndex);
  }

  //
  // 5372030: [SKX H0 PO] New BIOS knob to enable WA
  //
#ifndef MINIBIOS_BUILD
  PciePhyTestMode (IioGlobalData);
#endif //MINIBIOS_BUILD

  //
  //dump link status
  //
  DumpIioPciePortPcieLinkStatus (IioGlobalData);
  return Status;
}
