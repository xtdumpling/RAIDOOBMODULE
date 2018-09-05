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

#ifndef FNV_DA_UNIT_0_h
#define FNV_DA_UNIT_0_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* FNV_DA_UNIT_0_DEV 0                                                        */
/* FNV_DA_UNIT_0_FUN 2                                                        */

/* DA_DDRT_TRAINING_MR0_FNV_DA_UNIT_0_REG supported on:                       */
/*       EKV (0x40002000)                                                     */
/* Register default value:              0x00000020                            */
#define DA_DDRT_TRAINING_MR0_FNV_DA_UNIT_0_REG 0x05024000
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * This register mimics DDR4 SRAM Mode Registers.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tcl_gnt_data_0 : 1;
    /* tcl_gnt_data_0 - Bits[2:2], RW, default = 1'h0 
       CAS Latency measured from GNT to read data (in number of dclks). Low bit.[br]
       	            0x0-0x3: 9-12 [br]
                       0x4-0x7: 13-16 [br]
                       0x8: 18[br]
                       0x9: 20[br]
                       0xA: 22[br]
                       0xB: 24[br]
                       0xC: rsvd[br]
                       0xD: 17[br]
                       0xE: 19[br]
                       0xF: 21[br]
                       Note: This register field is used with bit[6:4] of this register 
       to form TCL GNT to Data register field[br] 
                       tcl setting must satisfy FNV min latency of 12 clocks for tcl + 
       tcl_add [br] 
                       tcl + tcl_add >= 12
     */
    UINT32 rsvd_3 : 1;
    /* rsvd_3 - Bits[3:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tcl_gnt_data : 3;
    /* tcl_gnt_data - Bits[6:4], RW, default = 3'h2 
       CAS Latency measured from GNT to read data (in number of dclks). High bits.[br]
       	            0x0-0x3: 9-12 [br]
                       0x4-0x7: 13-16 [br]
                       0x8: 18[br]
                       0x9: 20[br]
                       0xA: 22[br]
                       0xB: 24[br]
                       0xC: rsvd[br]
                       0xD: 17[br]
                       0xE: 19[br]
                       0xF: 21[br]
                       Note: This is combined with bit[2] of this register to form TCL 
       GNT to Data register field [br] 
                       tcl setting must satisfy FNV min latency of 12 clocks for tcl + 
       tcl_add [br] 
                       tcl + tcl_add >= 12
     */
    UINT32 rsvd_7 : 1;
    /* rsvd_7 - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_8 : 1;
    UINT32 rsvd_9 : 23;
    /* rsvd_9 - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TRAINING_MR0_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_TRAINING_MR1_FNV_DA_UNIT_0_REG supported on:                       */
/*       EKV (0x40002004)                                                     */
/* Register default value:              0x00000000                            */
#define DA_DDRT_TRAINING_MR1_FNV_DA_UNIT_0_REG 0x05024004
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * This register mimics DDR4 SRAM Mode Registers.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 6;
    UINT32 rlvl_en : 1;
    /* rlvl_en - Bits[6:6], RW, default = 1'b0 
       Not used.
     */
    UINT32 wlvl_en : 1;
    /* wlvl_en - Bits[7:7], RW, default = 1'b0 
       Write Leveling Enable [br]
                       0 : disable
                       1 : enable
     */
    UINT32 rsvd_8 : 3;
    UINT32 rsvd_11 : 21;
    /* rsvd_11 - Bits[31:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TRAINING_MR1_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_TRAINING_MR2_FNV_DA_UNIT_0_REG supported on:                       */
/*       EKV (0x40002008)                                                     */
/* Register default value:              0x00000020                            */
#define DA_DDRT_TRAINING_MR2_FNV_DA_UNIT_0_REG 0x05024008
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * This register mimics DDR4 SRAM Mode Registers.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 3;
    /* rsvd_0 - Bits[2:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 tcwl : 3;
    /* tcwl - Bits[5:3], RW, default = 3'b100 
       CAS write latency (in number of dclks)[br]
               3'b000 : 9, tcwl_add max of 15 (FNV Does NOT Support!) [br]
       		3'b001 : 10, tcwl_add max of 15  (FNV Does NOT Support!)[br]
       		3'b010 : 11, tcwl_add max of 15  (FNV Does NOT Support!)[br]
         3'b011 : 12, tcwl_add max of 14  (Supported only with cwl_add of at least '1' 
       when par_flopen = '1')[br] 
       		3'b100 : 14, tcwl_add max of 13  [br]
       		3'b101 : 16, tcwl_add max of 11  [br]
       		3'b110 : 18, tcwl_add max of 9  [br]
       		3'b111 : RSVD [br]
               The effective write latency observed will be 12+tcwl_add+1 when CAS 
       write latency is set to 3'h3.[br] 
               Note: tcwl setting must satisfy FNV min latency of 12 clocks for tcwl + 
       tcwl_add [br] 
               tcwl + tcwl_add >= 12
     */
    UINT32 cmd_2n_en : 2;
    /* cmd_2n_en - Bits[7:6], RW, default = 2'h0 
       DDRT Command 1N/2N Timing [br]
                       00 = 1N (default)[br]
                       01 = 2N[br]
                       1x = Reserved
     */
    UINT32 rsvd_8 : 1;
    /* rsvd_8 - Bits[8:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_9 : 23;
  } Bits;
  UINT32 Data;
} DA_DDRT_TRAINING_MR2_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_TRAINING_MR3_FNV_DA_UNIT_0_REG supported on:                       */
/*       EKV (0x4000200C)                                                     */
/* Register default value:              0x00000000                            */
#define DA_DDRT_TRAINING_MR3_FNV_DA_UNIT_0_REG 0x0502400C
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * This register mimics DDR4 SRAM Mode Registers.
 */
typedef union {
  struct {
    UINT32 mpr_sel : 2;
    /* mpr_sel - Bits[1:0], RW, default = 2'h0 
       MPR page selection[br]
                       0x0 : page 0[br]
                       0x1 : page 1[br]
                       0x2 : page 2[br]
                       0x3 : page 3
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
    UINT32 rsvd_4 : 1;
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
} DA_DDRT_TRAINING_MR3_FNV_DA_UNIT_0_STRUCT;










/* DA_DDRT_MPR3_PAGE0_FNV_DA_UNIT_0_REG supported on:                         */
/*       EKV (0x40002020)                                                     */
/* Register default value:              0x000F3355                            */
#define DA_DDRT_MPR3_PAGE0_FNV_DA_UNIT_0_REG 0x05024020
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Training Pattern
 */
typedef union {
  struct {
    UINT32 mpr0 : 8;
    /* mpr0 - Bits[7:0], RW, default = 8'h55 
       MPR training pattern 3 (from ddrt_training_mpr3)
     */
    UINT32 mpr1 : 8;
    /* mpr1 - Bits[15:8], RW, default = 8'h33 
       MPR training pattern 1 (from ddrt_training_mpr1)
     */
    UINT32 mpr2 : 8;
    /* mpr2 - Bits[23:16], RW, default = 8'h0F 
       MPR training pattern 2 (from ddrt_training_mpr2)
     */
    UINT32 mpr3 : 8;
    /* mpr3 - Bits[31:24], RW, default = 8'h00 
       MPR training pattern 3 (from ddrt_training_mpr3)
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_MPR3_PAGE0_FNV_DA_UNIT_0_STRUCT;








/* DA_DDRT_TRAINING_EN_FNV_DA_UNIT_0_REG supported on:                        */
/*       EKV (0x40002030)                                                     */
/* Register default value:              0x00000000                            */
#define DA_DDRT_TRAINING_EN_FNV_DA_UNIT_0_REG 0x05024030
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Training Enable
 */
typedef union {
  struct {
    UINT32 tm_en : 1;
    /* tm_en - Bits[0:0], RW, default = 1'h0 
       FMC general training mode - [br]
       		       needs to be set for all training steps. [br]
       			   Setting this bit will block read and write command from sending to memory
     */
    UINT32 early_cmd_ck_trn : 1;
    /* early_cmd_ck_trn - Bits[1:1], RW, default = 1'h0 
       FMC parity-only training mode. [br]
                      Does not submit commands to ingress queue - only computes parity
     */
    UINT32 adv_cmd_ck_trn : 1;
    /* adv_cmd_ck_trn - Bits[2:2], RW, default = 1'h0 
       FMC block CSR writes training mode.[br]
                      Used during advanced cmd-clk training. Protects the FMC states 
       from being changed as CMD/CTL signals are margined.[br] 
       			   MRS, extended MRS and Memory Mapped config access are ignored
     */
    UINT32 basic_erid_trn : 1;
    /* basic_erid_trn - Bits[3:3], RW, default = 1'h0 
       This enables the generation of this constant pattern sourced from FMC [br]
       		       during early read ID Fine Training
     */
    UINT32 recen_dq_dqs_mpr_trn : 1;
    /* recen_dq_dqs_mpr_trn - Bits[4:4], RW, default = 1'h0 
       RECEN DQ DQS using MPR Training Enable [br]
                      MPR pattern is returned for GNT even when REQ was not asserted 
       [br] 
       			   and no Read Command was originally sent.
     */
    UINT32 disable_rid_feedback : 1;
    /* disable_rid_feedback - Bits[5:5], RW, default = 1'h0 
       When set, the RID bits are no longer returned within the data packet, 
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
} DA_DDRT_TRAINING_EN_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_TRAINING_OTHER_EN_FNV_DA_UNIT_0_REG supported on:                  */
/*       EKV (0x40002034)                                                     */
/* Register default value:              0x00000000                            */
#define DA_DDRT_TRAINING_OTHER_EN_FNV_DA_UNIT_0_REG 0x05024034
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Other Training Enable
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 2;
    /* rsvd_0 - Bits[1:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 clkadj_en : 1;
    /* clkadj_en - Bits[2:2], RW, default = 1'h0 
       Enable IO to core dclk ajustment[br]
                      Set this bit to enable io to core dclk CK adjustment FSM
     */
    UINT32 rsvd_3 : 2;
    /* rsvd_3 - Bits[4:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 reset_trn : 1;
    /* reset_trn - Bits[5:5], RW, default = 1'h0 
       Reset Training Enable [br]
                      Set this bit to reset training FSM TBD
     */
    UINT32 rsvd_6 : 26;
    /* rsvd_6 - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TRAINING_OTHER_EN_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_RX_DQ_SWZ0_FNV_DA_UNIT_0_REG supported on:                         */
/*       EKV (0x40002038)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DA_DDRT_RX_DQ_SWZ0_FNV_DA_UNIT_0_REG 0x05024038
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * DQ15 through DQ0 Swizzle
 */
typedef union {
  struct {
    UINT32 dq0_swz_sel : 2;
    /* dq0_swz_sel - Bits[1:0], RW, default = 2'h0 
       RX DQ0 swizzling[br]
                       00 = IO dq0 to core dq0[br]
                       01 = IO dq1 to core dq0[br]
                       10 = IO dq2 to core dq0[br]
                       11 = IO dq3 to core dq0
     */
    UINT32 dq1_swz_sel : 2;
    /* dq1_swz_sel - Bits[3:2], RW, default = 2'h1 
       DQ1 swizzling
     */
    UINT32 dq2_swz_sel : 2;
    /* dq2_swz_sel - Bits[5:4], RW, default = 2'h2 
       DQ2 swizzling
     */
    UINT32 dq3_swz_sel : 2;
    /* dq3_swz_sel - Bits[7:6], RW, default = 2'h3 
       DQ3 swizzling
     */
    UINT32 dq4_swz_sel : 2;
    /* dq4_swz_sel - Bits[9:8], RW, default = 2'h0 
       DQ4 swizzling
     */
    UINT32 dq5_swz_sel : 2;
    /* dq5_swz_sel - Bits[11:10], RW, default = 2'h1 
       DQ5 swizzling
     */
    UINT32 dq6_swz_sel : 2;
    /* dq6_swz_sel - Bits[13:12], RW, default = 2'h2 
       DQ6 swizzling
     */
    UINT32 dq7_swz_sel : 2;
    /* dq7_swz_sel - Bits[15:14], RW, default = 2'h3 
       DQ7 swizzling
     */
    UINT32 dq8_swz_sel : 2;
    /* dq8_swz_sel - Bits[17:16], RW, default = 2'h0 
       DQ8 swizzling
     */
    UINT32 dq9_swz_sel : 2;
    /* dq9_swz_sel - Bits[19:18], RW, default = 2'h1 
       DQ9 swizzling
     */
    UINT32 dq10_swz_sel : 2;
    /* dq10_swz_sel - Bits[21:20], RW, default = 2'h2 
       DQ10 swizzling
     */
    UINT32 dq11_swz_sel : 2;
    /* dq11_swz_sel - Bits[23:22], RW, default = 2'h3 
       DQ11 swizzling
     */
    UINT32 dq12_swz_sel : 2;
    /* dq12_swz_sel - Bits[25:24], RW, default = 2'h0 
       DQ12 swizzling
     */
    UINT32 dq13_swz_sel : 2;
    /* dq13_swz_sel - Bits[27:26], RW, default = 2'h1 
       DQ13 swizzling
     */
    UINT32 dq14_swz_sel : 2;
    /* dq14_swz_sel - Bits[29:28], RW, default = 2'h2 
       DQ14 swizzling
     */
    UINT32 dq15_swz_sel : 2;
    /* dq15_swz_sel - Bits[31:30], RW, default = 2'h3 
       DQ15 swizzling
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_RX_DQ_SWZ0_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_RX_DQ_SWZ1_FNV_DA_UNIT_0_REG supported on:                         */
/*       EKV (0x4000203C)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DA_DDRT_RX_DQ_SWZ1_FNV_DA_UNIT_0_REG 0x0502403C
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * DQ31 through DQ16 Swizzle
 */
typedef union {
  struct {
    UINT32 dq16_swz_sel : 2;
    /* dq16_swz_sel - Bits[1:0], RW, default = 2'h0 
       DQ16 swizzling
     */
    UINT32 dq17_swz_sel : 2;
    /* dq17_swz_sel - Bits[3:2], RW, default = 2'h1 
       DQ17 swizzling
     */
    UINT32 dq18_swz_sel : 2;
    /* dq18_swz_sel - Bits[5:4], RW, default = 2'h2 
       DQ18 swizzling
     */
    UINT32 dq19_swz_sel : 2;
    /* dq19_swz_sel - Bits[7:6], RW, default = 2'h3 
       DQ19 swizzling
     */
    UINT32 dq20_swz_sel : 2;
    /* dq20_swz_sel - Bits[9:8], RW, default = 2'h0 
       DQ20 swizzling
     */
    UINT32 dq21_swz_sel : 2;
    /* dq21_swz_sel - Bits[11:10], RW, default = 2'h1 
       DQ21 swizzling
     */
    UINT32 dq22_swz_sel : 2;
    /* dq22_swz_sel - Bits[13:12], RW, default = 2'h2 
       DQ22 swizzling
     */
    UINT32 dq23_swz_sel : 2;
    /* dq23_swz_sel - Bits[15:14], RW, default = 2'h3 
       DQ23 swizzling
     */
    UINT32 dq24_swz_sel : 2;
    /* dq24_swz_sel - Bits[17:16], RW, default = 2'h0 
       DQ24 swizzling
     */
    UINT32 dq25_swz_sel : 2;
    /* dq25_swz_sel - Bits[19:18], RW, default = 2'h1 
       DQ25 swizzling
     */
    UINT32 dq26_swz_sel : 2;
    /* dq26_swz_sel - Bits[21:20], RW, default = 2'h2 
       DQ26 swizzling
     */
    UINT32 dq27_swz_sel : 2;
    /* dq27_swz_sel - Bits[23:22], RW, default = 2'h3 
       DQ27 swizzling
     */
    UINT32 dq28_swz_sel : 2;
    /* dq28_swz_sel - Bits[25:24], RW, default = 2'h0 
       DQ28 swizzling
     */
    UINT32 dq29_swz_sel : 2;
    /* dq29_swz_sel - Bits[27:26], RW, default = 2'h1 
       DQ29 swizzling
     */
    UINT32 dq30_swz_sel : 2;
    /* dq30_swz_sel - Bits[29:28], RW, default = 2'h2 
       DQ30 swizzling
     */
    UINT32 dq31_swz_sel : 2;
    /* dq31_swz_sel - Bits[31:30], RW, default = 2'h3 
       DQ31 swizzling
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_RX_DQ_SWZ1_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_RX_DQ_SWZ2_FNV_DA_UNIT_0_REG supported on:                         */
/*       EKV (0x40002040)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DA_DDRT_RX_DQ_SWZ2_FNV_DA_UNIT_0_REG 0x05024040
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * DQ47 through DQ32 Swizzle
 */
typedef union {
  struct {
    UINT32 dq32_swz_sel : 2;
    /* dq32_swz_sel - Bits[1:0], RW, default = 2'h0 
       DQ32 swizzling
     */
    UINT32 dq33_swz_sel : 2;
    /* dq33_swz_sel - Bits[3:2], RW, default = 2'h1 
       DQ33 swizzling
     */
    UINT32 dq34_swz_sel : 2;
    /* dq34_swz_sel - Bits[5:4], RW, default = 2'h2 
       DQ34 swizzling
     */
    UINT32 dq35_swz_sel : 2;
    /* dq35_swz_sel - Bits[7:6], RW, default = 2'h3 
       DQ35 swizzling
     */
    UINT32 dq36_swz_sel : 2;
    /* dq36_swz_sel - Bits[9:8], RW, default = 2'h0 
       DQ36 swizzling
     */
    UINT32 dq37_swz_sel : 2;
    /* dq37_swz_sel - Bits[11:10], RW, default = 2'h1 
       DQ37 swizzling
     */
    UINT32 dq38_swz_sel : 2;
    /* dq38_swz_sel - Bits[13:12], RW, default = 2'h2 
       DQ38 swizzling
     */
    UINT32 dq39_swz_sel : 2;
    /* dq39_swz_sel - Bits[15:14], RW, default = 2'h3 
       DQ39 swizzling
     */
    UINT32 dq40_swz_sel : 2;
    /* dq40_swz_sel - Bits[17:16], RW, default = 2'h0 
       DQ40 swizzling
     */
    UINT32 dq41_swz_sel : 2;
    /* dq41_swz_sel - Bits[19:18], RW, default = 2'h1 
       DQ41 swizzling
     */
    UINT32 dq42_swz_sel : 2;
    /* dq42_swz_sel - Bits[21:20], RW, default = 2'h2 
       DQ42 swizzling
     */
    UINT32 dq43_swz_sel : 2;
    /* dq43_swz_sel - Bits[23:22], RW, default = 2'h3 
       DQ43 swizzling
     */
    UINT32 dq44_swz_sel : 2;
    /* dq44_swz_sel - Bits[25:24], RW, default = 2'h0 
       DQ44 swizzling
     */
    UINT32 dq45_swz_sel : 2;
    /* dq45_swz_sel - Bits[27:26], RW, default = 2'h1 
       DQ45 swizzling
     */
    UINT32 dq46_swz_sel : 2;
    /* dq46_swz_sel - Bits[29:28], RW, default = 2'h2 
       DQ46 swizzling
     */
    UINT32 dq47_swz_sel : 2;
    /* dq47_swz_sel - Bits[31:30], RW, default = 2'h3 
       DQ47 swizzling
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_RX_DQ_SWZ2_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_RX_DQ_SWZ3_FNV_DA_UNIT_0_REG supported on:                         */
/*       EKV (0x40002044)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DA_DDRT_RX_DQ_SWZ3_FNV_DA_UNIT_0_REG 0x05024044
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * DQ63 through DQ48 Swizzle
 */
typedef union {
  struct {
    UINT32 dq48_swz_sel : 2;
    /* dq48_swz_sel - Bits[1:0], RW, default = 2'h0 
       DQ48 swizzling
     */
    UINT32 dq49_swz_sel : 2;
    /* dq49_swz_sel - Bits[3:2], RW, default = 2'h1 
       DQ49 swizzling
     */
    UINT32 dq50_swz_sel : 2;
    /* dq50_swz_sel - Bits[5:4], RW, default = 2'h2 
       DQ50 swizzling
     */
    UINT32 dq51_swz_sel : 2;
    /* dq51_swz_sel - Bits[7:6], RW, default = 2'h3 
       DQ51 swizzling
     */
    UINT32 dq52_swz_sel : 2;
    /* dq52_swz_sel - Bits[9:8], RW, default = 2'h0 
       DQ52 swizzling
     */
    UINT32 dq53_swz_sel : 2;
    /* dq53_swz_sel - Bits[11:10], RW, default = 2'h1 
       DQ53 swizzling
     */
    UINT32 dq54_swz_sel : 2;
    /* dq54_swz_sel - Bits[13:12], RW, default = 2'h2 
       DQ54 swizzling
     */
    UINT32 dq55_swz_sel : 2;
    /* dq55_swz_sel - Bits[15:14], RW, default = 2'h3 
       DQ55 swizzling
     */
    UINT32 dq56_swz_sel : 2;
    /* dq56_swz_sel - Bits[17:16], RW, default = 2'h0 
       DQ56 swizzling
     */
    UINT32 dq57_swz_sel : 2;
    /* dq57_swz_sel - Bits[19:18], RW, default = 2'h1 
       DQ57 swizzling
     */
    UINT32 dq58_swz_sel : 2;
    /* dq58_swz_sel - Bits[21:20], RW, default = 2'h2 
       DQ58 swizzling
     */
    UINT32 dq59_swz_sel : 2;
    /* dq59_swz_sel - Bits[23:22], RW, default = 2'h3 
       DQ59 swizzling
     */
    UINT32 dq60_swz_sel : 2;
    /* dq60_swz_sel - Bits[25:24], RW, default = 2'h0 
       DQ60 swizzling
     */
    UINT32 dq61_swz_sel : 2;
    /* dq61_swz_sel - Bits[27:26], RW, default = 2'h1 
       DQ61 swizzling
     */
    UINT32 dq62_swz_sel : 2;
    /* dq62_swz_sel - Bits[29:28], RW, default = 2'h2 
       DQ62 swizzling
     */
    UINT32 dq63_swz_sel : 2;
    /* dq63_swz_sel - Bits[31:30], RW, default = 2'h3 
       DQ63 swizzling
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_RX_DQ_SWZ3_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_RX_DQ_SWZ4_FNV_DA_UNIT_0_REG supported on:                         */
/*       EKV (0x40002048)                                                     */
/* Register default value:              0x0000E4E4                            */
#define DA_DDRT_RX_DQ_SWZ4_FNV_DA_UNIT_0_REG 0x05024048
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * DQ71 through DQ64 Swizzle
 */
typedef union {
  struct {
    UINT32 dq64_swz_sel : 2;
    /* dq64_swz_sel - Bits[1:0], RW, default = 2'h0 
       DQ64 swizzling
     */
    UINT32 dq65_swz_sel : 2;
    /* dq65_swz_sel - Bits[3:2], RW, default = 2'h1 
       DQ65 swizzling
     */
    UINT32 dq66_swz_sel : 2;
    /* dq66_swz_sel - Bits[5:4], RW, default = 2'h2 
       DQ66 swizzling
     */
    UINT32 dq67_swz_sel : 2;
    /* dq67_swz_sel - Bits[7:6], RW, default = 2'h3 
       DQ67 swizzling
     */
    UINT32 dq68_swz_sel : 2;
    /* dq68_swz_sel - Bits[9:8], RW, default = 2'h0 
       DQ68 swizzling
     */
    UINT32 dq69_swz_sel : 2;
    /* dq69_swz_sel - Bits[11:10], RW, default = 2'h1 
       DQ69 swizzling
     */
    UINT32 dq70_swz_sel : 2;
    /* dq70_swz_sel - Bits[13:12], RW, default = 2'h2 
       DQ70 swizzling
     */
    UINT32 dq71_swz_sel : 2;
    /* dq71_swz_sel - Bits[15:14], RW, default = 2'h3 
       DQ71 swizzling
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_RX_DQ_SWZ4_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_TX_DQ_SWZ0_FNV_DA_UNIT_0_REG supported on:                         */
/*       EKV (0x4000204C)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DA_DDRT_TX_DQ_SWZ0_FNV_DA_UNIT_0_REG 0x0502404C
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * DQ15 through DQ0 Swizzle
 */
typedef union {
  struct {
    UINT32 dq0_swz_sel : 2;
    /* dq0_swz_sel - Bits[1:0], RW, default = 2'h0 
       RX DQ0 swizzling[br]
                       00 = IO dq0 to core dq0[br]
                       01 = IO dq1 to core dq0[br]
                       10 = IO dq2 to core dq0[br]
                       11 = IO dq3 to core dq0
     */
    UINT32 dq1_swz_sel : 2;
    /* dq1_swz_sel - Bits[3:2], RW, default = 2'h1 
       DQ1 swizzling
     */
    UINT32 dq2_swz_sel : 2;
    /* dq2_swz_sel - Bits[5:4], RW, default = 2'h2 
       DQ2 swizzling
     */
    UINT32 dq3_swz_sel : 2;
    /* dq3_swz_sel - Bits[7:6], RW, default = 2'h3 
       DQ3 swizzling
     */
    UINT32 dq4_swz_sel : 2;
    /* dq4_swz_sel - Bits[9:8], RW, default = 2'h0 
       DQ4 swizzling
     */
    UINT32 dq5_swz_sel : 2;
    /* dq5_swz_sel - Bits[11:10], RW, default = 2'h1 
       DQ5 swizzling
     */
    UINT32 dq6_swz_sel : 2;
    /* dq6_swz_sel - Bits[13:12], RW, default = 2'h2 
       DQ6 swizzling
     */
    UINT32 dq7_swz_sel : 2;
    /* dq7_swz_sel - Bits[15:14], RW, default = 2'h3 
       DQ7 swizzling
     */
    UINT32 dq8_swz_sel : 2;
    /* dq8_swz_sel - Bits[17:16], RW, default = 2'h0 
       DQ8 swizzling
     */
    UINT32 dq9_swz_sel : 2;
    /* dq9_swz_sel - Bits[19:18], RW, default = 2'h1 
       DQ9 swizzling
     */
    UINT32 dq10_swz_sel : 2;
    /* dq10_swz_sel - Bits[21:20], RW, default = 2'h2 
       DQ10 swizzling
     */
    UINT32 dq11_swz_sel : 2;
    /* dq11_swz_sel - Bits[23:22], RW, default = 2'h3 
       DQ11 swizzling
     */
    UINT32 dq12_swz_sel : 2;
    /* dq12_swz_sel - Bits[25:24], RW, default = 2'h0 
       DQ12 swizzling
     */
    UINT32 dq13_swz_sel : 2;
    /* dq13_swz_sel - Bits[27:26], RW, default = 2'h1 
       DQ13 swizzling
     */
    UINT32 dq14_swz_sel : 2;
    /* dq14_swz_sel - Bits[29:28], RW, default = 2'h2 
       DQ14 swizzling
     */
    UINT32 dq15_swz_sel : 2;
    /* dq15_swz_sel - Bits[31:30], RW, default = 2'h3 
       DQ15 swizzling
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TX_DQ_SWZ0_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_TX_DQ_SWZ1_FNV_DA_UNIT_0_REG supported on:                         */
/*       EKV (0x40002050)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DA_DDRT_TX_DQ_SWZ1_FNV_DA_UNIT_0_REG 0x05024050
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * DQ31 through DQ16 Swizzle
 */
typedef union {
  struct {
    UINT32 dq16_swz_sel : 2;
    /* dq16_swz_sel - Bits[1:0], RW, default = 2'h0 
       DQ16 swizzling
     */
    UINT32 dq17_swz_sel : 2;
    /* dq17_swz_sel - Bits[3:2], RW, default = 2'h1 
       DQ17 swizzling
     */
    UINT32 dq18_swz_sel : 2;
    /* dq18_swz_sel - Bits[5:4], RW, default = 2'h2 
       DQ18 swizzling
     */
    UINT32 dq19_swz_sel : 2;
    /* dq19_swz_sel - Bits[7:6], RW, default = 2'h3 
       DQ19 swizzling
     */
    UINT32 dq20_swz_sel : 2;
    /* dq20_swz_sel - Bits[9:8], RW, default = 2'h0 
       DQ20 swizzling
     */
    UINT32 dq21_swz_sel : 2;
    /* dq21_swz_sel - Bits[11:10], RW, default = 2'h1 
       DQ21 swizzling
     */
    UINT32 dq22_swz_sel : 2;
    /* dq22_swz_sel - Bits[13:12], RW, default = 2'h2 
       DQ22 swizzling
     */
    UINT32 dq23_swz_sel : 2;
    /* dq23_swz_sel - Bits[15:14], RW, default = 2'h3 
       DQ23 swizzling
     */
    UINT32 dq24_swz_sel : 2;
    /* dq24_swz_sel - Bits[17:16], RW, default = 2'h0 
       DQ24 swizzling
     */
    UINT32 dq25_swz_sel : 2;
    /* dq25_swz_sel - Bits[19:18], RW, default = 2'h1 
       DQ25 swizzling
     */
    UINT32 dq26_swz_sel : 2;
    /* dq26_swz_sel - Bits[21:20], RW, default = 2'h2 
       DQ26 swizzling
     */
    UINT32 dq27_swz_sel : 2;
    /* dq27_swz_sel - Bits[23:22], RW, default = 2'h3 
       DQ27 swizzling
     */
    UINT32 dq28_swz_sel : 2;
    /* dq28_swz_sel - Bits[25:24], RW, default = 2'h0 
       DQ28 swizzling
     */
    UINT32 dq29_swz_sel : 2;
    /* dq29_swz_sel - Bits[27:26], RW, default = 2'h1 
       DQ29 swizzling
     */
    UINT32 dq30_swz_sel : 2;
    /* dq30_swz_sel - Bits[29:28], RW, default = 2'h2 
       DQ30 swizzling
     */
    UINT32 dq31_swz_sel : 2;
    /* dq31_swz_sel - Bits[31:30], RW, default = 2'h3 
       DQ31 swizzling
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TX_DQ_SWZ1_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_TX_DQ_SWZ2_FNV_DA_UNIT_0_REG supported on:                         */
/*       EKV (0x40002054)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DA_DDRT_TX_DQ_SWZ2_FNV_DA_UNIT_0_REG 0x05024054
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * DQ47 through DQ32 Swizzle
 */
typedef union {
  struct {
    UINT32 dq32_swz_sel : 2;
    /* dq32_swz_sel - Bits[1:0], RW, default = 2'h0 
       DQ32 swizzling
     */
    UINT32 dq33_swz_sel : 2;
    /* dq33_swz_sel - Bits[3:2], RW, default = 2'h1 
       DQ33 swizzling
     */
    UINT32 dq34_swz_sel : 2;
    /* dq34_swz_sel - Bits[5:4], RW, default = 2'h2 
       DQ34 swizzling
     */
    UINT32 dq35_swz_sel : 2;
    /* dq35_swz_sel - Bits[7:6], RW, default = 2'h3 
       DQ35 swizzling
     */
    UINT32 dq36_swz_sel : 2;
    /* dq36_swz_sel - Bits[9:8], RW, default = 2'h0 
       DQ36 swizzling
     */
    UINT32 dq37_swz_sel : 2;
    /* dq37_swz_sel - Bits[11:10], RW, default = 2'h1 
       DQ37 swizzling
     */
    UINT32 dq38_swz_sel : 2;
    /* dq38_swz_sel - Bits[13:12], RW, default = 2'h2 
       DQ38 swizzling
     */
    UINT32 dq39_swz_sel : 2;
    /* dq39_swz_sel - Bits[15:14], RW, default = 2'h3 
       DQ39 swizzling
     */
    UINT32 dq40_swz_sel : 2;
    /* dq40_swz_sel - Bits[17:16], RW, default = 2'h0 
       DQ40 swizzling
     */
    UINT32 dq41_swz_sel : 2;
    /* dq41_swz_sel - Bits[19:18], RW, default = 2'h1 
       DQ41 swizzling
     */
    UINT32 dq42_swz_sel : 2;
    /* dq42_swz_sel - Bits[21:20], RW, default = 2'h2 
       DQ42 swizzling
     */
    UINT32 dq43_swz_sel : 2;
    /* dq43_swz_sel - Bits[23:22], RW, default = 2'h3 
       DQ43 swizzling
     */
    UINT32 dq44_swz_sel : 2;
    /* dq44_swz_sel - Bits[25:24], RW, default = 2'h0 
       DQ44 swizzling
     */
    UINT32 dq45_swz_sel : 2;
    /* dq45_swz_sel - Bits[27:26], RW, default = 2'h1 
       DQ45 swizzling
     */
    UINT32 dq46_swz_sel : 2;
    /* dq46_swz_sel - Bits[29:28], RW, default = 2'h2 
       DQ46 swizzling
     */
    UINT32 dq47_swz_sel : 2;
    /* dq47_swz_sel - Bits[31:30], RW, default = 2'h3 
       DQ47 swizzling
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TX_DQ_SWZ2_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_TX_DQ_SWZ3_FNV_DA_UNIT_0_REG supported on:                         */
/*       EKV (0x40002058)                                                     */
/* Register default value:              0xE4E4E4E4                            */
#define DA_DDRT_TX_DQ_SWZ3_FNV_DA_UNIT_0_REG 0x05024058
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * DQ63 through DQ48 Swizzle
 */
typedef union {
  struct {
    UINT32 dq48_swz_sel : 2;
    /* dq48_swz_sel - Bits[1:0], RW, default = 2'h0 
       DQ48 swizzling
     */
    UINT32 dq49_swz_sel : 2;
    /* dq49_swz_sel - Bits[3:2], RW, default = 2'h1 
       DQ49 swizzling
     */
    UINT32 dq50_swz_sel : 2;
    /* dq50_swz_sel - Bits[5:4], RW, default = 2'h2 
       DQ50 swizzling
     */
    UINT32 dq51_swz_sel : 2;
    /* dq51_swz_sel - Bits[7:6], RW, default = 2'h3 
       DQ51 swizzling
     */
    UINT32 dq52_swz_sel : 2;
    /* dq52_swz_sel - Bits[9:8], RW, default = 2'h0 
       DQ52 swizzling
     */
    UINT32 dq53_swz_sel : 2;
    /* dq53_swz_sel - Bits[11:10], RW, default = 2'h1 
       DQ53 swizzling
     */
    UINT32 dq54_swz_sel : 2;
    /* dq54_swz_sel - Bits[13:12], RW, default = 2'h2 
       DQ54 swizzling
     */
    UINT32 dq55_swz_sel : 2;
    /* dq55_swz_sel - Bits[15:14], RW, default = 2'h3 
       DQ55 swizzling
     */
    UINT32 dq56_swz_sel : 2;
    /* dq56_swz_sel - Bits[17:16], RW, default = 2'h0 
       DQ56 swizzling
     */
    UINT32 dq57_swz_sel : 2;
    /* dq57_swz_sel - Bits[19:18], RW, default = 2'h1 
       DQ57 swizzling
     */
    UINT32 dq58_swz_sel : 2;
    /* dq58_swz_sel - Bits[21:20], RW, default = 2'h2 
       DQ58 swizzling
     */
    UINT32 dq59_swz_sel : 2;
    /* dq59_swz_sel - Bits[23:22], RW, default = 2'h3 
       DQ59 swizzling
     */
    UINT32 dq60_swz_sel : 2;
    /* dq60_swz_sel - Bits[25:24], RW, default = 2'h0 
       DQ60 swizzling
     */
    UINT32 dq61_swz_sel : 2;
    /* dq61_swz_sel - Bits[27:26], RW, default = 2'h1 
       DQ61 swizzling
     */
    UINT32 dq62_swz_sel : 2;
    /* dq62_swz_sel - Bits[29:28], RW, default = 2'h2 
       DQ62 swizzling
     */
    UINT32 dq63_swz_sel : 2;
    /* dq63_swz_sel - Bits[31:30], RW, default = 2'h3 
       DQ63 swizzling
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TX_DQ_SWZ3_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_TX_DQ_SWZ4_FNV_DA_UNIT_0_REG supported on:                         */
/*       EKV (0x4000205C)                                                     */
/* Register default value:              0x0000E4E4                            */
#define DA_DDRT_TX_DQ_SWZ4_FNV_DA_UNIT_0_REG 0x0502405C
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * DQ71 through DQ64 Swizzle
 */
typedef union {
  struct {
    UINT32 dq64_swz_sel : 2;
    /* dq64_swz_sel - Bits[1:0], RW, default = 2'h0 
       DQ64 swizzling
     */
    UINT32 dq65_swz_sel : 2;
    /* dq65_swz_sel - Bits[3:2], RW, default = 2'h1 
       DQ65 swizzling
     */
    UINT32 dq66_swz_sel : 2;
    /* dq66_swz_sel - Bits[5:4], RW, default = 2'h2 
       DQ66 swizzling
     */
    UINT32 dq67_swz_sel : 2;
    /* dq67_swz_sel - Bits[7:6], RW, default = 2'h3 
       DQ67 swizzling
     */
    UINT32 dq68_swz_sel : 2;
    /* dq68_swz_sel - Bits[9:8], RW, default = 2'h0 
       DQ68 swizzling
     */
    UINT32 dq69_swz_sel : 2;
    /* dq69_swz_sel - Bits[11:10], RW, default = 2'h1 
       DQ69 swizzling
     */
    UINT32 dq70_swz_sel : 2;
    /* dq70_swz_sel - Bits[13:12], RW, default = 2'h2 
       DQ70 swizzling
     */
    UINT32 dq71_swz_sel : 2;
    /* dq71_swz_sel - Bits[15:14], RW, default = 2'h3 
       DQ71 swizzling
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TX_DQ_SWZ4_FNV_DA_UNIT_0_STRUCT;








/* DA_DDRIO_INIT_CONTROL_FNV_DA_UNIT_0_REG supported on:                      */
/*       EKV (0x4000206C)                                                     */
/* Register default value:              0x00684000                            */
#define DA_DDRIO_INIT_CONTROL_FNV_DA_UNIT_0_REG 0x0502406C
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Connects to DDR IO Interface Initialization 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 9;
    /* rsvd_0 - Bits[8:0], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_9 : 2;
    UINT32 rsvd_11 : 1;
    /* rsvd_11 - Bits[11:11], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_12 : 9;
    UINT32 ddrtio_par_flopen : 1;
    /* ddrtio_par_flopen - Bits[21:21], RW, default = 1'b1 
       When this is set (1'b1 by default), Command Parity Bit path across FNVIO has an 
       extra dclk latency impact. 
     */
    UINT32 rsvd_22 : 10;
  } Bits;
  UINT32 Data;
} DA_DDRIO_INIT_CONTROL_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRIO_CMDEN_FNV_DA_UNIT_0_REG supported on:                             */
/*       EKV (0x40002070)                                                     */
/* Register default value:              0x800000C0                            */
#define DA_DDRIO_CMDEN_FNV_DA_UNIT_0_REG 0x05024070
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Output Enable for REQ, ERR, ERID, BCOM, BCLK, BCKE, BODT.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 6;
    UINT32 fnvbcomflopen : 1;
    /* fnvbcomflopen - Bits[6:6], RW, default = 1'h1 
       Enables an additional Flops (2dclks) in BCOM latency for RD & WR bcom only.[br]
               BCOM Flop enable should be set to 1 to use EKV Core bcom logic. [br]
       	When set to 0, Bcom RD&WR only will use FNVIO BCOM (save 2dclks).
     */
    UINT32 rsvd_7: 25; 
  } Bits;
  UINT32 Data;
} DA_DDRIO_CMDEN_FNV_DA_UNIT_0_STRUCT;






/* DA_ECC_ENABLE_FNV_DA_UNIT_0_REG supported on:                              */
/*       EKV (0x4000207C)                                                     */
/* Register default value:              0x00001F00                            */
#define DA_ECC_ENABLE_FNV_DA_UNIT_0_REG 0x0502407C
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Write ECC Control
 */
typedef union {
  struct {
    UINT32 ecc_wr_chk_en : 1;
    /* ecc_wr_chk_en - Bits[0:0], RW, default = 1'h0 
       Enable write ECC generation[br]
                       0 = ecc check disabled[br]
                       1 = ecc check enabled
     */
    UINT32 ecc_corr_en : 1;
    /* ecc_corr_en - Bits[1:1], RW, default = 1'h0 
       Enable Write ECC Correction[br]
                       0 = ecc correction disabled.[br]
                       1 = ecc correction enabled.[br]
       		ECC Correction must be enabled to run Flow B. [br]
       		ECC Correcation can be disabled to run Flow A. [br]
     */
    UINT32 rsvd_2 : 6;
    /* rsvd_2 - Bits[7:2], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 trnreseterrexit_en : 1;
    /* trnreseterrexit_en - Bits[8:8], RW, default = 1'h1 
       Enable assertion of DDRT IO Train Reset on DDRT Err Exit for progammable number 
       of dclks[br] 
                       0 = disabled.[br]
                       1 = enabled. [br]
     */
    UINT32 trnreseterrexit_cntr_termval : 7;
    /* trnreseterrexit_cntr_termval - Bits[15:9], RW, default = 7'h0f 
       Determines the pulse length of DDRT IO Train Reset on DDRT Err Exit for 
       progammable number of dclks[br] 
                       default - 16 dclks .[br]
     */
    UINT32 rsvd_16 : 13;
    /* rsvd_16 - Bits[28:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 errflow_a_alwayscorr : 1;
    /* errflow_a_alwayscorr - Bits[29:29], RW, default = 1'h0 
       With this bit set, HW will always correct Write ECC Error in Flow A, if error is 
       correctable[br] 
                       0 = WECC Flow A Always Correct disabled[br]
                       1 = WECC Flow A Always Correct  enabled[br]
       		Note** Field is Deprecated. Do not change value from default
     */
    UINT32 ecc_poison_en : 1;
    /* ecc_poison_en - Bits[30:30], RW, default = 1'h0 
       With this bit set, HW will poison on WECC Correction Fail[br]
                       0 = WECC Poison disabled[br]
                       1 = WECC Poison  enabled
     */
    UINT32 errflow_a_exitcorr : 1;
    /* errflow_a_exitcorr - Bits[31:31], RW, default = 1'h0 
       Enable Write ECC Flow A Correction on Retry Flow Exit[br]
                       0 = ecc Flow A correction on retry flow exit disabled[br]
                       1 = ecc Flow A correction on retry flow exit enabled
     */
  } Bits;
  UINT32 Data;
} DA_ECC_ENABLE_FNV_DA_UNIT_0_STRUCT;


















/* DA_DDRT_TRAINING_RC06_FNV_DA_UNIT_0_REG supported on:                      */
/*       EKV (0x400020A0)                                                     */
/* Register default value:              0x00000000                            */
#define DA_DDRT_TRAINING_RC06_FNV_DA_UNIT_0_REG 0x050240A0
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Command Space Control Word
 */
typedef union {
  struct {
    UINT32 misc_ctrl : 4;
    /* misc_ctrl - Bits[3:0], RW, default = 4'h0 
       0000 = reserved (SoftReset)[br]
                       0001 = DB Reset [br]
                       0010 = DRAM Reset[br]
                       0011 = Clear DRAM Reset [br]
                       0100 = CW Read operation [br]
                       0101 = CW Write operation [br]
                       0110 = Clear parity error[br]
                       0111 = reserved (Geardown Mode delay CS)[br]
                       1000 = reserved (Geardown mode entry )[br]
                       others = reserved 
     */
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TRAINING_RC06_FNV_DA_UNIT_0_STRUCT;












/* DA_DDRT_TRAINING_RC0C_FNV_DA_UNIT_0_REG supported on:                      */
/*       EKV (0x400020B8)                                                     */
/* Register default value:              0x00000000                            */
#define DA_DDRT_TRAINING_RC0C_FNV_DA_UNIT_0_REG 0x050240B8
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Training Control Word
 */
typedef union {
  struct {
    UINT32 training_mode : 3;
    /* training_mode - Bits[2:0], RW, default = 3'h0 
       Training mode selection[br]
                       000 = Normal operating [br]
                       001 = CK-CA training[br]
                       010 = CS0_n loopback[br]
                       011 = GNT loopback (CS1_n loopback)[br]
                       100 = CKE0 loopback[br]
                       101 = reserved (CKE1 loopback; DDRT REQ)[br]
                       110 = ODT0 loopback[br]
                       111 = ERID loopback (ODT1 loopback, DDRT ERR)
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TRAINING_RC0C_FNV_DA_UNIT_0_STRUCT;




/* DA_DDRT_TRAINING_RC0E_FNV_DA_UNIT_0_REG supported on:                      */
/*       EKV (0x400020C0)                                                     */
/* Register default value:              0x00000000                            */
#define DA_DDRT_TRAINING_RC0E_FNV_DA_UNIT_0_REG 0x050240C0
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Parity Control Word
 */
typedef union {
  struct {
    UINT32 par_en : 1;
    /* par_en - Bits[0:0], RW, default = 1'h0 
       Parity Enable [br]
                       If parity is enabled, command parity emask is n/a
     */
    UINT32 rsvd_1 : 1;
    /* rsvd_1 - Bits[1:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_2 : 30;
  } Bits;
  UINT32 Data;
} DA_DDRT_TRAINING_RC0E_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_TRAINING_RC0F_FNV_DA_UNIT_0_REG supported on:                      */
/*       EKV (0x400020C4)                                                     */
/* Register default value:              0x00000003                            */
#define DA_DDRT_TRAINING_RC0F_FNV_DA_UNIT_0_REG 0x050240C4
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Command Latency Adder Control Word
 */
typedef union {
  struct {
    UINT32 wcmd_add : 4;
    /* wcmd_add - Bits[3:0], RW, default = 4'h3 
       Latency adder measured from the 2nd UI of DDRT write command to BCOM write 
       command [br] 
                       0x0-0x3: +0 dclk [br]
                       0x4-0xF: +1 to +12 dclk [br]
     */
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TRAINING_RC0F_FNV_DA_UNIT_0_STRUCT;








/* DA_DDRT_TRAINING_RC4X_FNV_DA_UNIT_0_REG supported on:                      */
/*       EKV (0x400020D4)                                                     */
/* Register default value:              0x00000000                            */
#define DA_DDRT_TRAINING_RC4X_FNV_DA_UNIT_0_REG 0x050240D4
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * CW Source Selection Control Word
 */
typedef union {
  struct {
    UINT32 ad_12_8 : 5;
    /* ad_12_8 - Bits[4:0], RW, default = 5'h0 
       Content of A[12:8] for BCW write/read
     */
    UINT32 func : 3;
    /* func - Bits[7:5], RW, default = 3'h0 
       Function select for BCW access
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TRAINING_RC4X_FNV_DA_UNIT_0_STRUCT;




/* DA_DDRT_TRAINING_RC6X_FNV_DA_UNIT_0_REG supported on:                      */
/*       EKV (0x400020DC)                                                     */
/* Register default value:              0x00000000                            */
#define DA_DDRT_TRAINING_RC6X_FNV_DA_UNIT_0_REG 0x050240DC
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * CW Data Control Word
 */
typedef union {
  struct {
    UINT32 ad_7_0 : 8;
    /* ad_7_0 - Bits[7:0], RW, default = 8'h0 
       Content of A[7:0] for BCW write/read
     */
    UINT32 rsvd : 24;
    /* rsvd - Bits[31:8], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_TRAINING_RC6X_FNV_DA_UNIT_0_STRUCT;




















/* DA_RD_SCR_LFSR_FNV_DA_UNIT_0_REG supported on:                             */
/*       EKV (0x40002104)                                                     */
/* Register default value:              0x0000FFFF                            */
#define DA_RD_SCR_LFSR_FNV_DA_UNIT_0_REG 0x05024104
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Initial LFSR seed for read data scrambling
 */
typedef union {
  struct {
    UINT32 seed : 16;
    /* seed - Bits[15:0], RW, default = 16'hFFFF 
       Initial LFSR seed for read data scrambling[br]
                       x^16 + x^13 + x^10 + x^9 + x^8 + x^4 + 1
     */
    UINT32 en : 1;
    /* en - Bits[16:16], RW, default = 1'h0 
       Enable read data scrambling[br]
                       0: Read data is NOT scrambled[br]
                       1: Enable read data scrambler; read data is scrambled
     */
    UINT32 lfsr_load : 1;
    /* lfsr_load - Bits[17:17], RW, default = 1'b0 
       Load new LFSR seed, this bit needs to be clear first and then set to load a new 
       lfsr seed; this bit is used to load both lower and upper wr lfsr seed 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_RD_SCR_LFSR_FNV_DA_UNIT_0_STRUCT;


/* DA_WR_SCR_LFSR_L_FNV_DA_UNIT_0_REG supported on:                           */
/*       EKV (0x40002108)                                                     */
/* Register default value:              0x0000FFFF                            */
#define DA_WR_SCR_LFSR_L_FNV_DA_UNIT_0_REG 0x05024108
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Initial LFSR seed for write data scrambling. Low register
 */
typedef union {
  struct {
    UINT32 seed_lower : 16;
    /* seed_lower - Bits[15:0], RW, default = 16'hFFFF 
       Initial LFSR seed for read data scrambling[br]
                       x^16 + x^13 + x^10 + x^9 + x^8 + x^4 + 1
     */
    UINT32 en : 1;
    /* en - Bits[16:16], RW, default = 1'h0 
       ENable write data scrambling[br]
                       0: Write data is NOT scrambled[br]
                       1: Enable write data scrambler; write data is scrambled
     */
    UINT32 lfsr_load : 1;
    /* lfsr_load - Bits[17:17], RW, default = 1'b0 
       Load new LFSR seed, this bit needs to be clear first and then set to load a new 
       lfsr seed; this bit is used to load both lower and upper wr lfsr seed 
     */
    UINT32 rsvd : 14;
    /* rsvd - Bits[31:18], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_WR_SCR_LFSR_L_FNV_DA_UNIT_0_STRUCT;


/* DA_WR_SCR_LFSR_H_FNV_DA_UNIT_0_REG supported on:                           */
/*       EKV (0x4000210C)                                                     */
/* Register default value:              0x0000FFFF                            */
#define DA_WR_SCR_LFSR_H_FNV_DA_UNIT_0_REG 0x0502410C
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Initial LFSR seed for write data scrambling. High register
 */
typedef union {
  struct {
    UINT32 seed_upper : 16;
    /* seed_upper - Bits[15:0], RW, default = 16'hFFFF 
       Initial LFSR seed for read data scrambling[br]
                       x^16 + x^13 + x^10 + x^9 + x^8 + x^4 + 1
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_WR_SCR_LFSR_H_FNV_DA_UNIT_0_STRUCT;


/* DA_WR_SCRAMBLE_SEED_SEL_FNV_DA_UNIT_0_REG supported on:                    */
/*       EKV (0x40002110)                                                     */
/* Register default value:              0x00000000                            */
#define DA_WR_SCRAMBLE_SEED_SEL_FNV_DA_UNIT_0_REG 0x05024110
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Write Data Scrambler Seed Reordering
 */
typedef union {
  struct {
    UINT32 ordersel : 16;
    /* ordersel - Bits[15:0], RW, default = 16'h0000 
       select for scrambler seed re-ordering[br]
                       default - no re-ordering
     */
    UINT32 rsvd : 16;
    /* rsvd - Bits[31:16], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_WR_SCRAMBLE_SEED_SEL_FNV_DA_UNIT_0_STRUCT;


/* DA_DDRT_LATENCY_FNV_DA_UNIT_0_REG supported on:                            */
/*       EKV (0x40002114)                                                     */
/* Register default value:              0x00000001                            */
#define DA_DDRT_LATENCY_FNV_DA_UNIT_0_REG 0x05024114
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Performance
 */
typedef union {
  struct {
    UINT32 tcl_gnt_erid : 3;
    /* tcl_gnt_erid - Bits[2:0], RW, default = 3'h1 
       ERID CAS latency measured from GNT to ERID (in number of dclks + 6)[br]
                       0x0 : RSVD (FNV does not support 6 dclks) [br]
       		0x1-0x7: 7-13 [br]
                       (Write recovery and read to precharge for DDR4)
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[31:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_DDRT_LATENCY_FNV_DA_UNIT_0_STRUCT;






/* DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_REG supported on:                       */
/*       EKV (0x40002120)                                                     */
/* Register default value:              0x0063C601                            */
#define DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_REG 0x05024120
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * DQ buffer BCOM interface control
 */
typedef union {
  struct {
    UINT32 en : 1;
    /* en - Bits[0:0], RW, default = 1'b1 
       Enable Data Buffer (DB) [br]
                       0: Disable DB, commands are not generated for BCOM (default) 
       [br] 
                       1: Enable DB; rd, wr, mrs, bcwrd, bcwwr command are generated 
       for BCOM 
     */
    UINT32 rsvd_1 : 1;
    UINT32 tcl_add : 3;
    /* tcl_add - Bits[4:2], RW, default = 3'h0 
       0x0-0x7: 0-7 dclk
     */
    UINT32 tcwl_add : 4;
    /* tcwl_add - Bits[8:5], RW, default = 4'h0 
       0x0-0xf: 0-15 dclk
     */
    UINT32 rsvd_9 : 4;
    UINT32 bcw_mpr_rd_req_en : 1;
    /* bcw_mpr_rd_req_en - Bits[13:13], RW, default = 1'b0 
       DDRT REQ# is generated for MEMRD when DQ buffer is set to MPR read override 
       mode[br] 
                       Default is not to generate REQ#, since MEMRD is converted to BCW 
       read and sent to DQ buffer [br] 
                       0: Disabled (default) [br]
                       1: Enabled
     */
    UINT32 rsvd_14 : 18;
  } Bits;
  UINT32 Data;
} DA_DDRT_DQ_BCOM_CTRL_FNV_DA_UNIT_0_STRUCT;




/* DA_WRITE_CREDIT_FNV_DA_UNIT_0_REG supported on:                            */
/*       EKV (0x40002128)                                                     */
/* Register default value:              0x00000038                            */
#define DA_WRITE_CREDIT_FNV_DA_UNIT_0_REG 0x05024128
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * This register contains the default value of write credits. Will be loaded when 
 * value of CSR changes 
 */
typedef union {
  struct {
    UINT32 val : 6;
    /* val - Bits[5:0], RW, default = 6'h38 
       Value of write credits. Default value is 56. Max is 56.
     */
    UINT32 rsvd : 26;
    /* rsvd - Bits[31:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_WRITE_CREDIT_FNV_DA_UNIT_0_STRUCT;








/* DA_WCRD_CNT_FNV_DA_UNIT_0_REG supported on:                                */
/*       EKV (0x4000213C)                                                     */
/* Register default value:              0x00000000                            */
#define DA_WCRD_CNT_FNV_DA_UNIT_0_REG 0x0502413C
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Indicates status of write credit availability
 */
typedef union {
  struct {
    UINT32 wcrd_cnt : 7;
    /* wcrd_cnt - Bits[6:0], ROV, default = 7'h0 
       Indicates the number of write credits available in FNV
     */
    UINT32 rsvd : 25;
    /* rsvd - Bits[31:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_WCRD_CNT_FNV_DA_UNIT_0_STRUCT;
















/* DA_DDRT_TRAINING_DEFEATURE0_FNV_DA_UNIT_0_REG supported on:                */
/*       EKV (0x40002300)                                                     */
/* Register default value:              0x00000108                            */
#define DA_DDRT_TRAINING_DEFEATURE0_FNV_DA_UNIT_0_REG 0x05024300
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Training Defeature 0
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 3;
    UINT32 nodata_trn_en : 1;
    /* nodata_trn_en - Bits[3:3], RW, default = 1'h1 
       No data packet is returned if GNT# is received too early during training[br]
                       0 = Normal [br]
                       1 = Enable nodata packet during training (default)
     */
    UINT32 rsvd_4 : 28;
  } Bits;
  UINT32 Data;
} DA_DDRT_TRAINING_DEFEATURE0_FNV_DA_UNIT_0_STRUCT;


























































/* DA_EMASK_FNV_DA_UNIT_0_REG supported on:                                   */
/*       EKV (0x40002510)                                                     */
/* Register default value:              0x3FFEBFFF                            */
#define DA_EMASK_FNV_DA_UNIT_0_REG 0x05024510
/* Struct format extracted from XML file EKV\0.0.2.EKV.xml.
 * Masking errors per source
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 3;
    UINT32 wr_credit : 1;
    /* wr_credit - Bits[3:3], RW, default = 1'b1 
       Mask error [br]
                       1 = Mask error [br]
                       0 = Allow error [br]
                       Default is 1
     */
    UINT32 rsvd_4 : 28;
  } Bits;
  UINT32 Data;
} DA_EMASK_FNV_DA_UNIT_0_STRUCT;
























































































































#endif /* FNV_DA_UNIT_0_h */
