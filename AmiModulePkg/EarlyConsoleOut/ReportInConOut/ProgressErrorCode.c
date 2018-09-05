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

/** @file ProgressErrorCode.c
    Library functions to be used for getting the progress checkpoint , Error CheckPoint
    Progress String and Error String 

**/

#include <Include/AmiLib.h>
#include <Library/PrintLib.h>
#include <ProgressErrorCode.h>

/**

    Return the PostCode value for the Progress Code 
  
    @param  EFI ProgressCode

    @return  PostCode.

**/
UINT8
GetAmiProgressCodeCheckPoint (
    IN EFI_STATUS_CODE_VALUE    ProgressCode 
)
{
    UINTN       CheckPointIndex;

    for (CheckPointIndex = 0; ProgressCheckpointMap[CheckPointIndex].Value != 0; CheckPointIndex++) {
        if ( ProgressCheckpointMap[CheckPointIndex].Value == ProgressCode) {
            return ProgressCheckpointMap[CheckPointIndex].PostCode ;  
        }
    }
       
    return 0;
}

/**

    Return the PostCode value for the EFI Error Code 
  
    @param  EFI Error Code

    @return  PostCode.

**/
UINT8
GetAmiErrorCodeCheckPoint (
    IN EFI_STATUS_CODE_VALUE    ErrorCode 
)
{
    UINTN       CheckPointIndex;

    for (CheckPointIndex = 0; ErrorCodeCheckpointMap[CheckPointIndex].Value != 0; CheckPointIndex++) {
        if ( ErrorCodeCheckpointMap[CheckPointIndex].Value == ErrorCode) {
            return ErrorCodeCheckpointMap[CheckPointIndex].PostCode ;  
        }
    }
       
    return 0;
}

/**

    Return the String for the Progress Code 
  
    @param  EFI ProgressCode

    @return  String.

**/
CHAR8*
GetAmiProgressCodeString (
    IN EFI_STATUS_CODE_VALUE    ProgressCode 
)
{
    UINTN       CheckPointIndex;

    for (CheckPointIndex = 0; ProgressCodeVideoStrings[CheckPointIndex].Value != 0; CheckPointIndex++) {
        if ( ProgressCodeVideoStrings[CheckPointIndex].Value == ProgressCode) {
            return ProgressCodeVideoStrings[CheckPointIndex].PostString ;  
        }
    }
       
    return NULL;
}


/**

    Return the String for the Progress Code 
  
    @param  EFI ProgressCode

    @return  String.

**/
EFI_STATUS
GetAmiErrorCodeString (
    IN EFI_STATUS_CODE_VALUE    ErroCode,
    OUT CHAR8                   **ErrorCodeString,
    OUT CHAR8                   **RootCauseCodeString,
    OUT CHAR8                   **PosibileSolutionString
)
{
    UINTN       CheckPointIndex;

    for (CheckPointIndex = 0; ErrorCodeVideoStrings[CheckPointIndex].Value != 0; CheckPointIndex++) {
        if ( ErrorCodeVideoStrings[CheckPointIndex].Value == ErroCode) {
            *ErrorCodeString= ErrorCodeVideoStrings[CheckPointIndex].ErrorString ;  
            *RootCauseCodeString= ErrorCodeVideoStrings[CheckPointIndex].PosibileCauseString ;  
            *PosibileSolutionString= ErrorCodeVideoStrings[CheckPointIndex].PosibibleSoutionString ;  
            return EFI_SUCCESS;
        }
    }
       
    return EFI_NOT_FOUND;
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
