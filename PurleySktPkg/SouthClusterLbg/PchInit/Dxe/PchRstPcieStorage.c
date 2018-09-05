/** @file
  Configures the remapping for PCH Intel RST PCie Storage
  In order to use this feature, Intel RST Driver is required

@copyright
  Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/
#include "PchInit.h"

//
// Variables below indicate the memory range to be allocated to the PCIe storage device BAR usage when it is HIDDEN,
// thus it does not overlapped with the SGREG_BAR from the host perspective
//
#define RST_PCIE_STORAGE_MEMORY_START_RANGE         PCH_PCR_BASE_ADDRESS
#define RST_PCIE_STORAGE_MEMORY_END_RANGE           (PCH_PCR_BASE_ADDRESS + PCH_PCR_MMIO_SIZE - 1)

#define RST_PCIE_STORAGE_CYCLE_ROUTER_1             0
#define RST_PCIE_STORAGE_CYCLE_ROUTER_2             1
#define RST_PCIE_STORAGE_CYCLE_ROUTER_3             2
#define RST_PCIE_STORAGE_CYCLE_ROUTER_INVALID       99
#define RST_PCIE_STORAGE_INTERFACE_NONE             0
#define RST_PCIE_STORAGE_INTERFACE_AHCI             1
#define RST_PCIE_STORAGE_INTERFACE_NVME             2

#define PCI_CARD_PM_CAP_ID              0x01
#define PCI_CARD_MSIX_CAP_ID            0x11
#define PCI_CARD_SATA_CAP_ID            0x12
#define PCI_CARD_BAR_TOTAL              6
#define PCI_CARD_LINK_SPEED_GEN1_GEN2   0
#define PCI_CARD_LINK_SPEED_GEN3        1
#define PCI_CARD_LINK_WIDTH_1           0
#define PCI_CARD_LINK_WIDTH_2           1
#define PCI_CARD_LINK_WIDTH_4           2
#define PCI_CARD_REVISION               0x08
#define PCI_CARD_BASE_ADDR0             0x10
#define PCI_CARD_BASE_ADDR1             0x14
#define PCI_CARD_BASE_ADDR5             0x24
#define PCI_CLASS_MASS_STORAGE_NVME     0x08
#define PCI_CLASS_MASS_STORAGE_AHCI     PCI_CLASS_MASS_STORAGE_SATADPA

GLOBAL_REMOVE_IF_UNREFERENCED PCH_RST_PCIE_STORAGE_DETECTION mRstPcieStorageDetection [PCH_MAX_RST_PCIE_STORAGE_CR];

STATIC UINT32     PchRstPcieStorageCurrentMemoryRange   = RST_PCIE_STORAGE_MEMORY_START_RANGE;

STATIC UINT8      PchLpRstPcieStorageSupportedPort[]    = {
                                                          4,5,6,7,
                                                          8,9,10,11
                                                          };

STATIC UINT8      PchHRstPcieStorageSupportedPort[]     = {
                                                          8,9,10,11,
                                                          12,13,14,15,
                                                          16,17,18,19
                                                          };
//
// Lewisburg has enabled NVM Remapping on all Root Ports
//
STATIC UINT8      LbgRstPcieStorageSupportedPort[]      = {
                                                          0,1,2,3,
                                                          4,5,6,7,
                                                          8,9,10,11,
                                                          12,13,14,15,
                                                          16,17,18,19
                                                          };

/**
  Function to perform dump for PCH_RST_PCIE_STORAGE_DETECTION

  @param[in] Index      Index for RST PCIe Storage Cycle Router Instance

  @retval None
**/
VOID
PchRstPcieStorageRemappingDump (
  IN        UINTN              Index
  )
{
DEBUG_CODE_BEGIN();
  DEBUG ((DEBUG_INFO, "PchRstPcieStorageRemappingDump() Started\n"));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].SupportRstPcieStoragRemapping    =  %x\n", Index, mRstPcieStorageDetection[Index].SupportRstPcieStoragRemapping));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].RootPortNum                      =  %x\n", Index, mRstPcieStorageDetection[Index].RootPortNum));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].RootPortLane                     =  %x\n", Index, mRstPcieStorageDetection[Index].RootPortLane));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].DeviceInterface                  =  %x\n", Index, mRstPcieStorageDetection[Index].DeviceInterface));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].IsMsixSupported                  =  %x\n", Index, mRstPcieStorageDetection[Index].IsMsixSupported));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].MsixStartingVector               =  %x\n", Index, mRstPcieStorageDetection[Index].MsixStartingVector));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].MsixEndingVector                 =  %x\n", Index, mRstPcieStorageDetection[Index].MsixEndingVector));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].EndPointBarSize                  =  %x\n", Index, mRstPcieStorageDetection[Index].EndPointBarSize));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].EndPointUniqueMsixTableBar       =  %x\n", Index, mRstPcieStorageDetection[Index].EndPointUniqueMsixTableBar));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].EndPointUniqueMsixTableBarValue  =  %x\n", Index, mRstPcieStorageDetection[Index].EndPointUniqueMsixTableBarValue));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].EndPointUniqueMsixPbaBar         =  %x\n", Index, mRstPcieStorageDetection[Index].EndPointUniqueMsixPbaBar));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].EndPointUniqueMsixPbaBarValue    =  %x\n", Index, mRstPcieStorageDetection[Index].EndPointUniqueMsixPbaBarValue));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].EndPointBcc                      =  %x\n", Index, mRstPcieStorageDetection[Index].EndPointBcc));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].EndPointScc                      =  %x\n", Index, mRstPcieStorageDetection[Index].EndPointScc));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].EndPointPi                       =  %x\n", Index, mRstPcieStorageDetection[Index].EndPointPi));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].PchRstPcieStorageSaveRestore.PmCapPtr                    =  %x\n", Index, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PmCapPtr));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].PchRstPcieStorageSaveRestore.PcieCapPtr                  =  %x\n", Index, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PcieCapPtr));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].PchRstPcieStorageSaveRestore.L1ssCapPtr                  =  %x\n", Index, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.L1ssCapPtr));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].PchRstPcieStorageSaveRestore.EndpointL1ssControl2        =  %x\n", Index, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointL1ssControl2));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].PchRstPcieStorageSaveRestore.EndpointL1ssControl1        =  %x\n", Index, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointL1ssControl1));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].PchRstPcieStorageSaveRestore.LtrCapPtr                   =  %x\n", Index, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.LtrCapPtr));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].PchRstPcieStorageSaveRestore.EndpointLtrData             =  %x\n", Index, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointLtrData));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].PchRstPcieStorageSaveRestore.EndpointLctlData16          =  %x\n", Index, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointLctlData16));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].PchRstPcieStorageSaveRestore.EndpointDctlData16          =  %x\n", Index, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointDctlData16));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].PchRstPcieStorageSaveRestore.EndpointDctl2Data16         =  %x\n", Index, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointDctl2Data16));
  DEBUG ((DEBUG_INFO, "mRstPcieStorageDetection[%d].PchRstPcieStorageSaveRestore.RootPortDctl2Data16         =  %x\n", Index, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.RootPortDctl2Data16));
  DEBUG ((DEBUG_INFO, "PchRstPcieStorageRemappingDump() Ended\n"));
DEBUG_CODE_END();
}

/**
  Perform  memory space allocation or free memory pool for AHCI BAR

  @param[in]      Operation             True:  Perform memory space allocation for AHCI BAR
                                        False: Perform memory pool free for AHCI BAR
  @param[in,out]  MemBaseAddress        The base memory address used by AHCI BAR
**/
VOID
AbarMemorySpaceOperation (
  IN      BOOLEAN                             Operation,
  IN OUT  EFI_PHYSICAL_ADDRESS                *MemBaseAddress
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  BaseAddress;
  UINTN                 AhciBarAlignment;
  UINTN                 AhciBarLength;

  //
  // Assign a 512k size memory space to the ABAR when NVM Remapping is enabled
  //
  AhciBarAlignment = N_PCH_SATA_AHCI_BAR_ALIGNMENT_512K;     // 2^19: 512K Alignment
  AhciBarLength = V_PCH_SATA_AHCI_BAR_LENGTH_512K;           // 512K Length


  if (Operation == TRUE) {
    ///
    /// If MemBaseAddress is not allocated yet, allocate the AHCI BAR
    ///
    if ( (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchBottomUp) || (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchTopDown) ){
      BaseAddress = 0x0ffffffff;
    }
    Status = gDS->AllocateMemorySpace (
                    PcdGet8(PcdEfiGcdAllocateType),
                    EfiGcdMemoryTypeMemoryMappedIo,
                    AhciBarAlignment,
                    AhciBarLength,
                    &BaseAddress,
                    mImageHandle,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
    *MemBaseAddress = BaseAddress;
  } else {
    ///
    /// Else, free the GCD pool
    ///
    gDS->FreeMemorySpace (
          *MemBaseAddress,
          AhciBarLength
          );
  }
}

/**
  Configure the related Cycle Router in RST PCIe Storage's Cycle Router Global Control

  @param[in] SataRegBase            SATA Register Base
  @param[in] CycleRouterNumber      The RST PCIe Storage Cycle Router number

  @retval None
**/
VOID
SetAndSaveRstPcieStorageCycleRouter (
  IN UINTN            SataRegBase,
  IN UINTN            CycleRouterNumber
  )
{
  MmioWrite8 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CRGC, (UINT8) CycleRouterNumber);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CRGC),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CRGC)
    );
}

/**
  Configure the Power Management setting for RST PCIe Storage Remapping

  @retval None
**/
VOID
ConfigurePmForRstRemapping (
  VOID
  )
{
  UINTN               SataRegBase;
  UINTN               Index;
  PCH_STEPPING        PchStep;

  PchStep             = PchStepping ();
  SataRegBase         = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA);

  ///
  /// Step 2 For each RST PCIe Storage Cycle Router, program the "Extended General Configuration Register" field,
  ///
  for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
    SetAndSaveRstPcieStorageCycleRouter (SataRegBase, Index);
    DEBUG ((DEBUG_INFO, "ConfigurePmForRstRemapping: Low Power Programming - Recommanded Setting\n"));
    //
    // Program the SATA PCI offset 354h bit [20, 19, 17, 16] to [1b, 1b, 1b, 1b]
    //
    MmioOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_EGCR,
              B_PCH_RST_PCIE_STORAGE_EGCR_CRDCGE | B_PCH_RST_PCIE_STORAGE_EGCR_CRTCGE | B_PCH_RST_PCIE_STORAGE_EGCR_ICAS | B_PCH_RST_PCIE_STORAGE_EGCR_TSCAS);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_EGCR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_EGCR)
      );
  }
}

/**
  Configure the ASPM for Root Port and PCIe storage device before enabling RST PCIe Storage Remapping

  @param[in] PortNum                The Root Port that has PCIe storage device
  @param[in] TempPciBusMin          The temporary minimum Bus number for root port initialization
  @param[in] TempPciBusMax          The temporary maximum Bus number for root port initialization

  @retval None
**/
VOID
ConfigurePcieStorageDeviceAspm (
  IN UINT8                         PortNum,
  IN UINT8                         TempPciBusMin,
  IN UINT8                         TempPciBusMax
  )
{
  BOOLEAN                                   L1SubstatesSupportedPerPort;
  BOOLEAN                                   LtrSupported;
  PCH_PCIE_DEVICE_OVERRIDE                  *DevAspmOverride;
  UINT32                                    NumOfDevAspmOverride;
  UINTN                                     RpDevice;
  UINTN                                     RpFunction;

  DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: ConfigurePcieStorageDeviceAspm() Started\n"));

  DevAspmOverride                   = NULL;
  NumOfDevAspmOverride              = 0;
  L1SubstatesSupportedPerPort       = FALSE;
  LtrSupported                      = TRUE;

  GetPchPcieRpDevFun (PortNum, &RpDevice, &RpFunction);

  //
  // Set the ASPM for both root port and PCIe storage device
  //
  PcieSetPm (
    DEFAULT_PCI_BUS_NUMBER_PCH,
    (UINT8) RpDevice,
    (UINT8) RpFunction,
    NumOfDevAspmOverride,
    DevAspmOverride,
    TempPciBusMin,
    TempPciBusMax,
    &mPchPolicyHob->PcieConfig.RootPort[PortNum],
    &L1SubstatesSupportedPerPort,
    mPchPolicyHob->Revision,
    FALSE,
    FALSE,
    &LtrSupported,
    FALSE
    );

  DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: ConfigurePcieStorageDeviceAspm() Ended\n"));
}

/**
  Function to perform memory allocation for PCIe storage device that support unique BAR

  @param[in]        BarSize         The BAR size required for memory allocation
  @param[in,out]    AllocAddr       The Address that been allocated by this function

  @retval EFI_SUCCESS               The function completed successfully
  @retval EFI_OUT_OF_RESOURCES      Memory or storage is not enough
**/
EFI_STATUS
PchRstPcieStorageMemAllocation (
  IN        UINT32              BarSize,
  IN OUT    UINT32              *AllocAddr
  )
{
  if ((PchRstPcieStorageCurrentMemoryRange + BarSize) > RST_PCIE_STORAGE_MEMORY_END_RANGE) {
    return EFI_OUT_OF_RESOURCES;
  }

  if ((PchRstPcieStorageCurrentMemoryRange & (BarSize - 1)) != 0) {
    *AllocAddr      = (PchRstPcieStorageCurrentMemoryRange + BarSize) & ~(BarSize-1);
  } else {
    *AllocAddr      = PchRstPcieStorageCurrentMemoryRange;
  }

  PchRstPcieStorageCurrentMemoryRange       = *AllocAddr + BarSize;
  return EFI_SUCCESS;
}

/**
  Function to perform some configurations if RST PCIe Storage Remapping is Disabled

  @param[in] SataRegBase            SATA Register Base
  @param[in] Index                  Index of PCH_RST_PCIE_STORAGE_DETECTION struct

  @retval None
**/
VOID
DisableRstPcieStorageRemapping (
  IN UINTN                      SataRegBase,
  IN UINTN                      Index
  )
{
  PCH_STEPPING      PchStep;

  PchStep           = PchStepping ();

  DEBUG ((DEBUG_INFO, "DisableRstPcieStorageRemapping: DisableRstPcieStorageRemapping() Started\n"));

  SetAndSaveRstPcieStorageCycleRouter (SataRegBase, Index);

  ///
  /// Program Sata PCI offset 300h, bit[31:29] to '111b'
  ///
  MmioOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR, (UINT32) (B_PCH_RST_PCIE_STORAGE_GCR_CREL | B_PCH_RST_PCIE_STORAGE_GCR_RCL | B_PCH_RST_PCIE_STORAGE_GCR_PNCAIDL));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR)
    );
  //
  // Program Low Power settings
  //
  DEBUG ((DEBUG_INFO, "DisableRstPcieStorageRemapping: DisableRstPcieStorageRemapping() Ended\n"));
}

/**
  Function to perform lockdown if a RST PCIe Storage Cycle Router is unused/disabled

  @param[in] SataRegBase            SATA Register Base

  @retval None
**/
VOID
LockdownUnusedRstPcieStorageCycleRouter (
  IN UINTN                      SataRegBase
  )
{
  UINTN             Index;
  UINTN             LastEnabledCycleRouter;
  UINT32            Data32;

  DEBUG ((DEBUG_INFO, "LockdownUnusedRstPcieStorageCycleRouter: LockdownUnusedRstPcieStorageCycleRouter() Started\n"));


  ///
  /// Check if a RST PCIe Storage Cycle Router is enabled and locked or not
  /// If it is not enabled and not locked, perform the lockdown by programming the Sata PCI offset 300h, bit[31:29] to '111b'
  ///
  LastEnabledCycleRouter = 0;
  for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
    SetAndSaveRstPcieStorageCycleRouter (SataRegBase, Index);
    Data32 = MmioRead32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR) &
                         (UINT32) (B_PCH_RST_PCIE_STORAGE_GCR_CREL |
                                   B_PCH_RST_PCIE_STORAGE_GCR_RCL |
                                   B_PCH_RST_PCIE_STORAGE_GCR_PNCAIDL |
                                   B_PCH_RST_PCIE_STORAGE_GCR_CRE);
    if (Data32 == 0) {
      DEBUG ((DEBUG_INFO, "LockdownUnusedRstPcieStorageCycleRouter: RST PCIe Storage Cycle Router %d is not used and not locked\n", Index + 1));
      DEBUG ((DEBUG_INFO, "LockdownUnusedRstPcieStorageCycleRouter: Lockdown the unused RST PCIe Storage Cycle Router\n"));
      DisableRstPcieStorageRemapping (SataRegBase, Index);
    } else {
      LastEnabledCycleRouter = Index;
    }
  }

  ///
  /// After the lockdown, set the "Cycle Router Accessibility Select", SATA PCI offset FC0h [1:0] back to an enabled Cycle Router
  ///
  SetAndSaveRstPcieStorageCycleRouter (SataRegBase, LastEnabledCycleRouter);

  DEBUG ((DEBUG_INFO, "LockdownUnusedRstPcieStorageCycleRouter: LockdownUnusedRstPcieStorageCycleRouter() Ended\n"));
}

/**
  Program and remap the PCIe storage device Sata Capability

  @param[in] EndPointPciBase      Endpoint Base Address
  @param[in] SataRegBase          Sata Base Address
  @param[in] CapPtr               Capability Pointer to Endpoint's Sata Capability

  @retval None
**/
VOID
RemapDeviceSataCap (
  IN UINTN          EndPointPciBase,
  IN UINTN          SataRegBase,
  IN UINT8          CapPtr
  )
{
  UINT32    Data32Or;
  UINT32    Data32And;

  ///
  /// Setup  and enable Sata Capability Remap
  /// Program the PCIe storage device's Sata capability offset to Sata PCI offset 338h[23:16] and set bit[31] = 1b
  ///
  Data32Or  = (UINT32) ((CapPtr << N_PCH_RST_PCIE_STORAGE_MXCRCC_TCSO) | B_PCH_RST_PCIE_STORAGE_AIDPCRC_RE);
  Data32And = (UINT32) ~(B_PCH_RST_PCIE_STORAGE_AIDPCRC_RE | B_PCH_RST_PCIE_STORAGE_AIDPCRC_TCSO);
  MmioAndThenOr32 (
    SataRegBase + R_PCH_RST_PCIE_STORAGE_AIDPCRC,
    Data32And,
    Data32Or
    );
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_AIDPCRC),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_AIDPCRC)
    );

  ///
  /// Setup and enable I/O BAR Remap by programming Sata PCI offset 320h[31] = 1b
  ///
  MmioOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_IOBRSC, (UINT32) B_PCH_RST_PCIE_STORAGE_IOBRSC_RE);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_IOBRSC),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_IOBRSC)
    );
}

/**
  Program and remap the PCIe storage device MSI-X Capability

  @param[in]        EndPointPciBase         Endpoint Base Address
  @param[in]        SataRegBase             Sata Base Address
  @param[in]        CapPtr                  Capability Pointer to Endpoint's MSI-X Capability
  @param[in]        Index                   Index of PCH_RST_PCIE_STORAGE_DETECTION struct
  @param[in, out]   MsixCurrentVector       Value of Current Vector for MSI-X table size

  @retval None
**/
VOID
RemapDeviceMsixCap (
  IN UINTN          EndPointPciBase,
  IN UINTN          SataRegBase,
  IN UINT8          CapPtr,
  IN UINTN          Index,
  IN OUT UINT16     *MsixCurrentVector
  )
{
  EFI_STATUS        Status;
  UINT32            Data32Or;
  UINT32            Data32And;
  UINT32            MsixBaseAddr;
  UINT32            PbaBaseAddr;
  UINT32            DeviceBarSize;
  UINT32            DeviceTableOffsetAndBir;
  UINT32            DevicePbaOffsetAndBir;
  UINT16            MsixTableSize;
  UINT16            StartingVector;
  UINT16            EndingVector;
  UINT8             TableBir;
  UINT8             PbaBir;
  BOOLEAN           IsMsixBarUnique;
  BOOLEAN           IsPbaBarUnique;

  MsixBaseAddr      = 0;
  PbaBaseAddr       = 0;
  IsMsixBarUnique   = FALSE;
  IsPbaBarUnique    = FALSE;

  ///
  /// Get the PCIe storage device MSI-X table size
  ///
  MsixTableSize     = MmioRead16 (EndPointPciBase + CapPtr + 0x02) & 0x7FF;

  ///
  /// Calculate the Starting vecotr and Ending vector
  ///
  StartingVector    = 1 + *MsixCurrentVector;
  EndingVector      = StartingVector + MsixTableSize;

  ///
  /// If the ending vector is overflowed (> 2047), skip this port from RST PCIe Storage Remapping
  ///
  if (EndingVector > 2047) {
    goto Exit;
  }

  ///
  /// Read the PCIe storage device's table offset, table BIR and check if it supports unique MSI-X Table BAR
  ///
  DeviceTableOffsetAndBir  = MmioRead32 (EndPointPciBase + CapPtr + 0x04);
  TableBir = (UINT8) DeviceTableOffsetAndBir & (BIT2|BIT1|BIT0);
  if (((TableBir != 0) && (mRstPcieStorageDetection[Index].DeviceInterface == RST_PCIE_STORAGE_INTERFACE_NVME)) ||
        ((TableBir != 5) && (mRstPcieStorageDetection[Index].DeviceInterface == RST_PCIE_STORAGE_INTERFACE_AHCI))) {
    IsMsixBarUnique     = TRUE;

    ///
    /// Allocate PCH Reserved Memory Pool for PCIe storage device if it has unique MSI-X BAR
    ///
    MmioWrite32 (EndPointPciBase + PCI_CARD_BASE_ADDR0 + (0x04 * TableBir), 0xFFFFFFFF);
    DeviceBarSize       = MmioRead32 (EndPointPciBase + PCI_CARD_BASE_ADDR0 + (0x04 * TableBir));
    DeviceBarSize       &= 0xFFFFFFF0;
    DeviceBarSize       = ~DeviceBarSize + 1;
    Status              = PchRstPcieStorageMemAllocation (DeviceBarSize,&MsixBaseAddr);

    ///
    /// Skip this port from RST PCIe Storage Remapping if it is running out of resource
    ///
    if (Status == EFI_OUT_OF_RESOURCES) {
      goto Exit;
    }
  }

  ///
  /// Read the PCIe storage device's PBA offset, PBA BIR and check if it supports unique MSI-X PBA BAR
  ///
  DevicePbaOffsetAndBir  = MmioRead32 (EndPointPciBase + CapPtr + 0x08);
  PbaBir = (UINT8) DevicePbaOffsetAndBir & (BIT2|BIT1|BIT0);
  if ((((PbaBir != 0) && (mRstPcieStorageDetection[Index].DeviceInterface == RST_PCIE_STORAGE_INTERFACE_NVME)) ||
        ((PbaBir != 5) && (mRstPcieStorageDetection[Index].DeviceInterface == RST_PCIE_STORAGE_INTERFACE_AHCI))) &&
        (PbaBir != TableBir)) {
    IsPbaBarUnique      = TRUE;

    ///
    /// Allocate PCH Reserved Memory Pool for PCIe storage device if it has unique PBA BAR
    ///
    MmioWrite32 (EndPointPciBase + PCI_CARD_BASE_ADDR0 + (0x04 * PbaBir), 0xFFFFFFFF);
    DeviceBarSize       = MmioRead32 (EndPointPciBase + PCI_CARD_BASE_ADDR0 + (0x04 * PbaBir));
    DeviceBarSize       &= 0xFFFFFFF0;
    DeviceBarSize       = ~DeviceBarSize + 1;
    Status              = PchRstPcieStorageMemAllocation (DeviceBarSize,&PbaBaseAddr);

    ///
    /// Skip this port from RST PCIe Storage Remapping if it is running out of resource
    ///
    if (Status == EFI_OUT_OF_RESOURCES) {
     goto Exit;
    }
  }

  ///
  /// Update the MSI-X Current Vector
  ///
  *MsixCurrentVector   = EndingVector;

  ///
  /// Update IsMsixSupported flag and keep the PCIe storage device Starting vector and Ending vector for later steps
  ///
  mRstPcieStorageDetection[Index].IsMsixSupported     = TRUE;
  mRstPcieStorageDetection[Index].MsixStartingVector  = StartingVector;
  mRstPcieStorageDetection[Index].MsixEndingVector    = EndingVector;

  ///
  /// If PCIe storage device supports unique MSI-X BAR, store the BAR and BIR for later step usage
  /// Program the "MSI-X Table Base Address Register", SATA PCI offset 344h[31:01] to the allocated value and set its bit[0]
  ///
  if (IsMsixBarUnique) {
    mRstPcieStorageDetection[Index].EndPointUniqueMsixTableBarValue     = MsixBaseAddr;
    mRstPcieStorageDetection[Index].EndPointUniqueMsixTableBar          = (UINT32) PCI_CARD_BASE_ADDR0 + (0x04 * TableBir);
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXTBAR, (MsixBaseAddr | B_PCH_RST_PCIE_STORAGE_MXTBAR_TBAV));
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXTBAR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXTBAR)
      );
  }

  ///
  /// If PCIe storage device supports unique MSI-X PBA BAR, store the BAR and BIR for later step usage
  /// Program the "MSI-X PBA Base Address Register", SATA PCI offset 34Ch[31:01] to the allocated value and set its bit[0]
  ///
  if (IsPbaBarUnique) {
    mRstPcieStorageDetection[Index].EndPointUniqueMsixPbaBarValue     = PbaBaseAddr;
    mRstPcieStorageDetection[Index].EndPointUniqueMsixPbaBar          = (UINT32) PCI_CARD_BASE_ADDR0 + (0x04 * PbaBir);
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXPBAR, (PbaBaseAddr | B_PCH_RST_PCIE_STORAGE_MXPBAR_TBAV));
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXPBAR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXPBAR)
      );
  }

  ///
  /// Program the PCIe storage device's table offset, table BIR to "MSI-X Table Remap Configuration",
  /// Sata PCI offset 340h [31:03, 2:0] accordingly
  ///
  MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXTRC, DeviceTableOffsetAndBir);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXTRC),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXTRC)
    );

  ///
  /// Program the PCIe storage device's PBA offset, PBA BIR to "MSI-X PBA Remap Configuration",
  /// Sata PCI offset 348h [31:03, 2:0] accordingly
  ///
  MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXPRC, DevicePbaOffsetAndBir);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXPRC),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXPRC)
    );

  ///
  /// Setup  and enable MSI-X Capability Remap
  /// Program the PCIe storage device's MSI-X capability offset to Sata PCI offset 33Ch[23:16] and set bit[31] = 1b
  ///
  Data32Or  = (UINT32) ((CapPtr << N_PCH_RST_PCIE_STORAGE_MXCRCC_TCSO) | B_PCH_RST_PCIE_STORAGE_MXCRC_RE);
  Data32And = (UINT32) ~(B_PCH_RST_PCIE_STORAGE_MXCRC_RE | B_PCH_RST_PCIE_STORAGE_MXCRCC_TCSO);
  MmioAndThenOr32 (
    SataRegBase + R_PCH_RST_PCIE_STORAGE_MXCRC,
    Data32And,
    Data32Or
    );
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXCRC),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MXCRC)
    );

  return;

Exit:
  mRstPcieStorageDetection[Index].SupportRstPcieStoragRemapping     = FALSE;
  return;
}

/**
  Check if a root port is supporting RST PCIe Storage Remapping

  @param[in] RootPortNum            Root Port Number

  @retval TRUE                      Root Port supports RST PCIe Storage Remapping
  @retval FALSE                     Root Port doesn't support RST PCIe Storage Remapping
**/
BOOLEAN
IsRootPortSupportRemapping (
  IN  UINT8                             RootPortNum
  )
{
  PCH_SERIES              PchSeries;
  UINTN                   Index;

  PchSeries               = GetPchSeries ();

  if (PchSeries == PchLp) {
    for (Index = 0; Index < sizeof(PchLpRstPcieStorageSupportedPort); Index++) {
      if (PchLpRstPcieStorageSupportedPort[Index] == RootPortNum) {
        return TRUE;
      }
    }
  } else {
    for (Index = 0; Index < sizeof(LbgRstPcieStorageSupportedPort); Index++) {
      if (LbgRstPcieStorageSupportedPort[Index] == RootPortNum) {
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
  Check the lane occupied by certain root port according to the root port number and configuration strap
  Return 8-bit bitmap where each bit represents the lane number (e.g.: return 00000011b means the root port owns 2 lane)

  @param[in] RootPortNum            Root Port Number

  @retval UINT8                     Lane Occupied by the Root Port (bitmap)
**/
UINT8
CheckRootPortLaneOccupied (
  IN  UINT8                    RootPortNum
  )
{
  EFI_STATUS              Status;
  UINTN                   PciRootPortRegBase;
  UINTN                   RpDev;
  UINTN                   RpFunc;
  UINT32                  Data32;
  UINT8                   LaneOccupied;

  LaneOccupied       = 0;

  Status = GetPchPcieRpDevFun (RootPortNum, &RpDev, &RpFunc);
  ASSERT_EFI_ERROR (Status);
  PciRootPortRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32) RpDev, (UINT32) RpFunc);

  ///
  /// Read the Root Port Configuration Straps for the link width, and return LaneOccupied by the Root Port accordingly
  ///
  Data32 = MmioRead32 (PciRootPortRegBase + R_PCH_PCIE_STRPFUSECFG);
  switch ((Data32 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC) {
    case V_PCH_PCIE_STRPFUSECFG_RPC_4:
      if (RootPortNum % 4 == 0) {
        LaneOccupied = (BIT3|BIT2|BIT1|BIT0);
      }
      break;
    case V_PCH_PCIE_STRPFUSECFG_RPC_2_2:
      if ((RootPortNum % 2 == 0)) {
        LaneOccupied = (BIT1|BIT0);
      }
      break;
    case V_PCH_PCIE_STRPFUSECFG_RPC_2_1_1:
      if (RootPortNum % 4 == 0) {
        LaneOccupied = (BIT1|BIT0);
      } else if (RootPortNum % 4 != 1) {
        LaneOccupied = BIT0;
      }
      break;
    case V_PCH_PCIE_STRPFUSECFG_RPC_1_1_1_1:
      LaneOccupied = BIT0;
      break;
    default:
      break;
  }

  return LaneOccupied;
}

/**
  Check the RST PCIe Storage Cycle Router number according to the root port number and PCH type

  @param[in] RootPortNum            Root Port Number

  @retval UINT8                     The RST PCIe Storage Cycle Router Number
**/
UINT8
CheckCycleRouterNum (
  IN  UINT8                    RootPortNum
  )
{
  PCH_SERIES        PchSeries;

  PchSeries         = GetPchSeries ();

  ///
  /// Check if the PCH is LP or H
  ///
  /// SKL PCH-LP RST PCIe Storage Cycle Router Assignment:
  /// i.)   Cycle Router 2 -> RP5 - RP8
  /// ii.)  Cycle Router 3 -> RP9 - RP12
  ///
  /// SKL PCH-H RST PCIe Storage Cycle Router Assignment:
  /// i.)   Cycle Router 1 -> RP9  - RP12
  /// ii.)  Cycle Router 2 -> RP13 - RP16
  /// iii.) Cycle Router 3 -> RP17 - RP20
  ///
  if (PchSeries == PchLp) {
    if (RootPortNum < 4) {
      return RST_PCIE_STORAGE_CYCLE_ROUTER_INVALID;
    } else if (RootPortNum < 8) {
      return RST_PCIE_STORAGE_CYCLE_ROUTER_2;
    } else if (RootPortNum < 12) {
      return RST_PCIE_STORAGE_CYCLE_ROUTER_3;
    } else {
      return RST_PCIE_STORAGE_CYCLE_ROUTER_INVALID;
    }
  } else {
    if (RootPortNum > 15) {
      return RST_PCIE_STORAGE_CYCLE_ROUTER_3;
    } else if (RootPortNum > 11) {
      return RST_PCIE_STORAGE_CYCLE_ROUTER_2;
    } else if (RootPortNum > 7) {
      return RST_PCIE_STORAGE_CYCLE_ROUTER_1;
    } else {
      return RST_PCIE_STORAGE_CYCLE_ROUTER_INVALID;
    }
  }
}

/**
  Check and detect PCIe storage device per policies, and if existed, indicate if it should be subjected for remapping
  Populate the  mRstPcieStorageDetection structure along with it for later usages

  @param[in, out]   RemappingRequired           Boolean value with TRUE indicates that RST PCIe Storage Remapping is required
  @param[in]        TempPciBusMin               The temporary minimum Bus number for root port initialization
  @param[in]        SataRegBase                 SATA Register Base

  @retval None
**/
VOID
DetectPcieStorageDevices (
  IN OUT BOOLEAN                        *RemappingRequired,
  IN UINT8                              TempPciBusMin,
  IN UINTN                              SataRegBase
  )
{
  EFI_STATUS              Status;
  PCH_SERIES              PchSeries;
  UINT8                   DeviceInterface;
  UINT8                   PortNum;
  UINTN                   PciRootPortRegBase;
  UINTN                   EndPointPciBase;
  UINTN                   RpDev;
  UINTN                   RpFunc;
  UINT8                   TempRootPortBusNum;
  UINT16                  Data16;
  UINT32                  Data32;
  UINT8                   CycleRouterNum;
  UINT8                   RootPortLane;

  PchSeries               = GetPchSeries ();
  TempRootPortBusNum      = TempPciBusMin;

  DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: DetectPcieStorageDevices Started\n"));

  for (PortNum = 0; PortNum < GetPchMaxPciePortNum (); PortNum++) {

    if (!(IsRootPortSupportRemapping (PortNum))) {
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: IsRootPortSupportRemapping @ port%d = FALSE, proceed to next port\n", PortNum + 1));
      continue;
    }

    Status = GetPchPcieRpDevFun (PortNum, &RpDev, &RpFunc);
    ASSERT_EFI_ERROR (Status);

    PciRootPortRegBase  = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32) RpDev, (UINT32) RpFunc);
    CycleRouterNum      = CheckCycleRouterNum (PortNum);
    RootPortLane        = CheckRootPortLaneOccupied (PortNum);


    if (CycleRouterNum == RST_PCIE_STORAGE_CYCLE_ROUTER_INVALID) {
      continue;
    }

    ///
    /// Check if the RST PCIe Storage Cycle Router is enabled, continue to the next root port in the next x4 lane if it is disabled
    ///
    if (mPchPolicyHob->SataConfig.RstPcieStorageRemap[CycleRouterNum].Enable == 0)
    {
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: RST PCIe Storage Cycle Router %d is disabled\n", CycleRouterNum));
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: Proceed to root port in the next x4 lane (@ port %d)\n", PortNum + 1));
      PortNum += 3 - (PortNum % 4);
      continue;
    }

    ///
    /// Check if the current root port is matching selected root port from policy, continue to next port if they are not matched
    ///
    if ((mPchPolicyHob->SataConfig.RstPcieStorageRemap[CycleRouterNum].RstPcieStoragePort != 0) &&
        (mPchPolicyHob->SataConfig.RstPcieStorageRemap[CycleRouterNum].RstPcieStoragePort != (UINT32) (PortNum + 1)))
    {
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: root port %d not matched with selected root port, proceed to next port\n", PortNum + 1));
      continue;
    }

    ///
    /// Read Sata PCI offset FC0h bit[16+n] to check if CR# is fused enabled
    /// Skip the related port(s) under the same CR# if its CR# is fuse disabled and move to the root port in the next x4 lane
    ///
    Data32 = MmioRead32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CRGC) & (BIT16 << CycleRouterNum);
    if (Data32 == 0) {
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: RST PCIe Storage Cycle Router %d is fuse disabled\n", CycleRouterNum));
      DEBUG ((DEBUG_INFO, "Proceed to root port in the next x4 lane (@ port %d)\n", PortNum + 1));
      PortNum += 3 - (PortNum % 4);
      continue;
    }


    ///
    /// Set RST PCIe Storage Cycle Router Accessibility Select, Sata offset FC0h bit [1:0] based on CR#
    ///
    MmioWrite8 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CRGC, CycleRouterNum);

    ///
    /// Check if root port exists/enable and if it is remap-capable (via GSR.PLRC field)
    /// Continue to next port if
    /// i.)   root port doesn't exit or enable
    /// ii.)  root port lane is not occupied
    ///
    Data16 = MmioRead16 (PciRootPortRegBase + PCI_VENDOR_ID_OFFSET);
    if ((Data16 == 0xFFFF) || (RootPortLane == 0))
    {
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: proceed to next port due to either:\n"));
      DEBUG ((DEBUG_INFO, "1.) root port doesn't exit, 2.) root port is not remap-capable, 3.) root port lane is not occupied\n"));
      continue;
    }

    ///
    /// Check for mismatching configuration
    /// Set "PCIe Port Selected" Bit[RP#], Sata PCI offset 300h[RP#] to 1
    ///
    MmioAndThenOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR, (UINT32)~(B_PCH_RST_PCIE_STORAGE_GCR_PLS), (UINT32) RootPortLane << (PortNum + 1));

    ///
    /// Clear PCCS, Sata PCI offset 304h [30], then read the value of PCCS
    ///
    MmioOr32(SataRegBase + R_PCH_RST_PCIE_STORAGE_GSR, (UINT32) B_PCH_RST_PCIE_STORAGE_GSR_PCCS);
    Data32 = MmioRead32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_GSR) & (UINT32) B_PCH_RST_PCIE_STORAGE_GSR_PCCS;

    ///
    /// Clear "PCIe Port Selected", Sata PCI offset 300h [20:1] to 0
    ///
    MmioAnd32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR, (UINT32)~(B_PCH_RST_PCIE_STORAGE_GCR_PLS));

    ///
    /// If PCCS value is 1, a mismatch configuration is occurred, skip this root port and proceed to next root port
    ///
    if (Data32) {
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: RP# %d width line not matching the CR# strap\n", PortNum + 1));
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: Removed it from supported port\n"));
      continue;
    }

    ///
    /// Assign temporary bus number to root port
    ///
    MmioAndThenOr32 (
      PciRootPortRegBase + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET,
      (UINT32)~B_PCI_BRIDGE_BNUM_SBBN_SCBN,
      ((UINT32) (TempRootPortBusNum << 8))
        | ((UINT32) (TempRootPortBusNum << 16))
    );

    ///
    /// A config write is required in order for the device to re-capture the Bus number,
    /// according to PCI Express Base Specification, 2.2.6.2
    /// Write to a read-only register VendorID to not cause any side effects.
    ///
    EndPointPciBase = MmPciBase (TempRootPortBusNum, 0, 0);
    MmioWrite16 (EndPointPciBase + PCI_VENDOR_ID_OFFSET, 0);

    ///
    /// Initial Device Interface variable
    ///
    DeviceInterface = RST_PCIE_STORAGE_INTERFACE_NONE;

    ///
    /// Read Vendor ID to check if device exists
    /// if no device exists, then check next device
    ///
    Data16 = MmioRead16 (EndPointPciBase + PCI_VENDOR_ID_OFFSET);

    if (Data16 == 0xFFFF) {
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: No card detected on root port\n"));

      ///
      /// Clear bus numbers and continue to next port
      ///
      MmioAnd32 (PciRootPortRegBase + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, (UINT32)~B_PCI_BRIDGE_BNUM_SBBN_SCBN);
      continue;
    }

    ///
    /// Check for PCIe storage device (AHCI, RAID or NVM)
    ///
    if (((MmioRead8 (EndPointPciBase + R_PCI_BCC_OFFSET)) == PCI_CLASS_MASS_STORAGE) &&
        ((MmioRead8 (EndPointPciBase + R_PCI_SCC_OFFSET)) == PCI_CLASS_MASS_STORAGE_AHCI) &&
        ((MmioRead8 (EndPointPciBase + R_PCI_PI_OFFSET)) == 0x01))
    {
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: AHCI Card found\n"));
      DeviceInterface = RST_PCIE_STORAGE_INTERFACE_AHCI;
    }

    if (((MmioRead8 (EndPointPciBase + R_PCI_BCC_OFFSET)) == PCI_CLASS_MASS_STORAGE) &&
        ((MmioRead8 (EndPointPciBase + R_PCI_SCC_OFFSET)) == PCI_CLASS_MASS_STORAGE_RAID) &&
        ((MmioRead16 (EndPointPciBase + PCI_VENDOR_ID_OFFSET)) == V_PCH_INTEL_VENDOR_ID))
    {
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: RAID Card found\n"));
      DeviceInterface = RST_PCIE_STORAGE_INTERFACE_AHCI;
    }

    if (((MmioRead8 (EndPointPciBase + R_PCI_BCC_OFFSET)) == PCI_CLASS_MASS_STORAGE) &&
        ((MmioRead8 (EndPointPciBase + R_PCI_SCC_OFFSET)) == PCI_CLASS_MASS_STORAGE_NVME) &&
        ((MmioRead8 (EndPointPciBase + R_PCI_PI_OFFSET)) == 0x02))
    {
      DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: NVMe Card found\n"));
      DeviceInterface = RST_PCIE_STORAGE_INTERFACE_NVME;
    }


    ///
    /// Clear bus numbers
    ///
    MmioAnd32 (PciRootPortRegBase + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, (UINT32)~B_PCI_BRIDGE_BNUM_SBBN_SCBN);

    ///
    /// Update the remapping detail for detected PCIe storage device, and move to the root port in the next x4 lane
    ///
    if ((DeviceInterface != RST_PCIE_STORAGE_INTERFACE_NONE)) {
      mRstPcieStorageDetection[CycleRouterNum].SupportRstPcieStoragRemapping     = TRUE;
      mRstPcieStorageDetection[CycleRouterNum].RootPortNum                       = PortNum;
      mRstPcieStorageDetection[CycleRouterNum].RootPortLane                      = RootPortLane;
      mRstPcieStorageDetection[CycleRouterNum].DeviceInterface                   = DeviceInterface;
      PortNum                       += 3 - (PortNum % 4);;
      *RemappingRequired      = TRUE;
    } else {
      continue;
    }
  }

  DEBUG ((DEBUG_INFO, "DetectPcieStorageDevices: DetectPcieStorageDevices() Ended\n"));
}

/**
  Function to perform late configuration for RST PCIe Storage Remapping

  @param[in] SataRegBase                SATA Register Base
  @param[in] AhciBar                    AHCI Base Address
  @param[in] Index                      Index of PCH_RST_PCIE_STORAGE_DETECTION struct
  @param[in,out] RemapDeviceLinkInfo    Variable to store link info (link speed, link width) for remapped device

  @retval EFI_SUCCESS                   The function completed successfully
**/
EFI_STATUS
RstPcieStorageRemappingLateConfig (
  IN UINTN                                  SataRegBase,
  IN UINT32                                 AhciBar,
  IN UINTN                                  Index,
  IN OUT UINT16                             *RemapDeviceLinkInfo
  )
{
  UINTN                 BarIndex;
  UINT32                BaseAddr;
  UINT32                Timeout;
  UINT32                Data32;
  UINT16                Data16;
  PCH_STEPPING          PchStep;
  PCH_SERIES            PchSeries;
  UINT8                 LinkWidth;

  DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: RstPcieStorageRemappingLateConfig() Started\n"));

DEBUG_CODE_BEGIN();
  DEBUG ((DEBUG_INFO, "=====================================================\n"));
  DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: RstPcieStorageRemappingLateConfig() Dump\n"));
  PchRstPcieStorageRemappingDump(Index);
  DEBUG ((DEBUG_INFO, "=====================================================\n"));
DEBUG_CODE_END();

  SetAndSaveRstPcieStorageCycleRouter (SataRegBase, Index);

  ///
  /// Step 22.Clear all BARs within endpoint to 0
  ///
  BaseAddr = PCI_CARD_BASE_ADDR0;
  for (BarIndex = 0; BarIndex < PCI_CARD_BAR_TOTAL; BarIndex++) {
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR, BaseAddr);
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR, 0x0);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR)
      );
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR)
      );

    BaseAddr += 0x4;
  }

  ///
  /// Step 23.After the link reenabled and the 100ms (tunable) delay,
  /// If L1 Sub-Stated is supported, restore saved endpoint's L1 Sub-States Extended Capability, offset 0x0C and 0x08
  ///
  if (mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.L1ssCapPtr != 0) {
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR, (UINT32) mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.L1ssCapPtr + 0x0C);
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR, (UINT32) mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointL1ssControl2);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR)
      );
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR)
      );

    Data32 = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointL1ssControl1 & ~ (0xF);
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR, (UINT32) mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.L1ssCapPtr + 0x08);
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR, (UINT32) Data32);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR)
      );
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR)
      );

    Data32 = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointL1ssControl1;
    MmioWrite32 ((UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR), (UINT32) Data32);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR)
      );
  }

  ///
  /// Step 23.After the link reenabled and the 100ms (tunable) delay,
  /// If LTR is supported, restore saved endpoint's LTR Extended Capability, offset 0x04 [31:0]
  ///
  if (mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.LtrCapPtr != 0) {
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR, (UINT32) mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.LtrCapPtr + 0x04);
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR, (UINT32) mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointLtrData);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR)
      );
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR)
      );
  }

  ///
  /// Step 23.Restore the following endpoint's registers
  /// i.)   endpoint's Link Control's "Enable Clock Power Management" field and "Common Clock Configuration" field
  /// ii.)  endpoint's Device Control 2 for "LTR Mechanism Enable" field
  /// iii.) endpoint's Device Control for "Max Payload Size" field
  ///
  MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PcieCapPtr + 0x10);
  MmioAndThenOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR,
                   (UINT32) ~(BIT6 | BIT8),
                   (UINT32) (mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointLctlData16 & (~B_PCIE_LCTL_ASPM)));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR)
    );
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR)
    );

  MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PcieCapPtr + 0x28);
  MmioAndThenOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR,
                   (UINT32) ~(BIT10),
                   (UINT32) (mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointDctl2Data16));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR)
    );
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR)
    );

  MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PcieCapPtr + 0x8);
  MmioAndThenOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR,
                   (UINT32) ~(BIT5 | BIT6 | BIT7),
                   (UINT32) (mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointDctlData16));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR)
    );
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR)
    );

  ///
  /// Step 23.Restore the root port's Device Control 2  "LTR Mechanism Enable" field, offset 68h[10]
  ///
  MmioAndThenOr32 (SataRegBase + R_PCH_PCIE_DCTL2 + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET,
                   (UINT32) ~(B_PCIE_DCTL2_LTREN),
                   (UINT32) (mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.RootPortDctl2Data16));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_PCIE_DCTL2 + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_PCIE_DCTL2 + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET)
    );

  ///
  /// Step 23.If the "Common Clock Configuration" field is set, perform link retrain by setting the "Retrain Link" bit, Sata PCI offset 150h[5] to 1b
  ///
  if ((mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointLctlData16 & B_PCIE_LCTL_CCC) == B_PCIE_LCTL_CCC) {

    MmioOr16 (SataRegBase + R_PCH_PCIE_LCTL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET, (UINT16) (B_PCIE_LCTL_RL));
    Data16 = MmioRead16 (SataRegBase + R_PCH_PCIE_LCTL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET);
    Data16 |= B_PCIE_LCTL_RL;
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint16,
      (UINTN) (SataRegBase + R_PCH_PCIE_LCTL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET),
      1,
      &Data16
      );

    ///
    /// Poll PCIe Link Active status till it is active
    ///
    Timeout = 0;
    Data16 = MmioRead16 ((SataRegBase + R_PCH_PCIE_LSTS + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET)) & B_PCIE_LSTS_LA;
    while (Data16 == 0) {
      MicroSecondDelay (15);
      Timeout += 15;
      Data16 = MmioRead16 ((SataRegBase + R_PCH_PCIE_LSTS + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET)) & B_PCIE_LSTS_LA;
      if ((Timeout > 30000)) {
        DEBUG ((DEBUG_INFO, "RstPcieStorageRemappingLateConfig: Timeout (exceed 30ms) when polling for link active\n"));
        break;
      }
    }

    Data16 = B_PCIE_LSTS_LA;
    S3BootScriptSaveMemPoll (
      S3BootScriptWidthUint16,
      (UINTN) (SataRegBase + R_PCH_PCIE_LSTS + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET),
      &Data16,
      &Data16,
      15,
      2000
      );
  }

  ///
  /// Step 23 Restore the endpoint's Link Control's "Active State Link PM Control" field
  ///
  MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR, mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PcieCapPtr + 0x10);
  MmioAndThenOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR,
                   (UINT32) ~(BIT0 | BIT1),
                   (UINT32) (mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointLctlData16 & B_PCIE_LCTL_ASPM));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR)
    );
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR)
    );

  ///
  /// Disable MSE, read and write back ABAR, then enable MSE
  ///
  MmioAnd16 (SataRegBase + PCI_COMMAND_OFFSET, (UINT16) ~EFI_PCI_COMMAND_MEMORY_SPACE);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (SataRegBase + PCI_COMMAND_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + PCI_COMMAND_OFFSET)
    );

  Data32 = MmioRead32 (SataRegBase + R_PCH_SATA_AHCI_BAR);
  MmioWrite32 (SataRegBase + R_PCH_SATA_AHCI_BAR, (UINT32) Data32);
  S3BootScriptSaveMemReadWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_SATA_AHCI_BAR),
    &Data32,
    0
    );

  MmioOr16 (SataRegBase + PCI_COMMAND_OFFSET, (UINT16) EFI_PCI_COMMAND_MEMORY_SPACE);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (SataRegBase + PCI_COMMAND_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + PCI_COMMAND_OFFSET)
    );

  ///
  /// Step 23 If endpoint supports MSI-X, perform the following:
  /// i.)  program endpoint starting and ending vector to AHCIBar + offset 808h + n*80h, bit[10:0] and bit[26:16]
  /// ii.) enable MSI-X Interrupt delivery by programming "Interrupt Delivery", AHCIBar + offset 808h + n*80h + 4h, bit[31:30]
  if (mRstPcieStorageDetection[Index].IsMsixSupported) {
    Data32 = (mRstPcieStorageDetection[Index].MsixEndingVector << N_PCH_RST_PCIE_STORAGE_CR1_DMXC_MXEV) | mRstPcieStorageDetection[Index].MsixStartingVector;
    MmioWrite32 (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DMXC + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR), Data32);
    MmioOr32 (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DMXC + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR) + 0x04, BIT31);
  } else {
    ///
    /// Step 24 Else, program "Device MSI-X Configuration", AHCIBar + offset 808h + n*80h, bit [31:0], and
    /// AHCIBar + offset 808h bit [31:0], and n*80h + 4h, bit [31:0] = 0;
    ///
    MmioWrite32 (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DMXC + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR), 0);
    MmioWrite32 (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DMXC + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR) + 0x04, 0);
  }

  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DMXC + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR)),
    1,
    (VOID *) (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DMXC + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR))
    );
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DMXC + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR) + 0x04),
    1,
    (VOID *) (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DMXC + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR) + 0x04)
    );

  ///
  /// Setup a 32-bit data for RWO usage later
  /// Refer to CC, SCC, PI from step 7.d, set up bit[23:0] accordingly
  /// If an AHCI device is detected, also set the "Device Type" (bit31)
  ///
  Data32 = (mRstPcieStorageDetection[Index].EndPointBcc << N_PCH_RST_PCIE_STORAGE_CR1_DCC_BCC) | (mRstPcieStorageDetection[Index].EndPointScc << N_PCH_RST_PCIE_STORAGE_CR1_DCC_SCC) | (mRstPcieStorageDetection[Index].EndPointPi);
  if (mRstPcieStorageDetection[Index].DeviceInterface == RST_PCIE_STORAGE_INTERFACE_AHCI) {
    Data32 |= B_PCH_RST_PCIE_STORAGE_CR1_DCC_DT;
  }

  ///
  /// Step 25 Program the endpoint "Device Class Code", AHCIBar + offset 800h + n*80h with the 32-bit data
  ///
  MmioWrite32 (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DCC + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR), Data32);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DCC + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR)),
    1,
    (VOID *) (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DCC + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR))
    );

  ///
  /// Step 26 Read the remapped root ports Link Status, and update the RemapDeviceLinkInfo according on the "Negotiated Link Width" and "Current Link Speed"
  /// 9 bits are allocated to record these links info (3 bits per Cycle Router) as below:
  /// i.)   BIT[2:0] for Cycle Router 1
  /// ii.)  BIT[5:3] for Cycle Router 2
  /// iii.) BIT[8:6] for Cycle Router 3
  ///
  /// The bits definition is as below:
  /// i.)  Link Width (2 bits): 0 = x1, 1 = x2, 2 = x4
  /// ii.) Link Speed (1 bit) : 0 = GEN1/GEN2, 1 = GEN3
  ///
  Data16 = MmioRead16 (SataRegBase + R_PCH_PCIE_LSTS + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET);

  ///
  /// Check the link speed and update the following bit in RemapDeviceLinkInfo:
  /// i.)   BIT[0] for Cycle Router 1
  /// ii.)  BIT[3] for Cycle Router 2
  /// iii.) BIT[6] for Cycle Router 3
  ///
  switch (Data16 & B_PCIE_LSTS_CLS) {
  case V_PCIE_LSTS_CLS_GEN1:
  case V_PCIE_LSTS_CLS_GEN2:
    *RemapDeviceLinkInfo |= PCI_CARD_LINK_SPEED_GEN1_GEN2 << (Index * 3);
    break;

  case V_PCIE_LSTS_CLS_GEN3:
    *RemapDeviceLinkInfo |= PCI_CARD_LINK_SPEED_GEN3 << (Index * 3);
    break;

  default:
    break;
  }

  ///
  /// Check the link width and update the following bits in RemapDeviceLinkInfo:
  /// i.)   BIT[2:1] for Cycle Router 1
  /// ii.)  BIT[5:4] for Cycle Router 2
  /// iii.) BIT[8:7] for Cycle Router 3
  ///
  switch (Data16 & B_PCIE_LSTS_NLW) {
  case V_PCIE_LSTS_NLW_1:
    *RemapDeviceLinkInfo |= PCI_CARD_LINK_WIDTH_1 << (Index * 3 + 1);
    break;

  case V_PCIE_LSTS_NLW_2:
    *RemapDeviceLinkInfo |= PCI_CARD_LINK_WIDTH_2 << (Index * 3 + 1);
    break;

  case V_PCIE_LSTS_NLW_4:
    *RemapDeviceLinkInfo |= PCI_CARD_LINK_WIDTH_4 << (Index * 3 + 1);
    break;

  default:
    break;
  }

  ///
  /// Step 27 Program the endpoint MBAR length into the "Device Memory BAR Length", AHCIBar offset 804h + n*80h
  ///
  MmioWrite32 (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DMBL + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR), mRstPcieStorageDetection[Index].EndPointBarSize);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DMBL + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR)),
    1,
    (VOID *) (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_CR1_DMBL + (Index * V_PCH_RST_PCIE_STORAGE_REMAP_CONFIG_CR))
    );

  ///
  /// Step 28 If the device supports unique MSI-X Table BAR, program the related PCIe storage device BAR with the same BAR value that been allocated early
  /// Program 0 to the higher memory BAR if it is 64 bit BAR (Done in Step 22.where all BARs within endpoint are cleared to 0)
  ///
  if ((mRstPcieStorageDetection[Index].EndPointUniqueMsixTableBarValue != 0)) {
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR, mRstPcieStorageDetection[Index].EndPointUniqueMsixTableBar);
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR, mRstPcieStorageDetection[Index].EndPointUniqueMsixTableBarValue);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR)
      );
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR)
      );
  }

  ///
  /// Step 29 If the device supports unique MSI-X PBA BAR, program the related PCIe storage device BAR with the same BAR value that been allocated early
  /// Program 0 to the higher memory BAR if it is 64 bit BAR (Done in Step 22.where all BARs within endpoint are cleared to 0)
  ///
  if ((mRstPcieStorageDetection[Index].EndPointUniqueMsixPbaBarValue != 0)) {
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR, mRstPcieStorageDetection[Index].EndPointUniqueMsixPbaBar);
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR, mRstPcieStorageDetection[Index].EndPointUniqueMsixPbaBarValue);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CAIR)
      );
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CADR)
      );
  }

  PchSeries = GetPchSeries();
  PchStep = PchStepping ();
  if (((PchSeries == PchLp) && (PchStep <= PchLpB1)) ||
      ((PchSeries == PchH)  && (PchStep <= PchHC0))) {

    LinkWidth = ((*RemapDeviceLinkInfo >> (Index * 3 + 1)) & (BIT0 | BIT1));

    if ((LinkWidth == PCI_CARD_LINK_WIDTH_1) || (LinkWidth == PCI_CARD_LINK_WIDTH_2)) {
      MmioAndThenOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_CCFG, (UINT32)~(B_PCH_PCIE_CCFG_UNRS), (UINT32)BIT4);
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint32,
        (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CCFG),
        1,
        (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_CCFG)
        );
    }
  }

//LBG PO WA - Setting these bits causes SATA controller is hidden. Should be investigated. It could be silicon bug.
  ///
  /// Step 30 Lockdown the Remap Configuration and RST PCIe Storage Cycle Router
  ///
  MmioOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR, (UINT32) (B_PCH_RST_PCIE_STORAGE_GCR_CREL | B_PCH_RST_PCIE_STORAGE_GCR_RCL));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR)
    );

  DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: RstPcieStorageRemappingLateConfig() Ended\n"));

  return EFI_SUCCESS;
}

/**
  Function to perform early configuration for RST PCIe Storage Remapping

  @param[in] SataRegBase                SATA Register Base
  @param[in] AhciBar                    AHCI Base Address
  @param[in] Index                      Index of PCH_RST_PCIE_STORAGE_DETECTION struct
  @param[in] SataPortsEnabled           SATA Port Implemented
  @param[in] RemapBarEnabled            Boolean to indicate Is AHCI Remapped BAR bit is set or not
  @param[in] TempPciBusMin              The temporary minimum Bus number for root port initialization
  @param[in, out] MsixCurrentVector     Value of Current Vector MSI-X table size
  @retval EFI_SUCCESS                   The function completed successfully
**/
EFI_STATUS
RstPcieStorageRemappingEarlyConfig (
  IN UINTN                                  SataRegBase,
  IN UINT32                                 AhciBar,
  IN UINTN                                  Index,
  IN UINT16                                 SataPortsEnabled,
  IN OUT BOOLEAN                            *RemapBarEnabled,
  IN UINT8                                  TempPciBusMin,
  IN OUT UINT16                             *MsixCurrentVector
  )
{
  EFI_STATUS            Status;
  PCH_SERIES            PchSeries;
  UINTN                 PciRootPortRegBase;
  UINTN                 EndPointPciBase;
  UINTN                 RpDev;
  UINTN                 RpFunc;
  UINT32                Data32;
  UINT32                Data32Or;
  UINT32                Data32And;
  UINT16                Data16;
  UINT16                ValueData16;
  UINT8                 Data8;
  UINT32                DeviceBarSize;
  UINT8                 TempRootPortBusNum;
  UINT16                CapRegs;
  UINT8                 CapPtr;
  UINT8                 CapId;
  UINT32                Timeout;

  DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: RstPcieStorageRemappingEarlyConfig() Started\n"));

DEBUG_CODE_BEGIN();
  DEBUG ((DEBUG_INFO, "=====================================================\n"));
  DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: RstPcieStorageRemappingEarlyConfig() Dump\n"));
  PchRstPcieStorageRemappingDump(Index);
  DEBUG ((DEBUG_INFO, "=====================================================\n"));
DEBUG_CODE_END();

  PchSeries                 = GetPchSeries ();
  TempRootPortBusNum        = TempPciBusMin;

  SetAndSaveRstPcieStorageCycleRouter (SataRegBase, Index);

  ///
  /// Get Root Port Base with PCIe storage device plugged on
  ///
  Status = GetPchPcieRpDevFun (mRstPcieStorageDetection[Index].RootPortNum, &RpDev, &RpFunc);
  ASSERT_EFI_ERROR (Status);
  PciRootPortRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, (UINT32) RpDev, (UINT32) RpFunc);

  ///
  /// Assign temporary bus number to root port
  ///
  MmioAndThenOr32 (
    PciRootPortRegBase + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET,
    (UINT32)~B_PCI_BRIDGE_BNUM_SBBN_SCBN,
    ((UINT32) (TempRootPortBusNum << 8))
      | ((UINT32) (TempRootPortBusNum << 16))
    );

  ///
  /// A config write is required in order for the device to re-capture the Bus number,
  /// according to PCI Express Base Specification, 2.2.6.2
  /// Write to a read-only register VendorID to not cause any side effects.
  ///
  EndPointPciBase = MmPciBase (TempRootPortBusNum, 0, 0);
  MmioWrite16 (EndPointPciBase + PCI_VENDOR_ID_OFFSET, 0);

  if (mRstPcieStorageDetection[Index].DeviceInterface == RST_PCIE_STORAGE_INTERFACE_AHCI) {
  ///
  /// Step 7.a
  /// If the PCIe storage device is using AHCI register interface, map its BAR5 by setting
  /// the Sata PCI offset 310h[31, 20, 19:16] to [1, 0, 1001b]
  /// Also write all 1s into the device BAR5, read it back and keep its value for later steps,
  /// clear the device BAR5 afterward
  ///
    MmioAndThenOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_MBRC,
                     (UINT32)~(B_PCH_RST_PCIE_STORAGE_MBRC_RE | B_PCH_RST_PCIE_STORAGE_MBRC_TT | B_PCH_RST_PCIE_STORAGE_MBRC_TMB),
                     (B_PCH_RST_PCIE_STORAGE_MBRC_RE | (V_PCH_RST_PCIE_STORAGE_MBRC_TMB_BAR5 << N_PCH_RST_PCIE_STORAGE_MBRC_TMB)));

    MmioWrite32 (EndPointPciBase + PCI_CARD_BASE_ADDR5, 0xFFFFFFFF);
    DeviceBarSize = MmioRead32 (EndPointPciBase + PCI_CARD_BASE_ADDR5);
    MmioWrite32 (EndPointPciBase + PCI_CARD_BASE_ADDR5, 0);
  } else {
  ///
  /// Step 7.a
  /// else the PCIe storage device is using NVMe register interface, map its BAR0 by setting
  /// the Sata PCI offset 310h[31, 20, 19:16] to [1, 1, 0100b]
  /// Also write all 1s into the device BAR0, read it back and keep its value for later steps,
  /// clear the device BAR0 afterward
  ///
    MmioAndThenOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_MBRC,
                     (UINT32)~(B_PCH_RST_PCIE_STORAGE_MBRC_RE | B_PCH_RST_PCIE_STORAGE_MBRC_TT | B_PCH_RST_PCIE_STORAGE_MBRC_TMB),
                     (B_PCH_RST_PCIE_STORAGE_MBRC_RE | B_PCH_RST_PCIE_STORAGE_MBRC_TT | (V_PCH_RST_PCIE_STORAGE_MBRC_TMB_BAR0 << N_PCH_RST_PCIE_STORAGE_MBRC_TMB)));

    MmioWrite32 (EndPointPciBase + PCI_CARD_BASE_ADDR0, 0xFFFFFFFF);
    DeviceBarSize = MmioRead32 (EndPointPciBase + PCI_CARD_BASE_ADDR0);
    MmioWrite32 (EndPointPciBase + PCI_CARD_BASE_ADDR0, 0);
  }

  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MBRC),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_MBRC)
    );

  ///
  /// Step 7.a.i, 7.a.ii, 7.a.iii
  /// Check for PCIe storage device BAR size and make sure BAR size is <= 16KB
  /// Skip the device from RST PCIe Storage Remapping if the device BAR size > 16KB
  ///
  DeviceBarSize &= 0xFFFFFFF0;
  DeviceBarSize = (UINT32) ~DeviceBarSize;

  if (DeviceBarSize > 0x3FFF) {
    DEBUG ((DEBUG_INFO, "RstPcieStorageRemappingEarlyConfig: PCIe storage device BAR size exceed 16KB\n"));
    DEBUG ((DEBUG_INFO, "RstPcieStorageRemappingEarlyConfig: Skip this device and continue to next CR#\n"));
    mRstPcieStorageDetection[Index].SupportRstPcieStoragRemapping = FALSE;
    ///
    /// Clear bus numbers
    ///
    MmioAnd32 (PciRootPortRegBase + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, (UINT32)~B_PCI_BRIDGE_BNUM_SBBN_SCBN);
    return EFI_SUCCESS;
  } else {
    mRstPcieStorageDetection[Index].EndPointBarSize = DeviceBarSize;
  }

  ///
  /// Step 7.b Get the endpoint first capability pointer
  ///
  CapPtr = MmioRead8 (EndPointPciBase + PCI_CAPBILITY_POINTER_OFFSET);

  while (CapPtr) {
    CapRegs = MmioRead16 (EndPointPciBase + CapPtr);
    CapId  = (UINT8) (CapRegs & 0x00FF);

    switch (CapId) {
      case PCI_CARD_PM_CAP_ID:
        //
        // Save the Capability Pointer for the device's Power Management Capability to NVS for RTD3 usage
        //
        DEBUG ((DEBUG_INFO, "RstPcieStorageRemappingEarlyConfig: PCIe storage device supports Power Management Capability\n"));
        mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PmCapPtr = CapPtr;
        break;
      case PCI_CARD_MSIX_CAP_ID:
        DEBUG ((DEBUG_INFO, "RstPcieStorageRemappingEarlyConfig: PCIe storage device supports MSI-X Capability\n"));
        RemapDeviceMsixCap (EndPointPciBase, SataRegBase, CapPtr, Index, MsixCurrentVector);
        break;
      case PCI_CARD_SATA_CAP_ID:
        DEBUG ((DEBUG_INFO, "RstPcieStorageRemappingEarlyConfig: PCIe storage device supports Sata Capability\n"));
        RemapDeviceSataCap (EndPointPciBase,SataRegBase,CapPtr);
        break;
      default:
        break;
    }
    CapPtr = (CapRegs & 0xFF00) >> 8;
  }

  ///
  /// Re-check SupportRstPcieStoragRemapping Flag after configurating PCIe storage device's MSI-X Cap
  /// If the flag return FALSE due to table size overflowed, skip the current device and proceed to next device
  ///
  if (mRstPcieStorageDetection[Index].SupportRstPcieStoragRemapping == FALSE) {
    DEBUG ((DEBUG_INFO, "RstPcieStorageRemappingEarlyConfig: skip remapping @ CR#%x due to MSI-X table size overflowed\n", Index + 1));
    return EFI_SUCCESS;
  }

  //
  // Read the PCIe storage device's Extended Capability Pointer
  // If it supports ARI, clear "NVM Remapping Device:Function", Sata PCI offset 350h to 0h
  //
  if (PcieFindExtendedCapId (TempRootPortBusNum, 0, 0, 0xE) != 0) {
    MmioWrite32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_NRDF, 0x0);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_NRDF),
      1,
      (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_NRDF)
      );
  }

  ///
  /// Read and keep PCIe storage device's BCC, SCC and PI
  ///
  mRstPcieStorageDetection[Index].EndPointBcc = MmioRead8 (EndPointPciBase + R_PCI_BCC_OFFSET);
  mRstPcieStorageDetection[Index].EndPointScc = MmioRead8 (EndPointPciBase + R_PCI_SCC_OFFSET);
  mRstPcieStorageDetection[Index].EndPointPi  = MmioRead8 (EndPointPciBase + R_PCI_PI_OFFSET);

  ///
  /// Step 8 Set PCIe storage device discovered root port respectively to "PCIe Lane Selected", Sata PCI offset 300h [20:1]
  ///
  MmioOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR, (UINT32) mRstPcieStorageDetection[Index].RootPortLane << (mRstPcieStorageDetection[Index].RootPortNum + 1));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR)
    );

  ///
  /// Step 9 Clear PCCS, Sata PCI offset 304h [30] by writing 1 to it
  ///
  MmioOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_GSR, (UINT32) B_PCH_RST_PCIE_STORAGE_GSR_PCCS);

  Data32 = MmioRead32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_GSR);
  Data32 |= B_PCH_RST_PCIE_STORAGE_GSR_PCCS;
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_GSR),
    1,
    &Data32
    );

  ///
  /// Step 10.Check the endpoint extended Capability ID
  /// If the endpoint supports L1 Substates, perform the following:
  /// i.)  Store the endpoint's L1 Sub-States Extended Capability offset + 0x0C[7:0]
  /// ii.) Store the endpoint's L1 Sub-States Extended Capability offset + 0x08[31:0]
  ///
  mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.L1ssCapPtr = PcieFindExtendedCapId (TempRootPortBusNum , 0, 0, V_PCIE_EX_L1S_CID);
  if (mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.L1ssCapPtr != 0) {
    DEBUG ((DEBUG_INFO, "RstPcieStorageRemappingEarlyConfig: PCIe storage device supports L1SS Capability\n"));
    mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointL1ssControl2   = MmioRead8 (EndPointPciBase + mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.L1ssCapPtr + 0x0C);
    mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointL1ssControl1   = MmioRead32 (EndPointPciBase + mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.L1ssCapPtr + 0x08);
  }

  ///
  /// Step 10.Check the endpoint extended Capability ID
  /// If the endpoint supports LTR, store the endpoint's LTR Extended Capability offset + 0x04[31:0]
  ///
  mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.LtrCapPtr = PcieFindExtendedCapId (TempRootPortBusNum , 0, 0, 0x18);
  if (mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.LtrCapPtr != 0) {
    DEBUG ((DEBUG_INFO, "RstPcieStorageRemappingEarlyConfig: PCIe storage device supports LTR Capability\n"));
    mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointLtrData     = MmioRead32 (EndPointPciBase + mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.LtrCapPtr + 0x04);
  }

  ///
  /// Step 10.Store the following endpoint's registers
  /// i.)   endpoint's Link Control for "Active State Link PM Control" field, "Enable Clock Power Management" field and "Common Clock Configuration" field
  /// ii.)  endpoint's Device Control 2 for "LTR Mechanism Enable" field
  /// iii.) endpoint's Device Control for "Max Payload Size" field
  ///
  mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PcieCapPtr            = PcieFindCapId (TempRootPortBusNum , 0, 0, 0x10);
  mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointLctlData16    = MmioRead16 (EndPointPciBase + mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PcieCapPtr + 0x10) & (BIT0 | BIT1 | BIT6 | BIT8);
  mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointDctl2Data16   = MmioRead16 (EndPointPciBase + mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PcieCapPtr + 0x28) & BIT10;
  mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointDctlData16    = MmioRead16 (EndPointPciBase + mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PcieCapPtr + 0x8) & (BIT5 | BIT6 | BIT7);

  ///
  /// Step 10.Store the root port's Device Control 2 "LTR Mechanism Enable" field
  ///
  mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.RootPortDctl2Data16   = MmioRead16 (PciRootPortRegBase + R_PCH_PCIE_DCTL2) & B_PCIE_DCTL2_LTREN;

  ///
  /// Clear bus numbers
  ///
  MmioAnd32 (PciRootPortRegBase + PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, (UINT32)~B_PCI_BRIDGE_BNUM_SBBN_SCBN);

  ///
  /// Step 11 BIOS sets the link disable bit in the Link Control register within the associated PCIe root port
  /// to place the link in the disabled state
  ///
  MmioOr16 (PciRootPortRegBase + R_PCH_PCIE_LCTL, (UINT16) (B_PCIE_LCTL_LD));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (PciRootPortRegBase + R_PCH_PCIE_LCTL),
    1,
    (VOID *) (UINTN) (PciRootPortRegBase + R_PCH_PCIE_LCTL)
    );

  ///
  /// Poll PCIe Link Active status till it is down
  ///
  Timeout = 0;
  Data16 = MmioRead16 (PciRootPortRegBase + R_PCH_PCIE_LSTS) & B_PCIE_LSTS_LA;
  while (Data16 == B_PCIE_LSTS_LA) {
    MicroSecondDelay (15);
    Timeout += 15;
    Data16 = MmioRead16 (PciRootPortRegBase + R_PCH_PCIE_LSTS) & B_PCIE_LSTS_LA;
    if ((Timeout > 50000)) {
      DEBUG ((DEBUG_INFO, "RstPcieStorageRemappingEarlyConfig: Timeout (exceed 50ms) when polling for link down\n"));
      break;
    }
  }

  Data16 = B_PCIE_LSTS_LA;
  ValueData16 = 0;
  S3BootScriptSaveMemPoll (
    S3BootScriptWidthUint16,
    (UINTN) (PciRootPortRegBase + R_PCH_PCIE_LSTS),
    &Data16,
    &ValueData16,
    50,
    1000
    );

  ///
  /// Step 11.b Set PCR[PSF_1] + "AGNT_T0_SHDW_PCIEN"[RPx][8] to 1
  ///
  Data32And = (UINT32)~0;
  Data32Or  = B_PCH_PCR_PSFX_T1_SHDW_PCIEN_FUNDIS;
  if (PchSeries == PchLp) {
    PchPcrAndThenOr32 (
       PID_PSF1, (R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE01_REG_BASE - (mRstPcieStorageDetection[Index].RootPortNum * 0x100)) + R_PCH_PCR_PSFX_T1_SHDW_PCIEN,
       Data32And,
       Data32Or
       );
    PCH_PCR_BOOT_SCRIPT_READ_WRITE (
      S3BootScriptWidthUint32,
      PID_PSF1, (R_PCH_LP_PCR_PSF1_T1_SHDW_PCIE01_REG_BASE - (mRstPcieStorageDetection[Index].RootPortNum * 0x100)) + R_PCH_PCR_PSFX_T1_SHDW_PCIEN,
      &Data32Or,
      &Data32And
      );
  } else {
    PchPcrAndThenOr32 (
       PID_PSF1, (R_PCH_H_PCR_PSF1_T1_SHDW_PCIE01_REG_BASE - (mRstPcieStorageDetection[Index].RootPortNum * 0x100)) + R_PCH_PCR_PSFX_T1_SHDW_PCIEN,
       Data32And,
       Data32Or
       );
    PCH_PCR_BOOT_SCRIPT_READ_WRITE (
      S3BootScriptWidthUint32,
      PID_PSF1, (R_PCH_H_PCR_PSF1_T1_SHDW_PCIE01_REG_BASE - (mRstPcieStorageDetection[Index].RootPortNum * 0x100)) + R_PCH_PCR_PSFX_T1_SHDW_PCIEN,
      &Data32Or,
      &Data32And
      );
  }

  ///
  /// Step 12 Mirror the "Ports Implemented" captured in previous step,
  /// program it to the shadowed AHCI Ports Implemented field, SATA PCI offset 358h
  ///
  MmioWrite16 (SataRegBase + R_PCH_RST_PCIE_STORAGE_SAPI, SataPortsEnabled);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_SAPI),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_SAPI)
    );

  ///
  /// Step 13. Program "NVM Remap Memory Offset", "Memory Space Limit" and "NVM Remap Memory BAR Enable"
  /// Set AHCIBar offset A4h [27:16, 12:1, 0] to [10h, 1EFh, '1b']
  /// Only program it once
  ///
  if (!(*RemapBarEnabled)) {
    Data32 = (V_PCH_SATA_VS_CAP_NRMO << N_PCH_SATA_VS_CAP_NRMO) | (B_PCH_SATA_VS_CAP_NRMBE) | (V_PCH_SATA_VS_CAP_MSL << N_PCH_SATA_VS_CAP_MSL);
    MmioWrite32 (AhciBar + R_PCH_SATA_VS_CAP, Data32);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (AhciBar + R_PCH_SATA_VS_CAP),
      1,
      &Data32
      );
    *RemapBarEnabled = TRUE;
  }

  ///
  /// Step 14. Enable RST PCIe Storage Cycle Router
  /// BIOS then writes to the RST PCIe Storage's Cycle Router Enable (CRE) bit in the GCR register to enable
  /// the register remapping.
  ///
  MmioOr32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR, (UINT32) B_PCH_RST_PCIE_STORAGE_GCR_CRE);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR)
    );

  ///
  /// Assert if RST PCIe Storage Cycle Router failed to enable
  ///
  Data32 = MmioRead32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR) & (UINT32) B_PCH_RST_PCIE_STORAGE_GCR_CRE;
  if (Data32 == 0) {
    DEBUG ((DEBUG_ERROR, "RstPcieStorageRemappingEarlyConfig: CRE not able to set, Sata + 300h = %x, Sata + 304h = %x\n", MmioRead32 ((UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_GCR)), MmioRead32 ((UINTN) (SataRegBase + R_PCH_RST_PCIE_STORAGE_GSR))));
    ASSERT (FALSE);
  }

  ///
  /// Step 15 Read and write back "ASSEL" and "MSS" field , SATA PCI offset 9Ch[4:3,2:0] using byte access
  ///
  Data8 = MmioRead8 (SataRegBase + R_PCH_SATA_SATAGC);
  MmioWrite8 (SataRegBase + R_PCH_SATA_SATAGC, Data8);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    (UINTN) (SataRegBase + R_PCH_SATA_SATAGC),
    1,
    &Data8
    );

  ///
  /// Step 16. Set BME of root port
  ///
  MmioOr32 (SataRegBase + PCI_COMMAND_OFFSET + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET, EFI_PCI_COMMAND_BUS_MASTER);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + PCI_COMMAND_OFFSET + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + PCI_COMMAND_OFFSET + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET)
    );

  ///
  /// Step 16 Set all "Memory Base" bits in MBL, PBML and PMBU32 to 1
  ///
  MmioOr32 (SataRegBase + R_PCI_BRIDGE_MBL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET, B_PCI_BRIDGE_MBL_MB);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCI_BRIDGE_MBL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCI_BRIDGE_MBL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET)
    );
  MmioOr32 (SataRegBase + R_PCI_BRIDGE_PMBL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET, B_PCI_BRIDGE_PMBL_PMB);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCI_BRIDGE_PMBL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCI_BRIDGE_PMBL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET)
    );
  MmioOr32 (SataRegBase + R_PCI_BRIDGE_PMBU32 + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET, B_PCI_BRIDGE_PMBU32);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCI_BRIDGE_PMBU32 + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCI_BRIDGE_PMBU32 + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET)
    );

  ///
  /// Step 17. Re-enable PCIe link
  ///
  MmioAnd32 (SataRegBase + R_PCH_PCIE_LCTL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET, (UINT32) ~(B_PCIE_LCTL_LD));
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_PCIE_LCTL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_PCIE_LCTL + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET)
    );

  DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: RstPcieStorageRemappingEarlyConfig() Ended\n"));

  return EFI_SUCCESS;
}

/**
  Function to configure RST PCIe Storage Remapping (Intel RST Driver is required)

  @param[in] TempPciBusMin        The temporary minimum Bus number for root port initialization
  @param[in] TempPciBusMax        The temporary maximum Bus number for root port initialization

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureRstPcieStorageRemapping (
  IN UINT8                         TempPciBusMin,
  IN UINT8                         TempPciBusMax,
  IN UINT8                         SataControllerNo
  )
{
  EFI_PHYSICAL_ADDRESS  MemBaseAddress;
  UINTN                 SataRegBase;
  UINT8                 PortNum;
  UINT16                SataPortsEnabled;
  UINT32                AhciBar;
  UINT32                MaxDeviceResetDelay;
  UINTN                 Index;
  UINT16                Data16;
  UINT16                RemapDeviceLinkInfo;
  UINT8                 RemapCapReg;
  BOOLEAN               RstPcieStorageRemapEnabled;
  BOOLEAN               RemappingRequired;
  BOOLEAN               RemapBarEnabled;
  PCH_STEPPING          PchStep;
  UINT16                MsixCurrentVector;
  UINT32                Timeout;


  PortNum                     = 0;
  RemapCapReg                 = 0;
  MsixCurrentVector           = 0;
  RemappingRequired           = FALSE;
  RemapBarEnabled             = FALSE;
  if (SataControllerNo == PCH_SATA_FIRST_CONTROLLER ) {
    SataRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA);
    RstPcieStorageRemapEnabled  = IsRstPcieStorageRemapEnabled (&mPchPolicyHob->SataConfig, SataControllerNo);
  }
  else if (SataControllerNo == PCH_SATA_SECOND_CONTROLLER) {
    SataRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_EVA, PCI_FUNCTION_NUMBER_PCH_SSATA);
    RstPcieStorageRemapEnabled  = IsRstPcieStorageRemapEnabled (&mPchPolicyHob->sSataConfig, SataControllerNo);
  }
  else {
    DEBUG ((DEBUG_ERROR, "Error: Invalid SATA controller!\n"));
    return EFI_INVALID_PARAMETER;
  }

  ConfigurePmForRstRemapping ();


  ///
  /// Assert if Sata Controller is Disabled and RST PCIe Storage Remapping is Enabled
  ///
  if ((mPchPolicyHob->SataConfig.Enable == FALSE) &&
        (RstPcieStorageRemapEnabled == TRUE)) {
    DEBUG ((DEBUG_ERROR, "ConfigureRstPcieStorageRemapping: Can't perform RST PCIe Storage Remapping when Sata Controller is Disabled\n"));
    ASSERT (FALSE);
    return EFI_SUCCESS;
  }

  ///
  /// Assert if Sata Controller mode is not RAID mode and RST PCIe Storage Remapping is Enabled
  ///
  if ((RstPcieStorageRemapEnabled == TRUE) && (MmioRead8 (SataRegBase + R_PCI_SCC_OFFSET) != PCI_CLASS_MASS_STORAGE_RAID)) {
    DEBUG ((DEBUG_ERROR, "Can't perform RST PCIe Storage remapping when Sata Controller mode is not RAID mode\n"));
    ASSERT (FALSE);
    return FALSE;
  }

  ///
  /// Detect PCIe storage device at root port if RST PCIe Storage Remapping is Enabled
  ///
  if (RstPcieStorageRemapEnabled) {
    DetectPcieStorageDevices (&RemappingRequired, TempPciBusMin, SataRegBase);
  }

  ///
  /// Step 5 Allocate the AHCI BAR
  /// Assign a 64K aligned memory space for ABAR
  ///
  AbarMemorySpaceOperation (TRUE, &MemBaseAddress);

  ///
  /// Disable command register memory space decoding
  ///
  MmioAnd16 (SataRegBase + PCI_COMMAND_OFFSET, (UINT16) ~EFI_PCI_COMMAND_MEMORY_SPACE);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (SataRegBase + PCI_COMMAND_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + PCI_COMMAND_OFFSET)
    );

  ///
  /// Set the AHCI BAR
  ///
  AhciBar = (UINT32) MemBaseAddress;
  MmioWrite32 (SataRegBase + R_PCH_SATA_AHCI_BAR, AhciBar);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_SATA_AHCI_BAR),
    1,
    &AhciBar
    );

  ///
  /// Enable command register memory space decoding
  ///
  MmioOr16 (SataRegBase + PCI_COMMAND_OFFSET, (UINT16) EFI_PCI_COMMAND_MEMORY_SPACE);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (SataRegBase + PCI_COMMAND_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + PCI_COMMAND_OFFSET)
    );

  ///
  /// If no any PCIe storage device is detect, perform to the disabling steps
  ///
  if (!RemappingRequired) {
    DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: No PCIe storage device is detected\n"));
    DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: Disabling RST PCIe Storage Remapping\n"));

    ///
    /// Program "NVM Remap Memory BAR Enable",  AHCIBar offset A4h [0] to ['0b']
    ///
    MmioAnd32 (AhciBar + R_PCH_SATA_VS_CAP, (UINT32) ~(B_PCH_SATA_VS_CAP_NRMBE));
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (AhciBar + R_PCH_SATA_VS_CAP),
      1,
      (VOID *) (UINTN) (AhciBar + R_PCH_SATA_VS_CAP)
      );

      ///
      /// Perform disable flow for each RST PCIe Storage Cycle Router Instance
      ///
      for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
        DisableRstPcieStorageRemapping (SataRegBase, Index);
      }
    goto Exit;
  }

  DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: ConfigureRstPcieStorageRemapping() Started\n"));

  ///
  /// Step 6 Read and store Port Implemented, AHCIBar offset 0Ch for later step usage
  ///
  SataPortsEnabled = MmioRead16 (AhciBar + R_PCH_SATA_AHCI_PI);

  ///
  /// Perform the remapping for every RST PCIe Storage Cycle Router(s)
  ///
  for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
    ///
    /// Proceed to next RST PCIe Storage Cycle Router if no PCIe storage device is found
    ///
    if (mRstPcieStorageDetection[Index].SupportRstPcieStoragRemapping == FALSE) {
      DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: no PCIe storage device is found for RST PCIe Storage Cycle Router %x\n", Index + 1));
      continue;
    }

    ///
    /// Configure the ASPM for Root Port and PCIe storage device before enabling RST PCIe Storage Remapping
    ///
    ConfigurePcieStorageDeviceAspm (mRstPcieStorageDetection[Index].RootPortNum, TempPciBusMin, TempPciBusMax);

    ///
    /// Perform Early Configuration for remapping if the RST PCIe Storage Cycle Router contains PCIe storage device for remapping
    /// This early configuration contains steps to be performed for fast S3/S4/S5,
    /// while the remaining steps can be splitted for late exeuction
    ///
    RstPcieStorageRemappingEarlyConfig (SataRegBase, AhciBar, Index, SataPortsEnabled, &RemapBarEnabled, TempPciBusMin, &MsixCurrentVector);
  }

  ///
  /// Step 19 Poll PCIe Link Active status till it is active for every remapped port(s)
  ///
  MaxDeviceResetDelay = 0;
  for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
    ///
    /// Proceed to next RST PCIe Storage Cycle Router if no PCIe storage device is found
    ///
    if (mRstPcieStorageDetection[Index].SupportRstPcieStoragRemapping == FALSE) {
      continue;
    }


    //
    // Get the highest delay time based on the delay policy for each RST PCIe Storage Cycle Router that is enabled
    //
    if (mPchPolicyHob->SataConfig.RstPcieStorageRemap[Index].DeviceResetDelay > MaxDeviceResetDelay) {
      MaxDeviceResetDelay = mPchPolicyHob->SataConfig.RstPcieStorageRemap[Index].DeviceResetDelay;
    }

    SetAndSaveRstPcieStorageCycleRouter (SataRegBase, Index);

    ///
    /// Poll PCIe Link Active status till it is active
    ///
    Timeout = 0;
    Data16 = MmioRead16 ((SataRegBase + R_PCH_PCIE_LSTS + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET)) & B_PCIE_LSTS_LA;
    while (Data16 == 0) {
      MicroSecondDelay(15);
      Timeout += 15;
      Data16 = MmioRead16 ((SataRegBase + R_PCH_PCIE_LSTS + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET)) & B_PCIE_LSTS_LA;
      if ((Timeout > 50000)) {
        DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: Timeout (exceed 50ms) when polling for link active\n"));
        break;
      }
    }

    Data16 = B_PCIE_LSTS_LA;
    S3BootScriptSaveMemPoll (
      S3BootScriptWidthUint16,
      (UINTN) (SataRegBase + R_PCH_PCIE_LSTS + V_PCH_RST_PCIE_STORAGE_REMAP_RP_OFFSET),
      &Data16,
      &Data16,
      50,
      1000
      );
  }

  ///
  /// Step 20.Stall for a delay based on policy after enable PCIE link
  ///
  MicroSecondDelay (MaxDeviceResetDelay * 1000);
  S3BootScriptSaveStall (MaxDeviceResetDelay * 1000);

  ///
  /// Step 20 Replace the MSI cap from Sata capability link with MSI-X cap
  ///
  MmioWrite8 (SataRegBase + PCI_CAPBILITY_POINTER_OFFSET, R_PCH_SATA_MXID);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    (UINTN) (SataRegBase + PCI_CAPBILITY_POINTER_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + PCI_CAPBILITY_POINTER_OFFSET)
    );

  ///
  /// Step 20.a Link MSI-X identifiers' Next Pointer to PCI Power Management Capability
  /// Program "Next Pointer" in Sata MSI-X Identifiers, Sata PCI offset D0h bit [15:8] to 70h
  ///
  MmioOr16 (SataRegBase + R_PCH_SATA_MXID, R_PCH_SATA_PID << N_PCH_SATA_MXID_NEXT);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (SataRegBase + R_PCH_SATA_MXID),
    1,
    (VOID *) (UINTN) (SataRegBase + R_PCH_SATA_MXID)
    );
  RemapDeviceLinkInfo = 0;

  ///
  /// Perform the remaining remapping configuration steps for every RST PCIe Storage Cycle Router(s)
  ///
  for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {

    if (mRstPcieStorageDetection[Index].SupportRstPcieStoragRemapping == FALSE) {
      ///
      /// If a Cycle Router is not enabled, proceed to next RST PCIe Storage Cycle Router
      ///
      DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: no PCIe storage device is found for RST PCIe Storage Cycle Router %x\n", Index + 1));
      DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: Proceed to next RST PCIe Storage Cycle Router\n"));
      continue;
    }

    ///
    /// Perform remaining (Late) Configuration for RST PCIe Storage Remapping after the delay required after re-enable PCIe lnik
    ///
    RstPcieStorageRemappingLateConfig (SataRegBase, AhciBar, Index, &RemapDeviceLinkInfo);

    ///
    /// Store enabled CR information to be used by "Remap Capability Register" later
    ///
    RemapCapReg |= B_PCH_RST_PCIE_STORAGE_RCR_NRS_CR1 << Index;
  }

  ///
  /// Step 31 Program "Remap Capability Register", AHCIBar offset 800h [2:0] based on enabled CR
  ///
  MmioWrite8 (AhciBar + R_PCH_RST_PCIE_STORAGE_RCR, RemapCapReg);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_RCR),
    1,
    (VOID *) (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_RCR)
  );

  ///
  /// Step 32 Program MSI-X Vector, "AHCI MSI-X Configuration", AHCIBar offset 808h [8:0]
  /// with the remapped devices' link info captured in previous step
  ///
  ///
  // If SPT Ax stepping is detected, program to AHCIBar offset 80Ch [8:0] 
  // The written value will be reflected on AHCIBar offset 808h [8:0] 
  // 
  PchStep = PchStepping (); 
  if (PchStep == PchHA0) { 
  MmioWrite16 (AhciBar + R_PCH_RST_PCIE_STORAGE_SPR, RemapDeviceLinkInfo); 
  S3BootScriptSaveMemWrite ( 
      S3BootScriptWidthUint16, 
      (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_SPR), 
      1, 
      (VOID *) (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_SPR) 
      ); 
  } else { 
  MmioWrite16 (AhciBar + R_PCH_RST_PCIE_STORAGE_AMXC, RemapDeviceLinkInfo);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_AMXC),
    1,
    (VOID *) (UINTN) (AhciBar + R_PCH_RST_PCIE_STORAGE_AMXC)
    );
  }


    for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
      SetAndSaveRstPcieStorageCycleRouter (SataRegBase, Index);
	  MmioAnd32 (SataRegBase + R_PCH_RST_PCIE_STORAGE_EGCR, (UINT32) ~B_PCH_RST_PCIE_STORAGE_EGCR_TSCAS);
      S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, (UINTN)(SataRegBase + R_PCH_RST_PCIE_STORAGE_EGCR), 1, (VOID *)(UINTN)(SataRegBase + R_PCH_RST_PCIE_STORAGE_EGCR));
    }

  ///
  /// Perform lockdown programming for unused Cycle Router
  ///
  LockdownUnusedRstPcieStorageCycleRouter (SataRegBase);

  DEBUG ((DEBUG_INFO, "ConfigureRstPcieStorageRemapping: ConfigureRstPcieStorageRemapping() Ended\n"));

Exit:
  ///
  /// Disable command register memory space decoding
  ///
  MmioAnd16 (SataRegBase + PCI_COMMAND_OFFSET, (UINT16) ~EFI_PCI_COMMAND_MEMORY_SPACE);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (SataRegBase + PCI_COMMAND_OFFSET),
    1,
    (VOID *) (UINTN) (SataRegBase + PCI_COMMAND_OFFSET)
    );

  ///
  /// Set Ahci Bar to zero
  ///
  AhciBar = 0;
  MmioWrite32 (SataRegBase + R_PCH_SATA_AHCI_BAR, AhciBar);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (SataRegBase + R_PCH_SATA_AHCI_BAR),
    1,
    &AhciBar
    );

  ///
  /// Free the ABAR memory space pool
  ///
  AbarMemorySpaceOperation (FALSE, &MemBaseAddress);

  return EFI_SUCCESS;
}
