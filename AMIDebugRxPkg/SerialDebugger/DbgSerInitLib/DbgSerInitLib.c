//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************
//
// Revision History
// ----------------
// $Log: $
// 
//**********************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:		DbgSerInitLib.c
//
// Description:This file contains code to halt debugger at pei entry point
//
//<AMI_FHDR_END>
//**********************************************************************

#include <PEI.h>
#include <EFI.h>
#ifndef EFIx64
#include <Library\AMIPeiDebug.h>
#else
#include <Library\AMIPeiDebugX64.h>
#endif

#pragma optimize( "", off )

#define DEBUG_MAILBOX_VECTOR            33

VOID SerialPeiInitialBreakPoint();

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	GetPeiDebugDataBaseaddress
//
// Description:	Function returns the DebugData pointer from  IDT.
//
// Input:		void
//
// Output:		UINTN
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
UINTN
GetPeiDebugDataBaseaddress (
  VOID
  )
{
  IA32_IDT_GATE_DESCRIPTOR   *IdtEntry;
  IA32_DESCRIPTOR            IdtDescriptor;
  UINTN                      Mailbox;

  AsmReadIdtr (&IdtDescriptor);
  IdtEntry = (IA32_IDT_GATE_DESCRIPTOR *) IdtDescriptor.Base;

  Mailbox = IdtEntry[DEBUG_MAILBOX_VECTOR].Bits.OffsetLow + (IdtEntry[DEBUG_MAILBOX_VECTOR].Bits.OffsetHigh << 16);
  return Mailbox;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   DbgSerInitConstructor
//
//  Description:
//  Library constructor - 
//
//  Input:	
//	VOID
//
//  Output: 
//	EFI_STATUS(based on result)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS 
EFIAPI 
DbgSerInitConstructor (
		VOID)
{
	PEIDebugData_T		*pData;
	SerXPortGblVar_T	*SerVar;
	EFI_STATUS Status = EFI_SUCCESS;
	UINTN PeiDebugDataBaseAddress = 0;
	
	// Get the PeiDebug Data Base Address
	PeiDebugDataBaseAddress = GetPeiDebugDataBaseaddress ();	

	pData		= (PEIDebugData_T *)PeiDebugDataBaseAddress;
	SerVar		= (SerXPortGblVar_T *)(pData->XportGlobalVar);

	//Initialize the PEIDbg Libraries
	Status = SerVar->gTransportProtocol->ReInitialize();
    
	if(!EFI_ERROR(Status)) {
		//SerialDebugger : This acts as the Pei Initial Breakpoint
		__debugbreak();
	}
	
	//Always return Success to avoid ASSERT
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	SerialPeiInitialBreakPoint
//
// Description:	Function sets Breakpoint
//
// Input:		VOID
//
// Output:		VOID
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
VOID SerialPeiInitialBreakPoint ()
{
	__debugbreak();
	
	return;
}
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
