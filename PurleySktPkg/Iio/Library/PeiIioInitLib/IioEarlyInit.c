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

  IioEarlyInit.c

Abstract:

  Houses all code related to Initialize IIO before Port Initialization

Revision History:

--*/


//
// Do not include any of this functionality in sim builds
//

#include <SysFunc.h>
#include <KtiHost.h>
#include <IioUtilityLib.h>
#include <IioInitLib.h>
#include <OemIioInit.h>
#include <PchReservedResources.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsPmc.h>
#include "IioEarlyInit.h"
#include <RcRegs.h>
#ifndef MINIBIOS_BUILD
#include "IioInitPei.h"
#include <Fpga.h>
#endif //MINIBIOS_BUILD
/**

  Initialize IIO Global Data

  @param *host     - Pointer to syshost struture
  @param Iio       - Index to Socket

  @retval None
--*/
VOID
InitializeIioInputVData (
   IN OUT struct sysHost       *host
  )
{
  IIO_GLOBALS IioGlobalData;
  UINT8       IioIndex;
  UINT8       PortNtbIndex;
  UINT8       PortIndex;

  IioCopyMem( &(IioGlobalData.IioVar), &(host->var.iio), sizeof(IIO_VAR));
  IioCopyMem( &(IioGlobalData.SetupData), &(host->setup.iio) , sizeof(IIO_CONFIG));

  //
  // Let IioInit Driver that link training has been done already in PEI
  //

  IioGlobalData.IioVar.IioVData.PciExpressBase = host->var.common.mmCfgBase;

  // Use the Board type default value
  IioGlobalData.IioVar.IioVData.PlatformType = 0;

#ifndef MINIBIOS_BUILD
  //
  // Get the Board ID
  //
  OemGetIioPlatformInfo(&IioGlobalData);
#endif //MINIBIOS_BUILD
  //
  // Update Other variables required for IIO Init
  //
  IioGlobalData.IioVar.IioVData.CpuType     = host->var.common.cpuType;
  IioGlobalData.IioVar.IioVData.CpuStepping = host->var.common.stepping;
  IioGlobalData.IioVar.IioVData.IsocEnable  = host->var.kti.OutIsocEn;
  IioGlobalData.IioVar.IioVData.meRequestedSize     = host->var.common.meRequestedSize;

  IioGlobalData.IioVar.IioVData.IOxAPICCallbackBootEvent   = FALSE;
  IioGlobalData.IioVar.IioVData.EVMode                     = host->var.common.EVMode;
  IioGlobalData.IioVar.IioVData.Pci64BitResourceAllocation = host->var.common.Pci64BitResourceAllocation;
  IioGlobalData.IioVar.IioVData.Emulation                  = host->var.common.emulation;
  IioGlobalData.IioVar.IioVData.IODC                       = host->setup.kti.IoDcMode;

  for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++){
    IioDebugPrintInfo(&IioGlobalData, IIO_DEBUG_ERROR, "Socket[%x] is socketValid = %x\n", IioIndex,  host->var.common.socketPresentBitMap & (1 << IioIndex));
    IioGlobalData.IioVar.IioVData.SocketPresent[IioIndex]         = (host->var.common.socketPresentBitMap & (1 << IioIndex)) ? 1: 0;
    IioDebugPrintInfo(&IioGlobalData, IIO_DEBUG_ERROR, "[%x]SocketPresent %x\n", IioIndex, IioGlobalData.IioVar.IioVData.SocketPresent[IioIndex]);
#ifndef MINIBIOS_BUILD
    IioGlobalData.IioVar.IioVData.FpgaActive[IioIndex]  = host->var.kti.OutKtiFpgaEnable[IioIndex];
#endif //MINIBIOS_BUILD

    if(!IioGlobalData.IioVar.IioVData.SocketPresent[IioIndex]){
      continue;
    }

    IioGlobalData.IioVar.IioVData.SegMmcfgBase[IioIndex].hi  = host->var.common.mmCfgBaseH[IioIndex];
    IioGlobalData.IioVar.IioVData.SegMmcfgBase[IioIndex].lo  = host->var.common.mmCfgBaseL[IioIndex];

    InitializeIioGlobalDataPerSocket(host, &IioGlobalData, IioIndex);
    IioPreLinkDataInitPerSocket(&IioGlobalData, IioIndex);

  }

  //
  // Copy Default Values
  //
  OemSetIioDefaultValues(&IioGlobalData);

#ifdef MINIBIOS_BUILD
  //
  // Get Iio Default Bifurcation
  //
  OemDefaultIioPortBifurationInfo(&IioGlobalData);
#else
  //
  // Update the IioGlobalData->SetupData with the NVRAM values
  //
  OemSetPlatformDataValues(&IioGlobalData);
  //
  // Get Iio PCIe Root Port Bifurcation Setup based on board layout
  //
  OemInitIioPortBifurcationInfo(&IioGlobalData);
#endif // MINIBIOS_BUILD

  for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++) {

    if (!IioGlobalData.IioVar.IioVData.SocketPresent[IioIndex]) {
      continue;
    }

    for (PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++) {
      PortNtbIndex = GetNtbPortPerPortIndex (PortIndex);
      if (PortNtbIndex == NO_NTB_PORT_INDEX) {
        continue;
      }
      if (IioGlobalData.SetupData.NtbPpd[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + PortNtbIndex] != NTB_PORT_DEF_TRANSPARENT) {
        IioGlobalData.IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] = IioGlobalData.SetupData.NtbPpd[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + PortNtbIndex];
      }
    }
  }

  IioCopyMem(&(host->var.iio), &(IioGlobalData.IioVar), sizeof(IIO_VAR));
  IioCopyMem(&(host->setup.iio), &(IioGlobalData.SetupData), sizeof(IIO_CONFIG));
}

/**

  Initialize IIO Global Data

  @param *host     - Pointer to syshost struture
  @param Iio       - Index to Socket

  @retval None
--*/
VOID
InitializeIioGlobalDataPerSocket (
  IN struct sysHost       *host,
  IN OUT IIO_GLOBALS          *IioGlobalData,
  IN UINT8                IioIndex
  )
{
  UINT8        PortIndex;
  UINT8        StackIndex;
  UINT8        DmiBus;

  IioGlobalData->IioVar.IioVData.SocketBaseBusNumber[IioIndex] = host->var.kti.CpuInfo[IioIndex].CpuRes.BusBase;
  IioGlobalData->IioVar.IioVData.SocketLimitBusNumber[IioIndex] = host->var.kti.CpuInfo[IioIndex].CpuRes.BusLimit;
  IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] = host->var.common.stackPresentBitmap[IioIndex];
  IioGlobalData->IioVar.IioVData.SegMmcfgBase[IioIndex] = host->var.kti.CpuInfo[IioIndex].CpuRes.SegMmcfgBase;
  IioGlobalData->IioVar.IioVData.SegmentSocket[IioIndex] = host->var.common.segmentSocket[IioIndex];

  for (PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET ; PortIndex++) {
    StackIndex = ((PortIndex+3)/4);
    IioGlobalData->IioVar.IioVData.SocketPortBusNumber[IioIndex][PortIndex] = host->var.kti.CpuInfo[IioIndex].CpuRes.StackRes[StackIndex].BusBase;
    IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex] = StackIndex;
    IioGlobalData->IioVar.IioOutData.PciePortOwnership[(IioIndex * NUMBER_NTB_PORTS_PER_SOCKET) + PortIndex] = REGULAR_PCIE_OWNERSHIP;
  }

  IioGlobalData->IioVar.IioVData.SocketUncoreBusNumber[IioIndex] = host->var.common.SocketFirstBus[IioIndex];

  IioGlobalData->IioVar.IioVData.PciResourceMem32Base[IioIndex] = host->var.kti.CpuInfo[IioIndex].CpuRes.MmiolBase;
  IioGlobalData->IioVar.IioVData.PciResourceMem32Limit[IioIndex] =  host->var.kti.CpuInfo[IioIndex].CpuRes.MmiolLimit;

  for(StackIndex = 0; StackIndex < MAX_IIO_STACK; StackIndex++) {
      if(!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << StackIndex))){
        continue;
      }
    IioGlobalData->IioVar.IioVData.SocketStackBus[IioIndex][StackIndex] = host->var.kti.CpuInfo[IioIndex].CpuRes.StackRes[StackIndex].BusBase;
    IioGlobalData->IioVar.IioVData.SocketStackBaseBusNumber[IioIndex][StackIndex] = host->var.kti.CpuInfo[IioIndex].CpuRes.StackRes[StackIndex].BusBase;
    IioGlobalData->IioVar.IioVData.SocketStackLimitBusNumber[IioIndex][StackIndex] = host->var.kti.CpuInfo[IioIndex].CpuRes.StackRes[StackIndex].BusLimit;
    IioGlobalData->IioVar.IioVData.VtdBarAddress[IioIndex][StackIndex] =  host->var.kti.CpuInfo[IioIndex].CpuRes.StackRes[StackIndex].MmiolLimit & (~0x3FFF);
#ifndef MINIBIOS_BUILD
    if ((host->var.kti.OutKtiFpgaEnable[IioIndex]) && (StackIndex == IIO_PSTACK3 )){
      IioGlobalData->IioVar.IioVData.VtdBarAddress[IioIndex][StackIndex] = (host->var.kti.CpuInfo[IioIndex].CpuRes.StackRes[StackIndex].MmiolLimit - FPGA_PREALLOCATE_MEM_SIZE) & (~0x3FFF);
    }
#endif //MINIBIOS_BUILD    
    IioGlobalData->IioVar.IioVData.IoApicBase[IioIndex][StackIndex] = host->var.kti.CpuInfo[IioIndex].CpuRes.StackRes[StackIndex].IoApicBase;
    IioGlobalData->IioVar.IioVData.StackPciResourceMem32Limit[IioIndex][StackIndex] = host->var.kti.CpuInfo[IioIndex].CpuRes.StackRes[StackIndex].MmiolLimit;
    //
    // Check if Hfi support is present
    //
    if (host->var.kti.CpuInfo[IioIndex].CpuRes.StackRes[StackIndex].Personality == TYPE_MCP) {
      IioGlobalData->IioVar.IioVData.SkuPersonality[IioIndex] = TYPE_MCP;
    }

  }
  
  if (host->var.kti.OutKtiFpgaPresent[IioIndex]){
    IioGlobalData->IioVar.IioVData.SkuPersonality[IioIndex] = TYPE_FPGA;
  }
  //
  // Get uplink Pcie Port information
  //
  IioOtherVariableInit(IioGlobalData, IioIndex);

  if (IioIndex == SOCKET_0_INDEX){
    IioGlobalData->IioVar.IioVData.RcBaseAddress = IioGlobalData->IioVar.IioVData.VtdBarAddress[IioIndex][IIO_CSTACK] + V_VT_D_IIO_BAR_SIZE;


    IioGlobalData->IioVar.IioVData.PchIoApicBase = IioGlobalData->IioVar.IioVData.IoApicBase[IioIndex][IIO_CSTACK];

    DmiBus =  IioGlobalData->IioVar.IioVData.SocketStackBus[0][IIO_CSTACK];
    //
    // Update the global data structure with the system configuration data
    //
    IioGlobalData->IioVar.IioVData.PmBase = IioPciExpressRead16(IioGlobalData, 0, DmiBus,
                                                                                  PCI_DEVICE_NUMBER_PCH_PMC,
                                                                                  PCI_FUNCTION_NUMBER_PCH_PMC,
                                                                                  R_PCH_PMC_ACPI_BASE) & ~(1 << 0);

    IioGlobalData->IioVar.IioVData.PchSegRegBaseAddress = PCH_PCR_BASE_ADDRESS;
  }  // Socket0
}


/**

  Execute the Phase 1 and 2 of IIO Ports initialization

  @param *host     - Pointer to syshost structure

  @retval None
--*/
IIO_STATUS
IioEarlyLinkInit(
  IN OUT struct sysHost             *host
)
{
  UINT8         IioIndex;
  IIO_GLOBALS   IioGlobalData;

  //
  // Saves the pointer to the host structure
  //
  //
  host->var.iio.IioOutData.PtrAddress.Address32bit.Value = (UINT32)host;

  InitializeIioInputVData(host);
  //
  // Initialize Local IioGlobalData with host->var.iio values 
  //
  IioCopyMem( &(IioGlobalData.IioVar), &(host->var.iio), sizeof(IIO_VAR));
  IioCopyMem( &(IioGlobalData.SetupData), &(host->setup.iio), sizeof(IIO_CONFIG));

  for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++) {

    if(!IioGlobalData.IioVar.IioVData.SocketPresent[IioIndex]){
      continue;
    }

    //
    // Update our PCIEXpress configuration in our status information
    // loop through 10 PCIE ports that correspond to the three IOUs
    //
    IioUpdatePcieConfigurationData(&IioGlobalData, IioIndex);
    IioVMDEarlySetup(&IioGlobalData, IioIndex);
  }

  //
  // Execute IIO Configuration only if not reset is required.
  // with this we avoid to configure IIO in the first boot after flash BIOS
  //
  if (host->var.common.resetRequired == 0) {
     IioDebugPrintInfo(&IioGlobalData, IIO_DEBUG_ERROR, "Calling IioEarlyIntiazeEntry Start\n");
     IioEarlyInitializeEntry(&IioGlobalData);
     IioDebugPrintInfo(&IioGlobalData, IIO_DEBUG_ERROR, "Calling IioEarlyIntiazeEntry Stop\n");
  }
  //
  // Check if warm reset is required due to VMD changes and override
  // resetRequired host variable.
  //
  if (IioGlobalData.IioVar.IioOutData.resetRequired ) {
     host->var.common.resetRequired |= POST_RESET_WARM;
  }
  //
  // Update host->var.iio values with IioGlobalData
  //
  IioCopyMem(&(host->var.iio), &(IioGlobalData.IioVar), sizeof(IIO_VAR));
  IioCopyMem(&(host->setup.iio) , &(IioGlobalData.SetupData), sizeof(IIO_CONFIG));

  return IIO_SUCCESS;
}

/**


  If there is a mismatch between the value expected and actual in lane width and lane speed, the issue is logged it in EWL structure

Arguments:
  *host     		- Pointer to syshost structure
  *IioGlobalData	- Pointer to IIO Universal Data Structure (UDS)
Returns:
  None
--*/
VOID
IioPciePortEWL(
  IN  struct sysHost    *host,
  IN  IIO_GLOBALS       *IioGlobalData
  )
{
#if ENHANCED_WARNING_LOG
  LNKCAP_IIO_PCIE_STRUCT  LnkCap;
  LNKSTS_IIO_PCIE_STRUCT  LnkSts;
  EWL_ENTRY_TYPE9         type9;
  EWL_ENTRY_TYPE8         type8;
  UINT8                   IioIndex, PortIndex;
  for (IioIndex = 0 ; IioIndex < MaxIIO ; IioIndex++) {
    if (!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex])
           continue;

    for (PortIndex = 0 ; PortIndex < NUMBER_PORTS_PER_SOCKET ; PortIndex++) {
      //check whether the PCIe port can be configured
      if (IioGlobalData->IioVar.IioOutData.PciePortConfig[(IioIndex * NUMBER_PORTS_PER_SOCKET) + PortIndex] == 0) {
        continue;
      }
      if (IdentifyDmiPort(IioGlobalData, IioIndex, PortIndex) == TRUE) { 	 //Skip DMI port
        continue;
      }
      LnkSts.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKSTS_IIO_PCIE_REG);
      LnkCap.Data = IioReadCpuCsr16(IioGlobalData, IioIndex, PortIndex, LNKCAP_IIO_PCIE_REG);

      if (LnkCap.Bits.maximum_link_width != LnkSts.Bits.negotiated_link_width) {
        //EWL
         type9.Header.Size     = sizeof(EWL_ENTRY_TYPE9);
         type9.Header.Type     = EwlType9;
         type9.Header.Severity = EwlSeverityWarning;

         type9.LinkDescription.Socket = IioIndex;
         type9.LinkDescription.Stack  = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
         type9.LinkDescription.Port   = PortIndex;

         type9.ExpectedLinkSpeed = (UINT8)LnkCap.Bits.maximum_link_width;
         type9.ActualLinkSpeed   = (UINT8)LnkSts.Bits.negotiated_link_width;

       PlatformEwlLogEntry(host, &type9.Header);
     }
      if (LnkCap.Bits.maxlnkspd != LnkSts.Bits.current_link_speed) {
        //EWL
        type8.Header.Size		= sizeof(EWL_ENTRY_TYPE9);
        type8.Header.Type		= EwlType8;
        type8.Header.Severity	= EwlSeverityWarning;
 
        type8.LinkDescription.Socket  = IioIndex;
        type8.LinkDescription.Stack   = IioGlobalData->IioVar.IioVData.StackPerPort[IioIndex][PortIndex];
        type8.LinkDescription.Port    = PortIndex;

        type8.ExpectedLinkWidth	= (UINT8)LnkCap.Bits.maxlnkspd;
        type8.ActualLinkWidth	= (UINT8)LnkSts.Bits.current_link_speed;

        PlatformEwlLogEntry(host, &type8.Header);
      }
    }
  }
#endif
  return;
}

/*++

Routine Description:

  Execute the Phase 3 of IIO Ports initialization

  @param *host     - Pointer to syshost structure

  @retval IIO_SUCCESS
--*/
IIO_STATUS
IioEarlyPostLinkInit(
  IN OUT struct sysHost             *host
  )
{
  IIO_GLOBALS IioGlobalData;

  //
  // Initialize Local IioGlobalData with host->var.iio values
  //
  IioCopyMem( &(IioGlobalData.IioVar), &(host->var.iio), sizeof(IIO_VAR));
  IioCopyMem( &(IioGlobalData.SetupData), &(host->setup.iio), sizeof(IIO_CONFIG));
  IioDebugPrintInfo(&IioGlobalData, IIO_DEBUG_ERROR, "IIO Early Post Link Training Starting...\n");

  DumpIioPciePortPcieLinkStatus(&IioGlobalData);

  IioLateInitializeEntry(&IioGlobalData);

  //
  // Check lane width and lane speed
  //
  IioPciePortEWL(host,&IioGlobalData);

  IioDebugPrintInfo( &IioGlobalData, IIO_DEBUG_ERROR, "IIO Early Post Link Training Completed!\n");

#ifndef MINIBIOS_BUILD
  //
  // Now publish IioHobData in order to allow DXE drivers get this information
  //
  IioPublishHobData(&IioGlobalData);
#endif // MINIBIOS_BUILD
  //
  // Update host->var.iio values with IioGlobalData
  //
  IioCopyMem(&(host->var.iio), &(IioGlobalData.IioVar), sizeof(IIO_VAR));
  IioCopyMem(&(host->setup.iio) , &(IioGlobalData.SetupData), sizeof(IIO_CONFIG));

  return IIO_SUCCESS;
}

