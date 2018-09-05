/** @file
  Header file for PchGbeLib.

@copyright
 Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PCH_GBE_LIB_H_
#define _PCH_GBE_LIB_H_

/**
  Check whether GbE region is valid
  Check SPI region directly since GBE might be disabled in SW.

  @retval TRUE                    Gbe Region is valid
  @retval FALSE                   Gbe Region is invalid
**/
BOOLEAN
PchIsGbeRegionValid (
  VOID
  );

/**
  Returns GbE over PCIe port number based on a soft strap.

  @return                         Root port number (1-based)
  @retval 0                       GbE over PCIe disabled
**/
UINT32
PchGetGbePortNumber (
  VOID
  );

/**
  Check whether LAN controller is enabled in the platform.

  @retval TRUE                    GbE is enabled
  @retval FALSE                   GbE is disabled
**/
BOOLEAN
PchIsGbePresent (
  VOID
  );

/**
  Check whether LAN controller is enabled in the platform.

  @deprecated Use PchIsGbePresent instead.

  @retval TRUE                    GbE is enabled
  @retval FALSE                   GbE is disabled
**/
BOOLEAN
PchIsGbeAvailable (
  VOID
  );

#endif // _PCH_GBE_LIB_H_