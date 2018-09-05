//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c) 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module:

**/
#ifndef __PCI_COMMON_LIB_HELPER_H__
#define __PCI_COMMON_LIB_HELPER_H__
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/PciExpressLib.h>
#include <Library/DebugLib.h>
#include "IndustryStandard/Pci30.h"
#include "Library/PcieCommonInitLib.h"
#include <Library/S3PciLib.h>
#include <Library/UsraAccessApi.h>

//
// function prototypes
//
UINT8
PcieFindCapId (
  USRA_ADDRESS            *PcieAddress,
  UINT8                   CapId
  )
/*++

Routine Description:
  
  Find the Offset to a given Capabilities ID
  CAPID list:
    0x01 = PCI Power Management Interface
    0x04 = Slot Identification
    0x05 = MSI Capability
    0x10 = PCI Express Capability
    
Arguments:
 
  Bus                     Pci Bus Number
  Device                  Pci Device Number
  Function                Pci Function Number
  CapId                   CAPID to search for

Returns:
  
  0                       CAPID not found
  Other                   CAPID found, Offset of desired CAPID

--*/
;

UINT16
PcieFindExtendedCapId (
  USRA_ADDRESS            *PcieAddress,
  UINT16                  CapId
  )
/*++

Routine Description:
  
  Search and return the offset of desired Pci Express Capability ID
  CAPID list:
    0x0001 = Advanced Error Reporting Capability
    0x0002 = Virtual Channel Capability
    0x0003 = Device Serial Number Capability
    0x0004 = Power Budgeting Capability
    
Arguments:
  
  Bus                     Pci Bus Number
  Device                  Pci Device Number
  Function                Pci Function Number
  CapId                   Extended CAPID to search for

Returns:
  
  0                       CAPID not found
  Other                   CAPID found, Offset of desired CAPID

--*/
;

BOOLEAN
IsPciDevicePresent (
  USRA_ADDRESS              *PcieAddress,
  PCI_TYPE00                *Pci
  );



VOID PcieProgramOpTable(
   PCI_LIB_OP_STRUCT        *OpTable,
   UINT32                   OpTableSize
   );

VOID
ResetPcieStack (
  PCIE_LIB_STACK            *PcieStack
);

VOID
PushPcieStack (
  PCIE_LIB_STACK            *PcieStack,
  IN USRA_PCIE_ADDR_TYPE    *PcieAddress
);


VOID
PopPcieStack (
  PCIE_LIB_STACK            *PcieStack
);

#endif


