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

/** @file IpmiBootBoard.c
    This file validates Remotely connected device details
    with boot option to select proper remote device. Remote devices details
    needs to be ported based on device type.

**/

//----------------------------------------------------------------------

#include "Efi.h"
#include "Token.h"//SMCPKG_SUPPORT
#if SMCPKG_SUPPORT
#include "IpmiBoot.h"
#else
#include "BootOptions.h"
#endif
#include "IpmiNetFnChassisDefinitions.h"

//----------------------------------------------------------------------

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

//
// Extern global variable
//
extern EFI_BOOT_SERVICES     *gBS;
extern IPMI_BOOT_DEVICE      gIpmiForceBootDevice;


/*
//
// Macro Definitions
//
#define isAcpiDeviePath(pDp) ( (pDp)->Header.Type==ACPI_DEVICE_PATH && (pDp)->Header.SubType==ACPI_DP )
#define isPciDeviePath(pDp) ( (pDp)->Header.Type==HARDWARE_DEVICE_PATH && (pDp)->Header.SubType==HW_PCI_DP )
#define isUsbDeviePath(pDp) ( (pDp)->Header.Type==MESSAGING_DEVICE_PATH && (pDp)->Header.SubType==MSG_USB_DP )

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   EfiDevicePathFromHandle
//
// Description:  Get the device path protocol interface installed on a 
//                  specified handle
//
// Input: IN EFI_HANDLE Handle - Handle to get device path
//
// Output:  EFI_DEVICE_PATH_PROTOCOL* - Returns device path installed on Given handle
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_DEVICE_PATH_PROTOCOL *
EfiDevicePathFromHandle (
  IN EFI_HANDLE  Handle )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  DevicePath = NULL;
  pBS->HandleProtocol (
        Handle,
        &gEfiDevicePathProtocolGuid,
        (VOID *) &DevicePath );
  return DevicePath;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   CompareRemoteDeviceDetails
//
// Description:  Compares remote device details with Boot option details
//
// Input: IN BOOT_OPTION *Option - the option that should be validated with 
//                  remote device details
//
// Output:  BOOLEAN - compares the two strings and returns the difference
//
// Note: This is a porting function. Based on the Remote device details,
//          this function definition can be changed to verify the boot option
//          details with remote device.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN
CompareRemoteDeviceDetails (
  IN BOOT_OPTION    *BootOption )
{

    ACPI_HID_DEVICE_PATH       *AcpiDPNode;
    PCI_DEVICE_PATH             *PciDPNode;
    USB_DEVICE_PATH             *UsbDPNode;

    EFI_DEVICE_PATH_PROTOCOL    *OptionDp;

    OptionDp = EfiDevicePathFromHandle (BootOption->DeviceHandle);

    //
    // Device path: ACPI(PNP0A03,0)/PCI(1A|0)/USB(1,0)/USB(3,0)
    // Check for ACPI device path: ACPI(PNP0A03,0) 
    //
    AcpiDPNode = (ACPI_HID_DEVICE_PATH  *) OptionDp;
    if ( !( (isAcpiDeviePath(AcpiDPNode)) && \
            (AcpiDPNode->HID == EISA_PNP_ID(0x0A03)) &&\
            (AcpiDPNode->UID == 0x00) \
           )) {
        return FALSE;
    }

    //
    // Check for PCI device path: PCI(1A|0)
    //
    OptionDp = NEXT_NODE(OptionDp);
    PciDPNode = (PCI_DEVICE_PATH *) OptionDp;
    if ( !( (isPciDeviePath(PciDPNode)) && \
            (PciDPNode->Function == 0x00) &&\
            (PciDPNode->Device == 0x1A) \
           )) {
        return FALSE;
    }

    //
    // Check for USB device path: USB(1,0)
    //        
    OptionDp = NEXT_NODE(OptionDp);
    UsbDPNode = (USB_DEVICE_PATH *) OptionDp;
    if ( !( (isUsbDeviePath(UsbDPNode)) && \
            (UsbDPNode->ParentPortNumber == 0x01) &&\
            (UsbDPNode->InterfaceNumber == 0x00) \
           )) {
        return FALSE;
    }

    //
    // Check for USB device path: USB(3,0)
    //        
    OptionDp = NEXT_NODE(OptionDp);
    UsbDPNode = (USB_DEVICE_PATH *) OptionDp;
    if ( !( (isUsbDeviePath(UsbDPNode)) && \
            (UsbDPNode->ParentPortNumber == 0x03) &&\
            (UsbDPNode->InterfaceNumber == 0x00) \
           )) {
        return FALSE;
    }

    return TRUE;
}
*/
/**
    Boot to remote device if proper
    remote device is available

    @param IpmiForceBootDevice Boot device

    @return EFI_STATUS returns Status of Remote boot

**/

EFI_STATUS
RemoteDeviceBoot (
    IPMI_BOOT_DEVICE    IpmiForceBootDevice )
{
    EFI_STATUS  Status = EFI_SUCCESS;
#if SMCPKG_SUPPORT
    SERVER_IPMI_TRACE ((-1, "RemoteDeviceBoot\n"));
    switch ( IpmiForceBootDevice ) {
        case ForceBootRemoteFloppy:
            SERVER_IPMI_TRACE ((-1, "ForceBootRemoteFloppy\n"));
            Status = SetBootOptionToHighestPriority (
            		     USB_FLOPPY,
                         MEDIA_DEVICE_PATH,
                         MSG_USB_DP );
								
            break;
        case ForceBootRemoteHardDrive:
            SERVER_IPMI_TRACE ((-1, "ForceBootRemoteHardDrive\n"));
            Status = SetBootOptionToHighestPriority (
            		     USB_HARDDISK,
                         MEDIA_DEVICE_PATH,
                         MSG_USB_DP );								
            break;

        case ForceBootRemoteCdDvd:
            SERVER_IPMI_TRACE ((-1, "ForceBootRemoteCdDvd\n"));
            Status = SetBootOptionToHighestPriority (
            		     USB_CDROM,
                         MEDIA_DEVICE_PATH,
                         MSG_USB_DP );            
            break;

        case ForceBootPrimaryRemoteMedia:
            SERVER_IPMI_TRACE ((-1, "ForceBootRemoteUSBKEY\n"));
            Status = SetBootOptionToHighestPriority (
            		     USB_KEY,
                         MEDIA_DEVICE_PATH,
                         MSG_USB_DP );
            break;
        default:
            break;
    }
#else
    switch ( IpmiForceBootDevice ) {
        case ForceBootRemoteFloppy:
         /* Sample Code 
          * Status = SetBootOptionToHighestPriority (
                                BBS_FLOPPY,
                                ACPI_DEVICE_PATH,
                                ACPI_DP ); */
            break;
        case ForceBootRemoteHardDrive:
        /* Sample code 
         * Status = SetBootOptionToHighestPriority (
                                BBS_HARDDISK,
                                MEDIA_DEVICE_PATH,
                                MEDIA_HARDDRIVE_DP ); */
            break;

        case ForceBootRemoteCdDvd:
        /* Sample Code 
         * Status = SetBootOptionToHighestPriority (
                                BBS_CDROM,
                                MEDIA_DEVICE_PATH,
                                MEDIA_CDROM_DP ); */
            break;
        default:
            break;

    }
#endif	
    return Status;
}

/**
    Validates whether to boot to normal HDD for ForceBootHardDriveSafeMode and
    ForceBootDiagnosticPartition also.

    @param UefiBoot TRUE for UEFI boot type, FALSE for Legacy boot
    @param BbsDevType BBS device Type of the Remote device
    @param DpType Device path type of the Remote device
    @param DpSubType Device path sub type of the Remote device
    @param Option The option that should be validated with 
           remote device details

    @retval TRUE Return TRUE if not to boot to normal HDD
    @retval FALSE Returns FALSE if boot to normal HDD
**/

BOOLEAN
BootToDiagnosticPartitionOrHddRequestSafeMode (
  IN  BOOLEAN        UefiBoot,
  IN  UINT16         BbsDevType,
  IN  UINT16         DpType,
  IN  UINT16         DpSubType,
  IN BOOT_OPTION    *BootOption
  )
{

    //
    // gIpmiForceBootDevice tells the type of user selected boot device
    //
    switch ( gIpmiForceBootDevice )
    {
        case ForceBootHardDriveSafeMode:
            //
            // write the code to validate boot option for
            // ForceBootHardDriveSafeMode and return appropriate value
            //
            break;
        case ForceBootDiagnosticPartition:
            //
            // write the code to validate boot option for
            // ForceBootDiagnosticPartition and return appropriate value
            //
            break;
        default:
            break;
          
    }
    //
    // Always return False as BIOS doesn't know the difference between ForceBootHardDrive,
    // ForceBootHardDriveSafeMode,ForceBootDiagnosticPartition
    //
    return FALSE;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
