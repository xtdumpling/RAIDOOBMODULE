//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
// APTIOV_SERVER_OVERRIDE_START : Removed unnecessary header files and added AMI needs
#if 0
#include <Library/PrintLib.h>
#endif
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#if 0
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Library/TimerLib.h>
#include <Library/PasswordEncodeLib.h>
#endif
#include <Library/PttHeciLib.h>

#if 0
#include "../../../../SecurityPkg/Tcg/Tcg2Config/Tcg2ConfigNvData.h"
#endif
#include <Guid/SocketProcessorCoreVariable.h>
#include <Guid/SocketIioVariable.h>
#include <Guid/SetupVariable.h>
#include <Guid/MeRcVariable.h>

#include <commonoem.h>           //For Password Encoding
#include "PpixOobTask.h"
#include "PpixOobPlatform.h"
#include <Token.h>
// APTIOV_SERVER_OVERRIDE_RC_END : Necessary header files needed to support AMI modules

#define	JMP_$()	{volatile int j; j = 1; while (j);}
#define	PPI_USER_CONFIRMATION_TIME_OUT_M	2		// M, Max wait time in Minutes for User Response
#define	PPI_USER_CONFORMATION_INTERVAL_S	2		// S, Delay in Seconds between successive checking for User Response
								// where S <> 0 and 0 < S <= 60
#define CONFIRM_BUFFER_SIZE         4096			// Max size of buffer in bytes

//-----------------------------------------------------------------------------------------------------
//
EFI_GUID		mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;	// GUID of SYSTEM_CONFIGURATION
UINTN			mSystemConfigurationSize = 0;				// Size of SYSTEM_CONFIGURATION
//APTIOV_SERVER_OVERRIDE_RC_START : To get AMI's TCG setup data
EFI_GUID                          gSetupGuid = SETUP_GUID;
// APTIOV_SERVER_OVERRIDE_RC_END : To get AMI's TCG setup data
//
//-----------------------------------------------------------------------------------------------------
//APTIOV_SERVER_OVERRIDE_RC_START : TPM2 Clear command ported to use AMI's protocol
/**
  Copy AuthSessionIn to TPM2 command buffer.

  @param [in]  AuthSessionIn   Input AuthSession data
  @param [out] AuthSessionOut  Output AuthSession data in TPM2 command buffer

  @return AuthSession size
**/
UINT32
EFIAPI
CopyAuthSessionCommand (
  IN      TPMS_AUTH_COMMAND         *AuthSessionIn, OPTIONAL
  OUT     UINT8                     *AuthSessionOut
  )
{
  UINT8  *Buffer;

  Buffer = (UINT8 *)AuthSessionOut;
  
  //
  // Add in Auth session
  //
  if (AuthSessionIn != NULL) {
    //  sessionHandle
    WriteUnaligned32 ((UINT32 *)Buffer, SwapBytes32(AuthSessionIn->sessionHandle));
    Buffer += sizeof(UINT32);

    // nonce
    WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16 (AuthSessionIn->nonce.size));
    Buffer += sizeof(UINT16);

    CopyMem (Buffer, AuthSessionIn->nonce.buffer, AuthSessionIn->nonce.size);
    Buffer += AuthSessionIn->nonce.size;

    // sessionAttributes
    *(UINT8 *)Buffer = *(UINT8 *)&AuthSessionIn->sessionAttributes;
    Buffer++;

    // hmac
    WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16 (AuthSessionIn->hmac.size));
    Buffer += sizeof(UINT16);

    CopyMem (Buffer, AuthSessionIn->hmac.buffer, AuthSessionIn->hmac.size);
    Buffer += AuthSessionIn->hmac.size;
  } else {
    //  sessionHandle
    WriteUnaligned32 ((UINT32 *)Buffer, SwapBytes32(TPM_RS_PW));
    Buffer += sizeof(UINT32);

    // nonce = nullNonce
    WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16(0));
    Buffer += sizeof(UINT16);

    // sessionAttributes = 0
    *(UINT8 *)Buffer = 0x00;
    Buffer++;

    // hmac = nullAuth
    WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16(0));
    Buffer += sizeof(UINT16);
  }

  return (UINT32)(UINTN)(Buffer - (UINT8 *)AuthSessionOut);
}

/**
  This command removes all TPM context associated with a specific Owner.

  @param[in] AuthHandle        TPM_RH_LOCKOUT or TPM_RH_PLATFORM+{PP}
  @param[in] AuthSession       Auth Session context
 
  @retval EFI_SUCCESS      Operation completed successfully.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm2Clear (
  IN TPMI_RH_CLEAR             AuthHandle,
  IN TPMS_AUTH_COMMAND         *AuthSession, OPTIONAL
  IN EFI_TREE_PROTOCOL         *Tcg2Protocol
  )
{
  EFI_STATUS                        Status;
  TPM2_CLEAR_COMMAND                Cmd;
  TPM2_CLEAR_RESPONSE               Res;
  UINT32                            ResultBufSize;
  UINT32                            CmdSize;
  UINT32                            RespSize;
  UINT8                             *Buffer;
  UINT32                            SessionInfoSize;

  Cmd.Header.tag         = SwapBytes16(TPM_ST_SESSIONS);
  Cmd.Header.commandCode = SwapBytes32(TPM_CC_Clear);
  Cmd.Auth               = SwapBytes32(AuthHandle);
  
  DEBUG ((EFI_D_ERROR, "\nIn Tpm2Clear\r\n"));

  //
  // Add in Auth session
  //
  Buffer = (UINT8 *)&Cmd.PwapAuth;

  // sessionInfoSize
  SessionInfoSize = CopyAuthSessionCommand (AuthSession, Buffer);
  Buffer += SessionInfoSize;
  Cmd.AuthorizationSize = SwapBytes32(SessionInfoSize);

  CmdSize = (UINT32)(Buffer - (UINT8 *)&Cmd);
  Cmd.Header.paramSize   = SwapBytes32(CmdSize);

  ResultBufSize = sizeof(Res);
  //APTIOV_SERVER_OVERRIDE_RC_START : TPM2 Clear command ported to use AMI's protocol
#if 0
  Status = Tpm2SubmitCommand (CmdSize, (UINT8 *)&Cmd, &ResultBufSize, (UINT8 *)&Res);
#else
  Status = Tcg2Protocol->SubmitCommand(Tcg2Protocol, CmdSize, (UINT8 *)&Cmd, ResultBufSize, (UINT8 *)&Res);
#endif
//APTIOV_SERVER_OVERRIDE_RC_END : TPM2 Clear command ported to use AMI's protocol

  DEBUG ((EFI_D_ERROR, "\nTcg2Protocol->SubmitCommand %r\r\n",Status));
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  if (ResultBufSize > sizeof(Res)) {
    DEBUG ((EFI_D_ERROR, "Clear: Failed ExecuteCommand: Buffer Too Small\r\n"));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Validate response headers
  //
  RespSize = SwapBytes32(Res.Header.paramSize);
  if (RespSize > sizeof(Res)) {
    DEBUG ((EFI_D_ERROR, "Clear: Response size too large! %d\r\n", RespSize));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Fail if command failed
  //
  if (SwapBytes32(Res.Header.responseCode) != TPM_RC_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Clear: Response Code error! 0x%08x\r\n", SwapBytes32(Res.Header.responseCode)));
    Status = EFI_DEVICE_ERROR;
    goto Done;
  }

  //
  // Unmarshal the response
  //

  // None
Done:
  //
  // Clear AuthSession Content
  //
  ZeroMem (&Cmd, sizeof(Cmd));
  ZeroMem (&Res, sizeof(Res));
  return Status;
}

/**
  Disables and enables the execution of TPM2_Clear().

  @param[in] AuthHandle        TPM_RH_LOCKOUT or TPM_RH_PLATFORM+{PP}
  @param[in] AuthSession       Auth Session context
  @param[in] Disable           YES if the disableOwnerClear flag is to be SET,
                               NO if the flag is to be CLEAR.

  @retval EFI_SUCCESS      Operation completed successfully.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm2ClearControl (
  IN TPMI_RH_CLEAR             AuthHandle,
  IN TPMS_AUTH_COMMAND         *AuthSession, OPTIONAL
  IN TPMI_YES_NO               Disable,
  IN EFI_TREE_PROTOCOL         *Tcg2Protocol
  )
{
  EFI_STATUS                        Status;
  TPM2_CLEAR_CONTROL_COMMAND        Cmd;
  TPM2_CLEAR_CONTROL_RESPONSE       Res;
  UINT32                            ResultBufSize;
  UINT32                            CmdSize;
  UINT32                            RespSize;
  UINT8                             *Buffer;
  UINT32                            SessionInfoSize;

  Cmd.Header.tag         = SwapBytes16(TPM_ST_SESSIONS);
  Cmd.Header.commandCode = SwapBytes32(TPM_CC_ClearControl);
  Cmd.Auth               = SwapBytes32(AuthHandle);

  //
  // Add in Auth session
  //
  Buffer = (UINT8 *)&Cmd.PwapAuth;

  // sessionInfoSize
  SessionInfoSize = CopyAuthSessionCommand (AuthSession, Buffer);
  Buffer += SessionInfoSize;
  Cmd.AuthorizationSize = SwapBytes32(SessionInfoSize);

  // disable
  *(UINT8 *)Buffer = Disable;
  Buffer++;

  CmdSize = (UINT32)(Buffer - (UINT8 *)&Cmd);
  Cmd.Header.paramSize   = SwapBytes32(CmdSize);

  ResultBufSize = sizeof(Res);
  //APTIOV_SERVER_OVERRIDE_RC_START : TPM2 Clear command ported to use AMI's protocol
#if 0
  Status = Tpm2SubmitCommand (CmdSize, (UINT8 *)&Cmd, &ResultBufSize, (UINT8 *)&Res);
#else
  Status = Tcg2Protocol->SubmitCommand(Tcg2Protocol, CmdSize, (UINT8 *)&Cmd, ResultBufSize, (UINT8 *)&Res);
#endif
//APTIOV_SERVER_OVERRIDE_RC_END : TPM2 Clear command ported to use AMI's protocol
  DEBUG ((EFI_D_ERROR, "\nTcg2Protocol->SubmitCommand %r\r\n",Status));
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  if (ResultBufSize > sizeof(Res)) {
    DEBUG ((EFI_D_ERROR, "ClearControl: Failed ExecuteCommand: Buffer Too Small\r\n"));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Validate response headers
  //
  RespSize = SwapBytes32(Res.Header.paramSize);
  if (RespSize > sizeof(Res)) {
    DEBUG ((EFI_D_ERROR, "ClearControl: Response size too large! %d\r\n", RespSize));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Fail if command failed
  //
  if (SwapBytes32(Res.Header.responseCode) != TPM_RC_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "ClearControl: Response Code error! 0x%08x\r\n", SwapBytes32(Res.Header.responseCode)));
    Status = EFI_DEVICE_ERROR;
    goto Done;
  }

  //
  // Unmarshal the response
  //

  // None
Done:
  //
  // Clear AuthSession Content
  //
  ZeroMem (&Cmd, sizeof(Cmd));
  ZeroMem (&Res, sizeof(Res));
  return Status;
}

/**
  Send ClearControl and Clear command to TPM.

  @param[in]  PlatformAuth      platform auth value. NULL means no platform auth change.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_DEVICE_ERROR      Unexpected device behavior.

**/

EFI_STATUS
EFIAPI
Tpm2CommandClear (
  IN TPM2B_AUTH           *PlatformAuth,  OPTIONAL
  IN EFI_TREE_PROTOCOL    *Tcg2Protocol
  )
{
  EFI_STATUS                Status;
  TPMS_AUTH_COMMAND         *AuthSession;
  TPMS_AUTH_COMMAND         LocalAuthSession;

  if (PlatformAuth == NULL) {
    AuthSession = NULL;
  } else {
    AuthSession = &LocalAuthSession;
    ZeroMem (&LocalAuthSession, sizeof(LocalAuthSession));
    LocalAuthSession.sessionHandle = TPM_RS_PW;
    LocalAuthSession.hmac.size = PlatformAuth->size;
    CopyMem (LocalAuthSession.hmac.buffer, PlatformAuth->buffer, PlatformAuth->size);
  }

  DEBUG ((EFI_D_INFO, "Tpm2ClearControl ... \n"));
  Status = Tpm2ClearControl (TPM_RH_PLATFORM, AuthSession, NO, Tcg2Protocol);
  DEBUG ((EFI_D_INFO, "Tpm2ClearControl - %r\n", Status));
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  DEBUG ((EFI_D_INFO, "Tpm2Clear ... \n"));
  Status = Tpm2Clear (TPM_RH_PLATFORM, AuthSession, Tcg2Protocol);
  DEBUG ((EFI_D_INFO, "Tpm2Clear - %r\n", Status));

Done:
  ZeroMem (&LocalAuthSession.hmac, sizeof(LocalAuthSession.hmac));
  return Status;
}
//APTIOV_SERVER_OVERRIDE_RC_END : TPM2 Clear command ported to use AMI's protocol
//-----------------------------------------------------------------------------------------------------
//---------------------	TPM1.2 Functions { ------------------------------------------------------------
/**
  Get TPM1.2 Permanent Flags.

  @param[in]	Opd		Ptr to OT_PLATFORM_DATA

  @retval	TPM_RESULT	TPM_SUCCESS, Success
				Other, Error

**/
TPM_RESULT
OT_GetTpm12PermanentFlags (
  IN  OT_PLATFORM_DATA	*Opd
  )
{
  EFI_STATUS		Status;
  TPM_RSP_COMMAND_HDR	*Rsp;
  UINT32		SendSize, t;
  UINT16		w;
  UINT8			CmdBuf[64], *ppf;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_GetTpm12PermanentFlags()  { -> Get TPM1.2 Permanent Flags\n"));

  // Validate input parameters
  t = TPM_SUCCESS;							// Assume success
  if ((Opd == NULL) || (Opd->TcgProtocol == NULL)) {			// Error
    DEBUG ((EFI_D_ERROR, "      Invalid ptr to OT_PLATFORM_DATA or EFI_TCG_PROTOCOL\n"));
    t = TPM_BAD_PARAMETER;
  } else {
    // Get TPM Permanent flags
    SendSize              = sizeof (TPM_RQU_COMMAND_HDR) + sizeof (UINT32) * 3;
    *(UINT16*)&CmdBuf[0]  = SwapBytes16 (TPM_TAG_RQU_COMMAND);
    *(UINT32*)&CmdBuf[2]  = SwapBytes32 (SendSize);
    *(UINT32*)&CmdBuf[6]  = SwapBytes32 (TPM_ORD_GetCapability);
  
    *(UINT32*)&CmdBuf[10] = SwapBytes32 (TPM_CAP_FLAG);
    *(UINT32*)&CmdBuf[14] = SwapBytes32 (sizeof (TPM_CAP_FLAG_PERMANENT));
    *(UINT32*)&CmdBuf[18] = SwapBytes32 (TPM_CAP_FLAG_PERMANENT);

    Status = Opd->TcgProtocol->PassThroughToTpm (Opd->TcgProtocol,
					SendSize, CmdBuf,
					sizeof (CmdBuf), CmdBuf);

    Rsp = (TPM_RSP_COMMAND_HDR*) &CmdBuf[0];
    t = SwapBytes32 (Rsp->returnCode); w = SwapBytes16 (Rsp->tag);
    DEBUG ((EFI_D_ERROR, "      EFI Status 0x%08x, TPM Response Tag 0x%02x, TPM Return Code 0x%04x --> ", Status, w, t));
    if ((Status != EFI_SUCCESS) || (w != TPM_TAG_RSP_COMMAND) || (t != TPM_SUCCESS)) {	// Error
      t = TPM_FAIL;
      DEBUG ((EFI_D_ERROR, "Error\n"));
    } else {
      ppf = (UINT8*) &CmdBuf[sizeof (TPM_RSP_COMMAND_HDR) + sizeof (UINT32)];	// Ptr to TPM_PERMANENT_FLAGS
      CopyMem ((UINT8*) &Opd->Tpm12PermanentFlags, ppf, sizeof (TPM_PERMANENT_FLAGS)); // Save in OT_PLATFORM_DATA
      DEBUG ((EFI_D_ERROR, "Success\n"));
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_GetTpm12PermanentFlags()  } -> TPM Result: 0x%04x ", t));
  if (t == TPM_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return t;
}

/**
  Issue TSC_PhysicalPresence command to TPM.

  @param[in] TcgProtocol	EFI TCG Protocol instance.  
  @param[in] PhysicalPresence	The state to set the TPM's Physical Presence flags.  
  
  @retval	TPM_RESULT	TPM_SUCCESS, Success
				Other, Error
**/
TPM_RESULT
OT_Tpm12PhysicalPresence (
  IN  EFI_TCG_PROTOCOL		*TcgProtocol,
  IN  TPM_PHYSICAL_PRESENCE	PhysicalPresence
  )
{
  EFI_STATUS		Status;
  TPM_RQU_COMMAND_HDR	*TpmRqu;
  TPM_PHYSICAL_PRESENCE	*TpmPp;
  TPM_RSP_COMMAND_HDR	TpmRsp;
  UINT8			Buffer[sizeof (*TpmRqu) + sizeof (*TpmPp)];
  UINT16		w;
  UINT32		t;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_Tpm12PhysicalPresence()  { -> PhysicalPresence 0x%02x\n", (UINT16)(UINTN) PhysicalPresence));

  t = TPM_SUCCESS;							// Assume success
  if (TcgProtocol == NULL) {						// Error
    DEBUG ((EFI_D_ERROR, "      Invalid ptr to EFI_TCG_PROTOCOL\n"));
    t = TPM_BAD_PARAMETER;
  } else {
    TpmRqu = (TPM_RQU_COMMAND_HDR*)Buffer;
    TpmPp = (TPM_PHYSICAL_PRESENCE*)(TpmRqu + 1);

    TpmRqu->tag       = SwapBytes16 (TPM_TAG_RQU_COMMAND);
    TpmRqu->paramSize = SwapBytes32 (sizeof (Buffer));
    TpmRqu->ordinal   = SwapBytes32 (TSC_ORD_PhysicalPresence);
    WriteUnaligned16 (TpmPp, (TPM_PHYSICAL_PRESENCE) SwapBytes16 (PhysicalPresence));  

    Status = TcgProtocol->PassThroughToTpm (TcgProtocol,
                          sizeof (Buffer), (UINT8*)TpmRqu,
                          sizeof (TpmRsp), (UINT8*)&TpmRsp);

    t = SwapBytes32 (TpmRsp.returnCode); w = SwapBytes16 (TpmRsp.tag);
    DEBUG ((EFI_D_ERROR, "      EFI Status 0x%08x, TPM Response Tag 0x%02x, TPM Return Code 0x%04x\n", Status, w, t));
   if ((Status != EFI_SUCCESS) || (w != TPM_TAG_RSP_COMMAND) || (t != TPM_SUCCESS)) {
     if (t == TPM_SUCCESS) { t = TPM_FAIL; }
   }
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_Tpm12PhysicalPresence()  } -> TPM Result 0x%04x -> ", t));
  if (t == TPM_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return t;
}

/**
  Issue a TPM command for which no additional output data will be returned.

  @param[in] TcgProtocol		EFI TCG Protocol instance
  @param[in] Ordinal			TPM command code
  @param[in] AdditionalParameterSize	Additional parameter size
  @param[in] AdditionalParameters	Pointer to the Additional paramaters
  
  @retval	TPM_RESULT		TPM_SUCCESS, Success
					Other, Error
**/
TPM_RESULT
OT_Tpm12CommandNoReturnData (
  IN  EFI_TCG_PROTOCOL	*TcgProtocol,
  IN  TPM_COMMAND_CODE	Ordinal,
  IN  UINTN		AdditionalParameterSize,
  IN  VOID		*AdditionalParameters
  )
{
  EFI_STATUS		Status;
  TPM_RQU_COMMAND_HDR	*TpmRqu;
  TPM_RSP_COMMAND_HDR	TpmRsp;
  UINT32		Size, t;
  UINT16		w;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_Tpm12CommandNoReturnData()  { -> Issue TPM command 0x%02x\n", Ordinal));

  t = TPM_SUCCESS; TpmRqu = NULL;
  if (TcgProtocol == NULL) {					// Error
    DEBUG ((EFI_D_ERROR, "      Invalid ptr to EFI_TCG_PROTOCOL\n"));
    t = TPM_BAD_PARAMETER;
  } else {
    TpmRqu = (TPM_RQU_COMMAND_HDR*) AllocatePool (sizeof (*TpmRqu) + AdditionalParameterSize);
    if (TpmRqu == NULL) {					// Error
      DEBUG ((EFI_D_ERROR, "      Memory not available\n"));
      t = TPM_FAIL;
    }
  }

  if (t == TPM_SUCCESS) {					// If success so far
    TpmRqu->tag       = SwapBytes16 (TPM_TAG_RQU_COMMAND);
    Size              = (UINT32)(sizeof (*TpmRqu) + AdditionalParameterSize);
    TpmRqu->paramSize = SwapBytes32 (Size);
    TpmRqu->ordinal   = SwapBytes32 (Ordinal);
    CopyMem (TpmRqu + 1, AdditionalParameters, AdditionalParameterSize);

    Status = TcgProtocol->PassThroughToTpm (TcgProtocol,
                          Size, (UINT8*)TpmRqu,
                          (UINT32)sizeof (TpmRsp), (UINT8*)&TpmRsp);

    t = SwapBytes32 (TpmRsp.returnCode); w = SwapBytes16 (TpmRsp.tag);
    DEBUG ((EFI_D_ERROR, "      EFI Status 0x%08x, TPM Response Tag 0x%02x, TPM Return Code 0x%04x\n", Status, w, t));
    if ((Status != EFI_SUCCESS) || (w != TPM_TAG_RSP_COMMAND) || (t != TPM_SUCCESS)) {
      if (t == TPM_SUCCESS) { t = TPM_FAIL; }
    }
  }

  if (TpmRqu != NULL) { FreePool (TpmRqu); }			// Dellocate memory (if allocated)

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_Tpm12CommandNoReturnData()  } -> TPM Result 0x%04x -> ", t));
  if (t == TPM_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return t;
}

//-----------------------------------------------------------------------------------------------------
/**
  OT_Tpm12DeactivateDisable(): Deactivate, Disable TPM.

  @param[in]  Opd	INPUT: Ptr to OT_PLATFORM_DATA
  @retval     Result	OUTPUT: UINT8, Status code

**/
UINT8
OT_Tpm12DeactivateDisable (
  IN OUT OT_PLATFORM_DATA	*Opd
  )
{
  // Local Data
  UINT8		e;
  BOOLEAN	b;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_Tpm12DeactivateDisable()  { -> TPM1.2: Deactivate, Disable\n"));

  // Deactivate TPM1.2, if necessary
  e = OXP_SUCCESS;						// Assume success
  if (!(Opd->Tpm12PermanentFlags.disable)) {			// If currently enabled
    DEBUG ((EFI_D_ERROR, "      Deactivate TPM1.2:\n"));
    if (!(Opd->Tpm12PermanentFlags.deactivated)) {		//   If currently activated
      b = TRUE;
      if ((OT_Tpm12CommandNoReturnData (Opd->TcgProtocol,	//     Deactivate TPM
		TPM_ORD_PhysicalSetDeactivated, sizeof (b), &b)) == TPM_SUCCESS) {
	Opd->Tpm12PermanentFlags.deactivated = TRUE;		//       Indicate Deactivated
	Opd->Tpm12ActivateStateChange = TRUE;			//       Indicate TPM ctivate State Changed
	DEBUG ((EFI_D_ERROR, "          Success -> Activate State Changed, Reboot Required\n"));
      } else {							//   If not success
	e = OXP_DTPM_DEACTIVATE_ERROR;				//     Error
      }
    } else {
      DEBUG ((EFI_D_ERROR, "          Already Deactivated\n"));
    }
  }

  // Disable TPM1.2, if necessary
  if (e == OXP_SUCCESS) {					// If success so far
    DEBUG ((EFI_D_ERROR, "      Disable TPM1.2:\n"));
    if (!(Opd->Tpm12PermanentFlags.disable)) {			//   If currently enabled
      if ((OT_Tpm12CommandNoReturnData (Opd->TcgProtocol,	//     Disable TPM
			TPM_ORD_PhysicalDisable, 0, NULL)) == TPM_SUCCESS) {
	Opd->Tpm12PermanentFlags.disable = TRUE;		//       Indicate Disabled
      } else {							//     If not success
	e = OXP_DTPM_DISABLE_ERROR;				//       Error
      }
    } else {							// if currently disabled
      DEBUG ((EFI_D_ERROR, "          Already Disabled\n"));
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_Tpm12DeactivateDisable()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status code
}

//-----------------------------------------------------------------------------------------------------
/**
  OT_Tpm12EnableActivate(): Enable, Activate TPM.

  @param[in]  Opd	INPUT: Ptr to OT_PLATFORM_DATA
  @param[in]  Always	INPUT: BOOLEAN, TRUE  --> Issue Enable and Activate always
					FALSE --> Issue Enable and Activate if necessary

  @retval     Result	OUTPUT: UINT8, Status code

**/
UINT8
OT_Tpm12EnableActivate (
  IN  OT_PLATFORM_DATA	*Opd,
  IN  BOOLEAN		Always
  )
{
  // Local Data
  UINT8		e;
  BOOLEAN	b;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_Tpm12EnableActivate()  { -> TPM1.2: Enable, Activate\n"));

  // Enable TPM1.2, if necessary
  e = OXP_SUCCESS;						// Assume Success
  DEBUG ((EFI_D_ERROR, "      Enable TPM1.2:\n"));
  if ((Always) || (Opd->Tpm12PermanentFlags.disable)) {		// If always enable OR currently disabled
    if ((OT_Tpm12CommandNoReturnData (Opd->TcgProtocol,		//   Enable TPM
	TPM_ORD_PhysicalEnable, 0, NULL)) == TPM_SUCCESS) {	//   If success
      Opd->Tpm12PermanentFlags.disable = FALSE;			//     Indicate Enabled
    } else {							//   If not success
      e = OXP_DTPM_ENABLE_ERROR;				//   Error
    }
  } else {
    DEBUG ((EFI_D_ERROR, "          Already Enabled\n"));
  }


  // Activate TPM1.2, if necessary
  if (e == OXP_SUCCESS) {					// If success so far
    DEBUG ((EFI_D_ERROR, "      Activate TPM1.2:\n"));
    if ((Always) || (Opd->Tpm12PermanentFlags.deactivated)) {	//   If always activate OR currently deactivated
      b = FALSE;
      if ((OT_Tpm12CommandNoReturnData (Opd->TcgProtocol,	//     Activate TPM
		TPM_ORD_PhysicalSetDeactivated, sizeof (b), &b)) == TPM_SUCCESS) {
	Opd->Tpm12PermanentFlags.deactivated = FALSE;		//       Indicate Activated
	Opd->Tpm12ActivateStateChange = TRUE;			//       Indicate TPM Activate State Changed --> Reboot Required
	DEBUG ((EFI_D_ERROR, "          Success -> Activate State Changed, Reboot Required\n"));
      } else {							//     If not success
	e = OXP_DTPM_DEACTIVATE_ERROR;				//       Error
      }
    } else {
      DEBUG ((EFI_D_ERROR, "          Already Activated\n"));
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_Tpm12EnableActivate()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status code
}

//-----------------------------------------------------------------------------------------------------
/**
  OT_Tpm12ClearEnableActivate(): Clear, Enable, Activate TPM.

  @param[in]  Opd	INPUT: Ptr to OT_PLATFORM_DATA

  @retval     Result	OUTPUT: UINT8, Status code

**/
UINT8
OT_Tpm12ClearEnableActivate (
  IN  OT_PLATFORM_DATA	*Opd
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_Tpm12ClearEnableActivate()  { -> TPM1.2: Clear, Enable, Activate\n"));

  e = OXP_SUCCESS;						// Assume auccess
  if (Opd->Tpm12PermanentFlags.disable) {			// If disabled
    DEBUG ((EFI_D_ERROR, "      TPM not enabled\n"));
    e = OXP_DTPM_NOT_ENABLED;					//   Error
  } else {
    if (Opd->Tpm12PermanentFlags.deactivated) {			// If deactivated
      DEBUG ((EFI_D_ERROR, "      TPM not activated\n"));
      e = OXP_DTPM_NOT_ACTIVATED;				//   Error
    }
  }

  if (e == OXP_SUCCESS) {					// If success so far
    DEBUG ((EFI_D_ERROR, "      Clear TPM1.2:\n"));
    if ((OT_Tpm12CommandNoReturnData (Opd->TcgProtocol,		//   Force Clear TPM
			TPM_ORD_ForceClear, 0, NULL)) != TPM_SUCCESS) {
      e = OXP_DTPM_CLEAR_ERROR;					//     Error
    } else {							//   If Clear success
      e = OT_Tpm12EnableActivate (Opd, TRUE);			//     Enable, Activate (always)
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_Tpm12ClearEnableActivate()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status code
}

//-----------------------------------------------------------------------------------------------------
/**
  OT_PrepareTpmBeforeOobTask(): Prepare TPM before performing OOB Task.

  @param[in]  Opd	INPUT: Ptr to OT_PLATFORM_DATA

  @retval     Result	OUTPUT: UINT8, Status code

**/
UINT8
OT_PrepareTpmBeforeOobTask (
  IN     OT_PLATFORM_DATA	*Opd
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_PrepareTpmBeforeOobTask()  { -> Prepare TPM before performing OOB Task\n"));
  Opd->Tpm12PreparedForTask = FALSE;				// Indicate TPM preparation not performed before task
  e = OXP_SUCCESS;						// Status code (assume success)

  // Check if TPM1.2
  if (Opd->TcgProtocol == NULL) {				// If not TPM1.2
    DEBUG ((EFI_D_ERROR, "      Not TPM1.2, no preparation required\n"));
  } else {							// If TPM1.2
    // Prepare TPM1.2: Ensure TPM can accept physical presence commands
    if (!Opd->Tpm12PermanentFlags.physicalPresenceCMDEnable) {	// If TPM not enabled for accepting commands
      if (Opd->Tpm12PermanentFlags.physicalPresenceLifetimeLock) {//   If TPM Locked
	DEBUG ((EFI_D_ERROR, "      TPM1.2 Locked, cannot accept physical presence command\n"));
	e = OXP_TPM_LOCKED;					//     Error
      } else {							//   If TPM not locked
	DEBUG ((EFI_D_ERROR, "      Physical presence command enable:\n"));
	if ((OT_Tpm12PhysicalPresence (Opd->TcgProtocol,	//     Set physical presence command enable
		TPM_PHYSICAL_PRESENCE_CMD_ENABLE)) != TPM_SUCCESS) {
	  e = OXP_TPM_PHYSICAL_PRESENCE_CMD_ENABLE_ERROR;	//     Error
	}
      }
    }
  }

  // Set Physical presence flag
  if ((e == OXP_SUCCESS) && (Opd->TcgProtocol != NULL)) {	// If success AND TPM1.2
    DEBUG ((EFI_D_ERROR, "      Set Physical presence as Present:\n"));
    if ((OT_Tpm12PhysicalPresence (Opd->TcgProtocol,		// Set physical presence flag
		TPM_PHYSICAL_PRESENCE_PRESENT)) == TPM_SUCCESS) {
      Opd->Tpm12PreparedForTask = TRUE;				//   Indicate TPM preparation performed before task
    } else {
      e = OXP_TPM_PHYSICAL_PRESENCE_ERROR;			//   Error
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_PrepareTpmBeforeOobTask()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status code
}

//-----------------------------------------------------------------------------------------------------
/**
  OT_PrepareTpmAfterOobTask(): Prepare TPM after performing OOB Task.

  @param[in]  Opd	INPUT: Ptr to OT_PLATFORM_DATA

  @retval     Result	OUTPUT: UINT8, Status code

**/
UINT8
OT_PrepareTpmAfterOobTask (
  IN     OT_PLATFORM_DATA	*Opd
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_PrepareTpmAfterOobTask()  { -> Prepare TPM after performing OOB Task\n"));
  e = OXP_SUCCESS;						// Status code (assume success)

  //--------------------------------------------------------------
  // Check if TPM preparation performed before task
  if (!Opd->Tpm12PreparedForTask) {				// If TPM preparation not performed before task
    DEBUG ((EFI_D_ERROR, "      Nothing to do (TPM not prepared before task)\n"));
  } else {
    // Prepare TPM1.2
    if (Opd->TcgProtocol != NULL) {				// If TCG protocol valid
      DEBUG ((EFI_D_ERROR, "      Set physical presence as Not-Present:\n"));
      if ((OT_Tpm12PhysicalPresence (Opd->TcgProtocol,		// Lock physical presence
		TPM_PHYSICAL_PRESENCE_NOTPRESENT)) != TPM_SUCCESS) {
	e = OXP_TPM_PHYSICAL_PRESENCE_ERROR;			//   Error
      }
    } else {							// If TCG protocol not valid
      e = OXP_UNKNOWN_ERROR;					// Error
      DEBUG ((EFI_D_ERROR, "      Implementation Error -> BUG\n"));
    }
  }

  //--------------------------------------------------------------

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_PrepareTpmAfterOobTask()  } -> Status 0x%02x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status code
}

//-----------------------------------------------------------------------------------------------------
//---------------------	} TPM1.2 Functions ------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
/**
  DisplayBinaryData(): Display binary data.

  @param  Data	INPUT: UINT8*, Ptr to data
  @param  Size	INPUT: UINTN, Data size in bytes

  @retval None
**/
VOID
DisplayBinaryData (
  UINT8      *Data,
  UINTN     Size
  )
{
  // Local data
  UINTN		i, n;
  UINT8		b, *c, j, l;

  // Display 16bytes per line
  n = (Size >> 4);						// #of lines (each line is 16bytes)
  l = (UINT8) (Size & 0x0F);					// #of bytes in last line
  c = Data;							// Ptr to start of data bytes
  DEBUG ((EFI_D_ERROR, "        ----------------------------------------------------------------------\n"));
  DEBUG ((EFI_D_ERROR, "        Offset  00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\n"));
  DEBUG ((EFI_D_ERROR, "        ----------------------------------------------------------------------\n"));
  if (n != 0) {							// If #of full 16byte lines valid
    for (i = 0; i < n; i++) {					// Display all full 16byte lines
      DEBUG ((EFI_D_ERROR, "        %04X  ", (i << 4)));
      for (j = 0; j < 16; j++) {				//   Display all 16bytes in the line
        b = *c; c++;						//     Get next data byte, Update ptr to data
        DEBUG ((EFI_D_ERROR, "  %02X", b));			//     Display next data byte
      }
      DEBUG ((EFI_D_ERROR, "\n"));				//     Display <CR><LF>
    }
  }
  if (l != 0) {							// If last line valid
    DEBUG ((EFI_D_ERROR, "        %04X  ", (n << 4)));
    for (j = 0; j < l; j++) {					//   Display all bytes in last line
      b = *c; c++;						//     Get next data byte, Update ptr to data
      DEBUG ((EFI_D_ERROR, "  %02X", b));			//     Display next data byte
    }
    DEBUG ((EFI_D_ERROR, "\n"));
  }
  DEBUG ((EFI_D_ERROR, "        ----------------------------------------------------------------------\n"));
}
//
//-----------------------------------------------------------------------------------------------------
/**
  Display BIOS Setup Options.

  @param[in]  Opd	*OT_PLATFORM_DATA: Ptr to OT_PLATFORM_DATA
  @param[in]  Info	BOOLEAN: TRUE/FALSE: (Modified before Write)/(Current after Read)

  @retval     None

**/
VOID
OT_DisplayBiosSetupOption (
  IN OUT OT_PLATFORM_DATA	*Opd,
  IN BOOLEAN				Info
  )
{
  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_DisplayBiosSetupOption()  { -> BIOS Setup Options, "));
  if (Info) { DEBUG ((EFI_D_ERROR, "To be written if Modified\n")); }
  else { DEBUG ((EFI_D_ERROR, "Current as Read\n")); }

  DEBUG ((EFI_D_ERROR, "                                     LTSX: "));
  if (Opd->Spcc.ProcessorLtsxEnable == 0) { DEBUG ((EFI_D_ERROR, "Disabled\n")); }
  else { DEBUG ((EFI_D_ERROR, "Enabled\n")); }

  DEBUG ((EFI_D_ERROR, "                             Lock Chipset: "));
  if (Opd->Spcc.LockChipset == 0) { DEBUG ((EFI_D_ERROR, "Disabled\n")); }
  else { DEBUG ((EFI_D_ERROR, "Enabled\n")); }

  DEBUG ((EFI_D_ERROR, "                                      VMX: "));
  if (Opd->Spcc.ProcessorVmxEnable == 0) { DEBUG ((EFI_D_ERROR, "Disabled\n")); }
  else { DEBUG ((EFI_D_ERROR, "Enabled\n")); }

  DEBUG ((EFI_D_ERROR, "                                      SMX: "));
  if (Opd->Spcc.ProcessorSmxEnable == 0) { DEBUG ((EFI_D_ERROR, "Disabled\n")); }
  else { DEBUG ((EFI_D_ERROR, "Enabled\n")); }

  DEBUG ((EFI_D_ERROR, "                         MSR Lock Control: "));
  if (Opd->Spcc.ProcessorMsrLockControl == 0) { DEBUG ((EFI_D_ERROR, "Disabled\n")); }
  else { DEBUG ((EFI_D_ERROR, "Enabled\n")); }

  DEBUG ((EFI_D_ERROR, "                                     VT-d: "));
  if (Opd->Sic.VTdSupport == 0) { DEBUG ((EFI_D_ERROR, "Disabled\n")); }
  else { DEBUG ((EFI_D_ERROR, "Enabled\n")); }

  DEBUG ((EFI_D_ERROR, "                 Detected TPM Device (0x%x): ", Opd->T2dd.TpmDeviceDetected));
  switch (Opd->T2dd.TpmDeviceDetected) {
    case TPM_DEVICE_2_0_DTPM:
      DEBUG ((EFI_D_ERROR, "TPM 2.0\n")); break;

    case TPM_DEVICE_1_2:
      DEBUG ((EFI_D_ERROR, "TPM 1.2\n")); break;

    case TPM_DEVICE_NULL:
    default:
      DEBUG ((EFI_D_ERROR, "None\n")); break;
  }

// APTIOV_SERVER_OVERRIDE_RC_START : To use AMI's setup data to get the selected device
  DEBUG ((EFI_D_ERROR, "               Configured TPM Device (0x%x): ", Opd->SetupData.DeviceType));
  switch (Opd->SetupData.DeviceType) {
    case 2:
      DEBUG ((EFI_D_ERROR, "Auto : TPM 2.0 devices if not found, TPM 1.2 devices will be enumerated\n")); break;
    case 1:
      DEBUG ((EFI_D_ERROR, "TPM 2.0\n")); break;

    case 0:
      DEBUG ((EFI_D_ERROR, "TPM 1.2\n")); break;

    default:
      DEBUG ((EFI_D_ERROR, "None\n")); break;
  }
// APTIOV_SERVER_OVERRIDE_RC_END : To use AMI's setup data to get the selected device

//  DEBUG ((EFI_D_ERROR, "                            TCG TPM Device: "));
//  if (Opd->SystemConfig->TpmEnable != 0) { DEBUG ((EFI_D_ERROR, "Enable\n")); }
//  else { DEBUG ((EFI_D_ERROR, "Disable\n")); }

//  DEBUG ((EFI_D_ERROR, "                      TCG TPM Device State: "));
//  if (Opd->SystemConfig->TpmState != 0) { DEBUG ((EFI_D_ERROR, "Activate\n")); }
//  else { DEBUG ((EFI_D_ERROR, "Deactivate\n")); }

  DEBUG ((EFI_D_ERROR, "                                PTT (fTPM): "));
  if (Opd->Merc.MePttEnabled == 0) { DEBUG ((EFI_D_ERROR, "Disabled\n")); }
  else { DEBUG ((EFI_D_ERROR, "Enabled\n")); }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_DisplayBiosSetupOption()  }\n"));
  }

//------------------------------------------------------------------------------------------------------
/**
  OT_GetTxtInfo(): Return TXT support and enabled information.

  @param  Opd		INPUT:	OT_PLATFORM_DATA*, Ptr to OT_PLATFORM_DATA
  @param  Support	INPUT:	UINT16*, Ptr to FeatureSupported
			OUTPUT:	UINT16*, Ptr to FeatureSupported filled with proper
					 bit-mapped info of TXT
  @param  Enabled	INPUT:	UINT16*, Ptr to FeatureEnabled
			OUTPUT:	UINT16*, Ptr to FeatureEnabled filled with proper
					 bit-mapped info of TXT
**/
VOID
OT_GetTxtInfo (
  IN  OT_PLATFORM_DATA	*Opd,
  IN  UINT16		*Support,
  IN  UINT16		*Enabled
  )
{
  // Local data
  UINT16	te, ts;						// TXT enabled, TXT support information
  UINT64	q;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_GetTxtInfo()  { -> TXT Support and Enabled Information\n"));
  ts = 0; te = 0;						// Init TXT support info, Init TXT enabled info

  // TXT Support information: Read LT register EXISTS at Physical Address FED30010h
  q = (UINT64)(UINTN) *((UINTN*)(UINTN)0x00000000FED30010);			// Bit-mapped info of LT capable threads that registered at reset
  if (q != 0) { ts = OXP_TXT_SUPPORT_BIT; }			// TXT supported

  // TXT Enabled information: ts = bit-mapped TXT support information
  if (((ts & OXP_TXT_SUPPORT_BIT) != 0) &&			// If TXT supported AND
      (Opd->Spcc.ProcessorLtsxEnable == 1)) {			//    TXT enabled
    te = OXP_TXT_ENABLED_BIT;					//   Indicate TXT Enabled
}

  // Display TXT support information: ts, te = bit-mapped TXT support, enabled information
  DEBUG ((EFI_D_ERROR, "      [LT EXISTS register at 0xFED30010]: 0x%08x%08x\n", (UINT32)(UINTN) (q >> 32), (UINT32)(UINTN) q));
  DEBUG ((EFI_D_ERROR, "      TXT Support Information Bitmap 0x%x -> TXT ", ts));
  if ((ts & OXP_TXT_SUPPORT_BIT) != 0) { DEBUG ((EFI_D_ERROR, "Supported\n")); }
  else { DEBUG ((EFI_D_ERROR, "Not Supported\n")); }

  // Display TXT enabled information: ts, te = bit-mapped TXT support, enabled information
  DEBUG ((EFI_D_ERROR, "      TXT Enabled Information Bitmap 0x%x -> TXT ", te));
  if ((te & OXP_TXT_ENABLED_BIT) != 0) { DEBUG ((EFI_D_ERROR, "Enabled\n")); }
  else { DEBUG ((EFI_D_ERROR, "Not Enabled\n")); }

  // ts, te = bit-mapped TXT support, enabled information
  *Support |= ts; *Enabled |= te;				// Return Support and Enabled info

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_GetTxtInfo()  }\n"));
}

//------------------------------------------------------------------------------------------------------
/**
  OT_GetTpmInfo(): Return TPM support and enabled information.

  @param  Opd		INPUT:  OT_PLATFORM_DATA*, Ptr to OT_PLATFORM_DATA
  @param  Support	INPUT:	UINT16*, Ptr to FeatureSupported
			OUTPUT:	UINT16*, Ptr to FeatureSupported filled with proper
					 bit-mapped info of TPM
  @param  Enabled	INPUT:	UINT16*, Ptr to FeatureEnabled
			OUTPUT:	UINT16*, Ptr to FeatureEnabled filled with proper
					 bit-mapped info of TPM
**/
VOID
OT_GetTpmInfo (
  IN  OT_PLATFORM_DATA	*Opd,
  IN  UINT16		*Support,
  IN  UINT16		*Enabled
  )
{
  // Local data
  UINT64	q;
  UINT32	d;
  UINT16	te, ts;						// TPM enabled, support info
  BOOLEAN	pe, ps;						// PTT enabled, support info

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_GetTpmInfo()  { -> TPM Support and Enabled Information\n"));

  //--------------------------------------------------------------
  // Find PTT (fTPM) support and enabled information from ME
  DEBUG ((EFI_D_ERROR, "      Find PTT (fTPM) information from ME: PTT -> "));
  ps = FALSE;							// Indicate PTT not supported
  if ((PttHeciGetCapability (&ps)) != EFI_SUCCESS) { ps = FALSE; }// Find if PTT supported
  if (ps) { DEBUG ((EFI_D_ERROR, "Supported, ")); }
  else { DEBUG ((EFI_D_ERROR, "Not Supported, ")); }

  pe = FALSE;							// Indicate PTT not enabled
  if ((PttHeciGetState (&pe)) != EFI_SUCCESS) { pe = FALSE; }	//   Find if PTT enabled in ME
  if (pe) {							//   If PTT enabled in ME
    DEBUG ((EFI_D_ERROR, "Enabled\n"));
    if (Opd->Merc.MePttEnabled == 0) {				//     If PTT disabled in Setup
      DEBUG ((EFI_D_ERROR, "      Ensure PTT enabled in ME Configuration Setup\n"));
      Opd->Merc.MePttEnabled = 1; Opd->Mercm = TRUE;		//     Ensure PTT is enabled in Setup, Data modified
    }
  } else {
    DEBUG ((EFI_D_ERROR, "Not Enabled\n"));
  }

  //--------------------------------------------------------------
  // TPM support information: ps = TRUE/FALSE PTT yes/not supported, pe = TRUE/FALSE PTT yes/not enabled
  DEBUG ((EFI_D_ERROR, "      Find TPM Support information: Detected TPM Device 0x%01x\n", Opd->T2dd.TpmDeviceDetected));
  ts = 0;							// Init TPM support info

  // Read LT EMC Version Number register at Physical Address FED30200h
  d = (UINT32)(UINTN) *((UINTN*)(UINTN)0x00000000FED30200);			// [LT EMC Version Number register]
  DEBUG ((EFI_D_ERROR, "        [LT EMC Version Number register at 0xFED30200]: 0x%08x", d));
  DEBUG ((EFI_D_ERROR, ", Bit28:27 = 0x%02x, Bit24:22 = 0x%02x\n", ((UINT8)(UINTN) ((d >> 27) & 0x03)), ((UINT8)(UINTN) ((d >> 22) & 0x07))));
  // Read TPM I/F Version Number register at Physical Address FED30800h
  q = (UINT64)(UINTN) *((UINTN*)(UINTN)0x00000000FED30800);			// [TPM I/F Version Number register]
  DEBUG ((EFI_D_ERROR, "        [TPM I/F Version Number register at 0xFED30800]: 0x%08x%08x", (UINT32)(UINTN) (q >> 32), (UINT32)(UINTN) q));
  DEBUG ((EFI_D_ERROR, ", Bit18:16 = 0x%02x\n", ((UINT8)(UINTN) ((q >> 16) & 0x07))));

  if (Opd->T2dd.TpmDeviceDetected != TPM_DEVICE_NULL) { ts |= OXP_DTPM_SUPPORT_BIT; }// If any TPM device detected, indicate dTPM supported
  if (ps) { ts |= OXP_FTPM_SUPPORT_BIT; }			// If PTT supported, indicate fTPM supported

  // Display TPM support information
  DEBUG ((EFI_D_ERROR, "        TPM Support Information Bitmap 0x%x -> ", ts));
  if ((ts & OXP_DTPM_SUPPORT_BIT) != 0) { DEBUG ((EFI_D_ERROR, "dTPM Supported, ")); }
  else { DEBUG ((EFI_D_ERROR, "dTPM Not Supported, ")); }
  if ((ts & OXP_FTPM_SUPPORT_BIT) != 0) { DEBUG ((EFI_D_ERROR, "fTPM Supported\n")); }
  else { DEBUG ((EFI_D_ERROR, "fTPM Not Supported\n")); }

  //--------------------------------------------------------------
  // TPM enabled information: ps = TRUE/FALSE PTT yes/not supported, pe = TRUE/FALSE PTT yes/not enabled
  // ts = bit-mapped TPM support information
  DEBUG ((EFI_D_ERROR, "      Find TPM Enabled information: Detected TPM Device 0x%01x\n", Opd->T2dd.TpmDeviceDetected));
  te = 0;							// Init TPM enabled info
  if ((ts & OXP_TPM_SUPPORT_BIT_MASK) != 0) {			// If TPM supported
    switch (Opd->T2dd.TpmDeviceDetected) {
      case TPM_DEVICE_2_0_DTPM:					// TPM2.0
	DEBUG ((EFI_D_ERROR, "        TPM2.0: "));
        if ((ps) && (pe)) {					// If fTPM supported AND PTT enabled
	  te = OXP_FTPM_ENABLED_BIT;				//   Indicate fTPM enabled
	  DEBUG ((EFI_D_ERROR, "fTPM Enabled\n"));
        } else {
	  te = OXP_DTPM_ENABLED_BIT;				// Indicate dTPM enabled
	  DEBUG ((EFI_D_ERROR, "dTPM Enabled\n"));
}
	break;

      case TPM_DEVICE_1_2:					// TPM1.2
	DEBUG ((EFI_D_ERROR, "        TPM1.2: Get TPM Permanent Flags\n"));
	if ((OT_GetTpm12PermanentFlags (Opd)) == TPM_SUCCESS) {
	  DEBUG ((EFI_D_ERROR, "        dTPM: "));
	  if (Opd->Tpm12PermanentFlags.disable) {		// If disabled
	    DEBUG ((EFI_D_ERROR, "Disabled/"));
	  } else {						// If enabled
	    te = OXP_DTPM_ENABLED_BIT;				//   Indicate dTPM enabled
	    DEBUG ((EFI_D_ERROR, "Enabled/"));
	  }
	  if (Opd->Tpm12PermanentFlags.deactivated) {		// If deactivated
	    DEBUG ((EFI_D_ERROR, "Deactivated, "));
	  } else {						// If activated
	    DEBUG ((EFI_D_ERROR, "Activated, "));
	  }
	  DEBUG ((EFI_D_ERROR, "Life Time Lock: "));
	  if (Opd->Tpm12PermanentFlags.physicalPresenceLifetimeLock) {	// If Life Time Locked

	    DEBUG ((EFI_D_ERROR, "Set, "));
	  } else {
	    DEBUG ((EFI_D_ERROR, "Not set, "));
	  }
	  DEBUG ((EFI_D_ERROR, "Physical Presence HW: "));
	  if (Opd->Tpm12PermanentFlags.physicalPresenceHWEnable) {	// If Physical Presence HW Enabled
	    DEBUG ((EFI_D_ERROR, "Enabled, "));
	  } else {
	    DEBUG ((EFI_D_ERROR, "Not-Enabled, "));
	  }
	  DEBUG ((EFI_D_ERROR, "Physical Presence Command: "));
	  if (Opd->Tpm12PermanentFlags.physicalPresenceCMDEnable) {	// If Physical Presence Command Enabled
	    DEBUG ((EFI_D_ERROR, "Enabled\n"));
	  } else {
	    DEBUG ((EFI_D_ERROR, "Not-Enabled\n"));
	  }
        }
	break;

      //case TPM_DEVICE_NULL:					// No TPM
      default:							// Invalid
	DEBUG ((EFI_D_ERROR, "        No/Invalid TPM device\n"));
	break;
    }
  }

  // Display TPM enabled information
  DEBUG ((EFI_D_ERROR, "      TPM Enabled Information Bitmap 0x%x -> ", te));
  if ((te & OXP_DTPM_ENABLED_BIT) != 0) { DEBUG ((EFI_D_ERROR, "dTPM Enabled, ")); }
  else { DEBUG ((EFI_D_ERROR, "dTPM Not Enabled, ")); }
  if ((te & OXP_FTPM_ENABLED_BIT) != 0) { DEBUG ((EFI_D_ERROR, "fTPM Enabled\n")); }
  else { DEBUG ((EFI_D_ERROR, "fTPM Not Enabled\n")); }

  //--------------------------------------------------------------
  // ts, te = bit-mapped TPM support, enabled information
  *Support |= ts; *Enabled |= te;				// Return Support and Enabled info

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_GetTpmInfo()  }\n"));
}

//-----------------------------	Password Functions { ---------------------------------------------------

//-------------------------------------------------------------------------------------------------------
/**
  Returns the information of different installed passwords.

  @param[in]	Data		SYSTEM_CONFIGURATION*: Ptr to System configuration

  @retval	p	UINT8: Bit-mapped information of installed passwords
				OT_NO_PASSWORD	No password installed
				OT_ADMIN_PASSWORD	Administrative password installed
				OT_USER_PASSWORD	User password installed
**/
UINT8
OT_CheckIfPasswordInstalled (
//APTIOV_SERVER_OVERRIDE_RC_START : To use AMI's setup password
#if 0
  IN  SYSTEM_CONFIGURATION	*Data
#else
    IN  AMITSESETUP           *Data
#endif
//APTIOV_SERVER_OVERRIDE_RC_END : To use AMI's setup password
  )
{
  // Local data
  //APTIOV_SERVER_OVERRIDE_RC_START : To use AMI's setup password
  UINT8		p, a[PASSWORD_MAX_SIZE * 2];
  //APTIOV_SERVER_OVERRIDE_RC_END : To use AMI's setup password

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_CheckIfPasswordInstalled()  { -> Check if Password installed in BIOS\n"));
  p = OT_NO_PASSWORD;					// No password installed
  ZeroMem (&a, sizeof (a));				// Init with 0

  if (Data != NULL) {					// If Setup Data valid
    if ((CompareMem (Data->AdminPassword, a, sizeof (a))) != 0) {
      p |= OT_ADMIN_PASSWORD;				//   Admin password installed
    }
    if ((CompareMem (Data->UserPassword, a, sizeof (a))) != 0) {
      p |= OT_USER_PASSWORD;				//   User password installed
      }
    }
  DEBUG ((EFI_D_ERROR, "      Installed Password Bitmap 0x%x\n", p));

  if (p == OT_NO_PASSWORD) {				// If No password installed
    DEBUG ((EFI_D_ERROR, "  No Password Installed\n"));
  } else {						// If some password installed
    if ((p & OT_ADMIN_PASSWORD) != 0) {			//   If Admin password installed
      DEBUG ((EFI_D_ERROR, "  Administrative Password Installed\n"));
    }
    if ((p & OT_USER_PASSWORD) != 0) {			//   If User password installed
      DEBUG ((EFI_D_ERROR, "  User Password Installed\n"));
    }
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_CheckIfPasswordInstalled()  }\n"));

  return p;						// Return password installed information
}

//-------------------------------------------------------------------------------------------------------
/**
  Match the given UnicodeZ password with the BIOS Administrative Password.

  @param[in]	*Scd		SYSTEM_CONFIGURATION*: Ptr to SYSTEM_CONFIGURATION
  @param[in]	*Password	CHAR16*: Ptr to given UnicodeZ password

  @retval	BOOLEAN		TRUE/FALSE: Success/Error

**/
BOOLEAN
OT_MatchAdminPassword (
// APTIOV_SERVER_OVERRIDE_RC_START: To use AMI's setup Password instead Intel's
#if 0
  IN  SYSTEM_CONFIGURATION	*Scd,
#else
  IN  AMITSESETUP   *AmiTseData,
#endif
// APTIOV_SERVER_OVERRIDE_RC_END: To use AMI's setup Password instead Intel's
  IN  CHAR16	    *Password
  )
{
// APTIOV_SERVER_OVERRIDE_RC_START : Encode using AMI TSE' library PasswordEncode
#if 0
  UINT8		ep[SHA256_DIGEST_LENGTH];			// Encoded password buffer
  UINTN		eps = SHA256_DIGEST_LENGTH;			// Encoded password size
  BOOLEAN	b = FALSE;					// Assume Error

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_MatchAdminPassword()  {\n"));
  DEBUG ((EFI_D_ERROR, "      Encode Supplied Password -> "));
  if ((EncodePassword(Password, &ep[0], &eps)) == EFI_SUCCESS) {// If Encode given password success
    DEBUG ((EFI_D_ERROR, "Success\n"));
  DEBUG ((EFI_D_ERROR, "  Encoded Supplied Password: Size 0x%08x bytes\n", eps));
  DisplayBinaryData ((UINT8*)(UINTN) &ep[0], eps);		// Display Encoded password
    if (eps == sizeof (Scd->AdminPassword)) {			//   If encoded password size match
  DEBUG ((EFI_D_ERROR, "  Encoded BIOS Administrative Password: Size 0x%08x bytes\n", sizeof (Scd->AdminPassword)));
  DisplayBinaryData ((UINT8*)(UINTN) &Scd->AdminPassword, sizeof (Scd->AdminPassword));	// Display encoded BIOS Password
  if (CompareMem ((UINT8*)(UINTN) &ep[0],			// If Admin Password match
	(UINT8*)(UINTN) &Scd->AdminPassword, eps) == 0) {
	b = TRUE;							// Success, match
  }
    } else {
      DEBUG ((EFI_D_ERROR, "      Mismatch in size of Encoded Supplied Password and Encode BIOS Administrative Password\n"));
    }
  } else {
    DEBUG ((EFI_D_ERROR, "Error\n"));
  }

  DEBUG ((EFI_D_ERROR, "      Clear Encoded Password Buffer\n"));
  ZeroMem (&ep[0], eps);					// Clear encoded password buffer
#else
  BOOLEAN	b = FALSE;					// Assume Error

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_MatchAdminPassword()  {\n"));
  PasswordEncode(Password, PASSWORD_MAX_SIZE * 2);
  
  DEBUG ((EFI_D_ERROR, "      Encoded Supplied Password -> Success"));
  DisplayBinaryData ((UINT8*)(UINTN) Password, PASSWORD_MAX_SIZE * 2);        // Display Encoded password
  DEBUG ((EFI_D_ERROR, " Encoded BIOS Administrative Password: Size 0x%08x bytes\n", sizeof (AmiTseData->AdminPassword)));
  DisplayBinaryData ((UINT8*)(UINTN) AmiTseData->AdminPassword, sizeof (AmiTseData->AdminPassword));     // Display BIOS Password

  if (CompareMem ( Password, AmiTseData->AdminPassword, sizeof (AmiTseData->AdminPassword)) == 0) {      // If Admin Password match
      b = TRUE;           // Success, match
  } else {
      DEBUG ((EFI_D_ERROR, "Mismatch between Supplied Password and BIOS Administrative Password\n"));
  }
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Encode using AMI TSE' library PasswordEncode

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_MatchAdminPassword()  } -> Password "));
  if (b) { DEBUG ((EFI_D_ERROR, "Match, Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "not match, Error\n")); }

  return b;							// Result
}

//-----------------------------	} Password Functions ---------------------------------------------------
//------------------------------------------------------------------------------------------------------
//----------------------------- Platform Information { -------------------------------------------------
/**
  OT_AllocatePlatformInformationArea(): Allocate platform information area.

  @param  None		INPUT: None
  @param  Opd		OUTPUT: Ptr to OT_PLATFORM_DATA, Success
				NULL, Error
**/
OT_PLATFORM_DATA*
OT_AllocatePlatformInformationArea (
  )
{
  // Local data
  EFI_STATUS		Status;
  OT_PLATFORM_DATA	*Opd;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_AllocatePlatformInformationArea()  { -> Allocate Memory for Platform Information\n"));

  // Init local data
  Opd = NULL; mSystemConfigurationSize = 0;

  // Allocate memory
  DEBUG ((EFI_D_ERROR, "      OT_PLATFORM_DATA: Allocate memory 0x%x bytes\n", (sizeof (OT_PLATFORM_DATA))));
  Opd = (OT_PLATFORM_DATA*)(UINTN) AllocateZeroPool (sizeof (OT_PLATFORM_DATA));// Allocate memory
  if (Opd != NULL) {								// If Memory available
    DEBUG ((EFI_D_ERROR, "      SYSTEM_CONFIGURATION: Find required memory size\n"));
    Status = gRT->GetVariable (PLATFORM_SETUP_VARIABLE_NAME,			//   Find Setup data size
		&mSystemConfigurationGuid, NULL, &mSystemConfigurationSize, Opd);	//	--> should return error EFI_BUFFER_TOO_SMALL
    if (Status == EFI_BUFFER_TOO_SMALL) {					//   If EFI_BUFFER_TOO_SMALL
      DEBUG ((EFI_D_ERROR, "      SYSTEM_CONFIGURATION: Allocate memory 0x%x bytes\n", mSystemConfigurationSize));
      Opd->SystemConfig = (SYSTEM_CONFIGURATION*)(UINTN)				//     Allocate memory for System configuration, fill ptr to SYSTEM_CONFIGURATION in OT_PLATFORM_DATA
				AllocateZeroPool (mSystemConfigurationSize);
      if (Opd->SystemConfig != NULL) {							//     If Success
	DEBUG ((EFI_D_ERROR, "      OT_PLATFORM_DATA allocated at memory address 0x%x\n", (UINTN) Opd));
	DEBUG ((EFI_D_ERROR, "      SYSTEM_CONFIGURATION allocated at memory address 0x%x\n", (UINTN) Opd->SystemConfig));
	DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_AllocatePlatformInformationArea()  } -> Success\n"));
	return Opd;									//       Ptr to OT_PLATFORM_DATA
      }
    }
  }

  // Some error
  if (Opd != NULL) { gBS->FreePool ((VOID*)(UINTN) Opd); }				// Deallocate memory
  DEBUG ((EFI_D_ERROR, "      Platform information data area not allocated\n"));
  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_AllocatePlatformInformationArea()  } -> Error\n"));
  return NULL;									// Error
}

//------------------------------------------------------------------------------------------------------
/**
  OT_ClearAndDeallocatePlatformInformationArea(): Clear and Deallocate platform information area.

  @param  Opd		INPUT: Ptr to OT_PLATFORM_DATA
  @param  None		OUTPUT

  Note:	Before deallocating, clear platform information area to prevent the data from being hacked.

**/
VOID
OT_ClearAndDeallocatePlatformInformationArea (
  IN  OT_PLATFORM_DATA	*Opd
  )
{
  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_ClearAndDeallocatePlatformInformationArea()  {\n"));
  if (Opd != NULL) {
    if (Opd->SystemConfig != NULL) {
      DEBUG ((EFI_D_ERROR, "         Clear System Configuration Area\n"));
      ZeroMem ((UINT8*)(UINTN) Opd->SystemConfig, sizeof (SYSTEM_CONFIGURATION));
      DEBUG ((EFI_D_ERROR, "         Deallocate System Configuration Area\n"));
      gBS->FreePool ((VOID*)(UINTN) Opd->SystemConfig);				// Deallocate memory
    }
    DEBUG ((EFI_D_ERROR, "         Clear OT_PLATFORM_DATA Area\n"));
    ZeroMem ((UINT8*)(UINTN) Opd, sizeof (OT_PLATFORM_DATA));
    DEBUG ((EFI_D_ERROR, "         Deallocate OT_PLATFORM_DATA area\n"));
    gBS->FreePool ((VOID*)(UINTN) Opd);						// Deallocate memory
  }
  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_ClearAndDeallocatePlatformInformationArea()  }\n"));
}

//------------------------------------------------------------------------------------------------------
/**
  OT_ReadPlatformInformation(): Read platform information.

  @param  Opd		INPUT/OUTPUT: Ptr to OT_PLATFORM_DATA where platform information to be read
  @param  Result	OUTPUT: BOOLEAN, TRUE/FALSE: Success/Error

**/
BOOLEAN
OT_ReadPlatformInformation (
  IN OUT OT_PLATFORM_DATA	*Opd
  )
{
  // Local data
  EFI_STATUS	Status;
  UINTN		Size;
  UINT16	pa, fs, fe;
  BOOLEAN	b;
  //APTIOV_SERVER_OVERRIED_RC_START : To use AMI's setup data
  EFI_GUID    AmiTseSetupGuid = AMITSESETUP_GUID;
  //APTIOV_SERVER_OVERRIED_RC_END : To use AMI's setup data

  // Init local data
  b = TRUE;							// Assume Success
  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_ReadPlatformInformation()  { -> Read Platform Information\n"));

  // Read Socket Processor Core Configuration data
  Size = sizeof (SOCKET_PROCESSORCORE_CONFIGURATION);		// Size of SOCKET_PROCESSORCORE_CONFIGURATION
  DEBUG ((EFI_D_ERROR, "      Read BIOS Setup Options: %s, 0x%x bytes --> ", SOCKET_PROCESSORCORE_CONFIGURATION_NAME, Size));
  Status = gRT->GetVariable (SOCKET_PROCESSORCORE_CONFIGURATION_NAME, &gEfiSocketProcessorCoreVarGuid,
                (UINT32*)(UINTN) &Opd->Spcca, &Size, (UINT32*)(UINTN) &Opd->Spcc);
  if (Status == EFI_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { b = FALSE; DEBUG ((EFI_D_ERROR, "Error\n")); }		//   Error

  Size = sizeof (SOCKET_IIO_CONFIGURATION);			// Size of SOCKET_IIO_CONFIGURATION
  DEBUG ((EFI_D_ERROR, "      Read BIOS Setup Options: %s, 0x%x bytes --> ", SOCKET_IIO_CONFIGURATION_NAME, Size));
  Status = gRT->GetVariable (SOCKET_IIO_CONFIGURATION_NAME, &gEfiSocketIioVariableGuid,
                (UINT32*)(UINTN) &Opd->Sica, &Size, (UINT32*)(UINTN) &Opd->Sic);
  if (Status == EFI_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { b = FALSE; DEBUG ((EFI_D_ERROR, "Error\n")); }		//   Error

// APTIOV_SERVER_OVERRIDE_RC_START : Use AMI setup to get TCG's configuration
#if 0
  Size = sizeof(TCG2_CONFIGURATION);
  DEBUG ((EFI_D_ERROR, "      Read BIOS Setup Options: %s, 0x%x bytes --> ", TCG2_STORAGE_NAME, Size));
  Status = gRT->GetVariable (TCG2_STORAGE_NAME, &gTcg2ConfigFormSetGuid,
                  (UINT32*)(UINTN) &Opd->T2ca, &Size, (UINT32*)(UINTN) &Opd->T2c);
#else
  Size = sizeof(SETUP_DATA);
  DEBUG ((EFI_D_ERROR, "      Read BIOS Setup Options: %s, 0x%x bytes --> ", L"Setup", Size));
  Status = gRT->GetVariable (L"Setup", &gSetupGuid,
                  (UINT32*)(UINTN) &Opd->Sda, &Size, (UINT32*)(UINTN) &Opd->SetupData);
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Use AMI setup to get TCG's configuration
  if (Status == EFI_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { b = FALSE; DEBUG ((EFI_D_ERROR, "Error\n")); }		//   Error

  Size = sizeof (ME_RC_CONFIGURATION);
  DEBUG ((EFI_D_ERROR, "      Read BIOS Setup Options: %s, 0x%x bytes --> ", ME_RC_CONFIGURATION_NAME, Size));
  Status = gRT->GetVariable (ME_RC_CONFIGURATION_NAME, &gEfiMeRcVariableGuid,
                  (UINT32*)(UINTN) &Opd->Merca, &Size, (UINT32*)(UINTN) &Opd->Merc);
  if (Status == EFI_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { b = FALSE; DEBUG ((EFI_D_ERROR, "Error\n")); }		//   Error

  DEBUG ((EFI_D_ERROR, "      Read BIOS Setup Options: %s, 0x%x bytes --> ", PLATFORM_SETUP_VARIABLE_NAME, mSystemConfigurationSize));
  Status = gRT->GetVariable (PLATFORM_SETUP_VARIABLE_NAME, &mSystemConfigurationGuid,
                  (UINT32*)(UINTN) &Opd->Sca, &mSystemConfigurationSize, (VOID*)(UINTN) Opd->SystemConfig);
  if (Status == EFI_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { b = FALSE; DEBUG ((EFI_D_ERROR, "Error\n")); }		//   Error
  
  //APTIOV_SERVER_OVERRIED_RC_START: To use AMI's setup password
  Size = sizeof (AMITSESETUP);
  DEBUG ((EFI_D_ERROR, "      Read BIOS Setup Options: %s, 0x%x bytes --> ", L"AMITSESetup", Size));
  Status = gRT->GetVariable ( L"AMITSESetup", &AmiTseSetupGuid, NULL, &Size, &Opd->AmiTseData );
  if (Status == EFI_SUCCESS) { 
  DEBUG ((EFI_D_ERROR, "Success\n")); 
  } else {
    b = FALSE;
    DEBUG ((EFI_D_ERROR, "Error\n"));
  }     //   Error
  //APTIOV_SERVER_OVERRIED_RC_END: To use AMI's setup password


  //--------------------------------------------------------------
  // The following code does NOT work when TPM is physically removed after BIOS is booted successfully
  // with TPM (because this detection variable is not initialized in the error path and is left with
  // the value found in the previous boot
  //Size = sizeof(TCG2_DEVICE_DETECTION);
  //DEBUG ((EFI_D_ERROR, "      Read BIOS Setup Options: %s, 0x%x bytes --> ", TCG2_DEVICE_DETECTION_NAME, Size));
  //Status = gRT->GetVariable (TCG2_DEVICE_DETECTION_NAME, &gTcg2ConfigFormSetGuid,
  //                (UINT32*)(UINTN) &Opd->T2dda, &Size, (UINT32*)(UINTN) &Opd->T2dd);
  //if (Status == EFI_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  //else { b = FALSE; DEBUG ((EFI_D_ERROR, "Error\n")); }		//   Error

  DEBUG ((EFI_D_ERROR, "         TPM Device Present: "));
  Opd->T2dd.TpmDeviceDetected = TPM_DEVICE_NULL;							// No TPM present
  Opd->Tcg2Protocol = NULL; Opd->TcgProtocol = NULL;							// TCG2 protocol and TCG protocol not found
  if (((gBS->LocateProtocol (&gEfiTcg2ProtocolGuid, NULL, (VOID**) &Opd->Tcg2Protocol)) == EFI_SUCCESS) &&// If TCG2 protocol found
      (Opd->Tcg2Protocol != NULL)) {
  Opd->T2dd.TpmDeviceDetected = TPM_DEVICE_2_0_DTPM;		// TPM2.0 present
  } else {												// If TCG2 protocol not found
    Opd->Tcg2Protocol = NULL;										//   TCG2 protocol not found
    if (((gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, (VOID**) &Opd->TcgProtocol)) == EFI_SUCCESS) &&//   If TCG protocol found
	(Opd->TcgProtocol != NULL)) {
    Opd->T2dd.TpmDeviceDetected = TPM_DEVICE_1_2;		//   TPM1.2 present
    } else {												//   If TCG protocol not found
      Opd->TcgProtocol = NULL;										//     TCG protocol not found
    }
  }
  switch (Opd->T2dd.TpmDeviceDetected) {			// Display Detected TPM device
    case TPM_DEVICE_2_0_DTPM:					// TPM 2.0
      DEBUG ((EFI_D_ERROR, "TPM 2.0\n"));
      break;

    case TPM_DEVICE_1_2:					// TPM 1.2
      DEBUG ((EFI_D_ERROR, "TPM 1.2\n"));
      break;

    //case TPM_DEVICE_NULL:					// No TPM
    default:							// Invalid
      DEBUG ((EFI_D_ERROR, "None or Invalid TPM Device\n"));
      break;
  }

  //--------------------------------------------------------------
  // Init different fields in OT_PLATFORM_DATA
  Opd->Tpm12PreparedForTask = FALSE;				// Indicate TPM preparation not performed before task
  Opd->Tpm12ActivateStateChange = FALSE;			// Indicate TPM activate state not changed
  Opd->Scm = FALSE; Opd->Spccm = FALSE; Opd->Sicm = FALSE;	// Indicate data not modified
  Opd->T2cm = FALSE; Opd->Mercm = FALSE;

  pa = 0; fs = 0; fe = 0;					// Init PasswordAttribute, FeatureSupported and FeatureEnabled
  if (b) {							// If read platform info success
    OT_DisplayBiosSetupOption (Opd, FALSE);			//   Display current Setup Option
    OT_GetTxtInfo (Opd, &fs, &fe);				//   Fill TXT support and enabled info
    OT_GetTpmInfo (Opd, &fs, &fe);				//   Fill TPM support and enabled info
    pa = (OXP_PASSWORD_ASCII_BIT | OXP_PASSWORD_UNICODE_BIT);	//   Default PasswordAttribute
// APTIOV_SERVER_OVERRIDE_RC_START: To use AMI's setup password
    if (((OT_CheckIfPasswordInstalled (&Opd->AmiTseData))	// If Administrative Password installed
// APTIOV_SERVER_OVERRIDE_RC_END: To use AMI's setup password
				& OT_ADMIN_PASSWORD) != 0) {
      pa |= OXP_PASSWORD_ADMIN_INSTALLED_BIT;			//   Update Password Attribute
    }
  }
  Opd->PasswordAttribute = pa;					// Password Attribute
  Opd->FeatureSupported = fs; Opd->FeatureEnabled = fe;		// Features supported, Features enabled
  //--------------------------------------------------------------

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_ReadPlatformInformation()  } -> "));
  if (b) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else {DEBUG ((EFI_D_ERROR, "Error\n")); }

  return b;							// Result
}

//-----------------------------------------------------------------------------------------------------
/**
  OT_WritePlatformInformation(): Write platform information, if necessary.

  @param  Opd		INPUT: Ptr to OT_PLATFORM_DATA having platform information
  @param  Result	OUTPUT: BOOLEAN, TRUE/FALSE: Success/Error

**/
BOOLEAN
OT_WritePlatformInformation (
  IN OUT OT_PLATFORM_DATA	*Opd
  )
{
  // Local data
  EFI_STATUS	Status;
  BOOLEAN	b;
// APTIOV_SERVER_OVERRIDE_RC_START : Use AMI setup to get TCG's configuration
  EFI_GUID                          gSetupGuid = SETUP_GUID;
// APTIOV_SERVER_OVERRIDE_RC_END : Use AMI setup to get TCG's configuration

  // Init local data
  b = TRUE;							// Assume Success
  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_WritePlatformInformation()  { -> Write Platform Information\n"));
  OT_DisplayBiosSetupOption (Opd, TRUE);			// Display modified Setup Option

  // Write Setup Option, if necessary
  DEBUG ((EFI_D_ERROR, "      Write BIOS Setup Options: %s --> ", SOCKET_PROCESSORCORE_CONFIGURATION_NAME));
  if (Opd->Spccm) {								// If modified
    Status = gRT->SetVariable (SOCKET_PROCESSORCORE_CONFIGURATION_NAME, &gEfiSocketProcessorCoreVarGuid,
                  Opd->Spcca, sizeof (SOCKET_PROCESSORCORE_CONFIGURATION), (UINT32*)(UINTN) &Opd->Spcc);
    if (Status == EFI_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
    else { b = FALSE; DEBUG ((EFI_D_ERROR, "Error\n")); }	//   Error
  } else {									// If not changed
    DEBUG ((EFI_D_ERROR, "Not modified, No need to write\n"));
  }

  DEBUG ((EFI_D_ERROR, "      Write BIOS Setup Options: %s --> ", SOCKET_IIO_CONFIGURATION_NAME));
  if (Opd->Sicm) {								// If modified
    Status = gRT->SetVariable (SOCKET_IIO_CONFIGURATION_NAME, &gEfiSocketIioVariableGuid,
                  Opd->Sica, sizeof (SOCKET_IIO_CONFIGURATION), (UINT32*)(UINTN) &Opd->Sic);
    if (Status == EFI_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
    else { b = FALSE; DEBUG ((EFI_D_ERROR, "Error\n")); }	//   Error
  } else {									// If not changed
    DEBUG ((EFI_D_ERROR, "Not modified, No need to write\n"));
  }

// APTIOV_SERVER_OVERRIDE_RC_START : Use AMI setup to get TCG's configuration
#if 0
  DEBUG ((EFI_D_ERROR, "      Write BIOS Setup Options: %s --> ", TCG2_STORAGE_NAME));
#else
  DEBUG ((EFI_D_ERROR, "      Write BIOS Setup Options: %s --> ", L"Setup"));
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Use AMI setup to get TCG's configuration
  if (Opd->T2cm) {								// If modified
// APTIOV_SERVER_OVERRIDE_RC_START : Use AMI setup to get TCG's configuration
#if 0
    Status = gRT->SetVariable (TCG2_STORAGE_NAME, &gTcg2ConfigFormSetGuid,
                  Opd->T2ca, sizeof (TCG2_CONFIGURATION), (UINT32*)(UINTN) &Opd->T2c);
#else
    Status = gRT->SetVariable (L"Setup", &gSetupGuid,
                  Opd->Sda, sizeof (SETUP_DATA), (UINT32*)(UINTN) &Opd->SetupData);
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Use AMI setup to get TCG's configuration
    if (Status == EFI_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
    else { b = FALSE; DEBUG ((EFI_D_ERROR, "Error\n")); }	//   Error
  } else {							// If not changed
    DEBUG ((EFI_D_ERROR, "Not modified, No need to write\n"));
  }

  DEBUG ((EFI_D_ERROR, "      Write BIOS Setup Options: %s --> ", ME_RC_CONFIGURATION_NAME));
  if (Opd->Mercm) {						// If modified
    Status = gRT->SetVariable (ME_RC_CONFIGURATION_NAME, &gEfiMeRcVariableGuid,
                  Opd->Merca, sizeof(ME_RC_CONFIGURATION), (UINT32*)(UINTN) &Opd->Merc);
    if (Status == EFI_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
    else { b = FALSE; DEBUG ((EFI_D_ERROR, "Error\n")); }	//   Error
  } else {							// If not changed
    DEBUG ((EFI_D_ERROR, "Not modified, No need to write\n"));
    }

  DEBUG ((EFI_D_ERROR, "      Write BIOS Setup Options: %s --> ", PLATFORM_SETUP_VARIABLE_NAME));
  if (Opd->Scm) {						// If modified
    Status = gRT->SetVariable (PLATFORM_SETUP_VARIABLE_NAME, &mSystemConfigurationGuid,
                  Opd->Sca, mSystemConfigurationSize, (VOID*)(UINTN) Opd->SystemConfig);
    if (Status == EFI_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
    else { b = FALSE; DEBUG ((EFI_D_ERROR, "Error\n")); }	//   Error
  } else {									// If not changed
    DEBUG ((EFI_D_ERROR, "Not modified, No need to write\n"));
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_WritePlatformInformation()  } -> "));
  if (b) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else {DEBUG ((EFI_D_ERROR, "Error\n")); }

  return b;									// Result
}

//------------------------------------------------------------------------------------------------------
// ****************************	dTPM functions *********************************************************
//------------------------------------------------------------------------------------------------------
/**
  OT_DTpmClear(): Clear dTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_DTPM_CLEAR_ERROR
**/
UINT8
OT_DTpmClear (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_DTpmClear() { -> Detected TPM Device 0x%01x\n", Opd->T2dd.TpmDeviceDetected));
  e = OXP_DTPM_CLEAR_ERROR;					// Assume Error
  switch (Opd->T2dd.TpmDeviceDetected) {					// Detected TPM device
    case TPM_DEVICE_2_0_DTPM:							// TPM 2.0
      DEBUG ((EFI_D_ERROR, "      TPM2.0: Clear --> "));
      if (Opd->Tcg2Protocol != NULL) {				// If TCG2 protocol found
        DEBUG ((EFI_D_ERROR, "Tpm2CommandClear(): "));
// APTIOV_SERVER_OVERRIDE_RC_START : TPM2 command clear using AMI's Tree protocol
	if (Tpm2CommandClear (Auth, Opd->Tcg2Protocol) == EFI_SUCCESS) {		// If success
// APTIOV_SERVER_OVERRIDE_RC_END: TPM2 command clear using AMI's Tree protocol
	  e = OXP_SUCCESS;					//   Indicate success
	  DEBUG ((EFI_D_ERROR, "Success\n"));
	} else {						// If not sucess
	  DEBUG ((EFI_D_ERROR, "Error\n"));
	}
      } else {							// If TCG2 protocol not found
        DEBUG ((EFI_D_ERROR, "Implementation Error -> BUG\n"));
      }
      break;

    case TPM_DEVICE_1_2:							// TPM 1.2
      DEBUG ((EFI_D_ERROR, "      TPM1.2: "));
      if (Opd->TcgProtocol != NULL) {				// If TCG protocol found
	DEBUG ((EFI_D_ERROR, "Clear, Enable, Activate -->\n"));
	e = OT_Tpm12ClearEnableActivate (Opd);			// Clear
      } else {							// If TCG protocol not found
        DEBUG ((EFI_D_ERROR, "Implementation Error -> BUG\n"));
      }
      break;

    //case TPM_DEVICE_NULL:					// No TPM
    default:							// Invalid
      DEBUG ((EFI_D_ERROR, "      None/Invalid TPM Device: Implementation Error -> BUG\n"));
      break;
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_DTpmClear() } -> Status 0x%01x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else {DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status code
}

//------------------------------------------------------------------------------------------------------
/**
  OT_DTpmEnable(): Enable dTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_DTPM_ENABLE_ERROR, OXP_DTPM_ACTIVATE_ERROR
**/
UINT8
OT_DTpmEnable (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_DTpmEnable()  { -> Detected TPM Device 0x%01x\n", Opd->T2dd.TpmDeviceDetected));
  e = OXP_DTPM_ENABLE_ERROR;					// Assume Error
  switch (Opd->T2dd.TpmDeviceDetected) {			// Detected TPM device
    case TPM_DEVICE_2_0_DTPM:					// TPM 2.0
      DEBUG ((EFI_D_ERROR, "      TPM2.0: Enable --> "));
      if (Opd->Tcg2Protocol != NULL) {				// If TCG2 protocol found
	e = OXP_SUCCESS;					//   Indicate success
	DEBUG ((EFI_D_ERROR, "Success\n"));
      } else {							// If TCG2 protocol not found
        DEBUG ((EFI_D_ERROR, "Implementation Error -> BUG\n"));
      }
      break;

    case TPM_DEVICE_1_2:					// TPM 1.2
      DEBUG ((EFI_D_ERROR, "      TPM1.2: "));
      if (Opd->TcgProtocol != NULL) {				// If TCG protocol found
	DEBUG ((EFI_D_ERROR, "Enable, Activate -->\n"));
	e = OT_Tpm12EnableActivate (Opd, FALSE);		// Enable, Activate (if necessary)
      } else {							// If TCG protocol not found
        DEBUG ((EFI_D_ERROR, "Implementation Error -> BUG\n"));
      }
      break;

    //case TPM_DEVICE_NULL:					// No TPM
    default:							// Invalid
      DEBUG ((EFI_D_ERROR, "      None/Invalid TPM Device: Error\n"));
      break;
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_DTpmEnable()  } -> Status 0x%01x -> ", e));
  if (e == OXP_SUCCESS) {					// If success
// APTIOV_SERVER_OVERRIDE_RC_START : Use AMI setup to configure Tpm
#if 0
    if (Opd->T2c.TpmDevice != Opd->T2dd.TpmDeviceDetected) {	//   If configured TPM <> detected TPM
      Opd->T2c.TpmDevice = Opd->T2dd.TpmDeviceDetected; Opd->T2cm = TRUE;// Set configured TPM = detected TPM, Data modified
#else
    if (Opd->SetupData.TpmEnable == TPM_DEVICE_NULL) {
        Opd->SetupData.TpmEnable = 1;
        Opd->T2cm = TRUE;
    }
    if ( (Opd->T2dd.TpmDeviceDetected != TPM_DEVICE_NULL) && (Opd->SetupData.DeviceType != (Opd->T2dd.TpmDeviceDetected - 1)) ) {
        Opd->SetupData.DeviceType = Opd->T2dd.TpmDeviceDetected - 1;
        Opd->T2cm = TRUE;  // Set configured TPM = detected TPM, Data modified
    }
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Use AMI setup to configure Tpm
    DEBUG ((EFI_D_ERROR, "Success\n"));
  } else {
    DEBUG ((EFI_D_ERROR, "Error\n"));
  }

  return e;							// Status
}

//------------------------------------------------------------------------------------------------------
/**
  OT_DTpmDisable(): Disable dTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_DTPM_DISABLE_ERROR
**/
UINT8
OT_DTpmDisable (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_DTpmDisable()  { -> Detected TPM Device 0x%01x\n", Opd->T2dd.TpmDeviceDetected));
  e = OXP_DTPM_DISABLE_ERROR;					// Assume Error
  switch (Opd->T2dd.TpmDeviceDetected) {					// Detected TPM device
    case TPM_DEVICE_2_0_DTPM:							// TPM 2.0
      DEBUG ((EFI_D_ERROR, "      TPM2.0: Disable --> "));
// APTIOV_SERVER_OVERRIDE_RC_START : AMI's TCG setup doesn't have TPM_DEVICE_NULL option
#if 0
      if (Opd->T2c.TpmDevice != TPM_DEVICE_NULL) {
	Opd->T2c.TpmDevice = TPM_DEVICE_NULL; Opd->T2cm = TRUE;	// Disable detected TPM, Data modified
      }
#endif
//APTIOV_SERVER_OVERRIDE_END : AMI's TCG setup doesn't have TPM_DEVICE_NULL option

      if (Opd->Tcg2Protocol != NULL) {				// If TCG2 protocol found
	e = OXP_SUCCESS;					//   Success
	DEBUG ((EFI_D_ERROR, "Success\n"));
      } else {							// If TCG2 protocol not found
        DEBUG ((EFI_D_ERROR, "Implementation Error -> BUG\n"));
      }
      break;

    case TPM_DEVICE_1_2:							// TPM 1.2
      DEBUG ((EFI_D_ERROR, "      TPM1.2: "));
      if (Opd->TcgProtocol != NULL) {				// If TCG protocol found
	DEBUG ((EFI_D_ERROR, "Deactivate, Disable -->\n"));
	e = OT_Tpm12DeactivateDisable (Opd);			// Deactivate, Disable
      } else {							// If TCG protocol not found
        DEBUG ((EFI_D_ERROR, "Implementation Error -> BUG\n"));
      }
      break;

    //case TPM_DEVICE_NULL:					// No TPM
    default:							// Invalid
      DEBUG ((EFI_D_ERROR, "      None/Invalid TPM Device: Error\n"));
      break;
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_DTpmDisable()  } -> Status 0x%01x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else { DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}

//------------------------------------------------------------------------------------------------------
// ****************************	fTPM functions *********************************************************
//------------------------------------------------------------------------------------------------------
/**
  OT_FTpmClear(): Clear fTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_DTPM_CLEAR_ERROR
**/
UINT8
OT_FTpmClear (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_FTpmClear(): Detected TPM Device 0x%01x\n", Opd->T2dd.TpmDeviceDetected));
  e = OXP_DTPM_CLEAR_ERROR;					// Assume Error
  switch (Opd->T2dd.TpmDeviceDetected) {			// Detected TPM device
    case TPM_DEVICE_2_0_DTPM:					// TPM 2.0
      DEBUG ((EFI_D_ERROR, "      Clear fTPM --> TPM2.0: "));
      if (Opd->Tcg2Protocol != NULL) {				// If TCG2 protocol found
// APTIOV_SERVER_OVERRIDE_RC_START: TPM2 command clear using AMI's Tree protocol
        DEBUG ((EFI_D_ERROR, "Tpm2CommandClear() --> "));
	if (Tpm2CommandClear (Auth, Opd->Tcg2Protocol) == EFI_SUCCESS) {		// If success
//APTIOV_SERVER_OVERRIDE_RC_END: TPM2 command clear using AMI's Tree protocol
	  e = OXP_SUCCESS;					//   Indicate success
	  DEBUG ((EFI_D_ERROR, "Success\n"));
	} else {						// If not sucess
	  DEBUG ((EFI_D_ERROR, "Error\n"));
	}
      } else {							// If TCG2 protocol not found
        DEBUG ((EFI_D_ERROR, "Implementation Error -> BUG\n"));
      }
      break;

    //case TPM_DEVICE_1_2:					// TPM 1.2
    //case TPM_DEVICE_NULL:					// No TPM
    default:							// Invalid
      DEBUG ((EFI_D_ERROR, "      TPM1.2/None/Invalid TPM Device: Implementation Error -> BUG\n"));
      break;
  }

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_FTpmClear()  } -> Status 0x%01x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else {DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status code
}

//------------------------------------------------------------------------------------------------------
/**
  OT_FTpmEnable(): Enable fTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_FTPM_ENABLE_ERROR
**/
UINT8
OT_FTpmEnable (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_FTpmEnable()  { -> Detected TPM Device 0x%01x\n", Opd->T2dd.TpmDeviceDetected));

  e = OXP_SUCCESS;						// Assume success
  if ((Opd->Merc.MePttEnabled == 0) ||				// If PTT disabled
      ((Opd->FeatureEnabled & OXP_FTPM_ENABLED_BIT) == 0)) {
    DEBUG ((EFI_D_ERROR, "      PttHeciUpdateState(TRUE): Enable PTT in ME --> "));
    if ((PttHeciUpdateState (TRUE)) == EFI_SUCCESS) {
      DEBUG ((EFI_D_ERROR, "Success\n"));
      DEBUG ((EFI_D_ERROR, "      ME Configuration Setup: PTT --> Enable\n"));
      Opd->Merc.MePttEnabled = 1; Opd->Mercm = TRUE;		//   Enable PTT, Data modified
    } else {
      e = OXP_FTPM_ENABLE_ERROR;				// Error
      DEBUG ((EFI_D_ERROR, "Error\n"));
    }
  }
  
  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_FTpmEnable()  } -> Status 0x%01x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else {DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}

//------------------------------------------------------------------------------------------------------
/**
  OT_FTpmDisable(): Disable fTPM.

  @param  Auth		INPUT:	Ptr to TPM2B_AUTH, Platform auth value (NULL: no platform auth change)
  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_FTPM_DISABLE_ERROR
**/
UINT8
OT_FTpmDisable (
  IN     TPM2B_AUTH		*Auth,
  IN OUT OT_PLATFORM_DATA	*Opd
  )
{
  // Local data
  UINT8		e;

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_FTpmDisable(): Detected TPM Device 0x%01x\n", Opd->T2dd.TpmDeviceDetected));

  e = OXP_SUCCESS;						// Assume success
  if ((Opd->Merc.MePttEnabled != 0) ||				// If PTT enabled
      ((Opd->FeatureEnabled & OXP_FTPM_ENABLED_BIT) != 0)) {
    DEBUG ((EFI_D_ERROR, "      PttHeciUpdateState(FALSE): Disable PTT in ME --> "));
    if ((PttHeciUpdateState (FALSE)) == EFI_SUCCESS) {
      DEBUG ((EFI_D_ERROR, "Success\n"));
      DEBUG ((EFI_D_ERROR, "      ME Configuration Setup: PTT --> Disable\n"));
      Opd->Merc.MePttEnabled = 0; Opd->Mercm = TRUE;		//   Disable PTT, Data modified
    } else {
      e = OXP_FTPM_DISABLE_ERROR;				// Error
      DEBUG ((EFI_D_ERROR, "Error\n"));
    }
  }
  
  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_FTpmDisable()  } -> Status 0x%01x -> ", e));
  if (e == OXP_SUCCESS) { DEBUG ((EFI_D_ERROR, "Success\n")); }
  else {DEBUG ((EFI_D_ERROR, "Error\n")); }

  return e;							// Status
}

//------------------------------------------------------------------------------------------------------
// ****************************	TXT functions **********************************************************
//------------------------------------------------------------------------------------------------------
/**
  OT_TxtEnable(): Enable TXT.

  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_TXT_ENABLE_ERROR
**/
UINT8
OT_TxtEnable (
  IN OUT OT_PLATFORM_DATA	*Opd
  )
{
  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_EnableTxt()  { -> Enable all TXT related BIOS Setup Options\n"));
  if (Opd->Spcc.ProcessorVmxEnable == 0) { Opd->Spcc.ProcessorVmxEnable = 1; Opd->Spccm = TRUE; }					// Enable VMX, Data modified
  if (Opd->Spcc.ProcessorSmxEnable == 0) { Opd->Spcc.ProcessorSmxEnable = 1; Opd->Spcc.ProcessorMsrLockControl = 1; Opd->Spccm = TRUE; }// Enable SMX, Data modified
  if (Opd->Sic.VTdSupport == 0) { Opd->Sic.VTdSupport = 1; Opd->Sicm = TRUE; }								// Enable VT-d, Data modified
  if (Opd->Spcc.ProcessorLtsxEnable == 0) { Opd->Spcc.ProcessorLtsxEnable = 1; Opd->Spcc.LockChipset = 1; Opd->Spccm = TRUE; }		// Enable LTSX, Data modified

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_EnableTxt()  } -> Status 0x00 -> Success\n"));

  return OXP_SUCCESS;																// Success
}

//------------------------------------------------------------------------------------------------------
/**
  OT_TxtDisable(): Disable TXT.

  @param  Opd		INPUT:  Ptr to OT_PLATFORM_DATA
  @param  Result	OUTPUT: UINT8, Status Code
				OXP_SUCCESS, OXP_TXT_ENABLE_ERROR
**/
UINT8
OT_TxtDisable (
  IN OUT OT_PLATFORM_DATA	*Opd
  )
{
  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_DisableTxt()  { -> Disable all TXT related BIOS Setup Options\n"));
  if (Opd->Spcc.ProcessorVmxEnable != 0) { Opd->Spcc.ProcessorVmxEnable = 0; Opd->Spccm = TRUE; }	// Disable VMX, Data modified
  if (Opd->Spcc.ProcessorSmxEnable != 0) { Opd->Spcc.ProcessorSmxEnable = 0; Opd->Spccm = TRUE; }	// Disable SMX, Data modified
  if (Opd->Sic.VTdSupport != 0) { Opd->Sic.VTdSupport = 0; Opd->Sicm = TRUE; }				// Disable VT-d, Data modified
  if (Opd->Spcc.ProcessorLtsxEnable != 0) { Opd->Spcc.ProcessorLtsxEnable = 0; Opd->Spccm = TRUE; }	// Disable LTSX, Data modified

  DEBUG ((EFI_D_ERROR, "[OOB-Platform] OT_DisableTxt()  } -> Status 0x00 -> Success\n"));

  return OXP_SUCCESS;																// Success
}

//----------------------------- } Platform Information -------------------------------------------------
