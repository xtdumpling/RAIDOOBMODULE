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
// $Header: /AptioV/BIN/SerialDebugger/AMIDebugAgent/CommonDebugAgent.c 2     2/20/15 10:20a Jekekumarg $
//
// $Revision: 2 $
//
// $Date: 2/20/15 10:20a $
//*****************************************************************
//
// Revision History
// ----------------
// $Log: /AptioV/BIN/SerialDebugger/AMIDebugAgent/CommonDebugAgent.c $
// 
// 2     2/20/15 10:20a Jekekumarg
// [TAG]			EIP205866
// [Category]   	Improvement
// [Description]	New internal label (INT)5.004_SerialDebugger_3.01.0017
// for Aptio5 Serial Debugger Module
// [Files]			AMIDebugAgent.sdl
// PeiAMIDebugAgent\PeiAMIDebugAgentLib.lib
// PeiAMIDebugAgent\PeiAMIDebugAgentLibx64.lib
// DxeAMIDebugAgent\DxeAMIDebugAgentLibx86.lib
// DxeAMIDebugAgent\DxeAMIDebugAgentLibx64.lib
// SmmAMIDebugAgent\SmmAMIDebugAgentLibx86.lib
// SmmAMIDebugAgent\SmmAMIDebugAgentLibx64.lib
// CommonDebugAgent.c
// 
// 1     12/31/14 6:37p Sudhirv
// Updated for AMI Debug for UEFI 3.01.0016 Label
// 
//
//*****************************************************************


//*****************************************************************
//<AMI_FHDR_START>
//
// Name:	CommonDebugAgent.c
// Description:This file Contains variable and macro definitions common to pei,dxe and smm phase
//
//
//<AMI_FHDR_END>
//**********************************************************************
#include "token.h"

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

UINTN	SMMExitBreakpoint = 0;
UINT8   SMMExitBPOpcode = 0;
UINT8 	PMModeEnabled = 0;
UINT8 	gSMMEntryBreak = 0;
UINT8 	gSMMExitBreak = 0;
UINT8 	gSMMRuntime = 0;
UINTN 	SMMDxeDbgDataBaseAddr = 0;
UINT8 	SMMExitMessage[] = "AMIDebugger: Unable to Get SMM Save State registers hence SMM Exit Breakpoint can't be set...\nTo set SMM Exit BreakPoint manually go to the return address and Put Breakpoint...\n";
UINTN 	mSavedDebugRegisters[6];
CHAR8 	mErrorMsgVersionAlert[] = "\rThe AMIDebugRxPkg you are using requires a newer version of the AptioV Debugger Tool.\r\n";

volatile UINTN RedirectionOnlyEnabled = REDIRECTION_ONLY_MODE;
//volatile UINTN PeiDebuggerEnabled = PeiDebugger_SUPPORT;
volatile UINTN gSmmDebuggingSupport = SMM_DEBUG_SUPPORT;



//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	PeiDbgIsS3Hook
//
// Description:This function returns the status of pei debugger enabled or disbled
//
// Input: VOID
//
// Output: BOOLEAN
//
//--------------------------------------------------------------------
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
