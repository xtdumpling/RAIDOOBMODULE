//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/**
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      KTI Reference Code                                                *
// *                                                                        *
// *      ESS - Enterprise Silicon Software                                 *
// *                                                                        *
// *      Copyright (c) 2004 - 2015 Intel Corp.                             *
// *                                                                        *
// *      This program has been developed by Intel Corporation.             *
// *      Licensee has Intel's permission to incorporate this source code   *
// *      into their product, royalty free.  This source code may NOT be    *
// *      redistributed to anyone without Intel's written permission.       *
// *                                                                        *
// *      Intel specifically disclaims all warranties, express or           *
// *      implied, and all liability, including consequential and other     *
// *      indirect damages, for the use of this code, including liability   *
// *      for infringement of any proprietary rights, and including the     *
// *      warranties of merchantability and fitness for a particular        *
// *      purpose.  Intel does not assume any responsibility for any        *
// *      errors which may appear in this code nor any responsibility to    *
// *      update it.                                                        *
// *                                                                        *
// **************************************************************************
// **************************************************************************
// *                                                                        *
// *  PURPOSE:                                                              *
// *                                                                        *
// *      This file contains platfrom specific routines which might need    *
// *      porting for a particular platform.                                *
// *                                                                        *
// **************************************************************************
**/

#ifndef _KTI_COH_H_
#define _KTI_COH_H_

#include "DataTypes.h"
#include "PlatformHost.h"
#include "SysHost.h"


KTI_STATUS
ProgramMeshCredits (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL   *KtiInternalGlobal
  );

KTI_STATUS
ProgramMeshCredits1S (
  struct sysHost      *host,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

KTI_STATUS
ProgramMeshCreditsCommonSkx (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  KTI_HOST_INTERNAL_GLOBAL  *KtiInternalGlobal
  );

VOID
MCAErrWa (
  struct sysHost      *host,
  KTI_SOCKET_DATA     *SocketData,
  BOOLEAN             Mask
  );

#endif // _KTI_COH_H_
