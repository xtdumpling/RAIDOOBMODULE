//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  Instance of SmmCorePlatformHookLib for saving and restoring register context.
**/

#include <AmiSmmCorePlatformHooks.h>

/**
  Performs platform specific tasks before invoking registered SMI handlers.
  
  This function performs platform specific tasks before invoking registered SMI handlers.
  
  @retval EFI_SUCCESS       The platform hook completes successfully.
  @retval Other values      The platform hook cannot complete due to some error.

**/
EFI_STATUS EFIAPI PlatformHookBeforeSmmDispatch (VOID){
    INT32 i;

    for(i = 0; i<NumberOfHookFunctions; ++i) BeforeAfterSmmDispatchHookFunction[i](TRUE);
    return EFI_SUCCESS;
}


/**
  Performs platform specific tasks after invoking registered SMI handlers.
  
  This function performs platform specific tasks after invoking registered SMI handlers.
  
  @retval EFI_SUCCESS       The platform hook completes successfully.
  @retval Other values      The platform hook cannot complete due to some error.

**/
EFI_STATUS EFIAPI PlatformHookAfterSmmDispatch (VOID){
    INT32 i;

    for (i=NumberOfHookFunctions-1; i >= 0; --i) BeforeAfterSmmDispatchHookFunction[i](FALSE); //Restore in reverse order.
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**             5555 Oakbrook Pkwy, Norcross, GA 30093               **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
