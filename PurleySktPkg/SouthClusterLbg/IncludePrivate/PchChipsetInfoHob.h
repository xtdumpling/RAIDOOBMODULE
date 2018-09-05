/** @file
  This file contains definitions of PCH Chipset Info HOB.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
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
#ifndef _PCH_CHIPSET_INFO_HOB_H_
#define _PCH_CHIPSET_INFO_HOB_H_

typedef struct _CHIPSET_INIT_INFO_HOB {
  EFI_HOB_GUID_TYPE Header;
  UINT16            MeChipInitCrc;
} CHIPSET_INIT_INFO_HOB;

extern EFI_GUID gPchChipsetInitHobGuid;

#endif // _PCH_CHIPSET_INFO_HOB_H_

