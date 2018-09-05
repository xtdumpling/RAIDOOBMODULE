/** @file
  TraceHub supporting function for X64

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
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

#include <PiDxe.h>
#include <PiSmm.h>
#include <Library/PcdLib.h>
#include <Library/TraceHubDebugLib.h>
#include <sventx.h>

psven_handle_t    TraceHubHandle = NULL;
sven_header_t     TraceHubHandleHeader;
extern EFI_GUID   gEfiTraceHubDebugLibX64Guid;

RETURN_STATUS
EFIAPI
TraceHubDebugWrite (
  IN TRACE_HUB_SEVERITY_TYPE    SeverityType,
  IN UINT8                      *Buffer,
  IN UINTN                      NumberOfBytes
  )
{
  TRACE_HUB_PLATFORM_DATA   mTraceHubPlatformData;

  if (TraceHubHandle == NULL) {
    mTraceHubPlatformData.Master = PcdGet8 (PcdTraceHubDebugLibMaster);
    mTraceHubPlatformData.Channel = PcdGet8 (PcdTraceHubDebugLibChannel);

    SVEN_INIT_STATE (&TraceHubHandleHeader, sventx_PlatformInit, NULL);
    TraceHubHandle = SVEN_ALLOC_HANDLE_STATE (&TraceHubHandleHeader, &mTraceHubPlatformData);

    SVEN_SET_HANDLE_GUID_UNIT (
      TraceHubHandle,
      *((sven_guid_t *)(VOID *) &gEfiTraceHubDebugLibX64Guid),
      0
      );
  }

  SVEN_DEBUG (TraceHubHandle, (sven_severity_t) SeverityType, (const char *)Buffer, (sven_u16_t) NumberOfBytes);

  return RETURN_SUCCESS;
}
