//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
/** @file AmiPciExpressGen3Lib.c
    Library Class for AMI Pci Express GEN 3 Support Functions. Dummy Instance


**/
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <AmiLib.h>
#include <AcpiRes.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/AmiPciBusLib.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------


/**

    @param 

    @retval 

**/
/*
EFI_STATUS EFIAPI AmiPciExpressLibConstructor(IN EFI_HANDLE  ImageHandle, IN EFI_SYSTEM_TABLE  *SystemTable)
{
	EFI_STATUS				Status=EFI_SUCCESS;
//-------------------------------------------------
	InitAmiLib(ImageHandle, SystemTable);

	return Status;;
}
*/


/**

    @param 

    @retval 

**/
EFI_STATUS Pcie3EqualizeLink(PCI_DEV_INFO *Device, BOOLEAN *LnkRetrain, UINT8 LinkSpeed){
    return EFI_SUCCESS;
}


/**

    @param 

    @retval 

**/
EFI_STATUS Pcie3GetEqualizationStatus(PCI_DEV_INFO *Device, UINT8 LinkSpeed){

    return EFI_SUCCESS;
}

/**

    @param 

    @retval 

**/
VOID Pcie3InitDisplayPcie3Data(PCI_DEV_INFO *Device){
	return;
}

/**

    @param 

    @retval 

**/
EFI_STATUS Pcie3AllocateInitPcie3Data(PCI_DEV_INFO *Device, UINT32 Pcie3CapOffset){
    return EFI_SUCCESS;
}

/**

    @param 

    @retval 

**/
BOOLEAN Pcie3CheckPcie3Compatible(PCI_DEV_INFO	*Device){
	return FALSE;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

