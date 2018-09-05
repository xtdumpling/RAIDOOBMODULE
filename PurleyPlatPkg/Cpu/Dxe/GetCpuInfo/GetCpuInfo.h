//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to  
// the additional terms of the license agreement               
//
/**

Copyright (c) 2006 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.           

  @file GetCpuInfo.h

  Function prototype for GetCpuInfo driver

**/

#ifndef _ALL_CPU_INFO_H
#define _ALL_CPU_INFO_H

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/MdeModuleHii.h>
#include <Library/UefiLib.h>
#include <Setup/IioUniversalData.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Protocol/IioUds.h>
#include <Guid/PlatformInfo.h>
#include "SysFunc.h"
#include <Protocol/MpService.h>
#include <Library/HiiLib.h>
#include <Cpu/CpuRegs.h>
#include <Library/TimerLib.h>

#define UPDATE_DATA_SIZE  0x1000
//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//

extern UINT8  VfrBin[];

extern UINT8  GetCpuInfoStrings[];

typedef struct {
  EFI_HII_HANDLE      RegisteredHandle;
} GET_CPU_INFO;

EFI_STATUS
EFIAPI
RegisterAllCpuInfo (
  IN GET_CPU_INFO       *Private
  );

VOID
GetCpuInformation (
  VOID
  );
  
UINT32
GetCpuUcodeRevision (
  VOID
  );

VOID
UpdateTokenValue (
  CHAR16              *String, 
  CHAR8               *Lang,
  VOID                *StartOpCodeHandle,
  GET_CPU_INFO        *Private
  );    
  
#endif
