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
// $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/PatsburgPCH/Chipset/AcpiModeEnable.h 1     12/09/11 1:19p Archanaj $
//
// $Revision: 1 $
//
// $Date: 12/09/11 1:19p $
//
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/SouthBridge/PatsburgPCH/Chipset/AcpiModeEnable.h $
// 
// 1     12/09/11 1:19p Archanaj
// 
// 3     10/07/10 12:47p Bhimanadhunik
// Original source of Patsburg Module is taken from Cougar Point Module
// Label : 4.6.4_Intel_CPT_025.
// Changed the code to work with Patsburg PCH.
// 
// 2     7/07/10 2:56p Bhimanadhunik
// Intial check-in for Patsburg.
// 
// 1     2/24/10 11:04p Tonywu
// Intel Cougar Point/SB eChipset initially releases.
// 
//**********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        AcpiModeEnable.h
//
// Description: Define all the ACPI mode equates and structures in this file.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef __SMM_ACPI_EN_PROTOCOL_H__
#define __SMM_ACPI_EN_PROTOCOL_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <EFI.h>

#define EFI_ACPI_EN_DISPATCH_PROTOCOL_GUID \
                            { 0xbd88ec68, 0xebe4, 0x4f7b, 0x93, 0x5a, 0x4f,\
                              0x66, 0x66, 0x42, 0xe7, 0x5f }

#define EFI_ACPI_DIS_DISPATCH_PROTOCOL_GUID \
                            { 0x9c939ba6, 0x1fcc, 0x46f6, 0xb4, 0xe1, 0x10, \
                              0x2d, 0xbe, 0x18, 0x65, 0x67 }

#define PM1_CNT                 0x04
#define BIT_WAK_STS             0x8000
#define BIT_SLP_TYP_MASK        0x1C00
#define S3_SLP_TYP              0x05

typedef EFI_STATUS INIT_FUNCTION (IN VOID *ImageHandler, IN VOID *SystemTable);

typedef struct _EFI_ACPI_DISPATCH_PROTOCOL EFI_ACPI_DISPATCH_PROTOCOL;

//----------------------------------------------------------------------------
// EFI_ACPI_DISPATCH
//----------------------------------------------------------------------------

#ifndef __SMM_CHILD_DISPATCH__H__
typedef struct _GENERIC_LINK GENERIC_LINK;
typedef struct _GENERIC_LINK {
    void                    *Link;
};
#endif

typedef VOID (EFIAPI *EFI_ACPI_DISPATCH) (
    IN EFI_HANDLE           DispatchHandle
);

typedef struct _ACPI_DISPATCH_LINK ACPI_DISPATCH_LINK;
struct _ACPI_DISPATCH_LINK {
    IN ACPI_DISPATCH_LINK   *Link;
    IN EFI_ACPI_DISPATCH    Function;
};

typedef EFI_STATUS (EFIAPI *EFI_ACPI_REGISTER) (
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_ACPI_DISPATCH            DispatchFunction,
    OUT EFI_HANDLE                  *DispatchHandle
);

typedef EFI_STATUS (EFIAPI *EFI_ACPI_UNREGISTER) (
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                   DispatchHandle
);


struct _EFI_ACPI_DISPATCH_PROTOCOL {
    EFI_ACPI_REGISTER       Register;
    EFI_ACPI_UNREGISTER     UnRegister;
};


typedef EFI_STATUS (*EFI_TASK_FUNCTION) (VOID);


/****** DO NOT WRITE BELOW THIS LINE *******/
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
