/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef _FPKSETUP_FPKCOMMON_H
#define _FPKSETUP_FPKCOMMON_H

#define FPK_NUM_SUPPORTED_PORTS 4

#define EFI_ADAPTER_INFO_FPK_FUNC_ENABLE_DISABLE_GUID { 0xE6A009ED, 0xFDA8, 0x4902, { 0x85, 0x0B, 0x97, 0x55, 0xA3, 0x4A, 0x1F, 0xCA } }
#define FPK_FUNC_DISABLE_ALLOW_FUNC_DIS 0x01
#define FPK_FUNC_DISABLE_ALLOW_PORT_DIS 0x02

typedef struct {
  UINT8 AllowFuncDis[4];
} EFI_ADAPTER_INFO_FPK_FUNC_DISABLE;

#endif // !_FPKSETUP_FPKCOMMON_H
