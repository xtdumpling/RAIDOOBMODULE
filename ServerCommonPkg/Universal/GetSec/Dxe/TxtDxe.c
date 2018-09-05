/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

  Copyright (c) 2007 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


    @file LtDxe.c

  This is the main DXE file for LT.  It represents an abstract outline of the
  steps required during DXE for enabling LT.  Each individual step is further
  abstracted behind a function call interface.  This is intended to minimize
  the need to modify this file when porting LT to future platforms.

**/

#include "TxtDxeLib.h"
#include "TxtDxeProtocol.h"
#include <Library/LtDxeLib.h>
#include <Library/PlatformHooksLib.h> //for IsSimicsPlatform()

LT_DXE_LIB_CONTEXT          mLtDxeCtx ;
EFI_LEGACY_REGION2_PROTOCOL *mLegacyRegion = NULL;
BOOLEAN                    ScheckDone = FALSE;
BOOLEAN                     LtDxeEnabled = TRUE;
BOOLEAN                     DisableBIOSACMFlag = FALSE;

//
// PURLEY_OVERRIDE_BEGIN
//
/*
VOID WA_UnLock()
{
  UINT32              RegEax;
  UINT8               SteppingId;

  //
  // Do nothing if >= JKT-C1 stepping
  //
  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  SteppingId = (UINT8) (RegEax & 0xF);

  if (SteppingId > 5) return;
  if (mLegacyRegion) {
    //
    // Unlock Legacy region segment to let LockConfig not to block legacy boot
    //
    mLegacyRegion->UnLock (mLegacyRegion,
                           LEGACY_BIOS_MEMORY_SEGMENT,
                           LEGACY_BIOS_MEMORY_LENGTH,
                           NULL
                           );
  }
}
*/
//
// PURLEY_OVERRIDE_BEGIN
//

/**

  This function gets registered as a callback to run the SCHECK function
  from the LT BIOS ACM as a result of Boot Events.

  @param Event     - A pointer to the Event that triggered the callback.
  @param Context   - A pointer to private data registered with the callback function.

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
EFIAPI
ScheckCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS          Status;
  UINT32              Data32;
  UINT8               BiosAcmErrorResetBit;
  UINT8               AcmTypeValue;

/*
  if (IsSimicsPlatform()) {
    DEBUG ((EFI_D_ERROR, "LT w/a for Simics - Skip ScheckCallback\n"));
//    return EFI_SUCCESS ;
     while (Context){};
  }
*/

  DEBUG ((EFI_D_INFO, "DXE DoScheck ...\n"));
  DoScheck(&mLtDxeCtx) ;
  DEBUG ((EFI_D_INFO, "DoScheck Done!\n"));

//
// PURLEY_OVERRIDE_BEGIN
//
//  WA_UnLock();
//
// PURLEY_OVERRIDE_END
//
// APTIOV_SERVER_OVERRIDE_RC_START
    PrintLtsxErrors();   // TXT Error and status Register message printing.
// APTIOV_SERVER_OVERRIDE_RC_END

  // If LT_SPAD_HIGH 0xFED300A0 bit63 = 0, do legacy boot
  Data32 = *(volatile UINT32 *)(UINTN)LT_SPAD_HIGH;
  DEBUG ((EFI_D_INFO, "LT_SPAD_HIGH (0xFED300A4): %r\n", Data32));

  if ( (BIT31 & (*(volatile UINT32 *)(UINTN)LT_SPAD_HIGH)) == 0 ) {
    // BIOS ACM DoScheck error
    DEBUG ((EFI_D_ERROR, "Bios Acm DoScheck Failed. Reboot in non-ltsx mode\n"));

    // When BIOS ACM error occurs, If BiosAcmErrorReset setup option is enabled,
    // Disable LT-SX and reset system
    Status = GetBiosAcmErrorReset (&BiosAcmErrorResetBit);

    if ( (Status != EFI_SUCCESS) || BiosAcmErrorResetBit) {
      DisableLtsxAndReset();
    }

    // When BIOS ACM error occurs, If BiosAcmErrorReset setup option is disabled,
    // Ignore BIOS ACM errors and continue booting with no further BIOSACM being called
    //
    else {
      Status = GetAcmType (&AcmTypeValue);

      if (Status != EFI_SUCCESS) {
        DEBUG ((EFI_D_ERROR, "ERROR: No LtDxe Lib file, no platform funtions available\n"));
      } else {
        DEBUG ((EFI_D_ERROR, "ERROR: IGNORING ACM ERROR - ACM Type %x detected  - BIOS setup option is set to ignore ACM error\n",
          AcmTypeValue));
      }

      gBS->CloseEvent(Event);
      return EFI_SUCCESS;
    }
  }

  DEBUG ((EFI_D_INFO, "DoLockConfig ...\n"));
  TxtLockConfigForAll(&mLtDxeCtx) ;
  DEBUG ((EFI_D_INFO, "DoLockConfig Done!\n"));

  // If LT_SPAD_HIGH 0xFED300A0 bit63 = 0, do legacy boot
  //
  Data32 = *(volatile UINT32 *)(UINTN)LT_SPAD_HIGH;
  DEBUG ((EFI_D_INFO, "LT_SPAD_HIGH (0xFED300A4): %r\n", Data32));

  if ( (BIT31 & (*(volatile UINT32 *)(UINTN)LT_SPAD_HIGH)) == 0 ) {

    // BIOS ACM DoScheck error
    //
    DEBUG ((EFI_D_ERROR, "Bios Acm LockConfig Failed. Reboot in non-ltsx mode\n"));

    // When BIOS ACM error occurs, If BiosAcmErrorReset setup option is enabled,
    // Disable LT-SX and reset system
    //
    Status = GetBiosAcmErrorReset(&BiosAcmErrorResetBit);

    if ( (Status != EFI_SUCCESS) || BiosAcmErrorResetBit) {
      DisableLtsxAndReset();
    }

    // When BIOS ACM error occurs, If BiosAcmErrorReset setup option is disabled,
    // Ignore BIOS ACM errors and continue booting with no further BIOSACM being called
    //
    else {
      Status = GetAcmType (&AcmTypeValue);

      if (Status != EFI_SUCCESS) {
        DEBUG ((EFI_D_ERROR, "ERROR: No LtDxe Lib file, no platform funtions available\n"));
      } else {
        DEBUG ((EFI_D_ERROR, "ERROR: IGNORING ACM ERROR - ACM Type %x detected  - BIOS setup option is set to ignore ACM error\n",
          AcmTypeValue));
      }

      gBS->CloseEvent(Event);
      return EFI_SUCCESS;
    }
  }
  return EFI_SUCCESS ;
}

/**

  Lock MSR for this thread.

  @param None

  @retval EFI_SUCCESS           - MSR Locked

**/
EFI_STATUS
LockLTPatch (
  IN  EFI_MP_SERVICES_PROTOCOL  *MpService

  )
{
  UINT64              LtCtrl;

  //
  // Set LT_CONTROL_LOCK
  //
  LtCtrl = 0x1;
  AsmWriteMsr64 (EFI_MSR_LT_CONTROL, LtCtrl);

  return EFI_SUCCESS;
}

/**

  Lock LT MSR.

  @param None

  @retval EFI_SUCCESS           - Multiple processors re-load microcode patch

**/
EFI_STATUS
LockLtMsr (
  VOID
  )
{
  EFI_STATUS              Status;

  LockLTPatch (mLtDxeCtx.MpService);
  ASSERT (mLtDxeCtx.MpService);
  Status = mLtDxeCtx.MpService->StartupAllAPs (
                        mLtDxeCtx.MpService,
                        LockLTPatch,
                        FALSE,//OVERRIDE_HSD_5371612
                        NULL,
                        0,
                        (VOID *)mLtDxeCtx.MpService,
                        NULL
                        );

  return EFI_SUCCESS;
}

/**

  Callback function to lock down chipset after all chipset config is done

  @param EFI_HANDLE                                   RootBridgeHandle
  @param EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS  PciAddress
  @param EFI_PCI_ENUMERATION_PHASE                    Phase
  @param EFI_PCI_CALLBACK_CONTEXT                     *Context

  @retval EFI_SUCCESS           - Multiple processors re-load microcode patch

**/
VOID
EFIAPI
LTLockEventNotify (
  IN  EFI_HANDLE                                   RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS  PciAddress,
  IN  EFI_PCI_ENUMERATION_PHASE                    Phase,
  IN  EFI_PCI_CALLBACK_CONTEXT                     *Context
  )
{
  EFI_STATUS    Status;
  UINT8         LockChipsetBit;
  UINT8         EVModeBit = 1;

  if (!ScheckDone) {
    //
    // Only lock when disabled if LockChipset is set and EVMode is disabled
    // LtDxe Lib functions in platform must be installed to read LockChipset and EVMode bits
    //
    Status = GetLockChipset (&LockChipsetBit);

    if (Status == EFI_SUCCESS) {
      Status = GetEVMode (&EVModeBit);
    }

    if (LtDxeEnabled || (!LtDxeEnabled && (Status == EFI_SUCCESS) && LockChipsetBit && !EVModeBit) )
      LockLtMsr();

    if (LtDxeEnabled) {
        ScheckCallback (0, NULL);
    }
    ScheckDone = TRUE;
  }
}

/**

  This is the entry point to the LT DXE Driver.  This routine checks to see if
  the platform should be configured for LT and if so, configures the platform
  by reserving and initializing LT Configuration Space and LT Device Memory and
  registering a callback to run SCHECK from the LT BIOS ACM prior to boot.

  If the platform should not be configured for LT, this routine checks the
  establishment bit in the TPM and resets it if it is asserted.

  @param ImageHandle       - A handle for this module
  @param SystemTable       - A pointer to the EFI System Table

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
EFIAPI
DriverEntry (
  IN      EFI_HANDLE          ImageHandle,
  IN      EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS                  Status;
  UINT32                      Data32;
  EFI_PCI_CALLBACK_PROTOCOL   *PciCallBackProtocol;
  UINT8                       BiosAcmErrorResetBit;
  UINT8                       AcmTypeValue;
// APTIOV_SERVER_OVERRIDE_RC_START
// OVERRIDE_HSD_4757591_BEGIN
//  EFI_PHYSICAL_ADDRESS        TXTPublicBase  = TXT_PUBLIC_PRIVATE_BASE; //Added using sdl token
// OVERRIDE_HSD_4757591_END
//Moved later to get ACM type to get BiosAcmAddress from PlatformTxtPolicyData
#if 0
  //
  // Find ACM Binary type to Suppress "BIOS ACM Error Reset" setup option, if PW ACM is present.
  //
  Status = FindAcmBinaryType();

  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "No LtDxeLib available to execute platform functions\n"));
    DEBUG ((EFI_D_ERROR, "LtDxe driver cannot load\n"));
    return (EFI_UNSUPPORTED);
  }
#endif
// APTIOV_SERVER_OVERRIDE_RC_END
  //
  // Register for protocol notifications, because ReadyToBoot is too late
  // Need to lock LT config MSR whether it is enabled or not
  //
  Status = gBS->LocateProtocol ( 
                 &gEfiPciCallbackProtocolGuid, 
                 NULL, 
                 &PciCallBackProtocol
                 );
  ASSERT_EFI_ERROR(Status); 

  Status = PciCallBackProtocol->RegisterPciCallback(
                         PciCallBackProtocol,
                         LTLockEventNotify,
                         EfiPciEnumerationResourceAssigned
                         );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "Register for Scheck/LockConfig Callback\n"));

  //
  // Initialize the LT DXE Lib functions
  //
  Status = InitializeLtDxeLib (ImageHandle, SystemTable, &mLtDxeCtx);
  if (EFI_ERROR (Status)) {
    LtDxeEnabled = FALSE;
    DEBUG ((EFI_D_ERROR, "InitializeLtDxeLib Error: %r\n", Status));
    return EFI_SUCCESS;
  }
// APTIOV_SERVER_OVERRIDE_RC_START
  Status = FindAcmBinaryType(&mLtDxeCtx);
// APTIOV_SERVER_OVERRIDE_RC_END : Moved later to get ACM type to get BiosAcmAddress from PlatformTxtPolicyData
  //
  // Determine TPM presense. If not present - nothing to do.
  // Nothing but presense is precondition of this module since all
  // commands executed by BIOS ACM don't depend on TPM state.
  // TPM_NV_read will be successfully executed even if TPM is disabled
  // and/or deactivated because all indices defined for BIOS ACM
  // usage don't require authorization. TPM_ResetEstablishmentBit
  // doesn't depend on TPM state at all and can
  // be executed with disabled/deactivated TPM always.
  // Case when TPM is comletely not functional is not cinsidered.
  //
#ifndef SOFTSDV_FLAG
  if (!IsTpmPresent (&mLtDxeCtx)) {
    DEBUG ((EFI_D_ERROR, "IsTpmPresent Error\n"));
#ifdef MKF_TPM_PRESENT
    LtDxeEnabled = FALSE;
    return EFI_SUCCESS;
#endif
  }
#endif

  //
  // Detect LT capable Processor
  //
  if (!IsLtProcessor (&mLtDxeCtx)) {
    DEBUG ((EFI_D_ERROR, "LtDxe Error: Not LT processor\n"));
    LtDxeEnabled = FALSE;
    return EFI_SUCCESS ;
  }

  Status = gBS->LocateProtocol (
                 &gEfiLegacyRegion2ProtocolGuid,
                 NULL,
                 &mLegacyRegion
                 );
  ASSERT_EFI_ERROR(Status);

  //
  // If LT is enabled, configure platform appropriately
  //
#ifdef MKF_TPM_PRESENT
  if (IsLtEnabled (&mLtDxeCtx)) {
#else
  if (1) {
#endif

    // If LT_SPAD_HIGH 0xFED300A0 bit63 = 0, do legacy boot
    //
    Data32 = *(volatile UINT32 *)(UINTN)LT_SPAD_HIGH;
    DEBUG ((EFI_D_INFO, "LT_SPAD_HIGH (0xFED300A4): %r\n", Data32));

    if ( (BIT31 & (*(volatile UINT32 *)(UINTN)LT_SPAD_HIGH)) == 0 ) {

      // BIOS ACM error
      //
      DEBUG ((EFI_D_ERROR, "Bios Acm Failed. Reboot in non-ltsx mode\n"));

      Status = GetBiosAcmErrorReset (&BiosAcmErrorResetBit);

      if ( (Status == EFI_SUCCESS) && BiosAcmErrorResetBit) {
        DisableLtsxAndReset();
      } else {
        Status = GetAcmType (&AcmTypeValue);

        if (Status == EFI_SUCCESS) {
          DEBUG ((EFI_D_ERROR, "ERROR: IGNORING ACM ERROR - ACM Type %x detected  - BIOS setup option is set to ignore ACM error\n",
            AcmTypeValue));
        } else {
          DEBUG ((EFI_D_ERROR, "ERROR: No LtDxe Lib functions installed  - BIOS setup option is set to ignore ACM error\n"));
        }

        DisableBIOSACMFlag = TRUE;
      }
    }

    //
    // Initialize LT Device Memory
    //
    Status = SetupLtDeviceMemory (&mLtDxeCtx);

// APTIOV_SERVER_OVERRIDE_RC_START : This will be done in SDL file.
#if 0
    //
    // OVERRIDE_HSD_4757591_BEGIN
    //
    //
    // Add TPM Decode and TXT Public and Private memory region as Runtime memory 
    // so that it can be reported in E820 table
    //
    Status = gDS->AddMemorySpace (
                    EfiGcdMemoryTypeMemoryMappedIo,
                    TXTPublicBase,
                    TXT_PUBLIC_PRIVATE_SIZE,
                    EFI_MEMORY_RUNTIME | EFI_MEMORY_UC
                    );
    ASSERT_EFI_ERROR(Status);

    Status = gDS->AllocateMemorySpace (
                    EfiGcdAllocateAddress,
                    EfiGcdMemoryTypeMemoryMappedIo,
                    0,
                    TXT_PUBLIC_PRIVATE_SIZE,
                    &TXTPublicBase,
                    ImageHandle,
                    NULL
                    );
    ASSERT_EFI_ERROR(Status);

    Status = gDS->SetMemorySpaceAttributes (
                    TXTPublicBase,
                    TXT_PUBLIC_PRIVATE_SIZE,
                    EFI_MEMORY_RUNTIME | EFI_MEMORY_UC
                    );
    ASSERT_EFI_ERROR (Status);
    //
    // OVERRIDE_HSD_4757591_END
    //
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : This will be done in SDL file 
  } else {
    Status = EFI_UNSUPPORTED;
  }

  //--------------------------------------------------------------------------------------------
  // Install TXT DXE Protocol so that functions in TXT DXE Driver can be used by other modules
  if (Status == EFI_SUCCESS) {					// If successful so far
    DEBUG ((EFI_D_ERROR, "Install TXT_DXE_PROTOCOL..."));
    Status = InstallTxtDxeProtocol (ImageHandle, SystemTable);	//   Install TXT DXE Protocol
    if (Status == EFI_SUCCESS) {				//     If success
      DEBUG ((EFI_D_ERROR, "Success\n"));
    } else {							//     If Error
      DEBUG ((EFI_D_ERROR, "Error\n"));
    }
  }
  //--------------------------------------------------------------------------------------------

  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Lt not enabled\n"));

    //
    // LT is not enabled, so make sure TPM Establishment
    // bit is de-asserted
    //
    if (IsLtEstablished (&mLtDxeCtx)) {
      Status = ResetTpmEstBit (&mLtDxeCtx) ;
      ASSERT_EFI_ERROR (Status) ;
    }
  }

  return EFI_SUCCESS ;
}
