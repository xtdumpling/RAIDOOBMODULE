//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  2007 - 2012 Intel Corporation. All rights reserved
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

#ifndef _LT_DXE_LIB_H_
#define _LT_DXE_LIB_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Guid/PlatformTxt.h>

UINTN
GetSinitAddress (
  VOID
  );


EFI_STATUS
GetBiosAcmErrorReset (
  UINT8 *BiosAcmErrorResetBit
  );


EFI_STATUS
GetAcmType (
  UINT8 *AcmTypeValue
  );

EFI_STATUS
SetAcmType (
  IN UINT8 AcmTypeValue
  );


EFI_STATUS
GetLockChipset (
  UINT8 *LockChipsetBit
  );


EFI_STATUS
GetEVMode (
  UINT8 *EVModeBit
  );


EFI_STATUS
SetLockChipset (
  IN UINT8 LockChipsetValue
  );


EFI_STATUS
SetEVMode (
  IN UINT8 EVModeValue
  );

EFI_STATUS
GetProcessorLtsxEnable (
  UINT8 *ProcessorLtsxEnableBit
  );


EFI_STATUS
SetDisableLtsx (
  VOID
  );


VOID
SetDisableSoftwareSmi (
  VOID
  );

EFI_STATUS
SetupLtDeviceMemory (
  IN LT_DXE_LIB_CONTEXT *LtDxeCtx
  );

// APTIOV_SERVER_OVERRIDE_RC_START : Moved later to get ACM type to get BiosAcmAddress from PlatformTxtPolicyData
EFI_STATUS
FindAcmBinaryType(
  IN LT_DXE_LIB_CONTEXT *LtDxeCtx
  );
// APTIOV_SERVER_OVERRIDE_RC_END : Moved later to get ACM type to get BiosAcmAddress from PlatformTxtPolicyData

#endif //_LT_DXE_LIB_H_
