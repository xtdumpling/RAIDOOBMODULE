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

/** @file PlatformPeiSystemInformation.c

    This file retrieves the system information and prints it to the screen
*/

#include "PlatformPeiSystemInformation.h"
#include <AmiTextOutHob.h>
#include <Library/HobLib.h>

InitializeCpuData (
		IN  CONST EFI_PEI_SERVICES	**PeiServices,
		AMI_PEI_TEXT_OUT		*This
);
InitializeDimmData (
		IN  CONST EFI_PEI_SERVICES	**PeiServices,
		AMI_PEI_TEXT_OUT		*This
);

EFI_STATUS OutputBiosVersion();
EFI_STATUS OutputCpuInformation(IN CONST EFI_PEI_SERVICES**, IN EFI_PEI_NOTIFY_DESCRIPTOR*, IN VOID*);
EFI_STATUS OutputDimmInformation(IN CONST EFI_PEI_SERVICES**, IN EFI_PEI_NOTIFY_DESCRIPTOR*, IN VOID*);

static EFI_PEI_NOTIFY_DESCRIPTOR  mPeiCpuNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    OutputCpuInformation
  }
};

static EFI_PEI_NOTIFY_DESCRIPTOR  mPeiDimmNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiSiliconRcHobsReadyPpi,
    OutputDimmInformation
  }
};

EFI_GUID    gVideoTextOutHobGuid = AMI_TEXT_OUT_HOB_GUID;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   OutputBiosVersion
//
//  Description: Outputs system information to Pei Early Video
//
//  Input:       VOID
//
//  Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
OutputBiosVersion (
        IN CONST EFI_PEI_SERVICES **PeiServices
  )
{   
    EFI_STATUS      		Status = EFI_SUCCESS;
    AMI_PEI_TEXT_OUT     	*EarlyVideoPpi;
    CHAR8                   	TextString[160];
    
    AMI_TEXT_OUT_HOB                *VideoConsoleHob = NULL;   
	CHAR8                   PlatformString[20];
    
    //CHAR16    PlatformName[]   = L"TypeNeonCityEPRP";
    //UINTN     PlatformNameSize = 0;
   
    DEBUG((EFI_D_INFO,"[PEI System Information] \n"__FUNCTION__));
    
    Status = (*PeiServices)->LocatePpi(
                        PeiServices,
#if ConsoleOutConsplitter_SUPPORT 
                        &gAmiPeiConsplitterTextOutPpiGuid,
#else                         
                        &gAmiPeiTextOutPpiGuid,
#endif                        
                        0,
                        NULL,
                        &EarlyVideoPpi);
    if (EFI_ERROR(Status)) return Status;
 
    //# Zero terminated string to Platform Name
    /*
    PlatformNameSize = sizeof (PlatformName);
    Status = PcdSet32S (PcdOemSkuPlatformNameSize           , (UINT32)PlatformNameSize);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;
    Status = PcdSetPtrS (PcdOemSkuPlatformName              , &PlatformNameSize, PlatformName);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return Status;  
	*/  
	
	#if PLATFORM_TYPE == 0 //Platform is Neon City
    AsciiSPrint (PlatformString, 20, "NeonCity");
#else			//Platform is Lightning Ridge
    AsciiSPrint (PlatformString, 20, "LightningRidge");
#endif
    
    VideoConsoleHob = GetHobList();
    if (VideoConsoleHob == NULL) {
        return EFI_NOT_FOUND;
    }
    
    VideoConsoleHob = GetNextGuidHob (&gVideoTextOutHobGuid, VideoConsoleHob);
    if (VideoConsoleHob == NULL) {
        return EFI_NOT_FOUND;
    }
    VideoConsoleHob->UpperCursorCol = 0;
    VideoConsoleHob->UpperCursorRow = 0;
    
    Status = EarlyVideoPpi->SetAttribute(EarlyVideoPpi, 0xF, 0, 0);
    Status = EarlyVideoPpi->WriteString(EarlyVideoPpi, 0, "Copyright(c) 2016 American Megatrends, Inc. \n\r"); 
	Status = EarlyVideoPpi->WriteString(EarlyVideoPpi, 0, "Sample System Information \n\r"); 
    AsciiSPrint (TextString, 160, "%a System BIOS Version: %a Date: %a \n\r", PlatformString, CONVERT_TO_STRING(BIOS_TAG), CONVERT_TO_STRING(TODAY));
    Status = EarlyVideoPpi->WriteString(EarlyVideoPpi, 0, TextString);
    Status = EarlyVideoPpi->SetAttribute(EarlyVideoPpi, 0x7, 0, 0); 

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   OutputCpu
//
//  Description: Outputs system information to Pei Early Video
//
//  Input:       VOID
//
//  Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
OutputCpuInformation (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi)
{
	EFI_STATUS		Status=EFI_SUCCESS;
	AMI_PEI_TEXT_OUT	*EarlyVideoPpi;
	DEBUG((EFI_D_INFO,"[PEI System Information] \n"__FUNCTION__));
	
	Status = (*PeiServices)->LocatePpi(
			PeiServices,
			#if ConsoleOutConsplitter_SUPPORT 
			&gAmiPeiConsplitterTextOutPpiGuid,
			#else                
			&gAmiPeiTextOutPpiGuid,
			#endif          
			0,
			NULL,
			&EarlyVideoPpi);
	if (EFI_ERROR(Status)) return Status;
	
	InitializeCpuData (PeiServices, EarlyVideoPpi);
	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   OutputCpu
//
//  Description: Outputs system information to Pei Early Video
//
//  Input:       VOID
//
//  Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
OutputDimmInformation (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi)
{
	EFI_STATUS		Status=EFI_SUCCESS;
	AMI_PEI_TEXT_OUT	*EarlyVideoPpi;
	
	DEBUG((EFI_D_INFO,"[PEI System Information] \n"__FUNCTION__));
	
	Status = (*PeiServices)->LocatePpi(
			PeiServices,
			#if ConsoleOutConsplitter_SUPPORT 
			&gAmiPeiConsplitterTextOutPpiGuid,
			#else                
			&gAmiPeiTextOutPpiGuid,
			#endif          
			0,
			NULL,
			&EarlyVideoPpi);
	if (EFI_ERROR(Status)) return Status;
	
	InitializeDimmData (PeiServices, EarlyVideoPpi);
	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   PeiSystemInformationEntryPoint
//
//  Description: Registers notification for when HECI is initialized
//
//  Input:       VOID
//
//  Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
PeiSystemInformationEntryPoint (
    IN        EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES        **PeiServices 
    )
{
        EFI_STATUS          Status = EFI_SUCCESS;
        EFI_PEI_NOTIFY_DESCRIPTOR   *pNotifyList;
        
        DEBUG((EFI_D_INFO,"[PEI System Information] \n"__FUNCTION__));
        
        OutputBiosVersion(PeiServices);

        pNotifyList = &mPeiCpuNotifyList[0];
        Status = (*PeiServices)->NotifyPpi(PeiServices, pNotifyList);
        
        pNotifyList = &mPeiDimmNotifyList[0];
        Status = (*PeiServices)->NotifyPpi(PeiServices, pNotifyList);
        
        
        return EFI_SUCCESS;
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
