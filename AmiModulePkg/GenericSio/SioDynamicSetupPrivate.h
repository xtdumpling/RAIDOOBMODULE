//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file 
  @brief
    Dynamic Setup Driver Private Data Definitions.

**/

#ifndef _SIO_SETUP_PRIVATE_H_
#define _SIO_SETUP_PRIVATE_H_


#include <Efi.h>
#include <UefiHii.h>
#include <AmiGenericSio.h>
#include <SioSetup.h>
#include <Guid/MdeModuleHii.h>

typedef union {
	UINT16	VAR_ID;
	struct _VidField{
		UINT8	LdIndex			:8;	
		UINT8	SioIndex		:4; //Supports up to 8 SIOs 0...7
		UINT8   Reserved        :2; //always 0
		UINT8	IsNoneVolatile	:1;
		UINT8	Always1			:1;
	}VidField;
} SIO_VAR_ID;

typedef struct _LD_SETUP_GOTO_DATA{
	SIO_DEV2					*SioLd;
	AMI_SDL_LOGICAL_DEV_INFO	*SdlInfo;
	UINTN						SioIdx;
	UINTN						LdIdx;
	UINTN						LdNumber; //for display like Serial (LdNumber)... 
    EFI_STRING_ID   			GotoStringId;
    EFI_STRING_ID   			GotoHelpStringId;
    EFI_STRING_ID				LdFormTitleStringId;
    EFI_VARSTORE_ID             NvVarStoreId;
    EFI_VARSTORE_ID             VlVarStoreId;
    EFI_QUESTION_ID             GotoQid;
    EFI_QUESTION_ID             EnableQid;
    EFI_QUESTION_ID             ResSelQid;
    EFI_QUESTION_ID             ModeQid;
    SIO_DEV_NV_DATA             SetupNvData;
    EFI_STRING                  NvVarName;
//to make it look like T_ITEM_LIST
    UINTN						PrsItemCount;
    EFI_STRING_ID				*PrsStrId;
    //to make it look like T_ITEM_LIST
    UINTN						ModeItemCount;
    EFI_STRING_ID				*ModeStrId;
	EFI_STRING_ID				ModeHelpStrId;
}LD_SETUP_GOTO_DATA;

// main SIO form data structure
typedef struct _SIO_IFR_INFO {
    VOID *StartOpCodeHandle;
    VOID *EndOpCodeHandle;
    EFI_IFR_GUID_LABEL  *StartLabel;
    EFI_IFR_GUID_LABEL  *EndLabel;
//Like T_ITEM_LIST
    UINTN				LdInitCnt;
    UINTN				LdCount;
    LD_SETUP_GOTO_DATA	**LdSetupData;
} SIO_IFR_INFO;

// SIO LD device driver config form data structure
typedef struct _SIO_LD_FORM_DATA {
    VOID *StartOpCodeHandle;
    VOID *EndOpCodeHandle;
    EFI_IFR_GUID_LABEL  *StartLabel;
    EFI_IFR_GUID_LABEL  *EndLabel;
    VOID *PrsOptionsOpCodeHandle;
    VOID *ModeOptionsOpCodeHandle;
} SIO_LD_FORM_DATA;



#endif	//_SIO_SETUP_PRIVATE_H_


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

