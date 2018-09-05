//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioCommonRcConfigData.h

  Data format for Universal Data Structure

**/
#ifndef   __SOCKET_COMMONRC_CONFIG_DATA_H__
#define   __SOCKET_COMMONRC_CONFIG_DATA_H__


#include <UncoreCommonIncludes.h>
#include "SocketConfiguration.h"

extern EFI_GUID gEfiSocketCommonRcVariableGuid;
#define SOCKET_COMMONRC_CONFIGURATION_NAME L"SocketCommonRcConfig"

#pragma pack(1)
typedef struct {
    //
    //  Common Section of RC
    //
    UINT32   MmiohBase;
    UINT16   MmiohSize;
    UINT8    MmcfgBase;
    UINT8    MmcfgSize;
    UINT8    IsocEn;
    UINT8    NumaEn;
    UINT8    MirrorMode;
    UINT8    LockStep;
    UINT8    CpuStepping;
    UINT8    SystemRasType;
} SOCKET_COMMONRC_CONFIGURATION;
#pragma pack()

#endif


