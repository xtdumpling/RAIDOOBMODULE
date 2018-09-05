//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
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
// $Header: /Alaska/SOURCE/Modules/TcgNext/Common/Tpm20Acpi/Tpm20Acpi.h 1     10/08/13 12:06p Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 12:06p $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Common/Tpm20Acpi/Tpm20Acpi.h $
//
// 1     10/08/13 12:06p Fredericko
// Initial Check-In for Tpm-Next module
//
// 1     7/10/13 5:58p Fredericko
// [TAG]        EIP120969
// [Category]   New Feature
// [Description]    TCG (TPM20)
//
//
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:
//
// Description:
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _TPM20ACPI_H_
#define _TPM20ACPI_H_

#include <Efi.h>
#include <Protocol/AcpiTable.h>
#include <Hob.h>
#include <IndustryStandard/Acpi.h>

#define EFI_SIGNATURE_16(A, B)        ((A) | (B << 8))
#define EFI_SIGNATURE_32(A, B, C, D)  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))

#define EFI_SIGNATURE_64(A, B, C, D, E, F, G, H) \
    (EFI_SIGNATURE_32 (A, B, C, D) | ((UINT64) (EFI_SIGNATURE_32 (E, F, G, H)) << 32))

#define TPM20TABLEOEMREVISION 1
#define CREATOR_ID_AMI        0x20494D41  //" IMA""AMI "(AMI)
#define EFI_TPM2_ACPI_TABLE_START_METHOD_ACPI  2
#define EFI_TPM2_ACPI_TABLE_START_METHOD_TIS_CANCEL  6
#define EFI_TPM2_ACPI_TABLE_START_METHOD_CRB_INTF    7
#define EFI_TPM2_ACPI_TABLE_START_METHOD_CRB_INTF_ACPI  8

#define EFI_TPM2_ACPI_TABLE_REVISION  3

#pragma pack (1)


typedef struct
{
    EFI_ACPI_DESCRIPTION_HEADER Header;
    UINT32                      Flags;
    UINT64                      AddressOfControlArea;
    UINT32                      StartMethod;
} EFI_TPM2_ACPI_TABLE;

typedef struct
{
    UINT32   Reserved;
    UINT32   Error;
    UINT32   Cancel;
    UINT32   Start;
    UINT64   InterruptControl;
    UINT32   CommandSize;
    UINT64   Command;
    UINT32   ResponseSize;
    UINT64   Response;
} EFI_TPM2_ACPI_CONTROL_AREA;


typedef struct
{
    EFI_HOB_GUID_TYPE     EfiHobGuidType;
    UINT64                BufferAddress;
} INTEL_PTT_HOB_DATA;

typedef struct
{
    UINT64        PttCRBAddress;
} PTT_ACPI_NVS_AREA;

#pragma pack ()

/*
#define GET_HOB_TYPE( Hob )     ((Hob).Header->HobType)
#define GET_HOB_LENGTH( Hob )   ((Hob).Header->HobLength)
#define GET_NEXT_HOB( Hob )     ((Hob).Raw + GET_HOB_LENGTH( Hob ))
#define END_OF_HOB_LIST( Hob )  (GET_HOB_TYPE( Hob ) ==\
                                 EFI_HOB_TYPE_END_OF_HOB_LIST)
*/
#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
