/**

Copyright (c) 2005-2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file MMRCData.h

  Internal and external data structures, Macros, and enumerations.

**/
#ifndef _MMRCDATA_H_
#define _MMRCDATA_H_

#include "MmrcProjectDefinitions.h"
#include "SysHost.h"

#if SIM
#include <stdio.h>
#endif

//
// Define each stepping in the MMRC taking the values
// generated in the spreadsheet
//
#define STEPPING_X0   STEPPING_X0_SI
#define STEPPING_A0   STEPPING_A0_SI
#define STEPPING_A1   STEPPING_A1_SI
#define STEPPING_B0   STEPPING_B0_SI
#define STEPPING_B1   STEPPING_B1_SI

#if MRC_FLOW
#if !defined SIM  && !defined JTAG
#define PRINT_FUNCTION_INFO     MmrcDebugPrint ((SDBG_MIN, "%a\n", __FUNCTION__ ));
#define PRINT_FUNCTION_INFO_MAX MmrcDebugPrint ((SDBG_MAX, "%a\n", __FUNCTION__ ));
#else
#define PRINT_FUNCTION_INFO     MmrcDebugPrint ((SDBG_MIN, "%s\n", __FUNCTION__ ));
#define PRINT_FUNCTION_INFO_MAX MmrcDebugPrint ((SDBG_MAX, "%s\n", __FUNCTION__ ));
#endif
#else
#define PRINT_FUNCTION_INFO     
#define PRINT_FUNCTION_INFO_MAX 
#endif

#define MIN(a,b)          (((a) < (b)) ? (a) : (b))
#define MAX(a,b)          (((a) > (b)) ? (a) : (b))
#define GET_ABSOLUTE(a,b) (((a) > (b)) ? (a - b) : (b - a))  
#ifndef MMRC_NA
#define MMRC_NA           0xFF
#endif

#define HPET_1US          0x000F
#define HPET_1MS          0x37EF

#define EXTERNAL      0xFE
#define INTERNAL      0xFE

#define GETSETVREF    0
#define GETSETDELAY   1
#define GETCMD        2
#define LOW           0
#define HIGH          1

#ifndef ABSOLUTE 
#define ABSOLUTE                      1
#define RELATIVE                      2
#endif
#define CONSOLIDATE_STROBES           4
#define MULTIKNOB                     8
#define SWEEP_ALL_STROBES_TOGETHER    16
#define START_SWEEP_CENTER            32

//
// Defines for register initialization table.
//
#define C_(c)      (c)
#define _I(i)      (i)
#define CI(c,i)    (c), (i)
#define RB(r,b)    (((r & 0x0f) << 4) + ((b & 0x0f) << 0))
#define BB(bt,be)  (((bt & 0x0f) << 4) + ((be & 0x0f) << 0))
#define BB2(bt,be) (bt & 0xff), (be & 0xff)
#define AP(ac,pf)  (((ac & 0x07) << 5) + ((pf & 0x1f) << 0))
#define MASK1(m)   CONV1(m)
#define MASK2(m)   CONV2(m)
#define MASK3(m)   CONV3(m)
#define MASK4(m)   CONV4(m)
#define MASK5(m)   CONV5(m)
#define MASK6(m)   CONV6(m)
#define MASK7(m)   CONV7(m)
#define MASK8(m)   CONV8(m)
#define VAL1(m)    CONV1(m)
#define VAL2(m)    CONV2(m)
#define VAL3(m)    CONV3(m)
#define VAL4(m)    CONV4(m)
#define VAL5(m)    CONV5(m)
#define VAL6(m)    CONV6(m)
#define VAL7(m)    CONV7(m)
#define VAL8(m)    CONV8(m)
//
// Frequencies
//
#define MMRC_800           0x00
#define MMRC_1066          0x01
#define MMRC_1333          0x02
#define MMRC_1600          0x03
#define MMRC_1866          0x04
#define MMRC_2133          0x05
#define MMRC_2400          0x06
#define MMRC_2666          0x07
#define MMRC_2933          0x08
#define MMRC_3200          0x09

#define C_MAXDDR          MAXDDR - MINDDR + 1

#define COREFREQ_800      0x00
#define COREFREQ_1066     0x01
#define COREFREQ_1333     0x02
#define COREFREQ_1600     0x03

#define C_MAXCOREFREQ     MAXCOREFREQ - MINCOREFREQ + 1

#define STROBE 0

//
// Our current tools don't know about the RDRAND instruction yet.
// Note that this encoding will not work in a 16bit code segment.
// In 64bit CPU mode, this will fill RAX but we still only
// use the low 32 bits.
//
#define RDRAND_EAX  __asm _emit 0x0f __asm _emit 0xc7 __asm _emit 0xf0

typedef enum {
  FreqA,
  FreqB,
  MaxDvfsFreqs
} DVFS_FREQUENCIES;

typedef enum {
  MrcMemInit,
  MrcMemTest
} MEM_INIT_OR_TEST;

typedef enum {
  MrcNoHaltSystemOnError,
  MrcHaltSystemOnError,
  MrcHaltCpgcEngineOnError,
  MrcNoHaltCpgcEngineOnError
} HALT_TYPE;

typedef enum {
  TimingTras = 0,
  TimingTrp,
  TimingTrcd,
  TimingTwr,
  TimingTrfc,
  TimingTwtr,
  TimingTrrd,
  TimingTrrdl,
  TimingTrtp,
  TimingTfaw,
  TimingTccd,
  TimingTccdl,
  TimingTwtp,
  TimingTwcl,
  TimingTcmd,
  TimingTcl,
  TimingTrppb,
  TimingTrpab,
  TimingTrcpb,
  TimingTwtrl,
  TimingTrdpre,
  TimingTwrpre,
  TimingTrefi,
  TimingTxs,
  TimingTxsdll,
  TimingTxsr,
  TimingTckesre,
  TimingTxp,
  TimingTxpdll,
  TimingTcke,
  TimingTpd,
  TimingTprpden,
  TimingTrdpden,
  TimingTwrpden,
  TimingTrefpden,
  TimingTmrspden,
  TimingTcpded,
  TimingTmprr,
  TimingTmrd,
  TimingTmod,
  TimingTmrr,
  TimingTmrw,
  TimingOdtlon,
  TimingOdtloff,
  TimingOdth4,
  TimingOdth8,
  TimingTaonpdmin,
  TimingTaonpd,
  TimingTaofpdmin,
  TimingTaofpdmax,
  TimingTrpre,
  TimingTwpre,
  TimingTrrsr,
  TimingTrrsrl,
  TimingTrrdr,
  TimingTrrdd,
  TimingTwwsr,
  TimingTwwsrl,
  TimingTwwdr,
  TimingTwwdd,
  TimingTrwsr,
  TimingTrwdr,
  TimingTrwdd,
  TimingTwrsr,
  TimingTwrdr,
  TimingTwrdd,
  TimingTxpr,
  TimingTdllk,
  TimingTzqinit,
  TimingTzqoper,
  TimingTzqcs,
  TimingTzqreset,
  MaxTimingData
} TIMING_DATA_TYPE;

//
// Bit definitions
//
#ifndef BIT0
#define BIT0                  0x00000001
#define BIT1                  0x00000002
#define BIT2                  0x00000004
#define BIT3                  0x00000008
#define BIT4                  0x00000010
#define BIT5                  0x00000020
#define BIT6                  0x00000040
#define BIT7                  0x00000080
#define BIT8                  0x00000100
#define BIT9                  0x00000200
#define BIT10                 0x00000400
#define BIT11                 0x00000800
#define BIT12                 0x00001000
#define BIT13                 0x00002000
#define BIT14                 0x00004000
#define BIT15                 0x00008000
#define BIT16                 0x00010000
#define BIT17                 0x00020000
#define BIT18                 0x00040000
#define BIT19                 0x00080000
#define BIT20                 0x00100000
#define BIT21                 0x00200000
#define BIT22                 0x00400000
#define BIT23                 0x00800000
#define BIT24                 0x01000000
#define BIT25                 0x02000000
#define BIT26                 0x04000000
#define BIT27                 0x08000000
#define BIT28                 0x10000000
#define BIT29                 0x20000000
#define BIT30                 0x40000000
#define BIT31                 0x80000000
#endif

#define EVEN_MODE             1
#define ODD_MODE              0

#define OVERRIDE              1
#define NO_OVERRIDE           0

#define ZEROS                 0x00
#define ONES                  0xff

#define NOT_MET               0
#define MET                   1
#define MET2                  2
#define UP                    1
#define DN                    0

#define COUNT_UP_FOR_ONE      1
#define COUNT_DN_FOR_ZERO     0
#define SKIP_ACTION           0
#define TAKE_ACTION           1
#define PFCT_GLOBAL           0
#define PFCT_INDEPENDENT      0

#define LOC_FLAGS_U(x)        (((x[6])>>5) & 1)

#define RDWR_NOT_DONE         0
#define RDWR_DONE             1
#define RDWR_LOW              0
#define RDWR_HIGH             1

//
// RD/WR Leveling
//
#define RD_LEVELING           1
#define WR_LEVELING           2

#define CMDLOWHIGH_NONE   0
#define CMDLOWHIGH_LOW    1
#define CMDLOWHIGH_HIGH   2
#define CMDLOWHIGH_BOTH   3
#define LOOKING_FOR_FAILURE 0
#define FOUND_FAILURE       1
#define PHASE0_SEARCH_LOW   0
#define PHASE0_SEARCH_HIGH  1
#define PHASE0_FOUND_HIGH   2

#define REG_UNDEFINED     0xFF
// 
// Macros for converting Little-Endian to Big-Endian.
//
#define CONV1(x)              (x&0xff)
#define CONV2(x)              ((x>>0)&0xff), ((x>>8)&0xff)
#define CONV3(x)              ((x>>0)&0xff), ((x>>8)&0xff), ((x>>16)&0xff)
#define CONV4(x)              ((x>>0)&0xff), ((x>>8)&0xff), ((x>>16)&0xff), ((x>>24)& 0xff)
#define CONV5(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff)
#define CONV6(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff), ((x>>40)& 0xff)
#define CONV7(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff), ((x>>40)& 0xff), ((x>>48)& 0xff)
#define CONV8(x)              ((x>>0)& 0xff), ((x>>8)& 0xff), ((x>>16)& 0xff), ((x>>24)& 0xff), ((x>>32)& 0xff), ((x>>40)& 0xff), ((x>>48)& 0xff), ((x>>56)& 0xff)

// 
// Macros for MASK/VAL which are used in the projectlibraries.c declarations.
//
#define MASK(x)               CONV4(x)
#define VAL(x)                CONV4(x)

#define ASSIGNDONE            (0xff)
#define DETREGLIST_NULL       ((DETAILED_REGISTER_STRING *) 0xFFFFFFFF)
#define REGLIST_NULL          ((REGISTER_STRING *)    0xFFFFFFFF)

// 
// Read Value from cache
//
#define CMD_GET_CACHE         (RD_ONLY)
//
// Read from register
// 
#define CMD_GET_REG           (RD_ONLY|RD_REG)
//
// Read from register and update Value to cache
//
#define CMD_GET_REG_UC        (RD_ONLY|RD_REG|UPD_CACHE)
// 
// Read register, apply offset, write back to register
//
#define CMD_SET_OFFSET        (WR_OFF|FC_WR)
// 
// Write to register with offset to Cache Value, apply Value condition checking and update final Value to cache
//
#define CMD_SET_OFFCAC_UC     (WR_OFF|UPD_CACHE)
// 
// Write to register with offset to Reg Value, apply Value condition checking and update final Value to cache
//
#define CMD_SET_OFFREG_UC     (WR_OFF|RD_REG|UPD_CACHE)
// 
// Write to register with offset to Cache Value, force write with NO Value condition checking and update final Value to cache
//
#define CMD_SET_OFFCAC_FC_UC  (WR_OFF|FC_WR|UPD_CACHE)
// 
// Write to register with offset to Reg Value, force write with NO Value condition checking and update final Value to cache
//
#define CMD_SET_OFFREG_FC_UC  (WR_OFF|FC_WR|RD_REG|UPD_CACHE)
// 
// Write to register with input Value, apply Value condition checking and update final Value to cache
//
#define CMD_SET_VAL_UC        (UPD_CACHE)
// 
// Write to register with input Value, apply Value condition checking
//
#define CMD_SET_VAL           0
// 
// Write to register with input Value, force write with NO Value condition checking and update final Value to cache
//
#define CMD_SET_VAL_FC_UC     (FC_WR|UPD_CACHE)
// 
// Write to register with input Value, force write with NO Value condition checking
//
#define CMD_SET_VAL_FC        (FC_WR)

#define STATUS_PASS                0
#define STATUS_FAIL                1
#define STATUS_TYPE_NOT_SUPPORTED  2
#define STATUS_CMD_NOT_SUPPORTED   3
#define STATUS_LIMIT               4

//
// Voltages
//
#define DDR_VOLTAGE_NOT_DETECTED  0
#define DDR_VOLTAGE_125V          1
#define DDR_VOLTAGE_135V          2
#define DDR_VOLTAGE_150V          3

#ifdef MINIBIOS
#define Shr64(Value, Shift) LShiftU64 (Value, Shift)
#define Shl64(Value, Shift) LRightU64 (Value, Shift)
#endif

#define ODD_MODE_BITSHIFTS    16
#define EVEN_MODE_BITSHIFTS   1

//
// 1D/2D Eye Diagram Debug Message Parameters
//
#define MARGIN_DISTANCE(x,y)     ((x)>(y) ? (x)-(y) : (y)-(x)) 
#define MAX_LINE_SIZE            80   
#define	EYEDATA_1D_VREFPI_OFFSET 0    

typedef enum {
  MmrcNotStarted = 0,
  MmrcStarted,
  MmrcFinished
} MMRC_STATUS_FLOW;

typedef enum {
  TypeDdr3 = 0,
  TypeDdr3L,
  TypeDdr3U,
  TypeDdr3All,
  TypeLpDdr2,
  TypeLpDdr3,
  TypeDdr4,
  TypeWio2,
  TypeDdrT
} MEM_TYPE;

#pragma pack(1)
//
// Power Knobs supported
//
typedef enum {
  SocOdt = 0,
  DramRttWrOdt,
  DramRttNomOdt,
  MaxPowerKnobs
} POWER_KNOBS;

typedef enum {
  GlobalFirstTimeExecution = 0,
  TimingBelowTrigger,
  VoltageBelowTrigger,
  ExecuteTrainingTrigger,
  MaxTriggerLog
} TRIGGER_LOG;

typedef enum {
  ExecuteRead = 0,
  ExecuteWrite,
  MaxTrainingRequired
} TRAINING_REQUIRED;

typedef enum {
  MarginT,
  MarginV,
  MaxMarginVerify
} MARGIN_VERIFY;

typedef enum {
  Absolute = 0,
  AbsoluteTrigger,
  Relative,
  ExecuteTrainingDrop,
  MaxPowerCriteria
} POWER_CRITERIA;
//
// Supported DRAM typed for Power Training
//
typedef enum {
  PowerDdr3l = 0,
  PowerLpddr3,
  MaxDdrTypesSupportedForPower
} DDR_TYPES_SUPPORTED_FOR_POWER;
// 
// Allocated from Stronger to weaker
//
typedef enum {
  Odt20,
  Odt30,
  Odt40,
  Odt60,
  Odt90,
  Odt120,
  Odt150,
  Odt180,
  Odt210,
  Odt240,
  OdtInfinite,     // "OFF"
  MaxOdtValues,
  InvalidOdtValue
} ODT_VALUES;
//
// Initial values for each power nob  
//
typedef struct {
  UINT8   InitialValue[MaxDdrTypesSupportedForPower][MaxPowerKnobs];
  UINT8   FinalValue[MaxDdrTypesSupportedForPower][MaxPowerKnobs];
  BOOLEAN Execute[MaxTrainingRequired]; // Only used when both Read & Write Training need to be executed
} POWER_KNOB_VALUES;

//
// Rank Margin Tool Data Index
//
typedef enum {
  RxDqLeft,
  RxDqRight,
  RxVLow,
  RxVHigh,
  TxDqLeft,
  TxDqRight,
  TxVLow,
  TxVHigh,
  CmdLeft, 
  CmdRight,
  MaxRMTData,
} RMT_DATA_TYPE;

typedef enum  {
  Pfct =  0,
  PfctT,
  PfctC,
  PfctCT,
  PfctF,
  PfctFT,
  PfctFC,
  PfctFCT,
  PfctP,
  PfctPT,
  PfctPC,
  PfctPCT,
  PfctPF,
  PfctPFT,
  PfctPFC,
  PfctPFCT,
  MaxPfct,
} ENUM_PCFT_TYPE;


//
// Byte Enables supported in the CAPSULEINIT Sequences.
//
typedef enum {
  _00000000,
  _00000001,
  _00000010,
  _00000011,
  _00000100,
  _00000101,
  _00000110,
  _00000111,
  _00001000,
  _00001001,
  _00001010,
  _00001011,
  _00001100,
  _00001101,
  _00001110,
  _00001111,
  _00010000,
  _00010001,
  _00010010,
  _00010011,
  _00010100,
  _00010101,
  _00010110,
  _00010111,
  _00011000,
  _00011001,
  _00011010,
  _00011011,
  _00011100,
  _00011101,
  _00011110,
  _00011111,
  _00100000,
  _00100001,
  _00100010,
  _00100011,
  _00100100,
  _00100101,
  _00100110,
  _00100111,
  _00101000,
  _00101001,
  _00101010,
  _00101011,
  _00101100,
  _00101101,
  _00101110,
  _00101111,
  _00110000,
  _00110001,
  _00110010,
  _00110011,
  _00110100,
  _00110101,
  _00110110,
  _00110111,
  _00111000,
  _00111001,
  _00111010,
  _00111011,
  _00111100,
  _00111101,
  _00111110,
  _00111111,
  _01000000,
  _01000001,
  _01000010,
  _01000011,
  _01000100,
  _01000101,
  _01000110,
  _01000111,
  _01001000,
  _01001001,
  _01001010,
  _01001011,
  _01001100,
  _01001101,
  _01001110,
  _01001111,
  _01010000,
  _01010001,
  _01010010,
  _01010011,
  _01010100,
  _01010101,
  _01010110,
  _01010111,
  _01011000,
  _01011001,
  _01011010,
  _01011011,
  _01011100,
  _01011101,
  _01011110,
  _01011111,
  _01100000,
  _01100001,
  _01100010,
  _01100011,
  _01100100,
  _01100101,
  _01100110,
  _01100111,
  _01101000,
  _01101001,
  _01101010,
  _01101011,
  _01101100,
  _01101101,
  _01101110,
  _01101111,
  _01110000,
  _01110001,
  _01110010,
  _01110011,
  _01110100,
  _01110101,
  _01110110,
  _01110111,
  _01111000,
  _01111001,
  _01111010,
  _01111011,
  _01111100,
  _01111101,
  _01111110,
  _01111111,
  _10000000,
  _10000001,
  _10000010,
  _10000011,
  _10000100,
  _10000101,
  _10000110,
  _10000111,
  _10001000,
  _10001001,
  _10001010,
  _10001011,
  _10001100,
  _10001101,
  _10001110,
  _10001111,
  _10010000,
  _10010001,
  _10010010,
  _10010011,
  _10010100,
  _10010101,
  _10010110,
  _10010111,
  _10011000,
  _10011001,
  _10011010,
  _10011011,
  _10011100,
  _10011101,
  _10011110,
  _10011111,
  _10100000,
  _10100001,
  _10100010,
  _10100011,
  _10100100,
  _10100101,
  _10100110,
  _10100111,
  _10101000,
  _10101001,
  _10101010,
  _10101011,
  _10101100,
  _10101101,
  _10101110,
  _10101111,
  _10110000,
  _10110001,
  _10110010,
  _10110011,
  _10110100,
  _10110101,
  _10110110,
  _10110111,
  _10111000,
  _10111001,
  _10111010,
  _10111011,
  _10111100,
  _10111101,
  _10111110,
  _10111111,
  _11000000,
  _11000001,
  _11000010,
  _11000011,
  _11000100,
  _11000101,
  _11000110,
  _11000111,
  _11001000,
  _11001001,
  _11001010,
  _11001011,
  _11001100,
  _11001101,
  _11001110,
  _11001111,
  _11010000,
  _11010001,
  _11010010,
  _11010011,
  _11010100,
  _11010101,
  _11010110,
  _11010111,
  _11011000,
  _11011001,
  _11011010,
  _11011011,
  _11011100,
  _11011101,
  _11011110,
  _11011111,
  _11100000,
  _11100001,
  _11100010,
  _11100011,
  _11100100,
  _11100101,
  _11100110,
  _11100111,
  _11101000,
  _11101001,
  _11101010,
  _11101011,
  _11101100,
  _11101101,
  _11101110,
  _11101111,
  _11110000,
  _11110001,
  _11110010,
  _11110011,
  _11110100,
  _11110101,
  _11110110,
  _11110111,
  _11111000,
  _11111001,
  _11111010,
  _11111011,
  _11111100,
  _11111101,
  _11111110,
  _11111111,
}  BYTE_ENABLES;

//
// PFCT Variations supported in the CAPSULEINIT Sequences.
//
typedef enum {
  _____ = 0,
  ____T,
  ___C_,
  ___CT, 
  __F__,
  __F_T,
  __FC_,
  __FCT,
  _P___,
  _P__T,
  _P_C_,
  _P_CT,
  _PF__,
  _PF_T,
  _PFC_,
  _PFCT,
  D____,
  D___T,
  D__C_,
  D__CT, 
  D_F__,
  D_F_T,
  D_FC_,
  D_FCT,
  DP___,
  DP__T,
  DP_C_,
  DP_CT,
  DPF__,
  DPF_T,
  DPFC_,
  DPFCT,
} DPFCT_TYPES;


typedef enum {
  SidebandPort,
  AddressOffset
} INSTANCE_OFFSET_TYPE;

//
// The GetSet functions support a level parameter to select the appropriate DDRIO interface in the memory hierarchy
//
/*
typedef enum {
  MMRC_VMSE_LEVEL,
  MMRC_DDR_LEVEL,
  MMRC_LRBUF_LEVEL,
} GSM_LT;
*/

typedef enum {
  CACHE,
  NO_CACHE
} CacheType;

typedef enum {
  ChNone  = 0xFE,
  ChAll   = 0xFF,
  ChAllP  = 0xE0
} CHANNEL_SELECTION;


typedef enum {
  SmbusReadByte,
  SmbusWriteByte,
  SmbusReadBlock,
  SmbusWriteBlock
} SMBUS_CMD_TYPES;

//
// PATTERN MODES
//
#define PATTERN_VICAGG        0
#define LFSR_VICAGG           1
#define LFSR                  2

//
// Structures used to display registers.
//
typedef struct {
  char  *RegisterName;
  char  *RegisterDescription;
  UINT32 Offset;
  UINT8  NumFields;
  UINTX  Mask;
  UINTX  DefaultValue;
} RegisterInformation;

typedef struct {
  UINT8  StartBit;
  UINT8  EndBit;
  UINTX  DefaultFieldValue;
  char  *AccessType;
  char  *FieldName;
  char  *Description;
} RegisterField;

typedef struct {
  RegisterInformation *RegInfo;
  RegisterField       *RegFields;
} Register;

typedef struct {
  UINT16  location;
  UINT8   cachedIndex;
} DELAY_ELEMENT;

typedef struct {
  UINT32      RegisterOffset;
  UINT8       Lsb;
  UINT8       Msb;
  UINT8       ValueOffset;
  UINT8       Unit;
  UINT8       Name[20];
} DUNIT_PARAMETERS_STRUCT;

typedef enum {
  NoUnit,
  Clk,
  Mhz,
  TccdUnit,
  PwDDlyUnit,
  MaxUnitTypes,
} UNIT_TYPE;

typedef struct {
  UINT32  Pfct;
  UINT8   LengthVariation;
} PFCT_VARIATIONS;

typedef struct  {
  PFCT_VARIATIONS PFCTVariations[MaxPfct];
} LOCAL_PARAMS;

typedef struct {
  UINT32  Offset;           // Register Offset relative to the base address.
  UINTX   Mask;             // Starting bit within the register.
  UINT8   ShiftBit;         // Number of bits to shift to get to the register field in question
} REGISTER_ACCESS;

typedef union {
  UINT32      Data;
  // 
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
  //
#ifndef ASM_INC
  struct {
    UINT32    Offset   : 12;  // PCI register offset
    UINT32    Func     : 3;   // PCI Function
    UINT32    Device   : 5;   // PCI Device
    UINT32    Bus      : 8;   // PCI Bus
    UINT32    Base     : 4;   // PCI base address
  } Bits;
#endif
} PCI_REG;

typedef struct {
  UINT8  TotalInstances;
  UINT8  Instances;
  UINT8  InstanceStartIndex;
  UINT8  ReadOperation;
  UINT8  WriteOperation;
  UINT8  AccessMethod;
  UINT8  AccessBase;
  UINT8  SimulationSupported;
  UINT32 StringIndex;
} INSTANCE_PORT_MAP;

typedef struct {
  UINT16 Port;
  UINT16 Offset;
} INSTANCE_PORT_OFFSET;

typedef struct {
  UINT8  OneX[MAX_STROBES];
  UINT8  TwoX[MAX_STROBES];
  UINT8  Pi[MAX_STROBES];
  UINT8  CC0[MAX_STROBES];
  UINT8  CC1[MAX_STROBES];
  UINT8  Min[MAX_STROBES];
  UINT8  Vref[MAX_STROBES];
  UINT16 TotalDelay[MAX_STROBES];
} ELEMENTS;

typedef struct {
  UINT16    Values[MAX_NUM_ALGOS + 1][MAX_RANKS][MAX_STROBES];
} TRAINING_SETTING;

typedef struct {
  UINT8   *LinearToPhysicalTable;   // Pointer to an array that contains the physical values for external vref
  UINT8   SizeOfLtoPTable;          // Size of the LinearToPhysicalTable
  UINT8   *PhysicalToLinearTable;   // Pointer to an array that contains the linear values for external vref
  UINT8   SizeOfPtoLTable;          // Size of the PhysicalToLinearTable
} PHY_VREF;

typedef struct {
  UINT8 ManuIDlo;
  UINT8 ManuIDhi;
  UINT8 ManuLoc;
  UINT8 ManuDateLO;
  UINT8 ManuDateHI;
  UINT8 SerialNumber1;
  UINT8 SerialNumber2;
  UINT8 SerialNumber3;
  UINT8 SerialNumber4;
  UINT8 DimmCount;
} FASTBOOTDATA;

#if FAULTY_PART_TRACKING
typedef struct {
  UINT32   Byte0to3;                  // On the fly tracking of the minimum bit failure used by all trainings 
  UINT32   Byte4to7;                  // On the fly tracking of the minimum failure bit
  UINT8    ByteEcc;                   // On the fly tracking of the minimum failure bit
  UINT16   TrainingFail;              // Training that failed due to faulty part
  UINT32   DataBitFailCumulativeLow;  // Field to cumulate bit failures found as training progresses for data bits
  UINT32   DataBitFailCumulativeHigh; // Field to cumulate bit failures found as training progresses for data bits
  UINT8    EccBitFailCumulative;      // Field to cumulate bit failures found as training progresses for ECC byte
  UINT8    ErrorType;                 // 0- No error, 1-Correctable, 2-Uncorrectable
} FAULTY_PART;
#endif

typedef struct {
  BOOLEAN                   Enabled;
  UINT32                    TotalMem;
  UINT8                     DimmCount;
  UINT8                     DimmPresent[MAX_DIMMS];
  UINT32                    SlotMem[MAX_DIMMS];
  UINT8                     D_Ranks[MAX_DIMMS];
  UINT8                     D_DataWidth[MAX_DIMMS];
  UINT8                     D_Banks[MAX_DIMMS];
  UINT8                     D_BusWidth[MAX_DIMMS];
  UINT8                     D_Size[MAX_DIMMS];
  UINT8                     D_Type[MAX_DIMMS];
  UINT8                     Ddr3S[MAX_DIMMS];
  UINT8                     Ddr3L[MAX_DIMMS];
  UINT8                     Ddr3U[MAX_DIMMS];
  BOOLEAN                   SamsungDram[MAX_DIMMS];
  UINT8                     DimmFrequency[MAX_DIMMS];
  UINT8                     RankPresent[MAX_RANKS];
  BOOLEAN                   RankEnabled[MAX_RANKS];
  BOOLEAN                   EccEnabled;
  UINT32                    EccStrobes;
  UINT8                     MaxDq[MAX_RANKS];
  FASTBOOTDATA              FastBootData[MAX_DIMMS];
  UINT8                     DramType;
  UINT8                     CurrentPlatform;
  UINT8                     CurrentFrequency;
  UINT8                     CurrentConfiguration;
  UINT8                     CurrentDdrType;
  UINT16                    ScramblerSeed;
  UINT16                    TimingData[MaxTimingData][MaxDvfsFreqs];
  UINT16                    TimingDataOverrides[MaxTimingData][MaxDvfsFreqs];
  TRAINING_SETTING          Trained_Value;
  UINT32                    CachedValues[MAX_NUM_CACHE_ELEMENTS + 1][MAX_RANKS][MAX_STROBES];
  UINT8                     DimmVrefWrite[MAX_RANKS][MAX_STROBES];
  INT16                     RMT_Data[MAX_RANKS][MaxRMTData];
  UINT8                     VrefSmbAddress;
#if FAULTY_PART_TRACKING
  FAULTY_PART               FaultyPart[MAX_RANKS];
#endif
} CHANNEL;

#define MRC_PARAMS_SAVE_RESTORE_DEF                   \
  UINT8      MrcParamsValidFlag;                      \
  UINT8      BootMode;                                \
  UINT8      CurrentFrequency;                        \
  UINT8      CoreFreq;                                \
  UINT8      TotalDimm;                               \
  UINT32     DimmVoltageFlags;                        \
  UINT32     SystemMemorySize;                        \
  UINT16     DVFSFreqIndex;                           \
  CHANNEL    Channel[MAX_CHANNELS];
//
// Do not add or change this structure.
//
typedef struct {
  MRC_PARAMS_SAVE_RESTORE_DEF
} MRC_PARAMS_SAVE_RESTORE;

typedef struct {
  UINT16    MmioSize;         // 1 MB Aligned
  UINT16    TsegSize;         // 1 MB Aligned
  PHY_VREF  PhyVrefTable;
  UINT8     SpdAddressTable[MAX_CHANNELS][MAX_DIMMS];
} MRC_OEM_FRAME;

typedef struct {
  UINT8     RcvEn;
  UINT8     RxDqs;
  UINT8     TxDqs;
  UINT8     TxDq;
} R2R_SWITCHING_PARAMETERS;

typedef struct {
  UINT8   Multiplier;
  UINT32  Offset;                 // Used when adjust is linear
  UINT32  IncrementalMultiplier;  // It specifies the increment every 1000 activates (For non linear)
  UINT32  IncrementalOffset;      // It specifies the offset every 1000 activates (For non linear)
} DELAY_PASS_GATE;

typedef struct {
  BOOLEAN EnableTest;                 // Enable/Disable the Pass Gate Test
  BOOLEAN EnablePeriodicSelfRefresh;  // Enables periodic Self Refresh during the test
  UINT8   Direction;                  // '0': Lowest -> Highest  '1': Highest -> Lowest
  UINT32  RepetitionCountRange;       // The number of times a Row will be tested
  UINT32  IterationOnRow;             // How many times the test over the same row will be repeated
  UINT8   SwizzleMode;                // 0: Automatic 1: Force Samsung
  UINT8   Pattern;                    // Pattern used for the Agressor
  UINT8   TargetPattern;              // Pattern used for the Victim
  BOOLEAN EnablePartialTest;          // Enable the execution of the Pass Gate in a memory segment
  UINT16  PartialRowBankMin;          //
  UINT16  PartialRowBankMax;          //
  BOOLEAN EnableMontecarloSearch;     // MonteCarlo Search Algorithm
  UINT8   MontecarloMaxFailures;      // Max number of error prior to go to next Repetition
  UINT32  MontecarloStartRepetition;  // Max repetition number for Montecarlo Algorithm
  UINT32  MontecarloDecrement;        // Min repetition number for Montecarlo Algorithm
  UINT8   SpeedSelect;                // 0: Automatic 1: Force 1x Algorithm
  BOOLEAN RankEnable[MAX_CHANNELS][MAX_RANKS];  // Specifies which Ranks will be tested
} PASS_GATE_PARAMETERS;

typedef struct {
  UINT8     SetupType;
  UINT8     CmdSequence;
  UINT32    Pattern;
  } CPGC_OPTIONS;

typedef struct {
  MRC_PARAMS_SAVE_RESTORE_DEF
  UINT8                     PostCode;
  MMRC_STATUS               ErrorCode;
  BOOLEAN                   RestorePath;
  UINT8                     DigitalDll;
  UINT8                     MrcDebugMsgLevel;
  UINT8                     MrcRMTSupport;
  UINT32                    MrcRmtEnableSignal;
  UINT8                     MrcTrainingCpgcExpLoopCnt;
  UINT8                     MrcRmtCpgcExpLoopCnt;
  UINT8                     MrcCPGCNumBursts;
#if FAULTY_PART_TRACKING
  BOOLEAN                   FaultyPartTracking;           
  BOOLEAN                   FaultyPartContinueOnCorrectable;
#endif
  UINT16                    SmbusBar;
  UINT8                     Simics;
  R2R_SWITCHING_PARAMETERS  Rk2RkEn;
  UINT8                     InputFineDdrVoltage;
#if SIM
  FILE                      *LogFile;
#endif
  MRC_OEM_FRAME             OemMrcData;
  UINT8                     CpuStepping;
  UINT16                    CpuType;
  UINT32                    MemoryCeiling;
  UINT32                    EcBase;
  UINT16                    GpioBar;
  UINT32                    PmcBar;
  UINT32                    HpetHptcSave;
  UINT8                     DdrFreqCap;
  UINT8                     DrbSecondTimePath;
  UINT8                     PowerOn;
  UINT8                     PpoPlatform;
  UINT8                     FastBootEnable;
  UINT8                     DynSR;
  UINT8                     PatrolScrubEnable;
  UINT8                     PatrolScrubPeriod;
  UINT32                    PatrolScrubAddr;
  UINT8                     DemandScrub;
  UINT8                     OpenPolicyTimer;
  UINT8                     EnableScrambler;
  UINT8                     EnableSlowPowerDownExit;
#if ADR_SUPPORT
  UINT8                     ADRC2F;       // Setup option
  UINT8                     ADRCh1;       // Enable the ADR in channel 1 (BWD only has one)
  UINT16                    MinType0;     // For table E820
#endif // ADR_SUPPORT
  UINT8                     MemoryTestLoop;
  UINT32                    LoopCount;
  UINT8                     HaltSystemOnMemTestError;
  UINT8                     ResetLoop;
  UINT8                     HaltCpgcEngineOnError;
  UINT8                     MemoryThermalEnable;
  UINT8                     OutOfOrderAgingThreshold;
  UINT8                     OutOfOrderDisable;
  UINT8                     NewRequestBypassDisable;
  UINT8                     VrefOverrideEnable;
  UINT8                     VrefOverrideValue;
  UINT8                     InputDdrVoltage;


  BOOLEAN                   ZqcEnable;
  UINT8                     DdrChannels;
  UINT8                     SkipFWL;
  UINT8                     SkipCWL;
  UINT8                     SkipRcvEn;
  UINT8                     SkipBasicRxDqDqs;
  UINT8                     SkipAdvancedRead;
  UINT8                     SkipAdvancedWrite;
  BOOLEAN                   EnableCkeByRank;
  UINT16                    DimmConfig;
  UINT32                    BCtrl;
  UINT32                    BDebug1;
  UINT32                    BwFlush;
  UINT8                     AbSegToDram;
  UINT8                     ESegToDram;
  UINT8                     FSegToDram;
  UINT32                    SupportedCasLat;
  UINT8                     SignalType;
  BOOLEAN                   EnableParallelTraining;
  BOOLEAN                   ExecuteThisRoutineInParallel;
  POWER_KNOB_VALUES         SettingsPowerKnobs;
  BOOLEAN                   DebugRmtPowerTraining;
  CPGC_OPTIONS              CpgcOptions;
#ifdef DDR4_SUPPORT
  UINT8                     CommandAddressParity;
#endif
#if PASS_GATE_TEST == 1
  PASS_GATE_PARAMETERS      PassGateParameters;
#endif
  UINT8                     DVFS_FreqA;
  UINT8                     DVFS_FreqB;
  UINT8                     DVFS_Enable;
  // GROWING LIST OF INPUT PARAMS NEEDED FOR MMRC
  UINTX                     DynamicVars[MAX_CHANNELS][DYNVAR_MAX];
} MMRC_DATA;

typedef union {
  UINT32        Data;
  // Remove bit field definitions from ASM include file due to MASM/H2INC limitations
#ifndef ASM_INC
  struct {
    UINT32    command         : 3;            // Command: 000-MRS,001-Refresh,010-Pre-charge,011-Activate,110  ZQ Calibration,111-NOP 
    UINT32    bankAddress     : 3;            // Bank Address (BA[2:0]) 
    UINT32    multAddress     : 16;           // Multiplexed Address (MA[15:0]) 
    UINT32    rankSelect      : 2;            // Rank Select 
    UINT32    reserved3       : 8;
  } Bits;
#endif
} DramInitMisc;                               // Miscellaneous DDRx Initialization Command 

#pragma pack()

#define NO_PRINT                0 // Set to 0xFF for MAX DEBUG PRINT

#define ANALOGDLL               0
#define DIGITALDLL              1

#define LPDDR3_WL_ENABLE        BIT19
#define DDR3_WL_ENABLE          BIT13

#define JEDEC_PRECHARGEALL      0x01
#define JEDEC_MRS               0x02
#define JEDEC_REFRESH           0x03

#define FORCEODT_LOW            0x00
#define FORCEODT_HIGH           0x01
#define FORCEODT_RELEASE        0x02

#define TASK_FUNCTION_DESC_DONE 0, 0, ((MMRC_STATUS(*)(PSYSHOST, UINT16, UINT16, UINT8)) 0), 0, 0
#define DETREGLIST_NULL         ((DETAILED_REGISTER_STRING *) 0xFFFFFFFF)
#define REGLIST_NULL            ((REGISTER_STRING *)    0xFFFFFFFF)
#define PHYINIT_LIST_DONE       ((UINT8 *) 0xFFFFFFFF), REGLIST_NULL, DETREGLIST_NULL

typedef struct {
  UINT8   PostCode;
  UINT8   BootMode;
  MMRC_STATUS  (*Function) (PSYSHOST, UINT16, UINT16, UINT8);
  UINT16  CapsuleStartIndex;
  UINT8   StringIndex;
  UINT8   Channel;
} TASK_DESCRIPTOR;

typedef struct {
  MMRC_STATUS  (*Function) (MMRC_DATA *MrcData, UINT8 Socket, UINT8 Channel, UINT8 Dimm, UINT8 Rank, UINT8 Strobe, UINT8 Bit, UINT8 IoLevel, UINT8 Cmd, UINT32 *Value);
} EXTERNAL_GETSET_FUNCTION;

//
// When specifying a unique location, the elements required are 
// offset/starting/ending bits.
//
typedef struct {
  UINT16 Offset;              // Byte offset for the specific rank. 
  UINT8 StartingBit;          // Starting bit position for the specific rank.
  UINT8 EndingBit;            // Ending bit position for the specific rank.
} REG_LOC;
// 
// Elements is the main structure for identifying the location for an individual register
// assignment with the Get/Set API.
//
typedef struct {
  UINT8   Box_Type;            // Box type.
  REG_LOC RegisterLocation[MAX_RANKS];
  UINT8   CacheIndex;          // Index into the cache array where this element is stored.
  BOOLEAN BitLevelControl;     // True when this has bit-level granularity like per-bit deskew.
  INT16   SPMByteOffset;       // Strobe-to-Strobe Per Module Byte offset.
  INT8    SPMBitOffset;        // Strobe-to-Strobe Per Module Bit offset.
  UINT32  MaxLimit;            // Maximum absolute value allowed
  UINT32  WaitTime;            // Time in nanoseconds required for new setting to take effect
  UINT8   Minus1Index;         // The index of the minus 1 select associated with this delay
  UINT8   VrefIndex;           // The index of the vref associated with this delay.
} GET_SET_ELEMENTS;
// 
// Information about the signals. Things like number of signal groups, name of signal, etc.
//
typedef struct {
  UINT8   NumSignalGroups;     // Number of signal groups for this signal type.
  char   *Name;                // Name of the signal
} SIGNAL_INFO;

typedef struct {
  UINT8        Signal;
  UINT8        LowSide;
  UINT8        HighSide;
  UINT8        NumElements;
  MMRC_STATUS   (*Function) (MMRC_DATA *, UINT8, UINT8,  UINT8, UINT8, UINT8 PassFail[MAX_CHANNELS][MAX_KNOBS][MAX_STROBES][2]);
} RMT_SIGNAL;
//
// ClockCrossing structure provides when the specific clock crossing will change
// based on the linear PI values.  The range is specified in (%) of the PI range
// since the PI range is dynamic based on analog/digitial dll.
//
typedef struct {
  UINT16      MinPercent;       // Minimum PI value for InValue to be programmed.
  UINT16      MaxPercent;       // Maximum PI value for InValue to be programmed.
  UINT16      InValue;          // In Range Value.
  UINT16      OutValue;         // Out of Range Value.
} CLOCK_CROSSINGS;


//
// Structure for the PHY init registers. This contains information on how to find registers for a given
// bytelane, module, rank, etc. Also indicates whether we should write a register to all enabled channels
// or just to a specified channel.
//
typedef struct {
  UINT8  RankLoop         :1;   // Loop on rank.
  UINT8  StrobeLoop       :1;   // Loop on strobe.
  UINT8  Offset16         :1;   // When set, the offset field is 2-bytes long, otherwise 1-byte long.
  UINT8  CondExec         :1;   // When set, Conditional Execution.
  UINT8  MaskPresent      :1;   // When set, mask is present in the Assignment.
  UINT8  NeedSecondByte   :1;   // When set, REGISTER_LOCATION_ATTRIBUTES2 is present.
  UINT8  UseBB2           :1;   // If the boxtype is above 15 or the value being written is greater than 32 bits 
                                //  then we need to use the BB2 macro to allow 8 bits for boxtype and 8 for byte enables.
  UINT8  Done             :1;   // When set, this is the final entry for this task.
} REGISTER_LOCATION_ATTRIBUTES;
//
// Optional Byte 2 of register attributes. Only used if we need bits in this byte set to 1.
//
typedef struct {
  UINT8  StringPresent    :1;   // There is a string that needs to be output associated with this function entry.
  UINT8  InstanceDependent:1;   // When set, only specific instance will be written to.
  UINT8  ChannelDependent :1;   // When set, only specific channel will be written.
  UINT8  Reserved         :5;   // Reserved
} REGISTER_LOCATION_ATTRIBUTES2;

// 
// The structure definition for the floorplan which is given a channel and strobelane, to provide a physical
// channel and strobelane.
//
typedef struct {
  UINT8 Channel;
  UINT8 Strobelane;
} FLOORPLAN;


typedef struct {
  INT8    bitoffset;
  INT16   byteoffset;
} LP_TABLE;

// 
// Layout for the Register Display Strings
//
typedef struct {
  UINT8 string[16]; // Maximum of 16 characters for the name of the register.
  UINT8 start;      // Index into the detailed register (fields Names) where the first field is listed.
  UINT8 number;     // Number of fields in the detailed register array.
} REGISTER_STRING;

// 
// Layout for the Detailed Register Display Strings (Fields).
//
typedef struct {
  UINT8 string[13]; // Maximum of 13 character for the register field name.
  UINT8 endBit;     // End bit for the field.
  UINT8 startBit;   // Starting bit for the field.
} DETAILED_REGISTER_STRING;

typedef struct {
  CONST UINT8                       *regAssign;
  CONST REGISTER_STRING             *regString;
  CONST DETAILED_REGISTER_STRING    *detRegString;
} PHYINIT_LIST;

typedef struct {
  UINT16   halfClk;
  UINT16   minVal1;
  UINT16   maxVal1;
  UINT16   minVal2;
  UINT16   maxVal2;
} DIGITAL_DLL_LIST;

//
// Used to pass data between MRC and MMRC. Not used at the moment
// so setting the variable to Reserved.
//
typedef struct {
  BOOLEAN Enable;       // Set to TRUE if you want the feature enabled. FALSE if you want it disabled.
  UINT8   Channel;      // Optional channel on which to operate. If not populated, all channels will be handled.
  BOOLEAN ChannelValid; // Set to TRUE if Channel contains a particular channel or channels on which to operate.
  MMRC_STATUS  (*Function) (MMRC_DATA *, UINT8);
} HANDSHAKE_PARAMETERS;

#ifndef MRC_DATA_REQUIRED_FROM_OUTSIDE
typedef MMRC_STATUS (*MRC_TASK_FUNCTION) (MMRC_DATA  *MrcData, UINT8 Channel);
#endif
//
// Prototypes for each Power Nob function
//
typedef MMRC_STATUS (*HOOK_SET_VALUE) (MMRC_DATA *MrcData, UINT8 Channel, UINT16 Index);
typedef MMRC_STATUS (*TRAINING_STEP)  (MMRC_DATA *MrcData, UINT16 CapsuleStartIndex, UINT16 StringIndex, UINT8 Channel);

typedef enum {
  MrcErrOk = 0,
  MrcErrDebug,
  MrcErrDimmsFailed,
  MrcErrInstallPeiMemory,
  MrcErrInvalidChannelNumber,
  MrcErrInvalidMemorySize,
  MrcErrIllegalVoltageConfig,
  MrcErrMailbox,
  MrcErrMemTest,
  MrcErrWarmResetError,
  MrcErrGetSpdData, 
  MrcErrNoDimmsPresent, 
  MrcErrInvalidRankNumber,
  MrcErrInvalidDimmNumber, 
  MrcErrConfigureMemory, 
  MrcErrNullPointer,
  MrcErrStatusLimit,
  MrcErrFaultyPartCorrectable,
  MrcErrFaultyPartUncorrectable,
  MrcErrInvalidFrequency,
  MrcErrPlatformSettings,
  MrcErrInvalidLinearValue,
  MrcErrInvalidPhysicalValue,
  MrcErrRankStrobeClockDelta,
  MrcErrCenter2DAccMarginXZero,
  MrcErrCenter2DAccMarginYZero,
  MrcErrSearchBeyondHalfClk,
  MrcErrInvalidPowerKnob,
  MrcErrChannelsDisabled,
  MrcErrMsgMaxError
} OEM_MRC_ERROR;

#endif
