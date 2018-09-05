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

/** @file SelProtocol.h
    SEL protocol definitions

**/

#ifndef _EFI_SEL_H_
#define _EFI_SEL_H_

#define EFI_SM_SEL_PROTOCOL_GUID \
  { \
    0xbc5f861c, 0x86ed, 0x417e, { 0xbb, 0x7b, 0x6c, 0x2, 0x6b, 0xdc, 0x65, 0x23 } \
  }

typedef struct _EFI_SM_SEL_STATUS_CODE_PROTOCOL EFI_SM_SEL_STATUS_CODE_PROTOCOL;

//
//  SEL Protocol APIs
//
typedef
EFI_STATUS
(EFIAPI *EFI_SEL_REPORT_STATUS_CODE) (
  IN          EFI_SM_SEL_STATUS_CODE_PROTOCOL      *This,
  IN          EFI_STATUS_CODE_TYPE                 CodeType,
  IN          EFI_STATUS_CODE_VALUE                Value,
  IN          UINT32                               Instance,
  IN          EFI_GUID                             *CallerId,
  IN OPTIONAL EFI_STATUS_CODE_DATA                 *Data
);

typedef struct _EFI_SM_SEL_STATUS_CODE_PROTOCOL {
  EFI_SEL_REPORT_STATUS_CODE           EfiSelReportStatusCode;
} EFI_SM_SEL_STATUS_CODE_PROTOCOL;

extern EFI_GUID gEfiSelStatusCodeProtocolGuid;

#endif

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
