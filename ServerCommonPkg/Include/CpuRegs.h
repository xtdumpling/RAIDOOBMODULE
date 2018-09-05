//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license 
// agreement with Intel or your vendor.  This file may be      
// modified by the user, subject to additional terms of the    
// license agreement                                           
//
/*++

Copyright (c) 2005 - 2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  CpuRegs.h

Abstract:

--*/

#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_

#include <CommonIncludes.h>

//
// Local APIC defines
//
#define APIC_REGISTER_LOCAL_ID_OFFSET         0x00000020
#define APIC_REGISTER_APIC_VERSION_OFFSET     0x00000030
#define APIC_REGISTER_SPURIOUS_VECTOR_OFFSET  0x000000F0
#define APIC_REGISTER_ICR_LOW_OFFSET          0x00000300
#define APIC_REGISTER_ICR_HIGH_OFFSET         0x00000310
#define APIC_REGISTER_LINT0_VECTOR_OFFSET     0x00000350
#define APIC_REGISTER_LINT1_VECTOR_OFFSET     0x00000360

#define BROADCAST_MODE_SPECIFY_CPU            0x00
#define BROADCAST_MODE_ALL_INCLUDING_SELF     0x01
#define BROADCAST_MODE_ALL_EXCLUDING_SELF     0x02

#ifndef DELIVERY_MODE_REMOTE_READ

//#define DELIVERY_MODE_FIXED                   0x00
//#define DELIVERY_MODE_LOWEST_PRIORITY         0x01
//#define DELIVERY_MODE_SMI                     0x02
//#define DELIVERY_MODE_REMOTE_READ             0x03
//#define DELIVERY_MODE_NMI                     0x04
#define DELIVERY_MODE_INIT                    0x05
#define DELIVERY_MODE_SIPI                    0x06
//#define DELIVERY_MODE_MAX                     0x07
#endif

#define TRIGGER_MODE_EDGE                     0x00
#define TRIGGER_MODE_LEVEL                    0x01


#define EFI_CACHE_UNCACHEABLE                 0
#define EFI_CACHE_WRITECOMBINING              1
#define EFI_CACHE_WRITETHROUGH                4
#define EFI_CACHE_WRITEPROTECTED              5
#define EFI_CACHE_WRITEBACK                   6

#define EFI_CPUID_SIGNATURE                   0x0
#define EFI_CPUID_VERSION_INFO                0x1
#define   B_EFI_CPUID_VERSION_INFO_ECX_MWAIT    BIT3
#define   B_EFI_CPUID_VERSION_INFO_ECX_VME      BIT5
#define   B_EFI_CPUID_VERSION_INFO_ECX_SME      BIT6
#define   B_EFI_CPUID_VERSION_INFO_ECX_EIST     BIT7
#define   B_EFI_CPUID_VERSION_INFO_ECX_TM2      BIT8
#define   B_EFI_CPUID_VERSION_INFO_ECX_DCA      BIT18
#define   B_EFI_CPUID_VERSION_INFO_ECX_XAPIC    BIT21
#define   B_EFI_CPUID_VERSION_INFO_ECX_AES      BIT25
#define   B_EFI_CPUID_VERSION_INFO_EDX_XD       BIT20
#define   B_EFI_CPUID_VERSION_INFO_EDX_HT       BIT28
#define EFI_CPUID_CACHE_INFO                  0x2
#define EFI_CPUID_SERIAL_NUMBER               0x3
#define EFI_CPUID_CACHE_PARAMS                0x4
#define EFI_CPUID_MONITOR_MWAIT_PARAMS        0x5
#define EFI_CPUID_POWER_MANAGEMENT_PARAMS     0x6
#define   EFI_CPUID_POWER_MANAGEMENT_EAX_TURBO   BIT1
#define EFI_CPUID_REV7                        0x7
#define EFI_CPUID_REV8                        0x8
#define EFI_CPUID_DCA_PARAMS                  0x9
#define EFI_CPUID_REVA                        0xA
#define EFI_CPUID_CORE_TOPOLOGY               0xB

#define EFI_CPUID_EXTENDED_FUNCTION           0x80000000
#define EFI_CPUID_EXTENDED_CPU_SIG            0x80000001
#define EFI_CPUID_BRAND_STRING1               0x80000002
#define EFI_CPUID_BRAND_STRING2               0x80000003
#define EFI_CPUID_BRAND_STRING3               0x80000004
#define EFI_CPUID_L2_CACHE_FEATURE            0x80000006
#define EFI_CPUID_VIR_PHY_ADDRESS_SIZE        0x80000008

#define EFI_MSR_IA32_PLATFORM_ID              0x00000017
#define EFI_MSR_IA32_APIC_BASE                0x0000001B
#define   B_EFI_MSR_IA32_APIC_BASE_G_XAPIC      BIT11
#define   B_EFI_MSR_IA32_APIC_BASE_M_XAPIC      BIT10
#define   B_EFI_MSR_IA32_APIC_BASE_BSP          BIT8
#define EFI_PIC_THREAD_CONTROL                0x0000002E
#define   B_EFI_PIC_THREAD_CONTROL_TPR_DIS      BIT10
#define EFI_MSR_SOCKET_ID                     0x00000039
#define EFI_MSR_IA32_FEATURE_CONTROL          0x0000003A
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_LOCK   BIT0
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_ELT    BIT1
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_EVT    BIT2
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_D3ST    BIT11
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_SLFE   (BIT8 | BIT9 | BIT10 | BIT11 | BIT12 | BIT13 | BIT14)
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_SGE    BIT15
#define EFI_MSR_IA32_BIOS_UPDT_TRIG           0x00000079
#define EFI_MSR_IA32_BIOS_SIGN_ID             0x0000008B
#define EFI_PLATFORM_INFORMATION              0x000000CE
#define   N_EFI_PLATFORM_INFO_MIN_RATIO         40
#define   B_EFI_PLATFORM_INFO_RATIO_MASK        0xFF
#define   N_EFI_PLATFORM_INFO_MAX_RATIO         8
#define   B_EFI_PLATFORM_INFO_TDC_TDP_LIMIT     BIT29 
#define   B_EFI_PLATFORM_INFO_RATIO_LIMIT       BIT28
#define   B_EFI_PLATFORM_INFO_DCUMODE_SUPPORT   BIT26
#define EFI_IA32_PMG_CST_CONFIG               0x000000E2
#define   B_EFI_C1_AUTO_DEMOTION_ENABLE         BIT26
#define   B_EFI_C3_AUTO_DEMOTION_ENABLE         BIT25
#define   B_EFI_CST_CONTROL_LOCK                BIT15
#define   B_EFI_IO_MWAIT_REDIRECTION_ENABLE     BIT10
#define   B_EFI_PACKAGE_C_STATE_LIMIT           BIT2+BIT1+BIT0
#define EFI_IA32_PMG_IO_BASE_ADDR             0x000000E3
#define EFI_IA32_PMG_IO_CAPTURE_ADDR          0x000000E4
#define   B_EFI_IA32_PMG_CST_RANGE              BIT18+BIT17+BIT16
#define EFI_IA32_MTRR_CAP                     0x000000FE
#define   B_EFI_IA32_MTRR_VARIABLE_SUPPORT      0xFF
#define   B_EFI_IA32_MTRR_CAP_FIXED_SUPPORT     BIT8
#define   B_EFI_IA32_MTRR_CAP_SMRR_SUPPORT      BIT11
#define   B_EFI_IA32_MTRR_CAP_EMRR_SUPPORT      BIT12
#define EFI_MSR_IA32_FEATURE_CONFIG          0x0000013C
#define   B_EFI_IA32_AESNI_DISABLE              BIT1
#define   B_EFI_IA32_AESNI_LOCK                 BIT0
#define	EFI_MSR_VPE			                  0x00000178
#define	  B_POISON_FORWARD			            BIT1
#define   B_VIRAL_BIT				            BIT2
#define EFI_IA32_MCG_CAP                      0x00000179
#define EFI_IA32_MCG_STATUS                   0x0000017A
#define EFI_IA32_CLOCK_FLEX_MAX               0x00000194
#define   N_EFI_FLEX_RATIO                      8
#define EFI_IA32_PERF_STS                     0x00000198
#define EFI_IA32_PERF_CTRL                    0x00000199
#define   B_EFI_IA32_PERF_CTRL_TURBO_DIS        BIT32
#define EFI_MSR_IA32_CLOCK_MODULATION         0x19A
#define   B_EFI_CLOCK_MODULATION_ENABLE         BIT4
#define EFI_IA32_THERM_INTERRUPT              0x0000019B
#define   B_EFI_IA32_THERM_INTERRUPT_VIE        BIT4
#define EFI_MSR_IA32_MISC_ENABLE              0x000001A0
#define   B_EFI_MSR_IA32_MISC_ENABLE_FSE        BIT0
#define   B_EFI_MSR_IA32_MISC_ENABLE_TME        BIT3
#define   B_EFI_MSR_IA32_MISC_ENABLE_EIST       BIT16
#define   B_EFI_MSR_IA32_MISC_ENABLE_MONITOR    BIT18
#define   B_EFI_MSR_IA32_MISC_ENABLE_CPUID_MAX  BIT22
#define   B_EFI_MSR_IA32_MISC_ENABLE_TPR_DIS    BIT23
#define   B_EFI_MSR_IA32_MISC_ENABLE_XD         BIT34
#define   B_EFI_MSR_IA32_MISC_DISABLE_TURBO     BIT38
#define EFI_MISC_FEATURE_CONTROL              0x000001A4
#define   B_EFI_MISC_FEATURE_CONTROL_MLC_STRP   BIT0
#define   B_EFI_MISC_FEATURE_CONTROL_MLC_SPAP   BIT1
#define   B_EFI_MISC_FEATURE_CONTROL_DCU_STRP   BIT2
#define   B_EFI_MISC_FEATURE_CONTROL_DCU_IPP    BIT3
#define   B_EFI_MISC_FEATURE_CONTROL_3STRIKE_DIS  BIT11
#define EFI_MSR_MISC_PWR_MGMT                 0x000001AA
#define   B_EFI_MSR_MISC_PWR_MGMT_EIST_HW       BIT0
#define   B_EFI_MSR_MISC_PWR_MGMT_PERF_BIAS_ENABLE BIT1
#define   B_EFI_MSR_MISC_PWR_MGMT_PECI_TD       BIT11   
#define   B_EFI_MSR_MISC_PWR_MGMT_LTMI          BIT22
#define EFI_MSR_TURBO_RATIO_LIMIT             0x000001AD
#define   N_EFI_MSR_TURBO_RATIO_LIMIT_1C        0
#define   B_EFI_MSR_TURBO_RATIO_LIMIT_1C        (0xFF<<0)
#define   N_EFI_MSR_TURBO_RATIO_LIMIT_2C        8
#define   B_EFI_MSR_TURBO_RATIO_LIMIT_2C        (0xFF<<8)
#define   N_EFI_MSR_TURBO_RATIO_LIMIT_3C        16
#define   B_EFI_MSR_TURBO_RATIO_LIMIT_3C        (0xFF<<16)
#define   N_EFI_MSR_TURBO_RATIO_LIMIT_4C        24
#define   B_EFI_MSR_TURBO_RATIO_LIMIT_4C        (0xFF<<24)
#define EFI_MSR_ENERGY_PERF_BIAS              0x000001B0
#define EFI_MSR_IA32_PLATFORM_DCA_CAP         0x000001F8
#define   B_EFI_MSR_IA32_PLATFORM_DCA_CAP_TYPE0_EN  BIT0
#define EFI_MSR_IA32_CPU_DCA_CAP              0x000001F9
#define   B_EFI_MSR_IA32_CPU_DCA_CAP_TYPE0_SUP  BIT0
#define EFI_MSR_IA32_DCA_0_CAP                0x000001FA
#define   B_EFI_MSR_IA32_CPU_DCA_CAP_ENDID      BIT11
#define   N_EFI_MSR_IA32_CPU_DCA_CAP_DELAY      13 
#define   B_EFI_MSR_IA32_CPU_DCA_CAP_DELAY      (BIT13 | BIT14 | BIT15 | BIT16) 
#define   B_EFI_MSR_IA32_CPU_DCA_CAP_SW_LOCK     BIT24
#define   B_EFI_MSR_IA32_CPU_DCA_CAP_SW_FLUSH    BIT25
#define   B_EFI_MSR_IA32_CPU_DCA_CAP_HW_LOCK     BIT26
#define EFI_MSR_POWER_CTL                     0x000001FC
#define   B_EFI_MSR_POWER_CTL_BROCHOT           BIT0
#define   B_EFI_MSR_POWER_CTL_C1E               BIT1
#define EFI_MSR_FERR_CAPABILITIES             0x000001F1
#define   B_MSR_EFI_FERR_ENABLE                 BIT0

#define EFI_SMRR_PHYS_BASE                    0x000001F2
#define EFI_SMRR_PHYS_MASK                    0x000001F3
#define EFI_EMRR_PHYS_BASE                    0x000001F4
#define EFI_EMRR_PHYS_MASK                    0x000001F5

#define EFI_CACHE_VARIABLE_MTRR_BASE          0x00000200
#define EFI_CACHE_VARIABLE_MTRR_END           0x0000020F
#define   V_EFI_FIXED_MTRR_NUMBER                     11
#define EFI_IA32_MTRR_FIX64K_00000            0x00000250
#define EFI_IA32_MTRR_FIX16K_80000            0x00000258
#define EFI_IA32_MTRR_FIX16K_A0000            0x00000259
#define EFI_IA32_MTRR_FIX4K_C0000             0x00000268
#define EFI_IA32_MTRR_FIX4K_C8000             0x00000269
#define EFI_IA32_MTRR_FIX4K_D0000             0x0000026A
#define EFI_IA32_MTRR_FIX4K_D8000             0x0000026B
#define EFI_IA32_MTRR_FIX4K_E0000             0x0000026C
#define EFI_IA32_MTRR_FIX4K_E8000             0x0000026D
#define EFI_IA32_MTRR_FIX4K_F0000             0x0000026E
#define EFI_IA32_MTRR_FIX4K_F8000             0x0000026F
#define EFI_CACHE_IA32_MTRR_DEF_TYPE          0x000002FF
#define   B_EFI_CACHE_MTRR_VALID                BIT11
#define   B_EFI_CACHE_FIXED_MTRR_VALID          BIT10

#define EFI_NO_EVICT_MODE                     0x000002E0
#define   B_EFI_NO_EVICT_MODE_SETUP             BIT0
#define   B_EFI_NO_EVICT_MODE_RUN               BIT1

#define EFI_PCIEXBAR                          0x00000300
#define   B_EFI_PCIEXBAR_EN                     BIT0
#define   B_EFI_PCIEXBAR_SIZE                   (BIT1 | BIT2 | BIT3)
#define   N_EFI_PCIEXBAR_SIZE                   1

#define EFI_IA32_MC0_CTL                      0x00000400
#define EFI_IA32_MC0_STATUS                   0x00000401
#define EFI_IA32_MC0_ADDR                     0x00000402
#define EFI_IA32_MC0_MISC                     0x00000403
#define EFI_IA32_MC8_CTL                      (EFI_IA32_MC0_CTL + (8*4))
#define EFI_IA32_MC8_STATUS                   (EFI_IA32_MC0_STATUS + (8*4))

#define MSR_PACKAGE_POWER_SKU_UNIT            0x606
#define MSR_TURBO_POWER_LIMIT									0x610

#define EFI_APIC_GLOBAL_ENABLE                0x00000800
#define EFI_EXT_XAPIC_LOGICAL_APIC_ID         0x00000802
#define EFI_EXT_XAPIC_VERSION                 0x00000803
#define EFI_EXT_XAPIC_SVR                     0x0000080F
#define EFI_EXT_XAPIC_ICR                     0x00000830
#define EFI_EXT_XAPIC_LVT_LINT0               0x00000835
#define EFI_EXT_XAPIC_LVT_LINT1               0x00000836


#endif
