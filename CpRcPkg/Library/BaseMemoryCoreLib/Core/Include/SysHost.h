//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  add RC error log to BMC and report error DIMM To screen(refer to Grantley)
//      Reason:     
//      Auditor:    Timmy Wang
//      Date:       May/26/2016
//
//*****************************************************************************


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
  SysHost.h

@brief
       This file contains reference code data structures

 ************************************************************************/

#ifndef _syshost_h
#define _syshost_h
//#define CCMRC       1

#include <Token.h>			//SMCPKG_SUPPORT++

#define RC_SIM_FASTCADB 0


#if SMCPKG_SUPPORT==0		//SMCPKG_SUPPORT++
#ifndef ENHANCED_WARNING_LOG
#define ENHANCED_WARNING_LOG 1
#endif
#else
#ifndef ENHANCED_WARNING_LOG
#define ENHANCED_WARNING_LOG 0
#endif
#endif //SMCPKG_SUPPORT==0	//SMCPKG_SUPPORT++
//
// Host reset states (0-7 are definable)
//
typedef enum BootMode {
  NormalBoot  = 0,  // Normal path through RC with full init, mem detection, init, training, etc.
                    //    Some of these MRC specific init routines can be skipped based on MRC input params
                    //    in addition to the sub-boot type (WarmBoot, WarmBootFast, etc).
  S3Resume    = 1   // S3 flow through RC. Should do the bare minimum required for S3
                    //    init and be optimized for speed.
} BootMode;


typedef struct sysHost SYSHOST, *PSYSHOST;

#include "DataTypes.h"
#include "SysHostChipCommon.h"
#include "Printf.h"
#include "PlatformHost.h"
#include "CpuHost.h"
#include "MemHost.h"
#include "KtiHost.h"
#include "UsbDebugPort.h"

#include "SysRegs.h"
#include "IioPlatformData.h"
#ifdef BDAT_SUPPORT
#include "bdat.h"
#endif // BDAT_SUPPORT
#include "EnhancedWarningLog.h"
#ifndef MINIBIOS_BUILD
#include <Ppi/MrcOemHooksPpi.h>
#include <Ppi/MrcHooksServicesPpi.h>
#include <Ppi/MrcHooksChipServicesPpi.h>
#endif
//
// -------------------------------------
// Declarations and directives
// -------------------------------------
// Reference Code (RC) revision in BCD format:
// [31:24] = Major revision number
// [23:16] = Minor revision number
// [15:8] = Release Candidate number
//
#define CCMRC_REVISION  0x00500000

#define SUCCESS 0

#define SDBG_MIN        BIT0
#define SDBG_MAX        BIT1
#define SDBG_TRACE      BIT2
#define SDBG_MEM_TRAIN  BIT3 + SDBG_MAX
#define SDBG_TST        BIT4
#define SDBG_CPGC       BIT5
#define SDBG_RCWRITETAG BIT6
#define SDBG_REG_ACCESS BIT6                                  // Displays all register accesses.
#define SDBG_MINMAX     SDBG_MIN + SDBG_MAX

#define SDBG_BUF_ENABLE      1
#define SDBG_BUF_DISABLE     0
#define SDBG_BUF_EN_DEFAULT  SDBG_BUF_DISABLE                 // Default disable

#define SDBG_PIPE_ENABLE      1
#define SDBG_PIPE_DISABLE     0
#define SDBG_PIPE_DEFAULT     SDBG_PIPE_DISABLE

#define SDBG_PIPE_COMPRESS_ENABLE     1
#define SDBG_PIPE_COMPRESS_DISABLE    0
#define SDBG_PIPE_COMPRESS_DEFAULT    SDBG_PIPE_COMPRESS_DISABLE


//
// nvramPtr DWORD ?     ; Linear pointer to hostNvram structure (optional for S3 resume path)
//                      ; The NVRAM image is returned in the nvram field of the sysHost structure
//                      ; on the normal boot path. The OEM BIOS should use SIZEOF (struct sysNvram)
//                      ; to determine NVRAM size requirements.
//                      ; NULL indicates the NVRAM pointer is not valid and internal NVRAM
//                      ; hooks will be used instead. See OemReadNvram() and OemWriteNvram() hooks
//                      ; in oemhook.c for more details.
//
#pragma pack(1)
#ifdef SERIAL_DBG_MSG
struct usbDebugPortSetup {
    UINT8   Enable;
    UINT8   Bus;
    UINT8   Device;
    UINT8   Function;
};
#endif // SERIAL_DBG_MSG


///
/// Common setup structure
///
struct commonSetup {
  UINT32  options;
#ifdef SERIAL_DBG_MSG
  UINT8   debugJumper;
  UINT8   serialDebugMsgLvl;
  UINT8   serialBufEnable;
  UINT8   serialPipeEnable;
  UINT8   serialPipeCompress;
  UINT16  bsdBreakpoint;
#endif
  UINT32  hideMemPhaseMsg[MAX_PHASE];
  UINT16  hideChRankMsg;
  UINT32  maxAddrMem;
  UINT16  debugPort;
  UINT32  nvramPtr;
  UINT32  sysHostBufferPtr;
  UINT32  mmCfgBase; ///< MMCFG Base address, must be 64MB (SKX, HSX, BDX) / 256MB (GROVEPORT) aligned
  UINT32  mmCfgSize; ///< MMCFG Size address, must be 64M, 128M or 256M
  UINT32  mmiolBase; ///< MMIOL Base address, must be 64MB aligned
  UINT32  mmiolSize; ///< MMIOL Size address
  UINT32  mmiohBase; ///< Address bits above 4GB, i,e, the hex value here is address Bit[45:32] for SKX/KNH, Bit[51:32] for ICX
  UINT8   pchumaEn; ///< 1 - Enable; 0 - Disable
  UINT8   numaEn; ///< 1 - Enable; 0 - Disable (i.e use UMA); N/A for 4S
  UINT8   DmiVcm;
  UINT8   bdatEn; ///< 1 - Enable; 0 - Disable
#ifdef SERIAL_DBG_MSG
  UINT16  consoleComPort;   ///< Base address of Console ComPort if in system
  UINT16  debugComPort;     ///< Base address of Debug ComPort if in system - CCMRC override
  struct  usbDebugPortSetup usbDebug;
#endif
  UINT8   BiosGuardEnabled;
  UINT8   lowGap;
  UINT8   highGap;
  UINT16  mmiohSize; ////<< Number of 1GB contiguous regions to be assigned for MMIOH space per CPU.  Range 1-1024
  UINT8   isocEn;    ///< 1 - Enable; 0 - Disable (BIOS will force this for 4S)
  UINT8   mesegEn;   ///< 1 - Enable; 0 - Disable (Mutually exclusive with 'isocEn')
  UINT8   dcaEn;     ///< 1 - Enable; 0 - Disable
  UINT8   PSMIEnabledSupport; ///< 1 - Enable; 0 - Disable
  UINT8   socketType;
  UINT8   ddrtXactor; ///< 1 - Enable; 0 - Disable
  UINT8   skipInitThrottling;
  UINT8   skipIioEarlyInit;
  UINT8   DfxAltPostCode; ///< 1 - Enable; 0 - Disable
  COMMONSETUP_CHIP  ///< Chip hook to enable commonSetup fields
}; //struct commonSetup

//
// -----------------------------------------------------------------------------
//

///
/// System Setup structure
//
struct sysSetup {
  struct cpuSetup     cpu;
  struct memSetup     mem;
  struct commonSetup  common;
  IIO_CONFIG          iio;
  SYSSETUP_CHIP        ///< Chip hook to enable sysSetup fields
}; //struct sysSetup

///
/// System NVRAM structure
//
struct sysNvram {
  struct cpuNvram     cpu;
  struct memNvram     mem;
  struct commonNvram  common;
  SYSNVRAM_CHIP        ///< Chip hook to enable sysNvram fields
}; //struct sysNvram

#pragma pack()

//
// -----------------------------------------------------------------------------
// Variable structures
//

//
// Warning log
//
#define MAX_LOG 64

struct warningEntry {
  UINT32  code;
  UINT32  data;
};
struct warningLog {
  UINT32              index;
  struct warningEntry log[MAX_LOG];
};

///
/// Enhanced Warning Log Spec defined data log structure
///
typedef struct {
  EWL_HEADER Header;          /// The size will vary by implementation and should not be assumed
  UINT8      Buffer[4*1024];  /// The spec requirement is that the buffer follow the header
} EWL_PUBLIC_DATA;

///
/// EWL private data structure.  This is going to be implementation dependent
///   When we separate OEM hooks via a PPI, we can remove this
///
typedef struct {
  UINT32            bufSizeOverflow;  // Number of bytes that could not be added to buffer
  UINT32            numEntries;       // Number of entries currently logged
  EWL_PUBLIC_DATA   status;           // Spec defined EWL
} EWL_PRIVATE_DATA;

struct processorCommon {
  UINT32  capid0;
  UINT32  capid1;
  UINT32  capid2;
  UINT32  capid3;
  UINT32  capid4;
  UINT32  capid5;
  UINT32  capid6;
};

#define USB_BUF_LIMIT (4096-160)
#define USB_BUF_SIZE  (USB_BUF_LIMIT + 160)

#pragma pack(1)
struct serialPacket {
  UINT32                    packetNum;
  UINT8                     threadNum;
  UINT16                    size;
  UINT8                     usbBuffer[USB_BUF_SIZE];
};
#pragma pack()

///
/// Common variable structure
///
struct commonVar {
  UINT32                    rcVersion;              ///< Version of the reference code last executed
  UINT32                    rcStatus;               ///< Non-zero indicates fatal error
  UINT8                     resetRequired;          ///< Indicate type of reset required. 0 = no reset, 2 = Power Good, 3 = Warm reset.
  BootMode                  bootMode;               ///< NormalBoot or S3Resume indication
  UINT8                     emulation;              ///< indicate if running on emulation software. 0 = No emulation software. 4 = Simics
                                                    // it is a copy of uncore Ubox CSR device 11, function 0, offset 0xB0.
  UINT8                     rcWriteRegDump;
  UINT8                     memHpSupport;           ///< 1 - if platform supports memory HP. 0 - otherwise
  UINT8                     CurrentReset;           ///< Reset type witnessed by the system (viz PWRGD, WARM)


  UINT8                     cpuType;                ///< CPU Type
  UINT8                     chopType[MAX_SOCKET];   ///< HCC, MCC, LCC, MCC-DE, LCC-DE (HSX family only)
  UINT32                    cpuFamily;              ///< Bit[19:4] of cpuid(1)_eax.  CPU_FAMILY_SKX
  UINT8                     stepping;               ///< Indicate current CPU stepping (a copy of RID register of PCI header offset 06h)
  UINT8                     socketId;               ///< socketID / NodeID of the current package BSP (0,1,2,3)
  UINT8                     sbsp;                   ///< Non-zero value indicates that the socket is System BSP
  UINT8                     numCpus;                ///< 1,..4. Total number of CPU packages installed and detected (1..4)by KTI RC
  UINT8                     MaxCoreCount;           ///< Max number of cores. 8 for JKT, 15 for IVT. Initialized based on cpuFamily field.
  UINT32                    FusedCores[MAX_SOCKET]; ///< Fused Core Mask in the package
  UINT8                     FusedCoreCount[MAX_SOCKET];///<  array for CPU package Core count, indexed by SocketID
  UINT32                    socketPresentBitMap;    ///< bitmap of socket Id with CPUs present detected by KTI RC
  UINT32                    FpgaPresentBitMap;      // bitmap of fpga NID detected by KTI RC
  UINT8                     MaxCoreToBusRatio[MAX_SOCKET];  ///< Package Max Non-turbo Ratio (per socket)
  UINT8                     MinCoreToBusRatio[MAX_SOCKET];  ///< Package Maximum Efficiency Ratio (per socket)
  UINT8                     printfSync;
  UINT8                     smbSync;
  UINT8                     SmbusAPSemaphoreAcquired;    ///<SMBus AP System Semaphore has been acquired, ensure to release it
  UINT16                    pmBase;                 ///< Power Management Base Address
  UINT32                    pwrmBase;
  UINT32                    rcbaBase;
  UINT16                    gpioBase;               ///< GPIO Base Address
  UINT16                    smbBase;                ///< SMB Base Address
  UINT16                    tolmLimit;
  UINT32                    tohmLimit;
  UINT32                    mmCfgBase;
  UINT32                    checkpoint;
  UINT32                    exitFrame;
  UINT32                    exitVector;
  UINT32                    rcEntryPoint;
  UINT32                    heapBase;
  UINT32                    heapSize;
  UINT64_STRUCT             cpuFreq;
  UINT64_STRUCT             startTsc;

  USB2_DEBUG_PORT_INSTANCE  usbDebugPort;
  struct serialPacket       packet;
  UINT32                    usbBufCount;
  UINT32                    usbStartCount;
  UINT32                    serialBufEnable;
  UINT32                    serialPacketCount;
  UINT8                     serialInBuf[8];
  UINT32                    serialInCount;
  UINT32                    serialInByte;
  UINT32                    serialCompressedBytes;
  UINT32                    serialUncompressedBytes;
  UINT32                    serialPacketNum;
  UINT8                     serialPipeEnable;
  UINT8                     serialPipeCompress;
  UINT32                    serialPipeNum;
#ifdef SERIAL_DBG_MSG
  UINT16                    globalComPort;   ///< Base address of Global ComPort currently in use
#endif

#define MAX_DEV 16
  UINT32                    dev[MAX_DEV];
  UINT32                    numDev;
  UINT8                     binMode;
  UINT32                    oemVariable;

#if ENHANCED_WARNING_LOG
  EWL_PRIVATE_DATA          ewlPrivateData;         // implementation data for EWL
#else
  struct warningLog         status;                 ///< Warning log
#endif
  struct processorCommon    procCom[MAX_SOCKET];
  UINT32                    MicroCodeRev;

#define MAX_PROMOTE_WARN_LIMIT  32
  UINT32                    promoteWarnLimit;
  UINT16                    promoteWarnList[MAX_PROMOTE_WARN_LIMIT];
  UINT8                     skuType;                ///< CPU SKU (EX , EP , DE, self boot, leverage boot )
  UINT8                     busIio[MAX_SOCKET];     ///< array for CPU IIO bus#, indexed by SocketID.
  UINT8                     EVMode;
  UINT8                     socketType;             ///< Socket P0
  UINT16                    LlcWays;                ///< Number of LLC way as determined by cpuid(eax=4)
  UINT32                    printfDepth;            ///< handle nested calls to get/releasePrintFControl
  UINT32                    smbusDepth;             ///< handles nested calls to get/releaseSmbus
  UINT8                     rdrandSupported;
  UINT8                     numActiveHAperSocket;   ///< Number active HA per socket (1 or 2)
  UINT8                     currentReset;           ///< Reset type witnessed by the system (viz PWRGD, WARM)
#if SMCPKG_SUPPORT
  UINT32                    ipmiTransPpi;
#endif
#ifdef SSA_FLAG
#if defined(SSA_LOADER_FLAG) && (!defined(RC_SIM) && !defined(MINIBIOS_BUILD))
  UINT32                     HobPtrSsaResults;      /// SSA HOB pointer will be typecast to UINT32
#endif
#endif //SSA_FLAG

  COMMONVAR_CHIP           ///< Chip hook to enable commonVar fields
};  //struct commonVar

///
/// System Variable structure
///
struct sysVar {
  struct cpuVar     cpu;
  struct memVar     mem;
  //IIO_GLOBALS       iio;
  IIO_VAR           iio;
  struct commonVar  common;
  CPU_CSR_ACCESS_VAR        CpuCsrAccessVarHost;    // Common resource for CsrAccessRoutines
  SYSVAR_CHIP              ///< Chip hook to enable sysVar fields
};  //struct sysVar

//
// System previous boot error structure
//
#define MC_BANK_STATUS_REG    1
#define MC_BANK_ADDRESS_REG   2
#define MC_BANK_MISC_REG      3

#define MSR_LOG_VALID   BIT31
#define MSR_LOG_UC      BIT29
#define MSR_LOG_EN      BIT28

struct mcBankTableEntry {
  UINT8   mcBankNum;
  UINT16  mcBankAddress;
};

struct mcBankStruct {
  UINT8           socketId;
  UINT8           mcBankNum;
  UINT64_STRUCT   mcStatus;
  UINT64_STRUCT   mcAddress;
  UINT64_STRUCT   mcMisc;
};

struct prevBootErrInfo {
  UINT32 validEntries;
  struct mcBankStruct mcRegs[MAX_PREV_BOOT_ERR_ENTRIES];
};

//
// ----------------------------------------------------------------
// System Ras Host structure
//
struct sysRasSetup {
  struct cpuSetup     cpu;
  struct commonSetup  common;
  SYSRASSETUP_CHIP              ///< Chip hook to enable sysRasSetup fields
};

struct sysRASVar {
  struct cpuVar     cpu;
  struct commonVar  common;
  SYSRASVAR_CHIP                ///< Chip hook to enable sysRasVar fields
};

typedef struct sysRasHost {
  const struct sysRasSetup setup;  ///< constant input data
  struct sysRASVar         var;    ///< variable, volatile data
}
SYSRASHOST, *PSYSRASHOST;

// Bit definitions for commonSetup.options
//                   ; PROMOTE_WARN_EN enables warnings to be treated as fatal error
//                   ; PROMOTE_MRC_WARN_EN enables MRC warnings to be treated as fatal error
//                   ; HALT_ON_ERROR_EN enables errors to loop forever
#define PROMOTE_WARN_EN           BIT0
#define PROMOTE_MRC_WARN_EN       BIT1
#define HALT_ON_ERROR_EN          BIT2


// -----------------------------------------------------------------------------
//

///
/// System host (root structure)
///
struct sysHost {
  struct sysSetup       setup;  ///< constant input data
  struct sysVar         var;    ///< variable, volatile data
  struct sysNvram       nvram;  ///< variable, non-volatile data for S3 and fast path
  struct prevBootErrInfo prevBootErr;
#ifdef  BDAT_SUPPORT
  BDAT_MEMORY_DATA_STRUCTURE    bdat;
  BDAT_RMT_STRUCTURE            bdatRmt;
#endif  // BDAT_SUPPORT
  UINT32  cpuFlows; ///< Which flows to take
  UINT32  ktiFlows;
  UINT32  qpiFlows;
  UINT32  opioFlows;
  UINT32  memFlows;
  UINT32  memFlowsExt;
  UINT32  hbmFlows;
  UINT32  LastCsrAddress[2];
  UINT32  LastCsrMmioAddr;
  UINT8   CsrCachingEnable;
#ifndef MINIBIOS_BUILD
  MRC_OEM_HOOKS_PPI           *MrcOemHooksPpi;
  MRC_HOOKS_SERVICES_PPI      *MrcHooksServicesPpi;
  MRC_HOOKS_CHIP_SERVICES_PPI *MrcHooksChipServicesPpi;
#endif
};

//
// Handle assertions with RC_ASSERT
//
#if defined(SIM_BUILD) || defined(IA32) || defined (HEADLESS_MRC)

#define RC_ASSERT(assertion, majorCode, minorCode) \
  if (!(assertion)) { \
    DebugPrintRc (host, 0xFF, "\n\nRC_ASSERT! %s: %u   %s ", __FILE__, __LINE__, #assertion);\
    FatalError (host, majorCode, minorCode);\
  }

#else

#define RC_ASSERT(assertion, majorCode, minorCode) \
  if (!(assertion)) { \
    CpuCsrAccessError (host, "\n\nRC_ASSERT! %s: %u   %s ", __FILE__, __LINE__, #assertion);\
  }

#endif


#define ACQUIRE_SEMAPHORE   1
#define RELEASE_SEMAPHORE   0
#define SYSTEMSEMAPHORE0    0
#define SYSTEMSEMAPHORE1    1

#ifdef SSA_FLAG
//BIOS SSA PRE-PROCESSOR DEFINES

#ifndef MAX_HOB_ENTRY_SIZE
#define MAX_HOB_ENTRY_SIZE  60*1024  //Needed for BiosSaveToBdat()
#endif //MAX_HOB_ENTRY_SIZE

#ifndef MINIBIOS_BUILD
#define MRC_SSA_SERVICES_SET  1  //1. Used to Include a BIOS specific header file in "SsaCommonConfig.h" for the BSSA code(Servers), but to exclude that file from the Test Content side of the code as the same*
//*header is shared on both sides. Not declaring #define in a Header file as both "MrcSsaServices.h" and the other "Headers" declared in them need to be exact copies on both sides..
// 2. An #ifndef in the "header", has the required structs redefined so that they are available only on the Test Content Side
#else
typedef UINT32  UINTN; //just for the minibios build "executetargetonly.c"
#endif //MINIBIOS_BUILD


//BSSA 1.0 GUID.. #define SSA_BIOS_SERVICES_PPI_GUID  {0xBAE29D7B, 0x89BB, 0x4223, 0xAF, 0x76, 0x96, 0xD0, 0xB3, 0x24, 0x9B, 0x36}
//{0xBAE29D7B, 0x89BB, 0x4223, { 0xAF, 0x76, 0x96, 0xD0, 0xB3, 0x24, 0x9B, 0x36 }}

#define EXPONENTIAL                     (0)
#define LINEAR                          (1)

//MRC_SYSTEM_INFO
#define MAX_CH_IMC                      (MAX_CH / MAX_IMC)     //Max of 3 Channels per MC
#define CPU_SOCKET_BIT_MASK             (((UINT32) (~0)) >> (32 - MAX_SOCKET))
#define BUS_WIDTH                       (64)
#define BUS_FREQ                        (1333)

//MRC_SERVER_CPGC_INFO
#define MAX_NUMBER_DQDB_CACHELINES                 64
#define MAX_NUMBER_DQDB_UNISEQS                    3
#define MAX_DQDB_UNISEQ_SEED_VALUE                 0xFFFFFF
#define MAX_DQDB_UNISEQ_L_VALUE                    0xFF
#define MAX_DQDB_UNISEQ_M_VALUE                    0xFF
#define MAX_DQDB_UNISEQ_N_VALUE                    0xFF
#define MAX_DQDB_INC_RATE_EXP_VALUE                7
#define MAX_DQDB_INC_RATE_LINEAR_VALUE             31
#define MAX_DQDB_UNISEQ_SEED_RELOAD_RATE_VAL       0x7
#define MAX_DQDB_UNISEQ_SEED_SAVE_RATE_VAL         0x3F
#define MAX_DQDB_INV_DC_SHIFT_RATE_EXP_VAL         0xF
#define MAX_NUMBER_CADB_ROWS                       16
#define MAX_NUMBER_CADB_UNISEQS                    4
#define MAX_NUMBER_CADB_UNISEQ_SEED_VALUE          0xFFFF
#define MAX_CADB_UNISEQ_L_VALUE                    0xFF
#define MAX_CADB_UNISEQ_M_VALUE                    0xFF
#define MAX_CADB_UNISEQ_N_VALUE                    0xFF
#define MAX_DUMMY_READ_L_VAL                       0xFF
#define MAX_DUMMY_READ_M_VAL                       0xFF
#define MAX_DUMMY_READ_N_VAL                       0xFF
//BOOLEAN IsLoopCountExponential;                  TRUE
#define MAX_LOOP_COUNT_VAL                         0x1F
#define MAX_START_DELAY_VAL                        0xFF
#define MAX_NUMBER_SUBSEQS                         10
#define MAX_BURST_LENGHT_EXP_VAL                   0x1F
#define MAX_BURST_LENGHT_LIN_VAL                   0x3F
#define MAX_INTER_SUBSEQ_WAIT_VAL                  0xFF
#define MAX_OFFSET_ADDR_UPDATE_RATE_VAL            0x1F
#define MAX_ADDRESS_INV_RATE_VAL                   0xF
#define MAX_RANK_ADDRESS_INC_RATE_EXP_VAL          0x1F
#define MAX_RANK_ADDRESS_INC_RATE_LIN_VAL          0x1F
#define MIN_RANK_ADDRESS_INC_VAL                   -4
#define MAX_RANK_ADDRESS_INC_VAL                   3
#define MAX_BANK_ADDRESS_INC_RATE_EXP_VAL          0x1F
#define MAX_BANK_ADDRESS_INC_RATE_LIN_VAL          0x1F
#define MIN_BANK_ADDRESS_INC_VAL                   -8
#define MAX_BANK_ADDRESS_INC_VAL                   7
#define MAX_ROW_ADDRESS_INC_RATE_EXP_VAL           0x1F
#define MAX_ROW_ADDRESS_INC_RATE_LIN_VAL           0x1F
#define MIN_ROW_ADDRESS_INC_VAL                    -2048
#define MAX_ROW_ADDRESS_INC_VAL                    2047
#define MAX_COL_ADDRESS_INC_RATE_EXP_VAL           0x1F
#define MAX_COL_ADDRESS_INC_RATE_LIN_VAL           0x1F
#define MIN_COL_ADDRESS_INC_VAL                   -128
#define MAX_COL_ADDRESS_INC_VAL                    127
#define MAX_NUMBER_RANK_MAP_ENTRIES                8
#define MAX_NUMBER_BANK_MAP_ENTRIES                0
#define MAX_STOP_ON_NTH_ERR_CNT_VAL                0x1F
#define MAX_NUMBER_ERROR_COUNTERS                  9
#define MAX_NUMBER_ROW_ADD_SWIZZLE_ENTRIES         6
#define MAX_ROW_ADD_SWIZZLE_VAL                    16
//MRC_SERVER_CPGC_INFO -END

//min/max limit for the margin parameter offset
//used for override the theoretical min/max limits
#define MIN_TIMING_MARGIN_PAMRAM_OFFSET         -32
#define MAX_TIMING_MARGIN_PAMRAM_OFFSET          31
#define MIN_VOLTAGE_MARGIN_PAMRAM_OFFSET        -64
#define MAX_VOLTAGE_MARGIN_PAMRAM_OFFSET         63

#define LOWER_ROW_ADDR_SWIZZLE_CNT      6   //Number of lower row address swizzle parameters
#define UPPER_ROW_ADDR_SWIZZLE_CNT      12  //Number of upper row address swizzle parameters in first upper register
#define MAX_ROW_ADDR_SWIZZLE_VAL        16  //Maximum swizzle value
#endif //SSA_FLAG

#endif // _syshost_h
