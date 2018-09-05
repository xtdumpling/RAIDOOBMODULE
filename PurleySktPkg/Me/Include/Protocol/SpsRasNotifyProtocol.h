/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file SpsRasNotifyProtocol.h

  Protocol for ME RAS Notification support.
  This protocol is used by RAS.

**/
#ifndef _SPS_RAS_NOTIFY_PROTOCOL_H_
#define _SPS_RAS_NOTIFY_PROTOCOL_H_

#define HECI_RAS_TIMEOUT                            5    // HECI timeout in ms


typedef enum
{
  RasNotificationStart                    = 0x00,
  RasNotificationDone                     = 0x01,
  RasNotificationSerialPeciQuiescence     = 0x02,
  RasNotificationSerialPeciUnQuiescence   = 0x03,
  RasNotificationDmiQuiescence            = 0x04,
  RasNotificationDmiUnQuiescence          = 0x05
} RAS_NOTIFICATION_TYPE;



extern EFI_GUID gSpsRasNotifyProtocolGuid;

typedef struct _SPS_SMM_HMRFPO_PROTOCOL_ _SPS_SMM_HMRFPO_PROTOCOL;

typedef EFI_STATUS (EFIAPI *RAS_NOTIFY_FUNC)(RAS_NOTIFICATION_TYPE NotificationType);

typedef struct _SPS_RAS_NOTIFY_PROTOCOL {

  RAS_NOTIFY_FUNC     RasNotify;

} SPS_RAS_NOTIFY_PROTOCOL;

#endif // _SPS_RAS_NOTIFY_PROTOCOL_H_