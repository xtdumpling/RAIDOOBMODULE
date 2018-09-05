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

/** @file ErrorDisplayFrameworkProtocol.h
 Contains GUIDs, Data Structures, etc related to the Error Display Protocol
 */

#ifndef __ERROR_DISPLAY_PROTOCOL_H__
#define __ERROR_DISPLAY_PROTOCOL_H__
#ifdef __cplusplus
extern "C" {
#endif

#define ERROR_DISPLAY_FFS_GUID \
    { 0x2CE70F66, 0xAC57, 0x4346, { 0xA9, 0x1F, 0x89, 0x28, 0x1A, 0x07, 0xFA, 0xD6} }

#define EFI_ERROR_DISPLAY_FRAMEWORK_PROTOCOL_GUID \
    { 0xec69a5fc, 0x83be, 0x43bb, { 0x87, 0xf7, 0x4b, 0x7, 0xe3, 0x14, 0xe7, 0xc0 } }
extern EFI_GUID gAmiErrorDisplayFrameworkProtocolGuid;


#include <UefiHii.h>

/// function prototype for the protocol function AnyErrorsToDisplay
typedef EFI_STATUS (EFIAPI *ANY_ERRORS_TO_DISPLAY)
    ( IN struct _EFI_ERROR_DISPLAY_PROTOCOL   *This);

/// function prototype for the protocol function ErrorDisplay
typedef EFI_STATUS (EFIAPI *ERROR_DISPLAY )
    (
    IN  struct _EFI_ERROR_DISPLAY_PROTOCOL  *This,
    IN  UINT32                              *Flags
 );


/**  
  This data structure defines the Error Display Protocol.  The Protocol 
  contains two function pointers.  These two functions are used to 
  display the Error messages as requested by the OEM.  The protocol 
  should be accessed at the end of post to display the messages logged
  during driver execution.
 */
typedef struct _EFI_ERROR_DISPLAY_PROTOCOL {
    ANY_ERRORS_TO_DISPLAY   AnyErrorsToDisplay; ///< Function pointer to determine if there have been any errors logged in the Data Hub that should be displayed
    ERROR_DISPLAY           ErrorDisplay; ///< Function pointer to display all the errors found in the Data Hub
    } EFI_ERROR_DISPLAY_PROTOCOL;



/**
  This data structure contains information used to determine if an error 
  reported to the Data Hub, needs to have an POST error message displayed
*/
typedef struct _ERROR_CODE_LOOKUP_STRUCT 
    {
        UINT32      StatusCode; ///< Error code used to log the error into the data hub
        STRING_REF  ErrorStringToken; ///> Error message string token 
        UINT32      Flags; ///> Return code that can specifiy actions based on individual error
        UINT8       AlreadyDisplayed; ///> value to determine if the error has already been displayed on this boot
    } ERROR_CODE_LOOKUP_STRUCT;


#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

