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

/** @file NvmeComponentName.c
    Provides Controller and device Name information 

**/

//---------------------------------------------------------------------------

#include "NvmeIncludes.h"
#include "NvmeBus.h"

//---------------------------------------------------------------------------

extern  EFI_DRIVER_BINDING_PROTOCOL gNvmeBusDriverBinding;

EFI_STATUS 
EFIAPI
NvmeBusCtlDriverName (
    IN  EFI_COMPONENT_NAME2_PROTOCOL    *This,
    IN  CHAR8                           *Language,
    OUT CHAR16                          **DriverName
);

EFI_STATUS
EFIAPI
NvmeBusCtlGetControllerName (
    IN  EFI_COMPONENT_NAME2_PROTOCOL    *This,
    IN  EFI_HANDLE                      ControllerHandle,
    IN  EFI_HANDLE                      ChildHandle     OPTIONAL,
    IN  CHAR8                           *Language,
    OUT CHAR16                          **ControllerName
);

CHAR16 *gNvmeBusDriverName = L"AMI NVMe BUS Driver";
CHAR16 *gNvmeBusControllerName = L"NVMe Mass Storage Controller";
EFI_COMPONENT_NAME2_PROTOCOL gNvmeBusControllerDriverName = {
    NvmeBusCtlDriverName,
    NvmeBusCtlGetControllerName,
    LANGUAGE_CODE_ENGLISH
};

/**
    Retrieves a Unicode string that is the user readable name of 
    the EFI Driver.

    @param    This - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
    @param    Language   - A pointer to a three character ISO 639-2 language 
              identifier.This is the language of the driver name that that 
              the caller is requesting, and it must match one of the 
              languages specified in SupportedLanguages.The number of 
              languages supported by a driver is up to the driver writer.
    @param    DriverName - A pointer to the Unicode string to return. This 
              Unicode string is the name of the driver specified by This in 
              the language specified by Language.

    @retval EFI_SUCCES The Unicode string for the Driver specified by This
            and the language specified by Language was returned
            in DriverName.
    @retval EFI_INVALID_PARAMETER Language is NULL.
    @retval EFI_INVALID_PARAMETER DriverName is NULL.
    @retval EFI_UNSUPPORTED The driver specified by This does not support the
            language specified by Language.

**/

EFI_STATUS
EFIAPI
NvmeBusCtlDriverName (
    IN  EFI_COMPONENT_NAME2_PROTOCOL    *This,
    IN  CHAR8                           *Language,
    OUT CHAR16                          **DriverName
)
{
    //
    //Supports only English
    //
    if(!Language || !DriverName) return EFI_INVALID_PARAMETER;
    if (AsciiStrCmp( Language, LANGUAGE_CODE_ENGLISH)) return EFI_UNSUPPORTED;
    *DriverName = gNvmeBusDriverName;
    return EFI_SUCCESS;
}

/**
    Retrieves a Unicode string that is the user readable name of
    the controller that is being managed by an EFI Driver.

    @param  This    - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
    @param  ControllerHandle - The handle of a controller that the driver 
            specified by This is managing.  This handle specifies the 
            controller whose name is to be returned.
    @param  ChildHandle      - The handle of the child controller to retrieve 
            the name of.  This is an optional parameter that may be NULL.It
            will be NULL for device drivers.  It will also be NULL
            for a bus drivers that wish to retrieve the name of the
            bus controller.  It will not be NULL for a bus driver
            that wishes to retrieve the name of a child controller.
    @param  Language    - A pointer to a three character ISO 639-2 language
            identifier.  This is the language of the controller name
            that that the caller is requesting, and it must match one
            of the languages specified in SupportedLanguages.  The
            number of languages supported by a driver is up to the
            driver writer.
    @param  ControllerName   - A pointer to the Unicode string to return.This 
            Unicode string is the name of the controller specified by
            ControllerHandle and ChildHandle in the language
            specified by Language from the point of view of the
            driver specified by This.

    @retval EFI_SUCCESS The Unicode string for the user readable name in the
            language specified by Language for the driver
            specified by This was returned in DriverName.
    @retval EFI_INVALID_PARAMETER ControllerHandle is not a valid EFI_HANDLE.
    @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a 
            valid EFI_HANDLE.
    @retval EFI_INVALID_PARAMETER Language is NULL.
    @retval EFI_INVALID_PARAMETER ControllerName is NULL.
    @retval EFI_UNSUPPORTED The driver specified by This is not currently
            managing the controller specified by ControllerHandle and ChildHandle.
    @retval EFI_UNSUPPORTED The driver specified by This does not support the
            language specified by Language.

**/

EFI_STATUS
EFIAPI
NvmeBusCtlGetControllerName (
    IN  EFI_COMPONENT_NAME2_PROTOCOL    *This,
    IN  EFI_HANDLE                      Controller,
    IN  EFI_HANDLE                      ChildHandle OPTIONAL,
    IN  CHAR8                           *Language,
    OUT CHAR16                          **ControllerName
)
{

    EFI_STATUS                      Status;
    AMI_NVME_CONTROLLER_PROTOCOL    *NvmeController;
    ACTIVE_NAMESPACE_DATA           *ActiveNameSpace;
    LIST_ENTRY                      *LinkData;

    // Check if gAmiNvmeControllerProtocolGuid is installed on the device
    Status = gBS->OpenProtocol( Controller,
                    &gAmiNvmeControllerProtocolGuid,
                    (VOID **)&NvmeController,
                    gNvmeBusDriverBinding.DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    
    if (Status != EFI_SUCCESS && Status != EFI_ALREADY_STARTED) {
        return EFI_UNSUPPORTED;
    }

    //
    //Supports only "en-US" 
    //
    if(!Language || !ControllerName || !Controller) return EFI_INVALID_PARAMETER;
    if (AsciiStrCmp( Language, LANGUAGE_CODE_ENGLISH)) return EFI_UNSUPPORTED;

    if (ChildHandle == NULL) {
        *ControllerName = gNvmeBusControllerName;
        return EFI_SUCCESS;
    } else {
          for (LinkData = NvmeController->ActiveNameSpaceList.ForwardLink; \
              LinkData != &NvmeController->ActiveNameSpaceList; 
              LinkData = LinkData->ForwardLink) {
            
              ActiveNameSpace = BASE_CR(LinkData ,ACTIVE_NAMESPACE_DATA, Link);

              if (ActiveNameSpace->NvmeDeviceHandle == ChildHandle){
                  *ControllerName = NvmeController->UDeviceName->UnicodeString;
                  return EFI_SUCCESS;
                
              }
          }
    }
    
    return EFI_UNSUPPORTED;
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
