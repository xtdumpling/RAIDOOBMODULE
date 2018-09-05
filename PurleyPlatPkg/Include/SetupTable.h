/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2012-2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SetupTable.h

Abstract:

--*/

#ifndef _SETUP_TABLE_H_
#define _SETUP_TABLE_H_
#include <Guid/SocketVariable.h>
#include <Guid/SetupVariable.h>
#if ME_SUPPORT_FLAG
#include <Guid/MeRcVariable.h>
#endif // ME_SUPPORT_FLAG

#ifdef IE_SUPPORT
#include <Guid/IeRcVariable.h>
#endif //IE_SUPPORT

#include <Guid/PchRcVariable.h>
#include <Guid/FpgaSocketVariable.h>

typedef struct {
  SOCKET_CONFIGURATION       SocketConfig;
  SYSTEM_CONFIGURATION       SystemConfig;
  PCH_RC_CONFIGURATION       PchRcConfig;
#if ME_SUPPORT_FLAG
  ME_RC_CONFIGURATION        MeRcConfig;
#endif // ME_SUPPORT_FLAG
#ifdef IE_SUPPORT
  IE_RC_CONFIGURATION        IeRcConfig;
#endif //IE_SUPPORT

  FPGA_SOCKET_CONFIGURATION  SocketFpgaConfig;  
// APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
} INTEL_SETUP_DATA;
// APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error

#endif
