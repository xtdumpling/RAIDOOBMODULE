//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/** @file

  Code File for CPU Power Management

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  PpmInfo.c

**/

#include "PpmInitialize.h"
#include <Library/CpuConfigLib.h>
#include <Library/BaseLib.h>
#include <Protocol/IioSystem.h>

PPM_FROM_PPMINFO_HOB *mPpmInfo;

EFI_MP_SERVICES_PROTOCOL        *mMpService = NULL;
EFI_GUID gEfiMpServiceProtocolGuid = EFI_MP_SERVICES_PROTOCOL_GUID;


VOID 
EFIAPI
DetectHwpFeature (
  EFI_CPU_PM_STRUCT *ppm 
  )
{
	UINT8		socket;

	ppm->Info->HwpmSupport = ACC_ENABLE | HWP_ENABLE;
	for(socket = 0; socket < MAX_SOCKET; socket++) {
          if (ppm->Info->SocketPresentBitMap & (1 << socket)) {
            ppm->Info->SocketNumber = socket;
            ppm->Info->HwpmSupport &= GetHwpmSupport(ppm);
          }
	}

	return;
}

VOID InitializeCpuInfoStruct( VOID ){

  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;
  IIO_UDS *UdsHobPtr;
  UINT8 socket;
  EFI_STATUS Status;
  EFI_HOB_GUID_TYPE *GuidHob;
  VOID *HobList;
  EFI_GUID UniversalDataGuid=IIO_UNIVERSAL_DATA_GUID;
  MSR_REGISTER CoreThreadCount;
  UINT64      RegAddr = 0;
  UINTN       NumberOfCPUs = 0;
  UINTN       numberOfEnabledCPUs = 0;
  UINT32      CapId4 = 0;
  UINT32      RegEax;
  UINT32      mNumOfBitShift;
  EFI_IIO_SYSTEM_PROTOCOL     *IioSystemProtocol = NULL;
  IIO_GLOBALS                 *IioGlobalData = NULL;
  UINT8 PortIndex, Aspm;

  AsmCpuid (1, &RegEax, NULL, NULL, NULL);  //EFI_CPUID_VERSION_INFO
  //
  // Find out the bit shift so it can be used to identify APs on other sockets
  //
  AsmCpuidEx(0xB, 1, &mNumOfBitShift, NULL, NULL, NULL); //EFI_CPUID_CORE_TOPOLOGY

  Status = gBS->LocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &CpuCsrAccess);
  ASSERT_EFI_ERROR (Status);

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  ASSERT_EFI_ERROR (Status);

  GuidHob    = GetFirstGuidHob (&UniversalDataGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return;
  }
  UdsHobPtr = GET_GUID_HOB_DATA(GuidHob); 

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (PPM_FROM_PPMINFO_HOB), (VOID **) &mPpmInfo );
  ASSERT_EFI_ERROR (Status);
  ZeroMem(mPpmInfo, sizeof(PPM_FROM_PPMINFO_HOB));

  //
  // Need to get the IioGlobalData
  //
  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &IioSystemProtocol);
  ASSERT_EFI_ERROR (Status);

  IioGlobalData = IioSystemProtocol->IioGlobalData;

  for(socket = 0; socket < MAX_SOCKET; socket++){
    mPpmInfo->OutKtiPerLinkL1En[socket] = UdsHobPtr->SystemStatus.OutKtiPerLinkL1En[socket];
    mPpmInfo->OutPciePerLinkL1En[socket] = 0;
    for(PortIndex = 0; PortIndex < NUMBER_PORTS_PER_SOCKET; PortIndex++){
      Aspm = IioGlobalData->SetupData.PcieAspm[(socket * NUMBER_PORTS_PER_SOCKET) + PortIndex];
      if ((Aspm == PCIE_ASPM_AUTO) || (Aspm == PCIE_ASPM_L1_ONLY)) {
        mPpmInfo->OutPciePerLinkL1En[socket] |= (1 << PortIndex);   // ASPM = L1
      }
    }
  }

  mPpmInfo->SocketPresentBitMap = UdsHobPtr->SystemStatus.socketPresentBitMap;
  mPpmInfo->DdrXoverMode = UdsHobPtr->SystemStatus.DdrXoverMode;
  mPpmInfo->mNumOfBitShift = (UINT32) (mNumOfBitShift & 0x1F);
  mPpmInfo->CpuStepping = (UINT32) (RegEax & 0xF);
  mPpmInfo->CpuType = (UINT32) (RegEax >> 4);

  CoreThreadCount.Qword = AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
  UdsHobPtr->SystemStatus.CpuPCPSInfo = (UINT32) (CoreThreadCount.Dwords.Low & THREAD_COUNT_MASK);
  //DEBUG((EFI_D_ERROR, ":::: CoreThreadCount.Qword = %x  %x\n", CoreThreadCount.Dwords.High, CoreThreadCount.Dwords.Low));
  if (CoreThreadCount.Dwords.High != CoreThreadCount.Dwords.Low) {
    UdsHobPtr->SystemStatus.CpuPCPSInfo |= B_PCPS_HT_ENABLE;
  }

  CapId4 = CpuCsrAccess->ReadCpuCsr(0, 0, CAPID4_PCU_FUN3_REG);

  if ((CapId4 & B_PCPS_DISABLE) == 0) {
    UdsHobPtr->SystemStatus.CpuPCPSInfo |= B_PCPS_SPD_ENABLE;
  }

  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  &mMpService
                  );
  ASSERT (mMpService);
  //
  // Determine the number of processors
  //
  mMpService->GetNumberOfProcessors (
              mMpService,
              &NumberOfCPUs,
              &numberOfEnabledCPUs
              );

  mPpmInfo->NumberOfProcessors = NumberOfCPUs;

  RegAddr = PcdGet64(PcdCpuPmStructAddr);
  if(RegAddr == 0) {
    DEBUG ( (EFI_D_ERROR, " \n\n :: !!!NULL pointer on CpuPM struct PCD!!!. \n\n") );
    ASSERT(RegAddr != 0);
  }

  mPpmLib.Info  = mPpmInfo;
  mPpmLib.Setup = (EFI_PPM_STRUCT *)(RegAddr);
  mPpmLib.CpuCsrAccess = CpuCsrAccess;
  mPpmLib.Info->EistCap = ((PcdGet32 (PcdCpuProcessorFeatureCapability) & PCD_CPU_TURBO_MODE_BIT) ? TRUE : FALSE);
  mPpmLib.Info->CapId4 = CapId4;

  GetMsrTemperatureTarget((EFI_CPU_PM_STRUCT *)&mPpmLib);

  if(mPpmLib.Info->CpuType == CPU_FAMILY_SKX) {
    DetectHwpFeature((EFI_CPU_PM_STRUCT *)&mPpmLib);
  }

  return;
}
