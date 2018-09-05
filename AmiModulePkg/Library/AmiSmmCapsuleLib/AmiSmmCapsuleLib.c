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
  AmiSmmCapsuleLib library instance
**/
#include <Protocol/SmmVariable.h>
#include <Guid/CapsuleVendor.h>
#include <Library/BaseLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PrintLib.h>

EFI_SMM_VARIABLE_PROTOCOL *AmiSmmCapsuleLibSmmVariableInterface = NULL;

EFI_STATUS AmiSmmCapsuleLibBuildCapsuleVariableName(CHAR16* CapsuleVarName, UINTN MaxSize)
{
    CHAR16 *TempVarName;  
    UINTN Size;
    UINTN Index;
    EFI_STATUS Status;

    if (MaxSize <= sizeof(CHAR16)) return EFI_INVALID_PARAMETER;
    if (AmiSmmCapsuleLibSmmVariableInterface == NULL) return EFI_INVALID_PARAMETER;

    // Construct variable name CapsuleUpdateData, CapsuleUpdateData1, CapsuleUpdateData2...
    // if user calls AmiScheduleCapsule multiple times.
    StrCpyS (CapsuleVarName, MaxSize, EFI_CAPSULE_VARIABLE_NAME);
    TempVarName = CapsuleVarName + StrLen (CapsuleVarName);
    Index = 1;
    do{
        Size = 0;
        Status = AmiSmmCapsuleLibSmmVariableInterface->SmmGetVariable(
            CapsuleVarName, &gEfiCapsuleVendorGuid, NULL, &Size, NULL
        );
        if (Status==EFI_BUFFER_TOO_SMALL){
            UnicodeValueToString (TempVarName, 0, Index, 0);
            Index++;
            continue;
        }
        return EFI_SUCCESS;
    }while(TRUE);
}


/**
  Schedules capsule for processing after system reset.

  @param  ScatterGatherList     Physical pointer to a set of
                                EFI_CAPSULE_BLOCK_DESCRIPTOR that describes the
                                location in physical memory of a set of capsules.

  @return  EFI_STATUS
*/
EFI_STATUS AmiScheduleCapsule(IN EFI_PHYSICAL_ADDRESS    ScatterGatherList){
    EFI_STATUS Status;
    CHAR16 CapsuleVarName[30];
    
    if (ScatterGatherList==0 || ScatterGatherList > MAX_UINTN) return EFI_INVALID_PARAMETER;
    if (AmiSmmCapsuleLibSmmVariableInterface==NULL){
        Status = gSmst->SmmLocateProtocol(&gEfiSmmVariableProtocolGuid, NULL, &AmiSmmCapsuleLibSmmVariableInterface);
        if (EFI_ERROR(Status)) return Status;
    }
    Status = AmiSmmCapsuleLibBuildCapsuleVariableName(CapsuleVarName, sizeof(CapsuleVarName)/sizeof(CHAR16));
    if (EFI_ERROR(Status)) return Status;
    Status = AmiSmmCapsuleLibSmmVariableInterface->SmmSetVariable (
        CapsuleVarName, &gEfiCapsuleVendorGuid,
        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
        sizeof(UINTN), (VOID *)&ScatterGatherList
    );
    return Status;
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
