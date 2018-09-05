/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

/**
@copyright
  Copyright (c)  2012 - 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  EviCallbackPeim.h

@brief
  Header file for the EVI Callback Init PEIM

**/
#ifndef _EV_LOADER_PEIM_H_
#define _EV_LOADER_PEIM_H_

#ifdef SSA_FLAG
#ifdef SSA_LOADER_FLAG

#include "SysHost.h"
#ifndef MINIBIOS_BUILD
#include <Base.h>
#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#endif //MINIBIOS_BUILD

#include "MrcSsaServices.h"

extern EFI_GUID gSsaBiosCallBacksPpiGuid;

typedef int MRC_OEM_STATUS_COMMAND;

typedef struct SSA_BIOS_SERVICES_LOADER_STATE {
  MRC_OEM_STATUS_COMMAND      RunCheckpoint;
  BOOLEAN                     LoadFromTarget;
  VOID *                      SysConfig;
} SSA_BIOS_SERVICES_LOADER_STATE;

#define BIOS_SSA_VARIABLES_GUID \
  {0x43eeffe8, 0xa978, 0x41dc, 0x9d, 0xb6, 0x54, 0xc4, 0x27, 0xf2, 0x7e, 0x2a}

#define BIOS_SSA_RESULTS_GUID \
  {0x8f4e928, 0xf5f, 0x46d4, 0x84, 0x10, 0x47, 0x9f, 0xda, 0x27, 0x9d, 0xb6}


#define VAR_BUFFER_SIZE 2048

#define MAX_NUM_CONFIGS 20
#define MAX_NUM_CHUNKS 9999

#define SYS_CONFIG_NAME L"syscg"
#define TOOL_BINARY_NAME L"toolh"


#pragma pack(push, 1)

typedef struct SSA_BIOS_SYS_CONFIG_HEADER {
  UINT32         SystemConfigRevision;
  UINT32         ToolSize;
  UINT32         PostCode;
  UINT32         PrintLevel;
  UINT32         ConfigCount;
} SSA_BIOS_SYS_CONFIG_HEADER;

#pragma pack(pop)

#define VAR_NAME_LENGTH 5
#define VAR_NAME_SIZE (sizeof(CHAR16) * VAR_NAME_LENGTH)

#define VAR_NULL_NAME_LENGTH (VAR_NAME_LENGTH + 1)     // length as a null terminated string
#define VAR_NULL_NAME_SIZE (sizeof(CHAR16) * VAR_NULL_NAME_LENGTH)

#define NULL_STRING L""

#endif //SSA_LOADER_FLAG
#else
//Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
#endif
