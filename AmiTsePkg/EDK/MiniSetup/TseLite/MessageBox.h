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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/MessageBox.h $
//
// $Author: Rajashakerg $
//
// $Revision: 8 $
//
// $Date: 12/01/11 5:42a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

#ifndef _MSGBOX_H_
#define	_MSGBOX_H_

#if TSE_USE_EDK_LIBRARY
#include "Tiano.h" 
#else
#include "Efi.h"
#endif

#include "popup.h"
#include "Memo.h"  

//#define MAX_MSGBOX_WIDTH		45
// messagebox constants
#define MSGBOX_TYPE_NULL        0
#define MSGBOX_TYPE_OK			1
#define MSGBOX_TYPE_OKCANCEL	2
#define MSGBOX_TYPE_YESNO		3
#define MSGBOX_TYPE_CUSTOM		4
#define MSGBOX_TYPE_PROGRESS		5  	// internal use only (NOT PUBLISHED)
#define	MSGBOX_TYPE_MASK		0x0F
#define MSGBOX_TYPE_MAXTYPE     MSGBOX_TYPE_MASK
#define	MSGBOX_STYLE_CENTER		(UINT8)(JUSTIFY_CENTER << 4)
#define	MSGBOX_STYLE_RIGHT		(UINT8)(JUSTIFY_RIGHT << 4)
#define	MSGBOX_STYLE_LEFT		(UINT8)(JUSTIFY_LEFT << 4)
#define	MSGBOX_STYLE_MASK		0xF0
#define MSGBOX_OK				0
#define MSGBOX_CANCEL			1
#define MSGBOX_YES				0	
#define MSGBOX_NO				1

#define MSGBOX_MAX_OPTIONS		4
typedef struct _IFR_MSGBOX
{ // this structure is, in some way, following EFI_IFR structures
	UINT8	Opcode, Length;
	UINT16	Title;
	/*EFI_HII_HANDLE*/VOID * 	TextHandle;
	UINT16	Text;
}AMI_IFR_MSGBOX;

#define	MSGBOX_MEMBER_VARIABLES	\
    VOID	*ScreenBuf;			\
	MEMO_DATA *Memo;			\
	UINT8	MsgBoxType;			\
	UINT8	MsgboxSel;			\
	UINT8	Button1Len;			\
	UINT8	Button2Len;			\
	CHAR16	*Legend;			\
	MSGBOX_EX_CATAGORY MsgBoxCatagory;	\
	UINT16  *PtrTokens;			\
	UINT16  OptionCount;			\
	AMI_POST_MGR_KEY  *HotKeyList;		\
	UINT16 	HotKeyListCount; 		\
	AMI_POST_MGR_KEY  *OutKey;		\
	UINTN 	Percent;				// 0 - 100


typedef struct _MSGBOX_METHODS	MSGBOX_METHODS;

typedef struct _MSGBOX_DATA
{
	MSGBOX_METHODS	*Methods;

	OBJECT_MEMBER_VARIABLES
	CONTROL_MEMBER_VARIABLES
	POPUP_MEMBER_VARIABLES
	MSGBOX_MEMBER_VARIABLES
}
MSGBOX_DATA;

typedef	EFI_STATUS	(*MSGBOX_METHOD_SET_TYPE)		( MSGBOX_DATA *object, UINT8 Type );

#define	MSGBOX_METHOD_FUNCTIONS \
      MSGBOX_METHOD_SET_TYPE SetType;

struct _MSGBOX_METHODS
{
	OBJECT_METHOD_FUNCTIONS
	CONTROL_METHOD_FUNCTIONS
    MSGBOX_METHOD_FUNCTIONS
};

extern MSGBOX_METHODS gMsgBox;


// Object Methods
EFI_STATUS MsgBoxCreate( MSGBOX_DATA **object );
EFI_STATUS MsgBoxDestroy( MSGBOX_DATA *object, BOOLEAN freeMem );
EFI_STATUS MsgBoxInitialize( MSGBOX_DATA *object, VOID *data );
EFI_STATUS MsgBoxDraw( MSGBOX_DATA *object );
EFI_STATUS MsgBoxHandleAction(MSGBOX_DATA *object, ACTION_DATA *Data);
EFI_STATUS MsgBoxSetCallback(  MSGBOX_DATA *object, OBJECT_DATA *container, OBJECT_CALLBACK callback, VOID *cookie );
EFI_STATUS MsgBoxSetType(MSGBOX_DATA *object, UINT8 Type );

// Control Methods
EFI_STATUS MsgBoxSetFocus(MSGBOX_DATA *object, BOOLEAN focus);
EFI_STATUS MsgBoxSetPosition(MSGBOX_DATA *object, UINT16 Left, UINT16 Top);
EFI_STATUS MsgBoxSetDimensions(MSGBOX_DATA *object, UINT16 Width, UINT16 Height);
EFI_STATUS MsgBoxSetAttributes(MSGBOX_DATA *object, UINT8 FGColor, UINT8 BGColor);
EFI_STATUS MsgBoxGetControlHight( MSGBOX_DATA *object,VOID *frame, UINT16 *height );

#endif /* _MSGBOX_H_ */


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
