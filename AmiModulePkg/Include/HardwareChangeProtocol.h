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


/** @file  HardwareChangeProtocol.h

  The header file for Hardware change protocol. 

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.
  
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __HARDWARE_CHANGE_PROTOCOL__H__
#define __HARDWARE_CHANGE_PROTOCOL__H__

#ifdef __cplusplus
extern "C" {
#endif

// {43169678-506C-46fe-B32A-FCB301F74FBD}
#define HARDWARE_CHANGE_PROTOCOL_GUID \
   { 0x43169678, 0x506c, 0x46fe, { 0xb3, 0x2a, 0xfc, 0xb3, 0x1, 0xf7, 0x4f, 0xbd } }

// {B80A8E5B-C02A-4a31-AE12-58E46E803E89}
#define HARDWARE_CONFIG_DATA_GUID \
   { 0xb80a8e5b, 0xc02a, 0x4a31, { 0xae, 0x12, 0x58, 0xe4, 0x6e, 0x80, 0x3e, 0x89 } }

// {81c76078-bfde-4368-9790-570914c01a65}
#define SETUP_UPDATE_COUNT_GUID \
   { 0x81c76078, 0xbfde, 0x4368, { 0x97, 0x90, 0x57, 0x09, 0x14, 0xc0, 0x1a, 0x65 } }

#define SETUP_UPDATE_COUNT_VARIABLE L"SetUpdateCountVar"


#define HW_CHANGE_PS2_KEYBOARD       BIT0
#define HW_CHANGE_PS2_MOUSE          BIT1
#define HW_CHANGE_MEMORY_SIZE_CONFIG BIT2
#define HW_CHANGE_MEMORY_MAP_CONFIG  BIT3
#define HW_CHANGE_PCI_CONFIG         BIT4
#define HW_CHANGE_USB_CONFIG         BIT5
#define HW_CHANGE_VIDEO_CONFIG       BIT6
#define HW_CHANGE_DISK_CONFIG        BIT7
//For limit the protocol be called
#define LIMIT_PROTOCOL_CALLED 20

#if HARDWARE_SIGNATURE_DEBUG_MESSAGES
#define HWSIG_TRACE(Arguments) TRACE(Arguments)
#else
#define HWSIG_TRACE(Arguments)
#endif

typedef enum {
  Ps2KeyboardConfigType,
  Ps2MouseConfigType,
  MemorySizeConfigType,
  MemoryMapConfigType,
  PciConfigType,
  UsbConfigType,
  VideoConfigType,
  DiskConfigType,
  MaxConfigType
} HW_CONFIG_TYPE;

#pragma pack (push,1)

/**
  HW_MEMORY_MAP_DATA

  This structure represents the memory map data which contains
  memory size(Megabyte) and checksum of the runtime memory type.

  MemMapCheckSum : Checksum of the runtime memory type.
  MemoryMbSize : Total memory size(Megabyte).

**/
typedef struct _HW_MEMORY_MAP_DATA {
  UINT8   MemMapCheckSum;
  UINT32  MemoryMbSize;
} HW_MEMORY_MAP_DATA;

/**
  HW_PCI_DATA

  This structure represents the PCI configuration data which 
  contains number of PCI/PCIE devices, checksum of Bus number/Device number/ 
  Function number, and checksum of VID/DID.

  PciNum    : Number of PCI/PCIE devices .
  BdfChecksum : Checksum of Bus number/Device number/Function number .
  VidDidChecksum     : Checksum of VID/DID.

**/
typedef struct _HW_PCI_DATA {
  UINT16  PciNum;
  UINT8   BdfChecksum;
  UINT8   VidDidChecksum;
} HW_PCI_DATA;

/**
  HW_USB_DATA

  This structure represents the USB configuration data which 
  contains number of USB devices, checksum of VID/PID, and checksum of Port  
  number and Interface number.

  UsbNum : Number of USB devices.
  VidPidChecksum : Checksum of VID/PID.
  PortNumInterfaceNumChecksum : Checksum of Port number and Interface number.
  
**/
typedef struct _HW_USB_DATA {
  UINT16  UsbNum;
  UINT8   VidPidChecksum;
  UINT8   PortNumInterfaceNumChecksum;
} HW_USB_DATA;

/**
  HW_VIDEO_DATA

  This structure represents the Video configuration data which 
  contains checksum of EDID data, and checksum of resolution.  

  EdidCheckNum : Checksum of EDID data. 
  ResolutionChecksum : Checksum of resolution.
  
**/
typedef struct _HW_VIDEO_DATA {
  UINT8   EdidCheckNum;
  UINT8   ResolutionChecksum;
} HW_VIDEO_DATA;

/**
  HW_DISK_DATA

  This structure represents the Disk configuration data with checksum.
  
  DiskInfoChecksum : The checksum included Ide Channel, Device , Serial number.
  
**/
typedef struct _HW_DISK_DATA {
  UINT32    DiskInfoChecksum;
} HW_DISK_DATA;

/**
  DISK_STRUCTURE

  This structure represents the Disk configuration data which 
  contains port number and serial number .

  PortNumber : Port number.
  SerialNumber : Serial number.
  
**/
typedef struct _DISK_STRUCTURE {
  UINT8     PortNumber;
  UINT8     SerialNumber[20];
} DISK_STRUCTURE;

/**
  HW_CONFIG_DATA

  This structure represents the hardware configuration data 
  which contains PS2 Keyboard/Mouse configuration, Memory Map configuration,  
  PCI configuration Data, USB configuration Data,  Video configuration data, 
  Disk configuration data, Firmware update times, and Checksum which external 
  function given.

  BitPs2Keyboard : The present state of PS2 Keyboard.
  BitPs2Mouse : The present state of PS2 Mouse.
  MemMapData : Memory configuration data.
  PciData   : PCI configuration data.
  UsbData : USB configuration data.
  VideoData : Video configuration data.
  DiskData : Disk configuration data.
  FirmwareUpdateCount : Firmware update count.
  ReturnChecksum     : Checksum which the external function given.
  
**/
typedef struct _HW_CONFIG_DATA {
  BOOLEAN BitPs2Keyboard;
  BOOLEAN BitPs2Mouse;  
  HW_MEMORY_MAP_DATA MemMapData;
  HW_PCI_DATA PciData;
#if HARDWARE_SIGNATURE_USB_CHANGE  
  HW_USB_DATA UsbData;
#endif
  HW_VIDEO_DATA VideoData;
  HW_DISK_DATA DiskData;
  UINT32 SetupUpdateCount;
  UINT32 ReturnChecksum[LIMIT_PROTOCOL_CALLED];
  UINT16  BiosBuildYear;
  UINT8   BiosBuildMonth;
  UINT8   BiosBuildDay;
  UINT8   BiosBuildHour;
  UINT8   BiosBuildMinute;
  UINT8   BiosBuildSecond;
} HW_CONFIG_DATA;
#pragma pack (pop)

typedef 
EFI_STATUS (EFIAPI *SET_HW_CONFIG_DATA) (
    IN HW_CONFIG_TYPE ConfigType, 
    IN UINTN BufferSize,
    IN VOID *Buffer
);

/**
  GET_HW_CONFIG_DATA_FROM_NVRAM

  This structure represents the Hardware configuration data with checksum.
  
  HardwareConfigData : The checksum of hardware config data..
  
**/
typedef 
EFI_STATUS (EFIAPI *GET_HW_CONFIG_DATA_FROM_NVRAM) (
    IN OUT HW_CONFIG_DATA *HardwareConfigData
);

/**
  SIGNAL_HW_SIGNATURE_CHANGE

  This structure represents the checksum that externel function reported.
  
  ReturnChecksum : The checksum that externel function reported.
  
**/
typedef 
EFI_STATUS (EFIAPI *SIGNAL_HW_SIGNATURE_CHANGE) (
    IN UINT32 ReturnChecksum
);

/**
  EFI_HARDWARE_CHANGE_PROTOCOL

  This structure represents the HardwareSignature protocol.
    
**/  
typedef struct {
    SET_HW_CONFIG_DATA             SetHardwareConfigData;
    GET_HW_CONFIG_DATA_FROM_NVRAM  GetHardwareConfigDataFromNvram;
    SIGNAL_HW_SIGNATURE_CHANGE     SignalHardwareSignatureChange;
} EFI_HARDWARE_CHANGE_PROTOCOL;

EFI_STATUS 
EFIAPI
SetHardwareConfigData(
    IN HW_CONFIG_TYPE ConfigType, 
    IN UINTN BufferSize,
    IN VOID *Buffer
);

EFI_STATUS 
EFIAPI
GetHardwareConfigDataFromNvram(
    IN OUT HW_CONFIG_DATA *HardwareConfigData
);

EFI_STATUS 
EFIAPI
SignalHardwareSignatureChange( 
    IN UINT32 ReturnChecksum );

EFI_STATUS GetSetupCountFromNvram(
    IN OUT UINT32 *ChangeTime );

UINT32 Crc32Algorithm (
    IN UINT8  *FpData,
    IN UINT16 Length,
    IN UINT8  ExtenData 
);


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
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

