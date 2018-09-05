/** @file
  ME End Of Post message and process implementation prior to boot OS

@copyright
  Copyright (c) 2015 - 2016 Intel Corporation. All rights reserved
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
#if defined(AMT_SUPPORT) && AMT_SUPPORT

#include <Base.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MeShowBufferLib.h>
#include <IndustryStandard/Pci22.h>
#include <Library/PerformanceLib.h>
#include <Pi/PiMultiPhase.h>
#include <Pi/PiHob.h>
#include <Library/HobLib.h>
#include <BupMsgs.h>
#include <MeState.h>
#include <HeciRegs.h>
#include <MeiBusMsg.h>
#include <IncludePrivate/PchPolicyHob.h>
#include <Library/PchP2sbLib.h>
#include <Library/PchInfoLib.h>

#include <Library/MmPciBaseLib.h>
#include <PchPolicyCommon.h>
#include <Library/UefiBootServicesTableLib.h>
#include "MeChipset.h"
#include "Protocol/HeciProtocol.h"
#include "Heci/Library/HeciCoreLib/HeciCore.h"
#include "Library/HeciCoreLib.h"
#include "Library/DxeMeLib.h"
#include <IncludePrivate/Library/PchInitCommonLib.h>

#define MAX_EOP_SEND_RETRIES          0x2


/**
  Send End of Post Request Message through HECI.

  @param[in] RequestedActions     Action request returned by EOP ACK
                                  0x00 (HECI_EOP_STATUS_SUCCESS) - Continue to boot
                                  0x01 (HECI_EOP_PERFORM_GLOBAL_RESET) - Global reset

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
HeciSendEndOfPostMessage (
  OUT UINT32                      *RequestedActions
  )
{
  EFI_STATUS                      Status;
  UINT32                          HeciSendLength;
  UINT32                          HeciRecvLength;
  GEN_END_OF_POST_ACK             CbmEndOfPost;
  HECI_PROTOCOL                   *Heci;
  UINT32                          MeMode;

  DEBUG ((DEBUG_INFO, "[ME] HeciSendEndOfPostMessage.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }

  Status = Heci->GetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }

  CbmEndOfPost.Header.Data              = 0;
  CbmEndOfPost.Header.Fields.Command    = GEN_END_OF_POST_CMD;
  CbmEndOfPost.Header.Fields.IsResponse = 0;
  CbmEndOfPost.Header.Fields.GroupId    = MKHI_GEN_GROUP_ID;
  CbmEndOfPost.Data.RequestedActions    = 0;

  HeciSendLength                        = sizeof (MKHI_MESSAGE_HEADER);
  HeciRecvLength                        = sizeof (GEN_END_OF_POST_ACK);

  Status = Heci->SendMsg (
            HECI1_DEVICE,
            (UINT32 *) &CbmEndOfPost,
            HeciSendLength,
            BIOS_FIXED_HOST_ADDR,
            HECI_CORE_MESSAGE_ADDR
            );

  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciSendEndOfPostMessage: message failed! - %r\n", Status));
    return Status;
  }

  Status = Heci->ReadMsg (
            HECI1_DEVICE,
            BLOCKING,
            (UINT32 *) &CbmEndOfPost,
            &HeciRecvLength
            );

  if (!EFI_ERROR (Status)){
    if ((((GEN_END_OF_POST_ACK*)&CbmEndOfPost)->Header.Fields.IsResponse != 1) ||
        (((GEN_END_OF_POST_ACK*)&CbmEndOfPost)->Header.Fields.GroupId != MKHI_GEN_GROUP_ID) ||
        (((GEN_END_OF_POST_ACK*)&CbmEndOfPost)->Header.Fields.Result != 0) ||
        (((GEN_END_OF_POST_ACK*)&CbmEndOfPost)->Header.Fields.Command != GEN_END_OF_POST_CMD)) {
      DEBUG ((DEBUG_ERROR, "[ME] No ACK for sent message.\n"));
      return EFI_DEVICE_ERROR;
    }
    *RequestedActions = CbmEndOfPost.Data.RequestedActions;
    if (CbmEndOfPost.Data.RequestedActions == HECI_EOP_PERFORM_GLOBAL_RESET) {
      DEBUG ((DEBUG_ERROR, "HeciSendEndOfPostMessage(): Global Reset requested by FW EOP ACK\n"));
    }
  }
  else {
    DEBUG ((DEBUG_ERROR, "[ME] Error: Unable to Read GEN_END_OF_POST_ACK Result - %r\n", Status));
  }
  return Status;
}

/**
  This message is sent by the BIOS if EOP-ACK not received to force ME to disable
  HECI interfaces.


  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             HECI interfaces disabled by ME
**/
EFI_STATUS
HeciDisableHeciBusMsg (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT32                          Length;
  UINT32                          RespLength;
  HECI_BUS_DISABLE_CMD_ACK        MsgHeciBusDisable;
  HECI_PROTOCOL                   *Heci;

  DEBUG ((DEBUG_INFO, "[ME] HeciDisableHeciBusMsg.\n"));
  Status = gBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  (VOID **) &Heci
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] Unable to Locate Heci Protocol.- %r\n", Status));
    return Status;
  }
  ZeroMem(&MsgHeciBusDisable, sizeof(HECI_BUS_DISABLE_CMD_ACK));


  MsgHeciBusDisable.Command.Data = HECI_BUS_DISABLE_OPCODE;
  Length     = sizeof (HECI_BUS_DISABLE_CMD);
  RespLength = sizeof (HECI_BUS_DISABLE_CMD_ACK);
  Status = Heci->SendwACK (
            HECI1_DEVICE,
            (UINT32 *) &MsgHeciBusDisable,
            Length,
            &RespLength,
            BIOS_FIXED_HOST_ADDR,
            HECI_MEI_BUS_MESSAGE_ADDR
            );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[ME] HeciDisableHeciBusMsg: message failed! - %r\n", Status));
    return Status;
  }

  if (MsgHeciBusDisable.Command.Data != HECI_BUS_DISABLE_ACK_OPCODE) {
    DEBUG ((DEBUG_ERROR, "[ME] No ACK for sent message.\n"));
    return EFI_DEVICE_ERROR;
  }

  if (MsgHeciBusDisable.Status != 0) {
    Status = EFI_ABORTED;
  }

  return Status;
}

/**
  Send ME the BIOS end of Post message.

  @retval EFI_SUCCESS             EOP is sent successfully.
  @retval EFI_NO_RESPONSE         Failed to send EOP successfully with 3 trials.
  @retval EFI_DEVICE_ERROR        No HECI1 device
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this function
**/
EFI_STATUS
MeEndOfPostEvent (
  OUT UINT32                          *RequestedActions
  )
{
  EFI_STATUS                          Status;
  UINT8                               EopSendRetries;
  UINTN                               HeciMBAR;
  HECI_HOST_CONTROL_REGISTER          HeciRegHCsr;
  volatile HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;
  volatile HECI_ME_CONTROL_REGISTER   *HeciRegMeCsrHaPtr;

  DEBUG ((DEBUG_INFO, "[ME] MeEndOfPostEvent () - Start\n"));

  //
  // Initialize pointers to control registers
  //

  HeciMBAR = CheckAndFixHeciForAccess (HECI1_DEVICE);
  HeciRegHCsrPtr    = (VOID *) (UINTN) (HeciMBAR + H_CSR);
  HeciRegMeCsrHaPtr = (VOID *) (UINTN) (HeciMBAR + ME_CSR_HA);

  Status = EFI_SUCCESS;
  for (EopSendRetries = 0; EopSendRetries <= MAX_EOP_SEND_RETRIES; EopSendRetries++) {

    Status = HeciSendEndOfPostMessage (RequestedActions);
    if ((Status == EFI_SUCCESS) || (Status == EFI_DEVICE_ERROR) || (Status == EFI_UNSUPPORTED)) {
      break;
    }

    Status = EFI_NO_RESPONSE;

    //
    // Set H_RST and H_IG bits to reset HECI
    //
    HeciRegHCsr.ul      = HeciRegHCsrPtr->ul;
    HeciRegHCsr.r.H_RST = 1;
    HeciRegHCsr.r.H_IG  = 1;
    HeciRegHCsrPtr->ul  = HeciRegHCsr.ul;

    //
    // Wait for ME_RDY
    //
    if (WaitForMEReady (HECI1_DEVICE) != EFI_SUCCESS) {
      Status = EFI_TIMEOUT;
      break;
    }

    //
    // Clear H_RST, set H_RDY & H_IG bits
    //
    HeciRegHCsr.ul      = HeciRegHCsrPtr->ul;
    HeciRegHCsr.r.H_RST = 0;
    HeciRegHCsr.r.H_IG  = 1;
    HeciRegHCsr.r.H_RDY = 1;
    HeciRegHCsrPtr->ul  = HeciRegHCsr.ul;
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "[ME] ERROR: Can't send EOP message, all ME devices disabled, proceeding with boot!\n"
      ));
    HeciDisableHeciBusMsg ();

    //
    // Disable HECI function
    //
    DisableAllMeDevices ();
  }
  // Set EndOfPostDone unconditionally
  SetEndOfPostDone();

  return Status;
}

/**
  1. Cf9Gr Lock Config
      - PCH BIOS Spec Rev 0.9 Section 18.4  Additional Power Management Programming
        Step 2
        Set "Power Management Initialization Register (PMIR) Field 1", D31:F0:ACh[31] = 1b
        for production machine according to "RS - PCH Intel Management Engine
        (Intel(r) ME) BIOS Writer's Guide".
  2. Remove PSF access prior to boot

**/
VOID
LockConfig (
  VOID
  )
{
  UINT32                          MeMode;
  HECI_FWS_REGISTER               MeFirmwareStatus;
  UINT32                          Data;
  EFI_PEI_HOB_POINTERS            HobPtr;
  PCH_POLICY_HOB                  *PchPolicyHob;

  DEBUG ((DEBUG_INFO, "LockConfig () - Start\n"));
  HeciGetMeMode (&MeMode);

  MeFirmwareStatus.ul = MmioRead32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + R_FWSTATE);

  ///
  /// PCH BIOS Spec Rev 0.9 Section 18.4  Additional Power Management Programming
  /// Step 2
  ///   Set "Power Management Initialization Register (PMIR) Field 1", D31:F0:ACh[31] = 1b
  ///   for production machine according to "RS - PCH Intel Management Engine
  ///  (Intel(r) ME) BIOS Writer's Guide".
  ///
  /// PCH ME BWG section 4.5.1
  /// The IntelR FPT tool /GRST option uses CF9GR bit to trigger global reset.
  /// Based on above reason, the BIOS should not lock down CF9GR bit during Manufacturing and
  /// Re-manufacturing environment.
  ///
  Data = 0;
  if (((MeMode == ME_MODE_NORMAL) || (MeMode == ME_MODE_TEMP_DISABLED)) && !(MeFirmwareStatus.r.ManufacturingMode)) {
    ///
    /// PCH ME BWG section 4.4.1
    /// BIOS must also ensure that CF9GR is cleared and locked (via bit31 of the same register) before
    /// handing control to the OS in order to prevent the host from issuing global resets and reseting
    /// Intel Management Engine.
    ///
    Data |= B_PCH_PMC_ETR3_CF9LOCK;
  }

  MmioAndThenOr32 (
    MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_ETR3,
    (UINT32) (~(B_PCH_PMC_ETR3_CF9LOCK | B_PCH_PMC_ETR3_CF9GR)),
    (UINT32) Data
    );

  //
  // Get PCH Policy HOB.
  //
  HobPtr.Guid   = GetFirstGuidHob (&gPchPolicyHobGuid);
  ASSERT (HobPtr.Guid != NULL);
  if (HobPtr.Guid != NULL) {
    PchPolicyHob = GET_GUID_HOB_DATA (HobPtr.Guid);

    //
    // Remove PSF access prior to boot.
    //
    RemovePsfAccess (&PchPolicyHob->P2sbConfig);
  }
}

#endif // AMT_SUPPORT