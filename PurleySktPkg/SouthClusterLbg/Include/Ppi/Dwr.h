/** @file
  This file defines the Dirty Warm Reset Signal

@copyright
 Copyright (c) 2016 Intel Corporation. All rights reserved
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

#ifndef _DWR_H_
#define _DWR_H_

//
// Extern the GUID for PPI users.
//
extern EFI_GUID               gDirtyWarmResetSignalGuid;
extern EFI_GUID               gDirtyWarmResetGuid;

typedef
VOID
(EFIAPI *EXECUTE_DIRTY_WARM_RESET_FLOW) (
  VOID
  );

///
/// PPI definition
///
typedef struct DWR_PPI {
  EXECUTE_DIRTY_WARM_RESET_FLOW      ExecuteDirtyWarmResetFlow;
} DWR_PPI;

#endif // _DWR_H_
