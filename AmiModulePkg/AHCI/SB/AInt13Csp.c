//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AInt13Csp.c
    AHCI INT13 Support Chip set Configuration File

**/
//---------------------------------------------------------------------------

#include "AmiDxeLib.h"
#include "Protocol/PciIo.h"

//---------------------------------------------------------------------------

#define LBAR_REGISTER		    0x20
#define LBAR_ADDRESS_MASK	    0xFFFFFFE0
#define INDEX_OFFSET_FROM_LBAR  0x10
#define DATA_OFFSET_FROM_LBAR   0x14



/**
    This is chip set porting routine that returns index/data ports
    to access memory-mapped registers.

    @param    PciIo

    @retval    EFI_SUCCESS Access information is collected
    @retval    EFI_ACCESS_DENIED No Access information available

**/

EFI_STATUS
GetAccessInfo (
    IN EFI_PCI_IO_PROTOCOL *PciIo,
    OUT UINT16  *AccessIndexPort,
    OUT UINT16  *AccessDataPort
)
{
    EFI_STATUS Status;
    UINT32 lbar;

    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, LBAR_REGISTER, 1, &lbar);
    ASSERT_EFI_ERROR(Status);

    lbar &= LBAR_ADDRESS_MASK;  // Legacy Bus Master Base Address

    *AccessIndexPort = (UINT16)lbar + INDEX_OFFSET_FROM_LBAR;
    *AccessDataPort = (UINT16)lbar + DATA_OFFSET_FROM_LBAR;

    return EFI_SUCCESS;

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************