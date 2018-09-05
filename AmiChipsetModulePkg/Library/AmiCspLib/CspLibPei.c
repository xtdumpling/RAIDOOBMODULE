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

/** @file CspLibPei.c
    This file contains code for Generic CSP Library PEI functions.  The 
    functions include PCI table update etc.

*/

//============================================================================
// Module specific Includes
#include "Efi.h"
#include <AmiPeiLib.h>
#include "StatusCodes.h"
#include "token.h"
#include "Ppi/CspLibPpi.h"



//============================================================================
// GUID Definitions
//	EFI_GUID	gPeiPCITableInitPCIGUID = AMI_PEI_PCI_TABLE_INIT_PPI_GUID;
//============================================================================
// Produced PPIs
// PPI Member definitions

#ifndef PI_SPECIFICATION_VERSION //old Core
/**
    This function is the entry point for this PEI. This function initializes 
    installs the CSP PPI

    @param PeiServices - Pointer to the PEI services table
    @param Width - Register-level access width
    @param Address - PCI address of the register to write to (Bus/Dev/Func/Reg)
    @param SetBits - Mask of bits to be set (1 = Set)
    @param ClearBits - Mask of bits to be reset (1 = Reset)

    @retval Return Status based on errors that are returned by the InstallPpi
            function.

**/
EFI_STATUS PciCfgModify(
    IN CONST EFI_PEI_SERVICES 	**PeiServices,
    IN EFI_PEI_PCI_CFG_PPI_WIDTH	Width,
    IN UINT64					Address,
    IN UINTN					SetBits,
    IN UINTN					ClearBits)
{
    if((*PeiServices)->PciCfg==NULL)
        return EFI_NOT_AVAILABLE_YET;

    return (*PeiServices)->PciCfg->Modify(
                    (EFI_PEI_SERVICES**)PeiServices,
                    (*PeiServices)->PciCfg,
                    Width, Address,
                    SetBits, ClearBits);
}
#endif


EFI_STATUS PciTableInit (
  IN CONST	EFI_PEI_SERVICES      			**PeiServices,
  IN  		AMI_PEI_PCI_TABLE_INIT_PPI		*This,
  IN		EFI_PEI_PCI_CFG2_PPI				*PciCfg,
  IN  		UINT64                			Address,
  IN		AMI_PEI_PCI_INIT_TABLE_STRUCT	*PCIInitTable,
  IN		UINT16							wSize
);
	
EFI_STATUS PciTableInit2 (
	IN			EFI_PEI_SERVICES                **PeiServices,
	IN			AMI_PEI_PCI_TABLE_INIT_PPI      *This,
    IN		    EFI_PEI_PCI_CFG2_PPI				*PciCfg,
	IN			UINT64							CfgAddress,
	IN			AMI_PEI_PCI_INIT_TABLE_STRUCT2	*PciInitTable,
	IN			UINT16							TableEntries,
    IN          EFI_PEI_PCI_CFG_PPI_WIDTH       AccessWidth
);


// PPI interface definition
AMI_PEI_PCI_TABLE_INIT_PPI  mPciTableInitPpi =
{
	PciTableInit,
    PciTableInit2//,
};

static EFI_PEI_PPI_DESCRIPTOR mPpiList[] =  { 
(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
					    &gAmiPeiPciTableInitPpiGuid,//&gPeiPciTableInitPpiGuid,
					    &mPciTableInitPpi};

//============================================================================
// Portable Constants

//============================================================================
// Function Prototypes

//============================================================================
// Function Definitions


/**
    This function programs the PCI device with the values provided 
    in the initialized table provided

    @param PeiServices - Pointer to the PEI Core data Structure
    @param This - Pointer to an instance of the AMI PEI PCI TABLE INIT PPI
    @param PciCfg - Optional pointer to an instance of the PciCfg PPI
    @param Address - PCI address of the register to write to (Bus/Dev/Func/Reg)
    @param PCIInitTable - Table with register number, set and clear bits
    @param wSize - Table length (multiples of structure)

    @retval Always returns EFI_SUCCESS

**/
EFI_STATUS
PciTableInit (
  IN CONST	EFI_PEI_SERVICES      			**PeiServices,
  IN  		AMI_PEI_PCI_TABLE_INIT_PPI		*This,
  IN		EFI_PEI_PCI_CFG2_PPI			*PciCfg,			// OPTIONAL
  IN  		UINT64                			Address,
  IN		AMI_PEI_PCI_INIT_TABLE_STRUCT	PCIInitTable[],
  IN		UINT16							wSize
  )
{
	EFI_STATUS		Status = EFI_SUCCESS;
	UINTN			Index;
	UINT64			LocalAddr;


	// If PciCfg is NULL then locate PciCfg PPI
//		if (PciCfg == NULL) {
//			PciCfg = (*PeiServices)->PciCfg;
//		}

	if ((! wSize) || (!PCIInitTable))
		return Status;

    // Take data defined in the PCIInitTable and program the PCI devices
    // as ported
	for (Index = 0; Index < wSize; Index ++) {
		LocalAddr = Address + PCIInitTable[Index].bRegIndex;

        PciCfgModify(PeiServices,
                    EfiPeiPciCfgWidthUint8, LocalAddr,
                    PCIInitTable[Index].bORMask, ~PCIInitTable[Index].bANDMask);
//			PciCfg->Modify( PeiServices, PciCfg, EfiPeiPciCfgWidthUint8,
//							LocalAddr, 
//							PCIInitTable[Index].bORMask, 
//							~PCIInitTable[Index].bANDMask);
        
	}

	return EFI_SUCCESS;
}

/**
    This function is identical to PciTableInit with the addition of an extra 
    parameter to specify PCI access width.

    @param PeiServices - Pointer to the PEI Core data Structure
    @param This - Pointer to an instance of the AMI PEI PCI TABLE INIT PPI
    @param PciCfg - Optional pointer to an instance of the PciCfg PPI
    @param CfgAddress - PCI address of the register to write to (Bus/Dev/Func/Reg)
    @param PCIInitTable - Table with register number, set and clear bits
    @param TableEntries - Table length (multiples of structure)
    @param AccessWidth - Register-level access width

    @retval Always returns EFI_SUCCESS

**/

EFI_STATUS                            
PciTableInit2 (
	IN			EFI_PEI_SERVICES                **PeiServices,
	IN			AMI_PEI_PCI_TABLE_INIT_PPI      *This,
    IN			EFI_PEI_PCI_CFG2_PPI			*PciCfg,		// OPTIONAL
	IN			UINT64							CfgAddress,
	IN			AMI_PEI_PCI_INIT_TABLE_STRUCT2	*PciInitTable,
	IN			UINT16							TableEntries,
    IN          EFI_PEI_PCI_CFG_PPI_WIDTH       AccessWidth
  )
{
	EFI_STATUS		Status = EFI_SUCCESS;
	UINTN			Index;
	UINT64			LocalAddr;

	// If PciCfg is NULL then locate PciCfg PPI
//		if (PciCfg == NULL) {
//			PciCfg = (*PeiServices)->PciCfg;
//		}

	if ((! TableEntries) || (!PciInitTable))
		return Status;

    // Take data defined in the PCIInitTable and program the PCI devices
    // as ported
	for (Index = 0; 
         Index < TableEntries && !EFI_ERROR(Status); 
         Index ++) 
    {
		LocalAddr = CfgAddress + PciInitTable[Index].bRegIndex;

                    Status = PciCfgModify(PeiServices,
                                AccessWidth, LocalAddr,
                                PciInitTable[Index].bORMask, PciInitTable[Index].bANDMask);

//			Status = PciCfg->Modify( PeiServices,
//	                        PciCfg,
//	                        AccessWidth,
//							LocalAddr,
//							PciInitTable[Index].bORMask,
//							PciInitTable[Index].bANDMask);
        
    }

	return EFI_SUCCESS;
}


/**
    This function is the entry point for this PEI. This function initializes 
    installs the CSP PPI

    @param FfsHeader   Pointer to the FFS file header
    @param PeiServices Pointer to the PEI services table

    @retval Return Status based on errors that are returned by the InstallPpi
            function.

**/

EFI_STATUS
EFIAPI
CspLibPeiInit (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{

	EFI_STATUS					Status;

	// Install the NB Init Policy PPI
	Status = (*PeiServices)->InstallPpi(PeiServices, &mPpiList[0]);
#ifdef EFI_DEBUG
	if (Status) {
	    DEBUG((EFI_D_INFO, "CspLibPei_Init Return Code : %r\n", Status));
	}
#endif
	return Status;

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

