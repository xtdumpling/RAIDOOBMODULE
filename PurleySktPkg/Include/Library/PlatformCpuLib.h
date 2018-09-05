/**

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PlatformCpuLib.h
  

**/

#ifndef __PLATFORM_CPU_LIB__
#define __PLATFORM_CPU_LIB__

//
// Bit defines for Cpu Features enabled for S3 resume
//
#define B_PROC_MSR_LOCK_CTRL      BIT0
#define B_PROC_MSR_ACC_EN         BIT1
#define B_PROC_LLC_PREFETCH_EN    BIT2


/**
  Library call to get the enabled CPU features from the Setup variable and
  pass back these values as a bitmap.

  @param  VOID           Nothing passed in.

  @retval UINT32         Bitmap of enabled CPU features.

**/
UINT32
EFIAPI
GetPlatformEnabledCpuFeatures (
  VOID
  );
  
#endif