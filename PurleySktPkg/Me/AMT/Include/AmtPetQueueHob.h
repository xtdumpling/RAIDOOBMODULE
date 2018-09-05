/** @file
  AmtForcePushPetHob Guid definitions

@copyright
 Copyright (c) 2013 - 2016 Intel Corporation. All rights reserved
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
#ifndef _AMT_PET_QUEUE_HOB_H_
#define _AMT_PET_QUEUE_HOB_H_


///
/// Queue Hob.
/// This hob is used by PEI Status Code generic driver before Intel AMT PEI status code driver
/// starts.
/// 1) If AMT driver is not ready yet, StatusCode driver can save information
///    to QueueHob.
/// 2) If after AMT driver start, but ME is not ready, the AMT
///    driver can also save information to QueueHob.
/// Later, when ME is ready, AMT driver will send all the
/// message in the QueueHob.
///
typedef struct {
  EFI_HOB_GUID_TYPE     EfiHobGuidType; ///< The GUID type hob header
  EFI_STATUS_CODE_VALUE Value;          ///< Status code value
  EFI_STATUS_CODE_TYPE  Type;           ///< Status code type
} AMT_PET_QUEUE_HOB;

extern EFI_GUID gAmtPetQueueHobGuid;

#endif
