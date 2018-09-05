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


  @file IioUniphy.c

  Houses all code related to Program Uniphy recipe

Revision History:

**/
#include <SysFunc.h>
#include <IioUtilityLib.h>
#include <EvAutoRecipe.h>
#include <RcRegs.h>
#include <SysHostChip.h>
#include "IioEvRecipeEntry.h"
#include "IioUniphy.h"


/**

  Function to map the Processor Types in EvRecipe and CpuPciAccess.h. 

  @param CpuType  - Processor type of the host
  @param RecipeCpuType - Processor type of the Register being updated in the EV Recipe

  @retval True/False - Returns True if the host CPU type matches the CPU type of the EV Recipe Entry

**/
BOOLEAN
IsValidCpuType (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT32            CpuType, 
  IN  UINT32            RecipeCpuType
  )
{
  UINT8 Result = FALSE;
  switch(CpuType) {
    case CPU_SKX:
      if (RecipeCpuType == SKX_SOCKET){
      Result = TRUE;
      }
    break;
    default:
      break;
  }
  return Result;
}

/**

  Function to map the Processor SKUs in EvRecipe and CpuPciAccess.h. 

  @param CpuSubType  - Processor sub type of the host whether its an EX or EP
  @param RecipeCpuSubType - Processor sub type of the Register being updated in the EV Recipe

  @retval True/False - Returns True if the host CPU sub type matches the SKU of the EV Recipe Entry

**/
BOOLEAN
IsValidSkuType (
  IN  UINT16            CpuSubType,
  IN  UINT16            RecipeSkuType
  )
{
  return TRUE;
}

/**

  Function to map the Processor Stepping in EvRecipe and Syshost.h. 

  @param CpuStepping  - Processor stepping of the host
  @param RecipeCpuStepping - Processor stepping of the Register being updated in the EV Recipe

  @retval True/False - Returns True if the host and EV Recipe Entry steppings match

**/
BOOLEAN
IsValidStepping (
  IN  IIO_GLOBALS      *IioGlobalData,
  IN  UINT8            Iio,
  IN  UINT32           CpuStepping, 
  IN  UINT64           RecipeCpuStepping
  )
{
  BOOLEAN Result = FALSE;

  if (RecipeCpuStepping & (UINT64)(1 << CpuStepping)) { 
    Result = TRUE;
  }
  return Result;
}

/**

  Function to Check if Global DFX register needs to be updated. 

  @param PortBitMap - PortBitMap Mask

  @retval True/False - Returns True if Global Dfx is selected or not. Bit 1 of PortBitMask is set.

**/
BOOLEAN
IsGlobalDfxEnable (
  IN UINT32 PortBitMap
  )
{
    BOOLEAN Result = FALSE;

    // Check if BIT31 of PortBitMask is set, if so Global Dfx is selected. 
    if (PortBitMap & GLOBAL_DFX_BIT) {
        Result = TRUE;
    }

    return Result;
}

/**

  This function identifies the DFX register type either normal PCIE port or Global DFX

  @param  RegisterAddress - Pcie Address of the DFX register

  @retval Register Type   - IIO_UNKNOW_REG
                            IIO_PORT_DFX_REG
                            IIO_GLOBAL_DFX_REG
**/
UINT8
GetRegisterType (
  IN  UINT32 RegisterAddress
  )
{
    UINT8 Result = IIO_UNKNOW_REG;
    UINT32 Device;
    UINT32 Func;

    Device = (RegisterAddress & 0x000F8000) >> 0xF;
    Func = (RegisterAddress & 0x00007000) >> 0xC;

    if ((Device == PCIE_PORT_GLOBAL_DFX_DEV) && (Func == PCIE_PORT_GLOBAL_DFX_FUNC)) {
        Result = IIO_GLOBAL_DFX_REG;
    } else if ((Device == PCIE_PORT_1A_DFX_DEV) && ((Func >= PCIE_PORT_1A_DFX_FUNC) && (Func <= PCIE_PORT_1D_DFX_FUNC))) {
       Result = IIO_PORT_DFX_REG;
    } else if (((Device >= PCIE_PORT_1A_DEV) && (Device <= PCIE_PORT_1D_DEV)) &&  (Func == PCIE_PORT_1A_FUNC)){
      Result = IIO_PORT_PCIE_REG;
    }

    return Result;
}

/**

  Function to check if Recipe Entry is valid/applicable for this cpu.

  @param SocBitMap - SocBitMap Mask
  @param IioIndex  - Index to Iio


  @retval True/False - Returns True if recipe entry is applicable for cpu/socket or not.

**/
BOOLEAN
IsValidCpuEntry(
  IN UINT32      SocBitMap,
  IN UINT8      IioIndex
  )
{
    BOOLEAN Result = FALSE;
    UINT8   CpuMask;

    CpuMask = 1 << IioIndex;

    if((SocBitMap == WILDCARD_32) || (CpuMask & SocBitMap)) {
      Result = TRUE;
    }

    return Result;
}

/**

  Function to check if SRIS is enabled 

  @param IioGlobalData - Pointer to IIO_GLOBALS structure
  @param IioIndex  - Index to Iio

  @retval True/False - Returns True/False if SRIS is enabled or 
         not.
  
**/
BOOLEAN
IsSrisEnable(
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT8        IioIndex,
  IN  UINT8        PortIndex
  )
{
  if (IioGlobalData->SetupData.SRIS[IioIndex * NUMBER_PORTS_PER_SOCKET + PortIndex]){
    return TRUE;
  } else {
   return FALSE;
  }
}

/**

  This function does a blind write to LBC registers.

  @param IioGlobalData   - IioGlobalData pointer
  @param IioIndex        - Iio Index
  @param PortIndex       - IIO port number 
  @param Lbctype         - Type of LBC write either AFE or COM 
  @param EvLbcRegister   - 7-bit wide load select (address within LBC)
  @param EvData          - 14-bit data value to write
   
  @retval None 
   
**/
VOID
IioWriteLbc (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex,
  IN  UINT8             PortIndex,
  IN  UINT8             LbcType,
  IN  UINT32            EvLoadSelectLbcRegister,
  IN  UINT32            EvData
  )
{
  UINT8     BusNumber;
  UINT8     Device, Function;
  UINT8     LaneCount = 0;
  UINT16    LaneIndex;
  LBC_PER_IOU_DATA_IIO_DFX_STRUCT      LbcPerIouData;
  LBC_PER_IOU_CONTROL_IIO_DFX_STRUCT   LbcPerIouControl;
  LBC_COM_DATA_IIO_DFX_GLOBAL_STRUCT   LbcComData;
  LBC_COM_CONTROL_IIO_DFX_GLOBAL_STRUCT LbcComControl;
  UINT32   WaitTime;
  
  if( IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex) == FALSE){  //check whether the PCIe port can be configured
    return;
  }

  BusNumber = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex];

  if(LbcType == TYPE_LBC_AFE) {
    //
    // Get lane count based on IOU width
    //
    switch(PortIndex) {
      case PORT_0_INDEX:
        LaneCount = 4;
        break;
      case PORT_1A_INDEX:
      case PORT_2A_INDEX:
      case PORT_3A_INDEX:
      case PORT_4A_INDEX:
      case PORT_5A_INDEX:
        LaneCount = 16;
        break;
      default:
        return;
    }

    Device  = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].DfxClusterDevice;
    Function = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].DfxClusterFunction;

    if (!(IioGlobalData->IioVar.IioVData.Emulation & SIMICS_FLAG)) {
      LbcPerIouControl.Data = 0;
      LbcPerIouControl.Bits.lbc_start = LBC_START_COMMAND;

      //
      // Make sure LBC_PER_IOU_CONTROL.lbc_start == 0x0
      //
      WaitTime = LBC_TIME_OUT;
      while((LbcPerIouControl.Bits.lbc_start != LBC_CPL_COMMAND) && (WaitTime > 0) ){
        IioStall(IioGlobalData, LBC_PROGRAM_DELAY);
        LbcPerIouControl.Data = IioPciExpressRead32 (IioGlobalData, IioIndex, BusNumber, Device, Function, LBC_PER_IOU_CONTROL_IIO_DFX_REG);
        WaitTime--;
      }
      if(WaitTime == 0){
         IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LoadBus_Write ERROR: lbc_start bit has not be cleared by HW\n", IioIndex, BusNumber, PortIndex, EvLoadSelectLbcRegister, EvData);
         IIO_ASSERT(IioGlobalData, IIO_ERR_GENERAL_FAILURE, IIO_MINOR_ERR_TIMEOUT);
      }
    } // Simics 

    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "  LBC_AFE programming for IIO[%d] Bus[0x%x] Port[%d] Sel_ld[%d] Data[0x%x]\n", IioIndex, BusNumber, PortIndex, EvLoadSelectLbcRegister, EvData);

    for (LaneIndex = 0; LaneIndex < LaneCount; LaneIndex++) {
      //
      // Setup Load Bus Data register
      //
      LbcPerIouData.Data = 0;
      LbcPerIouData.Bits.lbc_data = EvData;
      IioPciExpressWrite32 (IioGlobalData, IioIndex, BusNumber, Device, Function, LBC_PER_IOU_DATA_IIO_DFX_REG, LbcPerIouData.Data);

      //
      // Setup Load Bus Control register
      //
      LbcPerIouControl.Bits.lbc_req    = LBC_WRITE_COMMAND;
      LbcPerIouControl.Bits.lbc_start  = LBC_START_COMMAND;
      LbcPerIouControl.Bits.lbc_ld_sel = EvLoadSelectLbcRegister;
      LbcPerIouControl.Data |= 1 << (LaneIndex + LBC_LN_SEL_OFFSET);     // Lbc_ln_selX[1:0], X = {7...0} Bits[26:11]
      IioPciExpressWrite32 (IioGlobalData, IioIndex, BusNumber, Device, Function, LBC_PER_IOU_CONTROL_IIO_DFX_REG, LbcPerIouControl.Data);
      //IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "  LbcPerIouControl Addr %x = 0x%x\n", RegAddr, LbcPerIouControl.Data);
    }// for ...
  } else { // LCB_TYPE_COM

    Device = PCIE_PORT_GLOBAL_DFX_DEV;
    Function = PCIE_PORT_GLOBAL_DFX_FUNC;

    if (!(IioGlobalData->IioVar.IioVData.Emulation & SIMICS_FLAG)) {
      LbcComControl.Data = 0;
      LbcComControl.Bits.lbc_start = LBC_START_COMMAND;

      //
      // Make sure LBC_COM_CONTROL_IIO_DFX_REG.lbc_start == 0x0
      //
      WaitTime = LBC_TIME_OUT;
      while((LbcComControl.Bits.lbc_start != LBC_CPL_COMMAND) && (WaitTime > 0) ){
        IioStall(IioGlobalData, LBC_PROGRAM_DELAY);
        LbcComControl.Data = IioPciExpressRead32 (IioGlobalData, IioIndex, BusNumber, Device, Function, LBC_PER_IOU_CONTROL_IIO_DFX_REG);
        WaitTime--;
      }
      if (WaitTime == 0){
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "LoadBus_Write ERROR: lbc_start bit has not be cleared by HW\n", IioIndex, BusNumber, PortIndex, EvLoadSelectLbcRegister, EvData);
        IIO_ASSERT(IioGlobalData, IIO_ERR_GENERAL_FAILURE, IIO_MINOR_ERR_TIMEOUT);
      }
    } // Simics 

    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "  LBC_COM programming for IIO[%d] Bus[0x%x] Port[%d] Sel_ld[%d] Data[0x%x]\n", IioIndex, BusNumber, PortIndex, EvLoadSelectLbcRegister, EvData);
	   
    //
    // Setup Load Bus Data register
    //
    LbcComData.Data = 0;
    LbcComData.Bits.lbc_data = EvData;
    IioPciExpressWrite32 (IioGlobalData, IioIndex, BusNumber, Device, Function, LBC_COM_DATA_IIO_DFX_GLOBAL_REG, LbcComData.Data);

    //
    // Setup Load Bus Control register
    //
    LbcComControl.Bits.lbc_req    = LBC_WRITE_COMMAND;
    LbcComControl.Bits.lbc_start  = LBC_START_COMMAND;
    LbcComControl.Bits.lbc_ld_sel = EvLoadSelectLbcRegister;
    IioPciExpressWrite32 (IioGlobalData, IioIndex, BusNumber, Device, Function, LBC_COM_CONTROL_IIO_DFX_GLOBAL_REG, LbcComControl.Data);
    //IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "  LbcComControl Addr %x = 0x%x\n", RegAddr, LbcComControl.Data);
  }
}

/**

   This function loops through each entry in the IioUniphyRecipe  
   and updates the Registers based on Processor type, subtype, Bitrates and Stepping conditions. 
   This function is called for all ports of all sockets.

   @param IioGlobalData - IioGlobalData pointer
   @param Iio - IIO number
   @param BusNumber - Iio Bus Base 
   @retval Status

**/
VOID 
IioRxRecipeSettingsAuto (
  IN  IIO_GLOBALS             *IioGlobalData,
  IN  UINT8                   IioIndex
  )
{
  UINT32      RegAddr;
  UINT32      Data = 0;
  UINT32      TableIndex;
  UINT32      NumEntries;
  BOOLEAN     SteppingCmp;
  BOOLEAN     CpuTypeCmp;
  BOOLEAN     SkuTypeCmp;
  UINT8       PortIndex;
  UINT8       BusNumber;
  UINT8       IioDev;
  UINT8       IioFunc;
  UINT8       RegType;
  BOOLEAN     ValidCpuEntry;
  UINT32      IioEvRecipeTableSize = 0;
  UINT16      ConfigMask;
  EV_RECIPE_HEADER *IioEvHdrTablePtr = NULL;
  EV_RECIPE_ENTRY  *IioEvRecipeTablePtr = NULL;
  //
  // Point to the right EV Recipe Table
  //
  if (IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) {
    IioEvHdrTablePtr = (EV_RECIPE_HEADER*) &IioUniphyRecipeVer;
    IioEvRecipeTablePtr = (EV_RECIPE_ENTRY*) &IioUniphyRecipe;
    IioEvRecipeTableSize = sizeof(IioUniphyRecipe);
  } else {
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Automated uniphy EV recipe not supported for CPUs other SKX-EP or SK-EX \n");
    IIO_ASSERT(IioGlobalData, FALSE, IioGlobalData->IioVar.IioVData.CpuType); // halt
  }

  if (IioEvRecipeTablePtr == NULL){
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "\nERROR!!! IioRxRecipeSettingsAuto() - IioEvRecipeTablePtr is a NULL pointer.\n");
    return;
  }

  if (IioEvHdrTablePtr == NULL){
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "\nERROR!!! IioRxRecipeSettingsAuto() - IioEvHdrTablePtr is a NULL pointer.\n");
    return;
  }

  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Program Uniphy recipe Revision %s\n",IioEvHdrTablePtr->Revision);
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Program RX Recipe values Start.\n");

  // Loop though all the recipe entries in IiPciAddressoEvRecipeTablePtr and apply the recipe if applicable
  NumEntries = IioEvHdrTablePtr->NumberEntries;
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "EV_RECIPE_ENTRY Size = %x\n", NumEntries);
  for (TableIndex = 0; TableIndex < NumEntries; TableIndex++) {

    //Verify if the Processor Type, SubType (SKU) and Stepping match with the EV Recipe Entry
    CpuTypeCmp = IsValidCpuType(IioGlobalData, IioGlobalData->IioVar.IioVData.CpuType, IioEvRecipeTablePtr[TableIndex].ProcessorType);
    SkuTypeCmp = IsValidSkuType(IioGlobalData->IioVar.IioVData.CpuSubType, IioEvRecipeTablePtr[TableIndex].Skus);
    SteppingCmp = IsValidStepping(IioGlobalData, IioIndex, IioGlobalData->IioVar.IioVData.CpuStepping, IioEvRecipeTablePtr[TableIndex].Steppings);
    RegType = GetRegisterType(IioEvRecipeTablePtr[TableIndex].RegisterAddress);
    ValidCpuEntry = IsValidCpuEntry(IioEvRecipeTablePtr[TableIndex].SocBitmap, IioIndex);

    // Skip Entry if CPU type, SKU and stepping doesn;t apply to the current CPU settings.
    if (!(CpuTypeCmp && SkuTypeCmp && SteppingCmp && ValidCpuEntry)){
      continue;
    }

    for(PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {

      // Check whether the PCIe port can be configured or Skip port if it is not selected
     if (!(IioEvRecipeTablePtr[TableIndex].PortBitmap >> PortIndex) & BIT0){
        continue;
     }

      // Check Config criteria if not applicable for all configs
      if (IioEvRecipeTablePtr[TableIndex].Config != WILDCARD_16) {
        // check if SRIS is enabled
        if (IsSrisEnable(IioGlobalData, IioIndex, PortIndex)) {
          // Check if Config includes SRIS_CFG
          ConfigMask = (IioEvRecipeTablePtr[TableIndex].Config & (1 << SRIS_CFG));
          if (ConfigMask != (1 << SRIS_CFG)){
            continue;
          }
        } else {
          if (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == TRUE){
            // Check if Config includes DMI_CFG
            ConfigMask = (IioEvRecipeTablePtr[TableIndex].Config & (1 << DMI_CFG));
            if (ConfigMask != (1 << DMI_CFG)) {
                continue;
            }
          } else if (PortIndex > PORT_3D_INDEX){
            // Check if FPGA port
            if(CheckFPGAPort(IioGlobalData, IioIndex, PortIndex) == TRUE){
              ConfigMask = (IioEvRecipeTablePtr[TableIndex].Config & (1 << FPGA_CFG));
               if (ConfigMask != (1 << FPGA_CFG)){
                   continue;
               }
            } else { // Check if WFR case
                // Check if Config includes WFR_CFG
              ConfigMask = (IioEvRecipeTablePtr[TableIndex].Config & (1 << WFR_CFG));
              if (ConfigMask != (1 << WFR_CFG)){
                 continue;
               }
            }
          } else {
             ConfigMask = (IioEvRecipeTablePtr[TableIndex].Config & (1 << PCIE_CFG));
             if (ConfigMask != (1 << PCIE_CFG)){
                continue;
             }
          }
        }
      }

      if ((IioEvRecipeTablePtr[TableIndex].AccessType == TYPE_LBC_AFE) || 
          (IioEvRecipeTablePtr[TableIndex].AccessType == TYPE_LBC_COM)) {
        //
        // Perform a blind LBC write
        //
        IioWriteLbc (IioGlobalData,
                     IioIndex,
                     PortIndex,
                     IioEvRecipeTablePtr[TableIndex].AccessType,
                     IioEvRecipeTablePtr[TableIndex].RegisterAddress,
                     IioEvRecipeTablePtr[TableIndex].Data
                     );
      } else {
        //
        // TYPE_CSR
        //


        BusNumber = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex];
        // Check register type
        switch (RegType){
          case  IIO_GLOBAL_DFX_REG:
           IioDev = PCIE_PORT_GLOBAL_DFX_DEV;
           IioFunc = PCIE_PORT_GLOBAL_DFX_FUNC;
           break;
          case IIO_PORT_DFX_REG:
            IioDev = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].DfxDevice;
            IioFunc = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].DfxFunction;
            break;
          case IIO_PORT_PCIE_REG:
            IioDev = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device;
            IioFunc = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function;
            break;
          default:
            // Skip it if unknow register type
            continue;
           break;
        }

        //Compute the register address
        RegAddr = PCI_PCIE_ADDR(BusNumber, IioDev, IioFunc, (IioEvRecipeTablePtr[TableIndex].RegisterAddress & 0xFFF));
        //
        // Check register size and read data
        //
        switch (IioEvRecipeTablePtr[TableIndex].RegisterSize) {
          case sizeof (UINT32):
            Data = IioPciExpressRead32 (IioGlobalData, IioIndex, \
            		BusNumber, IioDev, IioFunc, (IioEvRecipeTablePtr[TableIndex].RegisterAddress & 0xFFF));
            break;
          case sizeof (UINT16):
            Data = IioPciExpressRead16 (IioGlobalData, IioIndex, \
            		BusNumber, IioDev, IioFunc, (IioEvRecipeTablePtr[TableIndex].RegisterAddress & 0xFFF));
            break;
          case sizeof (UINT8):
            Data = IioPciExpressRead8 (IioGlobalData, IioIndex, \
            		BusNumber, IioDev, IioFunc, (IioEvRecipeTablePtr[TableIndex].RegisterAddress & 0xFFF));
            break;
          default:
            break;
        }
        //Update the data with the values from the EV Recipe
        Data = ((Data & IioEvRecipeTablePtr[TableIndex].FieldMask) |IioEvRecipeTablePtr[TableIndex].Data);

        //Write the data
        switch (IioEvRecipeTablePtr[TableIndex].RegisterSize) {
          case sizeof (UINT32):
            IioPciExpressWrite32 (IioGlobalData, IioIndex, \
            		BusNumber, IioDev, IioFunc, (IioEvRecipeTablePtr[TableIndex].RegisterAddress & 0xFFF), Data);

            break;

          case sizeof (UINT16):
            IioPciExpressWrite16 (IioGlobalData, IioIndex, \
            		BusNumber, IioDev, IioFunc, (IioEvRecipeTablePtr[TableIndex].RegisterAddress & 0xFFF), (UINT16)Data);
            break;

          case sizeof (UINT8):
            IioPciExpressWrite8 (IioGlobalData, IioIndex, \
            		BusNumber, IioDev, IioFunc, (IioEvRecipeTablePtr[TableIndex].RegisterAddress & 0xFFF), (UINT8)Data);
            break;

          default:
            break;
        }
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "S:%x P:%x B:%x D:%x F:%x O:%xh = 0x%x\n", IioIndex, PortIndex, BusNumber, IioDev, IioFunc, RegAddr, Data);
      }
    }
  }
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "Program RX Recipe values End.\n");
  return;
}
/************* IIO Uniphy recipe *****************/

