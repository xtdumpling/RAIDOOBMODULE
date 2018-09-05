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

/** @file UsbDef.h
    AMI USB driver definitions

**/

// AVOID including multiple instance of this file
#ifndef     __USB_H
#define     __USB_H

#include    <Token.h>
#include    <AmiDxeLib.h>

#include    <Protocol/UsbHc.h>
#include    <Protocol/AmiUsbController.h>

#include    "Uhci.h"
#include    "Ohci.h"
#include    "Ehci.h"

#include	<Protocol/PciIo.h>
#include    <Protocol/DevicePath.h>
#include    <Protocol/UsbPolicy.h>

//
// USB Module version number
//
#define     USB_MAJOR_VER               USB_DRIVER_MAJOR_VER
#define     USB_ACTIVE                  0xFC
#define     USB_LEGACY_ENABLE           0x01
#define     USB_6064_ENABLE             0x02

#define MAX_DEVICE_TYPES                0x07        // 7 different types of devices
#define MAX_HC_TYPES                    0x04        // 4 different types of host controllers
#define MAX_MASS_DEVICES                0x06
#define MAX_CCID_DEVICES                0x06

#define BIOS_DEV_TYPE_HID				0x01			
//#define BIOS_DEV_TYPE_KEYBOARD          0x01
//#define BIOS_DEV_TYPE_MOUSE             0x02
#define BIOS_DEV_TYPE_HUB               0x03
#define BIOS_DEV_TYPE_STORAGE           0x04
#define BIOS_DEV_TYPE_SECURITY          0x05
#define BIOS_DEV_TYPE_USBBUS            0x06        // Generic USB device driver
#define BIOS_DEV_TYPE_USBBUS_SHADOW     0x07        // Dummy device type for temp usage
#define BIOS_DEV_TYPE_CCID              0x08        // CCID device type

#define MAX_DEVICES     (USB_DEV_HID_COUNT+USB_DEV_MASS_COUNT+USB_DEV_HUB_COUNT+USB_DEV_CCID_COUNT+USB_DEV_UNSUPPORTED)


// USB HC type
#define     USB_HC_UHCI                 0x10
#define     USB_HC_OHCI                 0x20
#define     USB_HC_EHCI                 0x30
#define     USB_HC_XHCI                 0x40
#if !USB_RT_DXE_DRIVER
#define     GET_HCD_INDEX(bHCType)      (((bHCType) - USB_HC_UHCI) >> 4)
#else
#define     GET_HCD_INDEX(bHCType)       ((((bHCType) - USB_HC_UHCI) >> 4) + MAX_HC_TYPES)
#endif
#define     USB_INDEX_UHCI              (GET_HCD_INDEX(USB_HC_UHCI))
#define     USB_INDEX_OHCI              (GET_HCD_INDEX(USB_HC_OHCI))
#define     USB_INDEX_EHCI              (GET_HCD_INDEX(USB_HC_EHCI))
#define     USB_INDEX_XHCI              (GET_HCD_INDEX(USB_HC_XHCI))

#define     USB_MEM_BLK_SIZE            32  // 32 bytes
#define     USB_MEM_BLK_SIZE_SHIFT      5   // log2 (USB_MEM_BLK_SIZE)

#define     USB_FORCE_64BIT_ALIGNMENT   1

#if USB_FORCE_64BIT_ALIGNMENT
#define USB_MEM_ALLOCATION_UNIT_SIZE 64
#else
#define USB_MEM_ALLOCATION_UNIT_SIZE 32
#endif

// The following macro returns number of memory blocks needed for the structure provided
#define     GET_MEM_BLK_COUNT_STRUC(Struc)      ((sizeof(Struc)+(sizeof(MEM_BLK)-1))/sizeof(MEM_BLK))

// The following macro returns number of memory blocks needed for the size of data provided
#define     GET_MEM_BLK_COUNT(Size)             (((Size) + (sizeof(MEM_BLK)-1))/sizeof(MEM_BLK))

#define     MAX_SPLIT_PERIODIC_NUMBER   0x07
//#define       TEMP_BUFFER_SIZE            0x80    // Size of temp buffer
//#define       CONTROL_DATA_SIZE           0x100
#define     MAX_CONTROL_DATA_SIZE       0x800
#define     MAX_TEMP_BUFFER_SIZE        0x80    // Size of temp buffer
#define     MAX_CONSUME_BUFFER_SIZE         0x1000  //(EIP59738+)
// USB state flag equates
#define     USB_FLAG_ENABLE_BEEP_MESSAGE    0x0001  // BIT 0
#define     USB_FLAG_RUNNING_UNDER_EFI      0x0002  // BIT 1
#define     USB_FLAG_DISABLE_LEGACY_SUPPORT 0x0004  // BIT 2
#define     USB_FLAG_6064EMULATION_ON       0x0008  // BIT 3
#define     USB_FLAG_RUNNING_UNDER_OS       0x0010  // BIT 4
#define     USB_FLAG_DRIVER_CONSISTENT      0x0020  // BIT 5 //AMI Tracker 27603
#define     USB_FLAG_DRIVER_STARTED         0x0080  // BIT 7
#define     USB_FLAG_6064EMULATION_IRQ_SUPPORT  0x0100  // BIT 8
#define     USB_HOTPLUG_FDD_ENABLED         0x0200  // BIT 9
#define     USB_HOTPLUG_HDD_ENABLED         0x0400  // BIT 10
#define     USB_HOTPLUG_CDROM_ENABLED       0x0800  // BIT 11
#define     USB_FLAG_MASS_NATIVE_EMULATION  0x1000  // BIT 12
#define     USB_FLAG_MASS_MEDIA_CHECK       0x2000  // BIT 13
#define     USB_FLAG_MASS_SKIP_FDD_MEDIA_CHECK  0x4000  // BIT 14
#define     USB_FLAG_EFIMS_DIRECT_ACCESS    0x8000  // BIT15
#define     USB_FLAG_SKIP_CARD_READER_CONNECT_BEEP  0x10000 //BIT16 //(EIP64781+)
#define     USB_FLAG_MASS_SIZE_EMULATION    0x20000 //BIT17 //(EIP80382+)
#define     USB_FLAG_MASS_EMULATION_FOR_NO_MEDIA    0x40000 //BIT18	//(EIP86793+)
#define     USB_FLAG_CSM_ENABLED            0x80000 // BIT19

// PCI related equates
    // Invalid PCI register address bits
#define     PCI_REG_MAX_ADDRESS         0xFF00
#define     PCI_REG_ADDRESS_BYTE        PCI_REG_MAX_ADDRESS + 0x000
#define     PCI_REG_ADDRESS_WORD        PCI_REG_MAX_ADDRESS + 0x001
#define     PCI_REG_ADDRESS_DWORD       PCI_REG_MAX_ADDRESS + 0x003

// For systems with config mechanism 1
#define     CFG_SPACE_INDEX_REG         0xCF8
#define     CFG_SPACE_DATA_REG          0xCFC

// Standard PCI configuration register offsets and relevant values
//------------------------------------------------------------------------------
#define        PCI_REG_VENDID       0x00    //PCI vendor ID register
#define        PCI_REG_DEVID        0x02    //PCI device ID register
#define        PCI_REG_COMMAND      0x04    //PCI command register

//----------------------------------------------------------------------------
//          USB API equates
//----------------------------------------------------------------------------
#define     USB_NEW_API_START_FUNC          0x20

#define     USB_API_CHECK_PRESENCE          0x00
#define     USB_API_START                   0x20
#define     USB_API_STOP                    0x21
#define     USB_API_DISABLE_INTERRUPTS      0x22
#define     USB_API_ENABLE_INTERRUPTS       0x23
#define     USB_API_MOVE_DATA_AREA          0x24
#define     USB_API_GET_DEVICE_INFO         0x25
#define     USB_API_CHECK_DEVICE_PRESENCE   0x26
#define     USB_API_MASS_DEVICE_REQUEST     0x27
#define     USB_API_POWER_MANAGEMENT        0x28
#define     USB_API_PREPARE_FOR_OS          0x29
#define     USB_API_SECURITY_INTERFACE      0x2A
#define     USB_API_LIGHTEN_KEYBOARD_LEDS   0x2B
#define     USB_API_CHANGE_OWNER            0x2C
#define     USB_API_HC_PROC                 0x2D
#define     USB_API_CORE_PROC               0x2E
#define     USB_API_KBC_ACCESS_CONTROL      0x30    //(EIP29733+)
#define     USB_API_LEGACY_CONTROL          0x31    //
#define     USB_API_GET_DEV_ADDR            0x32
#define     USB_API_EXT_DRIVER_REQUEST      0x33
#define     USB_API_CCID_DEVICE_REQUEST     0x34
#define     USB_API_USB_STOP_CONTROLLER     0x35	//(EIP74876+)
#define		USB_API_HC_START_STOP			0x36

#define     USB_MASSAPI_GET_DEVICE_INFO     0x000
#define     USB_MASSAPI_GET_DEVICE_GEOMETRY 0x001
#define     USB_MASSAPI_RESET_DEVICE        0x002
#define     USB_MASSAPI_READ_DEVICE         0x003
#define     USB_MASSAPI_WRITE_DEVICE        0x004
#define     USB_MASSAPI_VERIFY_DEVICE       0x005
#define     USB_MASSAPI_FORMAT_DEVICE       0x006
#define     USB_MASSAPI_CMD_PASS_THRU       0x007
#define     USB_MASSAPI_ASSIGN_DRIVE_NUMBER 0x008
#define     USB_MASSAPI_CHECK_DEVICE        0x009
#define     USB_MASSAPI_GET_MEDIA_STATUS    0x00A
#define     USB_MASSAPI_GET_DEV_PARMS       0x00B
#define     USB_MASSAPI_EFI_READ_DEVICE     0x00C
#define     USB_MASSAPI_EFI_WRITE_DEVICE    0x00D
#define     USB_MASSAPI_EFI_VERIFY_DEVICE   0x00E

#define     USB_MASS_MEDIA_PRESENT              BIT0
#define     USB_MASS_MEDIA_CHANGED              BIT1
#define	    USB_MASS_GET_MEDIA_FORMAT	        BIT2		//(EIP13457+)
#define     USB_MASS_MEDIA_REMOVEABLE           BIT3

#define     USB_SECURITY_API_READ_DEVICE    0x000
#define     USB_SECURITY_API_WRITE_DEVICE   0x001

#define     USB_PM_SUSPEND                  0x010
#define     USB_PM_RESUME                   0x020

// Error returned from API handler
#define     USB_SUCCESS             0x000
#define     USB_PARAMETER_ERROR     0x010
#define     USB_NOT_SUPPORTED       0x020
#define     USBAPI_INVALID_FUNCTION 0x0F0
#define     USB_ERROR               0x0FF

//
// Bit definitions for a generic pointer
//
#define        TERMINATE        0x00000001
#define        QUEUE_HEAD       0x00000002
#define        VERTICAL_FLAG    0x00000004
#define        LINK_POINTER     0xFFFFFFF0
// Mass storage data sync equates
#define USB_BULK_IN_DATA_SYNC           BIT0
#define USB_BULK_IN_DATA_SYNC_SHIFT     0
#define USB_BULK_OUT_DATA_SYNC          BIT1
#define USB_BULK_OUT_DATA_SYNC_SHIFT    1
#define USB_INT_DATA_SYNC               BIT2
#define USB_INT_DATA_SYNC_SHIFT         2

// Highest possible device address
#define MAX_DEVICE_ADDR                 MAX_DEVICES
// Addr that is guaranted not to be used
#define DUMMY_DEVICE_ADDR               (MAX_DEVICE_ADDR + 1)

#define DEFAULT_PACKET_LENGTH           8       // Max size of packet data

// USB BIOS related error codes
#define USB_ERROR_CODE_START            0x8100
#define ERRUSB_HC_NOT_FOUND             (USB_ERROR_CODE_START + 1)
#define ERRUSB_DEVICE_INIT              (USB_ERROR_CODE_START + 2)
#define ERRUSB_DEVICE_DISABLED          (USB_ERROR_CODE_START + 3)
#define ERRUSB_OHCI_EMUL_NOT_SUPPORTED  (USB_ERROR_CODE_START + 4)
#define ERRUSB_EHCI_64BIT_DATA_STRUC    (USB_ERROR_CODE_START + 5)

// USB internal error codes
#define USB_ERR_DEV_INIT_MEM_ALLOC              0x01
#define USB_ERR_DEV_INIT_GET_DESC_8             0x02
#define USB_ERR_DEV_INIT_SET_ADDR               0x03
#define USB_ERR_DEV_INIT_GET_DESC_100           0x04
#define USB_ERR_DEV_INIT_GET_DESC_200           0x05
#define USB_ERR_NO_DRIVER                       0x20
#define USB_ERR_NO_HCSTRUC                      0x21
#define USB_ERR_STARTHC_NO_MEMORY               0x22
#define USB_ERR_KBCONNECT_FAILED                0x23
#define USB_ERR_HC_RESET_FAILED                 0x24
#define USB_ERR_PORT_RESET_FAILED               0x25
#define USB_ERR_CONTROL_XFER_TIMEOUT            0x80
//----------------------------------------------------------------------------
//          Descriptor Type Values
//---------------------------------------------------------------------------
#define DESC_TYPE_DEVICE        1   // Device Descriptor (Type 1)
#define DESC_TYPE_CONFIG        2   // Configuration Descriptor (Type 2)
#define DESC_TYPE_STRING        3   // String Descriptor (Type 3)
#define DESC_TYPE_INTERFACE     4   // Interface Descriptor (Type 4)
#define DESC_TYPE_ENDPOINT      5   // Endpoint Descriptor (Type 5)
										//(EIP38434+)>
#define DESC_TYPE_REPORT        0x22 // Report Descriptor (Type 22h)
#define DESC_TYPE_HID           0x21 // HID Descriptor (Type 21h)
										//<(EIP38434+)
#define DESC_TYPE_HUB           0x29 // Hub Descriptor (Type 29h)
#define DESC_TYPE_SS_HUB		0x2A

#define DESC_TYPE_SS_EP_COMP        48  //SuperSpeed Endpoint Companion Decsriptor

#define DESC_TYPE_CLASS_HUB     0x2900   // Hub Class Descriptor (Type 0)
#define DESC_TYPE_IAD           0xB // Interface Association


//----------------------------------------------------------------------------
//          Usb device requests timeout
//---------------------------------------------------------------------------

#ifndef USB_GET_CONFIG_DESC_TIMEOUT_MS
#define USB_GET_CONFIG_DESC_TIMEOUT_MS          500
#endif

#ifndef USB_GET_STRING_DESC_TIMEOUT_MS
#define USB_GET_STRING_DESC_TIMEOUT_MS          100
#endif

#ifndef USB_GET_SERIAL_NUMBER_DESC_TIMEOUT_MS
#define USB_GET_SERIAL_NUMBER_DESC_TIMEOUT_MS   3000
#endif

#ifndef USB_GET_REPORT_DESC_TIMEOUT_MS
#define USB_GET_REPORT_DESC_TIMEOUT_MS          500
#endif

#ifndef USB_SUSPEND_HUB_PORT_TIMEOUT_MS
#define USB_SUSPEND_HUB_PORT_TIMEOUT_MS			200
#endif

#ifndef USB_HUB_WARM_RESET_PORT_TIMEOUT_MS
#define USB_HUB_WARM_RESET_PORT_TIMEOUT_MS		500
#endif

#ifndef USB_PORT_CONNECT_STABLE_DELAY_MS
#define USB_PORT_CONNECT_STABLE_DELAY_MS 100
#endif

#ifndef USB_DEVICE_ENUMERATE_RETRY_COUNT
#define USB_DEVICE_ENUMERATE_RETRY_COUNT        6
#endif

//----------------------------------------------------------------------------
//  USB protocol related routines
//----------------------------------------------------------------------------

#define MAX_USB_ERROR_RETRY     01

// USB Version structure
typedef struct {
    UINT8   bMajor;
    UINT8   bMinor;
    UINT8   bBugRel;
} USB_VERSION;

typedef struct {
    UINT8   aBuf[32];
} MEM_BLK;

#define		MEM_BLK_COUNT			(MEM_PAGE_COUNT * (4096 / USB_MEM_BLK_SIZE))
#define     MEM_BLK_STS_COUNT       (MEM_BLK_COUNT / 32)


typedef struct {
    UINT16  wPCIDev;
    UINT16  wHCType;
} HC_PCI_INFO;

#pragma pack(push, 1)

typedef struct {
    UINT8       bDescLength;
    UINT8       bDescType;
    UINT16      wTotalLength;
    UINT8       bNumInterfaces;
    UINT8       bConfigValue;
    UINT8       bConfigString;
    UINT8       Attributes;
    UINT8       bConfigPower;
} CNFG_DESC;

typedef struct {
    UINT8          bDescLength;
    UINT8          bDescType;
    UINT8          bInterfaceNum;
    UINT8          bAltSettingNum;
    UINT8          bNumEndpoints;
    UINT8          bBaseClass;
    UINT8          bSubClass;
    UINT8          bProtocol;
    UINT8          bInterfaceString;
}INTRF_DESC;

typedef struct {
    UINT8           DescLength;
    UINT8           DescType;
    UINT8           MaxBurst;
    UINT8           Attributes;
    UINT16          BytesPerInterval;
} SS_ENDP_COMP_DESC;

typedef struct {
    UINT8           bDescLength;
    UINT8           bDescType;
    UINT8           bEndpointAddr;
    UINT8           bEndpointFlags;
    UINT16          wMaxPacketSize;
    UINT8           bPollInterval;
} ENDP_DESC;

										//(EIP38434+)>
typedef struct {
    UINT8          bDescLength;
    UINT8          bDescType;
    UINT16         bcdHID;
    UINT8          bCountryCode;
    UINT8          bNumEndpoints;
    UINT8          bDescriptorType;
    UINT16         bDescriptorLength;
}HID_DESC;

#pragma pack(pop)


//----------------------------------------------------------------------------
//  Config descriptor bmAttributes define
//----------------------------------------------------------------------------
#define CNFG_DESC_ATT_REMOTEWAKEUP              BIT5
#define CNFG_DESC_ATT_SELFPOWER                 BIT6

										//<(EIP38434+)
//----------------------------------------------------------------------------
//      Bit definitions for EndpointDescriptor.EndpointAddr
//----------------------------------------------------------------------------
#define EP_DESC_ADDR_EP_NUM     0x0F    //Bit 3-0: Endpoint number
#define EP_DESC_ADDR_DIR_BIT    0x80    //Bit 7: Direction of endpoint, 1/0 = In/Out
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  Bit definitions for EndpointDescriptor.EndpointFlags
//----------------------------------------------------------------------------
#define EP_DESC_FLAG_TYPE_BITS  0x03    //Bit 1-0: Indicate type of transfer on endpoint
#define EP_DESC_FLAG_TYPE_CONT  0x00    //Bit 1-0: 00 = Endpoint does control transfers
#define EP_DESC_FLAG_TYPE_ISOC  0x01    //Bit 1-0: 01 = Endpoint does isochronous transfers
#define EP_DESC_FLAG_TYPE_BULK  0x02    //Bit 1-0: 10 = Endpoint does bulk transfers
#define EP_DESC_FLAG_TYPE_INT   0x03    //Bit 1-0: 11 = Endpoint does interrupt transfers
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Values for InterfaceDescriptor.BaseClass
//---------------------------------------------------------------------------
#define BASE_CLASS_HID              0x03
#define BASE_CLASS_MASS_STORAGE     0x08
#define BASE_CLASS_HUB              0x09
//----------------------------------------------------------------------------


//---------------------------------------------------------------------------
//      Values for InterfaceDescriptor.SubClass
//---------------------------------------------------------------------------
#define SUB_CLASS_BOOT_DEVICE       0x01    // Boot device sub-class
#define SUB_CLASS_HUB               0x00    //Hub Device Sub Class?

// Mass storage related sub-class equates
#define SUB_CLASS_RBC                   0x01    // RBC T10 project,1240-D, e.g. Flash
#define SUB_CLASS_SFF8020I              0x02    // SFF8020I, e.g. ATAPI CD-ROM
#define SUB_CLASS_QIC157                0x03    // QIC-157, e.g. ATAPI Tape device
#define SUB_CLASS_UFI                   0x04    // UFI, e.g. Floppy
#define SUB_CLASS_SFF8070I              0x05    // SFF8070I, e.g. ATAPI Floppy
#define SUB_CLASS_SCSI                  0x06    // SCSI transparent command set

// Vendor specific mass storage related sub-class equates
#define SUB_CLASS_PL2307                0x80    // Prolific 2307 ,USB to IDE
#define SUB_CLASS_SL11R                 0x81    // ScanLogic SL11R-IDE
#define SUB_CLASS_THUMB_DRV             0x82    // ThumbDrive
#define SUB_CLASS_DFUSB01               0x83    // DataFab ATA Bridge
#define SUB_CLASS_DOK                   0x84    // Disk On Key
#define SUB_CLASS_VENDOR_SPECIFIC       0xFF    // Vendor Specific
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//      Values for InterfaceDescriptor.Protocol
//---------------------------------------------------------------------------
#define PROTOCOL_KEYBOARD       0x01    // Keyboard device protocol
#define PROTOCOL_MOUSE          0x02    // Mouse device protocol?

#define PROTOCOL_HUB_SINGLE_TT              0x00    // Hub single TT protocol
#define PROTOCOL_HUB_MULTIPLE_TTS           0x02    // Hub multiple TTs protocol

// Mass storage related protocol equates
#define PROTOCOL_CBI            0x00    // Mass Storage Control/Bulk/Interrupt
                                        // with command completion interrupt
#define PROTOCOL_CBI_NO_INT     0x01    // MASS STORAGE Control/Bulk/Interrupt
                                        // with NO command completion interrupt
#define PROTOCOL_BOT            0x50    // Mass Storage Bulk-Only Transport
#define PROTOCOL_VENDOR         0xFF    // Vendor specific mass protocol
//---------------------------------------------------------------------------

// Definitions for Interface Association Descriptor
#define DEV_BASE_CLASS_MISC         0xEF
#define DEV_SUB_CLASS_COMMON        0x02
#define DEV_PROTOCOL_IAD            0x01

// Definition of CCID class
#define BASE_CLASS_CCID_STORAGE     0x0B        // SMART device class
#define SUB_CLASS_CCID              0x00        // SubClass
#define PROTOCOL_CCID               0x00        // Interface Protocol
#define DESC_TYPE_SMART_CARD        0x21         // Smart Card Descriptor (Type 21h)

#pragma pack(push, 1)

typedef struct {
    UINT16      wRequestType;
    UINT16      wValue;
    UINT16      wIndex;
    UINT16      wDataLength;
} DEV_REQ;

#pragma pack(pop)

/**
    USB Host Controller Driver function list structure.

 Fields:   Name       Type    Description
      ------------------------------------------------------------
    bFlag UINT8 Driver Header Status
    pfnHCDStart UINT8 Driver Start
    pfnHCDStop UINT8 Driver Stop
    pfnHCDEnumeratePorts UINT8 Enumerate Root Ports
    pfnHCDDisableInterrupts UINT8 Disable Interrupts
    pfnHCDEnableInterrupts UINT8 Enable Interrupts
    pfnHCDProcessInterrupt UINT8 Process Interrupt
    pfnHCDGetRootHubStatus UINT8 Get Root Hub Ports Status
    pfnHCDDisableRootHub UINT8 Disable Root Hub 
    pfnHCDEnableRootHub UINT8 Enable Root Hub
    pfnHCDControlTransfer UINT16 Perform Control Transfer
    pfnHCDBulkTransfer UINT32 Perform Bulk Transfer
    pfnHCDInterruptTransfer UINT8 Perform Interrupt Transfer
    pfnHCDDeactivatePolling UINT8  Deactivate Polling
    pfnHCDActivatePolling UINT8 Activate Polling
    pfnHCDDisableKeyRepeat UINT8 Disable Key Repead
    pfnHCDEnableKeyRepeat UINT8 Enable Key Repeat
**/
//!!!!
//!!!! If you change this structure, please, check UN_HCD_HEADER also.
//!!!!
typedef struct {
    UINT8       bFlag;
    UINT8       (*pfnHCDStart) (HC_STRUC*);
    UINT8       (*pfnHCDStop) (HC_STRUC*);
    UINT8       (*pfnHCDEnumeratePorts) (HC_STRUC*);
    UINT8       (*pfnHCDDisableInterrupts) (HC_STRUC*);
    UINT8       (*pfnHCDEnableInterrupts) (HC_STRUC*);
    UINT8       (*pfnHCDProcessInterrupt) (HC_STRUC*);
    UINT32      (*pfnHCDGetRootHubStatus) (HC_STRUC*, UINT8, BOOLEAN);
    UINT8       (*pfnHCDDisableRootHub) (HC_STRUC*,UINT8);
    UINT8       (*pfnHCDEnableRootHub) (HC_STRUC*,UINT8);
    UINT16      (*pfnHCDControlTransfer) (HC_STRUC*,DEV_INFO*,UINT16,UINT16,UINT16,UINT8*,UINT16);
    UINT32      (*pfnHCDBulkTransfer) (HC_STRUC*,DEV_INFO*,UINT8,UINT8*,UINT32);
    UINT32      (*pfnHCDIsocTransfer) (HC_STRUC*,DEV_INFO*,UINT8,UINT8*,UINT32,UINT8*);
    UINT16      (*pfnHCDInterruptTransfer) (HC_STRUC*, DEV_INFO*, UINT8, UINT16, UINT8*, UINT16);
    UINT8       (*pfnHCDDeactivatePolling) (HC_STRUC*,DEV_INFO*);
    UINT8       (*pfnHCDActivatePolling) (HC_STRUC*,DEV_INFO*);
    UINT8       (*pfnHCDEnableEndpoints) (HC_STRUC*,DEV_INFO*,UINT8*);    
    UINT8       (*pfnHCDDisableKeyRepeat) (HC_STRUC*);
    UINT8       (*pfnHCDEnableKeyRepeat) (HC_STRUC*);
    UINT8       (*pfnHCDInitDeviceData) (HC_STRUC*, DEV_INFO*, UINT32, UINT8**);
    UINT8       (*pfnHCDDeinitDeviceData) (HC_STRUC*,DEV_INFO*);
	UINT8       (*pfnHCDResetRootHub) (HC_STRUC*,UINT8);
	UINT8		(*pfnHCDClearEndpointState) (HC_STRUC*,DEV_INFO*,UINT8);	//(EIP54283+)
	UINT8       (*pfnHCDGlobalSuspend) (HC_STRUC*);		//(EIP54018+)
	UINT8       (*pfnHCDSmiControl) (HC_STRUC*, BOOLEAN);
} HCD_HEADER;

typedef union {
    HCD_HEADER hcd_header;
    struct {
        UINT8       bFlag;
        VOID*       proc[(sizeof(HCD_HEADER)-1)/sizeof(VOID*)];
    } asArray;
} UN_HCD_HEADER;

// Equates related to host controller state
#define HC_STATE_RUNNING						BIT0
#define HC_STATE_SUSPEND						BIT1
#define HC_STATE_USED							BIT2
#define HC_STATE_INITIALIZED					BIT3
#define HC_STATE_EXTERNAL						BIT4
#define HC_STATE_OWNERSHIP_CHANGE_IN_PROGRESS	BIT5
#define HC_STATE_CONTROLLER_WITH_RMH        	BIT6
#define HC_STATE_IRQ                            BIT7

#pragma pack(push, 1)

typedef struct {
    UINT8              bDescLength;
    UINT8              bDescType;
    UINT8              bNumPorts;               // Number of downstream ports on hub
    UINT16             wHubFlags;               // See HUB_FLAG_xxx bit definitions below
    UINT8              bPowerOnDelay;           // Time to delay after turning on power to port (in 2ms units)
    UINT8              bHubControlAmps;         // Milliamps of current needed by hub controller
	UINT8              bHubHdrDecLat;
	UINT16             wHubDelay;
    UINT16             DeviceRemovable;         // Variable size array of bits (one for each port)
} HUB_DESC;

#pragma pack(pop)

#define DEV_INFO_VALID_STRUC            BIT0    // Structure validity
#define DEV_INFO_DEV_PRESENT            BIT1    // Device presence status
#define DEV_INFO_MASS_DEV_REGD          BIT2    // Mass device registered(have
                                                // drive number assigned)
#define DEV_INFO_MULTI_IF               BIT3    // Indicates that the device
                                                // is a part of multiple
                                                // interface device
#define DEV_INFO_HOTPLUG                BIT4    // Indicates that this device
                                                // is a hotplugged device
#define DEV_INFO_DEV_DUMMY              BIT5
#define DEV_INFO_DEV_BUS                BIT6    // Device info is locked by the bus
#define DEV_INFO_DEV_DISCONNECTING      BIT7	//(EIP60460+)
#define DEV_INFO_IN_QUEUE               BIT8
#define DEV_INFO_ALT_SETTING_IF         BIT9    // Indicates that the device
                                                // has alternate setting for the 
                                                // interface
#define DEV_INFO_DEV_UNSUPPORTED        BIT10


#define DEV_INFO_VALIDPRESENT  (DEV_INFO_VALID_STRUC | DEV_INFO_DEV_PRESENT)
// Call back routine type definition
typedef UINT8       (*CALLBACK_FUNC) (HC_STRUC*, DEV_INFO*, UINT8*, UINT8*, UINT16);


#define MAX_CALLBACK_FUNCTION           50

#if !USB_RT_DXE_DRIVER
#define     CALLBACK_FUNCTION_START        0
#else
#define     CALLBACK_FUNCTION_START MAX_CALLBACK_FUNCTION
#endif

#define MAX_USB_ERRORS_NUM              0x30    // 48 errors max

#pragma pack(push, 1)

/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call USBMass_HotPlugDeviceSupport
    (API #27h, SubFunc 09h)

 Fields:   Name       Type    Description
      ------------------------------------------------------------
      bDevAddr    BYTE    USB device address of the device
      bNumUSBFDD  BYTE    Number of USB FDD's installed
      bNumUSBCDROM    BYTE    Number of USB CDROM's installed
      bDeviceFlag BYTE    Flag indicating what hot plug devices to be added

**/

typedef struct _MASS_HOTPLUG {
    UINT8   bDevAddr;       // USB Device Address
    UINT8   bNumUSBFDD;     // Number of USB FDD's installed
    UINT8   bNumUSBCDROM;   // Number of USB CDROM's installed
    UINT8   bDeviceFlag;    // Flag indicating what hot plug devices to be added
} MASS_HOTPLUG;

#define USB_HOTPLUG_ENABLE_FDD      BIT0
#define USB_HOTPLUG_ENABLE_CDROM    BIT1
#define USB_HOTPLUG_HDD_ADDRESS     0x7D
#define USB_HOTPLUG_FDD_ADDRESS     0x7E
#define USB_HOTPLUG_CDROM_ADDRESS   0x7F

#pragma pack(pop)

/**
    This structure holds the information needed for the mass
    transaction (for CBI or BULK)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      pCmdBuffer  UINT16  Pointer to the mass transaction command buffer
      bCmdSize    UINT8   Size of the command buffer
      bXferDir    UINT8   Transfer direction (BIT7)
      fpBuffer    UINT32  Far pointer of the data buffer (IN/OUT)
      dwLength    UINT32  Length of the data buffer
      wPreSkip    UINT16  Number of bytes to skip before getting actual data
      wPostSkip   UINT16  Number of bytes to skip after getting actual data
      wMiscFlag   UINT16  Flag for special cases refer USBM_XACT_FLAG_XXX
**/

typedef struct {
    UINT8       *fpCmdBuffer;
    UINT8       bCmdSize;
    UINT8       bXferDir;
    UINT8       *fpBuffer;
    UINT32      dLength;
    UINT16      wPreSkip;
    UINT16      wPostSkip;
    UINT16      wMiscFlag;
} MASS_XACT_STRUC;

#define USBM_XACT_FLAG_32BIT_DATA_BUFFER        BIT0

typedef struct _QUEUE_T{
    VOID* volatile* data;
    int maxsize;
    volatile int head;
    volatile int tail;
} QUEUE_T;

typedef struct MOUSE_DATA{
    UINT8   ButtonStatus;
    INT32   MouseX;
    INT32   MouseY;
    INT32   MouseZ;
} MOUSE_DATA;


#define MAX_NOTIFICATIONS_COUNT 100

/**
    USB Global Data Area structure

**/

typedef struct {
    USB_VERSION     stUSBVersion;
    UINT32          dUSBStateFlag;
    UINT16          aErrorLogBuffer[MAX_USB_ERRORS_NUM];
    UINT8           bErrorLogIndex;
    HCD_HEADER      aHCDriverTable[MAX_HC_TYPES * 2];           // For 4 type of HC
    DEV_DRIVER      aDevDriverTable[MAX_DEVICE_TYPES * 2];      // For 5 types of devices
    DEV_DRIVER      aDelayedDrivers[MAX_DEVICE_TYPES * 2];      // For 5 types of devices
    DEV_INFO        aDevInfoTable[MAX_DEVICES];
    HC_STRUC        **HcTable;
    UINT8           HcTableCount;
    UINT8           NumOfHc;
    DEV_INFO        FddHotplugDev;
    DEV_INFO        HddHotplugDev;
    DEV_INFO        CdromHotplugDev;
    UINT8           bCallBackFunctionIndex;
    CALLBACK_FUNC   aCallBackFunctionTable[MAX_CALLBACK_FUNCTION * 2];
    UINT64          DeviceAddressMap;
    UINT8           bEnumFlag;
	UINT32			MemPages;
    UINT8           *fpMemBlockStart;
	UINT32			MemBlkStsBytes;
    UINT32          *aMemBlkSts;
//Hub related fields
    UINT32          dHubPortStatus;
    //UINT16          wHubPortStatus;
//KBD related fields
// Buffer to store keyboard shift key status bytes. This is correlated with
// scan code buffer to generate proper scan code sequence
	UINT8			aKBCShiftKeyStatusBufferStart[16];
	UINT8			aKBCDeviceIDBufferStart[16];	// Buffer to store keyboard device ID
// Buffer to store keyboard shift key status bytes. This is correlated with
// scan code buffer to generate proper scan code sequence
	UINT8			aKBCScanCodeBufferStart[16];

    UINT8           aKBCCharacterBufferStart[128];	//(EIP29345)
    UINT8           bCurrentUSBKeyCode;
    UINT8           bUSBKBShiftKeyStatus;
    UINT8           bNonUSBKBShiftKeyStatus;
    UINT8           KbShiftKeyStatusUnderOs;
    UINT8           bUSBKBC_ExtStatusFlag;
    UINT8           bUSBDeviceList;
    UINT8           bSet2ScanCode;          // Temporary storage for the scan code set 2 scan code
    UINT8           bLastUSBKeyCode;        // Last USB key code processed
    UINT8           bBreakCodeDeviceID;     // Device IDs for the keyboards generating break code
    UINT8           bCurrentDeviceID;       // Current USB keyboard device ID
    UINT16          wUSBKBC_StatusFlag;
    UINT16          wRepeatCounter;         // Typematic repeat counter
    UINT16          wRepeatRate;            // Typematic repeat rate
    UINT8           *fpKBCCharacterBufferHead;  // Keyboard character buffer head and tail pointers
    UINT8           *fpKBCCharacterBufferTail;
    UINT8           *fpKBCScanCodeBufferPtr;        // Keyboard scan code buffer pointer
    UINT8           bUSBKBC_MassStorage;
    UINT8           bKbdDataReady;
    UINT8           aKBInputBuffer[16];     // Keyboard expanded input buffer pointer (null-terminated)
    UINT8           bCCB;
    UINT8           RepeatKey;
    HC_STRUC        *fpKeyRepeatHCStruc;
    DEV_INFO        *fpKeyRepeatDevInfo;
    DEV_INFO        *aUSBKBDeviceTable[USB_DEV_HID_COUNT];
// Added by mouse driver
    MOUSE_DATA      MouseData;				
    UINT8           aMouseInputBuffer[12];
// Mouse input buffer head and tail pointers
    UINT8           *fpMouseInputBufferHeadPtr;
    UINT8           *fpMouseInputBufferTailPtr;
    UINT8           bMouseStatusFlag;
                    // Bit 7   : Mouse enabled bit (1/0)
                    // Bit 6   : Mouse data ready (1/0)
                    // BIT 5   : Mouse data from USB (1/0)
                    // BIT 4   : 4-byte mouse data (1/0)
                    // Bit 3-0 : Reserved
    UINT8           *fpUSBTempBuffer;
    UINT8           *fpUSBMassConsumeBuffer;
    UINT32          wInterruptStatus;
    URP_STRUC       *fpURP;     // Request Packet pointer
// BOTCommandTag used to maintain BOT command block number
    UINT32          dBOTCommandTag;
    //UINT16          wMassTempData;
    UINT8           bUSBStorageDeviceDelayCount;
    UINT16          wBulkDataXferDelay;
// Flag that allows mass storage device to handle special conditions. The
// bit pattern is defined by the USBMASS_MISC_FLAG_XXX equates in USB.EQU
    UINT16          wMassStorageMiscFlag;
    UINT8           bGeometryCommandStatus;
    UINT8           bModeSenseSectors;
    UINT8           bModeSenseHeads;
    UINT16          wModeSenseCylinders;
    UINT16          wModeSenseBlockSize;
    UINT32          dModeSenseMaxLBA;
    UINT8           bReadCapSectors;
    UINT8           bReadCapHeads;
    UINT16          wReadCapCylinders;
    UINT16          wReadCapBlockSize;
    UINT32          dReadCapMaxLBA;
    UINT8           bDiskMediaType;
    UINT16          wTimeOutValue;
    UINT8           bLastCommandStatus;
    UINT32          dLastCommandStatusExtended;
// Added by EHCI driver
//    EHCI_QH         *fpQHAsyncXfer;
    UINT8           bIgnoreConnectStsChng;
    UINT8           ProcessingPeriodicList;
    UINT8           bHandOverInProgress;
    HC_STRUC		*RootHubHcStruc;

// Tokens representation for the module binary
    UINT8           kbc_support;
    UINT8           fdd_hotplug_support;
    UINT8           hdd_hotplug_support;
    UINT8           cdrom_hotplug_support;
    UINT8           UsbMassResetDelay;  // 0/1/2/3 for 10/20/30/40 seconds
    UINT8           PowerGoodDeviceDelay;  // 0/1/5/6/../10 seconds
    UINT8			UsbXhciSupport;		// 0/1 for Disabled/Enabled
    UINT8           UsbXhciHandoff;     // 0/1 for Disabled/Enabled
    UINT8           UsbEhciHandoff;     // 0/1 for Disabled/Enabled
    UINT8           UsbOhciHandoff;     // 0/1 for Disabled/Enabled
    UINT8           UsbEmul6064;        // 0/1 for Disabled/Enabled
    UINT8           NumberOfFDDs;
    UINT8           NumberOfHDDs;
    UINT8           NumberOfCDROMs;
    UINT8           USBMassEmulationOptionTable[16];
    QUEUE_T         QueueCnnctDisc;
    QUEUE_T         ICCQueueCnnctDisc;    //QueueCnnctDisc will work of USB devices. Smart Card reader will work in that queue. But Smart Card(ICC) isn't a USB device. So create a new queue to handle it.
    DEV_INFO        *QueueData1[MAX_NOTIFICATIONS_COUNT];
    UINT8   		*gUsbSkipListTable;			//(EIP51653+)	
    UINT8			UsbHiSpeedSupport;
    USB_TIMING_POLICY   UsbTimingPolicy;
    USB_SUPPORT_SETUP   UsbSetupData;	//(EIP99882+)
    UINTN           PciExpressBaseAddress;
    BOOLEAN         EfiMakeCodeGenerated;
    BOOLEAN         LegacyMakeCodeGenerated;
    UINT8           IsKbcAccessBlocked;
    UINT8           MsOrgButtonStatus;
    UINT8           CcidSequence;
    DLIST           DevConfigInfoList; 
} USB_GLOBAL_DATA;

typedef struct{
    USB_DEV_CONFIG_INFO DevConfigInfo;
    DLINK   Link;
} USB_DEV_CONFIG_LINK;

// Note: If additional space is needed in USB data segment,
// MAX_BULK_DATA_SIZE can be changed to 200h without significant
// decrease in mass storage data transfer performance
// .. moved to SDL
//#define MAX_BULK_DATA_SIZE          0x0400          // Maximum amount of data to transfer

#define USB_HC_CLASS_CODE           0x0C03

#define MAX_NUM_HC_MODULES          0x03

// Equates to disable/enable USB port interrupt generation and
// 060/064h trapping
#define USB_DISABLE_INTERRUPT       0x000
#define USB_SAFE_DISABLE_INTERRUPT  0x001
#define USB_ENABLE_INTERRUPT        0x00F

#define TRAP_REQUEST_CLEAR          0x000
#define TRAP_REQUEST_DISABLE        0x0FF

//---------------------------------------------------------------------------
// Equates for Generic USB specific registers in the PCI config space
//---------------------------------------------------------------------------
#define USB_REG_COMMAND         0x004
#define USB_MEM_BASE_ADDRESS    0x010   // Offset 10-13h
#define USB_IO_BASE_ADDRESS     0x020
#define USB_IRQ_LEVEL           0x03C
#define USB_RELEASE_NUM         0x060
#define USB_HC_CLASS_CODE       0x0C03

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Equates common to all host controllers
//---------------------------------------------------------------------------
#define USB_PORT_STAT_DEV_CONNECTED                 BIT0
#define USB_PORT_STAT_DEV_LOWSPEED                  BIT1
#define USB_PORT_STAT_DEV_FULLSPEED                 BIT2
#define USB_PORT_STAT_DEV_HISPEED                   0//(BIT1 + BIT2)
#define USB_PORT_STAT_DEV_SUPERSPEED                BIT3
#define USB_PORT_STAT_DEV_SPEED_MASK                (BIT1 + BIT2 + BIT3 + BIT7)
#define USB_PORT_STAT_DEV_SPEED_MASK_SHIFT          0x1
#define USB_PORT_STAT_DEV_CONNECT_CHANGED           BIT4
#define USB_PORT_STAT_DEV_ENABLED					BIT5
#define USB_PORT_STAT_DEV_OWNER                     BIT6
#define USB_PORT_STAT_DEV_SUPERSPEED_PLUS           BIT7
#define USB_PORT_STAT_DEV_SUSPEND                   BIT8
#define USB_PORT_STAT_DEV_OVERCURRENT               BIT9
#define USB_PORT_STAT_DEV_RESET                     BIT10
#define USB_PORT_STAT_DEV_POWER                     BIT11
#define USB_PORT_STAT_DEV_ENABLE_CHANGED            BIT12
#define USB_PORT_STAT_DEV_SUSPEND_CHANGED           BIT13
#define USB_PORT_STAT_DEV_OVERCURRENT_CHANGED       BIT14
#define USB_PORT_STAT_DEV_RESET_CHANGED             BIT15

#define USB_DEV_SPEED_LOW	(USB_PORT_STAT_DEV_LOWSPEED >> USB_PORT_STAT_DEV_SPEED_MASK_SHIFT)
#define USB_DEV_SPEED_FULL	(USB_PORT_STAT_DEV_FULLSPEED >> USB_PORT_STAT_DEV_SPEED_MASK_SHIFT)
#define USB_DEV_SPEED_HIGH	(USB_PORT_STAT_DEV_HISPEED >> USB_PORT_STAT_DEV_SPEED_MASK_SHIFT)
#define USB_DEV_SPEED_SUPER	(USB_PORT_STAT_DEV_SUPERSPEED >> USB_PORT_STAT_DEV_SPEED_MASK_SHIFT)
#define USB_DEV_SPEED_SUPER_PLUS	(USB_PORT_STAT_DEV_SUPERSPEED_PLUS >> USB_PORT_STAT_DEV_SPEED_MASK_SHIFT)

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Equates related to USB equipment list
//---------------------------------------------------------------------------
                                        //(EIP84455+)>
#define USB_TYPE_KEYBOARD       BIT0
#define USB_TYPE_MOUSE          BIT1
#define USB_TYPE_POINT          BIT2    
#define USB_TYPE_HUB            BIT3
#define USB_TYPE_MASS_STORAGE   BIT4
                                        //<(EIP84455+)

// Bulk transfer error status (bLastCommandStatus)
#define USB_BULK_STALLED        BIT0
#define USB_BULK_TIMEDOUT       BIT1
#define USB_CONTROL_STALLED     BIT2

#define USB_TRNSFR_ERRBIT_SHIFT 0
#define USB_TRNSFR_BITSTUFF     BIT0
#define USB_TRNSFR_CRCERROR     BIT1
#define USB_TRNSFR_NAK          BIT2
#define USB_TRNSFR_BABBLE       BIT3
#define USB_TRSFR_BUFFER_ERROR  BIT4
#define USB_TRSFR_STALLED       BIT5
#define USB_TRNSF_ERRORS_MASK   (USB_TRNSFR_ERRBIT_SHIFT | \
        USB_TRNSFR_BITSTUFF | USB_TRNSFR_CRCERROR |\
        USB_TRNSFR_NAK|USB_TRNSFR_BABBLE|USB_TRSFR_BUFFER_ERROR )
#define USB_TRNSFR_TIMEOUT      BIT6

//----------------------------------------------------------------------------
// Equates regarding USB device info structure search parameter
//----------------------------------------------------------------------------
#define USB_SRCH_DEV_ADDR       0x10        //
#define USB_SRCH_DEV_TYPE       0x20        // Next device of a given type
#define USB_SRCH_DEV_NUM        0x30        // Number of devices of certain type
#define USB_SRCH_DEVBASECLASS_NUM 0x31      // Number of devices of certain base class
#define USB_SRCH_HC_STRUC       0x40        // Next device of a given HC
#define USB_SRCH_DEV_INDX       0x80        // Device of a given index
/*
// USB Initialization Flags - passed in when USB is initialized
//----------------------------------------------------------------------------
#define INIT_FLAG_MANUAL            0x07        //Bit 2-0: 000 = Auto enum
                    //         001 = KB on port 1
                    //         ...   ...
                    //         111 = KB on port 7
#define INIT_FLAG_ENUM_DISABLE      0x08        //    3: If set, do not enum the USB
#define INIT_FLAG_BEEP_DISABLE      0x10        //  4: If set, do not beep on new devices
#define INIT_FLAG_USB_STOP_EHCI_IN_OHCI_HANDOVER 0x20
*/
//----------------------------------------------------------------------------
//      Bit definitions for DeviceRequest.RequestType
//----------------------------------------------------------------------------
//               Bit 7:   Data direction
#define USB_REQ_TYPE_OUTPUT         0x00    //  0 = Host sending data to device
#define USB_REQ_TYPE_INPUT          0x80    //  1 = Device sending data to host

//               Bit 6-5: Type
#define USB_REQ_TYPE_STANDARD       0x00    //  00 = Standard USB request
#define USB_REQ_TYPE_CLASS          0x20    //  01 = Class specific
#define USB_REQ_TYPE_VENDOR         0x40    //  10 = Vendor specific

//               Bit 4-0: Recipient
#define USB_REQ_TYPE_DEVICE         0x00    //  00000 = Device
#define USB_REQ_TYPE_INTERFACE      0x01    //  00001 = Interface
#define USB_REQ_TYPE_ENDPOINT       0x02    //  00010 = Endpoint
#define USB_REQ_TYPE_OTHER          0x03    //  00011 = Other

//----------------------------------------------------------------------------
// Values for DeviceRequest.RequestType and DeviceRequest.RequestCode fields
// combined as a word value.
//---------------------------------------------------------------------------
#define USB_RQ_SET_ADDRESS          ((0x05 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE)
#define USB_RQ_GET_DESCRIPTOR       ((0x06 << 8) | USB_REQ_TYPE_INPUT  | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE)
#define USB_RQ_GET_CONFIGURATION    ((0x08 << 8) | USB_REQ_TYPE_INPUT  | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE)
#define USB_RQ_SET_CONFIGURATION    ((0x09 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE)
#define USB_RQ_SET_INTERFACE        ((0x0B << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE)
#define USB_RQ_SET_FEATURE          ((0x03 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE)

#define USB_FSEL_DEV_REMOTE_WAKEUP  01

#define USB_RQ_GET_CLASS_DESCRIPTOR ((0x06 << 8) | USB_REQ_TYPE_INPUT  | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_DEVICE)
#define HID_RQ_GET_DESCRIPTOR	    ((0x06 << 8) | USB_REQ_TYPE_INPUT  | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE)	//(EIP38434+)
#define HID_RQ_SET_PROTOCOL         ((0x0B << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE)
#define HID_RQ_SET_REPORT           ((0x09 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE)
#define HID_RQ_SET_IDLE             ((0x0A << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE)
#define HUB_RQ_GET_PORT_STATUS      ((0x00 << 8) | USB_REQ_TYPE_INPUT  | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_OTHER)
#define HUB_RQ_SET_PORT_FEATURE     ((0x03 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_OTHER)
#define HUB_RQ_CLEAR_PORT_FEATURE   ((0x01 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_OTHER)
#define HUB_RQ_SET_HUB_DEPTH	    ((0x0C << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_DEVICE)

//----------------------------------------------------------------------------
//      Bit definitions for HubDescriptor.HubFlags
//----------------------------------------------------------------------------
#define HUB_FLAG_PWR_MODE_BITS      0x03    //Bit 1-0: Power switching mode used by hub
#define HUB_FLAG_PWR_MODE_GANG      0x00    //    =00: All ports power on/off together
#define HUB_FLAG_PWR_MODE_INDIV     0x01    //    =01: Ports power on/off individually
#define HUB_FLAG_PWR_MODE_FIXED     0x02    //    =1x: Ports power is always on
#define HUB_FLAG_COMPOUND_DEV       0x04    //Bit 2: If set, hub is part of a compound device
#define HUB_FLAG_OVR_CUR_BITS       0x18    //Bit 4-3: Over-current protection mode used by hub
#define HUB_FLAG_OVR_CUR_GLOBAL     0x00    //    =00: Hub reports only global over-current status
#define HUB_FLAG_OVR_CUR_INDIV      0x08    //    =01: Hub reports individual over-current status
#define HUB_FLAG_OVR_CUR_NONE       0x10    //    =1x: Hub has no over-current protection
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//          Hub Class Feature Selectors
//----------------------------------------------------------------------------
#define	HUB_FEATURE_C_HUB_LOCAL_POWER		0
#define	HUB_FEATURE_C_HUB_OVER_CURRENT		1
#define	HUB_FEATURE_PORT_CONNECTION			0
#define HUB_FEATURE_PORT_ENABLE				1	//Hub port enable feature
#define	HUB_FEATURE_PORT_SUSPEND			2
#define HUB_FEATURE_PORT_OVER_CURRENT		3
#define HUB_FEATURE_PORT_RESET				4	//Hub port reset feature
#define HUB_FEATURE_PORT_LINK_STATE			5
#define HUB_FEATURE_PORT_POWER				8	//Hub port power feature
#define HUB_FEATURE_PORT_LOW_SPEED      	9	//Hub port low speed feature
#define HUB_FEATURE_C_PORT_CONNECTION		16	//Hub port connect change feature
#define HUB_FEATURE_C_PORT_ENABLE  			17	//Hub port enable change feature
#define HUB_FEATURE_C_PORT_SUSPEND			18
#define HUB_FEATURE_C_PORT_OVER_CURRENT 	19
#define HUB_FEATURE_C_PORT_RESET			20	//Hub port reset change feature
#define HUB_FEATURE_PORT_U1_TIMEOUT			23
#define HUB_FEATURE_PORT_U2_TIMEOUT			24
#define HUB_FEATURE_C_PORT_LINK_STATE		25
#define HUB_FEATURE_C_PORT_CONFIG_ERROR		26
#define HUB_FEATURE_PORT_REMOTE_WAKE_MASK	27
#define	HUB_FEATURE_BH_PORT_RESET			28
#define	HUB_FEATURE_C_BH_PORT_RESET			29
#define	HUB_FEATURE_FORCE_LINKPM_ACCEPT		30

typedef enum {
	HubLocalPower = 0,
	HubOverCurrent,
	PortConnection = 0,
	PortEnable,
	PortSuspend,
	PortOverCurrent,
	PortReset,
	PortLinkState,
	PortPower = 8,
	PortLowSpeed,
	PortConnectChange = 16,
	PortEnableChange,
	PortSuspendChange,
	PortOverCurrentChange,
	PortResetChange,
	PortTest,
	PortIndicator,
	PortU1Timeout,
	PortU2Timeout,
	PortLinkStateChange,
	PortConfigErrorChange,
	PortRemoteWakeMask,
	BhPortReset,
	BhPortResetChange,
	ForceLinkPmAccept
} HUB_FEATURE;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//          Hub Port Status Bit Definitions
//----------------------------------------------------------------------------
#define HUB_PORT_STATUS_PORT_CONNECTION		BIT0	//Bit 0: Set if device present
#define HUB_PORT_STATUS_PORT_ENABLED		BIT1 	//Bit 1: Set if port is enabled
#define HUB_PORT_STATUS_PORT_SUSPEND		BIT2 	//Bit 2: Set if device on port is suspended
#define HUB_PORT_STATUS_PORT_OVER_CURRENT	BIT3	//Bit 3: Set if port has been powered down due to over-current
#define HUB_PORT_STATUS_PORT_RESET			BIT4	//Bit 4: Set if reset sigaling is active
#define HUB_PORT_STATUS_PORT_POWER			BIT8 	//Bit 8: Set if port is enabled
#define HUB_PORT_STATUS_PORT_LOW_SPEED		BIT9 	//Bit 9: Set if a low speed device is attached
#define HUB_PORT_STATUS_PORT_HIGH_SPEED     BIT10 	//Bit 10: Set if a high speed device is attached
#define	HUB_PORT_STATUS_PORT_TEST			BIT11
#define	HUB_PORT_STATUS_PORT_INDICATOR		BIT12
#define HUB_PORT_STATUS_C_PORT_CONNECTION	(BIT0 << 16)	//Bit 0: Set if device has been attached/removed
#define HUB_PORT_STATUS_C_PORT_ENABLE		(BIT1 << 16)	//Bit 1: Set if port has been enabled/disabled by hardware in hub
#define HUB_PORT_STATUS_C_PORT_SUSPEND		(BIT2 << 16)	//Bit 2: Set if device has entered/left suspend state
#define HUB_PORT_STATUS_C_PORT_OVER_CURRENT	(BIT3 << 16)	//Bit 3: Set if over current indicator has changed
#define HUB_PORT_STATUS_C_PORT_RESET		(BIT4 << 16)	//Bit 4: Set when port reset sequence is complete

#define USB3_HUB_PORT_STATUS_PORT_CONNECTION		BIT0
#define USB3_HUB_PORT_STATUS_PORT_ENABLED			BIT1
#define USB3_HUB_PORT_STATUS_PORT_OVER_CURRENT		BIT3
#define USB3_HUB_PORT_STATUS_PORT_RESET				BIT4
#define USB3_HUB_PORT_STATUS_PORT_LINK_STATE		(BIT5 | BIT6 | BIT7 | BIT8)
#define USB3_HUB_PORT_STATUS_PORT_POWER				BIT9
#define USB3_HUB_PORT_STATUS_PORT_SPEED				(BIT10 | BIT11 | BIT12)
#define	USB3_HUB_PORT_STATUS_C_PORT_CONNECTION		(BIT0 << 16)
#define	USB3_HUB_PORT_STATUS_C_PORT_OVER_CURRENT	(BIT3 << 16)
#define	USB3_HUB_PORT_STATUS_C_PORT_RESET			(BIT4 << 16)
#define	USB3_HUB_PORT_STATUS_C_BH_PORT_RESET		(BIT5 << 16)
#define	USB3_HUB_PORT_STATUS_C_PORT_LINK_STATE		(BIT6 << 16)
#define	USB3_HUB_PORT_STATUS_C_PORT_CONFIG_ERROR	(BIT7 << 16)

#define	USB3_HUB_PORT_LINK_U0				0x00
#define	USB3_HUB_PORT_LINK_U1				0x01
#define	USB3_HUB_PORT_LINK_U2				0x02
#define	USB3_HUB_PORT_LINK_U3				0x03
#define	USB3_HUB_PORT_LINK_DISABLED			0x04
#define	USB3_HUB_PORT_LINK_RXDETECT			0x05
#define	USB3_HUB_PORT_LINK_INACTIVE			0x06
#define	USB3_HUB_PORT_LINK_POLLING			0x07
#define	USB3_HUB_PORT_LINK_RECOVERY			0x08
#define	USB3_HUB_PORT_LINK_HOT_RESET		0x09
#define	USB3_HUB_PORT_LINK_COMPLIANCE_MODE	0x0A
#define	USB3_HUB_PORT_LINK_LOOPBACK			0x0B

#pragma pack(push, 1)

typedef struct {
	struct {
		UINT16	Connected	:1;
		UINT16	Enabled		:1;
		UINT16	Suspend		:1;
		UINT16	OverCurrent	:1;
		UINT16	Reset		:1;
		UINT16	Reserved	:3;
		UINT16	Power		:1;
		UINT16	LowSpeed	:1;
		UINT16	HighSpeed	:1;
		UINT16	TestMode	:1;
		UINT16	Indicator	:1;
		UINT16	Reserved1	:3;
	} PortStatus;
	struct {
		UINT16	ConnectChange		:1;
		UINT16	EnableChange		:1;
		UINT16	SuspendChange		:1;
		UINT16	OverCurrentChange	:1;
		UINT16	ResetChange			:1;
		UINT16	Reserved			:11;
	} PortChange;
} HUB_PORT_STATUS;

typedef struct {
	struct {
		UINT16	Connected	:1;
		UINT16	Enabled		:1;
		UINT16	Reserved	:1;
		UINT16	OverCurrent	:1;
		UINT16	Reset		:1;
		UINT16	LinkState	:4;
		UINT16	Power		:1;
		UINT16	Speed		:3;
		UINT16	Reserved1	:3;
	} PortStatus;
	struct {
		UINT16	ConnectChange		:1;
		UINT16	Reserved			:2;
		UINT16	OverCurrentChange	:1;
		UINT16	ResetChange			:1;
		UINT16	BhResetChange		:1;
		UINT16	LinkStateChange		:1;
		UINT16	ConfigErrorChange	:1;
		UINT16	Reserved1			:8;
	} PortChange;
} USB3_HUB_PORT_STATUS;

#pragma pack(pop)

#define ENDPOINT_CLEAR_PORT_FEATURE (0x01 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_ENDPOINT

#define ADSC_OUT_REQUEST_TYPE       (0x00 << 8) | USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE
#define ADSC_IN_REQUEST_TYPE        (0x00 << 8) | USB_REQ_TYPE_INPUT  | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE

#define ENDPOINT_HALT               00

// Standard PCI configuration register offsets and relevant values
//------------------------------------------------------------------------------
#define PCI_REG_VENDID          0x00    //PCI vendor ID register
#define PCI_REG_DEVID           0x02    //PCI device ID register
#define PCI_REG_COMMAND         0x04    //PCI command register
#define CMD_DEV_DISABLE         0x00    //Disables device when written to cmd reg
#define CMD_IO_SPACE            0x01    //IO space enable bit
#define CMD_MEM_SPACE           0x02    //Memory space enable bit
#define CMD_BUS_MASTER          0x4     //Bus master enable bit
#define CMD_SPECIAL_CYCLE       0x08    //Special cycle enable bit
#define CMD_MEM_INVALIDATE      0x10    //Memory write & invalidate enable bit
#define CMD_PAL_SNOOP           0x20    //VGA palette snoop enable bit
#define CMD_PARITY              0x40    //Parity error enable bit
#define CMD_WAIT_CYCLE          0x80    //Wait cycle control bit
#define CMD_SERR                0x100   //SERR# enable bit
#define CMD_FAST_BTOB           0x200   //Fast back-to-back enable bit
#define PCI_REG_STATUS          0x06    //PCI status register
#define STAT_RESET_ALL          0x0FFFF //Resets all status bits
#define PCI_REG_REVID           0x08    //PCI revision ID register
#define PCI_REG_IF_TYPE         0x09    //PCI interface type register
#define PCI_REG_SUB_TYPE        0x0A    //PCI sub type register
#define PCI_REG_BASE_TYPE       0x0B    //PCI base type register
#define PCI_REG_LINE_SIZE       0x0C    //PCI cache line size register
#define PCI_REG_LATENCY         0x0D    //PCI latency timer register
#define PCI_REG_LATENCY         0x0D    //PCI latency timer register
#define PCI_REG_HEADER_TYPE     0x0E    //PCI header type register
#define MULTI_FUNC_BIT          0x80    //If set, device is multi function
#define PCI_CFG_HEADER_STD      0x00    //Standard PCI config space
#define PCI_CFG_HEADER_PPB      0x01    //PCI-PCI bridge config space
#define PCI_REG_BIST            0x0F    //PCI built in self test register
#define PCI_REG_FIRST_BASE_ADD  0x10    //PCI first base address register
#define PCI_REG_LAST_BASE_ADD   0x24    //PCI last base address register
#define PCI_BASE_ADD_PORT_BIT   0x01    //If set, base add reg is for I/O port
#define PCI_BASE_ADD_MEMTYPE    0x06    //Bits in lower word that are mem type
#define PCI_BASE_ADD_MT_32      0x00    //Memory must be located at 32 bit add
#define PCI_BASE_ADD_MT_20      0x02    //Memory must be located at 20 bit add
#define PCI_BASE_ADD_MT_64      0x04    //Memory must be located at 64 bit add
#define PCI_BASE_ADD_PREFETCH   0x08    //If set, memory is prefetchable
#define PCI_BASE_ADD_MEMMASK    0x0FFF0 //Bits in lower word that are memory mask
#define PCI_BASE_ADD_PORTMASK   0x0FFFC //Bits in lower word that are port mask
#define PCI_REG_ROM_BASE_ADD    0x30    //PCI expansion ROM base address register
#define PCI_BASE_ADD_ROMMASK    0x0FC00 //Bits in lower word that are ROM mask
#define PCI_REG_INT_LINE        0x3C    //PCI interrupt line register
#define PCI_REG_INT_PIN         0x3D    //PCI interrupt pin register
#define PCI_REG_MAX_GNT         0x3E    //PCI max grant register
#define PCI_REG_MAX_LAT         0x3F    //PCI max latency register

#define INTR_CNTRLR_MASTER_PORT     0x020
#define INTR_CNTRLR_SLAVE_PORT      0x0A0
#define READ_IN_SERVICE_REGISTER    0x00B

/**
    This structure is used to define a non-compliant USB device

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      wVID        WORD        Vendor ID of the device
      wDID        WORD        Device ID of the device
      bBaseClass  BYTE        Base class of the device
      bSubClass   BYTE        Sub class of the device
      bProtocol   BYTE        Protocol used by the device
      wFlags      INCMPT_FLAGS    Incompatibility flags

**/

typedef struct {
    UINT16  wVID;
    UINT16  wDID;
    UINT8   bBaseClass;
    UINT8   bSubClass;
    UINT8   bProtocol;
    UINT16  wFlags;
} USB_BADDEV_STRUC;


/**
 Type:        Equates

    This equates are used to describe the incompatible USB devices. 
    The bits defined are:
    BIT     Description
    =============================================================
    0       1, indicates this device does not support mode sense command
    1       1, indicates that this is a single LUN device (even though it is reporting as multiple LUN)
    2       1, indicates that this device should be disabled
    3       1, indicates this device does not support test unit ready command
    4       1, indicates this device responds with wrong BOT status value
    5       1, indicates that this device does not support start unit command
    6       1, indicates that this device does not support read format capacity command
    7       1, indicates that this hispeed device has to be in full speed always
    8       1, indicates that this hispeed device has to be in hispeed always
    9       1, indicated that device does not support set boot protocol command
    10      1, indicated that device does not support get max lun command
    11      1, indicated that device is a Rate Matching Hub(RMH)
    12      1, indicated that data of this hid device is overflow
    13      1, indicated that this mouse device only returns report protocol data
    14      1, indicated that this hid device only supports report protocol
    15      1, indicated that this hid device only supports boot protocol

    @note  The device is identified by the vendor id and device id
      associated with the flags above

**/

#define USB_INCMPT_MODE_SENSE_NOT_SUPPORTED         BIT0
#define USB_INCMPT_SINGLE_LUN_DEVICE                BIT1
#define USB_INCMPT_DISABLE_DEVICE                   BIT2
#define USB_INCMPT_TEST_UNIT_READY_FAILED           BIT3
#define USB_INCMPT_BOT_STATUS_FAILED                BIT4
#define USB_INCMPT_START_UNIT_NOT_SUPPORTED         BIT5
#define USB_INCMPT_FORMAT_CAPACITY_NOT_SUPPORTED    BIT6
#define USB_INCMPT_KEEP_FULLSPEED                   BIT7
#define USB_INCMPT_KEEP_HISPEED                     BIT8
#define USB_INCMPT_SET_BOOT_PROTOCOL_NOT_SUPPORTED  BIT9
#define USB_INCMPT_GETMAXLUN_NOT_SUPPORTED          BIT10
#define USB_INCMPT_RMH_DEVICE                       BIT11
#define USB_INCMPT_HID_DATA_OVERFLOW                BIT12
#define USB_INCMPT_BOOT_PROTOCOL_IGNORED            BIT13
#define USB_INCMPT_REPORT_PROTOCOL_ONLY             BIT14
#define USB_INCMPT_HID_BOOT_PROTOCOL_ONLY           BIT15

/**
    state information for USB_HC_PROTOCOL implementation

**/

typedef struct _HC_DXE_RECORD {
    EFI_USB_HC_PROTOCOL hcprotocol;
    EFI_USB2_HC_PROTOCOL hcprotocol2;
    HC_STRUC            *hc_data;
    EFI_PCI_IO_PROTOCOL *pciIo;
    DLIST               AsyncTransfers;
} HC_DXE_RECORD;

typedef struct {
    QUEUE_T         QCompleted;
    UINTN           DataLength; //size of each transfer
    EFI_ASYNC_USB_TRANSFER_CALLBACK  CallbackFunction;
    VOID*           Context;
    EFI_EVENT       Event;
    DLINK           Link;
    UINT8           Lock;
    UINT8           EndpointAddress;
    UINT8           Data[1];
} USBHC_INTERRUPT_DEVNINFO_T;

#define USB_ISOC_XFER_MEM_LENGTH 0xc00

typedef struct {
    EFI_ASYNC_USB_TRANSFER_CALLBACK  CallbackFunction;
    VOID*           Context;
    EFI_EVENT       Event;
    VOID            *Data;
    UINTN           DataLength;
    UINT8           Complete;
} USBHC_ASYNC_ISOC_CONTEXT;

int VALID_DEVINFO(DEV_INFO* pDevInfo);
VOID    USB_AbortConnectDev(DEV_INFO*);
EFI_STATUS  UsbHcStrucValidation(HC_STRUC*);
EFI_STATUS  UsbDevInfoValidation(DEV_INFO*);

#endif      // __USB_H

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
