//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

  Memory Reference Code

  ESS - Enterprise Silicon Software

  INTEL CONFIDENTIAL

@copyright
  Copyright 2016 Intel Corporation All Rights Reserved.

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
  SysHostChipCommon.h

@brief
       These are structures and definitions from SKX SysHostChip.h
       that are needed in the core.

 ************************************************************************/

#ifndef _SYSHOST_CHIP_COMMON_H_
#define _SYSHOST_CHIP_COMMON_H_

#ifndef SEGMENT_ACCESS
#define SEGMENT_ACCESS
#endif

#define CCMRC_REVISION_CHANGELIST  320717

//
// Steppings
//
#define A0_REV      0x00

//TODO:Need to remove the old ones.  Keep for noe to allow building
#define CPU_HSX              0
#define CPU_IVT              1
#define CPU_BDX              2
#define CPU_SKX              0

// Defines for socketType
//
#define SOCKET_2S   0
#define SOCKET_4S   1
#define SOCKET_HEDT 2

//
// CpuPciAccess
//
#define READ_ACCESS  0
#define WRITE_ACCESS 1

#ifndef ASM_INC
//
// -----------------------------------------------------------------------------
// Nvram structures
//
#pragma pack(1)

typedef union {
  struct {
    UINT32 Bit0:1;
    UINT32 Bit1:1;
    UINT32 Bit2:1;
    UINT32 Bit3:1;
    UINT32 Bit4:1;
    UINT32 Bit5:1;
    UINT32 Bit6:1;
    UINT32 Bit7:1;
    UINT32 Bit8:1;
    UINT32 Bit9:1;
    UINT32 Bit10:1;
    UINT32 Bit11:1;
    UINT32 Bit12:1;
    UINT32 Bit13:1;
    UINT32 Bit14:1;
    UINT32 Bit15:1;
    UINT32 Bit16:1;
    UINT32 Bit17:1;
    UINT32 Bit18:1;
    UINT32 Bit19:1;
    UINT32 Bit20:1;
    UINT32 Bit21:1;
    UINT32 Bit22:1;
    UINT32 Bit23:1;
    UINT32 Bit24:1;
    UINT32 Bit25:1;
    UINT32 Bit26:1;
    UINT32 Bit27:1;
    UINT32 Bit28:1;
    UINT32 Bit29:1;
    UINT32 Bit30:1;
    UINT32 Bit31:1;
  } Bits;
    UINT32 Data;
} DUMMY_REG;

//
// -----------------------------------------------------------------------------
// Nvram structures
//

struct commonNvram {
  UINT64_STRUCT         cpuFreq;
  UINT32                rcVersion;      // Version of the reference code last executed
  UINT8                 sbspSocketId;
  UINT8                 platformType;
  UINT32                platformRevId;
  UINT32                meRequestedSizeNv;
  DUMMY_REG             TsegBase;
  DUMMY_REG             TsegLimit;
  DUMMY_REG             MeMemLowBaseAddr;
  DUMMY_REG             MeMemHighBaseAddr;
  DUMMY_REG             MeMemLowLimit;
  DUMMY_REG             MeMemHighLimit;
  DUMMY_REG             MeNcMemLowBaseAddr;
  DUMMY_REG             MeNcMemHighBaseAddr;
  DUMMY_REG             MeNcMemLowLimit;
  DUMMY_REG             MeNcMemHighLimit;
};
#pragma pack()

//
// System previous boot error structure
//
#define MAX_PREV_BOOT_ERR_ENTRIES   15

//
// Chip specific section of struct sysRasSetup
//
#define SYSRASSETUP_CHIP     \
  KTI_HOST_IN         kti;

//
// Chip specific section of struct sysRASVar
//
#define SYSRASVAR_CHIP       \
  KTI_HOST_OUT      kti;

//
// Chip specific section of struct commonVar
//
#define COMMONVAR_CHIP       \
  UINT8                     memRiserType;                         /* 1 - 4 RGT, 0 - 3 RGT */                          \
  UINT8                     KtiPortCnt;                           /* num KTI ports residing on each Socket */         \
  UINT8                     stackPresentBitmap[MAX_SOCKET];       /* bitmap of present stacks per socket */           \
  UINT8                     M2PciePresentBitmap[MAX_SOCKET];      /* bitmap of present m2pcie ring stop per socket */ \
  UINT8                     StackBus[MAX_SOCKET][MAX_IIO_STACK];  /* Bus of each stack */                             \
  UINT8                     SocketLastBus[MAX_SOCKET];            /* Last bus of socket */                            \
  UINT8                     SocketFirstBus[MAX_SOCKET];                                                               \
  UINT8                     segmentSocket[MAX_SOCKET];                                                                \
  UINT32                    mmCfgBaseH[MAX_SOCKET];                                                                   \
  UINT32                    mmCfgBaseL[MAX_SOCKET];                                                                   \
  UINT8                     mmrcBootMode;                                                                             \
  UINT32                    MchBar;                                                                                   \
  UINT32                    meRequestedSize; /* Size of the memory range requested by ME FW, in MB */                 \
  UINT8                     Pci64BitResourceAllocation;                                                               \
  UINT32                    HostAddress;                                                                              \
  UINT32                    SlavePipeAddress;                                                                         \
  UINT8                     MemRasFlag;                                                                               \
  UINT8                     inComingSktId;                                                                            \
  UINT8                     SystemRasType;                                                                            \

//
// Chip specific section of struct common
//
#define COMMONSETUP_CHIP       \

//
// Chip specific section of struct sysVar
//
#define SYSVAR_CHIP       \
  KTI_HOST_OUT      kti;     \

//
// Chip specific section of struct sysNvram
//
#define SYSNVRAM_CHIP     \
  KTI_HOST_NVRAM      kti;    \

//
// Chip specific section of struct sysSetup
//
#define SYSSETUP_CHIP     \
  KTI_HOST_IN         kti;    \
  UINT8   WFRWAEnable;    \
  UINT8   PmaxDisable;    \

//
// -----------------------------------------------------------------------------
// BIOS to PCode Mailbox Commands
//
#define MAILBOX_BIOS_CMD_ZERO                       0x00  // NOP
#define MAILBOX_BIOS_CMD_WRITE_PCS                  0x02  // Write Package Configuration Space
#define MAILBOX_BIOS_CMD_READ_PCS                   0x03  // Read Package Configuration Space
#define MAILBOX_BIOS_CMD_VR_INTERFACE               0x04  // New in HXS. SVID command for accessing VR regs.
  #define MAILBOX_BIOS_DATA_VR_REG_VENDOR_ID          0x00  //  Vendor ID
  #define MAILBOX_BIOS_DATA_VR_REG_PRODUCT_ID         0x01  //  Product ID
  #define MAILBOX_BIOS_DATA_VR_REG_PRODUCT_REV        0x02  //  Product Revision
  #define MAILBOX_BIOS_DATA_VR_REG_PRODUCT_DC         0x03  //  Product Date Code
  #define MAILBOX_BIOS_DATA_VR_REG_Lot_CODE           0x04  //  Lot Code
  #define MAILBOX_BIOS_DATA_VR_REG_PROTOCOL_ID        0x05  //  Protocol ID
  #define MAILBOX_BIOS_DATA_VR_REG_CAPABILITY         0x06  //  Capability
  #define MAILBOX_BIOS_VR_CMD_GET_REG                 0x07  //  GetRegs VR Command
#define MAILBOX_BIOS_CMD_READ_PCU_MISC_CONFIG       0x05  // New in HSX. Returns the value from the MISC_CONFIG memory variable.
#define MAILBOX_BIOS_CMD_WRITE_PCU_MISC_CONFIG      0x06  // New in HSX. HSX Changes: Adding chicken bits for bios to program
#define MAILBOX_BIOS_CMD_IO_SA_CONFIG               0x7C
#define MAILBOX_BIOS_BGF_PTR_SEPARATION_OVERRIDE    0x7D  // Override the default pointer separation values for BGFs
  #define MAILBOX_BIOS_BGF_INTERFACE_KTI_M3KTI        0x00  //   KTI/M3KTI BGF
  #define MAILBOX_BIOS_BGF_INTERFACE_IIO_M2PCIE       0x01  //   IIO/M2PCIe BGF
  #define MAILBOX_BIOS_BGF_INTERFACE_MC_M2M_133       0x02  //   MC/M2Mem - 133MHz ref clk
  #define MAILBOX_BIOS_BGF_INTERFACE_MC_M2M_100       0x03  //   MC/M2Mem - 100Mhz ref clk
  #define MAILBOX_BIOS_BGF_INTERFACE_PCU_R3KTI        0x04  //   PCU/R3KTI BGF - not used for SKX
#define MAILBOX_BIOS_CMD_MASK_FW_AGENT              0x7E  // New in IVT. BIOS can use this command to clear the FW agent link layer attribute in KTI parameter 2 on the neighboring socket
#define MAILBOX_BIOS_CMD_IIO_DISABLE                0x7F  // New in IVT. Disable unconnected PCIE/DMI links as well as clock gating entire IIO
#define MAILBOX_BIOS_CMD_VDD_RAMP                   0x80  // Triggers the Vdd ramp up sequence
  #define MAILBOX_BIOS_120V_RAMP_ENCODING_VR12        0xbf  // VR12 Ramp Value Encoding for 1.20v
  #define MAILBOX_BIOS_120V_RAMP_ENCODING_VR12_5      0x47  // VR12.5 Ramp Value Encoding for 1.20v
  #define MAILBOX_BIOS_120V_RAMP_ENCODING_VR13_5      0xbf  // VR13 5mV Ramp Value Encoding for 1.20v
  #define MAILBOX_BIOS_120V_RAMP_ENCODING_VR13_10     0x47  // VR13 10mV Ramp Value Encoding for 1.20v
#define MAILBOX_BIOS_CMD_DIMM_VR_PHASE_SHED         0x81  // Triggers the DIMM VR Phase Shedding flow in PCODE
  #define MAILBOX_BIOS_DATA_PS0                       0x00  //   PS0: full-phase
  #define MAILBOX_BIOS_DATA_PS1                       0x01  //   PS1: single-phase, typically <20A load
  #define MAILBOX_BIOS_DATA_PS2                       0x02  //   PS2: fixed loss, typically <5A load
#define MAILBOX_BIOS_CMD_DRAM_PM_RESET_CTR          0x82  // Resets an energy counter for DRAM Power Meter
#define MAILBOX_BIOS_CMD_DRAM_PM_START_CTR          0x83  // Starts an energy counter for DRAM Power Meter
#define MAILBOX_BIOS_CMD_DRAM_PM_STOP_CTR           0x84  // Stops an energy counter for DRAM Power Meter
#define MAILBOX_BIOS_CMD_DRAM_PM_READ_CTR           0x85  // Reads an energy counter for DRAM Power Meter
#define MAILBOX_BIOS_CMD_DRAM_PM_WRITE_POWER_COEFF  0x86  // Writes power coefficients
#define MAILBOX_BIOS_CMD_ACTIVE_VR_MASK             0x87  // Acquire a mask of active VRs discovered by PCODE
#define MAILBOX_BIOS_CMD_SET_KTI_FREQ               0x88  // Set KTI frequency value for next reset
#define MAILBOX_BIOS_CMD_READ_KTI_FREQ              0x89  // Read KTI frequency value for next reset
#define MAILBOX_BIOS_CMD_WRITE_PWR_BW_SCALE         0x8A  // Program the DRAM pbm conversion from remaining energy to transaction count for bandwidth throttling.
#define MAILBOX_BIOS_WRITE_DRAM_PBM_THRT_ADJUST     0x8B  // Program the DRAM pbm factor
#define MAILBOX_BIOS_CMD_QUIESCE_PECI               0x8C  // Quiesce PECI
#define MAILBOX_BIOS_CMD_UNQUIESCE_PECI             0x8D  // Removed the blocks on PECI
#define MAILBOX_BIOS_CMD_SYNCH_TSC_FOR_HOT_ADD      0x8F  // Used as the trigger mechanism for TSC synchronization of hot added/onlined sockets

#define MAILBOX_BIOS_CMD_CPU_ONLY_RESET             0x8F  // Assert/Deassert CPU Only Reset  0x8f  // Change the PCIe gear ratio
#define MAILBOX_BIOS_CMD_READ_PCIE_FREQ             0x90  // Read the current PCIE frequency
#define MAILBOX_BIOS_CMD_PROGRAM_KTI_PORTS          0x91  // New in HSX. Identify Ports in L1 mode.  Value: Bit[0]=Port0, Bit[1]=Port1,..
#define MAILBOX_BIOS_CMD_READ_CPU_VR_INFO           0x93  // Read CPU VR Info
  #define MAILBOX_BIOS_DATA_CORE_VR_ADDR              0x00  //  VR Address for CPU core VR
  #define MAILBOX_BIOS_DATA_SA_VR_ADDR                0x01  //  VR Address for CPU SA VR
  #define MAILBOX_BIOS_DATA_DRAM0_VR_ADDR             0x02  //  VR Address for DRAM0 VR
  #define MAILBOX_BIOS_DATA_DRAM1_VR_ADDR             0x03  //  VR Address for DRAM1 VR
#define MAILBOX_BIOS_CMD_VR12_WA_ENABLE             0x94  // Read DRAM VR Info
  #define MAILBOX_BIOS_DATA_VR12WA_ENC_SET_VID_AUTOACK   0x00    // VR12 WA enable - WA encoding 0
  #define MAILBOX_BIOS_DATA_VR12WA_GVFSM_DISABLE_PREEMPT 0x01    // VR12 WA enable - WA encoding 1
  #define MAILBOX_BIOS_DATA_VR12WA_ENC_MORE_THAN_10MV    0x02    // VR12 WA enable - WA encoding 2
  #define MAILBOX_BIOS_DATA_VR12WA_ENC_DISABLE_SA_ITD    0x03    // VR12 WA enable - WA encoding 3
  #define MAILBOX_BIOS_DATA_VR12WA_ENC_IDLE_CLK_ON       0x04    // VR12 WA enable - WA encoding 4
#define MAILBOX_BIOS_CMD_DRAM_RAPL_MODE             0x96  // DRAM RAPL Mode
#define MAILBOX_BIOS_CMD_DRAM_PM_CHN_VR_MAP         0x97  // DRAM PM Chn VR Map
#define MAILBOX_BIOS_CMD_PROBE_DRAM_VR              0x98  // Request that Pcode re-poll the DRAM VRs
#define MAILBOX_BIOS_CMD_REMOVE_DRAM_VR             0x99  // request that pcode quiesce its interactions with the specified DRAM VR prior to its removal during a memory riser hot-swap
#define MAILBOX_BIOS_CMD_RAS_START                  0x9A  // Stop deep C state (non C0 state) during RAS events
#define MAILBOX_BIOS_CMD_RAS_EXIT                   0x9B  // Re-start C state (no clipping)  RAS events
#define MAILBOX_BIOS_CMD_KTI_DISABLE                0x9C  // Request Pcode to turn off FIVRs on disabled KTI ports
#define MAILBOX_BIOS_CMD_FAST_RAPL_DUTY_CYCLE       0x9D  // Bios programs the Nstrike PL2 duty cycle for fast rapl. Pcode will clip the data to a programmable range.
#define MAILBOX_BIOS_CMD_DRAM_PM_DISABLE            0x9E  // New in SKX. Bios disables DRAM PM
#define MAILBOX_BIOS_CMD_DRAM_PM_ENABLE             0x9F  // New in SKX. Bios enables DRAM PM
#define MAILBOX_BIOS_CMD_DRAM_PM_ENABLED_MASK       0xA0  // New in SKX. Bios checks DRAM PM mask
#define MAILBOX_BIOS_CMD_DRAM_THERMAL_DISABLE       0xA1  // New in SKX. Bios enables or disables DRAM Thermal
#define MAILBOX_BIOS_CMD_DRAM_THERMAL_ENABLE        0xA2  // New in SKX. Bios enables or enables DRAM Thermal
#define MAILBOX_BIOS_CMD_DRAM_THERMAL_ENABLED_MASK  0xA3  // New in SKX. Bios checks DRAM Thermal Mask
  #define MAILBOX_BIOS_MC_0_MASK                      0x01 // New in SKX. Mask for MCID 0
  #define MAILBOX_BIOS_MC_1_MASK                      0x02 // New in SKX. Mask for MCID 1
#define MAILBOX_BIOS_CMD_DRAM_DIMM_VR_MAPPING       0xA4  // New in SKX.
#define MAILBOX_BIOS_CMD_DRAM_VR_MC_MAPPING         0xA5  // New in SKX.
#define MAILBOX_BIOS_CMD_SET_MC_FREQ                0xA6  // New in SKX. Set MC frequency value for next reset (for all instaces of MCs)
  #define MAILBOX_BIOS_MC_FIVR_ALT_VOLT               0x80 // New in SKX.  1: voltage @ freq <=2400
#define MAILBOX_BIOS_CMD_READ_MC_FREQ               0xA7  // New in SKX. Bios reads the current MC frequency (for instance 0, which is same for all MCs)
#define MAILBOX_BIOS_CMD_DRAM_VR_SENSOR_MAPPING     0xA8  // New in SKX. BIOS sets the 12V sensor MC Mapping
#define MAILBOX_BIOS_CMD_MCP_TEMP_CONFIG            0xA9  // New in SKX.
#define MAILBOX_BIOS_CMD_MC_PATROL_SCRUB_INTERVAL   0xAA  // New in SKX. Bios programs Patrol Scrub Interval
#define MAILBOX_BIOS_CMD_DRAM_MIN_OPS               0xAB  //
#define MAILBOX_BIOS_CMD_SREF_PKGC                  0xAD  //New in SKX. BIOS programs the SREF_PKGC register values same as SREF_LL3
#define MAILBOX_BIOS_CMD_SMB_ACCESS                 0xAE  // New in SKX B-step. BIOS to read/write into the respective SMBus2 control register
  #define MAILBOX_BIOS_SMB_ACCESS_READ                0x00
  #define MAILBOX_BIOS_SMB_ACCESS_WRITE               0x01
  #define MAILBOX_BIOS_SMB_ACCESS_SEMAPHORE_NONE      0x00
  #define MAILBOX_BIOS_SMB_ACCESS_SEMAPHORE_REQUESET  0x01
  #define MAILBOX_BIOS_SMB_ACCESS_SEMAPHORE_RELEASE   0x02
  #define MAILBOX_SMB2_CMD_INDEX                      0x00
  #define MAILBOX_SMB2_CMD_EXT_INDEX                  0x01
  #define MAILBOX_SMB2_DATA_INDEX                     0x02
#define MAILBOX_BIOS_CMD_SET_CORE_MESH_RATIO        0xAF  //New in SKX. BIOS programs ratio for core and for mesh (WFR WA)
#define MAILBOX_BIOS_CMD_MISC_WORKAROUND_ENABLE     0xB0  //New in SKX. BIOS needs to send 2LM enabled/disabled status to make uclk >= dclk (WA)
  #define MAILBOX_BIOS_ADDDC_QUIESCE                0x02  // Quiesce memory to for ADDDC
  #define MAILBOX_BIOS_NOMCSTOPONDDR4ERR            0x04  //
  #define MAILBOX_BIOS_VCCSA_VCCIO_DISABLE          0x05  // 
#define MAILBOX_BIOS_CMD_READ_MC_VOLTAGE            0xB2  //New in SKX. BIOS needs to read Vccp voltage from FIVR via PCODE
#define MAILBOX_BIOS_CMD_REMOVE_MCP                 0xB6  //New in SKXP. BIOS needs to power-down the MCP VRs and disconnect them from CPU when FPGA is disabled.
#define MAILBOX_BIOS_CMD_XEON_POWER_LIMIT           0xB8  //New in SKX. Allow BIOS to read and write the Xeon CPU die power limit
  #define MAILBOX_BIOS_DIE_POWER_LIMIT_MASK           0x7FFF  //Bit[14:0]
#define MAILBOX_BIOS_CMD_OOB_INIT_EPP               0xB9  //New in SKX. Allow BIOS to set initial default EPP value depending EPB setting

//
// Mesh Mode related PCode Mailbox Commands
//
#define MAILBOX_BIOS_CMD_MESH_MODE                     0xA1  // Mesh Mode (SKXTODO: conflict with other commands)
#define MAILBOX_BIOS_MESH_MODE_ID_ALLTOALL             0x00
#define MAILBOX_BIOS_MESH_MODE_ID_HEMISPHERE           0x01
#define MAILBOX_BIOS_MESH_MODE_ID_QUADRANT             0x02
#define MAILBOX_BIOS_MESH_MODE_ID_SNC                  0x03
  #define MAILBOX_BIOS_SUBCMD_MESH_MODE_SNC2             0x00
  #define MAILBOX_BIOS_SUBCMD_MESH_MODE_SNC4             0x01


//
// VR Addresses
//
#define MAILBOX_BIOS_CORE_VR_ADDR               0  // Core  VR Addr encoded with the SVID VR address
#define MAILBOX_BIOS_SA_VR_ADDR                 1  // SA    VR Addr encoded with the SVID VR address
#define MAILBOX_BIOS_DRAM0_VR_ADDR              0x10  // DRAM0 VR Addr encoded with the SVID VR address
#define MAILBOX_BIOS_DRAM1_VR_ADDR              0x12  // DRAM1 VR Addr encoded with the SVID VR address
#define MAILBOX_BIOS_DRAM2_VR_ADDR              0x14  // DRAM2 VR Addr encoded with the SVID VR address
#define MAILBOX_BIOS_DRAM3_VR_ADDR              0x16  // DRAM3 VR Addr encoded with the SVID VR address

//
// VR Error Codes
//
#define MAILBOX_BIOS_ERROR_CODE_NO_ERROR               0x00
#define MAILBOX_BIOS_ERROR_CODE_INVALID_COMMAND        0x01
#define MAILBOX_BIOS_ERROR_CODE_BAD_VR_ADDR            0x02
#define MAILBOX_BIOS_ERROR_CODE_SVID_ERROR             0x03
#define MAILBOX_BIOS_ERROR_CODE_UNSUPPORTED_KTI_FREQ   0x04
#define MAILBOX_BIOS_ERROR_CODE_UNSUPPORTED_IIO_FREQ   0x05
#define MAILBOX_BIOS_ERROR_CODE_BAD_DIMM_INDEX         0x06
#define MAILBOX_BIOS_ERROR_CODE_BAD_BW_SCALE           0x07
#define MAILBOX_BIOS_ERROR_CODE_COUNTER_NOT_ENABLED    0x08
#define MAILBOX_BIOS_ERROR_CODE_BAD_CHANNEL_INDEX      0x09
#define MAILBOX_BIOS_ERROR_CODE_BAD_RANK_INDEX         0x0a
#define MAILBOX_BIOS_ERROR_CODE_INVALID_VR_WA_CMD      0x0b
#define MAILBOX_BIOS_ERROR_CODE_VR_REG_READ_ERROR      0x0c
#define MAILBOX_BIOS_ERROR_CODE_UNSUPPORTED_MC_FREQ    0x1b
#define MAILBOX_BIOS_ERROR_CODE_INVALID_WORKAROUND     0x21

//
// VR Voltage Modes
//
#define VR_MODE_12          1
#define VR_MODE_12_5        2
#define VR_MODE_13_10       4  // VR13 10mV
#define VR_MODE_13_5        7  // VR13 5mV

#define RANK_MARGIN_TOOL_REQ  (2 << 24)
#define JEDEC_INIT_SEQ_REQ  (1 << 24)
#define GET_REQ       0
#define RESTORE_REQ   1

//
// Platform Assigned CSRs for PIPE/SOCKET/MRC MILESTONES, Pipe.h
// These macros are used by code that does not have visiblity to the chip register definitions
//
#define SR_MEMORY_DATA_STORAGE_DISPATCH_PIPE_CSR_ADDR(host)        (MEM_CHIP_POLICY_VALUE(host, SrMemoryDataStorageDispatchPipeCsr))
#define SR_MEMORY_DATA_STORAGE_COMMAND_PIPE_CSR_ADDR(host)         (MEM_CHIP_POLICY_VALUE(host, SrMemoryDataStorageCommandPipeCsr))
#define SR_MEMORY_DATA_STORAGE_DATA_PIPE_CSR_ADDR(host)            (MEM_CHIP_POLICY_VALUE(host, SrMemoryDataStorageDataPipeCsr))
#define SR_BDAT_STRUCT_PTR_CSR_ADDR(host)                          (MEM_CHIP_POLICY_VALUE(host, SrBdatStructPtrCsr))

// Scratchpad register indirect references
#define SR_BIOS_SERIAL_DEBUG                       6
#define SR_POST_CODE                               7


#endif // ndef ASM_INC
#endif // _SYSHOST_CHIP_COMMON_H_
