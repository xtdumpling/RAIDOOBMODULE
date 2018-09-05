//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  History
//
//    Rev.  1.00
//     Bug Fix: Add sum load default function.
//     Reason:
//     Auditor:	Hartmann Hsieh
//     Date:  	Oct/11/2016
//
//****************************************************************************
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        ResetConfigPeiLib.c
//
//
//<AMI_FHDR_END>
//**********************************************************************

#include "AmiPeiLib.h"
#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SmcResetConfig
//
// Description:
//
// Input:
//  **PeiServices - pointer to a pointer to the PEI Services Table.
//  *ReadVariablePpi - pointer to EFI_PEI_READ_ONLY_VARIABLE2_PPI PPI. The pointer can be used to
//                     read and enumerate existing NVRAM variables.
//
// Output:
//  TRUE - Reset BIOS Config
//  FALSE - Don't Reset BIOS Config
//
// Notes:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN SmcResetConfig (
    IN EFI_PEI_SERVICES **PeiServices,
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
)
{
    EFI_STATUS Status;
    UINTN Size;
    UINT8 Value;

    Size = sizeof (UINT8);
    Status = ReadVariablePpi->GetVariable (
                 ReadVariablePpi,
                 L"SmcInBandLoadDefault",
                 &gEfiSmcInBandLoadDefaultGuid,
                 NULL,
                 &Size,
                 &Value );
    if (EFI_ERROR (Status)) {
        return FALSE;
    }
    return TRUE;
}


