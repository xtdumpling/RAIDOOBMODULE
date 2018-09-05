/** @file
  The GUID definition for MeFwHob

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

#ifndef _ME_FW_HOB_H_
#define _ME_FW_HOB_H_

#include <Pi/PiHob.h>
#include <FwStsSmbiosTable.h>

extern EFI_GUID gMeFwHobGuid;

#pragma pack(push,1)

typedef struct {
  UINT32  FunNumber;
  UINT32  Reg[MAX_FWSTS_REG];
} FWSTS_REG_MAP;

typedef struct {
  EFI_HOB_GUID_TYPE     EfiHobGuidType;
  UINT8                 Revision;
  UINT8                 Count;
  UINT16                Reserved;
  FWSTS_REG_MAP         Group[HECI_MAX_FUNC];
} ME_FW_HOB;
#pragma pack(pop)
#endif
