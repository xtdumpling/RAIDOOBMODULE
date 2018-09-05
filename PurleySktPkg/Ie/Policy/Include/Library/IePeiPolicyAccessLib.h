/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
/**

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file IePeiPolicyAccessLib.h

  Header file for PEI IePlatformPolicyUpdate Library.

**/
#ifndef _IE_PEI_POLICY_ACCESS_LIB_H_
#define _IE_PEI_POLICY_ACCESS_LIB_H_

#include "Ppi/IePlatformPolicyPei.h"

/**
  Read soft-strap to determine if IE device is enabled

  param                     None

  retval TRUE               IE device is enabled
  retval FALSE              IE device is disabled
 **/
BOOLEAN IeIsEnabled (
  VOID
  )
;

#endif
