//**********************************************************************
//**********************************************************************
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/Resources.c 6     5/02/11 3:31p Arunsb $
//
// $Revision: 6 $
//
// $Date: 5/02/11 3:31p $
//**********************************************************************
/** @file Resources.c
    Library routines to load image resources.

**/
//**********************************************************************
#include <minisetup.h>

#if TSE_USE_EDK_LIBRARY

#include EFI_PROTOCOL_DEFINITION (FirmwareVolume)
#include EFI_PROTOCOL_DEFINITION (LoadedImage)
//#include EFI_PROTOCOL_DEFINITION (HII)
#if PI_SPECIFICATION_VERSION<0x00010000
    EFI_GUID guidFV = EFI_FIRMWARE_VOLUME_PROTOCOL_GUID;        //For EDK Library usage tse have to set the fv guids correctly
#else
    EFI_GUID guidFV = EFI_FIRMWARE_VOLUME2_PROTOCOL_GUID;
#endif

/**
    Function to read all the resources of a image.

    @param ImageHandle EFI_GUID *pGuid,
    @param ppData UINTN *pDataSize

    @retval Status

**/
EFI_STATUS ReadImageResource(
	EFI_HANDLE ImageHandle, EFI_GUID *pGuid, 
	VOID **ppData, UINTN *pDataSize
)
{
	EFI_STATUS Status;
	EFI_LOADED_IMAGE_PROTOCOL *pImage;
	EFI_FIRMWARE_VOLUME_PROTOCOL *pFV;
	UINTN i=0, DataSize;
	EFI_GUID *pSectionGuid;
	UINT32 Authentication;
      
	Status=gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, &pImage);
	if (EFI_ERROR(Status)) return Status;
	Status=gBS->HandleProtocol(pImage->DeviceHandle, &guidFV, &pFV);
	if (EFI_ERROR(Status)) return Status;
	do{
		pSectionGuid=NULL;
		DataSize=0;
		Status=pFV->ReadSection (
			pFV,&((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)(pImage->FilePath))->NameGuid,
			EFI_SECTION_FREEFORM_SUBTYPE_GUID,i++, &pSectionGuid, &DataSize,
			&Authentication
	 	);
		if (!EFI_ERROR(Status)&&EfiCompareGuid(pSectionGuid,pGuid)) break;
		gBS->FreePool(pSectionGuid);
	}while(!EFI_ERROR(Status));
	if (EFI_ERROR(Status)) return Status;
	*ppData = pSectionGuid;
	if (pDataSize) *pDataSize = DataSize;
	return EFI_SUCCESS;
}
#endif

/**
    Function to read all the section from of specified image.

    @param NameGuid IN EFI_GUID *pGuid,IN OUT VOID **Buffer,IN OUT UINTN *BufferSize

    @retval Status

**/
EFI_STATUS FvReadResources (
  IN EFI_GUID                       *NameGuid,
  IN EFI_GUID                       *pGuid, 
  IN OUT VOID                       **Buffer,
  IN OUT UINTN                      *BufferSize
  )
{
	EFI_FIRMWARE_VOLUME_PROTOCOL	*Fv;
    UINT32 AuthenticationStatus;

	EFI_STATUS	Status;
	UINTN		NumHandles;
	EFI_HANDLE	*HandleBuffer;
	UINTN		i,j=0;
	UINTN DataSize=0;
	EFI_GUID *pSectionGuid=NULL;

	Status = gBS->LocateHandleBuffer(ByProtocol,&guidFV,NULL,&NumHandles,&HandleBuffer);
	if (EFI_ERROR(Status)) return Status;

	for (i = 0; i< NumHandles; ++i)
	{
        j=0;
    	gBS->HandleProtocol(HandleBuffer[i],&guidFV,(void**)&Fv);
       	do{
    		pSectionGuid=NULL;
    		DataSize=0;
    		Status = Fv->ReadSection(
    			Fv,NameGuid,
    			EFI_SECTION_FREEFORM_SUBTYPE_GUID,j++,
    			(void **)&pSectionGuid, &DataSize,
    			&AuthenticationStatus
    		);
            if (!EFI_ERROR(Status)&&EfiCompareGuid(pSectionGuid,pGuid)) break;
    		gBS->FreePool(pSectionGuid);
    	}while(!EFI_ERROR(Status));

    	if (!EFI_ERROR(Status))
        {
            // Section found
            break;
        }
	}
  
    if (!EFI_ERROR(Status))  
    {
    	*Buffer = pSectionGuid;
    	if (BufferSize) *BufferSize = DataSize;
    }

	gBS->FreePool(HandleBuffer);
	return Status;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************