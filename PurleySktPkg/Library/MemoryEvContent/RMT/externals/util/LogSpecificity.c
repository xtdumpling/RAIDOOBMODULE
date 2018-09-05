/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
@copyright
  Copyright (c) 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  LogSpecificity.c

@brief
  This file contains a function to log the margin parameter specificity.
**/
#ifdef SSA_FLAG

#include "LogSpecificity.h"

/**
@brief
  This function is used to log the given specificity value.

  @param[in, out]  SsaServicesHandle  Pointer to SSA services.
  @param[in]       Specificity        Specificity.

  @retval  Nothing.
**/
VOID LogSpecificity(
  IN OUT SSA_BIOS_SERVICES_PPI *SsaServicesHandle,
  IN MARGIN_PARAM_SPECIFICITY Specificity)
{
  BOOLEAN SomethingLogged = FALSE;

  if (Specificity & PlatformSpecific) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "Platform");
    SomethingLogged = TRUE;
  }

  if (Specificity & SocketSpecific) {
    if (SomethingLogged) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "|");
    }
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "Socket");
    SomethingLogged = TRUE;
  }

  if (Specificity & ControllerSpecific) {
    if (SomethingLogged) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "|");
    }
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "Controller");
    SomethingLogged = TRUE;
  }

  if (Specificity & ChannelSpecific) {
    if (SomethingLogged) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "|");
    }
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "Channel");
    SomethingLogged = TRUE;
  }

  if (Specificity & RankSpecific) {
    if (SomethingLogged) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "|");
    }
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "Rank");
    SomethingLogged = TRUE;
  }

  if (Specificity & StrobeSpecific) {
    if (SomethingLogged) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "|");
    }
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "Strobe");
    SomethingLogged = TRUE;
  }

  if (Specificity & LaneSpecific) {
    if (SomethingLogged) {
      NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "|");
    }
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "Lane");
    SomethingLogged = TRUE;
  }

  if (!SomethingLogged) {
    NORMAL_LOG(SsaServicesHandle, SSA_D_ERROR, "None");
  }
} // end function LogSpecificity


#endif  // SSA_FLAG

// end file LogSpecificity.c
