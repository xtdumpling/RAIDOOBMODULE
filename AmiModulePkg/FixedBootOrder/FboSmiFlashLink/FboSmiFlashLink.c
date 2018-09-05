  //***********************************************************************
  //***********************************************************************
  //**                                                                   **
  //**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
  //**                                                                   **
  //**                       All Rights Reserved.                        **
  //**                                                                   **
  //**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
  //**                                                                   **
  //**                       Phone: (770)-246-8600                       **
  //**                                                                   **
  //***********************************************************************
  //***********************************************************************

/** @file FboSmiFlashLink.c

    Restore some specified variables after flashing BIOS by AFU.

*/

#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiSmm.h>
#include <../FixedBootOrder.h>
#include <Setup.h>
#include <Include/BootOptions.h>

#pragma pack(1)
typedef struct {
    CHAR16      *BootVarName;
    EFI_GUID    Guid;
    UINT32      Attrib;
    UINTN       Size;
    UINT8       *Data;
} FBO_SAVED_VAR;
#pragma pack()

typedef EFI_STATUS (*SHOW_BOOT_TIME_VARIABLES)(BOOLEAN Show);
typedef struct{
    SHOW_BOOT_TIME_VARIABLES ShowBootTimeVariables;
} AMI_NVRAM_CONTROL_PROTOCOL;
AMI_NVRAM_CONTROL_PROTOCOL *LocateNvramControlSmmProtocol(VOID);

STATIC BOOLEAN PreserveRestoreLock = FALSE;

//
// Need UefiDevOrder, FboLegacyDevOrder and OldLegacyDevOrder variables 
// to set boot options' attributes right(at FboSyncBootOptionsFromDevOrder function)
//
FBO_SAVED_VAR   *BootVariables, FboVariables[] = 
{
	    {L"UefiDevOrder", FIXED_BOOT_ORDER_GUID, 0, 0, NULL}, 
	    {L"FboLegacyDevOrder", FIXED_BOOT_ORDER_GUID, 0, 0, NULL},
	    {L"OldLegacyDevOrder", LEGACY_DEV_ORDER_GUID, 0, 0, NULL},
	    {NULL, SETUP_GUID, 0, 0, NULL}
};
SETUP_DATA          OldSetupData;
/**
    Get Variable and allocate Name and Data buffer

    @param Name - Used to get the variable
        VarBuffer - buffer to save the variable

    @retval VOID

**/
EFI_STATUS
FboGetVariable
(
    CHAR16          *Name,
    FBO_SAVED_VAR   *VarBuffer
)
{
    EFI_STATUS  Status;
    UINT8       FakeBuffer;
    
    Status = pSmst->SmmAllocatePool(
             EfiRuntimeServicesData, 
             (sizeof(CHAR16) * (Wcslen(Name) + 1)),  // 1 for L'\0'
             (void**)&(VarBuffer->BootVarName));
    
    if(EFI_ERROR(Status))
        return Status;
    
    MemCpy(VarBuffer->BootVarName, Name, (Wcslen(Name) + 1) * sizeof(CHAR16));

    VarBuffer->BootVarName[Wcslen(Name)] = L'\0';

    VarBuffer->Size = 0;
    Status = pRS->GetVariable( 
            VarBuffer->BootVarName, 
            &VarBuffer->Guid, 
            &VarBuffer->Attrib, 
            &VarBuffer->Size, 
            &FakeBuffer);

    // because Size = 0, only get EFI_NOT_FOUND and EFI_BUFFER_TOO_SMALL
    if(Status != EFI_BUFFER_TOO_SMALL)
	{
        pSmst->SmmFreePool(VarBuffer->BootVarName);
		VarBuffer->BootVarName = NULL;
        return Status;
	}
    
    Status = pSmst->SmmAllocatePool(
             EfiRuntimeServicesData, 
             VarBuffer->Size, 
             (void**)&(VarBuffer->Data));
    
    if(EFI_ERROR(Status))
    {
        pSmst->SmmFreePool(VarBuffer->BootVarName);
		VarBuffer->BootVarName = NULL;
        return Status;
    }
    
    Status = pRS->GetVariable( 
            VarBuffer->BootVarName, 
            &VarBuffer->Guid, 
            &VarBuffer->Attrib, 
            &VarBuffer->Size, 
            VarBuffer->Data);
    
    return Status;
}
/**
    Save / Restore Boot Variables

    @param SaveRestore - FASLE - Save
        TRUE  - Restore

    @retval VOID

**/
VOID
SaveRestoreBootVariables
(
    BOOLEAN    SaveRestore
)
{
    static FBO_SAVED_VAR*   BootVariables = NULL;
    EFI_GUID        gEfiGlobalVariableGuid = EFI_GLOBAL_VARIABLE_GUID;
    UINTN           i;
    static UINTN    NumBootOption = 0;
    EFI_STATUS      Status;

    // Save
    if(!SaveRestore)
    {
        UINTN           VarSize = 0;

        if(BootVariables)
            return;

        Status = pRS->GetVariable( 
                L"BootOrder", 
                &gEfiGlobalVariableGuid, 
                NULL, 
                &VarSize, 
                NULL);

        // because Size = 0, only get EFI_NOT_FOUND and EFI_BUFFER_TOO_SMALL
        if(Status != EFI_BUFFER_TOO_SMALL)
            return;
        
        // Allocate FBO_SAVED_VAR needed.
        Status = pSmst->SmmAllocatePool(
                 EfiRuntimeServicesData, 
                 (sizeof(FBO_SAVED_VAR) * (VarSize/sizeof(UINT16) + 1)), 
                 (void**)&BootVariables);
        

         if(EFI_ERROR(Status))
             return;
         
         // Index 0 is for BootOrder.
         BootVariables[0].Guid = gEfiGlobalVariableGuid;
         Status = FboGetVariable(L"BootOrder", &BootVariables[0]);

         if(EFI_ERROR(Status))
         {
             pSmst->SmmFreePool(BootVariables);
             BootVariables = NULL;
             return;
         }
         
         for(i = 1 ; i <= VarSize/sizeof(UINT16) ; i ++)
         {
             CHAR16     BootOptionName[9];

             Swprintf( BootOptionName, L"Boot%04x", (UINT16)(BootVariables[0].Data[(i-1)*2]));
             BootVariables[i].Guid = gEfiGlobalVariableGuid;
             FboGetVariable(BootOptionName, &BootVariables[i]);
             TRACE((-1,"[FBO] Variable %S Saved \n",BootOptionName));
         }
         
        NumBootOption = i;
    }else
        // Restore
    {
        if(!BootVariables)
            return;

        for(i = 0 ; i < NumBootOption ; i++)
        {
            if(!BootVariables[i].BootVarName)
                continue;
            pRS->SetVariable(
                    BootVariables[i].BootVarName,
                    &BootVariables[i].Guid,
                    BootVariables[i].Attrib,
                    BootVariables[i].Size,
                    BootVariables[i].Data);
            TRACE((-1,"[FBO] Restore %S \n",BootVariables[i].BootVarName));
            pSmst->SmmFreePool(BootVariables[i].BootVarName);
            pSmst->SmmFreePool(BootVariables[i].Data);
        }
        // Free Main buffer
        pSmst->SmmFreePool(BootVariables);
        BootVariables = NULL;
    }
}
/**
    Preserve Fixed Boot Order variables

    @param VOID

    @retval VOID

**/
VOID PreserveFboVariables 
(
    VOID 
)
{
    UINTN           i;
    EFI_STATUS      Status;
    EFI_GUID        SetupGuid = SETUP_GUID;
    AMI_NVRAM_CONTROL_PROTOCOL *AmiNvramCtrlProtocol;

    if(PreserveRestoreLock)
        return;

    AmiNvramCtrlProtocol = LocateNvramControlSmmProtocol();
    if(!AmiNvramCtrlProtocol) 
        return; 

    Status = AmiNvramCtrlProtocol->ShowBootTimeVariables(TRUE);
    if(EFI_ERROR(Status))
        return;
    
    for(i = 0 ;FboVariables[i].BootVarName != NULL ; i++ )
    {
        FboVariables[i].Size = 0;
        Status = pRS->GetVariable( 
                FboVariables[i].BootVarName, 
                &FboVariables[i].Guid, 
                &FboVariables[i].Attrib, 
                &FboVariables[i].Size, 
                FboVariables[i].Data);

        // because Size = 0, only get EFI_NOT_FOUND and EFI_BUFFER_TOO_SMALL
        if(Status == EFI_NOT_FOUND)
            continue;
        
        // Allocate Pool for buffer
        Status = pSmst->SmmAllocatePool(
                EfiRuntimeServicesData, 
                FboVariables[i].Size, 
                (void**)&FboVariables[i].Data);

        if(EFI_ERROR(Status))
            continue;

        // Fill buffer data
        pRS->GetVariable( 
                FboVariables[i].BootVarName, 
                &FboVariables[i].Guid, 
                &FboVariables[i].Attrib, 
                &FboVariables[i].Size, 
                FboVariables[i].Data);
        TRACE((-1,"[FBO] Variable %S Saved \n",FboVariables[i].BootVarName));

    }
    
    i = sizeof(SETUP_DATA);
    // Handle Setup variable, 
    pRS->GetVariable(
                 L"Setup",
                 &SetupGuid,
                 NULL,
                 &i,
                 &OldSetupData
             );

    SaveRestoreBootVariables(FALSE);
    PreserveRestoreLock = TRUE;

    AmiNvramCtrlProtocol->ShowBootTimeVariables(FALSE);
}
/**
    Restore Fixed Boot Order variables

    @param VOID

    @retval VOID

**/

VOID RestoreFboVariables 
(
    VOID
)
{
    UINTN           i;
    EFI_STATUS      Status;
    SETUP_DATA      NewSetupData;
    EFI_GUID        SetupGuid = SETUP_GUID;
	UINT32          SetupAttr;
    AMI_NVRAM_CONTROL_PROTOCOL *AmiNvramCtrlProtocol;
    
    if(!PreserveRestoreLock)
        return;

    AmiNvramCtrlProtocol = LocateNvramControlSmmProtocol();
    if(!AmiNvramCtrlProtocol) 
        return; 

    Status = AmiNvramCtrlProtocol->ShowBootTimeVariables(TRUE);
    if(EFI_ERROR(Status))
        return;

    for(i = 0 ;FboVariables[i].BootVarName != NULL ; i++ )
    {
        // If buffer is Null
        if(!FboVariables[i].Data)
            continue;
        
        Status = pRS->SetVariable( 
                FboVariables[i].BootVarName, 
                &FboVariables[i].Guid, 
                FboVariables[i].Attrib, 
                FboVariables[i].Size, 
                FboVariables[i].Data);
        TRACE((-1,"[FBO] Restore %S \n",FboVariables[i].BootVarName));
        pSmst->SmmFreePool(FboVariables[i].Data);
	 	FboVariables[i].Data = NULL;
    }
    
    SaveRestoreBootVariables(TRUE);
    
    i = sizeof(SETUP_DATA);
    // Handle Setup variable, 
    Status = pRS->GetVariable(
                 L"Setup",
                 &SetupGuid,
                 &SetupAttr,
                 &i,
                 &NewSetupData
             );

    // Setup Data struct isn't changed and
    // the size isn't different form current define
    if(i == sizeof(SETUP_DATA) && !EFI_ERROR(Status))
    {
        for(i = 0 ; i < 16 ; i++) // 16 sync from DefaultFixedBootOrder.sd
        {
            NewSetupData.LegacyPriorities[i]
                                          = OldSetupData.LegacyPriorities[i];
            NewSetupData.UefiPriorities[i]
                                          = OldSetupData.UefiPriorities[i];
        }

        #if FBO_DUAL_MODE
        for(i = 0 ; i < 32 ; i++) // 32 sync from DefaultFixedBootOrder.sd
            NewSetupData.DualPriorities[i]
                                          = OldSetupData.DualPriorities[i];
        #endif

        NewSetupData.BootMode = OldSetupData.BootMode;
        
        pRS->SetVariable(
                     L"Setup",
                     &SetupGuid,
                     SetupAttr,
                     sizeof(SETUP_DATA),
                     &NewSetupData
                 );
    }
    
    PreserveRestoreLock = FALSE;

    AmiNvramCtrlProtocol->ShowBootTimeVariables(FALSE);
}

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2013, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
