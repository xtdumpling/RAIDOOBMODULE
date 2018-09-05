
/** @file
  This library abstracts read/write access for basic type data those values may be
  stored into the different media. 
  
Copyright (c) 2012 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.  

**/

#ifndef _SETUP_LIB_INTERNAL_H_
#define _SETUP_LIB_INTERNAL_H_

#include <Library/SetupLib.h>
GROUP_INFO mSetupInfo[] = {
  {&gEfiSocketIioVariableGuid,                     SOCKET_IIO_CONFIGURATION_NAME,               sizeof(SOCKET_IIO_CONFIGURATION)},
  {&gEfiSocketCommonRcVariableGuid,                SOCKET_COMMONRC_CONFIGURATION_NAME,          sizeof(SOCKET_COMMONRC_CONFIGURATION)},
  {&gEfiSocketMpLinkVariableGuid,                  SOCKET_MP_LINK_CONFIGURATION_NAME,           sizeof(SOCKET_MP_LINK_CONFIGURATION)},
  {&gEfiSocketMemoryVariableGuid,                  SOCKET_MEMORY_CONFIGURATION_NAME,            sizeof(SOCKET_MEMORY_CONFIGURATION)},
  {&gEfiSocketPowermanagementVarGuid,              SOCKET_POWERMANAGEMENT_CONFIGURATION_NAME,   sizeof(SOCKET_POWERMANAGEMENT_CONFIGURATION)},
  {&gEfiSocketProcessorCoreVarGuid,                SOCKET_PROCESSORCORE_CONFIGURATION_NAME,     sizeof(SOCKET_PROCESSORCORE_CONFIGURATION)},
  {&gEfiSetupVariableGuid,                         PLATFORM_SETUP_VARIABLE_NAME,                sizeof(SYSTEM_CONFIGURATION)},
  {&gEfiPchRcVariableGuid,                         PCH_RC_CONFIGURATION_NAME,                   sizeof(PCH_RC_CONFIGURATION)},
#if defined(ME_SUPPORT_FLAG) && ME_SUPPORT_FLAG
  {&gEfiMeRcVariableGuid,                          ME_RC_CONFIGURATION_NAME,                    sizeof(ME_RC_CONFIGURATION)},
#endif // ME_SUPPORT_FLAG
#ifdef IE_SUPPORT
  {&gEfiIeRcVariableGuid,                          IE_RC_CONFIGURATION_NAME,                    sizeof(IE_RC_CONFIGURATION)},
#endif //IE_SUPPORT
  {&gFpgaSocketVariableGuid,                       FPGA_SOCKET_CONFIGURATION_NAME,              sizeof(FPGA_SOCKET_CONFIGURATION)},
  {NULL,                                           NULL}
};

#endif
