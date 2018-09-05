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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/string.h $
//
// $Author: Rajashakerg $
//
// $Revision: 6 $
//
// $Date: 10/21/11 2:58a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file string.h
    Header file for string functionalities.

**/

#ifndef _STRING_H_
#define	_STRING_H_

#define CharIsUpper(c) ((c >= L'A') && (c <= L'Z'))
#define CharIsLower(c) ((c >= L'a') && (c <= L'z'))
#define CharIsAlpha(c) (CharIsUpper(c) || CharIsLower(c)) 
#define CharIsNumeric(c) ((c >= L'0') && (c <= L'9'))
#define CharIsAlphaNumeric(c) (CharIsAlpha(c) || CharIsNumeric(c))
#define CharIsAlphaNumericSpecial(c) ((c >= (CHAR16)0x20) && (c <= (CHAR16)0x7E))

#define	JUSTIFY_CENTER	(0)
#define	JUSTIFY_LEFT	(1)
#define	JUSTIFY_RIGHT	(2)
#define	JUSTIFY_MAX		JUSTIFY_RIGHT

//UINTN atoi(CHAR16 *string, UINT8 base);
UINTN atoi_base(CHAR16 *string, UINT8 base); 
CHAR16 *StrZeroPad(INT32 Tm, CHAR16 *Str);
CHAR16 *StrDup(CHAR16 *string);
VOID DrawAttribute( UINTN Col, UINTN Row, UINT8 Attrib, UINTN Length );
VOID DrawAttributeOffset( UINTN Offset, UINT8 Attrib, UINTN Length );
VOID DrawString( UINTN Col, UINTN Row, CHAR16 *String );
VOID DrawStringWithAttribute( UINTN Col, UINTN Row, CHAR16 *String, UINT8 Attrib );
VOID DrawMultiLineStringWithAttribute( UINTN Col, UINTN Row,UINTN Width, UINTN Height, CHAR16 *line, UINT8 Attrib );
VOID DrawStringJustified( UINTN MinCol, UINTN MaxCol, UINTN Row, UINTN Justify, CHAR16 *String );
VOID DrawStringJustifiedWithAttribute( UINTN MinCol, UINTN MaxCol, UINTN Row, UINTN Justify, CHAR16 *String, UINT8 Attrib );
VOID WriteChar( UINTN Row, UINTN Col, CHAR16 Char );
VOID WriteCharWithAttribute( UINTN Row, UINTN Col, CHAR16 Char, UINT8 Attrib );

CHAR16 *StringWrapText( CHAR16 *text, UINT16 width, UINT16 *height );
UINTN StringGetWrapHeight( CHAR16 *text, UINT16 width );
UINTN _StringFindBreak( CHAR16 *string );

EFI_STATUS PrintPostMessage( CHAR16 *message, BOOLEAN bAdvanceLine );
VOID DisplayActivePostMsgs(UINTN LogoHeight);
VOID PreservePostMessages(UINTN NewMaxCols, UINTN OldMaxCols);

EFI_STATUS SetCurPos(UINTN X, UINTN Y);
EFI_STATUS GetCurPos(UINTN * const pX, UINTN * const pY);
UINTN GetTokenCount(CHAR8 *String);
CHAR8* GetTokenString(CHAR8 *String, UINTN *pos);
CHAR8 *StrDup8( CHAR8 *string );
CHAR16 *StrDup8to16( CHAR8 *string );
UINTN StrLen8(CHAR8 *string);
VOID StrCpy8(CHAR8 *dest, CHAR8 *src);
CHAR8* StrDup16to8(CHAR16 *String);

#endif /* _STRING_H_ */


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
