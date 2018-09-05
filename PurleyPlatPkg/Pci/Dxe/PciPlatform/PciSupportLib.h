//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to  
// the additional terms of the license agreement               
//
/** 

Copyright (c)  1999 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


    @file EfiPciSupportLib.h

  Support PCI chipset initialization. 

**/
#ifndef  _EFI_PCI_SUPPORT_H_
#define  _EFI_PCI_SUPPORT_H_

#include <Protocol/PciRootBridgeIo.h>

#include <Guid/SetupVariable.h>

typedef struct {
  UINT8  PcieCapPtr;
  UINT8  Function;
  UINT8  Device;  
  UINT8  Bus;
  UINT16 PcieLnkCap;
  UINT16 PcieDevCap;
  //Added to Support AtomicOp Request-->Start
  UINT16 PcieDevCap2;
  //Added to Support AtomicOp Request-->End
} PCIE_CAP_INFO;

typedef struct {
  INTN            Top;
  PCIE_CAP_INFO   PcieCapInfo[FixedPcdGet32(PcdMaxNestedLevel)];
} PCIE_STACK;

extern  PCIE_STACK  mPcieStack;

BOOLEAN
IsPciDevicePresent (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo,
  PCI_TYPE00                          *Pci,
  UINT8                               Bus,
  UINT8                               Device,
  UINT8                               Func
  );
  

#endif
