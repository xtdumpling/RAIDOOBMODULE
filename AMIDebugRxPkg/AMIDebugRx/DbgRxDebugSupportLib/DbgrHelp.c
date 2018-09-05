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
// $Header: /AptioV/BIN/AMIDebugRx/DbgRxDebugSupportLib/DbgrHelp.c 1     11/02/12 10:14a Sudhirv $
//
// $Revision: 1 $
//
// $Date: 11/02/12 10:14a $
//*****************************************************************
//*****************************************************************
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/AMIDebugRx/DbgRxDebugSupportLib/DbgrHelp.c $
// 
//
//**********************************************************************


//**********************************************************************
//<AMI_FHDR_START>
//
// Name:		DbgHelp.c
//
// Description:	File containing the globals for CAR base address & size.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef EFIx64
#include <Library/AMIpeidebug.h>
#else
#include <Library/AMIpeidebugX64.h>
#endif
#include <AmiHobs.h>
#include <Library/AMIPeiGUIDS.h>
#include "Token.h"

#ifndef USB_DEBUG_TRANSPORT
#define USB_DEBUG_TRANSPORT	0
#endif

#ifndef AMI_DEBUG_RX_IN_S3_SUPPORT
#define AMI_DEBUG_RX_IN_S3_SUPPORT	0
#endif

volatile UINT16 gS3ResumeSupport = AMI_DEBUG_RX_IN_S3_SUPPORT;
//volatile UINTN USB_DEBUGGER_ENABLED = USB_DEBUG_TRANSPORT;

const UINTN	AMI_PEIDEBUGGER_DS_BASEADDRESS 	= 0;
const UINTN	AMI_PEIDEBUGGER_DS_SIZE			= 0x2048;

EFI_GUID  mPeiDebugDataGuidDbgSup = PEI_DBGSUPPORT_DATA_GUID;
EFI_GUID  mDxeDebugDataGuidDbgSup = DXE_DBG_DATA_GUID;
EFI_GUID  mPeiDbgBasePpiGuidDbgSup = EFI_PEI_DBG_BASEADDRESS_PPI_GUID;
EFI_GUID  mPeiDbgDbgrIfcGuidDbgSup = PEI_DBGR_REPORTSTATUSCODE_GUID;

UINT8 GetEndOfPeiSupport()
{
	if(DBG_ENDOFPEI_CALLBACK_SUPPORT == 1)
		return 1;
	else
		return 0;
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


