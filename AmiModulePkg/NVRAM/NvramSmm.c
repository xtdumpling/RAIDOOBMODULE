//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.00
//       Bug fixed:   Add SMM NVRAM  control to access boot time variable in SMM
//       Reason:      
//       Auditor:     William Hsu (reference from greenlow refresh WS)
//       Date:        Dec/16/2016
//
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

#include "NvramDxeCommon.h"
#include <Protocol/SmmVariable.h>
#include <Protocol/SmmReadyToLock.h>
#include <Guid/HobList.h>
#include "NvramSmi.h"

EFI_STATUS SmmSetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
);
VOID ResetMorVariables();

EFI_SMM_VARIABLE_PROTOCOL SmmVariable = {DxeGetVariableWrapper, DxeGetNextVariableNameWrapper, SmmSetVariable, QueryVariableInfo};
EFI_SMM_VARIABLE_PROTOCOL SmmVariableWithLockSupport = {DxeGetVariableWrapper, DxeGetNextVariableNameWrapper, DxeSetVariableWrapper, QueryVariableInfo};
BOOLEAN LockEnforcementIsActive = TRUE;

#if defined(SMCPKG_SUPPORT) && (SMCPKG_SUPPORT == 1)
typedef EFI_STATUS (*SHOW_BOOT_TIME_VARIABLES)(BOOLEAN Show);
typedef struct{
    SHOW_BOOT_TIME_VARIABLES ShowBootTimeVariables;
} AMI_NVRAM_CONTROL_PROTOCOL;
static EFI_GUID gAmiNvramControlProtocolGuid = { 0xf7ca7568, 0x5a09, 0x4d2c, { 0x8a, 0x9b, 0x75, 0x84, 0x68, 0x59, 0x2a, 0xe2 } };
EFI_STATUS ShowBootTimeVariables (BOOLEAN Show);
AMI_NVRAM_CONTROL_PROTOCOL NvramControl = {ShowBootTimeVariables };

EFI_STATUS InstallNvramControlSmmProtocol(){
    return pSmst->SmmInstallConfigurationTable(
        pSmst, &gAmiNvramControlProtocolGuid, &NvramControl, sizeof(NvramControl) 
    );
}
#endif

#if AuthVariable_SUPPORT
VOID AuthVariableServiceInitSMM (VOID);
#else
VOID AuthVariableServiceInitSMM (VOID) {};
#endif //#if AuthVariable_SUPPORT

EFI_STATUS
EFIAPI
NvramSmiEntry(
    IN EFI_HANDLE                ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
);

VOID* NvramAllocatePages(UINTN Size){
	EFI_PHYSICAL_ADDRESS Address;
	EFI_STATUS Status;
	
	Status = pSmst->SmmAllocatePages(AllocateAnyPages, EfiRuntimeServicesData, EFI_SIZE_TO_PAGES(Size), &Address);
	if (EFI_ERROR(Status)) return NULL;
	return (VOID*)Address;
}

VOID NvramFreePages(VOID *Address, UINTN Size){
	EFI_STATUS Status;
	
	Status =  pSmst->SmmFreePages((EFI_PHYSICAL_ADDRESS)(UINTN)Address, EFI_SIZE_TO_PAGES(Size));
	ASSERT_EFI_ERROR (Status);
}

AMI_FLASH_PROTOCOL *LocateFlashProtocol(){
    typedef EFI_STATUS (EFIAPI *AMI_NVRAM_FLASH_INTERFACE_GET)(OUT AMI_FLASH_PROTOCOL **FlashInterface);

    typedef struct _AMI_NVRAM_FLASH_INTERFACE_PROTOCOL{
        AMI_NVRAM_FLASH_INTERFACE_GET GetNvramFlashInterface;
    } AMI_NVRAM_FLASH_INTERFACE_PROTOCOL;

    static EFI_GUID gAmiNvramFlashInterfaceProtocolGuid =
        { 0xfed7c6, 0xad32, 0x4793, { 0x81, 0x94, 0xd2, 0xd2, 0x41, 0xa3, 0x3f, 0x10 } };

    AMI_NVRAM_FLASH_INTERFACE_PROTOCOL *FlashInterface;
    EFI_STATUS Status;
    AMI_FLASH_PROTOCOL *Flash;
    
    Status = pSmst->SmmLocateProtocol(&gAmiNvramFlashInterfaceProtocolGuid, NULL, (VOID**)&FlashInterface);
    ASSERT_EFI_ERROR(Status); 
    if (EFI_ERROR(Status)) return NULL;
    Status = FlashInterface->GetNvramFlashInterface(&Flash);
    ASSERT_EFI_ERROR(Status); 
    if (EFI_ERROR(Status)) return NULL;
    return Flash;
}

/**
    This function sets Var with specific GUID, Name and attributes in SMM
    synchronizing Varstors before and after operation

    @param VariableName pointer to Var Name in Unicode
    @param VendorGuid pointer to Var GUID
    @param Attributes Attributes of the Var
    @param DataSize size of Var
    @param Data Pointer to memory where Var data is stored

    @retval EFI_STATUS based on result

**/
EFI_STATUS SmmSetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
    EFI_STATUS Status;
    
    LockEnforcementIsActive = FALSE;
    Status = DxeSetVariableWrapper(VariableName, VendorGuid, Attributes, DataSize, Data);
    LockEnforcementIsActive = TRUE;
    
    return Status;
}

EFI_STATUS EFIAPI SmmReadyToLockCallback(
  IN CONST EFI_GUID *Protocol, IN VOID *Interface, IN EFI_HANDLE Handle
){
	SwitchToBds();
	ResetMorVariables();
	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI NvramSmmInit(
    IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS  Status;
    EFI_HANDLE  Handle = NULL;
    VOID *Registration;
    VOID *Address;
    NVRAM_MAILBOX Mailbox;
    UINTN Size=0;
    
    InitAmiSmmLib(ImageHandle,SystemTable);
    Status = NvramInitialize();
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    AuthVariableServiceInitSMM();
#if defined(SMCPKG_SUPPORT) && (SMCPKG_SUPPORT == 1)
    InstallNvramControlSmmProtocol();
#endif    
    Size = sizeof(Mailbox);

    Status = pST->RuntimeServices->GetVariable(
        L"NvramMailBox",&gAmiNvramSmmCommunicationGuid,NULL,&Size, &Mailbox
    );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    // Delete the NvramMailBox variable
    Status = pST->RuntimeServices->SetVariable(
        L"NvramMailBox",&gAmiNvramSmmCommunicationGuid,0,0, NULL
    );
    ASSERT_EFI_ERROR(Status);
    
    ASSERT(VarStoreInfo.MemInfo->NvramSize == Mailbox.VarStoreInfoPtr->MemInfo->NvramSize);
    if (VarStoreInfo.MemInfo->NvramSize != Mailbox.VarStoreInfoPtr->MemInfo->NvramSize) return EFI_INVALID_PARAMETER;
    MemCpy(VarStoreInfo.MemInfo->NvramAddress,Mailbox.VarStoreInfoPtr->MemInfo->NvramAddress,VarStoreInfo.MemInfo->NvramSize);
    ResetVarStore(VarStoreInfo.MemInfo);
    if ((VarStoreInfo.NvramMode & NVRAM_MODE_SIMULATION)!=0){
        // if we're in simulation mode, copy content from DXE in-memory NVARM image
        ASSERT(VarStoreInfo.NvInfo->NvramSize == Mailbox.VarStoreInfoPtr->NvInfo->NvramSize);
        if (VarStoreInfo.NvInfo->NvramSize != Mailbox.VarStoreInfoPtr->NvInfo->NvramSize) return EFI_INVALID_PARAMETER;
        MemCpy(VarStoreInfo.NvInfo->NvramAddress,Mailbox.VarStoreInfoPtr->NvInfo->NvramAddress,VarStoreInfo.NvInfo->NvramSize);
        ResetVarStore(VarStoreInfo.NvInfo);
        UpdateNestedVarstores();
    }

    Status = pSmst->SmmAllocatePool ( EfiRuntimeServicesData, NVRAM_SIZE / 2, &Address);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    InitNvramLockBuffer(Address,NVRAM_SIZE/2);

    Status = NvramSmiEntry(ImageHandle, SystemTable);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;

    Status = Mailbox.SwitchToSmmWrapper();
    if (EFI_ERROR(Status)) return Status;
    
    pSmst->SmmRegisterProtocolNotify(&gEfiSmmReadyToLockProtocolGuid,SmmReadyToLockCallback,&Registration);
    //Make sure pRS is replaced with the in-SMM version by the library (AmiDxeLib)
    ASSERT(pRS!=pST->RuntimeServices);
    pRS->GetNextVariableName = DxeGetNextVariableNameWrapper;
    pRS->GetVariable = DxeGetVariableWrapper;
    pRS->SetVariable = DxeSetVariableWrapper;
    pRS->QueryVariableInfo = QueryVariableInfo;
    
    Status = pSmst->SmmInstallProtocolInterface(
    	&Handle, &gEfiSmmVariableProtocolGuid, EFI_NATIVE_INTERFACE, &SmmVariable
    );
    ASSERT_EFI_ERROR (Status);
    Status = pSmst->SmmInstallProtocolInterface(
        &Handle, &gAmiNvramSmmCommunicationGuid, EFI_NATIVE_INTERFACE, &SmmVariableWithLockSupport
    );
    ASSERT_EFI_ERROR (Status);
    Status = pSmst->SmmInstallProtocolInterface(
    	&Handle, &gAmiSmmNvramUpdateProtocolGuid, EFI_NATIVE_INTERFACE, &AmiNvramUpdate
    );
    ASSERT_EFI_ERROR (Status);
    
    return Status;
}

BOOLEAN IsLockEnforcementActive (){
    return IsSmmLocked() && LockEnforcementIsActive;
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
