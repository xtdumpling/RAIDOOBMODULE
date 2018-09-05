/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2007 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  Hecidrv.h

@brief:
  HECI driver

**/

#define EFI_DEADLOOP() { VOLATILE UINTN __DeadLoopVar__ = 1; while (__DeadLoopVar__); }
#define EVENT_REC_TOK                   "EventRec"        ///< GUID or String Event
#define SCRIPT_MEM_WRITE PCH_INIT_COMMON_SCRIPT_MEM_WRITE

EFI_STATUS MeReadyToBootEventFunction(IN EFI_EVENT Event, IN VOID *ParentImageHandle);
EFI_STATUS MeScriptSaveEventFunction(IN EFI_EVENT Event, IN VOID *ParentImageHandle);
VOID PerformIderWorkaround(IN EFI_HANDLE Handle);
