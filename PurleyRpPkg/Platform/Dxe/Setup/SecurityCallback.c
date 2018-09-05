/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2005 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SecurityCallback.c

  Updates the IFR with runtime information.

Revision History:

**/

#include "SetupPlatform.h"
#include <Library/SetupLib.h>

//#include "EfiTpm.h"

STATIC EFI_PASSWORD_DATA *SecurityData = NULL;
STATIC UINT8             State;

EFI_PASSWORD_DATA mAdminPassword = {
  {
    0
  },
  {
    0
  },
  &mSystemConfiguration.AdminPassword,
  EfiAdminPassword
};

EFI_PASSWORD_DATA mUserPassword = {
  {
    0
  },
  {
    0
  },
  &mSystemConfiguration.UserPassword,
  EfiUserPassword
};

STATIC
EFI_STATUS
EnableTpmDevice (
  IN     BOOLEAN                   TpmEnable
  );

STATIC
EFI_STATUS
ActivateTpmDevice (
  IN     BOOLEAN                   TpmActive
  );

STATIC
TPM_RESULT
TpmCommandNoReturnData (
  IN     EFI_TCG_PROTOCOL          *TcgProtocol,
  IN     TPM_COMMAND_CODE          Ordinal,
  IN     UINTN                     AdditionalParameterSize,
  IN     VOID                      *AdditionalParameters
  );

#define TPM_PP_USER_ABORT           ((TPM_RESULT)(-0x10))
#define TPM_PP_BIOS_FAILURE         ((TPM_RESULT)(-0x0f))

#define H2NS(x) ((((x) << 8) | ((x) >> 8)) & 0xffff)
#define H2NL(x) (H2NS ((x) >> 16) | (H2NS ((x) & 0xffff) << 16))

/**

    GC_TODO: add routine description

    @param Ordinal                 - GC_TODO: add arg description
    @param AdditionalParameterSize - GC_TODO: add arg description
    @param AdditionalParameters    - GC_TODO: add arg description

    @retval Status                - GC_TODO: add retval description
    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description

**/
STATIC
EFI_STATUS
SimpleTpmCommand (
  IN     TPM_COMMAND_CODE          Ordinal,
  IN     UINTN                     AdditionalParameterSize,
  IN     VOID                      *AdditionalParameters
  )
{
  EFI_STATUS                       Status;
  TPM_RQU_COMMAND_HDR              *TpmRqu;
  TPM_RSP_COMMAND_HDR              TpmRsp;
  UINT32                           Size;
  EFI_TCG_PROTOCOL                 *TcgProtocol;

  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, &TcgProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TpmRqu = (TPM_RQU_COMMAND_HDR*)AllocateZeroPool (
                                   sizeof (*TpmRqu) + AdditionalParameterSize
                                   );
  ASSERT (TpmRqu != NULL);

  if (TpmRqu == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  TpmRqu->tag       = H2NS (TPM_TAG_RQU_COMMAND);
  Size              = (UINT32)(sizeof (*TpmRqu) + AdditionalParameterSize);
  TpmRqu->paramSize = H2NL (Size);
  TpmRqu->ordinal   = H2NL (Ordinal);
  gBS->CopyMem (TpmRqu + 1, AdditionalParameters, AdditionalParameterSize);

  Status = TcgProtocol->PassThroughToTpm (
                          TcgProtocol,
                          Size,
                          (UINT8*)TpmRqu,
                          (UINT32)sizeof (TpmRsp),
                          (UINT8*)&TpmRsp
                          );
  gBS->FreePool (TpmRqu);
  ASSERT_EFI_ERROR (Status);
  ASSERT (TpmRsp.tag == H2NS (TPM_TAG_RSP_COMMAND));

  if (H2NL (TpmRsp.returnCode) != 0) {
    return EFI_INVALID_PARAMETER;
  }
  return Status;
}


EFI_STATUS
GetTpmState (
     OUT BOOLEAN                   *TpmEnable, OPTIONAL
     OUT BOOLEAN                   *TpmActivated, OPTIONAL
     OUT BOOLEAN                   *PhysicalPresenceLock, OPTIONAL
     OUT BOOLEAN                   *LifetimeLock, OPTIONAL
     OUT BOOLEAN                   *CmdEnable OPTIONAL
  )
/**

    GC_TODO: add routine description

    @param OPTIONAL - GC_TODO: add arg description

    @retval Status           - GC_TODO: add retval description
    @retval EFI_DEVICE_ERROR - GC_TODO: add retval description
    @retval EFI_SUCCESS      - GC_TODO: add retval description

**/
{
  EFI_STATUS                       Status;
  TPM_RSP_COMMAND_HDR              *TpmRsp;
  UINT32                           TpmSendSize;
  TPM_PERMANENT_FLAGS              *TpmPermanentFlags;
  TPM_STCLEAR_FLAGS                *VFlags;
  UINT8                            CmdBuf[64];
  EFI_TCG_PROTOCOL                 *TcgProtocol;

  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, &TcgProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get TPM Permanent flags (TpmEnable, TpmActivated, LifetimeLock, CmdEnable)
  //
  if ((TpmEnable != NULL) || (TpmActivated != NULL) || (LifetimeLock != NULL) || (CmdEnable != NULL)) {
    TpmSendSize           = sizeof (TPM_RQU_COMMAND_HDR) + sizeof (UINT32) * 3;
    *(UINT16*)&CmdBuf[0]  = H2NS (TPM_TAG_RQU_COMMAND);
    *(UINT32*)&CmdBuf[2]  = H2NL (TpmSendSize);
    *(UINT32*)&CmdBuf[6]  = H2NL (TPM_ORD_GetCapability);

    *(UINT32*)&CmdBuf[10] = H2NL (TPM_CAP_FLAG);
    *(UINT32*)&CmdBuf[14] = H2NL (sizeof (TPM_CAP_FLAG_PERMANENT));
    *(UINT32*)&CmdBuf[18] = H2NL (TPM_CAP_FLAG_PERMANENT);

    Status = TcgProtocol->PassThroughToTpm (
                            TcgProtocol,
                            TpmSendSize,
                            CmdBuf,
                            sizeof (CmdBuf),
                            CmdBuf
                            );
    TpmRsp = (TPM_RSP_COMMAND_HDR*)&CmdBuf[0];
    if (EFI_ERROR (Status) || (TpmRsp->tag != H2NS (TPM_TAG_RSP_COMMAND)) || (TpmRsp->returnCode != 0)) {
      return EFI_DEVICE_ERROR;
    }

    TpmPermanentFlags = (TPM_PERMANENT_FLAGS *)&CmdBuf[sizeof (TPM_RSP_COMMAND_HDR) + sizeof (UINT32)];

    if (TpmEnable != NULL) {
      *TpmEnable = !TpmPermanentFlags->disable;
    }

    if (TpmActivated != NULL) {
      *TpmActivated = !TpmPermanentFlags->deactivated;
    }
    if (LifetimeLock != NULL) {
      *LifetimeLock = TpmPermanentFlags->physicalPresenceLifetimeLock;
    }

    if (CmdEnable != NULL) {
      *CmdEnable = TpmPermanentFlags->physicalPresenceCMDEnable;
    }
  }

  //
  // Get TPM Volatile flags (PhysicalPresenceLock)
  //
  if (PhysicalPresenceLock != NULL) {
    TpmSendSize           = sizeof (TPM_RQU_COMMAND_HDR) + sizeof (UINT32) * 3;
    *(UINT16*)&CmdBuf[0]  = H2NS (TPM_TAG_RQU_COMMAND);
    *(UINT32*)&CmdBuf[2]  = H2NL (TpmSendSize);
    *(UINT32*)&CmdBuf[6]  = H2NL (TPM_ORD_GetCapability);

    *(UINT32*)&CmdBuf[10] = H2NL (TPM_CAP_FLAG);
    *(UINT32*)&CmdBuf[14] = H2NL (sizeof (TPM_CAP_FLAG_VOLATILE));
    *(UINT32*)&CmdBuf[18] = H2NL (TPM_CAP_FLAG_VOLATILE);

    Status = TcgProtocol->PassThroughToTpm (
                            TcgProtocol,
                            TpmSendSize,
                            CmdBuf,
                            sizeof (CmdBuf),
                            CmdBuf
                            );
    TpmRsp = (TPM_RSP_COMMAND_HDR*)&CmdBuf[0];
    if (EFI_ERROR (Status) || (TpmRsp->tag != H2NS (TPM_TAG_RSP_COMMAND)) || (TpmRsp->returnCode != 0)) {
      return EFI_DEVICE_ERROR;
    }

    VFlags = (TPM_STCLEAR_FLAGS *)&CmdBuf[sizeof (TPM_RSP_COMMAND_HDR) + sizeof (UINT32)];

    if (PhysicalPresenceLock != NULL) {
      *PhysicalPresenceLock = VFlags->physicalPresenceLock;
    }
  }

  return EFI_SUCCESS;
}


/**

    GC_TODO: add routine description

    @param PhysicalPresence - GC_TODO: add arg description

    @retval Status                 - GC_TODO: add retval description
    @retval EFI_SECURITY_VIOLATION - GC_TODO: add retval description

**/
STATIC
EFI_STATUS
TpmPhysicalPresence (
  IN      TPM_PHYSICAL_PRESENCE     PhysicalPresence
  )
{
  EFI_STATUS                        Status;
  EFI_TCG_PROTOCOL                  *TcgProtocol;
  TPM_RQU_COMMAND_HDR               *TpmRqu;
  TPM_PHYSICAL_PRESENCE             *TpmPp;
  TPM_RSP_COMMAND_HDR               TpmRsp;
  UINT8                             Buffer[sizeof (*TpmRqu) + sizeof (*TpmPp)];

  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, &TcgProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TpmRqu = (TPM_RQU_COMMAND_HDR*)Buffer;
  TpmPp = (TPM_PHYSICAL_PRESENCE*)(TpmRqu + 1);

  TpmRqu->tag = H2NS (TPM_TAG_RQU_COMMAND);
  TpmRqu->paramSize = H2NL (sizeof (Buffer));
  TpmRqu->ordinal = H2NL (TSC_ORD_PhysicalPresence);
  *TpmPp = H2NS (PhysicalPresence);

  Status = TcgProtocol->PassThroughToTpm (
                          TcgProtocol,
                          sizeof (Buffer),
                          (UINT8*)TpmRqu,
                          sizeof (TpmRsp),
                          (UINT8*)&TpmRsp
                          );
  ASSERT_EFI_ERROR (Status);
  ASSERT (TpmRsp.tag == H2NS (TPM_TAG_RSP_COMMAND));
  if (TpmRsp.returnCode != 0) {
    //
    // If it fails, some requirements may be needed for this command.
    //
    return EFI_SECURITY_VIOLATION;
  }
  return Status;
}


/**

    GC_TODO: add routine description

    @param None

    @retval Status      - GC_TODO: add retval description
    @retval EFI_ABORTED - GC_TODO: add retval description

**/
STATIC
EFI_STATUS
SetOppPresent (
  VOID
  )
{
  EFI_STATUS                        Status;
  BOOLEAN                           LifetimeLock;
  BOOLEAN                           CmdEnable;

  //
  // Make sure TPM_PHYSICAL_PRESENCE_CMD_ENABLE is TRUE.
  //
  Status = GetTpmState (NULL, NULL, NULL, &LifetimeLock, &CmdEnable);
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "[TPM] Get GetTpmState error Status = %r\n", Status));
    return Status;
  }

  if (!CmdEnable) {
    if (LifetimeLock) {
      //
      // physicalPresenceCMDEnable is locked, can't change.
      //
      return EFI_ABORTED;
    }

    Status = TpmPhysicalPresence (TPM_PHYSICAL_PRESENCE_CMD_ENABLE);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Set operator physical presence flags
  //
  Status = TpmPhysicalPresence (TPM_PHYSICAL_PRESENCE_PRESENT);
  return Status;
}



/**

    GC_TODO: add routine description

    @param None

    @retval Status      - GC_TODO: add retval description
    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
STATIC
EFI_STATUS
TpmForceClear (
  VOID
  )
{
  EFI_STATUS                       Status;
  UINT8                            TpmStateOption;
  UINT8                            TpmEnableOption;

  DEBUG ((EFI_D_ERROR, "[TPM] Into ForceClear!\n"));
  Status = SetOppPresent();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // TPM Force Clear
  //
  DEBUG ((EFI_D_ERROR, "[TPM] SimpleTpmCommand TPM_ORD_ForceClear!\n"));
  Status = SimpleTpmCommand (TPM_ORD_ForceClear, 0, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[TPM] Fail to force clear TPM device!\n"));
    return Status;
  }

  //
  // Get current TPM configuration
  //
  if (mSystemConfiguration.TpmEnable || mSystemConfiguration.TpmState) {
    //
    // TPM variable need update
    //
    TpmEnableOption = FALSE;
    TpmStateOption  = FALSE;
    Status = SetOptionData (&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, TpmEnable), &TpmEnableOption, sizeof(TpmEnableOption));
    Status |= SetOptionData (&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, TpmState), &TpmStateOption, sizeof(TpmStateOption));
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR, "[TPM] Fail to update TPM NV Variable!(0x%x)\n", Status));
      return Status;
    }

    //
    // Pass changed uncommitted data back to Form Browser
    //
    //SetBrowserData (NULL, NULL, sizeof (mSystemConfiguration), (UINT8 *) &mSystemConfiguration, NULL);
  }

  return EFI_SUCCESS;
}


/**

    GC_TODO: add routine description

    @param MorActive - GC_TODO: add arg description

    @retval Status      - GC_TODO: add retval description
    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
STATIC
EFI_STATUS
EnableMor (
  IN     BOOLEAN                   MorActive
  )
{
  EFI_STATUS                       Status;

  Status = SetOptionData (&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, MorState), &MorActive, sizeof(MorActive));
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "[Mor] Fail to update MOR NV Variable!(0x%x)\n", Status));
    return Status;
  }
  return EFI_SUCCESS;
}

/**

  Set password state.

  @param SecurityData  -  Proposed password state to set to.

  @retval EFI_INVALID_PARAMETER  -  Invalid parameter passed in.
  @retval EFI_SUCCESS            -  State has been set successfully.
  @retval Others                 -  Error occurred during state setting.

**/
EFI_STATUS
SetPasswordState (
  IN  EFI_PASSWORD_DATA               *SecurityData
  )
{

  return EFI_INVALID_PARAMETER;
}

#define TPM_PP_USER_ABORT           ((TPM_RESULT)(-0x10))
#define TPM_PP_BIOS_FAILURE         ((TPM_RESULT)(-0x0f))

#define H2NS(x) ((((x) << 8) | ((x) >> 8)) & 0xffff)
#define H2NL(x) (H2NS ((x) >> 16) | (H2NS ((x) & 0xffff) << 16))

/**

    GC_TODO: add routine description

    @param TcgProtocol             - GC_TODO: add arg description
    @param Ordinal                 - GC_TODO: add arg description
    @param AdditionalParameterSize - GC_TODO: add arg description
    @param AdditionalParameters    - GC_TODO: add arg description

    @retval TPM_PP_BIOS_FAILURE - GC_TODO: add retval description
    @retval H2NL (TpmRsp.Code)  - GC_TODO: add retval description

**/
STATIC
TPM_RESULT
TpmCommandNoReturnData (
  IN     EFI_TCG_PROTOCOL          *TcgProtocol,
  IN     TPM_COMMAND_CODE          Ordinal,
  IN     UINTN                     AdditionalParameterSize,
  IN     VOID                      *AdditionalParameters
  )
{
  EFI_STATUS                       Status;
  TPM_RQU_COMMAND_HDR              *TpmRqu;
  TPM_RSP_COMMAND_HDR              TpmRsp;
  UINT32                           Size;

  TpmRqu = (TPM_RQU_COMMAND_HDR*)AllocatePool (
                                   sizeof (*TpmRqu) + AdditionalParameterSize
                                   );
  ASSERT (TpmRqu != NULL);
  if (TpmRqu == NULL) {
    return TPM_PP_BIOS_FAILURE;
  }

  TpmRqu->tag       = H2NS (TPM_TAG_RQU_COMMAND);
  Size              = (UINT32)(sizeof (*TpmRqu) + AdditionalParameterSize);
  TpmRqu->paramSize = H2NL (Size);
  TpmRqu->ordinal   = H2NL (Ordinal);
  gBS->CopyMem (TpmRqu + 1, AdditionalParameters, AdditionalParameterSize);

  Status = TcgProtocol->PassThroughToTpm (
                          TcgProtocol,
                          Size,
                          (UINT8*)TpmRqu,
                          (UINT32)sizeof (TpmRsp),
                          (UINT8*)&TpmRsp
                          );
  ASSERT_EFI_ERROR (Status);
  ASSERT (TpmRsp.tag == H2NS (TPM_TAG_RSP_COMMAND));
  return H2NL (TpmRsp.returnCode);
}

/**

    GC_TODO: add routine description

    @param TpmEnable - GC_TODO: add arg description

    @retval Status      - GC_TODO: add retval description
    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
STATIC
EFI_STATUS
EnableTpmDevice (
  IN     BOOLEAN                   TpmEnable
  )
{
  EFI_STATUS                       Status;
  EFI_TCG_PROTOCOL                 *TcgProtocol;

  // Load TCG protocol
  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, &TcgProtocol);
  ASSERT_EFI_ERROR (Status);

  //
  // Enable/Disable TPM
  //
  if (TpmEnable == TRUE) {
    Status = TpmCommandNoReturnData (
               TcgProtocol,
               TPM_ORD_PhysicalEnable,
               0,
               NULL
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[TPM] Fail to enable TPM device!\n"));
      return Status;
    }
  } else {
    Status = TpmCommandNoReturnData (
               TcgProtocol,
               TPM_ORD_PhysicalDisable,
               0,
               NULL
               );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[TPM] Fail to disable TPM device!\n"));
      return Status;
    }
  }

  // Update TPM variable
  Status = SetOptionData (&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, TpmEnable), &TpmEnable, sizeof(TpmEnable));
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "[TPM] Fail to update TPM NV Variable!(0x%x)\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**

    GC_TODO: add routine description

    @param TpmActive - GC_TODO: add arg description

    @retval Status      - GC_TODO: add retval description
    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
STATIC
EFI_STATUS
ActivateTpmDevice (
  IN     BOOLEAN                   TpmActive
  )
{
  EFI_STATUS                       Status;
  EFI_TCG_PROTOCOL                 *TcgProtocol;
  BOOLEAN                          BoolVal;

  // Load TCG protocol
  Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, &TcgProtocol);
  ASSERT_EFI_ERROR (Status);

  //
  // Activate/Deactivate TPM
  //
  if (TpmActive == TRUE) {
    BoolVal = FALSE;
    Status = TpmCommandNoReturnData (
               TcgProtocol,
               TPM_ORD_PhysicalSetDeactivated,
               sizeof (BoolVal),
               &BoolVal
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[TPM] Fail to activate TPM device!\n"));
      return Status;
    }
  } else {
    BoolVal = TRUE;
    Status = TpmCommandNoReturnData (
               TcgProtocol,
               TPM_ORD_PhysicalSetDeactivated,
               sizeof (BoolVal),
               &BoolVal
               );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[TPM] Fail to deactivate TPM device!\n"));
      return Status;
    }
  }

  // Update TPM variable
  Status = SetOptionData (&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, TpmState), &TpmActive, sizeof(TpmActive));
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "[TPM] Fail to update TPM NV Variable!(0x%x)\n", Status));
    return Status;
  }
  return EFI_SUCCESS;
}

/**

    GC_TODO: add routine description

    @param StringId - GC_TODO: add arg description

    @retval EFI_SUCCESS   - GC_TODO: add retval description
    @retval EFI_NOT_READY - GC_TODO: add retval description
    @retval Status        - GC_TODO: add retval description

**/
EFI_STATUS
ValidatePassword (
  EFI_STRING_ID                   StringId
  )
{
  EFI_STATUS                      Status;
  CHAR16                          Password[PASSWORD_MAX_SIZE];
  UINT8                           EncodedPassword[SHA256_DIGEST_LENGTH];
  UINTN                           EncodedPasswordBufferSize = SHA256_DIGEST_LENGTH;
  EFI_STRING                      TempPassword;
  UINT8                           ZeroArray[SHA256_DIGEST_LENGTH];

  ZeroMem (&ZeroArray, SHA256_DIGEST_LENGTH);

  if (CompareMem(SecurityData->PasswordLocation, ZeroArray, SHA256_DIGEST_LENGTH) == 0) {
    //
    // Old password not exist
    //
    return EFI_SUCCESS;
  }

  //
  // Get user input password
  //
  TempPassword = HiiGetString (mSecurityHiiHandle, StringId, NULL);
  if (TempPassword == NULL) {
    return EFI_NOT_READY;
  }
  if (StrLen (TempPassword) >= PASSWORD_MAX_SIZE) {
    FreePool (TempPassword);
    return EFI_NOT_READY;
  }

  Status = StrnCpyS (Password, PASSWORD_MAX_SIZE, TempPassword, (PASSWORD_MAX_SIZE - 1));
  ASSERT_EFI_ERROR(Status);
  FreePool (TempPassword);
  if (EFI_ERROR(Status)) return Status;
  //
  // Validate old password
  //
  // APTIOV_SERVER_OVERRIDE_RC_START : To resolve build error
  //EncodePassword (Password, EncodedPassword, &EncodedPasswordBufferSize);
  // APTIOV_SERVER_OVERRIDE_RC_END : To resolve build error
  if (CompareMem (EncodedPassword, SecurityData->PasswordLocation, SHA256_DIGEST_LENGTH) != 0) {
    //
    // Old password mismatch, return EFI_NOT_READY to prompt for error message
    //
    Status = EFI_NOT_READY;
  } else {
    Status = EFI_SUCCESS;
  }

  return Status;
}

/**

    GC_TODO: add routine description

    @param StringId - GC_TODO: add arg description

    @retval EFI_NOT_READY - GC_TODO: add retval description
    @retval Status        - GC_TODO: add retval description

**/
EFI_STATUS
SetPassword (
  EFI_STRING_ID                   StringId
  )
{
  EFI_STATUS                      Status;
  EFI_STRING                      Password;
  CHAR16                          TempPasswordString[PASSWORD_MAX_SIZE];
  UINTN                           EncodedPasswordBufferSize = SHA256_DIGEST_LENGTH;

  //
  // Get user input password
  //
  if (StringId == 0) {
    return EFI_NOT_READY;
  }
  Password = HiiGetString (mSecurityHiiHandle, StringId, NULL);
  if (Password == NULL) {
    return EFI_NOT_READY;
  }
  if (StrLen (Password) >= PASSWORD_MAX_SIZE) {
    FreePool (Password);
    return EFI_NOT_READY;
  }
  Status = StrnCpyS (TempPasswordString, PASSWORD_MAX_SIZE, Password, (PASSWORD_MAX_SIZE - 1));
  ASSERT_EFI_ERROR(Status);
  FreePool (Password);
  if (EFI_ERROR(Status)) return Status;
  if (StrLen (TempPasswordString) == 0) {
    //
    // Clearing password, so just set PasswordLocation to zero
    //
    ZeroMem (SecurityData->PasswordLocation, SHA256_DIGEST_LENGTH);
  } else {
    //
    // Encode and set password
    //
    // APTIOV_SERVER_OVERRIDE_RC_START : To resolve build error
    //EncodePassword(TempPasswordString, SecurityData->PasswordLocation, &EncodedPasswordBufferSize);
    // APTIOV_SERVER_OVERRIDE_RC_END : To resolve build error
  }
  Status = SetSpecificConfigGuid (&mSystemConfigurationGuid, &mSystemConfiguration);

  return Status;
}
/**

    GC_TODO: add routine description

    @param None

    @retval None

**/
VOID
ClearPassword(
)
{
  EFI_STATUS                      Status;

  //
  // Clear out the password data
  //
  ZeroMem (SecurityData->PasswordLocation, SHA256_DIGEST_LENGTH);

  Status = SetSpecificConfigGuid (&mSystemConfigurationGuid, &mSystemConfiguration);
  ASSERT_EFI_ERROR(Status);

}

/**

    GC_TODO: add routine description

    @param State - GC_TODO: add arg description

    @retval None

**/
STATIC
VOID
SetAdminIndicatorAndSavePassword (
  IN UINT8                      State
  )
{
}

/**

    GC_TODO: add routine description

    @param State - GC_TODO: add arg description

    @retval None

**/
STATIC
VOID
SetUserIndicatorAndSavePassword (
  IN UINT8                      State
  )
{
}

/**

    GC_TODO: add routine description

    @param This       - GC_TODO: add arg description
    @param Action     - GC_TODO: add arg description
    @param QuestionId - GC_TODO: add arg description
    @param Type       - GC_TODO: add arg description
    @param Value      - GC_TODO: add arg description
    @param OPTIONAL   - GC_TODO: add arg description

    @retval EFI_UNSUPPORTED       - GC_TODO: add retval description
    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_READY         - GC_TODO: add retval description
    @retval EFI_NOT_AVAILABLE_YET - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
EFIAPI
SecurityDriverCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
  )
{
  EFI_STATUS                                 Status;
  EFI_STRING                                 PasswordString = NULL;


  if (Action != EFI_BROWSER_ACTION_CHANGING && Action != EFI_BROWSER_ACTION_CHANGED) {
    //
    // Do nothing for other UEFI Action. Only do call back when data is changed.
    //
    return EFI_UNSUPPORTED;
  }

  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;

  switch (QuestionId) {
  case 0x1000:
    SecurityData = &mAdminPassword;
    switch (State) {
    case BROWSER_STATE_VALIDATE_PASSWORD:
      Status = ValidatePassword (Value->string);
      if (Status == EFI_SUCCESS) {
        State = BROWSER_STATE_SET_PASSWORD;
      } else {
        return EFI_NOT_READY;
      }
      break;

    case BROWSER_STATE_SET_PASSWORD:

      // User had Pressed only the Null Character , So update the statue as Admin password not set
      if(Value->string != 0){
        PasswordString = HiiGetString (mSecurityHiiHandle, Value->string, NULL);
        if (PasswordString == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }

        if (*PasswordString == 00)
          {
           //Clear User P/W also
           SecurityData = &mUserPassword;
           ClearPassword();
           SetUserIndicatorAndSavePassword (0x00);

           SecurityData = &mAdminPassword;
           ClearPassword();
           SetAdminIndicatorAndSavePassword (0x00);
           State = BROWSER_STATE_SET_PASSWORD;
           break;
          }
      Status = SetPassword (Value->string);
      SetAdminIndicatorAndSavePassword (0x01);
      }
      State = BROWSER_STATE_VALIDATE_PASSWORD;
      break;

    default:
      break;
    }
    break;

  case 0x1001:
    SecurityData = &mUserPassword;
    switch (State) {
    case BROWSER_STATE_VALIDATE_PASSWORD:
      Status = ValidatePassword (Value->string);
      if (Status == EFI_SUCCESS) {
        State = BROWSER_STATE_SET_PASSWORD;
      } else {
        return EFI_NOT_READY;
      }
      break;

    case BROWSER_STATE_SET_PASSWORD:

      // User has Pressed only the Null Character , So update the statue as password not set
      if(Value->string != 0){
        PasswordString = HiiGetString (mSecurityHiiHandle, Value->string, NULL);
        if (PasswordString == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        
        if (*PasswordString == 00)
          {
           ClearPassword(); // When USER pressed the enter key Clear the password
           SetUserIndicatorAndSavePassword (0x00);
           State = BROWSER_STATE_SET_PASSWORD;
           break;
          }
      Status = SetPassword (Value->string);

        SetUserIndicatorAndSavePassword (0x01);
       }
      State = BROWSER_STATE_VALIDATE_PASSWORD;
      break;

    default:
      break;
    }
    break;

  case 0x1002:
    //
    // Clear all the password
    //
    ZeroMem (&mSystemConfiguration.AdminPassword, PASSWORD_MAX_SIZE * sizeof (CHAR16));
    ZeroMem (&mSystemConfiguration.UserPassword, PASSWORD_MAX_SIZE * sizeof (CHAR16));
    Status = SetSpecificConfigGuid (&mSystemConfigurationGuid, &mSystemConfiguration);
    ASSERT_EFI_ERROR(Status);
    return EFI_NOT_AVAILABLE_YET;

  case 0x1003:
    SecurityData = &mAdminPassword;
    Status = ValidatePassword (Value->string);
    if (Status == EFI_SUCCESS) {
      //
      // Do clear password action
      //
      ZeroMem (SecurityData->PasswordLocation, SHA256_DIGEST_LENGTH);
      Status = SetSpecificConfigGuid (&mSystemConfigurationGuid, &mSystemConfiguration);
      ASSERT_EFI_ERROR(Status);
      SetAdminIndicatorAndSavePassword (0x0);
      return EFI_NOT_AVAILABLE_YET;
    } else {
      //
      // Not match the password
      //
      return EFI_NOT_READY;
    }
    break;

  default:
    break;
  }

  return Status;
}


/**

    GC_TODO: add routine description

    @param DriverHandle - GC_TODO: add arg description
    @param ConfigAccess - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
InstallSecurityCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_CONFIG_ACCESS_PROTOCOL *ConfigAccess
  )
{
  EFI_STATUS          Status;
  UINT8               ZeroArray[SHA256_DIGEST_LENGTH];

  ASSERT (ConfigAccess != NULL);
  
  if (ConfigAccess == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ConfigAccess->ExtractConfig = DefaultExtractConfig;
  ConfigAccess->RouteConfig   = DefaultRouteConfig;
  ConfigAccess->Callback      = SecurityDriverCallback;

  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  ConfigAccess
                  );
  ASSERT_EFI_ERROR (Status);

  ZeroMem(ZeroArray, SHA256_DIGEST_LENGTH);
  if (CompareMem(mAdminPassword.PasswordLocation, ZeroArray, SHA256_DIGEST_LENGTH) != 0) {
    SetAdminIndicatorAndSavePassword (0x01);
  } else if (CompareMem(mUserPassword.PasswordLocation, ZeroArray, SHA256_DIGEST_LENGTH) != 0) {
    SetUserIndicatorAndSavePassword (0x01);
  } else {
    SetAdminIndicatorAndSavePassword (0x00);
  }

  return EFI_SUCCESS;
}

/**

    GC_TODO: add routine description

    @param This       - GC_TODO: add arg description
    @param Action     - GC_TODO: add arg description
    @param QuestionId - GC_TODO: add arg description
    @param Type       - GC_TODO: add arg description
    @param Value      - GC_TODO: add arg description
    @param OPTIONAL   - GC_TODO: add arg description

    @retval EFI_SUCCESS     - GC_TODO: add retval description
    @retval Status          - GC_TODO: add retval description
    @retval EFI_UNSUPPORTED - GC_TODO: add retval description

**/
EFI_STATUS
EFIAPI
TpmDriverCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
  )
{
  EFI_STATUS                                 Status;
  BOOLEAN                                    TpmEnable;
  BOOLEAN                                    TpmActivated;

  Status = EFI_SUCCESS;

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN) {
    //
    // Upate TPM state when TPM Form is opened.
    // This will be done only in FORM_OPEN CallBack of question with ID 0x1000 from TPM Form.
    //
    if (QuestionId != 0x1000) {
      return EFI_SUCCESS;
    }

    //
    // Get the TPM state
    //
    Status = GetTpmState (&TpmEnable, &TpmActivated, NULL, NULL, NULL);
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR, "[TPM] Get GetTpmState error Status = %r\n", Status));
      return EFI_SUCCESS;
    }

    //
    // Update TPM variable if it differs from current TPM hardware state
    //
    if ((mSystemConfiguration.TpmEnable != TpmEnable) ||
        (mSystemConfiguration.TpmState  != TpmActivated)) {
      Status = SetOptionData (&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, TpmEnable), &TpmEnable, sizeof(TpmEnable));
      Status |= SetOptionData (&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, TpmState), &TpmActivated, sizeof(TpmActivated));
      if (EFI_ERROR(Status)) {
        return Status;
      }
    }

    return EFI_SUCCESS;
  }

  if (Action == EFI_BROWSER_ACTION_FORM_CLOSE) {
    //
    // Do nothing for UEFI CLOSE Action
    //
    return EFI_SUCCESS;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGING && Action != EFI_BROWSER_ACTION_CHANGED) {
    //
    // Do nothing for other UEFI Action. Only do call back when data is changed.
    //
    return EFI_UNSUPPORTED;
  }

  switch (QuestionId) {

  case 0x1000:
    //
    // Enable/disable TPM device
    //
    Status = EnableTpmDevice (Value->u8);
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR, "[TPM] Error occurs when enable/disable TPM!(0x%x) \n", Status));
      return Status;
    }
    *ActionRequest = EFI_BROWSER_ACTION_REQUEST_SUBMIT;
    break;

  case 0x1001:
    //
    // Deactivate TPM device
    //
    Status = ActivateTpmDevice (Value->u8);
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR, "[TPM] Error occurs when activate/deactivate TPM!(0x%x) \n", Status));
      return Status;
    }
    *ActionRequest = EFI_BROWSER_ACTION_REQUEST_SUBMIT;
    break;


  case 0x1002:
    //
    // TPM Force Clear
    //
    Status = TpmForceClear ();
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR, "[TPM] Error occurs when force clear TPM!(0x%x) \n", Status));
      return Status;
    }
    *ActionRequest = EFI_BROWSER_ACTION_REQUEST_SUBMIT;
    break;

  case 0x1003:
    //
    // Enable MOR
    //
    Status = EnableMor (Value->u8);
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR, "[MOR] Error occurs when enable/disable Mor!(0x%x) \n", Status));
      return Status;
    }
    break;

  default:
    Status = EFI_SUCCESS;
    break;
  }

  return Status;
}


/**

    GC_TODO: add routine description

    @param DriverHandle - GC_TODO: add arg description
    @param ConfigAccess - GC_TODO: add arg description

    @retval EFI_SUCCESS - GC_TODO: add retval description

**/
EFI_STATUS
InstallTpmCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_CONFIG_ACCESS_PROTOCOL *ConfigAccess
  )
{
  EFI_STATUS          Status;

  ASSERT (ConfigAccess != NULL);

  if (ConfigAccess == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ConfigAccess->ExtractConfig = DefaultExtractConfig;
  ConfigAccess->RouteConfig   = DefaultRouteConfig;
  ConfigAccess->Callback      = TpmDriverCallback;

  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  ConfigAccess
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
