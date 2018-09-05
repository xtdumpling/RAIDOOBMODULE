//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
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
//*************************************************************************
// $Header:$
//
// $Revision:  $
//
// $Date: $
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:    AmiAcpiCpuLib.h
//
// Description:
//  Library functions for creating APIC and MP entries.
//
// Notes:
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef AMI_ACPI_CPU_LIB_H
#define AMI_ACPI_CPU_LIB_H
#ifdef __cplusplus
extern "C" {
#endif

EFI_STATUS CreateCpuMadtEntries(IN UINT32 TableVer, OUT VOID **Entries, OUT UINT32 *NumEntries, OUT UINT32 *TableSize);
EFI_STATUS CreateCpuMpTableEntries(OUT VOID **Entries, OUT UINT32 *NumEntries, OUT UINT32 *TableSize);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

