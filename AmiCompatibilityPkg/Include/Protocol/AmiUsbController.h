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

/** @file AmiUsbController.h
    AMI USB Driver Protocol definition

**/

#ifndef _USB_PROT_H
#define _USB_PROT_H

#include <Efi.h>
#include <AmiDxeLib.h>

#define EFI_USB_PROTOCOL_GUID    \
  { 0x2ad8e2d2, 0x2e91, 0x4cd1, 0x95, 0xf5, 0xe7, 0x8f, 0xe5, 0xeb, 0xe3, 0x16 }

#define AMI_USB_SMM_PROTOCOL_GUID    \
  { 0x3ef7500e, 0xcf55, 0x474f, 0x8e, 0x7e, 0x0, 0x9e, 0xe, 0xac, 0xec, 0xd2 }

GUID_VARIABLE_DECLARATION(gEfiUsbProtocolGuid,EFI_USB_PROTOCOL_GUID);

GUID_VARIABLE_DECLARATION(gAmiUsbSmmProtocolGuid,AMI_USB_SMM_PROTOCOL_GUID);

#ifndef GUID_VARIABLE_DEFINITION
#include <Protocol/BlockIo.h>

typedef union {
    VOID  *fpUHCIDescPtrs;
    VOID  *fpOHCIDescPtrs;
    VOID  *fpEHCIDescPtrs;
} DESC_PTRS;

typedef struct {
	UINT8			Flag;			
	UINT8			ReportId;
	UINT16			UsagePage;
	UINT32			ReportCount;
	UINT8			ReportSize;
	UINT32			LogicalMin;
	UINT32			LogicalMax;
    UINT32          PhysicalMax;        //(EIP127014)
    UINT32          PhysicalMin;        //(EIP127014)
    UINT8           UnitExponent;       //(EIP127014)
	UINT16 			UsageCount;
	UINT16			*Usages;
	UINT16			MaxUsages;
	UINT16			UsageMin;
	UINT16			UsageMax;
} HID_REPORT_FIELD;

typedef struct {
	UINT8 				Flag;
	UINT16				AbsMaxX; 			
	UINT16				AbsMaxY;
    UINT16          	ReportDescLen;
    UINT8				FieldCount;
	HID_REPORT_FIELD	**Fields;
} HID_REPORT;

typedef struct {
    VOID    **Buffer;
    UINT8   Head;
    UINT8   Tail;
    UINT8   MaxKey;
    UINT32  KeySize;
} KEY_BUFFER;

typedef struct _ABS_MOUSE{
    UINT8   ReportID;
    UINT8   ButtonStauts;
    UINT16  Xcoordinate;
    UINT16  Ycoordinate;
    UINT16  Pressure;
    UINT16  Max_X;
    UINT16  Max_Y;
} ABS_MOUSE;

#pragma pack(push, 1)

typedef struct {
    UINT8       DescLength;
    UINT8       DescType;
    UINT16      BcdUsb;
    UINT8       BaseClass;
    UINT8       SubClass;
    UINT8       Protocol;
    UINT8       MaxPacketSize0;
    UINT16      VendorId;
    UINT16      DeviceId;
    UINT16      BcdDevice;
    UINT8       MfgStr;
    UINT8       ProductStr;
    UINT8       SerialStr;
    UINT8       NumConfigs;
} DEV_DESC;

#pragma pack(pop)

typedef struct _HC_STRUC HC_STRUC;
typedef struct _DEV_INFO DEV_INFO;
typedef struct _DEV_DRIVER DEV_DRIVER;
typedef struct _URP_STRUC URP_STRUC;

#define AMI_USB_PORT_STAT_LOW_SPEED         0x00
#define AMI_USB_PORT_STAT_FULL_SPEED        0x01
#define AMI_USB_PORT_STAT_HIGH_SPEED        0x02

/**
    USB Host Controller structure

 Fields:   Name       Type    Description
      ------------------------------------------------------------
      bHCNumber   UINT8   Host Controller number, 1-based
      bHCType     UINT8   Host Controller Type, U/O/E HCI
      fpFrameList UINT32* Host Controller Frame List Address
      BaseAddress UINTN   Host Controller Base Address, memory (EHCI,OHCI) or IO (UHCI)
      bNumPorts   UINT8   Number of root ports, 1-based
      wBusDevFuncNum UINT16   PCI location, bus (Bits8..15), device (Bits3..7), function(bits0..2)
      fpIRQInfo   IRQ_INFO IRQ information
      stDescPtrs  DESC_PTRS   Commonly used descriptor pointers, see definition of DESC_PTRS
      wAsyncListSize  UINT16  Async. list size
      bOpRegOffset UINT8  Operation region offset
      dMaxBulkDataSize    UINT32 Maximum Bulk Transfer data size
      dHCFlag     UINT32  Host Controller flag
      bExtCapPtr  UINT8   EHCI Extended Capabilities Pointer
      bRegOfs     UINT8   EHCI Capabilities PCI register Offset
      DebugPort   UINT8   Port number of EHCI debug port
      usbbus_data VOID*   USB Bus data specific to this Host Controller
      Controller  EFI_HANDLE  EFI Handle of this controller
      pHCdp   EFI_DEVICE_PATH_PROTOCOL* Pointer to this controller's device path
**/

struct _HC_STRUC {
    UINT8           bHCNumber;
    UINT8           bHCType;
    UINT32          *fpFrameList;
    UINTN           BaseAddress;
    UINTN           BaseAddressSize;
    UINT8           bNumPorts;
    UINT16          wBusDevFuncNum;
    UINT8           Irq;
    DESC_PTRS       stDescPtrs;
    UINT16          wAsyncListSize;
    UINT8           bOpRegOffset;
    UINT32          dMaxBulkDataSize;
	UINT32			dHCSParams;
	UINT32			dHCCParams;
    UINT32          dHCFlag;
    UINT8           bExtCapPtr; // EHCI Extended Capabilities Pointer
    UINT8           DebugPort;
    VOID*           usbbus_data;
    EFI_HANDLE      Controller;
    EFI_DEVICE_PATH_PROTOCOL    *pHCdp;
    UINT8           PwrCapPtr;	//(EIP54018+)
    VOID			*PciIo;
    UINT16          Vid;
    UINT16          Did;
    EFI_HANDLE      HwSmiHandle;
    UINT16          SplitPeriodicIndex;
#if !USB_RUNTIME_DRIVER_IN_SMM
    UINT32          *FrameListPhyAddr;
    VOID            *FrameListMapping;
	UINT32			MemPoolPages;
    UINT8           *MemPoolPhyAddr;
    VOID            *MemPoolMapping;  
	UINT8			*MemPool;
	UINT32			MemBlkStsBytes;
	UINT32			*MemBlkSts;
#endif
    VOID            *IsocTds;
};

// Isochronous transfer related fields of DEV_INFO
typedef struct _DEVINFO_ISOC_DETAILS {
    UINT8       Endpoint;
    UINT16      EpMaxPkt;
    UINT8       EpMult;
    UINT32      *XferDetails;
    UINT8       *AsyncStatus;
    UINTN       XferStart;
    UINTN       XferRing;
    UINTN       XferKey;
} DEVINFO_ISOC_DETAILS;

/**
    USB Device Information Structure

 Fields:   Name       Type    Description
      ------------------------------------------------------------
  bFlag       UINT8   Device Information flags
  bDeviceType UINT8   Device Type
  wVendorId   UINT16  Device VID
  wDeviceId   UINT16  Device DID
  bDeviceAddress  UINT8   Device USB Address
  bHCNumber   UINT8   Host Controller Number this device is attached to
  bHubDeviceNumber    UINT8   USB Hub Device Number this device is attached to
  bHubPortNumber  UINT8   USB Hub Port Number this device is attached to
  bEndpointNum    UINT8   Endpoint number
  bEndpointSpeed  UINT8   Endpoint speed
  bLUN        UINT8   Device Logical Unit number
  wEndp0MaxPacket UINT16  Endpoint0 max packet size, in Bytes
  bNumConfigs UINT8   Number of configurations
  bConfigNum  UINT8   Active configuration number (0-based)
  bInterfaceNum   UINT8   Active interface number
  bAltSettingNum  UINT8   Alternate setting number (0-based)
  bCallBackIndex  UINT8   Callback function index
  fpPollTDPtr     UINT8*  Polling TD pointer
  fpPollTEPtr     UINT8*  Polling ED pointer
  bHubNumPorts    UINT8   Hub # of ports (USB hubs only)
  bHubPowerOnDelay    UINT8 Hub power-on delay (USB hubs only)
  fpLUN0DevInfoPtr    DEV_INFO* Pointer to Lun0 device (for multiple-LUN devices)
  wDataIn/OutSync   UINT16   toggle tracking information
  bStorageType    UINT8   USB_MASS_DEV_ARMD, USB_MASS_DEV_HDD, etc.
  wIntMaxPkt  UINT16  Interrupt Max Packet size, in Bytes
  bPresent    UINT8   Device presence indicator
  bIntEndpoint    UINT8   Interrupt endpoint number
  bBulkInEndpoint UINT8   Bulk-In endpoint number
  bBulkOutEndpoint    UINT8   Bulk-Out endpoint number
  bProtocol   UINT8   Protocol
  wEmulationOption    UINT16  USB Mass Storage Drive Emulation Option, from Setup
  bHiddenSectors  UINT8   Number of hidden sectors, for USB mass storage devices only
  bSubClass   UINT8   Device sub-class
  wBlockSize  UINT16  USB Mass Storage Device block size, in Bytes
  dMaxLba     UINT32  USB Mass Storage Device Maximum LBA number
  bHeads      UINT8   USB Mass Storage Device # of heads
  bSectors    UINT8   USB Mass Storage Device # of sectors
  wCylinders  UINT16  USB Mass Storage Device # of cylinders
  bNonLBAHeads UINT8   USB Mass Storage Device # of heads reported in Non-LBA (CHS) functions
  bNonLBASectors  UINT8   USB Mass Storage Device # of sectors reported in Non-LBA (CHS) functions
  wNonLBACylinders    UINT16  USB Mass Storage Device # of cylinders reported in Non-LBA (CHS) functions
  bEmuType    UINT8   USB Mass Storage Device emulation type
  bPhyDevType UINT8   USB Mass Storage Device physical type
  bMediaType  UINT8   USB Mass Storage Device media type
  bDriveNumber    UINT8   USB Mass Storage Device INT13 drive number
  wBulkInMaxPkt   UINT16  USB Mass Storage Device Bulk-In max packet size, in Bytes
  wBulkOutMaxPkt  UINT16  USB Mass Storage Device Bulk-Out max packet size, in Bytes
  wIncompatFlags  UINT16  USB Mass Storage Device Incompatibility flags
  MassDev     VOID*   USB Mass Storage Device EFI handle
  fpDeviceDriver  DEV_DRIVER*   Device driver pointer
  bLastStatus UINT8   Last transaction status
  pExtra      UINT8*  Pointer to extra device specific data
  UINT32      UINT8   USB Mass Storage Device # of heads
  Handle      UINT32[2]   USB Device Handle
**/
struct _DEV_INFO {
    UINT32      Flag;          //00
    UINT8       bDeviceType;    //01
    UINT16      wVendorId;      //02
    UINT16      wDeviceId;      //04
    UINT8       bDeviceAddress; //06

    UINT8       bHCNumber;      //07
    UINT8       bHubDeviceNumber;   //08
    UINT8       bHubPortNumber; //09
//    UINT8       bEndpointNum;   //0A //(EIP70933-)
    UINT8       bEndpointSpeed; //0B
    UINT8       bLUN;           //0C
    UINT16      wEndp0MaxPacket;//0D
//    UINT8       bNumConfigs;    //0F
    UINT8       bConfigNum;     //10
    UINT8       bInterfaceNum;  //11
    UINT8       bAltSettingNum; //12

    UINT8       bCallBackIndex; //13
    UINT8       *fpPollTDPtr;   //14
    UINT8       *fpPollEDPtr;   //18

    UINT8       bHubNumPorts;   //1C
    UINT8       bHubPowerOnDelay;//1D

    struct _DEV_INFO *fpLUN0DevInfoPtr; //1E
//    UINT8       bDataSync;      //22

    UINT16      wDataInSync;    // 22
    UINT16      wDataOutSync;   // 24
    UINT8       bStorageType;   // 26, USB_MASS_DEV_ARMD, USB_MASS_DEV_HDD, etc.
    UINT16      IntInMaxPkt;     //27
    UINT8       IntInEndpoint;   //2A
    UINT16      IntOutMaxPkt;
    UINT8       IntOutEndpoint;
    UINT8       bBulkInEndpoint;
    UINT8       bBulkOutEndpoint;

    UINT8       bBaseClass;     // BASE_CLASS_HID, BASE_CLASS_MASS_STORAGE or BASE_CLASS_HUB
    UINT8       bSubClass;
    UINT8       bProtocol;          //
    UINT16      wEmulationOption;   //
    UINT8       bHiddenSectors;     //

    UINT16      wBlockSize;         //
    UINT64      MaxLba;            //
    UINT16      Heads;             //
    UINT8       bSectors;           //
    UINT16      wCylinders;         //
    UINT16      NonLBAHeads;       //
    UINT8       bNonLBASectors;     //
    UINT16      wNonLBACylinders;   //
    UINT8       bEmuType;           //
    UINT8       bPhyDevType;        //
    UINT8       bMediaType;         //
    UINT8       bDriveNumber;       //
    UINT16      wBulkInMaxPkt;      //
    UINT16      wBulkOutMaxPkt;     //
    UINT16      wIncompatFlags;     //
    VOID        *MassDev;           //
    DEV_DRIVER  *fpDeviceDriver;    //
    UINT8       bLastStatus;        //
    UINT8       *pExtra;            //
    UINT32      Handle[2];
    UINT8       DevNameString[64];
    VOID        *DevMiscInfo;
    UINT8        HubDepth;
    UINT8         *fpPollDataBuffer;        //Polling Data Buffer    //(EIP54782+)
    VOID		*pCCIDDescriptor; // Ptr to CCID descriptor
    UINT32      *DataRates;           // List of DataRates supported by CCID  
    UINT32      *ClockFrequencies;    // List of Frequencies suported by CCID
    DLIST        ICCDeviceList;        // Linked list of ICC devices. :Linked to "ICCDeviceLink"
	HID_REPORT	HidReport;
    UINT64      HidDevType;
	UINT8       bPollInterval;          //(EIP84455+)
	UINT16		PollingLength;
	UINT16      HubPortConnectMap;
    UINT8       BpbMediaDesc;
    KEY_BUFFER  KeyCodeBuffer;
    KEY_BUFFER  UsbKeyBuffer;
    ABS_MOUSE   AbsMouseData;
	DEVINFO_ISOC_DETAILS    IsocDetails;
    DEV_DESC    DevDesc;
};

/**
    USB Device Driver Structure

 Fields:   Name       Type    Description
      ------------------------------------------------------------
  bDevType    UINT8   Device Type
  bBaseClass  UINT8   Device Base Type
  bSubClass   UINT8   Device Subclass
  bProtocol   UINT8   Device Protocol
  pfnDeviceInit   VOID    Device Initialization Function
  pfnCheckDeviceType  UINT8   Check Device Type Function
  pfnConfigureDevice  DEV_INFO*   Configure Device Function
  pfnDisconnectDevice UINT8   Disconnect Device Function
**/

struct _DEV_DRIVER {
    UINT8           bDevType;
    UINT8           bBaseClass;
    UINT8           bSubClass;
    UINT8           bProtocol;
    VOID            (*pfnDeviceInit)(VOID);
    UINT8           (*pfnCheckDeviceType)(DEV_INFO*, UINT8, UINT8, UINT8);
    DEV_INFO*       (*pfnConfigureDevice)(HC_STRUC*, DEV_INFO*, UINT8*, UINT16, UINT16);
    UINT8           (*pfnDisconnectDevice)(DEV_INFO*);
    VOID            (*pfnDriverRequest)(DEV_INFO*, URP_STRUC*);
};

#pragma pack(push, 1)

/**
    This is a URP (USB Request Packet) structure for the BIOS
    API call CheckPresence (API #0)

**/

typedef struct {
    UINT16      wBiosRev;       // USB BIOS Revision
    UINT8       bBiosActive;    // USB BIOS active/inactive
    UINT8       bNumBootDev;    // # of USB boot device
    UINT16      wUsbDataArea;   // USB Data area
    UINT8       bNumKeyboards;  // Number of USB keyboards present
    UINT8       bNumMice;       // Number of USB mice present
    UINT8       bNumPoint;      // Number of USB point present				//<(EIP38434+)
    UINT8       bNumHubs;       // Number of USB hubs present
    UINT8       bNumStorage;    // Number of USB storage devices present
///////// DO NOT ADD ANY FIELD HERE. IF IT IS NECESSARY PLEASE UPDATE THE CODE
///////// IN THE FUNCTION USBWrap_GetDeviceCount in the file USBWRAP.ASM
    UINT8       bNumHarddisk;   // Number of hard disk emulated USB devices
    UINT8       bNumCDROM;      // Number of CDROM emulated USB devices
    UINT8       bNumFloppy;     // Number of floppy emulated USB devices
} CK_PRESENCE;


/**
    This is a URP (USB Request Packet) structure for the BIOS
    API call StartHC and MoveDataArea (API #20 & #24)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      wDataAreaFlag   UINT16  Indicates which data area to use

**/

typedef struct {
    UINT16  wDataAreaFlag;      // Data area to use
} START_HC;


/**
    This is a URP (USB Request Packet) structure for the BIOS
    API call GetDeviceInfo (API #25)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevNumber  UINT8   Device # whose info is requested
      bHCNumber   UINT8   HC # to which this device is connected (0 if no such device found)
      bDevType    UINT8   Device type (0 if no such device found)

**/

typedef struct {
    UINT8   bDevNumber;
    UINT8   bHCNumber;
    UINT8   bDevType;
} GET_DEV_INFO;


/**
    This is a URP (USB Request Packet) structure for the BIOS
    API call CheckDevicePresence (API #26)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevType    UINT8           Type of device to look for
      fpHCStruc   FPHC_STRUC      Pointer to HC being checked for device connection
      bNumber     UINT8           Number of devices connected

**/

typedef struct {
    UINT8       bDevType;
    HC_STRUC    *fpHCStruc;
    UINT8       bNumber;
} CHK_DEV_PRSNC;


typedef struct {
    UINT8   ScrLock:    1;
    UINT8   NumLock:    1;
    UINT8   CapsLock:   1;
    UINT8   Resrvd:     5;
} LED_MAP;

/**
    This is a URP (USB Request Packet) structure for the BIOS
    API call LightenKeyboardLeds(API #2B)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      LedMapPtr    	UINT32		32-bit Pointer to LED_MAP structure
		DevInfoPtr    	UINT32   	32-bit Pointer to DEV_INFO structure

**/

typedef struct {
	UINT32	LedMapPtr;
	UINT32  DevInfoPtr;
} KB_LEDS_DATA;

/**
    This is a URP (USB Request Packet) structure for the BIOS
    API call LightenKeyboardLeds(API #2B)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      LedMapPtr    	UINT32		32-bit Pointer to LED_MAP structure
		DevInfoPtr    	UINT32   	32-bit Pointer to DEV_INFO structure

**/

typedef struct {
	UINTN	LedMapPtr;
	UINTN   DevInfoPtr;
} EFI_KB_LEDS_DATA;


/**
    This is a URP (USB Request Packet) structure for the BIOS
    API call SecurityInterface (API #2Ah)

 Fields:        Name           Type            Description
               ------------------------------------------------------------
               fpBuffer        FAR     Buffer pointer to read/write data
               dLength         UINT32   Length of the buffer
               dWaitTime       UINT32   Wait time for the transaction in msec

**/

typedef struct {
        UINT32  fpBuffer;
        UINT32  dLength;
        UINT32  dWaitTime;
} SECURITY_IF;


/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassGetDeviceInfo (API #27h, SubFunc 00h)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command
      bDevType    UINT8   Device type byte (HDD, CD, Removable)
      bEmuType    UINT8   Emulation type used
      fpDevId     UINT32  Far pointer to the device ID
**/

typedef struct {
    UINT8       bDevAddr;       // (Return value)
    UINT32      dSenseData;     // USB Sense data
    UINT8       bDevType;       // Device type
    UINT8       bEmuType;       // Emulation type
//  UINT8       bPhyDevType;    // Physical device type
    UINT32      fpDevId;        // Far ptr to the device id
// DO NOT ADD OR DELETE ANY FIELD ABOVE - This should match the MASS_INQUIRY
// structure for proper working
    UINT8       bTotalMassDev;  // TotalNumber of devices
    UINT8       bReserved;
    UINT16      wPciInfo;       // PCI Bus/Dev/Func number of HC the device is connected to
    UINT32      Handle[2];      // Device handle
} MASS_GET_DEV_INFO;

/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MASS_GET_DEV_STATUS (API #27h, SubFunc XXh)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      bDeviceStatus   UINT8   Connection status of the Mass device
**/

typedef struct {
    UINT8       bDevAddr;
    UINT8       bDeviceStatus;
} MASS_GET_DEV_STATUS;


/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassGetDeviceGeometry (API #27h,
    SubFunc 01h)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command
      bNumHeads   UINT8   Number of heads
      wNumCylinders   UINT16  Number of cylinders
      bNumSectors UINT8   Number of sectors
      bLBANumHeads    UINT8   Number of heads (for INT13h function 48h)
      wLBANumCyls UINT16  Number of cylinders (for INT13h function 48h)
      bLBANumSectors  UINT8   Number of sectors (for INT13h function 48h)
      wUINT8sPerSector    UINT16  Number of bytes per sector
      bMediaType  UINT8   Media type
      dLastLBA    UINT32  Last LBA address
**/

typedef struct {
    UINT8           bDevAddr;   // (Return value)
    UINT32          dSenseData; // USB Sense data
    UINT16          NumHeads;
    UINT16          wNumCylinders;
    UINT8           bNumSectors;
    UINT16          LBANumHeads;
    UINT16          wLBANumCyls;
    UINT8           bLBANumSectors;
    UINT16          wBytesPerSector;
    UINT8           bMediaType;
    UINT64          LastLBA;
    UINT8	        bInt13FuncNum;	//(EIP13457+)
    UINT8           BpbMediaDesc;
} MASS_GET_DEV_GEO;


/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassResetDevice (API #27h, SubFunc 02h)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command

**/

typedef struct {
    UINT8       bDevAddr;       // USB Device Address
    UINT32      dSenseData;     // USB Sense data
} MASS_RESET;


/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call USBMass_AssignDriveNumber
    (API #27h, SubFunc 0Eh)

 Fields:   Name       Type    Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      bLogDevNum  UINT8   Logical Drive Number to assign to the device
      bHeads      UINT8   Number of heads
      bSectors    UINT8   Number of sectors/track
      wCylinders  UINT16  Number of cylinders
      wBlockSize  UINT16  Sector size in bytes
      bLUN        UINT8   Maximum LUNs in the system
      bSpeed      UINT8   <>0 if the device is hi-speed device

**/

typedef struct {
    UINT8       bDevAddr;   // USB Device Address
    UINT8       bLogDevNum; // Logical Drive Number to assign to the device
    UINT8       bHeads;
    UINT8       bSectors;
    UINT16      wCylinders;
    UINT16      wBlockSize;
    UINT8       bLUN;
    UINT8       bSpeed;
} MASS_ASSIGN_DRIVE_NUM;


/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassReadDevice (API #27h, SubFunc 03h)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command
      dStartLBA   UINT32  Starting LBA address
      wNumBlks    UINT16  Number of blocks to read
      wPreSkipSize    UINT16  Number of bytes to skip before
      wPostSkipSize   UINT16  Number of bytes to skip after
      fpBufferPtr UINT32  Far buffer pointer

**/

typedef struct {
    UINT8       DevAddr;        // USB Device Address
    UINT32      SenseData;      // USB Sense data
    UINT64      StartLba;       // Starting LBA address
    UINT16      NumBlks;        // Number of blocks to read
    UINT16      PreSkipSize;    // Number of bytes to skip before
    UINT16      PostSkipSize;   // Number of bytes to skip after
    UINT32      BufferPtr;      // Far buffer pointer
} MASS_READ;

/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassWriteDevice (API #27h, SubFunc 04h)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command
      dStartLBA   UINT32  Starting LBA address
      wNumBlks    UINT16  Number of blocks to write
      wPreSkipSize    UINT16  Number of bytes to skip before
      wPostSkipSize   UINT16  Number of bytes to skip after
      fpBufferPtr UINT32  Far buffer pointer

**/

typedef struct {
    UINT8       DevAddr;        // USB Device Address
    UINT32      SenseData;      // USB Sense data
    UINT64      StartLba;       // Starting LBA address
    UINT16      NumBlks;        // Number of blocks to write
    UINT16      PreSkipSize;    // Number of bytes to skip before
    UINT16      PostSkipSize;   // Number of bytes to skip after
    UINT32      BufferPtr;      // Far buffer pointer
} MASS_WRITE;

/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassVerifyDevice (API #27h, SubFunc 05h)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command
      dStartLBA   UINT32  Starting LBA address
      wNumBlks    UINT16  Number of blocks to verify
      wPreSkipSize    UINT16  Number of bytes to skip before
      wPostSkipSize   UINT16  Number of bytes to skip after
      fpBufferPtr UINT32  Far buffer pointer

**/

typedef struct {
    UINT8       DevAddr;        // USB Device Address
    UINT32      SenseData;      // USB Sense data
    UINT64      StartLba;       // Starting LBA address
    UINT16      NumBlks;        // Number of blocks to verify
    UINT16      PreSkipSize;    // Number of bytes to skip before
    UINT16      PostSkipSize;   // Number of bytes to skip after
    UINT32      BufferPtr;      // Far buffer pointer
} MASS_VERIFY;

/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassReadDevice (API #27h, SubFunc 0Ch)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      DevAddr         UINT8   USB device address of the device
      SenseData       UINT32  Sense data of the last command
      StartLBA        UINT64  Starting LBA address
      NumBlks         UINT16  Number of blocks to write
      PreSkipSize     UINT16  Number of bytes to skip before
      PostSkipSize    UINT16  Number of bytes to skip after
      BufferPtr       UINTN   Far buffer pointer

**/

typedef struct {
    UINT8       DevAddr;        // USB Device Address
    UINT32      SenseData;      // USB Sense data
    UINT64      StartLba;       // Starting LBA address
    UINT16      NumBlks;        // Number of blocks to read
    UINT16      PreSkipSize;    // Number of bytes to skip before
    UINT16      PostSkipSize;   // Number of bytes to skip after
    UINTN       BufferPtr;      // Far buffer pointer
} EFI_MASS_READ;

/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassWriteDevice (API #27h, SubFunc 0Dh)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      DevAddr         UINT8   USB device address of the device
      SenseData       UINT32  Sense data of the last command
      StartLBA        UINT64  Starting LBA address
      NumBlks         UINT16  Number of blocks to write
      PreSkipSize     UINT16  Number of bytes to skip before
      PostSkipSize    UINT16  Number of bytes to skip after
      BufferPtr       UINTN   Far buffer pointer

**/

typedef struct {
    UINT8       DevAddr;        // USB Device Address
    UINT32      SenseData;      // USB Sense data
    UINT64      StartLba;       // Starting LBA address
    UINT16      NumBlks;        // Number of blocks to write
    UINT16      PreSkipSize;    // Number of bytes to skip before
    UINT16      PostSkipSize;   // Number of bytes to skip after
    UINTN       BufferPtr;      // Far buffer pointer
} EFI_MASS_WRITE;

/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassVerifyDevice (API #27h, SubFunc 0Eh)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      DevAddr         UINT8   USB device address of the device
      SenseData       UINT32  Sense data of the last command
      StartLBA        UINT64  Starting LBA address
      NumBlks         UINT16  Number of blocks to verify
      PreSkipSize     UINT16  Number of bytes to skip before
      PostSkipSize    UINT16  Number of bytes to skip after
      BufferPtr       UINTN   Far buffer pointer

**/

typedef struct {
    UINT8       DevAddr;        // USB Device Address
    UINT32      SenseData;      // USB Sense data
    UINT64      StartLba;       // Starting LBA address
    UINT16      NumBlks;        // Number of blocks to verify
    UINT16      PreSkipSize;    // Number of bytes to skip before
    UINT16      PostSkipSize;   // Number of bytes to skip after
    UINTN       BufferPtr;      // Far buffer pointer
} EFI_MASS_VERIFY;

/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassFormatDevice (API #27h, SubFunc 06h)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command
      bHeadNumber UINT8   Head number to format
      bTrackNumber    UINT8   Track number to format

**/

typedef struct {
    UINT8       bDevAddr;       // USB Device Address
    UINT32      dSenseData;     // USB Sense data
    UINT8       bHeadNumber;    // Head number to format
    UINT8       bTrackNumber;   // Track number to format
} MASS_FORMAT;

/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassRequestSense (API #27h, SubFunc 07h)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command

**/

typedef struct {
    UINT8       bDevAddr;       // USB Device Address
    UINT32      dSenseData;     // USB Sense data
} MASS_REQ_SENSE;


/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassTestUnitReady (API #27h, SubFunc 08h)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command

**/

typedef struct {
    UINT8       bDevAddr;       // USB Device Address
    UINT32      dSenseData;     // USB Sense data
} MASS_TEST_UNIT_RDY;


/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassStartStopUnit (API #27h, SubFunc 09h)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command
      bCommand    UINT8   0 - Stop, 1 - Start

**/

typedef struct {
    UINT8       bDevAddr;       // USB Device Address
    UINT32      dSenseData;     // USB Sense data
    UINT8       bCommand;       // 0 - Stop, 1 - Start
} MASS_START_STOP_UNIT;


/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassReadCapacity (API #27h, SubFunc 0Ah)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command
      dMaxLBA     UINT32  Maximum LBA address
      dBlockSize  UINT32  Block size

**/

typedef struct {
    UINT8       bDevAddr;       // USB Device Address
    UINT32      dSenseData;     // USB Sense data
    UINT32      dMaxLBA;        // Max LBA address
    UINT32      dBlockSize;     // Block size
} MASS_READ_CAPACITY;


/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassModeSense (API #27h, SubFunc 0Bh)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command
      bNumHeads   UINT8   Number of heads
      wNumCylinders   UINT16  Number of cylinders
      bNumSectors UINT8   Number of sectors
      wBytesPerSector UINT16  Number of bytes per sector
      bMediaType  UINT8   Media type

**/

typedef struct {
    UINT8       bDevAddr;       // USB Device Address
    UINT32      dSenseData;     // USB Sense data
    UINT8       bNumHeads;      // Number of heads
    UINT16      wNumCylinders;  // Number of cylinders
    UINT8       bNumSectors;    // Number of sectors
    UINT16      wBytesPerSector;// Number of bytes per sector
    UINT8       bMediaType;     // Media type
} MASS_MODE_SENSE;

/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassInquiry (API #27h, SubFunc 0Ch)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    UINT8   USB device address of the device
      dSenseData  UINT32  Sense data of the last command
      bDevType    UINT8   Device type byte (HDD, CD, Removable)
      bEmuType    BYTE    Emulation type used
      fpDevId     UINT32  Far pointer to the device ID

**/

typedef struct {
    UINT8       bDevAddr;       // USB Device Address
    UINT32      dSenseData;     // USB Sense data
    UINT8       bDevType;       // Device type
    UINT8       bEmuType;       // Emulation type
    UINT32      fpDevId;        // Far ptr to the device id
// DO NOT ADD OR DELETE ANY FIELD ABOVE - This should match the
// MASS_GET_DEV_INFO structure for proper working
} MASS_INQUIRY;

typedef struct {
    DEV_INFO        *fpDevInfo;
    MASS_INQUIRY    *fpInqData;
} MASS_GET_DEV_PARMS;

typedef struct {
    DEV_INFO*   fpDevInfo;
} MASS_CHK_DEV_READY;

/**
    This is a Mass URP (Mass USB Request Packet) structure for
    the BIOS API call MassCmdPassThru command (API #27h,
    SubFunc 0Dh)

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bDevAddr    BYTE    USB device address of the device
      dSenseData  UINT32  Sense data of the last command
      fpCmdBuffer UINT32  Far pointer to the command buffer
      wCmdLength  UINT16  Command length
      fpDataBuffer    UINT32  Far pointer for data buffer
      wDataLength UINT16  Data length
      bXferDir    BYTE    Data transfer direction

**/

typedef struct {
    UINT8       bDevAddr;
    UINT32      dSenseData;
    UINT32      fpCmdBuffer;
    UINT16      wCmdLength;
    UINT32      fpDataBuffer;
    UINT16      wDataLength;
    UINT8       bXferDir;
} MASS_CMD_PASS_THRU;

/**
    N/A

**/

typedef struct _HCPROC_PARAM{
    VOID*       paramBuffer; //parameters as they should apear in stack of
                            // of the corresponding function invocation
    unsigned    bHCType;
    unsigned    paramSize;
    UINTN       retVal;
} HCPROC_PARAM;

/**
    This is a Core Procedure URP  structure for
    the BIOS API call core command (API #2eh )
**/

typedef struct _COREPROC_PARAM{
    VOID*       paramBuffer; //parameters as they should apear in stack of
                            // of the corresponding function invocation
    unsigned    paramSize;
    UINTN       retVal;
} COREPROC_PARAM, * FPCOREPROC_PARAM;

/**
    This is a URP structure for the BIOS API(API #32h)
**/

typedef struct {
    UINT16	Vid;		// Vendor Id
	UINT16	Did;		// Device Id
    UINT8   DevAddr;	// USB Device Address
} GET_DEV_ADDR;

/**
    This is a URP structure for the BIOS API(API #36)
**/

typedef struct {
	BOOLEAN 	Start;
	HC_STRUC    *HcStruc;
} HC_START_STOP;

/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _CCID_GETSMARTCLASSDESCRIPTOR{
    OUT UINTN            fpResponseBuffer;
    IN  UINT8            Slot;
    OUT UINTN            fpDevInfo;            
} CCID_GETSMARTCLASSDESCRIPTOR, * FPCCID_GETSMARTCLASSDESCRIPTOR;


/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _CCID_ATR{
    IN UINT8            Slot;
    IN OUT UINTN        ATRData;
    OUT UINTN           fpDevInfo;
} CCID_ATR, * FPCCID_ATR;


/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _CCID_POWERUP_SLOT{
    IN UINT8            Slot;
    OUT UINT8            bStatus;
    OUT UINT8            bError;
    IN OUT UINTN        ATRData;
    OUT UINTN            fpDevInfo;                        
} CCID_POWERUP_SLOT, * FPCCID_POWERUP_SLOT;

/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _CCID_POWERDOWN_SLOT{
    IN  UINT8            Slot;
    OUT UINT8            bStatus;
    OUT UINT8            bError;
    OUT UINTN            fpDevInfo;            
} CCID_POWERDOWN_SLOT, * FPCCID_POWERDOWN_SLOT;


/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _CCID_GETSLOT_STATUS{
    OUT UINT8            bStatus;
    OUT UINT8            bError;
    OUT UINT8            bClockStatus;    
    IN  UINT8            Slot;
    OUT UINTN            fpDevInfo;            
} CCID_GETSLOT_STATUS, * FPCCID_GETSLOT_STATUS;

/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _CCID_CCID_XFRBLOCK{
    IN UINTN            CmdLength;
    IN UINTN            fpCmdBuffer;
    IN UINT8            ISBlock;
    OUT UINT8            bStatus;
    OUT UINT8            bError;
    IN OUT UINTN        ResponseLength;
    OUT UINTN            fpResponseBuffer;
    IN  UINT8            Slot;
    OUT UINTN            fpDevInfo;
} CCID_XFRBLOCK, * FPCCID_XFRBLOCK;

/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _CCID_GET_PARAMS{
    OUT UINT8            bStatus;
    OUT UINT8            bError;
    IN OUT UINTN        ResponseLength;
    OUT UINTN            fpResponseBuffer;
    IN  UINT8            Slot;
    OUT UINTN            fpDevInfo;
} CCID_GET_PARAMS, * FPCCID_GET_PARAMS;

/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _SMART_CARD_READER_CONNECT{
    IN  UINT8            Slot;
    IN  UINTN            fpDevInfo;
    IN  UINT32           AccessMode;
    IN  UINT32           CardAction;
    IN  UINT32           PreferredProtocols;
    OUT UINTN            ActiveProtocol;
    OUT EFI_STATUS       EfiStatus;
} SMART_CARD_READER_CONNECT, *FPSMART_CARD_READER_CONNECT;

/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _SMART_CARD_READER_DISCONNECT{
    IN  UINT8            Slot;
    IN  UINTN            fpDevInfo; 
    IN  UINT32           CardAction;
    OUT EFI_STATUS       EfiStatus;
} SMART_CARD_READER_DISCONNECT, * FPSMART_CARD_READER_DISCONNECT;

/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _SMART_CARD_READER_STATUS {
    IN  UINT8            Slot;
    IN  UINTN            fpDevInfo;
    OUT UINTN            ReaderName;
    IN OUT UINTN         ReaderNameLength;
    OUT UINTN            State;
    OUT UINTN            CardProtocol;
    OUT UINTN            Atr;
    IN OUT UINTN         AtrLength;
    OUT EFI_STATUS       EfiStatus;
} SMART_CARD_READER_STATUS, * FPSMART_CARD_READER_STATUS;

/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _SMART_CARD_READER_TRANSMIT{
    IN  UINT8            Slot;
    IN  UINTN            fpDevInfo;
    IN  UINTN            CAPDU;
    IN  UINTN            CAPDULength;
    OUT UINTN            RAPDU;
    IN OUT UINTN         RAPDULength;
    OUT EFI_STATUS       EfiStatus;
} SMART_CARD_READER_TRANSMIT, *FPSMART_CARD_READER_TRANSMIT;

/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _SMART_CARD_READER_CONTROL{
    IN  UINT8            Slot;
    IN  UINTN            fpDevInfo;
    IN UINT32            ControlCode;
    IN UINTN             InBuffer;
    IN UINTN             InBufferLength;
    OUT UINTN            OutBuffer;
    IN OUT UINTN         OutBufferLength;
    OUT EFI_STATUS       EfiStatus;
} SMART_CARD_READER_CONTROL, * FPSMART_CARD_READER_CONTROL;

/**
    This is a Core Procedure URP  structure for
    the BIOS API call USB_API_CCID_DEVICE_REQUEST command (API #2Fh )
**/
typedef struct _SMART_CARD_READER_GETATTRIB{
    IN  UINT8            Slot;
    IN  UINTN            fpDevInfo;
    IN UINT32            Attrib;
    OUT UINTN            OutBuffer;
    IN OUT UINTN         OutBufferLength;
    OUT EFI_STATUS       EfiStatus;
} SMART_CARD_READER_GETATTRIB, *FPSMART_CARD_READER_GETATTRIB;

/**
    This is a union data type of all the API related data

**/

typedef union {
    CK_PRESENCE             CkPresence;
    START_HC                StartHc;
    GET_DEV_INFO            GetDevInfo;
    CHK_DEV_PRSNC           ChkDevPrsnc;
    SECURITY_IF             SecurityIf;
    MASS_GET_DEV_INFO       MassGetDevInfo;
    MASS_GET_DEV_STATUS     MassGetDevSts;
    MASS_GET_DEV_GEO        MassGetDevGeo;
    MASS_RESET              MassReset;
    MASS_READ               MassRead;
    MASS_WRITE              MassWrite;
    MASS_VERIFY             MassVerify;
    MASS_FORMAT             MassFormat;
    MASS_REQ_SENSE          MassReqSense;
    MASS_TEST_UNIT_RDY      MassTstUnitRdy;
    MASS_START_STOP_UNIT    MassStartStop;
    MASS_READ_CAPACITY      MassReadCap;
    MASS_MODE_SENSE         MassModeSense;
    MASS_INQUIRY            MassInquiry;
    MASS_CMD_PASS_THRU      MassCmdPassThru;
    MASS_ASSIGN_DRIVE_NUM   MassAssignNum;
    MASS_CHK_DEV_READY      MassChkDevReady;
    MASS_GET_DEV_PARMS      MassGetDevParms;
    KB_LEDS_DATA            KbLedsData;
    UINT8                   Owner;
    HCPROC_PARAM            HcProc;
    COREPROC_PARAM          CoreProc;
    UINT8                   KbcControlCode;            //(EIP29733+)
    GET_DEV_ADDR            GetDevAddr;
    UINT8                   DevAddr;

    //    CCID APIs
    CCID_GETSMARTCLASSDESCRIPTOR CCIDSmartClassDescriptor;
    CCID_ATR                    CCIDAtr;
    CCID_POWERUP_SLOT           CCIDPowerupSlot;
    CCID_POWERDOWN_SLOT         CCIDPowerdownSlot;
    CCID_GETSLOT_STATUS         CCIDGetSlotStatus;
    CCID_XFRBLOCK               CCIDXfrBlock;
    CCID_GET_PARAMS             CCIDGetParameters;

    // SMART CARD READER PROTOCOL APIs
    SMART_CARD_READER_CONNECT      SmartCardReaderConnect;
    SMART_CARD_READER_DISCONNECT   SmartCardReaderDisconnect;
    SMART_CARD_READER_STATUS       SmartCardReaderStatus;
    SMART_CARD_READER_TRANSMIT     SmartCardReaderTransmit;
    SMART_CARD_READER_CONTROL      SmartCardReaderControl;
    SMART_CARD_READER_GETATTRIB    SmartCardReaderGetAttrib;

	UINT16					HcBusDevFuncNum;    //(EIP74876+)
	HC_START_STOP			HcStartStop;
    EFI_MASS_READ             EfiMassRead;
    EFI_MASS_WRITE            EfiMassWrite;
    EFI_MASS_VERIFY           EfiMassVerify;
    EFI_KB_LEDS_DATA          EfiKbLedsData;
} U_API_DATA;

/**
    This structure is the URP structure

 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bFuncNumber UINT8       Function number of the URP
      bSubFunc    UINT8       Sub-func number of the URP
      bRetValue   UINT8       Return value
      ApiData     API_DATA    Refer structure definition
**/

struct _URP_STRUC {
    UINT8       bFuncNumber;
    UINT8       bSubFunc;
    UINT8       bRetValue;
    U_API_DATA  ApiData;
};

#pragma pack(pop)

typedef struct {
	UINT8	NumUsbKbds;
	UINT8	NumUsbMice;
	UINT8	NumUsbPoint; 				//(EIP38434+)
	UINT8	NumUsbMass;
	UINT8	NumUsbHubs;
	UINT8	NumUsbCcids;
	UINT8	NumUhcis;
    UINT8   NumOhcis;
    UINT8   NumEhcis;
    UINT8   NumXhcis;
} CONNECTED_USB_DEVICES_NUM;

typedef VOID (EFIAPI *EFI_USB_REPORT_DEVICES ) (
  CONNECTED_USB_DEVICES_NUM	*);

typedef struct _EFI_USB_HOTPLUG_DEVS {
	BOOLEAN cdrom;
	BOOLEAN	floppy;
} EFI_USB_HOTPLUG_DEVS;

typedef EFI_STATUS (EFIAPI *EFI_USB_GET_HOTPLUG_DEVS ) (
  EFI_USB_HOTPLUG_DEVS *);

typedef EFI_STATUS (EFIAPI *EFI_USB_GET_RUNTIME_REGION ) (
  EFI_PHYSICAL_ADDRESS *,
  EFI_PHYSICAL_ADDRESS *);

typedef UINT8 (EFIAPI *EFI_USB_GET_NEXT_MASS_DEVICE_NAME ) (
  UINT8*, UINT8, UINT8);

typedef struct
{
    EFI_BLOCK_IO_PROTOCOL   BlockIoProtocol;
    EFI_BLOCK_IO_MEDIA      *Media;
    VOID                    *DevInfo;
    UINT16                  LogicalAddress;
    EFI_HANDLE              Handle;
    UINT16                  PciBDF;
    UINT8                   *DevString;
    UINT8                   StorageType;
} USB_MASS_DEV;

//(EIP51653+)>
/**
    If your roothub port 4 insert a hub.You want to skip hub's port 2.
    Set bRootPort = 4,  dRoutePath =2
    If your roothub port 4 insert a hub1.And hub1 port 2 insert a hub2. 
    You want to skip hub2's port 1. 
    Set bRootPort = 4,  dRoutePath =21 
 Fields:   Name       Type        Description
      ------------------------------------------------------------
      bSkipType   BYTE        Skip by which Type
      bFlag   	BYTE        Skip port or keep port or skip level. 
      wBDF        WORD        Bus Dev Function
		bRootPort	BYTE		Root port path
      dRoutePath 	DWORD       Hub route path. See description.
      bBaseClass 	BYTE        Device Type

**/
#define SKIP_FOR_ALLCONTROLLER              	0x1		//(EIP62695)
                                        //(EIP88776+)>
#define SKIP_FLAG_SKIP_PORT              	    0x0		 
#define SKIP_FLAG_KEEP_PORT              	    0x1		 
#define SKIP_FLAG_SKIP_LEVEL         	        0x2 //Skip usb ports on the same level.
#define SKIP_FLAG_SKIP_MULTI_LEVEL              0x3 //Skip usb ports which include down stream ports.
                                        //<(EIP88776+)
typedef struct _USB_SKIP_LIST{
	UINT8 	bSkipType;
    UINT8  	bFlag; 					    //(EIP88776)
    UINT16  wBDF;
	UINT8	bRootPort;
    UINT32  dRoutePath;
    UINT8   bBaseClass;
} USB_SKIP_LIST;
//<(EIP51653+) 

//----------------------------------------------------------------------------
//      USB Mass Storage Related Data Structures and Equates
//----------------------------------------------------------------------------
#define USB_EMU_NONE            0
#define USB_EMU_FLOPPY_ONLY     1
#define USB_EMU_HDD_ONLY        2
#define USB_EMU_HDD_OR_FDD      3
#define USB_EMU_FORCED_FDD      4

#define BAID_TYPE_HDD           1
#define BAID_TYPE_RMD_HDD       2
#define BAID_TYPE_CDROM	        3
#define BAID_TYPE_RMD_FDD       4
#define BAID_TYPE_FDD           5

// Values for Mass Storage Device type
//-------------------------------------
#define USB_MASS_DEV_UNKNOWN    0
#define USB_MASS_DEV_HDD        1
#define USB_MASS_DEV_CDROM      2
#define USB_MASS_DEV_ARMD       3
#define USB_MASS_DEV_FDD        4
#define USB_MASS_DEV_MO         5


#define STOP_USB_CONTROLLER     0				//(EIP43475+)
#define START_USB_CONTROLLER    1				//(EIP43475+)

typedef struct _EFI_USB_PROTOCOL EFI_USB_PROTOCOL;

typedef VOID (*API_FUNC)(URP_STRUC*);
typedef VOID (EFIAPI *EFI_USB_CHANGE_EFI_TO_LEGACY) (UINT8);
//typedef EFI_STATUS (EFIAPI *EFI_USB_BBS_REMOVE_MASSSTORAGE) ();

typedef EFI_STATUS (EFIAPI *EFI_INSTALL_USB_LEGACY_BOOT_DEVICES)(VOID);
typedef EFI_STATUS (EFIAPI *EFI_USB_INSTALL_LEGACY_DEVICE)(USB_MASS_DEV*);
typedef EFI_STATUS (EFIAPI *EFI_USB_UNINSTALL_LEGACY_DEVICE)(USB_MASS_DEV*);
typedef EFI_STATUS (EFIAPI *EFI_GET_ASSIGN_USB_BOOT_PORT)(UINT8*, UINT8*);
typedef VOID (EFIAPI *EFI_KBC_ACCESS_CONTROL)(UINT8);
typedef EFI_STATUS (EFIAPI *EFI_USB_RT_LEGACY_CONTROL)(VOID *);
typedef VOID (EFIAPI *EFI_USB_STOP_UNSUPPORTED_HC)();
typedef VOID (EFIAPI *EFI_USB_SHUTDOWN_LEGACY)(); 			//<(EIP52339+)
typedef VOID (EFIAPI *EFI_USB_COPY_SKIP_TABLE)(USB_SKIP_LIST*, UINT8);			//(EIP51653+)	
typedef VOID (EFIAPI *EFI_USB_RT_STOP_CONTROLLER)(UINT16);		    //(EIP74876+)
typedef VOID (EFIAPI *EFI_USB_INVOKE_API)(VOID*);
typedef EFI_STATUS (EFIAPI *EFI_USB_ACCUMULATE_ISOCHRONOUS_DATA)
        (EFI_USB_PROTOCOL*, VOID*, UINT8*, UINT8, UINT16, UINT8, UINT32, UINT32*, UINT8*);

typedef struct _EFI_USB_PROTOCOL {
	UINT32							Signature;				//(EIP55275+)
	VOID        					*USBDataPtr;
//    VOID                            *UsbBadDeviceTable;		//(EIP60706-)
	EFI_USB_REPORT_DEVICES			UsbReportDevices;
	EFI_USB_GET_NEXT_MASS_DEVICE_NAME	UsbGetNextMassDeviceName;
    EFI_USB_CHANGE_EFI_TO_LEGACY    UsbChangeEfiToLegacy;
//    EFI_USB_BBS_REMOVE_MASSSTORAGE  UsbBbsRemoveMassStorage;
    EFI_USB_GET_RUNTIME_REGION      UsbGetRuntimeRegion;
    EFI_INSTALL_USB_LEGACY_BOOT_DEVICES InstallUsbLegacyBootDevices;
    EFI_USB_INSTALL_LEGACY_DEVICE   UsbInstallLegacyDevice;
    EFI_USB_UNINSTALL_LEGACY_DEVICE UsbUninstallLegacyDevice;
    EFI_GET_ASSIGN_USB_BOOT_PORT    UsbGetAssignBootPort;
    EFI_KBC_ACCESS_CONTROL          UsbRtKbcAccessControl;
    EFI_USB_RT_LEGACY_CONTROL       UsbLegacyControl;
	EFI_USB_STOP_UNSUPPORTED_HC		UsbStopUnsupportedHc;
    EFI_USB_SHUTDOWN_LEGACY       UsbRtShutDownLegacy;      //EIP52339+
    EFI_USB_COPY_SKIP_TABLE       	UsbCopySkipTable;			//(EIP51653+)	
    EFI_USB_RT_STOP_CONTROLLER      UsbRtStopController;	        //(EIP74876+)
    EFI_USB_INVOKE_API				UsbInvokeApi;
    EFI_USB_ACCUMULATE_ISOCHRONOUS_DATA UsbAccumulateIsochronousData;
} EFI_USB_PROTOCOL;

typedef struct {
    UINT32                          ConstantDataCrc32;
    UINT32                          Crc32Hash;
} AMI_USB_GLOBAL_DATA_VALIDATION;

typedef struct {
	EFI_USB_STOP_UNSUPPORTED_HC		UsbStopUnsupportedHc;
    API_FUNC                        *UsbApiTable;
    API_FUNC                        *UsbMassApiTable;
    AMI_USB_GLOBAL_DATA_VALIDATION  GlobalDataValidation;
} AMI_USB_SMM_PROTOCOL;

#endif
#endif // _USB_PROT_H

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
