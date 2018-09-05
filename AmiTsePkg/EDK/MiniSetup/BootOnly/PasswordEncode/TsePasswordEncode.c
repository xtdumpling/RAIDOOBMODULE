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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/PasswordEncode/PasswordEncode.c $
//
/** @file TsePasswordEncode.c
    This file contains code to handle password encoding feature

**/

/**
    Encodes the input string

    @param Password : Password array to be encrypted. Encryped
        password is returned in the same array.
        MaxSize : Max size of Password

    @retval VOID

**/
BOOLEAN IsPasswordSupportNonCaseSensitive();
VOID TsePasswordEncodeLocal ( CHAR16 *Password, UINTN MaxSize)
{
	UINTN	ii;
	unsigned int key = 0x935b;

   if (IsPasswordSupportNonCaseSensitive ())
   {
   	for ( ii = 0; ii < MaxSize/2; ii++ )
		   Password[ii] = ((Password[ii]>=L'a')&&(Password[ii]<=L'z'))?(Password[ii]+L'A'-L'a'):Password[ii];
   }
	// Encode the password..
	for ( ii = 1; ii <= MaxSize/2; ii++ )
			Password[ii-1] = (CHAR16)(Password[ii-1] ^ (key*ii));
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
