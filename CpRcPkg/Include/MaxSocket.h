//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

//
// This defines the maximum number of sockets supported by some modules.
// It is generally better to use a dynamic solution.
// This is also defined by build tools for some special build
// environments used in validation that do not support EDK II build
// and thus can't use PCD.
//

#ifndef MAX_SOCKET
#define MAX_SOCKET (FixedPcdGet32 (PcdMaxCpuSocketCount))
#endif

