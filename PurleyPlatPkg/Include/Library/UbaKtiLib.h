//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  UbaKtiLib header file

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#include <Base.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <MemHost.h>
#include <KtiHost.h>
#include <PlatformFuncCommon.h>

#define PLATFORM_KTIEP_UPDATE_SIGNATURE  SIGNATURE_32 ('P', 'K', 'T', 'I')
#define PLATFORM_KTIEP_UPDATE_VERSION    01


typedef struct _PLATFORM_KTI_EPARAM_UPDATE_TABLE {
  UINT32                            Signature;
  UINT32                            Version;
  ALL_LANES_EPARAM_LINK_INFO        *AllLanesEparamTablePtr;
  UINT32                            SizeOfAllLanesEparamTable;
  PER_LANE_EPARAM_LINK_INFO         *PerLaneEparamTablePtr;
  UINT32                             SizeOfPerLaneEparamTable;
} PLATFORM_KTI_EPARAM_UPDATE_TABLE;

