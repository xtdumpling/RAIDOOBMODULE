//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        XhciDebugger.C
//
// Description: 
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#pragma optimize( "", off )

#include <Library/DebugAgentLib.h>
#include <Library/AMIpeidebug.h>

#define DEBUG_AGENT_INIT_XHCI_PEI		16
#define DEBUG_MAILBOX_VECTOR            33

UINTN InitXportLayerXhci( 
	UINTN DebugBaseAddress
);

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	GetPeiDebugDataBaseaddress
//
// Description:	Function returns the DebugData pointer from  IDT.
//
// Input:		 VOID
//
// Output:		UINTN
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
UINTN
GetPeiDebugDataBaseaddress1 (
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
//  Procedure:   DbgXhciInitialize
//
//  Description:
//
//  Input:	IN EFI_PEI_FILE_HANDLE, 
//			IN CONST EFI_PEI_SERVICES  **
//
//  Output: 
//	EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS 
DbgXhciInitialize (
		IN EFI_PEI_FILE_HANDLE     FileHandle,
		IN CONST EFI_PEI_SERVICES  **PeiServices )
{
	EFI_STATUS	Status = EFI_SUCCESS;
	UINTN PeiDebugDataBaseAddress = 0;
	PEIDebugData_T *pData;
	SerXPortGblData_T *SerData;
		
	// Get the PeiDebug Data Base Address
	PeiDebugDataBaseAddress = GetPeiDebugDataBaseaddress1 ();
    InitializeDebugAgent (DEBUG_AGENT_INIT_XHCI_PEI, &Status, NULL);
    pData = (PEIDebugData_T *)PeiDebugDataBaseAddress;
    SerData = (SerXPortGblData_T *)(pData->XportLocalVar);
    
    if(SerData->m_IsHostConnected) {
    	//Initial Breakpoint for USB 3.0 Debugger
    	__debugbreak();
    }
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
