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
#ifndef _PEI_ALERT_STANDARD_FORMAT_H
#define _PEI_ALERT_STANDARD_FORMAT_H

#include <Library/PeiAmtLib.h>
#include <Library/PeiMeLib.h>
#include <MkhiMsgs.h>
#include <AlertStandardFormat.h>
//
// Driver Consumed Protocol Prototypes
//
#include <Ppi/HeciPpi.h>
#include <Ppi/AmtStatusCode.h>
#include <MeBiosExtensionSetup.h>
#include <AmtForcePushPetPolicy.h>
#include <AmtForcePushPetHob.h>
#include <AmtPetQueueHob.h>
#include <IncludePrivate/Library/AlertStandardFormatLib.h>

typedef struct {
  UINT8           Command;
  UINT8           ByteCount;
  ASF_MESSAGE     AsfMessage;
  UINT8           EventData[6];
} HECI_ASF_PUSH_PROGRESS_CODE;

#define HECI_ASF_PUSH_PROGRESS_CODE_LENGTH  0x15

///
/// Defines for Command
///
#define  ASF_MESSAGING_CMD              0x04
#define  ASF_PUSH_PROGESS_CODE_SUBCMD   0x12
#define  ASF_MANAGEMENT_CONTROL         0x02
#define  ASF_WDT_START_SUBCMD           0x13
#define  ASF_WDT_STOP_SUBCMD            0x14
#define  ASF_CONFIGURATION_CMD          0x03
#define  ASF_CLEAR_BOOT_OPTION_SUBCMD   0x15
#define  ASF_RETURN_BOOT_OPTION_SUBCMD  0x16
#define  ASF_NO_BOOT_OPTION_SUBCMD      0x17


//
// Prototypes
//

/**
  Perform AMT PET message sending

  @retval EFI_SUCCESS             if the AMT StatusCode PPI is successfully installed.
  @exception EFI_UNSUPPORTED      ASF is not enabled or ManageabilityMode is zero.
**/
EFI_STATUS
AlertStandardFormatInit (
  VOID
  );

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
  );

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
  );

/**
  Sends a POST packet across ASF

  @param[in] MessageType          POST Status Code

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SendPostPacket (
  IN  ASF_FRAMEWORK_MESSAGE_TYPE  MessageType
  );

/**
  This routine saves current ForcePush ErrorEvent to Hob, which will be sent again.

  @param[in] MessageType          ASF PET message type.

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SaveForcePushErrorEvent (
  IN  ASF_FRAMEWORK_MESSAGE_TYPE  MessageType
  );

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
  );

/**
  This routine sends PET message in MessageQueue.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_NOT_READY           No controller
**/
EFI_STATUS
SendPETMessageInQueue (
  VOID
  );
#endif
