/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

Copyright (c)  1999 - 2016  Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioEarlyInitialize.c

  Implement IIO initialization protocol to provide IIO initialization functions.
  This interfaces will be called by PciHostBridge driver in the PCI bus enumeration.

**/
#include <SysFunc.h>
#include <IioUtilityLib.h>
#include <OemIioInit.h>
#include <IioInitLib.h>
#include <IioInitPeiLib.h>
#include <RcRegs.h>
#include "IioGen3.h"
#include "IioEvDebug.h"
#include "IioWorkAround.h"
#include "IioDmiInit.h"
#include "IioPortInit.h"
#include "IioEarlyInitialize.h"

//
// Global Data Items
//
#pragma pack (push, 1)


typedef struct {
  UINT8   PortValue[4];
} PCIE_IOUPORT_STATUS;

PCIE_IOUPORT_STATUS PcieIouStatus [] = { // int  IOUX_array [5][4]
    {0x04, 0x04, 0x04, 0x04},     //   00 = x4  | x4 | x4 | x4    (IOU 2 & 3)
    {0x08, 0x00, 0x04, 0x04},     //   01 = x8  | -- | x4 | x4    (IOU 2 & 3)
    {0x04, 0x04, 0x08, 0x00},     //   02 = x4  | x4 | x8 | --    (IOU 2 & 3)
    {0x08, 0x00, 0x08, 0x00},     //   03 = x8  | -- | x8 | --    (IOU 2 & 3)
    {0x10, 0x00, 0x00, 0x00}      //   04 = x16 | -- | -- | --    (IOU 2 & 3)
};

#pragma pack (pop)


/**

    IioOtherVariableInit: Initialize any platform port specific settings here
                          for ex, Uplink port

    @param IioGlobalData - Pointer to Universal Data Structure

    @retval None

**/
VOID
IioOtherVariableInit (
  IN IIO_GLOBALS  *IioGlobalData,
  IN UINT8        IioIndex
  )
{
  UINT8                 IioUplinkPortIndex;
  UINT8                 Bus;
  UINT8                 Dev;
  UINT8                 Func;

  //
  // init any platform port specific settings here
  // for ex, Uplink port
  //
  IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_INFO, "  IioOtherVariableInit %d\n", IioIndex);
  OemIioUplinkPortDetails(IioGlobalData, IioIndex, &IioUplinkPortIndex, &Bus, &Dev, &Func);

  if ((IioIndex < MaxIIO) && (IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]) &&
      (IioUplinkPortIndex < NUMBER_PORTS_PER_SOCKET)) {

    IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_INFO, "  Found uplink port @ skt %d port %d\n", IioIndex, IioUplinkPortIndex);

    IioGlobalData->IioVar.PreLinkData.UplinkInfo[IioIndex].Valid = 1;
    IioGlobalData->IioVar.PreLinkData.UplinkInfo[IioIndex].IioUplinkPortIndex = IioUplinkPortIndex;
    IioGlobalData->IioVar.PreLinkData.UplinkInfo[IioIndex].UplinkPortInfo.Device = Dev;
    IioGlobalData->IioVar.PreLinkData.UplinkInfo[IioIndex].UplinkPortInfo.Function = Func;
  } else {
    IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_INFO, "  Platform does not support uplink port!\n");
  }
}

/**

    Update our PCIEXpress configuration in our status information  
    loop through 15 PCIE ports that correspond to the three IOUs

    @param IioGlobalData - IioGlobalData Pointer
    @param IioIndex      - Iio Index

    @retval None

**/
VOID
IioUpdatePcieConfigurationData (
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT8       IioIndex
)
{
  UINT8   i,j;
  UINT8   *Array=NULL;
  IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_INFO, " IioUpdatePcieConfigurationData Start IioIndex = %x!\n", IioIndex);
  if(IioIndex >= MaxIIO) {
    IIO_ASSERT(IioGlobalData, IIO_ERROR, IioIndex);
    return;
  }
  IioGlobalData->IioVar.IioOutData.CurrentPXPMap[(IioIndex*NUMBER_PORTS_PER_SOCKET)] = 0x04;  // DMI port is active or else we wouldnt be booting
  Array = &(IioGlobalData->IioVar.IioOutData.CurrentPXPMap[0]);  // setup array to update status of PCIe ports

  for (i = 0; i < sizeof(PcieIouStatus)/sizeof(PCIE_IOUPORT_STATUS); i++) {
    if (IioGlobalData->SetupData.ConfigIOU0[IioIndex] == i){
      for (j = 0; j < sizeof(PCIE_IOUPORT_STATUS) ; j++) {
        Array[(IioIndex*NUMBER_PORTS_PER_SOCKET)+j+PORT_1A_INDEX] = PcieIouStatus[i].PortValue[j];
      }
    }
    if (IioGlobalData->SetupData.ConfigIOU1[IioIndex] == i) {
      for (j = 0; j < sizeof(PCIE_IOUPORT_STATUS) ; j++) {
        Array[(IioIndex*NUMBER_PORTS_PER_SOCKET)+j+PORT_2A_INDEX] = PcieIouStatus[i].PortValue[j];
      }
    }
    if (IioGlobalData->SetupData.ConfigIOU2[IioIndex] == i) {
      for (j = 0; j < sizeof(PCIE_IOUPORT_STATUS) ; j++) {
         Array[(IioIndex*NUMBER_PORTS_PER_SOCKET)+j+PORT_3A_INDEX] = PcieIouStatus[i].PortValue[j];
      }
    }
    if (IioGlobalData->SetupData.ConfigMCP0[IioIndex] == i){
      for (j = 0; j < sizeof(PCIE_IOUPORT_STATUS) ; j++) {
        Array[(IioIndex*NUMBER_PORTS_PER_SOCKET)+j+PORT_4A_INDEX] = PcieIouStatus[i].PortValue[j];
      }
    }
    if (IioGlobalData->SetupData.ConfigMCP1[IioIndex] == i){
      for (j = 0; j < sizeof(PCIE_IOUPORT_STATUS) ; j++) {
        Array[(IioIndex*NUMBER_PORTS_PER_SOCKET)+j+PORT_5A_INDEX] = PcieIouStatus[i].PortValue[j];
      }
    }
  }//End i
}

/**

    Wait until Link is trained based on the PortIndex

    @param IioGlobalData        - Pointer to IioGlobalDAta
    @param IioIndex             - Index to Iio
    @param PortIndex            - Index to Iio Port

    @retval None

**/
VOID 
WaitTrainingFinished (
  IN  IIO_GLOBALS                         *IioGlobalData,
  IN  UINT8                               IioIndex,
  IN  UINT8                               PortIndex
  )
{
  BOOLEAN        PcieLinkTrained; 
  UINTN          Loop;
  UINT16         Var16;
   
  //
  // We need to wait 50ms according to SA BS to let training completed
  // We used not to care about this, but now we do the port init immediately
  // after Bifurcation, hence have  to wait enough time and also poll on
  // LNKSTS.DLLA bit as per the IIO PCIe HAS
  //
  PcieLinkTrained = FALSE;
  Loop            = 0;    
  do {
    Var16 = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIE_REG);
    if ( Var16 & B_PCIE_PORT_PXPLNKSTS_DLLA ) {   //wait on DLLA bit would never set for unpopulated slots
      //
      // if completed training
      //
      PcieLinkTrained = TRUE;
    } else {
      //
      // Wait for 10ms
      IioStall (IioGlobalData, GEN3_LINK_POLL_TIMEOUT);
      Loop++;
    }    
  } while (!PcieLinkTrained && Loop < 10);  // sending bifurcation command in parallel, here delay can be shorter to 100ms from 500ms

}

/**

    This routine is to wait link to be trained based on the
    bifurcation 

    @param IioGlobalData        - Pointer to IioGlobalData
    @param IioIndex             - Index to Iio
    @param iou_bif              - Iio Bifurcation 

    @retval None

**/
VOID
WaitTrainingFinishedEx (
  IN  IIO_GLOBALS                     *IioGlobalData,
  IN  UINT8                           IioIndex,
  IN  UINT8                           iou_bif
  )
{
  if(IioIndex == 0){   //PCIe port 0 always x4
    WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_0_FUNC);
  }


  if(IioIndex == 1 || IioIndex == 2 || IioIndex == 3){   // IOU2, IOU0 or IOU1
    switch(iou_bif){
    case 0:   //x4x4x4x4
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1A_FUNC);
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1B_FUNC);
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1C_FUNC);
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1D_FUNC);
      break;
    case 1:   //x4x4x8
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1A_FUNC);
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1B_FUNC);
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1D_FUNC);
      break;
    case 2:   //x8x4x4
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1A_FUNC);
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1D_FUNC);
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1C_FUNC);
      break;
    case 3:   //x8x8
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1A_FUNC);
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_1C_FUNC);
      break;
    case 4:   //x16
      WaitTrainingFinished(IioGlobalData, IioIndex, PCIE_PORT_2A_FUNC);
      break;
    }
  }
}

/**

   The purpose of this routine is to do the necessary work
   required to enable the PCIE POrts. The implementaiton is
   slightly different from Seaburg so we will check the straps
   and make the necessary adjustments before the links begin
   training.  In addition this routine could and should be be
   used to do any othe initialization required before PCI
   Express training.
  
    @param IioGlobalData - Pointer to IioGlobalData
    @param IioIndex      - Index to Iio

    @retval None

**/
VOID
PcieLinkTrainingInit (
  IN  IIO_GLOBALS                         *IioGlobalData,
  IN  UINT8                               IioIndex
  )
{
  PCIE_IOU_BIF_CTRL_IIO_PCIE_STRUCT BifCtrlData;
  UINT8                             Iou0 = 0, Iou1 = 0, Iou2 = 0;
  UINT8                             *Array=NULL;
  UINT8                             Mcp0 = 0, Mcp1 =0;

  IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_INFO, "PcieLinkTrainingInit at device scanning...\n");
    
  //get IOU bifurcation info
  Array = &(IioGlobalData->SetupData.ConfigIOU0[0]);
  Iou0 = Array[IioIndex];
  IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_INFO, "IIO=%d, IOU0=%d.\n", IioIndex, Iou0);
  Array = &(IioGlobalData->SetupData.ConfigIOU1[0]);
  Iou1 = Array[IioIndex];
  IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_INFO, "IIO=%d, IOU1=%d.\n", IioIndex, Iou1);
  Array = &(IioGlobalData->SetupData.ConfigIOU2[0]);
  Iou2 = Array[IioIndex];
  IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_INFO, "IIO=%d, IOU2=%d.\n", IioIndex, Iou2);
  Array = &(IioGlobalData->SetupData.ConfigMCP0[0]);
  Mcp0 = Array[IioIndex];
  IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_INFO, "IIO=%d, MCP0=%d.\n", IioIndex, Mcp0);
  Array = &(IioGlobalData->SetupData.ConfigMCP1[0]);
  Mcp1 = Array[IioIndex];
  IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_INFO, "IIO=%d, MCP1=%d.\n", IioIndex, Mcp1);

  LimitPcieLinkWidth(IioGlobalData, IioIndex);

  if (IioGlobalData->SetupData.DfxLtssmLogger) {
      // unlock DFX
      PcieUnlockDfx(IioGlobalData, IioIndex, 0);    //0= unlock

      //setup loggers
      LoggerStartStop(IioGlobalData, IioIndex, PORT_0_INDEX,IioGlobalData->SetupData.DfxLtssmLoggerSpeed,IioGlobalData->SetupData.DfxLtssmLoggerStop,IioGlobalData->SetupData.DfxLtssmLoggerMask);
      LoggerStartStop(IioGlobalData, IioIndex, PORT_1A_INDEX,IioGlobalData->SetupData.DfxLtssmLoggerSpeed,IioGlobalData->SetupData.DfxLtssmLoggerStop,IioGlobalData->SetupData.DfxLtssmLoggerMask);
      LoggerStartStop(IioGlobalData, IioIndex, PORT_2A_INDEX, IioGlobalData->SetupData.DfxLtssmLoggerSpeed,IioGlobalData->SetupData.DfxLtssmLoggerStop,IioGlobalData->SetupData.DfxLtssmLoggerMask);
      LoggerStartStop(IioGlobalData, IioIndex, PORT_3A_INDEX,IioGlobalData->SetupData.DfxLtssmLoggerSpeed,IioGlobalData->SetupData.DfxLtssmLoggerStop,IioGlobalData->SetupData.DfxLtssmLoggerMask);
  }

  //PCIE0 always x4
  if (IioIndex != 0) {
     BifCtrlData.Data = 0;
     BifCtrlData.Bits.iou_start_bifurcation = 1; // or in bifurcation control and set start bifurcation control bit
     IioWriteCpuCsr16(IioGlobalData,IioIndex, PORT_0_INDEX, PCIE_IOU_BIF_CTRL_IIO_PCIEDMI_REG, BifCtrlData.Data);
  }

  BifCtrlData.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PORT_1A_INDEX, PCIE_IOU_BIF_CTRL_IIO_PCIE_REG);
  BifCtrlData.Bits.iou_bifurcation_control = Iou0; // in bifurcation control
  BifCtrlData.Bits.iou_start_bifurcation = 1; // set start bifurcation control bit
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PORT_1A_INDEX, PCIE_IOU_BIF_CTRL_IIO_PCIE_REG, BifCtrlData.Data);

  BifCtrlData.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PORT_2A_INDEX, PCIE_IOU_BIF_CTRL_IIO_PCIE_REG);
  BifCtrlData.Bits.iou_bifurcation_control = Iou1; // in bifurcation control
  BifCtrlData.Bits.iou_start_bifurcation = 1; // set start bifurcation control bit
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PORT_2A_INDEX, PCIE_IOU_BIF_CTRL_IIO_PCIE_REG, BifCtrlData.Data);

  BifCtrlData.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PORT_3A_INDEX, PCIE_IOU_BIF_CTRL_IIO_PCIE_REG);
  BifCtrlData.Bits.iou_bifurcation_control = Iou2; // in bifurcation control
  BifCtrlData.Bits.iou_start_bifurcation = 1; // set start bifurcation control bit
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PORT_3A_INDEX, PCIE_IOU_BIF_CTRL_IIO_PCIE_REG, BifCtrlData.Data);

  BifCtrlData.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PORT_4A_INDEX, PCIE_IOU_BIF_CTRL_IIO_PCIE_REG);
  BifCtrlData.Bits.iou_bifurcation_control = Mcp0; // in bifurcation control
  BifCtrlData.Bits.iou_start_bifurcation = 1; // set start bifurcation control bit
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PORT_4A_INDEX, PCIE_IOU_BIF_CTRL_IIO_PCIE_REG, BifCtrlData.Data);

  BifCtrlData.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PORT_5A_INDEX, PCIE_IOU_BIF_CTRL_IIO_PCIE_REG);
  BifCtrlData.Bits.iou_bifurcation_control = Mcp1; // or in bifurcation control
  BifCtrlData.Bits.iou_start_bifurcation = 1; // set start bifurcation control bit
  IioWriteCpuCsr16(IioGlobalData, IioIndex, PORT_5A_INDEX, PCIE_IOU_BIF_CTRL_IIO_PCIE_REG, BifCtrlData.Data);

  if (IioGlobalData->SetupData.DfxLtssmLogger) {
        LogSnapShot(IioGlobalData, IioIndex, PORT_0_INDEX);
        LogSnapShot(IioGlobalData, IioIndex, PORT_1A_INDEX);
        LogSnapShot(IioGlobalData, IioIndex, PORT_2A_INDEX);
        LogSnapShot(IioGlobalData, IioIndex, PORT_3A_INDEX);

        // lock DFX
        PcieUnlockDfx(IioGlobalData, IioIndex, 1);    //1= lock
  }

}

/**

    This function limits PCIE Port link width independent of
    Bifurcation configuration

    @param IioGlobalData              - Pointer to IioGlobalData
    @param IioIndex                   - Iio Index 

    @retval None

**/
VOID
LimitPcieLinkWidth (
  IN  IIO_GLOBALS                           *IioGlobalData,
  IN  UINT8                                 IioIndex
  )
{
  UINT8        PortIndex;
  UINT8        DefaultMaxLinkWidth;
  UINT8        OverrideMaxLinkWidthOptVal;
  UINT8        OverrideMaxLinkWidth[NUMBER_PORTS_PER_SOCKET] = {0};
  UINT16       LaneMaskVal;
  UINT8        *ArrayMaxLinkWidth;
  UINT8        *ArrayReverseLink;

  //
  // Collect data about overridden max link widths except Port 13 and 14 (MCP Ports)
  //
  for ( PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
    if( IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex) == FALSE){
      //check whether the PCIe port can be configured
      continue;
    }

    // Store default max link width
    DefaultMaxLinkWidth = IioGlobalData->IioVar.IioOutData.CurrentPXPMap[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex];

    // Check setup option and choose desired width
    OverrideMaxLinkWidthOptVal = IioGlobalData->SetupData.PciePortLinkMaxWidth[((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex];
    if(OverrideMaxLinkWidthOptVal == 0) {
      OverrideMaxLinkWidth[PortIndex] = DefaultMaxLinkWidth;
      continue;
    }

    //
    // Find option setting
    //
    switch(OverrideMaxLinkWidthOptVal) {
    case 1:
      // x1
      OverrideMaxLinkWidth[PortIndex] = PORT_LINK_WIDTH_x1;
      break;
    case 2:
      // x2
      OverrideMaxLinkWidth[PortIndex] = PORT_LINK_WIDTH_x2;
      break;
    case 3:
      // x4
      OverrideMaxLinkWidth[PortIndex] = PORT_LINK_WIDTH_x4;
      break;
    case 4:
      // x8
      OverrideMaxLinkWidth[PortIndex] = PORT_LINK_WIDTH_x8;
      break;
    case 5:
      // x16
      OverrideMaxLinkWidth[PortIndex] = PORT_LINK_WIDTH_x16;
      break;
    default:
      // Shouldn't happen, set to default bifurcated link width
      OverrideMaxLinkWidth[PortIndex] = DefaultMaxLinkWidth;
      break;
     }

    // Check to make sure Override Max doesn't exceed the max possible by bifurcation
    if(OverrideMaxLinkWidth[PortIndex] > DefaultMaxLinkWidth) {
      OverrideMaxLinkWidth[PortIndex] = DefaultMaxLinkWidth;
    }

    if(DefaultMaxLinkWidth != OverrideMaxLinkWidth[PortIndex]) {
      // Document change of link width, if there is any change
      IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_INFO, "Link Width (IIO%d, port%s(%d)) forced from default x%d to override x%d\n", IioIndex, IIO_PORT_LABEL(PortIndex), PortIndex, DefaultMaxLinkWidth, OverrideMaxLinkWidth[PortIndex]);
    }
  } //End PortIndex For

  //
  // Fetch setup data
  //
  ArrayMaxLinkWidth = &(IioGlobalData->IioVar.IioOutData.MaxPXPMap[0]);
  ArrayReverseLink = &(IioGlobalData->IioVar.IioOutData.LaneReversedPXPMap[0]);

  //
  // Calculate port masks for slots where max link width has been overridden
  //
  for ( PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
 
    if( IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex) == FALSE){  //check whether the PCIe port can be configured
      continue;
    }

    DefaultMaxLinkWidth = IioGlobalData->IioVar.IioOutData.CurrentPXPMap[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex];
    OverrideMaxLinkWidthOptVal = IioGlobalData->SetupData.PciePortLinkMaxWidth[((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex];

    if((OverrideMaxLinkWidthOptVal == 0) || (OverrideMaxLinkWidth[PortIndex] == DefaultMaxLinkWidth)){
      continue;
    }

    ArrayMaxLinkWidth[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] = OverrideMaxLinkWidth[PortIndex];

    if (PortIndex == 0) {
        LaneMaskVal = IioReadCpuCsr16(IioGlobalData, IioIndex,IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort, PCIE_LANE_MASK_IIO_DFX_DMI_REG);
    } else{
        LaneMaskVal = IioReadCpuCsr16(IioGlobalData, IioIndex,IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort, PCIE_LANE_MASK_IIO_DFX_REG);
    }

    switch(DefaultMaxLinkWidth) {
      case PORT_LINK_WIDTH_x16:
        // if port is bifurcated to x16, force all 16 lanes to the mask specified by the override link width
        switch(OverrideMaxLinkWidth[PortIndex]) {
          case PORT_LINK_WIDTH_x8:
            // Force x16 to x8
            if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
              LaneMaskVal = 0x00FF;
            } else {
              LaneMaskVal = 0xFF00;
            }
            break;
          case PORT_LINK_WIDTH_x4:
            // Force x16 to x4
            if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
              LaneMaskVal = 0x0FFF;
            } else {
              LaneMaskVal = 0xFFF0;
            }
            break;
          case PORT_LINK_WIDTH_x2:
            // Force x16 to x2
          if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
            LaneMaskVal = 0x3FFF;
          } else {
            LaneMaskVal = 0xFFFC;
          }
          break;
        case PORT_LINK_WIDTH_x1:
          // Force x16 to x1
          if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
            LaneMaskVal = 0x7FFF;
          } else {
            LaneMaskVal = 0xFFFE;
          }
          break;
        default:
          LaneMaskVal = 0x0000;
          break;
        }//End switch(OverrideMaxLinkWidth[PortIndex]) x16
        break;
      case PORT_LINK_WIDTH_x8:
        // if port is bifurcated to x8, set lane mask according to bifurcation and which port it is.
        switch(OverrideMaxLinkWidth[PortIndex]) {
          case PORT_LINK_WIDTH_x4:
            // Force x8 to x4
            switch(PortIndex) {
             case PORT_1A_INDEX:
             case PORT_2A_INDEX:
             case PORT_3A_INDEX:
               if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
                 LaneMaskVal |= 0x000F;
               } else {
                 LaneMaskVal |= 0x00F0;
               }
               break;
             case PORT_1C_INDEX:
             case PORT_2C_INDEX:
             case PORT_3C_INDEX:
               if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
                 LaneMaskVal |= 0x0F00;
               } else {
                 LaneMaskVal |= 0xF000;
               }
               break;
             default:
                  LaneMaskVal |= 0x0000;
                   break;
           }//End switch(PortIndex)
           break;
      case PORT_LINK_WIDTH_x2:
        // Force x8 to x2
        switch(PortIndex) {
        case PORT_1A_INDEX:
        case PORT_2A_INDEX:
        case PORT_3A_INDEX:
          if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
            LaneMaskVal |= 0x003F;
          } else {
            LaneMaskVal |= 0x00FC;
          }
          break;
        case PORT_1C_INDEX:
        case PORT_2C_INDEX:
        case PORT_3C_INDEX:
          if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
            LaneMaskVal |= 0x3F00;
          } else {
            LaneMaskVal |= 0xFC00;
          }
          break;
        default:
          LaneMaskVal |= 0x0000;
          break;
        }// end switch(PortIndex)
        break;
      case PORT_LINK_WIDTH_x1:
        // Force x8 to x1
        switch(PortIndex) {
        case PORT_1A_INDEX:
        case PORT_2A_INDEX:
        case PORT_3A_INDEX:
          if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
            LaneMaskVal |= 0x007F;
          } else {
            LaneMaskVal |= 0x00FE;
          }
          break;
        case PORT_1C_INDEX:
        case PORT_2C_INDEX:
        case PORT_3C_INDEX:
          if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
            LaneMaskVal |= 0x7F00;
          } else {
            LaneMaskVal |= 0xFE00;
          }
          break;
        default:
          LaneMaskVal |= 0x0000;
          break;
        }// End switch(PortIndex)
        break;
      } // switch(OverrideMaxLinkWidth[PortIndex]) for x8
      break;
    case PORT_LINK_WIDTH_x4:
      switch(OverrideMaxLinkWidth[PortIndex]) {
        case PORT_LINK_WIDTH_x2:
          // Force x4 to x2
          switch(PortIndex) {
           case PORT_0_INDEX:
           case PORT_1A_INDEX:
           case PORT_2A_INDEX:
           case PORT_3A_INDEX:
             if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
               LaneMaskVal |= 0x0003;
             } else {
               LaneMaskVal |= 0x000C;
             }
             break;
           case PORT_1B_INDEX:
           case PORT_2B_INDEX:
           case PORT_3B_INDEX:
             if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
              LaneMaskVal |= 0x0030;
             } else {
               LaneMaskVal |= 0x00C0;
             }
             break;
          case PORT_1C_INDEX:
          case PORT_2C_INDEX:
          case PORT_3C_INDEX:
            if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
              LaneMaskVal |= 0x0300;
            } else {
              LaneMaskVal |= 0x0C00;
            }
            break;
          case PORT_1D_INDEX:
          case PORT_2D_INDEX:
          case PORT_3D_INDEX:
            if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
              LaneMaskVal |= 0x3000;
            } else {
              LaneMaskVal |= 0xC000;
            }
            break;
          default:
            LaneMaskVal |= 0x0000;
            break;
        } //End Switch(PortIndex)
        break;
      case PORT_LINK_WIDTH_x1:
        // Force x4 to x1
        switch(PortIndex) {
          case PORT_0_INDEX:
          case PORT_1A_INDEX:
          case PORT_2A_INDEX:
          case PORT_3A_INDEX:
            if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
              LaneMaskVal |= 0x0007;
            } else {
              LaneMaskVal |= 0x000E;
            }
            break;
          case PORT_1B_INDEX:
          case PORT_2B_INDEX:
          case PORT_3B_INDEX:
            if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
              LaneMaskVal |= 0x0070;
            } else {
              LaneMaskVal |= 0x00E0;
            }
            break;
          case PORT_1C_INDEX:
          case PORT_2C_INDEX:
          case PORT_3C_INDEX:
            if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
              LaneMaskVal |= 0x0700;
            } else {
              LaneMaskVal |= 0x0E00;
            }
            break;
          case PORT_1D_INDEX:
          case PORT_2D_INDEX:
          case PORT_3D_INDEX:
            if(ArrayReverseLink[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex]) {
              LaneMaskVal |= 0x7000;
            } else {
              LaneMaskVal |= 0xE000;
            }
            break;
          default:
            LaneMaskVal |= 0x0000;
            break;
          }//End Switch(PortIndex)
        break;
      }// End  switch(OverrideMaxLinkWidth[PortIndex]) x4
      break;
    } // End switch(DefaultMaxLinkWidth)

    //
    // Write lane-disable values calculated by this function
    //
    if (PortIndex == PORT_0_INDEX){
      IioWriteCpuCsr16(IioGlobalData, IioIndex, IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort, PCIE_LANE_MASK_IIO_DFX_DMI_REG, LaneMaskVal);
    } else {
      IioWriteCpuCsr16(IioGlobalData, IioIndex, IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort, PCIE_LANE_MASK_IIO_DFX_REG, LaneMaskVal);
    }
  } // end PortIndex For

}

/**

    This routine programs the Pcie Slot Clock configuration per
    Port

    @param IioGlobalData              - Pointer to IioGlobalData
    @param IioIndex                   - Index to Iio
    @param PortIndex                  - Index to Iio Port   
    @param PcieSlotClockSource        - Slot Index
    @param PciePortClockConfiguration - Clock Configuration

    @retval None

**/
VOID
InitPortClockEarly (
  IN  IIO_GLOBALS                                   *IioGlobalData,
  IN  UINT8                                         IioIndex,
  IN  UINT8                                         PortIndex
  )
{
  CC_RCOMP_IIO_DFX_GLOBAL_STRUCT   CcRcomp;
  UINT8          Stack;

  //
  // Program PCIe port clocking
  //
  if (!(IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == TRUE)){
    if(PortIndex == IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort ){
      Stack = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
      CcRcomp.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, Stack, CC_RCOMP_IIO_DFX_GLOBAL_REG);
      if (IioGlobalData->SetupData.PciePllSsc == 0xFF){
        CcRcomp.Bits.rx_rt_contcal_en = 0;
      } else {
        CcRcomp.Bits.rx_rt_contcal_en = 1;
      }
      IioWriteCpuCsr32(IioGlobalData, IioIndex, Stack, CC_RCOMP_IIO_DFX_GLOBAL_REG, CcRcomp.Data);
    }
  }
}

/**

    This routine programs the Clock Configuration

    @param IioGlobalData - Pointer to IioGlobalData
    @param IioIndex      - Index to Iio

    @retval None

**/
VOID
ProgramIioClockConfiguration (
  IN  IIO_GLOBALS                           *IioGlobalData,
  IN  UINT8                                 IioIndex
  )
{
  UINT8          PortIndex; 
  UINT8          PortSerialNumber;

  for ( PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
  
    if(!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex)){  //check whether the PCIe port can be configured
      continue;
    }
    //
    // make the root port to operate on common clock mode, don't care about
    // receiver side clock configuration during pre-link configuration
    PortSerialNumber = ((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex;
    InitPortClockEarly(IioGlobalData,
                  IioIndex,
                  PortIndex);
  }

}


/**

    VMD - Volume Management Device, setup

    @param IioGlobalData - Pointer to IioGlobalData
    @param IioIndex      - Iio Index

    @retval None

**/
VOID
IioVMDEarlySetup (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex
  )
{
  UINT8                     PortIndex;
  UINT8                     StackIndex;
  CFGBARSZ_IIO_VMD_STRUCT   VMDCfgBarSz;
  CFGBARSZ_IIO_VMD_STRUCT   VMDCfgBarSzOrg;
  MEMBAR1SZ_IIO_VMD_STRUCT  VMDMemBar1Sz;
  MEMBAR1SZ_IIO_VMD_STRUCT  VMDMemBar1SzOrg;
  MEMBAR2SZ_IIO_VMD_STRUCT  VMDMemBar2Sz;
  MEMBAR2SZ_IIO_VMD_STRUCT  VMDMemBar2SzOrg;
  VMASSIGN_IIO_VMD_STRUCT   VMAssign;
  VMASSIGN_IIO_VMD_STRUCT   VMAssignOrg;
  OTCNUMMAXCRDT_N0_IIO_VTD_STRUCT  OtcNumMaxCrdtN0;
  OTCNUMMINCRDT_N0_IIO_VTD_STRUCT  OtcNumMinCrdtN0;

  for ( StackIndex = 0; StackIndex < VMD_STACK_PER_SOCKET; StackIndex++) {    //only check p-stacks which is stack index 1-3
     if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << (StackIndex+IIO_PSTACK0)))){
       continue;
     }

     // Skip initialization if VMD is not enabled is this stack.
     if(IioGlobalData->SetupData.VMDEnabled[(IioIndex*VMD_STACK_PER_SOCKET)+StackIndex]== 0){
       continue;
     }

     IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "IIOVMDSetup on socket:%d stack:%d...\n", IioIndex, StackIndex);

     // program VMAssign
     VMAssign.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, VMASSIGN_IIO_VMD_REG);
     // Skip VMD configuration if Config space is not valid.
     if(VMAssign.Data == 0xFFFFFFFF){
       IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "VMD Stack %02d not present\n", VMAssign.Data);
       continue;
     }

     VMAssignOrg.Data = VMAssign.Data; // Backup Org value to verify later if reset is required.

     // addressing VMD rootport
     for ( PortIndex = 0; PortIndex < VMD_PORT_PER_STACK; PortIndex++) {      //only check root ports on this p-stacks
       if(IioGlobalData->SetupData.VMDPortEnable[(IioIndex*VMD_PORTS_PER_SOCKET) + (StackIndex*VMD_PORT_PER_STACK )+ PortIndex]){
          VMAssign.Bits.port_assign |=  (1<<PortIndex);
       }
     }

     IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, VMASSIGN_IIO_VMD_REG, VMAssign.Data);
     // If any port used as VMD on this stack, program OTCNUMMAXCRDT_N0.np_pool=9, non-posted pool
     if( VMAssign.Bits.port_assign ) {
       //  
       //4929373: Cloned From SKX Si Bug Eco: VMD: Outbound posted performance degradation observed in certain OTC credit configurations
       //
       OtcNumMaxCrdtN0.Data  = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, OTCNUMMAXCRDT_N0_IIO_VTD_REG);
       OtcNumMaxCrdtN0.Bits.np_pool = 0x9;
       IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, OTCNUMMAXCRDT_N0_IIO_VTD_REG, OtcNumMaxCrdtN0.Data);
       //
       // 4928276: VMD: Outbound posted performance degradation observed in certain OTC credit configurations
       //
       OtcNumMinCrdtN0.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, OTCNUMMINCRDT_N0_IIO_VTD_REG);
       OtcNumMinCrdtN0.Bits.np_pool = 0xA;
       IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, OTCNUMMINCRDT_N0_IIO_VTD_REG, OtcNumMinCrdtN0.Data);
     }

     // program CfgBarSz
     VMDCfgBarSz.Data  = IioReadCpuCsr8(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, CFGBARSZ_IIO_VMD_REG);
     VMDCfgBarSzOrg.Data = VMDCfgBarSz.Data; // Backup Org value to verify later if reset is required.
     VMDCfgBarSz.Bits.size = IioGlobalData->SetupData.VMDCfgBarSz[IioIndex*VMD_STACK_PER_SOCKET+StackIndex];
     IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, CFGBARSZ_IIO_VMD_REG, VMDCfgBarSz.Data);

     // program MemBarSz 1/2
     VMDMemBar1Sz.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, MEMBAR1SZ_IIO_VMD_REG);
     VMDMemBar1SzOrg.Data = VMDMemBar1Sz.Data; // Backup Org value to verify later if reset is required.
     VMDMemBar1Sz.Bits.size = IioGlobalData->SetupData.VMDMemBarSz1[IioIndex*VMD_STACK_PER_SOCKET+StackIndex];
     IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, MEMBAR1SZ_IIO_VMD_REG, VMDMemBar1Sz.Data);

     VMDMemBar2Sz.Data = IioReadCpuCsr8(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, MEMBAR2SZ_IIO_VMD_REG);
     VMDMemBar2SzOrg.Data = VMDMemBar2Sz.Data; // Backup Org value to verify later if reset is required.
     VMDMemBar2Sz.Bits.size = IioGlobalData->SetupData.VMDMemBarSz2[IioIndex*VMD_STACK_PER_SOCKET+StackIndex];
     IioWriteCpuCsr32(IioGlobalData, IioIndex, StackIndex+IIO_PSTACK0, MEMBAR2SZ_IIO_VMD_REG, VMDMemBar2Sz.Data);

     IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "VMD VMAssign:%02x CfgBarSz:%02d MemBar1Sz:%02d MemBar2Size:%02d\n", VMAssign.Data, VMDCfgBarSz.Data, VMDMemBar1Sz.Data, VMDMemBar2Sz.Data);

     if ((VMAssignOrg.Data != VMAssign.Data) || (VMDCfgBarSzOrg.Data != VMDCfgBarSz.Data) ||
         (VMDMemBar1SzOrg.Data != VMDMemBar1Sz.Data) || (VMDMemBar2SzOrg.Data != VMDMemBar2Sz.Data)){
         IioGlobalData->IioVar.IioOutData.resetRequired = 1;
         IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "VMD Configuration change detected. Issue WARM RESET!!\n");
     }
   } //for ( StackIndex = 0...

}

/**

    This routine process all Port Config Space setting 
    before link trainig is done 

    @param IioGlobalData - Pointer to IioGlobalData
    @param IioIndex      - Iio Index

    @retval None

**/
VOID
IioEarlyPortInitialization (
  IN IIO_GLOBALS                         *IioGlobalData,
  IN UINT8                               IioIndex
  )
{
  UINT8                             PortIndex;

  for (PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
    if (!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex))
       continue;
    if (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == TRUE){
      DmiLinkInit ( IioGlobalData, IioIndex, PortIndex);
    } else {
      PciePortEarlyInit (IioGlobalData, IioIndex, PortIndex);
    }
  }
}

/**

    This routine process initializate and apply all Si WA
    before link trainig is done 

    @param IioGlobalData - Pointer to IioGlobalData
    @param IioIndex      - Iio Index

    @retval None

**/
VOID
IioEarlyPreLinkTrainingPhase (
  IN  IIO_GLOBALS               *IioGlobalData,
  IN  UINT8                     Iio
  )
{
  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR,"IioEarlyPreLinkTrainingPhase End \n");
  IioBreakAtCheckPoint (IioGlobalData, STS_EARLY_PRELINK_TRAINING, 1, Iio);
  ProgramIioClockConfiguration (IioGlobalData, Iio);
  IioWorkAround (IioGlobalData, Iio, IioBeforeBifurcation);
  DfxPcieInit (IioGlobalData, Iio);
  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_ERROR,"IioEarlyPreLinkTrainingPhase End \n");
}

/**

    This routine will initiaze all the process for link training 

    @param IioGlobalData - Pointer to IioGlobalData
    @param IioIndex      - Iio Index

    @retval None

**/
VOID
IioEarlyPcieLinkTrainingPhase (
  IN IIO_GLOBALS                         *IioGlobalData,
  IN UINT8                               IioIndex
  )
{
  IioBreakAtCheckPoint(IioGlobalData, STS_GEN3_OVERRIDE, 3, IioIndex);
  CheckGen3PreLinkOverride(IioGlobalData, IioIndex);
  IioEarlyPortInitialization(IioGlobalData, IioIndex);
  IioBreakAtCheckPoint(IioGlobalData, STS_LINK_TRAINING, 2, IioIndex);
  //
  // For Emulation minibios: Please skip the IIOINIT bifurcation commands
  //
  PcieLinkTrainingInit(IioGlobalData, IioIndex);
}

/**

    This routine apply all Si configuration 
    after link trainig is done 

    @param IioGlobalData - Pointer to IioGlobalData
    @param IioIndex      - Iio Index

    @retval None

**/
VOID
IioEarlyPostLinkTrainingPhase (
  IN  IIO_GLOBALS                         *IioGlobalData,
  IN  UINT8                               IioIndex
  )
{
  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR,"IioEarlyPreLinkTrainingPhase Start \n");
  //early post link training phase
  //Note: make sure before this routine is call there is a big delay for PCie link
  // training for that same IIO is covered before this routine is invoked.
  IioWorkAround (IioGlobalData, IioIndex, IioAfterBifurcation);
  //
  //IIO Isoc Enabling here instead of IioDmiPeim driver for minibios
  //
  IioIsocConfiguration (IioGlobalData, IioIndex);
}

/**

    This function is used as interface to initialize IIO Si features
    for DXE/SMM drivers

    @param IioGlobalData - IioGlobalData Pointer

    @retval None

**/
IIO_STATUS
IioEarlyInitializeEntry (
  IN IIO_GLOBALS  *IioGlobalData
  )
{
  IIO_STATUS    Status = EFI_SUCCESS;
  UINT8       IioIndex;
  IioBreakAtCheckPoint(IioGlobalData, STS_IIO_EARLY_INIT_ENTRY, 0, 0xFF );

  for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++) {

    if(!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]) {
       continue;
    }
    //
    // Execute IIO Configuration only if not reset is required.
    // with this we avoid to configure IIO in the first boot after flash BIOS
    //

    IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR,"IIO Early Link Training for Skx %x\n Starting...\n", IioIndex);
    IioEarlyPreLinkTrainingPhase (IioGlobalData, IioIndex);
    IioEarlyPcieLinkTrainingPhase (IioGlobalData,IioIndex);
    IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR, "IIO Early Link Training for Skx %x Completed!\n", IioIndex);
  }

#ifdef IA32
  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR, "IIO Early Link Tc/Vc Configuration Start\n");
  //
  // Configure Dmi Tc/Vc
  //
  // 5331816: [LBG Val Critical] After warm reset, ME11 access to UMA does not work immediately after DID
  IioDmiTcVcSetup (IioGlobalData);
  IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR, "IIO Early Link Tc/Vc Configuration End\n");
  //
  // 5371608: BIOS should train DMI before DID/MRC
  //
  DmiLinkReTrain(IioGlobalData, 0, 0);
#else
  if (IioGlobalData->IioVar.IioVData.RasOperation) {
    //
    // For IIO Online add any link training delay which would be common for all (out of spec)
    //
    IioStall (IioGlobalData, LINK_TRAINING_DELAY);   //400ms delay
    DumpIioPciePortPcieLinkStatus (IioGlobalData);    //dump link status
  }
#endif

  IioBreakAtCheckPoint (IioGlobalData, STS_IIO_EARLY_INIT_EXIT, 0, 0xFF );

  return Status;
}
