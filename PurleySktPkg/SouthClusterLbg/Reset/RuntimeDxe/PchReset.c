/** @file
  PCH RESET Runtime Driver

@copyright
 Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
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
#include "PchReset.h"

GLOBAL_REMOVE_IF_UNREFERENCED PCH_RESET_INSTANCE  *mPchResetInstance;
STATIC UINT8 mDaysOfMonthInfo[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

GLOBAL_REMOVE_IF_UNREFERENCED PCH_RESET_DATA mPchPowerCycleReset = {
  PCH_PLATFORM_SPECIFIC_RESET_STRING,
  PCH_POWER_CYCLE_RESET_GUID
};
GLOBAL_REMOVE_IF_UNREFERENCED PCH_RESET_DATA mPchGlobalReset = {
  PCH_PLATFORM_SPECIFIC_RESET_STRING,
  PCH_GLOBAL_RESET_GUID
};
GLOBAL_REMOVE_IF_UNREFERENCED PCH_RESET_DATA mPchGlobalResetWithEc = {
  PCH_PLATFORM_SPECIFIC_RESET_STRING,
  PCH_GLOBAL_RESET_WITH_EC_GUID
};

GLOBAL_REMOVE_IF_UNREFERENCED UINT32 mCapsuleResetType = 0;

/**
  Check if it is leap year

  @param[in] Year            year to be check

  @retval True               year is leap year
  @retval FALSE              year is not a leap year
**/
BOOLEAN
IsLeapYear (
  IN UINT16 Year
  )
{
  return (Year % 4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0));
}

/**
  Set System Wakeup Alarm.

  @param[in] WakeAfter       Time offset in seconds to wake from S3

  @retval EFI_SUCCESS        Timer started successfully
**/
STATIC
EFI_STATUS
SetSystemWakeupAlarm (
  IN       UINT32          WakeAfter
  )
{
  EFI_STATUS              Status;
  EFI_TIME                Time;
  EFI_TIME_CAPABILITIES   Capabilities;
  UINT32                  Reminder;
  UINT16                  ABase;
  UINT8                   DayOfMonth;

  ///
  /// For an instant wake 2 seconds is a safe value
  ///
  if (WakeAfter < 2) {
    WakeAfter = 2;
  }

  Status = EfiGetTime (&Time, &Capabilities);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Reminder = WakeAfter + (UINT32) Time.Second;
  Time.Second = Reminder % 60;
  Reminder = Reminder / 60;
  Reminder = Reminder + (UINT32) Time.Minute;
  Time.Minute = Reminder % 60;
  Reminder = Reminder / 60;
  Reminder = Reminder + (UINT32) Time.Hour;
  Time.Hour = Reminder % 24;
  Reminder = Reminder / 24;

  if (Reminder > 0) {
    Reminder = Reminder + (UINT32) Time.Day;
    if ((Time.Month == 2) && IsLeapYear (Time.Year)) {
      DayOfMonth = 29;
    } else {
      DayOfMonth = mDaysOfMonthInfo[Time.Month - 1];
    }
    if (Reminder > DayOfMonth) {
      Time.Day = (UINT8)Reminder - DayOfMonth;
      Reminder = 1;
    } else {
      Time.Day = (UINT8)Reminder;
      Reminder = 0;
    }
  }
  if (Reminder > 0) {
    if (Time.Month == 12) {
      Time.Month = 1;
      Time.Year = Time.Year + 1;
    } else {
      Time.Month = Time.Month + 1;
    }
  }

  Status = EfiSetWakeupTime (TRUE, &Time);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ABase = mPchResetInstance->PchAcpiBase;


  ///
  /// Clear RTC PM1 status
  ///
  IoWrite16 (ABase + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_RTC);

  ///
  /// set RTC_EN bit in PM1_EN to wake up from the alarm
  ///
  IoWrite16 (
    ABase + R_PCH_ACPI_PM1_EN,
    (IoRead16 (ABase + R_PCH_ACPI_PM1_EN) | B_PCH_ACPI_PM1_EN_RTC)
    );
  return Status;
}

/**
  Retrieve PCH platform specific ResetData

  @param[in]  Guid      PCH platform specific reset GUID.
  @param[out] DataSize  The size of ResetData in bytes.

  @retval ResetData     A platform specific reset that the exact type of 
                        the reset is defined by the EFI_GUID that follows
                        the Null-terminated Unicode string.
  @retval NULL          If Guid is not defined in PCH platform specific reset.
**/
VOID *
EFIAPI
GetResetData (
  IN   EFI_GUID *Guid,
  OUT  UINTN    *DataSize
  )
{
  *DataSize = 0;
  if (CompareGuid (Guid, &gPchPowerCycleResetGuid)) {
    *DataSize = sizeof (mPchPowerCycleReset);
    return (VOID *)&mPchPowerCycleReset;
  } else if (CompareGuid (Guid, &gPchGlobalResetGuid)) {
    *DataSize = sizeof (mPchGlobalReset);
    return (VOID *)&mPchGlobalReset;
  } else if (CompareGuid (Guid, &gPchGlobalResetWithEcGuid)) {
    *DataSize = sizeof (mPchGlobalResetWithEc);
    return (VOID *)&mPchGlobalResetWithEc;
  }
   
   return NULL;
}

/**
  Execute Pch Reset from the host controller.

  @param[in] This                 Pointer to the PCH_RESET_PROTOCOL instance.
  @param[in] ResetType            UEFI defined reset type.
  @param[in] DataSize             The size of ResetData in bytes.
  @param[in] ResetData            Optional element used to introduce a platform specific reset.
                                  The exact type of the reset is defined by the EFI_GUID that follows
                                  the Null-terminated Unicode string.

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_INVALID_PARAMETER   If ResetType is invalid.
**/
EFI_STATUS
EFIAPI
Reset (
  IN PCH_RESET_PROTOCOL *This,
  IN PCH_RESET_TYPE     PchResetType,
  IN UINTN              DataSize,
  IN VOID               *ResetData OPTIONAL
  )
{
  PCH_RESET_INSTANCE  *PchResetInstance;
  EFI_STATUS          Status;

  PchResetInstance = PCH_RESET_INSTANCE_FROM_THIS (This);  

  Status = PchReset (PchResetInstance, PchResetType);

  return Status;
}

/**
  <b>PchReset Runtime  DXE Driver Entry Point</b>\n
  - <b>Introduction</b>\n
    The PchReset Runtime DXE driver provide a standard way for other modules to 
    use the PCH Reset Interface in DXE/SMM/Runtime environments. It has no longer
    hooked ResetSystem() function of the runtime service table.

  - @pre
    - If there is any driver which needs to run the callback function right before 
      issuing the reset, PCH Reset Callback Protocol will need to be installed
      before PCH Reset Runtime DXE driver. If PchReset Runtime DXE driver is run
      before Status Code Runtime Protocol is installed and there is the need
      to use Status code in the driver, it will be necessary to add EFI_STATUS_CODE_RUNTIME_PROTOCOL_GUID
      to the dependency file.
    - @link _PCH_RESET_CALLBACK_PROTOCOL  PCH_RESET_CALLBACK_PROTOCOL @endlink

  - @result
    The Reset driver produces @link _PCH_RESET_PROTOCOL PCH_RESET_PROTOCOL @endlink

  @param[in] ImageHandle          Image handle of the loaded driver
  @param[in] SystemTable          Pointer to the System Table

  @retval EFI_SUCCESS             Thread can be successfully created
  @retval EFI_OUT_OF_RESOURCES    Cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR        Cannot create the timer service
**/
EFI_STATUS
EFIAPI
InstallPchReset (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                      Status;
// APTIOV_SERVER_OVERRIDE_RC_START : PCI Address is added as Runtime Attributes in SbDxe.c.
#if 0
  UINT64                          BaseAddress;
  UINT64                          Length;
  UINT32                          PwrmBaseAddress;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR MemorySpaceDescriptor;
  UINT64                          Attributes;
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : PCI Address is added as Runtime Attributes in SbDxe.c.
  EFI_EVENT                       AddressChangeEvent;
  EFI_PEI_HOB_POINTERS            HobPtr;
  PCH_POLICY_HOB                  *PchPolicyHob;

  DEBUG ((DEBUG_INFO, "InstallPchReset() Start\n"));
// APTIOV_SERVER_OVERRIDE_RC_START
// PCI Address is added as Runtime Attributes in SbDxe.c.
#if 0
  //
  // Set PMC PCI address space to RUNTIME MEMORY.
  //
  BaseAddress = MmPciBase(
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_PMC,
                  PCI_FUNCTION_NUMBER_PCH_PMC
                  );
  Length  = 0x1000; // 4KB
  DEBUG ((DEBUG_INFO, "BaseAddress %lx\n", BaseAddress));   // Add Debug message

  Status  = gDS->GetMemorySpaceDescriptor (BaseAddress, &MemorySpaceDescriptor);
  ASSERT_EFI_ERROR (Status);

  Attributes = MemorySpaceDescriptor.Attributes | EFI_MEMORY_RUNTIME;

  Status = gDS->SetMemorySpaceAttributes (
                  BaseAddress,
                  Length,
                  Attributes
                  );
  ASSERT_EFI_ERROR (Status);
#endif

// PCH MMIO resources not added to GCD in PchInitPreMem.c. Used RSVCHIPSET in SB.SDL to add this memory to GCD map.
  #if 0
  //
  // Set PWRM MMIO address space to RUNTIME MEMORY.
  //
  PchPwrmBaseGet (&PwrmBaseAddress);
  Length = 0x10000; // 64KB

  Status  = gDS->GetMemorySpaceDescriptor (PwrmBaseAddress, &MemorySpaceDescriptor);
  ASSERT_EFI_ERROR (Status);

  Attributes = MemorySpaceDescriptor.Attributes | EFI_MEMORY_RUNTIME;

  Status = gDS->SetMemorySpaceAttributes (
                  PwrmBaseAddress,
                  Length,
                  Attributes
                  );
  ASSERT_EFI_ERROR (Status);
  #endif
// APTIOV_SERVER_OVERRIDE_RC_END
  ///
  /// Allocate Runtime memory for the PchReset protocol instance.
  ///
  mPchResetInstance = AllocateRuntimeZeroPool (sizeof (PCH_RESET_INSTANCE));
  if (mPchResetInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = PchResetConstructor (mPchResetInstance);
  ///
  /// Initialize the Reset protocol instance
  ///
  mPchResetInstance->PchResetInterface.PchResetProtocol.Reset        = Reset;
  mPchResetInstance->PchResetInterface.PchResetProtocol.GetResetData = GetResetData;
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Install protocol interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mPchResetInstance->Handle,
                  &gPchResetProtocolGuid,
                  &mPchResetInstance->PchResetInterface.PchResetProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  ///
  /// Create Address Change event 
  ///
  ///
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  PchResetVirtualAddressChangeEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &AddressChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  HobPtr.Guid   = GetFirstGuidHob (&gPchPolicyHobGuid);
  if (HobPtr.Guid != NULL) {
    PchPolicyHob = GET_GUID_HOB_DATA (HobPtr.Guid);
    mCapsuleResetType = PchPolicyHob->PmConfig.CapsuleResetType;
  }
  ///
  /// The Lib Deconstruct will automatically be called when entrypoint return error.
  ///
  DEBUG ((DEBUG_INFO, "InstallPchReset() End\n"));

  return Status;
}

/**
  If need be, do any special reset required for capsules. For this
  implementation where we're called from the ResetSystem() api,
  just set our capsule variable and return to let the caller
  do a soft reset.
**/
VOID
CapsuleS3Reset (
  VOID
  )
{
  UINT32      Data32;
  UINT32      Eflags;
  UINT16      ABase;

  DEBUG ((DEBUG_INFO, "Capsule Present: Will be issuing S3 reset.\n"));

  ///
  /// Wake up system 2 seconds after putting system into S3 to complete the reset operation.
  ///
  SetSystemWakeupAlarm (2);
  ///
  /// Process capsules across a system reset.
  ///
  ABase = mPchResetInstance->PchAcpiBase;
  ASSERT (ABase != 0);

  Data32  = IoRead32 ((UINTN) (ABase + R_PCH_ACPI_PM1_CNT));

  Data32  = (UINT32) ((Data32 & ~(B_PCH_ACPI_PM1_CNT_SLP_TYP + B_PCH_ACPI_PM1_CNT_SLP_EN)) | V_PCH_ACPI_PM1_CNT_S3);

  Eflags  = (UINT32) AsmReadEflags ();

  if ((Eflags & 0x200)) {
    DisableInterrupts ();
  }

  AsmWbinvd ();
  AsmWriteCr0 (AsmReadCr0 () | 0x060000000);

  IoWrite32 (
    (UINTN) (ABase + R_PCH_ACPI_PM1_CNT),
    (UINT32) Data32
    );

  Data32 = Data32 | B_PCH_ACPI_PM1_CNT_SLP_EN;

  IoWrite32 (
    (UINTN) (ABase + R_PCH_ACPI_PM1_CNT),
    (UINT32) Data32
    );

  if ((Eflags & 0x200)) {
    EnableInterrupts ();
  }
  ///
  /// Should not return
  ///
  CpuDeadLoop ();
}

/**
  Execute call back function for Pch Reset.

  @param[in] PchResetType         Pch Reset Types which includes PowerCycle, Globalreset.

  @retval EFI_SUCCESS             The callback function has been done successfully
  @retval EFI_NOT_FOUND           Failed to find Pch Reset Callback protocol. Or, none of
                                  callback protocol is installed.
  @retval Others                  Do not do any reset from PCH
**/
EFI_STATUS
EFIAPI
PchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  )
{
  EFI_STATUS                  Status;
  UINTN                       NumHandles;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       Index;
  PCH_RESET_CALLBACK_PROTOCOL *PchResetCallback;
  UINTN                       Size;
  UINTN                       CapsuleDataPtr;

  if (EfiAtRuntime () == FALSE) {
    ///
    /// Retrieve all instances of Pch Reset Callback protocol
    ///
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gPchResetCallbackProtocolGuid,
                    NULL,
                    &NumHandles,
                    &HandleBuffer
                    );

    if (EFI_ERROR (Status)) {
      ///
      /// Those drivers that need to install Pch Reset Callback protocol have the responsibility
      /// to make sure themselves execute before Pch Reset Runtime driver.
      ///
      if (Status == EFI_NOT_FOUND) {
        DEBUG ((DEBUG_ERROR | DEBUG_INFO, "Or, none of Pch Reset callback protocol is installed.\n"));
      }

      return Status;
    }

    for (Index = 0; Index < NumHandles; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gPchResetCallbackProtocolGuid,
                      (VOID **) &PchResetCallback
                      );
      ASSERT_EFI_ERROR (Status);

      if (!EFI_ERROR (Status)) {
        PchResetCallback->ResetCallback (PchResetType);
      } else {
        DEBUG ((DEBUG_ERROR | DEBUG_INFO, "Failed to locate Pch Reset Callback protocol.\n"));
        return Status;
      }
    }
  }
  if(PchResetType == WarmReset) {
    ///
    /// Check if there are pending capsules to process
    ///
    Size = sizeof (CapsuleDataPtr);
    Status = EfiGetVariable (
               EFI_CAPSULE_VARIABLE_NAME,
               &gEfiCapsuleVendorGuid,
               NULL,
               &Size,
               (VOID *) &CapsuleDataPtr
               );
    if (Status == EFI_SUCCESS) {
      if (mCapsuleResetType == CAPSULE_RESET_S3) { //default value S3 resume
        CapsuleS3Reset ();
      }
      AsmWbinvd ();
  }
  }
  return EFI_SUCCESS;
}

/**
  Fixup internal data pointers so that the services can be called in virtual mode.

  @param[in] Event                The event registered.
  @param[in] Context              Event context. Not used in this event handler.

**/
VOID
EFIAPI
PchResetVirtualAddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  gRT->ConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &(mPchResetInstance->PchResetInterface.PchResetProtocol.Reset));
  gRT->ConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &(mPchResetInstance->PchPmcBase));
  gRT->ConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &(mPchResetInstance->PchPwrmBase));
  gRT->ConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &(mPchResetInstance));
}
