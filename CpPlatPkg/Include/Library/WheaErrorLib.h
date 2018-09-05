//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This file implement runtime library functions for creating Whea 
  error logs. 

  Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _WHEA_ERROR_LIB_H_
#define _WHEA_ERROR_LIB_H_

//
// Includes
//
#include <Library/BaseMemoryLib.h>
#include <Library/PciExpressLib.h>
#include <Library/WheaPlatformHooksLib.h>

#include <IndustryStandard/Pci22.h>
#include <IndustryStandard/WheaAcpi.h>


#define EFI_PCI_WHEA_ADDRESS(seg, bus, dev, func, reg) \
    ((UINTN) ((((UINTN) bus) << 20) + (((UINTN) dev) << 15) + (((UINTN) func) << 12) + ((UINTN) reg)))

//
//      Runtime Whea Error data log functions
//

EFI_STATUS
WheaGenElogMemory(
    UINT16                    ErrorType,
    MEMORY_DEV_INFO           *MemInfo,
    GENERIC_ERROR_STATUS      *ErrStsBlk
  );

EFI_STATUS
WheaGenElogPcieRootDevBridge(
    UINT16                      ErrorType,
    PCIE_PCI_DEV_INFO           *ErrPcieDev,
    GENERIC_ERROR_STATUS        *ErrStsBlk
  );

EFI_STATUS
WheaGenElogPciDev(
    UINT16                      ErrorType,
    PCIE_PCI_DEV_INFO           *ErrPcieDev,
    GENERIC_ERROR_STATUS        *ErrStsBlk
  );

EFI_STATUS
WheaGenElogProcessor(
    IN UINT16                  ErrorType,
    IN CPU_DEV_INFO            *CpuInfo,
    IN GENERIC_ERROR_STATUS    *ErrStsBlk
  );

EFI_STATUS
WheaGenElogIohDev (
    IN UINT16                      ErrorType,
    IN PCIE_PCI_DEV_INFO           *ErrIohDev,
    IN GENERIC_ERROR_STATUS        *ErrStsBlk,
    IN UINT32                      ErrorSource
  );

EFI_STATUS
WheaGenElogNonStandardDev(
    IN UINT16                              ErrorType,
    IN NON_STANDARD_DEV_ERROR_INFO         *ErrNonStdDevInfo,
    IN GENERIC_ERROR_STATUS                *ErrStsBlk
  );
#endif //_WHEA_ERROR_LIB_H_



