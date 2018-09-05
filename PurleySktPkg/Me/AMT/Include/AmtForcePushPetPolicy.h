/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c)  2010 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  AmtForcePushPetPolicy.h

@brief
  AmtForcePushPetPolicy Guid definitions

**/
#ifndef _EFI_AMT_FORCE_PUSH_PET_POLICY_GUID_H_
#define _EFI_AMT_FORCE_PUSH_PET_POLICY_GUID_H_

#define AMT_FORCE_PUSH_PET_POLICY_GUID \
  { \
    0xacc8e1e4, 0x9f9f, 0x4e40, 0xa5, 0x7e, 0xf9, 0x9e, 0x52, 0xf3, 0x4c, 0xa5 \
  }

typedef struct {
  EFI_HOB_GUID_TYPE EfiHobGuidType;
  INT32             MessageType[1];
} AMT_FORCE_PUSH_PET_POLICY_HOB;

extern EFI_GUID gAmtForcePushPetPolicyGuid;

#endif
