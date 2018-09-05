//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2016 Intel Corporation All Rights Reserved.
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

#ifndef _syshostchip_h
#define _syshostchip_h

#include "CpuHost.h"
#include "SysHost.h"
#include "RcRegs.h"
#include "KtiHost.h"
#include "UsbDebugPort.h"

#define CCMRC_REVISION_CHANGELIST  320717

//
// -----------------------------------------------------------------------------
// Setup structures
//

#ifndef SEGMENT_ACCESS
#define SEGMENT_ACCESS
#endif

// SKX note: use HSX rev ID for now to minimize porting changes
// Planning to remove HSX A0 support and maintain compatibility with B0, C0
//
// Steppings
#define A0_REV      0x00
//
// SKX Steppings
//
#define SKX_LCC_CHOP 0x00  //(U0)
#define SKX_HCC_CHOP 0x02  //(L0),(M0)
#define SKX_XCC_CHOP 0x03  //(A0/A1/A2/B0/B1/H0)

/*++
// SKX   REV_ID  SiliconID  SteppingID  CPUID       ChopType
// A0    0       0          0           0x50650     3
// A1    1       1          0           0x50650     3
// A2    2       2          1           0x50651     3
// B0    3       3          2           0x50652     3
// L0    4       4          2           0x50652     2
// B1    5       5          3           0x50653     3
// H0    6       6          4           0x50654     3 (xcc)
// M0    7       6          4           0x50654     2 (hcc)
// U0    8       6          4           0x50654     0 (lcc)
--*/

//
// Xy_REV_SKX is map to SiliconID in above table except the A0 and A1
// A0 and A1 still keep to 0
//
#define A0_REV_SKX  A0_REV
#define A1_REV_SKX  A0_REV
#define A2_REV_SKX  0x02
#define B0_REV_SKX  0x03
#define L0_REV_SKX  0x04
#define B1_REV_SKX  0x05
#define H0_REV_SKX  0x06
#define M0_REV_SKX  0x07
#define U0_REV_SKX  0x08

//
// Xy_CPU_STEP is from CPUID
//
#define A0_CPU_STEP  0x00
#define A1_CPU_STEP  A0_CPU_STEP
#define A2_CPU_STEP  0x01
#define B0_CPU_STEP  0x02
#define L0_CPU_STEP  0x02
#define B1_CPU_STEP  0x03
#define H0_CPU_STEP  0x04
#define MO_CPU_STEP  0x04
#define U0_CPU_STEP  0x04

//
// HSX Steppings
//
#define A0_REV_HSX  A0_REV
#define B0_REV_HSX  0x01
#define C0_REV_HSX  0x02

#ifndef ASM_INC
#pragma pack(1)




#pragma pack()

//
// Move to a separate header file
// -----------------------------------------------------------------------------
// BIOS to PCode Mailbox CSR Registers
//
#define BIOS_MAILBOX_DATA       BIOS_MAILBOX_DATA_PCU_FUN1_REG
#define BIOS_MAILBOX_INTERFACE  BIOS_MAILBOX_INTERFACE_PCU_FUN1_REG

//
// MSR for WFR Uncore GV Rate Reduction WA
//
#ifndef MSR_UNCORE_RATIO_LIMIT
#define MSR_UNCORE_RATIO_LIMIT        0x0620
#define   B_MAX_CLM_RATIO_SHIFT       0
#define   MAX_CLM_RATIO_MASK          (0x7F) << B_MAX_CLM_RATIO_SHIFT
#define   B_MIN_CLM_RATIO_SHIFT       8
#define   MIN_CLM_RATIO_MASK          (0x7F) << B_MIN_CLM_RATIO_SHIFT
#define   MIN_CLM_RATIO_DEFAULT       12
#endif
#ifndef MSR_UNCORE_PERF_STATUS
#define MSR_UNCORE_PERF_STATUS        0x0621
#define   B_CURRENT_CLM_RATIO_SHIFT   0
#define   CURRENT_CLM_RATIO_MASK      (0x7F) << B_CURRENT_CLM_RATIO_SHIFT
#endif

//
// -----------------------------------------------------------------------------
// BIOS to VCode Mailbox Commands
//

#define VCODE_API_OPCODE_NOP                   0x0
#define VCODE_API_OPCODE_OPEN_SEQUENCE         0x3
#define VCODE_API_OPCODE_SET_PARAMETER         0x1
#define VCODE_API_OPCODE_CLOSE_SEQUENCE        0x4
#define VCODE_API_OPCODE_IOT_LLC_SETUP         0x1007
#define VCODE_API_OPCODE_IOT_REG_READ          0x1003
#define VCODE_API_OPCODE_IOT_REG_WRITE         0x1006
#define VCODE_API_OPCODE_READ_LOCAL_CSR        0x0010
#define VCODE_API_OPCODE_READ_DATA             0x0002
#define VCODE_API_OPCODE_WRITE_LOCAL_CSR       0x0011
#define VCODE_API_OPCODE_WRITE_DATA            0x0006

#define VCODE_API_SEQ_ID_IOT_LLC_SETUP 0x1000e
#define VCODE_API_SEQ_ID_OPEN_SEQUENCE 0x0

#define VCODE_API_SEQ_ID_IOT_TRACE_DUMP_SETUP 0x1000c
#define VCODE_API_OPCODE_IOT_TRACE_DUMP_SETUP 0x1005

#define VCODE_API_SEQ_ID_READ_LOCAL_CSR       0x0001
#define VCODE_API_SEQ_ID_WRITE_LOCAL_CSR      0x0002


// command return code
#define VCODE_MAILBOX_CC_SUCCESS            0x40     // Successful completion
#define VCODE_MAILBOX_CC_TIMEOUT            0x80     // Time-out.
#define VCODE_MAILBOX_CC_THREAD_UNAVAILABLE 0x82     // Thread Unavailable.
#define VCODE_MAILBOX_CC_ILLEGAL            0x90     // Illegal.

#define VCODE_MAILBOX_PECI_CC_SUCCESS 0x40     // Successful completion
#define VCODE_MAILBOX_PECI_CC_ILLEGAL 0x90     // Illegal

#define BIOS_VCU_MAILBOX_TIMEOUT_RETRY 3      // Retry count used by BIOS

//
// Move to a separate header file
// Scratch register usage
//
#define CSR_EMULATION_FLAG_1_11_0_CFG_REG 0x401580b0  // register at B:D:F: 1:11:0, offset 0xB0. with SoftSimulation flag
// -----------------------------------------------------------------------------
// Sticky Scratch Pad regsiter usage
//  B:D:F:  n:11:3: offset 0x40, 0x44, ...,  total 8 registers
//
#define SR_WARMBOOT_MMCFG_DATA_CSR  BIOSSCRATCHPAD7_UBOX_MISC_REG

//
// -----------------------------------------------------------------------------
// Non-Sticky Scratch Pad regsiter usage
//  B:D:F:  n:11:3: offset 0x60, 0x64, ...,  total 16 registers


//
// CSR for maintaining a pointer to the Compatible BIOS Data structure.
//
#define SR_BDAT_STRUCT_PTR_CSR                    (BIOSNONSTICKYSCRATCHPAD5_UBOX_MISC_REG)   // UBOX scratchpad CSR5
#define SR_BDAT_STRUCT_PTR_CSR_MMIO               0x4015b074  // BIOSNONSTICKYSCRATCHPAD5_1_11_3_CFG_REG

//
// CSRs for PIPE/SOCKET/MRC MILESTONES
//
#define SR_PBSP_CHECKIN_CSR                       (BIOSNONSTICKYSCRATCHPAD2_UBOX_MISC_REG)   // UBOX scratchpad CSR02
#define SR_BIOS_SERIAL_DEBUG_CSR                  (BIOSSCRATCHPAD6_UBOX_MISC_REG)            // UBOX scratchpad CSR6
#define SR_POST_CODE_CSR                          (BIOSNONSTICKYSCRATCHPAD7_UBOX_MISC_REG)   // UBOX scratchpad CSR7
#define SR_ERROR_CODE_CSR                         (BIOSNONSTICKYSCRATCHPAD8_UBOX_MISC_REG)   // UBOX scratchpad CSR8
#define SR_PRINTF_SYNC_CSR                        (BIOSNONSTICKYSCRATCHPAD9_UBOX_MISC_REG)   // UBOX scratchpad CSR9
#define SR_RC_REVISION_CSR                        (BIOSNONSTICKYSCRATCHPAD12_UBOX_MISC_REG)  // UBOX scratchpad CSR12
#define PIPE_DISPATCH_SYNCH_PSYSHOST  4
#define SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR  (BIOSNONSTICKYSCRATCHPAD13_UBOX_MISC_REG)  // UBOX scratchpad CSR13
#define SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR   (BIOSNONSTICKYSCRATCHPAD14_UBOX_MISC_REG)  // UBOX scratchpad CSR14
#define SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR      (BIOSNONSTICKYSCRATCHPAD15_UBOX_MISC_REG)  // UBOX scratchpad CSR15

//
// bit definitions for the PCU_BIOS_SPARE2 register
//
#define PCU_BIOS_SPARE2_UDIMM                 0
#define PCU_BIOS_SPARE2_RDIMM                 BIT12
#define PCU_BIOS_SPARE2_LRDIMM                BIT13
#define PCU_BIOS_SPARE2_125V_DIMM             0
#define PCU_BIOS_SPARE2_135V_DIMM             BIT14
#define PCU_BIOS_SPARE2_150V_DIMM             BIT15


#endif // ASM_INC
#endif // _syshostchip_h
