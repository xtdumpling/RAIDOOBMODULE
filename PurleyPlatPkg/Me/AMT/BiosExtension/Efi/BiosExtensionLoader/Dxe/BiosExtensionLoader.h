/** @file
  Include for AMT Bios Extentions Loader

@copyright
  Copyright (c) 2004 - 2016 Intel Corporation. All rights reserved
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
#ifndef _AMT_PTBX_H_
#define _AMT_PTBX_H_

#include <IndustryStandard/Acpi10.h>
#include <IndustryStandard/Acpi20.h>
#include <IndustryStandard/Acpi30.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/IdeControllerInit.h>
#include <IndustryStandard/SmBus.h>
#include <Protocol/SmbusHc.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/SmBios.h>
#include <Guid/Acpi.h>

#include "Inventory.h"
#include "UsbProvision.h"
#include <IndustryStandard/Pci22.h>
//
// Consumed Protocol and Guid definition
//
#include <Protocol/AlertStandardFormat.h>
#include <Protocol/ActiveManagement.h>
#include <Protocol/AmtPolicy.h>
#include <Protocol/Wdt.h>
#include <Protocol/MebxProtocol.h>
#include <MeBiosExtensionSetup.h>
#include <Protocol/AmtReadyToBoot.h>

#include <MeChipset.h>
#include <PchAccess.h>
#include <Library/MmPciBaseLib.h>
#include <Library/PchInfoLib.h>
#include <Library/DxeMeLib.h>
#include <Library/DxeAmtLib.h>
#include <Library/TimerLib.h>
#include <MeBiosPayloadHob.h>

// Server BIOS uses global definitions
#include "IndustryStandard/SmBios.h"
// Redefine Client's definitions to use global
#define SMBIOS_ENTRY_POINT              SMBIOS_TABLE_ENTRY_POINT
#define SMBIOS_HEADER                   SMBIOS_STRUCTURE
#define SMBIOS_TABLE_TYPE_ONE           SMBIOS_TABLE_TYPE1
#define SMBIOS_TABLE_TYPE_THREE         SMBIOS_TABLE_TYPE3
#define SMBIOS_TABLE_TYPE_FOUR          SMBIOS_TABLE_TYPE4
#define SMBIOS_TABLE_TYPE_TWENTY_THREE  SMBIOS_TABLE_TYPE23
#define ACPI_HEADER                     EFI_ACPI_DESCRIPTION_HEADER

#define MEBX_RET_ACTION_CONTINUE_TO_BOOT  0x01
#define MEBX_RET_ACTION_RESET             0x04
#define MEBX_RET_CODE_MASK                0x07

#define MEBX_RET_ACTION_GLOBAL_RESET      0x01

#define MEBX_USER_ENTERED_RESPONSE        0x0002
#define MEBX_RA_SELECTION_MENU            0x0004
#define MEBX_HIDE_UNCONFIGURE_CONF_PROMPT 0x0040
#define MEBX_DEBUG_MSG                    0x4000
#define MEBX_UNCONFIGURE                  0x8000

#define MEBX_STF_PCI_DEV_TABLE            0x0001
#define MEBX_STF_MEDIA_DEV_TABLE          0x0002

#define MEBX_MEDIA_IN_ATA                 0x0000
#define MEBX_MEDIA_IN_ATAPI               0x0001
#define MEBX_MEDIA_IN_SATA                0x0002

#define MEBX_MEDIA_DT_HDD                 0x0000
#define MEBX_MEDIA_DT_CDROM               0x0001
#define MEBX_MEDIA_DT_DVD                 0x0002
#define MEBX_MEDIA_DT_FDD                 0x0003
#define MEBX_MEDIA_DT_MO                  0x0004

//
// MEBX Software Class DXE Subclass Progress Code definitions.
//
#define EFI_SW_DXE_MEBX_OPROM_DONE              (EFI_OEM_SPECIFIC | 0x00000000)
#define EFI_SW_DXE_MEBX_OTHER_UNSPECD           (EFI_OEM_SPECIFIC | 0x00000009)
#define EFI_SW_DXE_MEBX_RESET_ACTION            (EFI_OEM_SPECIFIC | 0x0000000a)

#pragma pack(1)

#define DEVICES_LIST_VERSION            0x0001
#define AMT_DATA_VERSION                0x0101
#define MEBX_SETUP_VERSION              0xB000
#define MEBX_VERSION                    0xB000      ///< for MEBx 10.0.0 or later
#define MEBX_SETUP_PLATFORM_MNT_DEFAULT MNT_OFF
#define MEBX_SETUP_SOL_DEFAULT          0
// APTIOV_SERVER_OVERRIDE_RC_START : Too many PCI devices in Purley so array is out of range to collect failed.
//#define PCI_DEVICE_MAX_NUM              256
#define PCI_DEVICE_MAX_NUM              512
// APTIOV_SERVER_OVERRIDE_RC_END : Too many PCI devices in Purley so array is out of range to collect failed.
#define MEDIA_DEVICE_MAX_NUM            32

#define SMB_SIG                         0x5F4D535F
#define SMB_SIG_0                       0x5F
#define SMB_SIG_1                       0x53
#define SMB_SIG_2                       0x4D
#define SMB_SIG_3                       0x5F
#define RSDT_SIG                        0x54445352  ///< RSDT (in reverse)
#define ASF_SIG                         0x21465341  ///< ASF! (in reverse)
#define ONEMIN                          60000000
#define FIVEMIN                         300000000
#define TENMIN                          600000000

#define INVOKE_MEBX_BIT                 0x08

typedef enum _HWAI_FRU_TYPE_
{
  HWAI_FRU_TYPE_NONE= 0,
  HWAI_FRU_TYPE_PCI
} HWAI_FRU_TYPE;

typedef struct {
  UINT16  VersionInfo;
  UINT8   DevCount;
  UINT8   Reserved1[5];
} AMT_FRU_DEVICES_HEADER;

typedef struct {
  UINT16  Vid;
  UINT16  Did;
  UINT16  Rid;
  UINT16  SubSysVid;
  UINT16  SubSysDid;
  UINT32  ClassCode;
  UINT16  BusDevFcn;
} MEBX_FRU_PCI_DEVICES;

typedef struct {
  AMT_FRU_DEVICES_HEADER  PciDevicesHeader;
  MEBX_FRU_PCI_DEVICES    PciDevInfo[PCI_DEVICE_MAX_NUM];
} AMT_PCI_FRU;

typedef struct _PCI_DEV_INFO {
  UINT16  VersionInfo;
  UINT8   PciDevCount;
  UINT8   Reserved[5];
} PCI_DEV_INFO;

typedef struct _HWAI_PCI_FRU_DATA_ {
  UINT16  VendorID;
  UINT16  DeviceID;
  UINT16  RevID;
  UINT16  SubsystemVendorID;
  UINT16  SubsystemDeviceID;
  UINT32  ClassCode;
  UINT16  BusDevFunc;
} HWAI_PCI_DATA;

typedef struct _HWAI_PCI_FRU_ENTRY {
  UINT8         SmbiosType;
  UINT8         Length;
  UINT16        SmbiosHandle;
  UINT32        FruType;
  HWAI_PCI_DATA FruData;
} HWAI_PCI_FRU;

typedef struct _HWAI_FRU_TABLE_HEADER {
  UINT16        TableByteCount;
  UINT16        StructureCount;
  HWAI_PCI_FRU  Data[1];
} HWAI_FRU_TABLE;

typedef struct _HWAI_MEDIA_DATA_ {
  UINT16  Length;
  UINT8   Interface;
  UINT8   DevType;
  UINT8   Manufacturer[40];
  UINT8   SerialNumber[20];
  UINT8   Version[8];
  UINT8   ModelNumber[40];
  UINT32  MaxMediaSizeLow;
  UINT32  MaxMediaSizeHigh;
  UINT16  Capabilities[3];
} HWAI_MEDIA_DATA;

typedef struct _HWAI_MEDIA_ENTRY {
  UINT8           SmbiosType;
  UINT8           Length;
  UINT16          SmbiosHandle;
  HWAI_MEDIA_DATA MediaData;
} HWAI_MEDIA_ENTRY;

typedef struct _HWAI_MEDIA_DEVICE_TABLE_HEADER {
  UINT16            TableByteCount;
  UINT16            StructureCount;
  HWAI_MEDIA_ENTRY  Data[1];
} HWAI_MEDIA_TABLE;

typedef struct _MEDIA_DEV_INFO {
  UINT16  VersionInfo;
  UINT8   MediaDevCount;
  UINT8   Reserved[5];
} MEDIA_DEV_INFO;

typedef struct {
  UINT16  StructSize;
  UINT8   Interface;
  UINT8   DevType;
  UINT8   Rsvd[40];
  UINT8   SerialNo[20];
  UINT8   VersionNo[8];
  UINT8   ModelNo[40];
  UINT64  MaxMediaSize;
  UINT16  SupportedCmdSets[3];
} MEBX_FRU_MEDIA_DEVICES;

typedef struct {
  AMT_FRU_DEVICES_HEADER  MediaDevicesHeader;
  MEBX_FRU_MEDIA_DEVICES  MediaDevInfo[MEDIA_DEVICE_MAX_NUM];
} AMT_MEDIA_FRU;

// Server BIOS usese UEFI definitionns


typedef struct _MEBX_DEBUG_FLAGS_ {
  UINT16  Port80 : 1;   ///< Port 80h
  UINT16  Rsvd : 15;    ///< Reserved
} MEBX_DEBUG_FLAGS;

typedef struct _MEBX_OEM_RESOLUTION_SETTINGS_ {
  UINT16  MebxNonUiTextMode : 4;
  UINT16  MebxUiTextMode : 4;
  UINT16  MebxGraphicsMode : 4;
  UINT16  Rsvd : 4;
} MEBX_OEM_RESOLUTION_SETTINGS;

typedef struct {
  UINT16                        BpfVersion;
  UINT8                         CpuReplacementTimeout;
  UINT8                         Reserved[7];
  UINT8                         ActiveRemoteAssistanceProcess;
  UINT8                         CiraTimeout;
  UINT16                        OemFlags;
  MEBX_DEBUG_FLAGS              MebxDebugFlags;
  UINT32                        MeBiosSyncDataPtr;
  UINT32                        UsbKeyDataStructurePtr;
  MEBX_OEM_RESOLUTION_SETTINGS  OemResolutionSettings;
  UINT8                         Reserved3[0x2E];
} MEBX_BPF;

typedef struct {
  UINT16  VersionInfo;
  UINT8   Reserved[2];
  UINT32  PtrSmbiosTable;
  UINT32  PtrAcpiRsdt;
  UINT16  SupportedTablesFlags;
  UINT32  PciDevAssertInfoPtr;
  UINT32  MediaDevAssetInfoPtr;
} AMT_DATA;

typedef union {
  UINT32  Data32;
  UINT16  Data16[2];
  UINT8   Data8[4];
} DATA32_UNION;

#pragma pack()

/**
  Signal a event for Amt ready to boot.


  @retval EFI_SUCCESS             Mebx launched or no controller
**/
EFI_STATUS
EFIAPI
AmtReadyToBoot (
  VOID
  );

/**
  This routine is run at boot time.
  1) Initialize AMT library.
  2) Check if MEBx is required to be launched by user.
  3) Build and send asset tables to ME FW.
  4) Check USB provision.
  5) Hide unused AMT devices in prior to boot.

  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structure.
  @retval EFI_NOT_FOUND           Consumer FW SKU detected - there is no MEBx on Consumer FW SKU
**/
EFI_STATUS
MebxNotifyEvent (
  VOID
  );

/**
  Check the status of Amt devices

  @param[in] MeBiosExtensionSetupData   MEBx setting data
**/
VOID
AmtDeviceConfigure (
  IN  ME_BIOS_EXTENSION_SETUP     *MeBiosExtensionSetupData
  );

/**
  Detect EFI MEBx support; Loading and execute.

  @param[in] MebxBiosParamsBuffer MebxBiosParams Flat pointer
  @param[in] VarAttributes of ME_BIOS_EXTENSION_SETUP variable.
**/
EFI_STATUS
AdjustAndExecuteMebxImage (
  IN  VOID                        *MebxBiosParamsBuffer,
  IN  UINT32                      VarAttributes
  );

/**
  Detect BIOS invoke reasons.

  @param[in] InvokeMebx           Pointer to the Invoke MEBx flag
  @param[in] MebxBiosParamsBuffer MebxBiosParams Flat pointer
**/
VOID
CheckForBiosInvokeReason (
  IN  UINT8       *InvokeMebx,
  IN  MEBX_BPF    *MebxBiosParams
  );

/**
  Dump MEBx BIOS Params

  @param[in]   MebxBiosParams - MEBx BIOS params
**/
VOID
DxeMebxBiosParamsDebugDump (
  IN MEBX_BPF *MebxBiosParams
  );

extern AMT_MEDIA_FRU  mAmtMediaFru;
extern AMT_PCI_FRU    mAmtPciFru;

//
// USB Provisioning
//
extern VOID           *mUsbProvData;
extern VOID           *mUsbProvDataBackup;
extern UINTN          mUsbProvDataSize;
extern BOOLEAN        mUsbProvsionDone;
extern UINT8          mUsbKeyBus, mUsbKeyPort, mUsbKeyDevice, mUsbKeyFunction;

#endif // _AMT_PTBX_H_
