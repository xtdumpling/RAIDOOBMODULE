//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file

  Copyright (c) 2011 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef __PCIE_MMCFG_H__
#define __PCIE_MMCFG_H__

#include <UsraAccessType.h>


//////////////////////////////////////////////////////////////////////////
//
// PCIE MMCFG Table definition
// This table was based on PCI Firmware Spec Rev 3.1
//
//////////////////////////////////////////////////////////////////////////

typedef struct 
  {
    UINT8                         Signature[4];       // "MCFG" Signature For this Table
    UINT32                        Length;             // Length, in bytes, include base address allocation structures.
    UINT8                         Revision;           // "1"
    UINT8                         SegMax;             // The Maximum number of Segments
    UINT16                        ValidSegMap;        // Valid Segment Bit Map, LSB Bit0 for Seg0, bit1 for seg1 ...
    UINT8                         Reserved[4];        // Reserved
} PCIE_MMCFG_HEADER_TYPE;

typedef struct 
  {
    UINT32                        BaseAddressL;       // Processor-relative Base Address (Lower 32-bit) for the Enhanced Configuration Access Mechanism
    UINT32                        BaseAddressH;       // Processor-relative Base Address (Upper 32-bit) for the Enhanced Configuration Access Mechanism
    UINT16                        Segment;            // PCI Segment Group Number. Default is 0.
    UINT8                         StartBus;           // Start PCI Bus number decoded by the host bridge
    UINT8                         EndBus;             // End PCI Bus number decoded by the host bridge
    UINT8                         Reserved[4];        // Reserved
} PCIE_MMCFG_BASE_ADDRESS_TYPE;


typedef struct 
  {
    PCIE_MMCFG_HEADER_TYPE        Header;             // The header of MMCFG Table
    PCIE_MMCFG_BASE_ADDRESS_TYPE  MmcfgBase[1];       // First Array of base address allocation structures.
} PCIE_MMCFG_TABLE_TYPE;


/**
  This Lib is used for platform to set platform specific Pcie MMCFG Table
  
  @param[in] MmcfgTable           A pointer of the MMCFG Table structure for PCIE_MMCFG_TABLE_TYPE type
  @param[in] NumOfSeg             Number of Segments in the table

  @retval <>NULL                  The function completed successfully.
  @retval NULL                    Return Error
**/
UINTN
EFIAPI
SetPcieSegMmcfgTable (
  IN PCIE_MMCFG_TABLE_TYPE *MmcfgTable,
  IN UINT32                 NumOfSeg
);

/**
  This Lib return PCIE MMCFG Base Address
  
  @param[in] Address              A pointer of the address of the USRA Address Structure for PCIE type

  @retval <>NULL                  The function completed successfully.
  @retval NULL                    Return Error
**/
UINTN
EFIAPI
GetPcieSegMmcfgBaseAddress (
  IN USRA_ADDRESS    *Address
  );


#endif
