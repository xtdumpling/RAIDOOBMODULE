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


  @file IioWorkAround.h

  This file defines everything that's needed for IIO VData struct.

**/

#ifndef IIOWORKAROUND_H_
#define IIOWORKAROUND_H_

VOID
IioGlobalPermanentWorkarounds (
  IN  IIO_GLOBALS              *IioGlobalData,
  IN  UINT8                    IioIndex
  );

#ifndef MINIBIOS_BUILD
VOID
PciePhyLTSSM (
    IN  IIO_GLOBALS     *IioGlobalData,
    IN  UINT8           IioIndex,
    IN  UINT8           PortIndex
    );


VOID
PciePhyTestMode (
  IN  IIO_GLOBALS              *IioGlobalData
  );
#endif //MINIBIOS_BUILD

VOID
IioOtherMiscellaneousWorkarounds (
  IN  IIO_GLOBALS              *IioGlobalData,
  IN  UINT8                    IioIndex
  );

VOID
ClearXPUncErrStsBit6(
  IN  IIO_GLOBALS          *IioGlobalData,
  IN  UINT8                IioIndex
  );

VOID
IioMiscellaneousCommonWorkarounds (
  IN  IIO_GLOBALS             *IioGlobalData,
  IN  UINT8                   IioIndex
  );

VOID
IioWABeforeBif (
  IN  IIO_GLOBALS             *IioGlobalData,
  IN  UINT8                   IioIndex
  );

VOID
IioMiscCtrlWorkAround(
  IN  IIO_GLOBALS                 *IioGlobalData,
  IN  UINT8                       IioIndex
  );

VOID
IioDisableUnpopulatePciePorts(
  IN IIO_GLOBALS      *IioGlobalData,
  IN UINT8            IioIndex
  );

VOID
IioRxRecipeWorkAround (
    IIO_GLOBALS                     *IioGlobalData,
    UINT8                           IioIndex
  );

VOID
IioLerWorkaround(
  IN IIO_GLOBALS      *IioGlobalData,
  IN UINT8            IioIndex
  );

VOID
IioPostInitWorkaround(
  IN  IIO_GLOBALS                                         *IioGlobalData,
  IN  UINT8                                               IioIndex
  );

VOID
IioWorkAround (
  IN  IIO_GLOBALS                  *IioGlobalData,
  IN  UINT8                        IioIndex,
  IN  IIO_INIT_ENUMERATION         Phase
  );

#endif /* IIOWORKAROUND_H_ */
