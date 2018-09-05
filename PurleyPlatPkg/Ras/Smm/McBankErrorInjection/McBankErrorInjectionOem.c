/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2014 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file McBankErrorInjectionOem.c

    This file is to be update by OEM.

---------------------------------------------------------------------------**/


#include "McBankErrorInjection.h"

extern UINT8  mSetupMcejSupport;

/*
 Access setup variable and updates the mSetupMcejSupport variable from setup data.

@param none 

@retval none
*/
VOID
SetupInterface(
  )
{
  EFI_STATUS              Status;

  // This variable is updated by setup option value of McbankErrorInjection
  mSetupMcejSupport = MCEJ_SUPPORT_DIS;

  Status = gSmst->SmmLocateProtocol (
                    &gEfiPlatformRasPolicyProtocolGuid,
                    NULL,
                    &mPlatformRasPolicyProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }
  
  mRasTopology = mPlatformRasPolicyProtocol->EfiRasSystemTopology;
  mRasSetup = mPlatformRasPolicyProtocol->SystemRasSetup;
  mRasCapability = mPlatformRasPolicyProtocol->SystemRasCapability;  
  
  if(mRasSetup->ErrInj.McaBankErrInjEn) {
    mSetupMcejSupport = MCEJ_SUPPORT_EN;
  }

}


/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2014 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

---------------------------------------------------------------------------**/
