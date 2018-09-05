//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/password.h $
//
// $Author: Arunsb $
//
// $Revision: 1 $
//
// $Date: 12/08/11 4:54a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file password.h
    Header file for code to handle password operations

**/

#ifndef _PASSWORD_H_
#define	_PASSWORD_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"  
#endif

BOOLEAN _PasswordProtocolInit( VOID );
BOOLEAN PasswordBootMode( VOID );
UINT32 PasswordCheckInstalledVarIndex( UINT32 VarIndex );
UINT32 PasswordAuthenticate( CHAR16 *Password );
BOOLEAN PasswordAuthenticateVarIndex( UINT32 VarIndex, CHAR16 *Password );
CHAR16 *PasswordUpdate( CHAR16 *Password, UINTN Size );
VOID PasswordCommitChanges( BOOLEAN SaveChanges );

#endif /* _PASSWORD_H_ */


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
