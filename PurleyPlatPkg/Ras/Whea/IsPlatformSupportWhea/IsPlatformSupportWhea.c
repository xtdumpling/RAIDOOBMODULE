/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c) 2009-2015 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file IsPlatformSupportWhea.c

  IsPlatformSupportWhea Protocol driver, should be ported for platform.

-------------------------------------------------------------------------------**/

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>


#include <Guid/SetupVariable.h>
#include <Protocol/IsPlatformSupportWhea.h>
#include <Library/SetupLib.h>

BOOLEAN
IsSystemSupportWhea (
  IN EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL          *This
  );

STATIC
EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL  IsPlatformSupportWheaInst = \
{
    WheaNotSupported,
    0,
    &IsSystemSupportWhea
};


/**

  This function will install IsPlatformSupportWhea Protocol if the platform supports Whea.
  The IsPlatformSupportWhea will be consumed by core whea module 

  @param ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  @param SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table        

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded 
  @retval EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
InstallIsPlatformSupportWhea(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_HANDLE                  Handle;
  UINT8                       WheaSupportEn;
  UINT8                       WheaErrInjEn;
  IsPlatformSupportWheaInst.WheaSupport = WheaNotSupported;

  //
  // Get the whea setup variable from setup.
  //
  //If Whea Support is not enabled, just return EFI_SUCCESS;
  //If yes, install EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL for WHEA core module to consume

  Status = GetOptionData (&gEfiSetupVariableGuid,  OFFSET_OF(SYSTEM_CONFIGURATION, WheaSupportEn), &WheaSupportEn, sizeof(UINT8));

  if( EFI_ERROR(Status)) {
    WheaSupportEn = 0;
  }
  Status = GetOptionData (&gEfiSetupVariableGuid,  OFFSET_OF(SYSTEM_CONFIGURATION, WheaErrInjEn), &WheaErrInjEn, sizeof(UINT8));
  if( EFI_ERROR(Status)) {
    WheaErrInjEn = 0;
  }
  //DEBUG((DEBUG_ERROR, "IsPlatformSupportWhea. wheaData.WheaSupport == %x \n",SetupData.WheaSupport));
  
  // if Whea is disabled, return Success
  if (WheaSupportEn == 0) {
    return EFI_SUCCESS;
  } else if (WheaSupportEn == 1) {
    // WHEA EINJ 5.0 setup
    if(WheaErrInjEn)	IsPlatformSupportWheaInst.WheaAcpi5Support  = TRUE;
    else    IsPlatformSupportWheaInst.WheaAcpi5Support  = FALSE;

    IsPlatformSupportWheaInst.WheaSupport = WheaSupported;
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                      &Handle,
                      &gEfiIsPlatformSupportWheaProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &IsPlatformSupportWheaInst
                      );
    DEBUG((DEBUG_ERROR, "IsPlatformSupportWhea. InstallProtocolInterface.Status = %x \n",Status));
  }

  return Status;
}

/**

    This function will return Boolean status indicating whether the platform wants to enable WHEA support or not.

    @param This - Protocol instance pointer.

    @retval FALSE - Returns FALSE if WHEA support to be disabled 
    @retval TRUE  - Returns TRUE if WHEA support to be enabled 

**/
BOOLEAN
IsSystemSupportWhea (
  IN EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL          *This
  )
{
  if(This->WheaSupport == WheaNotSupported) {
      return FALSE;
  }
  
  return TRUE;
}
