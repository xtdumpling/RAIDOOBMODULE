/**
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/

/**

Copyright (c) 2009-2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MeState.h

  Register Definitions for Me States

**/
#ifndef _ME_STATE_H
#define _ME_STATE_H
///
/// Ignore ME_FW_INIT_COMPLETE status Macro
///
#define ME_STATUS_ME_STATE_ONLY(a)  ((a) & (~(ME_FW_INIT_COMPLETE | ME_FW_BOOT_OPTIONS_PRESENT)))
//
// Ignore ME_FW_INIT_COMPLETE status Macro
//
#define ME_STATUS_IGNORE_FW_INIT_COMPLETE(a)  ((a) & (~ME_FW_INIT_COMPLETE))

//
// Macro to check if ME FW INIT is completed
//
#define ME_STATUS_IS_ME_FW_INIT_COMPLETE(a) (((a) & ME_FW_INIT_COMPLETE) == ME_FW_INIT_COMPLETE)

//
// Marco to combind the complete bit to status
//
#define ME_STATUS_WITH_ME_INIT_COMPLETE(a)  ((a) | ME_FW_INIT_COMPLETE)

//
// Macro to check ME Boot Option Present
//
#define ME_STATUS_IS_ME_FW_BOOT_OPTIONS_PRESENT(a)  (((a) & ME_FW_BOOT_OPTIONS_PRESENT) == ME_FW_BOOT_OPTIONS_PRESENT)

//
// Abstract ME Mode Definitions
//
#define ME_MODE_NORMAL        0x00
#define ME_MODE_DEBUG         0x02
#define ME_MODE_TEMP_DISABLED 0x03
#define ME_MODE_SECOVER       0x04
#define ME_MODE_MASK          0x0F
#define ME_MODE_DFX_FW        0x05

#define ME_MODE_FAILED  0x06
//
// Abstract ME Status definitions
//
#define ME_READY                    0x00
#define ME_INITIALIZING             0x01
#define ME_IN_RECOVERY_MODE         0x02
#define ME_DISABLE_WAIT             0x06
#define ME_TRANSITION               0x07
#define ME_NOT_READY                0x0F
#define ME_INVALID                  0xFF
#define ME_FW_INIT_COMPLETE         0x80
#define ME_FW_BOOT_OPTIONS_PRESENT  0x100

#define ME_FW_UPDATES_IN_PROGRESS   0x200

#endif // ME_STATE_H
