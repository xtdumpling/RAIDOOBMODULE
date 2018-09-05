//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/16/2016
//
//***************************************************************************
//***************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcCspLibSb.c
//
// Description: Supermicro sourth bridge library.
//
//<AMI_FHDR_END>
//**********************************************************************
#include "token.h"
#include <Register/PchRegsPmc.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include "SmcLib.h"
#include "SmcCspLibSb.h"

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure: SMC_Enable_WOL
//
// Description: Enable PME for WOL
//
// Input: None
//
// Output: None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
SMC_Enable_PME(VOID)
{
    // Clear PME_B0_STS and PME_STS, then Set PME_B0_EN and PME_EN and PCI_EXP_EN
    IoWrite16(PM_BASE_ADDRESS + 0x8C, 0xffff);
    IoOr16(PM_BASE_ADDRESS + 0x9C, BIT13 + BIT11 + BIT9);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure: SMC_Enable_WOL
//
// Description: Enable PME for WOL
//
// Input: None
//
// Output: None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
SMC_Enable_WOL(VOID)
{
    IoWrite16(PM_BASE_ADDRESS + 0x20, BIT11);
    IoOr16(PM_BASE_ADDRESS + 0x98, BIT11);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure: SMC_Enable_WOR
//
// Description: Enable RI# for WOR
//
// Input: None
//
// Output: None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
SMC_Enable_WOR(VOID)
{
    IoWrite16(PM_BASE_ADDRESS + 0x20, BIT8);
    IoOr16(PM_BASE_ADDRESS + 0x98, BIT8);
}


//****************************************************************************

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  SMC_Get_SATA_Mode
//
// Description: 
//  Get SATA Interface Mode
//
// Input:
//      None
//
// Output:
//      UINT8 (return) -
//          1 : Raid mode
//          0 : AHCI mode
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT8
SMC_Get_SATA_Mode()
{
    UINT32	TempData32;

    TempData32 = PciRead32(PCI_LIB_ADDRESS(0, 23, 0, 0x9C));
    if(TempData32 & BIT16)
	return 1;
    else
	return 0;
}

EFI_STATUS
AfterG3Setting(
    IN	BOOLEAN	LastState
)
{
    UINT8	TempValue;

    DEBUG((-1, "AfterG3Setting Start.\n"));
    
    TempValue = PciRead8(PCI_LIB_ADDRESS(0, 31, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B));

    if(LastState)
	TempValue |= B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN;
    else
	TempValue &= (UINT8)~B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN;

    PciWrite8(PCI_LIB_ADDRESS(0, 31, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B), TempValue);
    
    DEBUG((-1, "AfterG3Setting End, Value = %x.\n", TempValue));
    return EFI_SUCCESS;
}
