/** @file
  Definitions for the TraceHubStatusCodeHandlerRuntimeDxe

@copyright
  Copyright (c) 2013 - 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
 This file contains 'Framework Code' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may not be modified, except as allowed by
 additional terms of your license agreement.

@par Specification Reference:
**/

#ifndef __TRACE_HUB_STATUS_CODE_HANDLER_RUNTIME_DXE_H__
#define __TRACE_HUB_STATUS_CODE_HANDLER_RUNTIME_DXE_H__

#include <PiDxe.h>
#include <Guid/EventGroup.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Protocol/ReportStatusCodeHandler.h>

#include <TraceHubStatusCodeWorkerRuntimeDxe.h>

#endif
