//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file FastBootTseHook.c
 *  Implementation of suppress of TSE "Press DEL..." message.
 */

///============================================================================
/// Includes
///============================================================================
#include <Efi.h>
#include <Setup.h>
#include <Protocol/FastBootProtocol2.h>
#include <FastBoot2.h>
#include "AMITSEStrTokens.h"

/// Define
#define    BOOT_FLOW_CONDITION_NORMAL    0
#if FASTBOOT_NEED_RESTART
static EFI_GUID gFastbootRestartGuid = FAST_BOOT_RESTART_GUID;
static EFI_GUID gFastbootRestartCountGuid = FAST_BOOT_RESTART_COUNT_GUID;
#endif

/// External Global Variable Declaration

extern EFI_RUNTIME_SERVICES *gRT;
extern EFI_BOOT_SERVICES     *gBS;
extern UINT32 gBootFlow;
extern BOOLEAN gEnterSetup;
extern EFI_EVENT gKeyTimer;
extern CHAR16 *HiiGetString( VOID* handle, UINT16 token );
extern EFI_HII_HANDLE  gHiiHandle;

/// External Function Definitions

EFI_STATUS 
EFIAPI 
PostManagerDisplayPostMessage( CHAR16 *message );

BOOLEAN 
EFIAPI 
ProcessConInAvailability(VOID);

VOID 
EFIAPI 
BbsBoot(VOID);

VOID 
EFIAPI 
TSEIDEPasswordFreezeDevices();///(EIP68329)+

VOID 
EFIAPI 
CheckForKeyHook( EFI_EVENT Event, VOID *Context );

EFI_STATUS 
EFIAPI 
TimerStopTimer( EFI_EVENT *Event );    

/// Global Variable Declaration

FAST_BOOT_POLICY2    *gFastBootPolicy;
FAST_BOOT_TSE_PROTOCOL2 gFastBootTseProtocol = {
    FastBootCheckForKey,
    FastBootStopCheckForKeyTimer,
    TSEIDEPasswordFreezeDevices         ///(EIP68329)++
};
                                        ///(EIP63924+)>
/**
 * This function check TSE variable, gEnterSetup and gBootFlow.
 *    
 * @param[in]   EnterSetup          Enter to setup flag. 
 * @param[in]   BootFlow            Boot flow data.
 *
 * @retval EFI_SUCCESS              BootFlow is changed or EnterSetup if true.
 * @retval EFI_NOT_READY            BootFlow is not changed and EnterSetup if false.
 */

EFI_STATUS 
EFIAPI 
FastBootCheckForKey ( 
  IN BOOLEAN *EnterSetup,
  IN UINT32  *BootFlow )
{
    CheckForKeyHook( (EFI_EVENT)NULL, NULL );

    *EnterSetup = gEnterSetup;
    *BootFlow = gBootFlow;
    
    if ((gEnterSetup) || (gBootFlow != BOOT_FLOW_CONDITION_NORMAL)){
        return EFI_SUCCESS;
    }
    else { 
        return EFI_NOT_READY;
    }
}

/**
 * This function stop the timer of CheckForKey callback.
 *    
 */

EFI_STATUS 
EFIAPI 
FastBootStopCheckForKeyTimer()
{
    return TimerStopTimer(&gKeyTimer);
}

/**
 * Function that will be called when enter TSE DXE entry.
 *    
 */

VOID 
EFIAPI 
FastBootMinisetupDriverEntryHook(VOID)
{
    EFI_HANDLE Handle = NULL;            
    EFI_GUID   FastBootTseGuid = FAST_BOOT_TSE_PROTOCOL2_GUID; 
    EFI_GUID   FastBootPolicyGuid = FAST_BOOT_POLICY_PROTOCOL2_GUID;
    
    gBS->InstallProtocolInterface(&Handle,
                              &FastBootTseGuid,
                              EFI_NATIVE_INTERFACE,
                              &gFastBootTseProtocol);

    gBS->LocateProtocol(&FastBootPolicyGuid, NULL, (VOID**)&gFastBootPolicy);
        
} 
                                        ///<(EIP63924+)
#if FASTBOOT_NEED_RESTART                        

/**
 * Initialize reboot count before enter to setup.
 *    
 */

VOID 
EFIAPI 
InitialRebootCount(VOID)
{
    UINTN RebootFlag = 1;
    UINTN RebootFlagSize = sizeof(RebootFlag);
    UINTN ClearCount = 0;
    UINTN ClearCountSize = sizeof(ClearCount); 
    
    gRT->SetVariable(FAST_BOOT_RESTART_COUNT, 
                &gFastbootRestartCountGuid, 
                EFI_VARIABLE_NON_VOLATILE |
                EFI_VARIABLE_BOOTSERVICE_ACCESS |
                EFI_VARIABLE_RUNTIME_ACCESS,
                ClearCountSize,
                &ClearCount);

    gRT->SetVariable(FAST_BOOT_RESTART, 
                &gFastbootRestartGuid, 
                EFI_VARIABLE_NON_VOLATILE |
                EFI_VARIABLE_BOOTSERVICE_ACCESS |
                EFI_VARIABLE_RUNTIME_ACCESS,
                RebootFlagSize,
                &RebootFlag);

}
#endif

/**
 * Function that will be called instead of generic TSE callback on Console Input device is installed event.
 *    
 *
 * @retval TRUE                    Screen was used to ask password.
 * @retval FALSE                Screen was not used to ask password
 */

BOOLEAN 
EFIAPI 
FastBootConInAvailHook(VOID)
{

    if (gFastBootPolicy->FastBootEnable == 0 || gFastBootPolicy->CheckPassword == 1) 
        return ProcessConInAvailability();

#if SETUP_PRINT_ENTER_SETUP_MSG 
{
        CHAR16 *Text;
        
        Text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_DEL_ENTER_SETUP) );
        if ( Text != NULL )
            PostManagerDisplayPostMessage(Text);
        gBS->FreePool(Text);
}
#endif

    return FALSE;
}

/**
 * Function that will be called instead of generic TSE callback on BBS Popup boot path.
 *    
 *
 */

VOID 
EFIAPI 
FastBootBbsBootHook(VOID)
{
    static EFI_GUID FastBootVariableGuid = FAST_BOOT_VARIABLE_GUID;
    UINT32     BbsPopupCalled = 0x55aa55aa;
    
    gRT->SetVariable(L"BbsPopupCalled", 
                              &FastBootVariableGuid, 
                              EFI_VARIABLE_BOOTSERVICE_ACCESS,
                              sizeof(BbsPopupCalled),
                              &BbsPopupCalled);
    BbsBoot();
}

#if OVERRIDE_FastBootLaunch

/**
 *Function that will be called instead of generic TSE callback on fast boot path.
 *    
 *
 * @retval EFI_NOT_FOUND        gFastBootProtocolGuid protocol not found.
 */

EFI_STATUS 
EFIAPI 
FastBootLaunch()
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    AMI_FAST_BOOT_PROTOCOL2 * FastBootProtocol = NULL;

    // do the Fast Boot
    if (!EFI_ERROR(gBS->LocateProtocol(&gAmiFastBootProtocol2Guid, NULL, (VOID**)&FastBootProtocol)))
        Status = FastBootProtocol->Launch();

    // If gFastBootProtocolGuid protocol not found or FastBoot Failed. return
    return Status;
}
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
