/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
 **/
/*++

Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  IeHeciMsgs.h

Abstract:

  Definitions of IE HECI messages

 **/
#ifndef IEHECIMSGS_H_
#define IEHECIMSGS_H_

#define BIOS_FIXED_HOST_ADDR          0
#define HECI_CORE_MESSAGE_ADDR        0x07

///
/// BUP group IDs
///
typedef enum {
  BUP_COMMON_GROUP_ID = 0xF0,
} BUP_GROUP_ID;



typedef union _IE_HECI_MESSAGE_HEADER {
  UINT32  Data;
  struct {
    UINT32  GroupId    : 8;
    UINT32  Command    : 7;
    UINT32  IsResponse : 1;
    UINT32  Reserved   : 8;
    UINT32  Result     : 8;
  } Fields;
} IE_HECI_MESSAGE_HEADER;


#define MEMORY_INIT_DONE_CMD  0x01

typedef union _MEMORY_INIT_DONE_DATA {
  struct {
    UINT32 Reserved0      :24; ///< BIT0-13
    UINT32 Status         :4;  ///< BIT24 - BIT27
    UINT32 Reserved1      :4;  ///< BIT28 - BIT31
  } Fields;
  UINT32 Data;
} MEMORY_INIT_DONE_DATA;


typedef struct _MEMORY_INIT_DONE_CMD_REQ {
  IE_HECI_MESSAGE_HEADER  Header;
  MEMORY_INIT_DONE_DATA   Data;
} MEMORY_INIT_DONE_CMD_REQ;

typedef struct _MEMORY_INIT_DONE_CMD_RSP {
  IE_HECI_MESSAGE_HEADER  Header;
  UINT8                   Action;
  UINT8                   Reserved[3];
} MEMORY_INIT_DONE_CMD_RSP;

typedef union _MEMORY_INIT_DONE_MSG {
  MEMORY_INIT_DONE_CMD_REQ  Request;
  MEMORY_INIT_DONE_CMD_RSP  Response;
} MEMORY_INIT_DONE_MSG;


#endif /* IEHECIMSGS_H_ */
