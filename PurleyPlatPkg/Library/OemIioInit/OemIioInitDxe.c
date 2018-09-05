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

    OemIioInitDxe.c

Abstract:

    Oem Hooks file for IioInit Module

--*/

#include <IioPlatformData.h>
#include <OemIioInit.h>
#include <IioUtilityLib.h>
#include "OemIioInitCommon.h"

/** 
 Invoque the SystemIioPortBifurcationInit to update IioGloblaData
 with the curent IIO bifurcation acording to the board detected. 
  
 @param  IioGlobalData  Pointer to the IIO Global for this driver. 
 
 @retval None 
**/ 
VOID
OemGetIioPortsBifurcation (
  IN OUT  IIO_GLOBALS  *IioGlobalData
  )
{
  EFI_STATUS   Status;
  SYSTEM_BOARD_PROTOCOL *mSystemBoard;

  Status = gBS->LocateProtocol (&gEfiDxeSystemBoardProtocolGuid, NULL, (VOID**)&mSystemBoard);
  ASSERT_EFI_ERROR(Status);
  mSystemBoard->SystemIioPortBifurcationInit (mSystemBoard, IioGlobalData);
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
  SYSTEM_BOARD_PROTOCOL *mSystemBoard;

  Status = gBS->LocateProtocol (&gEfiDxeSystemBoardProtocolGuid, NULL, (VOID**)&mSystemBoard);
  ASSERT_EFI_ERROR(Status);
  *PortIndex  = 0xFF;
  *Bus        = 0xFF;
  *Dev        = 0xFF;
  *Func       = 0xFF;
  *PortIndex = mSystemBoard->GetUplinkPortInformation(mSystemBoard, IioIndex);
  if ((*PortIndex != 0xFF) && (*PortIndex < NUMBER_PORTS_PER_SOCKET)) {
    *Bus        = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][*PortIndex];
    *Dev        = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[*PortIndex].Device;
    *Func       = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[*PortIndex].Function;
  }	
  return; 
}

/** 
  OEM Hook to check for particular support on IIO initialized devices and take action,
  such as install the proper protocol to indicate it is detected 
  
 @param  IioGlobalData  Pointer to the IIO Global for this driver. 
 
 @retval None 
**/ 
VOID
OemCheckForOtherSupport(
   IN IIO_GLOBALS * IioGlobalData
  )
{
  EFI_STATUS   Status;
  EFI_HANDLE   VMDHandle = NULL;
  EFI_HANDLE   HfiPcieGen3Handle = NULL;
  BOOLEAN      HfiPresent = FALSE;
  UINT8        Index;
  //
  //  492822: Add VMD driver OpRom to Purley BIOS
  //  Install Dummy protocol if VMD is enabled
  //
  if (IioGlobalData->SetupData.VMDEnabled) {
     Status = gBS->InstallProtocolInterface (
                         &VMDHandle,
                         &gEfiVMDDriverProtocolGuid,
                         EFI_NATIVE_INTERFACE,
                         NULL
                        );
     if (EFI_ERROR(Status)){
         IioDebugPrintInfo(IioGlobalData, IIO_ERROR,"VMD EFI Driver GUID NOT installed = %x!!!\n", Status);
     } else {
        IioDebugPrintInfo(IioGlobalData, IIO_ERROR,"VMD EFI Driver GUID installed successfully = %x!\n",Status);
     }
  }
  //
  // Check if HFI Enabled SKU is present and install protocol to indicate it is detected.
  //
  for (Index = 0; Index < MAX_SOCKET; Index++){
    if (IioGlobalData->IioVar.IioVData.SkuPersonality[Index] == TYPE_MCP) {
      HfiPresent = TRUE;
    }
  }
  if (HfiPresent) {
      Status = gBS->InstallProtocolInterface (
                           &HfiPcieGen3Handle,
                           &gEfiHfiPcieGen3ProtocolGuid,
                           EFI_NATIVE_INTERFACE,
                           NULL
                          );
       if (EFI_ERROR(Status)){
           IioDebugPrintInfo(IioGlobalData, IIO_ERROR,"HfiPcieGen3H EFI Driver GUID NOT installed = %x!!!\n", Status);
       } else {
           IioDebugPrintInfo(IioGlobalData, IIO_ERROR,"HfiPcieGen3H EFI Driver GUID installed successfully = %x!\n",Status);
       }
  }

}


/**
  This function update the HidePEXPMenu based on PciePort present values and
  do extra setup based on the IIO Features enabled.

  @param IioGlobalData     - Pointer to IioGlobalData

  @retval None
**/
VOID
OemUpdateHidePCIePortMenu (
  IN  IIO_GLOBALS  *IioGlobalData
  )
{
  EFI_STATUS Status;
  UINT8      HidePEXPMenuOption[MAX_TOTAL_PORTS];

  CopyMem (
    (VOID *)&HidePEXPMenuOption,
    (VOID *)&(IioGlobalData->SetupData.HidePEXPMenu),
    sizeof(HidePEXPMenuOption)
    );
  Status = SetOptionData (&gEfiSocketIioVariableGuid, OFFSET_OF(SOCKET_IIO_CONFIGURATION, HidePEXPMenu), &HidePEXPMenuOption, sizeof(HidePEXPMenuOption));

}
