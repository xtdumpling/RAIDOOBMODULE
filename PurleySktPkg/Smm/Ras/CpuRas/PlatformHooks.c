/*++

Copyright (c) 2008 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformHooks.c

Abstract:



--*/


#include "PlatformHooks.h"
extern SPS_RAS_NOTIFY_PROTOCOL *mSpsRasNotifyProtocol;

VOID
PlatformHookNotifyBmc(
  UINT8 EventType,
  UINT8 EventState
) {
  return;
}

EFI_STATUS
PlatformHookNotifyME(
  UINT8 EventType,
  UINT8 EventState
) {
  EFI_STATUS Status = EFI_INVALID_PARAMETER;

  if(mSpsRasNotifyProtocol != NULL) {
    Status = mSpsRasNotifyProtocol->RasNotify(EventState);
  }

  return Status;
}

//End of File