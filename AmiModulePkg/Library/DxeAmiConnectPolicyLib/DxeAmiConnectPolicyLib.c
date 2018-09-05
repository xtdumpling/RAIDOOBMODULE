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
  DxeAmiConnectPolicyLib instance.
**/

#include <Protocol/AmiBdsConnectPolicy.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SecurityManagementLib.h>
#include <Library/DebugLib.h>

AMI_BDS_CONNECT_POLICY_PROTOCOL* AmiConnectPolicyLibAmiBdsConnectPolicy = NULL;

/**
  The function returns platform specific driver connect policy.
  
  The function uses an instance of AMI_BDS_CONNECT_POLICY_PROTOCOL to return
  platform specific driver connect policy for a specified device path.
  NOTE: Even though the function uses Security Architectural Protocol infrastructure, it has no effect on
    platform security. It provides a way to implement platform specific device connect policy.
  
  @param[in]    AuthenticationStatus    Not used 
  @param[in]    File                    The pointer to the device path being connected. 
                                          This is device path associated with ControllerHandle and RemainingDevicePath
                                          passed to a gBS->ConnectController.
  @param[in]    FileBuffer              A pointer to the buffer with the UEFI file image. Must be NULL for gBS->ConnectController handlers.
  @param[in]    FileSize                Not used

  @retval EFI_SUCCESS             FileBuffer is not NULL
  @retval EFI_SUCCESS             Platform connect policy cannot be retrieved
  @retval EFI_SUCCESS             FileBuffer is NULL and platform policy allows to start
                                  UEFI device drivers on the device path specified by File.
  @retval EFI_SECURITY_VIOLATION  FileBuffer is NULL and platform policy does not allow to start
                                  UEFI device drivers on the device path specified by File.
**/
EFI_STATUS EFIAPI  AmiConnectPolicyHandler(
    IN  UINT32 AuthenticationStatus, IN CONST EFI_DEVICE_PATH_PROTOCOL *File,
    IN  VOID *FileBuffer, IN  UINTN FileSize, IN BOOLEAN BootPolicy
){
	AMI_BDS_CONNECT_POLICY Policy;
	EFI_STATUS Status;
	
	if ( FileBuffer != NULL || File == NULL ) return EFI_SUCCESS;
	if ( AmiConnectPolicyLibAmiBdsConnectPolicy == NULL){
	    Status = gBS->LocateProtocol(
	        &gAmiBdsConnectPolicyProtocolGuid, NULL, &AmiConnectPolicyLibAmiBdsConnectPolicy
	    );
	    if (EFI_ERROR(Status)) return EFI_SUCCESS;
	}
	Status = AmiConnectPolicyLibAmiBdsConnectPolicy->GetDevicePathPolicy(
	    AmiConnectPolicyLibAmiBdsConnectPolicy, File, &Policy
	);
	return (Status==EFI_SUCCESS && Policy==AmiBdsConnectPolicyDisable) ? EFI_SECURITY_VIOLATION : EFI_SUCCESS;
}

/**
  Library constructor. Called in Driver's entry point.
  @param ImageHandle     Image handle.
  @param SystemTable    Pointer to the EFI system table.

  @retval: EFI_SUCCESS  
**/
EFI_STATUS EFIAPI AmiConnectPolicyLibConstructor(
    IN  EFI_HANDLE ImageHandle, IN  EFI_SYSTEM_TABLE *SystemTable
){
    EFI_STATUS Status;
    
    Status = RegisterSecurity2Handler(
        AmiConnectPolicyHandler,EFI_AUTH_OPERATION_CONNECT_POLICY
    );
    ASSERT_EFI_ERROR (Status);
    return EFI_SUCCESS;
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
