/**@file

@copyright
 Copyright (c) 2011 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef __AMT_PET_ALERT_H__
#define __AMT_PET_ALERT_H__

#include <Guid/GlobalVariable.h>
#include <Library/DxeAmtLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/HeciProtocol.h>
#include <MkhiMsgs.h>
#include <AlertStandardFormat.h>
#include <Protocol/IdeControllerInit.h>
#include <Library/UefiBootManagerLib.h>


// BAE device type defines
#define BAE_NETWORK_DEVICE    0x1
#define BAE_HDD_DEVICE        0x2
#define BAE_REMOVABLE_DEVICE  0x3
#define BAE_EMPTY_QUEUE       0x7F

#define MODELSERIALNUMBER_STR_SIZE  64

#pragma pack(push,1)


// Example 1: 16 10 0F 6F 02 68 08 FF FF 00 00 40 13 XX XX XX
// Example 2: 15 10 0F 6F 02 68 10 FF FF 22 00 AA 13 03 03 02

typedef struct {
  UINT8 SubCommand;      // 0x00
  UINT8 VersionNumber;   // 0x01
  UINT8 EventSensorType; // 0x02
  UINT8 EventType;       // 0x03
  UINT8 EventOffset;     // 0x04
  UINT8 EventSourceType; // 0x05
  UINT8 EventSeverity;   // 0x06
  UINT8 SensorDevice;    // 0x07
  UINT8 Sensornumber;    // 0x08
  UINT8 Entity;          // 0x09
  UINT8 EntityInstance;  // 0x0A
  UINT8 EventData1;      // 0x0B
  UINT8 EventData2;      // 0x0C
  UINT8 EventData3;      // 0x0D
  UINT8 EventData4;      // 0x0E
  UINT8 EventData5;      // 0x0F
} BOOT_AUDIT_ENTRY;

typedef struct {
  UINT8            Command;
  UINT8            ByteCount;
  BOOT_AUDIT_ENTRY Data;
} BOOT_AUDIT_ENTRY_PACK;

typedef struct {  
  UINT8 BootQueue[3];
  UINT8 BootHarddriveTag[64];
} PET_ALERT_CFG;

#pragma pack(pop)

EFI_STATUS
AmtPetAlertReadyToBoot (
  EFI_EVENT Event,
  VOID      *ParentImageHandle
);

EFI_STATUS
AmtPetAlertLegacyBoot (
  EFI_EVENT Event,
  VOID      *ParentImageHandle
);

#endif
