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

/** @file Debug.c
    AMI USB Debug output implementation routnes

**/

#include "AmiDef.h"

/**
    This routine prints the debug message

    @param Variable

    @retval Status: SUCCESS = Success
        FAILURE = Failure

**/
/*
RETCODE
PrintDebugMsg(
    int EdkErrorLevel,
    int UsbErrorLevel,
    char * Message, ...)
{
	va_list ArgList;

    va_start(ArgList, Message);
    
    if ((UsbErrorLevel == 0) || ((UsbErrorLevel <= TopDebugLevel) &&
            (UsbErrorLevel >= BottomDebugLevel))) {
#if USB_DEBUG_MESSAGES == 1
        EfiDebugVPrint(EFI_D_ERROR, Message, ArgList);
#endif
    }

    va_end(ArgList);

    return SUCCESS;
}
*/

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
