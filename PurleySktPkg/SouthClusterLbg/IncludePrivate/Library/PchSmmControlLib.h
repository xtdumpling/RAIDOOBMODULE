/** @file
  Header file for SMM Control PEI Library.

@copyright
  Copyright (c) 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _PCH_SMM_CONTROL_LIB_H_
#define _PCH_SMM_CONTROL_LIB_H_

/**
  This function install PEI SMM Control PPI

  @retval EFI_STATUS  Results of the installation of the SMM Control PPI
**/
EFI_STATUS
EFIAPI
PchSmmControlInit (
  VOID
  );

#endif
