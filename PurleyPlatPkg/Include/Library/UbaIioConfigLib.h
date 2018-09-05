//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UBA IIO Config Library Header File.

  Copyright (c) 2012 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _UBA_IIO_CONFIG_LIB_H
#define _UBA_IIO_CONFIG_LIB_H

#include <Base.h>
#include <Uefi.h>

#include <IioPlatformData.h>

#define PLATFORM_IIO_CONFIG_UPDATE_SIGNATURE  SIGNATURE_32 ('P', 'I', 'I', 'O')
#define PLATFORM_IIO_CONFIG_UPDATE_VERSION    01

// {EB35ED63-EACA-4e29-9516-7EDF1F818837}
#define   PLATFORM_IIO_CONFIG_DATA_GUID \
{ 0xeb35ed63, 0xeaca, 0x4e29, { 0x95, 0x16, 0x7e, 0xdf, 0x1f, 0x81, 0x88, 0x37 } }

// {3093F83B-5934-473e-8523-24BF297EE684}
#define   PLATFORM_IIO_CONFIG_DATA_GUID_1 \
{ 0x3093f83b, 0x5934, 0x473e, { 0x85, 0x23, 0x24, 0xbf, 0x29, 0x7e, 0xe6, 0x84 } }

// {1C5267A4-634B-4bf2-BFF8-9A1164E6D198}
#define   PLATFORM_IIO_CONFIG_DATA_GUID_2 \
{ 0x1c5267a4, 0x634b, 0x4bf2, { 0xbf, 0xf8, 0x9a, 0x11, 0x64, 0xe6, 0xd1, 0x98 } }

// {1E486CCA-048E-4702-B28C-0B677201683A}
#define   PLATFORM_IIO_CONFIG_DATA_GUID_3 \
{ 0x1e486cca, 0x48e, 0x4702, { 0xb2, 0x8c, 0xb, 0x67, 0x72, 0x1, 0x68, 0x3a } }

// {6FE6C559-4F35-4111-98E1-332A251512F3}
#define   PLATFORM_IIO_CONFIG_DATA_DXE_GUID \
{ 0x6fe6c559, 0x4f35, 0x4111, { 0x98, 0xe1, 0x33, 0x2a, 0x25, 0x15, 0x12, 0xf3 } }

// {0F722F2A-650F-448a-ABB7-04EECD75BB30}
#define   PLATFORM_IIO_CONFIG_DATA_DXE_GUID_1 \
{ 0xf722f2a, 0x650f, 0x448a, { 0xab, 0xb7, 0x4, 0xee, 0xcd, 0x75, 0xbb, 0x30 } }

// {EBD11A00-8C5C-4f71-BB9E-5394032B01F4}
#define   PLATFORM_IIO_CONFIG_DATA_DXE_GUID_2 \
{ 0xebd11a00, 0x8c5c, 0x4f71, { 0xbb, 0x9e, 0x53, 0x94, 0x3, 0x2b, 0x1, 0xf4 } }

// {123BD082-3201-465c-B139-0CB8C77208F8}
#define   PLATFORM_IIO_CONFIG_DATA_DXE_GUID_3 \
{ 0x123bd082, 0x3201, 0x465c, { 0xb1, 0x39, 0xc, 0xb8, 0xc7, 0x72, 0x8, 0xf8 } }

typedef
EFI_STATUS
(*IIO_VAR_UPDATE_CALLBACK) (
  IN  IIO_GLOBALS             *IioGlobalData
);

typedef struct _PLATFORM_IIO_BIFURCATION_ENTRY {
  UINT8 Socket;
  UINT8 IouNumber;
  UINT8 Bifurcation;
} IIO_BIFURCATION_DATA_ENTRY;

typedef struct _PLATFORM_IIO_SLOT_ENTRY {
  UINT8   PortIndex;
#ifdef PC_HOOK
  UINT16  SlotNumber;     // 0xff if slot not implemented , Slot number if slot implemented
#else
  UINT8   SlotNumber;     // 0xff if slot not implemented , Slot number if slot implemented
#endif
  BOOLEAN InterLockPresent;
  UINT8   SlotPowerLimitScale;
  UINT8   SlotPowerLimitValue;
  BOOLEAN HotPlugCapable;
  UINT8   VppPort;        // 0xff if Vpp not enabled
  UINT8   VppAddress;
  BOOLEAN PcieSSDCapable;
  UINT8   PcieSSDVppPort; // 0xff if Vpp not enabled
  UINT8   PcieSSDVppAddress;
  BOOLEAN Hidden;
} IIO_SLOT_CONFIG_DATA_ENTRY;

typedef struct _PLATFORM_IIO_CONFIG_UPDATE_TABLE {
  UINT32                            Signature;
  UINT32                            Version;
  IIO_BIFURCATION_DATA_ENTRY        *IioBifurcationTablePtr;
  UINTN                             IioBifurcationTableSize;
  IIO_VAR_UPDATE_CALLBACK           CallUpdate;
  IIO_SLOT_CONFIG_DATA_ENTRY        *IioSlotTablePtr;
  UINTN                             IioSlotTableSize;
} PLATFORM_IIO_CONFIG_UPDATE_TABLE;

EFI_STATUS
PlatformIioConfigInit (
  IN OUT IIO_BIFURCATION_DATA_ENTRY    **BifurcationTable,  
  IN OUT UINT8                         *BifurcationEntries,
  IN OUT IIO_SLOT_CONFIG_DATA_ENTRY    **SlotTable,        
  IN OUT UINT8                         *SlotEntries
);

EFI_STATUS
PlatformIioConfigInit2 (
  IN     UINT8                         SkuPersonalityType,
  IN OUT IIO_BIFURCATION_DATA_ENTRY    **BifurcationTable,  
  IN OUT UINT8                         *BifurcationEntries,
  IN OUT IIO_SLOT_CONFIG_DATA_ENTRY    **SlotTable,        
  IN OUT UINT8                         *SlotEntries
);

EFI_STATUS
PlatformUpdateIioConfig (
  IN  IIO_GLOBALS             *IioGlobalData
);

STATIC  EFI_GUID gPlatformIioConfigDataGuid = PLATFORM_IIO_CONFIG_DATA_GUID;
STATIC  EFI_GUID gPlatformIioConfigDataGuid_1 = PLATFORM_IIO_CONFIG_DATA_GUID_1;
STATIC  EFI_GUID gPlatformIioConfigDataGuid_2 = PLATFORM_IIO_CONFIG_DATA_GUID_2;
STATIC  EFI_GUID gPlatformIioConfigDataGuid_3 = PLATFORM_IIO_CONFIG_DATA_GUID_3;

STATIC  EFI_GUID gPlatformIioConfigDataDxeGuid = PLATFORM_IIO_CONFIG_DATA_DXE_GUID;
STATIC  EFI_GUID gPlatformIioConfigDataDxeGuid_1 = PLATFORM_IIO_CONFIG_DATA_DXE_GUID_1;
STATIC  EFI_GUID gPlatformIioConfigDataDxeGuid_2 = PLATFORM_IIO_CONFIG_DATA_DXE_GUID_2;
STATIC  EFI_GUID gPlatformIioConfigDataDxeGuid_3 = PLATFORM_IIO_CONFIG_DATA_DXE_GUID_3;

#endif //_UBA_IIO_CONFIG_LIB_H
