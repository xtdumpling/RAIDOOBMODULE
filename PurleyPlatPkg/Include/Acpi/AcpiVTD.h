//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**
Copyright (c) 1996 - 2006, Intel Corporation.

This source code and any documentation accompanying it ("Material") is furnished
under license and may only be used or copied in accordance with the terms of that
license.  No license, express or implied, by estoppel or otherwise, to any
intellectual property rights is granted to you by disclosure or delivery of these
Materials.  The Materials are subject to change without notice and should not be
construed as a commitment by Intel Corporation to market, license, sell or support
any product or technology.  Unless otherwise provided for in the license under which
this Material is provided, the Material is provided AS IS, with no warranties of
any kind, express or implied, including without limitation the implied warranties
of fitness, merchantability, or non-infringement.  Except as expressly permitted by
the license for the Material, neither Intel Corporation nor its suppliers assumes
any responsibility for any errors or inaccuracies that may appear herein.  Except
as expressly permitted by the license for the Material, no part of the Material
may be reproduced, stored in a retrieval system, transmitted in any form, or
distributed by any means without the express written consent of Intel Corporation.


  @file AcpiVtd.h

  This file describes the contents of the VTD ACPI Support

**/

#ifndef _ACPI_VTD_H
#define _ACPI_VTD_H

//
// Statements that include other files
//
#include <PiDxe.h>
#include <Library/UefiLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/DevicePathLib.h>
#include <Library/HobLib.h>
#include <Acpi/DMARemappingReportingTable.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/DmaRemap.h>
#include <Guid/HobList.h>
#include <Guid/PlatformInfo.h>
#include <Guid/SocketVariable.h>

#define SEGMENT0                        0x00
#define SEGMENT1                        0x01

//
// equates used in DMAR Table.
//
#define EFI_ACPI_DMAR_OEMID                 "INTEL "    // OEM ID - 6 Bytes
#define EFI_ACPI_DMAR_OEM_TABLEID           "INTEL ID"  // OEM Table ID - 8 Bytes
#define EFI_ACPI_DMAR_OEM_REVISION          0x01            
#define EFI_ACPI_DMAR_OEM_CREATOR_ID        0x01            
#define EFI_ACPI_DMAR_OEM_CREATOR_REVISION  0x01            

//EDK2_TODO Compiler is reporting error with below statement.
//Need to resovle later.
//#pragma pack(1)

#define TABLE_SIZE                      2 * 1024

typedef struct {
  UINT32                                Signature;
  EFI_HANDLE                            Handle;
  EFI_DMA_REMAP_PROTOCOL                DmaRemapProt;
  EFI_ACPI_DMAR_DESCRIPTION_TABLE       *Dmar;
  EFI_ACPI_DMAR_DESCRIPTION_TABLE       *DmarOrder;
} EFI_VTD_SUPPORT_INSTANCE;

//EDK2_TODO Compiler is reporting error with below statement.
//Need to resovle later.
//#pragma pack()

#endif
