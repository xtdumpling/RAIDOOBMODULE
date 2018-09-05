/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2010 -2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file HeciInit.h

  Framework PEIM to provide Heci.

**/
#ifndef _HECI_INIT_H_
#define _HECI_INIT_H_

#include <PiPei.h>

//
// Prototypes
//

/**

  The entry point of the HECI PEIM. Installs HECI PPI interface.

  @param FileHandle  Handle of the file being invoked.
  @param PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS   Indicates that HECI initialization completed successfully.

**/
EFI_STATUS
HeciPeimEntryPoint (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
;

#endif // _HECI_INIT_H_

