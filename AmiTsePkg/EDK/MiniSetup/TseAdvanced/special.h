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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseAdvanced/special.h $
//
// $Author: Arunsb $
//
// $Revision: 8 $
//
// $Date: 1/30/12 1:27a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file special.h
    Header file for TSE special controls like AMI callback controls
				

**/
#ifndef _SPECIAL_H_
#define	_SPECIAL_H_

#define	GUID_KEY_BOOT_NOW					0x0001
#define	GUID_KEY_AMI_CALLBACK			0x0002
#define	GUID_KEY_ERROR_MANAGER			0x0003
#define	GUID_KEY_LANG_CODES				0x0005
#define 	GUID_KEY_BBS_ORDER				0x0007
#define 	GUID_KEY_IDE_SECURITY			0x0008
#define 	GUID_KEY_ADD_BOOT_OPTION   	0x0009
#define 	GUID_KEY_OEM_CONTROL	    		0x000A
#define 	GUID_KEY_MODAL						0x000B
#define 	GUID_KEY_REFRESH					0x000C
#define	GUID_KEY_NO_COMMIT				0x000D
#define	GUID_KEY_DYNAMIC_PAGE			0x000E
#define	GUID_KEY_DRIVER_HEALTH_ENB		0x000F
#define	GUID_KEY_DRIVER_HEALTH			0x0010
#define	GUID_KEY_DRV_HEALTH_CTRL_COUNT	0x0011
#define	GUID_KEY_DRIVER_OPTION			0x0012		
#define	GUID_KEY_OEMGUID1					0x0013	
#define	GUID_KEY_OEMGUID2					0x0014
#define GUID_KEY_DYNAMIC_PAGE_GROUP         0x0016
#define GUID_KEY_DYNAMIC_PAGE_DEVICE        0x0017
#define GUID_KEY_DYNAMIC_PAGE_CLASS        	0x0018

UINT16 SpecialGetValue( CONTROL_DATA *control, GUID_INFO **guidInfo );
VOID SpecialAddMultipleControls( CONTROL_INFO *controlInfo, GUID_INFO *guidInfo );
VOID SpecialUpdatePageControls(UINT32 CurrentPage);

#endif /* _SPECIAL_H_ */

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
