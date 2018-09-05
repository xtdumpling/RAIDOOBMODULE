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
// $Header: /AptioV/BIN/SerialDebugger/DbgHostStatusLib/CommonDebug.c 2     2/20/15 10:34a Jekekumarg $
//
// $Revision: 2 $
//
// $Date: 2/20/15 10:34a $
//
//*********************************************************************
// Revision History
// ----------------
// $Log: /AptioV/BIN/SerialDebugger/DbgHostStatusLib/CommonDebug.c $
// 
// 2     2/20/15 10:34a Jekekumarg
// [TAG]			EIP205866
// [Category]   	Improvement
// [Description]	New internal label (INT)5.004_SerialDebugger_3.01.0017
// for Aptio5 Serial Debugger Module
// [Files]			PeiDbgHostStatusLib.lib
// PeiDbgHostStatusLibx64.lib
// DxeDbgHostStatusLibx86.lib
// DxeDbgHostStatusLibx64.lib
// SmmDbgHostStatusLibx86.lib
// SmmDbgHostStatusLibx64.lib
// CommonDebug.c
// 
// 1     12/31/14 6:37p Sudhirv
// Updated for AMI Debug for UEFI 3.01.0016 Label
// 
//
//*********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          CommonDebug.C
//
// Description:   Common Debug definitions
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include "efi.h"
#include "Pei.h"

//#include "misc.h"
#ifndef	EFIx64
#include <Library\AMIPeiDebug.h>
#else
#include <Library\AMIPeiDebugX64.h>
#endif
#include <Library\AMIPeiGUIDs.h>

#include "token.h"

#ifdef REDIRECTION_ONLY_MODE
#if REDIRECTION_ONLY_MODE
volatile UINTN gRedirectionOnlyEnabled = 1;
#else
volatile UINTN gRedirectionOnlyEnabled = 0;
#endif
#endif

#ifdef DBG_PERFORMANCE_RECORDS
UINTN gDbgPerformanceRecords = DBG_PERFORMANCE_RECORDS;
#else
UINTN gDbgPerformanceRecords = 0;
#endif

#ifdef DBG_WRITE_IO_80_SUPPORT
volatile UINTN gDbgWriteIO80Support = DBG_WRITE_IO_80_SUPPORT;
#else
volatile UINTN gDbgWriteIO80Support = 0;
#endif

#ifdef SERIAL_SEC_DBG_SUPPORT
volatile UINT8 gSecDbgSupport = SERIAL_SEC_DBG_SUPPORT;
#else
volatile UINT8 gSecDbgSupport = 0;
#endif

#ifndef GENERIC_USB_CABLE_SUPPORT
#define GENERIC_USB_CABLE_SUPPORT 0
#endif

// Load Fv Support
UINTN gFvMainBase = FV_MAIN_BASE;
UINTN gFvMainBlocks = FV_MAIN_BLOCKS;
UINTN gFvBBBlocks = FV_BB_BLOCKS;
UINTN gBlockSize = FLASH_BLOCK_SIZE;

volatile UINTN gGenericUsbSupportEnabled = GENERIC_USB_CABLE_SUPPORT;

EFI_GUID  mPeiDbgBasePpiGuid = EFI_PEI_DBG_BASEADDRESS_PPI_GUID;

EFI_GUID  mDxeDbgDataGuid = DXE_DBG_DATA_GUID;
INT8 CompareGuid(EFI_GUID *G1, EFI_GUID *G2);

UINTN DebugDataBaseAddress = 0;
UINTN SMMDebugDataBaseAddress = 0;

BOOLEAN CheckForHostConnectedinPEI (EFI_PEI_SERVICES **PeiServices);

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	GetPCIBaseAddr
//
// Description:	Returns the PCIBase Address
//
// Input:		VOID
//
// Output:		UINTN 
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>

UINTN  GetPciExBaseAddr()
{
	return (UINTN) PcdGet64 (PcdPciExpressBaseAddress);
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
