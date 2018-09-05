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


  @file IioNtbInit.c

  Implement NTB port initialization  

**/

#include <SysFunc.h>
#include <IioUtilityLib.h>
#include <RcRegs.h>
#include <SysHostChip.h>
#include "IioNtbInit.h"
#ifndef IA32
#include <Library/S3BootScriptLib.h>
#endif

/**
Routine Description:
 Sets the given port Max. Payload size as per the user defined system configuration variable.
 This routine translates the PCI Port address to map correctly to port specific setup option.

Arguments:
  IioGlobalData  - Pointer to IIO_GLOBALS
  IioIndex       - index to CPU/IIO

Returns:
  None

**/
VOID
Set_NTB_MPL(
    IIO_GLOBALS   *IioGlobalData,
    UINT8         IioIndex,
    UINT8         PortIndex
)
{
  DEVCTRL_IIO_PCIE_STRUCT     DevCtrl;

  if(IioGlobalData->SetupData.PcieMaxPayload[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] != 2){
    DevCtrl.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, DEVCTRL_IIO_PCIE_REG);
    DevCtrl.Bits.max_payload_size = IioGlobalData->SetupData.PcieMaxPayload[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex];
    IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, DEVCTRL_IIO_PCIE_REG, DevCtrl.Data);
  }
}

/**

Routine Description:
 Performs any port configuration that is required before switching to NTB mode.

Arguments:
  IioGlobalData  - Pointer to IIO_GLOBALS
  IioIndex     - index to CPU/IIO

Returns:
  None

**/
VOID
PreNtbInitPortConfiguration(
    IIO_GLOBALS   *IioGlobalData,
    UINT8         IioIndex,
    UINT8         PortIndex
)
{
  //
  //5002495:Clone from BDX Sighting:[BDX-DE/EP] Max Payload Size = 256B does not set bit correctly
  //
  Set_NTB_MPL(IioGlobalData, IioIndex, PortIndex);
}

/**

    Initializes the non-transparent bridge for dev 3 / fun 0
    We configured the NTB_PPD setup option as follows:
      NTB_PPD = 0  Transparent Bridge configuration
      NTB_PPD = 1  Non Transparent Bridge to Non Transparent Bridge configuration
      NTB_PPD = 2  Non Transparent Brigde to Root Port configuration
 
      Note that in the PPD register (d3/f0/off 0D4h), bits 1:0 are used to configure the
      NTB port as follows: 
         00 = TB
         01 = NTB-NTB
         02 =  NTB-RP
     
    @param IioGlobalData     - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
NtbInit (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT8         IioIndex
  )
{
  UINT8  PPD;  //the current value of this setup option
  UINT8  Bus;
  PPD_IIO_PCIENTB_STRUCT            PpdReg;
  PBAR23SZ_IIO_PCIENTB_STRUCT           Pbar23Sz;
  PB23BASE_0_IIO_PCIENTB_STRUCT         Pb23BaseN0;
  PB23BASE_1_IIO_PCIENTB_STRUCT         Pb23BaseN1;
  PB45BASE_0_IIO_PCIENTB_STRUCT         Pb45BaseN0;
  PB45BASE_1_IIO_PCIENTB_STRUCT         Pb45BaseN1;
  PBAR45SZ_IIO_PCIENTB_STRUCT           Pbar45Sz;
  SBAR23SZ_IIO_PCIENTB_STRUCT           Sbar23Sz;
  SBAR45SZ_IIO_PCIENTB_STRUCT           Sbar45Sz;
  MISCCTRLSTS_0_IIO_PCIE_STRUCT         IioMiscCtrl;
  MISCCTRLSTS_1_IIO_PCIE_STRUCT         MiscCtrlStsN1;
  UINT8                                  NtbPortIndex;
  UINT8                                  PortIndex;
  UINT8                                  Stack;


  for (PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
    if (!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex)){
       continue;
    }
  
    if ((IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] != NTB_PORT_DEF_NTB_NTB ) &&
           (IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] != NTB_PORT_DEF_NTB_RP )) {
         continue;
    }
  
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "Initializing NTB for SKT%x Port%x\n", IioIndex, PortIndex);
  
    //5332038: We need to set this bit to 0 to enable OB parity checking
    // (This apply just for SKX; CNX will have it in pcode or ucode)
    Stack = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
    IioMiscCtrl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, MISCCTRLSTS_0_IIO_PCIENTB_REG );
    IioMiscCtrl.Bits.disable_ob_parity_check = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, MISCCTRLSTS_0_IIO_PCIENTB_REG, IioMiscCtrl.Data);
  
    //retrieve NTB PPD setup question to determine if port should be configured as Transparent Bridge, NTB to NTB, or
    //NTB to Root Port
  
    PPD = IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex];
    //
    // 5370470: When NTB ,ode is enabled, max pay load size bit is locked
    //
    if(PPD != NTB_PORT_DEF_TRANSPARENT){
      PreNtbInitPortConfiguration(IioGlobalData, IioIndex, PortIndex);
    }
    // s5370470 -end
  
    NtbPortIndex = GetNtbPortPerPortIndex(PortIndex);
    //Read in PPD register
    PpdReg.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, PPD_IIO_PCIENTB_REG);
    PpdReg.Bits.port_definition = PPD;
    if (IioGlobalData->SetupData.NtbSplitBar[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]){
      PpdReg.Data |= BIT6;
    }
    IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, PPD_IIO_PCIENTB_REG, PpdReg.Data);
  
    //now if the NTB is acting as just a TB, we don't need to do anything further in terms of the BARs...
    //else, let's now configure the sizes of the prefetchable BARs
  
    if (PPD != NTB_PORT_DEF_TRANSPARENT) { //if NTB is not configured as "TB"
  
      //djm_xxx: we wish to only set the NTB BAR Size registers if the setup option for BAR register enable indicates so.  The reason
      //is that in some usage models, we leave it up to the drivers to write these registers.  If BIOS writes these registers, this may
      //cause driver implementations issues as these registers are write once.
  
      if (IioGlobalData->SetupData.NtbBarSizeOverride[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex] == 1) {
        // HSD 3616783: IVT- EP: PCIe NTB: pb23base and pb45base not set correctly
        //              set the prefetchable BAR size registers
        // Applies to HSX
        //
  
        //
        // Set PBAR23.
        //
        Pbar23Sz.Data = 0;
        Pbar23Sz.Bits.primary_bar_2_3_size = IioGlobalData->SetupData.NtbBarSizePBar23[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]; 
        IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, PBAR23SZ_IIO_PCIENTB_REG, Pbar23Sz.Data);
  
        Pb23BaseN0.Data = 0;
        Pb23BaseN0.Bits.primary_bar_2_3_base = 0;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PB23BASE_0_IIO_PCIENTB_REG, Pb23BaseN0.Data);
  
        Pb23BaseN1.Data = 0;
        Pb23BaseN1.Bits.primary_bar_2_3_base = 0xff00;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PB23BASE_1_IIO_PCIENTB_REG, Pb23BaseN1.Data);
  
        if (!IioGlobalData->SetupData.NtbSplitBar[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]) {
          //
          // Set PBAR45.
          //
          Pbar45Sz.Data = 0;
          Pbar45Sz.Bits.primary_bar_4_5_size = IioGlobalData->SetupData.NtbBarSizePBar45[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]; 
          IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, PBAR45SZ_IIO_PCIENTB_REG, Pbar45Sz.Data);
  
          Pb45BaseN0.Data = 0;
          Pb45BaseN0.Bits.primary_bar_4_5_base = 0;
          IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PB45BASE_0_IIO_PCIENTB_REG, Pb45BaseN0.Data);
  
          Pb45BaseN1.Data = 0;
          Pb45BaseN1.Bits.primary_bar_4_5_base = 0xff00;
          IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, PB45BASE_1_IIO_PCIENTB_REG, Pb45BaseN1.Data);
        } else {
          //
          // Set PBAR4.
          //
          Pbar45Sz.Bits.primary_bar_4_5_size = IioGlobalData->SetupData.NtbBarSizePBar4[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]; 
          IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, PBAR45SZ_IIO_PCIENTB_REG, Pbar45Sz.Data);
          //
          // Set PBAR5.
          //
          Bus = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex];
          Pbar45Sz.Bits.primary_bar_4_5_size = IioGlobalData->SetupData.NtbBarSizePBar5[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]; 
          IioPciExpressWrite8 (IioGlobalData, IioIndex, \
           Bus, IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device, \
           IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function, 0xD6, Pbar45Sz.Data);
        }
        //
        // Set SBAR23.
        //
        Sbar23Sz.Data = 0;
        Sbar23Sz.Bits.secondary_bar_2_3_size = IioGlobalData->SetupData.NtbBarSizeSBar23[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]; 
        IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, SBAR23SZ_IIO_PCIENTB_REG, Sbar23Sz.Data);
  
        if (!IioGlobalData->SetupData.NtbSplitBar[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]) {
          //
          // Set SBAR45.
          //
          Sbar45Sz.Data = 0;
          Sbar45Sz.Bits.secondary_bar_4_5_size = IioGlobalData->SetupData.NtbBarSizeSBar45[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]; 
          IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, SBAR45SZ_IIO_PCIENTB_REG, Sbar45Sz.Data);
        } else {
          //
          // Set SBAR4.
          //
          Sbar45Sz.Data = (UINT8)0;
          Sbar45Sz.Bits.secondary_bar_4_5_size = IioGlobalData->SetupData.NtbBarSizeSBar4[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]; 
          IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, SBAR45SZ_IIO_PCIENTB_REG, Sbar45Sz.Data);
          
          //
          // Set SBAR5.
          //
          Bus = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex];
          Sbar45Sz.Bits.secondary_bar_4_5_size = IioGlobalData->SetupData.NtbBarSizeSBar5[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]; 
          IioPciExpressWrite8 (IioGlobalData, IioIndex, \
                               Bus, IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device, \
                               IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function, 0xD5, Sbar45Sz.Data);
        }
      }
    }
  
    //if NTB is configured as NTB-NTB...
    if (PPD == NTB_PORT_DEF_NTB_NTB) {
      //only in the NTB-NTB configuration does BIOS need to configure the crosslink control override field of the
      //PCIE Port Definition register.  According to EAS, one platform BIOS should configure the register field as
      // "USD/DSP" while the other platform BIOS should configure the register field as "DSD/USP".
  
      //a setup option has been created for specifying this value (see IIO.sd).  we will read the value of that
      //setup option into XLINK below.  The value of the setup option matches the value that should be written
      //into bits 2:3 of the PPD register.
  
      //Port bifurcation is always "wait-on-BIOS".  However a potential issue is that the crosslink control
      //option is programmed after link training is initiated.  Also has no Crosslink-control HW strap like JSP does,
      //so this is always prorammed from setup.
  
      //now read in the PPD register so that we can set bits 2:3
      PpdReg.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, PPD_IIO_PCIENTB_REG);
      PpdReg.Bits.crosslink_control = IioGlobalData->SetupData.NtbXlinkCtlOverride[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + NtbPortIndex]; 
      IioWriteCpuCsr8(IioGlobalData, IioIndex, PortIndex, PPD_IIO_PCIENTB_REG, PpdReg.Data);
    }
  
    // SERVER_BIOS_FLAG note: skip power management programming, should be taken care of standard port config.
    // SERVER_BIOS_FLAG note: skip gen1/gen2/gen3 programming, should be taken care of in standard port config.
    // SERVER_BIOS_FLAG note: skip no_forw_prog programming, appears to not be available in this architcture.
  
    if (PPD != NTB_PORT_DEF_TRANSPARENT) {
      //
      // To avoid potential deadlock issues arising from two Processors connected together which create certain
      // dependencies, disable remote p2p in the system, disable ACS on the NTB port, and mark the NTB port
      // as problematic.  Reference IIO EAS $6.4 "In/Out Dependency".
      // 
      //
  
      // disable remote peer2peer in the system.
      Stack = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
      IioMiscCtrl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N0_IIO_VTD_REG );
      //IioMiscCtrl.Bits.disable_remote_p2p_reads = 1; // A step and B step use same bit locations
      //IioMiscCtrl.Bits.disable_remote_p2p_memory_writes = 1; // A step and B step use same bit locations
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N0_IIO_VTD_REG, IioMiscCtrl.Data);
  
      // 
      // HSD 4166662: HSX Clone: Supercollider: 3-strike timeout (Ubox Lock FSM hang)
      //                         Only set B_PROBLEMATIC_PORT_FOR_LOCK_FLOW in IVT
      // HSD 4986522: HSX C0 NTB: BIOS should set NTB port as problematic(problematic_port_for_lock=1)
      //
      MiscCtrlStsN1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_1_IIO_PCIENTB_REG);
      MiscCtrlStsN1.Bits.problematic_port_for_lock_flows = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, PortIndex, MISCCTRLSTS_1_IIO_PCIENTB_REG, MiscCtrlStsN1.Data);
    } // if PPD != NTP_PORT_DEF_TRANSPARENT
  } // per-NtbPortIndex loop

} //end NtbInit

#ifndef IA32
/**

    This function will program the remain NTB settings required after PCI enumeration.

    @param RootBridgeHandle - Handle to RootBridge
    @param Address          - Pcie Address of the RootBridge
    @param PciPhase         - Pci Phase
    @param Context          - Contex of the function

    @retval None

**/
VOID
NtbLateInit (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT8         IioIndex
  )
{
  UINT64 Addr, Addr1, AddrPcmd1, AddrPcmd2;
  UINT32 Data32;
  PCICMD_IIO_PCIENTB_STRUCT   PciCmd;
  UINT8                       PortIndex;
  UINT64                      AddrTemp, Data64;

  // NTB valid for P-STACK0 to P-STACK2
  for (PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
    if (!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex)){
       continue;
    }

    if ((IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] != NTB_PORT_DEF_NTB_NTB ) &&
          (IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] != NTB_PORT_DEF_NTB_RP )) {
      continue;
    }

    //if NTB is configured as NTB-RP...
    Data32 = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PB01BASE_0_IIO_PCIENTB_REG);
    Addr = Data32;
    Addr &= ~0x1fff;
    AddrTemp = IioReadCpuCsr32(IioGlobalData, IioIndex, PortIndex, PB01BASE_1_IIO_PCIENTB_REG);
    Addr = (AddrTemp << 32) | Addr;
    Addr1 = Addr + R_PB01BASE_PBAR4XLAT;
    AddrPcmd1 = Addr + R_PB01BASE_PBARPCIPCMD;
    AddrPcmd2 = Addr + R_PB01BASE_SBARPCIPCMD;
    Addr += R_PB01BASE_PBAR2XLAT;
   
   
    //Enable the address access and transactions forwarding on the NTB port
    PciCmd.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, PCICMD_IIO_PCIENTB_REG);
    PciCmd.Bits.mse = 1;  //Memory Space Enable
    PciCmd.Bits.bme = 1;  //Bus Master Enable
    IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, PCICMD_IIO_PCIENTB_REG, PciCmd.Data);

    //
    // 5372217: NTB pcicmd setup on secondary side
    //

    //Enable the address access and transactions forwarding primary side of the NTB port
    PciCmd.Data = *(volatile UINT16 *) AddrPcmd1;
    PciCmd.Bits.mse = 1;  //Memory Space Enable
    PciCmd.Bits.bme = 1;  //Bus Master Enable
    *(volatile UINT16 *) AddrPcmd1 =  PciCmd.Data;
    S3BootScriptSaveMemWrite ( S3BootScriptWidthUint16, IioGlobalData->IioVar.IioVData.PciExpressBase + AddrPcmd1, 1, &PciCmd.Data);

    //Enable the address access and transactions forwarding secondary side of the NTB port
    PciCmd.Data = *(volatile UINT16 *) AddrPcmd2;
    PciCmd.Bits.mse = 1;  //Memory Space Enable
    PciCmd.Bits.bme = 1;  //Bus Master Enable
    *(volatile UINT16 *) AddrPcmd2 =  PciCmd.Data;
    S3BootScriptSaveMemWrite ( S3BootScriptWidthUint16, IioGlobalData->IioVar.IioVData.PciExpressBase + AddrPcmd2, 1, &PciCmd.Data);

    // end 5372217


    //Bit 3 is the prefetchable bit, with 1 = enabled
    //Memory Access Type must be 64-bit addressable
    //Programming PBAR2XLAT
    Data64 = *(volatile UINT64 *) Addr;
    // HSD: 5331333: [SKX H0 PO BIOS] NTB pcicmd and BAR setup
    if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) &&
        (IioGlobalData->IioVar.IioVData.CpuStepping < H0_REV_SKX)) {
        Data64 = PBAR2XLAT_MBA_Default;
    }
    Data64 |= B_SB01BASE_PREFETCHABLE;
    Data64 |= B_PBAR2XLAT_Type_Mem64;
   
    *(volatile UINT64 *) Addr =  Data64;
    S3BootScriptSaveMemWrite ( S3BootScriptWidthUint64, IioGlobalData->IioVar.IioVData.PciExpressBase + Addr, 1, &Data64);
    //Programming PBAR4XLAT
    Data64 = *(volatile UINT64 *) Addr1;
    // HSD: 5331333: [SKX H0 PO BIOS] NTB pcicmd and BAR setup
    if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) &&
        (IioGlobalData->IioVar.IioVData.CpuStepping < H0_REV_SKX)) {
        Data64 = PBAR4XLAT_MBA_Default;
    }
    Data64 |= B_SB01BASE_PREFETCHABLE;
    Data64 |= B_PBAR2XLAT_Type_Mem64;
   
    *(volatile UINT64 *) Addr1 =  Data64;
    S3BootScriptSaveMemWrite ( S3BootScriptWidthUint64, IioGlobalData->IioVar.IioVData.PciExpressBase + Addr1, 1, &Data64);
   
  } // per-NtbPortIndex loop
} //end NtbInitAfterResources


#endif //IA32
