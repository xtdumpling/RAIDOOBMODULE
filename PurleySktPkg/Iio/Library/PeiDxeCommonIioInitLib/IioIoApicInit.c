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


  @file IioIoApicInit.c

  This file initialize the IoApic device on IIO

**/
#include <SysFunc.h>
#include <IioUtilityLib.h>
#include <RcRegs.h>
#include "IioIoApicInit.h"

/**

    Enable IOxAPIC: If the stack is present enables Memory space and Bus Master, making the
    IOxAPIC PCI area visible.

    @param IioGlobalData - pointer to IIO_GLOBALS

    @param IioIndex - IIO (Socket) index number.

    @param Stack    - Stack number.

    @retval None

**/
VOID
Enable_IOxAPIC(
  IN  IIO_GLOBALS     *IioGlobalData,
  IN  UINT8           IioIndex,
  IN  UINT8           Stack
  )
{
  PCICMD_IIO_IOAPIC_STRUCT PciCmd_Iio_Ioapic;

  if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex]& (1<<Stack))){
    return;
  }

  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Enable IIO[%d] IOxAPIC\n",IioIndex);

  PciCmd_Iio_Ioapic.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, Stack, PCICMD_IIO_IOAPIC_REG);
  PciCmd_Iio_Ioapic.Bits.mse = 1;
  PciCmd_Iio_Ioapic.Bits.bme = 1;
  IioWriteCpuCsr16(IioGlobalData, IioIndex, Stack, PCICMD_IIO_IOAPIC_REG, PciCmd_Iio_Ioapic.Data);
}

/**

    Post initialization IO APIC. To be called after initialization completes.

    @param Phase - Current phase for initialization

    @param IioGlobalData - pointer to IIO_GLOBALS

    @param IioIndex - IIO (Socket) index number.

    @retval None

**/
VOID
IioIoApicInit (  
  IN  IIO_GLOBALS               *IioGlobalData,
  IN  UINT8                     IioIndex
  )
{  
  UINT32    IoApicAddress=0;
  UINT16    Data16;
  UINT8     Stack;

  MBAR_IIO_IOAPIC_STRUCT    Mbar_Iio_Ioapic;
  VID_IIO_IOAPIC_STRUCT     Vid_Iio_Ioapic;
  SVID_IIO_IOAPIC_STRUCT    Svid_Iio_Ioapic;
  DID_IIO_IOAPIC_STRUCT     Did_Iio_Ioapic;
  TOMMIOL_OB_IIO_VTD_STRUCT Tommiol_Ob_Iio_Vtd;
  PMCAP_IIO_IOAPIC_STRUCT   Pmcap_Iio_Ioapic;
  SDID_IIO_IOAPIC_STRUCT    Sdid_Iio_Ioapic;


  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    // Check if the Stack is present.
    if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1<<Stack))) {
    	continue;
    }

    // Skip enabling IOAPIC on specific disabled stack 
    if (IioGlobalData->IioVar.IioVData.SocketStackPersonality[IioIndex][Stack] == TYPE_DISABLED){
      continue;
    }

    IoApicAddress = (UINT32)IioGlobalData->IioVar.IioVData.IoApicBase[IioIndex][Stack];
    if (IoApicAddress == (UINT32)IioGlobalData->IioVar.IioVData.PchIoApicBase) {
        IoApicAddress = IoApicAddress + 0x1000;       // dont collide with PCH IOAPIC!!
    }
  
    //
    // Read enable/disable from Setup for this IIO
    //

    if (IioGlobalData->SetupData.DevPresIoApicIio[(IioIndex * MAX_IIO_STACK) + Stack]) {
      IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Initialize IIO[%d][%d] IOxAPIC...\n", IioIndex,Stack);

      //                                        
      // Enable IOxAPIC
      //
      //  first make sure the MBAR (offset 10h BAR) is initialized during the regular
      //  PCI enumeration, if not than register for the callback during boot event
      Mbar_Iio_Ioapic.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, MBAR_IIO_IOAPIC_REG);

      Mbar_Iio_Ioapic.Data &= 0xFFFFFFF0;
      //
      // If MBAR has already been assigned
      //
      if (Mbar_Iio_Ioapic.Data) {
        //
        // Enable PCI CMD reg bits and no need for a call
        //
        Enable_IOxAPIC(IioGlobalData, IioIndex, Stack);
        IioGlobalData->IioVar.IioVData.IOxAPICCallbackBootEvent = FALSE;
      } else {
        //
        // Else set a callback event to set PCI CMD reg bits
        //
        IioGlobalData->IioVar.IioVData.IOxAPICCallbackBootEvent = TRUE;
      }
    }

    //
    // Set IOAPIC ABAR
    //
    Data16 = B_APIC_ABAR_ABAR_EN | (0x00000FFF & (UINT16)(IoApicAddress >> 8));
    IioWriteCpuCsr16(IioGlobalData,IioIndex,Stack,ABAR_IIO_IOAPIC_REG, Data16);
    IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "IIO[%d], Stack=%d, IOxAPIC Base=%X\n",IioIndex,Stack,IoApicAddress);

    //
    // Enable accesses by us and the OS to the IOAPIC register address area from 0xFC00_0000 to 0xFEFF_FFFF
    // Block accesses from 0xFF00_0000 to 0xFFFF_FFFF by anybody but us
    //
    Tommiol_Ob_Iio_Vtd.Data = IioReadCpuCsr32(IioGlobalData, IioIndex,Stack, TOMMIOL_OB_IIO_VTD_REG);
    Tommiol_Ob_Iio_Vtd.Bits.tommiol_ob = 0xFEF;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, TOMMIOL_OB_IIO_VTD_REG, Tommiol_Ob_Iio_Vtd.Data);
    IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "IIO[%d] Stack[%d] TOMMIOL_OB = %08X\n",IioIndex,Stack, Tommiol_Ob_Iio_Vtd.Data);

#ifndef MINIBIOS_BUILD
    if (FeaturePcdGet (PcdLockCsrSsidSvidRegister)) {
#endif //MINIBIOS_BUILD
      //
      // Lock Subsystem Vendor ID
      //

      //
      // 4927266: BARS not filled in for IOxAPICS on the p-stacks 1-5
      //
      Vid_Iio_Ioapic.Data = IioReadCpuCsr16(IioGlobalData, IioIndex,Stack,VID_IIO_IOAPIC_REG);
      Svid_Iio_Ioapic.Data = IioReadCpuCsr16(IioGlobalData, IioIndex,Stack,SVID_IIO_IOAPIC_REG);
      //
      // Update Subsystem Vendor ID with the same value as Vendor ID
      //
      if (Svid_Iio_Ioapic.Data != Vid_Iio_Ioapic.Data) {
        Svid_Iio_Ioapic.Data = Vid_Iio_Ioapic.Data;
      }
      IioWriteCpuCsr16(IioGlobalData, IioIndex,Stack,SVID_IIO_IOAPIC_REG,Svid_Iio_Ioapic.Data);
      //
      // Lock Subsystem Device ID
      //
      //
      // 4927266: BARS not filled in for IOxAPICS on the p-stacks 1-5
      //
      if (!(IioGlobalData->IioVar.IioVData.Emulation & SIMICS_FLAG)) {
          if (Stack == IIO_CSTACK){
              Did_Iio_Ioapic.Data = 0x2026;
          } else {
              Did_Iio_Ioapic.Data = 0x2036;
          }
      } else {
        Did_Iio_Ioapic.Data = IioReadCpuCsr16 (IioGlobalData, IioIndex,Stack,DID_IIO_IOAPIC_REG);
      }

      Sdid_Iio_Ioapic.Data = IioReadCpuCsr16(IioGlobalData, IioIndex,Stack,SDID_IIO_IOAPIC_REG);
      //
      // Update Subsystem Device ID with the same value as Device ID
      //
      Sdid_Iio_Ioapic.Data = Did_Iio_Ioapic.Data;
      IioWriteCpuCsr16(IioGlobalData, IioIndex,Stack, SDID_IIO_IOAPIC_REG, Sdid_Iio_Ioapic.Data);
#ifndef MINIBIOS_BUILD
    }
#endif // MINIBIOS_BUILD
    //
    // Lock PMCAP
    //
    Pmcap_Iio_Ioapic.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, PMCAP_IIO_IOAPIC_REG);
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, PMCAP_IIO_IOAPIC_REG, Pmcap_Iio_Ioapic.Data);
  }
}

/**

    Post initialization IO APIC. To be called after initialization completes.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Socket Index

    @retval None

**/
VOID
IoApicPostInit(
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex
)
{
  UINT8       Stack;
  IIOMISCCTRL_N1_IIO_VTD_STRUCT IioMisCCtrlN1;
  
  for (Stack=0; Stack < MAX_IIO_STACK; Stack++) {
    if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1<<Stack))){
      continue;
    }
    //
    // Importing the permanent workaround - s3875843
    //  (Interrupts dropped intermittently due to faulty workround)
    //  Note: applicable to IVT
    IioMisCCtrlN1.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG);
    IioMisCCtrlN1.Bits.disable_new_apic_ordering = 1;   //set disable_new_apic_ordering to 1
    IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, IIOMISCCTRL_N1_IIO_VTD_REG, IioMisCCtrlN1.Data);
  }
}

/**

    Boot event handler. Enable IOxAPIC before passing control to OS.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to the current IIO (Socket)

    @retval None

**/
VOID
IioIoApicInitBootEvent (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex
  )
{
  UINT8     Stack;
  for (Stack = 0; Stack < MAX_IIO_STACK; Stack ++) {
  if(IioGlobalData->IioVar.IioVData.IOxAPICCallbackBootEvent == FALSE){    //no boot event registered
    return;
  }

  if(IioGlobalData->SetupData.DevPresIoApicIio[(IioIndex * MAX_IIO_STACK) + Stack])
      Enable_IOxAPIC(IioGlobalData, IioIndex, Stack);
  }
}
