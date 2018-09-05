//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
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
  Implementation of the AmiCriticalSectionLib library class.
*/
#include <Library/AmiCriticalSectionLib.h>
#include <AmiCspLib.h>
#include <AmiDxeLib.h>

typedef struct{
	UINT32 Signature;
	BOOLEAN Busy;
    BOOLEAN SmiState;
    UINT8 IntState[2];
} CRITICAL_SECTION_INTERNAL;

#define CS_SIGNATURE SIGNATURE_32('_','C','S','_')

#define IS_INVALID_CRITICAL_SECTION(Cs) \
  ((Cs) == NULL || ((CRITICAL_SECTION_INTERNAL*)(Cs))->Signature!=CS_SIGNATURE)

static CRITICAL_SECTION_INTERNAL gCriticalSection = {
	CS_SIGNATURE, // UINT32 Signature;
	FALSE, // BOOLEAN Busy;
    FALSE, // BOOLEAN SmiState;
    {0,0} // UINT8 IntState[2];
};

static CRITICAL_SECTION_INTERNAL *gCriticalSectionPtr = NULL;

/**
 Creates critical section.
 @param CriticalSection
 @retval EFI_OUT_OF_RESOURCES
 @retval EFI_INVALID_PARAMETER
 @retval EFI_SUCCESS
*/
EFI_STATUS CreateCriticalSection(OUT CRITICAL_SECTION *CriticalSection){
	if (gCriticalSectionPtr != NULL) return EFI_OUT_OF_RESOURCES;
	if (CriticalSection == NULL) return EFI_INVALID_PARAMETER;
	gCriticalSection.Busy = FALSE;
	gCriticalSectionPtr = &gCriticalSection;
	*CriticalSection = gCriticalSectionPtr;
	return EFI_SUCCESS;
}

EFI_STATUS DestroyCriticalSection(IN CRITICAL_SECTION CriticalSection){
	if (IS_INVALID_CRITICAL_SECTION(CriticalSection))
		return EFI_INVALID_PARAMETER;
	gCriticalSectionPtr = NULL;
	return EFI_SUCCESS;
}

/**
  This function is called when critical section begins. It disables interupts, 
  and Smi and fills CRITICAL_SECTION structure fields
  @param CriticalSection - pointer to CRITICAL_SECTION structure
*/
EFI_STATUS BeginCriticalSection(IN CRITICAL_SECTION CriticalSection)
{
    CRITICAL_SECTION_INTERNAL TempCs;
	CRITICAL_SECTION_INTERNAL *Cs;
	
	if (IS_INVALID_CRITICAL_SECTION(CriticalSection))
		return EFI_INVALID_PARAMETER;
		
	Cs = (CRITICAL_SECTION_INTERNAL*)CriticalSection;
	if (Cs->Busy) return EFI_ACCESS_DENIED;
	
    TempCs.IntState[0] = IoRead8(0x21);
    TempCs.IntState[1] = IoRead8(0xa1);
    TempCs.SmiState = SbLib_GetSmiState();

    IoWrite8(0x21, 0xff);
    IoWrite8(0xa1, 0xff);
    SbLib_SmiDisable();

    Cs->Busy = TRUE;

    Cs->IntState[0] = TempCs.IntState[0];
    Cs->IntState[1] = TempCs.IntState[1];
    Cs->SmiState = TempCs.SmiState;

	return EFI_SUCCESS;
}

/**
  This function calls when critical section ends. It enable interupts, 
  and Smi and fills CRITICAL_SECTION structure fields
  @param CriticalSection - pointer to CRITICAL_SECTION structure
*/
EFI_STATUS EndCriticalSection(IN CRITICAL_SECTION CriticalSection)
{
    CRITICAL_SECTION_INTERNAL TempCs;
	CRITICAL_SECTION_INTERNAL *Cs;
	
	if (IS_INVALID_CRITICAL_SECTION(CriticalSection))
		return EFI_INVALID_PARAMETER;

	Cs = (CRITICAL_SECTION_INTERNAL*)CriticalSection;
	if (!Cs->Busy) return EFI_NOT_STARTED;
	
    TempCs.IntState[0] = Cs->IntState[0];
    TempCs.IntState[1] = Cs->IntState[1];
    TempCs.SmiState = Cs->SmiState;

    Cs->Busy = FALSE;

    if (TempCs.SmiState) SbLib_SmiEnable();
    IoWrite8(0x21, TempCs.IntState[0]);
    IoWrite8(0xa1, TempCs.IntState[1]);
	return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
