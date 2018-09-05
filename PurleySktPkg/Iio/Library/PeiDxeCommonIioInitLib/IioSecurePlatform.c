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


  @file IioSecurePlatform.c

  Implement IIO initialization protocol to provide IIO initialization functions.
  This interfaces will be called by PciHostBridge driver in the PCI bus enumeration.

**/

#include <SysFunc.h>
#include <IioUtilityLib.h>
#include <RcRegs.h>
#include <SysHostChip.h>

/**

    This function will override IioDevFuncHide table with BIOS
    knob DfxSocketDevFuncHide value

    @param IioGlobalData  - Pointer to IIO_GLOBALS
    @param IioIndex       - Index of Iio

    @retval None

**/
VOID
IioApplyDfxSocketDevFuncHideSettings (
  IN     IIO_GLOBALS             *IioGlobalData,
  IN     UINT8                    IioIndex,
  IN OUT IIO_DEVFUNHIDE_TABLE     *IioDevFuncHide
  )
{
  UINT8        Stack;
  UINT8        FuncNum;
  UINT8        SocketDFHIndex;
  UINT8       Index;

  Stack = 0;
  Index = 0;

  // Setup of S4-S7 alias to S0-S3
  SocketDFHIndex = (IioIndex) * MAX_DEVHIDE_REGS;

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    Index =  SocketDFHIndex + (Stack * NUM_DEVHIDE_REGS);
    for (FuncNum = 0; FuncNum < NUM_DEVHIDE_REGS; FuncNum++) {
      if (IioGlobalData->SetupData.DfxSocketDevFuncHide[Index + FuncNum]) {
        IioDevFuncHide[IioIndex].IioStackDevHide[Stack].DevToHide[FuncNum] = IioGlobalData->SetupData.DfxSocketDevFuncHide[Index + FuncNum];
      }
   }// End FuncNum For
  }// End Stack For

}

/**

    This funtion will update IioDevFuncHide Table based on setup values
    for any device in specific

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to CPU/IIO

    @retval None

**/
VOID
IioApplyIioDevFuncHideSettings (
  IN      IIO_GLOBALS               *IioGlobalData,
  IN      UINT8                      IioIndex,
  IN OUT  IIO_DEVFUNHIDE_TABLE      *IioDevFuncHide
  )
{
  DEVHIDE_FIELD DevHide;
  UINT8  Stack;
  UINT8  Index;
  UINT8  *Array = NULL;
  if (IioIndex < MaxIIO){
    for (Index = 0; Index < IOAT_TOTAL_FUNCS; Index++) {
      if ( IioGlobalData->SetupData.Cb3DmaEn[(IioIndex * IOAT_TOTAL_FUNCS) + Index] == 0 ){
         DevHide.Data = IioDevFuncHide[IioIndex].IioStackDevHide[IIO_CSTACK].DevToHide[IOAT_FUNC_START + Index];
         DevHide.Bits.Dev4 = 1;
         IioDevFuncHide[IioIndex].IioStackDevHide[IIO_CSTACK].DevToHide[IOAT_FUNC_START + Index] = DevHide.Data;
      }
    } // End Index For

    Array = &(IioGlobalData->SetupData.DevPresIoApicIio[(IioIndex * MAX_IIO_STACK)]);

    for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
      if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1<<Stack))) {
        continue;
      }

      //
      // Hide IoApic if required
      //
      IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO," DevPresIoApicIio[%x] = %x\n",((IioIndex * MAX_IIO_STACK) + Stack), IioGlobalData->SetupData.DevPresIoApicIio[(IioIndex * MAX_IIO_STACK)+Stack]);
      if (Array[Stack] == 0) {
        DevHide.Data = IioDevFuncHide[IioIndex].IioStackDevHide[Stack].DevToHide[APIC_FUNC_NUM];
        DevHide.Bits.Dev5 = 1;
        IioDevFuncHide[IioIndex].IioStackDevHide[Stack].DevToHide[APIC_FUNC_NUM] = DevHide.Data;
        IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO,"Hide IIOAPIC = %x %x\n",IioIndex, Stack);
      }
      //
      // 4928364: IO resource issue causing POST hang in Purley EX 4S
      // Hide VMD devices if IIO port is not configured as VMD
      // 4930121: VMD: Devhide unused VMDs on Mstacks (root buses 4 and 5) on all sockets
      //
      if(!CheckVMDEnablePerStack(IioGlobalData, IioIndex, Stack)){
        DevHide.Data = IioDevFuncHide[IioIndex].IioStackDevHide[Stack].DevToHide[VMD_FUNC_NUM];
        DevHide.Bits.Dev5 = 1;
        IioDevFuncHide[IioIndex].IioStackDevHide[Stack].DevToHide[VMD_FUNC_NUM] = DevHide.Data;
      }

      if (Stack == IIO_PSTACK1){
        if ((IioGlobalData->IioVar.IioVData.CpuType == CPU_SKX) &&
                    (IioGlobalData->IioVar.IioVData.CpuStepping < B0_REV_SKX)) {
          //
          // 4929300: Cloned From SKX Si Bug Eco: DID mismatch for BDF 2,23,5
          //
          DevHide.Data = IioDevFuncHide[IioIndex].IioStackDevHide[Stack].DevToHide[CORE05_DEV_NUM];
          DevHide.Bits.Dev23 = 1;
          IioDevFuncHide[IioIndex].IioStackDevHide[Stack].DevToHide[CORE05_DEV_NUM] = DevHide.Data;

          //
          // 4929734: Cloned From SKX Si Bug Eco: Ubox : Pcie header register, Class code (offset0x9) for
          // BDF 2,22,0 is not initialized to correct value. Only applies to SKX A0
          //
          DevHide.Data = IioDevFuncHide[IioIndex].IioStackDevHide[Stack].DevToHide[0];
          DevHide.Bits.Dev22 = 1;
          IioDevFuncHide[IioIndex].IioStackDevHide[Stack].DevToHide[0] = DevHide.Data;
        }
      }
    }// End Stack For
  }// End If IioIndex
}

/**

    This function will Hide all Devices in a Socket
    based on IioDevFuncHide Table after Resource Allocation
    process

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to CPU/IIO

    @retval None

**/
VOID
IioMiscHide (
  IN     IIO_GLOBALS            *IioGlobalData,
  IN     UINT8                 IioIndex,
  IN     UINT8                 Phase,
  IN OUT IIO_DEVFUNHIDE_TABLE  *IioDevFuncHide
  )
{
  DEVHIDE0_UBOX_MISC_STRUCT DevHideData;
  UINT32 StackDevHideReg;
  UINT32 DevHideReg;
  DEVHIDE_FIELD DevHide;
  UINT8 Stack;
  UINT8 FuncNum;

  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Hide devices in Iio:%d, Phase = %x\n",IioIndex,Phase);

  switch (Phase){
    case IioBeforeResources:
      // Update IioDevHide Table with BIOS knobs values for CBDMA, IOAPIC and VDM
      IioApplyIioDevFuncHideSettings(IioGlobalData, IioIndex, IioDevFuncHide);
      break;
    case IioAfterResources:
      //
      // Hide PCU F6 if required
      //
      if (IioGlobalData->SetupData.PCUF6Hide != 0){
         DevHide.Data = IioDevFuncHide[IioIndex].IioStackDevHide[IIO_PSTACK0].DevToHide[PCU22_FUNC_NUM];
         DevHide.Bits.Dev30 = 1;
         IioDevFuncHide[IioIndex].IioStackDevHide[IIO_PSTACK0].DevToHide[PCU22_FUNC_NUM] = DevHide.Data;
      }
      // Update IioDevFuncHide Table with DfxSocketDevFuncHide values.
      IioApplyDfxSocketDevFuncHideSettings(IioGlobalData,IioIndex, IioDevFuncHide);
      break;
    default:
      break;
  }

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    StackDevHideReg = DEVHIDE0_UBOX_MISC_REG + ((sizeof(UINT32) * NUM_DEVHIDE_REGS) * Stack);
    for (FuncNum = 0; FuncNum < NUM_DEVHIDE_REGS; FuncNum++) {
      DevHideReg = StackDevHideReg + (4 * FuncNum);
      DevHideData.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, 0, DevHideReg);
      DevHideData.Data |= IioDevFuncHide[IioIndex].IioStackDevHide[Stack].DevToHide[FuncNum];
      IioWriteCpuCsr32(IioGlobalData, IioIndex, 0, DevHideReg, DevHideData.Data);
    }// End FuncNum For
  }// End Stack For
}

/**

    This function will Hide all PCIe Ports based on  PEXPHIDE Settings

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
IioPcieDeviceHide (
  IN      IIO_GLOBALS                   *IioGlobalData,
  IN      UINT8                         IioIndex,
  IN OUT  IIO_DEVFUNHIDE_TABLE          *IioDevFuncHide
  )
{
  UINT8   PortIndex;
  UINT8   MaxRootDev;
  UINT8   BusNumber;
  UINT8   FuncX=0, DevNum=0, Stack=0;
  DEVHIDE_B0F0_PCU_FUN6_STRUCT DevHidePcuFunc;
  UINT8   ClrHdrMfd[MAX_IOU_PORT_DEVICES];           // Clear Header register per-Iou
  UINT8   DeviceNum, ClrHdrMfdVal8 = 0; // Clear Header register Multi-function device bit.
  DEVHIDE_FIELD DevHide;

  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "IioInit PCIe device hide..\n");

  DevHide.Data = 0;
  ClrHdrMfdVal8 = 0;      // Value to program into ClrHdrMfd field
  ClrHdrMfd[0] = 0;       // Device 0 is always single-function

  for(DeviceNum = 1; DeviceNum < MAX_IOU_PORT_DEVICES; DeviceNum++) {
     ClrHdrMfd[DeviceNum] = BIT3 | BIT2 | BIT1;   // All other devices have 4 ports (Funcs 1-3 represented here)
  }

  for( PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++ ){
     //
     // Check if IioRootPort is populated otherwise hide the port.
     //
     if ((CheckPciePortEnable(IioGlobalData, IioIndex, PortIndex) == FALSE)){
         IioGlobalData->SetupData.PEXPHIDE[ (IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] = 1;
     }

     BusNumber = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex];
     Stack = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
     //
     // Skip ports if stack is disabled
     //
     if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1<<Stack))) {
       continue;
     }

     // Per PCI specification, if any function is present on a device, then Device/Function 0 must be present as well
     switch (PortIndex) {
        case PORT_0_INDEX:
          //
          // Disable Port 0 of non-legacy sockets if C-stack is not valid
          //
          if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
            IioGlobalData->SetupData.PEXPHIDE[ (IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] = 1;
          }
          break;
        case PORT_1A_INDEX:
        case PORT_2A_INDEX:
        case PORT_3A_INDEX:
        case PORT_4A_INDEX:
        case PORT_5A_INDEX:
          //
          // 4928750: [BIOS DV] Invalid secondary bus number programmed for PCIe port on Bus2 (set to 0x0).
          // Check if Stack is enable otherwise disable the ports
          //
          if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
            IioGlobalData->SetupData.PEXPHIDE[ (IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] = 1;
          }
          break;
        default:
          break;
     }

     MaxRootDev = (IioGlobalData->IioVar.IioVData.SocketStackLimitBusNumber[IioIndex][Stack] - 1) - (IioGlobalData->IioVar.IioVData.SocketStackBaseBusNumber[IioIndex][Stack] + 1);
     if (IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device > 0 &&
         IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device > MaxRootDev) {
         IioGlobalData->SetupData.PEXPHIDE[ (IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] = 1;
         IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "WARNING: CPU%x stack%x limit bus res[%x, %x] forces hiding Bus=x%02x, Device=%d, Function=%d.\n",
                IioIndex,
                Stack,
                IioGlobalData->IioVar.IioVData.SocketStackBaseBusNumber[IioIndex][Stack] + 1,
                IioGlobalData->IioVar.IioVData.SocketStackLimitBusNumber[IioIndex][Stack] - 1,
                BusNumber,
                IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device,
                IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function);
     }

     if (IioGlobalData->IioVar.IioVData.SocketStackPersonality[IioIndex][Stack] == TYPE_DISABLED) {
        IioGlobalData->SetupData.PEXPHIDE[ (IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] = 1;
     }

     DevHide.Data = 0;
     DevNum = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device;
     FuncX = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function;
     // Mask the device as hide in UBOX device 8 function 2
     DevHide.Data = 1 << DevNum;

     if(IioGlobalData->SetupData.PEXPHIDE[ (IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]){
       ClrHdrMfd[DevNum] &= ~(1 << FuncX);  // Clear the bit representing the function being disabled in this device.

       IioDevFuncHide[IioIndex].IioStackDevHide[Stack].DevToHide[FuncX] |= DevHide.Data;
       IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Bus=x%02X, Device=%d, Function=%d is hidden in UBOX.\n", BusNumber, DevNum, FuncX);
     } // End of if(IioGlobalData->SetupData.PEXPHIDE

     // For PCU use the following criteria:
     // Avoid hide Dev0/Func 0, If there is slots on the platform where devices could be plugged in
     // does not matter if there is someone connected or not DEvice should not be hide.
     if(IioGlobalData->SetupData.HidePEXPMenu[ (IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]){
        DevHidePcuFunc.Data =  IioReadCpuCsr32(IioGlobalData, IioIndex, 0, (DEVHIDE_B0F0_PCU_FUN6_REG + (Stack *32) + (FuncX * 4)));
        DevHidePcuFunc.Data |=  DevHide.Data;
        IioWriteCpuCsr32(IioGlobalData, IioIndex, 0, (DEVHIDE_B0F0_PCU_FUN6_REG + (Stack *32) + (FuncX * 4)), DevHidePcuFunc.Data);
        IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Bus=x%02X, Device=%d, Function=%d is hidden in PCU.\n", BusNumber, DevNum, FuncX);
     }

  } // End of for( PortIndex = 0; PortIndex

  for (Stack=0; Stack <MAX_IIO_STACK; Stack++) {
     /// Since on SKX the devices have only one function setting all the devices have only function 0.
     /// so we set this for all the devices.
     ClrHdrMfdVal8 = 0x7;
     // Program Clr_HdrMfd register with the intended value
     IioWriteCpuCsr8(IioGlobalData, IioIndex, Stack, HDRTYPECTRL_IIO_VTD_REG, ClrHdrMfdVal8);
  } // End of for (Stack=0; Stack <MAX_IIO_STACK; Stack++)

}

/**


   Function that writes the CSR that enables PECI as trusted interface.
   HSD 4166549: Implement Power Management Security Configuration as per SeCoE recommendation

    @param pIioGlobalData - Pointer to USD

    @retval None

**/
VOID
TxtTrustPeci (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT8       IioIndex
  )
{
  PCU_LT_CTRL_PCU_FUN3_STRUCT PcuLtCtrl;

#ifdef EFI_DEBUG
   { //
     // PECI is Trusted (PIT) bit is sampled at RST_CPL3, make sure RST_CPL3 was not asserted yet
     //
     BIOS_RESET_CPL_PCU_FUN1_STRUCT RstCplReg;
     RstCplReg.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_CSTACK, BIOS_RESET_CPL_PCU_FUN1_REG);
     ASSERT(!RstCplReg.Bits.rst_cpl3);
   }
#endif
   PcuLtCtrl.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, IIO_CSTACK, PCU_LT_CTRL_PCU_FUN3_REG);
   if (IioGlobalData->SetupData.PeciInTrustControlBit) {
     PcuLtCtrl.Bits.pit = 1;
   } else {
     PcuLtCtrl.Bits.pit = 0;
   }
   IioWriteCpuCsr32(IioGlobalData, IioIndex, IIO_CSTACK, PCU_LT_CTRL_PCU_FUN3_REG, PcuLtCtrl.Data);
}


