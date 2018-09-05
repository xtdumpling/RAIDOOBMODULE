/** @file
  PCH policy PPI produced by a platform driver specifying HSIO PTSS Table.
  This PPI is consumed by the PCH PEI modules. This is deprecated.

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
#ifndef PCH_HSIO_PTSS_TABLE_H_
#define PCH_HSIO_PTSS_TABLE_H_

///
/// HSIO PTSS table PPI GUID.
///
extern EFI_GUID  gPchHsioPtssTablePpiGuid;

///
/// The PCH_SBI_PTSS_HSIO_TABLE block describes HSIO PTSS settings for PCH.
///
typedef struct {
  UINT8       SbPortID;
  UINT8       LaneNum;
  UINT8       PhyMode;
  UINT16      Offset;
  UINT32      Value;
  UINT32      BitMask;
} PCH_SBI_PTSS_HSIO_TABLE;

#endif // PCH_HSIO_PTSS_TABLE_H_
