/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  MeHeci.h

Abstract:

  Definitions for HECI interface to ME.
  TODO: Currently this file is used by HECI-3 SMM driver only.
  TODO: The plan is to make it common core for all HECI functions.

--*/
#ifndef _ME_UMA_EINJ_LIB_H_
#define _ME_UMA_EINJ_LIB_H_

typedef enum
{
  MeEinjModeInstant  = 0,
  MeEinjModeDeffered = 0,
  MeEinjModeGoS1RW   = 1, 
  MeEinjModeOnReset  = 2,
  MeEinjModeMax
} ME_UMA_EINJ_MODE;


/**
 * Read ME UMA error injection page offset.
 *
 * param[out] pMeEinjPageOffset  Pointer to a buffer for ME error injection page offset
 *
 * return EFI_STATUS is returned.
 */
EFI_STATUS
EFIAPI
MeUmaEinjPageOffset(OUT UINT32 *pMeEinjPageOffset);


/**
 * Trigger ME UMA error injection page consumption.
 *
 * param[in] ImageHandle    Handle of driver image
 * param[in] pSysTable      Pointer to the system table
 *
 * return EFI_STATUS is returned.
 */
EFI_STATUS
EFIAPI
MeUmaEinjConsume(IN ME_UMA_EINJ_MODE EinMode);


#endif // _ME_UMA_EINJ_LIB_H_

