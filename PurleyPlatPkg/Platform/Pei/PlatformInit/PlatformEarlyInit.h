//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PlatformEarlyInit.h

  Platform Stage1 header file

**/

#ifndef _EFI_PLATFORM_EARLY_INIT_H_
#define _EFI_PLATFORM_EARLY_INIT_H_

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Ppi/BaseMemoryTest.h>
#include <Ppi/Stall.h>
#include <Ppi/FlashMap.h>
#include <Ppi/PlatformMemorySize.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/PchPolicy.h>
#include <Ppi/Spi.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Ppi/Reset.h>
#include <Ppi/Smbus2.h>
#include <GpioInitData.h>

#include <Guid/PlatformInfo.h>
#include <Guid/SetupVariable.h>
#include <Guid/PchRcVariable.h>
#include <MeRcVariable.h>
#include <Guid/MemoryTypeInformation.h>
#include <Guid/PlatformTxt.h>
#include <Guid/BiosId.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PciLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PlatformHooksLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/PrintLib.h>
#include <Library/BiosIdLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PlatformStatusCodes.h>
#include <Library/SetupLib.h>
#include <Ppi/SystemBoard.h>

#include <Cpu/CpuBaseLib.h>
#include <Platform.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsLpc.h>
#include <Register/PchRegsSpi.h>
#include <PchAccess.h>
#include <Library/MmPciBaseLib.h>
#include <IndustryStandard/Pci22.h>

#include <Framework/Hob.h>
#include <Guid/Capsule.h>
#include <SioRegs.h>
#include <FlashMap.h>

#include <Ppi/SpiSoftStraps.h>
#include <Protocol/IioUds.h>

#define LEAVE_POWER_STATE_UNCHANGED 0xFF

//
// Reset Generator I/O Port
//
#define RESET_GENERATOR_PORT  0xCF9

//-----------------------------------------------------------------------;
// PCH: Chipset Configuration Register Equates
//-----------------------------------------------------------------------;
#define ICH_RCRB_IRQ0                       0
#define ICH_RCRB_IRQA                       1
#define ICH_RCRB_IRQB                       2
#define ICH_RCRB_IRQC                       3
#define ICH_RCRB_IRQD                       4
#define ICH_RCRB_PIRQA                      0
#define ICH_RCRB_PIRQB                      1
#define ICH_RCRB_PIRQC                      2
#define ICH_RCRB_PIRQD                      3
#define ICH_RCRB_PIRQE                      4
#define ICH_RCRB_PIRQF                      5
#define ICH_RCRB_PIRQG                      6
#define ICH_RCRB_PIRQH                      7

//
// Wake Event Types
//
#define SMBIOS_WAKEUP_TYPE_RESERVED           0x00
#define SMBIOS_WAKEUP_TYPE_OTHERS             0x01
#define SMBIOS_WAKEUP_TYPE_UNKNOWN            0x02
#define SMBIOS_WAKEUP_TYPE_APM_TIMER          0x03
#define SMBIOS_WAKEUP_TYPE_MODEM_RING         0x04
#define SMBIOS_WAKEUP_TYPE_LAN_REMOTE         0x05
#define SMBIOS_WAKEUP_TYPE_POWER_SWITCH       0x06
#define SMBIOS_WAKEUP_TYPE_PCI_PME            0x07
#define SMBIOS_WAKEUP_TYPE_AC_POWER_RESTORED  0x08


//-----------------------------------------------------------------------
// SPI control regs.
//-----------------------------------------------------------------------
#define R_SB_SPI_HSFSTS 0x04
#define B_SB_SPI_LOCK_DOWN 0x8000;
#define R_SB_SPI_HSFCTL 0x06
#define B_SB_SPI_READ_CYCLE  BIT0;
#define B_SB_SPI_WRITE_CYCLE BIT2|BIT0;
#define B_SB_SPI_BLOCK_ERASE BIT2|BIT1|BIT0;

#define SPI_VSCC_MINUM_COMMUN_DENOMINATOR  0xD817
#define DESCRIP_ADDR        0x10;

#define R_SB_SPI_FADDR  0x08
#define DESCRIPT_SIZE   0x1000


#define R_SB_SPI_VSCC0 0xC4
#define R_SB_SPI_VSCC1 0xC8
#define R_SB_SPI_FDAT0 0x10

// APTIOV_SERVER_OVERRIDE_RC_START : Added to resolve redefinition errors.
#ifndef SPI_BASE_ADDRESS
#define SPI_BASE_ADDRESS                  PCH_SPI_BASE_ADDRESS
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Added to resolve redefinition errors.
#define SPI_REGION_SV_FLAG_DESC_OVERR_DIS 0x00000001
#define SPI_REGION_SV_SIGNATURE           0xC0CAC01A

struct _SPI_REGION_SV {
  UINT32 signature;
  UINT32 sv_flags;
  UINT32 reserved_0;
  UINT32 reserved_1;
};

typedef struct _SPI_REGION_SV SPI_REGION_SV;


#define RESET_PORT                0x0CF9
#define CLEAR_RESET_BITS          0x0F1
#define COLD_RESET                0x02  // Set bit 1 for cold reset
#define RST_CPU                   0x04  // Setting this bit triggers a reset of the CPU
#define FULL_RESET                0x08  // Set bit 4 with bit 1 for full reset

#define PCHSTRAP_9   9
#define PCHSTRAP_10  10
#define PCHSTRAP_16  16
#define PCHSTRAP_17  17

//
// From WBG Soft Straps WIP.xlsx
//
#define WBG_DOWNSKU_STRAP_DSKU      0x80046000
#define WBG_DOWNSKU_STRAP_BSKU      0x8004E003
#define WBG_DOWNSKU_STRAP_TSKU      0x00044000

#define DESCRIP_FLMAP1      0x18;

#define AUTO_BIF     4

#define HSFC_MASK                   (BIT15|BIT7|BIT6)   

/*
//
// Driver Consumed Guids
//
#include EFI_GUID_DEFINITION (TcgDataHob)
#include EFI_GUID_DEFINITION (MemoryTypeInformation)
#include EFI_GUID_DEFINITION (PlatformInfo)
#include EFI_GUID_DEFINITION (SetupVariable)

//
// Driver Consumed PPI Prototypes
//
#include EFI_PPI_DEPENDENCY (CpuIo)
#include EFI_PPI_DEPENDENCY (PciCfg)
#include EFI_PPI_DEPENDENCY (BaseMemoryTest)
#include EFI_PPI_DEFINITION (FlashMap)
#include EFI_PPI_DEFINITION (SmbusPolicy)
#include EFI_PPI_DEFINITION (Cache)
#include EFI_PPI_DEFINITION (MemoryDiscovered)
#include EFI_PPI_DEFINITION (EndOfPeiSignal)
#include EFI_PPI_DEFINITION (Capsule)

#include EFI_PPI_DEFINITION (PchPlatformPolicy)

//
// Driver Produced PPI Prototypes
//
#include EFI_PPI_PRODUCER (AcpiPolicy)
#include EFI_PPI_PRODUCER (AtaPolicy)
#include EFI_PPI_PRODUCER (BootMode)
#include EFI_PPI_PRODUCER (BootInRecoveryMode)
#include EFI_PPI_PRODUCER (Stall)
#include EFI_PPI_PRODUCER (Reset)
#include EFI_PPI_PRODUCER (Variable)
#include EFI_PPI_PRODUCER (Variable2)
#include EFI_PPI_PRODUCER (PlatformMemorySize)
*/

//
// Defines for stall ppi
//
#define PEI_STALL_RESOLUTION  1

//
// Used in PEI memory test routines
//
#define MEMORY_TEST_COVER_SPAN  0x40000
#define MEMORY_TEST_PATTERN     0x5A5A5A5A

#define LEAVE_POWER_STATE_UNCHANGED 0xFF

#ifndef EFI_DEADLOOP
 #define EFI_DEADLOOP()    { volatile int __iii; __iii = 1; while (__iii); }
#endif

#define ___INTERNAL_CONVERT_TO_STRING___(a) #a
#define CONVERT_TO_STRING(a) ___INTERNAL_CONVERT_TO_STRING___(a)

#define ADV_DEBUG_AUTO 2
#define ADV_DEBUG_ENABLE 1
#define ADV_DEBUG_DISABLE 0
//
// Function prototypes for breakpoint callbacks
//
EFI_STATUS
EFIAPI
AfterMrcBreakpoint (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  );

EFI_STATUS
EFIAPI
AfterQpircBreakpoint (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  );

EFI_STATUS
EFIAPI
ReadyForIbistBreakpoint (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  );

EFI_STATUS
EFIAPI
AfterFullSpeedSetupBreakpoint (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  );

EFI_STATUS
PeiPrintPlatformInfo (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN VOID                         *Ppi
  );

EFI_STATUS
PeiGetBiosId (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN OUT  BIOS_ID_IMAGE           *BiosIdImage
  );
  
VOID
EnableMasterAccess (
  IN EFI_PEI_SERVICES           **PeiServices
  );

typedef struct _PEI_FV_INFO_PPI_PRIVATE {
  EFI_PEI_PPI_DESCRIPTOR                  PpiList;
  EFI_PEI_FIRMWARE_VOLUME_INFO_PPI        FvInfoPpi;
} PEI_FV_INFO_PPI_PRIVATE;
//
// The following CK505 configuration registers settings are based on Simics's implementation
//
#define CLOCK_GENERATOR_SETTINGS_CK505       {0x00, 0xF3, 0x0F, 0xFE, 0x98, 0x02, 0x08, 0x26, 0x7C, 0xE7, 0x0F, 0xFE, 0x08}

VOID
EFIAPI
InstallFvInfoPpi (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_FV_HANDLE           VolumeHandle
  );


//
// Function Prototypes
//
/**

  GC_TODO: Add function description

  @param PeiServices   - GC_TODO: add argument description
  @param This          - GC_TODO: add argument description
  @param BeginAddress  - GC_TODO: add argument description
  @param MemoryLength  - GC_TODO: add argument description
  @param Operation     - GC_TODO: add argument description
  @param ErrorAddress  - GC_TODO: add argument description

  @retval GC_TODO: add return values

**/
EFI_STATUS
EFIAPI
BaseMemoryTest (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  PEI_BASE_MEMORY_TEST_PPI  *This,
  IN  EFI_PHYSICAL_ADDRESS      BeginAddress,
  IN  UINT64                    MemoryLength,
  IN  PEI_MEMORY_TEST_OP        Operation,
  OUT EFI_PHYSICAL_ADDRESS      *ErrorAddress
  )
;

VOID
EarlyPlatformPchInit (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN SYSTEM_CONFIGURATION        *SystemConfiguration,
  //
  // PCH_VARIABLE
  // Add the PCH Variable till we move to PCH pkg
  //
  IN PCH_RC_CONFIGURATION        *PchConfiguration,
  IN SYSTEM_BOARD_PPI            *SystemBoard
  );

BOOLEAN
isFlashDescriptorLocked(
  IN EFI_PEI_SERVICES **PeiServices
  );

/**
  isFlashDescriptorOverrideDisadled - DFX function. Checks if validation team allows for Flash Descriptor override

  @param[in] PeiServices            Pointer to the PeiServices
  @param[in] SpiProtocol            Pointer to the SpiProtocol

  @retval TRUE                           Flash descriptor is locked for write
  @retval FALSE                          Flash descriptor is open for write
**/
BOOLEAN
isFlashDescriptorOverrideDisadled(
  IN EFI_PEI_SERVICES **PeiServices,
  IN EFI_SPI_PROTOCOL *SpiProtocol,
  IN UINT32            fmba
  );
/**

  GC_TODO: Add function description

  @param PeiServices   - GC_TODO: add argument description
  @param This          - GC_TODO: add argument description
  @param Microseconds  - GC_TODO: add argument description

  @retval GC_TODO: add return values

**/
EFI_STATUS
EFIAPI
Stall (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN CONST EFI_PEI_STALL_PPI          *This,
  IN UINTN                      Microseconds
  )
;

EFI_STATUS
PeimInitializeFlashMap (
  IN EFI_PEI_FILE_HANDLE       *FileHandle,
  IN CONST EFI_PEI_SERVICES           **PeiServices
  );

EFI_STATUS
PeimInstallFlashMapPpi (
  IN EFI_PEI_FILE_HANDLE       *FileHandle,
  IN CONST EFI_PEI_SERVICES           **PeiServices
  );

/**

  GC_TODO: Add function description

  @param PeiServices   - GC_TODO: add argument description
  @param AreaType      - GC_TODO: add argument description
  @param AreaTypeGuid  - GC_TODO: add argument description
  @param NumEntries    - GC_TODO: add argument description
  @param Entries       - GC_TODO: add argument description

  @retval GC_TODO: add return values

**/
EFI_STATUS
EFIAPI
GetAreaInfo (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  EFI_FLASH_AREA_TYPE       AreaType,
  IN  EFI_GUID                  *AreaTypeGuid,
  OUT UINT32                    *NumEntries,
  OUT EFI_FLASH_SUBAREA_ENTRY   **Entries
  )
;

/**

  GC_TODO: Add function description

  @param PeiServices       - GC_TODO: add argument description
  @param NotifyDescriptor  - GC_TODO: add argument description
  @param Ppi               - GC_TODO: add argument description

  @retval GC_TODO: add return values

**/
EFI_STATUS
EFIAPI
SiliconRcHobsReadyPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
;
/**

  Provide the read variable functionality of the variable services.

  @param This             - Pointer to EFI_PEI_READ_ONLY_VARIABLE2_PPI.

  VariableName     - The variable name

  VendorGuid       - The vendor's GUID

  Attributes       - Pointer to the attribute

  DataSize         - Size of data

  Data             - Pointer to data

  @retval EFI_SUCCESS           - The interface could be successfully installed

  EFI_NOT_FOUND         - The variable could not be discovered

  EFI_BUFFER_TOO_SMALL  - The caller buffer is not large enough

**/
EFI_STATUS
EFIAPI
PeiGetVariable (
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI    *This,
  IN CONST CHAR16                             *VariableName,
  IN CONST EFI_GUID                           *VariableGuid,
  OUT UINT32                                  *Attributes,
  IN OUT UINTN                                *DataSize,
  OUT VOID                                    *Data
  )
;

/**

  Provide the get next variable functionality of the variable services.

  @param This             - Pointer to EFI_PEI_READ_ONLY_VARIABLE2_PPI.
  @param VariabvleNameSize  - The variable name's size.
  @param VariableName       - A pointer to the variable's name.
  @param VendorGuid         - A pointer to the EFI_GUID structure.

  VariableNameSize - Size of the variable name

  VariableName     - The variable name

  VendorGuid       - The vendor's GUID

  @retval EFI_SUCCESS - The interface could be successfully installed

  EFI_NOT_FOUND - The variable could not be discovered

**/
EFI_STATUS
EFIAPI
PeiGetNextVariableName (
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI    *This,
  IN OUT UINTN                                *VariableNameSize,
  IN OUT CHAR16                               *VariableName,
  IN OUT EFI_GUID                             *VariableGuid
  )
;

/**

  GC_TODO: Add function description

  @param PeiServices - GC_TODO: add argument description
  @param This        - GC_TODO: add argument description
  @param MemorySize  - GC_TODO: add argument description

  @retval GC_TODO: add return values

**/
EFI_STATUS
EFIAPI
GetPlatformMemorySize (
  IN      EFI_PEI_SERVICES                       **PeiServices,
  IN      PEI_PLATFORM_MEMORY_SIZE_PPI           *This,
  IN OUT  UINT64                                 *MemorySize
  )
;

EFI_STATUS
EndOfPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

/**

  GC_TODO: Add function description

  @param PeiServices - GC_TODO: add argument description

  @retval GC_TODO: add return values

**/
EFI_STATUS
EFIAPI
PeimInitializeRecovery (
  IN EFI_PEI_SERVICES     **PeiServices
  )
;

VOID
CheckPowerOffNow (
  VOID
  );

VOID
PchBaseInit (
  );

VOID
LpcSioEarlyInit (
  IN     SYSTEM_BOARD_PPI               *SystemBoard
  );

VOID
W83527SioEarlyInit (
  VOID
  );

VOID
PeiIioInit(
  VOID
  );

EFI_STATUS
PcieSecondaryBusReset (
  IN EFI_PEI_SERVICES  **PeiServices,
  IN UINT8             Bus,
  IN UINT8             Dev,
  IN UINT8             Fun
  );

EFI_STATUS
ConfigurePlatformClocks (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *SmbusPpi
  );

//   EDK2_TODO
#define EFI_MAX_ADDRESS   0xFFFFFFFF        /* Definition in EfiBind.h */

EFI_STATUS
PlatformPchUsbInit (
  IN EFI_PEI_SERVICES          **PeiServices
  );

EFI_STATUS
PchLockDescriptorRegion(
  IN EFI_PEI_SERVICES            **PeiServices
  );

VOID
GetIioUdsHob (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN IIO_UDS                  **UdsHobPtr
  );

VOID
InitPchUsbConfig (
  IN PCH_USB_CONFIG                     *PchUsbConfig
  );

EFI_STATUS
SystemPchInit (
  IN CONST EFI_PEI_SERVICES          **PeiServices,
  IN SYSTEM_BOARD_PPI                *SystemBoard,
  IN PCH_POLICY_PPI                  *PchPlatformPolicyPpi
  );

VOID
PchLpcInit (
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  );

VOID
PchRcrbInit (
  IN SYSTEM_CONFIGURATION        *SystemConfiguration
  );

VOID
PchSataInitPcdSet (
  IN SYSTEM_BOARD_PPI       *SystemBoard
  );

/**

  GC_TODO: Add function description

  @param PeiServices - GC_TODO: add argument description

  @retval GC_TODO: add return values

**/
// APTIOV_SERVER_OVERRIDE_RC_START : Added PchResetPlatform() to call PchResetPpi->Reset 
EFI_STATUS
EFIAPI
PchResetPlatform (
// APTIOV_SERVER_OVERRIDE_RC_END : Added PchResetPlatform() to call PchResetPpi->Reset 
  IN CONST EFI_PEI_SERVICES           **PeiServices
  );
#endif
