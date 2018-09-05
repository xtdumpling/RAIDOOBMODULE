//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/**

Copyright (c)  1999 - 2001 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file MemoryConfigData.h
    
  GUID used for Memory Configuration Data entries in the HOB list.

**/

#ifndef _MEMORY_CONFIG_DATA_GUID_H_
#define _MEMORY_CONFIG_DATA_GUID_H_

#define EFI_MEMORY_CONFIG_DATA_GUID \
  { \
  0x80dbd530, 0xb74c, 0x4f11, {0x8c, 0x03, 0x41, 0x86, 0x65, 0x53, 0x28, 0x31 }\
  }

extern EFI_GUID gEfiMemoryConfigDataGuid;
extern CHAR16   EfiMemoryConfigVariable[];

#endif
