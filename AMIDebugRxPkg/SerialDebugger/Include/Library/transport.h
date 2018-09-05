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
// $Header: /AptioV/SRC/SerialDebugger/Include/Library/transport.h 2     1/23/14 5:44p Sudhirv $
//
// $Revision: 2 $
//
// $Date: 1/23/14 5:44p $
//*****************************************************************
//*****************************************************************
//
//	transport.h
//
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/SerialDebugger/Include/Library/transport.h $
// 
// 2     1/23/14 5:44p Sudhirv
// Updated Copyright Info
// 
// 1     1/23/14 4:37p Sudhirv
// EIP I51926 - Serial Debugger - make Serial Debugger independent of
// AMIDebugRx and Debuger
// 
// 1     11/02/12 10:07a Sudhirv
// AMIDebugRxPkg modulepart for AMI Debug for UEFI 3.0
// 
// 2     8/03/12 6:32p Sudhirv
// Updated before making binaries.
//
// 1     7/15/12 8:12p Sudhirv
// AMIDebugRx Module for Aptio 5
//
//*****************************************************************
#ifndef EFIx64
#include <Library\AMIPeiDebug.h>
#else
#include <Library\AMIPeiDebugX64.h>
#endif

#ifndef EFIx64
UINT16	Initialize( AMI_PEI_TRANSPORT_PROTOCOL *This );
UINT16	Restore( AMI_PEI_TRANSPORT_PROTOCOL *This );
UINT16	Send( AMI_PEI_TRANSPORT_PROTOCOL *This, VOID *Buffer, UINT16 BufSize );
UINT16	SendNoAck( AMI_PEI_TRANSPORT_PROTOCOL *This, VOID *Buffer, UINT16 BufSize );
UINT16	Receive( AMI_PEI_TRANSPORT_PROTOCOL *This, VOID *Buffer, UINT16 *BufSize, UINT16 Timeout );
UINT16	SyncPacketNumber( AMI_PEI_TRANSPORT_PROTOCOL *This );
UINT16  RegisterHandler( VOID *This, VOID *Handler );
VOID	Callback( VOID *data );
void	CopyConsoleBufIntoPktBuf(VOID*, VOID*, UINTN);
RETURN_STATUS	ConsoleIn( AMI_PEI_TRANSPORT_PROTOCOL *This, VOID *Buffer, UINTN *BufSize );
UINT16	SendExptn( AMI_PEI_TRANSPORT_PROTOCOL *This,UINT8 ExptnType, VOID *Buffer, UINT16 BufSize );
RETURN_STATUS	ReInitialize( AMI_PEI_TRANSPORT_PROTOCOL *This);
RETURN_STATUS	SmmReInitialize( AMI_PEI_TRANSPORT_PROTOCOL *This);
RETURN_STATUS	UpdateTargetState( AMI_PEI_TRANSPORT_PROTOCOL *This);
#else
UINT16	Initialize( AMI_PEI_TRANSPORT_PROTOCOLx64 *This );
UINT16	Restore( AMI_PEI_TRANSPORT_PROTOCOLx64 *This );
UINT16	Send( AMI_PEI_TRANSPORT_PROTOCOLx64 *This, VOID *Buffer, UINT16 BufSize );
UINT16	SendNoAck( AMI_PEI_TRANSPORT_PROTOCOLx64 *This, VOID *Buffer, UINT16 BufSize );
UINT16	Receive( AMI_PEI_TRANSPORT_PROTOCOLx64 *This, VOID *Buffer, UINT16 *BufSize, UINT16 Timeout );
UINT16	SyncPacketNumber( AMI_PEI_TRANSPORT_PROTOCOLx64 *This );
UINT16  RegisterHandler( VOID *This, VOID *Handler );
VOID	Callback( VOID *data );
void	CopyConsoleBufIntoPktBuf(VOID*, VOID*, UINTN);
RETURN_STATUS	ConsoleIn( AMI_PEI_TRANSPORT_PROTOCOLx64 *This, VOID *Buffer, UINTN *BufSize );
UINT16	SendExptn( AMI_PEI_TRANSPORT_PROTOCOLx64 *This,UINT8 ExptnType, VOID *Buffer, UINT16 BufSize );
RETURN_STATUS	ReInitialize( AMI_PEI_TRANSPORT_PROTOCOLx64 *This);
RETURN_STATUS	SmmReInitialize( AMI_PEI_TRANSPORT_PROTOCOLx64 *This);
RETURN_STATUS	UpdateTargetState( AMI_PEI_TRANSPORT_PROTOCOLx64 *This);
#endif

#include "ser_acc.h"

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
