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
  Copyright 2006 - 2017 Intel Corporation All Rights Reserved.

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
  MemPlatformCommon.h

@brief
  This file contains memory detection and initialization for
  IMC and DDR3 modules compliant with JEDEC specification.

**/

#ifndef  _mem_platformcommon_h
#define  _mem_platformcommon_h


//
// InitMem.c and InitMemChip.c
//
#define NO_POST_CODE               0xFF
#define NOT_SUP                    0xFF

//
// Major Checkpoint progress indicators written to debug port for HBM
//
#define  STS_HBM_MRC               0x70
  #define SUB_HBM_INIT                 0x01
  #define SUB_HBM_DISCOVERY            0x02
  #define SUB_HBM_POR                  0x03
  #define SUB_HBM_DISPLAY              0x04
  #define SUB_HBM_BASIC_FSM            0x05
  #define SUB_HBM_RX_DQ_DELAY          0x06
  #define SUB_HBM_ADV_AWORD            0x07
  #define SUB_HBM_TX_DS_TCO            0x08
  #define SUB_HBM_RX_VREF_             0x09
  #define SUB_HBM_ADV_RX_DWORD         0x0A
  #define SUB_HBM_ADV_TX_DWORD         0x0B
  #define SUB_HBM_RNDTRIP_LAT          0x0C
  #define SUB_HBM_RX_DELAY_OPT         0x0D
  #define SUB_HBM_THROTTLING           0x0E
  #define SUB_HBM_MEM_TEST             0x0F
  #define SUB_HBM_MEM_INIT             0x10
  #define SUB_HBM_RAS                  0x11
#define  STS_HBM_DEBUG             0x71
#define  STS_HBM_INTERNAL          0x72


//
// Major Checkpoint progress indicators written to debug port
//
#define  STS_DIMM_DETECT            0xB0
  #define SUB_DIMM_DETECT               0x00
  #define SUB_DISPATCH_SLAVES           0x01
  #define SUB_INIT_SMB                  0x02
  #define SUB_GET_SLAVE_DATA            0x03
  #define SUB_MAILBOX_READY             0x04
#define  STS_CLOCK_INIT             0xB1
  #define SUB_SEND_FREQ                 0x01
  #define SUB_SEND_STATUS               0x02
  #define SUB_SET_FREQ                  0x03
  #define SUB_SET_CLK_VDD               0x04
  #define SUB_XMP_INIT                  0x05
#define  STS_SPD_DATA               0xB2
  #define SUB_SEND_DATA                 0x01
#define  STS_GLOBAL_EARLY           0xB3
  #define SUB_PROG_TIMINGS              0x01
#define  STS_RANK_DETECT            0xB4
  #define SUB_SEND_DATA_SPD             0x01
    #define DATA_TYPE_VAR                 0x01
    #define DATA_TYPE_NVRAM_COMMON        0x02
    #define DATA_TYPE_NVRAM_DATA          0x03
#define  STS_CHANNEL_EARLY          0xB5
#define  STS_DDRIO_INIT             0xB6
  #define EARLY_INIT                      0x01
  #define LATE_INIT                       0x02
  #define INIT_VCCU                       0x03
  #define JEDEC_RESET_INIT                0x04
  #define SUB_DDRT_IO_INIT                0x05
  #define SENSE_AMP_CAL                   0x06
#define  STS_CHANNEL_TRAINING       0xB7
  // These minor checkpoints are defined in MemHost.h
  // #define RECEIVE_ENABLE_BASIC  0
  // #define WRITE_LEVELING_BASIC  1
  // #define WR_FLY_BY             2
  #define SUB_WR_FLY_BY_TRAINING        0x01
  #define SUB_WR_FLY_BY_EVALUATE        0x02
  // #define RX_DQ_DQS_BASIC       3
  // #define TX_DQ_BASIC           4
  // #define LATE_CMD_CLK          5
  // #define TRAIN_RD_DQS          6
  // #define TRAIN_WR_DQS          7
  // #define TRAIN_RD_VREF         8
  // #define TRAIN_WR_VREF         9
  // #define CROSSOVER_CALIBRATION 0x0A
  // #define SENSE_AMP             0x0B
  // #define EARLY_CMD_CLK         0x0C
  #define SUB_ENABLE_PARITY             0x03
  // #define PER_BIT_DESKEW_RX     0x0D
  // #define PER_BIT_DESKEW_TX     0x0E
  // #define RX_VREF_TRAINING      0x0F
  #define  STS_RMT                  0x10
    #define SUB_LATE_CONFIG             0x01
    #define SUB_INIT_THROTTLING         0x02
  #define  STS_MINOR_END_TRAINING   0x11
  #define  STS_MINOR_NORMAL_MODE    0x12
    #define SUB_DATA_SYNC               0x01
#define  STS_INIT_THROTTLING        0xB8
#define  STS_MEMBIST                0xB9
  #define SUB_MT_STEP1               0x01
  #define SUB_MT_STEP2               0x02
  #define SUB_MT_STEP3               0x03
  #define SUB_MT_STEP4               0x04
  #define SUB_MT_STEP5               0x05
  #define SUB_MT_STEP6               0x06
  #define SUB_MT_STEP7               0x07
  #define SUB_MT_STEP8               0x08
  #define SUB_MT_STEP9               0x09
  #define SUB_MT_STEP10              0x0A
#define  STS_MEMINIT                0xBA
#define  STS_DDR_MEMMAP             0xBB
  #define SUB_SAD_INTERLEAVE            0x01
  #define SUB_TAD_INTERLEAVE            0x02
  #define SUB_SAD_NONINTER              0x03
  #define SUB_TAD_NONINTER              0x04
  #define SUB_RANK_INTER                0x05
  #define SUB_WRITE_SAD                 0x06
  #define SUB_WRITE_TAD                 0x07
  #define SUB_WRITE_RIR                 0x08
  #define SUB_WRITE_SPARE_RANK 10
#define  STS_RAS_CONFIG             0xBC
  #define SUB_MIRROR_MODE               0x01
  #define SUB_SPARE_MODE                0x02
  #define SUB_DEVICE_TAG                0x03
  #define SUB_ERR_THRESH                0x04
  #define SUB_LOCKSTEP_MODE             0x05
  #define SUB_ADDDC_MIRROR              0x06
#define STS_GET_MARGINS             0xBD
#ifdef SSA_FLAG
#define  SSA_API_INIT               0xBE
  #define  SSA_API_INIT_START           0x01
  #define  SSA_API_INIT_EV_LOADER       0x02 //SSA API has been initialized; EV loader to be called
#endif //SSA_FLAG
#define  STS_MRC_DONE               0xBF
// The following do not appear as check points but are used to set currentMemPhase for performance tracking.
#define  STS_JEDEC_INIT             0xC0

//
// Major Fatal error codes written to port 80h
//
#define  ERR_NO_MEMORY                     0xE8
 #define  ERR_NO_MEMORY_MINOR_NO_MEMORY                0x01
 #define  ERR_NO_MEMORY_MINOR_ALL_CH_DISABLED          0x02
 #define  ERR_NO_MEMORY_MINOR_ALL_CH_DISABLED_MIXED    0x03
#define  ERR_LT_LOCK                       0xE9
#define  ERR_DDR_INIT                      0xEA
 #define  ERR_RD_DQ_DQS                                0x01
 #define  ERR_RC_EN                                    0x02
 #define  ERR_WR_LEVEL                                 0x03
 #define  ERR_WR_DQ_DQS                                0x04
#define  ERR_MEM_TEST                      0xEB
 #define  ERR_MEM_TEST_MINOR_SOFTWARE                  0x01
 #define  ERR_MEM_TEST_MINOR_HARDWARE                  0x02
 #define  ERR_MEM_TEST_MINOR_LOCKSTEP_MODE             0x03
#define  ERR_VENDOR_SPECIFIC               0xEC
 #define ERR_DIMM_PLL_LOCK_ERROR                       0x01
#define  ERR_DIMM_COMPAT                   0xED
 #define  ERR_MIXED_MEM_TYPE                           0x01
 #define  ERR_INVALID_POP                              0x02
 #define  ERR_INVALID_POP_MINOR_QR_AND_3RD_SLOT        0x03
 #define  ERR_INVALID_POP_MINOR_UDIMM_AND_3RD_SLOT     0x04
 #define  ERR_INVALID_POP_MINOR_UNSUPPORTED_VOLTAGE    0x05
 #define  ERR_DDR3_DDR4_MIXED                          0x06
 #define  ERR_MIXED_SPD_TYPE                           0x07
 #define  ERR_MISMATCH_DIMM_TYPE                       0x08
 #define  ERR_INVALID_DDR4_SPD_CONTENT                 0x09
 #define  ERR_SMBUS_READ_FAILURE                       0x0A
 #define  ERR_MIXED_MEM_AEP_AND_UDIMM                  0x0B
 #define  ERR_NVMDIMM_NOT_SUPPORTED                    0x0C
 #define  ERR_CPU_CAP_NVMDIMM_NOT_SUPPORTED             0x0D
#define  ERR_MRC_COMPATIBILITY             0XEE
 #define  ERR_MRC_DIR_NONECC                           0X01
#define  ERR_MRC_STRUCT                    0xEF
 #define   ERR_INVALID_BOOT_MODE                       0x01
 #define   ERR_INVALID_SUB_BOOT_MODE                   0x02
 #define   ERR_INVALID_HOST_ADDR                       0x03
 #define   ERR_ARRAY_OUT_OF_BOUNDS                     0x04
 #define   ERR_IMC_NUMBER_EXCEEDED                     0x05
 #define   ERR_ODT_STRUCT                              0x06
#define ERR_SET_VDD                        0xF0
 #define ERR_UNKNOWN_VR_MODE                           0x01
#define ERR_IOT_MEM_BUFFER                 0xF1
#define ERR_RC_INTERNAL                    0xF2
 #define ERR_RC_INTERNAL_HBM                           0x01
#define ERR_INVALID_REG_ACCESS             0xF3
 #define ERR_INVALID_WRITE_REG_BDF                     0x01
 #define ERR_INVALID_WRITE_REG_OFFSET                  0x02
 #define ERR_INVALID_READ_REG_BDF                      0x03
 #define ERR_INVALID_READ_REG_OFFSET                   0x04
 #define ERR_INVALID_WRITE_REG_SIZE                    0x05
 #define ERR_INVALID_READ_REG_SIZE                     0x06
 #define ERR_UNKNOWN_REG_TYPE                          0x07
#define ERR_SET_MC_FREQ                    0xF4
 #define ERR_UNSUPPORTED_MC_FREQ                       0x01
#define ERR_READ_MC_FREQ                   0xF5
 #define ERR_NOT_ABLE_READ_MC_FREQ                     0x01
#define ERR_DIMM_CHANNEL                   0x70
#define ERR_BIST_CHECK                     0x74
#define ERR_SMBUS                          0xF6
 #define TSOD_POLLING_ENABLED_READ                     0x01
 #define TSOD_POLLING_ENABLED_WRITE                    0x02
 #define ERR_LRDIMM_SMBUS_READ_FAILURE                 0x03

#define ERR_PCU                            0xF7
 #define PCU_NOT_RESPONDING                            0x01
#define ERR_NGN                            0xF8
 #define NGN_DRIVER_NOT_RESPONSIBLE                    0x01
 #define NGN_ARRAY_OUT_OF_BOUNDS                       0x02
 #define NGN_PMEM_CONFIG_ERROR                         0x03
 #define INTERLEAVE_EXCEED                             0x04
 #define NGN_BYTES_MISMATCH                            0x05
 #define NGN_SKU_MISMATCH                              0x06
#define ERR_INTERLEAVE_FAILURE             0xF9
 #define  ERR_RIR_RULES_EXCEEDED                       0x01

#define ERR_SKU_LIMIT                      0xFA
 #define  ERR_SKU_LIMIT_VIOLATION                      0x01

//
// Minor Warning codes
//
typedef enum {
  RC_ASSERT_MINOR_CODE_0 = 0,
  RC_ASSERT_MINOR_CODE_1,
  RC_ASSERT_MINOR_CODE_2,
  RC_ASSERT_MINOR_CODE_3,
  RC_ASSERT_MINOR_CODE_4,
  RC_ASSERT_MINOR_CODE_5,
  RC_ASSERT_MINOR_CODE_6,
  RC_ASSERT_MINOR_CODE_7,
  RC_ASSERT_MINOR_CODE_8,
  RC_ASSERT_MINOR_CODE_9,
  RC_ASSERT_MINOR_CODE_10,
  RC_ASSERT_MINOR_CODE_11,
  RC_ASSERT_MINOR_CODE_12,
  RC_ASSERT_MINOR_CODE_13,
  RC_ASSERT_MINOR_CODE_14,
  RC_ASSERT_MINOR_CODE_15,
  RC_ASSERT_MINOR_CODE_16,
  RC_ASSERT_MINOR_CODE_17,
  RC_ASSERT_MINOR_CODE_18,
  RC_ASSERT_MINOR_CODE_19,
  RC_ASSERT_MINOR_CODE_20,
  RC_ASSERT_MINOR_CODE_21,
  RC_ASSERT_MINOR_CODE_22,
  RC_ASSERT_MINOR_CODE_23,
  RC_ASSERT_MINOR_CODE_24,
  RC_ASSERT_MINOR_CODE_25,
  RC_ASSERT_MINOR_CODE_26,
  RC_ASSERT_MINOR_CODE_27,
  RC_ASSERT_MINOR_CODE_28,
  RC_ASSERT_MINOR_CODE_29,
  RC_ASSERT_MINOR_CODE_30,
  RC_ASSERT_MINOR_CODE_31,
  RC_ASSERT_MINOR_CODE_32,
  RC_ASSERT_MINOR_CODE_33,
  RC_ASSERT_MINOR_CODE_34,
  RC_ASSERT_MINOR_CODE_35,
  RC_ASSERT_MINOR_CODE_36,
  RC_ASSERT_MINOR_CODE_37,
  RC_ASSERT_MINOR_CODE_38,
  RC_ASSERT_MINOR_CODE_39,
  RC_ASSERT_MINOR_CODE_40,
  RC_ASSERT_MINOR_CODE_41,
  RC_ASSERT_MINOR_CODE_42,
  RC_ASSERT_MINOR_CODE_43,
  RC_ASSERT_MINOR_CODE_44,
  RC_ASSERT_MINOR_CODE_45,
  RC_ASSERT_MINOR_CODE_46,
  RC_ASSERT_MINOR_CODE_47,
  RC_ASSERT_MINOR_CODE_48,
  RC_ASSERT_MINOR_CODE_49,
  RC_ASSERT_MINOR_CODE_50,
  RC_ASSERT_MINOR_CODE_51,
  RC_ASSERT_MINOR_CODE_52,
  RC_ASSERT_MINOR_CODE_53,
  RC_ASSERT_MINOR_CODE_54,
  RC_ASSERT_MINOR_CODE_55,
  RC_ASSERT_MINOR_CODE_56,
  RC_ASSERT_MINOR_CODE_57,
  RC_ASSERT_MINOR_CODE_58,
  RC_ASSERT_MINOR_CODE_59,
  RC_ASSERT_MINOR_CODE_60,
  RC_ASSERT_MINOR_CODE_61,
  RC_ASSERT_MINOR_CODE_62,
  RC_ASSERT_MINOR_CODE_63,
  RC_ASSERT_MINOR_CODE_64,
  RC_ASSERT_MINOR_CODE_65,
  RC_ASSERT_MINOR_CODE_66,
  RC_ASSERT_MINOR_CODE_67,
  RC_ASSERT_MINOR_CODE_68,
  RC_ASSERT_MINOR_CODE_69,
  RC_ASSERT_MINOR_CODE_70,
  RC_ASSERT_MINOR_CODE_71,
  RC_ASSERT_MINOR_CODE_72,
  RC_ASSERT_MINOR_CODE_73,
  RC_ASSERT_MINOR_CODE_74,
  RC_ASSERT_MINOR_CODE_75,
  RC_ASSERT_MINOR_CODE_76,
  RC_ASSERT_MINOR_CODE_77,
  RC_ASSERT_MINOR_CODE_78,
  RC_ASSERT_MINOR_CODE_79,
  RC_ASSERT_MINOR_CODE_80,
  RC_ASSERT_MINOR_CODE_81,
  RC_ASSERT_MINOR_CODE_82,
  RC_ASSERT_MINOR_CODE_83,
  RC_ASSERT_MINOR_CODE_84,
  RC_ASSERT_MINOR_CODE_85
} WARN_MINOR_CODE;


//
// Major Warning codes
//
#define  WARN_RDIMM_ON_UDIMM               0x01
  #define WARN_MINOR_CONFIG_NOT_SUPPORTED      0x01
#define  WARN_UDIMM_ON_RDIMM               0x02
#define  WARN_SODIMM_ON_RDIMM              0x03
#define  WARN_4Gb_FUSE                     0x04
#define  WARN_8Gb_FUSE                     0x05
#define  WARN_IMC_DISABLED                 0x06
#define  WARN_DIMM_COMPAT                  0x07
 #define  WARN_DIMM_COMPAT_MINOR_X16_C0MBO             0x01
 #define  WARN_DIMM_COMPAT_MINOR_MAX_RANKS             0x02
 #define  WARN_DIMM_COMPAT_MINOR_QR                    0x03
 #define  WARN_DIMM_COMPAT_MINOR_NOT_SUPPORTED         0x04
 #define  WARN_RANK_NUM                                0x05
 #define  WARN_TOO_SLOW                                0x06
 #define  WARN_DIMM_COMPAT_MINOR_ROW_ADDR_ORDER        0x07
 #define  WARN_CHANNEL_CONFIG_NOT_SUPPORTED            0x08
 #define  WARN_CHANNEL_MIX_ECC_NONECC                  0x09
 #define  WARN_DIMM_VOLTAGE_NOT_SUPPORTED              0x0A
 #define  WARN_DIMM_COMPAT_TRP_NOT_SUPPORTED           0x0B
 #define  WARN_DIMM_NONECC                             0x0C
 #define  WARN_DIMM_COMPAT_3DS_RDIMM_NOT_SUPPORTED     0x0D //3DS
 #define  WARN_RANK_COUNT_MISMATCH                     0x0E
 #define  WARN_DIMM_SKU_MISMATCH                       0x0F
 #define  WARN_DIMM_3DS_NOT_SUPPORTED                  0x10
 #define  WARN_DIMM_NVMDIMM_NOT_SUPPORTED              0x11
 #define  WARN_DIMM_16GB_SUPPORTED                     0x12
 #define  WARN_CHANNEL_SKU_NOT_SUPPORTED               0x13
 #define  WARN_DIMM_SKU_NOT_SUPPORTED                  0x14
 #define  WARN_CHANNEL_CONFIG_FREQ_NOT_SUPPORTED       0x15
 #define  WARN_DIMM_SPEED_NOT_SUP                      0x16
#define  WARN_LOCKSTEP_DISABLE             0x09
 #define  WARN_LOCKSTEP_DISABLE_MINOR_RAS_MODE         0x01
 #define  WARN_LOCKSTEP_DISABLE_MINOR_MISMATCHED       0x02
 #define  WARN_LOCKSTEP_DISABLE_MINOR_MEMTEST_FAILED   0x03
#define  WARN_USER_DIMM_DISABLE            0x0a
 #define  WARN_USER_DIMM_DISABLE_QUAD_AND_3DPC         0x01
 #define  WARN_USER_DIMM_DISABLE_MEMTEST               0x02
#define  WARN_MEMTEST_DIMM_DISABLE         0x0b
#define  WARN_MIRROR_DISABLE               0x0c
 #define  WARN_MIRROR_DISABLE_MINOR_RAS_DISABLED       0x01
 #define  WARN_MIRROR_DISABLE_MINOR_MISMATCH           0x02
 #define  WARN_MIRROR_DISABLE_MINOR_MEMTEST            0x03
#define  WARN_PMIRROR_DISABLE              0x0d
 #define  WARN_PMIRROR_DISABLE_MINOR_RAS_DISABLED      0x01
#define  WARN_INTERLEAVE_FAILURE           0x0e
 #define  WARN_SAD_RULES_EXCEEDED                      0x01
 #define  WARN_TAD_RULES_EXCEEDED                      0x02
 #define  WARN_RIR_RULES_EXCEEDED                      0x03
 #define  WARN_TAD_OFFSET_NEGATIVE                     0x04
 #define  WARN_TAD_LIMIT_ERROR                         0x05
 #define  WARN_INTERLEAVE_3WAY                         0x06
 #define  WARN_A7_MODE_AND_3WAY_CH_INTRLV              0x07
 #define  WARN_INTERLEAVE_EXCEEDED                     0x08
 #define  WARN_DIMM_CAPACITY_MISMATCH                  0x09
 #define  WARN_DIMM_POPULATION_MISMATCH                0x0A
 #define  WARN_NM_MAX_SIZE_EXCEEDED                    0x0B
 #define  WARN_NM_SIZE_BELOW_MIN_LIMIT                 0x0C
 #define  WARN_NM_SIZE_NOT_POWER_OF_TWO                0x0D
 #define  WARN_MAX_INTERLEAVE_SETS_EXCEEDED            0x0E
 #define  WARN_NGN_DIMM_COMM_FAILED                    0x0F
#define  WARN_DIMM_COMM_FAILED             0x0F
 #define WARN_MINOR_DIMM_COMM_FAILED_TIMEOUT           0x01
 #define WARN_MINOR_DIMM_COMM_FAILED_STATUS            0x02
#define  WARN_SPARE_DISABLE                0x10
  #define WARN_SPARE_DISABLE_MINOR_RK_SPARE            0x01
#define  WARN_PTRLSCRB_DISABLE             0x11
#define  WARN_UNUSED_MEMORY                0x12
 #define  WARN_UNUSED_MEMORY_MINOR_MIRROR              0x01
 #define  WARN_UNUSED_MEMORY_MINOR_LOCKSTEP            0x02
#define  WARN_RD_DQ_DQS                    0x13
#define  WARN_RD_RCVEN                     0x14
 #define  WARN_ROUNDTRIP_EXCEEDED                      0x01
 #define  WARN_RCVEN_PI_DELAY_EXCEEDED                 0x02
#define  WARN_WR_LEVEL                     0x15
 #define  WARN_WR_FLYBY_CORR                           0x00
 #define  WARN_WR_FLYBY_UNCORR                         0x01
 #define  WARN_WR_FLYBY_DELAY                          0x02
#define  WARN_WR_DQ_DQS                    0x16
#define  WARN_DIMM_POP_RUL                 0x17
 #define  WARN_DIMM_POP_RUL_MINOR_OUT_OF_ORDER         0x01
 #define  WARN_DIMM_POP_RUL_MINOR_INDEPENDENT_MODE     0x02
 #define  WARN_DIMM_POP_RUL_2_AEP_FOUND_ON_SAME_CH     0x03
 #define  WARN_DIMM_POP_RUL_MINOR_MIXED_RANKS_FOUND    0x04
 #define  WARN_DIMM_POP_RUL_NVMDIMM_OUT_OF_ORDER       0x05
 #define  WARN_DIMM_POP_RUL_UDIMM_POPULATION           BIT5
 #define  WARN_DIMM_POP_RUL_RDIMM_POPULATION           BIT6
 #define  WARN_DIMM_POP_RUL_LRDIMM_DUAL_DIE_POPULATION BIT7
 #define  WARN_DIMM_POP_RUL_LRDIMM_3DS_POPULATION      BIT8
 #define  WARN_DIMM_POP_RUL_RDIMM_3DS_POPULATION       BIT9


#define  WARN_CLTT_DISABLE                 0x18
 #define  WARN_CLTT_MINOR_NO_TEMP_SENSOR               0x01
 #define  WARN_CLTT_MINOR_CIRCUIT_TST_FAILED           0x02
#define  WARN_THROT_INSUFFICIENT           0x19
#define  WARN_CLTT_DIMM_UNKNOWN            0x1a
#define  WARN_DQS_TEST                     0x1b
#define  WARN_MEM_TEST                     0x1c
#define  WARN_CLOSED_PAGE_OVERRIDE         0x1d
#define  WARN_DIMM_VREF_NOT_PRESENT        0x1e
#define  WARN_EARLY_RID                    0x1f
 #define  WARN_EARLY_RID_UNCORR                        0x01
 #define  WARN_EARLY_RID_CYCLE_FAIL                    0x02
#define  WARN_LV_STD_DIMM_MIX              0x20
#define  WARN_LV_2QR_DIMM                  0x21
#define  WARN_LV_3DPC                      0x22
#define  WARN_CMD_ADDR_PARITY_ERR          0x23
#define  WARN_DQ_SWIZZLE_DISC              0x24
 #define  WARN_DQ_SWIZZLE_DISC_UNCORR                  0x01
#define  WARN_COD_HA_NOT_ACTIVE            0x25
#define  WARN_CMD_CLK_TRAINING             0x26
 #define  WARN_CMD_PI_GROUP_SMALL_EYE                  0x01
#define  WARN_INVALID_BUS                  0x27
#define  WARN_INVALID_FNV_OPCODE           0x28
#define  WARN_MEMORY_TRAINING              0x29
 #define  WARN_CTL_CLK_LOOPBACK_TRAINING               0x02
 #define  WARN_ODT_TIMING_OVERFLOW                     0x03
#define  WARN_NO_DDR_MEMORY                0x2a
 #define  WARN_NO_MEMORY_MINOR_NO_MEMORY               0x01
 #define  WARN_NO_MEMORY_MINOR_ALL_CH_DISABLED         0x02
 #define  WARN_NO_MEMORY_MINOR_ALL_CH_DISABLED_MIXED   0x03
#define  WARN_ROUNDTRIP_ERROR              0x2b
 #define  WARN_RCVNTAP_CMDDELAY_EXCEEDED               0x01
#define  WARN_MEMORY_MODEL_ERROR           0x2c
 #define  WARN_SNC24_MODEL_ERROR                       0x01
 #define  WARN_QUAD_HEMI_MODEL_ERROR                   0x02
 #define  WARN_SNC24_DIMM_POPULATION_MISMATCH          0x03
 #define  WARN_SNC24_INCOMPATIBLE_DDR_CAPACITY         0x04
 #define  WARN_SNC24_INCOMPATIBLE_MCDRAM_CAPACITY      0x05
 #define  WARN_MCDRAM_CONFIG_NOT_SUPPORTED             0x06
 #define  WARN_SNC24_TILE_POPULATION_MISMATCH          0x07
#define  WARN_OVERRIDE_MEMORY_MODE  0x2d
 #define  WARN_OVERRIDE_TO_FLAT_NO_MCDRAM_MEMORY       0x01
 #define  WARN_OVERRIDE_TO_FLAT_NO_DDR_MEMORY          0x02
 #define  WARN_OVERRIDE_TO_HYBRID_CACHESIZE_1_2        0x03
#define  WARN_MEM_INIT                     0x2e
#define  WARN_SENS_AMP_TRAINING            0x2f
  #define  WARN_SENS_AMP_CH_FAILIURE                   0x01

#define  WARN_FPT_CORRECTABLE_ERROR        0x30
#define  WARN_FPT_UNCORRECTABLE_ERROR      0x31
  #define  WARN_FPT_MINOR_RD_DQ_DQS                    0x13
  #define  WARN_FPT_MINOR_RD_RCVEN                     0x14
  #define  WARN_FPT_MINOR_WR_LEVEL                     0x15
  #define  WARN_FPT_MINOR_WR_FLYBY                     0x00
  #define  WARN_FPT_MINOR_WR_DQ_DQS                    0x16
  #define  WARN_FPT_MINOR_DQS_TEST                     0x1b
  #define  WARN_FPT_MINOR_MEM_TEST                     0x1c
  #define  WARN_FPT_MINOR_LRDIMM_TRAINING              0x24
  #define  WARN_FPT_MINOR_VREF_TRAINING                0x25
  #define  WARN_FPT_MINOR_LRDIMM_RCVEN_PHASE_TRAINING  0x26
  #define  WARN_FPT_MINOR_LRDIMM_RCVEN_CYCLE_TRAINING  0x27
  #define  WARN_FPT_MINOR_LRDIMM_READ_DELAY_TRAINING   0x28
  #define  WARN_FPT_MINOR_LRDIMM_WL_TRAINING           0x29
  #define  WARN_FPT_MINOR_LRDIMM_COARSE_WL_TRAINING    0x2A
  #define  WARN_FPT_MINOR_LRDIMM_WRITE_DELAY_TRAINING  0x2B

#define  WARN_CH_DISABLED                  0x32
  #define  WARN_TWR_LIMIT_REACHED                      0x01
  #define  WARN_TWR_LIMIT_ON_LOCKSTEP_CH               0x02

#define  WARN_MEM_LIMIT                    0x33
#define  WARN_RT_DIFF_EXCEED               0x34
  #define WARN_RT_DIFF_MINOR_EXCEED                    0x01

#define  WARN_PPR_FAILED                   0x35
#define  WARN_REGISTER_OVERFLOW            0x36
  #define  WARN_MINOR_REGISTER_OVERFLOW                      0x01
#define  WARN_SWIZZLE_DISCOVERY_TRAINING   0x37
 #define  WARN_SWIZZLE_PATTERN_MISMATCH                0x01
#define  WARN_WRCRC_DISABLE                0x38
 #define  WARN_TRAIL_ODT_LIMIT_REACHED                 0x01
#define  WARN_FNV_BSR                      0x39
 #define  WARN_DT_ERROR                                0x01
 #define  WARN_MEDIA_READY_ERROR                       0x02
 #define  WARN_POLLING_LOOP_TIMEOUT                    0x03
 #define  WARN_OPCODE_INDEX_LOOKUP                     0x04

#define  WARN_ADDDC_DISABLE                0x3a
  #define  WARN_ADDDC_MINOR_DISABLE                0x01
#define  WARN_SDDC_DISABLE                 0x3b
  #define  WARN_SDDC_MINOR_DISABLE                 0x02

#define WARN_FW_OUT_OF_DATE                0x3c
  #define WARN_FW_CLK_MOVEMENT                         0x00
  #define WARN_FW_BCOM_MARGINING                       0x01

#define  WARN_MEM_CONFIG_CHANGED           0x40
 #define  WARN_MEM_OVERRIDE_DISABLED                   0x01

#define  WARN_MCA_UNCORRECTABLE_ERROR   0x50

#define  WARN_DM_TEST_ERROR_CODE        0x60
  #define WARN_DM_TEST_PARSE_ERROR_MINOR_CODE          0x01
  #define WARN_DM_TEST_CONFIGURATION_ERROR_MINOR_CODE  0x02
  #define WARN_DM_TEST_EXECUTION_ERROR_MINOR_CODE      0x03
#define WARN_FPGA_NOT_DETECTED          0x75


//
// HBM WARNING CODES
//
#define WARN_FSM_ERROR_CODE             0x80
  #define WARN_FSM_TIMEOUT_ERROR                       0x01

#define WARN_IEEE_1500_ERROR_CODE       0x81
  #define WARN_IEEE_1500_TIMEOUT_ERROR                 0x01

#define  WARN_HBM_CORRECTABLE_ERROR     0x82
#define  WARN_HBM_UNCORRCTABLE_ERROR    0x83
  #define  WARN_HBM_BASIC_XOVER_FSM                    0x01
  #define  WARN_HBM_BASIC_AWORD_FSM                    0x02
  #define  WARN_HBM_BASIC_DWORD_TX_FSM                 0x03
  #define  WARN_HBM_BASIC_DWORD_RX_FSM                 0x04
  #define  WARN_HBM_RX_DQ_DELAY_ADJ                    0x05
  #define  WARN_HBM_ADVANCE_AWORD                      0x06
  #define  WARN_HBM_TX_DS_TCO_TRAINING                 0x07
  #define  WARN_HBM_RX_VREF_TRAINING                   0x08
  #define  WARN_HBM_ADVANCE_RX_DWORD                   0x09
  #define  WARN_HBM_ADVANCE_TX_DWORD                   0x0A
  #define  WARN_HBM_ROUNDTRIP_LATENCY                  0x0B
  #define  WARN_HBM_RX_DELAY_OPT                       0x0C
#define WARN_NVMCTRL_MEDIA_STATUS       0x84
  #define WARN_NVMCTRL_MEDIA_NOTREADY        0x02
  #define WARN_NVMCTRL_MEDIA_INERROR         0x03
  #define WARN_NVMCTRL_MEDIA_RESERVED        0x04

#endif // _mem_platformcommon_h
