//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file 
CleanErrorLog.h

Header file for the CleanErrorLog

*/

#ifndef _EFI_CLEANERRORLOG_H_
#define _EFI_CLEANERRORLOG_H_
#ifdef __cplusplus
extern "C" {
#endif

VOID CleanErrorLogEntry (IN VOID *Buffer, IN OUT UINT8 *pOFBDDataHandled);
VOID CleanErrorLogInSmm (VOID);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
