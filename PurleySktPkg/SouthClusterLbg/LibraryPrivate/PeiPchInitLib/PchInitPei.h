/** @file
  Header file for the PCH Init PEIM

@copyright
 Copyright (c) 2013 - 2015 Intel Corporation. All rights reserved
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
#ifndef _PCH_INIT_PEI_H_
#define _PCH_INIT_PEI_H_

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/TimerLib.h>
#include <PchAccess.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchGbeLib.h>
#include <Library/PchHsioLib.h>
#include <Library/PchPcieRpLib.h>
#include <Library/PchP2sbLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/PchInfoLib.h>
#include <Library/TraceHubInitLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/PchPmcLib.h>
#include <IncludePrivate/Library/PchPciExpressHelpersLib.h>
#include <IncludePrivate/Library/PchInitCommonLib.h>
#include <IncludePrivate/PchPolicyHob.h>
#include <Ppi/PchPolicy.h>
#include <Ppi/PchPcieDeviceTable.h>
#include <Ppi/PchReset.h>
#include <Library/PlatformHooksLib.h>
#include <Library/GpioLib.h>
#include <Library/GpioNativeLib.h>
#include <IncludePrivate/Library/GpioPrivateLib.h>
#include <IndustryStandard/Pci30.h>
#include <MeChipset.h>
#include "PchUsb.h"

extern EFI_GUID gPchDeviceTableHobGuid;

/**
  Internal function performing sSATA init needed in PEI phase for PCH-H
   
  @param[in] PchPolicyPpi The PCH Platform Policy PPI instance
  @param[in] TempMemBaseAddr      Temporary Memory Base Address for PCI 
                                  devices to be used to initialize MMIO 
                                  registers.

  @retval None
**/
VOID
PchHsSataInit (
  IN  PCH_POLICY_PPI            *PchPolicy,
  IN UINT32                     TempMemBaseAddr
  );
/**
  Perform PCH SKU detection to check if the RST PCIe Storage Remapping feature is supported 
  Prior to any PCIe device enumeration, initial the RST PCIe Storage "Cycle Router Enable (CRE)" bit to 1 
  If the CRE bit value after the write is 1, it indicates the bit is RW,and is enabled by fuse 
  else, it indicates the bit is RO only and is disabled by fuse 
  The value after the write is stored in RST PCIe Storage Cycle Router Global Control regiser, and then CRE bit is cleared 
   
  @retval None
**/
VOID
PchRstPcieStorageCreEarlyInit (
  );


/**
  Additional SATA configuration

  @param[in] SataConfig                     The PCH Policy for SATA configuration
  @param[in] AhciBarAddress                 The Base Address for AHCI BAR
**/
VOID
ConfigurePchLpSata (
  IN CONST PCH_SATA_CONFIG                    *SataConfig,
  IN UINT32                                   AhciBarAddress
  );

/**
  Configures Sata Controller for PCH-H

  @param[in] SataConfig                     The PCH Policy for SATA configuration 
  @param[in] RstPcieStorageRemapEnabled     A Boolean to indicate if RST PCIe Storage Remapping is enabled
  @param[in] AhciBarAddress                 The Base Address for AHCI BAR

  @retval EFI_SUCCESS                       The function completed successfully
**/
EFI_STATUS
EarlyConfigurePchHSata (
  IN  CONST PCH_SATA_CONFIG                   *SataConfig,
  IN BOOLEAN                                  RstPcieStorageRemapEnabled,
  IN UINT32                                   AhciBarAddress,
  IN UINT8                                    SataControllerNo
  );

/**
  Additional Sata configuration for PCH-H

  @param[in]      SataConfig                The PCH Policy for SATA configuration
  @param[in] AhciBarAddress                 The Base Address for AHCI BAR
**/
VOID
ConfigurePchHSata (
  IN CONST PCH_SATA_CONFIG                    *SataConfig,
  IN UINT32                                   AhciBarAddress,
  IN UINT8                                    SataControllerNo
  );

/**
  Program PSF grant counts for PCI express depending on controllers configuration
**/
VOID
PchConfigurePsfGrantCountsForPcie (
  VOID
  );

/**
  Reload default RP PSF device number and function number.
  The PSF register doesn't got reset after system reset, which will result in mismatch between
  PSF register setting and PCIE PCR PCD register setting. Reset PSF regsiter in early phase
  to avoid cycle decoding confusing.
**/
VOID
PchRpReloadDefaultDevFunc (
  VOID
  );

/**
  Perform Root Port Initialization.

  @param[in]  PortIndex               The root port to be initialized (zero based)
  @param[in]  PchPolicy               The PCH Policy PPI
  @param[in]  PmcBaseAddress          The Power Management Controller Base Address
  @param[in]  ABase                   The PM I/O Base address of the PCH
  @param[in] TempPciBusMin            The temporary minimum Bus number for root port initialization
  @param[in] TempPciBusMax            The temporary maximum Bus number for root port initialization

  @retval EFI_SUCCESS             Device found. The root port must be enabled.
  @retval EFI_NOT_FOUND           No device is found on the root port. It may be disabled.
  @exception EFI_UNSUPPORTED      Unsupported operation.
**/
EFI_STATUS
PchInitSingleRootPort (
  IN  UINT8                                     PortIndex,
  IN  CONST PCH_POLICY_PPI                      *PchPolicy,
  IN  UINTN                                     PmcBaseAddress,
  IN  UINT16                                    ABase,
  IN  UINT8                                     TempPciBusMin,
  IN  UINT8                                     TempPciBusMax,
  OUT BOOLEAN                                   *Gen3DeviceFound
  );

/**
  Perform Initialization of the Downstream Root Ports.

  @param[in] PchPolicy            The PCH Policy PPI
  @param[in] TempPciBusMin        The temporary minimum Bus number for root port initialization
  @param[in] TempPciBusMax        The temporary maximum Bus number for root port initialization

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
PchInitRootPorts (
  IN CONST PCH_POLICY_PPI *PchPolicy,
  IN UINT8                   TempPciBusMin,
  IN UINT8                   TempPciBusMax
  );

VOID
DirtyWarmResetExecute (
  VOID
  );

EFI_STATUS
PchSetLinkWidth(
  IN  PCH_POLICY_PPI  *PchPolicyPpi
);

/**
  This is the function to enable the clock gating for PCI Express ports.

  @param[in] BusNumber            The Bus Number of the PCH device
  @param[in] PortIndex            The root port number (zero based)

  @retval EFI_SUCCESS             Successfully completed.
**/
EFI_STATUS
PcieEnableClockGating (
  IN  UINT8                                PortIndex
  );


/**
  This is the function to do IOSF Port Configuration and Grant Count Programming.

  @param[in] RootComplexBar       Root complex base address
  @param[in] RPBase               Root Port Base Address
  @param[in] PchPolicy            PCH Policy PPI

  @retval None
**/
VOID
SetPcieIosfPortAndGrantCount (
  IN UINTN                      RPBase,
  IN PCH_POLICY_PPI*   PchPolicy
  );

/**
  This function performs basic initialization for TraceHub in PEI phase.

  @param[in] PchPolicyPpi The PCH Policy PPI instance

**/
VOID
TraceHubPolicyInit (
  IN  PCH_POLICY_PPI   *PchPolicyPpi
  );

/**
  Program Pch devices Subsystem Vendor Identifier (SVID) and Subsystem Identifier (SID).

  @param[in] PchPolicy    The PCH Policy PPI instance

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ProgramSvidSid (
  IN  PCH_POLICY_PPI     *PchPolicy
  );

/**
  Configures PCH USB device (xDCI)

  @param[in]  PchPolicy         The PCH Policy PPI instance
  @param[in] TempMemBaseAddr    Temporary Memory Base Address for PCI 
                                devices to be used to initialize MMIO 
                                registers.

  @retval EFI_SUCCESS           The function completed successfully
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver
**/
EFI_STATUS
ConfigureXdci (
  IN  PCH_POLICY_PPI            *PchPolicy
  );

/**
  This function performs TraceHub PCH initialization stage on end of PEI phase, when DRAM is available.

  @param[in] PchPolicy              The PCH Policy PPI instance

  @retval EFI_OUT_OF_RESOURCES      Requested amount of memory could not be allocated.
  @retval EFI_NOT_FOUND             The Cache PPI descriptor is not found in the database.
  @retval EFI_LOAD_ERROR            No empty MTRRs to use.
  @retval EFI_INVALID_PARAMETER     The input parameter to SetCache is not valid.
**/
EFI_STATUS
PchTraceHubOnEndOfPei (
  IN  PCH_POLICY_PPI          *PchPolicy
  );

/**
  This function completes power gating management for TraceHub and ensures locking of HSWPGCR1 register.
**/
VOID
TraceHubManagePowerGateControl (
  VOID
 );

/**
  This function configures DMI and OP-DMI.

  @param[in] PchPolicyPpi The PCH Policy PPI instance

  @retval None
**/
VOID
PchDmiAndOpDmiInit (
  IN  PCH_POLICY_PPI  *PchPolicyPpi
  );

/**
  Initialize IOAPIC according to IoApicConfig policy of the PCH
  Policy PPI

  @param[in] PchPolicyPpi         The PCH Policy PPI instance

  @retval EFI_SUCCESS             Succeeds.
  @retval EFI_DEVICE_ERROR        Device error, aborts abnormally.
**/
EFI_STATUS
PchIoApicInit (
  IN  PCH_POLICY_PPI     *PchPolicyPpi
  );

/**
  Configures PCH devices interrupts

  @param[in] PchPolicy          Policy

  @retval Status
**/
EFI_STATUS
PchConfigureInterrupts (
  IN  PCH_POLICY_PPI           *PchPolicy
  );


/**
  Initialize LAN device.

  Global reset may be perfomed due to static power gating requirements.

  @param[in] PchPolicy            The PCH Policy PPI instance
  @param[in] TempMemBaseAddr      Temporary Memory Base Address for PCI 
                                  devices to be used to initialize MMIO 
                                  registers.
**/
VOID
PchConfigureLan (
  IN  PCH_POLICY_PPI            *PchPolicy
  );

/**
  Initialize the Intel High Definition Audio Controller and Codec(s) present in the system.
  For each codec, a predefined codec verb table should be programmed.
  The list contains 32-bit verbs to be sent to the corresponding codec.
  If it is not programmed, the codec uses the default verb table, which may or may not
  correspond to the platform jack information.

  @param[in] PchPolicy            Policy
  @param[in] TempMemBaseAddr      Temporary Memory Base Address for PCI 
                                  devices to be used to initialize MMIO 
                                  registers.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Provided VerbTableData is null
**/
EFI_STATUS
PchHdAudioInit (
  IN  PCH_POLICY_PPI            *PchPolicy
  );

/**
  Perform Thermal Management Support initialization

  @param[in] PchPolicy            The PCH Policy PPI instance
  @param[in] TempMemBaseAddr      Temporary Memory Base Address for PCI 
                                  devices to be used to initialize MMIO 
                                  registers.

  @retval EFI_SUCCESS             Succeeds.
  @retval EFI_DEVICE_ERROR        Device error, aborts abnormally.
**/
EFI_STATUS
PchThermalInit (
  IN  PCH_POLICY_PPI            *PchPolicy
  );

/**
  Init Memory Throttling.

  @param[in] PchPolicyPpi               The PCH Policy PPI instance
**/
VOID
PchMemoryThrottlingInit (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  );


/**
  Prepares SerialIo controllers for OS boot
  @param[in] PchPolicy        The PCH Policy PPI instance
**/
VOID
ConfigureSerialIoAtS3Resume (
  IN PCH_POLICY_PPI           *PchPolicy
  );

/**
  Initializes GPIO pins assigned to SerialIo
  @param[in] PchPolicy        The PCH Policy PPI instance
**/
VOID
SerialIoGpioInit (
  IN PCH_POLICY_PPI           *PchPolicy
  );


/**
  Perform power management initialization

  @param[in] PchPolicy          The PCH Policy PPI instance

  @retval EFI_SUCCESS                   Succeeds.
**/
EFI_STATUS
PchPmInit (
  IN  PCH_POLICY_PPI           *PchPolicy
  );

/**
  The function is used to detemine if a ChipsetInitSync with ME is required and syncs with ME if required.
  
  @param[in] MeSync               Sync ME Settings

  @retval EFI_SUCCESS             BIOS and ME ChipsetInit settings are in sync
  @retval EFI_UNSUPPORTED         BIOS and ME ChipsetInit settings are not in sync
**/
EFI_STATUS
PchHsioChipsetInitProg (
    IN  BOOLEAN                      MeSync
  );

/**
  The function program HSIO registers.

  @param[in] PchPolicyPpi         The PCH Policy PPI instance

  @retval EFI_SUCCESS             THe HSIO settings have been programmed correctly
**/
EFI_STATUS
PchHsioBiosProg (
  IN  PCH_POLICY_PPI    *PchPolicyPpi
  );

/**
  Write Pmc Xram data.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.
  @param[in]  TempMemBaseAddr           Temporary memory base address for PMC device
  @param[in]  PmcBaseAddress            PMC Base Address
**/
VOID
PchPmcXramWrite32 (
  IN  PCH_SBI_PID                       Pid,
  IN  UINT16                            Offset,
  IN  UINT32                            InData,
  IN  UINT32                            TempMemBaseAddr,
  IN  UINTN                             PmcBaseAddress
  );

/**
  Write PMC XRAM End Table Marker

  @param[in]  TempMemBaseAddr           Temporary memory base address for PMC device
  @param[in]  PmcBaseAddress            PMC Base Address
**/
VOID
PchPmcXramEndTableMarker (
  IN  UINT32          TempMemBaseAddr,
  IN  UINTN           PmcBaseAddress
  );

/**
  Program PMC XRAM Data Header

  @param[in]  TempMemBaseAddr           Temporary memory base address for PMC device
  @param[in]  PmcBaseAddress            PMC Base Address
**/
VOID
PchPmcXramDataHdrProg (
  IN  UINT32                            TempMemBaseAddr,
  IN  UINTN                             PmcBaseAddress
  );
/**
  Configure PMC related settings when SCS controllers are disabled.
**/
VOID
ConfigurePmcWhenScsDisabled (
  BOOLEAN                               Disable
  );

/**
  Configure PMC related settings when CIO2 controllers is disabled.
**/
VOID
ConfigurePmcWhenCio2Disabled (
  BOOLEAN                               Disable
  );

/**
  Lock down PMC settings

  @param[in] PchPolicy                  The PCH Policy PPI instance
**/
VOID
LockPmcSettings (
  IN  PCH_POLICY_PPI                    *PchPolicy
  );

/**
  Configure LPC device on early PEI.
**/
VOID
ConfigureLpcOnEarlyPei (
  VOID
  );

/**
  Configure LPC device on Policy callback.

  @param[in] PchPolicyPpi  The PCH Policy PPI instance
**/
VOID
ConfigureLpcOnPolicy (
  IN  PCH_POLICY_PPI                    *PchPolicyPpi
  );

/**
  Get BMC RootPort

  @param none

  @raturn BMC root port number
**/
UINTN
GetBmcRootPort (
  VOID
  );
#endif
