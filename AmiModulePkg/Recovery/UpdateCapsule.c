//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015 American Megatrends, Inc.         **
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
  This file contains implementation of Capsule recovery device

**/

#include <Token.h>
#include <AmiPeiLib.h>
#include <AmiHobs.h>
#include <Hob.h>
#include <FlashUpd.h>
#include <Capsule.h>

// Definitions

#ifndef EFI_HOB_TYPE_CV
#define EFI_HOB_TYPE_CV EFI_HOB_TYPE_UEFI_CAPSULE
typedef EFI_HOB_UEFI_CAPSULE EFI_HOB_CAPSULE_VOLUME;
#endif // PI BACKWARD_COMPATIBLE_MODE


static EFI_GUID W8FwUpdateImageCapsuleGuid = W8_FW_UPDATE_IMAGE_CAPSULE_GUID;
// end debug


static EFI_PHYSICAL_ADDRESS gCapsuleAddress = 0; //!< Capsule image address
static UINT64               gCapsuleLength = 0;  //!< Capsule image size


EFI_STATUS VerifyFwImage(
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN OUT VOID          **pCapsule,
  IN OUT UINT32         *pCapsuleSize,
  IN OUT UINT32         *FailedVTask
);

/**
  This function searches for recovery capsule image
  
  @param PeiServices Pointer to pointer to EFI_PEI_SERVICES structure

  @retval EFI_SUCCESS   Image found
  @retval EFI_NOT_FOUND Image not found
**/
EFI_STATUS FindFWCapsuleHOB (
    IN    EFI_PEI_SERVICES **PeiServices
)
{
    EFI_HOB_CAPSULE_VOLUME  *pHob;
    EFI_PHYSICAL_ADDRESS        CapsuleAddress = 0;
    UINT64                      CapsuleLength = 0;
    EFI_CAPSULE_HEADER         *FWCapsuleVolume = NULL;
    EFI_GUID gFWCapsuleGuid   = APTIO_FW_CAPSULE_GUID;

    (*PeiServices)->GetHobList(PeiServices, &pHob);
// attempt to locate capsule volume hob
    while (!EFI_ERROR(FindNextHobByType(EFI_HOB_TYPE_CV, &pHob)))
    {
        // if capsule volume hob is found, determine the capsule's location
        CapsuleAddress = pHob->BaseAddress;
        CapsuleLength  = pHob->Length;
        FWCapsuleVolume = (EFI_CAPSULE_HEADER*) CapsuleAddress;

        // Check for WIndows GUID for Windows Firmware Update
        if (!guidcmp( &FWCapsuleVolume->CapsuleGuid,  &W8FwUpdateImageCapsuleGuid))
        {
            //  skip over Capsule header with Windows GUID and point to the actual BIOS Payload
            CapsuleLength = pHob->Length - FWCapsuleVolume->HeaderSize;
            CapsuleAddress = CapsuleAddress + FWCapsuleVolume->HeaderSize;
            FWCapsuleVolume = ( EFI_CAPSULE_HEADER *)CapsuleAddress;
            gCapsuleLength = CapsuleLength;
            gCapsuleAddress = CapsuleAddress;
        } 
        // this test should only be run if it is not a Windows Firmware Update situation
        // For this case the capsule header should always be outside the BIOS image and 
        //  therefore the check for GUID should be valid.
        else if (CapsuleLength != 0 && FWCapsuleVolume->CapsuleImageSize <= CapsuleLength &&
            !(guidcmp(&(FWCapsuleVolume->CapsuleGuid), &gFWCapsuleGuid)))
        {
            gCapsuleLength = CapsuleLength;
            gCapsuleAddress = CapsuleAddress;
        }
    }

    return ((gCapsuleAddress==0)? EFI_NOT_FOUND : EFI_SUCCESS);
}

/**
  This function verifies capsule image and prepares recovery HOB
  
  @param PeiServices Pointer to pointer to EFI_PEI_SERVICES structure
  @param RecoveryHob Pointer to RECOVERY_IMAGE_HOB structure

  @retval EFI_SUCCESS   Image verified and HOB created
  @retval EFI_NOT_FOUND Image not found
**/
EFI_STATUS LoadUpdateCapsule(
	IN EFI_PEI_SERVICES **PeiServices,
	RECOVERY_IMAGE_HOB *RecoveryHob
)
{
	EFI_STATUS Status;
    UINTN       Size;
    EFI_PHYSICAL_ADDRESS       CapsuleAddress;

	PEI_TRACE((TRACE_DXEIPL, PeiServices, "Loading Recovery Image..."));

// Locate Capsule Hob
    if(!gCapsuleAddress || !gCapsuleLength) return EFI_NOT_FOUND;

    Size = (UINTN)gCapsuleLength;
    CapsuleAddress = (EFI_PHYSICAL_ADDRESS)gCapsuleAddress;
    Status = VerifyFwImage(PeiServices, (VOID**)&CapsuleAddress, (UINT32*)&Size,(UINT32*)&RecoveryHob->FailedStage ); 
    RecoveryHob->Status = (UINT8)Status;
    RecoveryHob->Address = CapsuleAddress;
    RecoveryHob->ImageSize = (UINT32)gCapsuleLength;
    if (EFI_ERROR(Status)) {
	    PEI_ERROR_CODE(PeiServices, PEI_RECOVERY_INVALID_CAPSULE, EFI_ERROR_MAJOR);
    }else{
    	PEI_PROGRESS_CODE(PeiServices,PEI_RECOVERY_CAPSULE_LOADED);
	}
	return Status;
}

/**
  This function implements upper level capsule recovery device interface
  
  @param PeiServices Pointer to pointer to EFI_PEI_SERVICES structure
  @param RecoveryHob Pointer to RECOVERY_IMAGE_HOB structure

  @retval EFI_SUCCESS   Image verified and HOB created
  @retval EFI_NOT_FOUND Image not found
**/
EFI_STATUS ProcessUpdateCapsule(
    IN EFI_PEI_SERVICES **PeiServices,
	IN RECOVERY_IMAGE_HOB *RecoveryHob
)
{
	EFI_STATUS Status;

    Status = FindFWCapsuleHOB(PeiServices);
    if (!EFI_ERROR(Status)) {
		Status = LoadUpdateCapsule(PeiServices,RecoveryHob);
	}
    if (EFI_ERROR(Status)) {
		PEI_ERROR_CODE(PeiServices, PEI_RECOVERY_NO_CAPSULE, EFI_ERROR_MAJOR);	
    }
	return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015 American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
