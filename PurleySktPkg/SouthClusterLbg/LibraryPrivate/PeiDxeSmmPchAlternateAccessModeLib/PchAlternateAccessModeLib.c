/** @file
  PCH alternate access mode library.
  All function in this library is available for PEI, DXE, and SMM,
  But do not support UEFI RUNTIME environment call.

@copyright
 Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <PchAccess.h>

/**
  This function can be called to enable/disable Alternate Access Mode
  @note, the alternate access mode does not support to read NMI_EN# bit.

  @param[in] AmeCtrl              If TRUE, enable Alternate Access Mode.
                                  If FALSE, disable Alternate Access Mode.

**/
VOID
EFIAPI
PchAlternateAccessMode (
  IN  BOOLEAN       AmeCtrl
  )
{
  UINT32  Data32Or;
  UINT32  Data32And;

  Data32Or  = 0;
  Data32And = 0xFFFFFFFF;

  if (AmeCtrl == TRUE) {
    ///
    /// Enable Alternate Access Mode
    ///
    Data32Or  = (UINT32) (B_PCH_PCR_ITSS_GIC_AME);
  }

  if (AmeCtrl == FALSE) {
    ///
    /// Disable Alternate Access Mode
    ///
    Data32And = (UINT32) ~(B_PCH_PCR_ITSS_GIC_AME);
  }

  ///
  /// Program Alternate Access Mode Enable bit
  ///
  MmioAndThenOr32 (
    PCH_PCR_ADDRESS (PID_ITSS, R_PCH_PCR_ITSS_GIC),
    Data32And,
    Data32Or
  );
}
