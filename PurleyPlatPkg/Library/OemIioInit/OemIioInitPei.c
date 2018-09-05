//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.04
//      Bug Fixed:  Fix bifurcation wrong with FPGA CPU.
//      Reason:     FPGA IOU3 change to IOU1 and pcie reverse.
//      Auditor:    Kasber Chen
//      Date:       Jun/08/2017
//
//  Rev. 1.03
//      Bug Fixed:  Support FPGA CPU.
//      Reason:     FPGA IOU3 change to IOU1 and pcie reverse.
//      Auditor:    Kasber Chen
//      Date:       Jun/08/2017
//
//  Rev. 1.02
//      Bug Fixed:  Support FPGA CPU.
//      Reason:     FPGA CPU need to swap PE1 and PE3
//      Auditor:    Kasber Chen
//      Date:       May/22/2017
//
//  Rev. 1.01
//      Bug Fixed:  Fix Bus dynamic change issue.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Sep/06/2016
//
//  Rev. 1.00
//      Bug Fixed:  Add PCIE bifurcation setting in PEI.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Jun/06/2016
//
//*****************************************************************************
/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    OemIioInitPei.c

Abstract:

    Oem Hooks file for IioInit Module

--*/
#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/SystemBoard.h>
#include <IioPlatformData.h>
#include <IioSetupDefinitions.h>
#include <IioUtilityLib.h>
#include <Guid/IioPolicyHob.h>
#include <OemIioInit.h>
#include "OemIioInitCommon.h"
#include <../../Build/Token.h>
#if SMCPKG_SUPPORT
#ifndef __SMC_KTI_OVERRIDE__
#define __SMC_KTI_OVERRIDE__
#endif
#include "../../SmcPkg/Include/SuperMPeiDriver.h"
extern EFI_GUID  gSuperMPeiPrococolGuid;
#endif

VOID
OemGetIioPlatformInfo (
  IN IIO_GLOBALS *IioGlobalData
  )
{
  EFI_PLATFORM_INFO             *mPlatformInfo;
  EFI_HOB_GUID_TYPE             *GuidHob;

  GuidHob    = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT(GuidHob != NULL);
  if (GuidHob == NULL) {
    return;
  }
  mPlatformInfo = GET_GUID_HOB_DATA (GuidHob);

  //
  // Update Other variables required for IIO Init
  //
  IioGlobalData->IioVar.IioVData.PlatformType   = mPlatformInfo->BoardId;
}


VOID
OemGetIioPortsBifurcation (
  IN  IIO_GLOBALS  *IioGlobalData
  )
{
  EFI_STATUS   Status;
  EFI_PEI_SERVICES  **PeiServices;
#if SMCPKG_SUPPORT
  SUPERM_PEI_DRIVER_PROTOCOL	*mSuperMPeiDriver = NULL;
  UINT8         i, j;
  UINT64        TempCPUBus;
  UINTN         CPU_IIO_Bus_Tbl[] = {
                          PcdToken(PcdSmcCPU0IIOBus), PcdToken(PcdSmcCPU1IIOBus),
                          PcdToken(PcdSmcCPU2IIOBus), PcdToken(PcdSmcCPU3IIOBus)};
#endif
  SYSTEM_BOARD_PPI  *mSystemBoard;

  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
#if SMCPKG_SUPPORT
  for(i = 0; i < MAX_SOCKET; i++){	//max 4 CPU
      TempCPUBus = 0;
      for(j = 0; j < MAX_IIO_STACK; j++){
          if(IioGlobalData->IioVar.IioVData.StackPresentBitmap[i] & (1 << j))
              TempCPUBus |= LShiftU64(IioGlobalData->IioVar.IioVData.SocketStackBus[i][j], j * 8);
      }
      LibPcdSet64(CPU_IIO_Bus_Tbl[i], TempCPUBus);
  }
  
  Status = (**PeiServices).LocatePpi(
                    PeiServices,
                    &gSuperMPeiPrococolGuid,
                    0,
                    NULL,
                    &mSuperMPeiDriver);
  
  ASSERT_EFI_ERROR(Status);
  
  mSuperMPeiDriver->GetSuperMPcieBifurcation(PeiServices, &(IioGlobalData->SetupData));
//exchange pcie3, pcie1 bifurcation and settings when CPU is FPGA
  for(i = 0; i < MAX_SOCKET; i++){
      if(IioGlobalData->IioVar.IioVData.FpgaActive[i]){
          DEBUG((-1, "FPGA CPU, save IOU3 bifurcate and set flag.\n"));
//save IOU3 bifurcation to bit 48~55 and set FPGA flag(bit56)
          
          if(IioGlobalData->SetupData.ConfigIOU0[i] == IIO_BIFURCATE_x4x4xxx8)
              IioGlobalData->SetupData.ConfigIOU2[i] = IIO_BIFURCATE_xxx8x4x4;
          else if(IioGlobalData->SetupData.ConfigIOU0[i] == IIO_BIFURCATE_xxx8x4x4)
              IioGlobalData->SetupData.ConfigIOU2[i] = IIO_BIFURCATE_x4x4xxx8;
          else
              IioGlobalData->SetupData.ConfigIOU2[i] = IioGlobalData->SetupData.ConfigIOU0[i];

		  TempCPUBus = LibPcdGet64(CPU_IIO_Bus_Tbl[i]) + BIT56;
          TempCPUBus |= LShiftU64(IioGlobalData->SetupData.ConfigIOU2[i], 48);          
          LibPcdSet64(CPU_IIO_Bus_Tbl[i], TempCPUBus);
//

//if IOU3 = x4x4x4x4, x4x4xxx8, xxx8x4x4, xxx8xxx8, 3A = 1C, 3B = 1D, 3C = 1A, 3D = 1B
          if(IioGlobalData->SetupData.ConfigIOU2[i] < 4){
              for(j = 0; j < 2; j++){   //3A = 1C, 3B = 1D
                  IioGlobalData->SetupData.SLOTIMP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTIMP[i * NUMBER_PORTS_PER_SOCKET + 3 + j];
                  IioGlobalData->SetupData.SLOTHPCAP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTHPCAP[i * NUMBER_PORTS_PER_SOCKET + 3 + j];
                  IioGlobalData->SetupData.SLOTHPSUP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTHPSUP[i * NUMBER_PORTS_PER_SOCKET + 3 + j];
                  IioGlobalData->SetupData.SLOTAIP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTAIP[i * NUMBER_PORTS_PER_SOCKET + 3 + j];
                  IioGlobalData->SetupData.SLOTPIP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTPIP[i * NUMBER_PORTS_PER_SOCKET + 3 + j];
                  IioGlobalData->SetupData.SLOTABP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTABP[i * NUMBER_PORTS_PER_SOCKET + 3 + j];
                  IioGlobalData->SetupData.SLOTPCP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTPCP[i * NUMBER_PORTS_PER_SOCKET + 3 + j];
                  IioGlobalData->SetupData.VppEnable[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.VppEnable[i * NUMBER_PORTS_PER_SOCKET + 3 + j];
                  IioGlobalData->SetupData.VppPort[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.VppPort[i * NUMBER_PORTS_PER_SOCKET + 3 + j];
                  IioGlobalData->SetupData.VppAddress[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.VppAddress[i * NUMBER_PORTS_PER_SOCKET + 3 + j];
                  IioGlobalData->SetupData.VMDPortEnable[i * 12 + 8 + j] = \
                      IioGlobalData->SetupData.VMDPortEnable[i * 12 + 2 + j];
              }
              for(j = 2; j < 4; j++){   //3C = 1A, 3D = 1B
                  IioGlobalData->SetupData.SLOTIMP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTIMP[i * NUMBER_PORTS_PER_SOCKET + j - 1];
                  IioGlobalData->SetupData.SLOTHPCAP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTHPCAP[i * NUMBER_PORTS_PER_SOCKET + j - 1];
                  IioGlobalData->SetupData.SLOTHPSUP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTHPSUP[i * NUMBER_PORTS_PER_SOCKET + j - 1];
                  IioGlobalData->SetupData.SLOTAIP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTAIP[i * NUMBER_PORTS_PER_SOCKET + j - 1];
                  IioGlobalData->SetupData.SLOTPIP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTPIP[i * NUMBER_PORTS_PER_SOCKET + j - 1];
                  IioGlobalData->SetupData.SLOTABP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTABP[i * NUMBER_PORTS_PER_SOCKET + j - 1];
                  IioGlobalData->SetupData.SLOTPCP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTPCP[i * NUMBER_PORTS_PER_SOCKET + j - 1];
                  IioGlobalData->SetupData.VppEnable[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.VppEnable[i * NUMBER_PORTS_PER_SOCKET + j - 1];
                  IioGlobalData->SetupData.VppPort[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.VppPort[i * NUMBER_PORTS_PER_SOCKET + j - 1];
                  IioGlobalData->SetupData.VppAddress[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.VppAddress[i * NUMBER_PORTS_PER_SOCKET + j - 1];
                  IioGlobalData->SetupData.VMDPortEnable[i * 12 + 8 + j] = \
                      IioGlobalData->SetupData.VMDPortEnable[i * 12 + j - 2];
              }
          } else {      //IOU = x16, auto
              for(j = 0; j < 4; j++){
                  IioGlobalData->SetupData.SLOTIMP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTIMP[i * NUMBER_PORTS_PER_SOCKET + 1 + j];
                  IioGlobalData->SetupData.SLOTHPCAP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTHPCAP[i * NUMBER_PORTS_PER_SOCKET + 1 + j];
                  IioGlobalData->SetupData.SLOTHPSUP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTHPSUP[i * NUMBER_PORTS_PER_SOCKET + 1 + j];
                  IioGlobalData->SetupData.SLOTAIP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTAIP[i * NUMBER_PORTS_PER_SOCKET + 1 + j];
                  IioGlobalData->SetupData.SLOTPIP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTPIP[i * NUMBER_PORTS_PER_SOCKET + 1 + j];
                  IioGlobalData->SetupData.SLOTABP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTABP[i * NUMBER_PORTS_PER_SOCKET + 1 + j];
                  IioGlobalData->SetupData.SLOTPCP[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.SLOTPCP[i * NUMBER_PORTS_PER_SOCKET + 1 + j];
                  IioGlobalData->SetupData.VppEnable[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.VppEnable[i * NUMBER_PORTS_PER_SOCKET + 1 + j];
                  IioGlobalData->SetupData.VppPort[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.VppPort[i * NUMBER_PORTS_PER_SOCKET + 1 + j];
                  IioGlobalData->SetupData.VppAddress[i * NUMBER_PORTS_PER_SOCKET + 9 + j] = \
                      IioGlobalData->SetupData.VppAddress[i * NUMBER_PORTS_PER_SOCKET + 1 + j];
                  IioGlobalData->SetupData.VMDPortEnable[i * 12 + 8 + j] = \
                      IioGlobalData->SetupData.VMDPortEnable[i * 12 + j];
              }
          }
          IioGlobalData->SetupData.VMDEnabled[i * 3 + 2] = IioGlobalData->SetupData.VMDEnabled[i * 3];
//clear IOU3 settings
          IioGlobalData->SetupData.VMDEnabled[i * 3] = 0;
          for(j = 0; j < 4; j++){
              IioGlobalData->SetupData.SLOTIMP[i * NUMBER_PORTS_PER_SOCKET + 1 + j] = 0;
              IioGlobalData->SetupData.SLOTHPCAP[i * NUMBER_PORTS_PER_SOCKET + 1 + j] = 0;
              IioGlobalData->SetupData.SLOTHPSUP[i * NUMBER_PORTS_PER_SOCKET + 1 + j] = 0;
              IioGlobalData->SetupData.SLOTAIP[i * NUMBER_PORTS_PER_SOCKET + 1 + j] = 0;
              IioGlobalData->SetupData.SLOTPIP[i * NUMBER_PORTS_PER_SOCKET + 1 + j] = 0;
              IioGlobalData->SetupData.SLOTABP[i * NUMBER_PORTS_PER_SOCKET + 1 + j] = 0;
              IioGlobalData->SetupData.SLOTPCP[i * NUMBER_PORTS_PER_SOCKET + 1 + j] = 0;
              IioGlobalData->SetupData.VppEnable[i * NUMBER_PORTS_PER_SOCKET + 1 + j] = 0;
              IioGlobalData->SetupData.VppPort[i * NUMBER_PORTS_PER_SOCKET + 1 + j] = 0;
              IioGlobalData->SetupData.VppAddress[i * NUMBER_PORTS_PER_SOCKET + 1 + j] = 0;
              IioGlobalData->SetupData.VMDPortEnable[i * 12 + j] = 0;
          }
      }
  }
#endif
  //
  // Get SystemBoard PPI
  //
  Status = (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiSystemBoardPpiGuid,
                  0,
                  NULL,
                  &mSystemBoard
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize all Bifurcation settings once
  //

  mSystemBoard->SystemIioPortBifurcationInit (IioGlobalData);

}

/**
 
  This function returns the Uplink port details from the IIO
               side, as per the platform board schematics, if no
               uplink port than return 0xFF for both IIO and
               port index.
  
  @param  IioIndex - Index to Iio
  
  @param  PortIndex - Index to Iio Port 
  @param  Bus       - Bus where uplink is located 
  @param  Device    - Device where uplink is located 
  @param  Function  - Function where uplink is located
  
  @retval  CPU index & its port index
  
 **/
VOID
OemIioUplinkPortDetails(
    IIO_GLOBALS  *IioGlobalData,
    UINT8   IioIndex,
    UINT8   *PortIndex,
    UINT8   *Bus,
    UINT8   *Dev,
    UINT8   *Func
)
{
  EFI_STATUS   Status;
  EFI_PEI_SERVICES  **PeiServices;
  SYSTEM_BOARD_PPI  *mSystemBoard;

  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
  Status = (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiSystemBoardPpiGuid,
                  0,
                  NULL,
                  &mSystemBoard
                  );
  ASSERT_EFI_ERROR (Status);
  *PortIndex  = 0xFF;
  *Bus        = 0xFF;
  *Dev        = 0xFF;
  *Func       = 0xFF;
  *PortIndex = mSystemBoard->GetUplinkPortInformation(IioIndex);
  if ((*PortIndex != 0xFF) && (*PortIndex < NUMBER_PORTS_PER_SOCKET)) {
    *Bus        = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][*PortIndex];
    *Dev        = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[*PortIndex].Device;
    *Func       = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[*PortIndex].Function;
  }	
  return; 
}

//
// 4926908: SKX not support Vcp
//

/**

    This function checks if we are running under Simics environment

    @param None

    @retval IsSimicsPlatform  - TRUE/FALSE if Simics


**/
UINT8
CheckSoftSimulationPlatform(
  VOID
  )
{
  return (UINT8)IsSimicsPlatform();
}



/**

    This function update SRIS configuration settings
    based on the IOUx bifurcation.

    @param IioGlobalData - Pointer to IioGlobalData

    @retval None

**/
VOID
OemUpdateSrisConfig (
  IN IIO_GLOBALS   *IioGlobalData
  )
{
  UINT8 IioIndex;
  UINT8 Iou;
  UINT8 PXPOffset;
  UINT8 CurrentIOUConfigValue;

  CurrentIOUConfigValue = 0;

  for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++){
    if(!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex])
      continue;
   IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "OemUpdateSrisConfig Start\n");
    for(Iou = 0; Iou < IioIouMax; Iou++){
      PXPOffset = (IioIndex * NUMBER_PORTS_PER_SOCKET);
      switch (Iou) {
        case IioIou0:
          CurrentIOUConfigValue = IioGlobalData->SetupData.ConfigIOU0[IioIndex];
          PXPOffset+= PORT_1A_INDEX;
          break;
        case IioIou1:
          CurrentIOUConfigValue = IioGlobalData->SetupData.ConfigIOU1[IioIndex];
          PXPOffset+= PORT_2A_INDEX;
          break;
        case IioIou2:
          CurrentIOUConfigValue = IioGlobalData->SetupData.ConfigIOU2[IioIndex];
          PXPOffset+= PORT_3A_INDEX;
          break;
        case IioMcp0:
          CurrentIOUConfigValue = IioGlobalData->SetupData.ConfigMCP0[IioIndex];
          PXPOffset+= PORT_4A_INDEX;
          break;
        case IioMcp1:
          CurrentIOUConfigValue = IioGlobalData->SetupData.ConfigMCP1[IioIndex];
          PXPOffset += PORT_5A_INDEX;
          break;
      }
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "SRIS: Iou = %x, CurrentIOUConfigValue = %x \n", Iou, CurrentIOUConfigValue);
      switch(CurrentIOUConfigValue){
        case IIO_BIFURCATE_x4x4xxx8:
          if (IioGlobalData->SetupData.SRIS[PXPOffset + IioPortA] == IIO_OPTION_ENABLE){
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortB] = IIO_OPTION_ENABLE;
          } else {
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortB] = IIO_OPTION_DISABLE;
          }
          break;
        case IIO_BIFURCATE_xxx8x4x4:
          if (IioGlobalData->SetupData.SRIS[PXPOffset + IioPortC] == IIO_OPTION_ENABLE){
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortD] = IIO_OPTION_ENABLE;
          } else {
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortD] = IIO_OPTION_DISABLE;
          }
          break;
        case IIO_BIFURCATE_xxx8xxx8:
          if (IioGlobalData->SetupData.SRIS[PXPOffset + IioPortA] == IIO_OPTION_ENABLE){
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortB] = IIO_OPTION_ENABLE;
          } else {
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortB] = IIO_OPTION_DISABLE;
          }
          if (IioGlobalData->SetupData.SRIS[PXPOffset + IioPortC] == IIO_OPTION_ENABLE){
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortD] = IIO_OPTION_ENABLE;
          } else {
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortD] = IIO_OPTION_DISABLE;
          }
          break;
        case IIO_BIFURCATE_xxxxxx16:
          if( IioGlobalData->SetupData.SRIS[PXPOffset + IioPortA] == IIO_OPTION_ENABLE){
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortB] = IIO_OPTION_ENABLE;
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortC] = IIO_OPTION_ENABLE;
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortD] = IIO_OPTION_ENABLE;
          } else {
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortB] = IIO_OPTION_DISABLE;
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortC] = IIO_OPTION_DISABLE;
            IioGlobalData->SetupData.SRIS[PXPOffset + IioPortD] = IIO_OPTION_DISABLE;
          }
          break;
        default:
          // Do nothing we will conserve user configuration
          break;
       }
    }
  }
}

/**

  Initialize the Global Data Structure with the IOUx Bifurcation
  based on Board layout or user selection

  @param IioGlobalData - Pointer to IioGlobalData

  @retval VOID              All other error conditions encountered result in an ASSERT

**/
VOID
OemInitIioPortBifurcationInfo (
  IN  IIO_GLOBALS  *IioGlobalData
  )
{
  IioDebugPrintInfo(IioGlobalData,IIO_DEBUG_ERROR,"Calling OemGetIioPortBifurcation Start\n");
  OemGetIioPortsBifurcation (IioGlobalData);
  IioDebugPrintInfo(IioGlobalData,IIO_DEBUG_ERROR,"Calling OemGetIioPortBifurcation End\n");
  OemUpdateSrisConfig (IioGlobalData);
}

/**

  Initialize the Global Data Structure with the Setup Values
  read from NVRAM

  @param IioGlobalData - Pointer to IioGlobalData

  @retval VOID              All other error conditions encountered result in an ASSERT

**/
VOID
OemSetPlatformDataValues(
  IN OUT IIO_GLOBALS *IioGlobalData
  )
{
  EFI_STATUS            Status;
  // APTIOV_SERVER_OVERRIDE_RC_START
  INTEL_SETUP_DATA            SetupData;
  // APTIOV_SERVER_OVERRIDE_RC_END
  UINT16  Index;
  UINT8   Socket; // Used to copy setup option from S0-S3 into S4-S
  UINT8   Stack;
  UINT8 MaxTotalPorts;

  // APTIOV_SERVER_OVERRIDE_RC_START
  ZeroMem(&SetupData, sizeof(INTEL_SETUP_DATA));
  // APTIOV_SERVER_OVERRIDE_RC_END
  Status = GetEntireConfig(&SetupData);

  IioGlobalData->SetupData.VTdSupport                   = SetupData.SocketConfig.IioConfig.VTdSupport;
  IioGlobalData->SetupData.InterruptRemap               = SetupData.SocketConfig.IioConfig.InterruptRemap;
  IioGlobalData->SetupData.CoherencySupport             = SetupData.SocketConfig.IioConfig.CoherencySupport;
  IioGlobalData->SetupData.ATS                          = SetupData.SocketConfig.IioConfig.ATS;
  IioGlobalData->SetupData.PassThroughDma               = SetupData.SocketConfig.IioConfig.PassThroughDma;
  IioGlobalData->SetupData.PostedInterrupt              = SetupData.SocketConfig.IioConfig.PostedInterrupt;
  IioGlobalData->SetupData.VtdAcsWa                     = SetupData.SocketConfig.IioConfig.VtdAcsWa;

  IioGlobalData->SetupData.CompletionTimeoutGlobal      = SetupData.SocketConfig.IioConfig.CompletionTimeoutGlobal;
  IioGlobalData->SetupData.CompletionTimeoutGlobalValue = SetupData.SocketConfig.IioConfig.CompletionTimeoutGlobalValue;
  IioGlobalData->SetupData.CoherentReadPart             = SetupData.SocketConfig.IioConfig.CoherentReadPart;
  IioGlobalData->SetupData.CoherentReadFull             = SetupData.SocketConfig.IioConfig.CoherentReadFull;
  IioGlobalData->SetupData.PcieGlobalAspm               = SetupData.SocketConfig.IioConfig.PcieGlobalAspm;
  IioGlobalData->SetupData.StopAndScream                = SetupData.SocketConfig.IioConfig.StopAndScream;
  IioGlobalData->SetupData.SnoopResponseHoldOff         = SetupData.SocketConfig.IioConfig.SnoopResponseHoldOff;
  IioGlobalData->SetupData.PCIe_LTR                     = SetupData.SocketConfig.IioConfig.PCIe_LTR;
  IioGlobalData->SetupData.PcieExtendedTagField         = SetupData.SocketConfig.IioConfig.PcieExtendedTagField;
  IioGlobalData->SetupData.PCIe_AtomicOpReq             = SetupData.SocketConfig.IioConfig.PCIe_AtomicOpReq;
  IioGlobalData->SetupData.PcieMaxReadRequestSize       = SetupData.SocketConfig.IioConfig.PcieMaxReadRequestSize;
  IioGlobalData->SetupData.DfxLtssmLogger               = SetupData.SocketConfig.IioConfig.DfxLtssmLogger;
  IioGlobalData->SetupData.DfxLtssmLoggerStop           = SetupData.SocketConfig.IioConfig.DfxLtssmLoggerStop;
  IioGlobalData->SetupData.DfxLtssmLoggerSpeed          = SetupData.SocketConfig.IioConfig.DfxLtssmLoggerSpeed;
  IioGlobalData->SetupData.DfxLtssmLoggerMask           = SetupData.SocketConfig.IioConfig.DfxLtssmLoggerMask;
  IioGlobalData->SetupData.DfxJitterLogger              = SetupData.SocketConfig.IioConfig.DfxJitterLogger;
  IioGlobalData->SetupData.DisableTPH                   = SetupData.SocketConfig.IioConfig.DisableTPH;
  IioGlobalData->SetupData.PrioritizeTPH                = SetupData.SocketConfig.IioConfig.PrioritizeTPH;
  IioGlobalData->SetupData.CbRelaxedOrdering            = SetupData.SocketConfig.IioConfig.CbRelaxedOrdering;
  IioGlobalData->SetupData.PCUF6Hide                    = SetupData.SocketConfig.IioConfig.PCUF6Hide;
  IioGlobalData->SetupData.EN1K                         = SetupData.SocketConfig.IioConfig.EN1K;
  IioGlobalData->SetupData.DualCvIoFlow                 = SetupData.SocketConfig.IioConfig.DualCvIoFlow;
  IioGlobalData->SetupData.PcieBiosTrainEnable          = SetupData.SocketConfig.IioConfig.PcieBiosTrainEnable;
  IioGlobalData->SetupData.MultiCastEnable              = SetupData.SocketConfig.IioConfig.MultiCastEnable;
  IioGlobalData->SetupData.McastBaseAddrRegion          = SetupData.SocketConfig.IioConfig.McastBaseAddrRegion;
  IioGlobalData->SetupData.McastIndexPosition           = SetupData.SocketConfig.IioConfig.McastIndexPosition;
  IioGlobalData->SetupData.McastNumGroup                = SetupData.SocketConfig.IioConfig.McastNumGroup;
  //
  // PCIE Global Option
  //
  IioGlobalData->SetupData.NoSnoopRdCfg                 = SetupData.SocketConfig.IioConfig.NoSnoopRdCfg;
  IioGlobalData->SetupData.NoSnoopWrCfg                 = SetupData.SocketConfig.IioConfig.NoSnoopWrCfg;
  IioGlobalData->SetupData.MaxReadCompCombSize          = SetupData.SocketConfig.IioConfig.MaxReadCompCombSize;
  IioGlobalData->SetupData.ProblematicPort              = SetupData.SocketConfig.IioConfig.ProblematicPort;
  IioGlobalData->SetupData.DmiAllocatingFlow            = SetupData.SocketConfig.IioConfig.DmiAllocatingFlow;
  IioGlobalData->SetupData.PcieAllocatingFlow           = SetupData.SocketConfig.IioConfig.PcieAllocatingFlow;
  IioGlobalData->SetupData.PcieHotPlugEnable            = SetupData.SocketConfig.IioConfig.PcieHotPlugEnable;
  IioGlobalData->SetupData.PcieAcpiHotPlugEnable        = SetupData.SocketConfig.IioConfig.PcieAcpiHotPlugEnable;
  IioGlobalData->SetupData.HaltOnDmiDegraded            = SetupData.SocketConfig.IioConfig.HaltOnDmiDegraded;
  IioGlobalData->SetupData.RxClockWA                    = SetupData.SocketConfig.IioConfig.RxClockWA;
  IioGlobalData->SetupData.GlobalPme2AckTOCtrl          = SetupData.SocketConfig.IioConfig.GlobalPme2AckTOCtrl;


  IioGlobalData->SetupData.PcieSlotOprom1   = SetupData.SocketConfig.IioConfig.PcieSlotOprom1;
  IioGlobalData->SetupData.PcieSlotOprom2   = SetupData.SocketConfig.IioConfig.PcieSlotOprom2;
  IioGlobalData->SetupData.PcieSlotOprom3   = SetupData.SocketConfig.IioConfig.PcieSlotOprom3;
  IioGlobalData->SetupData.PcieSlotOprom4   = SetupData.SocketConfig.IioConfig.PcieSlotOprom4;
  IioGlobalData->SetupData.PcieSlotOprom5   = SetupData.SocketConfig.IioConfig.PcieSlotOprom5;
  IioGlobalData->SetupData.PcieSlotOprom6   = SetupData.SocketConfig.IioConfig.PcieSlotOprom6;
  IioGlobalData->SetupData.PcieSlotOprom7   = SetupData.SocketConfig.IioConfig.PcieSlotOprom7;
  IioGlobalData->SetupData.PcieSlotOprom8   = SetupData.SocketConfig.IioConfig.PcieSlotOprom8;
  IioGlobalData->SetupData.PcieSlotItemCtrl = SetupData.SocketConfig.IioConfig.PcieSlotItemCtrl;
  IioGlobalData->SetupData.PcieRelaxedOrdering   = SetupData.SocketConfig.IioConfig.PcieRelaxedOrdering;
  IioGlobalData->SetupData.PciePhyTestMode  = SetupData.SocketConfig.IioConfig.PciePhyTestMode;
  IioGlobalData->SetupData.ProcessorMsrLockControl = SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorMsrLockControl;
  IioGlobalData->SetupData.Serr = SetupData.SystemConfig.PropagateSerr;
  IioGlobalData->SetupData.Perr = SetupData.SystemConfig.PropagatePerr;
  IioGlobalData->SetupData.LockChipset = SetupData.SocketConfig.SocketProcessorCoreConfiguration.LockChipset;
  IioGlobalData->SetupData.PeciInTrustControlBit = SetupData.SocketConfig.SocketProcessorCoreConfiguration.PeciInTrustControlBit;
  IioGlobalData->SetupData.LegacyVgaSoc = SetupData.SocketConfig.CsiConfig.LegacyVgaSoc;
  IioGlobalData->SetupData.LegacyVgaStack = SetupData.SocketConfig.CsiConfig.LegacyVgaStack;

  //
  // PCH
  //
  IioGlobalData->SetupData.PciePllSsc        = SetupData.PchRcConfig.PciePllSsc;

  // Get IOAPIC setup option for S4-S7 in the same way as S0-S3
  for (Stack = 0; Stack < TOTAL_IIO_STACKS; Stack ++) {
    IioGlobalData->SetupData.DevPresIoApicIio[Stack] = SetupData.SocketConfig.IioConfig.DevPresIoApicIio[(Stack < 24) ? Stack : Stack - 24];
  }
  // Disable the IOAPIC in the stack where FPGA connected.
  for (Socket=0; Socket < MaxIIO; Socket++) {
    if (IioGlobalData->IioVar.IioVData.FpgaActive[Socket] == 1) {
      IioGlobalData->SetupData.DevPresIoApicIio[Socket * MAX_IIO_STACK + IIO_PSTACK3] = 0;
    }
  }

  // PCIE RAS (Errors)
  IioGlobalData->SetupData.LerEn                    = SetupData.SystemConfig.LerEn;
  IioGlobalData->SetupData.MctpEn                   = SetupData.SocketConfig.IioConfig.MctpEn;
  IioGlobalData->SetupData.IioErrorEn               = SetupData.SystemConfig.IioErrorEn;
  IioGlobalData->SetupData.WheaPcieErrInjEn         = SetupData.SystemConfig.WheaPcieErrInjEn;
  IioGlobalData->SetupData.ProcessorX2apic          = SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorX2apic;
  IioGlobalData->SetupData.ProcessorMsrLockControl  = SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorMsrLockControl;

  //
  //  Setup Option per Socket
  //
  for(Socket=0; Socket < MaxIIO; Socket++){
    if (IioGlobalData->IioVar.IioVData.SocketPresent[Socket] == 0) continue;
    // Copy BIFUR setup option from S0-S3 into S4-S7
    Index = (Socket < 4) ? Socket : Socket - 4;

    IioGlobalData->SetupData.IioPresent[Socket]             = SetupData.SocketConfig.IioConfig.IioPresent[Socket];
    IioGlobalData->SetupData.ConfigIOU0[Socket]             = SetupData.SocketConfig.IioConfig.ConfigIOU0[Index];
    IioGlobalData->SetupData.ConfigIOU1[Socket]             = SetupData.SocketConfig.IioConfig.ConfigIOU1[Index];
    IioGlobalData->SetupData.ConfigIOU2[Socket]             = SetupData.SocketConfig.IioConfig.ConfigIOU2[Index];

    IioGlobalData->SetupData.ConfigMCP0[Socket]             = SetupData.SocketConfig.IioConfig.ConfigMCP0[Index];
    IioGlobalData->SetupData.ConfigMCP1[Socket]             = SetupData.SocketConfig.IioConfig.ConfigMCP1[Index];

    IioGlobalData->SetupData.CompletionTimeout[Socket]      = SetupData.SocketConfig.IioConfig.CompletionTimeout[Index];
    IioGlobalData->SetupData.CompletionTimeoutValue[Socket] = SetupData.SocketConfig.IioConfig.CompletionTimeoutValue[Index];
    IioGlobalData->SetupData.RpCorrectableErrorEsc[Socket]           = SetupData.SocketConfig.IioConfig.RpCorrectableErrorEsc[Index];
    IioGlobalData->SetupData.RpUncorrectableNonFatalErrorEsc[Socket] = SetupData.SocketConfig.IioConfig.RpUncorrectableNonFatalErrorEsc[Index];
    IioGlobalData->SetupData.RpUncorrectableFatalErrorEsc[Socket]    = SetupData.SocketConfig.IioConfig.RpUncorrectableFatalErrorEsc[Index];
    IioGlobalData->SetupData.Cb3DcaEn[Socket]                        = SetupData.SocketConfig.IioConfig.Cb3DcaEn[Index];

  }

  //
  // Setup Options for MAX_TOTAL_PORTS
  //
#if MAX_SOCKET > 4
  MaxTotalPorts = (NUMBER_PORTS_PER_SOCKET * MAX_SOCKET) / 2;
#else
  MaxTotalPorts = (NUMBER_PORTS_PER_SOCKET * MAX_SOCKET);
#endif

  for(Index=0; Index < MaxTotalPorts; Index++){
    Socket = (UINT8)(Index / NUMBER_PORTS_PER_SOCKET);
    if (IioGlobalData->IioVar.IioVData.SocketPresent[Socket] == 0) continue;
    IioGlobalData->SetupData.SLOTHPCAP[Index]            = SetupData.SocketConfig.IioConfig.SLOTHPCAP[Index];
    IioGlobalData->SetupData.SLOTHPSUP[Index]            = SetupData.SocketConfig.IioConfig.SLOTHPSUP[Index];
    IioGlobalData->SetupData.PcieLinkDis[Index]          = SetupData.SocketConfig.IioConfig.PcieLinkDis[Index];
    IioGlobalData->SetupData.PcieAspm[Index]             = SetupData.SocketConfig.IioConfig.PcieAspm[Index];
    IioGlobalData->SetupData.PcieCommonClock[Index]      = SetupData.SocketConfig.IioConfig.PcieCommonClock[Index];
    IioGlobalData->SetupData.PcieMaxPayload[Index]       = SetupData.SocketConfig.IioConfig.PcieMaxPayload[Index];
    IioGlobalData->SetupData.PcieDState[Index]           = SetupData.SocketConfig.IioConfig.PcieDState[Index];
    IioGlobalData->SetupData.PcieL0sLatency[Index]       = SetupData.SocketConfig.IioConfig.PcieL0sLatency[Index];
    IioGlobalData->SetupData.PcieL1Latency[Index]        = SetupData.SocketConfig.IioConfig.PcieL1Latency[Index];
    IioGlobalData->SetupData.MsiEn[Index]                = SetupData.SocketConfig.IioConfig.MsiEn[Index];
    IioGlobalData->SetupData.ExtendedSync[Index]         = SetupData.SocketConfig.IioConfig.ExtendedSync[Index];
    IioGlobalData->SetupData.InbandPresenceDetect[Index] = SetupData.SocketConfig.IioConfig.InbandPresenceDetect[Index];
    IioGlobalData->SetupData.PciePortDisable[Index]      = SetupData.SocketConfig.IioConfig.PciePortDisable[Index];
    IioGlobalData->SetupData.PciePmeIntEn[Index]         = SetupData.SocketConfig.IioConfig.PciePmeIntEn[Index];
    IioGlobalData->SetupData.IODC[Index]                 = SetupData.SocketConfig.IioConfig.IODC[Index];
    //
    // PCIE setup options for Link Control2
    //
    IioGlobalData->SetupData.PciePortLinkSpeed[Index]       = SetupData.SocketConfig.IioConfig.PciePortLinkSpeed[Index];
    IioGlobalData->SetupData.ComplianceMode[Index]          = SetupData.SocketConfig.IioConfig.ComplianceMode[Index];
    IioGlobalData->SetupData.PciePortLinkMaxWidth[Index]    = SetupData.SocketConfig.IioConfig.PciePortLinkMaxWidth[Index];
    IioGlobalData->SetupData.DeEmphasis[Index]              = SetupData.SocketConfig.IioConfig.DeEmphasis[Index];

    //
    // PCIE setup options for MISCCTRLSTS
    //
    IioGlobalData->SetupData.EOI[Index]         = SetupData.SocketConfig.IioConfig.EOI[Index];
    IioGlobalData->SetupData.MSIFATEN[Index]    = SetupData.SocketConfig.IioConfig.MSIFATEN[Index];
    IioGlobalData->SetupData.MSINFATEN[Index]   = SetupData.SocketConfig.IioConfig.MSINFATEN[Index];
    IioGlobalData->SetupData.MSICOREN[Index]    = SetupData.SocketConfig.IioConfig.MSICOREN[Index];
    IioGlobalData->SetupData.ACPIPMEn[Index]    = SetupData.SocketConfig.IioConfig.ACPIPMEn[Index];
    IioGlobalData->SetupData.DISL0STx[Index]    = SetupData.SocketConfig.IioConfig.DISL0STx[Index];
    IioGlobalData->SetupData.P2PWrtDis[Index]   = SetupData.SocketConfig.IioConfig.P2PWrtDis[Index];
    IioGlobalData->SetupData.P2PRdDis[Index]    = SetupData.SocketConfig.IioConfig.P2PRdDis[Index];
    IioGlobalData->SetupData.DisPMETOAck[Index] = SetupData.SocketConfig.IioConfig.DisPMETOAck[Index];
    IioGlobalData->SetupData.ACPIHP[Index]      = SetupData.SocketConfig.IioConfig.ACPIHP[Index];
    IioGlobalData->SetupData.ACPIPM[Index]      = SetupData.SocketConfig.IioConfig.ACPIPM[Index];
    IioGlobalData->SetupData.SRIS[Index]        = SetupData.SocketConfig.IioConfig.SRIS[Index];
    IioGlobalData->SetupData.TXEQ[Index]        = SetupData.SocketConfig.IioConfig.TXEQ[Index];
    IioGlobalData->SetupData.ECRC[Index]        = SetupData.SocketConfig.IioConfig.ECRC[Index];

    IioGlobalData->SetupData.PcieUnsupportedRequests[Index] = SetupData.SocketConfig.IioConfig.PcieUnsupportedRequests[Index];

    IioGlobalData->SetupData.PEXPHIDE[Index]                    = SetupData.SocketConfig.IioConfig.PEXPHIDE[Index];
    IioGlobalData->SetupData.HidePEXPMenu[Index]                 = SetupData.SocketConfig.IioConfig.HidePEXPMenu[Index];

    //
    // DFX Gen3
    //
    IioGlobalData->SetupData.DfxGen3OverrideMode[Index]       = SetupData.SocketConfig.IioConfig.DfxGen3OverrideMode[Index];
    IioGlobalData->SetupData.DfxGen3TestCard[Index]         = SetupData.SocketConfig.IioConfig.DfxGen3TestCard[Index];
    IioGlobalData->SetupData.DfxGen3ManualPh2_Precursor[Index]    = SetupData.SocketConfig.IioConfig.DfxGen3ManualPh2_Precursor[Index];
    IioGlobalData->SetupData.DfxGen3ManualPh2_Cursor[Index]     = SetupData.SocketConfig.IioConfig.DfxGen3ManualPh2_Cursor[Index];
    IioGlobalData->SetupData.DfxGen3ManualPh2_Postcursor[Index]     = SetupData.SocketConfig.IioConfig.DfxGen3ManualPh2_Postcursor[Index];
    IioGlobalData->SetupData.DfxGen3ManualPh3_Precursor[Index]    = SetupData.SocketConfig.IioConfig.DfxGen3ManualPh3_Precursor[Index];
    IioGlobalData->SetupData.DfxGen3ManualPh3_Cursor[Index]     = SetupData.SocketConfig.IioConfig.DfxGen3ManualPh3_Cursor[Index];
    IioGlobalData->SetupData.DfxGen3ManualPh3_Postcursor[Index]   = SetupData.SocketConfig.IioConfig.DfxGen3ManualPh3_Postcursor[Index];
    IioGlobalData->SetupData.DfxDnTxPreset[Index]         = SetupData.SocketConfig.IioConfig.DfxDnTxPreset[Index];
    IioGlobalData->SetupData.DfxRxPreset[Index]           = SetupData.SocketConfig.IioConfig.DfxRxPreset[Index];
    IioGlobalData->SetupData.DfxUpTxPreset[Index]         = SetupData.SocketConfig.IioConfig.DfxUpTxPreset[Index];

    //
    //
    //
    IioGlobalData->SetupData.InboundConfiguration[Index]          = SetupData.SocketConfig.IioConfig.InboundConfiguration[Index];

#if MAX_SOCKET > 4
      IioGlobalData->SetupData.SLOTHPCAP[Index + MAX_TOTAL_PORTS] = SetupData.SocketConfig.IioConfig.SLOTHPCAP[Index];
      IioGlobalData->SetupData.SLOTHPSUP[Index]            = SetupData.SocketConfig.IioConfig.SLOTHPSUP[Index];
      IioGlobalData->SetupData.PEXPHIDE[Index + MAX_TOTAL_PORTS]  = SetupData.SocketConfig.IioConfig.PEXPHIDE[Index];
#endif
  } // for (Index ...

  //
  // Setup Options per CB3 Devices
  //
#if MAX_SOCKET > 4
  for (Index=0; Index < TOTAL_CB3_DEVICES/2; Index++){
    IioGlobalData->SetupData.Cb3DmaEn[Index]     = SetupData.SocketConfig.IioConfig.Cb3DmaEn[Index];
    IioGlobalData->SetupData.Cb3NoSnoopEn[Index] = SetupData.SocketConfig.IioConfig.Cb3NoSnoopEn[Index];
    // To avoid linker error: unresolved smybol memset
    IioGlobalData->SetupData.Cb3DmaEn[Index + (TOTAL_CB3_DEVICES/2)] = IioGlobalData->SetupData.Cb3DmaEn[Index];
    IioGlobalData->SetupData.Cb3NoSnoopEn[Index + (TOTAL_CB3_DEVICES/2)] = IioGlobalData->SetupData.Cb3NoSnoopEn[Index];
  }
#else
  for (Index=0; Index < TOTAL_CB3_DEVICES; Index++){
    Socket = (UINT8)(Index / CB3_DEVICES_PER_SOCKET);
    if (IioGlobalData->IioVar.IioVData.SocketPresent[Socket] == 0) continue;
    IioGlobalData->SetupData.Cb3DmaEn[Index]     = SetupData.SocketConfig.IioConfig.Cb3DmaEn[Index];
    IioGlobalData->SetupData.Cb3NoSnoopEn[Index] = SetupData.SocketConfig.IioConfig.Cb3NoSnoopEn[Index];
  }
#endif


  //
  // Setup Options per MAX_NTB_PORTS
  // Need to enable these setup options for S4-S7
  //
  for (Index=0; Index < MAX_NTB_PORTS; Index++){
    Socket = (UINT8)(Index / NUMBER_NTB_PORTS_PER_SOCKET);
    if (IioGlobalData->IioVar.IioVData.SocketPresent[Socket] == 0) continue;
    IioGlobalData->SetupData.NtbPpd[Index] =  SetupData.SocketConfig.IioConfig.NtbPpd[Index];
    IioGlobalData->SetupData.NtbBarSizeOverride[Index] = SetupData.SocketConfig.IioConfig.NtbBarSizeOverride[Index];
    IioGlobalData->SetupData.NtbSplitBar[Index] = SetupData.SocketConfig.IioConfig.NtbSplitBar[Index];
    IioGlobalData->SetupData.NtbBarSizePBar23[Index] = SetupData.SocketConfig.IioConfig.NtbBarSizePBar23[Index];
    IioGlobalData->SetupData.NtbBarSizePBar45[Index] = SetupData.SocketConfig.IioConfig.NtbBarSizePBar45[Index];
    IioGlobalData->SetupData.NtbBarSizePBar4[Index]  = SetupData.SocketConfig.IioConfig.NtbBarSizePBar4[Index];
    IioGlobalData->SetupData.NtbBarSizePBar5[Index]  = SetupData.SocketConfig.IioConfig.NtbBarSizePBar5[Index];
    IioGlobalData->SetupData.NtbBarSizeSBar23[Index] = SetupData.SocketConfig.IioConfig.NtbBarSizeSBar23[Index];
    IioGlobalData->SetupData.NtbBarSizeSBar45[Index] = SetupData.SocketConfig.IioConfig.NtbBarSizeSBar45[Index];
    IioGlobalData->SetupData.NtbBarSizeSBar4[Index]  = SetupData.SocketConfig.IioConfig.NtbBarSizeSBar4[Index];
    IioGlobalData->SetupData.NtbBarSizeSBar5[Index]  = SetupData.SocketConfig.IioConfig.NtbBarSizeSBar5[Index];
    IioGlobalData->SetupData.NtbXlinkCtlOverride[Index] = SetupData.SocketConfig.IioConfig.NtbXlinkCtlOverride[Index];
  }

  //
  // Setup Options per MAX_VMD_PORTS
  //
  for (Index=0; Index < MAX_VMD_PORTS; Index++){
    Socket = (UINT8)(Index / VMD_PORTS_PER_SOCKET);
    if (IioGlobalData->IioVar.IioVData.SocketPresent[Socket] == 0) continue;
    IioGlobalData->SetupData.VMDPortEnable[Index] = SetupData.SocketConfig.IioConfig.VMDPortEnable[Index];
    IioGlobalData->SetupData.PcieAICPortEnable[Index] = SetupData.SocketConfig.IioConfig.PcieAICPortEnable[Index];
  }

  for (Index=0; Index < MAX_VMD_STACKS; Index++) {
    Socket = (UINT8)(Index / VMD_STACK_PER_SOCKET);
    if (IioGlobalData->IioVar.IioVData.SocketPresent[Socket] == 0){
      continue;
    }
    IioGlobalData->SetupData.PcieAICEnabled[Index]  = SetupData.SocketConfig.IioConfig.PcieAICEnabled[Index];
    IioGlobalData->SetupData.PcieAICHotPlugEnable[Index] = SetupData.SocketConfig.IioConfig.PcieAICHotPlugEnable[Index];
    IioGlobalData->SetupData.VMDEnabled[Index]  = SetupData.SocketConfig.IioConfig.VMDEnabled[Index];
    IioGlobalData->SetupData.VMDHotPlugEnable[Index]  = SetupData.SocketConfig.IioConfig.VMDHotPlugEnable[Index];
    IioGlobalData->SetupData.VMDCfgBarSz[Index] = SetupData.SocketConfig.IioConfig.VMDCfgBarSz[Index];
    IioGlobalData->SetupData.VMDCfgBarAttr[Index] = SetupData.SocketConfig.IioConfig.VMDCfgBarAttr[Index];
    IioGlobalData->SetupData.VMDMemBarSz1[Index] = SetupData.SocketConfig.IioConfig.VMDMemBarSz1[Index];
    IioGlobalData->SetupData.VMDMemBar1Attr[Index] = SetupData.SocketConfig.IioConfig.VMDMemBar1Attr[Index];
    IioGlobalData->SetupData.VMDMemBarSz2[Index] = SetupData.SocketConfig.IioConfig.VMDMemBarSz2[Index];
    IioGlobalData->SetupData.VMDMemBar2Attr[Index] = SetupData.SocketConfig.IioConfig.VMDMemBar2Attr[Index];
  }

  for (Index=0; Index < MAX_DEVHIDE_REGS_PER_SYSTEM; Index++){
    Socket = (UINT8)(Index / MAX_DEVHIDE_REGS);
    if (IioGlobalData->IioVar.IioVData.SocketPresent[Socket] == 0) continue;
    IioGlobalData->SetupData.DfxSocketDevFuncHide[Index] = SetupData.SocketConfig.IioConfig.DfxSocketDevFuncHide[Index];
  }
}

/**

  Publish OPAHob for HIFIGen3 driver usage 

  @param IioGlobalData - Pointer to IioGlobalData

  @retval VOID       

**/
VOID
OemPublishOpaSocketMapHob (
  IN IIO_GLOBALS *IioGlobalData
 )
{
  OPA_SOCKET_MAP_HOB    OpaSocketMapHob;
  VOID               *HobPtr;
  UINT8               Socket;

  ZeroMem(&OpaSocketMapHob, sizeof(OPA_SOCKET_MAP_HOB));

  for (Socket = 0; Socket < MaxIIO; Socket++) {
    if (IioGlobalData->IioVar.IioVData.SocketPresent[Socket] == 0){
      continue;
    }
    OpaSocketMapHob.Info[Socket].Valid = 1;
    OpaSocketMapHob.Info[Socket].SocketId = Socket;
    OpaSocketMapHob.Info[Socket].Segment = IioGlobalData->IioVar.IioVData.SegmentSocket[Socket];
    //
    // Only send the MCP (PSTACK3) Bus range
    //
    OpaSocketMapHob.Info[Socket].BusBase = IioGlobalData->IioVar.IioVData.SocketStackBaseBusNumber[Socket][IIO_PSTACK3];
    OpaSocketMapHob.Info[Socket].BusLimit = IioGlobalData->IioVar.IioVData.SocketStackLimitBusNumber[Socket][IIO_PSTACK3];
  }

  HobPtr = BuildGuidDataHob (&gEfiOpaSocketMapHobGuid, &OpaSocketMapHob, sizeof (OPA_SOCKET_MAP_HOB));
  if(HobPtr == NULL){
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "ERROR: gEfiOpaSocketMapHobGuid could not be Published\n");
  } else {
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR, "gEfiOpaSocketMapHobGuid Published!!!\n");
  }

}
