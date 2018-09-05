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
// $Header: /AptioV/BIN/AMIDebugRx/DebugStatusCodeLib/DbgrHelp.c 1     11/02/12 10:14a Sudhirv $
//
// $Revision: 1 $
//
// $Date: 11/02/12 10:14a $
//*****************************************************************
//*****************************************************************
//**********************************************************************
// Revision History
// ----------------
// $Log: /AptioV/SRC/AMIDebugRx/DebugStatusCodeLib/DbgrHelp.c $
// 
//
//*****************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	DbgrHelp.c
//
// Description:	File containing the globals for CAR base address & size.
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Efi.h>
#include <AmiPeiLib.h>

#ifndef EFIx64
#include <Library/AMIpeidebug.h>
#else
#include <Library/AMIpeidebugX64.h>
#endif
#include <Library/AMIPeiGUIDs.h>
#include <Library/BaseMemoryLib.h>

#include "Token.h"

//**********************************************************************
#ifndef AMI_DEBUG_RX_IN_S3_SUPPORT
#define AMI_DEBUG_RX_IN_S3_SUPPORT	0
#endif

EFI_GUID  mPeiDebugDataGuidSts = PEI_DBGSUPPORT_DATA_GUID;
EFI_GUID  mPeiDbgBasePpiGuidSts = EFI_PEI_DBG_BASEADDRESS_PPI_GUID;
UINTN AMI_PEIDEBUGGER_DS1_SIZE = 0x500;
volatile UINT16 gS3ResumeSupport = AMI_DEBUG_RX_IN_S3_SUPPORT;

VOID CPULib_SaveIdt(DESCRIPTOR_TABLE);


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

