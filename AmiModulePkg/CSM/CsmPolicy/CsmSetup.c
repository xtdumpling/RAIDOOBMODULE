//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
/** @file CsmSetup.c
    CSM Setup related functions

**/
//**********************************************************************

#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Protocol/AMIPostMgr.h>
#include <Protocol/PciIo.h>
#include <Setup.h>

EFI_GUID gAmiCsmThunkDriverGuid = { 0x2362ea9c, 0x84e5, 0x4dff, { 0x83, 0xbc, 0xb5, 0xac, 0xec, 0xb5, 0x7c, 0xbb } };

/**
    This function is eLink'ed with the chain executed right before
    the Setup.

**/

VOID InitCsmStrings(EFI_HII_HANDLE HiiHandle, UINT16 Class)
{
    UINT8 MjCsmVer = *(UINT8*)0xF0018;
    UINT8 MnCsmVer = *(UINT8*)0xF0019;

	//example: InitString(HiiHandle, STRING_TOKEN(STR_USB_MODULE_VERSION_VALUE), L"%d", 25);
	// Module version

    if (MjCsmVer != 0xff)
        InitString(HiiHandle, STRING_TOKEN(STR_CSM_MODULE_VERSION_VALUE),
                L"%02x.%02x", MjCsmVer, MnCsmVer);

}

/**
    This function checks wheter passed controller is managed by  UEFI GOP driver
               
         
    @param pci_hnd handle of the controller

          
    @retval 0 controller managed by legacy driver
    @retval 1 controller managed by UEFI driver

**/
BOOLEAN IsUefiGop(
    IN EFI_HANDLE pci_hnd
)
{
    EFI_STATUS Status;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *ent;
    UINTN n_ent;
    UINTN i;
    VOID *p;
    BOOLEAN Result = TRUE;

    Status = pBS->OpenProtocolInformation(pci_hnd, &gEfiPciIoProtocolGuid, &ent, &n_ent);
    if(EFI_ERROR(Status))
        return FALSE;

    for(i = 0; i < n_ent; i++) {
        if(!(ent[i].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER))
            continue;

        Status = pBS->HandleProtocol(ent[i].AgentHandle, &gAmiCsmThunkDriverGuid, &p);
        if(!EFI_ERROR(Status)) {
            Result = FALSE;
            break;
        }
    }
    pBS->FreePool(ent);

    return Result;
}

/**
    This function checks if active video is Legacy or UEFI
               
         
    @param VOID

          
    @retval 0 active video is UEFI
    @retval 1 active vide is Legacy

**/
BOOLEAN IsLegacyVideo(
    VOID
)
{
    EFI_STATUS Status;
    EFI_HANDLE *hnd;
    EFI_HANDLE pci_hnd;
    UINTN n_hnd;
    UINTN i;
    EFI_DEVICE_PATH_PROTOCOL *dp;

    Status = pBS->LocateHandleBuffer(ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL, &n_hnd, &hnd);
    if(EFI_ERROR(Status))
        return FALSE;

    for(i = 0; i < n_hnd; i++) {
        Status = pBS->HandleProtocol(hnd[i], &gEfiDevicePathProtocolGuid, &dp);
        if(EFI_ERROR(Status))
            continue;
        Status = pBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &dp, &pci_hnd);
        if(EFI_ERROR(Status))
            continue;

        if(IsUefiGop(pci_hnd))
            return FALSE;
    }
    pBS->FreePool(hnd);
    return TRUE;
}

/**
    This function checks if CSM can be disabled in current boot
               
         
    @param HiiHandle HII handle of formset
    @param Class class of formset
    @param SubClass subclass of formset
    @param Key Id of setup control

          
    @retval 0 active video is UEFI
    @retval 1 active vide is Legacy

**/
EFI_STATUS CsmPolicyLaunchCallback(
    IN EFI_HII_HANDLE HiiHandle, 
    IN UINT16 Class, 
    IN UINT16 SubClass, 
    IN UINT16 Key
)
{
    EFI_STATUS Status;
    static EFI_GUID AmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
    AMI_POST_MANAGER_PROTOCOL* AmiPostMgr;
    UINT8 MsgKey;
    EFI_BROWSER_ACTION_REQUEST *rq;

    CALLBACK_PARAMETERS *Callback = NULL;

    Callback = GetCallbackParameters();
    if(!Callback || Callback->Action != EFI_BROWSER_ACTION_CHANGING)
        return EFI_UNSUPPORTED;

    if(Callback->Value->u8 == 0) {
    /* trying to disable CSM */
        if(IsLegacyVideo()) {
            rq = Callback->ActionRequest;
            *rq = EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD;
            Status = pBS->LocateProtocol(&AmiPostManagerProtocolGuid, NULL, &AmiPostMgr);
            Status = AmiPostMgr->DisplayMsgBox(L" Warning!!! ",
                                               L"Video is in Legacy mode. Select Video policy UEFI first, reboot and try again",
                                               MSGBOX_TYPE_OK,
                                               &MsgKey);
			return EFI_NOT_FOUND;
/* this should be enough, but TSE has a bug that doesn't support FORM_DISCARD action try a workaround instead 
{
    SETUP_DATA Setup;
    UINTN Size = sizeof(Setup);
    static EFI_GUID SetupGuid = SETUP_GUID;
    Status = HiiLibGetBrowserData(&Size, &Setup, &SetupGuid, L"Setup");
    if(EFI_ERROR(Status))
        return Status;
	TRACE((-1, "Data size(c2): %x\n", Size));
    Setup.CsmSupport = 1;
    Status = HiiLibSetBrowserData(Size, &Setup, &SetupGuid, L"Setup");
}
 end of workaround */
        }
    }
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**           5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093      **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
