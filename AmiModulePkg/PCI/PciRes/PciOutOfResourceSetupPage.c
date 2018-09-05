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
//#**********************************************************************
/** @file PciOutOfResourceSetupPage.c
    Pci Bus Out of resource condition Setup Page and handling routines

    @note  Use this hook to dynamically change PCI BUS Driver Out of resource Setup screen Strings.

**/
#include "PciOutOfResourceSetupPage.h"
#include <AcpiRes.h>

EFI_GUID guidPciOutOfResource = PCI_OUT_OF_RESOURCE_FORM_SET_GUID;
EFI_HANDLE ThisImageHandle;

#define EFI_BDS_EVENT_BEFORE_SETUP_GUID \
  { 0xdbc9fd21, 0xfad8, 0x45b0, 0x9e, 0x78, 0x27, 0x15, 0x88, 0x67, 0xcc, 0x93 };
VOID EFIAPI CheckPciOutOfResourcesCondition(IN EFI_EVENT Event, IN VOID *Context);

#if EFI_SPECIFICATION_VERSION>0x20000

EFI_STATUS EFIAPI PciOutOfResourcePage(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
);

EFI_HII_CONFIG_ACCESS_PROTOCOL CallBack = {NULL,NULL,PciOutOfResourcePage};
#else

EFI_STATUS EFIAPI PciOutOfResourcePage(
    IN EFI_FORM_CALLBACK_PROTOCOL   *This,
    IN UINT16 KeyValue,
    IN EFI_IFR_DATA_ARRAY           *Data,
    OUT EFI_HII_CALLBACK_PACKET      **Packet
);

EFI_FORM_CALLBACK_PROTOCOL CallBack = {NULL,NULL,PciOutOfResourcePage};
#endif

CALLBACK_INFO SetupCallBack[] =
{
    // Last field in every structure will be filled by the Setup
    { &guidPciOutOfResource, &CallBack, PCI_OUT_OF_RESOURCE_FORM_SET_CLASS, 0, 0},
};

EFI_HII_HANDLE PciOutOfResourceSetupPageHiiHandle = NULL;
/**
    This is the Callback called by setup engine. It is prosseds with Error Message.

    @param This Instance of Callback Protocol
    @param KeyValue A unique value which identifies control that caused callback.
    @param Data A pointer to the data being sent.
    @param Packet Double pointer to where data will be stored

    @retval EFI_STATUS

**/

#if EFI_SPECIFICATION_VERSION>0x20000
EFI_STATUS EFIAPI PciOutOfResourcePage(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest
)

#else
EFI_STATUS EFIAPI PciOutOfResourcePage(
    IN EFI_FORM_CALLBACK_PROTOCOL   *This,
    IN UINT16 KeyValue,
    IN EFI_IFR_DATA_ARRAY           *Data,
    OUT EFI_HII_CALLBACK_PACKET      **Packet
)

#endif

{

   EFI_STATUS Status    = EFI_SUCCESS;
   return Status;

}

VOID EFIAPI CheckPciOutOfResourcesCondition(IN EFI_EVENT Event, IN VOID *Context)
{
    EFI_STATUS              Status;
//------------------------------------
    
    Status=AmiPciOutOfRes(NULL, TRUE);
    
    if(!EFI_ERROR(Status)){
        LoadResources(ThisImageHandle,sizeof(SetupCallBack)/sizeof(CALLBACK_INFO), SetupCallBack, NULL);
    }

    return ;
}

/**
    This is the standard EFI driver entry point called for
    PciOutOfResourceSetupPage module initlaization
    @param ImageHandle ImageHandle of the loaded driver
    @param SystemTable Pointer to the System Table

    @retval EFI_SUCCESS

**/
EFI_STATUS EFIAPI PciOutOfResourceSetupPageEntry (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    static EFI_EVENT       SetupEnterEvent;
    VOID                   *pSetupRegistration;
    static EFI_GUID        guidSetupEnter = EFI_BDS_EVENT_BEFORE_SETUP_GUID;//AMITSE_SETUP_ENTER_GUID
//----------------------------
    ThisImageHandle = ImageHandle;
    InitAmiLib(ImageHandle,SystemTable);

    //---Set up Callback----------------
    RegisterProtocolCallback(&guidSetupEnter,CheckPciOutOfResourcesCondition,
                                NULL,&SetupEnterEvent, &pSetupRegistration);

    return EFI_SUCCESS;
}
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
