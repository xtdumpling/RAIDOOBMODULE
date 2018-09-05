/**@file

@copyright
 Copyright (c) 2009 - 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/
#include "Library/MeTypeLib.h"
#include <Guid/HiiSetupEnter.h>
#include "AmtWrapperDxe.h"

// APTIOV_SERVER_OVERRIDE_RC_START
#include <Protocol/Security2.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/BlockIo.h>
#include "Protocol/AmtReadyToBoot.h"
#include "Amt.h"
#if defined(Terminal_SUPPORT) && (Terminal_SUPPORT == 1)
#include <Setup.h>
#include <TerminalSetupVar.h>

static EFI_GUID   gSetupGuid = SETUP_GUID;
#endif

//for USBdiskInfo
EFI_STATUS EFIAPI InstallUSBDummyDiskInfo (VOID);
AMT_READY_TO_BOOT_PROTOCOL      mUSBDummyDiskInfo = {
  AMT_READY_TO_BOOT_PROTOCOL_REVISION_1,
  InstallUSBDummyDiskInfo
};
// APTIOV_SERVER_OVERRIDE_RC_END

typedef struct {
  EFI_INPUT_KEY        Key;
  UINT32               KeyShiftState;
} ME_HOT_KEY_TABLE;

// APTIOV_SERVER_OVERRIDE_RC_START
#define MAX_BBS_ENTRIES 0x100
// APTIOV_SERVER_OVERRIDE_RC_END

#define NUMBER_OF_HOTKEY_CTRL_P      6
#define NUMBER_OF_HOTKEY_ALL         10

GLOBAL_REMOVE_IF_UNREFERENCED ME_HOT_KEY_TABLE mMeHotKeyTables[] = {
  // Ctrl + P
  { {CHAR_NULL, 0x10}, EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED},
  { {CHAR_NULL, 0x10}, EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED},
  { {CHAR_NULL, L'p'}, EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED},
  { {CHAR_NULL, L'p'}, EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED},
  { {CHAR_NULL, L'P'}, EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED},
  { {CHAR_NULL, L'P'}, EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED},

  // Ctrl + Alt + F1
  { {SCAN_F1, CHAR_NULL}, EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED | EFI_RIGHT_ALT_PRESSED},
  { {SCAN_F1, CHAR_NULL}, EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED | EFI_LEFT_ALT_PRESSED},
  { {SCAN_F1, CHAR_NULL}, EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED | EFI_RIGHT_ALT_PRESSED},
  { {SCAN_F1, CHAR_NULL}, EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED | EFI_LEFT_ALT_PRESSED}
};

GLOBAL_REMOVE_IF_UNREFERENCED AMT_WRAPPER_PROTOCOL AmtWrapperInstance = {
                                        AmtWrapperInit,
                                        AmtWrapperGet,
                                        AmtWrapperSet,
                                        AmtWrapperEnableSol,
                                        AmtWrapperEnableStorageRedir,
                                        AmtWrapperPauseBoot,
                                        AmtWrapperEnterSetup,
                                        AmtWrapperBootOptionExist,
                                        AmtWrapperEnableSecureErase,
                                        AmtWrapperBdsBootViaAsf,
                                        ActiveManagementConsoleLocked,
                                        AmtWrapperEnableKvm,
                                        GetSetupPrompt,
                                        DetermineSetupHotKey,
                                        GetSupportedHotkeys
                                  };

extern ACTIVE_MANAGEMENT_PROTOCOL       *mActiveManagement;
extern ASF_BOOT_OPTIONS                 *mAsfBootOptions;


//
// Protocol GUID definition
//
GLOBAL_REMOVE_IF_UNREFERENCED UINTN gEnterRemoteAssistance = 0;
GLOBAL_REMOVE_IF_UNREFERENCED UINTN gEnterIntelMeSetup = 0;
extern AMT_POLICY_PROTOCOL    *mDxeAmtPolicy;
GLOBAL_REMOVE_IF_UNREFERENCED ME_BIOS_EXTENSION_SETUP                 MeBiosExtensionSetup;
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN                                 MeBiosExtensionSetupReady = FALSE;
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                                   mFwImageType;

GLOBAL_REMOVE_IF_UNREFERENCED PLATFORM_PCI_SERIAL_OVER_LAN_DEVICE_PATH gSerialOverLanDevicePath = {
            gPciRootBridge,
            {
                HARDWARE_DEVICE_PATH,
                HW_PCI_DP,
                (UINT8)(sizeof(PCI_DEVICE_PATH)),
                (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8),
                SOL_FUNCTION_NUMBER,
                ME_DEVICE_NUMBER
            },
            {
                MESSAGING_DEVICE_PATH,
                MSG_UART_DP,
                (UINT8)(sizeof(UART_DEVICE_PATH)),
                (UINT8)((sizeof(UART_DEVICE_PATH)) >> 8),
                0,
                115200,
                8,
                1,
                1
            },
            {
                MESSAGING_DEVICE_PATH,
                MSG_VENDOR_DP,
                (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
                (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8),
                DEVICE_PATH_MESSAGING_PC_ANSI
            },
            gEndEntire
        };

GLOBAL_REMOVE_IF_UNREFERENCED EFI_HANDLE mBdsImageHandle;
GLOBAL_REMOVE_IF_UNREFERENCED UINT16 gPlatformBootTimeOutDefault = 1;


GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING mSetupPrompt[] = {
    L"Press [CTRL+P] to enter the MEBx Setup Menu.",
    L"Press [CTRL+ALT+F1] to enter MEBx Remote Assistance."
  };

GLOBAL_REMOVE_IF_UNREFERENCED EFI_STRING mSetupPromptSelectionMenu[] = {
    L"Press [CTRL+P] to enter the MEBx Setup Menu."
  };

GLOBAL_REMOVE_IF_UNREFERENCED UINT16 NullPrompt[] = {
    0
  };

//
// Driver entry point
//

//
// Function implementations
//

#define EFI_CONTROL_PRESSED         (EFI_RIGHT_CONTROL_PRESSED + EFI_LEFT_CONTROL_PRESSED)
#define EFI_ALT_PRESSED             (EFI_RIGHT_ALT_PRESSED + EFI_LEFT_ALT_PRESSED)

BOOLEAN
IsCtrlKey(UINT32 KeyShiftState)
{
  BOOLEAN b = FALSE;

  if ((KeyShiftState & EFI_SHIFT_STATE_VALID) && (KeyShiftState & EFI_CONTROL_PRESSED))
    b = TRUE;

  return b;
}


BOOLEAN
IsAltKey(UINT32 KeyShiftState)
{
  BOOLEAN b = FALSE;

  if ((KeyShiftState & EFI_SHIFT_STATE_VALID) && (KeyShiftState & EFI_ALT_PRESSED))
    b = TRUE;

  return b;
}

BOOLEAN
IsPKey(CHAR16 UnicodeChar)
{
  BOOLEAN b = FALSE;

  if (UnicodeChar == L'p' || UnicodeChar == L'P')
    b = TRUE;

  return b;
}

VOID
LocateSetupAndPolicyData()
{
  EFI_STATUS                Status;
  UINTN                     VariableSize;

  if (mDxeAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    return;
  }

  if (MeBiosExtensionSetupReady == FALSE) {
    VariableSize = sizeof(MeBiosExtensionSetup);
    Status = gRT->GetVariable (
                    ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME,
                    &gMeBiosExtensionSetupGuid,
                    NULL,
                    &VariableSize,
                    &MeBiosExtensionSetup
                    );
    if (!EFI_ERROR(Status))
      MeBiosExtensionSetupReady = TRUE;
  }
}

VOID
EnterIntelMeSetup() {
  if (mDxeAmtPolicy == NULL)
  {
    return;
  }
  mDxeAmtPolicy->AmtConfig.AmtbxHotkeyPressed = 1;
  mDxeAmtPolicy->AmtConfig.CiraRequest = 0;
  DEBUG((DEBUG_ERROR, "Entering Intel ME Setup\n"));
}

VOID
EnterRemoteAssistance() {
  if (mDxeAmtPolicy == NULL)
  {
    return;
  }
  mDxeAmtPolicy->AmtConfig.AmtbxHotkeyPressed = 1;
  mDxeAmtPolicy->AmtConfig.CiraRequest = 1;
  DEBUG((DEBUG_ERROR, "Entering Remote Assistance\n"));
}

BOOLEAN
DisplayRemoteAssistancePrompt() {
  BOOLEAN st = FALSE;

  DEBUG((DEBUG_ERROR, "<DisplayRemoteAssistancePrompt>" ));
  if ((mDxeAmtPolicy != NULL) && (MeBiosExtensionSetupReady == TRUE)) {
    DEBUG((DEBUG_ERROR, "<AmtbxSelectionScreen=%d>", mDxeAmtPolicy->AmtConfig.AmtbxSelectionScreen));
    DEBUG((DEBUG_ERROR, "<RemoteAssistanceTriggerAvailablilty=%d>", MeBiosExtensionSetup.RemoteAssistanceTriggerAvailablilty));
    if ((mDxeAmtPolicy->AmtConfig.AmtbxSelectionScreen == 0) &&
        (MeBiosExtensionSetup.RemoteAssistanceTriggerAvailablilty == 1)) {
      st = TRUE;
    } else {
      st = FALSE;
    }
  }

  DEBUG((DEBUG_ERROR, "<st=%d>", st));

  return st;
}

BOOLEAN
IsBiosHotkeyEnabled(
)
{
  BOOLEAN                                 st = FALSE;
  EFI_STATUS  Status;
  HECI_PROTOCOL                          *Heci;
  UINT32                                  MeMode;

  Heci = NULL;
  MeMode = ME_MODE_TEMP_DISABLED;

  Status = gBS->LocateProtocol (&gHeciProtocolGuid, NULL, &Heci);
  if (!EFI_ERROR(Status)) {
    Heci->GetMeMode(&MeMode);
  } else {
    return FALSE;
  }

  if ((mFwImageType != INTEL_ME_CONSUMER_FW) && (MeMode == ME_MODE_NORMAL)) {
    st = TRUE;
  }

  return st;
}


EFI_STATUS
GetSetupPrompt (
  IN OUT  UINTN     *PromptIndex,
  OUT     CHAR16    **String
)
{
  EFI_STATUS     Status;

  if (PromptIndex == NULL || String == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;
  *String = NULL;
  InitAmtWrapperLib();
  LocateSetupAndPolicyData ();

  if(!ActiveManagementEnableSol() && !ActiveManagementEnableKvm())
    if (IsBiosHotkeyEnabled ()) {
      if (DisplayRemoteAssistancePrompt ()) {
        if (*PromptIndex < sizeof (mSetupPrompt) / sizeof (EFI_STRING)) {
          *String =  mSetupPrompt[*PromptIndex];
          Status = EFI_SUCCESS;
          (*PromptIndex)++;
        }
      } else {
        if (*PromptIndex < sizeof (mSetupPromptSelectionMenu) / sizeof (EFI_STRING)) {
          *String =  mSetupPromptSelectionMenu[*PromptIndex];
          Status = EFI_SUCCESS;
          (*PromptIndex)++;
        }
      }
    }
  return Status;
}

// Ctrl+ALT+F1  : Scan    = 0x0B, Shift = 0x3C
// Ctrl+P       : Unicode = 0x10/L'p', Shift = 0x0C

EFI_STATUS
DetermineSetupHotKey(
  IN EFI_KEY_DATA                       *KeyData
)
{
  EFI_STATUS Status = EFI_NOT_FOUND;

  //
  // React to hotkey once per boot only
  //
  if (gEnterIntelMeSetup || gEnterRemoteAssistance) {
    return Status;
  }

  InitAmtWrapperLib();
  LocateSetupAndPolicyData();
  if(!ActiveManagementEnableSol() && !ActiveManagementEnableKvm())
    if (IsBiosHotkeyEnabled()) {
      if ((KeyData->Key.UnicodeChar == 0x10 || IsPKey (KeyData->Key.UnicodeChar)) && IsCtrlKey (KeyData->KeyState.KeyShiftState)) {
        EnterIntelMeSetup();
        gEnterIntelMeSetup = 1;
        Status = EFI_SUCCESS;
      } else if ((DisplayRemoteAssistancePrompt() == TRUE) &&
                 (KeyData->Key.ScanCode == SCAN_F1) && IsCtrlKey(KeyData->KeyState.KeyShiftState) && IsAltKey(KeyData->KeyState.KeyShiftState)) {
        EnterRemoteAssistance();
        gEnterRemoteAssistance = 1;
        Status = EFI_SUCCESS;
      }

      DEBUG((DEBUG_ERROR, "\n"));
      DEBUG((DEBUG_ERROR, "<Scan=0x%X>", KeyData->Key.ScanCode));
      DEBUG((DEBUG_ERROR, "<Unicode=0x%X>", KeyData->Key.UnicodeChar));
      DEBUG((DEBUG_ERROR, "<ShiftState=0x%X>", KeyData->KeyState.KeyShiftState));
      DEBUG((DEBUG_ERROR, "<ToggleState=0x%X>", KeyData->KeyState.KeyToggleState));
      DEBUG((DEBUG_ERROR, "<gEnterIntelMeSetup=0x%X>", gEnterIntelMeSetup));
      DEBUG((DEBUG_ERROR, "<gEnterRemoteAssistance=0x%X>", gEnterRemoteAssistance));
    }
  return Status;
}


EFI_STATUS
GetSupportedHotkeys (
  OUT  UINTN             *KeyCnt,
  OUT  EFI_KEY_DATA      **KeyData
  )
{
  EFI_STATUS     Status;
  UINTN          Index;

  Status = EFI_NOT_FOUND;
  LocateSetupAndPolicyData();
  *KeyCnt = 0;
  *KeyData = NULL;

  if (IsBiosHotkeyEnabled ()) {
    if (DisplayRemoteAssistancePrompt ()) {
      //
      // CTRL + P and CTRL + ALT + F1 hotkey
      //
      *KeyCnt = NUMBER_OF_HOTKEY_ALL;
    } else {
      //
      // CTRL + P hotkey
      //
      *KeyCnt = NUMBER_OF_HOTKEY_CTRL_P;
    }
    *KeyData = AllocateZeroPool (sizeof (EFI_KEY_DATA) * (*KeyCnt));
    if (*KeyData == NULL){
      return EFI_OUT_OF_RESOURCES;
    }
    for (Index = 0; Index < *KeyCnt; Index++) {
      (*KeyData)[Index].Key.ScanCode = mMeHotKeyTables[Index].Key.ScanCode;
      (*KeyData)[Index].Key.UnicodeChar = mMeHotKeyTables[Index].Key.UnicodeChar;
      (*KeyData)[Index].KeyState.KeyShiftState = mMeHotKeyTables[Index].KeyShiftState;
    }

    Status = EFI_SUCCESS;
  }

  return Status;
}

VOID
AsfWatchDogStopOnSetupEntry (
  IN EFI_EVENT Event,
  IN VOID *Context
  )
{
  AmtWrapperSet (SET_WDT_STOP, 0);
}

// APTIOV_SERVER_OVERRIDE_RC_START
#if defined(Terminal_SUPPORT) && (Terminal_SUPPORT == 1)
/**
    Enable/Disable SOL COM.

    @param Enabled 

    @retval VOID


**/
VOID
SetSOLCOMEnable(
    IN BOOLEAN         Enabled
)
{
    UINT32      ComPort;
    UINTN       SetupDataSize;
    SETUP_DATA  gSetupData;
    EFI_STATUS  Status;
    UINT32              Attributes = 0;
#if (TOTAL_PCI_SERIAL_PORTS > 0)
    UINT32      i = 0;
    EFI_GUID    gTerminalVarGuid   = TERMINAL_VAR_GUID;
    UINTN       PciSerialPortsLocationVarSize = 
                                    sizeof(PCI_SERIAL_PORTS_LOCATION_VAR);
    UINT32      PciSerialPortsLocationVarAttributes=0;
    PCI_SERIAL_PORTS_LOCATION_VAR PciSerialPortsLocationVar; 
    UINT32 gTotalSioSerialPorts = TOTAL_SIO_SERIAL_PORTS;
    Status = gRT->GetVariable(L"PciSerialPortsLocationVar", 
                                &gTerminalVarGuid, 
                                &PciSerialPortsLocationVarAttributes, 
                                &PciSerialPortsLocationVarSize, 
                                &PciSerialPortsLocationVar);

    if(EFI_ERROR(Status)) {
        ComPort = gTotalSioSerialPorts;
    }
    ComPort = gTotalSioSerialPorts; 

    for (i = 0; i < TOTAL_PCI_SERIAL_PORTS; i++) {
        if ((PciSerialPortsLocationVar.Device[i] == SOL_DEVICE_NUMBER) && 
            (PciSerialPortsLocationVar.Function[i] == SOL_FUNCTION_NUMBER)) {
                ComPort = gTotalSioSerialPorts+i; 
                break;            
        }
    }	
#else 
    return;
#endif
    SetupDataSize = sizeof(SETUP_DATA);
    Status = gRT->GetVariable ( L"Setup", \
                                &gSetupGuid, \
                                &Attributes, \
                                &SetupDataSize, \
                                &gSetupData );

    if(Enabled)
     {
         gSetupData.ConsoleRedirectionEnable[ComPort] = 1;
         gSetupData.ComPortforLegacy = ComPort;
     }
     else
     {
         gSetupData.ConsoleRedirectionEnable[ComPort] = 0;
         gSetupData.ComPortforLegacy = 0;
         
     }

    Status = gRT->SetVariable ( L"Setup", \
                                &gSetupGuid, \
                                Attributes, \
                                SetupDataSize, \
                                &gSetupData );
    
}
#endif

EFI_STATUS EmptyFileAuthentication (
    IN CONST EFI_SECURITY2_ARCH_PROTOCOL *This,
    IN CONST EFI_DEVICE_PATH_PROTOCOL *File,
    IN VOID *FileBuffer,
    IN UINTN FileSize,
    IN BOOLEAN BootPolicy
)
{
    if (File==NULL) return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}    
//for USBdiskInfo>>
EFI_STATUS
DiskInfoInquiry (
        IN EFI_DISK_INFO_PROTOCOL   *This,
        IN OUT VOID                 *InquiryData,
        IN OUT UINT32               *InquiryDataSize
    	)
{
	    return EFI_UNSUPPORTED;
}

EFI_STATUS
DiskInfoIdentify (
        EFI_DISK_INFO_PROTOCOL      *This,
        IN OUT VOID                 *IdentifyData,
        IN OUT UINT32               *IdentifyDataSize
)
{
	    return EFI_UNSUPPORTED;
}

EFI_STATUS
DiskInfoSenseData (
        EFI_DISK_INFO_PROTOCOL      *This,
        VOID                        *SenseData,
        UINT32                      *SenseDataSize,
        UINT8                       *SenseDataNumber
)
{
	    return EFI_UNSUPPORTED;
}

EFI_STATUS
DiskInfoWhichIDE (
        IN EFI_DISK_INFO_PROTOCOL   *This,
        OUT UINT32                  *IdeChannel,
        OUT UINT32                  *IdeDevice
)
{
	    return EFI_UNSUPPORTED;
}
// For USB diskinfo

EFI_STATUS
EFIAPI
InstallUSBDummyDiskInfo (
  VOID
  )
{
	  EFI_STATUS             Status;
	  UINTN                     Index;
	  EFI_DISK_INFO_PROTOCOL    *DiskInfo;
	  EFI_BLOCK_IO_PROTOCOL     *BlkIo;
	  UINTN                     HandleCount;
	  EFI_HANDLE                *HandleBuffer;
	  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
	  EFI_DEVICE_PATH_PROTOCOL  *TmpDevicePathNode; 
	  UINT8			    ParentPortNumber;
	  UINT8			    InterfaceNumber;  


	      Status = gBS->LocateHandleBuffer (
	                       ByProtocol,
	                       &gEfiBlockIoProtocolGuid,
	                       NULL,
	                       &HandleCount,
	                       &HandleBuffer
	                       );

	       if (EFI_ERROR (Status) || HandleCount == 0) {
	         return Status;
	       }

	       for (Index = 0; Index < HandleCount; Index++) {
	         Status = gBS->HandleProtocol (
	                         HandleBuffer[Index],
	                         &gEfiBlockIoProtocolGuid,
	                         (VOID **) &BlkIo
	                         );
	         ASSERT_EFI_ERROR (Status);

	         Status = gBS->HandleProtocol (
	                         HandleBuffer[Index],
	                         &gEfiDevicePathProtocolGuid,
	                         (VOID *) &DevicePath
	                         );
	         ASSERT_EFI_ERROR (Status);      
	      
	          TmpDevicePathNode = DevicePath;
	          DiskInfo = NULL;
	          while (!IsDevicePathEnd (TmpDevicePathNode)) {   
	            if(TmpDevicePathNode->Type == MESSAGING_DEVICE_PATH) {
	                 if(TmpDevicePathNode->SubType == MSG_USB_DP) { 
	                         	
	             ///USBR ParentPortNumber is 0xB(PCH-LP) and 0xF(PCH-H)
	             ///     InterfaceNumber is 0 and 1
	             ///If we find this device, skip it 
	                                 
	                      ParentPortNumber = ((USB_DEVICE_PATH*)TmpDevicePathNode)->ParentPortNumber;
	                      InterfaceNumber = ((USB_DEVICE_PATH*)TmpDevicePathNode)->InterfaceNumber;

	                       if(((ParentPortNumber == 0xB) || (ParentPortNumber == 0xF)) && 
	                         ((InterfaceNumber == 0) || (InterfaceNumber == 1)))  
	                         {
	                	       Status = gBS->HandleProtocol (
	                	                       HandleBuffer[Index],
	                	                       &gEfiDiskInfoProtocolGuid,
	                	                       (VOID **) &DiskInfo
	                	                       );
	                	       if (EFI_ERROR (Status)) {
	                		       
	                		       Status = gBS->AllocatePool (EfiBootServicesData,
	                		                   sizeof(EFI_DISK_INFO_PROTOCOL),
	                		                   (VOID**)&DiskInfo);
	                		       if (EFI_ERROR(Status)){
	                		           return Status;
	                		       }
	                		       
	                		       DiskInfo->Identify = DiskInfoIdentify;
	                		       DiskInfo->Inquiry = DiskInfoInquiry;
	                		       DiskInfo->SenseData = DiskInfoSenseData;
	                		       DiskInfo->WhichIde = DiskInfoWhichIDE;
	                		       DiskInfo->Interface = gEfiDiskInfoUsbInterfaceGuid;
	                		       
	                		       Status = gBS->InstallProtocolInterface(
	                		   		    &HandleBuffer[Index],
	                		   		    &gEfiDiskInfoProtocolGuid,
	                		   		    EFI_NATIVE_INTERFACE,
	                		   		    DiskInfo); 
		                	       
	                	       }
	                         }
	                    }
	             }   
	                     TmpDevicePathNode = NextDevicePathNode (TmpDevicePathNode);
	          }    
	                 
	       }         
	      	
	         return EFI_SUCCESS;	
}
// APTIOV_SERVER_OVERRIDE_RC_END

EFI_STATUS
EFIAPI
AmtWrapperDxeEntryPoint (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
/*++

Routine Description:

  Entry point for the Active Management Driver.

Arguments:

  ImageHandle       Image handle of this driver.
  SystemTable       Global system service table.

Returns:

  EFI_SUCCESS           Initialization complete.
  EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  EFI_DEVICE_ERROR      Device error, driver exits abnormally.

--*/
{
  EFI_STATUS             Status;
  EFI_EVENT              ReadyToBootEvent;
  ME_BIOS_PAYLOAD_HOB    *MbpHob;
  EFI_EVENT              PlatformSetupEvent;
  VOID                   *Registration;

#if defined(SPS_SUPPORT) && SPS_SUPPORT
    if (!MeTypeIsAmt()) {
      return EFI_UNSUPPORTED;
    }
#endif //SPS_SUPPORT

  mAsfBootOptions   = NULL;
  mActiveManagement = NULL;
  MbpHob            = NULL;

  InitAmtWrapperLib();

  mFwImageType = INTEL_ME_CONSUMER_FW;

  //
  // Get Mbp Protocol
  //
  MbpHob = GetFirstGuidHob (&gMeBiosPayloadHobGuid);
  if (MbpHob != NULL) {
    mFwImageType = (UINT8)MbpHob->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType;
  }

  //
  // Install the EFI_ACTIVE_MANAGEMENT_PROTOCOL interface
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
               &ImageHandle,
               &gEfiAmtWrapperProtocolGuid,   &AmtWrapperInstance,
               NULL
           );

// APTIOV_SERVER_OVERRIDE_RC_START
#if defined (Terminal_SUPPORT) && (Terminal_SUPPORT == 1)  
    // Enable/Disable Sol redirection
      SetSOLCOMEnable(AmtWrapperEnableSol());
#endif
// APTIOV_SERVER_OVERRIDE_RC_END
  //
  // Register Ready to Boot Event for AMT Wrapper
  //
  Status = EfiCreateEventReadyToBootEx (
              TPL_CALLBACK,
              AmtWrapperReadyToBoot,
              (VOID *)&ImageHandle,
              &ReadyToBootEvent
              );

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  AsfWatchDogStopOnSetupEntry,
                  NULL,
                  &PlatformSetupEvent);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->RegisterProtocolNotify (
                  &gEfiSetupEnterGuid,
                  PlatformSetupEvent,
                  &Registration);
  ASSERT_EFI_ERROR (Status);
// APTIOV_SERVER_OVERRIDE_RC_START : For USB diskinfo
  {
          EFI_HANDLE                      *Handles, Handle;
          UINTN                           Count, i, *Buffer;

      if(ActiveManagementEnableStorageRedir())
      {  	  
          Status = gBS->LocateHandleBuffer (ByProtocol, \
                          &gAmtReadyToBootProtocolGuid,  NULL, &Count, &Handles);
          if (EFI_ERROR(Status)) Count = 0;
          
          // Allocate enough buffer to save current AmtReadyToBootProtocol.    
      	Status = gBS->AllocatePool(EfiBootServicesData, \
                                      (Count + 1) * sizeof(UINTN), (VOID*)&Buffer);
          
          // In order to install the mUSBDummyDiskInfo prior to 
          // AmtReadyToBootProtocol of Mebx driver, below to save then uninstall
          // all the AmtReadyToBootProtocols
          if ((!EFI_ERROR(Status)) && (Count > 0)) { 
              for (i = 0; i < Count; i++) {
                  Status = gBS->HandleProtocol (Handles[i], 
                              &gAmtReadyToBootProtocolGuid, (VOID**)&Buffer[i + 1]);
                  if (!EFI_ERROR(Status)) {
                      Status = gBS->UninstallProtocolInterface (Handles[i],
                              &gAmtReadyToBootProtocolGuid, (VOID*)Buffer[i + 1]);
                  }
              }
          }

          // Force the mUSBDummyDiskInfo at 1st AmtReadyToBoot Protocol then 
          // reinstall all the saved AmtReadyToBootProtocols.
          Buffer[0] = (UINTN)&mUSBDummyDiskInfo;
          for (i = 0; i < (Count + 1); i++) {
              Handle = NULL;
              Status = gBS->InstallProtocolInterface (\
                          &Handle,  &gAmtReadyToBootProtocolGuid, 
                          EFI_NATIVE_INTERFACE, (VOID*)Buffer[i]);
          }
      }
  }
// APTIOV_SERVER_OVERRIDE_RC_END : For USB diskinfo
  return Status;
}

BOOLEAN
AmtWrapperBootOptionExist (
    IN VOID
)
{
    return (BOOLEAN) (mAsfBootOptions->SubCommand == ASF_BOOT_OPTIONS_PRESENT);
}

BOOLEAN
AmtWrapperGet(
    IN UINTN Index,
    IN OUT VOID *pVar
)
{
  BOOLEAN st = FALSE;

  InitAmtWrapperLib();

  if (Index == GET_SOL_DP) {
    *((PLATFORM_PCI_SERIAL_OVER_LAN_DEVICE_PATH **)pVar) = &gSerialOverLanDevicePath;
    st = TRUE;
  } else if (Index == GET_BOOT_OPTION) {
    *((UINT16 *)pVar) = mAsfBootOptions->BootOptions;
     st = TRUE;
  } else if (Index == GET_IS_SOL) {
    *((UINTN *)pVar) = IsSOL(*((EFI_HANDLE *)pVar));
     st = TRUE;
  }

  return st;
}

UINTN IsSOL(
    IN EFI_HANDLE Controller
)
{
    EFI_STATUS                  Status;
    EFI_PCI_IO_PROTOCOL         *PciIo;
    UINTN                       Segment=0;
    UINTN                       Bus=0;
    UINTN                       Device=0;
    UINTN                       Function=0;
    UINTN                       flag = FALSE;

    Status = gBS->HandleProtocol(Controller,
                                 &gEfiPciIoProtocolGuid,
                                 &PciIo);
    if (!EFI_ERROR(Status)) {
        Status = PciIo->GetLocation(PciIo,
                                    &Segment,
                                    &Bus,
                                    &Device,
                                    &Function);
        if (!EFI_ERROR(Status)) {
            if (Device == SOL_DEVICE_NUMBER && Function == SOL_FUNCTION_NUMBER) {
                flag = TRUE;
            }
        }
    }

    return flag;
}

BOOLEAN
AmtWrapperSet(
  IN UINTN Index,
  IN OUT VOID *pVar
)
{
  BOOLEAN    st = FALSE;

  InitAmtWrapperLib();

  if (Index == SET_BIOS_WDT_START) {
    AsfStartWatchDog(ASF_START_BIOS_WDT);
    st = TRUE;
  }
  else if (Index == SET_OS_WDT_START) {
    AsfStartWatchDog(ASF_START_OS_WDT);
    st = TRUE;
  }
  else if (Index == SET_WDT_STOP) {
    AsfStopWatchDog();
    st = TRUE;
  }
  else if (Index == SET_OS_SOL) {
    DEBUG((DEBUG_ERROR, "<SET_OS_SOL>"));
    st = TRUE;
  }

  return st;
}

EFI_STATUS
AmtWrapperInit(
    IN EFI_HANDLE ImageHandle,
    IN OUT EFI_SYSTEM_TABLE *SystemTable
)
{
    mBdsImageHandle = ImageHandle;
    InitAmtWrapperLib();

    return EFI_SUCCESS;
}


VOID
InitAmtWrapperLib(VOID)
{
    if (mActiveManagement == NULL)
      AmtLibInit();

    if (mAsfBootOptions == NULL) {
      BdsAsfInitialization();
    }
}

BOOLEAN
AmtWrapperEnableSecureErase(
    IN VOID
)
{
    BOOLEAN st;

    InitAmtWrapperLib();
    st = ActiveManagementEnableSecureErase();

    return st;
}

BOOLEAN
AmtWrapperEnableSol(
    IN VOID
)
{
    BOOLEAN st;

    InitAmtWrapperLib();
    st = ActiveManagementEnableSol();

    return st;
}

BOOLEAN
AmtWrapperEnableStorageRedir(
    IN VOID
)
{
    BOOLEAN st;
    // APTIOV_SERVER_OVERRIDE_RC_START
    EFI_STATUS Status;
    EFI_SECURITY2_ARCH_PROTOCOL              *mSecurity2;
    // APTIOV_SERVER_OVERRIDE_RC_END
    
    InitAmtWrapperLib();
    st = ActiveManagementEnableStorageRedir();
    
    // APTIOV_SERVER_OVERRIDE_RC_START
    if(!st)
        return st;
    // USBR enabled and Enforce Secure Boot disabled
    if((mAsfBootOptions->SpecialCommandParam & ENFORCE_SECURE_BOOT) 
                                            != ENFORCE_SECURE_BOOT)
    {        
        Status = gBS->LocateProtocol(&gEfiSecurity2ArchProtocolGuid, 
                                     NULL, &mSecurity2);
        if(EFI_ERROR(Status))
           return st;

        // Set a empty File Authentication to skip Secure check.
        // This will not been restore, because USBRR boot fail system HALT

        mSecurity2->FileAuthentication = EmptyFileAuthentication;
        
    }    
    // APTIOV_SERVER_OVERRIDE_RC_END
    return st;
}

BOOLEAN
AmtWrapperPauseBoot(
    IN VOID
)
{
    BOOLEAN st;

    InitAmtWrapperLib();
    st = ActiveManagementPauseBoot();

    return st;
}

BOOLEAN
AmtWrapperEnterSetup(
    IN VOID
)
{
    BOOLEAN st;

    InitAmtWrapperLib();
    st = ActiveManagementEnterSetup();

    return st;
}

EFI_STATUS
AmtWrapperBdsBootViaAsf(
    IN VOID
)
{
    EFI_STATUS Status = EFI_NOT_FOUND;

    InitAmtWrapperLib();

    if (mActiveManagement != NULL && mAsfBootOptions != NULL) {
      Status = BdsBootViaAsf();
    }

    return Status;
}



EFI_STATUS
AmtWrapperReadyToBoot(
  EFI_EVENT           Event,
  VOID                *ParentImageHandle
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  gBS->CloseEvent(Event);
  return Status;
}

BOOLEAN
AmtWrapperEnableKvm(
  IN VOID
)
{
  BOOLEAN Status;

  InitAmtWrapperLib();
  Status = ActiveManagementEnableKvm();

  return Status;
}
