/** @file
  MBP Sensitive PPI

@copyright
 Copyright (c) 2006 - 2014 Intel Corporation. All rights reserved
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
#ifndef _MBP_SENSITIVE_PPI_H_
#define _MBP_SENSITIVE_PPI_H_

#include <MeBiosPayloadData.h>

///
/// MBP Sensitive PPI GUID
/// This PPI provides an interface to store MBP sensitive data in PEI phase
///

extern EFI_GUID gMbpSensitivePpiGuid;

///
/// Revision
///
#define MBP_SENSITIVE_PPI_REVISION_1  1

typedef struct _MBP_SENSITIVE_PPI  MBP_SENSITIVE_PPI;

///
/// MBP SENSITIVE PPI
/// Contains MBP sensitive data which should not be passed on to DXE as an HOB
///
struct _MBP_SENSITIVE_PPI {
  UINT8                       Revision;       ///< Policy structure revision number
  ME_BIOS_PAYLOAD_SENSITIVE   MbpSensitiveData;
};

#endif // MBP_SENSITIVE_PPI