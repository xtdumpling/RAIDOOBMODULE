//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     Jun/07/2016
//
//****************************************************************************
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

/** @file IpmiBoot.h
    Header file for IPMI Boot Modular part

**/

#ifndef __IPMI_BOOT__H__
#define __IPMI_BOOT__H__

//----------------------------------------------------------------------

#include "Token.h"
#include <AmiDxeLib.h>
#include "BootOptions.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/HobList.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Protocol/DevicePath.h>
#include <Include/IpmiNetFnChassisDefinitions.h>

//----------------------------------------------------------------------

//----------------------------------------------------------------------

#define IPMI_STALL                       1000
#define MAXIMUM_BOOT_OPTION_NUMBER       0xFFFF

#define BOOT_INFO_HANDLED_BY_BIOS        (1 << 0)  // BIT0
#define BOOT_INFO_HANDLED_BY_OS_LOADER   (1 << 1)  // BIT1
#define BOOT_INFO_HANDLED_BY_OS          (1 << 2)  // BIT2
#define BOOT_INFO_HANDLED_BY_SMS         (1 << 3)  // BIT3
#define BOOT_INFO_HANDLED_BY_OEM         (1 << 4)  // BIT4

#define VAR_FLAG            EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                            EFI_VARIABLE_NON_VOLATILE

//
// define BBS Device Types
//
#define BBS_FLOPPY        0x01
#define BBS_HARDDISK      0x02
#define BBS_CDROM         0x03
#define BBS_PCMCIA        0x04
#define BBS_USB           0x05
#define BBS_EMBED_NETWORK 0x06
#define BBS_BEV_DEVICE    0x80
#define BBS_UNKNOWN       0xff

#if SMCPKG_SUPPORT
#define USB_HARDDISK      0x09
#define USB_CDROM         0x0a
#define USB_KEY           0x0b
#define USB_FLOPPY        0x08
#endif
//
// define BAID Device Types
//
#define BAID_TYPE_HDD       1
#define BAID_TYPE_RMD_HDD   2
#define BAID_TYPE_CDROM     3
#define BAID_TYPE_RMD_FDD   4
#define BAID_TYPE_FDD       5

//
//USB device Class code
//
#define PCI_CL_SER_BUS  0x0C

//
//USB Device SubClass Code
//
#define PCI_CL_SER_BUS_SCL_USB          0x03
#define BOOT_FLOW_CONDITION_FIRST_BOOT  0x04

#define BOOT_FLOW_VARIABLE_GUID \
    { 0xef152fb4, 0x7b2f, 0x427d, { 0xbd, 0xb4, 0x7e, 0x0a, 0x05, 0x82, 0x6e, 0x64 } }

#define IPMI_BOOT_GUID  \
    { 0x842680f2, 0x1a9c, 0x48e6, { 0xa4, 0x33, 0xbe, 0x9a, 0xcb, 0xd, 0xd4, 0x38 } }


#define EfiCopyMem(Destinaton, Source, Length)      gBS->CopyMem(Destinaton, Source, Length)
#define EfiZeroMem(Address, Length)                 gBS->SetMem  ((Address), (Length), 0)
#define EfiAllocatePool(EfiBootServicesData, AllocationSize, Memory)\
                         gBS->AllocatePool (EfiBootServicesData, AllocationSize, (VOID **)&Memory)

#define SetDevicePathNodeLength     SET_NODE_LENGTH
#define EfiAppendDevicePathNode     DPAddNode

#define HIGHEST_BOOT_OPTION_PRIORITY            0x00
#define HIGHEST_BOOT_OPTION_TAG                 0x00
#define INTERNAL_EXTERNAL_DEVICE_INSTANCE_MASK  0x10
#define DEVICE_INSTANCE_SELECTION_MASK          0x0F
#define BOOT_OPTION_NUMBER_SIZE    sizeof(UINT16)

#define INVALID_DEVICE            0xFF
#define NON_USB_COMPATIBLE_DEVICE 0x00
#define USB_COMPATIBLE_DEVICE     0x01

#if (defined (CSM_SUPPORT) && CSM_SUPPORT)
#define IsUsbDeviceClassCode(Option) ((Option )? ((Option)->BbsEntry->Class == \
    PCI_CL_SER_BUS) && ((Option)->BbsEntry->SubClass == \
    PCI_CL_SER_BUS_SCL_USB): 0)
#define isUsbCompatibleDevice(Option,DevType) \
    ((Option) ? ((((Option)->BbsEntry->InitPerReserved) >> 24)& 0XFF) == DevType: 0)
#endif

#define isAcpiDeviePath(pDp)  ( (pDp) ? (pDp)->Header.Type==ACPI_DEVICE_PATH && \
    (pDp)->Header.SubType==ACPI_DP && \
    (pDp)->HID==EISA_PNP_ID(0x0604): 0)
#define isBbsDeviePath(pDp, BbsDevType)  \
    ( (pDp) ? (pDp)->Header.Type==BBS_DEVICE_PATH && \
    (pDp)->Header.SubType==BBS_BBS_DP && \
    (pDp)->DeviceType==BbsDevType: 0)

//
// Function Prototypes
//

EFI_STATUS
IpmiWaitSetInProgressClear (
  VOID
  );

EFI_STATUS
IpmiClearBootFlags (
  VOID
  );

EFI_STATUS
IpmiSetBootInfoAck (
  IN UINT8                    BootInfo
  );

EFI_STATUS
IpmiGetBootFlags (
  OUT GET_BOOT_FLAG_RESPONSE  *ResponseParameters
  );

VOID                      *
GetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  );

BOOLEAN
CheckNodeInDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *pDp,
  IN  UINT16                    DpType,
  IN  UINT16                    DpSubType
  );

BOOT_OPTION *
SetUsbDeviceBootOptionToHighestPriority(
  IN  UINTN                     RequestedUsbDeviceInstance,
  IN  UINT16                    BbsDevType,
  IN  UINT16                    DpType,
  IN  UINT16                    DpSubType
  );

VOID
SaveMatchedBootOptionData(
  IN  UINT16 BootOptionNumber
  );

EFI_STATUS 
SetBootOptionToHighestPriority(
  IN  UINT16                    BbsDevType,
  IN  UINT16                    DpType,
  IN  UINT16                    DpSubType
  );

#if SMCPKG_SUPPORT
EFI_STATUS
RemoteDeviceBoot(
  IN IPMI_BOOT_DEVICE    IpmiForceBootDevice
  );

#else

BOOLEAN
RemoteDeviceBoot(
  IN IPMI_BOOT_DEVICE    IpmiForceBootDevice
  );
#endif

BOOLEAN
BootToDiagnosticPartitionOrHddRequestSafeMode (
  IN  BOOLEAN        UefiBoot,
  IN  UINT16         BbsDevType,
  IN  UINT16         DpType,
  IN  UINT16         DpSubType,
  IN  BOOT_OPTION    *BootOption
  );

VOID
UpdateIpmiBootGlobalVariables (
  IN GET_BOOT_FLAG_RESPONSE     BootFlags
  );

VOID *
EfiAllocateZeroPool (
  IN  UINTN   AllocationSize
  );

VOID
SaveCurrentBootorder (
  VOID
  );

VOID
EFIAPI
ReadBootOptions (
  VOID
  );

BOOT_OPTION* 
CreateBootOptionsFromNvramOption (
  IN DLIST           *BootOptionList,
  IN UINT16          BootOptionNumber,
  IN EFI_LOAD_OPTION *NvramOption,
  IN UINTN           NvramOptionSize,
  IN UINT32          *Priority
  );

extern EFI_GUID gEfiIpmiBootGuid;

//----------------------------------------------------------------------

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
