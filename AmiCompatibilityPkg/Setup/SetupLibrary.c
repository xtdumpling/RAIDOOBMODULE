//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
    This file contains the functionality for the "SetupLibrary". These
    functions are to enable the runtime registration of items as opposed
    to the build time.
*/
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/AmiSetupLibrary.h>
#include <SetupCallbackList.h>

extern LIST_ENTRY AmiCallbackList;
extern LIST_ENTRY AmiStringInitializationList;
extern LIST_ENTRY AmiExtractConfigList;
extern LIST_ENTRY AmiRouteConfigList;

/**
  Register a callback handler to the callback list. The registered callback will be
  called when a HiiConfigAccess request occurs that matches the QuestionId. 

  @param[in]  QuestionId Id associated with the callback function
  @param[in]  CallbackFunction Callback function to call

  @return EFI_STATUS 
  @retval EFI_SUCCESS The handler was registered successfully.
  @retval EFI_INVALID_PARAMETER The Callback function was NULL
  @retval EFI_UNSUPPORTED A calback function was already registered for the QuestionId
**/
EFI_STATUS
AmiSetupRegisterCallbackHandler (
    IN EFI_QUESTION_ID              QuestionId,
    IN AMI_SETUP_CALLBACK_FUNCTION  CallbackFunction
  )
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    AMI_CALLBACK_LIST_ENTRY *NewEntry = NULL;

    
    if(CallbackFunction != NULL)
    {
        AMI_CALLBACK_LIST_ENTRY *NextLink = NULL;
        Status = EFI_SUCCESS;
        
        for(NextLink = (AMI_CALLBACK_LIST_ENTRY*)GetFirstNode(&AmiCallbackList);
            !IsNull(&AmiCallbackList, &NextLink->Link); 
            NextLink = (AMI_CALLBACK_LIST_ENTRY*)GetNextNode(&AmiCallbackList, &NextLink->Link)
        ){
            if(NextLink->QuestionId == QuestionId)
            {
                Status = EFI_UNSUPPORTED;
                break;
            }
        }

        if(!EFI_ERROR(Status))
        {
            Status = gBS->AllocatePool(EfiBootServicesData, sizeof(AMI_CALLBACK_LIST_ENTRY), (VOID**)&NewEntry);
            if(!EFI_ERROR(Status))
            {
                NewEntry->QuestionId = QuestionId;
                NewEntry->Function = CallbackFunction;
                InsertTailList((LIST_ENTRY*)&AmiCallbackList, (LIST_ENTRY*)&(NewEntry->Link));
            }
        }
    }
    return Status;
}

/**
  Unregistered the callback handler that uses the passed CallbackFunction.

  @param[in]  CallbackFunction The callback function to remove from the callback list

  @return EFI_STATUS
  @retval EFI_INVALID_PARAMETER The Callback function was NULL
  @retval EFI_SUCCESS The handler was registered successfully.
  @retval EFI_NOT_FOUND The Handle was invalid
**/
EFI_STATUS
AmiSetupUnRegisterCallbackHandler (
    IN AMI_SETUP_CALLBACK_FUNCTION  CallbackFunction
  )
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    AMI_CALLBACK_LIST_ENTRY *TempLink = NULL;

    if(CallbackFunction != NULL)
    {
        AMI_CALLBACK_LIST_ENTRY *NextLink = NULL;
        Status = EFI_NOT_FOUND;
        for(NextLink = (AMI_CALLBACK_LIST_ENTRY*)GetFirstNode(&AmiCallbackList), 
            TempLink = (AMI_CALLBACK_LIST_ENTRY*)GetNextNode(&AmiCallbackList, &NextLink->Link); 
            !IsNull(&AmiCallbackList, &NextLink->Link); 
            NextLink = TempLink, TempLink = (AMI_CALLBACK_LIST_ENTRY*)GetNextNode(&AmiCallbackList, &NextLink->Link)) 
        {
            if(NextLink->Function == CallbackFunction)
            {
                RemoveEntryList((LIST_ENTRY*)&(NextLink->Link));
                gBS->FreePool(NextLink);
                Status = EFI_SUCCESS;
            }
        }
    }

    return Status;
}

/**
  Register the String Initialization Function. This function will be called when it is determined that
  the strings in the HiiDatabase need to be updated with their runtime values. This function will called
  registered string initialization function prior to entering setup so that strings referenced in setup
  can be updated with any runtime information.

  @param[in] StringInitFunction Function pointer to the string initialization function

  @return EFI_STATUS
  @retval EFI_INVALID_PARAMETER The StringInitFunction was NULL
  @retval EFI_SUCCESS The StringInitFunction was registered
**/
EFI_STATUS
AmiSetupRegisterStringInitializer (
    AMI_STRING_INIT_FUNCTION StringInitFunction
  )
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    AMI_STRING_INITIALIZATION_ENTRY *NewEntry = NULL;

    if(StringInitFunction != NULL)
    {
        Status = gBS->AllocatePool(EfiBootServicesData, sizeof(AMI_STRING_INITIALIZATION_ENTRY), (VOID**)&NewEntry);
        if(!EFI_ERROR(Status))
        {
            NewEntry->Function = StringInitFunction;

            InsertTailList((LIST_ENTRY*)&AmiStringInitializationList, (LIST_ENTRY*)&(NewEntry->Link));
        }
    }
    return Status;
}

/**
  Register into the internal structures the passed function into the list of functions that are called
  through the Hii Access Protocol's ExtractConfig function.
  
  @param[in]  ExtractConfigFunction  Function pointer to the extract config function

  @return EFI_STATUS
  @return EFI_INVALID_PARAMETER The ExtractConfigFunction was NULL
  @retval EFI_SUCCESS  The handler was registered successfully.
**/
EFI_STATUS
AmiSetupRegisterExtractConfig (
    EFI_HII_ACCESS_EXTRACT_CONFIG ExtractConfigFunction
  )
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    AMI_EXTRACT_CONFIG_ENTRY *NewEntry = NULL;

    if(ExtractConfigFunction != NULL)
    {
        Status = gBS->AllocatePool(EfiBootServicesData, sizeof(AMI_EXTRACT_CONFIG_ENTRY), (VOID**)&NewEntry);
        if(!EFI_ERROR(Status))
        {
            NewEntry->Function = ExtractConfigFunction;
            InsertTailList((LIST_ENTRY*)&AmiExtractConfigList, (LIST_ENTRY*)&(NewEntry->Link));
        }
    }
    return Status;
}

/**
  Unregister from the internal structures passed function from the list of functions that are called
  through the Hii Access Protocol's ExtractConfig function.
  
  @param[in]  ExtractConfigFunction  Function pointer to the string initialization function

  @retval EFI_SUCCESS The handler was unregistered successfully.
  @retval EFI_INVALID_PARAMETER The FunctionPointer was NULL
  @retval EFI_NOT_FOUND The function did not exist in the list
**/
EFI_STATUS
AmiSetupUnRegisterExtractConfig (
    EFI_HII_ACCESS_EXTRACT_CONFIG ExtractConfigFunction
  )
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    if(ExtractConfigFunction != NULL)
    {
        AMI_EXTRACT_CONFIG_ENTRY *NextLink = NULL;
        Status = EFI_NOT_FOUND;
        for(NextLink = (AMI_EXTRACT_CONFIG_ENTRY*)GetFirstNode(&AmiExtractConfigList);
            !IsNull(&AmiExtractConfigList, &NextLink->Link); 
            NextLink = (AMI_EXTRACT_CONFIG_ENTRY*)GetNextNode(&AmiExtractConfigList, &NextLink->Link)) 

        {
            if(NextLink->Function == ExtractConfigFunction)
            {
                RemoveEntryList((LIST_ENTRY*)&(NextLink->Link));
                gBS->FreePool(NextLink);
                Status = EFI_SUCCESS;
                break;
            }
        }
    }
    return Status;
}

/**
  Register into the internal structures the passed function into the list of functions that are called
  through the Hii Access Protocol's RouteConfig function.
  
  @param[in] AccessRouteConfigFunction Function pointer to the AccessRouteConfig 

  @return EFI_STATUS
  @retval EFI_INVALID_PARAMETER The AccessRouteConfigFunction was NULL
  @retval EFI_SUCCESS The AccessRouteConfigFunction was registered
**/
EFI_STATUS
AmiSetupRegisterAccessRouteConfig (
    EFI_HII_ACCESS_ROUTE_CONFIG AccessRouteConfigFunction
  )
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    AMI_ROUTE_CONFIG_ENTRY *NewEntry = NULL;

    if(AccessRouteConfigFunction != NULL)
    {
        Status = gBS->AllocatePool(EfiBootServicesData, sizeof(AMI_ROUTE_CONFIG_ENTRY), (VOID**)&NewEntry);
        if(!EFI_ERROR(Status))
        {
            NewEntry->Function = AccessRouteConfigFunction;
            InsertTailList((LIST_ENTRY*)&AmiRouteConfigList, (LIST_ENTRY*)&(NewEntry->Link));
        }
    }
    return Status;
}

/**
  Unregister from the internal structures passed function from the list of functions that are called
  through the Hii Access Protocol's RouteConfig function.
  
  @param[in] AccessRouteConfigFunction Function pointer to the AccessRouteConfig 

  @return EFI_STATUS
  @retval EFI_INVALID_PARAMETER The AccessRouteConfigFunction was NULL
  @retval EFI_NOT_FOUND The function was not found
  @retval EFI_SUCCESS The AccessRouteConfigFunction was unregistered
**/
EFI_STATUS
AmiSetupUnRegisterAccessRouteConfig (
    EFI_HII_ACCESS_ROUTE_CONFIG AccessRouteConfigFunction
  )
{
    EFI_STATUS Status = EFI_INVALID_PARAMETER;
    if(AccessRouteConfigFunction != NULL)
    {
        AMI_ROUTE_CONFIG_ENTRY *NextLink = NULL;
        Status = EFI_NOT_FOUND;
        for(NextLink = (AMI_ROUTE_CONFIG_ENTRY*)GetFirstNode(&AmiRouteConfigList);
            !IsNull(&AmiRouteConfigList, &NextLink->Link); 
            NextLink = (AMI_ROUTE_CONFIG_ENTRY*)GetNextNode(&AmiRouteConfigList, &NextLink->Link)) 
        {
            if(NextLink->Function == AccessRouteConfigFunction)
            {
                RemoveEntryList((LIST_ENTRY*)&(NextLink->Link));
                gBS->FreePool(NextLink);
                Status = EFI_SUCCESS;
                break;
            }
        }
    }
    return Status;
 }
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
