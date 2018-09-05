/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IioDmiEarlyInit.c

Abstract:

  Houses all code related to Initialize DFX 

Revision History:

--*/
#include <SysFunc.h>
#include <IioUtilityLib.h>
#include <IioInitLib.h>
#include <IioInitPeiLib.h>
#include <RcRegs.h>
#ifndef MINIBIOS_BUILD
#include "IioInitPei.h"
#endif //MINIBIOS_BUILD

/**

    This routine will apply all Si Workarounds for DMI
    if required.

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
VOID
InitDmiSpecificSiWorkaorunds  (
  IN IIO_GLOBALS *IioGlobalData
  )
{
  UINT8 Bus;
  UINT32 ClassCode;
  UINT8 IioIndex;

  if (IioGlobalData->IioVar.IioVData.MultiPch) {
    // WA for Multi-PCH, Send a dummy write to non legacy PCH D17:F0:Off:0 to make sure MROM0 and MROM1 can capture the bus number.
	// Make sure this is done before any read\write access to non legacy PCH.
	for(IioIndex = 0; IioIndex < MaxIIO; IioIndex++ ){
	  if (!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]) continue;

	  Bus = IioGlobalData->IioVar.IioVData.SocketBaseBusNumber[IioIndex];
	  ClassCode = IioPciExpressRead32 (IioGlobalData, IioIndex, Bus, 0, 0, 0x8); // Read RID (Class code in offset 0x9)
	  if ((ClassCode >> 8) == V_PCIE_CCR_PCI_HOST_BRIDGE) {  // If RP configured as DMI then send dummy write
	    IioPciExpressWrite16 ( IioGlobalData, IioIndex, Bus, 17, 0, 0, 0x8086);
	  }
	}
  }	  
}

/**

    This routine will initialize Vc/Tc settings 
    if required.

    @param IioGlobalData - Pointer to IIO_GLOBALS

    @retval None

**/
IIO_STATUS
IioGetDmiVcTcSettings (
  IN IIO_GLOBALS               *IioGlobalData
  )
{
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  if ( IioGlobalData->IioVar.IioVData.IsocEnable == 0) {
     IioGlobalData->IioVar.IioVData.DmiVc[DmiTypeVc1] = 0;
   } else { /// Use default values
     IioGlobalData->IioVar.IioVData.DmiVc[DmiTypeVc1] = 1;
  }
  IioGlobalData->IioVar.IioVData.DmiVc[DmiTypeVcm] = (IioGlobalData->IioVar.IioVData.meRequestedSize == 0) ? 0 : 1;
#else
  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "Disable all DMI VCx\n");
  IioGlobalData->IioVar.IioVData.IsocEnable = 0;
  IioGlobalData->IioVar.IioVData.meRequestedSize = 0;
  IioGlobalData->IioVar.IioVData.DmiVc[DmiTypeVc1] = 0;
  IioGlobalData->IioVar.IioVData.DmiVc[DmiTypeVcm] = 0;
#endif // ME_SUPPORT_FLAG
  return IIO_SUCCESS;
}


/**

  Map SA DMI TCs to VC
  
  @param PchDmiTcVcMapPpi     - Instance of PCH_DMI_TC_VC_MAP
  @param DmiBar               - DMIBAR address

  @retval EFI_SUCCESS

**/
IIO_STATUS
IioSetDmiTcVcMapping (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT64              DmiBar
  )
{
  UINT32  Data32And;
  UINT32  Data32Or;
  UINT8   Data8And;
  UINT8   Data8Or;
  UINT8   Index;
  UINT16  Register;
  UINT8   VcId;
  UINT8   VcMap[MaxDmiVcType] = {0};

  //
  // Set the TC/VC mappings
  //
  for (Index = 0; Index < MaxDmiTcType; Index++) {
    VcMap[IioGlobalData->IioVar.IioVData.DmiTc[Index]] |= (BIT0 << Index);
  }
  //
  // 6.2.3.1  Virtual Channel 0 Configuration
  // a. System BIOS must ensure that only TC0 is mapped to VC0:
  //
  // Set DMIBAR Offset 014h [7:0] = `00010001b'
  // Set DMIBAR Offest 20h  [7:0] = `00100010b'
  // Set DMIBAR Offeset 2Ch [7:0] = `01000100b'
  //
  // 6.2.3.2 Virtual Channel 1 Configuration
  // Step1. Assign Virtual Channel ID 1 to VC1:
  //    Set the DMIVC1RCTL DMI Port Register DMIBAR Offset 020h[26:24]=001b
  //
  // Step2. Set TC1 and TC5 (Traffic Class) to VC1:
  //    Program the DMIVC1RCTL DMI Port Register DMIBAR Offset 020h[7:1] = '0010001b' .
  //
  // Step3. Enable VC1 by programming the DMIVC1RCTL DMI Port Register DMIBAR Offset 020h[31] = '1b'.
  //
  //
  // 6.2.3.3 Private Virtual Channel Configuration
  // Step1. Assign Virtual Channel ID 2 to VCp:
  //    Programming the DMIVCPRCTL DMI Port Register DMIBAR Offset 02Ch[26:24] = '010b'.
  //
  // Step2. Set TC2 and TC6 (Traffic Class) to VC2:
  //    Program the DMIVCPRCTL DMI Port Register DMIBAR Offset 02Ch[7:1] = '0100010b' .
  //
  //-----------------------------------------------------------------------------------------------------
  // Step3. Enable VCp by programming the DMIVCPRCTL DMI Port Register DMIBAR Offset 02Ch[31] = '1b'.
  //
  //-----------------------------------------------------------------------------------------------------
  // 6.2.3.4 Virtual Channel for ME (VCm) Configuration
  // This is configured by ConfigMemMe
  //
  // Step1. Assign Virtual Channel ID 7 to VCm:
  //    Programming the DMIVCMRCTL DMI Port Register DMIBAR Offset 038h[26:24] = '111b'.
  //
  // Step2. Enable VCm:
  //    Programming the DMIVMPRCTL DMI Port Register DMIBAR Offset 038h[31] = '1b'.
  //
  // Step3. Enable VCm by programming the DMIVCMRCTL DMI Port Register DMIBAR Offset 038h[31] = '1b'.
  //
  for (Index = 0; Index < MaxDmiVcType; Index++) {
    if (IioGlobalData->IioVar.IioVData.DmiVc[Index]) {
      //
      // Map TCs to VC, Set the VC ID, Enable VC
      //
      VcId = IioGlobalData->IioVar.IioVData.DmiVcId[Index];

      Data32And = (UINT32) (~(B_IIO_RCRB_DMIVCxRCTL_VCx_ID_MASK | B_IIO_RCRB_DMIVCxRCTL_VCx_TCVCxM_MASK));
      Data32Or = VcId << 24;
      Data32Or |= VcMap[Index];
      Data32Or |= B_IIO_RCRB_DMIVCxRCTL_VCx_EN;

      switch (Index) {
      case DmiTypeVc0:
        Register = R_IIO_RCRB_DMIVC0RCTL;
        break;

      case DmiTypeVc1:
        Register = R_IIO_RCRB_DMIVC1RCTL;
        break;
      case DmiTypeVcm:
        Register = R_IIO_RCRB_DMIVCMRCTL;
        break;

      default:
        return IIO_INVALID_PARAMETER;
      }

      IioMmioAndThenOr32 (IioGlobalData, (UINT32) (DmiBar + Register), Data32And, Data32Or);
    }
  }

  //
  // 6.2.3.1 b. System BIOS must program the extended VC Count:
  //    Set the DMI Port Register DMIBAR Offset 004h[2:0]=001b
  //
  Data8And = (UINT8) (~0x07);
  if (IioGlobalData->IioVar.IioVData.DmiVc[DmiTypeVc1]) {
    Data8Or = 1;
  } else {
    Data8Or = 0;
  }

  IioMmioAndThenOr8 (IioGlobalData, (UINT32) (DmiBar + R_IIO_RCRB_DMIPVCCAP1), Data8And, Data8Or);

  return IIO_SUCCESS;
}


/**

  Poll SA DMI negotiation completion
  
  @param PchDmiTcVcMapPpi     - Instance of PCH_DMI_TC_VC_MAP
  @param DmiBar               - DMIBAR address

  @retval EFI_SUCCESS

**/
IIO_STATUS
IioPollDmiVcStatus (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT64              DmiBar
  )
{
  UINT8   Index;
  UINT16  Register;

  //
  // 6.2.3.2 - Step 4, Poll until VC1 has been negotiated
  //    Read the DMIVC1RSTS DMI Port Register Offset 026h until [1]==0
  //
  // 6.2.3.3 - Step4. Poll the VCp Negotiation Pending bit until it reads 0:
  //    Read the DMIVCPRSTS DMI Port Register Offset 032h until [1]==0
  //
  // 6.2.3.4 - Step4. Poll the VCm Negotiation Pending bit until it reads 0:
  //    Read the DMIVCMRSTS DMI Port Register Offset 03Eh until [1]==0
  //
  for (Index = 0; Index < MaxDmiVcType; Index ++) {
    if (IioGlobalData->IioVar.IioVData.DmiVc[Index]) {
      switch (Index) {
      case DmiTypeVc0:
        Register = R_IIO_RCRB_DMIVC0RSTS;
        break;

      case DmiTypeVc1:
        Register = R_IIO_RCRB_DMIVC1RSTS;
        break;
//
// 4926908: SKX not support Vcp
//
      case DmiTypeVcm:
        Register = R_IIO_RCRB_DMIVCMRSTS;
        break;

      default:
        return IIO_INVALID_PARAMETER;
      }
#ifndef MINIBIOS_BUILD
      if (!(IioGlobalData->IioVar.IioVData.Emulation & SIMICS_FLAG)) {
        //
        // Wait for negotiation to complete
        //
        while ((IioMmioRead16 (IioGlobalData, (UINT32)(DmiBar + Register)) & B_IIO_RCRB_DMIVCxRSTS_VCx_NP) != 0);
      }
#endif //MINIBIOS_BUILD
    }
  }

  return IIO_SUCCESS;
}

/**

    This routine will program DMi TcVc mappings for IIO and PCH side.
    Also it will update LNKCTRL PCH side to support DMI Max Link Speed.

    @param IioGlobalData    - Pointer to IIO_GLOBALS structure

    @retval IIO_SUCCESS          - Configuration was completed without any error.

**/
IIO_STATUS
IioDmiTcVcSetup (
  IN  IIO_GLOBALS                 *IioGlobalData
  )
{
  UINT32            DmiBar;
  IIOMISCCTRL_N1_IIO_VTD_STRUCT IioMiscCtrlN1;

  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "DMI TcVc Setup Start\n");
  //
  //  Apply any DMI specific Si workarounds first
  //
  InitDmiSpecificSiWorkaorunds(IioGlobalData);
#ifndef MINIBIOS_BUILD
  switch (IioGlobalData->SetupData.PciePortLinkSpeed[0]) {
    // Gen1 is not supported on this configuration.
    case 2: //Gen2
      IioDmiGen2Prog (IioGlobalData);
      break;
    case 0: //Auto
    default:
    case 3: //Gen3
      //
      // For DMI 3.0
      // First Program PCH DMI to gen3
      //
      IioDmiGen3Prog(IioGlobalData);
    break;
    }
#endif // MINIBIOS_BUILD

  if (IioGlobalData->IioVar.IioVData.PciResourceMem32Limit[0] <= IioGlobalData->IioVar.IioVData.PciResourceMem32Base[0]) {
    return IIO_OUT_OF_RESOURCES;
  }
  
  DmiBar = (UINT64) IioGlobalData->IioVar.IioVData.StackPciResourceMem32Limit[0][0] & (~0xFFF);

  //
  // Update DmiTcVcMapping Settings
  //
  IioGetDmiVcTcSettings(IioGlobalData);
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "DEBUG:::: DmiVc1 = %x ; DmiVcm = %x\n", IioGlobalData->IioVar.IioVData.DmiVc[DmiTypeVc1], IioGlobalData->IioVar.IioVData.DmiVc[DmiTypeVcm]);
  if (!(IioGlobalData->IioVar.IioVData.DmiVc[DmiTypeVc1] + IioGlobalData->IioVar.IioVData.DmiVc[DmiTypeVcm]))
    return IIO_SUCCESS;

  //
  // Temporarily enable DmiRcBar at a valid address
  //
  IioWriteCpuCsr32(IioGlobalData, 0, PORT_0_INDEX, DMIRCBAR_IIO_PCIEDMI_REG, (UINT32)DmiBar+ BIT0 );
  //
  // Temporarily Unlock mmio access to DmiRcbar
  //
  IioMiscCtrlN1.Data = IioReadCpuCsr32(IioGlobalData, 0, 0, IIOMISCCTRL_N1_IIO_VTD_REG);
  IioMiscCtrlN1.Bits.mmiolock = 0x0;
  IioWriteCpuCsr32(IioGlobalData, 0, 0, IIOMISCCTRL_N1_IIO_VTD_REG, IioMiscCtrlN1.Data);

  //
  // Call PchDmiTcVcProg
  //
#ifndef MINIBIOS_BUILD
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "DEBUG::::  Calling IioDmiTcVcProg\n");
  IioDmiTcVcProg(IioGlobalData);
#endif // MINIBIOS_BUILD

  //
  // Program NB TC/VC mapping
  //
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "DEBUG::::  SetDmiTcVcMapping\n");
  IioSetDmiTcVcMapping (IioGlobalData, DmiBar);
  //
  // Poll NB negotiation completion
  //
  IioPollDmiVcStatus (IioGlobalData, DmiBar);
  //
  // Call PchDmiTcVcProgPoll
  //
#ifndef MINIBIOS_BUILD
  //
  // Call PchDmiTcVcProgPoll
  //
  IioDmiTcVcPoll(IioGlobalData);
#endif // MINIBIOS_BUILD

  //
  // Turn off the DmiRcBar so it can be appropriately allocated later in BIOS
  //
  IioWriteCpuCsr32(IioGlobalData, 0, PORT_0_INDEX, DMIRCBAR_IIO_PCIEDMI_REG, 0);
  //
  // Lock Mmio access to DmiRcbar
  //
  IioMiscCtrlN1.Data = IioReadCpuCsr32(IioGlobalData, 0, 0, IIOMISCCTRL_N1_IIO_VTD_REG);
  IioMiscCtrlN1.Bits.mmiolock = 1;
  IioWriteCpuCsr32(IioGlobalData, 0, 0, IIOMISCCTRL_N1_IIO_VTD_REG, IioMiscCtrlN1.Data);
  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_INFO, "DEBUG:::: DMI TcVc Setup Done\n");

  return IIO_SUCCESS;

}

