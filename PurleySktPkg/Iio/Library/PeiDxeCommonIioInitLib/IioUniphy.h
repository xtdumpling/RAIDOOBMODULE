//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioUniphy.h

  This file defines everything that's needed for IIO VData struct.

**/

#ifndef IIO_UNIPHY_H_
#define IIO_UNIPHY_H_

BOOLEAN
IsValidCpuType (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT32            CpuType,
  IN  UINT32            RecipeCpuType
  );

BOOLEAN
IsValidSkuType (
  IN  UINT16            CpuSubType,
  IN  UINT16            RecipeSkuType
  );

BOOLEAN
IsValidStepping (
  IN  IIO_GLOBALS      *IioGlobalData,
  IN  UINT8            Iio,
  IN  UINT32           CpuStepping,
  IN  UINT64           RecipeCpuStepping
  );

BOOLEAN
IsGlobalDfxEnable(
  IN UINT32 PortBitMap
  );

UINT8
GetRegisterType(
  IN  UINT32 RegisterAddress
  );

BOOLEAN
IsValidCpuEntry(
  IN  UINT32     SocBitMap,
  IN  UINT8      IioIndex
  );

VOID
IioWriteLbc (
  IN  IIO_GLOBALS       *IioGlobalData,
  IN  UINT8             IioIndex,
  IN  UINT8             PortIndex,
  IN  UINT8             LbcType,
  IN  UINT32            EvLoadSelectLbcRegister,
  IN  UINT32            EvData
  );

BOOLEAN
IsSrisEnable(
  IN  IIO_GLOBALS *IioGlobalData,
  IN  UINT8        IioIndex,
  IN  UINT8        PortIndex
  );

VOID
IioRxRecipeSettingsAuto (
  IN  IIO_GLOBALS             *IioGlobalData,
  IN UINT8                   IioIndex
  );


#endif /* IIO_UNIPHY_H_*/
