/*++ @file
  Contains proprietary oem code executed in smm

@copyright
  Copyright (c) 2014 Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/

#ifndef _PCH_OEM_SMM_LIB_H__
#define _PCH_OEM_SMM_LIB_H__

/**
  Allows OEM to add proprietary code before go to Sx state

  @param[in] None

  @retval NONE
**/
VOID
OemSmmSxPrepareToSleep(
  VOID
  );

#endif

