#pragma warning(disable: 4001)
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

/** @file UsbKbd.c
    USB keyboard driver SMI routines

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "AmiUsb.h"
#include "UsbKbd.h"
#include <UsbDevDriverElinks.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/AmiUsbHid.h>

extern USB_GLOBAL_DATA *gUsbData;
extern BOOLEAN gKeyRepeatStatus;
extern UINT16 gKbcSetTypeRate11CharsSec;
extern UINT16 gKbcSetTypeDelay500MSec;
UINT8   gLastKeyCodeArray[8]={0,0,0,0,0,0,0,0};

//----------------------------------------------------------------------------
// Typematic rate delay table will have counts to generate key repeat delays.
// Since the periodic interrupt is set to 8msec the following repeat times
// will generate necessary delay.
// First three numbers are meant to define the frequency of the repeated keys;
// four other numbers are used to define the amount of delay between the first
// keypress-and-hold til the key actually starts repeating; the appropriate values
// of this table are selected using the equates defined in UsbKbd.h
//
UINT8   aTypematicRateDelayTable[]      =   {2, 4, 8, 16, 32, 48, 64, 96};

//
// The global data variables are stored in USB_GLOBAL_DATA structure and can be accessed through
// gUsbData->xxx
//

LEGACY_USB_KEYBOARD mLegacyKeyboard;

extern  VOID USBKB_LEDOn();

extern EFI_EMUL6064KBDINPUT_PROTOCOL* gKbdInput ;

typedef	BOOLEAN (KBD_BUFFER_CHECK_FUNCTIONS)( 
    DEV_INFO	*DevInfo,
    UINT8       *Buffer
	);
extern KBD_BUFFER_CHECK_FUNCTIONS KBD_BUFFER_CHECK_ELINK_LIST EndOfInitList;
KBD_BUFFER_CHECK_FUNCTIONS* KbdBufferCheckFunctionsList[] = {KBD_BUFFER_CHECK_ELINK_LIST NULL};

UINT8	UsbControlTransfer(HC_STRUC*, DEV_INFO*, DEV_REQ, UINT16, VOID*);
UINT32	ExtractInputReportData (UINT8 *Report, UINT16 Offset, UINT16 Size);
UINT8	UsbKbdDataHandler(DEV_INFO*, UINT8*);

/**
    This function returns fills the host controller driver
    routine pointers in the structure provided

    @param VOID

    @retval VOID

**/

VOID
USBKBDInitialize (VOID)
{
    UINT8       bTemp;

    //
    // Initialize the typematic rate to 500 ms, 10.9 Char/Sec and auto repeat flag
    // to disabled
    //
    gUsbData->wUSBKBC_StatusFlag |= ((gKbcSetTypeRate11CharsSec << KBC_TYPE_RATE_BIT_SHIFT) +
                                        (gKbcSetTypeDelay500MSec << KBC_TYPE_DELAY_BIT_SHIFT));

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_5, "USBKBDInitialize:  CodeBufferStart : %lx\n", gUsbData->aKBCScanCodeBufferStart);

    //
    // Initialize the scanner buffer
    //
    gUsbData->fpKBCScanCodeBufferPtr       = gUsbData->aKBCScanCodeBufferStart;
    gUsbData->bLastUSBKeyCode              = 0;

    //
    // Initialize the character buffer
    //
    gUsbData->fpKBCCharacterBufferHead     = gUsbData->aKBCCharacterBufferStart;
    gUsbData->fpKBCCharacterBufferTail     = gUsbData->aKBCCharacterBufferStart;

    gUsbData->fpKeyRepeatDevInfo=NULL;

    //
    // Set scan code set to 2 in the scanner flag
    //
    gUsbData->wUSBKBC_StatusFlag           |= KBC_SET_SCAN_CODE_SET2;

    gUsbData->bUSBKBShiftKeyStatus = 0;

    //
    // Get the keyboard controller command byte (CCB) and store it locally
    //
    //USBKBC_GetAndStoreCCB();
    gUsbData->bCCB = 0x40;

    for (bTemp=0; bTemp<6; bTemp++) mLegacyKeyboard.KeyCodeStorage[bTemp] = 0;
    mLegacyKeyboard.KeyToRepeat = 0;

    gUsbData->EfiMakeCodeGenerated = FALSE;
    gUsbData->LegacyMakeCodeGenerated = FALSE;
    gUsbData->IsKbcAccessBlocked = FALSE;

    return;
}

/**
    Initialize the key buffer

    @param KeyBuffer    - Pointer to the key buffer structure

    @retval VOID

**/
VOID
InitKeyBuffer (
    KEY_BUFFER    *KeyBuffer
)
{
    UINT32      Index;
    UINT8       *Buffer;

    if ((KeyBuffer == NULL) || (KeyBuffer->Buffer == NULL)) {
        return;
    }

    Buffer = (UINT8*)KeyBuffer->Buffer;

    SetMem(Buffer, (KeyBuffer->MaxKey * sizeof(VOID*)) + 
                    (KeyBuffer->MaxKey * KeyBuffer->KeySize), 0);

    Buffer += KeyBuffer->MaxKey * sizeof(VOID*);    
    for (Index = 0; 
        Index < KeyBuffer->MaxKey;
        Index++, Buffer += KeyBuffer->KeySize) {
        KeyBuffer->Buffer[Index] = Buffer;
    }
    KeyBuffer->Head = 0;
    KeyBuffer->Tail = 0;
}

/**
    Create a key buffer

    @param KeyBuffer    - Pointer to the key buffer structure
        MaxKey        - Maximum key of the buffer
        KeySize        - Size of one key

    @retval USB_SUCCESS or USB_ERROR

**/

UINT8
CreateKeyBuffer (
    KEY_BUFFER      *KeyBuffer,
    UINT8           MaxKey,
    UINT32          KeySize
)
{
    UINT16          MemSize;
    UINT8           *Buffer = NULL;

    MemSize = (UINT16)(MaxKey * sizeof(VOID*)) + (MaxKey * KeySize);

    Buffer = USB_MemAlloc(GET_MEM_BLK_COUNT(MemSize));

    if (Buffer == NULL) {
        return USB_ERROR;
    }

    KeyBuffer->Buffer = (VOID**)Buffer;
    KeyBuffer->MaxKey = MaxKey;
    KeyBuffer->KeySize = KeySize;

    InitKeyBuffer(KeyBuffer);

    return USB_SUCCESS;
}

/**
    Destroy the key buffer

    @param KeyBuffer    - Pointer to the key buffer structure

    @retval USB_SUCCESS or USB_ERROR

**/

UINT8
DestroyKeyBuffer (
    KEY_BUFFER      *KeyBuffer,
    UINT8           MaxKey,
    UINT32          KeySize
)
{
    UINT16  MemSize;
    
    if ((KeyBuffer == NULL) || (KeyBuffer->Buffer == NULL)) {
        return USB_SUCCESS;
    }

    MemSize = (UINT16)(MaxKey * sizeof(VOID*)) + (MaxKey * KeySize);

    USB_MemFree(KeyBuffer->Buffer, GET_MEM_BLK_COUNT(MemSize));
    SetMem(KeyBuffer, sizeof(KEY_BUFFER), 0);

    return USB_SUCCESS;
}

/**
    This routine checks an interface descriptor of the USB device
    detected to see if it describes a HID/Boot/Keyboard device.
    If the device matches the above criteria, then the device is
    configured and initialized

    @param DevInfo   Device information structure pointer
              
    @retval DEV_INFO  New device info structure, 0 on error

**/

DEV_INFO*
USBKBDConfigureDevice (
    DEV_INFO*   DevInfo
)
{
    UINT16          Index;
    UINT8           Status;

    DevInfo->fpPollTDPtr  = 0;                                      
    Index = USBKBDFindUSBKBDeviceTableEntry(DevInfo);
    if (Index == 0xFFFF) {
        Index  = USBKBDFindUSBKBDeviceTableEntry(NULL);
    }
    if (Index != 0xFFFF) {
        Status = CreateKeyBuffer(&DevInfo->KeyCodeBuffer, MAX_KEY_ALLOWED, sizeof(UINT8));
        if (Status == USB_ERROR) {
            return 0;
        }
	    Status = CreateKeyBuffer(&DevInfo->UsbKeyBuffer, MAX_KEY_ALLOWED, sizeof(USB_KEY));
        if (Status == USB_ERROR) {
            return 0;
        }
        gUsbData->aUSBKBDeviceTable[Index] = DevInfo;
        if (!(gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI)) {
            if (BOOT_PROTOCOL_SUPPORT || (DevInfo->HidReport.Flag & HID_REPORT_FLAG_LED_FLAG) ||
                (DevInfo->wIncompatFlags & USB_INCMPT_HID_BOOT_PROTOCOL_ONLY)) {
                USBKB_LEDOn();
            }
        }
    } else {    
        return 0;
    }
    
    return DevInfo;
}

/**
    This routine searches for the HID table entry which matches
    the provided device info structure

    @param DevInfo   Pointer to DeviceInfo structure

    @retval 0xFFFF on error

**/

UINT16
USBKBDFindUSBKBDeviceTableEntry(
    DEV_INFO* Devinfo
)
{
    UINT16  Count ;

    for (Count = 0; Count < USB_DEV_HID_COUNT; Count++) {
        if (gUsbData->aUSBKBDeviceTable[Count] == Devinfo) {
            return Count;
        }
    }
    
    if (Devinfo == NULL) {
        USB_DEBUG(DEBUG_WARN, DEBUG_LEVEL_3, "No Free KBD DevInfo Entry\n");
    }
    
    return 0xFFFF;
}

/**
    This routine disconnects the keyboard by freeing
    the USB keyboard device table entry

    @param fpDevInfo   Pointer to DeviceInfo structure

    @retval USB_SUCCESS/USB_ERROR

**/

UINT8
USBKBDDisconnectDevice(
    DEV_INFO*   DevInfo
)
{
    UINT16      Index;
    UINT8       ScanCodeCount = (UINT8)(gUsbData->fpKBCScanCodeBufferPtr - 
                         (UINT8*)gUsbData->aKBCScanCodeBufferStart);
    UINT8       i = 0;
    UINT8       CurrentDeviceId;
    UINT8       Key;

    Index = USBKBDFindUSBKBDeviceTableEntry(DevInfo);
    if (Index == 0xFFFF) {
        USBLogError(USB_ERR_KBCONNECT_FAILED);
        return USB_ERROR;
    } else {
        CurrentDeviceId = (UINT8)(1 << ((DevInfo->bDeviceAddress) -1));
        while ((i < ScanCodeCount) && (ScanCodeCount != 0)) {     
            if (gUsbData->aKBCDeviceIDBufferStart[i] & CurrentDeviceId) {
                gUsbData->aKBCDeviceIDBufferStart[i] &= ~CurrentDeviceId;
                if (gUsbData->aKBCDeviceIDBufferStart[i] == 0) {
                    Key = gUsbData->aKBCScanCodeBufferStart[i]; 
                    if ((Key == HID_KEYBOARD_RIGHT_SHIFT) ||
                        (Key == HID_KEYBOARD_LEFT_SHIFT)) {
                        gUsbData->bUSBKBShiftKeyStatus &= ~(KB_RSHIFT_KEY_BIT_MASK+KB_LSHIFT_KEY_BIT_MASK);
                    }
                    USBKB_DiscardCharacter(&gUsbData->aKBCShiftKeyStatusBufferStart[i]); 
                    gUsbData->fpKBCScanCodeBufferPtr--;
                    ScanCodeCount--;
                    continue;
                }
            }
            i++;
        }

        DestroyKeyBuffer(&DevInfo->KeyCodeBuffer, MAX_KEY_ALLOWED, sizeof(UINT8));
        DestroyKeyBuffer(&DevInfo->UsbKeyBuffer, MAX_KEY_ALLOWED, sizeof(USB_KEY));
 
        gUsbData->aUSBKBDeviceTable[Index] = 0;
        return USB_SUCCESS;
    }
}

/**
    This routine is called with USB keyboard report data.  This
    routine handles the translation of  USB keyboard data
    into PS/2 keyboard data, and makes the PS/2 data available
    to software using ports 60/64h by communicating with
    PS/2 keyboard controller.

    @param HcStruc   Pointer to HCStruc
        DevInfo   Pointer to device information structure
        Td        Pointer to the polling TD
        Buffer    Pointer to the data buffer

    @retval Nothing

    @note  TD's control status field has the packet length (0 based).
      It could be one of three values 0,1 or 7 indicating packet
      lengths of 1, 2 & 8 repectively.
      The format of 8 byte data packet is as follow:
           Byte              Description
      -----------------------------------------------------------
          0   Modifier key (like shift, cntr & LED status)
          1   Reserved
          2   Keycode of 1st key pressed
          3   Keycode of 2nd key pressed
          4   Keycode of 3rd key pressed
          5   Keycode of 4th key pressed
          6   Keycode of 5th key pressed
          7   Keycode of 6th key pressed
      -----------------------------------------------------------
**/

UINT8
USBKBDProcessKeyboardData (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       *Td,
    UINT8       *Buffer,
    UINT16      DataLength
)
{
    UINT8               Count = 8;
	HID_REPORT_FIELD	*Field = NULL;
	UINT8				FieldIndex = 0;
	UINT32				BitOffset = 0;
	BOOLEAN				ValidData = FALSE;
	UINT8				Data = 0;
	UINT16				Index = 0;
	UINT8				UsageBuffer[32] = {0};
	UINT16				UsageIndex = 0;
	UINT8				i;
    UINTN               OemHookIndex;

	if (DevInfo->HidReport.Flag & HID_REPORT_FLAG_REPORT_PROTOCOL) {
		for (FieldIndex = 0; FieldIndex < DevInfo->HidReport.FieldCount; FieldIndex++) {
			Field = DevInfo->HidReport.Fields[FieldIndex];

			// Check if the field is input report.
			if (!(Field->Flag & HID_REPORT_FIELD_FLAG_INPUT)) {
				continue;
			}
	
			//if report id exist, check first byte
			if ((Field->ReportId != 0) && (Field->ReportId != Buffer[0])) {
				continue;
			}

			// Check if the field is contant.
            if (Field->Flag & HID_REPORT_FIELD_FLAG_CONSTANT) {
			    BitOffset += Field->ReportCount * Field->ReportSize;
                continue;
            }

            //find start offset
			if (Field->UsagePage == HID_UP_KEYBOARD) {
                ValidData = TRUE;
                
                // If Report ID tags are used in the report descriptor, the first byte is
                // report id, we offset 8 bits to get data.  
				if (Field->ReportId != 0) {
					BitOffset += 8;
				}

				for (Index = 0; Index < Field->ReportCount; Index++) {
					Data = ExtractInputReportData(Buffer, 
						BitOffset + (Index * Field->ReportSize), Field->ReportSize);

					if ((Data < Field->LogicalMin) || (Data > Field->LogicalMax)) {
						continue;
					}
                    
					Data = Field->Flag & HID_REPORT_FIELD_FLAG_VARIABLE ?
							(Data != 0 ? Field->Usages[Index] : Data) :
							Field->Usages[Data - Field->LogicalMin];

					if ((Data != 0) && (UsageIndex < COUNTOF(UsageBuffer))) {
						UsageBuffer[UsageIndex++] = Data;
					}
				}
                if (Field->ReportId != 0) {
                    BitOffset -= 8;
                }
            }
			BitOffset += Field->ReportCount * Field->ReportSize;
		}

		if (ValidData == FALSE) {
			return USB_SUCCESS;
		}

		ZeroMem(Buffer, 8);

        // Translate the report data to boot protocol data.
        
        // 0   Modifier key (like shift, cntr & LED status)
        // 1   Reserved
        // 2   Keycode of 1st key pressed
        // 3   Keycode of 2nd key pressed
        // 4   Keycode of 3rd key pressed
        // 5   Keycode of 4th key pressed
        // 6   Keycode of 5th key pressed
        // 7   Keycode of 6th key pressed

        for (Index = 0, i = 0; Index < UsageIndex; Index++) {
            if (UsageBuffer[Index] >= HID_KEYBOARD_LEFT_CTRL && 
                UsageBuffer[Index] <= HID_KEYBOARD_RIGHT_GUI) {
            	Buffer[0] |= 1 << (UsageBuffer[Index] - HID_KEYBOARD_LEFT_CTRL);
            } else {
            	if (i < 6) {
            		Buffer[i + 2] = UsageBuffer[Index];
					i++;
            	}
            }
        }               
	}

    // Call all the OEM hooks that wants to check KBD buffer    
    for (OemHookIndex = 0; KbdBufferCheckFunctionsList[OemHookIndex]; OemHookIndex++) {
        if (KbdBufferCheckFunctionsList[OemHookIndex](DevInfo, Buffer)) {
            return USB_SUCCESS;
		}
	}
	//Is KBC access allowed?
	if (gUsbData->IsKbcAccessBlocked) {
		if (!(gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) || !gUsbData->EfiMakeCodeGenerated) {
			return USB_SUCCESS;
		}
		ZeroMem(Buffer, 8);
	}

    //
    // Save the device info pointer for later use
    //
    gUsbData->fpKeyRepeatDevInfo = DevInfo;

    for (i = 0, Count = 8; i < 8; i++, Count--) {
		if (Buffer[i]) {
			break;
		}
    }

    if ((gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI)) {
        if (Count == 0) {
            gUsbData->EfiMakeCodeGenerated = FALSE;
        } else {
            gUsbData->EfiMakeCodeGenerated = TRUE;
            gUsbData->LegacyMakeCodeGenerated = FALSE;
        }
    } else {
        if (Count == 0) {
            gUsbData->LegacyMakeCodeGenerated = FALSE;
        } else {
            gUsbData->LegacyMakeCodeGenerated = TRUE;
            gUsbData->EfiMakeCodeGenerated = FALSE;
        }
    }

    //
    // checks for new key stroke.
    // if no new key got, return immediately.
    //
    for (i = 0; i < 8; i ++) {
        if (Buffer[i] != gLastKeyCodeArray[i]) {
            break;
        }
    }

#if USB_HID_KEYREPEAT_USE_SETIDLE == 1 
    if ((i == 8) && gKeyRepeatStatus) {
        USBKBDPeriodicInterruptHandler(HcStruc);
        return USB_SUCCESS;
    }
#endif

    //
    // Update LastKeycodeArray[] buffer in the
    // Usb Keyboard Device data structure.
    //
    for (i = 0; i < 8; i ++) {
        gLastKeyCodeArray[i] = Buffer[i];
    }
    
    if ((!(gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) || 
        gUsbData->LegacyMakeCodeGenerated) && (!gUsbData->EfiMakeCodeGenerated)) {
        if (Count == 0) {
            gUsbData->LegacyMakeCodeGenerated = FALSE;
        }
        UsbScanner(DevInfo, Buffer);
    } else {
        if (Count==0) {
            gUsbData->EfiMakeCodeGenerated = FALSE;
        }
        UsbKbdDataHandler(DevInfo, Buffer);
    }

    return USB_SUCCESS;
}

/**
    This routine is called every 16ms and is used to send
    the characters read from USB keyboard to the keyboard
    controller for legacy operation. Also this function updates
    the keyboard LED status

    @param fpHCStruc   Pointer to the HCStruc structure

    @retval VOID

**/

VOID
USBKBDPeriodicInterruptHandler (HC_STRUC* fpHcStruc)
{
    if (!(gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI)) {
        LegacyAutoRepeat(fpHcStruc);
    } else {
        USBKeyRepeat(NULL, 1);  // Disable Key repeat
    }
}

/**
    Process the keys that alter NumLock/ScrollLock/CapsLock; updates
    gUsbData->LastUsbKbShiftKeyStatus accordingly.

    @param USB key buffer

**/

VOID
ProcessLockKeysUnderOs(
    UINT8 *UsbKeys
)
{
    UINT8   i;

    for (i = 2; i < 8; i++) {
        switch (UsbKeys[i]) {
            case USB_NUM_LOCK_KEY_CODE:  // NumLock
                gUsbData->KbShiftKeyStatusUnderOs ^= KB_NUM_LOCK_BIT_MASK;
                break;
            case USB_CAPS_LOCK_KEY_CODE:  // CapsLock
                gUsbData->KbShiftKeyStatusUnderOs ^= KB_CAPS_LOCK_BIT_MASK;
                break;
            case USB_SCROLL_LOCK_KEY_CODE:  // ScrlLock
                gUsbData->KbShiftKeyStatusUnderOs ^= KB_SCROLL_LOCK_BIT_MASK;
                break;
        }
    }
}

/**
    This routine is executed to convert USB scan codes into PS/2
    make/bread codes.

    @param DevInfo   - USB keyboard device
        Buffer    - USB scan codes data buffer

    @retval VOID

**/

VOID
UsbScanner(
    DEV_INFO *DevInfo,
    UINT8 *Buffer
)
{
    if (gUsbData->kbc_support || ((gUsbData->dUSBStateFlag & USB_FLAG_6064EMULATION_ON) 
        && (gUsbData->dUSBStateFlag & USB_FLAG_6064EMULATION_IRQ_SUPPORT))) {
        USBKBC_GetAndStoreCCB();
        USBKB_Scanner(DevInfo, Buffer);
        if (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_OS) {
            ProcessLockKeysUnderOs(Buffer);
        }
        USBKB_UpdateLEDState(0xFF);
    }else {
        USBKB_Int9(Buffer);
    }
}

/**
    This routine is called periodically based on 8ms TD and used
    to implement the key repeat.

    @param Hc   Pointer to the HCStruc structure

    @retval VOID

**/

VOID
LegacyAutoRepeat(
    HC_STRUC *Hc
)
{
    if(gUsbData->kbc_support || ((gUsbData->dUSBStateFlag & USB_FLAG_6064EMULATION_ON) 
        && (gUsbData->dUSBStateFlag & USB_FLAG_6064EMULATION_IRQ_SUPPORT))) {
        SysKbcAutoRepeat(Hc);
    } else {
        SysNoKbcAutoRepeat();
    }
}

/**
    This routine set the USB keyboard LED status.

    @param DevInfo		Pointer to device information structure
        LedStatus	LED status

    @retval USB_SUCCESS or USB_ERROR

**/

UINT8
UsbKbdSetLed (
	DEV_INFO    *DevInfo,
	UINT8		LedStatus
)
{
	HC_STRUC	*HcStruc = gUsbData->HcTable[DevInfo->bHCNumber - 1];
	DEV_REQ		DevReq = {0};
	UINT8		Status;
	UINT8		ReportId = 0;
	UINT16		ReportLen;
	UINT8		*ReportData = NULL;
	UINT8		Index;

	if ((DevInfo->HidReport.Flag & HID_REPORT_FLAG_REPORT_PROTOCOL) && 
		!(DevInfo->HidReport.Flag & HID_REPORT_FLAG_LED_FLAG)) {
		return USB_ERROR;
	}

	ReportData = USB_MemAlloc(GET_MEM_BLK_COUNT(4));
	if (ReportData == NULL) return USB_ERROR;

	ReportLen = 1;
	ReportData[0] = LedStatus & 0x7;

	if (DevInfo->HidReport.Flag & HID_REPORT_FLAG_REPORT_PROTOCOL) {
        if (DevInfo->HidReport.Fields == NULL) {
            USB_MemFree(ReportData, GET_MEM_BLK_COUNT(4));
            return USB_ERROR;
        }
		for (Index = 0; Index < DevInfo->HidReport.FieldCount; Index++) {
			//find start offset
			if ((DevInfo->HidReport.Fields[Index]->UsagePage == HID_UP_LED) &&
				(DevInfo->HidReport.Fields[Index]->ReportId != 0) &&
				(DevInfo->HidReport.Fields[Index]->Usages[0] == HID_LED_NUM_LOCK)) {
				ReportId = DevInfo->HidReport.Fields[Index]->ReportId;
				ReportData[1] = ReportData[0];
				ReportData[0] = ReportId;
				ReportLen++;
			}
		}
	}
    if (DevInfo->IntOutEndpoint == 0) {
    	DevReq.wRequestType = HID_RQ_SET_REPORT;
    	DevReq.wValue = (0x02 << 8) | ReportId;		// Output
    	DevReq.wIndex = DevInfo->bInterfaceNum;
    	DevReq.wDataLength = ReportLen;

    	Status = UsbControlTransfer(HcStruc, DevInfo, DevReq, USB_KBD_SET_LED_TIMEOUT_MS, ReportData);
    } else {
        Status = UsbInterruptTransfer(HcStruc, DevInfo, DevInfo->IntOutEndpoint, 
            DevInfo->IntOutMaxPkt, ReportData, ReportLen, USB_KBD_SET_LED_TIMEOUT_MS);
    }

	USB_MemFree(ReportData, GET_MEM_BLK_COUNT(4));
	return Status;
}


/**
    Insert a key to key buffer.

    @param KeyBuffer	- Pointer to the key buffer
        Key			- The key to be inserted

    @retval VOID

**/

VOID
InsertKey (
	KEY_BUFFER	*KeyBuffer,
	VOID		*Key
)
{
    if ((KeyBuffer == NULL) || (KeyBuffer->Buffer == NULL) || (Key == NULL)) {
        return; 
    }

	//EfiCopyMem(KeyBuffer->Buffer[KeyBuffer->Head++], Key, KeyBuffer->KeySize);
	CopyMem(KeyBuffer->Buffer[KeyBuffer->Head], Key, KeyBuffer->KeySize);

	KeyBuffer->Head = ((KeyBuffer->Head + 1) % KeyBuffer->MaxKey);

    if (KeyBuffer->Head == KeyBuffer->Tail){
        //Drop data from buffer
		KeyBuffer->Tail = ((KeyBuffer->Tail + 1) % KeyBuffer->MaxKey);
    }
}


/**
    Find the specific key in key buffer.

    @param KeyBuffer	- Pointer to the key buffer
        Key			- The key to be inserted

    @retval VOID

**/

VOID*
FindKey (
	KEY_BUFFER	*KeyBuffer,
	VOID		*Key
)
{
	UINT8	Index;

    if ((KeyBuffer == NULL) || (KeyBuffer->Buffer == NULL) || (Key == NULL)) {
        return NULL; 
    }

	for (Index = KeyBuffer->Tail; 
		Index != KeyBuffer->Head; 
		Index = ((Index + 1) % KeyBuffer->MaxKey)) {
		if (CompareMem(KeyBuffer->Buffer[Index], Key, KeyBuffer->KeySize) == 0) {
			return KeyBuffer->Buffer[Index];
		}
	}

	return NULL;
}


/**
    Get the next available key in the buffer

    @param KeyBuffer	- Pointer to the key buffer
        Key			- The key to be inserted

    @retval VOID

**/

VOID*
GetNextKey (
	KEY_BUFFER	*KeyBuffer,
	VOID		*Key
)
{
	UINT8	Index;
	BOOLEAN	KeyFound = FALSE;

    if ((KeyBuffer == NULL) || (KeyBuffer->Buffer == NULL)) {
        return NULL;
    }

	for (Index = KeyBuffer->Tail; 
		Index != KeyBuffer->Head; 
		Index = ((Index + 1) % KeyBuffer->MaxKey)) {

		if (Key == NULL || KeyFound) {
			return KeyBuffer->Buffer[Index];
		}
		if (KeyBuffer->Buffer[Index] == Key) {
			KeyFound = TRUE;
		}
	}

	// If the key is not available, return the first available key
	if (!KeyFound && (KeyBuffer->Tail != KeyBuffer->Head)) {
		return KeyBuffer->Buffer[KeyBuffer->Tail];
	}

	return NULL;
}


/**
    Remove the key in key buffer.

    @param KeyBuffer	- Pointer to the key buffer
        Key			- The key to be inserted

    @retval VOID

**/

VOID
RemoveKey (
	KEY_BUFFER	*KeyBuffer,
	VOID		*Key
)
{
	UINT8	Index = 0;
	UINT8	NextIndex = 0;

    if ((KeyBuffer == NULL) || (KeyBuffer->Buffer == NULL) || (Key == NULL)) {
        return; 
    }

	for (Index = KeyBuffer->Tail; 
		Index != KeyBuffer->Head; 
		Index = ((Index + 1) % KeyBuffer->MaxKey)) {
		if (KeyBuffer->Buffer[Index] == Key) {
			break;
		}
	}
	
	if (Index == KeyBuffer->Head) {
		return;
	}

	for (; Index != KeyBuffer->Tail; 
		Index = (Index - 1 + KeyBuffer->MaxKey) % KeyBuffer->MaxKey) {
		NextIndex = ((Index - 1) + KeyBuffer->MaxKey) % KeyBuffer->MaxKey;
		CopyMem(KeyBuffer->Buffer[Index],
				KeyBuffer->Buffer[NextIndex], KeyBuffer->KeySize);
	}

	KeyBuffer->Tail = ((KeyBuffer->Tail + 1) % KeyBuffer->MaxKey);
}


/**
    This routine is called every time the keyboard data is updated

    @param HcStruc, DevInfo, fpBuf2 always NULL
        Buf1  - Pointer to 8 bytes USB data array

    @retval USB_SUCCESS or USB_ERROR

**/

UINT8
UsbKbdDataHandler (
    DEV_INFO    *DevInfo,
    UINT8       *Data
)
{
	UINT8		KeyCode;
	UINT8		Modifier;
	UINT8		WorkSpace[16] = {0};
	UINT8		WorkSpaceIndex = 0;
	UINT8		Index = 0;
	VOID		*Key = NULL;
	USB_KEY		UsbKey = {0};
    UINT8       *MemBlockEnd = gUsbData->fpMemBlockStart + (gUsbData->MemPages << 12);

    if ((DevInfo->UsbKeyBuffer.Buffer == NULL) || (DevInfo->KeyCodeBuffer.Buffer == NULL)) {
        return USB_ERROR;
    }
    if (((UINT8*)DevInfo->UsbKeyBuffer.Buffer < gUsbData->fpMemBlockStart) ||
       ((UINT8*)DevInfo->UsbKeyBuffer.Buffer > MemBlockEnd)) {
            return USB_ERROR;
    }
    if (((UINT8*)DevInfo->KeyCodeBuffer.Buffer < gUsbData->fpMemBlockStart) ||
       ((UINT8*)DevInfo->KeyCodeBuffer.Buffer > MemBlockEnd)) {
            return USB_ERROR;
    }
	// Translate the modifier to USB key code
	for (KeyCode = 0xE0, Modifier = Data[0];
		KeyCode <= 0xE7;
		KeyCode++, Modifier >>= 1) {
		if (Modifier & BIT0) {
			WorkSpace[WorkSpaceIndex++] = KeyCode;
		}
	}

	for (Index = 2; Index < 8 && Data[Index]; Index++) {
		// Check if the input overrun
		if (Data[Index] == 1) {
			UsbKey.KeyCode = Data[Index];
			UsbKey.Press = TRUE;
			InsertKey((KEY_BUFFER*)&DevInfo->UsbKeyBuffer, &UsbKey);
			return USB_SUCCESS;
		}
		WorkSpace[WorkSpaceIndex++] = Data[Index];
	}

	while (Key = GetNextKey(&DevInfo->KeyCodeBuffer, Key)) {
		KeyCode = *(UINT8*)Key;
		for (Index = 0; Index < WorkSpaceIndex; Index++) {
			if (KeyCode == WorkSpace[Index]) {
				break;
			}
		}

		if (Index == WorkSpaceIndex) {
			// The key in key code buffer is released
			RemoveKey((KEY_BUFFER*)&DevInfo->KeyCodeBuffer, Key);

			UsbKey.KeyCode = KeyCode;
			UsbKey.Press = FALSE;
			InsertKey((KEY_BUFFER*)&DevInfo->UsbKeyBuffer, &UsbKey);
		}
	}

	// Check if the key is in the key code buffer
	for (Index = 0; Index < WorkSpaceIndex && WorkSpace[Index]; Index++) {
		if (!FindKey(&DevInfo->KeyCodeBuffer, &WorkSpace[Index])) {
			// A new key pressed, insert the key code buffer
			InsertKey((KEY_BUFFER*)&DevInfo->KeyCodeBuffer, &WorkSpace[Index]);

			UsbKey.KeyCode = WorkSpace[Index];
			UsbKey.Press = TRUE;
			InsertKey((KEY_BUFFER*)&DevInfo->UsbKeyBuffer, &UsbKey);
		}
	}

	return USB_SUCCESS;
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
