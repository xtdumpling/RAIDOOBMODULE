/** @file
  Processes ASF messages.

@copyright
  Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
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

#include <PiDxe.h>
#include <Amt.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/Runtime.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Pi/PiStatusCode.h>
#include "AlertStandardFormatDxe.h"
#include <Library/TimerLib.h>
#include <IncludePrivate/Library/AlertStandardFormatLib.h>

#include "MkhiMsgs.h"
#include "Protocol/HeciProtocol.h"
#include "MeState.h"
#include "Library/MeTypeLib.h"


GLOBAL_REMOVE_IF_UNREFERENCED UINT64                          mAsfMonotonicCount;
GLOBAL_REMOVE_IF_UNREFERENCED ASF_BOOT_OPTIONS                mAsfBootOptions;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                         mBootOptionsValid;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                         mAsfAddressValid;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                         mProgressEventEnabled;

GLOBAL_REMOVE_IF_UNREFERENCED ALERT_STANDARD_FORMAT_INSTANCE  *mAsfContext;

GLOBAL_REMOVE_IF_UNREFERENCED AMT_PET_QUEUE_PROTOCOL          gAmtPetQueue;
GLOBAL_REMOVE_IF_UNREFERENCED AMT_PET_QUEUE_PROTOCOL          *gAmtPetQueueProtocol;
GLOBAL_REMOVE_IF_UNREFERENCED AMT_FORCE_PUSH_PET_POLICY_HOB   *gAmtForcePushPETPolicyHob;

/**
  Update ASF Boot Options data in ACPI ASF RMCP table

  @param[in] AsfBootOptions     ASF Boot Options data for ACPI ASF RMCP table used
**/
VOID
UpdateAcpiAsfRmcpBootOptions (
  ASF_BOOT_OPTIONS                    *AsfBootOptions
  )
{
  EFI_STATUS                          Status;
  ACPI_2_0_ASF_DESCRIPTION_TABLE      *AcpiAsfTable;
  UINTN                               Handle;
  UINT16                              *WordPointer;
  EFI_ACPI_TABLE_PROTOCOL             *AcpiTable;

  ///
  /// Locate ACPI Table Protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);
  if (EFI_ERROR(Status)) {
    return;
  }
  ///
  /// Initialize ASL manipulation library
  ///
  InitializeAslUpdateLib ();

  ///
  /// Locate the ASF Table
  ///
  Status = LocateAcpiTableBySignature (
             EFI_ACPI_ASF_DESCRIPTION_TABLE_SIGNATURE,
             (EFI_ACPI_DESCRIPTION_HEADER **) &AcpiAsfTable,
             &Handle
             );
  if (EFI_ERROR (Status)) {
    return;
  }

  if (AsfBootOptions->SubCommand == ASF_BOOT_OPTIONS_PRESENT) {
    ///
    /// Check Asf table if get boot option successfully
    ///
    AcpiAsfTable->AsfRmcp.RMCPCompletionCode  = 00;
    AcpiAsfTable->AsfRmcp.RMCPIANA            = AsfBootOptions->IanaId;
    AcpiAsfTable->AsfRmcp.RMCPSpecialCommand  = AsfBootOptions->SpecialCommand;
    WordPointer = (UINT16 *) &AcpiAsfTable->AsfRmcp.RMCPSpecialCommandParameter;
    *WordPointer = AsfBootOptions->SpecialCommandParam;
    WordPointer = (UINT16 *) &AcpiAsfTable->AsfRmcp.RMCPBootOptions;
    *WordPointer = AsfBootOptions->BootOptions;
    WordPointer = (UINT16 *) &AcpiAsfTable->AsfRmcp.RMCPOEMParameters;
    *WordPointer = AsfBootOptions->OemParameters;
  } else {
    ///
    /// Failed to get boot option, update the completion code to 0x1
    ///
    AcpiAsfTable->AsfRmcp.RMCPCompletionCode = 01;
  }
  if (Handle != 0) {
    Status = AcpiTable->UninstallAcpiTable (
                          AcpiTable,
                          Handle
                          );
  }
  ///
  /// Update the Acpi Asf table
  ///
  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        AcpiAsfTable,
                        sizeof(ACPI_2_0_ASF_DESCRIPTION_TABLE),
                        &Handle
                        );
  FreePool (AcpiAsfTable);
}

/**
  16 bits are changed backward for string transfer to value used

  @param[in] Value                The value to be transferred

  @retval UINT16 Value            The value after transferring
**/
UINT16
Swap16 (
  IN  UINT16                      Value
  )
{
  UINT16                          OutValue;
  UINT8                           *TempIn;
  UINT8                           *TempOut;

  TempIn      = (UINT8 *) &Value;
  TempOut     = (UINT8 *) &OutValue;

  TempOut[0]  = TempIn[1];
  TempOut[1]  = TempIn[0];

  return OutValue;
}

/**
  The driver entry point - detect ASF support or not, if support, will install relative protocol.

  @param[in] ImageHandle          Handle for this drivers loaded image protocol.
  @param[in] SystemTable          EFI system table.

  @retval EFI_SUCCESS             The driver installed without error.
  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures.
**/
EFI_STATUS
EFIAPI
AlertStandardFormatDriverEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_DATA_HUB_PROTOCOL           *DataHub;
  EFI_EVENT                       DataHubEvent;
  UINT64                          DataClass;
  ASF_BOOT_OPTIONS                *AsfBootOptions;

#if defined(SPS_SUPPORT) && SPS_SUPPORT
  if (!MeTypeIsAmt()) {
    return EFI_UNSUPPORTED;
  }
#endif // SPS_SUPPORT
  ///
  /// First check if ASF support is enabled in Setup.
  ///
  if (!AsfSupported ()) {
    return EFI_UNSUPPORTED;
  }

  mBootOptionsValid = FALSE;
  mAsfAddressValid  = FALSE;

  mAsfContext       = AllocateZeroPool (sizeof (ALERT_STANDARD_FORMAT_INSTANCE));
  ASSERT (mAsfContext != NULL);
  if (mAsfContext == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// Get the Data Hub Protocol. Assume only one instance
  /// of Data Hub Protocol is available in the system.
  ///
  Status = gBS->LocateProtocol (
                  &gEfiDataHubProtocolGuid,
                  NULL,
                  (VOID **) &DataHub
                  );
  ASSERT_EFI_ERROR (Status);

  mAsfContext->Handle = ImageHandle;
  mAsfContext->Signature = ALERT_STANDARD_FORMAT_PRIVATE_DATA_SIGNATURE;
  mAsfContext->AlertStandardFormat.GetSmbusAddr = GetSmbusAddr;
  mAsfContext->AlertStandardFormat.SetSmbusAddr = SetSmbusAddr;
  mAsfContext->AlertStandardFormat.GetBootOptions = GetBootOptions;
  mAsfContext->AlertStandardFormat.SendAsfMessage = SendAsfMessage;

  ///
  /// Sending ASF Messaging if ManageabilityMode is not zero
  ///
  if (ManageabilityModeSetting () != MNT_OFF) {
    ///
    /// ActiveManagement Protocol is not ready at this point.  Need to Check Boot Options Manually
    ///
    mProgressEventEnabled = FALSE;
    Status                = GetBootOptions (&(mAsfContext->AlertStandardFormat), &AsfBootOptions);
    if (!EFI_ERROR (Status)) {
      if (AsfBootOptions->SubCommand == ASF_BOOT_OPTIONS_PRESENT) {
        if ((AsfBootOptions->BootOptions & FORCE_PROGRESS_EVENTS) == FORCE_PROGRESS_EVENTS) {
          mProgressEventEnabled = TRUE;
        }
      }
    }
    ///
    /// If no boot options available, check policy
    ///
    if (!mProgressEventEnabled) {
      mProgressEventEnabled = FwProgressSupport ();
    }
    ///
    /// Create message queue
    ///
    AmtCreateMessageQueue ();

    ///
    /// Get ForcePushPetPolicy Hob
    ///
    gAmtForcePushPETPolicyHob = GetForcePushPetPolicy ();

    ///
    /// save PEI force push error event from hob to variable
    ///
    SaveForcePushErrorEventFromPeiToDxe ();

    ///
    /// Try to send message
    ///
    SendPETMessageInQueue ();

    ///
    /// Register our Setup Data Filter Function.
    /// This function is notified at the lowest TPL
    ///
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    DataHubEventCallback,
                    NULL,
                    &DataHubEvent
                    );
    ASSERT_EFI_ERROR (Status);

    if (mProgressEventEnabled) {
      DataClass = EFI_DATA_RECORD_CLASS_ERROR | EFI_DATA_RECORD_CLASS_PROGRESS_CODE;
    } else {
      DataClass = EFI_DATA_RECORD_CLASS_ERROR;
    }

    Status = DataHub->RegisterFilterDriver (
                        DataHub,
                        DataHubEvent,
                        TPL_APPLICATION,
                        DataClass,
                        NULL
                        );
    ASSERT_EFI_ERROR (Status);
  }
  ///
  /// Install the AlertStandardFormat interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mAsfContext->Handle,
                  &gAlertStandardFormatProtocolGuid,
                  &mAsfContext->AlertStandardFormat,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Return the SMBus address used by the ASF driver.
  Not applicable in Intel ME/HECI system, need to return EFI_UNSUPPORTED.

  @param[in] This                 The address of protocol
  @param[in] SmbusDeviceAddress   Out put the Smbus Address

  @exception EFI_UNSUPPORTED      The function is unsupported by this driver
**/
EFI_STATUS
EFIAPI
GetSmbusAddr (
  IN  ALERT_STANDARD_FORMAT_PROTOCOL       *This,
  OUT UINTN                                *SmbusDeviceAddress
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Set the SMBus address used by the ASF driver. 0 is an invalid address.
  Not applicable in Intel ME/HECI system, need to return EFI_UNSUPPORTED.

  @param[in] This                 The address of protocol
  @param[in] SmbusDeviceAddress   SMBus address of the device

  @exception EFI_UNSUPPORTED      The function is unsupported by this driver
**/
EFI_STATUS
EFIAPI
SetSmbusAddr (
  IN  ALERT_STANDARD_FORMAT_PROTOCOL       *This,
  IN  UINTN                                SmbusDeviceAddress
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Return EFI_SUCCESS if Firmware Init Complete is set in HFS[9].

  @param[in] Heci                 EFI_HECI_PROTOCOL
  @param[in] Timeout              Time in second

  @retval EFI_SUCCESS             Firmware Init Complete is set
  @retval EFI_TIMEOUT             Firmware Init Complete is not set in 5 seconds
**/
EFI_STATUS
WaitFirmwareInitDone (
  HECI_PROTOCOL                   *Heci,
  UINT32                          Timeout
  )
{
  UINT32      idx;
  EFI_STATUS  Status;
  UINT32      MeStatus;

  Status  = EFI_SUCCESS;
  idx     = 0;

  Heci->GetMeStatus (&MeStatus);

  while (!ME_STATUS_IS_ME_FW_INIT_COMPLETE (MeStatus)) {
    MicroSecondDelay (100000);
    idx++;
    if (idx > Timeout * 10) {
      Status = EFI_TIMEOUT;
      break;
    }

    Heci->GetMeStatus (&MeStatus);
  }

  return Status;
}

/**
  Initialize KVM by sending HECI messafe to ME

  @param[in] Event                The event registered.
  @param[in] Context              Event context. Not used in this event handler.
**/
VOID
EFIAPI
QueryKvm (
  IN EFI_EVENT                    Event,
  IN VOID                         *Context
  )
{
  BdsKvmInitialization ();
  gBS->CloseEvent (Event);
}

/**
  Return the ASF Boot Options obtained from the controller. If the
  Boot Options parameter is NULL and no boot options have been retrieved,
  Query the ASF controller for its boot options.
  Get ASF Boot Options through HECI.

  @param[in] This                 The address of protocol
  @param[in] AsfBootOptions       Pointer to ASF boot options to copy current ASF Boot options

  @retval EFI_SUCCESS             Boot options copied
  @retval EFI_NOT_READY           No boot options
**/
EFI_STATUS
EFIAPI
GetBootOptions (
  IN      ALERT_STANDARD_FORMAT_PROTOCOL       *This,
  IN  OUT ASF_BOOT_OPTIONS                     **AsfBootOptions
  )
{
  EFI_STATUS                          Status;
  UINT8                               ConsoleLock;
  UINT8                               LastConsoleLock;
  UINTN                               Size;
  UINT32                              VarAttributes;
  UINT32                              MeStatus;
  UINT32                              MeMode;
  EFI_STATUS                          TempStatus;
  VOID                                *AfterConsolOutNotifyReg;
  EFI_EVENT                           AfterConsolOutInstalledEvent;
  HECI_PROTOCOL                       *Heci;
  HECI_ASF_GET_BOOT_OPTIONS_RESPONSE  HeciAsfGetBootOptionsResponse;
  HECI_ASF_CLEAR_BOOT_OPTION          HeciAsfClearBootOption;
  UINT32                              HeciLength;

  if (!mBootOptionsValid) {
    Status = gBS->LocateProtocol (
                    &gHeciProtocolGuid,
                    NULL,
                    (VOID **) &Heci
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "ASF Get Boot Options failed!, Status = %r\n", Status));
      return Status;
    }

    ZeroMem ((VOID *) &mAsfBootOptions, sizeof (ASF_BOOT_OPTIONS));
    mAsfBootOptions.SubCommand  = ASF_BOOT_OPTIONS_NOT_PRESENT;
    mAsfBootOptions.Version     = 0x10;
    UpdateAcpiAsfRmcpBootOptions (&mAsfBootOptions);

    ///
    /// Get ME Status
    ///
    TempStatus = Heci->GetMeStatus (&MeStatus);
    ASSERT_EFI_ERROR (TempStatus);

    ///
    /// Get ME Operation Mode
    ///
    Heci->GetMeMode (&MeMode);

    ///
    /// Only Send ASF Get Boot Options message when ME is ready and ME FW INIT is completed
    ///
    if (ME_STATUS_IS_ME_FW_BOOT_OPTIONS_PRESENT (MeStatus) &&
        (ManageabilityModeSetting () != MNT_OFF) &&
        (MeMode == ME_MODE_NORMAL)
        ) {
      if (WaitFirmwareInitDone (Heci, 40) != EFI_TIMEOUT) {
        ///
        /// Prepare Boot Option Command header
        ///
        HeciAsfGetBootOptionsResponse.Command                   = ASF_MESSAGE_COMMAND_ASF_CONFIGURATION;
        HeciAsfGetBootOptionsResponse.ByteCount                 = ASF_MESSAGE_BYTEOUNTT_GET_BOOT_OPT;
        HeciAsfGetBootOptionsResponse.AsfBootOptions.SubCommand = ASF_MESSAGE_SUBCOMMAND_RETURN_BOOT_OPT;
        HeciAsfGetBootOptionsResponse.AsfBootOptions.Version    = 0x10;
        HeciLength = HECI_ASF_GET_BOOT_OPTIONS_LENGTH;
        Status = Heci->SendMsg (
                        HECI1_DEVICE,
                        (UINT32 *) &HeciAsfGetBootOptionsResponse,
                        HeciLength,
                        BIOS_ASF_HOST_ADDR,
                        HECI_ASF_MESSAGE_ADDR
                        );
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "ASF Get Boot Options failed!(SendwACK), Status = %r\n", Status));
          return Status;
        }

        HeciLength = HECI_ASF_GET_BOOT_OPTIONS_RESPONSE_LENGTH;
        Status = Heci->ReadMsg (
                        HECI1_DEVICE,
                        BLOCKING,
                        (UINT32 *) &HeciAsfGetBootOptionsResponse,
                        &HeciLength
                        );
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "ASF Get Boot Options failed!(ReadMsg), Status = %r\n", Status));
          return Status;
        }

        if (HeciAsfGetBootOptionsResponse.Command != ASF_MESSAGE_COMMAND_ASF_CONFIGURATION) {
          DEBUG ((DEBUG_ERROR, "ASF Get Boot Options failed!(ReadMsg), Wrong responce 0x%x\n", HeciAsfGetBootOptionsResponse.Command));
          return EFI_DEVICE_ERROR;
        }

        CopyMem (
          (VOID *) &mAsfBootOptions,
          (VOID *) &(HeciAsfGetBootOptionsResponse.AsfBootOptions),
          sizeof (ASF_BOOT_OPTIONS)
          );

        ///
        /// By default, this table is belong to all ACPI table versions.
        /// It is ok if the table is not found and that means the platform does not publish that version.
        ///
        UpdateAcpiAsfRmcpBootOptions (&mAsfBootOptions);

        mAsfBootOptions.SpecialCommandParam = Swap16 (mAsfBootOptions.SpecialCommandParam);

        ///
        /// ASF Get Boot Options -  Clear Boot Options
        /// Need to do this so don't get caught in an endless loop plus by
        /// definition get boot options is a one time boot situation.
        /// So if server on other end of ASF device wants another back to back
        /// boot it will request it. Do this only if there were boot options
        ///
        if (mAsfBootOptions.SubCommand == ASF_BOOT_OPTIONS_PRESENT) {
          if(
              ( mAsfBootOptions.SpecialCommand == ASF_INTEL_OEM_CMD )&&
              ( mAsfBootOptions.SpecialCommandParam & ENABLE_SECURE_ERASE )
              ) {
            //
            // When secure erase boot option is set, don't clear boot options, but discard
            // any other boot option except secure erase, KVM and SOL
            //
            DEBUG (( DEBUG_INFO, "ASF Clear Boot Options skipped due to secure erase command\n"));
            DEBUG (( DEBUG_INFO, "Dumping SpecialCommandParam, BootOptions and OemParameters before discarding\n"));
            DEBUG (( DEBUG_INFO, "mAsfBootOptions.SpecialCommandParam = 0x%x\n", mAsfBootOptions.SpecialCommandParam));
            DEBUG (( DEBUG_INFO, "mAsfBootOptions.BootOptions         = 0x%x\n", mAsfBootOptions.BootOptions));
            DEBUG (( DEBUG_INFO, "mAsfBootOptions.OemParameters       = 0x%x\n", mAsfBootOptions.OemParameters));
            DEBUG (( DEBUG_INFO, "End\n\n" ));
            mAsfBootOptions.SpecialCommandParam  &= ( ENABLE_SECURE_ERASE | USE_KVM );
            mAsfBootOptions.OemParameters        &= USE_SOL;
            mAsfBootOptions.BootOptions           = 0x0;
          } else {
            HeciAsfClearBootOption.Command                        = ASF_MESSAGE_COMMAND_ASF_CONFIGURATION;
            HeciAsfClearBootOption.AsfClearBootOptions.SubCommand = ASF_MESSAGE_SUBCOMMAND_CLEAR_BOOT_OPT;
            HeciAsfClearBootOption.AsfClearBootOptions.Version    = ASF_MESSAGE_VERSIONNUMBER;
            HeciAsfClearBootOption.ByteCount                      = ASF_MESSAGE_BYTECOUNT_CLEAR_BOOT_OPT;
            HeciLength = HECI_ASF_CLEAR_BOOT_OPTION_LENGTH;
            Status = Heci->SendMsg (
                            HECI1_DEVICE,
                            (UINT32 *) &HeciAsfClearBootOption,
                            HeciLength,
                            BIOS_ASF_HOST_ADDR,
                            HECI_ASF_MESSAGE_ADDR
                            );

            if (EFI_ERROR (Status)) {
              DEBUG ((DEBUG_ERROR, "ASF Clear Boot Options failed!, Status = %r\n", Status));
              return Status;
            }
          }
        }
      }
    }
    ///
    /// Set flag so we don't try to get the Boot options again.
    ///
    mBootOptionsValid = TRUE;

    ///
    /// Set up Event for KVM for when Output Display Console is installed
    ///
    if ((mAsfBootOptions.SpecialCommandParam & USE_KVM) == USE_KVM) {

      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_NOTIFY,
                      QueryKvm,
                      NULL,
                      &AfterConsolOutInstalledEvent
                      );

      Status = gBS->RegisterProtocolNotify (
                      &gEfiConsoleOutDeviceGuid,
                      AfterConsolOutInstalledEvent,
                      &AfterConsolOutNotifyReg
                      );

    }
    ///
    /// Check for keyboard locking in the boot options
    ///
    Size = sizeof (ConsoleLock);
    LastConsoleLock = NO_LOCK_CONSOLE;
    Status = gRT->GetVariable (
                    CONSOLE_LOCK_VARIABLE_NAME,
                    &gConsoleLockGuid,
                    &VarAttributes,
                    &Size,
                    &LastConsoleLock
                    );
    if (Status == EFI_NOT_FOUND) {
      VarAttributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
    }

    //
    // Update the variable if the variable has not been created yet or GetVariable with success.
    //
    if ((Status == EFI_SUCCESS) || (Status == EFI_NOT_FOUND)) {
      ConsoleLock = NO_LOCK_CONSOLE;

      if (mAsfBootOptions.SubCommand == ASF_BOOT_OPTIONS_PRESENT) {
        if ((mAsfBootOptions.BootOptions & LOCK_KEYBOARD) == LOCK_KEYBOARD) {
          ConsoleLock = LOCK_CONSOLE;
        }
      }
      if ((Status == EFI_NOT_FOUND) || (ConsoleLock != LastConsoleLock)) {
        ///
        /// Save the console lock flag for later usage in console locking determination
        ///
        gRT->SetVariable (
               CONSOLE_LOCK_VARIABLE_NAME,
               &gConsoleLockGuid,
               VarAttributes,
               sizeof (ConsoleLock),
               &ConsoleLock
               );
        DEBUG ((DEBUG_INFO, "Update ConsoleLock flag to 0x%x\n", ConsoleLock));
      }
    }
  }

  if (AsfBootOptions) {
    *AsfBootOptions = &mAsfBootOptions;

    ///
    /// If we're asking for the options, then resend them to the debug output just encase they've been hosed.
    ///
    DEBUG ((DEBUG_INFO, "mAsfBootOptions.SubCommand          = 0x%x\n", mAsfBootOptions.SubCommand));
    DEBUG ((DEBUG_INFO, "mAsfBootOptions.Version             = 0x%x\n", mAsfBootOptions.Version));
    DEBUG ((DEBUG_INFO, "mAsfBootOptions.IanaId              = 0x%x\n", mAsfBootOptions.IanaId));
    DEBUG ((DEBUG_INFO, "mAsfBootOptions.SpecialCommand      = 0x%x\n", mAsfBootOptions.SpecialCommand));
    DEBUG ((DEBUG_INFO, "mAsfBootOptions.SpecialCommandParam = 0x%x\n", mAsfBootOptions.SpecialCommandParam));
    DEBUG ((DEBUG_INFO, "mAsfBootOptions.BootOptions         = 0x%x\n", mAsfBootOptions.BootOptions));
    DEBUG ((DEBUG_INFO, "mAsfBootOptions.OemParameters       = 0x%x\n", mAsfBootOptions.OemParameters));
  }

  return EFI_SUCCESS;
}

/**
  Send ASF Message through HECI.

  @param[in] This                 The address of protocol
  @param[in] AsfMessage           Pointer to ASF message

  @retval EFI_SUCCESS             Boot options copied
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_NOT_READY           No controller
**/
EFI_STATUS
EFIAPI
SendAsfMessage (
  IN  ALERT_STANDARD_FORMAT_PROTOCOL       *This,
  IN  ASF_MESSAGE                          *AsfMessage
  )
{
  EFI_STATUS                  Status;
  HECI_PROTOCOL               *Heci;
  UINT32                      HeciLength;
  HECI_ASF_PUSH_PROGRESS_CODE HeciAsfPushProgressCode;
  UINT32                      MeStatus;
  UINT32                      MeMode;
  EFI_STATUS                  TempStatus;

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );

  if (!EFI_ERROR (Status)) {
    TempStatus = Heci->GetMeStatus (&MeStatus);
    ASSERT_EFI_ERROR (TempStatus);

    ///
    /// Get ME Operation Mode
    ///
    Heci->GetMeMode (&MeMode);

    ///
    /// Only send ASF Push Progress code when ME is ready and ME is in normal mode.  Ignore FW Init Status.
    ///
    if (ME_STATUS_ME_STATE_ONLY (MeStatus) == ME_READY && MeMode == ME_MODE_NORMAL) {
      ZeroMem ((VOID *) &HeciAsfPushProgressCode, sizeof (HECI_ASF_PUSH_PROGRESS_CODE));
      HeciAsfPushProgressCode.Command   = ASF_MESSAGE_COMMAND_MESSAGE;
      HeciAsfPushProgressCode.ByteCount = 0x13;
      HeciLength                        = HECI_ASF_PUSH_PROGRESS_CODE_LENGTH;
      CopyMem ((VOID *) &(HeciAsfPushProgressCode.AsfMessage), (VOID *) AsfMessage, sizeof (ASF_MESSAGE));

      Status = Heci->SendMsg (
                      HECI1_DEVICE,
                      (UINT32 *) &HeciAsfPushProgressCode,
                      HeciLength,
                      BIOS_ASF_HOST_ADDR,
                      HECI_ASF_MESSAGE_ADDR
                      );
    }
  }

  return Status;
}

/**
  This routine returns ForcePushPetPolicy information.

  @retval AMT_FORCE_PUSH_PET_POLICY_HOB   ForcePushPetPolicy information.
**/
AMT_FORCE_PUSH_PET_POLICY_HOB *
GetForcePushPetPolicy (
  VOID
  )
{
  AMT_FORCE_PUSH_PET_POLICY_HOB *AmtForcePushPETPolicyHob;
  EFI_STATUS                    Status;

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &AmtForcePushPETPolicyHob);
  ASSERT_EFI_ERROR (Status);

  return GetNextGuidHob (&gAmtForcePushPetPolicyGuid, AmtForcePushPETPolicyHob);
}

/**
  This routine checks whethre current message is ForcePush message.

  @param[in] MessageType          AMT PET Message Type.

  @retval TRUE                    It is ForcePush message.
  @retval FALSE                   It is not ForcePush message.
**/
BOOLEAN
IsForcePushErrorEvent (
  IN  ASF_FRAMEWORK_MESSAGE_TYPE  MessageType
  )
{
  UINTN Index;
  UINTN Number;

  Number = (gAmtForcePushPETPolicyHob->EfiHobGuidType.Header.HobLength - sizeof (EFI_HOB_GUID_TYPE)) /
             sizeof (ASF_FRAMEWORK_MESSAGE_TYPE);
  for (Index = 0; Index < Number; Index++) {
    if (gAmtForcePushPETPolicyHob->MessageType[Index] == MessageType) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Filters all the progress and error codes for Asf.

  @param[in] Event                The event registered.
  @param[in] Context              Event context. Not used in this event handler.
**/
VOID
EFIAPI
DataHubEventCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                        Status;
  EFI_DATA_HUB_PROTOCOL             *DataHub;
  EFI_DATA_RECORD_HEADER            *Record;
  DATA_HUB_STATUS_CODE_DATA_RECORD  *StatusRecord;
  UINTN                             Index;

  ///
  /// Get the Data Hub Protocol. Assume only one instance
  ///
  Status = gBS->LocateProtocol (&gEfiDataHubProtocolGuid, NULL, (VOID **) &DataHub);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Get all available data records from data hub
  ///
  Record = NULL;

  do {
    Status = DataHub->GetNextRecord (DataHub, &mAsfMonotonicCount, &Event, &Record);
    if (!EFI_ERROR (Status)) {
      if (mProgressEventEnabled) {
        if (Record->DataRecordClass == EFI_DATA_RECORD_CLASS_PROGRESS_CODE) {
          StatusRecord = (DATA_HUB_STATUS_CODE_DATA_RECORD *) (Record + 1);
          for (Index = 0; Index < AsfProgressDataHubMapSize / sizeof (ASF_DATA_HUB_MAP); Index++) {
            if (mAsfProgressDataHubMap[Index].StatusCodeValue == StatusRecord->Value) {
              Status = SendPostPacket (mAsfProgressDataHubMap[Index].MessageType);
              break;
            }
          }
        }
      }

      if (Record->DataRecordClass == EFI_DATA_RECORD_CLASS_ERROR) {
        StatusRecord = (DATA_HUB_STATUS_CODE_DATA_RECORD *) (Record + 1);
        for (Index = 0; Index < AsfErrorDataHubMapSize / sizeof (ASF_DATA_HUB_MAP); Index++) {
          if (mAsfErrorDataHubMap[Index].StatusCodeValue == StatusRecord->Value) {
            Status = SendPostPacket (mAsfErrorDataHubMap[Index].MessageType);
            if ((Status == EFI_DEVICE_ERROR) && IsForcePushErrorEvent (mAsfErrorDataHubMap[Index].MessageType)) {
              SaveForcePushErrorEvent (mAsfErrorDataHubMap[Index].MessageType);
            }
            break;
          }
        }
      }
    }
  } while (!EFI_ERROR (Status) && (mAsfMonotonicCount != 0));
}

/**
  Sends a POST packet across ASF

  @param[in] MessageType          POST Status Code

  @retval EFI_DEVICE_ERROR        No message found
  @retval EFI_SUCCESS             Boot options copied
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_NOT_READY           No controller
**/
EFI_STATUS
SendPostPacket (
  IN  ASF_FRAMEWORK_MESSAGE_TYPE      MessageType
  )
{
  EFI_STATUS  Status;
  UINTN       Index;

  Status = EFI_DEVICE_ERROR;
  ///
  /// Find the message to send across the wire
  ///
  for (Index = 0; Index < AsfFrameworkMessageSize / sizeof (ASF_FRAMEWORK_MESSAGE); Index++) {
    if (mAsfFrameworkMessage[Index].MessageType == MessageType) {
      Status = SendAsfMessage (NULL, &mAsfFrameworkMessage[Index].Message);
      break;
    }
  }

  return Status;
}

/**
  Provides an interface that a software module can call to report an ASF DXE status code.

  @param[in] Type                 Indicates the type of status code being reported.
  @param[in] Value                Describes the current status of a hardware or software entity.
                                  This included information about the class and subclass that is
                                  used to classify the entity as well as an operation.
  @param[in] Instance             The enumeration of a hardware or software entity within
                                  the system. Valid instance numbers start with 1.
  @param[in] CallerId             This optional parameter may be used to identify the caller.
                                  This parameter allows the status code driver to apply different
                                  rules to different callers.
  @param[in] Data                 This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_DEVICE_ERROR        The function should not be completed due to a device error.
**/
EFI_STATUS
EFIAPI
AmtReportStatusCode (
  IN  EFI_STATUS_CODE_TYPE        Type,
  IN  EFI_STATUS_CODE_VALUE       Value,
  IN  UINT32                      Instance,
  IN  EFI_GUID                    * CallerId OPTIONAL,
  IN  EFI_STATUS_CODE_DATA        * Data OPTIONAL
  )
{
  UINTN       Index;
  EFI_STATUS  Status;

  if (mProgressEventEnabled) {
    if ((Type & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
      for (Index = 0; Index < AsfProgressDataHubMapSize / sizeof (ASF_DATA_HUB_MAP); Index++) {
        if (mAsfProgressDataHubMap[Index].StatusCodeValue == Value) {
          return SendPostPacket (mAsfProgressDataHubMap[Index].MessageType);
        }
      }
    }
  }

  if ((Type & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
    for (Index = 0; Index < AsfErrorDataHubMapSize / sizeof (ASF_DATA_HUB_MAP); Index++) {
      if (mAsfErrorDataHubMap[Index].StatusCodeValue == Value) {
        Status = SendPostPacket (mAsfErrorDataHubMap[Index].MessageType);
        if ((Status == EFI_DEVICE_ERROR) && IsForcePushErrorEvent (mAsfErrorDataHubMap[Index].MessageType)) {
          SaveForcePushErrorEvent (mAsfErrorDataHubMap[Index].MessageType);
        }

        return Status;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  This routine puts PET message to MessageQueue, which will be sent later.

  @param[in] Type                 StatusCode message type.
  @param[in] Value                StatusCode message value.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures
**/
EFI_STATUS
QueuePetMessage (
  IN  EFI_STATUS_CODE_TYPE        Type,
  IN  EFI_STATUS_CODE_VALUE       Value
  )
{
  AMT_PET_QUEUE_NODE  *NewNode;

  NewNode = AllocateZeroPool (sizeof (AMT_PET_QUEUE_NODE));
  ASSERT (NewNode != NULL);
  if (NewNode == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  NewNode->Signature  = AMT_PET_QUEUE_NODE_SIGNATURE;
  NewNode->Type       = Type;
  NewNode->Value      = Value;
  InsertTailList ((LIST_ENTRY *) &gAmtPetQueueProtocol->MessageList, (LIST_ENTRY *) &NewNode->Link);

  return EFI_SUCCESS;
}

/**
  This routine sends PET message in MessageQueue.

  @param[in] VOID

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_NOT_READY           No controller
**/
EFI_STATUS
SendPETMessageInQueue (
  VOID
  )
{
  EFI_STATUS            Status;
  AMT_PET_QUEUE_HOB     *PETQueueHob;
  EFI_LIST_ENTRY        *Link;
  AMT_PET_QUEUE_NODE    *Node;
  EFI_PEI_HOB_POINTERS  Hob;

  HECI_PROTOCOL         *Heci;
  UINT32                MeStatus;
  EFI_STATUS            TempStatus;

  ///
  /// Try HECI state
  ///
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_READY;
  }

  TempStatus = Heci->GetMeStatus (&MeStatus);
  ASSERT_EFI_ERROR (TempStatus);

  ///
  /// Only send ASF Push Progress code when ME is ready.  Ignore FW Init Status.
  ///
  if (ME_STATUS_ME_STATE_ONLY (MeStatus) != ME_READY) {
    return EFI_NOT_READY;
  }
  ///
  /// Get PETQueueHob
  ///
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &PETQueueHob);

  while (TRUE) {
    PETQueueHob = GetNextGuidHob (&gAmtPetQueueHobGuid, PETQueueHob);
    if (PETQueueHob == NULL) {
      break;
    }
    ///
    /// Send message
    ///
    AmtReportStatusCode (PETQueueHob->Type, PETQueueHob->Value, 0, NULL, NULL);

    ///
    /// Mark it as sent
    ///
    PETQueueHob->Type = (UINT32) -1;

    ///
    /// Need find next one
    ///
    Hob.Raw     = (VOID *) PETQueueHob;
    PETQueueHob = (AMT_PET_QUEUE_HOB *) GET_NEXT_HOB (Hob);
  }
  ///
  /// Send DXEQueue
  ///
  Link = (EFI_LIST_ENTRY *) GetFirstNode ((LIST_ENTRY *) &gAmtPetQueueProtocol->MessageList);

  while (!IsNull ((LIST_ENTRY *) &gAmtPetQueueProtocol->MessageList, (LIST_ENTRY *) Link)) {
    Node = AMT_PET_QUEUE_NODE_FROM_LINK (Link);

    ///
    /// Send message
    ///
    AmtReportStatusCode (Node->Type, Node->Value, 0, NULL, NULL);

    ///
    /// Mark it as sent
    ///
    Node->Type = (UINT32) -1;

    Link = (EFI_LIST_ENTRY *) GetNextNode (
                                (LIST_ENTRY *) &gAmtPetQueueProtocol->MessageList,
                                (LIST_ENTRY *) &Node->Link
                                );
  }

  return EFI_SUCCESS;
}

/**
  This routine creats PET MessageQueue.

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
AmtCreateMessageQueue (
  VOID
  )
{
  ///
  /// Create Queue for later usage
  ///
  gAmtPetQueueProtocol = &gAmtPetQueue;

  InitializeListHead ((LIST_ENTRY *) &gAmtPetQueueProtocol->MessageList);

  return EFI_SUCCESS;
}

/**
  This routine saves current ForcePush ErrorEvent to Variable, which will be sent again.

  @param[in] MessageType          ASF PET message type.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures
**/
EFI_STATUS
SaveForcePushErrorEvent (
  IN  ASF_FRAMEWORK_MESSAGE_TYPE  MessageType
  )
{
  EFI_STATUS                      Status;
  UINTN                           Size;
  ASF_FRAMEWORK_MESSAGE_TYPE      *Message;
  UINT32                          VarAttributes;
  ///
  /// Create PET queue variable
  ///
  Message = NULL;
  Size    = 0;
  Status = gRT->GetVariable (
                  AMT_FORCE_PUSH_PET_VARIABLE_NAME,
                  &gAmtForcePushPetVariableGuid,
                  &VarAttributes,
                  &Size,
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    ///
    /// Get the exist message
    ///
    Message = AllocateZeroPool (Size + sizeof (MessageType));
    ASSERT (Message != NULL);
    if (Message == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = gRT->GetVariable (
                    AMT_FORCE_PUSH_PET_VARIABLE_NAME,
                    &gAmtForcePushPetVariableGuid,
                    &VarAttributes,
                    &Size,
                    Message
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// Fill new item
    ///
    *(ASF_FRAMEWORK_MESSAGE_TYPE *) ((UINTN) Message + Size) = MessageType;
    Size += sizeof (MessageType);
  } else if (Status == EFI_NOT_FOUND) {
    ///
    /// Create a new one
    ///
    Size    = sizeof (MessageType);
    Message = AllocateZeroPool (sizeof (MessageType));
    ASSERT (Message != NULL);
    if (Message == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    VarAttributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
    *Message = MessageType;
  } else {
    ASSERT (FALSE);
  }
  ///
  /// Set PET message to variable
  ///
  if (Message != NULL) {
    Status = gRT->SetVariable (
                    AMT_FORCE_PUSH_PET_VARIABLE_NAME,
                    &gAmtForcePushPetVariableGuid,
                    VarAttributes,
                    Size,
                    Message
                    );
    ASSERT_EFI_ERROR (Status);

    FreePool (Message);
  }

  return EFI_SUCCESS;
}

/**
  This routine converts Hob ForcePush ErrorEvent to Variable, which will be sent again.

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SaveForcePushErrorEventFromPeiToDxe (
  VOID
  )
{
  AMT_FORCE_PUSH_PET_HOB  *AmtForcePushPETHob;
  EFI_STATUS              Status;
  EFI_PEI_HOB_POINTERS    Hob;

  ///
  /// Find ASF ForcePush PET Hob
  ///
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &AmtForcePushPETHob);

  while (TRUE) {
    AmtForcePushPETHob = GetNextGuidHob (&gAmtForcePushPetHobGuid, AmtForcePushPETHob);
    if (AmtForcePushPETHob == NULL) {
      break;
    }

    SaveForcePushErrorEvent (AmtForcePushPETHob->MessageType);

    ///
    /// Need find next one
    ///
    Hob.Raw             = (VOID *) AmtForcePushPETHob;
    AmtForcePushPETHob  = (AMT_FORCE_PUSH_PET_HOB *) GET_NEXT_HOB (Hob);
  }

  return EFI_SUCCESS;
}

/**
  This routine tries to send all ForcePush ErrorEvent.
  If message is sent, it will be deleted from Variable.
  If message is not sent, it will be still stored to Variable.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures
**/
EFI_STATUS
SendAllForcePushErrorEvent (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           Size;
  ASF_FRAMEWORK_MESSAGE_TYPE      *Message;
  UINTN                           Index;
  ASF_FRAMEWORK_MESSAGE_TYPE      *NewMessage;
  UINTN                           NewIndex;
  UINT32                          VarAttributes;

  ///
  /// Create PET queue variable
  ///
  Message = NULL;
  Size    = 0;
  Status = gRT->GetVariable (
                  AMT_FORCE_PUSH_PET_VARIABLE_NAME,
                  &gAmtForcePushPetVariableGuid,
                  &VarAttributes,
                  &Size,
                  NULL
                  );
  if (Status == EFI_NOT_FOUND) {
    return EFI_SUCCESS;
  }

  if (Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }
  ///
  /// Get the exist message
  ///
  Message = AllocateZeroPool (Size);
  ASSERT (Message != NULL);
  if (Message == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  NewMessage = AllocateZeroPool (Size);
  ASSERT (NewMessage != NULL);
  if (NewMessage == NULL) {
    FreePool (Message);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = gRT->GetVariable (
                  AMT_FORCE_PUSH_PET_VARIABLE_NAME,
                  &gAmtForcePushPetVariableGuid,
                  &VarAttributes,
                  &Size,
                  Message
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    FreePool (Message);
    FreePool (NewMessage);
    return EFI_DEVICE_ERROR;
  }

  NewIndex = 0;
  for (Index = 0; Index < Size / sizeof (ASF_FRAMEWORK_MESSAGE_TYPE); Index++) {
    Status = SendPostPacket (Message[Index]);
    if (EFI_ERROR (Status)) {
      ///
      /// Fail, save it again.
      ///
      NewMessage[NewIndex] = Message[Index];
      NewIndex++;
    }
  }

  FreePool (Message);

  ///
  /// SetVariable again
  ///
  if (NewIndex == 0) {
    FreePool (NewMessage);
    NewMessage = NULL;
  }

  Status = gRT->SetVariable (
                  AMT_FORCE_PUSH_PET_VARIABLE_NAME,
                  &gAmtForcePushPetVariableGuid,
                  VarAttributes,
                  NewIndex * sizeof (ASF_FRAMEWORK_MESSAGE_TYPE),
                  NewMessage
                  );

  if (NewMessage != NULL) {
    FreePool (NewMessage);
  }

  return EFI_SUCCESS;
}
