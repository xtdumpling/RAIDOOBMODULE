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


  @file IioVtdInit.c

  This file initialize the Vtd registers and report DMAR table

**/
#include <SysFunc.h>
#include <IioUtilityLib.h>
#include <RcRegs.h>
#include <SysHostChip.h>
#include "IioVtdInit.h"

/**
    This routine is primarily to apply PreEnable VTD Workarounds

    @param IioGlobalData  - Pointer to IIO_GLOBALS
    @param IioIndex       - Index to CPU/IIO

    @retval None

**/
VOID
IioVtDPreEnWorkAround  (
  IN  IIO_GLOBALS                                         *IioGlobalData,
  IN  UINT8                                               IioIndex
)
{
  VTDPRIVC0_IIO_DFX_VTD_STRUCT        VTdPrivC0;
  XPDFXSPAREREG_IIO_DFX_STRUCT    XpDfxSpareReg;
  IIMI_ASC0LDVALLO_IIO_DFX_GLOBAL_STRUCT  IimiAsColdValLo;
  IIMI_ASC0LDVALHI_IIO_DFX_GLOBAL_STRUCT  IimiAsColdValHi;
  IIMI_ASCCONTROL_IIO_DFX_GLOBAL_STRUCT   IimoAsCControl;
  ITCRSPFUNC_IIO_DFX_GLOBAL_STRUCT        ItCrsPfunc;
  ITCARBBLOCK_IIO_DFX_GLOBAL_STRUCT       ItCarBblock;
  UINT8                               Stack;

  for (Stack = 0; Stack< MAX_IIO_STACK; Stack++) {
      if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
          continue;
      }
  //
  // All HSX Workarounds related to VTd are below
  // ____________________________________________
  //
      if (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX){
       //
       // HSD 4166366: [HSX_A0_PO] CLONE from HSX: PUSH from ivytown: With the VT-d denial of
       //              service fix enabled the PCIE TXN Layer does not return posted data credits
       //
       VTdPrivC0.Data =  IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, VTDPRIVC0_IIO_DFX_VTD_REG);
       VTdPrivC0.Bits.ud &= ~(BIT3 | BIT4);

       //
       //4167999: VT-d ATS faults (ITE+ICE) when protected memory regions enabled
       //4986774: HSX Clone: VT-d ATS faults (ITE+ICE) when protected memory regions enabled
       //         Timeout increased from  256us (0x01) <set in 4167999> to 1s (0x02)
       //
       VTdPrivC0.Bits.tlbcmpto = 0x02;
       IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, VTDPRIVC0_IIO_DFX_VTD_REG, VTdPrivC0.Data);


       //
       // Port based HSX workarounds related to VTd below
       //
       //
       // HSD 4166366 (continued): [HSX_A0_PO] CLONE from HSX: PUSH from ivytown: With the VT-d denial of
       //              service fix enabled the PCIE TXN Layer does not return posted data credits
       //
       XpDfxSpareReg.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, XPDFXSPAREREG_IIO_DFX_REG);
       XpDfxSpareReg.Bits.disable_msi_gt_1dw = 1;
       IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, XPDFXSPAREREG_IIO_DFX_REG, XpDfxSpareReg.Data);
       //
       // 5332547: [SKX B0 PO] CLONE SKX Sighting: Hang with PCIE transactions stuck in waiting for VTd translation(VTdF)  in ITC
       //
       if (IioGlobalData->IioVar.IioVData.CpuStepping < B0_REV_SKX) {
         IimiAsColdValHi.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIMI_ASC0LDVALHI_IIO_DFX_GLOBAL_REG);
         IimiAsColdValHi.Bits.ldhival = 100; // To block Vtd pipes for 100 cycles
         IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIMI_ASC0LDVALHI_IIO_DFX_GLOBAL_REG,IimiAsColdValHi.Data);

         IimiAsColdValLo.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack,IIMI_ASC0LDVALLO_IIO_DFX_GLOBAL_REG);
         IimiAsColdValLo.Bits.ldlowval = 100000; // Unblock Vtd pipes for 10000 cycles.
         IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack,IIMI_ASC0LDVALLO_IIO_DFX_GLOBAL_REG, IimiAsColdValLo.Data);

         IimoAsCControl.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, Stack, IIMI_ASCCONTROL_IIO_DFX_GLOBAL_REG);
         IimoAsCControl.Bits.asc0en = 1; // ASC0s enabled (Artificial Startvation Counter 0)
         IioWriteCpuCsr16(IioGlobalData, IioIndex, Stack, IIMI_ASCCONTROL_IIO_DFX_GLOBAL_REG,IimoAsCControl.Data);

         ItCrsPfunc.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, ITCRSPFUNC_IIO_DFX_GLOBAL_REG);
         ItCrsPfunc.Bits.startsel0 = 4; // ASC0
         ItCrsPfunc.Bits.stopsel0 = 0xc; // When start is false
         IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, ITCRSPFUNC_IIO_DFX_GLOBAL_REG,ItCrsPfunc.Data);

         ItCarBblock.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, ITCARBBLOCK_IIO_DFX_GLOBAL_REG);
         ItCarBblock.Bits.enable = 2; // Block based on RspFunc0
         ItCarBblock.Bits.typemask = 7; //  Block VTd pipes
         IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, ITCARBBLOCK_IIO_DFX_GLOBAL_REG,ItCarBblock.Data);
      } // SKX < B0_REV_SKX
    } // SKX
  }//End of for (Stack = 0; Stack< MAX_IIO_STACK; Stack++)
}

/**

    This routine is primarily to apply all Vtd Enabling Workarounds


    @param IioGlobalData  - Pointer to IIO_GLOBALS
    @param IioIndex       - Index to CPU/IIO
    @param  Stack         - Index to Stack
    @retval None

**/
VOID
IioVtdEnWorkAround  (
   IN IIO_GLOBALS                                         *IioGlobalData,
   IN UINT8                                               IioIndex,
   IN UINT8                                               Stack
)
{
  VTGENCTRL2_IIO_VTD_STRUCT       VTdGenCtrl2;
  VTDPRIVC0_IIO_DFX_VTD_STRUCT    VTdPrivC0;
  VTGENCTRL3_IIO_VTD_STRUCT       VTdGenCtrl3;

  /// s4985923 ECO needed for all steppings of CPU_HSX
  if (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX)  {
     //VT-d related ECO for A0 stepping
     // ivt_eco: s4572807: Hang in VTd due to busy bit not getting set in retry response
     //  [cloned to 4030375]
     VTdGenCtrl2.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, VTGENCTRL2_IIO_VTD_REG);
     //
     // HSD 4986149 HSX Clone: PCIE: Rocket CATERR with enable_extended_vtstf=1
     //
     VTdGenCtrl2.Bits.en_lock_rsvd_ent = 0; // Disables standard method of reserving entries in the TLB

     //
     // HSD 4166665: HSX Clone: ESCALATE from hexaii_hsx_mock:VT-d LRUctrl set incorrectly (flexconVTd cbVTdJKT3876632wa check)
     //
     VTdGenCtrl2.Bits.lructrl = 0;
     IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, VTGENCTRL2_IIO_VTD_REG, VTdGenCtrl2.Data);

     //to force periodic switch retry
     VTdPrivC0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, VTDPRIVC0_IIO_DFX_VTD_REG);

     VTdPrivC0.Bits.fpsr = 1;

     IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, VTDPRIVC0_IIO_DFX_VTD_REG, VTdPrivC0.Data);

     //
     // HSD 4986149 HSX Clone: PCIE: Rocket CATERR with enable_extended_vtstf=1
     //
     VTdGenCtrl3.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, VTGENCTRL3_IIO_VTD_REG);
     VTdGenCtrl3.Bits.en_agt_rsvd_p_f_ent_tlb0 = 0x3f; //These two reserve posted entries in TLB for each port.
     VTdGenCtrl3.Bits.en_agt_rsvd_p_f_ent_tlb1 = 0x1ff;
     IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, VTGENCTRL3_IIO_VTD_REG, VTdGenCtrl3.Data);
  }
}

#ifndef MINIBIOS_BUILD
/**

    FPGA VTD initialization on specific socket

    @param Phase         - VTD initialization phase 1-3
    @param IioGlobalData - pointer to IioGlobalData structure
    @param IioIndex      - IIO index to be processed

    @retval EFI_SUCCESS  - retuen EFI success
               EFI_UNSUPPORTED  - retuen Not Supported

**/


IIO_STATUS
FpgaVtdInitialization (
  IN  IIO_GLOBALS                                         *IioGlobalData,
  IN  UINT8                                               IioIndex,
  IN  UINT8                                               Stack
  )
{
  VTGENCTRL_IIO_VTD_STRUCT   VtGenCtrl;
  
  if  (( Stack != IIO_PSTACK3)  || (!IioGlobalData->SetupData.VTdSupport)) {
    return IIO_UNSUPPORTED;
  }

  //
  // Check FPGA active
  //
  if (IioGlobalData->IioVar.IioVData.FpgaActive[IioIndex]) {
    //
    // Disable Vtd support in IIO MCP0 for FPGA SKU
    //
    VtGenCtrl.Data  = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, VTGENCTRL_IIO_VTD_REG);
    VtGenCtrl.Bits.lockvtd = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, VTGENCTRL_IIO_VTD_REG, VtGenCtrl.Data);
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Vt-D support is disabled in MCP0 for FPGA SKU. \n");
    
    return IIO_SUCCESS;
  }
  
  return IIO_UNSUPPORTED;

}
#endif //MINIBIOS_BUILD

/**

    IIO VTD initialization on specific socket


    @param IioGlobalData - pointer to IioGlobalData structure
    @param IioIndex      - IIO index to be processed

    @retval EFI_SUCCESS  - retuen EFI success

**/
IIO_STATUS
VtdInitialization (
    IN IIO_GLOBALS                                         *IioGlobalData,
    IN UINT8                                               IioIndex
    ) 
{
  IIO_PTR_ADDRESS     VtdMmioCap;
  IIO_PTR_ADDRESS     VtdMmioExtCap;
  IIO_PTR_ADDRESS     Vtd1MmioCap;
  IIO_PTR_ADDRESS     Vtd1MmioExtCap;
  UINT32              VtdMemoryMap;
  UINT32              VtdBase;
  IIO_PTR_ADDRESS     VtdIntReMapTable;

  UINT8             Stack;
#ifndef MINIBIOS_BUILD
  IIO_STATUS        Status;
#endif // MINIBIOS_BUILD

  VTGENCTRL_IIO_VTD_STRUCT   VtGenCtrl;
  VTISOCHCTRL_IIO_VTD_STRUCT VtdIsoCtrl;
  IIOMISCCTRL_N1_IIO_VTD_STRUCT IioMiscCtrlN1;

  if (!IioGlobalData->SetupData.VTdSupport) {
    for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
      // Check for a valid stack 
      if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
         continue;
      }
      VtGenCtrl.Data  = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, VTGENCTRL_IIO_VTD_REG);
      VtGenCtrl.Bits.lockvtd = 1;
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, VTGENCTRL_IIO_VTD_REG, VtGenCtrl.Data);
    }  
    IioDebugPrintInfo ( IioGlobalData, IIO_DEBUG_INFO, "Vt-D support is disabled. \n");
    return IIO_UNSUPPORTED;
  }

  IioDebugPrintInfo ( IioGlobalData, IIO_DEBUG_INFO, "VT-d Chipset Initialization for IIO%d ...\n", IioIndex);

  IioVtDPreEnWorkAround(IioGlobalData, IioIndex);
  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    // Check for a valid stack 
    if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
      continue;
    }

#ifndef MINIBIOS_BUILD
      // Check for SKX-P FPGA
      Status = FpgaVtdInitialization (IioGlobalData, IioIndex, Stack);
      if (Status == IIO_SUCCESS){
        continue;
      }

#endif // MINIBIOS_BUILD

    VtdBase = (UINT32)IioGlobalData->IioVar.IioVData.VtdBarAddress[IioIndex][Stack];
    if (VtdBase){


      IioVtdEnWorkAround(IioGlobalData, IioIndex, Stack);
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack,VTBAR_IIO_VTD_REG, (VtdBase | B_IIO_VT_BAR_EN));

      VtdIsoCtrl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, VTISOCHCTRL_IIO_VTD_REG);

      //
      // Ensure that Vcp is off for Azalia since Skx does not support Vcp 
      //
      VtdIsoCtrl.Bits.azalia_on_vcp = 0; //  &= ~(B_IIO_VTISOCHCTRL_AZALIA_ON_VCP);
      //
      //  NonIsoc GPAand HPA limits
      // 
      VtGenCtrl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, VTGENCTRL_IIO_VTD_REG);

      VtGenCtrl.Bits.lockvtd = 1;
      VtGenCtrl.Bits.hpa_limit = V_IIO_VTCTRL_VTHPAL_46BIT;
      VtGenCtrl.Bits.gpa_limit = V_IIO_VTCTRL_VTNGPAL_48BIT;

      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, VTGENCTRL_IIO_VTD_REG, VtGenCtrl.Data);
        
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, VTISOCHCTRL_IIO_VTD_REG, VtdIsoCtrl.Data);
        
      IioDebugPrintInfo ( IioGlobalData, IIO_DEBUG_INFO, "Stack[%x] Vt-D base address      :  0x%08X\n", Stack, VtdBase);
      IioDebugPrintInfo ( IioGlobalData, IIO_DEBUG_INFO, "Stack[%x] VtDGenCtrlReg          :  0x%08X\n", Stack, VtGenCtrl.Data);
      IioDebugPrintInfo ( IioGlobalData, IIO_DEBUG_INFO, "Stack[%x] VtDIsoCtrlReg          :  0x%08X\n", Stack, VtdIsoCtrl.Data);
        
      //
      // Non Isoc engine Capabilities and Ext capabilities
      //
        
      VtdMemoryMap      = VtdBase;
      VtdMmioCap.Address32bit.Value = IioMmioRead32(IioGlobalData,(VtdMemoryMap + R_VTD_CAP_LOW));
        
      //
      // BIT23=0 for other than Isoch VT-d MMMIO.
      //
      VtdMmioCap.Address32bit.Value &= ~(UINT64)BIT23;
        
      //
      // ZLR support enable BIT22 = 1
      //
      VtdMmioCap.Address32bit.Value |= BIT22;

      IioMmioWrite32(IioGlobalData,(VtdMemoryMap + R_VTD_CAP_LOW),VtdMmioCap.Address32bit.Value);

      //
      // 4927064:Provide BIOS Setup enable/disable of capabilites bit to enable or disable posted interrupts
      // VTBar offset 0x08, vtd0_cap, bit 59: when set allows Posted Interrupts to be set up.
      //
      VtdMmioCap.Address32bit.ValueHigh = IioMmioRead32(IioGlobalData,(VtdMemoryMap + R_VTD_CAP_HIGH));
      if (IioGlobalData->SetupData.PostedInterrupt) {
        VtdMmioCap.Address32bit.ValueHigh   |= BIT27;  // BIT27 is the same as BIT59, since we are reading High DWORD of 64bit address.
      }
        
      IioMmioWrite32(IioGlobalData,(VtdMemoryMap + R_VTD_CAP_HIGH),VtdMmioCap.Address32bit.ValueHigh);

      VtdMmioExtCap.Address32bit.Value = IioMmioRead32(IioGlobalData,(VtdMemoryMap + R_VTD_EXT_CAP_LOW));
        
      //
      // Set Coherency for Non-Isoch Vtd
      // 
      VtdMmioExtCap.Address32bit.Value &= ~(BIT0);
      if (IioGlobalData->SetupData.CoherencySupport) {
        VtdMmioExtCap.Address32bit.Value   |= BIT0;
      }
        
      VtdMmioExtCap.Address32bit.Value   &= ~(BIT2);
      if (IioGlobalData->SetupData.ATS) {
        VtdMmioExtCap.Address32bit.Value   |= BIT2;
      }
        
      VtdMmioExtCap.Address32bit.Value   &= ~(BIT6);
      if (IioGlobalData->SetupData.PassThroughDma) {
        VtdMmioExtCap.Address32bit.Value   |= BIT6;
      }
        
      VtdMmioExtCap.Address32bit.Value   &= ~(BIT3);
      if (IioGlobalData->SetupData.InterruptRemap) {
        VtdMmioExtCap.Address32bit.Value   |= BIT3;
      }
        
      IioMmioWrite32(IioGlobalData,(VtdMemoryMap + R_VTD_EXT_CAP_LOW), VtdMmioExtCap.Address32bit.Value);

      VtdMmioExtCap.Address32bit.ValueHigh = IioMmioRead32(IioGlobalData,(VtdMemoryMap + R_VTD_EXT_CAP_HIGH));

      IioDebugPrintInfo ( IioGlobalData, IIO_DEBUG_INFO, "Non-Iso Engine CapReg     :  0x%X%X\n", VtdMmioCap.Address32bit.ValueHigh, VtdMmioCap.Address32bit.Value);
      IioDebugPrintInfo ( IioGlobalData, IIO_DEBUG_INFO, "Non-Iso Engine ExtCapReg  :  0x%X%X\n", VtdMmioExtCap.Address32bit.ValueHigh, VtdMmioExtCap.Address32bit.Value);
        
      //
      // Program Isoch Capability and ExtCapabilities
      //
        
      Vtd1MmioCap.Address32bit.Value = IioMmioRead32(IioGlobalData,(VtdBase + R_VTD1_CAP_LOW));
      Vtd1MmioExtCap.Address32bit.Value = IioMmioRead32(IioGlobalData,(VtdBase + R_VTD1_EXT_CAP_LOW));
        
      //
      // Set Coherency for Isoch Vtd
      // 
      Vtd1MmioExtCap.Address32bit.Value &= ~(BIT0);
      if (Vtd1MmioCap.Address32bit.Value & BIT23) {
        Vtd1MmioExtCap.Address32bit.Value   |= BIT0;
      } 
        
      //
      // Write back updated Vtd1MmioExtCap register
      // 
      IioMmioWrite32(IioGlobalData,(VtdBase + R_VTD1_EXT_CAP_LOW), Vtd1MmioExtCap.Address32bit.Value);

      // 5330960 : Cloned From SKX Si Bug Eco: CLONE SKX Sighting: interrupt remapping cannot be enabled in x2APIC
      // mode (ia32_extended_interrupt_enable bit is locked)
      if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX)  &&
          (IioGlobalData->IioVar.IioVData.CpuStepping < B0_REV_SKX)){
          //
          // Temporarily Unlock mmio access to modify R_VTD0_INTR_REMAP_TABLE_BASE and R_VTD1_INTR_REMAP_TABLE_BASE
          //
          IioMiscCtrlN1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG);
          IioMiscCtrlN1.Bits.mmiolock = 0x0;
          IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG, IioMiscCtrlN1.Data);

          VtdIntReMapTable.Address32bit.Value = IioMmioRead32(IioGlobalData,(VtdBase + R_VTD0_INTR_REMAP_TABLE_BASE));
          VtdIntReMapTable.Address32bit.Value &= ~B_IA32_EXT_INT_ENABLE;
          if (IioGlobalData->SetupData.ProcessorX2apic){
            VtdIntReMapTable.Address32bit.Value |=  B_IA32_EXT_INT_ENABLE;
          }
          IioMmioWrite32(IioGlobalData,(VtdBase + R_VTD0_INTR_REMAP_TABLE_BASE), VtdIntReMapTable.Address32bit.Value);

          //
          // Lock mmio access again
          //
          IioMiscCtrlN1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG);
          IioMiscCtrlN1.Bits.mmiolock = 1;
          IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG, IioMiscCtrlN1.Data);
      }
   } // End if VtdBase
  } //End of for (Stack = 0; Stack < MAX_IIO_STACK; Stack++)
  return IIO_SUCCESS;
}
