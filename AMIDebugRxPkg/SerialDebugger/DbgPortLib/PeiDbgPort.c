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
// $Header: /AptioV/SRC/AMIDebugRx/DbgRxPortLib/PeiDbgPort.c 3     11/19/12 7:44a Sudhirv $
//
// $Revision: 3 $
//
// $Date: 11/19/12 7:44a $
//*****************************************************************
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/AMIDebugRx/DbgRxPortLib/PeiDbgPort.c $
// 
// 3     11/19/12 7:44a Sudhirv
// [TAG]     EIP106836
// [Category] Function Request
// [Severity] Normal
// [Symptom] Make changes to AMIDebugRx\Debugger for chipset porting
// through ELink
// [Solution] Issue is fixed, Now chipset porting is done by ELink.
// 
// 6     8/10/12 1:34p Sudhirv
// Updated
//
// 5     8/09/12 10:15p Sudhirv
// Updated
//
// 4     8/09/12 12:16p Sudhirv
// After making SBDebugPorting Modulepart
//
// 3     8/03/12 6:32p Sudhirv
// Updated before making binaries.
//
// 1     7/15/12 8:12p Sudhirv
// AMIDebugRx Module for Aptio 5
//
//*****************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:		PeiDbgPort.c
//
// Description:This file contains code to intialize com port in pei
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Library/PciLib.h>
#include <Token.h>
#ifndef EFIx64
#include <Library\AMIPeiDebug.h>
#else
#include <Library\AMIPeiDebugX64.h>
#endif
#include <AmiDebugPort.h>
#include <Library/SerialPortLib.h>
#include <Library\AMIpeidebug.h>
#include "PEIDbgPortELink.h"

extern EFI_STATUS SBPEIDBG_Initialize(PEI_DBG_PORT_INFO *DebugPort);

void InitPeiDebugPortInterface(PEI_DBG_PORT_INFO *,UINTN, UINTN);
void Initialise(PEI_DBG_PORT_INFO *);
void DBGWriteChkPort(UINT8	DbgChkPoint);

extern PEI_INIT_FUNCTION PEI_DBG_INIT_LIST EndOfInitList;
PEI_INIT_FUNCTION* PeiDebuggerInitList [] = {PEI_DBG_INIT_LIST NULL};


//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	PeiDbgIsS3Hook
//
// Description:	Dummy Function to return FALSE.
//
// Input:		void
//
// Output:		UINT8
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 PeiDbgIsS3Hook()
{
	return FALSE;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	ExecutePeiDebuggerInitList()
//
// Description:	Internal Helper function to invoke ELINKS.
//
// Input:		IN OUT PEI_DBG_PORT_INFO*
//
// Output:		void
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
ExecutePeiDebuggerInitList(
	IN OUT	PEI_DBG_PORT_INFO	*DebugPort
)
{
	PeiDebuggerInitList[0](DebugPort);
	//SBPEIDBG_Initialize(DebugPort);
}
void Stall (UINTN);
//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	InitPeiDbgPort
//
// Description:	Entrypoint for the PeiDbgPort, the module init the specific
//				chipset.
//
// Input:		UINTN	
//
// Output:		void
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void InitPeiDbgPort(UINTN	DebugDataBaseAddr)
{
	PEI_DBG_PORT_INFO	DebugPort;
	UINTN				PeiDbgSetupFlag;
	RETURN_STATUS		Status = 0;
	UINT8       		Buffer[]="SerialDebugger: SerialPort Initialized Successfully\r\n";
#ifndef EFIx64
	PEIDebugData_T		*pData = (PEIDebugData_T *)DebugDataBaseAddr;
	SerXPortGblData_T	*SerData = (SerXPortGblData_T *)pData->XportLocalVar;
#else
	PEIDebugData_Tx64	*pData = (PEIDebugData_Tx64 *)DebugDataBaseAddr;
	SerXPortGblData_Tx64	*SerData = (SerXPortGblData_Tx64 *)pData->XportLocalVar;
#endif

#if	PEI_DEBUG_SUPPORT_AFTER_MEMORY
	PeiDbgSetupFlag = 1;
#else
	PeiDbgSetupFlag = 0;
#endif
	SerData->m_IsHostConnected = 0;

	Initialise(&DebugPort);
#ifndef EFIx64
	//Program the TCO IO to avoid rebooting of the hardware
	ExecutePeiDebuggerInitList(&DebugPort);
#endif
	
	Status = SerialPortInitialize ();

	//To Let the users know the Serial Port status is ok or not
	if(!EFI_ERROR(Status)) {
		if(SEND_SERIAL_INIT_MSG_SUPPORT) {
			Stall(10*1000);
			SerialPortWrite(Buffer,0x35);
		}
	}
	
	InitPeiDebugPortInterface(&DebugPort,PeiDbgSetupFlag,DebugDataBaseAddr);//IDTBASEADDR);
	SerData->m_IsPeiDbgIsNotS3 = 1;
}


//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	NeedReinitializeHardwareDbg
//
// Description:	Procedure to determine if Re-Initialization of the USB
//				Debug port is required or not.
//
// Input:		IN UINTN 
//
// Output:		BOOLEAN	(True = Re-Init Required)
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN NeedReinitializeHardwareDbg(IN UINTN Context)
{
	return FALSE;
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
