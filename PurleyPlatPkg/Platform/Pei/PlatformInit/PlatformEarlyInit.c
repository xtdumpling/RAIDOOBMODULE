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


  @file PlatformEarlyInit.c

  Do platform specific stage1 initializations.

**/
// APTIOV_SERVER_OVERRIDE_RC_START
#include <Token.h>
// APTIOV_SERVER_OVERRIDE_RC_END
#include "PlatformEarlyInit.h"

#include <Ppi/BaseMemoryTest.h>
#include <Ppi/FlashMap.h>
#include <Ppi/SmbusPolicy.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/ReadOnlyVariable2.h>
// APTIOV_SERVER_OVERRIDE_RC_START
//#include <Library/CmosAccessLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END
#include <Library/PeiPolicyInitLib.h>
#include <Library/PeiPlatformHooklib.h>
#include <FirwmareConfigurations.h>
#include <Guid/GlobalVariable.h>
#include <Library/TimerLib.h>
#include <Register/PchRegsPmc.h>
#include <Library/PchCycleDecodingLib.h>
#include <Register/PchRegsLpc.h>
#include <Ppi/PchReset.h>
#include <Ppi/Spi.h>
#include <Guid/FirmwareFileSystem3.h>
#include <Guid/TcgDataHob.h>
#include <Guid/AdminPasswordGuid.h>
#include <Guid/SetupBreakpointGuid.h>
#include <Guid/SetupVariable.h>
#include <Guid/PchRcVariable.h>
#include <Guid/SocketVariable.h>
#if defined(AMT_SUPPORT) && AMT_SUPPORT
#include "Library/MeTypeLib.h"
#endif //AMT_SUPPORT

extern EFI_GUID gEfiBiosIdGuid;
extern EFI_GUID gEfiSiliconRcHobsReadyPpi;
EFI_GUID  gEfiAfterMrcGuid            = EFI_AFTER_MRC_GUID;
EFI_GUID  gEfiAfterQpircGuid          = EFI_AFTER_QPIRC_GUID;
EFI_GUID  gEfiAfterFullSpeedSetupGuid = EFI_AFTER_FULL_SPEED_SETUP_GUID;
EFI_GUID  gEfiReadyForIbistGuid       = EFI_READY_FOR_IBIST_GUID;

static PEI_BASE_MEMORY_TEST_PPI     mPeiBaseMemoryTestPpi = { BaseMemoryTest };

static PEI_PLATFORM_MEMORY_SIZE_PPI mMemoryMemorySizePpi  = { GetPlatformMemorySize };

static EFI_PEI_STALL_PPI                mStallPpi = {
  PEI_STALL_RESOLUTION,
  Stall
};
// APTIOV_SERVER_OVERRIDE_RC_START : Added PchResetPlatform() to call PchResetPpi->Reset
static EFI_PEI_RESET_PPI                mResetPpi = { PchResetPlatform };
// APTIOV_SERVER_OVERRIDE_RC_END : Added PchResetPlatform() to call PchResetPpi->Reset

static UINT8                        mSmbusRsvdAddresses[PLATFORM_NUM_SMBUS_RSVD_ADDRESSES] = {
  SMBUS_ADDR_CH_A_1,
  SMBUS_ADDR_CH_A_2,
  SMBUS_ADDR_CH_B_1,
  SMBUS_ADDR_CH_B_2,
};

static PEI_SMBUS_POLICY_PPI         mSmbusPolicyPpi = {
  PCH_SMBUS_BASE_ADDRESS,
  PCH_SMBUS_BUS_DEV_FUNC,
  PLATFORM_NUM_SMBUS_RSVD_ADDRESSES,
  mSmbusRsvdAddresses
};

static EFI_PEI_PPI_DESCRIPTOR       mInstallStallPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gEfiPeiStallPpiGuid,
  &mStallPpi
};

static EFI_PEI_PPI_DESCRIPTOR       mPpiList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gEfiPeiResetPpiGuid,
    &mResetPpi
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gPeiBaseMemoryTestPpiGuid,
    &mPeiBaseMemoryTestPpi
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gPeiPlatformMemorySizePpiGuid,
    &mMemoryMemorySizePpi
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gPeiSmbusPolicyPpiGuid,
    &mSmbusPolicyPpi
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMasterBootModePpiGuid,
    NULL
  }
};

static EFI_PEI_NOTIFY_DESCRIPTOR    mNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiEndOfPeiSignalPpiGuid,
    EndOfPeiPpiNotifyCallback
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiSmbus2PpiGuid,
    ConfigurePlatformClocks
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiSmbus2PpiGuid,
    PeiPrintPlatformInfo
  },
  { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiAfterMrcGuid,
    AfterMrcBreakpoint
  },
  { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiAfterQpircGuid,
    AfterQpircBreakpoint
  },
  { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiAfterFullSpeedSetupGuid,
    AfterFullSpeedSetupBreakpoint
  },
  { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiReadyForIbistGuid,
    ReadyForIbistBreakpoint
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiSiliconRcHobsReadyPpi,
    SiliconRcHobsReadyPpiNotifyCallback
  }
};

#ifndef PC_HOOK

// APTIOV_SERVER_OVERRIDE_RC_START : Removing FV not present.
#if 0
static EFI_PEI_FIRMWARE_VOLUME_INFO_PPI mFvMrcNormalInfoPpi = {
  EFI_FIRMWARE_FILE_SYSTEM3_GUID,
  (VOID*)FLASH_REGION_FV_MRC_NORMAL_BASE,
  FLASH_REGION_FV_MRC_NORMAL_SIZE,
  NULL,
  NULL
};

static EFI_PEI_PPI_DESCRIPTOR     mPpiListFvMrcNormal = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiFirmwareVolumeInfoPpiGuid,
  &mFvMrcNormalInfoPpi
};
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Removing FV not present.
#endif

/**

  Print platform information such as Bios ID and platform Type in PEI.

  @param **PeiServices - Pointer to the list of PEI services
  @param *NotifyDescriptor - The notification structure this PEIM registered on install
  @param *Ppi - Pointer to the PPI

  @retval EFI_SUCCESS -  Function has completed successfully.


**/
EFI_STATUS
PeiPrintPlatformInfo (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *Ppi
  )
{
  EFI_STATUS                  Status;
  EFI_PLATFORM_INFO           *PlatformInfo;
  EFI_HOB_GUID_TYPE           *GuidHob;
  BIOS_ID_IMAGE               BiosIdImage;
  //
  // Search for the Platform Info PEIM GUID HOB.
  //
  GuidHob = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfo = GET_GUID_HOB_DATA(GuidHob);

  DEBUG ((EFI_D_ERROR, "Platform Type = %d\n", PlatformInfo->BoardId));

  Status = PeiGetBiosId(PeiServices, &BiosIdImage);

  if (!EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "Bios ID: %s\n", (CHAR16 *) (&(BiosIdImage.BiosIdString))));
  }

  return EFI_SUCCESS;

}

/**

  Configure the clock generator to enable free-running operation.  This keeps
  the clocks from being stopped when the system enters C3 or C4.

  @param None

  @retval EFI_SUCCESS    The function completed successfully.

**/
EFI_STATUS
ConfigurePlatformClocks (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *SmbusPpi
  )
{
  EFI_STATUS                            Status;
  SYSTEM_BOARD_PPI                      *SystemBoard;

  //
  // Get SystemBoard PPI
  //
  (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiSystemBoardPpiGuid,
                  0,
                  NULL,
                  &SystemBoard
                  );

  Status = SystemBoard->ConfigurePlatformClocks(PeiServices, NotifyDescriptor, SmbusPpi);
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to Configure Platform Clocks\n"));
  }
  return EFI_SUCCESS;
}

/**

  Find the BIOS ID from the BIOS ID binary included in the flash

  @param **PeiServices - Pointer to the list of PEI services
  @param *BiosIdImage  - Pointer to the BIOS ID structure being populated

  @retval EFI_NOT_FOUND - Failed to find the Bios ID binary.
  @retval EFI_SUCCESS   - Found the BIOS ID binary.


**/
EFI_STATUS
PeiGetBiosId (
  IN EFI_PEI_SERVICES       **PeiServices,
  IN OUT  BIOS_ID_IMAGE     *BiosIdImage
  )
{
  EFI_STATUS                  Status;
  VOID                        *Address;
  UINTN                       Size;
  EFI_PEI_FV_HANDLE           VolumeHandle;
  EFI_PEI_FILE_HANDLE         FileHandle;
  EFI_FFS_FILE_HEADER         *FileHeader;
  UINTN                       Instance          = 0;
  BOOLEAN                     BiosIdBinaryFound = FALSE;

  //
  // Find BiosId Binary - First loop for Volume Handle
  //
  while (TRUE) {
    VolumeHandle = NULL;
    Status = (*PeiServices)->FfsFindNextVolume (PeiServices, Instance++, &VolumeHandle);

    if (EFI_ERROR(Status)) {
      return Status;
    }

    FileHandle = NULL;
    //
    // Second loop for FileHandle
    //
    while (TRUE) {
      Status = (*PeiServices)->FfsFindNextFile (PeiServices, EFI_FV_FILETYPE_FREEFORM, VolumeHandle, &FileHandle);
      // According to PI spec, FileHandle is FileHeader.
      FileHeader = (EFI_FFS_FILE_HEADER *)FileHandle;
      if (EFI_ERROR (Status)){
        break;
      }
      //
      // Compare GUID to find BIOS ID FFS
      //
      if (CompareGuid (&(FileHeader->Name), &gEfiBiosIdGuid)) {
        BiosIdBinaryFound = TRUE;
        break;
      }
    }//end of second loop
    //
    //  If the binary was found, break Volume loop
    //
    if(BiosIdBinaryFound)
      break;
  }//end of first loop

  Status = (*PeiServices)->FfsFindSectionData (
            PeiServices,
            EFI_SECTION_RAW,
            FileHandle,
            &Address
            );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Size = sizeof (BIOS_ID_IMAGE);

  (*PeiServices)->CopyMem ((VOID *) BiosIdImage, Address, Size);

  return EFI_SUCCESS;

}

#ifdef LT_FLAG
/**

  Finds the address of a FIT Record in the bootblock region of bios.

  @param FITRecordAddressInFit = The address of the FIT Record (in the FIT table) if one exists, or 0 otherwise.
  @param FITRecordAddress      = The address of the FIT REcord if one exists, or 0 otherwise.

  @retval Nothing.

**/
BOOLEAN
FindFITRecord (
  IN    UINT8          Type,
  OUT   UINTN          *FITRecordAddressInFit,
  OUT   UINTN          *FITRecordAddress
)
{
  UINTN     *FITAddressPtr;
  UINT8     *FITAddress;
  UINT16     NumberOfFitEntries;
  BOOLEAN   OkAddress;
  UINTN     FITAddressTmp;
  BOOLEAN   FindValidFit;

  OkAddress = FALSE;
  FindValidFit = FALSE;

  FITAddressPtr = (UINTN *) 0xFFFFFFC0;      // first try the defualt address for bios that supports only WSM CPU family
  FITAddress = (UINT8 *) (* FITAddressPtr);
  if (FITAddress == (UINT8 *)0xFFFFFFFF) { // Bad Fit pointer - do not try to dereference
    *FITRecordAddressInFit = (UINTN) 0;
    *FITRecordAddress = (UINTN) 0;
    return FindValidFit;
  }


  if (  ((FITAddress[0])=='_') && ((FITAddress[1])=='F') && ((FITAddress[2])=='I') && ((FITAddress[3])=='T') && ((FITAddress[4])=='_')  ) {
      OkAddress = TRUE;
    }

  if (!OkAddress) {
    FITAddressPtr = (UINTN *) 0xFFFFFFE8;      // now try the defualt address for bios that supports only the special NHM C0 fused CPU
    FITAddress = (UINT8 *) (* FITAddressPtr);
    if (  ((FITAddress[0])=='_') && ((FITAddress[1])=='F') && ((FITAddress[2])=='I') && ((FITAddress[3])=='T') && ((FITAddress[4])=='_')  ) {
        OkAddress = TRUE;
      }
    }

  if (OkAddress) {
    OkAddress = FALSE;
    NumberOfFitEntries = (UINT16) FITAddress[8];
    while (NumberOfFitEntries) {
      if ( FITAddress[0x0E] == Type ) {
        FITAddressTmp = (UINTN) FITAddress[3];
        FITAddressTmp = FITAddressTmp << 8;
        FITAddressTmp |= (UINTN) FITAddress[2];
        FITAddressTmp = FITAddressTmp << 8;
        FITAddressTmp |= (UINTN) FITAddress[1];
        FITAddressTmp = FITAddressTmp << 8;
        FITAddressTmp |= (UINTN) FITAddress[0];
        FITAddressTmp =  FITAddressTmp;
        OkAddress = TRUE;
        FindValidFit = TRUE;
        FITAddressPtr = (UINTN *) FITAddressTmp;
        break;
      }
      FITAddress += 0x10;               // move to the next entry in the FIT table
      NumberOfFitEntries --;
    }
  }

  if (!OkAddress) {
    // specified FIT entry type not found in FIT table
    FITAddress = 0;
    FITAddressPtr = 0;
  }

  *FITRecordAddressInFit = (UINTN) FITAddress;
  *FITRecordAddress = (UINTN) FITAddressPtr;

  return FindValidFit;
}
#endif


/**
  Set the state to go after G3

  @param[in]  UINT8 StateAfterG3
**/
VOID
SetTheStateToGoAfterG3 (
  IN UINT8 StateAfterG3
  )
{
  UINT8                 DataUint8;
  UINTN                 PmcBaseAddress;

  //
  // Check for PowerState option for AC power loss and program the chipset
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Keep S0 as default state since Unchanged state is handled in AcpiSmmPlatform.c{
  //if (StateAfterG3 != LEAVE_POWER_STATE_UNCHANGED) 
  // APTIOV_SERVER_OVERRIDE_RC_END : Keep S0 as default state since Unchanged state is handled in AcpiSmmPlatform.c{
    PmcBaseAddress = MmPciBase (
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_PMC,
                       PCI_FUNCTION_NUMBER_PCH_PMC
                       );
    DataUint8 = MmioRead8 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B);
    if (StateAfterG3 == 1) {
      DataUint8 |= B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN; // AfterG3 = S5
    } else {
      DataUint8 &= (UINT8)~B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN; // AfterG3 = S0
    }
    MmioWrite8 (PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B, DataUint8);
  // APTIOV_SERVER_OVERRIDE_RC_START : Keep S0 as default state since Unchanged state is handled in AcpiSmmPlatform.c{
  //}
  // APTIOV_SERVER_OVERRIDE_RC_END : Keep S0 as default state since Unchanged state is handled in AcpiSmmPlatform.c{
}

/**

  Initialize POC register by Variable.

  @param *SystemConfiguration  -  Pointer to SystemConfiguration variables.

  @retval EFI_SUCCESS  -  Success.

**/
EFI_STATUS
UpdatePlatformInfo (
  IN   SYSTEM_CONFIGURATION               *SystemConfiguration,
  IN   SOCKET_CONFIGURATION               *SocketConfiguration
  )
{
  EFI_PLATFORM_INFO *PlatformInfo;
  EFI_HOB_GUID_TYPE *GuidHob;
  SOCKET_PROCESSORCORE_CONFIGURATION *SocketProcessorCoreConfig;
  SOCKET_IIO_CONFIGURATION *SocketIioConfig;

  SocketProcessorCoreConfig = &SocketConfiguration->SocketProcessorCoreConfiguration;
  SocketIioConfig = &SocketConfiguration->IioConfig;

  //
  // Update the PCIE base and 32/64bit PCI resource support
  //
  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfo  = GET_GUID_HOB_DATA(GuidHob);

  PlatformInfo->SysData.SysIoApicEnable       = PCH_IOAPIC;
#if MAX_SOCKET <= 4
  if (SocketIioConfig->DevPresIoApicIio[0]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC00_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[1]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC01_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[2]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC02_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[3]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC03_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[4]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC04_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[5]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC05_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[6]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC06_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[7]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC07_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[8]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC08_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[9]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC09_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[10]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC10_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[11]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC11_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[12]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC12_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[13]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC13_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[14]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC14_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[15]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC15_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[16]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC16_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[17]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC17_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[18]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC18_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[19]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC19_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[20]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC20_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[21]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC21_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[22]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC22_IOAPIC);
  }
  if (SocketIioConfig->DevPresIoApicIio[23]) {
    PlatformInfo->SysData.SysIoApicEnable |= (PC23_IOAPIC);
  }
#else
  // Enable all 32 IOxAPIC
  PlatformInfo->SysData.SysIoApicEnable = 0xFFFFFFFF;
#endif
  //
  // Check to make sure TsegSize is in range, if not use default.
  //
  if (SocketProcessorCoreConfig->TsegSize > MAX_PROCESSOR_TSEG) {
    SocketProcessorCoreConfig->TsegSize = MAX_PROCESSOR_TSEG; // if out of range make default 64M
  }
  PlatformInfo->MemData.MemTsegSize  = (0x400000 << SocketProcessorCoreConfig->TsegSize);
  if (SocketProcessorCoreConfig->IedSize > 0) {
    PlatformInfo->MemData.MemIedSize = (0x400000 << (SocketProcessorCoreConfig->IedSize - 1));
  } else {
    PlatformInfo->MemData.MemIedSize = 0;
  }

  //
  // Minimum SMM range in TSEG should be larger than 3M
  //
  ASSERT (PlatformInfo->MemData.MemTsegSize - PlatformInfo->MemData.MemIedSize >= 0x300000);

#ifdef LT_FLAG
  PlatformInfo->MemData.MemLtMemSize  = LT_PLATFORM_DPR_SIZE + LT_DEV_MEM_SIZE;
  PlatformInfo->MemData.MemDprMemSize = LT_PLATFORM_DPR_SIZE;
#endif
  return EFI_SUCCESS;
}

/**

  Platform specific initializations in stage1.

  @param FileHandle         Pointer to the PEIM FFS file header.
  @param PeiServices       General purpose services available to every PEIM.

  @retval EFI_SUCCESS       Operation completed successfully.
  @retval Otherwise         Platform initialization failed.
**/
EFI_STATUS
EFIAPI
PlatformEarlyInitEntry (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS                            Status;
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  INTEL_SETUP_DATA                            SetupData;
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  SYSTEM_CONFIGURATION                  SystemConfiguration;
  PCH_RC_CONFIGURATION                  PchRcConfiguration;
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  ME_RC_CONFIGURATION                   MeRcConfiguration;
#endif // ME_SUPPORT_FLAG
  SOCKET_CONFIGURATION                  SocketConfiguration;
  EFI_BOOT_MODE                         BootMode = BOOT_WITH_FULL_CONFIGURATION;
  UINT32                                SerialDebugMsgLevelSetting;
  UINT8                                 RprBitSet = 0;
  SYSTEM_BOARD_PPI                      *SystemBoard;
#ifdef LT_FLAG
  EFI_PLATFORM_TXT_POLICY_DATA          PlatformTxtPolicyData;
  UINTN                                 StartupAcmAddressInFit;
  UINTN                                 StartupAcmAddress;
  BOOLEAN                               FoundFit;
  UINTN                                 FailSafeAddressInFit=0;
  UINTN                                 FailSafeAddress=0;
  UINTN                                 LcpSize=0;
#endif
  UINT32                                DfxDebugJumperValue = ADV_DEBUG_AUTO;
  EFI_PLATFORM_INFO                     *PlatformInfo;
  EFI_HOB_GUID_TYPE                     *GuidHob;
  UINT16                                ABase;
  UINT16                                Pm1Sts = 0;
  UINT32                                Pm1Cnt;

  //
  // Report Status Code EFI_CU_CHIPSET_PLATFORM_PEIM_INIT
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    (EFI_COMPUTING_UNIT_CHIPSET | EFI_CU_CHIPSET_PLATFORM_PEIM_INIT)
    );

  //
  // Get SystemBoard PPI
  //
  (**PeiServices).LocatePpi (
                  PeiServices,
                  &gEfiPeiSystemBoardPpiGuid,
                  0,
                  NULL,
                  &SystemBoard
                  );

  Status = GetEntireConfig (&SetupData);
  ASSERT_EFI_ERROR (Status);
  CopyMem (&SocketConfiguration, &(SetupData.SocketConfig), sizeof (SOCKET_CONFIGURATION));
  CopyMem (&PchRcConfiguration, &(SetupData.PchRcConfig), sizeof (PCH_RC_CONFIGURATION));
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  CopyMem (&MeRcConfiguration, &(SetupData.MeRcConfig), sizeof (ME_RC_CONFIGURATION));
#endif // ME_SUPPORT_FLAG
  CopyMem (&SystemConfiguration, &(SetupData.SystemConfig), sizeof (SYSTEM_CONFIGURATION));

  RprBitSet = MmioRead8(PCH_PCR_ADDRESS (PID_DMI, R_PCH_PCR_DMI_GCS+1));
  if(PchRcConfiguration.PchPort80Route == 0) {   // LPC
    RprBitSet = RprBitSet & (UINT8)(~B_PCH_PCR_DMI_RPR >> 8);
  }
  else {  // PCI
    RprBitSet = RprBitSet | (B_PCH_PCR_DMI_RPR >> 8);
  }
  MmioWrite8(PCH_PCR_ADDRESS (PID_DMI, R_PCH_PCR_DMI_GCS+1), RprBitSet);

  // APTIOV_SERVER_OVERRIDE_RC_START : Use AMI SIO module for SIO initialization
  #if 0
  ///
  /// Set LPC SIO
  ///
  MmioOr16(
    (MmPciBase(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC) + R_PCH_LPC_IOE),
    B_PCH_LPC_IOE_SE
    );

   LpcSioEarlyInit (SystemBoard);
  #endif
  // APTIOV_SERVER_OVERRIDE_RC_END : Use AMI SIO module for SIO initialization

  ///
  /// Configure GPIO and SIO
  ///
  GuidHob       = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  PlatformInfo  = GET_GUID_HOB_DATA(GuidHob);
  Status = BoardInit (PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  //
  // Check AdvDebug Jumper if set to Auto, otherwise, use Enabled/Disabled value
  //
  if ( SetupData.SystemConfig.DfxAdvDebugJumper == ADV_DEBUG_AUTO) {
    GpioGetInputValue(GPIO_SKL_H_GPP_A18, &DfxDebugJumperValue);
  } else {
    DfxDebugJumperValue = SetupData.SystemConfig.DfxAdvDebugJumper;
  }
  Status = PcdSetBoolS(PcdDfxAdvDebugJumper, (BOOLEAN)DfxDebugJumperValue);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return Status;
  DEBUG((EFI_D_INFO, "PcdDfxAdvDebugJumper: %d\n", PcdGetBool(PcdDfxAdvDebugJumper)));

  ///
  /// Do Early PCH init
  ///
  EarlyPlatformPchInit ((EFI_PEI_SERVICES**)PeiServices, &SystemConfiguration, &PchRcConfiguration, SystemBoard);
  PeiPolicyInitPreMem (FwConfigProduction, &SystemConfiguration, &PchRcConfiguration);

  ///
  /// Set what state (S0/S5) to go to when power is re-applied after a power failure (G3 state)
  ///
  SetTheStateToGoAfterG3 (PchRcConfiguration.StateAfterG3);

  ///----------------------------------------------------------------------------------
  ///
  /// BIOS should check the WAK_STS bit in PM1_STS[15] (PCH register ABASE+00h) before memory
  /// initialization to determine if ME has reset the system while the host was in a sleep state.
  /// If WAK_STS is not set, BIOS should ensure a non-sleep exit path is taken by overwriting
  /// PM1_CNT[12:10] (PCH register ABASE+04h) to 111b to force an s5 exit.
  ///
  PchAcpiBaseGet (&ABase);
  Pm1Sts = IoRead16 (ABase + R_PCH_ACPI_PM1_STS);
  if ((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) == 0) {
    Pm1Cnt = IoRead32 (ABase + R_PCH_ACPI_PM1_CNT);
    Pm1Cnt |= V_PCH_ACPI_PM1_CNT_S5;
    IoWrite32 (ABase + R_PCH_ACPI_PM1_CNT, Pm1Cnt);
  }

  //
  // Initialize platform PPIs
  //
  Status = PeiServicesInstallPpi (&mInstallStallPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Install MRC {Normal,Recovery} FV info
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : Removing FV not present.
  #if 0
  Status = PeiServicesGetBootMode (&BootMode);

  SystemBoard->HandleBootMode(BootMode);

  ASSERT_EFI_ERROR (Status);

#ifndef PC_HOOK
  if (BootMode != BOOT_IN_RECOVERY_MODE) {
    Status = (**PeiServices).InstallPpi (PeiServices, &mPpiListFvMrcNormal);
  }
  ASSERT_EFI_ERROR (Status);
#endif
  #endif
  // APTIOV_SERVER_OVERRIDE_RC_END : Removing FV not present.
  //
  // Create the platform Flash Map
  //
  Status = PeimInitializeFlashMap (FileHandle, PeiServices);
  ASSERT_EFI_ERROR (Status);

  if((EFI_ERROR (Status) )) {
    // If first boot, setup variable not yet set, so use default debug print level.
    SerialDebugMsgLevelSetting = PcdGet32(PcdDebugPrintErrorLevel);
  } else {
    switch(SystemConfiguration.serialDebugMsgLvl) {
    case 0:
      // Disabled
      SerialDebugMsgLevelSetting = 0x00000000;
      break;
    case 1:
      // Error only
      SerialDebugMsgLevelSetting = EFI_D_ERROR;
      break;
    case 2:
      // Normal
      SerialDebugMsgLevelSetting = PcdGet32(PcdDebugPrintErrorLevel);
      break;
    case 3:
      // Maximum
      SerialDebugMsgLevelSetting = (EFI_D_INIT | EFI_D_WARN | EFI_D_LOAD | EFI_D_FS | EFI_D_POOL | EFI_D_PAGE |
                                    EFI_D_INFO | EFI_D_VARIABLE | EFI_D_BM | EFI_D_BLKIO | EFI_D_NET | EFI_D_UNDI |
                                    EFI_D_LOADFILE | EFI_D_EVENT | EFI_D_ERROR);
      break;
    case 4:
    default:
      // Auto
      if (PcdGetBool(PcdDfxAdvDebugJumper)) {
        DEBUG((EFI_D_INFO, "Advanced Debug Jumper set - Set Debug Msg Level to Normal\n"));
        SerialDebugMsgLevelSetting = PcdGet32(PcdDebugPrintErrorLevel);
      } else {
        // Error only
        SerialDebugMsgLevelSetting = EFI_D_ERROR;
      }
      break;
    }
  }

  // APTIOV_SERVER_OVERRIDE_RC_START : #SerialDebugMsg - Restrict serialDebugMsgLvl to MRC messages only
  //SetDebugPrintErrorLevel(SerialDebugMsgLevelSetting);
  // APTIOV_SERVER_OVERRIDE_RC_END : #SerialDebugMsg - Restrict serialDebugMsgLvl to MRC messages only

  UpdatePlatformInfo (&SystemConfiguration, &SocketConfiguration);

  if (BootMode == BOOT_WITH_MFG_MODE_SETTINGS) {
  // APTIOV_SERVER_OVERRIDE_RC_START : Use AMI TPM module
  #if 0
   Status = PcdSetBoolS (PcdTpmPhysicalPresence, TRUE);
   ASSERT_EFI_ERROR (Status);
   if (EFI_ERROR(Status)) return Status;
   #endif
  // APTIOV_SERVER_OVERRIDE_RC_END : Use AMI TPM module
  }

  //
  // Modify Master Access in descriptor region to enable access to all regions
  //
  if (PcdGetBool(PcdDfxAdvDebugJumper)) {
    EnableMasterAccess((EFI_PEI_SERVICES**)PeiServices);
  }

  //
  // Do platform specific on-board Zoar init
  //
  SystemBoard->LanEarlyInit((EFI_PEI_SERVICES**)PeiServices, &SystemConfiguration);

  //
  // Initialize platform PPIs
  //
  Status = PeiServicesInstallPpi (&mPpiList[0]);
  ASSERT_EFI_ERROR (Status);

  Status = PeimInstallFlashMapPpi (FileHandle, PeiServices);
  ASSERT_EFI_ERROR (Status);

#ifdef LT_FLAG
    //
    // Add BIOS ACM address hob
    //
    FoundFit = FindFITRecord (PLATFORM_TXT_FIT_BIOSACM_TYPE, &StartupAcmAddressInFit, &StartupAcmAddress);
    //
    // Get BIOS LCP PD Base & Size by searching FAIL_SAFE  Policy in FIT Table
    //
    FindFITRecord (PLATFORM_TXT_FIT_FAILSAFE_TYPE, &FailSafeAddressInFit, &FailSafeAddress);
    if (FailSafeAddress != 0) {
        LcpSize = (UINTN)*(UINT64 *)(FailSafeAddressInFit+8);
        LcpSize = LcpSize & 0x0000FFFFFF;
        LcpSize = LcpSize << 4;
        LcpSize = LcpSize + 0xC;
    }
    // Only build HOB when LTSX strapping ON, in which case ucode would setup LT_EXIST.
    if ( (FoundFit) && ((*(volatile UINT32 *)(UINTN)LT_SPAD_EXIST)) ) {
      // APTIOV_SERVER_OVERRIDE_RC_START : TO DO: Use SDL token for ACM BASE. Comment the hardcoded value when IntelLtsxFit_SUPPORT is Enabled.
      //PlatformTxtPolicyData.BiosAcmAddress           = ACM_BASE;
      PlatformTxtPolicyData.BiosAcmAddress           = StartupAcmAddress;
      DEBUG ((DEBUG_INFO, "BiosAcmAddress: %X\n", StartupAcmAddress));
     // APTIOV_SERVER_OVERRIDE_RC_END : TO DO: Use SDL token for ACM BASE. Comment the hardcoded value when IntelLtsxFit_SUPPORT is Enabled.
      PlatformTxtPolicyData.StartupAcmAddressInFit   = (EFI_PHYSICAL_ADDRESS) StartupAcmAddressInFit; // This is to be used later in EFI shell utility when updating the FI table to enable the ACM module (might not be needed anymore)
      PlatformTxtPolicyData.StartupAcmAddress        = (EFI_PHYSICAL_ADDRESS) StartupAcmAddress;
      PlatformTxtPolicyData.MiscPolicy               = (EFI_PLATFORM_TXT_POLICY_MISC_NO_SCHECK_IN_RESUME | EFI_PLATFORM_TXT_POLICY_MISC_NO_NODMA_TABLE);
      PlatformTxtPolicyData.BiosOsDataRegionRevision = 3;
      PlatformTxtPolicyData.LcpPolicyDataBase        = (EFI_PHYSICAL_ADDRESS) FailSafeAddress;
      PlatformTxtPolicyData.LcpPolicyDataSize        = (UINTN)LcpSize;
      PlatformTxtPolicyData.TxtScratchAddress        = 0;
      PlatformTxtPolicyData.BiosAcmPolicy            = 0;
      PlatformTxtPolicyData.FlagVariable             = 0;

      BuildGuidDataHob (
        &gEfiPlatformTxtPolicyDataGuid,
        &PlatformTxtPolicyData,
        sizeof (PlatformTxtPolicyData)
        );
    } else {
      DEBUG((EFI_D_ERROR, "Fit Table not found, do nothing for LT-SX ...\n"));
    }
#endif

  //
  // Initialize platform PPIs
  //
  Status = PeiServicesNotifyPpi (&mNotifyList[0]);
  ASSERT_EFI_ERROR (Status);

// APTIOV_SERVER_OVERRIDE_RC_START : Use token to control FDT lock
#if FDT_LOCK_ENABLE
  if (!PcdGetBool(PcdDfxAdvDebugJumper)) {
    DEBUG((EFI_D_INFO, "SPI Descriptor Lock\n"));
    PchLockDescriptorRegion((EFI_PEI_SERVICES**)PeiServices);
  }
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Use token to control FDT lock
  return Status;
}

EFI_STATUS SpiDescriptorAccess (
  UINT8  BYTE_COUNT,
  UINT8  FLASH_CYCLE,
  UINT32 FLASH_ADDR,
  UINT32 *DATA_BUFF
)
{
  UINT8  Index;
  UINT8  ReadCmd;
  UINT8  WriteCmd;

  ReadCmd  = B_SB_SPI_READ_CYCLE;
  WriteCmd = B_SB_SPI_WRITE_CYCLE;

  //set Flash Address to get Descriptor
  *(volatile UINT32 *)(SPI_BASE_ADDRESS + R_SB_SPI_FADDR) = FLASH_ADDR;

  //initiate Read cycle to Flash and get Descriptor Region 0


  //
  // HSFCTL offset 0x6: Hardware Sequencing Flash Control
  // need to clear following bits:
  //     bits [13:8] Flash Data Byte Count(FDBC), specifies the number of bytes
  //                 to shift in or out during data portion of SPI cycle
  //     bits [07:1] Flash Cycle, this field specifies the cycle type
  //                 00 Read     (1 to 64 defined by FDBC)
  //                 01 Reserved
  //                 10 Write    (1 to 64 defined by FDBC)
  //                 11 Block Erase
  //     bit  [0]    Flash Cycle Go, A write to this register with 1 will initiate
  //                 an spi cycle
  //
  *(volatile UINT16 *)(SPI_BASE_ADDRESS + R_SB_SPI_HSFCTL )    &=  HSFC_MASK;                //clear register
  *(volatile UINT16 *)(SPI_BASE_ADDRESS + R_SB_SPI_HSFCTL )    |= ((UINT16)BYTE_COUNT)<<8; //read/write X bytes [13:8]


  //READ command to SPI
  if(FLASH_CYCLE == ReadCmd){

     //send Read Command
     *(volatile UINT16 *)(SPI_BASE_ADDRESS + R_SB_SPI_HSFCTL )    |=   FLASH_CYCLE;

     //
     //wait for Data to be ready
     // HSFSTS offset 0x4: Hardware Sequencing Flash Status
     // SPI Cycle in progress bit [5]
     //     Hardware sets this bit when software sets the flash cycle go
     //     this bit remains set until the cycle completes on SPI interface
     //
     while((*(volatile UINT16 *)(SPI_BASE_ADDRESS + R_SB_SPI_HSFSTS)) & (1<<5));

     //Data is ready copy to Buffer
     for(Index=0; Index <= (BYTE_COUNT/4); Index++){
         DATA_BUFF[Index] = *(volatile UINT32 *)(SPI_BASE_ADDRESS + R_SB_SPI_FDAT0 + Index );
     }
  }

  //WRITE command to SPI
  if(FLASH_CYCLE == WriteCmd){

     //Copy Buffer to registers for writing
     for(Index=0; Index <= (BYTE_COUNT/4); Index++){
         *(volatile UINT32 *)(SPI_BASE_ADDRESS + R_SB_SPI_FDAT0 + Index ) = DATA_BUFF[Index];
     }

     //send Write Command
     *(volatile UINT16 *)(SPI_BASE_ADDRESS + R_SB_SPI_HSFCTL )    |=   FLASH_CYCLE;

     //
     //wait for Data to be ready
     // HSFSTS offset 0x4: Hardware Sequencing Flash Status
     // SPI Cycle in progress bit [5]
     //     Hardware sets this bit when software sets the flash cycle go
     //     this bit remains set until the cycle completes on SPI interface
     //
     while((*(volatile UINT16 *)(SPI_BASE_ADDRESS + R_SB_SPI_HSFSTS)) & (1<<5));
  }

  return(EFI_SUCCESS);
}

EFI_STATUS DescriptorCheck(void)
{

  UINT32   Data32;
  UINT32   FlashAddr;
  UINT8    CmdCycle;

  CmdCycle  = B_SB_SPI_READ_CYCLE;
  FlashAddr = DESCRIP_ADDR;

  //
  //FLVALSIG offset 010h: Flash Valid signature
  //       bits [31:00]: 0x0FF0A55Ah
  //       This field identifies the flash descriptor sector as vaild
  //       if content is not expected value, then flash descriptor region
  //       is assumed un-programmed or corrupted
  //
  SpiDescriptorAccess(0x3,CmdCycle,FlashAddr,&Data32);

  if(Data32 == 0x0FF0A55A){
     //Valid signature found
     return(EFI_SUCCESS);
  }else{
     //Valid signature not found we are not in descriptor mode
     return(EFI_UNSUPPORTED);
  }//end valid signature

}

EFI_STATUS SetupVSCC()
{
    UINT16 VSCC = 0;

  VSCC=SPI_VSCC_MINUM_COMMUN_DENOMINATOR;

  *(volatile UINT16 *)(SPI_BASE_ADDRESS + R_SB_SPI_VSCC0  )    = VSCC; //Set Erase block upcode
  *(volatile UINT16 *)(SPI_BASE_ADDRESS + R_SB_SPI_VSCC1  )    = VSCC; //Set Erase block upcode

  if(*(volatile UINT16 *)(SPI_BASE_ADDRESS + R_SB_SPI_VSCC1  ) == VSCC){
      return(EFI_SUCCESS);
  }else{
      return(EFI_UNSUPPORTED);
  }
}

EFI_STATUS SpiAccessCheck(void)
{

  UINT32   Data32;
  UINT16   Data16;

  // Check if SPI controller is available
  Data32 = *(volatile UINT32 *)(UINT32)(SPI_BASE_ADDRESS);

  if(Data32 != 0xFFFFFFFF){

     SetupVSCC(); // Setup VSCC values.

     //
     // HSFSTS offset 0x4: Hardware Sequencing Flash Status
     // Flash Configuration Lock-Down bit [15]
     //     when set to 1 Flash program registers controlled by FLOCKDN
     //     cannot be written.
     //

     Data16    = *(volatile UINT16 *)(UINT32)( SPI_BASE_ADDRESS + R_SB_SPI_HSFSTS );

     if (Data16 & 0x8000){
         //if Lock down is set there is nothing to do
         return(EFI_UNSUPPORTED);
     }else{
         //SPI Controller is available
         return(EFI_SUCCESS);
     }//end lock bit set

  }else{
     //SPI controller was not available signal error
     return(EFI_UNSUPPORTED);
  }//end SPI controller available

}


/**

  Modify Master Access fields in the descriptor region to enable
  access to all regions

  @param **PeiServices - Pointer to the list of PEI services

  @retval VOID

**/
VOID
EnableMasterAccess (
  IN EFI_PEI_SERVICES           **PeiServices
  )
{
  EFI_STATUS  Status;
  UINT32      Data32;
  UINT32      FlashAddr;
  UINT8       CmdCycle;
  UINT8       Index;
  UINT8       NumRegions;
  UINT32      fmba;
  UINT32      Flmap1;

  Status = SpiAccessCheck();

  if (Status != EFI_SUCCESS){
    DEBUG((EFI_D_ERROR,  "ERROR: SPI Controller not available \n"));
    return;
  }

  Status = DescriptorCheck();

  if (Status != EFI_SUCCESS){
    DEBUG((EFI_D_ERROR, "ERROR: VALID SIGNATURE NOT FOUND.\n"));
    return;
  }

  //
  // Locate the Flash Master base address
  //
  FlashAddr = DESCRIP_FLMAP1;
  CmdCycle = B_SB_SPI_READ_CYCLE;
  SpiDescriptorAccess(0x3, CmdCycle, FlashAddr, &Flmap1);

  fmba = 0;
  fmba |= (Flmap1 & 0x000000FF) << 4;

  DEBUG((EFI_D_INFO, "FLASH MAP1 = %x, FMBA = %x.\n",Flmap1, fmba));

  //
  // Set the number of FLMSTR fields
  //
  NumRegions = 6;

  FlashAddr = fmba;

  for (Index = 0; Index < NumRegions; Index++) {
    // (FMBA+00h) Flash Master 1 - Host CPU / BIOS
    // (FMBA+04h) Flash Master 2 - CSME - Skip
    // (FMBA+08h) Flash Master 3 - GbE
    // (FMBA+0Ch) Flash Master 4 - Sideband Device
    // (FMBA+10h) Flash Master 5 - BMC
    // (FMBA+14h) Flash Master 6 - IE

    CmdCycle = B_SB_SPI_READ_CYCLE;
    SpiDescriptorAccess(0x3, CmdCycle, FlashAddr, &Data32);

    DEBUG((EFI_D_INFO, "Flash Master %d = %x.\n", Index + 1, Data32));

    //
    // Need to skip enabling master access for ME region
    //
    if(Index == 1){
      FlashAddr += 4;
      continue;
    }
    //
    // For LBG/SPT, set all read and write access bits [31:0]
    //
    if (Data32 != 0xFFFFFFFF) {
      Data32 = 0xFFFFFFFF;
      CmdCycle = B_SB_SPI_WRITE_CYCLE;
      SpiDescriptorAccess(0x3, CmdCycle, FlashAddr, &Data32);
    }
    FlashAddr += 4;
  }

}

/**

  Checks the setup option for breakpoint type and starts
  a deadloop if After MRC was selected

  @param **PeiServices - Pointer to the list of PEI services
  @param *NotifyDescriptor - The notification structure this PEIM registered on install
  @param *Ppi - Pointer to the PPI

  @retval EFI_SUCCESS  -  Function has completed successfully.

**/
EFI_STATUS
EFIAPI
AfterMrcBreakpoint (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  )
{
  EFI_STATUS Status;
  UINT8 ValidationBreakpointTypeOption;

  Status = GetOptionData(&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, ValidationBreakpointType), &ValidationBreakpointTypeOption, sizeof(ValidationBreakpointTypeOption));
  if (EFI_ERROR(Status))
    ValidationBreakpointTypeOption = BP_NONE;

  if (ValidationBreakpointTypeOption == BP_AFTER_MRC) {
    DEBUG((EFI_D_INFO, "== Validation Breakpoint After MRC is done! == \n"));
    EFI_DEADLOOP();
  }

  return Status;
}

/**

  Checks the setup option for breakpoint type and starts
  a deadloop if After QPIRC was selected

  @param **PeiServices - Pointer to the list of PEI services
  @param *NotifyDescriptor - The notification structure this PEIM registered on install
  @param *Ppi - Pointer to the PPI

  @retval EFI_SUCCESS  -  Function has completed successfully.

**/
EFI_STATUS
EFIAPI
AfterQpircBreakpoint (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  )
{
  EFI_STATUS Status;
  UINT8 ValidationBreakpointTypeOption;

  Status = GetOptionData(&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, ValidationBreakpointType), &ValidationBreakpointTypeOption, sizeof(ValidationBreakpointTypeOption));
  if (EFI_ERROR(Status))
    ValidationBreakpointTypeOption = BP_NONE;

  if (ValidationBreakpointTypeOption == BP_AFTER_QPIRC) {
    DEBUG((EFI_D_INFO, "== Validation Breakpoint After QPIRC is done! == \n"));
    EFI_DEADLOOP();
  }

  return Status;
}


/**

  Checks the setup option for breakpoint type and starts
  a deadloop if Ready for IBIST was selected

  @param **PeiServices - Pointer to the list of PEI services
  @param *NotifyDescriptor - The notification structure this PEIM registered on install
  @param *Ppi - Pointer to the PPI

  @retval EFI_SUCCESS  -  Function has completed successfully.

**/
EFI_STATUS
EFIAPI
ReadyForIbistBreakpoint (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  )
{
  EFI_STATUS Status;
  UINT8 ValidationBreakpointTypeOption;

  Status = GetOptionData(&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, ValidationBreakpointType), &ValidationBreakpointTypeOption, sizeof(ValidationBreakpointTypeOption));
  if (EFI_ERROR(Status))
    ValidationBreakpointTypeOption = BP_NONE;

  if (ValidationBreakpointTypeOption == BP_READY_FOR_IBIST) {
    DEBUG((EFI_D_INFO, "== Validation Breakpoint when Ready for IBIST! == \n"));
    EFI_DEADLOOP();
  }

  return Status;
}

/**

  Checks the setup option for breakpoint type and starts
  a deadloop if the After Full Speed Setup was selected

  @param **PeiServices - Pointer to the list of PEI services
  @param *NotifyDescriptor - The notification structure this PEIM registered on install
  @param *Ppi - Pointer to the PPI

  @retval EFI_SUCCESS  -  Function has completed successfully.

**/
EFI_STATUS
EFIAPI
AfterFullSpeedSetupBreakpoint (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  )
{
  EFI_STATUS Status;
  UINT8 ValidationBreakpointTypeOption;

  Status = GetOptionData(&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, ValidationBreakpointType), &ValidationBreakpointTypeOption, sizeof(ValidationBreakpointTypeOption));
  if (EFI_ERROR(Status))
    ValidationBreakpointTypeOption = BP_NONE;

  if (ValidationBreakpointTypeOption == BP_AFTER_FULL_SPEED_SETUP) {
    DEBUG((EFI_D_INFO, "== Validation Breakpoint before QPI go Fast! == \n"));
    EFI_DEADLOOP();
  }

  return Status;
}

/**

    Read reset type and count warm resets in CMOS location.
    For warm reset counter access from DXE, put counter
    to Pcd also.

    @param PeiServices   - pointer to the PEI Service Table

    @retval None.
**/
VOID
UpdateResetTypeInformation(
    IN EFI_PEI_SERVICES **PeiServices
    )
{
  UINT8      Data8;
  BOOLEAN    WarmReset;
  UINT8      CmosValue;
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  EFI_STATUS Status;
#endif // ME_SUPPORT_FLAG

  DEBUG((EFI_D_INFO, "UpdateResetTypeInformation() "));

  // Read CMOS WarmReset Counter
  IoWrite8(R_IOPORT_CMOS_UPPER_INDEX, CMOS_WARM_RESET_COUNTER_OFFSET);
  CmosValue = IoRead8( R_IOPORT_CMOS_UPPER_DATA );

  Data8 = MmioRead8 (
            MmPciBase(
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_PMC,
               PCI_FUNCTION_NUMBER_PCH_PMC
            ) +
               R_PCH_PMC_GEN_PMCON_A
          );
  WarmReset = (BOOLEAN) !!(Data8 & B_PCH_PMC_GEN_PMCON_A_MEM_SR);

  if (WarmReset) {
    // current boot is warm
    CmosValue++;
    DEBUG((EFI_D_INFO, "to WARM\n"));
#define WARM_RESET_COUNTER_LIMIT 10
    if (CmosValue > WARM_RESET_COUNTER_LIMIT) {
      // limit CmosValue to avoid counter rewind
      // and false cold reset indication
      CmosValue = WARM_RESET_COUNTER_LIMIT;
    }
#undef WARM_RESET_COUNTER_LIMIT
  } else {
    // current boot is cold, reset warm reset counter
    CmosValue = 0;
    DEBUG((EFI_D_INFO, "to COLD\n"));
  }

  IoWrite8(R_IOPORT_CMOS_UPPER_INDEX, CMOS_WARM_RESET_COUNTER_OFFSET);
  IoWrite8(R_IOPORT_CMOS_UPPER_DATA, CmosValue);
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  Status = PcdSet32S(PcdMePlatformResetType, (UINT32) CmosValue);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return;
#endif // ME_SUPPORT_FLAG

}

/**

    GC_TODO: add routine description

    @param PeiServices - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description
    @retval (Status)    - GC_TODO: add retval description

**/
EFI_STATUS
PchLockDescriptorRegion(
  IN EFI_PEI_SERVICES **PeiServices
  )
{
  UINT32      FlashAddr;
  UINT8       CmdCycle;
  UINT32      Flmap1;
  UINT32      fmba;
  UINT32      Data32;
  EFI_STATUS  Status;
  EFI_SPI_PROTOCOL *SpiProtocol = NULL;
#ifndef PC_HOOK
  PCH_RESET_PPI    *PchResetPpi = NULL;
#endif

  DEBUG((EFI_D_INFO, "LockFlashDescriptor() Entry \n"));

  UpdateResetTypeInformation(PeiServices);

#if defined(AMT_SUPPORT) && AMT_SUPPORT
  if (MeTypeIsAmt()) {
    DEBUG((EFI_D_INFO, "For AMT do not execute LockFlashDescriptor()\n"));
    return EFI_SUCCESS;
  }
#endif // AMT_SUPPORT
  Status = SpiAccessCheck();

  if (Status != EFI_SUCCESS){
    DEBUG((EFI_D_ERROR,  "ERROR: SPI Controller not available \n"));
    return(Status);
  }

  Status = DescriptorCheck();

  if (Status != EFI_SUCCESS){
    DEBUG((EFI_D_ERROR, "ERROR: VALID SIGNATURE NOT FOUND.\n"));
    return(Status);
  }

  FlashAddr = DESCRIP_FLMAP1;
  CmdCycle = B_SB_SPI_READ_CYCLE;
  SpiDescriptorAccess(0x3, CmdCycle, FlashAddr, &Flmap1);

  fmba = 0;
  fmba |= (Flmap1 & 0x000000FF) << 4;

  DEBUG((EFI_D_INFO, "FLASH MAP1 = %x, FMBA = %x.\n",Flmap1, fmba));

  // Check if softstrap override is disabled by SV
  Status = (**PeiServices).LocatePpi(
                              PeiServices,
                              &gPeiSpiPpiGuid,
                              0,
                              NULL,
                              &SpiProtocol
                              );
  ASSERT_EFI_ERROR (Status);

  if (isFlashDescriptorOverrideDisadled(PeiServices, SpiProtocol, fmba) == TRUE) {
    return EFI_ABORTED;
  }

  //Flash Master 1 - Host CPU / BIOS
  FlashAddr = fmba;
  CmdCycle = B_SB_SPI_READ_CYCLE;
  SpiDescriptorAccess(0x3, CmdCycle, FlashAddr, &Data32);

  DEBUG((EFI_D_INFO, "Flash Master 1 = %x.\n", Data32));
#ifndef PC_HOOK
  if ((Data32 & BIT20) == BIT20) {
    Data32 &= ~BIT20;
    FlashAddr = fmba;
    CmdCycle = B_SB_SPI_WRITE_CYCLE;
    SpiDescriptorAccess(0x3, CmdCycle, FlashAddr, &Data32);
    DEBUG((EFI_D_ERROR, "Debug Performing global reset to block SPI flash descriptor...\n"));
    Status = (*PeiServices)->LocatePpi (
                                PeiServices,
                                &gPchResetPpiGuid,
                                0,
                                NULL,
                                (VOID **)&PchResetPpi
                                );
    ASSERT_EFI_ERROR (Status);
    PchResetPpi->Reset (PchResetPpi, GlobalReset);
    CpuDeadLoop();
  }
#endif
  return(Status);
}
