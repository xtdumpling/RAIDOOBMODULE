//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/**

Copyright (c)  2003 - 2003 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file CapsuleOnDataCD.h
    
  Capsule on Data CD GUID.

  This is the contract between the recovery module and device recovery module
  in order to convey the name of a given recovery module type

**/

#ifndef _PEI_CAPSULE_ON_DATA_CD_H
#define _PEI_CAPSULE_ON_DATA_CD_H

#define PEI_CAPSULE_ON_DATA_CD_GUID \
  { \
  0x5cac0099, 0x0dc9, 0x48e5, {0x80, 0x68, 0xbb, 0x95, 0xf5, 0x40, 0x0a, 0x9f } \
  };

extern EFI_GUID gPeiCapsuleOnDataCDGuid;

#endif
