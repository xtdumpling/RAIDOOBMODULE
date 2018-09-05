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

  PlatformHooks.h

Abstract:

--*/


#define RasEventStart 0x0
#define RasEventEnd   0x1

#include <Protocol/SpsRasNotifyProtocol.h>

VOID
PlatformHookNotifyBmc(
  UINT8 EventType,
  UINT8 EventState
);

EFI_STATUS
PlatformHookNotifyME(
  UINT8 EventType,
  UINT8 EventState
);

//End of File