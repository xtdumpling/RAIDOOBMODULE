/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2007 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file HeciInit.c

  HECI Initialization DXE Driver

**/

#include <Uefi.h>
#include "HeciInit.h"
#include <PchAccess.h>
#if defined(AMT_SUPPORT) && AMT_SUPPORT
#include <Library/HobLib.h>
#include "MeBiosPayloadHob.h"
#include "Hecidrv.h"
#include <Library/MmPciBaseLib.h>
#include "MkhiMsgs.h"
#include <Library/HobLib.h>
#include "MeBiosPayloadData.h"
#include <Library/HeciMsgLib.h>
#include "MeChipset.h"
#include "CommonIncludes.h"
#include <Protocol/Smbios.h>

extern EFI_GUID gEfiShellEnvironment2Guid;
#endif // AMT_SUPPORT

#include "HeciRegs.h"
#include "Library/MeTypeLib.h"
#include "Guid/HiiSetupEnter.h"
#include "MeAccess.h"
#include "IndustryStandard/Pci.h"

//
// Global driver data
//
HECI_INSTANCE *mHeciContext = NULL;


/**

    GC_TODO: add routine description

    @param Event   - GC_TODO: add arg description
    @param Context - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
ExecuteOnSetupEnter (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  DEBUG ((EFI_D_INFO, "HECI: ExecuteOnSetupEnter\n"));
  //
  // Check if it's DFX ME FW first
  //
  if (MeTypeIsDfx() || MeTypeIsDisabled()) {
    return EFI_SUCCESS;
  }


  return EFI_SUCCESS;
}

#if defined(AMT_SUPPORT) && AMT_SUPPORT
/**

    GC_TODO: add routine description

    @param ImageHandle - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description
    @retval EFI_NOT_STARTED       - GC_TODO: add retval description

**/
EFI_STATUS
HeciInstallForAMT (
  IN EFI_HANDLE ImageHandle
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (MeTypeIsAmt()) {
    ME_BIOS_PAYLOAD_HOB   *MbpHob = NULL;

    if (mHeciContext == NULL) {
      DEBUG ((EFI_D_ERROR, "HECI: mHeciContext is NULL in HeciInstallForAMT()\n"));
      return EFI_INVALID_PARAMETER;
    }

    //
    // Get the MBP Data.
    //
    MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
    if (MbpHob != NULL) {
      mHeciContext->MeFwImageType = (UINT8) MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType;
    } else {
       DEBUG ((DEBUG_ERROR, "ERROR: HeciInstallForAMT: No MBP Hob available\n"));
    }

    //
    // Initialize the Me Reference Code Information
    //
    mHeciContext->MeInfo.Revision   = ME_INFO_PROTOCOL_REVISION_1;
    mHeciContext->MeInfo.RCVersion  = ME_RC_VERSION;


    //
    // Install the Me Reference Code Information
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiMeInfoProtocolGuid,
                  &mHeciContext->MeInfo,
                  NULL
                  );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "HECI: Can't install gEfiMeInfoProtocolGuid. Not critical, continue.\n"));
    }


    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    MeScriptSaveEventFunction,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &mHeciContext->EfiPmEvent
                    );
    ASSERT_EFI_ERROR (Status);

    if (mHeciContext->EfiPmEvent == NULL) {
      ASSERT(mHeciContext->EfiPmEvent != NULL);
      return EFI_NOT_STARTED;
    }

    //
    // Create events to send EOP message
    // 1. on ExitBootServices event
    // 2. on LegacyBoot event
    // 2. on enter EfiShell using gEfiShellEnvironment2Guid
    //    /ExitBootServices is not called on EfiShell enter/
    //
    DEBUG ((EFI_D_ERROR, "HECI install ReadyToBoot handler\n"));
    Status = EfiCreateEventReadyToBootEx (
                                          TPL_CALLBACK,
                                          MeReadyToBootEventFunction,
                                          (VOID *) &ImageHandle,
                                          &mHeciContext->ReadyToBootEvent
                                         );
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return Status;
    }

  }

  return Status;
}
#endif

/**

    GC_TODO: add routine description

    @param Status      - GC_TODO: add arg description
    @param ImageHandle - GC_TODO: add arg description

    @retval None

**/
void
HeciOnErrorCleanup (
  IN EFI_STATUS   Status,
  IN EFI_HANDLE   ImageHandle
  )
{
  DEBUG ((EFI_D_ERROR, "HECI: Error occurred (%r). Starting HECI cleaning sequence.\n", Status));

  if ((mHeciContext != NULL) &&
      EFI_ERROR(Status)) {
    HECI_PROTOCOL     *HeciCtlr;

    // gHeciProtocolGuid
    Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  &HeciCtlr
                  );
    if (!EFI_ERROR(Status)) {
#if defined(AMT_SUPPORT) && AMT_SUPPORT
      if (!MeTypeIsAmt())
#endif // AMT_SUPPORT
      {
        Status = gBS->UninstallMultipleProtocolInterfaces (
                ImageHandle,
                &gHeciProtocolGuid,
                HeciCtlr,
                NULL
             );
        DEBUG ((EFI_D_INFO, "\t Removing gHeciProtocolGuid (%r)\n", Status));
      }
    }

#if defined(AMT_SUPPORT) && AMT_SUPPORT
    if (MeTypeIsAmt()) {
      EFI_ME_INFO_PROTOCOL    *MeInfo;

      Status = gBS->LocateProtocol (
                    &gEfiMeInfoProtocolGuid,
                    NULL,
                    &MeInfo
                    );
      if (!EFI_ERROR(Status)) {
        Status = gBS->UninstallMultipleProtocolInterfaces (
                      ImageHandle,
                      &gEfiMeInfoProtocolGuid,
                      MeInfo,
                      NULL
                      );
        DEBUG ((EFI_D_INFO, "\t Removing gEfiMeInfoProtocolGuid (%r)\n", Status));
      }

      DEBUG ((EFI_D_INFO, "\t Removing HECI devices from PCI space\n"));
      DisableAllMeDevices ();

      // Events
      if (mHeciContext->ReadyToBootEvent) {
        gBS->CloseEvent (mHeciContext->ReadyToBootEvent);
        DEBUG ((EFI_D_INFO, "\t Closing Event ReadyToBootEvent\n"));
      }
      if (mHeciContext->EfiShellEvent) {
        gBS->CloseEvent (mHeciContext->EfiShellEvent);
        DEBUG ((EFI_D_INFO, "\t Closing Event EfiShellEvent\n"));
      }
      if (mHeciContext->EfiPmEvent) {
        gBS->CloseEvent (mHeciContext->EfiPmEvent);
        DEBUG ((EFI_D_INFO, "\t Closing Event EfiPmEvent\n"));
      }
      if (mHeciContext->LegacyBootEvent) {
        gBS->CloseEvent (mHeciContext->LegacyBootEvent);
        DEBUG ((EFI_D_INFO, "\t Closing Event LegacyBootEvent\n"));
      }
      if (mHeciContext->MpbClearEvent) {
        gBS->CloseEvent (mHeciContext->MpbClearEvent);
        DEBUG ((EFI_D_INFO, "\t Closing Event MpbClearEvent\n"));
      }
    }
#endif //AMT_SUPPORT
      if (mHeciContext->SetupEnterEvent) {
        gBS->CloseEvent (mHeciContext->SetupEnterEvent);
        DEBUG ((EFI_D_INFO, "\t Closing Event SetupEnterEvent\n"));
      }
#if defined(AMT_SUPPORT) && AMT_SUPPORT
      if (!MeTypeIsAmt()) {
#endif // AMT_SUPPORT
      DEBUG ((EFI_D_INFO, "\t Removing mHeciContext allocation\n"));
      FreePool(mHeciContext);
      mHeciContext = NULL;
#if defined(AMT_SUPPORT) && AMT_SUPPORT
    } else {
      mHeciContext->MeFwImageType = INTEL_ME_CONSUMER_FW;
    }
#endif // defined(AMT_SUPPORT) && AMT_SUPPORT
  }
}

EFI_STATUS
HeciInitializePrivate(VOID)
/**

    Determines if the HECI device is present and, if present, initializes it for
    use by the BIOS.

    @param None.

    @retval EFI_STATUS

**/
{
  HECI_HOST_CONTROL_REGISTER          HeciRegHCsr;
  VOLATILE HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;
  EFI_STATUS                          Status;
  UINTN                               HeciMBAR;

  Status = EFI_SUCCESS;

  do {
    //
    // Store HECI vendor and device information away
    //
    mHeciContext->DeviceInfo = HeciPciRead16 (PCI_DEVICE_ID_OFFSET);

    //
    // Check for HECI-1 PCI device availability
    //
    DEBUG((EFI_D_INFO, "[HECI] DeviceInfo: %2x, \n", mHeciContext->DeviceInfo));
    if (mHeciContext->DeviceInfo == 0xFFFF) {
      Status = EFI_DEVICE_ERROR;
      break;
    }
#if 0
  //
  // Disabling the code below. HECI transport driver should not analyze
  // ME status, it is not a job for transport layer.
  //
    //
    // Check for ME FPT Bad
    //
    DEBUG((EFI_D_INFO, "[HECI] ME R_FWSTATE : %2x, \n", HeciPciRead32 (R_FWSTATE)));
    if ((HeciPciRead32 (R_FWSTATE) & 0x0020) != 0) {
      Status = EFI_DEVICE_ERROR;
      break;
    }
    //
    // Check for ME error status
    //
    if ((HeciPciRead32 (R_FWSTATE) & 0xF000) != 0) {
      //
      //  Special Condition: ME_RECOVERY There is HECI
      //  MASK with 0x3000 means ME it is in Forced Recovery Mode. (OPR invalid )
      //
      if ((HeciPciRead32 (R_FWSTATE) & 0x3000) == 0)
      {
        //
        // ME failed to start so no HECI
        //
        DEBUG ((EFI_D_ERROR, "[HECI] ME RECOVERY but unknown error : %2x, \n", HeciPciRead32 (R_FWSTATE)));
        Status = EFI_DEVICE_ERROR;
        break;
      }
    }
#endif
    //
    // Store HECI revision ID
    //
    mHeciContext->RevisionInfo = HeciPciRead8 (PCI_REVISION_ID_OFFSET);

    //
    // Get HECI_MBAR and see if it is programmed to a useable value
    //
    HeciMBAR = HeciMbarRead();
    if (HeciMBAR == 0)
    {
      DEBUG((EFI_D_ERROR, "[HECI] MBAR not programmed in DXE phase, using default 0x%08X\n",
             HECI1_PEI_DEFAULT_MBAR));
      HeciMBAR = HECI1_PEI_DEFAULT_MBAR;
      HeciPciWrite32(R_HECIMBAR + 4, 0);
      HeciPciWrite32(R_HECIMBAR, HECI1_PEI_DEFAULT_MBAR | 4);
    }
    //
    // Set HECI interrupt delivery mode.
    // HECI-1 using legacy/MSI interrupt
    //
    HeciPciWrite8(R_HIDM, HECI_INTERRUPUT_GENERATE_LEGACY_MSI);
    //
    // Enable HECI BME and MSE
    //
    HeciPciWrite8(R_COMMAND, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);
    //
    // Need to do following on ME init:
    //
    //  1) wait for ME_CSR_HA reg ME_CSR_HA bit set
    //
    // if (WaitForMEReady (HECI1_DEVICE) != EFI_SUCCESS) {
    //   Status = EFI_TIMEOUT;
    //   break;
    // }
    //
    //  2) setup H_CSR reg as follows:
    //     a) Make sure H_RST is clear
    //     b) Set H_RDY
    //     c) Set H_IG
    //
    HeciRegHCsrPtr  = (VOID *) (UINTN) (HeciMBAR + H_CSR);
    HeciRegHCsr.ul  = HeciRegHCsrPtr->ul;
    if (HeciRegHCsrPtr->r.H_RDY == 0) {
      HeciRegHCsr.r.H_RST = 0;
      HeciRegHCsr.r.H_RDY = 1;
      HeciRegHCsr.r.H_IG  = 1;
      HeciRegHCsrPtr->ul  = HeciRegHCsr.ul;
    }
  } while (EFI_ERROR (Status));

  return Status;
}

/**

    GC_TODO: add routine description

    @param ImageHandle - GC_TODO: add arg description
    @param SystemTable - GC_TODO: add arg description

    @retval Status - GC_TODO: add retval description

**/
EFI_STATUS
EFIAPI
HeciDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS              Status = EFI_OUT_OF_RESOURCES;
  void                    *pSetupRegistration;
#if defined(AMT_SUPPORT) && AMT_SUPPORT
  EFI_STATUS              AmtStatus = EFI_UNSUPPORTED;
#endif // defined(AMT_SUPPORT) && AMT_SUPPORT

  DEBUG ((DEBUG_INFO, "HeciDxeEntryPoint () - Start\n"));

  mHeciContext = AllocateZeroPool (sizeof (HECI_INSTANCE));
  if (mHeciContext == NULL) {
     goto ErrExit;
  }

  mHeciContext->HeciDrvImageHandle = ImageHandle;
  //
  // Initialize HECI protocol pointers
  //
  mHeciContext->HeciCtlr.SendwACK     = HeciSendwAck;
  mHeciContext->HeciCtlr.ReadMsg      = HeciReceive;
  mHeciContext->HeciCtlr.SendMsg      = HeciSend;
  mHeciContext->HeciCtlr.ResetHeci    = HeciResetInterface;
  mHeciContext->HeciCtlr.InitHeci     = HeciInitialize;
  mHeciContext->HeciCtlr.MeResetWait  = HeciMeResetWait;
  mHeciContext->HeciCtlr.ReInitHeci   = HeciReInitialize;
  mHeciContext->HeciCtlr.GetMeStatus  = HeciGetMeStatus;
  mHeciContext->HeciCtlr.GetMeMode    = HeciGetMeMode;

  //
  // Initialize the HECI device
  //
  Status = HeciInitializePrivate ();
  if (EFI_ERROR (Status)) {
    goto ErrExit;
  }

  //
  // Install the HECI interface
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
              &ImageHandle,
              &gHeciProtocolGuid,
              &mHeciContext->HeciCtlr,
              NULL
           );
  if (EFI_ERROR (Status)) {
    goto ErrExit;
  }

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  ExecuteOnSetupEnter,
                  NULL,
                  &mHeciContext->SetupEnterEvent
                  );
  if (EFI_ERROR (Status)) {
    goto ErrExit;
  }

  Status = gBS->RegisterProtocolNotify (
                &gEfiSetupEnterGuid,
                mHeciContext->SetupEnterEvent,
                &pSetupRegistration
              );
  if (EFI_ERROR (Status)) {
    goto ErrExit;
  }
  DEBUG ((EFI_D_ERROR, "HECI Registered on Setup Enter procedure!\n"));

#if defined(AMT_SUPPORT) && AMT_SUPPORT
  AmtStatus = HeciInstallForAMT(ImageHandle);
  if (EFI_ERROR (AmtStatus)) {
    DEBUG ((EFI_D_ERROR, "Warning: AMT HECI features are not loaded, but leaving HECI driver\n"));
  }
#endif

ErrExit:
  if (EFI_ERROR (Status)) {
    HeciOnErrorCleanup(Status, ImageHandle);
  }
#if defined(AMT_SUPPORT) && AMT_SUPPORT
  if (EFI_ERROR (Status) || EFI_ERROR (AmtStatus)) {
    if (MeTypeIsAmt()) {
      EFI_SMBIOS_PROTOCOL *Smbios;
      VOID                *Registration;
      EFI_EVENT           Event;

      ///
      /// Install Smbios before disabling all ME devices.
      ///
      Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);

      if (!EFI_ERROR (Status)) {
        AddFwStsSmbiosEntry (NULL, NULL);
        DEBUG ((DEBUG_ERROR, "[HECI] Removing ME devices from config space!\n"));
        DisableAllMeDevices ();
      } else {
        Status = gBS->CreateEvent (
                        EVT_NOTIFY_SIGNAL,
                        TPL_CALLBACK,
                        AddFwStsSmbiosEntry,
                        NULL,
                        &Event
                        );
        ASSERT_EFI_ERROR (Status);

        if (!EFI_ERROR (Status)) {
          Status = gBS->RegisterProtocolNotify (
                          &gEfiSmbiosProtocolGuid,
                          Event,
                          &Registration
                          );
          ASSERT_EFI_ERROR (Status);
        }
      }
    }
  }
#endif // defined(AMT_SUPPORT) && AMT_SUPPORT

  return Status;
}
