//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: /Alaska/SOURCE/Core/Library/Print.c 7     7/10/09 4:01p Felixp $
//
// $Revision: 7 $
//
// $Date: 7/10/09 4:01p $
//*************************************************************************
//<INT:AMI_FHDR_START>
//
// Name:	Print.c
//
// Description:
//  Contains functions which convert device paths to strings.
//
//<INT:AMI_FHDR_END>
//*************************************************************************

#include <Protocol/DevicePath.h>
#include <AmiDxeLib.h>

CHAR8 DefaultDevicePath[]="DevicePath(Type %i, SubType %i)";

CHAR8 *HWDP[] = {
	"PCI(%X|%X)\\",			//HW_PCI_DP
	"Pccard(Socket %i)\\",		//HW_PCCARD_DP
	"VenHw(%g)\\"				//HW_VENDOR_DP
};

CHAR8 *ACPIDP[] = {
	"Acpi(%x, %x)\\"			//ACPI_DP
};

CHAR8 *MSGDP[] = {
	"ATA(%s, %i)\\",			//MSG_ATAPI_DP
	"SCSI(%i, %i)\\",			//MSG_SCSI_DP
	"VenMsg(%g)\\"				//MSG_VENDOR_DP
};

CHAR8 *MEDIADP[] = {
	"HD(Part%i, Sig %s)\\",	//MEDIA_HARDDRIVE_DP
	"CDROM(Entry%i)\\",		//MEDIA_CDROM_DP
	"VenMedia(%g)\\",			//MEDIA_VENDOR_DP
	"%g\\"						//MEDIA_FILEPATH_DP
};

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: HWToStr
//
// Description:
//  UINTN HWToStr(IN EFI_DEVICE_PATH_PROTOCOL *Path, OUT CHAR8 *Str) converts
// the provided hardware device path to a string.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *Path
// The hardware device path which is desired as a string.
//
//  OUT CHAR8 *Str
// ASCII string form of device path.
//
// Output:
//  UINTN number of characters in Str, not including the null-terminator. 
//
// Modified:
//
// Referrals:
//  Sprintf
// 
// Notes:
//  Helper function for DevicePathToStr.
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
UINTN HWToStr(
	EFI_DEVICE_PATH_PROTOCOL *Path,
	CHAR8			*Str
	)
{
	switch(Path->SubType)
	{
	case HW_PCI_DP:
		return Sprintf(Str,HWDP[0],((PCI_DEVICE_PATH*)Path)->Device,((PCI_DEVICE_PATH*)Path)->Function);
	case HW_PCCARD_DP:
		return Sprintf(Str,HWDP[1],((PCCARD_DEVICE_PATH*)Path)->FunctionNumber);    // Build: PCCARD_DEVICE_PATH has field name changed from
	case HW_VENDOR_DP:                                                              // "Function" to "FunctionNumber".
		return Sprintf(Str,HWDP[2],&((VENDOR_DEVICE_PATH*)Path)->Guid);
	}

	return Sprintf(Str,DefaultDevicePath,Path->Type,Path->SubType);
}

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: ACPIToStr
//
// Description:
//  UINTN ACPIToStr(IN EFI_DEVICE_PATH_PROTOCOL *Path, OUT CHAR8 *Str) 
// converts the provided ACPI device path to a string.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *Path
// The ACPI device path which is desired as a string.
//
//  OUT CHAR8 *Str
// ASCII string form of device path.
//
// Output:
//  UINTN number of characters in Str, not including the null-terminator. 
//
// Modified:
//
// Referrals:
//  Sprintf
// 
// Notes:
//  Helper function for DevicePathToStr.
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
UINTN ACPIToStr(
	EFI_DEVICE_PATH_PROTOCOL *Path,
	CHAR8			*Str
	)
{
	switch(Path->SubType)
	{
	case ACPI_DP:
		return Sprintf(Str,ACPIDP[0], ((ACPI_HID_DEVICE_PATH*)Path)->HID, ((ACPI_HID_DEVICE_PATH*)Path)->UID);
	}
	return Sprintf(Str,DefaultDevicePath,Path->Type,Path->SubType);
}

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: MSGToStr
//
// Description:
//  UINTN MSGToStr(IN EFI_DEVICE_PATH_PROTOCOL *Path, OUT CHAR8 *Str)
// converts the message device path to a string.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *Path
// The message device path which is desired as a string.
//
//  OUT CHAR8 *Str
// ASCII string form of device path.
//
// Output:
//  UINTN number of characters in Str, not including the null-terminator. 
//
// Modified:
//
// Referrals:
//  Sprintf
// 
// Notes:
//  Helper function for DevicePathToStr.
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
UINTN MSGToStr(
	EFI_DEVICE_PATH_PROTOCOL *Path,
	CHAR8			*Str
	)
{
	switch(Path->SubType)
	{
	case MSG_ATAPI_DP:
		if (((ATAPI_DEVICE_PATH*)Path)->PrimarySecondary == 0)
			return Sprintf(Str,MSGDP[0], "Master",((ATAPI_DEVICE_PATH*)Path)->Lun);
		else if (((ATAPI_DEVICE_PATH*)Path)->PrimarySecondary == 1)
			return Sprintf(Str,MSGDP[0], L"Slave", ((ATAPI_DEVICE_PATH*)Path)->Lun);
		else break;
	case MSG_SCSI_DP:
		return Sprintf(Str,MSGDP[1], ((SCSI_DEVICE_PATH*)Path)->Pun, ((SCSI_DEVICE_PATH*)Path)->Lun);
	case MSG_VENDOR_DP:
		return Sprintf(Str,MSGDP[2], ((VENDOR_DEVICE_PATH*)Path)->Guid);
	case MSG_USB_DP:
		return Sprintf(Str,"USB(%x,%x)\\",
			((USB_DEVICE_PATH*)Path)->ParentPortNumber,((USB_DEVICE_PATH*)Path)->InterfaceNumber);
		
	}
	return Sprintf(Str,DefaultDevicePath,Path->Type,Path->SubType);
}

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: MEDIAToStr
//
// Description:
//  UINTN MEDIAToStr(IN EFI_DEVICE_PATH_PROTOCOL *Path, OUT CHAR8 *Str)
// converts the media device path to a string.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *Path
// The media device path which is desired as a string.
//
//  OUT CHAR8 *Str
// ASCII string form of device path.
//
// Output:
//  UINTN number of characters in Str, not including the null-terminator.
//
// Modified:
//
// Referrals:
//  Sprintf
// 
// Notes:
//  Helper function for DevicePathToStr.
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
UINTN MEDIAToStr(
	EFI_DEVICE_PATH_PROTOCOL *Path,
	CHAR8			*Str
	)
{
	switch(Path->SubType)
	{
	case MEDIA_HARDDRIVE_DP:
		return Sprintf(Str,MEDIADP[0], ((HARDDRIVE_DEVICE_PATH*)Path)->PartitionNumber, "?");// ((HARDDRIVE_DEVICE_PATH*)Path)->Signature);
	case MEDIA_CDROM_DP:
		return Sprintf(Str,MEDIADP[1], ((CDROM_DEVICE_PATH*)Path)->BootEntry);
	case MEDIA_VENDOR_DP:
		return Sprintf(Str,MEDIADP[2], ((VENDOR_DEVICE_PATH*)Path)->Guid);
	case MEDIA_FILEPATH_DP:
		return Sprintf(Str,MEDIADP[3], &((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)Path)->FvFileName);
	}                                                                   // Build: MEDIA_FW_VOL_FILEPATH_DEVICE_PATH has field name changed
	return Sprintf(Str,DefaultDevicePath,Path->Type,Path->SubType);     // from NameGuid to FvFileName.
}

//*************************************************************************
//<INT:AMI_PHDR_START>
//
// Name: UnknownToStr
//
// Description:
//  UINTN UnknownToStr(IN EFI_DEVICE_PATH_PROTOCOL *Path, OUT CHAR8 *Str)
// converts the device path to a default generic string.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *Path
// The device path which is desired as a string.
//
//  OUT CHAR8 *Str
// ASCII string form of device path.
//
// Output:
//  UINTN number of characters in Str, not including the null-terminator.
//
// Modified:
//
// Referrals:
//  Sprintf
// 
// Notes:
//  Helper function for DevicePathToStr.
//          
//<INT:AMI_PHDR_END>
//*************************************************************************
UINTN UnknownToStr(
	EFI_DEVICE_PATH_PROTOCOL *Path,
	CHAR8			*Str
	)
{
	return Sprintf(Str,DefaultDevicePath,Path->Type,Path->SubType);
}

//*************************************************************************
//<AMI_PHDR_START>
//
// Name: DevicePathToStr
//
// Description:
//  EFI_STATUS DevicePathToStr(IN EFI_DEVICE_PATH_PROTOCOL *Path,
// OUT CHAR8 **Str) convert the provided device path into string.  Str is
// allocated by this routine.  It must be freed by the caller.
//
// Input:
//  IN EFI_DEVICE_PATH_PROTOCOL *Path
// The device path which is desired as a string.
//
//  OUT CHAR8 **Str
// Address to store the pointer for the device path's string.  User is
// responsible for freeing this memory.
//
// Output:
//  EFI_UNSUPPORTED, if string conversion fails.
//  Otherwise, EFI_SUCCESS.
//
// Modified:
//
// Referrals:
//  NEXT_NODE
//  MallocZ
//  MemCpy
// 
// Notes:
//   This function is intended for debug purpose only. 
//   It does not provide support for all variety of device path types.
//          
//<AMI_PHDR_END>
//*************************************************************************
EFI_STATUS DevicePathToStr(
	EFI_DEVICE_PATH_PROTOCOL *Path,
	CHAR8			**Str
	)
{
	CHAR8	Buffer[512];
	CHAR8	*p = Buffer;
	UINTN	Count = 0;
	UINTN	Length;
 
	for(;;)
	{
		switch(Path->Type)
		{
		case HARDWARE_DEVICE_PATH:
			Count = HWToStr(Path, p);
			break;
		case ACPI_DEVICE_PATH:
			Count = ACPIToStr(Path, p);
			break;
		case MESSAGING_DEVICE_PATH:
			Count = MSGToStr(Path, p);
			break;
		case MEDIA_DEVICE_PATH:
			Count = MEDIAToStr(Path, p);
			break;
		case END_DEVICE_PATH:
			Count = 0;
			break;
		default:
			Count = UnknownToStr(Path, p);
		}

		p += Count;

		if (Count == -1) return EFI_UNSUPPORTED;
		if (isEndNode(Path) && Path->SubType == END_ENTIRE_SUBTYPE) break;

		Path = NEXT_NODE(Path);
	}

	Length = (p - Buffer) * sizeof(CHAR8);

	*Str = (CHAR8*)MallocZ(Length+1);

	MemCpy(*Str, Buffer, Length);

	return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
