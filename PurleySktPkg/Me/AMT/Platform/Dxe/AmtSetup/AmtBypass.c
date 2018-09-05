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
/** @file AmtBypass.c
    AMT Bypass Functions.

**/
#include <Setup.h>
#include <Protocol/AlertStandardFormat.h>
//#include <AmiDxeLib.h>
#include <Library/DebugLib.h>


#define NETWORK_STACK_GUID \
  { 0xD1405D16, 0x7AFC, 0x4695, 0xBB, 0x12, 0x41, 0x45, 0x9D, 0x36, 0x95, 0xA2 }

#define SETUP_GUID \
        { 0xEC87D643, 0xEBA4, 0x4BB5, 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 }

EFI_GUID  	mNetworkStackGuid = NETWORK_STACK_GUID;

extern BOOLEAN gEnterSetup;

VOID AMTBypassPasswordRestore(VOID);

extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_RUNTIME_SERVICES *gRT;


/**
    This function is called before AMITSE entry
    In the generic implementation setup password 
    is prompted in this function.

    @param VOID

    @retval VOID

**/
VOID AMTBypassPasswordRestore(VOID)
{

    EFI_STATUS Status;
    UINTN VariableSize;
    EFI_GUID gAmiTseSetupGuid = AMITSESETUP_GUID;
    AMITSESETUP AmiTseData;
    UINT32              Attributes = 0;

    VariableSize = sizeof(AMITSESETUP);
    Status = gRT->GetVariable ( L"AMITSESetupBackup", \
                                &gAmiTseSetupGuid, \
                                &Attributes, \
                                &VariableSize, \
                                &AmiTseData );

    // If we get this variable, restore it back
    if (!EFI_ERROR (Status)) {

        VariableSize = sizeof(AMITSESETUP);
        	gRT->SetVariable ( L"AMITSESetup", \
                                    &gAmiTseSetupGuid, \
                                    Attributes, \
                                    VariableSize, \
                                    &AmiTseData );

        VariableSize = 0;
        	gRT->SetVariable ( L"AMITSESetupBackup", \
                                    &gAmiTseSetupGuid, \
                                    Attributes, \
                                    VariableSize, \
                                    &AmiTseData );

    }

}

/**
    Regisetr callback at gAlertStandardFormatProtocolGuid
    If this function does not work, please make sure gAlertStandardFormatProtocolGuid is installed before AMITSE 
    If AMT Bypass user password, Delete AMITSESetup.

    @param VOID

        
    @retval VOID

**/
VOID AMTBypassPasswordCheck(
        IN EFI_EVENT Event,
        IN VOID *Context
)
{
    EFI_STATUS Status;
    UINTN VariableSize;
    EFI_GUID gAmiTseSetupGuid = AMITSESETUP_GUID;
    AMITSESETUP AmiTseData;

    UINTN VarSize=0;
    ALERT_STANDARD_FORMAT_PROTOCOL  *Asf;
    ASF_BOOT_OPTIONS                *mAsfBootOptions;
    EFI_GUID  gSetupGuid = SETUP_GUID;
    UINT32              Attributes = 0;
    
    
    mAsfBootOptions = NULL;
    //
    // Get Protocol for ASF, If locate fail, Please check gAlertStandardFormatProtocolGuid is installed before AMITSE or not
    //
    Status = gBS->LocateProtocol (
                    &gAlertStandardFormatProtocolGuid,
                    NULL,
                    &Asf
                    );
    if (EFI_ERROR (Status)) {
//      DEBUG ((EFI_D_ERROR, "Info : Error gettings ASF protocol -> %r\n", Status));
      return;
    }
    
    // Close event and continue AMTBypass check function.
    gBS->CloseEvent (Event);

    Status = Asf->GetBootOptions (Asf, &mAsfBootOptions);
    // Configuration data reset.
    if(mAsfBootOptions->BootOptions & 0x8000)
    {        
        // Delete Variable.
        VariableSize = 0;
        gRT->GetVariable ( L"Setup", \
                           &gSetupGuid, \
                           &Attributes, \
                           &VariableSize, \
                           NULL );  
        gRT->SetVariable(
                L"Setup",
                &gSetupGuid,
                Attributes,
                0,
                &VarSize);
        
        VariableSize = 0;
        gRT->GetVariable(L"NetworkStackVar", 
                         &mNetworkStackGuid, 
                         &Attributes, 
                         &VariableSize, 
                         NULL);

        gRT->SetVariable(
                L"NetworkStackVar",
                &mNetworkStackGuid,
                Attributes,
                0,
                &VarSize);
        
    }
    // ByPass Password
    if(mAsfBootOptions->BootOptions & 0x0800)
    {
        VariableSize = sizeof(AMITSESETUP);
        Status = gRT->GetVariable ( L"AMITSESetup", \
                                    &gAmiTseSetupGuid, \
                                    &Attributes, \
                                    &VariableSize, \
                                    &AmiTseData );

      // No Password installed just return
      if (Status == EFI_NOT_FOUND) {
        return;
      }
      // If exist , store a backup then delete AMITSESetup

        VariableSize = sizeof(AMITSESETUP);
        	gRT->SetVariable ( L"AMITSESetupBackup", \
                                    &gAmiTseSetupGuid, \
                                    Attributes, \
                                    VariableSize, \
                                    &AmiTseData );
        	gRT->SetVariable ( L"AMITSESetup", \
                                    &gAmiTseSetupGuid, \
                                    Attributes, \
                                    0, \
                                    &AmiTseData );

    }else
    {
        // If not bypass password , check AMITSESetupBackup variable
        AMTBypassPasswordRestore();
    }

    return;
}

EFI_STATUS AMTbypassEntryPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status;
    EFI_EVENT   AMTBypassEvt;
    VOID        *AMTBypassReg;
    
    //Create Event to do Bypass
    Status = gBS->CreateEvent(
        EFI_EVENT_NOTIFY_SIGNAL,
        TPL_CALLBACK, 
        AMTBypassPasswordCheck, 
        NULL, 
        &AMTBypassEvt
    );
    if (!EFI_ERROR(Status)) 
    Status = gBS->RegisterProtocolNotify(&gAlertStandardFormatProtocolGuid , AMTBypassEvt, &AMTBypassReg);
    ASSERT_EFI_ERROR (Status);
    AMTBypassPasswordCheck(AMTBypassEvt, NULL);

    return EFI_SUCCESS; 
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
