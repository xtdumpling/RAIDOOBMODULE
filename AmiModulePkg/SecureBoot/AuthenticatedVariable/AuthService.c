//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  Implement authentication services for the authenticated variables.

  Caution: This module requires additional review when modified.
  This driver will have external input - variable data. It may be input in SMM mode.
  This external input must be validated carefully to avoid security issue like
  buffer overflow, integer overflow.
  Variable attribute should also be checked to avoid authentication bypass.
     The whole SMM authentication variable design relies on the integrity of flash part and SMM.
  which is assumed to be protected by platform.  All variable code and metadata in flash/SMM Memory
  may not be modified without authorization. If platform fails to protect these resources,
  the authentication service provided in this driver will be broken, and the behavior is undefined.

  VerifyVariable()is the main function to do variable authentication.

  VerifyVariable() and VerifyVariable2() are sub function to do verification.
  They will do basic validation for authentication data structure, then call crypto protocol
  to verify the signature.

Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Setup.h>
#include "AuthVariable.h"

extern EFI_GUID gEfiGlobalVariableGuid;

static EFI_GUID  gSecureSetupGuid = SECURITY_FORM_SET_GUID;
static EFI_GUID  gAmiDeploymentModeNvGuid = AMI_DEPLOYMENT_MODE_VARIABLE_GUID;
static EFI_GUID  gAmiVendorKeysNvGuid = AMI_VENDOR_KEYS_NV_VARIABLE_GUID;

static EFI_GUID  mSignatureSupport[SIGSUPPORT_NUM] = {SIGSUPPORT_LIST};

static SECURE_BOOT_SETUP_VAR  mSecureBootSetup;
static SECURE_BOOT_MODE_TYPE  mSecureBootMode;
static UINT8                  mVendorKeyState;
// #define ECR1557
// ECR1557: Clarification for Secure Boot Mode and SecureBoot variable state changes
// SecureBoot Variable state state transition required. Only one transition may be queued at a time.
// Further SetVariable() calls that attempt to initiate a Secure Boot mode transition are failed with EFI_ALREADY_STARTED
static BOOLEAN                mbAlreadyStarted;


AMI_DIGITAL_SIGNATURE_PROTOCOL *mDigitalSigProtocol = NULL;

static CHAR16* ReservedReadOnlyVarNameList[] = {
 EFI_SETUP_MODE_NAME,
 EFI_SECURE_BOOT_NAME,
 EFI_AUDIT_MODE_NAME,
 EFI_DEPLOYED_MODE_NAME,
 EFI_VENDOR_KEYS_VARIABLE_NAME,
 EFI_SIGNATURE_SUPPORT_NAME,
 EFI_DB_DEFAULT_VARIABLE_NAME, 
 EFI_DBX_DEFAULT_VARIABLE_NAME,
 EFI_DBT_DEFAULT_VARIABLE_NAME,
 EFI_DBR_DEFAULT_VARIABLE_NAME,
 EFI_PK_DEFAULT_VARIABLE_NAME,
 EFI_KEK_DEFAULT_VARIABLE_NAME,
 NULL
};

//
// Secure Boot Mode state machine
//
SECURE_BOOT_MODE mSecureBootState[SecureBootModeTypeMax] = {
  // USER MODE
  {
      AUDIT_MODE_DISABLE,                        // AuditMode
      FALSE,                                     // IsAuditModeRO, AuditMode is RW
      DEPLOYED_MODE_DISABLE,                     // DeployedMode
      FALSE,                                     // IsDeployedModeRO, DeployedMode is RW
      SETUP_MODE_DISABLE,                        // SetupMode
                                                 // SetupMode is always RO
      SECURE_BOOT_MODE_ENABLE                    // SecureBoot
  },
  // SETUP MODE
  {
      AUDIT_MODE_DISABLE,                        // AuditMode
      FALSE,                                     // IsAuditModeRO, AuditMode is RW
      DEPLOYED_MODE_DISABLE,                     // DeployedMode
      TRUE,                                      // IsDeployedModeRO, DeployedMode is RO
      SETUP_MODE_ENABLE,                         // SetupMode
                                                 // SetupMode is always RO
      SECURE_BOOT_MODE_DISABLE                   // SecureBoot
  },
  // AUDIT MODE
  {
      AUDIT_MODE_ENABLE,                         // AuditMode
      TRUE,                                      // AuditModeValAttr RO, AuditMode is RO
      DEPLOYED_MODE_DISABLE,                     // DeployedMode
      TRUE,                                      // DeployedModeValAttr RO, DeployedMode is RO
      SETUP_MODE_ENABLE,                         // SetupMode
                                                 // SetupMode is always RO
      SECURE_BOOT_MODE_DISABLE                   // SecureBoot
  },
  // DEPLOYED MODE
  {
      AUDIT_MODE_DISABLE,                        // AuditMode, AuditMode is RO
      TRUE,                                      // AuditModeValAttr RO
      DEPLOYED_MODE_ENABLE,                      // DeployedMode
      TRUE,                                      // DeployedModeValAttr RO, DeployedMode is RO
      SETUP_MODE_DISABLE,                        // SetupMode
                                                 // SetupMode is always RO
      SECURE_BOOT_MODE_ENABLE                    // SecureBoot
  }
};

/**
  Finds variable in storage blocks of volatile and non-volatile storage areas.

  This code finds variable in storage blocks of volatile and non-volatile storage areas.
  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.

  @param[in]  VariableName          Name of the variable to be found.
  @param[in]  VendorGuid            Variable vendor GUID to be found.
  @param[out] Data                  Pointer to data address.
  @param[out] DataSize              Pointer to data size.

  @retval EFI_INVALID_PARAMETER     If VariableName is not an empty string,
                                    while VendorGuid is NULL.
  @retval EFI_SUCCESS               Variable successfully found.
  @retval EFI_NOT_FOUND             Variable not found

**/
EFI_STATUS
AuthServiceInternalFindVariable (
  IN  CHAR16            *VariableName,
  IN  EFI_GUID          *VendorGuid,
  OUT VOID              **Data,
  OUT UINTN             *DataSize
  )
{
  UINT32  VarAttr = 0;

  return FindVariable ( VariableName, VendorGuid, &VarAttr, DataSize, Data);
}

/**
  Update the variable region with Variable information.

  @param[in] VariableName           Name of variable.
  @param[in] VendorGuid             Guid of variable.
  @param[in] Data                   Data pointer.
  @param[in] DataSize               Size of Data.
  @param[in] Attributes             Attribute value of the variable.

  @retval EFI_SUCCESS               The update operation is success.
  @retval EFI_INVALID_PARAMETER     Invalid parameter.
  @retval EFI_WRITE_PROTECTED       Variable is write-protected.
  @retval EFI_OUT_OF_RESOURCES      There is not enough resource.

**/
EFI_STATUS
AuthServiceInternalUpdateVariable (
  IN CHAR16             *VariableName,
  IN EFI_GUID           *VendorGuid,
  IN VOID               *Data,
  IN UINTN              DataSize,
  IN UINT32             Attributes
  )
{
  return DxeSetVariable(VariableName, VendorGuid, Attributes, DataSize, Data);
}

/**
   This function will be invoked to convert runtime pointers to virtual address

  This code finds variable in storage blocks of volatile and non-volatile storage areas.
  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.

  @param[in]  none
  @retval none

**/
VOID
AuthServiceVirtualFixup()
{
    static BOOLEAN  bSigProtoVirtual = FALSE;
    VOID            **p;
    UINT8           i;

    //if mDigitalSigProtocol is NULL, nothing to fix up
    if (!mDigitalSigProtocol) return;
    
    // This function gets called from Nvramdxe.c, do nothing when 
    // the function is called second time.
    if (bSigProtoVirtual == TRUE) return;
    else bSigProtoVirtual = TRUE;
//AVAR_TRACE((TRACE_ALWAYS,"AuthVarService mDigitalSig Virtual addr Fixup\n"));
    //Fixup mDigitalSigProtocol member functions
    for(i=0,p = (VOID**)mDigitalSigProtocol; p < (VOID**)(mDigitalSigProtocol + 1); i++,p++)
//    {
//AVAR_TRACE((TRACE_ALWAYS,"mSigAPI[%d] before Virtual MemFixup = %lx (%lx), ", i, p));
        pRS->ConvertPointer(0, p);
//AVAR_TRACE((TRACE_ALWAYS,"After = %lx\n", p));
//    }
    pRS->ConvertPointer(0,&mDigitalSigProtocol);
//    AVarRuntime = TRUE;
}

/**
  This function initialize mDigitalSigProtocol in SMM

  @param[in]  IN EFI_EVENT Event - Event that was triggered
  @param[in]  IN VOID *Context - data pointer to information that is defined 
              when the event is registered

  @retval EFI_STATUS

**/
static EFI_STATUS 
InitSmmAuthServiceCallback (
            IN CONST EFI_GUID  *Protocol,
            IN VOID            *Interface,
            IN EFI_HANDLE      Handle
)
{
//    return pBS->LocateProtocol(
    return pSmst->SmmLocateProtocol(    
            &gAmiSmmDigitalSignatureProtocolGuid, NULL, &mDigitalSigProtocol
            );
}

/**
  This function calls InitSmmAuthServiceCallback to initialize 
              DigitalSigProtocol in SMM by trying to Locate
              DigitalSigProtocol. If Protocol is not installed yet 
              RegisterProtocolCallback will be called.

  @param[in]  none

  @retval VOID

**/
VOID
AuthVariableServiceInitSMM ()
{
    EFI_STATUS    Status;
    VOID          *pSmm;
    UINT8         *Data;
    UINTN          DataSize;
    
    // w/a: don't print debug traces from SMM
//    AVarRuntime = TRUE;

    //
    // Initialize Authenticated variable in global space.
    //
    Status = AuthServiceInternalFindVariable (AMI_DEPLOYMENT_MODE_VARIABLE_NAME, &gAmiDeploymentModeNvGuid, (VOID **)&Data, &DataSize);
    mSecureBootMode = (SECURE_BOOT_MODE_TYPE)*Data;
    AVAR_TRACE((TRACE_ALWAYS,"SMM.mSecureBootMode %X\n", mSecureBootMode));
    //
    Status = AuthServiceInternalFindVariable (EFI_VENDOR_KEYS_NV_VARIABLE_NAME, &gAmiVendorKeysNvGuid, (VOID **) &Data, &DataSize);
    if (!EFI_ERROR (Status)) {
      mVendorKeyState = *(UINT8 *)Data;
    }
    AVAR_TRACE((TRACE_ALWAYS,"SMM.mVendorKeyState %X\n", mVendorKeyState));

    // Set mSecureBoot structure
    GetmSecureBootSetup();

    mbAlreadyStarted = FALSE;

    Status = InitSmmAuthServiceCallback (NULL, NULL, NULL);
    if (EFI_ERROR(Status))
        pSmst->SmmRegisterProtocolNotify(
            &gAmiSmmDigitalSignatureProtocolGuid,
            InitSmmAuthServiceCallback,
            &pSmm
        );
}

/**
  This function initialize mDigitalSigProtocol not in SMM
  
  @param[in]  IN EFI_EVENT Event - Event that was triggered
  @param[in]  IN VOID *Context - data pointer to information that is defined 
              when the event is registered

  @retval EFI_STATUS

**/

static EFI_STATUS 
InitAuthServiceCallback (IN EFI_EVENT Event, IN VOID *Context)
{
    return pBS->LocateProtocol(
            &gAmiDigitalSignatureProtocolGuid, NULL, &mDigitalSigProtocol
            );
}

/**
  Initialize Secure Boot variables.
   This function calls InitAuthServiceCallback to initialize 
   DigitalSigProtocol not in SMM by trying to Locate
   DigitalSigProtocol. If Protocol is not installed yet 
   RegisterProtocolCallback will be called.

  @retval none 

**/
VOID
AuthVariableServiceInit ()
{
  EFI_EVENT       Event;
  VOID            *p;

  EFI_STATUS             Status;
  UINT8                  *Data;
  UINTN                  DataSize;
  UINT32                 SecureBoot;

  SECURE_BOOT_MODE_TYPE  SecureBootMode;
  BOOLEAN                IsPkPresent;

  //
  // Find "PK" variable
  //
  IsPkPresent = GetPkPresent();
  //
  // Init "SecureBootMode" variable.
  // Initial case
  //   SecureBootMode doesn't exist. Init it with PK state
  // 3 inconsistency cases need to sync
  //   1.1 Add PK     -> system break -> update SecureBootMode Var
  //   1.2 Delete PK  -> system break -> update SecureBootMode Var
  //   1.3 Set AuditMode ->Delete PK  -> system break -> Update SecureBootMode Var
  //
  Status = AuthServiceInternalFindVariable (AMI_DEPLOYMENT_MODE_VARIABLE_NAME, &gAmiDeploymentModeNvGuid, (VOID **)&Data, &DataSize);
  if (EFI_ERROR(Status)) {
    //
    // Variable driver Initial Case
    //
    if (IsPkPresent) {
      SecureBootMode = SecureBootModeTypeUserMode;
    } else {
      SecureBootMode = SecureBootModeTypeSetupMode;
    }
  } else {
    //
    // 3 inconsistency cases need to sync
    //
    SecureBootMode = (SECURE_BOOT_MODE_TYPE)*Data;
    ASSERT(SecureBootMode < SecureBootModeTypeMax);

    if (IsPkPresent) {
      //
      // 3.1 Add PK     -> system break -> update SecureBootMode Var
      //
      if (SecureBootMode == SecureBootModeTypeSetupMode) {
        SecureBootMode = SecureBootModeTypeUserMode;
      } else if (SecureBootMode == SecureBootModeTypeAuditMode) {
        SecureBootMode = SecureBootModeTypeDeployedMode;
      }
    } else {
      //
      // 3.2 Delete PK -> system break -> update SecureBootMode Var
      // 3.3 Set AuditMode ->Delete PK  -> system break -> Update SecureBootMode Var. Re-init to be SetupMode
      //
      if ((SecureBootMode == SecureBootModeTypeUserMode) || (SecureBootMode == SecureBootModeTypeDeployedMode)) {
        SecureBootMode = SecureBootModeTypeSetupMode;
      }
    }
  }

  if (EFI_ERROR(Status) || (SecureBootMode != (SECURE_BOOT_MODE_TYPE)*Data)) {
    //
    // Update SecureBootMode Var
    //
    Status = AuthServiceInternalUpdateVariable (
               AMI_DEPLOYMENT_MODE_VARIABLE_NAME,
               &gAmiDeploymentModeNvGuid,
               &SecureBootMode,
               sizeof (UINT8),
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS
               );
    AVAR_TRACE((TRACE_ALWAYS,"Init Var %S = %d, %r\n",
            AMI_DEPLOYMENT_MODE_VARIABLE_NAME, SecureBootMode, Status));
      if (EFI_ERROR (Status)) {
        goto Exit;
      } 
  }
  //
  // Save SecureBootMode in global space
  //
  mSecureBootMode = SecureBootMode;
  AVAR_TRACE((TRACE_ALWAYS,"mSecureBootMode %X\n", mSecureBootMode));
  //
  // Init "AuditMode"
  //
  Status = AuthServiceInternalUpdateVariable (
             EFI_AUDIT_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &mSecureBootState[SecureBootMode].AuditMode,
             sizeof(UINT8),
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS
             );
  AVAR_TRACE((TRACE_ALWAYS,"Init Var %S = %d, %r\n",
          EFI_AUDIT_MODE_NAME, mSecureBootState[SecureBootMode].AuditMode, Status));

  if (EFI_ERROR (Status)) {
    goto Exit;
  } 
  //
  // Init "DeployedMode"
  //
  Status = AuthServiceInternalUpdateVariable (
             EFI_DEPLOYED_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &mSecureBootState[SecureBootMode].DeployedMode,
             sizeof(UINT8),
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS
             );
  AVAR_TRACE((TRACE_ALWAYS,"Init Var %S = %d, %r\n",
          EFI_DEPLOYED_MODE_NAME, mSecureBootState[SecureBootMode].DeployedMode, Status));

  if (EFI_ERROR (Status)) {
    goto Exit;
  } 
  //
  // Init "SetupMode"
  //
  Status = AuthServiceInternalUpdateVariable (
             EFI_SETUP_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &mSecureBootState[SecureBootMode].SetupMode,
             sizeof(UINT8),
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS
             );
  AVAR_TRACE((TRACE_ALWAYS,"Init Var %S = %d, %r\n",
          EFI_SETUP_MODE_NAME, mSecureBootState[SecureBootMode].SetupMode, Status));

  if (EFI_ERROR (Status)) {
    goto Exit;
  } 
  // Set SecureBoot, both conditions must be met 
  GetmSecureBootSetup();
  SecureBoot = (IsPkPresent && (mSecureBootSetup.SecureBootSupport==SECURE_BOOT))?1:0;

  Status = AuthServiceInternalUpdateVariable (
             EFI_SECURE_BOOT_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &SecureBoot,
             sizeof (UINT8),
             EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS
             );

  AVAR_TRACE((TRACE_ALWAYS,"Init Var %S = %d, %r\n",
          EFI_SECURE_BOOT_MODE_NAME, SecureBoot, Status));

  if (EFI_ERROR (Status)) {
    goto Exit;
  } 

  // Create "SignatureSupport" with RO access 
  AuthServiceInternalUpdateVariable(
          EFI_SIGNATURE_SUPPORT_NAME,
          &gEfiGlobalVariableGuid,
          &mSignatureSupport, 
          SIGSUPPORT_NUM * sizeof(EFI_GUID),
          EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS
  );
  //
  // Check "VendorKeysNv" variable's existence and create "VendorKeys" variable accordingly.
  //
  Status = AuthServiceInternalFindVariable (EFI_VENDOR_KEYS_NV_VARIABLE_NAME, &gAmiVendorKeysNvGuid, (VOID **) &Data, &DataSize);
  if (!EFI_ERROR (Status)) {
    mVendorKeyState = *(UINT8 *)Data;
  } else {
    //
    // "VendorKeysNv" not exist, initialize it in VENDOR_KEYS_VALID state.
    //
    mVendorKeyState = VENDOR_KEYS_VALID;
    Status = AuthServiceInternalUpdateVariable (
               EFI_VENDOR_KEYS_NV_VARIABLE_NAME,
               &gAmiVendorKeysNvGuid,
               &mVendorKeyState,
               sizeof (UINT8),
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS 
               );
      if (EFI_ERROR (Status)) {
        goto Exit;
      } 
  }

  //
  // Create "VendorKeys" variable with BS+RT attribute set.
  //
  Status = AuthServiceInternalUpdateVariable (
             EFI_VENDOR_KEYS_VARIABLE_NAME,
             &gEfiGlobalVariableGuid,
             &mVendorKeyState,
             sizeof (UINT8),
             EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS
             );
  AVAR_TRACE((TRACE_ALWAYS, "Init Var %S = %d, %r\n", EFI_VENDOR_KEYS_VARIABLE_NAME, mVendorKeyState, Status));
  if (EFI_ERROR (Status)) {
    goto Exit;
  } 

  mbAlreadyStarted = FALSE;

  if (EFI_ERROR(InitAuthServiceCallback (NULL, NULL)))
      RegisterProtocolCallback(
          &gAmiDigitalSignatureProtocolGuid,
          InitAuthServiceCallback,
          NULL,
          &Event,
          &p
      );

Exit:
  
  return;
}

/**
  Update SecureBootMode variable.

  @param[in] NewMode                New Secure Boot Mode.

  @retval EFI_SUCCESS               The initialization operation is successful.
  @retval EFI_OUT_OF_RESOURCES      There is not enough resource.

**/
EFI_STATUS
UpdateSecureBootMode(
  IN  SECURE_BOOT_MODE_TYPE  NewMode
  )
{
  EFI_STATUS              Status;

  //
  // Update "SecureBootMode" variable to new Secure Boot Mode
  //
  Status = AuthServiceInternalUpdateVariable (
             AMI_DEPLOYMENT_MODE_VARIABLE_NAME,
             &gAmiDeploymentModeNvGuid,
             &NewMode,
             sizeof (UINT8),
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS
             );

  if (!EFI_ERROR(Status)) {
    AVAR_TRACE((TRACE_ALWAYS,"SecureBootMode Update to %x\n", NewMode));
    mSecureBootMode = NewMode;
  } else {
    AVAR_TRACE((TRACE_ALWAYS, "Update SecureBootMode Variable fail %r\n", Status));
  }

  return Status;
}

/**
  Current secure boot mode is AuditMode. This function performs secure boot mode transition
  to a new mode.

  @param[in] NewMode                New Secure Boot Mode.

  @retval EFI_SUCCESS               The initialization operation is successful.
  @retval EFI_OUT_OF_RESOURCES      There is not enough resource.

**/
EFI_STATUS
TransitionFromAuditMode(
  IN  SECURE_BOOT_MODE_TYPE               NewMode
  )
{
  EFI_STATUS  Status;
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
  VOID        *AuditVarData;
  VOID        *DeployedVarData;
#endif
  VOID        *SetupVarData;
  VOID        *SecureBootVarData;
  UINTN       DataSize;

  //
  // AuditMode/DeployedMode/SetupMode/SecureBoot are all NON_NV variable maintained by Variable driver
  // they can be RW. but can't be deleted. so they can always be found.
  //
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
  Status = AuthServiceInternalFindVariable (
             EFI_AUDIT_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &AuditVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }

  Status = AuthServiceInternalFindVariable (
             EFI_DEPLOYED_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &DeployedVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }
#endif

  Status = AuthServiceInternalFindVariable (
             EFI_SETUP_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &SetupVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }

  Status = AuthServiceInternalFindVariable (
             EFI_SECURE_BOOT_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &SecureBootVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }  
  //
  // Make Secure Boot Mode transition ATOMIC
  // Update Private NV SecureBootMode Variable first, because it may fail due to NV range overflow.
  // other transition logic are all memory operations.
  //
  Status = UpdateSecureBootMode(NewMode);
  if (EFI_ERROR(Status)) {
    //
    // UpdateSecureBootMode fails and no other variables are updated before. rollback this transition
    //
    return Status;
  }
  if (NewMode == SecureBootModeTypeDeployedMode) {
    //
    // Since PK is enrolled, can't rollback, always update SecureBootMode in memory
    //
    //
    // AuditMode ----> DeployedMode
    // Side Effects
    //   AuditMode =: 0 / DeployedMode := 1 / SetupMode := 0
    //
    // Update the value of AuditMode variable by a simple mem copy, this could avoid possible
    // variable storage reclaim at runtime.
    //
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
    CopyMem (AuditVarData, &mSecureBootState[NewMode].AuditMode, sizeof(UINT8));
    //
    // Update the value of DeployedMode variable by a simple mem copy, this could avoid possible
    // variable storage reclaim at runtime.
    //
    CopyMem (DeployedVarData, &mSecureBootState[NewMode].DeployedMode, sizeof(UINT8));
#endif
    //
    // Update the value of SetupMode variable by a simple mem copy, this could avoid possible
    // variable storage reclaim at runtime.
    //
    CopyMem (SetupVarData, &mSecureBootState[NewMode].SetupMode, sizeof(UINT8));
    //
    // ECR1557: Clarification for Secure Boot Mode and SecureBoot variable state changes
    // SecureBoot Variable state state transition required. Only one transition may be queued at a time.
    // Further SetVariable() calls that attempt to initiate a Secure Boot mode transition are failed with EFI_ALREADY_STARTED
    //
#if defined(ECR1557)
    if (*(UINT8*)SecureBootVarData != mSecureBootState[NewMode].SecureBoot ) {
        if(mbAlreadyStarted == FALSE)
            mbAlreadyStarted = TRUE;
//            return EFI_WARN_RESET_REQUIRED;
        else
            return EFI_ALREADY_STARTED;
    }
#endif

  }

  return Status;
}

/**
  Current secure boot mode is DeployedMode. This function performs secure boot mode transition
  to a new mode.

  @param[in] NewMode                New Secure Boot Mode.

  @retval EFI_SUCCESS               The initialization operation is successful.
  @retval EFI_OUT_OF_RESOURCES      There is not enough resource.

**/
EFI_STATUS
TransitionFromDeployedMode(
  IN  SECURE_BOOT_MODE_TYPE               NewMode
  )
{
  EFI_STATUS  Status;
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
  VOID        *DeployedVarData;
#endif
  VOID        *SetupVarData;
  VOID        *SecureBootVarData;
  UINTN       DataSize;

  //
  // AuditMode/DeployedMode/SetupMode/SecureBoot are all NON_NV variable maintained by Variable driver
  // they can be RW. but can't be deleted. so they can always be found.
  //
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
  Status = AuthServiceInternalFindVariable (
             EFI_DEPLOYED_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &DeployedVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }
#endif
  Status = AuthServiceInternalFindVariable (
             EFI_SETUP_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &SetupVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }

  Status = AuthServiceInternalFindVariable (
             EFI_SECURE_BOOT_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &SecureBootVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }  
  //
  // Make Secure Boot Mode transition ATOMIC
  // Update Private NV SecureBootMode Variable first, because it may fail due to NV range overflow.
  // other transition logic are all memory operations.
  //
  Status = UpdateSecureBootMode(NewMode);
  if (EFI_ERROR(Status)) {
    //
    // UpdateSecureBootMode fails and no other variables are updated before. rollback this transition
    //
    return Status;
  }

  switch(NewMode) {
    case SecureBootModeTypeUserMode:
      //
      // DeployedMode ----> UserMode
      // Side Effects
      //   DeployedMode := 0
      //
      // Platform Specific DeployedMode clear.
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
      CopyMem (DeployedVarData, &mSecureBootState[NewMode].DeployedMode, sizeof(UINT8));
#endif
      break;

    case SecureBootModeTypeSetupMode:
      //
      // Since PK is processed before, can't rollback, still update SecureBootMode in memory
      //
      // DeployedMode ----> SetupMode
      //
      // Platform Specific PKpub clear or Delete Pkpub
      // Side Effects
      //   DeployedMode := 0 / SetupMode := 1 / SecureBoot := 0
      //
      // Update the value of DeployedMode variable by a simple mem copy, this could avoid possible
      // variable storage reclaim at runtime.
      //
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
      CopyMem (DeployedVarData, &mSecureBootState[NewMode].DeployedMode, sizeof(UINT8));
#endif
      //
      // Update the value of SetupMode variable by a simple mem copy, this could avoid possible
      // variable storage reclaim at runtime.
      //
      CopyMem (SetupVarData, &mSecureBootState[NewMode].SetupMode, sizeof(UINT8));
      //
      // ECR1557: Clarification for Secure Boot Mode and SecureBoot variable state changes
      // SecureBoot Variable state state transition required. Only one transition may be queued at a time.
      // Further SetVariable() calls that attempt to initiate a Secure Boot mode transition are failed with EFI_ALREADY_STARTED
      //
#if defined(ECR1557)
      if (*(UINT8*)SecureBootVarData != mSecureBootState[NewMode].SecureBoot ) {
          if(mbAlreadyStarted == FALSE)
              mbAlreadyStarted = TRUE;
//            return EFI_WARN_RESET_REQUIRED;
          else
              return EFI_ALREADY_STARTED;
      }
#endif

      break;

    default:
      AVAR_TRACE((TRACE_ALWAYS, "Invalid state transition from %x to %x\n", SecureBootModeTypeDeployedMode, NewMode));
      ASSERT(FALSE);
  }

  return Status;
}

/**
  Current secure boot mode is UserMode. This function performs secure boot mode transition
  to a new mode.

  @param[in] NewMode                New Secure Boot Mode.

  @retval EFI_SUCCESS               The initialization operation is successful.
  @retval EFI_OUT_OF_RESOURCES      There is not enough resource.

**/
EFI_STATUS
TransitionFromUserMode(
  IN  SECURE_BOOT_MODE_TYPE               NewMode
  )
{
  EFI_STATUS   Status;
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
  VOID         *AuditVarData;
  VOID         *DeployedVarData;
  VOID         *PkVarData;
#endif  
  VOID         *SetupVarData;
  VOID         *SecureBootVarData;
  UINTN        DataSize;

  //
  // AuditMode/DeployedMode/SetupMode/SecureBoot are all NON_NV variable maintained by Variable driver
  // they can be RW. but can't be deleted. so they can always be found.
  //
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT  
  Status = AuthServiceInternalFindVariable (
             EFI_AUDIT_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &AuditVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }

  Status = AuthServiceInternalFindVariable (
             EFI_DEPLOYED_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &DeployedVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }
#endif
  Status = AuthServiceInternalFindVariable (
             EFI_SETUP_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &SetupVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }

  Status = AuthServiceInternalFindVariable (
             EFI_SECURE_BOOT_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &SecureBootVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }
  //
  // Make Secure Boot Mode transition ATOMIC
  // Update Private NV SecureBootMode Variable first, because it may fail due to NV range overflow. 
  // Other transition logic are all memory operations and PK delete is assumed to be always successful.
  //
  Status = UpdateSecureBootMode(NewMode);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  switch(NewMode) {
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
    case SecureBootModeTypeDeployedMode:
      //
      // UserMode ----> DeployedMode
      // Side Effects
      //   DeployedMode := 1
      //
      CopyMem (DeployedVarData, &mSecureBootState[NewMode].DeployedMode, sizeof(UINT8));
      break;

    case SecureBootModeTypeAuditMode:
      //
      // UserMode ----> AuditMode
      // Side Effects
      //   Delete PKpub / SetupMode := 1 / SecureBoot := 0
      //
      // Delete PKpub without verification. Should always succeed.
      //
      Status = AuthServiceInternalFindVariable (EFI_PLATFORM_KEY_NAME, &gEfiGlobalVariableGuid, (VOID **) &PkVarData, &DataSize);
      if (!EFI_ERROR (Status)) {
          PkVarData = NULL;
          Status = AuthServiceInternalUpdateVariable (
                     EFI_PLATFORM_KEY_NAME,
                     &gEfiGlobalVariableGuid,
                     PkVarData,
                     0,
                     EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS
                     );
      }
      if (EFI_ERROR(Status)) {
          AVAR_TRACE((TRACE_ALWAYS, "UserMode -> AuditMode. Delete PK fail %r\n", Status));
          //
          // restore Private NV SecureBootMode Variable
          //
          if (EFI_ERROR(UpdateSecureBootMode(SecureBootModeTypeUserMode))) {
              AVAR_TRACE((TRACE_ALWAYS, "Restore SecureBootMode fail\n"));
           }
          return Status;
      }

      CopyMem (AuditVarData, &mSecureBootState[NewMode].AuditMode, sizeof(UINT8));
#endif
      //
      // Fall into SetupMode logic
      //
    case SecureBootModeTypeSetupMode:
      //
      // Since PK is deleted before , can't rollback, still update SecureBootMode in memory
      //
      // UserMode ----> SetupMode
      //  Side Effects
      //    DeployedMode :=0 / SetupMode :=1 / SecureBoot :=0
      //
      // Update the value of SetupMode variable by a simple mem copy, this could avoid possible
      // variable storage reclaim at runtime.
      //
      CopyMem (SetupVarData, &mSecureBootState[NewMode].SetupMode, sizeof(UINT8));
      //
      // ECR1557: Clarification for Secure Boot Mode and SecureBoot variable state changes
      // SecureBoot Variable state state transition required. Only one transition may be queued at a time.
      // Further SetVariable() calls that attempt to initiate a Secure Boot mode transition are failed with EFI_ALREADY_STARTED
      //
#if defined(ECR1557)
      if (*(UINT8*)SecureBootVarData != mSecureBootState[NewMode].SecureBoot ) {
          if(mbAlreadyStarted == FALSE)
              mbAlreadyStarted = TRUE;
//            return EFI_WARN_RESET_REQUIRED;
          else
              return EFI_ALREADY_STARTED;
      }
#endif

      break;

    default:
      AVAR_TRACE((TRACE_ALWAYS, "Invalid state transition from %x to %x\n", SecureBootModeTypeUserMode, NewMode));
      ASSERT(FALSE);
  }

  return Status;
}

/**
  Current secure boot mode is SetupMode. This function performs secure boot mode transition
  to a new mode.

  @param[in] NewMode                New Secure Boot Mode.

  @retval EFI_SUCCESS               The initialization operation is successful.
  @retval EFI_OUT_OF_RESOURCES      There is not enough resource.

**/
EFI_STATUS
TransitionFromSetupMode(
  IN  SECURE_BOOT_MODE_TYPE              NewMode
  )
{
  EFI_STATUS   Status;
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
  VOID         *AuditVarData;
#endif
  VOID         *SetupVarData;
  VOID         *SecureBootVarData;
  UINTN        DataSize;

  //
  // AuditMode/DeployedMode/SetupMode/SecureBoot are all NON_NV variable maintained by Variable driver
  // they can be RW. but can't be deleted. so they can always be found.
  //
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT  
  Status = AuthServiceInternalFindVariable (
             EFI_AUDIT_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &AuditVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }
#endif
  Status = AuthServiceInternalFindVariable (
             EFI_SETUP_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &SetupVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }

  Status = AuthServiceInternalFindVariable (
             EFI_SECURE_BOOT_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &SecureBootVarData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    ASSERT(FALSE);
  }
  //
  // Make Secure Boot Mode transition ATOMIC
  // Update Private NV SecureBootMode Variable first, because it may fail due to NV range overflow.
  // Other transition logic are all memory operations and PK delete is assumed to be always successful.
  //
  Status = UpdateSecureBootMode(NewMode);
  if (EFI_ERROR(Status)) {
    //
    // UpdateSecureBootMode fails and no other variables are updated before. rollback this transition
    //
    return Status;
  }

  switch(NewMode) {
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
    case SecureBootModeTypeAuditMode:
      //
      // SetupMode ----> AuditMode
      // Side Effects
      //   AuditMode := 1
      //
      // Update the value of AuditMode variable by a simple mem copy, this could avoid possible
      // variable storage reclaim at runtime.
      //
      CopyMem (AuditVarData, &mSecureBootState[NewMode].AuditMode, sizeof(UINT8));
      break;
#endif
    case SecureBootModeTypeUserMode:
      //
      // Since PK is enrolled before, can't rollback, still update SecureBootMode in memory
      //
      // SetupMode ----> UserMode
      // Side Effects
      //   SetupMode := 0 / SecureBoot := 1
      //
      // Update the value of AuditMode variable by a simple mem copy, this could avoid possible
      // variable storage reclaim at runtime.
      //
      CopyMem (SetupVarData, &mSecureBootState[NewMode].SetupMode, sizeof(UINT8));
      //
      // ECR1557: Clarification for Secure Boot Mode and SecureBoot variable state changes
      // SecureBoot Variable state state transition required. Only one transition may be queued at a time.
      // Further SetVariable() calls that attempt to initiate a Secure Boot mode transition are failed with EFI_ALREADY_STARTED
      //
#if defined(ECR1557)
      if (*(UINT8*)SecureBootVarData != mSecureBootState[NewMode].SecureBoot ) {
          if(mbAlreadyStarted == FALSE)
              mbAlreadyStarted = TRUE;
//            return EFI_WARN_RESET_REQUIRED;
          else
              return EFI_ALREADY_STARTED;
      }
#endif
      break;

    default:
      AVAR_TRACE((TRACE_ALWAYS, "Invalid state transition from %x to %x\n", SecureBootModeTypeSetupMode, NewMode));
      ASSERT(FALSE);
  }

  return Status;
}

/**
  This function performs main secure boot mode transition logic.

  @param[in] CurMode                Current Secure Boot Mode.
  @param[in] NewMode                New Secure Boot Mode.

  @retval EFI_SUCCESS               The initialization operation is successful.
  @retval EFI_OUT_OF_RESOURCES      There is not enough resource.
  @retval EFI_INVALID_PARAMETER     The Current Secure Boot Mode is wrong.

**/
EFI_STATUS
SecureBootModeTransition(
  IN  SECURE_BOOT_MODE_TYPE  CurMode,
  IN  SECURE_BOOT_MODE_TYPE  NewMode
  )
{
  EFI_STATUS Status;

  AVAR_TRACE((TRACE_ALWAYS,"Transfer SecureBootMode %x ==> to SecureBootMode %x\n", CurMode, NewMode));

  //
  // SecureBootMode transition
  //
  switch (CurMode) {
    case SecureBootModeTypeUserMode:
      Status = TransitionFromUserMode(NewMode);
      break;

    case SecureBootModeTypeSetupMode:
      Status = TransitionFromSetupMode(NewMode);
      break;

    case SecureBootModeTypeAuditMode:
      Status = TransitionFromAuditMode(NewMode);
      break;

    case SecureBootModeTypeDeployedMode:
      Status = TransitionFromDeployedMode(NewMode);
      break;

    default:
      Status = EFI_INVALID_PARAMETER;
      ASSERT(FALSE);
  }

  return Status;
}

/**
  Determine whether this operation needs a physical present user.

  @param[in]      VariableName            Name of the Variable.
  @param[in]      VendorGuid              GUID of the Variable.

  @retval TRUE      This variable is protected, only a physical present user could set this variable.
  @retval FALSE     This variable is not protected.

**/
/*
BOOLEAN
NeedPhysicallyPresent(
  IN     CHAR16         *VariableName,
  IN     EFI_GUID       *VendorGuid
  )
{
// TBD    
  if ((CompareGuid (VendorGuid, &gSecureSetupGuid) && (StrCmp (VariableName, AMI_SECURE_BOOT_SETUP_VAR) == 0))) {
    return TRUE;
  }

  return FALSE;
}
*/
/**
  Determine whether the platform is operating in Custom Secure Boot mode.

  @retval TRUE           The platform is operating in Custom mode.
  @retval FALSE          The platform is operating in Standard mode.

**/
BOOLEAN
InCustomSecureBootMode (
  VOID
  )
{
  BOOLEAN bCustomMode = FALSE;

  if(IsNvramRuntime())
      return FALSE;

#if USER_MODE_POLICY_OVERRIDE == 1
  GetmSecureBootSetup();
  bCustomMode = (PhysicalUserPresent() && (mSecureBootSetup.SecureBootMode == CUSTOM_SECURE_BOOT))?1:0;
#endif
  AVAR_TRACE((TRACE_ALWAYS,"Custom Mode with Physical User %s\n",
           (bCustomMode?"detected - suppress Variable Authentication":"not detected")));

  return bCustomMode;
}

/**
  This function returns UNICODE string size in bytes 

  @param[in] CHAR16 *String - Pointer to string

  @retval UINTN - Size of string in bytes excluding the null-terminator

**/
UINTN
StrSize16(CHAR16 *String)
{
    UINTN Size = 0;//2;
    while(*String++)
        Size += 2;
    return Size;
}

/**
  This function compares two UNICODE strings 

  @param[in] CHAR16 *Dest - Pointer to destination string
  @param[in] CHAR16 *Src - Pointer to source string

  @retval INTN - Zero if strings are equal, non-zero otherwise

**/
INTN
StrCmp16(CHAR16 *Dest, CHAR16 *Src)
{
    return ((StrSize16(Src) != StrSize16(Dest)) || 
            MemCmp(Dest, Src, StrSize16(Src)));
}

/**
  This This function determines if variable is a PK database

  @param[in] CHAR16 *Dest - Pointer to destination string
  @param[in] EFI_GUID - Pointer to Variable GUID

  @retval BOOLEAN - TRUE if variable is PK

**/
BOOLEAN IsPkVar(
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid
)
{
    return ((guidcmp(VendorGuid, &gEfiGlobalVariableGuid) == 0) && 
            (StrCmp16 ((CHAR16 *)VariableName, (CHAR16 *)EFI_PLATFORM_KEY_NAME) == 0));
}

/**
  This This function determines if variable is a KEK database

  @param[in] CHAR16 *Dest - Pointer to destination string
  @param[in] EFI_GUID - Pointer to Variable GUID

  @retval BOOLEAN - TRUE if variable is KEK

**/
BOOLEAN
IsKekVar(
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid
)
{
    return ((guidcmp(VendorGuid, &gEfiGlobalVariableGuid) == 0) && 
            (StrCmp16 ((CHAR16 *)VariableName, (CHAR16 *)EFI_KEY_EXCHANGE_KEY_NAME) == 0));
}    

/**
  This function determines if variable is a VendorKeys database

  @param[in] CHAR16 *Dest - Pointer to destination string
  @param[in] EFI_GUID - Pointer to Variable GUID

  @retval BOOLEAN - TRUE if variable is VendorKeys

**/
BOOLEAN
IsVendorKeysVar(
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid
)
{
    return ((guidcmp(VendorGuid, &gEfiGlobalVariableGuid) == 0) && 
            (StrCmp16 ((CHAR16 *)VariableName, (CHAR16 *)EFI_VENDOR_KEYS_VARIABLE_NAME) == 0));
}

/**
  This function determines if variable needs to be treated as a Signature Database 

  @param[in] CHAR16 *Dest - Pointer to destination string
  @param[in] EFI_GUID - Pointer to Variable GUID

  @retval BOOLEAN - TRUE if variable is db/dbx/dbt...

**/
BOOLEAN
IsDbVar(
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid
)
{
    return ( (guidcmp(VendorGuid, &gEfiImageSecurityDatabaseGuid) == 0) );
}

/**
  This function determines if the variable is one of OsRecoveryOrder or OsRecoveryXXXX list

  @param[in] CHAR16 *Dest - Pointer to destination string
  @param[in] EFI_GUID - Pointer to Variable GUID

  @retval BOOLEAN - TRUE if variable is a match

**/
BOOLEAN IsDbrVar(
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid
)
{
    CHAR16 OsRecoveryNameBuffer[30];
    UINTN Index = 0;
    BOOLEAN IsOsRecoveryXXXX = FALSE;

// OsRecoveryOrder    
    if(guidcmp(VendorGuid, &gEfiGlobalVariableGuid) == 0 && 
       StrCmp16 ((CHAR16 *)VariableName, (CHAR16 *)EFI_OS_RECOVERY_ORDER_VARIABLE_NAME) == 0)
     return TRUE;
//OsRecoveryXXXX
    IsOsRecoveryXXXX = (StrCmp16(VariableName, EFI_OS_RECOVERY_XXXX_VARIABLE_NAME) == 0);
    // convert XXXX to integer and iterate 0 - FFFF to verify if one of OsRecoveryXXXX    
    while(IsOsRecoveryXXXX && Index<=0xFFFF) {
        Swprintf_s(OsRecoveryNameBuffer, 30, L"%s%d", EFI_OS_RECOVERY_XXXX_VARIABLE_NAME, Index);
        if(StrCmp16 ((CHAR16 *)VariableName, (CHAR16 *)&OsRecoveryNameBuffer) == 0)
            return TRUE;
        Index++;
    }
    return FALSE;
}

/**
  This function determines if variable is either AuditMode or DeployedMode 

  @param[in] CHAR16 *Dest - Pointer to destination string
  @param[in] EFI_GUID - Pointer to Variable GUID

  @retval BOOLEAN - TRUE if variable is a match

**/
BOOLEAN
IsDeploymentModeVar(
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid
)
{
    return ((guidcmp(VendorGuid, &gEfiGlobalVariableGuid) == 0) && 
            ((StrCmp16 ((CHAR16 *)VariableName, (CHAR16 *)EFI_AUDIT_MODE_NAME) == 0) ||
             (StrCmp16 ((CHAR16 *)VariableName, (CHAR16 *)EFI_DEPLOYED_MODE_NAME) == 0)) );
}

/**
  Check if Variable name matches one from Predefined Read-Only Variable Name list
        
  @param[in]   CHAR16 *VariableName Pointer to UNICODE Variable Name 
  @param[in]   EFI_GUID *VendorGuid Variable GUID
  
  @retval BOOLEAN TRUE if Variable name found in the Predefined Read-Only Variable Name lists

**/
BOOLEAN
IsReservedVariableName(
    CHAR16 *VariableName,
    EFI_GUID *VendorGuid
)
{
    UINT8   Index;
    Index = 0;

    while(!IsNvramRuntime() && ReservedReadOnlyVarNameList[Index] != NULL)
    {
        if(StrCmp16 (VariableName, ReservedReadOnlyVarNameList[Index]) == 0)
        {
// Reserved Secure Boot RO variables are gEfiGlobalVariableGuid.
            return (guidcmp(VendorGuid, &gEfiGlobalVariableGuid) == 0);
        }
        Index++;
    }

    return FALSE;
}

/**
  return Pk Present status
        
  @param[in]   none
  
  @param[out]  IsPkPresent

**/
BOOLEAN
GetPkPresent (
    VOID
)
{
    EFI_STATUS  Status;
    UINT32      VarAttr;
    UINTN       VarDataSize;
    VOID        *Data;
    BOOLEAN     IsPkPresent;

    //
    // Check PK database's existence to determine the value.
    //
    IsPkPresent = 0;
    Status = FindVariable ( EFI_PLATFORM_KEY_NAME, &gEfiGlobalVariableGuid, &VarAttr, &VarDataSize, &Data);
    if (!EFI_ERROR (Status) && (VarAttr & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES) ) {
        IsPkPresent = 1;
    }
    AVAR_TRACE((TRACE_ALWAYS,"IsPkPresent %s\n", (IsPkPresent == 1)?"Yes":"No"));

    return IsPkPresent;
}

/**
  Initializes mSecureBootSetup structure
        
  @param[in]   none
  
  @retval EFI_STATUS Function successfully executed.
  @retval EFI_OUT_OF_RESOURCES      There is not enough resource.

**/
EFI_STATUS
GetmSecureBootSetup (
    VOID
)
{
    EFI_STATUS Status;
    UINTN      DataSize;
    
    if(IsNvramRuntime())
        return EFI_SUCCESS;
    //
    // Get Setup variable, check SecureBoot and set the EFI Var
    //
    DataSize = sizeof(SECURE_BOOT_SETUP_VAR);
    Status = DxeGetVariable(
        AMI_SECURE_BOOT_SETUP_VAR,
        &gSecureSetupGuid,
        NULL,
        &DataSize,
        &mSecureBootSetup
    );
    if(Status == EFI_BUFFER_TOO_SMALL)
        Status = DxeGetVariable(
            AMI_SECURE_BOOT_SETUP_VAR,
            &gSecureSetupGuid,
            NULL,
            &DataSize,
            &mSecureBootSetup
        );
    ASSERT_EFI_ERROR (Status);

    return Status;
}

/**
  Update "VendorKeys" variable to record the out of band secure boot key modification.

  @return EFI_SUCCESS           Variable is updated successfully.
  @return Others                Failed to update variable.

**/
EFI_STATUS
VendorKeyIsModified (
  VOID
)
{
  EFI_STATUS    Status;
  VOID          *VarData;
  UINTN         VarDataSize;

  AVAR_TRACE((TRACE_ALWAYS,"VendorKey Is Modified %x\n", mVendorKeyState));

  if (mVendorKeyState == VENDOR_KEYS_MODIFIED) {
    return EFI_SUCCESS;
  }

  mVendorKeyState = VENDOR_KEYS_MODIFIED;

  Status = AuthServiceInternalUpdateVariable (
             EFI_VENDOR_KEYS_NV_VARIABLE_NAME,
             &gAmiVendorKeysNvGuid,
             &mVendorKeyState,
             sizeof (UINT8),
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS
             );

  AVAR_TRACE((TRACE_ALWAYS,"Upd %S = %d, %r\n", EFI_VENDOR_KEYS_NV_VARIABLE_NAME, mVendorKeyState, Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }
  // Update EFI_VENDOR_KEYS_NAME global variable
  //
  // all NON_NV variable maintained by Variable driver
  // they can be RW. but can't be deleted. so they can always be found.
  //
  Status = AuthServiceInternalFindVariable (
             EFI_VENDOR_KEYS_VARIABLE_NAME,
             &gEfiGlobalVariableGuid,
             &VarData,
             &VarDataSize
             );
  if (EFI_ERROR(Status)) {
  // first time
    Status =   AuthServiceInternalUpdateVariable (
               EFI_VENDOR_KEYS_VARIABLE_NAME,
               &gEfiGlobalVariableGuid,
               &mVendorKeyState,
               sizeof (UINT8),
               EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS
               );
  } else {
  // Update the value of AuditMode variable by a simple mem copy, this could avoid possible
  // variable storage reclaim at runtime.
  //
      CopyMem (VarData, &mVendorKeyState, sizeof(UINT8));
      Status = EFI_SUCCESS;
  }  

  return Status;
}

/**
  Update Secure Boot Status mode after updating the PK variable 
  
  @return EFI_INVALID_PARAMETER           Invalid parameter.
  @return EFI_SECURITY_VIOLATION          The variable does NOT pass the validation.
                                          check carried out by the firmware.
  @return EFI_SUCCESS                     Variable passed validation successfully.

**/
EFI_STATUS
AfterPkVarProcess ()
{
  EFI_STATUS Status;
  BOOLEAN    IsPkPresent;


  Status = EFI_SUCCESS;
  //
  // Find "PK" variable
  //
  IsPkPresent = GetPkPresent();
  //
  // Delete or Enroll PK causes SecureBootMode change
  //
  // If delete PK in user mode -> change to setup mode.
  // If enroll PK in setup mode -> change to user mode.
  //
  //
  if (IsPkPresent) { // enrolled
    if (mSecureBootMode == SecureBootModeTypeSetupMode) {
        //
        // If enroll PK in setup mode,  change to user mode.
        //
        Status = SecureBootModeTransition (mSecureBootMode, SecureBootModeTypeUserMode);
    } else if (mSecureBootMode == SecureBootModeTypeAuditMode) {
        //
        // If enroll PK in Audit mode,  change to Deployed mode.
        //
        Status = SecureBootModeTransition (mSecureBootMode, SecureBootModeTypeDeployedMode);
    } else {
        AVAR_TRACE((TRACE_ALWAYS,"PK is updated in %x mode. No SecureBootMode change.\n", mSecureBootMode));
    }
  } else {
    if ((mSecureBootMode == SecureBootModeTypeUserMode) || (mSecureBootMode == SecureBootModeTypeDeployedMode)) {
        //
        // If delete PK in User Mode or DeployedMode,  change to Setup Mode.
        //
        Status = SecureBootModeTransition (mSecureBootMode, SecureBootModeTypeSetupMode);
    }
  }

  return Status;
}

/**
  NVRAM SetVariable hook to update AuditMode\DeployedMode Vars
  
  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode, and datasize and data are external input.
  This function will do basic validation, before parse the data.
  This function will parse the authentication carefully to avoid security issues, like
  buffer overflow, integer overflow.
  This function will check attribute carefully to avoid authentication bypass.

  @param[in]  VariableName                Name of Variable to be found.
  @param[in]  VendorGuid                  Variable vendor GUID.
  @param[in]  Attributes                  Attribute value of the variable
  @param[in]  Data                        Data pointer.
  @param[in]  DataSize                    Size of Data found. If size is less than the
                                          data, this value contains the required size.

  @return EFI_INVALID_PARAMETER           Invalid parameter
  @return EFI_SECURITY_VIOLATION          The variable does NOT pass the validation
                                          check carried out by the firmware.
  @return EFI_WRITE_PROTECTED             Variable is Read-Only.
  @return EFI_SUCCESS                     Variable passed validation successfully.

**/
#if CUSTOMIZED_SECURE_BOOT_DEPLOYMENT
EFI_STATUS
SetDeploymentModeVarHook ( 
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
  )
{
  EFI_STATUS    Status;
  VOID          *VarData;
  UINTN         VarDataSize;

  // not one of our Vars
  if(!IsDeploymentModeVar(VariableName, VendorGuid))
    return EFI_UNSUPPORTED;

  AVAR_TRACE((TRACE_ALWAYS,"Set %S Data[0]=%0X, Size=0x%X, Attr 0x%X, Curr. mSecureBootMode %X\n", VariableName, *(UINT8*)Data, DataSize, Attributes, mSecureBootMode));

  if (IsNvramRuntime()) {
    return EFI_WRITE_PROTECTED;
  }

  //
  // Delete not OK
  //
  if ((DataSize != sizeof(UINT8)) || (Attributes == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  if (StrCmp (VariableName, EFI_AUDIT_MODE_NAME) == 0) {
    if(mSecureBootState[mSecureBootMode].IsAuditModeRO) {
      return EFI_WRITE_PROTECTED;
    }
  } else {
    //
    // Platform specific deployedMode clear. Set DeployedMode = RW
    //
    if (!InCustomSecureBootMode() || mSecureBootMode != SecureBootModeTypeDeployedMode) {
      if(mSecureBootState[mSecureBootMode].IsDeployedModeRO) {
        return EFI_WRITE_PROTECTED;
      }
    }
  }

  if (*(UINT8 *)Data != 0 && *(UINT8 *)Data != 1) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // AuditMode/DeployedMode/SetupMode/SecureBoot are all NON_NV variable maintained by Variable driver
  // they can be RW. but can't be deleted. so they can always be found.
  //
  Status = AuthServiceInternalFindVariable (
             VariableName,
             VendorGuid,
             &VarData,
             &VarDataSize
             );
  if (EFI_ERROR(Status)) {
    ASSERT(FALSE);
  }

  //
  // If AuditMode/DeployedMode is assigned same value. Simply return EFI_SUCCESS
  //
  if (*(UINT8 *)VarData == *(UINT8 *)Data) {
    return EFI_SUCCESS;
  }

  //
  // Perform SecureBootMode transition
  //
  if (StrCmp (VariableName, EFI_AUDIT_MODE_NAME) == 0) {
    return SecureBootModeTransition(mSecureBootMode, SecureBootModeTypeAuditMode);
  } else if (StrCmp (VariableName, EFI_DEPLOYED_MODE_NAME) == 0) {
    if (mSecureBootMode == SecureBootModeTypeDeployedMode) {
      //
      // Platform specific DeployedMode clear. InCustomSecureBootMode() is checked before
      //
      return SecureBootModeTransition(mSecureBootMode, SecureBootModeTypeUserMode);
    } else {
      return SecureBootModeTransition(mSecureBootMode, SecureBootModeTypeDeployedMode);
    }
  }

  return EFI_INVALID_PARAMETER;
}
#endif
/**
  NVRAM SetVariable hook to update Secure Boot policy variables

  Caution: This function may receive untrusted input.
  This function may be invoked in SMM mode, and datasize and data are external input.
  This function will do basic validation, before parse the data.
  This function will parse the authentication carefully to avoid security issues, like
  buffer overflow, integer overflow.
  This function will check attribute carefully to avoid authentication bypass.

  @param[in]  VariableName                Name of the variable.
  @param[in]  VendorGuid                  Variable vendor GUID.
  @param[in]  Data                        Data pointer.
  @param[in]  DataSize                    Size of Data.
  @param[in]  Attributes                  Attribute value of the variable.

  @return EFI_INVALID_PARAMETER           Invalid parameter.
  @return EFI_WRITE_PROTECTED             Variable is write-protected and needs authentication with
                                          EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  @return EFI_OUT_OF_RESOURCES            The Database to save the public key is full.
  @return EFI_SECURITY_VIOLATION          The variable is with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                          set, but the AuthInfo does NOT pass the validation
                                          check carried out by the firmware.
  @return EFI_SUCCESS                     Variable is not write-protected or pass validation successfully.

**/
EFI_STATUS
SetSecureBootVariablesHook ( 
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
  )
{
  EFI_STATUS    Status;
  UINT8         AuthVarType;

  if (IsDbVar(VariableName, VendorGuid))
      AuthVarType = IsDbVarType;
  else
      if (IsPkVar(VariableName, VendorGuid))
          AuthVarType = IsPkVarType;
      else
          if (IsKekVar(VariableName, VendorGuid))
              AuthVarType = IsKekVarType;
          else
              if (IsDbrVar(VariableName, VendorGuid))
                  AuthVarType = IsDbrVarType;
              else 
                 return EFI_UNSUPPORTED;

  AVAR_TRACE((TRACE_ALWAYS,"==>Set %S Data[0]=%0X, Size=0x%X, Attr 0x%X, Curr.mSecureBootMode %X\n", VariableName, *(UINT8*)Data, DataSize, Attributes, mSecureBootMode));

  Status = AuthServiceInternalUpdateVariable(
          VariableName, VendorGuid, Data, DataSize, Attributes);

  // Update VendorKeys. Only if in UserMode and CustomMode
  if (!EFI_ERROR(Status)) {
      if (InCustomSecureBootMode() && 
         ((mSecureBootMode != SecureBootModeTypeSetupMode) && (mSecureBootMode != SecureBootModeTypeAuditMode)) ){

          Status = VendorKeyIsModified ();
      }
  }
  if (!EFI_ERROR(Status)) {

      if( AuthVarType == IsPkVarType)
          Status = AfterPkVarProcess ();

  }

  AVAR_TRACE((TRACE_ALWAYS,"<==Set %S %r\n", VariableName, Status));

  return Status;
}

/**
  NVRAM SetVariablee hook to update SecureBootSetup Var

  @return EFI_UNSUPPORTED                 Not THE variable.
  @return EFI_WRITE_PROTECTED             Variable is write-protected.
 
**/
EFI_STATUS
SetSecureBootSetupVarHook ( 
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
  )
{
  if( (guidcmp(VendorGuid, &gSecureSetupGuid) != 0) || 
      (StrCmp16 ((CHAR16 *)VariableName, (CHAR16 *)AMI_SECURE_BOOT_SETUP_VAR) != 0) )
      return EFI_UNSUPPORTED;

  AVAR_TRACE((TRACE_ALWAYS,"Set %S, Attributes 0x%X\n", VariableName, Attributes));

//Block updates from Runtiume and from outside of Setup - no physical user detected
  if (IsNvramRuntime() || !PhysicalUserPresent())
      return EFI_WRITE_PROTECTED;

  return EFI_UNSUPPORTED;
}

/**
  NVRAM SetVariablee hook to update DeploymentModeNV and VendorKeyNV Var

  @return EFI_UNSUPPORTED                 Not THE variable.
  @return EFI_WRITE_PROTECTED             Variable is write-protected.
  
**/
EFI_STATUS
SetAuthServicesNVVarHook ( 
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
  )
{
  if (((guidcmp(VendorGuid, &gAmiDeploymentModeNvGuid) == 0) && (StrCmp16 ((CHAR16 *)VariableName, (CHAR16 *)AMI_DEPLOYMENT_MODE_VARIABLE_NAME) == 0)) ||
      ((guidcmp(VendorGuid, &gAmiVendorKeysNvGuid) == 0)     && (StrCmp16 ((CHAR16 *)VariableName, (CHAR16 *)EFI_VENDOR_KEYS_NV_VARIABLE_NAME)  == 0)) )
  {
      AVAR_TRACE((TRACE_ALWAYS,"Set %S, DataSize %X, Data[0]=%X\nWRITE_PROTECTED\n", VariableName, DataSize, *(UINT8*)Data ));
      return EFI_WRITE_PROTECTED;
  }

  return EFI_UNSUPPORTED;
}

/**
  Validate the data payload begins with valid Signature List header
  and based on the results returns Status.
        
  @param[in]   VOID *Data - pointer to the Var data
  @param[in]   UINTN DataSize - size of Var data
  @param[out]  *SigCount - Number of Signatures encountered inside the Data
  
  @return EFI_SECURITY_VIOLATION          The Signature does NOT pass the validation
                                          check carried out by the firmware.
  @return EFI_SUCCESS                     Data passed validation successfully.

**/
EFI_STATUS
ValidateSignatureList (
    IN VOID         *Data,
    IN UINTN        DataSize
)
{
    EFI_STATUS            Status;
    EFI_SIGNATURE_LIST   *SigList;
    UINTN                 Index;

    Status = EFI_SECURITY_VIOLATION;

    if(Data == NULL || DataSize == 0)
        return Status; // Sig not found

    SigList  = (EFI_SIGNATURE_LIST *)Data;

// loop till end of DataSize for all available SigLists

// Verify signature is one from SigDatabase list mSignatureSupport / sizeof(EFI_GUID)
// SigData begins with SigOwner GUID
// SignatureHdrSize = 0 for known Sig Types

    while ((DataSize > 0) && (DataSize >= SigList->SignatureListSize)) {

        for (Index = 0; Index < SIGSUPPORT_NUM; Index++) {
            if (!guidcmp ((EFI_GUID*) &(SigList->SignatureType), &mSignatureSupport[Index]))
                break;
        }
AVAR_TRACE((TRACE_ALWAYS,"SigList.Type-"));
        if(Index >= SIGSUPPORT_NUM)
           return EFI_SECURITY_VIOLATION; // Sig not found

AVAR_TRACE((TRACE_ALWAYS,"OK\nSigList.Size-"));
        if(SigList->SignatureListSize < 0x4c || // Min size for SHA2 Hash Certificate sig list
           SigList->SignatureListSize > NVRAM_SIZE)
            return EFI_SECURITY_VIOLATION; 

AVAR_TRACE((TRACE_ALWAYS,"OK\nSigList.HdrSize-"));
        if(SigList->SignatureHeaderSize != 0)
            return EFI_SECURITY_VIOLATION; // Sig not found
AVAR_TRACE((TRACE_ALWAYS,"OK\n"));
        DataSize -= SigList->SignatureListSize;
        SigList = (EFI_SIGNATURE_LIST *) ((UINT8 *) SigList + SigList->SignatureListSize);

        Status = EFI_SUCCESS;
    }

    return Status;
}

/**
   For variables with GUID EFI_IMAGE_SECURITY_DATABASE_GUID 
  (i.e. where the data buffer is formatted as EFI_SIGNATURE_LIST), 
   the driver shall not perform an append of EFI_SIGNATURE_DATA values 
   that are already part of the existing variable value 
   Note: This situation is not considered an error, and shall in itself not cause a status code other 
   than EFI_SUCCESS to be returned or the timestamp associated with the variable not to be updated

  @param[in]  VOID  *Data - pointer to data block within AutVar Data
  @param[in]  UINTN *DataSize - ptr to size of data block
  @param[in]  VOID  *SigDb - current SigDb
  @param[in]  UINTN  SigDbSize 

  @return EFI_ALREADY_STARTED           Signature is already present in current Signature Database
  @return EFI_SUCCESS                   New signature can be appended to a current signature list

**/
EFI_STATUS 
IsAppendToSignatureDb (
    IN VOID         *Data,
    IN OUT UINTN    *DataSize,
    IN VOID         *SigDb,
    IN UINTN        SigDbSize
){
    EFI_SIGNATURE_LIST             *SigList;
    EFI_SIGNATURE_LIST             *SigListNew;
    EFI_SIGNATURE_DATA             *SigItem;
    EFI_SIGNATURE_DATA             *SigItemNew;
    UINT32                          SigCount;
    UINT32                          Index;
    UINT32                          SigCountNew;
    UINT32                          IndexNew;
    UINTN                           SigNewSize;
    BOOLEAN                         bSigMatch;

    if (SigDb && SigDbSize && (Data != SigDb) &&
        // Validate Signature List integrity 
        !EFI_ERROR(ValidateSignatureList (Data, *DataSize))
    ) {
        SigList     = (EFI_SIGNATURE_LIST *)SigDb;
        SigListNew  = (EFI_SIGNATURE_LIST *)Data;
AVAR_TRACE((TRACE_ALWAYS,"FindInDB(x)\nDataSize In %d (0x%X)\n",*DataSize,*DataSize));
        //
        // Enumerate all Sig items in this list to verify the variable certificate data.
        //
        //
        // scan through multiple Sig Lists in DB exist.
        while ((SigDbSize > 0) && (SigDbSize >= SigList->SignatureListSize)) {
            SigCount  = (SigList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - SigList->SignatureHeaderSize) / SigList->SignatureSize;
            SigItem   = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigList + sizeof (EFI_SIGNATURE_LIST) + SigList->SignatureHeaderSize);
//AVAR_TRACE((TRACE_ALWAYS,"Org SigList Count: %d, SigSize %X\n", SigCount, SigList->SignatureSize));
            // scan through multiple Sig Lists in NewSigList.
            for (Index = 1; Index <= SigCount; Index++) {
// AVAR_TRACE((TRACE_ALWAYS,"OrgCert %d, Data %X\n",Index, *(UINT32*)SigItem->SignatureData));
                SigListNew  = (EFI_SIGNATURE_LIST *)Data;
                SigNewSize  = *DataSize;
                while ((SigNewSize > 0) && (SigNewSize >= SigListNew->SignatureListSize)) {
                    bSigMatch = FALSE;
                    SigItemNew = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigListNew + sizeof (EFI_SIGNATURE_LIST) + SigListNew->SignatureHeaderSize);
                    SigCountNew  = (SigListNew->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - SigListNew->SignatureHeaderSize) / SigListNew->SignatureSize;
                    if (!guidcmp ((EFI_GUID*) &(SigList->SignatureType), (EFI_GUID*)&(SigListNew->SignatureType)) && 
                        SigList->SignatureSize == SigListNew->SignatureSize) {
//AVAR_TRACE((TRACE_ALWAYS,"New SigDb Size %X\nNew SigList Count: %d, SigSize %X\n", SigNewSize, SigCountNew, SigListNew->SignatureSize));
                      // loop through all instances of NewSigList->SigData. 
                      for (IndexNew = 1; IndexNew <= SigCountNew; IndexNew++) {
// AVAR_TRACE((TRACE_ALWAYS,"NewCert %d, Data %X\n",IndexNew, *(UINT32*)SigItemNew->SignatureData));
                            if (MemCmp (SigItem->SignatureData, SigItemNew->SignatureData, SigList->SignatureSize-sizeof(EFI_GUID)) == 0) {
//AVAR_TRACE((TRACE_ALWAYS,"---> match found!!!\n"));
//AVAR_TRACE((TRACE_ALWAYS,"OrgCert %4d, Data %X\n",Index, *(UINT32*)SigItem->SignatureData));
//AVAR_TRACE((TRACE_ALWAYS,"NewCert %4d, Data %X\n",IndexNew, *(UINT32*)SigItemNew->SignatureData));
                               if(SigCountNew == 1) {
                                // only 1 SigData per SigList - discard this SigList
                                      bSigMatch = TRUE;
//AVAR_TRACE((TRACE_ALWAYS,"Before: DataSize=%x\nAfter : DataSize=%x\n", *DataSize, *DataSize-SigListNew->SignatureListSize));
                                      // 1. Decrease *Datasize by SigListNew->SignatureSize 
                                      SigNewSize -= SigListNew->SignatureListSize;
                                      *DataSize -= SigListNew->SignatureListSize;
                                      // 2. replace this SigData block with data following it
                                      MemCpy (SigListNew, (void*)((UINTN)SigListNew+SigListNew->SignatureListSize), SigNewSize); 
                                      // 3. Skip to next SigListNew    
                                      break;
                                } else {
                                // more then 1 - discard this SigData
                                    // 1. replace this SigData block with data following it
                                    MemCpy (SigItemNew, (void*)((UINTN)SigItemNew+SigListNew->SignatureSize), ((UINTN)Data+*DataSize)-((UINTN)SigItemNew+SigListNew->SignatureSize));
                                    // 2. Decrease SigListNew->SignatureListSize by SigListNew->SignatureSize 
                                    SigListNew->SignatureListSize-=SigListNew->SignatureSize;
                                    *DataSize-=SigListNew->SignatureSize;
//AVAR_TRACE((TRACE_ALWAYS,"Upd SignatureListSize=%x, DataSize=%x\n",SigListNew->SignatureListSize, *DataSize));
                                    // 3. If this is last SigData element
                                    if((SigListNew->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - SigListNew->SignatureHeaderSize)==0)
                                    {  
//AVAR_TRACE((TRACE_ALWAYS,"SigList is Empty!\n"));
                                        break;
                                    }         
                                    // 4. Skip incrementing of SigItemNew
                                    continue;
                                }
                            } // if cmp 
                            SigItemNew = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigItemNew + SigListNew->SignatureSize);
                        } // for SigItemNew 
                    } // if guid
                    // Skip incrementing of SigListNew if bSigListMatch is found - we already on next siglist
                    if(!bSigMatch) {
                        SigNewSize -= SigListNew->SignatureListSize;
                        SigListNew = (EFI_SIGNATURE_LIST *) ((UINT8 *) SigListNew + SigListNew->SignatureListSize);
                    }
                } // while SigListNew
                SigItem = (EFI_SIGNATURE_DATA *) ((UINT8 *) SigItem + SigList->SignatureSize);
            } // for SigItem
            SigDbSize -= SigList->SignatureListSize;
            SigList = (EFI_SIGNATURE_LIST *) ((UINT8 *) SigList + SigList->SignatureListSize);
        } // while SigList 

AVAR_TRACE((TRACE_ALWAYS,"DataSize Out: %d (0x%X)\n",*DataSize, *DataSize));
    if(*DataSize==0)
        return EFI_ALREADY_STARTED;

AVAR_TRACE((TRACE_ALWAYS,"APPEND OK!\n"));
    }

    return EFI_SUCCESS;
}

/**
  This function is called every time variable with 
  EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS and 
  EFI_VARIABLE_AUTHENTICATED_TIME_BASED_ACCESS attributes 
  created, updated or deleted. This function does all necessary 
  Authentication checks and based on the results returns Status.
  Also it returns the Mc Hash of PublicKey from Variable's AuthInfo Hdr

  @param[in]   IN CHAR16 *VariableName - pointer to Var Name in Unicode
  @param[in]   IN EFI_GUID *VendorGuid - pointer to Var GUID
  @param[in]   UINT32 Attributes - Attributes of the Var
  @param[in]   VOID **Data - pointer to the Var data
  @param[in]   UINTN *DataSize - size of Var data
  @param[in]   VOID  *OldData - pointer to Existing in NVRAM data block 
  @param[in]   UINTN  OldDataSize - size of data block
  @param[in]   UINT64 ExtFlags.MonotonicCount - value of MC or TIME stamp 
  @param[in]   UINT8  ExtFlags.KeyHash[32] - pointer to memory, allocated by caller, 
                      where Hash of Public Key will be returned.
  
  @retval EFI_SUCCESS               The update operation is success.
  @retval EFI_INVALID_PARAMETER     Invalid parameter.
  @retval EFI_WRITE_PROTECTED       Variable is write-protected.
  @retval EFI_OUT_OF_RESOURCES      There is not enough resource.

**/
EFI_STATUS
VerifyVariable (
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid,
    IN UINT32   *Attributes,
    IN VOID    **Data,
    IN UINTN    *DataSize,
    IN VOID     *OldData,
    IN UINTN     OldDataSize,
    IN OUT EXT_SEC_FLAGS *ExtSecFlags
){
    EFI_STATUS  Status;
    UINT32      AuthAttributes; 
// dbg
//    UINT8      *Byte = *(UINT8**)Data;
//    AVAR_TRACE((TRACE_ALWAYS,"Set Var '%S', Data[0]=%X(buf %X), Size=0x%X, Attr=%0X, OldData=%X, OldDataSize=%d\n",
//                VariableName, *Byte, *Data, *DataSize, *Attributes, OldData, OldDataSize));

    // existing reserved variables are RO!!!
    if(OldData && OldDataSize && IsReservedVariableName(VariableName, VendorGuid)) {

        return EFI_WRITE_PROTECTED;
    }

    AuthAttributes = ExtSecFlags->AuthFlags & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES;
    if ((*Attributes & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES)
            // Old Variable with no attributes can be erased after proper AuthHeader validation
            // EIP88439: irrespective of SetupMode, only properly formatted Auth Variable can be erased
            || (AuthAttributes) 
    ){
        // check if both attributes are set  
        if ((*Attributes & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES) == 
                           UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES
        )
          return EFI_INVALID_PARAMETER;

        // ignore Variable Authentication rules while in Custom mode and Physical User Presence
        if(*DataSize==0 || *Data==NULL) {
            if(InCustomSecureBootMode()) { //PhysicalUserPresent()
                return EFI_SUCCESS; 
            }
        }
//    Old - nonAuth, New - nonAuth - exit with EFI_SUCCESS
//    Old - nonAuth, New - Auth    - Continue with new Auth attr
//    Old - Auth,    New - nonAuth - if *Attribs=0 - Erase in progress if in SetupMode
//                                   else EFI_SECURITY_VIOLATION
//    Old - Auth,    New - Auth    - Continue if AuthAttr matching
//                                   else EFI_SECURITY_VIOLATION
// OldVar AuthAttributes mismatch
        if( AuthAttributes && *Attributes &&
          !(AuthAttributes & (*Attributes & UEFI23_1_AUTHENTICATED_VARIABLE_ATTRIBUTES))
        )
        // Attribute mismatch
          return EFI_SECURITY_VIOLATION;

        // else in process of erasing or Setting AuthVar

        AuthAttributes |= *Attributes;

        if(*DataSize==0 || *Data==NULL)
          return EFI_SECURITY_VIOLATION;

        if(mDigitalSigProtocol == NULL)
          return EFI_UNSUPPORTED;

//AVAR_TRACE((TRACE_ALWAYS,"Verify AuthVar: %S, Attr=%X, Data=%X, Size=%d, OldData=%X, OldDataSize=%d\n", VariableName, *Attributes, *Data, *DataSize, OldData, OldDataSize));
        if (AuthAttributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)
            Status = VerifyVariable2(VariableName, VendorGuid, AuthAttributes, Data, DataSize, OldData, OldDataSize, ExtSecFlags);
        else
            Status = VerifyVariable1(VariableName, VendorGuid, AuthAttributes, Data, DataSize, OldData, OldDataSize, ExtSecFlags);
//AVAR_TRACE((TRACE_ALWAYS, "Exit - %r, Data - %X, Size %d, Attributes %x\n",Status, *Data, *DataSize, *Attributes));
        return Status;
    }

    return EFI_SUCCESS; // variable not changed
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
