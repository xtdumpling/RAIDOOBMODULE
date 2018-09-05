/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/** @file

Copyright (c) 2011, Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module: 

  PCH Extended Reset Protocol

**/

#ifndef _EFI_PCH_EXTENDED_RESET_H_
#define _EFI_PCH_EXTENDED_RESET_H_

//BUGBUG This file was added in order to get LewisburgPkg to build with Purley. This needs to be looked at to make sure it's necessary.

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                                 gEfiPchExtendedResetProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_PCH_EXTENDED_RESET_PROTOCOL EFI_PCH_EXTENDED_RESET_PROTOCOL;

//
// Related Definitions
//
//
// PCH Extended Reset Types
//
typedef struct {
  UINT8 PowerCycleReset : 1;   // 0: Disabled*; 1: Enabled
  UINT8 GlobalReset : 1;  // 0: Disabled*; 1: Enabled
  UINT8 GlobalResetWithEc : 1;  // 0: Do Nothing;
                          // 1: GPIO[30](SUS_PWR_DN_ACK) level is set low prior to Global Reset(for systems with an embedded controller)
  UINT8 MaxRestReq : 5;     // Reserved fields for future expansion w/o protocol change
} PCH_EXTENDED_RESET_TYPES;

//
// Member functions
//
typedef
EFI_STATUS
(EFIAPI *EFI_PCH_EXTENDED_RESET) (
  IN     EFI_PCH_EXTENDED_RESET_PROTOCOL   * This,
  IN     PCH_EXTENDED_RESET_TYPES          PchExtendedResetTypes
  );

/**

  Execute Pch Extended Reset from the host controller.

  @param This                    - Pointer to the EFI_PCH_EXTENDED_RESET_PROTOCOL instance.
  @param PchExtendedResetTypes   - Pch Extended Reset Types which includes PowerCycle, Globalreset.

  @retval Does not return if the reset takes place.
  @retval EFI_INVALID_PARAMETER   - If ResetType is invalid.

**/

//
// Interface structure for the Pch Extended Reset Protocol
//
struct _EFI_PCH_EXTENDED_RESET_PROTOCOL {
  EFI_PCH_EXTENDED_RESET  Reset;
};

#endif
