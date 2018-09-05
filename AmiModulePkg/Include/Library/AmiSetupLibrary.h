//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  Function prototypes for modules that link into the Setup Module.
      These functions are to allow runtime registration of callbacks
      and string initializers
*/

#ifndef __AMI_SETUP_LIBRARY__H__
#define __AMI_SETUP_LIBRARY__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Uefi.h>
#include <Protocol/HiiConfigAccess.h>

/**
  Structure definition that matches the EFI_HII_ACCESS_FORM_CALLBACK parameters.
**/
typedef struct _AMI_HII_CALLBACK_PARAMETERS
{
    CONST EFI_HII_CONFIG_ACCESS_PROTOCOL    *This;
    EFI_BROWSER_ACTION                      Action;
    EFI_QUESTION_ID                         QuestionId;
    UINT8                                   Type;
    EFI_IFR_TYPE_VALUE                      *Value;
    EFI_BROWSER_ACTION_REQUEST              *ActionRequest;
    EFI_HII_HANDLE                          HiiHandle;
} AMI_HII_CALLBACK_PARAMETERS;

/**
  Function declaration of callback handler functions
**/
typedef EFI_STATUS (*AMI_SETUP_CALLBACK_FUNCTION) (
    AMI_HII_CALLBACK_PARAMETERS *CallbackValues
  );

/**
  Function declaration of string initialization functions
**/
typedef EFI_STATUS (*AMI_STRING_INIT_FUNCTION) (
    EFI_HII_HANDLE HiiHandle
  );


/**
  Register a callback handler to the callback list. The registered callback will be
  called when a HiiConfigAccess request occurs that matches the QuestionId. The
  EFI_HANDLE that is returend can be used to uniquely identify this callback function
  for unregistering.

  @param[in]  EFI_QUESTION_ID               Security measurement service handler to be registered.
  @param[in]  AMI_SETUP_CALLBACK_FUNCTION   Operation type is specified for the registered handler.
  @param[out] EFI_HANDLE                    Handle used to uniquely identify the registered callback.

  @retval EFI_SUCCESS              The handler was registered successfully.
**/
EFI_STATUS
AmiSetupRegisterCallbackHandler (
    IN EFI_QUESTION_ID              QuestionId,
    IN AMI_SETUP_CALLBACK_FUNCTION  CallbackFunction
  );

/**
  Unregister the callback handler refered to by the passed Handle.

  @param[in]  EFI_QUESTION_ID               Security measurement service handler to be registered.
  @param[in]  AMI_SETUP_CALLBACK_FUNCTION   Operation type is specified for the registered handler.
  @param[out] EFI_HANDLE                    Handle used to uniquely identify the registered callback.

  @retval EFI_SUCCESS              The handler was registered successfully.
**/
EFI_STATUS
AmiSetupUnRegisterCallbackHandler (
    IN AMI_SETUP_CALLBACK_FUNCTION CallbackFunction
  );


/**
  Register the String Initilization Function. This function will be called when it is determined that
  the strings in the HiiDatabase need to be updated with their runtime values. This function will called
  registered string initialization function prior to enterings setup so that strings referenced in setup
  can be updated with any runtime information.

  @param[in]  AMI_STRING_INIT_FUNCTION  Function pointer to the string initialization function

  @retval EFI_SUCCESS              The handler was registered successfully.
**/
EFI_STATUS
AmiSetupRegisterStringInitializer (
    AMI_STRING_INIT_FUNCTION StringInitFunction
  );

#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
