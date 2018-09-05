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
// $Header: /AptioV/SRC/SerialDebugger/Include/Library/peidebug.h 2     1/23/14 5:44p Sudhirv $
//
// $Revision: 2 $
//
// $Date: 1/23/14 5:44p $
//*****************************************************************
//*****************************************************************
//
//	peidebug.h
//
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/SerialDebugger/Include/Library/peidebug.h $
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

#include "token.h"
#include <PEI.h>

#define PEIDEBUGDISPATCH_GUID \
  { 0x77277a7d, 0xfc11, 0x4d29, 0x9e, 0xb5, 0x0a, 0x78, 0x8c, 0x78, 0x58, 0x56 }

#include <Efi.h>
void DebugSecStartup( EFI_SEC_PEI_HAND_OFF   *SecCoreData,
					  EFI_PEI_PPI_DESCRIPTOR *PpList);

#pragma pack (push, pei_struct_data_pack, 1)

typedef struct {
  EFI_GUID                      PEIDebugDispatchGuid;
  void 							(*SecStartup) (EFI_SEC_PEI_HAND_OFF*,EFI_PEI_PPI_DESCRIPTOR*);
  void							(*PEIDebugIDT) (UINTN);
  void							(*PEIDebugSIO) (UINTN);
  void							(*PEIDebugXPORT) ();
  void							(*PEIDebugDBGR) ();
} PEIDEBUG_DISPATCH_TABLE;

#pragma pack (pop, pei_struct_data_pack)

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
