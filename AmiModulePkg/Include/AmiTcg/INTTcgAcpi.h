//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/INTTcgAcpi.h 2     5/19/10 5:24p Fredericko $
//
// $Revision: 2 $
//
// $Date: 5/19/10 5:24p $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  INTTcgAcpi.h
//
// Description: Acpi definitions for TCG module
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _TCG_ACPI_3_0_H_
#define _TCG_ACPI_3_0_H_


#define EFI_ACPI_30_TABLE_GUID EFI_ACPI_20_TABLE_GUID

extern EFI_GUID gEfiAcpiTableGuid;
extern EFI_GUID gEfiAcpi20TableGuid;
extern EFI_GUID gEfiAcpi30TableGuid;


#pragma pack(1)
typedef struct
{
    UINT32 Signature;
    UINT32 Length;
    UINT8  Revision;
    UINT8  Checksum;
    UINT8  OemId[6];
    UINT64 OemTableId;
    UINT32 OemRevision;
    UINT32 CreatorId;
    UINT32 CreatorRevision;
} TCG_EFI_ACPI_DESCRIPTION_HEADER;


//
// "TCPA" Trusted Computing Platform Alliance Capabilities Table
//
//#define\
//    EFI_ACPI_3_0_TRUSTED_COMPUTING_PLATFORM_ALLIANCE_CAPABILITIES_TABLE_SIGNATURE \
//    0x41504354


#pragma pack()

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
