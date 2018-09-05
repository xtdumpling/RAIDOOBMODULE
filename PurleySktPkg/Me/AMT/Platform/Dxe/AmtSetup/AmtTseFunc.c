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
//**********************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
//
//**********************************************************************
/** @file AmtTseFunc.c
    AMT TSE Functions.

**/
#include <Setup.h>
#include <TseCommon.h>
#include "variable.h"
#include <Protocol/AmtWrapperProtocol.h>
#include <Protocol/AlertStandardFormat.h>

#define NETWORK_STACK_GUID \
  { 0xD1405D16, 0x7AFC, 0x4695, 0xBB, 0x12, 0x41, 0x45, 0x9D, 0x36, 0x95, 0xA2 }

#define SETUP_GUID \
        { 0xEC87D643, 0xEBA4, 0x4BB5, 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 }

#define	BOOT_MANAGER_GUID \
	{ 0xB4909CF3, 0x7B93, 0x4751, 0x9B, 0xD8, 0x5B, 0xA8, 0x22, 0x0B, 0x9B, 0xB2 }

#define EFI_BOOT_SCRIPT_SAVE_PROTOCOL_GUID \
  { \
    0x470e1529, 0xb79e, 0x4e32, 0xa0, 0xfe, 0x6a, 0x15, 0x6d, 0x29, 0xf9, 0xb2 \
  }

#define AMI_EFI_SOL_POST_MESSAGE_GUID \
 { 0xf42f3752, 0x12e, 0x4812, 0x99, 0xe6, 0x49, 0xf9, 0x43, 0x4, 0x84, 0x54 }

//EFI_GUID gEfiAmtWrapperProtocolGuidTse = EFI_AMT_WRAPPER_PROTOCOL_GUID;
extern EFI_GUID gEfiAmtWrapperProtocolGuid;
EFI_GUID  	mNetworkStackGuid = NETWORK_STACK_GUID;

extern BOOLEAN gEnterSetup;


extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_RUNTIME_SERVICES *gRT;

//*******************************************************************************
/**

    @param Event : Timer event.
    @param Context : Event context; always NULL

    @retval VOID

**/
//*******************************************************************************
VOID iAMTProcessProceedToBootHook ( EFI_EVENT Event, VOID *Context)
{
    AMT_WRAPPER_PROTOCOL *pAmtWrapper = NULL;
    EFI_STATUS            Status;
    ALERT_STANDARD_FORMAT_PROTOCOL  *AsfCheck;
    ASF_BOOT_OPTIONS  *mInternalAsfBootOptions;
    if (pAmtWrapper == NULL) {
        Status = gBS->LocateProtocol(&gEfiAmtWrapperProtocolGuid, NULL, &pAmtWrapper);
    }

    //case IDER
    if (pAmtWrapper != NULL) {
        if (pAmtWrapper->ActiveManagementEnableStorageRedir()||pAmtWrapper->ActiveManagementEnableSol()){
        Status = pAmtWrapper->BdsBootViaAsf();
        return;
        }
    }

    //case ASF
    //Get the ASF options
    //if set then we have to do and Asfboot
    Status = gBS->LocateProtocol (
                    &gAlertStandardFormatProtocolGuid,
                    NULL,
                    &AsfCheck
                    );
	
    if (EFI_ERROR (Status)) {
        return;
    }

    Status = AsfCheck->GetBootOptions (AsfCheck, &mInternalAsfBootOptions);
    if (mInternalAsfBootOptions->SubCommand != ASF_BOOT_OPTIONS_PRESENT) {
        return;
    }

    else{
        if(mInternalAsfBootOptions->SpecialCommandParam == USE_KVM)
        {
            return;
        }
        else
        {
            Status = pAmtWrapper->BdsBootViaAsf();
            return;            
        }
    }
}


//*******************************************************************************
/**

    @param Event : Timer event.
    @param Context : Event context; always NULL

    @retval VOID

**/
//*******************************************************************************
VOID iAMTProcessEnterSetupHook  ( EFI_EVENT Event, VOID *Context)
{
    EFI_STATUS Status;
    UINT16	   count = 0;
    EFI_GUID BootManGuid = BOOT_MANAGER_GUID;
    AMT_WRAPPER_PROTOCOL *pAmtWrapper = NULL;

    if (pAmtWrapper == NULL) 
        Status = gBS->LocateProtocol(&gEfiAmtWrapperProtocolGuid, NULL, &pAmtWrapper);
    if (pAmtWrapper != NULL) {		  
        if (pAmtWrapper->ActiveManagementEnableStorageRedir()) {
            count = 0xFFFF;
        }
    }
    if (count == 0xFFFF){ 
        gRT->SetVariable( L"BootManager", &BootManGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS,sizeof(count),&count );}
}
/**
    This function is a hook called when TSE determines
    that SETUP utility has to be displayed. This function
    is available as ELINK. In the generic implementation
    setup password is prompted in this function.

    @param VOID

    @retval VOID

**/
BOOLEAN iAMTProcessConInAvailabilityHook  (VOID)
{
    AMT_WRAPPER_PROTOCOL *pAmtWrapper = NULL;
    EFI_HANDLE	SolPostMessageHandle = NULL;
    EFI_STATUS Status;
    EFI_GUID gAmiEfiSolPostMessageGuid = AMI_EFI_SOL_POST_MESSAGE_GUID;
    // Handle EnterSetup Flag here !!
    if (pAmtWrapper == NULL){
        Status = gBS->LocateProtocol(&gEfiAmtWrapperProtocolGuid, NULL, &pAmtWrapper);
    }
    if(EFI_ERROR(Status))
        return FALSE;

    if(pAmtWrapper->ActiveManagementEnterSetup())
    {
        gEnterSetup = TRUE;
    }
    // Install Protocol here, Trig Callback event to show SOL message.
    Status = gBS->InstallMultipleProtocolInterfaces(
                 &SolPostMessageHandle,
                 &gAmiEfiSolPostMessageGuid,
                 NULL,
                 NULL);


    return FALSE;
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
