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
// $Header: /AptioV/BIN/AMIDebugRx/DbgRxDbgrLib/DbgrHdr.c 1     11/02/12 10:14a Sudhirv $
//
// $Revision: 1 $
//
// $Date: 11/02/12 10:14a $
//*****************************************************************
//*****************************************************************
// Revision History
// ----------------
// $Log: /AptioV/BIN/AMIDebugRx/DbgRxDbgrLib/DbgrHdr.c $
// 
// 1     11/02/12 10:14a Sudhirv
// AMIDebugRx eModule for AMI Debug for UEFI 3.0
// 
// 1     8/08/12 3:12p Sudhirv
// AMIDebugRx 3.0 Binary Module
// 
// 2     8/03/12 6:31p Sudhirv
// Updated before making binaries.
//
// 1     7/15/12 8:12p Sudhirv
// AMIDebugRx Module for Aptio 5
//
//*****************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	DbgrHdr.c
//
// Description:	File containing the globals for CAR base address & size.
//				Also the code which make use of this so that it can be
//				integrated as it is Debugger eModule of binary.
//
//<AMI_FHDR_END>
//**********************************************************************

#include "Pei.h"
#ifndef EFIx64
#include <Library/AMIpeidebug.h>
#else
#include <Library/AMIpeidebugX64.h>
#endif
#include "Token.h"
//**********************************************************************

#define Code_Start_Addr			0xffffffff - FLASH_SIZE
#define Code_End_Addr			0xffffffff

#ifndef CAR_BASE_ADDRESS
UINTN CAR_Start_Addr = 0;
UINTN CAR_End_Addr = 0;
#else
#ifndef CAR_TOTAL_SIZE
UINTN CAR_End_Addr = 0;
#else
UINTN CAR_End_Addr = CAR_BASE_ADDRESS + CAR_TOTAL_SIZE;
#endif
UINTN CAR_Start_Addr = CAR_BASE_ADDRESS;
#endif

#ifndef SMM_BASE_SAVE_STATE_OFFSET
#define SMM_BASE_SAVE_STATE_OFFSET 0
#endif
#ifndef MAX_SMM_SAVE_STATE_SIZE
#define MAX_SMM_SAVE_STATE_SIZE 0
#endif
#ifndef SMM_BSP_BASE
#define SMM_BSP_BASE 0
#endif
#ifndef SW_SMI_IO_ADDRESS
#define SW_SMI_IO_ADDRESS 0
#endif
#ifndef TSEG_SIZE
#define TSEG_SIZE 0
#endif

//For SMM Outcontext Support
volatile UINT16 gSMIIOAddress = SW_SMI_IO_ADDRESS;
volatile UINTN gSMMBaseSaveState = SMM_BASE_SAVE_STATE_OFFSET;
volatile UINTN gSMMSaveStateSize = MAX_SMM_SAVE_STATE_SIZE;
volatile UINTN gSMMBspBase = SMM_BSP_BASE;
volatile UINTN gTsegSize = TSEG_SIZE;
//**********************************************************************

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

