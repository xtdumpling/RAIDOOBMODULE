/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  1999 - 2017   Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioSecurePlatform.c

  Implement IIO initialization protocol to provide IIO initialization functions.
  This interfaces will be called by PciHostBridge driver in the PCI bus enumeration.

**/

#include <SysFunc.h>
#include <Library/IioUtilityLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Protocol/MpService.h>
#include <Protocol/IioSystem.h>
#include <Cpu/CpuRegs.h>
#include <RcRegs.h>
#include <IioSetupDefinitions.h>
#include <Library/S3BootScriptLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START : Changes done to resolve the Legacy Boot hang Issue when PAM LOCK BIT set
#include <Protocol/LegacyBiosExt.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Changes done to resolve the Legacy Boot hang Issue when PAM LOCK BIT set

extern EFI_GUID gEfiEventExitBootServicesGuid;

BOOLEAN LtConfLockDone = FALSE;

/**

  Lock MSR for this thread.

  @param None

  @retval EFI_SUCCESS           - MSR Locked

**/
EFI_STATUS
LockFeatureControl (
  IN  EFI_MP_SERVICES_PROTOCOL  *mMpService
  )
{
  UINT32                    MsrValue;

  MsrValue = 0;
  MsrValue = AsmReadMsr32(EFI_MSR_IA32_FEATURE_CONTROL);
  //
  // Set MSR (0x3A) lock bit0 if it is not set.
  //
  if((MsrValue & B_EFI_MSR_IA32_FEATURE_CONTROL_LOCK) == 0) {
    MsrValue |= B_EFI_MSR_IA32_FEATURE_CONTROL_LOCK;
    AsmWriteMsr32(EFI_MSR_IA32_FEATURE_CONTROL, MsrValue);
  }
  return EFI_SUCCESS;
}

/**

  Lock IA32_FEATURE_CONTROL MSR.

  @param None

  @retval EFI_SUCCESS           - Locked IA32_FEATURE_CONTROL MSR on all threads

**/
EFI_STATUS
LockMsr (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_MP_SERVICES_PROTOCOL  *mMpService;
  UINTN                     ProcessorNumber;

  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  &mMpService
                  );

  ASSERT_EFI_ERROR (Status);

  Status = mMpService->WhoAmI(mMpService, &ProcessorNumber);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "ProcessorMsrFeatureControlLock in IioLateInitialize \n"));

  LockFeatureControl (mMpService);
  ASSERT (mMpService);
  Status = mMpService->StartupAllAPs (
                        mMpService,
                        LockFeatureControl,
                        FALSE,
                        NULL,
                        0,
                        (VOID *)mMpService,
                        NULL
                        );

  return EFI_SUCCESS;
}

/**

  Iio IioOnEndOfDxe callback function

  @param Event   - The instance of the event triggering this function
  @param Context - Pointer to Context, not used

  @retval None

**/
VOID
EFIAPI
IioOnEndOfDxe (
  IN   EFI_EVENT    Event,
  IN   VOID         *Context
  )
{
  EFI_STATUS                    Status;
  EFI_IIO_SYSTEM_PROTOCOL       *IioSystemProtocol = NULL;
  IIO_GLOBALS                   *IioGlobalData = NULL;

  //
  // Locate Iio System Protocol to be used to check Setup value for lock control
  //
  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &IioSystemProtocol);
  if (EFI_ERROR(Status))
    return;

  IioGlobalData = IioSystemProtocol->IioGlobalData;

  //
  // MSR (0X3A) bit0 IA32_FEATURE_CONTROL lock
  //
  if ((!LtConfLockDone) && (IioGlobalData->SetupData.ProcessorMsrLockControl)) {
      LockMsr();
      LtConfLockDone = TRUE;
  }

  return;
}

/**

 Function that writes all the CSRs and MSRs defined in the SeCoE
 BIOS recommendations document that need to be done late in the
 boot process. These are all done as a ready to boot event.


// ******************   WARNING  **************************************
// *******  Settings within this function must not be changed *********
// *******  without reviewing the changes with SeCoE or the   *********
// *******  TXT BIOS owner.  Changes to this function will    *********
// *******  produce corresponding failures to the ACM Lock    *********
// *******  Configuration Checks and will prevent Trusted     *********
// *******  Boot.                                             *********
// ********************************************************************
// ********************************************************************


    @param IioGlobalData - Ptr to IIO_GLOBALS

    @retval None

**/
VOID
TxtLateSecurePlatform (
  IN  IIO_GLOBALS    *IioGlobalData
  )
{
  UINT8                       IioIndex;
  PAM0123_CHABC_SAD_STRUCT    Pam0123;

  DEBUG ((EFI_D_INFO, "IioInit Late Secure the Platform (TXT)..\n"));

  for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++) {
    if (!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]){
      continue;
    }

    //
    // HSD 4166062: Add New register lock bit that would Lock down PAM configuration
    // HSD 4166542: Implement CONFIG_LOCK as per SeCoE recommendation
    //
    Pam0123.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, 0, PAM0123_CHABC_SAD_REG);
    Pam0123.Bits.lock = 1;
    IioWriteCpuCsr32(IioGlobalData, IioIndex, 0, PAM0123_CHABC_SAD_REG, Pam0123.Data);

  } // End of for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++)
}

/**

  Routine to read and store the PAM registers to the S3 boot script

 @param EFI_EVENT                 Event

 @param VOID                      *Context

 @retval None
*/
VOID
IioPamRegSave(
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  EFI_STATUS                  Status;
  UINT8                       IioIndex;
  PAM0123_CHABC_SAD_STRUCT    Pam0123;
  PAM456_CHABC_SAD_STRUCT     Pam456;
  LTDPR_IIO_VTD_STRUCT        Ltdpr;
  EFI_IIO_SYSTEM_PROTOCOL     *IioSystemProtocol = NULL;
  IIO_GLOBALS                 *IioGlobalData = NULL;
  EFI_CPU_CSR_ACCESS_PROTOCOL *CpuCsrAccess;
  UINT8                       Size;
  UINT64                      RegAddr;
  //
  // Need to get the IioGlobalData
  //
  Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &IioSystemProtocol);
  if (EFI_ERROR(Status))
    return;

  IioGlobalData = IioSystemProtocol->IioGlobalData;
  CpuCsrAccess = (EFI_CPU_CSR_ACCESS_PROTOCOL *)IioGlobalData->IioVar.IioOutData.PtrAddress.Address64bit;

  for (IioIndex = 0; IioIndex < MaxIIO; IioIndex++) {
    if (!IioGlobalData->IioVar.IioVData.SocketPresent[IioIndex]){
      continue;
    }
    //
    // Need to save PAM register settings to be restored on S3 resume.
    //
    Pam0123.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, 0, PAM0123_CHABC_SAD_REG);
    Ltdpr.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, 0, LTDPR_IIO_VTD_REG);
    Pam456.Data = IioReadCpuCsr32(IioGlobalData, IioIndex, 0, PAM456_CHABC_SAD_REG);

    //
    // Save lock bit for restore, but do not set PAM lock since we are not at runtime yet
    //
    if ((IioGlobalData->SetupData.LockChipset) && (!IioGlobalData->IioVar.IioVData.EVMode)) {
      Pam0123.Bits.lock = 1;
      Ltdpr.Bits.lock = 1;
    }

    RegAddr = CpuCsrAccess->GetCpuCsrAddress(IioIndex, 0, PAM456_CHABC_SAD_REG, &Size);
    S3BootScriptSaveMemWrite(S3BootScriptWidthUint32, RegAddr, 1, &Pam456.Data);
      
    RegAddr = CpuCsrAccess->GetCpuCsrAddress(IioIndex, 0, PAM0123_CHABC_SAD_REG, &Size);
    S3BootScriptSaveMemWrite(S3BootScriptWidthUint32, RegAddr, 1, &Pam0123.Data);

    IioWriteCpuCsr32(IioGlobalData, IioIndex, 0, LTDPR_IIO_VTD_REG, Ltdpr.Data);
  }
}

/**

    This function is to do TXT locks on ExitBootServices Event

    @param Event   - Pointer to EFI_EVENT
    @param Context - Pointer to recover function Context

    @retval None

**/
VOID
EFIAPI
OnExitBootServices (
      EFI_EVENT                 Event,
      VOID                      *Context
  )
{
    EFI_STATUS Status;
    EFI_IIO_SYSTEM_PROTOCOL       *IioSystemProtocol = NULL;
    IIO_GLOBALS                   *IioGlobalData = NULL;

    Status = gBS->LocateProtocol (&gEfiIioSystemProtocolGuid, NULL, &IioSystemProtocol);
    if (EFI_ERROR(Status))
      return;

    DEBUG ((EFI_D_INFO, "\nOnExitBootServices..\n"));

    IioGlobalData = IioSystemProtocol->IioGlobalData;

    //
    //
    // HSD 4168370 Add EVMode Support to the HSX EP BIOS
    //
    // Late TXT Secure Platform Locks
    //
    if ((IioGlobalData->SetupData.LockChipset) && (!IioGlobalData->IioVar.IioVData.EVMode)) {
      TxtLateSecurePlatform(IioGlobalData);
    }
}

/**

  Call back routine used to do all IIO post PCie port configuration, initialization

  @param IioGlobalData  - Pointer to IIO_GLOBALS

  @retval None
**/
EFI_STATUS
IioSecurePlatform (
  IIO_GLOBALS  *IioGlobalData
  )
{
  VOID            *Registration;
  EFI_STATUS      Status;
  EFI_EVENT       mExitBootServicesEvent;
  EFI_EVENT       mLegacyEvent;
  EFI_EVENT       ProcessorLocks;
  // APTIOV_SERVER_OVERRIDE_RC_START : Changes done to resolve the Legacy Boot hang Issue when PAM LOCK BIT set
  VOID            *EventRegistration;
  // APTIOV_SERVER_OVERRIDE_RC_END : Changes done to resolve the Legacy Boot hang Issue when PAM LOCK BIT set
  //
  // Save PAM registers for S3 restore just before Script lock
  //
  Status = gBS->CreateEventEx(
      EVT_NOTIFY_SIGNAL,
      TPL_CALLBACK,
      IioPamRegSave,
      NULL,
      &gEfiEndOfDxeEventGroupGuid,
      &Registration
      );
  ASSERT_EFI_ERROR(Status);

  //
  // Register IioOnEndOfDxe callback for IIO driver to lock FEATURE_CONTROL MSR
  //
  Status = gBS->CreateEventEx (
     EVT_NOTIFY_SIGNAL,
     TPL_CALLBACK,
     IioOnEndOfDxe,
     NULL,
     &gEfiEndOfDxeEventGroupGuid,
     &ProcessorLocks
     );
  ASSERT_EFI_ERROR(Status);
  // APTIOV_SERVER_OVERRIDE_RC_START : Changes done to resolve the Legacy Boot hang Issue when PAM LOCK BIT set
  #if 0
  //
  // Register the event to inform PAM locking proc variable that it is at runtime.
  //
  Status = EfiCreateEventLegacyBootEx (
             TPL_NOTIFY,
             OnExitBootServices,
             NULL,
             &mLegacyEvent
             );
  ASSERT_EFI_ERROR(Status);
  #endif
   
	//
	// Register for protocol notifactions
	//

    Status = gBS->CreateEvent (
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                OnExitBootServices,
                NULL,
                &mLegacyEvent
                );
    ASSERT_EFI_ERROR (Status);
    
    //
    // Register for protocol notifications on this event
    //
    
    Status = gBS->RegisterProtocolNotify (
                &gAmiLegacyBootProtocolGuid,
                mLegacyEvent,
                &EventRegistration
                );
    
    ASSERT_EFI_ERROR (Status);
    
  // APTIOV_SERVER_OVERRIDE_RC_END : Changes done to resolve the Legacy Boot hang Issue when PAM LOCK BIT set

  //
  // Also need to register the callback for UEFI boot
  //
  Status = gBS->CreateEventEx (
              EVT_NOTIFY_SIGNAL,
              TPL_NOTIFY,
              OnExitBootServices,
              NULL,
              &gEfiEventExitBootServicesGuid,
              &mExitBootServicesEvent
              );

  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


