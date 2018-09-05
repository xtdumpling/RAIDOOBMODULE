//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for SNC feature.

  Copyright (c) 2014, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  CpuSncInit.h

**/

#ifndef _CPU_SNC_INIT_H_
#define _CPU_SNC_INIT_H_


/**
  This function program CPU SNC MSRs

  @param  None

  @retval None

**/
VOID
EFIAPI
CpuSncInitialize (
  VOID
  );

#endif
