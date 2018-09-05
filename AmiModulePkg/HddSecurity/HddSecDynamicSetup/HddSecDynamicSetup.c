//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file HddSecDynamicSetup.c
    HDD Security Setup Routines
    Use this hook to dynamically change HDD Security VFR data.

**/

#include <Token.h>
#include <AmiDxeLib.h>
// Setup eModule Header
#include <Setup.h>
// For variable access
#include <Guid/GlobalVariable.h>
#include <Library/MemoryAllocationLib.h>
// For HII operations
#include <Library/HiiLib.h>
#include <Protocol/HiiString.h>
#include <Guid/MdeModuleHii.h>
#include <Library/AmiHiiUpdateLib.h>
// For HddSecurity data
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#include <HddSecurityCommon.h>
// For this Module
#include "HddSecDynamicSetup.h"

#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
EFI_STRING_ID gHddSecSupportedStrIds[HDD_SECURITY_MAX_HDD_COUNT];
EFI_STRING_ID gHddSecEnabledStrIds[HDD_SECURITY_MAX_HDD_COUNT];
EFI_STRING_ID gHddSecLockedStrIds[HDD_SECURITY_MAX_HDD_COUNT];
EFI_STRING_ID gHddSecFrozenStrIds[HDD_SECURITY_MAX_HDD_COUNT];
EFI_STRING_ID gHddSecUserPwdStsStrIds[HDD_SECURITY_MAX_HDD_COUNT];
EFI_STRING_ID gHddSecMasterPwdStsStrIds[HDD_SECURITY_MAX_HDD_COUNT];
#else
EFI_STRING_ID gHddSecSupportedStrIds[1];
EFI_STRING_ID gHddSecEnabledStrIds[1];
EFI_STRING_ID gHddSecLockedStrIds[1];
EFI_STRING_ID gHddSecFrozenStrIds[1];
EFI_STRING_ID gHddSecUserPwdStsStrIds[1];
EFI_STRING_ID gHddSecMasterPwdStsStrIds[1];
#endif

UINTN gHddSecFrozenBaseKey = HDD_SECURITY_DYNAMIC_SETUP_SEC_FROZEN_00;
UINTN gHddSecShowMasterBaseKey = HDD_SECURITY_DYNAMIC_SETUP_SEC_SHOW_MP_00;
UINTN gHddSecUserPasswordBaseKey = HDD_SECURITY_DYNAMIC_SETUP_UP_00;
UINTN gHddSecMasterPasswordBaseKey = HDD_SECURITY_DYNAMIC_SETUP_MP_00;
UINTN gHddSecGotoBaseKey = HDD_SECURITY_DYNAMIC_SETUP_GOTO_00;
EFI_VARSTORE_ID     gVarstoreId = HDD_SECURITY_DYNAMIC_VARIABLE_ID;

static HDD_SECURITY_CONFIGURATION     *gHddSecurityConfig = NULL;
static EFI_GUID                       gHddSecurityConfigurationGuid       = HDD_SECURITY_CONFIGURATION_VARIABLE_GUID;
static HDD_SECURITY_INTERNAL_DATA     *gHddSecurityInternalData = NULL;
static UINT16                         gHddSecurityCount = 0;
static UINT16                         gValidHddSecurityCount;
static UINTN                          gCurrentHddIndex = 0xFFFF;
static EFI_HII_STRING_PROTOCOL        *HiiString = NULL;
static CHAR8                          *SupportedLanguages=NULL;
static EFI_GUID                       gSecurityFormsetGuid = SECURITY_FORM_SET_GUID;
static EFI_HII_HANDLE                 gHiiHandle = NULL;
static EFI_GUID                       gHddSecurityLoadSetupDefaultsGuid = AMI_HDD_SECURITY_LOAD_SETUP_DEFAULTS_GUID;
static EFI_EVENT                      gHddSecurityLoadSetupDefaultsEvent = NULL;

#if defined(HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE) && HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE
/**
    function to update the given password to non case sensitive

    @param CHAR16 *Password
    @param UINTN PwdLength

    @retval VOID

**/
static
VOID
UpdatePasswordToNonCaseSensitive(
        CHAR16 *Password,
        UINTN PwdLength
)
{
    UINTN Idx;
    for ( Idx = 0; Idx < PwdLength; Idx++ ) {
        Password[Idx] = ((Password[Idx]>=L'a')&&(Password[Idx]<=L'z'))?(Password[Idx]+L'A'-L'a'):Password[Idx];
    }
}
#endif

/**
    Locate the Security Protocols and return the information

    @param VOID

    @retval VOID

**/
static
UINT16
InitHddSecurityInternalDataPtr(
    VOID
)
{
    EFI_STATUS                      Status;
    AMI_HDD_SECURITY_INTERFACE      *Security = NULL;
    UINTN                           i;
    VOID                            *TempPtr;

    Status = pBS->LocateProtocol(
                    &gAmiHddSecurityProtocolGuid,
                    NULL,
                    (VOID**) &Security);

    if ( !EFI_ERROR( Status )) {
        gHddSecurityCount = *((UINT16*)Security->PostHddCount);
        gHddSecurityInternalData = *((HDD_SECURITY_INTERNAL_DATA**)Security->PostHddData);

        gValidHddSecurityCount = gHddSecurityCount;

        for(i=0;i<gHddSecurityCount;i++) {

            if( (gHddSecurityInternalData+i) != NULL ) {
                Status = pBS->HandleProtocol(
                                gHddSecurityInternalData[i].DeviceHandle,
                                &gAmiHddSecurityProtocolGuid,
                                &TempPtr
                                );

                if( EFI_ERROR(Status) ) {
                    TempPtr = 0;
                    gValidHddSecurityCount--;
                }
            }
        }

        return gValidHddSecurityCount;
    }

    return 0;
}

/**
    This function is wrapper function of pRS->GetVariable() with
    error check.

    @param CHAR16* - Variable name
    @param EFI_GUID - Variable GUID
    @param UINT32* - Variable attribute
    @param UINTN* - Variable size
    @param VOID** - pointer to variable's memory location

    @retval VOID

**/
EFI_STATUS
HddSecGetEfiVariable(
    IN CHAR16 *sName,
    IN EFI_GUID *pGuid,
    OUT UINT32 *pAttributes OPTIONAL,
    IN OUT UINTN *pDataSize,
    OUT VOID **ppData
)
{
    EFI_STATUS Status;
    if (!*ppData) *pDataSize=0;
    Status = pRS->GetVariable(sName, pGuid, pAttributes, pDataSize, *ppData);
    if (!EFI_ERROR(Status)) return Status;
    if (Status==EFI_BUFFER_TOO_SMALL)
    {
        if (*ppData) pBS->FreePool(*ppData);
        if (!(*ppData=Malloc(*pDataSize))) return EFI_OUT_OF_RESOURCES;
        Status = pRS->GetVariable(sName, pGuid, pAttributes, pDataSize, *ppData);
    }
    return Status;
}

/**
    return the String based on StringId

    @param HiiHandle
    @param StringId

    @retval EFI_STRING

**/
EFI_STRING
HddSecHiiGetString (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  EFI_STRING_ID   StringId
)
{
    EFI_STRING String = NULL;
    UINTN StringSize = 0;
    EFI_STATUS Status;
    CHAR8* PlatformLang = NULL;
    UINTN Size = 0;

    if(HiiString == NULL) {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) &HiiString);
        if(EFI_ERROR(Status))
            return NULL;
    }

    //language not provided - try platform language
    Status = HddSecGetEfiVariable(L"PlatformLang", &gEfiGlobalVariableGuid, NULL, &Size, &PlatformLang);
    if(!EFI_ERROR(Status)) {
        Status = HiiString->GetString(HiiString, PlatformLang, HiiHandle, StringId, String, &StringSize, NULL);
        if(Status == EFI_BUFFER_TOO_SMALL) {
            Status = pBS->AllocatePool(EfiBootServicesData, StringSize, (VOID**)&String);
            if(!EFI_ERROR(Status))
                Status = HiiString->GetString(HiiString, PlatformLang, HiiHandle, StringId, String, &StringSize, NULL);
        }
        pBS->FreePool(PlatformLang);
    }
    return (EFI_ERROR(Status)) ? NULL : String;
}

/**
    Add/Set the String to HII Database using HiiString Protocol

    @param HiiHandle
    @param String
    @param StringId

    @retval VOID

**/
EFI_STRING_ID
HddSecHiiAddStringInternal (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16 *        String,
    IN  EFI_STRING_ID   StringId
)
{
    EFI_STATUS      Status;
    CHAR8*          Languages = NULL;
    UINTN           LangSize = 0;
    CHAR8*          CurrentLanguage;
    BOOLEAN         LastLanguage = FALSE;

    if(HiiString == NULL) {
        Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) &HiiString);
        if(EFI_ERROR(Status)) {
            return 0;
        }
    }

    if(SupportedLanguages == NULL) {
        Status = HiiString->GetLanguages(HiiString, HiiHandle, Languages, &LangSize);
        if(Status == EFI_BUFFER_TOO_SMALL) {
            Status = pBS->AllocatePool(EfiBootServicesData, LangSize, (VOID**)&Languages);
            if(EFI_ERROR(Status)) {
                //
                //not enough resources to allocate string
                //
                return 0;
            }
            Status = HiiString->GetLanguages(HiiString, HiiHandle, Languages, &LangSize);
        }
        SupportedLanguages=Languages;
    } else {
        Languages=SupportedLanguages;
    }

    while(!LastLanguage) {
        //
        //point CurrentLanguage to start of new language
        //
        CurrentLanguage = Languages;
        while(*Languages != ';' && *Languages != 0)
            Languages++;

        //
        //Last language in language list
        //
        if(*Languages == 0) {
            LastLanguage = TRUE;
            if(StringId == 0) {
                Status = HiiString->NewString(HiiString, HiiHandle, &StringId, CurrentLanguage, NULL, String, NULL);
            } else {
                Status = HiiString->SetString(HiiString, HiiHandle, StringId, CurrentLanguage, String, NULL);
            }
            if(EFI_ERROR(Status)) {
                return 0;
            }
        } else {
            //
            //put null-terminator
            //
            *Languages = 0;
            if(StringId == 0) {
                Status = HiiString->NewString(HiiString, HiiHandle, &StringId, CurrentLanguage, NULL, String, NULL);
            } else {
                Status = HiiString->SetString(HiiString, HiiHandle, StringId, CurrentLanguage, String, NULL);
            }
            *Languages = ';';       //restore original character
            Languages++;
            if(EFI_ERROR(Status)) {
                return 0;
            }
        }
    }
    return StringId;
}

/**
    Add the String to HII Database using HiiString Protocol

    @param HiiHandle
    @param String

    @retval VOID

**/
EFI_STRING_ID
HddSecHiiAddString (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16 *        String
)
{
    return HddSecHiiAddStringInternal(HiiHandle, String, 0);
}

/**
    Set the String to HII Database using HiiString Protocol

    @param HiiHandle
    @param String
    @param StringId

    @retval VOID

**/
EFI_STRING_ID
HddSecHiiSetString (
    IN  EFI_HII_HANDLE  HiiHandle,
    IN  CHAR16 *        String,
    IN  EFI_STRING_ID   StringId
)
{
    return HddSecHiiAddStringInternal(HiiHandle, String, StringId);
}


/**
  Create EFI_IFR_PASSWORD_OP opcode.

  If OpCodeHandle is NULL, then ASSERT().
  If any reserved bits are set in QuestionFlags, then ASSERT().

  @param[in]  OpCodeHandle          Handle to the buffer of opcodes.
  @param[in]  QuestionId            Question ID
  @param[in]  VarStoreId            Storage ID
  @param[in]  VarOffset             Offset in Storage or String ID of the name (VarName)
                                    for this name/value pair.
  @param[in]  Prompt                String ID for Prompt
  @param[in]  Help                  String ID for Help
  @param[in]  QuestionFlags         Flags in Question Header
  @param[in]  PasswordFlag          Flags for password opcode
  @param[in]  MinSize               String minimum length
  @param[in]  MaxSize               String maximum length
  @param[in]  DefaultsOpCodeHandle  Handle for a buffer of DEFAULT opcodes.  This
                                    is an optional parameter that may be NULL.

  @retval NULL   There is not enough space left in Buffer to add the opcode.
  @retval Other  A pointer to the created opcode.

**/
static
UINT8 *
EFIAPI
HiiCreatePasswordOpCode (
  IN VOID             *OpCodeHandle,
  IN EFI_QUESTION_ID  QuestionId,
  IN EFI_VARSTORE_ID  VarStoreId,
  IN UINT16           VarOffset,
  IN EFI_STRING_ID    Prompt,
  IN EFI_STRING_ID    Help,
  IN UINT8            QuestionFlags,
  //IN UINT8            PasswordFlag,
  IN UINT8            MinSize,
  IN UINT8            MaxSize,
  IN VOID             *DefaultsOpCodeHandle  OPTIONAL
  )
{
  EFI_IFR_PASSWORD  OpCode;
  UINTN           Position;

  ASSERT ((QuestionFlags & (~(EFI_IFR_FLAG_READ_ONLY | EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_RESET_REQUIRED))) == 0);

  pBS->SetMem (&OpCode, sizeof (OpCode), 0);
  OpCode.Question.Header.Prompt          = Prompt;
  OpCode.Question.Header.Help            = Help;
  OpCode.Question.QuestionId             = QuestionId;
  OpCode.Question.VarStoreId             = VarStoreId;
  OpCode.Question.VarStoreInfo.VarOffset = VarOffset;
  OpCode.Question.Flags                  = QuestionFlags;
  OpCode.MinSize                         = MinSize;
  OpCode.MaxSize                         = MaxSize;
  //OpCode.Flags                           = PasswordFlag;

  if (DefaultsOpCodeHandle == NULL) {
    return InternalHiiCreateOpCodeExtended (OpCodeHandle, &OpCode, EFI_IFR_PASSWORD_OP, sizeof (OpCode), 0, 0);
  }

  Position = InternalHiiOpCodeHandlePosition (OpCodeHandle);
  InternalHiiCreateOpCodeExtended (OpCodeHandle, &OpCode, EFI_IFR_PASSWORD_OP, sizeof (OpCode), 0, 1);
  InternalHiiAppendOpCodes (OpCodeHandle, DefaultsOpCodeHandle);
  HiiCreateEndOpCode (OpCodeHandle);
  return InternalHiiOpCodeHandleBuffer (OpCodeHandle) + Position;
}

/**
  Create EFI_IFR_SUPPRESS_IF_OP/EFI_IFR_GRAY_OUT_IF_OP opcode with
  TRUE/FALSE condition.

  If OpCodeHandle is NULL, then ASSERT().

  @param[in]  OpCodeHandle          Handle to the buffer of opcodes.
  @param[in]  Value                 Condition value - TRUE/FALSE
  @param[in]  Suppress              1: EFI_IFR_SUPPRESS_IF_OP, 0: EFI_IFR_GRAY_OUT_IF_OP

  @retval NULL   There is not enough space left in Buffer to add the opcode.
  @retval Other  A pointer to the created opcode.

**/
static
UINT8 *
EFIAPI
AmiHiiCreateSuppresGrayVal(
  IN VOID               *OpCodeHandle,
  BOOLEAN               Value,
  BOOLEAN               Suppress    //if TRUE Suppress; False Gray out.
  )
{
    EFI_IFR_OP_HEADER   *Condition;
    EFI_IFR_OP_HEADER   *Header;
    UINT8               *Buffer=NULL;
    UINTN               sz;
//-------------------------------------
    //Allocate buffer for SUPPRESS_IF/GRAY_OUT_IF opcode + EFI_IFR_EQ_ID_VAL for suppress;
    sz=sizeof(EFI_IFR_OP_HEADER)+sizeof(EFI_IFR_OP_HEADER);
    Buffer=AllocateZeroPool(sz);
    if(Buffer==NULL) {
        ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
        return Buffer;
    }

    //Init Pointers;
    Header=(EFI_IFR_OP_HEADER*)Buffer;
    Condition=(EFI_IFR_OP_HEADER*)(Header+1);

    //Update OpCodes...
    //EFI_IFR_SUPPRESS_IF_OP/EFI_IFR_GRAY_OUT_IF_OP first;
    if(Suppress) Header->OpCode=EFI_IFR_SUPPRESS_IF_OP;
    else Header->OpCode=EFI_IFR_GRAY_OUT_IF_OP;
    Header->Length=sizeof(EFI_IFR_OP_HEADER);
    Header->Scope=1;

    //then goes conditions.
    //First update OpCode Header
    if(Value) Condition->OpCode=EFI_IFR_TRUE_OP;
    else Condition->OpCode=EFI_IFR_FALSE_OP;
    Condition->Scope=0;
    Condition->Length=sizeof(EFI_IFR_OP_HEADER);

    //Here we go...
    return HiiCreateRawOpCodes(OpCodeHandle,Buffer,sz);
}

/**
    This function will dynamically add VFR contents to HDD security BIOS setup page
    using HII library functions.

    @param VOID

    @retval VOID

**/
VOID
HddSecInitDynamicChildFormContents(
        VOID
)
{
    VOID                *StartOpCodeHandle;
    VOID                *EndOpCodeHandle;
    VOID                *YesNoOneOfOptionOpCodeHandle;
    EFI_IFR_GUID_LABEL  *StartLabel;
    EFI_IFR_GUID_LABEL  *EndLabel;
    UINTN               HddIndex;
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE    
    EFI_STRING          HddName = NULL;
    EFI_STRING_ID       HddNameToken = 0;
    UINTN               TotalHddIndex;
#endif

    StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    EndOpCodeHandle = HiiAllocateOpCodeHandle ();
    YesNoOneOfOptionOpCodeHandle = HiiAllocateOpCodeHandle ();

    // Create Hii Extended Label OpCode as the start and end opcode
    StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
            StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
            EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    StartLabel->Number = HDD_SECURITY_DYNAMIC_ITEM_START;
    EndLabel->Number = HDD_SECURITY_DYNAMIC_ITEM_END;


    HiiCreateOneOfOptionOpCode (
            YesNoOneOfOptionOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_YES),
            (EFI_IFR_OPTION_DEFAULT | EFI_IFR_OPTION_DEFAULT_MFG),
            EFI_IFR_NUMERIC_SIZE_1,
            1 );

    HiiCreateOneOfOptionOpCode (
            YesNoOneOfOptionOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_NO),
            0,
            EFI_IFR_NUMERIC_SIZE_1,
            0 );

#if (defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE) || ALL_HDD_SAME_PW == 0
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_PASSWORD_DESCRIPTION),
            STRING_TOKEN(STR_EMPTY),
            0,0);

    // HDD password description
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_EMPTY),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_ONE),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_TWO),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_THREE),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_FOUR),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_FIVE),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_SIX),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_SEVEN),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_EIGHT),
            STRING_TOKEN(STR_EMPTY),
            0,0);
#if ALL_HDD_SAME_PW
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_NINE),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_TEN),
            STRING_TOKEN(STR_EMPTY),
            0,0);
#endif


    //SEPARATOR
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_EMPTY),
            STRING_TOKEN(STR_EMPTY),
            0,0);
#endif

#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE && ALL_HDD_SAME_PW
    AmiHiiCreateSuppresGrayIdVal(StartOpCodeHandle, (EFI_QUESTION_ID)(gHddSecFrozenBaseKey), 1,TRUE);
    HiiCreatePasswordOpCode(
        StartOpCodeHandle,
        (EFI_QUESTION_ID)(gHddSecUserPasswordBaseKey),
        gVarstoreId,
        (UINT16)EFI_FIELD_OFFSET(HDD_SECURITY_CONFIGURATION, HddPassword[0]),
        STRING_TOKEN(STR_HDDSEC_USER_PASSWORD),
        STRING_TOKEN(STR_HDDSEC_USER_PASSWORD_HELP),
        EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_RESET_REQUIRED,
        0,
        32,
        NULL
    );
    //close scope
    AmiHiiTerminateScope(StartOpCodeHandle);
#if defined(DISPLAY_MASTER_PASSWORD) && DISPLAY_MASTER_PASSWORD
    AmiHiiCreateSuppresGrayIdVal(StartOpCodeHandle, (EFI_QUESTION_ID)(gHddSecFrozenBaseKey), 1,TRUE);
    AmiHiiCreateSuppresGrayIdVal(StartOpCodeHandle, (EFI_QUESTION_ID)(gHddSecShowMasterBaseKey), 0,FALSE);
    HiiCreatePasswordOpCode(
        StartOpCodeHandle,
        (EFI_QUESTION_ID)(gHddSecMasterPasswordBaseKey),
        gVarstoreId,
        (UINT16)EFI_FIELD_OFFSET(HDD_SECURITY_CONFIGURATION, HddPassword[1]),
        STRING_TOKEN(STR_HDDSEC_MASTER_PASSWORD),
        STRING_TOKEN(STR_HDDSEC_MASTER_PASSWORD),
        EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_RESET_REQUIRED,
        0,
        32,
        NULL
    );
    //close scope
    AmiHiiTerminateScope(StartOpCodeHandle);
    AmiHiiTerminateScope(StartOpCodeHandle);
#endif
    //SEPARATOR
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_EMPTY),
            STRING_TOKEN(STR_EMPTY),
            0,0);
#endif

    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_PASSWORD_CONFIGURATION),
            STRING_TOKEN(STR_EMPTY),
            0,0);

    //SEPARATOR
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_EMPTY),
            STRING_TOKEN(STR_EMPTY),
            0,0);

#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
    for(HddIndex = 0, TotalHddIndex=0; (TotalHddIndex < gHddSecurityCount) && (HddIndex < HDD_SECURITY_MAX_HDD_COUNT) \
             && (HddIndex < gValidHddSecurityCount); TotalHddIndex++ ) {

        if( !(HddSecurityInternalData + TotalHddIndex) ) {
            continue;
        }

        if(gHiiHandle == gHddSecurityInternalData[HddIndex].PostHiiHandle) {
            HddNameToken = gHddSecurityInternalData[HddIndex].PromptToken;
        } else {
            HddName = HddSecHiiGetString(gHddSecurityInternalData[HddIndex].PostHiiHandle, gHddSecurityInternalData[HddIndex].PromptToken);
            HddNameToken = HddSecHiiAddString(gHiiHandle, HddName);
        }

        HiiCreateSubTitleOpCode(StartOpCodeHandle,
                HddNameToken,
                STRING_TOKEN(STR_EMPTY),
                0,0);
#else
        HddIndex = 0;
#endif

    gHddSecSupportedStrIds[HddIndex] = HddSecHiiAddString(gHiiHandle, L"No");
    HiiCreateTextOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_SUPPORTED),
            STRING_TOKEN(STR_EMPTY),
            gHddSecSupportedStrIds[HddIndex]
    );

    gHddSecEnabledStrIds[HddIndex] = HddSecHiiAddString(gHiiHandle, L"No");
    HiiCreateTextOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_ENABLED),
            STRING_TOKEN(STR_EMPTY),
            gHddSecEnabledStrIds[HddIndex]
    );

    gHddSecLockedStrIds[HddIndex] = HddSecHiiAddString(gHiiHandle, L"No");
    HiiCreateTextOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_LOCKED),
            STRING_TOKEN(STR_EMPTY),
            gHddSecLockedStrIds[HddIndex]
    );

    // Suppress by default
    AmiHiiCreateSuppresGrayVal(StartOpCodeHandle, TRUE, TRUE);
    //Now create OneOf OpCode...
    HiiCreateOneOfOpCode (
        StartOpCodeHandle,
        (EFI_QUESTION_ID)(gHddSecFrozenBaseKey + HddIndex),
        gVarstoreId,
        (UINT16)EFI_FIELD_OFFSET(HDD_SECURITY_CONFIGURATION, Frozen[HddIndex]),
        STRING_TOKEN(STR_HDDSEC_FROZEN),
        STRING_TOKEN(STR_EMPTY),
        0,
        EFI_IFR_NUMERIC_SIZE_1,
        YesNoOneOfOptionOpCodeHandle,
        NULL
    );
    //close scope
    AmiHiiTerminateScope(StartOpCodeHandle);

    gHddSecFrozenStrIds[HddIndex] = HddSecHiiAddString(gHiiHandle, L"No");
    HiiCreateTextOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_FROZEN),
            STRING_TOKEN(STR_EMPTY),
            gHddSecFrozenStrIds[HddIndex]
    );

    gHddSecUserPwdStsStrIds[HddIndex] = HddSecHiiAddString(gHiiHandle, L"NOT INSTALLED");
    HiiCreateTextOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_USER_PASSWORD_STS),
            STRING_TOKEN(STR_EMPTY),
            gHddSecUserPwdStsStrIds[HddIndex]
    );

    gHddSecMasterPwdStsStrIds[HddIndex] = HddSecHiiAddString(gHiiHandle, L"NOT INSTALLED");
    HiiCreateTextOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_MASTER_PASSWORD_STS),
            STRING_TOKEN(STR_EMPTY),
            gHddSecMasterPwdStsStrIds[HddIndex]
    );

    // Suppress by default
    AmiHiiCreateSuppresGrayVal(StartOpCodeHandle, TRUE, TRUE);
    //Now create OneOf OpCode...
    HiiCreateOneOfOpCode (
        StartOpCodeHandle,
        (EFI_QUESTION_ID)(gHddSecShowMasterBaseKey + HddIndex),
        gVarstoreId,
        (UINT16)EFI_FIELD_OFFSET(HDD_SECURITY_CONFIGURATION, ShowMaster[HddIndex]),
        STRING_TOKEN(STR_EMPTY),
        STRING_TOKEN(STR_EMPTY),
        0,
        EFI_IFR_NUMERIC_SIZE_1,
        YesNoOneOfOptionOpCodeHandle,
        NULL
    );
    //close scope
    AmiHiiTerminateScope(StartOpCodeHandle);
#if !defined(SECURITY_SETUP_ON_SAME_PAGE) || SECURITY_SETUP_ON_SAME_PAGE == 0
    //SEPARATOR
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_EMPTY),
            STRING_TOKEN(STR_EMPTY),
            0,0);
#endif
#if ALL_HDD_SAME_PW == 0
    AmiHiiCreateSuppresGrayIdVal(StartOpCodeHandle, (EFI_QUESTION_ID)(gHddSecFrozenBaseKey + HddIndex), 1,TRUE);
    HiiCreatePasswordOpCode(
        StartOpCodeHandle,
        (EFI_QUESTION_ID)(gHddSecUserPasswordBaseKey + HddIndex),
        gVarstoreId,
        (UINT16)EFI_FIELD_OFFSET(HDD_SECURITY_CONFIGURATION, HddPassword[0]),
        STRING_TOKEN(STR_HDDSEC_USER_PASSWORD),
        STRING_TOKEN(STR_HDDSEC_USER_PASSWORD_HELP),
        EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_RESET_REQUIRED,
        0,
        32,
        NULL
    );
    //close scope
    AmiHiiTerminateScope(StartOpCodeHandle);

#if defined(DISPLAY_MASTER_PASSWORD) && DISPLAY_MASTER_PASSWORD
    AmiHiiCreateSuppresGrayIdVal(StartOpCodeHandle, (EFI_QUESTION_ID)(gHddSecFrozenBaseKey + HddIndex), 1,TRUE);
    AmiHiiCreateSuppresGrayIdVal(StartOpCodeHandle, (EFI_QUESTION_ID)(gHddSecShowMasterBaseKey + HddIndex), 0,FALSE);
    HiiCreatePasswordOpCode(
        StartOpCodeHandle,
        (EFI_QUESTION_ID)(gHddSecMasterPasswordBaseKey + HddIndex),
        gVarstoreId,
        (UINT16)EFI_FIELD_OFFSET(HDD_SECURITY_CONFIGURATION, HddPassword[1]),
        STRING_TOKEN(STR_HDDSEC_MASTER_PASSWORD),
        STRING_TOKEN(STR_HDDSEC_MASTER_PASSWORD),
        EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_RESET_REQUIRED,
        0,
        32,
        NULL
    );
    //close scope
    AmiHiiTerminateScope(StartOpCodeHandle);
    AmiHiiTerminateScope(StartOpCodeHandle);
#endif
#endif

#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
        //SEPARATOR
        HiiCreateSubTitleOpCode(StartOpCodeHandle,
                STRING_TOKEN(STR_EMPTY),
                STRING_TOKEN(STR_EMPTY),
                0,0);
        //SEPARATOR
        HiiCreateSubTitleOpCode(StartOpCodeHandle,
                STRING_TOKEN(STR_EMPTY),
                STRING_TOKEN(STR_EMPTY),
                0,0);

        HddIndex++;

    } // end of FOR loop
#endif

    //We are done!!
    HiiUpdateForm (
      gHiiHandle,
      &gSecurityFormsetGuid,
      HDD_SECURITY_DYNAMIC_SETUP_FORM,
      StartOpCodeHandle,
      EndOpCodeHandle
    );

    if (StartOpCodeHandle != NULL)
    {
        HiiFreeOpCodeHandle (StartOpCodeHandle);
        HiiFreeOpCodeHandle (EndOpCodeHandle);
        HiiFreeOpCodeHandle (YesNoOneOfOptionOpCodeHandle);
    }

}

/**
    This function will dynamically add VFR contents to HDD security BIOS setup page
    using HII library functions.

    @param VOID

    @retval VOID

**/
VOID
HddSecInitDynamicMainFormContents(
        VOID
)
{
    VOID                *StartOpCodeHandle;
    VOID                *EndOpCodeHandle;
    EFI_IFR_GUID_LABEL  *StartLabel;
    EFI_IFR_GUID_LABEL  *EndLabel;
    UINTN               HddIndex;
#if !(defined SECURITY_SETUP_ON_SAME_PAGE) || SECURITY_SETUP_ON_SAME_PAGE == 0
    EFI_STRING          HddName = NULL;
    EFI_STRING_ID       HddNameToken = 0;
    UINTN               TotalHddIndex;
#endif

    StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    EndOpCodeHandle = HiiAllocateOpCodeHandle ();

    // Create Hii Extended Label OpCode as the start and end opcode
    StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
            StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
            EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;

    StartLabel->Number = HDD_SECURITY_DYNAMIC_GOTO_START;
    EndLabel->Number = HDD_SECURITY_DYNAMIC_GOTO_END;

    //SEPARATOR
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_EMPTY),
            STRING_TOKEN(STR_EMPTY),
            0,0);

#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
    HddIndex = 0;
    // Add goto control
    HiiCreateGotoOpCode (
        StartOpCodeHandle,
        HDD_SECURITY_DYNAMIC_SETUP_FORM,
        STRING_TOKEN(STR_HDDSEC_SECURITY_ALL_HELP),
        STRING_TOKEN(STR_HDDSEC_SECURITY_ALL_HELP),
        EFI_IFR_FLAG_CALLBACK,
        (EFI_QUESTION_ID)(gHddSecGotoBaseKey + HddIndex)
    );
#else
#if ALL_HDD_SAME_PW
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_PASSWORD_DESCRIPTION),
            STRING_TOKEN(STR_EMPTY),
            0,0);

    // HDD password description
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_EMPTY),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_ONE),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_TWO),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_THREE),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_FOUR),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_FIVE),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_SIX),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_SEVEN),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_EIGHT),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_NINE),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_BANNER_TEN),
            STRING_TOKEN(STR_EMPTY),
            0,0);

    //SEPARATOR
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_EMPTY),
            STRING_TOKEN(STR_EMPTY),
            0,0);


    AmiHiiCreateSuppresGrayIdVal(StartOpCodeHandle, (EFI_QUESTION_ID)(gHddSecFrozenBaseKey), 1,TRUE);
    HiiCreatePasswordOpCode(
        StartOpCodeHandle,
        (EFI_QUESTION_ID)(gHddSecUserPasswordBaseKey),
        gVarstoreId,
        (UINT16)EFI_FIELD_OFFSET(HDD_SECURITY_CONFIGURATION, HddPassword[0]),
        STRING_TOKEN(STR_HDDSEC_USER_PASSWORD),
        STRING_TOKEN(STR_HDDSEC_USER_PASSWORD_HELP),
        EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_RESET_REQUIRED,
        0,
        32,
        NULL
    );
    //close scope
    AmiHiiTerminateScope(StartOpCodeHandle);

#if defined(DISPLAY_MASTER_PASSWORD) && DISPLAY_MASTER_PASSWORD
    AmiHiiCreateSuppresGrayIdVal(StartOpCodeHandle, (EFI_QUESTION_ID)(gHddSecFrozenBaseKey), 1,TRUE);
    AmiHiiCreateSuppresGrayIdVal(StartOpCodeHandle, (EFI_QUESTION_ID)(gHddSecShowMasterBaseKey), 0,FALSE);
    HiiCreatePasswordOpCode(
        StartOpCodeHandle,
        (EFI_QUESTION_ID)(gHddSecMasterPasswordBaseKey),
        gVarstoreId,
        (UINT16)EFI_FIELD_OFFSET(HDD_SECURITY_CONFIGURATION, HddPassword[1]),
        STRING_TOKEN(STR_HDDSEC_MASTER_PASSWORD),
        STRING_TOKEN(STR_HDDSEC_MASTER_PASSWORD),
        EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_RESET_REQUIRED,
        0,
        32,
        NULL
    );
    //close scope
    AmiHiiTerminateScope(StartOpCodeHandle);
    AmiHiiTerminateScope(StartOpCodeHandle);
#endif

    //SEPARATOR
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_EMPTY),
            STRING_TOKEN(STR_EMPTY),
            0,0);
    //SEPARATOR
    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_EMPTY),
            STRING_TOKEN(STR_EMPTY),
            0,0);
#endif

    HiiCreateSubTitleOpCode(StartOpCodeHandle,
            STRING_TOKEN(STR_HDDSEC_SECURITY_CONFIGURATION),
            STRING_TOKEN(STR_EMPTY),
            0,0);

    for(HddIndex = 0, TotalHddIndex=0; (TotalHddIndex < gHddSecurityCount) && (HddIndex < HDD_SECURITY_MAX_HDD_COUNT) \
                 && (HddIndex < gValidHddSecurityCount); TotalHddIndex++ ) {

        if( !(gHddSecurityInternalData + TotalHddIndex) ) {
            continue;
        }

        if(gHiiHandle == gHddSecurityInternalData[HddIndex].PostHiiHandle) {
            HddNameToken = gHddSecurityInternalData[HddIndex].PromptToken;
        } else {
            HddName = HddSecHiiGetString(gHddSecurityInternalData[HddIndex].PostHiiHandle, gHddSecurityInternalData[HddIndex].PromptToken);
            HddNameToken = HddSecHiiAddString(gHiiHandle, HddName);
        }

        // Add goto control
        HiiCreateGotoOpCode (
            StartOpCodeHandle,
            HDD_SECURITY_DYNAMIC_SETUP_FORM,
            HddNameToken,
            STRING_TOKEN(STR_HDDSEC_SECURITY_HELP),
            EFI_IFR_FLAG_CALLBACK,
            (EFI_QUESTION_ID)(gHddSecGotoBaseKey + HddIndex)
        );

        HddIndex++;
    }
#endif

    //We are done!!
    HiiUpdateForm (
      gHiiHandle,
      &gSecurityFormsetGuid,
      SECURITY_MAIN,
      StartOpCodeHandle,
      EndOpCodeHandle
    );

    if (StartOpCodeHandle != NULL)
    {
        HiiFreeOpCodeHandle (StartOpCodeHandle);
        HiiFreeOpCodeHandle (EndOpCodeHandle);
    }

}

/**
    Notification function for "Load Default" action in BIOS setup. This function
    will re-initialize the HDD security setup data.

    @param EFI_EVENT Event - Event to signal
    @param VOID* Context - Event specific context

    @retval VOID

**/
static
VOID
EFIAPI
HddSecurityLoadSetupDefaults (
        IN EFI_EVENT Event,
        IN VOID *Context
)
{
    HddSecurityRefreshSetupData(gCurrentHddIndex);
}

/**
    This function initializes HDD Security HiiHandle and IFR contents.

    @param  HiiHandle   Handle to HII database
    @param  Class       Indicates the setup class

    @retval VOID

**/

VOID
InitHddSecurityDynamicIfrContents (
    EFI_HII_HANDLE  HiiHandle,
    UINT16          Class
)
{
    
    EFI_STATUS    Status;
    UINTN         BufferSize = sizeof(UINTN);
    VOID          *NotifyRegistration;
    EFI_EVENT     NotifyEvent;
    EFI_HANDLE    TseHddNotify;
    EFI_GUID      AmiTseHddNotifyGuid = AMI_TSE_HDD_NOTIFY_GUID;
       
    gHiiHandle = HiiHandle;
    
    Status = pBS->LocateHandle ( ByProtocol,
                                 &AmiTseHddNotifyGuid,
                                 NULL,
                                 &BufferSize,
                                 &TseHddNotify );
       
       if (Status == EFI_SUCCESS) {
       //If HDD post data already initialized then Init Setup Strings here.
           AmiTseHddNotifyInitString (NULL, NULL);        
       } else if (Status == EFI_NOT_FOUND) {
           //Else register notify for AmiTseHddNotifyGuid
           Status = pBS->CreateEvent( EFI_EVENT_NOTIFY_SIGNAL,
                                      TPL_CALLBACK,
                                      AmiTseHddNotifyInitString,
                                      &NotifyRegistration,
                                      &NotifyEvent );
           if(EFI_ERROR(Status)) {
              return;
           }
           
           pBS->RegisterProtocolNotify( &AmiTseHddNotifyGuid,
                                        NotifyEvent,
                                        &NotifyRegistration );
       }
}

/**
    This Event notifies String initialization for HddSecurity for  can be processed
    
    @param  Event                 Event whose notification function is being invoked.
    @param  Context               The pointer to the notification function's context,
                                  which is implementation-dependent.

    @retval VOID

**/

VOID 
EFIAPI
AmiTseHddNotifyInitString(
    IN EFI_EVENT Event, 
    IN VOID *Context
)
{
    EFI_STATUS                Status;
    EFI_STRING                 vname = L"HddSecDynamicSetup";
    EFI_GUID                   vguid = HDD_SECURITY_CONFIGURATION_VARIABLE_GUID;
    UINTN                      vsize =  sizeof(HDD_SECURITY_CONFIGURATION);
    HDD_SECURITY_CONFIGURATION vdata;

    Status = pRS->SetVariable(vname, &vguid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS,
                              vsize, &vdata);

    // Initialize HDD security internal data structure and gHddSecurityCount
    gHddSecurityCount = InitHddSecurityInternalDataPtr( );

    if( 0 == gHddSecurityCount) { // No HDD detected
        return;
    }

    HddSecInitDynamicChildFormContents();
    HddSecInitDynamicMainFormContents();

    if(gHddSecurityLoadSetupDefaultsEvent == NULL) {
      Status = pBS->CreateEventEx(
                     EVT_NOTIFY_SIGNAL,
                     TPL_CALLBACK,
                     HddSecurityLoadSetupDefaults,
                     NULL,
                     &gHddSecurityLoadSetupDefaultsGuid,
                     &gHddSecurityLoadSetupDefaultsEvent);
      ASSERT_EFI_ERROR(Status);
    }

}


/**
    return the Security Status Information

    @param HddSecurityProtocol
    @param ReqStatus
    @param Mask

    @retval BOOLEAN      status in ReqStatus is valid

**/
BOOLEAN
HddSecurityCheckSecurityStatus (
    IN  AMI_HDD_SECURITY_PROTOCOL   *HddSecurityProtocol,
    IN  OUT BOOLEAN                 *ReqStatus,
    IN  UINT16                      Mask
)
{
    UINT16      SecurityStatus = 0;
    EFI_STATUS  Status;

    //get the security status of the device
    Status = HddSecurityProtocol->ReturnSecurityStatus( HddSecurityProtocol, &SecurityStatus );

    if ( EFI_ERROR( Status ))
        return FALSE; // Unable to get security status
    // Mask security status based on supplied mask data
    *ReqStatus = (BOOLEAN)((SecurityStatus & Mask) ? TRUE : FALSE );
    return TRUE;
}

/**
    Updates the password for the current HDD alone.

    @param UINT32 Index,
    @param CHAR16 *Password,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
BOOLEAN
HddSecurityUpdateHdd (
    UINT32      Index,
    CHAR16      *Password,
    BOOLEAN     bCheckUser
)
{
    AMI_HDD_SECURITY_PROTOCOL   *HddSecurityProtocol = NULL;
    UINT16                      Control              = bCheckUser ? 0 : 1;
    UINT8                       RevisionCode         = 0;
    BOOLEAN                     Locked, Enabled;
    EFI_STATUS                  Status = EFI_UNSUPPORTED;
    UINT8                       Buffer[IDE_PASSWORD_LENGTH + 1];
    #if !SETUP_SAME_SYS_HDD_PW
    UINTN                       ii;
    #endif
    HDD_SECURITY_INTERNAL_DATA  *DataPtr;

#if defined(HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE) && HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE
    UpdatePasswordToNonCaseSensitive(Password, Wcslen(Password));
#endif

    DataPtr = &gHddSecurityInternalData[Index];

    if ( DataPtr == NULL ) {
        return FALSE;
    }
    HddSecurityProtocol = DataPtr->HddSecurityProtocol;

    //get the status of the device
    if ( !(
            HddSecurityCheckSecurityStatus(
                                     HddSecurityProtocol, &Locked,
                                     SecurityLockedMask )
             && HddSecurityCheckSecurityStatus( HddSecurityProtocol, &Enabled,
                                     SecurityEnabledMask ))) {
        return FALSE;
    }

    if ( !Locked ) {
        if ( Password[0] == 0 ) {
            //empty string is entered -> disable password
            Status = HddSecurityProtocol->SecurityDisablePassword(
                HddSecurityProtocol,
                Control,
                gHddSecurityInternalData[Index].PWD );
        } else {
            //set new password
            MemSet( &Buffer, IDE_PASSWORD_LENGTH + 1, 0 );
            #if !SETUP_SAME_SYS_HDD_PW

            for ( ii = 0; ii < IDE_PASSWORD_LENGTH + 1; ii++ ) {
                Buffer[ii] = (UINT8)Password[ii];

                if ( Password[ii] == L'\0' ) {
                    break;
                }
            }// end of for
            #else
            pBS->CopyMem( Buffer, Password, IDE_PASSWORD_LENGTH + 1 );
            #endif

            Status = HddSecurityProtocol->SecuritySetPassword(
                HddSecurityProtocol,
                Control,
                Buffer,
                RevisionCode );
        }
    }// end if(!Locked)

    if ( EFI_ERROR( Status )) {
        return FALSE;
    }

    return TRUE;
}

/**
    Updates the HDD password for all the HDDs present.

    @param UINT32 Index,
    @param CHAR16 *Password,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
BOOLEAN
HddSecurityUpdateAllHdd (
    UINT32      Index,
    CHAR16      *Password,
    BOOLEAN     bCheckUser
)
{
    UINTN       i;
    BOOLEAN     Status = FALSE;
        for ( i = 0; i < gHddSecurityCount; i++ ) {
            Status = HddSecurityUpdateHdd( (UINT32)i, Password, bCheckUser);
        }
    return Status;
}

/**
    Hook function to update the password for the HDDs based
    on the token ALL_HDD_SAME_PW.

    @param  UINT32 Index,
    @param  CHAR16 *Password,
    @param  BOOLEAN bCheckUser

    @retval VOID

**/
BOOLEAN
HddSecurityUpdate (
    UINT32      Index,
    CHAR16      *Password,
    BOOLEAN     bCheckUser
)
{
    #if ALL_HDD_SAME_PW
     return HddSecurityUpdateAllHdd( Index, Password, bCheckUser);
    #else
     return HddSecurityUpdateHdd( Index, Password, bCheckUser);
    #endif
}

/**
    Validates the password for the current HDD alone.

    @param CHAR16 *Password,
    @param VOID* Ptr,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
EFI_STATUS
HddSecurityAuthenticateHdd (
    CHAR16  *Password,
    VOID    * Ptr,
    BOOLEAN bCheckUser
)
{
    UINT16            Control = 0;
    EFI_STATUS        Status;
    UINT8             Buffer[IDE_PASSWORD_LENGTH + 1];

    #if !SETUP_SAME_SYS_HDD_PW
    UINTN             i;
    #endif

    HDD_SECURITY_INTERNAL_DATA * DataPtr = (HDD_SECURITY_INTERNAL_DATA*)Ptr;

#if defined(HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE) && HDD_SECURITY_PASSWORD_NON_CASE_SENSITIVE
    UpdatePasswordToNonCaseSensitive(Password, Wcslen(Password));
#endif

    MemSet( &Buffer, IDE_PASSWORD_LENGTH + 1, 0 );

    #if !SETUP_SAME_SYS_HDD_PW

    for ( i = 0; i < IDE_PASSWORD_LENGTH + 1; i++ ) {
        Buffer[i] = (UINT8)Password[i];

        if ( Password[i] == L'\0' ) {
            break;
        }
    }
    #else
    pBS->CopyMem( Buffer, Password, IDE_PASSWORD_LENGTH + 1 );
    #endif

    Control = bCheckUser ? 0 : 1;

    Status = (DataPtr->HddSecurityProtocol)->SecurityUnlockPassword(
        DataPtr->HddSecurityProtocol,
        Control,
        Buffer );

    if ( EFI_ERROR( Status )) {
        return EFI_ACCESS_DENIED;
    }

    //save password in case we need to disable it during the setup
    pBS->CopyMem( &(DataPtr->PWD), &Buffer, IDE_PASSWORD_LENGTH + 1 );
//    DataPtr->Locked = FALSE;

    if ( !bCheckUser ) {
        DataPtr->LoggedInAsMaster = TRUE;
    }

    return EFI_SUCCESS;
}

/**
    Validates the password for all the HDDs Present.

    @param CHAR16 *Password,
    @param VOID* Ptr,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
EFI_STATUS
HddSecurityAuthenticateAllHdd (
    CHAR16  *Password,
    VOID    * Ptr,
    BOOLEAN bCheckUser
)
{
    HDD_SECURITY_INTERNAL_DATA *DataPtr;
    UINTN i;
    EFI_STATUS        Status=EFI_NOT_FOUND;

    //For all drives
    DataPtr = gHddSecurityInternalData;

    if(DataPtr == NULL) {
        return EFI_NOT_FOUND;
    }

    for ( i = 0; i < gHddSecurityCount; i++ ) {

        Status = HddSecurityAuthenticateHdd( Password,
                                          DataPtr,
                                          bCheckUser );
        if ( EFI_SUCCESS != Status ) {
            // Unlock failed.
            LibReportStatusCode( EFI_ERROR_CODE | EFI_ERROR_MAJOR,
                                    DXE_INVALID_IDE_PASSWORD,
                                    0,
                                    NULL,
                                    NULL );
        }
        DataPtr++;
    }
    return Status;
}

/**
    Hook function to validate Password for the HDDs based on
    the token ALL_HDD_SAME_PW

    @param CHAR16 *Password,
    @param VOID* Ptr,
    @param BOOLEAN bCheckUser

    @retval VOID

**/
EFI_STATUS
HddSecurityAuthenticate (
    CHAR16  *Password,
    VOID    * Ptr,
    BOOLEAN bCheckUser
)
{
    #if ALL_HDD_SAME_PW
     return HddSecurityAuthenticateAllHdd( Password, Ptr, bCheckUser);
    #else
     return HddSecurityAuthenticateHdd( Password, Ptr, bCheckUser);
    #endif
}

/**
    Initializes the structure HDD_SECURITY_CONFIG for the current
    HDD if the data pointer to the structure HDD_SECURITY_INTERNAL_DATA is
    initialized already.

    @param HDD_SECURITY_CONFIGURATION *HddSecurityConfig
    @param  UINTN value

    @retval VOID

**/
VOID
HddSecurityUpdateSetupDataByHddIndex(
    VOID    *TempHddSecurityConfig,
    UINTN   value
)
{
    HDD_SECURITY_INTERNAL_DATA    *DataPtr             = NULL;
    AMI_HDD_SECURITY_PROTOCOL     *HddSecurityProtocol = NULL;
    BOOLEAN                       Status;
    UINT32                        IdePasswordFlags = 0;
    EFI_STATUS                    ReturnStatus;
    HDD_SECURITY_CONFIGURATION           *HddSecurityConfig
        = (HDD_SECURITY_CONFIGURATION*)TempHddSecurityConfig;

    //Set current HDD security page
    DataPtr = (HDD_SECURITY_INTERNAL_DATA*)&gHddSecurityInternalData[value];
#if !defined(SECURITY_SETUP_ON_SAME_PAGE) || SECURITY_SETUP_ON_SAME_PAGE == 0
    value = 0;
#endif

    if ( DataPtr ) {
        HddSecurityProtocol = DataPtr->HddSecurityProtocol;

        HddSecurityCheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecuritySupportedMask );
        HddSecurityConfig->Supported[value] = Status ? 1 : 0;
        HddSecurityCheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecurityEnabledMask );
        HddSecurityConfig->Enabled[value] = Status ? 1 : 0;
        HddSecurityCheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecurityLockedMask );
        HddSecurityConfig->Locked[value] = Status ? 1 : 0;
        HddSecurityCheckSecurityStatus(
            HddSecurityProtocol,
            &Status,
            SecurityFrozenMask );
        HddSecurityConfig->Frozen[value] = Status ? 1 : 0;
        ReturnStatus         = HddSecurityProtocol->ReturnIdePasswordFlags(
            HddSecurityProtocol,
            &IdePasswordFlags );

        if ( EFI_ERROR( ReturnStatus )) {
            return;
        }

        HddSecurityConfig->UserPasswordStatus[value]
            = (IdePasswordFlags & 0x00020000) ? 1 : 0;
        HddSecurityConfig->MasterPasswordStatus[value]
            = (IdePasswordFlags & 0x00010000) ? 1 : 0;

        HddSecurityConfig->ShowMaster[value] = 0x0000;

        if ( HddSecurityConfig->Locked[value] ) {
            HddSecurityConfig->ShowMaster[value] = 0x0001;
        } else if ( (DataPtr->LoggedInAsMaster)) {
            HddSecurityConfig->ShowMaster[value] = 0x0001;
        } else if ( !(HddSecurityConfig->UserPasswordStatus[value])) {
            HddSecurityConfig->ShowMaster[value] = 0x0001;
        }

        HddSecHiiSetString(gHiiHandle, HddSecurityConfig->Supported[value]  ? L"Yes" : L"No", gHddSecSupportedStrIds[value]);
        HddSecHiiSetString(gHiiHandle, HddSecurityConfig->Enabled[value]  ? L"Yes" : L"No", gHddSecEnabledStrIds[value]);
        HddSecHiiSetString(gHiiHandle, HddSecurityConfig->Locked[value]  ? L"Yes" : L"No", gHddSecLockedStrIds[value]);
        HddSecHiiSetString(gHiiHandle, HddSecurityConfig->Frozen[value]  ? L"Yes" : L"No", gHddSecFrozenStrIds[value]);
        HddSecHiiSetString(gHiiHandle, HddSecurityConfig->UserPasswordStatus[value]  ? L"INSTALLED" : L"NOT INSTALLED", gHddSecUserPwdStsStrIds[value]);
        HddSecHiiSetString(gHiiHandle, HddSecurityConfig->MasterPasswordStatus[value]  ? L"INSTALLED" : L"NOT INSTALLED", gHddSecMasterPwdStsStrIds[value]);
    }// end if
    return;
}

/**
    Initializes the structure HDD_SECURITY_CONFIG for all the
    HDDs present if the data pointer to the structure
    HDD_SECURITY_INTERNAL_DATA is initialized already.

    @param HDD_SECURITY_CONFIG *HddSecurityConfig
    @param  UINTN value

    @retval VOID

**/
VOID
HddSecurityUpdateSetupDataForAllHdd(
    VOID    *HddSecurityConfig,
    UINTN   value
)
{
    UINTN i;

    for ( i = 0; i < gHddSecurityCount; i++ ) {
        HddSecurityUpdateSetupDataByHddIndex( HddSecurityConfig, i);
    }
    return;
}

/**
    Hook function to Initialize the structure HDD_SECURITY_CONFIG
    for the HDDs based on the token ALL_HDD_SAME_PW.

    @param HDD_SECURITY_CONFIG *HddSecurityConfig
    @param  UINTN value

    @retval VOID

**/
VOID
HddSecurityUpdateSetupData(
    VOID    *HddSecurityConfig,
    UINTN   value
)
{
    #if ALL_HDD_SAME_PW
    HddSecurityUpdateSetupDataForAllHdd( HddSecurityConfig, value);
    #else
    HddSecurityUpdateSetupDataByHddIndex( HddSecurityConfig, value);
    #endif

}

/**
    function to update the setup configuration page after HDD password update

    @param VOID

    @retval VOID

**/
VOID
HddSecurityRefreshSetupData (
        UINTN               HddIndex
)
{
    EFI_STATUS          Status;

    // Allocate memory for setup configuration data
    if(NULL == gHddSecurityConfig) {
        Status = pBS->AllocatePool( EfiBootServicesData,
                                    sizeof(HDD_SECURITY_CONFIGURATION),
                                    (VOID**)&gHddSecurityConfig);
        if(EFI_ERROR(Status) || NULL == gHddSecurityConfig) {
            return;
        }
    }

    gValidHddSecurityCount = InitHddSecurityInternalDataPtr( );
    
    if(HddIndex == 0xFFFF) {
        // Update setup data for all HDDs
        HddSecurityUpdateSetupDataForAllHdd( (VOID*)gHddSecurityConfig, HddIndex );
    } else {
        HddSecurityUpdateSetupData( (VOID*)gHddSecurityConfig, HddIndex );
    }

    // Set the new setup configuration data
    pRS->SetVariable(L"HddSecDynamicSetup",
                    &gHddSecurityConfigurationGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    sizeof(HDD_SECURITY_CONFIGURATION),
                    gHddSecurityConfig);

    // Refresh browser data
    HiiLibSetBrowserData( sizeof(HDD_SECURITY_CONFIGURATION),
                        gHddSecurityConfig,
                        &gHddSecurityConfigurationGuid,
                        L"HddSecDynamicSetup");

}

/**
    This function handles password operations (check/validate/change/clear)
    for all HDD password fields

    @param  EFI_HII_HANDLE HiiHandle - HII handle of formset
    @param  UINT16 Class             - class of formset
    @param  UINT16 SubClass          - subclass of formset
    @param  UINT16 Key               - Id of setup control

    @retval EFI_STATUS

    @note
    Case1: Check password status
        EFI_SUCCESS    - No password
        EFI_NOT_FOUND  - Password exists
    Case2: Validate password
        EFI_SUCCESS    - Password is correct
        EFI_NOT_READY  - Password is wrong
    Case3: Set/Clear password
        EFI_SUCCESS    - Operation successful
        EFI_NOT_READY  - Operation failed
**/

EFI_STATUS
HddSecurityDynamicSetupPasswordCallback(
        EFI_HII_HANDLE HiiHandle,
        UINT16         Class,
        UINT16         SubClass,
        UINT16         Key
)
{
    EFI_STATUS          Status;
    CALLBACK_PARAMETERS *CallbackParameters;
    CHAR16              PasswordBuffer[IDE_PASSWORD_LENGTH+1];
    static CHAR16       OldPasswordBuffer[IDE_PASSWORD_LENGTH+1];
    BOOLEAN             bCheckUser = FALSE;
    BOOLEAN             IsSecurityEnabled = FALSE;
    static BOOLEAN      IsPasswordValidated = FALSE;
    static BOOLEAN      NoPasswordExistedOnFirstCall = FALSE;
    static UINTN        LastHardDiskNumber = 0xFFFF;
    UINTN               CurrentHardDiskNumber;
    UINTN               size;
    VOID*               DataPtr;

    // Get actual parameter passed by browser
    CallbackParameters = GetCallbackParameters();

    // Check for supported browser actions
    if(!CallbackParameters || ((CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING) && (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED) && (CallbackParameters->Action != EFI_BROWSER_ACTION_DEFAULT_STANDARD))) {
        return EFI_UNSUPPORTED;
    }

    // Always return success here
    if((CallbackParameters->Action == EFI_BROWSER_ACTION_CHANGED) || (CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD )) {
        return EFI_SUCCESS;
    }

    // Check which password field was selected
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
        if(gHddSecUserPasswordBaseKey <= Key && Key < gHddSecUserPasswordBaseKey + HDD_SECURITY_MAX_HDD_COUNT) {
            bCheckUser = TRUE;
            CurrentHardDiskNumber = Key - gHddSecUserPasswordBaseKey;
        } else if (gHddSecMasterPasswordBaseKey <= Key && Key < gHddSecMasterPasswordBaseKey + HDD_SECURITY_MAX_HDD_COUNT) {
            bCheckUser = FALSE;
            CurrentHardDiskNumber = Key - gHddSecMasterPasswordBaseKey;
        } else {
            CurrentHardDiskNumber = 0xFFFF;
            LastHardDiskNumber = 0xFFFF;
        }
#else
        if(gHddSecUserPasswordBaseKey == Key) {
            bCheckUser = TRUE;
            CurrentHardDiskNumber = gCurrentHddIndex;
        } else if (gHddSecMasterPasswordBaseKey == Key) {
            bCheckUser = FALSE;
            CurrentHardDiskNumber = gCurrentHddIndex;
        } else {
            CurrentHardDiskNumber = 0xFFFF;
            LastHardDiskNumber = 0xFFFF;
        }
#endif

    // Not a valid HDD number
    if( CurrentHardDiskNumber == 0xFFFF ) {
        return EFI_NOT_FOUND;
    }

    // Just to make sure that two subsequent calls are for same HDD
    if(LastHardDiskNumber != 0xFFFF && LastHardDiskNumber != CurrentHardDiskNumber) {
        IsPasswordValidated = FALSE;
        LastHardDiskNumber = 0xFFFF;
        NoPasswordExistedOnFirstCall = FALSE;
        OldPasswordBuffer[0] = L'\0';
    }

    // Allocate memory for setup data
    if(NULL == gHddSecurityConfig) {
        Status = pBS->AllocatePool( EfiBootServicesData,
                                    sizeof(HDD_SECURITY_CONFIGURATION),
                                    (VOID**)&gHddSecurityConfig);
        if(EFI_ERROR(Status) || NULL == gHddSecurityConfig) {
            return EFI_NOT_FOUND;
        }
    }

    // Get setup/browser data
    size = sizeof(HDD_SECURITY_CONFIGURATION);
    Status = HiiLibGetBrowserData(  &size,
                                    gHddSecurityConfig,
                                    &gHddSecurityConfigurationGuid,
                                    L"HddSecDynamicSetup");
    if(EFI_ERROR(Status)) {
        return EFI_NOT_FOUND;
    }

#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
    IsSecurityEnabled = gHddSecurityConfig->Enabled[CurrentHardDiskNumber] ? TRUE : FALSE;
#else
    IsSecurityEnabled = gHddSecurityConfig->Enabled[0] ? TRUE : FALSE;
#endif

    // Get string from HII database using input string token
    if(CallbackParameters->Value->string != 0) {
        size = sizeof(PasswordBuffer);
        Status = HiiLibGetString(   HiiHandle,
                                    CallbackParameters->Value->string,
                                    &size,
                                    (EFI_STRING)PasswordBuffer);
        if(EFI_ERROR(Status)) {
            return EFI_NOT_FOUND;
        }
    }
    else {
        return EFI_NOT_FOUND;
    }

    // Return password status
    if (PasswordBuffer[0] == L'\0' && (IsPasswordValidated == FALSE) && (NoPasswordExistedOnFirstCall == FALSE)) {
        LastHardDiskNumber = CurrentHardDiskNumber;
        OldPasswordBuffer[0] = L'\0';
        if(IsSecurityEnabled == TRUE) {
            NoPasswordExistedOnFirstCall = FALSE;
            return EFI_NOT_FOUND;   // Pre-existing password
        }
        else {
            NoPasswordExistedOnFirstCall = TRUE;
            return EFI_SUCCESS;    // No pre-existing password.
        }
    }
    else if(IsSecurityEnabled == TRUE && IsPasswordValidated == FALSE) { // Validate password
        DataPtr = &gHddSecurityInternalData[CurrentHardDiskNumber];
        Status = HddSecurityAuthenticate( PasswordBuffer, DataPtr, bCheckUser ); //Authenticate it
        if(EFI_ERROR( Status )) {
            return EFI_NOT_READY;   // Password is wrong.
        }
        else {
            IsPasswordValidated = TRUE; // Set it so that password could be changed/cleared on next call
            Wcscpy(OldPasswordBuffer,PasswordBuffer); // Save existing HDD password
            return EFI_SUCCESS;    // Password is correct.
        }
    }
    else { // Set new password or clear old one
        // Reset static variables
        IsPasswordValidated = FALSE;
        NoPasswordExistedOnFirstCall = FALSE;

/* Commenting below code. Now Master password will be cleared on ESC when user password is not set
        // If existing password is equals to new password then do nothing
        // because of below code master password cannot be removed when security is disabled
        if(Wcscmp(OldPasswordBuffer,PasswordBuffer)==0) {
            OldPasswordBuffer[0] = L'\0';
            return EFI_SUCCESS;
        }
*/

        OldPasswordBuffer[0] = L'\0';

        // Set/Clear HDD password
        if(!HddSecurityUpdate( (UINT32)CurrentHardDiskNumber, PasswordBuffer, bCheckUser )) {
            if(bCheckUser) {
                return EFI_NOT_READY; // Not updated
            }
        }

        //Update NVRAM and browser data
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE && ALL_HDD_SAME_PW
        HddSecurityRefreshSetupData(0xFFFF);
#else
        HddSecurityRefreshSetupData(CurrentHardDiskNumber);
#endif
    }
    return EFI_SUCCESS;
}

/**
    This function will update security status of HDDs

    @param  EFI_HII_HANDLE HiiHandle - HII handle of formset
    @param  UINT16 Class             - class of formset
    @param  UINT16 SubClass          - subclass of formset
    @param  UINT16 Key               - Id of setup control

    @retval EFI_SUCCESS      - Operation finished
    @retval EFI_UNSUPPORTED  - Operation not supported on current event

**/
EFI_STATUS
HddSecurityDynamicSetupUpdateFormCallback(
        EFI_HII_HANDLE HiiHandle,
        UINT16         Class,
        UINT16         SubClass,
        UINT16         Key
)
{
    CALLBACK_PARAMETERS *CallbackParameters;

    // Get actual parameter passed by browser
    CallbackParameters = GetCallbackParameters();

    // Check for supported browser actions
    if(!CallbackParameters || ((CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING) && (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED) && (CallbackParameters->Action != EFI_BROWSER_ACTION_RETRIEVE)) )
        return EFI_UNSUPPORTED;

    // Always return success here
    if((CallbackParameters->Action == EFI_BROWSER_ACTION_CHANGED)) {
        return EFI_SUCCESS;
    }

    if((CallbackParameters->Action == EFI_BROWSER_ACTION_RETRIEVE)) {
        HddSecurityRefreshSetupData(gCurrentHddIndex);
        return EFI_SUCCESS;
    }

    // Check which HDD was selected
#if defined(SECURITY_SETUP_ON_SAME_PAGE) && SECURITY_SETUP_ON_SAME_PAGE
    gCurrentHddIndex = 0xFFFF;
#else
    if(gHddSecGotoBaseKey <= Key && Key < gHddSecGotoBaseKey + HDD_SECURITY_MAX_HDD_COUNT) {
        gCurrentHddIndex = Key - gHddSecGotoBaseKey;
    } else {
        gCurrentHddIndex = 0xFFFF;
    }
#endif

    //Update NVRAM and browser data
    HddSecurityRefreshSetupData(gCurrentHddIndex);

    return EFI_SUCCESS;
}

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
