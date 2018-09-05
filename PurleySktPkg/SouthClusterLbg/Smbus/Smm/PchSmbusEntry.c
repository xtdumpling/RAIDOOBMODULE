/** @file
  PCH Smbus Driver Entry

@copyright
 Copyright (c) 2009 - 2015 Intel Corporation. All rights reserved
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
#include "PchSmbus.h"

SMBUS_INSTANCE  *mSmbusContext;

/**
  Execute an SMBUS operation

  @param[in] This                 The protocol instance
  @param[in] SlaveAddress         The address of the SMBUS slave device
  @param[in] Command              The SMBUS command
  @param[in] Operation            Which SMBus protocol will be issued
  @param[in] PecCheck             If Packet Error Code Checking is to be used
  @param[in, out] Length          Length of data
  @param[in, out] Buffer          Data buffer

  @retval EFI_SUCCESS             The SMBUS operation is successful
  @retval Other Values            Something error occurred
**/
EFI_STATUS
EFIAPI
SmbusExecute (
  IN      CONST EFI_SMBUS_HC_PROTOCOL   *This, 
  IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN      EFI_SMBUS_DEVICE_COMMAND      Command,
  IN      EFI_SMBUS_OPERATION           Operation,
  IN      BOOLEAN                       PecCheck,
  IN OUT  UINTN                         *Length,
  IN OUT  VOID                          *Buffer
  )
{
  InitializeSmbusRegisters ();

  return SmbusExec (
          SlaveAddress,
          Command,
          Operation,
          PecCheck,
          Length,
          Buffer
          );
}

/**
  <b>SMBus SMM Module Entry Point</b>\n
  - <b>Introduction</b>\n
    The SMBus module is a SMM driver that provides a standard way to execute an SMBus command.

  - @pre
    - EFI_SMM_BASE2_PROTOCOL
      - Documented in the System Management Mode Core Interface Specification 

  - @result
    The SMBus driver produces EFI_SMBUS_HC_PROTOCOL documented in the SMBus Host Controller Protocol Specification.

  - <b>Integration Checklist</b>\n
    - Verify prerequisites are met
    - Add Smbus\Smm\PchSmbus.inf to the platform DSC file.

  @param[in] ImageHandle          ImageHandle of this module
  @param[in] SystemTable          EFI System Table

  @retval EFI_SUCCESS             Driver initializes successfully
  @retval Other values            Some error occurred
**/
EFI_STATUS
EFIAPI
InitializePchSmbusSmm (
  IN      EFI_HANDLE            ImageHandle,
  IN      EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS  Status;

  ///
  /// Locate SMM Base Protocol
  ///

  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (SMBUS_INSTANCE), (VOID **) &mSmbusContext);

  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    ZeroMem (mSmbusContext, sizeof (SMBUS_INSTANCE));

    mSmbusContext->Signature                  = PCH_SMBUS_PRIVATE_DATA_SIGNATURE;
    mSmbusContext->IoDone                     = IoDone;
    mSmbusContext->SmbusIoRead                = SmbusIoRead;
    mSmbusContext->SmbusIoWrite               = SmbusIoWrite;
    mSmbusContext->SmbusController.Execute    = SmbusExecute;
    mSmbusContext->SmbusController.ArpDevice  = SmbusArpDevice;
    mSmbusContext->SmbusController.GetArpMap  = SmbusGetArpMap;
    mSmbusContext->SmbusController.Notify     = SmbusNotify;

    ///
    /// Install the SMBUS interface
    ///
    Status = gSmst->SmmInstallProtocolInterface (
                      &mSmbusContext->Handle,
                      &gEfiSmmSmbusProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mSmbusContext->SmbusController
                      );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
