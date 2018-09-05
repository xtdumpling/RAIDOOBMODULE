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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/box.h $
//
// $Author: Madhans $
//
// $Revision: 3 $
//
// $Date: 2/19/10 1:01p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file box.h
    Header file for box related functions

**/

#ifndef _BOX_H_
#define	_BOX_H_

VOID DrawLine( UINTN Col, UINTN Rown, UINTN Length, CHAR16 Type );
VOID DrawLineWithAttribute( UINTN Col, UINTN Rown, UINTN Length, CHAR16 Type, UINT8 Attrib );
VOID DrawBorder( UINTN Left, UINTN Top, UINTN Width, UINTN Height );
VOID DrawWindow( UINTN Left, UINTN Top, UINTN Width, UINTN Height, UINT8 Attrib, BOOLEAN Border, BOOLEAN Shadow );
VOID DrawBox( UINTN Left, UINTN Top, UINTN Width, UINTN Height, UINT8 Attrib );
#endif /* _BOX_H_ */

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
