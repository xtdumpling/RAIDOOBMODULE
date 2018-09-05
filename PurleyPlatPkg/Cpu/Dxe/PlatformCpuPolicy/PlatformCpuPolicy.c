///**************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.03
//    Bug Fix : Add X11DDW power saving.
//    Reason  : 
//    Auditor : Leon Xu
//    Date    : Aug/04/2017
//
//  Rev. 1.02
//    Bug Fix : Fixed SMBIOS type 4 Socket Designation doesn't match with PCB
//              silkscreens on UP project. It should be shown "CPU" instead of
//              "CPU1". (PB #74492 "Some incorrect value in SMBIOS".)
//    Reason  : Follows SMBIOS_Structure_Template_0.7_Jun262015.xls
//    Auditor : Joe Jhang
//    Date    : Nov/10/2016
//
//  Rev. 1.01   Add always turbo mode function.
//    Bug Fix:
//    Reason:   Reference from Grantlley, control by TOKEN "MAX_PERFORMANCE_OPTION", default disabled.
//    Auditor:  Jimmy Chiu
//    Date:     May/16/2016
//
//  Rev. 1.00
//    Bug Fix:  Fix Type 4 CPU socket name of SMBIOS not meet our MB silkscreen
//    Reason:   
//    Auditor:  Salmon Chen
//    Date:     Jan/20/2016
//
//****************************************************************************
//****************************************************************************
/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.

  @file PlatformCpuPolicy.c

  EPG specific CPU Policy Platform Driver. Formely known as PlatformMicrocode.

**/

#include <PiDxe.h>
#include <Protocol/IntelCpuPcdsSetDone.h>
#include <Setup/IioUniversalData.h>
#include <Protocol/PlatformCpu.h>
#include <Protocol/PlatformType.h>
#include <Guid/SetupVariable.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/CpuConfigLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/CpuPpmLib.h>
#include <Library/SetupLib.h>
#include <Platform.h>
#include <Protocol/CpuCsrAccess.h>
#include <Protocol/SystemBoard.h>
#include "RcRegs.h"
#include "PlatformHost.h"
#include "BiosGuardDefinitions.h"
#include <Library/PlatformHooksLib.h>

extern EFI_GUID gBiosGuardHobGuid;
VOID
InitializeBiosGuardToolsIntCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

#if SMCPKG_SUPPORT
#if defined(NUMBER_CPU_SOCKETS) && (NUMBER_CPU_SOCKETS == 1)
CHAR16 mCpuSocketStr[1][4] = {L"CPU"};
#else
CHAR16 mCpuSocketStr[8][5] = {L"CPU1", L"CPU2", L"CPU3", L"CPU4", L"CPU5", L"CPU6", L"CPU7", L"CPU8"};
#endif //#if defined(NUMBER_CPU_SOCKETS) && (NUMBER_CPU_SOCKETS == 1)
#else
CHAR16 mCpuSocketStr[8][5] = {L"CPU0", L"CPU1", L"CPU2", L"CPU3", L"CPU4", L"CPU5", L"CPU6", L"CPU7"};
#endif  //#if SMCPKG_SUPPORT == 1
CHAR16 mCpuAssetTagStr[] = L"UNKNOWN";
EFI_CPU_CSR_ACCESS_PROTOCOL *mCpuCsrAccess;
IIO_UDS                     *mIioUds;


/**
  Set platform CPU data that related to SMBIOS.
**/
VOID
PlatformCpuSmbiosData (
  VOID
  )
{
  UINT32                        CpuSocketCount;
  UINTN                         Index;
  CHAR16                        **CpuSocketNames;
  CHAR16                        **CpuAssetTags;
  SYSTEM_BOARD_PROTOCOL         *SystemBoard;
  EFI_STATUS                    Status = EFI_SUCCESS;

  Status = gBS->LocateProtocol (&gEfiDxeSystemBoardProtocolGuid, NULL, (VOID**)&SystemBoard); 
  ASSERT_EFI_ERROR (Status); 
  //
  // Set the count of CPU sockets on the board.
  //
  SystemBoard->UpdateCpuSmbiosData (&CpuSocketCount);    

  Status = PcdSet32S (PcdPlatformCpuSocketCount, CpuSocketCount);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return;

  CpuSocketNames = AllocatePool (CpuSocketCount * sizeof (UINTN));

  if (CpuSocketNames == NULL) {
    DEBUG ((EFI_D_ERROR, "\nEFI_OUT_OF_RESOURCES!!! AllocatePool() returned NULL pointer.\n"));
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    return;
  }

  CpuAssetTags = AllocatePool (CpuSocketCount * sizeof (UINTN));
  if (CpuAssetTags == NULL) {
    DEBUG ((EFI_D_ERROR, "\nEFI_OUT_OF_RESOURCES!!! AllocatePool() returned NULL pointer.\n"));
    ASSERT_EFI_ERROR (EFI_OUT_OF_RESOURCES);
    gBS->FreePool (CpuSocketNames);
    return;
  }

  for (Index = 0; Index < CpuSocketCount; Index++) {
    CpuSocketNames[Index] = mCpuSocketStr[Index];
    CpuAssetTags[Index] = mCpuAssetTagStr;
  }

  Status = PcdSet64S (PcdPlatformCpuSocketNames, (UINT64) (UINTN) CpuSocketNames);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return;
  
  Status = PcdSet64S (PcdPlatformCpuAssetTags, (UINT64) (UINTN) CpuAssetTags);
  ASSERT_EFI_ERROR (Status);
  return;
}

/**
  Check and allocate memory for IED trace, aslo report
  base and size (in 1M boundary) to scrtachpad register
  BIOSNONSTICKYSCRATCHPAD3 (Base address) and
  BIOSNONSTICKYSCRATCHPAD3 (size)
**/
VOID
CheckAndPopulateIedTraceMemory(
  UINTN       IedTraceSize
  )
{
  EFI_STATUS          Status;
  UINTN               NumPages;
  UINTN               Addr;
  UINTN               AllocateSize;
  UINT8               i;
  UINT32              RegData;


  AllocateSize = IedTraceSize;

  //valid Ied trace size 4M to 1GB
  ASSERT((IedTraceSize >= 0x400000) && (IedTraceSize <= 0x40000000));

  while(1) {
    NumPages = EFI_SIZE_TO_PAGES (AllocateSize + 0x200000); //IED need to start at even 1M boundary
    Status = gBS->AllocatePages (
                          AllocateAnyPages,
                          EfiReservedMemoryType,
                          NumPages,
                          &Addr
                          );
    if(Status == EFI_SUCCESS) {
      //make IedTrace start at even 1M boundary
      if(Addr & 0x100000) { //Addr starts somewhere inside odd Meg address
        Addr = (Addr & 0xFFF00000) + 0x100000; //make it at even 1M boundary
      } else { //Addr starts somehwere in an even Meg
        Addr = (Addr & 0xFFF00000) + 0x200000;
      }

      gBS->SetMem((UINT8 *)Addr, AllocateSize, 0);
      //set "INTEL RSVD" signature on begging address of IedTrace memory
      gBS->CopyMem((UINT8 *)Addr, "INTEL RSVD", 10);
      break;
    } else {
      AllocateSize /= 2;
      if(AllocateSize < 0x400000) { //min 4M
        DEBUG ((EFI_D_INFO, "Error(%x): Failed to allocate IedTrace memory\n", Status));
        Addr = 0;
        AllocateSize = 0;
        break;
      }
    }

  }

  if(AllocateSize != IedTraceSize) {
    DEBUG ((EFI_D_INFO, "Warning: Request Iedtrace Size: 0x%x, only allocate 0x%x != \n", IedTraceSize, AllocateSize));
  } else {
    DEBUG ((EFI_D_INFO, "Allocate 0x%x byte IedTrace Memory @0x%x\n", AllocateSize, Addr));
  }

  //now store info into scratchpad
  for(i = 0; i < MAX_SOCKET; i++) {
    if(mIioUds->PlatformData.CpuQpiInfo[i].Valid) {
      // populate IEDTrace Base (MB bounded) to NonSticky Scratchpad10
      RegData = (UINT32)(UINTN)(Addr >> 20);
      mCpuCsrAccess->WriteCpuCsr(i, 0, BIOSNONSTICKYSCRATCHPAD3_UBOX_MISC_REG, RegData);

      // populate IEDTrace Size in MB to NonSticky Scratchpad11
      RegData = (UINT32)(UINTN)(AllocateSize >> 20);
      mCpuCsrAccess->WriteCpuCsr(i, 0, BIOSNONSTICKYSCRATCHPAD11_UBOX_MISC_REG, RegData);

    }
  }
}


/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
VOID
CheckAndReAssignSocketId(
  VOID
  )
{
#define APICID_MASK_BIT14_8 0x7F    //current Si support programmable APICID up to 15bits
#define EFI_CPUID_CORE_TOPOLOGY 0xB
  CPU_SOCKET_ID_INFO    *pcdSktIdPtr;
  UINT32                i, IntraPackageIdBits;
  UINTN                 PcdSize;
  EFI_STATUS            Status;
  UINT32                MaxSocketCount;
  
  MaxSocketCount = FixedPcdGet32(PcdMaxCpuSocketCount);
  DEBUG ((EFI_D_ERROR, "::SocketCount %08x\n", MaxSocketCount));
  pcdSktIdPtr = (CPU_SOCKET_ID_INFO *)PcdGetPtr(PcdCpuSocketId);
// APTIOV_SERVER_OVERRIDE_RC_START : For DynamicEx, PCD should use the Method of PcdGetSize not LibPcdGetSize.
#if 0
  PcdSize = LibPcdGetSize (PcdToken (PcdCpuSocketId)); //MAX_SOCKET * sizeof(CPU_SOCKET_ID_INFO);
#else
  PcdSize = PcdGetSize (PcdCpuSocketId); //MAX_SOCKET * sizeof(CPU_SOCKET_ID_INFO);
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : For DynamicEx, PCD should use the Method of PcdGetSize not LibPcdGetSize.
  ASSERT(PcdSize == (MAX_SOCKET * sizeof(CPU_SOCKET_ID_INFO)));
  Status = PcdSetPtrS (PcdCpuSocketId, &PcdSize, (VOID *)pcdSktIdPtr);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return;
  DEBUG ((EFI_D_INFO, "::SockeId Pcd at %08x, size %x\n", PcdGetPtr(PcdCpuSocketId), PcdSize));

  for(i = 0; i < MAX_SOCKET; i++) {
    if(mIioUds->PlatformData.CpuQpiInfo[i].Valid) {
      pcdSktIdPtr[i].DefaultSocketId = mIioUds->PlatformData.CpuQpiInfo[i].SocId;
      pcdSktIdPtr[i].NewSocketId     = mIioUds->PlatformData.CpuQpiInfo[i].SocId;
    } else {
      pcdSktIdPtr[i].DefaultSocketId = (UINT32)-1;   //make sure Default and New are same
      pcdSktIdPtr[i].NewSocketId     = (UINT32)-1;
    }
  }

  AsmCpuidEx (EFI_CPUID_CORE_TOPOLOGY, 1, &IntraPackageIdBits, NULL, NULL, NULL);
  //assign new socketId
  for(i = 0; i < MAX_SOCKET; i++) {

    if(pcdSktIdPtr[i].DefaultSocketId == (UINT32)-1) continue;

    switch(IntraPackageIdBits) {
      case 4: //socket bit starts from bit4 of ApicId
      case 5: //socket bit starts from bit5 of ApicId
        if(MAX_SOCKET == 4) {
          pcdSktIdPtr[i].NewSocketId |= (APICID_MASK_BIT14_8 << (8 - IntraPackageIdBits));
        } else {
          //3bit in lower 8bit as skt field, to avoid ApicID= FFs, leave bit8 untouched for 8S
          pcdSktIdPtr[i].NewSocketId |= (0x7E << (8 - IntraPackageIdBits));      //leave bit8 to 0 so we don't have FFs in ApicId
        }
        break;

     case 6: //socket bit starts from bit6 of ApicId
        if(MAX_SOCKET == 4) {
          //only 2bit in lower 8bit as skt field, to avoid ApicID= FFs, leave bit8 untouched for 4S
          pcdSktIdPtr[i].NewSocketId |= (0x7E << (8 - IntraPackageIdBits));
        } else {
          //only 2bit in lower 8bit as skt field, to avoid ApicID= FFs, leave bit9 untouched for 8S
          pcdSktIdPtr[i].NewSocketId |= (0x7C << (8 - IntraPackageIdBits));
        }
        break;

     default:
        DEBUG ((EFI_D_INFO, "::Need more info to make sure we can support!!!\n"));
        break;

    } //end switch
  }
}


/**

  This is the EFI driver entry point for the CpuPolicy Driver. This
  driver is responsible for getting microcode patches from FV.

  @param ImageHandle     - Handle for the image of this driver.
  @param SystemTable     - Pointer to the EFI System Table.

  @retval EFI_SUCCESS     - Protocol installed sucessfully.

**/
EFI_STATUS
EFIAPI
PlatformCpuPolicyEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                       Status;
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                 SetupData; // 
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  UINT32                           CpuPolicy;
  UINT32                           CpuPolicyEx1;
  EFI_HANDLE                       Handle;
  UINT32                           CsrSapmCtl = 0;
  UINT32                           CsrPerfPlimitCtl = 0;
  UINT8                            ConfigTDPCtrl;
  UINT8                            PCPSOptions = 0;
  UINT32                           AdvPwrMgtCtl;
  UINT8                            socket;
  UINT32                           *UpiInPkgCEntry = NULL;
  UINT32                           *PcieInPkgCEntry = NULL;
  UINT32                           MsrPowerCtlLow  = 0;
  UINT32                           MsrTurboPowerLimitHigh = 0;
  UINT32                           MsrTurboPowerLimitLow = 0;
  UINT32                           MsrPriPlaneCurrentCfgCtlHigh = 0;
  UINT32                           MsrPriPlaneCurrentCfgCtlLow = 0;
  UINT32                           CsrDynamicPerfPowerCtl = 0;
  UINT32                           CsrPcieIltrOvrd = 0;
  UINT32                           MsrPerfBiasConfig = 0;
  MSR_REGISTER                     *CStateLatencyCtrl = NULL;
  EFI_PLATFORM_TYPE_PROTOCOL       *PlatformType;
  UINT32                           CpuFamilyModelStepping;
  UINT32                           scratchpad7;
  UINT64                           i;
  UINT64                           *Addr;
  EFI_PPM_STRUCT                   *ppm = NULL;
  XE_STRUCT                        *XePtr = NULL;
  TURBO_RATIO_LIMIT_RATIO_CORES    *TurboRatioLimitRatioCores = NULL;
  UINT8                            PackageCStateSetting = 0;
  UINT8                            CpuCStateValue = 0;
// APTIOV_SERVER_OVERRIDE_RC_START
//  EFI_EVENT                        EndOfDxeEvent;
// APTIOV_SERVER_OVERRIDE_RC_END
#if SPS_SUPPORT
  MSR_REGISTER                     Ia32PerfCtrl;
  MSR_REGISTER                     PlatformInfo;
#endif  //SPS_SUPPORT

  EFI_GUID                         UniversalDataGuid = IIO_UNIVERSAL_DATA_GUID;
  EFI_HOB_GUID_TYPE                *GuidHob;

  GuidHob   = GetFirstGuidHob (&UniversalDataGuid);
  ASSERT (GuidHob != NULL);
  if(GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  mIioUds   = GET_GUID_HOB_DATA(GuidHob); 

  AsmCpuid (1, &CpuFamilyModelStepping, NULL, NULL, NULL);

  PlatformCpuSmbiosData ();

  ///
  /// Create an EndOfDxe event callback event for BIOS Guard.
  ///
  // APTIOV_SERVER_OVERRIDE_RC_START : Not used.
  #if 0
  Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                InitializeBiosGuardToolsIntCallback,
                NULL,
                &gEfiEndOfDxeEventGroupGuid,
                &EndOfDxeEvent
                );
  ASSERT_EFI_ERROR (Status);
  #endif
  // APTIOV_SERVER_OVERRIDE_RC_END : Not used.

  Status = gBS->LocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->AllocatePool (
                        EfiBootServicesData,
                        sizeof(EFI_PPM_STRUCT),
                        &Addr
                        );
  if(Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_INFO, "::Failed to allocate mem for PPM Struct\n"));
    ASSERT_EFI_ERROR (Status);      //may need to create a default
  } else {
    ZeroMem(Addr, sizeof(EFI_PPM_STRUCT));
    // APTIOV_SERVER_OVERRIDE_RC_START : To resolve system hang at exception D if EFI_PPM_STRUCT was allocated a base which is above 4G.
    i = (UINT64)(*(UINT64 *)(&Addr));
    // APTIOV_SERVER_OVERRIDE_RC_END : To resolve system hang at exception D if EFI_PPM_STRUCT was allocated a base which is above 4G.
    ppm = (EFI_PPM_STRUCT *)(Addr);
    Status = PcdSet64S (PcdCpuPmStructAddr, i);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    DEBUG ((EFI_D_INFO, "::PPM mem allocate @ %x %X %X\n", i, PcdGet64(PcdCpuPmStructAddr), ppm));
    UpiInPkgCEntry = (UINT32 *)(((EFI_PPM_STRUCT *)Addr)->Cst.PkgCstEntryCriteriaMaskKti);
    PcieInPkgCEntry = (UINT32 *)(((EFI_PPM_STRUCT *)Addr)->Cst.PkgCstEntryCriteriaMaskPcie);
    XePtr = (XE_STRUCT *)(&((EFI_PPM_STRUCT *)Addr)->Xe);
    TurboRatioLimitRatioCores = (TURBO_RATIO_LIMIT_RATIO_CORES *)(&((EFI_PPM_STRUCT *)Addr)->TurboRatioLimitRatioCores);
    //DEBUG ((EFI_D_INFO, ":: PkgC @ %X XE @ %X\n", PkgCstEntryCriteriaMask, XePtr));

    CStateLatencyCtrl = (MSR_REGISTER *)(ppm->Cst.LatencyCtrl);

  }

  //
  // Read the current system configuration variable store.
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  ZeroMem(&SetupData, sizeof(INTEL_SETUP_DATA));
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  Status = GetEntireConfig(&SetupData);

  //
  // Hardcoding value to always use offset table for 0.9mohm boards
  // Customers may re-enable Setup question tied to this or change hardcode value as needed
  //
  SetupData.SocketConfig.PowerManagementConfig.UsePmaxOffsetTable = 1;

  if (EFI_ERROR(Status)) {
    //
    // Safe default settings.
    //
    CpuPolicy = PCD_CPU_HT_BIT |
                PCD_CPU_C1E_BIT |
                PCD_CPU_EXECUTE_DISABLE_BIT |
                PCD_CPU_MACHINE_CHECK_BIT |
                PCD_CPU_DCU_PREFETCHER_BIT |
                PCD_CPU_IP_PREFETCHER_BIT |
                PCD_CPU_MONITOR_MWAIT_BIT |
                PCD_CPU_THERMAL_MANAGEMENT_BIT |
                PCD_CPU_EIST_BIT;

    CpuPolicyEx1 = PCD_CPU_MLC_STREAMER_PREFETCHER_BIT |
                   PCD_CPU_MLC_SPATIAL_PREFETCHER_BIT |
                   PCD_CPU_ENERGY_PERFORMANCE_BIAS_BIT |
                   PCD_CPU_TURBO_MODE_BIT |
                   PCD_CPU_C_STATE_BIT;

    Status = PcdSet8S (PcdCpuDcuMode, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet8S (PcdCpuProcessorMsrLockCtrl, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;

    Status = PcdSet64S(PcdCpuIioLlcWaysBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuExpandedIioLlcWaysBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuRemoteWaysBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet8S(PcdPchTraceHubEn, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuRRQCountThreshold, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuQlruCfgBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuIioLlcWaysBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuExpandedIioLlcWaysBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuRemoteWaysBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet8S(PcdPchTraceHubEn, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuRRQCountThreshold, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuQlruCfgBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuIioLlcWaysBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuExpandedIioLlcWaysBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuRemoteWaysBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet8S(PcdPchTraceHubEn, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;

    Status = PcdSet64S(PcdCpuQlruCfgBitMask, 0);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    SetupData.SocketConfig.SocketProcessorCoreConfiguration.IedTraceSize = 0;

    Status = PcdSet8S (PcdSbspSelection, 0xFF);   //default to auto
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    ppm->PcpsCtrl = 0;
    ppm->FastRaplDutyCycle = 0;

  } else {

	scratchpad7 = mCpuCsrAccess->ReadCpuCsr(0, 0, BIOSSCRATCHPAD7_UBOX_MISC_REG);
	DEBUG ((EFI_D_INFO, "AYP Debug scratchpad7: %x Stepping %x\n", scratchpad7 >> 31, CpuFamilyModelStepping & 0x0f));
    if ((scratchpad7 >> 31) && (CpuFamilyModelStepping & 0x0f) < 0x04) {
      SetupData.SocketConfig.PowerManagementConfig.PackageCState = 0;
	}
	
	if (SetupData.SocketConfig.PowerManagementConfig.PackageCState == PPM_AUTO) {
        PackageCStateSetting = 3;   //POR Default = C6
    } else {
        PackageCStateSetting = SetupData.SocketConfig.PowerManagementConfig.PackageCState;
    }

    if ((SetupData.SocketConfig.PowerManagementConfig.C6Enable == PPM_AUTO) ||
          SetupData.SocketConfig.PowerManagementConfig.ProcessorAutonomousCstateEnable) {
        CpuCStateValue |= C6_ENABLE;   //POR Default = Enabled
    } else {
        CpuCStateValue |= (SetupData.SocketConfig.PowerManagementConfig.C6Enable * C6_ENABLE);
    }

    Status = PcdSet8S (PcdCpuCoreCStateValue, CpuCStateValue);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;

    //
    // If ACC enabled, then override C1E to be enabled
    //
    if (SetupData.SocketConfig.PowerManagementConfig.ProcessorAutonomousCstateEnable) {
      SetupData.SocketConfig.PowerManagementConfig.ProcessorC1eEnable = TRUE;
    }

    //
    // Verify that the value being set is within the valid range 0 to MAX_SOCKET - 1
    //
    if (SetupData.SocketConfig.SocketProcessorCoreConfiguration.BspSelection > MAX_SOCKET)
      SetupData.SocketConfig.SocketProcessorCoreConfiguration.BspSelection= 0xFF;
    Status = PcdSet8S (PcdSbspSelection, SetupData.SocketConfig.SocketProcessorCoreConfiguration.BspSelection);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    //
    // Map CPU setup options to PcdCpuProcessorFeatureUserConfiguration
    //
    CpuPolicy = (SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorHyperThreadingDisable ? 0 : PCD_CPU_HT_BIT) |
                (SetupData.SocketConfig.PowerManagementConfig.ProcessorEistEnable ? PCD_CPU_EIST_BIT : 0) |
                (SetupData.SocketConfig.PowerManagementConfig.ProcessorC1eEnable ? PCD_CPU_C1E_BIT : 0) |
                (SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorVmxEnable ? PCD_CPU_VT_BIT : 0) |
                (SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorSmxEnable ? PCD_CPU_LT_BIT : 0) |
                (SetupData.SocketConfig.SocketProcessorCoreConfiguration.FastStringEnable ? PCD_CPU_FAST_STRING_BIT : 0) |
                (SetupData.SocketConfig.SocketProcessorCoreConfiguration.CpuidMaxValue ? PCD_CPU_MAX_CPUID_VALUE_LIMIT_BIT : 0) |
                (SetupData.SocketConfig.SocketProcessorCoreConfiguration.ExecuteDisableBit ? PCD_CPU_EXECUTE_DISABLE_BIT : 0) |
                (SetupData.SocketConfig.SocketProcessorCoreConfiguration.MachineCheckEnable ? PCD_CPU_MACHINE_CHECK_BIT : 0) |
                (SetupData.SocketConfig.SocketProcessorCoreConfiguration.DCUStreamerPrefetcherEnable ? PCD_CPU_DCU_PREFETCHER_BIT : 0) |
                (SetupData.SocketConfig.SocketProcessorCoreConfiguration.DCUIPPrefetcherEnable ? PCD_CPU_IP_PREFETCHER_BIT : 0) |
                PCD_CPU_MONITOR_MWAIT_BIT | //never disable Mwait due to silicon HSD
                (SetupData.SocketConfig.PowerManagementConfig.TurboMode ? PCD_CPU_TURBO_MODE_BIT : 0) |
                (SetupData.SocketConfig.PowerManagementConfig.EnableThermalMonitor ? PCD_CPU_THERMAL_MANAGEMENT_BIT : 0);

    if (SetupData.SocketConfig.PowerManagementConfig.TStateEnable && (SetupData.SocketConfig.PowerManagementConfig.OnDieThermalThrottling > 0)) {
       CpuPolicy |= (SetupData.SocketConfig.PowerManagementConfig.TStateEnable ? PCD_CPU_TSTATE_BIT : 0);
    }

    CpuPolicyEx1 = (SetupData.SocketConfig.SocketProcessorCoreConfiguration.MlcStreamerPrefetcherEnable ? PCD_CPU_MLC_STREAMER_PREFETCHER_BIT : 0) |
                   (SetupData.SocketConfig.SocketProcessorCoreConfiguration.MlcSpatialPrefetcherEnable ? PCD_CPU_MLC_SPATIAL_PREFETCHER_BIT : 0) |
                   (SetupData.SocketConfig.SocketProcessorCoreConfiguration.ThreeStrikeTimer ? PCD_CPU_THREE_STRIKE_COUNTER_BIT : 0) |
                   PCD_CPU_ENERGY_PERFORMANCE_BIAS_BIT |
                   (SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorX2apic ? PCD_CPU_X2APIC_BIT : 0) |
                   (SetupData.SocketConfig.SocketProcessorCoreConfiguration.AesEnable ? PCD_CPU_AES_BIT : 0) |
                   (SetupData.SocketConfig.SocketProcessorCoreConfiguration.PCIeDownStreamPECIWrite ? PCD_CPU_PECI_DOWNSTREAM_WRITE_BIT : 0) |
                   PCD_CPU_C_STATE_BIT;


    PCPSOptions =  (SetupData.SocketConfig.PowerManagementConfig.ProcessorSinglePCTLEn ? PCD_CPU_PCPS_SINGLEPCTL : 0) |
                   (SetupData.SocketConfig.PowerManagementConfig.ProcessorSPD ? PCD_CPU_PCPS_SPD : 0) |
                   (SetupData.SocketConfig.PowerManagementConfig.PStateDomain ? PCD_CPU_PCPS_PSTATEDOMAIN : 0) |
                   (UINT8) SetupData.SocketConfig.PowerManagementConfig.ProcessorEistPsdFunc;

#if SMCPKG_SUPPORT && MAX_PERFORMANCE_OPTION   //For always turbo feature
    if((SetupData.SocketConfig.PowerManagementConfig.PwrPerfTuning == 1) && (SetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS == 3)){
      SetupData.SocketConfig.PowerManagementConfig.ProcessorEistEnable = 1;
      SetupData.SocketConfig.PowerManagementConfig.TurboMode = 1;
      SetupData.SocketConfig.PowerManagementConfig.ProcessorEistPsdFunc = 0;
      SetupData.SocketConfig.PowerManagementConfig.BootPState = 0;
      SetupData.SocketConfig.PowerManagementConfig.PackageCState = 0;
      SetupData.SocketConfig.PowerManagementConfig.C6Enable = 0;
      SetupData.SocketConfig.PowerManagementConfig.ProcessorC1eEnable = 0;
      SetupData.SocketConfig.PowerManagementConfig.TStateEnable = 0;
      SetupData.SocketConfig.PowerManagementConfig.ProcessorSPD = 1;

      PackageCStateSetting = 0;
      CpuCStateValue &= ~(C6_ENABLE);
      PcdSet8 (PcdCpuCoreCStateValue, CpuCStateValue);
      
      CpuPolicy |= (PCD_CPU_EIST_BIT | PCD_CPU_TURBO_MODE_BIT);
      CpuPolicy &= ~(PCD_CPU_C1E_BIT | PCD_CPU_TSTATE_BIT);
      PCPSOptions |= (PCD_CPU_PCPS_SINGLEPCTL | PCD_CPU_PCPS_SPD);
    }
#endif //SMCPKG_SUPPORT && MAX_PERFORMANCE_OPTION
		   
    ppm->PcpsCtrl = PCPSOptions;
    ppm->OverclockingLock = SetupData.SocketConfig.PowerManagementConfig.OverclockingLock;

    ppm->FastRaplDutyCycle = SetupData.SocketConfig.PowerManagementConfig.FastRaplDutyCycle;

    if(mIioUds->PlatformData.EVMode)
      CpuPolicy &= ~PCD_CPU_LT_BIT;

    if (SetupData.SocketConfig.PowerManagementConfig.ProcessorEistEnable) {
      Status = PcdSetBoolS (PcdCpuHwCoordination, SetupData.SocketConfig.PowerManagementConfig.ProcessorEistPsdFunc ? FALSE : TRUE);
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) return Status;
    }

    Status = PcdSet16S (PcdCpuAcpiLvl2Addr, PCH_ACPI_BASE_ADDRESS + R_ACPI_LV2);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet8S (PcdCpuPackageCStateLimit, PackageCStateSetting);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;

    if ((SetupData.SocketConfig.PowerManagementConfig.TStateEnable) && (SetupData.SocketConfig.PowerManagementConfig.OnDieThermalThrottling > 0)) {
      Status = PcdSet8S (PcdCpuClockModulationDutyCycle, SetupData.SocketConfig.PowerManagementConfig.OnDieThermalThrottling);
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) return Status;
      ppm->TCCActivationOffset = SetupData.SocketConfig.PowerManagementConfig.TCCActivationOffset;
    }
    Status = PcdSet8S (PcdCpuDcuMode, SetupData.SocketConfig.SocketProcessorCoreConfiguration.DCUModeSelection);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;

    //
    // HSD 4926912 SKX: CPU Knobs add a eSMM setup option.
    //
    Status = PcdSetBoolS (PcdCpuSmmMsrSaveStateEnable, SetupData.SocketConfig.SocketProcessorCoreConfiguration.eSmmSaveState? TRUE : FALSE);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;

    if((!IsSimicsPlatform()) && SetupData.SystemConfig.EmcaMsmiEn != 0) {
      Status = PcdSetBoolS (PcdCpuSmmProtectedModeEnable, TRUE);    
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) return Status;
    } else {
      Status = PcdSetBoolS (PcdCpuSmmProtectedModeEnable, FALSE);  
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) return Status;
    }

    if((!IsSimicsPlatform()) && ((CpuFamilyModelStepping >> 4) == CPU_FAMILY_SKX)) {
      Status = PcdSetBoolS (PcdCpuSmmRuntimeCtlHooks, TRUE);
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) return Status;
    } else {
      Status = PcdSetBoolS (PcdCpuSmmRuntimeCtlHooks, FALSE);
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) return Status;
    }
    DEBUG ((EFI_D_INFO, ":: PcdCpuSmmRuntimeCtlHooks= %x\n", PcdGetBool(PcdCpuSmmRuntimeCtlHooks)));

    // 
    // HSD 4928956  SKX: option for SV to turnoff the SMM protected mode Entry 
    //


    if(mIioUds->PlatformData.EVMode || SetupData.SystemConfig.LmceEn) {
      Status = PcdSet8S (PcdCpuProcessorMsrLockCtrl, 0);
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) return Status;
    } else {
      Status = PcdSet8S (PcdCpuProcessorMsrLockCtrl, SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorMsrLockControl);
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) return Status;
    }

    Status = PcdSet64S(PcdCpuIioLlcWaysBitMask, SetupData.SocketConfig.SocketProcessorCoreConfiguration.IioLlcWaysMask);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuExpandedIioLlcWaysBitMask, SetupData.SocketConfig.SocketProcessorCoreConfiguration.ExpandedIioLlcWaysMask);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuRemoteWaysBitMask, SetupData.SocketConfig.SocketProcessorCoreConfiguration.RemoteWaysMask);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet8S(PcdPchTraceHubEn, SetupData.SocketConfig.SocketProcessorCoreConfiguration.PchTraceHubEn);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuQlruCfgBitMask, ((UINT64) SetupData.SocketConfig.SocketProcessorCoreConfiguration.QlruCfgMask_Hi << 32) | (UINT64)SetupData.SocketConfig.SocketProcessorCoreConfiguration.QlruCfgMask_Lo );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSet64S(PcdCpuRRQCountThreshold, mIioUds->PlatformData.RemoteRequestThreshold);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) return Status;

    //CSR SAPM CTL
    CsrSapmCtl = 0;

   for( socket = 0; socket < MAX_SOCKET; socket++) {
      CsrSapmCtl = (( SetupData.SocketConfig.PowerManagementConfig.Iio0PkgcClkGateDis[socket] << IIO012_PKGC_CLK_GATE_DISABLE_SHIFT) |
                   ( SetupData.SocketConfig.PowerManagementConfig.Iio1PkgcClkGateDis[socket] << (IIO012_PKGC_CLK_GATE_DISABLE_SHIFT + 1)) |
                   ( SetupData.SocketConfig.PowerManagementConfig.Iio2PkgcClkGateDis[socket] << (IIO012_PKGC_CLK_GATE_DISABLE_SHIFT + 2)) );

      CsrSapmCtl |= (( SetupData.SocketConfig.PowerManagementConfig.Kti23PkgcClkGateDis[socket] << KTI23_PKGC_CLK_GATE_DISABLE_SHIFT ) |
                 ( SetupData.SocketConfig.PowerManagementConfig.Kti01PkgcClkGateDis[socket] << KTI01_PKGC_CLK_GATE_DISABLE_SHIFT ) |
                 ( SetupData.SocketConfig.PowerManagementConfig.Mc1PkgcClkGateDis[socket] << MC1_PKGC_CLK_GATE_DISABLE_SHIFT ) |
                 ( SetupData.SocketConfig.PowerManagementConfig.Mc0PkgcClkGateDis[socket] << MC0_PKGC_CLK_GATE_DISABLE_SHIFT ) |
                 ( SetupData.SocketConfig.PowerManagementConfig.Kti01pllOffEna[socket] << KTI_PLL_OFF_EN_SHIFT) |
                 ( SetupData.SocketConfig.PowerManagementConfig.Kti23pllOffEna[socket] << (KTI_PLL_OFF_EN_SHIFT + 1) ) );

      if (SetupData.SocketConfig.MemoryConfig.OppSrefEn == 1) {
        CsrSapmCtl |= ((1 << MC0_PKGC_IO_VOLTAGE_REDUCTION_DISABLE_SHIFT) | (1 << MC1_PKGC_IO_VOLTAGE_REDUCTION_DISABLE_SHIFT) |
                      (1 << MC0_PKGC_DIG_VOLTAGE_REDUCTION_DISABLE_SHIFT) | (1 << MC1_PKGC_DIG_VOLTAGE_REDUCTION_DISABLE_SHIFT)) ;
      }

      CsrSapmCtl |= (( SetupData.SocketConfig.PowerManagementConfig.P0pllOffEna[socket] << IIO_PLL_OFF_EN_SHIFT) |
                 ( SetupData.SocketConfig.PowerManagementConfig.P1pllOffEna[socket] << (IIO_PLL_OFF_EN_SHIFT + 1) ) |
                 ( SetupData.SocketConfig.PowerManagementConfig.P2pllOffEna[socket] << (IIO_PLL_OFF_EN_SHIFT + 2) ) |
                 ( SetupData.SocketConfig.PowerManagementConfig.Mc0pllOffEna[socket] << MEM_PLL_OFF_EN_SHIFT) |
                 ( SetupData.SocketConfig.PowerManagementConfig.Mc1pllOffEna[socket] << (MEM_PLL_OFF_EN_SHIFT + 1) ) |
                 ( SetupData.SocketConfig.PowerManagementConfig.SetvidDecayDisable[socket] << SETVID_DECAY_DISABLE_SHIFT ) |
                 ( SetupData.SocketConfig.PowerManagementConfig.SapmCtlLock[socket] << SAPMCTL_LOCK_SHIFT) );

      ppm->SapmCtl[socket]  = CsrSapmCtl;
   }

    ppm->PmaxConfig  = (SetupData.SocketConfig.PowerManagementConfig.UsePmaxOffsetTable ? USER_PMAX_USE_OFFSET_TABLE : 0 ) |
                       SetupData.SocketConfig.PowerManagementConfig.PmaxOffset |
                      (SetupData.SocketConfig.PowerManagementConfig.PmaxSign ? USER_PMAX_NEGATIVE_BIT : 0);

    CsrPerfPlimitCtl = ( SetupData.SocketConfig.PowerManagementConfig.PerfPLmtThshld << PERF_PLIMIT_THRESHOLD_SHIFT ) |
                      ( SetupData.SocketConfig.PowerManagementConfig.PerfPLimitClipC << PERF_PLIMIT_CLIP_SHIFT ) |
                      ( SetupData.SocketConfig.PowerManagementConfig.PerfPlimitDifferential << PERF_PLIMIT_DIFFERENTIAL_SHIFT ) |
                      ( SetupData.SocketConfig.PowerManagementConfig.PerfPLimitEn << REPERF_PLIMIT_EN_SHIFT );
    ppm->PerPLimitCtl = CsrPerfPlimitCtl;

    ConfigTDPCtrl = ( SetupData.SocketConfig.PowerManagementConfig.ConfigTDPLevel << CONFIG_TDP_LEVEL_SHIFT );

    ppm->ConfigTDP = ConfigTDPCtrl;

    for( socket = 0; socket < MAX_SOCKET; socket++) {
        UpiInPkgCEntry[socket] = (SetupData.SocketConfig.PowerManagementConfig.Kti0In[socket] |
                                              (SetupData.SocketConfig.PowerManagementConfig.Kti1In[socket] << 1) |
                                              (SetupData.SocketConfig.PowerManagementConfig.Kti2In[socket] << 2) );

        if (SetupData.SocketConfig.PowerManagementConfig.PcieIio0In[socket]) {
          PcieInPkgCEntry[socket] |= SET_PCIEx_MASK;
        }
        if (SetupData.SocketConfig.PowerManagementConfig.PcieIio1In[socket]) {
          PcieInPkgCEntry[socket] |= (SET_PCIEx_MASK << 4);
        }
        if (SetupData.SocketConfig.PowerManagementConfig.PcieIio2In[socket]) {
          PcieInPkgCEntry[socket] |= (SET_PCIEx_MASK << 8);
        }
        if (SetupData.SocketConfig.PowerManagementConfig.PcieIio3In[socket]) {
          PcieInPkgCEntry[socket] |= (SET_PCIEx_MASK << 12);
        }
        if (SetupData.SocketConfig.PowerManagementConfig.PcieIio4In[socket]) {
          PcieInPkgCEntry[socket] |= (SET_PCIEx_MASK << 16);
        }
        if (SetupData.SocketConfig.PowerManagementConfig.PcieIio5In[socket]) {
          PcieInPkgCEntry[socket] |= (SET_PCIEx_MASK << 20);
        }

    } 

    AdvPwrMgtCtl = (SetupData.SocketConfig.PowerManagementConfig.SapmctlValCtl? PCD_CPU_SAPM_CTL_VAL_CTL : 0) |
                  (SetupData.SocketConfig.PowerManagementConfig.CurrentConfig? PCD_CPU_CURRENT_CONFIG : 0) |
                  (SetupData.SocketConfig.PowerManagementConfig.BootPState? PCU_CPU_EFFICIENT_BOOT : 0) |
                  (SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorMsrLockControl? CPU_MSR_LOCK : 0) |
                  (SetupData.SocketConfig.PowerManagementConfig.TurboPowerLimitCsrLock? TURBO_LIMIT_CSR_LOCK : 0);
 
    AdvPwrMgtCtl |= SetupData.SocketConfig.PowerManagementConfig.PkgCstEntryValCtl; //PCD_CPU_PKG_CST_ENTRY_VAL_CTL

#if SPS_SUPPORT
    if (SetupData.SocketConfig.PowerManagementConfig.BootPState == 2) {
      PlatformInfo.Qword =  AsmReadMsr64 (MSR_PLATFORM_INFO);
      Ia32PerfCtrl.Qword = AsmReadMsr64 (MSR_IA32_PERF_CTL);
      if ( (PlatformInfo.Dwords.High & (UINT32)B_IA32_PERF_CTRLP_STATE_TARGET) == (Ia32PerfCtrl.Dwords.Low & (UINT32)B_IA32_PERF_CTRLP_STATE_TARGET) ){
        AdvPwrMgtCtl |= PCU_CPU_EFFICIENT_BOOT;
        DEBUG ((EFI_D_INFO, "NM FW has configured Efficient Boot frequency on BSP.\n"));
      }
      else{
        AdvPwrMgtCtl &= ~PCU_CPU_EFFICIENT_BOOT;
        DEBUG ((EFI_D_INFO, "NM FW has configured Performance Boot frequency on BSP.\n"));
      }

    }
#endif

    if (SetupData.SocketConfig.PowerManagementConfig.ProcessorEistEnable == 0) {
      AdvPwrMgtCtl |= PCU_CPU_EFFICIENT_BOOT;
    } 

    if (((CpuFamilyModelStepping >> 4) == CPU_FAMILY_HSX) && SetupData.SocketConfig.PowerManagementConfig.PriPlnCurCfgValCtl) {
      AdvPwrMgtCtl |= PCD_CPU_PRI_PLN_CURR_CFG_CTL;
    }

    if ((PackageCStateSetting > 0) && SetupData.SocketConfig.PowerManagementConfig.DynamicL1) {
      AdvPwrMgtCtl |= DYNAMIC_L1_DISABLE;
    }

    if (SetupData.SocketConfig.PowerManagementConfig.SPTWorkaround) {
      AdvPwrMgtCtl |= SPT_PCH_WORKAROUND;
    }

    if (SetupData.SocketConfig.PowerManagementConfig.VccSAandVccIOdisable) {
      AdvPwrMgtCtl |= VCCSA_VCCIO_DISABLE;
    }
    ppm->AdvPwrMgtCtlFlags = AdvPwrMgtCtl;

    // MSR_POWER_CTL 0x1FC
    MsrPowerCtlLow = ( SetupData.SocketConfig.PowerManagementConfig.PkgCLatNeg << PCH_NEG_DISABLE_SHIFT ) |
                     ( SetupData.SocketConfig.PowerManagementConfig.LTRSwInput << LTR_SW_DISABLE_SHIFT ) |
                     ( SetupData.SocketConfig.PowerManagementConfig.PwrPerfTuning << PWR_PERF_TUNING_CFG_MODE_SHIFT ) |
                     ( SetupData.SocketConfig.PowerManagementConfig.PwrPerfSwitch << PWR_PERF_TUNING_ENABLE_DYN_SHIFT ) |
                     ( SetupData.SocketConfig.PowerManagementConfig.SAPMControl << PWR_PERF_TUNING_DISABLE_SAPM_SHIFT ) |
                     ( SetupData.SocketConfig.PowerManagementConfig.EETurboDisable << EE_TURBO_DISABLE_SHIFT ) |
                     ( SetupData.SocketConfig.PowerManagementConfig.ProchotLock << PROCHOT_LOCK_SHIFT ) |
                     ( SetupData.SocketConfig.PowerManagementConfig.ProcessorC1eEnable << C1E_ENABLE_SHIFT ) |
                     ( (SetupData.SocketConfig.PowerManagementConfig.EnableProcHot & 0x1) << DIS_PROCHOT_OUT_SHIFT ) |
                     ( (SetupData.SocketConfig.PowerManagementConfig.EnableProcHot & 0x2) >> 1 );

    // 5332865	BIOS needs to set bit 25 in MSR 0x1FC when enabling HWP autonomous out of band mode
    if (SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMEnable == 2) {   //if HWPM = OOB Mode
        MsrPowerCtlLow |= ( 1 << PWR_PERF_TUNING_CFG_MODE_SHIFT );
    }

    ppm->PowerCtl.Dwords.Low = MsrPowerCtlLow;

    ppm->ProchotRatio = SetupData.SocketConfig.PowerManagementConfig.ProchotResponseRatio;

    if ((CpuFamilyModelStepping >> 4) == CPU_FAMILY_HSX) {
      // PRIMARY_PLANE_CURRENT_CONFIG_CONTROL 0x601
      MsrPriPlaneCurrentCfgCtlHigh = ( SetupData.SocketConfig.PowerManagementConfig.Psi3Code << PSI3_CODE_SHIFT ) |
        ( SetupData.SocketConfig.PowerManagementConfig.Psi3Thshld << PSI3_THSHLD_SHIFT ) |
        ( SetupData.SocketConfig.PowerManagementConfig.Psi2Code << PSI2_CODE_SHIFT ) |
        ( SetupData.SocketConfig.PowerManagementConfig.Psi2Thshld << PSI2_THSHLD_SHIFT ) |
        ( SetupData.SocketConfig.PowerManagementConfig.Psi1Code << PSI1_CODE_SHIFT ) |
        ( SetupData.SocketConfig.PowerManagementConfig.Psi1Thshld << PSI1_THSHLD_SHIFT );
    } 

    MsrPriPlaneCurrentCfgCtlLow = ( SetupData.SocketConfig.PowerManagementConfig.PpcccLock << PPCCC_LOCK_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.CurrentLimit << CURRENT_LIMIT_SHIFT );

    ppm->PP0CurrentCfg.Dwords.High = MsrPriPlaneCurrentCfgCtlHigh;
    ppm->PP0CurrentCfg.Dwords.Low = MsrPriPlaneCurrentCfgCtlLow;

    // MSR_TURBO_POWER_LIMIT 0x610
    // CSR_TURBO_POWER_LIMIT 1:30:0:0xe8
    MsrTurboPowerLimitHigh = ( SetupData.SocketConfig.PowerManagementConfig.TurboPowerLimitLock << POWER_LIMIT_LOCK_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.PowerLimit2En << POWER_LIMIT_ENABLE_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.PkgClmpLim2 << PKG_CLMP_LIM_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.PowerLimit2Power );

    MsrTurboPowerLimitLow = ( SetupData.SocketConfig.PowerManagementConfig.PowerLimit1Time << POWER_LIMIT_1_TIME_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.PowerLimit1En << POWER_LIMIT_ENABLE_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.PowerLimit1Power );

    if ((CpuFamilyModelStepping >> 4) == CPU_FAMILY_HSX) {
      MsrTurboPowerLimitLow  |= ( SetupData.SocketConfig.PowerManagementConfig.PkgClmpLim1 << PKG_CLMP_LIM_SHIFT );
      MsrTurboPowerLimitHigh |= ( SetupData.SocketConfig.PowerManagementConfig.PkgClmpLim2 << PKG_CLMP_LIM_SHIFT );
    }

    if ((CpuFamilyModelStepping >> 4) == CPU_FAMILY_SKX) {
      MsrTurboPowerLimitHigh |= ( SetupData.SocketConfig.PowerManagementConfig.PowerLimit2Time << POWER_LIMIT_1_TIME_SHIFT );
    }

    ppm->TurboPowerLimit.Dwords.Low = MsrTurboPowerLimitLow;
    ppm->TurboPowerLimit.Dwords.High = MsrTurboPowerLimitHigh;

    // DYNAMIC_PERF_POWER_CTL (CSR 1:30:2:0x64)
    CsrDynamicPerfPowerCtl = ( SetupData.SocketConfig.PowerManagementConfig.UncrPerfPlmtOvrdEn << UNCORE_PERF_PLIMIT_OVERRIDE_ENABLE_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.EetOverrideEn << EET_OVERRIDE_ENABLE_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.IoBwPlmtOvrdEn << IO_BW_PLIMIT_OVERRIDE_ENABLE_SHIFT ) |
      //( SetupData.SocketConfig.PowerManagementConfig.ImcApmOvrdEn << IMC_APM_OVERRIDE_ENABLE_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.IomApmOvrdEn << IOM_APM_OVERRIDE_ENABLE_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.QpiApmOvrdEn << QPI_APM_OVERRIDE_ENABLE_SHIFT );   //4986218: Remove both changes from 4168487

    if((CpuFamilyModelStepping >> 4) == CPU_FAMILY_HSX) {
      CsrDynamicPerfPowerCtl |= (( SetupData.SocketConfig.PowerManagementConfig.EepLOverride << EEP_L_OVERRIDE_SHIFT ) |
        ( SetupData.SocketConfig.PowerManagementConfig.EepLOverrideEn << EEP_L_OVERRIDE_ENABLE_SHIFT ) |
        ( SetupData.SocketConfig.PowerManagementConfig.ITurboOvrdEn << I_TURBO_OVERRIDE_ENABLE_SHIFT ) |
        ( SetupData.SocketConfig.PowerManagementConfig.CstDemotOvrdEN << CST_DEMOTION_OVERRIDE_ENABLE_SHIFT ) |
        ( SetupData.SocketConfig.PowerManagementConfig.TrboDemotOvrdEn << TURBO_DEMOTION_OVERRIDE_ENABLE_SHIFT ));
    }

    ppm->DynamicPerPowerCtl = CsrDynamicPerfPowerCtl;

    // CSR_PCIE_ILTR_OVRD (CSR 1:30:1:78)
    // SW_LTR_OVRD (MSR 0xa02) -- not used
    CsrPcieIltrOvrd = ( SetupData.SocketConfig.PowerManagementConfig.SnpLatVld << SNOOP_LATENCY_VLD_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.SnpLatOvrd << FORCE_SNOOP_OVRD_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.SnpLatMult << SNOOP_LATENCY_MUL_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.SnpLatVal << SNOOP_LATENCY_Value_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.NonSnpLatVld << NON_SNOOP_LATENCY_VLD_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.NonSnpLatOvrd << FORCE_NON_SNOOP_OVRD_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.NonSnpLatMult << NON_SNOOP_LATENCY_MUL_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.NonSnpLatVal << NON_SNOOP_LATENCY_Value_SHIFT );

    ppm-> PcieIltrOvrd = CsrPcieIltrOvrd;

    if((CpuFamilyModelStepping >> 4) == CPU_FAMILY_SKX) { //Need to check if programming needs to be limited only if Turbo mode is enabled.
      for(i = 0; i < 8; i++) {
        TurboRatioLimitRatioCores->RatioLimitRatio[i] = SetupData.SocketConfig.PowerManagementConfig.TurboRatioLimitRatio[i];

        TurboRatioLimitRatioCores->RatioLimitRatioMask[i] = 0xFF;
        if (SetupData.SocketConfig.PowerManagementConfig.TurboRatioLimitRatio[i] > 0) {
          TurboRatioLimitRatioCores->RatioLimitRatioMask[i] = 0;
        }

        TurboRatioLimitRatioCores->RatioLimitCoresMask[i] = 0xFF;
        TurboRatioLimitRatioCores->RatioLimitCores[i] = 0;
        if (SetupData.SocketConfig.PowerManagementConfig.TurboRatioLimitCores[i] != 0xFF) {
          TurboRatioLimitRatioCores->RatioLimitCoresMask[i] = 0;
          TurboRatioLimitRatioCores->RatioLimitCores[i] = SetupData.SocketConfig.PowerManagementConfig.TurboRatioLimitCores[i];
        }
      }
    }

#if SmcOptimize_SUPPORT
#if SMC_OPTIMIZE_CPU2006
	if ( SetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS == SMC_OPTIMIZE_CPU2006 ) {
		SetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS = 0;
	}
#endif
#if SMC_OPTIMIZE_SPECPOWER
	if ( SetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS == SMC_OPTIMIZE_SPECPOWER ) {
		SetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS = 7;
	}
#endif
#endif

    MsrPerfBiasConfig = ( SetupData.SocketConfig.PowerManagementConfig.EngAvgTimeWdw1 << AVG_TIME_Window_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.P0TtlTimeLow1 << PO_TOTAL_TIME_THSHLD_LOW_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.P0TtlTimeHigh1 << PO_TOTAL_TIME_THSHLD_HIGH_SHIFT ) |
      ( SetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS << ALT_ENERGY_PERF_BIAS_SHIFT) |
      ( SetupData.SocketConfig.PowerManagementConfig.WorkLdConfig << WORKLD_CONFIG_SHIFT );

    ppm->PerfBiasConfig.Dwords.Low = MsrPerfBiasConfig;

    //
    //ProcessorHWPM-init as disabled.
    //
    ppm->Hwpm.HWPMNative = 0;
    ppm->Hwpm.HWPMOOB = 0;
    ppm->Hwpm.HWPMEnable = SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMEnable;
    ppm->Hwpm.AutoCState = SetupData.SocketConfig.PowerManagementConfig.ProcessorAutonomousCstateEnable;
    ppm->Hwpm.HWPMInterrupt = SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMInterrupt;
    ppm->Hwpm.EPPEnable = SetupData.SocketConfig.PowerManagementConfig.ProcessorEPPEnable;
    ppm->Hwpm.EPPProfile = SetupData.SocketConfig.PowerManagementConfig.ProcessorEppProfile;

    if ((SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMEnable == 1) || 
       (SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMEnable == 3)) {
      ppm->Hwpm.HWPMNative = SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMEnable;
    }else if (SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMEnable == 2){
      ppm->Hwpm.HWPMOOB = SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMEnable;
      ppm->Hwpm.HWPMInterrupt = 0;
    }else if (SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMEnable == 0){
      ppm->Hwpm.HWPMNative = 0;
      ppm->Hwpm.HWPMOOB = 0;
      ppm->Hwpm.HWPMInterrupt = 0;
      ppm->Hwpm.EPPEnable = 0;
    }

    ppm->Hwpm.APSrocketing = SetupData.SocketConfig.PowerManagementConfig.ProcessorAPSrocketing;
    ppm->Hwpm.Scalability = SetupData.SocketConfig.PowerManagementConfig.ProcessorScalability;
    ppm->Hwpm.PPOBudget = SetupData.SocketConfig.PowerManagementConfig.ProcessorPPOBudget;
    ppm->Hwpm.OutofBandAlternateEPB = SetupData.SocketConfig.PowerManagementConfig.ProcessorOutofBandAlternateEPB;

    if(SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorX2apic && SetupData.SocketConfig.SocketProcessorCoreConfiguration.ForceX2ApicIds &&
       (CpuPolicyEx1 & PCD_CPU_X2APIC_BIT)) { //if user want to reprogram > 8bit ApicId (must be X2Apic too)
      CheckAndReAssignSocketId();
    }

    for(i = 0; i < NUM_CST_LAT_MSR; i++) {    //3 CStateLatencyCtrl CSRs
      ppm->Cst.LatencyCtrl[i].Dwords.Low = ( SetupData.SocketConfig.PowerManagementConfig.CStateLatencyCtrlValid[i] << VALID_SHIFT ) |
                             ( SetupData.SocketConfig.PowerManagementConfig.CStateLatencyCtrlMultiplier[i] << MULTIPLIER_SHIFT ) |
                             ( SetupData.SocketConfig.PowerManagementConfig.CStateLatencyCtrlValue[i] << VALUE_SHIFT );
    }

    if(SetupData.SocketConfig.PowerManagementConfig.C2C3TT) { //if option is not AUTO
      ppm->C2C3TT = (UINT32)SetupData.SocketConfig.PowerManagementConfig.C2C3TT;
    } else {
      ppm->C2C3TT = 0x10;
    }

  } //end - else

  CpuPolicy |= PCD_CPU_L3_CACHE_BIT;

  //
  // If Emulation flag set by InitializeDefaultData in MemoryQpiInit.c
  //  force X2APIC
  //
  Status = gBS->LocateProtocol (
                  &gEfiPlatformTypeProtocolGuid,
                  NULL,
                  &PlatformType
                  );
  ASSERT_EFI_ERROR (Status);

  Status = PcdSet32S (PcdCpuProcessorFeatureUserConfiguration, CpuPolicy);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return Status;
  Status = PcdSet32S (PcdCpuProcessorFeatureUserConfigurationEx1, CpuPolicyEx1);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return Status;
  if (SetupData.SystemConfig.McBankWarmBootClearError == 1) {
     Status = PcdSetBoolS (PcdIsPowerOnReset, TRUE);
     ASSERT_EFI_ERROR (Status);
     if (EFI_ERROR(Status)) return Status;
  } else {
     Status = PcdSetBoolS (PcdIsPowerOnReset, FALSE);
     ASSERT_EFI_ERROR (Status);
     if (EFI_ERROR(Status)) return Status;
  }

  // allocate memory for IedTrace
  if((SetupData.SocketConfig.SocketProcessorCoreConfiguration.IedTraceSize != 0) && (SetupData.SocketConfig.SocketProcessorCoreConfiguration.IedSize != 0)) {
    CheckAndPopulateIedTraceMemory(0x400000 << (SetupData.SocketConfig.SocketProcessorCoreConfiguration.IedTraceSize - 1));
  }
 
  //
  // Cpu Driver could be dispatched after this protocol installed.
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gIntelCpuPcdsSetDoneProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**

   Drop into SMM to register IOTRAP for BIOS Guard tools interface

   @param Event     - A pointer to the Event that triggered the callback.
   @param Context   - A pointer to private data registered with the callback function.

   @retval None

**/
VOID
InitializeBiosGuardToolsIntCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_PHYSICAL_ADDRESS BaseAddress;
  BIOSGUARD_HOB        *BiosGuardHobPtr;
  EFI_GUID             BiosGuardHobGuid = gBiosGuardHobGuid;

  BiosGuardHobPtr = GetFirstGuidHob (&BiosGuardHobGuid);
  if (BiosGuardHobPtr != NULL) {
    BaseAddress = (EFI_PHYSICAL_ADDRESS) BiosGuardHobPtr->BiosGuardToolsIntIoTrapAdd;
    ///
    /// IOTRAP TO SMM
    ///
    IoRead8 (BaseAddress);
  }
  return;
}
