//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

  Reference Code

  ESS - Enterprise Silicon Software

  INTEL CONFIDENTIAL

  Copyright 2006 - 2016 Intel Corporation All Rights Reserved.

  The source code contained or described herein and all documents
  related to the source code ("Material") are owned by Intel
  Corporation or its suppliers or licensors. Title to the Material
  remains with Intel Corporation or its suppliers and licensors.
  The Material contains trade secrets and proprietary and confidential
  information of Intel or its suppliers and licensors. The Material
  is protected by worldwide copyright and trade secret laws and treaty
  provisions.  No part of the Material may be used, copied, reproduced,
  modified, published, uploaded, posted, transmitted, distributed, or
  disclosed in any way without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other
  intellectual property right is granted to or conferred upon you
  by disclosure or delivery of the Materials, either expressly,
  by implication, inducement, estoppel or otherwise. Any license
  under such intellectual property rights must be express and
  approved by Intel in writing.

@file
  SysFuncChipCommon.h

@brief
  This file contains chip common functions

**/
#ifndef  _sysfuncchipcommon_h
#define  _sysfuncchipcommon_h

#include "SysHost.h"

#ifndef MINIBIOS_BUILD
#include "Uefi.h"   // We need this for the EFI_XXXXX errorcode macros
//#include <Cpu\CpuBaseLib.h>
#endif // MINIBIOS_BUILD

#include "PlatformFuncCommon.h"
#include "PlatformApi.h"
#include "MemFunc.h"
#include "SysFunc.h"

//
// Global defines
//
#define  SUCCESS  0
#define  FAILURE  1
#define  RETRY    2

#ifndef  NULL
#define  NULL ((void *) 0)
#endif

#ifndef NT32_BUILD
#ifdef MINIBIOS_BUILD
#define EFIERR(a)                 (0x80000000 | (a))
#define EFI_ERROR(a)              (((INT32) (a)) < 0)
#define EFI_SUCCESS               0
#define EFI_UNSUPPORTED           EFIERR (3)
#define EFI_BUFFER_TOO_SMALL      EFIERR (5)
#define EFI_DEVICE_ERROR          EFIERR (7)
#define EFI_OUT_OF_RESOURCES      EFIERR (9)
#define EFI_TIMEOUT               EFIERR (18)
typedef struct {
  UINT32  RegEax;
  UINT32  RegEbx;
  UINT32  RegEcx;
  UINT32  RegEdx;
} EFI_CPUID_REGISTER;
#endif // MINIBIOS_BUILD
#endif // #ifndef NT32_BUILD

#define  BUS_PCH  2
//
// This macro must be used to access only ICH/PCH regs using
// PCI access routines in PciAccess.c. Note that bit[27:20] is encoded with 2 to indicate PCH/ICH bus
// to distinguish with IIO/Uncore buses.
//
#define REG_ADDR( bus, dev, func, reg, size ) ((size << 28) + ((bus + BUS_PCH) << 20) + (dev << 15) + (func << 12) + reg)

//
// PrintfChip.c
//
void   debugPrintMmrc (UINT32 dbgLevel, char* Format, ...);            // MMRC

//
// MemoryQpiInit.c / MailBox.c
//
UINT64_STRUCT WriteBios2VcuMailboxCommand (struct sysHost *host, UINT8 socket, UINT32 dwordCommand, UINT32 dworddata);       // CHIP
//
// DebugCmd.c
//
#define SERIAL_CONSOLE_PIPE      0
#define SERIAL_BSD_PIPE          1
#define SERIAL_THREAD_PIPE       2

//
// CpuInit.c
//

CPU_STATUS ProgramProcessorFlexRatio( struct sysHost  *host );    // NONE
CPU_STATUS ProgramProcessorUncoreRatio( struct sysHost  *host );  // NONE
VOID SetActiveCoresAndSmtEnableDisable ( struct sysHost *host );  // NONE

#if !defined(SIM_BUILD) && defined(IA32) || defined(RC_SIM)

#ifdef __GNUC__
#define PUSH_EXIT_FRAME(exitFrame, exitVector) \
   asm (\
        "pushfl\n\t" \
        "push  %%esi\n\t" \
        "push  %%edi\n\t" \
        "push  %%ebp\n\t" \
        "push  %%edi\n\t" \
        "mov   %%esp, %0\n\t" \
        "add   $4, %0\n\t" \
        "mov   mrcExit, %1\n\t" \
        "pop   %%edi\n\t"\
        : "=r" (exitFrame), "=r" (exitVector)\
        : "r" (exitFrame), "r" (exitVector)\
        :\
       );

#define POP_EXIT_FRAME(status) \
    asm (\
        "mov   %0, %%eax\n\t" \
        "mrcExit:\n\t" \
        "pop   %%ebp\n\t" \
        "pop   %%edi\n\t" \
        "pop   %%esi\n\t" \
        "popfl\n\t" \
        "mov   %%eax, %0\n\t"\
        : "=r" (status)\
        : "r" (status)\
        :\
       );
#else
#define PUSH_EXIT_FRAME(exitFrame, exitVector) \
   _asm {pushfd} \
   _asm {push  esi} \
   _asm {push  edi} \
   _asm {push  ebp} \
   _asm {push  edi} \
   _asm {mov   exitFrame, esp} \
   _asm {add   exitFrame, 4} \
   _asm {mov   exitVector, OFFSET mrcExit} \
   _asm {pop   edi}

#define POP_EXIT_FRAME(status) \
   _asm {mov   eax, status} \
   _asm {mrcExit:} \
   _asm {pop   ebp} \
   _asm {pop   edi} \
   _asm {pop   esi} \
   _asm {popfd} \
   _asm {mov   status, eax}
#endif

#else //  !defined(SIM_BUILD) && defined(IA32)

#define PUSH_EXIT_FRAME(exitFrame, exitVector)
#define POP_EXIT_FRAME(status)

#endif //  !defined(SIM_BUILD) && defined(IA32)


//
// CpuPciAccess.c
//

/**

  Computes address of CPU Uncore & IIO PCI configuration space using the MMIO mechanism

  @param host      - Pointer to sysHost, the system host (root) structure
  @param SocId     - CPU Socket Node number (Socket ID)
  @param BoxInst   - Box Instance, 0 based
  @param Offset    - Register offset; values come from the auto generated header file
  @param Size      - Register size in bytes (may be updated if pseudo-offset)

  @retval Address

**/
UINT8  *GetCpuPciCfgAddress (PSYSHOST host, UINT8 SocId, UINT8 BoxInst, UINT32 Offset, UINT8 *Size); // CHIP / QPI
UINT8  GetMaxImc (PSYSHOST host);                                                                    // CHIP
VOID   GetCpuCsrAccessVar (PSYSHOST host, CPU_CSR_ACCESS_VAR *CpuCsrAccessVar);                       // CHIP
VOID   CpuCsrAccessCount (PSYSHOST host);                                                             // CHIP

//
// CpuPciAccessCommon.c
//

/**

  CsrAccess specific print to serial output

  @param host      - Pointer to the system host (root) structure
  @param Format    - string format

  @retval None

**/
VOID CpuCsrAccessError (PSYSHOST host, char* Format, ...);                                          // CHIP

// 
// MmrcLibraries.c
//
MRC_STATUS MmrcEntry (IN  OUT   PSYSHOST   MrcData);   // CHIP

#define ISNODEONLINED(host) \
  (host->var.common.memHpSupport && \
  host->var.mem.hotPlugMode)

#define ISMEMINITED(host, node) \
  (host->var.mem.memNodeHP == node && \
  (host->var.mem.actionHP & BIT4) == BIT4)

#define ISMEMMAPPED(host, node) \
  (host->var.mem.memNodeHP == node && \
  (host->var.mem.actionHP & BIT5) == BIT5)

#define GETONLINEACTION(host, act)   ((host->var.mem.actionHP & act) == act)

#define SKIPTHISNODE(host, node) \
  ((ISNODEONLINED(host) == TRUE) ? ((ISMEMINITED(host, node) || ISMEMMAPPED(host, node)) ? FALSE : TRUE) : \
  (((host->var.mem.memRiserPresent & (1 << node)) == FALSE) ? TRUE : FALSE))

#endif   // _sysfuncchip_h
