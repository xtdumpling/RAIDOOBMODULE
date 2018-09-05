//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//    Bug Fix:  ACPI Debug Feature.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Apr/07/2016
//
//***************************************************************************

//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  Acpi Debug ASL code.

Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/ 

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        AcpiDebugSmm.c
//
// Description: ACPI debug SMM code.
//
//<AMI_FHDR_END>
//**********************************************************************

#include "Token.h"
#include <PiDxe.h>
#include <Library\BaseLib.h>
#include <Library\BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library\UefiBootServicesTableLib.h>
#include <Library\DebugLib.h>
#include <Library\PcdLib.h>
#include <Library\DxeServicesLib.h>
#include <Protocol\AcpiTable.h>
#include <IndustryStandard\Acpi.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Guid/StatusCodeDataTypeDebug.h>
#include "AmiDxeLib.h"
#include "A7PrintfLib.h"


#define ACPI_DEBUG_STR "INTEL ACPI DEBUG"

#define ACPI_DEBUG_BUFFER_SIZE 0x10000
#define ACPI_DEBUG_BUFFER_EX_SIZE 0x1000
#define ACPI_DEBUG_STRING_SIZE 64

// ad414dd9-076f-40fa-8b6a-1b6779adeca3
#define ACPI_DEBUG_TABLES_GUID \
  { \
     0xad414dd9, 0x076f, 0x40fa, 0x8b, 0x6a, 0x1b, 0x67, 0x79, 0xad, 0xec, 0xa3 \
  }

//
// ASL NAME structure 
//
#pragma pack(1)
//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: NAME_LAYOUT
//
// Description: Name layout.
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// NameOp                   UINT8                   Byte [0]=0x08:NameOp.
// NameString               UINT32                  Byte [4:1]=Name of object.
// DWordPrefix              UINT8                   Byte [5]=0x0C:DWord Prefix.
// Value                    UINT32                  0   ; Value of named object.
//----------------------------------------------------------------------------
//<AMI_THDR_END>
typedef struct {
  UINT8   NameOp;       //  Byte [0]=0x08:NameOp.
  UINT32  NameString;   //  Byte [4:1]=Name of object.
  UINT8   DWordPrefix;  //  Byte [5]=0x0C:DWord Prefix.
  UINT32  Value;        //  0   ; Value of named object.
} NAME_LAYOUT;
#pragma pack()

#pragma pack(1)
//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: ACPI_DEBUG_HEAD
//
// Description: ACPI debug head.
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// "Signature[16]"          UINT8                   "INTEL ACPI DEBUG"
// BufferSize               UINT32                  Total size of Acpi Debug buffer including header structure
// Head                     UINT32                  Current buffer pointer for SMM to print out
// Tail                     UINT32                  Current buffer pointer for ASL to input
// SmiTrigger               UINT8                   Value to trigger the SMI via B2 port
// Wrap                     UINT8                   If current Tail < Head
// Reserved                 UINT16                  
//----------------------------------------------------------------------------
//<AMI_THDR_END>
typedef struct {
  UINT8  Signature[16];     // "INTEL ACPI DEBUG"
  UINT32 BufferSize;        // Total size of Acpi Debug buffer including header structure
  UINT32 Head;              // Current buffer pointer for SMM to print out
  UINT32 Tail;              // Current buffer pointer for ASL to input
  UINT8  SmiTrigger;        // Value to trigger the SMI via B2 port
  UINT8  Wrap;              // If current Tail < Head
  UINT16 Reserved;
} ACPI_DEBUG_HEAD;
#pragma pack()

#pragma pack(1)
//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: ACPI_DEBUG_VA_ARG
//
// Description: ACPI debug arguments.
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// Type                     UINT8                   5 - string message, 6 - format string message.
// Arg1                     UINT32                  argument 1 of format string message.
// Arg2                     UINT32                  argument 2 of format string message.
// Arg3                     UINT32                  argument 3 of format string message.
//----------------------------------------------------------------------------
//<AMI_THDR_END>
typedef struct {
  UINT8 Type;
  UINT32 Arg1;
  UINT32 Arg2;
  UINT32 Arg3;
} ACPI_DEBUG_VA_ARG;
#pragma pack()

#define AD_SIZE sizeof(ACPI_DEBUG_HEAD) // This is 0x20

UINT32 mBufferIndex;
UINT32 mBufferEnd;
ACPI_DEBUG_HEAD *mAcpiDebug = NULL;


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  AcpiDebugSmmCallback
//
// Description: 
//  Software SMI callback for ACPI Debug which is called from ACPI method.
//
// Input:       
//      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
//      Context         Points to an optional handler context which was specified when the
//                      handler was registered.
//
// Output:      
//      CommBuffer      A pointer to a collection of data in memory that will
//                      be conveyed from a non-SMM environment into an SMM environment.
//      CommBufferSize  The size of the CommBuffer.
//      retval EFI_SUCCESS     The interrupt was handled successfully.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
EFIAPI
AcpiDebugSmmCallback (
    IN EFI_HANDLE DispatchHandle,
    IN CONST VOID *Context,
    IN OUT VOID *CommBuffer,
    IN OUT UINTN *CommBufferSize
)
{
    ACPI_DEBUG_VA_ARG* VaArg;
    
    VaArg = (ACPI_DEBUG_VA_ARG*)(UINTN)(mAcpiDebug->Head + ACPI_DEBUG_STRING_SIZE);
    
    if (VaArg->Type == 5) {
        a7printf ((UINT8*)mAcpiDebug->Head);
    }
    else if (VaArg->Type == 6) {
        a7printf ((UINT8*)mAcpiDebug->Head, VaArg->Arg1, VaArg->Arg2, VaArg->Arg3);
    }
    else {
        a7printf ((UINT8*)mAcpiDebug->Head);
    }

    //
    // The size is from CPTR in "AcpiDebug.asl".
    //
    ZeroMem((UINT8*)mAcpiDebug->Head, ACPI_DEBUG_STRING_SIZE + 1 + (4 * 3));

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  LoadAcpiTables
//
// Description: 
//  Install ACPI table to system.
//
// Input:       
//      None.
//
// Output:      
//      None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
LoadAcpiTables (
    VOID
)
{
    EFI_STATUS Status;
    EFI_ACPI_TABLE_PROTOCOL *AcpiTable;
    UINTN Size;
    EFI_ACPI_DESCRIPTION_HEADER *TableHeader;
    UINTN TableKey;
    UINT8 *CurrPtr;
    UINT32 *Signature;
    NAME_LAYOUT *NamePtr;
    UINT8 UpdateCounter;
    EFI_GUID AcpiDebugTablesGuid = ACPI_DEBUG_TABLES_GUID;
    
    Status = GetSectionFromFv (
        &AcpiDebugTablesGuid,
        EFI_SECTION_RAW,
        0,
        &TableHeader,
        &Size);
    ASSERT_EFI_ERROR (Status);
    
    //
    // This is Acpi Debug SSDT. Acpi Debug should be enabled if we reach here so load the table.
    //
    ASSERT (((EFI_ACPI_DESCRIPTION_HEADER *) TableHeader)->OemTableId == SIGNATURE_64 ('A', 'D', 'e', 'b', 'T', 'a', 'b', 'l'));
    
    //
    // count pointer updates so we can stop after all three pointers are patched 
    //
    UpdateCounter = 1;
    for (CurrPtr = (UINT8 *) TableHeader; CurrPtr <= ((UINT8 *) TableHeader + TableHeader->Length), UpdateCounter < 4; CurrPtr++) {
        Signature = (UINT32 *) (CurrPtr + 1);

        //
        // patch DPTR (address of Acpi debug memory buffer)
        //
        if ((*CurrPtr == AML_NAME_OP) && *Signature == SIGNATURE_32 ('D', 'P', 'T', 'R')) {
            NamePtr = (NAME_LAYOUT *) CurrPtr;
            NamePtr->Value  = (UINT32)(UINTN)mAcpiDebug;
            TRACE((TRACE_ALWAYS, "Patch DPTR NamePtr->Value = 0x%x\n", NamePtr->Value));
            UpdateCounter++;
        }

        //
        // patch EPTR (end of Acpi debug memory buffer)
        //
        if ((*CurrPtr == AML_NAME_OP) && *Signature == SIGNATURE_32 ('E', 'P', 'T', 'R')) {
            NamePtr = (NAME_LAYOUT *) CurrPtr;
            NamePtr->Value  = (UINT32) mBufferEnd;
            TRACE((TRACE_ALWAYS, "Patch EPTR NamePtr->Value = 0x%x\n", NamePtr->Value));
            UpdateCounter++;
        }

        //
        // patch CPTR (used as an index that starts after the buffer signature)
        //
        if ((*CurrPtr == AML_NAME_OP) && *Signature == SIGNATURE_32 ('C', 'P', 'T', 'R')) {
            NamePtr = (NAME_LAYOUT *) CurrPtr;
            NamePtr->Value  = (UINT32) mBufferIndex;
            TRACE((TRACE_ALWAYS, "Patch CPTR NamePtr->Value = 0x%x\n", NamePtr->Value));
            UpdateCounter++;
        }
    }
    
    //
    // Add the table
    //
    Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
    ASSERT_EFI_ERROR (Status);
    
    TableKey = 0;
    Status = AcpiTable->InstallAcpiTable (
        AcpiTable,
        TableHeader,
        Size,
        &TableKey
        );
    ASSERT_EFI_ERROR (Status);
    
return ;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  InitializeAcpiDebug
//
// Description: 
//  Driver entry.
//
// Input:       
//      ImageHandle   - Pointer to the loaded image protocol for this driver
//      SystemTable   - Pointer to the EFI System Table
//
// Output:      
//      retval EFI_SUCCESS   The driver initializes correctly.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
EFIAPI
InitializeAcpiDebug (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    PHYSICAL_ADDRESS BaseAddressMem;
    EFI_STATUS Status;
    EFI_SMM_SW_DISPATCH2_PROTOCOL *SwDispatch;
    EFI_SMM_SW_REGISTER_CONTEXT SwContext;
    EFI_HANDLE SwHandle;
    
    InitAmiLib(ImageHandle, SystemTable);

    TRACE((TRACE_ALWAYS, "ACPI Debug Begin...\n"));
    
    //
    // Reserve 64kb buffer of system memory to store Acpi Debug data.
    //
    BaseAddressMem = 0xFFFFFFFF;
    Status = gBS->AllocatePages (
        AllocateMaxAddress,
        EfiReservedMemoryType,
        EFI_SIZE_TO_PAGES (ACPI_DEBUG_BUFFER_SIZE + ACPI_DEBUG_BUFFER_EX_SIZE),
        &BaseAddressMem);
    ASSERT_EFI_ERROR(Status);
    
    //
    // Clear the 64kb buffer
    //
    mBufferIndex = (UINT32) BaseAddressMem;
    mBufferEnd = mBufferIndex + ACPI_DEBUG_BUFFER_SIZE;

    //
    // init ACPI DEBUG buffer to lower case 'x' 
    //
    SetMem ((VOID *)(UINTN)BaseAddressMem, ACPI_DEBUG_BUFFER_SIZE, 0x78);

    //
    // Clear header of AD_SIZE bytes: signature /current head pointer / current tail pointer /reserved
    //
    SetMem ((VOID *)(UINTN)BaseAddressMem, AD_SIZE, 0);
    
    //
    // Write a signature to the first line of the buffer, "INTEL ACPI DEBUG".
    //
    mBufferIndex = (UINT32)BaseAddressMem;
    CopyMem ((VOID *)(UINTN)mBufferIndex, ACPI_DEBUG_STR, sizeof(ACPI_DEBUG_STR) - 1);
    
    //
    // leave the Index after the signature
    //
    mBufferIndex += sizeof (ACPI_DEBUG_HEAD);
    
    TRACE((TRACE_ALWAYS, "AcpiDebugAddress - 0x%08x\n", BaseAddressMem));
    mAcpiDebug = (ACPI_DEBUG_HEAD *)BaseAddressMem;
    
    //
    // Load the SSDT ACPI Tables.
    //
    LoadAcpiTables ();
    
    //PcdSet32 (PcdAcpiDebugAddress, (UINT32)BaseAddressMem);

    //
    // Get the Sw dispatch protocol and register SMI callback functions.
    //
    SwDispatch = NULL;
    Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&SwDispatch);
    ASSERT_EFI_ERROR (Status);
    SwContext.SwSmiInputValue = (UINTN) SWSMI_SMC_ACPI_DEBUG;
    Status = SwDispatch->Register (SwDispatch, AcpiDebugSmmCallback, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
        TRACE((TRACE_ALWAYS, "Register SW SMI failed for ACPI Debug.\n"));
        return Status;
    }
    mAcpiDebug->SmiTrigger  = (UINT8) SwContext.SwSmiInputValue;
    mAcpiDebug->BufferSize  = ACPI_DEBUG_BUFFER_SIZE;
    mAcpiDebug->Head = (UINT32) mBufferIndex;
    mAcpiDebug->Tail = (UINT32) mBufferIndex; 
    
    return EFI_SUCCESS;
}

