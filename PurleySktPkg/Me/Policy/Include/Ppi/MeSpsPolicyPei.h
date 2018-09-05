/** @file
  ME SPS Policy for ME PEIMs

@copyright
  Copyright (c) 2010 - 2016 Intel Corporation. All rights reserved
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
#ifndef _SPS_POLICY_PPI_H_
#define _SPS_POLICY_PPI_H_

#include <Config/MeSpsConfig.h>


#define SPS_POLICY_PPI_REVISION    1


///
/// Extern the GUID for PPI users.
///
extern EFI_GUID gSpsPolicyPpiGuid;

///
/// PPI definition
///
typedef struct
{
  UINT32        Revision;       ///< Revision for the PPI structure
  
  ME_SPS_CONFIG SpsConfig;
  
} SPS_POLICY_PPI;

#endif // _SPS_POLICY_PPI_H_

