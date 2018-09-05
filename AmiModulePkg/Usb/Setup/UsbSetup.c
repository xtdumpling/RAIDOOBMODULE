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

/** @file UsbSetup.c
    USB driver setup related functions implementation.

**/

#include <Library/DebugLib.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Protocol/AmiUsbController.h>
#include <Protocol/UsbPolicy.h>

#define MAX_DEVS_LINE_LENGTH 80
#define MAX_DEVICE_NUMBER_LENGTH 10
#define MAX_DEVICE_AMOUNT 127

static EFI_GUID gEfiSetupGuid = SETUP_GUID;

/**
    This function retrieves the information about connected
    USB devices.

    @retval returns TRUE if device connection status has changed since this
        function is called last time; otherwise FALSE.
    @note  When FALSE is returned, none of the output parameters are valid.
**/

EFI_STATUS
GetConnectedDevices(
    CHAR16                          *DevNumStr,
    CHAR16                          *ControllerNumStr,
    EFI_USB_PROTOCOL                *UsbProtocol,
    CONNECTED_USB_DEVICES_NUM       *Devs
)
{
    CHAR16 	StrMassStorage[] = L"Drive";
    CHAR16 	StrKeyboard[] = L"Keyboard";
    CHAR16 	StrMouse[] = L"Mouse";
    CHAR16 	StrPoint[] = L"Point"; 		//(EIP38434+)
    CHAR16 	StrMice[] = L"Mice";
    CHAR16 	StrHub[] = L"Hub";
	CHAR16 	StrCcid[] = L"SmartCard Reader";
    CHAR16 	StrUhci[] = L"UHCI";
    CHAR16 	StrOhci[] = L"OHCI";
    CHAR16 	StrEhci[] = L"EHCI";
    CHAR16 	StrXhci[] = L"XHCI";
    CHAR16 	Name[MAX_DEVS_LINE_LENGTH];
    CHAR16 	*StrPtr = Name;
    CHAR16 	NumberToString [MAX_DEVICE_NUMBER_LENGTH];
    UINTN 	NumSize;
    CHAR16 	Comma[] = L", ";
    CHAR16 	Space[] = L" ";
    CHAR16 	LeadingSpace[] = L"      None";
	UINT8	MassStorageNumber;
	UINT8 	KeyboardNumber; 
	UINT8	MouseNumber;
	UINT8	PointNumber; 
	UINT8	HubNumber; 
	UINT8	CcidNumber;	//(EIP38434)
	UINT8   UhciNumber;
    UINT8   OhciNumber;
    UINT8   EhciNumber;
    UINT8   XhciNumber;
    BOOLEAN	Is1stItem = TRUE;

    UsbProtocol->UsbReportDevices(Devs);
    MassStorageNumber = Devs->NumUsbMass;
    KeyboardNumber = Devs->NumUsbKbds;
    MouseNumber	 = Devs->NumUsbMice;
    PointNumber = Devs->NumUsbPoint;
    HubNumber = Devs->NumUsbHubs;
	CcidNumber = Devs->NumUsbCcids;
    UhciNumber = Devs->NumUhcis;
    OhciNumber = Devs->NumOhcis;
    EhciNumber = Devs->NumEhcis;
    XhciNumber = Devs->NumXhcis;

    // Form the string
    pBS->SetMem(StrPtr, MAX_DEVS_LINE_LENGTH * sizeof(CHAR16), 0);
    pBS->CopyMem(StrPtr, LeadingSpace, 10 * sizeof(CHAR16)); 
    StrPtr += 6;  // leave string pointer at "None"

    // Drive/Drives
    if ((MassStorageNumber) && (MassStorageNumber < MAX_DEVICE_AMOUNT)) {
        ItowEx(MassStorageNumber, NumberToString, 10, FALSE);
        NumSize = Wcslen(NumberToString);

        pBS->CopyMem(StrPtr, NumberToString, (NumSize * sizeof (CHAR16)));
        StrPtr += NumSize;
        // move pointer 1 more space
        pBS->CopyMem(StrPtr, Space, 2);
        StrPtr += 1;

        pBS->CopyMem(StrPtr, StrMassStorage, 10); 
        StrPtr += 5;		        // L"Drive"
        if (MassStorageNumber > 1) {
          *StrPtr++ = L's';   // L"Drives"
        }
        Is1stItem = FALSE;
    }

    // Keyboard/Keyboards
    if ((KeyboardNumber) && (KeyboardNumber < MAX_DEVICE_AMOUNT)) {
        if (!Is1stItem) {
            pBS->CopyMem(StrPtr, Comma, 4); 
            StrPtr += 2;    // L" ,"
        }
        ItowEx(KeyboardNumber, NumberToString, 10, FALSE);
        NumSize = Wcslen(NumberToString);

        // move pointer 1 more space then string length
        pBS->CopyMem(StrPtr, NumberToString, (NumSize * sizeof (CHAR16)));
        StrPtr += NumSize;
        // move pointer 1 more space
        pBS->CopyMem(StrPtr, Space, 2);
        StrPtr += 1;

        pBS->CopyMem(StrPtr, StrKeyboard, 16); 
        StrPtr += 8;    // L"Keyboard"
        if (KeyboardNumber > 1) {
          *StrPtr++ = L's'; // L"Keyboards"
        }
        Is1stItem = FALSE;
    }

    // Mouse/Mice
    if ((MouseNumber) && (MouseNumber < MAX_DEVICE_AMOUNT)) {
        if (!Is1stItem) {
            pBS->CopyMem(StrPtr, Comma, 4);
            StrPtr += 2;    // L" ,"
        }
        ItowEx(MouseNumber, NumberToString, 10, FALSE);
        NumSize = Wcslen(NumberToString);

        // move pointer 1 more space then string length
        pBS->CopyMem(StrPtr, NumberToString, (NumSize * sizeof (CHAR16)));
        StrPtr += NumSize;
        // move pointer 1 more space
        pBS->CopyMem(StrPtr, Space, 2);
        StrPtr += 1;

        if (MouseNumber == 1) {
            pBS->CopyMem(StrPtr, StrMouse, 10);
            StrPtr += 5;        // L"Mouse"
        } else {
            pBS->CopyMem(StrPtr, StrMice, 8);
            StrPtr += 4;        // L"Mice"
        }
        Is1stItem = FALSE;
    }
										//(EIP38434+)>
    // Point/Points
    if ((PointNumber) && (PointNumber < MAX_DEVICE_AMOUNT)) {
        if (!Is1stItem) {
            pBS->CopyMem(StrPtr, Comma, 4);
            StrPtr += 2;    // L" ,"
        }
        ItowEx(PointNumber, NumberToString, 10, FALSE);
        NumSize = Wcslen(NumberToString);
        // move pointer 1 more space then string length
        pBS->CopyMem(StrPtr, NumberToString, (NumSize * sizeof (CHAR16)));
        StrPtr += NumSize;
        // move pointer 1 more space
        pBS->CopyMem(StrPtr, Space, 2);
        StrPtr += 1;

        pBS->CopyMem(StrPtr, StrPoint, 10);
        StrPtr += 5;            // L"Point"
        if (PointNumber > 1) {
          *StrPtr++ = L's';     // L"Points"
        }
        Is1stItem = FALSE;
    } 
										//<(EIP38434+)
    // Hub/Hubs
    if ((HubNumber) && (HubNumber < MAX_DEVICE_AMOUNT)) {
        if (!Is1stItem) {
            pBS->CopyMem(StrPtr, Comma, 4);
            StrPtr += 2;    // L" ,"
        }
        ItowEx(HubNumber, NumberToString, 10, FALSE);
        NumSize = Wcslen(NumberToString);

        // move pointer 1 more space then string length
        pBS->CopyMem(StrPtr, NumberToString, (NumSize * sizeof (CHAR16)));
        StrPtr += NumSize;
        // move pointer 1 more space
        pBS->CopyMem(StrPtr, Space, 2);
        StrPtr += 1;
        pBS->CopyMem(StrPtr, StrHub, 6);
        StrPtr += 3;            // L"Hub"
        if (HubNumber > 1) {
            *StrPtr++ = L's';   // L"Hubs"
        }
        Is1stItem = FALSE;
    }
	// Ccid/Ccids
	if (CcidNumber) {
		if (!Is1stItem) {
			pBS->CopyMem(StrPtr, Comma, 4);
            StrPtr += 2;	// L" ,"
		}
        ItowEx(CcidNumber, NumberToString, 10, FALSE);
        NumSize = Wcslen(NumberToString);

        // move pointer 1 more space then string length
        pBS->CopyMem(StrPtr, NumberToString, (NumSize * sizeof (CHAR16)));
        StrPtr += NumSize;

        // move pointer 1 more space
        pBS->CopyMem(StrPtr, Space, 2);
        StrPtr += 1;	//	L" "

		pBS->CopyMem(StrPtr, StrCcid, 32); 
        StrPtr += 16;           // L"SmartCard Reader"
		if (CcidNumber > 1) {
			*StrPtr++ = L's';   // L'SmartCard Readers'
		}
		Is1stItem = FALSE;
	}

    pBS->CopyMem(DevNumStr, Name, MAX_DEVS_LINE_LENGTH * sizeof(CHAR16));

    Is1stItem = TRUE;
    StrPtr = Name;
    // Form the string
    pBS->SetMem(StrPtr, MAX_DEVS_LINE_LENGTH * sizeof(CHAR16), 0);
    pBS->CopyMem(StrPtr, LeadingSpace, 10 * sizeof(CHAR16)); 
    StrPtr += 6;  // leave string pointer at "None"

    // Drive/Drives
    if (UhciNumber) {
        ItowEx(UhciNumber, NumberToString, 10, FALSE);
        NumSize = Wcslen(NumberToString);

        pBS->CopyMem(StrPtr, NumberToString, (NumSize * sizeof (CHAR16)));
        StrPtr += NumSize;
        // move pointer 1 more space
        pBS->CopyMem(StrPtr, Space, 2);
        StrPtr += 1;

        pBS->CopyMem(StrPtr, StrUhci, 8); 
        StrPtr += 4;		  // L"UHCI"
        if (UhciNumber > 1) {
          *StrPtr++ = L's';   // L"UHCIs"
        }
        Is1stItem = FALSE;
    }

	if (OhciNumber) {
		if (!Is1stItem) {
			pBS->CopyMem(StrPtr, Comma, 4);
            StrPtr += 2;	// L" ,"
		}
        ItowEx(OhciNumber, NumberToString, 10, FALSE);
        NumSize = Wcslen(NumberToString);

        // move pointer 1 more space then string length
        pBS->CopyMem(StrPtr, NumberToString, (NumSize * sizeof (CHAR16)));
        StrPtr += NumSize;

        // move pointer 1 more space
        pBS->CopyMem(StrPtr, Space, 2);
        StrPtr += 1;	//	L" "

		pBS->CopyMem(StrPtr, StrOhci, 8); 
        StrPtr += 4;            // L"OHCI"
		if (OhciNumber > 1) {
			*StrPtr++ = L's';   // L'OHCIs'
		}
		Is1stItem = FALSE;
	}

	if (EhciNumber) {
		if (!Is1stItem) {
			pBS->CopyMem(StrPtr, Comma, 4);
            StrPtr += 2;	// L" ,"
		}
        ItowEx(EhciNumber, NumberToString, 10, FALSE);
        NumSize = Wcslen(NumberToString);

        // move pointer 1 more space then string length
        pBS->CopyMem(StrPtr, NumberToString, (NumSize * sizeof (CHAR16)));
        StrPtr += NumSize;

        // move pointer 1 more space
        pBS->CopyMem(StrPtr, Space, 2);
        StrPtr += 1;	//	L" "

		pBS->CopyMem(StrPtr, StrEhci, 8); 
        StrPtr += 4;            // L"EHCI"
		if (EhciNumber > 1) {
			*StrPtr++ = L's';   // L'EHCIs'
		}
		Is1stItem = FALSE;
	}

	if (XhciNumber) {
		if (!Is1stItem) {
			pBS->CopyMem(StrPtr, Comma, 4);
            StrPtr += 2;	// L" ,"
		}
        ItowEx(XhciNumber, NumberToString, 10, FALSE);
        NumSize = Wcslen(NumberToString);

        // move pointer 1 more space then string length
        pBS->CopyMem(StrPtr, NumberToString, (NumSize * sizeof (CHAR16)));
        StrPtr += NumSize;

        // move pointer 1 more space
        pBS->CopyMem(StrPtr, Space, 2);
        StrPtr += 1;	//	L" "

		pBS->CopyMem(StrPtr, StrXhci, 8); 
        StrPtr += 4;            // L"XHCI"
		if (XhciNumber > 1) {
			*StrPtr++ = L's';   // L'XHCIs'
		}
		Is1stItem = FALSE;
	}

    pBS->CopyMem(ControllerNumStr, Name, MAX_DEVS_LINE_LENGTH * sizeof(CHAR16));

    return EFI_SUCCESS;
}


/**
    This function retrieves the USB mass storage device Unicode name.

**/

UINT8
GetMassDeviceName(
    CHAR16              *DevName,
    UINT8               DevAddr,
    EFI_USB_PROTOCOL *UsbProtocol
)
{
    CHAR8   Data[48];
    UINT8   AsciiCodeData[48] = {0};
    UINT8   NextDev;

    // Get the name using USBMassAPIGetDeviceInformation
    NextDev = UsbProtocol->UsbGetNextMassDeviceName(Data, sizeof(Data), DevAddr);
    
    if (NextDev != 0xFF) {
        Sprintf((char*)AsciiCodeData, "%a", Data);
    }

    AsciiStrToUnicodeStr(AsciiCodeData, DevName);

    return NextDev;
}


/**
    This function is eLink'ed with the chain executed right before
    the Setup.

**/

VOID
InitUSBStrings(
    EFI_HII_HANDLE HiiHandle,
    UINT16          Class
)
{
    CHAR16  DevNumStr[MAX_DEVS_LINE_LENGTH];
    CHAR16  ControllerNumStr[MAX_DEVS_LINE_LENGTH];
    CHAR16  MassStr[MAX_DEVS_LINE_LENGTH];
    UINT8   NextDev;
    UINT16  MassDev[16] = {
        STRING_TOKEN(STR_USB_MASS_DEVICE1),
        STRING_TOKEN(STR_USB_MASS_DEVICE2),
        STRING_TOKEN(STR_USB_MASS_DEVICE3),
        STRING_TOKEN(STR_USB_MASS_DEVICE4),
        STRING_TOKEN(STR_USB_MASS_DEVICE5),
        STRING_TOKEN(STR_USB_MASS_DEVICE6),
        STRING_TOKEN(STR_USB_MASS_DEVICE7),
        STRING_TOKEN(STR_USB_MASS_DEVICE8),
        STRING_TOKEN(STR_USB_MASS_DEVICE9),
        STRING_TOKEN(STR_USB_MASS_DEVICE10),
        STRING_TOKEN(STR_USB_MASS_DEVICE11),
        STRING_TOKEN(STR_USB_MASS_DEVICE12),
        STRING_TOKEN(STR_USB_MASS_DEVICE13),
        STRING_TOKEN(STR_USB_MASS_DEVICE14),
        STRING_TOKEN(STR_USB_MASS_DEVICE15),
        STRING_TOKEN(STR_USB_MASS_DEVICE16)

    };
    UINT8               MassDevValid[16];
    UINT8               i;
    UINTN               VariableSize;
    USB_MASS_DEV_NUM    SetupData;
    EFI_STATUS          Status;
    EFI_USB_PROTOCOL    *UsbProtocol;
    EFI_GUID            UsbProtocolGuid = EFI_USB_PROTOCOL_GUID;
	CONNECTED_USB_DEVICES_NUM       DevNum;
    UINT32              VariableAttributes = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    USB_CONTROLLER_NUM  UsbControllerNum;

#if USB_SETUP_VARIABLE_RUNTIME_ACCESS
    VariableAttributes |= EFI_VARIABLE_RUNTIME_ACCESS;
#endif


    if (Class!=ADVANCED_FORM_SET_CLASS) {
        return;
    }
                                        //(EIP102493+)>
    InitString(HiiHandle, STRING_TOKEN(STR_USB_MODULE_VER), L"%02d", USB_DRIVER_MAJOR_VER);
                                        //<(EIP102493+)

    Status = pBS->LocateProtocol(&UsbProtocolGuid, NULL, &UsbProtocol);
//####  ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
        return;
    }

    // Number of connected devices
    GetConnectedDevices(DevNumStr, ControllerNumStr, UsbProtocol, &DevNum);
    
    DEBUG((DEBUG_INFO, "Devices total: %d KBDs, %d HUBs, %d MICE, %d POINT %d MASS %d CCID\n",
        DevNum.NumUsbKbds, DevNum.NumUsbHubs, DevNum.NumUsbMice,
        DevNum.NumUsbPoint, DevNum.NumUsbMass, DevNum.NumUsbCcids));
    
    InitString(HiiHandle, STRING_TOKEN(STR_USB_DEVICES_ENABLED_LIST), L"%s", DevNumStr);
    InitString(HiiHandle, STRING_TOKEN(STR_USB_CONTROLLERS_ENABLED_LIST), L"%s", ControllerNumStr);

    // Mass storage device names
    for (i = 0, NextDev = 0; i < 16; i++) {
        NextDev = GetMassDeviceName(MassStr, NextDev, UsbProtocol);
        if (NextDev == 0xFF) {
            break;    // No more devices
        }
        InitString(HiiHandle, MassDev[i], L"%s", MassStr);
        if (NextDev & 0x80) {
            break;     // Last device
        }
    }

    for (i = 0; i < 16; i++) {
        if (i < DevNum.NumUsbMass) {
            MassDevValid[i] = 1;
        } else {
            MassDevValid[i] = 0;
        }
    }

    VariableSize = sizeof(USB_MASS_DEV_VALID);

    Status = pRS->SetVariable(L"UsbMassDevValid",
        &gEfiSetupGuid,
        VariableAttributes,
        VariableSize,
        &MassDevValid);

    VariableSize = sizeof(SetupData);
    Status = pRS->GetVariable(L"UsbMassDevNum",
        &gEfiSetupGuid,
        NULL,
        &VariableSize,
        &SetupData);

    if (EFI_ERROR(Status)) {
        return;
    }

    SetupData.IsInteractionAllowed = 0;
    SetupData.UsbMassDevNum = DevNum.NumUsbMass;

    Status = pRS->SetVariable(L"UsbMassDevNum",
        &gEfiSetupGuid,
        VariableAttributes,
        VariableSize,
        &SetupData);

    VariableSize = sizeof(UsbControllerNum);
    Status = pRS->GetVariable(L"UsbControllerNum",
        &gEfiSetupGuid,
        NULL,
        &VariableSize,
        &UsbControllerNum);

    if (EFI_ERROR(Status)) {
        return;
    }

    UsbControllerNum.UhciNum = DevNum.NumUhcis;
    UsbControllerNum.OhciNum = DevNum.NumOhcis;
    UsbControllerNum.EhciNum = DevNum.NumEhcis;
    UsbControllerNum.XhciNum = DevNum.NumXhcis;

    Status = pRS->SetVariable(L"UsbControllerNum",
        &gEfiSetupGuid,
        VariableAttributes,
        VariableSize,
        &UsbControllerNum);

}

/**
    This function is updating usb setup page dynamically.

**/

VOID
UpdateUSBStrings(
    EFI_HII_HANDLE HiiHandle,
    UINT16          Class,
    UINT16          SubClass,
    UINT16          Key
)
{
    UINT8       MassDevValid[16];
    UINT8       i;
    UINT8       NextDev;
    UINTN       VariableSize;
    EFI_GUID    UsbProtocolGuid = EFI_USB_PROTOCOL_GUID;
    EFI_STATUS  Status;
    EFI_USB_PROTOCOL                *UsbProtocol;
    CONNECTED_USB_DEVICES_NUM       DevNum;
    USB_MASS_DEV_NUM                MassDevNumData;
    USB_CONTROLLER_NUM              UsbControllerNum;
    CHAR16      DevNumStr[MAX_DEVS_LINE_LENGTH];
    CHAR16      ControllerNumStr[MAX_DEVS_LINE_LENGTH];
    CHAR16      MassStr[MAX_DEVS_LINE_LENGTH];
    UINT16      MassDev[16] = {
                    STRING_TOKEN(STR_USB_MASS_DEVICE1),
                    STRING_TOKEN(STR_USB_MASS_DEVICE2),
                    STRING_TOKEN(STR_USB_MASS_DEVICE3),
                    STRING_TOKEN(STR_USB_MASS_DEVICE4),
                    STRING_TOKEN(STR_USB_MASS_DEVICE5),
                    STRING_TOKEN(STR_USB_MASS_DEVICE6),
                    STRING_TOKEN(STR_USB_MASS_DEVICE7),
                    STRING_TOKEN(STR_USB_MASS_DEVICE8),
                    STRING_TOKEN(STR_USB_MASS_DEVICE9),
                    STRING_TOKEN(STR_USB_MASS_DEVICE10),
                    STRING_TOKEN(STR_USB_MASS_DEVICE11),
                    STRING_TOKEN(STR_USB_MASS_DEVICE12),
                    STRING_TOKEN(STR_USB_MASS_DEVICE13),
                    STRING_TOKEN(STR_USB_MASS_DEVICE14),
                    STRING_TOKEN(STR_USB_MASS_DEVICE15),
                    STRING_TOKEN(STR_USB_MASS_DEVICE16)
                };
    
    if (Key == USB_DEVICES_ENABLED_REFRESH_KEY) {
        
        Status = pBS->LocateProtocol(&UsbProtocolGuid, NULL, &UsbProtocol);

        if (EFI_ERROR(Status)) {
            return;
        }

        // Number of connected devices
        GetConnectedDevices(DevNumStr, ControllerNumStr, UsbProtocol, &DevNum);

        InitString(HiiHandle, STRING_TOKEN(STR_USB_DEVICES_ENABLED_LIST), L"%s", DevNumStr);

        // Mass storage device names
        for (i = 0, NextDev = 0; i < 16; i++) {
            NextDev = GetMassDeviceName(MassStr, NextDev, UsbProtocol);
            if (NextDev == 0xFF) {
                break;    // No more devices
            }
            InitString(HiiHandle, MassDev[i], L"%s", MassStr);
            if (NextDev & 0x80) {
                break;     // Last device
            }
        }

        VariableSize = sizeof(MassDevValid);
    	   
    	Status = HiiLibGetBrowserData(
                    &VariableSize, &MassDevValid,
                    &gEfiSetupGuid, L"UsbMassDevValid");
        
        if (EFI_ERROR(Status)) {
            return;
        }
        	   	   	    
        for (i = 0; i < 16; i++) {
            if (i < DevNum.NumUsbMass) {
                MassDevValid[i] = 1;
            } else {
                MassDevValid[i] = 0;
            }
        }

    	Status = HiiLibSetBrowserData(
                    VariableSize, &MassDevValid,
                    &gEfiSetupGuid, L"UsbMassDevValid");
             
        if (EFI_ERROR(Status)) {
            return;
        }

        VariableSize = sizeof(MassDevNumData);

    	Status = HiiLibGetBrowserData(
                    &VariableSize, &MassDevNumData,
                    &gEfiSetupGuid, L"UsbMassDevNum");
        
        if (EFI_ERROR(Status)) {
            return;
        }

        MassDevNumData.UsbMassDevNum = DevNum.NumUsbMass;

    	Status = HiiLibSetBrowserData(
                    VariableSize, &MassDevNumData,
                    &gEfiSetupGuid, L"UsbMassDevNum");
        
        VariableSize = sizeof(UsbControllerNum);

    	Status = HiiLibGetBrowserData(
                    &VariableSize, &UsbControllerNum,
                    &gEfiSetupGuid, L"UsbControllerNum");
        
        if (EFI_ERROR(Status)) {
            return;
        }

        UsbControllerNum.UhciNum = DevNum.NumUhcis;
        UsbControllerNum.OhciNum = DevNum.NumOhcis;
        UsbControllerNum.EhciNum = DevNum.NumEhcis;
        UsbControllerNum.XhciNum = DevNum.NumXhcis;

    	Status = HiiLibSetBrowserData(
                    VariableSize, &UsbControllerNum,
                    &gEfiSetupGuid, L"UsbControllerNum");
        
    }	
    
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
