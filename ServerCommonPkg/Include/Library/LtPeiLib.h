//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  2007 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  LtPeiLib.h

Abstract:

  GUID used for Platform Txt information in the HOB list.

--*/

#ifndef _LT_PEI_LIB_H_
#define _LT_PEI_LIB_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Ppi/MpService.h>

EFI_STATUS
GetLockChipset (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN UINT8                        *LockChipsetBit
  );


EFI_STATUS
GetEVMode (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN UINT8                        *EVModeBit
  );


EFI_STATUS
GetProcessorLtsxEnable (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN UINT8                        *ProcessorLtsxEnableBit
  );


EFI_STATUS
GetPeiMpServices (
  IN      CONST EFI_PEI_SERVICES  **PeiServices,
  OUT     PEI_MP_SERVICES_PPI      **PeiMpServices
  );

#endif //_LT_PEI_LIB_H_
