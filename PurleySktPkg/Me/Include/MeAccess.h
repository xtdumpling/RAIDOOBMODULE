/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2010-2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MeAccess.h

  Macros to simplify and abstract the interface to PCI configuration.

**/
#ifndef _ME_ACCESS_H_
#define _ME_ACCESS_H_
#include <Library/IoLib.h>
#include "Library/MmPciBaseLib.h"
#include "MeChipset.h"
#include "MkhiMsgs.h"

//
// HECI PCI Access Macro
//
#define HeciPciRead32(Register) MmioRead32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register)

#define HeciPciWrite32(Register, Data) \
  MmioWrite32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register, \
  (UINT32) Data \
  )

#define HeciPciOr32(Register, Data) \
  MmioOr32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register, \
  (UINT32) Data \
  )

#define HeciPciAnd32(Register, Data) \
  MmioAnd32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register, \
  (UINT32) Data \
  )

#define HeciPciAndThenOr32(Register, AndData, OrData) \
  MmioAndThenOr32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register, \
  (UINT32) AndData, \
  (UINT32) OrData \
  )

#define HeciPciRead16(Register) MmioRead16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register)

#define HeciPciWrite16(Register, Data) \
  MmioWrite16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register, \
  (UINT16) Data \
  )

#define HeciPciOr16(Register, Data) \
  MmioOr16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register, \
  (UINT16) Data \
  )

#define HeciPciAnd16(Register, Data) \
  MmioAnd16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register, \
  (UINT16) Data \
  )

#define HeciPciAndThenOr16(Register, AndData, OrData) \
  MmioAndThenOr16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register, \
  (UINT16) AndData, \
  (UINT16) OrData \
  )

#define HeciPciRead8(Register)  MmioRead8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register)

#define HeciPciWrite8(Register, Data) \
  MmioWrite8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register, \
  (UINT8) Data \
  )

#define HeciPciOr8(Register, Data) \
  MmioOr8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register, \
  (UINT8) Data \
  )

#define HeciPciAnd8(Register, Data) \
  MmioAnd8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER) + Register, \
  (UINT8) Data \
  )

#define HeciPciAndThenOr8(Register, AndData, OrData) \
  MmioAndThenOr8 ( \
  (UINT8) AndData, \
  (UINT8) OrData \
  )

//
// HECI2 PCI Access Macro
//
#define Heci2PciRead32(Register)  MmioRead32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register)

#define Heci2PciWrite32(Register, Data) \
  MmioWrite32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT32) Data \
  )

#define Heci2PciOr32(Register, Data) \
  MmioOr32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT32) Data \
  )

#define Heci2PciAnd32(Register, Data) \
  MmioAnd32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT32) Data \
  )

#define Heci2PciAndThenOr32(Register, AndData, OrData) \
  MmioAndThenOr32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT32) AndData, \
  (UINT32) OrData \
  )

#define Heci2PciRead16(Register)  MmioRead16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register)

#define Heci2PciWrite16(Register, Data) \
  MmioWrite16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT16) Data \
  )

#define Heci2PciOr16(Register, Data) \
  MmioOr16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT16) Data \
  )

#define Heci2PciAnd16(Register, Data) \
  MmioAnd16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT16) Data \
  )

#define Heci2PciAndThenOr16(Register, AndData, OrData) \
  MmioAndThenOr16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT16) AndData, \
  (UINT16) OrData \
  )

#define Heci2PciRead8(Register) MmioRead8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register)

#define Heci2PciWrite8(Register, Data) \
  MmioWrite8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT8) Data \
  )

#define Heci2PciOr8(Register, Data) \
  MmioOr8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT8) Data \
  )

#define Heci2PciAnd8(Register, Data) \
  MmioAnd8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT8) Data \
  )

#define Heci2PciAndThenOr8(Register, AndData, OrData) \
  MmioAndThenOr8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER) + Register, \
  (UINT8) AndData, \
  (UINT8) OrData \
  )

//
// HECI3 PCI Access Macro
//
#define Heci3PciRead32(Register)  MmioRead32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register)

#define Heci3PciWrite32(Register, Data) \
  MmioWrite32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT32) Data \
  )

#define Heci3PciOr32(Register, Data) \
  MmioOr32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT32) Data \
  )

#define Heci3PciAnd32(Register, Data) \
  MmioAnd32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT32) Data \
  )

#define Heci3PciAndThenOr32(Register, AndData, OrData) \
  MmioAndThenOr32 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT32) AndData, \
  (UINT32) OrData \
  )

#define Heci3PciRead16(Register)  MmioRead16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register)

#define Heci3PciWrite16(Register, Data) \
  MmioWrite16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT16) Data \
  )

#define Heci3PciOr16(Register, Data) \
  MmioOr16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT16) Data \
  )

#define Heci3PciAnd16(Register, Data) \
  MmioAnd16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT16) Data \
  )

#define Heci3PciAndThenOr16(Register, AndData, OrData) \
  MmioAndThenOr16 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT16) AndData, \
  (UINT16) OrData \
  )

#define Heci3PciRead8(Register) MmioRead8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register)

#define Heci3PciWrite8(Register, Data) \
  MmioWrite8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT8) Data \
  )

#define Heci3PciOr8(Register, Data) \
  MmioOr8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT8) Data \
  )

#define Heci3PciAnd8(Register, Data) \
  MmioAnd8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT8) Data \
  )

#define Heci3PciAndThenOr8(Register, AndData, OrData) \
  MmioAndThenOr8 (MmPciBase(ME_BUS, ME_DEVICE_NUMBER, HECI3_FUNCTION_NUMBER) + Register, \
  (UINT8) AndData, \
  (UINT8) OrData \
  )

#ifndef MmPciAddress
#define MmPciAddress(Segment, Bus, Device, Function, Register) \
  ( (UINTN) PcdGet64 (PcdPciExpressBaseAddress) + \
    (UINTN)(Bus << 20) + \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) + \
    (UINTN)(Register) \
  )
#endif

#define MmPci32Ptr(Segment, Bus, Device, Function, Register) \
    ((VOLATILE UINT32 *) MmPciAddress (Segment, Bus, Device, Function, Register))
#define MmPci32(Segment, Bus, Device, Function, Register) *MmPci32Ptr (Segment, Bus, Device, Function, Register)

//
// ME Macro Setting
//
#define McD0PciCfg64(Register)              MmPci64 (0, MC_BUS, 0, 0, Register)
#define McD0PciCfg32And(Register, AndData)  PciAnd32 (MmPciBase (MC_BUS, 0, 0) + Register, (UINT32) AndData)
#define McD0PciCfg16And(Register, AndData)  PciAnd16 (MmPciBase (MC_BUS, 0, 0) + Register, (UINT16) AndData)

///
/// Device 22, Function 0
///
#define PchD22PciCfg32(Register)                            MmPci32 (0, 0, 22, 0, Register)
#define PchD22PciCfg32Or(Register, OrData)                  MmPci32Or (0, 0, 22, 0, Register, OrData)
#define PchD22PciCfg32And(Register, AndData)                MmPci32And (0, 0, 22, 0, Register, AndData)
#define PchD22PciCfg32AndThenOr(Register, AndData, OrData)  MmPci32AndThenOr (0, 0, 22, 0, Register, AndData, OrData)

//
//  MMIO access macros
//
#define MmioAddress(BaseAddr, Register) ((UINTN) BaseAddr + (UINTN) (Register))

//
// 16 bit MMIO access
//
#define Mmio16Ptr(BaseAddr, Register) ((VOLATILE UINT16 *) MmioAddress (BaseAddr, Register))

#define Mmio16(BaseAddr, Register)    *Mmio16Ptr (BaseAddr, Register)

#define Mmio16Or(BaseAddr, Register, OrData) \
  Mmio16 (BaseAddr, Register) = (UINT16) \
    (Mmio16 (BaseAddr, Register) | (UINT16) (OrData))

#define Mmio16And(BaseAddr, Register, AndData) \
  Mmio16 (BaseAddr, Register) = (UINT16) \
    (Mmio16 (BaseAddr, Register) & (UINT16) (AndData))

#define Mmio16AndThenOr(BaseAddr, Register, AndData, OrData) \
  Mmio16 (BaseAddr, Register) = (UINT16) \
    ((Mmio16 (BaseAddr, Register) & (UINT16) (AndData)) | (UINT16) (OrData))

//
// 32 bit MMIO access
//
#define Mmio32Ptr(BaseAddr, Register) ((VOLATILE UINT32 *) MmioAddress (BaseAddr, Register))

#define Mmio32(BaseAddr, Register)    *Mmio32Ptr (BaseAddr, Register)

#define Mmio32Or(BaseAddr, Register, OrData) \
  Mmio32 (BaseAddr, Register) = (UINT32) \
    (Mmio32 (BaseAddr, Register) | (UINT32) (OrData))

#define Mmio32And(BaseAddr, Register, AndData) \
  Mmio32 (BaseAddr, Register) = (UINT32) \
    (Mmio32 (BaseAddr, Register) & (UINT32) (AndData))

#define Mmio32AndThenOr(BaseAddr, Register, AndData, OrData) \
  Mmio32 (BaseAddr, Register) = (UINT32) \
    ((Mmio32 (BaseAddr, Register) & (UINT32) (AndData)) | (UINT32) (OrData))

//
// Memory Controller PCI access macros
//
#define MCH_REGION_BASE               (McD0PciCfg64 (MC_MCHBAR_OFFSET) &~BIT0)
#define McMmioAddress(Register)       ((UINTN) MCH_REGION_BASE + (UINTN) (Register))

#define McMmio8Ptr(Register)          ((VOLATILE UINT8 *) McMmioAddress (Register))
#define McMmio8(Register)             *McMmio8Ptr (Register)
#define McMmio8Or(Register, OrData)   (McMmio8 (Register) |= (UINT8) (OrData))
#define McMmio8And(Register, AndData) (McMmio8 (Register) &= (UINT8) (AndData))
#define McMmio8AndThenOr(Register, AndData, OrData) \
    (McMmio8 (Register) = (McMmio8 (Register) & (UINT8) (AndData)) | (UINT8) (OrData))

#endif // _ME_ACCESS_H_
