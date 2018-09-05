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
// $Header:$
//
// $Revision:  $
//
// $Date: $
//*************************************************************************
/** @file AmiPciExpressGen3Lib.h
    Library Class for AMI Pci Express GEN3 Support Functions.

**/
//*************************************************************************
#ifndef _AMI_PCI_EXPRESS_GEN_3_LIB_H_
#define _AMI_PCI_EXPRESS_GEN_3_LIB_H_
#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Pci.h>
#include <PciE.h>
#include <PciE21.h>
#include <PciE30.h>
#include <PciBus.h>
#include <AmiLib.h>
#include <AcpiRes.h>
#include <Protocol/PciIo.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------


/**

    @param 

         
    @retval EFI_SUCCESS Set successfully.
    @retval EFI_INVALID_PARAMETER the Input parameter is invalid.

**/
EFI_STATUS Pcie3EqualizeLink(PCI_DEV_INFO *Device, BOOLEAN *LnkRetrain, UINT8 LinkSpeed);

/**

    @param 

         
    @retval EFI_SUCCESS Set successfully.
    @retval EFI_INVALID_PARAMETER the Input parameter is invalid.

**/
EFI_STATUS Pcie3GetEqualizationStatus(PCI_DEV_INFO *Device, UINT8 LinkSpeed);


/**

    @param 

    @retval 

**/
VOID Pcie3InitDisplayPcie3Data(PCI_DEV_INFO *Device);




/**

    @param 

    @retval 

**/
EFI_STATUS Pcie3AllocateInitPcie3Data(PCI_DEV_INFO *Device, UINT32 Pcie3CapOffset);



/**

    @param 

    @retval 

**/
BOOLEAN Pcie3CheckPcie3Compatible(PCI_DEV_INFO	*Device);


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif //_AMI_PCI_EXPRESS_GEN_3_LIB_H_
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

