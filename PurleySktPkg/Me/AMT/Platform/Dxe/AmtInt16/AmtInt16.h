/** @file
  Header file for Amt INT16 support.

@copyright
 Copyright (c) 2011 - 2016 Intel Corporation. All rights reserved
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

#ifndef _AMT_INT16_H
#define _AMT_INT16_H

#include <Base.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/AmtWrapperProtocol.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/FirmwareVolume2.h>

#define AMT_INT16_CSM_GUID \
    {0x6046e678, 0x24ef, 0x4005, 0xba, 0x39, 0xbd, 0xa1, 0x1f, 0x6d, 0x55, 0x5d}

EFI_GUID  gAmtInt16CsmGuid  = AMT_INT16_CSM_GUID;

#endif
