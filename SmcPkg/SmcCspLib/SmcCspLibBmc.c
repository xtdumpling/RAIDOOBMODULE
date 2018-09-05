//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2015 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Read GPIO by GPIOLib
//    Reason:   
//    Auditor:  Mark Chen
//    Date:     Feb/16/2015
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/01/2014
//
//***************************************************************************
//***************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcCspLibBmc.c
//
// Description: Supermicro BMC library.
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include "SmcCspLibBmc.h"
#include "token.h"

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  SmcCspLibBmcPostCpl
//
// Description: 
//  Rise the GPIO pin to notify BMC that the BIOS POST was complete.
//  The function need the token(BMC_POST_COMPLETE) define. It's GPIO pin number.
//
// Input:
//      BOOLEAN BmcPostCpl -
//          TRUE : Rise GPIO (BMC_POST_COMPLETE).
//          FALSE : Drive a low value on GPIO (BMC_POST_COMPLETE).
//
// Output:      
//      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
SmcCspLibBmcPostCpl(
    IN	BOOLEAN	BmcPostCpl
)
{	
    GpioSetOutputValue(BMC_POST_COMPLETE, 0x1);
    return;
}

UINT8
EFIAPI
CheckBMCPresent(VOID)
{
    UINT8 ReturnVal = 0;
#ifdef BMC_Present_Pin
    EFI_STATUS	Status;

    UINT32         Value = 0;

    
    Status = GpioGetInputValue(BMC_Present_Pin, &Value);
    if (EFI_ERROR (Status)) {
	    ASSERT (FALSE);
	    return 0;
    }
    ReturnVal = (UINT8)Value;
#endif
    return ReturnVal;
}

//****************************************************************************
