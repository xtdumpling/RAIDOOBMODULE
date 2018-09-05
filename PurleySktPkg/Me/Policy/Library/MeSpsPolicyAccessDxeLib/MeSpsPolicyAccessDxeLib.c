/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2006 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MeSpsPolicyAccessDxeLib.c

@brief:
  Implementation file for Me SPS Policy functionality

**/

#include "Library/UefiBootServicesTableLib.h"
#include "Library/DebugLib.h"
#include <Protocol/MeSpsPolicyProtocol.h>

/**

  Get Sps Policy pointer

  @param[in] None.

**/
SPS_POLICY_PROTOCOL *
GetSpsPolicy (
  VOID
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  SPS_POLICY_PROTOCOL  *retDxeSpsPolicy;

  Status = gBS->LocateProtocol (&gSpsPolicyProtocolGuid, NULL, &retDxeSpsPolicy);
  if (EFI_ERROR(Status)) {
    retDxeSpsPolicy = NULL;
  }
  
  return retDxeSpsPolicy;
}