/** @file
  Initializes PCH Storage and Communications Subsystem Controllers.

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
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
#include <PchInit.h>

//
// HS400 Tuning Definitions
//
#define RX_STROBE_DLL1_TAP_MAX_RANGE          39
#define RX_STROBE_DLL1_TAP_MIN_RANGE          0
#define RX_STROBE_DLL1_TAP_MIN_MEPT           5
#define RX_STROBE_DLL1_TAP_MAX_MEPT           16
#define TX_DATA_DLL_TAP_MAX_RANGE             79
#define TX_DATA_DLL_TAP_MIN_RANGE             0
#define TX_DATA_DLL_TAP_MIN_MEPT              4
#define TX_DATA_DLL_TAP_MAX_MEPT              22

//
// Command Definitions
//
#define CMD6                                  6
#define CMD8                                  8
#define CMD13                                 13
#define CMD31                                 31
#define SWITCH                                CMD6
#define SEND_EXT_CSD                          CMD8
#define SEND_STATUS                           CMD13
#define SEND_WRITE_PROT_TYPE                  CMD31
#define WRITE_BYTE_MODE                       3
#define BLOCK_LENGTH                          512
#define TIMEOUT_COMMAND                       100
#define TIMEOUT_DATA                          5000
#define HS_TIMING_INDEX                       185
#define BUS_WIDTH_INDEX                       183

//
// Card Status Definitions
//
#define SWITCH_ERROR                          BIT7
#define ERASE_RESET                           BIT13
#define WP_ERASE_SKIP                         BIT15
#define CID_CSD_OVERWRITE                     BIT16
#define ERROR                                 BIT19
#define CC_ERROR                              BIT20
#define CARD_ECC_FAILED                       BIT21
#define ILLEGAL_COMMAND                       BIT22
#define COM_CRC_ERROR                         BIT23
#define LOCK_UNLOCK_FAILED                    BIT24
#define CARD_IS_LOCKED                        BIT25
#define WP_VIOLATION                          BIT26
#define ERASE_PARAM                           BIT27
#define ERASE_SEQ_ERROR                       BIT28
#define BLOCK_LEN_ERROR                       BIT29
#define ADDRESS_MISALIGN                      BIT30
#define ADDRESS_OUT_OF_RANGE                  BIT31
