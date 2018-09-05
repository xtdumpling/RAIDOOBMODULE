//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/**

   Intel Restricted Secret

   CPU Reference Code

@copyright
   Copyright (c) 2007-2016, Intel Corporation.

   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.

   This program has been developed by Intel Corporation.
   Licensee has Intel's permission to incorporate this source code
   into their product, royalty free.  This source code may NOT be
   redistributed to anyone without Intel's written permission.

   Intel specifically disclaims all warranties, express or
   implied, and all liability, including consequential and other
   indirect damages, for the use of this code, including liability
   for infringement of any proprietary rights, and including the
   warranties of merchantability and fitness for a particular
   purpose.  Intel does not assume any responsibility for any
   errors which may appear in this code nor any responsibility to
   update it.

@file
  CpuHost.h

@brief
  This file contains generic definitions.

**/

#ifndef cpuhost_h
#define cpuhost_h
#include "PlatformHost.h"
#include "DataTypes.h"
#include "SysHostChipCommon.h"
#ifndef ASM_INC
#endif

#define inline          __inline

//
//  CPU ExtFamily/Family/Model  bit[19:4] of cpuid(1)_eax
//
#ifndef CPU_FAMILY_HSX
#define CPU_FAMILY_HSX     0x306F      // Haswell CPU
#endif
#ifndef CPU_FAMILY_SKX
#define CPU_FAMILY_SKX     0x5065      // Skylake  CPU
#endif
#ifndef CPU_FAMILY_ICX
#define CPU_FAMILY_ICX     0x606a      // IceLake  CPU
#endif
#ifndef CPU_FAMILY_KNH
#define CPU_FAMILY_KNH     0x706F      // KnightsHill  CPU
#endif


//typedef INT32  CPU_STATUS;    // this causes MiniBIOS build failure
typedef int  CPU_STATUS;
#define CPU_SUCCESS 0
#define CPU_FAILURE -1


#ifndef ASM_INC

/**

    @struct cpuSetup
    @brief CPU init related Setup options

**/
struct cpuSetup {                      // CPU init related Setup options
  UINT8   PchTraceHubEn;                ///< 1: Enable NPK_STH_ACPIBAR_BASE (MSR 0x80)
  UINT8   debugInterfaceEn;             ///< 1: Enable Debug Inerface for DFX
  UINT8   dcuModeSelect;                ///< 0: 32KB 8-way (hardware default). Non-zero: 16KB 4-way with ECC (CPU MSR 031h)
  UINT8   EnableGv;                     ///< GV3 Enable
  UINT8   flexRatioEn;                  ///< FLEX_RATIO Override Enable
  UINT8   flexRatioNext;                ///< FLEX_RATIO, common for all CPU sockets
                                        ///<  0=Don't change flex ratio (default) 0xff = Max Non-turbo ratio. Other values defines target flex ratio
  UINT8   ConfigTDP;                    ///< 0/1 Disable/Enable Config TDP. common for all CPU sockets
  UINT8   ConfigTDPLevel;               ///< 0 - 2: 0 = Normal; 1 =  Level 1; 2 = Level 2
  UINT32  CoreDisableMask[MAX_SOCKET];  ///< CoreOffMask value for each CPU socket
  UINT64  TileDisableMask[MAX_SOCKET];  ///< TileOffMask value for each CPU socket
  UINT8   TileCountOverride[MAX_SOCKET];///< 0/1 Disable/Enable Desired Tile Count Override
  UINT8   DesiredTileCount[MAX_SOCKET]; ///< Count of desired tiles if TileCountOverride=1 (enabled)
  UINT8   smtEnable;                    ///< 0/1 Disable/Enable SMT(HT). common for all CPU sockets
  UINT8   vtEnable;                     ///< 0/1 Disable/Enable VMX. Common for all CPU sockets
  UINT8   ltEnable;
  UINT8   ltsxEnable;
  UINT8   PpinControl;                  ///< 0/1 Disable/Enable
  UINT8   IotEn[MAX_SOCKET];            ///< IOT/OCLA Config Disable/Enable,
  UINT8   OclaTorEntry[MAX_SOCKET];     ///< IOT/OCLA MaxTorEntry
  UINT8   OclaWay[MAX_SOCKET];          ///< IOT/OCLA LLC Ways
  UINT8   UFSDisable;                   ///< 0/1 Enable/Disable UFS. Mailbox Command to PCU_MISC_CONFIG Bit[28]
  UINT8   vrsOff;
  UINT8   AllowMixedPowerOnCpuRatio;  ///< Keep CPU ratios at power-on default without forcing common ratio among CPU socekts
  UINT8   CheckCpuBist;                 ///< Check BIST result and disable failed cores when enabled. Otherwise, ignore BIST result
};

/**

  @struct CpuBistStruct
  @brief CPU BIST information

**/
struct CpuBistStruct {                  // CPU BIST information
  UINT8   valid;                        ///< non-zero if the data in this struct is valid
  UINT8   apicId;                       ///< APIC ID
  UINT32  result;                       ///< BIST result value
};

/**

  @struct cpuVar
  @brief CPU information

**/
struct cpuVar {
  UINT32 packageBspApicID[MAX_SOCKET];   ///< Package BSP APIC IDs (per socket)
  UINT8  packageBspStepping[MAX_SOCKET]; ///< Package BSP Setpping (per socket).
  UINT8  TotHa[MAX_SOCKET];              ///< Total HAs; All CPUs must have same number of HA
                                         ///< Each array element stores the initial APIC ID of the corresponding
                                         ///< Socket/Node BSP that is present (as indicated by the socketPresentBitMap)
                                         ///<

  struct CpuBistStruct CpuBist[MAX_SOCKET*MAX_CORE];   ///< one for each core indexed by CoreID(0,1,2...).
                                                       ///< A non-zero value indicates BIST failure for that core.
                                                       ///<
};

/**

  @struct cpuNvram
  @brief Data that need to be saved in NVRAM for S3 resume

**/
struct cpuNvram {                       // data that need to be saved in NVRAM for S3 resume
  UINT32 flexRatioCsr;                   ///<  Common for all sockets
  UINT32 desiredCoresCsr[MAX_SOCKET];    ///<  One per socket
  UINT32 desiredCoresCfg2Csr[MAX_SOCKET];///<  One per socket
};
#pragma pack()
#endif  // ASM_INC

//
// Reset types needed post execution
//
#define POST_RESET_NO_RESET       0x0
#define POST_RESET_WARM           0x2    // bit1
#define POST_RESET_POWERGOOD      0x4    // bit2
#define POST_RESET_AMI            0x8    // bit3

//
// Max reservable TOR entries defines
//
#define MAX_TOR_ENTRIES_ISOC   15
#define MAX_TOR_ENTRIES_NORMAL 17

//
// Error Code used for LogError()
//
#define ERROR_CPU_BIST     0xC0
  #define ERROR_CPU_BIST_MINOR_SOME_SOCKET           0x01
  #define ERROR_CPU_BIST_MINOR_SOME_BISTRESULTMASK   0x02
  #define ERROR_CPU_BIST_MINOR_ALL    0x03

//
// Error Codes used for LogError() and LogWarning()
//
#define WARN_CPU_BIST                           0xC0
#define WARN_CPU_BIST_MINOR_LOWER_TILE_RANGE    0x01
#define WARN_CPU_BIST_MINOR_MIDDLE_TILE_RANGE   0x02
#define WARN_CPU_BIST_MINOR_UPPER_TILE_RANGE    0x03
#define WARN_CPU_BIST_MINOR_ALL                 0x04


//
// MSR definitions
//
#ifndef MSR_IA32_PLATFORM_ID
#define MSR_IA32_PLATFORM_ID          0x0017
#endif
#ifndef MSR_APIC_BASE
#define MSR_APIC_BASE                 0x001B
#endif
#ifndef MSR_EBC_FREQUENCY_ID
#define MSR_EBC_FREQUENCY_ID          0x002C
#endif
#ifndef MSR_CORE_THREAD_COUNT
#define MSR_CORE_THREAD_COUNT         0x0035
#endif
#ifndef MSR_SOCKET_ID
#define MSR_SOCKET_ID                 0x0039
#endif
#ifndef MSR_IA32_FEATURE_CONTROL
#define MSR_IA32_FEATURE_CONTROL      0x003A
#endif
#ifndef VIRTUAL_MSR_MCA_ON_NON_NEW_CACHABLE_MMIO_EN_ADDR
#define VIRTUAL_MSR_MCA_ON_NON_NEW_CACHABLE_MMIO_EN_ADDR  0x61
#endif
#ifndef MCAONNONNEMCACHEABLEMMIO_BIT
#define MCAONNONNEMCACHEABLEMMIO_BIT  0x1
#endif
#ifndef MSR_IA32_BIOS_UPDT_TRIG
#define MSR_IA32_BIOS_UPDT_TRIG       0x0079
#endif
#ifndef MSR_TRACE_HUB_STH_ACPIBAR_BASE
#define MSR_TRACE_HUB_STH_ACPIBAR_BASE                                0x00000080
#define B_MSR_TRACE_HUB_STH_ACPIBAR_BASE_LOCK                         BIT0
#define V_MSR_TRACE_HUB_STH_ACPIBAR_BASE_MASK                         0x0003FFFF
#endif
#ifndef PCH_TRACE_HUB_FW_BASE_ADDRESS
#define PCH_TRACE_HUB_FW_BASE_ADDRESS                                 0xFE0C0000     ///< TraceHub FW MMIO base address
#endif
#ifndef MSR_IA32_BIOS_SIGN_ID
#define MSR_IA32_BIOS_SIGN_ID         0x008B
#endif
#ifndef MSR_PLATFORM_INFO
#define MSR_PLATFORM_INFO             0x00CE
#endif
#ifndef MSR_PMG_CST_CONFIG_CONTROL
#define MSR_PMG_CST_CONFIG_CONTROL    0x00E2
#endif
#ifndef MSR_PMG_IO_CAPTURE_BASE
#define MSR_PMG_IO_CAPTURE_BASE       0x0E4
#endif
#ifndef MSR_MCG_CONTAIN
#define MSR_MCG_CONTAIN               0x178
#define B_MSR_MCG_CONTAIN_PE          BIT0
#endif
#ifndef MSR_IA32_MCG_CAP
#define MSR_IA32_MCG_CAP              0x179
#define B_MSR_MCG_CAP_GCM             BIT24
#endif
#ifndef MSR_CLOCK_FLEX_MAX
#define MSR_CLOCK_FLEX_MAX            0x0194
#endif
#ifndef MSR_IA32_PERF_STS
#define MSR_IA32_PERF_STS             0x0198
#endif
#ifndef MSR_IA32_PERF_CTL
#define MSR_IA32_PERF_CTL             0x0199
#endif
#ifndef MSR_IA32_MISC_ENABLES
#define MSR_IA32_MISC_ENABLES         0x01A0
#endif
#ifndef IA32_MISC_ENABLE
#define IA32_MISC_ENABLE              0x01A0
#endif
#ifndef FAST_STRING_ENABLE_BIT
#define FAST_STRING_ENABLE_BIT        0x1
#endif
#ifndef MSR_MISC_PWR_MGMT
#define MSR_MISC_PWR_MGMT             0x01AA
#endif
#ifndef MSR_TURBO_POWER_CURRENT_LIMIT
#define MSR_TURBO_POWER_CURRENT_LIMIT 0x1AC
#endif
#ifndef MSR_TURBO_RATIO_LIMIT
#define MSR_TURBO_RATIO_LIMIT         0x01AD
#endif
#ifndef MSR_DFX_ENABLES
#define MSR_DFX_ENABLES               0x01E2
#endif
#ifndef MSR_POWER_CTRL
#define MSR_POWER_CTRL                0x01FC
#endif
#ifndef MSR_NO_EVICT_MODE
#define MSR_NO_EVICT_MODE             0x02E0
#endif
#ifndef MSR_IA32_MC7_CTL
#define MSR_IA32_MC7_CTL              0x041C
#endif
#ifndef MSR_IA32_MC8_MISC2
#define MSR_IA32_MC8_MISC2            0x0288
#endif
#ifndef MSR_PCIEXBAR
#define MSR_PCIEXBAR                  0x0300
#endif
#ifndef MSR_PPIN_CTL
#define MSR_PPIN_CTL                  0x004E
#endif
#ifndef MSR_PPIN
#define MSR_PPIN                      0x004F
#endif
#ifndef MSR_MC_CTL
#define MSR_MC_CTL                    0x0434
#endif
#define MSR_UNCORE_FREQ               0x0620

#ifndef MTRR_PHYS_BASE_0
#define MTRR_PHYS_BASE_0      0x0200
#define MTRR_PHYS_MASK_0      0x0201
#define MTRR_PHYS_BASE_1      0x0202
#define MTRR_PHYS_MASK_1      0x0203
#define MTRR_PHYS_BASE_2      0x0204
#define MTRR_PHYS_MASK_2      0x0205
#define MTRR_PHYS_BASE_3      0x0206
#define MTRR_PHYS_MASK_3      0x0207
#define MTRR_PHYS_BASE_4      0x0208
#define MTRR_PHYS_MASK_4      0x0209
#define MTRR_PHYS_BASE_5      0x020A
#define MTRR_PHYS_MASK_5      0x020B
#define MTRR_PHYS_BASE_6      0x020C
#define MTRR_PHYS_MASK_6      0x020D
#define MTRR_PHYS_BASE_7      0x020E
#define MTRR_PHYS_MASK_7      0x020F
#define MTRR_FIX_64K_00000    0x0250
#define MTRR_FIX_16K_80000    0x0258
#define MTRR_FIX_16K_A0000    0x0259
#define MTRR_FIX_4K_C0000     0x0268
#define MTRR_FIX_4K_C8000     0x0269
#define MTRR_FIX_4K_D0000     0x026A
#define MTRR_FIX_4K_D8000     0x026B
#define MTRR_FIX_4K_E0000     0x026C
#define MTRR_FIX_4K_E8000     0x026D
#define MTRR_FIX_4K_F0000     0x026E
#define MTRR_FIX_4K_F8000     0x026F
#define MTRR_DEF_TYPE         0x02FF

#define MTRR_MEMORY_TYPE_UC   0x00
#define MTRR_MEMORY_TYPE_WC   0x01
#define MTRR_MEMORY_TYPE_WT   0x04
#define MTRR_MEMORY_TYPE_WP   0x05
#define MTRR_MEMORY_TYPE_WB   0x06

#define MTRR_DEF_TYPE_E       0x0800
#define MTRR_DEF_TYPE_FE      0x0400
#define MTRR_PHYS_MASK_VALID  0x0800
#endif // MTRR_PHYS_BASE_0

//
// Memory-mapped APIC Offsets
//
#define APIC_LOCAL_APIC_ID    0x020
#define APIC_ICR_LO           0x300
#define APIC_ICR_HI           0x310
#define APIC_TMR_INITIAL_CNT  0x380
#define APIC_TMR_CURRENT_CNT  0x390

//
// APIC Timer runs at 133MHz and by default decrements
// the current count register at once per two clocks.
// t = time in milliseconds
// c = APIC Timer Initial Value
// c = (t * 10^(-6) sec) * (133 * 10^6 count/sec) * (1/2 clocks)
// Notice seconds and exponents cancel out leaving count value
// c = (t * 133 / 2)
//
#define APIC_TMR_1US    (1 * 133 / 2)
#define APIC_TMR_10US   (10 * 133 / 2)
#define APIC_TMR_20US   (20 * 133 / 2)
#define APIC_TMR_100US  (100 * 133 / 2)
#define APIC_TMR_200US  (200 * 133 / 2)
#define APIC_TMR_10MS   (10 * 1000 * 133 / 2)


#ifndef ASM_INC
#pragma pack(push, 1)
typedef struct ucode {
  UINT32  version;
  UINT32  revision;
  UINT32  date;
  UINT32  processor;
  UINT32  checksum;
  UINT32  loader;
  UINT32  rsvd[6];
  UINT32  data[500];
} ucode_t;

typedef struct ucode_hdr {
  UINT32  version;
  UINT32  revision;
  UINT32  date;
  UINT32  processor;
  UINT32  checksum;
  UINT32  loader;
  UINT32  flags;
  UINT32  data_size;
  UINT32  total_size;
  UINT32  rsvd[3];
} ucode_hdr_t;

typedef struct ext_sig_hdr {
  UINT32  count;
  UINT32  checksum;
  UINT32  rsvd[3];
} ext_sig_hdr_t;

typedef struct ext_sig {
  UINT32  processor;
  UINT32  flags;
  UINT32  checksum;
} ext_sig_t;


#pragma pack(pop)
#endif // !ASM_INC

#endif // cpuhost_h
