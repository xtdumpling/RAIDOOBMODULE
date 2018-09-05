/** @file
  Pch common library for PCH INIT PEI/DXE/SMM modules

@copyright
  Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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

#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/PchPmcLib.h>
#include <PchAccess.h>
#include <PchPolicyCommon.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/PchPcieRpLib.h>
#include <Library/PchP2sbLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/PchInfoLib.h>
#include <Library/S3BootScriptLib.h>

extern CONST PCH_PCIE_CONTROLLER_INFO mPchPcieControllerInfo[];
extern CONST UINT32 mPchPcieControllerInfoSize;


/**
  This function returns PID according to PCIe controller index

  @param[in] ControllerIndex     PCIe controller index

  @retval PCH_SBI_PID    Returns PID for SBI Access
**/
PCH_SBI_PID
PchGetPcieControllerSbiPid (
  IN  UINT32  ControllerIndex
  )
{
  ASSERT (ControllerIndex < mPchPcieControllerInfoSize);
  return mPchPcieControllerInfo[ControllerIndex].Pid;
}

/**
  This function returns PID according to Root Port Number

  @param[in] RpIndex     Root Port Index (0-based)

  @retval PCH_SBI_PID    Returns PID for SBI Access
**/
PCH_SBI_PID
GetRpSbiPid (
  IN  UINTN  RpIndex
  )
{
  return PchGetPcieControllerSbiPid ((UINT32) (RpIndex / PCH_PCIE_CONTROLLER_PORTS));
}
// SERVER_BIOS_FLAG
/**
  Calculate root port device number based on physical port index.

  @param[in]  RpIndex              Root port index (0-based).

  @retval     Root port device number.
**/
UINT32
PchGetPcieRpDevice (
  IN  UINT32   RpIndex
  )
{
  UINTN ControllerIndex;
  ControllerIndex = RpIndex / PCH_PCIE_CONTROLLER_PORTS;
  ASSERT (ControllerIndex < mPchPcieControllerInfoSize);
  return mPchPcieControllerInfo[ControllerIndex].DevNum;
}

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
  )
{
  EFI_STATUS    Status;
  UINT32        RpDevice;
  UINT8         Response;
  UINT16        Fid;


  RpDevice = PchGetPcieRpDevice (RpIndex);
  Fid = (UINT16) ((RpDevice << 3) | (RpIndex % 4 ));
  Status = PchSbiExecutionEx (
             GetRpSbiPid (RpIndex),
             Offset,
             PciConfigRead,
             FALSE,
             0xF,
             0,
             Fid,
             Data32,
             &Response
             );
  if (Status != EFI_SUCCESS) {
    DEBUG((DEBUG_ERROR,"Sideband Read Failed\n"));
    ASSERT (FALSE);
  }
  return Status;
}

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
  )
{
  EFI_STATUS  Status;
  UINT32      RpDevice;
  UINT32      Data32;
  UINT8       Response;
  UINT16      Fid;

  RpDevice = PchGetPcieRpDevice (RpIndex);
  Status = PchSbiRpPciRead32 (RpIndex, Offset, &Data32);
  if (Status == EFI_SUCCESS) {
    Data32 &= Data32And;
    Data32 |= Data32Or;
    Fid = (UINT16) ((RpDevice << 3) | (RpIndex % 4 ));
    Status = PchSbiExecutionEx (
               GetRpSbiPid (RpIndex),
               Offset,
               PciConfigWrite,
               FALSE,
               0xF,
               0,
               Fid,
               &Data32,
               &Response
               );
    if (Status != EFI_SUCCESS) {
      DEBUG((DEBUG_ERROR,"Sideband Write Failed\n"));
      ASSERT (FALSE);
    }
  } else {
    ASSERT (FALSE);
  }
  return Status;
}

/**
  Check if RST PCIe Storage Remapping is enabled based on policy

  @param[in] SataConfig             The PCH Policy for SATA configuration

  @retval TRUE                      RST PCIe Storage Remapping is enabled
  @retval FALSE                     RST PCIe Storage Remapping is disabled
**/
BOOLEAN
IsRstPcieStorageRemapEnabled (
  IN  CONST PCH_SATA_CONFIG   *SataConfig,
  IN UINT8                     SataControllerNo
  )
{
  BOOLEAN                      RstPcieStorageRemapEnabled;
  UINTN                        Index;

  RstPcieStorageRemapEnabled  = FALSE;

  DEBUG ((DEBUG_INFO, "IsRstPcieStorageRemapEnabled Started\n"));

  for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
    if (SataConfig->RstPcieStorageRemap[Index].Enable == 1) {
      RstPcieStorageRemapEnabled = TRUE;
    }
  }

  DEBUG ((DEBUG_INFO, "IsRstPcieStorageRemapEnabled Ended\n"));
  return RstPcieStorageRemapEnabled;
}

/**
  This function lock down the P2sb SBI before going into OS.
  This only apply to PCH B0 onward.

  @param[in] P2sbConfig                 The PCH policy for P2SB configuration
**/
VOID
ConfigureP2sbSbiLock (
  IN  CONST PCH_P2SB_CONFIG             *P2sbConfig
  )
{
  UINTN                                 P2sbBase;
  BOOLEAN                               P2sbOrgStatus;
  PCH_STEPPING                          PchStep;
  PCH_SERIES                            PchSeries;

  PchStep     = PchStepping ();
  PchSeries   = GetPchSeries ();

  if (P2sbConfig->SbiUnlock == FALSE)
  {
    P2sbBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_P2SB, PCI_FUNCTION_NUMBER_PCH_P2SB);
    PchRevealP2sb (P2sbBase, &P2sbOrgStatus);

    ///
    /// Set P2SB PCI Offset 0xE0[31] to 1 to lock down SBI interface.
    /// @note: SbiLock is write once. Please make sure it's not touched earlier.
    ///
    MmioOr8 (P2sbBase + R_PCH_P2SB_E0 + 3, BIT7);

    if (!P2sbOrgStatus) {
      PchHideP2sb (P2sbBase);
    }
  }
}

/**
  Bios will remove the host accessing right to PSF register range
  prior to any 3rd party code execution.

#ifndef SERVER_BIOS_FLAG
  1) Set P2SB PCI offset C4h [29, 28, 27, 26] to [1, 1, 1, 1]
  2) Set the "Endpoint Mask Lock!", P2SB PCI offset E2h bit[1] to 1.
#endif

  @param[in] P2sbConfig                 The PCH policy for P2SB configuration
**/
VOID
RemovePsfAccess (
  IN  CONST PCH_P2SB_CONFIG             *P2sbConfig
  )
{
  UINTN                                 P2sbBase;
  BOOLEAN                               P2sbOrgStatus;
  UINT32                                RegisterNo;
  UINT32                                BitNo;
  UINTN                                 Index;

  UINT8 P2SBMaskedDevices[] = {PID_PSF1, PID_PSF2, PID_PSF3, PID_PSF4, PID_PSF5, PID_PSF6, PID_PSF7, PID_PSF8, PID_PSF9, PID_PSF10};
  DEBUG ((DEBUG_INFO, "RemovePsfAccess\n"));

  if (P2sbConfig->PsfUnlock == TRUE) {
    return;
  }

  P2sbBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_P2SB, PCI_FUNCTION_NUMBER_PCH_P2SB);
  PchRevealP2sb (P2sbBase, &P2sbOrgStatus);

  for (Index = 0; Index < sizeof(P2SBMaskedDevices); Index++){
    RegisterNo = (P2SBMaskedDevices[Index] / 32) * sizeof(UINT32); // Calculate which 32 bit mask register
    BitNo = P2SBMaskedDevices[Index] % 32; // Calculate the bit number to be set in the mask register
    MmioOr32 ((UINTN) (P2sbBase + R_PCH_P2SB_EPMASK0 + RegisterNo), (UINT32) 1 << BitNo);
  }
  MmioOr8 (P2sbBase + R_PCH_P2SB_E0 + 2, BIT1);
  if (!P2sbOrgStatus) {
    PchHideP2sb (P2sbBase);
  }
}


/**
  Configure PMC static function disable lock
**/
VOID
ConfigurePmcStaticFunctionDisableLock (
  VOID
  )
{
  UINT32                                PchPwrmBase;
  PchPwrmBaseGet (&PchPwrmBase);

  MmioOr32 (PchPwrmBase + R_PCH_PWRM_ST_PG_FDIS_PMC_1, B_PCH_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK);
}

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
  )
{
  UINT32  Offset;
  DEBUG ((DEBUG_VERBOSE, "       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"));
  for (Offset = 0; Offset < PrintSize; Offset++) {
    if ((Offset % 16) == 0) {
      DEBUG ((DEBUG_VERBOSE, "\n %04X: ", (Offset + OffsetFromBase) & 0xFFF0));
    }
    DEBUG ((DEBUG_VERBOSE, "%02X ", MmioRead8 (PrintMmioBase + Offset)));
  }
  DEBUG ((DEBUG_VERBOSE, "\n"));
}

VOID
PrintPchPciConfigSpace (
  VOID
  )
{
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 00 - 0 ] Host Bridge\n"));
  PrintRegisters (MmPciBase ( 0,0,0 ), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 01 - 0 ] PCI Bridge (0-1) x16@1 (x16)\n"));
  PrintRegisters (MmPciBase ( 0,1,0 ), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 02 - 0 ] IGD\n"));
  PrintRegisters (MmPciBase ( 0,2,0 ), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 03 - 0 ] High Def Audio\n"));
  PrintRegisters (MmPciBase (00, 03, 00), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 20(0x14) - 0 ] XHCI Controller\n"));
  PrintRegisters (MmPciBase (00, 20, 00), 256, 0);
  if (PchIsDwrFlow() == FALSE) {
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 22(0x16) - 0 ] ME\n"));
  PrintRegisters (MmPciBase (00, 22, 00), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 22(0x16) - 2 ] ME IDER\n"));
  PrintRegisters (MmPciBase (00, 22, 02), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 22(0x16) - 3 ] ME SOL\n"));
  PrintRegisters (MmPciBase (00, 22, 03), 256, 0);
  }
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 25(0x19) - 0 ] LAN\n"));
  PrintRegisters (MmPciBase (00, 25, 00), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 26(0x1A) - 0 ] EHCI Controller\n"));
  PrintRegisters (MmPciBase (00, 26, 00), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 27(0x1B) - 0 ] High Def Audio\n"));
  PrintRegisters (MmPciBase (00, 27, 00), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 28(0x1C) - 0 ] PCI Bridge (0-2) x0@1 (x1)\n"));
  PrintRegisters (MmPciBase (00, 28, 00), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "PCI Bridge: AECH\n"));
  PrintRegisters (MmPciBase (00, 28, 00) + 0x0100, 0x040,0x0100);
  DEBUG ((DEBUG_VERBOSE, "PCI Bridge: DFT\n"));
  PrintRegisters (MmPciBase (00, 28, 00) + 0x0310, 0x030,0x0310);
  DEBUG ((DEBUG_VERBOSE, "PCI Bridge: VC0\n"));
  PrintRegisters (MmPciBase (00, 28, 00) + 0x0340, 0x020,0x0340);
  DEBUG ((DEBUG_VERBOSE, "PCI Bridge: Port Configuration Extension\n"));
  PrintRegisters (MmPciBase (00, 28, 00) + 0x0400, 0x020,0x0400);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 28(0x1C) - 6 ] PCI Bridge (0-3:10) x0@1 (x1)\n"));
  PrintRegisters (MmPciBase (00, 28, 06), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 29(0x1D) - 0 ] EHCI Controller\n"));
  PrintRegisters (MmPciBase (00, 29, 00), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 31(0x1F) - 0 ] ISA Bridge\n"));
  PrintRegisters (MmPciBase (00, 31, 00), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 31(0x1F) - 2 ] SATA\n"));
  PrintRegisters (MmPciBase (00, 31, 02), 256, 0);
  DEBUG ((DEBUG_VERBOSE, "[ 0 - 31(0x1F) - 4 ] SMBus Controller\n"));
  PrintRegisters (MmPciBase (00, 31, 04), 256, 0);

  DEBUG ((DEBUG_VERBOSE, "\n\n"));
}
