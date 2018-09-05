
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


  @file IioVData.c

  IioVData initialization.

**/
#include <SysFunc.h>
#include <OemIioInit.h>
#include <IioSetupDefinitions.h>
#include <IioUtilityLib.h>
#include <IioInitLib.h>
#include "IioVData.h"
#include <IioPciePortInfo.h>
#include <RcRegs.h>
#include <PchReservedResources.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsDmi.h>
#include <Register/PchRegsPcie.h>

static UINTN BifurcationMap[5][4] = {{ 4, 4, 4, 4},
                                     { 8, 0, 4, 4},
                                     { 4, 4, 8, 0},
                                     { 8, 0, 8, 0},
                                     {16, 0, 0, 0} };

/**

    This function verifies if Iio Port is present and update
    PciePortConfig

    @param IioGlobalData - Pointer to IioGlobalData

    @retval None

**/
VOID
IioPreLinkDataInitPerSocket(
    IIO_GLOBALS  *IioGlobalData,
    UINT8        IioIndex
  )
{
  UINT8   PortIndex;
  UINT32  Vid;
  UINT8   BusBase;
  UINT8   Device = 0;
  UINT8   Function = 0;
  UINT8   StackIndex;

  IioCopyMem(&(IioGlobalData->IioVar.PreLinkData.PcieInfo), &PcieInfoDefaults, sizeof(INTEL_DMI_PCIE_INFO));

    for(PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
      StackIndex = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
      if (IioGlobalData->IioVar.IioVData.SocketStackPersonality[IioIndex][StackIndex] == TYPE_DISABLED) {
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "PciePortConfig[Cpu%d Stack%d] disabled\n", IioIndex, StackIndex);

        IioGlobalData->IioVar.IioOutData.PciePortConfig[(IioIndex*NUMBER_PORTS_PER_SOCKET) + PortIndex] = 0x00;
        continue;
      }

      BusBase = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex];
      Device = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device;
      Function = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function;

      StackIndex = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
      // Skip enabling port if Stack is not valid
      if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << StackIndex)))
        continue;

      IioGlobalData->IioVar.IioOutData.PciePortConfig[(IioIndex*NUMBER_PORTS_PER_SOCKET) + PortIndex] = 1;

      //
      // If the Port is not present (e.g. Vendor ID is not Intel, then the port doesn't exist and should not be configured)
      //  This condition could exist on some SKUs where certain IOUs are not present or if some external entity has disabled a port
      //
      if (PortIndex == 0) {
          Vid = VID_IIO_PCIEDMI_REG;
      } else {
          Vid = VID_IIO_PCIE_REG;
      }
      if (IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, Vid) != 0x8086){
        IioGlobalData->IioVar.IioOutData.PciePortConfig[(IioIndex*NUMBER_PORTS_PER_SOCKET) + PortIndex] = 0x00;
      } else {
        IioGlobalData->IioVar.IioOutData.PciePortPresent[(IioIndex*NUMBER_PORTS_PER_SOCKET) + PortIndex] = 0x01;
      }
    }
}

/**

    This funtions enables the link of the selected port.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to CPU/IIO
    @param PortIndex     - Index to IIO Root Port
    @param PortEn        - Flag to determine if port is enable/disable

    @retval None

**/
VOID
EnableDisablePCIePort (
  IN  IIO_GLOBALS            *IioGlobalData,
  IN  UINT8                  IioIndex,
  IN  UINT8                  PortIndex,
  IN  BOOLEAN                PortEn
  )
{
  LNKCON_IIO_PCIE_STRUCT   LnkCon;

  if (PortIndex == PORT_0_INDEX){
    LnkCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG);
  } else {
    LnkCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIE_REG);
  }

  if(PortEn == TRUE){
    LnkCon.Bits.link_disable = 0;   //enable link
  } else {
    LnkCon.Bits.link_disable = 1;   //disable link
  }
    
  if (PortIndex == PORT_0_INDEX){
    IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG, LnkCon.Data);
  } else {
    IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIE_REG, LnkCon.Data);
  }
    
  IioStall (IioGlobalData, IIO_LINK_TRAIN_TIME_MARGIN_G3EQ);
}

/**

    The purpose of this function is to retrain a link in a specific port.

    @param IioGlobalData - Pointer to IIO_GLOBLAS structure
    @param IioIndex      - Socket index
    @param PortIndex     - Port index

    @retval None

**/
VOID
IssuePortRetrainLink (
  IN  IIO_GLOBALS            *IioGlobalData,
  IN  UINT8                  IioIndex,
  IN  UINT8                  PortIndex
  )
{
  LNKCON_IIO_PCIE_STRUCT   LnkCon;

  if (PortIndex == PORT_0_INDEX) {
     LnkCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG);
     LnkCon.Bits.retrain_link = 1;
     IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIEDMI_REG, LnkCon.Data);
  } else {
     LnkCon.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIE_REG);
     LnkCon.Bits.retrain_link = 1;
     IioWriteCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCON_IIO_PCIE_REG, LnkCon.Data);
  }
  IioStall (IioGlobalData, IIO_LINK_TRAIN_TIME_MARGIN_G3EQ);
}

/**

  Helper routine which physically varies the port status

  @param Param1: protocol based PCI address for the root port device

  @retval boolean indicating TRUE for port active or FALSE for not active

**/
BOOLEAN
IsPciePortDLActive (
  IN  IIO_GLOBALS    *IioGlobalData,
  IN  UINT8          IioIndex,
  IN  UINT8          PortIndex
  )
{
  LNKSTS_IIO_PCIE_STRUCT LnkSts;

  LnkSts.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIE_REG);
  if( LnkSts.Bits.data_link_layer_link_active != 0  ){
    return  TRUE;
  } else {
    return  FALSE;
  }
}

/**

    Check if current port is TPH (TLP Processing hints) capable

    @param IioGlobalData - Pointer to IIO globals
    @param IioIndex      - Current socket.
    @param PortIndex     - Current port

    @retval TRUE  - Port is TPH capable.
    @retval FALSE - Port is not TPH capable.

**/
BOOLEAN
IsPciePortTPHCapable (
  IN  IIO_GLOBALS        *IioGlobalData,
  IN  UINT8              IioIndex,
  IN  UINT8              PortIndex
  )
{
  DEVCAP2_IIO_PCIE_STRUCT  DevCap2;

  DevCap2.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, DEVCAP2_IIO_PCIE_REG);
  if(DevCap2.Bits.tph_completer_supported != 0){      //check the TPH completer capability
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Determines if the PCH PCIe slots have devices connected or not.

  @param IioGlobalData  - Pointer to IIO_GLOBALS
  @param IioIndex       - Index to CPU/IIO

  @retval TRUE  - if one or more PCIe slots are populated
  @retval FALSE - if all the PCIe slots for that IIO are open / empty

**/
BOOLEAN
IsPchPciePortsPopulated (
  IN  IIO_GLOBALS           *IioGlobalData,
  IN  UINT8                 IioIndex
  )
{
  UINT8   BusBase;

  BusBase = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PORT_0_INDEX];

   //TODO: Update function for LBG
  return TRUE;
}

/**

  Determines if the PCIe slots have devices connected or not.

  @param IioGlobalData  - Pointer to IIO_GLOBALS
  @param IioIndex       - index to CPU/IIO

  @retval TRUE  - if one or more PCIe slots are populated
  @retval FALSE - if all the PCIe slots for that IIO are open / empty

**/
BOOLEAN
IsIioPciePortsPopulated (
  IN  IIO_GLOBALS          *IioGlobalData,
  IN  UINT8                 IioIndex
  )
{
  UINT8                   PortIndex;
  BOOLEAN                 IioPortsActive = FALSE;

  for(PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
    if( IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex) == FALSE){  //check whether the PCIe port can be configured
      continue;
    }

    if(IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == TRUE){    //DMI port
      continue;
    }

    if(IsPciePortDLActive(IioGlobalData, IioIndex, PortIndex) == TRUE){    //data link layer active
      IioPortsActive = TRUE;
    }
  }

  if(IioPortsActive == FALSE && PortIndex == NUMBER_PORTS_PER_SOCKET){
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_WARN, "  No PCIe devices connected to IIO%d!\n",IioIndex);

    if(!IioIndex){    //legacy socket
      IioPortsActive = IsPchPciePortsPopulated(IioGlobalData, IioIndex);

      if(IioPortsActive == TRUE){
        IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_WARN, "  But PCH has devices connected..\n");
      }
    }
  }
  return IioPortsActive;
}

/**

  Determines if the PCIe slots have devices connected or not.

  @param IioGlobalData  - Pointer to IIO_GLOBALS
  @param IioIndex            - Index to IIO

  @retval TRUE  - if one or more PCIe slots are populated
  @retval FALSE - if all the PCIe slots for that IIO are open / empty

**/
BOOLEAN
IsIioPortPopulated (
  IN  IIO_GLOBALS         *IioGlobalData,
  IN  UINT8               IioIndex,
  IN  UINT8               PortIndex
  )
{
  BOOLEAN                 IioPortsActive = FALSE;

   if(IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE){ //DMI port
    //check whether the PCIe port can be configured
    if( IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex) == TRUE){
      //Is data link layer active?
      if(IsPciePortDLActive(IioGlobalData, IioIndex, PortIndex) == TRUE){
        IioPortsActive = TRUE;
      }
    }
   } else {
      IioPortsActive = TRUE;
   }

  if(IioPortsActive == FALSE){
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_WARN, " No PCIe devices connected to IIO %x PortIndex %x!\n",IioIndex, PortIndex);
  }

  return IioPortsActive;
}


/**

    This function sets IioGlobalData->IioVar.IioOutData.PciePortConfig to prevents Iio driver to configure this port.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to Iio
    @param PortIndex     - Index to Iio Root Port

    @retval None

**/
VOID
IioVData_DisablePciePortConfigure (
  IN  IIO_GLOBALS                     *IioGlobalData,
  IN  UINT8                           IioIndex,
  IN  UINT8                           PortIndex
  )
{
  IioGlobalData->IioVar.IioOutData.PciePortConfig[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] = 0;
  return;
}

/**

    This functions indicates if the IIO Port can be configured or not.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to Iio
    @param PortIndex     - Index to Iio Root Port

    @retval TRUE         - if Iio Port can be configured
    @retval FALSE        - if Iio Port CAN not be configured.

**/
BOOLEAN
IioVData_ConfigurePciePort (
  IN  IIO_GLOBALS                     *IioGlobalData,
  IN  UINT8                           IioIndex,
  IN  UINT8                           PortIndex
  )
{
  return (IioGlobalData->IioVar.IioOutData.PciePortConfig[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == 0)  \
          ? FALSE  : TRUE;
}

/**

    This function return the current port Link Width.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to Iio
    @param PortIndex     - Index to Iio Root Port

    @retval IioGlobalData->IioVar.IioOutData.CurrentPXPMap[(Iio*NUMBER_PORTS_PER_SOCKET)+PortIndex] - Current bifurcation value.

**/
UINT8
GetAssignedPortIndexWidth (
  IN  IIO_GLOBALS             *IioGlobalData,
  IN  UINT8                  IioIndex,
  IN  UINT8                  PortIndex
  )
{
  return IioGlobalData->IioVar.IioOutData.CurrentPXPMap[(IioIndex*NUMBER_PORTS_PER_SOCKET)+PortIndex];
}

/**

   This routine will update Link Speed support in PCH side

    @param IioGlobalData - Pointer to IIO_GLOBALS structure
    @param LinkSpeed     - Target LinkSpeed value

    @retval FALSE   - Error during speed change
    @retval TRUE    - speed change operation done on PCH DMI side

**/
BOOLEAN
SetPchDmiLinkSpeed (
  IN  IIO_GLOBALS      *IioGlobalData,
  IN  UINT8            LinkSpeed
  )
{
    UINT8   PchDmiLinkSpeed;
    PchDmiLinkSpeed = (IioMmioRead8(IioGlobalData, PCH_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LCTL2)) & (BIT3 | BIT2 | BIT1 | BIT0));
    IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "SetPchDmiLinkSpeed: PCH DMI link speed [target/current]= %x/%x\n", LinkSpeed, PchDmiLinkSpeed);

    if(PchDmiLinkSpeed == LinkSpeed){
      return TRUE;
    } else {
      IioMmioAndThenOr8(IioGlobalData, PCH_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LCTL2), (UINT8)~(BIT3 | BIT2 | BIT1 | BIT0), LinkSpeed);
      return TRUE;    //to indicate speed change operation done on PCH DMI side
    }
}
/**

    This function dumps the IOUx PCIe Bifurcation

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to Iio

    @retval None

**/
VOID
IioVDataPostLinkInit (
  IN IIO_GLOBALS                     *IioGlobalData,
  IN UINT8                           IioIndex
  )
{
  UINT8 PortIndex;
  UINT16 BifurcationData;

  for(PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
    // Check whether the PCIe port can be configured
    if (!IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex)){
      continue;
    }

    if (PortIndex == PORT_0_INDEX || PortIndex == PORT_1A_INDEX || PortIndex == PORT_2A_INDEX || PortIndex == PORT_3A_INDEX) {
      //
      // Get PCIE IOU Bifurcation register
      //
      BifurcationData = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, PCIE_IOU_BIF_CTRL_IIO_PCIE_REG);
      IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "IIO=%d, Port index = %s(%d), BIFCTRL=0x%x.\n", IioIndex, IIO_PORT_LABEL(PortIndex), PortIndex, (BifurcationData & 0x7));

    }
  }
}

/**

    this function returns the bifurcation value of the Port specified by PortIndex and IIoIndex.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to Iio
    @param PortIndex     - Index to Iio Root Port

    @retval Bifurcation  - bifurcation value of the Port

**/

UINT8
CheckBifurcationMapPort(
    IN IIO_GLOBALS    *IioGlobalData,
    IN  UINT8         IioIndex,
    UINT8             PortIndex
    )
{
  UINT8                             ClusterPort;
  UINT8                             Bifurcation;

  Bifurcation = 0;

  ClusterPort = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].SuperClusterPort;
  switch(ClusterPort){
  case PORT_0_INDEX:
    Bifurcation = 0;
    break;
  case PORT_1A_INDEX:
    Bifurcation = IioGlobalData->SetupData.ConfigIOU0[IioIndex];
    break;
  case PORT_2A_INDEX:
    Bifurcation = IioGlobalData->SetupData.ConfigIOU1[IioIndex];
    break;
  case PORT_3A_INDEX:
    Bifurcation =IioGlobalData->SetupData.ConfigIOU2[IioIndex];
    break;
  case PORT_4A_INDEX:
    Bifurcation = IioGlobalData->SetupData.ConfigMCP0[IioIndex];
    break;
  case PORT_5A_INDEX:
    Bifurcation = IioGlobalData->SetupData.ConfigMCP1[IioIndex];
    break;
  default:
    IIO_ASSERT(IioGlobalData, IIO_INVALID_PARAMETER, ClusterPort);
    break;
  }
  return Bifurcation;
}

/**

    Checks if the port is the uplink port.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to Iio
    @param PortIndex     - Index to Iio Root Port

    @retval TRUE  - if uplink port
    @retval FALSE - if not uplink port

**/
BOOLEAN
IsThisUplinkPort (
  IN  IIO_GLOBALS      *IioGlobalData,
  IN  UINT8            IioIndex,
  IN  UINT8            PortIndex
  )
{
  if(IioIndex < MaxIIO){
    if(IioGlobalData->IioVar.PreLinkData.UplinkInfo[IioIndex].Valid &&
       IioGlobalData->IioVar.PreLinkData.UplinkInfo[IioIndex].IioUplinkPortIndex == PortIndex){
      return TRUE;
    }else{
      return FALSE;
    }
  } else {
    return FALSE;
  }
}

/**

    This function checks if IIO port is enabled or not.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to Iio
    @param PortIndex     - Index to Iio Root Port

    @retval PCI_PORT_ENABLE         - to indicate port is to be disabled
    @retval PCI_PORT_DISABLE        - to indicate port is to be enabled
    @retval PCI_PORT_AUTO           - to indicate port is to be auto


**/
UINT8
CheckVarPortEnable (
  IN  IIO_GLOBALS           *IioGlobalData,
  IN  UINT8                 IioIndex,
  IN  UINT8                 PortIndex
  )
{
  UINT8 PciePortDisable;

  PciePortDisable = \
      IioGlobalData->SetupData.PciePortDisable[((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex];

  if(PciePortDisable == IIO_OPTION_ENABLE)
    return PCIE_PORT_ENABLE;

  if(PciePortDisable == IIO_OPTION_DISABLE)
    return PCIE_PORT_DISABLE;

  return PCIE_PORT_AUTO;
}

/**

    This function checks if PCIE port is enabled or not.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to Iio
    @param PortIndex     - Index to Iio Root Port

    @retval TRUE         - to indicate port is to be disabled
    @retval FALSE        - to indicate port is to be enabled


**/
BOOLEAN
CheckPciePortEnable (
  IN  IIO_GLOBALS           *IioGlobalData,
  IN  UINT8                 IioIndex,
  IN  UINT8                 PortIndex
  )
{
  UINT8   PciePortEnable;
  UINT8   VarPortEnable;

  PciePortEnable = TRUE;

  if((CheckNtbPortConfSetting(IioGlobalData, IioIndex, PortIndex) == FALSE) &&
      (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == FALSE)){

     VarPortEnable = CheckVarPortEnable(IioGlobalData, IioIndex, PortIndex);

     switch(VarPortEnable){
       case PCIE_PORT_ENABLE:
         break;
       case PCIE_PORT_AUTO:
         if(IsIioPortPopulated(IioGlobalData, IioIndex, PortIndex) == FALSE){
           if((IioGlobalData->SetupData.PcieHotPlugEnable == FALSE) ||
              ((IioGlobalData->SetupData.PcieHotPlugEnable == TRUE) &&
              (IioGlobalData->SetupData.SLOTHPCAP[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == FALSE)))
                 PciePortEnable = FALSE;
              }
         break;
       case PCIE_PORT_DISABLE:
       default:
         PciePortEnable = FALSE;
         break;
     }
   }
  return PciePortEnable;
}
/**

    Determines the Link Speed value based on he setup option value.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to Iio
    @param PortIndex     - Index to Iio Root Port

    @retval PCIE_LINK_SPEED_GEN1     - Gen1 speed
    @retval PCIE_LINK_SPEED_GEN2     - expected HW default for PCIe port 0
    @retval PCIE_LINK_SPEED_GEN3     - expected HW default for all other ports
    @retval PcieLinkSpeed            - Current link Speed

**/
UINT8
CheckVarPortLinkSpeed (
  IN  IIO_GLOBALS        *IioGlobalData,
  IN  UINT8              IioIndex,
  IN  UINT8              PortIndex
  )
{
  UINT8   PcieLinkSpeed;

  PcieLinkSpeed = \
      IioGlobalData->SetupData.PciePortLinkSpeed[((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex];

  if(PcieLinkSpeed == PCIE_LINK_SPEED_AUTO){
      return PCIE_LINK_SPEED_GEN3;              //expected HW default for all other ports
  }  else {
    return PcieLinkSpeed;
  }
}

/**

    Checks if Link Speed needs to be changed in case of the uplink port.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to Iio
    @param PortIndex     - Index to Iio Root Port

    @retval TRUE         - speed change required in case of AUTO
    @retval FALSE        - no speed change required in case of other ports

**/
BOOLEAN
ChangeVarPortLinkSpeed (
  IN  IIO_GLOBALS        *IioGlobalData,
  IN  UINT8              IioIndex,
  IN  UINT8              PortIndex
  )
{
  //
  // HSD 3616041: change S0 port3D (uplink port) default to Gen1.
  //
  if (IsThisUplinkPort(IioGlobalData, IioIndex, PortIndex) == TRUE){
    return TRUE;            //speed change required in case of AUTO
  } else {
    return FALSE;           // no speed change required in case of other ports
  }
}

/**

    Indicates the ASPM value based on PCieAspm setup option value.

    @param IioGlobalData - Pointer to IIO_GLOBALS
    @param IioIndex      - Index to Iio
    @param PortIndex     - Index to Iio Root Port

    @retval Aspm - Aspm value that needs to be setup

**/
UINT8
CheckVarPortASPM (
  IN  IIO_GLOBALS      *IioGlobalData,
  IN  UINT8            IioIndex,
  IN  UINT8            PortIndex
  )
{
  UINT8       Aspm;

  Aspm = IioGlobalData->SetupData.PcieAspm[((IioIndex) * NUMBER_PORTS_PER_SOCKET) + PortIndex];

  switch(Aspm){
    case PCIE_ASPM_AUTO:
    case PCIE_ASPM_L1_ONLY:
      Aspm = PCIE_ASPM_L1_ONLY;     // ASPM = L1
      break;
    default:
      Aspm = PCIE_ASPM_DISABLE;     // ASPM = disable
      break;
  }

  return Aspm;
}

/**

  Helper routine which returns the PCI header class code value for the given BDF

  @param IioGlobalData - IIO Global data structure
  @param IioIndex      - Socket Index
  @param PortIndex     - Port Index

  @retval Class code value from PCI header

**/
UINT32
GetPortClassCode (
  IN IIO_GLOBALS   *IioGlobalData,
  IN UINT8          IioIndex,
  IN UINT8          PortIndex
  )
{
  UINT32 ClassCode;
  ClassCode = (UINT32)IioReadCpuCsr8(IioGlobalData, IioIndex, PortIndex, CCR_N0_IIO_PCIE_REG);
  ClassCode |= (UINT32)(IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, CCR_N1_IIO_PCIE_REG) << 8);
  return ClassCode;
}

/**

  Hook to check whether the BDF given is valid port of the IIO

  @param IioGlobalData - IIO Global data structure
  @param Bus, Device, Function - PCIE port BDF

  @retval TRUE if valid, else FALSE

**/
BOOLEAN
IdentifyIioPort (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8          IioIndex,
  IN  UINT8          PortIndex
  )
{

    if(IioIndex >= MaxIIO){
      return FALSE;
    }


    if(PortIndex >= NUMBER_PORTS_PER_SOCKET){
        return FALSE;
    } else {
        return TRUE;
    }

}


/**

  Hook to check whether the BDF given is DMI port

  @param IioGlobalData - IIO Global data structure
  @param Bus, Device, Function - PCIE port BDF

  @retval TRUE if valid, else FALSE

**/
BOOLEAN
IdentifyDmiPort (
  IN  IIO_GLOBALS   *IioGlobalData,
  IN  UINT8         IioIndex,
  IN  UINT8         PortIndex
  )
{
  if(IdentifyIioPort(IioGlobalData, IioIndex, PortIndex) == FALSE)
     return FALSE;
  //
  // if Class code matches the PCI Host Bridge's base class (06), subclass (0),
  // Reg. Prog interface(0), than it is connected to the PCH on the down link
  // which eventually determines the DMI port
  //
  if (GetPortClassCode(IioGlobalData, IioIndex, PortIndex) == V_PCIE_CCR_PCI_HOST_BRIDGE){
    return TRUE;              //return TRUE to indicate that the PCI address belongs to DMI port
  } else {
    return FALSE;             //return FALSE as it does not belong to DMI port
  }
}

/**

    GetPortPerStack: Returns the maximum number of ports per
    stack

    @param Stack - Stack number

    @retval PortNumber - Number of ports defined by stack.

**/
UINT8
GetMaxPortPerStack (
  IN UINT8              Stack
  )
{
    UINT8 PortNumber;

   switch(Stack) {
     case IIO_CSTACK:
     case IIO_PSTACK4:
         PortNumber = 1;
         break;
     case IIO_PSTACK0:
     case IIO_PSTACK1:
     case IIO_PSTACK2:
     case IIO_PSTACK3:
         PortNumber = 4;
         break;
     default:
         PortNumber = 0;
         break;
   }
   return PortNumber;
}

/**

    Check if Port will be configured as NTB

    @param IioGlobalData - Global IIO Data Structure
    @param Iio           - Iio Index
    @param Bus           - Bus Number

    @retval TRUE/FALSE   - Based on setup option value

**/
UINT8
GetNtbPortPerPortIndex (
  IN UINT8        PortIndex
  )
{
    UINT8 NtbPortIndex;

    NtbPortIndex = NO_NTB_PORT_INDEX;

    switch(PortIndex) {
    case PORT_1A_INDEX:
        NtbPortIndex = 0;
        break;
    case PORT_2A_INDEX:
        NtbPortIndex = 1;
        break;
    case PORT_3A_INDEX:
        NtbPortIndex = 2;
        break;
    default:
        NtbPortIndex = NO_NTB_PORT_INDEX;
        break;
    }

  return NtbPortIndex;
}

/**

    Check if Port will be configured as NTB

    @param IioGlobalData - Global IIO Data Structure
    @param Iio           - Iio Index
    @param Bus           - Bus Number

    @retval TRUE/FALSE   - Based on setup option value

**/
BOOLEAN
CheckNtbPortConfSetting (
  IN  IIO_GLOBALS        *IioGlobalData,
  IN  UINT8              IioIndex,
  IN  UINT8              PortIndex
  )
{
  if ((IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == NTB_PORT_DEF_NTB_NTB ) ||
      (IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == NTB_PORT_DEF_NTB_RP )) {
    return TRUE;
  }

  return FALSE;
}


/**

    Check if at least one Port on the Stack will be configured as VMD

    @param IioGlobalData - Global IIO Data Structure
    @param IioIndex      - Iio Index
    @param StackIndex    - Stack Index

    @retval VMDPortsEnable   - TRUE/FALSE to indicate that at least one port in the stack
                               will be configured as VMD.


**/
BOOLEAN
CheckVMDEnablePerStack (
  IN  IIO_GLOBALS        *IioGlobalData,
  IN  UINT8              IioIndex,
  IN  UINT8              StackIndex
  )
{
  BOOLEAN VMDStackEnable = FALSE;

  if ((StackIndex == IIO_CSTACK) || (StackIndex >= IIO_PSTACK3)){
    return VMDStackEnable;
  }
  //
  // Decrease StackIndex to fit VMDPortEnable index calculation
  //
  StackIndex--;
  if (IioGlobalData->SetupData.VMDEnabled[(IioIndex*VMD_STACK_PER_SOCKET)+StackIndex]){
     VMDStackEnable = TRUE;
     IioDebugPrintInfo (IioGlobalData, IIO_DEBUG_ERROR, "VMD enabled at StackIndex = %x \n",StackIndex);
  }
  return VMDStackEnable;
}

/**

    Check if it is a port connected to FPGA (Only for SKX-FPGA).

    @param IioGlobalData - Pointer to IIO global data/methods
    @param PortIndex     - Port number being initialized.

    @retval TRUE - It is a port connected to FPGA
    @retval FALSE - It is NOT a port connected to FPGA

**/
BOOLEAN
CheckFPGAPort (
  IN IIO_GLOBALS   *IioGlobalData,
  IN UINT8         IioIndex,
  IN UINT8         PortIndex
  )
{
  if(IioGlobalData->IioVar.IioVData.SkuPersonality[IioIndex] == TYPE_FPGA){
    if((PortIndex >= PORT_4A_INDEX) && (PortIndex <= PORT_4D_INDEX)){
      return TRUE;
    }
  }
  return FALSE;
}

/**

    Dump Iio Port Info

    @param IioGlobalData              - Pointer to IioGlobalData

    @retval None

**/
VOID  
DumpIioPciePortPcieLinkStatus (
  IN IIO_GLOBALS *IioGlobalData
  )
{
  PCI_ROOT_BRIDGE_PCI_ADDRESS PciAddress;
  UINT8                       IioIndex, PortIndex;
  LNKSTS_IIO_PCIE_STRUCT      Lksts_Iio_Pcie;

  IioDebugPrintInfo(IioGlobalData, IIO_DEBUG_INFO, "\tDumpIioPcieLinkStatus()..\n");

  for (IioIndex = 0 ; IioIndex < MaxIIO ; IioIndex++) {
    if (!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]){
       continue;
    }

    for (PortIndex = 0 ; PortIndex < NUMBER_PORTS_PER_SOCKET ; PortIndex++) {
      PciAddress.Bus      = IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex];
      PciAddress.Device   = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Device;
      PciAddress.Function = IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PortIndex].Function;
      if( IioVData_ConfigurePciePort(IioGlobalData, IioIndex, PortIndex) == FALSE){  //check whether the PCIe port can be configured
        continue;
      }

      if(IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == TRUE){ 	 //Skip DMI port
        continue;
      }

      Lksts_Iio_Pcie.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIE_REG);
      if(Lksts_Iio_Pcie.Bits.negotiated_link_width == 0) {
        IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_ERROR, " Skt[%d], B[x%02X]:D[%x]:F[%x] ", IioIndex, PciAddress.Bus, PciAddress.Device, PciAddress.Function);
        IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_ERROR, " Link Down! \n");
      } else {
        if((Lksts_Iio_Pcie.Bits.negotiated_link_width == 1) ||
           (Lksts_Iio_Pcie.Bits.negotiated_link_width == 2) ||
           (Lksts_Iio_Pcie.Bits.negotiated_link_width == 4) ||
           (Lksts_Iio_Pcie.Bits.negotiated_link_width == 8) ||
           (Lksts_Iio_Pcie.Bits.negotiated_link_width == 16)) {
          IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_ERROR, " Skt[%d], B[x%02X]:D[%x]:F[%x] ", IioIndex, PciAddress.Bus, PciAddress.Device, PciAddress.Function);
          IioDebugPrintInfo( IioGlobalData, IIO_DEBUG_ERROR, "\t\tLink up as x%02d Gen%d! \n",Lksts_Iio_Pcie.Bits.negotiated_link_width, Lksts_Iio_Pcie.Bits.current_link_speed);
        }
      }
    }
  }
}

/*-------------------------------------------------------------------------**/
