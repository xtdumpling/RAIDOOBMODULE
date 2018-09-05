//***************************************************************************
//**                                                                       **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.           **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  Rev. 1.02
//    Bug Fix:  Fixed the UEFI iSCSI not working
//    Reason:   Depend on Guid.
//    Auditor:  Sunny Yang
//    Date:     Aug/29/2016
//
//***************************************************************************
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

//**********************************************************************
/** @file NetworkStackSetupScreen.c
    NetworkStackSetupScreen related functions

**/
//**********************************************************************
#include <AmiDxeLib.h>
#include <NetworkStackSetup.h>
#include "Token.h"

#if SMCPKG_SUPPORT
#define INTEL_GIGABIT_LAN_POLICY_PROTOCOL_GUID\
	{0x2BD77D17, 0x3771, 0x4974, 0x86, 0x44, 0xF9, 0x9C, 0xF5, 0xB2, 0x66, 0xA7}

#define INTEL_10G_LAN_POLICY_PROTOCOL_GUID\
	{0x2BD77D17, 0x3771, 0x4974, 0x86, 0x44, 0xF9, 0x9C, 0xF5, 0xB2, 0x66, 0xA8}
#endif

NETWORK_STACK  	mNetworkStackData;


/**
    Entry point of the  network stack setup driver. This entry point is 
    necessary to initlialize the NetworkStack setup driver.

    @param 
        ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
        SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table  
 
    @retval 
  EFI_SUCCESS:              Driver initialized successfully
**/

EFI_STATUS EFIAPI InitNetworkStackVar (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{

  UINTN                Size;
  EFI_STATUS           Status;
#if SMCPKG_SUPPORT
  EFI_GUID gIntelGigabitLanPolicyProtocolGuid = INTEL_GIGABIT_LAN_POLICY_PROTOCOL_GUID;	
  EFI_GUID gIntel10GLanPolicyProtocolGuid = INTEL_10G_LAN_POLICY_PROTOCOL_GUID;	
#endif  

  InitAmiLib(ImageHandle,SystemTable);

  Size = sizeof(NETWORK_STACK);
  Status = pRS->GetVariable(L"NetworkStackVar",&gEfiNetworkStackSetupGuid, NULL, &Size, &mNetworkStackData);

#if SMCPKG_SUPPORT
  if( mNetworkStackData.Enable == 1 ) {
    //#if IntelGigabitLan_SUPPORT
	    pBS->InstallProtocolInterface(
                 &ImageHandle,
                 &gIntelGigabitLanPolicyProtocolGuid,
                 EFI_NATIVE_INTERFACE,				
		   NULL);
    //#endif

    //#if Intel10GLan_SUPPORT
	    pBS->InstallProtocolInterface(
                 &ImageHandle,
                 &gIntel10GLanPolicyProtocolGuid,
                 EFI_NATIVE_INTERFACE,				
		   NULL);
    //#endif
  }
#endif
  if (Status == EFI_NOT_FOUND) {

    pBS->SetMem(&mNetworkStackData, 
                    sizeof(NETWORK_STACK), 
                    0);

    pRS->SetVariable(
         L"NetworkStackVar",&gEfiNetworkStackSetupGuid,
         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
         sizeof(NETWORK_STACK), &mNetworkStackData
     );

  } 

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

