//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/TcgPlatformSetupPeiPolicy/TcgPlatformSetupPeiPolicy.c 3     12/18/11 10:24p Fredericko $
//
// $Revision: 3 $
//
// $Date: 12/18/11 10:24p $
//**********************************************************************

//<AMI_FHDR_START>
//---------------------------------------------------------------------------
// Name: TcgPlatformpeipolicy.c
//
// Description:	Installs Tcg policy from setup variables in Pei
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Setup.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/TcgPlatformSetupPeiPolicy.h>
#include <Library/DebugLib.h>
#include <Guid/AmiTcgGuidIncludes.h>
#include <Library/HobLib.h>
#include <Hob.h>
#include <Library/BaseMemoryLib.h>
#include <AmiTcg/TCGMisc.h>


extern EFI_GUID gTcgPeiPolicyGuid;

EFI_STATUS TcgPeiFindNextHobByType(IN UINT16   Type, IN OUT VOID **Hob)
{
    EFI_HOB_GENERIC_HEADER *ThisHob;
    if (Hob == NULL) return EFI_INVALID_PARAMETER;
    ThisHob = *Hob;
    while(ThisHob->HobType != EFI_HOB_TYPE_END_OF_HOB_LIST) {
        ThisHob=NextHob(ThisHob,EFI_HOB_GENERIC_HEADER);
        if (ThisHob->HobType==Type) {
            *Hob=ThisHob;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

EFI_STATUS
 getTcgPeiPolicy (IN EFI_PEI_SERVICES     **PeiServices ,
                  IN TCG_CONFIGURATION    *ConfigFlags)
{
  EFI_STATUS              Status;
  UINTN                   VariableSize = sizeof(SETUP_DATA);
  UINT8                   DisallowTpmFlag=0;
  UINT8                   SyncVar = 0;
  UINTN                   SyncVarSize = sizeof(UINT8);
  VOID                          *pHobList;
  AMI_TCG_PEI_POLICY_HOB        *TcgPolicyHob=NULL;
  //
  //
  
  Status = (*PeiServices)->GetHobList(PeiServices, (VOID**)&pHobList);

  TcgPolicyHob = (AMI_TCG_PEI_POLICY_HOB*)pHobList;
  while (!EFI_ERROR(Status = TcgPeiFindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION, &TcgPolicyHob)))
  {
     if (CompareMem(&TcgPolicyHob->EfiHobGuidType.Name, &TcgPeiPolicyHobGuid, sizeof(EFI_GUID)) == 0)
     {
         break;
     }
  }
  
  if (EFI_ERROR(Status))
  {
     ConfigFlags->TpmSupport           = 0;
     ConfigFlags->TcmSupportEnabled    = 0; 
     ConfigFlags->TpmEnable            = 0;
     ConfigFlags->TpmAuthenticate      = 0;
     ConfigFlags->TpmOperation         = 0;
     ConfigFlags->Tpm20Device          = 0;
     ConfigFlags->EndorsementHierarchy   = 0;
     ConfigFlags->StorageHierarchy       = 0;
     ConfigFlags->PlatformHierarchy      = 0;
     ConfigFlags->InterfaceSel = 0;
     ConfigFlags->DeviceType = 0;
     ConfigFlags->DisallowTpm = 0;
     ConfigFlags->PpiSetupSyncFlag  = 0;
     ConfigFlags->PcrBanks  = 0;
  }else {
      ConfigFlags->TpmSupport           = TcgPolicyHob->ConfigInfo.TpmSupport;
      ConfigFlags->TpmEnable            = TcgPolicyHob->ConfigInfo.TpmEnable ;
      ConfigFlags->TpmAuthenticate      = TcgPolicyHob->ConfigInfo.TpmAuthenticate;
      ConfigFlags->TpmOperation         = TcgPolicyHob->ConfigInfo.TpmOperation;
      ConfigFlags->Tpm20Device          = TcgPolicyHob->ConfigInfo.Tpm20Device;
      ConfigFlags->Tcg2SpecVersion            = TcgPolicyHob->ConfigInfo.Tcg2SpecVersion;
      ConfigFlags->EndorsementHierarchy = TcgPolicyHob->ConfigInfo.EndorsementHierarchy;
      ConfigFlags->StorageHierarchy       = TcgPolicyHob->ConfigInfo.StorageHierarchy;
      ConfigFlags->PlatformHierarchy      = TcgPolicyHob->ConfigInfo.PlatformHierarchy;
      ConfigFlags->InterfaceSel           = TcgPolicyHob->ConfigInfo.InterfaceSel;
      ConfigFlags->DeviceType             = TcgPolicyHob->ConfigInfo.DeviceType;
      ConfigFlags->TcmSupportEnabled      = TcgPolicyHob->ConfigInfo.TcmSupportEnabled;
      ConfigFlags->TcgSupportEnabled      = TcgPolicyHob->ConfigInfo.TcgSupportEnabled;
      ConfigFlags->PcrBanks               = TcgPolicyHob->ConfigInfo.PcrBanks;
      ConfigFlags->DisallowTpm = TcgPolicyHob->ConfigInfo.DisallowTpm;
      ConfigFlags->PpiSetupSyncFlag  = TcgPolicyHob->ConfigInfo.PpiSetupSyncFlag;
  }
  return Status;
}



static TCG_PLATFORM_SETUP_INTERFACE   TcgPlatformSetupInstance = {
    TCG_PLATFORM_SETUP_PEI_PROTOCOL_REVISION_1,
    getTcgPeiPolicy
};

static EFI_PEI_PPI_DESCRIPTOR TcgPlatformSetupPeiPolicyDesc[] = {
    {
        EFI_PEI_PPI_DESCRIPTOR_PPI
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gTcgPeiPolicyGuid,
        &TcgPlatformSetupInstance
    }
};



//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   TcgPlatformSetupPolicyEntryPoint
//
// Description:  Entry point for TcgPlatformSetupPolicyEntryPoint
//
// Input:       ImageHandle       Image handle of this driver.
//              SystemTable       Global system service table.
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:   
//
// Notes:       
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI
TcgPlatformSetupPeiPolicyEntryPoint (
  IN EFI_PEI_FILE_HANDLE FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
)
{
  EFI_STATUS 	                Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
  UINTN                          VariableSize = sizeof(SETUP_DATA);
  SETUP_DATA                     SetupData;
  AMI_TCG_PEI_POLICY_HOB         *TcgPolicyHob=NULL;
  UINT8                         SyncVar = 0;
  UINTN                         SyncVarSize = sizeof(UINT8);
  UINT8                         DisallowTpmFlag=0;
    
  //check if ReadOnlyVar is available
  Status = (*PeiServices)->LocatePpi(
                PeiServices,
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0, NULL,
                &ReadOnlyVariable);

  if (EFI_ERROR(Status))
    return EFI_SUCCESS;
  

  //Use Hob to store the policy, it can be updated via Hob
  Status = (*PeiServices)->CreateHob(PeiServices,
                                      EFI_HOB_TYPE_GUID_EXTENSION,
                                      sizeof(AMI_TCG_PEI_POLICY_HOB),
                                      &TcgPolicyHob);
 
  if (EFI_ERROR(Status))return EFI_SUCCESS;

  Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable,
                              L"Setup",
                              &gSetupVariableGuid,
                              NULL,
                              &VariableSize,
                              &SetupData);
  
  ASSERT_EFI_ERROR(Status);
  
  //initialize Hob
  if (TcgPolicyHob != NULL && !EFI_ERROR(Status))
  {
      (*PeiServices)->SetMem((VOID *)&(TcgPolicyHob->ConfigInfo), (sizeof(AMI_TCG_PEI_POLICY_HOB) - sizeof(EFI_HOB_GUID_TYPE)), 0);

      TcgPolicyHob->EfiHobGuidType.Name = TcgPeiPolicyHobGuid;
      TcgPolicyHob->ConfigInfo.TpmSupport           = SetupData.TpmSupport;

      TcgPolicyHob->ConfigInfo.TpmEnable            = SetupData.TpmEnable ;
      TcgPolicyHob->ConfigInfo.TpmAuthenticate      = SetupData.TpmAuthenticate;
      TcgPolicyHob->ConfigInfo.TpmOperation         = SetupData.TpmOperation;
      TcgPolicyHob->ConfigInfo.Tpm20Device          = SetupData.Tpm20Device;
      TcgPolicyHob->ConfigInfo.Tcg2SpecVersion            = SetupData.Tcg2SpecVersion;
      TcgPolicyHob->ConfigInfo.EndorsementHierarchy = SetupData.EndorsementHierarchy;
      TcgPolicyHob->ConfigInfo.StorageHierarchy       = SetupData.StorageHierarchy;
      TcgPolicyHob->ConfigInfo.PlatformHierarchy      = SetupData.PlatformHierarchy;
      TcgPolicyHob->ConfigInfo.InterfaceSel           = SetupData.InterfaceSel;
      TcgPolicyHob->ConfigInfo.DeviceType             = SetupData.DeviceType;
      TcgPolicyHob->ConfigInfo.TcmSupportEnabled      = SetupData.TcmSupportEnabled;
      TcgPolicyHob->ConfigInfo.TcgSupportEnabled      = SetupData.TcgSupportEnabled;
      TcgPolicyHob->ConfigInfo.PcrBanks               = SetupData.Sha1 | SetupData.Sha256 | SetupData.Sha384 |\
                                                         SetupData.Sha512 | ((SetupData.SM3)<<4);
#if (defined(TPM2_DISABLE_PLATFORM_HIERARCHY_RANDOMIZATION) && (TPM2_DISABLE_PLATFORM_HIERARCHY_RANDOMIZATION == 1))            
      TcgPolicyHob->ConfigInfo.Reserved3              = SetupData.PhRandomization;
#endif
#if (defined(EXPOSE_FORCE_TPM_ENABLE) && (EXPOSE_FORCE_TPM_ENABLE == 1))            
      TcgPolicyHob->ConfigInfo.Reserved4              = SetupData.ForceTpmEnable;
#endif
      
      VariableSize = sizeof(UINT8);
      Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable,
                                  L"InternalDisallowTpmFlag",
                                  &gTcgInternalflagGuid,
                                  NULL,
                                  &VariableSize,
                                  &DisallowTpmFlag);
      
      if(EFI_ERROR(Status)){
          Status = EFI_SUCCESS;
          DisallowTpmFlag = 0;
      }

      Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable,
                              L"TcgInternalSyncFlag",
                              &gTcgPpiSyncFlagGuid,
                              NULL,
                              &SyncVarSize,
                              &SyncVar);

      DEBUG((-1, "gTcgPpiSyncFlagGuid Status = %r \n", Status));

      if(EFI_ERROR(Status)){
       SyncVar = 0;
       Status = EFI_SUCCESS;
      }  
          
      TcgPolicyHob->ConfigInfo.DisallowTpm              = DisallowTpmFlag;
      TcgPolicyHob->ConfigInfo.PpiSetupSyncFlag         = SyncVar;
  }
    
  Status = (**PeiServices).InstallPpi (PeiServices, TcgPlatformSetupPeiPolicyDesc);
 
  return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
