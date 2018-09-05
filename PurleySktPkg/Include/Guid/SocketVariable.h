//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SocketVariable.h

  Data format for Socket Data Structure

**/
#ifndef   __SOCKET_CONFIG_DATA_H__
#define   __SOCKET_CONFIG_DATA_H__

#include <UncoreCommonIncludes.h>
#include "SocketConfiguration.h"
#include <Guid/SocketIioVariable.h>
#include <Guid/SocketCommonRcVariable.h>
#include <Guid/SocketPowermanagementVariable.h>
#include <Guid/SocketProcessorCoreVariable.h>
#include <Guid/SocketMpLinkVariable.h>
#include <Guid/SocketMemoryVariable.h>

#pragma pack(1)

typedef struct {
  SOCKET_IIO_CONFIGURATION       IioConfig;
  SOCKET_COMMONRC_CONFIGURATION  CommonRcConfig;
  SOCKET_MP_LINK_CONFIGURATION   CsiConfig;
  SOCKET_MEMORY_CONFIGURATION    MemoryConfig;
  SOCKET_POWERMANAGEMENT_CONFIGURATION PowerManagementConfig;
  SOCKET_PROCESSORCORE_CONFIGURATION   SocketProcessorCoreConfiguration;
} SOCKET_CONFIGURATION;



#pragma pack()
#endif

