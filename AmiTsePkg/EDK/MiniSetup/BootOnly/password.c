//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/password.c $
//
// $Author: Rajashakerg $
//
// $Revision: 2 $
//
// $Date: 12/08/11 12:46p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file password.c
    This file contains code to handle password operations

**/

#include "minisetup.h"

/**
    function to check the password type

    @param void

    @retval void

**/
UINT32 PasswordCheckInstalledLocal(VOID)
{
	UINT32	Installed = AMI_PASSWORD_NONE;
	UINT8 *setup = NULL;
	AMITSESETUP *mSysConf = NULL;
	UINTN VarSize = 0;
	CHAR16 *TestPassword;
	
	TestPassword = EfiLibAllocateZeroPool((TsePasswordLength + 1)*sizeof(CHAR16));
	
	GetAMITSEVariable(&mSysConf,&setup,&VarSize);

	if ( ( mSysConf != NULL ) && ( VarSize == sizeof(AMITSESETUP) ) )
	{
		MemSet( TestPassword, (TsePasswordLength + 1)*sizeof(CHAR16), 0 );
		if ( ! EfiCompareMem( TestPassword, mSysConf->UserPassword, TsePasswordLength * sizeof(CHAR16) ) )
			Installed |= AMI_PASSWORD_USER;
		if ( ! EfiCompareMem( TestPassword, mSysConf->AdminPassword, TsePasswordLength * sizeof(CHAR16) ) )
			Installed |= AMI_PASSWORD_ADMIN;
		Installed ^= AMI_PASSWORD_ANY;
	}

	if(setup)
		MemFreePointer( (VOID **)&setup );
	else
		MemFreePointer( (VOID **)&mSysConf );

	MemFreePointer( (VOID **)&TestPassword);

	return Installed;
}

/**
    function to check the authentication of the password

    @param Password 

    @retval password type

**/
BOOLEAN IsPasswordSupportNonCaseSensitive ();
UINT32 PasswordAuthenticateLocal( CHAR16 *Password )
{
	UINT32 PasswordType = AMI_PASSWORD_NONE;
	UINT8 *setup = NULL;
	AMITSESETUP *mSysConf = NULL;
	UINTN VarSize = 0;
	CHAR16 *EncPass;
	UINTN	ii;

	GetAMITSEVariable(&mSysConf,&setup,&VarSize);
	
	if ( ( mSysConf == NULL ) || ( VarSize != sizeof(AMITSESETUP) ) )
		return PasswordType;

	EncPass = EfiLibAllocateZeroPool((TsePasswordLength + 1)*sizeof(CHAR16));

	MemSet( EncPass, (TsePasswordLength + 1)*sizeof(CHAR16), 0 );
	EfiStrCpy( EncPass, Password );
	if (IsPasswordSupportNonCaseSensitive ()) //If admin/user passwords are not encoded then case pbm will arose so avoiding that
	{
		for (ii = 0; ii < TsePasswordLength; ii++)
			Password [ii] = ((Password [ii]>=L'a')&&(Password [ii]<=L'z'))?(Password [ii]+L'A'-L'a'):Password [ii];
	}

	PasswordEncodeHook( EncPass, TsePasswordLength*sizeof(CHAR16));

	if ( ( ! EfiCompareMem( Password, mSysConf->UserPassword, TsePasswordLength * sizeof(CHAR16) ) ) ||
		 ( ! EfiCompareMem( EncPass, mSysConf->UserPassword, TsePasswordLength * sizeof(CHAR16) ) ) )
	{
		PasswordType = AMI_PASSWORD_USER;
	}

	if ( ( ! EfiCompareMem( Password, mSysConf->AdminPassword, TsePasswordLength * sizeof(CHAR16) ) ) ||
		 ( ! EfiCompareMem( EncPass, mSysConf->AdminPassword, TsePasswordLength * sizeof(CHAR16) ) ) )
	{
		PasswordType = AMI_PASSWORD_ADMIN;
	}

	if(setup)
		MemFreePointer( (VOID **)&setup );
	else
		MemFreePointer( (VOID **)&mSysConf );

	MemFreePointer( (VOID **)&EncPass );

	return PasswordType;
}

/**
    function to ubdate the password

    @param Password , UINTN Size

    @retval updated password

**/
CHAR16 *PasswordUpdateLocal( CHAR16 *Password, UINTN Size )
{
	CHAR16 *Buffer = NULL;

    if ( Password[0] == L'\0' )
        return Password;

	Buffer = EfiLibAllocateZeroPool( Size );
	if ( Buffer == NULL )
		return Buffer;

	MemCopy( Buffer, Password, Size );
	PasswordEncodeHook( Buffer, Size );

	return Buffer;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
