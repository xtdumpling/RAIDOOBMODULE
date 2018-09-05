//****************************************************************************
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Fixed Power Button 4 sec shutdown
//    Reason:   
//    Auditor:  Sunny Yang
//    Date:     Jun/15/2016
//**************************************************************************//
/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2009 - 2016, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file AcpiSmmPlatform.c

  ACPISMM Driver implementation file.

**/

#include <AcpiSmmPlatform.h>
#include <Library/PchCycleDecodingLib.h>

#include <Library/PchPcrLib.h>

BIOS_ACPI_PARAM   *mAcpiParameter;
EFI_IIO_UDS_PROTOCOL            *mIioUds = NULL;


//
// Modular variables needed by this driver
//
EFI_ACPI_SMM_DEV                 mAcpiSmm;
SYSTEM_CONFIGURATION             mSystemConfiguration;
CHAR16    mEfiMemoryOverwriteControlVariable[] = MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME;
// APTIOV_SERVER_OVERRIDE_RC_START
#if 0
CHAR16    mEfiPhysicalPresenceVariable[]       = PHYSICAL_PRESENCE_VARIABLE;
EFI_GUID  mEfiPhysicalPresenceGuid             = EFI_PHYSICAL_PRESENCE_DATA_GUID;
#endif
PCH_RC_CONFIGURATION  gPchRcConfiguration; // PCH RC configuration in order to get StateAfterG3 setup option.
// APTIOV_SERVER_OVERRIDE_RC_END
EFI_GUID  mEfiMemoryOverwriteControlDataGuid   = MEMORY_ONLY_RESET_CONTROL_GUID;

STATIC UINT8  mPciCfgRegTable[] = {
  //
  // Logic to decode the table masks to arrive at the registers saved
  // Dword Registers are saved. For a given mask, the Base+offset register
  // will be saved as in the table below.
  // (example) To save register 0x24, 0x28 the mask at the Base 0x20 will be 0x06
  //     Base      0x00   0x20   0x40  0x60  0x80  0xA0  0xC0  0xE0
  // Mask  offset
  // 0x01   0x00
  // 0x02   0x04
  // 0x04   0x08
  // 0x08   0x0C
  // 0x10   0x10
  // 0x20   0x14
  // 0x40   0x18
  // 0x80   0x1C
  //

  //
  // Bus,   Dev,  Func,
  // 00-1F, 20-3F, 40-5F, 60-7F, 80-9F, A0-BF, C0-DF, E0-FF
  // Only Bus 0 device is supported now
  //

  PCI_DEVICE   (0, 0, 0),					//DMI
  PCI_REG_MASK (0x00, 0x00, 0x10, 0x00, 0x30, 0x00, 0x00, 0x00),

  PCI_DEVICE   (0, 1, 0),					//PCIE 1
  PCI_REG_MASK (0x08, 0x80, 0x00, 0x00, 0x50, 0x46, 0x00, 0x00),

  PCI_DEVICE   (0, 1, 1),					//PCIE 2
  PCI_REG_MASK (0x08, 0x80, 0x00, 0x00, 0x50, 0x46, 0x00, 0x00),

  PCI_DEVICE   (0, 2, 0),					//PCIE 3
  PCI_REG_MASK (0x08, 0x80, 0x00, 0x00, 0x50, 0x46, 0x00, 0x00),

  PCI_DEVICE   (0, 2, 1),					//PCIE 4
  PCI_REG_MASK (0x08, 0x80, 0x00, 0x00, 0x50, 0x46, 0x00, 0x00),

  PCI_DEVICE   (0, 2, 2),					//PCIE 5
  PCI_REG_MASK (0x08, 0x80, 0x00, 0x00, 0x50, 0x46, 0x00, 0x00),

  PCI_DEVICE   (0, 2, 3),					//PCIE 6
  PCI_REG_MASK (0x08, 0x80, 0x00, 0x00, 0x50, 0x46, 0x00, 0x00),

  PCI_DEVICE   (0, 3, 0),					//PCIE 7
  PCI_REG_MASK (0x08, 0x80, 0x00, 0x00, 0x50, 0x46, 0x00, 0x00),

  PCI_DEVICE   (0, 3, 1),					//PCIE 8
  PCI_REG_MASK (0x08, 0x80, 0x00, 0x00, 0x50, 0x46, 0x00, 0x00),

  PCI_DEVICE   (0, 3, 2),					//PCIE 9
  PCI_REG_MASK (0x08, 0x80, 0x00, 0x00, 0x50, 0x46, 0x00, 0x00),

  PCI_DEVICE   (0, 3, 3),					//PCIE A
  PCI_REG_MASK (0x08, 0x80, 0x00, 0x00, 0x50, 0x46, 0x00, 0x00),

  PCI_DEVICE   (0, 4, 0),					//CBDMA
  PCI_REG_MASK (0x3A, 0x80, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00),

  PCI_DEVICE   (0, 4, 1),					//CBDMA
  PCI_REG_MASK (0x3A, 0x80, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00),

  PCI_DEVICE   (0, 4, 2),					//CBDMA
  PCI_REG_MASK (0x3A, 0x80, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00),

  PCI_DEVICE   (0, 4, 3),					//CBDMA
  PCI_REG_MASK (0x3A, 0x80, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00),

  PCI_DEVICE   (0, 4, 4),					//CBDMA
  PCI_REG_MASK (0x3A, 0x80, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00),

  PCI_DEVICE   (0, 4, 5),					//CBDMA
  PCI_REG_MASK (0x3A, 0x80, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00),

  PCI_DEVICE   (0, 4, 6),					//CBDMA
  PCI_REG_MASK (0x3A, 0x80, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00),

  PCI_DEVICE   (0, 4, 7),					//CBDMA
  PCI_REG_MASK (0x3A, 0x80, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00),

  PCI_DEVICE   (0, 5, 0),					//VT
  PCI_REG_MASK (0x08, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00),

  PCI_DEVICE   (0, 5, 1),					//Mem Hot
  PCI_REG_MASK (0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),

  PCI_DEVICE   (0, 5, 4),					//IOAPIC
  PCI_REG_MASK (0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),

  PCI_DEVICE   (0, 17, 4),
  PCI_REG_MASK (0xF2, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),

  PCI_DEVICE   (0, 17, 0),
  PCI_REG_MASK (0xF2, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),

  PCI_DEVICE   (0, 17, 5),
  PCI_REG_MASK (0xF2, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),

  // Save Memory base register of D25F0
  PCI_DEVICE   (0, 25, 0),
  PCI_REG_MASK (0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),

  // Save PIRQ register of D31F0
  PCI_DEVICE   (0, 31, 0),
  PCI_REG_MASK (0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00),

  // Save CMD register of D31F2
  PCI_DEVICE   (0, 31, 2),
  PCI_REG_MASK (0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),

  // Save CMD, MEM register of D31F3
  PCI_DEVICE   (0, 31, 3),
  PCI_REG_MASK (0xF2, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),

  // SMBUS
  PCI_DEVICE   (0, 31, 4),
  PCI_REG_MASK (0x32, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),

  // GbE
  PCI_DEVICE   (0, 31, 6),
  PCI_REG_MASK (0x32, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00),

  PCI_DEVICE_END
};

// APTIOV_SERVER_OVERRIDE_RC_START
#if 0
EFI_STATUS
SmiPpCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                         *CommBuffer,     OPTIONAL
  IN OUT   UINTN                        *CommBufferSize  OPTIONAL
  );
#endif
// APTIOV_SERVER_OVERRIDE_RC_END
EFI_STATUS
SmiMorCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                         *CommBuffer,     OPTIONAL
  IN OUT   UINTN                        *CommBufferSize  OPTIONAL
  );


/**
  Allocate EfiACPIMemoryNVS below 4G memory address.

  This function allocates EfiACPIMemoryNVS below 4G memory address.

  @param Size   Size of memory to allocate.

  @return       Allocated address for output.

**/
VOID*
AllocateAcpiNvsMemoryBelow4G (
  IN UINTN  Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID*                 Buffer;

  Pages = EFI_SIZE_TO_PAGES (Size);
  Address = 0xffffffff;

  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   Pages,
                   &Address
                   );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Buffer = (VOID *) (UINTN) Address;
  ZeroMem (Buffer, Size);

  return Buffer;
}

EFI_STATUS
EFIAPI
ReservedS3Memory (
  UINTN  SystemMemoryLength

  )
/**

  Reserved S3 memory for InstallS3Memory

  @retval EFI_OUT_OF_RESOURCES  -  Insufficient resources to complete function.
  @retval EFI_NOT_FOUND         -  Could not locate HOB.
  @retval EFI_SUCCESS           -  Function has completed successfully.

**/
{

  VOID                                      *GuidHob;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK            *DescriptorBlock;
  VOID                                      *AcpiReservedBase;

  UINTN                                     TsegIndex;
  UINTN                                     TsegSize;
  UINTN                                     TsegBase;
  RESERVED_ACPI_S3_RANGE                    *AcpiS3Range;
  TsegIndex = 0;
  //
  // Get Hob list for SMRAM desc
  //
  GuidHob    = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  ASSERT (GuidHob);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }
  DescriptorBlock = GET_GUID_HOB_DATA(GuidHob);

  //
  // Use the hob to get SMRAM capabilities
  //
  if (DescriptorBlock->NumberOfSmmReservedRegions > 0) {
    TsegIndex = DescriptorBlock->NumberOfSmmReservedRegions - 1;
  } else {
    ASSERT(FALSE); // System halts if there is no SMM reserved region exist
  }
  ASSERT (TsegIndex <= (MAX_SMRAM_RANGES - 1));
  TsegBase  = DescriptorBlock->Descriptor[TsegIndex].PhysicalStart;
  TsegSize  = DescriptorBlock->Descriptor[TsegIndex].PhysicalSize;

  DEBUG ((EFI_D_ERROR, "SMM  Base: %08X\n", TsegBase));
  DEBUG ((EFI_D_ERROR, "SMM  Size: %08X\n", TsegSize));

  //
  // Now find the location of the data structure that is used to store the address
  // of the S3 reserved memory.
  //
  AcpiS3Range = (RESERVED_ACPI_S3_RANGE*) (UINTN) (TsegBase + RESERVED_ACPI_S3_RANGE_OFFSET);

  //
  // Allocate reserved ACPI memory for S3 resume.  Pointer to this region is
  // stored in SMRAM in the first page of TSEG.
  //
  AcpiReservedBase = AllocateAcpiNvsMemoryBelow4G (PcdGet32 (PcdS3AcpiReservedMemorySize));
  if (AcpiReservedBase != NULL) {
    AcpiS3Range->AcpiReservedMemoryBase = (UINT32)(UINTN) AcpiReservedBase;
    AcpiS3Range->AcpiReservedMemorySize = PcdGet32 (PcdS3AcpiReservedMemorySize);
  }
  AcpiS3Range->SystemMemoryLength = (UINT32)SystemMemoryLength;

  DEBUG ((EFI_D_ERROR, "S3 Memory  Base:    %08X\n", AcpiS3Range->AcpiReservedMemoryBase));
  DEBUG ((EFI_D_ERROR, "S3 Memory  Size:    %08X\n", AcpiS3Range->AcpiReservedMemorySize));
  DEBUG ((EFI_D_ERROR, "S3 SysMemoryLength: %08X\n", AcpiS3Range->SystemMemoryLength));

  return EFI_SUCCESS;
}


/**

  Initializes the SMM S3 Handler Driver.

  @param ImageHandle  -  The image handle of Wake On Lan driver.
  @param SystemTable  -  The starndard EFI system table.

  @retval EFI_OUT_OF_RESOURCES  -  Insufficient resources to complete function.
  @retval EFI_SUCCESS           -  Function has completed successfully.
  @retval Other                 -  Error occured during execution.

**/
EFI_STATUS
EFIAPI
InitAcpiSmmPlatform (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_GLOBAL_NVS_AREA_PROTOCOL    *AcpiNvsProtocol = NULL;
  UINTN                           MemoryLength;
  EFI_PEI_HOB_POINTERS            Hob;


  //
  // Init Power Management I/O Base aka ACPI Base
  //
  Status  = PchAcpiBaseGet (&(mAcpiSmm.PchPmBase));
  ASSERT_EFI_ERROR (Status);

  // Located the IOH Protocol Interface
  if (mIioUds == NULL){
  Status = gBS->LocateProtocol (
                &gEfiIioUdsProtocolGuid,
                NULL,
                &mIioUds
                );
  ASSERT_EFI_ERROR (Status);
  }

  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  &AcpiNvsProtocol
                  );
  ASSERT_EFI_ERROR (Status);


  mAcpiParameter = AcpiNvsProtocol->Area;
  ASSERT (mAcpiParameter);

  DEBUG ((DEBUG_ERROR, "TPM ACPI Parameter Block Address: 0x%X\n", mAcpiParameter));

    mAcpiSmm.BootScriptSaved  = 0;

    //
  // Calculate the system memory length by memory hobs
  //
  MemoryLength  = 0x100000;
  Hob.Raw = GetFirstHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR);
  ASSERT (Hob.Raw != NULL);
  while ((Hob.Raw != NULL) && (!END_OF_HOB_LIST (Hob))) {
    if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) {
      //
      // Skip the memory region below 1MB
      //
      if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000) {
        MemoryLength += Hob.ResourceDescriptor->ResourceLength;
      }
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
    Hob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, Hob.Raw);
  }

  ReservedS3Memory(MemoryLength);

    //
    // Locate and Register to Parent driver
    //
    Status = RegisterToDispatchDriver ();
    ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**

  Register to dispatch driver.

  @param None.

  @retval EFI_SUCCESS  -  Successfully init the device.
  @retval Other        -  Error occured whening calling Dxe lib functions.

**/
EFI_STATUS
RegisterToDispatchDriver (
  VOID
  )
{
  UINTN                         Length;
  EFI_STATUS                    Status;
  EFI_SMM_SX_DISPATCH2_PROTOCOL  *SxDispatch;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch;
  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL  *PowerButtonDispatch;
  EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT  PowerButtonContext;
  EFI_SMM_SX_REGISTER_CONTEXT   *EntryDispatchContext;
  EFI_SMM_SX_REGISTER_CONTEXT   *EntryS1DispatchContext;
  EFI_SMM_SX_REGISTER_CONTEXT   *EntryS3DispatchContext;
  EFI_SMM_SX_REGISTER_CONTEXT   *EntryS4DispatchContext;
  EFI_SMM_SX_REGISTER_CONTEXT   *EntryS5DispatchContext;
  EFI_SMM_SW_REGISTER_CONTEXT   *SwContext;
  EFI_SMM_SW_REGISTER_CONTEXT   *AcpiDisableSwContext;
  EFI_SMM_SW_REGISTER_CONTEXT   *AcpiEnableSwContext;
  EFI_SMM_SW_REGISTER_CONTEXT   *PpCallbackSwContext;
  EFI_SMM_SW_REGISTER_CONTEXT   *MorCallbackSwContext;

  //
  // Locate the ICH SMM Power Button dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmPowerButtonDispatch2ProtocolGuid, NULL, (VOID**)&PowerButtonDispatch);
  ASSERT_EFI_ERROR (Status);

  //
  // Register the power button SMM event
  //
  PowerButtonContext.Phase = EfiPowerButtonEntry;
#if SMCPKG_SUPPORT == 0
  Status = PowerButtonDispatch->Register (
                                  PowerButtonDispatch,
                                  PowerButtonCallback,
                                  &PowerButtonContext,
                                  &(mAcpiSmm.PowerButtonHandle)
                                  );
  ASSERT_EFI_ERROR (Status);
#endif
  //
  // Locate the SmmSxDispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmSxDispatch2ProtocolGuid,
                  NULL,
                  &SxDispatch
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol(
                  &gEfiSmmSwDispatch2ProtocolGuid,
                  NULL,
                  &SwDispatch
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetSpecificConfigGuid (&gEfiSetupVariableGuid, &mSystemConfiguration);
  ASSERT_EFI_ERROR (Status);

// APTIOV_SERVER_OVERRIDE_RC_START : Locate Pch RC configuration variable in order to get StateAfterG3 setup option.
  Status = GetSpecificConfigGuid (&gEfiPchRcVariableGuid, &gPchRcConfiguration);
  ASSERT_EFI_ERROR (Status);
// APTIOV_SERVER_OVERRIDE_RC_END : Locate Pch RC configuration variable in order to get StateAfterG3 setup option.

  Length = sizeof ( EFI_SMM_SX_REGISTER_CONTEXT) * 4 + sizeof ( EFI_SMM_SW_REGISTER_CONTEXT) * 2;
  Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      Length,
                      &EntryDispatchContext
                      );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetMem (EntryDispatchContext, Length, 0);

  EntryS1DispatchContext  = EntryDispatchContext++;
  EntryS3DispatchContext  = EntryDispatchContext++;
  EntryS4DispatchContext  = EntryDispatchContext++;
  EntryS5DispatchContext  = EntryDispatchContext++;

  SwContext = ( EFI_SMM_SW_REGISTER_CONTEXT *)EntryDispatchContext;
  AcpiDisableSwContext = SwContext++;
  AcpiEnableSwContext  = SwContext++;
  PpCallbackSwContext  = SwContext++;
  MorCallbackSwContext = SwContext++;

  //
  // Register the enable handler
  //
  AcpiEnableSwContext->SwSmiInputValue = EFI_ACPI_ACPI_ENABLE;
  Status = SwDispatch->Register (
                        SwDispatch,
                        EnableAcpiCallback,
                        AcpiEnableSwContext,
                        &(mAcpiSmm.DisableAcpiHandle)
                        );

  //
  // Register the disable handler
  //
  AcpiDisableSwContext->SwSmiInputValue = EFI_ACPI_ACPI_DISABLE;
  Status = SwDispatch->Register (
                        SwDispatch,
                        DisableAcpiCallback,
                        AcpiDisableSwContext,
                        &(mAcpiSmm.EnableAcpiHandle)
                        );


  //
  // Register entry phase call back function for S1
  //
  EntryS1DispatchContext->Type  = SxS1;
  EntryS1DispatchContext->Phase = SxEntry;
  Status = SxDispatch->Register (
                        SxDispatch,
                        SxSleepEntryCallBack,
                        EntryS1DispatchContext,
                        &(mAcpiSmm.S1SleepEntryHandle)
                        );

  //
  // Register entry phase call back function
  //
  EntryS3DispatchContext->Type  = SxS3;
  EntryS3DispatchContext->Phase = SxEntry;
  Status = SxDispatch->Register (
                        SxDispatch,
                        SxSleepEntryCallBack,
                        EntryS3DispatchContext,
                        &(mAcpiSmm.S3SleepEntryHandle)
                        );

  //
  // Register entry phase call back function for S4
  //
  EntryS4DispatchContext->Type  = SxS4;
  EntryS4DispatchContext->Phase = SxEntry;
  Status = SxDispatch->Register (
                        SxDispatch,
                        SxSleepEntryCallBack,
                        EntryS4DispatchContext,
                        &(mAcpiSmm.S4SleepEntryHandle)
                        );

  //
  // Register callback for S5 in order to workaround the LAN shutdown issue
  //
  EntryS5DispatchContext->Type  = SxS5;
  EntryS5DispatchContext->Phase = SxEntry;
  Status = SxDispatch->Register (
                        SxDispatch,
                        SxSleepEntryCallBack,
                        EntryS5DispatchContext,
                        &(mAcpiSmm.S5SoftOffEntryHandle)
                        );

  return Status;
}


/**

  SMI handler to retore PchS3 code & context for S3 path
  This will be only triggered when BootScript got executed during resume

  @param DispatchHandle  - EFI Handle
  @param DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

  @retval Nothing

**/
EFI_STATUS
RestorePchS3SwCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  )
{
  //
  // Restore to original address by default
  //
  RestoreLockBox(&gPchS3CodeInLockBoxGuid, NULL, NULL);
  RestoreLockBox(&gPchS3ContextInLockBoxGuid, NULL, NULL);
  return EFI_SUCCESS;
}

EFI_STATUS
DisableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                         *CommBuffer,     OPTIONAL
  IN OUT   UINTN                        *CommBufferSize  OPTIONAL
  )
/**

  SMI handler to disable ACPI mode

  Dispatched on reads from APM port with value 0xA1

  ACPI events are disabled and ACPI event status is cleared.
  SCI mode is then disabled.
   Clear all ACPI event status and disable all ACPI events
   Disable PM sources except power button
   Clear status bits
   Disable GPE0 sources
   Clear status bits
   Disable GPE1 sources
   Clear status bits
   Disable SCI

  @param DispatchHandle  - EFI Handle
  @param DispatchContext - Pointer to the  EFI_SMM_SW_REGISTER_CONTEXT

  @retval Nothing

**/
{
  UINT16      Pm1Cnt;

  ASSERT (mAcpiSmm.PchPmBase != 0);
  Pm1Cnt = IoRead16 (mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_CNT);

  //
  // Disable SCI
  //
  Pm1Cnt &= ~B_PCH_ACPI_PM1_CNT_SCI_EN;

  IoWrite16 (mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_CNT, Pm1Cnt);

  return EFI_SUCCESS;
}


EFI_STATUS
EnableAcpiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext,
  IN OUT    VOID                          *CommBuffer,     OPTIONAL
  IN OUT    UINTN                         *CommBufferSize  OPTIONAL
  )
/**

  SMI handler to enable ACPI mode

  Dispatched on reads from APM port with value 0xA0

  Disables the SW SMI Timer.
  ACPI events are disabled and ACPI event status is cleared.
  SCI mode is then enabled.

   Disable SW SMI Timer

   Clear all ACPI event status and disable all ACPI events
   Disable PM sources except power button
   Clear status bits

   Disable GPE0 sources
   Clear status bits

   Disable GPE1 sources
   Clear status bits

   Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)

   Enable SCI

  @param DispatchHandle  - EFI Handle
  @param DispatchContext - Pointer to the  EFI_SMM_SW_REGISTER_CONTEXT

  @retval Nothing

**/
{
  EFI_STATUS  Status;
  UINT32      SmiEn;
  UINT16      Pm1En;
  UINT16      Pm1Cnt;
  UINT8       Data8;

#ifdef ACPI_PC_BUILD
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    (EFI_SOFTWARE_SMM_DRIVER | EFI_SW_SMM_ACPI_ENABLE)
    );
#endif // #ifdef ACPI_PC_BUILD

  ASSERT (mAcpiSmm.PchPmBase != 0);

  SmiEn = IoRead32 (mAcpiSmm.PchPmBase + R_PCH_SMI_EN);

  //
  // Disable SW SMI Timer and legacy USB
  //
  SmiEn &= ~(B_PCH_SMI_EN_SWSMI_TMR | B_PCH_SMI_EN_LEGACY_USB | B_PCH_SMI_EN_LEGACY_USB2);

  //
  // And enable SMI on write to B_PCH_ACPI_PM1_CNT_SLP_EN when SLP_TYP is written
  //
  SmiEn |= B_PCH_SMI_EN_ON_SLP_EN;
  IoWrite32 (mAcpiSmm.PchPmBase + R_PCH_SMI_EN, SmiEn);

  //
  // Disable PM sources except power button
  //
  Pm1En   = B_PCH_ACPI_PM1_EN_PWRBTN;
  IoWrite16 (mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_EN, Pm1En);

  //
  // Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)
  //
  Data8 = RTC_ADDRESS_REGISTER_D;
  IoWrite8 (R_IOPORT_CMOS_STANDARD_INDEX, Data8);
  Data8 = 0x0;
  IoWrite8 (R_IOPORT_CMOS_STANDARD_DATA, Data8);

  //
  // Enable SCI
  //
  Pm1Cnt = IoRead16 (mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_CNT);
  Pm1Cnt |= B_PCH_ACPI_PM1_CNT_SCI_EN;
  IoWrite16 (mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_CNT, Pm1Cnt);

  //
  // Do platform specific stuff for ACPI enable SMI
  //


  return Status = EFI_SUCCESS;
}


//
// Only for 2 digit BCD.
//
UINT8
DecToBCD (
  UINT8 Dec
  )
/*++

Routine Description:

  Converts Decimal to BCD

Arguments:

  Dec - Decimal value to be converted to BCD

Returns:

  returns BCD for Dec number

--*/
{
  UINT8 FirstDigit;
  UINT8 SecondDigit;

  FirstDigit  = Dec % 10;
  SecondDigit = Dec / 10;

  return (SecondDigit << 4) + FirstDigit;
}

//
// Only for 2 digit BCD.
//
UINT8
BCDToDec (
  UINT8 BCD
  )
/*++

Routine Description:

  Converts BCD to Dec number

Arguments:

  BCD - BCD number which needs to be converted to Dec

Returns:

 Dec value for given BCD

--*/
{
  UINT8 FirstDigit;
  UINT8 SecondDigit;
  FirstDigit  = BCD & 0xf;
  SecondDigit = BCD >> 4;

  ;
  return SecondDigit * 10 + FirstDigit;
}

UINT8
ReadRtcIndex (
  IN UINT8 Index
  )
/*++

Routine Description:

  Reads the RTC Index register

Arguments:

  Index - Index register

Returns:

  Value in Index register

--*/
{
  UINT8           Value;
  UINT8           Addr;
  UINT8           Nmi;

  //Preseve NMI setting
  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~0, B_PCH_PCR_ITSS_GIC_AME);
  //
  // GIC read back is done in PchPcr lib
  //
  gSmst->SmmIo.Io.Read (
                    &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    CMOS_ADDR_PORT,
                    1,
                    &Addr
                    );
  Nmi = Addr & 0x80;
  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~B_PCH_PCR_ITSS_GIC_AME, 0);

  //
  // Check if Data Time is valid
  //
  if (Index <= 9) {
    do {
      Addr = 0x0A | Nmi;
      gSmst->SmmIo.Io.Write (
                        &gSmst->SmmIo,
                        SMM_IO_UINT8,
                        CMOS_ADDR_PORT,
                        1,
                        &Addr
                        );
      gSmst->SmmIo.Io.Read (
                        &gSmst->SmmIo,
                        SMM_IO_UINT8,
                        CMOS_DATA_PORT,
                        1,
                        &Value
                        );
    } while (Value & 0x80);
  }

  Addr = Index | Nmi;
  gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    CMOS_ADDR_PORT,
                    1,
                    &Addr
                    );
  //
  // Read register.
  //
  gSmst->SmmIo.Io.Read (
                    &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    CMOS_DATA_PORT,
                    1,
                    &Value
                    );
  if (Index <= 9) {
    Value = BCDToDec (Value);
  }

  return (UINT8) Value;
}


VOID
WriteRtcIndex (
  IN UINT8 Index,
  IN UINT8 Value
  )
/*++

Routine Description:

  Writes to an RTC Index register

Arguments:

  Index - Index to be written
  Value - Value to be written to Index register

Returns:

  VOID

--*/
{
  UINT8           Nmi;
  UINT8           Addr;

  //Preseve NMI Bit setting
  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~0, B_PCH_PCR_ITSS_GIC_AME);
  //
  // GIC read back is done in PchPcr lib
  //
  gSmst->SmmIo.Io.Read (
                    &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    CMOS_ADDR_PORT,
                    1,
                    &Addr
                    );
  Nmi = Addr & 0x80;
  PchPcrAndThenOr32 (PID_ITSS, R_PCH_PCR_ITSS_GIC, (UINT32)~B_PCH_PCR_ITSS_GIC_AME, 0);

  Addr = Index | Nmi;
  gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    CMOS_ADDR_PORT,
                    1,
                    &Addr
                    );
  if (Index <= 9) {
    Value = DecToBCD (Value);
  }
  //
  // Write Register.
  //
  gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    CMOS_DATA_PORT,
                    1,
                    &Value
                    );
}




VOID
SetWakeupTime (
  IN EFI_TIME     *Time
  )
/*++

Routine Description:

  Sets the time to RTC register

Arguments:

  Time  - Time to be set

Returns:

  VOID

--*/
{
  UINT8 Value;
  UINT8 ValueM;
  UINT8 ValueS;
  UINT8 ValueH;
  
  ValueS = ReadRtcIndex (0x0);
  ValueM = ReadRtcIndex (0x2);
  ValueH = ReadRtcIndex (0x4);
  DEBUG((EFI_D_INFO, "RTC Time:  %x:%x:%x\n", ValueH, ValueM, ValueS));
  
  WriteRtcIndex (5, Time->Hour);
  WriteRtcIndex (3, Time->Minute);
  WriteRtcIndex (1, Time->Second);
  
  ValueS = ReadRtcIndex (0x1);
  ValueM = ReadRtcIndex (0x3);
  ValueH = ReadRtcIndex (0x5);
  DEBUG((EFI_D_INFO, "RTC Alarm Time:  %x:%x:%x\n", ValueH, ValueM, ValueS));

  //
  // Set Enable
  //
  Value = ReadRtcIndex (0xB);
  Value |= 1 << 5;
  WriteRtcIndex (0xB, Value);
}



//pwu3: Remove PP and MOR for PO_WA
#if 0

EFI_STATUS
SmiMorCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                         *CommBuffer,     OPTIONAL
  IN OUT   UINTN                        *CommBufferSize  OPTIONAL
  )
/**

  When an smi event for Mor happen.

  @retval None

**/
{
  EFI_STATUS            Status;
  UINTN                 DataSize;
  UINT8                 MorControl;
  UINT32                Attributes;

  if (mAcpiParameter->TpmParamter == 1) {
    //
    // Called from ACPI _DSM method, save the MOR data to variable.
    //
    MorControl = (UINT8)mAcpiParameter->MorData;
  } else if (mAcpiParameter->TpmParamter == 2) {
    //
    // Called from ACPI _PTS method, setup ClearMemory flags if needed.
    //
    DataSize = sizeof (UINT8);
    Status = SmmGetVariable (
               mEfiMemoryOverwriteControlVariable,
               &mEfiMemoryOverwriteControlDataGuid,
			   &Attributes,
               &DataSize,
               &MorControl
               );
    if (EFI_ERROR (Status)) {
      return;
    }

    if (MOR_CLEAR_MEMORY_VALUE (MorControl) == 0x0) {
      return;
    }
    MorControl &= 0xFE;
  }

  DataSize = sizeof (UINT8);
  Status = SmmSetVariable (
             mEfiMemoryOverwriteControlVariable,
             &mEfiMemoryOverwriteControlDataGuid,
			 Attributes,
             DataSize,
             &MorControl
             );
  return EFI_SUCCESS;
}

// APTIOV_SERVER_OVERRIDE_RC_START
#if 0
EFI_STATUS
SmiPpCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                         *CommBuffer,     OPTIONAL
  IN OUT   UINTN                        *CommBufferSize  OPTIONAL
  )
/**

  When an smi event for physical presence happen.

  @retval None

**/
{
  EFI_STATUS            Status;
  UINTN                 DataSize;
  EFI_PHYSICAL_PRESENCE PpData;
  UINT32                Attributes

  //
  // Get the Physical Presence variable
  //
  DataSize = sizeof (EFI_PHYSICAL_PRESENCE);
  Status = SmmGetVariable (
             mEfiPhysicalPresenceVariable,
             &mEfiPhysicalPresenceGuid,
			 &Attributes,
             &DataSize,
             &PpData
             );
  if (EFI_ERROR (Status)) {
    return;
  }

  if (mAcpiParameter->TpmParamter == 5) {
    //
    // Return TPM Operation Response to OS Environment
    //
    mAcpiParameter->LastPPRequest = PpData.LastPPRequest;
    mAcpiParameter->PPResponse    = PpData.PPResponse;
  } else if (mAcpiParameter->TpmParamter == 2) {
    //
    // Submit TPM Operation Request to Pre-OS Environment
    //
    if (PpData.PPRequest != mAcpiParameter->PPRequest) {
      PpData.PPRequest = (UINT8)mAcpiParameter->PPRequest;
      DataSize = sizeof (EFI_PHYSICAL_PRESENCE);
      Status = SmmSetVariable (
                 mEfiPhysicalPresenceVariable,
                 &mEfiPhysicalPresenceGuid,
				 Attributes,
                 DataSize,
                 &PpData
                 );
    }
  }
  return EFI_SUCCESS;
}
#endif
// APTIOV_SERVER_OVERRIDE_RC_END
#endif

EFI_STATUS
SxSleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN CONST  EFI_SMM_SX_REGISTER_CONTEXT  *DispatchContext,
  IN OUT    VOID                         *CommBuffer,     OPTIONAL
  IN OUT    UINTN                        *CommBufferSize  OPTIONAL
  )
/**

  Callback function entry for Sx sleep state.

  @param DispatchHandle   -  The handle of this callback, obtained when registering.
  @param DispatchContext  -  The predefined context which contained sleep type and phase.

  @retval EFI_SUCCESS            -  Operation successfully performed.
  @retval EFI_INVALID_PARAMETER  -  Invalid parameter passed in.

**/
{
  EFI_STATUS  Status;
  UINT32      Data32;


  //
  // Used for RTC wake up feature
  //
  EFI_TIME  Time;
  UINT16    Data16;

  // APTIOV_SERVER_OVERRIDE_RC_START : "Leave power state unchanged" support for StateAfterG3 setup option.
  if (gPchRcConfiguration.StateAfterG3 == 0xFF) {
    UINT8                 DataUint8;
    UINTN                 PmcBaseAddress;
    //
    // Program Chipset to be off after AC power loss.
    //
    PmcBaseAddress = MmPciBase (
                         DEFAULT_PCI_BUS_NUMBER_PCH,
                         PCI_DEVICE_NUMBER_PCH_PMC,
                         PCI_FUNCTION_NUMBER_PCH_PMC
                         );
    gSmst->SmmIo.Mem.Read(&gSmst->SmmIo, SMM_IO_UINT8, PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B, 1, &DataUint8);
    DataUint8 |= B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN; // Program AFTERG3_EN bit
    gSmst->SmmIo.Mem.Write(&gSmst->SmmIo, SMM_IO_UINT8, PmcBaseAddress + R_PCH_PMC_GEN_PMCON_B, 1, &DataUint8);
  }
  // APTIOV_SERVER_OVERRIDE_RC_END : "Leave power state unchanged" support for StateAfterG3 setup option.


  //
  // Handling S1 - setting appropriate wake bits in GPE0_EN
  //
  if ((DispatchHandle == mAcpiSmm.S1SleepEntryHandle) && (((EFI_SMM_SX_REGISTER_CONTEXT *)DispatchContext)->Type == SxS1)) {
    if(mSystemConfiguration.XhciWakeOnUsbEnabled == TRUE){
      Status = gSmst->SmmIo.Io.Read( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
      Data32 |= (B_PCH_ACPI_GPE0_EN_127_96_USB_CON_DSX_EN | B_PCH_ACPI_GPE0_EN_127_96_PME_B0 | B_PCH_ACPI_GPE0_EN_127_96_PME);
      Status = gSmst->SmmIo.Io.Write( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
    }

    //
    // Set the power state in the BMC
    //
    if (mSystemConfiguration.WakeOnLanSupport == 1) {
      //
      // Enable bit11 (PME_EN), 9 (PCI_EXP) in GPE0_EN
      //
      Status = gSmst->SmmIo.Io.Read( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
      Data32 |= (B_PCH_ACPI_GPE0_EN_127_96_PME_B0 | B_PCH_ACPI_GPE0_EN_127_96_PCI_EXP | B_PCH_ACPI_GPE0_EN_127_96_PME);
      Status = gSmst->SmmIo.Io.Write( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
      return EFI_SUCCESS;
    }
  }

  //
  // Handling S4, S5 and WOL - setting appropriate wake bits in GPE0_EN
  //
  if (((DispatchHandle == mAcpiSmm.S4SleepEntryHandle) && (((EFI_SMM_SX_REGISTER_CONTEXT *)DispatchContext)->Type == SxS4)) ||
      ((DispatchHandle == mAcpiSmm.S5SoftOffEntryHandle) && (((EFI_SMM_SX_REGISTER_CONTEXT *)DispatchContext)->Type == SxS5))
     ) {
    //
    // Enable Wake on RTC feature
    //
    if (mSystemConfiguration.WakeOnRTCS4S5 == 1) {
      Time.Hour   = mSystemConfiguration.RTCWakeupTimeHour;
      Time.Minute = mSystemConfiguration.RTCWakeupTimeMinute;
      Time.Second = mSystemConfiguration.RTCWakeupTimeSecond;

      SetWakeupTime (&Time);
      //
      // Clear RTC PM1 status
      //
      gSmst->SmmIo.Io.Read (
                        &gSmst->SmmIo,
                        SMM_IO_UINT16,
                        mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_STS,
                        1,
                        &Data16
                        );
      Data16 = Data16 | B_PCH_ACPI_PM1_STS_RTC;
      gSmst->SmmIo.Io.Write (
                        &gSmst->SmmIo,
                        SMM_IO_UINT16,
                        mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_STS,
                        1,
                        &Data16
                        );
      //
      // set RTC_EN bit in PM1_EN to wake up from the alarm
      //
      gSmst->SmmIo.Io.Read (
                        &gSmst->SmmIo,
                        SMM_IO_UINT16,
                        mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_EN,
                        1,
                        &Data16
                        );
      Data16 = Data16 | B_PCH_ACPI_PM1_EN_RTC;
      gSmst->SmmIo.Io.Write (
                        &gSmst->SmmIo,
                        SMM_IO_UINT16,
                        mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_EN,
                        1,
                        &Data16
                        );
    }
    if(mSystemConfiguration.XhciWakeOnUsbEnabled == TRUE){
      Status = gSmst->SmmIo.Io.Read( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
      Data32 |= (B_PCH_ACPI_GPE0_EN_127_96_USB_CON_DSX_EN | B_PCH_ACPI_GPE0_EN_127_96_PME_B0 | B_PCH_ACPI_GPE0_EN_127_96_PME);
      Status = gSmst->SmmIo.Io.Write( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
    }

    if (mSystemConfiguration.WakeOnLanSupport == 1) {
      //
      // Enable bit11 (PME_EN)  ,9 (PCI_EXP) in GPE0_EN
      // Enable the WOL bits in GPE0_EN reg here for PME
      //
      Status = gSmst->SmmIo.Io.Read( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
      Data32 |= (B_PCH_ACPI_GPE0_EN_127_96_PME_B0 | B_PCH_ACPI_GPE0_EN_127_96_PCI_EXP | B_PCH_ACPI_GPE0_EN_127_96_PME);
      Status = gSmst->SmmIo.Io.Write( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
      return EFI_SUCCESS;
    }
  }

  //
  // Already handled S1, S4, S5
  // If not S3, must be error
  //
  if ((DispatchHandle != mAcpiSmm.S3SleepEntryHandle) || (((EFI_SMM_SX_REGISTER_CONTEXT *)DispatchContext)->Type != SxS3)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Assuming S3 from here on out
  //
  //
  // Reget PCH power mgmr regs base in case of OS changing it at runtime
  //
  ASSERT (mAcpiSmm.PchPmBase != 0);
  Status  = SaveRuntimeScriptTable (gSmst);
  if (EFI_ERROR (Status)) {
    return Status;
  }

      if(mSystemConfiguration.XhciWakeOnUsbEnabled == TRUE){
        Status = gSmst->SmmIo.Io.Read( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
        Data32 |= (B_PCH_ACPI_GPE0_EN_127_96_USB_CON_DSX_EN | B_PCH_ACPI_GPE0_EN_127_96_PME_B0 | B_PCH_ACPI_GPE0_EN_127_96_PME);
        Status = gSmst->SmmIo.Io.Write( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
      }

  if (mSystemConfiguration.WakeOnLanSupport == 1) {
    //
    // Enable bit11 (PME_EN), 9 (PCI_EXP) in GPE0_EN
    // Enable the WOL bits in GPE0_EN reg here for PME
    //
    Status = gSmst->SmmIo.Io.Read( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
    Data32 |= (B_PCH_ACPI_GPE0_EN_127_96_PME_B0 | B_PCH_ACPI_GPE0_EN_127_96_PCI_EXP | B_PCH_ACPI_GPE0_EN_127_96_PME);
    Status = gSmst->SmmIo.Io.Write( &gSmst->SmmIo, SMM_IO_UINT32, mAcpiSmm.PchPmBase + R_PCH_ACPI_GPE0_EN_127_96, 1, &Data32 );
  }


  return EFI_SUCCESS;
}



/**

    GC_TODO: add routine description

    @param Smst - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
SaveRuntimeScriptTable (
  IN EFI_SMM_SYSTEM_TABLE2       *Smst
  )
{
  USRA_ADDRESS          Address;
  UINT16                TcoBaseAddr;
  UINT32                Data32;
  UINT16                Data16;
  UINT8                 Data8;
  UINT8                 Mask;
  UINTN                 Index;
  UINTN                 Offset;


  //
  // Save PCI-Host bridge settings (0, 0, 0). 0x90, 94 and 9c are changed by CSM
  // and vital to S3 resume. That's why we put save code here
  //
  Index = 0;
  USRA_ZERO_ADDRESS_TYPE(Address, AddrTypePCIE);
  while (mPciCfgRegTable[Index] != PCI_DEVICE_END) {

    Address.Pcie.Bus              = mPciCfgRegTable[Index++];
    Address.Pcie.Dev              = mPciCfgRegTable[Index++];
    Address.Pcie.Func             = mPciCfgRegTable[Index++];
    Address.Pcie.Offset           = 0;
    Address.Pcie.Seg = 0;

    Address.Attribute.AccessWidth = UsraWidth16;
    RegisterRead(&Address, &Data16);
    if (Data16 == 0xFFFF) {
      Index += 8;
      continue;
    }

    Address.Attribute.AccessWidth = UsraWidth32;
    Address.Attribute.S3Enable = USRA_ENABLE;
    for (Offset = 0, Mask = 0x01; Offset < 256; Offset += 4, Mask <<= 1)
    {
      if (Mask == 0x00)
        Mask = 0x01;

      if (mPciCfgRegTable[Index + Offset / 32] & Mask) 
      {
        Address.Pcie.Offset = (UINT8)Offset;
        RegisterRead(&Address, &Data32);

        //
        // Save latest settings to runtime script table
        //        
        RegisterWrite(&Address, &Data32);
      }
    }

    Index += 8;
  }
  //
  // Selftest KBC
  //
  Data8 = 0xAA;
  S3BootScriptSaveIoWrite(S3BootScriptWidthUint8, 0x64, 1, &Data8);

  //
  // Important to trap Sx for SMM
  //
  Data32 = IoRead32 (mAcpiSmm.PchPmBase + R_PCH_SMI_EN);
  S3BootScriptSaveIoWrite(S3BootScriptWidthUint32, (mAcpiSmm.PchPmBase + R_PCH_SMI_EN), 1, &Data32);

  //
  // Save B_TCO_LOCK so it will be done on S3 resume path.
  //

  PchTcoBaseGet (&TcoBaseAddr);
  Data16 = IoRead16 (mAcpiSmm.PchPmBase + TcoBaseAddr + R_PCH_TCO1_CNT);

  S3BootScriptSaveIoWrite(S3BootScriptWidthUint16, (mAcpiSmm.PchPmBase + TcoBaseAddr + R_PCH_TCO1_CNT), 1, &Data16);
  return EFI_SUCCESS;
}

/**

  Updates the mBmcAcpiSwChild variable upon the availability of
  the BmcAcpiSwChild Protocol.

  @param (Standard EFI notify event - EFI_EVENT_NOTIFY)

  @retval None

**/
EFI_STATUS
EFIAPI
BmcAcpiSwChildCallbackFunction (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
    return EFI_SUCCESS;
}

/**

  Callback function set BMC ACPI Power State.

  @param DispatchHandle   -  The handle of this callback, obtained when registering.
  @param DispatchContext  -  The predefined context which contained sleep type and phase.

  @retval EFI_SUCCESS            -  Operation successfully performed.
  @retval EFI_INVALID_PARAMETER  -  Invalid parameter passed in.

**/
EFI_STATUS
SetBmcAcpiPowerStateCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer,
  IN  OUT UINTN                     *CommBufferSize
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;


  return Status;
}

/**
  When a power button event happens, it shuts off the machine

  @param[in] DispatchHandle    - Handle of this dispatch function
  @param[in] DispatchContext   - Pointer to the dispatch function's context
  @param[in] CommBuffer        - A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in] CommBufferSize    - The size of the CommBuffer.

  @retval EFI_SUCCESS          - Operation successfully performed
**/
EFI_STATUS
PowerButtonCallback (
  IN  EFI_HANDLE                 DispatchHandle,
  IN  CONST VOID                 *DispatchContext,
  IN  OUT VOID                   *CommBuffer  OPTIONAL,
  IN  UINTN                      *CommBufferSize  OPTIONAL
  )
{
  UINT32  Data32;
  UINT16  Data16;

  DEBUG ((DEBUG_ERROR, "PowerButtonCallback Entry\n"));

  ASSERT (mAcpiSmm.PchPmBase != 0);

  //
  // Clear Power Button Status
  //
  Data16 = B_PCH_ACPI_PM1_STS_PWRBTN;
  gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo,
                    SMM_IO_UINT16,
                    mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_STS,
                    1,
                    &Data16
                    );

  //
  // Shut it off now - set the sleep type == S5
  //
  gSmst->SmmIo.Io.Read (
                    &gSmst->SmmIo,
                    SMM_IO_UINT32,
                    mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_CNT,
                    1,
                    &Data32
                    );
  Data32 &= ~(B_PCH_ACPI_PM1_CNT_SLP_TYP);
  Data32 |= V_PCH_ACPI_PM1_CNT_S5;

  gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo,
                    SMM_IO_UINT32,
                    mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_CNT,
                    1,
                    &Data32
                    );

  //
  // Shut it off now - actually trigger it
  //
  Data32 |= B_PCH_ACPI_PM1_CNT_SLP_EN;
  gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo,
                    SMM_IO_UINT32,
                    mAcpiSmm.PchPmBase + R_PCH_ACPI_PM1_CNT,
                    1,
                    &Data32
                    );

  return EFI_SUCCESS;
}

