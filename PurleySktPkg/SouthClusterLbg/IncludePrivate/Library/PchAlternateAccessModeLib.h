/** @file
  Header file for PchAlternateAccessModeLib.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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
#ifndef _PCH_ALTERNATE_ACCESS_MODE_LIB_H_
#define _PCH_ALTERNATE_ACCESS_MODE_LIB_H_

/**
  This function can be called to enable/disable Alternate Access Mode
  @note, the alternate access mode does not support to read NMI_EN# bit.

  @param[in] AmeCtrl              If TRUE, enable Alternate Access Mode.
                                  If FALSE, disable Alternate Access Mode.
**/
VOID
EFIAPI
PchAlternateAccessMode (
  IN  BOOLEAN       AmeCtrl
  );

#endif // _PCH_ALTERNATE_ACCESS_MODE_LIB_H_