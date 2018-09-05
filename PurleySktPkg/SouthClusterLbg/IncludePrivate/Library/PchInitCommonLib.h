/** @file
  Header file for PCH Init Common Lib

@copyright
  Copyright (c) 2013 - 2014 Intel Corporation. All rights reserved
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
#ifndef _PCH_INIT_COMMON_LIB_H_
#define _PCH_INIT_COMMON_LIB_H_

#include <PchPolicyCommon.h>
#include <PchAccess.h>

// SERVER_BIOS_FLAG sync up to 2015-10-16
/**
  This function returns PID according to PCIe controller index

  @param[in] ControllerIndex     PCIe controller index

  @retval PCH_SBI_PID    Returns PID for SBI Access
**/
PCH_SBI_PID
PchGetPcieControllerSbiPid (
  IN  UINT32  ControllerIndex
  );

/**
  This function returns PID according to PCIe controller index

  @param[in] ControllerIndex     PCIe controller index

  @retval PCH_SBI_PID    Returns PID for SBI Access
**/
PCH_SBI_PID
PchGetPcieControllerSbiPid (
  IN  UINT32  ControllerIndex
  );

/**
  This function returns PID according to Root Port Number

  @param[in] RpPort             Root Port Number

  @retval PCH_SBI_PID    Returns PID for SBI Access
**/
PCH_SBI_PID
GetRpSbiPid (
  IN  UINTN  RpPort
  );
// SERVER_BIOS_FLAG
/**
  Calculate root port device number based on physical port index.

  @param[in]  RpIndex              Root port index (0-based).

  @retval     Root port device number.
**/
UINT32
PchGetPcieRpDevice (
  IN  UINT32   RpIndex
  );

/**
  This function reads Pci Config register via SBI Access

  @param[in]  RpIndex             Root Port Index (0-based)
  @param[in]  Offset              Offset of Config register
  @param[out] *Data32             Value of Config register

  @retval EFI_SUCCESS             SBI Read successful.
**/
EFI_STATUS
PchSbiRpPciRead32 (
  IN    UINT32  RpIndex,
  IN    UINT32  Offset,
  OUT   UINT32  *Data32
  );

/**
  This function And then Or Pci Config register via SBI Access

  @param[in]  RpIndex             Root Port Index (0-based)
  @param[in]  Offset              Offset of Config register
  @param[in]  Data32And           Value of Config register to be And-ed
  @param[in]  Data32AOr           Value of Config register to be Or-ed

  @retval EFI_SUCCESS             SBI Read and Write successful.
**/
EFI_STATUS
PchSbiRpPciAndThenOr32 (
  IN  UINT32  RpIndex,
  IN  UINT32  Offset,
  IN  UINT32  Data32And,
  IN  UINT32  Data32Or
  );

/**
  Configure root port function number mapping

  @retval EFI_SUCCESS                   The function completed successfully
**/
EFI_STATUS
PchConfigureRpfnMapping (
  VOID
  );

/**
  This function lock down the P2sb SBI before going into OS.
  This only apply to PCH B0 onward.

  @param[in] P2sbConfig                 The PCH policy for P2SB configuration
**/
VOID
ConfigureP2sbSbiLock (
  IN  CONST PCH_P2SB_CONFIG             *P2sbConfig
  );

/**
  Bios will remove the host accessing right to PSF register range
  prior to any 3rd party code execution.

  1) Set P2SB PCI offset C4h [29, 28, 27, 26] to [1, 1, 1, 1]
  2) Set the "Endpoint Mask Lock", P2SB PCI offset E2h bit[1] to 1.

  @param[in] P2sbConfig                 The PCH policy for P2SB configuration
**/
VOID
RemovePsfAccess (
  IN  CONST PCH_P2SB_CONFIG             *P2sbConfig
  );

/**
  Configure PMC static function disable lock
**/
VOID
ConfigurePmcStaticFunctionDisableLock (
  VOID
  );

/**
  Print registers value

  @param[in] PrintMmioBase       Mmio base address
  @param[in] PrintSize           Number of registers
  @param[in] OffsetFromBase      Offset from mmio base address

  @retval None
**/
VOID
PrintRegisters (
  IN  UINTN        PrintMmioBase,
  IN  UINT32       PrintSize,
  IN  UINT32       OffsetFromBase
  );

VOID
PrintPchPciConfigSpace (
  VOID
  );

/**
  Check if RST PCIe Storage Remapping is enabled based on policy

  @retval TRUE                RST PCIe Storage Remapping is enabled
  @retval FALSE               RST PCIe Storage Remapping is disabled
**/
BOOLEAN
IsRstPcieStorageRemapEnabled (
  IN  CONST PCH_SATA_CONFIG   *SataConfig,
  IN UINT8                     SataControllerNo
  );
#endif
