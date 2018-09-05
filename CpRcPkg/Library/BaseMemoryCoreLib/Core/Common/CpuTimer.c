/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#include "SysFunc.h"
#ifndef IA32
UINT64
EFIAPI
AsmReadTsc (
  VOID
  );
#endif // !IA32

void
InitCpuTimer (
             PSYSHOST host
             )
    
/*++

  Function to initialize the CPU timer

  @param host  - Pointer to the system host (root) structure

  @retval N/A
    @retval host->nvram.common.cpuFreq = Cpu Frequency (in Hz) intialized
    @retval host->var.common.startTsc = Current Time Stamp Counter value

--*/
{
  UINT64_STRUCT msrReg;

  //
  // Read PLATFORM_INFO MSR[15:8] to get core:bus ratio (max non-turbo ratio)
  //
  msrReg = ReadMsrLocal (MSR_PLATFORM_INFO);

  //
  // Calculate core frequency in Hz
  //
  host->nvram.common.cpuFreq = MulDw64 ((msrReg.lo >> 8) & 0xFF, 100000000);

  host->var.common.cpuFreq = host->nvram.common.cpuFreq;
  //
  // Save initial timestamp for latency measurements
  //
  ReadTsc64 (&host->var.common.startTsc);
}

UINT32
TimeDiff (
         PSYSHOST      host,
         UINT64_STRUCT startCount,
         UINT64_STRUCT endCount,
         UINT32        unit
         )
/*++

  Finds the time delta between two Time Stamp Counter values in the given time units.

  @param host        - Pointer to the system host (root) structure
  @param startCount  - First Time Stamp Counter value
  @param endCount    - Second Time Stamp Counter value
  @param unit        - For Time in Seconds (TDIFF_UNIT_S) = 1
                       For Time in milliseconds (TDIFF_UNIT_MS) = 1000
                       For Time in microseconds (TDIFF_UNIT_US) = 1000000
                       For Time in nanoseconds (TDIFF_UNIT_NS) = 1000000000


  @retval Time delta in provided units

--*/
{
#ifdef IA32
  UINT32        rval = 0;
  UINT64_STRUCT cpuFreq;
  UINT64_STRUCT diff;

  diff.hi = endCount.hi - startCount.hi;
  if (startCount.lo > endCount.lo) {
    diff.hi -= 1;
  }

  diff.lo = endCount.lo - startCount.lo;

  cpuFreq = host->var.common.cpuFreq;

  //
  // Divide diff and cpuFreq by 2 until they fit within 32-bits,
  // This technique reduces precision over longer time periods
  //
  while (diff.hi || cpuFreq.hi) {
    diff    = DivBy2Qw64 (diff);
    cpuFreq = DivBy2Qw64 (cpuFreq);
  }
  //
  // ns unit = 1000000000
  // us unit = 1000000
  // ms unit = 1000
  // s unit  = 1
  // (cycles * unit) / cpuFreq (kHz)
  //
  if (cpuFreq.lo == 0) {
    return 0;
  }

#ifdef __GNUC__
  asm
  (
    "mov %1, %%eax\n\t"
    "mov %2, %%ecx\n\t"
    "mul %%ecx\n\t"
    "mov %%ecx, %3\n\t"
    "div %%ecx\n\t"
    "mov %%eax, %0\n\t"
    : "=m" (rval)
    : "m" (diff.lo), "m" (unit), "m" (cpuFreq.lo)
    :
  );
#else
  _asm
  {
    mov eax, diff.lo
    mov ecx, unit
    mul ecx
    mov ecx, cpuFreq.lo
    div ecx
    mov rval, eax
  }
#endif
#else
  UINT32        rval = 0;
  UINT64_STRUCT cpuFreq;
  UINT64        diffVal, freqVal;
  cpuFreq = host->var.common.cpuFreq;
  diffVal   = (((((UINT64) (endCount.hi)) <<32) | ((UINT64) (endCount.lo))) -
               ((((UINT64) (startCount.hi)) <<32) | ((UINT64) (startCount.lo))));
  freqVal   = ((UINT64) (cpuFreq.hi) <<32) | ((UINT64) (cpuFreq.lo));
  if (freqVal > 0)
  rval      = (UINT32) (diffVal*((UINT64) unit)/freqVal);
#endif

  return rval;
}

void
ReadTsc64 (
          UINT64_STRUCT *count
          )
/*++

  Reads the time stamp counter

  @param count - location to read the time stamp counter into

  @retval N/A

--*/
{
  UINT32  hi = 0;

  UINT32  lo = 0;

#if defined(IA32) || defined(RC_SIM)
#ifdef __GNUC__
  asm
  (
    "rdtsc\n\t"
    "mov %%eax, %0\n\t"
    "mov %%edx, %1\n\t"
    : "=m" (lo), "=m" (hi)
    :
    :
  );
#else
  _asm
  {
    rdtsc
    mov lo, eax
    mov hi, edx
  }
#endif
#else // IA32
  {
    UINT64 tscCounter;
    tscCounter = AsmReadTsc();
    lo = (UINT32)tscCounter;
    hi = (UINT32)(tscCounter >> 32);
  }
#endif // IA32

  count->lo = lo;
  count->hi = hi;
}
