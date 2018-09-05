//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file MemorySetupData.h
    
  GUID used for Memory Setup Data entries in the HOB list.

**/

#ifndef _MEMORY_SETUP_DATA_GUID_H_
#define _MEMORY_SETUP_DATA_GUID_H_

#define EFI_MEMORY_SETUP_GUID \
  { \
    0x3eeff35f,0x147c,0x4cd1,0xa2,0x34,0x92,0xa0,0x69,0x70,0x0d,0xb6 \
  }

extern EFI_GUID gEfiMemorySetupGuid;
extern CHAR16   EfiMemorySetupVariable[];

#endif
