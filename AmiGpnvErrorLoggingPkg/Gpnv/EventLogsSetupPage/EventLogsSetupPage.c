//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file EventLogsSetupPage.c
    EventLogsSetupPage driver Implementation

**/

//---------------------------------------------------------------------------

#include <AmiDxeLib.h>
#include "EventLogsSetupPage.h"
#include <Protocol/EventLogsSetupProtocols.h>
#include <Protocol/AmiSmbios.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>

//---------------------------------------------------------------------------

#define DEBUG_ERROR     0x80000000

EFI_GUID gEfiSetupVariableGuid = SETUP_GUID;
EFI_GUID gErrorMngrGuid = ERROR_MANAGER_GUID;
EFI_GUID gAmiEventLogsFormsetGuid;

EFI_STATUS
CallbackFunction (
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest );

EVENT_LOGS_SETUP_HII_HANDLE_PROTOCOL  HiiHandleProtocolx;
EVENT_LOGS_SETUP_ADDED_DYNAMIC_SCREEN_PROTOCOL*  gDynamicCallbackInfo = NULL;
EFI_HII_CONFIG_ACCESS_PROTOCOL CallBack = { NULL,NULL,CallbackFunction };

CALLBACK_INFO SetupCallBack[] = {
// Last field in every structure will be filled by the Setup
        { &gAmiEventLogsFormsetGuid, &CallBack, EVENT_LOGS_FORM_SET_CLASS, 0, 0 },
    };
BOOLEAN IgnoreNext0xFFFEKeyValue = FALSE;


/**

    InitEventLogsStrings Run with HiiHandle

    @param HiiHandle - Handle that indicates which HII Package
                       that is being used
    @param pCallBackFound - Pointer to an instance of CALLBACK_INFO
                            that works with HiiHandle

    @return VOID

**/


VOID
InitEventLogsStrings (
     EFI_HII_HANDLE HiiHandle,
     CALLBACK_INFO *pCallBackFound )
{
  EFI_STATUS Status;
  EFI_HANDLE TempHandlex;

  if (pCallBackFound->Class == EVENT_LOGS_FORM_SET_CLASS) {
    HiiHandleProtocolx.HiiHandle = HiiHandle;

    HiiHandleProtocolx.BufferPtr = AllocateZeroPool(0x10000);
    (HiiHandleProtocolx.BufferPtr)->ErrorCount = 0;
    (HiiHandleProtocolx.BufferPtr)->ErrorFunction = 0x0000;

    TempHandlex = NULL;
     Status = gBS->InstallProtocolInterface (
                      &TempHandlex,
                      &gAmiEventLogsHiiHandleGuid,
                      EFI_NATIVE_INTERFACE,
                      &HiiHandleProtocolx 
                      );

     ASSERT_EFI_ERROR (Status);

  }

}





/**

    This is the Callback called by Setup engine. Displays logs

    @param This - Pointer to the instance of ConfigAccess Protocol
    @param Action - Action, that setup browser is performing
    @param KeyValue - A unique value which identifies control that caused 
                      callback.
    @param Type - Value type of setup control data
    @param Value - Pointer to setup control data
    @param ActionRequest - Pointer where to store requested action

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/


EFI_STATUS
CallbackFunction (
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest )

{
    EFI_STATUS Status;
    UINT16   ViewMoreRecordKeyValue=0;
    UINTN    Size = sizeof(ViewMoreRecordKeyValue);
    UINT32   AttributesRead = EFI_VARIABLE_BOOTSERVICE_ACCESS;
    UINTN    DummySize = 1;

    if ( (KeyValue == 0xFFFE)  &&  (IgnoreNext0xFFFEKeyValue == TRUE) ) {
        IgnoreNext0xFFFEKeyValue = FALSE;
        return EFI_SUCCESS;
     }

// Initialize all dynamic information

     (HiiHandleProtocolx.BufferPtr)->ErrorCount = 0;
     (HiiHandleProtocolx.BufferPtr)->ErrorFunction = 0xFFFF;

// Find all dynamic logs to be added

    if (gDynamicCallbackInfo == NULL) {

    Status = gBS->LocateProtocol( &gAmiEventLogsDynamicGuid, NULL, \
                                       (VOID **)&gDynamicCallbackInfo );
         if(EFI_ERROR(Status))
       {
            return Status;
        }

         if ( (Status != EFI_SUCCESS) || (gDynamicCallbackInfo == NULL) ) {

// Read the Attributes of ErrorManager Variable before performing
// any operation, in order to restore the same Attributes.

        Status = pRS->GetVariable (
                   L"ErrorManager",
                   &gErrorMngrGuid,
                   &AttributesRead,
                   &DummySize,
                   (HiiHandleProtocolx.BufferPtr)
                   );

        if(Status == EFI_NOT_FOUND){
          AttributesRead = EFI_VARIABLE_BOOTSERVICE_ACCESS;
        }

// Submit Empty Dynamic Information Area and exit.No Logs to Show.

        gRT->SetVariable (
             L"ErrorManager",
             &gErrorMngrGuid,
             AttributesRead,
             (sizeof(ERROR_MANAGER)+((HiiHandleProtocolx.BufferPtr)->ErrorCount)*sizeof(ERROR_LIST)),
             (HiiHandleProtocolx.BufferPtr)
             );
        return EFI_SUCCESS;
         }
     }

// Get "SmbiosMoreRecords" variable to get the Key value stored when Event
// log space is filled more than MAX_ERROR_MANAGER_NVRAM_VARIABLE_SIZE

    Status = gRT->GetVariable (
                    L"SmbiosMoreRecords",
                    &gErrorMngrGuid,
                    NULL,
                    &Size,
                    &ViewMoreRecordKeyValue
                    );

// In error case, do not do anything. Just check next case
// If variable is found, check for ViewMoreRecordKeyValue with KeyValue
// obtained from Setup Callback function

    if ( !EFI_ERROR(Status) && (ViewMoreRecordKeyValue == KeyValue) ) {

// If the condition is met,Use CallbackFunction of it.

            gDynamicCallbackInfo->CallbackFunction( This, 
                                                    Action, 
                                                    KeyValue, 
                                                    Type, 
                                                    Value, 
                                                    ActionRequest 
                                                   );

            IgnoreNext0xFFFEKeyValue = TRUE;
   } else if((KeyValue == SMBIOS_VIEW_FORM_KEY)) {

// If the KeyValue is SMBIOS_VIEW_FORM_KEY,call the callback function

        gDynamicCallbackInfo->CallbackFunction( This, 
                                                Action, 
                                                KeyValue, 
                                                Type, 
                                                Value, 
                                                ActionRequest 
                                               );

        IgnoreNext0xFFFEKeyValue = TRUE;
     }else {

        return EFI_SUCCESS;

      }


     return EFI_SUCCESS;
}

/**

    This is the standard EFI driver entry point called for EventLogsSetup 
    Page driver

    @param ImageHandle - Image handle for this driver image
    @param SystemTable - Pointer to the EFI System Table

    @return EFI_STATUS
    @retval EFI_SUCCESS

**/

EFI_STATUS
EFIAPI
EventLogsSetupPageEntry (IN EFI_HANDLE ImageHandle,
                         IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS Status;
    InitAmiLib(ImageHandle, SystemTable);
    Status = LoadResources( ImageHandle,
                            sizeof(SetupCallBack)/sizeof(CALLBACK_INFO), 
                            SetupCallBack, 
                            InitEventLogsStrings 
                          );

    if(EFI_ERROR(Status)) {
        return Status;
    }
    return EFI_SUCCESS;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
