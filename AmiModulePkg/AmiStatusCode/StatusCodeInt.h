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


/** @file StatusCodeInt.h
    NVRAM Definitions

**/
//**********************************************************************
#ifndef __StatusCode__H__
#define __StatusCode__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <StatusCodeElinks.h>
#include <Protocol/ReportStatusCodeHandler.h>

   typedef struct
    {
        EFI_STATUS_CODE_VALUE Value;
        UINT8 Byte;
    } STATUS_CODE_TO_BYTE_MAP;
    
    typedef VOID (ERROR_CODE_ACTION)(
        IN VOID *PeiServices, IN EFI_STATUS_CODE_VALUE Value
    );
    
    typedef struct
    {
        EFI_STATUS_CODE_VALUE Value;
        ERROR_CODE_ACTION *Action;
    } ERROR_CODE_ACTION_MAP;
    
#define STATUS_CODE_TYPE(Type) ((Type)&EFI_STATUS_CODE_TYPE_MASK)
    
    UINT8 FindByteCode(STATUS_CODE_TO_BYTE_MAP *Map, EFI_STATUS_CODE_VALUE Value);

    EFI_STATUS AmiReportStatusCode (
        IN VOID *PeiServices,
        IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value,
        IN UINT32 Instance, IN EFI_GUID *CallerId OPTIONAL,
        IN EFI_STATUS_CODE_DATA *Data OPTIONAL, UINT8 *String
    );
    VOID InitStatusCodeParts(IN VOID *ImageHandler, IN VOID *ServicesTable);
    VOID ResetOrResume(IN VOID *PeiServices, IN EFI_STATUS_CODE_VALUE Value);

typedef struct {
  EFI_STATUS_CODE_TYPE      Type;
  EFI_STATUS_CODE_VALUE     Value;
  UINT32                    Instance;
  UINT32                    Reserved;
  EFI_GUID                  CallerId;
  EFI_STATUS_CODE_DATA      Data;
} RSC_DATA_ENTRY;

typedef struct {
  EFI_RSC_HANDLER_CALLBACK  RscHandlerCallback;
  EFI_TPL                   Tpl;
  EFI_EVENT                 Event;
  EFI_PHYSICAL_ADDRESS      StatusCodeDataBuffer;
  UINTN                     BufferSize;
  EFI_PHYSICAL_ADDRESS      EndPointer;
} RSC_HANDLER_CALLBACK_ENTRY;

typedef struct {
  UINT32  RegisteredEntries;
  UINT32  TotalEntries;
} ROUTER_STRUCT_HEADER;


typedef EFI_STATUS (INIT_FUNCTION)(IN VOID *ImageHandler, IN VOID *ServicesTable);
typedef EFI_STATUS (STRING_FUNCTION)(IN VOID *PeiServices, IN CHAR8 *String);
typedef EFI_STATUS (SIMPLE_FUNCTION)(
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value
);
typedef EFI_STATUS (MISC_FUNCTION)(
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value,
    IN UINT32 Instance, IN CONST EFI_GUID *CallerId OPTIONAL,
    IN EFI_STATUS_CODE_DATA *Data OPTIONAL
);
typedef VOID (CHECKPOINT_FUNCTION)(IN VOID *PeiServices, IN UINT8 Checkpoint);

typedef struct {
    UINT32 ErrorValue;
    CHAR8* ErrorString;
} ERROR_MESSAGE;

VOID EFIAPI AmiDebigServiceDebugPrintHelper (
    IN VOID *PeiServices, OUT UINT8 *String, IN UINTN ErrorLevel, IN CONST CHAR8 *Format, VA_LIST VaListMarker
);
VOID EFIAPI AmiDebigServiceDebugAssertHelper (
    IN VOID *PeiServices, OUT UINT8 *String, IN CONST CHAR8  *FileName, IN UINTN LineNumber, IN CONST CHAR8 *Description
);
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
