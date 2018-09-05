//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
  @brief AMI DXE IPL Improvements.

  The file contains Aptio specific DXE IPL code.
**/

#include <AmiHobs.h>
#include <Ppi/AmiReadyToLoadDxeCore.h>
#include <Ppi/LoadImage.h>
#include <Guid/AmiGlobalVariable.h>
#include "DxeIpl.h"

// Defined in DxeLoad.c
BOOLEAN ValidateMemoryTypeInfoVariable (
    IN EFI_MEMORY_TYPE_INFORMATION *MemoryData, IN UINTN MemoryDataSize
);

CONST EFI_PEI_PPI_DESCRIPTOR ReadyToLoadDxeCorePpiDesc = {
	(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
	&gAmiReadyToLoadDxeCorePpiGuid, NULL
};

// The order of elements in this array is important.
// It defines the order of memory types in the DXE memory map.
// The first array element corresponds to the type with the largest address.
// Keeping boot time memory types at the bottom of the list improves 
// stability of the runtime portions of the memory map
// which is important during S4 resume.
CONST EFI_MEMORY_TYPE_INFORMATION DefaultMemoryTypeInformation[] = {
    { EfiRuntimeServicesCode,     0x30 },
    { EfiRuntimeServicesData,     0x20 },
    { EfiACPIMemoryNVS,           0x60 },
    { EfiACPIReclaimMemory,       0x10 },
    { EfiReservedMemoryType,      0x30 },
    { EfiBootServicesCode,        0x600 },
    { EfiBootServicesData,        0x1500 },
    { EfiMaxMemoryType,           0 }		// indicates the end of the table
};

CONST EFI_MEMORY_TYPE_INFORMATION *DefaultMemoryTypeInformationPtr = DefaultMemoryTypeInformation;
// APTIOV_SERVER_OVERRIDE_START: Take changes from latest SVN file check-in
UINTN DefaultMemoryTypeInformationSize = sizeof(DefaultMemoryTypeInformation);
// APTIOV_SERVER_OVERRIDE_END: Take changes from latest SVN file check-in

EFI_STATUS S3Resume(){
    EFI_STATUS Status;
    EFI_PEI_S3_RESUME2_PPI *S3ResumePpi;

    DEBUG ((DEBUG_INFO | DEBUG_LOAD, "S3Resume\n"));
    Status = PeiServicesLocatePpi (
        &gEfiPeiS3Resume2PpiGuid, 0, NULL, (VOID **) &S3ResumePpi
    );

    if (EFI_ERROR(Status)){
        REPORT_STATUS_CODE(EFI_ERROR_CODE|EFI_ERROR_MAJOR,PEI_S3_RESUME_PPI_NOT_FOUND);
        return Status;
    }

    DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Calling S3RestoreConfig\n"));
    PERF_END (NULL,"DxeIpl", NULL, 0);
    Status = S3ResumePpi->S3RestoreConfig2 (S3ResumePpi);

    return Status;
}

/**
  Implementation of the recovery boot path.
  
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCESS  The recovery capsule has been successfully loaded.
  @retval Others      Recovery capsule not found. 

**/
EFI_STATUS Recovery(){
    EFI_STATUS Status;
	EFI_PEI_RECOVERY_MODULE_PPI *PeiRecovery;

    Status = PeiServicesLocatePpi (
    	&gEfiPeiRecoveryModulePpiGuid, 0, NULL, (VOID **) &PeiRecovery
    );

	if (EFI_ERROR(Status)) {
        REPORT_STATUS_CODE(EFI_ERROR_CODE|EFI_ERROR_MAJOR, PEI_RECOVERY_PPI_NOT_FOUND);
        return Status;
    }
	return PeiRecovery->LoadRecoveryCapsule(GetPeiServicesTablePointer(), PeiRecovery);
}

/**
  Searches for recovery capsule address in the GUIDed HOB.
  
  @param  PeiServices Describes the list of possible PEI Services.
  @param  Buffer On output conains recovery capsule address.

  @retval EFI_SUCESS  The recovery capsule address has been found.
  @retval Others      The recovery capsule address cannot be found.

**/
EFI_STATUS FindRecoveryBuffer(OUT VOID **Buffer){

    RECOVERY_IMAGE_HOB *RecoveryHob;

    RecoveryHob = GetFirstGuidHob(&gAmiRecoveryImageHobGuid);
    if(RecoveryHob == NULL) return EFI_NOT_FOUND;

    if(RecoveryHob->Status == EFI_SUCCESS && RecoveryHob->Address != 0) {
        *Buffer = (VOID *)(UINTN)RecoveryHob->Address;
        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}

/**
  Publishes Firmware volumes required for DXE phase.
  
  @param  PeiServices Describes the list of possible PEI Services.
  @param  BootMode Current boot mode.

  @retval Number of the next firmware volume to start search for DXE_CORE from.

**/
UINTN ProcessRecoveryAndBootOnFlashUpdate(){
    EFI_STATUS Status;
    UINTN FvNum = 0;
    VOID *RecoveryBuffer = NULL;
    EFI_PEI_FV_HANDLE FvHandle;

// BOOT_IN_RECOVERY_MODE - actual recovery or flash update via recovery capsule on disk 
// (in a latter case, boot mode will be changed to BOOT_ON_FLASH_UPDATE after a call to Recovery() function)
// BOOT_ON_FLASH_UPDATE - flash update via recovery capsule in memory
	Status = Recovery();
	if (!EFI_ERROR(Status)) {
        Status = FindRecoveryBuffer(&RecoveryBuffer);
	}
    if(EFI_ERROR(Status)) {
      	// Can't find recovery capsule. Report an error
        PEI_ERROR_CODE(GetPeiServicesTablePointer(), PEI_RECOVERY_FAILED, EFI_ERROR_MAJOR);
        //if recovery failed, revert to a normal boot path based on PCD value
        if(!PcdGetBool(PcdForceSetupOnFailedRecovery))
         	PeiServicesSetBootMode(BOOT_WITH_FULL_CONFIGURATION);
    }else {
    	// Recovery function may have changed the boot mode.
        if(GetBootModeHob () == BOOT_IN_RECOVERY_MODE || PcdGetBool(PcdUseNewImageOnFlashUpdate)) {
        	// When we're in recovery mode, we publish DXE FV from recovery buffer, instead of flash
            while( !EFI_ERROR(PeiServicesFfsFindNextVolume(FvNum, &FvHandle)))
                FvNum++; //skip all FV that have already been published
        }
    }
    return FvNum;
}

VOID PublishMemoryInformationHob(EFI_BOOT_MODE BootMode){
	EFI_STATUS Status;
	EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariablePpi;
	UINTN Size;
	EFI_HOB_GUID_TYPE *MemoryInformationHob;
	EFI_MEMORY_TYPE_INFORMATION *MemoryInformationPtr;
	EFI_MEMORY_TYPE_INFORMATION MemoryInformationBuffer[EfiMaxMemoryType + 1];
	CHAR16 *MemoryInformationVariableName;
	
	// There can only be a single memory type information HOB.
	// Invalidate other HOB instances (if any).
    for(  MemoryInformationHob = GetFirstGuidHob(&gEfiMemoryTypeInformationGuid)
		  ; MemoryInformationHob != NULL
		  ; MemoryInformationHob = GetNextGuidHob(&gEfiMemoryTypeInformationGuid, MemoryInformationHob)
    ) MemoryInformationHob->Header.HobType = EFI_HOB_TYPE_UNUSED;
	    
	Status = PeiServicesLocatePpi (
		&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **)&VariablePpi
	);
	if (!EFI_ERROR (Status)) {
	    Size = sizeof (MemoryInformationBuffer);
	    MemoryInformationVariableName = (BootMode == BOOT_ON_S4_RESUME)
	    	            ? L"PreviousMemoryTypeInformation"
	    	            : EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME;
	    // Get variable with gAmiGlobalVariableGuid
	    Status = VariablePpi->GetVariable ( 
	    	VariablePpi, 
	    	MemoryInformationVariableName, &gAmiGlobalVariableGuid,
	    	NULL, &Size, MemoryInformationBuffer
	    );
	    if ( Status == EFI_NOT_FOUND ){
	    	// If variable has not been found, we might be dealing with old version of BDS.
	    	// Try getting a variable using old GUID
		    Status = VariablePpi->GetVariable ( 
		    	VariablePpi, 
		    	MemoryInformationVariableName, &gEfiMemoryTypeInformationGuid,
		    	NULL, &Size, MemoryInformationBuffer
		    );
	    }
	    if (!EFI_ERROR (Status)){
	    	if (ValidateMemoryTypeInfoVariable(MemoryInformationBuffer, Size))
	    		MemoryInformationPtr = MemoryInformationBuffer;
	    	else Status = EFI_UNSUPPORTED;
	    }
	}
	if (EFI_ERROR (Status)) {
	    Size = DefaultMemoryTypeInformationSize;
	    MemoryInformationPtr = DefaultMemoryTypeInformationPtr;
	}
    // Build Memory Information HOB for DXE Core.
    BuildGuidDataHob(
        &gEfiMemoryTypeInformationGuid, MemoryInformationPtr, Size
    );
    // This HOB will notify BDS that DxeIpl supports 
    // gAmiGlobalVariableGuid for the Memory Information variables.
    BuildGuidDataHob(
    	&gAmiGlobalVariableGuid, &gEfiMemoryTypeInformationGuid, sizeof(EFI_GUID)
    );
}

EFI_STATUS EFIAPI AmiDxeIplEntry (
    IN CONST EFI_DXE_IPL_PPI *This, IN EFI_PEI_SERVICES **PeiServices, 
    IN EFI_PEI_HOB_POINTERS  HobList
){
    EFI_STATUS Status;
    EFI_FV_FILE_INFO DxeCoreFileInfo;
    EFI_PHYSICAL_ADDRESS DxeCoreAddress;
    UINT64 DxeCoreSize;
    EFI_PHYSICAL_ADDRESS DxeCoreEntryPoint;
    EFI_BOOT_MODE BootMode;
    EFI_PEI_FV_HANDLE FvHandle;
    EFI_PEI_FILE_HANDLE FileHandle;
    EFI_PEI_LOAD_FILE_PPI *LoadFile;
    UINTN Instance;
    UINT32 AuthenticationState;
    UINTN FvNum = 0;

    PERF_START (NULL,"DxeIpl", NULL, 0);
    PEI_PROGRESS_CODE(PeiServices,PEI_DXE_IPL_STARTED);

    BootMode = GetBootModeHob ();
    if (BootMode == BOOT_ON_S3_RESUME) {
        Status = S3Resume();
        //if S3 Resume failed, report an error and reset the system
        if (EFI_ERROR(Status)) {
            REPORT_STATUS_CODE(EFI_ERROR_CODE|EFI_ERROR_MAJOR, PEI_S3_RESUME_FAILED);
            PeiServicesResetSystem();
        }
    } else if (BootMode == BOOT_IN_RECOVERY_MODE || BootMode == BOOT_ON_FLASH_UPDATE){
        FvNum = ProcessRecoveryAndBootOnFlashUpdate();
        // Re-read the boot mode. It may have been changed by the ProcessRecoveryAndBootOnFlashUpdate
        BootMode = GetBootModeHob ();
    }
    // RomLayoutPei has callback on this PPI. It will publish DXE FV there.
    PeiServicesInstallPpi(&ReadyToLoadDxeCorePpiDesc);
    PublishMemoryInformationHob(BootMode);
    
    // Look in all the FVs present in PEI and find the DXE Core FileHandle
    do{
        Status = PeiServicesFfsFindNextVolume(FvNum++, &FvHandle);
        if ( EFI_ERROR(Status) ) break;
        FileHandle = NULL;
        Status = PeiServicesFfsFindNextFile (EFI_FV_FILETYPE_DXE_CORE, FvHandle, &FileHandle);
        if ( !EFI_ERROR(Status) ) break;
    }while(TRUE);
    if (EFI_ERROR(Status)) {
        REPORT_STATUS_CODE (EFI_ERROR_CODE|EFI_ERROR_MAJOR, PEI_DXE_CORE_NOT_FOUND);
        PERF_END (NULL,"DxeIpl", NULL, 0);
        return EFI_NOT_FOUND;
    }

    // Load the DXE Core from a Firmware Volume.
    Instance = 0;
    do {
        Status = PeiServicesLocatePpi (&gEfiPeiLoadFilePpiGuid, Instance++, NULL, (VOID **) &LoadFile);
        // These must exist an instance of EFI_PEI_LOAD_FILE_PPI to support to load DxeCore file handle successfully.
        ASSERT_EFI_ERROR (Status);
        Status = LoadFile->LoadFile (
            LoadFile, FileHandle,
            &DxeCoreAddress, &DxeCoreSize, &DxeCoreEntryPoint,
            &AuthenticationState
        );
    } while (EFI_ERROR (Status));

    // Get the DxeCore File Info from the FileHandle to build DxeCore HOB.
    Status = PeiServicesFfsGetFileInfo (FileHandle, &DxeCoreFileInfo);
    ASSERT_EFI_ERROR (Status);

    // Add HOB for the DXE Core
    BuildModuleHob (
        &DxeCoreFileInfo.FileName, DxeCoreAddress,
        ALIGN_VALUE (DxeCoreSize, EFI_PAGE_SIZE), DxeCoreEntryPoint
    );

    // Transfer control to the DXE Core
    HandOffToDxeCore (DxeCoreEntryPoint, HobList);

    // If we get here, then the DXE Core returned.  This is an error
    // DxeCore should not return.
    ASSERT (FALSE);
    CpuDeadLoop ();

    return EFI_OUT_OF_RESOURCES;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
