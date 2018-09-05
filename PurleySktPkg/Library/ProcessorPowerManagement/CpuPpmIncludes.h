/**
//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//

Copyright (c) 2005 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file CpuPpmIncludes.h

  Cpu Power Management Includes that expose local Cpu PM Lib.

**/

#ifndef  _CPU_PPM_INCLUDES_H_
#define  _CPU_PPM_INCLUDES_H_

#include <Library/DebugLib.h>
#include <Library/CpuPpmLib.h>
#include <Library/BaseLib.h>
#include <Protocol/MpService.h>
#include <Library/CpuConfigLib.h>
#include <Library/S3BootScriptLib.h>
#include <Cpu/CpuRegs.h>

#define PPM_MAJOR           0
#define PPM_MINOR           1
#define PPM_REV             0
#define PPM_BUILD           0

// Note: There 3 separate DEF blocks: CSR related, MSR related and MISC
//       Please add your DEFs in proper block

//---------------------------------------------
//             defs related to CSR
//---------------------------------------------
//CSR_C2C3TT    bit definition
#define PPDN_INIT_MASK                    0xFFF   // Bit[11:0])

// IMC related info
//CSR_DIMMMTR bit definition
#define  POP_CH_EN                        (1 << 14)  // Bit 14
//CSR_PM_SREF bit definition
#define CHK_SREF_EN                       (1 << 20)    

// QPI info
#define CHK_LINK_INIT_STATUS_SHIFT        24        // Bits 27:24
#define CHK_QPI_NORM_OP                   (0x06 << CHK_LINK_INIT_STATUS_SHIFT)  // value for normal operation
#define QPI_LNK_STATUS_MASK               0x0f000000
#define CHK_L1_ENABLE                     (1 << 20)  // Bit 20
#define CHK_L0S_ENABLE                    (1 << 18)  // Bit 18

// BIOS_RESET_CPL bit description
#define BIOS_RSTCPL0_DONE                  BIT0
#define BIOS_RSTCPL1_MCALDONE              BIT1
#define BIOS_RSTCPL2_RCOMPDONE             BIT2
#define BIOS_RSTCPL3_PMCFGDONE             BIT3
#define BIOS_RSTCPL4_MEMCFGDONE            BIT4

//---------------------------------------------
//!!!!!!!!!!! MSR and defs related to MSR
//---------------------------------------------
//TODO - work with CORE team to see if we can consolidate MSR related DEFs
// Bit definitions for MSR_PLATFORM_INFO (ECX = 0CEh)
#define B_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET  (1 << 30) //bit 30

// MSR TURBO_RATIO_LIMIT 0x1AD/E/F EFINITIONS
//#define MSR_TURBO_RATIO_LIMIT                   0x1AD
#define MSR_TURBO_RATIO_LIMIT1       0x1AE
#define MSR_TURBO_RATIO_LIMIT2       0x1AF
#define MAX_OVERCLOCKING_BINS        0x7
#define MAX_RATIO_LIMIT_8C_MASK     (0xFF << 24)
#define MAX_RATIO_LIMIT_7C_MASK     (0xFF << 16)
#define MAX_RATIO_LIMIT_6C_MASK     (0xFF << 8)
#define MAX_RATIO_LIMIT_5C_MASK     (0xFF)
#define MAX_RATIO_LIMIT_4C_MASK     (0xFF << 24)
#define MAX_RATIO_LIMIT_3C_MASK     (0xFF << 16)
#define MAX_RATIO_LIMIT_2C_MASK     (0xFF << 8)
#define MAX_RATIO_LIMIT_1C_MASK     (0xFF)
#define MAX_EXTRA_VOLTAGE           0xFF
#define PROG_TDP_LIMITS             (1 << 29)
#define PROG_RATIO_LIMITS           (1 << 28)
#define CORE_COUNT_OFFSET           (16)
#define THREAD_COUNT_MASK           (0xFFFF) //MSR_CORE_THREAD_COUNTbit15:0 - threadcount
#define OVERCLOCKING_BINS           (0x7 << 17)
#define BIOS_PCU_SEMAPHORE          (1 << 31) // Bit 63

#define EFI_MSR_TURBO_RATIO_LIMIT_RATIOS MSR_TURBO_RATIO_LIMIT
#define EFI_MSR_TURBO_RATIO_LIMIT_CORES  MSR_TURBO_RATIO_LIMIT1

// MSR_ENERGY_PERFORMANCE_BIAS 0x1B0
#define MSR_ENERGY_PERFORMANCE_BIAS                0x1B0
#define ALTENERGYPERFIAS_DEFAULT                   0x7

#define MSR_PRIMARY_PLANE_CURRENT_CONFIG_CONTROL   0x601
#define MSR_VR_MISC_CONFIG                         0x603
#define  IDLE_ENTRY_DECAY_ENABLE_SHIFT             20
#define  B_IDLE_ENTRY_DECAY_ENABLE                 (1 << IDLE_ENTRY_DECAY_ENABLE_SHIFT) // Bit52

#define MSR_PACKAGE_POWER_SKU_UNIT                 0x606
#define PACKAGE_POWER_UNIT_MASK                    7
#define TCC_OFFSET_PROGRAMABLE_MASK                (1 << 30)
#define TCC_ACTIVATION_OFFSET_MASK                 (0xF << 24)
#define PACKAGE_TIME_UNIT_MASK                      0xF0000

#define MSR_ENERGY_PERF_BIAS_CONFIG                 0xA01

#define MSR_C_STATE_LATENCY_CONTROL_0               0x60a
#define MSR_C_STATE_LATENCY_CONTROL_1               0x60b
#define MSR_C_STATE_LATENCY_CONTROL_2               0x60c

// PCU_CR_CAPID4_CFG bit Description (FUSE value)
#define B_OC_ENABLED                                (1 << 28) // Bit 28

#define MSR_PRIMARY_PLANE_CURRENT_CONFIG_CONTROL     0x601


#define PPCCC_LOCK_SHIFT                             31
#define PPCCC_LOCK                                   (1 << PPCCC_LOCK_SHIFT)

#define OC_LOCK                                      (BIT20)
#define OC_LOCK_SHIFT                                20
#define OC_SUPPORTED                                 (BIT19 | BIT18 | BIT17)

//---------------------------------------------
//                 MISC defs
//---------------------------------------------
#define END_OF_TABLE                                 0xFF

//PCPS: MSR_MISC_PWR_MGMT
//#define MSR_MISC_PWR_MGMT                          0x1aa
#define   B_SINGLE_PCTL_EN                           (1 << 0)  //Bit 0

//PCU_MISC_CONFIG
#define PCU_MISC_CONFIG_SPD_ENABLE                   (1 << 30)
#define PCU_MISC_CONFIG_APS_ROCKETING                (1 << 8)
#define PCU_MISC_CONFIG_SCALABILITY                  (1 << 9)
#define PCU_MISC_CONFIG_PPO_BUDGET                   (1 << 10)
#define PCU_MISC_CONFIG_DYNAMIC_L1                   (1 << 22)

//
#define PCU_OUT_OF_BAND_ALTERNATE_EPB                (1 << 19)

//PPM_BIOS_MAILBOX  bit/command definition
#define RUN_BUSY                                     (1 << 31)              // Bit 31
#define PPM_MAILBOX_BIOS_CMD_READ_CPU_VR_INFO        0x93  // Read CPU VR Info
#define PPM_MAILBOX_BIOS_DATA_CORE_VR_ADDR           0x00  //  VR Address for CPU core VR
#define PPM_MAILBOX_BIOS_DATA_VR_ICC_MAX_ADR         0x21  //  VR ICC MAX ADR
#define PPM_MAILBOX_BIOS_ERROR_CODE_INVALID_COMMAND  0x01
#define MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG        0x05  // New in HSX. Returns the value from the MISC_CONFIG memory variable.
#define MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG       0x06  // New in HSX. HSX Changes: Adding chicken bits for bios to program
#define MAILBOX_BIOS_CMD_OOB_INIT_EPP                0xB9  // New in SKX

#define PL2_SAFETY_NET_ENABLE                       (1 << 1)
#define PKGC_RCOMP_DISABLE                          (1 << 23)

//
// Integer division with rounding to the nearest rather than truncating.
// For example 8/3=2 but EFI_IDIV_ROUND(8,3)=3. 1/3=0 and EFI_IDIV_ROUND(1,3)=0.
// A half is rounded up e.g., EFI_IDIV_ROUND(1,2)=1 but 1/2=0.
//
#define EFI_IDIV_ROUND(r, s)  ((r) / (s) + (((2 * ((r) % (s))) < (s)) ? 0 : 1))


extern const UINT16             mSecondsToMsrValueMapTable[][2];

// SKX A0 WA only: set P1 for core and mesh as 700MHz
#define UNCORE_MESH_RATIO_HARDCODE_PN             ((7 << 8) | 0x3f);
#define UNCORE_MESH_RATIO_HARDCODE_P1             ((40 << 8) | 0x3f);

// SKX A0,A1 WA only; Enable 2LM memory (status) to pcode to make sure uclk >= dclk
#define MC_ID0 0
#define MC_ID1 1
#endif







