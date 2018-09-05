//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2017 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//  File History
//
//  Rev. 1.05
//    Bug Fix:  
//    Reason:   Add a function hook before Update Bios Cfg to determine should update BIOS cfg.
//    Auditor:  Durant Lin
//    Date:     Jun/25/2018
//
//  Rev. 1.04
//    Bug Fix:  Add a function hook before LoadDefault.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Jun/12/2018
//
//  Rev. 1.03
//    Bug Fix:  Add other Oob Lbrary functions.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Feb/12/2018
//
//  Rev. 1.02
//    Bug Fix:  Add a interface to hook updatevariable before.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Jan/17/2018
//
//  Rev. 1.01
//    Bug Fix : N/A
//    Reason  : Isolate PciLib from OOB to OobLibraryProtocol, since different
//              platform will cause hang up issue.
//    Auditor : Durant Lin
//    Date    : Jun/02/2018
//
//  Rev. 1.00
//    Bug Fix : N/A
//    Reason  : Implement a SmcOobProtocol Interface for InBand and OutBand to
//              access Build time OobLibrary not OBJ OobLobrary. 
//    Auditor : Durant Lin
//    Date    : Dec/27/2017
//
//****************************************************************************

#include "SmcFeatureFlag.h"
#include "SmcOobPlatformPolicyDxe.h"
#include <Protocol/SmcOobLibraryProtocol.h>
#include <Library/PciLib.h>
#include "PlatformSpecificFunc.h"
#include "OobLibrary.h"
#include <SmcRomHole.h>

SMC_OOB_LIBRARY_PROTOCOL	SmcOobLibraryProtocolInterface;

typedef EFI_STATUS (SMC_UPDATE_PLATFORM_VARIABLE_FUNC)(IN CHAR16 	*VariableName, IN EFI_GUID	*VariableGuid, IN UINTN 	VariableSize, IN UINT8 	*VariableBuffer);

extern SMC_UPDATE_PLATFORM_VARIABLE_FUNC PLATFORM_SPECIFIC_FUNC_LIST  EndOfFunctionList;
static SMC_UPDATE_PLATFORM_VARIABLE_FUNC *PlatformSpecificFunc[] = { PLATFORM_SPECIFIC_FUNC_LIST NULL };

VOID SmcGenerateFeatureSupportFlagsFunc (
  IN     UINT8                          *Buffer,
  IN OUT UINT32 						*TotalLength,
  IN     SMC_OOB_PLATFORM_POLICY 		*SmcOobPlatformPolicy
){
	GenerateFeatureSupportFlags(Buffer,TotalLength,SmcOobPlatformPolicy);
}

UINT32 SmcPciRead32 (
  IN	UINT8	Bus,
  IN	UINT8	Device,
  IN	UINT8	Function,
  IN	UINT16	Register
  ){
		return PciRead32(PCI_LIB_ADDRESS(Bus,Device,Function,Register));
}
EFI_STATUS SmcUpdatePlatformVariable(
    IN CHAR16 	*VariableName,
    IN EFI_GUID	*VariableGuid,
    IN UINTN 	VariableSize,
    IN UINT8 	*VariableBuffer
){
	UINT8	i;
	EFI_STATUS Status;
	for(i=0;PlatformSpecificFunc[i] != NULL;++i){
		Status = PlatformSpecificFunc[i](VariableName,VariableGuid,VariableSize,VariableBuffer);
	}
	return Status;
}

VOID OobLoadDefaultPreSetting(){
	UINT8*		DataBuffer = NULL;
	EFI_STATUS	Status = EFI_SUCCESS;
	UINT32		BmcRegionSize = 0;

//Although we don't need to set this flag to be 0x01, I think we still obviously to set this flag
// in SUM loaddefault processing. This can prevent other unknown situations make DMI doesn't restore.
	BmcRegionSize = SmcRomHoleSize(SMBIOS_FOR_BMC_REGION);
	if(BmcRegionSize == 0) return;

	Status = gBS->AllocatePool(EfiBootServicesData,BmcRegionSize,&DataBuffer);
	if(EFI_ERROR(Status)) return;

	Status = SmcRomHoleReadRegion(SMBIOS_FOR_BMC_REGION,DataBuffer);
	if(EFI_ERROR(Status)) return;
	
	if(! (!!CompareMem(DataBuffer, "$SMC", 4))) return;
	
	*((UINT8*)(DataBuffer + 10)) = 0x01; //To Restore SMBIOS.
	
	SmcRomHoleWriteRegion(SMBIOS_FOR_BMC_REGION,DataBuffer);
	gBS->Stall(100); //For safeity, we stall 100 in here.

	gBS->FreePool(DataBuffer);

	return;	
}

BOOLEAN OobDetermineUpdBiosCfg(){
	

	return TRUE;
}


//===========================================================================================
VOID
InstallSmcOobLibraryProtocolInterface(){
	
	EFI_STATUS 		Status = EFI_SUCCESS;
	EFI_HANDLE		NewHandle = NULL;

	SmcOobLibraryProtocolInterface.Smc_GenerateFeatureSupportFlags = SmcGenerateFeatureSupportFlagsFunc;
	SmcOobLibraryProtocolInterface.Smc_PciRead32				   = SmcPciRead32;
	SmcOobLibraryProtocolInterface.Smc_UpdatePlatformVariable	   = SmcUpdatePlatformVariable;
	SmcOobLibraryProtocolInterface.Smc_CalculateMd5				   = CalculateMd5; 
	SmcOobLibraryProtocolInterface.Smc_OobGetCmosTokensValue	   = OobGetCmosTokensValue;
	SmcOobLibraryProtocolInterface.Smc_OobSetCmosTokensValue	   = OobSetCmosTokensValue;
	SmcOobLibraryProtocolInterface.Smc_OobLoadDefaultPreSetting	   = OobLoadDefaultPreSetting; 
	SmcOobLibraryProtocolInterface.Smc_OobDetermineUpdBiosCfg	   = OobDetermineUpdBiosCfg; 
	Status = gBS->InstallProtocolInterface (
                &NewHandle,
                &gSmcOobLibraryProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &SmcOobLibraryProtocolInterface
			);
}


