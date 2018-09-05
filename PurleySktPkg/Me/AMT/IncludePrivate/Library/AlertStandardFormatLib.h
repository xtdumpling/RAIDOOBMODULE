/** @file
  Include file for ASF Driver

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
#ifndef _ALERT_STANDARD_FORMAT_LIB_H
#define _ALERT_STANDARD_FORMAT_LIB_H

#include <AlertStandardFormat.h>

extern  ASF_DATA_HUB_MAP      mAsfProgressDataHubMap[];
extern  ASF_DATA_HUB_MAP      mAsfErrorDataHubMap[];
extern  ASF_FRAMEWORK_MESSAGE mAsfFrameworkMessage[];
extern  UINTN                 AsfProgressDataHubMapSize;
extern  UINTN                 AsfErrorDataHubMapSize;
extern  UINTN                 AsfFrameworkMessageSize;

#endif //_ALERT_STANDARD_FORMAT_LIB_H