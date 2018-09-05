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

//**********************************************************************
// $Header:  $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  OemVtdRmrrProtocol.h
//
// Description:	Oem Vtd Rmrr protocol definitions
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef _OEM_VTD_RMRR_PROTOCOL_H_
#define _OEM_VTD_RMRR_PROTOCOL_H_

#define EFI_OEM_VTD_RMRR_PROTOCOL_GUID \
        { 0xff1ed67a, 0x82d4, 0x4f9f, { 0x9e, 0x6d, 0xf5, 0x3f, 0x4, 0x20, 0x4, 0x5a } };

typedef struct _EFI_OEM_VTD_RMRR_PROTOCOL  EFI_OEM_VTD_RMRR_PROTOCOL;

typedef enum {
    OemVtdRmrrCollectInfo,
    OemVtdRmrrInsertRmrr,
    OemVtdRmrrMaxFuncNumber
} OEM_VTD_RMRR_FUNC_NUMBER;

typedef
EFI_STATUS
(EFIAPI *EFI_INSERT_OEM_VTD_RMRR) (
  IN  VOID  *DmaRemap );

typedef struct _EFI_OEM_VTD_RMRR_PROTOCOL {
    EFI_INSERT_OEM_VTD_RMRR   InsertOemVtdRmrr; 
} EFI_OEM_VTD_RMRR_PROTOCOL;

extern EFI_GUID gEfiOemVtdRmrrProtocolGuid;

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
