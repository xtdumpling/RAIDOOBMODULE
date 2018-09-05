//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/19/2014
//
//****************************************************************************
//****************************************************************************
#include <Token.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/IoLib.h>
#include <Sps.h>
#include "MeAccess.h"
#include <Library/PciLib.h>
#include <Guid/SetupVariable.h>
#include <ppi/ReadOnlyVariable2.h>
#include "PEI.h"

#include <PchAccess.h>
#include <SspTokens.h>
#include <SmcLibCommon.h>
#include <Setup.h>
#include <Guid/SetupVariable.h>
#include <Register/PchRegsPmc.h>

#include <Library/SetupLib.h>
#include <Library/HobLib.h>

EFI_GUID gEfiPeiReadOnlyVariablePpiGuid = EFI_PEI_READ_ONLY_VARIABLE2_PPI_GUID;


EFIAPI SmcNVDIMMEndofPEI(
	IN EFI_PEI_SERVICES 		 **PeiServices,
	IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
	IN VOID 					 *Ppi)
{

	EFI_STATUS	Status = EFI_SUCCESS;

	INTEL_SETUP_DATA	mSetupData;
	SOCKET_MEMORY_CONFIGURATION mSocketMemoryConfiguration;

	UINTN PciPmcRegBase;
	BOOLEAN findADRMemory = FALSE;
	EFI_PEI_HOB_POINTERS        Hob;

	DEBUG((-1,  "ProjectEndofPEI Start.\n"));
	Status = GetEntireConfig (&mSetupData);
	
	(*PeiServices)->CopyMem (&mSocketMemoryConfiguration, &(mSetupData.SocketConfig.MemoryConfig), sizeof(SOCKET_MEMORY_CONFIGURATION));
	
	DEBUG((-1,  "SmcGlobalResetEn = 0x%x; ADREn = 0x%x.\n", mSocketMemoryConfiguration.SmcGlobalResetEn, mSocketMemoryConfiguration.ADREn));

	Status = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);
	while (!END_OF_HOB_LIST (Hob)) {
		if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
			if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY && 
				Hob.ResourceDescriptor->ResourceAttribute & ( EFI_RESOURCE_ATTRIBUTE_PERSISTENT | EFI_RESOURCE_ATTRIBUTE_PERSISTABLE ) ) {
		    		findADRMemory = TRUE;
				break;
			}
		}
		Hob.Raw = GET_NEXT_HOB (Hob);
	}

	DEBUG((-1,  "findADRMemory = 0x%x.\n", findADRMemory));

	if ( mSocketMemoryConfiguration.ADREn && findADRMemory ) {
		PcdSet64(PcdSmcNVDIMM, PcdGet64(PcdSmcNVDIMM) | BIT0);
	}
	else {
		PcdSet64(PcdSmcNVDIMM, PcdGet64(PcdSmcNVDIMM) & (~BIT0));
	}

	DEBUG((-1,  "PcdSmcNVDIMM = 0x%x.\n", PcdGet64(PcdSmcNVDIMM)));

	if ( ( PcdGet64(PcdSmcNVDIMM) & BIT0 ) && 
		mSocketMemoryConfiguration.SmcGlobalResetEn ) {
		// Enable CF9 trigger global reset
		PciPmcRegBase = MmPciBase (
				DEFAULT_PCI_BUS_NUMBER_PCH,
				PCI_DEVICE_NUMBER_PCH_PMC,
				PCI_FUNCTION_NUMBER_PCH_PMC
				);
		MmioOr32(PciPmcRegBase + R_PCH_PMC_ETR3, B_PCH_PMC_ETR3_CF9GR);
		DEBUG((-1,  "00:1F.2.AC = 0x%x.\n", MmioRead32(PciPmcRegBase + R_PCH_PMC_ETR3)));
		/*
		// ADRTIMERCTRL
		PciPmcRegBase = MmPciBase (
				DEFAULT_PCI_BUS_NUMBER_PCH,
				17,
				0
				);
		MmioAnd32(PciPmcRegBase + 0x180, ~(BIT31+BIT30+BIT27+BIT26+BIT25+BIT24));
		switch(mSocketMemoryConfiguration.SmcADRCompleteTimeout) {
			case 0:	// 25us
				break;
			case 1:	//50us
				MmioOr32(PciPmcRegBase + 0x180, BIT30);
				break;
			case 2:	// 100us
				MmioOr32(PciPmcRegBase + 0x180, BIT31);
				break;
			case 3:	// 200us
				MmioOr32(PciPmcRegBase + 0x180, BIT24);
				break;
			case 4:	// 400us
				MmioOr32(PciPmcRegBase + 0x180, (BIT30+BIT24));
				break;
			case 5:	// 800us
				MmioOr32(PciPmcRegBase + 0x180, (BIT31+BIT24));
				break;
		}
		*/
	}
	else {
		// If Reset trigger ADR is not enabled, do nothing here.
		;
	}

	DEBUG ((-1,  "ProjectEndofPEI End.\n"));
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : ProjectPeiDriverInit
//
// Description : Init ProjectPeiDriverInit
//
// Parameters  : none
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
EFIAPI
SmcNVDIMMPeiDriverInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices )
{
	EFI_STATUS Status;
	EFI_PEI_NOTIFY_DESCRIPTOR *pEOPCallback;

	DEBUG((EFI_D_INFO, "ProjectPeiDriverInit.\n"));

	Status = (**PeiServices).AllocatePool(
			       PeiServices,
			       sizeof (EFI_PEI_NOTIFY_DESCRIPTOR),
			       &pEOPCallback);
	if ( !EFI_ERROR(Status) ) {
		pEOPCallback->Flags = EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
							| EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
		pEOPCallback->Guid   = &gEfiEndOfPeiSignalPpiGuid;
		pEOPCallback->Notify = SmcNVDIMMEndofPEI;
		(*PeiServices)->NotifyPpi( PeiServices, pEOPCallback);
	}

	PcdSet64(PcdSmcNVDIMM, BIT0);

	DEBUG((EFI_D_INFO, "Exit ProjectPeiDriverInit.\n"));

	return EFI_SUCCESS;
}
