/** @file
  File to contain all the hardware specific stuff for the Smm Gpi dispatch protocol.

@copyright
 Copyright (c) 1999 - 2015 Intel Corporation. All rights reserved
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
#include "PchSmmHelpers.h"

//
// Structure for GPI SMI is a template which needs to have
// GPI Smi bit offset and Smi Status & Enable registers updated (accordingly
// to choosen group and pad number) after adding it to SMM Callback database
//

GLOBAL_REMOVE_IF_UNREFERENCED CONST PCH_SMM_SOURCE_DESC PCH_GPI_SOURCE_DESC_TEMPLATE = {
  PCH_SMM_NO_FLAGS,
  {
    {
      {
        GPIO_ADDR_TYPE, {0x0}
      },
      S_PCH_GPIO_GP_SMI_EN, 0x0,
    },
    NULL_BIT_DESC_INITIALIZER
  },
  {
    {
      {
        GPIO_ADDR_TYPE, {0x0}
      },
      S_PCH_GPIO_GP_SMI_STS, 0x0,
    }
  },
  {
    {
      ACPI_ADDR_TYPE,
      {R_PCH_SMI_STS}
    },
    S_PCH_SMI_STS,
    N_PCH_SMI_STS_GPIO_SMI
  }
};
