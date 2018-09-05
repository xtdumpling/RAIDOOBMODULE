//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/AddBootOption.c $
//
// $Author: Arunsb $
//
// $Revision: 17 $
//
// $Date: 1/24/12 4:37a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file AddBootOption.c
    This file contains code to handle the Add boot option operation.

**/

#include "minisetup.h"
#include "Protocol/DevicePathToText.h"

///////////////////////////////////////////////////////
///	MACRO DEFINITIONS
///////////////////////////////////////////////////////

#define MAC_ADDR_SIZE 6

#define	RAM_DISK_VIRTUAL_DISK_GUID  { 0x77AB535A, 0x45FC, 0x624B, {0x55, 0x60, 0xF7, 0xB2, 0x81, 0xD1, 0xF9, 0x6E }}
#define RAM_DISK_VIRTUAL_CD_GUID  { 0x3D5ABD30, 0x4175, 0x87CE, {0x6D, 0x64, 0xD2, 0xAD, 0xE5, 0x23, 0xC4, 0xBB }}
#define RAM_DISK_PERSISTENT_VIRTUAL_DISK_GUID  { 0x5CEA02C9, 0x4D07, 0x69D3, {0x26, 0x9F ,0x44, 0x96, 0xFB, 0xE0, 0x96, 0xF9 }}
#define RAM_DISK_PERSISTENT_VIRTUAL_CD_GUID  { 0x08018188, 0x42CD, 0xBB48, {0x10, 0x0F, 0x53, 0x87, 0xD5, 0x3D, 0xED, 0x3D }}

///////////////////////////////////////////////////////
///	VARIABLE DECLARATIONS
///////////////////////////////////////////////////////

typedef struct
{
    UINT64 Type;
    UINTN Size;
    CHAR16 *Name;
    STRING_REF Token;
} FILE_TYPE;
EFI_FILE_PROTOCOL 			*gFs 							= NULL;
EFI_GUID 						gSimpleFileSystemGuid 	= EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
AMI_POST_MANAGER_PROTOCOL 	*mPostMgr 					= NULL;
static EFI_HANDLE 			*gSmplFileSysHndlBuf 	= NULL;	//Global variable gHandleBuffer changed to gSmplFileSysHndlBuf because of the conflict with HddSecurity module, which already using gHandleBuffer.
BOOLEAN 							gValidOption 				= FALSE;
UINT16 							gSelIdx						= 0;
UINTN 							gDelOptionCount;
BOOT_DATA 						*gDelBootData;
UINTN 							gFsCount;
FS_DATA 							*gFsList;
UINTN 							gDriverDelOptionCount;	
BOOT_DATA 						*gDelDriverData;		
///////////////////////////////////////////////////////
///	EXTERN VARIABLES
///////////////////////////////////////////////////////
extern UINTN gDelBootOptionReserved;
extern UINTN gAddBootOptionEmpty;
extern UINTN gAddDriverOptionEmpty;
///////////////////////////////////////////////////////
///	FUNCTION DECLARATIONS
///////////////////////////////////////////////////////
EFI_DEVICE_PATH_PROTOCOL *_BootBuildFileDevicePath( UINT32 *index, CHAR16 *fileName );
VOID 			BbsStrnCpy ( CHAR16  *Dst, CHAR16  *Src, UINTN   Length ); 
VOID 			_GetFsLists (VOID);
UINT16 		*_DevicePathToStr(EFI_DEVICE_PATH_PROTOCOL *Path);
VOID 			FixHiddenOptions (BOOLEAN, UINT16 **, UINTN);
UINT16 		_BootSetBootNowCount(VOID);
EFI_STATUS ShowPostMsgBox(IN CHAR16  *MsgBoxTitle,IN CHAR16  *Message,IN UINT8  MsgBoxType, UINT8 *pSelection);
EFI_STATUS FileBrowserLaunchFilePath(UINT32 Variable);
extern BOOLEAN IsTseDevicePathToTextProtocolSupport(void);
/**
    Function to update add and delete boot variables

    @param VOID

    @retval VOID

**/
void TseUpdateAddDeleteBootVar(void)
{
    UINT16 u16DelOption;
    EFI_GUID DelBootOptionGuid = DEL_BOOT_OPTION_GUID;
    EFI_GUID AddBootOptionGuid = ADD_BOOT_OPTION_GUID;
    NEW_BOOT_OPTION AddBootOption;

    //Set del boot option var
    u16DelOption = DISABLED_BOOT_OPTION;
    VarSetNvramName( L"DelBootOption", &DelBootOptionGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &u16DelOption, sizeof(UINT16) );

    //Set AddBootOption variable
    _GetFsLists ();
    MemSet(&AddBootOption, sizeof(NEW_BOOT_OPTION),0);
    AddBootOption.FsCount = (UINT16)gFsCount;
    VarSetNvramName( L"AddBootOption", &AddBootOptionGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &AddBootOption, sizeof(NEW_BOOT_OPTION) );

}

/**
    Function to update add and delete driver variables

    @param VOID

    @retval VOID

**/
void TseUpdateAddDeleteDriverVar (void)
{
    UINT16 u16DelOption;
    EFI_GUID DelDriverOptionGuid = DEL_DRIVER_OPTION_GUID;
    EFI_GUID AddDriverOptionGuid = ADD_DRIVER_OPTION_GUID;
    NEW_DRIVER_OPTION AddDriverOption;

    //Set del driver option var
    u16DelOption = DISABLED_DRIVER_OPTION;
    VarSetNvramName (L"DelDriverOption", &DelDriverOptionGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &u16DelOption, sizeof (UINT16));

    //Set AddDriverOption variable
    _GetFsLists();
    MemSet (&AddDriverOption, sizeof (NEW_DRIVER_OPTION), 0);
    AddDriverOption.FsCount = (UINT16)gFsCount;
    VarSetNvramName (L"AddDriverOption", &AddDriverOptionGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &AddDriverOption, sizeof (NEW_DRIVER_OPTION));
}

/**
    function to get the file system details 

    @param VOID

    @retval VOID

**/
VOID _GetFsLists (VOID)
{
    UINTN Count,i;
    EFI_HANDLE *HandleBuffer;
    EFI_DEVICE_PATH_PROTOCOL *DevicePath;
    CHAR16 *Str;
    EFI_STATUS Status;

    //Find all handles supporting Simple File Sys Protocol
	if (0 == gFsCount)			
	{
	    Status = gBS->LocateHandleBuffer(
	            ByProtocol,
	            &gEfiSimpleFileSystemProtocolGuid,
	            NULL,
	            &Count,
	            &HandleBuffer
	            );
	    if(EFI_ERROR(Status))
	        return;
	
	    for(i=0;i<Count;i++)
	    {
	        DevicePath = NULL;
	        Status = gBS->OpenProtocol (
	                    HandleBuffer[i],
	                    &gEfiDevicePathProtocolGuid,
	                    (VOID **) &DevicePath,
	                    NULL,
	                    HandleBuffer[i],
						EFI_OPEN_PROTOCOL_GET_PROTOCOL
	                    );
	        if(EFI_ERROR(Status))
	            continue;
	        if(!DevicePath)
	            continue;
	        Str = NULL;
	        Str = _DevicePathToStr(DevicePath);
	
	        gFsList = MemReallocateZeroPool(gFsList, gFsCount * sizeof(FS_DATA), (gFsCount + 1) * sizeof(FS_DATA));
	        gFsList[gFsCount].FsId = Str ? Str : StrDup(L"Unknown");
	        gFsList[gFsCount].FsPath = DevicePath;
	
	        gFsCount++;
	    }
	}
}

static UINT16 DefaultDevicePath[]=L"DevicePath(Type %x, SubType %x)";

static UINT16 *HWDP[] = {
	L"PCI(%X|%X)\\",			//HW_PCI_DP
	L"Pccard(Socket %x)\\",		//HW_PCCARD_DP
	L"VenHw(%g)\\"				//HW_VENDOR_DP
};

/*Uncomment if ACPI details are necessary
static UINT16 *ACPIDP[] = {
	L"Acpi(%x, %x)\\"			//ACPI_DP
};*/

static UINT16 *MSGDP[] = {
	L"ATA(%s,%s)\\",			//MSG_ATAPI_DP
	L"SCSI(%x,%x)\\",			//MSG_SCSI_DP
	L"VenMsg(%g)\\"				//MSG_VENDOR_DP
};

static UINT16 *MEDIADP[] = {
	L"CDROM(Entry%x)\\",		//MEDIA_CDROM_DP
	L"VenMedia(%g)\\",			//MEDIA_VENDOR_DP
	L"%g\\"						//MEDIA_FILEPATH_DP
};

/**
    Convert Device Path of type to string.

        
    @param Path Device Path
    @param Str String form of device path.

         
    @retval int number of characters not including \0 or -1 if error.

**/
UINTN _HWToStr(
	EFI_DEVICE_PATH_PROTOCOL *Path,
	UINT16			*Str
	)
{
	switch(Path->SubType)
	{
	case HW_PCI_DP:
		return SPrint(Str,0,HWDP[0],((PCI_DEVICE_PATH*)Path)->Device,((PCI_DEVICE_PATH*)Path)->Function);
	case HW_PCCARD_DP:
		return SPrint(Str,0,HWDP[1],((AMITSE_PCCARD_DEVICE_PATH*)Path)->FunctionNumber);
	case HW_VENDOR_DP:
		return SPrint(Str,0,HWDP[2],&((VENDOR_DEVICE_PATH*)Path)->Guid);
	}

	return SPrint(Str,0,DefaultDevicePath,Path->Type,Path->SubType);
}

/**
    Convert Device Path of type to string.

        
    @param Path Device Path
    @param Str String form of device path.

         
    @retval int number of characters not including \0 or -1 if error.

**/
/*
Uncomment below code if ACPI details are necessary.
CAUTION: If ACPI details are included then the string becomes too big
to display
*/
/*
UINTN ACPIToStr(
	EFI_DEVICE_PATH_PROTOCOL *Path,
	UINT16			*Str
	)
{
	switch(Path->SubType)
	{
	case ACPI_DP:
		return SPrint(Str,0,ACPIDP[0], ((ACPI_HID_DEVICE_PATH*)Path)->HID, ((ACPI_HID_DEVICE_PATH*)Path)->UID);
	}
	return SPrint(Str,0,DefaultDevicePath,Path->Type,Path->SubType);
}
*/

/**
    Convert Device Path of type to string.

        
    @param Path Device Path
    @param Str String form of device path.

         
    @retval int number of characters not including \0 or -1 if error.

**/
UINTN _MSGToStr(
	EFI_DEVICE_PATH_PROTOCOL *Path,
	UINT16			*Str
	)
{
	UINT8 *Uuid = NULL;
	UINTN i,Offset = 0;
	
	switch(Path->SubType)
	{
	case MSG_ATAPI_DP:
        return SPrint(Str,0,MSGDP[0],
            ((ATAPI_DEVICE_PATH*)Path)->PrimarySecondary ? L"Sec" : L"Pri",
            ((ATAPI_DEVICE_PATH*)Path)->SlaveMaster ? L"Sl" :L"Ma");
	case MSG_SCSI_DP:
		return SPrint(Str,0,MSGDP[1], ((SCSI_DEVICE_PATH*)Path)->Pun, ((SCSI_DEVICE_PATH*)Path)->Lun);
	case MSG_VENDOR_DP:
		return SPrint(Str,0,MSGDP[2], ((VENDOR_DEVICE_PATH*)Path)->Guid);
	case MSG_USB_DP:
		return SPrint(Str,0,L"USB(%x,%x)\\",
			((USB_DEVICE_PATH*)Path)->ParentPortNumber,((USB_DEVICE_PATH*)Path)->InterfaceNumber);
	case MSG_I2O_DP:
		return SPrint(Str,0,L"I2O(%x)\\",((I2O_DEVICE_PATH*)Path)->Tid);
	case MSG_USB_CLASS_DP:
		return SPrint(Str,0,L"USB_CLASS(%X,%X,%x,%X,%X)\\",((USB_CLASS_DEVICE_PATH*)Path)->VendorId,((USB_CLASS_DEVICE_PATH*)Path)->ProductId,((USB_CLASS_DEVICE_PATH*)Path)->DeviceClass,((USB_CLASS_DEVICE_PATH*)Path)->DeviceSubClass,((USB_CLASS_DEVICE_PATH*)Path)->DeviceProtocol);
	case MSG_USB_WWID_DP:
		return SPrint(Str,0,L"USB_WWID(%X,%X,%x)\\",((USB_WWID_DEVICE_PATH*)Path)->VendorId,((USB_WWID_DEVICE_PATH*)Path)->ProductId,((USB_WWID_DEVICE_PATH*)Path)->InterfaceNumber);
	case MSG_DEVICE_LOGICAL_UNIT_DP:
		return SPrint(Str,0,L"LOGICAL_UNIT(%x)\\",((DEVICE_LOGICAL_UNIT_DEVICE_PATH*)Path)->Lun);
	case MSG_SATA_DP:
		return SPrint(Str,0,L"SATA(%x,%X,%x)\\",((SATA_DEVICE_PATH*)Path)->HBAPortNumber,((SATA_DEVICE_PATH*)Path)->PortMultiplierPortNumber,((SATA_DEVICE_PATH*)Path)->Lun);
	case MSG_MAC_ADDR_DP:
		SPrint(Str,0,L"MAC(");
		Offset = EfiStrLen(L"MAC(");
		for(i=0;i<MAC_ADDR_SIZE;i++)
		{
			SPrint(Str + Offset,0,L"%02X",((MAC_ADDR_DEVICE_PATH*)Path)->MacAddress.Addr[i]);
			Offset += 2;
		}
		return SPrint(Str + Offset,0,L",%x)\\",((MAC_ADDR_DEVICE_PATH*)Path)->IfType);
	case MSG_IPv4_DP:
		return SPrint(Str,0,L"IPv4(%d.%d.%d.%d)\\",((IPv4_DEVICE_PATH*)Path)->LocalIpAddress.Addr[0],((IPv4_DEVICE_PATH*)Path)->LocalIpAddress.Addr[1],((IPv4_DEVICE_PATH*)Path)->LocalIpAddress.Addr[2],((IPv4_DEVICE_PATH*)Path)->LocalIpAddress.Addr[3]);
	case MSG_IPv6_DP:
			return SPrint(Str,0,L"Ipv6(%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x)\\",
					((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[0],((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[1],
					((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[2],((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[3],
					((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[4],((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[5],
					((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[6],((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[7],
					((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[8],((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[9],
					((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[10],((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[11],
					((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[12],((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[13],
					((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[14],((IPv6_DEVICE_PATH*)Path)->RemoteIpAddress.Addr[15]
					);
	case MSG_ISCSI_DP:
		return SPrint(Str,0,L"iSCSI(%x,%lx,%x,%s)\\",((ISCSI_DEVICE_PATH*)Path)->TargetPortalGroupTag,((ISCSI_DEVICE_PATH*)Path)->Lun,((ISCSI_DEVICE_PATH*)Path)->LoginOption,(((ISCSI_DEVICE_PATH*)Path)->NetworkProtocol == 0 ? L"TCP" : L"reserved"));
	case MSG_SASEX_DP:
		return SPrint(Str,0,L"SASEX(%x,%x,%x,%x)\\",((SASEX_DEVICE_PATH*)Path)->SasAddress,((SASEX_DEVICE_PATH*)Path)->Lun,((SASEX_DEVICE_PATH*)Path)->RelativeTargetPort,((SASEX_DEVICE_PATH*)Path)->DeviceTopology);
	case MSG_NVME_NAMESPACE_DP:
		Uuid = (UINT8 *)&((NVME_NAMESPACE_DEVICE_PATH*)Path)->NamespaceUuid ;
		return SPrint(Str,0,L"NVMe(0x%x,%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x)\\",((NVME_NAMESPACE_DEVICE_PATH*)Path)->NamespaceId,Uuid[7], Uuid[6], Uuid[5], Uuid[4],Uuid[3], Uuid[2], Uuid[1], Uuid[0]);
	case MSG_UFS_DP:
		return SPrint(Str,0,L"UFS(%x,%x)\\",((UFS_DEVICE_PATH*)Path)->Pun,((UFS_DEVICE_PATH*)Path)->Lun);
	case MSG_SD_DP:
		return SPrint(Str,0,L"SD(0x%x)\\",((SD_DEVICE_PATH*)Path)->SlotNumber);
	
	}
	return SPrint(Str,0,DefaultDevicePath,Path->Type,Path->SubType);
}

/**
    Convert Device Path of type to string.

        
    @param Path Device Path
    @param Str String form of device path.

         
    @retval int number of characters not including \0 or -1 if error.

**/
UINTN _MEDIAToStr(
	EFI_DEVICE_PATH_PROTOCOL *Path,
	UINT16			*Str
	)
{
	EFI_GUID EfiVirtualDiskGuid = RAM_DISK_VIRTUAL_DISK_GUID;
	EFI_GUID EfiVirtualCdGuid = RAM_DISK_VIRTUAL_CD_GUID;
	EFI_GUID EfiPersistentVirtualDiskGuid = RAM_DISK_PERSISTENT_VIRTUAL_DISK_GUID;
	EFI_GUID EfiPersistentVirtualCdGuid = RAM_DISK_PERSISTENT_VIRTUAL_CD_GUID;
	
	switch(Path->SubType)
	{
	case MEDIA_HARDDRIVE_DP:
        {
            HARDDRIVE_DEVICE_PATH *Hd = (HARDDRIVE_DEVICE_PATH *)Path;
            switch(Hd->SignatureType)
            {
            case SIGNATURE_TYPE_MBR:
                return SPrint(Str,0,L"HD(Part%d,Sig%08X)", Hd->PartitionNumber, *((UINT32 *)(&(Hd->Signature[0]))));
            case SIGNATURE_TYPE_GUID:
                return SPrint(Str,0, L"HD(Part%d,Sig%g)", Hd->PartitionNumber, (EFI_GUID *) &(Hd->Signature[0]));
            default:
                return SPrint(Str,0,L"HD(Part%d,MBRType=%02x,SigType=%02x)", 
                Hd->PartitionNumber,
                Hd->MBRType,
                Hd->SignatureType
                );
            }
        }
	case MEDIA_CDROM_DP:
		return SPrint(Str,0,MEDIADP[0], ((CDROM_DEVICE_PATH*)Path)->BootEntry);
	case MEDIA_VENDOR_DP:
		return SPrint(Str,0,MEDIADP[1], ((VENDOR_DEVICE_PATH*)Path)->Guid);
	case MEDIA_FILEPATH_DP:
		return SPrint(Str,0,MEDIADP[2], &((AMITSE_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)Path)->FvFileName);
	case MEDIA_RAM_DISK_DP:
		if (EfiCompareGuid (&((MEDIA_RAM_DISK_DEVICE_PATH*)Path)->TypeGuid, &EfiVirtualDiskGuid))
			return SPrint(Str,0,L"RAM_DISK(VirtualDisk,%d)\\", ((MEDIA_RAM_DISK_DEVICE_PATH*)Path)->Instance);
		else if (EfiCompareGuid (&((MEDIA_RAM_DISK_DEVICE_PATH*)Path)->TypeGuid, &EfiVirtualCdGuid))
			return SPrint(Str,0,L"RAM_DISK(VirtualCD,%d)\\", ((MEDIA_RAM_DISK_DEVICE_PATH*)Path)->Instance);
		else if (EfiCompareGuid (&((MEDIA_RAM_DISK_DEVICE_PATH*)Path)->TypeGuid, &EfiPersistentVirtualDiskGuid))	
			return SPrint(Str,0,L"RAM_DISK(PersistentVirtualDisk,%d)\\", ((MEDIA_RAM_DISK_DEVICE_PATH*)Path)->Instance);
		else if (EfiCompareGuid (&((MEDIA_RAM_DISK_DEVICE_PATH*)Path)->TypeGuid, &EfiPersistentVirtualCdGuid))
			return SPrint(Str,0,L"RAM_DISK(PersistentVirtualCD,%d)\\", ((MEDIA_RAM_DISK_DEVICE_PATH*)Path)->Instance);
		else
			return SPrint(Str,0,L"RAM_DISK(%g,%d)\\", ((MEDIA_RAM_DISK_DEVICE_PATH*)Path)->TypeGuid,((MEDIA_RAM_DISK_DEVICE_PATH*)Path)->Instance);			
	}
	return SPrint(Str,0,DefaultDevicePath,Path->Type,Path->SubType);
}

/**
    Convert Device Path of type to string.

        
    @param Path Device Path
    @param Str String form of device path.

         
    @retval int number of characters not including \0 or -1 if error.

**/
UINTN _UnknownToStr(
	EFI_DEVICE_PATH_PROTOCOL *Path,
	UINT16			*Str
	)
{
	return SPrint(Str,0,DefaultDevicePath,Path->Type,Path->SubType);
}


/**
    Convert Device Path of type to string. Str is allocated by
    this routine. It must be freed by the caller.

        
    @param Path Device Path
	

    @retval 
        OUT UINT16			*Str  - String form of device path.

**/
UINT16 *_DevicePathToStr(EFI_DEVICE_PATH_PROTOCOL *Path)
{
	CHAR16	Buffer[512];
	CHAR16	*p = Buffer, *Str = NULL;
	UINTN	Count = 0;
	UINTN	Length;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToTextProtocol = (EFI_DEVICE_PATH_TO_TEXT_PROTOCOL*)NULL;
    EFI_GUID DevPathToTextGuid = EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID;
    EFI_STATUS Status = EFI_UNSUPPORTED;
    
    MemSet(Buffer,sizeof(Buffer),0);
    
    if(IsTseDevicePathToTextProtocolSupport())
    {
    	Status = gBS->LocateProtocol(&DevPathToTextGuid,NULL,&DevPathToTextProtocol);
        
    	if(!EFI_ERROR(Status) && DevPathToTextProtocol)
    	{
    		return DevPathToTextProtocol->ConvertDevicePathToText(Path,TRUE,TRUE);
    	}
    }
    
	for(;;)
	{
		switch(Path->Type)
		{
		case HARDWARE_DEVICE_PATH:
			Count = _HWToStr(Path, p);
			break;
		case ACPI_DEVICE_PATH:
/*
Uncomment below code if ACPI details are necessary.
CAUTION: If ACPI details are included then the string becomes too big
to display
*/
/*
            Count = SPrint(p,0,L"Acpi\\");
            ACPIToStr(Path, p);
*/
			break;
		case MESSAGING_DEVICE_PATH:
			Count = _MSGToStr(Path, p);
			break;
		case MEDIA_DEVICE_PATH:
			Count = _MEDIAToStr(Path, p);
			break;
		case END_DEVICE_PATH_TYPE:
			Count = 0;
			break;
		case END_ENTIRE_DEVICE_PATH_SUBTYPE:
			*p = L'|';
			*(p+1) = L'\0';
			Count = 1;
			break;
		default:
			Count = _UnknownToStr(Path, p);
		}

		p += EfiStrLen(p);

		if (Count == -1) return NULL;
		if (IsDevicePathEnd(Path)) break;

		Path = NextDevicePathNode(Path);
	}

	Length = (UINTN)p - (UINTN)Buffer;

	Str = (UINT16 *)EfiLibAllocateZeroPool(Length+sizeof(UINT16));

	MemCopy(Str, Buffer, Length);

	if(DevPathToTextProtocol)
		MemFreePointer(&DevPathToTextProtocol);
	
	return Str;
}

/**
    Function to make the boot/driver option as disable in bootorder/driverorder
    if it is inactive and non hidden


    @param BOOLEAN , UINT16 **, UINTN 

    @retval VOID

**/
VOID _DisableRestorePrevOptions (BOOLEAN Option, UINT16 **Order, UINTN OptionCount)
{
	UINTN iIndex = 0;
	BOOT_DATA   *pBootData;

	if (0 == OptionCount)
	{
		return;
	}
	for (iIndex = 0; iIndex < OptionCount; iIndex ++)
	{
		if (BOOT_ORDER_OPTION == Option)
		{
			pBootData = BootGetBootData ((*Order) [iIndex]);
		}
		else
		{
			pBootData = DriverGetDriverData ((*Order) [iIndex]);
		}
		if (pBootData)
		{
			if ( (!(pBootData->Active & LOAD_OPTION_ACTIVE)) && (!(pBootData->Active & LOAD_OPTION_HIDDEN)) )
			{
					(*Order) [iIndex] = DISABLED_BOOT_OPTION;
			}
		}
	}
}

/**
    Function to add boot option 

    @param VOID

    @retval VOID

**/
VOID BootAddBootOption( VOID )
{
    UINTN i,size;
    UINT16 u16Option;
    NEW_BOOT_OPTION *newBoot = NULL, *tmpPtr;
    CHAR16 *fileName;
    UINT32 index;
    EFI_DEVICE_PATH_PROTOCOL *devicePath;
    UINT16 *NewBootOrder = NULL, *CurrBootOrder = NULL;
    UINT16 BootCountVal;
    CHAR16 *BootNameBuf = NULL;		
	EFI_STATUS Status;
    //Get input
    size = 0;
    Status = gBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL,(void**) &mPostMgr);
    if(Status != EFI_SUCCESS)
  	    return;
    newBoot = VarGetVariable( VARIABLE_ID_ADD_BOOT_OPTION, &size );
	if (NULL == newBoot){
        return;
    }

	//not to create boot option with out a name
    if (( EfiStrLen( newBoot->Path ) == 0 ) || ( EfiStrLen(newBoot->Label) == 0 ))
    {
	//	CallbackShowMessageBox( (UINTN)gAddBootOptionEmpty, MSGBOX_TYPE_OK );
		
    	 mPostMgr->DisplayMsgBox(  L"WARNING",  L"Please set Boot Option Name and File Path", MSGBOX_TYPE_OK,NULL);
		return;
    }
    BootNameBuf = EfiLibAllocateZeroPool( sizeof(newBoot->Label)+2 );  
    BbsStrnCpy( BootNameBuf, newBoot->Label, (sizeof(newBoot->Label))/2 );

    // cleanup old data
    tmpPtr = EfiLibAllocateZeroPool( size );
    tmpPtr->FsCount = (UINT16)gFsCount;
    VarSetValue( VARIABLE_ID_ADD_BOOT_OPTION, 0, size, tmpPtr );
    MemFreePointer( (VOID **)&tmpPtr );

    fileName = newBoot->Path;

    index = newBoot->SelFs;

    devicePath = _BootBuildFileDevicePath( &index, fileName );
    if ( devicePath != NULL )
    {
        EFI_DEVICE_PATH_PROTOCOL *TmpDevPath, *HddMediaPath;

        //Find a free option number
        for ( u16Option = 0; u16Option < MAX_BOOT_OPTIONS; u16Option++ )
        {
            for(i=0;i<gBootOptionCount;i++)
            {
                if(gBootData[i].Option == u16Option)
                    break;
            }
            if(i<gBootOptionCount)
                continue;
            for(i=0;i<gDelOptionCount;i++)
            {
                 if(gDelBootData[i].Option == u16Option)
                    break;
            }
            if(i<gDelOptionCount)
                continue;
            
            break;//The desired option number is in u16Option
        }
    
        if ( u16Option == MAX_BOOT_OPTIONS ) //Not possible
            return;

        gBootData = MemReallocateZeroPool(gBootData, gBootOptionCount * sizeof(BOOT_DATA), (gBootOptionCount + 1) * sizeof(BOOT_DATA) );

        gBootData[gBootOptionCount].Option = u16Option;
        gBootData[gBootOptionCount].Active |= LOAD_OPTION_ACTIVE;
			//if the boot option name is set with max number of characters allowed. 
        //gBootData[gBootOptionCount].Name = StrDup( newBoot->Label );
        gBootData[gBootOptionCount].Name = BootNameBuf;

        //For a hard drive start the dev path from the partition
        for( TmpDevPath = devicePath; 
             !IsDevicePathEndType(TmpDevPath);
             TmpDevPath=NextDevicePathNode(TmpDevPath)
        )
        {
            if(TmpDevPath->Type==MEDIA_DEVICE_PATH && TmpDevPath->SubType==MEDIA_HARDDRIVE_DP)
            {
                HddMediaPath = EfiLibAllocateZeroPool(EfiDevicePathSize(TmpDevPath));
                MemCopy(HddMediaPath, TmpDevPath, EfiDevicePathSize(TmpDevPath));
                MemFreePointer((VOID **)&devicePath);
                devicePath = HddMediaPath;
                break;
            }
        }
        gBootData[gBootOptionCount].DevicePath = devicePath;
        gBootData[gBootOptionCount].bNewOption = TRUE;

        size = 0;
        CurrBootOrder = VarGetVariable(VARIABLE_ID_BOOT_ORDER, &size);
/*        if (NULL == CurrBootOrder){		//If file system available and no boot option presents then add boot option always fails so commenting it.
            return;
        }*/
        NewBootOrder = EfiLibAllocateZeroPool((gBootOptionCount+1) * sizeof(UINT16));

        for(i=0;i<gBootOptionCount;i++)
        {
            if(DISABLED_BOOT_OPTION == CurrBootOrder[i])
                break;
            NewBootOrder[i] = CurrBootOrder[i];
        }
		if (i < gBootOptionCount)		//Preserve the hidden options at last
		{
			MemCpy (NewBootOrder+i+1, CurrBootOrder+i, (gBootOptionCount-i)*2);
		}
        NewBootOrder [i] = u16Option;
        //i ++;
        MemFreePointer ((VOID **) &CurrBootOrder);
        gBootOptionCount ++;
        MemFreePointer ((VOID **)&newBoot);

        //Update boot manager vars in memory
        //1. TSE Boot order
        MemFreePointer((VOID **) &gVariableList[VARIABLE_ID_BOOT_ORDER].Buffer);
		if (gLoadOptionHidden)				//Move hidden options to last
		{
			FixHiddenOptions (BOOT_ORDER_OPTION, &NewBootOrder, gBootOptionCount);
		}
        gVariableList[VARIABLE_ID_BOOT_ORDER].Buffer = (UINT8 *)NewBootOrder;
        gVariableList[VARIABLE_ID_BOOT_ORDER].Size = gBootOptionCount * sizeof(UINT16);

        //2. BootManager
        BootCountVal = (UINT16)gBootOptionCount;
        VarSetValue(VARIABLE_ID_BOOT_MANAGER, 0, sizeof(UINT16), (VOID *)(&BootCountVal));

        //3. Set Boot now count
		if(gShowAllBbsDev)
	        BootCountVal = _BootSetBootNowCount();

        Status = VarSetValue(VARIABLE_ID_BOOT_NOW, 0, sizeof(UINT16), (VOID *)(&BootCountVal));
        if(Status == EFI_SUCCESS)
        	mPostMgr->DisplayMsgBox(  L"SUCCESS",  L"Boot Option Created Successfully", MSGBOX_TYPE_OK,NULL);
    }
}

/**
    Function to add driver options

    @param VOID

    @retval VOID

**/
VOID DriverAddDriverOption(VOID)
{
    UINTN i,size;
    UINT16 u16Option;
    NEW_DRIVER_OPTION *newDriver = NULL, *tmpPtr;
    CHAR16 *fileName;
    UINT32 index;
    EFI_DEVICE_PATH_PROTOCOL *devicePath;
    UINT16 *NewDriverOrder = NULL, *CurrDriverOrder = NULL;
    UINT16 DriverCountVal;
    CHAR16 *BootNameBuf = NULL;
	EFI_STATUS Status;
	 //Get input
    size = 0;
    Status = gBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL,(void**) &mPostMgr);
    if(Status != EFI_SUCCESS)
	    return;
    newDriver = VarGetVariable( VARIABLE_ID_ADD_DRIVER_OPTION, &size );
	if (( EfiStrLen (newDriver->DriverPath) == 0 ) || (EfiStrLen (newDriver->Label) == 0))
    {
		//CallbackShowMessageBox( (UINTN)gAddDriverOptionEmpty, MSGBOX_TYPE_OK );
		
		 mPostMgr->DisplayMsgBox(  L"WARNING",  L"Please set Driver Option Name and File Path", MSGBOX_TYPE_OK,NULL);
		return;
    }

	BootNameBuf = EfiLibAllocateZeroPool( sizeof(newDriver->Label)+2 );  
    BbsStrnCpy( BootNameBuf, newDriver->Label, (sizeof(newDriver->Label))/2 );	
	
	    // cleanup old data
    tmpPtr = EfiLibAllocateZeroPool( size );
    tmpPtr->FsCount = (UINT16)gFsCount;
    VarSetValue( VARIABLE_ID_ADD_DRIVER_OPTION, 0, size, tmpPtr );
    MemFreePointer( (VOID **)&tmpPtr );

    fileName = newDriver->DriverPath;

    index = newDriver->SelFs;

    devicePath = _BootBuildFileDevicePath( &index, fileName );
	
	if ( devicePath != NULL )
    {
//        EFI_DEVICE_PATH_PROTOCOL *TmpDevPath, *HddMediaPath;

        //Find a free option number
        for ( u16Option = 0; u16Option < MAX_BOOT_OPTIONS; u16Option++ )
        {
         for(i=0;i<gDriverOptionCount;i++)
            {
                if(gDriverData[i].Option == u16Option)
                    break;
            }
            if(i<gDriverOptionCount)
                continue;
		 for(i=0;i<gDriverDelOptionCount;i++)
            {
                 if(gDelDriverData[i].Option == u16Option)
                    break;
            }
            if(i<gDriverDelOptionCount)
                continue;
            
            break;//The desired option number is in u16Option
        }

        if ( u16Option == MAX_BOOT_OPTIONS ) //Not possible
            return;

        gDriverData = MemReallocateZeroPool(gDriverData, gDriverOptionCount * sizeof(BOOT_DATA), (gDriverOptionCount + 1) * sizeof(BOOT_DATA) );

        gDriverData[gDriverOptionCount].Option = u16Option;
        gDriverData[gDriverOptionCount].Active |= LOAD_OPTION_ACTIVE;
			//if the boot option name is set with max number of characters allowed. 
        //gBootData[gBootOptionCount].Name = StrDup( newDriver->Label );
        gDriverData[gDriverOptionCount].Name = BootNameBuf;

        /*
		//TSE will append hard drive device path at front if it is MEDIA_DEVICE_PATH but core needs fullpath so commented below
		//For a hard drive start the dev path from the partition
        for( TmpDevPath = devicePath; 						
             !IsDevicePathEndType(TmpDevPath);
             TmpDevPath=NextDevicePathNode(TmpDevPath)
        )
        {
            if(TmpDevPath->Type==MEDIA_DEVICE_PATH && TmpDevPath->SubType==MEDIA_HARDDRIVE_DP)
            {
                HddMediaPath = EfiLibAllocateZeroPool(EfiDevicePathSize(TmpDevPath));
                MemCopy(HddMediaPath, TmpDevPath, EfiDevicePathSize(TmpDevPath));
                MemFreePointer((VOID **)&devicePath);
                devicePath = HddMediaPath;
                break;
            }
        }*/

		gDriverData[gDriverOptionCount].DevicePath = devicePath;
        gDriverData[gDriverOptionCount].bNewOption = TRUE;

        size = 0;
        CurrDriverOrder = VarGetVariable(VARIABLE_ID_DRIVER_ORDER, &size);
        NewDriverOrder = EfiLibAllocateZeroPool((gDriverOptionCount+1) * sizeof(UINT16));

        for(i=0;i<gDriverOptionCount;i++)
        {
            if(DISABLED_DRIVER_OPTION == CurrDriverOrder[i])
                break;
            NewDriverOrder[i] = CurrDriverOrder[i];
        }
		if (i < gDriverOptionCount)		//Preserve the hidden options at last
		{
			MemCpy (NewDriverOrder+i+1, CurrDriverOrder+i, (gDriverOptionCount-i)*2);
		}

        NewDriverOrder[i] = u16Option;
        //i++;

        MemFreePointer((VOID **) &CurrDriverOrder);

        gDriverOptionCount ++;

        MemFreePointer( (VOID **)&newDriver );

        //Update boot manager vars in memory
        //1. TSE driver order
        MemFreePointer((VOID **) &gVariableList[VARIABLE_ID_DRIVER_ORDER].Buffer);
		if (gLoadOptionHidden)			//Move the hidden options to last
		{
			FixHiddenOptions (DRIVER_ORDER_OPTION, &NewDriverOrder, gDriverOptionCount);
		}
        gVariableList[VARIABLE_ID_DRIVER_ORDER].Buffer = (UINT8 *)NewDriverOrder;
        gVariableList[VARIABLE_ID_DRIVER_ORDER].Size = gDriverOptionCount * sizeof(UINT16);

        //2. DriverManager
        DriverCountVal = (UINT16)gDriverOptionCount;
        Status = VarSetValue(VARIABLE_ID_DRIVER_MANAGER, 0, sizeof(UINT16), (VOID *)(&DriverCountVal));
        if(Status == EFI_SUCCESS)
                mPostMgr->DisplayMsgBox(  L"SUCCESS",  L"Driver Option Created Successfully", MSGBOX_TYPE_OK,NULL);
	}
}

/**
    Function to handle the del boot option 

    @param index 

    @retval VOID

**/
VOID BootDelBootOption( UINT16 index )
{
    UINTN i,j,k, size;
    BOOT_DATA *pBootData = NULL;
    UINT16 BootCountVal;
    UINT16 *NewBootOrder = NULL, *CurrBootOrder = NULL;
    EFI_STATUS Status=EFI_UNSUPPORTED;
    VOID * UnicodeInterface;
    CHAR16 *ResStr = NULL;
    CHAR16 *TmpStr = NULL;
	
	// If it BBS device we should not delete it.
	// FIX......to check for the correct Boot option.
    Status = gBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL,(void**) &mPostMgr);
      if(Status != EFI_SUCCESS)
  	    return;
	for(i=0; i<gBootOptionCount; i++)
    {
    	if(index == gBootData[i].Option)
		{
 		   if(gBootData[i].DevicePath->Type == BBS_DEVICE_PATH && gBootData[i].DevicePath->SubType == BBS_BBS_DP)
				return;

			if( ( IsReservedBootOptionNamesEnable() ) && 
			    ( gBootData[i].Name ) ) 
			{ 
				ResStr = CONVERT_TO_WSTRING(RESERVED_ADD_DEL_BOOT_OPTION_NAME);
				TmpStr = EfiLibAllocateZeroPool( EfiStrLen(ResStr)+1 ); 
				BbsStrnCpy(TmpStr, gBootData[i].Name, (EfiStrLen(ResStr)+1) );
				TmpStr[EfiStrLen(ResStr)]= L'\0';
				Status = InitUnicodeCollectionProtocol(&UnicodeInterface);
				if(!EFI_ERROR(Status)) {
					if( StringColl( UnicodeInterface, TmpStr, ResStr) == 0 )
					{
						//CallbackShowMessageBox( (UINTN)gDelBootOptionReserved, MSGBOX_TYPE_OK );
						 mPostMgr->DisplayMsgBox(  L"WARNING",  L"Reserved Boot Option, can not be Deleted", MSGBOX_TYPE_OK,NULL);
						return;
					}
				}
			}
		}
	}

    //Remove option from gBootData and add to gDelBootData
    //Remove option from BootOrder
    if(gBootOptionCount - 1)
    {
        pBootData = EfiLibAllocateZeroPool(sizeof(BOOT_DATA) * (gBootOptionCount - 1));
        NewBootOrder = EfiLibAllocateZeroPool(sizeof(UINT16) * (gBootOptionCount - 1));
    }

    size = 0;
    CurrBootOrder = VarGetVariable(VARIABLE_ID_BOOT_ORDER, &size);
    if (NULL == CurrBootOrder){
    	if(pBootData)
    		MemFreePointer((VOID **)&pBootData);
    	if(NewBootOrder)
    		MemFreePointer((VOID **)&NewBootOrder);
        return;
    }

    gDelBootData = MemReallocateZeroPool(gDelBootData, sizeof(BOOT_DATA) * gDelOptionCount, sizeof(BOOT_DATA) * (gDelOptionCount + 1));

    for(i=0,j=0,k=0; i<gBootOptionCount; i++)
    {
        if(index == gBootData[i].Option)
        {
            MemCopy(gDelBootData + gDelOptionCount, gBootData + i, sizeof(BOOT_DATA));
        }
        else
        {
            MemCopy(pBootData + j, gBootData + i, sizeof(BOOT_DATA));
            j++;
        }

        if(index != CurrBootOrder[i])
        {
            if(k<(gBootOptionCount-1))
            {
                NewBootOrder[k] = CurrBootOrder[i];
                k++;
            }
        }
    }

    gDelOptionCount++;
    gBootOptionCount--;

    MemFreePointer((VOID **)&gBootData);
    gBootData = pBootData;

    MemFreePointer((VOID **)&CurrBootOrder);

    //Update boot manager vars in memory
    //1. TSE Boot order
    MemFreePointer((VOID **) &gVariableList[VARIABLE_ID_BOOT_ORDER].Buffer);
	if (gLoadOptionHidden)			//Move the hidden options to last
	{
		FixHiddenOptions (BOOT_ORDER_OPTION, &NewBootOrder, gBootOptionCount);
	}
    gVariableList[VARIABLE_ID_BOOT_ORDER].Buffer = (UINT8 *)NewBootOrder;
    gVariableList[VARIABLE_ID_BOOT_ORDER].Size = gBootOptionCount * sizeof(UINT16);

    //2. BootManager
    BootCountVal = (UINT16)gBootOptionCount;
    VarSetValue(VARIABLE_ID_BOOT_MANAGER,0,sizeof(UINT16),(VOID *)(&BootCountVal));

    //3. Set Boot now count
	if(gShowAllBbsDev)
	    BootCountVal = _BootSetBootNowCount();

    Status = VarSetValue(VARIABLE_ID_BOOT_NOW,0,sizeof(UINT16),(VOID *)(&BootCountVal));
    if(EFI_SUCCESS == Status)
	    mPostMgr->DisplayMsgBox(  L"SUCCESS",  L"Boot Option Deleted Successfully", MSGBOX_TYPE_OK,NULL);
}

/**
    Function to handle the del Driver option 

    @param index 

    @retval VOID

**/
VOID DriverDelDriverOption( UINT16 index )
{
	UINTN i,j,k, size;
    BOOT_DATA *pDriverData = NULL;
    UINT16 DriverCountVal;
    UINT16 *NewDriverOrder = NULL, *CurrDriverOrder = NULL;
    EFI_STATUS Status=EFI_UNSUPPORTED; 
//    VOID * UnicodeInterface;
//    CHAR16 *ResStr = NULL; unused
 //   CHAR16 *TmpStr = NULL; unused
	
	if ((NULL == gDriverData) ||  (0 == gDriverOptionCount))
		return;
	Status = gBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL,(void**) &mPostMgr);
	      if(Status != EFI_SUCCESS)
	  	    return;
	for(i=0; i<gDriverOptionCount; i++)
    {
    	if(index == gDriverData[i].Option)
		{
 		   if(gDriverData[i].DevicePath->Type == BBS_DEVICE_PATH && gDriverData[i].DevicePath->SubType == BBS_BBS_DP)
				return;
			break;
		}
	}

    //Remove option from gDriverData and add to gDelDriverData
    //Remove option from DriverOrder
    if(gDriverOptionCount - 1)
    {
        pDriverData = EfiLibAllocateZeroPool(sizeof(BOOT_DATA) * (gDriverOptionCount - 1));
        NewDriverOrder = EfiLibAllocateZeroPool(sizeof(UINT16) * (gDriverOptionCount - 1));
    }

    size = 0;
    CurrDriverOrder = VarGetVariable(VARIABLE_ID_DRIVER_ORDER, &size);

    gDelDriverData = MemReallocateZeroPool(gDelDriverData, sizeof(BOOT_DATA) * gDriverDelOptionCount, sizeof(BOOT_DATA) * (gDriverDelOptionCount + 1));

    for(i=0,j=0,k=0; i<gDriverOptionCount; i++)
    {
        if(index == gDriverData[i].Option)
        {
            MemCopy(gDelDriverData + gDriverDelOptionCount, gDriverData + i, sizeof(BOOT_DATA));
        }
        else
        {
            MemCopy(pDriverData + j, gDriverData + i, sizeof(BOOT_DATA));
            j++;
        }

        if(index != CurrDriverOrder[i])
        {
            if(k<(gDriverOptionCount-1))
            {
                NewDriverOrder[k] = CurrDriverOrder[i];
                k++;
            }
        }
    }

    gDriverDelOptionCount++;
    gDriverOptionCount--;

    MemFreePointer((VOID **)&gDriverData);
    gDriverData = pDriverData;

    MemFreePointer((VOID **)&CurrDriverOrder);

    //Update boot manager vars in memory
    //1. TSE Driver Order
    MemFreePointer((VOID **) &gVariableList[VARIABLE_ID_DRIVER_ORDER].Buffer);
	if (gLoadOptionHidden)
	{
		FixHiddenOptions (DRIVER_ORDER_OPTION, &NewDriverOrder, gDriverOptionCount);
	}
    gVariableList[VARIABLE_ID_DRIVER_ORDER].Buffer = (UINT8 *)NewDriverOrder;
    gVariableList[VARIABLE_ID_DRIVER_ORDER].Size = gDriverOptionCount * sizeof(UINT16);

    //2. DriverManager
    DriverCountVal = (UINT16)gDriverOptionCount;
    Status =  VarSetValue(VARIABLE_ID_DRIVER_MANAGER,0,sizeof(UINT16),(VOID *)(&DriverCountVal));
    if(EFI_SUCCESS == Status)
	    mPostMgr->DisplayMsgBox(  L"SUCCESS",  L"Driver Deleted Successfully", MSGBOX_TYPE_OK,NULL);
}

/**
    Function to discarding the add/del boot option

    @param VOID

    @retval VOID

**/
VOID DiscardAddDelBootOptions(VOID)
{
    UINTN count,i,j;
    BOOT_DATA *pBootData;
	UINT16 *AddOptionList=NULL,*DelOptionList=NULL;
	UINTN AddOptionListCount=0, DelOptionListCount=0;
    UINT16 *NewBootOrder = NULL, *CurrBootOrder = NULL;
	UINTN   size = 0, tempsize = 0;

    //find no. of boot options
    count = 0;
    for(i=0;i<gBootOptionCount;i++)
    {
        if(!gBootData[i].bNewOption)
            count++;
        else
            AddOptionListCount++;
    }
    for(i=0;i<gDelOptionCount;i++)
    {
        if(!gDelBootData[i].bNewOption)
            count++;
        else
            DelOptionListCount++;
    }

    //Allocate for boot options
    pBootData = NULL;
    if(count)
        pBootData = EfiLibAllocateZeroPool(count * sizeof(BOOT_DATA));
    if(AddOptionListCount)
        AddOptionList=EfiLibAllocateZeroPool(AddOptionListCount * sizeof(UINT16));
    if(DelOptionListCount)
        DelOptionList=EfiLibAllocateZeroPool(DelOptionListCount * sizeof(UINT16));

    count = 0;
    AddOptionListCount=0;
    DelOptionListCount=0;

    //Copy old options
    for(i=0;i<gBootOptionCount;i++)
    {
        if(!gBootData[i].bNewOption)
        {
            MemCopy(pBootData+count, gBootData+i, sizeof(BOOT_DATA));
            count++;
        }
		else
		{ 	// Newly added option that need to removed.
			AddOptionList[AddOptionListCount++] = gBootData[i].Option;
		}
    }

    for(i=0;i<gDelOptionCount;i++)
    {
        if(!gDelBootData[i].bNewOption)
        {
            MemCopy(pBootData+count, gDelBootData+i, sizeof(BOOT_DATA));
            count++;

			// deleted option that need to added.
			DelOptionList[DelOptionListCount++] = gDelBootData[i].Option;
        }
    }

    MemFreePointer((VOID **) &gBootData);
    gBootData = pBootData;
    gBootOptionCount = count;

    MemFreePointer((VOID **) &gDelBootData);
    gDelOptionCount = 0;

// Update BootOrderVar Cache after discarding the changes.
// If newly add/Del option discorded.
	if(AddOptionListCount || DelOptionListCount)
	{
        CurrBootOrder = VarGetVariable(VARIABLE_ID_BOOT_ORDER, &size);
        NewBootOrder = EfiLibAllocateZeroPool((gBootOptionCount) * sizeof(UINT16));
		count = size/2;

		// Defaults might be load Move the disabled boot option to the end
        for(j=0;j<count;j++)
		{
			if(DISABLED_BOOT_OPTION == CurrBootOrder[j])
			{
				if (size < ((j+1)*sizeof(UINT16))) 
					tempsize = ((j+1)*sizeof(UINT16)) - size;
				else
					tempsize = size - ((j+1)*sizeof(UINT16));

				MemCopy(&CurrBootOrder[j], &CurrBootOrder[j+1], tempsize);
				size-=sizeof(UINT16);
			}
		}

		// remove the Newly added boot options
		count = size/2;
        for(i=0;i<AddOptionListCount;i++)
		{
	        for(j=0;j<count;j++)
			{
				if(CurrBootOrder[j] == AddOptionList[i])
				{					
					if (size < ((j+1)*sizeof(UINT16))) 
						tempsize = ((j+1)*sizeof(UINT16)) - size;
					else
						tempsize = size - ((j+1)*sizeof(UINT16));

					MemCopy(&CurrBootOrder[j], &CurrBootOrder[j+1], tempsize);
					size-=sizeof(UINT16);
				}
			}
		}
		count = size/2;
		// Add the DelOptions
		MemCopy(NewBootOrder, CurrBootOrder, size);
		MemCopy(&NewBootOrder[count],DelOptionList,DelOptionListCount*sizeof(UINT16) );
		
		// Make the remailing options as disabled.
		for(i=count+DelOptionListCount;i<gBootOptionCount;i++)
            NewBootOrder[i] = DISABLED_BOOT_OPTION;

        MemFreePointer((VOID **) &CurrBootOrder);
        MemFreePointer((VOID **) &DelOptionList);
        MemFreePointer((VOID **) &AddOptionList);

        //Update boot manager vars in memory
        //1. TSE Boot order
        MemFreePointer((VOID **) &gVariableList[VARIABLE_ID_BOOT_ORDER].Buffer);
		if (gLoadOptionHidden)
		{
			FixHiddenOptions (BOOT_ORDER_OPTION, &NewBootOrder, gBootOptionCount);
		}
		_DisableRestorePrevOptions (BOOT_ORDER_OPTION, &NewBootOrder, gBootOptionCount);			//Disable options after discarding changes to active so fixed it
        gVariableList[VARIABLE_ID_BOOT_ORDER].Buffer = (UINT8 *)NewBootOrder;
        gVariableList[VARIABLE_ID_BOOT_ORDER].Size = gBootOptionCount * sizeof(UINT16);
	}
}

/**
    Function to discarding the add/del driver option

    @param VOID

    @retval VOID

**/
VOID DiscardAddDelDriverOptions(VOID)
{
	UINTN count,i,j;
    BOOT_DATA *pDriverData;
	UINT16 *AddOptionList=NULL,*DelOptionList=NULL;
	UINTN AddOptionListCount=0, DelOptionListCount=0;
    UINT16 *NewDriverOrder = NULL, *CurrDriverOrder = NULL;
	UINTN   size = 0, tempsize = 0;

    //find no. of boot options
    count = 0;
    for(i=0;i<gDriverOptionCount;i++)
    {
        if(!gDriverData[i].bNewOption)
            count++;
        else
            AddOptionListCount++;
    }
    for(i=0;i<gDriverDelOptionCount;i++)
    {
        if(!gDelDriverData[i].bNewOption)
            count++;
        else
            DelOptionListCount++;
    }

    //Allocate for boot options
    pDriverData = NULL;
    if(count)
        pDriverData = EfiLibAllocateZeroPool(count * sizeof(BOOT_DATA));
    if(AddOptionListCount)
        AddOptionList=EfiLibAllocateZeroPool(AddOptionListCount * sizeof(UINT16));
    if(DelOptionListCount)
        DelOptionList=EfiLibAllocateZeroPool(DelOptionListCount * sizeof(UINT16));

    count = 0;
    AddOptionListCount=0;
    DelOptionListCount=0;

    //Copy old options
    for(i=0;i<gDriverOptionCount;i++)
    {
        if(!gDriverData[i].bNewOption)
        {
            MemCopy(pDriverData+count, gDriverData+i, sizeof(BOOT_DATA));
            count++;
        }
		else
		{ 	// Newly added option that need to removed.
			AddOptionList[AddOptionListCount++] = gDriverData[i].Option;
		}
    }

    for(i=0;i<gDriverDelOptionCount;i++)
    {
        if(!gDelDriverData[i].bNewOption)
        {
            MemCopy(pDriverData+count, gDelDriverData+i, sizeof(BOOT_DATA));
            count++;

			// deleted option that need to added.
			DelOptionList[DelOptionListCount++] = gDelDriverData[i].Option;
        }
    }

    MemFreePointer((VOID **) &gDriverData);
    gDriverData = pDriverData;
    gDriverOptionCount = count;

    MemFreePointer((VOID **) &gDelDriverData);
    gDriverDelOptionCount = 0;

// Update DriverOrderVar Cache after discarding the changes.
// If newly add/Del option discorded.
	if(AddOptionListCount || DelOptionListCount)
	{
        CurrDriverOrder = VarGetVariable(VARIABLE_ID_DRIVER_ORDER,(UINTN*) &size);
        NewDriverOrder = EfiLibAllocateZeroPool((gDriverOptionCount) * sizeof(UINT16));
		count = size/2;

		// Defaults might be load Move the disabled driver option to the end
        for(j=0;j<count;j++)
		{
			if(DISABLED_BOOT_OPTION == CurrDriverOrder[j])
			{
				if (size > ((j+1)*sizeof(UINT16))) 
					tempsize = size - ((j+1)*sizeof(UINT16));
				else
					tempsize = ((j+1)*sizeof(UINT16)) - size;

				MemCopy(&CurrDriverOrder[j], &CurrDriverOrder[j+1], tempsize );
				size-=sizeof(UINT16);
			}
		}

		// remove the Newly added driver options
		count = size/2;
        for(i=0;i<AddOptionListCount;i++)
		{
	        for(j=0;j<count;j++)
			{
				if(CurrDriverOrder[j] == AddOptionList[i])
				{
					if ((UINTN)size >= ((j+1)*sizeof (UINT16)) )		//Check added bcoz, If only added driver presents and loading defaults will lead to crash
					{
						if (size > ((j+1)*sizeof(UINT16))) 
							tempsize = size - ((j+1)*sizeof(UINT16));
						else
							tempsize = ((j+1)*sizeof(UINT16)) - size;

						MemCopy (&CurrDriverOrder [j], &CurrDriverOrder[j+1], tempsize );
						size-=sizeof(UINT16);
					}
				}
			}
		}

		count = size/2;
		// Add the DelOptions
		MemCopy(NewDriverOrder, CurrDriverOrder, size);
		MemCopy(&NewDriverOrder[count],DelOptionList,DelOptionListCount*sizeof(UINT16) );
		
		// Make the remaining options as disabled.
		for(i=count+DelOptionListCount;i<gDriverOptionCount;i++)
            NewDriverOrder[i] = DISABLED_BOOT_OPTION;

        MemFreePointer((VOID **) &CurrDriverOrder);
        MemFreePointer((VOID **) &DelOptionList);
        MemFreePointer((VOID **) &AddOptionList);

        //Update driver manager vars in memory
        //1. TSE Driver Order
        MemFreePointer((VOID **) &gVariableList[VARIABLE_ID_DRIVER_ORDER].Buffer);
		if (gLoadOptionHidden)
		{
			FixHiddenOptions (DRIVER_ORDER_OPTION, &NewDriverOrder, gDriverOptionCount);
		}
		_DisableRestorePrevOptions (DRIVER_ORDER_OPTION, &NewDriverOrder, gDriverOptionCount);
        gVariableList[VARIABLE_ID_DRIVER_ORDER].Buffer = (UINT8 *)NewDriverOrder;
        gVariableList[VARIABLE_ID_DRIVER_ORDER].Size = gDriverOptionCount * sizeof(UINT16);
	}
}

/**
    function to save the added/deleted boot option 

    @param VOID

    @retval VOID

**/
VOID SaveAddDelBootOptions(VOID)
{
	UINTN i,OptimalIndex,FailsafeIndex,size;
	UINT16 dpLength,*pNewOptimal, *pOldOptimal, *pNewFailsafe, *pOldFailsafe;
	BOOT_OPTION *pOption;
	CHAR16 varName [20] = L"BootXXXX";
	UINT16 BootCountVal;
	
	//Old Boot order defaults are not valid any more. Form the new
	//defaults by removing deleted options and adding the new options at the end.
	pNewOptimal = EfiLibAllocateZeroPool( gBootOptionCount * sizeof(UINT16) );
	pNewFailsafe = EfiLibAllocateZeroPool( gBootOptionCount * sizeof(UINT16) );
	pOldOptimal = (UINT16 *)gOptimalDefaults[ VARIABLE_ID_BOOT_ORDER ].Buffer;
	pOldFailsafe = (UINT16 *)gFailsafeDefaults[ VARIABLE_ID_BOOT_ORDER ].Buffer;
	size = gOptimalDefaults[ VARIABLE_ID_BOOT_ORDER ].Size;
	OptimalIndex = FailsafeIndex = 0;
	
	//Add existing options first
	for(i=0;i<size/sizeof(UINT16);i++)
	{
		if(BootGetBootData(pOldOptimal[i]))
		{
			pNewOptimal[OptimalIndex] = pOldOptimal[i];
			OptimalIndex++;
		}
		if(BootGetBootData(pOldFailsafe[i]))
		{
			pNewFailsafe[FailsafeIndex] = pOldFailsafe[i];
			FailsafeIndex++;
		}
	}
	//Save New boot options
	for(i=0;i<gBootOptionCount;i++)
	{
		if(gBootData[i].bNewOption)
		{
			dpLength = (UINT16)EfiDevicePathSize( gBootData[i].DevicePath );
			size = sizeof(BOOT_OPTION) + sizeof(CHAR16)*EfiStrLen(gBootData[i].Name) + dpLength;
			pOption = EfiLibAllocateZeroPool( size );
			pOption->Active = gBootData[i].Active;
			pOption->PathLength = dpLength;
			EfiStrCpy( pOption->Name, gBootData[i].Name );
			MemCopy( (UINT8 *)pOption + size - dpLength, gBootData[i].DevicePath, dpLength );
	
			SPrint( varName, sizeof(CHAR16) * (EfiStrLen( varName )+1), gBootFormarSpecifier, gBootData[i].Option );
			VarSetNvramName( varName,
				&gEfiGlobalVariableGuid,
				EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
				pOption,
				size );
	
			gBootData[i].bNewOption = 0;
	
			//Append this option in defaults
			pNewOptimal[OptimalIndex] = gBootData[i].Option;
			pNewFailsafe[FailsafeIndex] = gBootData[i].Option;
			OptimalIndex++;
			FailsafeIndex++;
		}
	}
	
	//Put new defaults in global default repository
   //If Default order is not found in first time then will take current order.
   //If any boot options added after that then new boot option always follow the first saved optimized buffer
   //All the time changing the default buffer is not good
	if (0 == gOptimalDefaults [VARIABLE_ID_BOOT_ORDER].Size)
	{
		UINT8 *pOptWithOutDefaultBootOrder = EfiLibAllocateZeroPool (gVariableList [VARIABLE_ID_BOOT_ORDER].Size);
		if (NULL != pOptWithOutDefaultBootOrder)
		{
			MemCopy (pOptWithOutDefaultBootOrder, gVariableList [VARIABLE_ID_BOOT_ORDER].Buffer, gVariableList [VARIABLE_ID_BOOT_ORDER].Size);
			gOptimalDefaults [VARIABLE_ID_BOOT_ORDER].Buffer = pOptWithOutDefaultBootOrder;
			gOptimalDefaults [VARIABLE_ID_BOOT_ORDER].Size = gVariableList [VARIABLE_ID_BOOT_ORDER].Size;
		}
	}
	else
	{		 	 
		MemFreePointer( (VOID **) &gOptimalDefaults[ VARIABLE_ID_BOOT_ORDER ].Buffer );
		gOptimalDefaults[ VARIABLE_ID_BOOT_ORDER ].Buffer = (UINT8 *)pNewOptimal;
		gOptimalDefaults[ VARIABLE_ID_BOOT_ORDER ].Size = OptimalIndex * sizeof(UINT16);
	}

	MemFreePointer( (VOID **) &gFailsafeDefaults[ VARIABLE_ID_BOOT_ORDER ].Buffer );
	gFailsafeDefaults[ VARIABLE_ID_BOOT_ORDER ].Buffer = (UINT8 *)pNewFailsafe;
	gFailsafeDefaults[ VARIABLE_ID_BOOT_ORDER ].Size = FailsafeIndex * sizeof(UINT16);
	
	//Delete removed options
	for(i=0;i<gDelOptionCount;i++)
	{
		if(!gDelBootData[i].bNewOption)
		{
			SPrint( varName, sizeof(CHAR16) * (EfiStrLen( varName )+1), gBootFormarSpecifier, gDelBootData[i].Option );
			VarSetNvramName( varName,
				&gEfiGlobalVariableGuid,
				EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
				varName,
				0);
		}
	}
	MemFreePointer((VOID **)&gDelBootData);
	gDelOptionCount = 0;
	
	//Update default value for Boot Manager and Boot Now count
	BootCountVal = (UINT16) gBootOptionCount;
	_VarGetSetValue (VAR_COMMAND_SET_VALUE, gOptimalDefaults, VARIABLE_ID_BOOT_MANAGER, 0, sizeof(UINT16), (VOID *)(&BootCountVal));
	_VarGetSetValue (VAR_COMMAND_SET_VALUE, gFailsafeDefaults, VARIABLE_ID_BOOT_MANAGER, 0, sizeof(UINT16), (VOID *)(&BootCountVal));
	
	if (gShowAllBbsDev)
		BootCountVal = _BootSetBootNowCount ();
	
	_VarGetSetValue (VAR_COMMAND_SET_VALUE, gOptimalDefaults, VARIABLE_ID_BOOT_NOW, 0, sizeof(UINT16), (VOID *)(&BootCountVal));
	_VarGetSetValue (VAR_COMMAND_SET_VALUE, gFailsafeDefaults, VARIABLE_ID_BOOT_NOW, 0, sizeof(UINT16), (VOID *)(&BootCountVal));
}

/**
    Function to save the added/deleted Driver option 

    @param VOID

    @retval VOID

**/
VOID SaveAddDelDriverOptions(VOID)
{
	UINTN i,OptimalIndex,FailsafeIndex,size;
	UINT16 dpLength,*pNewOptimal, *pOldOptimal, *pNewFailsafe, *pOldFailsafe;
	BOOT_OPTION *pOption;
	CHAR16 varName [20]= L"DriverXXXX";
	UINT16 DriverCountVal;
	
	//Old Driver order defaults are not valid any more. Form the new
	//defaults by removing deleted options and adding the new options at the end.
	pNewOptimal = EfiLibAllocateZeroPool( gDriverOptionCount * sizeof(UINT16) );
	pNewFailsafe = EfiLibAllocateZeroPool( gDriverOptionCount * sizeof(UINT16) );
	pOldOptimal = (UINT16 *)gOptimalDefaults[ VARIABLE_ID_DRIVER_ORDER ].Buffer;
	pOldFailsafe = (UINT16 *)gFailsafeDefaults[ VARIABLE_ID_DRIVER_ORDER ].Buffer;
	size = gOptimalDefaults[ VARIABLE_ID_DRIVER_ORDER ].Size;
	OptimalIndex = FailsafeIndex = 0;
	
	//Add existing options first
	for(i=0;i<size/sizeof(UINT16);i++)
	{
		if (DriverGetDriverData (pOldOptimal [i]))
		{
			pNewOptimal[OptimalIndex] = pOldOptimal[i];
			OptimalIndex++;
		}
		if(DriverGetDriverData (pOldFailsafe[i]))
		{
			pNewFailsafe[FailsafeIndex] = pOldFailsafe[i];
			FailsafeIndex++;
		}
	}
	//Save New driver options
	for(i=0;i<gDriverOptionCount;i++)
	{
		if(gDriverData[i].bNewOption)
		{
			dpLength = (UINT16)EfiDevicePathSize( gDriverData[i].DevicePath );
			size = sizeof(BOOT_OPTION) + sizeof(CHAR16)*EfiStrLen(gDriverData[i].Name) + dpLength;
			pOption = EfiLibAllocateZeroPool( size );
			pOption->Active = gDriverData[i].Active;
			pOption->PathLength = dpLength;
			EfiStrCpy( pOption->Name, gDriverData[i].Name );
			MemCopy( (UINT8 *)pOption + size - dpLength, gDriverData[i].DevicePath, dpLength );
	
			SPrint( varName, sizeof(CHAR16) * (EfiStrLen( varName )+1), gDriverFormarSpecifier, gDriverData[i].Option );
				VarSetNvramName( varName,
				&gEfiGlobalVariableGuid,
				EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
				pOption,
				size );
	
			gDriverData[i].bNewOption = 0;
			//Append this option in defaults
			pNewOptimal[OptimalIndex] = gDriverData[i].Option;
			pNewFailsafe[FailsafeIndex] = gDriverData[i].Option;
			OptimalIndex++;
			FailsafeIndex++;
		}
	}
	
	//Put new defaults in global default repository
   //Default order is not found in first time then will take current order.
   //If any driver options added after that then new driver option always follow the first saved optimized buffer
   //All the time changing the default buffer is not good
	if (0 == gOptimalDefaults [VARIABLE_ID_DRIVER_ORDER].Size)
	{
		UINT8 *pOptWithOutDefaultDriverOrder = EfiLibAllocateZeroPool (gVariableList [VARIABLE_ID_DRIVER_ORDER].Size);
		if (NULL != pOptWithOutDefaultDriverOrder)
		{
			MemCopy (pOptWithOutDefaultDriverOrder, gVariableList [VARIABLE_ID_DRIVER_ORDER].Buffer, gVariableList [VARIABLE_ID_DRIVER_ORDER].Size);
			gOptimalDefaults [VARIABLE_ID_DRIVER_ORDER].Buffer = pOptWithOutDefaultDriverOrder;
			gOptimalDefaults [VARIABLE_ID_DRIVER_ORDER].Size = gVariableList [VARIABLE_ID_DRIVER_ORDER].Size;
		}
	}
	else
	{
		MemFreePointer( (VOID **) &gOptimalDefaults[ VARIABLE_ID_DRIVER_ORDER ].Buffer );			
		gOptimalDefaults[ VARIABLE_ID_DRIVER_ORDER ].Buffer = (UINT8 *)pNewOptimal;
		gOptimalDefaults[ VARIABLE_ID_DRIVER_ORDER ].Size = OptimalIndex * sizeof(UINT16);
	}
	MemFreePointer( (VOID **) &gFailsafeDefaults[ VARIABLE_ID_DRIVER_ORDER ].Buffer );
	gFailsafeDefaults[ VARIABLE_ID_DRIVER_ORDER ].Buffer = (UINT8 *)pNewFailsafe;
	gFailsafeDefaults[ VARIABLE_ID_DRIVER_ORDER ].Size = FailsafeIndex * sizeof(UINT16);
	
	//Delete removed options
	for(i=0;i<gDriverDelOptionCount;i++)
	{
		if(!gDelDriverData[i].bNewOption)
		{
			SPrint( varName, sizeof(CHAR16) * (EfiStrLen( varName )+1), gDriverFormarSpecifier, gDelDriverData[i].Option );
			VarSetNvramName (varName,
				&gEfiGlobalVariableGuid,
				EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
				varName,
				0);
		}
	}
	MemFreePointer((VOID **)&gDelDriverData);
	gDriverDelOptionCount = 0;
	
	//Update default value for Driver Manager
	DriverCountVal = (UINT16) gDriverOptionCount;
	_VarGetSetValue( VAR_COMMAND_SET_VALUE, gOptimalDefaults, VARIABLE_ID_DRIVER_MANAGER, 0, sizeof(UINT16), (VOID *)(&DriverCountVal) );
	_VarGetSetValue( VAR_COMMAND_SET_VALUE, gFailsafeDefaults, VARIABLE_ID_DRIVER_MANAGER, 0, sizeof(UINT16), (VOID *)(&DriverCountVal) );
}

/**
    Function to fix Add Boot Option file list

    @param ControlData 

    @retval VOID

**/
VOID TSELiteFixAddBootOptionFileList(CONTROL_DATA *ControlData)
{
	POPUPSEL_DATA *popupSel = (POPUPSEL_DATA *)ControlData;
	UINT16 i=0;
	if(gFsCount >= popupSel->ItemCount) // If the filesystem found
	{
		for(i=0;i<popupSel->ItemCount;i++)
		{
			popupSel->PtrTokens[i].Option = HiiAddString( popupSel->ControlData.ControlHandle, gFsList[i].FsId );
			popupSel->PtrTokens[i].Value = i;
		}
	}
}

/**
    Function to fix Delete Boot option file list

    @param ControlData 

    @retval VOID

**/
VOID TSELiteFixDelBootOptionFileList(CONTROL_DATA *ControlData)
{
	POPUPSEL_DATA *popupSel = (POPUPSEL_DATA *)ControlData;
	UINT16 i=0;

	for(i=0;i<popupSel->ItemCount;i++)
	{
		if((UINT16)popupSel->PtrTokens[i].Value != 0xFFFF)
		{
			popupSel->PtrTokens[i].Value = gBootData[i-1].Option;
			popupSel->PtrTokens[i].Option = HiiAddString( popupSel->ControlData.ControlHandle, BootGetOptionName( &(gBootData[i-1]) ) );
		}
	}
}

/**
    Frees all allocated memory associated with the FILE_TYPE structure
    and resets the InternalString current strings next available string token
    to be the first dynamically added string

    @param FileList The array of FILE_TYPE structures found in 
        a directory
    @param FileCount pointer to the number of entries in the FileList

    @retval 

    @retval 

**/
VOID CleanFileTypes(FILE_TYPE **FileList, UINTN *FileCount)
{
    UINTN i;
    for(i = 0; i<*FileCount; i++) gBS->FreePool((*FileList)[i].Name);
    if(FileList!=NULL && (*FileList!=NULL) && (*FileCount>0)) gBS->FreePool(*FileList);
    if(FileList!=NULL) *FileList = NULL;
    *FileCount = 0;
}
/**
    Checks if the EFI_FILE_INFO is a directory (and not the current directory)

    @param File 

           

    @retval BOOLEAN TRUE - item is a directory, and not the current directory
    @retval FALSE item is not a directory, or is the current directory

**/
BOOLEAN CheckDirectoryType(EFI_FILE_INFO *File)
{
    BOOLEAN Status = FALSE;

    if((File->Attribute & EFI_FILE_DIRECTORY) && (EfiStrCmp(File->FileName, L".") != 0)) {

        Status = TRUE;
    }

    return Status;
}

/**
    Check is the EFI_FILE_INFO has the same extension as the 
    extension passed in the second parameter

    @param File The file entry whose extension should be checked
    @param ExtensionEfi the extension

         

    @retval BOOLEAN TRUE - The extension matches
    @retval FALSE the extension does not match

**/
BOOLEAN CheckExtension(EFI_FILE_INFO *File, CHAR16 *ExtensionEfi)
{
    BOOLEAN Status = FALSE;
    UINTN Length = EfiStrLen(File->FileName);

    if((File->Attribute & EFI_FILE_DIRECTORY) != EFI_FILE_DIRECTORY && Length > 3)
        if((((File->FileName[Length-1])&0xdf) == ((ExtensionEfi[2])&0xdf)) &&
           (((File->FileName[Length-2])&0xdf) == ((ExtensionEfi[1])&0xdf)) &&
           (((File->FileName[Length-3])&0xdf) == ((ExtensionEfi[0])&0xdf)))
            Status = TRUE;
    return Status;    
}

/**
    Finds the inded where directories items turn into file items

    @param List the current array of File Type structures
    @param FileCount the count of File Type structures in the array

    @retval 

    @retval the index to insert a new item

**/
UINTN FindInsertionIndex(FILE_TYPE *List, UINTN FileCount)
{
    UINTN i = 0;
    
    if(FileCount <= 1) return 0;

    for(i = 1; i < (FileCount-1); i++)
    {
        if(List[i-1].Type != List[i].Type)
        break;
    }

    return i;
}

/**
    Creates a new entry in the FILE_TYPE array and adds the current File into
    the array.

    @param List Array of FILE_TYPE structures alread found
    @param FileCount number of entries in the FILE_TYPE array
    @param FileInfo file info of the file that should be added

    @retval 

    @retval 

**/
static VOID AddFileTypeEntry(FILE_TYPE **List, UINTN *FileCount, EFI_FILE_INFO *FileInfo)
{
    FILE_TYPE *NewList=NULL;
    UINTN Length;
    UINTN Index = 0;

    Length = (EfiStrLen(FileInfo->FileName)+3)*sizeof(CHAR16);

    // Allocate space for a new list entry plus all the previous list items
  //  NewList = EfiLibAllocateCopyPool(sizeof(FILE_TYPE)*(++(*FileCount)), NewList);
    NewList = EfiLibAllocateZeroPool(sizeof(FILE_TYPE)*(++(*FileCount)));
    if (NewList != NULL) 
    {
        // Clear the memory of the entire list
        MemSet(NewList, sizeof(FILE_TYPE)*(*FileCount), 0);
    
        // Copy the old entries (if there are any old entries to copy)
        if(*List != NULL) 
        {
            Index = FindInsertionIndex(*List, *FileCount);

            MemCopy(NewList, *List, sizeof(FILE_TYPE)*(Index));
            MemCopy(&(NewList[Index+1]), &((*List)[Index]), sizeof(FILE_TYPE)*((*FileCount)-Index-1));

            gBS->FreePool(*List);
        }

        // Store the type of this FILE_TYPE entry (non-zero is directory)
        NewList[Index].Type = ((FileInfo->Attribute) & EFI_FILE_DIRECTORY);

        // Store the size of the file
        NewList[Index].Size = (UINTN)FileInfo->FileSize;

        // Allocate space for the string
       // NewList[Index].Name = EfiLibAllocateCopyPool(Length, NewList[Index].Name);
        NewList[Index].Name = EfiLibAllocateZeroPool (Length);
        if((NewList[Index].Name) != NULL )
        {
            // Clear the allocated memory
            MemSet(NewList[Index].Name, Length, 0);

            // Create either a Dir string or a File string for addition to the HiiDataBase
            if(NewList[Index].Type == EFI_FILE_DIRECTORY)
                 SPrint(NewList[Index].Name, (sizeof(CHAR16)*EfiStrLen(FileInfo->FileName)+1), L"<%s>", FileInfo->FileName);
            else
                 SPrint(NewList[Index].Name, (sizeof(CHAR16)*EfiStrLen(FileInfo->FileName)+1), L"%s", FileInfo->FileName);

            // Add the string to the HiiDataBase
            ///NewList[Index].Token = AddStringToHii(FileInfo->FileName, &gInternalStrings);	///Just by trying using the following line
            NewList[Index].Token = 	HiiAddString(gHiiHandle, NewList[Index].Name );

            // Clear the memory and create the string for the File Structure
            MemSet(NewList[Index].Name, Length, 0);
             SPrint(NewList[Index].Name, (sizeof(CHAR16)*EfiStrLen(FileInfo->FileName)+1), L"%s", FileInfo->FileName);            
        }
        *List = NewList;
    }
}

/**
    Parse all the files in the current directory and add valid files to the
    FILE_TYPE list and update the filecount

    @param FileProtocol the current direcotry to parse

    @retval FILE_TYPE **FileList - pointer in which to return the array of FileType items
        UINTN *FileCount - the count of filetype items discovered

    @retval EFI_STATUS

**/
 EFI_STATUS CreateFileList(EFI_FILE_PROTOCOL *FileProtocol, FILE_TYPE **FileList, UINTN *FileCount)
{
    EFI_STATUS Status = EFI_SUCCESS;

    UINTN   BufferSize = 1;
    EFI_FILE_INFO *File = NULL;

    CHAR16 ExtensionEfi[] = L"EFI";

    // Continue parsing the directory until we no longer read valid files
    while(BufferSize != 0 && !EFI_ERROR(Status))
    {
        BufferSize = 0;
        Status = FileProtocol->Read(FileProtocol, &BufferSize, NULL);

        if(!EFI_ERROR(Status)) break;

        if(Status == EFI_BUFFER_TOO_SMALL)
        {
            File = EfiLibAllocateZeroPool(BufferSize);
            if(File != NULL) {
					MemSet(File, BufferSize, 0);
				}
        }

        Status = FileProtocol->Read(FileProtocol, &BufferSize, File);

        // Check if a valid file was read
        if(!EFI_ERROR(Status) && BufferSize != 0)
        {
            // check if the file read was a directory or a ".efi" extension
            if(CheckDirectoryType(File) || CheckExtension(File, ExtensionEfi))
            {
                // the file was valid, add it to the file list
                AddFileTypeEntry(FileList, FileCount, File);
            }
        }

        // free the space allocated for readin the file info structure
        gBS->FreePool(File);

        // set the pointer to null to prevent the chance of memory corruption
        File = NULL;
    }

    if(*FileCount == 0) Status = EFI_NOT_FOUND;

    return Status;
}

/**
    Display a menu of the FILE_TYPE items and return the selected item
    in the Selection

    @param FileList List of FILE_TYPE items to display in the menu
    @param FileCount the number of FILE_TYPE items in the list

    @retval UINT16 *Selection - The index of the selected FILE_TYPE item

    @retval 

**/
static EFI_STATUS DisplayFileListMenu(FILE_TYPE *FileList, UINTN FileCount, UINT16 *Selection, UINT32 Variable)
{
    EFI_STATUS Status = EFI_SUCCESS;

    UINT16 i = 0;

    POSTMENU_TEMPLATE *List = NULL;

    // Check there are any files to display
    if(FileCount != 0 && FileList != NULL)
    {
        // allocate space for the POSTMENU_TEMPLATE items
        List = EfiLibAllocateZeroPool (sizeof(POSTMENU_TEMPLATE)*FileCount);
        if(List != NULL)
        {
            // Clear the memory allocated 
            MemSet(List, sizeof(POSTMENU_TEMPLATE)*FileCount, 0);

            // Add the STRING_REF tokens to the POSTMENU_TEMPLATE structures
            for(i = 0; i < FileCount; i++)
                List[i].ItemToken = FileList[i].Token;
        }

        // Call post manager to display the menu
        Status = mPostMgr->DisplayPostMenu(gHiiHandle,
                                            (VARIABLE_ID_ADD_BOOT_OPTION == Variable) ? STRING_TOKEN (STR_FILE_PATH) : STRING_TOKEN (STR_DRIVER_PATH),
                                            0,
                                            List,
                                            (UINT16)FileCount,
                                            Selection);
    }
    return Status;
}

/**
    To create the File Path string based on the file selected.

    @param FilePath Buffer to fill with the file path
    @param CurFile current file selected
    @param idx Index of the file in the current directory

    @retval CHAR16 *FilePath - Updated File Path

    @retval 

**/
VOID UpdateFilePathString(CHAR16 *FilePath, CHAR16 * CurFile, UINT16 idx)
{
	UINTN Length=0;

	if(EfiStrLen(FilePath))
	{
		if( idx==0 ) {
			if(EfiStrCmp(CurFile,L".."))  {
				EfiStrCat(FilePath,L"\\");
				EfiStrCat(FilePath,CurFile);
			}
			else {
				
				for ( Length = EfiStrLen(FilePath); ( Length!= 0 ) && (FilePath[Length-1] != L'\\') ; Length -- ); 
					if ( Length )
						FilePath[Length-1] = L'\0';
					else
						FilePath[Length] = L'\0';	
			}
		}
		else {
			EfiStrCat(FilePath,L"\\");
			EfiStrCat(FilePath,CurFile);
		}
	}
	else {
		EfiStrCpy(FilePath,L"\\");
		EfiStrCat(FilePath,CurFile);
		//EfiStrCpy(FilePath,CurFile);
	}
}

/**
    To select the File System for the new boot option with the help of file browser.

    @param VOID

    @retval Selected File System Index

    @retval EFI_STATUS

**/
EFI_STATUS FileBrowserLaunchFileSystem(UINT32 Variable)
{
    EFI_STATUS Status;
    UINTN Count = 0;
    UINT16 i = 0;

    EFI_GUID DevicePathGuid = EFI_DEVICE_PATH_PROTOCOL_GUID;
    EFI_DEVICE_PATH_PROTOCOL *Dp = NULL;

    POSTMENU_TEMPLATE *PossibleFileSystems = NULL;

    Status = gBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL,(void**) &mPostMgr);
    if(EFI_ERROR(Status)) {
	return EFI_UNSUPPORTED;
    }
    // Locate all the simple file system devices in the system
    Status = gBS->LocateHandleBuffer(ByProtocol, &gSimpleFileSystemGuid, NULL, &Count, &gSmplFileSysHndlBuf);
    if(!EFI_ERROR(Status))
    {
        // allocate space to display all the simple file system devices
        PossibleFileSystems = EfiLibAllocateZeroPool (sizeof(POSTMENU_TEMPLATE)*Count);
        if(PossibleFileSystems != NULL)
        {
            // clear the allocated space
            MemSet(PossibleFileSystems, sizeof(POSTMENU_TEMPLATE)*Count, 0);
            for(i = 0; i < Count; i++)
            {
                // get the device path for each handle with a simple file system
                Status = gBS->HandleProtocol(gSmplFileSysHndlBuf[i], &DevicePathGuid,(void**) &Dp);
                if(!EFI_ERROR(Status))
                {
                    CHAR16 *Name = NULL;

                    // Get the name of the driver installed on the handle
                    // GetControllerName(gHandleBuffer[i],&Name);

                    Name = NULL;
                    Name = _DevicePathToStr(Dp);

                    // Add the name to the Hii Database
                    ///PossibleFileSystems[i].ItemToken = AddStringToHii(Name);
                    PossibleFileSystems[i].ItemToken = HiiAddString(gHiiHandle, Name ); 

                    PossibleFileSystems[i].Attribute = AMI_POSTMENU_ATTRIB_FOCUS;
                }
                else
                {
                    PossibleFileSystems[i].ItemToken = 0;
                    PossibleFileSystems[i].Attribute = AMI_POSTMENU_ATTRIB_HIDDEN;
                }
            }
            // Reset the item selected to be the first item
            gSelIdx = 0;

            // Display the post menu and wait for user input
            Status = mPostMgr->DisplayPostMenu(gHiiHandle,
                                                (Variable != 0xffff)? STRING_TOKEN(STR_FILE_SYSTEM) : STRING_TOKEN(STR_FILE_SYSTEM_TO_SAVE_IMG),
                                                0,
                                                PossibleFileSystems,
                                                (UINT16)Count,
                                                &gSelIdx);


            // A valid item was selected by the user
            if(!EFI_ERROR(Status))
            {
                    gValidOption = TRUE;
            }
        }
    }
	
    else {
        ShowPostMsgBox( L"No Valid File System", L"No Valid File System Available", MSGBOX_TYPE_OK, NULL );

    }
    // Free the allocated menu list space
    if(PossibleFileSystems != NULL) 
        gBS->FreePool(PossibleFileSystems);
	if(Variable == VARIABLE_ID_ADD_BOOT_OPTION)
	{
    // Set the File System Index for the new boot option added
 	   VarSetValue(VARIABLE_ID_ADD_BOOT_OPTION, STRUCT_OFFSET(NEW_BOOT_OPTION,SelFs), sizeof(UINT16), (VOID*)&gSelIdx);
	}
	if(Variable == VARIABLE_ID_ADD_DRIVER_OPTION)		
	{
		 VarSetValue(VARIABLE_ID_ADD_DRIVER_OPTION, STRUCT_OFFSET(NEW_DRIVER_OPTION,SelFs), sizeof(UINT16), (VOID*)&gSelIdx);
	}
    return Status;
}


/**
    To select the Boot file for the new boot option with the help of file browser.

    @param VOID

    @retval File Path string

    @retval EFI_STATUS

**/
EFI_STATUS FileBrowserLaunchFilePath(UINT32 Variable)
{
	EFI_STATUS Status;
//    EFI_HANDLE Handle = 0; Unused

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFs = NULL;
	EFI_FILE_PROTOCOL *NewFs = NULL;
	FILE_TYPE *FileList = NULL;
	UINTN FileCount = 0;
	UINT16 i = 0;
	//CHAR16 FilePath[120];
	CHAR16 *FilePath=NULL;
	UINTN filenamelength = 0, Length = 0;

	// Attempt to locate the post manager protocol
    Status = gBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL, (void**)&mPostMgr);
	if(!EFI_ERROR(Status))
	{
		if( gValidOption == TRUE ) 
		{
			gValidOption = FALSE;

			// Get the simple file system protocol 
            Status = gBS->HandleProtocol(gSmplFileSysHndlBuf[gSelIdx], &gSimpleFileSystemGuid, (void**)&SimpleFs);
			if(!EFI_ERROR(Status))
			{
				// And open it and return the efi file protocol
				Status = SimpleFs->OpenVolume(SimpleFs, &gFs);
			}
		}
		else {
			return EFI_UNSUPPORTED;
		}

		// Free handle buffer space (auto allocated by the called function)
		if(gSmplFileSysHndlBuf != NULL) 
			gBS->FreePool(gSmplFileSysHndlBuf);

		// clean up the file list and strings used in getting the file system
		CleanFileTypes(&FileList, &FileCount);
	}

	//MemSet(FilePath, 120, 0);
	if ( NULL == FilePath ) 
	{
		FilePath = (CHAR16*)EfiLibAllocateZeroPool(sizeof(CHAR16));
	}

	while(!EFI_ERROR(Status) && gFs != NULL)
	{
		i = 0;

		// Create a list of the files in the current directory
		Status = CreateFileList(gFs, &FileList, &FileCount);
		if(!EFI_ERROR(Status))
		{
			// Display the list in a menu and allow the user to select
			Status = DisplayFileListMenu(FileList, FileCount, &i, Variable);
			if(!EFI_ERROR(Status))
			{
				// The user selected something, attempt to open it
				Status = gFs->Open(  gFs,
					&NewFs,
					FileList[i].Name,
					EFI_FILE_MODE_READ,
					0);

				// close the old file system protocol and set it to null
				gFs->Close(gFs);
				gFs = NULL;

				Length = EfiStrLen(FilePath) + 3; // adding 2 more character for \ and null termination
				filenamelength = EfiStrLen(FileList[i].Name);
				if ( NULL != FilePath )
				{
					//Re-allocating based on filePath string length
					FilePath = MemReallocateZeroPool( FilePath, (Length * sizeof (CHAR16)), ( (Length * sizeof (CHAR16)) + (filenamelength * sizeof (CHAR16)) ) );
				}


				// Create the File Path based on the file selected
				UpdateFilePathString(FilePath, FileList[i].Name, i);

				// the newly selected item was opened correctly
				if(!EFI_ERROR(Status))
				{
					// check what type was opened
					if(FileList[i].Type != EFI_FILE_DIRECTORY)
					{
						// the file was read, close the file system protocol and set it to null
						NewFs->Close(NewFs);
						NewFs = NULL;

						//SPrint (FileName, 50, L"%s", FileList[i].Name);
						//ShowPostMsgBox( L"Selected Boot File Name", FileName, MSGBOX_TYPE_OK, &SelOpt );
					}
					gFs = NewFs;
				}
			}
		}

		if(FileCount <= 0) {
			ShowPostMsgBox( L"No Valid File", L"No Valid File Available in the Selected File System", MSGBOX_TYPE_OK, NULL );
		}
		// clean the strings that were used and free allocated space
		CleanFileTypes(&FileList, &FileCount);

		if(Status == EFI_ABORTED) {
			return Status;
		}
	}
	// Set the File path for the new boot option added.
	if(!EFI_ERROR(Status))
	{
		if (VARIABLE_ID_ADD_BOOT_OPTION == Variable)
			VarSetValue(VARIABLE_ID_ADD_BOOT_OPTION, STRUCT_OFFSET(NEW_BOOT_OPTION,Path), ((EfiStrLen(FilePath)+1)*sizeof(CHAR16)), FilePath);
		if (VARIABLE_ID_ADD_DRIVER_OPTION == Variable)				
			VarSetValue(VARIABLE_ID_ADD_DRIVER_OPTION, STRUCT_OFFSET(NEW_DRIVER_OPTION,DriverPath), ((EfiStrLen(FilePath)+1)*sizeof(CHAR16)), FilePath);
	}
	MemFreePointer((VOID**)&FilePath);
	return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
