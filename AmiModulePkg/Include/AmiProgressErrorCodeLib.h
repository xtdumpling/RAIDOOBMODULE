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

/** @file  AmiProgressErrorCodeLib.h
    Declares structures, progress code , error code, progress code string and error code string
    used for by PEI and DXE status code driver 

**/

#ifndef __AMI_PROGRESS_ERROR_CODE_LIB_H__
#define __AMI_PROGRESS_ERROR_CODE_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#define POST_MSG_FOREGROUND         0x07    //White
#define POST_MSG_BACKGROUND         0x00    //Black

#define MAJOR_ERROR_FOREGROUND      0x04    //Red
#define MAJOR_ERROR_BACKGROUND      0x00    //Black

#define MINOR_ERROR_FOREGROUND      0x06    //Brown
#define MINOR_ERROR_BACKGROUND      0x00    //Black

#define PROGRESS_CODE_FOREGROUND    0x02    //Green
#define PROGRESS_CODE_BACKGROUND    0x00    //Black

#define DISPLAY_BLINK               0x00    // Not Supported  

UINT8
GetAmiProgressCodeCheckPoint (
    IN EFI_STATUS_CODE_VALUE    ProgressCode 
);

UINT8
GetAmiErrorCodeCheckPoint (
    IN EFI_STATUS_CODE_VALUE    ErrorCode 
);

CHAR8*
GetAmiProgressCodeString (
    IN EFI_STATUS_CODE_VALUE    ProgressCode 
);

EFI_STATUS
GetAmiErrorCodeString (
    IN EFI_STATUS_CODE_VALUE    ErroCode,
    OUT CHAR8                   **ErrorCodeString,
    OUT CHAR8                   **RootCauseCodeString,
    OUT CHAR8                   **PosibileSolutionString
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
