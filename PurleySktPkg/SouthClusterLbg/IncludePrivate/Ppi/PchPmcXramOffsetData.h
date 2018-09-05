/** @file
  PCH policy PPI produced by PCH RC to store PMC XRAM Offset Data.
  This PPI is consumed by the PCH PEI modules.

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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
#ifndef _PCH_PMC_XRAM_OFFSET_DATA_H_
#define _PCH_PMC_XRAM_OFFSET_DATA_H_

///
/// PMC XRAM Offset Data PPI GUID.
///
extern EFI_GUID gPchPmcXramOffsetDataPpiGuid;

typedef struct _PCH_PMC_XRAM_OFFSET_DATA {
  UINT32            PmcXramOffset;
  UINT8             PmcXramHsioCount;
  UINT8             PmcXramAfeCount;
} PCH_PMC_XRAM_OFFSET_DATA;

#endif // _PCH_PMC_XRAM_OFFSET_DATA_H_

