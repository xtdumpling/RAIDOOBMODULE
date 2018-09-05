//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2004 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file BdsLib.h

  BDS library definition, include the file and data structure

**/

#ifndef _BDS_LIB_H_
#define _BDS_LIB_H_

#include <Base.h>
#include <Uefi.h>
#include <Protocol/SimpleTextOut.h>
#include <Library/GenericBdsLib.h>
#include "Library/BmMachine.h"

//
// Include the performance head file and defind macro to add perf data
//
#ifdef EFI_DXE_PERFORMANCE
#include "Performance.h"
#define WRITE_BOOT_TO_OS_PERFORMANCE_DATA WriteBootToOsPerformanceData ()
#else
#define WRITE_BOOT_TO_OS_PERFORMANCE_DATA
#endif

extern EFI_HANDLE mBdsImageHandle;

//
// Constants which are variable names used to access variables
//
#define VarLegacyDevOrder L"LegacyDevOrder"

//
// Data structures and defines
//
#define FRONT_PAGE_QUESTION_ID  0x0000
#define FRONT_PAGE_DATA_WIDTH   0x01

//
// ConnectType
//
#define CONSOLE_OUT 0x00000001
#define STD_ERROR   0x00000002
#define CONSOLE_IN  0x00000004
#define CONSOLE_ALL (CONSOLE_OUT | CONSOLE_IN | STD_ERROR)

//
// Load Option Attributes defined in EFI Specification
//
#define LOAD_OPTION_ACTIVE              0x00000001
#define LOAD_OPTION_FORCE_RECONNECT     0x00000002
#define IS_LOAD_OPTION_TYPE(_c, _Mask)  (BOOLEAN) (((_c) & (_Mask)) != 0)

//
// Define Maxmim characters that will be accepted
//
#define MAX_CHAR            480
#define MAX_CHAR_SIZE       (MAX_CHAR * 2)

#define MIN_ALIGNMENT_SIZE  4
#define ALIGN_SIZE(a)       ((a % MIN_ALIGNMENT_SIZE) ? MIN_ALIGNMENT_SIZE - (a % MIN_ALIGNMENT_SIZE) : 0)

// APTIOV_SERVER_OVERRIDE_RC_START
//
// Device Path 
//
#define EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE    0xff
#define EfiDevicePathType(a)                  (((a)->Type) & 0x7f)
#define EfiIsDevicePathEndType(a)             (EfiDevicePathType (a) == 0x7f)
#define EfiIsDevicePathEndSubType(a)          ((a)->SubType == EFI_END_ENTIRE_DEVICE_PATH_SUBTYPE)
#define EfiIsDevicePathEnd(a)                 (EfiIsDevicePathEndType (a) && EfiIsDevicePathEndSubType (a))
// APTIOV_SERVER_OVERRIDE_RC_END

//
// Bds boot relate lib functions
//
EFI_STATUS
BdsLibUpdateBootOrderList (
  IN  LIST_ENTRY                     *BdsOptionList,
  IN  CHAR16                         *VariableName
  );

//
// Bds device path relate lib functions
//
EFI_DEVICE_PATH_PROTOCOL  *
BdsLibUnpackDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevPath
  );

VOID                      *
EfiLibGetVariable (
  IN CHAR16               *Name,
  IN EFI_GUID             *VendorGuid
  );

//
// Internal functions
//
EFI_STATUS
BdsBootByDiskSignatureAndPartition (
  IN  BDS_COMMON_OPTION          * Option,
  IN  HARDDRIVE_DEVICE_PATH      * HardDriveDevicePath,
  IN  UINT32                     LoadOptionsSize,
  IN  VOID                       *LoadOptions,
  OUT UINTN                      *ExitDataSize,
  OUT CHAR16                     **ExitData OPTIONAL
  );

//
// Notes: EFI 64 shadow all option rom
//
#ifdef EFI64
#define EFI64_SHADOW_ALL_LEGACY_ROM() ShadowAllOptionRom ();
VOID
ShadowAllOptionRom();
#else
#define EFI64_SHADOW_ALL_LEGACY_ROM()
#endif

//
// BBS support macros and functions
//

extern VOID
WriteBootToOsPerformanceData (
  VOID
  );

#endif // _BDS_LIB_H_
