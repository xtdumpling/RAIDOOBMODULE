/** @file
  Active Management Technology Protocol to return the state of ASF Boot Options
  related to Active Management Technology.

@copyright
 Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
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
#ifndef _ACTIVE_MANAGEMENT_PROTOCOL_H_
#define _ACTIVE_MANAGEMENT_PROTOCOL_H_

#include <Protocol/AlertStandardFormat.h>

///
/// Intel Active Management Technology Protocol
/// This protocol provides interface to get ASF boot options status
///

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gActiveManagementProtocolGuid;

#define DXE_ACTIVE_MANAGEMENT_PROTOCOL_REVISION_1  1
//
// Forward reference for ANSI C compatibility
//
typedef struct _ACTIVE_MANAGEMENT_PROTOCOL  ACTIVE_MANAGEMENT_PROTOCOL;

///
/// Protocol definitions
///

/**
  Return current state of Boot Options
  @param[in] This                 Pointer to the EFI_ACTIVE_MANAGEMENT_PROTOCOL instance.
  @param[in] CurrentState         TRUE when the boot options is enabled

  @retval EFI_SUCCESS             Command succeed.
**/
typedef
EFI_STATUS
(EFIAPI *ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT BOOLEAN                          *CurrentState
);

/**
   This will return Storage Redirection boot device to boot

  @param[in] This                 The address of protocol
  @param[in] IdeBootDevice        Return the boot device number to boot
                                  Bit 1  Bit0
                                     0    0    USBr is connected to CD/DVD device 
                                     0    1    USBr is connected to floppy device 
                                  Bits 2-7: Reserved set to 0

  @retval EFI_SUCCESS             The function completed successfully.
**/
typedef
EFI_STATUS
(EFIAPI *ACTIVE_MANAGEMENT_STORAGE_REDIR_BOOT_DEVICE_SELECTED) (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT UINT8                            *IdeBootDevice
);

/**
  Return current ASF Boot Options

  @param[in] This                 Pointer to the EFI_ACTIVE_MANAGEMENT_PROTOCOL instance.
  @param[in] AsfBootOptions       ASF Boot Options

  @retval EFI_SUCCESS             Command succeed.
**/
typedef
EFI_STATUS
(EFIAPI *ACTIVE_MANAGEMENT_ASF_BOOT_OPTIONS_GET) (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT ASF_BOOT_OPTIONS                 **AsfBootOptions
);

/**
  This will return verbosity request option

  @param[in] CurrentState         Return the state of verbosity option

  @retval EFI_SUCCESS             The function completed successfully.
  @retval CurrentState            00 - No BootOption available
                                  01 - Non-Verbosity request
                                  02 - Verbosity request
**/
typedef
EFI_STATUS
(EFIAPI *ACTIVE_MANAGEMENT_VERBOSITY_REQUESTED) (
  IN     ACTIVE_MANAGEMENT_PROTOCOL       *This,
  IN OUT UINT8                            *CurrentState
);

///
/// Intel Active Management Technology Protocol
/// It provides abstract level function of ASF boot options defined in ASF 2.0 specification for
/// other modules to get ASF boot options status.
///
struct _ACTIVE_MANAGEMENT_PROTOCOL {
  ///
  /// Revision for the protocol structure
  ///
  UINT8                                                Revision;
  ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE                 GetStorageRedirState;
  ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE                 GetEnforceSecureBootState;
  ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE                 GetSolState;
  ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE                 GetRemoteFlashState;
  ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE                 GetBiosSetupState;
  ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE                 GetBiosPauseState;
  ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE                 GetConsoleLockState;
  ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE                 GetKvmState;
  ACTIVE_MANAGEMENT_STORAGE_REDIR_BOOT_DEVICE_SELECTED GetStorageRedirBootDeviceSelected;
  ACTIVE_MANAGEMENT_ASF_BOOT_OPTIONS_GET               GetAsfBootOptions;
  ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE                 GetProgressMsgRequest;
  ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE                 GetSecureEraseState;
};

#endif
