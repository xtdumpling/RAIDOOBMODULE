//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/BIN/Core/Include/Setup.h 38    8/28/12 4:20p Artems $
//
// $Revision: 38 $
//
// $Date: 8/28/12 4:20p $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        Setup.h
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************
#ifndef __SETUP__H__
#define __SETUP__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Token.h>
#ifndef VFRCOMPILE
#include <UefiHii.h>
#include <Protocol/HiiConfigAccess.h>
#else//VFRCOMPILE
#include<ControlDefinitions.h>
#endif//VFRCOMPILE
#if Setup_SUPPORT || AUTOID_HEADER_SUPPORT
#include <AutoId.h>
#endif
//**********************************************************************//
//                      Generic Definitions                             //
//**********************************************************************//
#define SETUP_GUID { 0xEC87D643, 0xEBA4, 0x4BB5, 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 }

#define SETUP_FORM_SET_GUID    { 0x7B59104A, 0xC00D, 0x4158, 0x87, 0xFF, 0xF0, 0x4D, 0x63, 0x96, 0xA9, 0x15 }

#define MAIN_FORM_SET_GUID     SETUP_FORM_SET_GUID
#define ADVANCED_FORM_SET_GUID SETUP_FORM_SET_GUID
#define CHIPSET_FORM_SET_GUID  SETUP_FORM_SET_GUID
#define BOOT_FORM_SET_GUID     SETUP_FORM_SET_GUID
#define SECURITY_FORM_SET_GUID SETUP_FORM_SET_GUID
#define EXIT_FORM_SET_GUID     SETUP_FORM_SET_GUID

#define SETUP_FORM_SET_CLASS            0x01
#define MAIN_FORM_SET_CLASS             SETUP_FORM_SET_CLASS
#define ADVANCED_FORM_SET_CLASS         SETUP_FORM_SET_CLASS
#define CHIPSET_FORM_SET_CLASS          SETUP_FORM_SET_CLASS
#define BOOT_FORM_SET_CLASS             SETUP_FORM_SET_CLASS
#define SECURITY_FORM_SET_CLASS         SETUP_FORM_SET_CLASS
#define EXIT_FORM_SET_CLASS             SETUP_FORM_SET_CLASS


#include <SetupDataDefinition.h>

#include <AMIVfr.h>

#ifdef VFRCOMPILE
//**********************************************************************//
//                        VFR Definitions                               //
//**********************************************************************//
#if defined(INVENTORY_SUPPORT) && INVENTORY_SUPPORT == 1
  #define INVENTORY(Name,Value) \
    inventory \
      help  = STRING_TOKEN(STR_EMPTY), \
      text  = Name, \
      text  = Value;
#else
  #define INVENTORY(Name,Value) \
    text \
      help  = STRING_TOKEN(STR_EMPTY), \
      text  = Name, \
      text  = Value, \
      flags = 0, \
      key   = 0;
#endif

#define SUBTITLE(Text) subtitle text = Text;
#define SEPARATOR SUBTITLE(STRING_TOKEN(STR_EMPTY))

#define SYSTEM_ACCESS_KEY_ID            0xF000

#define INTERACTIVE_TEXT(HelpToken, CaptionToken, ValueToken, Key)\
    text help  = HelpToken,\
    	text  = CaptionToken,\
    	text  = ValueToken;\
    suppressif TRUE;\
        suppressif ideqval AMI_CALLBACK.Value == INTERACTIVE_TEXT_VALUE;\
	    text help  = HelpToken,\
	    	text  = CaptionToken,\
			flags = INTERACTIVE, key = Key;\
        endif;\
    endif;

#define SUPPRESS_GRAYOUT_ENDIF endif; endif;
//definitions of the standard varstores
#define SETUP_DATA_VARSTORE\
    varstore SETUP_DATA, name = Setup, guid = SETUP_GUID;
#define LANGUAGE_VARSTORE\
    varstore LANGUAGE, key = LANGUAGE_KEY_ID,\
        name = PlatformLang, guid = EFI_GLOBAL_VARIABLE_GUID;
#define LANGUAGE_CODES_VARSTORE\
    varstore LANGUAGE_CODES, key = LANGUAGE_CODES_KEY_ID,\
        name = PlatformLangCodes, guid = EFI_GLOBAL_VARIABLE_GUID;
#define SYSTEM_ACCESS_VARSTORE\
    varstore SYSTEM_ACCESS, key = SYSTEM_ACCESS_KEY_ID,\
        name = SystemAccess, guid = SYSTEM_ACCESS_GUID;
#define AMITSESETUP_VARSTORE\
    varstore AMITSESETUP, key = AMITSESETUP_KEY,\
        name = AMITSESetup, guid = AMITSESETUP_GUID;
#define BOOT_MANAGER_VARSTORE\
    varstore BOOT_MANAGER, key = BOOT_MANAGER_KEY_ID,\
        name = BootManager, guid = BOOT_MANAGER_GUID;
#define TIMEOUT_VARSTORE\
    varstore TIMEOUT, key = BOOT_TIMEOUT_KEY_ID,\
        name = Timeout, guid = EFI_GLOBAL_VARIABLE_GUID;
#define BOOT_ORDER_VARSTORE\
    varstore BOOT_ORDER, key = BOOT_ORDER_KEY_ID,\
        name = BootOrder, guid = EFI_GLOBAL_VARIABLE_GUID;
#define NEW_BOOT_OPTION_VARSTORE\
    varstore NEW_BOOT_OPTION, key = NEW_BOOT_OPTION_KEY_ID,\
        name = AddBootOption, guid = ADD_BOOT_OPTION_GUID;
#define DEL_BOOT_OPTION_VARSTORE\
    varstore DEL_BOOT_OPTION, key = DEL_BOOT_OPTION_KEY_ID,\
        name = DelBootOption, guid = DEL_BOOT_OPTION_GUID;
#define LEGACY_DEV_INFO_VARSTORE\
    varstore LEGACY_DEV_INFO, key = LEGACY_DEV_KEY_ID,\
        name = LegacyDev, guid = LEGACY_DEV_ORDER_GUID;
#define LEGACY_GROUP_INFO_VARSTORE\
    varstore LEGACY_GROUP_INFO, key = LEGACY_GROUP_KEY_ID,\
        name = LegacyGroup, guid = LEGACY_DEV_ORDER_GUID;
#define LEGACY_DEV_ORDER_VARSTORE\
    varstore LEGACY_DEV_ORDER, key = LEGACY_ORDER_KEY_ID,\
        name = LegacyDevOrder, guid = LEGACY_DEV_ORDER_GUID;
#define AMI_CALLBACK_VARSTORE\
    varstore AMI_CALLBACK, key = AMI_CALLBACK_KEY_ID,\
        name = AMICallback, guid = AMI_CALLBACK_GUID;
#define BOOT_NOW_COUNT_VARSTORE\
    varstore BOOT_NOW_COUNT, key = BOOT_NOW_COUNT_KEY_ID,\
        name = BootNowCount, guid = BOOT_NOW_COUNT_GUID;
#define EFI_SHELL_VARSTORE\
    varstore EFI_SHELL, key = EFI_SHELL_KEY_ID,\
        name = Shell, guid = EFI_SHELL_FILE_GUID;
#else // #ifdef VFRCOMPILE
//**********************************************************************//
//                        C Definitions                                 //
//**********************************************************************//
//Type of the Setup Callback Handling function registered using 
// ITEM_CALLBACK eLink
#define SETUP_ITEM_CALLBACK_DEFINED
typedef EFI_STATUS (SETUP_ITEM_CALLBACK_HANDLER)(
    EFI_HII_HANDLE HiiHandle, 
    UINT16 Class, 
    UINT16 SubClass, 
    UINT16 Key
);

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name:    SETUP_ITEM_CALLBACK
//
// Fields: Type Name Description
//  UINT16 Class - Primary identification value of an Hii Formset
//  UINT16 SubClass - Secondary identification value of an Hii Formset 
//  UINT16 Key - A unique value that identifies a specific item on a setup form
//  SETUP_ITEM_CALLBACK_HANDLER *UpdateItem - function used to update an item 
//                          defined by the Class, SubClass and Key parameters
//
// Description:
//  This Data Structure is used by the setup infrastructure to define 
//  callback functions that should be used for interacting with setup forms
//  or individual questions.
//
// Notes:
//   
//----------------------------------------------------------------------------
//<AMI_THDR_END>
typedef struct{
    UINT16 Class;
    UINT16 SubClass;
    UINT16 Key;
    SETUP_ITEM_CALLBACK_HANDLER *UpdateItem;
} SETUP_ITEM_CALLBACK;

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name:    SETUP_CALLBACK
//
// Fields: Type Name Description
//  EFI_FORM_CALLBACK_PROTOCOL Callback - Callback Protocol Instance for the 
//                                        Class and SubClass defined below
//  UINT16 Class - Value defined to identify a particular Hii form 
//  UINT16 SubClass - Secondary value used to uniquely define the an Hii form 
//
// Description:
//  These Data Structure define a structure used to match a specific 
//  Callback Protocol to an HII Form through the use of Class and SubClass 
//  values
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_THDR_END>
typedef struct{
    EFI_HII_CONFIG_ACCESS_PROTOCOL *This;
    EFI_BROWSER_ACTION Action;
    EFI_QUESTION_ID KeyValue;
    UINT8 Type;
    EFI_IFR_TYPE_VALUE *Value;
    EFI_BROWSER_ACTION_REQUEST *ActionRequest;
} CALLBACK_PARAMETERS;

//Defined in the Setup Module
VOID InitString(EFI_HII_HANDLE HiiHandle, STRING_REF StrRef, CHAR16 *sFormat, ...);
CALLBACK_PARAMETERS* GetCallbackParameters();
#endif

//Boot Flow Definitions
// EF152FB4-7B2F-427D-BDB4-7E0A05826E64
#define	BOOT_FLOW_VARIABLE_GUID \
	{ 0xEF152FB4, 0x7B2F, 0x427D, 0xBD, 0xB4, 0x7E, 0x0A, 0x05, 0x82, 0x6E, 0x64 }

#define	BOOT_FLOW_CONDITION_RECOVERY            2
#define	BOOT_FLOW_CONDITION_FIRST_BOOT          4
#define BOOT_FLOW_CONDITION_PCI_OUT_OF_RESOURCE 9
#define	BOOT_FLOW_CONDITION_OS_UPD_CAP 			0xB


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
