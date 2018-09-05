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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/screen.h $
//
// $Author: Madhans $
//
// $Revision: 3 $
//
// $Date: 2/19/10 1:02p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

#ifndef _SCREEN_H_
#define	_SCREEN_H_

VOID ClearScreenWithoutFlush( UINT8 Attrib );
VOID ClearLinesWithoutFlush( UINT16 LineStart, UINT16 NumLines, UINT8 Attrib );
VOID ClearScreen( UINT8 Attrib );
VOID ClearFlushBuffer(UINT8 Attrib);
VOID *SaveScreen( VOID );
VOID RestoreScreen( VOID *ScreenBuffer );
VOID UpdateScreen( VOID );
typedef struct
{
	UINT32	ScreenTag;
	UINT8	Attribs[MAX_DIMENSIONS];
	CHAR16	Chars[MAX_DIMENSIONS];
}
SCREEN_BUFFER;

#define	ACTIVE_SCREEN_TAG	0x54434153	// "SACT"
#define	SAVE_SCREEN_TAG		0x56415353	// "SSAV"

extern SCREEN_BUFFER *gActiveBuffer;

VOID MemFillUINT16Buffer( VOID *buffer, UINTN bufferSize, UINT16 value );

#endif /* _SCREEN_H_ */

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
