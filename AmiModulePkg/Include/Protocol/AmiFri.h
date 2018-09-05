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

/** @file
    AFRI Protocol Definition.
**/

#ifndef __AMI_FRI_PROTOCOL_H__
#define __AMI_FRI_PROTOCOL_H__

#include <PiSmm.h>

#define AMI_FRI_PROTOCOL_GUID \
    { 0xbef4d2b1, 0xc53b, 0x4933, { 0xa3, 0xba, 0x2a, 0x42, 0x2c, 0x89, 0xa4, 0x39 } }

typedef struct _AMI_FRI_PROTOCOL  AMI_FRI_PROTOCOL;

/**
  Registers AFRI handler.

  @param  Handler        Handler function.
  @param  HandlerType    Handler type.
  @param  DispatchHandle On return, contains a unique handle which can be used to later unregister the handler function.

  @retval EFI_SUCCESS           Handler has beedn successfully registered.
  @retval EFI_INVALID_PARAMETER Handler, HandlerType or DispatchHandle is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *AMI_FRI_HANDLER_REGISTER)(
    IN  EFI_SMM_HANDLER_ENTRY_POINT2 Handler,
    IN  CONST EFI_GUID              *HandlerType,
    OUT EFI_HANDLE                  *DispatchHandle
);

/**
  Unregister a AFRI handler.

  @param  DispatchHandle  The handle that was returned when the handler was registered.

  @retval EFI_SUCCESS           Handler function was successfully unregistered.
  @retval EFI_INVALID_PARAMETER DispatchHandle does not refer to a valid handle.
**/
typedef
EFI_STATUS
    (EFIAPI * AMI_FRI_HANDLER_UNREGISTER)(
    IN EFI_HANDLE DispatchHandle
);

struct _AMI_FRI_PROTOCOL {
    AMI_FRI_HANDLER_REGISTER   RegisterAfriHandler;
    AMI_FRI_HANDLER_UNREGISTER UnregisterAfriHandler;
};

extern EFI_GUID gAmiFriProtocolGuid;

#endif
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
