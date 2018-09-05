//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file

  Include file for Enhanced Intel Speedstep Technology

  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Eist.h

**/

#ifndef _CPU_EIST_H_
#define _CPU_EIST_H_

#include "Cpu.h"

typedef struct {
  BOOLEAN EnableEist;
  BOOLEAN EnableTurbo;
  BOOLEAN EnableHwCoordination;
} EIST_CONFIG;

//
//EPSDIP_EIST_START
//
#define MSR_IA32_PERF_CTL               0x0199
#define MSR_IA32_MISC_ENABLES           0x01A0
#define EIST_ENABLE_OFFSET              16
#define EIST_ENABLE                     (1 << EIST_ENABLE_OFFSET)
#define P_STATE_TARGET_MASK             (0xFF << 8)
#define P_STATE_TARGET_OFFSET           8
#define MAX_VALUE_UINT8                 255
//
//EPSDIP_EIST_END
//

/**
  Detect capability of EIST feature for specified processor.
  
  This function detects capability of EIST feature for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
EistDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for EIST feature for all processors.
  
  This function configures Processor Feature Lists for EIST feature for all processors.

**/
VOID
EistConfigFeatureList (
  VOID
  );

/**
  Produces entry of EIST feature in Register Table for specified processor.
  
  This function produces entry of EIST feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
EistReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

//
//EPSDIP_EIST_START
//PURLEY_OVERRIDE_5372787_BEGIN
//
STATIC
VOID 
SetMaxNonTurboPReqFrequency (
  UINTN     ProcessorNumber
  );
//
//PURLEY_OVERRIDE_5372787_END
//EPSDIP_EIST_END
//
#endif
