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


  @file IioIoat.c

  Houses all code related to Uncore specific CPU IIO initialization 

Revision History:

**/
#include <SysFunc.h>
#include <IioUtilityLib.h>
#include <RcRegs.h>
#include <Library/S3BootScriptLib.h>
#include "IioIoatInit.h"
/**

  Helper routine which returns the PCI header class code value for the given BDF

  @param IioGlobalData - IIO Global data structure
  @param IioIndex      - Socket Index
  @param Function      - CBDMA function

  @retval Class code value from CBDMA

**/
UINT32
GetCBDMAClassCode (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         Function
  )
{
  UINT32  Register = 0;
  UINT32 ClassCode;

  Register = CCR_N0_IIOCB_FUNC0_REG | (Function << 16);
  ClassCode = (UINT32)IioReadCpuCsr8(IioGlobalData, IioIndex, IIO_CSTACK, Register);
  Register = CCR_N1_IIOCB_FUNC0_REG | (Function << 16);
  ClassCode |= (UINT32)(IioReadCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, Register) << 8);
  return ClassCode;
}

/**

  Hook to check whether the BDF given belongs to Intel's CBDMA device

  @param IioGlobalData         - IIO Global data structure
  @param IioIndex              - Socket Index
  @param Function              - CBDMA Function value

  @retval  TRUE if valid, else FALSE

**/
BOOLEAN
IdentifyCbDmaDevice (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         Function
  )
{

  if(Function == 0xFF)
    return FALSE;

  //
  // if Class code matches the Other system peripheral device  base class (08), subclass (80h),
  // Reg. Prog interface(0), then it is the regular CBDMA device
  //
  if(GetCBDMAClassCode(IioGlobalData, IioIndex, Function) == V_PCIE_CCR_PCI_OTHER_SYS_PERIPHERAL){
    return TRUE;              //return TRUE to indicate that the PCI address belongs to CBDMA device
  } else {
    return FALSE;             //return FALSE as it does not belong to CBDMA
  }
}

/**

  Returns the current value of the CDMBA BAR 

  @param IioGlobalData         - IIO Global data structure
  @param IioIndex              - Socket Index
  @param Function              - CBDMA Function value

  @retval  CBBAR               - UINT64 value 

**/
UINT64
GetActualCbBAR (
  IN  IIO_GLOBALS                                    *IioGlobalData,
  IN  UINT8                                          IioIndex,
  IN  UINT8                                          Function
  )
{
  UINT32   Offset;
  UINT64  CbBar;

  //
  // Determine the CB version from the CB DMA MMIO config space
  //
  Offset = CB_BAR_0_IIOCB_FUNC0_REG | (Function <<16);
  CbBar = ((UINT64)IioReadCpuCsr32(IioGlobalData, IioIndex,IIO_CSTACK, Offset)) & ~0x0f;
  Offset = CB_BAR_1_IIOCB_FUNC0_REG | (Function << 16);
  CbBar |= ((UINT64)IioReadCpuCsr32(IioGlobalData, IioIndex,IIO_CSTACK, Offset)) << 32;

  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR, "GetActualCBBar: IioIndex: %d, Function: %d, CBBar: 0x%lx\n", IioIndex, Function, CbBar);

  return CbBar;
}

/**

  This function returns the CDMBA version supported in Si 

  @param IioGlobalData         - IIO Global data structure
  @param IioIndex              - Socket Index
  @param Function              - CBDMA Function value

  @retval  CBDMA version value

**/
UINT8
Get_CB_version (
  IN  IIO_GLOBALS                                     *IioGlobalData,
  IN  UINT8                                           IioIndex,
  IN  UINT8                                           Func,
  IN  UINT64                                          CbBar
  )
{
  UINT32                      Offset;
  PCICMD_IIOCB_FUNC0_STRUCT   OrgPciCmd, PciCmd;
  UINT8                       CbVer;
  //
  //
  // Retrieve the CB version from the device
  //
  //  (0) Calculate the proper PCICMD_IIOCB_FUNCx_REG.
  //      The address of the PCICMD_IIOCB_FUNCx_REG is:
  //      0x160f2004, where "f" represents the function number.

  Offset = PCICMD_IIOCB_FUNC0_REG | (Func <<16) ;

  //(1) - save the CB PCI CMD register
  PciCmd.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, Offset);
  OrgPciCmd.Data = PciCmd.Data;

  //(2) - enable the CB configuration MMIO space
  PciCmd.Bits.mse = 1;
  // This should not be set on S3 script, so we use WriteCpuCsr rather than IioWriteCpuCsr.
  IioWriteCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, Offset, PciCmd.Data);

  //{3} - read CBVER register from the CB MMIO space
  CbVer = *(volatile UINT8 *)((UINTN)(CbBar + R_CB_BAR_CBVER));

  //(4) - restore the CB device state
  IioWriteCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK,Offset,OrgPciCmd.Data);

  return CbVer;
}

/**

  This funtion will verify if DMA is supported in Si 

  @param IioGlobalData         - IIO Global data structure
  @param IioIndex              - Socket Index
  @param Function              - CBDMA Function value

  @retval  TRUE if valid, else FALSE

**/
BOOLEAN
DmaFunctionExist (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         Function
  )
{
  UINT32 Offset;
  /// Calculate the offset of CSR based on current function.
  Offset = VID_IIOCB_FUNC0_REG | (Function <<16);
  if (IioReadCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, Offset)!=0x8086){
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Initialize CBDMA structure with the default values
  
  @param IioGlobalData       pointer to the IIO_GLOBALS structure
  @param IioIndex            Socket Index
  @param PciAddress          Device address in BDF format
  
  @retval None
 */
VOID
InitializeCbDmaType (
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  PCI_ROOT_BRIDGE_PCI_ADDRESS    PciAddress
  )
{
  //
  // Initialize CB_VER based on CPU type & SKU
  //
  if(IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX){
     IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CB_VER = V_CB33_VERSION;
  } else { // Unkown
     IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CB_VER = V_CB32_VERSION;
  }

  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO,"CBDMA version=0x%x\n", IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CB_VER);

  //
  // Initialize  the PCI address and number of channels
  //
  IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.BusNo = PciAddress.Bus;
  IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.DevNo = PciAddress.Device;
  IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.FunNo = PciAddress.Function;
  if((IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CB_VER == V_CB32_VERSION) ||
     (IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CB_VER == V_CB33_VERSION)){
    if (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX){
        IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.MaxNoChannels = IOAT_TOTAL_FUNCS;
    } else {
       IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.MaxNoChannels = 0;//unknown
    }
  }

  //
  // Based on the CBDMA version initialize the  capability features
  //
  if((IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CB_VER == V_CB32_VERSION) ||
    (IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CB_VER == V_CB33_VERSION)){
    if(IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX){
      IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CBConfigCap.DcaSupported = TRUE;
      IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CBConfigCap.NoSnoopSupported = TRUE;
      IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CBConfigCap.RelaxOrderSupported = TRUE;
    } else {
      // Unknown
      IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CBConfigCap.DcaSupported = FALSE;
      IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CBConfigCap.NoSnoopSupported = FALSE;
      IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CBConfigCap.RelaxOrderSupported = FALSE;
    }
  }

}

VOID
IioIoatWorkarounds(
  IN  IIO_GLOBALS                     *IioGlobalData,
  IN  UINT8                           IioIndex
  )
{
 DMACHICKEN0_IIO_DFX_VTD_DMI_STRUCT DmaChicken0;

  DmaChicken0.Data  = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_CSTACK, DMACHICKEN0_IIO_DFX_VTD_DMI_REG);
  DmaChicken0.Bits.byte2  = 4;
  DEBUG((DEBUG_ERROR, "DmaChicken0.Data = %x\n",DmaChicken0.Data));
  IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_CSTACK, DMACHICKEN0_IIO_DFX_VTD_DMI_REG, DmaChicken0.Data);

}


/**

    Detects if CPU is DCA capable. Frontend for DetectCpuIdDcaCapable

    @param None

    @retval True: CPU is DCA capable, False: CPU is not DXA Capable

**/
BOOLEAN
DetectCpuDcaCapable (
  IN  IIO_GLOBALS    *IioGlobalData,
  IN  UINT8          IioIndex
  )
{  
    CAPID1_PCU_FUN3_STRUCT CapId1;
    BOOLEAN                DcaCapable;

    CapId1.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, CAPID1_PCU_FUN3_REG);

    // This bit is enabled by PCODE, BIOS only check it
    if((CapId1.Bits.tph_en)){
      IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Core DCA Cap is enabled !\n");
      DcaCapable = TRUE;
    } else{
      IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO,"Core DCA Cap is disabled !\n");
      DcaCapable = FALSE;
    }
    return  DcaCapable;
}


VOID
RwoRegisterProgramming (
  IN  IIO_GLOBALS                                 *IioGlobalData,
  IN  UINT8                                       IioIndex,
  IN  PCI_ROOT_BRIDGE_PCI_ADDRESS                 PciAddress
  )
{
  UINT32                      Offset;
  INTPIN_IIOCB_FUNC0_STRUCT   IntPinReg;

  Offset = INTPIN_IIOCB_FUNC0_REG | (PciAddress.Function << 16);
  // write back read value to lock RW-O INTPIN
  IntPinReg.Data = IioReadCpuCsr8(IioGlobalData, IioIndex,IIO_CSTACK, Offset);
  IioWriteCpuCsr8(IioGlobalData, IioIndex, IIO_CSTACK, Offset, IntPinReg.Data);

}


/**

    IOAT initialization routine

    @param PciAddress    - PCI address of the initialized IOAT
    @param Phase         - Phase where this routine was called
    @param IioGlobalData - Pointer to IIO Global information.

    @retval None

**/
VOID
IioIoatInit (
  IN  IIO_GLOBALS                                   *IioGlobalData,
  IN  UINT8                                         IioIndex,
  IN  PCI_ROOT_BRIDGE_PCI_ADDRESS                   PciAddress
  )
{

  BOOLEAN                             EnableDCA;
  UINT8                               DeviceNum, FunctionNum;
  UINT8                               Bus;
  UINT16                              Value16;
  UINT32                              Value32;
  UINT64                              CbBar;
  UINTN                               Addr;
  PCICMD_IIOCB_FUNC0_STRUCT           CmdReg;
  PCICMD_IIOCB_FUNC0_STRUCT           CmdRegOrg;
  DEVCFG_IIOCB_FUNC0_STRUCT           DevCfg;
  DEVCON_IIOCB_FUNC0_STRUCT           DevCon;
  UINT64                              MmcfgBase;

  Bus         = PciAddress.Bus;
  FunctionNum = PciAddress.Function;
  DeviceNum   = PciAddress.Device;

  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, " IIO IOAT Init B:%x, D:%x F:%x\n", Bus, DeviceNum, FunctionNum);
  CbBar = GetActualCbBAR(IioGlobalData,IioIndex, FunctionNum);

  if (CbBar){
    // read CB version
    IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CB_VER =
    Get_CB_version( IioGlobalData, IioIndex, PciAddress.Function,  CbBar);
  }

  MmcfgBase = (UINT64)(IioGlobalData->IioVar.IioVData.SegMmcfgBase[IioIndex].hi);
  MmcfgBase = MmcfgBase << 32;
  MmcfgBase |= (UINT64)(IioGlobalData->IioVar.IioVData.SegMmcfgBase[IioIndex].lo);

  //
  // Save 64-bit CbBar for S3 resume restore
  //
  Addr = (UINTN)(MmcfgBase + PCI_PCIE_ADDR(Bus, DeviceNum, FunctionNum, ONLY_REGISTER_OFFSET(CB_BAR_1_IIOCB_FUNC0_REG)));
  Value32 = (UINT32)(CbBar >> 32);
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, Addr, 1, &Value32);

  Addr = (UINTN)(MmcfgBase + PCI_PCIE_ADDR(Bus, DeviceNum, FunctionNum, ONLY_REGISTER_OFFSET(CB_BAR_0_IIOCB_FUNC0_REG)));
  Value32 = (UINT32)CbBar;
  S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, Addr, 1, &Value32);

  //IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "IIO IOAT Init After Resource Assigned.... !\n"));
        
  //
  // Program DMA requirements based on setup questions
  //

  CmdReg.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK,
                         (PCICMD_IIOCB_FUNC0_REG | FunctionNum <<16));
  CmdRegOrg.Data = CmdReg.Data;
  //
  // 4927142: IIO Online - RAS (Legacy) [From Brickland]
  //
  if ((IioGlobalData->SetupData.Cb3DmaEn[(IioIndex * IOAT_TOTAL_FUNCS) + FunctionNum]) || (IioGlobalData->IioVar.IioVData.RasOperation)) {
    CmdReg.Bits.bme = 0x01;
  } else {
    CmdReg.Bits.bme = 0x00;
  }

  IioWriteCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, PCICMD_IIOCB_FUNC0_REG, CmdReg.Data);

  if(IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CBConfigCap.NoSnoopSupported == TRUE){
    //
    // This bit enable is shared for all DMA engines and resides in Func 0 only
    //
    if (FunctionNum == 0x00 &&
       IdentifyCbDmaDevice(IioGlobalData, IioIndex, FunctionNum) == TRUE) {

      DevCfg.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, DEVCFG_IIOCB_FUNC0_REG);
      if (IioGlobalData->SetupData.Cb3DmaEn[(IioIndex * IOAT_TOTAL_FUNCS) + FunctionNum]){
        DevCfg.Bits.enable_no_snoop = 0x01; // A and B step are same bit
      } else {
        DevCfg.Bits.enable_no_snoop = 0x00;
      }

      IioWriteCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, DEVCFG_IIOCB_FUNC0_REG, DevCfg.Data);
    }

    //
    // Setup DMA no-snoop based on setup questions
    //
    DevCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK,
                         ( DEVCON_IIOCB_FUNC0_REG | (FunctionNum << 16)));

    if (IioGlobalData->SetupData.Cb3NoSnoopEn[(IioIndex * IOAT_TOTAL_FUNCS) + FunctionNum]) {
        DevCon.Bits.enable_no_snoop = 0x01;
    } else {
        DevCon.Bits.enable_no_snoop = 0x00;
    }

    IioWriteCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, 
                     ( DEVCON_IIOCB_FUNC0_REG | (FunctionNum << 16)), DevCon.Data);
  } // NoSnoopSupported End

  if(IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CBConfigCap.RelaxOrderSupported == TRUE){

    DevCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK,
                         ( DEVCON_IIOCB_FUNC0_REG | (FunctionNum << 16)));
    //
    // HSD 3612867: defeature CBDMA relaxed ordering
    // HSD 3612733: Getting completion for a strongly ordered txn which does not exist!
    //
    if (IioGlobalData->SetupData.CbRelaxedOrdering) {
      DevCon.Bits.enable_relaxed_ordering = 1;
    } else {
      DevCon.Bits.enable_relaxed_ordering = 0;
    }
    IioWriteCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, 
                     ( DEVCON_IIOCB_FUNC0_REG | (FunctionNum << 16)), DevCon.Data);
  }// RelaxedOrderSupported End


  if (IioGlobalData->IioVar.IioOutData.DMAhost[IioIndex].CbDmaDevice.CBConfigCap.DcaSupported == TRUE) {

    if(CbBar){
      //
      // Read command register
      //
      CmdReg.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK,
                             (PCICMD_IIOCB_FUNC0_REG | (FunctionNum << 16)));
      CmdRegOrg.Data = CmdReg.Data;
      CmdReg.Bits.mse = 0x01; // Enable Memory Space

      IioWriteCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, (PCICMD_IIOCB_FUNC0_REG | (FunctionNum << 16)), CmdReg.Data);

      IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "SKT=%d: CB BAR =0x%lx, Value=0x%x.\n", IioIndex, CbBar, *((UINT32*)(UINT64)CbBar));

      //
      // (DCA) Check if DCA is supported and enabled by CPU
      //
      EnableDCA = FALSE;

      if (IioGlobalData->SetupData.Cb3DcaEn[IioIndex]){
        //
        // Enable CB DCA based on setup question if CPU is capable - Note that CPU will only enable DCA if all sockets support DCA
        //
        EnableDCA = DetectCpuDcaCapable(IioGlobalData,IioIndex);

        if (EnableDCA) {
          //
          // (DCA) Set "Enable Prefetch Hint on QPI"
          //
          Addr = (UINTN) (CbBar + R_CB_BAR_CSI_CAP_ENABLE);
          Value16 = *(volatile UINT16 *)Addr;
          Value16 |= B_CB_BAR_CSI_CAP_ENABLE_PREFETCH_HINT;
          *(volatile UINT16 *)Addr = Value16;
          S3BootScriptSaveMemWrite (S3BootScriptWidthUint16, Addr, 1, &Value16);

          //
          // (DCA) Set "Enable MemWR on PCIE"
          //
          Addr = (UINTN) (CbBar + R_CB_BAR_PCIE_CAP_ENABLE);
          Value16 = *(volatile UINT16 *)Addr;
          Value16 |= B_CB_BAR_PCIE_CAP_ENABLE_MEM_WR;
          *(volatile UINT16 *)Addr = Value16;
          S3BootScriptSaveMemWrite (S3BootScriptWidthUint16, Addr, 1, &Value16);
        }
      } //end if Cb3DcaEn

      if (EnableDCA) {
        //  Enable port DCA
        //
        Addr =  (UINTN) (CbBar + R_CB_BAR_CSI_DMACAPABILITY);
        Value32 = *(volatile UINT32 *)Addr;
        Value32 |= B_CB_BAR_CSI_DMACAP_DCA_SUP;
        *(volatile UINT32 *)Addr = Value32;
        S3BootScriptSaveMemWrite (S3BootScriptWidthUint16, Addr, 1, &Value32);

      } else {
        //
        //  Disable port DCA
        //
        Addr =  (UINTN) (CbBar + R_CB_BAR_CSI_DMACAPABILITY);
        Value32 = *(volatile UINT32 *)Addr;
        Value32 &= ~B_CB_BAR_CSI_DMACAP_DCA_SUP;
        *(volatile UINT32 *)Addr = Value32;
        S3BootScriptSaveMemWrite (S3BootScriptWidthUint16, Addr, 1, &Value32);

      } //end if EnableDCA


      //
      // Restore Command input command register setting
      //
      IioWriteCpuCsr16(IioGlobalData, IioIndex, IIO_CSTACK, (PCICMD_IIOCB_FUNC0_REG | (FunctionNum << 16)),CmdRegOrg.Data);
    } else {
         IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR,"ERROR! CB BAR not programmed for %d,%d,%d\n", Bus, DeviceNum, FunctionNum);
    }
  }  // if CBConfigCap.DcaSupported
  //
  // Lock Down RWO registers
  //
  RwoRegisterProgramming(IioGlobalData, IioIndex, PciAddress);
  
}

/**

    Code to be executed during boot event.

    @param IioGlobalData - Pointer to IIO Global information
    @param Iio           - Index with the changed IIO (Socket)

    @retval None

**/
VOID
IioIoatInitBootEvent (
  IN  IIO_GLOBALS                  *IioGlobalData,
  IN  UINT8                        IioIndex
  )
{
  PCI_ROOT_BRIDGE_PCI_ADDRESS    PciAddress;
  UINT8                          DmaFn;

  //
  // apply any CBDMA specific WA here
  //
  IioIoatWorkarounds(IioGlobalData, IioIndex);

  PciAddress.Bus      = IioGlobalData->IioVar.IioVData.SocketStackBus[IioIndex][IIO_CSTACK];
  PciAddress.Device   = IOAT_DEVICE_NUM;
  PciAddress.Function = IOAT_FUNC_START;

  InitializeCbDmaType(IioGlobalData, IioIndex, PciAddress);

  //
  // process each valid IOAT component
  //
  for (DmaFn = PciAddress.Function; DmaFn < IOAT_TOTAL_FUNCS; DmaFn++) {
    PciAddress.Function = DmaFn;
    //
    // Verify that the function is present (e.g. not hidden)
    //
    if (DmaFunctionExist(IioGlobalData, IioIndex, PciAddress.Function )== FALSE){
      IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR, "B:%x, D:%x F:%x Disable \n", PciAddress.Bus , PciAddress.Device,  PciAddress.Function);
      continue;
    }

    if (IdentifyCbDmaDevice(IioGlobalData, IioIndex, PciAddress.Function) == TRUE) {
       IioIoatInit(IioGlobalData, IioIndex, PciAddress);
    } // if IdentifyCbDmaDevice
  } //for each IOAT function

}

