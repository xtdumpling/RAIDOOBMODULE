/** @file
  This is the driver that initializes the Intel PCH.

@copyright
  Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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
#include <Library/PcdLib.h>
#include <Library/PcieAddress.h>

//APTIOV_SERVER_OVERRIDE_START: Limit PchNvsAreaAddress to be allocated below 4GB 
#include <Base.h>
//APTIOV_SERVER_OVERRIDE_END: Limit PchNvsAreaAddress to be allocated below 4GB 

//
// Module variables
//
GLOBAL_REMOVE_IF_UNREFERENCED PCH_NVS_AREA_PROTOCOL    mPchNvsAreaProtocol;

extern PCH_RST_PCIE_STORAGE_DETECTION mRstPcieStorageDetection[];


/**
  Update NVS Area after RST PCIe Storage Remapping and before Boot

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
PchUpdateNvsAreaAfterRemapping (
  )
{
  UINTN                 Index;

  for (Index = 0; Index < PCH_MAX_RST_PCIE_STORAGE_CR; Index++) {
    mPchNvsAreaProtocol.Area->RstPcieStorageInterfaceType[Index]        = mRstPcieStorageDetection[Index].DeviceInterface;
    mPchNvsAreaProtocol.Area->RstPcieStoragePmCapPtr[Index]             = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PmCapPtr;
    mPchNvsAreaProtocol.Area->RstPcieStoragePcieCapPtr[Index]           = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.PcieCapPtr;
    mPchNvsAreaProtocol.Area->RstPcieStorageL1ssCapPtr[Index]           = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.L1ssCapPtr;
    mPchNvsAreaProtocol.Area->RstPcieStorageEpL1ssControl2[Index]       = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointL1ssControl2;
    mPchNvsAreaProtocol.Area->RstPcieStorageEpL1ssControl1[Index]       = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointL1ssControl1;
    mPchNvsAreaProtocol.Area->RstPcieStorageLtrCapPtr[Index]            = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.LtrCapPtr;
    mPchNvsAreaProtocol.Area->RstPcieStorageEpLtrData[Index]            = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointLtrData;
    mPchNvsAreaProtocol.Area->RstPcieStorageEpLctlData16[Index]         = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointLctlData16;
    mPchNvsAreaProtocol.Area->RstPcieStorageEpDctlData16[Index]         = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointDctlData16;
    mPchNvsAreaProtocol.Area->RstPcieStorageEpDctl2Data16[Index]        = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.EndpointDctl2Data16;
    mPchNvsAreaProtocol.Area->RstPcieStorageRpDctl2Data16[Index]        = mRstPcieStorageDetection[Index].PchRstPcieStorageSaveRestore.RootPortDctl2Data16;
    mPchNvsAreaProtocol.Area->RstPcieStorageUniqueTableBar[Index]       = mRstPcieStorageDetection[Index].EndPointUniqueMsixTableBar;
    mPchNvsAreaProtocol.Area->RstPcieStorageUniqueTableBarValue[Index]  = mRstPcieStorageDetection[Index].EndPointUniqueMsixTableBarValue;
    mPchNvsAreaProtocol.Area->RstPcieStorageUniquePbaBar[Index]         = mRstPcieStorageDetection[Index].EndPointUniqueMsixPbaBar;
    mPchNvsAreaProtocol.Area->RstPcieStorageUniquePbaBarValue[Index]    = mRstPcieStorageDetection[Index].EndPointUniqueMsixPbaBarValue;
  }
  return EFI_SUCCESS;
}

/**
  PCH ACPI initialization before Boot Sript Table is closed
  It update ACPI table and ACPI NVS area.

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
PchAcpiOnEndOfDxe (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  
  PCH_SERIES      PchSeries;
  
  DEBUG ((DEBUG_INFO, "PchAcpiOnEndOfDxe() Start\n"));

  PchSeries = GetPchSeries ();

  ///
  /// Closed the event to avoid call twice when launch shell
  ///
  gBS->CloseEvent (Event);

  //
  // Init HDA Audio ACPI tables
  //
  PchHdAudioAcpiInit ();

  //
  // Update Pch Nvs Area
  //
  PchUpdateNvsArea ();

  //
  // Patch PchNvsArea Address
  //
  PatchPchNvsAreaAddress ();

  DEBUG ((DEBUG_INFO, "PchAcpiOnEndOfDxe() End\n"));

  return;
}
/**
  Initialize Pch acpi
  @param[in] ImageHandle          Handle for the image of this driver

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
PchAcpiInit (
  IN EFI_HANDLE         ImageHandle
  )
{
  EFI_STATUS           Status;
  EFI_EVENT            EndOfDxeEvent;
  //APTIOV_SERVER_OVERRIDE_START: Limit PchNvsAreaAddress to be allocated below 4GB 
  mPchNvsAreaProtocol.Area = (PCH_NVS_AREA *) MAX_UINT32;

  DEBUG ((DEBUG_INFO, "Install PCH NVS protocol\n"));
  Status = (gBS->AllocatePages) (AllocateMaxAddress, EfiACPIMemoryNVS, EFI_SIZE_TO_PAGES (sizeof(PCH_NVS_AREA)), (EFI_PHYSICAL_ADDRESS *)&mPchNvsAreaProtocol.Area);
  ASSERT_EFI_ERROR (Status);
  
  //APTIOV_SERVER_OVERRIDE_END: Limit PchNvsAreaAddress to be allocated below 4GB 

  ZeroMem ((VOID *) mPchNvsAreaProtocol.Area, sizeof (PCH_NVS_AREA));
  mPchNvsAreaProtocol.Area->RcRevision = PCH_RC_VERSION;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gPchNvsAreaProtocolGuid,
                  &mPchNvsAreaProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Update the NVS Area after RST PCIe Storage Remapping
  ///
  PchUpdateNvsAreaAfterRemapping ();
  //
  // Register an end of DXE event for PCH ACPI to do tasks before invoking any UEFI drivers,
  // applications, or connecting consoles,...
  //

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  PchAcpiOnEndOfDxe,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  PCH Update NvsArea ExitBootServicesFlag on ExitBootService. This event is used if only ExitBootService is used
  and not in legacy boot

  @retval None
**/
VOID
EFIAPI
PchUpdateNvsOnExitBootServices (
  VOID
  )
{
  mPchNvsAreaProtocol.Area->ExitBootServicesFlag = 1;

  return;
}

/**
  Update ASL object before Boot

  @retval EFI_STATUS
  @retval EFI_NOT_READY         The Acpi protocols are not ready.
**/
EFI_STATUS
PchUpdateNvsArea (
  VOID
  )
{
  EFI_STATUS                            Status;
  PCH_SERIES                            PchSeries;
  UINTN                                 Index;
  UINT32                                HpetBaseAdress;
  GPIO_GROUP                            GroupToGpeDw0;
  GPIO_GROUP                            GroupToGpeDw1;
  GPIO_GROUP                            GroupToGpeDw2;
  GPIO_GROUP                            Group;
  UINT32                                PadNumber;
  GPIO_PAD                              GpioPad;
  GPIO_PAD_OWN                          PadOwnVal;
  GPIO_CONFIG                           GpioData;
  UINTN                                 RpDev;
  UINTN                                 RpFun;
  UINT32                                Data32;
  USRA_ADDRESS                          Address;
  UINT32                                Seg;
  UINT32                                Bdfo;

  Status = EFI_SUCCESS;
  PchSeries = GetPchSeries ();


  DEBUG ((DEBUG_INFO, "PchUpdateNvsArea\n"));
  mPchNvsAreaProtocol.Area->PciDelayOptimizationEcr = mPchPolicyHob->PcieConfig.PciDelayOptimizationEcr;

  //
  // PCIe LTR Configuration - Latency Tolerance Reporting
  //
  for (Index = 0; Index < GetPchMaxPciePortNum (); Index++) {
    mPchNvsAreaProtocol.Area->LtrEnable[Index]     = mPchPolicyHob->PcieConfig.RootPort[Index].LtrEnable;
  }

  
  //
  // Update ASL PcieMmCfgBaseAddress value
  //
  Seg  = 0;
  Bdfo = 0;
  USRA_PCIE_SEG_BDFO_ADDRESS(Address, UsraWidth32, Seg, Bdfo);
  mPchNvsAreaProtocol.Area->MmCfgAddress = (UINT32)GetPcieSegMmcfgBaseAddress(&Address);

  //
  // Update ASL PCIE port address according to root port device and function
  //
  for (Index = 0; Index < GetPchMaxPciePortNum (); Index++) {
    Status = GetPchPcieRpDevFun (Index, &RpDev, &RpFun);
    ASSERT_EFI_ERROR (Status);

    Data32 = ((UINT8) RpDev << 16) | (UINT8) RpFun;
    mPchNvsAreaProtocol.Area->RpAddress[Index] = Data32;

    //
    // Update Maximum Snoop Latency and Maximum No-Snoop Latency values for PCIE
    //
    mPchNvsAreaProtocol.Area->PcieLtrMaxSnoopLatency[Index]   = mPchPolicyHob->PcieConfig.RootPort[Index].LtrMaxSnoopLatency;
    mPchNvsAreaProtocol.Area->PcieLtrMaxNoSnoopLatency[Index] = mPchPolicyHob->PcieConfig.RootPort[Index].LtrMaxNoSnoopLatency;
  }

  //
  // Update PCHS.
  //
  mPchNvsAreaProtocol.Area->PchSeries     = (UINT16) PchSeries;
  //
  // Update PCHG.
  //
  mPchNvsAreaProtocol.Area->PchGeneration = (UINT16) GetPchGeneration ();
  //
  // Update HPET base address.
  //
  PchHpetBaseGet (&HpetBaseAdress);
  mPchNvsAreaProtocol.Area->HPTE          = (UINT8) mPchPolicyHob->HpetConfig.Enable;
  mPchNvsAreaProtocol.Area->HPTB          = HpetBaseAdress;
  //
  // Update SBREG_BAR.
  //
  mPchNvsAreaProtocol.Area->SBRG          = PCH_PCR_BASE_ADDRESS;

  //
  // Update GPP_X to GPE_DWX mapping.
  //
  GpioGetGroupToGpeDwX (&GroupToGpeDw0, &GroupToGpeDw1, &GroupToGpeDw2);

  //
  // GPEM is an object for informing how GPIO groups are mapped to GPE.
  // Mapping for GPP_x is evaluated from (GPEM >> (GroupNumber*2)) & 0x3
  // Here GroupNumber does not match xxx_GPIO_GROUP type and is always
  // 0 based (GPP_A = 0, both for LP and H)
  // Possible values for each group:
  // 00b - 2-tier
  // 01b - 1-tier, GPE_DW0
  // 10b - 1-tier, GPE_DW1
  // 11b - 1-tier, GPE_DW2
  //
  mPchNvsAreaProtocol.Area->GPEM = (0x1 << (GpioGetGroupIndexFromGroup (GroupToGpeDw0) * 2)) |
                                   (0x2 << (GpioGetGroupIndexFromGroup (GroupToGpeDw1) * 2)) |
                                   (0x3 << (GpioGetGroupIndexFromGroup (GroupToGpeDw2) * 2));

  //
  // GP2T[GroupIndex] is an object for storing information about GPIO pads which are
  // enabled for 2-tier GPE event and their interrupt is configured for level
  // 0b - Gpio Pad which is not 2-tier event or hasn't got level interrupt
  // 1b - Gpio Pad enabled for 2-tier event and having level interrupt
  //
  for (Group = GpioGetLowestGroup (); Group <= GpioGetHighestGroup (); Group++) {

    mPchNvsAreaProtocol.Area->GP2T[GpioGetGroupIndexFromGroup (Group)] = 0x0;

    if ((Group == GroupToGpeDw0) ||
        (Group == GroupToGpeDw1) ||
        (Group == GroupToGpeDw2)) {
      //
      // not 2-tier GPE
      //
      continue;
    }

    for (PadNumber = 0; PadNumber < GpioGetPadPerGroup (Group); PadNumber++) {

      GpioPad = GpioGetGpioPadFromGroupAndPadNumber (Group, PadNumber);

      GpioGetPadOwnership (GpioPad, &PadOwnVal);
      if (PadOwnVal != GpioPadOwnHost) {
        continue;
      }

      GpioGetPadConfig (GpioPad, &GpioData);

      if (((GpioData.InterruptConfig & GPIO_INT_CONFIG_INT_SOURCE_MASK) == GpioIntSci) &&
          ((GpioData.InterruptConfig & GPIO_INT_CONFIG_INT_TYPE_MASK) == GpioIntLevel)) {
        //
        // This pad is enabled for GPE and has level interrupt
        //
        mPchNvsAreaProtocol.Area->GP2T[GpioGetGroupIndexFromGroup (Group)] |= 1 << PadNumber;
      }
    }
  }

  //
  // Thermal device in ACPI mode
  //
  mPchNvsAreaProtocol.Area->ThermalDeviceAcpiEnabled = mPchPolicyHob->ThermalConfig.ThermalDeviceEnable == 2 ? 1 : 0;
  //
  // Get Thermal Device interrupt line number
  //
  for (Index = 0; Index < mPchPolicyHob->PchInterruptConfig.NumOfDevIntConfig; Index++) {
    if ((mPchPolicyHob->PchInterruptConfig.DevIntConfig[Index].Device == PCI_DEVICE_NUMBER_PCH_THERMAL) &&
        (mPchPolicyHob->PchInterruptConfig.DevIntConfig[Index].Function == PCI_FUNCTION_NUMBER_PCH_THERMAL)) {
      mPchNvsAreaProtocol.Area->ThermalDeviceInterruptLine = mPchPolicyHob->PchInterruptConfig.DevIntConfig[Index].Irq;
    }
  }
  

  return Status;
}

/**
  Initialize PCH Nvs Area opeartion region.

  @retval EFI_SUCCESS    initialized successfully
  @retval EFI_NOT_FOUND  Nvs Area operation region is not found
**/
EFI_STATUS
PatchPchNvsAreaAddress (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT32                                Address;
  UINT16                                Length;
  
    Status = InitializeAslUpdateLib ();

  ASSERT_EFI_ERROR (Status);

  Address = (UINT32) (UINTN) mPchNvsAreaProtocol.Area;
  Length  = (UINT16) sizeof (PCH_NVS_AREA);

  DEBUG ((DEBUG_INFO, "PatchPchNvsAreaAddress: PCH NVS Address %x Length %x\n", Address, Length));
  Status  = UpdateNameAslCode (SIGNATURE_32('P','N','V','B'), &Address, sizeof (Address));
  ASSERT_EFI_ERROR (Status);
  Status  = UpdateNameAslCode (SIGNATURE_32('P','N','V','L'), &Length, sizeof (Length));
  ASSERT_EFI_ERROR (Status);


  return EFI_SUCCESS;
}

