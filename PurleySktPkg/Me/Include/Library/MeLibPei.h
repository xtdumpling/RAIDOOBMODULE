/** @file
  This is a library for ME functionality.

@copyright
  Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/


#ifndef __ME_LIB_PEI_
#define __ME_LIB_PEI_

#include <Ppi/MeSpsPolicyPei.h>

/**
  This function performs basic initialization for ME
  in PEI phase after memory is initialized.
**/
VOID
EFIAPI
MePostMemInit (
  VOID
  );

#ifndef AMT_SUPPORT
/**
  ME End of PEI callback function. Configure HECI devices on End Of Pei
**/
VOID
MeOnEndOfPei (
  VOID
  );
#endif // ~AMT_SUPPORT

#endif // __ME_LIB_PEI_
  