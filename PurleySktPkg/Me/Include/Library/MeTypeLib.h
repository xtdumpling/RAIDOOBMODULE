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

  MeTypeLib.h

@brief:

  HECI Library

**/
#ifndef _ME_TYPE_LIB_H_
#define _ME_TYPE_LIB_H_

#include "Library/MeTypeDefs.h"

UINT8      MeTypeGet (VOID);
BOOLEAN    MeTypeIsDfx (VOID);
BOOLEAN    MeTypeIsDisabled (VOID);
BOOLEAN    MeTypeIsSps(VOID);
BOOLEAN    MeTypeIsSpsReady(VOID);
BOOLEAN    MeTypeIsSpsInRecovery (VOID);
BOOLEAN    MeTypeIsAmtInRecovery (VOID);
BOOLEAN    MeTypeIsInRecovery (VOID);
BOOLEAN    MeTypeIsSpsNm(VOID);
BOOLEAN    MeTypeIsAmt(VOID);
#if defined(AMT_SUPPORT) && AMT_SUPPORT
BOOLEAN    MeTypeIsAmtReady(VOID);
BOOLEAN    MeTypeIsCorporateAmt (VOID);
BOOLEAN    MeTypeIsCorporateAmtReady (VOID);
#endif // AMT_SUPPORT
VOID       MeTypeShowDebug(CHAR16 *);
UINT32     GetMeFs1FromHob(UINT32 Mefs1);

#endif // _ME_TYPE_LIB_H_
