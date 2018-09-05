//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
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
// Revision History
// ----------------
// $Log: $
// 
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  AmiHiiUpdateLib.C
//
// Description: Porting for PEI phase.Just for necessary devices porting.
//
//
//<AMI_FHDR_END>
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Uefi.h>
//Library used
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>

//GUIDs used
#include <Guid/MdeModuleHii.h>


//-------------------------------------------------------------------------
// Library Functions Implementation
//-------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////
UINT8 *
EFIAPI
AmiHiiCreateSuppresGrayIdVal(
  IN VOID           	*OpCodeHandle,
  IN EFI_QUESTION_ID  	QuestionId,
  UINT16				Value,
  BOOLEAN  				Suppress	//if TRUE Suppress; False Gray out.
  )
{
	EFI_IFR_EQ_ID_VAL	*Condition;
	EFI_IFR_OP_HEADER  	*Header;
	UINT8              	*Buffer=NULL;
	UINTN				sz;
//-------------------------------------
	//Allocate buffer for SUPPRESS_IF/GRAY_OUT_IF opcode + EFI_IFR_EQ_ID_VAL for suppress;
	sz=sizeof(EFI_IFR_OP_HEADER)+sizeof(EFI_IFR_EQ_ID_VAL);
	Buffer=AllocateZeroPool(sz);
	if(Buffer==NULL) {
		ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
		return Buffer;
	}
	
	//Init Pointers;
	Header=(EFI_IFR_OP_HEADER*)Buffer;
	Condition=(EFI_IFR_EQ_ID_VAL*)(Header+1);
	
	//Update OpCodes...
	//EFI_IFR_SUPPRESS_IF_OP/EFI_IFR_GRAY_OUT_IF_OP first;
	if(Suppress) Header->OpCode=EFI_IFR_SUPPRESS_IF_OP;
	else Header->OpCode=EFI_IFR_GRAY_OUT_IF_OP;
	Header->Length=sizeof(EFI_IFR_OP_HEADER);
	Header->Scope=1;
	
	//then goes conditions.
	//First update OpCode Header
	Condition->Header.OpCode=EFI_IFR_EQ_ID_VAL_OP;
	Condition->Header.Scope=0;
	Condition->Header.Length=sizeof(EFI_IFR_EQ_ID_VAL);
	//Then goes Opcode Data..
	Condition->QuestionId=QuestionId;
	Condition->Value=Value;
	
	//Here we go...
	return HiiCreateRawOpCodes(OpCodeHandle,Buffer,sz);
}


//*************************************************************************
UINT8 *
EFIAPI
AmiHiiTerminateScope(IN VOID *OpCodeHandle)
{
	EFI_IFR_END  		*IfrEnd;
    UINT8              	*Buffer;
    UINTN				sz;
//----------------------------------------- 
    sz=sizeof(EFI_IFR_END);
    Buffer=AllocateZeroPool(sz);
    IfrEnd=(EFI_IFR_END*)Buffer;

    //Update Header...
    IfrEnd->Header.OpCode = EFI_IFR_END_OP;
    IfrEnd->Header.Scope  = 0;
    IfrEnd->Header.Length = sizeof (EFI_IFR_END);

    //Submit it...	
    return HiiCreateRawOpCodes(OpCodeHandle,Buffer, sz);
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

