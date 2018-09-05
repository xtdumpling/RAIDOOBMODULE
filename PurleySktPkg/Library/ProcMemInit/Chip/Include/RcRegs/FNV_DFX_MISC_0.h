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

#ifndef FNV_DFX_MISC_0_h
#define FNV_DFX_MISC_0_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* FNV_DFX_MISC_0_DEV 1                                                       */
/* FNV_DFX_MISC_0_FUN 4                                                       */



/* VENDOR_DEVICE_ID_FNV_DFX_MISC_0_REG supported on:                          */
/*       EKV (0x4000C008)                                                     */
/* Register default value:              0x8086097A                            */
#define VENDOR_DEVICE_ID_FNV_DFX_MISC_0_REG 0x050C4008
/* Struct format extracted from XML file EKV\0.1.4.EKV.xml.
 * This register contains the Vendor/Device ID
 */
typedef union {
  struct {
    UINT32 device_id : 16;
    /* device_id - Bits[15:0], RO, default = 16'h097A 
       Device ID.[br]
               FNV - 0x0979[br]
               EKV - 0x097A[br]
               BWV - 0x097B
     */
    UINT32 vendor_id : 16;
    /* vendor_id - Bits[31:16], RO, default = 16'h8086 
       Vendor ID
     */
  } Bits;
  UINT32 Data;
} VENDOR_DEVICE_ID_FNV_DFX_MISC_0_STRUCT;


/* REVISION_MFG_ID_FNV_DFX_MISC_0_REG supported on:                           */
/*       EKV (0x4000C00C)                                                     */
/* Register default value:              0x00000000                            */
#define REVISION_MFG_ID_FNV_DFX_MISC_0_REG 0x050C400C
/* Struct format extracted from XML file EKV\0.1.4.EKV.xml.
 * Revision ID control and values.[br]
 *             MFG Stepping ID value
 */
typedef union {
  struct {
    UINT32 rev_id_sel : 2;
    /* rev_id_sel - Bits[1:0], RWO, default = 2'h0 
       Selects RevID[br]
                       00 : SRID (default)[br]
                       01 : CRID[br] 
     */
    UINT32 revision_id : 8;
    /* revision_id - Bits[9:2], ROV, default = 8'h0 
       OS accessible RevID selected by RevID Sel[br]
                       The 8-bit REVID is split into 2 sections:[br]
                       The following bits are Reserved, [7:6] and [3:2][br]
                           Upper 2-bits [5:4], translates to base layer stepping[br]
                               00 A-step[br] 
                               01 B-step[br]
                               10 C-step[br]
                               11 D-step[br]
                           Lower 2-bits [1:0], translates to metal layer stepping[br]
                               00 Metal-step 0[br] 
                               01 Metal-step 1[br]
                               10 Metal-step 2[br]
                               11 Metal-step 3[br]
     */
    UINT32 rsvd : 22;
    /* rsvd - Bits[31:10], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} REVISION_MFG_ID_FNV_DFX_MISC_0_STRUCT;
























/* FNV_DEBUG_LOCK_FNV_DFX_MISC_0_REG supported on:                            */
/*       EKV (0x4000C048)                                                     */
/* Register default value:              0x00000000                            */
#define FNV_DEBUG_LOCK_FNV_DFX_MISC_0_REG 0x050C4048
/* Struct format extracted from XML file EKV\0.1.4.EKV.xml.
 * This register controls access to DFx functionality. Mainly JTAG tunneling.
 */
typedef union {
  struct {
    UINT32 dbg_en : 1;
    /* dbg_en - Bits[0:0], RWV, default = 1'b0 
       0=Debug Disabled[br]
       1=Debug Enabled[br]
       Note: The default value is loaded from Fuses. When lock is set, this field will 
       not change until next assertion of RST_N. 
     */
    UINT32 rsvd : 29;
    /* rsvd - Bits[29:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 lock : 1;
    /* lock - Bits[30:30], RWV, default = 1'b0 
       0=unlocked[br]
       1=locked[br]
       When set, locks this register from further changes until next assertion of 
       RST_N. The default value is loaded from fuses 
     */
    UINT32 dwe : 1;
    /* dwe - Bits[31:31], ROV, default = 1'b0 
       HW sets this bit to indicate that Debug was Enabled.
     */
  } Bits;
  UINT32 Data;
} FNV_DEBUG_LOCK_FNV_DFX_MISC_0_STRUCT;


















/* MSC_TEMP_CRIT_CSR_FNV_DFX_MISC_0_REG supported on:                         */
/*       EKV (0x4000C074)                                                     */
/* Register default value:              0x000000FF                            */
#define MSC_TEMP_CRIT_CSR_FNV_DFX_MISC_0_REG 0x050C4074
/* Struct format extracted from XML file EKV\0.1.4.EKV.xml.
 * Critical trip point.
 */
typedef union {
  struct {
    UINT32 tempcrit : 9;
    /* tempcrit - Bits[8:0], RW, default = 9'h0ff 
       Critical threshold trip point.
     */
    UINT32 rsvd : 23;
    /* rsvd - Bits[31:9], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MSC_TEMP_CRIT_CSR_FNV_DFX_MISC_0_STRUCT;


















/* MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_REG supported on:                  */
/*       EKV (0x4000C180)                                                     */
/* Register default value:              0x54414269                            */
#define MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_REG 0x050C4180
/* Struct format extracted from XML file EKV\0.1.4.EKV.xml.
 * FNVIO Interface Initialization 
 */
typedef union {
  struct {
    UINT32 mc2ddrttrainreset : 1;
    /* mc2ddrttrainreset - Bits[0:0], RW, default = 1'h1 
       DDR IO Training Reset. This should be pulsed after each IO Training Stage to 
       reload picodes updates.[br] 
                Note that the actual training reset signal to fnvio is an OR function 
       of this CSR and normal reset to DDRT IO 
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[2:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 ddrtclkdetectdetlev : 5;
    /* ddrtclkdetectdetlev - Bits[7:3], RW, default = 5'b01101 
       Analog control: DDRT Ref. CLK Detect Lev. [br]
       	        ddrtclkdetectdetlev[1:0] - Real Usage. [br]
       		ddrtclkdetectdetlev[2] - Spare Usage. [br]
       		ddrtclkdetectdetlev[4:3] - Real Usage. [br]
       		Spare Usage Criteria: [br]
       		ddrtclkdetectdetlev[2] - rxbiasadj_pq[0] (default 1'b1). [br]
     */
    UINT32 ddrtclkdetectfiltlev : 2;
    /* ddrtclkdetectfiltlev - Bits[9:8], RW, default = 2'b10 
       Analog control: DDRT Ref. CLK Filter Lev
     */
    UINT32 ddrtrefclkbypass : 1;
    /* ddrtrefclkbypass - Bits[10:10], RW, default = 1'h0 
       Enable bit for DDRT reference clock divider bypass
     */
    UINT32 ddrtrefclkdivratio : 5;
    /* ddrtrefclkdivratio - Bits[15:11], RW, default = 5'h08 
       DDRT Reference clock divider ratio[br]
       	        5'h06: divide by 6 - Input DClk 800 MHz[br]
       		5'h07: divide by 7 - Input DClk 933 MHz[br]
       		5'h08: divide by 8 - Input DClk 1067 MHz[br]
       		5'h09: divide by 9 - Input DClk 1200 MHz[br]
           *This CSR needs to be programmed during init flow*[br]
           *This CSR needs to be programmed during ddrt frequency change*
     */
    UINT32 ddrvrefgenenableqnnnh : 1;
    /* ddrvrefgenenableqnnnh - Bits[16:16], RW, default = 1'h1 
       set to 1 along with the other ddrtclk controls(like bias and SA enable)
     */
    UINT32 ddrrxvrefctlqnnnh : 7;
    /* ddrrxvrefctlqnnnh - Bits[23:17], RW, default = 7'h20 
       Default: 7'b010_0000
     */
    UINT32 ddrvrefhalfupqnnnh : 1;
    /* ddrvrefhalfupqnnnh - Bits[24:24], RW, default = 1'h0 
       DDR VREF Half Up - Set to 0)
     */
    UINT32 ddrtclkdetectbiascon : 4;
    /* ddrtclkdetectbiascon - Bits[28:25], RW, default = 4'b1010 
       Analog control: DDRT Ref. CLK BIAS Con. [br]
       	        ddrtclkdetectbiascon[2:0] - Real Usage. [br]
       		ddrtclkdetectbiascon[3] - Spare Usage. [br]
       		Spare Usage Criteria: [br]
       		ddrtclkdetectbiascon[3] - rxbiasadj_pq[1] (default 1'b1). [br] 
     */
    UINT32 ddrtclkdetecthyscon : 2;
    /* ddrtclkdetecthyscon - Bits[30:29], RW, default = 2'b10 
       Analog control: DDRT Ref. CLK HYS Con
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} MSC_FNVIO_INIT_CONTROL_0_FNV_DFX_MISC_0_STRUCT;




/* MSC_FNVIO_INIT_CONTROL_1_FNV_DFX_MISC_0_REG supported on:                  */
/*       EKV (0x4000C188)                                                     */
/* Register default value:              0x2E010805                            */
#define MSC_FNVIO_INIT_CONTROL_1_FNV_DFX_MISC_0_REG 0x050C4188
/* Struct format extracted from XML file EKV\0.1.4.EKV.xml.
 * Connects to FNVIO Interface Initialization 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    UINT32 rsvd_1 : 1;
    /* rsvd_1 - Bits[1:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_2 : 1;
    UINT32 rsvd_3 : 4;
    /* rsvd_3 - Bits[6:3], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 rsvd_7 : 23;
    UINT32 ddrt_resync : 1;
    /* ddrt_resync - Bits[30:30], RWV, default = 1'h0 
       DDRT Frequency change mode is enabled
                On entry of DDRT Frequency change, set Clock Idle Mask. Clear Clock 
       Idle Mask when new DDRT Frequencies are correctly programmed 
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} MSC_FNVIO_INIT_CONTROL_1_FNV_DFX_MISC_0_STRUCT;






/* MSC_NGN_PLL_CTRL1_FNV_DFX_MISC_0_REG supported on:                         */
/*       EKV (0x4000C194)                                                     */
/* Register default value:              0x00000008                            */
#define MSC_NGN_PLL_CTRL1_FNV_DFX_MISC_0_REG 0x050C4194
/* Struct format extracted from XML file EKV\0.1.4.EKV.xml.
 * Current NGN feedback divider ratio
 * 	    7'h08 : NGN CLK frequency is 533 Mhz
 * 	    7'h0A : NGN CLK frequency is 667 Mhz
 * 	    7'h0C : NGN CLK frequency is 800 Mhz
 */
typedef union {
  struct {
    UINT32 freqcur : 7;
    /* freqcur - Bits[6:0], RWV, default = 7'h08 
       Feedback divider ratio value
     */
    UINT32 spare_rw_v_p : 16;
    /* spare_rw_v_p - Bits[22:7], RWV, default = 16'h0 
       Spare RW V P
     */
    UINT32 rsvd : 2;
    /* rsvd - Bits[24:23], n/a, default = n/a 
       Padding added by header generation tool.
     */
    UINT32 spare_rw_p : 7;
    /* spare_rw_p - Bits[31:25], RW, default = 7'h0 
       Spare RW P
     */
  } Bits;
  UINT32 Data;
} MSC_NGN_PLL_CTRL1_FNV_DFX_MISC_0_STRUCT;


/* MSC_PLL_CTRL_FNV_DFX_MISC_0_REG supported on:                              */
/*       EKV (0x4000C198)                                                     */
/* Register default value:              0xE0003000                            */
#define MSC_PLL_CTRL_FNV_DFX_MISC_0_REG 0x050C4198
/* Struct format extracted from XML file EKV\0.1.4.EKV.xml.
 * NGN/DDRT PLL Control Register
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 12;
    UINT32 daunit_pmreset_mask : 1;
    /* daunit_pmreset_mask - Bits[12:12], RW, default = 1'h1 
       When set: RFSM will reset DAUNIT in DDRT PM Modes [br]
                If this bit is cleared(set to zero), then da_ckgate_ctrl.dclk_gate_dis 
       must be set to 0.  
     */

    UINT32 ddrtio_pmreset_mask : 1;
    /* ddrtio_pmreset_mask - Bits[13:13], RW, default = 1'h1 
       When set: RFSM will reset DDRT IO in DDRT Reset PM Mode
     */
    UINT32 rsvd_13 : 17;
    UINT32 ddrtio_ckidle_mask : 1;
    /* ddrtio_ckidle_mask - Bits[31:31], RW, default = 1'h1 
       When this bit is set, clock idle from ddrio is masked. Firmware should only 
       clear this bit when 
                correct DDRT Frequencies are programmed in boot flow. Set this bit when 
       entering DDRT Frequency Change 
                         0 : No Mask
                         1 : MASK Clock Idle (Default)
     */
  } Bits;
  UINT32 Data;
} MSC_PLL_CTRL_FNV_DFX_MISC_0_STRUCT;
















/* MSC_FNVIO_DDRTDLL_CSR_FNV_DFX_MISC_0_REG supported on:                     */
/*       EKV (0x4000C1B8)                                                     */
/* Register default value:              0x00000000                            */
#define MSC_FNVIO_DDRTDLL_CSR_FNV_DFX_MISC_0_REG 0x050C41B8
/* Struct format extracted from XML file EKV\0.1.4.EKV.xml.
 * FNVIO interface Ddrtdll override signals register
 */
typedef union {
  struct {
    UINT32 en : 1;
    /* en - Bits[0:0], RW, default = 1'b0 
       Enable to override signals with CSR value
     */
    UINT32 ddrtdllpowerdown01qnnnh_val : 1;
    /* ddrtdllpowerdown01qnnnh_val - Bits[1:1], RW, default = 1'b0 
       DDRT DLL power down control
     */
    UINT32 ddrtdllpowerdown23qnnnh_val : 1;
    /* ddrtdllpowerdown23qnnnh_val - Bits[2:2], RW, default = 1'b0 
       DDRT DLL power down control
     */
    UINT32 ddrtdllpowerdown4qnnnh_val : 1;
    /* ddrtdllpowerdown4qnnnh_val - Bits[3:3], RW, default = 1'b0 
       DDRT DLL power down control
     */
    UINT32 rsvd : 28;
    /* rsvd - Bits[31:4], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} MSC_FNVIO_DDRTDLL_CSR_FNV_DFX_MISC_0_STRUCT;




































































































#endif /* FNV_DFX_MISC_0_h */
