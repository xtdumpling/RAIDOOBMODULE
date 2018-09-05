/** @file
  Include file for ASF Driver.

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
#ifndef _ALERT_STANDARD_FORMAT_DXE_H
#define _ALERT_STANDARD_FORMAT_DXE_H

//
// Driver Consumed Protocol Prototypes
//
#include <Protocol/DataHub.h>
#include <Guid/ConsoleOutDevice.h>
#include <Guid/HobList.h>
#include <Guid/DataHubStatusCodeRecord.h>
#include <Library/HobLib.h>
#include <PiPei.h>
#include <AlertStandardFormat.h>
#include <AmtForcePushPetPolicy.h>
#include <AmtForcePushPetHob.h>
#include <AmtPetQueueHob.h>
#include <Library/AslUpdateLib.h>
#include <Library/DxeAmtLib.h>

#include "Library/AmtLib.h"

#include <Ppi/AmtStatusCode.h>
#include <IndustryStandard/AlertStandardFormatTable.h>
#include <Protocol/AlertStandardFormat.h>

extern EFI_GUID gConsoleLockGuid;
extern EFI_GUID gAmtForcePushPetVariableGuid;

#define EFI_IOB_PCI_PC_BUS_ENUM   (EFI_SUBCLASS_SPECIFIC | 0x00000000)

#define LOCK_CONSOLE    1
#define NO_LOCK_CONSOLE 0

#define CONSOLE_LOCK_VARIABLE_NAME        L"ConsoleLock"
#define AMT_FORCE_PUSH_PET_VARIABLE_NAME  L"AmtForcePushErrorPET"

///
/// ASF Over HECI
///
typedef struct {
  UINT8           Command;
  UINT8           ByteCount;
  ASF_MESSAGE     AsfMessage;
  UINT8           EventData[6];
} HECI_ASF_PUSH_PROGRESS_CODE;

#define HECI_ASF_PUSH_PROGRESS_CODE_LENGTH  0x15

typedef struct {
  UINT8 Command;
  UINT8 ByteCount;
  UINT8 SubCommand;
  UINT8 VersionNumber;
} HECI_ASF_GET_BOOT_OPTIONS;

#define HECI_ASF_GET_BOOT_OPTIONS_LENGTH  0x04

typedef struct {
  UINT8                 Command;
  UINT8                 ByteCount;
  ASF_BOOT_OPTIONS      AsfBootOptions;
  UINT8                 Reserved;
} HECI_ASF_GET_BOOT_OPTIONS_RESPONSE;

#define HECI_ASF_GET_BOOT_OPTIONS_RESPONSE_LENGTH 0x0F

typedef struct {
  UINT8                       Command;
  UINT8                       ByteCount;
  ASF_CLEAR_BOOT_OPTIONS      AsfClearBootOptions;
} HECI_ASF_CLEAR_BOOT_OPTION;

#define HECI_ASF_CLEAR_BOOT_OPTION_LENGTH 0x04

///
/// Defines for Command
///
#define ASF_MESSAGING_CMD              0x04
#define ASF_PUSH_PROGESS_CODE_SUBCMD   0x12
#define ASF_MANAGEMENT_CONTROL         0x02
#define ASF_WDT_START_SUBCMD           0x13
#define ASF_WDT_STOP_SUBCMD            0x14
#define ASF_CONFIGURATION_CMD          0x03
#define ASF_CLEAR_BOOT_OPTION_SUBCMD   0x15
#define ASF_RETURN_BOOT_OPTION_SUBCMD  0x16
#define ASF_NO_BOOT_OPTION_SUBCMD      0x17

#define ALERT_STANDARD_FORMAT_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('a', 's', 'f', 'd')

///
/// Declare a local instance structure for this driver
///
typedef struct _ALERT_STANDARD_FORMAT_INSTANCE {
  UINTN                           Signature;
  EFI_HANDLE                      Handle;

  ///
  /// Published interface
  ///
  ALERT_STANDARD_FORMAT_PROTOCOL      AlertStandardFormat;

} ALERT_STANDARD_FORMAT_INSTANCE;

#define AMT_PET_QUEUE_NODE_SIGNATURE      SIGNATURE_32 ('A', 'M', 'T', 'Q')

typedef struct _AMT_PET_QUEUE_NODE {
  UINT32                Signature;
  EFI_LIST_ENTRY        Link;
  EFI_STATUS_CODE_VALUE Value;
  EFI_STATUS_CODE_TYPE  Type;
} AMT_PET_QUEUE_NODE;

#define AMT_PET_QUEUE_NODE_FROM_LINK(_node) CR (_node, AMT_PET_QUEUE_NODE, Link, AMT_PET_QUEUE_NODE_SIGNATURE)

typedef struct {
  EFI_LIST_ENTRY  MessageList;
} AMT_PET_QUEUE_PROTOCOL;

//
// Prototypes
//

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
;

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
;

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
;

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
;

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
;

/**
  This routine returns ForcePushPetPolicy information.


  @retval AMT_FORCE_PUSH_PET_POLICY_HOB   ForcePushPetPolicy information.
**/
AMT_FORCE_PUSH_PET_POLICY_HOB *
GetForcePushPetPolicy (
  VOID
  )
;

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
;

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
;

/**
  This routine sends PET message in MessageQueue.

  @param[in] PeiServices          PeiServices pointer.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_NOT_READY           No controller
**/
EFI_STATUS
SendPETMessageInQueue (
  VOID
  )
;

/**
  This routine creats PET MessageQueue.


  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
AmtCreateMessageQueue (
  VOID
  )
;

/**
  This routine saves current ForcePush ErrorEvent to Variable, which will be sent again.

  @param[in] MessageType          ASF PET message type.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures
**/
EFI_STATUS
SaveForcePushErrorEvent (
  IN  ASF_FRAMEWORK_MESSAGE_TYPE      MessageType
  )
;

/**
  This routine converts Hob ForcePush ErrorEvent to Variable, which will be sent again.


  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SaveForcePushErrorEventFromPeiToDxe (
  VOID
  )
;
#endif
