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

/** @file ComponentName.c
    AMI USB Component Name definitions

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "Uhcd.h"
#include "ComponentName.h"
#include <Protocol/ComponentName2.h>
                                      
static NAME_SERVICE_T cn_template = {
  AMIUSBComponentNameGetDriverName,
  AMIUSBComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};

/**
    USB device and controller names initializer.

**/

EFI_COMPONENT_NAME2_PROTOCOL*
InitNamesStatic(
    NAME_SERVICE_T* NameService,
    CHAR16* Driver,
    CHAR16* Component
)
{
    *NameService  = cn_template;
    NameService->driver_name = Driver;
    NameService->component_static = Component;
    NameService->component_cb = 0;
    return &NameService->icn;
}


/**
    Component names protocol initializer

**/

EFI_COMPONENT_NAME2_PROTOCOL*
InitNamesProtocol(
    NAME_SERVICE_T* NameService,
    CHAR16* Driver,
    COMPONENT_CB_T ComponentCb
)
{
    *NameService  = cn_template;
    NameService->driver_name = Driver;
    NameService->component_static = 0;
    NameService->component_cb = ComponentCb;
    return &NameService->icn;
}


/**
    Retrieves a Unicode string that is the user readable name of
    the EFI Driver.


    @param 
        This       - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
        Language   - A pointer to a three character ISO 639-2 language identifier.
        This is the language of the driver name that that the caller
        is requesting, and it must match one of the languages specified
        in SupportedLanguages.  The number of languages supported by a
        driver is up to the driver writer.
        DriverName - A pointer to the Unicode string to return.  This Unicode string
        is the name of the driver specified by This in the language
        specified by Language.

         
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
AMIUSBComponentNameGetDriverName (
  EFI_COMPONENT_NAME2_PROTOCOL  *ComponentNameProtocol,
  CHAR8                        *Language,
  CHAR16                       **DriverName
  )
{
    
    NAME_SERVICE_T* This = (NAME_SERVICE_T*)ComponentNameProtocol;
                                        //(EIP59272)>
    if(!Language || !DriverName) {
        return EFI_INVALID_PARAMETER;
    }
    if ( !LanguageCodesEqual( Language, LANGUAGE_CODE_ENGLISH) ) {
        return EFI_UNSUPPORTED;
    }
                                        //<(EIP59272)
    *DriverName = This->driver_name;
    return  EFI_SUCCESS;
}


/**
    Retrieves a Unicode string that is the user readable name of
    the controller that is being managed by an EFI Driver.

    @param 
        This             - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
        ControllerHandle - The handle of a controller that the driver specified by
        This is managing.  This handle specifies the controller
        whose name is to be returned.
        ChildHandle      - The handle of the child controller to retrieve the name
        of.  This is an optional parameter that may be NULL.  It
        will be NULL for device drivers.  It will also be NULL
        for a bus drivers that wish to retrieve the name of the
        bus controller.  It will not be NULL for a bus driver
        that wishes to retrieve the name of a child controller.
        Language         - A pointer to a three character ISO 639-2 language
        identifier.  This is the language of the controller name
        that that the caller is requesting, and it must match one
        of the languages specified in SupportedLanguages.  The
        number of languages supported by a driver is up to the
        driver writer.
        ControllerName   - A pointer to the Unicode string to return.  This Unicode
        string is the name of the controller specified by
        ControllerHandle and ChildHandle in the language
        specified by Language from the point of view of the
        driver specified by This.

         
    @retval EFI_SUCCESS The Unicode string for the user readable name in the
        language specified by Language for the driver
        specified by This was returned in DriverName.
    @retval EFI_INVALID_PARAMETER ControllerHandle is not a valid EFI_HANDLE.
    @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid
        EFI_HANDLE.
    @retval EFI_INVALID_PARAMETER Language is NULL.
    @retval EFI_INVALID_PARAMETER ControllerName is NULL.
    @retval EFI_UNSUPPORTED The driver specified by This is not currently
        managing the controller specified by
        ControllerHandle and ChildHandle.
    @retval EFI_UNSUPPORTED The driver specified by This does not support the
        language specified by Language.

**/

EFI_STATUS
EFIAPI
AMIUSBComponentNameGetControllerName (
  EFI_COMPONENT_NAME2_PROTOCOL  *ComponentNameProtocol,
  EFI_HANDLE                   Controller,
  EFI_HANDLE                   Child,
  CHAR8                        *Language,
  CHAR16                       **ControllerName
  )
{
    NAME_SERVICE_T* This = (NAME_SERVICE_T*)ComponentNameProtocol;
                                        //(EIP59272)>
    if(!Language || !ControllerName || !Controller) {	//(EIP69250)
        return EFI_INVALID_PARAMETER;
    }
    if ( !LanguageCodesEqual( Language, LANGUAGE_CODE_ENGLISH) ) {
        return EFI_UNSUPPORTED;
    }
                                        //<(EIP59272)
    if( This->component_cb != 0){
        CHAR16 *Str = This->component_cb(Controller, Child);
        if(Str==0) return EFI_UNSUPPORTED;
        *ControllerName = Str;
    } else {
        if (Child != NULL) {
            return EFI_UNSUPPORTED;
        }
        *ControllerName = This->component_static;
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
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
