/** @file
  Definitions for MEI Bus message support

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _MEI_BUS_MSG_H_
#define _MEI_BUS_MSG_H_

///
/// ME Address of Fixed Address MEI Header -
/// This is the logical address of the Intel(R) ME client
/// of the message. This address is assigned during ME firmware initialization.
///
#define HECI_MEI_BUS_MESSAGE_ADDR     0x00

#define HECI_BUS_DISABLE_OPCODE       0x0C
#define HECI_BUS_DISABLE_ACK_OPCODE   0x8C

///
/// Disable Heci Bus message
///
typedef union _HBM_COMMAND {
  UINT8 Data;
  struct {
    UINT8 Command    : 7;
    UINT8 IsResponse : 1;
  } Fields;
} HBM_COMMAND;

typedef struct _HECI_BUS_DISABLE_CMD {
  HBM_COMMAND Command;
  UINT8       Reserved[3];
} HECI_BUS_DISABLE_CMD;

typedef struct _HECI_BUS_DISABLE_CMD_ACK {
  HBM_COMMAND Command;
  UINT8       Status;
  UINT8       Reserved[2];
} HECI_BUS_DISABLE_CMD_ACK;

#endif // _MEI_BUS_MSG_H_

