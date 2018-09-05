/** @file
  Header file for the MrcOemHooksPeim that produces the MRC_OEM_HOOKS_PPI.

@copyright
 Copyright (c) 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _MRC_OEM_HOOKS_PEIM_H_
#define _MRC_OEM_HOOKS_PEIM_H_

//
// Below are header files that are to be cleaned up. Since Silicon(Mrc/Iio/Kti) side 
// should expose only 3 or less header file to be included by platform
//
#include "DataTypes.h"
#include "MemHost.h"
#include "KtiHost.h"
#include "KtiMisc.h"
#include "SysHost.h"
#include "MemAddrMap.h"
#include "KtiLib.h"

//
// Below are normal include
//
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/OemIioInit.h>

#include <Ppi/MrcOemHooksPpi.h>
#include <Library/OemProcMemInit.h>

#endif // _MRC_OEM_HOOKS_PEIM_H_
