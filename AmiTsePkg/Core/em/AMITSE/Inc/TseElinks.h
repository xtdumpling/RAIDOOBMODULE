//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2012, American Megatrends, Inc.        **//
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
// $Archive: /Alaska/BIN/Modules/AMITSE2_0/AMITSE/Inc/TseElinks.h $
//
// $Author: Rajashakerg $
//
// $Revision: 4 $
//
// $Date: 5/28/12 6:21a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file TseElinks.h
    Header file for Elink related customizations

**/

#ifndef _TSEELINKS_H_
#define _TSEELINKS_H_

#define END_OF_EXIT_PAGE_OPTION_LIST	0xFF

///Default fucntions to handle Exit page options...
VOID HandleSaveAndExit(VOID);
VOID HandleExitApplication(VOID);
VOID HandleSaveAndReset(VOID);
VOID HandleResetSys(VOID);
VOID HandleSaveWithoutExit(VOID);
VOID HandleLoadPreviousValues(VOID);
VOID HandleLoadOptimalDefaults(VOID);
VOID HandleSaveUserDefaults(VOID);
VOID HandleRestoreUserDefaults(VOID);
VOID HandleBootAddBootOption(VOID);
VOID HandleLaunchFileSystem(VOID);
VOID HandleLaunchFileSystemDriver (VOID);
VOID HandleDriverAddDriverOption (VOID);
VOID SaveUserDefaultsSilently(VOID);
VOID LoadUserDefaultsSilently(VOID);

typedef	VOID	(*OPTION_CALLBACK)(VOID);

typedef struct _EXIT_PAGE_OPTIONS
{
	UINT8	OptIdx;
	OPTION_CALLBACK		ExitPageCallBack;
}
EXIT_PAGE_OPTIONS;

typedef struct			
{
	EFI_GUID FormsetGuid;
	UINT32	QuestionID;
	BOOLEAN 	EncodingFlag;
}PASSWORD_ENOCDE_LIST_TEMPLATE;

#endif /* _TSEELINKS_H_ */

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 2012, American Megatrends, Inc.             **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093       **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
