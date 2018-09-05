/** @file
  This file contains the tests for the SecureMemoryMapConfiguration bit

@copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification
**/

#include "HstiIhvProviderDxe.h"
// APTIOV_SERVER_OVERRIDE_RC_START: SpiProtocol not used, replace with SDL TOKEN
#include <Token.h>
// APTIOV_SERVER_OVERRIDE_RC_END: SpiProtocol not used, replace with SDL TOKEN

extern IIO_GLOBALS              *IioGlobalData;
extern EFI_IIO_UDS_PROTOCOL     *mIioUds;

typedef struct {
  UINT64   Base;
  UINT64   End;
} MEMORY_RANGE;

typedef enum {
  LockableMemoryRangeLtSpace,
  LockableMemoryRangeHighBios,
  LockableMemoryRangeLowDram,
  LockableMemoryRangeHighDram,
  LockableMemoryRangeMmcfgBar,
  LockableMemoryRangeCpuRsvd1Bar,
  LockableMemoryRangeCpuRsvd2Bar,
  LockableMemoryRangeMax,
} LOCKABLE_MEMORY_RAMGE;

MEMORY_RANGE  mLockableMemoryRange[LockableMemoryRangeMax] = {
  // 1. LT space (0xFED20000 - 0xFED7FFFF)
  {0xFED20000, 0xFED7FFFF},
  // 2. High BIOS
  {0x0, SIZE_4GB - 1},
  // 3. Low DRAM (0 - TOLUD)
  {0x0, 0},
  // 4. High DRAM (4GB - TOUUD)
  {SIZE_4GB, 0},
  // 5. MMCFGBAR
  {0, 0},
  // 7. CPU Reserved space: 0xFEB00000 to 0xFEB0FFFF
  {0xFEB00000, 0xFEB0FFFF},
  // 8. CPU Reserved space: 0xFEB80000 to 0xFEB8FFFF
  {0xFEB80000, 0xFEB8FFFF},
};

typedef enum {
  NonLockableMemoryRangeABAR,
  NonLockableMemoryRangeSBREG_BAR,
  NonLockableMemoryRangePWRMBASE,
  NonLockableMemoryRangeSPI_BAR0,
  NonLockableMemoryRangeMax,
} NONLOCKABLE_MEMORY_RAMGE;

MEMORY_RANGE  mNonLockableMemoryRange[NonLockableMemoryRangeMax] = {
  {0, 0},
// 1. Local Apic for each CPU thread (IA32_APICBASE MSR 0x1B)
// 2. ABAR,          BDF 0:31:2 + 0x24
// 3. SBREG_BAR (BDF 0:31:1 + 0x10)
// 4. PWRMBASE (BDF 0:31:2 + 0x48)
// 5. SPI_BAR0 (BDF 0:31:5 + 0x10)
};

/**
  Check for overlaps in single range array

  @param[in] Range     - Pointer to Range array
  @param[in] Count     - Number of Enties

  @retval BOOLEAN - Overlap Exists
**/
BOOLEAN
CheckOverlap (
  IN MEMORY_RANGE *Range,
  IN UINTN        Count
  )
{
  UINTN  Index;
  UINTN  SubIndex;

  for (Index = 0; Index < Count - 1; Index++) {
    if ((Range[Index].Base == 0) && (Range[Index].End == 0)) {
      continue;
    }
    for (SubIndex = Index + 1; SubIndex < Count; SubIndex++) {
      if ((Range[SubIndex].Base == 0) && (Range[SubIndex].End == 0)) {
        continue;
      }
      if (((Range[Index].Base >= Range[SubIndex].Base) && (Range[Index].Base <= Range[SubIndex].End)) ||
          ((Range[SubIndex].Base >= Range[Index].Base) && (Range[SubIndex].Base <= Range[Index].End))) {
        DEBUG ((DEBUG_ERROR, "OVERLAP: \n"));
        DEBUG ((DEBUG_ERROR, "  0x%016lx - 0x%016lx\n", Range[Index].Base, Range[Index].End));
        DEBUG ((DEBUG_ERROR, "  0x%016lx - 0x%016lx\n", Range[SubIndex].Base, Range[SubIndex].End));
        return TRUE;
      }
    }
  }

  DEBUG ((DEBUG_INFO, "CheckOverlap: PASS\n"));
  return FALSE;
}

/**
  Check for overlaps between two arrays of memory ranges

  @param[in] Range1     - Pointer to Range1 array
  @param[in] Count1     - Number of Enties
  @param[in] Range2     - Pointer to Range2 array
  @param[in] Count2     - Number of Enties

  @retval BOOLEAN - Overlap Exists
**/
BOOLEAN
CheckOverlap2 (
  IN MEMORY_RANGE *Range1,
  IN UINTN        Count1,
  IN MEMORY_RANGE *Range2,
  IN UINTN        Count2
  )
{
  UINTN  Index1;
  UINTN  Index2;

  for (Index1 = 0; Index1 < Count1; Index1++) {
    if ((Range1[Index1].Base == 0) && (Range1[Index1].End == 0)) {
      continue;
    }
    for (Index2 = 0; Index2 < Count2; Index2++) {
      if ((Range2[Index2].Base == 0) && (Range2[Index2].End == 0)) {
        continue;
      }
      if (((Range1[Index1].Base >= Range2[Index2].Base) && (Range1[Index1].Base <= Range2[Index2].End)) ||
          ((Range2[Index2].Base >= Range1[Index1].Base) && (Range2[Index2].Base <= Range1[Index1].End))) {
        DEBUG ((DEBUG_ERROR, "OVERLAP2: \n"));
        DEBUG ((DEBUG_ERROR, "  0x%016lx - 0x%016lx\n", Range1[Index1].Base, Range1[Index1].End));
        DEBUG ((DEBUG_ERROR, "  0x%016lx - 0x%016lx\n", Range2[Index2].Base, Range2[Index2].End));
        return TRUE;
      }
    }
  }

  DEBUG ((DEBUG_INFO, "CheckOverlap2: PASS\n"));
  return FALSE;
}

/**
  Dumps Ranges to Serial

  @param[in] Range     - Pointer to Range array
  @param[in] Count     - Number of Enties

**/
VOID
DumpRange (
  IN MEMORY_RANGE *Range,
  IN UINTN        Count
  )
{
  UINTN  Index;

  for (Index = 0; Index < Count; Index ++) {
    DEBUG ((DEBUG_INFO, "  [%02d] 0x%016lx - 0x%016lx\n", Index, Range[Index].Base, Range[Index].End));
  }
}

/**
  Run tests for SecureMemoryMapConfiguration bit
**/
VOID
CheckSecureMemoryMapConfiguration (
  VOID
  )
{
  EFI_STATUS        Status;
  BOOLEAN           Result;
  UINT32            Dpr;
  UINT32            MeSegMask;
  CHAR16            *HstiErrorString;
  UINT32            BarRead;
  UINT16            VendorIdRead;
  UINTN             PchSpiBase;
  UINT32            PwrmBase;
  UINT32            AbarSize;
  // APTIOV_SERVER_OVERRIDE_RC_START: SpiProtocol not used, replace with SDL TOKEN
  // EFI_SPI_PROTOCOL  *SpiProtocol;
  // APTIOV_SERVER_OVERRIDE_RC_END: SpiProtocol not used, replace with SDL TOKEN
  UINT32            DmiRcBar;
  UINT32            RegionSize;
  UINT8             Stack;
  UINT8             IioIndex = 0;
  UINT64            LtCtrl;
  UINT64            BaseAddress;
  EFI_CPU_CSR_ACCESS_PROTOCOL   *CpuCsrAccess;

  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;

  if ((mFeatureImplemented[1] & HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION) == 0) {
    return;
  }

  Result = TRUE;

  DEBUG ((DEBUG_INFO, "  Memory Map Secure Configuration\n"));

  DEBUG ((DEBUG_INFO, "    1. DPR\n"));

  for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++) {
    if(!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]){
      continue;
    }
    break;
  }

  //
  // Return if we did not find socket present
  //
  if (IioIndex == MaxIIO) {
    return;
  }

  for (Stack = 0; Stack < MAX_IIO_STACK; Stack++) {
    // Check for a valid stack 
    if (!(IioGlobalData->IioVar.IioVData.StackPresentBitmap[IioIndex] & (1 << Stack))){
      continue;
    }
    break;
  }

  //
  // Return if we did not find stack present
  //
  if (Stack == MAX_IIO_STACK) {
    return;
  }

  Dpr = CpuCsrAccess->ReadCpuCsr (IioIndex, Stack, LTDPR_IIO_VTD_REG);

  if ((Dpr & B_SA_DPR_LOCK_MASK) == 0) {
    DEBUG ((DEBUG_INFO, "Fail: DPR not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_CODE_1 ,HSTI_MEMORY_MAP_SECURITY_CONFIGURATION, HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_STRING_1);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    2. MESEG\n"));
  MeSegMask =  MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,SA_MC_DEV,SA_MC_FUN) + R_SA_MESEG_MASK);
  LtCtrl = AsmReadMsr64(EFI_MSR_LT_CONTROL);
  //
  // MESEG locks with LT_CONTROL lock
  //
  if ((LtCtrl & BIT0) == 0) {
    DEBUG ((DEBUG_INFO, "Fail: MESEG not locked\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_CODE_1 ,HSTI_MEMORY_MAP_SECURITY_CONFIGURATION, HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_STRING_1);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    3. DMIRCBAR\n"));

  DmiRcBar = CpuCsrAccess->ReadCpuCsr (IioIndex, Stack, DMIRCBAR_IIO_PCIEDMI_REG);
  if ((DmiRcBar & BIT0) != 0) {
    DEBUG ((DEBUG_INFO, "Fail: DMIRCBAR enabled\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_CODE_1 ,HSTI_MEMORY_MAP_SECURITY_CONFIGURATION, HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_STRING_1);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }
  DEBUG ((DEBUG_INFO, "    4. Lockable/fixed memory ranges overlap\n"));

// APTIOV_SERVER_OVERRIDE_RC_START: SpiProtocol not used, replace with SDL TOKEN
#if 0
  //
  // Locate BIOS region size to update High bios base address
  //
  SpiProtocol = NULL;
  Status = gBS->LocateProtocol (&gEfiSpiProtocolGuid, NULL, (VOID **) &SpiProtocol);
  ASSERT_EFI_ERROR (Status);

  if(SpiProtocol != NULL) {
    SpiProtocol->GetRegionAddress (SpiProtocol, FlashRegionBios, &(UINT32)BaseAddress, &RegionSize);

    DEBUG ((DEBUG_INFO, "Bios Region Size %x:\n", RegionSize));
    mLockableMemoryRange[LockableMemoryRangeHighBios].Base  = SIZE_4GB - RegionSize;
  }
#endif
  RegionSize = BIOS_AREA_SIZE;
  DEBUG ((DEBUG_INFO, "Bios Region Size %x:\n", RegionSize));
  mLockableMemoryRange[LockableMemoryRangeHighBios].Base  = SIZE_4GB - RegionSize;
// APTIOV_SERVER_OVERRIDE_RC_END: SpiProtocol not used, replace with SDL TOKEN

  mLockableMemoryRange[LockableMemoryRangeLowDram].End  = mIioUds->IioUdsPtr->PlatformData.MemTolm - 1;
  BaseAddress = LShiftU64(CpuCsrAccess->ReadCpuCsr (IioIndex, Stack, TOHM_1_IIO_VTD_REG), 32);
  BaseAddress |= (UINT32)CpuCsrAccess->ReadCpuCsr (IioIndex, Stack, TOHM_0_IIO_VTD_REG);
  mLockableMemoryRange[LockableMemoryRangeHighDram].End = BaseAddress - 1;

  mLockableMemoryRange[LockableMemoryRangeMmcfgBar].Base = (UINT32)mIioUds->IioUdsPtr->PlatformData.PciExpressBase;
  mLockableMemoryRange[LockableMemoryRangeMmcfgBar].End  = (mLockableMemoryRange[LockableMemoryRangeMmcfgBar].Base + 
                                                          (UINT32)mIioUds->IioUdsPtr->PlatformData.PciExpressSize) - 1;

  DEBUG ((DEBUG_INFO, "Lockable memory ranges:\n"));
  DumpRange (mLockableMemoryRange, LockableMemoryRangeMax);
  if (CheckOverlap (mLockableMemoryRange, LockableMemoryRangeMax) || EFI_ERROR (Status)) {

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_CODE_2 ,HSTI_MEMORY_MAP_SECURITY_CONFIGURATION, HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_STRING_2);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    5. Non lockable memory ranges overlap\n"));

  VendorIdRead = MmioRead16 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_SATA,PCI_FUNCTION_NUMBER_PCH_SATA) +0);
  DEBUG ((DEBUG_INFO, "VendorIdRead SATA: %x\n",VendorIdRead));

  if (VendorIdRead != 0xFFFF) {
    BarRead = MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_SATA,PCI_FUNCTION_NUMBER_PCH_SATA) + R_PCH_SATA_SATAGC);

    if ((BarRead & BIT10) == 0 ) {
      mNonLockableMemoryRange[NonLockableMemoryRangeABAR].Base = MmioRead32 (MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH,PCI_DEVICE_NUMBER_PCH_SATA,PCI_FUNCTION_NUMBER_PCH_SATA) + R_PCH_SATA_AHCI_BAR) & 0xFFFFF800;

      switch (BarRead & 0x7) {
        case V_PCH_SATA_SATAGC_ASSEL_2K:
          AbarSize = SIZE_2KB;
          break;

        case V_PCH_SATA_SATAGC_ASSEL_16K:
          AbarSize = SIZE_16KB;
          break;

        case V_PCH_SATA_SATAGC_ASSEL_32K:
          AbarSize = SIZE_32KB;
          break;

        case V_PCH_SATA_SATAGC_ASSEL_64K:
          AbarSize = SIZE_64KB;
          break;

        case V_PCH_SATA_SATAGC_ASSEL_128K:
          AbarSize = SIZE_128KB;
          break;

        case V_PCH_SATA_SATAGC_ASSEL_512K:
          AbarSize = SIZE_256KB;
          break;

        default:
          AbarSize= SIZE_2KB;
          break;
      }
      mNonLockableMemoryRange[NonLockableMemoryRangeABAR].End  = mNonLockableMemoryRange[NonLockableMemoryRangeABAR].Base + AbarSize - 1;
    }
  }

  mNonLockableMemoryRange[NonLockableMemoryRangeSBREG_BAR].Base = PCH_PCR_BASE_ADDRESS;
  mNonLockableMemoryRange[NonLockableMemoryRangeSBREG_BAR].End = PCH_PCR_BASE_ADDRESS + SIZE_16MB-1;

  PchPwrmBaseGet (&PwrmBase);
  mNonLockableMemoryRange[NonLockableMemoryRangePWRMBASE].Base = PwrmBase;
  mNonLockableMemoryRange[NonLockableMemoryRangePWRMBASE].End  = PwrmBase + PCH_PWRM_MMIO_SIZE - 1;

  PchSpiBase = MmioRead32 (MmPciBase (
                             DEFAULT_PCI_BUS_NUMBER_PCH,
                             PCI_DEVICE_NUMBER_PCH_SPI,
                             PCI_FUNCTION_NUMBER_PCH_SPI)
                             + R_PCH_SPI_BAR0) & ~B_PCH_SPI_BAR0_MASK;

  mNonLockableMemoryRange[NonLockableMemoryRangeSPI_BAR0].Base = PchSpiBase;
  mNonLockableMemoryRange[NonLockableMemoryRangeSPI_BAR0].End  = PchSpiBase + SIZE_4KB-1;

  DEBUG ((DEBUG_INFO, "Non lockable memory ranges:\n"));
  DumpRange (mNonLockableMemoryRange, NonLockableMemoryRangeMax);
  if (CheckOverlap (mNonLockableMemoryRange, NonLockableMemoryRangeMax) ||
      CheckOverlap2 (mLockableMemoryRange, LockableMemoryRangeMax, mNonLockableMemoryRange, NonLockableMemoryRangeMax)) {

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_CODE_3 ,HSTI_MEMORY_MAP_SECURITY_CONFIGURATION, HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION_ERROR_STRING_3);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  //
  // ALL PASS
  //
  if (Result) {
    Status = HstiLibSetFeaturesVerified (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               1,
               HSTI_BYTE1_SECURE_MEMORY_MAP_CONFIGURATION
               );
    ASSERT_EFI_ERROR (Status);
  }

  return;
}
