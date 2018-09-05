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
// $Header: /AptioV/BIN/Debugger/DxeDebugger/PeCoffExtraActionLibDebug/PeCoffHelp.c 1     11/02/12 10:18a Sudhirv $
//
// $Revision: 1 $
//
// $Date: 11/02/12 10:18a $
//*****************************************************************
//*****************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:		PeCoffHelp.c
//
// Description:	This file contains variable and function declarations common to PEI and DXE data
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Efi.h>
#include <Base.h>
#include "Pei.h"
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Guid/HobList.h>

#include <Library/AMIPeiGUIDs.h>

#ifndef	EFIx64
#include <Library/AMIpeidebug.h>
#else
#include <Library/AMIpeidebugX64.h>
#include <Library/HobLib.h>
#endif

#ifdef DbgModuleLocator_SUPPORT
volatile UINT8 gDbgModuleLocator = DbgModuleLocator_SUPPORT;
#else
volatile UINT8 gDbgModuleLocator = 0;
#endif

EFI_GUID  mPeiDebugDataGuidPeCoff = PEI_DBGSUPPORT_DATA_GUID;
EFI_GUID  mDxeDebugDataGuidPeCoff = DXE_DBG_DATA_GUID;
EFI_GUID  mPeiDbgBasePpiGuidPeCoff = EFI_PEI_DBG_BASEADDRESS_PPI_GUID;

INT8 CompareGuid(EFI_GUID *G1, EFI_GUID *G2);
#ifdef	EFIx64
extern EFI_SYSTEM_TABLE 	*pST;
VOID * GetEfiConfigurationTable(
	IN EFI_SYSTEM_TABLE *SystemTable,
	IN EFI_GUID			*Guid
	);
EFI_STATUS FindNextHobByGuid(IN EFI_GUID *Guid, IN OUT VOID **Hob);
#endif




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
