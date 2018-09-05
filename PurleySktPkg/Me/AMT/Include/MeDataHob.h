/** @file
  The GUID definition for MeDataHob

@copyright
 Copyright (c) 2012 - 2014 Intel Corporation. All rights reserved
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

#ifndef _ME_DATA_HOB_H_
#define _ME_DATA_HOB_H_

extern EFI_GUID gMeDataHobGuid;

#ifndef _PEI_HOB_H_
#ifndef __HOB__H__
#ifndef __PI_HOB_H__
typedef struct _EFI_HOB_GENERIC_HEADER {
  UINT16  HobType;
  UINT16  HobLength;
  UINT32  Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct _EFI_HOB_GUID_TYPE {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Name;
  //
  // Guid specific data goes here
  //
} EFI_HOB_GUID_TYPE;
#endif
#endif
#endif

typedef struct {
  EFI_HOB_GUID_TYPE     EfiHobGuidType;
  UINT64                PttBufferAddress;
} ME_DATA_HOB;

#endif
