/** @file
  This is the Common driver that initializes the Intel PCH.

@copyright
  Copyright (c) 1999 - 2017 Intel Corporation. All rights reserved
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

#include <Protocol/HeciProtocol.h>
#include <Library/HeciMsgLib.h>
#include <Library/PlatformHooksLib.h>

//
// Module variables
//
extern UINT16                                          mPcieIoTrapAddress;

GLOBAL_REMOVE_IF_UNREFERENCED PCH_POLICY_HOB           *mPchPolicyHob;


//
// EFI_EVENT
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_EVENT mHeciEvent;



/**
  <b>PchInit DXE Module Entry Point</b>\n
  - <b>Introduction</b>\n
      The PchInit module is a DXE driver that initializes the Intel Platform Controller Hub
      following the PCH BIOS specification and EDS requirements and recommendations. It consumes
      the PCH_POLICY_HOB SI_POLICY_HOB for expected configurations per policy.
      This is the standard EFI driver point that detects whether there is an supported PCH in
      the system and if so, initializes the chipset.

  - <b>Details</b>\n
    This module is required for initializing the Intel Platform Controller Hub to
    follow the PCH BIOS specification and EDS.
    This includes some initialization sequences, enabling and disabling PCH devices,
    configuring clock gating, RST PCIe Storage Remapping, SATA controller, ASPM of PCIE devices. Right before end of DXE,
    it's responsible to lock down registers for security requirement.

  - @pre
    - PCH PCR base address configured
    - EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
      - This is to ensure that PCI MMIO and IO resource has been prepared and available for this driver to allocate.

  - @result
    - Publishes the @link _PCH_INFO_PROTOCOL PCH_INFO_PROTOCOL @endlink
    - Publishes the @link _PCH_EMMC_TUNING_PROTOCOL PCH_EMMC_TUNING_PROTOCOL @endlink

  - <b>References</b>\n
    - @link _PCH_POLICY PCH_POLICY_HOB @endlink.
    - @link _SI_POLICY_STRUCT SI_POLICY_HOB @endlink.

  - <b>Integration Checklists</b>\n
    - Verify prerequisites are met. Porting Recommendations.
    - No modification of this module should be necessary
    - Any modification of this module should follow the PCH BIOS Specification and EDS


**/
VOID
PchInitEntryPoint (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS        HobPtr;
  UINTN                       DummyHandle;
  EFI_STATUS                  Status;

  DEBUG ((DEBUG_INFO, "Common PchInitEntryPoint() Start\n"));

  //
  // Get PCH Policy HOB.
  //
  HobPtr.Guid = GetFirstGuidHob (&gPchPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  if (HobPtr.Guid == NULL) {
    return;
  }
  mPchPolicyHob = GET_GUID_HOB_DATA(HobPtr.Guid);


  if (mPchPolicyHob->SataConfig.Rst.EfiRaidDriverLoad == TRUE){
    Status = gBS->LocateProtocol(&gPchSataEfiLoadProtocolGuid, NULL, (VOID **)&DummyHandle );
    if (EFI_ERROR(Status)) {
       Status = gBS->InstallProtocolInterface (
                     &mImageHandle,
                     &gPchSataEfiLoadProtocolGuid,
                     EFI_NATIVE_INTERFACE,
                     NULL
                    );
      if (EFI_ERROR(Status)) {
         DEBUG ((DEBUG_ERROR,"Sata RSTe EFI OpRom load NOT triggered (Status:%g)!!!\n", Status));
      } else {
         DEBUG ((DEBUG_INFO,"Sata RSTe EFI OpRom load triggered\n"));
      }
    }
  }
  if (mPchPolicyHob->sSataConfig.Rst.EfiRaidDriverLoad == TRUE){
    Status = gBS->LocateProtocol(&gPchsSataEfiLoadProtocolGuid, NULL, (VOID **)&DummyHandle );
    if (EFI_ERROR(Status)) {
       Status = gBS->InstallProtocolInterface (
                     &mImageHandle,
                     &gPchsSataEfiLoadProtocolGuid,
                     EFI_NATIVE_INTERFACE,
                     NULL
                    );
      if (EFI_ERROR(Status)) {
         DEBUG ((DEBUG_ERROR,"Secondary Sata RSTe EFI OpRom load NOT triggered (Status:%g)!!!\n", Status));
      } else {
         DEBUG ((DEBUG_INFO,"Secondary Sata RSTe EFI OpRom load triggered\n"));
      }
    }
  }
  DEBUG ((DEBUG_INFO, "Common PchInitEntryPoint() End\n"));

  return;
}

/**
  Lock USB registers before boot

  @param[in] UsbConfig      The PCH Policy for USB configuration
**/
VOID
UsbEndOfInit (
  IN  CONST PCH_USB_CONFIG    *UsbConfig
  )
{
  UINTN                       XhciPciMmBase;
  UINT32                      XhccCfg;

  DEBUG ((DEBUG_INFO, "UsbEndOfInit() - Start\n"));

  XhciPciMmBase = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_XHCI,
                      PCI_FUNCTION_NUMBER_PCH_XHCI
                      );

  ///
  /// PCH BIOS Spec Section 13.2.4 Locking xHCI Register Settings
  /// PCH BIOS Spec Locking xHCI Register settings
  /// After xHCI is initialized, BIOS should lock the xHCI configuration registers to RO.
  /// This prevent any unintended changes. There is also a lockdown feature for OverCurrent
  /// registers. BIOS should set these bits to lock down the settings prior to end of POST.
  /// 1. Set Access Control bit at XHCI PCI offset 40h[31] to 1b to lock xHCI register settings.
  /// 2. Set OC Configuration Done bit at XHCI PCI offset 44h[31] to lock overcurrent mappings from
  ///    further changes.
  ///
  MmioOr32 (XhciPciMmBase + R_PCH_XHCI_XHCC2, (UINT32) (BIT31));

  ///
  /// PCH BIOS Spec xHCI controller setup
  /// Note:
  /// XHCI PCI offset 40h is write once register.
  /// Unsupported Request Detected bit is write clear
  ///
  XhccCfg = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_XHCC1);
  XhccCfg &= (UINT32) ~(B_PCH_XHCI_XHCC1_URD);
  XhccCfg |= (UINT32) (B_PCH_XHCI_XHCC1_ACCTRL);
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_XHCC1, XhccCfg);

  DEBUG ((DEBUG_INFO, "UsbEndOfInit() - End\n"));
}

/**
  Process all the lock downs

  @retval     EFI_SUCCESS        All lock downs successful
  @retval     Others             Not successful
**/
EFI_STATUS
ProcessAllLocks (
  VOID
  )
{
  EFI_STATUS    Status;
  UINTN         Index;
  UINT8         Data8;
  UINT16        Data16;
  UINT16        Data16And;
  UINT16        Data16Or;
  UINT32        Data32;
  UINT32        Data32And;
  UINT32        Data32Or;
  UINT32        DlockValue;
  UINTN         PciLpcRegBase;
  UINTN         PciSpiRegBase;
  UINTN         PciPmcRegBase;
  UINT16        ABase;
  UINT16        TcoBase;
  UINT32        PchPwrmBase;
  UINT32        PchSpiBar0;
  PCH_SERIES    PchSeries;
  EFI_STATUS    GpioStatus;
  UINTN         SmbusRegBase;
  UINT32        Timer;
  UINTN         Mrom0RegBase;
  
  Mrom0RegBase = MmPciBase (
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_EVA,
                  PCI_FUNCTION_NUMBER_EVA_MROM0
                  );
  SmbusRegBase    = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_SMBUS,
                      PCI_FUNCTION_NUMBER_PCH_SMBUS
                      );

  Status = EFI_SUCCESS;

  PciLpcRegBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC
                      );
  PciPmcRegBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_PMC,
                      PCI_FUNCTION_NUMBER_PCH_PMC
                      );
  PciSpiRegBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_SPI,
                      PCI_FUNCTION_NUMBER_PCH_SPI
                      );
                      
  //
  // Make sure SPI BAR0 has fixed address before writing to boot script. 
  // The same base address is set in PEI and will be used during resume. 
  // 
  PchSpiBar0 = PCH_SPI_BASE_ADDRESS; 

  MmioAnd8    (PciSpiRegBase + PCI_COMMAND_OFFSET, (UINT8) ~EFI_PCI_COMMAND_MEMORY_SPACE); 
  MmioWrite32 (PciSpiRegBase + R_PCH_SPI_BAR0, PchSpiBar0); 
  MmioOr8     (PciSpiRegBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE); 


  PchAcpiBaseGet (&ABase);
  PchTcoBaseGet (&TcoBase);

  PchSeries = GetPchSeries ();

#ifndef FSP_FLAG
  ///
  /// Set PWRMBASE + 620h [31] to lock the ST and NST PG register fields.
  ///
  ConfigurePmcStaticFunctionDisableLock ();

  Data32And = 0xFFFFFFFF;
  Data32Or = 0;


  ///
  /// SKL PCH BWG 7.2.4 Additional PCH DMI and OP-DMI Programming Steps
  /// Step 9.2
  /// Set PCR[DMI] + 2234h [31] = 1b.
  /// Leave this in DXE since setting it in PEI would break the ActiveBIOS module.
  ///
  Data32And = 0xFFFFFFFF;
  Data32Or = B_PCH_PCR_DMI_DMIC_SRL;


  PchPcrAndThenOr32 (
    PID_DMI, R_PCH_PCR_DMI_DMIC,
    Data32And,
    Data32Or
    );
  PCH_PCR_BOOT_SCRIPT_READ_WRITE (
    S3BootScriptWidthUint32,
    PID_DMI, R_PCH_PCR_DMI_DMIC,
    &Data32Or,
    &Data32And
    );
#endif // FSP_FLAG

  //
  // Lock XHCI registers
  //
  UsbEndOfInit (&(mPchPolicyHob->UsbConfig));


    ///
    /// Program the Flash Protection Range Register based on policy
    ///
    DlockValue = MmioRead32 (PchSpiBar0 + R_PCH_SPI_DLOCK);
    for (Index = 0; Index < PCH_FLASH_PROTECTED_RANGES; ++Index) {
      if ((mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].WriteProtectionEnable ||
           mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].ReadProtectionEnable) != TRUE) {
        continue;
      }

      ///
      /// Proceed to program the register after ensure it is enabled
      ///
      Data32 = 0;
      Data32 |= (mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].WriteProtectionEnable == TRUE) ? B_PCH_SPI_PRX_WPE : 0;
      Data32 |= (mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].ReadProtectionEnable == TRUE) ? B_PCH_SPI_PRX_RPE : 0;
      Data32 |= ((UINT32) mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].ProtectedRangeLimit << N_PCH_SPI_PRX_PRL) & B_PCH_SPI_PRX_PRL_MASK;
      Data32 |= ((UINT32) mPchPolicyHob->FlashProtectConfig.ProtectRange[Index].ProtectedRangeBase << N_PCH_SPI_PRX_PRB) & B_PCH_SPI_PRX_PRB_MASK;
      DEBUG ((DEBUG_INFO, "Protected range %d: 0x%08x \n", Index, Data32));

      DlockValue |= (UINT32) (B_PCH_SPI_DLOCK_PR0LOCKDN << Index);
      MmioWrite32 ((UINTN) (PchSpiBar0 + (R_PCH_SPI_PR0 + (Index * S_PCH_SPI_PRX))), Data32);
      S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
        (UINTN) (PchSpiBar0 + (R_PCH_SPI_PR0 + (Index * S_PCH_SPI_PRX))),
        1,
        (VOID *) (UINTN) (PchSpiBar0 + (R_PCH_SPI_PR0 + (Index * S_PCH_SPI_PRX)))
        );
    }
    //
    // Program DLOCK register
    //
    MmioWrite32 ((UINTN) (PchSpiBar0 + R_PCH_SPI_DLOCK), DlockValue);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (PchSpiBar0 + R_PCH_SPI_DLOCK),
      1,
      (VOID *) (UINTN) (PchSpiBar0 + R_PCH_SPI_DLOCK)
      );

    ///
    /// PCH BIOS Spec Section 3.6 Flash Security Recommendation
    /// In PCH SPI controller the BIOS should set the Flash Configuration Lock-Down bit
    /// (SPI_BAR0 + 04[15]) at end of post.  When set to 1, those Flash Program Registers
    /// that are locked down by this FLOCKDN bit cannot be written.
    /// Please refer to the EDS for which program registers are impacted.
    /// Additionally BIOS must program WRSDIS bit (SPI_BAR0 + 0x04[11]) to disable Write Status in HW sequencing
    ///
    
    // 
    // Ensure there is no pending SPI trasaction before setting lock bits 
    // 
    Timer = 0; 
    while (MmioRead16 (PchSpiBar0 + R_PCH_SPI_HSFSC) & B_PCH_SPI_HSFSC_SCIP) { 
      if (Timer > SPI_WAIT_TIME) { 
        //
        // SPI transaction is pending too long at this point, exit with error.
        //
        DEBUG ((DEBUG_ERROR, "SPI Cycle Timeout\n"));
        ASSERT (FALSE); 
        break; 
      } 
      MicroSecondDelay (SPI_WAIT_PERIOD); 
      Timer += SPI_WAIT_PERIOD; 
    } 
    
    Data16And = B_PCH_SPI_HSFSC_SCIP; 
    Data16  = 0; 
    S3BootScriptSaveMemPoll ( 
      S3BootScriptWidthUint16, 
      PchSpiBar0 + R_PCH_SPI_HSFSC, 
      &Data16And, 
      &Data16, 
      SPI_WAIT_PERIOD, 
      SPI_WAIT_TIME / SPI_WAIT_PERIOD 
      ); 
    
    // 
    // Clear any outstanding status 
    // 
    Data16Or  = B_PCH_SPI_HSFSC_SAF_DLE 
              | B_PCH_SPI_HSFSC_SAF_ERROR 
              | B_PCH_SPI_HSFSC_AEL 
              | B_PCH_SPI_HSFSC_FCERR 
              | B_PCH_SPI_HSFSC_FDONE; 
    Data16And = 0xFFFF; 
    MmioAndThenOr16 (PchSpiBar0 + R_PCH_SPI_HSFSC, Data16And, Data16Or); 
    S3BootScriptSaveMemReadWrite ( 
      S3BootScriptWidthUint16, 
      PchSpiBar0 + R_PCH_SPI_HSFSC, 
      &Data16Or, 
      &Data16And 
      ); 
    
    // 
    // Set WRSDIS 
    // 
    Data16Or  = B_PCH_SPI_HSFSC_WRSDIS; 
    Data16And = 0xFFFF; 
    MmioAndThenOr16 (PchSpiBar0 + R_PCH_SPI_HSFSC, Data16And, Data16Or); 
    S3BootScriptSaveMemReadWrite ( 
      S3BootScriptWidthUint16, 
      PchSpiBar0 + R_PCH_SPI_HSFSC, 
      &Data16Or, 
      &Data16And 
      ); 
    
    // 
    // Set FLOCKDN 
    // 
    Data16Or  = B_PCH_SPI_HSFSC_FLOCKDN; 
    Data16And = 0xFFFF; 
    MmioAndThenOr16 (PchSpiBar0 + R_PCH_SPI_HSFSC, Data16And, Data16Or); 
    S3BootScriptSaveMemReadWrite ( 
      S3BootScriptWidthUint16, 
      PchSpiBar0 + R_PCH_SPI_HSFSC, 
      &Data16Or, 
      &Data16And
      );

  ///
  /// SPI Flash Programming Guide Section 5.5.2 Vendor Component Lock
  /// It is strongly recommended that BIOS sets the Vendor Component Lock (VCL) bits. VCL applies
  /// the lock to both VSCC0 and VSCC1 even if VSCC0 is not used. Without the VCL bits set, it is
  /// possible to make Host/GbE VSCC register(s) changes in that can cause undesired host and
  /// integrated GbE Serial Flash functionality.
  ///
  MmioOr32 ((UINTN) (PchSpiBar0 + R_PCH_SPI_SFDP0_VSCC0), B_PCH_SPI_SFDP0_VSCC0_VCL);
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (PchSpiBar0 + R_PCH_SPI_SFDP0_VSCC0),
    1,
    (VOID *) (UINTN) (PchSpiBar0 + R_PCH_SPI_SFDP0_VSCC0)
    );

  ///
  /// Additional Power Management Programming
  /// Step 3
  /// Set GEN_PMCON_LOCK register, PMC PCI offset A6h = 06h, after stretch and ACPI base programming completed.
  ///
  MmioOr8 (
    (UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B + 2),
    (UINT8) ((B_PCH_PMC_GEN_PMCON_B_SLPSX_STR_POL_LOCK | B_PCH_PMC_GEN_PMCON_B_ACPI_BASE_LOCK) >> 16)
    );
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    (UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B + 2),
    1,
    (VOID *) (UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_B + 2)
    );

  ///
  /// PCH BIOS Spec Section 3.6 Flash Security Recommendation
  /// BIOS needs to enable the BIOS Lock Enable (BLE) feature of the PCH by setting
  /// SPI/eSPI/LPC PCI offset DCh[1] = 1b.
  /// When this bit is set, attempts to write the BIOS Write Enable (BIOSWE) bit
  /// in PCH will cause a SMI which will allow the BIOS to verify that the write is
  /// from a valid source.
  /// Remember that BIOS needs to set SPI/LPC/eSPI PCI Offset DC [0] = 0b to enable
  /// BIOS region protection before exiting the SMI handler.
  /// Also, TCO_EN bit needs to be set (SMI_EN Register, ABASE + 30h[13] = 1b) to keep
  /// BLE feature enabled after booting to the OS.
  /// Intel requires that BIOS enables the Lock Enable (LE) feature of the PCH to
  /// ensure SMM protection of flash.
  /// Left to platform code to register a callback function to handle BiosWp SMI
  ///
  if (mPchPolicyHob->LockDownConfig.BiosLock == TRUE) {
    //
    // eSPI/LPC
    //
    if (! (MmioRead8 (PciLpcRegBase + R_PCH_LPC_BC) & B_PCH_LPC_BC_LE)) {
      DEBUG ((DEBUG_INFO, "Set LPC bios lock\n"));
      MmioOr8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC), B_PCH_LPC_BC_LE);
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint8,
        (UINTN) (PciLpcRegBase + R_PCH_LPC_BC),
        1,
        (VOID *) (UINTN) (PciLpcRegBase + R_PCH_LPC_BC)
        );
    }
    //
    // SPI
    //
    if (! (MmioRead8 (PciSpiRegBase + R_PCH_SPI_BC) & B_PCH_SPI_BC_LE)) {
      DEBUG ((DEBUG_INFO, "Set SPI bios lock\n"));
      MmioOr8 ((UINTN) (PciSpiRegBase + R_PCH_SPI_BC), (UINT8) B_PCH_SPI_BC_LE);
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint8,
        (UINTN) (PciSpiRegBase + R_PCH_SPI_BC),
        1,
        (VOID *) (UINTN) (PciSpiRegBase + R_PCH_SPI_BC)
        );
    }
  }
    ///
  /// PCH BIOS Spec Section 3.6 Flash Security Recommendation
  /// BIOS needs to enables SMI_LOCK (PMC PCI offset A0h[4] = 1b) which prevent writes
  /// to the Global SMI Enable bit (GLB_SMI_EN ABASE + 30h[0]). Enabling this bit will
  /// mitigate malicious software attempts to gain system management mode privileges.
    ///
  if (mPchPolicyHob->LockDownConfig.GlobalSmi == TRUE) {
    ///
    /// Save Global SMI Enable bit setting before BIOS enables SMI_LOCK during S3 resume
    ///
    Data32Or = IoRead32 ((UINTN) (ABase + R_PCH_SMI_EN));
    if ((Data32Or & B_PCH_SMI_EN_GBL_SMI) != 0) {
      Data32And = 0xFFFFFFFF;
      Data32Or |= B_PCH_SMI_EN_GBL_SMI;
      S3BootScriptSaveIoReadWrite (
        S3BootScriptWidthUint32,
        (UINTN) (ABase + R_PCH_SMI_EN),
        &Data32Or,  // Data to be ORed
        &Data32And  // Data to be ANDed
        );
    }
    MmioOr8 ((UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_A), B_PCH_PMC_GEN_PMCON_A_SMI_LOCK);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint8,
      (UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_A),
      1,
      (VOID *) (UINTN) (PciPmcRegBase + R_PCH_PMC_GEN_PMCON_A)
      );
  }

  ///
  /// PCH BIOS Spec Section 3.6 Flash Security Recommendation
  /// BIOS also needs to set the BIOS Interface Lock Down bit in multiple locations
  /// (PCR[DMI] + 274Ch[0], LPC/eSPI PCI offset DCh[7] and SPI PCI offset DCh[7]).
  /// Setting these bits will prevent writes to the Top Swap bit (under their respective locations)
  /// and the Boot BIOS Straps. Enabling this bit will mitigate malicious software
  /// attempts to replace the system BIOS option ROM with its own code.
  ///
  if (mPchPolicyHob->LockDownConfig.BiosInterface == TRUE) {
      ///
    /// LPC
      ///
    MmioOr8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC), (UINT32) B_PCH_LPC_BC_BILD);
    S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
      (UINTN) (PciLpcRegBase + R_PCH_LPC_BC),
      1,
      (VOID *) (UINTN) (PciLpcRegBase + R_PCH_LPC_BC)
      );

    ///
    /// Reads back for posted write to take effect
    ///
    Data8 = MmioRead8 ((UINTN) (PciLpcRegBase + R_PCH_LPC_BC));
    S3BootScriptSaveMemPoll  (
    S3BootScriptWidthUint8,
      (UINTN) (PciLpcRegBase + R_PCH_LPC_BC),
      &Data8,  // BitMask
      &Data8,  // BitValue
      1,       // Duration
      1        // LoopTimes
      );

    ///
    /// SPI
    ///
    MmioOr8 ((UINTN) (PciSpiRegBase + R_PCH_SPI_BC), (UINT32) B_PCH_SPI_BC_BILD);
    S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
      (UINTN) (PciSpiRegBase + R_PCH_SPI_BC),
      1,
      (VOID *) (UINTN) (PciSpiRegBase + R_PCH_SPI_BC)
      );

    ///
    /// Reads back for posted write to take effect
    ///
    Data8 = MmioRead8 ((UINTN) (PciSpiRegBase + R_PCH_SPI_BC));
    S3BootScriptSaveMemPoll  (
    S3BootScriptWidthUint8,
      (UINTN) (PciSpiRegBase + R_PCH_SPI_BC),
      &Data8,     // BitMask
      &Data8,     // BitValue
      1,          // Duration
      1           // LoopTimes
      );

    ///
    /// Set PCR[DMI] + 274C[0] = 1b
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or = B_PCH_PCR_DMI_BILD;
    PchPcrAndThenOr32 (PID_DMI, R_PCH_PCR_DMI_GCS, Data32And, Data32Or);
    PCH_PCR_BOOT_SCRIPT_READ_WRITE (
      S3BootScriptWidthUint32,
      PID_DMI, R_PCH_PCR_DMI_GCS,
      &Data32Or,
      &Data32And
      );
  }

  ///
  /// PCH BIOS Spec Section 5.13 BIOS guide on using RTC RAM
  /// For Data integrity protection, set RTC Memory locks (Upper 128 Byte Lock and
  /// Lower 128 Byte Lock) at SBCR[RTC] + 3400h[4] and SBCR[RTC] + 3400h[3]. Note once locked
  /// bytes 0x38 - 0x3F in each of the Upper and Lower Byte blocks, respectively,
  /// cannot be unlocked until next reset.
  ///
  if (mPchPolicyHob->LockDownConfig.RtcLock == TRUE) {
    Data32And = 0xFFFFFFFF;
    Data32Or  = (B_PCH_PCR_RTC_CONF_UCMOS_LOCK | B_PCH_PCR_RTC_CONF_LCMOS_LOCK | B_PCH_PCR_RTC_CONF_RESERVED);
    PchPcrAndThenOr32 (
      PID_RTC, R_PCH_PCR_RTC_CONF,
      Data32And,
      Data32Or
      );
    PCH_PCR_BOOT_SCRIPT_READ_WRITE (
      S3BootScriptWidthUint32,
      PID_RTC, R_PCH_PCR_RTC_CONF,
      &Data32Or,
      &Data32And
      );
  }

  ///
  /// TCO Base Address lock
  ///
  if (mPchPolicyHob->LockDownConfig.TcoLock == TRUE) {
    MmioOr8 (SmbusRegBase + R_PCH_SMBUS_TCOCTL, B_PCH_SMBUS_TCOCTL_TCO_BASE_LOCK);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint8,
      (UINTN) (SmbusRegBase + R_PCH_SMBUS_TCOCTL),
      1,
      (VOID *) (UINTN) (SmbusRegBase + R_PCH_SMBUS_TCOCTL)
      );
  }

  ///
  /// Lock Down TCO
  ///
  Data16And = 0xFFFF;
  Data16Or  = B_PCH_TCO_CNT_LOCK;
  IoOr16 (TcoBase + R_PCH_TCO1_CNT, Data16Or);
  S3BootScriptSaveIoReadWrite (
    S3BootScriptWidthUint16,
    (UINTN) (TcoBase + R_PCH_TCO1_CNT),
    &Data16Or,  // Data to be ORed
    &Data16And  // Data to be ANDed
    );

  ///
  /// PCH BIOS Spec Section 5.15.1 Additional Chipset Initialization
  /// Step 1
  /// Set SPIBAR0 + F0h [0] to 1b
  ///
  MmioOr8 (PchSpiBar0 + R_PCH_SPI_SSML, B_PCH_SPI_SSML_SSL);

  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint8,
    (UINTN) (PchSpiBar0 + R_PCH_SPI_SSML),
    1,
    (VOID *) (UINTN) (PchSpiBar0 + R_PCH_SPI_SSML)
  );


  if (mPchPolicyHob->LockDownConfig.GpioLockDown == TRUE) {
    GpioStatus = GpioLockGpios ();
    if (GpioStatus != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "ProcessAllLocks: GpioLockGpios() failed: %r\n", GpioStatus));
    }
  }

  //
  // Apply MSDEVFUNCHIDE lock
  //
  Data32 = B_PCH_EVA_MSUNIT_MSDEVFUNCHIDE_REGLOCK;
  MmioOr32(Mrom0RegBase + R_PCH_LBG_MSUINT_MSDEVFUNCHIDE, Data32);

  S3BootScriptSaveMemWrite (
  S3BootScriptWidthUint32,
    (UINTN) (Mrom0RegBase + R_PCH_LBG_MSUINT_MSDEVFUNCHIDE),
    1,
    (VOID *) (UINTN) (Mrom0RegBase + R_PCH_LBG_MSUINT_MSDEVFUNCHIDE)
    );

  //
  // Lock Down PMC
  // Set PWRM + 0x18 [27, 22] prior to OS.
  //
  PchPwrmBaseGet (&PchPwrmBase);
  Data32Or = BIT27;
  if (mPchPolicyHob->PmConfig.PmcReadDisable) {
    Data32Or |= BIT22;
  }
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG, Data32Or) ;
  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINTN) (PchPwrmBase + R_PCH_PWRM_CFG),
    1,
    (VOID *) (UINTN) (PchPwrmBase + R_PCH_PWRM_CFG)
    );

  return Status;
}


/**
  This function process P2SB configuration when End Of Dxe callback
  This MUST be executed in very end of End Of Dxe
**/
VOID
ConfigureP2sbOnEndOfDxe (
  VOID
  )
{
  PCH_SERIES                            PchSeries;
  PCH_STEPPING                          PchStep;
  BOOLEAN                               P2sbOrgStatus;
  UINTN                                 P2sbBase;
  DEBUG ((DEBUG_INFO, "ConfigureP2sbOnEndOfDxe() Start\n"));
  ///
  /// Lock down the P2sb SBI before going into OS.
  ///
  ConfigureP2sbSbiLock (&mPchPolicyHob->P2sbConfig);

  PchSeries = GetPchSeries ();
  PchStep   = PchStepping ();
  P2sbBase  = MmPciBase (
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_PCH_P2SB,
                 PCI_FUNCTION_NUMBER_PCH_P2SB
              );

  if (((PchSeries == PchH ) && (PchStep == PchHA0 )) || (mPchPolicyHob->P2sbConfig.P2SbReveal == TRUE)) {
    //
    // Skip Hiding P2sb for SPT PCH H A0
    //
    PchRevealP2sb (P2sbBase, &P2sbOrgStatus);
  }

  DEBUG ((DEBUG_INFO, "ConfigureP2sbOnEndOfDxe() End\n"));
}

/**
  Common PCH initialization before Boot Sript Table is closed

**/
VOID
PchOnEndOfDxe (
    VOID
  )
{
  EFI_STATUS                                Status;
  UINT32                                    Data32And;
  UINT32                                    Data32Or;
#ifndef FSP_FLAG
  PCH_PCIE_IOTRAP_PROTOCOL                  *PchPcieIoTrapProtocol;
#endif

  DEBUG ((DEBUG_INFO, "Common PchOnEndOfDxe() Start\n"));

  ///
  /// Perform remaining configuration for PCH SATA on End of DXE
  ///
  ConfigurePchSataOnEndOfDxe (PCH_SATA_FIRST_CONTROLLER);
  if(GetIsPchsSataPresent() == TRUE){
    ConfigurePchSataOnEndOfDxe (PCH_SATA_SECOND_CONTROLLER);
  }

#ifndef FSP_FLAG

  Status = gBS->LocateProtocol (&gPchPcieIoTrapProtocolGuid, NULL, (VOID **) &PchPcieIoTrapProtocol);
  ASSERT_EFI_ERROR (Status);
  mPcieIoTrapAddress = PchPcieIoTrapProtocol->PcieTrapAddress;
  DEBUG ((DEBUG_INFO, "PcieIoTrapAddress: %0x\n", mPcieIoTrapAddress));

#endif // FSP_FLAG

    ///
    /// Check to disable Smbus controller
    ///
  if (mPchPolicyHob->SmbusConfig.Enable == FALSE) {
      Data32And = 0xFFFFFFFF;
    Data32Or  = B_PCH_PCR_SMBUS_GC_FD;
      PchPcrAndThenOr32 (
      PID_SMB, R_PCH_PCR_SMBUS_GC,
        Data32And,
        Data32Or
        );
      PCH_PCR_BOOT_SCRIPT_READ_WRITE (
        S3BootScriptWidthUint32,
      PID_SMB, R_PCH_PCR_SMBUS_GC,
        &Data32Or,
        &Data32And
        );
      PchPcrAndThenOr32 (
        PID_PSF2, R_PCH_PCR_PSF3_T0_SHDW_SMBUS_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_PCIEN,
        (UINT32)~(0),
        B_PCH_PCR_PSFX_T0_SHDW_PCIEN_FUNDIS
        );
  }

  Status = ProcessAllLocks ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Error when processing locks in PchOnEndOfDxe \n"));
    ASSERT (FALSE);
  }
#ifndef FSP_FLAG

  if (mPcieIoTrapAddress != 0) {
    //
    // Save PCH PCIE IoTrap address to re-config PCIE power management setting after resume from S3
    //
    Data32Or = PciePmTrap;
    S3BootScriptSaveIoWrite (
      S3BootScriptWidthUint32,
      (UINTN) (mPcieIoTrapAddress),
      1,
      &Data32Or
      );
  } else {
    ASSERT (FALSE);
  }
#endif // FSP_FLAG


  //
  // This function process P2SB configuration when End Of Dxe callback
  // This MUST be executed in very end of End Of Dxe
  // All PSF access after this function will not acceptable
  //
  ConfigureP2sbOnEndOfDxe ();

  DEBUG ((DEBUG_INFO, "Common PchOnEndOfDxe() End\n"));

  return;
}
