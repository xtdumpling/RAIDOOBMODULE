//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
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
//<AMI_FHDR_START>
//
// Name:  <F81866PEI.C>
//
// Description: Porting for PEI phase.Just for necessary devices porting.
//
//
//<AMI_FHDR_END>
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiPeiLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/AmiSio.h>
#include <Library/AmiSioPeiLib.h>
#include <Library/AmiIoDecodeLib.h>
#include <Library/PciCf8Lib.h>



// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: ProgramRtRegisterTable
//
// Description:
//  This function will program the runtime register.
//
// Input:
//  IN  UINT16  Base - Runtime register IO base
//  IN  SIO_DEVICE_INIT_DATA  *Table - initial table
//  IN  UINT8   Count - table data count
// Output:
//  NONE
//
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
VOID EFIAPI ProgramRtRegisterTable(
    IN  UINT16  Base,
    IN  SIO_DEVICE_INIT_DATA  *Table,
    IN  UINT8   Count
)
{
    UINT8   i;
    UINT8   Value8;

    for(i=0;i<Count;i++) {
        //Some registers may not allow to read, and also we can write some registers without any read operations. 
        if(Table[i].AndData8 == 0x00)  Value8 = Table[i].OrData8;
        else Value8 = (IoRead8(Base+Table[i].Reg16) & Table[i].AndData8) | Table[i].OrData8;
        IoWrite8(Base+Table[i].Reg16, Value8 );
    }
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

