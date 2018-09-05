/** @file
    LbgPchH Bx D HSIO Header File

@copyright

 Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
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

#ifdef SKXD_EN
#ifndef _PCH_LBG_HSIO_BX_D_H_
#define _PCH_LBG_HSIO_BX_D_H_

#define PCH_LBG_HSIO_VER_BX_D   0x1

extern UINT8                      PchLbgChipsetInitTable_BxD[2844];
extern PCH_SBI_HSIO_TABLE_STRUCT  *PchLbgHsio_BxD_Ptr;
extern UINT16                     PchLbgHsio_BxD_Size;

#endif //_PCH_LBG_HSIO_BX_D_H_
#endif // SKXD_EN
