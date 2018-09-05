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


/** @file DxeSmmRedirFuncs.h
    DxeSmmRedirFuncs Header file

**/

#ifndef __DxeSmmRedirFuncs__H__
#define __DxeSmmRedirFuncs__H__

#include <Protocol/GpnvRedirProtocol.h>


// Communicate data for the Redirect Functions SetEventLogData

typedef struct {

// By the FunctionNumber the handler decides which function to be called
// 1-EfiSetSmbiosElogData,2-EfiGetSmbiosElogData,3-EfiEraseSmbiosElogData,
// 4-EfiActivateSmbiosElogData

    UINTN                       FunctionNumber;
    UINT8                       *ElogData;
    EFI_SM_ELOG_REDIR_TYPE      DataType;
    BOOLEAN                     AlertEvent;
    UINTN                       Size;
    UINT64                      *RecordId;
    EFI_STATUS                  ReturnStatus;
} SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA;

// Communicate data for the Redirect Functions GetEventLogData

typedef struct {

// By the FunctionNumber the handler decides which function to be called
// 1-EfiSetSmbiosElogData,2-EfiGetSmbiosElogData,3-EfiEraseSmbiosElogData,
// 4-EfiActivateSmbiosElogData

    UINTN                        FunctionNumber;
    UINT8                       *ElogData;
    EFI_SM_ELOG_REDIR_TYPE       DataType;
    UINTN                       *Size;
    UINT64                      *RecordId;
    EFI_STATUS                   ReturnStatus;
} SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA;


// Communicate data for the Redirect Functions EraseEventLogData

typedef struct {

// By the FunctionNumber the handler decides which function to be called
// 1-EfiSetSmbiosElogData,2-EfiGetSmbiosElogData,3-EfiEraseSmbiosElogData,
// 4-EfiActivateSmbiosElogData

    UINTN                               FunctionNumber;
    EFI_SM_ELOG_REDIR_TYPE              DataType;
    UINT64                             *RecordId;
    EFI_STATUS                          ReturnStatus;
} SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA;

// Communicate data for the Redirect Functions ActivateEventLogData

typedef struct {

// By the FunctionNumber the handler decides which function to be called
// 1-EfiSetSmbiosElogData,2-EfiGetSmbiosElogData,3-EfiEraseSmbiosElogData,
// 4-EfiActivateSmbiosElogData

    UINTN                          FunctionNumber;
    EFI_SM_ELOG_REDIR_TYPE         DataType;
    BOOLEAN                       *EnableElog;
    BOOLEAN                       *ElogStatus;
    EFI_STATUS                     ReturnStatus;
} SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA;

// The payload for this function is SMM_REDIR_FUNCS_SET_COMMUNICATE.

#define SMM_REDIR_FUNCS_SET_COMMUNICATE         1

// The payload for this function is SMM_REDIR_FUNCS_GET_COMMUNICATE.

#define SMM_REDIR_FUNCS_GET_COMMUNICATE         2

// The payload for this function is SMM_REDIR_FUNCS_ERASE_COMMUNICATE.

#define SMM_REDIR_FUNCS_ERASE_COMMUNICATE       3

// The payload for this function is SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE.

#define SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE    4

// Communicate Header size

#define SMM_COMMUNICATE_HEADER_SIZE  (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data))

// SetEventLog Communicate data size

#define SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA_SIZE sizeof(SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA);

// GetEventLog Communicate data size

#define SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA_SIZE sizeof(SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA);

// EraseEventLog Communicate data size

#define SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA_SIZE sizeof(SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA);

// ActivateEventLog Communicate data size

#define SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA_SIZE sizeof(SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA);

#endif

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
