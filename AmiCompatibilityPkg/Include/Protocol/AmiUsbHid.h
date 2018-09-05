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

/** @file AmiUsbHid.h
    Protocol used for Ami Usb Hid definitions

**/

#ifndef _AMIUSB_HID_PROTOCOL_H_
#define _AMIUSB_HID_PROTOCOL_H_

#define AMI_USB_HID_PROTOCOL_GUID \
    {0x1fede521, 0x31c, 0x4bc5, {0x80, 0x50, 0xf3, 0xd6, 0x16, 0x1e, 0x2e, 0x92}}

typedef struct _AMI_USB_HID_PROTOCOL AMI_USB_HID_PROTOCOL;

#define HID_DEV_TYPE_KEYBOARD           BIT0        
#define HID_DEV_TYPE_MOUSE              BIT1
#define HID_DEV_TYPE_POINT              BIT2
#define HID_DEV_TYPE_U2F                BIT3
#define HID_DEV_TYPE_OEM                BIT63

#define HID_BTYPE_KEYBOARD              0x1
#define HID_BTYPE_MOUSE                 0x2
#define HID_BTYPE_POINT                 0X3 

//----------------------------------------------------------------------------
// HID usage pages
//----------------------------------------------------------------------------
#define HID_UP_GENDESK                      0x01
#define HID_UP_KEYBOARD                     0x07
#define HID_UP_LED                          0x08
#define HID_UP_BUTTON                       0x09
#define HID_UP_DIGITIZER                    0x0D
#define HID_UP_FIDO                         0xF1D0 //Fast IDentity Online Alliance

#define HID_GENDESK_POINTER                 0x01
#define HID_GENDESK_MOUSE                   0x02
#define HID_GENDESK_KEYBOARD                0x06
#define HID_GENDESK_KEYPAD                  0x07
#define HID_GENDESK_X                       0x30
#define HID_GENDESK_Y                       0x31
#define HID_GENDESK_Z                       0x32
#define HID_GENDESK_WHEEL                   0x38

#define HID_KEYBOARD_LEFT_CTRL              0xE0
#define HID_KEYBOARD_LEFT_SHIFT             0xE1
#define HID_KEYBOARD_LEFT_ALT               0xE2
#define HID_KEYBOARD_LEFT    _GUI           0xE3
#define HID_KEYBOARD_RIGHT_CTRL             0xE4
#define HID_KEYBOARD_RIGHT_SHIFT            0xE5
#define HID_KEYBOARD_RIGHT_ALT              0xE6
#define HID_KEYBOARD_RIGHT_GUI              0xE7

#define HID_LED_NUM_LOCK                    0x01
#define HID_LED_CAP_LOCK                    0x02
#define HID_LED_SCROLL_LOCK                 0x03

#define HID_BUTTON_BUTTON1                  0x01

#define HID_DIGITIZER_TOUCH_SCREEN          0x04
#define HID_DIGITIZER_TIP_SWITCH            0x42    
#define HID_DIGITIZER_DEVICE_MODE           0x52
#define HID_DIGITIZER_DEVICE_IDENTIFIER     0x53

#define HID_FIDO_U2F_AD                     0x01    // U2F Authenticator Device
#define HID_FIDO_INPUT_REPORT_DATA          0x20
#define HID_FIDO_OUTPUT_REPORT_DATA         0x21

//----------------------------------------------------------------------------
//  Report descriptor struct define
//----------------------------------------------------------------------------
#define HID_REPORT_FIELD_FLAG_CONSTANT          BIT0        //0:DATA        1:CONSTANT
#define HID_REPORT_FIELD_FLAG_VARIABLE          BIT1        //0:ARRAY       1:VARIABLE
#define HID_REPORT_FIELD_FLAG_RELATIVE          BIT2        //0:ABSOLUTE    1:RELATIVE
#define HID_REPORT_FIELD_FLAG_INPUT             BIT4        //0:OUTPUT  1:INPUT

#define HID_REPORT_FLAG_REPORT_PROTOCOL             BIT0        //If use report protocol
#define HID_REPORT_FLAG_REPORT_ID                   BIT1        //1:REPORT_ID EXIST
#define HID_REPORT_FLAG_TOUCH_BUTTON_FLAG           BIT2        
#define HID_REPORT_FLAG_LED_FLAG                    BIT3        //1:LED 
#define HID_REPORT_FLAG_RELATIVE_DATA               BIT4
#define HID_REPORT_FLAG_ABSOLUTE_DATA               BIT5

//----------------------------------------------------------------------------
//  Report descriptor's hid_item
//----------------------------------------------------------------------------
typedef struct {
    UINT8   bSize;
    UINT8   bType;
    UINT8   bTag;
    union {
        UINT8   u8;
        UINT16  u16;
        UINT32  u32;
    } data;
} HID_ITEM;


//----------------------------------------------------------------------------
// HID Report define start
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// HID report item format
//----------------------------------------------------------------------------
#define HID_ITEM_FORMAT_SHORT                   0
#define HID_ITEM_FORMAT_LONG                    1

//----------------------------------------------------------------------------
// HID report descriptor item type (prefix bit 2,3)
//----------------------------------------------------------------------------
#define HID_ITEM_TYPE_MAIN                      0
#define HID_ITEM_TYPE_GLOBAL                    1
#define HID_ITEM_TYPE_LOCAL                     2
#define HID_ITEM_TYPE_RESERVED                  3

//----------------------------------------------------------------------------
// HID report descriptor main item tags
//----------------------------------------------------------------------------
#define HID_MAIN_ITEM_TAG_INPUT                     8
#define HID_MAIN_ITEM_TAG_OUTPUT                    9
#define HID_MAIN_ITEM_TAG_BEGIN_COLLECTION          10
#define HID_MAIN_ITEM_TAG_FEATURE                   11
#define HID_MAIN_ITEM_TAG_END_COLLECTION            12

//----------------------------------------------------------------------------
// HID report descriptor main item contents
//----------------------------------------------------------------------------
#define HID_MAIN_ITEM_CONSTANT                  0x001
#define HID_MAIN_ITEM_VARIABLE                  0x002
#define HID_MAIN_ITEM_RELATIVE                  0x004
#define HID_MAIN_ITEM_WRAP                      0x008
#define HID_MAIN_ITEM_NONLINEAR                 0x010
#define HID_MAIN_ITEM_NO_PREFERRED              0x020
#define HID_MAIN_ITEM_NULL_STATE                0x040
#define HID_MAIN_ITEM_VOLATILE                  0x080
#define HID_MAIN_ITEM_BUFFERED_BYTE             0x100

//----------------------------------------------------------------------------
// HID report descriptor collection item types
//----------------------------------------------------------------------------
#define HID_COLLECTION_PHYSICAL                 0
#define HID_COLLECTION_APPLICATION              1
#define HID_COLLECTION_LOGICAL                  2

//----------------------------------------------------------------------------
// HID report descriptor global item tags
//----------------------------------------------------------------------------
#define HID_GLOBAL_ITEM_TAG_USAGE_PAGE                  0
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM             1
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM             2
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM            3
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM            4
#define HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT               5
#define HID_GLOBAL_ITEM_TAG_UNIT                        6
#define HID_GLOBAL_ITEM_TAG_REPORT_SIZE                 7
#define HID_GLOBAL_ITEM_TAG_REPORT_ID                   8
#define HID_GLOBAL_ITEM_TAG_REPORT_COUNT                9
#define HID_GLOBAL_ITEM_TAG_PUSH                        10
#define HID_GLOBAL_ITEM_TAG_POP                         11

//----------------------------------------------------------------------------
// HID report descriptor local item tags
//----------------------------------------------------------------------------
#define HID_LOCAL_ITEM_TAG_USAGE                        0
#define HID_LOCAL_ITEM_TAG_USAGE_MINIMUM                1
#define HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM                2
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_INDEX             3
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MINIMUM           4
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MAXIMUM           5
#define HID_LOCAL_ITEM_TAG_STRING_INDEX                 7
#define HID_LOCAL_ITEM_TAG_STRING_MINIMUM               8
#define HID_LOCAL_ITEM_TAG_STRING_MAXIMUM               9

//----------------------------------------------------------------------------
// HID Report define end
//----------------------------------------------------------------------------

struct _AMI_USB_HID_PROTOCOL {
    UINT64                          HidDevType;        
};

#endif

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
