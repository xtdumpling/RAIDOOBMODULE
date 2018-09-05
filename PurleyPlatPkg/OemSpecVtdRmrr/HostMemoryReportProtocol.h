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
// Name:  HostMemoryReportProtocol.h
//
// Description:	Host Memory Report protocol definitions
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef _HOST_MEMORY_REPORT_PROTOCOL_H_
#define _HOST_MEMORY_REPORT_PROTOCOL_H_

#include <Uefi/UefiBaseType.h>

#define EFI_HOST_MEMORY_REPORT_PROTOCOL_GUID \
		{ 0x14f829ec, 0xec6, 0x4aaf, 0x99, 0xc6, 0x7c, 0x4d, 0x30, 0xa4, 0xbb, 0xb3}

typedef struct _EFI_HOST_MEMORY_REPORT_PROTOCOL  EFI_HOST_MEMORY_REPORT_PROTOCOL;

#pragma pack(1)
typedef struct _EFI_HOST_MEMORY_REPORT_INFO
{
    EFI_PHYSICAL_ADDRESS   HostMemoryAddress;
    UINT64        Size;    // in bytes
    UINTN       Bus;     // Bus number of this cotnroller
    UINTN       Device; // Device numbe rof this controller 
    UINTN       Func;    // Function number of this cotnroller
} EFI_HOST_MEMORY_REPORT_INFO;
#pragma pack()

typedef
EFI_STATUS
(EFIAPI *EFI_HOST_MEMORY_REPORT_GETINFO) (
  IN  EFI_HOST_MEMORY_REPORT_PROTOCOL  *This,
  OUT  EFI_HOST_MEMORY_REPORT_INFO    *Info
);

typedef struct _EFI_HOST_MEMORY_REPORT_PROTOCOL {
    EFI_HOST_MEMORY_REPORT_GETINFO   GetInfo; 
} EFI_HOST_MEMORY_REPORT_PROTOCOL;

extern EFI_GUID gEfiHostMemoryReportProtocolGuid;

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
