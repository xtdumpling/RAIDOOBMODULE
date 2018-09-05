/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/**
Copyright (c) 1996 - 2016, Intel Corporation.

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


  @file AcpiPlatformHooks.c

  ACPI Platform Driver Hooks

**/

//
// Statements that include other files
//
#include "AcpiPlatform.h"
#include "AcpiPlatformHooks.h"
#include "MemWeight.h"
#include "RcRegs.h"
// APTIOV_SERVER_OVERRIDE_RC_START : Use OEM functions from AmiPlatformHooksLib.h 
#include <Library/AmiPlatformHooksLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Use OEM functions from AmiPlatformHooksLib.h 

#pragma optimize("",off)

extern BOOLEAN                        mForceX2ApicId;
extern BOOLEAN                        mX2ApicEnabled;
extern UINT32                         mNumOfBitShift;
//extern struct SystemMemoryMapHob      *mSystemMemoryMap;
extern EFI_PLATFORM_INFO              *mPlatformInfo;
extern BIOS_ACPI_PARAM                *mAcpiParameter;
extern EFI_GUID gEfiMemoryMapGuid;
extern EFI_IIO_UDS_PROTOCOL           *mIioUds;             
extern EFI_CPU_CSR_ACCESS_PROTOCOL *mCpuCsrAccess;

extern SOCKET_MEMORY_CONFIGURATION    mSocketMemoryConfiguration;
extern SOCKET_MP_LINK_CONFIGURATION   mSocketMpLinkConfiguration;
extern SOCKET_COMMONRC_CONFIGURATION  mSocketCommonRcConfiguration;
extern SOCKET_IIO_CONFIGURATION       mSocketIioConfiguration;
extern SOCKET_POWERMANAGEMENT_CONFIGURATION mSocketPowermanagementConfiguration;
extern SOCKET_PROCESSORCORE_CONFIGURATION   mSocketProcessorCoreConfiguration;
extern SYSTEM_CONFIGURATION  mSystemConfiguration;
extern PCH_RC_CONFIGURATION  mPchRcConfiguration;

EFI_GLOBAL_NVS_AREA_PROTOCOL        mGlobalNvsArea;


/**

    GC_TODO: add routine description

    @param None

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
PlatformHookInit (
  VOID
  )
{
  EFI_STATUS                    Status;
 // EFI_HOB_GUID_TYPE             *GuidHob;
  EFI_HANDLE                    Handle;
  EFI_PHYSICAL_ADDRESS          AcpiParameterAddr;
  EFI_CPUID_REGISTER            CpuidRegisters;
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                    SetupData;
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error

/**
  GuidHob    = GetFirstGuidHob (&gEfiMemoryMapGuid);
  ASSERT (GuidHob != NULL); 
  if (GuidHob == NULL) { 
    return EFI_NOT_FOUND;
  }
  mSystemMemoryMap = GET_GUID_HOB_DATA (GuidHob); 
**/
  Status = GetEntireConfig (&SetupData);

  CopyMem (&mSocketMemoryConfiguration, &(SetupData.SocketConfig.MemoryConfig), sizeof(SOCKET_MEMORY_CONFIGURATION));
  CopyMem (&mSocketMpLinkConfiguration, &(SetupData.SocketConfig.CsiConfig), sizeof(SOCKET_MP_LINK_CONFIGURATION));
  CopyMem (&mSocketCommonRcConfiguration, &(SetupData.SocketConfig.CommonRcConfig), sizeof(SOCKET_COMMONRC_CONFIGURATION));
  CopyMem (&mSocketPowermanagementConfiguration, &(SetupData.SocketConfig.PowerManagementConfig), sizeof(SOCKET_POWERMANAGEMENT_CONFIGURATION));
  CopyMem (&mSocketProcessorCoreConfiguration, &(SetupData.SocketConfig.SocketProcessorCoreConfiguration), sizeof(SOCKET_PROCESSORCORE_CONFIGURATION));
  CopyMem (&mSystemConfiguration, &(SetupData.SystemConfig), sizeof(SYSTEM_CONFIGURATION));
  CopyMem (&mSocketIioConfiguration, &(SetupData.SocketConfig.IioConfig), sizeof(SOCKET_IIO_CONFIGURATION));
  CopyMem (&mPchRcConfiguration, &(SetupData.PchRcConfig), sizeof(PCH_RC_CONFIGURATION));

  if (EFI_ERROR (Status)) {
    mSocketPowermanagementConfiguration.ProcessorEistEnable = 0;
    mSocketPowermanagementConfiguration.TurboMode           = 0;
    mSocketPowermanagementConfiguration.ProcessorAutonomousCstateEnable  = 1;
    mSocketPowermanagementConfiguration.PackageCState       = 0;
    mSocketPowermanagementConfiguration.PwrPerfTuning       = 1;
    mSocketProcessorCoreConfiguration.ProcessorX2apic         = 0;
    mSocketProcessorCoreConfiguration.ForcePhysicalModeEnable = 0;
  }

  //
  // If Emulation flag set by InitializeDefaultData in ProcMemInit.c
  //  force X2APIC
  // Else read setup data
  //
  mX2ApicEnabled = mSocketProcessorCoreConfiguration.ProcessorX2apic;
  if(mX2ApicEnabled) {
    mForceX2ApicId = mSocketProcessorCoreConfiguration.ForceX2ApicIds;
  } else {
    mForceX2ApicId = FALSE;
  }

  //
  // Allocate 256 runtime memory to pass ACPI parameter
  // This Address must be < 4G because we only have 32bit in the dsdt
  //
  AcpiParameterAddr = 0xffffffff;
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiACPIMemoryNVS,
                  EFI_SIZE_TO_PAGES (sizeof(BIOS_ACPI_PARAM)),
                  &AcpiParameterAddr
                  );
  ASSERT_EFI_ERROR (Status);
  mAcpiParameter = (BIOS_ACPI_PARAM *)AcpiParameterAddr;

  //DEBUG ((EFI_D_ERROR, "ACPI Parameter Block Address: 0x%X\n", mAcpiParameter));

  ZeroMem (mAcpiParameter, sizeof (BIOS_ACPI_PARAM));
  mAcpiParameter->PlatformId    = (UINT32)mPlatformInfo->BoardId;
  mAcpiParameter->IoApicEnable  = mPlatformInfo->SysData.SysIoApicEnable;

  Handle = NULL;
  mGlobalNvsArea.Area = mAcpiParameter;
  // APTIOV_SERVER_OVERRIDE_RC_START : Assign result to Status
  Status = gBS->InstallProtocolInterface ( 
  // APTIOV_SERVER_OVERRIDE_RC_END : Assign result to Status
         &Handle,
         &gEfiGlobalNvsAreaProtocolGuid,
         EFI_NATIVE_INTERFACE,
         &mGlobalNvsArea
         );
  ASSERT_EFI_ERROR (Status);

  AsmCpuid (EFI_CPUID_VERSION_INFO,  &CpuidRegisters.RegEax, &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);
  mAcpiParameter->ProcessorId = (CpuidRegisters.RegEax & 0xFFFF0);

  // support up to 64 threads/socket
  AsmCpuidEx(EFI_CPUID_CORE_TOPOLOGY, 1, &mNumOfBitShift, NULL, NULL, NULL);
  mNumOfBitShift &= 0x1F;

  // Set the bit shift value for CPU SKU
  mAcpiParameter->CpuSkuNumOfBitShift = (UINT8) mNumOfBitShift;

  mAcpiParameter->ProcessorApicIdBase[0] = (UINT32) (0 << mNumOfBitShift);
  mAcpiParameter->ProcessorApicIdBase[1] = (UINT32) (1 << mNumOfBitShift);
  mAcpiParameter->ProcessorApicIdBase[2] = (UINT32) (2 << mNumOfBitShift);
  mAcpiParameter->ProcessorApicIdBase[3] = (UINT32) (3 << mNumOfBitShift);
  mAcpiParameter->ProcessorApicIdBase[4] = (UINT32) (4 << mNumOfBitShift);
  mAcpiParameter->ProcessorApicIdBase[5] = (UINT32) (5 << mNumOfBitShift);
  mAcpiParameter->ProcessorApicIdBase[6] = (UINT32) (6 << mNumOfBitShift);
  mAcpiParameter->ProcessorApicIdBase[7] = (UINT32) (7 << mNumOfBitShift);

  if(mForceX2ApicId) {
    mAcpiParameter->ProcessorApicIdBase[0] = 0x7F00;
    mAcpiParameter->ProcessorApicIdBase[1] = 0x7F20;
    mAcpiParameter->ProcessorApicIdBase[2] = 0x7F40;
    mAcpiParameter->ProcessorApicIdBase[3] = 0x7F60;
    mAcpiParameter->ProcessorApicIdBase[4] = 0x7F80;
    mAcpiParameter->ProcessorApicIdBase[5] = 0x7Fa0;
    mAcpiParameter->ProcessorApicIdBase[6] = 0x7Fc0;
    mAcpiParameter->ProcessorApicIdBase[7] = 0x7Fe0;

     if (mNumOfBitShift == 4) {
      mAcpiParameter->ProcessorApicIdBase[0] = 0x7F00;
      mAcpiParameter->ProcessorApicIdBase[1] = 0x7F10;
      mAcpiParameter->ProcessorApicIdBase[2] = 0x7F20;
      mAcpiParameter->ProcessorApicIdBase[3] = 0x7F30;
      mAcpiParameter->ProcessorApicIdBase[4] = 0x7F40;
      mAcpiParameter->ProcessorApicIdBase[5] = 0x7F50;
      mAcpiParameter->ProcessorApicIdBase[6] = 0x7F60;
      mAcpiParameter->ProcessorApicIdBase[7] = 0x7F70;
    } else if(mNumOfBitShift == 6) {
      mAcpiParameter->ProcessorApicIdBase[0] = 0x7E00;
      mAcpiParameter->ProcessorApicIdBase[1] = 0x7E20;
      mAcpiParameter->ProcessorApicIdBase[2] = 0x7E40;
      mAcpiParameter->ProcessorApicIdBase[3] = 0x7E60;
      mAcpiParameter->ProcessorApicIdBase[4] = 0x7E80;
      mAcpiParameter->ProcessorApicIdBase[5] = 0x7Ea0;
      mAcpiParameter->ProcessorApicIdBase[6] = 0x7Ec0;
      mAcpiParameter->ProcessorApicIdBase[7] = 0x7Ee0;
    }
  }

  //
  // If SNC is enabled, and NumOfCluster is 2, set the ACPI variable for PXM value
  //
  if(mIioUds->IioUdsPtr->SystemStatus.OutSncEn && (mIioUds->IioUdsPtr->SystemStatus.OutNumOfCluster == 2)){
    mAcpiParameter->SncAnd2Cluster = 1;
  } else {
    mAcpiParameter->SncAnd2Cluster = 0;
  }

   mAcpiParameter->MmCfg = (UINT32)mIioUds->IioUdsPtr->PlatformData.PciExpressBase;
   mAcpiParameter->TsegSize = (UINT32)(mIioUds->IioUdsPtr->PlatformData.MemTsegSize >> 20);

  return EFI_SUCCESS;
}


/**
  Install Xhci ACPI Table

**/
VOID
InstallXhciAcpiTable (
  VOID
  )
{
  EFI_HANDLE                    *HandleBuffer;
  EFI_STATUS                    Status;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINT32                        FvStatus;
  UINTN                         Size;
  UINTN                         TableHandle;
  INTN                          Instance;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINT64                        XhciAcpiTable;
  UINT64                        TempOemTableId;
  SYSTEM_BOARD_PROTOCOL         *SystemBoard;

  HandleBuffer  = 0;
  Instance      = 0;
  TableHandle   = 0;
  CurrentTable  = NULL;
  FwVol         = NULL;
  XhciAcpiTable = 0;

  DEBUG ((DEBUG_INFO, "InstallXhciAcpiTable\n"));

  Status = gBS->LocateProtocol (&gEfiDxeSystemBoardProtocolGuid, NULL, (VOID**)&SystemBoard); 
  ASSERT_EFI_ERROR (Status); 

  if (SystemBoard->UpdateOemTableIdXhci(&XhciAcpiTable) == FALSE) {
    DEBUG ((DEBUG_INFO, "No XHCI table for current platform\n"));
  }
  // APTIOV_SERVER_OVERRIDE_RC_START : Oem Hook to Override XhciAcpiTable
  OemInstallXhciAcpiTableOverride((VOID *) mPlatformInfo, &XhciAcpiTable);
  // APTIOV_SERVER_OVERRIDE_RC_END : Oem Hook to Override XhciAcpiTable

  //
  // Find the AcpiSupport protocol
  //
  Status = LocateSupportProtocol (
            &gEfiAcpiTableProtocolGuid,
            gEfiAcpiTableStorageGuid,
            &AcpiTable,
            FALSE
            );

  ASSERT_EFI_ERROR (Status);

  //
  // Locate the firmware volume protocol
  //
  Status = LocateSupportProtocol (
            &gEfiFirmwareVolume2ProtocolGuid,
            gEfiAcpiTableStorageGuid,
            &FwVol,
            TRUE
            );
  
  ASSERT_EFI_ERROR (Status);

  //
  // Read tables from the storage file.
  //
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gEfiAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      &CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {

      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;

      if (TableHeader->OemTableId == XhciAcpiTable) {
        DEBUG ((DEBUG_INFO, "Install xhci table: %x\n", TableHeader->OemTableId));

        TempOemTableId  = PcdGet64(PcdAcpiDefaultOemTableId);

        CopyMem (TableHeader->OemId, PcdGetPtr(PcdAcpiDefaultOemId), sizeof(TableHeader->OemId));
        CopyMem (&TableHeader->OemTableId, &TempOemTableId, sizeof(TableHeader->OemTableId));

        TableHeader->CreatorId = EFI_ACPI_CREATOR_ID;
        TableHeader->CreatorRevision = EFI_ACPI_CREATOR_REVISION;
    
        //
        // Add the table
        //
        TableHandle = 0;

        Status = AcpiTable->InstallAcpiTable (
                                    AcpiTable,
                                    CurrentTable,
                                    CurrentTable->Length,
                                    &TableHandle
                                    );
        break;
      }

      //
      // Increment the instance
      //

      Instance++;
      CurrentTable = NULL;
    }
  }
}

UINT8 
EFIAPI
DetectHwpFeature (
   VOID 
  )
{
  UINT8  HwpSupport = 1;
  UINT8  socket;
  CAPID5_PCU_FUN3_STRUCT          Capid5;
  UINT64 MsrValue;

  for (socket = 0; socket < MAX_SOCKET; socket++) {
    if (mIioUds->IioUdsPtr->SystemStatus.socketPresentBitMap & (1 << socket)) {
      Capid5.Data = mCpuCsrAccess->ReadCpuCsr(socket, 0, CAPID5_PCU_FUN3_REG);
      HwpSupport &= Capid5.Bits.hwp_en;
    }
  }

  if (!HwpSupport) {
    return HwpSupport;
  }

  MsrValue = AsmReadMsr64 (MSR_MISC_PWR_MGMT); 
  HwpSupport = 0;

  if (MsrValue && B_MSR_ENABLE_HWP) {
    HwpSupport = 1;
  } else if (MsrValue && B_MSR_ENABLE_HWP_AUTONOMOUS) {
    HwpSupport = 2;
  }

  if (HwpSupport > 0)
  {
    return (mSocketPowermanagementConfiguration.ProcessorHWPMEnable);
  }
  return HwpSupport;
}
