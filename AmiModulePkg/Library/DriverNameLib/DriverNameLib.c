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
/** @file
  DriverNameLib DXE Core instance.
  
  This is a private DXE Core library. The DXE Core uses it to construct a driver name string.
**/

#include <Library/DriverNameLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <IndustryStandard/PeImage.h>

BOOLEAN GetPeImageName(UINT8 *p, CHAR8 *Name)
{
	
	UINTN length;
	CHAR8 *s, *q;
    s=PeCoffLoaderGetPdbPointer(p);
    if (s== NULL) return FALSE;
	length = AsciiStrLen(s);
	if (length<5 || AsciiStrCmp(&s[length-4],".pdb")) return FALSE;
	for(q = &s[length-5]; q>=s && *q!='\\'; q--);
	q++;
	length = s+length-4-q;
	CopyMem(Name,q,length);
	Name[length]=0;
	return TRUE;
}

BOOLEAN GetImageNameByHandle(EFI_HANDLE ImageHandle, CHAR8 *Name, UINTN size)
{
	EFI_LOADED_IMAGE_PROTOCOL *Image;
	EFI_COMPONENT_NAME_PROTOCOL *ComponentName;
	CHAR16 *UnicodeName;
	EFI_STATUS Status;
	
	Status = gDxeCoreST->BootServices->HandleProtocol(ImageHandle,&gEfiLoadedImageProtocolGuid,(VOID **)&Image);
	if (EFI_ERROR(Status)) Image = NULL;
	
 	if ( Image != NULL && GetPeImageName((UINT8*)Image->ImageBase,Name)) return TRUE;

	if (    !EFI_ERROR(gDxeCoreST->BootServices->HandleProtocol(ImageHandle,&gEfiComponentName2ProtocolGuid,(VOID **)&ComponentName))
		 && (     !EFI_ERROR(ComponentName->GetDriverName(ComponentName,"en",&UnicodeName))
		       || !EFI_ERROR(ComponentName->GetDriverName(ComponentName,"en-US",&UnicodeName))   
		    )
		    //Try UEFI 2.0 ComponentName protocol
       ||   !EFI_ERROR(gDxeCoreST->BootServices->HandleProtocol(ImageHandle,&gEfiComponentNameProtocolGuid,(VOID **)&ComponentName))
		 && !EFI_ERROR(ComponentName->GetDriverName(ComponentName,"eng",&UnicodeName))
	)
	{
		AsciiSPrint(Name,size,"%S", UnicodeName);
		return TRUE;
	}

	if (   Image != NULL 
		&& Image->FilePath != NULL
		&& Image->FilePath->Type==MEDIA_DEVICE_PATH
		&& Image->FilePath->SubType==MEDIA_PIWG_FW_FILE_DP
	)
	{
 		AsciiSPrint(Name,size,"[%g]",&((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)Image->FilePath)->FvFileName);
		return TRUE;
	}
	AsciiSPrint(Name,size,"Unknown");
	return FALSE;
}

CHAR8* GetDriverName(EFI_DRIVER_BINDING_PROTOCOL *pDriver)
{
	static CHAR8 Name[0x100];
	if (   !GetImageNameByHandle(pDriver->DriverBindingHandle,Name,0x100)
		&& !GetImageNameByHandle(pDriver->ImageHandle,Name,0x100)
	) AsciiSPrint(Name,0x100,"Unknown");
	return Name;
}
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
