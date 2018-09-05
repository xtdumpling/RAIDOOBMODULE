/** @file
  PCH Init Smm module for PCH specific SMI handlers.

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
**/
#include "PchInitSmm.h"
#include <Library/PchPcrLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START
// To resolve Power Button Assert issue in LR.
#include <Token.h>
// SPiEISS MUST set after BG service is ready
#if defined BiosGuard_SUPPORT && BiosGuard_SUPPORT == 1
#include <Protocol/BiosGuardProtocol.h>
#include <Cpu/CpuRegs.h>
#endif
//APTIOV_SERVER_OVERRIDE_RC_END

GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL        *mPchIoTrap;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_SMM_SX_DISPATCH2_PROTOCOL             *mSxDispatch;

GLOBAL_REMOVE_IF_UNREFERENCED PCH_NVS_AREA                              *mPchNvsArea;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16                                    mAcpiBaseAddr;

GLOBAL_REMOVE_IF_UNREFERENCED PCH_POLICY_HOB                            *mPchPolicyHob;

//
// The reserved MMIO range to be used in Sx handler
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS                      mResvMmioBaseAddr;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN                                     mResvMmioSize;
//
// The reserved MMIO range to be used by xHCI ASL code
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS                      mXhciMmioBaseAddr;
// APTIOV_SERVER_OVERRIDE_RC_START : Allocate resources to PCH devices from IIO UDS CPU0 MMIOL range
EFI_IIO_UDS_PROTOCOL  	  *gIioUds;
// APTIOV_SERVER_OVERRIDE_RC_END : Allocate resources to PCH devices from IIO UDS CPU0 MMIOL range

#define IO_TRAP_HANDLER_NUM 4
/**
  SMM ready to lock callback function of IoTrap feature.
  It adds boot scripts to save and restore IoTrap PCR registers for S3 resume.

  @retval EFI_SUCCESS   SmmReadyToLockCallback runs successfully
**/
EFI_STATUS
EFIAPI
IoTrapSmmReadyToLockCallback (
  VOID
  )
{
  UINT8                 TrapHandlerNum;
  UINT32                IoTrapRegLowDword;
  UINT32                IoTrapRegHighDword;

  //
  // Loop through the first IO Trap handler
  //
  for (TrapHandlerNum = 0; TrapHandlerNum < IO_TRAP_HANDLER_NUM; TrapHandlerNum++) {
    //
    // Get IoTrapRegLowDword from Io Trap handler register
    //
    PchPcrRead32 (PID_PSTH, R_PCH_PCR_PSTH_TRPREG0 + TrapHandlerNum * 8, &IoTrapRegLowDword);
    //
    // Check if the IO Trap handler is not used
    //
    if ((IoTrapRegLowDword & B_PCH_PCR_PSTH_TRPREG_AD) == 0) {
      continue;
    }
    PCH_PCR_BOOT_SCRIPT_WRITE (
      S3BootScriptWidthUint32,
      PID_PSTH, R_PCH_PCR_PSTH_TRPREG0 + TrapHandlerNum * 8,
      1,
      &IoTrapRegLowDword
      );
    PCH_PCR_BOOT_SCRIPT_WRITE (
      S3BootScriptWidthUint32,
      PID_DMI, R_PCH_PCR_DMI_IOT1 + TrapHandlerNum * 8,
      1,
      &IoTrapRegLowDword
      );
    //
    // Get IoTrapRegHighDword from Io Trap handler register
    //
    PchPcrRead32 (PID_PSTH, R_PCH_PCR_PSTH_TRPREG0 + TrapHandlerNum * 8 + 4, &IoTrapRegHighDword);
    PCH_PCR_BOOT_SCRIPT_WRITE (
      S3BootScriptWidthUint32,
      PID_PSTH, R_PCH_PCR_PSTH_TRPREG0 + TrapHandlerNum * 8 + 4,
      1,
      &IoTrapRegHighDword
      );
    PCH_PCR_BOOT_SCRIPT_WRITE (
      S3BootScriptWidthUint32,
      PID_DMI, R_PCH_PCR_DMI_IOT1 + TrapHandlerNum * 8 + 4,
      1,
      &IoTrapRegHighDword
      );
  }
  return EFI_SUCCESS;
}

/**
SMBUS Sx entry SMI handler.
**/
VOID
SmbusSxCallback(
VOID
)
{
	UINTN                       SmbusRegBase;
	UINT16                      SmbusIoBase;

	SmbusRegBase = MmPciBase(
		DEFAULT_PCI_BUS_NUMBER_PCH,
		PCI_DEVICE_NUMBER_PCH_SMBUS,
		PCI_FUNCTION_NUMBER_PCH_SMBUS
		);

	if (MmioRead32(SmbusRegBase) == 0xFFFFFFFF) {
		return;
	}

	SmbusIoBase = MmioRead16(SmbusRegBase + R_PCH_SMBUS_BASE) & B_PCH_SMBUS_BASE_BAR;
	if (SmbusIoBase == 0) {
		return;
	}

	MmioOr8(SmbusRegBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_IO_SPACE);
	//
	// Clear SMBUS status and SMB_WAK_STS of GPE0
	//
	IoWrite8(SmbusIoBase + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_SMBALERT_STS);
	IoWrite32(mAcpiBaseAddr + R_PCH_ACPI_GPE0_STS_127_96, B_PCH_ACPI_GPE0_STS_127_96_SMB_WAK);
}

/**
  PCH Sx entry SMI handler.

  @param[in]     Handle          Handle of the callback
  @param[in]     Context         The dispatch context
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
PchSxHandler (
  IN  EFI_HANDLE                   Handle,
  IN CONST VOID                    *Context OPTIONAL,
  IN OUT VOID                      *CommBuffer OPTIONAL,
  IN OUT UINTN                     *CommBufferSize OPTIONAL
  )
{
  PchGpioSxIsolationCallback ();
// APTIOV_SERVER_OVERRIDE_RC_START : To resolve Power Button Assert issue in LR.
#if (PLATFORM_TYPE == 0)
  PchLanSxCallback ();
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : To resolve Power Button Assert issue in LR.
  IoTrapSmmReadyToLockCallback();
  
  SmbusSxCallback ();

  return EFI_SUCCESS;
}

/**
  Initialize PCH Sx entry SMI handler.

  @param[in] ImageHandle - Handle for the image of this driver
**/
VOID
InitializeSxHandler (
  IN EFI_HANDLE        ImageHandle
  )
{
  EFI_SMM_SX_REGISTER_CONTEXT               SxDispatchContext;
  EFI_HANDLE                                SxDispatchHandle;
  EFI_SLEEP_TYPE                            SxType;
  EFI_STATUS                                Status;

  DEBUG ((DEBUG_INFO, "InitializeSxHandler() Start\n"));

  //
  // Register the callback for S3/S4/S5 entry
  //
  SxDispatchContext.Phase = SxEntry;
  for (SxType = SxS3; SxType <= SxS5; SxType++) {
    SxDispatchContext.Type  = SxType;
    Status = mSxDispatch->Register (
                            mSxDispatch,
                            PchSxHandler,
                            &SxDispatchContext,
                            &SxDispatchHandle
                            );
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((DEBUG_INFO, "InitializeSxHandler() End\n"));
}

/**
  Allocates reserved MMIO for Sx SMI handler use.

  This function is only called from entry point therefore DXE/boot Services can be used here.
  Updates ACPI NVS location to reserve allocated MMIO range as system resource.
**/
VOID 
AllocateReservedMmio (
  IN EFI_HANDLE        ImageHandle
  )
{
  PCH_NVS_AREA_PROTOCOL  *PchNvsAreaProtocol;
  EFI_STATUS             Status;

  mResvMmioSize = 1 << N_PCH_LAN_MBARA_ALIGN;

  // APTIOV_SERVER_OVERRIDE_RC_START : Allocate resources to PCH devices from IIO UDS CPU0 MMIOL range
  #if 0
  if ( (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchBottomUp) || (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchTopDown) ){
    mResvMmioBaseAddr = 0xFFFFFFFF;
  }
  #else 
    mResvMmioBaseAddr = gIioUds->IioUdsPtr->PlatformData.IIO_resource[0].StackRes[0].PciResourceMem32Limit;
  #endif
  Status = gDS->AllocateMemorySpace (
                  //PcdGet8(PcdEfiGcdAllocateType),
		  	  	  EfiGcdAllocateMaxAddressSearchTopDown,
   //APTIOV_SERVER_OVERRIDE_RC_END : Allocate resources to PCH devices from IIO UDS CPU0 MMIOL range
                  EfiGcdMemoryTypeMemoryMappedIo,
                  N_PCH_LAN_MBARA_ALIGN,
                  mResvMmioSize,
                  &mResvMmioBaseAddr,
                  ImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((DEBUG_INFO, "mResvMmioBaseAddr %x\n", mResvMmioBaseAddr));

  //
  // Locate the PCH shared data area and update reserved memory base address
  //
  Status = gBS->LocateProtocol (&gPchNvsAreaProtocolGuid, NULL, (VOID **) &PchNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);

  PchNvsAreaProtocol->Area->SxMemSize = (UINT32) mResvMmioSize;
  PchNvsAreaProtocol->Area->SxMemBase = (UINT32) mResvMmioBaseAddr;

  //
  // Install and initialize all the needed protocols
  //
  //APTIOV_SERVER_OVERRIDE_RC_START : Allocate resources to PCH devices from IIO UDS CPU0 MMIOL range
  #if 0
  if ( (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchBottomUp) || (PcdGet8(PcdEfiGcdAllocateType) == EfiGcdAllocateMaxAddressSearchTopDown) ){
    mXhciMmioBaseAddr = 0xFFFFFFFF;
  }
  #else 
    mXhciMmioBaseAddr = gIioUds->IioUdsPtr->PlatformData.IIO_resource[0].StackRes[0].PciResourceMem32Limit;
  #endif
  Status = gDS->AllocateMemorySpace (
                  //PcdGet8(PcdEfiGcdAllocateType),
		  	  	  EfiGcdAllocateMaxAddressSearchTopDown,
  // APTIOV_SERVER_OVERRIDE_RC_END : Allocate resources to PCH devices from IIO UDS CPU0 MMIOL range
                  EfiGcdMemoryTypeMemoryMappedIo,
                  N_PCH_XHCI_MEM_ALIGN,
                  V_PCH_XHCI_MEM_LENGTH,
                  &mXhciMmioBaseAddr,
                  ImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((DEBUG_INFO, "mXhciMmioBaseAddr %x\n", mXhciMmioBaseAddr));

  //
  // Update XWMB, XHCI memory base address
  //
  PchNvsAreaProtocol->Area->XhciRsvdMemBase = (UINT32) mXhciMmioBaseAddr;
}

//APTIOV_SERVER_OVERRIDE_RC_START : SPiEISS MUST set after BG service is ready
#if defined BiosGuard_SUPPORT && BiosGuard_SUPPORT == 1
EFI_STATUS 
PchBiosWriteProtectCallback (
  IN      CONST EFI_GUID   *Protocol,
  IN      VOID             *Interface,
  IN      EFI_HANDLE        Handle
  )
{
  EFI_SYSTEM_TABLE      *SystemTable = NULL;
  EFI_STATUS            Status;
  
  Status = InstallPchBiosWriteProtect (Handle, SystemTable);
  ASSERT_EFI_ERROR (Status);
  
  return EFI_SUCCESS;
}
#endif
//APTIOV_SERVER_OVERRIDE_RC_END

/**
  Initializes the PCH SMM handler for for PCIE hot plug support
  <b>PchInit SMM Module Entry Point</b>\n
  - <b>Introduction</b>\n
      The PchInitSmm module is a SMM driver that initializes the Intel Platform Controller Hub
      SMM requirements and services. It consumes the PCH_POLICY_HOB and SI_POLICY_HOB for expected
      configurations per policy.

  - <b>Details</b>\n
    This module provides SMI handlers to services PCIE HotPlug SMI, LinkActive SMI, and LinkEq SMI.
    And also provides port 0x61 emulation support, registers BIOS WP handler to process BIOSWP status,
    and registers SPI Async SMI handler to handler SPI Async SMI.
    This module also registers Sx SMI callback function to detail with GPIO Sx Isolation and LAN requirement.

  - @pre
    - PCH PCR base address configured
    - EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL
      - This is to ensure that PCI MMIO and IO resource has been prepared and available for this driver to allocate.
    - EFI_SMM_BASE2_PROTOCOL
    - EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL
    - EFI_SMM_SX_DISPATCH2_PROTOCOL
    - EFI_SMM_CPU_PROTOCOL
    - @link _PCH_SMM_IO_TRAP_CONTROL_PROTOCOL PCH_SMM_IO_TRAP_CONTROL_PROTOCOL @endlink
    - @link _PCH_SMI_DISPATCH_PROTOCOL PCH_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_PCIE_SMI_DISPATCH_PROTOCOL PCH_PCIE_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_TCO_SMI_DISPATCH_PROTOCOL PCH_TCO_SMI_DISPATCH_PROTOCOL @endlink
    - @link _PCH_ESPI_SMI_DISPATCH_PROTOCOL PCH_ESPI_SMI_DISPATCH_PROTOCOL @endlink

  - <b>References</b>\n
    - @link _PCH_POLICY PCH_POLICY_HOB @endlink.
    - @link _SI_POLICY_STRUCT SI_POLICY_HOB @endlink.

  - <b>Integration Checklists</b>\n
    - Verify prerequisites are met. Porting Recommendations.
    - No modification of this module should be necessary
    - Any modification of this module should follow the PCH BIOS Specification and EDS

  @param[in] ImageHandle - Handle for the image of this driver
  @param[in] SystemTable - Pointer to the EFI System Table

  @retval EFI_SUCCESS    - PCH SMM handler was installed
**/
EFI_STATUS
EFIAPI
PchInitSmmEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  PCH_NVS_AREA_PROTOCOL                     *PchNvsAreaProtocol;
  EFI_PEI_HOB_POINTERS                      HobPtr;

  DEBUG ((DEBUG_INFO, "PchInitSmmEntryPoint()\n"));

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmIoTrapDispatch2ProtocolGuid,
                    NULL,
                    (VOID **) &mPchIoTrap
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSxDispatch2ProtocolGuid, 
                    NULL, 
                    (VOID**) &mSxDispatch
                    );
  ASSERT_EFI_ERROR (Status);

// APTIOV_SERVER_OVERRIDE_RC_START : Allocate resources to PCH devices from IIO UDS CPU0 MMIOL range
  Status = gBS->LocateProtocol(
				&gEfiIioUdsProtocolGuid,
				NULL,
				&gIioUds);
  ASSERT_EFI_ERROR (Status);
  if ( EFI_ERROR(Status) ){
    return Status;
  }
// APTIOV_SERVER_OVERRIDE_RC_END : Allocate resources to PCH devices from IIO UDS CPU0 MMIOL range

  Status = gBS->LocateProtocol (&gPchNvsAreaProtocolGuid, NULL, (VOID **) &PchNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mPchNvsArea = PchNvsAreaProtocol->Area;

  HobPtr.Guid   = GetFirstGuidHob (&gPchPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  if (HobPtr.Guid == NULL) {
      return EFI_NOT_FOUND;
  }
  mPchPolicyHob = GET_GUID_HOB_DATA(HobPtr.Guid); 
  

  PchAcpiBaseGet (&mAcpiBaseAddr);

  AllocateReservedMmio (ImageHandle);

  InitializeSxHandler (ImageHandle);
  
  Status = InitializePchPcieSmm (ImageHandle, SystemTable);
  ASSERT_EFI_ERROR (Status);
  
  Status = InstallIoTrapPort61h (ImageHandle, SystemTable);
  ASSERT_EFI_ERROR (Status);

//APTIOV_SERVER_OVERRIDE_RC_START : SPiEISS MUST set after BG service is ready
#if defined BiosGuard_SUPPORT && BiosGuard_SUPPORT == 1
  if ( !((AsmReadMsr64 (MSR_PLATFORM_INFO) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) &&
           (AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL) & B_MSR_PLAT_FRMW_PROT_CTRL_EN))) {
    DEBUG((EFI_D_INFO, "Bios Guard is disabled\n"));
    Status = InstallPchBiosWriteProtect (ImageHandle, SystemTable);
    ASSERT_EFI_ERROR (Status);
  }
  else {
    BIOSGUARD_PROTOCOL *BiosGuardInstance = NULL;
    VOID               *Registration;
    DEBUG((EFI_D_INFO, "Bios Guard is enabled\n"));
    Status = gSmst->SmmLocateProtocol (&gSmmBiosGuardProtocolGuid, NULL, &BiosGuardInstance);
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "Bios Guard Service is not ready\n"));
      Status = gSmst->SmmRegisterProtocolNotify (
                       &gSmmBiosGuardProtocolGuid,
                       PchBiosWriteProtectCallback,
                       &Registration);
      ASSERT_EFI_ERROR (Status);
    } else {
      DEBUG((EFI_D_INFO, "Bios Guard Service is ready\n"));
      Status = InstallPchBiosWriteProtect (ImageHandle, SystemTable);
      ASSERT_EFI_ERROR (Status);
    }
    
    
  }
#else
//APTIOV_SERVER_OVERRIDE_RC_END
  Status = InstallPchBiosWriteProtect (ImageHandle, SystemTable);
  ASSERT_EFI_ERROR (Status);
//APTIOV_SERVER_OVERRIDE_RC_START
#endif
//APTIOV_SERVER_OVERRIDE_RC_END

  Status = InstallPchSpiAsyncSmiHandler ();
  ASSERT_EFI_ERROR (Status);


  return EFI_SUCCESS;
}
