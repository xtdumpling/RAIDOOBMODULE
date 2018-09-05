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
/** @file MebxPrompt.c
    AMT TSE Functions.

**/
#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/AmtPolicy.h>
#include <Protocol/HeciProtocol.h>
#include <MeBiosExtensionSetup.h>
#include <MeBiosPayloadData.h>
#include <AmtSetup.h>
#include <Protocol/AmtWrapperProtocol.h>


#define MEBX_KeyShiftState1	LEFT_CONTROL_PRESSED | SHIFT_STATE_VALID
#define MEBX_KeyShiftState2	RIGHT_CONTROL_PRESSED | SHIFT_STATE_VALID
#define CIRA_KeyShiftState1	LEFT_CONTROL_PRESSED | SHIFT_STATE_VALID | LEFT_ALT_PRESSED
#define CIRA_KeyShiftState2	RIGHT_CONTROL_PRESSED | SHIFT_STATE_VALID | RIGHT_ALT_PRESSED

extern EFI_STATUS PostManagerDisplayPostMessage( CHAR16 *message );

extern EFI_BOOT_SERVICES      *gBS;
extern EFI_SYSTEM_TABLE       *gST;
extern EFI_RUNTIME_SERVICES   *gRT;
ME_BIOS_EXTENSION_SETUP       MeBiosExtensionSetupData;

EFI_HANDLE                    MebxKeyHandle[15];
EFI_HANDLE                    CIRAKeyHandle[15];
AMT_POLICY_PROTOCOL           *mDxeAmtPolicy = NULL;

#define EFI_EVENT_ME_PLATFORM_READY_TO_BOOT \
  { \
    0x3fdf171, 0x1d67, 0x4ace, 0xa9, 0x4, 0x3e, 0x36, 0xd3, 0x38, 0xfa, 0x74 \
  }
EFI_GUID      gEfiSimpleTextInExProtocolGuid = EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID;

EFI_GUID      gMePlatformReadyToBootGuid = EFI_EVENT_ME_PLATFORM_READY_TO_BOOT;
/**
    Set to GraphicMode

    @param Event 
    @param ParentImageHandle 

    @retval VOID

**/
EFI_STATUS
SetConsoleControlModeBack(
  EFI_EVENT   Event,
  VOID        *ParentImageHandle
)
{
  EFI_STATUS                        Status;
  EFI_CONSOLE_CONTROL_PROTOCOL      *ConsoleControl;

  Status = gBS->LocateProtocol( &gEfiConsoleControlProtocolGuid, NULL, &ConsoleControl);

 //Set back to Graphics
  Status = ConsoleControl->SetMode( ConsoleControl, EfiConsoleControlScreenGraphics );

  gBS->CloseEvent(Event);
  return Status;
}

/**

    @param Key 

    @retval EFI_STATUS Status

**/
EFI_STATUS CIRACheckForKey
(
  IN EFI_KEY_DATA *Key
)
{
#if CIRA_SCANCODE || CIRA_UNICODE
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEX;
  EFI_STATUS                        Status;
  UINTN                             HandleCount, i;
  EFI_HANDLE                        *HandleBuffer;  
  
  if(mDxeAmtPolicy == NULL)
  {    
    Status = gBS->LocateProtocol (&gDxeAmtPolicyGuid, NULL, &mDxeAmtPolicy);
    if (EFI_ERROR(Status)) return Status;
  }

  Status = gBS->HandleProtocol(gST->ConsoleInHandle, 
                              &gEfiSimpleTextInExProtocolGuid, 
                              (void*)&SimpleTextInEX);

  if (EFI_ERROR(Status)) return Status;

  HandleCount = 0;
  Status = gBS->LocateHandleBuffer (
            ByProtocol,
            &gEfiSimpleTextInExProtocolGuid,
            NULL, 
            &HandleCount,
            &HandleBuffer
            );
  for(i = 0 ; i < HandleCount ; i++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiSimpleTextInExProtocolGuid,
                    &SimpleTextInEX
                    );
    if(EFI_ERROR(Status))
        continue;
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[i*2]);  
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[(i*2) + 1]);
  }
  
  mDxeAmtPolicy->AmtConfig.CiraRequest = 1;
  PostManagerDisplayPostMessage(L"Requesting CIRA ......");
#endif
  return EFI_SUCCESS;
}
/**

    @param Key 

    @retval EFI_STATUS Status

**/
EFI_STATUS MebxCheckForKey
(
  IN EFI_KEY_DATA *Key
)
{
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEX;
  EFI_STATUS                        Status;
  EFI_CONSOLE_CONTROL_PROTOCOL      *ConsoleControl;
  EFI_EVENT                         SetConsoleControlModeBackEvent;
  EFI_CONSOLE_CONTROL_SCREEN_MODE   ScreenMode;
  UINTN                             HandleCount, i;
  EFI_HANDLE                        *HandleBuffer;  

  if(mDxeAmtPolicy == NULL) {    
    Status = gBS->LocateProtocol (&gDxeAmtPolicyGuid, NULL, &mDxeAmtPolicy);
    if (EFI_ERROR(Status)) return Status;
  }

  HandleCount = 0;
  Status = gBS->LocateHandleBuffer (
            ByProtocol,
            &gEfiSimpleTextInExProtocolGuid,
            NULL, 
            &HandleCount,
            &HandleBuffer
            );
  for(i = 0 ; i < HandleCount ; i++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiSimpleTextInExProtocolGuid,
                    &SimpleTextInEX
                    );
    if(EFI_ERROR(Status))
      continue;
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[i*4]);  
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[(i*4) + 1]);
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[(i*4) + 2]);
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[(i*4) + 3]);
  }
  mDxeAmtPolicy->AmtConfig.AmtbxHotkeyPressed = 1;
  PostManagerDisplayPostMessage(L"Entering MEBX setup menu ......");
  // Fix Mebx Setup screen not full screen.
  Status = gBS->LocateProtocol( &gEfiConsoleControlProtocolGuid, NULL, &ConsoleControl);

  ConsoleControl->GetMode(ConsoleControl, &ScreenMode, NULL, NULL);
  if (ScreenMode == EfiConsoleControlScreenGraphics) {
    ConsoleControl->SetMode( ConsoleControl, EfiConsoleControlScreenText );  
	  
    Status = gBS->CreateEventEx (
                    EFI_EVENT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    SetConsoleControlModeBack,
                    NULL,
                    &gMePlatformReadyToBootGuid,
                    &SetConsoleControlModeBackEvent
                    );
  }
  return EFI_SUCCESS;
}
/**
    This function is a hook called when TSE determines
    that SETUP utility has to be displayed. This function
    is available as ELINK. In the generic implementation
    setup password is prompted in this function.

    @param VOID

    @retval VOID

**/
BOOLEAN MebxPromptConInAvailabilityHook  (VOID)
{
#if MEBX_UNICODE>0x40 && MEBX_UNICODE<0x5b == 1
                      // ScanCode, UnicodeChar, KeyShiftState, KeyToggleState
  EFI_KEY_DATA      Key[] = {{MEBX_SCANCODE, MEBX_UNICODE, MEBX_KeyShiftState1, MEBX_KeyToggleState},
                             {MEBX_SCANCODE, MEBX_UNICODE+0x20, MEBX_KeyShiftState1, MEBX_KeyToggleState},
                             {MEBX_SCANCODE, MEBX_UNICODE, MEBX_KeyShiftState2, MEBX_KeyToggleState},
                             {MEBX_SCANCODE, MEBX_UNICODE+0x20, MEBX_KeyShiftState2, MEBX_KeyToggleState}};
#else
#if MEBX_UNICODE>0x60 && MEBX_UNICODE<0x7b == 1
  EFI_KEY_DATA      Key[] = {{MEBX_SCANCODE, MEBX_UNICODE-0x20, MEBX_KeyShiftState1, MEBX_KeyToggleState},
                             {MEBX_SCANCODE, MEBX_UNICODE, MEBX_KeyShiftState1, MEBX_KeyToggleState},
                             {MEBX_SCANCODE, MEBX_UNICODE-0x20, MEBX_KeyShiftState2, MEBX_KeyToggleState},
                             {MEBX_SCANCODE, MEBX_UNICODE, MEBX_KeyShiftState2, MEBX_KeyToggleState}};
#else
  EFI_KEY_DATA      Key[] = {{MEBX_SCANCODE, MEBX_UNICODE, MEBX_KeyShiftState, MEBX_KeyToggleState}};
#endif
#endif
  EFI_STATUS                        Status;
  HECI_PROTOCOL                     *Heci;
  UINT32                            MeMode = ME_MODE_FAILED;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEX;
  UINTN                             HandleCount, i;
  EFI_HANDLE                        *HandleBuffer;    

  if(mDxeAmtPolicy == NULL) {    
    Status = gBS->LocateProtocol (&gDxeAmtPolicyGuid, NULL, &mDxeAmtPolicy);
    if (EFI_ERROR(Status))
      return FALSE;
  }

  Status = pBS->LocateProtocol (
                  &gHeciProtocolGuid,
                  NULL,
                  &Heci
                  );
  if (EFI_ERROR(Status)) return EFI_SUCCESS; 

  // If not normal mode
  Status = Heci->GetMeMode (&MeMode);
  if(MeMode != 0)
    return FALSE;

  if(mDxeAmtPolicy->AmtConfig.AmtbxHotkeyPressed == 1) {
    EFI_EVENT        SetConsoleControlModeBackEvent;
    EFI_CONSOLE_CONTROL_PROTOCOL      *ConsoleControl;
    EFI_CONSOLE_CONTROL_SCREEN_MODE   ScreenMode;

    // Fix Mebx Setup screen not full screen.
    Status = gBS->LocateProtocol( 
             &gEfiConsoleControlProtocolGuid, NULL, &ConsoleControl);

    ConsoleControl->GetMode(ConsoleControl, &ScreenMode, NULL, NULL);
    if (ScreenMode == EfiConsoleControlScreenGraphics) {
      Status = gBS->CreateEventEx (
                      EFI_EVENT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      SetConsoleControlModeBack,
                      NULL,
                      &gMePlatformReadyToBootGuid,
                      &SetConsoleControlModeBackEvent
                      );
    }
    return FALSE;
  }

  HandleCount = 0;
  Status = gBS->LocateHandleBuffer (
            ByProtocol,
            &gEfiSimpleTextInExProtocolGuid,
            NULL, 
            &HandleCount,
            &HandleBuffer
            );

  for(i = 0 ; i < HandleCount ; i++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiSimpleTextInExProtocolGuid,
                    &SimpleTextInEX
                    );
    if(EFI_ERROR(Status))
      continue;
    Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[0], &MebxCheckForKey, &MebxKeyHandle[i*4]);
    Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[1], &MebxCheckForKey, &MebxKeyHandle[i*4 +1]);
    Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[2], &MebxCheckForKey, &MebxKeyHandle[i*4 +2]);
    Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[3], &MebxCheckForKey, &MebxKeyHandle[i*4 +3]);
  }

  PostManagerDisplayPostMessage(L"Press <CTRL + P> to Enter MEBX setup menu ");
    
  return FALSE;
}
/**
    This function is a hook called when TSE determines
    that SETUP utility has to be displayed. This function
    is available as ELINK. In the generic implementation
    setup password is prompted in this function.

    @param VOID

    @retval VOID

**/
BOOLEAN RemoteAssistConInAvailabilityHook  (VOID)
{
#if CIRA_SUPPORT
  EFI_STATUS                        Status;
  UINTN                             VariableSize;
  AMT_WRAPPER_PROTOCOL              *AmtWrapperProtocol;
  UINTN                             HandleCount, i;
  EFI_HANDLE                        *HandleBuffer;

  Status = gBS->LocateProtocol (&gDxeAmtPolicyGuid, NULL, &mDxeAmtPolicy);
  if (EFI_ERROR(Status)) return FALSE;

  Status = gBS->LocateProtocol (&gEfiAmtWrapperProtocolGuid, NULL, &AmtWrapperProtocol);
  if (EFI_ERROR(Status)) return FALSE;

  VariableSize = sizeof (ME_BIOS_EXTENSION_SETUP);
  Status = gRT->GetVariable (
           ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME,
           &gMeBiosExtensionSetupGuid,
           NULL,
           &VariableSize,
           &MeBiosExtensionSetupData);

  if(EFI_ERROR(Status))
    return FALSE;

  if((MeBiosExtensionSetupData.RemoteAssistanceTriggerAvailablilty) &&
     (mDxeAmtPolicy->AmtConfig.AmtEnabled) &&
     (!AmtWrapperProtocol->ActiveManagementEnableKvm()) &&
     (!AmtWrapperProtocol->ActiveManagementEnableSol())) {
#if CIRA_UNICODE>0x40 && CIRA_UNICODE<0x5b == 1
                    // ScanCode, UnicodeChar, KeyShiftState, KeyToggleState
    EFI_KEY_DATA      Key[] = {{CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState, CIRA_KeyToggleState},
                             {CIRA_SCANCODE, CIRA_UNICODE+0x20, CIRA_KeyShiftState, CIRA_KeyToggleState}};

#else
#if CIRA_UNICODE>0x60 && CIRA_UNICODE<0x7b == 1
                    // ScanCode, UnicodeChar, KeyShiftState, KeyToggleState
    EFI_KEY_DATA      Key[] = {{CIRA_SCANCODE, CIRA_UNICODE-0x20, CIRA_KeyShiftState, CIRA_KeyToggleState},
                             {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState, CIRA_KeyToggleState}};
#else
    EFI_KEY_DATA      Key[] = {{CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState1, CIRA_KeyToggleState},
                               {CIRA_SCANCODE, CIRA_UNICODE, CIRA_KeyShiftState2, CIRA_KeyToggleState}};
#endif
#endif
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEX;

    HandleCount = 0;
    Status = gBS->LocateHandleBuffer (
              ByProtocol,
              &gEfiSimpleTextInExProtocolGuid,
              NULL, 
              &HandleCount,
              &HandleBuffer
              );

    for(i = 0 ; i < HandleCount ; i++) {
      Status = gBS->HandleProtocol (
                     HandleBuffer[i],
                     &gEfiSimpleTextInExProtocolGuid,
                     &SimpleTextInEX
                     );
      if(EFI_ERROR(Status))
        continue;

      Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[0], &CIRACheckForKey, &CIRAKeyHandle[i*2]);
      Status = SimpleTextInEX->RegisterKeyNotify (SimpleTextInEX, &Key[1], &CIRACheckForKey, &CIRAKeyHandle[i*2 +1]);
    }
    PostManagerDisplayPostMessage(L"Press <CTRL + ALT + F1> Remote Assistance");
  }
#endif //CIRA_SUPPORT
  return FALSE;
}
/**
    This function is a hook called when TSE determines
    that it has to load the boot options in the boot
    order. This function is available as ELINK.

    @param VOID

    @retval VOID

**/
VOID MEProcessEnterSetup(VOID)
{
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEX;
  EFI_STATUS                        Status;
  UINTN                             HandleCount, i;
  EFI_HANDLE                        *HandleBuffer;  

  HandleCount = 0;
  Status = gBS->LocateHandleBuffer (
            ByProtocol,
            &gEfiSimpleTextInExProtocolGuid,
            NULL, 
            &HandleCount,
            &HandleBuffer
            );

  for(i = 0 ; i < HandleCount ; i++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiSimpleTextInExProtocolGuid,
                    &SimpleTextInEX
                    );
    if(EFI_ERROR(Status))
      continue;
#if CIRA_SUPPORT
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[i*2]);  
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, CIRAKeyHandle[(i*2) + 1]);
#endif
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[i*4]);  
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[(i*4) + 1]);
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[(i*4) + 2]);
    Status = SimpleTextInEX->UnregisterKeyNotify (SimpleTextInEX, MebxKeyHandle[(i*4) + 3]);
  }
}
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
