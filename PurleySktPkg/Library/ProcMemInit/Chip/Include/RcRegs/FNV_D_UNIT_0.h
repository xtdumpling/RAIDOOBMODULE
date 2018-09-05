//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      Reference Code                                                    *
// *                                                                        *
// *      Copyright (c) 2007-2016, Intel Corporation.                       *
// *                                                                        *
// *      This software and associated documentation (if any) is furnished  *
// *      under a license and may only be used or copied in accordance      *
// *      with the terms of the license. Except as permitted by such        *
// *      license, no part of this software or documentation may be         *
// *      reproduced, stored in a retrieval system, or transmitted in any   *
// *      form or by any means without the express written consent of       *
// *      Intel Corporation.                                                *
// *                                                                        *
// *      This program has been developed by Intel Corporation.             *
// *      Licensee has Intel's permission to incorporate this source code   *
// *      into their product, royalty free.  This source code may NOT be    *
// *      redistributed to anyone without Intel's written permission.       *
// *                                                                        *
// *      Intel specifically disclaims all warranties, express or           *
// *      implied, and all liability, including consequential and other     *
// *      indirect damages, for the use of this code, including liability   *
// *      for infringement of any proprietary rights, and including the     *
// *      warranties of merchantability and fitness for a particular        *
// *      purpose.  Intel does not assume any responsibility for any        *
// *      errors which may appear in this code nor any responsibility to    *
// *      update it.                                                        *
// *                                                                        *
// **************************************************************************
// **************************************************************************
// *                                                                        *
// *  PURPOSE:                                                              *
// *                                                                        *
// *      This file contains Silicon register definitions.                  *
// *                                                                        *
// **************************************************************************
//
/* Date Stamp: 1/5/2016 */

#ifndef FNV_D_UNIT_0_h
#define FNV_D_UNIT_0_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* FNV_D_UNIT_0_DEV 0                                                         */
/* FNV_D_UNIT_0_FUN 4                                                         */

/* D_FW_STATUS_FNV_D_UNIT_0_REG supported on:                                 */
/*       EKV (0x40004000)                                                     */
/* Register default value:              0x00000000                            */
#define D_FW_STATUS_FNV_D_UNIT_0_REG 0x05044000
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * BIOS will read this register to determine FNV Firmware Status during boot.
 */
typedef union {
  struct {
    UINT32 fw_status : 32;
    /* fw_status - Bits[31:0], RW, default = 32'h0 
       Firmware status.
     */
  } Bits;
  UINT32 Data;
} D_FW_STATUS_FNV_D_UNIT_0_STRUCT;


/* D_FW_STATUS_H_FNV_D_UNIT_0_REG supported on:                               */
/*       EKV (0x40004004)                                                     */
/* Register default value:              0x00000000                            */
#define D_FW_STATUS_H_FNV_D_UNIT_0_REG 0x05044004
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * BIOS will read this register to determine FNV Firmware Status during boot.
 */
typedef union {
  struct {
    UINT32 fw_status : 32;
    /* fw_status - Bits[31:0], RW, default = 32'h0 
       Firmware status.
     */
  } Bits;
  UINT32 Data;
} D_FW_STATUS_H_FNV_D_UNIT_0_STRUCT;


/* D_DDRT_TRAINING_EN_FNV_D_UNIT_0_REG supported on:                          */
/*       EKV (0x40004008)                                                     */
/* Register default value:              0x00000000                            */
#define D_DDRT_TRAINING_EN_FNV_D_UNIT_0_REG 0x05044008
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Training  Enable
 */
typedef union {
  struct {
    UINT32 tm_en : 1;
    /* tm_en - Bits[0:0], RW, default = 1'h0 
       DDRT Training Mode Enable [br]
                      Setting this bit will block read and write command from sending 
       to memory 
     */
    UINT32 early_cmd_ck_trn : 1;
    /* early_cmd_ck_trn - Bits[1:1], RW, default = 1'h0 
       EARLY CMD-CK Training Enable [br]
                      Parity only, commands are not written into ingress command queue
     */
    UINT32 adv_cmd_ck_trn : 1;
    /* adv_cmd_ck_trn - Bits[2:2], RW, default = 1'h0 
       Advanced CMD-CK Training Enable[br]
                      MRS, extended MRS and Memory Mapped config access are ignored
     */
    UINT32 reserved_3 : 1;
    /* reserved_3 - Bits[3:3], RW, default = 1'h0 
       Reserved (3)
     */
    UINT32 reserved_4 : 1;
    /* reserved_4 - Bits[4:4], RW, default = 1'h0 
       Reserved (4)
     */
    UINT32 disable_rid_feedback : 1;
    /* disable_rid_feedback - Bits[5:5], RW, default = 1'h0 
       When set, the RID bits are no longer returned within the data packet,[br]
       		       allowing for all data in the buffer to be returned as it was written
     */
    UINT32 reserved_6 : 1;
    /* reserved_6 - Bits[6:6], RW, default = 1'h0 
       Reserved (6)
     */
    UINT32 reserved_7 : 1;
    /* reserved_7 - Bits[7:7], RW, default = 1'h0 
       Reserved (7)
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} D_DDRT_TRAINING_EN_FNV_D_UNIT_0_STRUCT;


/* D_DDRT_TRAINING_OTHER_EN_FNV_D_UNIT_0_REG supported on:                    */
/*       EKV (0x4000400C)                                                     */
/* Register default value:              0x00000000                            */
#define D_DDRT_TRAINING_OTHER_EN_FNV_D_UNIT_0_REG 0x0504400C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Other Training  Enable
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reset_trn : 1;
    /* reset_trn - Bits[2:2], RW, default = 1'h0 
       Reset Training Enable [br]
                      Set this bit to reset training FSM TBD
     */
    UINT32 rsvd_3 : 27;
    /* rsvd_3 - Bits[29:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 pche_inv_set0_only : 1;
    /* pche_inv_set0_only - Bits[30:30], RWV, default = 1'b0 
       When this bit is asserted, DUNIT Prefetch Cache will invalidate SET 0 Only.
       		Hardware will clear CSR, once done.
       		Use this CSR right after Training. Prefetch SET 0 is used for training and 
       		must be invalidated when switching to normal mode.
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} D_DDRT_TRAINING_OTHER_EN_FNV_D_UNIT_0_STRUCT;








/* D_DDRT_TRAINING_MPR_FNV_D_UNIT_0_REG supported on:                         */
/*       EKV (0x4000401C)                                                     */
/* Register default value:              0x000F3355                            */
#define D_DDRT_TRAINING_MPR_FNV_D_UNIT_0_REG 0x0504401C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * MPR Training Pattern
 */
typedef union {
  struct {
    UINT32 mpr0 : 8;
    /* mpr0 - Bits[7:0], RW, default = 8'h55 
       MPR training pattern 0
     */
    UINT32 mpr1 : 8;
    /* mpr1 - Bits[15:8], RW, default = 8'h33 
       MPR training pattern 1
     */
    UINT32 mpr2 : 8;
    /* mpr2 - Bits[23:16], RW, default = 8'h0F 
       MPR training pattern 2
     */
    UINT32 mpr3 : 8;
    /* mpr3 - Bits[31:24], RW, default = 8'h00 
       MPR training pattern 3
     */
  } Bits;
  UINT32 Data;
} D_DDRT_TRAINING_MPR_FNV_D_UNIT_0_STRUCT;


/* D_DDRT_TRAINING_MR3_FNV_D_UNIT_0_REG supported on:                         */
/*       EKV (0x40004020)                                                     */
/* Register default value:              0x00000000                            */
#define D_DDRT_TRAINING_MR3_FNV_D_UNIT_0_REG 0x05044020
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register mimics DDR4 SRAM Mode Registers.
 */
typedef union {
  struct {
    UINT32 mpr_sel : 2;
    /* mpr_sel - Bits[1:0], RW, default = 2'h0 
       MPR page selection[br]
                       0x0 : Select MPR0[br]
                       0x1 : Select MPR1[br]
                       0x2 : Select MPR2[br]
                       0x3 : Select MPR3
     */
    UINT32 mpr_op : 1;
    /* mpr_op - Bits[2:2], RW, default = 1'h0 
       MPR operation[br]
                       0 : normal[br]
                       1 : data flow from/to MPR
     */
    UINT32 rsvd_3 : 1;
    /* rsvd_3 - Bits[3:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 per_dram_addr_ability : 1;
    /* per_dram_addr_ability - Bits[4:4], RW, default = 1'h0 
       Reserved
     */
    UINT32 rsvd_5 : 6;
    /* rsvd_5 - Bits[10:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mpr_rd_frmt : 2;
    /* mpr_rd_frmt - Bits[12:11], RW, default = 2'h0 
       MPR read format; FNV supports both serial and parallel MPR read format
                       00 = Serial [br]
                       01 = Parallel [br]
                       others = reserved
     */
    UINT32 rsvd_13 : 19;
    /* rsvd_13 - Bits[31:13], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} D_DDRT_TRAINING_MR3_FNV_D_UNIT_0_STRUCT;


/* D_MC_WDATA_FNV_D_UNIT_0_REG supported on:                                  */
/*       EKV (0x40004030)                                                     */
/* Register default value:              0x00000000                            */
#define D_MC_WDATA_FNV_D_UNIT_0_REG 0x05044030
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register holds write data for Message Channel windowed write transaction. 
 * This should be programmed prior to starting a window transaction. 
 */
typedef union {
  struct {
    UINT32 mc_wdata : 32;
    /* mc_wdata - Bits[31:0], RW, default = 32'h0 
       Write Data for MC transaction.
     */
  } Bits;
  UINT32 Data;
} D_MC_WDATA_FNV_D_UNIT_0_STRUCT;


/* D_MC_CTRL_FNV_D_UNIT_0_REG supported on:                                   */
/*       EKV (0x40004034)                                                     */
/* Register default value:              0x00000000                            */
#define D_MC_CTRL_FNV_D_UNIT_0_REG 0x05044034
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register includes address and command for indirect access to DDRT IO and 
 * DDRT Agent CSRs, which live on Message Channel. 
 */
typedef union {
  struct {
    UINT32 mc_addr : 16;
    /* mc_addr - Bits[15:0], RW, default = 16'h0 
       Destination Address for Message Channel transaction.
     */
    UINT32 mc_port : 8;
    /* mc_port - Bits[23:16], RW, default = 8'h0 
       Destination Port ID for Message Channel transaction.[br]
                       Daunit CSRs PortID: 8'h03[br]
                       IO CSRs PortID: 8'h71
     */
    UINT32 mc_be : 4;
    /* mc_be - Bits[27:24], RW, default = 4'b0 
       Byte enable for Message Channel transaction. Corresponds to First BE[3:0].
     */
    UINT32 rsvd : 1;
    /* rsvd - Bits[28:28], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mc_posted : 1;
    /* mc_posted - Bits[29:29], RW, default = 1'b0 
       Indicates whether the Message Channel message is posted or nonposted. Non-posted 
       messages require a Message Channel Completion message. Posted message do not 
       require a Message Channel Completion message. All reads are nonposted because 
       data returns in the completion message.[br] 
                       1: Posted[br]
                       0: Nonposted
     */
    UINT32 mc_opcode : 1;
    /* mc_opcode - Bits[30:30], RW, default = 1'b0 
       Opcode bit for Message Channel transaction. (0 = Read, 1 = Write)
     */
    UINT32 mc_start : 1;
    /* mc_start - Bits[31:31], RWV, default = 1'b0 
       AHB start bit for Message Channel window control transaction. This bit initiates 
       a command on to the Message Channel. 
     */
  } Bits;
  UINT32 Data;
} D_MC_CTRL_FNV_D_UNIT_0_STRUCT;


/* D_MC_ACC_INFO_FNV_D_UNIT_0_REG supported on:                               */
/*       EKV (0x40004038)                                                     */
/* Register default value:              0x00000000                            */
#define D_MC_ACC_INFO_FNV_D_UNIT_0_REG 0x05044038
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This holds addition information for MC transactions
 */
typedef union {
  struct {
    UINT32 mc_acc_info : 12;
    /* mc_acc_info - Bits[11:0], RW, default = 12'h0 
       Holds additional info required for IO MC transaction.[br]
       IO register access:[br]
       [br]
       Bits   [11] - IA [br]
       Bits [10:8] - Bus [br]
       Bits  [7:3] - Device [br]
       Bits  [2:0] - Function [br]
       Note: Bus is always 2 for IO MC access.[br]
       For non-IO MC CSR access, this register should be ignored, because it is not 
       used. 
     */
    UINT32 rsvd : 20;
    /* rsvd - Bits[31:12], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} D_MC_ACC_INFO_FNV_D_UNIT_0_STRUCT;


/* D_MC_TIMEOUT_FNV_D_UNIT_0_REG supported on:                                */
/*       EKV (0x4000403C)                                                     */
/* Register default value:              0x00000000                            */
#define D_MC_TIMEOUT_FNV_D_UNIT_0_REG 0x0504403C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register controls the timeout setting of when to trigger a timeout.
 */
typedef union {
  struct {
    UINT32 mc_timeout : 31;
    /* mc_timeout - Bits[30:0], RW, default = 31'b0 
       After a non-posted Message Channel transaction has been sent and timeout counter 
       is enabled, the counter will trigger a timeout and the status is reported in the 
       Message Channel status register. The counter increments per core clock cycle. 
     */
    UINT32 mc_timeout_vld : 1;
    /* mc_timeout_vld - Bits[31:31], RW, default = 1'b0 
       After a non-posted Message Channel transaction has been sent and timeout counter 
       is enabled, the counter will trigger a timeout and the status is reported in the 
       Message Channel status register.[br] 
               1'b1 - Enable timeout.[br]
               1'b0 - Disable timeout
     */
  } Bits;
  UINT32 Data;
} D_MC_TIMEOUT_FNV_D_UNIT_0_STRUCT;


/* D_MC_STAT_FNV_D_UNIT_0_REG supported on:                                   */
/*       EKV (0x40004040)                                                     */
/* Register default value:              0x00000000                            */
#define D_MC_STAT_FNV_D_UNIT_0_REG 0x05044040
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register holds the status of the Message Channel.
 */
typedef union {
  struct {
    UINT32 mc_sts : 1;
    /* mc_sts - Bits[0:0], ROV, default = 1'b0 
       This bit signifies MC Window Control is busy and cannot process another 
       transaction yet. 
     */
    UINT32 mc_addr_err : 1;
    /* mc_addr_err - Bits[1:1], ROV, default = 1'b0 
       This indicates that the provided Message Channel address and/or PortID is 
       invalid.[br] 
                       1'b1: Invalid[br]
                       1'b0: No errors
     */
    UINT32 mc_to : 1;
    /* mc_to - Bits[2:2], RW1CV, default = 1'b0 
       This indicates that the provided Message Channel access exceeded the timeout 
       value while waiting for a response.[br] 
                       1'b1: Timeout[br]
                       1'b0: Default
     */
    UINT32 rsvd_3 : 5;
    /* rsvd_3 - Bits[7:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mc_rdata_vld : 1;
    /* mc_rdata_vld - Bits[8:8], RW1CV, default = 1'b0 
       Valid bit for MC read data.
     */
    UINT32 rsvd_9 : 7;
    /* rsvd_9 - Bits[15:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mc_rdata_src : 1;
    /* mc_rdata_src - Bits[16:16], RW1CV, default = 1'b0 
       This holds the rdata source (0 = IO, 1 = Daunit).
     */
    UINT32 rsvd_17 : 7;
    /* rsvd_17 - Bits[23:17], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 mc_resp : 1;
    /* mc_resp - Bits[24:24], RW1CV, default = 1'b0 
       Message Channel message responded. Either the write completed or there was a 
       miss. 
     */
    UINT32 rsvd_25 : 7;
    /* rsvd_25 - Bits[31:25], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} D_MC_STAT_FNV_D_UNIT_0_STRUCT;


/* D_MC_RDATA_FNV_D_UNIT_0_REG supported on:                                  */
/*       EKV (0x40004044)                                                     */
/* Register default value:              0x00000000                            */
#define D_MC_RDATA_FNV_D_UNIT_0_REG 0x05044044
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Holds the read data from MC transaction.
 */
typedef union {
  struct {
    UINT32 mc_rdata : 32;
    /* mc_rdata - Bits[31:0], RWV, default = 32'h0 
       This holds the returned read data from the Message Channel.
     */
  } Bits;
  UINT32 Data;
} D_MC_RDATA_FNV_D_UNIT_0_STRUCT;


/* DN_ECC_ENABLE_FNV_D_UNIT_0_REG supported on:                               */
/*       EKV (0x40004048)                                                     */
/* Register default value:              0x00000000                            */
#define DN_ECC_ENABLE_FNV_D_UNIT_0_REG 0x05044048
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Read ECC Control Registers
 */
typedef union {
  struct {
    UINT32 ecc_rd_chk_en : 1;
    /* ecc_rd_chk_en - Bits[0:0], RW, default = 1'h0 
       Enable read ECC generation[br]
                       0 = ecc check disabled[br]
                       1 = ecc check enabled
     */
    UINT32 rsvd : 31;
    /* rsvd - Bits[31:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DN_ECC_ENABLE_FNV_D_UNIT_0_STRUCT;


/* DRDP_WCRD_CONFIG_FNV_D_UNIT_0_REG supported on:                            */
/*       EKV (0x40004050)                                                     */
/* Register default value:              0x00000038                            */
#define DRDP_WCRD_CONFIG_FNV_D_UNIT_0_REG 0x05044050
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Controls the number of write credits allocated for DDRT
 */
typedef union {
  struct {
    UINT32 wcrd_cfg : 6;
    /* wcrd_cfg - Bits[5:0], RW, default = 6'd56 
       Setting this will specify the number of write credits allocated for DDRT. 
       Default is 56 write credits 
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DRDP_WCRD_CONFIG_FNV_D_UNIT_0_STRUCT;


/* DN_CRD_CNT_FNV_D_UNIT_0_REG supported on:                                  */
/*       EKV (0x40004054)                                                     */
/* Register default value:              0x00000000                            */
#define DN_CRD_CNT_FNV_D_UNIT_0_REG 0x05044054
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Indicates status of write and read credit counts
 */
typedef union {
  struct {
    UINT32 wcrd_rcyc_cnt : 7;
    /* wcrd_rcyc_cnt - Bits[6:0], ROV, default = 7'h0 
       Indicates the number of write credits waiting to return to host
     */
    UINT32 rcrd_cnt : 7;
    /* rcrd_cnt - Bits[13:7], ROV, default = 7'h0 
       Indicates the number of read credits free within FNV
     */
    UINT32 wcrd_cnt : 7;
    /* wcrd_cnt - Bits[20:14], ROV, default = 7'h0 
       Indicates the number of write credits free within FNV
     */
    UINT32 rsvd : 11;
    /* rsvd - Bits[31:21], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DN_CRD_CNT_FNV_D_UNIT_0_STRUCT;


/* D_READ_CREDIT_FNV_D_UNIT_0_REG supported on:                               */
/*       EKV (0x40004058)                                                     */
/* Register default value:              0x00000028                            */
#define D_READ_CREDIT_FNV_D_UNIT_0_REG 0x05044058
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register contains the default value of read credits. Will be loaded when 
 * value of CSR changes 
 */
typedef union {
  struct {
    UINT32 val : 6;
    /* val - Bits[5:0], RW, default = 6'h28 
       Value of Read credits. Default value is 40
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} D_READ_CREDIT_FNV_D_UNIT_0_STRUCT;


















































/* D_FUNC_DEFEATURE1_FNV_D_UNIT_0_REG supported on:                           */
/*       EKV (0x40004130)                                                     */
/* Register default value:              0x00041C00                            */
#define D_FUNC_DEFEATURE1_FNV_D_UNIT_0_REG 0x05044130
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Defeature 1
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 21;
    UINT32 wcrd_init : 1;
    /* wcrd_init - Bits[21:21], RW, default = 1'h0 
       Initialize credit to max setting [br]
                       Write 0 and then 1 to initialize write credit [br]
                       0: default [br]
                       1: Initial write credit
     */
    UINT32 rsvd_22 : 10;
  } Bits;
  UINT32 Data;
} D_FUNC_DEFEATURE1_FNV_D_UNIT_0_STRUCT;


/* D_FUNC_DEFEATURE2_FNV_D_UNIT_0_REG supported on:                           */
/*       EKV (0x40004134)                                                     */
/* Register default value:              0x0011FA1E                            */
#define D_FUNC_DEFEATURE2_FNV_D_UNIT_0_REG 0x05044134
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Defeature 2
 */
typedef union {
  struct {
    UINT32 err_pche_miss : 1;
    /* err_pche_miss - Bits[0:0], RW, default = 1'h0 
       DDRT_ERR# is asserted if read misses prefetch cache during training [br]
                       0: Disable - (default)[br]
                       1: DDRT_ERR# is asserted if read misses prefetch cache during 
       DDRT training 
     */
    UINT32 rsvd_1 : 31;
  } Bits;
  UINT32 Data;
} D_FUNC_DEFEATURE2_FNV_D_UNIT_0_STRUCT;












/* D_RST_MASK_FNV_D_UNIT_0_REG supported on:                                  */
/*       EKV (0x4000414C)                                                     */
/* Register default value:              0x00000003                            */
#define D_RST_MASK_FNV_D_UNIT_0_REG 0x0504414C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Reset Interrupt Mask
 */
typedef union {
  struct {
    UINT32 sprs_rst_int : 1;
    /* sprs_rst_int - Bits[0:0], RW, default = 1'h1 
       Surprise reset interrupt mask [br]
                       0: Generate interrupt after surprise reset is detected 
       (ddrt_rst_n=0)[br] 
                          Surprise reset will generate interrupt to FW only in 
       functional mode; [br] 
                          in training mode, surprise reset will not generate interrupt 
       to FW [br] 
                       1: default
     */
    UINT32 rsvd_1 : 1;
    UINT32 rsvd : 30;
    /* rsvd - Bits[31:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} D_RST_MASK_FNV_D_UNIT_0_STRUCT;


























/* DN_EMASK_FNV_D_UNIT_0_REG supported on:                                    */
/*       EKV (0x40004180)                                                     */
/* Register default value:              0x001FFFFF                            */
#define DN_EMASK_FNV_D_UNIT_0_REG 0x05044180
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Masking errors per source
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    UINT32 wr_credit : 1;
    /* wr_credit - Bits[2:2], RW, default = 1'b1 
       Mask Error [br]
                       1 = Mask Error [br]
                       0 = Allow Error [br]
                       Default is 1
     */
    UINT32 rsvd_3 : 29;
  } Bits;
  UINT32 Data;
} DN_EMASK_FNV_D_UNIT_0_STRUCT;




































































































































































































































































































































































































































































/* DN_SPARE_CTRL_FNV_D_UNIT_0_REG supported on:                               */
/*       EKV (0x40004704)                                                     */
/* Register default value:              0x00000000                            */
#define DN_SPARE_CTRL_FNV_D_UNIT_0_REG 0x05044704
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register contains spare
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 16;
    UINT32 spare_1 : 16;
    /* spare_1 - Bits[31:16], RW, default = 16'h0000 
       [0] - Disable DN Message Channel Arbitration timeout. Otherwise default is to 
       timeout if no response is seen from source.[br] 
                                          [1:13] - Reserved for future use.[br]
                                          [14] - Reserved.[br]
                                          [15] - Reserved.
     */
  } Bits;
  UINT32 Data;
} DN_SPARE_CTRL_FNV_D_UNIT_0_STRUCT;
































/* MB_SMBUS_CMD_FNV_D_UNIT_0_REG supported on:                                */
/*       EKV (0x40004804)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_CMD_FNV_D_UNIT_0_REG 0x05044804
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register contains the FW Mailbox Command.
 */
typedef union {
  struct {
    UINT32 opcode : 16;
    /* opcode - Bits[15:0], RW, default = 16'h0 
       FW MB OPCODE. Refer to FIS for detailed explaination.
     */
    UINT32 doorbell : 1;
    /* doorbell - Bits[16:16], RW, default = 1'b0 
       FW MB Doorbell
     */
    UINT32 spares : 15;
    /* spares - Bits[31:17], RW, default = 15'b0 
       Refer to FIS for details
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_CMD_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_NONCE_0_FNV_D_UNIT_0_REG supported on:                            */
/*       EKV (0x40004808)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_NONCE_0_FNV_D_UNIT_0_REG 0x05044808
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register contains the FW Mailbox Nonce.
 */
typedef union {
  struct {
    UINT32 nonce : 32;
    /* nonce - Bits[31:0], RW, default = 32'h0 
       Firmware Mailbox Nonce
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_NONCE_0_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_NONCE_1_FNV_D_UNIT_0_REG supported on:                            */
/*       EKV (0x4000480C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_NONCE_1_FNV_D_UNIT_0_REG 0x0504480C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register contains the FW Mailbox Nonce.
 */
typedef union {
  struct {
    UINT32 nonce : 32;
    /* nonce - Bits[31:0], RW, default = 32'h0 
       Firmware Mailbox Nonce
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_NONCE_1_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_NONCE_2_FNV_D_UNIT_0_REG supported on:                            */
/*       EKV (0x40004810)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_NONCE_2_FNV_D_UNIT_0_REG 0x05044810
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Reserved for future use.
 */
typedef union {
  struct {
    UINT32 nonce : 32;
    /* nonce - Bits[31:0], RW, default = 32'h0 
       Firmware Mailbox Nonce
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_NONCE_2_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_NONCE_3_FNV_D_UNIT_0_REG supported on:                            */
/*       EKV (0x40004814)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_NONCE_3_FNV_D_UNIT_0_REG 0x05044814
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Reserved for future use.
 */
typedef union {
  struct {
    UINT32 nonce : 32;
    /* nonce - Bits[31:0], RW, default = 32'h0 
       Firmware Mailbox Nonce
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_NONCE_3_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_0_FNV_D_UNIT_0_REG supported on:                    */
/*       EKV (0x40004818)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_0_FNV_D_UNIT_0_REG 0x05044818
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_0_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_1_FNV_D_UNIT_0_REG supported on:                    */
/*       EKV (0x4000481C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_1_FNV_D_UNIT_0_REG 0x0504481C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_1_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_2_FNV_D_UNIT_0_REG supported on:                    */
/*       EKV (0x40004820)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_2_FNV_D_UNIT_0_REG 0x05044820
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_2_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_3_FNV_D_UNIT_0_REG supported on:                    */
/*       EKV (0x40004824)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_3_FNV_D_UNIT_0_REG 0x05044824
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_3_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_4_FNV_D_UNIT_0_REG supported on:                    */
/*       EKV (0x40004828)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_4_FNV_D_UNIT_0_REG 0x05044828
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_4_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_5_FNV_D_UNIT_0_REG supported on:                    */
/*       EKV (0x4000482C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_5_FNV_D_UNIT_0_REG 0x0504482C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_5_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_6_FNV_D_UNIT_0_REG supported on:                    */
/*       EKV (0x40004830)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_6_FNV_D_UNIT_0_REG 0x05044830
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_6_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_7_FNV_D_UNIT_0_REG supported on:                    */
/*       EKV (0x40004834)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_7_FNV_D_UNIT_0_REG 0x05044834
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_7_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_8_FNV_D_UNIT_0_REG supported on:                    */
/*       EKV (0x40004838)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_8_FNV_D_UNIT_0_REG 0x05044838
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_8_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_9_FNV_D_UNIT_0_REG supported on:                    */
/*       EKV (0x4000483C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_9_FNV_D_UNIT_0_REG 0x0504483C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_9_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_10_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004840)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_10_FNV_D_UNIT_0_REG 0x05044840
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_10_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_11_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004844)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_11_FNV_D_UNIT_0_REG 0x05044844
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_11_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_12_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004848)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_12_FNV_D_UNIT_0_REG 0x05044848
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_12_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_13_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x4000484C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_13_FNV_D_UNIT_0_REG 0x0504484C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_13_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_14_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004850)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_14_FNV_D_UNIT_0_REG 0x05044850
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_14_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_15_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004854)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_15_FNV_D_UNIT_0_REG 0x05044854
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_15_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_16_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004858)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_16_FNV_D_UNIT_0_REG 0x05044858
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_16_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_17_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x4000485C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_17_FNV_D_UNIT_0_REG 0x0504485C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_17_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_18_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004860)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_18_FNV_D_UNIT_0_REG 0x05044860
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_18_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_19_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004864)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_19_FNV_D_UNIT_0_REG 0x05044864
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_19_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_20_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004868)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_20_FNV_D_UNIT_0_REG 0x05044868
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_20_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_21_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x4000486C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_21_FNV_D_UNIT_0_REG 0x0504486C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_21_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_22_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004870)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_22_FNV_D_UNIT_0_REG 0x05044870
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_22_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_23_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004874)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_23_FNV_D_UNIT_0_REG 0x05044874
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_23_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_24_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004878)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_24_FNV_D_UNIT_0_REG 0x05044878
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_24_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_25_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x4000487C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_25_FNV_D_UNIT_0_REG 0x0504487C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_25_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_26_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004880)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_26_FNV_D_UNIT_0_REG 0x05044880
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_26_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_27_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004884)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_27_FNV_D_UNIT_0_REG 0x05044884
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_27_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_28_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004888)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_28_FNV_D_UNIT_0_REG 0x05044888
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_28_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_29_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x4000488C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_29_FNV_D_UNIT_0_REG 0x0504488C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_29_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_30_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004890)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_30_FNV_D_UNIT_0_REG 0x05044890
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_30_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_INPUT_PAYLOAD_31_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x40004894)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_INPUT_PAYLOAD_31_FNV_D_UNIT_0_REG 0x05044894
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_INPUT_PAYLOAD_31_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_STATUS_FNV_D_UNIT_0_REG supported on:                             */
/*       EKV (0x40004898)                                                     */
/* Register default value:              0x00000001                            */
#define MB_SMBUS_STATUS_FNV_D_UNIT_0_REG 0x05044898
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register contains the FW Mailbox Status.
 */
typedef union {
  struct {
    UINT32 comp : 1;
    /* comp - Bits[0:0], RWV, default = 1'h1 
       FW Mailbox Completion:[br]
                       0 = Not Complete[br]
                       1 = Complete[br]
                       HW will clear this bit when doorbell bit is set.
     */
    UINT32 fail : 1;
    /* fail - Bits[1:1], RW, default = 1'h0 
       FW Mailbox Nonce Fail:[br]
                       0 = Success[br]
                       1 = Failure
     */
    UINT32 spare0 : 6;
    /* spare0 - Bits[7:2], RW, default = 6'h0 
       See FIS for details.
     */
    UINT32 stat : 8;
    /* stat - Bits[15:8], RW, default = 8'h0 
       FW Mailbox Status Code. See FIS for details
     */
    UINT32 rsvd_16 : 16;
  } Bits;
  UINT32 Data;
} MB_SMBUS_STATUS_FNV_D_UNIT_0_STRUCT;




/* MB_SMBUS_OUTPUT_PAYLOAD_0_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x400048A0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_0_FNV_D_UNIT_0_REG 0x050448A0
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_0_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_1_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x400048A4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_1_FNV_D_UNIT_0_REG 0x050448A4
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_1_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_2_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x400048A8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_2_FNV_D_UNIT_0_REG 0x050448A8
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_2_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_3_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x400048AC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_3_FNV_D_UNIT_0_REG 0x050448AC
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_3_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_4_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x400048B0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_4_FNV_D_UNIT_0_REG 0x050448B0
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_4_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_5_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x400048B4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_5_FNV_D_UNIT_0_REG 0x050448B4
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_5_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_6_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x400048B8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_6_FNV_D_UNIT_0_REG 0x050448B8
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_6_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_7_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x400048BC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_7_FNV_D_UNIT_0_REG 0x050448BC
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_7_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_8_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x400048C0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_8_FNV_D_UNIT_0_REG 0x050448C0
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_8_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_9_FNV_D_UNIT_0_REG supported on:                   */
/*       EKV (0x400048C4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_9_FNV_D_UNIT_0_REG 0x050448C4
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_9_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_10_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048C8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_10_FNV_D_UNIT_0_REG 0x050448C8
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_10_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_11_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048CC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_11_FNV_D_UNIT_0_REG 0x050448CC
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_11_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_12_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048D0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_12_FNV_D_UNIT_0_REG 0x050448D0
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_12_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_13_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048D4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_13_FNV_D_UNIT_0_REG 0x050448D4
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_13_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_14_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048D8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_14_FNV_D_UNIT_0_REG 0x050448D8
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_14_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_15_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048DC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_15_FNV_D_UNIT_0_REG 0x050448DC
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_15_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_16_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048E0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_16_FNV_D_UNIT_0_REG 0x050448E0
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_16_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_17_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048E4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_17_FNV_D_UNIT_0_REG 0x050448E4
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_17_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_18_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048E8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_18_FNV_D_UNIT_0_REG 0x050448E8
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_18_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_19_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048EC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_19_FNV_D_UNIT_0_REG 0x050448EC
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_19_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_20_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048F0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_20_FNV_D_UNIT_0_REG 0x050448F0
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_20_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_21_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048F4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_21_FNV_D_UNIT_0_REG 0x050448F4
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_21_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_22_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048F8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_22_FNV_D_UNIT_0_REG 0x050448F8
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_22_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_23_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x400048FC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_23_FNV_D_UNIT_0_REG 0x050448FC
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_23_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_24_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x40004900)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_24_FNV_D_UNIT_0_REG 0x05044900
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_24_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_25_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x40004904)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_25_FNV_D_UNIT_0_REG 0x05044904
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_25_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_26_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x40004908)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_26_FNV_D_UNIT_0_REG 0x05044908
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_26_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_27_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x4000490C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_27_FNV_D_UNIT_0_REG 0x0504490C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_27_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_28_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x40004910)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_28_FNV_D_UNIT_0_REG 0x05044910
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_28_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_29_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x40004914)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_29_FNV_D_UNIT_0_REG 0x05044914
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_29_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_30_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x40004918)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_30_FNV_D_UNIT_0_REG 0x05044918
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_30_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_OUTPUT_PAYLOAD_31_FNV_D_UNIT_0_REG supported on:                  */
/*       EKV (0x4000491C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_OUTPUT_PAYLOAD_31_FNV_D_UNIT_0_REG 0x0504491C
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * These registers contains the opcode specific FW Mailbox Short Payload.
 */
typedef union {
  struct {
    UINT32 payload : 32;
    /* payload - Bits[31:0], RW, default = 32'h0 
       FW MB Payload
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_OUTPUT_PAYLOAD_31_FNV_D_UNIT_0_STRUCT;


/* MB_SMBUS_ABORT_FNV_D_UNIT_0_REG supported on:                              */
/*       EKV (0x40004920)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMBUS_ABORT_FNV_D_UNIT_0_REG 0x05044920
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * This register contains the FW Mailbox Abort.
 */
typedef union {
  struct {
    UINT32 abort : 1;
    /* abort - Bits[0:0], RW, default = 1'b0 
       FW MB Abort:[br]
                       1 = Abort Command in Command Register
     */
    UINT32 rsvd : 31;
    /* rsvd - Bits[31:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MB_SMBUS_ABORT_FNV_D_UNIT_0_STRUCT;


/* DS_DDRT_TRAINING_EN_FNV_D_UNIT_0_REG supported on:                         */
/*       EKV (0x40004928)                                                     */
/* Register default value:              0x00000000                            */
#define DS_DDRT_TRAINING_EN_FNV_D_UNIT_0_REG 0x05044928
/* Struct format extracted from XML file EKV\0.0.4.EKV.xml.
 * Training  Enable
 */
typedef union {
  struct {
    UINT32 tm_en : 1;
    /* tm_en - Bits[0:0], RW, default = 1'h0 
       DDRT Training Mode Enable (not used) [br]
                      Setting this bit will block write data from sending to memory
     */
    UINT32 early_cmd_ck_trn : 1;
    /* early_cmd_ck_trn - Bits[1:1], RW, default = 1'h0 
       EARLY CMD-CK Training Enable [br]
                      Parity only, commands are not written into ingress command queue
     */
    UINT32 adv_cmd_ck_trn : 1;
    /* adv_cmd_ck_trn - Bits[2:2], RW, default = 1'h0 
       Advanced CMD-CK Training Enable[br]
                      MRS, extended MRS and Memory Mapped config access are ignored
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DS_DDRT_TRAINING_EN_FNV_D_UNIT_0_STRUCT;


















































































































































































































































































#endif /* FNV_D_UNIT_0_h */
