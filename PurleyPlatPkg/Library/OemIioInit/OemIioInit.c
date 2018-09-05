/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


    @file OemIioInit.c

    Oem Hooks file for IioInit Module

**/

#include <SysFunc.h>
#include <IioSetupDefinitions.h>
#include <IioUtilityLib.h>
#include <OemIioInit.h>
#include <RcRegs.h>



/**
    Auto determine which PCIe Root port to be hidden if its
    lanes are assigned to its preceding root port...use the
    Setup option variable of ConfigIOU to determine which ports
    are to be hidden on each IOU for corresponding IIO

    @param IOUx                  - IOUx Index
    @param IioIndex              - Index to Iio
    @param CurrentIOUConfigValue - Current IOU config value
    @param LaneReversed          - Flag to enable Lanereversal
    @param PXPHide               - Pointer to PXPHide array
    @param HidePEXPMenu          - Pointer to HidePEXPMenu array

    @retval None

**/
VOID
DeducePEXPHideFromIouBif  (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINTN     IOUx,
  IN  UINTN     IioIndex,
  IN  UINT8     CurrentIOUConfigValue,
  IN OUT  UINT8     *PXPHide,
  IN OUT  UINT8     *HidePEXPMenu
  )
{
  if( CurrentIOUConfigValue == 0xFF){
    IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR,"ERROR: Invalid IOUx Bifurcation =%x\n", CurrentIOUConfigValue);
    return;
  }

  switch (IOUx) {
  case  0:   // IOU0 has 4 PCIe root ports 1A, 1B, 1C, 1D
    switch(CurrentIOUConfigValue)
    {
      case 1:
        PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1B_INDEX] = 1;        // hide 1B
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1B_INDEX] = 1;   // hide the Setup menu for 1B
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1C_INDEX] = 0;   // make sure the port 1C is not hidden
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1D_INDEX] = 0;   // make sure the port 1D is not hidden
        break;
      case 2:
        PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1D_INDEX] = 1;        // hide port 1D
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1D_INDEX] = 1;   // hide the Setup menu for 1D
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1B_INDEX] = 0;   // make sure the port 1B is not hidden
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1C_INDEX] = 0;   // make sure the port 1C is not hidden
        break;
      case 3:
        PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1B_INDEX] = 1;        // hide 1B
        PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1D_INDEX] = 1;        // hide 1D
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1B_INDEX] = 1;   // hide the Setup menu for 1B
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1D_INDEX] = 1;   // hide the Setup menu for 1D
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1C_INDEX] = 0;   // make sure the port 1C 1s not hidden
        break;
      case 4:
        PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1B_INDEX] = 1;        // hide 1B
        PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1C_INDEX] = 1;        // hide 1C
        PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1D_INDEX] = 1;        // hide 1D
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1B_INDEX] = 1;   // hide the Setup menu for 1B
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1C_INDEX] = 1;   // hide the Setup menu for 1C
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1D_INDEX] = 1;   // hide the Setup menu for 1D
        break;
      default:
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1B_INDEX] = 0;
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1C_INDEX] = 0;
        HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_1D_INDEX] = 0;
        break;
      }
      break;
  case  1:   // IOU1 has the 4 PCIe root ports 2A, 2B, 2C, 2D
    switch(CurrentIOUConfigValue)
    {
    case 1:
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2B_INDEX] = 1;        // hide 2B
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2B_INDEX] = 1;   // hide the Setup menu for 2B
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2C_INDEX] = 0;   // make sure the port 2C is not hidden
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2D_INDEX] = 0;   // make sure the port 2D is not hidden
      break;
    case 2:
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2D_INDEX] = 1;        // hide port 2D
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2D_INDEX] = 1;   // hide the Setup menu for 2D
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2B_INDEX] = 0;   // make sure the port 2B is not hidden
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2C_INDEX] = 0;   // make sure the port 2C is not hidden
      break;
    case 3:
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2B_INDEX] = 1;        // hide 2B
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2D_INDEX] = 1;        // hide 2D
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2B_INDEX] = 1;   // hide the Setup menu for 2B
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2D_INDEX] = 1;   // hide the Setup menu for 2D
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2C_INDEX] = 0;   // make sure the port 2C is not hidden
      break;
    case 4:
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2B_INDEX] = 1;        // hide 2B
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2C_INDEX] = 1;        // hide 2C
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2D_INDEX] = 1;        // hide 2D
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2B_INDEX] = 1;   // hide the Setup menu for 2B
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2C_INDEX] = 1;   // hide the Setup menu for 2C
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2D_INDEX] = 1;   // hide the Setup menu for 2D
      break;
    default:
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2B_INDEX] = 0;
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2C_INDEX] = 0;
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_2D_INDEX] = 0;
      break;
    }
    break;

  case  2:   // IOU2 has the 4 PCIe root ports 3A, 3B, 3C, 3D
    switch(CurrentIOUConfigValue)
    {
    case 1:
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3B_INDEX] = 1;        // hide port 3B
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3B_INDEX] = 1;   // hide the Setup menu for 3B
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3C_INDEX] = 0;   // make sure the port 3C is not hidden
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3D_INDEX] = 0;  // make sure the port 3D is not hidden
      break;
    case 2:
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3D_INDEX] = 1;       // hide port 3D
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3D_INDEX] = 1;  // hide the Setup menu for 3D
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3B_INDEX] = 0;   // make sure port 3B menu is not hidden
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3C_INDEX] = 0;   // make sure port 3C menu is not hidden
      break;
    case 3:
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3B_INDEX] = 1;        // hide 3B
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3D_INDEX] = 1;       // hide 3D
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3B_INDEX] = 1;   // hide the Setup menu for 3B
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3D_INDEX] = 1;  // hide the Setup menu for 3D
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3C_INDEX] = 0;   // make sure port 3C menu is not hidden
      break;
    case 4:
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3B_INDEX] = 1;        // hide 3B
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3C_INDEX] = 1;        // hide 3C
      PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3D_INDEX] = 1;        // hide 3D
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3B_INDEX] = 1;   // hide the Setup menu for 3B
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3C_INDEX] = 1;   // hide the Setup menu for 3C
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3D_INDEX] = 1;  // hide the Setup menu for 3D
      break;
    default:
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3B_INDEX] = 0;
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3C_INDEX] = 0;
      HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_3D_INDEX] = 0;
      break;
    }
    break;
  case  3:   // MCP0 has the 4 PCIe root ports 4A, 4B, 4C, 4D
    switch(CurrentIOUConfigValue)
        {
        case 1:
          PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4B_INDEX] = 1;        // hide port 4B
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4B_INDEX] = 1;   // hide the Setup menu for 4B
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4C_INDEX] = 0;   // make sure the port 4C is not hidden
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4D_INDEX] = 0;   // make sure the port 4D is not hidden
          break;
        case 2:
          PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4D_INDEX] = 1;       // hide port 4D
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4D_INDEX] = 1;  // hide the Setup menu for 4D
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4B_INDEX] = 0;  // make sure port 4B menu is not hidden
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4C_INDEX] = 0;  // make sure port 4C menu is not hidden
          break;
        case 3:
          PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4B_INDEX] = 1;       // hide 4B
          PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4D_INDEX] = 1;       // hide 4D
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4B_INDEX] = 1;  // hide the Setup menu for 4B
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4D_INDEX] = 1;  // hide the Setup menu for 4D
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4C_INDEX] = 0;  // make sure port 4C menu is not hidden
          break;
        case 4:
          PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4B_INDEX] = 1;        // hide 4B
          PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4C_INDEX] = 1;        // hide 4C
          PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4D_INDEX] = 1;        // hide 4D
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4B_INDEX] = 1;   // hide the Setup menu for 4B
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4C_INDEX] = 1;   // hide the Setup menu for 4C
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4D_INDEX] = 1;   // hide the Setup menu for 4D
          break;
        default:
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4B_INDEX] = 0;
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4C_INDEX] = 0;
          HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_4D_INDEX] = 0;
          break;
        }
      break;
    case  4:   // MCP1 has the 4 PCIe root ports 5A, 5B, 5C, 5D
      switch(CurrentIOUConfigValue)
          {
          case 1:
            PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5B_INDEX] = 1;        // hide port 5B
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5B_INDEX] = 1;   // hide the Setup menu for 5B
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5C_INDEX] = 0;   // make sure the port 5C is not hidden
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5D_INDEX] = 0;   // make sure the port 5D is not hidden
            break;
          case 2:
            PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5D_INDEX] = 1;       // hide port 5D
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5D_INDEX] = 1;  // hide the Setup menu for 5D
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5B_INDEX] = 0;  // make sure port 5B menu is not hidden
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5C_INDEX] = 0;  // make sure port 5C menu is not hidden
            break;
          case 3:
            PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5B_INDEX] = 1;       // hide 5B
            PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5D_INDEX] = 1;       // hide 5D
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5B_INDEX] = 1;  // hide the Setup menu for 5B
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5D_INDEX] = 1;  // hide the Setup menu for 5D
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5C_INDEX] = 0;  // make sure port 5C menu is not hidden
            break;
          case 4:
            PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5B_INDEX] = 1;        // hide 5B
            PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5C_INDEX] = 1;        // hide 5C
            PXPHide[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5D_INDEX] = 1;        // hide 5D
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5B_INDEX] = 1;   // hide the Setup menu for 5B
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5C_INDEX] = 1;   // hide the Setup menu for 5C
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5D_INDEX] = 1;   // hide the Setup menu for 5D
            break;
          default:
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5B_INDEX] = 0;
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5C_INDEX] = 0;
            HidePEXPMenu[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PORT_5D_INDEX] = 0;
            break;
        }
     break;
  }
  return;
}

/**

  Initialize the Global Data Structure with the IOUx Bifurcation
  with default values

  @param IioGlobalData - Pointer to IioGlobalData

  @retval VOID           All other error conditions encountered result in an ASSERT

**/
VOID
OemDefaultIioPortBifurationInfo(
    IN  IIO_GLOBALS  *IioGlobalData
  )
{
  UINT8 IioIndex;

  for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++){

    if (IioGlobalData->SetupData.ConfigIOU0[IioIndex] == IIO_BIFURCATE_AUTO) {
       IioGlobalData->SetupData.ConfigIOU0[IioIndex] = IIO_BIFURCATE_xxxxxx16;
    }

    if (IioGlobalData->SetupData.ConfigIOU1[IioIndex] == IIO_BIFURCATE_AUTO) {
        IioGlobalData->SetupData.ConfigIOU1[IioIndex] = IIO_BIFURCATE_xxxxxx16;
    }

    if (IioGlobalData->SetupData.ConfigIOU2[IioIndex] == IIO_BIFURCATE_AUTO) {
       IioGlobalData->SetupData.ConfigIOU2[IioIndex] = IIO_BIFURCATE_xxx8xxx8;
    }

    if (IioGlobalData->SetupData.ConfigMCP0[IioIndex] == IIO_BIFURCATE_AUTO) {
         IioGlobalData->SetupData.ConfigMCP0[IioIndex] = IIO_BIFURCATE_xxxxxx16;
    }

    if (IioGlobalData->SetupData.ConfigMCP1[IioIndex] == IIO_BIFURCATE_AUTO) {
          IioGlobalData->SetupData.ConfigMCP1[IioIndex] = IIO_BIFURCATE_xxxxxx16;
    }

    //
    // Hide the root ports whose lanes are assigned preceding ports
    //
    DeducePEXPHideFromIouBif(IioGlobalData, 0, IioIndex, IioGlobalData->SetupData.ConfigIOU0[IioIndex],
           IioGlobalData->SetupData.PEXPHIDE, IioGlobalData->SetupData.HidePEXPMenu);

    DeducePEXPHideFromIouBif(IioGlobalData, 1, IioIndex, IioGlobalData->SetupData.ConfigIOU1[IioIndex],
            IioGlobalData->SetupData.PEXPHIDE, IioGlobalData->SetupData.HidePEXPMenu);

    DeducePEXPHideFromIouBif(IioGlobalData, 2, IioIndex, IioGlobalData->SetupData.ConfigIOU2[IioIndex],
            IioGlobalData->SetupData.PEXPHIDE, IioGlobalData->SetupData.HidePEXPMenu);

  }

}


/**

  Initialize the Global Data Structure with the Default Values 

  @param IioGlobalData - Pointer to IioGlobalData

  @retval NONE

**/
VOID
OemSetIioDefaultValues(
  IN OUT IIO_GLOBALS *IioGlobalData
  )
{
  UINT8  Index;
  UINT8  IioIndex;
  UINT8  PortIndex;
  //
  // Set all values to zero by default
#ifdef IA32
  MemSetLocal ((UINT8 *)&(IioGlobalData->SetupData), 0x00, sizeof (IIO_CONFIG));
#else
  ZeroMem(&(IioGlobalData->SetupData), sizeof(IIO_CONFIG));
#endif //IA32
  //
  // Override Only the ones that are not Zero by default
  //
  IioGlobalData->SetupData.VTdSupport                   = IIO_OPTION_ENABLE;
  IioGlobalData->SetupData.ATS                          = IIO_OPTION_ENABLE;
  IioGlobalData->SetupData.VtdAcsWa                     = IIO_OPTION_ENABLE;
  IioGlobalData->SetupData.CompletionTimeoutGlobal      = IIO_OPTION_ENABLE;
  IioGlobalData->SetupData.CompletionTimeoutGlobalValue = COMPLETION_TIMEOUT_260MS_900MS;
  IioGlobalData->SetupData.SnoopResponseHoldOff         = SNOOP_RESP_DEF_VALUE;
  IioGlobalData->SetupData.PCIe_LTR                     = IIO_OPTION_AUTO;
  IioGlobalData->SetupData.PcieExtendedTagField         = IIO_OPTION_ENABLE;
  IioGlobalData->SetupData.PCIe_AtomicOpReq             = IIO_OPTION_DISABLE;
  IioGlobalData->SetupData.DualCvIoFlow                 = IIO_OPTION_ENABLE;
  IioGlobalData->SetupData.PcieBiosTrainEnable          = IIO_OPTION_ENABLE;
  IioGlobalData->SetupData.McastIndexPosition           = MC_INDEX_POS_12;
  IioGlobalData->SetupData.McastNumGroup                = MC_NUM_GROUP_8;
  IioGlobalData->SetupData.DmiAllocatingFlow            = IIO_OPTION_ENABLE;
  IioGlobalData->SetupData.PcieAllocatingFlow           = IIO_OPTION_ENABLE;

  for(Index=0; Index < MaxIIO; Index++){
    IioGlobalData->SetupData.IioPresent[Index]          = IIO_OPTION_ENABLE;
    IioGlobalData->SetupData.ConfigIOU0[Index]          = CONFIG_IOU_AUTO;
    IioGlobalData->SetupData.ConfigIOU1[Index]          = CONFIG_IOU_AUTO;
    IioGlobalData->SetupData.ConfigIOU2[Index]          = CONFIG_IOU_AUTO;
    IioGlobalData->SetupData.ConfigMCP0[Index]          = CONFIG_IOU_AUTO;
    IioGlobalData->SetupData.ConfigMCP1[Index]          = CONFIG_IOU_AUTO;

    IioGlobalData->SetupData.CompletionTimeoutValue[Index] = COMPLETION_TIMEOUT_260MS_900MS;
  }

  for(Index=0; Index < ( NUMBER_PORTS_PER_SOCKET * MAX_SOCKET); Index++){
    IioIndex = Index/NUMBER_PORTS_PER_SOCKET;
    PortIndex = Index%NUMBER_PORTS_PER_SOCKET;

	if (!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]){
		continue;
	   }

    IioGlobalData->SetupData.PcieAspm[Index]         = PCIE_ASPM_AUTO;

    if(IioGlobalData->IioVar.IioVData.SkuPersonality[IioIndex] == TYPE_FPGA){
      if((PortIndex >= PORT_4A_INDEX)&&(PortIndex <=PORT_4D_INDEX)){
        IioGlobalData->SetupData.PcieAspm[Index]         = PCIE_ASPM_L0S_ONLY;
      }
    }

    IioGlobalData->SetupData.PcieCommonClock[Index]      = IIO_OPTION_ENABLE;
    IioGlobalData->SetupData.PcieMaxPayload[Index]       = IIO_OPTION_AUTO;
    IioGlobalData->SetupData.PcieL0sLatency[Index]       = PCIE_L0S_4US_8US;
    IioGlobalData->SetupData.PcieL1Latency[Index]        = PCIE_L1_8US_16US;
    //
    // PCIE setup options for MISCCTRLSTS
    //
    IioGlobalData->SetupData.EOI[Index]         = IIO_OPTION_ENABLE;
    IioGlobalData->SetupData.DISL0STx[Index]    = IIO_OPTION_ENABLE;
  }

#if MAX_SOCKET > 4
  for (Index=0; Index < TOTAL_CB3_DEVICES/2; Index++){
    IioGlobalData->SetupData.Cb3DmaEn[Index] = IIO_OPTION_ENABLE;
    // To avoid linker error: unresolved smybol memset 
    IioGlobalData->SetupData.Cb3DmaEn[Index + (TOTAL_CB3_DEVICES/2)] = IioGlobalData->SetupData.Cb3DmaEn[Index];
  }
#else
  for (Index=0; Index < TOTAL_CB3_DEVICES; Index++){
    IioGlobalData->SetupData.Cb3DmaEn[Index] = IIO_OPTION_ENABLE;
  }
#endif

  for (Index=0; Index < MAX_NTB_PORTS; Index++){
    IioGlobalData->SetupData.NtbBarSizePBar23[Index] = NTB_BARSIZE_PBAR23_DEFAULT;
    IioGlobalData->SetupData.NtbBarSizePBar45[Index] = NTB_BARSIZE_PBAR45_DEFAULT;
    IioGlobalData->SetupData.NtbBarSizePBar4[Index]  = NTB_BARSIZE_PBAR4_DEFAULT;
    IioGlobalData->SetupData.NtbBarSizePBar5[Index]  = NTB_BARSIZE_PBAR5_DEFAULT;
    IioGlobalData->SetupData.NtbBarSizeSBar23[Index] = NTB_BARSIZE_SBAR23_DEFAULT;
    IioGlobalData->SetupData.NtbBarSizeSBar45[Index] = NTB_BARSIZE_SBAR45_DEFAULT;
    IioGlobalData->SetupData.NtbBarSizeSBar4[Index]  = NTB_BARSIZE_SBAR4_DEFAULT;
    IioGlobalData->SetupData.NtbBarSizeSBar5[Index]  = NTB_BARSIZE_SBAR5_DEFAULT;
    IioGlobalData->SetupData.NtbXlinkCtlOverride[Index] = NTB_IIO_XLINK_CTL_DSD_USP;
  }

  for (Index=0; Index < MAX_NTB_PORTS; Index++){
     IioGlobalData->SetupData.VMDCfgBarSz[Index]  = VMD_CFG_BAR_SIZE_DEFAULT;
     IioGlobalData->SetupData.VMDMemBarSz1[Index] = VMD_MEM_BAR_SIZE1_DEFAULT;
     IioGlobalData->SetupData.VMDMemBarSz2[Index] = VMD_MEM_BAR_SIZE2_DEFAULT;
  }

}

/**

  OEM Hook before bus number is assigned to DMI (earliest PCI enumeration point) but after chipset DMI
  Initialization occurs (chipset DMI initialization before resources are assigned).

   @param IioGlobalData   Pointer to the IIO Global for this driver

   @retval VOID              All other error conditions
           encountered result in an ASSERT

**/
VOID
OemDmiPreInit (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN UINT8          IioIndex,
  IN UINT8          PortIndex
  )
{
}

/**

  OEM Hook before resources (Bus number is assigned, other resources aren't) are assigned to DMI but after
  chipset DMI Initialization occurs.

   @param IioGlobalData   Pointer to the IIO Global for this driver

   @retval VOID              All other error conditions
           encountered result in an ASSERT

**/
VOID
OemDmiDeviceInit (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN UINT8          IioIndex,
  IN UINT8          PortIndex
  )
{
}

/**

  OEM Hook after all Dmi resources are assigned and all DMI Chipset init is complete.

   @param IioGlobalData   Pointer to the IIO Global for this driver

   @retval VOID              All other error conditions encountered result in an ASSERT

**/
VOID
OemDmiResourceAssignedInit (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN UINT8          IioIndex,
  IN UINT8          PortIndex
  )
{
}

#ifdef MINIBIOS_BUILD
/**
 
  This function returns the Uplink port details from the IIO
               side, as per the platform board schematics, if no
               uplink port than return 0xFF for both IIO and
               port index.
  
  @param  IioIndex - Index to Iio
  
  @param  PortIndex - Index to Iio Port 
  @param  Bus       - Bus where uplink is located 
  @param  Device       - Device where uplink is located 
  @param  Function       - Function where uplink is located
  
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
  //
  //init the input variables
  //
  *PortIndex  = 0xFF;
  *Bus        = 0xFF;
  *Dev        = 0xFF;
  *Func       = 0xFF;
  //
  // override with platform specific data
  //
  if (IioIndex == 0){
    *PortIndex  = IIO_UPLINK_PORT_INDEX; //platform specific uplink port index
    *Bus        = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[0][*PortIndex];
    *Dev        = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[*PortIndex].Device;
    *Func       = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[*PortIndex].Function;
  }
  return;
}
#endif // MINIBIOS_BUILD

/**

  OEM Hook to apply any Si WA

  @param IioGlobalData         - IIO Global data structure
  @param IioIndex              - Socket Index
  @param Phase                 - Before/After Bifurcation

  @retval  NONE

**/
VOID
OemIioEarlyWorkAround (
    IIO_GLOBALS                     *IioGlobalData,
    UINT8                           IioIndex,
    IIO_INIT_ENUMERATION            Phase
  )
{

  switch(Phase) {
    case IioBeforeBifurcation:
      break;
    case IioAfterBifurcation:
      break;
    default:
      break;
  }

  return;
}

/**

  OEM Hook to override the IIO DXF settings

  @param IioGlobalData         - IIO Global data structure
  @param IioIndex              - Socket Index
  @param Phase                 - Before/After Bifurcation

  @retval  NONE

**/
VOID
OemOverrideIioRxRecipeSettings (
    IIO_GLOBALS                     *IioGlobalData,
    UINT8                           IioIndex
  )
{

}
