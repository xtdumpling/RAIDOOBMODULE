//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
  
*/
#ifndef  _AMI_PROTOCOL_H   //To Avoid this header get compiled twice
#define  _AMI_PROTOCOL_H

#include <Efi.h>
// Include files from EDKII with definitions that moved locations
#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <KeyboardCommonDefinitions.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/VariableWrite.h>
#include <Library/DevicePathLib.h>

// Include files from EDKII with definitions needed in this file
// Some of the protocols are commented out.
// If a macro that was defined in the Aptio 4 header X.h 
// is defined here using definitions from the EDKII version of X.h, 
// no need to include X.h here.

#include <Protocol/DevicePath.h>
#include <Uefi/UefiSpec.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/SerialIo.h>
#include <Protocol/UnicodeCollation.h>


//Protocol/AcpiSupport.h
extern EFI_GUID gEfiAcpiSupportGuid;
#define EFI_ACPI_TABLE_VERSION_X        (EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0 | EFI_ACPI_TABLE_VERSION_4_0)
#define EFI_ACPI_TABLE_VERSION_ALL      (EFI_ACPI_TABLE_VERSION_1_0B|EFI_ACPI_TABLE_VERSION_X)

// Protocol/BootScriptSave.h
// The following definitions are in AMI's version but not EDKII
#define EFI_BOOT_SCRIPT_SAVE_GUID EFI_BOOT_SCRIPT_SAVE_PROTOCOL_GUID
#define EFI_ACPI_S3_RESUME_SCRIPT_TABLE					0x00

#define BOOT_SCRIPT_IO_WRITE_MACRO(This, TableName, Width, Address, Count, Buffer) \
	This->Write(This,TableName,EFI_BOOT_SCRIPT_IO_WRITE_OPCODE,Width,(UINT64)Address,(UINTN)Count,Buffer)

#define BOOT_SCRIPT_IO_READ_WRITE_MACRO(This,TableName,Width,Address,Data,DataMask) \
	This->Write(This,TableName,EFI_BOOT_SCRIPT_IO_READ_WRITE_OPCODE,Width,(UINT64)Address,Data,DataMask)

#define BOOT_SCRIPT_MEM_WRITE_MACRO(This,TableName,Width,Address,Count,Buffer) \
	This->Write(This,TableName,EFI_BOOT_SCRIPT_MEM_WRITE_OPCODE,Width,(UINT64)Address,(UINTN)Count,Buffer)

#define BOOT_SCRIPT_MEM_READ_WRITE_MACRO(This,TableName,Width,Address,Data,DataMask) \
	This->Write(This,TableName,EFI_BOOT_SCRIPT_MEM_READ_WRITE_OPCODE,Width,(UINT64)Address,Data,DataMask)

#define BOOT_SCRIPT_PCI_CONFIG_WRITE_MACRO(This,TableName,Width,Address,Count,Buffer) \
	This->Write(This,TableName,EFI_BOOT_SCRIPT_PCI_CONFIG_WRITE_OPCODE,Width,(UINT64)Address,(UINTN)Count,Buffer)

#define BOOT_SCRIPT_PCI_CONFIG_READ_WRITE_MACRO(This,TableName,Width,Address,Data,DataMask) \
	This->Write(This,TableName,EFI_BOOT_SCRIPT_PCI_CONFIG_READ_WRITE_OPCODE,Width,(UINT64)Address,Data,DataMask)

#define BOOT_SCRIPT_SMBUS_EXECUTE_MACRO(This,TableName,SlaveAddress,Command,Operation,PecCheck,Length,Buffer) \
	This->Write(This,TableName,EFI_BOOT_SCRIPT_SMBUS_EXECUTE_OPCODE,SlaveAddress,Command,Operation,PecCheck,Length,Buffer)

#define BOOT_SCRIPT_STALL_MACRO(This,TableName,Duration) \
	This->Write(This,TableName,EFI_BOOT_SCRIPT_STALL_OPCODE,Duration)

#define BOOT_SCRIPT_DISPATCH_MACRO(This,TableName,EntryPoint) \
	This->Write(This,TableName,EFI_BOOT_SCRIPT_DISPATCH_OPCODE,(EFI_PHYSICAL_ADDRESS)EntryPoint)

//OEM

#define BOOT_SCRIPT_IO_POLL_MACRO(This,TableName,Width,Address,Mask,Result) \
    This->Write(This,TableName,EFI_BOOT_SCRIPT_IO_POLL_OPCODE_OEM,Width,(UINT64)Address,Mask,Result)

#define BOOT_SCRIPT_MEM_POLL_MACRO(This,TableName,Width,Address,Mask,Result) \
    This->Write(This,TableName,EFI_BOOT_SCRIPT_MEM_POLL_OPCODE_OEM,Width,(UINT64)Address,Mask,Result)

#define BOOT_SCRIPT_PCI_POLL_MACRO(This,TableName,Width,Address,Mask,Result) \
    This->Write(This,TableName,EFI_BOOT_SCRIPT_PCI_POLL_OPCODE_OEM,Width,(UINT64)Address,Mask,Result)

// Protocol/DataHub.h
// These #defines have a different name in IntelFrameworkPkg
#define EFI_DATA_CLASS_DEBUG			EFI_DATA_RECORD_CLASS_DEBUG
#define EFI_DATA_CLASS_ERROR			EFI_DATA_RECORD_CLASS_ERROR
#define EFI_DATA_CLASS_DATA				EFI_DATA_RECORD_CLASS_DATA
#define EFI_DATA_CLASS_PROGRESS_CODE	EFI_DATA_RECORD_CLASS_PROGRESS_CODE


// Protocol/DebugSupport.h
// These #defines have a different name in MdePkg
#define EXCEPT_EBC_SINGLE_STEP  EXCEPT_EBC_STEP

// Protocol/DevicePath.h
// AMI definitions with different names in MdePkg
#define MBR_TYPE_MASTER_BOOT_RECORD     MBR_TYPE_PCAT

#define MSG_USB_WWID_CLASS_DP           MSG_USB_WWID_DP
#define MSG_USB_LOGICAL_UNIT_CLASS_DP   MSG_DEVICE_LOGICAL_UNIT_DP
#define MSG_USB_SATA_DP                 MSG_SATA_DP
#define MSG_USB_ISCSI_DP 				MSG_ISCSI_DP
#define MEDIA_FV_FILEPATH_DP            MEDIA_PIWG_FW_FILE_DP
#define MEDIA_FV_DP                     MEDIA_PIWG_FW_VOL_DP
#define END_DEVICE_PATH                 END_DEVICE_PATH_TYPE
#define END_INSTANCE_SUBTYPE            END_INSTANCE_DEVICE_PATH_SUBTYPE
#define END_ENTIRE_SUBTYPE              END_ENTIRE_DEVICE_PATH_SUBTYPE

#define EFI_SAS_PROTOCOL_GUID           EFI_SAS_DEVICE_PATH_GUID


#define HW_PCI_DEVICE_PATH_LENGTH       0x06
#define ACPI_DEVICE_PATH_LENGTH         0x0C
#define MSG_DEVICE_PATH_LENGTH          0x08
#define ATAPI_DEVICE_PATH_LENGTH        0x08
#define SATA_DEVICE_PATH_LENGTH         0x0A
#define UART_DEVICE_PATH_LENGTH         0x13

// The definition of NODE_LENGTH and SET_NODE_LENGTH macros is different than in Aptio 4.
// Unlike Aptio 4, Aptio V supports multiple CPU architectures.
// The defintion has been updated to prevent alignment fault on ARM and Itenium.
// This modification does not affect Aptio 4 compatibility.
// Code the uses these macro will still work.
#define NODE_LENGTH(pPath) ((pPath)->Length[0]+((pPath)->Length[1]<<8))
#define SET_NODE_LENGTH(pPath,NodeLength) ((pPath)->Length[0]=(UINT8)(NodeLength),(pPath)->Length[1]=(NodeLength)>>8)
#define NEXT_NODE(pPath) ((EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)(pPath)+NODE_LENGTH(pPath)))
#define isEndNode(pDp) ((pDp)->Type==END_DEVICE_PATH)

#pragma pack (push)	//May not be alligned on a boundary.
#pragma pack (1)

typedef DEVICE_LOGICAL_UNIT_DEVICE_PATH  LOGICAL_UNIT_DEVICE_PATH;
typedef I2O_DEVICE_PATH                  I20_DEVICE_PATH;
typedef ISCSI_DEVICE_PATH                iSCSI_DEVICE_PATH;

// Named differently in MdePkg with different member names
typedef struct {
    EFI_DEVICE_PATH_PROTOCOL Header;
    EFI_MAC_ADDRESS          MacAddr;
    UINT8                    IfType;
} MAC_ADDRESS_DEVICE_PATH;

//Media Device Path Type
typedef struct {
	EFI_DEVICE_PATH_PROTOCOL	Header;
	EFI_GUID		ProtocolGuid;
} PROTOCOL_DEVICE_PATH;

#pragma pack (pop)


// Protocol/IdeControllerInit.h
typedef enum {
  EfiIdeCableTypeUnknown,
  EfiIdeCableType40pin,
  EfiIdeCableType80Pin,
  EfiIdeCableTypeSerial,
  EfiIdeCableTypeMaximum
} EFI_IDE_CABLE_TYPE;

// Protocol/PciRootBridgeIo.h
typedef EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS  EFI_PCI_CONFIGURATION_ADDRESS;

// Protocol/SerialIo.h
#define EFI_SERIAL_SOFTWARE_FLOW_CONTROL_ENABLE	0x8000
typedef EFI_SERIAL_IO_MODE SERIAL_IO_MODE;

// Protocol/SimpleTextOut.h
typedef EFI_SIMPLE_TEXT_OUTPUT_MODE  SIMPLE_TEXT_OUTPUT_MODE;

// Protocol/StatusCode.h
// GUID for the Status Code Architectural Protocol
#define EFI_STATUS_CODE_ARCH_PROTOCOL_GUID \
  { 0xd98e3ea3, 0x6f39, 0x4be4, 0x82, 0xce, 0x5a, 0x89, 0x0c, 0xcb, 0x2c, 0x95 }
extern EFI_GUID gEfiStatusCodeArchProtocolGuid;

// Protocol/TcgService.h
#define EFI_TCG_PLATFORM_PROTOCOL_GUID  \
  { 0x8c4c9a41, 0xbf56, 0x4627, 0x9e, 0xa, 0xc8, 0x38, 0x6d, 0x66, 0x11, 0x5c }
extern EFI_GUID gEfiTcgPlatformProtocolGuid;
#define TSS_EVENT_DATA_MAX_SIZE   256

//
// EFI TCG Platform Protocol
//
typedef
EFI_STATUS
(EFIAPI *EFI_TCG_MEASURE_PE_IMAGE) (
  IN      BOOLEAN                   BootPolicy,
  IN      EFI_PHYSICAL_ADDRESS      ImageAddress,
  IN      UINTN                     ImageSize,
  IN      UINTN                     LinkTimeBase,
  IN      UINT16                    ImageType,
  IN      EFI_HANDLE                DeviceHandle,
  IN      EFI_DEVICE_PATH_PROTOCOL  *FilePath
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TCG_MEASURE_ACTION) (
  IN      CHAR8                     *ActionString
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TCG_MEASURE_GPT_TABLE) (
  IN      EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

typedef struct _EFI_TCG_PLATFORM_PROTOCOL {
  EFI_TCG_MEASURE_PE_IMAGE          MeasurePeImage;
  EFI_TCG_MEASURE_ACTION            MeasureAction;
  EFI_TCG_MEASURE_GPT_TABLE         MeasureGptTable;
} EFI_TCG_PLATFORM_PROTOCOL;

// UnicodeCollation.h
#define EFI_UNICODE_COLLATION2_PROTOCOL_GUID EFI_UNICODE_COLLATION_PROTOCOL2_GUID 
typedef EFI_UNICODE_COLLATION_PROTOCOL EFI_UNICODE_COLLATION2_PROTOCOL;

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
