/**@file

@copyright
 Copyright (c) 2011 - 2017 Intel Corporation. All rights reserved
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
#include "AmtPetAlert.h"
#include <Protocol/MeSpsPolicyProtocol.h>
#include <Protocol/BlockIo.h>
#include "Library/MeTypeLib.h"
#include "Library/AmtSupportLib.h"
// APTIOV_SERVER_OVERRIDE_RC_START
#include <Uefi.h>
#include <Pi/PiStatusCode.h>
#include <Library/BaseLib.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/AtaPassThru.h>
// APTIOV_SERVER_OVERRIDE_RC_END

#define PLATFORM_GLOBAL_VARIABLE \
  { \
    0xa5dabcf0, 0xc7a1, 0x4370, { 0x8c, 0x21, 0xfa, 0x50, 0xd3, 0x22, 0xa4, 0x3e } \
  }
EFI_GUID gPlatformGlobalVariableGuid = PLATFORM_GLOBAL_VARIABLE;


// APTIOV_SERVER_OVERRIDE_RC_START
extern EFI_GUID                        gEfiBlockIoProtocolGuid;

/**
  This function converts an input device structure to a Unicode string.

  @param DevPath                  A pointer to the device path structure.

  @return A new allocated Unicode string that represents the device path.
**/
CHAR16 *
DevicePathToStr (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  )
{
  EFI_STATUS                       Status;
  CHAR16                           *ToText;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;

  if (DevPath == NULL) {
    return NULL;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathToTextProtocolGuid,
                  NULL,
                  (VOID **) &DevPathToText
                  );
  ASSERT_EFI_ERROR (Status);
  ToText = DevPathToText->ConvertDevicePathToText (
                            DevPath,
                            FALSE,
                            TRUE
                            );
  ASSERT (ToText != NULL);
  return ToText;
}
// APTIOV_SERVER_OVERRIDE_RC_END

/**
  Check whether there is a instance in BlockIoDevicePath, which contain multi device path
  instances, has the same partition node with HardDriveDevicePath device path

  @param  BlockIoDevicePath      Multi device path instances which need to check
  @param  HardDriveDevicePath    A device path which starts with a hard drive media
                                 device path.

  @retval TRUE                   There is a matched device path instance.
  @retval FALSE                  There is no matched device path instance.

**/
BOOLEAN
DevPathContainsPartion (
  IN  HARDDRIVE_DEVICE_PATH           *ParitionDevPath,
  IN  EFI_DEVICE_PATH_PROTOCOL        *FullDevPath
  )
{
  HARDDRIVE_DEVICE_PATH               *PartitionToCompare;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  BOOLEAN                              Match;

  DEBUG ((DEBUG_INFO , "Device Path Contains Partition\n"));

  if ((FullDevPath == NULL) || (ParitionDevPath == NULL)) {
    return FALSE;
  }

  //
  // find the partition device path node
  //
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL*)FullDevPath;
  PartitionToCompare = NULL;
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP)) {

      PartitionToCompare = (HARDDRIVE_DEVICE_PATH *) DevicePath;
      break;
    }
    DevicePath = NextDevicePathNode (DevicePath);
  }

  if (PartitionToCompare == NULL) {
    return FALSE;
  }

  DEBUG_CODE(
      CHAR16                          *PartitionToCompareStr;
      CHAR16                          *PartitionStr;
      PartitionToCompareStr = DevicePathToStr ((EFI_DEVICE_PATH_PROTOCOL*)PartitionToCompare);
      PartitionStr = DevicePathToStr ((EFI_DEVICE_PATH_PROTOCOL*)ParitionDevPath);
      DEBUG ((DEBUG_INFO, "\t\tpartition         : \"%s\"\n\t\tpartitionToCompare: \"%s\"\n", PartitionStr, PartitionToCompareStr));
      FreePool (PartitionStr);
      FreePool (PartitionToCompareStr);
  );

  //
  // Check for the match
  //
  Match = FALSE;

  if ((PartitionToCompare->MBRType == ParitionDevPath->MBRType) &&
      (PartitionToCompare->SignatureType == ParitionDevPath->SignatureType)) {

    switch (PartitionToCompare->SignatureType) {
    case SIGNATURE_TYPE_GUID:
      Match = CompareGuid (
                (EFI_GUID *)PartitionToCompare->Signature,
                (EFI_GUID *)ParitionDevPath->Signature
              );

      DEBUG ((
          DEBUG_INFO, "GUID Signature\n\t\tpartition          %016X%016X\n"
              "\t\tpartitionToCompare: %016X%016X\n",
          (UINT64*) ((VOID*)&ParitionDevPath->Signature[0]),
          (UINT64*) ((VOID*)&ParitionDevPath->Signature[1]),
          (UINT64*) ((VOID*)&PartitionToCompare->Signature[0]),
          (UINT64*) ((VOID*)&PartitionToCompare->Signature[1])
      ));
      break;

    case SIGNATURE_TYPE_MBR:
      Match = (
                (BOOLEAN)(*((UINT32 *)(&(PartitionToCompare->Signature[0]))) ==
                ReadUnaligned32((UINT32 *)(&(ParitionDevPath->Signature[0]))))
              );

      DEBUG ((
          DEBUG_INFO,
          "MBR signature type:\n\tpartition          %08X\n\tpartitionToCompare %08X\n",
          ReadUnaligned32((UINT32 *)(&(ParitionDevPath->Signature[0]))),
          *((UINT32 *)(&(PartitionToCompare->Signature[0])))
      ));
      break;

    default:
      Match = FALSE;
      break;
    }
  }

  if (Match) {
    DEBUG ((DEBUG_INFO, "Partition is contained inside dev path.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "Partition is not contained inside dev path.\n"));
  }

  return Match;
}

/**
  Tries to expand the given partitions device path to start with physical HDD
  device path
  @param[in]       LoadOption          pointer to an Efi Boot Manager boot option
  @param[out]      BootDevicePath      indirect pointer to retrieved device path

  @retval          EFI_SUCCESS         Function found
**/
EFI_STATUS
ExpandPartitionsDevPath (
  IN EFI_BOOT_MANAGER_LOAD_OPTION     *LoadOption,
  OUT EFI_DEVICE_PATH_PROTOCOL       **BootDevicePath
  )
{
  EFI_STATUS                           Status;
  UINTN                                BlockIoHandleCount;
  EFI_HANDLE                          *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL            *PartitionDevPath;
  EFI_DEVICE_PATH                     *BlockIoDevicePath;
  UINT32                               i;
  DEBUG ((DEBUG_INFO, "Expand partitions device path\n"));

  *BootDevicePath = NULL;
  PartitionDevPath = LoadOption->FilePath;

  DEBUG_CODE (
    CHAR16                            *DevPathStr;
    DevPathStr = DevicePathToStr(PartitionDevPath);
    DEBUG ((DEBUG_INFO, "Partition dev path : %s\n", DevPathStr));
    FreePool (DevPathStr);
  );

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status) || BlockIoHandleCount == 0 || BlockIoBuffer == NULL) {
    return EFI_NOT_FOUND;
  }

  for (i=0; i < BlockIoHandleCount ; i++) {

    Status = gBS->HandleProtocol (BlockIoBuffer[i], &gEfiDevicePathProtocolGuid, (VOID *) &BlockIoDevicePath);
    if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
      continue;
    }

    DEBUG_CODE (
      CHAR16                          *DevPathStr;
      DevPathStr = DevicePathToStr(BlockIoDevicePath);
      DEBUG ((DEBUG_INFO, "Checking dev path : %s\n", DevPathStr));
      FreePool (DevPathStr);
    );

    if (DevPathContainsPartion((HARDDRIVE_DEVICE_PATH*) PartitionDevPath, BlockIoDevicePath)) {
      *BootDevicePath = DuplicateDevicePath(BlockIoDevicePath);
      break;
    }
  }

  if (*BootDevicePath == NULL) {
    Status = EFI_NOT_FOUND;
  }
  if (BlockIoBuffer != NULL) {
    FreePool (BlockIoBuffer);
  }
  return Status;
}

/**
  Checks whenever a given load option starts with a partition device path

  @param[in]       LoadOption          Pointer to load option to test

  @retval          TRUE
  @retval          FALSE
**/
BOOLEAN
IsItPartitionsDevicePath(
    IN EFI_BOOT_MANAGER_LOAD_OPTION     *LoadOption
  )
{
  return  DevicePathType (LoadOption->FilePath) == MEDIA_DEVICE_PATH &&
          (DevicePathSubType (LoadOption->FilePath) == MEDIA_HARDDRIVE_DP);
}


EFI_STATUS
EFIAPI
InitializeAmtPetAlert (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   ReadyToBootEvent;

#if defined(SPS_SUPPORT) && SPS_SUPPORT
  if (!MeTypeIsAmt()) {
    return EFI_UNSUPPORTED;
  }
#endif // SPS_SUPPORT

  //
  // Register Ready to Boot Event for AMT Wrapper
  //
  Status = EfiCreateEventReadyToBootEx (
            TPL_CALLBACK,
            AmtPetAlertReadyToBoot,
            (VOID *) &ImageHandle,
            &ReadyToBootEvent
            );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

VOID SwapEntries (
  IN CHAR8    *Data,
  IN UINT16   Size
)
{
  UINT16  Index;
  CHAR8   Temp8;

  for (Index = 0; (Index+1) < Size; Index+=2) {
    Temp8           = Data[Index];
    Data[Index]     = Data[Index + 1];
    Data[Index + 1] = Temp8;
  }
}

// Example 1: 16 10 0F 6F 02 68 08 FF FF 22 00 40 13 XX XX XX
// Example 2: 15 10 0F 6F 02 68 10 FF FF 22 00 AA 13 03 03 02
BOOT_AUDIT_ENTRY mPetAlertWithoutChange = {
    0x16, 0x10, 0x0F, 0x6F, 0x02, 0x68, 0x08, 0xFF, 0xFF, 0x22, 0x00, 0x40, 0x13, 0x00, 0x00, 0x00
};

BOOT_AUDIT_ENTRY mPetAlertWithChange = {
    0x15, 0x10, 0x0F, 0x6F, 0x02, 0x68, 0x10, 0xFF, 0xFF, 0x22, 0x00, 0xAA, 0x13, 0x03, 0x03, 0x02
};

EFI_STATUS
SendBaeMessage(
  BOOT_AUDIT_ENTRY                *Buffer
)
{
  HECI_PROTOCOL                   *Heci;
  UINT32                          HeciLength;
  BOOT_AUDIT_ENTRY_PACK           Pack;
  UINT32                          MeStatus;
  EFI_STATUS                      TempStatus;
  EFI_STATUS                      Status;

  CopyMem(&Pack.Data, Buffer, sizeof(BOOT_AUDIT_ENTRY));

  Pack.Command                = ASF_MESSAGE_COMMAND_MESSAGE;  // 0x04
  Pack.ByteCount              = sizeof(BOOT_AUDIT_ENTRY);
  HeciLength                  = sizeof(BOOT_AUDIT_ENTRY_PACK);    // length include Command & ByteCount

  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  &Heci
                  );
  if (!EFI_ERROR(Status)) {
    TempStatus = Heci->GetMeStatus(&MeStatus);
    ASSERT_EFI_ERROR(TempStatus);

    //
    // Only send ASF Push Progress code when ME is ready.  Ignore FW Init Status.
    //
    if (ME_STATUS_ME_STATE_ONLY(MeStatus) == ME_READY) {
      Status = Heci->SendMsg(
                      HECI1_DEVICE,
                      (UINT32*)&Pack,
                      HeciLength,
                      BIOS_ASF_HOST_ADDR,
                      HECI_ASF_MESSAGE_ADDR
                      );
    }
  }

  return Status;
}

EFI_STATUS
PetAlert(
  UINT8                   DeviceType,
  CHAR8                   *HarddriveTag
)
{
  PET_ALERT_CFG           PetAlertCfgVar;
  UINTN                   VarSize;
  UINTN                   UpdateFlag;
  EFI_STATUS              Status;
  BOOT_AUDIT_ENTRY        *PetAlertMsg;

  UpdateFlag  = 0;
  PetAlertMsg = &mPetAlertWithoutChange;

  if (HarddriveTag == NULL) {
    HarddriveTag = "";
  }

  VarSize = sizeof(PetAlertCfgVar);
  Status = gRT->GetVariable (
                  L"PetAlertCfg",
                  &gPlatformGlobalVariableGuid,
                  NULL,
                  &VarSize,
                  &PetAlertCfgVar
                 );
  if (EFI_ERROR (Status)) {
    UpdateFlag = 1;
    PetAlertCfgVar.BootQueue[0] = DeviceType;
    PetAlertCfgVar.BootQueue[1] = DeviceType;
    PetAlertCfgVar.BootQueue[2] = DeviceType;
    Status = AsciiStrCpyS(PetAlertCfgVar.BootHarddriveTag, sizeof(PetAlertCfgVar.BootHarddriveTag), HarddriveTag);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
  }

  DEBUG((DEBUG_INFO, "PetAlert HDD ModelSerialNumber = %a\n", PetAlertCfgVar.BootHarddriveTag));

  if ((DeviceType != BAE_HDD_DEVICE) ||
    ((DeviceType == BAE_HDD_DEVICE) && AsciiStrCmp(HarddriveTag, PetAlertCfgVar.BootHarddriveTag))) {
    //
    // If booting from a network/removable device or from a different HDD send expanded BAE message
    //
    UpdateFlag                = 1;
    PetAlertMsg               = &mPetAlertWithChange;
    PetAlertMsg->EventData5   = PetAlertCfgVar.BootQueue[2] = PetAlertCfgVar.BootQueue[1];
    PetAlertMsg->EventData4   = PetAlertCfgVar.BootQueue[1] = PetAlertCfgVar.BootQueue[0];
    PetAlertMsg->EventData3   = PetAlertCfgVar.BootQueue[0] = DeviceType;
  }

  SendBaeMessage(PetAlertMsg);

  if (UpdateFlag) {
    Status = AsciiStrCpyS(PetAlertCfgVar.BootHarddriveTag, sizeof(PetAlertCfgVar.BootHarddriveTag), HarddriveTag);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    VarSize = sizeof(PetAlertCfgVar);
    Status = gRT->SetVariable (
                    L"PetAlertCfg",
                    &gPlatformGlobalVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    VarSize,
                    &PetAlertCfgVar
                    );
    ASSERT_EFI_ERROR(Status);
  }

  return Status;
}

/**
  Compare two device paths up to a size of Boot Opion's Device Path

  @param[in] BootOptionDp     Device path acquired from BootXXXX EFI variable
  @param[in] FileSysDp    Device path acquired through EFI_SIMPLE_FILE_SYSTEM_PROTOCOL Handles buffer

  @retval TRUE   Both device paths point to the same device
  @retval FALSE   Device paths point to different devices
**/
BOOLEAN
CompareDevicePaths(
  IN  EFI_DEVICE_PATH_PROTOCOL *BootOptionDp,
  IN  EFI_DEVICE_PATH_PROTOCOL *FileSysDp
)
{
  UINTN BootOptionDpSize;
  UINTN FileSysDpSize;

  if (BootOptionDp == NULL || FileSysDp == NULL) {
    return FALSE;
  }

  BootOptionDpSize = GetDevicePathSize (BootOptionDp) - END_DEVICE_PATH_LENGTH;
  FileSysDpSize    = GetDevicePathSize (FileSysDp) - END_DEVICE_PATH_LENGTH;

  if ((BootOptionDpSize <= FileSysDpSize) && (CompareMem (FileSysDp, BootOptionDp, BootOptionDpSize) == 0)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Get EFI device path through EFI_SIMPLE_FILE_SYSTEM_PROTOCOL Handles buffer. Acquired path must
  point to the same device as argument DevicePath passed to the function.

  @param[in] DevicePath   Device path acquired from BootXXXX EFI variable

  @retval EFI_DEVICE_PATH_PROTOCOL   Device path for booting
**/
EFI_DEVICE_PATH_PROTOCOL *
GetFullBootDevicePath(
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
)
{
  EFI_STATUS               Status;
  EFI_DEVICE_PATH_PROTOCOL *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL *ReturnDevicePath;
  UINTN                    HandleNum;
  EFI_HANDLE               *HandleBuf;
  UINTN                    Index;

  ReturnDevicePath = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &HandleNum,
                  &HandleBuf
                  );
  if ((EFI_ERROR (Status)) || (HandleBuf == NULL)) {
    return NULL;
  }

  for (Index = 0; Index < HandleNum; Index++) {
    TempDevicePath     = DevicePathFromHandle (HandleBuf[Index]);

    if (CompareDevicePaths(DevicePath, TempDevicePath)) {
      ReturnDevicePath = DuplicateDevicePath(TempDevicePath);
      break;
    }
  }

  return ReturnDevicePath;
}

UINT8
GetDeviceType(
  IN EFI_DEVICE_PATH_PROTOCOL *DevPath
  )
{
  UINT8                    DeviceType;
  BBS_BBS_DEVICE_PATH      *BbsDevicePath;

  DeviceType    = BAE_EMPTY_QUEUE;
  BbsDevicePath = (BBS_BBS_DEVICE_PATH *)DevPath;

  if (BbsDevicePath->Header.Type == BBS_DEVICE_PATH && BbsDevicePath->Header.SubType == BBS_BBS_DP) {
    //
    // Get device type from legacy-BBS style device path
    //
    switch (BbsDevicePath->DeviceType) {
      case BBS_TYPE_EMBEDDED_NETWORK:
        DeviceType = BAE_NETWORK_DEVICE;
        break;
      case BBS_TYPE_HARDDRIVE:
        DeviceType = BAE_HDD_DEVICE;
        break;
      case BBS_TYPE_CDROM:
      case BBS_TYPE_USB:
      case BBS_TYPE_FLOPPY:
        DeviceType = BAE_REMOVABLE_DEVICE;
        break;
      default:
        DeviceType = BAE_EMPTY_QUEUE;
        break;
    }
  } else {
    while (!IsDevicePathEnd(DevPath)) {
      if ((DevicePathType(DevPath) == MESSAGING_DEVICE_PATH) &&
                  (DevicePathSubType(DevPath) == MSG_USB_DP)){
        //
        // If device connected through USB, we automatically treat it as removable
        //
        DeviceType = BAE_REMOVABLE_DEVICE;
        break;
      } else if (DevicePathType(DevPath) == MEDIA_DEVICE_PATH) {
        if (DevicePathSubType(DevPath) == MEDIA_HARDDRIVE_DP) {
          DeviceType = BAE_HDD_DEVICE;
          break;
        } else if (DevicePathSubType(DevPath) == MEDIA_CDROM_DP) {
          DeviceType = BAE_REMOVABLE_DEVICE;
          break;
        } else if (DevicePathSubType(DevPath) == MEDIA_FILEPATH_DP) {
          DeviceType = BAE_NETWORK_DEVICE;
          break;
        }
      }

      DevPath = NextDevicePathNode(DevPath);
    }
  }

  return DeviceType;
}

EFI_STATUS
Trim(
  CHAR8 *sDesc,
  CHAR8 *sSrc,
  UINTN len
)
{
  UINTN p1 ,p2;
  CHAR8 ch;
  UINTN i,l;

  p1 = 0xFF;
  p2 = 0xFF;
  i = 0;
  for(;;) {
    ch = *(sSrc+i);
    if (ch == 0) {
      break;
    } else if (ch != 0x20) {
      p2 = i;
      if (p1 == 0xFF) {
          p1 = i;
      }
    } 
    i++;
  }

  l = (p2 - p1) + 1;
  CopyMem(sDesc, sSrc+p1, l);
  if (l < len) {
    *(sDesc+l) = 0;
    return EFI_SUCCESS;
  } else {
    return EFI_OUT_OF_RESOURCES;
  }
}

EFI_STATUS
GetModelSerialNumber(
  IN EFI_HANDLE Handle,
  OUT CHAR8 *ModelSerialNumber
)
{
  EFI_STATUS                      Status = EFI_NOT_FOUND;
  EFI_DISK_INFO_PROTOCOL          *DiskInfo;
  EFI_ATAPI_IDENTIFY_DATA         *IdentifyDriveInfo = NULL;
  UINT32                          BufferSize;
  CHAR8                           ModelNumber[42];
  CHAR8                           SerialNumber[22];
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath = NULL;

  Status = AsciiStrCpyS(ModelSerialNumber,MODELSERIALNUMBER_STR_SIZE, "");
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;

  Status = gBS->HandleProtocol(
                  Handle,
                  &gEfiDevicePathProtocolGuid, 
                  &DevicePath);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->LocateDevicePath (
                  &gEfiDiskInfoProtocolGuid,
                  &DevicePath,
                  &Handle
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDiskInfoProtocolGuid,
                  &DiskInfo
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  IdentifyDriveInfo = AllocatePool(sizeof(EFI_ATAPI_IDENTIFY_DATA));
  if (IdentifyDriveInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SetMem(IdentifyDriveInfo, sizeof (EFI_ATAPI_IDENTIFY_DATA), 0); 
  BufferSize = sizeof (EFI_ATAPI_IDENTIFY_DATA);
  Status = DiskInfo->Identify (
                      DiskInfo,
                      IdentifyDriveInfo,
                      &BufferSize
                      );
  if (EFI_ERROR(Status)) {
    FreePool(IdentifyDriveInfo);
    return Status;
  }

  CopyMem(SerialNumber, IdentifyDriveInfo->SerialNo, 20);
  SwapEntries (SerialNumber, 20);
  SerialNumber[20] = '\0';
  Status = Trim (SerialNumber, SerialNumber, sizeof(SerialNumber));
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem(ModelNumber, IdentifyDriveInfo->ModelName, 40);
  SwapEntries (ModelNumber, 40);
  ModelNumber[40] = '\0';
  Status = Trim (ModelNumber, ModelNumber, sizeof(ModelNumber));
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = AsciiStrCpyS(ModelSerialNumber, MODELSERIALNUMBER_STR_SIZE, ModelNumber);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  Status = AsciiStrCatS(ModelSerialNumber, MODELSERIALNUMBER_STR_SIZE, "_");
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  Status = AsciiStrCatS(ModelSerialNumber, MODELSERIALNUMBER_STR_SIZE, SerialNumber);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;

  FreePool(IdentifyDriveInfo);

  return EFI_SUCCESS;
}

EFI_STATUS
GetModelSerialNumberFromBbs(
  IN  UINT8   DeviceType,
  OUT CHAR8   *ModelSerialNumber
)
{
  EFI_LEGACY_BIOS_PROTOCOL        *LegacyBios;
  UINT16                          HddCount = 0;
  HDD_INFO                        *LocalHddInfo = NULL;
  UINT16                          BbsCount = 0;
  BBS_TABLE                       *BbsTable;
  EFI_STATUS                      Status;
  UINTN                           i;
  EFI_HANDLE                      Handle;
  UINTN                           DeviceIndex;
  UINTN                           DevicePriority;

  if (ModelSerialNumber == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = LegacyBios->GetBbsInfo (
                        LegacyBios,
                        &HddCount,
                        &LocalHddInfo,
                        &BbsCount,
                        &BbsTable
                        );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Find matching device by boot priority
  //
  DeviceIndex = (UINTN)-1;
  DevicePriority = (UINTN)-1;
  for (i = 0; i < BbsCount; i++) {
    if (BbsTable[i].BootPriority != BBS_IGNORE_ENTRY) {
      if (BbsTable[i].DeviceType == DeviceType) {
        if (DevicePriority > BbsTable[i].BootPriority) {
          DevicePriority = BbsTable[i].BootPriority;
          DeviceIndex = i;
        }
      }
    }
  }

  //
  // Get model and serial number
  //
  if (DeviceIndex != (UINTN)-1) {
    Handle = *(VOID**)(&BbsTable[DeviceIndex].IBV1);
    GetModelSerialNumber(Handle, ModelSerialNumber);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
NotifyPetAlert(
  EFI_EVENT           Event,
  VOID                *ParentImageHandle
)
{
  EFI_STATUS                      Status;
  UINT16                          BootCurrent;
  UINTN                           VarSize;
  CHAR16                          BootXXXX[16];
  EFI_DEVICE_PATH_PROTOCOL        *BootDevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *TempDevicePath;
  UINT8                           DeviceType;
  CHAR8                           ModelSerialNumber[MODELSERIALNUMBER_STR_SIZE];
  EFI_HANDLE                      BootDeviceHandle;
  BBS_BBS_DEVICE_PATH *           LegacyBootDP;
  EFI_BOOT_MANAGER_LOAD_OPTION    BootOption;

  Status = AsciiStrCpyS(ModelSerialNumber, MODELSERIALNUMBER_STR_SIZE, "");
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;

  BootDeviceHandle            = 0;
  DeviceType                  = 0;
  BootDevicePath              = NULL;

  //
  // Get current boot option from NVRAM variables
  //
  VarSize = sizeof (UINT16);
  Status = gRT->GetVariable (
              L"BootCurrent",
              &gEfiGlobalVariableGuid,
              NULL,
              &VarSize,
              &BootCurrent
              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  UnicodeSPrint(BootXXXX, sizeof(BootXXXX), L"Boot%04x", BootCurrent);

  Status = EfiBootManagerVariableToLoadOption (BootXXXX, &BootOption);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (((BBS_BBS_DEVICE_PATH *)BootOption.FilePath)->Header.Type == BBS_DEVICE_PATH) {
    BootDevicePath = DuplicateDevicePath(BootOption.FilePath);
  } else if (IsItPartitionsDevicePath (&BootOption)) {
    Status = ExpandPartitionsDevPath (&BootOption, &BootDevicePath);
   } else {
    //
    // For EFI boot get full device path from EFI_SIMPLE_FILE_SYSTEM
    //
    BootDevicePath = GetFullBootDevicePath(BootOption.FilePath);
  }

  EfiBootManagerFreeLoadOption (&BootOption);

  if (BootDevicePath == NULL) {
    return EFI_ABORTED;
  }

  DeviceType = GetDeviceType(BootDevicePath);

  //
  // Get model serial number if device is HDD
  //
  if (DeviceType == BAE_HDD_DEVICE) {
    LegacyBootDP = (BBS_BBS_DEVICE_PATH *)BootDevicePath;
    if (LegacyBootDP->Header.Type == BBS_DEVICE_PATH &&
      LegacyBootDP->Header.SubType == BBS_BBS_DP) {
      GetModelSerialNumberFromBbs(BBS_TYPE_HARDDRIVE, ModelSerialNumber);
    } else {
      TempDevicePath = BootDevicePath;
      Status = gBS->LocateDevicePath (
                    &gEfiSimpleFileSystemProtocolGuid,
                    &TempDevicePath,
                    &BootDeviceHandle
                    );
      if (EFI_ERROR(Status)) {
        FreePool(BootDevicePath);
        return Status;
      }

      GetModelSerialNumber(BootDeviceHandle, ModelSerialNumber);
    }
  }

  FreePool(BootDevicePath);

  Status = PetAlert(DeviceType, ModelSerialNumber);

  return Status;
}

EFI_STATUS
AmtPetAlertReadyToBoot(
  EFI_EVENT           Event,
  VOID                *ParentImageHandle
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (ManageabilityModeSetting() != 0) {
    DEBUG((DEBUG_ERROR, "<BAE_ReadyToBoot=TRUE>\n"));
    Status = NotifyPetAlert(Event, ParentImageHandle);
  } else {
    DEBUG((DEBUG_ERROR, "<BAE_ReadyToBoot=FALSE>\n"));
  }

  gBS->CloseEvent(Event);
  return Status;
}

