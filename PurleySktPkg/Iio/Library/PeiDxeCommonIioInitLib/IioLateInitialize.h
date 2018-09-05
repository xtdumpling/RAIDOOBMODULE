/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioLateInitialize.h

  This file defines everything that's needed for firmware to access
  and manipulate the Intel(R) silicon chip.
  All registers, bits, constants, etc. that are described in the chip's
  EDS are represented here.
  This file usurps all defines that relate to this chip. This means that
  it is inappropriate to define any of such define in any other file.
  Although this file maybe incomplete while under construction (maybe it
  will never be completed as there may be no need for access to all
  features), any new define must go here and follow naming conventions
  set therefore.
  In an ideal world, this file dould originate from the hardware design
  team that designed the chip.

**/
#ifndef _IIO_LATE_INITIALIZE_H_
#define _IIO_LATE_INITIALIZE_H_


VOID 
IioPcieDeviceHide(  
  IN  IIO_GLOBALS                      *IioGlobalData,
  IN  UINT8                            IioIndex,
  IN  IIO_DEVFUNHIDE_TABLE             *IioDevFuncHides
  );


VOID
IioApplyDfxSocketDevFuncHideSettings (
  IN      IIO_GLOBALS              *IioGlobalData,
  IN      UINT8                    IioIndex,
  IN OUT  IIO_DEVFUNHIDE_TABLE     *IioDevFuncHide
  );

VOID
IioApplyIioDevFuncHideSettings (
  IN      IIO_GLOBALS               *IioGlobalData,
  IN      UINT8                      IioIndex,
  IN OUT  IIO_DEVFUNHIDE_TABLE      *IioDevFuncHide
  );

VOID
IioMiscFeatureInit (
  IN  UINT8                          IioIndex,
  IN  IIO_GLOBALS                    *IioGlobalData
  );

#endif
