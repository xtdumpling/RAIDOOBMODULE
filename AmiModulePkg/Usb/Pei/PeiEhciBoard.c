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

/** @file PeiEhciBoard.c
    This file contains routines that are board specific for
    initializing the EHCI controller in the USB Recovery module.

**/

#include <Token.h>
#include <Ppi/Stall.h>
#include "PeiEhci.h"

#if EHCI_PEI_SUPPORT


//----------------------------------------------------------------------------
// Porting Steps:
//
// 1)  define/undefine the appropriate southbridge type
// 2)  define registers and their set/clear bits for each register to be
//     modified in each controller's PCI configuration space
// 3)  do not do any generic initialization - programming of BAR and command
//     register is done generically
//
//----------------------------------------------------------------------------



/**
    This is a porting hook for board-specific EHCI controller
    initialization

        
    @param PeiServices 
    @param Pci 
    @param StallPpi 

         
    @retval VOID

**/

EFI_STATUS EhciPeiBoardInit (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI *Pci,
    IN EFI_PEI_STALL_PPI    *StallPpi )
{
    EFI_STATUS Status = EFI_SUCCESS;

    return Status;

}

#endif

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
