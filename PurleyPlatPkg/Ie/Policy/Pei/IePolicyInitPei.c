/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  IePolicyInitPei.c

@brief:

  This file is SampleCode for Intel IE PEI Platform Policy initialization.

**/

#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include "Ppi/IePlatformPolicyPei.h"
#include "Library/IePlatformPolicyUpdatePeiLib.h"

#include <Library/IoLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/PchPcrLib.h>
#include <Register/PchRegsPsf.h>
#include <Library/IePeiPolicyAccessLib.h>
#include <PchAccess.h>  // for V_PCH_DEFAULT_SID
#include <Library/PchPmcLib.h>

#include <IeHeciRegs.h>

/*****************************************************************************
* Local function prototypes.
*****************************************************************************/
VOID
IeConfigurePciDevices(
  IN     PEI_IE_PLATFORM_POLICY_PPI    *pIePolicy);

VOID
IeConfigurePciDevice(
  IN     UINT8    FunNo,
  IN     BOOLEAN  Enabled,
  IN     UINT32   SidSvid);


/*****************************************************************************
* Function definitions.
*****************************************************************************/

/**

  @brief
  Initialize Intel IE PEI Platform Policy

  @param[in] FfsHeader            Pointer to Firmware File System file header.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures.

**/
EFI_STATUS
IePolicyInitPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS                  Status;
  PEI_IE_PLATFORM_POLICY_PPI  *IePlatformPolicyPpi;
  EFI_PEI_PPI_DESCRIPTOR      *IePlatformPolicyPpiDesc;

  ///
  /// Allocate descriptor and PPI structures
  ///
  IePlatformPolicyPpi = (PEI_IE_PLATFORM_POLICY_PPI *) AllocateZeroPool (sizeof (PEI_IE_PLATFORM_POLICY_PPI));
  if (IePlatformPolicyPpi == NULL) {
    ASSERT(IePlatformPolicyPpi != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  IePlatformPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  if (IePlatformPolicyPpiDesc == NULL) {
    FreePool(IePlatformPolicyPpi);
    ASSERT(IePlatformPolicyPpiDesc != NULL);
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// Initialize the PPI
  ///
  IePlatformPolicyPpiDesc->Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  IePlatformPolicyPpiDesc->Guid   = &gPeiIePlatformPolicyPpiGuid;


  ///
  /// Initialize the Platform Configuration
  ///
  UpdatePeiIePlatformPolicy (IePlatformPolicyPpi);

  IePlatformPolicyPpiDesc->Ppi = IePlatformPolicyPpi;

  IeConfigurePciDevices(IePlatformPolicyPpi);

  ///
  /// Install the IE PEI Platform Policy PPI
  ///
  Status = (**PeiServices).InstallPpi (PeiServices, IePlatformPolicyPpiDesc);
  ASSERT_EFI_ERROR (Status);
  DEBUG((DEBUG_INFO, "[IE Policy] IE PEI Platform Policy PPI Installed\n"));

  return Status;
}


/**
  Enable or disable IE device function on PCI.

  If SidSvid is not zero then it will be written to the SID/SVID registers
  in the PCI config space.

  NOTE: SID/SVID are write-once registers. If already configured writing new
        value will fail.
  
  @param[in] FunNo    Function number on IE PCI device
  @param[in] Enabled  Desired state of the IE device function
  @param[in] SidSvid  Optional Subsystem ID + Subsystem Vendor ID for the function
**/
VOID
IeConfigurePciDevice(
  IN     UINT8    FunNo,
  IN     BOOLEAN  Enabled,
  IN     UINT32   SidSvid)
{
  STATIC CONST UINT16 FunNo2RegBase[] = {R_PCH_PCR_PSF3_T0_SHDW_HECI1_REG_BASE,
                                         R_PCH_PCR_PSF3_T0_SHDW_HECI2_REG_BASE,
                                         R_PCH_PCR_PSF3_T0_SHDW_IDER_REG_BASE,
                                         R_PCH_PCR_PSF3_T0_SHDW_KT_REG_BASE,
                                         R_PCH_PCR_PSF3_T0_SHDW_HECI3_REG_BASE};
  STATIC CONST CHAR8 *FunNo2DevName[] = {"HECI1", "HECI2", "IDEr", "KT", "HECI3"};
  
  UINT16 PsfRegBase;
  
  if (FunNo >= sizeof(FunNo2RegBase)/sizeof(FunNo2RegBase[0])) {
    ASSERT(FALSE);
    return;
  }
  PsfRegBase = FunNo2RegBase[FunNo];
  
  if (Enabled) {
    PchPcrAndThenOr32(PID_PSF3, PsfRegBase + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
                      (UINT32)~B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS, 0);
    DEBUG((DEBUG_INFO, "[IE] %a: enabled\n", FunNo2DevName[FunNo]));
    DEBUG((DEBUG_INFO, "[IE] %a: Set SID/SVID to %08X\n", FunNo2DevName[FunNo], SidSvid));
    MmioWrite32(MmPciBase(IE_BUS, IE_DEV, FunNo) +  HECI_R_SUBSYSID, SidSvid);
    //
    // If the SID/SVID registers were already written they became read-only.
    // Warn about such situation, it means error in firmware flow.
    //
    if (MmioRead32(MmPciBase(IE_BUS, IE_DEV, FunNo) +  HECI_R_SUBSYSID) != SidSvid) {
      DEBUG((DEBUG_ERROR, "[IE] ERROR: Setting SID/SVID for %a failed\n", FunNo2DevName[FunNo]));
    }
  } else {
    PchPcrAndThenOr32(PID_PSF3, PsfRegBase + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
                      (UINT32)~0, B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS);
    DEBUG((DEBUG_INFO, "[IE] %a: disabled\n", FunNo2DevName[FunNo]));
  }
}


/**
  Send HECI message to signal that memory is ready to use after MRC phase.

  @param[in]      HeciDevReg         PSF Register Base for device.
  @param[in]      Enabled            State of the device to set.
**/
VOID
IeConfigurePciDevices(
  IN     PEI_IE_PLATFORM_POLICY_PPI    *pIePolicy)
{
  UINT32     SidSvid = (V_PCH_DEFAULT_SID << 16) | V_PCH_INTEL_VENDOR_ID;
  BOOLEAN    IeHeci1Enabled = FALSE;
  BOOLEAN    IeHeci2Enabled = FALSE;
  BOOLEAN    IeHeci3Enabled = FALSE;
  BOOLEAN    IeIderEnabled = FALSE;
  BOOLEAN    IeKtEnabled = FALSE;
  BOOLEAN    IeEnabled;
  IEFS1      Iefs;

  if (PchIsDwrFlow()) {
    DEBUG((DEBUG_WARN, "[IE] WARNING: DWR detected - no IE Pci devices configured\n"));
    return;
  }

  IeEnabled = IeIsEnabled();
  DEBUG((DEBUG_INFO, "[IE] IE in soft-strap is %a\n", IeEnabled ? "ENABLED" : "DISABLED"));
  
  if (IeEnabled) {
    Iefs.DWord = IeHeciPciReadIefs1();
    DEBUG((DEBUG_INFO, "[IE] IEFS: %08X\n", Iefs.DWord));
    if (Iefs.DWord != (UINT32)-1 && Iefs.Bits.CurrentState == IEFS1_CURSTATE_DFXFW) {
      IeHeci1Enabled = TRUE;
      IeHeci2Enabled = TRUE;
      IeHeci3Enabled = TRUE;
      IeIderEnabled = TRUE;
      IeKtEnabled = TRUE;
      DEBUG((DEBUG_INFO, "[IE] DFX detected, enabling IE devices\n"));
    } else if (pIePolicy != NULL) {
      SidSvid = (pIePolicy->SubsystemId << 16) | pIePolicy->SubsystemVendorId;
      IeHeci1Enabled = pIePolicy->IeHeci1Enabled;
      IeHeci2Enabled = pIePolicy->IeHeci2Enabled;
      IeHeci3Enabled = pIePolicy->IeHeci3Enabled;
      IeIderEnabled = pIePolicy->IeIderEnabled;
      IeKtEnabled = pIePolicy->IeKtEnabled;
    } else {
      ASSERT(pIePolicy != NULL);
    }
  }
  
  IeConfigurePciDevice(IE_FUN_HECI1, IeHeci1Enabled, SidSvid);
  IeConfigurePciDevice(IE_FUN_HECI2, IeHeci2Enabled, SidSvid);
  IeConfigurePciDevice(IE_FUN_HECI3, IeHeci3Enabled, SidSvid);
  IeConfigurePciDevice(IE_FUN_IDER, IeIderEnabled, SidSvid);
  IeConfigurePciDevice(IE_FUN_KT, IeKtEnabled, SidSvid);
  
  return;
}

