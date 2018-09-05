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
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Uefi2.1/Uefi21Wapper.c $
//
// $Author: Arunsb $
//
// $Revision: 60 $
//
// $Date: 5/28/12 12:38p $
//
//*****************************************************************//
//*****************************************************************//
/** @file uefi21wapper.c

**/
//****************************************************************//

//----------------------------------------------------------------------------

#include "minisetup.h"
#include "TseUefiHii.h"
#include "TseElinks.h"          
#include EFI_PROTOCOL_DEFINITION(UnicodeCollation)
#if TSE_DEBUG_MESSAGES
#include <Protocol/SimpleFileSystem.h>
#endif 
#include "TseDrvHealth.h"       //Has TSE related driver health structures
//Driver health support
UINT16 *_DevicePathToStr (EFI_DEVICE_PATH_PROTOCOL *Path);
CHAR16 DriverHealthStatus [6][23] = {
									L"Healthy",
									L"Repair Required",
									L"Configuration Required",
									L"Failed",
									L"Reconnect Required",
									L"Reboot Required"
								};
EFI_HANDLE                  gDrvHandle = NULL;
EFI_DRIVER_HEALTH_PROTOCOL  *gDrvHealthInstance = NULL;
DRV_HEALTH_HNDLS            *gDrvHealthHandlesHead = NULL;
extern BOOLEAN gBrowserCallbackEnabled;
extern BOOLEAN gEnableDrvNotification; //TRUE if allow notification function to process action, FALSE to ignore the notification
extern AMI_POST_MANAGER_PROTOCOL 	*mPostMgr;
extern VOID _GetNextLanguage(CHAR8 **LangCode, CHAR8 *Lang);
extern DYNAMIC_PAGE_GROUP *gDynamicPageGroup ;
extern UINTN gDynamicPageGroupCount ;
UINT16 gCurrDynamicPageGroupClass ;
//----------------------------------------------------------------------------
#if TSE_DEBUG_MESSAGES
BOOLEAN SetupDebugKeyHandler(UINT16 ItemIndex, UINT16 Unicode, CHAR16	*Value) ;
EFI_STATUS DisplayParseFormSetData(PAGE_INFO *PageInfo) ; 
EFI_STATUS DebugShowAllHiiVariable(UINT32 formID) ;
UINT16 _GetQuestionToken(UINT8 *ifrData);
VOID ProcessPackToFile(EFI_HII_DATABASE_NOTIFY_TYPE NotifyType, EFI_HANDLE Handle);
UINTN HpkFileCount=0;
#endif //End of TSE_DEBUG_MESSAGES

EFI_FILE_PROTOCOL * CreateFile(CHAR16 *filename, UINT32 index);
EFI_GUID  guidSimpleFileSystemProtocol = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

static EFI_IFR_SUBTITLE	_Title = { { EFI_IFR_SUBTITLE_OP, sizeof(EFI_IFR_SUBTITLE) }, 0 };
static EFI_IFR_SUBTITLE	_Help = { { EFI_IFR_SUBTITLE_OP, sizeof(EFI_IFR_SUBTITLE) }, 0 };
static EFI_IFR_SUBTITLE	_SubTitle = { { EFI_IFR_SUBTITLE_OP, sizeof(EFI_IFR_SUBTITLE) }, 0 };
static EFI_IFR_SUBTITLE	_HelpTitle = { { EFI_IFR_SUBTITLE_OP, sizeof(EFI_IFR_SUBTITLE) }, STRING_TOKEN(STR_HELP_TITLE) };
static EFI_IFR_SUBTITLE	_NavStrings = { { EFI_IFR_SUBTITLE_OP, sizeof(EFI_IFR_SUBTITLE) }, 0 };

// Not usedstatic STRING_REF CallbackErrToken=0;

AMI_IFR_MSGBOX _PreviousMsgBox = { 0, 0, STRING_TOKEN(STR_LOAD_PREVIOUS), (VOID*)INVALID_HANDLE, STRING_TOKEN(STR_LOAD_PREVIOUS_MSG) };
AMI_IFR_MSGBOX _FailsafeMsgBox = { 0, 0, STRING_TOKEN(STR_LOAD_FAILSAFE), (VOID*)INVALID_HANDLE, STRING_TOKEN(STR_LOAD_FAILSAFE_MSG) };
AMI_IFR_MSGBOX _OptimalMsgBox = { 0, 0, STRING_TOKEN(STR_LOAD_OPTIMAL), (VOID*)INVALID_HANDLE, STRING_TOKEN(STR_LOAD_OPTIMAL_MSG) };
AMI_IFR_MSGBOX _SaveMsgBox = { 0, 0, STRING_TOKEN(STR_SAVE_VALUES), (VOID*)INVALID_HANDLE, STRING_TOKEN(STR_SAVE_VALUES_MSG) };
AMI_IFR_MSGBOX _SaveExitMsgBox = { 0, 0, STRING_TOKEN(STR_SAVE_EXIT), (VOID*)INVALID_HANDLE, STRING_TOKEN(STR_SAVE_EXIT_MSG) };
AMI_IFR_MSGBOX _ExitMsgBox = { 0, 0, STRING_TOKEN(STR_EXIT), (VOID*)INVALID_HANDLE,  STRING_TOKEN(STR_EXIT_MSG) };
AMI_IFR_MSGBOX _SaveResetMsgBox = { 0, 0, STRING_TOKEN(STR_SAVE_RESET), (VOID*)INVALID_HANDLE, STRING_TOKEN(STR_SAVE_RESET_MSG) };
AMI_IFR_MSGBOX _ResetMsgBox = { 0, 0, STRING_TOKEN(STR_RESET), (VOID*)INVALID_HANDLE, STRING_TOKEN(STR_RESET_MSG) };
AMI_IFR_MSGBOX _HelpMsgBox = { 0, 0, STRING_TOKEN(STR_GENERAL_HELP), (VOID*)INVALID_HANDLE, STRING_TOKEN(STR_GENERAL_HELP_MSG) };
AMI_IFR_MSGBOX _SaveUserMsgBox = { 0, 0, STRING_TOKEN(STR_SAVE_USER_DEFAULTS), (VOID*)INVALID_HANDLE, STRING_TOKEN(STR_SAVE_VALUES_MSG) };
AMI_IFR_MSGBOX _LoadUserMsgBox = { 0, 0, STRING_TOKEN(STR_LOAD_USER_DEFAULTS),  (VOID*)INVALID_HANDLE, STRING_TOKEN(STR_LOAD_USER_MSG) };

AMI_IFR_MSGBOX _InvalidPasswordFailMsgBox	= { 0, 0, STRING_TOKEN(STR_ERROR),(VOID*) INVALID_HANDLE, STRING_TOKEN(STR_ERROR_PSWD) };
AMI_IFR_MSGBOX _ClearPasswordMsgBox 		= { 0, 0, STRING_TOKEN(STR_WARNING),(VOID*) INVALID_HANDLE, STRING_TOKEN(STR_PSWD_CLR) };
AMI_IFR_MSGBOX _ClearPasswordLabelMsgBox 	= { 0, 0, STRING_TOKEN(STR_WARNING),(VOID*) INVALID_HANDLE, STRING_TOKEN(STR_PSWD_CLR_LABEL) };
AMI_IFR_MSGBOX _BootLaunchFailedMsgBox 		= { 0, 0, STRING_TOKEN(STR_WARNING), INVALID_HANDLE, STRING_TOKEN(STR_WARNING_NOT_FOUND) };
AMI_IFR_MSGBOX _gInvalidRangeFailMsgBox 	= { 0, 0, STRING_TOKEN(STR_ERROR), INVALID_HANDLE, STRING_TOKEN(STR_ERROR_INPUT) };
AMI_IFR_MSGBOX _CannotChangePasswordMsgBox = { 0, 0, STRING_TOKEN(STR_ERROR),  (VOID*)INVALID_HANDLE, STRING_TOKEN(STR_CANT_CHANGE_PASSWORD) };

AMI_IFR_MSGBOX _gDelBootOptionReserved = { 0, 0, STRING_TOKEN(STR_WARNING), INVALID_HANDLE, STRING_TOKEN(STR_DEL_BOOT_OPTION_RESERVED) };
AMI_IFR_MSGBOX _gAddBootOptionReserved = { 0, 0, STRING_TOKEN(STR_WARNING), INVALID_HANDLE, STRING_TOKEN(STR_ADD_BOOT_OPTION_RESERVED) };
AMI_IFR_MSGBOX _gAddBootOptionEmpty = { 0, 0, STRING_TOKEN(STR_WARNING), INVALID_HANDLE, STRING_TOKEN(STR_ADD_BOOT_OPTION_EMPTY) };
AMI_IFR_MSGBOX _gAddDriverOptionEmpty = { 0, 0, STRING_TOKEN(STR_WARNING), INVALID_HANDLE, STRING_TOKEN(STR_ADD_DRIVER_OPTION_EMPTY) };


UINTN gPreviousMsgBox = (UINTN)&_PreviousMsgBox;
UINTN gFailsafeMsgBox = (UINTN)&_FailsafeMsgBox;
UINTN gOptimalMsgBox = (UINTN)&_OptimalMsgBox;
UINTN gSaveMsgBox = (UINTN)&_SaveMsgBox;
UINTN gSaveExitMsgBox = (UINTN)&_SaveExitMsgBox;
UINTN gExitMsgBox = (UINTN)&_ExitMsgBox;
UINTN gSaveResetMsgBox = (UINTN)&_SaveResetMsgBox;
UINTN gResetMsgBox = (UINTN)&_ResetMsgBox;
UINTN gHelpMsgBox = (UINTN)&_HelpMsgBox;
UINTN gSaveUserMsgBox = (UINTN)&_SaveUserMsgBox;
UINTN gLoadUserMsgBox = (UINTN)&_LoadUserMsgBox;

UINTN gBootLaunchFailedMsgBox 		=	(UINTN)&_BootLaunchFailedMsgBox;
UINTN gInvalidPasswordFailMsgBox	=	(UINTN)&_InvalidPasswordFailMsgBox;
UINTN gClearPasswordMsgBox			=	(UINTN)&_ClearPasswordMsgBox;
UINTN gClearLabelPasswordMsgBox		=	(UINTN)&_ClearPasswordLabelMsgBox;
UINTN gInvalidRangeFailMsgBox		=	(UINTN)&_gInvalidRangeFailMsgBox;
UINTN gDelBootOptionReserved 		=	(UINTN)&_gDelBootOptionReserved;
UINTN gAddBootOptionReserved 		=	(UINTN)&_gAddBootOptionReserved;
UINTN gAddBootOptionEmpty 			=	(UINTN)&_gAddBootOptionEmpty;
UINTN gAddDriverOptionEmpty 		=	(UINTN)&_gAddDriverOptionEmpty;
UINTN gCannotChangePasswordMsgBox 	= 	(UINTN)&_CannotChangePasswordMsgBox;

// Not used static AMI_IFR_MSGBOX gCallbackErrorMsgBox = { 0, 0, STRING_TOKEN(STR_ERROR), (VOID*)INVALID_HANDLE, 0 };

static BOOLEAN gHiiInitialized = FALSE;

//EFI_GUID gEfiHiiConfigRoutingProtocolGuid = EFI_HII_CONFIG_ROUTING_PROTOCOL_GUID;

EFI_HII_CONFIG_ROUTING_PROTOCOL *gHiiConfigRouting;

UINTN	gTitle = (UINTN)&_Title;
UINTN	gHelp = (UINTN)&_Help;
UINTN	gSubTitle = (UINTN)&_SubTitle;
UINTN	gHelpTitle = (UINTN)&_HelpTitle;
UINTN	gNavStrings = (UINTN)&_NavStrings;

VOID _UefiGetValidOptions(CONTROL_INFO *IfrPtr, UINT16 **OptionPtrTokens,
                        UINT64 **ValuePtrTokens, UINT16 *ItemCount,
						UINT8 MaxItems);

EFI_STATUS _VerifyResponseString(EFI_STRING Configuration, BOOLEAN NameValue);
UINT32 FindVarFromITKQuestionId(UINT16 QuestionId);

//---------------------------------------------------------------------------
// Defines for Utility Functions
//---------------------------------------------------------------------------
#define MAX_FORMS_TO_PROCESS      50
#define EFI_INCONSISTENT_VALUE      (0x80000000 | 28)
#define EFI_NOSUBMIT_VALUE      (0x80000000 | 29)
#define CHECKBOX_OPTION_COUNT   2
#define OPTION_DEFAULT            0x10
#define OPTION_DEFAULT_MFG        0x20
#define TSE_GET_ONE_OF_WIDTH(a)				(((EFI_IFR_ONE_OF*)(a))->Width)
#define TSE_GET_ONE_OF_OPTION_WIDTH(a) ( \
          ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_NUM_SIZE_8 ? sizeof(UINT8) : (\
          ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_NUM_SIZE_16 ? sizeof(UINT16) : (\
          ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_NUM_SIZE_32 ? sizeof(UINT32) : (\
          ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_NUM_SIZE_64 ? sizeof(UINT64) : (\
          ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_BOOLEAN ? sizeof(BOOLEAN) : (\
          ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_TIME ? sizeof(EFI_HII_TIME) : (\
          ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_DATE ? sizeof(EFI_HII_DATE) : (\
          ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_STRING ? sizeof(EFI_STRING_ID) : (\
          0)))))))))



typedef struct _OPTION_LIST OPTION_LIST;

struct _OPTION_LIST 
{
  UINT16  Option;
  UINT64   Value;
  UINT8  Flag;
  OPTION_LIST *Next;
};

static BOOLEAN  LibInitialized = FALSE;
UINT32 *gProcessedForms; //[MAX_FORMS_TO_PROCESS]; 
UINT32 gVarOffset = 0;
UINT32 gProFormOffset = 0;
UINT32 gProFormOffsetAllocCount = 0;
//---------------------------------------------------------------------------
// Prototypes for Utility Functions
//---------------------------------------------------------------------------
BOOLEAN _InconsistenceCheck(PAGE_INFO *PgInfo, UINT16 *ErrStrToken);
BOOLEAN _DestinationFormProcessed(UINT32 FormID);
EFI_STATUS _GetVariableIDList(UINT32 FormID, UINT32 **varIDList);
EFI_STATUS _GetCheckBoxOptions(CONTROL_INFO *CtrlInfo, UINT32 *OptionCount, OPTION_LIST **OptionArray);
EFI_STATUS LoadDefaults(UINT32 FormID, BOOLEAN *ResetVal, BOOLEAN * ValChanged);
BOOLEAN _InconsistenceCheck(PAGE_INFO *PgInfo, UINT16 *ErrStrToken);
BOOLEAN _NoSubmitCheck(CONTROL_INFO *ctrlInfo, UINT16 *ErrStrToken);
EFI_STATUS _GetVariableIDList(UINT32 FormID, UINT32 **VarIDList);
BOOLEAN _DestinationFormProcessed(UINT32 FormID);
BOOLEAN IsNoSubmitOfForms(UINT32 FormID, UINT16* ErrToken);
EFI_STATUS CheckforNosubmitIf(UINT32 FormID, BOOLEAN Recursive, UINT16 *ErrToken);
EFI_STATUS _GetControlFlag(UINT32 PgIndex, UINT32 CtrlIndex, CONTROL_FLAGS **CtrlFlag);
EFI_STATUS GetNumOfPages(UINT32 *NumOfPages);
EFI_STATUS GetPageTitle(UINT32 PgIndex, UINT16 *TitleToken);
EFI_STATUS GetPageSubTitle(UINT32 PgIndex, UINT16 *SubtitleToken);
EFI_STATUS GetPageHandle(UINT32 PgIndex, EFI_HII_HANDLE  *Handle);
EFI_STATUS GetPageIdIndex(UINT32 PgIndex, UINT16 *PgIdIndex);
BOOLEAN IsParentPage(UINT32 PgIndex);
EFI_STATUS GetPageHiiId(UINT32 PgIndex, UINT16 *PageId);
EFI_STATUS GetPageParentId(UINT32 PgIndex, UINT16 *PageParentId);
EFI_STATUS GetCtrlsDestCtrlId(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *DestCtrlId);
EFI_STATUS GetControlType(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *CtrlType);
EFI_STATUS GetControlQuestionPrompt(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *CtrlPrompt);
EFI_STATUS GetControlHelpStr(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *CtrlHelp);
BOOLEAN IsControlCheckBox(UINT32 PgIndex, UINT32 CtrlIndex);
EFI_STATUS GetControlDataWidth(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *CtrlWidth);
EFI_STATUS GetControlDestinationPage(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *CtrlDestPg);
UINT8 GetControlAccess(UINT32 PgIndex, UINT32 CtrlIndex);
UINT8 GetControlRefresh(UINT32 PgIndex, UINT32 CtrlIndex);
BOOLEAN IsControlVisible(UINT32 PgIndex, UINT32 CtrlIndex);
BOOLEAN IsControlResetRequired(UINT32 PgIndex, UINT32 CtrlIndex);
BOOLEAN IsControlReadOnly(UINT32 PgIndex, UINT32 CtrlIndex);
BOOLEAN IsControlInteractive(UINT32 PgIndex, UINT32 CtrlIndex);
EFI_STATUS EvaluateControlCondition(UINT32 PgIndex, UINT32 CtrlIndex, UINTN *ExpResult);
BOOLEAN IsControlReConnect(UINT32 PgIndex, UINT32 CtrlIndex);
EFI_STATUS GetQuestionValue(UINT32 PgIndex, UINT32 CtrlIndex, UINT8 **Value, UINTN *ValueSize);
EFI_STATUS SetQuestionValue(UINT32 PgIndex, UINT32 CtrlIndex, UINT8 *Value, UINTN ValueSize, UINT16 *ErrStrToken);
EFI_STATUS SaveQuestionValues(UINT32 FormID);
VOID FreeOptionList(OPTION_LIST *OptionArray);
EFI_STATUS GetControlDisplayFormat(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *Format);
EFI_STATUS GetControlMinMax(UINT32 PgIndex, UINT32 CtrlIndex, UINT64 *Min, UINT64 *Max);
EFI_STATUS GetControlDefault(UINT32 PgIndex, UINT32 CtrlIndex,UINT8 *Def, BOOLEAN Optimal);
EFI_STATUS GetOrderedListMaxEntries(UINT32 PgIndex, UINT32 CtrlIndex, UINT8 *MaxEntry);
EFI_STATUS GetOrderedListOptionDataWidth(CONTROL_INFO *ControlInfo, UINT32 *DataWidth);
EFI_STATUS GetOrderedListDataTypeWidth(UINT32 PgIndex, UINT32 CtrlIndex, UINT32 *DataWidth);
BOOLEAN IsPageRefreshable(UINT32 PgIndex);
BOOLEAN IsPageModal(UINT32 PgIndex);
BOOLEAN IfNoCommitVariable(UINT32 PgIndex, UINT32 CtrlIndex);
BOOLEAN IdentifyFormHasDefault(UINT32 FormID);
BOOLEAN IsFormsetHasDefault(UINT32 FormID);
EFI_STATUS GetPageGuidFromPgIndex(UINT32 PgIndex, EFI_GUID *FormGuid);
void AddEntryIngProcessedForms (UINT32 FormID);
void CleargProcessedForms (void);
//----------------------------------------------------------------------------

#define _GET_MINMAXSTEP_DATA(b,c,d)			( \
				((c)==EFI_IFR_NUMERIC_SIZE_1)?((b).u8.d):( 	\
				((c)==EFI_IFR_NUMERIC_SIZE_2)?((b).u16.d):( \
				((c)==EFI_IFR_NUMERIC_SIZE_4)?((b).u32.d): 	\
				((b).u64.d) )))

#define TSE_GET_NUMERIC_MINVALUE(a)			_GET_MINMAXSTEP_DATA((((EFI_IFR_NUMERIC*)(a))->data),(((EFI_IFR_NUMERIC*)(a))->Flags&EFI_IFR_NUMERIC_SIZE),MinValue)
#define TSE_GET_NUMERIC_MAXVALUE(a)			_GET_MINMAXSTEP_DATA((((EFI_IFR_NUMERIC*)(a))->data),(((EFI_IFR_NUMERIC*)(a))->Flags&EFI_IFR_NUMERIC_SIZE),MaxValue)
#define TSE_GET_NUMERIC_STEP(a)				_GET_MINMAXSTEP_DATA((((EFI_IFR_NUMERIC*)(a))->data),(((EFI_IFR_NUMERIC*)(a))->Flags&EFI_IFR_NUMERIC_SIZE),Step)
#define TSE_GET_NUMERIC_BASE(a)				(((((EFI_IFR_NUMERIC*)a)->Flags&EFI_IFR_DISPLAY)==EFI_IFR_DISPLAY_UINT_HEX)?AMI_BASE_HEX:((((EFI_IFR_NUMERIC*)a)->Flags&EFI_IFR_DISPLAY)==EFI_IFR_DISPLAY_INT_DEC) ? AMI_BASE_INT_DEC :AMI_BASE_DEC)

//----------------------------------------------------------------------------

/**

    @param IfrPtr 

    @retval UINT16
**/
UINT16 UefiGetHelpField(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT16 Token = 0;

  if(IfrPtr == NULL){
	  return Token;
  }
  switch(OpHeader->OpCode)
  {
  case EFI_IFR_REF_OP:
    Token = ((EFI_IFR_REF *)OpHeader)->Question.Header.Help;
    break;
  case EFI_IFR_TEXT_OP:
    Token = ((EFI_IFR_TEXT *)OpHeader)->Statement.Help;
    break;
  case EFI_IFR_ONE_OF_OP:
    Token = ((EFI_IFR_ONE_OF *)OpHeader)->Question.Header.Help;
    break;
  case EFI_IFR_TIME_OP:
    Token = ((EFI_IFR_TIME *)OpHeader)->Question.Header.Help;
    break;
  case EFI_IFR_DATE_OP:
    Token = ((EFI_IFR_DATE *)OpHeader)->Question.Header.Help;
    break;
  case EFI_IFR_NUMERIC_OP:
    Token = ((EFI_IFR_NUMERIC *)OpHeader)->Question.Header.Help;
    break;
  case EFI_IFR_PASSWORD_OP:
    Token = ((EFI_IFR_PASSWORD *)OpHeader)->Question.Header.Help;
    break;
  case EFI_IFR_ACTION_OP:
    Token = ((EFI_IFR_ACTION *)OpHeader)->Question.Header.Help;
    break;
  case EFI_IFR_STRING_OP:
    Token = ((EFI_IFR_STRING *)OpHeader)->Question.Header.Help;
    break;
  case EFI_IFR_RESET_BUTTON_OP:
	 Token = ((EFI_IFR_RESET_BUTTON *)OpHeader)->Statement.Help;//Updated the Help field for Reset Button
    break;
  case EFI_IFR_ORDERED_LIST_OP:
	 Token = ((EFI_IFR_ORDERED_LIST *)OpHeader)->Question.Header.Help;//Updated the Help field for order list control.
    break;
  case  EFI_IFR_CHECKBOX_OP:
  	 Token = ((EFI_IFR_CHECKBOX *)OpHeader)->Question.Header.Help;
  	 break;
  default:
    break;
  }

  return Token;
}

/**

    @param IfrPtr 

    @retval UINT16
**/
UINT16 UefiGetPromptField(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT16 Token = 0;

  if(IfrPtr == NULL){
	  return Token;
  }
  switch(OpHeader->OpCode)
  {
  case EFI_IFR_REF_OP:
    Token = ((EFI_IFR_REF *)OpHeader)->Question.Header.Prompt;
    break;
  case EFI_IFR_NUMERIC_OP:
    Token = ((EFI_IFR_NUMERIC *)OpHeader)->Question.Header.Prompt;
    break;
  case EFI_IFR_TIME_OP:
    Token = ((EFI_IFR_TIME *)OpHeader)->Question.Header.Prompt;
    break;
  case EFI_IFR_DATE_OP:
    Token = ((EFI_IFR_DATE *)OpHeader)->Question.Header.Prompt;
    break;
  case EFI_IFR_ONE_OF_OP:
    Token = ((EFI_IFR_ONE_OF *)OpHeader)->Question.Header.Prompt;
    break;
  case EFI_IFR_ORDERED_LIST_OP:
    Token = ((EFI_IFR_ORDERED_LIST *)OpHeader)->Question.Header.Prompt;
    break;
  case EFI_IFR_CHECKBOX_OP:
    Token = ((EFI_IFR_CHECKBOX *)OpHeader)->Question.Header.Prompt;
    break;
  case EFI_IFR_PASSWORD_OP:
    Token = ((EFI_IFR_PASSWORD *)OpHeader)->Question.Header.Prompt;
    break;
  case EFI_IFR_ACTION_OP:
    Token = ((EFI_IFR_ACTION *)OpHeader)->Question.Header.Prompt;
    break;
  case EFI_IFR_STRING_OP:
    Token = ((EFI_IFR_STRING *)OpHeader)->Question.Header.Prompt;
    break;
  case EFI_IFR_RESET_BUTTON_OP:
	 Token = ((EFI_IFR_RESET_BUTTON *)OpHeader)->Statement.Prompt;// Updated the Prompt field for Reset Button
    break;
  default:
    break;
  }

  return Token;
}

/**

    @param IfrPtr 

    @retval UINT16
**/
UINT16 UefiGetKeyField(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT16 Token = 0;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_REF_OP:
    Token = ((EFI_IFR_REF *)IfrPtr)->Question.QuestionId;
    break;
  case EFI_IFR_NUMERIC_OP:
    Token = ((EFI_IFR_NUMERIC *)IfrPtr)->Question.QuestionId;
    break;
  case EFI_IFR_STRING_OP:
    Token = ((EFI_IFR_STRING *)IfrPtr)->Question.QuestionId;
    break;

  case EFI_IFR_PASSWORD_OP:
    Token = ((EFI_IFR_PASSWORD *)IfrPtr)->Question.QuestionId;
    break;
  case EFI_IFR_CHECKBOX_OP:
    Token = ((EFI_IFR_CHECKBOX *)IfrPtr)->Question.QuestionId;
    break;
  case EFI_IFR_ACTION_OP:
    Token = ((EFI_IFR_ACTION *)IfrPtr)->Question.QuestionId;
    break;
  case EFI_IFR_DATE_OP:
    Token = ((EFI_IFR_DATE *)IfrPtr)->Question.QuestionId;
    break;
  case EFI_IFR_ONE_OF_OP:
    Token = ((EFI_IFR_ONE_OF *)IfrPtr)->Question.QuestionId;
    break;
  case EFI_IFR_ORDERED_LIST_OP:
    Token = ((EFI_IFR_ORDERED_LIST *)IfrPtr)->Question.QuestionId;
    break;
  case EFI_IFR_TIME_OP:
    Token = ((EFI_IFR_TIME *)IfrPtr)->Question.QuestionId;
    break;
  default:
    break;
  }

  return Token;
}

/**

    @param IfrPtr 

    @retval UINT16
**/
UINT16 UefiGetTitleField(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT16 Token = 0;

//  Minisetup crashes after controller reconnection during Hii callback
  if(IfrPtr == NULL){
      goto DONE;
  }
  switch(OpHeader->OpCode)
  {
  case EFI_IFR_SUBTITLE_OP:
    Token = ((EFI_IFR_SUBTITLE *)IfrPtr)->Statement.Prompt;
    break;
  case EFI_IFR_FORM_SET_OP:
    Token = ((EFI_IFR_FORM_SET *)IfrPtr)->FormSetTitle;
    break;
  case EFI_IFR_FORM_OP:
    Token = ((EFI_IFR_FORM *)IfrPtr)->FormTitle;
    break;
  default:
    break;
  }

DONE:
  return Token;
}

/**

    @param IfrPtr 

    @retval UINT8
**/
UINT8 UefiGetFlagsField(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT8 Flag = 0;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_REF_OP:
    Flag = ((EFI_IFR_REF *)IfrPtr)->Question.Flags;
    break;
  case EFI_IFR_TIME_OP:
	Flag = ((EFI_IFR_TIME *)IfrPtr)->Flags;
   	break;
  case EFI_IFR_DATE_OP:
	Flag = ((EFI_IFR_DATE *)IfrPtr)->Flags;
   	break;
  default:
    break;
  }

  return Flag;
}

/**

    @param ControlData 

    @retval BOOLEAN
**/
BOOLEAN UefiIsInteractive(CONTROL_INFO *ControlData)
{
  return (BOOLEAN)(ControlData->ControlFlags.ControlInteractive);
}

/**

    @param ControlData 

    @retval UINT16
**/
UINT16 UefiGetControlKey(CONTROL_INFO *ControlData)
{
  return 	ControlData->ControlKey;
}

/**

    @param IfrPtr 

    @retval UINT16
**/
UINT16 UefiGetTextField(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT16 Token = 0;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_TEXT_OP:
    Token = ((EFI_IFR_TEXT*)OpHeader)->Statement.Prompt;
    break;
  default:
    break;
  }

  return Token;
}

/**

    @param IfrPtr 

    @retval UINT16
**/
UINT16 UefiGetTextTwoField(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT16 Token = 0;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_TEXT_OP:
    Token = ((EFI_IFR_TEXT*)OpHeader)->TextTwo;
    break;
  default:
    break;
  }

  return Token;
}

/**

    @param IfrPtr UINT16 Token

    @retval VOID
**/
VOID UefiSetSubTitleField(VOID *IfrPtr,UINT16 Token)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_SUBTITLE_OP:
    ((EFI_IFR_SUBTITLE*)OpHeader)->Statement.Prompt = Token;
    break;
  default:
    break;
  }
}
/**

    @param IfrPtr UINT16 Token

    @retval VOID
**/
VOID UefiSetHelpField(VOID *IfrPtr, UINT16 Token)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  
  switch(OpHeader->OpCode)
  {
  case EFI_IFR_REF_OP:
    ((EFI_IFR_REF *)OpHeader)->Question.Header.Help = Token ;
    break;
  case EFI_IFR_TEXT_OP:
    ((EFI_IFR_TEXT *)OpHeader)->Statement.Help = Token ;
    break;
  case EFI_IFR_ONE_OF_OP:
    ((EFI_IFR_ONE_OF *)OpHeader)->Question.Header.Help = Token ;
    break;
  case EFI_IFR_TIME_OP:
    ((EFI_IFR_TIME *)OpHeader)->Question.Header.Help = Token ;
    break;
  case EFI_IFR_DATE_OP:
    ((EFI_IFR_DATE *)OpHeader)->Question.Header.Help = Token ;
    break;
  case EFI_IFR_NUMERIC_OP:
    ((EFI_IFR_NUMERIC *)OpHeader)->Question.Header.Help = Token ;
    break;
  case EFI_IFR_PASSWORD_OP:
    ((EFI_IFR_PASSWORD *)OpHeader)->Question.Header.Help = Token ;
    break;
  case EFI_IFR_ACTION_OP:
    ((EFI_IFR_ACTION *)OpHeader)->Question.Header.Help = Token ;
    break;
  case EFI_IFR_STRING_OP:
    ((EFI_IFR_STRING *)OpHeader)->Question.Header.Help = Token ;
    break;
  case EFI_IFR_RESET_BUTTON_OP:
	((EFI_IFR_RESET_BUTTON *)OpHeader)->Statement.Help = Token ;
    break;
  default:
    break;
  }
  
}
/**

    @param IfrPtr UINT16 Token

    @retval VOID
**/
VOID UefiSetPromptField(VOID *IfrPtr,UINT16 Token)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_REF_OP:
    ((EFI_IFR_REF*)OpHeader)->Question.Header.Prompt = Token;
    break;
  case EFI_IFR_ONE_OF_OP:
  case EFI_IFR_NUMERIC_OP:
  case EFI_IFR_TIME_OP:
  case EFI_IFR_DATE_OP:
  case EFI_IFR_ORDERED_LIST_OP:
  case EFI_IFR_CHECKBOX_OP:
  case EFI_IFR_PASSWORD_OP:
  case EFI_IFR_STRING_OP:
  {
    EFI_IFR_QUESTION_HEADER *Question = (EFI_IFR_QUESTION_HEADER *)((UINT8 *)OpHeader + sizeof(EFI_IFR_OP_HEADER));

    Question->Header.Prompt = Token;
  }
    break;
  default:
    break;
  }
}

/**

    @param Token 

    @retval VOID *
**/
VOID * UefiCreateSubTitleTemplate(UINT16 Token)
{
  EFI_IFR_OP_HEADER *OpHeader = EfiLibAllocateZeroPool(sizeof(EFI_IFR_SUBTITLE));

  OpHeader->OpCode = EFI_IFR_SUBTITLE_OP ;
  OpHeader->Length = sizeof(EFI_IFR_SUBTITLE) ;
  UefiSetSubTitleField((VOID *)OpHeader,Token);
  return (VOID*)OpHeader;
}
/**
    Function to create uefi string template

    @param Token 

    @retval VOID *
**/
VOID * UefiCreateStringTemplate(UINT16 Token)
{
  EFI_IFR_OP_HEADER *OpHeader = EfiLibAllocateZeroPool(sizeof(EFI_IFR_STRING));

  OpHeader->OpCode = EFI_IFR_STRING_OP ;
  OpHeader->Length = sizeof(EFI_IFR_STRING) ;
  UefiSetPromptField((VOID *)OpHeader,Token);
  return (VOID*)OpHeader;
}
/**

    @param IfrPtr 

    @retval UINT8
**/
UINT8 UefiGetIfrLength(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;

  return OpHeader->Length;
}

/**

    @param IfrPtr 

    @retval UINT64
**/
UINT64 UefiGetMinValue(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT64 Min = 0;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_NUMERIC_OP:
    // Should The type be Taken Into Account? //
    //Min =  (UINT64)((EFI_IFR_NUMERIC*)OpHeader)->data.u64.MinValue;
	Min = TSE_GET_NUMERIC_MINVALUE(IfrPtr);
	break;
  case EFI_IFR_PASSWORD_OP:
    Min =  (UINT64)((EFI_IFR_PASSWORD*)OpHeader)->MinSize;
    break;
  case EFI_IFR_STRING_OP:
    Min =  (UINT64)((EFI_IFR_STRING*)OpHeader)->MinSize;
    break;
  default:
    break;
  }
  return Min;

}

/**

    @param IfrPtr 

    @retval UINT64
**/
UINT64 UefiGetMaxValue(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT64 Max = 0;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_NUMERIC_OP:
    // Should The type be Taken Into Account? //
    //Max = (UINT64)((EFI_IFR_NUMERIC*)OpHeader)->data.u64.MaxValue;
	Max = TSE_GET_NUMERIC_MAXVALUE(IfrPtr);
    break;
  case EFI_IFR_PASSWORD_OP:
    Max = (UINT64)((EFI_IFR_PASSWORD*)OpHeader)->MaxSize;
    break;
  case EFI_IFR_STRING_OP:
    Max = (UINT64)((EFI_IFR_STRING*)OpHeader)->MaxSize;
    break;
  default:
    break;
  }
  return Max;
}

/**

    @param IfrPtr 

    @retval UINT64
**/
UINT64 UefiGetStepValue(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT64 Step = 0;
  switch(OpHeader->OpCode)
  {
  case EFI_IFR_NUMERIC_OP:
    // Should The type be Taken Into Account? //
    //Step = (UINT64)((EFI_IFR_NUMERIC*)OpHeader)->data.u64.Step;
    Step = TSE_GET_NUMERIC_STEP(IfrPtr);
    break;
  default:
    break;
  }
  return Step;
}

/**

    @param IfrPtr 

    @retval UINT8
**/
UINT8 UefiGetBaseValue(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT8 Base = EFI_IFR_DISPLAY_UINT_DEC;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_NUMERIC_OP:
    //Base = (((EFI_IFR_NUMERIC *)OpHeader)->Flags & EFI_IFR_DISPLAY);
	Base = TSE_GET_NUMERIC_BASE(IfrPtr);
    break;
  default:
    break;
  }
  return Base;
}

/**

    @param IfrPtr 

    @retval UINT16
**/
UINT16 UefiGetWidth(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT16 Width = 0;

  if (NULL == OpHeader) // Avoid setup crash
	  return Width;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_NUMERIC_OP:
  case EFI_IFR_ONE_OF_OP:
    Width = (UINT16)(1 << (((EFI_IFR_NUMERIC*)OpHeader)->Flags & EFI_IFR_NUMERIC_SIZE));
    break;
  case EFI_IFR_PASSWORD_OP:
    Width = (UINT16)((EFI_IFR_PASSWORD*)OpHeader)->MaxSize * sizeof(CHAR16);
    break;
  case EFI_IFR_STRING_OP:
    Width = (UINT16)((EFI_IFR_STRING*)OpHeader)->MaxSize * sizeof(CHAR16);
    break;
  case EFI_IFR_CHECKBOX_OP:
    Width = sizeof(UINT8);
    break;
  case EFI_IFR_DATE_OP:
    Width = sizeof(EFI_HII_DATE);
    break;
  case EFI_IFR_TIME_OP:
    Width = sizeof(EFI_HII_TIME);
    break;
  case EFI_IFR_EQ_ID_VAL_OP:
  case EFI_IFR_EQ_ID_LIST_OP:
  case EFI_IFR_EQ_ID_ID_OP:
    ASSERT(1);
    break;
  default:
    break;
  }
  return Width;
}

/**

    @param IfrPtr 

    @retval UINT16
**/
extern UINT32	gtempCurrentPage;
UINT16 UefiGetQuestionOffset(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  EFI_IFR_QUESTION_HEADER *QHeader = NULL;
  UINT16 VarOffset = 0;
  UINT32 iIndex = 0;
  VARIABLE_INFO *Variable = NULL;

  if (NULL == OpHeader) //Avoid setup crash
	  return VarOffset;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_CHECKBOX_OP:
  case EFI_IFR_REF_OP:
  case EFI_IFR_RESET_BUTTON_OP:
  case EFI_IFR_ACTION_OP:
  case EFI_IFR_DATE_OP:
  case EFI_IFR_NUMERIC_OP:
  case EFI_IFR_ONE_OF_OP:
  case EFI_IFR_STRING_OP:
  case EFI_IFR_PASSWORD_OP:
  case EFI_IFR_ORDERED_LIST_OP:
  case EFI_IFR_TIME_OP:
    QHeader = (EFI_IFR_QUESTION_HEADER *)((UINT8*)OpHeader + sizeof(EFI_IFR_OP_HEADER));
   for (iIndex = 0; iIndex < gVariables->VariableCount; iIndex++)
   {
      Variable = (VARIABLE_INFO*)((UINT8 *)gVariables + gVariables->VariableList [iIndex]);
      if ((VARIABLE_ATTRIBUTE_NAMEVALUE == (Variable->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE)))
      {
	      UINT32  page = (NULL == gApp ? gtempCurrentPage : gApp->CurrentPage);
	      PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList [page]);
	      
			//Always dont depend on VariableID alone it will be same for different handles, compare handle also
	      if (pageInfo && (pageInfo->PageHandle == Variable->VariableHandle) && (Variable->VariableID == QHeader->VarStoreId))
	      {
		      return 0;
	      }
      }
   }
    VarOffset = QHeader->VarStoreInfo.VarOffset;
    break;
  default:
    break;
  }
  return VarOffset;
}

/**

    @param IfrPtr UINT8 Type,UINT16 * Help,UINT16 * Min,UINT16 * Max

    @retval VOID
**/
VOID UefiGetDateTimeDetails(VOID *IfrPtr,UINT8 Type,UINT16 * Help,UINT16 * Min,UINT16 * Max)
{
/*
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  EFI_IFR_NUMERIC *NumIfr=NULL;
  switch(OpHeader->OpCode)
  {
  case EFI_IFR_TIME_OP:
    if(Type == AMI_TIME_HOUR)
      NumIfr = &(((EFI_IFR_TIME*)OpHeader)->Hour);
    else if (Type == AMI_TIME_MIN)
      NumIfr = &(((EFI_IFR_TIME*)OpHeader)->Minute);
    else if (Type == AMI_TIME_SEC)
      NumIfr = &(((EFI_IFR_TIME*)OpHeader)->Second);
    break;
  case EFI_IFR_DATE_OP:
    if(Type == AMI_DATE_YEAR)
      NumIfr = &(((EFI_IFR_DATE*)OpHeader)->Year);
    else if (Type == AMI_DATE_MONTH)
      NumIfr = &(((EFI_IFR_DATE*)OpHeader)->Month);
    else if (Type == AMI_DATE_DAY)
      NumIfr = &(((EFI_IFR_DATE*)OpHeader)->Day);
    break;
  default:
    break;
  }
  if(NumIfr)
  {
    *Help = NumIfr->Help;
    *Min = NumIfr->Minimum;
    *Max = NumIfr->Maximum;
  }*/

  *Min = 0;
  *Max = 0xFFFF;//Initializing the MIN/MAX values for date and time.

}

/**

    @param IfrPtr VOID **Handle,UINT16  **OptionPtrTokens,
    @param ValuePtrTokens UINT16 *ItemCount,
    @param Interactive UINT16 *CallBackKey

    @retval EFI_STATUS
**/
EFI_STATUS UefiGetOneOfOptions(CONTROL_INFO *CtrlInfo, VOID **Handle, UINT16 **OptionPtrTokens,
                               UINT64 **ValuePtrTokens, UINT16 *ItemCount,
                               UINT16 *Interactive,UINT16 *CallBackKey )
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)(CtrlInfo->ControlPtr);
  EFI_IFR_QUESTION_HEADER *QHeader;
  EFI_STATUS Status = EFI_SUCCESS;
  UINT16 Index= 0;
  UINT8 Opcode = OpHeader->OpCode;
  UINT8 MaxItems = 10;

  if(OpHeader->OpCode == EFI_IFR_CHECKBOX_OP)
  {
    MaxItems = 2;
    *Handle = gHiiHandle;
  }

  // Evaluate Interactive and Callback
  QHeader = (EFI_IFR_QUESTION_HEADER *)((UINT8 *)OpHeader + sizeof(EFI_IFR_OP_HEADER));
  if(QHeader->Flags & EFI_IFR_FLAG_CALLBACK)
  {
    if(Interactive)
      *Interactive = TRUE;
    if(CallBackKey)
      *CallBackKey = QHeader->QuestionId;
  }

  // Free Buffers if Allocated and Allocate afresh
  if( OptionPtrTokens != NULL )
  {
    if( *OptionPtrTokens != NULL )
    MemFreePointer((VOID **)OptionPtrTokens);
  }

  if( ValuePtrTokens != NULL )
  {
	if( *ValuePtrTokens != NULL )
		MemFreePointer((VOID **)ValuePtrTokens);
  }

	if( NULL != OptionPtrTokens )
	{
		*OptionPtrTokens = (UINT16 *)EfiLibAllocateZeroPool(MaxItems * sizeof(UINT16));
	}

	if(ValuePtrTokens)
		*ValuePtrTokens = (UINT64 *)EfiLibAllocateZeroPool(MaxItems * sizeof(UINT64));

	if ( NULL == OptionPtrTokens || NULL == *OptionPtrTokens )
	{
		Status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

  //Populate Options
  if(Opcode == EFI_IFR_CHECKBOX_OP)
  {
    *ItemCount = MaxItems;
	for(Index = 0; Index < MaxItems; Index++)
	{
		(*OptionPtrTokens)[Index] = gCheckboxTokens[Index];
		if(ValuePtrTokens)
      		(*ValuePtrTokens)[Index] = Index;
	}
  }
  else
  {
    *ItemCount = 0;
    _UefiGetValidOptions(CtrlInfo, OptionPtrTokens, ValuePtrTokens, ItemCount, MaxItems);
  }

DONE:
  return Status;
}

/**

    @param CtrlInfo UINT16 **OptionPtrTokens,
    @param ValuePtrTokens UINT16 *ItemCount,UINT8 MaxItems

    @retval VOID
**/
VOID _UefiGetValidOptions(CONTROL_INFO *CtrlInfo, UINT16 **OptionPtrTokens,
                                UINT64 **ValuePtrTokens, UINT16 *ItemCount,UINT8 MaxItems)
{
  EFI_IFR_OP_HEADER *OpHeader = (EFI_IFR_OP_HEADER *)CtrlInfo->ControlPtr;
  UINT32 Offset = 0;
  INT32 InScope = 0;
  UINT16 Max = (UINT16)MaxItems;
  UINT16 Itr = 0;
  UINT8 suppress = FALSE;
  UINT8 condition = COND_NONE;
  INT32 suppressScopeCount = 0;
  UINT8 *conditionPtr = NULL;

  do
  {
    if(OpHeader->Scope)
    {
      InScope++;
//      suppressScopeCount = suppressScopeCount? ++suppressScopeCount : 0;
      if (suppressScopeCount )
	      ++suppressScopeCount ;
    }
    switch(OpHeader->OpCode)
    {
    case EFI_IFR_SUPPRESS_IF_OP:
        if(suppress == FALSE)
        {
        	conditionPtr = (UINT8 *)OpHeader;
        	suppress = TRUE;
         	suppressScopeCount++;
		}
	  break;
    case EFI_IFR_ONE_OF_OPTION_OP:
		// Evaluate Condition to determine if option is required to be added to list
        condition = suppress && conditionPtr? CheckOneOfOptionSuppressIf(CtrlInfo, conditionPtr, (UINT8*)OpHeader) : COND_NONE;
		if(condition != COND_SUPPRESS)
        {
			// Add Option to Buffer.
			if(Itr == Max)
			{
				*OptionPtrTokens = MemReallocateZeroPool(*OptionPtrTokens, Max * sizeof(UINT16), (Max + 10) * sizeof(UINT16));
				if(ValuePtrTokens)
				{
        			*ValuePtrTokens = MemReallocateZeroPool(*ValuePtrTokens, Max * sizeof(UINT64), (Max + 10) * sizeof(UINT64));
				}
				Max += 10;
			}
			(*OptionPtrTokens)[Itr] = ((EFI_IFR_ONE_OF_OPTION *)OpHeader)->Option;
			if(ValuePtrTokens)
			{
				switch(((EFI_IFR_ONE_OF_OPTION *)OpHeader)->Type)
				{
				case EFI_IFR_TYPE_NUM_SIZE_16:
					(*ValuePtrTokens)[Itr] = ((EFI_IFR_ONE_OF_OPTION *)OpHeader)->Value.u16;
					break;
				case EFI_IFR_TYPE_NUM_SIZE_32:    
					(*ValuePtrTokens)[Itr] = ((EFI_IFR_ONE_OF_OPTION *)OpHeader)->Value.u32;
					break;
				case EFI_IFR_TYPE_NUM_SIZE_64:
					(*ValuePtrTokens)[Itr] = ((EFI_IFR_ONE_OF_OPTION *)OpHeader)->Value.u64;
					break;
				case EFI_IFR_TYPE_NUM_SIZE_8:
				default:
					(*ValuePtrTokens)[Itr] = ((EFI_IFR_ONE_OF_OPTION *)OpHeader)->Value.u8;
					break;
				}
			}
			Itr++;
		}
      break;
    case EFI_IFR_END_OP:
//		InScope = InScope > 0? --InScope : 0;
	    if (InScope > 0)
	    {
		    --InScope;
	    }
	    else
	    {
		    InScope  = 0;
	    }
       // suppressScopeCount = suppressScopeCount > 0? --suppressScopeCount : 0;
	    if (suppressScopeCount>0)
	    {
		    --suppressScopeCount;
	    }
	    else
	    {
		    suppressScopeCount = 0;
	    }
        suppress = suppressScopeCount == 0? FALSE : suppress;
        conditionPtr = suppress == TRUE? conditionPtr : NULL;
    default:
      break;
    }

    // Move to Next Opcode
    Offset += OpHeader->Length;
    OpHeader = (EFI_IFR_OP_HEADER *)((UINT8*)CtrlInfo->ControlPtr + Offset);
  }while(InScope);
  *ItemCount = Itr;
}

/**

    @param CtrlInfo, POSTMENU_TEMPLATE* ItemInfo, MaxItems
    @retval EFI_STATUS
    
**/

EFI_STATUS UefiGetValidOptionsCondition(CONTROL_INFO *CtrlInfo, POSTMENU_TEMPLATE* ItemInfo, UINT16 MaxItems)
{
  EFI_IFR_OP_HEADER *OpHeader = (EFI_IFR_OP_HEADER *)CtrlInfo->ControlPtr;
  UINT32 Offset = 0;
  INT32 InScope = 0;
  UINT16 Max = (UINT16)MaxItems;
  UINT16 Itr = 0;
  UINT8 ConditionExists = FALSE;
  UINT8 Condition = COND_NONE;
  INT32 CondScopeCount = 0;
  UINT8 *conditionPtr = NULL;
  EFI_STATUS Status = EFI_UNSUPPORTED;

  do
  {
    if(OpHeader->Scope)
    {
      InScope++;
      if (CondScopeCount )
	      ++CondScopeCount ;	

    }
    switch(OpHeader->OpCode)
    {
    case EFI_IFR_SUPPRESS_IF_OP:
    case EFI_IFR_GRAYOUT_IF_OP:
        if(ConditionExists == FALSE)
        {
        	conditionPtr = (UINT8 *)OpHeader;
        	ConditionExists = TRUE;
        }
        CondScopeCount++;
	  break;
    case EFI_IFR_ONE_OF_OPTION_OP:
		// Evaluate Condition to determine if option is required to be added to list
     	Condition = ConditionExists && conditionPtr? CheckOneOfOptionSuppressIf(CtrlInfo, conditionPtr, (UINT8*)OpHeader) : COND_NONE;
		if(Condition != COND_SUPPRESS)
        {
			if ((Condition == COND_GRAYOUT)&& (Itr < MaxItems))
			{
				ItemInfo[Itr].Attribute = AMI_POSTMENU_ATTRIB_NON_FOCUS;
				ItemInfo[Itr].ItemToken =((EFI_IFR_ONE_OF_OPTION *)OpHeader)->Option;
				Status = EFI_SUCCESS; 
			}
			Itr++;
		}
      break;
    case EFI_IFR_END_OP:
	    if (InScope > 0)
	    {
		    --InScope;
	    }
	    else
	    {
		    InScope  = 0;
	    }
	    if (CondScopeCount>0)
	    {
		    --CondScopeCount;
	    }
	    else
	    {
	    	CondScopeCount = 0;
	    }
	    
	    ConditionExists = CondScopeCount == 0? FALSE : ConditionExists;
        conditionPtr = ConditionExists == TRUE? conditionPtr : NULL;
        
    default:
      break;
    }
    // Move to Next Opcode
    Offset += OpHeader->Length;
    OpHeader = (EFI_IFR_OP_HEADER *)((UINT8*)CtrlInfo->ControlPtr + Offset);
  }while(InScope);
  
  return Status;
}

/**
    Assuming all the options in this control has option with
    same type and length

    @param CtrlInfo Pointer to the control info data
    @param Type Returns EFI_IFR_TYPE_NUM_SIZE

    @retval VOID
**/
VOID UefiGetValidOptionType(CONTROL_INFO *CtrlInfo, UINTN *Type, UINT32 *SizeOfData)
{
	EFI_IFR_OP_HEADER *OpHeader = (EFI_IFR_OP_HEADER *)CtrlInfo->ControlPtr;
	UINT32 Offset = 0;
	INT32 InScope = 0;
	UINT8 suppress = FALSE;
	UINT8 condition = COND_NONE;
	INT32 suppressScopeCount = 0;
	UINT8 *conditionPtr = NULL;

	do
	{
		if(OpHeader->Scope)
		{
			InScope++;
//			suppressScopeCount = suppressScopeCount? ++suppressScopeCount : 0;
			if (suppressScopeCount)
			{
				++suppressScopeCount;
			}
			else
			{
				suppressScopeCount = 0;
			}
		}
		switch(OpHeader->OpCode)
		{
			case EFI_IFR_SUPPRESS_IF_OP:
				if(suppress == FALSE)
				{
					conditionPtr = (UINT8 *)OpHeader;
					suppress = TRUE;
					suppressScopeCount++;
				}
				break;
			case EFI_IFR_ONE_OF_OPTION_OP:
				// Evaluate Condition to determine if option is required to be added to list
				condition = suppress && conditionPtr? CheckOneOfOptionSuppressIf(CtrlInfo, conditionPtr, (UINT8*)OpHeader) : COND_NONE;
				if(condition != COND_SUPPRESS)
				{
					switch(((EFI_IFR_ONE_OF_OPTION *)OpHeader)->Type)
					{
						case EFI_IFR_TYPE_NUM_SIZE_16:
							*Type = EFI_IFR_TYPE_NUM_SIZE_16;
							*SizeOfData = sizeof(UINT16);
							goto DONE;
						case EFI_IFR_TYPE_NUM_SIZE_32:    
							*Type = EFI_IFR_TYPE_NUM_SIZE_32;
							*SizeOfData = sizeof(UINT32);
							goto DONE;
						case EFI_IFR_TYPE_NUM_SIZE_64:
							*Type = EFI_IFR_TYPE_NUM_SIZE_64;
							*SizeOfData = sizeof(UINT64);
							goto DONE;
						case EFI_IFR_TYPE_NUM_SIZE_8:
						default:
							*Type = EFI_IFR_TYPE_NUM_SIZE_8;
							*SizeOfData = sizeof(UINT8);
							goto DONE;
					}
				}
				break;
			case EFI_IFR_END_OP:
//				InScope = InScope > 0? --InScope : 0;
				if (InScope>0 )
				{
					--InScope;
				}
				else
				{
					InScope = 0;
				}
//				suppressScopeCount = suppressScopeCount > 0? --suppressScopeCount : 0; Undefined Operation
				if (suppressScopeCount > 0)
				{
					--suppressScopeCount;
				}
				else
				{
					suppressScopeCount = 0;
				}
				suppress = suppressScopeCount == 0? FALSE : suppress;
				conditionPtr = suppress == TRUE? conditionPtr : NULL;
				break;
			default:
				break;
		}
		// Move to Next Opcode
		Offset += OpHeader->Length;
		OpHeader = (EFI_IFR_OP_HEADER *)((UINT8*)CtrlInfo->ControlPtr + Offset);
	}while(InScope);

DONE:
	return;
}

/**

    @param IfrPtr 

    @retval UINT8
**/
UINT8 UefiGetMaxEntries(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
  UINT8 Value = 0;

  switch(OpHeader->OpCode)
  {
  case EFI_IFR_ORDERED_LIST_OP:
    Value = ((EFI_IFR_ORDERED_LIST*)OpHeader)->MaxContainers;
    break;
  default:
    break;
  }
  return Value;
}

/**

    @param IfrPtr 

    @retval UINT16
**/
UINT16 UefiTseLiteGetBootOverRideIndex(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER * OpHeader = (EFI_IFR_OP_HEADER *)IfrPtr;
  EFI_IFR_EQ_ID_VAL_LIST *EqIdList;
  UINT16 Index=0xFFFF;
  UINT8 InScope = 0;

  if ((OpHeader != NULL) && (OpHeader->OpCode == EFI_IFR_SUPPRESS_IF_OP))
  {
    do
    {
      if(OpHeader->Scope)
      {
        InScope++;
      }

      if(OpHeader->OpCode == EFI_IFR_END_OP)
      {
        InScope--;
        if(InScope == 0)
        {
          // EFI_IFR_EQ_ID_LIST_OP opcode not found
          goto DONE;
        }
      }

      OpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)OpHeader + OpHeader->Length);
    }while(OpHeader->OpCode != EFI_IFR_EQ_ID_LIST_OP);

    // FIX ME : Do it better. Check the list length and 
    if(OpHeader->OpCode == EFI_IFR_EQ_ID_LIST_OP)
    {
      EqIdList = (EFI_IFR_EQ_ID_VAL_LIST*)OpHeader;
      Index = EqIdList->ListLength - 1;
    }
  }

DONE:
  return Index;
}

/**

    @param Ptr 

    @retval UINT16
**/
UINT16 UefiTseLiteGetAmiCallbackIndex(VOID * Ptr, VOID * CtrlPtr)
{
  EFI_IFR_OP_HEADER * OpHeader = (EFI_IFR_OP_HEADER *)Ptr;
  UINT16 Index=0xFFFF;
  UINT8 InScope = 0;
  VOID *SupressIfPtr = NULL;

  if ((OpHeader != NULL) && (OpHeader->OpCode == EFI_IFR_SUPPRESS_IF_OP))
  {
    // Locate the Last supress If till the control Ptr
    do
    {
      if(OpHeader->Scope)
      {
        InScope++;
      }
      if(OpHeader->OpCode == EFI_IFR_END_OP)
      {
        InScope--;
        if(InScope == 0)
        {
          // EFI_IFR_EQ_ID_VAL_OP opcode not found
          goto DONE;
        }
      }
      if(OpHeader->OpCode == EFI_IFR_SUPPRESS_IF_OP)
        SupressIfPtr =  (VOID*)OpHeader;
      OpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)OpHeader + OpHeader->Length);
    }while((UINTN)OpHeader < (UINTN)CtrlPtr);

    OpHeader = (EFI_IFR_OP_HEADER*)SupressIfPtr ;
    do
    {
      if(OpHeader->Scope)
      {
        InScope++;
      }
      if(OpHeader->OpCode == EFI_IFR_END_OP)
      {
        InScope--;
        if(InScope == 0)
        {
          // EFI_IFR_EQ_ID_VAL_OP opcode not found
          goto DONE;
        }
      }
      OpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)OpHeader + OpHeader->Length);
    }while(OpHeader->OpCode != EFI_IFR_EQ_ID_VAL_OP);

    // FIX ME : Do it better. Check the list length and 
    if(OpHeader->OpCode == EFI_IFR_EQ_ID_VAL_OP)
    {
      Index = ((EFI_IFR_EQ_ID_VAL*)OpHeader)->Value;
    }
  }

DONE:
  return Index;
}


/**

    @param name EFI_GUID *guid, UINT32 *attributes,
    @param size VOID **buffer

    @retval EFI_STATUS
**/
EFI_STATUS UefiFormCallbackNVRead(CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size, VOID **buffer)
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  return Status;
}

/**

    @param name EFI_GUID *guid, UINT32 attributes,
    @param buffer UINTN size

    @retval EFI_STATUS
**/
EFI_STATUS UefiFormCallbackNVWrite(CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size)
{
  EFI_STATUS Status = EFI_UNSUPPORTED;

  return Status;
}

/**

    @param VariableInfo VOID **Buffer, UINTN Offset, 
    @param Size 

    @retval EFI_STATUS
**/
EFI_STATUS NameValueGetHexStringAsBuffer (UINT8 **Buffer, CHAR16 *HexStringBuffer, UINTN Size);
EFI_STATUS UefiVarGetNvram(VARIABLE_INFO *VariableInfo, VOID **Buffer, UINTN Offset, UINTN Size)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_STRING configuration = (EFI_STRING)NULL;
	EFI_STRING result = (EFI_STRING)NULL;
	EFI_STRING stringPtr = (EFI_STRING)NULL;
	UINTN length = 1;
	UINT8 *buffer = NULL;
	EFI_GUID EfiHiiConfigAccessProtocolGuid = EFI_HII_CONFIG_ACCESS_PROTOCOL_GUID;
	EFI_HANDLE DriverHandle;
	EFI_HII_CONFIG_ACCESS_PROTOCOL	*ConfigAccess;
//	CHAR16 tmpString = (CHAR16)NULL; cast from pointer to integer of different size
	CHAR16 tmpString = 0;
	EFI_STRING progress = &tmpString;
   BOOLEAN PreservegEnableDrvNotification = FALSE;

	configuration = EfiLibAllocateZeroPool(length);
	if(configuration == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

  //
  // Construct the Configuration Header String
  //
  status = GetConfigHeader(VariableInfo, &configuration, &length);
	if(EFI_ERROR(status) || (configuration == NULL))
	{
		goto DONE;
	}

	if(VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE)
	{
		//Allocate Memory for "&<Name>"
		UINTN len = length + EfiStrLen(L"&") + EfiStrLen(VariableInfo->VariableName) + sizeof(CHAR16);
		
		configuration = MemReallocateZeroPool(configuration, length * sizeof(CHAR16), len * sizeof(CHAR16));
		if(configuration == NULL)
		{
			status = EFI_OUT_OF_RESOURCES;
			goto DONE;
		}

		EfiStrCat(configuration, L"&");
		EfiStrCat(configuration, VariableInfo->VariableName);
        length = len;
	}else
	{
        status = GetBlockName(&configuration, &length, Offset, Size);
		if(EFI_ERROR(status))
		{
			goto DONE;
		}
	}

	SetCallBackControlInfo(VariableInfo->VariableHandle, 0); //Set this varaible handle to allow BrowserCallback

	PreservegEnableDrvNotification = gEnableDrvNotification;
	gEnableDrvNotification = TRUE; //Set to enable notification processing

	if (!(VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE))		// Invoke Config Routing Route Config
	{
		status = gHiiConfigRouting->ExtractConfig (gHiiConfigRouting, configuration, &progress, &result);
	}
	else
	{
		status = gHiiDatabase->GetPackageListHandle (gHiiDatabase, VariableInfo->VariableHandle, (VOID**)&DriverHandle); //Calling the driver which publishes the name value
		if(!EFI_ERROR(status))
		{
			status = gBS->HandleProtocol (DriverHandle, &EfiHiiConfigAccessProtocolGuid , (VOID**)&ConfigAccess);
			if(!EFI_ERROR(status))
			{
				status = ConfigAccess->ExtractConfig (ConfigAccess, configuration, &progress, &result);
			}
		}
	}
  if (!PreservegEnableDrvNotification) // If gEnableDrvNotification is already True Don't touch it
		gEnableDrvNotification = FALSE; //Reset to disable notification processing
	SetCallBackControlInfo(NULL, 0); //Disable update
    if(EFI_ERROR(status) || (progress == (EFI_STRING)NULL))
	{
		goto DONE;
	}

	if(EFI_ERROR(status = _VerifyResponseString(result, (VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE)? TRUE : FALSE)))
  {
    goto DONE;
  }

	stringPtr = result;
	if(VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE)
	{
		for(stringPtr = result; *stringPtr != 0 && EfiStrnCmp(stringPtr, VariableInfo->VariableName, EfiStrLen(VariableInfo->VariableName)) != 0; stringPtr++)
			;
		stringPtr += EfiStrLen(VariableInfo->VariableName);
		stringPtr += EfiStrLen(L"=");
		status = NameValueGetHexStringAsBuffer (&buffer, stringPtr, Size);
	}else
	{
		for(stringPtr = result; *stringPtr != 0 && EfiStrnCmp(stringPtr, L"&VALUE=", EfiStrLen(L"&VALUE=")) != 0; stringPtr++)
			;
		stringPtr += EfiStrLen(L"&VALUE=");
		status = GetHexStringAsBuffer (&buffer, stringPtr);
	}


	if(EFI_ERROR(status))
	{
		goto DONE;
	}

	*Buffer = buffer;

DONE:
  if(configuration)
  {
  MemFreePointer((VOID**)&configuration);
  }
  if(result)
  {
    MemFreePointer((VOID**)&result);
  }
  return status;
}

/**

    @param VariableInfo VOID *Buffer, UINTN Offset,
    @param Size 

    @retval EFI_STATUS
**/
EFI_STATUS NameValueBufToHexString (IN OUT CHAR16 **, IN OUT UINTN *,  IN UINT8 *,  IN UINTN );
EFI_STATUS UefiVarSetNvram(VARIABLE_INFO *VariableInfo, VOID *Buffer, UINTN Offset, UINTN Size)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_STRING configuration = NULL;
	EFI_STRING progress = NULL;
	CHAR16 *tempBuffer = NULL;
	UINTN length = 0;
	UINTN finalLen = 0;
	UINTN bufLen = 1;
	BOOLEAN   PreservegEnableDrvNotification = FALSE;

  //
  //Construct the Configuration Header String
  //
  status = GetConfigHeader(VariableInfo, &configuration, &length);
  if(EFI_ERROR(status) || (configuration == NULL))
  {
    goto DONE;
  }

	if(VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE)
	{
		UINTN len = length + EfiStrLen(L"&") + EfiStrLen(VariableInfo->VariableName) + EfiStrLen(L"=") + sizeof(CHAR16);
		configuration = MemReallocateZeroPool (configuration, length * sizeof(CHAR16), len * sizeof(CHAR16));
		if (configuration == NULL)
		{
			status = EFI_OUT_OF_RESOURCES;
			goto DONE;
		}
		EfiStrCat(configuration, L"&");
		EfiStrCat(configuration, VariableInfo->VariableName);
        EfiStrCat(configuration, L"=");
        length = EfiStrLen (configuration) * sizeof (CHAR16);
	}else
	{
        status = GetBlockName(&configuration, &length, Offset, Size);
		if(EFI_ERROR(status))
		{
			goto DONE;
		}
	}

  //
  // Get the Buffer as Hex String
  //
	if (!(VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE))
	{
		tempBuffer = (UINT16*)EfiLibAllocateZeroPool(bufLen * sizeof(UINT16));
		if(tempBuffer == NULL)
		{
			status = EFI_OUT_OF_RESOURCES;
			goto DONE;
		}
		status = BufToHexString(tempBuffer, &bufLen, Buffer, Size);
		if(EFI_ERROR(status))
		{
			if(status != EFI_BUFFER_TOO_SMALL)
			{
				goto DONE;
			}
			if(tempBuffer)
			{
        		MemFreePointer((VOID**)&tempBuffer);
			}
			tempBuffer = (UINT16*)EfiLibAllocateZeroPool(bufLen * 2);
			if(tempBuffer == NULL)
			{
				status = EFI_OUT_OF_RESOURCES;
				goto DONE;
			}
		}
		status = BufToHexString(tempBuffer, &bufLen, Buffer, Size);
	}
	else
	{
		bufLen = 0;
		status = NameValueBufToHexString (&tempBuffer, &bufLen, Buffer, Size);
	}
	if(EFI_ERROR(status))
	{
		goto DONE;
	}

  //
  // Calculate the total Configuration string length
  // <Config Hdr>&OFFSET=<HexChar>0&WIDTH=<HexChar>&VALUE=<HexChar><NULL>
  // |                       length                |  7   | bufLen |  1  |
  //
    if (!(VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE))
    {
        finalLen = length + (bufLen * 2) + (EfiStrLen(L"&VALUE=") * sizeof(CHAR16));
    }
    else
    {             //<exislen> + <Var val> + <NULL>
        finalLen = length + bufLen + (2 * sizeof (CHAR16));			//Last 2 bytes for appending & and NULL
    }
    configuration = MemReallocateZeroPool(configuration, length, finalLen);
    if(configuration == NULL)
    {
        status = EFI_OUT_OF_RESOURCES;
        goto DONE;
    }
    if (!(VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE))
    {
        EfiStrCat(configuration, L"&VALUE=");
    }
    EfiStrCat(configuration, tempBuffer);
    if (VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE)
    {
        EfiStrCat (configuration, L"&");
    }
    MemFreePointer((VOID**)&tempBuffer);
    PreservegEnableDrvNotification =  gEnableDrvNotification; 
	 gEnableDrvNotification = TRUE; // Set to enable notification processing
    status = gHiiConfigRouting->RouteConfig (gHiiConfigRouting, configuration, &progress);
	 if(!PreservegEnableDrvNotification) // If gEnableDrvNotification is already True Don't touch it
		gEnableDrvNotification = FALSE; //Reset to disable notification processing
    MemFreePointer ((VOID**)&configuration);
DONE:
  return status;
}

/**
    Construct <ConfigHdr> using routing information GUID/NAME/PATH.

    @param ConfigHdr 
    @param StrBufferLen 
    @param Guid 
    @param Name OPTIONAL
    @param DriverHandle 

    @retval EFI_STATUS
**/
EFI_STATUS _ConstructConfigHdr (
  IN OUT CHAR16                *ConfigHdr,
  IN OUT UINTN                 *StrBufferLen,
  IN EFI_GUID                  *Guid,
  IN CHAR16                    *Name, OPTIONAL
  IN EFI_HANDLE                *DriverHandle
  )
{
  EFI_STATUS                Status;
  UINTN                     NameStrLen;
  UINTN                     DevicePathSize;
  UINTN                     BufferSize;
  CHAR16                    *StrPtr;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  if (Name == NULL)
  {
    //
    // There will be no "NAME" in <ConfigHdr> for  Name/Value storage
    //
    NameStrLen = 0;
  } else
  {
    //
    // For buffer storage
    //
    NameStrLen = EfiStrLen (Name);
  }

  //
  // Retrieve DevicePath Protocol associated with this HiiPackageList
  //
  Status = gBS->HandleProtocol (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**)&DevicePath
                  );
  if (EFI_ERROR (Status))
  {
    return Status;
  }

  DevicePathSize = EfiDevicePathSize (DevicePath);

  //
  // GUID=<HexCh>32&NAME=<Char>NameStrLen&PATH=<HexChar>DevicePathStrLen <NULL>
  // | 5  |   32   |  6  |  NameStrLen*4 |  6  |    DevicePathStrLen    | 1 |
  //
  BufferSize = (5 + 32 + 6 + NameStrLen * 4 + 6 + DevicePathSize * 2 + 1) * sizeof (CHAR16);
  if (*StrBufferLen < BufferSize)
  {
    *StrBufferLen = BufferSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StrBufferLen = BufferSize;

  StrPtr = ConfigHdr;

  EfiStrCpy (StrPtr, L"GUID=");
  StrPtr += 5;
  BufferToHexString (StrPtr, (UINT8 *) Guid, sizeof (EFI_GUID));
  StrPtr += 32;

  //
  // Convert name string, e.g. name "ABCD" => "&NAME=0041004200430044"
  //
  EfiStrCpy (StrPtr, L"&NAME=");
  StrPtr += 6;
  if (Name != NULL)
  {
    BufferSize = (NameStrLen * 4 + 1) * sizeof (CHAR16);
    UnicodeToConfigString (StrPtr, &BufferSize, Name);
    StrPtr += (NameStrLen * 4);
  }

  EfiStrCpy (StrPtr, L"&PATH=");
  StrPtr += 6;
  BufferToHexString (StrPtr, (UINT8 *) DevicePath, DevicePathSize);

  return EFI_SUCCESS;
}


/**

    @param VariableInfo EFI_STRING *Configuration, UINTN *Length

    @retval EFI_STATUS
**/
EFI_STATUS GetConfigHeader(VARIABLE_INFO *VariableInfo, EFI_STRING *Configuration, UINTN *Length)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_STRING configuration = NULL;
  EFI_HANDLE driverHandle;
  UINTN length = 50;

  //
  // Initialize Protocols
  //
  status = HiiInitializeProtocol();
	if(EFI_ERROR(status))
	{
		goto DONE;
	}

  //
  // Retrieve the Device Path from Handle
  //
	
#if BUILD_OS != BUILD_OS_LINUX
	status = gHiiDatabase->GetPackageListHandle(gHiiDatabase, VariableInfo->VariableHandle, &driverHandle);
#else	
	status = gHiiDatabase->GetPackageListHandle(gHiiDatabase,(EFI_HII_HANDLE)ReadUnaligned64((const UINT64 *)&( VariableInfo->VariableHandle)), &driverHandle);
#endif	
 
	if(EFI_ERROR(status))
	{
		goto DONE;
	}

	// Allocate static memory of 50 Bytes, which will be freed if not used
	configuration = (UINT16*)EfiLibAllocateZeroPool(50);

  // Construct the Configuration Header String
  //
  status = _ConstructConfigHdr(configuration, &length, &VariableInfo->VariableGuid, VariableInfo->VariableName, driverHandle);
  if(EFI_ERROR(status))
  {
    if(status == EFI_BUFFER_TOO_SMALL)
    {
      MemFreePointer((VOID**)&configuration);
      configuration = (UINT16*)EfiLibAllocateZeroPool(length);
      if(configuration == NULL)
      {
        status = EFI_OUT_OF_RESOURCES;
        goto DONE;
      }

      status = _ConstructConfigHdr(configuration, &length, &VariableInfo->VariableGuid, VariableInfo->VariableName, driverHandle);
    }
  }

	if(EFI_ERROR(status))
	{
		goto DONE;
	}

  if(*Configuration)
  {
    MemFreePointer((VOID**)Configuration);
  }
  *Configuration = configuration;
  *Length = length;

DONE:
  return status;
}

/**

    @param Configuration UINTN *Length, UINTN Size

    @retval EFI_STATUS
**/
EFI_STATUS GetBlockName(EFI_STRING *Configuration, UINTN *Length, UINTN Offset, UINTN Size)
{
	EFI_STATUS status = EFI_SUCCESS;
  UINTN length = 0;
  UINTN finalLen = 0;
	CHAR16 *tempBuffer = NULL;

  length = (EfiStrLen(L"&OFFSET=") + sizeof(Offset) + EfiStrLen(L"&WIDTH=") + sizeof(Size) + 1) * sizeof(CHAR16);
  tempBuffer = (CHAR16*)EfiLibAllocateZeroPool(length);
  if(tempBuffer == NULL)
  {
    status = EFI_OUT_OF_RESOURCES;
    goto DONE;
  }

  SPrint(tempBuffer, length, L"&OFFSET=%x&WIDTH=%x", Offset, Size);

  length = EfiStrLen(*Configuration) * sizeof(CHAR16);
  finalLen = (EfiStrLen(*Configuration) + EfiStrLen(tempBuffer) + 1) * sizeof(CHAR16);
  *Configuration = MemReallocateZeroPool(*Configuration, length, finalLen);
  if(*Configuration == NULL)
  {
    status = EFI_OUT_OF_RESOURCES;
    goto DONE;
  }

  EfiStrCat(*Configuration, tempBuffer);

	MemFreePointer((VOID**)&tempBuffer);
  *Length = finalLen;

DONE:
	return status;
}

/**
    Verifies Correctness of Response String. This is to prevent
    crashing if the result pointer is pointing to an invalid location.

    @param Configuration 

    @retval EFI_STATUS
**/
EFI_STATUS _VerifyResponseString(EFI_STRING Configuration, BOOLEAN NameValue)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_STRING config = Configuration;
  CHAR16 *keyWord[] = {L"GUID=", L"&NAME=", L"&PATH=", L"&OFFSET=",L"&WIDTH=", L"&VALUE="};
  UINT32 i = 0;
  UINT32 count = NameValue? 3 : 6; // If Name Value Check only upto &Path else check until &Value.

  if(EfiStrnCmp(config, keyWord[i], EfiStrLen(keyWord[i])) != 0)
  {
    status = EFI_NOT_FOUND;
    goto DONE;
  }

  for(i = 1; i < count; i++)
  {
    for(;EfiStrnCmp(config, keyWord[i], EfiStrLen(keyWord[i])) != 0 && (config != (Configuration + EfiStrLen(Configuration))); config++)
      ;
    if(EfiStrnCmp(config, keyWord[i], EfiStrLen(keyWord[i])) != 0)
    {
      status = EFI_NOT_FOUND;
      goto DONE;
    }
  }

DONE:
  return status;
}

#if !TSE_USE_EDK_LIBRARY
VOID
EfiLibSafeFreePool (
  IN  VOID             *Buffer
  )
{
MemFreePointer(&Buffer);
}
#endif

/**

    @param Buffer 
    @param HexStringBuffer 
    @param Size 

    @retval EFI_STATUS
**/
EFI_STATUS GetHexStringAsBuffer(UINT8 **Buffer, CHAR16 *HexStringBuffer)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINTN bufLength = 0;
	UINTN convertedStrLen = 0;

	status = HexStringToBuf(*Buffer, &bufLength, HexStringBuffer, &convertedStrLen);
	if(EFI_ERROR(status))
	{
		if(status == EFI_BUFFER_TOO_SMALL)
		{
			if(*Buffer)
			{
				MemFreePointer((VOID**)Buffer);
			}
			*Buffer = EfiLibAllocateZeroPool(bufLength);
			if(*Buffer == NULL)
			{
				status = EFI_OUT_OF_RESOURCES;
				goto DONE;
			}
		}else
		{
			goto DONE;
		}
	  status = HexStringToBuf(*Buffer, &bufLength, HexStringBuffer, &convertedStrLen);
	}

DONE:
	return status;
}

/**

    @param Buffer 
    @param HexStringBuffer 
    @param Size 

    @retval EFI_STATUS
**/
EFI_STATUS NameValueHexStringToBuf (IN OUT UINT8 *, IN OUT UINTN *Len,IN CHAR16 *, OUT UINTN *);
EFI_STATUS NameValueGetHexStringAsBuffer (UINT8 **Buffer, CHAR16 *HexStringBuffer, UINTN Size)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINTN bufLength = 0;
	UINTN convertedStrLen = 0;

	if (Size <= sizeof  (UINT64))
	{
		GetHexStringAsBuffer (Buffer, HexStringBuffer);
	}
	else
	{
		status = NameValueHexStringToBuf (*Buffer, &bufLength, HexStringBuffer, &convertedStrLen);
		if(EFI_ERROR(status))
		{
			if(status == EFI_BUFFER_TOO_SMALL)
			{
				if(*Buffer)
				{
					MemFreePointer(Buffer);
				}
				bufLength = (Size > bufLength) ? Size : bufLength;
				*Buffer = EfiLibAllocateZeroPool(bufLength);
				if(*Buffer == NULL)
				{
					status = EFI_OUT_OF_RESOURCES;
					goto DONE;
				}
			}else
			{
				goto DONE;
			}
		  status = NameValueHexStringToBuf (*Buffer, &bufLength, HexStringBuffer, &convertedStrLen);
		}
	}
DONE:
	return status;
}

/**

    @param 

    @retval EFI_STATUS
        EFI_SUCCESS
**/
EFI_STATUS HiiExtendedInitializeProtocol()
{
	EFI_STATUS status = EFI_SUCCESS;

	if (gHiiInitialized)
	{
		goto DONE;
	}

	status = gBS->LocateProtocol(&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID**)&gHiiConfigRouting);
	if(EFI_ERROR(status))
	{
		goto DONE;
	}

	gHiiInitialized = TRUE;

DONE:
	return status;
}


/**

    @param ControlInfo 

    @retval UINT32
**/
UINT32 GetControlConditionVariable(CONTROL_INFO *ControlInfo)
{
	EFI_IFR_OP_HEADER *ifrPtr;
	PAGE_INFO *pageInfo = NULL;
	UINTN control = 0;
	CONTROL_INFO *CtrlInfo;
	BOOLEAN found = FALSE;
	EFI_QUESTION_ID  QuestionId;
	UINT32 varId = 0;

	if ( ControlInfo == NULL )
		return 0;

	pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[ControlInfo->ControlPageID]);
	ifrPtr = (EFI_IFR_OP_HEADER *)ControlInfo->ControlConditionalPtr;

	if ( ifrPtr == NULL )
		return 0;
																			
	if ( ( ifrPtr->OpCode != EFI_IFR_SUPPRESS_IF_OP ) && ( ifrPtr->OpCode != EFI_IFR_GRAY_OUT_IF_OP ) )
		return 0;

	ifrPtr = (EFI_IFR_OP_HEADER *)((UINTN)ifrPtr + ifrPtr->Length);
	// skip NOT if it is present
	if ( ifrPtr->OpCode == EFI_IFR_NOT_OP )
		ifrPtr = (EFI_IFR_OP_HEADER *)((UINTN)ifrPtr + ifrPtr->Length);

	if ( ifrPtr->OpCode == EFI_IFR_EQ_ID_LIST_OP )
		QuestionId = ((EFI_IFR_EQ_ID_VAL_LIST *)ifrPtr)->QuestionId;
	else if ( ifrPtr->OpCode == EFI_IFR_EQ_ID_VAL_OP )
		QuestionId = ((EFI_IFR_EQ_ID_VAL *)ifrPtr)->QuestionId;
	else
		return 0;

	if(pageInfo->PageHandle ==0)
		return 0;

	for ( control= 0; (control < pageInfo->PageControls.ControlCount) && (found == FALSE); control++ )
	{
		CtrlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

		if(QuestionId == CtrlInfo->ControlKey)
		{
			varId = CtrlInfo->ControlVariable;
			found = TRUE;
		}
	}
	if(!found)
		return 0;

	return varId;
}

/**

    @param Handle UINT16 Token

    @retval VOID
**/
VOID HiiRemoveString(EFI_HII_HANDLE Handle, UINT16 Token)
{
  UINT32 i = 0;
  CHAR16 *nullString = NULL;

	for ( i = 0; i < gLangCount; i++ )
  {
  	HiiChangeStringLanguage( Handle, Token, gLanguages[i].Unicode, nullString );
  }
}


/**

    @param variable VARIABLE_INFO *varInfo

    @retval BOOLEAN
**/
BOOLEAN UefiIsEfiVariable(UINT32 variable, VARIABLE_INFO *varInfo)
{
	if (varInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_EFI_VARSTORE ||
		variable == VARIABLE_ID_LANGUAGE ||
		variable == VARIABLE_ID_DEL_BOOT_OPTION ||
		variable == VARIABLE_ID_ADD_BOOT_OPTION ||
		variable == VARIABLE_ID_BOOT_MANAGER ||
		variable == VARIABLE_ID_BOOT_NOW ||
		variable == VARIABLE_ID_LEGACY_DEV_INFO ||
		variable == VARIABLE_ID_LEGACY_GROUP_INFO ||
    	variable == VARIABLE_ID_BOOT_TIMEOUT ||
		variable == VARIABLE_ID_BOOT_ORDER ||
		variable == VARIABLE_ID_AMI_CALLBACK ||
		variable == VARIABLE_ID_AMITSESETUP ||
		variable == VARIABLE_ID_ERROR_MANAGER ||
		variable == VARIABLE_ID_USER_DEFAULTS ||
		variable == VARIABLE_ID_BBS_ORDER ||
		variable == VARIABLE_ID_DRIVER_MANAGER ||
		variable == VARIABLE_ID_DRIVER_ORDER || 
		variable == VARIABLE_ID_ADD_DRIVER_OPTION ||
		variable == VARIABLE_ID_DEL_DRIVER_OPTION ||
		variable == VARIABLE_ID_IDE_SECURITY ||
		variable == VARIABLE_ID_DYNAMIC_PAGE_GROUP ||
		variable == VARIABLE_ID_DYNAMIC_PAGE_DEVICE ||
		variable == VARIABLE_ID_DYNAMIC_PAGE_GROUP_CLASS ||
		variable == VARIABLE_ID_DRV_HLTH_ENB ||
		variable == VARIABLE_ID_DRV_HLTH_COUNT ||
		variable == VARIABLE_ID_DRV_HLTH_CTRL_COUNT)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

EFI_HII_PACKAGE_LIST_HEADER *
PreparePackageList (
  IN UINTN                    NumberOfPackages,
  IN EFI_GUID                 *GuidId,
  ...
  );

#if APTIO_4_00 != 1 && SETUP_USE_GUIDED_SECTION !=1
/**

    @param VOID

    @retval EFI_STATUS
**/
EFI_STATUS InitMiniSetupStrings( VOID )
{
	EFI_STATUS          Status=EFI_SUCCESS;
	UINT16              Index = 0;
  	EFI_HII_PACKAGE_LIST_HEADER     *PackageList=NULL;
#ifdef STANDALONE_APPLICATION
	EFI_GUID            MiniSetupGuid = ENVIRONMENT_VARIABLE_ID ;
#else
    EFI_GUID            MiniSetupGuid = MINI_SETUP_GUID ;
#endif
	Status = HiiInitializeProtocol();
	if ( EFI_ERROR ( Status ) )
		return Status;

	gHiiHandle = HiiFindHandle( &MiniSetupGuid, &Index );

	if(gHiiHandle != INVALID_HANDLE){ //Remove the previous pack installed with this GUID
		Status = gHiiDatabase->RemovePackageList(gHiiDatabase, gHiiHandle);
		gHiiHandle = INVALID_HANDLE;
	}

	if ( gHiiHandle == INVALID_HANDLE )
	{
	  PackageList = PreparePackageList (
	                  1,
	                  &MiniSetupGuid,
	                  STRING_ARRAY_NAME
	                  );
	  if (PackageList == NULL) {
	    return EFI_OUT_OF_RESOURCES;
	  }
	
	  Status = gHiiDatabase->NewPackageList (
	                          gHiiDatabase,
	                          PackageList,
	                          gImageHandle,
	                          &gHiiHandle
	                          );
	} 

	return Status;
}
#endif


/**

    @param VOID

    @retval UINTN
**/
UINTN HiiGetManufactuingMask(VOID)
{
	return AMI_FLAG_MANUFACTURING;
}

/**

    @param VOID

    @retval UINTN
**/
UINTN HiiGetDefaultMask(VOID)
{
	return AMI_FLAG_DEFAULT;
}

/**

    @param controlInfo 

    @retval UINT32
**/
UINT32 GetControlConditionVarId(CONTROL_INFO *controlInfo)
{
	EFI_IFR_OP_HEADER *ifrPtr;
	PAGE_INFO *pageInfo = NULL;
	UINTN control = 0;
	CONTROL_INFO *CtrlInfo;
	BOOLEAN found = FALSE;
	EFI_QUESTION_ID  QuestionId;
	UINT32 varId = 0;

	if ( controlInfo == NULL )
		return 0;
	
	pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[controlInfo->ControlPageID]);
	ifrPtr = (EFI_IFR_OP_HEADER *)controlInfo->ControlConditionalPtr;

	if ( ifrPtr == NULL )
		return 0;
																			
	if ( ( ifrPtr->OpCode != EFI_IFR_SUPPRESS_IF_OP ) && ( ifrPtr->OpCode != EFI_IFR_GRAY_OUT_IF_OP ) )
		return 0;

	ifrPtr = (EFI_IFR_OP_HEADER *)((UINTN)ifrPtr + ifrPtr->Length);
	// skip NOT if it is present
	if ( ifrPtr->OpCode == EFI_IFR_NOT_OP )
		ifrPtr = (EFI_IFR_OP_HEADER *)((UINTN)ifrPtr + ifrPtr->Length);

	if ( ifrPtr->OpCode == EFI_IFR_EQ_ID_LIST_OP )
		QuestionId = ((EFI_IFR_EQ_ID_VAL_LIST *)ifrPtr)->QuestionId;
	else if ( ifrPtr->OpCode == EFI_IFR_EQ_ID_VAL_OP )
		QuestionId = ((EFI_IFR_EQ_ID_VAL *)ifrPtr)->QuestionId;
	else
		return 0;

	if(pageInfo->PageHandle ==0)
		return 0;

	for ( control= 0; (control < pageInfo->PageControls.ControlCount) && (found == FALSE); control++ )
	{
		CtrlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

		if(QuestionId == CtrlInfo->ControlKey)
		{
			varId = CtrlInfo->ControlVariable;
			found = TRUE;
		}
	}
  if(!found)
  {
    // Control Definition not in this Page. Look within the formset
    EFI_HII_HANDLE pgHandle = controlInfo->ControlHandle;
    UINT32 page = 0;

    for(;page < gPages->PageCount && (found == FALSE); page++)
    {
      if(page == controlInfo->ControlPageID)
      {
        continue;
      }
      pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[page]);
      if(pageInfo->PageHandle != pgHandle)
      {
        continue;
      }

      for(control = 0; control < pageInfo->PageControls.ControlCount && (found == FALSE); control++)
      {
        controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

        if(QuestionId == controlInfo->ControlKey)
        {
          varId = controlInfo->ControlVariable;
          found = TRUE;
        }
      }
    }
  }

	if(!found)
		return 0;

	return varId;
}

/**
        Procedure	:	ProcessActionQuestionConfiguration

        Description	:	Function to process Action Control Configuration String.

        Input		:	CONTROL_INFO *control

        Output		:	EFI_STATUS

**/
EFI_STATUS ProcessActionQuestionConfiguration(CONTROL_INFO *control)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_STRING configuration = NULL;
  EFI_STRING progress = NULL;
  EFI_IFR_ACTION *UefiAction = NULL;
  BOOLEAN PreservegEnableDrvNotification = FALSE;

    if((control == NULL) || (control->ControlPtr == NULL)){ 
	  Status = EFI_UNSUPPORTED;
	  goto DONE;
  	}
  UefiAction = (EFI_IFR_ACTION*)control->ControlPtr;

  if(UefiAction->Header.Length == sizeof(EFI_IFR_ACTION))
  {
    if(UefiAction->QuestionConfig)
    {
      //Process UefiAction QuestionConfig
      configuration = HiiGetString(control->ControlHandle, UefiAction->QuestionConfig);
	  if((configuration != NULL) && EfiStrLen(configuration))
	  {
		  PreservegEnableDrvNotification = gEnableDrvNotification;
        gEnableDrvNotification = TRUE; //Set to enable notification processing
		  Status = gHiiConfigRouting->RouteConfig(gHiiConfigRouting, configuration, &progress);
		  if (!PreservegEnableDrvNotification) // If gEnableDrvNotification is already True Don't touch it
        	gEnableDrvNotification = FALSE; //Reset to disable notification processing
	  }
	  if(configuration)
		  MemFreePointer((VOID**)&configuration);
    }
  }
DONE:
  return Status;
}

/**

    @param actionReq 

    @retval EFI_STATUS
**/
EFI_STATUS UefiVarsetnvramforCurrentform(UINT32 FormID);
EFI_STATUS UefiLoadPreviousValuesforCurrentform(UINT32 FormID);
VOID HotclickESCaction(VOID);
EFI_STATUS DoDisconnectAndConnectController (void);
extern BOOLEAN BrowserActionDiscard;
EFI_STATUS ProcessBrowserActionRequest(EFI_BROWSER_ACTION_REQUEST ActionReq)
{

  switch(ActionReq)
  {
  case EFI_BROWSER_ACTION_REQUEST_RESET:
    // Do nothing. Reset the system.
    gApp->Quit = ResetSys();
    break;
  case EFI_BROWSER_ACTION_REQUEST_SUBMIT:
    //Save values here before exiting.
    gApp->Quit = SaveAndExit();
    break;
  case EFI_BROWSER_ACTION_REQUEST_EXIT:
    //Donot save values. Exit as is.
    gApp->Quit = ExitApplication();
    break;
	//Changes to support new action request form browser
  case EFI_BROWSER_ACTION_REQUEST_FORM_SUBMIT_EXIT:
	UefiVarsetnvramforCurrentform (gApp->CurrentPage) ;
	HotclickESCaction();
	break;
  case EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD_EXIT:
	BrowserActionDiscard = TRUE;
	UefiLoadPreviousValuesforCurrentform (gApp->CurrentPage);
	HotclickESCaction();
	break;
  case EFI_BROWSER_ACTION_REQUEST_FORM_APPLY:
	UefiVarsetnvramforCurrentform (gApp->CurrentPage) ;
	break;
  case EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD:
	BrowserActionDiscard = TRUE;
	UefiLoadPreviousValuesforCurrentform (gApp->CurrentPage);
	break;
  case EFI_BROWSER_ACTION_REQUEST_RECONNECT:
	DoDisconnectAndConnectController ();
	break;
  default:
    break;
  }
//  gProFormOffset = 0;				
  CleargProcessedForms ();
//  MemSet (gProcessedForms, MAX_FORMS_TO_PROCESS, 0);
		
  return EFI_SUCCESS;
}

VOID* UefiGetSpecialEqIDIfrPtr(CONTROL_INFO *ControlInfo, UINT32 *Variable, GUID_INFO **GuidInfo)
{
    VOID *Conditional = ControlInfo->ControlConditionalPtr;
    VOID *ControlPtr = ControlInfo->ControlPtr;
    EFI_IFR_OP_HEADER * OpHeader = (EFI_IFR_OP_HEADER *)Conditional;
    GUID_INFO *specialGuid = NULL;
    VARIABLE_INFO *varInfo = NULL;
    UINT32 i = 0;
    UINT8 InScope = 0;

    if((OpHeader==NULL) || (ControlPtr <= Conditional))
        return NULL;

    if ((OpHeader != NULL) &&
    ((OpHeader->OpCode == EFI_IFR_SUPPRESS_IF_OP) ||
     (OpHeader->OpCode == EFI_IFR_GRAY_OUT_IF_OP)))
    {
        do
        {
            if(OpHeader->Scope)
            {
                InScope++;
            }
            if(OpHeader->OpCode == EFI_IFR_END_OP)
            {
                InScope--;
                if(InScope == 0)
                {
                    // EFI_IFR_EQ_ID_VAL_OP opcode not found
                    OpHeader = NULL;
                    goto DONE;
                }
            }
            OpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)OpHeader + OpHeader->Length);
        }while((OpHeader->OpCode != EFI_IFR_EQ_ID_VAL_OP) &&
            (OpHeader->OpCode != EFI_IFR_EQ_ID_LIST_OP));
    }

	if(NoVarStoreSupport())
	{
	    if(*Variable == VARIABLE_ID_SETUP)
	    {
			*Variable = FindVarFromITKQuestionId(UefiGetEqIDQuestionID(OpHeader));
	    }
	}

  // Retrieve Special Control Information??? Update with proper comment
  varInfo = VarGetVariableInfoIndex( *Variable );
	if( varInfo == NULL )
  {
    OpHeader = NULL;
		goto DONE;
  }

	for ( i = 0; i < gGuidList->GuidCount; i++ )
	{
		GUID_INFO *guidInfo = (GUID_INFO *)((UINTN)gGuidList + gGuidList->GuidList[i]);
		if ( EfiCompareGuid( &varInfo->VariableGuid, &guidInfo->GuidValue ) )
		{
			specialGuid = guidInfo;
			break;
		}
	}

	if ( specialGuid != NULL )
	{
		if ( GuidInfo != NULL )
			*GuidInfo = specialGuid;
	}
	else
	{
		OpHeader = NULL;
	}

    // See if Other condition is Special condition
    if(OpHeader == NULL)
    {
        if(ControlPtr > Conditional)
        {
            UINTN Offset = _SkipExpression(Conditional);
            if(Offset)
            {
                Conditional = (VOID*)((UINTN)Conditional + Offset);
                if(ControlPtr > Conditional)
                {
                     CONTROL_INFO TempCtlInfo;
                     MemCopy(&TempCtlInfo, ControlInfo, sizeof (CONTROL_INFO));
                     TempCtlInfo.ControlConditionalPtr = Conditional;
                     *Variable = (UINT16)GetControlConditionVarId(&TempCtlInfo);
                     OpHeader = UefiGetSpecialEqIDIfrPtr(&TempCtlInfo, Variable, GuidInfo)  ;
                }
            }
        }
    }

DONE:
  return OpHeader;
}

/**

    @param ControlInfo GUID_INFO **GuidInfo

    @retval UINT16
**/
UINT16 UefiGetSpecialEqIDValue(CONTROL_INFO *ControlInfo, GUID_INFO **GuidInfo)
{
  UINT16 value = (UINT16)-2;
  UINT32 condVarID = 0;
  EFI_IFR_OP_HEADER *ifrPtr = NULL;

  condVarID = (UINT32)GetControlConditionVarId(ControlInfo);
  ifrPtr = (EFI_IFR_OP_HEADER *)UefiGetSpecialEqIDIfrPtr(ControlInfo, &condVarID, GuidInfo);

  if(ifrPtr != NULL)
  {
    switch(ifrPtr->OpCode)
    {
    case EFI_IFR_EQ_ID_LIST_OP:
      value = ((EFI_IFR_EQ_ID_VAL_LIST *)ifrPtr)->ValueList[0];
      break;
    case EFI_IFR_EQ_ID_VAL_OP:
      value = ((EFI_IFR_EQ_ID_VAL *)ifrPtr)->Value;
      break;
    default:
      break;
    }
  }

  return value;
}

/**

    @param IfrPtr UINT16 Value

    @retval VOID
**/
VOID UefiSetEqIDValue(VOID *IfrPtr, UINT16 Value)
{
  EFI_IFR_OP_HEADER *opHeader = (EFI_IFR_OP_HEADER *)IfrPtr;

  switch(opHeader->OpCode)
  {
  case EFI_IFR_EQ_ID_LIST_OP:
    ((EFI_IFR_EQ_ID_VAL_LIST *)IfrPtr)->ValueList[0] = Value;
    break;
  case EFI_IFR_EQ_ID_VAL_OP:
    ((EFI_IFR_EQ_ID_VAL *)IfrPtr)->Value = Value;
    break;
  default:
    break;
  }
}

/**

    @param IfrPtr 

    @retval BOOLEAN
**/
BOOLEAN UefiIsOneOfControl(VOID *IfrPtr)
{
  return (((EFI_IFR_OP_HEADER *)IfrPtr)->OpCode == EFI_IFR_ONE_OF_OP)? TRUE : FALSE;
}

/**

    @param IfrPtr UINT16 Value

    @retval VOID
**/
VOID UefiUpdateControlVarOffset(VOID *IfrPtr, UINT16 Value)
{
  EFI_IFR_OP_HEADER *opHeader = (EFI_IFR_OP_HEADER *)IfrPtr;

  switch(opHeader->OpCode)
  {
  case EFI_IFR_ONE_OF_OP:
    ((EFI_IFR_ONE_OF *)opHeader)->Question.VarStoreInfo.VarOffset = Value;
    break;
  default:
    break;
  }
}


/**

    @param OptionCount CONTROL_INFO *CtrlInfo

    @retval VOID*
**/
VOID* UefiCreateOneOfWithOptionsTemplate(UINTN OptionCount, CONTROL_INFO *CtrlInfo)
{
  VOID *buffer = NULL;
  UINTN size = 0;
  UINTN length = 0;
  UINTN offset;

  offset = (UINTN)CtrlInfo->ControlPtr - (UINTN)CtrlInfo->ControlConditionalPtr;

  size = offset + sizeof(EFI_IFR_ONE_OF) + sizeof(EFI_IFR_OP_HEADER) +
	 OptionCount * sizeof(EFI_IFR_ONE_OF_OPTION);
  buffer = EfiLibAllocateZeroPool(size);
  if(buffer == NULL)
  {
    goto DONE;
  }
  MemCopy(buffer, CtrlInfo->ControlConditionalPtr, offset);

  length = UefiGetIfrLength(CtrlInfo->ControlPtr);
  CtrlInfo->ControlConditionalPtr = buffer;
  MemCopy( (UINT8 *)buffer + offset, CtrlInfo->ControlPtr, length );
  CtrlInfo->ControlPtr = (UINT8 *)buffer + offset;

DONE:
  return (UINT8 *)buffer + offset;
}

/**

    @param IfrPtr UINT64 Value, UINT32 Size, UINT8 Flag, UINT16 Option

    @retval VOID
**/
VOID UefiSetOneOfOption(VOID *IfrPtr, UINT64 Value, UINT32 Size, UINT8 Flag, UINT16 Option)
{
  EFI_IFR_ONE_OF_OPTION *option = (EFI_IFR_ONE_OF_OPTION *)IfrPtr;
  UINT8 flag = 0;

  flag |= ((Flag & AMI_FLAG_DEFAULT) == AMI_FLAG_DEFAULT)? EFI_IFR_OPTION_DEFAULT : 0;
  flag |= ((Flag & AMI_FLAG_MANUFACTURING) == AMI_FLAG_MANUFACTURING)? EFI_IFR_OPTION_DEFAULT_MFG : 0;

  switch(Size)
  {
  case sizeof(UINT8):
    flag |= EFI_IFR_TYPE_NUM_SIZE_8;
    option->Value.u8 = (UINT8)Value;
    break;
  case sizeof(UINT16):
    flag |= EFI_IFR_TYPE_NUM_SIZE_16;
    option->Value.u16 = (UINT16)Value;
    break;
  case sizeof(UINT32):
    flag |= EFI_IFR_TYPE_NUM_SIZE_32;
    option->Value.u32 = (UINT32)Value;
    break;
  case sizeof(UINT64):
    flag |= EFI_IFR_TYPE_NUM_SIZE_64;
    option->Value.u64 = (UINT64)Value;
    break;
  default:
    break;
  }

  option->Flags = flag;
  option->Option = Option;
}

/**

    @param IfrPtr 

    @retval VOID
**/
VOID UefiAddEndOp(VOID *IfrPtr)
{
  EFI_IFR_OP_HEADER *opHeader =  (EFI_IFR_OP_HEADER *)IfrPtr;

  opHeader->OpCode = EFI_IFR_END_OP;
  opHeader->Length = sizeof(EFI_IFR_OP_HEADER);
}

/**

    @param Control 

    @retval UINT16
**/
UINT16 UefiGetQuestionID(CONTROL_INFO *Control)
{
	return 0; //(UINT16)GetControlConditionVarId(Control);
}

/**

    @param control UINT16 VarId, UINT16 PrevControlQuestionID,UINT16 Value

    @retval VOID
**/
VOID UefiCreateDynamicControlCondition(CONTROL_INFO *control,UINT16 VarId, UINT16 PrevControlQuestionID,UINT16 Value)
{
	//Add condition here for boot and bbs special controls
}

/**

    @param IfrPtr 

    @retval UINT16
**/
UINT16 UefiGetEqIDQuestionID(VOID *IfrPtr)
{
  UINT16 questionID = 0;

  return questionID;
}

/**

    @param IfrPtr UINT16 Value

    @retval VOID
**/
VOID UefiSetEqIDQuestionID(VOID *IfrPtr, UINT16 Value)
{
}

/**

    @param IfrPtr UINT16 Value

    @retval VOID
**/
VOID UefiUpdateControlQuestionID(VOID *IfrPtr, UINT16 Value)
{
  EFI_IFR_OP_HEADER *opHeader = (EFI_IFR_OP_HEADER *)IfrPtr;

  switch(opHeader->OpCode)
  {
  case EFI_IFR_ONE_OF_OP:
    ((EFI_IFR_ONE_OF *)opHeader)->Question.QuestionId = Value;
    break;
  default:
    break;
  }
}

/**

    @param HiiHandle UINT16 Key, EFI_GUID *pGUID

    @retval VOID
**/
VOID UefiSpecialGuidCallback(VOID * HiiHandle, UINT16 Key, EFI_GUID *pGUID)
{
	EFI_HANDLE DriverHandle;
	EFI_GUID EfiHiiConfigAccessProtocolGuid = EFI_HII_CONFIG_ACCESS_PROTOCOL_GUID;
	EFI_HII_CONFIG_ACCESS_PROTOCOL	*ConfigAccess;
	EFI_IFR_TYPE_VALUE	Value;
	EFI_BROWSER_ACTION_REQUEST ActionReq;
	EFI_STATUS	Status;
   BOOLEAN PreservegEnableDrvNotification = FALSE;

	Status = gHiiDatabase->GetPackageListHandle(gHiiDatabase,HiiHandle,&DriverHandle);
	if(!EFI_ERROR(Status))
	{
		Status = gBS->HandleProtocol( DriverHandle, &EfiHiiConfigAccessProtocolGuid , (VOID**)&ConfigAccess );
		if(!EFI_ERROR(Status))
		{
			EFI_BROWSER_ACTION action = EFI_BROWSER_ACTION_CHANGED;

			MemSet( &Value, sizeof(Value), 0 );
			Value.u64 = (UINTN)pGUID;
		   PreservegEnableDrvNotification = gEnableDrvNotification;
			gEnableDrvNotification = TRUE; //Set to enable notification processing
			Status = ConfigAccess->Callback(ConfigAccess, action,Key,((sizeof(VOID*)==sizeof(UINT32))?EFI_IFR_TYPE_NUM_SIZE_32:EFI_IFR_TYPE_NUM_SIZE_64),&Value,&ActionReq);
		   if (!PreservegEnableDrvNotification) // If gEnableDrvNotification is already True Don't touch it
			  gEnableDrvNotification = FALSE; //Reset to disable notification processing
            // Clean up PAGE_INFO struct with Handle set to 0xFFFF  
            Status = FixSetupData();
            if(EFI_ERROR(Status))
            {
            }
		}
	}

}

/**

    @param IfrPtr UINT8 Width

    @retval VOID
**/
VOID UefiSetWidth(VOID *IfrPtr,UINT8 Width)
{
  EFI_IFR_OP_HEADER *OpHeader = (EFI_IFR_OP_HEADER*)IfrPtr;

  switch(OpHeader->OpCode)
  {
	case EFI_IFR_NUMERIC_OP:
		break;
	case EFI_IFR_PASSWORD_OP:
		break;
	case EFI_IFR_STRING_OP:
		break;
	case EFI_IFR_ONE_OF_OP:
		break;
	case EFI_IFR_CHECKBOX_OP:
		break;
  default:
    break;
  }
}

/**

    @param control 

    @retval VOID
**/
VOID   BBSUpdateControlOffset(CONTROL_INFO *control)
{
    UINT16 offset=  ((EFI_IFR_ONE_OF*)control->ControlPtr)->Question.VarStoreInfo.VarOffset;

    offset = offset +sizeof(UINT32)+sizeof(UINT16);
    offset = (UINT16)(offset + gCurrLegacyBootData->LegacyEntryOffset);

	UefiUpdateControlVarOffset(control->ControlPtr, offset);
}

/**
    This function will return width of questions

    @param CONTROL_INFO *

    @retval UINTN
**/
UINTN GetControlDataLength(CONTROL_INFO *controlInfo)
{
  UINTN width = 0;

  switch(controlInfo->ControlType)
  {
  case CONTROL_TYPE_CHECKBOX:
  case CONTROL_TYPE_POPUPSEL:
  case CONTROL_TYPE_NUMERIC:
  case CONTROL_TYPE_PASSWORD:
  case CONTROL_TYPE_POPUP_STRING:
    width = UefiGetWidth(controlInfo->ControlPtr);
    break;
  case CONTROL_TYPE_ORDERED_LIST:
    width = (((EFI_IFR_ORDERED_LIST*)(controlInfo->ControlPtr))->MaxContainers);
    break;
  case CONTROL_TYPE_TEXT:
    width = sizeof(UINT16);
    break;
  case CONTROL_TYPE_DATE:
    width = sizeof(EFI_HII_DATE);
    break;
  case CONTROL_TYPE_TIME:
    width = sizeof(EFI_HII_TIME);
    break;
  default:
    break;
  }
  return width;
}


/**
    Function to return Password Encoding Status.

    @param CONTROL_INFO

    @retval TRUE/FALSE

**/
BOOLEAN IsPasswordEncodeEnabled (CONTROL_INFO *controlInfo)
{
	extern PASSWORD_ENOCDE_LIST_TEMPLATE gPasswordEncodeList [];
	extern UINT32 gEncodeListCount;
	UINT32 iIndex = 0;
	EFI_GUID tFormsetGuid;

	//Get page info
//	PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[controlInfo->ControlPageID]);

	//Get FormsetGuid
	GetPageGuidFromPgIndex (controlInfo->ControlPageID, &tFormsetGuid);
		
	for (iIndex = 0; iIndex < gEncodeListCount; iIndex ++)
	{
		//If given QuestionId and FormsetGuid is same then return Encoding Flag
		if ( (gPasswordEncodeList [iIndex].QuestionID == ((EFI_IFR_PASSWORD *)controlInfo->ControlPtr)->Question.QuestionId) &&
				(EfiCompareGuid(&gPasswordEncodeList [iIndex].FormsetGuid, &tFormsetGuid) )
			)
		{
			return gPasswordEncodeList [iIndex].EncodingFlag;
		}
	}
	
	// IDE Password Encode the password
	if (
      (VARIABLE_ID_IDE_SECURITY == controlInfo->ControlVariable) ||
	   (VARIABLE_ID_AMITSESETUP == controlInfo->ControlVariable)
      )
   {
		return TRUE;
   }
	return FALSE;
}

/**
    Function to get time

    @param control EFI_TIME *Tm

    @retval STATUS
**/
EFI_STATUS UefiGetTime(CONTROL_INFO *control, EFI_TIME *Tm)
{
  VOID *IfrPtr = NULL ;
  UINT8 Flags ;
  EFI_STATUS Status = EFI_UNSUPPORTED;	


  if((control != NULL) && (control->ControlPtr != NULL))
  {
	IfrPtr = control->ControlPtr; 
  	Flags = UefiGetFlagsField(IfrPtr) ;


  	if( ((Flags & QF_TIME_STORAGE) == QF_TIME_STORAGE_TIME)||
	    ((Flags & EFI_QF_DATE_STORAGE) == QF_DATE_STORAGE_TIME)  )//  Incorrect logic used to check Time and Date control types
  	{
		Status = gRT->GetTime( Tm, NULL );
 	}
  	else if( ((Flags & QF_TIME_STORAGE) == QF_TIME_STORAGE_NORMAL) ||
		 ((Flags & EFI_QF_DATE_STORAGE) == QF_DATE_STORAGE_NORMAL)   )// Incorrect logic used to check Time and Date control types
  	{
		
		if(control->ControlType == CONTROL_TYPE_DATE)
		{	
			EFI_HII_DATE *date = NULL ;	

			date = EfiLibAllocateZeroPool( sizeof(EFI_HII_DATE) );
			
			if ( date != NULL )
			{	
				Status = VarGetValue( control->ControlVariable, UefiGetQuestionOffset(IfrPtr), UefiGetWidth(IfrPtr), date );
				if ( ! EFI_ERROR( Status ) )
				{
					Tm->Month = date->Month ;
					Tm->Day = date->Day ;
					Tm->Year = date->Year ;
				}

				MemFreePointer( (VOID **)&date );
			}
		}else{
			EFI_HII_TIME *time = NULL ;
			
			time = EfiLibAllocateZeroPool( sizeof(EFI_HII_TIME) );

			if ( time != NULL )
			{	
				Status = VarGetValue( control->ControlVariable, UefiGetQuestionOffset(IfrPtr), UefiGetWidth(IfrPtr), time );
				Tm->Hour = time->Hour ;
				Tm->Minute = time->Minute ;
				Tm->Second = time->Second ;
				
				MemFreePointer( (VOID **)&time );
			}
		}
	}
	else if( ((Flags & QF_TIME_STORAGE) == QF_TIME_STORAGE_WAKEUP) ||
		 ((Flags & EFI_QF_DATE_STORAGE) == QF_DATE_STORAGE_WAKEUP) )// Incorrect logic used to check Time and Date control types
  	{
		
		BOOLEAN Enabled;
		BOOLEAN Pending;
		Status = gRT->GetWakeupTime( &Enabled, &Pending, Tm );
	}
  }
  else	
	Status = gRT->GetTime( Tm, NULL );

  return Status ;

}
/**
    Function to set time

    @param control EFI_TIME *Tm

    @retval STATUS
**/
EFI_STATUS UefiSetTime(CONTROL_INFO *control, EFI_TIME *Tm)
{
  VOID *IfrPtr = NULL ;
  UINT8 Flags = 0;
  EFI_STATUS Status = EFI_UNSUPPORTED;	  
  	

  if((control != NULL) && (control->ControlPtr != NULL))
  {
	IfrPtr = control->ControlPtr; 
  	Flags = UefiGetFlagsField(IfrPtr) ;

  	if( ((Flags & QF_TIME_STORAGE) == QF_TIME_STORAGE_TIME)||
	    ((Flags & EFI_QF_DATE_STORAGE) == QF_DATE_STORAGE_TIME)  )// Incorrect logic used to check Time and Date control types
  	{
		Status = gRT->SetTime( Tm );
 	}
  	else if( ((Flags & QF_TIME_STORAGE) == QF_TIME_STORAGE_NORMAL) ||
		 ((Flags & EFI_QF_DATE_STORAGE) == QF_DATE_STORAGE_NORMAL)   )//  Incorrect logic used to check Time and Date control types
  	{
		if(control->ControlType == CONTROL_TYPE_DATE)
		{	
			EFI_HII_DATE *date = NULL ;	
			EFI_TIME Tc ;

			date = EfiLibAllocateZeroPool( sizeof(EFI_HII_DATE) );

			if ( date != NULL )
			{
				UefiGetTime(control, &Tc);				
				date->Month = (Tm->Month <= 12)?Tm->Month : ((Tc.Month>12)? 1:Tc.Month) ;
				date->Day = (Tm->Day <= 31)?Tm->Day : ((Tc.Day>31)? 1:Tc.Day) ;
				date->Year = Tm->Year ;	
				Status = VarSetValue( control->ControlVariable, UefiGetQuestionOffset(IfrPtr), UefiGetWidth(IfrPtr), date );

				MemFreePointer( (VOID **)&date );
			}
			
		}else{
			EFI_HII_TIME *time = NULL ;	
			EFI_TIME Tc ;

			time = EfiLibAllocateZeroPool( sizeof(EFI_HII_TIME) );

			if ( time != NULL )
			{
				UefiGetTime(control, &Tc);
				time->Hour = (Tm->Hour <= 23)?Tm->Hour : ((Tc.Hour>23)? 0:Tc.Hour) ;
				time->Minute = (Tm->Minute <= 59)? Tm->Minute : ((Tc.Minute>59)? 0:Tc.Minute) ;
				time->Second = (Tm->Second <= 59)?Tm->Second : ((Tc.Second>59)? 0:Tc.Second);
				Status = VarSetValue( control->ControlVariable, UefiGetQuestionOffset(IfrPtr), UefiGetWidth(IfrPtr), time );
				MemFreePointer( (VOID **)&time );
			}
		}
	}
	else if( ((Flags & QF_TIME_STORAGE) == QF_TIME_STORAGE_WAKEUP) ||
		 ((Flags & EFI_QF_DATE_STORAGE) == QF_DATE_STORAGE_WAKEUP) )// Incorrect logic used to check Time and Date control types
  	{
		BOOLEAN Enabled = TRUE ;
		Status = gRT->SetWakeupTime( Enabled, Tm );
	}
  }
  else	
	Status = gRT->SetTime( Tm );


  return Status ;	

}
/**
    Finds the destination question ID for the current destination

    @param CurrPageID Current page ID
    @param DestQuestionId Destination question ID to set focus
    @param FocusControlIndex 

    @retval EFI_STATUS status - EFI_SUCCESS is successful, else EFI_ERROR
**/
EFI_STATUS UpdateDestiantionQuestion(UINT32 CurrPageID, UINT32 DestQuestionId, UINT32 *FocusControlIndex)
{
	EFI_STATUS status = EFI_NOT_FOUND;
	UINT32 j = 0;
	UINT8	MiscControls = 0;

	PAGE_INFO *PageInfo = (PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[CurrPageID]);

	for ( j = 0; j < PageInfo->PageControls.ControlCount; j++ )
	{
		CONTROL_INFO *ctrlInfo = (CONTROL_INFO*)(((UINTN)gControlInfo) + PageInfo->PageControls.ControlList [j /*PageInfo->PageIdIndex */]);
		if(!ctrlInfo){
			continue;
		}
		if(ctrlInfo->ControlType == CONTROL_TYPE_MSGBOX ||  ctrlInfo->ControlType == NO_SUBMIT_IF)
		{
			MiscControls ++;
			continue;
		}		
		
		if(UefiGetKeyField(ctrlInfo->ControlPtr) == DestQuestionId) 
		{
			*FocusControlIndex = j - MiscControls;
			status = EFI_SUCCESS;
			goto DONE;
		}
	}

DONE:
	return status;
}

/**
    Function to Refresh the questions

    @param ControlData 

    @retval EFI_STATUS
**/
EFI_STATUS UefiRefershQuestionValueNvRAM(CONTROL_INFO *ControlData)
{

	
	UINT8 * CurBuf=NULL;
	UINT8 * NvBuf = NULL;
	UINTN VarSize=0;
	EFI_STATUS Status=EFI_UNSUPPORTED;
	UINT8 Flags = 0;
	UINTN offset = UefiGetQuestionOffset(ControlData->ControlPtr);
	UINT32 Size = 0;// = UefiGetWidth(ControlData->ControlPtr);
	//If the control is Ordered list then we are calculating the actual size
	if( CONTROL_TYPE_ORDERED_LIST != ControlData->ControlType)
	{
		Size = UefiGetWidth(ControlData->ControlPtr);
		VarSize = GetControlDataLength(ControlData);
	}
	else
	{
		UINTN Type=0;
		UINT16 OptionCount = 0;
		UINT16 *OptionList=NULL;
		UINT64 *ValueList=NULL;

		UefiGetOneOfOptions(ControlData,&ControlData->ControlHandle,&OptionList, &ValueList, &OptionCount,NULL,NULL);
		if (OptionList)
			MemFreePointer( (VOID **)&(OptionList));
		if (ValueList)
			MemFreePointer( (VOID **)&(ValueList));

		UefiGetValidOptionType( ControlData, &Type, &Size);
		Size = OptionCount * Size;
		VarSize = Size;
		
	}
	NvBuf = VarGetNvramQuestionValue(ControlData->ControlVariable, offset, VarSize); //Read value for specific question
	if(NvBuf == NULL){ // No Variable Assosiated then Just refrsh the page
		return EFI_SUCCESS;
    }

    if((UINTN)(Size) >  VarSize){
        goto DONE;
    }

	CurBuf = EfiLibAllocateZeroPool(Size);

	switch(ControlData->ControlType)
	{

	case CONTROL_TYPE_NUMERIC:
	case CONTROL_TYPE_ORDERED_LIST:
	case CONTROL_TYPE_POPUPSEL:
	case CONTROL_TYPE_POPUP_STRING:
	case CONTROL_TYPE_CHECKBOX:
	case CONTROL_TYPE_PASSWORD:
		VarGetValue( ControlData->ControlVariable, 
					UefiGetQuestionOffset(ControlData->ControlPtr), Size, CurBuf );
		break;
	case CONTROL_TYPE_DATE:
	case CONTROL_TYPE_TIME:
  		Flags = UefiGetFlagsField(ControlData->ControlPtr) ;
  		if( ((Flags & QF_TIME_STORAGE) == QF_TIME_STORAGE_NORMAL)  ||
		    ((Flags & EFI_QF_DATE_STORAGE) == QF_DATE_STORAGE_NORMAL) )
		{
				VarGetValue( ControlData->ControlVariable, 
						UefiGetQuestionOffset(ControlData->ControlPtr), Size, CurBuf );
		}
		else
		{
			// RTC/Wakeup Date/Timecontrol
			Status = EFI_SUCCESS;
			goto DONE;
		}
		break;
	default:
		goto DONE;
	}
	if(MemCmp((UINT8 *)NvBuf, (UINT8 *)CurBuf, Size))
	{
		_VarGetSetValue (VAR_COMMAND_SET_VALUE, gVariableList, ControlData->ControlVariable, UefiGetQuestionOffset (ControlData->ControlPtr), Size, NvBuf);
	}
	else
	{
		if(UefiIsInteractive(ControlData))
			Status = CallFormCallBack(ControlData,UefiGetControlKey(ControlData),0,EFI_BROWSER_ACTION_RETRIEVE);
	}
	
	Status = EFI_SUCCESS;
DONE:
	
	MemFreePointer((VOID**)&CurBuf);		
	MemFreePointer((VOID**)&NvBuf);
	return Status;
}
/**
    Funtion get the ResetButton Default Id from the Opcode information

    @param IfrPtr 

    @retval UINT16
**/
UINT16 UefiGetResetButtonDefaultid(VOID *IfrPtr)
{
   EFI_IFR_OP_HEADER *OpHeader=(EFI_IFR_OP_HEADER *)IfrPtr;
   UINT16 DefaultId = 0xFFFF;//Initializing the default to handle error case 
	 
	 if(OpHeader->OpCode == EFI_IFR_RESET_BUTTON_OP )
	  {
		    DefaultId = ((EFI_IFR_RESET_BUTTON *)OpHeader)->DefaultId;
			return DefaultId;
	  }
return DefaultId;
}

/**
    Finds the driver name which supports driver health protocol

    @param UINT16 = Entry for which driver name has to be return

    @retval CHAR16 * = Driver Name

**/
CHAR16 *FindDriverHealthDriverName (UINT16 DriverEntry)
{
	EFI_STATUS  Status, StatusforGetHealth = EFI_UNSUPPORTED;
//	EFI_GUID 	DriverHealthProtocolGuid = EFI_DRIVER_HEALTH_PROTOCOL_GUID;
	EFI_HANDLE  *DriverHealthHandles;
	UINTN       NumHandles = 0, iIndex = 0;
	UINTN 		Count = 0;
	CHAR16      *DriverName = NULL, *DrvNameWithStatus  = NULL;
	EFI_GUID 	EfiComponentName2ProtocolGuid = EFI_COMPONENT_NAME2_PROTOCOL_GUID;
	EFI_COMPONENT_NAME2_PROTOCOL	*ComponentName2 = NULL;
	EFI_DRIVER_HEALTH_PROTOCOL		*DrvHealthInstance = NULL;
	EFI_DEVICE_PATH_PROTOCOL		*DevicePath;
	EFI_DRIVER_HEALTH_STATUS		HealthStatus = EfiDriverHealthStatusHealthy;
    EFI_GUID EfiDriverHealthProtocolGuid = EFI_DRIVER_HEALTH_PROTOCOL_GUID;
    
    CHAR8 *LangStrings = (CHAR8 *)NULL;
    CHAR8 *tempLangStrings = (CHAR8 *)NULL;
    CHAR8 Lang[200]; 
    
    SETUP_DEBUG_UEFI ("\n[TSE]FindDriverHealthDriverName: DriverEntry = %d\n", DriverEntry );

	Status = gBS->LocateHandleBuffer (
				ByProtocol,
				&EfiDriverHealthProtocolGuid,
				NULL,
				&NumHandles,
				&DriverHealthHandles
				);
	if ((Status != EFI_SUCCESS) || (NumHandles == 0))
	{
		return NULL;
	}
	SETUP_DEBUG_UEFI ("[TSE]FindDriverHealthDriverName:  Num of driver health handles = %d \n", NumHandles);
	        	
	for (iIndex = 0; iIndex < NumHandles; iIndex ++)
	{
		Status = gBS->HandleProtocol (DriverHealthHandles [iIndex], &EfiDriverHealthProtocolGuid, (VOID**)&DrvHealthInstance);
		if (EFI_ERROR (Status))
		{
			continue;
		}
		
		gEnableDrvNotification = TRUE;      
		gBrowserCallbackEnabled = TRUE;     

		StatusforGetHealth = DrvHealthInstance->GetHealthStatus (DrvHealthInstance, NULL, NULL, &HealthStatus, NULL, NULL);
		
		gEnableDrvNotification = FALSE;     
        gBrowserCallbackEnabled = FALSE;    

        if (EFI_ERROR (StatusforGetHealth))
        {
        	SETUP_DEBUG_UEFI ("[TSE]FindDriverHealthDriverName: Excluding handle with index=%d, StatusforGetHealth = %r \n", iIndex, StatusforGetHealth);
        	continue;
        }
        if (Count != DriverEntry)
        {
        	Count ++;
        	continue;
        }
        		
		Status = gBS->HandleProtocol (DriverHealthHandles [iIndex], &EfiComponentName2ProtocolGuid, (VOID**)&ComponentName2);
		if (EFI_ERROR (Status))
		{
			SETUP_DEBUG_UEFI ("[TSE]FindDriverHealthDriverName:  gBS->HandleProtocol(ComponentName2) = %r \n", Status);
			break;
		}
		LangStrings = StrDup8( ComponentName2->SupportedLanguages );
		tempLangStrings = LangStrings;							//Ptr will be incremented in _GetNextLanguage so taking backup for freeing
		_GetNextLanguage (&LangStrings, Lang);
		
		Status = ComponentName2->GetDriverName (
                            ComponentName2,
                            Lang,
                            //ComponentName2->SupportedLanguages,
                            &DriverName
                            );
		SETUP_DEBUG_UEFI ("[TSE] FindDriverHealthDriverName:  DriverName = %s, Status = %r \n", DriverName,Status);
		MemFreePointer( (VOID **)&tempLangStrings );
		
		if ((EFI_ERROR (Status)) || (NULL == DriverName))
		{
			break;
		}
		if (!EFI_ERROR (StatusforGetHealth))
		{
			DrvNameWithStatus = EfiLibAllocateZeroPool ((EfiStrLen (DriverName) + EfiStrLen (DriverHealthStatus [HealthStatus]) + 7) * sizeof (CHAR16));  //7 for spaces and NULL character
			if (NULL != DrvNameWithStatus)
			{
				SPrint (
					DrvNameWithStatus,
					((EfiStrLen (DriverName) + EfiStrLen (DriverHealthStatus [HealthStatus]) + 7) * sizeof (CHAR16)),
					L"%s     %s",
					DriverName,
					DriverHealthStatus [HealthStatus]
					);
					MemFreePointer ((VOID **)&DriverHealthHandles);
					SETUP_DEBUG_UEFI ("[TSE]Exiting FindDriverHealthDriverName:  DrvNameWithStatus = %s \n", DrvNameWithStatus);
					return DrvNameWithStatus;
			}
		}	
		goto DONE;
	}
	if (iIndex == NumHandles)
	{
		goto DONE;
	}
	if (!DriverName)
	{
		Status = gBS->HandleProtocol (DriverHealthHandles [iIndex], &gEfiDevicePathProtocolGuid, (VOID **) &DevicePath);
		if (!EFI_ERROR (Status))
		{
		    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToTextProtocol = (EFI_DEVICE_PATH_TO_TEXT_PROTOCOL*)NULL;
		    EFI_GUID DevPathToTextGuid = EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID;
		    EFI_STATUS RetStatus = EFI_UNSUPPORTED;
		    
		    RetStatus = gBS->LocateProtocol(&DevPathToTextGuid,NULL,&DevPathToTextProtocol);
		        
		    if(!EFI_ERROR(RetStatus) && DevPathToTextProtocol)
		    	DriverName =  DevPathToTextProtocol->ConvertDevicePathToText(DevicePath,TRUE,TRUE);
		    else
		    	DriverName = _DevicePathToStr (DevicePath);
	
			if (!EFI_ERROR (StatusforGetHealth))
			{
				DrvNameWithStatus = EfiLibAllocateZeroPool ((EfiStrLen (DriverName) + EfiStrLen (DriverHealthStatus [HealthStatus]) + 7) * sizeof (CHAR16));  //7 for spaces and NULL character
				if (NULL != DrvNameWithStatus)
				{
					SPrint (
						DrvNameWithStatus,
						((EfiStrLen (DriverName) + EfiStrLen (DriverHealthStatus [HealthStatus]) + 7) * sizeof (CHAR16)),
						L"%s     %s",
						DriverName,
						DriverHealthStatus [HealthStatus]
						);
						MemFreePointer ((VOID **)&DriverHealthHandles);
						SETUP_DEBUG_UEFI ("[TSE] Exiting FindDriverHealthDriverName:  DrvNameWithStatus from _DevicePathToStr = %s\n", DrvNameWithStatus);
						return DrvNameWithStatus;
				}
			}
			
			if(DevPathToTextProtocol)
				MemFreePointer(&DevPathToTextProtocol);
		}
		else
		{
			DriverName = EfiLibAllocateZeroPool (30 * sizeof (CHAR16));			//This case will be helpfull when driver name or device path not present
			SPrint (
				DriverName,
				(30 * sizeof (CHAR16)),
				L"Driver At Handle %x",
				DriverHealthHandles [iIndex]
				);
		}
	}
DONE:
	MemFreePointer ((VOID **)&DriverHealthHandles);
	SETUP_DEBUG_UEFI ( "[TSE] Exiting FindDriverHealthDriverName:  DriverName = %s\n", DriverName);
	return DriverName;
}

/**
    Finds the number of controllers supports driver health protocol for the driver entry

    @param UINT16 = Entry for driver to which total controllers has to be find

    @retval UINT16 = Total number of controllers supports driver health

**/
UINT16 CtrlsSupportsHealthProtocol (UINT16 EntryItem)
{
	EFI_STATUS  Status;
	EFI_HANDLE  *Handles;
	UINTN       NumHandles = 0;
	UINT16		ControllerCnt = 0, Count = 0;
	EFI_HANDLE  *DriverHealthHandles;
	UINTN       DriverHealthIndex;
	UINTN       ControllerIndex = 0;
	UINTN       HandleCount = 0;
	UINTN       ChildIndex = 0;
	EFI_DRIVER_HEALTH_PROTOCOL 	*DriverHealth;
	EFI_DRIVER_HEALTH_STATUS	HealthStatus;
    EFI_GUID EfiDriverHealthProtocolGuid = EFI_DRIVER_HEALTH_PROTOCOL_GUID;
    
    SETUP_DEBUG_UEFI( "\n[TSE] Entering CtrlsSupportsHealthProtocol, EntryItem: %d \n", EntryItem );

	Status = gBS->LocateHandleBuffer (
				ByProtocol,
				&EfiDriverHealthProtocolGuid,
				NULL,
				&NumHandles,
				&DriverHealthHandles
				);
	if ((EFI_ERROR (Status)) || (NumHandles == 0))
	{
		return ControllerCnt;
	}
	
	gEnableDrvNotification = TRUE;  
    gBrowserCallbackEnabled = TRUE; 
	
	for (DriverHealthIndex = 0; DriverHealthIndex < NumHandles; DriverHealthIndex++)
	{
		if (DriverHealthHandles [DriverHealthIndex] == NULL)
		{
			continue;
		}	
		Status = gBS->HandleProtocol (
			DriverHealthHandles [DriverHealthIndex],
			&EfiDriverHealthProtocolGuid,
			(VOID **)&DriverHealth
		);
		if (EFI_ERROR (Status))
		{
			continue;
		}

		Status = DriverHealth->GetHealthStatus (DriverHealth, NULL, NULL, &HealthStatus, NULL, NULL);
	
        if (EFI_ERROR (Status))
        {
        	SETUP_DEBUG_UEFI("[TSE]CtrlsSupportsHealthProtocol: Skipping DriverHealthHandles [%d]\n",DriverHealthIndex); 
        	continue;
        }
        		
		if (EntryItem > Count)
		{
			Count ++;
			continue;
		}
		gDrvHandle = DriverHealthHandles [DriverHealthIndex];
		gDrvHealthInstance = DriverHealth;
		Status = gBS->LocateHandleBuffer (
			AllHandles,
			NULL,
			NULL,
			&HandleCount,
			&Handles
	        );
		SETUP_DEBUG_UEFI("[TSE]CtrlsSupportsHealthProtocol: DriverHealthHandles [%d]\n",DriverHealthIndex); 
	    SETUP_DEBUG_UEFI("[TSE]CtrlsSupportsHealthProtocol: LocateHandleBuffer Status = %r, HandleCount=%d\n",Status,HandleCount);
		if (EFI_ERROR (Status))
        {
			//MemFreePointer ((VOID **)&DriverHealthHandles);
			//return ControllerCnt;
		    goto DONE ;
		}
 
		for (ControllerIndex = 0; ControllerIndex < HandleCount; ControllerIndex ++)
		{
			if (NULL == Handles [ControllerIndex])
			{
				continue;
      		}
			Status = DriverHealth->GetHealthStatus (DriverHealth, Handles [ControllerIndex], NULL, &HealthStatus, NULL, NULL);
			if (EFI_ERROR (Status))
			{
				continue;
			}
			SETUP_DEBUG_UEFI ("[TSE]CtrlsSupportsHealthProtocol: GetHealthStatus = %r ControllerIndex=%d\n",Status, ControllerIndex); 
			SETUP_DEBUG_UEFI ("[TSE]CtrlsSupportsHealthProtocol: AddHandleIntoList()\n"); 
			AddHandleIntoList (Handles [ControllerIndex], NULL, HealthStatus);
			ControllerCnt ++;
			for (ChildIndex = 0; ChildIndex < HandleCount; ChildIndex++)
			{
	       	if (Handles [ChildIndex] == NULL)
				{
          			continue;
    	    	}
				Status = DriverHealth->GetHealthStatus (DriverHealth, Handles [ControllerIndex], Handles [ChildIndex], &HealthStatus, NULL, NULL);
              if (EFI_ERROR (Status))
				{
					continue;
				}
                SETUP_DEBUG_UEFI ("[TSE]CtrlsSupportsHealthProtocol: GetHealthStatus HealthStatus:%s Status = %r, ChildIndex=%d FormHiiHandle:[0] ControllerHandle:[%X] ChildHandle:[%X]\n",DriverHealthStatus[HealthStatus],Status, ChildIndex,Handles [ControllerIndex],Handles [ChildIndex]); 
                SETUP_DEBUG_UEFI ("[TSE]CtrlsSupportsHealthProtocol: AddHandleIntoList()\n"); 
				AddHandleIntoList (Handles [ControllerIndex], Handles [ChildIndex], HealthStatus);
				ControllerCnt ++;
			}
		}
		break;
	}
DONE:
    MemFreePointer ((VOID **)&DriverHealthHandles);
    gEnableDrvNotification = FALSE;     
    gBrowserCallbackEnabled = FALSE;
	SETUP_DEBUG_UEFI("[TSE] Exiting CtrlsSupportsHealthProtocol, ControllerCnt: %x \n", ControllerCnt );
	return ControllerCnt;
}	

/**
    Finds the controller name with its health

    @param UINT16 = Entry for controller to which name has to be find

    @retval CHAR16 * = Controller name with its health status

**/
CHAR16 *GetCtrlNameAndHealth (UINT16 ControllerEntry)
{
	UINT16  Count = 0;
	CHAR16	*ContrNameAndHlth = NULL;
	DRV_HEALTH_HNDLS *DrvHlthHandles = NULL;

	SETUP_DEBUG_UEFI("\n[TSE] Entering GetCtrlNameAndHealth\n");
	DrvHlthHandles = gDrvHealthHandlesHead;
	SETUP_DEBUG_UEFI("[TSE]GetCtrlNameAndHealth ControllerEntry :%X\n",ControllerEntry);
	while (Count != ControllerEntry)
	{
		DrvHlthHandles = DrvHlthHandles->Next;
		Count ++;
	}
	ContrNameAndHlth = GetDrvHlthCtrlName (DrvHlthHandles);
	SETUP_DEBUG_UEFI("[TSE]GetCtrlNameAndHealth ContrNameAndHlth :%s\n",ContrNameAndHlth);
	SETUP_DEBUG_UEFI("[TSE] Exiting GetCtrlNameAndHealth\n");
	return ContrNameAndHlth;
}

/**
    Finds the controller name

    @param DRV_HEALTH_HNDLS * = Details for finding the controller name

    @retval CHAR16 * = Controller name for the controller and child handle

**/
CHAR16 *GetDrvHlthCtrlName (DRV_HEALTH_HNDLS *DrvHlthHandles)// EFI_HANDLE DriverHealth, EFI_HANDLE ControllerHandle, EFI_HANDLE ChildHandle)
{
	EFI_STATUS  Status = EFI_SUCCESS;
	CHAR16		*CtrlName = NULL;
	CHAR16		*ContrNameAndHlth = NULL;
	EFI_GUID 	EfiComponentName2ProtocolGuid = EFI_COMPONENT_NAME2_PROTOCOL_GUID;
	EFI_COMPONENT_NAME2_PROTOCOL	*ComponentName2 = NULL;
	EFI_DRIVER_HEALTH_STATUS 		HealthStatus = EfiDriverHealthStatusHealthy;  
	BOOLEAN		ReleaseMem=FALSE;	
	EFI_HII_HANDLE				FormHiiHandle;
	SETUP_DEBUG_UEFI("\n[TSE] Entering GetDrvHlthCtrlName()\n");
	Status = gDrvHealthInstance->GetHealthStatus (				//If controllers reconnected status might change so getting the status again
					gDrvHealthInstance,
	  				DrvHlthHandles->ControllerHandle,
		    		DrvHlthHandles->ChildHandle, 
					&HealthStatus,
					NULL,
					&FormHiiHandle
					);
	SETUP_DEBUG_UEFI ("[TSE]GetDrvHlthCtrlName: gDrvHealthInstance->GetHealthStatus: HealthStatus :%s Status:%r  FormHiiHandle:[%X] DrvHlthHandles->ControllerHandle:[%X] DrvHlthHandles->ChildHandle:[%X]\n",DriverHealthStatus[HealthStatus],Status,FormHiiHandle,DrvHlthHandles->ControllerHandle,DrvHlthHandles->ChildHandle);
	if (EFI_ERROR (Status))
	{
		return NULL;
	}
	DrvHlthHandles->HealthStatus = HealthStatus;
	Status = gBS->HandleProtocol (
		gDrvHandle,
		&EfiComponentName2ProtocolGuid,
		(VOID **)&ComponentName2
	);
	if (!EFI_ERROR (Status))
 	{
		Status = ComponentName2->GetControllerName (ComponentName2, DrvHlthHandles->ControllerHandle, DrvHlthHandles->ChildHandle, ComponentName2->SupportedLanguages, &CtrlName);
		if (EFI_ERROR (Status))
	 	{
			SETUP_DEBUG_UEFI ("[TSE]GetDrvHlthCtrlName: ComponentName2->GetControllerName Status :%r  DrvHlthHandles->ControllerHandle:[%X] DrvHlthHandles->ChildHandle:[%X]\n",Status,DrvHlthHandles->ControllerHandle,DrvHlthHandles->ChildHandle);
			CtrlName = NULL;
		}
	}
	if (NULL == CtrlName)
	{
		CtrlName = EfiLibAllocateZeroPool (50 * sizeof (CHAR16)); //16+16+"DriverName"
		if (NULL == CtrlName)
		{
			SETUP_DEBUG_UEFI ("[TSE]GetDrvHlthCtrlName:  CtrlName :NULL\n");
			return NULL;
		}
		SPrint (CtrlName, 50 * sizeof (CHAR16), L"Controller %x Child %x", DrvHlthHandles->ControllerHandle, DrvHlthHandles->ChildHandle);
		ReleaseMem=TRUE;
	}
	ContrNameAndHlth = EfiLibAllocateZeroPool ((EfiStrLen (CtrlName) + EfiStrLen (DriverHealthStatus [HealthStatus]) + 7) * sizeof (CHAR16));		//7 for spaces and NULL char.
	if (NULL == ContrNameAndHlth)
	{
		SETUP_DEBUG_UEFI ("[TSE]GetDrvHlthCtrlName:  ContrNameAndHlth :NULL\n");
		return NULL;
	}
	SPrint (
		ContrNameAndHlth,
		((EfiStrLen (CtrlName) + EfiStrLen (DriverHealthStatus [HealthStatus]) + 7) * sizeof (CHAR16)),
		L"%s     %s",     
		CtrlName,
		DriverHealthStatus [HealthStatus]
		);
	if(ReleaseMem)
		MemFreePointer( (VOID **)&CtrlName );
	SETUP_DEBUG_UEFI ("[TSE]GetDrvHlthCtrlName: Controller name = %s,\n",ContrNameAndHlth);
	SETUP_DEBUG_UEFI("[TSE] Exiting GetDrvHlthCtrlName()\n");
	return ContrNameAndHlth;
}

/**
    Adds the driver health details to the linked list

    @param EFI_HANDLE = Controller handle
        EFI_HANDLE = Child handle
        EFI_DRIVER_HEALTH_STATUS = Health status corresponding to controller and child handle

    @retval VOID

**/
VOID AddHandleIntoList (EFI_HANDLE ControllerHandle, EFI_HANDLE ChildHandle, EFI_DRIVER_HEALTH_STATUS HealthStatus)
{
	DRV_HEALTH_HNDLS *DrvHealthHandles;
	DRV_HEALTH_HNDLS *tempHealthHandle;
	SETUP_DEBUG_UEFI("\n[TSE] Entering AddHandleIntoList()\n");
	DrvHealthHandles = EfiLibAllocateZeroPool (sizeof (DRV_HEALTH_HNDLS));
	if (NULL == DrvHealthHandles)
	{
		return;
	}
	DrvHealthHandles->ControllerHandle = ControllerHandle;
	DrvHealthHandles->ChildHandle = ChildHandle;
	DrvHealthHandles->HealthStatus = HealthStatus;
	DrvHealthHandles->Next = NULL;
	if (NULL == gDrvHealthHandlesHead)
	{	
		SETUP_DEBUG_UEFI("\n[TSE] AddHandleIntoList():gDrvHealthHandlesHead = DrvHealthHandles :HealthStatus:%s ControllerHandle:[%X] ChildHandle:[%X]\n",DriverHealthStatus[DrvHealthHandles->HealthStatus],DrvHealthHandles->ControllerHandle,DrvHealthHandles->ChildHandle);
		gDrvHealthHandlesHead = DrvHealthHandles;
		return;
	}
	tempHealthHandle = gDrvHealthHandlesHead;
	while (tempHealthHandle->Next)
	{
		tempHealthHandle = tempHealthHandle->Next;
	}
	tempHealthHandle->Next = DrvHealthHandles;
	SETUP_DEBUG_UEFI("\n[TSE] Exiting AddHandleIntoList()\n");
}

/**
    Frees the driver health details linked list

    @param VOID

    @retval VOID

**/
VOID FreeControllersList (VOID)
{
	DRV_HEALTH_HNDLS *DrvHealthHandles, *tempDrvHealthHandles;
	SETUP_DEBUG_UEFI("\n[TSE] Entering FreeControllersList()\n");
	if (!gDrvHealthHandlesHead)
	{
		SETUP_DEBUG_UEFI("\n[TSE]  FreeControllersList():gDrvHealthHandlesHead NULL\n");
		return;
	}
	DrvHealthHandles = gDrvHealthHandlesHead;
	while (DrvHealthHandles)
	{
		tempDrvHealthHandles = DrvHealthHandles;
		DrvHealthHandles = DrvHealthHandles->Next;
		SETUP_DEBUG_UEFI("\n[TSE]FreeControllersList() Freedhandle :tempDrvHealthHandles :%X \n",tempDrvHealthHandles);
		MemFreePointer ((VOID **)&tempDrvHealthHandles);
	}
	SETUP_DEBUG_UEFI("\n[TSE] Exiting FreeControllersList()\n");
}

/**
    Finds and sets the number of controllers supports the driver health handle

    @param UINT16 = Driver health handle entry

    @retval VOID

**/
VOID SetDrvHealthHandleAndCtrlCount (UINT16 ItemEntry)
{
	UINTN 		TotControllers = 0;
	EFI_GUID 	DrvHealthCtrlGuid = AMITSE_DRIVER_HEALTH_CTRL_GUID;
	
	SETUP_DEBUG_UEFI("\n[TSE] Entering SetDrvHealthHandleAndCtrlCount, ItemEntry: %x \n", ItemEntry );
	
	FreeControllersList ();
	gDrvHealthHandlesHead = NULL;

	TotControllers = CtrlsSupportsHealthProtocol (ItemEntry);
	VarSetNvramName (L"DrvHealthCtrlCnt", &DrvHealthCtrlGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &TotControllers, sizeof (UINTN));
	VarUpdateVariable (VARIABLE_ID_DRV_HLTH_CTRL_COUNT); //Update NVRAM cache for DriverHealthCount
	SETUP_DEBUG_UEFI("\n[TSE]SetDrvHealthHandleAndCtrlCount() TotControllers:%X \n",TotControllers);
	SETUP_DEBUG_UEFI("[TSE] Exiting SetDrvHealthHandleAndCtrlCount, TotalControllers:  %x \n",  TotControllers);
}

/**
    Sets the DriverHealthCount and DriverHlthEnable variables

    @param VOID

    @retval VOID

**/
VOID SetDriverHealthCount (VOID)
{
	EFI_STATUS  Status;
	EFI_GUID 	DriverHealthProtocolGuid = EFI_DRIVER_HEALTH_PROTOCOL_GUID;
	EFI_HANDLE  *DriverHealthHandles = NULL;
	UINTN       NumHandles = 0, iIndex = 0, HealthCount = 0;
	EFI_GUID 	DrvHealthGuid = AMITSE_DRIVER_HEALTH_GUID;
	EFI_GUID 	DrvHlthEnableGuid = AMITSE_DRIVER_HEALTH_ENB_GUID;
	EFI_DRIVER_HEALTH_PROTOCOL	*DrvHealthInstance = NULL;
	EFI_DRIVER_HEALTH_STATUS  		HealthStatus = EfiDriverHealthStatusHealthy;
	
	SETUP_DEBUG_UEFI("\n[TSE] Entering SetDriverHealthCount()\n");
	
	Status = gBS->LocateHandleBuffer (
				ByProtocol,
				&DriverHealthProtocolGuid,
				NULL,
				&NumHandles,
				&DriverHealthHandles
				);
	if (EFI_SUCCESS != Status)
	{
		NumHandles = 0;
	}
	for (iIndex = 0; iIndex < NumHandles; iIndex ++)
	{
		Status = gBS->HandleProtocol (DriverHealthHandles [iIndex], &DriverHealthProtocolGuid, (VOID**)&DrvHealthInstance);
		if (EFI_ERROR (Status))
		{
			continue;
		}
		Status = DrvHealthInstance->GetHealthStatus (DrvHealthInstance, NULL, NULL, &HealthStatus, NULL, NULL);
		if (EFI_ERROR (Status))
		{
			 SETUP_DEBUG_UEFI("\n[TSE] DriverHealthProtocolGuid found, but Status from GetHealthStatus HealthStatus:%s Status =%r, index = %d  FormHiiHandle:[0] ControllerHandle:[0] ChildHandle:[0]\n", DriverHealthStatus[HealthStatus],Status, iIndex );
			 continue;
		}
		HealthCount ++;
	}
	if (NumHandles)
	{
		MemFreePointer ((VOID **)&DriverHealthHandles);
	}
    SETUP_DEBUG_UEFI("\n[TSE] DriverHealthCount() -> HealthCount = %d\n", HealthCount );
	VarSetNvramName (L"DriverHealthCount", &DrvHealthGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &HealthCount, sizeof (UINTN));
    VarUpdateVariable (VARIABLE_ID_DRV_HLTH_COUNT);             //Update NVRAM cache for DriverHealthCount
    HealthCount = HealthCount ? 1 : 0;          //To enable driver health root menu
    VarSetNvramName (L"DriverHlthEnable", &DrvHlthEnableGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &HealthCount, sizeof (UINTN));
    VarUpdateVariable (VARIABLE_ID_DRV_HLTH_ENB);               //Update NVRAM cache for DriverHlthEnable
    SETUP_DEBUG_UEFI("\n[TSE] Exiting SetDriverHealthCount()");
}

/**
    Displays the messages from EFI_DRIVER_HEALTH_HII_MESSAGE

    @param EFI_DRIVER_HEALTH_HII_MESSAGE * = Message to show

    @retval VOID

**/
VOID DisplayMsgListMessageBox (TSE_EFI_DRIVER_HEALTH_HII_MESSAGE *MessageList)
{
	CHAR16 	*DisplayString = NULL;
	CHAR16	*String = NULL;
	UINT8 	Count = 0;
	UINT8	MsgBoxSel = 0;
	EFI_STATUS	Status = EFI_SUCCESS;
	CHAR16 MessageCode [50];
	SETUP_DEBUG_UEFI("\n[TSE] Entering DisplayMsgListMessageBox()");
	while (MessageList [Count].HiiHandle)
	{
		String = HiiGetString (MessageList [Count].HiiHandle, MessageList [Count].StringId);
		if (Count > 15)							//Checking for safe termination, otherwise msgbox fill the full screen
		{
			break;
		}
		if (String)
		{
			if (!DisplayString)
			{
				DisplayString = EfiLibAllocateZeroPool ((EfiStrLen (String) + 2) * sizeof (CHAR16));		//2 for /n and NULL character
			}
			else
			{
				DisplayString = MemReallocateZeroPool (
									DisplayString,
									( (EfiStrLen (DisplayString) + 1) * sizeof (CHAR16) ),
									( ((EfiStrLen (String) + 2) * sizeof (CHAR16)) + ((EfiStrLen (DisplayString) + 1) * sizeof (CHAR16)) )    //2 for /n and NULL character
									);	
			}
			if (NULL == DisplayString)
			{
				MemFreePointer ((VOID **)&String);
				continue;
			}
			if (DisplayString [0])
			{
				EfiStrCat (DisplayString, String);
			}
			else
			{
				EfiStrCpy (DisplayString, String);
			}
			MemFreePointer ((VOID **)&String);
			EfiStrCat (DisplayString, L"\n");
			//If messagecode is non zero only, append it to displaystring
			if(MessageList[Count].MessageCode)
			{
			//The maximum size of buffer (MessageCode) is 50
				SPrint(MessageCode,50,L" Message Code : %ld \n",MessageList[Count].MessageCode);
				DisplayString = MemReallocateZeroPool (
													DisplayString,
													( (EfiStrLen (DisplayString) + 1) * sizeof (CHAR16) ),
													( ((EfiStrLen (MessageCode) + 2) * sizeof (CHAR16)) + ((EfiStrLen (DisplayString) + 1) * sizeof (CHAR16)) )    //2 for /n and NULL character
													);	
				EfiStrCat (DisplayString, MessageCode);
			}
			Count ++;
		}
	}
	if (DisplayString)
	{
		CHAR16 *text = NULL;
		text = HiiGetString (gHiiHandle, STRING_TOKEN (STR_DRV_HLTH_TITLE));
		Status = mPostMgr->DisplayMsgBox (
					text,
					DisplayString,
					MSGBOX_TYPE_OK, 
					&MsgBoxSel		
				);
		if(EFI_ERROR (Status))
			return;
		
		MemFreePointer ((VOID **)&DisplayString);
		if (text)
		{
			MemFreePointer ((VOID **)&text);
		}	
	}
	SETUP_DEBUG_UEFI("\n[TSE] Exiting DisplayMsgListMessageBox()");
}

/**
    Performs the driver health operations for the corresponding controller entry

    @param VOID * = Control Info for the correponding controller entry
        UINT16 = Controller entry in the page

    @retval VOID

**/
EFI_STATUS ShowDriverHealthConfigForm (PAGE_INFO *pageInfo);
VOID DoDriverHealthOperation (VOID *Tempcontrol, UINT16 ControllerEntry)
{
	UINT16 						Count = 0;
	EFI_STATUS					Status = EFI_SUCCESS;
	DRV_HEALTH_HNDLS 			*DrvHlthHandles = NULL;
	EFI_DRIVER_HEALTH_STATUS  		LocalHealthStatus = EfiDriverHealthStatusHealthy;
	EFI_DRIVER_HEALTH_HII_MESSAGE 	*MessageList = NULL;
	EFI_HII_HANDLE				FormHiiHandle;
	UINT8						MsgBoxSel = 0;
	CONTROL_INFO                    		*control = (CONTROL_INFO *)Tempcontrol;

	SETUP_DEBUG_UEFI ("\nEntering DoDriverHealthOperation:\n");
	gEnableDrvNotification = TRUE;  
	gBrowserCallbackEnabled = TRUE; 
	
	if(!mPostMgr)
	{
		//Loacate Post Manager Protocol
		Status = gBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL,(void**) &mPostMgr);
		if(Status != EFI_SUCCESS)
			//return;
		    goto DONE ;
	}
	
	DrvHlthHandles = gDrvHealthHandlesHead;
	while (Count != ControllerEntry)
	{
		DrvHlthHandles = DrvHlthHandles->Next;
		Count ++;
	}
	if (EfiDriverHealthStatusHealthy == DrvHlthHandles->HealthStatus)
	{
		SETUP_DEBUG_UEFI("DoDriverHealthOperation: DrvHlthHandles->HealthStatus = EfiDriverHealthStatusHealthy \n");
	    Status = gDrvHealthInstance->GetHealthStatus (
						gDrvHealthInstance,
						DrvHlthHandles->ControllerHandle,
						DrvHlthHandles->ChildHandle, 
						&LocalHealthStatus,
						&MessageList,
						&FormHiiHandle
					);
	    SETUP_DEBUG_UEFI ("DoDriverHealthOperation: gDrvHealthInstance->GetHealthStatus1  HealthStatus= %s Status:%r FormHiiHandle:[%X] DrvHlthHandles->ControllerHandle :[%X]  DrvHlthHandles->ChildHandle:[%X]\n",DriverHealthStatus[LocalHealthStatus],Status,FormHiiHandle,DrvHlthHandles->ControllerHandle,DrvHlthHandles->ChildHandle);
		if (NULL != MessageList)
		{
		    DisplayMsgListMessageBox ((TSE_EFI_DRIVER_HEALTH_HII_MESSAGE *)MessageList);
		}
	}
	if (EfiDriverHealthStatusRepairRequired == DrvHlthHandles->HealthStatus)
	{
		SETUP_DEBUG_UEFI ("DoDriverHealthOperation: DrvHlthHandles->HealthStatus = EfiDriverHealthStatusRepairRequired \n");
		Status = gDrvHealthInstance->Repair (
					gDrvHealthInstance,
					DrvHlthHandles->ControllerHandle,
					DrvHlthHandles->ChildHandle,
					NULL
                    	    	    );
		SETUP_DEBUG_UEFI ("DoDriverHealthOperation: gDrvHealthInstance->Repair Status= %r DrvHlthHandles->ControllerHandle:[%X]  DrvHlthHandles->ChildHandle:[%X] \n",Status,DrvHlthHandles->ControllerHandle,DrvHlthHandles->ChildHandle);
		if (!EFI_ERROR (Status))
		{
			Status = gDrvHealthInstance->GetHealthStatus (
						gDrvHealthInstance,
		  				DrvHlthHandles->ControllerHandle,
		  				DrvHlthHandles->ChildHandle, 
						&LocalHealthStatus,
						&MessageList,
						&FormHiiHandle
					);
			SETUP_DEBUG_UEFI ("DoDriverHealthOperation: gDrvHealthInstance->GetHealthStatus->2 HealthStatus:%s Status:%r FormHiiHandle:[%X]  DrvHlthHandles->ControllerHandle:[%X]  DrvHlthHandles->ChildHandle:[%X]\n",DriverHealthStatus[LocalHealthStatus],Status,FormHiiHandle,DrvHlthHandles->ControllerHandle,DrvHlthHandles->ChildHandle);
			if (NULL != MessageList)
			{
				DisplayMsgListMessageBox ((TSE_EFI_DRIVER_HEALTH_HII_MESSAGE *)(MessageList));
			}
		}
	}
	if ((EfiDriverHealthStatusConfigurationRequired == DrvHlthHandles->HealthStatus) || 
		(EfiDriverHealthStatusConfigurationRequired == LocalHealthStatus))
	{
		UINT32 		i=0;
		PAGE_INFO 	*pageInfo;
		if (EfiDriverHealthStatusConfigurationRequired != LocalHealthStatus)
		{
			SETUP_DEBUG_UEFI ("DoDriverHealthOperation: DrvHlthHandles->HealthStatus  LocalHealthStatus= %s  DrvHlthHandles->HealthStatus:%s \n",DriverHealthStatus[LocalHealthStatus],DriverHealthStatus[DrvHlthHandles->HealthStatus]);
			Status = gDrvHealthInstance->GetHealthStatus (
						gDrvHealthInstance,
						DrvHlthHandles->ControllerHandle,
						DrvHlthHandles->ChildHandle, 
						&LocalHealthStatus,
						&MessageList,
						&FormHiiHandle
					);
			SETUP_DEBUG_UEFI ("DoDriverHealthOperation: gDrvHealthInstance->GetHealthStatus->3 HealthStatus = %s Status:%r  FormHiiHandle:[%X] DrvHlthHandles->ControllerHandle:[%X] DrvHlthHandles->ChildHandle:[%X] \n",DriverHealthStatus[LocalHealthStatus],Status,FormHiiHandle,DrvHlthHandles->ControllerHandle,DrvHlthHandles->ChildHandle);
		}
		else
		{
			SETUP_DEBUG_UEFI ("DoDriverHealthOperation: LocalHealthStatus = EfiDriverHealthStatusRepairRequired \n");
		}
		if (!EFI_ERROR (Status) && (FormHiiHandle))
		{
			if (NULL != MessageList)
			{
				DisplayMsgListMessageBox ((TSE_EFI_DRIVER_HEALTH_HII_MESSAGE *)MessageList);
			}
			for (i = 0; i < gPages->PageCount; i++)
			{
				pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[i]);
				if (pageInfo->PageHandle == FormHiiHandle)
				{
//				    PAGE_DATA **page;                   //To display the configuration page in advanced menu itself
				    
				    control->ControlDestPageID = i;				    
				    pageInfo->PageFlags.PageVisible = FALSE;
				    pageInfo->PageParentID = control->ControlPageID;
				    SETUP_DEBUG_UEFI ("DoDriverHealthOperation: pageInfo->PageHandle == FormHiiHandle :%X \n",FormHiiHandle);
				    //For ESA gApp will be NULL but for TSE gApp will be not-NULL
				    if(gApp)
				    {
				    	ShowDriverHealthConfigForm (pageInfo);
				    	SETUP_DEBUG_UEFI ("DoDriverHealthOperation: gApp->CurrentPage :%X \n",i);
				    	gApp->CurrentPage = i;
/*				    	
					    page = &gApp->PageList [pageInfo->PageID];
					    gPage.Destroy (*page, FALSE);
					    gPage.Initialize (*page, pageInfo);
*/					    
				    }
				    break;
				}
			}
		}
	}
	if (EfiDriverHealthStatusFailed == DrvHlthHandles->HealthStatus)
	{
		SETUP_DEBUG_UEFI ("DoDriverHealthOperation: DrvHlthHandles->HealthStatus = EfiDriverHealthStatusFailed \n");
		Status = gDrvHealthInstance->GetHealthStatus (
						gDrvHealthInstance,
						DrvHlthHandles->ControllerHandle,
						DrvHlthHandles->ChildHandle, 
						&LocalHealthStatus,
						&MessageList,
						&FormHiiHandle
					);
		SETUP_DEBUG_UEFI ("DoDriverHealthOperation: gDrvHealthInstance->GetHealthStatus->4 HealthStatus = %s Status:%r FormHiiHandle:[%X]  DrvHlthHandles->ControllerHandle:[%X]  DrvHlthHandles->ChildHandle:[%X]\n",DriverHealthStatus[LocalHealthStatus],Status,FormHiiHandle,DrvHlthHandles->ControllerHandle,DrvHlthHandles->ChildHandle);
		if ((!EFI_ERROR (Status)) && (NULL != MessageList))
		{
			DisplayMsgListMessageBox ((TSE_EFI_DRIVER_HEALTH_HII_MESSAGE *)MessageList);
		}
	}
	if (EfiDriverHealthStatusReconnectRequired == DrvHlthHandles->HealthStatus)
	{
		CHAR16 *Title = NULL;
		CHAR16 *Message = NULL;
        
		SETUP_DEBUG_UEFI ("DoDriverHealthOperation: DrvHlthHandles->HealthStatus = EfiDriverHealthStatusReconnectRequired \n");
        
		Title = HiiGetString (gHiiHandle, STRING_TOKEN (STR_DRV_HLTH_TITLE));
		Message = HiiGetString (gHiiHandle, STRING_TOKEN (STR_DRV_HLTH_RECON));
		Status = mPostMgr->DisplayMsgBox (
					Title,
					Message,
					MSGBOX_TYPE_OKCANCEL, 
					&MsgBoxSel		
				);
		MemFreePointer( (VOID **)&Title );
		MemFreePointer( (VOID **)&Message );
		if(EFI_ERROR (Status))
		    goto DONE ;
			//return;
		
		if (0 == MsgBoxSel)			//User pressed OK
		{
			//If gEnableDrvNotification is true then it will process the notification and hang happens so making here false
	        gEnableDrvNotification = FALSE;    
	        gBrowserCallbackEnabled = FALSE;
			gBS->DisconnectController (DrvHlthHandles->ControllerHandle, NULL, NULL);
			gBS->ConnectController (DrvHlthHandles->ControllerHandle, NULL, NULL, TRUE);
			gEnableDrvNotification = TRUE;
			gBrowserCallbackEnabled = TRUE;
		}
	}
	if (EfiDriverHealthStatusRebootRequired == DrvHlthHandles->HealthStatus)
	{
		CHAR16 *Title = NULL;
		CHAR16 *Message = NULL;
		
		SETUP_DEBUG_UEFI ("DoDriverHealthOperation: DrvHlthHandles->HealthStatus = EfiDriverHealthStatusReconnectRequired \n");
		
        Title = HiiGetString (gHiiHandle, STRING_TOKEN (STR_DRV_HLTH_TITLE));
		Message = HiiGetString (gHiiHandle, STRING_TOKEN (STR_DRV_HLTH_REBOOT));
		Status = mPostMgr->DisplayMsgBox (
					Title,
					Message,
					MSGBOX_TYPE_OKCANCEL, 
					&MsgBoxSel		
				);
		MemFreePointer( (VOID **)&Title );
		MemFreePointer( (VOID **)&Message );
		if(EFI_ERROR (Status))
		    goto DONE ;
		    //return;
		
		if (0 == MsgBoxSel)			//User pressed OK
		{
    		DrvHlthHandles = gDrvHealthHandlesHead;
			while (DrvHlthHandles)
			{
				//Proceed with repair for all the controllers in repair state
				if (EfiDriverHealthStatusRepairRequired == DrvHlthHandles->HealthStatus)	
				{
					Status = gDrvHealthInstance->Repair (
								gDrvHealthInstance,
								DrvHlthHandles->ControllerHandle,
								DrvHlthHandles->ChildHandle,
								NULL
					    	    	    );
					SETUP_DEBUG_UEFI ("DoDriverHealthOperation: gDrvHealthInstance->Repair->  Status:%r DrvHlthHandles->ControllerHandle:[%X]  DrvHlthHandles->ChildHandle:[%X]\n",Status,DrvHlthHandles->ControllerHandle);
				}
				DrvHlthHandles = DrvHlthHandles->Next;
			}
    		FreeControllersList ();
			gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
		}
	}
	
DONE:
        gEnableDrvNotification = FALSE;     
        gBrowserCallbackEnabled = FALSE;    
        SETUP_DEBUG_UEFI ("Exiting DoDriverHealthOperation()\n");       
}


/**
    Sets the DynamicPageGroupCount variable

    @param VOID

    @retval VOID

**/
VOID SetDynamicPageGroupCount (VOID)
{
    UINT16 iIndex = 0, GroupCount = 0;
    
    EFI_GUID DynamicPageGroupGuid = DYNAMIC_PAGE_GROUP_COUNT_GUID ;
    EFI_GUID DynamicPageGroupClassGuid = DYNAMIC_PAGE_GROUP_CLASS_COUNT_GUID ;
    DYNAMIC_PAGE_GROUP_CLASS DynamicPageGroupClass;
    
    SETUP_DEBUG_TSE ( "\n[TSE] Entering SetDynamicPageGroupCount()\n" );
        
    for (iIndex = 0; iIndex < DYNAMIC_PAGE_GROUPS_SIZE ; iIndex ++)
    {
        SETUP_DEBUG_TSE ("\n[TSE] gDynamicPageGroup[0x%x].Count = 0x%x \n" ,iIndex, gDynamicPageGroup[iIndex].Count );
        
        if (gDynamicPageGroup[iIndex].Count){
            GroupCount ++; 
        }
        switch(iIndex)
        {
        case 0:
        	DynamicPageGroupClass.NonDeviceClassCount = gDynamicPageGroup[iIndex].Count;
        	break;
        case 1:
        	DynamicPageGroupClass.DiscDeviceClassCount = gDynamicPageGroup[iIndex].Count;
        	break;
        case 2:
        	DynamicPageGroupClass.VideoDeviceClassCount = gDynamicPageGroup[iIndex].Count;
        	break;
        case 3:
        	DynamicPageGroupClass.NetworkDeviceClassCount = gDynamicPageGroup[iIndex].Count;
        	break;
        case 4:
        	DynamicPageGroupClass.InputDeviceClassCount = gDynamicPageGroup[iIndex].Count;
        	break;
        case 5:
        	DynamicPageGroupClass.OnBoardDeviceClassCount = gDynamicPageGroup[iIndex].Count;
        	break;
        case 6:
        	DynamicPageGroupClass.OtherDeviceClassCount = gDynamicPageGroup[iIndex].Count;
        }
    }
    
    gDynamicPageGroupCount = GroupCount ;
    VarSetNvramName (L"DynamicPageGroupCount", &DynamicPageGroupGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &GroupCount, sizeof (UINT16));
    VarUpdateVariable(VARIABLE_ID_DYNAMIC_PAGE_GROUP);   //Update nvram cache for DynamicPageGroupCount    
    SETUP_DEBUG_TSE ( "\n[TSE] SetDynamicPageGroupCount() DynamicPageGroupCount:%x\n",GroupCount );

    VarSetNvramName (L"DynamicPageGroupClass", &DynamicPageGroupClassGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS, &DynamicPageGroupClass, sizeof (DYNAMIC_PAGE_GROUP_CLASS));
    VarUpdateVariable(VARIABLE_ID_DYNAMIC_PAGE_GROUP_CLASS);   //Update nvram cache for DynamicPageGroupCount    

SETUP_DEBUG_TSE ( "\n[TSE] Exiting SetDynamicPageGroupCount()\n" );
}


/**
    Function to check the time flags.

    @param ItemCount 

    @retval VOID
**/
BOOLEAN CheckTimeFlags(UINT8 Flags)
{
	return ((Flags & QF_TIME_STORAGE) == QF_TIME_STORAGE_NORMAL) ;
}

/**
    Function to check the date flags.

    @param ItemCount 

    @retval VOID
**/
BOOLEAN CheckDateFlags(UINT8 Flags)
{
	return ((Flags & EFI_QF_DATE_STORAGE) == QF_DATE_STORAGE_NORMAL);
}

/**
    Creates a file to output hpk data

    @param filenameFormat 
    @param Handle 
    @param Index 
			

    @retval EFI_FILE_PROTOCOL*
**/
EFI_FILE_PROTOCOL * CreateFile(CHAR16 *filename, UINT32 index)
{
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *pSFP;
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_FILE_PROTOCOL *pRoot,*FileHandle;
    EFI_HANDLE *gSmplFileSysHndlBuf = NULL;  
	UINTN Count = 0;
    //UINT16 i = 0;
	SETUP_DEBUG_TSE ( "\n[TSE] Entering CreateFile()\n" );
	// Locate all the simple file system devices in the system
    Status = pBS->LocateHandleBuffer(ByProtocol, &guidSimpleFileSystemProtocol, NULL, &Count, &gSmplFileSysHndlBuf);
	
	if(!EFI_ERROR(Status))
    {
	 	Status = gBS->HandleProtocol( gSmplFileSysHndlBuf[index], &guidSimpleFileSystemProtocol, &pSFP );

		if (!EFI_ERROR(Status))
		{
			Status = pSFP->OpenVolume(pSFP,&pRoot);
		
			if (!EFI_ERROR(Status))
			{
				Status = pRoot->Open(pRoot,
				                    &FileHandle,
				                    filename,
				                    EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
				                    0);
				
				pRoot->Close(pRoot);
		
				if (!EFI_ERROR(Status))
	 			{
					return FileHandle;
				}
			}
		}
	}

	if(gSmplFileSysHndlBuf != NULL) 
        pBS->FreePool(gSmplFileSysHndlBuf);
	SETUP_DEBUG_TSE ( "\n[TSE] Exiting CreateFile()\n" );
	return NULL;
}

/**
    Writes data to file

    @param filename VOID *Data, UINTN length
			

    @retval VOID
**/
EFI_STATUS WriteDataToFile(CHAR16 *filename, VOID *Data, UINTN length, UINT32 index)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	EFI_FILE_PROTOCOL *FileHandle=NULL;

	UINTN len = length;

	SETUP_DEBUG_UEFI_NOTIFICATION( "\n\n[TSE] Entering WriteDataToFile \n\n");
	
	if (Data == NULL)
	{
    	goto DONE;
	}	
  	
	FileHandle = CreateFile(filename, index);

	if (FileHandle != NULL)
	{
		FileHandle->SetPosition(FileHandle,0);
		FileHandle->Write(FileHandle,&len, Data);
		FileHandle->Close(FileHandle);
		Status = EFI_SUCCESS ;

		SETUP_DEBUG_UEFI_NOTIFICATION( "\n\n[TSE] Exporting Filename: %s \n\n", filename);
	}
DONE:
	
	SETUP_DEBUG_UEFI_NOTIFICATION( "\n\n[TSE] Exiting WriteDataToFile, Status: %x \n\n", Status);

	return Status ;
}

#if TSE_DEBUG_MESSAGES
/**
    Get HII form package by handle and form ID

    @param Handle 
    @param form 
    @param Length 

    @retval VOID *formPkBuffer - 
**/
VOID *HiiGetPackList( EFI_HII_HANDLE Handle, UINT16 form, UINTN *Length )
{
	EFI_STATUS status = EFI_SUCCESS;
	VOID *hiiFormPackage=NULL;
	UINTN len=10;

	if(Length == NULL)
		Length = &len;

	hiiFormPackage = EfiLibAllocateZeroPool(*Length);

	status = gHiiDatabase->ExportPackageLists(gHiiDatabase, Handle, Length, hiiFormPackage);
	if(status == EFI_BUFFER_TOO_SMALL)
	{
		MemFreePointer(&hiiFormPackage);
		//
		// Allocate space for retrieval of IFR data
		//
		hiiFormPackage = EfiLibAllocateZeroPool(*Length);

		if (hiiFormPackage == NULL)
		{
			hiiFormPackage = NULL;
			goto DONE;
		}
		//
		// Get all the packages associated with this HiiHandle
		//
		status = gHiiDatabase->ExportPackageLists (gHiiDatabase, Handle, Length, hiiFormPackage);
		if(EFI_ERROR(status))
		{
			MemFreePointer(&hiiFormPackage);
			hiiFormPackage = NULL;
		}
	}
	else if(EFI_ERROR(status))
	{
		hiiFormPackage = NULL;
	}

DONE:
	return hiiFormPackage;
}

/**
    Prints the HPK data to file

    @param NotifyType EFI_HANDLE Handle
			

    @retval VOID
**/
VOID ProcessPackToFile(EFI_HII_DATABASE_NOTIFY_TYPE NotifyType, EFI_HANDLE Handle)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	CHAR16 *filenameFormat = NULL;
	CHAR16 filename[50];
	VOID *formSetData = NULL;
	UINTN length = 0;
	
	SETUP_DEBUG_UEFI_NOTIFICATION( "\n\n[TSE] Entering ProcessPackToFile \n\n");
	

	if(NotifyType == EFI_HII_DATABASE_NOTIFY_REMOVE_PACK)
		filenameFormat = L"REMOVED_0x%x_%03d.hpk" ;

	else if(NotifyType == EFI_HII_DATABASE_NOTIFY_NEW_PACK)
		filenameFormat = L"NEW_0x%x_%03d.hpk" ;

	else if(NotifyType == EFI_HII_DATABASE_NOTIFY_ADD_PACK)
		filenameFormat = L"UPDATE_0x%x_%03d.hpk" ;

	else
		goto DONE ;


	formSetData = (UINT8*)HiiGetPackList(Handle, 0, &length);

	if (formSetData == NULL)
	{
	   	goto DONE;
 	}

	SPrint(filename, 50, filenameFormat, Handle, HpkFileCount);

	Status = WriteDataToFile(filename, formSetData, length, 0) ;
	
	if (!EFI_ERROR (Status))
	{
		HpkFileCount++ ;
	}

DONE:
	
	SETUP_DEBUG_UEFI_NOTIFICATION( "\n\n[TSE] Exiting ProcessPackToFile, Status: %x \n\n", Status);
}
/**
    Writes Hpk data to file

    @param filenameFormat EFI_HANDLE Handle, UINT8 *PackData, UINTN length
			

    @retval VOID
**/
EFI_STATUS WritePackToFile(CHAR16 *filenameFormat, EFI_HANDLE Handle, UINT8 *PackData, UINTN length)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	//EFI_FILE_PROTOCOL *FileHandle=NULL;
	UINT8 *formSet = PackData;
	UINTN len = length;
	CHAR16 filename[50];

	SETUP_DEBUG_UEFI_NOTIFICATION( "\n\n[TSE] Entering WritePackToFile \n\n");
	
	if(formSet == NULL)
  	{
    	formSet = (UINT8*)HiiGetPackList(Handle, 0, &len);
		if (formSet == NULL)
		{
	    	goto DONE;
 		}	
  	}

	SPrint(filename, 50, filenameFormat, Handle, HpkFileCount);

	Status = WriteDataToFile(filename, formSet, length,0) ;	
	
	if (!EFI_ERROR (Status))
	{
		HpkFileCount++ ;
	}
	
DONE:
	
	SETUP_DEBUG_UEFI_NOTIFICATION( "\n\n[TSE] Exiting WritePackToFile, Status: %x \n\n", Status);

	return Status ;
}
/**
    Prints the buffer, 16 characters max in a line

    @param bufSize Buffer length to print
    @param buffer Buffer to print

    @retval VOID
**/
VOID DebugShowBufferContent(UINTN bufSize, VOID *buffer)
{
    
 //#if TSE_DEBUG_MESSAGES == 2
    
    UINTN index = 0, j = 0;
	UINTN size = bufSize;
	UINT8 *temp = (UINT8 *)buffer;

    for(index = 0; index < size; ){
		SETUP_DEBUG_UEFI_CALLBACK ( "[0x%08x (%04d)]  ", index, index);
        for(j = 0; (j < 16) && (temp != NULL); index++, j++){
			if(index >= size){
				break;
			}
            if(j==8) SETUP_DEBUG_UEFI_CALLBACK ( "- ");
			SETUP_DEBUG_UEFI_CALLBACK ( "%02x ", (UINT8)temp[index]);
		}
		SETUP_DEBUG_UEFI_CALLBACK ("\n");
	}

	
// #endif

}
/**
    Prints the AMI Setup Lib control states

    @param VOID

    @retval VOID
**/
VOID DebugLibShowHiiControlState(UINT32 formID, UINT32 ctrlIndex, VOID *passedCtrlInfo)
{
	EFI_STATUS status = EFI_SUCCESS;
	CONTROL_INFO *ctrlInfo = (CONTROL_INFO *)NULL;
	UINTN ctrlCond = COND_NONE;
	
	ctrlInfo = (CONTROL_INFO *)passedCtrlInfo;
	if(passedCtrlInfo == NULL){
		//status = _GetControlInfo(formID, ctrlIndex, &ctrlInfo);
		//if((ctrlInfo == NULL) || EFI_ERROR(status)){
			SETUP_DEBUG_TSE("Invalid ctrlInfo, formID: 0x%x, ctrlIndex: 0x%x, status: 0x%x", formID, ctrlIndex, status);
			return;
		//}
	}
	ctrlCond = CheckControlCondition(ctrlInfo);
	
	switch(ctrlCond){
		case COND_NONE:
			SETUP_DEBUG_TSE("Control Condition: COND_NONE\n");
			break;
		case COND_SUPPRESS:
			SETUP_DEBUG_TSE("Control Condition: COND_SUPPRESS\n");
			break;
		case COND_HIDDEN:
			SETUP_DEBUG_TSE("Control Condition: COND_HIDDEN\n");
			break;
		case COND_GRAYOUT:
			SETUP_DEBUG_TSE("Control Condition: COND_GRAYOUT\n");
			break;
		case COND_INCONSISTENT:
			SETUP_DEBUG_TSE("Control Condition: COND_INCONSISTENT\n");
			break;
		default:
			break;
	}
	return;
}
/**
    Prints the cache controls information

    @param formID 
    @param count 
    @param passedCtrlInfo 
    @param detailed If TRUE, prints the control buffers

    @retval EFI_STATUS status - EFI_SUCCESS if successful, else EFI_ERROR
**/
EFI_STATUS DebugShowControlInfo(UINT32 formID, VOID *passedCtrlInfo)
{
	EFI_STATUS status = EFI_SUCCESS;
	//UINTN bufferLen = 0;
	//UINT32 i = 0, j = 0;
	//UINT8 *temp = (UINT8 *)NULL;
	//EFI_IFR_QUESTION_HEADER *questionHdr = NULL;
	//UINT8 *questionValue = (UINT8 *)NULL;
	//UINTN dataWidth = 0;
	CONTROL_INFO *ctrlInfo = (CONTROL_INFO *)NULL;
	CHAR16 *String = (CHAR16 *)NULL;

	ctrlInfo = (CONTROL_INFO *)passedCtrlInfo;
	
    if(passedCtrlInfo == NULL){
		//status = _GetControlInfo(formID, ctrlIndex, &ctrlInfo);
        goto DONE ;		
	}

	if((ctrlInfo == NULL) || EFI_ERROR(status)){
		SETUP_DEBUG_TSE("Invalid ctrlInfo, formID: 0x%x, status: 0x%x", formID, status);
        goto DONE ;		
	}
    
    SETUP_DEBUG_TSE("\n----- Adding Control -----\n");  

    if(ctrlInfo->ControlHandle != NULL){
		String = HiiGetString(ctrlInfo->ControlHandle, ctrlInfo->QuestionId);
	}
	
    if (String)
    {
	    SETUP_DEBUG_TSE("QuestionId: 0x%x, %s\n", ctrlInfo->QuestionId, String);
    }
	SETUP_DEBUG_TSE("ControlHandle: 0x%x\n", ctrlInfo->ControlHandle);
    SETUP_DEBUG_TSE("ControlType: 0x%x\n", ctrlInfo->ControlType);
	SETUP_DEBUG_TSE("ControlPageID: %d\n", ctrlInfo->ControlPageID);
    SETUP_DEBUG_TSE("ControlDestPageID: %d\n", ctrlInfo->ControlDestPageID);
	SETUP_DEBUG_TSE("ControlIndex: %d\n", ctrlInfo->ControlIndex);
	SETUP_DEBUG_TSE("ControlPtr: 0x%x\n", ctrlInfo->ControlPtr);
    SETUP_DEBUG_TSE("ControlConditionalPtr: 0x%x\n", ctrlInfo->ControlConditionalPtr);
	SETUP_DEBUG_TSE("ControlDataLength: 0x%x\n", ctrlInfo->ControlDataLength);
    SETUP_DEBUG_TSE("ControlDataWidth: 0x%x\n", ctrlInfo->ControlDataWidth);
    SETUP_DEBUG_TSE("ControlKey: %d\n", ctrlInfo->ControlKey);
    SETUP_DEBUG_TSE("DestQuestionID: 0x%x\n", &ctrlInfo->DestQuestionID);
    //DebugLibShowHiiControlState(formID, ctrlInfo->ControlIndex, ctrlInfo);
	//DebugShowControlFlags(formID, ctrlIndex, ctrlInfo, &ctrlInfo->ControlFlags);


DONE:
	MemFreePointer( (VOID **)&String );
	
	return status;
}

/**
    Shows the Page information

    @param formID The form 
    @param passedPageInfo 

    @retval VOID
**/
VOID DebugShowPageInfo(UINT32 formID, VOID *passedPageInfo)
{
	//EFI_STATUS status = EFI_SUCCESS;
	PAGE_INFO *pageInfo = (PAGE_INFO *)NULL;
	CHAR16 *String = (CHAR16 *)NULL;
	
	pageInfo = (PAGE_INFO *)passedPageInfo;

    if(pageInfo == NULL)
        return ;	
    
	SETUP_DEBUG_TSE("\n+++++ Adding Page +++++\n");
	if(NULL != pageInfo->PageHandle)
		String = HiiGetString(pageInfo->PageHandle, pageInfo->PageSubTitle );
	if(String)
  	  SETUP_DEBUG_TSE("PageTitle: %s \n",String); //Varies	Allows a page to override the main title bar
    //SETUP_DEBUG_TSE("PageSubTitle: %s \n", HiiGetString(pageInfo->PageHandle, pageInfo->PageSubTitle )); //Varies	Token for the subtitle string for this page
	SETUP_DEBUG_TSE("PageHandle: 0x%x\n", pageInfo->PageHandle);
	SETUP_DEBUG_TSE("PageIdIndex: %d\n", pageInfo->PageIdIndex); //varies	Index in to PageIdList triplets
	SETUP_DEBUG_TSE("PageFormID: %d\n", pageInfo->PageFormID); //Varies	Form ID within the formset for this page
	SETUP_DEBUG_TSE("PageID: %d\n", pageInfo->PageID); //Varies	unique ID for this page
	SETUP_DEBUG_TSE("PageParentID: %d\n", pageInfo->PageParentID); //Varies	Identify this page's parent page
	SETUP_DEBUG_TSE("PageFlags: 0x%x\n", pageInfo->PageFlags); //3			Various attributes for a specific page
	SETUP_DEBUG_TSE("PageVariable: 0x%x\n", pageInfo->PageVariable); //Varies	Overrides the variable ID for this page (0 = use default)
    SETUP_DEBUG_TSE("PageDynamic: 0x%x\n", pageInfo->PageFlags.PageDynamic); //Identify this page as dynamic page
    MemFreePointer( (VOID **)&String );

}
/**
    Prints the variable information for the given parameter

    @param formID 

    @retval EFI_STATUS status - EFI_SUCCESS if successful, else EFI_ERROR
**/
EFI_STATUS DebugShowHiiVariable(UINT32 formID, UINT32 varIndex, UINT32 *passedVarIDList)
{
	EFI_STATUS status = EFI_SUCCESS;
	NVRAM_VARIABLE *nvPtr = (NVRAM_VARIABLE *)NULL;
	//UINT32 *varIDList = (UINT32 *)NULL;
    VARIABLE_INFO *varInfo = (VARIABLE_INFO *)NULL ;
    UINTN index = 0, j = 0, size = 0 ;
    UINT8 *temp = (UINT8 *)NULL ;

	nvPtr = gVariableList;
	size = nvPtr[varIndex].Size < 0xFFFF ? nvPtr[varIndex].Size : 0;
	temp = nvPtr[varIndex].Buffer;
    varInfo = VarGetVariableInfoIndex( varIndex );

	if(size){
        SETUP_DEBUG_VAR ("\n----------- DebugShowHiiVariable -----------\n");
		SETUP_DEBUG_VAR ("VariableName: %s, VariableHandle: 0x%x, Buffer size: %d, Buffer: 0x%x \n", varInfo->VariableName, varInfo->VariableHandle, size, nvPtr[varIndex].Buffer); 
                
        for(index = 0; index < size; ){
			SETUP_DEBUG_VAR ( "[0x%08x (%04d)]  ", index, index);
            for(j = 0; (j < 16) && (temp != NULL); index++, j++){
				if(index >= size){
					break;
				}
                if(j==8) SETUP_DEBUG_VAR ( "- ");
				SETUP_DEBUG_VAR ( "%02x ", (UINT8)temp[index]);
			}
			SETUP_DEBUG_VAR ("\n");
		}

        SETUP_DEBUG_VAR ("----------- End of DebugShowHiiVariable, status: 0x%x -----------\n\n", status); 
	} else{
        SETUP_DEBUG_VAR ("!!!!!! VariableName: %s, size: %d !!!!!\n", varInfo->VariableName, nvPtr[varIndex].Size); 
    }

	return status;
}
/**
    Prints the variable information in the list

    @param formID 

    @retval EFI_STATUS status - EFI_SUCCESS if successful, else EFI_ERROR
**/
EFI_STATUS DebugShowAllHiiVariable(UINT32 formID)
{
	EFI_STATUS status = EFI_SUCCESS;
	//NVRAM_VARIABLE *nvPtr = (NVRAM_VARIABLE *)NULL;
	UINT32 i = 0;
	UINT32 *varIDList = (UINT32*)EfiLibAllocateZeroPool(gVariables->VariableCount * sizeof(UINT32));

	SETUP_DEBUG_VAR ("\n----------- DebugShowAllHiiVariable -----------\n");  

	if(varIDList == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		SETUP_DEBUG_VAR ("ERROR-%r: varIDList Memory Allocation Failed\n", status);
		goto DONE;
	}

	//nvPtr = gVariableList;

	SETUP_DEBUG_VAR ("++ Total number of variables: %d ++\n\n", gVariables->VariableCount);
	for ( i = 0; i < gVariables->VariableCount; i++){
		DebugShowHiiVariable(formID, i, varIDList);		
	}
	SETUP_DEBUG_VAR ("\n");

DONE:
	
    SETUP_DEBUG_VAR ("--------- End of DebugShowAllHiiVariable, status: 0x%x ---------\n\n", status); 
	return status;
}
/**
    Prints the current FormSet data

    @param FormID Library formID of a FormSet

    @retval EFI_STATUS status - EFI_SUCCESS if successful, else EFI_ERROR
**/
EFI_STATUS DisplayParseFormSetData(PAGE_INFO *PageInfo)
{
   	
    EFI_STATUS status = EFI_SUCCESS;
	PAGE_INFO *pageInfo = (PAGE_INFO *)PageInfo;
	EFI_HII_PACKAGE_HEADER *pkgHdr = (EFI_HII_PACKAGE_HEADER *)NULL;
	UINT8 *formSet = (UINT8 *)NULL;
	UINT8 *ifrData = (UINT8 *)NULL;
	EFI_IFR_OP_HEADER *opHeader = (EFI_IFR_OP_HEADER *)NULL;
	UINTN length = 0;
	UINTN i = 0, count = 0;
	CHAR16 *opcodeName = (CHAR16 *)NULL;
	//EFI_GUID *tempFormSetGuid = (EFI_GUID *)NULL;
	BOOLEAN validOpCode = TRUE;
	UINTN pScopeCount = 0;
    BOOLEAN CurrentForm = FALSE;
    UINT16 questionId = 0;
    FRAME_DATA *frame = NULL ;
    CHAR16 *Text=NULL;
    //UINT16	PageFormID = 0;
    
    if(pageInfo == NULL){
        status = EFI_UNSUPPORTED;
        SETUP_DEBUG_TSE ("ERROR: 0x%x, failed to get PageInfo data\n",status);
        goto DONE;
    }
    
    formSet = (UINT8*)HiiGetForm(pageInfo->PageHandle, 0, &length);
	if(formSet == NULL){
		status = EFI_UNSUPPORTED;
        SETUP_DEBUG_TSE ("ERROR: 0x%x, failed to get FormSet data\n",status);
		goto DONE;
	}

	pkgHdr = (EFI_HII_PACKAGE_HEADER*)formSet;

	if(pkgHdr->Type != EFI_HII_PACKAGE_FORMS)
	{
		status = EFI_UNSUPPORTED;
		SETUP_DEBUG_TSE("ERROR: 0x%x, pkgHdr->Type is not supported, Since it is not EFI_HII_PACKAGE_FORM\n",status);
		goto DONE;
	}


	ifrData = ((UINT8 *)pkgHdr) + sizeof(EFI_HII_PACKAGE_HEADER);

	while(i < pkgHdr->Length)
	{
		opHeader = (EFI_IFR_OP_HEADER*)(ifrData + i);
		//tempFormSetGuid = (EFI_GUID *)NULL;

		if(opHeader == NULL){
			break;
		}

		count = pScopeCount;
        questionId = 0;

		switch(opHeader->OpCode)
		{
			case EFI_IFR_FORM_OP: //0x01 - Form
				opcodeName = L"EFI_IFR_FORM_OP";

                if(CurrentForm)
                {
                    SETUP_DEBUG_TSE (  "[TSE] Form OpCode End\n" );
                    goto DONE;
                 }

                if(((EFI_IFR_FORM*)(ifrData + i))->FormId == pageInfo->PageFormID)
                {
                   SETUP_DEBUG_TSE (  "[TSE] Form OpCode Start\n" );  
                   Text = HiiGetString(pageInfo->PageHandle, pageInfo->PageSubTitle);
                   SETUP_DEBUG_TSE (  "FormTitle: %s\n", Text);
                   MemFreePointer( (VOID **)&Text );
                   SETUP_DEBUG_TSE (  "Handle: 0x%x\n", pageInfo->PageHandle);
                   
                   CurrentForm = TRUE;
                }

				break;
			case EFI_IFR_SUBTITLE_OP: //0x02 - Subtitle statement
				opcodeName = L"EFI_IFR_SUBTITLE_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_TEXT_OP: //0x03 - Static text/image statement
				opcodeName = L"EFI_IFR_TEXT_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_IMAGE_OP: //0x04 - Static image.
				opcodeName = L"EFI_IFR_IMAGE_OP";
				break;
			case EFI_IFR_ONE_OF_OP: //0x05 - One-of question
				opcodeName = L"EFI_IFR_ONE_OF_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_CHECKBOX_OP: //0x06 - Boolean question
				opcodeName = L"EFI_IFR_CHECKBOX_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_NUMERIC_OP: //0x07 - Numeric question
				opcodeName = L"EFI_IFR_NUMERIC_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_PASSWORD_OP: //0x08 - Password string question
				opcodeName = L"EFI_IFR_PASSWORD_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_ONE_OF_OPTION_OP: //0x09 - Option
				opcodeName = L"EFI_IFR_ONE_OF_OPTION_OP";
                /*
                switch(((EFI_IFR_ONE_OF_OPTION *)opHeader)->Type)
				{
				    case EFI_IFR_TYPE_NUM_SIZE_16:
					    questionId = ((EFI_IFR_ONE_OF_OPTION *)opHeader)->Value.u16;
					    break;
				    case EFI_IFR_TYPE_NUM_SIZE_32:    
					    questionId = ((EFI_IFR_ONE_OF_OPTION *)opHeader)->Value.u32;
					    break;
				    case EFI_IFR_TYPE_NUM_SIZE_64:
					    questionId = (UINT16)((EFI_IFR_ONE_OF_OPTION *)opHeader)->Value.u64;
					    break;
				    case EFI_IFR_TYPE_NUM_SIZE_8:
				    default:
					    questionId = ((EFI_IFR_ONE_OF_OPTION *)opHeader)->Value.u8;
					    break;
				}
                */       
				break;
			case EFI_IFR_SUPPRESS_IF_OP: //0x0A - Suppress if conditional
				opcodeName = L"EFI_IFR_SUPPRESS_IF_OP";
				break;
			case EFI_IFR_LOCKED_OP: //0x0B - Marks statement/question as locked
				opcodeName = L"EFI_IFR_LOCKED_OP";
				break;
			case EFI_IFR_ACTION_OP: //0x0C - Button question
				opcodeName = L"EFI_IFR_ACTION_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_RESET_BUTTON_OP: //0x0D - Reset button statement
				opcodeName = L"EFI_IFR_RESET_BUTTON_OP";
				break;
			case EFI_IFR_FORM_SET_OP: //0x0E - Form set
				opcodeName = L"EFI_IFR_FORM_SET_OP";
				break;
			case EFI_IFR_REF_OP: //0x0F - Cross-reference statement
				opcodeName = L"EFI_IFR_REF_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_NO_SUBMIT_IF_OP: //0x10 - Error checking conditional
				opcodeName = L"EFI_IFR_NO_SUBMIT_IF_OP";
				break;
			case EFI_IFR_INCONSISTENT_IF_OP: //0x11 - Error checking conditional
				opcodeName = L"EFI_IFR_INCONSISTENT_IF_OP";
				break;
			case EFI_IFR_EQ_ID_VAL_OP: //0x12 - Return true if question value equals UINT16
				opcodeName = L"EFI_IFR_EQ_ID_VAL_OP";
				break;
			case EFI_IFR_EQ_ID_ID_OP: //0x13 - Return true if question value equals another question value
				opcodeName = L"EFI_IFR_EQ_ID_ID_OP";
				break;
			case 0x14: //0x14 - Return true if question value is found in list of UINT16s
				opcodeName = L"EFI_IFR_EQ_ID_VAL_LIST_OP";
				break;
			case EFI_IFR_AND_OP: //0x15 - Push true if both sub-expressions returns true.
				opcodeName = L"EFI_IFR_AND_OP";
				break;
			case EFI_IFR_OR_OP: //0x16 - Push true if either sub-expressions returns true.
				opcodeName = L"EFI_IFR_OR_OP";
				break;
			case EFI_IFR_NOT_OP: //0x17 - Push false if sub-expression returns true, otherwise return true.
				opcodeName = L"EFI_IFR_NOT_OP";
				break;
			case EFI_IFR_RULE_OP: //0x18 - Create rule in current form.
				opcodeName = L"EFI_IFR_RULE_OP";
				break;
			case EFI_IFR_GRAY_OUT_IF_OP: //0x19 - Nested statements, questions or options will not be selectable if expression returns true.
				opcodeName = L"EFI_IFR_GRAY_OUT_IF_OP";
				break;
			case EFI_IFR_DATE_OP: //0x1A - Date question.
				opcodeName = L"EFI_IFR_DATE_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);    
				break;
			case EFI_IFR_TIME_OP: //0x1B - Time question.
				opcodeName = L"EFI_IFR_TIME_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_STRING_OP: //0x1C - String question
				opcodeName = L"EFI_IFR_STRING_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_REFRESH_OP: //0x1D - Interval for refreshing a question
				opcodeName = L"EFI_IFR_REFRESH_OP";
				break;
			case EFI_IFR_DISABLE_IF_OP: //0x1E - Nested statements, questions or options will not be processed if expression returns true.
				opcodeName = L"EFI_IFR_DISABLE_IF_OP";
				break;
			case 0x1F: //0x1F - Animation associated with question statement, form or form set.
				opcodeName = L"EFI_IFR_ANIMATION_OP";
				break;
			case EFI_IFR_TO_LOWER_OP: //0x20 - Convert a string on the expression stack to lower case.
				opcodeName = L"EFI_IFR_TO_LOWER_OP";
				break;
			case EFI_IFR_TO_UPPER_OP: //0x21 - Convert a string on the expression stack toupper case.
				opcodeName = L"EFI_IFR_TO_UPPER_OP";
				break;
			case 0x22: //0x22 - Convert one value to another by selecting a match from a list.
				opcodeName = L"EFI_IFR_MAP_OP";
				break;
			case EFI_IFR_ORDERED_LIST_OP: //0x23 - Set question
				opcodeName = L"EFI_IFR_ORDERED_LIST_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_VARSTORE_OP: //0x24 - Define a buffer-style variable storage.
				opcodeName = L"EFI_IFR_VARSTORE_OP";
				break;
			case EFI_IFR_VARSTORE_NAME_VALUE_OP: //0x25 - Define a name/value style variable storage.
				opcodeName = L"EFI_IFR_VARSTORE_NAME_VALUE_OP";
				break;
			case EFI_IFR_VARSTORE_EFI_OP: //0x26 - Define a UEFI variable style variable storage.
				opcodeName = L"EFI_IFR_VARSTORE_EFI_OP";
				break;
			case EFI_IFR_VARSTORE_DEVICE_OP: //0x27 - Specify the device path to use for variable storage.
				opcodeName = L"EFI_IFR_VARSTORE_DEVICE_OP";
				break;
			case EFI_IFR_VERSION_OP: //0x28 - Push the revision level of the UEFI Specification to which this FormsProcessor is compliant.
				opcodeName = L"EFI_IFR_VERSION_OP";
				break;
			case EFI_IFR_END_OP: //0x29 - Marks end of scope.
				opcodeName = L"EFI_IFR_END_OP";
				break;
			case EFI_IFR_MATCH_OP: //0x2A - Push TRUE if string matches a pattern.
				opcodeName = L"EFI_IFR_MATCH_OP";
				break;
			case 0x2B: //0x2B - Return a stored value.
				opcodeName = L"EFI_IFR_GET_OP";
				break;
			case 0x2C: //0x2C - Change a stored value.
				opcodeName = L"EFI_IFR_SET_OP";
				break;
			case 0x2D: //0x2D - Provides a value for the current question or default.
				opcodeName = L"EFI_IFR_READ_OP";
				break;
			case 0x2E: //0x2E - Change a value for the current question.
				opcodeName = L"EFI_IFR_WRITE_OP";
				break;
			case EFI_IFR_EQUAL_OP: //0x2F - Push TRUE if two expressions are equal.
				opcodeName = L"EFI_IFR_EQUAL_OP";
				break;
			case EFI_IFR_NOT_EQUAL_OP: //0x30 - Push TRUE if two expressions are not equal.
				opcodeName = L"EFI_IFR_NOT_EQUAL_OP";
				break;
			case EFI_IFR_GREATER_THAN_OP: //0x31 - Push TRUE if one expression is greater than another expression.
				opcodeName = L"EFI_IFR_GREATER_THAN_OP";
				break;
			case EFI_IFR_GREATER_EQUAL_OP: //0x32 - Push TRUE if one expression is greater than or equal to another expression.
				opcodeName = L"EFI_IFR_GREATER_EQUAL_OP";
				break;
			case EFI_IFR_LESS_THAN_OP: //0x33 - Push TRUE if one expression is less than another expression.
				opcodeName = L"EFI_IFR_LESS_THAN_OP";
				break;
			case EFI_IFR_LESS_EQUAL_OP: //0x34 - Push TRUE if one expression is less than or equal to another expression.
				opcodeName = L"EFI_IFR_LESS_EQUAL_OP";
				break;
			case EFI_IFR_BITWISE_AND_OP: //0x35 - Bitwise-AND two unsigned integers and push the result.
				opcodeName = L"EFI_IFR_BITWISE_AND_OP";
				break;
			case EFI_IFR_BITWISE_OR_OP: //0x36 - Bitwise-OR two unsigned integers and push the result.
				opcodeName = L"EFI_IFR_BITWISE_OR_OP";
				break;
			case EFI_IFR_BITWISE_NOT_OP: //0x37 - Bitwise-NOT an unsigned integer and push the result.
				opcodeName = L"EFI_IFR_BITWISE_NOT_OP";
				break;
			case EFI_IFR_SHIFT_LEFT_OP: //0x38 - Shift an unsigned integer left by a number of bits and push the result.
				opcodeName = L"EFI_IFR_SHIFT_LEFT_OP";
				break;
			case EFI_IFR_SHIFT_RIGHT_OP: //0x39 - Shift an unsigned integer right by a number of bits and push the result.
				opcodeName = L"EFI_IFR_SHIFT_RIGHT_OP";
				break;
			case EFI_IFR_ADD_OP: //0x3A - Add two unsigned integers and push the result.
				opcodeName = L"EFI_IFR_ADD_OP";
				break;
			case EFI_IFR_SUBTRACT_OP: //0x3B - Subtract two unsigned integers and push the result.
				opcodeName = L"EFI_IFR_SUBTRACT_OP";
				break;
			case EFI_IFR_MULTIPLY_OP: //0x3C - Multiply two unsigned integers and push the result.
				opcodeName = L"EFI_IFR_MULTIPLY_OP";
				break;
			case EFI_IFR_DIVIDE_OP: //0x3D - Divide one unsigned integer by another and push the result.
				opcodeName = L"EFI_IFR_DIVIDE_OP";
				break;
			case EFI_IFR_MODULO_OP: //0x3E - Divide one unsigned integer by another and push the remainder.
				opcodeName = L"EFI_IFR_MODULO_OP";
				break;
			case EFI_IFR_RULE_REF_OP: //0x3F //Evaluate a rule
				opcodeName = L"EFI_IFR_RULE_REF_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_QUESTION_REF1_OP: //0x40 - Push a question’s value
				opcodeName = L"EFI_IFR_QUESTION_REF1_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_QUESTION_REF2_OP: //0x41 - Push a question’s value
				opcodeName = L"EFI_IFR_QUESTION_REF2_OP";
                questionId = _GetQuestionToken((UINT8*)opHeader);
				break;
			case EFI_IFR_UINT8_OP: //0x42 - Push an 8-bit unsigned integer
				opcodeName = L"EFI_IFR_UINT8_OP";
				break;
			case EFI_IFR_UINT16_OP: //0x43 - Push a 16-bit unsigned integer.
				opcodeName = L"EFI_IFR_UINT16_OP";
				break;
			case EFI_IFR_UINT32_OP: //0x44 - Push a 32-bit unsigned integer
				opcodeName = L"EFI_IFR_UINT32_OP";
				break;
			case EFI_IFR_UINT64_OP: //0x45 - Push a 64-bit unsigned integer.
				opcodeName = L"EFI_IFR_UINT64_OP";
				break;
			case EFI_IFR_TRUE_OP: //0x46 - Push a boolean TRUE.
				opcodeName = L"EFI_IFR_TRUE_OP";
				break;
			case EFI_IFR_FALSE_OP: //0x47 - Push a boolean FALSE
				opcodeName = L"EFI_IFR_FALSE_OP";
				break;
			case EFI_IFR_TO_UINT_OP: //0x48 - Convert expression to an unsigned integer
				opcodeName = L"EFI_IFR_TO_UINT_OP";
				break;
			case EFI_IFR_TO_STRING_OP: //0x49 - Convert expression to a string
				opcodeName = L"EFI_IFR_TO_STRING_OP";
				break;
			case EFI_IFR_TO_BOOLEAN_OP: //0x4A - Convert expression to a boolean.
				opcodeName = L"EFI_IFR_TO_BOOLEAN_OP";
				break;
			case EFI_IFR_MID_OP: //0x4B - Extract portion of string or buffer
				opcodeName = L"EFI_IFR_MID_OP";
				break;
			case EFI_IFR_FIND_OP: //0x4C - Find a string in a string.
				opcodeName = L"EFI_IFR_FIND_OP";
				break;
			case EFI_IFR_TOKEN_OP: //0x4D - Extract a delimited byte or character string from buffer or string.
				opcodeName = L"EFI_IFR_TOKEN_OP";
				break;
			case EFI_IFR_STRING_REF1_OP: //0x4E - Push a string
				opcodeName = L"EFI_IFR_STRING_REF1_OP";
				break;
			case EFI_IFR_STRING_REF2_OP: //0x4F - Push a string
				opcodeName = L"EFI_IFR_STRING_REF2_OP";
				break;
			case EFI_IFR_CONDITIONAL_OP: //0x50 - Duplicate one of two expressions depending on result of the first expression.
				opcodeName = L"EFI_IFR_CONDITIONAL_OP";
				break;
			case EFI_IFR_QUESTION_REF3_OP: //0x51 - Push a question’s value from a different form.
				opcodeName = L"EFI_IFR_QUESTION_REF3_OP";
				break;
			case EFI_IFR_ZERO_OP: //0x52 - Push a zero
				opcodeName = L"EFI_IFR_ZERO_OP";
				break;
			case EFI_IFR_ONE_OP: //0x53 //Push a one
				opcodeName = L"EFI_IFR_ONE_OP";
				break;
			case EFI_IFR_ONES_OP: //0x54 - Push a 0xFFFFFFFFFFFFFFFF.
				opcodeName = L"EFI_IFR_ONES_OP";
				break;
			case EFI_IFR_UNDEFINED_OP: //0x55 - Push Undefined
				opcodeName = L"EFI_IFR_UNDEFINED_OP";
				break;
			case EFI_IFR_LENGTH_OP: //0x56 - Push length of buffer or string.
				opcodeName = L"EFI_IFR_LENGTH_OP";
				break;
			case EFI_IFR_DUP_OP: //0x57 - Duplicate top of expression stack
				opcodeName = L"EFI_IFR_DUP_OP";
				break;
			case EFI_IFR_THIS_OP: //0x58 - Push the current question’s value
				opcodeName = L"EFI_IFR_THIS_OP";
				break;
			case EFI_IFR_SPAN_OP: //0x59 - Return first matching/non-matching character in a string
				opcodeName = L"EFI_IFR_SPAN_OP";
				break;
			case EFI_IFR_VALUE_OP: //0x5A - Provide a value for a question
				opcodeName = L"EFI_IFR_VALUE_OP";
				break;
			case EFI_IFR_DEFAULT_OP: //0x5B - Provide a default value for a question.
				opcodeName = L"EFI_IFR_DEFAULT_OP";
				break;
			case 0x5C: //0x5C - Define a Default Type Declaration
				opcodeName = L"EFI_IFR_DEFAULTSTORE_OP";
				break;
			case 0x5D: //0x5D - Create a standards-map form.
				opcodeName = L"EFI_IFR_FORM_MAP_OP";
				break;
			case EFI_IFR_CATENATE_OP: //0x5E - Push concatenated buffers or strings.
				opcodeName = L"EFI_IFR_CATENATE_OP";
				break;
			case EFI_IFR_GUID_OP: //0x5F - An extensible GUIDed op-code
				opcodeName = L"EFI_IFR_GUID_OP";
				break;
			case 0x60: //0x60 - Returns whether current user profile contains specified setup access privileges.
				opcodeName = L"EFI_IFR_SECURITY_OP";
				break;
			case 0x61: //0x61 - Specify current form is modal
				opcodeName = L"EFI_IFR_MODAL_TAG_OP";
				break;
			case 0x62: //0x62 - Establish an event group for refreshing a forms-based element.
				opcodeName = L"EFI_IFR_REFRESH_ID_OP";
				break;
			case 0x63:// Support to display warning message
				opcodeName = L"TSE_EFI_IFR_WARNING_IF_OP";
				break;
			case EFI_IFR_MATCH2_OP://Support to display match2
				opcodeName = L"EFI_IFR_MATCH2_OP";
				break;
			default:
				opcodeName = L"Invalid Opcode";
				validOpCode = FALSE;
				break;
		}

		if(opHeader->OpCode == EFI_IFR_END_OP){
			pScopeCount--;
			count = pScopeCount;
		}
		while(count){
			if(CurrentForm) SETUP_DEBUG_TSE (  "    ");
                    
			count--;
		}

		if(CurrentForm) SETUP_DEBUG_TSE (  "\n\n%s, len: %d\n", opcodeName, opHeader->Length);
        
        if(CurrentForm && questionId) 
        {
        	Text = HiiGetString(pageInfo->PageHandle , questionId  );
        	SETUP_DEBUG_TSE (  "StringId: 0x%x  %s \n", questionId,Text );  
        	MemFreePointer( (VOID **)&Text );
        }
        
        if(opHeader->Length==0)
        {
            //validOpCode = FALSE ;
            goto DONE;
        }

		
        if(validOpCode){
			UINTN index = 0, j = 0;
			UINTN size = opHeader->Length;
			UINT8 *temp = (UINT8 *)opHeader;
			UINTN sCount = pScopeCount;

			if(size){		
				for(index = 0; index < size; ){
					sCount = pScopeCount;
    				while(sCount){
						sCount--;
					}
                    
                    if(CurrentForm) SETUP_DEBUG_TSE ( "[0x%08x (%04d)]  ", index, index);

					for(j = 0; (j < 16) && (temp != NULL); index++, j++)
                    {
						if(index >= size){
							break;
						}
                        if(CurrentForm && j==8) SETUP_DEBUG_TSE ( "- ");
            			if(CurrentForm) SETUP_DEBUG_TSE ( "%02x ", (UINT8)temp[index]);
					}
					if(CurrentForm)SETUP_DEBUG_TSE ("\n");

				}
			}
		}

		if(opHeader->Scope){
			pScopeCount++;
		}
		
		i += opHeader->Length;
		validOpCode = TRUE;
	}

DONE:
	if(formSet){
		EfiLibSafeFreePool(formSet);
	}

	pScopeCount = 0;

	return status;
}

#endif //End of TSE_DEBUG_MESSAGES
#define ASSERT_ERROR_STATUS(assertion)		if(EFI_ERROR(assertion)) goto DONE
#define MAX_PAGES_TO_PROCESS				50
UINT32 gProcessedPages[MAX_PAGES_TO_PROCESS];
UINT32 gProPageOffset = 0;
UINT32 gCurrLoadVarIndex = 0; //Used to track which variable is getting loaded
UINT8 *gCurrLoadVarBuf = (UINT8 *)NULL; //Used to store current variable buffer

/**
    Returns pointer to the CONTROL_INFO struct of given CtrlIndex

    @param PgIndex Page index containing the control
    @param CtrlIndex HII Control index
    @param CtrlInfo Pointer to the CONTROL_INFO structure

    @retval EFI_STATUS status - EFI_SUCCESS if successful, else EFI_ERROR
**/
EFI_STATUS _GetPageInfo(UINT32 PgIndex, PAGE_INFO **PageInfo);
EFI_STATUS _GetControlInfo(UINT32 PgIndex, UINT32 CtrlIndex, CONTROL_INFO **CtrlInfo)
{
  EFI_STATUS status = EFI_SUCCESS;
  PAGE_INFO *pageInfo = NULL;

  status = _GetPageInfo(PgIndex, &pageInfo);
  if(pageInfo && pageInfo->PageHandle == NULL)
  {
    status = EFI_NOT_FOUND; //If the page handle is invalid, then page may contain incorrect control data. Return error.
  }
  ASSERT_ERROR_STATUS(status);

  if(CtrlIndex > pageInfo->PageControls.ControlCount)
  {
    status = EFI_INVALID_PARAMETER;
    goto DONE;
  }

  *CtrlInfo = (CONTROL_INFO*)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[CtrlIndex]);

DONE:
  return status;
}

/**
    Get the Number of Control for a Page

    @param PgIndex 
    @param numOfControls 

    @retval EFI_STATUS 
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS GetNumOfControls(UINT32 PgIndex, UINT32 *NumOfControls)
{
  EFI_STATUS status = EFI_SUCCESS;
  PAGE_INFO *pageInfo = NULL;

  status = _GetPageInfo(PgIndex, &pageInfo);
  ASSERT_ERROR_STATUS(status);

  *NumOfControls = pageInfo->PageControls.ControlCount;

DONE:
  return status;
}

/**
    Cleans the temporary variables

    @param VOID

    @retval VOID
**/
VOID CleanCurrLoadVarBuffer()
{
    if(gCurrLoadVarBuf!= NULL) {
        MemFreePointer((VOID**)&gCurrLoadVarBuf);
    }
    gCurrLoadVarBuf = (UINT8 *)NULL;
    gCurrLoadVarIndex = 0;
}

/**
    Checks whether the page is already processed or not.

    @param PageID 

    @retval BOOLEAN
**/
BOOLEAN _DestinationPageProcessed(UINT32 PageID)
{
  BOOLEAN isProcessed = FALSE;
  UINT32 index = 0;

  for(index = 0; index < gProPageOffset; index++)
  {
    if(PageID == gProcessedPages[index])
    {
      isProcessed = TRUE;
      break;
    }
  }
  return isProcessed;
}

/**
    To get the default variable names.

    @param FormID 
    @param DefaultsFlag 
    @param ResetVal 
    @param ValChanged 

    @retval EFI_STATUS
**/
EFI_STATUS _GetDefaultNvramVariableIDList(UINT32 PageID, BOOLEAN DefaultsFlag, BOOLEAN *ResetVal, BOOLEAN *ValChanged)
{
  EFI_STATUS				status = EFI_SUCCESS;
  UINT16					CtrlDataSize = 0;
  UINT32					count = 0;
  UINT32					index = 0;
  UINTN						CurVarSize = 0;
  CONTROL_INFO				*ctrlInfo;
  NVRAM_VARIABLE			*nvPtr = gVariableList;
  NVRAM_VARIABLE			*defaults = gOptimalDefaults;
  EFI_IFR_QUESTION_HEADER	*questionHdr = (EFI_IFR_QUESTION_HEADER *)NULL;

  gProcessedPages[gProPageOffset++] = PageID;

  status = GetNumOfControls(PageID, &count);
  ASSERT_ERROR_STATUS(status);

  for(index = 0; index < count; index++)
  {
    //Iterating for all controls
    status = _GetControlInfo(PageID , index, &ctrlInfo);
    ASSERT_ERROR_STATUS(status);

    // CONTROL_TYPE_TEXT is a special case here since its value is not
    // in the varstore. If this is a text control, continue.
	if(ctrlInfo->ControlType == CONTROL_TYPE_TEXT){
	  continue;
	}

    if((ctrlInfo->ControlType == CONTROL_TYPE_SUBMENU) || (ctrlInfo->ControlType == CONTROL_TYPE_REF2))
    {
      if(_DestinationPageProcessed(ctrlInfo->ControlDestPageID) == FALSE)
      {
		status = _GetDefaultNvramVariableIDList(ctrlInfo->ControlDestPageID, DefaultsFlag, ResetVal, ValChanged);
        ASSERT_ERROR_STATUS(status);
      }
    }
    if(ctrlInfo->ControlType == CONTROL_TYPE_POPUP_STRING && DefaultsFlag)
    {
      UINT16 defaultToken = 0;
      CHAR16 * defaultValue = NULL;

      // First, check if the control has a default value. During parsing, the
      // Control Data Width is set to zero if there is no default value.
      if (ctrlInfo->ControlDataWidth == 0) {
        continue;       // move to next control if this one has no default. 
      }

      // Get the Default String Id
      defaultToken = *(UINT16*)((UINT8*)ctrlInfo + sizeof(CONTROL_INFO) + ctrlInfo->ControlDataWidth);
      // Get the string corresponding to the StringId
      defaultValue = HiiGetString(ctrlInfo->ControlHandle, defaultToken);
      // Set the default value to nvPtr
      questionHdr = (EFI_IFR_QUESTION_HEADER*)((UINT8*)ctrlInfo->ControlPtr + sizeof(EFI_IFR_OP_HEADER));
      if(defaultValue)
      {
        if(EfiStrCmp(defaultValue, (CHAR16 *)&nvPtr[ctrlInfo->ControlVariable].Buffer[questionHdr->VarStoreInfo.VarOffset]) != 0)
        {
          EfiStrCpy((CHAR16 *)&nvPtr[ctrlInfo->ControlVariable].Buffer[questionHdr->VarStoreInfo.VarOffset], defaultValue);
          *ValChanged = TRUE;

          if(ctrlInfo->ControlFlags.ControlReset)
          {
            //ControlReset reset requested.
            *ResetVal |= ctrlInfo->ControlFlags.ControlReset;
          }
        }
        MemFreePointer ((VOID **)&defaultValue);
      }
      continue;
    }

    if(ctrlInfo->ControlFlags.ControlEvaluateDefault && IsDefaultConditionalExpression())
    {
      UINT64 defaults = 0;
      UINT16 size = EvaluateControlDefault(ctrlInfo, &defaults);

      if(DefaultsFlag)
      {
        //Loading Defaults for control
        questionHdr = (EFI_IFR_QUESTION_HEADER*)((UINT8*)ctrlInfo->ControlPtr + sizeof(EFI_IFR_OP_HEADER));

        if(MemCmp((UINT8*)&nvPtr[ctrlInfo->ControlVariable].Buffer[questionHdr->VarStoreInfo.VarOffset], (UINT8*)&defaults, size) != 0)
        {
          //defaults and Memory value donot match. Copying defaults to memory value
          MemCopy( &nvPtr[ctrlInfo->ControlVariable].Buffer[questionHdr->VarStoreInfo.VarOffset], &defaults, size);
          *ValChanged = TRUE;

          if(ctrlInfo->ControlFlags.ControlReset)
          {
            //ControlReset reset requested
            *ResetVal |= ctrlInfo->ControlFlags.ControlReset;
          }
        }
		continue;
      }
    }

	if(DefaultsFlag) { //If loading defaults, use the data size given in CONTROL_INFO. Else get the data size using GetControlDataLength().
		CtrlDataSize = ctrlInfo->ControlDataWidth;
	}
	else {
		CtrlDataSize = (UINT16)GetControlDataLength(ctrlInfo);
	}

    if(CtrlDataSize == 0) {
     //Control Data Width for control %s is 0\n", HiiGetString(ctrlInfo->ControlHandle, ctrlInfo->QuestionId));
      continue;
    }

    if(ctrlInfo->ControlVariable <= gVariables->VariableCount)
    {
      if(DefaultsFlag == TRUE)
      {
        //Loading Defaults for control
        questionHdr = (EFI_IFR_QUESTION_HEADER*)((UINT8*)ctrlInfo->ControlPtr + sizeof(EFI_IFR_OP_HEADER));

        if(MemCmp((UINT8*)&nvPtr[ctrlInfo->ControlVariable].Buffer[questionHdr->VarStoreInfo.VarOffset], (UINT8*)&defaults[ctrlInfo->ControlVariable].Buffer[questionHdr->VarStoreInfo.VarOffset], ctrlInfo->ControlDataWidth) != 0)
        {
         //defaults and Memory value donot match. Copying defaults to memory value
          MemCopy( &nvPtr[ctrlInfo->ControlVariable].Buffer[questionHdr->VarStoreInfo.VarOffset], &defaults[ctrlInfo->ControlVariable].Buffer[questionHdr->VarStoreInfo.VarOffset], ctrlInfo->ControlDataWidth);
          *ValChanged = TRUE;

          if(ctrlInfo->ControlFlags.ControlReset)
          {
            //ControlReset reset requested.
            *ResetVal |= ctrlInfo->ControlFlags.ControlReset;
          }
        }
      }
      else
      {
        UINT8 *tempLoadVarBuff = (UINT8 *)NULL;

        if(gCurrLoadVarBuf != NULL)
        {
          if(gCurrLoadVarIndex != ctrlInfo->ControlVariable)
          {
            tempLoadVarBuff = VarGetNvram( ctrlInfo->ControlVariable, &CurVarSize );
            if(tempLoadVarBuff != NULL){
              CleanCurrLoadVarBuffer();
              gCurrLoadVarBuf = tempLoadVarBuff;
              gCurrLoadVarIndex = ctrlInfo->ControlVariable;
            }
          }
        }
        else
        {
          tempLoadVarBuff = VarGetNvram( ctrlInfo->ControlVariable, &CurVarSize );
          if(tempLoadVarBuff != NULL){
            CleanCurrLoadVarBuffer();
            gCurrLoadVarBuf = tempLoadVarBuff;
            gCurrLoadVarIndex = ctrlInfo->ControlVariable;
          }
        }

        if(gCurrLoadVarBuf != NULL)
        {
          if(tempLoadVarBuff != NULL){
            gCurrLoadVarIndex = ctrlInfo->ControlVariable;
          }
          questionHdr = (EFI_IFR_QUESTION_HEADER*)((UINT8*)ctrlInfo->ControlPtr + sizeof(EFI_IFR_OP_HEADER));
          MemCopy( &nvPtr[ctrlInfo->ControlVariable].Buffer[questionHdr->VarStoreInfo.VarOffset], &gCurrLoadVarBuf[questionHdr->VarStoreInfo.VarOffset], CtrlDataSize);
        }
      }
    }
  }

DONE:
  return status;
}

/**
    Intermediate function to get the nvram variables to restore.

    @param PageID 

    @retval EFI_STATUS
**/
EFI_STATUS LoadNvram(UINT32 PageID)
{
    EFI_STATUS status = EFI_SUCCESS;

    gProPageOffset = 0;
    MemSet(gProcessedPages, MAX_PAGES_TO_PROCESS * sizeof(UINT32), 0);
    status = _GetDefaultNvramVariableIDList(PageID, FALSE, NULL, NULL);
    ASSERT_ERROR_STATUS(status);

DONE:
    CleanCurrLoadVarBuffer();
    return status;
}

/**
    function to load the defaults to the nvram cache

    @param FormID form identifier
    @param ResetVal Reboot flag of the control variable
    @param ValChanged flag to check whether the value changed

    @retval EFI_STATUS
**/
EFI_STATUS LoadDefaults(UINT32 FormID, BOOLEAN *ResetVal, BOOLEAN * ValChanged)
{
    EFI_STATUS status = EFI_SUCCESS;
    UINT32 *varIDList = (UINT32*)EfiLibAllocateZeroPool(gVariables->VariableCount * sizeof(UINT32));


    if(varIDList == NULL)
    {
        status = EFI_OUT_OF_RESOURCES;

        goto DONE;
    }

    gVarOffset = 0;
//    gProFormOffset = 0;
//    MemSet(gProcessedForms, MAX_FORMS_TO_PROCESS, 0);
    CleargProcessedForms ();

    //inplace of updating all the variables, update only the variables that are modified
    //status = _GetVariableIDList(FormID, &varIDList);
    status = _GetDefaultNvramVariableIDList(FormID, TRUE, ResetVal, ValChanged);
    ASSERT_ERROR_STATUS(status);

DONE:
    if (varIDList) {
        MemFreePointer((VOID**)&varIDList);
    }
    CleanCurrLoadVarBuffer();


    return status;
}

/**
    Checks if the controls in a page is inconsistent

    @param PgInfo Input page info
    @param ErrStrToken Pointer to the output error code

    @retval BOOLEAN flag - TRUE if inconsistence, else FALSE
**/
BOOLEAN _InconsistenceCheck(PAGE_INFO *PgInfo, UINT16 *ErrStrToken)
{
  CONTROL_INFO *ctrlInfo = NULL;
  EFI_IFR_INCONSISTENT_IF *inconsistentIf = NULL;
  BOOLEAN flag = FALSE;
  UINT32 i = 0;

  //Find out if there is inconsistent value in any of the controls
  for(i=0; i < PgInfo->PageControls.ControlCount; i++)
  {
    
    ctrlInfo = (CONTROL_INFO*)((UINTN)(gControlInfo) + PgInfo->PageControls.ControlList[i]);
    //Check if there is a CONTROL_TYPE_MSGBOX in this page
    if(ctrlInfo->ControlType == CONTROL_TYPE_MSGBOX)
    {
      
      if(CheckControlCondition(ctrlInfo) == COND_INCONSISTENT)
      {
        
        inconsistentIf = (EFI_IFR_INCONSISTENT_IF*)ctrlInfo->ControlConditionalPtr;
        *ErrStrToken = inconsistentIf->Error;
        flag = TRUE;
        goto DONE;
      }
    }
  }

DONE:
  
  return flag;
}

/**
    Checks if the controls in a page has no submit

    @param ctrlInfo Input control info
    @param ErrStrToken Pointer to the output error code

    @retval BOOLEAN flag - TRUE if nosubmit, else FALSE
**/
BOOLEAN _NoSubmitCheck(CONTROL_INFO *ctrlInfo, UINT16 *ErrStrToken)
{
	EFI_IFR_NO_SUBMIT_IF *nosubmitIf = NULL;
	BOOLEAN flag = FALSE;

	

    if(ctrlInfo->ControlType == CONTROL_TYPE_MSGBOX)
    {
		
		if(CheckControlCondition(ctrlInfo) == COND_NOSUBMIT)
		{
			
			nosubmitIf = (EFI_IFR_NO_SUBMIT_IF*)ctrlInfo->ControlConditionalPtr;
			*ErrStrToken = nosubmitIf->Error;
			flag = TRUE;
		}
    }
  
	
	return flag;
}

/**
    function to get the list of all variables that are need to update

    @param FormID 
    @param VarIDList 

    @retval EFI_STATUS
**/
EFI_STATUS _GetVariableIDList(UINT32 FormID, UINT32 **VarIDList)
{
  EFI_STATUS status = EFI_SUCCESS;
  UINT32 count = 0;
  UINT32 index = 0;
  UINT32 varIndex = 0;
  UINT32 *varPtr = NULL;
  CONTROL_INFO *ctrlInfo;

  
  if(VarIDList == NULL || *VarIDList == NULL)
  {
    status = EFI_INVALID_PARAMETER;
  
    goto DONE;
  }
  AddEntryIngProcessedForms (FormID);
//  gProcessedForms[gProFormOffset++] = FormID;
  status = GetNumOfControls(FormID, &count);
  ASSERT_ERROR_STATUS(status);
  varPtr = *VarIDList;
  for(index = 0; index < count; index++)
  {
  
    status = _GetControlInfo(FormID , index, &ctrlInfo);
    ASSERT_ERROR_STATUS(status);

    if((ctrlInfo->ControlType == CONTROL_TYPE_SUBMENU) || (ctrlInfo->ControlType == CONTROL_TYPE_REF2))
    {

      if(_DestinationFormProcessed(ctrlInfo->ControlDestPageID) == FALSE)
      {

        _GetVariableIDList(ctrlInfo->ControlDestPageID, VarIDList);
      }
    }

    for(varIndex = 0; varIndex < gVariables->VariableCount; varIndex++)
    {

      if(varPtr[varIndex] == ctrlInfo->ControlVariable)
      {

        break;
      }
    }
    if(varIndex == gVariables->VariableCount)
    {

      varPtr[gVarOffset++] = ctrlInfo->ControlVariable;
    }
  }

DONE:

  return status;
}

/**
    Is this form Processed

    @param FormID 
					
				  
    @retval BOOLEAN
**/
BOOLEAN _DestinationFormProcessed(UINT32 FormID)
{
  BOOLEAN isProcessed = FALSE;
  UINT32 index = 0;

  
  for(index = 0; index < gProFormOffset; index++)
  {
    
    if(FormID == gProcessedForms[index])
    {
      
      isProcessed = TRUE;
      break;
    }
  }
  
  return isProcessed;
}

/**
    Is this formset or form has the no submit if

    @param FormID 
    @param ErrToken 
				  
    @retval BOOLEAN
**/
BOOLEAN IsNoSubmitOfForms(UINT32 FormID, UINT16* ErrToken)
{
	BOOLEAN flag = FALSE;
	EFI_STATUS status = EFI_SUCCESS;
	UINT32	count = 0;
	UINT32	index = 0;
	CONTROL_INFO	*ctrlInfo;
	
  
//	gProcessedForms[gProFormOffset++] = FormID;
	AddEntryIngProcessedForms (FormID);
	
	status = GetNumOfControls(FormID, &count);
	ASSERT_ERROR_STATUS(status);

	for(index = 0; index < count; index++)
	{
	    
		status = _GetControlInfo(FormID, index, &ctrlInfo);
		ASSERT_ERROR_STATUS(status);

		if((ctrlInfo->ControlType == CONTROL_TYPE_SUBMENU)//If submenu traverse recursively through the sub forms
			|| (ctrlInfo->ControlType == CONTROL_TYPE_REF2))
		{
			
			if(_DestinationFormProcessed(ctrlInfo->ControlDestPageID) == FALSE)//If the form has not be processed already
			{
				flag = IsNoSubmitOfForms(ctrlInfo->ControlDestPageID, ErrToken);
				if(flag)
				{
					break;
				}
			}
		}
		if(_NoSubmitCheck(ctrlInfo, ErrToken) == TRUE)//if the control is no submit if
		{
			flag = TRUE;
			break;
		}
	}//end of for loop of controls

DONE:
	
 	return flag;
}

/**
    Check for no submit if 

    @param FormID BOOLEAN Recursive, UINT16 *ErrToken
				  
    @retval BOOLEAN
**/
EFI_STATUS CheckforNosubmitIf(UINT32 FormID, BOOLEAN Recursive, UINT16 *ErrToken)
{
	EFI_STATUS status = EFI_SUCCESS;
	BOOLEAN flag = FALSE;

	if(Recursive)//To check all the forms in the formset
	{
//		gProFormOffset = 0;
//		MemSet(gProcessedForms, MAX_FORMS_TO_PROCESS, 0);
	   CleargProcessedForms ();
		flag = IsNoSubmitOfForms(FormID,ErrToken);
	}
	else//for single form ID
	{
		PAGE_INFO *PgInfo = NULL;
		EFI_STATUS status = EFI_SUCCESS;
		UINT32 i = 0;
		CONTROL_INFO *ctrlInfo = NULL; 

		status = _GetPageInfo(FormID, &PgInfo);
		ASSERT_ERROR_STATUS(status);

		for(i=0; i < PgInfo->PageControls.ControlCount; i++)
		{
			
			ctrlInfo = (CONTROL_INFO*)((UINTN)(gControlInfo) + PgInfo->PageControls.ControlList[i]);
			if(_NoSubmitCheck(ctrlInfo, ErrToken) == TRUE)
			{
				flag = TRUE;
				break;
			}
		}
	}
  
	if(flag)
	{
		status = EFI_NOSUBMIT_VALUE;
	}

DONE:
	return status;
}

/**
    Gets attribute flag for a given CtrlIndex in a page

    @param PgIndex Page index containing the control
    @param CtrlIndex Control index
    @param CtrlFlag Pointer to the flag attribute structure

    @retval EFI_STATUS status - EFI_SUCCESS if successful, else EFI_ERROR
**/
EFI_STATUS _GetControlFlag(UINT32 PgIndex, UINT32 CtrlIndex, CONTROL_FLAGS **CtrlFlag)
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *ctrlInfo = NULL;

  
  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  *CtrlFlag = &ctrlInfo->ControlFlags;

DONE:
  
  return status;
}

/**
    Get the Total Number of HII Form Pages to display

    @param numOfPages 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_OUT_OF_RESOURCES
**/
EFI_STATUS GetNumOfPages(UINT32 *NumOfPages)
{
  EFI_STATUS status = EFI_SUCCESS;

  
  if(LibInitialized == FALSE)
  {
    status = EFI_NOT_READY;
    
    goto DONE;
  }

  *NumOfPages = gPages->PageCount;

DONE:
  
  return status;
}

/**
    Get the Page Title String Index

    @param PgIndex 
    @param titleToken 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS GetPageTitle(UINT32 PgIndex, UINT16 *TitleToken)
{
  EFI_STATUS status = EFI_SUCCESS;
  PAGE_INFO *pageInfo = NULL;

  status = _GetPageInfo(PgIndex, &pageInfo);
  ASSERT_ERROR_STATUS(status);

  *TitleToken = pageInfo->PageTitle;

DONE:
  return status;

}

/**
    Get the Handle for the Page

    @param PgIndex 
    @param handle 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS GetPageHandle(UINT32 PgIndex, EFI_HII_HANDLE  *Handle)
{
  EFI_STATUS status = EFI_SUCCESS;
  PAGE_INFO *pageInfo = NULL;

  status = _GetPageInfo(PgIndex, &pageInfo);
  ASSERT_ERROR_STATUS(status);

  *Handle = pageInfo->PageHandle;
DONE:
  return status;
}

/**
    Get the page ID index for the Page

    @param PgIndex 
    @param PgIdIndex 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS GetPageIdIndex(UINT32 PgIndex, UINT16 *PgIdIndex)
{
  EFI_STATUS status = EFI_SUCCESS;
  PAGE_INFO *pageInfo = NULL;

  status = _GetPageInfo(PgIndex, &pageInfo);
  ASSERT_ERROR_STATUS(status);

  *PgIdIndex = pageInfo->PageIdIndex;

DONE:
  return status;
}

/**
    Is this page a parent page

    @param PgIndex 

    @retval BOOLEAN
**/
BOOLEAN IsParentPage(UINT32 PgIndex)
{
  EFI_STATUS status = EFI_SUCCESS;
  BOOLEAN isTrue = FALSE;
  PAGE_INFO *pageInfo = NULL;

  status = _GetPageInfo(PgIndex, &pageInfo);
  ASSERT_ERROR_STATUS(status);
  isTrue = pageInfo->PageParentID == 0? TRUE : FALSE;

DONE:
  return isTrue;
}

/**
    Get the Page Title String Index

    @param PgIndex 
    @param PageId 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS GetPageHiiId(UINT32 PgIndex, UINT16 *PageId)
{
  EFI_STATUS status = EFI_SUCCESS;
  PAGE_INFO *pageInfo = NULL;

  status = _GetPageInfo(PgIndex, &pageInfo);
  ASSERT_ERROR_STATUS(status);

  *PageId = pageInfo->PageFormID;

DONE:
  return status;
}

/**
    Get the parent page ID from the page index

    @param PgIndex Current page Index/ID
    @param PageParentId Parent page ID

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS GetPageParentId(UINT32 PgIndex, UINT16 *PageParentId)
{
  EFI_STATUS status = EFI_SUCCESS;
  PAGE_INFO *pageInfo = NULL;

  status = _GetPageInfo(PgIndex, &pageInfo);
  ASSERT_ERROR_STATUS(status);

  *PageParentId = pageInfo->PageParentID;

DONE:
  return status;
}

/**
    Returns the QuestionID to select in the destination page

    @param PgIndex ASL page ID
    @param CtrlIndex Control index, Lib control ID
    @param DestCtrlId Destination control index

    @retval EFI_STATUS status
**/
EFI_STATUS GetCtrlsDestCtrlId(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *DestCtrlId)
{
  EFI_STATUS status = EFI_NOT_FOUND;
  CONTROL_INFO *ctrlInfo = (CONTROL_INFO *)NULL;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  *DestCtrlId = ctrlInfo->DestQuestionID;

DONE:
  return status;
}

/**
    Get the AMI Control Type

    @param PgIndex 
    @param CtrlIndex 
    @param ctrlType 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS GetControlType(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *CtrlType)
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *ctrlInfo = NULL;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  *CtrlType = ctrlInfo->ControlType;

DONE:
  return status;
}

/**
    Get the Control Question Prompt's String Id

    @param PgIndex 
    @param CtrlIndex 
    @param ctrlPrompt 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS GetControlQuestionPrompt(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *CtrlPrompt)
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *ctrlInfo = NULL;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  *CtrlPrompt = ctrlInfo->QuestionId;
DONE:
  return status;
}

/**
    Get the Control Help String ID

    @param PgIndex 
    @param CtrlIndex 
    @param ctrlHelp 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS GetControlHelpStr(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *CtrlHelp)
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *ctrlInfo = NULL;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  *CtrlHelp = ctrlInfo->ControlHelp;
DONE:
  return status;
}

/**
    Get the List of Options for the Checkbox Control

    @param PgIndex 
    @param CtrlIndex 
    @param optionCount 
    @param optionArray 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS _GetCheckBoxOptions(CONTROL_INFO *CtrlInfo, UINT32 *OptionCount, OPTION_LIST **OptionArray)
{
  EFI_STATUS status = EFI_SUCCESS;
  OPTION_LIST *optList = NULL;
  OPTION_LIST *temp = NULL;
  EFI_IFR_CHECKBOX *checkBoxOp = (EFI_IFR_CHECKBOX*)(CtrlInfo->ControlPtr);
  UINT32 index = 0;

  *OptionCount = CHECKBOX_OPTION_COUNT;
  for(index = 0; index < *OptionCount; index++)
  {
  
    temp = (OPTION_LIST*)EfiLibAllocateZeroPool(sizeof(OPTION_LIST));
    if(temp == NULL)
    {
      status = EFI_OUT_OF_RESOURCES;
  
      goto DONE;
    }

    if(*OptionArray == NULL)
    {

      *OptionArray = temp;
    }else
    {

      if(optList){
        optList->Next = temp;
	  }
    }
    optList = temp;

    // Option will be set In the Application
    temp->Option = 0;
    temp->Value = index;
    temp->Flag = ((checkBoxOp->Flags & EFI_IFR_CHECKBOX_DEFAULT) == EFI_IFR_CHECKBOX_DEFAULT)? OPTION_DEFAULT : 0;
    temp->Flag = ((checkBoxOp->Flags & EFI_IFR_CHECKBOX_DEFAULT_MFG) == EFI_IFR_CHECKBOX_DEFAULT_MFG)? OPTION_DEFAULT_MFG : 0;
    temp->Next = NULL;
  }


DONE:

  return status;
}

/**
    Check if the control is a check box

    @param PgIndex 
    @param CtrlIndex 

    @retval BOOLEAN
**/
BOOLEAN IsControlCheckBox(UINT32 PgIndex, UINT32 CtrlIndex)
{
  BOOLEAN isCheckBox = FALSE;
  CONTROL_INFO *ctrlInfo = NULL;
  EFI_IFR_OP_HEADER *opHeader = NULL;

  _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);

  opHeader = (EFI_IFR_OP_HEADER*)ctrlInfo->ControlPtr;
  isCheckBox = (opHeader->OpCode == EFI_IFR_CHECKBOX_OP)? TRUE : FALSE;

  return isCheckBox;
}

/**
    Get the Data size

    @param PgIndex UINT32 CtrlIndex, UINT16 *controlWidth

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS GetControlDataWidth(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *CtrlWidth)
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *ctrlInfo = NULL;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  *CtrlWidth = (UINT16)GetControlDataLength(ctrlInfo)/*ctrlInfo->ControlDataWidth*/;
DONE:
  return status;
}

/**

    @param PgIndex 
    @param CtrlIndex 
    @param ctrlDestPg 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS GetControlDestinationPage(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *CtrlDestPg)
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *ctrlInfo = NULL;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  *CtrlDestPg = ctrlInfo->ControlDestPageID;
DONE:
  return status;
}

/**
    Get the Control Access Specifier for a Control

    @param PgIndex 
    @param CtrlIndex 

    @retval UINT8
**/
UINT8 GetControlAccess(UINT32 PgIndex, UINT32 CtrlIndex)
{
  CONTROL_FLAGS *flags = NULL;

  _GetControlFlag(PgIndex, CtrlIndex, &flags);
  
  return (UINT8)flags->ControlAccess;
}

/**
    Get the Control Refresh for a Control

    @param PgIndex 
    @param CtrlIndex 

    @retval UINT8
**/
UINT8 GetControlRefresh(UINT32 PgIndex, UINT32 CtrlIndex)
{
  CONTROL_FLAGS *flags = NULL;
  
  _GetControlFlag(PgIndex, CtrlIndex, &flags);
  
  return (UINT8)flags->ControlRefresh;
}

/**
    Get the Control Visiblity for a Control

    @param PgIndex 
    @param CtrlIndex 

    @retval BOOLEAN
**/
BOOLEAN IsControlVisible(UINT32 PgIndex, UINT32 CtrlIndex)
{
  CONTROL_FLAGS *flags = NULL;
  
  _GetControlFlag(PgIndex, CtrlIndex, &flags);
  
  return (BOOLEAN)flags->ControlVisible;
}

/**
    Get the Control Reset status for a Control

    @param PgIndex 
    @param CtrlIndex 

    @retval BOOLEAN
**/
BOOLEAN IsControlResetRequired(UINT32 PgIndex, UINT32 CtrlIndex)
{
  CONTROL_FLAGS *flags = NULL;
  
  _GetControlFlag(PgIndex, CtrlIndex, &flags);
  
  return (BOOLEAN)flags->ControlReset;
}

/**
    Get the Control Readonly Specifier for a Control

    @param PgIndex 
    @param CtrlIndex 

    @retval BOOLEAN
**/
BOOLEAN IsControlReadOnly(UINT32 PgIndex, UINT32 CtrlIndex)
{
  CONTROL_FLAGS *flags = NULL;
  
  _GetControlFlag(PgIndex, CtrlIndex, &flags);
  
  return (BOOLEAN)flags->ControlReadOnly;
}

/**
    Get the Control Interactive Specifier for a Control

    @param PgIndex 
    @param CtrlIndex 

    @retval EFI_STATUS
        BOOLEAN
**/
BOOLEAN IsControlInteractive(UINT32 PgIndex, UINT32 CtrlIndex)
{
    CONTROL_FLAGS *flags = (CONTROL_FLAGS *)NULL;
    EFI_STATUS status = EFI_SUCCESS;
    BOOLEAN isInteractive = FALSE;

    status = _GetControlFlag(PgIndex, CtrlIndex, &flags);
    ASSERT_ERROR_STATUS(status);

    if(flags != NULL){
        isInteractive = (BOOLEAN)flags->ControlInteractive;
    }

DONE:
    return isInteractive;
}

/**
    Evaluate control HII expression and return display Result

    @param PgIndex 
    @param CtrlIndex 
    @param expResult 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS EvaluateControlCondition(UINT32 PgIndex, UINT32 CtrlIndex, UINTN *ExpResult)
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *ctrlInfo = NULL;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  *ExpResult = CheckControlCondition(ctrlInfo);

DONE:
  return status;
}
/**
    Get the Control Reconnect Specifier for a Control

    @param PgIndex 
    @param CtrlIndex 

    @retval BOOLEAN
**/
BOOLEAN IsControlReConnect(UINT32 PgIndex, UINT32 CtrlIndex)
{
  CONTROL_FLAGS *flags = NULL;
  
  _GetControlFlag(PgIndex, CtrlIndex, &flags);
  if(NULL == flags )
	  return FALSE;
  return (BOOLEAN)flags->ControlReconnect;
}
/**
    Get the Question Value

    @param PgIndex 
    @param CtrlIndex 
    @param Value 
    @param ValueSize 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
        EFI_OUT_OF_RESOURCES
**/
EFI_STATUS GetQuestionValue(UINT32 PgIndex, UINT32 CtrlIndex, UINT8 **Value, UINTN *ValueSize)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_IFR_QUESTION_HEADER *questionHdr = NULL;
  CONTROL_INFO *ctrlInfo = NULL;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  *ValueSize = GetControlDataLength(ctrlInfo);
  *Value = (UINT8*)EfiLibAllocateZeroPool(*ValueSize);
  if(*Value == NULL)
  {
    status = EFI_OUT_OF_RESOURCES;
    
    goto DONE;
  }

  if(ctrlInfo->ControlType == CONTROL_TYPE_TEXT)
  {
    UINT16 textTwo = ((EFI_IFR_TEXT*)(ctrlInfo->ControlPtr))->TextTwo;
    
    MemCopy(*Value, &textTwo, *ValueSize);
    goto DONE;
  }

  questionHdr = (EFI_IFR_QUESTION_HEADER*)((UINT8*)ctrlInfo->ControlPtr + sizeof(EFI_IFR_OP_HEADER));
  status = VarGetValue(ctrlInfo->ControlVariable, questionHdr->VarStoreInfo.VarOffset, *ValueSize, Value);
  ASSERT_ERROR_STATUS(status);

DONE:
  
  return status;
}

/**
    Set the Value for a Question

    @param PgIndex 
    @param CtrlIndex 
    @param Value 
    @param ValueSize 
    @param ErrStrToken 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
**/
EFI_STATUS SetQuestionValue(UINT32 PgIndex, UINT32 CtrlIndex, UINT8 *Value, UINTN ValueSize, UINT16 *ErrStrToken)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_IFR_QUESTION_HEADER *questionHdr = NULL;
  CONTROL_INFO *ctrlInfo = NULL;
  PAGE_INFO *pgInfo = NULL;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  questionHdr = (EFI_IFR_QUESTION_HEADER*)((UINT8*)ctrlInfo->ControlPtr + sizeof(EFI_IFR_OP_HEADER));
  status = VarSetValue(ctrlInfo->ControlVariable, questionHdr->VarStoreInfo.VarOffset, ValueSize, Value);
  ASSERT_ERROR_STATUS(status);

  status = _GetPageInfo(PgIndex, &pgInfo);
  ASSERT_ERROR_STATUS(status);

  if(_InconsistenceCheck(pgInfo, ErrStrToken) == TRUE)
  {
    
    status = EFI_INCONSISTENT_VALUE;
  }

DONE:  
  return status;
}

/**
    Save all the values to the system

    @param VOID

    @retval EFI_STATUS
        EFI_SUCCESS
**/
EFI_STATUS SaveQuestionValues(UINT32 FormID)
{
  EFI_STATUS status = EFI_SUCCESS;
  EFI_STATUS tempStatus = EFI_SUCCESS;
  NVRAM_VARIABLE *nvPtr;
  UINT32 i = 0;
  UINT32 *varIDList = (UINT32*)EfiLibAllocateZeroPool(gVariables->VariableCount * sizeof(UINT32));
  
  if(varIDList == NULL)
  {
    status = EFI_OUT_OF_RESOURCES;
    
    goto DONE;
  }

  nvPtr = gVariableList;
  gVarOffset = 0;
//  gProFormOffset = 0;
//  MemSet(gProcessedForms, MAX_FORMS_TO_PROCESS, 0);
  CleargProcessedForms ();
  status = _GetVariableIDList(FormID, &varIDList);
  ASSERT_ERROR_STATUS(status);
  for ( i = 0; i < gVariables->VariableCount; i++)
  {
   
    if(nvPtr[varIDList[i]].Buffer == NULL)
    {
   
      continue;
    }
    tempStatus = VarSetNvram(varIDList[i], nvPtr[varIDList[i]].Buffer, nvPtr[varIDList[i]].Size);
    if(EFI_ERROR(tempStatus))
    {
      status = tempStatus;
    }
  }

DONE:
  if (varIDList) {
  	MemFreePointer((VOID**)&varIDList);
  }


  return status;
}

/**

    @param OptionArray 

    @retval VOID
**/
VOID FreeOptionList(OPTION_LIST *OptionArray)
{
  OPTION_LIST *optList = OptionArray;

  do
  {
    optList = OptionArray;
    OptionArray = OptionArray->Next;
    if(optList)
    {
      MemFreePointer((VOID**)&optList);
    }
  }while(OptionArray);
  
}

/**

    @param PgIndex 
    @param CtrlIndex 
    @param Format 

    @retval EFI_STATUS
**/
EFI_STATUS GetControlDisplayFormat(UINT32 PgIndex, UINT32 CtrlIndex, UINT16 *Format)
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *ctrlInfo;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);
  if(ctrlInfo->ControlType == CONTROL_TYPE_NUMERIC)
  {
   
    *Format = ((EFI_IFR_NUMERIC*)ctrlInfo->ControlPtr)->Flags & EFI_IFR_DISPLAY;
  }

DONE:
  return status;
}

/**

    @param PgIndex 
    @param CtrlIndex 
    @param Min 
    @param Max 

    @retval EFI_STATUS
        EFI_SUCCESS
        EFI_NOT_READY
        EFI_INVALID_PARAMETER
        EFI_OUT_OF_RESOURCES
**/
EFI_STATUS GetControlMinMax(UINT32 PgIndex, UINT32 CtrlIndex, UINT64 *Min, UINT64 *Max)
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *ctrlInfo = NULL;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  switch(ctrlInfo->ControlType)
  {
    case CONTROL_TYPE_NUMERIC:
    case CONTROL_TYPE_PASSWORD:
    case CONTROL_TYPE_POPUP_STRING:

    *Min = UefiGetMinValue(ctrlInfo->ControlPtr);
    *Max = UefiGetMaxValue(ctrlInfo->ControlPtr);
    break;
  default:

    break;
  }

DONE:
  return status;
}

/**

    @param PgIndex 
    @param CtrlIndex 
    @param Def 
    @param Optimal 

    @retval EFI_STATUS
**/
EFI_STATUS GetControlDefault(UINT32 PgIndex, UINT32 CtrlIndex,UINT8 *Def, BOOLEAN Optimal)
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *ctrlInfo = NULL;

  
  if(Def == NULL)
  {
    status = EFI_INVALID_PARAMETER;
  
    goto DONE;
  }
  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  switch(ctrlInfo->ControlDataWidth)
  {
    /*
    * ctrlInfo[sizeof(ctrlInfo)]
    * Used when user selects load Optimal value
    *
    * ctrlInfo[sizeof(ctrlInfo) + valSize]
    * Used when user selects load optimal value
    */
  case 1: // BYTE

    *Def = Optimal? *((UINT8 *) ((UINT8 *)ctrlInfo + sizeof(*ctrlInfo))) :
      *((UINT8 *) ((UINT8 *)ctrlInfo + sizeof(*ctrlInfo) + ctrlInfo->ControlDataWidth));
    break;
  case 2: //WORD

    *((UINT16*)Def) = Optimal? *((UINT16 *) ((UINT8 *)ctrlInfo + sizeof(*ctrlInfo))) :
      *((UINT16 *) ((UINT8 *)ctrlInfo + sizeof(*ctrlInfo) + ctrlInfo->ControlDataWidth));
    break;
  case 4: //DWORD

    *((UINT32*)Def) = Optimal? *((UINT32 *) ((UINT8 *)ctrlInfo + sizeof(*ctrlInfo))) :
      *((UINT32 *) ((UINT8 *)ctrlInfo + sizeof(*ctrlInfo) + ctrlInfo->ControlDataWidth));
    break;
  case 8: //QWORD

    *((UINT64*)Def) = Optimal? *((UINT64 *) ((UINT8 *)ctrlInfo + sizeof(*ctrlInfo))) :
      *((UINT64 *) ((UINT8 *)ctrlInfo + sizeof(*ctrlInfo) + ctrlInfo->ControlDataWidth));
    break;
  default:
    break;
  }

DONE:

  return status;
}

/**
    Create a Timer Event

    @param PgIndex UINT32 CtrlIndex, UINT8 *MaxEntry

    @retval EFI_STATUS
**/
EFI_STATUS GetOrderedListMaxEntries(UINT32 PgIndex, UINT32 CtrlIndex, UINT8 *MaxEntry)
{
  EFI_STATUS status = EFI_SUCCESS;
  CONTROL_INFO *ctrlInfo = NULL;

  status = _GetControlInfo(PgIndex, CtrlIndex, &ctrlInfo);
  ASSERT_ERROR_STATUS(status);

  *MaxEntry = 0;
  if(ctrlInfo->ControlType == CONTROL_TYPE_ORDERED_LIST)
  {
    *MaxEntry = UefiGetMaxEntries(ctrlInfo->ControlPtr);
  }
DONE:
  return status;
}

/**
    Gets OrderedList Data Width

    @param ControlInfo UINT32 *DataWidth

    @retval EFI_STATUS
**/
EFI_STATUS GetOrderedListOptionDataWidth(CONTROL_INFO *ControlInfo, UINT32 *DataWidth)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_IFR_OP_HEADER *OpHeader = NULL;
  EFI_IFR_ONE_OF_OPTION *OneOfOptionOp = NULL;

  OpHeader = (EFI_IFR_OP_HEADER*)ControlInfo->ControlPtr;

  if(OpHeader->OpCode != EFI_IFR_ORDERED_LIST_OP)
  {
    Status = EFI_UNSUPPORTED;
    goto DONE;
  }

  do
  {
    OpHeader = (EFI_IFR_OP_HEADER*)((UINT8*)ControlInfo->ControlPtr + OpHeader->Length);
  }while(OpHeader->OpCode != EFI_IFR_ONE_OF_OPTION_OP);

  OneOfOptionOp = (EFI_IFR_ONE_OF_OPTION*)OpHeader;

  switch(OneOfOptionOp->Type)
  {
  case EFI_IFR_TYPE_NUM_SIZE_16:
    *DataWidth = sizeof(UINT16);
    break;
  case EFI_IFR_TYPE_NUM_SIZE_32:
    *DataWidth = sizeof(UINT32);
    break;
  case EFI_IFR_TYPE_NUM_SIZE_64:
    *DataWidth = sizeof(UINT64);
    break;
  case EFI_IFR_TYPE_NUM_SIZE_8:
  default:
    *DataWidth = sizeof(UINT8);
    break;
  }

DONE:
  return Status;
}

/**
    Gets OrderedList DataType Width

    @param PgIndex UINT32 CtrlIndex, UINT32 *DataWidth

    @retval EFI_STATUS
**/
EFI_STATUS GetOrderedListDataTypeWidth(UINT32 PgIndex, UINT32 CtrlIndex, UINT32 *DataWidth)
{
  EFI_STATUS Status = EFI_SUCCESS;
  CONTROL_INFO *CtrlInfo = NULL;

  Status = _GetControlInfo(PgIndex, CtrlIndex, &CtrlInfo);
  if(EFI_ERROR(Status))
  {
    goto DONE;
  }

  Status = GetOrderedListOptionDataWidth(CtrlInfo, DataWidth);

DONE:
  return Status;
}

/**
    Is this page can be refreshable

    @param PgIndex 

    @retval BOOLEAN
**/
BOOLEAN IsPageRefreshable(UINT32 PgIndex)
{
  EFI_STATUS status = EFI_SUCCESS;
  BOOLEAN isTrue = FALSE;
  PAGE_INFO *pageInfo = NULL;

  status = _GetPageInfo(PgIndex, &pageInfo);
  ASSERT_ERROR_STATUS(status);
  isTrue = pageInfo->PageFlags.PageRefresh ? TRUE : FALSE;

DONE:
  return isTrue;
}

/**
    Is this a no commit variable 

    @param PgIndex 
    @param CtrlIndex 

    @retval BOOLEAN
**/
BOOLEAN IfNoCommitVariable(UINT32 PgIndex, UINT32 CtrlIndex)
{
    EFI_STATUS status = EFI_SUCCESS;
    BOOLEAN isTrue = FALSE;
    CONTROL_INFO *CtrlInfo = NULL;
    VARIABLE_INFO *varInfo = NULL;

    status = _GetControlInfo(PgIndex, CtrlIndex, &CtrlInfo);
    ASSERT_ERROR_STATUS(status);
    varInfo = VarGetVariableInfoIndex(CtrlInfo->ControlVariable);
    if((varInfo != NULL) && (varInfo->ExtendedAttibutes /*& AMI_SPECIAL_NO_COMMIT_VARIABLE*/)){ //If the variable is valid and set to NO_COMMIT
        isTrue = TRUE;
    }
DONE:
    return isTrue;
}

/**
    Is this formset has the default set

    @param FormID 
				  
    @retval BOOLEAN
**/
BOOLEAN IdentifyFormHasDefault(UINT32 FormID)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINT32	count = 0;
	UINT32	index = 0;
	CONTROL_INFO	*ctrlInfo;
	BOOLEAN isDefault = FALSE;

//	gProcessedForms[gProFormOffset++] = FormID;
	AddEntryIngProcessedForms (FormID);
	
	status = GetNumOfControls(FormID, &count);
	ASSERT_ERROR_STATUS(status);

	for(index = 0; index < count; index++)
	{

		status = _GetControlInfo(FormID, index, &ctrlInfo);
		ASSERT_ERROR_STATUS(status);

		if((ctrlInfo->ControlType == CONTROL_TYPE_SUBMENU)//If submenu traverse recursively through the sub forms
			|| (ctrlInfo->ControlType == CONTROL_TYPE_REF2))
		{

			if(_DestinationFormProcessed(ctrlInfo->ControlDestPageID) == FALSE)//If the form has not be processed already
			{
				isDefault = IdentifyFormHasDefault(ctrlInfo->ControlDestPageID);
				if(isDefault)
				{
					break;
				}
			}
		}

		if(ctrlInfo->ControlType == CONTROL_TYPE_CHECKBOX || //Check for the control is checkbox 
			ctrlInfo->ControlFlags.ControlEvaluateDefault || //or if the control has the Evaluate default bit set
			ctrlInfo->ControlDataWidth)						 // or if the ControlDataWidth is set to the size of the defaults
		{
			isDefault = TRUE;
			break;
		}
	}//end of for loop of controls

DONE:
	return isDefault;
}

/**
    Is this formset has the default set

    @param FormID 
				  
    @retval BOOLEAN
**/
BOOLEAN IsFormsetHasDefault(UINT32 FormID)
{
	BOOLEAN isDefault = FALSE;

//	gProFormOffset = 0;
//	MemSet(gProcessedForms, MAX_FORMS_TO_PROCESS, 0);
	CleargProcessedForms ();
	isDefault = IdentifyFormHasDefault(FormID);

	return isDefault;
}

/**
    To get the form GUID from the page index

    @param PgIndex 
    @param FormGuid 

    @retval EFI_STATUS
**/
EFI_STATUS GetPageGuidFromPgIndex(UINT32 PgIndex, EFI_GUID *FormGuid)
{
	EFI_STATUS status = EFI_INVALID_PARAMETER;
	PAGE_INFO *pgInfo = (PAGE_INFO *)NULL;
	PAGE_ID_INFO *pageIdInfo = (PAGE_ID_INFO*)NULL;

	status = _GetPageInfo(PgIndex, &pgInfo);
	ASSERT_ERROR_STATUS(status);

	if(gPages && gPageIdList && pgInfo)
	{
		pageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[pgInfo->PageIdIndex]);
		if(pageIdInfo)
		{
			MemCopy(FormGuid, &pageIdInfo->PageGuid, sizeof(EFI_GUID));
		}
	}

DONE:
	return status;
}
/**
    Save the values for the passed pageID
    If this function is not called as part of browser action then it is
    caller responsibility to reset the gProFormOffset and gProcessedForms
    gProFormOffset = 0;
    MemSet (gProcessedForms, MAX_FORMS_TO_PROCESS, 0);

    @param UINT32 - page ID to save the values

    @retval EFI_STATUS Status of the operation
**/
EFI_STATUS UefiVarsetnvramforCurrentform (UINT32 FormID)
{
	EFI_STATUS 	status = EFI_SUCCESS;
	UINT32 		count = 0;
	UINT32 		index = 0;
	CONTROL_INFO *ctrlInfo;
	UINT8 			*tempLoadVarBuff = (UINT8 *)NULL;
	UINT8 			*CurrentValue = NULL;
	UINTN			CurVarSize = 0;
	UINT32 		size = 0;
	UINT32 		offset = 0;
	VARIABLE_INFO 	*varInfo = (VARIABLE_INFO *)NULL;
	
//	gProcessedForms [gProFormOffset++] = FormID;
	AddEntryIngProcessedForms (FormID);
	
	status = GetNumOfControls (FormID, &count);
	ASSERT_ERROR_STATUS (status);
	
	for(index = 0; index < count; index++)
	{
		status = _GetControlInfo(FormID , index, &ctrlInfo);
		ASSERT_ERROR_STATUS(status);

		if (NULL == ctrlInfo->ControlPtr) //Setup crashes when clicking Discard and Exit goto control in simulator
			continue;
	
		if((ctrlInfo->ControlType == CONTROL_TYPE_SUBMENU) || (ctrlInfo->ControlType == CONTROL_TYPE_REF2))
		{
			if(_DestinationFormProcessed(ctrlInfo->ControlDestPageID) == FALSE)
			{
				UefiVarsetnvramforCurrentform(ctrlInfo->ControlDestPageID);
			}
			continue;
		}
		tempLoadVarBuff = VarGetNvram( ctrlInfo->ControlVariable, &CurVarSize );
		if (NULL == tempLoadVarBuff)
		{
			continue;
		}
		offset = UefiGetQuestionOffset (ctrlInfo->ControlPtr);
		if(CONTROL_TYPE_ORDERED_LIST == ctrlInfo->ControlType)
		{
			UINTN Type = 0;
			UefiGetValidOptionType (ctrlInfo, &Type, &size);
			size = (UefiGetMaxEntries (ctrlInfo->ControlPtr)) * size;
		}
		else
		{
			size = UefiGetWidth(ctrlInfo->ControlPtr);
		}
		//For Setup hangs on selecting 'Save current form now' option repeatedly four times and then selecting 'Discard current changes and exit' option issue
		if((0xFFFF == offset) || ((offset + size) > CurVarSize) || (0 == size))
			    continue;
		CurrentValue = EfiLibAllocateZeroPool(size);
		if(CurrentValue == NULL)
		{
			return EFI_OUT_OF_RESOURCES;
		}
		status = VarGetValue(ctrlInfo->ControlVariable, offset, size, CurrentValue );
		if (!EFI_ERROR (status))
		{
			MemCopy ( tempLoadVarBuff+offset, CurrentValue, size );
			
			 varInfo = VarGetVariableInfoIndex (ctrlInfo->ControlVariable);
			 if(varInfo == NULL)
		     {
        		continue;
    		 }
			 
			if((varInfo->ExtendedAttibutes & AMI_SPECIAL_VARIABLE_NO_SET) == AMI_SPECIAL_VARIABLE_NO_SET)
			{
				// Don't save the variable
				continue;
			}
			else
			{
				if(UefiIsEfiVariable(ctrlInfo->ControlVariable, varInfo))
				{
					VarSetNvramName( varInfo->VariableName, &varInfo->VariableGuid, varInfo->VariableAttributes, tempLoadVarBuff, CurVarSize );
				}
				else
				{
					UefiVarSetNvram(varInfo, tempLoadVarBuff, VAR_ZERO_OFFSET, CurVarSize);
				}
			}
		}
		MemFreePointer ((VOID **)&tempLoadVarBuff);
		MemFreePointer ((VOID **)&CurrentValue);
	}
DONE:
	return status;
}

/**
    Loads previous value for the current form

    @param FormID Current page ID

    @retval EFI_STATUS
**/
EFI_STATUS UefiLoadPreviousValuesforCurrentform(UINT32 FormID)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINT32 	count = 0;
	UINT32 	index = 0;
	CONTROL_INFO *ctrlInfo;
	UINT8 	*tempLoadVarBuff = (UINT8 *)NULL;
	UINT8 	*CurrentValue = NULL;
	UINTN		CurVarSize = 0;
	UINT32	size = 0;
	UINT32 	offset = 0;
	
//	gProcessedForms [gProFormOffset++] = FormID;
	AddEntryIngProcessedForms (FormID);
	
	status = GetNumOfControls (FormID, &count);
	ASSERT_ERROR_STATUS (status);
	
	for(index = 0; index < count; index++)
	{
		status = _GetControlInfo(FormID , index, &ctrlInfo);
		ASSERT_ERROR_STATUS(status);

		if (NULL == ctrlInfo->ControlPtr) //Avoid setup crash
			continue;

		if((ctrlInfo->ControlType == CONTROL_TYPE_SUBMENU) || (ctrlInfo->ControlType == CONTROL_TYPE_REF2))
		{
			if(_DestinationFormProcessed(ctrlInfo->ControlDestPageID) == FALSE)
			{
				UefiVarsetnvramforCurrentform(ctrlInfo->ControlDestPageID);
			}
		}
		tempLoadVarBuff = VarGetNvram (ctrlInfo->ControlVariable, &CurVarSize);
		if (NULL == tempLoadVarBuff)
		{
			continue;
		}
		offset = UefiGetQuestionOffset (ctrlInfo->ControlPtr);
		if (CONTROL_TYPE_ORDERED_LIST == ctrlInfo->ControlType)
		{
			UINTN Type = 0;
			UefiGetValidOptionType (ctrlInfo, &Type, &size);
			size = (UefiGetMaxEntries (ctrlInfo->ControlPtr)) * size;
		}
		else
		{
			size = UefiGetWidth (ctrlInfo->ControlPtr);
		}
		//For Setup hangs on selecting 'Save current form now' option repeatedly four times and then selecting 'Discard current changes and exit' option issue
		if((0xFFFF == offset) || ((offset + size) > CurVarSize) || (0 == size))
				    continue;
		CurrentValue = EfiLibAllocateZeroPool(size);
		if(CurrentValue == NULL)
		{
			return EFI_OUT_OF_RESOURCES;
		}
		status = VarGetValue (ctrlInfo->ControlVariable, offset, size, CurrentValue);
		if (!EFI_ERROR (status))
		{
			MemCopy (gVariableList [ctrlInfo->ControlVariable].Buffer + offset, tempLoadVarBuff + offset, size);
		}
		MemFreePointer ((VOID **)&tempLoadVarBuff);
		MemFreePointer ((VOID **)&CurrentValue);
	}
DONE:
	return status;
}

UINT16 GetCheckBoxStyle(CONTROL_INFO *controlInfo)
{
	EFI_GUID gSlideGuid = SLIDE_CHECKBOX_GUID;
	UINT8 *CondPtr;
	UINTN offset = 0;
//	GCC unused EFI_STATUS status = EFI_SUCCESS;
	INTN inScope = 0;
//  GCC unused 	BOOLEAN Inside = TRUE;

	CondPtr    = (UINT8 *)controlInfo->ControlPtr;

	if(!(((EFI_IFR_OP_HEADER*)CondPtr)->Scope))
		return CONTROL_TYPE_CHECKBOX;

	if(((EFI_IFR_OP_HEADER*)CondPtr)->OpCode != EFI_IFR_END_OP)
	{//Skip this Control
		inScope = 0;
	    do
	    {
	    	if(((EFI_IFR_OP_HEADER*)((UINT8*)CondPtr + offset))->Scope)
	        	inScope++;
	        offset += ((EFI_IFR_OP_HEADER*)((UINT8*)CondPtr + offset))->Length;

			if(*(CondPtr+offset) == EFI_IFR_GUID_OP)
			{//Check for the Guid opcode present
				if(EfiCompareGuid(&gSlideGuid, (EFI_GUID*)&((EFI_IFR_GUID*)(CondPtr+offset))->Guid ))
				 	return CONTROL_TYPE_SLIDE;
			}
	        if(((EFI_IFR_OP_HEADER*)((UINT8*)CondPtr + offset))->OpCode == EFI_IFR_END_OP)
	        	inScope--;
			if(!(((EFI_IFR_OP_HEADER*)CondPtr)->Scope))
				break;

	     }while(inScope > 0);
	  }

	return CONTROL_TYPE_CHECKBOX;
}

/**
    Clears gProcessedForms data

    @param void

    @retval void
**/
void CleargProcessedForms (void)
{
	gProFormOffset = 0;
	MemSet(gProcessedForms, (gProFormOffsetAllocCount * sizeof (UINT32)), 0);
}

/**
    Adds the formid in gProcessedForms

    @param FormID 

    @retval void
**/
void AddEntryIngProcessedForms (UINT32 FormID)
{
	if (gProFormOffset >= gProFormOffsetAllocCount)													//Check if enough size has been already allocated
	{
		gProcessedForms = MemReallocateZeroPool (gProcessedForms, gProFormOffsetAllocCount * sizeof (UINT32), (gProFormOffsetAllocCount + MAX_FORMS_TO_PROCESS) * sizeof (UINT32));
		if (NULL == gProcessedForms)
		{
			return;
		}
		gProFormOffsetAllocCount += MAX_FORMS_TO_PROCESS;
	}
	gProcessedForms [gProFormOffset++] = FormID;
}

/**
    Disconnects and Connects Disconnects controller

    @param void 

    @retval EFI_STATUS
**/
EFI_STATUS DoDisconnectAndConnectController (void)
{
	EFI_STATUS 	Status = EFI_SUCCESS;
	
	if (0 != gApp->CurrentPage)
	{
		EFI_HANDLE DriverHandle;
		EFI_HANDLE PageHandle = ((PAGE_INFO*)((UINTN)gApplicationData + gPages->PageList[gApp->CurrentPage]))->PageHandle;
		Status = gHiiDatabase->GetPackageListHandle(gHiiDatabase, PageHandle, &DriverHandle);		
		if (!EFI_ERROR (Status))
		{
			pBS->DisconnectController (DriverHandle, NULL, NULL);
			pBS->ConnectController (DriverHandle, NULL, NULL, TRUE);
		}
	}
	return EFI_SUCCESS;
}

/**
    function to get the max value for VARIABLE_ID_DYNAMIC_PAGE_GROUP_CLASS

    @param IN ContorlInfo, OUT Offset 

    @retval max

 **/
UINT16 GetMaxBasedOnClassCode(CONTROL_INFO *controlInfo, UINT16 *Offset)
{
	EFI_IFR_OP_HEADER *ifrPtr;
	PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[controlInfo->ControlPageID]);
	UINTN control = 0;
	CONTROL_INFO *CtrlInfo;
	BOOLEAN found = FALSE;
	EFI_QUESTION_ID  QuestionId;
	UINT32 varId = 0;
	UINT16 OffsetVal = 0, value = 0;

	ifrPtr = (EFI_IFR_OP_HEADER *)controlInfo->ControlConditionalPtr;

	if ( ifrPtr == NULL )
		return 0;

	if ( ( ifrPtr->OpCode != EFI_IFR_SUPPRESS_IF_OP ) && ( ifrPtr->OpCode != EFI_IFR_GRAY_OUT_IF_OP ) )
		return 0;

	ifrPtr = (EFI_IFR_OP_HEADER *)((UINTN)ifrPtr + ifrPtr->Length);
	// skip NOT if it is present
	if ( ifrPtr->OpCode == EFI_IFR_NOT_OP )
		ifrPtr = (EFI_IFR_OP_HEADER *)((UINTN)ifrPtr + ifrPtr->Length);

	if ( ifrPtr->OpCode == EFI_IFR_EQ_ID_LIST_OP )
		QuestionId = ((EFI_IFR_EQ_ID_VAL_LIST *)ifrPtr)->QuestionId;
	else if ( ifrPtr->OpCode == EFI_IFR_EQ_ID_VAL_OP )
		QuestionId = ((EFI_IFR_EQ_ID_VAL *)ifrPtr)->QuestionId;
	else
		return 0;

	if(pageInfo->PageHandle ==0)
		return 0;

	for ( control= 0; (control < pageInfo->PageControls.ControlCount) && (found == FALSE); control++ )
	{
		CtrlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

		if(QuestionId == CtrlInfo->ControlKey)
		{
			OffsetVal = UefiGetQuestionOffset(controlInfo->ControlPtr);
			found = TRUE;
		}
	}
	if(!found)
	{
		// Control Definition not in this Page. Look within the formset
		EFI_HII_HANDLE pgHandle = controlInfo->ControlHandle;
		UINT32 page = 0;

		for(;page < gPages->PageCount && (found == FALSE); page++)
		{
			if(page == controlInfo->ControlPageID)
			{
				continue;
			}
			pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[page]);
			if(pageInfo->PageHandle != pgHandle)
			{
				continue;
			}

			for(control = 0; control < pageInfo->PageControls.ControlCount && (found == FALSE); control++)
			{
				CtrlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

				if(QuestionId == CtrlInfo->ControlKey)
				{
					OffsetVal = UefiGetQuestionOffset(CtrlInfo->ControlPtr);
					found = TRUE;
				}
			}
		}
	}
	VarGetValue( CtrlInfo->ControlVariable,OffsetVal, sizeof(UINT16), &value );
	
	if(Offset != NULL)
		*Offset = OffsetVal;
	return value;
}
			
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
