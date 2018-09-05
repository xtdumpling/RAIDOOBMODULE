//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************************
// Revision History
// ----------------
// $Log: $
// 
//
//*****************************************************************************
//<AMI_FHDR_START>
//
// Name:        CRB.h
//
// Description: 
//              
//
//<AMI_FHDR_END>
//*****************************************************************************

#ifndef __CRB_H__
#define __CRB_H__

#include <Hob.h>

//----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#define AMI_PEI_CRB_CREAT_CONFIG_NOTIFY 0x00000001
#define AMI_PEI_CRB_SMBUS_NOTIFY 0x00000011

#define AMI_DXE_CRB_DRIVER_ENTRY 0x80000001
#define AMI_DXE_CRB_SB_CALLBACK 0x80000011
#define AMI_DXE_CRB_CSM_CALLBACK 0x80000012
#define AMI_DXE_CRB_ME_CALLBACK 0x80000013
#define AMI_DXE_CRB_NB_CALLBACK 0x80000014

#define AMI_SMM_CRB_DRIVER_ENTRY 0xC0000001
#define AMI_SMM_CRB_IN_SMM_ENTRY 0xC0000002
#define AMI_SMM_CRB_SMI 0xC0000003
#define AMI_SMM_CRB_SX_SMI 0xC0000011
#define AMI_SMM_CRB_SW_SMI 0xC0000012
#define AMI_SMM_CRB_ACPI_ENABLE 0xC0000013
#define AMI_SMM_CRB_ACPI_DISABLE 0xC0000014

typedef EFI_STATUS (EFIAPI *CRB_INIT_FUNC) (  
    IN CONST VOID *VoidPtr,
    IN UINT32 TaskCode,
    IN VOID *Data);

// {E40A9415-A005-407d-832F-60A6F837E9B8}
#define AMI_PEI_CRB_POLICY_GUID \
    {0xe40a9415, 0xa005, 0x407d, 0x83, 0x2f, 0x60, 0xa6, 0xf8, 0x37, 0xe9, 0xb8}

typedef struct _AMI_PEI_CRB_POLICY_PPI {
    UINT32 ServiceIndex;
    UINT32 BoardType:8;
    UINT32 ClockGen:1;
    UINT32 SioSupport:1;
    UINT32 Reserved0:22;
} AMI_PEI_CRB_POLICY_PPI;

#ifdef __cplusplus
}
#endif
#endif


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
