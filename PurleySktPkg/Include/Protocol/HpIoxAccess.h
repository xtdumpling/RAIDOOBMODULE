//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2007 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

    HpIOXAccess.h

Abstract:

    Header file for IOX access APIs.

-----------------------------------------------------------------------------*/
#ifndef _HPIOXACCESS_PROTOCOL_H_
#define _HPIOXACCESS_PROTOCOL_H_

//
// Forward reference for pure ANSI compatability
//
//EFI_FORWARD_DECLARATION (EFI_HP_IOX_ACCESS_PROTOCOL);


//
// HP IOX Access Protocol GUID
//
// {62652B53-79D9-4CF2-B5AA-AD99810A7F17}
#define EFI_HP_IOX_ACCESS_GUID \
  { \
    0x62652b53, 0x79d9, 0x4cf2, 0xb5, 0xaa, 0xad, 0x99, 0x81, 0x0a, 0x7f, 0x17 \
  }

#define ISSUE_ONLINE                      0
#define ISSUE_OFFLINE                     1
#define ISSUE_WARM_RESET                  2
#define ISSUE_POWER_ON                    3
#define ISSUE_BIOS_READY                  4
#define ISSUE_PLD_IDLE                    5

typedef
EFI_STATUS
(EFIAPI *READ_IOX_AND_UPDATE_HP_REQUEST) (
  IN UINT8      *HpRequest,
  IN BOOLEAN    TimeSliced
  );

typedef
EFI_STATUS
(EFIAPI *ISSUE_PLD_CMD_THRO_IOX) (
  IN UINT8           Request,
  IN UINT8           SocketId,
  IN BOOLEAN         TimeSliced
  );

typedef
EFI_STATUS
(EFIAPI *CLEAR_ATTN_LATCH) (
  IN BOOLEAN    TimeSliced
  );

typedef
EFI_STATUS
(EFIAPI *CLEAR_INTERRUPT) (
  IN  BOOLEAN TimeSliced
  );

typedef
EFI_STATUS
(EFIAPI *HP_SMBUS_READ_WRITE) (
  IN      UINT8    SlaveAddress,
  IN      UINT8    Operation,
  IN      UINT8    Command,
  IN OUT  VOID     *Buffer,
  IN      BOOLEAN  TimeSliced
  );

typedef
BOOLEAN
(EFIAPI *CHECK_ONLINE_OFFLINE_REQUEST) (
  VOID
  );

typedef struct _EFI_HP_IOX_ACCESS_PROTOCOL {
  READ_IOX_AND_UPDATE_HP_REQUEST   ReadIoxAndUpdateHpRequest;
  ISSUE_PLD_CMD_THRO_IOX           IssuePldCmdThroIox;
  CLEAR_ATTN_LATCH                 ClearAttnLatch;
  CLEAR_INTERRUPT                  ClearInterrupt;
  HP_SMBUS_READ_WRITE              HpSmbusReadWrite;
  CHECK_ONLINE_OFFLINE_REQUEST     CheckOnlineOfflineRequest;
} EFI_HP_IOX_ACCESS_PROTOCOL;

extern EFI_GUID         gEfiHpIoxAccessGuid;

#endif
