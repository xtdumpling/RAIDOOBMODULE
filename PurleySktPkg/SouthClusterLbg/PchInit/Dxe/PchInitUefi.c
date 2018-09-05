//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Fixed the COM1/COM2 IO decode/range can't be changed after label 44.
//    Reason:   The root cause is "AMI Generic LPC Super I/O Driver" executed after function
//              ProcessAllLocks, link PchOnEndOfDxeUefi to gExitPmAuthProtocolGuid to fix it.
//    Auditor:  Jimmy Chiu
//    Date:     Aug/15/2016
//
//****************************************************************************
/** @file
  This is the Uefi driver that initializes the Intel PCH.

@copyright
  Copyright (c) 2015 - 2016 Intel Corporation. All rights
  reserved This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted
  by the license, no part of this software or documentation may
  be reproduced, stored in a retrieval system, or transmitted in
  any form or by any means without the express written consent
  of Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/
#include <Token.h>	//SMCPKG_SUPPORT
#include "PchInit.h"

#include <Library/HeciMsgLib.h>
#include <Library/PlatformHooksLib.h> //for IsSimicsPlatform() will remove when Simic fixIsSimicsPlatform

GLOBAL_REMOVE_IF_UNREFERENCED EFI_HANDLE               mImageHandle;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                   mPcieIoTrapAddress;
extern PCH_USB_HC_PORT_PRECONDITION                    *mPrivatePreConditionList;

/**
  PCH initialization before ExitBootServices / LegacyBoot events
  Useful for operations which must happen later than at EndOfPost event

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
PchOnBootToOsUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

/**
  PCH initialization on ExitBootService. This event is used if only ExitBootService is used
  and not in legacy boot

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

  @retval None
**/
VOID
EFIAPI
PchOnExitBootServicesUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

/**
  PCH initialization before boot to OS

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
PchOnReadyToBootUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );


/**
  PCH initialization before Boot Sript Table is closed

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
PchOnEndOfDxeUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

/**
  BIOS W/A for Si bug on PCH stepping LpA0, LpA1, LpA2, LpB0:
  allocates 1MB memory space for ISH BAR,
  overrides 4KB memory allocation done by standard PCI enumarator.

  @retval EFI_STATUS
  @retval EFI_SUCCESS             Allocation successful or stepping other that A0/A1/A2 (W/A not required)
  @retval EFI_OUT_OF_RESOURCES    Memory could not be allocated.
**/


/**
  Reveal TraceHub device which is hidden when PEI memory init done to skip from Pci enumertion.

**/
VOID
PchRevealTraceHub (
  VOID
  )
{
  UINTN  TraceHubBaseAddress;

  DEBUG ((DEBUG_INFO, "PchRevealTraceHub() - Start\n"));

  ///
  /// After PCI Enumeration, TraceHub Device has to be unhidden if it is not power gated
  ///
  if (0 == mPchPolicyHob->PchTraceHubConfig.PchTraceHubHide) {
    PchPcrAndThenOr32 (
      PID_PSF3,
      R_PCH_PCR_PSF3_T0_SHDW_TRACE_HUB_REG_BASE + R_PCH_PCR_PSFX_T0_SHDW_CFG_DIS,
      (UINT32)~BIT0,
      (UINT32)0x00
      );
  }

  TraceHubBaseAddress = MmPciBase (
                            DEFAULT_PCI_BUS_NUMBER_PCH,
                            PCI_DEVICE_NUMBER_PCH_TRACE_HUB,
                            PCI_FUNCTION_NUMBER_PCH_TRACE_HUB
                            );
  DEBUG ((DEBUG_INFO, "PchRevealTraceHub() - BEFORE: SW_LBAR  = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR)));
  MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_UBAR, (UINT32)0x00);
  MmioWrite32 (TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR, PCH_TRACE_HUB_SW_BASE_ADDRESS);
  DEBUG ((DEBUG_INFO, "PchRevealTraceHub() - FW_LBAR  = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_FW_LBAR)));
  DEBUG ((DEBUG_INFO, "PchRevealTraceHub() - MTB_LBAR = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_CSR_MTB_LBAR)));
  DEBUG ((DEBUG_INFO, "PchRevealTraceHub() - SW_LBAR  = 0x%08x\n", MmioRead32(TraceHubBaseAddress + R_PCH_TRACE_HUB_SW_LBAR)));

}

/**
  Initialize PCH Uplinks for Skylake server PCH

  @param[in] none

  @retval FALSE                      Uplinks initalized properly
  @retval TRUE                     Uplinks failed to initialize
**/
VOID
PchUplinksInit(
  VOID
  )
{
  UINT16                            Data16Or;
  UINT16                            Data16And;
  UINT16                            Val16;
  UINT16                            DeviceId;
  UINTN                             Ux16Base = 0;
  UINTN                             Ux8Base = 0;
  UINT32                            BusNo;
  BOOLEAN                           UplinkFound = FALSE;
  UINT32                            UplinkBusNo;
  UINT32                            DevNo;
  UINTN                             VspBase;

  UplinkBusNo = 0;
  ASSERT(mPchPolicyHob != NULL);
  if (mPchPolicyHob == NULL){
    DEBUG ((EFI_D_ERROR, "\nERROR!!! PchUplinksInit() - mPchPolicyHob is a NULL pointer.\n"));
    return;
  }

  //
  // The PCI devices are enumerated. Find which bus has been assigned to the UX16 Uplink port
  //
  for (BusNo = 0; BusNo < PCI_MAX_BUS; BusNo++)
  {
    Ux16Base = MmPciBase (BusNo, PCI_DEVICE_NUMBER_PCH_PCIE_UX16, PCI_FUNCTION_NUMBER_PCH_PCIE_UX16);
    //
    // Read Device ID
    //
    Val16 = MmioRead16(Ux16Base + PCI_DEVICE_ID_OFFSET);
    if ((Val16 == 0xFFFFFFFF) || (Val16 != V_PCH_LBG_PCIE_DEVICE_ID_UX16)){
      continue;
    } else {
      //
      // We've found the Uplink x16 port
      //
      UplinkFound   = TRUE;
      UplinkBusNo = BusNo;
      break;
    }
  }
  if (UplinkFound == TRUE){
    ///
    /// Configure Completion Timeout for Uplink x16 port
    ///
    Data16And = (UINT16)~(B_PCIE_DCTL2_CTD | B_PCIE_DCTL2_CTV);
    Data16Or  = 0;
    if (mPchPolicyHob->PcieConfig.PchPcieUX16CompletionTimeout == PchPcieCompletionTO_Disabled) {
      Data16Or = B_PCIE_DCTL2_CTD;
    } else {
      switch (mPchPolicyHob->PcieConfig.PchPcieUX16CompletionTimeout) {
      case PchPcieCompletionTO_Default:
        Data16Or = V_PCIE_DCTL2_CTV_DEFAULT;
        break;

      case PchPcieCompletionTO_16_55ms:
        Data16Or = V_PCIE_DCTL2_CTV_40MS_50MS;
        break;

      case PchPcieCompletionTO_65_210ms:
        Data16Or = V_PCIE_DCTL2_CTV_160MS_170MS;
        break;

      case PchPcieCompletionTO_260_900ms:
        Data16Or = V_PCIE_DCTL2_CTV_400MS_500MS;
        break;

      case PchPcieCompletionTO_1_3P5s:
        Data16Or = V_PCIE_DCTL2_CTV_1P6S_1P7S;
        break;

      default:
        Data16Or = 0;
        break;
      }
    }
    MmioAndThenOr16 (Ux16Base + R_PCH_PCIE_DCTL2, Data16And, Data16Or);

    ///
    /// Configure Max Payload Size for Uplink x16 port
    ///
    switch (mPchPolicyHob->PcieConfig.PchPcieUX16MaxPayload) {
    case PchPcieMaxPayLoad128B:
      Data16And = (UINT16) ~B_PCIE_DCAP_MPS;
      Data16Or = 0;
      break;
    case PchPcieMaxPayLoad256B:
      Data16And = (UINT16)~(BIT2 | BIT1);
      Data16Or  = BIT0;
      break;
    case PchPcieMaxPayLoad512B:
      Data16And = (UINT16)~(BIT2 | BIT0);
      Data16Or  = BIT1;
      break;
    default:
      Data16And = (UINT16)~(0);
      Data16Or  = 0;
      DEBUG ((DEBUG_ERROR, "PchUplink x16 programming error! Unsupported Max Payload Size! \n"));
    }
    MmioAndThenOr16 (Ux16Base + R_PCH_PCIE_DCAP, Data16And, Data16Or);
  } else {
    DEBUG ((DEBUG_ERROR, "PchUplink programming error! Can not find the Uplink x16 port.\n"));
  }

  if (UplinkFound == TRUE){
    for (DevNo = 0; DevNo < 6; DevNo++) {  //loop VSP
      VspBase = MmPciBase (UplinkBusNo + 1, DevNo, 0);
      Val16 = MmioRead16(VspBase + PCI_DEVICE_ID_OFFSET);
      if (Val16 != 0xFFFFFFFF) {
        MmioOr16 (VspBase + R_PCH_VSPX_ERRCORSTS, (UINT16) B_PCH_VSPX_ERRCORSTS_ANFE);
      }
    }
  }

  DeviceId = GetPchLpcDeviceId();
  if (IS_PCH_LBG_NS_LPC_DEVICE_ID(DeviceId)) {
    //
    // The PCI devices are enumerated. Find which bus has been assigned to the UX8 Uplink port
    //
    UplinkFound = FALSE;
    for (BusNo = 0; BusNo < PCI_MAX_BUS; BusNo++)
    {
      Ux8Base = MmPciBase (BusNo, PCI_DEVICE_NUMBER_PCH_PCIE_UX8, PCI_FUNCTION_NUMBER_PCH_PCIE_UX8);
      //
      // Read Device ID
      //
      Val16 = MmioRead16(Ux8Base + PCI_DEVICE_ID_OFFSET);
      if ((Val16 == 0xFFFFFFFF) || (Val16 != V_PCH_LBG_NS_PCIE_DEVICE_ID_UX8)){
        continue;
      } else {
        //
        // We've found the Uplink x8 port
        //
        UplinkFound = TRUE;
        UplinkBusNo = BusNo;
        break;
      }
    }
    if (UplinkFound == TRUE){
      ///
      /// Configure Completion Timeout for Uplink x8 port
      ///
      Data16And = (UINT16)~(B_PCIE_DCTL2_CTD | B_PCIE_DCTL2_CTV);
      Data16Or  = 0;
      if (mPchPolicyHob->PcieConfig.PchPcieUX8CompletionTimeout == PchPcieCompletionTO_Disabled) {
        Data16Or = B_PCIE_DCTL2_CTD;
      } else {
        switch (mPchPolicyHob->PcieConfig.PchPcieUX8CompletionTimeout) {
        case PchPcieCompletionTO_Default:
          Data16Or = V_PCIE_DCTL2_CTV_DEFAULT;
          break;

        case PchPcieCompletionTO_16_55ms:
          Data16Or = V_PCIE_DCTL2_CTV_40MS_50MS;
          break;

        case PchPcieCompletionTO_65_210ms:
          Data16Or = V_PCIE_DCTL2_CTV_160MS_170MS;
          break;

        case PchPcieCompletionTO_260_900ms:
          Data16Or = V_PCIE_DCTL2_CTV_400MS_500MS;
          break;

        case PchPcieCompletionTO_1_3P5s:
          Data16Or = V_PCIE_DCTL2_CTV_1P6S_1P7S;
          break;

        default:
          Data16Or = 0;
          break;
        }
      }
      MmioAndThenOr16 (Ux8Base + R_PCH_PCIE_DCTL2, Data16And, Data16Or);

      ///
      /// Configure Max Payload Size for Uplink x8 port
      ///
      switch (mPchPolicyHob->PcieConfig.PchPcieUX8MaxPayload) {
      case PchPcieMaxPayLoad128B:
        Data16And = (UINT16) ~B_PCIE_DCAP_MPS;
        Data16Or = 0;
        break;
      case PchPcieMaxPayLoad256B:
        Data16And = (UINT16)~(BIT2 | BIT1);
        Data16Or  = BIT0;
        break;
      case PchPcieMaxPayLoad512B:
        Data16And = (UINT16)~(BIT2 | BIT0);
        Data16Or  = BIT1;
        break;
      default:
        Data16And = (UINT16)~(0);
        Data16Or  = 0;
        DEBUG ((DEBUG_ERROR, "PchUplink x8 programming error! Unsupported Max Payload Size! \n"));
      }
      MmioAndThenOr16 (Ux8Base + R_PCH_PCIE_DCAP, Data16And, Data16Or);
    } else {
      DEBUG ((DEBUG_ERROR, "PchUplink programming error! Can not find the Uplink x8 port.\n"));
    }
  }

  if (UplinkFound == TRUE){
    VspBase = MmPciBase (UplinkBusNo + 1, 2, 0); //VSP, Uplink x8 
    Val16 = MmioRead16(VspBase + PCI_DEVICE_ID_OFFSET);
    if (Val16 != 0xFFFFFFFF) {
      MmioOr16 (VspBase + R_PCH_VSPX_ERRCORSTS, (UINT16) B_PCH_VSPX_ERRCORSTS_ANFE);
    }    
  }
}

/**
  This is the callback function for PCI ENUMERATION COMPLETE.
**/
VOID
EFIAPI
PchOnPciEnumComplete (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS          Status;
  UINTN               PciSpiRegBase;
  VOID                *ProtocolPointer;
  UINTN               ThermalPciBase;
  UINT32              PchPwrmBase;

  ///
  /// Check if this is first time called by EfiCreateProtocolNotifyEvent() or not,
  /// if it is, we will skip it until real event is triggered
  ///
  Status = gBS->LocateProtocol (&gEfiPciEnumerationCompleteProtocolGuid, NULL, (VOID **) &ProtocolPointer);
  if (EFI_SUCCESS != Status) {
     return;
  }
  gBS->CloseEvent (Event);

  PchUplinksInit();
  //
  // Enable LPC MMIO & IO space after BAR reassignment during PCH enumeration
  //
  PciSpiRegBase = MmPciBase (
                          DEFAULT_PCI_BUS_NUMBER_PCH,
                          PCI_DEVICE_NUMBER_PCH_LPC,
                          PCI_FUNCTION_NUMBER_PCH_LPC);
  MmioOr8 (PciSpiRegBase + PCI_COMMAND_OFFSET, (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_IO_SPACE));

  if (IsSimicsPlatform()) {
    //
    // Write ports 0x1ce and 0x1d0 - QemuVideo/BochsID VGA driver requirement
    //
    IoWrite16(0x1ce, 0x0);
    IoWrite16(0x1d0, 0xB0C0);
  }

  PchPwrmBaseGet (&PchPwrmBase);
  if ((MmioRead32 (PchPwrmBase + R_PCH_PWRM_HSWPGCR1) & BIT0) == 0) {
  //
  // Reveal TraceHub controller.
  //
  PchRevealTraceHub ();
  }


  //
  // Enable Thermal MSE
  //
  ThermalPciBase = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_THERMAL,
                     PCI_FUNCTION_NUMBER_PCH_THERMAL
                     );
  if (MmioRead16 (ThermalPciBase + PCI_VENDOR_ID_OFFSET) != 0xFFFF) {
    if (((MmioRead32 (ThermalPciBase + R_PCH_THERMAL_TBAR) & B_PCH_THERMAL_TBAR_MASK) != 0) ||
        ((MmioRead32 (ThermalPciBase + R_PCH_THERMAL_TBARH) != 0)))
    {
      MmioOr8 (ThermalPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
    }
  }
}

VOID
PchRegisterNotifications (
  VOID
  )
{
  EFI_STATUS  Status;
#if SMCPKG_SUPPORT == 0
  EFI_EVENT   EndOfDxeEvent;
#endif
  EFI_EVENT   ReadyToBoot;
  EFI_EVENT   LegacyBootEvent;
  EFI_EVENT   ExitBootServicesEvent;
  VOID        *Registration;

  ///
  /// Create PCI Enumeration Completed callback for PCH
  ///
  EfiCreateProtocolNotifyEvent (
    &gEfiPciEnumerationCompleteProtocolGuid,
    TPL_CALLBACK,
    PchOnPciEnumComplete,
    NULL,
    &Registration
    );

#if SMCPKG_SUPPORT == 1
  /// Link PchOnEndOfDxeUefi to gExitPmAuthProtocolGuid to let 
  /// the SIO IO decode enable/range can be set in PCR register(AMI Generic LPC Super I/O Driver)
  /// before PCR register locked(DMIC.SRL (PCR[DMI] + 2234h [31] = 1 in "ProcessAllLocks")
  ///
  /// Create an ExitPmAuth protocol call back event.
  ///
  Status = EfiNamedEventListen (
                  &gExitPmAuthProtocolGuid,
                  TPL_CALLBACK,
                  PchOnEndOfDxeUefi,
                  NULL,
                  &Registration
                  );
#else
  //
  // Register an end of DXE event for PCH to do tasks before invoking any UEFI drivers,
  // applications, or connecting consoles,...
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  PchOnEndOfDxeUefi,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
#endif

  ASSERT_EFI_ERROR (Status);

  //
  // Register a Ready to boot event to config PCIE power management setting after OPROM executed
  //
  Status = EfiCreateEventReadyToBootEx (
            TPL_CALLBACK,
            PchOnReadyToBootUefi,
            NULL,
            &ReadyToBoot
            );
  ASSERT_EFI_ERROR (Status);

  //
  // Create events for PCH to do the task before ExitBootServices/LegacyBoot.
  // It is guaranteed that only one of two events below will be signalled
  //
  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_CALLBACK,
                  PchOnExitBootServicesUefi,
                  NULL,
                  &ExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);

  Status = EfiCreateEventLegacyBootEx (
            TPL_CALLBACK,
            PchOnBootToOsUefi,
            NULL,
            &LegacyBootEvent
            );
  ASSERT_EFI_ERROR (Status);
}

/**
  Updates PCIe configuration in PCH Info Protocol.

  @param[in,out] PchInfo      Pointer to PCH info protocol instance to be updated
**/
VOID
UpdatePcieConfigInfo (
  IN OUT PCH_INFO_PROTOCOL   *PchInfo
  )
{
  EFI_STATUS  Status;
  PCH_SERIES  PchSeries;
  UINT32      StrapFuseCfg1;
  UINT32      StrapFuseCfg5;
  UINT32      StrapFuseCfg9;
  UINT32      StrapFuseCfg13;
  UINT32      StrapFuseCfg17;

  PchSeries = GetPchSeries ();

  Status = PchSbiRpPciRead32 (
             0,
             R_PCH_PCIE_STRPFUSECFG,
             &StrapFuseCfg1
             );
  ASSERT_EFI_ERROR (Status);

  Status = PchSbiRpPciRead32 (
             4,
             R_PCH_PCIE_STRPFUSECFG,
             &StrapFuseCfg5
             );
  ASSERT_EFI_ERROR (Status);

  Status = PchSbiRpPciRead32 (
             8,
             R_PCH_PCIE_STRPFUSECFG,
             &StrapFuseCfg9
             );
  ASSERT_EFI_ERROR (Status);

  PchInfo->PcieControllerCfg1 = (UINT8) ((StrapFuseCfg1 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC);
  PchInfo->PcieControllerCfg2 = (UINT8) ((StrapFuseCfg5 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC);
  PchInfo->PcieControllerCfg3 = (UINT8) ((StrapFuseCfg9 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC);

  if (PchSeries == PchH) {
    Status = PchSbiRpPciRead32 (
               12,
               R_PCH_PCIE_STRPFUSECFG,
               &StrapFuseCfg13
               );
    ASSERT_EFI_ERROR (Status);
    Status = PchSbiRpPciRead32 (
               16,
               R_PCH_PCIE_STRPFUSECFG,
               &StrapFuseCfg17
               );
    ASSERT_EFI_ERROR (Status);

    PchInfo->PcieControllerCfg4 = (UINT8) ((StrapFuseCfg13 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC);
    PchInfo->PcieControllerCfg5 = (UINT8) ((StrapFuseCfg17 & B_PCH_PCIE_STRPFUSECFG_RPC) >> N_PCH_PCIE_STRPFUSECFG_RPC);
  }
}

/**
  Stores PCIE Gen3 software equalization settings in bootscript to accelerate S3 resume
**/
VOID
SavePcieGen3Eq (
  VOID
  )
{
  UINT8      RpIndex;
  UINTN      RpBase;
  UINT8      EqMethod;
  UINT32     Data32;
  UINT32     Data32Or;
  UINT32     Data32And;

  for (RpIndex = 0; RpIndex < GetPchMaxPciePortNum (); ++RpIndex) {
    RpBase = PchPcieBase (RpIndex);
    //
    // Before trying to save it, make sure software gen3 equalization did actually happen
    //
    if (MmioRead32 (RpBase) == 0xFFFFFFFF) {
      continue;
    }
    if (GetMaxLinkSpeed (RpBase) < 3) {
      continue;
    }
    EqMethod = mPchPolicyHob->PcieConfig.RootPort[RpIndex].Gen3EqPh3Method;
    if (EqMethod != PchPcieEqDefault && EqMethod != PchPcieEqSoftware) {
      continue;
    }
    //
    // Can't check current link speed due to some nonconformant devices
    // Instead we verify B_PCH_PCIE_EQCFG1_RTLEPCEB bit which together with policy check above
    // guarantees that software gen3 eq was performed
    //
    if (!(MmioRead32 (RpBase + R_PCH_PCIE_EQCFG1) & B_PCH_PCIE_EQCFG1_RTLEPCEB)) {
      continue;
    }

    Data32And = 0xFFFFFFFF;
    //
    // Restore PCIE Gen3 equalization coefficients
    //
    Data32 = MmioRead32 (RpBase + R_PCH_PCIE_RTPCL1);
    S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RpBase + R_PCH_PCIE_RTPCL1, 1, &Data32);
    Data32 = MmioRead32 (RpBase + R_PCH_PCIE_RTPCL2);
    S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RpBase + R_PCH_PCIE_RTPCL2, 1, &Data32);
    Data32Or = R_PCH_PCIE_EQCFG1;
    S3BootScriptSaveMemReadWrite (S3BootScriptWidthUint32, RpBase + R_PCH_PCIE_EQCFG1, &Data32Or, &Data32And);

    //
    // Perform link retrain.
    //
    Data32And = (UINT32)(~B_PCIE_LCTL2_TLS);
    Data32Or = V_PCIE_LCTL2_TLS_GEN3;
    S3BootScriptSaveMemReadWrite (S3BootScriptWidthUint32, RpBase + R_PCH_PCIE_LCTL2, &Data32Or, &Data32And);
    Data32And = 0xFFFFFFFF;
    Data32Or = B_PCIE_EX_LCTL3_PE;
    S3BootScriptSaveMemReadWrite (S3BootScriptWidthUint32, RpBase + R_PCH_PCIE_EX_LCTL3, &Data32Or, &Data32And);
    Data32Or = B_PCIE_LCTL_RL;
    S3BootScriptSaveMemReadWrite (S3BootScriptWidthUint32, RpBase + R_PCH_PCIE_LCTL, &Data32Or, &Data32And);
    /// PCH BIOS Spec Section 8.15 Additional PCI Express* Programming Steps
    /// Set "Link Speed Training Policy", Dxx:Fn + D4h[6] to 1.
    /// Make sure this is after mod-PHY related programming is completed.
    Data32Or = B_PCH_PCIE_MPC2_LSTP;
    DEBUG ((DEBUG_WARN, "Gen3Eq S3MmioOr32 0x%08x\n", RpBase + R_PCH_PCIE_MPC2));
    S3BootScriptSaveMemReadWrite (S3BootScriptWidthUint32, RpBase + R_PCH_PCIE_MPC2, &Data32Or, &Data32And);
  }
}

/**
  Initialize the PCH device according to the PCH Policy HOB
  and install PCH info instance.

**/
VOID
InitializePchDevice (
  VOID
  )
{
  EFI_HANDLE              Handle;
  PCH_INFO_PROTOCOL  *PchInfo;
  EFI_STATUS  Status;
  UINT32      PchPwrmBase;

  Handle = NULL;

  DEBUG ((DEBUG_INFO, "InitializePchDevice() Start\n"));

  PchPwrmBaseGet (&PchPwrmBase);

  //
  // Create the PCH Info protocol instance
  //
  PchInfo = AllocateZeroPool (sizeof (PCH_INFO_PROTOCOL));
  if (PchInfo == NULL) {
    ASSERT (FALSE);
    return;
  }

  PchInfo->Revision = PCH_INFO_PROTOCOL_REVISION_1;

  UpdatePcieConfigInfo (PchInfo);

  //
  // Perform USB precondition on XHCI
  //
  if (mPchPolicyHob->UsbConfig.UsbPrecondition == 1) {
    XhciPrecondition ();
    PchInfo->Preconditioned   = mPrivatePreConditionList;
  }

  //
  // Configure RST PCIe Storage Remapping
  //
  Status = ConfigureRstPcieStorageRemapping (mPchPolicyHob->TempPciBusMin, mPchPolicyHob->TempPciBusMax, PCH_SATA_FIRST_CONTROLLER);
  ASSERT_EFI_ERROR (Status);

  //
  // Store PCIE Gen3 software equalization settings in bootscripts to save time on S3 resume
  //
  SavePcieGen3Eq();

  //
  // Configure root port function number mapping
  // For UEFI bios, execute RPFN mapping after RST configuraion and before PCI enumeration.
  //
  Status = PchConfigureRpfnMapping ();
  ASSERT_EFI_ERROR (Status);


  //
  // Install PCH info protocol on new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gPchInfoProtocolGuid,
                  PchInfo,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "InitializePchDevice() End\n"));
}

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

  @param[in] ImageHandle          Handle for the image of this driver
  @param[in] SystemTable          Pointer to the EFI System Table

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
PchInitEntryPointUefi (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS            Status;

  DEBUG ((DEBUG_INFO, "Uefi PchInitEntryPoint() Start\n"));

  mImageHandle = ImageHandle;

  PchInitEntryPoint();

  //
  // Initialize PCH devices and install PCH info protocol
  //
  InitializePchDevice ();


  Status = PchAcpiInit (ImageHandle);


  PchRegisterNotifications ();

  DEBUG ((DEBUG_INFO, "Uefi PchInitEntryPoint() End\n"));

  return Status;
}

/**
  PCH initialization before ExitBootServices / LegacyBoot events
  Useful for operations which must happen later than at EndOfPost event

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
PchOnBootToOsUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  ///
  /// Closed the event to avoid call twice
  ///
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }


  return;
}

/**
  PCH initialization on ExitBootService. This event is used if only ExitBootService is used
  and not in legacy boot

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

  @retval None
**/
VOID
EFIAPI
PchOnExitBootServicesUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  PchOnBootToOsUefi (NULL, NULL);

  PchUpdateNvsOnExitBootServices ();

  return;
}

/**
  PCH initialization before boot to OS

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
PchOnReadyToBootUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  DEBUG ((DEBUG_INFO, "Uefi PchOnReadyToBoot() Start\n"));

  if (Event != NULL) {
    gBS->CloseEvent(Event);
  }

  //
  // Trigger an Iotrap SMI to config PCIE power management setting after PCI enumrate is done
  //
  if (mPcieIoTrapAddress != 0) {
    IoWrite32 ((UINTN) mPcieIoTrapAddress, PciePmTrap);
  } else {
    ASSERT (FALSE);
  }

  DEBUG ((DEBUG_INFO, "Uefi PchOnReadyToBoot() End\n"));
}


/**
  PCH initialization before Boot Sript Table is closed

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
PchOnEndOfDxeUefi (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{

  DEBUG ((DEBUG_INFO, "Uefi PchOnEndOfDxe() Start\n"));

  ///
  /// Closed the event to avoid call twice when launch shell
  ///
  gBS->CloseEvent (Event);

  PchOnEndOfDxe();

  DEBUG ((DEBUG_INFO, "Uefi PchOnEndOfDxe() End\n"));

  return;
}
