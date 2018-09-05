/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

/**

Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  MeTypeLibInternal.h

@brief:

  MeType Library

**/
#ifndef _ME_TYPE_LIB_INTERNAL_H_
#define _ME_TYPE_LIB_INTERNAL_H_

#define ME_DETECTION_REPEATS_MAX  200
#define ME_DETECTION_STEP_TIMEOUT 1000

typedef enum _ON_BOARD_ME_TYPE
{
   ME_TYPE_UNDEF = METYPE_UNDEF,
   ME_TYPE_DFX   = METYPE_DFX,
   ME_TYPE_SPS   = METYPE_SPS,
   ME_TYPE_AMT   = METYPE_AMT,
   ME_TYPE_DISABLED = METYPE_DISABLED
} ON_BOARD_ME_TYPE;

ON_BOARD_ME_TYPE GetOnBoardMeType (void);

#endif // _ME_TYPE_LIB_INTERNAL_H_
