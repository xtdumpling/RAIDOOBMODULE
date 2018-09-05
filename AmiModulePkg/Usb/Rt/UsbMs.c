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

/** @file UsbMs.c
    AMI USB mouse support implementation

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "AmiUsb.h"
#include "UsbKbd.h"
#include <Library/BaseMemoryLib.h>
#include <Protocol/AmiUsbHid.h>

extern  USB_GLOBAL_DATA     *gUsbData;

extern  EFI_EMUL6064MSINPUT_PROTOCOL* gMsInput;

VOID        USBMSInitialize (VOID);
DEV_INFO*   USBMSConfigureDevice (HC_STRUC*, DEV_INFO*, UINT8*, UINT16, UINT16);
UINT8       USBMSProcessMouseData (HC_STRUC*, DEV_INFO*, UINT8*, UINT8*, UINT16);
VOID        USBKeyRepeat(HC_STRUC*, UINT8);
VOID		SetMouseData (UINT8*, USBMS_DATA*, UINT8, UINT8, HID_REPORT_FIELD*);		//(EIP127014+)		
EFI_STATUS  SendMouseData(PS2MouseData*);

/**
    This routine is called once to initialize the USB mouse data
    area

    @param VOID

    @retval VOID


**/

VOID
USBMSInitialize()
{
    //
    // Initialize the mouse input buffer head and tail values
    //
    gUsbData->MsOrgButtonStatus = 0;
    gUsbData->fpMouseInputBufferHeadPtr = &gUsbData->aMouseInputBuffer[0];
    gUsbData->fpMouseInputBufferTailPtr = &gUsbData->aMouseInputBuffer[0];
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USBMSInitialize: Head and Tail are at %x\n", gUsbData->fpMouseInputBufferHeadPtr);
}

/**
    This routine checks an interface descriptor of the USB device
    detected to see if it describes a HID/Boot/Mouse device.
    If the device matches the above criteria, then the device is
    configured and initialized

    @param HcStruc   HCStruc pointer
        DevInfo   Device information structure pointer
        Desc      Pointer to the descriptor structure
        Start      Offset within interface descriptor
        supported by the device
        End        End offset of the device descriptor

    @retval FPDEV_INFO  New device info structure, 0 on error

**/

DEV_INFO*
USBMSConfigureDevice (
    HC_STRUC*   HcStruc,
    DEV_INFO*   DevInfo,
    UINT8*      Desc,
    UINT16      Start,
    UINT16      End)
{
    return NULL;
}

/**
    This function is called at regular intervals with USB mouse
    report data. This function handles the translation of USB
    mouse data into PS/2 mouse data, and makes the PS/2 data
    available to software using ports 60/64 to communicate with
    a PS/2 mouse.

    @param HcStruc   Pointer to HCStruc
        DevInfo   Pointer to device information structure
        Td        Pointer to the polling TD
        Buffer    Pointer to the data buffer

    @retval USB_SUCCESS

    @note  The format of 3 byte data packet is as follow:
               Byte              Description
          -----------------------------------------------------------
              0   Bit     Description
              -------------------------------------------
                   0      If set, button 1 is pressed
                   1      If set, button 2 is pressed
                   2      If set, button 3 is pressed
                   3-7        Reserved
              -------------------------------------------
              1   X displacement value
              2   Y displacement value
          -----------------------------------------------------------
**/

UINT8
USBMSProcessMouseData (
    HC_STRUC    *HcStruc,
    DEV_INFO    *DevInfo,
    UINT8       *Td,
    UINT8       *Buffer,
    UINT16      DataLength
)
{
    UINT8*              MachineConfigPtr = (UINT8*)(UINTN)0x410;
    PS2MouseData        MouseData;
    USBMS_DATA          TempData;
    INT32               Coordinates;
	HID_REPORT_FIELD 	*Field = NULL;
	UINT8 	            OffsetTmp = 0;
    UINT8               XStart;
    UINT8               XEnd;
    UINT8               YStart;
    UINT8               YEnd;
    UINT8               ButtonStart;
    UINT8               WheelStart;
    UINT8               i;
    UINT16              j;
    UINT8               ButtonSet = 0;
    UINT8               XSet = 0;
    UINT8               YSet = 0;
    UINT8               WheelSet = 0;

    //Is KBC access allowed?
    if (gUsbData->IsKbcAccessBlocked) {
        return USB_SUCCESS; //(EIP29733+)
    }
    
    ZeroMem(&TempData, sizeof(USBMS_DATA));

    if (DevInfo->HidReport.Flag & HID_REPORT_FLAG_REPORT_PROTOCOL) {
        //serach button and X Y
        for (i = 0; i < DevInfo->HidReport.FieldCount; i++) {
			Field = DevInfo->HidReport.Fields[i];

            //Check if it is input?
            if (!(Field->Flag & HID_REPORT_FIELD_FLAG_INPUT)) {
				continue;
            }
            //if report id is exist, check first byte
            if (Field->ReportId != 0 && Field->ReportId != Buffer[0]) {
				continue;
            }

            // Check if the field is contant.
            if (Field->Flag & HID_REPORT_FIELD_FLAG_CONSTANT) {
                OffsetTmp += Field->ReportCount * Field->ReportSize;
                continue;
            }
            
            //Check Button
            if ((Field->UsagePage == HID_UP_BUTTON) && 
                (Field->UsageCount != 0) && 
                (Field->Usages[0] == HID_BUTTON_BUTTON1)) {
                
                ButtonSet = 1;
                ButtonStart = OffsetTmp;
                if (Field->ReportId != 0) {
                    ButtonStart += 8;
                }
                ButtonStart /= 8;
                TempData.ButtonByte = *(Buffer + ButtonStart);
            }
            //Check X,Y
			if ((Field->UsagePage == HID_UP_GENDESK) && (Field->UsageCount != 0)) {
                for (j = 0; j < Field->UsageCount; j++) {
					//find X
                    if (Field->Usages[j] == HID_GENDESK_X) {
                        XSet = 1;         
                        XStart = (OffsetTmp + j * Field->ReportSize); 
                        if (Field->ReportId != 0) {
                            XStart += 8;
                        }
                        XEnd = XStart + Field->ReportSize;
                        TempData.FillUsage = HID_GENDESK_X;
                        SetMouseData(Buffer, &TempData, XStart, XEnd, Field);
					}
                    //find Y
					if (Field->Usages[j] == HID_GENDESK_Y) {
                        YSet = 1;         
						YStart = (OffsetTmp + j * Field->ReportSize);
                        if (Field->ReportId != 0) {
                            YStart += 8;
                        }
						YEnd = YStart + Field->ReportSize;
                        TempData.FillUsage = HID_GENDESK_Y;
						SetMouseData(Buffer, &TempData, YStart, YEnd, Field);
					}
					//find Wheel
                    if (Field->Usages[j] == HID_GENDESK_WHEEL) {
                        WheelSet = 1;
                        WheelStart = (OffsetTmp + j * Field->ReportSize) / 8;
                        if (Field->ReportId != 0) {
                            WheelStart += 1;
                        }
                        TempData.Z = *(Buffer + WheelStart);
					}
				}
			}
            OffsetTmp += Field->ReportCount * Field->ReportSize;
		}
                                        
        for (i = 0; i < 8; i++) {
        	Buffer[i] = 0;
        }

		//fill MS DATA
        if (ButtonSet != 0) {
    		*Buffer = TempData.ButtonByte;
    		gUsbData->MsOrgButtonStatus = TempData.ButtonByte;
        } else {
            *Buffer = gUsbData->MsOrgButtonStatus;
        }
        
        if (XSet == 1) {
    	    *(Buffer + 1) = TempData.X;
        }
        if (YSet == 1) {
		    *(Buffer + 2) = TempData.Y;
        }
        if (WheelSet == 1) { 
		    *(Buffer + 3) = TempData.Z;
        }
	} else {
        TempData.EfiX = *((INT8*)Buffer + 1);
        TempData.EfiY = *((INT8*)Buffer + 2);
    }

    if ((!(DevInfo->HidReport.Flag & HID_REPORT_FLAG_REPORT_PROTOCOL)) && 
            (DevInfo->wIncompatFlags & USB_INCMPT_BOOT_PROTOCOL_IGNORED)) {
        Buffer++;
    }
    
	if ((BOOT_PROTOCOL_SUPPORT == 0) && 
        !(DevInfo->wIncompatFlags & USB_INCMPT_HID_BOOT_PROTOCOL_ONLY)) {
        if (!(ButtonSet || XSet || YSet || WheelSet)) {
            return USB_SUCCESS;
        }
    }
    
    if (gUsbData->dUSBStateFlag & USB_FLAG_RUNNING_UNDER_EFI) {

        gUsbData->MouseData.ButtonStatus = *(UINT8*)Buffer;

        Coordinates = (INT16)TempData.EfiX;		//(EIP127014)
        gUsbData->MouseData.MouseX += Coordinates;

        Coordinates = (INT16)TempData.EfiY;		//(EIP127014)
        gUsbData->MouseData.MouseY += Coordinates;

        Coordinates= *((INT8*)Buffer + 3);
        gUsbData->MouseData.MouseZ += Coordinates;

        return USB_SUCCESS; // Here should be code that prepares buffer for AMIUHCD
    }

    if (!(*MachineConfigPtr & BIT2)) {
        return USB_SUCCESS;    // No mouse indication in BIOS Data area equipment byte
    }

    //
    // Check mouse data availability
    //
    if (gMsInput != 0) {
        //
        // Get mouse status byte and prepare it.
        // Bit 2, 1, 0 = Middle, right and left button status
        // Bit 3 is always 1
        //
        MouseData.flags = (*(UINT8*)Buffer) & 7 | 8;

        //
        // Get mouse X, Y position
        //
        MouseData.x = (*((UINT8*)Buffer + 1));
        MouseData.y = (UINT8)(-*((INT8*)Buffer + 2)); // Y data is opposite in USB than PS2

        //
        // Verify the direction of X-axis movement
        //
        if (MouseData.x >= 0x80) {
            MouseData.flags |= 0x10;    // Negative X-axis movement
        }
        if (MouseData.y >= 0x80) {
            MouseData.flags |= 0x20;    // Negative Y-axis movement
        }

        if (gUsbData->kbc_support || (gUsbData->dUSBStateFlag & USB_FLAG_6064EMULATION_ON)) {
            SendMouseData(&MouseData);
            USBKeyRepeat(NULL, 2);          // Enable Key repeat	//(EIP49214+)
        }				    
    }

    return USB_SUCCESS;
}
										//(EIP127014+)>
/**
    This routine checks for mouse type device from the
    interface data provided

    @param bBaseClass  USB base class code
        bSubClass   USB sub-class code
        bProtocol   USB protocol code

    @retval BIOS_DEV_TYPE_MOUSE type on success or 0FFH on error

**/

VOID
SetMouseData (
    UINT8           	*Buffer,
    USBMS_DATA 	    	*MsData,
    UINT8 		    	Start,
    UINT8 		    	End,
    HID_REPORT_FIELD	*Field
)
{
	UINT8 	ReportSize;
    UINT8   Size;
    UINT8   PreSkip;
    UINT8   PostSkip;
	UINT16  TempData = 0;
    UINT16  MinMask = 0;
    UINT16  Multi = 1;
    UINT16  Resolution;
    UINT16  Count = 0;
    UINT16  i;

    if ((Field->PhysicalMax == 0) && (Field->PhysicalMin == 0)) {
        Field->PhysicalMax = Field->LogicalMax;
        Field->PhysicalMin = Field->LogicalMin;
    }
    if (Field->UnitExponent != 0) {
        Count = (~Field->UnitExponent) + 1;
    }
    
    for (i = 0; i < Count; i++){
        Multi = Multi * 10;
    }        
    
    Resolution = ((INT16)Field->LogicalMax - (INT16)Field->LogicalMin) * Multi /
                 ((INT16)Field->PhysicalMax - (INT16)Field->PhysicalMin);

	ReportSize = End - Start;
    MinMask = ((~MinMask) >> ReportSize) << ReportSize;

	Size = ReportSize / 8;
    
    if ((ReportSize % 8) != 0) {
        Size++;
    }

	ASSERT(Size > 0 && Size <= sizeof(TempData));
    if ((Size == 0) || (Size > sizeof(TempData))) {
        return;
    }

    CopyMem(&TempData, Buffer + Start / 8, Size);

	PreSkip = Start % 8;
	PostSkip = End % 8;
		
    if (PreSkip != 0) {
        TempData = TempData >> PreSkip;
    }
    if (PostSkip != 0) {
        TempData = TempData << PostSkip; 	
        TempData = TempData >> PostSkip; 			
	}	

    if (TempData > Field->LogicalMax) {
        TempData |= MinMask;
    }

    if (MsData->FillUsage == HID_GENDESK_X) {
        MsData->EfiX = TempData;
        MsData->X = (UINT8)TempData; 
    }
    if (MsData->FillUsage == HID_GENDESK_Y) {
        MsData->EfiY = TempData;
        MsData->Y = (UINT8)TempData; 
    } 

	return;
} 
										//<(EIP127014+)
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
