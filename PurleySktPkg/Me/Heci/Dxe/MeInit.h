/** @file
  ME Firmware Version Info implementation.

@copyright
 Copyright (c) 2013 - 2016 Intel Corporation. All rights
 reserved This software and associated documentation (if any) is
 furnished under a license and may only be used or copied in
 accordance with the terms of the license. Except as permitted
 by the license, no part of this software or documentation may
 be reproduced, stored in a retrieval system, or transmitted in
 any form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _ME_INIT_H
#define _ME_INIT_H


/**
  Send ME the BIOS end of Post message.

  @retval EFI_SUCCESS             EOP is sent successfully
  @retval EFI_DEVICE_ERROR        No HECI1 device
  @retval EFI_TIMEOUT             Timeout when send EOP message
**/
EFI_STATUS
MeEndOfPostEvent (
  OUT UINT32                       *RequestedActions
  );

/**
  1. Cf9Gr Lock Config
      - PCH BIOS Spec Rev 0.9 Section 18.4  Additional Power Management Programming
        Step 2
        Set "Power Management Initialization Register (PMIR) Field 1", D31:F0:ACh[31] = 1b
        for production machine according to "RS - PCH Intel Management Engine
        (Intel(r) ME) BIOS Writer's Guide".
  2. Remove PSF access prior to boot

**/
VOID
LockConfig (
  VOID
  );

#endif