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
//
// $Header: /AptioV/BIN/AMIDebugRx/AMIDebugAgent/CommonDebugAgent.c 4     1/07/15 9:43a Jekekumarg $
//
// $Revision: 4 $
//
// $Date: 1/07/15 9:43a $
//*****************************************************************
//
// Revision History
// ----------------
// $Log: /AptioV/BIN/AMIDebugRx/AMIDebugAgent/CommonDebugAgent.c $
// 
// 4     1/07/15 9:43a Jekekumarg
// Added Warning Message for SMM EXIT when SMM Read Save state resgister
// fails
// as a part of the EIP-186481
// 
//
//*****************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	CommonDebugAgent.c
//
// Description:	This file Contains variable and macro definitions common to pei,dxe and smm phase
//
//<AMI_FHDR_END>
//**********************************************************************
#include "Token.h"

#ifndef REDIRECTION_ONLY_MODE
#define REDIRECTION_ONLY_MODE 0
#endif

#ifndef PeiDebugger_SUPPORT
#define PeiDebugger_SUPPORT 0
#endif

#ifndef GENERIC_USB_CABLE_SUPPORT
#define GENERIC_USB_CABLE_SUPPORT 0
#endif

#ifndef SMM_DEBUG_SUPPORT
#define SMM_DEBUG_SUPPORT 0
#endif

UINT8 	PMModeEnabled = 0;
UINT8 	gSMMEntryBreak = 0;
UINT8 	gSMMExitBreak = 0;
UINT8 	gSMMRuntime = 0;
UINT8	gSMMSendModuleInfo = 0;
UINTN 	SMMDxeDbgDataBaseAddr = 0;
UINTN 	mSavedDebugRegisters[6];
CHAR8 	mErrorMsgVersionAlert[] = "\rThe AMIDebugRxPkg you are using requires a newer version of the AptioV Debugger Tool.\r\n";

volatile UINTN gSmmDebuggingSupport = SMM_DEBUG_SUPPORT;


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   PeiDebuggerEnabled
//
//  Description: returns the status of PeiDebugger_SUPPORT
//
//  Input:	void
//
//  Output: 
//	Boolean -based on result
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN PeiDebuggerEnabled()
{
#ifdef PeiDebugger_SUPPORT
	return PeiDebugger_SUPPORT;
#else
	return 0;
#endif
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
