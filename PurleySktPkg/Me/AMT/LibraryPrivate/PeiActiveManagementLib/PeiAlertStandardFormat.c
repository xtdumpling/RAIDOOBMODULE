/** @file
  Processes ASF messages.

@copyright
 Copyright (c) 2010 - 2016 Intel Corporation. All rights reserved
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

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Pi/PiStatusCode.h>
#include "PeiAlertStandardFormat.h"


static AMT_STATUS_CODE_PPI      mAmtStatusCodePpi = { PeiAmtReportStatusCode };

static EFI_PEI_PPI_DESCRIPTOR   mInstallAmtStatusCodePpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gAmtStatusCodePpiGuid,
  &mAmtStatusCodePpi
};

/**
  Perform AMT PET message sending

  @retval EFI_SUCCESS             if the AMT StatusCode PPI is successfully installed.
  @exception EFI_UNSUPPORTED      ASF is not enabled or ManageabilityMode is zero.
**/
EFI_STATUS
AlertStandardFormatInit (
  VOID
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "AlertStandardFormatInit () - Start\n"));

  Status = EFI_SUCCESS;

  ///
  /// First check if ASF support is enabled in Setup.
  ///
  if (!PeiAsfSupported ()) {
    DEBUG ((DEBUG_INFO, "ASF is not supported, return directly!\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// Sending ASF Messaging if ManageabilityMode is not zero
  ///
  if (ManageabilityModeSetting () == MNT_OFF) {
    DEBUG ((DEBUG_INFO, "ManageabilityMode is not supported, return directly!\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// Install AMT report status code PPI
  ///
  Status = PeiServicesInstallPpi (&mInstallAmtStatusCodePpi);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Try to send PET message
  ///
  SendPETMessageInQueue ();

  DEBUG ((DEBUG_INFO, "AlertStandardFormatInit () - End\n"));
  return Status;
}

/**
  Send ASF Message.

  @param[in] AsfMessage           Pointer to ASF message

  @retval EFI_SUCCESS             Boot options copied
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_NOT_READY           No controller
  @retval EFI_DEVICE_ERROR        The function should not be completed due to a device error
**/
EFI_STATUS
SendAsfMessage (
  IN  ASF_MESSAGE                 *AsfMessage
  )
{
  EFI_STATUS                      Status;
  HECI_PPI                        *Heci;
  UINT32                          HeciLength;
  HECI_ASF_PUSH_PROGRESS_CODE     HeciAsfPushProgressCode;
  UINT32                          MeStatus;

  Status = PeiServicesLocatePpi (
                            &gHeciPpiGuid,    // GUID
                            0,                // INSTANCE
                            NULL,             // EFI_PEI_PPI_DESCRIPTOR
                            (VOID **) &Heci   // PPI
                            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Only send ASF Push Progress code when ME is ready.  Ignore FW Init Status.
  ///
  if (ME_STATUS_ME_STATE_ONLY (MeStatus) != ME_READY) {
    return EFI_NOT_READY;
  }

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
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  This routine checks whether current message is ForcePush message.

  @param[in] MessageType          AMT PET Message Type.

  @retval TRUE                    It is ForcePush message.
  @retval FALSE                   It is not ForcePush message.
**/
BOOLEAN
IsForcePushErrorEvent (
  IN  ASF_FRAMEWORK_MESSAGE_TYPE  MessageType
  )
{
  AMT_FORCE_PUSH_PET_POLICY_HOB *AmtForcePushPETPolicyHob;
  UINTN                         Index;
  UINTN                         Number;
  EFI_STATUS                    Status;

  Status = PeiServicesGetHobList ((VOID **) &AmtForcePushPETPolicyHob);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  AmtForcePushPETPolicyHob = GetNextGuidHob (&gAmtForcePushPetPolicyGuid, AmtForcePushPETPolicyHob);
  if (AmtForcePushPETPolicyHob == NULL) {
    return FALSE;
  }

  Number = (AmtForcePushPETPolicyHob->EfiHobGuidType.Header.HobLength - sizeof (EFI_HOB_GUID_TYPE)) /
  sizeof (ASF_FRAMEWORK_MESSAGE_TYPE);
  for (Index = 0; Index < Number; Index++) {
    if (AmtForcePushPETPolicyHob->MessageType[Index] == MessageType) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Provides an interface that a software module can call to report an ASF PEI status code.

  @param[in] This                 This interface.
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
PeiAmtReportStatusCode (
  IN  AMT_STATUS_CODE_PPI         * This,
  IN  EFI_STATUS_CODE_TYPE        Type,
  IN  EFI_STATUS_CODE_VALUE       Value,
  IN  UINT32                      Instance,
  IN  EFI_GUID                    * CallerId OPTIONAL,
  IN  EFI_STATUS_CODE_DATA        * Data OPTIONAL
  )
{
  UINTN       Index;
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  if (PeiFwProgressSupport ()) {
    if ((Type & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
      for (Index = 0; Index < AsfProgressDataHubMapSize / sizeof (ASF_DATA_HUB_MAP); Index++) {
        if (mAsfProgressDataHubMap[Index].StatusCodeValue == Value) {
          ///
          /// Queue Progress Code and send PET after checking Boot Options
          ///
          QueuePetMessage (Type, Value);
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

        if (Status == EFI_NOT_READY) {
          QueuePetMessage (Type, Value);
        }
      }
    }
  }

  return Status;
}

/**
  Sends a POST packet across ASF

  @param[in] MessageType          POST Status Code

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SendPostPacket (
  IN  ASF_FRAMEWORK_MESSAGE_TYPE  MessageType
  )
{
  UINTN Index;

  ///
  /// Find the message to send across the wire
  ///
  for (Index = 0; Index < AsfFrameworkMessageSize / sizeof (ASF_FRAMEWORK_MESSAGE); Index++) {
    if (mAsfFrameworkMessage[Index].MessageType == MessageType) {
      return SendAsfMessage ( &mAsfFrameworkMessage[Index].Message);
    }
  }

  return EFI_SUCCESS;
}

/**
  This routine saves current ForcePush ErrorEvent to Hob, which will be sent again.

  @param[in] MessageType          ASF PET message type.

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SaveForcePushErrorEvent (
  IN  ASF_FRAMEWORK_MESSAGE_TYPE  MessageType
  )
{
  AMT_FORCE_PUSH_PET_HOB          *ForcePushPETHob;
  EFI_STATUS                      Status;

  ///
  /// Create PET queue hob
  ///
  Status = PeiServicesCreateHob (
                            EFI_HOB_TYPE_GUID_EXTENSION,
                            sizeof (AMT_FORCE_PUSH_PET_HOB),
                            (VOID **) &ForcePushPETHob
                            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ForcePushPETHob->EfiHobGuidType.Name  = gAmtForcePushPetHobGuid;
  ForcePushPETHob->MessageType          = MessageType;

  return EFI_SUCCESS;
}

/**
  This routine puts PET message to MessageQueue, which will be sent later.

  @param[in] Type                 StatusCode message type.
  @param[in] Value                StatusCode message value.

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
QueuePetMessage (
  IN  EFI_STATUS_CODE_TYPE        Type,
  IN  EFI_STATUS_CODE_VALUE       Value
  )
{
  AMT_PET_QUEUE_HOB *PETQueueHob;
  EFI_STATUS        Status;

  ///
  /// Create PET queue hob
  ///
  Status = PeiServicesCreateHob (
                            EFI_HOB_TYPE_GUID_EXTENSION,
                            sizeof (AMT_PET_QUEUE_HOB),
                            (VOID **) &PETQueueHob
                            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PETQueueHob->EfiHobGuidType.Name  = gAmtPetQueueHobGuid;
  PETQueueHob->Value                = Value;

  return EFI_SUCCESS;
}

/**
  This routine sends PET message in MessageQueue.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_NOT_READY           No controller
**/
EFI_STATUS
SendPETMessageInQueue (
  VOID
  )
{
  EFI_STATUS                  Status;
  AMT_PET_QUEUE_HOB           *PETQueueHob;
  EFI_PEI_HOB_POINTERS        Hob;

  HECI_PPI                    *Heci;
  UINT32                      MeStatus;

  ///
  /// Try HECI state
  ///
  Status = PeiServicesLocatePpi (
                            &gHeciPpiGuid,    // GUID
                            0,                // INSTANCE
                            NULL,             // EFI_PEI_PPI_DESCRIPTOR
                            (VOID **) &Heci   // PPI
                            );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = Heci->GetMeStatus (&MeStatus);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ///
  /// Only send ASF Push Progress code when ME is ready.  Ignore FW Init Status.
  ///
  if (ME_STATUS_ME_STATE_ONLY (MeStatus) != ME_READY) {
    return EFI_NOT_READY;
  }
  ///
  /// Get PETQueueHob
  ///
  Status = PeiServicesGetHobList ((VOID **) &PETQueueHob);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  while (TRUE) {
    PETQueueHob = GetNextGuidHob (&gAmtPetQueueHobGuid, PETQueueHob);
    if (PETQueueHob == NULL) {
      break;
    }
    ///
    /// Send message
    ///
    PeiAmtReportStatusCode (NULL, PETQueueHob->Type, PETQueueHob->Value, 0, NULL, NULL);

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

  return EFI_SUCCESS;
}
