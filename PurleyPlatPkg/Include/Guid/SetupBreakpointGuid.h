/**

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SetupBreakpointGuid.h
  
  Breakpoint GUID definitions

**/


#define EFI_AFTER_MRC_GUID \
  { 0x2bac49bc, 0x5628, 0x4f1b, { 0x95, 0x24, 0x86, 0xbd, 0xda, 0x00, 0x1c, 0x47 } }
#define EFI_AFTER_QPIRC_GUID \
  { 0x6f079b18, 0x0591, 0x4554, { 0xbb, 0x8a, 0xfb, 0x95, 0xb1, 0xa5, 0x3f, 0x6b } }
#define EFI_AFTER_RESOURCE_ALLOC_GUID \
  { 0x76b4d838, 0x6204, 0x4e75, { 0x9b, 0xf9, 0x42, 0x2d, 0x2f, 0x64, 0xbf, 0xa1 } }
#define EFI_AFTER_POST_GUID \
  { 0xf2b76307, 0x0881, 0x4513, { 0xb1, 0xc6, 0xab, 0x5c, 0x7c, 0xc5, 0xbe, 0xe1 } }
#define EFI_AFTER_FULL_SPEED_SETUP_GUID \
  { 0x02812072, 0x6d3d, 0x42b1, { 0x84, 0x24, 0xb3, 0xae, 0x38, 0x05, 0xa0, 0x21 } }
#define EFI_READY_FOR_IBIST_GUID \
  { 0x7bbc005c, 0xab40, 0x4771, { 0x94, 0xc9, 0xe5, 0xa3, 0xa5, 0x05, 0x38, 0x09 } }

  
#define BP_NONE                     0
#define BP_AFTER_MRC                1
#define BP_AFTER_QPIRC              2
#define BP_AFTER_RESOURCE_ALLOC     3
#define BP_AFTER_POST               4
#define BP_AFTER_FULL_SPEED_SETUP   5
#define BP_READY_FOR_IBIST          6
