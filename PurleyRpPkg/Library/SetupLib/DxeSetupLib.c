/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
--*/
/** @file
  
  Library functions for SetupLib.
  This library instance provides methods to access Setup option.

  Copyright (c) 2012-2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <SetupLibInternal.h>
#include <Protocol/SmmBase2.h>

// APTIOV_SERVER_OVERRIDE_RC_START
INTEL_SETUP_DATA               gSetupData; // To solve redefinition error
// we should always get NVRAM variables from NV storage area
//BOOLEAN                      gFirstCall = TRUE;
// APTIOV_SERVER_OVERRIDE_RC_END
EFI_SMM_BASE2_PROTOCOL         *sSmmBase2 = NULL;
EFI_SMM_SYSTEM_TABLE2          *sSmst = NULL;
BOOLEAN                        sInSmm = FALSE;

/**
  Gets the data and size of a variable.

  Read the EFI variable (Guid/Name) and return a dynamically allocated
  buffer, and the size of the buffer. If failure return NULL.

  @param  Name                     String part of EFI variable name
  @param  Guid                     GUID part of EFI variable name
  @param  VariableSize             Returns the size of the EFI variable that was
                                   read

  @return Dynamically allocated memory that contains a copy of the EFI variable.
          Caller is responsible freeing the buffer.
  @retval NULL                     Variable was not read

**/
VOID *
EFIAPI
GetVariableAndSize (
  IN  CHAR16                  *Name,
  IN  UINT32                  *Attributes,
  IN  EFI_GUID                *Guid,
  IN OUT UINTN                BufferSize
  )
{
  EFI_STATUS                     Status;
  VOID                           *Buffer = NULL;

  if (sSmmBase2 == NULL || sInSmm == FALSE || sSmst == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID**) &sSmmBase2);
    if (Status == EFI_SUCCESS) {
      Status = sSmmBase2->InSmm (sSmmBase2, &sInSmm);
    }
  }

  if (!sInSmm) {
    Buffer = AllocateZeroPool (BufferSize);
    ASSERT(Buffer != NULL);
    if (Buffer == NULL) {
      return NULL;
    }
  } else {
    Status = sSmmBase2->GetSmstLocation (sSmmBase2, &sSmst);
    ASSERT_EFI_ERROR (Status);
    Status = sSmst->SmmAllocatePool (EfiRuntimeServicesData, BufferSize, (VOID **) &Buffer);
    ASSERT_EFI_ERROR (Status);
    if (Buffer == NULL) {
      return NULL;
    }
    SetMem (Buffer, BufferSize, 0);
  }

  //
  // Read variable into the allocated buffer.
  //
  Status = gRT->GetVariable (Name, Guid, Attributes, &BufferSize, Buffer);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    if (sInSmm) {
      sSmst->SmmFreePool (Buffer);
    } else {
      gBS->FreePool(Buffer);
    }
    return NULL;
  }
  return Buffer;
}

/**
  Constructor function initialized Setup Data with the default data.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS   The constructor executed correctly.

**/
EFI_STATUS
EFIAPI
DxeSetupLibConstructor (
  VOID
)
{
  EFI_STATUS  Status;
  VOID        *Variable = NULL;
  UINTN       Index;
  UINT8       *SetupDataPtr;
  
  if (sSmmBase2 == NULL || sInSmm == FALSE || sSmst == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID**) &sSmmBase2);
    if (Status == EFI_SUCCESS) {
      Status = sSmmBase2->InSmm (sSmmBase2, &sInSmm);
    }
    if (sInSmm) {
      Status = sSmmBase2->GetSmstLocation (sSmmBase2, &sSmst);
      ASSERT_EFI_ERROR (Status);
    }
  }

  SetupDataPtr = (UINT8 *)&gSetupData;
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  ZeroMem (SetupDataPtr, sizeof(INTEL_SETUP_DATA));
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error

  for (Index = 0; mSetupInfo[Index].GuidValue != NULL; Index ++) {
    if (!sInSmm) {
      Variable = AllocateZeroPool (mSetupInfo[Index].VariableSize);
      ASSERT(Variable != NULL);
      if (Variable == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
    } else {
      Status = sSmst->SmmAllocatePool (EfiRuntimeServicesData, mSetupInfo[Index].VariableSize, (VOID **) &Variable);
      ASSERT_EFI_ERROR (Status);
      if (Variable == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      SetMem (Variable, mSetupInfo[Index].VariableSize, 0);
    }
    Status = gRT->GetVariable(mSetupInfo[Index].SetupName, mSetupInfo[Index].GuidValue, NULL, &mSetupInfo[Index].VariableSize, Variable); 
    ASSERT_EFI_ERROR (Status);
    CopyMem (SetupDataPtr, Variable, mSetupInfo[Index].VariableSize);
    if (sInSmm) {
      sSmst->SmmFreePool (Variable);
    } else {
      gBS->FreePool(Variable);
    }
    SetupDataPtr = SetupDataPtr + mSetupInfo[Index].VariableSize;
  }
  return EFI_SUCCESS;
}

/**
  Retrieves the specified group space.

  @param[in]  Guid             Pointer to a 128-bit unique value that designates which namespace 
                               to set a value from.  
  @return GroupIndex           Returns Index of variable, MAX_ADDRESS will return if not found.
**/
UINTN
InternalGetGroupInfo (
  IN EFI_GUID   *Guid
  )
{
  UINTN      Index;

  if (Guid == NULL) {
    return MAX_ADDRESS;
  }
  
  //
  // Find the matched GUID space.
  //
  for (Index = 0; mSetupInfo[Index].GuidValue != NULL; Index ++) {
    if (CompareGuid (mSetupInfo[Index].GuidValue, Guid)) {
      break;
    }
  }

  //
  // No matched GUID space
  //
  if (mSetupInfo[Index].GuidValue == NULL) {
    return MAX_ADDRESS;
  }
  return Index;
}

/**
  This function provides a means by which to retrieve a value for a given option.
  
  Returns the data, data type and data size specified by OptionNumber and Guid.

  @param[in]  Guid          Pointer to a 128-bit unique value that designates 
                            which namespace to retrieve a value from.
  @param[in]  OptionNumber  The option number to retrieve a current value for.
  @param[in, out] Data      A pointer to the buffer to be retrieved.
  @param[in, out] DataSize  The size, in bytes, of Buffer.

  @retval EFI_SUCCESS           Data is successfully reterieved.
  @retval EFI_INVALID_PARAMETER Guid is NULL or DataSize is NULL or OptionNumber is invalid.
  @retval EFI_BUFFER_TOO_SMALL  Input data buffer is not enough.
  @retval EFI_NOT_FOUND         The given option is not found.

**/
EFI_STATUS
EFIAPI
GetOptionData (
  IN EFI_GUID            *Guid, 
  IN UINTN               OptionNumber, 
  IN OUT VOID            *Data, 
  IN UINTN               DataSize
  )
{
  UINTN       GroupIndex;
  VOID        *Variable = NULL;

  if (Guid == NULL || DataSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  GroupIndex = InternalGetGroupInfo (Guid);
  if (GroupIndex == MAX_ADDRESS) {
    return EFI_NOT_FOUND;
  }
  if (OptionNumber > mSetupInfo[GroupIndex].VariableSize) {
    return EFI_INVALID_PARAMETER;
  }
  Variable = GetVariableAndSize(mSetupInfo[GroupIndex].SetupName, NULL, mSetupInfo[GroupIndex].GuidValue, mSetupInfo[GroupIndex].VariableSize);
  ASSERT(Variable != NULL);
  if (Variable == NULL) {
      return EFI_NOT_FOUND;
  }
  CopyMem(Data, (UINT8 *)Variable + OptionNumber, DataSize); 
  if (sInSmm) {
    sSmst->SmmFreePool (Variable);
  } else {
    gBS->FreePool(Variable);
  }
  return EFI_SUCCESS;
}

/**
  This function provides a means by which to set a value for a given option number.
  
  Sets a buffer for the token specified by OptionNumber to the value specified by 
  Data and DataSize.  
  If DataSize is greater than the maximum size support by OptionNumber, 
  then set DataSize to the maximum size supported by OptionNumber.
  
  @param[in]  Guid              Pointer to a 128-bit unique value that 
                                designates which namespace to set a value from.
  @param[in]  OptionNumber      The option number to set a current value for.
  @param[in]  Data              A pointer to the buffer to set.
  @param[in, out] DataSize      The size, in bytes, of Buffer.
  
  @retval EFI_SUCCESS           Data is successfully updated.
  @retval EFI_INVALID_PARAMETER OptionNumber is invalid, Guid is NULL, or Data is NULL, or DataSize is NULL.
  @retval EFI_NOT_FOUND         The given option is not found.
  @retval EFI_UNSUPPORTED       Set action is not supported.
**/
EFI_STATUS
EFIAPI
SetOptionData (
  IN EFI_GUID *Guid, 
  IN UINTN    OptionNumber, 
  IN VOID     *Data, 
  IN UINTN    DataSize
  )
{
  EFI_STATUS  Status;
  UINTN       GroupIndex;
  VOID        *Variable = NULL;
  UINT32      Attributes;

  if (Guid == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  GroupIndex = InternalGetGroupInfo (Guid);
  if (GroupIndex == MAX_ADDRESS) {
    return EFI_NOT_FOUND;
  }

  if (OptionNumber > mSetupInfo[GroupIndex].VariableSize) {
    return EFI_INVALID_PARAMETER;
  }
  Variable = GetVariableAndSize(mSetupInfo[GroupIndex].SetupName, &Attributes, mSetupInfo[GroupIndex].GuidValue, mSetupInfo[GroupIndex].VariableSize); 
  ASSERT(Variable != NULL);
  if (Variable == NULL) {
      return EFI_NOT_FOUND;
  }
  CopyMem ((UINT8*)Variable + OptionNumber, Data, DataSize);
  Status = gRT->SetVariable (mSetupInfo[GroupIndex].SetupName, Guid, Attributes, mSetupInfo[GroupIndex].VariableSize, Variable);
  ASSERT_EFI_ERROR (Status);

  if (sInSmm) {
    sSmst->SmmFreePool (Variable);
  } else {
    gBS->FreePool(Variable);
  }
  return EFI_SUCCESS;
}

/**
  Get all data in the setup

  @retval EFI_SUCCESS           Data is committed successfully.
  @retval EFI_INVALID_PARAMETER Guid is NULL.
  @retval EFI_NOT_FOUND         Guid is not found.
  @retval EFI_DEVICE_ERROR      Data can't be committed.
**/
EFI_STATUS
EFIAPI
GetEntireConfig (
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  OUT INTEL_SETUP_DATA *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  if (SetupData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // APTIOV_SERVER_OVERRIDE_RC_START : we should always get NVRAM variables from NV storage area
  //if (gFirstCall == TRUE) {
  Status = DxeSetupLibConstructor ();
  if (EFI_ERROR(Status)) {
    return Status;
  }
  //gFirstCall = FALSE; 
  //}
  CopyMem (SetupData, &gSetupData, sizeof(INTEL_SETUP_DATA)); // To solve redefinition error
  // APTIOV_SERVER_OVERRIDE_RC_END : we should always get NVRAM variables from NV storage area
  
  return EFI_SUCCESS;
}


/**
  Set all data in the setup

  @retval EFI_SUCCESS           Data is committed successfully.
  @retval EFI_INVALID_PARAMETER Guid is NULL.
  @retval EFI_NOT_FOUND         Guid is not found.
  @retval EFI_DEVICE_ERROR      Data can't be committed.
**/
EFI_STATUS
EFIAPI
SetEntireConfig (
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  IN INTEL_SETUP_DATA *SetupData
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  )
{
  EFI_STATUS  Status;
  VOID        *Variable = NULL;
  UINTN       Index;
  UINT32      Attributes;
  UINT8       *SetupDataPtr;

  if (SetupData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // APTIOV_SERVER_OVERRIDE_RC_START : we should always get NVRAM variables from NV storage area
  //if (gFirstCall == TRUE) {
    //Status = DxeSetupLibConstructor ();
    //gFirstCall = FALSE;
  //}
  // APTIOV_SERVER_OVERRIDE_RC_END : we should always get NVRAM variables from NV storage area
  SetupDataPtr = (UINT8 *)&gSetupData;
  // APTIOV_SERVER_OVERRIDE_RC_START : To solve redefinition error
  CopyMem (SetupDataPtr, SetupData, sizeof(INTEL_SETUP_DATA));
  // APTIOV_SERVER_OVERRIDE_RC_END : To solve redefinition error
  for (Index = 0; mSetupInfo[Index].GuidValue != NULL; Index ++) {
    Variable = GetVariableAndSize(mSetupInfo[Index].SetupName, &Attributes, mSetupInfo[Index].GuidValue, mSetupInfo[Index].VariableSize);
    ASSERT(Variable != NULL);
    if (Variable == NULL) {
      return EFI_NOT_FOUND;
    }
    CopyMem ((UINT8 *)Variable, SetupDataPtr, mSetupInfo[Index].VariableSize);
    Status = gRT->SetVariable (mSetupInfo[Index].SetupName, mSetupInfo[Index].GuidValue, Attributes, mSetupInfo[Index].VariableSize, Variable);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) {
      return Status;
    }
    if (sInSmm) {
      sSmst->SmmFreePool (Variable);
    } else {
      gBS->FreePool(Variable);
    }
    SetupDataPtr = SetupDataPtr + mSetupInfo[Index].VariableSize;
  }

  return EFI_SUCCESS;
}

/**
  This function provides a means by which to retrieve a value for a given option.
  
  Returns the data, data type and data size specified by OptionNumber and Guid.

  @param[in]  Guid          Pointer to a 128-bit unique value that designates 
                            which namespace to retrieve a value from.
  @param[in]  Variable      Pointer to data location where variable is stored.

  @retval EFI_SUCCESS           Data is successfully reterieved.
  @retval EFI_INVALID_PARAMETER Guid or Variable is null.
  @retval EFI_NOT_FOUND         The given option is not found.

**/
EFI_STATUS
EFIAPI
GetSpecificConfigGuid (
  IN EFI_GUID            *Guid, 
  OUT VOID               *Variable
  )
{
  EFI_STATUS  Status;
  UINTN       GroupIndex;

  if ((Guid == NULL) || (Variable == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  GroupIndex = InternalGetGroupInfo (Guid);
  if (GroupIndex == MAX_ADDRESS) {
    return EFI_NOT_FOUND;
  }
  Status = gRT->GetVariable (mSetupInfo[GroupIndex].SetupName, mSetupInfo[GroupIndex].GuidValue, NULL, &mSetupInfo[GroupIndex].VariableSize, Variable);

  return Status;
}

/**
  This function provides a means by which to set a value for a given option number.
  
  Sets a buffer for the token specified by OptionNumber to the value specified by 
  Data and DataSize.  
  If DataSize is greater than the maximum size support by OptionNumber, 
  then set DataSize to the maximum size supported by OptionNumber.
  
  @param[in]  Guid              Pointer to a 128-bit unique value that 
                                designates which namespace to set a value from.
  @param[in]  Variable      Pointer to data location where variable is stored.

  @retval EFI_SUCCESS           Data is successfully updated.
  @retval EFI_INVALID_PARAMETER OptionNumber is invalid, Guid is NULL, or Data is NULL, or DataSize is NULL.
  @retval EFI_NOT_FOUND         The given option is not found.
  @retval EFI_UNSUPPORTED       Set action is not supported.
**/
EFI_STATUS
EFIAPI
SetSpecificConfigGuid (
  IN EFI_GUID *Guid, 
  IN VOID *Variable 
  )
{
  EFI_STATUS  Status;
  UINTN       GroupIndex;
  VOID        *DummyVariable = NULL;
  UINT32      Attributes;

  if ((Guid == NULL) || (Variable == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  GroupIndex = InternalGetGroupInfo (Guid);
  if (GroupIndex == MAX_ADDRESS) {
    return EFI_NOT_FOUND;
  }

  DummyVariable = GetVariableAndSize(mSetupInfo[GroupIndex].SetupName, &Attributes, Guid, mSetupInfo[GroupIndex].VariableSize);
  ASSERT(DummyVariable != NULL);
  if (DummyVariable == NULL) {
      return EFI_NOT_FOUND;
  }
  Status = gRT->SetVariable (mSetupInfo[GroupIndex].SetupName, Guid, Attributes, mSetupInfo[GroupIndex].VariableSize, Variable);
  ASSERT_EFI_ERROR (Status);

  if (sInSmm) {
    sSmst->SmmFreePool (DummyVariable);
  } else {
    gBS->FreePool(DummyVariable);
  }
  return EFI_SUCCESS;
}

