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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Include/Protocol/AMIScreenMgmt.h $
//
// $Author:  $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file AMIScreenMgmt.h
    AMI Screen management protocol related code

**/

#ifndef _AMI_SCREENMGMT_H_
#define	_AMI_SCREENMGMT_H_

#define AMI_TSE_SCREEN_MGMT_PROTOCOL_GUID \
   { 0xd6eb696b, 0x7ec3, 0x4d1b, 0xaa, 0x28, 0x67, 0x75, 0x74, 0x4c, 0x9e, 0xb5 }

typedef 
EFI_STATUS 
(EFIAPI *AMI_SAVE_TSE_SCREEN) ();

typedef 
EFI_STATUS 
(EFIAPI *AMI_RESTORE_TSE_SCREEN) ();

typedef 
EFI_STATUS 
(EFIAPI *AMI_CLEAR_TSE_SCREEN) (
		IN UINT8 Size
	);

typedef struct _AMI_TSE_SCREEN_MGMT_PROTOCOL
{
	AMI_SAVE_TSE_SCREEN				SaveTseScreen;	
	AMI_RESTORE_TSE_SCREEN			RestoreTseScreen;
	AMI_CLEAR_TSE_SCREEN			ClearScreen;
}
AMI_TSE_SCREEN_MGMT_PROTOCOL;

extern EFI_GUID	gTSEScreenMgmtProtocolGuid;

#endif /* _AMI_SCREENMGMT_H_ */

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
