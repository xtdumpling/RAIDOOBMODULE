//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
  Entry point of the Flash SMM driver and supporting code.
**/

#include "FlashCommon.h"
#include <Library/SmmServicesTableLib.h>

EFI_GUID gAmiNvramFlashInterfaceProtocolGuid =
    { 0xfed7c6, 0xad32, 0x4793, { 0x81, 0x94, 0xd2, 0xd2, 0x41, 0xa3, 0x3f, 0x10 } };

typedef EFI_STATUS (EFIAPI *AMI_NVRAM_FLASH_INTERFACE_GET)(OUT AMI_FLASH_PROTOCOL **FlashInterface);

/// An internal protocol use by NVRAM driver to get an instance of the flash interface.
typedef struct _AMI_NVRAM_FLASH_INTERFACE_PROTOCOL{
    AMI_NVRAM_FLASH_INTERFACE_GET GetNvramFlashInterface;
} AMI_NVRAM_FLASH_INTERFACE_PROTOCOL;

EFI_STATUS EFIAPI GetNvramFlashInterface(OUT AMI_FLASH_PROTOCOL **FlashInterface);
EFI_HANDLE AmiNvramFlashInterfaceHandle = NULL;
AMI_NVRAM_FLASH_INTERFACE_PROTOCOL NvramFlashInterface = {GetNvramFlashInterface};
AMI_FLASH_PROTOCOL NvramFlashProtocolInstance;

/**
  Returns an instance of the flash interface dedicated for usage by NVRAM driver.

  The function returns an instance of the flash interface dedicated for usage by NVRAM driver.
  This is done to ensure that NVRAM driver has an unrestricted access to a flash API. 
  The standard flash interface may have flash update restrictions imposed by the platform firmware update policy.
  The function is a member function of the internal(no public header provided) AMI_NVRAM_FLASH_INTERFACE_PROTOCOL protocol.
  
  @param[out] FlashInterface Flash interface instance returned by the function
  @return  EFI_STATUS
**/
EFI_STATUS EFIAPI GetNvramFlashInterface(OUT AMI_FLASH_PROTOCOL **FlashInterface){
   static BOOLEAN NvramFlashInterfaceAquired = FALSE;
   
   
   if (NvramFlashInterfaceAquired) return EFI_ACCESS_DENIED;
   if (FlashInterface==NULL) return EFI_INVALID_PARAMETER;
   
   *FlashInterface = &NvramFlashProtocolInstance;
   NvramFlashInterfaceAquired = TRUE;
   // The protocol is intended for a single consumer. Uninstall the protocol was interface is returned.
   gSmst->SmmUninstallProtocolInterface(AmiNvramFlashInterfaceHandle, &gAmiNvramFlashInterfaceProtocolGuid, &NvramFlashInterface);
   return EFI_SUCCESS;
   
}

/**
  Entry point for the SMM Flash Driver.
  
  @param ImageHandle Image handle for this driver
  @param SystemTable Pointer to the system table
  
  @return EFI_STATUS
  @retval EFI_SUCCESS
**/
EFI_STATUS FlashDriverSmmInit(
    IN EFI_HANDLE ImageHandle, 
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status;
    EFI_HANDLE Handler = NULL;

    Status = CreateCriticalSection(&Cs);
    ASSERT_EFI_ERROR(Status);

    // Initialize NVRAM flash protocol instance
    CopyMem(&NvramFlashProtocolInstance, &FlashData.Flash, sizeof(NvramFlashProtocolInstance));
    Status = gBS->InstallProtocolInterface(&Handler, &gAmiSmmFlashProtocolGuid, EFI_NATIVE_INTERFACE, &FlashData.Flash);
    ASSERT_EFI_ERROR(Status);
    Handler = NULL;
    Status = gSmst->SmmInstallProtocolInterface(&Handler, &gAmiSmmFlashProtocolGuid, EFI_NATIVE_INTERFACE, &FlashData.Flash);
    ASSERT_EFI_ERROR(Status);
    Status = gSmst->SmmInstallProtocolInterface(&AmiNvramFlashInterfaceHandle, &gAmiNvramFlashInterfaceProtocolGuid, EFI_NATIVE_INTERFACE, &NvramFlashInterface);
    ASSERT_EFI_ERROR(Status);
    return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
