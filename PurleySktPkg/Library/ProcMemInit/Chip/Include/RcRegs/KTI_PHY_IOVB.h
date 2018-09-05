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
/* Date Stamp: 9/13/2016 */

#ifndef KTI_PHY_IOVB_h
#define KTI_PHY_IOVB_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* KTI_PHY_IOVB_DEV 14                                                        */
/* KTI_PHY_IOVB_FUN 1                                                         */

/* IOVB_RX_CTL0_0_0_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000001)                                                  */
/*       SKX (0x00000001)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_0_0_KTI_PHY_IOVB_REG 0x00000001
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_0_0_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_0_0_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000004)                                                  */
/*       SKX (0x00000004)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_0_0_KTI_PHY_IOVB_REG 0x00000004
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_0_0_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000005)                                                  */
/*       SKX (0x00000005)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_0_0_KTI_PHY_IOVB_REG 0x00000005
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_0_0_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000006)                                                  */
/*       SKX (0x00000006)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_0_0_KTI_PHY_IOVB_REG 0x00000006
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_0_0_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000007)                                                  */
/*       SKX (0x00000007)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_0_0_KTI_PHY_IOVB_REG 0x00000007
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_0_0_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_0_0_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000009)                                                  */
/*       SKX (0x00000009)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_0_0_KTI_PHY_IOVB_REG 0x00000009
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_0_0_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_0_0_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000000D)                                                  */
/*       SKX (0x0000000D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_0_0_KTI_PHY_IOVB_REG 0x0000000D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_0_0_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_0_0_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000000F)                                                  */
/*       SKX (0x0000000F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_0_0_KTI_PHY_IOVB_REG 0x0000000F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_0_0_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000010)                                                  */
/*       SKX (0x00000010)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_0_0_KTI_PHY_IOVB_REG 0x00000010
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_0_0_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_0_0_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000013)                                                  */
/*       SKX (0x00000013)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_0_0_KTI_PHY_IOVB_REG 0x00000013
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_0_0_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000014)                                                  */
/*       SKX (0x00000014)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_0_0_KTI_PHY_IOVB_REG 0x00000014
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_0_0_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_0_0_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000016)                                                  */
/*       SKX (0x00000016)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_0_0_KTI_PHY_IOVB_REG 0x00000016
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_0_0_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000017)                                                  */
/*       SKX (0x00000017)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_0_0_KTI_PHY_IOVB_REG 0x00000017
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_0_0_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000018)                                                  */
/*       SKX (0x00000018)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_0_0_KTI_PHY_IOVB_REG 0x00000018
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_0_0_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000019)                                                  */
/*       SKX (0x00000019)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_0_0_KTI_PHY_IOVB_REG 0x00000019
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_0_0_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_0_0_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000001C)                                                  */
/*       SKX (0x0000001C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_0_0_KTI_PHY_IOVB_REG 0x0000001C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_0_0_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_0_0_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000020)                                                  */
/*       SKX (0x00000020)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_0_0_KTI_PHY_IOVB_REG 0x00000020
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_0_0_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_0_0_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000027)                                                  */
/*       SKX (0x00000027)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_0_0_KTI_PHY_IOVB_REG 0x00000027
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_0_0_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000028)                                                  */
/*       SKX (0x00000028)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_0_0_KTI_PHY_IOVB_REG 0x00000028
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_0_0_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000029)                                                  */
/*       SKX (0x00000029)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_0_0_KTI_PHY_IOVB_REG 0x00000029
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_0_0_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x0000002A)                                                  */
/*       SKX (0x0000002A)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_0_0_KTI_PHY_IOVB_REG 0x0000002A
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_0_0_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_0_0_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000002C)                                                  */
/*       SKX (0x0000002C)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_0_0_KTI_PHY_IOVB_REG 0x0000002C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_0_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_0_0_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000002D)                                                  */
/*       SKX (0x0000002D)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_0_0_KTI_PHY_IOVB_REG 0x0000002D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_0_0_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_0_0_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000034)                                                  */
/*       SKX (0x00000034)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_0_0_KTI_PHY_IOVB_REG 0x00000034
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_0_0_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_1_0_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000041)                                                  */
/*       SKX (0x00000041)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_1_0_KTI_PHY_IOVB_REG 0x00000041
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_1_0_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_1_0_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000044)                                                  */
/*       SKX (0x00000044)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_1_0_KTI_PHY_IOVB_REG 0x00000044
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_1_0_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000045)                                                  */
/*       SKX (0x00000045)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_1_0_KTI_PHY_IOVB_REG 0x00000045
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_1_0_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000046)                                                  */
/*       SKX (0x00000046)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_1_0_KTI_PHY_IOVB_REG 0x00000046
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_1_0_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000047)                                                  */
/*       SKX (0x00000047)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_1_0_KTI_PHY_IOVB_REG 0x00000047
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_1_0_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_1_0_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000049)                                                  */
/*       SKX (0x00000049)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_1_0_KTI_PHY_IOVB_REG 0x00000049
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_1_0_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_1_0_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000004D)                                                  */
/*       SKX (0x0000004D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_1_0_KTI_PHY_IOVB_REG 0x0000004D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_1_0_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_1_0_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000004F)                                                  */
/*       SKX (0x0000004F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_1_0_KTI_PHY_IOVB_REG 0x0000004F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_1_0_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000050)                                                  */
/*       SKX (0x00000050)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_1_0_KTI_PHY_IOVB_REG 0x00000050
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_1_0_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_1_0_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000053)                                                  */
/*       SKX (0x00000053)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_1_0_KTI_PHY_IOVB_REG 0x00000053
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_1_0_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000054)                                                  */
/*       SKX (0x00000054)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_1_0_KTI_PHY_IOVB_REG 0x00000054
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_1_0_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_1_0_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000056)                                                  */
/*       SKX (0x00000056)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_1_0_KTI_PHY_IOVB_REG 0x00000056
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_1_0_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000057)                                                  */
/*       SKX (0x00000057)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_1_0_KTI_PHY_IOVB_REG 0x00000057
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_1_0_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000058)                                                  */
/*       SKX (0x00000058)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_1_0_KTI_PHY_IOVB_REG 0x00000058
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_1_0_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000059)                                                  */
/*       SKX (0x00000059)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_1_0_KTI_PHY_IOVB_REG 0x00000059
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_1_0_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_1_0_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000005C)                                                  */
/*       SKX (0x0000005C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_1_0_KTI_PHY_IOVB_REG 0x0000005C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_1_0_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_1_0_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000060)                                                  */
/*       SKX (0x00000060)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_1_0_KTI_PHY_IOVB_REG 0x00000060
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_1_0_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_1_0_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000067)                                                  */
/*       SKX (0x00000067)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_1_0_KTI_PHY_IOVB_REG 0x00000067
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_1_0_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000068)                                                  */
/*       SKX (0x00000068)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_1_0_KTI_PHY_IOVB_REG 0x00000068
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_1_0_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000069)                                                  */
/*       SKX (0x00000069)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_1_0_KTI_PHY_IOVB_REG 0x00000069
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_1_0_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x0000006A)                                                  */
/*       SKX (0x0000006A)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_1_0_KTI_PHY_IOVB_REG 0x0000006A
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_1_0_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_1_0_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000006C)                                                  */
/*       SKX (0x0000006C)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_1_0_KTI_PHY_IOVB_REG 0x0000006C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_1_0_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_1_0_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000006D)                                                  */
/*       SKX (0x0000006D)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_1_0_KTI_PHY_IOVB_REG 0x0000006D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_1_0_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_1_0_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000074)                                                  */
/*       SKX (0x00000074)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_1_0_KTI_PHY_IOVB_REG 0x00000074
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_1_0_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_0_1_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000081)                                                  */
/*       SKX (0x00000081)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_0_1_KTI_PHY_IOVB_REG 0x00000081
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_0_1_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_0_1_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000084)                                                  */
/*       SKX (0x00000084)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_0_1_KTI_PHY_IOVB_REG 0x00000084
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_0_1_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000085)                                                  */
/*       SKX (0x00000085)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_0_1_KTI_PHY_IOVB_REG 0x00000085
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_0_1_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000086)                                                  */
/*       SKX (0x00000086)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_0_1_KTI_PHY_IOVB_REG 0x00000086
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_0_1_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000087)                                                  */
/*       SKX (0x00000087)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_0_1_KTI_PHY_IOVB_REG 0x00000087
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_0_1_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_0_1_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000089)                                                  */
/*       SKX (0x00000089)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_0_1_KTI_PHY_IOVB_REG 0x00000089
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_0_1_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_0_1_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000008D)                                                  */
/*       SKX (0x0000008D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_0_1_KTI_PHY_IOVB_REG 0x0000008D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_0_1_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_0_1_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000008F)                                                  */
/*       SKX (0x0000008F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_0_1_KTI_PHY_IOVB_REG 0x0000008F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_0_1_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000090)                                                  */
/*       SKX (0x00000090)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_0_1_KTI_PHY_IOVB_REG 0x00000090
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_0_1_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_0_1_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000093)                                                  */
/*       SKX (0x00000093)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_0_1_KTI_PHY_IOVB_REG 0x00000093
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_0_1_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000094)                                                  */
/*       SKX (0x00000094)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_0_1_KTI_PHY_IOVB_REG 0x00000094
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_0_1_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_0_1_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000096)                                                  */
/*       SKX (0x00000096)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_0_1_KTI_PHY_IOVB_REG 0x00000096
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_0_1_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000097)                                                  */
/*       SKX (0x00000097)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_0_1_KTI_PHY_IOVB_REG 0x00000097
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_0_1_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000098)                                                  */
/*       SKX (0x00000098)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_0_1_KTI_PHY_IOVB_REG 0x00000098
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_0_1_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000099)                                                  */
/*       SKX (0x00000099)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_0_1_KTI_PHY_IOVB_REG 0x00000099
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_0_1_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_0_1_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000009C)                                                  */
/*       SKX (0x0000009C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_0_1_KTI_PHY_IOVB_REG 0x0000009C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_0_1_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_0_1_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000000A0)                                                  */
/*       SKX (0x000000A0)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_0_1_KTI_PHY_IOVB_REG 0x000000A0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_0_1_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_0_1_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000000A7)                                                  */
/*       SKX (0x000000A7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_0_1_KTI_PHY_IOVB_REG 0x000000A7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_0_1_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000000A8)                                                  */
/*       SKX (0x000000A8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_0_1_KTI_PHY_IOVB_REG 0x000000A8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_0_1_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000000A9)                                                  */
/*       SKX (0x000000A9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_0_1_KTI_PHY_IOVB_REG 0x000000A9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_0_1_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000000AA)                                                  */
/*       SKX (0x000000AA)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_0_1_KTI_PHY_IOVB_REG 0x000000AA
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_0_1_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_0_1_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000000AC)                                                  */
/*       SKX (0x000000AC)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_0_1_KTI_PHY_IOVB_REG 0x000000AC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_0_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_0_1_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000000AD)                                                  */
/*       SKX (0x000000AD)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_0_1_KTI_PHY_IOVB_REG 0x000000AD
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_0_1_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_0_1_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000000B4)                                                  */
/*       SKX (0x000000B4)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_0_1_KTI_PHY_IOVB_REG 0x000000B4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_0_1_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_1_1_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000000C1)                                                  */
/*       SKX (0x000000C1)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_1_1_KTI_PHY_IOVB_REG 0x000000C1
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_1_1_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_1_1_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000000C4)                                                  */
/*       SKX (0x000000C4)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_1_1_KTI_PHY_IOVB_REG 0x000000C4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_1_1_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000000C5)                                                  */
/*       SKX (0x000000C5)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_1_1_KTI_PHY_IOVB_REG 0x000000C5
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_1_1_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x000000C6)                                                  */
/*       SKX (0x000000C6)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_1_1_KTI_PHY_IOVB_REG 0x000000C6
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_1_1_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x000000C7)                                                  */
/*       SKX (0x000000C7)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_1_1_KTI_PHY_IOVB_REG 0x000000C7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_1_1_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_1_1_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x000000C9)                                                  */
/*       SKX (0x000000C9)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_1_1_KTI_PHY_IOVB_REG 0x000000C9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_1_1_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_1_1_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000000CD)                                                  */
/*       SKX (0x000000CD)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_1_1_KTI_PHY_IOVB_REG 0x000000CD
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_1_1_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_1_1_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000000CF)                                                  */
/*       SKX (0x000000CF)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_1_1_KTI_PHY_IOVB_REG 0x000000CF
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_1_1_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000000D0)                                                  */
/*       SKX (0x000000D0)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_1_1_KTI_PHY_IOVB_REG 0x000000D0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_1_1_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_1_1_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000000D3)                                                  */
/*       SKX (0x000000D3)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_1_1_KTI_PHY_IOVB_REG 0x000000D3
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_1_1_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000000D4)                                                  */
/*       SKX (0x000000D4)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_1_1_KTI_PHY_IOVB_REG 0x000000D4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_1_1_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_1_1_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000000D6)                                                  */
/*       SKX (0x000000D6)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_1_1_KTI_PHY_IOVB_REG 0x000000D6
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_1_1_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000000D7)                                                  */
/*       SKX (0x000000D7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_1_1_KTI_PHY_IOVB_REG 0x000000D7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_1_1_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000000D8)                                                  */
/*       SKX (0x000000D8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_1_1_KTI_PHY_IOVB_REG 0x000000D8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_1_1_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000000D9)                                                  */
/*       SKX (0x000000D9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_1_1_KTI_PHY_IOVB_REG 0x000000D9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_1_1_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_1_1_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000000DC)                                                  */
/*       SKX (0x000000DC)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_1_1_KTI_PHY_IOVB_REG 0x000000DC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_1_1_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_1_1_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000000E0)                                                  */
/*       SKX (0x000000E0)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_1_1_KTI_PHY_IOVB_REG 0x000000E0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_1_1_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_1_1_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000000E7)                                                  */
/*       SKX (0x000000E7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_1_1_KTI_PHY_IOVB_REG 0x000000E7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_1_1_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000000E8)                                                  */
/*       SKX (0x000000E8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_1_1_KTI_PHY_IOVB_REG 0x000000E8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_1_1_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000000E9)                                                  */
/*       SKX (0x000000E9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_1_1_KTI_PHY_IOVB_REG 0x000000E9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_1_1_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000000EA)                                                  */
/*       SKX (0x000000EA)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_1_1_KTI_PHY_IOVB_REG 0x000000EA
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_1_1_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_1_1_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000000EC)                                                  */
/*       SKX (0x000000EC)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_1_1_KTI_PHY_IOVB_REG 0x000000EC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_1_1_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_1_1_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000000ED)                                                  */
/*       SKX (0x000000ED)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_1_1_KTI_PHY_IOVB_REG 0x000000ED
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_1_1_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_1_1_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000000F4)                                                  */
/*       SKX (0x000000F4)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_1_1_KTI_PHY_IOVB_REG 0x000000F4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_1_1_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_0_2_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000101)                                                  */
/*       SKX (0x00000101)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_0_2_KTI_PHY_IOVB_REG 0x00000101
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_0_2_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_0_2_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000104)                                                  */
/*       SKX (0x00000104)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_0_2_KTI_PHY_IOVB_REG 0x00000104
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_0_2_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000105)                                                  */
/*       SKX (0x00000105)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_0_2_KTI_PHY_IOVB_REG 0x00000105
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_0_2_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000106)                                                  */
/*       SKX (0x00000106)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_0_2_KTI_PHY_IOVB_REG 0x00000106
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_0_2_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000107)                                                  */
/*       SKX (0x00000107)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_0_2_KTI_PHY_IOVB_REG 0x00000107
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_0_2_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_0_2_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000109)                                                  */
/*       SKX (0x00000109)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_0_2_KTI_PHY_IOVB_REG 0x00000109
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_0_2_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_0_2_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000010D)                                                  */
/*       SKX (0x0000010D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_0_2_KTI_PHY_IOVB_REG 0x0000010D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_0_2_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_0_2_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000010F)                                                  */
/*       SKX (0x0000010F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_0_2_KTI_PHY_IOVB_REG 0x0000010F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_0_2_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000110)                                                  */
/*       SKX (0x00000110)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_0_2_KTI_PHY_IOVB_REG 0x00000110
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_0_2_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_0_2_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000113)                                                  */
/*       SKX (0x00000113)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_0_2_KTI_PHY_IOVB_REG 0x00000113
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_0_2_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000114)                                                  */
/*       SKX (0x00000114)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_0_2_KTI_PHY_IOVB_REG 0x00000114
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_0_2_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_0_2_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000116)                                                  */
/*       SKX (0x00000116)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_0_2_KTI_PHY_IOVB_REG 0x00000116
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_0_2_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000117)                                                  */
/*       SKX (0x00000117)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_0_2_KTI_PHY_IOVB_REG 0x00000117
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_0_2_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000118)                                                  */
/*       SKX (0x00000118)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_0_2_KTI_PHY_IOVB_REG 0x00000118
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_0_2_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000119)                                                  */
/*       SKX (0x00000119)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_0_2_KTI_PHY_IOVB_REG 0x00000119
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_0_2_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_0_2_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000011C)                                                  */
/*       SKX (0x0000011C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_0_2_KTI_PHY_IOVB_REG 0x0000011C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_0_2_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_0_2_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000120)                                                  */
/*       SKX (0x00000120)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_0_2_KTI_PHY_IOVB_REG 0x00000120
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_0_2_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_0_2_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000127)                                                  */
/*       SKX (0x00000127)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_0_2_KTI_PHY_IOVB_REG 0x00000127
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_0_2_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000128)                                                  */
/*       SKX (0x00000128)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_0_2_KTI_PHY_IOVB_REG 0x00000128
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_0_2_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000129)                                                  */
/*       SKX (0x00000129)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_0_2_KTI_PHY_IOVB_REG 0x00000129
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_0_2_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x0000012A)                                                  */
/*       SKX (0x0000012A)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_0_2_KTI_PHY_IOVB_REG 0x0000012A
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_0_2_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_0_2_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000012C)                                                  */
/*       SKX (0x0000012C)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_0_2_KTI_PHY_IOVB_REG 0x0000012C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_0_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_0_2_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000012D)                                                  */
/*       SKX (0x0000012D)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_0_2_KTI_PHY_IOVB_REG 0x0000012D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_0_2_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_0_2_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000134)                                                  */
/*       SKX (0x00000134)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_0_2_KTI_PHY_IOVB_REG 0x00000134
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_0_2_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_1_2_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000141)                                                  */
/*       SKX (0x00000141)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_1_2_KTI_PHY_IOVB_REG 0x00000141
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_1_2_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_1_2_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000144)                                                  */
/*       SKX (0x00000144)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_1_2_KTI_PHY_IOVB_REG 0x00000144
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_1_2_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000145)                                                  */
/*       SKX (0x00000145)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_1_2_KTI_PHY_IOVB_REG 0x00000145
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_1_2_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000146)                                                  */
/*       SKX (0x00000146)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_1_2_KTI_PHY_IOVB_REG 0x00000146
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_1_2_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000147)                                                  */
/*       SKX (0x00000147)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_1_2_KTI_PHY_IOVB_REG 0x00000147
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_1_2_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_1_2_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000149)                                                  */
/*       SKX (0x00000149)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_1_2_KTI_PHY_IOVB_REG 0x00000149
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_1_2_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_1_2_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000014D)                                                  */
/*       SKX (0x0000014D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_1_2_KTI_PHY_IOVB_REG 0x0000014D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_1_2_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_1_2_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000014F)                                                  */
/*       SKX (0x0000014F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_1_2_KTI_PHY_IOVB_REG 0x0000014F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_1_2_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000150)                                                  */
/*       SKX (0x00000150)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_1_2_KTI_PHY_IOVB_REG 0x00000150
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_1_2_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_1_2_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000153)                                                  */
/*       SKX (0x00000153)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_1_2_KTI_PHY_IOVB_REG 0x00000153
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_1_2_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000154)                                                  */
/*       SKX (0x00000154)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_1_2_KTI_PHY_IOVB_REG 0x00000154
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_1_2_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_1_2_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000156)                                                  */
/*       SKX (0x00000156)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_1_2_KTI_PHY_IOVB_REG 0x00000156
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_1_2_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000157)                                                  */
/*       SKX (0x00000157)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_1_2_KTI_PHY_IOVB_REG 0x00000157
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_1_2_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000158)                                                  */
/*       SKX (0x00000158)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_1_2_KTI_PHY_IOVB_REG 0x00000158
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_1_2_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000159)                                                  */
/*       SKX (0x00000159)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_1_2_KTI_PHY_IOVB_REG 0x00000159
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_1_2_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_1_2_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000015C)                                                  */
/*       SKX (0x0000015C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_1_2_KTI_PHY_IOVB_REG 0x0000015C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_1_2_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_1_2_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000160)                                                  */
/*       SKX (0x00000160)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_1_2_KTI_PHY_IOVB_REG 0x00000160
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_1_2_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_1_2_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000167)                                                  */
/*       SKX (0x00000167)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_1_2_KTI_PHY_IOVB_REG 0x00000167
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_1_2_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000168)                                                  */
/*       SKX (0x00000168)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_1_2_KTI_PHY_IOVB_REG 0x00000168
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_1_2_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000169)                                                  */
/*       SKX (0x00000169)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_1_2_KTI_PHY_IOVB_REG 0x00000169
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_1_2_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x0000016A)                                                  */
/*       SKX (0x0000016A)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_1_2_KTI_PHY_IOVB_REG 0x0000016A
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_1_2_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_1_2_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000016C)                                                  */
/*       SKX (0x0000016C)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_1_2_KTI_PHY_IOVB_REG 0x0000016C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_1_2_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_1_2_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000016D)                                                  */
/*       SKX (0x0000016D)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_1_2_KTI_PHY_IOVB_REG 0x0000016D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_1_2_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_1_2_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000174)                                                  */
/*       SKX (0x00000174)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_1_2_KTI_PHY_IOVB_REG 0x00000174
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_1_2_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_0_3_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000181)                                                  */
/*       SKX (0x00000181)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_0_3_KTI_PHY_IOVB_REG 0x00000181
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_0_3_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_0_3_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000184)                                                  */
/*       SKX (0x00000184)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_0_3_KTI_PHY_IOVB_REG 0x00000184
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_0_3_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000185)                                                  */
/*       SKX (0x00000185)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_0_3_KTI_PHY_IOVB_REG 0x00000185
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_0_3_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000186)                                                  */
/*       SKX (0x00000186)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_0_3_KTI_PHY_IOVB_REG 0x00000186
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_0_3_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000187)                                                  */
/*       SKX (0x00000187)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_0_3_KTI_PHY_IOVB_REG 0x00000187
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_0_3_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_0_3_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000189)                                                  */
/*       SKX (0x00000189)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_0_3_KTI_PHY_IOVB_REG 0x00000189
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_0_3_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_0_3_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000018D)                                                  */
/*       SKX (0x0000018D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_0_3_KTI_PHY_IOVB_REG 0x0000018D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_0_3_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_0_3_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000018F)                                                  */
/*       SKX (0x0000018F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_0_3_KTI_PHY_IOVB_REG 0x0000018F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_0_3_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000190)                                                  */
/*       SKX (0x00000190)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_0_3_KTI_PHY_IOVB_REG 0x00000190
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_0_3_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_0_3_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000193)                                                  */
/*       SKX (0x00000193)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_0_3_KTI_PHY_IOVB_REG 0x00000193
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_0_3_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000194)                                                  */
/*       SKX (0x00000194)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_0_3_KTI_PHY_IOVB_REG 0x00000194
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_0_3_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_0_3_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000196)                                                  */
/*       SKX (0x00000196)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_0_3_KTI_PHY_IOVB_REG 0x00000196
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_0_3_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000197)                                                  */
/*       SKX (0x00000197)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_0_3_KTI_PHY_IOVB_REG 0x00000197
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_0_3_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000198)                                                  */
/*       SKX (0x00000198)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_0_3_KTI_PHY_IOVB_REG 0x00000198
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_0_3_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000199)                                                  */
/*       SKX (0x00000199)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_0_3_KTI_PHY_IOVB_REG 0x00000199
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_0_3_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_0_3_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000019C)                                                  */
/*       SKX (0x0000019C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_0_3_KTI_PHY_IOVB_REG 0x0000019C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_0_3_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_0_3_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000001A0)                                                  */
/*       SKX (0x000001A0)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_0_3_KTI_PHY_IOVB_REG 0x000001A0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_0_3_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_0_3_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000001A7)                                                  */
/*       SKX (0x000001A7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_0_3_KTI_PHY_IOVB_REG 0x000001A7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_0_3_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000001A8)                                                  */
/*       SKX (0x000001A8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_0_3_KTI_PHY_IOVB_REG 0x000001A8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_0_3_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000001A9)                                                  */
/*       SKX (0x000001A9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_0_3_KTI_PHY_IOVB_REG 0x000001A9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_0_3_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000001AA)                                                  */
/*       SKX (0x000001AA)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_0_3_KTI_PHY_IOVB_REG 0x000001AA
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_0_3_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_0_3_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000001AC)                                                  */
/*       SKX (0x000001AC)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_0_3_KTI_PHY_IOVB_REG 0x000001AC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_0_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_0_3_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000001AD)                                                  */
/*       SKX (0x000001AD)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_0_3_KTI_PHY_IOVB_REG 0x000001AD
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_0_3_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_0_3_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000001B4)                                                  */
/*       SKX (0x000001B4)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_0_3_KTI_PHY_IOVB_REG 0x000001B4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_0_3_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_1_3_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000001C1)                                                  */
/*       SKX (0x000001C1)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_1_3_KTI_PHY_IOVB_REG 0x000001C1
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_1_3_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_1_3_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000001C4)                                                  */
/*       SKX (0x000001C4)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_1_3_KTI_PHY_IOVB_REG 0x000001C4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_1_3_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000001C5)                                                  */
/*       SKX (0x000001C5)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_1_3_KTI_PHY_IOVB_REG 0x000001C5
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_1_3_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x000001C6)                                                  */
/*       SKX (0x000001C6)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_1_3_KTI_PHY_IOVB_REG 0x000001C6
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_1_3_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x000001C7)                                                  */
/*       SKX (0x000001C7)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_1_3_KTI_PHY_IOVB_REG 0x000001C7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_1_3_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_1_3_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x000001C9)                                                  */
/*       SKX (0x000001C9)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_1_3_KTI_PHY_IOVB_REG 0x000001C9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_1_3_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_1_3_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000001CD)                                                  */
/*       SKX (0x000001CD)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_1_3_KTI_PHY_IOVB_REG 0x000001CD
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_1_3_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_1_3_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000001CF)                                                  */
/*       SKX (0x000001CF)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_1_3_KTI_PHY_IOVB_REG 0x000001CF
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_1_3_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000001D0)                                                  */
/*       SKX (0x000001D0)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_1_3_KTI_PHY_IOVB_REG 0x000001D0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_1_3_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_1_3_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000001D3)                                                  */
/*       SKX (0x000001D3)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_1_3_KTI_PHY_IOVB_REG 0x000001D3
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_1_3_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000001D4)                                                  */
/*       SKX (0x000001D4)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_1_3_KTI_PHY_IOVB_REG 0x000001D4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_1_3_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_1_3_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000001D6)                                                  */
/*       SKX (0x000001D6)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_1_3_KTI_PHY_IOVB_REG 0x000001D6
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_1_3_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000001D7)                                                  */
/*       SKX (0x000001D7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_1_3_KTI_PHY_IOVB_REG 0x000001D7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_1_3_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000001D8)                                                  */
/*       SKX (0x000001D8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_1_3_KTI_PHY_IOVB_REG 0x000001D8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_1_3_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000001D9)                                                  */
/*       SKX (0x000001D9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_1_3_KTI_PHY_IOVB_REG 0x000001D9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_1_3_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_1_3_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000001DC)                                                  */
/*       SKX (0x000001DC)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_1_3_KTI_PHY_IOVB_REG 0x000001DC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_1_3_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_1_3_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000001E0)                                                  */
/*       SKX (0x000001E0)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_1_3_KTI_PHY_IOVB_REG 0x000001E0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_1_3_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_1_3_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000001E7)                                                  */
/*       SKX (0x000001E7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_1_3_KTI_PHY_IOVB_REG 0x000001E7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_1_3_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000001E8)                                                  */
/*       SKX (0x000001E8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_1_3_KTI_PHY_IOVB_REG 0x000001E8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_1_3_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000001E9)                                                  */
/*       SKX (0x000001E9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_1_3_KTI_PHY_IOVB_REG 0x000001E9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_1_3_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000001EA)                                                  */
/*       SKX (0x000001EA)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_1_3_KTI_PHY_IOVB_REG 0x000001EA
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_1_3_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_1_3_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000001EC)                                                  */
/*       SKX (0x000001EC)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_1_3_KTI_PHY_IOVB_REG 0x000001EC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_1_3_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_1_3_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000001ED)                                                  */
/*       SKX (0x000001ED)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_1_3_KTI_PHY_IOVB_REG 0x000001ED
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_1_3_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_1_3_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000001F4)                                                  */
/*       SKX (0x000001F4)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_1_3_KTI_PHY_IOVB_REG 0x000001F4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_1_3_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_0_4_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000201)                                                  */
/*       SKX (0x00000201)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_0_4_KTI_PHY_IOVB_REG 0x00000201
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_0_4_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_0_4_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000204)                                                  */
/*       SKX (0x00000204)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_0_4_KTI_PHY_IOVB_REG 0x00000204
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_0_4_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000205)                                                  */
/*       SKX (0x00000205)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_0_4_KTI_PHY_IOVB_REG 0x00000205
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_0_4_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000206)                                                  */
/*       SKX (0x00000206)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_0_4_KTI_PHY_IOVB_REG 0x00000206
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_0_4_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000207)                                                  */
/*       SKX (0x00000207)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_0_4_KTI_PHY_IOVB_REG 0x00000207
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_0_4_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_0_4_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000209)                                                  */
/*       SKX (0x00000209)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_0_4_KTI_PHY_IOVB_REG 0x00000209
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_0_4_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_0_4_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000020D)                                                  */
/*       SKX (0x0000020D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_0_4_KTI_PHY_IOVB_REG 0x0000020D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_0_4_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_0_4_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000020F)                                                  */
/*       SKX (0x0000020F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_0_4_KTI_PHY_IOVB_REG 0x0000020F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_0_4_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000210)                                                  */
/*       SKX (0x00000210)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_0_4_KTI_PHY_IOVB_REG 0x00000210
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_0_4_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_0_4_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000213)                                                  */
/*       SKX (0x00000213)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_0_4_KTI_PHY_IOVB_REG 0x00000213
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_0_4_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000214)                                                  */
/*       SKX (0x00000214)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_0_4_KTI_PHY_IOVB_REG 0x00000214
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_0_4_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_0_4_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000216)                                                  */
/*       SKX (0x00000216)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_0_4_KTI_PHY_IOVB_REG 0x00000216
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_0_4_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000217)                                                  */
/*       SKX (0x00000217)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_0_4_KTI_PHY_IOVB_REG 0x00000217
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_0_4_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000218)                                                  */
/*       SKX (0x00000218)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_0_4_KTI_PHY_IOVB_REG 0x00000218
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_0_4_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000219)                                                  */
/*       SKX (0x00000219)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_0_4_KTI_PHY_IOVB_REG 0x00000219
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_0_4_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_0_4_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000021C)                                                  */
/*       SKX (0x0000021C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_0_4_KTI_PHY_IOVB_REG 0x0000021C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_0_4_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_0_4_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000220)                                                  */
/*       SKX (0x00000220)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_0_4_KTI_PHY_IOVB_REG 0x00000220
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_0_4_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_0_4_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000227)                                                  */
/*       SKX (0x00000227)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_0_4_KTI_PHY_IOVB_REG 0x00000227
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_0_4_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000228)                                                  */
/*       SKX (0x00000228)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_0_4_KTI_PHY_IOVB_REG 0x00000228
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_0_4_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000229)                                                  */
/*       SKX (0x00000229)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_0_4_KTI_PHY_IOVB_REG 0x00000229
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_0_4_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x0000022A)                                                  */
/*       SKX (0x0000022A)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_0_4_KTI_PHY_IOVB_REG 0x0000022A
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_0_4_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_0_4_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000022C)                                                  */
/*       SKX (0x0000022C)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_0_4_KTI_PHY_IOVB_REG 0x0000022C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_0_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_0_4_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000022D)                                                  */
/*       SKX (0x0000022D)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_0_4_KTI_PHY_IOVB_REG 0x0000022D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_0_4_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_0_4_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000234)                                                  */
/*       SKX (0x00000234)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_0_4_KTI_PHY_IOVB_REG 0x00000234
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_0_4_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_1_4_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000241)                                                  */
/*       SKX (0x00000241)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_1_4_KTI_PHY_IOVB_REG 0x00000241
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_1_4_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_1_4_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000244)                                                  */
/*       SKX (0x00000244)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_1_4_KTI_PHY_IOVB_REG 0x00000244
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_1_4_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000245)                                                  */
/*       SKX (0x00000245)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_1_4_KTI_PHY_IOVB_REG 0x00000245
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_1_4_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000246)                                                  */
/*       SKX (0x00000246)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_1_4_KTI_PHY_IOVB_REG 0x00000246
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_1_4_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000247)                                                  */
/*       SKX (0x00000247)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_1_4_KTI_PHY_IOVB_REG 0x00000247
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_1_4_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_1_4_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000249)                                                  */
/*       SKX (0x00000249)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_1_4_KTI_PHY_IOVB_REG 0x00000249
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_1_4_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_1_4_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000024D)                                                  */
/*       SKX (0x0000024D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_1_4_KTI_PHY_IOVB_REG 0x0000024D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_1_4_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_1_4_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000024F)                                                  */
/*       SKX (0x0000024F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_1_4_KTI_PHY_IOVB_REG 0x0000024F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_1_4_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000250)                                                  */
/*       SKX (0x00000250)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_1_4_KTI_PHY_IOVB_REG 0x00000250
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_1_4_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_1_4_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000253)                                                  */
/*       SKX (0x00000253)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_1_4_KTI_PHY_IOVB_REG 0x00000253
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_1_4_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000254)                                                  */
/*       SKX (0x00000254)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_1_4_KTI_PHY_IOVB_REG 0x00000254
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_1_4_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_1_4_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000256)                                                  */
/*       SKX (0x00000256)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_1_4_KTI_PHY_IOVB_REG 0x00000256
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_1_4_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000257)                                                  */
/*       SKX (0x00000257)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_1_4_KTI_PHY_IOVB_REG 0x00000257
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_1_4_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000258)                                                  */
/*       SKX (0x00000258)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_1_4_KTI_PHY_IOVB_REG 0x00000258
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_1_4_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000259)                                                  */
/*       SKX (0x00000259)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_1_4_KTI_PHY_IOVB_REG 0x00000259
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_1_4_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_1_4_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000025C)                                                  */
/*       SKX (0x0000025C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_1_4_KTI_PHY_IOVB_REG 0x0000025C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_1_4_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_1_4_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000260)                                                  */
/*       SKX (0x00000260)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_1_4_KTI_PHY_IOVB_REG 0x00000260
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_1_4_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_1_4_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000267)                                                  */
/*       SKX (0x00000267)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_1_4_KTI_PHY_IOVB_REG 0x00000267
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_1_4_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000268)                                                  */
/*       SKX (0x00000268)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_1_4_KTI_PHY_IOVB_REG 0x00000268
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_1_4_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000269)                                                  */
/*       SKX (0x00000269)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_1_4_KTI_PHY_IOVB_REG 0x00000269
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_1_4_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x0000026A)                                                  */
/*       SKX (0x0000026A)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_1_4_KTI_PHY_IOVB_REG 0x0000026A
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_1_4_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_1_4_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000026C)                                                  */
/*       SKX (0x0000026C)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_1_4_KTI_PHY_IOVB_REG 0x0000026C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_1_4_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_1_4_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000026D)                                                  */
/*       SKX (0x0000026D)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_1_4_KTI_PHY_IOVB_REG 0x0000026D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_1_4_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_1_4_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000274)                                                  */
/*       SKX (0x00000274)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_1_4_KTI_PHY_IOVB_REG 0x00000274
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_1_4_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_0_5_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000281)                                                  */
/*       SKX (0x00000281)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_0_5_KTI_PHY_IOVB_REG 0x00000281
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_0_5_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_0_5_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000284)                                                  */
/*       SKX (0x00000284)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_0_5_KTI_PHY_IOVB_REG 0x00000284
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_0_5_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000285)                                                  */
/*       SKX (0x00000285)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_0_5_KTI_PHY_IOVB_REG 0x00000285
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_0_5_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000286)                                                  */
/*       SKX (0x00000286)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_0_5_KTI_PHY_IOVB_REG 0x00000286
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_0_5_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000287)                                                  */
/*       SKX (0x00000287)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_0_5_KTI_PHY_IOVB_REG 0x00000287
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_0_5_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_0_5_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000289)                                                  */
/*       SKX (0x00000289)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_0_5_KTI_PHY_IOVB_REG 0x00000289
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_0_5_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_0_5_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000028D)                                                  */
/*       SKX (0x0000028D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_0_5_KTI_PHY_IOVB_REG 0x0000028D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_0_5_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_0_5_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000028F)                                                  */
/*       SKX (0x0000028F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_0_5_KTI_PHY_IOVB_REG 0x0000028F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_0_5_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000290)                                                  */
/*       SKX (0x00000290)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_0_5_KTI_PHY_IOVB_REG 0x00000290
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_0_5_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_0_5_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000293)                                                  */
/*       SKX (0x00000293)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_0_5_KTI_PHY_IOVB_REG 0x00000293
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_0_5_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000294)                                                  */
/*       SKX (0x00000294)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_0_5_KTI_PHY_IOVB_REG 0x00000294
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_0_5_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_0_5_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000296)                                                  */
/*       SKX (0x00000296)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_0_5_KTI_PHY_IOVB_REG 0x00000296
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_0_5_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000297)                                                  */
/*       SKX (0x00000297)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_0_5_KTI_PHY_IOVB_REG 0x00000297
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_0_5_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000298)                                                  */
/*       SKX (0x00000298)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_0_5_KTI_PHY_IOVB_REG 0x00000298
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_0_5_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000299)                                                  */
/*       SKX (0x00000299)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_0_5_KTI_PHY_IOVB_REG 0x00000299
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_0_5_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_0_5_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000029C)                                                  */
/*       SKX (0x0000029C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_0_5_KTI_PHY_IOVB_REG 0x0000029C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_0_5_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_0_5_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000002A0)                                                  */
/*       SKX (0x000002A0)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_0_5_KTI_PHY_IOVB_REG 0x000002A0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_0_5_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_0_5_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000002A7)                                                  */
/*       SKX (0x000002A7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_0_5_KTI_PHY_IOVB_REG 0x000002A7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_0_5_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000002A8)                                                  */
/*       SKX (0x000002A8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_0_5_KTI_PHY_IOVB_REG 0x000002A8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_0_5_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000002A9)                                                  */
/*       SKX (0x000002A9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_0_5_KTI_PHY_IOVB_REG 0x000002A9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_0_5_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000002AA)                                                  */
/*       SKX (0x000002AA)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_0_5_KTI_PHY_IOVB_REG 0x000002AA
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_0_5_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_0_5_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000002AC)                                                  */
/*       SKX (0x000002AC)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_0_5_KTI_PHY_IOVB_REG 0x000002AC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_0_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_0_5_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000002AD)                                                  */
/*       SKX (0x000002AD)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_0_5_KTI_PHY_IOVB_REG 0x000002AD
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_0_5_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_0_5_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000002B4)                                                  */
/*       SKX (0x000002B4)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_0_5_KTI_PHY_IOVB_REG 0x000002B4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_0_5_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_1_5_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000002C1)                                                  */
/*       SKX (0x000002C1)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_1_5_KTI_PHY_IOVB_REG 0x000002C1
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_1_5_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_1_5_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000002C4)                                                  */
/*       SKX (0x000002C4)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_1_5_KTI_PHY_IOVB_REG 0x000002C4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_1_5_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000002C5)                                                  */
/*       SKX (0x000002C5)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_1_5_KTI_PHY_IOVB_REG 0x000002C5
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_1_5_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x000002C6)                                                  */
/*       SKX (0x000002C6)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_1_5_KTI_PHY_IOVB_REG 0x000002C6
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_1_5_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x000002C7)                                                  */
/*       SKX (0x000002C7)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_1_5_KTI_PHY_IOVB_REG 0x000002C7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_1_5_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_1_5_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x000002C9)                                                  */
/*       SKX (0x000002C9)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_1_5_KTI_PHY_IOVB_REG 0x000002C9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_1_5_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_1_5_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000002CD)                                                  */
/*       SKX (0x000002CD)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_1_5_KTI_PHY_IOVB_REG 0x000002CD
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_1_5_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_1_5_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000002CF)                                                  */
/*       SKX (0x000002CF)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_1_5_KTI_PHY_IOVB_REG 0x000002CF
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_1_5_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000002D0)                                                  */
/*       SKX (0x000002D0)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_1_5_KTI_PHY_IOVB_REG 0x000002D0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_1_5_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_1_5_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000002D3)                                                  */
/*       SKX (0x000002D3)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_1_5_KTI_PHY_IOVB_REG 0x000002D3
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_1_5_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000002D4)                                                  */
/*       SKX (0x000002D4)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_1_5_KTI_PHY_IOVB_REG 0x000002D4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_1_5_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_1_5_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000002D6)                                                  */
/*       SKX (0x000002D6)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_1_5_KTI_PHY_IOVB_REG 0x000002D6
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_1_5_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000002D7)                                                  */
/*       SKX (0x000002D7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_1_5_KTI_PHY_IOVB_REG 0x000002D7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_1_5_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000002D8)                                                  */
/*       SKX (0x000002D8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_1_5_KTI_PHY_IOVB_REG 0x000002D8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_1_5_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000002D9)                                                  */
/*       SKX (0x000002D9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_1_5_KTI_PHY_IOVB_REG 0x000002D9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_1_5_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_1_5_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000002DC)                                                  */
/*       SKX (0x000002DC)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_1_5_KTI_PHY_IOVB_REG 0x000002DC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_1_5_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_1_5_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000002E0)                                                  */
/*       SKX (0x000002E0)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_1_5_KTI_PHY_IOVB_REG 0x000002E0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_1_5_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_1_5_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000002E7)                                                  */
/*       SKX (0x000002E7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_1_5_KTI_PHY_IOVB_REG 0x000002E7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_1_5_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000002E8)                                                  */
/*       SKX (0x000002E8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_1_5_KTI_PHY_IOVB_REG 0x000002E8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_1_5_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000002E9)                                                  */
/*       SKX (0x000002E9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_1_5_KTI_PHY_IOVB_REG 0x000002E9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_1_5_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000002EA)                                                  */
/*       SKX (0x000002EA)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_1_5_KTI_PHY_IOVB_REG 0x000002EA
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_1_5_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_1_5_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000002EC)                                                  */
/*       SKX (0x000002EC)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_1_5_KTI_PHY_IOVB_REG 0x000002EC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_1_5_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_1_5_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000002ED)                                                  */
/*       SKX (0x000002ED)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_1_5_KTI_PHY_IOVB_REG 0x000002ED
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_1_5_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_1_5_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000002F4)                                                  */
/*       SKX (0x000002F4)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_1_5_KTI_PHY_IOVB_REG 0x000002F4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_1_5_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_0_6_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000301)                                                  */
/*       SKX (0x00000301)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_0_6_KTI_PHY_IOVB_REG 0x00000301
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_0_6_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_0_6_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000304)                                                  */
/*       SKX (0x00000304)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_0_6_KTI_PHY_IOVB_REG 0x00000304
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_0_6_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000305)                                                  */
/*       SKX (0x00000305)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_0_6_KTI_PHY_IOVB_REG 0x00000305
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_0_6_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000306)                                                  */
/*       SKX (0x00000306)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_0_6_KTI_PHY_IOVB_REG 0x00000306
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_0_6_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000307)                                                  */
/*       SKX (0x00000307)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_0_6_KTI_PHY_IOVB_REG 0x00000307
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_0_6_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_0_6_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000309)                                                  */
/*       SKX (0x00000309)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_0_6_KTI_PHY_IOVB_REG 0x00000309
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_0_6_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_0_6_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000030D)                                                  */
/*       SKX (0x0000030D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_0_6_KTI_PHY_IOVB_REG 0x0000030D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_0_6_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_0_6_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000030F)                                                  */
/*       SKX (0x0000030F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_0_6_KTI_PHY_IOVB_REG 0x0000030F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_0_6_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000310)                                                  */
/*       SKX (0x00000310)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_0_6_KTI_PHY_IOVB_REG 0x00000310
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_0_6_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_0_6_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000313)                                                  */
/*       SKX (0x00000313)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_0_6_KTI_PHY_IOVB_REG 0x00000313
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_0_6_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000314)                                                  */
/*       SKX (0x00000314)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_0_6_KTI_PHY_IOVB_REG 0x00000314
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_0_6_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_0_6_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000316)                                                  */
/*       SKX (0x00000316)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_0_6_KTI_PHY_IOVB_REG 0x00000316
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_0_6_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000317)                                                  */
/*       SKX (0x00000317)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_0_6_KTI_PHY_IOVB_REG 0x00000317
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_0_6_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000318)                                                  */
/*       SKX (0x00000318)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_0_6_KTI_PHY_IOVB_REG 0x00000318
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_0_6_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000319)                                                  */
/*       SKX (0x00000319)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_0_6_KTI_PHY_IOVB_REG 0x00000319
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_0_6_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_0_6_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000031C)                                                  */
/*       SKX (0x0000031C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_0_6_KTI_PHY_IOVB_REG 0x0000031C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_0_6_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_0_6_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000320)                                                  */
/*       SKX (0x00000320)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_0_6_KTI_PHY_IOVB_REG 0x00000320
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_0_6_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_0_6_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000327)                                                  */
/*       SKX (0x00000327)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_0_6_KTI_PHY_IOVB_REG 0x00000327
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_0_6_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000328)                                                  */
/*       SKX (0x00000328)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_0_6_KTI_PHY_IOVB_REG 0x00000328
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_0_6_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000329)                                                  */
/*       SKX (0x00000329)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_0_6_KTI_PHY_IOVB_REG 0x00000329
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_0_6_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x0000032A)                                                  */
/*       SKX (0x0000032A)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_0_6_KTI_PHY_IOVB_REG 0x0000032A
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_0_6_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_0_6_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000032C)                                                  */
/*       SKX (0x0000032C)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_0_6_KTI_PHY_IOVB_REG 0x0000032C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_0_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_0_6_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000032D)                                                  */
/*       SKX (0x0000032D)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_0_6_KTI_PHY_IOVB_REG 0x0000032D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_0_6_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_0_6_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000334)                                                  */
/*       SKX (0x00000334)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_0_6_KTI_PHY_IOVB_REG 0x00000334
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_0_6_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_1_6_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000341)                                                  */
/*       SKX (0x00000341)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_1_6_KTI_PHY_IOVB_REG 0x00000341
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_1_6_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_1_6_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000344)                                                  */
/*       SKX (0x00000344)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_1_6_KTI_PHY_IOVB_REG 0x00000344
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_1_6_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000345)                                                  */
/*       SKX (0x00000345)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_1_6_KTI_PHY_IOVB_REG 0x00000345
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_1_6_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000346)                                                  */
/*       SKX (0x00000346)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_1_6_KTI_PHY_IOVB_REG 0x00000346
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_1_6_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000347)                                                  */
/*       SKX (0x00000347)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_1_6_KTI_PHY_IOVB_REG 0x00000347
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_1_6_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_1_6_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000349)                                                  */
/*       SKX (0x00000349)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_1_6_KTI_PHY_IOVB_REG 0x00000349
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_1_6_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_1_6_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000034D)                                                  */
/*       SKX (0x0000034D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_1_6_KTI_PHY_IOVB_REG 0x0000034D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_1_6_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_1_6_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000034F)                                                  */
/*       SKX (0x0000034F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_1_6_KTI_PHY_IOVB_REG 0x0000034F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_1_6_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000350)                                                  */
/*       SKX (0x00000350)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_1_6_KTI_PHY_IOVB_REG 0x00000350
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_1_6_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_1_6_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000353)                                                  */
/*       SKX (0x00000353)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_1_6_KTI_PHY_IOVB_REG 0x00000353
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_1_6_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000354)                                                  */
/*       SKX (0x00000354)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_1_6_KTI_PHY_IOVB_REG 0x00000354
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_1_6_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_1_6_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000356)                                                  */
/*       SKX (0x00000356)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_1_6_KTI_PHY_IOVB_REG 0x00000356
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_1_6_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000357)                                                  */
/*       SKX (0x00000357)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_1_6_KTI_PHY_IOVB_REG 0x00000357
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_1_6_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000358)                                                  */
/*       SKX (0x00000358)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_1_6_KTI_PHY_IOVB_REG 0x00000358
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_1_6_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000359)                                                  */
/*       SKX (0x00000359)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_1_6_KTI_PHY_IOVB_REG 0x00000359
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_1_6_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_1_6_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000035C)                                                  */
/*       SKX (0x0000035C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_1_6_KTI_PHY_IOVB_REG 0x0000035C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_1_6_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_1_6_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000360)                                                  */
/*       SKX (0x00000360)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_1_6_KTI_PHY_IOVB_REG 0x00000360
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_1_6_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_1_6_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000367)                                                  */
/*       SKX (0x00000367)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_1_6_KTI_PHY_IOVB_REG 0x00000367
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_1_6_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000368)                                                  */
/*       SKX (0x00000368)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_1_6_KTI_PHY_IOVB_REG 0x00000368
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_1_6_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000369)                                                  */
/*       SKX (0x00000369)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_1_6_KTI_PHY_IOVB_REG 0x00000369
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_1_6_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x0000036A)                                                  */
/*       SKX (0x0000036A)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_1_6_KTI_PHY_IOVB_REG 0x0000036A
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_1_6_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_1_6_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000036C)                                                  */
/*       SKX (0x0000036C)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_1_6_KTI_PHY_IOVB_REG 0x0000036C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_1_6_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_1_6_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000036D)                                                  */
/*       SKX (0x0000036D)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_1_6_KTI_PHY_IOVB_REG 0x0000036D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_1_6_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_1_6_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000374)                                                  */
/*       SKX (0x00000374)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_1_6_KTI_PHY_IOVB_REG 0x00000374
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_1_6_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_0_7_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000381)                                                  */
/*       SKX (0x00000381)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_0_7_KTI_PHY_IOVB_REG 0x00000381
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_0_7_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_0_7_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000384)                                                  */
/*       SKX (0x00000384)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_0_7_KTI_PHY_IOVB_REG 0x00000384
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_0_7_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000385)                                                  */
/*       SKX (0x00000385)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_0_7_KTI_PHY_IOVB_REG 0x00000385
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_0_7_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000386)                                                  */
/*       SKX (0x00000386)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_0_7_KTI_PHY_IOVB_REG 0x00000386
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_0_7_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000387)                                                  */
/*       SKX (0x00000387)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_0_7_KTI_PHY_IOVB_REG 0x00000387
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_0_7_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_0_7_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000389)                                                  */
/*       SKX (0x00000389)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_0_7_KTI_PHY_IOVB_REG 0x00000389
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_0_7_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_0_7_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000038D)                                                  */
/*       SKX (0x0000038D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_0_7_KTI_PHY_IOVB_REG 0x0000038D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_0_7_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_0_7_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000038F)                                                  */
/*       SKX (0x0000038F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_0_7_KTI_PHY_IOVB_REG 0x0000038F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_0_7_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000390)                                                  */
/*       SKX (0x00000390)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_0_7_KTI_PHY_IOVB_REG 0x00000390
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_0_7_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_0_7_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000393)                                                  */
/*       SKX (0x00000393)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_0_7_KTI_PHY_IOVB_REG 0x00000393
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_0_7_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000394)                                                  */
/*       SKX (0x00000394)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_0_7_KTI_PHY_IOVB_REG 0x00000394
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_0_7_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_0_7_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000396)                                                  */
/*       SKX (0x00000396)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_0_7_KTI_PHY_IOVB_REG 0x00000396
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_0_7_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000397)                                                  */
/*       SKX (0x00000397)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_0_7_KTI_PHY_IOVB_REG 0x00000397
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_0_7_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000398)                                                  */
/*       SKX (0x00000398)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_0_7_KTI_PHY_IOVB_REG 0x00000398
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_0_7_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000399)                                                  */
/*       SKX (0x00000399)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_0_7_KTI_PHY_IOVB_REG 0x00000399
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_0_7_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_0_7_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000039C)                                                  */
/*       SKX (0x0000039C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_0_7_KTI_PHY_IOVB_REG 0x0000039C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_0_7_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_0_7_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000003A0)                                                  */
/*       SKX (0x000003A0)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_0_7_KTI_PHY_IOVB_REG 0x000003A0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_0_7_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_0_7_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000003A7)                                                  */
/*       SKX (0x000003A7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_0_7_KTI_PHY_IOVB_REG 0x000003A7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_0_7_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000003A8)                                                  */
/*       SKX (0x000003A8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_0_7_KTI_PHY_IOVB_REG 0x000003A8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_0_7_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000003A9)                                                  */
/*       SKX (0x000003A9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_0_7_KTI_PHY_IOVB_REG 0x000003A9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_0_7_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000003AA)                                                  */
/*       SKX (0x000003AA)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_0_7_KTI_PHY_IOVB_REG 0x000003AA
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_0_7_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_0_7_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000003AC)                                                  */
/*       SKX (0x000003AC)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_0_7_KTI_PHY_IOVB_REG 0x000003AC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_0_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_0_7_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000003AD)                                                  */
/*       SKX (0x000003AD)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_0_7_KTI_PHY_IOVB_REG 0x000003AD
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_0_7_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_0_7_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000003B4)                                                  */
/*       SKX (0x000003B4)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_0_7_KTI_PHY_IOVB_REG 0x000003B4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_0_7_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_1_7_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000003C1)                                                  */
/*       SKX (0x000003C1)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_1_7_KTI_PHY_IOVB_REG 0x000003C1
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_1_7_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_1_7_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000003C4)                                                  */
/*       SKX (0x000003C4)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_1_7_KTI_PHY_IOVB_REG 0x000003C4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_1_7_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000003C5)                                                  */
/*       SKX (0x000003C5)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_1_7_KTI_PHY_IOVB_REG 0x000003C5
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_1_7_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x000003C6)                                                  */
/*       SKX (0x000003C6)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_1_7_KTI_PHY_IOVB_REG 0x000003C6
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_1_7_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x000003C7)                                                  */
/*       SKX (0x000003C7)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_1_7_KTI_PHY_IOVB_REG 0x000003C7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_1_7_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_1_7_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x000003C9)                                                  */
/*       SKX (0x000003C9)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_1_7_KTI_PHY_IOVB_REG 0x000003C9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_1_7_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_1_7_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000003CD)                                                  */
/*       SKX (0x000003CD)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_1_7_KTI_PHY_IOVB_REG 0x000003CD
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_1_7_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_1_7_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000003CF)                                                  */
/*       SKX (0x000003CF)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_1_7_KTI_PHY_IOVB_REG 0x000003CF
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_1_7_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000003D0)                                                  */
/*       SKX (0x000003D0)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_1_7_KTI_PHY_IOVB_REG 0x000003D0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_1_7_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_1_7_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000003D3)                                                  */
/*       SKX (0x000003D3)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_1_7_KTI_PHY_IOVB_REG 0x000003D3
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_1_7_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000003D4)                                                  */
/*       SKX (0x000003D4)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_1_7_KTI_PHY_IOVB_REG 0x000003D4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_1_7_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_1_7_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000003D6)                                                  */
/*       SKX (0x000003D6)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_1_7_KTI_PHY_IOVB_REG 0x000003D6
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_1_7_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000003D7)                                                  */
/*       SKX (0x000003D7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_1_7_KTI_PHY_IOVB_REG 0x000003D7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_1_7_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000003D8)                                                  */
/*       SKX (0x000003D8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_1_7_KTI_PHY_IOVB_REG 0x000003D8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_1_7_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000003D9)                                                  */
/*       SKX (0x000003D9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_1_7_KTI_PHY_IOVB_REG 0x000003D9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_1_7_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_1_7_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000003DC)                                                  */
/*       SKX (0x000003DC)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_1_7_KTI_PHY_IOVB_REG 0x000003DC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_1_7_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_1_7_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000003E0)                                                  */
/*       SKX (0x000003E0)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_1_7_KTI_PHY_IOVB_REG 0x000003E0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_1_7_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_1_7_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000003E7)                                                  */
/*       SKX (0x000003E7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_1_7_KTI_PHY_IOVB_REG 0x000003E7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_1_7_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000003E8)                                                  */
/*       SKX (0x000003E8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_1_7_KTI_PHY_IOVB_REG 0x000003E8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_1_7_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000003E9)                                                  */
/*       SKX (0x000003E9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_1_7_KTI_PHY_IOVB_REG 0x000003E9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_1_7_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000003EA)                                                  */
/*       SKX (0x000003EA)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_1_7_KTI_PHY_IOVB_REG 0x000003EA
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_1_7_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_1_7_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000003EC)                                                  */
/*       SKX (0x000003EC)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_1_7_KTI_PHY_IOVB_REG 0x000003EC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_1_7_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_1_7_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000003ED)                                                  */
/*       SKX (0x000003ED)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_1_7_KTI_PHY_IOVB_REG 0x000003ED
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_1_7_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_1_7_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000003F4)                                                  */
/*       SKX (0x000003F4)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_1_7_KTI_PHY_IOVB_REG 0x000003F4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_1_7_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_0_8_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000401)                                                  */
/*       SKX (0x00000401)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_0_8_KTI_PHY_IOVB_REG 0x00000401
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_0_8_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_0_8_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000404)                                                  */
/*       SKX (0x00000404)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_0_8_KTI_PHY_IOVB_REG 0x00000404
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_0_8_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000405)                                                  */
/*       SKX (0x00000405)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_0_8_KTI_PHY_IOVB_REG 0x00000405
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_0_8_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000406)                                                  */
/*       SKX (0x00000406)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_0_8_KTI_PHY_IOVB_REG 0x00000406
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_0_8_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000407)                                                  */
/*       SKX (0x00000407)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_0_8_KTI_PHY_IOVB_REG 0x00000407
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_0_8_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_0_8_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000409)                                                  */
/*       SKX (0x00000409)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_0_8_KTI_PHY_IOVB_REG 0x00000409
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_0_8_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_0_8_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000040D)                                                  */
/*       SKX (0x0000040D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_0_8_KTI_PHY_IOVB_REG 0x0000040D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_0_8_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_0_8_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000040F)                                                  */
/*       SKX (0x0000040F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_0_8_KTI_PHY_IOVB_REG 0x0000040F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_0_8_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000410)                                                  */
/*       SKX (0x00000410)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_0_8_KTI_PHY_IOVB_REG 0x00000410
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_0_8_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_0_8_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000413)                                                  */
/*       SKX (0x00000413)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_0_8_KTI_PHY_IOVB_REG 0x00000413
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_0_8_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000414)                                                  */
/*       SKX (0x00000414)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_0_8_KTI_PHY_IOVB_REG 0x00000414
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_0_8_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_0_8_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000416)                                                  */
/*       SKX (0x00000416)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_0_8_KTI_PHY_IOVB_REG 0x00000416
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_0_8_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000417)                                                  */
/*       SKX (0x00000417)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_0_8_KTI_PHY_IOVB_REG 0x00000417
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_0_8_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000418)                                                  */
/*       SKX (0x00000418)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_0_8_KTI_PHY_IOVB_REG 0x00000418
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_0_8_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000419)                                                  */
/*       SKX (0x00000419)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_0_8_KTI_PHY_IOVB_REG 0x00000419
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_0_8_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_0_8_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000041C)                                                  */
/*       SKX (0x0000041C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_0_8_KTI_PHY_IOVB_REG 0x0000041C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_0_8_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_0_8_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000420)                                                  */
/*       SKX (0x00000420)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_0_8_KTI_PHY_IOVB_REG 0x00000420
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_0_8_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_0_8_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000427)                                                  */
/*       SKX (0x00000427)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_0_8_KTI_PHY_IOVB_REG 0x00000427
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_0_8_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000428)                                                  */
/*       SKX (0x00000428)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_0_8_KTI_PHY_IOVB_REG 0x00000428
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_0_8_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000429)                                                  */
/*       SKX (0x00000429)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_0_8_KTI_PHY_IOVB_REG 0x00000429
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_0_8_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x0000042A)                                                  */
/*       SKX (0x0000042A)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_0_8_KTI_PHY_IOVB_REG 0x0000042A
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_0_8_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_0_8_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000042C)                                                  */
/*       SKX (0x0000042C)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_0_8_KTI_PHY_IOVB_REG 0x0000042C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_0_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_0_8_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000042D)                                                  */
/*       SKX (0x0000042D)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_0_8_KTI_PHY_IOVB_REG 0x0000042D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_0_8_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_0_8_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000434)                                                  */
/*       SKX (0x00000434)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_0_8_KTI_PHY_IOVB_REG 0x00000434
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_0_8_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_1_8_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000441)                                                  */
/*       SKX (0x00000441)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_1_8_KTI_PHY_IOVB_REG 0x00000441
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_1_8_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_1_8_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000444)                                                  */
/*       SKX (0x00000444)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_1_8_KTI_PHY_IOVB_REG 0x00000444
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_1_8_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000445)                                                  */
/*       SKX (0x00000445)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_1_8_KTI_PHY_IOVB_REG 0x00000445
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_1_8_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000446)                                                  */
/*       SKX (0x00000446)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_1_8_KTI_PHY_IOVB_REG 0x00000446
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_1_8_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000447)                                                  */
/*       SKX (0x00000447)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_1_8_KTI_PHY_IOVB_REG 0x00000447
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_1_8_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_1_8_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000449)                                                  */
/*       SKX (0x00000449)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_1_8_KTI_PHY_IOVB_REG 0x00000449
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_1_8_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_1_8_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000044D)                                                  */
/*       SKX (0x0000044D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_1_8_KTI_PHY_IOVB_REG 0x0000044D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_1_8_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_1_8_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000044F)                                                  */
/*       SKX (0x0000044F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_1_8_KTI_PHY_IOVB_REG 0x0000044F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_1_8_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000450)                                                  */
/*       SKX (0x00000450)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_1_8_KTI_PHY_IOVB_REG 0x00000450
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_1_8_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_1_8_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000453)                                                  */
/*       SKX (0x00000453)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_1_8_KTI_PHY_IOVB_REG 0x00000453
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_1_8_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000454)                                                  */
/*       SKX (0x00000454)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_1_8_KTI_PHY_IOVB_REG 0x00000454
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_1_8_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_1_8_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000456)                                                  */
/*       SKX (0x00000456)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_1_8_KTI_PHY_IOVB_REG 0x00000456
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_1_8_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000457)                                                  */
/*       SKX (0x00000457)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_1_8_KTI_PHY_IOVB_REG 0x00000457
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_1_8_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000458)                                                  */
/*       SKX (0x00000458)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_1_8_KTI_PHY_IOVB_REG 0x00000458
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_1_8_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000459)                                                  */
/*       SKX (0x00000459)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_1_8_KTI_PHY_IOVB_REG 0x00000459
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_1_8_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_1_8_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000045C)                                                  */
/*       SKX (0x0000045C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_1_8_KTI_PHY_IOVB_REG 0x0000045C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_1_8_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_1_8_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000460)                                                  */
/*       SKX (0x00000460)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_1_8_KTI_PHY_IOVB_REG 0x00000460
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_1_8_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_1_8_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000467)                                                  */
/*       SKX (0x00000467)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_1_8_KTI_PHY_IOVB_REG 0x00000467
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_1_8_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000468)                                                  */
/*       SKX (0x00000468)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_1_8_KTI_PHY_IOVB_REG 0x00000468
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_1_8_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x00000469)                                                  */
/*       SKX (0x00000469)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_1_8_KTI_PHY_IOVB_REG 0x00000469
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_1_8_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x0000046A)                                                  */
/*       SKX (0x0000046A)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_1_8_KTI_PHY_IOVB_REG 0x0000046A
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_1_8_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_1_8_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000046C)                                                  */
/*       SKX (0x0000046C)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_1_8_KTI_PHY_IOVB_REG 0x0000046C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_1_8_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_1_8_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x0000046D)                                                  */
/*       SKX (0x0000046D)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_1_8_KTI_PHY_IOVB_REG 0x0000046D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_1_8_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_1_8_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000474)                                                  */
/*       SKX (0x00000474)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_1_8_KTI_PHY_IOVB_REG 0x00000474
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_1_8_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_0_9_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000481)                                                  */
/*       SKX (0x00000481)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_0_9_KTI_PHY_IOVB_REG 0x00000481
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_0_9_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_0_9_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000484)                                                  */
/*       SKX (0x00000484)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_0_9_KTI_PHY_IOVB_REG 0x00000484
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_0_9_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x00000485)                                                  */
/*       SKX (0x00000485)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_0_9_KTI_PHY_IOVB_REG 0x00000485
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_0_9_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x00000486)                                                  */
/*       SKX (0x00000486)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_0_9_KTI_PHY_IOVB_REG 0x00000486
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_0_9_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000487)                                                  */
/*       SKX (0x00000487)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_0_9_KTI_PHY_IOVB_REG 0x00000487
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_0_9_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_0_9_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x00000489)                                                  */
/*       SKX (0x00000489)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_0_9_KTI_PHY_IOVB_REG 0x00000489
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_0_9_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_0_9_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000048D)                                                  */
/*       SKX (0x0000048D)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_0_9_KTI_PHY_IOVB_REG 0x0000048D
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_0_9_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_0_9_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x0000048F)                                                  */
/*       SKX (0x0000048F)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_0_9_KTI_PHY_IOVB_REG 0x0000048F
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_0_9_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000490)                                                  */
/*       SKX (0x00000490)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_0_9_KTI_PHY_IOVB_REG 0x00000490
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_0_9_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_0_9_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x00000493)                                                  */
/*       SKX (0x00000493)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_0_9_KTI_PHY_IOVB_REG 0x00000493
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_0_9_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x00000494)                                                  */
/*       SKX (0x00000494)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_0_9_KTI_PHY_IOVB_REG 0x00000494
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_0_9_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_0_9_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000496)                                                  */
/*       SKX (0x00000496)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_0_9_KTI_PHY_IOVB_REG 0x00000496
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_0_9_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000497)                                                  */
/*       SKX (0x00000497)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_0_9_KTI_PHY_IOVB_REG 0x00000497
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_0_9_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000498)                                                  */
/*       SKX (0x00000498)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_0_9_KTI_PHY_IOVB_REG 0x00000498
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_0_9_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x00000499)                                                  */
/*       SKX (0x00000499)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_0_9_KTI_PHY_IOVB_REG 0x00000499
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_0_9_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_0_9_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x0000049C)                                                  */
/*       SKX (0x0000049C)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_0_9_KTI_PHY_IOVB_REG 0x0000049C
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_0_9_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_0_9_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000004A0)                                                  */
/*       SKX (0x000004A0)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_0_9_KTI_PHY_IOVB_REG 0x000004A0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_0_9_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_0_9_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000004A7)                                                  */
/*       SKX (0x000004A7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_0_9_KTI_PHY_IOVB_REG 0x000004A7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_0_9_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000004A8)                                                  */
/*       SKX (0x000004A8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_0_9_KTI_PHY_IOVB_REG 0x000004A8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_0_9_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000004A9)                                                  */
/*       SKX (0x000004A9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_0_9_KTI_PHY_IOVB_REG 0x000004A9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_0_9_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000004AA)                                                  */
/*       SKX (0x000004AA)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_0_9_KTI_PHY_IOVB_REG 0x000004AA
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_0_9_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_0_9_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000004AC)                                                  */
/*       SKX (0x000004AC)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_0_9_KTI_PHY_IOVB_REG 0x000004AC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_0_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_0_9_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000004AD)                                                  */
/*       SKX (0x000004AD)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_0_9_KTI_PHY_IOVB_REG 0x000004AD
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_0_9_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_0_9_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000004B4)                                                  */
/*       SKX (0x000004B4)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_0_9_KTI_PHY_IOVB_REG 0x000004B4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_0_9_KTI_PHY_IOVB_STRUCT;












/* IOVB_RX_CTL0_1_9_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000004C1)                                                  */
/*       SKX (0x000004C1)                                                     */
/* Register default value:              0xC0                                  */
#define IOVB_RX_CTL0_1_9_KTI_PHY_IOVB_REG 0x000004C1
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_rtermdis : 1;
    /* rx_rtermdis - Bits[0:0], RWS, default = 1'b0  */
    UINT8 rx_rtermdis_ovr : 1;
    /* rx_rtermdis_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 rx_rtermpullhighn : 2;
    /* rx_rtermpullhighn - Bits[3:2], RWS, default = 2'b0  */
    UINT8 rx_rtermpullhighp : 2;
    /* rx_rtermpullhighp - Bits[5:4], RWS, default = 2'b0  */
    UINT8 rx_dfe_summer_ctl : 2;
    /* rx_dfe_summer_ctl - Bits[7:6], RWS, default = 2'd3  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTL0_1_9_KTI_PHY_IOVB_STRUCT;






/* IOVB_RX_CTLE0_1_9_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000004C4)                                                  */
/*       SKX (0x000004C4)                                                     */
/* Register default value:              0x01                                  */
#define IOVB_RX_CTLE0_1_9_KTI_PHY_IOVB_REG 0x000004C4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_ctle_offset_en : 1;
    /* rx_ctle_offset_en - Bits[0:0], RWS, default = 1'b1 
        
       	1 = Enable Receive CTLE offset correction
       	0 = disable
       	Note: this is needed on a lane basis for HVM testing
             
     */
    UINT8 rx_ctle_peak : 5;
    /* rx_ctle_peak - Bits[5:1], RWS, default = 5'h0 
        
        Full speed encoded number to degeneration capacitance atoms to turn on in the 
       first stage of the CTLE 
       	00000 = Disable equalization, boost DC gain.
       	Note: This is a bundle level signal
             
     */
    UINT8 rx_ctle_vcm_ovrd_dis : 1;
    /* rx_ctle_vcm_ovrd_dis - Bits[6:6], RWS, default = 1'b0 
        
       	1 = Disable CTLE vcm override
       	0= enable CTLE internal common mode
       	This is per bundle control.
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 1;
    /* rsvd - Bits[7:7], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE0_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_RX_VREF0_1_9_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000004C5)                                                  */
/*       SKX (0x000004C5)                                                     */
/* Register default value:              0x6D                                  */
#define IOVB_RX_VREF0_1_9_KTI_PHY_IOVB_REG 0x000004C5
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX VREF Control 0 Register
 */
typedef union {
  struct {
    UINT8 rx_vref_sel : 5;
    /* rx_vref_sel - Bits[4:0], RWS, default = 5'b01101  */
    UINT8 rx_vcm_sum_sel : 1;
    /* rx_vcm_sum_sel - Bits[5:5], RWS, default = 1'b1  */
    UINT8 rx_vcm_ds_sel : 2;
    /* rx_vcm_ds_sel - Bits[7:6], RWS, default = 2'b1  */
  } Bits;
  UINT8 Data;
} IOVB_RX_VREF0_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_SPARE0_1_9_KTI_PHY_IOVB_REG supported on:                             */
/*       SKX_A0 (0x000004C6)                                                  */
/*       SKX (0x000004C6)                                                     */
/* Register default value:              0x80                                  */
#define IOVB_SPARE0_1_9_KTI_PHY_IOVB_REG 0x000004C6
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB spare 0 Register
 */
typedef union {
  struct {
    UINT8 spare0 : 8;
    /* spare0 - Bits[7:0], RWS, default = 8'h80  */
  } Bits;
  UINT8 Data;
} IOVB_SPARE0_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_IG0_1_9_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x000004C7)                                                  */
/*       SKX (0x000004C7)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IG0_1_9_KTI_PHY_IOVB_REG 0x000004C7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Inegral Gain Control 0 Register
 */
typedef union {
  struct {
    UINT8 ig_acq : 3;
    /* ig_acq - Bits[2:0], RWS, default = 3'h0  */
    UINT8 ig_track : 3;
    /* ig_track - Bits[5:3], RWS, default = 3'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IG0_1_9_KTI_PHY_IOVB_STRUCT;




/* IOVB_PG1_1_9_KTI_PHY_IOVB_REG supported on:                                */
/*       SKX_A0 (0x000004C9)                                                  */
/*       SKX (0x000004C9)                                                     */
/* Register default value:              0x0A                                  */
#define IOVB_PG1_1_9_KTI_PHY_IOVB_REG 0x000004C9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Proportional Gain Control 1 Register
 */
typedef union {
  struct {
    UINT8 pg_track : 6;
    /* pg_track - Bits[5:0], RWS, default = 6'd10  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_PG1_1_9_KTI_PHY_IOVB_STRUCT;








/* IOVB_CDR3_1_9_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000004CD)                                                  */
/*       SKX (0x000004CD)                                                     */
/* Register default value:              0x40                                  */
#define IOVB_CDR3_1_9_KTI_PHY_IOVB_REG 0x000004CD
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB CDR Control 3 Register
 */
typedef union {
  struct {
    UINT8 cdr_pmod_peak : 6;
    /* cdr_pmod_peak - Bits[5:0], RWS, default = 6'b0  */
    UINT8 cdr_acq_length : 2;
    /* cdr_acq_length - Bits[7:6], RWS, default = 2'h1  */
  } Bits;
  UINT8 Data;
} IOVB_CDR3_1_9_KTI_PHY_IOVB_STRUCT;




/* IOVB_AGC0_1_9_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000004CF)                                                  */
/*       SKX (0x000004CF)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AGC0_1_9_KTI_PHY_IOVB_REG 0x000004CF
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 0 Register
 */
typedef union {
  struct {
    UINT8 agc_open_loop : 1;
    /* agc_open_loop - Bits[0:0], RWS, default = 1'b0  */
    UINT8 agc_open_loop_ovr : 1;
    /* agc_open_loop_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 agc_force_acq : 1;
    /* agc_force_acq - Bits[2:2], RWS, default = 1'b0  */
    UINT8 agc_update_rate_acq : 2;
    /* agc_update_rate_acq - Bits[4:3], RWS, default = 2'b0  */
    UINT8 agc_update_rate_track : 2;
    /* agc_update_rate_track - Bits[6:5], RWS, default = 2'b0  */
    UINT8 rx_agc_use_err_data : 1;
    /* rx_agc_use_err_data - Bits[7:7], RWS, default = 1'b0  */
  } Bits;
  UINT8 Data;
} IOVB_AGC0_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_AGC1_1_9_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000004D0)                                                  */
/*       SKX (0x000004D0)                                                     */
/* Register default value:              0x1F                                  */
#define IOVB_AGC1_1_9_KTI_PHY_IOVB_REG 0x000004D0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AGC Control 1 Register
 */
typedef union {
  struct {
    UINT8 agc_gain_acq : 2;
    /* agc_gain_acq - Bits[1:0], RWS, default = 2'h3  */
    UINT8 agc_gain_track : 2;
    /* agc_gain_track - Bits[3:2], RWS, default = 2'h3  */
    UINT8 agc_acq_length : 2;
    /* agc_acq_length - Bits[5:4], RWS, default = 2'h1  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_AGC1_1_9_KTI_PHY_IOVB_STRUCT;






/* IOVB_DFE2_1_9_KTI_PHY_IOVB_REG supported on:                               */
/*       SKX_A0 (0x000004D3)                                                  */
/*       SKX (0x000004D3)                                                     */
/* Register default value:              0x05                                  */
#define IOVB_DFE2_1_9_KTI_PHY_IOVB_REG 0x000004D3
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB DFE Control 2 Register
 */
typedef union {
  struct {
    UINT8 rx_dfesum_mfc_10gen : 2;
    /* rx_dfesum_mfc_10gen - Bits[1:0], RWS, default = 2'h1  */
    UINT8 rx_dfelsb_sel : 2;
    /* rx_dfelsb_sel - Bits[3:2], RWS, default = 2'h1  */
    UINT8 bkch_threshold : 2;
    /* bkch_threshold - Bits[5:4], RWS, default = 2'h0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_DFE2_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_CTL0_1_9_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000004D4)                                                  */
/*       SKX (0x000004D4)                                                     */
/* Register default value:              0x23                                  */
#define IOVB_TX_CTL0_1_9_KTI_PHY_IOVB_REG 0x000004D4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX Control 0 Register
 */
typedef union {
  struct {
    UINT8 tx_align_en : 1;
    /* tx_align_en - Bits[0:0], RWS, default = 1'b1 
        
       	Enable TX data alignment (TX FIFO enable?)
             
     */
    UINT8 tx_align_en_ovr : 1;
    /* tx_align_en_ovr - Bits[1:1], RWS, default = 1'b1 
        
       	override enable for tx_align_en
             
     */
    UINT8 tx_spare : 4;
    /* tx_spare - Bits[5:2], RWS, default = 4'h8 
        
             [3]: (*) reset CDR FSM and accumulator at exit of L1. (VREF FSM can be 
       reset at L1 exit by setting agc_udpate_rate_track[0]) 
             [2]: simulation hook to skip DDR calibration. Mask off 
       pcie_rate_change_det to RXTXDIG_OFFSCAL 
             [1:0]: unused
             
     */
    UINT8 tx_lcl_icomp_dis : 1;
    /* tx_lcl_icomp_dis - Bits[6:6], RWS, default = 1'b0 
        
       	Local icomp en.
       	0 =start the local TX block icomp loop. Should be enabled for normal operation.
        1 =stop the local TX block icomp loop. For debug or HVM mode to check the RTerm 
       legs. 
        Note: This signal is overidden by the power-up sequence logic in the common 
       block (comdig) and will not affect the icomp loop until the icomp calibration 
       has completed. Need the local loop to accurately measure the legs. 
             
     */
    UINT8 tx_sense_lane : 1;
    /* tx_sense_lane - Bits[7:7], RWS, default = 1'b0 
        
       	Lane 0 identifier for KTI
       	0= this bundle does not contain KTI lane 0
       	1= this bundle contains KTI lane 0
        When bit= 1, the lane will not enter L1 state, but the lane can still be in L0s 
       and perform Rx Detect 
        (NOTE: This behavior is not entirely correct for KTI, so the KTI agent will not 
       use it but instead will simply not assert L0s on its lane 0.) 
       	Note: This is a bundle level signal
             
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_CTL0_1_9_KTI_PHY_IOVB_STRUCT;




/* IOVB_TXEQ_LVL0_1_9_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000004D6)                                                  */
/*       SKX (0x000004D6)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL0_1_9_KTI_PHY_IOVB_REG 0x000004D6
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl0 : 6;
    /* txeq_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL0_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL1_1_9_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000004D7)                                                  */
/*       SKX (0x000004D7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL1_1_9_KTI_PHY_IOVB_REG 0x000004D7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl1 : 6;
    /* txeq_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL1_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL2_1_9_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000004D8)                                                  */
/*       SKX (0x000004D8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL2_1_9_KTI_PHY_IOVB_REG 0x000004D8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl2 : 6;
    /* txeq_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL2_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_LVL3_1_9_KTI_PHY_IOVB_REG supported on:                          */
/*       SKX_A0 (0x000004D9)                                                  */
/*       SKX (0x000004D9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_LVL3_1_9_KTI_PHY_IOVB_REG 0x000004D9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_lvl3 : 6;
    /* txeq_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_LVL3_1_9_KTI_PHY_IOVB_STRUCT;






/* IOVB_AFE_CTL0_1_9_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000004DC)                                                  */
/*       SKX (0x000004DC)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_AFE_CTL0_1_9_KTI_PHY_IOVB_REG 0x000004DC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB AFE Control 0 Register
 */
typedef union {
  struct {
    UINT8 oc_start : 1;
    /* oc_start - Bits[0:0], RWS, default = 1'b0  */
    UINT8 oc_start_ovr : 1;
    /* oc_start_ovr - Bits[1:1], RWS, default = 1'b0  */
    UINT8 dfx_bdl_sel : 1;
    /* dfx_bdl_sel - Bits[2:2], RWS, default = 1'b0  */
    UINT8 eyemon_trig : 1;
    /* eyemon_trig - Bits[3:3], RWS_V, default = 1'b0  */
    UINT8 nearend_lpbk_en : 1;
    /* nearend_lpbk_en - Bits[4:4], RWS, default = 1'b0  */
    UINT8 farend_lpbk_en : 1;
    /* farend_lpbk_en - Bits[5:5], RWS, default = 1'b0  */
    UINT8 psa_cfg : 2;
    /* psa_cfg - Bits[7:6], RWS, default = 2'd0  */
  } Bits;
  UINT8 Data;
} IOVB_AFE_CTL0_1_9_KTI_PHY_IOVB_STRUCT;








/* IOVB_IREF_OVR_1_9_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000004E0)                                                  */
/*       SKX (0x000004E0)                                                     */
/* Register default value:              0x00                                  */
#define IOVB_IREF_OVR_1_9_KTI_PHY_IOVB_REG 0x000004E0
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB_IREF Override Register
 */
typedef union {
  struct {
    UINT8 ism_iref_ovr_en : 1;
    /* ism_iref_ovr_en - Bits[0:0], RWS, default = 1'b0  */
    UINT8 ism_iref : 5;
    /* ism_iref - Bits[5:1], RWS, default = 5'b0  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_IREF_OVR_1_9_KTI_PHY_IOVB_STRUCT;














/* IOVB_TXEQ_HALF_LVL0_1_9_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000004E7)                                                  */
/*       SKX (0x000004E7)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL0_1_9_KTI_PHY_IOVB_REG 0x000004E7
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level0 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl0 : 6;
    /* txeq_half_lvl0 - Bits[5:0], RWS, default = 6'h3F 
        
       	Transmit Equalization Half Speed Level0 coefficients for FIR settings
       	See UniPHY MAS tables for settings.
        NOTE: The CSRs controlling txeq_lvl* have no effect in Slow Mode (KTI), during 
       which the KTI agent hardwires the value to 3Fh. 
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL0_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL1_1_9_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000004E8)                                                  */
/*       SKX (0x000004E8)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL1_1_9_KTI_PHY_IOVB_REG 0x000004E8
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level1 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl1 : 6;
    /* txeq_half_lvl1 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL1_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL2_1_9_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000004E9)                                                  */
/*       SKX (0x000004E9)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL2_1_9_KTI_PHY_IOVB_REG 0x000004E9
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level2 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl2 : 6;
    /* txeq_half_lvl2 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL2_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TXEQ_HALF_LVL3_1_9_KTI_PHY_IOVB_REG supported on:                     */
/*       SKX_A0 (0x000004EA)                                                  */
/*       SKX (0x000004EA)                                                     */
/* Register default value:              0x3F                                  */
#define IOVB_TXEQ_HALF_LVL3_1_9_KTI_PHY_IOVB_REG 0x000004EA
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB Transmit Equalization Half Speed Level3 Cofficients
 */
typedef union {
  struct {
    UINT8 txeq_half_lvl3 : 6;
    /* txeq_half_lvl3 - Bits[5:0], RWS, default = 6'h3F  */
    UINT8 rsvd : 2;
    /* rsvd - Bits[7:6], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TXEQ_HALF_LVL3_1_9_KTI_PHY_IOVB_STRUCT;




/* IOVB_TX_VREF_1_9_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000004EC)                                                  */
/*       SKX (0x000004EC)                                                     */
/* Register default value:              0x1E                                  */
#define IOVB_TX_VREF_1_9_KTI_PHY_IOVB_REG 0x000004EC
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX VREF Register
 */
typedef union {
  struct {
    UINT8 tx_vref_sel : 5;
    /* tx_vref_sel - Bits[4:0], RWS, default = 5'h1E 
        
       	Comparator reference voltage select
       	This bit alone controls comparator reference voltage
       	Note: This is a bundle level signal
             
     */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_VREF_1_9_KTI_PHY_IOVB_STRUCT;


/* IOVB_TX_IREF_1_9_KTI_PHY_IOVB_REG supported on:                            */
/*       SKX_A0 (0x000004ED)                                                  */
/*       SKX (0x000004ED)                                                     */
/* Register default value:              0x13                                  */
#define IOVB_TX_IREF_1_9_KTI_PHY_IOVB_REG 0x000004ED
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB TX IREF Register
 */
typedef union {
  struct {
    UINT8 tx_iref_ctl : 5;
    /* tx_iref_ctl - Bits[4:0], RWS, default = 5'h13  */
    UINT8 rsvd : 3;
    /* rsvd - Bits[7:5], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT8 Data;
} IOVB_TX_IREF_1_9_KTI_PHY_IOVB_STRUCT;














/* IOVB_RX_CTLE2_1_9_KTI_PHY_IOVB_REG supported on:                           */
/*       SKX_A0 (0x000004F4)                                                  */
/*       SKX (0x000004F4)                                                     */
/* Register default value:              0x44                                  */
#define IOVB_RX_CTLE2_1_9_KTI_PHY_IOVB_REG 0x000004F4
/* Struct format extracted from XML file SKX\3.14.1.WINDOW_0.xml.
 * IOVB RX CTLE control 2 Register
 */
typedef union {
  struct {
    UINT8 ctle_rdeg_fullspeed : 4;
    /* ctle_rdeg_fullspeed - Bits[3:0], RWS, default = 4'h4  */
    UINT8 ctle_rdeg_halfspeed : 4;
    /* ctle_rdeg_halfspeed - Bits[7:4], RWS, default = 4'h4  */
  } Bits;
  UINT8 Data;
} IOVB_RX_CTLE2_1_9_KTI_PHY_IOVB_STRUCT;












#endif /* KTI_PHY_IOVB_h */
