/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2010 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  MeUma.c

@brief:

  Framework PEIM to MeUma

**/

///
/// Statements that include other files
///
#include "MeAccess.h"
#include "HeciRegs.h"
#include "IndustryStandard/Pci22.h"
#include "MeUma.h"
#include "Library/DebugLib.h"
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/TimerLib.h>
#include <PchAccess.h>
#include <Register/PchRegsPmc.h>
#include "Library/PerformanceLib.h"
#include <Ppi/MeSpsPolicyPei.h>
//
#include "Ppi/Wdt.h"
#include "Library/IoLib.h"
#include "Protocol/PchReset.h"
#include "Library/MeTypeLib.h"
#include <Library/PlatformHooksLib.h>
#include <Library/HeciMsgLib.h>
#include <Library/MeSpsPolicyAccessPeiLib.h>
#include "Ppi/HeciPpi.h"
#include <Library/PchPmcLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START : To resolve build error.
#include "Library/BaseLib.h"
// APTIOV_SERVER_OVERRIDE_RC_END : To resolve build error.


/*****************************************************************************
 * Local definitions
 */
#define DEBUG_ME_UMA "ME UMA: "

/*****************************************************************************
 * Local prototypes
 */
CONST EFI_PEI_PPI_DESCRIPTOR gMemoryInitDoneSentSignalPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gMemoryInitDoneSentGuid,
  NULL
};

UINT32 MeSendUmaSize ( IN EFI_FFS_FILE_HEADER  *FfsHeader );
EFI_STATUS CpuReplacementCheck ( IN EFI_FFS_FILE_HEADER *FfsHeader,
                                 UINT8                  *ForceFullTraining,
                                 UINT8                  *nextStep
                                );
EFI_STATUS MeConfigDidReg ( IN EFI_FFS_FILE_HEADER  *FfsHeader,
                            UINT8                   InitStat,
                            UINT8                   *nextStep
                           );
EFI_STATUS HandleMeBiosAction ( UINT8 BiosAction );
EFI_STATUS UmaPerformReset ( UINT8 ResetType );
EFI_STATUS ClearDisb ( VOID );
EFI_STATUS SetDISB ( VOID );
BOOLEAN EFIAPI isMeUmaEnabled ( VOID );
EFI_STATUS HostResetWarning( OUT UINT8 *pAction );

// Assume HECI is initialized
// If not, in driver entry variable is set to false.
BOOLEAN HeciInitialized = TRUE;

#if TESTMENU_FLAG || ME_TESTMENU_FLAG
EFI_STATUS EFIAPI TestUmaLocation (
          IN UINT32 MeNcMemLowBaseAddr,
          IN UINT32 MeNcMemHighBaseAddr,
          IN UINT32 MeNcMemLowLimit,
          IN UINT32 MeNcMemHighLimit
        );
#endif // TESTMENU_FLAG || ME_TESTMENU_FLAG

/*****************************************************************************
 * Global variables
 */
static ME_UMA_PPI         mMeUmaPpi = {
  MeSendUmaSize,
  CpuReplacementCheck,
  MeConfigDidReg,
  HandleMeBiosAction,
  isMeUmaEnabled
  ,HostResetWarning
#if TESTMENU_FLAG || ME_TESTMENU_FLAG
  ,TestUmaLocation
#endif // TESTMENU_FLAG || ME_TESTMENU_FLAG
};

static EFI_PEI_PPI_DESCRIPTOR mMeUmaPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gMeUmaPpiGuid,
    &mMeUmaPpi
  }
};

/**

  @brief
  This function reads configurations for UMA.

  @retval Return Status based on errors that occurred while waiting for time to expire.

**/
BOOLEAN
EFIAPI
isMeUmaEnabled (
  VOID
  )
{
  BOOLEAN                         UmaEnabled = TRUE;
#if ME_TESTMENU_FLAG
  ME_POLICY_PPI                  *MePolicyPpi;
#endif // ME_TESTMENU_FLAG

  if (PchIsDwrFlow() == TRUE) {
    DEBUG((DEBUG_ERROR, DEBUG_ME_UMA" DWR detected, UMA is disabled\n"));
    return FALSE;
  }

#if ME_TESTMENU_FLAG
  //
  // Configure UMA support
  // Disable UMA when END_OF_POST message is disabled
  //
  MePolicyPpi = GetMePolicyPpi();
  if (MePolicyPpi != NULL)
  {
    UmaEnabled = (BOOLEAN)MePolicyPpi->MeConfig.SendDidMsg;
  }
  else
  {
    DEBUG((DEBUG_ERROR, DEBUG_ME_UMA" ERROR: Cannot locate ME Policy PPI, useing default\n"));
  }
#endif // ME_TESTMENU_FLAG


  if (MeTypeIsDisabled())
  {
    UmaEnabled = FALSE;
  }

  DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" UMA Enabled flag = %d\n", UmaEnabled));

  return UmaEnabled;
}

/**

  @brief
  This procedure will read and return the amount of ME UMA requested
  by ME ROM from the HECI device.

  @param[in] FfsHeader            Pointer to the FFS file header

  @retval UINT32                  Return ME UMA Size
  @retval 0                       Do not check for ME UMA

**/
UINT32
MeSendUmaSize (
  IN EFI_FFS_FILE_HEADER  *FfsHeader
  )
{
  UINT32                  Timeout;
  HECI_MISC_SHDW_REGISTER MeMiscShdw;
  HECI_FWS_REGISTER       MeHfs;

  if (isMeUmaEnabled() == 0) {
    DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME UMA feature disabled (MeSendUmaSize()).\n"));
    return 0;
  }

  MeMiscShdw.ul = HeciPciRead32 (R_ME_MISC_SHDW);
  MeHfs.ul      = HeciPciRead32 (R_ME_HFS);
  Timeout       = 0x0;

  if (!MeTypeIsDfx()) {
    if (MeTypeIsDisabled()) {
      DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME debug mode, do not check for ME UMA. \n"));
      return 0;
    }

    if (MeHfs.r.ErrorCode != ME_ERROR_CODE_NO_ERROR) {
      DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME error, do not check for ME UMA. \n"));
      return 0;
    }
  }


  PERF_START (FfsHeader, "MUSZV", NULL, AsmReadTsc ());
  while ((MeMiscShdw.r.MUSZV == 0) && (Timeout < MUSZV_TIMEOUT_MULTIPLIER)) {
    MicroSecondDelay (STALL_1_MILLISECOND);
    MeMiscShdw.ul = HeciPciRead32 (R_ME_MISC_SHDW);
    Timeout++;
  }

  PERF_END (FfsHeader, "MUSZV", NULL, AsmReadTsc ());

  if (Timeout >= MUSZV_TIMEOUT_MULTIPLIER) {
    DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" Timeout occurred waiting for MUSZV. \n"));
    if (IsSimicsPlatform() == TRUE) {
      DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" For Simics set 32MB for ME UMA size. \n"));
      return 32;
    }
    else {
      return 0;
    }
  }

  ///
  /// Return MeUmaSize value
  ///
  DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME UMA Size Requested: %x\n", MeMiscShdw.r.MUSZ));

  return MeMiscShdw.r.MUSZ;
}

/**

  @brief
  This procedure will determine whether or not the CPU was replaced
  during system power loss or via dynamic fusing.
  Calling this procedure could result in a warm reset (if ME FW is requesting one).

  @param[in] FfsHeader            Not used.
  @param[in] ForceFullTraining    When set = 0x1, MRC will be forced to perform
                                  a full memory training cycle.
  @param[in] nextStep             returns information about required next action

  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
CpuReplacementCheck (
  IN EFI_FFS_FILE_HEADER *FfsHeader,
  UINT8                  *ForceFullTraining,
  UINT8                  *nextStep
  )
{
  return EFI_UNSUPPORTED;
}


/**

  @brief
  This procedure will configure the ME Host General Status register,
  indicating that DRAM Initialization is complete and ME FW may
  begin using the allocated ME UMA space.

  @param[in] FfsHeader            Pointer to the FFS file header
  @param[in] InitStat             H_GS[27:24] Status
  @param[in] nextStep             returns information about required next action

  @retval EFI_SUCCESS

**/
EFI_STATUS
MeConfigDidReg (
  IN EFI_FFS_FILE_HEADER  *FfsHeader,
  UINT8                   InitStat,
  UINT8                   *nextStep
  )
{
  UINT32                      MeUmaBase;
  UINT32                      MeUmaBaseExt;
  UINT32                      MeHgs;
  UINT32                      Timeout;
  HECI_FWS_REGISTER           MeHfs;
  EFI_STATUS                  Status;
  EFI_STATUS                  Status2;
#if ME_TESTMENU_FLAG
  ME_POLICY_PPI               *MePolicyPpi = GetMePolicyPpi();
#endif

  *nextStep = CBM_DIR_DO_NOTHING;

  if (isMeUmaEnabled() == 0) {
    DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME UMA feature disabled (MeConfigDidReg()).\n"));
    Status = EFI_SUCCESS;
    goto MemoryInitDoneSentNotify;
  }

  if (!HeciInitialized) {
    Status = PeiHeciInitialize();
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, DEBUG_ME_UMA" ERROR: MeConfigDidReg(): Cannot initialize HECI\n"));
      ASSERT_EFI_ERROR (Status);
      goto MemoryInitDoneSentNotify;
    }
    HeciInitialized = TRUE;
  }

  MeHgs     = 0x0;
  Timeout   = 0x0;
  MeHfs.ul  = HeciPciRead32 (R_ME_HFS);

  if (!MeTypeIsDfx()) {
    if (MeTypeIsDisabled()) {
      DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME debug mode, do not check for ME UMA. \n"));
      Status = EFI_SUCCESS;
      goto MemoryInitDoneSentNotify;
    }

    if (MeHfs.r.ErrorCode != ME_ERROR_CODE_NO_ERROR) {
      DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME error, do not check for ME UMA. \n"));
      Status = EFI_SUCCESS;
      goto MemoryInitDoneSentNotify;
    }
  }

  DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" Entered ME DRAM Init Done procedure.\n"));

  ///
  /// Read MESEGBASE value
  ///
  MeUmaBase     = MmioRead32 (MmPciBase(0, 5, 0) + R_MESEG_BASE);
  MeUmaBaseExt  = MmioRead32 (MmPciBase(0, 5, 0) + R_MESEG_BASE + 0x04);
  DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" MeUmaBase read: %x\n", MeUmaBase));

  ///
  /// Write DRAM Init Done (DID) data to the ME H_GS[23:0].
  /// H_GS[23:16] = extended UMA base address (reserved)
  /// H_GS[15:0] = 1M aligned UMA base address
  /// ME FW will 0 extend these values to determine MeUmaBase
  ///
  MeUmaBase = ((MeUmaBaseExt << 28) + (MeUmaBase >> 4)) >> 16;
  MeHgs     = MeUmaBase;

  ///
  /// Set H_GS[31:28] = 0x1 indicating DRAM Init Done
  ///
  MeHgs |= B_ME_DID_TYPE_MASK;

  ///
  /// RapidStart
  ///
  if (InitStat & 0x80) {
    MeHgs |= B_ME_DID_RAPID_START_BIT;
  }

  InitStat &= 0x7F;

  ///
  /// Set H_GS[27:24] = Status
  ///   0x0 = Success
  ///   0x1 = No Memory in channels
  ///   0x2 = Memory Init Error
  ///   0x3 = Memory not preserved across reset
  ///   0x4-0xF = Reserved
  ///
  ///
  /// If TestDidInitStat is not set to "Auto" (0), provide the TestDidInitStat
  /// value specified in the setup option to ME FW.
  ///
#if ME_TESTMENU_FLAG
  if ((MePolicyPpi != NULL) && (MePolicyPpi->MeConfig.DidInitStat > 0)) {
    InitStat = (UINT8) MePolicyPpi->MeConfig.DidInitStat - 1;
  }
#endif // TESTMENU_FLAG || ME_TESTMENU_FLAG
  DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" InitStat: %x\n", InitStat));
  MeHgs |= (InitStat << 24);

  PERF_START (FfsHeader, "DID", NULL, AsmReadTsc ());
  //
  // Send DID using HECI message
  //
  DEBUG ((DEBUG_INFO, " Sending DID as MKHI message\n"));
  Status = PeiHeciSendDid(MeHgs, nextStep);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Sending DID failed! Error: %r\n", Status));
  }

  PERF_END (FfsHeader, "DID", NULL, AsmReadTsc ());

  DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" BiosAction = %x\n", *nextStep));

MemoryInitDoneSentNotify:
  //
  // MemoryInitDone signal
  //
  DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" Sending MemoryInitDoneSent Notification ...\n"));
  Status2 = PeiServicesInstallPpi (&gMemoryInitDoneSentSignalPpi);
  DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" Sent MemoryInitDoneSent Notification (%r)\n", Status2));
  ASSERT_EFI_ERROR (Status2);

  return Status;
}


 /**

  @brief
  This procedure will Host Reset Warning message over HECI to ME.
  Host Reset Warning message is used in server BIOS to warn ME about
  a reset of host partition done on Sx resume path that will not break
  the Sx resume. I.e. after reset host continues the same Sx flow,
  so ME should ignore this reset and sustaing previews flow.

  @param[out] pAction   optional pointer to buffer for action requested by ME

  @retval EFI_SUCCESS

 **/
EFI_STATUS
HostResetWarning(
    OUT UINT8                  *pAction
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINT32                        RspLen;
  HECI_FWS_REGISTER             Mefs1;
  HECI_PPI                     *pHeciPpi;
#if ME_TESTMENU_FLAG
  ME_POLICY_PPI                *pMePolicy = GetMePolicyPpi();
#endif
  union
  {
    HOST_RESET_WARNING_CMD_REQ  Req;
    HOST_RESET_WARNING_CMD_RESP Rsp;
  } HeciMsg;

  if (MeTypeIsDisabled()) {
      DEBUG((DEBUG_ERROR, DEBUG_ME_UMA" HostResetWarning(): Me disabled - no reset warning\n"));
      goto GetOut;
  }

  if (!HeciInitialized) {
    Status = PeiHeciInitialize();
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, DEBUG_ME_UMA" ERROR: HostResetWarning(): Cannot initialize HECI\n"));
      ASSERT_EFI_ERROR (Status);
      goto GetOut;
    }
    HeciInitialized = TRUE;
  }

#if ME_TESTMENU_FLAG
  if (pMePolicy != NULL && !pMePolicy->MeConfig.HostResetNotification)
  {
    DEBUG((DEBUG_INFO, DEBUG_ME_UMA" HostResetWarning notification disabled\n"));
    goto GetOut;
  }
#endif
  
  Mefs1.ul = HeciPciRead32(R_ME_HFS);
  if (Mefs1.r.ErrorCode != 0 ||
      Mefs1.r.CurrentState == ME_STATE_RECOVERY ||
      Mefs1.r.CurrentState > ME_STATE_TRANSITION)
  {
    DEBUG((DEBUG_WARN, DEBUG_ME_UMA" WARNING: HostResetWarning not supported in "
           "current ME state (MEFS1: %08X)\n", Mefs1.ul));
    Status = EFI_TIMEOUT;
    goto GetOut;
  }
  Status = PeiServicesLocatePpi(&gHeciPpiGuid, 0, NULL, (VOID**)&pHeciPpi);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, DEBUG_ME_UMA" ERROR: Cannot locate HECI PPI (%r)\n", Status));
    ASSERT_EFI_ERROR (Status);
    goto GetOut;
  }
  
  HeciMsg.Req.MkhiHeader.Data               = 0;
  HeciMsg.Req.MkhiHeader.Fields.Command     = HOST_RESET_WARNING_CMD;
  HeciMsg.Req.MkhiHeader.Fields.IsResponse  = 0;
  HeciMsg.Req.MkhiHeader.Fields.GroupId     = BUP_PM_GROUP_ID;
  HeciMsg.Req.MkhiHeader.Fields.Reserved    = 0;
  HeciMsg.Req.MkhiHeader.Fields.Result      = 0;
  
  RspLen = sizeof(HeciMsg);
  Status = pHeciPpi->SendwAck(HECI1_DEVICE, (UINT32*)&HeciMsg.Req,
                              sizeof(HeciMsg.Req), &RspLen,
                              BIOS_FIXED_HOST_ADDR, HECI_CORE_MESSAGE_ADDR);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, DEBUG_ME_UMA" ERROR: Cannot send HostResetWarning message (%r)\n",
         Status));
  }
  else if (HeciMsg.Rsp.MkhiHeader.Fields.GroupId != BUP_PM_GROUP_ID ||
           HeciMsg.Rsp.MkhiHeader.Fields.Command != HOST_RESET_WARNING_CMD ||
           HeciMsg.Rsp.MkhiHeader.Fields.IsResponse != 1 ||
           HeciMsg.Rsp.MkhiHeader.Fields.Result != 0)
  {
    DEBUG((DEBUG_ERROR, DEBUG_ME_UMA" ERROR: Invalid MKHI header in HostResetWarning response "
         "(%08X)\n", HeciMsg.Rsp.MkhiHeader.Data));
  }
  else if (HeciMsg.Rsp.BiosAction > HOST_RESET_WARNING_ACTION_LAST)
  {
    DEBUG((DEBUG_ERROR, DEBUG_ME_UMA" ERROR: Unknown action in HostResetWarning response "
         "(%d)\n", HeciMsg.Rsp.BiosAction));
  }
  else
  {
    DEBUG((DEBUG_INFO, DEBUG_ME_UMA" HostResetWarning response action is %d\n",
           HeciMsg.Rsp.BiosAction));
    if (pAction != NULL)
    {
      *pAction = HeciMsg.Rsp.BiosAction;
    }
  }
 GetOut:
  return Status;
} // HostResetWarning()

/**

  @brief
  This procedure will enforce the BIOS Action that was requested by ME FW
  as part of the DRAM Init Done message.

  @param[in] BiosAction           Me requests BIOS to act

  @retval EFI_SUCCESS             Always return EFI_SUCCESS

**/
EFI_STATUS
HandleMeBiosAction (
  UINT8               BiosAction
  )
{
  EFI_STATUS            Status;
  HECI_GS_SHDW_REGISTER MeFwsts2;

  if (isMeUmaEnabled() == 0) {
    DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME UMA feature disabled (HandleMeBiosAction()).\n"));
    return EFI_SUCCESS;
  }

  if (!MeTypeIsDfx()) {
    if (MeTypeIsDisabled()) {
      DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME debug mode, do not HandleMeBiosAction. \n"));
      return EFI_SUCCESS;
    }
  }

  ///
  /// Read ME FWS2
  ///
  MeFwsts2.ul = HeciPciRead32 (R_ME_GS_SHDW);
  DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" MeFwsts2 = %x.\n", MeFwsts2.r));

  switch (BiosAction) {
  case CBM_DIR_DO_NOTHING:
    ///
    /// DID ACK was not received
    ///
    DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" DID Ack was not received, no BIOS Action to process.\n"));
    break;
  case CBM_DIR_NON_PCR_CLEARDISB:
    DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" ME FW DID ACK has requested a Clear DISB.\n"));
    ///
    /// Clear DISB and Issue a Non-Power Cycle Reset
    ///
    Status  = ClearDisb ();
    ASSERT_EFI_ERROR (Status);
    ///
    /// This type of reset is handled directly by MRC
    ///
    return Status;
  case CBM_DIR_NON_PCR:
    ///
    /// Perform Non-Power Cycle Reset
    ///
    DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" ME FW has requested a Non-PCR.\n"));
    Status = UmaPerformReset (CBM_DIR_NON_PCR);
    break;

  case CBM_DIR_PCR:
    ///
    /// Perform Power Cycle Reset
    ///
    DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" ME FW has requested a PCR.\n"));
    Status = UmaPerformReset (CBM_DIR_PCR);
    break;

  case 3:
    ///
    /// Go To S3
    ///
    DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME FW DID ACK has requested entry to S3.  Not defined, continuing to POST.\n"));
    break;

  case 4:
    ///
    /// Go To S4
    ///
    DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME FW DID ACK has requested entry to S4.  Not defined, continuing to POST.\n"));
    break;

  case 5:
    ///
    /// Go To S5
    ///
    DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME FW DID ACK has requested entry to S5.  Not defined, continuing to POST.\n"));
    break;

  case CBM_DIR_GLOBAL_RESET:
    ///
    /// Perform Global Reset
    ///
    DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" ME FW has requested a Global Reset.\n"));
    Status = UmaPerformReset (CBM_DIR_GLOBAL_RESET);
    break;

  case CBM_DIR_CONTINUE_POST:
    ///
    /// Continue to POST
    ///
    DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME FW DID Ack requested to continue to POST.\n"));
    break;
  }

  return EFI_SUCCESS;
}

/**

@brief
  This procedure will issue a Non-Power Cycle, Power Cycle, or Global Rest.

  @param[in] ResetType            Type of reset to be issued.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval All other error conditions encountered result in an ASSERT.

**/
EFI_STATUS
UmaPerformReset (
  UINT8               ResetType
  )
{
  PCH_RESET_PROTOCOL  *mPchReset;
  PCH_RESET_TYPE      PchResetType;
  EFI_STATUS          Status;

  if (MeTypeIsDisabled()) {
      DEBUG((DEBUG_ERROR, DEBUG_ME_UMA" UmaPerformReset(): Me disabled - no reset\n"));
      return EFI_SUCCESS;
  }

  Status = PeiServicesLocatePpi(&gPchResetPpiGuid, 0, NULL, &mPchReset);
  ASSERT_EFI_ERROR (Status);
  if (Status != EFI_SUCCESS)
    return Status;

  switch (ResetType) {
  case CBM_DIR_NON_PCR_CLEARDISB:
  case CBM_DIR_NON_PCR:
    DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" ME FW DID ACK has requested a Non Power Cycle Reset.\n"));
    PchResetType = ColdReset;
    break;

  case CBM_DIR_PCR:
    ///
    /// Power Cycle Reset requested
    ///
    DEBUG ((DEBUG_INFO, DEBUG_ME_UMA" ME FW DID ACK has requested a Power Cycle Reset.\n"));
    PchResetType = PowerCycleReset;
    break;

  case CBM_DIR_GLOBAL_RESET:
    ///
    /// Global Reset
    ///
    DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" ME FW DID Ack requested a global reset.\n"));
    PchResetType = GlobalReset;
    break;
  default:
    DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" ME FW No reset.\n"));

    return EFI_SUCCESS;
  }

  mPchReset->Reset (mPchReset, PchResetType, 0, NULL);

  //
  // We should never get here
  //
  ASSERT (FALSE);

  return EFI_SUCCESS;
}

/**

  @brief
  This procedure will clear the DISB.

  @param[in] None

  @retval EFI_SUCCESS             Always return EFI_SUCCESS

**/
EFI_STATUS
ClearDisb (
  VOID
  )
{
  UINT16  Data16;

  Data16  = MmioRead16 (MmPciBase(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_A);
  Data16  = Data16 &~B_PCH_LPC_GEN_PMCON_DRAM_INIT;
  MmioWrite16 (MmPciBase(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_A, Data16);

  return EFI_SUCCESS;
}

/**

  @brief
  This procedure will clear the DISB.

  @param[in] None

  @retval EFI_SUCCESS             Always return EFI_SUCCESS

**/
EFI_STATUS
SetDISB (
  VOID
  )
{
  UINT16  Data16;

  Data16  = MmioRead16 (MmPciBase(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_A);
  Data16  = Data16 & B_PCH_LPC_GEN_PMCON_DRAM_INIT;
  MmioWrite16 (MmPciBase(0, PCI_DEVICE_NUMBER_PCH_LPC, 0) + R_PCH_PMC_GEN_PMCON_A, Data16);

  return EFI_SUCCESS;
}

/**

  @brief
  This function is the entry point for this PEI.

  @param[in] FfsHeader            Pointer to the FFS file header
  @param[in] PeiServices          Pointer to the PEI services table

  @retval Return Status based on errors that occurred while waiting for time to expire.

**/
EFI_STATUS
EFIAPI
MeUmaEntry (
  IN EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES   **PeiServices
  )
{
  EFI_STATUS   Status = EFI_SUCCESS;

  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" ME UMA PPI Driver EntryPoint\n"));

  Status = PeiHeciInitialize();
  if (EFI_ERROR(Status)) {
    // Defer Heci initialization
    HeciInitialized = FALSE;
    // and allow for driver loading
    // Heci initialization will be retried on UMA PPI interface usage.
    // When UMA PPI is about to send message.
    Status = EFI_SUCCESS;
  }
  if (!EFI_ERROR(Status)) {
    Status  = PeiServicesInstallPpi (mMeUmaPpiList);
    DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" ME UMA PPI Installation status %r\n", Status));
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}

#if TESTMENU_FLAG || ME_TESTMENU_FLAG
/**

  @brief
  This function tests UMA location stored in MRC against read from register.

  @param[in] MeNcMemLowBaseAddr           Low part of UMA base address
  @param[in] MeNcMemHighBaseAddr          High part of UMA base address
  @param[in] MeNcMemLowLimit              Low part of UMA base address limit
  @param[in] MeNcMemHighLimit             High part of UMA base address limit

  @retval Return Status based comparation result.

**/
EFI_STATUS
EFIAPI
TestUmaLocation (
  IN UINT32 MeNcMemLowBaseAddr,
  IN UINT32 MeNcMemHighBaseAddr,
  IN UINT32 MeNcMemLowLimit,
  IN UINT32 MeNcMemHighLimit
  )
{
  EFI_STATUS   Status = EFI_SUCCESS;
  UINT32       MeUmaBase;
  UINT32       MeUmaBaseExt;
  UINT32       MeUmaSize;
  UINT32       MeUmaSizeM;
  UINT64       MeNcMemLowLimit2 = (((MeNcMemLowLimit >> 19)+1) << 19);

  if (isMeUmaEnabled() == 0) {
    return Status;
  }

  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" ====================================================\n"));
  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" Stored UMA Location:\n"));
  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" MeNcMemLowBaseAddr  = 0x%x\n", MeNcMemLowBaseAddr));
  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" MeNcMemHighBaseAddr = 0x%x\n", MeNcMemHighBaseAddr));
  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" MeNcMemLowLimit     = 0x%x (->0x%x)\n", MeNcMemLowLimit, MeNcMemLowLimit2));
  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" MeNcMemHighLimit    = 0x%x\n", MeNcMemHighLimit));
  // APTIOV_SERVER_OVERRIDE_RC_START : To resolve build error.
  MeUmaSize = (UINT32) ((LShiftU64((UINT64)MeNcMemHighLimit, 32) + (UINT64)MeNcMemLowLimit2) -
                       (LShiftU64((UINT64)MeNcMemHighBaseAddr, 32) + (UINT64)MeNcMemLowBaseAddr));
  // APTIOV_SERVER_OVERRIDE_RC_END : To resolve build error.
  MeUmaSizeM =MeUmaSize/1024/1024;
  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" MeUmaSize           = 0x%x (%dM)\n", MeUmaSize, MeUmaSizeM));

  ///
  /// Read MESEGBASE value
  ///
  MeUmaBase     = MmioRead32 (MmPciBase(0, 5, 0) + R_MESEG_BASE);
  MeUmaBaseExt  = MmioRead32 (MmPciBase(0, 5, 0) + R_MESEG_BASE + 0x04);
  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" UMA parameters read:\n"));
  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" MeUmaBase           = 0x%x\n", MeUmaBase));
  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" MeUmaBaseExt        = 0x%x\n", MeUmaBaseExt));

  if ((MeNcMemLowBaseAddr == 0) || (MeNcMemLowLimit == 0)) {
    if (MeUmaSize > 0) {
      DEBUG ((EFI_D_WARN, DEBUG_ME_UMA" WARNING: Lower part of UMA addresses is 0.\n"));
    }
  } else {
    if ((MeNcMemLowBaseAddr != MeUmaBase) || (MeNcMemHighBaseAddr != MeUmaBaseExt)) {
      DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" ERROR: UMA addresses error.\n"));
      Status = EFI_DEVICE_ERROR;
    }
    if ((MeUmaSizeM > ME_UMA_SIZE_UPPER_LIMIT) && (MeUmaSizeM > 0)) {
      DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" ERROR: UMA size error #01.\n"));
      Status = EFI_DEVICE_ERROR;
    } else if (((MeUmaSize/2)*2) != MeUmaSize) {
      // UMA size has to be n*2
      DEBUG ((EFI_D_ERROR, DEBUG_ME_UMA" ERROR: UMA size error #02.\n"));
      Status = EFI_DEVICE_ERROR;
    }
  }
  DEBUG ((EFI_D_INFO, DEBUG_ME_UMA" ====================================================\n"));

  return Status;
}
#endif // TESTMENU_FLAG || ME_TESTMENU_FLAG
