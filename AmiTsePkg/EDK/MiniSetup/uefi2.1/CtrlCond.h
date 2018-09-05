//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2012, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Uefi2.1/CtrlCond.h $
//
// $Author: Rajashakerg $
//
// $Revision: 7 $
//
// $Date: 2/01/12 4:59a $
//
//*****************************************************************//
/** @file CtrlCond.h
    Header File

**/
//*************************************************************************

//---------------------------------------------------------------------------
#include "minisetup.h"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#define COND_NONE			0x0
#define COND_SUPPRESS		0x1
#define COND_HIDDEN			0x2
#define COND_GRAYOUT		0x3
#define COND_INCONSISTENT	0x4
#define COND_NOSUBMIT		0x5
#define COND_WARN_IF		0x6
//---------------------------------------------------------------------------


// functions
//UINT8 CheckControlCondition(UINT32 ControlAccess, UINT8  *CtrlPtr, UINT32 CtrlVar, UINT32 CtrlVar2, UINT8 *CondPtr);
UINT8 CheckControlCondition( CONTROL_INFO *controlInfo );
BOOLEAN CheckInconsistence( PAGE_INFO *pPageInfo );
UINT8 CheckOneOfOptionSuppressIf(CONTROL_INFO *ParentCtrlInfo, UINT8* CtrlCondPtr, UINT8* CtrlPtr);
BOOLEAN CheckNoSubmitIf( VOID );
BOOLEAN DrawMessageBox(CONTROL_INFO *pControlInfo, UINT16 Title, EFI_STRING_ID  Error);
UINT8 OEMCheckControlCondition(UINT8 FinalCond, CONTROL_INFO *controlInfo);

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
