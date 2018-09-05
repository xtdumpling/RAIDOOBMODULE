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


  @file IioDmiInit.h

  This file defines everything that's needed for IIO VData struct.

**/

#ifndef IIO_DMI_INIT_H_
#define IIO_DMI_INIT_H_


VOID
DmiInit (
  IN IIO_INIT_PHASE                     Phase,
  IN IIO_GLOBALS                        *IioGlobalData,
  IN UINT8                              IioIndex
  );

VOID
DmiResourceAssignedInit (
  IN  IIO_GLOBALS                     *IioGlobalData,
  IN  UINT8                           IioIndex,
  IN  UINT8                           PortIndex
  );

VOID
DmiMiscInit (
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  UINT8                          PortIndex
  );

VOID
DmiDeviceInit (
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  UINT8                          PortIndex
  );

VOID
DmiPreInit (
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  UINT8                          PortIndex
  );

VOID
DmiLinkInit (
  IN  IIO_GLOBALS                    *IioGlobalData,
  IN  UINT8                          IioIndex,
  IN  UINT8                          PortIndex
  );



VOID
DmiPostInit (
    IIO_GLOBALS                         *IioGlobalData,
    UINT8                                IioIndex
  );
#endif /* IIO_DMI_INIT_H_*/
