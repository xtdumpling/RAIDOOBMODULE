//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Renew the SMC ROM Hole.
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/04/2016
//
//****************************************************************************
//****************************************************************************

#ifndef __SMC_PLATFORM_SMBIOS__H__
#define __SMC_PLATFORM_SMBIOS__H__
#ifdef __cplusplus
extern "C" {
#endif

typedef
EFI_STATUS
(EFIAPI *SMC_SMBIOS_UPDATE)(
    IN		UINT16	UpdateHandle,
    IN OUT	VOID	*UpdateBuffer,
    IN OUT	UINT16	*UpdateBufferSize
);

#pragma pack(push,1)

typedef struct{
    UINT8		SmBiosUpdateType;
    SMC_SMBIOS_UPDATE	SmBiosUpdateFn;
}SMC_SMBIOS_UPDATE_TABLE;

typedef struct{
    UINT8	DIMM_NUM[3];
}IMC_DIMM_NUM;
#pragma pack(pop)

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************
