//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
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
/** @file AmiSriovLib.c
    Library Class for AMI SR-IOV Functions. The dummy instance


**/
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <AmiLib.h>
#include <AcpiRes.h>
#include <PciBus.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------


/**
    This function will collect information about SRIOV PCIE Device
    and initialize it based on information collected.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_OUT_OF_RESOURCES    When system runs out of resources.
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS SriovProbeDevice(PCI_DEV_INFO *Device, UINT8 *MaxBusFound){
    return EFI_SUCCESS;
}


/**

    @param 

    @retval 

**/
EFI_STATUS SriovAllocateSriovData(PCI_DEV_INFO *Device, UINT32 SriovCapOffset){
    return EFI_SUCCESS;
}


/**

    @param 

    @retval 

**/
BOOLEAN SriovCheckSriovCompatible(PCI_DEV_INFO	*Device){
	return FALSE;
}


/**

    @param 

    @retval 

**/
EFI_STATUS	SriovDbAddVirtualBar(PCI_DEV_INFO *Device, DBE_DATABASE *Db, MRES_TYPE ResType){
    return EFI_SUCCESS;
}


/**

    @param 

    @retval 

**/
BOOLEAN SriovCheckBarType(PCI_DEV_INFO *Device, PCI_BAR_TYPE BarType){
    return FALSE;
}


/**

    @param 

    @retval 

**/
VOID SriovConvertResources(	PCI_DEV_INFO *Device, PCI_BAR_TYPE NarrowType, PCI_BAR_TYPE WideType,
							RES_CONV_TYPE ConvType, BOOLEAN NeedToConvert, BOOLEAN CombineMemPmem)
{
	return;
}

EFI_STATUS  SriovAriCapableHierarchy(PCI_DEV_INFO *Device, BOOLEAN Set){
    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

