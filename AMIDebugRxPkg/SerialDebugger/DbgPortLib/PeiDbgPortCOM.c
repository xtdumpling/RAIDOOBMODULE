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
// $Header: /AptioV/SRC/SerialDebugger/DbgPortLib/PeiDbgPortCOM.c 2     1/23/14 5:44p Sudhirv $
//
// $Revision: 2 $
//
// $Date: 1/23/14 5:44p $
//*****************************************************************
//*****************************************************************
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/SerialDebugger/DbgPortLib/PeiDbgPortCOM.c $
// 
// 2     1/23/14 5:44p Sudhirv
// Updated Copyright Info
// 
// 1     9/21/13 4:28a Sudhirv
// Added for AMI Debug for UEFI 3.01.0006 Label.
//
//*****************************************************************

//*****************************************************************
//<AMI_FHDR_START>
//
// Name:	PeiDbgPortCOM.c
// Description:	This file contains code to intitialize com port interface
//
//
//<AMI_FHDR_END>
//**********************************************************************

#ifdef ALASKA_SUPPORT
#include <Efi.h>
#else
/*#include "Tiano.h"
#include "SimpleCpuIoLib.h"*/
#endif
#include "AmiDebugPort.h"
#ifndef EFIx64
#include <Library\AMIPeiDebug.h>
#else
#include <Library\AMIPeiDebugX64.h>
#endif
//#include <Library\AMIPeiDebug.h>

extern UINTN	AMI_PEIDEBUGGER_DS_BASEADDRESS;
extern UINTN 	gDbgWriteIO80Support;

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	Initialise
//
// Description:	Internal Helper function.
//
// Input:		PEI_DBG_PORT_INFO *,
//
// Output:		void
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void Initialise(PEI_DBG_PORT_INFO *DebugPort)
{
	DebugPort->SerialPort.COMBaseAddr = 0;
	DebugPort->SerialPort.SIO_COM_LDN = 0;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	InitPeiDebugPortInterface
//
// Description:	Internal Helper function.
//
// Input:		PEI_DBG_PORT_INFO *,UINTN , UINTN 
//
// Output:		void
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void InitPeiDebugPortInterface(PEI_DBG_PORT_INFO *DebugPort,UINTN SetupFlag, UINTN IDTBaseAddress)
{
#ifndef EFIx64
	PEIDebugData_T		*pData = (PEIDebugData_T *)IDTBaseAddress;
	SerXPortGblData_T	*PeiXportData = (SerXPortGblData_T *)pData->XportLocalVar;
	DbgGblData_T		*DbgData = (DbgGblData_T *)(pData->DbgrLocalVar);
#else
	PEIDebugData_Tx64		*pData = (PEIDebugData_Tx64 *)IDTBaseAddress;
	SerXPortGblData_Tx64	*PeiXportData = (SerXPortGblData_Tx64 *)pData->XportLocalVar;
	DbgGblData_Tx64		*DbgData = (DbgGblData_Tx64 *)(pData->DbgrLocalVar);
#endif

	DbgData->m_FlagToDebugAfterMemory = (UINT8)SetupFlag;

	//PeiXportData->m_BaseAddr = (UINT16)PcdGet64 (PcdSerialRegisterBase);;
	//PeiXportData->m_SIO_LDN_UART = 0x10;

	switch((UINT16)PcdGet64 (PcdSerialRegisterBase)){
		case 0:
			IoWrite8(0x80,0xCF);	//err code for invalid COM base address
			while(1);
		case 0xffff:
			IoWrite8(0x80,0xD1);	//err code for invalid CAR
			while(1);
		default:
			break;
	}
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
