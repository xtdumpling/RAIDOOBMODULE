//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SocketSetup.h

  Header file for Platform Initialization Driver.

Revision History:

**/

#ifndef _SOCKET_SETUP_H_
#define _SOCKET_SETUP_H_

#include <Uefi.h>
#include <Uefi/UefiInternalFormRepresentation.h>
#include <FrameworkDxe.h>
#include <Protocol/IioUds.h>
#include "SocketSetupStrDefs.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <IncludePrivate/Library/PchAlternateAccessModeLib.h>
#include <Library/IoLib.h>
#include <Library/S3IoLib.h>

#include <Protocol/DataHub.h>
#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
// APTIOV_SERVER_OVERRIDE_RC_START
#include <Protocol/HiiPackageList.h>
// APTIOV_SERVER_OVERRIDE_RC_END
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/CpuIo.h>
#include <Protocol/Smbios.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/MpService.h>
#include <Protocol/IioSystem.h>
#include <Protocol/Cpu.h>
#include <Guid/DataHubRecords.h>
#include <Guid/HobList.h>

#include <Guid/MemoryConfigData.h>
#include <Guid/MemoryMapData.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/SocketVariable.h>
#include "SocketConfiguration.h"
#include <Cpu/CpuRegs.h>
#include "SysFunc.h"
#include "KtiSetupDefinitions.h"
#include <IndustryStandard/SmBios.h>
#include <Library/MmPciBaseLib.h>
#include <PchAccess.h>

#define NEWSTRING_SIZE 0x200

extern EFI_GUID        gSocketPkgListGuid;
// APTIOV_SERVER_OVERRIDE_RC_START : Commenting the GUID as it is not signalled in AMI BIOS.
//extern EFI_GUID        gSignalBeforeEnterSetupGuid;
// APTIOV_SERVER_OVERRIDE_RC_END : Commenting the GUID as it is not signalled in AMI BIOS.

typedef struct {
  UINT8   Type;
  UINT8   Length;
  UINT16  Handle;
} SMBIOS_HEADER;

typedef struct {
  UINT32  Id;
  UINT32  Freq;
  UINT32  MaxRatio;
  UINT32  MinRatio;
  UINT32  Ucode;
  UINT32  L1Cache;
  UINT32  L2Cache;
  UINT32  L3Cache;
  CHAR16  *Version;
} CPU_SETUP_INFO;

#endif
