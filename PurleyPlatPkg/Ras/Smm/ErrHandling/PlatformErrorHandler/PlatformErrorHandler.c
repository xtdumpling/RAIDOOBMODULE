//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//  
//  Rev. 1.03
//      Bug Fixed:  Fix SERR/PERR item no function.
//      Reason:     
//      Auditor:    Kasber Chen
//      Date:       Apr/11/2017
//
//  Rev. 1.02
//      Bug Fixed:  Draft template v0.1.
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Jan/17/2017
// 
//  Rev. 1.01
//      Bug Fixed:  Fix cannot log SERR/PERR. 
//      Reason:     
//      Auditor:    Chen Lin
//      Date:       Sep/09/2016
// 
//  Rev. 1.00
//      Bug Fixed:  Support SMC Memory map-out function.
//      Reason:     
//      Auditor:    Ivern Yeh
//      Date:       Jul/07/2016
//
//****************************************************************************
/** @file
  Implementation of the Platform Error Handler.  This module will
  provide hooks for OEMs to customize error handling as well as
  to provide library classes for overriding system capabilities.

  Copyright (c) 2009-2015 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/

#include "PlatformErrorHandler.h"
#include <PchAccess.h>
#include "Token.h"	//SMCPKG_SUPPORT

#include <Protocol/CpuCsrAccess.h>
#include <Setup/IioUniversalData.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/IioUds.h>
#include <Protocol/MemRasProtocol.h>
#include <Protocol/CrystalRidge.h>
#include <Protocol/MpService.h>
#include <Protocol/Smbios.h>
#include <Protocol/PlatformType.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/OemRasLib/OemRasLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PciExpressLib.h>
#include <Library/BaseLib.h>
#include <Library/mpsyncdatalib.h>
#include <Library/SynchronizationLib.h>
#include <Library/emcaplatformhookslib.h>
#include <Library/FpgaConfigurationLib.h>
#include <Guid/SetupVariable.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/IioSystem.h>
#include <Library/SetupLib.h>
#include <Guid/SocketMpLinkVariable.h>
#if SMCPKG_SUPPORT && MemoryMapOut_SUPPORT
#include <Guid/SetupVariable.h>

EFI_GUID gMemMapOutGuid = {0x3a2d8619, 0x050b, 0x4b13, 0x91, 0x91, 0x8, 0x51, 0x59, 0x12, 0xa9, 0x64};
#endif

//
// Default policies
//

HOST_ALERT_POLICY mHostAlertPolicy[MAX_HOST_TRANSACTIONS] = {
//  {LogLevel, AitErrViralEn, AitErrInterruptEn, DpaErrViralEn, DpaErrInterruptEn, UncErrViralEn, UncErrInterruptEn, DataPathErrInterruptEn, DataPathErrViralEn, IllegalAccessInterruptEn, IllegalAccessViralEn, Rsvd1, Rsvd2, Rsvd3, Rsvd4},
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}, // FNV_TT_2LM_READ
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, ENABLE,  DISABLE, ENABLE,  DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}, // FNV_TT_2LM_WRITE
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}, // FNV_TT_PM_READ
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, ENABLE,  DISABLE, ENABLE,  DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}, // FNV_TT_PM_WRITE
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}, // FNV_TT_BW_READ
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, ENABLE,  DISABLE, ENABLE,  DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}, // FNV_TT_BW_WRITE
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}, // FNV_TT_AIT_READ
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}, // FNV_TT_AIT_WRITE
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}, // FNV_TT_WEAR_LEVEL_MOVE
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}, // FNV_TT_PATROL_SCRUB
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}, // FNV_TT_CSR_READ
	{HIGH_PRIORITY, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE, DISABLE}  // FNV_TT_CSR_WRITE
};

// Protocol Definitions

EFI_PLATFORM_ERROR_HANDLING_PROTOCOL PlatformErrorHandlingProtocol = {
  RegisterErrorListener,
  LogReportedError,
  RegisterNotifier,
  NotifyOs,
  NULL
};

EFI_MP_SERVICES_PROTOCOL            *mMpService;
EFI_PLATFORM_RAS_POLICY_PROTOCOL    PlatformRasPolicyProtocol;
LISTENER_LIST_ENTRY                 *ListenerListHead = NULL;
LISTENER_LIST_ENTRY                 *ListenerListTail = NULL;
NOTIFIER_LIST_ENTRY                 *NotifierListHead = NULL;
NOTIFIER_LIST_ENTRY                 *NotifierListTail = NULL;

// Function Definitions

/**
  Populate RAS Capability Structure

  This function allocates memory for the RAS Capability Structure and populates it

  @param[out] **CapabilityPtr   A pointer to the pointer for the RAS Capability Structure

  @retval Status

**/
EFI_STATUS
PopulateRasCapabilityStructure (
      OUT   SYSTEM_RAS_CAPABILITY   **CapabilityPtr
)
{
  EFI_STATUS              Status;
  UINT8                   i;
  SYSTEM_RAS_CAPABILITY   *Capability;

  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData,
                  sizeof (SYSTEM_RAS_CAPABILITY),
                  (VOID **)CapabilityPtr);
  ASSERT_EFI_ERROR (Status);

  if (!EFI_ERROR (Status)) {
    Capability = *CapabilityPtr;

    // Zero out the structure.  
    ZeroMem (Capability, sizeof (SYSTEM_RAS_CAPABILITY));

    // Populate Silicon Capability Structure
    PopulateSiliconCapabilityStructure (&(Capability->SiliconRasCapability));

    // Initialize the Platform RAS Capability to all 1s
    for (i = 0; i < sizeof (PLATFORM_RAS_CAPABILITY); i++) {
      ((BOOLEAN *)Capability)[i] = TRUE;
    }
  }

  return Status;
}

/**
  Populate RAS Topology Structure

  This function allocates memory for the RAS Topology Structure and populates it

  @param[out] **TopologyPtr   A pointer to the pointer for the RAS System Topology Structure

  @retval Status

**/
EFI_STATUS
PopulateRasTopologyStructure (
      OUT   EFI_RAS_SYSTEM_TOPOLOGY   **TopologyPtr
)
{
  EFI_STATUS                  Status;
  IIO_UDS                     *IioUdsData = NULL;
  EFI_IIO_UDS_PROTOCOL        *IioUds = NULL;
  UINT8                       Skt;
  UINT8                       Mc;
  UINT8                       Core;
  UINT16                      Thread;
  UINT8                       MpLink;
  UINT8                       Ch;
  UINT8                       Dimm;
  UINT8                       PciPort;
  UINT16                      ProcessorNumber;
  UINT8                       IioStack;
  UINT8                       Count;

  EFI_MEM_RAS_PROTOCOL        *MemRas = NULL;
  EFI_PROCESSOR_INFORMATION   ProcessorInfoBuffer;
  SYSTEM_INFO                 *SystemInfo;
  EFI_SMBIOS_PROTOCOL         *SmbiosProtocol = NULL;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  EFI_SMBIOS_TYPE             SmbiosType;
  EFI_SMBIOS_TABLE_HEADER     *SmbiosRecord;
  EFI_RAS_SYSTEM_TOPOLOGY     *Topology;
  EFI_IIO_SYSTEM_PROTOCOL     *mIioSystem;
  EFI_PLATFORM_TYPE_PROTOCOL  *PlatformType;
  FPGA_CONFIGURATION          FpgaConfiguration;


  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData,
                  sizeof (EFI_RAS_SYSTEM_TOPOLOGY),
                  (VOID **)TopologyPtr);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Topology = *TopologyPtr;

  Status = gSmst->SmmLocateProtocol (&gEfiMemRasProtocolGuid, NULL, &MemRas);
  ASSERT_EFI_ERROR(Status);

  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &IioUds);
  IioUdsData = (IIO_UDS *)IioUds->IioUdsPtr;

  Status = gBS->LocateProtocol (&gEfiPlatformTypeProtocolGuid, NULL, &PlatformType);
  ASSERT_EFI_ERROR(Status);

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &SmbiosProtocol);
  ASSERT_EFI_ERROR(Status);

  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &mIioSystem);
  ASSERT_EFI_ERROR (Status);

  SystemInfo = &Topology->SystemInfo;

  ZeroMem (SystemInfo, sizeof (SYSTEM_INFO));

  Status = FpgaConfigurationGetValues (&FpgaConfiguration);

  SystemInfo->FpgaSktActive = EFI_ERROR(Status)? 0 : FpgaConfiguration.FpgaSktActive;
  SystemInfo->BoardId = PlatformType->BoardId;
  SystemInfo->NumOfSocket = IioUdsData->SystemStatus.numCpus;
  SystemInfo->SocketBitMap = IioUdsData->SystemStatus.socketPresentBitMap;
  SystemInfo->CpuType = IioUdsData->SystemStatus.cpuType;
  SystemInfo->CpuSubType = IioUdsData->SystemStatus.cpuSubType;
  SystemInfo->CpuStepping = IioUdsData->SystemStatus.MinimumCpuStepping;
  SystemInfo->SystemRasType = IioUdsData->SystemStatus.SystemRasType;
  SystemInfo->MmCfgBase = IioUdsData->SystemStatus.mmCfgBase;
  SystemInfo->RasModesEnabled = MemRas->SystemMemInfo->RasModesEnabled;
  SystemInfo->MemType = MemRas->SystemMemInfo->DimmType;
  SystemInfo->NumMemNode = MemRas->SystemMemInfo->maxIMC;
  SystemInfo->NumChanEnabled = MemRas->SystemMemInfo->maxCh;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY;
  Status = SmbiosProtocol->GetNext (SmbiosProtocol, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
  if (!(EFI_ERROR(Status))) {
    SystemInfo->SmbiosType16Handle = SmbiosRecord->Handle;
  }

  for (Skt = 0; Skt < MAX_SOCKET; Skt++) {
    if ((IioUdsData->SystemStatus.socketPresentBitMap & (1 << Skt)) == 0) 
      continue;
      
    SystemInfo->SocketInfo[Skt].Valid = 1;
    for (MpLink = 0; MpLink < MAX_KTI_PORTS; MpLink++) {
      SystemInfo->SocketInfo[Skt].QpiInfo[MpLink].LinkValid = IioUdsData->PlatformData.CpuQpiInfo[Skt].PeerInfo[MpLink].Valid;
    }
    for (Mc = 0; Mc < MAX_IMC; Mc++) {
      SystemInfo->SocketInfo[Skt].imcEnabled[Mc] = MemRas->SystemMemInfo->Socket[Skt].imcEnabled[Mc];
    }
    for (Ch = 0; Ch < MAX_CH; Ch++) {
      SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].Valid = MemRas->SystemMemInfo->Socket[Skt].ChannelInfo[Ch].Enabled;
      if (SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].Valid == 0)
        continue;
      SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].ChannelFailed = MemRas->SystemMemInfo->Socket[Skt].ChannelInfo[Ch].chFailed;
      SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].NgnChannelFailed = MemRas->SystemMemInfo->Socket[Skt].ChannelInfo[Ch].ngnChFailed;
      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
        if (SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].DimmInfo[Dimm].Valid == 0)
          continue;
        SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].DimmInfo[Dimm].Valid = MemRas->SystemMemInfo->Socket[Skt].ChannelInfo[Ch].DimmInfo[Dimm].Present;
        SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].DimmInfo[Dimm].X4present = MemRas->SystemMemInfo->Socket[Skt].ChannelInfo[Ch].DimmInfo[Dimm].X4Present;
        SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].DimmInfo[Dimm].MemSize = MemRas->SystemMemInfo->Socket[Skt].ChannelInfo[Ch].DimmInfo[Dimm].DimmSize;
        SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].DimmInfo[Dimm].NumOfRanks = MemRas->SystemMemInfo->Socket[Skt].ChannelInfo[Ch].DimmInfo[Dimm].NumRanks;
        SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].MaxDimms = MemRas->SystemMemInfo->Socket[Skt].ChannelInfo[Ch].MaxDimm;
        SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].NumRanks += SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].DimmInfo[Dimm].NumOfRanks;
        SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
        Status = SmbiosProtocol->GetNext (SmbiosProtocol, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
        if (!(EFI_ERROR(Status))) {
          SystemInfo->SocketInfo[Skt].ChannelInfo[Ch].DimmInfo[Dimm].SmbiosType17Handle = SmbiosRecord->Handle;
        }
      }
    }

    SystemInfo->SocketInfo[Skt].UncoreInfo.M3UpiCount = IioUdsData->PlatformData.CpuQpiInfo[Skt].TotM3Kti;
	SystemInfo->SocketInfo[Skt].UncoreInfo.M2PcieCount = 0;
	for (Count = 0; Count < 8; Count++) { // M2PciePresentBitMap = UINT8
	  if ((IioUdsData->PlatformData.CpuQpiInfo[Skt].M2PciePresentBitmap & (1 << Count)) != 0) {
	    SystemInfo->SocketInfo[Skt].UncoreInfo.M2PcieCount++;
	  }
	}

    SystemInfo->SocketInfo[Skt].UncoreIioInfo.Valid = IioUdsData->PlatformData.IIO_resource[Skt].Valid;
    if (SystemInfo->SocketInfo[Skt].UncoreIioInfo.Valid != 0) {
      SystemInfo->SocketInfo[Skt].UncoreIioInfo.SocketId = IioUdsData->PlatformData.IIO_resource[Skt].SocketID;
      SystemInfo->SocketInfo[Skt].UncoreIioInfo.IIOBusBase = IioUdsData->PlatformData.IIO_resource[Skt].BusBase;
      for (PciPort = 0; PciPort < NUMBER_PORTS_PER_SOCKET; PciPort++) {
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.PciPortInfo[PciPort].BusDevFun =
          (IioUdsData->PlatformData.IIO_resource[Skt].BusBase << 16) + (IioUdsData->PlatformData.IIO_resource[Skt].PcieInfo.PortInfo[PciPort].Device << 8) + IioUdsData->PlatformData.IIO_resource[Skt].PcieInfo.PortInfo[PciPort].Function; // Bus [23:16], Dev [15:8], Fun [7:0]
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.PciPortInfo[PciPort].Bus = mIioSystem->IioGlobalData->IioVar.IioVData.SocketPortBusNumber[Skt][PciPort];
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.PciPortInfo[PciPort].Device = mIioSystem->IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PciPort].Device;
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.PciPortInfo[PciPort].Function = mIioSystem->IioGlobalData->IioVar.PreLinkData.PcieInfo.PortInfo[PciPort].Function;
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.PciPortInfo[PciPort].StackPerPort = mIioSystem->IioGlobalData->IioVar.IioVData.StackPerPort[Skt][PciPort];
        if (PciPort == PORT_4A_INDEX){
          SystemInfo->SocketInfo[Skt].UncoreIioInfo.FpgaStack = SystemInfo->SocketInfo[Skt].UncoreIioInfo.PciPortInfo[PciPort].StackPerPort; //Store fpga stack for later use.
        }
      }
    // Copy over the IIO Stack info
      for (IioStack = 0; IioStack < MAX_IIO_STACK; IioStack++) {
        
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.IioStackInfo[IioStack].Valid = (IioUdsData->PlatformData.CpuQpiInfo[Skt].stackPresentBitmap & (1 << IioStack)) != 0; 
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.IioStackInfo[IioStack].BusBase = IioUdsData->PlatformData.IIO_resource[Skt].StackRes[IioStack].BusBase;
        SystemInfo->SocketInfo[Skt].UncoreIioInfo.IioStackInfo[IioStack].BusLimit = IioUdsData->PlatformData.IIO_resource[Skt].StackRes[IioStack].BusLimit;
      }
    }
  }

  for (ProcessorNumber = 0; ProcessorNumber < gSmst->NumberOfCpus; ProcessorNumber++) {
    Status = mMpService->GetProcessorInfo (
                          mMpService,
                          ProcessorNumber,
                          &ProcessorInfoBuffer
                          );
    if (Status == EFI_SUCCESS &&
        (ProcessorInfoBuffer.StatusFlag & PROCESSOR_ENABLED_BIT) != 0) {

      RASDEBUG((EFI_D_ERROR, "The ProcessorNumber is %x\n",ProcessorNumber));
      SystemInfo->ProcessorValid[ProcessorNumber] = 1;

      Skt = ProcessorInfoBuffer.Location.Package;
      Thread = ProcessorInfoBuffer.Location.Thread;

      if (Thread == 0)
        SystemInfo->SocketInfo[Skt].CoreCount++;

      Core = SystemInfo->SocketInfo[Skt].CoreCount - 1;

      if (Skt < MAX_SOCKET && Core < MAX_CORE && Thread < MAXTHREADID) {
        SystemInfo->SocketInfo[Skt].CpuInfo[Core][Thread].Valid = 1;
        SystemInfo->SocketInfo[Skt].CpuInfo[Core][Thread].CpuNum = ProcessorNumber;
        SystemInfo->SocketInfo[Skt].CpuInfo[Core][Thread].ApicId = ((UINT32)ProcessorInfoBuffer.ProcessorId);
        SystemInfo->SocketInfo[Skt].CpuInfo[Core][Thread].ApicVer = 2;
        SystemInfo->SocketInfo[Skt].CpuInfo[Core][Thread].BspFlag = (ProcessorInfoBuffer.StatusFlag & PROCESSOR_AS_BSP_BIT);
      } else {
        RASDEBUG((EFI_D_ERROR, "ERROR: ApicId convertion may be wrong\n"));
        ASSERT(FALSE);
      }
    }
  }

  return Status;
}

/**
  Load Default Setup Variables into RAS Setup Structure

  This function loads default setup variables into RAS Setup Structure

  @param[out] *RasSetup   A pointer for the RAS Setup Structure

  @retval EFI_SUCCESS

**/
EFI_STATUS
LoadDefaultSetupVariables (
      OUT   SYSTEM_RAS_SETUP    *RasSetup
)
{
  UINT8      TransType;
  RasSetup->SystemErrorEn = 0x01;
  RasSetup->PoisonEn = 0x00;
  RasSetup->ViralEn = 0x00;
  RasSetup->CloakingEn = 0x00;
  RasSetup->ClearViralStatus = 0x00;
  RasSetup->UboxToPcuMcaEn = 0x00;
  RasSetup->FatalErrSpinLoopEn = 0x00;
  
  // eMCA
  RasSetup->Emca.EmcaEn = 0x01;
  RasSetup->Emca.EmcaIgnOptin = 0x00;
  RasSetup->Emca.EmcaCsmiEn = 0x02;
  RasSetup->Emca.EmcaMsmiEn = 0x02;
  RasSetup->Emca.ElogCorrErrEn = 0x01;
  RasSetup->Emca.ElogMemErrEn = 0x01;
  RasSetup->Emca.ElogProcErrEn = 0x00;
  RasSetup->Emca.LmceEn = 0x00;

  // Whea
  RasSetup->Whea.WheaSupportEn = 0x01;
  RasSetup->Whea.WheaLogMemoryEn = 0x01;
  RasSetup->Whea.WheaLogProcEn = 0x01;
  RasSetup->Whea.WheaLogPciEn = 0x01;

  // Error injection
  RasSetup->ErrInj.WheaErrorInjSupportEn = 0x00;
  RasSetup->ErrInj.McaBankErrInjEn = 0x01;
  RasSetup->ErrInj.WheaErrInjEn = 0x00;
  RasSetup->ErrInj.WheaPcieErrInjEn = 0x00;
  RasSetup->ErrInj.PcieErrInjActionTable = 0x00;
  RasSetup->ErrInj.MeSegErrorInjEn = 0x00;

  // QPI
  RasSetup->Qpi.QpiCpuSktHotPlugEn = 0x00;

  // Memory
  RasSetup->Memory.MemErrEn = 0x01;
  RasSetup->Memory.CorrMemErrEn = 0x01;
  RasSetup->Memory.LeakyBktHiLeakyBktLo = 0x01;
  RasSetup->Memory.SpareIntSelect = 0x01;
  RasSetup->Memory.FnvErrorEn;
  RasSetup->Memory.FnvErrorLowPrioritySignal;
  RasSetup->Memory.FnvErrorHighPrioritySignal;
  //RasSetup->Memory.DieSparing = 0x01;
  //RasSetup->Memory.DieSparingAggressivenessLevel = 128;
  RasSetup->Memory.NgnAddressRangeScrub = 0x01;
  ZeroMem(&(RasSetup->Memory.HostAlerts[0]), sizeof(UINT16) * NUM_HOST_TRANSACTIONS);
  for (TransType = 0; TransType < NUM_HOST_TRANSACTIONS; TransType++) {
    RasSetup->Memory.HostAlerts[TransType] = *((UINT16 *) mHostAlertPolicy + TransType);
  }
  // IIO
  RasSetup->Iio.IioErrorEn = 0x01;
  RasSetup->Iio.IoMcaEn = 0x01;
  RasSetup->Iio.IioErrRegistersClearEn = 0x01;
  RasSetup->Iio.IioErrorPinEn = 0x00;
  RasSetup->Iio.LerEn = 0x01;
  RasSetup->Iio.DisableLerMAerrorLogging = 0x01;
  RasSetup->Iio.IioIrpErrorEn = 0x01;

  // IRPP0ERRCTL  
  RasSetup->Iio.IioIrpp0ErrCtlBitMap = (BIT1 | BIT2 | BIT3 | BIT4 | BIT10 | BIT13 | BIT14);
 
  // IRPPERRSV  
  RasSetup->Iio.IioCohSevBitMap = (BIT4 | BIT6 | BIT8 | BIT21 | BIT27 | BIT29);  
  RasSetup->Iio.IioMiscErrorEn = 0x01;
  // MIERRCTL
  RasSetup->Iio.IioMiscErrorBitMap = (BIT0 | BIT2 | BIT3 | BIT4);  
  // MIERRSV  
  RasSetup->Iio.IioMiscErrorSevMap = (BIT1 | BIT6);  
  RasSetup->Iio.IioVtdErrorEn = 0x01;
  // VTUNCERRMSK
  RasSetup->Iio.IioVtdBitMap = (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7 | BIT8);
  // VTUNCERRSEV  
  RasSetup->Iio.IioVtdSevBitMap = (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT7 | BIT8);  
  RasSetup->Iio.IioItcErrSevBitMap = 0;
  RasSetup->Iio.IioOtcErrSevBitMap = 0;
  RasSetup->Iio.IioDmaErrorEn = 0x01;
  // CHANERRMSK  
  RasSetup->Iio.IioDmaBitMap = (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7 | BIT8 | BIT9 
                                | BIT10 | BIT11 | BIT12 | BIT13 | BIT14 | BIT15 | BIT16); 
  // CHANERRSEV  
  RasSetup->Iio.IioDmaSevBitMap = (BIT16);    
  RasSetup->Iio.IioPcieAddCorrErrorEn = 0x01;       
  RasSetup->Iio.IioPcieAddUnCorrEn = 0x01;
  // XPUNCEDMASK, XPUNCERRMSK
  RasSetup->Iio.IioPcieAddUnCorrBitMap = (BIT1 | BIT3 | BIT4 | BIT8 | BIT9); 
  RasSetup->Iio.IioPcieAddUnCorrSevBitMap = BIT1; 
  RasSetup->Iio.IioPcieAerSpecCompEn = 0x00;

  // PCI-EX
  RasSetup->Pcie.PcieErrEn = 0x01;
  RasSetup->Pcie.PcieCorrErrEn = 0x01;
  RasSetup->Pcie.PcieUncorrErrEn = 0x01;
  RasSetup->Pcie.PcieFatalErrEn = 0x01;
  RasSetup->Pcie.PcieCorErrCntr = 0x01;
  RasSetup->Pcie.PcieCorErrMaskBitMap = (BIT16 | BIT17 | BIT18 | BIT19 | BIT20 | BIT21);
  RasSetup->Pcie.PcieCorErrThres = 0x01;
  RasSetup->Pcie.PcieAerCorrErrEn = 0x01;
  // COREDMASK, CORERRMSK
  RasSetup->Pcie.PcieAerCorrErrBitMap = (BIT0 | BIT6 | BIT7 | BIT8 | BIT12);  
  RasSetup->Pcie.PcieAerAdNfatErrEn = 0x01;
  // UNCERRMSK, UNCEDMASK, UNCERRSEV
  RasSetup->Pcie.PcieAerAdNfatErrBitMap = (BIT12 | BIT15 | BIT16 | BIT20);
  RasSetup->Pcie.PcieAerNfatErrEn = 0x01;
  // UNCERRMSK, UNCEDMASK, UNCERRSEV
  RasSetup->Pcie.PcieAerNfatErrBitMap = (BIT14 | BIT21);
  RasSetup->Pcie.PcieAerFatErrEn = 0x01;
  // UNCERRMSK, UNCEDMASK, UNCERRSEV
  RasSetup->Pcie.PcieAerFatErrBitMap = (BIT4 | BIT5 | BIT13 | BIT17 | BIT18);  
  RasSetup->Pcie.SerrPropEn = 0x01;
  RasSetup->Pcie.PerrPropEn = 0x01;
  RasSetup->Pcie.OsSigOnSerrEn = 0x01;
  RasSetup->Pcie.OsSigOnPerrEn = 0x01;


  // Platform
  RasSetup->Platform.CaterrGpioSmiEn = 0x0;
  
  return EFI_SUCCESS;
}

/**
  Populate RAS Setup Structure

  This function allocates memory for the RAS Setup Structure and populates it

  @param[out] **RasSetupPtr   A pointer to the pointer for the RAS Setup Structure

  @retval Status

**/
EFI_STATUS
PopulateRasSetupStructure(
      OUT   SYSTEM_RAS_SETUP    **RasSetupPtr
)
{
  EFI_STATUS               Status;
  SYSTEM_CONFIGURATION     *SystemConfiguration;
  SYSTEM_RAS_SETUP         *RasSetup;
// APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
 INTEL_SETUP_DATA               *SetupData;
// APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  UINT8                    TransType;
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, 
                  sizeof (SYSTEM_RAS_SETUP), 
                  (VOID **)RasSetupPtr);
  ASSERT_EFI_ERROR (Status);  
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  RasSetup = *RasSetupPtr;
  
  // Clear the SystemRasSetup structure.
  ZeroMem (RasSetup, sizeof (SYSTEM_RAS_SETUP));
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (INTEL_SETUP_DATA), &SetupData); 
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  ASSERT_EFI_ERROR (Status);
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  ZeroMem (SetupData, sizeof (INTEL_SETUP_DATA)); 
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (SYSTEM_CONFIGURATION), &SystemConfiguration);
  ASSERT_EFI_ERROR (Status);
  ZeroMem (SystemConfiguration, sizeof (SYSTEM_CONFIGURATION));
  
  Status = GetEntireConfig (SetupData);
  if (EFI_ERROR (Status)) {
    RASDEBUG ((DEBUG_INFO, "Setup Variable Not Found. Loading Default Values.\n"));
    Status = LoadDefaultSetupVariables (RasSetup);

    gBS->FreePool (SystemConfiguration);
    gBS->FreePool (SetupData);
    return Status;
  }

  CopyMem (SystemConfiguration, &(SetupData->SystemConfig), sizeof(SYSTEM_CONFIGURATION));
  
  // Populate structure
  RasSetup->SystemErrorEn = SystemConfiguration->SystemErrorEn;

  if (RasSetup->SystemErrorEn != 0x00) {
    RasSetup->ViralEn = SystemConfiguration->ViralEn;
    if (RasSetup->ViralEn != 0x00) {
      RasSetup->PoisonEn = 0x01;
      RasSetup->ClearViralStatus = SystemConfiguration->ClearViralStatus;
    } else {
      RasSetup->PoisonEn = SystemConfiguration->PoisonEn;
    }
    RasSetup->CloakingEn = SystemConfiguration->CloakingEn;
    RasSetup->UboxToPcuMcaEn = SystemConfiguration->UboxToPcuMcaEn;
	  RasSetup->FatalErrSpinLoopEn     = SystemConfiguration->FatalErrSpinLoopEn;

    // eMCA
    RasSetup->Emca.EmcaEn = SystemConfiguration->EmcaEn;
    RasSetup->Emca.EmcaIgnOptin = SystemConfiguration->EmcaIgnOptin;
    RasSetup->Emca.LmceEn = SystemConfiguration->LmceEn;
    if (RasSetup->Emca.EmcaEn != 0x00) {
      RasSetup->Emca.EmcaCsmiEn = SystemConfiguration->EmcaCsmiEn;
      RasSetup->Emca.EmcaMsmiEn = SystemConfiguration->EmcaMsmiEn;
      RasSetup->Emca.ElogCorrErrEn = SystemConfiguration->ElogCorrErrEn;
      RasSetup->Emca.ElogMemErrEn = SystemConfiguration->ElogMemErrEn;
      RasSetup->Emca.ElogProcErrEn = SystemConfiguration->ElogProcErrEn;
    }

    // Whea
    RasSetup->Whea.WheaSupportEn = SystemConfiguration->WheaSupportEn;
    if (RasSetup->Whea.WheaSupportEn != 0x00) {
      RasSetup->Whea.WheaLogMemoryEn = SystemConfiguration->WheaLogMemoryEn;
      RasSetup->Whea.WheaLogProcEn = SystemConfiguration->WheaLogProcEn;
      RasSetup->Whea.WheaLogPciEn = SystemConfiguration->WheaLogPciEn;
    }

    RASDEBUG ((DEBUG_INFO, "WheaSupportEn: %x , WheaLogMemoryEn: %x, WheaLogPciEn: %x\n",RasSetup->Whea.WheaSupportEn,RasSetup->Whea.WheaLogMemoryEn,RasSetup->Whea.WheaLogPciEn));

    // Error injection
    RasSetup->ErrInj.WheaErrorInjSupportEn = SystemConfiguration->WheaErrorInjSupportEn;
    RasSetup->ErrInj.McaBankErrInjEn = SystemConfiguration->McaBankErrInjEn;
    RasSetup->ErrInj.WheaErrInjEn = SystemConfiguration->WheaErrInjEn;
    RasSetup->ErrInj.WheaPcieErrInjEn = SystemConfiguration->WheaPcieErrInjEn;
    RasSetup->ErrInj.MeSegErrorInjEn = SystemConfiguration->MeSegErrorInjEn;
    RasSetup->ErrInj.PcieErrInjActionTable = SystemConfiguration->PcieErrInjActionTable;
    RasSetup->ErrInj.ParityCheckEn = SystemConfiguration->ParityCheckEn;


    // QPI
    RasSetup->Qpi.QpiCpuSktHotPlugEn = SetupData->SocketConfig.CsiConfig.QpiCpuSktHotPlugEn;
    RasSetup->Qpi.DirectoryModeEn = SetupData->SocketConfig.CsiConfig.DirectoryModeEn;


    // Memory
    RasSetup->Memory.MemErrEn = SystemConfiguration->MemErrEn;
    if (RasSetup->Memory.MemErrEn != 0x00) {
      RasSetup->Memory.CorrMemErrEn = SystemConfiguration->CorrMemErrEn;
      RasSetup->Memory.LeakyBktHiLeakyBktLo = SystemConfiguration->LeakyBktHiLeakyBktLo;
      if (RasSetup->Memory.CorrMemErrEn != 0x00) {
        RasSetup->Memory.SpareIntSelect = SystemConfiguration->SpareIntSelect;
      }
      RasSetup->Memory.FnvErrorEn = SystemConfiguration->FnvErrorEn;
      RasSetup->Memory.FnvErrorHighPrioritySignal = SystemConfiguration->FnvErrorHighPrioritySignal;
      RasSetup->Memory.FnvErrorLowPrioritySignal = SystemConfiguration->FnvErrorLowPrioritySignal;
    }
    RASDEBUG ((DEBUG_INFO, "CorrMemErrEn: %x , SpareIntSelect: %x\n",RasSetup->Memory.CorrMemErrEn,RasSetup->Memory.SpareIntSelect));

    //RasSetup->Memory.DieSparing = SystemConfiguration->DieSparing;
    //RasSetup->Memory.DieSparingAggressivenessLevel = SystemConfiguration->DieSparingAggressivenessLevel;
    RasSetup->Memory.NgnAddressRangeScrub = SystemConfiguration->NgnAddressRangeScrub;
    RasSetup->Memory.NgnHostAlertAit      = SystemConfiguration->NgnHostAlertAit;
    RasSetup->Memory.NgnHostAlertDpa	  = SystemConfiguration->NgnHostAlertDpa;
    RasSetup->Memory.pprType            = SetupData->SocketConfig.MemoryConfig.pprType;
    RasSetup->Memory.pprErrInjTest        = SetupData->SocketConfig.MemoryConfig.pprErrInjTest;
    RasSetup->Memory.multiSparingRanks    = SetupData->SocketConfig.MemoryConfig.multiSparingRanks;
    RasSetup->Memory.PatrolScrubAddrMode = SetupData->SocketConfig.MemoryConfig.PatrolScrubAddrMode;
    for (TransType = 0; TransType < NUM_HOST_TRANSACTIONS; TransType++) {
      RasSetup->Memory.HostAlerts[TransType] = *((UINT16 *) mHostAlertPolicy + TransType);
      if (RasSetup->Memory.NgnHostAlertAit != 0x00) {
        RasSetup->Memory.HostAlerts[TransType] |=  BIT2;
      }
      if (RasSetup->Memory.NgnHostAlertDpa != 0x00) {
        RasSetup->Memory.HostAlerts[TransType] |=  BIT4;
      }
    }

    // IIO
    RasSetup->Iio.IioErrorEn = SystemConfiguration->IioErrorEn;
#if SMCPKG_SUPPORT
    if((SystemConfiguration->IioErrorEn) && (!SystemConfiguration->SerrPerrEn)){
        SystemConfiguration->IioErrorEn = 0;
        RasSetup->Iio.IioErrorEn = 0;
    }
#endif
    if (RasSetup->Iio.IioErrorEn != 0x00) {
      RasSetup->Iio.IoMcaEn = SystemConfiguration->IoMcaEn;
      RasSetup->Iio.IioErrRegistersClearEn = SystemConfiguration->IioErrRegistersClearEn;
      if (RasSetup->Iio.IoMcaEn == 0x00) {
        RasSetup->Iio.IioErrorPinEn = SystemConfiguration->IioErrorPinEn;
      } else {
        RasSetup->Iio.IioErrorPinEn = 0;
      }
     
      RasSetup->Iio.LerEn = SystemConfiguration->LerEn;
      if (RasSetup->Iio.LerEn != 0x00) {
        RasSetup->Iio.DisableLerMAerrorLogging = SystemConfiguration->DisableMAerrorLoggingDueToLER;
      }
      RasSetup->Iio.IioDmiErrorEn = SystemConfiguration->IioDmiErrorEn;
      RasSetup->Iio.IioIrpErrorEn = SystemConfiguration->IioIrpErrorEn;
      if (RasSetup->Iio.IioIrpErrorEn != 0x00) {
        // IRPP0ERRCTL
        RasSetup->Iio.IioIrpp0ErrCtlBitMap =
					(UINT32) (SystemConfiguration->irpp0_parityError << 14) | 
					(UINT32) (SystemConfiguration->irpp0_qtOverflow << 13) | 
					(UINT32) (SystemConfiguration->irpp0_unexprsp << 10) | 
					(UINT32) (SystemConfiguration->irpp0_csraccunaligned << 6) | 
					(UINT32) (SystemConfiguration->irpp0_unceccCs1 << 5) | 
					(UINT32) (SystemConfiguration->irpp0_unceccCs0 << 4) | 
					(UINT32) (SystemConfiguration->irpp0_rcvdpoison << 3) | 
					(UINT32) (SystemConfiguration->irpp0_crreccCs1 << 2) | 
					(UINT32) (SystemConfiguration->irpp0_crreccCs0 << 1);

        // IRPPERRSV
        RasSetup->Iio.IioCohSevBitMap = (BIT4 | BIT6 | BIT8 | BIT21 | BIT27 | BIT29);
      }
      RasSetup->Iio.IioMiscErrorEn = SystemConfiguration->IioMiscErrorEn;
      if (RasSetup->Iio.IioMiscErrorEn != 0x00) {
        // MIERRCTL
        RasSetup->Iio.IioMiscErrorBitMap = (BIT0 | BIT2 | BIT3 | BIT4);
        // MIERRSV
        RasSetup->Iio.IioMiscErrorSevMap = (BIT1 | BIT6);
      }

      RasSetup->Iio.IioVtdErrorEn = SystemConfiguration->IioVtdErrorEn;
      if (RasSetup->Iio.IioVtdErrorEn  != 0x00) {
        // VTUNCERRMSK, VTUNCERRSEV
        RasSetup->Iio.IioVtdBitMap = (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7 | BIT8);
        RasSetup->Iio.IioVtdSevBitMap = (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT7 | BIT8);
      }
      RasSetup->Iio.IioItcErrSevBitMap = 0;
      RasSetup->Iio.IioOtcErrSevBitMap = 0;
      RasSetup->Iio.IioDmaErrorEn = SystemConfiguration->IioDmaErrorEn;
      if (RasSetup->Iio.IioDmaErrorEn  != 0x00) {
        // CHANERRMSK, CHANERRSEV
        RasSetup->Iio.IioDmaBitMap = (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7 | BIT8 | BIT9
                                        | BIT10 | BIT11 | BIT12 | BIT13 | BIT14 | BIT15 | BIT16);
        RasSetup->Iio.IioDmaSevBitMap = (BIT16);                                         
      }
      RasSetup->Pcie.PcieErrEn = SystemConfiguration->PcieErrEn;
      RasSetup->Iio.IioPcieAddCorrErrorEn = SystemConfiguration->IioPcieAddCorrErrorEn;
      RasSetup->Iio.IioPcieAddUnCorrEn = SystemConfiguration->IioPcieAddUnCorrEn;
      if (RasSetup->Iio.IioPcieAddUnCorrEn != 0x00) {
        // XPUNCEDMASK, XPUNCERRMSK, XPUNCERRSEV
        RasSetup->Iio.IioPcieAddUnCorrBitMap = (BIT1 | BIT3 | BIT8 | BIT9); //bit 5 and 6 values shouldn't be 1, System hangs on "GeneratePciDevInfo"
        RasSetup->Iio.IioPcieAddUnCorrSevBitMap = BIT1; 
      }
      RasSetup->Iio.IioPcieAerSpecCompEn = SystemConfiguration->IioPcieAerSpecCompEn;

      // PCIe ECRC enable by port
      CopyMem (&(RasSetup->Iio.IioPciePortEcrcEn), &(SetupData->SocketConfig.IioConfig.ECRC), sizeof(UINT8) * MAX_TOTAL_PORTS);
    }
    
    // PCI-EX
    if (RasSetup->Pcie.PcieErrEn != 0x00) {
      RasSetup->Pcie.PcieCorrErrEn = SystemConfiguration->PcieCorrErrEn;
      RasSetup->Pcie.PcieUncorrErrEn = SystemConfiguration->PcieUncorrErrEn;
      RasSetup->Pcie.PcieFatalErrEn = SystemConfiguration->PcieFatalErrEn;
      RasSetup->Pcie.PcieCorErrCntr = SystemConfiguration->PcieCorErrCntr;
      //BIT16 receiver error Mask, BIT17 BAD TLP Mask, BIT18 BAD DLLP Mask, BIT19 Replay Num Rollover Mask, BIT20 Replay Timer Timeout Mask, BIT21 Advisory Non Fatal Mask
      RasSetup->Pcie.PcieCorErrMaskBitMap = (UINT32)((SystemConfiguration->PcieCorErrMaskBitMap&0x3F)<<16);
      RasSetup->Pcie.PcieCorErrThres = SystemConfiguration->PcieCorErrThres;
      RasSetup->Pcie.PcieAerCorrErrEn = SystemConfiguration->PcieAerCorrErrEn;
      if (RasSetup->Pcie.PcieAerCorrErrEn != 0x00) {
        // COREDMASK, CORERRMSK
        RasSetup->Pcie.PcieAerCorrErrBitMap = (BIT0 | BIT6 | BIT7 | BIT8 | BIT12);
      }
      RasSetup->Pcie.PcieAerAdNfatErrEn = SystemConfiguration->PcieAerAdNfatErrEn;
      if (RasSetup->Pcie.PcieAerAdNfatErrEn != 0x00) {
        // UNCERRMSK, UNCEDMASK, UNCERRSEV
        RasSetup->Pcie.PcieAerAdNfatErrBitMap = (BIT12 | BIT15 | BIT16 | BIT20);
      }
      RasSetup->Pcie.PcieAerNfatErrEn = SystemConfiguration->PcieAerNfatErrEn;
      if (RasSetup->Pcie.PcieAerNfatErrEn != 0x00) {
        // UNCERRMSK, UNCEDMASK, UNCERRSEV
        RasSetup->Pcie.PcieAerNfatErrBitMap = (BIT14 | BIT19 | BIT21);
      }
      RasSetup->Pcie.PcieAerFatErrEn = SystemConfiguration->PcieAerFatErrEn;
      if (RasSetup->Pcie.PcieAerFatErrEn != 0x00) {
        // UNCERRMSK, UNCEDMASK, UNCERRSEV
        RasSetup->Pcie.PcieAerFatErrBitMap = (BIT4 | BIT5 | BIT13 | BIT17 | BIT18);
      }
      RasSetup->Pcie.SerrPropEn = SystemConfiguration->SerrPropEn;
      RasSetup->Pcie.PerrPropEn = SystemConfiguration->PerrPropEn;
      RasSetup->Pcie.OsSigOnSerrEn = SystemConfiguration->OsSigOnSerrEn;
      RasSetup->Pcie.OsSigOnPerrEn = SystemConfiguration->OsSigOnPerrEn;
    }

    // Platform
    RasSetup->Platform.CaterrGpioSmiEn = SystemConfiguration->CaterrGpioSmiEn;
    
  }
  
  gBS->FreePool (SystemConfiguration);
  gBS->FreePool (SetupData);
  return Status;
}


/**
  Prime RAS Setup Structure based on SystemRasType

  This function disables IoMCA, LER, Viral and LMCE if the SystemRasType is not Advanced

  @param[in] **RasSetupPtr   A pointer to the RAS Setup Structure

  @retval NONE

**/
VOID
PrimeRasSetupStructure(
      IN   SYSTEM_RAS_SETUP         *RasSetup,
      IN   EFI_RAS_SYSTEM_TOPOLOGY   *RasSystemTopology
)
{
  if ((RasSystemTopology->SystemInfo.SystemRasType == STANDARD_RAS) || (RasSystemTopology->SystemInfo.SystemRasType == CORE_RAS)) {
     RASDEBUG ((DEBUG_INFO, "The SystemRasType is identified as STANDARD_RAS or CORE_RAS\n"));
     RASDEBUG ((DEBUG_INFO, "Disabling LER, Viral, LMCE, CPUHotPlug, Cloaking, McaBankErrorInj..\n"));
     RasSetup->Iio.LerEn = 0;
     RasSetup->ViralEn = 0;
     RasSetup->Qpi.QpiCpuSktHotPlugEn = 0;
     RasSetup->CloakingEn = 0;
     RasSetup->ErrInj.McaBankErrInjEn = 0;
   }
}
/**
  Initialize MP Sync Data Structure

  This function allocates memory for the MP Sync Data Structure and populates it

  @param[out] **SmmErrorMpSyncDataPtr   A pointer to the pointer for the MP Sync Data Structure

  @retval Status

**/
EFI_STATUS
InitMcaSyncGlobals (
      OUT   SMM_ERROR_MP_SYNC_DATA  **SmmErrorMpSyncDataPtr
)
{
  UINT32                    i;
  EFI_STATUS                Status;    
  SMM_ERROR_MP_SYNC_DATA    *SmmErrorMpSyncData;
  
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, 
                  sizeof (SMM_ERROR_MP_SYNC_DATA), 
                  (VOID **)SmmErrorMpSyncDataPtr);
  ASSERT_EFI_ERROR (Status);  
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmmErrorMpSyncData = *SmmErrorMpSyncDataPtr;

  SmmErrorMpSyncData->Broadcast = FALSE;
  InitializeSpinLock (&SmmErrorMpSyncData->SerialLock);

  for (i = 0; i < FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber); i++) {
    SetMem (&SmmErrorMpSyncData->CpuData[i], sizeof (SMM_ERROR_CPU_DATA_BLOCK), 0);
    SmmErrorMpSyncData->CpuData[i].EventLog.ErrorsFound = FALSE;
    SmmErrorMpSyncData->CpuData[i].EventLog.Events[McbankSrc].SeverityMap = 0;
    SmmErrorMpSyncData->CpuData[i].EventLog.Events[McbankSrc].ErrorFound = FALSE;
  }

  return Status;
}

/**
  Register an error listener. 

  This function allows a module to add its listener to the 
  list of listeners that will be executed to log reported errors.

  @param[in] FunctionPtr          A pointer to the listener

  @retval Status.

**/
EFI_STATUS
RegisterErrorListener (
  IN      EFI_ERROR_LISTENER_FUNCTION  FunctionPtr
  )
{
  EFI_STATUS            Status;
  LISTENER_LIST_ENTRY   *NewEntry;

  Status = EFI_SUCCESS;

  if (FunctionPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (LISTENER_LIST_ENTRY),
                    &NewEntry
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NewEntry->Listener = FunctionPtr;
  NewEntry->NextListener = NULL;

  if (ListenerListHead == NULL) {
    ListenerListHead = NewEntry;
    ListenerListTail = NewEntry;
  } else {
    ListenerListTail->NextListener = NewEntry;
    ListenerListTail = ListenerListTail->NextListener;
  }

  return Status;
}

/**
  Register an OS notifier. 

  This function allows a module to add its notifier to the 
  list of OS Notifiers that will be executed to to notify 
  the OS that an error occurred.

  @param[in] FunctionPtr          A pointer to the notifier
  @param[in] Priority             The priority of the notifier, 
                                  with 0 being the highest priority

  @retval Status.

**/
EFI_STATUS
RegisterNotifier (
  IN      EFI_NOTIFIER_FUNCTION  FunctionPtr,
  IN      UINT8                  Priority
  )
{
  EFI_STATUS            Status;
  NOTIFIER_LIST_ENTRY   *NewEntry;
  NOTIFIER_LIST_ENTRY   *PrevEntry;
  NOTIFIER_LIST_ENTRY   *CurrEntry;

  Status = EFI_SUCCESS;

  if (FunctionPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (NOTIFIER_LIST_ENTRY),
                    &NewEntry
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NewEntry->Notifier = FunctionPtr;
  NewEntry->NextNotifier = NULL;
  NewEntry->Priority = Priority;
  CurrEntry = NULL;
  PrevEntry = NULL;

  if (NotifierListHead == NULL) {
    NotifierListHead = NewEntry;
    NotifierListTail = NewEntry;
  } else {
    for (CurrEntry = NotifierListHead; CurrEntry != NULL; ) {
      if (Priority < CurrEntry->Priority) {
        if (CurrEntry == NotifierListHead) {
          NewEntry->NextNotifier = CurrEntry;
          NotifierListHead = NewEntry;
        } else {
          PrevEntry->NextNotifier = NewEntry;
          NewEntry->NextNotifier = CurrEntry;
        }
        break;
      } else {
        if (CurrEntry == NotifierListTail) {
          CurrEntry->NextNotifier = NewEntry;
          NotifierListTail = NewEntry;
          break;
        } else {
          PrevEntry = CurrEntry;
          CurrEntry = CurrEntry->NextNotifier;
        }
      }
    }
  }

  return Status;
}

/**
  Log a reported error 

  This function logs an error.

  @param[in] ErrorRecord          A pointer to the error record header

  @retval Status.

**/
EFI_STATUS
LogReportedError (
  IN      UEFI_ERROR_RECORD_HEADER   *ErrorRecordHeader
  )
{
  LISTENER_LIST_ENTRY   *CurrentListener;

  CurrentListener = ListenerListHead;
  while (CurrentListener != NULL) {
    CurrentListener->Listener (ErrorRecordHeader);
    CurrentListener = CurrentListener->NextListener;
  }

  return EFI_SUCCESS;
}

/**
  Notify the OS

  This function takes the appropriate action for a particular error based on severity.

  @param[in] ErrorSeverity    The severity of the error to be handled

  @retval Status.

**/
EFI_STATUS
NotifyOs (
  IN      UINT8 ErrorSeverity
  )
{
  NOTIFIER_LIST_ENTRY   *CurrentNotifier;
  BOOLEAN               OsNotified;
  
  OsNotified = FALSE;

  OemNotifyOs (ErrorSeverity, &OsNotified);

  CurrentNotifier = NotifierListHead;
  while ((CurrentNotifier != NULL) && (OsNotified == FALSE)) {
    CurrentNotifier->Notifier (ErrorSeverity, &OsNotified);
    CurrentNotifier = CurrentNotifier->NextNotifier;
  }

  return EFI_SUCCESS;
}

/**
  Entry point for the Platform Error Handler. 

  This function function performs generic error handling
  initialization.  It is responsible for installing the Platform
  Error Handling Protocol.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval Status.

**/
EFI_STATUS
EFIAPI
InitializePlatformErrHandler (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS      Status;
  EFI_HANDLE      Handle;
  
  Status = gBS->LocateProtocol (
                &gEfiMpServiceProtocolGuid,
                NULL,
                &mMpService
                );
  ASSERT_EFI_ERROR (Status);

  // Allocate memory for the RAS Topology Structure and populate it
  PopulateRasTopologyStructure (&PlatformRasPolicyProtocol.EfiRasSystemTopology);

  if(PlatformRasPolicyProtocol.EfiRasSystemTopology->SystemInfo.CpuStepping  < B0_REV_SKX) {
    if((PlatformRasPolicyProtocol.EfiRasSystemTopology->SystemInfo.SystemRasType != STANDARD_RAS) &&
        (PlatformRasPolicyProtocol.EfiRasSystemTopology->SystemInfo.SystemRasType != ADVANCED_RAS)) {
         RASDEBUG ((DEBUG_INFO, " A-stepping non adv or std part... skipping error enabling\n"));
         return EFI_SUCCESS;
    }
  } else {
    if ((PlatformRasPolicyProtocol.EfiRasSystemTopology->SystemInfo.SystemRasType == S1WS_RAS) || (PlatformRasPolicyProtocol.EfiRasSystemTopology->SystemInfo.SystemRasType == HEDT_RAS)) {
       RASDEBUG ((DEBUG_INFO, " HEDT or 1SWS... skipping error enabling\n"));
       return EFI_SUCCESS;
    } else {
      if (PlatformRasPolicyProtocol.EfiRasSystemTopology->SystemInfo.SystemRasType == UNDEFINED_RAS) {
        RASDEBUG ((DEBUG_ERROR, "System is identified as UNKNOWN RAS Type. Skipping error enabling totally\n"));
        return EFI_SUCCESS;
      }
    }
  }

  // Allocate memory for the RAS Setup Structure and populate it
  Status = PopulateRasSetupStructure (&PlatformRasPolicyProtocol.SystemRasSetup);
  
  if (PlatformRasPolicyProtocol.SystemRasSetup->SystemErrorEn == 1) {
    RASDEBUG ((DEBUG_INFO, "System Error Logging Enabled\n"));
    
    // Allocate memory for the RAS Capability Structure and populate it
    Status = PopulateRasCapabilityStructure (&PlatformRasPolicyProtocol.SystemRasCapability);
    
    // Prime RasSetup Strucure based on SystemRasType
    PrimeRasSetupStructure (PlatformRasPolicyProtocol.SystemRasSetup, PlatformRasPolicyProtocol.EfiRasSystemTopology);

    // Call OEM Library to override Setup Structure
    Status = OemOverrideRasSetupStructure (PlatformRasPolicyProtocol.SystemRasSetup);
       
    // Call OEM Library to override RAS Capability Structure
    Status = OemOverrideRasCapabilityStructure (&PlatformRasPolicyProtocol.SystemRasCapability->PlatformRasCapability);

    // Call OEM Library to override RAS Topology Structure
    Status = OemOverrideRasTopologyStructure (PlatformRasPolicyProtocol.EfiRasSystemTopology);
    
    // Initialize the MP Sync data structure
    Status = InitMcaSyncGlobals (&PlatformErrorHandlingProtocol.SmmErrorMpSyncData);

    // Register Platform Error Handling Protocol
    Handle = NULL;
    Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gEfiPlatformErrorHandlingProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &PlatformErrorHandlingProtocol
                    );
    ASSERT_EFI_ERROR (Status);

    // Register Platform RAS Policy Protocol
    Handle = NULL;
    Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gEfiPlatformRasPolicyProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &PlatformRasPolicyProtocol
                    );
    ASSERT_EFI_ERROR (Status);
  }
  
  return Status;
}
