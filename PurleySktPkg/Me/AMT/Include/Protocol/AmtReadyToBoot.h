/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c)  2010 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  AmtReadyToBoot.h

@brief
  AmtReadyToBoot Protocol definitions

**/
#ifndef _AMT_READY_TO_BOOT_PROTOCOL_H_
#define _AMT_READY_TO_BOOT_PROTOCOL_H_

///
/// AMT Readt to Boot Protocol
/// This protocol performs all Management Engine task
///

#define AMT_READY_TO_BOOT_PROTOCOL_GUID \
  { \
    0x40b09b5a, 0xf0ef, 0x4627, 0x93, 0xd5, 0x27, 0xf0, 0x4b, 0x75, 0x4d, 0x5 \
  }

#define AMT_READY_TO_BOOT_PROTOCOL_REVISION_1  1

typedef struct _AMT_READY_TO_BOOT_PROTOCOL  AMT_READY_TO_BOOT_PROTOCOL;

/**
  Signal an event for Amt ready to boot.


  @retval EFI_SUCCESS             Mebx launched or no controller
**/
typedef
EFI_STATUS
(EFIAPI *AMT_READY_TO_BOOT_PROTOCOL_SIGNAL) (
  VOID
  );

/**
  AMT Ready to Boot Protocol
  The interface functions are for Performing all Management Engine task
**/
struct _AMT_READY_TO_BOOT_PROTOCOL {
  UINT8                             Revision; ///< Revision for the protocol structure
  AMT_READY_TO_BOOT_PROTOCOL_SIGNAL Signal;   ///< Performs all Management Engine task
};

extern EFI_GUID gAmtReadyToBootProtocolGuid;

#endif
