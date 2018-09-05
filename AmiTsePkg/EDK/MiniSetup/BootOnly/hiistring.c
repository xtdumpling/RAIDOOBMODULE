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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/hiistring.c $
//
// $Author: Madhans $
//
// $Revision: 4 $
//
// $Date: 12/02/10 2:33p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file hiistring.c
    file contains code to handle the hii sting operations

**/

#include "minisetup.h"

#define VARIABLE_NAME_LENGTH 40

/**
    function to check the boundary of the print stirng.
    Find out the index of the character from where the print boundry will be met

    @param String IN UINTN PrintWidth

    @retval count

**/
UINTN HiiFindStrPrintBoundary ( IN CHAR16   *String, IN UINTN PrintWidth )
{
  UINTN Length = 0;
  UINT16                Count = 0;
  CHAR16				TempString[10];

  while (String[Count]) {
	  TempString[0] = String[Count];
	  TempString[1] = 0;

	  if((Length + TestPrintLength(TempString) / NG_SIZE) > PrintWidth)
		  break;

	  Length += TestPrintLength(TempString) / NG_SIZE;

	  Count++;
  }
  return Count;
}

/**
    function to check the given character is wide

    @param strWide 

    @retval TRUE/FALSE

**/
UINTN IsCharWide( CHAR16 strWide)
{
  UINTN		Length = 0;
  CHAR16	TempString[10];

  TempString[0] = strWide;
  TempString[1] = 0;
  Length = 0;
  Length = TestPrintLength( TempString ) / (NG_SIZE);
  if(2 == Length)
	  return TRUE;
  else
	  return FALSE;
}

CHAR16 *HiiGetString( VOID* handle, UINT16 token )
{
	if(INVALID_TOKEN != token)
		return HiiGetStringLanguage( handle, token, NULL );
	else
		return NULL;
}

/**
    function to add a string

    @param handle CHAR16 *string

    @retval token

**/

UINT16 HiiAddString( VOID* handle, CHAR16 *string )
{
	UINT16 token = 0;

	if(ItkSupport())
	{
	    token = HiiChangeStringLanguage( handle, 0, gLanguages[0].Unicode, string );
	}
	else
	{
	    UINT16 FirstToken;
		UINTN i, j;

		FirstToken = token = HiiChangeStringLanguage( handle, 0, gLanguages[0].Unicode, string );
	    if ( token == INVALID_TOKEN )
	    	return token;
		for ( j = 0, i = 1; i < gLangCount; i++ )
	    {
	        if ( i != j )
	    		token = HiiChangeStringLanguage( handle, token, gLanguages[i].Unicode, string );
	        if ( token == INVALID_TOKEN )
	        {
			    token = HiiChangeStringLanguage( handle, 0, gLanguages[i].Unicode, string );
	            if ( token == INVALID_TOKEN )
	    	        return FirstToken; //token;
	            j = i;
	            i = 0;
	        }
	    }
	}
   	return token;
}

/**
    function to get the printable lenght of the string

    @param string 

    @retval Size of string

**/
UINTN EfiTestPrintLength ( IN CHAR16   *String )
{
	UINTN Size;

	if(String == NULL)
		return 0;

	String = (UINT16* )TseSkipEscCode(String);
	Size = (EfiStrLen(String) * NG_SIZE);
	gBS->FreePool( String );
	return Size;
}
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

