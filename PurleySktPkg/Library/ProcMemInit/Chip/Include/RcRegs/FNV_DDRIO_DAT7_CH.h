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

#ifndef FNV_DDRIO_DAT7_CH_h
#define FNV_DDRIO_DAT7_CH_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* FNV_DDRIO_DAT7_CH_DEV 22                                                   */
/* FNV_DDRIO_DAT7_CH_FUN 1                                                    */

/* RXGROUP0N0RANK0_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1200)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK0_7_FNV_DDRIO_DAT7_CH_REG 0x050F4200
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 or CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS (0: 1/16 UI 
       offset, [br] 
                 1: 3/16 UI Offset, 2: 5/16 UI Offsets, 3: 7/16 UI Offsets, 4: 9/16 UI 
       Offsets, 5,6,7: 11/16 UI Offsets.) 
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK1_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1204)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK1_7_FNV_DDRIO_DAT7_CH_REG 0x050F4204
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank1. Multicast read for CH012 or CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK2_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1208)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK2_7_FNV_DDRIO_DAT7_CH_REG 0x050F4208
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank2. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK2_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK3_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B120C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK3_7_FNV_DDRIO_DAT7_CH_REG 0x050F420C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank3. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK3_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK4_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1210)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK4_7_FNV_DDRIO_DAT7_CH_REG 0x050F4210
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank4. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK4_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK5_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1214)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK5_7_FNV_DDRIO_DAT7_CH_REG 0x050F4214
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank5. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       mux select for crossover from ClkPiRef to CkPi4RcvEn, used in xover2to2 mode
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK5_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK6_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1218)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK6_7_FNV_DDRIO_DAT7_CH_REG 0x050F4218
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank6. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK6_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK7_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B121C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK7_7_FNV_DDRIO_DAT7_CH_REG 0x050F421C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank7. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK7_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK0_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1220)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK0_7_FNV_DDRIO_DAT7_CH_REG 0x050F4220
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK1_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1224)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK1_7_FNV_DDRIO_DAT7_CH_REG 0x050F4224
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK2_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1228)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK2_7_FNV_DDRIO_DAT7_CH_REG 0x050F4228
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK2_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK3_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B122C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK3_7_FNV_DDRIO_DAT7_CH_REG 0x050F422C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK3_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK4_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1230)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK4_7_FNV_DDRIO_DAT7_CH_REG 0x050F4230
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK4_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK5_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1234)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK5_7_FNV_DDRIO_DAT7_CH_REG 0x050F4234
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK5_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK6_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1238)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK6_7_FNV_DDRIO_DAT7_CH_REG 0x050F4238
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK6_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK7_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B123C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK7_7_FNV_DDRIO_DAT7_CH_REG 0x050F423C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK7_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK0_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1240)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK0_7_FNV_DDRIO_DAT7_CH_REG 0x050F4240
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       select one of the four DQ Clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK1_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1244)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK1_7_FNV_DDRIO_DAT7_CH_REG 0x050F4244
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK2_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1248)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK2_7_FNV_DDRIO_DAT7_CH_REG 0x050F4248
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK2_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK3_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B124C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK3_7_FNV_DDRIO_DAT7_CH_REG 0x050F424C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK3_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK4_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1250)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK4_7_FNV_DDRIO_DAT7_CH_REG 0x050F4250
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK4_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK5_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1254)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK5_7_FNV_DDRIO_DAT7_CH_REG 0x050F4254
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 3;
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK5_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK6_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1258)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK6_7_FNV_DDRIO_DAT7_CH_REG 0x050F4258
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK6_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK7_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B125C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK7_7_FNV_DDRIO_DAT7_CH_REG 0x050F425C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK7_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK0_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1260)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK0_7_FNV_DDRIO_DAT7_CH_REG 0x050F4260
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte. 0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK1_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1264)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK1_7_FNV_DDRIO_DAT7_CH_REG 0x050F4264
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK2_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1268)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK2_7_FNV_DDRIO_DAT7_CH_REG 0x050F4268
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK2_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK3_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B126C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK3_7_FNV_DDRIO_DAT7_CH_REG 0x050F426C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK3_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK4_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1270)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK4_7_FNV_DDRIO_DAT7_CH_REG 0x050F4270
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK4_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK5_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1274)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK5_7_FNV_DDRIO_DAT7_CH_REG 0x050F4274
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK5_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK6_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1278)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK6_7_FNV_DDRIO_DAT7_CH_REG 0x050F4278
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK6_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK7_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B127C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK7_7_FNV_DDRIO_DAT7_CH_REG 0x050F427C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK7_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK0_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1280)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK0_7_FNV_DDRIO_DAT7_CH_REG 0x050F4280
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       selects which skewed DQSP (to sample even bits) is used in the RX FIFO. For 
       proper operation, always pick latest DQSP 
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       selects which skewed DQSN (to sample odd bits) is used in the RX FIFO. For 
       proper operation, always pick latest DQSN 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK1_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1284)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK1_7_FNV_DDRIO_DAT7_CH_REG 0x050F4284
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK2_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1288)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK2_7_FNV_DDRIO_DAT7_CH_REG 0x050F4288
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK2_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK3_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B128C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK3_7_FNV_DDRIO_DAT7_CH_REG 0x050F428C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK3_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK4_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1290)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK4_7_FNV_DDRIO_DAT7_CH_REG 0x050F4290
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK4_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK5_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1294)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK5_7_FNV_DDRIO_DAT7_CH_REG 0x050F4294
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK5_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK6_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1298)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK6_7_FNV_DDRIO_DAT7_CH_REG 0x050F4298
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK6_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK7_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B129C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK7_7_FNV_DDRIO_DAT7_CH_REG 0x050F429C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK7_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXXTALKN0_7_FNV_DDRIO_DAT7_CH_REG supported on:                            */
/*       EKV (0x401B12A0)                                                     */
/* Register default value:              0x00000000                            */
#define TXXTALKN0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42A0
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * TX Crosstalk Cancellation Per Lane Write CoEfficents. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 twototwologicdelayrank0 : 4;
    /* twototwologicdelayrank0 - Bits[3:0], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 0
     */
    UINT32 twototwologicdelayrank1 : 4;
    /* twototwologicdelayrank1 - Bits[7:4], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 1
     */
    UINT32 twototwologicdelayrank2 : 4;
    /* twototwologicdelayrank2 - Bits[11:8], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 2
     */
    UINT32 twototwologicdelayrank3 : 4;
    /* twototwologicdelayrank3 - Bits[15:12], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 3
     */
    UINT32 twototwologicdelayrank4 : 4;
    /* twototwologicdelayrank4 - Bits[19:16], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 4
     */
    UINT32 twototwologicdelayrank5 : 4;
    /* twototwologicdelayrank5 - Bits[23:20], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 5
     */
    UINT32 twototwologicdelayrank6 : 4;
    /* twototwologicdelayrank6 - Bits[27:24], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 6
     */
    UINT32 twototwologicdelayrank7 : 4;
    /* twototwologicdelayrank7 - Bits[31:28], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 7
     */
  } Bits;
  UINT32 Data;
} TXXTALKN0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXVREFCTRLN0_7_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B12A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42A4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * RxVrefCtrl. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 vrefperbit0 : 7;
    /* vrefperbit0 - Bits[6:0], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben0 : 1;
    /* halflsben0 - Bits[7:7], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit1 : 7;
    /* vrefperbit1 - Bits[14:8], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben1 : 1;
    /* halflsben1 - Bits[15:15], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefperbit2 : 7;
    /* vrefperbit2 - Bits[22:16], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben2 : 1;
    /* halflsben2 - Bits[23:23], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefperbit3 : 7;
    /* vrefperbit3 - Bits[30:24], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben3 : 1;
    /* halflsben3 - Bits[31:31], RW, default = 1'b0 
       shift down the Vref voltage by ~0.5 stepsize
     */
  } Bits;
  UINT32 Data;
} RXVREFCTRLN0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATATRAINFEEDBACKN0_7_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B12A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42A8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Training Feedback Results. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 datatrainfeedback : 9;
    /* datatrainfeedback - Bits[8:0], RW, default = 9'b0 
       Data Training Feedback Storage[br]
       [br]
               DataTrainFeedback Description:[br]
                Training Step    Bits    Nibble    Description[br]
               --------------    ----    -------  --------------------------[br]
               Cross Over Cal     0      0        Tx Dqs Phase Detection[br]
               Cross Over Cal     1      0        Tx Dq Phase Detection[br]
               Cross Over Cal     2      0        Rec En Phase Detection[br]
               RcvEn              8:0    0        Byte detection[br]
       [br]
               Wr Leveling        8:0    0        Byte detection[br]
               Senseamp offset    3:0    0        Dq Senseamp offset Detection
     */
    UINT32 rsvd_9 : 23;
  } Bits;
  UINT32 Data;
} DATATRAINFEEDBACKN0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRFSMCALN0_7_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B12C8)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRFSMCALN0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42C8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Fsm for  Voc and Xover caliberation, Voc caliberation is a procedure that cancel 
 * out intrinsic offset in analogue receivers and Xover training ,it aligns all the 
 * local dll to the global qclk 
 */
typedef union {
  struct {
    UINT32 saoffset0fsm : 5;
    /* saoffset0fsm - Bits[4:0], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of first receiver is done(read 
       only) 
     */
    UINT32 saoffset1fsm : 5;
    /* saoffset1fsm - Bits[9:5], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of second receiver is done(read 
       only) 
     */
    UINT32 saoffset2fsm : 5;
    /* saoffset2fsm - Bits[14:10], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of third receiver is done(read 
       only) 
     */
    UINT32 saoffset3fsm : 5;
    /* saoffset3fsm - Bits[19:15], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of fourth receiver is done(read 
       only) 
     */
    UINT32 fsmcalcomplete : 1;
    /* fsmcalcomplete - Bits[20:20], ROV, default = 1'b0 
        when the caliberation is done
     */
    UINT32 refpiclkdelayfsm : 6;
    /* refpiclkdelayfsm - Bits[26:21], ROV, default = 6'b0 
       Load FSM with a code at which , the transition is detected.
     */
    UINT32 fsmcaldone : 1;
    /* fsmcaldone - Bits[27:27], ROV, default = 1'b0 
       when the caliberation of voc or x-over is done
     */
    UINT32 fsmtimersel : 1;
    /* fsmtimersel - Bits[28:28], RW, default = 1'b0 
       Timer is set for either 8 cycles or 16 cycles
     */
    UINT32 fsmpatternsel : 1;
    /* fsmpatternsel - Bits[29:29], RW, default = 1'b0 
        It selects whther the pattern of three bit or two bit is selected when the 
       transition happen from 1-->0 for pdout and receiver , to avoid glitch 
     */
    UINT32 fsmvoccal : 1;
    /* fsmvoccal - Bits[30:30], RW, default = 1'b0 
        This bit tells , wether the xover or voc caliberation will be done, if its zero 
       x-over caliberation is done and vice-versa 
     */
    UINT32 fsmcalen : 1;
    /* fsmcalen - Bits[31:31], RW, default = 1'b0 
       FsmCalEn starts the voc-Xover Fsm
     */
  } Bits;
  UINT32 Data;
} DDRCRFSMCALN0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL4N0_7_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B12CC)                                                     */
/* Register default value:              0x801F0000                            */
#define DATACONTROL4N0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42CC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    UINT32 vsshiorvrefctl1 : 8;
    /* vsshiorvrefctl1 - Bits[8:1], RW, default = 8'b0 
        VssHi control bits[br]
               Field Description: [br]
       [br]
                Field             Bits    Description[br]
               -------            ----    ---------------------------------[br]
       	 disoverflow        24     Disable overflow logic from BDW[br]
         fast panic enable  25     Bypass 2 flops in panic path for faster panic 
       response[br] 
         vsshivccd2by3cfgen 26     Forces the driver DAC to get supply from 2/3 
       vccd[br] 
         vsshivccd2by3cfg   27:29  Current trim setting for 2/3 vccd mode, 001 fast, 
       011 typical, 111 slow silicon [br] 
       	 ddrvssh2ndcompen   30     Enable second comparator (ZBB mode)[br]
         comp high bwen     31     Improve comparator performance to support low 
       digital voltage range 
     */
    UINT32 dqtxeqsel : 1;
    /* dqtxeqsel - Bits[9:9], RW, default = 1'b0 
       DQ TxEq setting selection to achieve constant EQ
     */
    UINT32 rsvd_10 : 2;
    UINT32 vrefpwrsave : 1;
    /* vrefpwrsave - Bits[12:12], RW, default = 1'h0 
                If set to 0 (default), Vref generators selected through RxVrefSel[3:0] 
       are always on.[br] 
                If set to 1, Vref generators are switched off while in Self Refresh.
     */
    UINT32 vreftonblbus : 4;
    /* vreftonblbus - Bits[16:13], RW, default = 4'b1000 
       Per-bit Enable connection to share Vref Bus
     */
    UINT32 rxvrefsel : 4;
    /* rxvrefsel - Bits[20:17], RW, default = 4'b1111 
       Per Bit Verf generator Enable
     */
    UINT32 refpiclkdelay : 6;
    /* refpiclkdelay - Bits[26:21], RW, default = 6'b000000 
       RefPiClkDelay
     */
    UINT32 fnvcrsdlbypassen : 1;
    /* fnvcrsdlbypassen - Bits[27:27], RW, default = 1'b0 
       FNV Slave Delay Line Bypass Enable
     */
    UINT32 fnvcrdllbypassen : 1;
    /* fnvcrdllbypassen - Bits[28:28], RW, default = 1'b0 
       FNV DLL Bypass Enable
     */
    UINT32 fnvcrvisaen : 1;
    /* fnvcrvisaen - Bits[29:29], RW, default = 1'b0 
       FNV Visa Fub Enable - data fub used for VISA if asserted
     */
    UINT32 writecrcenable : 1;
    /* writecrcenable - Bits[30:30], RW, default = 1'h0 
       Enable write crc mode
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DATACONTROL4N0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* ATTACK1SELN0_7_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B12D0)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK1SELN0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42D0
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr1d0 : 3;
    /* attackr1d0 - Bits[2:0], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[0] in a byte
     */
    UINT32 attackr1d1 : 3;
    /* attackr1d1 - Bits[5:3], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[1] in a byte
     */
    UINT32 attackr1d2 : 3;
    /* attackr1d2 - Bits[8:6], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[2] in a byte
     */
    UINT32 attackr1d3 : 3;
    /* attackr1d3 - Bits[11:9], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[3] in a byte
     */
    UINT32 attackr1d4 : 3;
    /* attackr1d4 - Bits[14:12], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[4] in a byte
     */
    UINT32 attackr1d5 : 3;
    /* attackr1d5 - Bits[17:15], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[5] in a byte
     */
    UINT32 attackr1d6 : 3;
    /* attackr1d6 - Bits[20:18], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[6] in a byte
     */
    UINT32 attackr1d7 : 3;
    /* attackr1d7 - Bits[23:21], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK1SELN0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* COMPDATA0N0_7_FNV_DDRIO_DAT7_CH_REG supported on:                          */
/*       EKV (0x401B12D4)                                                     */
/* Register default value:              0x00028000                            */
#define COMPDATA0N0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42D4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[5:0], RWV, default = 6'b0 
       Data Rcomp DrvUp Comp Value can be overriden via message channel if periodical 
       retrain is disable 
     */
    UINT32 spare0 : 3;
    /* spare0 - Bits[8:6], RWV, default = 3'b0 
       Spare
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[14:9], RWV, default = 6'b0 
       Data Rcomp DrvDown Comp Value
     */
    UINT32 vtcomp : 5;
    /* vtcomp - Bits[19:15], RWV, default = 5'b00101 
       VT Comp Values
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[26:20], RWV, default = 7'b0 
       Data TCO Comp Value.
     */
    UINT32 slewratecomp : 5;
    /* slewratecomp - Bits[31:27], RWV, default = 5'b0 
       Data Slew Rate Comp Value.
     */
  } Bits;
  UINT32 Data;
} COMPDATA0N0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* COMPDATA1N0_7_FNV_DDRIO_DAT7_CH_REG supported on:                          */
/*       EKV (0x401B12D8)                                                     */
/* Register default value:              0x04010000                            */
#define COMPDATA1N0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42D8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rcompodtup : 6;
    /* rcompodtup - Bits[5:0], RWV, default = 6'b0 
       Data Rcomp OdtUp Comp Value
     */
    UINT32 rcompodtdown : 6;
    /* rcompodtdown - Bits[11:6], RWV, default = 6'b0 
       Data Rcomp Odt-Down Comp Value
     */
    UINT32 panicdrvdn : 10;
    /* panicdrvdn - Bits[21:12], RWV, default = 10'b0000010000 
       Panic DrvDn RComp Value.
     */
    UINT32 panicdrvup : 10;
    /* panicdrvup - Bits[31:22], RWV, default = 10'b0000010000 
       Panic DrvUp RComp Comp Value.
     */
  } Bits;
  UINT32 Data;
} COMPDATA1N0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATAOFFSETTRAINN0_7_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B12DC)                                                     */
/* Register default value:              0x00000000                            */
#define DATAOFFSETTRAINN0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42DC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rcvenoffset : 6;
    /* rcvenoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained RxRcvEn Value.
     */
    UINT32 rxdqsoffset : 7;
    /* rxdqsoffset - Bits[12:6], RW, default = 7'b0 
       Offset added to Trained Rx DQS Value.
     */
    UINT32 txdqoffset : 6;
    /* txdqoffset - Bits[18:13], RW, default = 6'b00 
       Offset added to Trained Tx DQ Value.
     */
    UINT32 txdqsoffset : 6;
    /* txdqsoffset - Bits[24:19], RW, default = 6'b000000 
       Ooffset added to Trained Tx DQS Value.
     */
    UINT32 vrefoffset : 7;
    /* vrefoffset - Bits[31:25], RW, default = 7'b0000000 
       2s Compliment offset added to Trained Vref Value.  Positive number increases 
       Vref, and each step is Vdd/384. 
     */
  } Bits;
  UINT32 Data;
} DATAOFFSETTRAINN0_7_FNV_DDRIO_DAT7_CH_STRUCT;




/* ATTACK0SELN0_7_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B12E4)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK0SELN0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42E4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 txddjcen : 4;
    /* txddjcen - Bits[15:12], RW, default = 4'b000 
       TxddjcEn
     */
    UINT32 dfeenable : 1;
    /* dfeenable - Bits[16:16], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfedlysel : 1;
    /* dfedlysel - Bits[17:17], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfecode : 4;
    /* dfecode - Bits[21:18], RW, default = 4'b0000 
       xtalk aggressor0 select for dq bit[7] in a byte
     */
    UINT32 rsvd_22 : 10;
  } Bits;
  UINT32 Data;
} ATTACK0SELN0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL0N0_7_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B12E8)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL0N0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42E8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxtrainingmode : 1;
    /* rxtrainingmode - Bits[0:0], RW, default = 1'b0 
       Enables Read DqDqs Training Mode
     */
    UINT32 wltrainingmode : 1;
    /* wltrainingmode - Bits[1:1], RW, default = 1'b0 
       Enables Write Leveling Training Mode
     */
    UINT32 rltrainingmode : 1;
    /* rltrainingmode - Bits[2:2], RW, default = 1'b0 
       Enables Read Leveling Training Mode.
     */
    UINT32 senseamptrainingmode : 1;
    /* senseamptrainingmode - Bits[3:3], RW, default = 1'b0 
       Enables SenseAmp offset cancellation training mode.
     */
    UINT32 txon : 1;
    /* txon - Bits[4:4], RW, default = 1'b0 
       When set, forces DQ/DQS drive enable to active.  Used in power measurements and 
       IO loopback mode 
     */
    UINT32 rfon : 1;
    /* rfon - Bits[5:5], RW, default = 1'b0 
       Power-down Disable:  Forces register file read, overriding the rank power down 
       logic. 
     */
    UINT32 rxpion : 1;
    /* rxpion - Bits[6:6], RW, default = 1'b0 
       Power-down Disable:  Forces RxDqs PI clocks on.
     */
    UINT32 txpion : 1;
    /* txpion - Bits[7:7], RW, default = 1'b0 
       Power-down Disable:  Forces TxDq and TxDqs PI clocks on.
     */
    UINT32 internalclockson : 1;
    /* internalclockson - Bits[8:8], RW, default = 1'b0 
                 Forces on master DLL and all PI enables ON, despite both channel in 
       SelfRefresh, etc. low power states. 
     */
    UINT32 saoffsettraintxon : 1;
    /* saoffsettraintxon - Bits[9:9], RW, default = 1'b0 
       SaOffsetTrainTxon
     */
    UINT32 txdisable : 1;
    /* txdisable - Bits[10:10], RW, default = 1'b0 
       Power-down:  All write commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 rxdisable : 1;
    /* rxdisable - Bits[11:11], RW, default = 1'b0 
       Power-down:  All read commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 txlong : 1;
    /* txlong - Bits[12:12], RW, default = 1'b0 
       Sets the duration of the first transmitted DQ bit of the burst. 0=1UI, 1=2UI.
     */
    UINT32 rsvd_13 : 5;
    UINT32 driversegmentenable : 1;
    /* driversegmentenable - Bits[18:18], RW, default = 1'b0 
       Controls which segements of the driver are enabled:  {0: All, 1: Only ODT}.
     */
    UINT32 rsvd_19 : 1;
    UINT32 readrfrd : 1;
    /* readrfrd - Bits[20:20], RWV, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a read command to Rank = ReadRFRank 
     */
    UINT32 readrfwr : 1;
    /* readrfwr - Bits[21:21], RWV, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a write command to Rank = ReadRFRank 
     */
    UINT32 readrfrank : 3;
    /* readrfrank - Bits[24:22], RWV, default = 3'b000 
       Specifies the rank that ReadRFRd or ReadRFWr will manually download the CR 
       values from 
     */
    UINT32 forceodton : 1;
    /* forceodton - Bits[25:25], RW, default = 1'b0 
       ODT is forced-on.
     */
    UINT32 odtsampoff : 1;
    /* odtsampoff - Bits[26:26], RW, default = 1'b0 
       ODT and Senseamp are forced-off.
     */
    UINT32 disableodtstatic : 1;
    /* disableodtstatic - Bits[27:27], RW, default = 1'b0 
       not used
     */
    UINT32 ddrcrforceodton : 1;
    /* ddrcrforceodton - Bits[28:28], RW, default = 1'b0 
       force ODT to the transmitter on, regardless of state of DriveEnable
     */
    UINT32 spare : 1;
    /* spare - Bits[29:29], RW, default = 1'b0 
       spare
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[30:30], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
    UINT32 longpreambleenable : 1;
    /* longpreambleenable - Bits[31:31], RW, default = 1'b0 
       Enables long preamble for DDR4 devices
     */
  } Bits;
  UINT32 Data;
} DATACONTROL0N0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL1N0_7_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B12EC)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL1N0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42EC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 xtalkpienable : 1;
    /* xtalkpienable - Bits[0:0], RW, default = 1'b0 
       enable phase shift based crosstalk cancellation.
     */
    UINT32 xtalkpisign : 1;
    /* xtalkpisign - Bits[1:1], RW, default = 1'b0 
       select if 0: capacitive coupling; or 1: inductive coupling; xtalk dominant
     */
    UINT32 xtalkpidelta : 3;
    /* xtalkpidelta - Bits[4:2], RW, default = 3'b0 
       delta phase shift in one aggressor is in effect
     */
    UINT32 xtalkpideltarank5 : 2;
    /* xtalkpideltarank5 - Bits[6:5], RW, default = 2'b0 
       XtalkPiDeltaRank5
     */
    UINT32 rsvd_7 : 3;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 3'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll[br] 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 3'b000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 3'b000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL2N0_7_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B12F0)                                                     */
/* Register default value:              0x07800000                            */
#define DATACONTROL2N0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42F0
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier).[br] 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency 
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = 1'h0 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = 1'h0 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = 1'h0 
       Enable Imode EQ per byte lane. [br]
                 0: Imode completely off [br]
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 enearlywriterankqnnnh : 1;
    /* enearlywriterankqnnnh - Bits[11:11], RW, default = 1'h0 
       Spare
     */
    UINT32 picodepipeqnnnh : 1;
    /* picodepipeqnnnh - Bits[12:12], RW, default = 1'h0 
       Spare
     */
    UINT32 spare : 2;
    /* spare - Bits[14:13], RW, default = 2'h0 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = 1'h0 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = 4'h0 
       Binary Imode Coefficient, sets Imode current strength.[br]
                 (used for either Xtalk cancellation or swing boost)[br]
                 Code   Imode Current (approx)[br]
                 ------------------[br]
                 0000        0[br]
                 0001        1[br]
                 0010        2[br]
                   *[br]
                   *[br]
                   *[br]
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = 1'h0 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL3N0_7_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B12F4)                                                     */
/* Register default value:              0x2198DFCF                            */
#define DATACONTROL3N0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42F4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 drvstaticlegcfg : 2;
    /* drvstaticlegcfg - Bits[1:0], RW, default = 2'b11 
       binary default setting for static legs in DRV mode[br]
                  0    0       0 legs (EQ disabled)[br]
                  0    1       3 legs (EQ max 3 legs)[br]
                  1    0       6 legs (EQ max 6 legs)[br]
                  1    1      12 legs (EQ max 12 legs)[br]
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher [br] 
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit [br] 
                 the higher Ron target value.
     */
    UINT32 odtstaticlegcfg : 2;
    /* odtstaticlegcfg - Bits[3:2], RW, default = 2'b11 
       binary default setting for static legs in ODT mode[br]
                  0    0       0 legs (EQ disabled)[br]
                  0    1       3 legs (EQ max 3 legs)[br]
                  1    0       6 legs (EQ max 6 legs)[br]
                  1    1      12 legs (EQ max 12 legs)[br]
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher [br] 
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit [br] 
                 the higher Ron target value.
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[4:4], RW, default = 1'b0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rsvd_5 : 1;
    UINT32 odtsegmentenable : 3;
    /* odtsegmentenable - Bits[8:6], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.  [br]
                            After that, a shift register will determine the enabled 
       segments 
     */
    UINT32 datasegmentenable : 3;
    /* datasegmentenable - Bits[11:9], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.
     */
    UINT32 longodtr2w : 1;
    /* longodtr2w - Bits[12:12], RW, default = 1'b1 
       When set, it will extends ODT by 1 qclk on read to write turnarounds. hsd 230773
     */
    UINT32 spare2 : 1;
    /* spare2 - Bits[13:13], RW, default = 1'b0 
       Spare2
     */
    UINT32 imodebiasvrefen : 1;
    /* imodebiasvrefen - Bits[14:14], RW, default = 1'b1 
       imodebias control
     */
    UINT32 imodebiasrxbiastrim : 3;
    /* imodebiasrxbiastrim - Bits[17:15], RW, default = 3'b001 
       imodebias control. Also used for rxbiasana
     */
    UINT32 spare : 1;
    /* spare - Bits[18:18], RW, default = 1'b0 
       Spare
     */
    UINT32 imodebiasdfxddr4legup : 1;
    /* imodebiasdfxddr4legup - Bits[19:19], RW, default = 1'b1 
       imodebias control
     */
    UINT32 imodebiasdfxlegdn : 1;
    /* imodebiasdfxlegdn - Bits[20:20], RW, default = 1'b1 
       imodebias control
     */
    UINT32 ddrcrctlecapen : 2;
    /* ddrcrctlecapen - Bits[22:21], RW, default = 2'b00 
       ctle capacitor setting
     */
    UINT32 ddrcrctleresen : 2;
    /* ddrcrctleresen - Bits[24:23], RW, default = 2'b11 
       ctle resistor setting
     */
    UINT32 pcasbiasclosedloopen : 1;
    /* pcasbiasclosedloopen - Bits[25:25], RW, default = 1'b0 
       RxBias CR
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[26:26], RW, default = 1'b0 
       RxBias CR
     */
    UINT32 rxbiasfoldedlegtrim : 2;
    /* rxbiasfoldedlegtrim - Bits[28:27], RW, default = 2'b00 
       RxBias CR
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[30:29], RW, default = 2'b01 
       RxBias CR
     */
    UINT32 postambleenable : 1;
    /* postambleenable - Bits[31:31], RW, default = 1'h0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DATACONTROL3N0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* VSSHIORVREFCONTROLN0_7_FNV_DDRIO_DAT7_CH_REG supported on:                 */
/*       EKV (0x401B12F8)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROLN0_7_FNV_DDRIO_DAT7_CH_REG 0x050F42F8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings.[br] 
       [br]
               VssHi Field Description: [br]
       [br]
                Field          Bits    Description[br]
               -------         ----    
       ---------------------------------------------------------------------------------------------------------------------------------------------------------------[br] 
               SelCode          23     Selects the code to output:  {0: Code, 1: 
       TCode}[br] 
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode   [br] 
               PanicVoltage  21:18  Unsigned integer controlling the voltage error on 
       VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV)[br] 
               CloseLoop        17     Enables the VssHi close loop tracking [br]
               PanicEn          16     Enables the VssHi Panic Driver[br]
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError)[br] 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved[br] 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128}[br] 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROLN0_7_FNV_DDRIO_DAT7_CH_STRUCT;




/* RXGROUP0N1RANK0_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1300)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK0_7_FNV_DDRIO_DAT7_CH_REG 0x050F4300
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK1_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1304)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK1_7_FNV_DDRIO_DAT7_CH_REG 0x050F4304
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK2_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1308)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK2_7_FNV_DDRIO_DAT7_CH_REG 0x050F4308
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0.Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK2_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK3_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B130C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK3_7_FNV_DDRIO_DAT7_CH_REG 0x050F430C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK3_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK4_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1310)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK4_7_FNV_DDRIO_DAT7_CH_REG 0x050F4310
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK4_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK5_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1314)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK5_7_FNV_DDRIO_DAT7_CH_REG 0x050F4314
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK5_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK6_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1318)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK6_7_FNV_DDRIO_DAT7_CH_REG 0x050F4318
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK6_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK7_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B131C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK7_7_FNV_DDRIO_DAT7_CH_REG 0x050F431C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK7_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK0_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1320)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK0_7_FNV_DDRIO_DAT7_CH_REG 0x050F4320
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK1_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1324)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK1_7_FNV_DDRIO_DAT7_CH_REG 0x050F4324
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK2_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1328)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK2_7_FNV_DDRIO_DAT7_CH_REG 0x050F4328
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK2_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK3_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B132C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK3_7_FNV_DDRIO_DAT7_CH_REG 0x050F432C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK3_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK4_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1330)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK4_7_FNV_DDRIO_DAT7_CH_REG 0x050F4330
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK4_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK5_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1334)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK5_7_FNV_DDRIO_DAT7_CH_REG 0x050F4334
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK5_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK6_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1338)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK6_7_FNV_DDRIO_DAT7_CH_REG 0x050F4338
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK6_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK7_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B133C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK7_7_FNV_DDRIO_DAT7_CH_REG 0x050F433C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK7_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK0_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1340)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK0_7_FNV_DDRIO_DAT7_CH_REG 0x050F4340
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the four clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK1_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1344)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK1_7_FNV_DDRIO_DAT7_CH_REG 0x050F4344
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the four clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK2_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1348)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK2_7_FNV_DDRIO_DAT7_CH_REG 0x050F4348
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK2_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK3_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B134C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK3_7_FNV_DDRIO_DAT7_CH_REG 0x050F434C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK3_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK4_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1350)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK4_7_FNV_DDRIO_DAT7_CH_REG 0x050F4350
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       select one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK4_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK5_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1354)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK5_7_FNV_DDRIO_DAT7_CH_REG 0x050F4354
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled [br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK5_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK6_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1358)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK6_7_FNV_DDRIO_DAT7_CH_REG 0x050F4358
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK6_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK7_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B135C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK7_7_FNV_DDRIO_DAT7_CH_REG 0x050F435C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK7_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK0_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1360)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK0_7_FNV_DDRIO_DAT7_CH_REG 0x050F4360
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK1_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1364)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK1_7_FNV_DDRIO_DAT7_CH_REG 0x050F4364
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK2_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1368)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK2_7_FNV_DDRIO_DAT7_CH_REG 0x050F4368
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK2_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK3_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B136C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK3_7_FNV_DDRIO_DAT7_CH_REG 0x050F436C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK3_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK4_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1370)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK4_7_FNV_DDRIO_DAT7_CH_REG 0x050F4370
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK4_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK5_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1374)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK5_7_FNV_DDRIO_DAT7_CH_REG 0x050F4374
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK5_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK6_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1378)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK6_7_FNV_DDRIO_DAT7_CH_REG 0x050F4378
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK6_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK7_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B137C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK7_7_FNV_DDRIO_DAT7_CH_REG 0x050F437C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK7_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK0_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1380)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK0_7_FNV_DDRIO_DAT7_CH_REG 0x050F4380
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK0_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK1_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1384)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK1_7_FNV_DDRIO_DAT7_CH_REG 0x050F4384
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK2_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1388)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK2_7_FNV_DDRIO_DAT7_CH_REG 0x050F4388
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK2_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK3_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B138C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK3_7_FNV_DDRIO_DAT7_CH_REG 0x050F438C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK3_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK4_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1390)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK4_7_FNV_DDRIO_DAT7_CH_REG 0x050F4390
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK4_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK5_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1394)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK5_7_FNV_DDRIO_DAT7_CH_REG 0x050F4394
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK5_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK6_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1398)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK6_7_FNV_DDRIO_DAT7_CH_REG 0x050F4398
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK6_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK7_7_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B139C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK7_7_FNV_DDRIO_DAT7_CH_REG 0x050F439C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK7_7_FNV_DDRIO_DAT7_CH_STRUCT;




/* RXVREFCTRLN1_7_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B13A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43A4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * RxVrefCtrl. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 vrefperbit0 : 7;
    /* vrefperbit0 - Bits[6:0], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben0 : 1;
    /* halflsben0 - Bits[7:7], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit1 : 7;
    /* vrefperbit1 - Bits[14:8], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben1 : 1;
    /* halflsben1 - Bits[15:15], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit2 : 7;
    /* vrefperbit2 - Bits[22:16], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben2 : 1;
    /* halflsben2 - Bits[23:23], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit3 : 7;
    /* vrefperbit3 - Bits[30:24], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben3 : 1;
    /* halflsben3 - Bits[31:31], RW, default = 1'b0 
       shift down the Vref voltage by ~0.5 stepsize
     */
  } Bits;
  UINT32 Data;
} RXVREFCTRLN1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATATRAINFEEDBACKN1_7_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B13A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43A8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Training Feedback Results. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 datatrainfeedback : 9;
    /* datatrainfeedback - Bits[8:0], RW, default = 9'b0 
       Data Training Feedback Storage [br]
                Training Step    Bits    Nibble    Description[br]
               --------------    ----    -------  ---------------[br]
               Cross Over Cal     0      1        Tx Dqs Phase Detection[br]
               Cross Over Cal     1      1        Tx Dq Phase Detection[br]
               Cross Over Cal     2      1        Rec En Phase Detection[br]
               RcvEn              8:0    1        Byte detection[br]
               Wr Leveling        8:0    1        Byte detection[br]
               Senseamp offset    3:0    1        Dq Senseamp offset Detection
     */
    UINT32 rsvd_9 : 23;
  } Bits;
  UINT32 Data;
} DATATRAINFEEDBACKN1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRFSMCALN1_7_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B13C8)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRFSMCALN1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43C8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Fsm for  Voc and Xover caliberation, Voc caliberation is a procedure that cancel 
 * out intrinsic offset in analogue receivers and Xover training ,it aligns all the 
 * local dll to the global qclk 
 */
typedef union {
  struct {
    UINT32 saoffset0fsm : 5;
    /* saoffset0fsm - Bits[4:0], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of first receiver is done(read 
       only) 
     */
    UINT32 saoffset1fsm : 5;
    /* saoffset1fsm - Bits[9:5], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of second receiver is done(read 
       only) 
     */
    UINT32 saoffset2fsm : 5;
    /* saoffset2fsm - Bits[14:10], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of third receiver is done(read 
       only) 
     */
    UINT32 saoffset3fsm : 5;
    /* saoffset3fsm - Bits[19:15], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of fourth receiver is done(read 
       only) 
     */
    UINT32 fsmcalcomplete : 1;
    /* fsmcalcomplete - Bits[20:20], ROV, default = 1'b0 
        when the caliberation is done
     */
    UINT32 refpiclkdelayfsm : 6;
    /* refpiclkdelayfsm - Bits[26:21], ROV, default = 6'b0 
       Load FSM with a code at which , the transition is detected.
     */
    UINT32 fsmcaldone : 1;
    /* fsmcaldone - Bits[27:27], ROV, default = 1'b0 
       when the caliberation of voc or x-over is done
     */
    UINT32 fsmtimersel : 1;
    /* fsmtimersel - Bits[28:28], RW, default = 1'b0 
       Timer is set for either 8 cycles or 16 cycles
     */
    UINT32 fsmpatternsel : 1;
    /* fsmpatternsel - Bits[29:29], RW, default = 1'b0 
        It selects whther the pattern of three bit or two bit is selected when the 
       transition happen from 1-->0 for pdout and receiver , to avoid glitch 
     */
    UINT32 fsmvoccal : 1;
    /* fsmvoccal - Bits[30:30], RW, default = 1'b0 
        This bit tells , wether the xover or voc caliberation will be done, if its zero 
       x-over caliberation is done and vice-versa 
     */
    UINT32 fsmcalen : 1;
    /* fsmcalen - Bits[31:31], RW, default = 1'b0 
       FsmCalEn starts the voc-Xover Fsm
     */
  } Bits;
  UINT32 Data;
} DDRCRFSMCALN1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL4N1_7_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B13CC)                                                     */
/* Register default value:              0x801F0000                            */
#define DATACONTROL4N1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43CC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    UINT32 vsshiorvrefctl1 : 8;
    /* vsshiorvrefctl1 - Bits[8:1], RW, default = 8'b0 
        VssHi control bits[br]
               Field Description: [br]
       [br]
                Field             Bits    Description[br]
               -------            ----    ---------------------------------[br]
       	 disoverflow        24     Disable overflow logic from BDW[br]
         fast panic enable  25     Bypass 2 flops in panic path for faster panic 
       response[br] 
         vsshivccd2by3cfgen 26     Forces the driver DAC to get supply from 2/3 
       vccd[br] 
         vsshivccd2by3cfg   27:29  Current trim setting for 2/3 vccd mode, 001 fast, 
       011 typical, 111 slow silicon [br] 
       	 ddrvssh2ndcompen   30     Enable second comparator (ZBB mode)[br]
         comp high bwen     31     Improve comparator performance to support low 
       digital voltage range 
     */
    UINT32 dqtxeqsel : 1;
    /* dqtxeqsel - Bits[9:9], RW, default = 1'b0 
       DQ TxEq setting selection to achieve constant EQ
     */
    UINT32 rsvd_10 : 2;
    UINT32 vrefpwrsave : 1;
    /* vrefpwrsave - Bits[12:12], RW, default = 1'h0 
                If set to 0 (default), Vref generators selected through RxVrefSel[3:0] 
       are always on.[br] 
                If set to 1, Vref generators are switched off while in Self Refresh.
     */
    UINT32 vreftonblbus : 4;
    /* vreftonblbus - Bits[16:13], RW, default = 4'b1000 
       Per-bit Enable connection to share Vref Bus
     */
    UINT32 rxvrefsel : 4;
    /* rxvrefsel - Bits[20:17], RW, default = 4'b1111 
       Per Bit Verf generator Enable
     */
    UINT32 refpiclkdelay : 6;
    /* refpiclkdelay - Bits[26:21], RW, default = 6'b000000 
       RefPiClkDelay
     */
    UINT32 fnvcrsdlbypassen : 1;
    /* fnvcrsdlbypassen - Bits[27:27], RW, default = 1'b0 
       FNV Slave Delay Line Bypass Enable
     */
    UINT32 fnvcrdllbypassen : 1;
    /* fnvcrdllbypassen - Bits[28:28], RW, default = 1'b0 
       FNV DLL Bypass Enable
     */
    UINT32 fnvcrvisaen : 1;
    /* fnvcrvisaen - Bits[29:29], RW, default = 1'b0 
       FNV Visa Fub Enable - data fub used for VISA if asserted
     */
    UINT32 writecrcenable : 1;
    /* writecrcenable - Bits[30:30], RW, default = 1'h0 
       Enable write crc mode
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DATACONTROL4N1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* ATTACK1SELN1_7_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B13D0)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK1SELN1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43D0
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr1d0 : 3;
    /* attackr1d0 - Bits[2:0], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[0] in a byte
     */
    UINT32 attackr1d1 : 3;
    /* attackr1d1 - Bits[5:3], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[1] in a byte
     */
    UINT32 attackr1d2 : 3;
    /* attackr1d2 - Bits[8:6], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[2] in a byte
     */
    UINT32 attackr1d3 : 3;
    /* attackr1d3 - Bits[11:9], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[3] in a byte
     */
    UINT32 attackr1d4 : 3;
    /* attackr1d4 - Bits[14:12], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[4] in a byte
     */
    UINT32 attackr1d5 : 3;
    /* attackr1d5 - Bits[17:15], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[5] in a byte
     */
    UINT32 attackr1d6 : 3;
    /* attackr1d6 - Bits[20:18], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[6] in a byte
     */
    UINT32 attackr1d7 : 3;
    /* attackr1d7 - Bits[23:21], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK1SELN1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* COMPDATA0N1_7_FNV_DDRIO_DAT7_CH_REG supported on:                          */
/*       EKV (0x401B13D4)                                                     */
/* Register default value:              0x00028000                            */
#define COMPDATA0N1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43D4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[5:0], RWV, default = 6'b0 
       Data Rcomp DrvUp Comp Value can be overriden via message channel if periodical 
       retrain is disable 
     */
    UINT32 spare0 : 3;
    /* spare0 - Bits[8:6], RWV, default = 3'b0 
       Spare
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[14:9], RWV, default = 6'b0 
       Data Rcomp DrvDown Comp Value
     */
    UINT32 vtcomp : 5;
    /* vtcomp - Bits[19:15], RWV, default = 5'b00101 
       VT Comp Values
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[26:20], RWV, default = 7'b0 
       Data TCO Comp Value.
     */
    UINT32 slewratecomp : 5;
    /* slewratecomp - Bits[31:27], RWV, default = 5'b0 
       Data Slew Rate Comp Value.
     */
  } Bits;
  UINT32 Data;
} COMPDATA0N1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* COMPDATA1N1_7_FNV_DDRIO_DAT7_CH_REG supported on:                          */
/*       EKV (0x401B13D8)                                                     */
/* Register default value:              0x04010000                            */
#define COMPDATA1N1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43D8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rcompodtup : 6;
    /* rcompodtup - Bits[5:0], RWV, default = 6'b0 
       Data Rcomp OdtUp Comp Value
     */
    UINT32 rcompodtdown : 6;
    /* rcompodtdown - Bits[11:6], RWV, default = 6'b0 
       Data Rcomp Odt-Down Comp Value
     */
    UINT32 panicdrvdn : 10;
    /* panicdrvdn - Bits[21:12], RWV, default = 10'b0000010000 
       Panic DrvDn RComp Value.
     */
    UINT32 panicdrvup : 10;
    /* panicdrvup - Bits[31:22], RWV, default = 10'b0000010000 
       Panic DrvUp RComp Comp Value.
     */
  } Bits;
  UINT32 Data;
} COMPDATA1N1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATAOFFSETTRAINN1_7_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B13DC)                                                     */
/* Register default value:              0x00000000                            */
#define DATAOFFSETTRAINN1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43DC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rcvenoffset : 6;
    /* rcvenoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained RxRcvEn Value.
     */
    UINT32 rxdqsoffset : 7;
    /* rxdqsoffset - Bits[12:6], RW, default = 7'b0 
       Offset added to Trained Rx DQS Value.
     */
    UINT32 txdqoffset : 6;
    /* txdqoffset - Bits[18:13], RW, default = 6'b00 
       Offset added to Trained Tx DQ Value.
     */
    UINT32 txdqsoffset : 6;
    /* txdqsoffset - Bits[24:19], RW, default = 6'b000000 
       Ooffset added to Trained Tx DQS Value.
     */
    UINT32 vrefoffset : 7;
    /* vrefoffset - Bits[31:25], RW, default = 7'b0000000 
       2s Compliment offset added to Trained Vref Value.  Positive number increases 
       Vref, and each step is Vdd/384. 
     */
  } Bits;
  UINT32 Data;
} DATAOFFSETTRAINN1_7_FNV_DDRIO_DAT7_CH_STRUCT;




/* ATTACK0SELN1_7_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B13E4)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK0SELN1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43E4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 txddjcen : 4;
    /* txddjcen - Bits[15:12], RW, default = 4'b000 
       TxddjcEn
     */
    UINT32 dfeenable : 1;
    /* dfeenable - Bits[16:16], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfedlysel : 1;
    /* dfedlysel - Bits[17:17], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfecode : 4;
    /* dfecode - Bits[21:18], RW, default = 4'b0000 
       xtalk aggressor0 select for dq bit[7] in a byte
     */
    UINT32 rsvd_22 : 10;
  } Bits;
  UINT32 Data;
} ATTACK0SELN1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL0N1_7_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B13E8)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL0N1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43E8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxtrainingmode : 1;
    /* rxtrainingmode - Bits[0:0], RW, default = 1'b0 
       Enables Read DqDqs Training Mode
     */
    UINT32 wltrainingmode : 1;
    /* wltrainingmode - Bits[1:1], RW, default = 1'b0 
       Enables Write Leveling Training Mode
     */
    UINT32 rltrainingmode : 1;
    /* rltrainingmode - Bits[2:2], RW, default = 1'b0 
       Enables Read Leveling Training Mode.
     */
    UINT32 senseamptrainingmode : 1;
    /* senseamptrainingmode - Bits[3:3], RW, default = 1'b0 
       Enables SenseAmp offset cancellation training mode.
     */
    UINT32 txon : 1;
    /* txon - Bits[4:4], RW, default = 1'b0 
       When set, forces DQ/DQS drive enable to active.  Used in power measurements and 
       IO loopback mode 
     */
    UINT32 rfon : 1;
    /* rfon - Bits[5:5], RW, default = 1'b0 
       Power-down Disable:  Forces register file read, overriding the rank power down 
       logic. 
     */
    UINT32 rxpion : 1;
    /* rxpion - Bits[6:6], RW, default = 1'b0 
       Power-down Disable:  Forces RxDqs PI clocks on.
     */
    UINT32 txpion : 1;
    /* txpion - Bits[7:7], RW, default = 1'b0 
       Power-down Disable:  Forces TxDq and TxDqs PI clocks on.
     */
    UINT32 internalclockson : 1;
    /* internalclockson - Bits[8:8], RW, default = 1'b0 
                 Forces on master DLL and all PI enables ON, despite both channel in 
       SelfRefresh, etc. low power states. 
     */
    UINT32 saoffsettraintxon : 1;
    /* saoffsettraintxon - Bits[9:9], RW, default = 1'b0 
       SaOffsetTrainTxon
     */
    UINT32 txdisable : 1;
    /* txdisable - Bits[10:10], RW, default = 1'b0 
       Power-down:  All write commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 rxdisable : 1;
    /* rxdisable - Bits[11:11], RW, default = 1'b0 
       Power-down:  All read commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 txlong : 1;
    /* txlong - Bits[12:12], RW, default = 1'b0 
       Sets the duration of the first transmitted DQ bit of the burst. 0=1UI, 1=2UI.
     */
    UINT32 rsvd_13 : 5;
    UINT32 driversegmentenable : 1;
    /* driversegmentenable - Bits[18:18], RW, default = 1'b0 
       Controls which segements of the driver are enabled:  {0: All, 1: Only ODT}.
     */
    UINT32 rsvd_19 : 1;
    UINT32 readrfrd : 1;
    /* readrfrd - Bits[20:20], RWV, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a read command to Rank = ReadRFRank 
     */
    UINT32 readrfwr : 1;
    /* readrfwr - Bits[21:21], RWV, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a write command to Rank = ReadRFRank 
     */
    UINT32 readrfrank : 3;
    /* readrfrank - Bits[24:22], RWV, default = 3'b000 
       Specifies the rank that ReadRFRd or ReadRFWr will manually download the CR 
       values from 
     */
    UINT32 forceodton : 1;
    /* forceodton - Bits[25:25], RW, default = 1'b0 
       ODT is forced-on.
     */
    UINT32 odtsampoff : 1;
    /* odtsampoff - Bits[26:26], RW, default = 1'b0 
       ODT and Senseamp are forced-off.
     */
    UINT32 disableodtstatic : 1;
    /* disableodtstatic - Bits[27:27], RW, default = 1'b0 
       not used
     */
    UINT32 ddrcrforceodton : 1;
    /* ddrcrforceodton - Bits[28:28], RW, default = 1'b0 
       force ODT to the transmitter on, regardless of state of DriveEnable
     */
    UINT32 spare : 1;
    /* spare - Bits[29:29], RW, default = 1'b0 
       spare
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[30:30], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
    UINT32 longpreambleenable : 1;
    /* longpreambleenable - Bits[31:31], RW, default = 1'b0 
       Enables long preamble for DDR4 devices
     */
  } Bits;
  UINT32 Data;
} DATACONTROL0N1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL1N1_7_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B13EC)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL1N1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43EC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 xtalkpienable : 1;
    /* xtalkpienable - Bits[0:0], RW, default = 1'b0 
       enable phase shift based crosstalk cancellation.
     */
    UINT32 xtalkpisign : 1;
    /* xtalkpisign - Bits[1:1], RW, default = 1'b0 
       select if 0: capacitive coupling; or 1: inductive coupling; xtalk dominant
     */
    UINT32 xtalkpidelta : 3;
    /* xtalkpidelta - Bits[4:2], RW, default = 3'b0 
       delta phase shift in one aggressor is in effect
     */
    UINT32 xtalkpideltarank5 : 2;
    /* xtalkpideltarank5 - Bits[6:5], RW, default = 2'b0 
       XtalkPiDeltaRank5
     */
    UINT32 rsvd_7 : 3;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 3'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll[br] 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 3'b000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 3'b000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL2N1_7_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B13F0)                                                     */
/* Register default value:              0x07800000                            */
#define DATACONTROL2N1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43F0
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier).[br] 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency 
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = 1'h0 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = 1'h0 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = 1'h0 
       Enable Imode EQ per byte lane. [br]
                 0: Imode completely off [br]
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 enearlywriterankqnnnh : 1;
    /* enearlywriterankqnnnh - Bits[11:11], RW, default = 1'h0 
       Spare
     */
    UINT32 picodepipeqnnnh : 1;
    /* picodepipeqnnnh - Bits[12:12], RW, default = 1'h0 
       Spare
     */
    UINT32 spare : 2;
    /* spare - Bits[14:13], RW, default = 2'h0 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = 1'h0 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = 4'h0 
       Binary Imode Coefficient, sets Imode current strength.[br]
                 (used for either Xtalk cancellation or swing boost)[br]
                 Code   Imode Current (approx)[br]
                 ------------------[br]
                 0000        0[br]
                 0001        1[br]
                 0010        2[br]
                   *[br]
                   *[br]
                   *[br]
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = 1'h0 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL3N1_7_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B13F4)                                                     */
/* Register default value:              0x2198DFCF                            */
#define DATACONTROL3N1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43F4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 drvstaticlegcfg : 2;
    /* drvstaticlegcfg - Bits[1:0], RW, default = 2'b11 
       binary default setting for static legs in DRV mode[br]
                  0    0       0 legs (EQ disabled)[br]
                  0    1       3 legs (EQ max 3 legs)[br]
                  1    0       6 legs (EQ max 6 legs)[br]
                  1    1      12 legs (EQ max 12 legs)[br]
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher [br] 
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit [br] 
                 the higher Ron target value.
     */
    UINT32 odtstaticlegcfg : 2;
    /* odtstaticlegcfg - Bits[3:2], RW, default = 2'b11 
       binary default setting for static legs in ODT mode[br]
                  0    0       0 legs (EQ disabled)[br]
                  0    1       3 legs (EQ max 3 legs)[br]
                  1    0       6 legs (EQ max 6 legs)[br]
                  1    1      12 legs (EQ max 12 legs)[br]
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher [br] 
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit [br] 
                 the higher Ron target value.
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[4:4], RW, default = 1'b0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rsvd_5 : 1;
    UINT32 odtsegmentenable : 3;
    /* odtsegmentenable - Bits[8:6], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.  [br]
                            After that, a shift register will determine the enabled 
       segments 
     */
    UINT32 datasegmentenable : 3;
    /* datasegmentenable - Bits[11:9], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.
     */
    UINT32 longodtr2w : 1;
    /* longodtr2w - Bits[12:12], RW, default = 1'b1 
       When set, it will extends ODT by 1 qclk on read to write turnarounds. hsd 230773
     */
    UINT32 spare2 : 1;
    /* spare2 - Bits[13:13], RW, default = 1'b0 
       Spare2
     */
    UINT32 imodebiasvrefen : 1;
    /* imodebiasvrefen - Bits[14:14], RW, default = 1'b1 
       imodebias control
     */
    UINT32 imodebiasrxbiastrim : 3;
    /* imodebiasrxbiastrim - Bits[17:15], RW, default = 3'b001 
       imodebias control. Also used for rxbiasana
     */
    UINT32 spare : 1;
    /* spare - Bits[18:18], RW, default = 1'b0 
       Spare
     */
    UINT32 imodebiasdfxddr4legup : 1;
    /* imodebiasdfxddr4legup - Bits[19:19], RW, default = 1'b1 
       imodebias control
     */
    UINT32 imodebiasdfxlegdn : 1;
    /* imodebiasdfxlegdn - Bits[20:20], RW, default = 1'b1 
       imodebias control
     */
    UINT32 ddrcrctlecapen : 2;
    /* ddrcrctlecapen - Bits[22:21], RW, default = 2'b00 
       ctle capacitor setting
     */
    UINT32 ddrcrctleresen : 2;
    /* ddrcrctleresen - Bits[24:23], RW, default = 2'b11 
       ctle resistor setting
     */
    UINT32 pcasbiasclosedloopen : 1;
    /* pcasbiasclosedloopen - Bits[25:25], RW, default = 1'b0 
       RxBias CR
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[26:26], RW, default = 1'b0 
       RxBias CR
     */
    UINT32 rxbiasfoldedlegtrim : 2;
    /* rxbiasfoldedlegtrim - Bits[28:27], RW, default = 2'b00 
       RxBias CR
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[30:29], RW, default = 2'b01 
       RxBias CR
     */
    UINT32 postambleenable : 1;
    /* postambleenable - Bits[31:31], RW, default = 1'h0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DATACONTROL3N1_7_FNV_DDRIO_DAT7_CH_STRUCT;


/* VSSHIORVREFCONTROLN1_7_FNV_DDRIO_DAT7_CH_REG supported on:                 */
/*       EKV (0x401B13F8)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROLN1_7_FNV_DDRIO_DAT7_CH_REG 0x050F43F8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings.[br] 
       [br]
               VssHi Field Description: [br]
       [br]
                Field          Bits    Description[br]
               -------         ----    
       ---------------------------------------------------------------------------------------------------------------------------------------------------------------[br] 
               SelCode          23     Selects the code to output:  {0: Code, 1: 
       TCode}[br] 
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode   [br] 
               PanicVoltage  21:18  Unsigned integer controlling the voltage error on 
       VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV)[br] 
               CloseLoop        17     Enables the VssHi close loop tracking [br]
               PanicEn          16     Enables the VssHi Panic Driver[br]
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError)[br] 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved[br] 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128}[br] 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROLN1_7_FNV_DDRIO_DAT7_CH_STRUCT;




/* RXGROUP0N0RANK0_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1400)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK0_8_FNV_DDRIO_DAT7_CH_REG 0x050F4400
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 or CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS (0: 1/16 UI 
       offset, [br] 
                 1: 3/16 UI Offset, 2: 5/16 UI Offsets, 3: 7/16 UI Offsets, 4: 9/16 UI 
       Offsets, 5,6,7: 11/16 UI Offsets.) 
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK1_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1404)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK1_8_FNV_DDRIO_DAT7_CH_REG 0x050F4404
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank1. Multicast read for CH012 or CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK2_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1408)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK2_8_FNV_DDRIO_DAT7_CH_REG 0x050F4408
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank2. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK2_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK3_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B140C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK3_8_FNV_DDRIO_DAT7_CH_REG 0x050F440C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank3. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_N to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK3_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK4_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1410)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK4_8_FNV_DDRIO_DAT7_CH_REG 0x050F4410
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank4. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK4_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK5_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1414)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK5_8_FNV_DDRIO_DAT7_CH_REG 0x050F4414
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank5. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       readx4modeenb
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       mux select for crossover from ClkPiRef to CkPi4RcvEn, used in xover2to2 mode
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK5_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK6_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1418)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK6_8_FNV_DDRIO_DAT7_CH_REG 0x050F4418
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank6. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       per nibble READ timing control, delay DQS_P to match the earlest arrival DQ, 
       0~1.5UI with step size ClkPi/64 
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK6_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N0RANK7_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B141C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK7_8_FNV_DDRIO_DAT7_CH_REG 0x050F441C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank7. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       CTLE current steering tuning (0=no eq, 7=max eq). It is decoded to[br]
                   CTLE resistor tuning: 00 - 3R (10.8Kohms), 01/10 -  R (3.6Kohms), 11 
       - R/3 (1.2Kohms)[br] 
                   CTLE capacitor tuning: 00 - 0C (not used), 01 - 1C (200ff), 10 - 2C 
       (400ff), 11 - 3C (600ff) 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N0RANK7_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK0_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1420)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK0_8_FNV_DDRIO_DAT7_CH_REG 0x050F4420
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_P to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       per bit READ timing control,[br]
                   delay DQS_N to match each DQ, 0~.25UI with step size ClkPi/64 added 
       to nibble delay RxDqsPDelay 
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK1_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1424)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK1_8_FNV_DDRIO_DAT7_CH_REG 0x050F4424
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK2_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1428)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK2_8_FNV_DDRIO_DAT7_CH_REG 0x050F4428
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK2_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK3_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B142C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK3_8_FNV_DDRIO_DAT7_CH_REG 0x050F442C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK3_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK4_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1430)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK4_8_FNV_DDRIO_DAT7_CH_REG 0x050F4430
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK4_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK5_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1434)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK5_8_FNV_DDRIO_DAT7_CH_REG 0x050F4434
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK5_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK6_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1438)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK6_8_FNV_DDRIO_DAT7_CH_REG 0x050F4438
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK6_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N0RANK7_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B143C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK7_8_FNV_DDRIO_DAT7_CH_REG 0x050F443C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N0RANK7_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK0_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1440)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK0_8_FNV_DDRIO_DAT7_CH_REG 0x050F4440
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       select one of the four DQ Clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK1_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1444)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK1_8_FNV_DDRIO_DAT7_CH_REG 0x050F4444
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK2_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1448)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK2_8_FNV_DDRIO_DAT7_CH_REG 0x050F4448
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK2_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK3_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B144C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK3_8_FNV_DDRIO_DAT7_CH_REG 0x050F444C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK3_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK4_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1450)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK4_8_FNV_DDRIO_DAT7_CH_REG 0x050F4450
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK4_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK5_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1454)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK5_8_FNV_DDRIO_DAT7_CH_REG 0x050F4454
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 3;
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK5_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK6_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1458)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK6_8_FNV_DDRIO_DAT7_CH_REG 0x050F4458
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK6_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N0RANK7_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B145C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK7_8_FNV_DDRIO_DAT7_CH_REG 0x050F445C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       Select one of the four DQ clk
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N0RANK7_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK0_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1460)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK0_8_FNV_DDRIO_DAT7_CH_REG 0x050F4460
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte. 0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK1_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1464)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK1_8_FNV_DDRIO_DAT7_CH_REG 0x050F4464
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK2_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1468)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK2_8_FNV_DDRIO_DAT7_CH_REG 0x050F4468
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK2_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK3_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B146C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK3_8_FNV_DDRIO_DAT7_CH_REG 0x050F446C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK3_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK4_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1470)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK4_8_FNV_DDRIO_DAT7_CH_REG 0x050F4470
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK4_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK5_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1474)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK5_8_FNV_DDRIO_DAT7_CH_REG 0x050F4474
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK5_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK6_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1478)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK6_8_FNV_DDRIO_DAT7_CH_REG 0x050F4478
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK6_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N0RANK7_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B147C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK7_8_FNV_DDRIO_DAT7_CH_REG 0x050F447C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N0RANK7_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK0_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1480)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK0_8_FNV_DDRIO_DAT7_CH_REG 0x050F4480
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       selects which skewed DQSP (to sample even bits) is used in the RX FIFO. For 
       proper operation, always pick latest DQSP 
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       selects which skewed DQSN (to sample odd bits) is used in the RX FIFO. For 
       proper operation, always pick latest DQSN 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK1_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1484)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK1_8_FNV_DDRIO_DAT7_CH_REG 0x050F4484
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK2_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1488)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK2_8_FNV_DDRIO_DAT7_CH_REG 0x050F4488
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK2_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK3_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B148C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK3_8_FNV_DDRIO_DAT7_CH_REG 0x050F448C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK3_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK4_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1490)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK4_8_FNV_DDRIO_DAT7_CH_REG 0x050F4490
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       [0]=0 selects the raw output from the nibble DQS amplifier; [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK4_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK5_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1494)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK5_8_FNV_DDRIO_DAT7_CH_REG 0x050F4494
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK5_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK6_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1498)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK6_8_FNV_DDRIO_DAT7_CH_REG 0x050F4498
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK6_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN0RANK7_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B149C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK7_8_FNV_DDRIO_DAT7_CH_REG 0x050F449C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       select the phase1 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN0RANK7_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXXTALKN0_8_FNV_DDRIO_DAT7_CH_REG supported on:                            */
/*       EKV (0x401B14A0)                                                     */
/* Register default value:              0x00000000                            */
#define TXXTALKN0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44A0
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * TX Crosstalk Cancellation Per Lane Write CoEfficents. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 twototwologicdelayrank0 : 4;
    /* twototwologicdelayrank0 - Bits[3:0], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 0
     */
    UINT32 twototwologicdelayrank1 : 4;
    /* twototwologicdelayrank1 - Bits[7:4], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 1
     */
    UINT32 twototwologicdelayrank2 : 4;
    /* twototwologicdelayrank2 - Bits[11:8], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 2
     */
    UINT32 twototwologicdelayrank3 : 4;
    /* twototwologicdelayrank3 - Bits[15:12], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 3
     */
    UINT32 twototwologicdelayrank4 : 4;
    /* twototwologicdelayrank4 - Bits[19:16], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for Rank 4
     */
    UINT32 twototwologicdelayrank5 : 4;
    /* twototwologicdelayrank5 - Bits[23:20], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 5
     */
    UINT32 twototwologicdelayrank6 : 4;
    /* twototwologicdelayrank6 - Bits[27:24], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 6
     */
    UINT32 twototwologicdelayrank7 : 4;
    /* twototwologicdelayrank7 - Bits[31:28], RW, default = 4'b0000 
       This may need to be set for bits 3:0 in the nibble for rank 7
     */
  } Bits;
  UINT32 Data;
} TXXTALKN0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXVREFCTRLN0_8_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B14A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44A4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * RxVrefCtrl. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 vrefperbit0 : 7;
    /* vrefperbit0 - Bits[6:0], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben0 : 1;
    /* halflsben0 - Bits[7:7], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit1 : 7;
    /* vrefperbit1 - Bits[14:8], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben1 : 1;
    /* halflsben1 - Bits[15:15], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefperbit2 : 7;
    /* vrefperbit2 - Bits[22:16], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben2 : 1;
    /* halflsben2 - Bits[23:23], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefperbit3 : 7;
    /* vrefperbit3 - Bits[30:24], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben3 : 1;
    /* halflsben3 - Bits[31:31], RW, default = 1'b0 
       shift down the Vref voltage by ~0.5 stepsize
     */
  } Bits;
  UINT32 Data;
} RXVREFCTRLN0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATATRAINFEEDBACKN0_8_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B14A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44A8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Training Feedback Results. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 datatrainfeedback : 9;
    /* datatrainfeedback - Bits[8:0], RW, default = 9'b0 
       Data Training Feedback Storage[br]
       [br]
               DataTrainFeedback Description:[br]
                Training Step    Bits    Nibble    Description[br]
               --------------    ----    -------  --------------------------[br]
               Cross Over Cal     0      0        Tx Dqs Phase Detection[br]
               Cross Over Cal     1      0        Tx Dq Phase Detection[br]
               Cross Over Cal     2      0        Rec En Phase Detection[br]
               RcvEn              8:0    0        Byte detection[br]
       [br]
               Wr Leveling        8:0    0        Byte detection[br]
               Senseamp offset    3:0    0        Dq Senseamp offset Detection
     */
    UINT32 rsvd_9 : 23;
  } Bits;
  UINT32 Data;
} DATATRAINFEEDBACKN0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRFSMCALN0_8_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B14C8)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRFSMCALN0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44C8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Fsm for  Voc and Xover caliberation, Voc caliberation is a procedure that cancel 
 * out intrinsic offset in analogue receivers and Xover training ,it aligns all the 
 * local dll to the global qclk 
 */
typedef union {
  struct {
    UINT32 saoffset0fsm : 5;
    /* saoffset0fsm - Bits[4:0], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of first receiver is done(read 
       only) 
     */
    UINT32 saoffset1fsm : 5;
    /* saoffset1fsm - Bits[9:5], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of second receiver is done(read 
       only) 
     */
    UINT32 saoffset2fsm : 5;
    /* saoffset2fsm - Bits[14:10], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of third receiver is done(read 
       only) 
     */
    UINT32 saoffset3fsm : 5;
    /* saoffset3fsm - Bits[19:15], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of fourth receiver is done(read 
       only) 
     */
    UINT32 fsmcalcomplete : 1;
    /* fsmcalcomplete - Bits[20:20], ROV, default = 1'b0 
        when the caliberation is done
     */
    UINT32 refpiclkdelayfsm : 6;
    /* refpiclkdelayfsm - Bits[26:21], ROV, default = 6'b0 
       Load FSM with a code at which , the transition is detected.
     */
    UINT32 fsmcaldone : 1;
    /* fsmcaldone - Bits[27:27], ROV, default = 1'b0 
       when the caliberation of voc or x-over is done
     */
    UINT32 fsmtimersel : 1;
    /* fsmtimersel - Bits[28:28], RW, default = 1'b0 
       Timer is set for either 8 cycles or 16 cycles
     */
    UINT32 fsmpatternsel : 1;
    /* fsmpatternsel - Bits[29:29], RW, default = 1'b0 
        It selects whther the pattern of three bit or two bit is selected when the 
       transition happen from 1-->0 for pdout and receiver , to avoid glitch 
     */
    UINT32 fsmvoccal : 1;
    /* fsmvoccal - Bits[30:30], RW, default = 1'b0 
        This bit tells , wether the xover or voc caliberation will be done, if its zero 
       x-over caliberation is done and vice-versa 
     */
    UINT32 fsmcalen : 1;
    /* fsmcalen - Bits[31:31], RW, default = 1'b0 
       FsmCalEn starts the voc-Xover Fsm
     */
  } Bits;
  UINT32 Data;
} DDRCRFSMCALN0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL4N0_8_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B14CC)                                                     */
/* Register default value:              0x801F0000                            */
#define DATACONTROL4N0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44CC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    UINT32 vsshiorvrefctl1 : 8;
    /* vsshiorvrefctl1 - Bits[8:1], RW, default = 8'b0 
        VssHi control bits[br]
               Field Description: [br]
       [br]
                Field             Bits    Description[br]
               -------            ----    ---------------------------------[br]
       	 disoverflow        24     Disable overflow logic from BDW[br]
         fast panic enable  25     Bypass 2 flops in panic path for faster panic 
       response[br] 
         vsshivccd2by3cfgen 26     Forces the driver DAC to get supply from 2/3 
       vccd[br] 
         vsshivccd2by3cfg   27:29  Current trim setting for 2/3 vccd mode, 001 fast, 
       011 typical, 111 slow silicon [br] 
       	 ddrvssh2ndcompen   30     Enable second comparator (ZBB mode)[br]
         comp high bwen     31     Improve comparator performance to support low 
       digital voltage range 
     */
    UINT32 dqtxeqsel : 1;
    /* dqtxeqsel - Bits[9:9], RW, default = 1'b0 
       DQ TxEq setting selection to achieve constant EQ
     */
    UINT32 rsvd_10 : 2;
    UINT32 vrefpwrsave : 1;
    /* vrefpwrsave - Bits[12:12], RW, default = 1'h0 
                If set to 0 (default), Vref generators selected through RxVrefSel[3:0] 
       are always on.[br] 
                If set to 1, Vref generators are switched off while in Self Refresh.
     */
    UINT32 vreftonblbus : 4;
    /* vreftonblbus - Bits[16:13], RW, default = 4'b1000 
       Per-bit Enable connection to share Vref Bus
     */
    UINT32 rxvrefsel : 4;
    /* rxvrefsel - Bits[20:17], RW, default = 4'b1111 
       Per Bit Verf generator Enable
     */
    UINT32 refpiclkdelay : 6;
    /* refpiclkdelay - Bits[26:21], RW, default = 6'b000000 
       RefPiClkDelay
     */
    UINT32 fnvcrsdlbypassen : 1;
    /* fnvcrsdlbypassen - Bits[27:27], RW, default = 1'b0 
       FNV Slave Delay Line Bypass Enable
     */
    UINT32 fnvcrdllbypassen : 1;
    /* fnvcrdllbypassen - Bits[28:28], RW, default = 1'b0 
       FNV DLL Bypass Enable
     */
    UINT32 fnvcrvisaen : 1;
    /* fnvcrvisaen - Bits[29:29], RW, default = 1'b0 
       FNV Visa Fub Enable - data fub used for VISA if asserted
     */
    UINT32 writecrcenable : 1;
    /* writecrcenable - Bits[30:30], RW, default = 1'h0 
       Enable write crc mode
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DATACONTROL4N0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* ATTACK1SELN0_8_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B14D0)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK1SELN0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44D0
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr1d0 : 3;
    /* attackr1d0 - Bits[2:0], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[0] in a byte
     */
    UINT32 attackr1d1 : 3;
    /* attackr1d1 - Bits[5:3], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[1] in a byte
     */
    UINT32 attackr1d2 : 3;
    /* attackr1d2 - Bits[8:6], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[2] in a byte
     */
    UINT32 attackr1d3 : 3;
    /* attackr1d3 - Bits[11:9], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[3] in a byte
     */
    UINT32 attackr1d4 : 3;
    /* attackr1d4 - Bits[14:12], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[4] in a byte
     */
    UINT32 attackr1d5 : 3;
    /* attackr1d5 - Bits[17:15], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[5] in a byte
     */
    UINT32 attackr1d6 : 3;
    /* attackr1d6 - Bits[20:18], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[6] in a byte
     */
    UINT32 attackr1d7 : 3;
    /* attackr1d7 - Bits[23:21], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK1SELN0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* COMPDATA0N0_8_FNV_DDRIO_DAT7_CH_REG supported on:                          */
/*       EKV (0x401B14D4)                                                     */
/* Register default value:              0x00028000                            */
#define COMPDATA0N0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44D4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[5:0], RWV, default = 6'b0 
       Data Rcomp DrvUp Comp Value can be overriden via message channel if periodical 
       retrain is disable 
     */
    UINT32 spare0 : 3;
    /* spare0 - Bits[8:6], RWV, default = 3'b0 
       Spare
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[14:9], RWV, default = 6'b0 
       Data Rcomp DrvDown Comp Value
     */
    UINT32 vtcomp : 5;
    /* vtcomp - Bits[19:15], RWV, default = 5'b00101 
       VT Comp Values
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[26:20], RWV, default = 7'b0 
       Data TCO Comp Value.
     */
    UINT32 slewratecomp : 5;
    /* slewratecomp - Bits[31:27], RWV, default = 5'b0 
       Data Slew Rate Comp Value.
     */
  } Bits;
  UINT32 Data;
} COMPDATA0N0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* COMPDATA1N0_8_FNV_DDRIO_DAT7_CH_REG supported on:                          */
/*       EKV (0x401B14D8)                                                     */
/* Register default value:              0x04010000                            */
#define COMPDATA1N0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44D8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rcompodtup : 6;
    /* rcompodtup - Bits[5:0], RWV, default = 6'b0 
       Data Rcomp OdtUp Comp Value
     */
    UINT32 rcompodtdown : 6;
    /* rcompodtdown - Bits[11:6], RWV, default = 6'b0 
       Data Rcomp Odt-Down Comp Value
     */
    UINT32 panicdrvdn : 10;
    /* panicdrvdn - Bits[21:12], RWV, default = 10'b0000010000 
       Panic DrvDn RComp Value.
     */
    UINT32 panicdrvup : 10;
    /* panicdrvup - Bits[31:22], RWV, default = 10'b0000010000 
       Panic DrvUp RComp Comp Value.
     */
  } Bits;
  UINT32 Data;
} COMPDATA1N0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATAOFFSETTRAINN0_8_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B14DC)                                                     */
/* Register default value:              0x00000000                            */
#define DATAOFFSETTRAINN0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44DC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rcvenoffset : 6;
    /* rcvenoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained RxRcvEn Value.
     */
    UINT32 rxdqsoffset : 7;
    /* rxdqsoffset - Bits[12:6], RW, default = 7'b0 
       Offset added to Trained Rx DQS Value.
     */
    UINT32 txdqoffset : 6;
    /* txdqoffset - Bits[18:13], RW, default = 6'b00 
       Offset added to Trained Tx DQ Value.
     */
    UINT32 txdqsoffset : 6;
    /* txdqsoffset - Bits[24:19], RW, default = 6'b000000 
       Ooffset added to Trained Tx DQS Value.
     */
    UINT32 vrefoffset : 7;
    /* vrefoffset - Bits[31:25], RW, default = 7'b0000000 
       2s Compliment offset added to Trained Vref Value.  Positive number increases 
       Vref, and each step is Vdd/384. 
     */
  } Bits;
  UINT32 Data;
} DATAOFFSETTRAINN0_8_FNV_DDRIO_DAT7_CH_STRUCT;




/* ATTACK0SELN0_8_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B14E4)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK0SELN0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44E4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 txddjcen : 4;
    /* txddjcen - Bits[15:12], RW, default = 4'b000 
       TxddjcEn
     */
    UINT32 dfeenable : 1;
    /* dfeenable - Bits[16:16], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfedlysel : 1;
    /* dfedlysel - Bits[17:17], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfecode : 4;
    /* dfecode - Bits[21:18], RW, default = 4'b0000 
       xtalk aggressor0 select for dq bit[7] in a byte
     */
    UINT32 rsvd_22 : 10;
  } Bits;
  UINT32 Data;
} ATTACK0SELN0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL0N0_8_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B14E8)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL0N0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44E8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxtrainingmode : 1;
    /* rxtrainingmode - Bits[0:0], RW, default = 1'b0 
       Enables Read DqDqs Training Mode
     */
    UINT32 wltrainingmode : 1;
    /* wltrainingmode - Bits[1:1], RW, default = 1'b0 
       Enables Write Leveling Training Mode
     */
    UINT32 rltrainingmode : 1;
    /* rltrainingmode - Bits[2:2], RW, default = 1'b0 
       Enables Read Leveling Training Mode.
     */
    UINT32 senseamptrainingmode : 1;
    /* senseamptrainingmode - Bits[3:3], RW, default = 1'b0 
       Enables SenseAmp offset cancellation training mode.
     */
    UINT32 txon : 1;
    /* txon - Bits[4:4], RW, default = 1'b0 
       When set, forces DQ/DQS drive enable to active.  Used in power measurements and 
       IO loopback mode 
     */
    UINT32 rfon : 1;
    /* rfon - Bits[5:5], RW, default = 1'b0 
       Power-down Disable:  Forces register file read, overriding the rank power down 
       logic. 
     */
    UINT32 rxpion : 1;
    /* rxpion - Bits[6:6], RW, default = 1'b0 
       Power-down Disable:  Forces RxDqs PI clocks on.
     */
    UINT32 txpion : 1;
    /* txpion - Bits[7:7], RW, default = 1'b0 
       Power-down Disable:  Forces TxDq and TxDqs PI clocks on.
     */
    UINT32 internalclockson : 1;
    /* internalclockson - Bits[8:8], RW, default = 1'b0 
                 Forces on master DLL and all PI enables ON, despite both channel in 
       SelfRefresh, etc. low power states. 
     */
    UINT32 saoffsettraintxon : 1;
    /* saoffsettraintxon - Bits[9:9], RW, default = 1'b0 
       SaOffsetTrainTxon
     */
    UINT32 txdisable : 1;
    /* txdisable - Bits[10:10], RW, default = 1'b0 
       Power-down:  All write commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 rxdisable : 1;
    /* rxdisable - Bits[11:11], RW, default = 1'b0 
       Power-down:  All read commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 txlong : 1;
    /* txlong - Bits[12:12], RW, default = 1'b0 
       Sets the duration of the first transmitted DQ bit of the burst. 0=1UI, 1=2UI.
     */
    UINT32 rsvd_13 : 5;
    UINT32 driversegmentenable : 1;
    /* driversegmentenable - Bits[18:18], RW, default = 1'b0 
       Controls which segements of the driver are enabled:  {0: All, 1: Only ODT}.
     */
    UINT32 rsvd_19 : 1;
    UINT32 readrfrd : 1;
    /* readrfrd - Bits[20:20], RWV, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a read command to Rank = ReadRFRank 
     */
    UINT32 readrfwr : 1;
    /* readrfwr - Bits[21:21], RWV, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a write command to Rank = ReadRFRank 
     */
    UINT32 readrfrank : 3;
    /* readrfrank - Bits[24:22], RWV, default = 3'b000 
       Specifies the rank that ReadRFRd or ReadRFWr will manually download the CR 
       values from 
     */
    UINT32 forceodton : 1;
    /* forceodton - Bits[25:25], RW, default = 1'b0 
       ODT is forced-on.
     */
    UINT32 odtsampoff : 1;
    /* odtsampoff - Bits[26:26], RW, default = 1'b0 
       ODT and Senseamp are forced-off.
     */
    UINT32 disableodtstatic : 1;
    /* disableodtstatic - Bits[27:27], RW, default = 1'b0 
       not used
     */
    UINT32 ddrcrforceodton : 1;
    /* ddrcrforceodton - Bits[28:28], RW, default = 1'b0 
       force ODT to the transmitter on, regardless of state of DriveEnable
     */
    UINT32 spare : 1;
    /* spare - Bits[29:29], RW, default = 1'b0 
       spare
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[30:30], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
    UINT32 longpreambleenable : 1;
    /* longpreambleenable - Bits[31:31], RW, default = 1'b0 
       Enables long preamble for DDR4 devices
     */
  } Bits;
  UINT32 Data;
} DATACONTROL0N0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL1N0_8_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B14EC)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL1N0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44EC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 xtalkpienable : 1;
    /* xtalkpienable - Bits[0:0], RW, default = 1'b0 
       enable phase shift based crosstalk cancellation.
     */
    UINT32 xtalkpisign : 1;
    /* xtalkpisign - Bits[1:1], RW, default = 1'b0 
       select if 0: capacitive coupling; or 1: inductive coupling; xtalk dominant
     */
    UINT32 xtalkpidelta : 3;
    /* xtalkpidelta - Bits[4:2], RW, default = 3'b0 
       delta phase shift in one aggressor is in effect
     */
    UINT32 xtalkpideltarank5 : 2;
    /* xtalkpideltarank5 - Bits[6:5], RW, default = 2'b0 
       XtalkPiDeltaRank5
     */
    UINT32 rsvd_7 : 3;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 3'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll[br] 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 3'b000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 3'b000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL2N0_8_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B14F0)                                                     */
/* Register default value:              0x07800000                            */
#define DATACONTROL2N0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44F0
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier).[br] 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency 
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = 1'h0 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = 1'h0 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = 1'h0 
       Enable Imode EQ per byte lane. [br]
                 0: Imode completely off [br]
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 enearlywriterankqnnnh : 1;
    /* enearlywriterankqnnnh - Bits[11:11], RW, default = 1'h0 
       Spare
     */
    UINT32 picodepipeqnnnh : 1;
    /* picodepipeqnnnh - Bits[12:12], RW, default = 1'h0 
       Spare
     */
    UINT32 spare : 2;
    /* spare - Bits[14:13], RW, default = 2'h0 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = 1'h0 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = 4'h0 
       Binary Imode Coefficient, sets Imode current strength.[br]
                 (used for either Xtalk cancellation or swing boost)[br]
                 Code   Imode Current (approx)[br]
                 ------------------[br]
                 0000        0[br]
                 0001        1[br]
                 0010        2[br]
                   *[br]
                   *[br]
                   *[br]
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = 1'h0 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL3N0_8_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B14F4)                                                     */
/* Register default value:              0x2198DFCF                            */
#define DATACONTROL3N0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44F4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 drvstaticlegcfg : 2;
    /* drvstaticlegcfg - Bits[1:0], RW, default = 2'b11 
       binary default setting for static legs in DRV mode[br]
                  0    0       0 legs (EQ disabled)[br]
                  0    1       3 legs (EQ max 3 legs)[br]
                  1    0       6 legs (EQ max 6 legs)[br]
                  1    1      12 legs (EQ max 12 legs)[br]
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher [br] 
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit [br] 
                 the higher Ron target value.
     */
    UINT32 odtstaticlegcfg : 2;
    /* odtstaticlegcfg - Bits[3:2], RW, default = 2'b11 
       binary default setting for static legs in ODT mode[br]
                  0    0       0 legs (EQ disabled)[br]
                  0    1       3 legs (EQ max 3 legs)[br]
                  1    0       6 legs (EQ max 6 legs)[br]
                  1    1      12 legs (EQ max 12 legs)[br]
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher [br] 
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit [br] 
                 the higher Ron target value.
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[4:4], RW, default = 1'b0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rsvd_5 : 1;
    UINT32 odtsegmentenable : 3;
    /* odtsegmentenable - Bits[8:6], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.  [br]
                            After that, a shift register will determine the enabled 
       segments 
     */
    UINT32 datasegmentenable : 3;
    /* datasegmentenable - Bits[11:9], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.
     */
    UINT32 longodtr2w : 1;
    /* longodtr2w - Bits[12:12], RW, default = 1'b1 
       When set, it will extends ODT by 1 qclk on read to write turnarounds. hsd 230773
     */
    UINT32 spare2 : 1;
    /* spare2 - Bits[13:13], RW, default = 1'b0 
       Spare2
     */
    UINT32 imodebiasvrefen : 1;
    /* imodebiasvrefen - Bits[14:14], RW, default = 1'b1 
       imodebias control
     */
    UINT32 imodebiasrxbiastrim : 3;
    /* imodebiasrxbiastrim - Bits[17:15], RW, default = 3'b001 
       imodebias control. Also used for rxbiasana
     */
    UINT32 spare : 1;
    /* spare - Bits[18:18], RW, default = 1'b0 
       Spare
     */
    UINT32 imodebiasdfxddr4legup : 1;
    /* imodebiasdfxddr4legup - Bits[19:19], RW, default = 1'b1 
       imodebias control
     */
    UINT32 imodebiasdfxlegdn : 1;
    /* imodebiasdfxlegdn - Bits[20:20], RW, default = 1'b1 
       imodebias control
     */
    UINT32 ddrcrctlecapen : 2;
    /* ddrcrctlecapen - Bits[22:21], RW, default = 2'b00 
       ctle capacitor setting
     */
    UINT32 ddrcrctleresen : 2;
    /* ddrcrctleresen - Bits[24:23], RW, default = 2'b11 
       ctle resistor setting
     */
    UINT32 pcasbiasclosedloopen : 1;
    /* pcasbiasclosedloopen - Bits[25:25], RW, default = 1'b0 
       RxBias CR
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[26:26], RW, default = 1'b0 
       RxBias CR
     */
    UINT32 rxbiasfoldedlegtrim : 2;
    /* rxbiasfoldedlegtrim - Bits[28:27], RW, default = 2'b00 
       RxBias CR
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[30:29], RW, default = 2'b01 
       RxBias CR
     */
    UINT32 postambleenable : 1;
    /* postambleenable - Bits[31:31], RW, default = 1'h0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DATACONTROL3N0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* VSSHIORVREFCONTROLN0_8_FNV_DDRIO_DAT7_CH_REG supported on:                 */
/*       EKV (0x401B14F8)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROLN0_8_FNV_DDRIO_DAT7_CH_REG 0x050F44F8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings.[br] 
       [br]
               VssHi Field Description: [br]
       [br]
                Field          Bits    Description[br]
               -------         ----    
       ---------------------------------------------------------------------------------------------------------------------------------------------------------------[br] 
               SelCode          23     Selects the code to output:  {0: Code, 1: 
       TCode}[br] 
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode   [br] 
               PanicVoltage  21:18  Unsigned integer controlling the voltage error on 
       VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV)[br] 
               CloseLoop        17     Enables the VssHi close loop tracking [br]
               PanicEn          16     Enables the VssHi Panic Driver[br]
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError)[br] 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved[br] 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128}[br] 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROLN0_8_FNV_DDRIO_DAT7_CH_STRUCT;




/* RXGROUP0N1RANK0_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1500)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK0_8_FNV_DDRIO_DAT7_CH_REG 0x050F4500
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK1_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1504)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK1_8_FNV_DDRIO_DAT7_CH_REG 0x050F4504
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK2_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1508)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK2_8_FNV_DDRIO_DAT7_CH_REG 0x050F4508
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0.Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK2_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK3_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B150C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK3_8_FNV_DDRIO_DAT7_CH_REG 0x050F450C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK3_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK4_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1510)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK4_8_FNV_DDRIO_DAT7_CH_REG 0x050F4510
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK4_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK5_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1514)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK5_8_FNV_DDRIO_DAT7_CH_REG 0x050F4514
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK5_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK6_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1518)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK6_8_FNV_DDRIO_DAT7_CH_REG 0x050F4518
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK6_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP0N1RANK7_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B151C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK7_8_FNV_DDRIO_DAT7_CH_REG 0x050F451C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data RX training values for Rank0. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rcvendelay : 9;
    /* rcvendelay - Bits[8:0], RW, default = 9'b0 
       RcvEn timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 readx8modeen : 1;
    /* readx8modeen - Bits[9:9], RW, default = 1'b0 
       ReadX8ModeEn
     */
    UINT32 rxdqspdelay : 7;
    /* rxdqspdelay - Bits[16:10], RW, default = 7'b0 
       common DQS_P delay control, per nibble
     */
    UINT32 rxdqsndelay : 7;
    /* rxdqsndelay - Bits[23:17], RW, default = 7'b0 
       common DQS_N delay control, per nibble
     */
    UINT32 rxdqdelay : 3;
    /* rxdqdelay - Bits[26:24], RW, default = 3'b0 
       Rx DQ delay, per nibble, used in case if DQ is too early than DQS
     */
    UINT32 rxeq : 3;
    /* rxeq - Bits[29:27], RW, default = 3'b0 
       Rx Equalization code:  {4:3} controls the cap tuning 0: (not used), 1: 200ff, 2: 
       400ff, 3: 600ff, [br] 
                            controls the resistor tuning (0: 3R, 1: 1R, 2: 1R, 3: R/3).
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} RXGROUP0N1RANK7_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK0_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1520)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK0_8_FNV_DDRIO_DAT7_CH_REG 0x050F4520
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK1_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1524)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK1_8_FNV_DDRIO_DAT7_CH_REG 0x050F4524
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK2_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1528)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK2_8_FNV_DDRIO_DAT7_CH_REG 0x050F4528
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK2_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK3_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B152C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK3_8_FNV_DDRIO_DAT7_CH_REG 0x050F452C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK3_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK4_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1530)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK4_8_FNV_DDRIO_DAT7_CH_REG 0x050F4530
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK4_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK5_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1534)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK5_8_FNV_DDRIO_DAT7_CH_REG 0x050F4534
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK5_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK6_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1538)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK6_8_FNV_DDRIO_DAT7_CH_REG 0x050F4538
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK6_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXGROUP1N1RANK7_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B153C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK7_8_FNV_DDRIO_DAT7_CH_REG 0x050F453C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rxdqspdelay0 : 4;
    /* rxdqspdelay0 - Bits[3:0], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay1 : 4;
    /* rxdqspdelay1 - Bits[7:4], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay2 : 4;
    /* rxdqspdelay2 - Bits[11:8], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqspdelay3 : 4;
    /* rxdqspdelay3 - Bits[15:12], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay0 : 4;
    /* rxdqsndelay0 - Bits[19:16], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay1 : 4;
    /* rxdqsndelay1 - Bits[23:20], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay2 : 4;
    /* rxdqsndelay2 - Bits[27:24], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
    UINT32 rxdqsndelay3 : 4;
    /* rxdqsndelay3 - Bits[31:28], RW, default = 4'b0 
       Read timing offset DQS to match a DQ bit.  +tQCLK/4 with step size of tQCK/64
     */
  } Bits;
  UINT32 Data;
} RXGROUP1N1RANK7_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK0_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1540)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK0_8_FNV_DDRIO_DAT7_CH_REG 0x050F4540
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the four clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK1_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1544)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK1_8_FNV_DDRIO_DAT7_CH_REG 0x050F4544
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the four clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK2_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1548)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK2_8_FNV_DDRIO_DAT7_CH_REG 0x050F4548
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK2_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK3_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B154C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK3_8_FNV_DDRIO_DAT7_CH_REG 0x050F454C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK3_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK4_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1550)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK4_8_FNV_DDRIO_DAT7_CH_REG 0x050F4550
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       select one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK4_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK5_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1554)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK5_8_FNV_DDRIO_DAT7_CH_REG 0x050F4554
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled [br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK5_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK6_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1558)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK6_8_FNV_DDRIO_DAT7_CH_REG 0x050F4558
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       xelect for crossover from PiRef to PiN for bit 1
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK6_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP0N1RANK7_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B155C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK7_8_FNV_DDRIO_DAT7_CH_REG 0x050F455C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing and Voltage control for writing one data byte on Rank. Multicast read for 
 * CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqdelay : 9;
    /* txdqdelay - Bits[8:0], RW, default = 9'b0 
       Write DQ timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqsdelay : 9;
    /* txdqsdelay - Bits[17:9], RW, default = 9'b0 
       Write DQS timing control.  Unsigned number from 0 to 8 tQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpiclksel : 2;
    /* txdqpiclksel - Bits[19:18], RW, default = 2'b0 
       one of the 4 clocks
     */
    UINT32 rsvd_20 : 2;
    UINT32 txdqsxsel : 1;
    /* txdqsxsel - Bits[22:22], RW, default = 1'b0 
       xelect for crossover from Qclk to CkPi4Dqs
     */
    UINT32 txeq : 6;
    /* txeq - Bits[28:23], RW, default = 6'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are 
       meaningful.[br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled[br] 
                   for deemphasized, and so on.[br]
                   BDX-change. Increased the width by 1bit for TXEQ tracking
     */
    UINT32 x8writeen : 1;
    /* x8writeen - Bits[29:29], RW, default = 1'b0 
       Rsvd
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} TXGROUP0N1RANK7_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK0_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1560)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK0_8_FNV_DDRIO_DAT7_CH_REG 0x050F4560
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK1_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1564)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK1_8_FNV_DDRIO_DAT7_CH_REG 0x050F4564
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK2_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1568)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK2_8_FNV_DDRIO_DAT7_CH_REG 0x050F4568
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK2_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK3_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B156C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK3_8_FNV_DDRIO_DAT7_CH_REG 0x050F456C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK3_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK4_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1570)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK4_8_FNV_DDRIO_DAT7_CH_REG 0x050F4570
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK4_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK5_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1574)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK5_8_FNV_DDRIO_DAT7_CH_REG 0x050F4574
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK5_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK6_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1578)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK6_8_FNV_DDRIO_DAT7_CH_REG 0x050F4578
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank. Multicast read for CH012 
 * and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK6_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* TXGROUP1N1RANK7_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B157C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK7_8_FNV_DDRIO_DAT7_CH_REG 0x050F457C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Timing control for each lane of the data byte on Rank
 */
typedef union {
  struct {
    UINT32 txdqpicode0 : 6;
    /* txdqpicode0 - Bits[5:0], RW, default = 6'b0 
       Write timing offset for bit[0] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode1 : 6;
    /* txdqpicode1 - Bits[11:6], RW, default = 6'b0 
       Write timing offset for bit[1] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode2 : 6;
    /* txdqpicode2 - Bits[17:12], RW, default = 6'b0 
       Write timing offset for bit[2] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 txdqpicode3 : 6;
    /* txdqpicode3 - Bits[23:18], RW, default = 6'b0 
       Write timing offset for bit[3] of the DQ byte.  0 to toQCK with step size of 
       tQCK/64 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} TXGROUP1N1RANK7_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK0_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1580)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK0_8_FNV_DDRIO_DAT7_CH_REG 0x050F4580
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK0_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK1_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1584)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK1_8_FNV_DDRIO_DAT7_CH_REG 0x050F4584
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK2_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1588)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK2_8_FNV_DDRIO_DAT7_CH_REG 0x050F4588
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK2_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK3_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B158C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK3_8_FNV_DDRIO_DAT7_CH_REG 0x050F458C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK3_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK4_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1590)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK4_8_FNV_DDRIO_DAT7_CH_REG 0x050F4590
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK4_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK5_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1594)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK5_8_FNV_DDRIO_DAT7_CH_REG 0x050F4594
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK5_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK6_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1598)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK6_8_FNV_DDRIO_DAT7_CH_REG 0x050F4598
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK6_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* RXOFFSETN1RANK7_8_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B159C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK7_8_FNV_DDRIO_DAT7_CH_REG 0x050F459C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Read Per Lane Amplifier Voltage Offset. Multicast read for CH012 and CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 saoffset0 : 5;
    /* saoffset0 - Bits[4:0], RW, default = 5'b0 
       Amplifier voltage offset for bit[0] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset1 : 5;
    /* saoffset1 - Bits[9:5], RW, default = 5'b0 
       Amplifier voltage offset for bit[1] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset2 : 5;
    /* saoffset2 - Bits[14:10], RW, default = 5'b0 
       Amplifier voltage offset for bit[2] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 saoffset3 : 5;
    /* saoffset3 - Bits[19:15], RW, default = 5'b0 
       Amplifier voltage offset for bit[3] of the DQ byte. {0: Most negative offset,... 
       8: 0 offset, ... 63: Most postive offset} 
     */
    UINT32 rxdqspsel : 2;
    /* rxdqspsel - Bits[21:20], RW, default = 2'b0 
       x4/x8 DQS mux select[br]
                   [0]=1 selects the raw output from the nibble0 DQS amplifier in x8 
       mode; [br] 
                   [0]=0 select nibble1 amplifier output in x4 mode; [br]
                   [1] is not used.
     */
    UINT32 rxdqsnsel : 2;
    /* rxdqsnsel - Bits[23:22], RW, default = 2'b0 
       select the phase2 dqs clock skewed with max delay to drive rxFIFO write
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} RXOFFSETN1RANK7_8_FNV_DDRIO_DAT7_CH_STRUCT;




/* RXVREFCTRLN1_8_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B15A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45A4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * RxVrefCtrl. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 vrefperbit0 : 7;
    /* vrefperbit0 - Bits[6:0], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben0 : 1;
    /* halflsben0 - Bits[7:7], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit1 : 7;
    /* vrefperbit1 - Bits[14:8], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben1 : 1;
    /* halflsben1 - Bits[15:15], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit2 : 7;
    /* vrefperbit2 - Bits[22:16], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben2 : 1;
    /* halflsben2 - Bits[23:23], RW, default = 1'b0 
       Rsvd
     */
    UINT32 vrefperbit3 : 7;
    /* vrefperbit3 - Bits[30:24], RW, default = 7'b0 
       Per bit Rx Vref Control
     */
    UINT32 halflsben3 : 1;
    /* halflsben3 - Bits[31:31], RW, default = 1'b0 
       shift down the Vref voltage by ~0.5 stepsize
     */
  } Bits;
  UINT32 Data;
} RXVREFCTRLN1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATATRAINFEEDBACKN1_8_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B15A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45A8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Training Feedback Results. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 datatrainfeedback : 9;
    /* datatrainfeedback - Bits[8:0], RW, default = 9'b0 
       Data Training Feedback Storage [br]
                Training Step    Bits    Nibble    Description[br]
               --------------    ----    -------  ---------------[br]
               Cross Over Cal     0      1        Tx Dqs Phase Detection[br]
               Cross Over Cal     1      1        Tx Dq Phase Detection[br]
               Cross Over Cal     2      1        Rec En Phase Detection[br]
               RcvEn              8:0    1        Byte detection[br]
               Wr Leveling        8:0    1        Byte detection[br]
               Senseamp offset    3:0    1        Dq Senseamp offset Detection
     */
    UINT32 rsvd_9 : 23;
  } Bits;
  UINT32 Data;
} DATATRAINFEEDBACKN1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRFSMCALN1_8_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B15C8)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRFSMCALN1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45C8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Fsm for  Voc and Xover caliberation, Voc caliberation is a procedure that cancel 
 * out intrinsic offset in analogue receivers and Xover training ,it aligns all the 
 * local dll to the global qclk 
 */
typedef union {
  struct {
    UINT32 saoffset0fsm : 5;
    /* saoffset0fsm - Bits[4:0], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of first receiver is done(read 
       only) 
     */
    UINT32 saoffset1fsm : 5;
    /* saoffset1fsm - Bits[9:5], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of second receiver is done(read 
       only) 
     */
    UINT32 saoffset2fsm : 5;
    /* saoffset2fsm - Bits[14:10], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of third receiver is done(read 
       only) 
     */
    UINT32 saoffset3fsm : 5;
    /* saoffset3fsm - Bits[19:15], ROV, default = 5'b0 
       Loads the value from fsm to Cr when caliberation of fourth receiver is done(read 
       only) 
     */
    UINT32 fsmcalcomplete : 1;
    /* fsmcalcomplete - Bits[20:20], ROV, default = 1'b0 
        when the caliberation is done
     */
    UINT32 refpiclkdelayfsm : 6;
    /* refpiclkdelayfsm - Bits[26:21], ROV, default = 6'b0 
       Load FSM with a code at which , the transition is detected.
     */
    UINT32 fsmcaldone : 1;
    /* fsmcaldone - Bits[27:27], ROV, default = 1'b0 
       when the caliberation of voc or x-over is done
     */
    UINT32 fsmtimersel : 1;
    /* fsmtimersel - Bits[28:28], RW, default = 1'b0 
       Timer is set for either 8 cycles or 16 cycles
     */
    UINT32 fsmpatternsel : 1;
    /* fsmpatternsel - Bits[29:29], RW, default = 1'b0 
        It selects whther the pattern of three bit or two bit is selected when the 
       transition happen from 1-->0 for pdout and receiver , to avoid glitch 
     */
    UINT32 fsmvoccal : 1;
    /* fsmvoccal - Bits[30:30], RW, default = 1'b0 
        This bit tells , wether the xover or voc caliberation will be done, if its zero 
       x-over caliberation is done and vice-versa 
     */
    UINT32 fsmcalen : 1;
    /* fsmcalen - Bits[31:31], RW, default = 1'b0 
       FsmCalEn starts the voc-Xover Fsm
     */
  } Bits;
  UINT32 Data;
} DDRCRFSMCALN1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL4N1_8_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B15CC)                                                     */
/* Register default value:              0x801F0000                            */
#define DATACONTROL4N1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45CC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 1;
    UINT32 vsshiorvrefctl1 : 8;
    /* vsshiorvrefctl1 - Bits[8:1], RW, default = 8'b0 
        VssHi control bits[br]
               Field Description: [br]
       [br]
                Field             Bits    Description[br]
               -------            ----    ---------------------------------[br]
       	 disoverflow        24     Disable overflow logic from BDW[br]
         fast panic enable  25     Bypass 2 flops in panic path for faster panic 
       response[br] 
         vsshivccd2by3cfgen 26     Forces the driver DAC to get supply from 2/3 
       vccd[br] 
         vsshivccd2by3cfg   27:29  Current trim setting for 2/3 vccd mode, 001 fast, 
       011 typical, 111 slow silicon [br] 
       	 ddrvssh2ndcompen   30     Enable second comparator (ZBB mode)[br]
         comp high bwen     31     Improve comparator performance to support low 
       digital voltage range 
     */
    UINT32 dqtxeqsel : 1;
    /* dqtxeqsel - Bits[9:9], RW, default = 1'b0 
       DQ TxEq setting selection to achieve constant EQ
     */
    UINT32 rsvd_10 : 2;
    UINT32 vrefpwrsave : 1;
    /* vrefpwrsave - Bits[12:12], RW, default = 1'h0 
                If set to 0 (default), Vref generators selected through RxVrefSel[3:0] 
       are always on.[br] 
                If set to 1, Vref generators are switched off while in Self Refresh.
     */
    UINT32 vreftonblbus : 4;
    /* vreftonblbus - Bits[16:13], RW, default = 4'b1000 
       Per-bit Enable connection to share Vref Bus
     */
    UINT32 rxvrefsel : 4;
    /* rxvrefsel - Bits[20:17], RW, default = 4'b1111 
       Per Bit Verf generator Enable
     */
    UINT32 refpiclkdelay : 6;
    /* refpiclkdelay - Bits[26:21], RW, default = 6'b000000 
       RefPiClkDelay
     */
    UINT32 fnvcrsdlbypassen : 1;
    /* fnvcrsdlbypassen - Bits[27:27], RW, default = 1'b0 
       FNV Slave Delay Line Bypass Enable
     */
    UINT32 fnvcrdllbypassen : 1;
    /* fnvcrdllbypassen - Bits[28:28], RW, default = 1'b0 
       FNV DLL Bypass Enable
     */
    UINT32 fnvcrvisaen : 1;
    /* fnvcrvisaen - Bits[29:29], RW, default = 1'b0 
       FNV Visa Fub Enable - data fub used for VISA if asserted
     */
    UINT32 writecrcenable : 1;
    /* writecrcenable - Bits[30:30], RW, default = 1'h0 
       Enable write crc mode
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DATACONTROL4N1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* ATTACK1SELN1_8_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B15D0)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK1SELN1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45D0
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr1d0 : 3;
    /* attackr1d0 - Bits[2:0], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[0] in a byte
     */
    UINT32 attackr1d1 : 3;
    /* attackr1d1 - Bits[5:3], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[1] in a byte
     */
    UINT32 attackr1d2 : 3;
    /* attackr1d2 - Bits[8:6], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[2] in a byte
     */
    UINT32 attackr1d3 : 3;
    /* attackr1d3 - Bits[11:9], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[3] in a byte
     */
    UINT32 attackr1d4 : 3;
    /* attackr1d4 - Bits[14:12], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[4] in a byte
     */
    UINT32 attackr1d5 : 3;
    /* attackr1d5 - Bits[17:15], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[5] in a byte
     */
    UINT32 attackr1d6 : 3;
    /* attackr1d6 - Bits[20:18], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[6] in a byte
     */
    UINT32 attackr1d7 : 3;
    /* attackr1d7 - Bits[23:21], RW, default = 3'b0 
       xtalk aggressor1 select for dq bit[7] in a byte
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} ATTACK1SELN1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* COMPDATA0N1_8_FNV_DDRIO_DAT7_CH_REG supported on:                          */
/*       EKV (0x401B15D4)                                                     */
/* Register default value:              0x00028000                            */
#define COMPDATA0N1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45D4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[5:0], RWV, default = 6'b0 
       Data Rcomp DrvUp Comp Value can be overriden via message channel if periodical 
       retrain is disable 
     */
    UINT32 spare0 : 3;
    /* spare0 - Bits[8:6], RWV, default = 3'b0 
       Spare
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[14:9], RWV, default = 6'b0 
       Data Rcomp DrvDown Comp Value
     */
    UINT32 vtcomp : 5;
    /* vtcomp - Bits[19:15], RWV, default = 5'b00101 
       VT Comp Values
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[26:20], RWV, default = 7'b0 
       Data TCO Comp Value.
     */
    UINT32 slewratecomp : 5;
    /* slewratecomp - Bits[31:27], RWV, default = 5'b0 
       Data Slew Rate Comp Value.
     */
  } Bits;
  UINT32 Data;
} COMPDATA0N1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* COMPDATA1N1_8_FNV_DDRIO_DAT7_CH_REG supported on:                          */
/*       EKV (0x401B15D8)                                                     */
/* Register default value:              0x04010000                            */
#define COMPDATA1N1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45D8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rcompodtup : 6;
    /* rcompodtup - Bits[5:0], RWV, default = 6'b0 
       Data Rcomp OdtUp Comp Value
     */
    UINT32 rcompodtdown : 6;
    /* rcompodtdown - Bits[11:6], RWV, default = 6'b0 
       Data Rcomp Odt-Down Comp Value
     */
    UINT32 panicdrvdn : 10;
    /* panicdrvdn - Bits[21:12], RWV, default = 10'b0000010000 
       Panic DrvDn RComp Value.
     */
    UINT32 panicdrvup : 10;
    /* panicdrvup - Bits[31:22], RWV, default = 10'b0000010000 
       Panic DrvUp RComp Comp Value.
     */
  } Bits;
  UINT32 Data;
} COMPDATA1N1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATAOFFSETTRAINN1_8_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B15DC)                                                     */
/* Register default value:              0x00000000                            */
#define DATAOFFSETTRAINN1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45DC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rcvenoffset : 6;
    /* rcvenoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained RxRcvEn Value.
     */
    UINT32 rxdqsoffset : 7;
    /* rxdqsoffset - Bits[12:6], RW, default = 7'b0 
       Offset added to Trained Rx DQS Value.
     */
    UINT32 txdqoffset : 6;
    /* txdqoffset - Bits[18:13], RW, default = 6'b00 
       Offset added to Trained Tx DQ Value.
     */
    UINT32 txdqsoffset : 6;
    /* txdqsoffset - Bits[24:19], RW, default = 6'b000000 
       Ooffset added to Trained Tx DQS Value.
     */
    UINT32 vrefoffset : 7;
    /* vrefoffset - Bits[31:25], RW, default = 7'b0000000 
       2s Compliment offset added to Trained Vref Value.  Positive number increases 
       Vref, and each step is Vdd/384. 
     */
  } Bits;
  UINT32 Data;
} DATAOFFSETTRAINN1_8_FNV_DDRIO_DAT7_CH_STRUCT;




/* ATTACK0SELN1_8_FNV_DDRIO_DAT7_CH_REG supported on:                         */
/*       EKV (0x401B15E4)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK0SELN1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45E4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Select 2 aggressors out 3 neighbors in a nibble. Multicast read for CH012 and 
 * CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 attackr0d0 : 3;
    /* attackr0d0 - Bits[2:0], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[0] in a byte
     */
    UINT32 attackr0d1 : 3;
    /* attackr0d1 - Bits[5:3], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[1] in a byte
     */
    UINT32 attackr0d2 : 3;
    /* attackr0d2 - Bits[8:6], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[2] in a byte
     */
    UINT32 attackr0d3 : 3;
    /* attackr0d3 - Bits[11:9], RW, default = 3'b0 
       xtalk aggressor0 select for dq bit[3] in a byte
     */
    UINT32 txddjcen : 4;
    /* txddjcen - Bits[15:12], RW, default = 4'b000 
       TxddjcEn
     */
    UINT32 dfeenable : 1;
    /* dfeenable - Bits[16:16], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfedlysel : 1;
    /* dfedlysel - Bits[17:17], RW, default = 1'b0 
       DfeEnable
     */
    UINT32 dfecode : 4;
    /* dfecode - Bits[21:18], RW, default = 4'b0000 
       xtalk aggressor0 select for dq bit[7] in a byte
     */
    UINT32 rsvd_22 : 10;
  } Bits;
  UINT32 Data;
} ATTACK0SELN1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL0N1_8_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B15E8)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL0N1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45E8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxtrainingmode : 1;
    /* rxtrainingmode - Bits[0:0], RW, default = 1'b0 
       Enables Read DqDqs Training Mode
     */
    UINT32 wltrainingmode : 1;
    /* wltrainingmode - Bits[1:1], RW, default = 1'b0 
       Enables Write Leveling Training Mode
     */
    UINT32 rltrainingmode : 1;
    /* rltrainingmode - Bits[2:2], RW, default = 1'b0 
       Enables Read Leveling Training Mode.
     */
    UINT32 senseamptrainingmode : 1;
    /* senseamptrainingmode - Bits[3:3], RW, default = 1'b0 
       Enables SenseAmp offset cancellation training mode.
     */
    UINT32 txon : 1;
    /* txon - Bits[4:4], RW, default = 1'b0 
       When set, forces DQ/DQS drive enable to active.  Used in power measurements and 
       IO loopback mode 
     */
    UINT32 rfon : 1;
    /* rfon - Bits[5:5], RW, default = 1'b0 
       Power-down Disable:  Forces register file read, overriding the rank power down 
       logic. 
     */
    UINT32 rxpion : 1;
    /* rxpion - Bits[6:6], RW, default = 1'b0 
       Power-down Disable:  Forces RxDqs PI clocks on.
     */
    UINT32 txpion : 1;
    /* txpion - Bits[7:7], RW, default = 1'b0 
       Power-down Disable:  Forces TxDq and TxDqs PI clocks on.
     */
    UINT32 internalclockson : 1;
    /* internalclockson - Bits[8:8], RW, default = 1'b0 
                 Forces on master DLL and all PI enables ON, despite both channel in 
       SelfRefresh, etc. low power states. 
     */
    UINT32 saoffsettraintxon : 1;
    /* saoffsettraintxon - Bits[9:9], RW, default = 1'b0 
       SaOffsetTrainTxon
     */
    UINT32 txdisable : 1;
    /* txdisable - Bits[10:10], RW, default = 1'b0 
       Power-down:  All write commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 rxdisable : 1;
    /* rxdisable - Bits[11:11], RW, default = 1'b0 
       Power-down:  All read commands are ignored for this data-byte.  Used in 
       single-channel MCI mode. 
     */
    UINT32 txlong : 1;
    /* txlong - Bits[12:12], RW, default = 1'b0 
       Sets the duration of the first transmitted DQ bit of the burst. 0=1UI, 1=2UI.
     */
    UINT32 rsvd_13 : 5;
    UINT32 driversegmentenable : 1;
    /* driversegmentenable - Bits[18:18], RW, default = 1'b0 
       Controls which segements of the driver are enabled:  {0: All, 1: Only ODT}.
     */
    UINT32 rsvd_19 : 1;
    UINT32 readrfrd : 1;
    /* readrfrd - Bits[20:20], RWV, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a read command to Rank = ReadRFRank 
     */
    UINT32 readrfwr : 1;
    /* readrfwr - Bits[21:21], RWV, default = 1'b0 
       Self clearing command bit. When set, it will download the three 32 bit lines 
       from the RegFile associated with a write command to Rank = ReadRFRank 
     */
    UINT32 readrfrank : 3;
    /* readrfrank - Bits[24:22], RWV, default = 3'b000 
       Specifies the rank that ReadRFRd or ReadRFWr will manually download the CR 
       values from 
     */
    UINT32 forceodton : 1;
    /* forceodton - Bits[25:25], RW, default = 1'b0 
       ODT is forced-on.
     */
    UINT32 odtsampoff : 1;
    /* odtsampoff - Bits[26:26], RW, default = 1'b0 
       ODT and Senseamp are forced-off.
     */
    UINT32 disableodtstatic : 1;
    /* disableodtstatic - Bits[27:27], RW, default = 1'b0 
       not used
     */
    UINT32 ddrcrforceodton : 1;
    /* ddrcrforceodton - Bits[28:28], RW, default = 1'b0 
       force ODT to the transmitter on, regardless of state of DriveEnable
     */
    UINT32 spare : 1;
    /* spare - Bits[29:29], RW, default = 1'b0 
       spare
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[30:30], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
    UINT32 longpreambleenable : 1;
    /* longpreambleenable - Bits[31:31], RW, default = 1'b0 
       Enables long preamble for DDR4 devices
     */
  } Bits;
  UINT32 Data;
} DATACONTROL0N1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL1N1_8_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B15EC)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL1N1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45EC
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 xtalkpienable : 1;
    /* xtalkpienable - Bits[0:0], RW, default = 1'b0 
       enable phase shift based crosstalk cancellation.
     */
    UINT32 xtalkpisign : 1;
    /* xtalkpisign - Bits[1:1], RW, default = 1'b0 
       select if 0: capacitive coupling; or 1: inductive coupling; xtalk dominant
     */
    UINT32 xtalkpidelta : 3;
    /* xtalkpidelta - Bits[4:2], RW, default = 3'b0 
       delta phase shift in one aggressor is in effect
     */
    UINT32 xtalkpideltarank5 : 2;
    /* xtalkpideltarank5 - Bits[6:5], RW, default = 2'b0 
       XtalkPiDeltaRank5
     */
    UINT32 rsvd_7 : 3;
    UINT32 rxbiasctl : 3;
    /* rxbiasctl - Bits[12:10], RW, default = 3'b0 
       Controls the Current and BW of the Receiver: {0: 0.34Idll, 1: 0.66Idll, 2: 
       0.88Idll, 3: Idll, 4: 1.33Idll, 5: 1.66Idll[br] 
       , 6: 2.00Idll, 7: 2.33Idll}
     */
    UINT32 odtdelay : 4;
    /* odtdelay - Bits[16:13], RW, default = 4'b0000 
       Controls ODT turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 odtduration : 3;
    /* odtduration - Bits[19:17], RW, default = 3'b000 
       Controls the ODT ON duration from (Code, Duration) = (0x0, 11 tQCK) to (0x7, 18 
       tQCK) 
     */
    UINT32 senseampdelay : 4;
    /* senseampdelay - Bits[23:20], RW, default = 4'b0000 
       Controls Senseamp turn-on delay.   To turn on N tQCK before RcvEn, program to 
       RcvEn[8:6]  N + 4. 
     */
    UINT32 senseampduration : 3;
    /* senseampduration - Bits[26:24], RW, default = 3'b000 
       Controls the SenseAmp ON duration from (Code, Duration) = (0x0, 11 tQCK) to 
       (0x7, 18 tQCK) 
     */
    UINT32 burstendodtdelay : 3;
    /* burstendodtdelay - Bits[29:27], RW, default = 3'b000 
       Cycles after the burst, when the current pulse should turn on by enabling OdtEn 
       for 1 cycle 
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DATACONTROL1N1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL2N1_8_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B15F0)                                                     */
/* Register default value:              0x07800000                            */
#define DATACONTROL2N1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45F0
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Data offsets to training values. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rxvocselqnnnh : 5;
    /* rxvocselqnnnh - Bits[4:0], RW, default = 5'b0 
       Enables SenseAmp offset cancellation training mode
     */
    UINT32 forcebiason : 1;
    /* forcebiason - Bits[5:5], RW, default = 1'b0 
       Force on the internal Vref and Rx bias circuit, regardless of any other power 
       downs 
     */
    UINT32 forcerxon : 1;
    /* forcerxon - Bits[6:6], RW, default = 1'b0 
       Force on the RxAmp only (as opposed to OdtSampOn, which turns on both ODT and 
       the amplifier).[br] 
               Use for overclocking support where we may not be able to hit the fast 
       exit latency 
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[7:7], RW, default = 1'b0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[8:8], RW, default = 1'h0 
       DDRCOMP_CR_DDRCRCOMPOVR CTTermination
     */
    UINT32 imodecfg : 1;
    /* imodecfg - Bits[9:9], RW, default = 1'h0 
       Imode Select, 0: Swing Boost Mode; 1: (not supported) Xtalk Cancellation Mode
     */
    UINT32 imodeenable : 1;
    /* imodeenable - Bits[10:10], RW, default = 1'h0 
       Enable Imode EQ per byte lane. [br]
                 0: Imode completely off [br]
                 1: Imode enabled (Xtalk cancellation OR swing boost selected by 
       ImodeCfg 
     */
    UINT32 enearlywriterankqnnnh : 1;
    /* enearlywriterankqnnnh - Bits[11:11], RW, default = 1'h0 
       Spare
     */
    UINT32 picodepipeqnnnh : 1;
    /* picodepipeqnnnh - Bits[12:12], RW, default = 1'h0 
       Spare
     */
    UINT32 spare : 2;
    /* spare - Bits[14:13], RW, default = 2'h0 
       Spare
     */
    UINT32 imodebiasen : 1;
    /* imodebiasen - Bits[15:15], RW, default = 1'h0 
       tbd
     */
    UINT32 rsvd_16 : 2;
    UINT32 imodeeqcode : 4;
    /* imodeeqcode - Bits[21:18], RW, default = 4'h0 
       Binary Imode Coefficient, sets Imode current strength.[br]
                 (used for either Xtalk cancellation or swing boost)[br]
                 Code   Imode Current (approx)[br]
                 ------------------[br]
                 0000        0[br]
                 0001        1[br]
                 0010        2[br]
                   *[br]
                   *[br]
                   *[br]
                 1111        3
     */
    UINT32 imodescompovrd : 1;
    /* imodescompovrd - Bits[22:22], RW, default = 1'h0 
       Imode Scomp Override Enable
     */
    UINT32 rxdqssaoffset : 5;
    /* rxdqssaoffset - Bits[27:23], RW, default = 5'b01111 
       strobe Rx Amp offset control
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DATACONTROL2N1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* DATACONTROL3N1_8_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B15F4)                                                     */
/* Register default value:              0x2198DFCF                            */
#define DATACONTROL3N1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45F4
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * static configurations of tx and rx. Multicast read for CH012 and CH345 versions 
 * are not supported. 
 */
typedef union {
  struct {
    UINT32 drvstaticlegcfg : 2;
    /* drvstaticlegcfg - Bits[1:0], RW, default = 2'b11 
       binary default setting for static legs in DRV mode[br]
                  0    0       0 legs (EQ disabled)[br]
                  0    1       3 legs (EQ max 3 legs)[br]
                  1    0       6 legs (EQ max 6 legs)[br]
                  1    1      12 legs (EQ max 12 legs)[br]
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher [br] 
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit [br] 
                 the higher Ron target value.
     */
    UINT32 odtstaticlegcfg : 2;
    /* odtstaticlegcfg - Bits[3:2], RW, default = 2'b11 
       binary default setting for static legs in ODT mode[br]
                  0    0       0 legs (EQ disabled)[br]
                  0    1       3 legs (EQ max 3 legs)[br]
                  1    0       6 legs (EQ max 6 legs)[br]
                  1    1      12 legs (EQ max 12 legs)[br]
                 In most cases, we would set this to be 11, but for some configurations 
       that require a higher [br] 
                 Ron value, we need to reduce the number of static legs enabled so that 
       fast corner can hit [br] 
                 the higher Ron target value.
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[4:4], RW, default = 1'b0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rsvd_5 : 1;
    UINT32 odtsegmentenable : 3;
    /* odtsegmentenable - Bits[8:6], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.  [br]
                            After that, a shift register will determine the enabled 
       segments 
     */
    UINT32 datasegmentenable : 3;
    /* datasegmentenable - Bits[11:9], RW, default = 3'b111 
       This will tell which  segments are enabled out of reset.
     */
    UINT32 longodtr2w : 1;
    /* longodtr2w - Bits[12:12], RW, default = 1'b1 
       When set, it will extends ODT by 1 qclk on read to write turnarounds. hsd 230773
     */
    UINT32 spare2 : 1;
    /* spare2 - Bits[13:13], RW, default = 1'b0 
       Spare2
     */
    UINT32 imodebiasvrefen : 1;
    /* imodebiasvrefen - Bits[14:14], RW, default = 1'b1 
       imodebias control
     */
    UINT32 imodebiasrxbiastrim : 3;
    /* imodebiasrxbiastrim - Bits[17:15], RW, default = 3'b001 
       imodebias control. Also used for rxbiasana
     */
    UINT32 spare : 1;
    /* spare - Bits[18:18], RW, default = 1'b0 
       Spare
     */
    UINT32 imodebiasdfxddr4legup : 1;
    /* imodebiasdfxddr4legup - Bits[19:19], RW, default = 1'b1 
       imodebias control
     */
    UINT32 imodebiasdfxlegdn : 1;
    /* imodebiasdfxlegdn - Bits[20:20], RW, default = 1'b1 
       imodebias control
     */
    UINT32 ddrcrctlecapen : 2;
    /* ddrcrctlecapen - Bits[22:21], RW, default = 2'b00 
       ctle capacitor setting
     */
    UINT32 ddrcrctleresen : 2;
    /* ddrcrctleresen - Bits[24:23], RW, default = 2'b11 
       ctle resistor setting
     */
    UINT32 pcasbiasclosedloopen : 1;
    /* pcasbiasclosedloopen - Bits[25:25], RW, default = 1'b0 
       RxBias CR
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[26:26], RW, default = 1'b0 
       RxBias CR
     */
    UINT32 rxbiasfoldedlegtrim : 2;
    /* rxbiasfoldedlegtrim - Bits[28:27], RW, default = 2'b00 
       RxBias CR
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[30:29], RW, default = 2'b01 
       RxBias CR
     */
    UINT32 postambleenable : 1;
    /* postambleenable - Bits[31:31], RW, default = 1'h0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DATACONTROL3N1_8_FNV_DDRIO_DAT7_CH_STRUCT;


/* VSSHIORVREFCONTROLN1_8_FNV_DDRIO_DAT7_CH_REG supported on:                 */
/*       EKV (0x401B15F8)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROLN1_8_FNV_DDRIO_DAT7_CH_REG 0x050F45F8
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings.[br] 
       [br]
               VssHi Field Description: [br]
       [br]
                Field          Bits    Description[br]
               -------         ----    
       ---------------------------------------------------------------------------------------------------------------------------------------------------------------[br] 
               SelCode          23     Selects the code to output:  {0: Code, 1: 
       TCode}[br] 
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode   [br] 
               PanicVoltage  21:18  Unsigned integer controlling the voltage error on 
       VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV)[br] 
               CloseLoop        17     Enables the VssHi close loop tracking [br]
               PanicEn          16     Enables the VssHi Panic Driver[br]
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError)[br] 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved[br] 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128}[br] 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROLN1_8_FNV_DDRIO_DAT7_CH_STRUCT;




/* DDRCRCLKRANKSUSED_CH_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1A00)                                                     */
/* Register default value:              0x0000E00F                            */
#define DDRCRCLKRANKSUSED_CH_FNV_DDRIO_DAT7_CH_REG 0x050F4A00
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CLK Ranks Used. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ranken : 4;
    /* ranken - Bits[3:0], RW, default = 4'b1111 
       Enables output buffers, PI clocks and output datapaths for this rank
     */
    UINT32 rsvd_4 : 4;
    UINT32 ddrtclk1en : 1;
    /* ddrtclk1en - Bits[8:8], RW, default = 1'h0 
       DDRT Mode Clk1 Enable
     */
    UINT32 ddrtclk3en : 1;
    /* ddrtclk3en - Bits[9:9], RW, default = 1'h0 
       DDRT Mode Clk3 Enable
     */
    UINT32 ddrtclk0en : 1;
    /* ddrtclk0en - Bits[10:10], RW, default = 1'h0 
       DDRT Mode Clk0 Enable
     */
    UINT32 ddrtclk2en : 1;
    /* ddrtclk2en - Bits[11:11], RW, default = 1'h0 
       DDRT Mode Clk2 Enable
     */
    UINT32 ddrttraining : 1;
    /* ddrttraining - Bits[12:12], RW, default = 1'b0 
       Asserted during the Read ID fine training sub-step Causes the sampled value for 
       the Read ID signal to be placed in a results register within the DDRIO fub 
     */
    UINT32 drvsegen : 3;
    /* drvsegen - Bits[15:13], RW, default = 3'b111 
       Segments to enable for driving (inverse for ODT)
     */
    UINT32 drvpupdis : 1;
    /* drvpupdis - Bits[16:16], RW, default = 1'h0 
       This will be connected to all tx, and must toggle 1->0 after a power state
     */
    UINT32 clkdealignbitqnnnh : 1;
    /* clkdealignbitqnnnh - Bits[17:17], RW, default = 1'b0 
       XNOR with DAlign
     */
    UINT32 fifoptctlqnnnh : 1;
    /* fifoptctlqnnnh - Bits[18:18], RW, default = 1'b0 
       Fifo Point separation bit
     */
    UINT32 spare : 1;
    /* spare - Bits[19:19], RW, default = 1'b0 
       spare
     */
    UINT32 clkctlecapen : 2;
    /* clkctlecapen - Bits[21:20], RW, default = 2'h0 
       ClkCtleCapEn for Clk RX buffer
     */
    UINT32 clkctleresen : 2;
    /* clkctleresen - Bits[23:22], RW, default = 2'h0 
       ClkCtleResEn for Clk RX buffer
     */
    UINT32 rsvd_24 : 1;
    UINT32 refpiclk : 6;
    /* refpiclk - Bits[30:25], RW, default = 6'h0 
       RefD0Clk
     */
    UINT32 ddrclkverticalbumpplacmenten : 1;
    /* ddrclkverticalbumpplacmenten - Bits[31:31], RW, default = 1'h0 
       Ddr CLK fub vertical bump placement enable for RGT/FNV
     */
  } Bits;
  UINT32 Data;
} DDRCRCLKRANKSUSED_CH_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCLKCOMP_CLK_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B1A04)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCLKCOMP_CLK_FNV_DDRIO_DAT7_CH_REG 0x050F4A04
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CLK Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 5;
    /* scomp - Bits[4:0], RWV, default = 5'h0 
       Scomp Value
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[11:5], RWV, default = 7'h0 
       Tco Comp Value
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[17:12], RWV, default = 6'h0 
       Rcomp Drv Up Value
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[23:18], RWV, default = 6'h0 
       Rcomp Drv Dn Value
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[26:24], RWV, default = 3'b100 
       LevelShift Comp Value. Not unsed, controlled via MsgCh only.
     */
    UINT32 spare : 5;
    /* spare - Bits[31:27], RWV, default = 5'h0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCLKCOMP_CLK_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCLKCOMPOFFSET_CH_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1A08)                                                     */
/* Register default value:              0x82010000                            */
#define DDRCRCLKCOMPOFFSET_CH_FNV_DDRIO_DAT7_CH_REG 0x050F4A08
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CLK Comp Offset Values. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 17;
    UINT32 clkpcasbiasclosedloopen : 1;
    /* clkpcasbiasclosedloopen - Bits[17:17], RW, default = 1'b0 
       ClkPcasBiasClosedLoopEn for Clk Rx buf
     */
    UINT32 clkrxbiasselqnnnh : 1;
    /* clkrxbiasselqnnnh - Bits[18:18], RW, default = 1'b0 
       ClkRxBiasSelQnnnH for Clk Rx Buf
     */
    UINT32 clkrxeq : 3;
    /* clkrxeq - Bits[21:19], RW, default = 3'b0 
       ClkRxEq for Clk Rx Buf
     */
    UINT32 ddrclkrxbiasctl : 3;
    /* ddrclkrxbiasctl - Bits[24:22], RW, default = 3'b0 
       DdrClkRxBiasCtl for Clk Rx Buf
     */
    UINT32 forceclkbiason : 1;
    /* forceclkbiason - Bits[25:25], RW, default = 1'b1 
       ForceClkBiason for Clk Rx Buf
     */
    UINT32 rsvd_26 : 1;
    UINT32 ddrclkrxbiasfoldedlegtrim : 2;
    /* ddrclkrxbiasfoldedlegtrim - Bits[28:27], RW, default = 2'b0 
       DdrClkRxBiasFoldedLegTrim for Clk Rx Buf
     */
    UINT32 ddrclkrxbiasgcncomp : 2;
    /* ddrclkrxbiasgcncomp - Bits[30:29], RW, default = 2'b0 
       DdrClkRxBiasGcnComp for Clk Rx Buf
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DDRCRCLKCOMPOFFSET_CH_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCLKPICODE_CH_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1A0C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCLKPICODE_CH_FNV_DDRIO_DAT7_CH_REG 0x050F4A0C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CLK Pi Codes. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 pisettingrank0 : 6;
    /* pisettingrank0 - Bits[5:0], RW, default = 6'b0 
       Pi Setting, Rank 0[br]
                            [5:0] - Pi setting for xxDDR_CLK_P/N[0]
     */
    UINT32 pilogicdelayrank0 : 1;
    /* pilogicdelayrank0 - Bits[6:6], RW, default = 1'b0 
       Pi Logic Delay, Rank 0, delays CLK by an extra qclk cycle
     */
    UINT32 rsvd_7 : 1;
    UINT32 pisettingrank1 : 6;
    /* pisettingrank1 - Bits[13:8], RW, default = 6'b0 
       Pi Setting, Rank 1[br]
                            [5:0] - Pi setting for xxDDR_CLK_P/N[1]
     */
    UINT32 pilogicdelayrank1 : 1;
    /* pilogicdelayrank1 - Bits[14:14], RW, default = 1'b0 
       Pi Logic Delay, Rank 1
     */
    UINT32 spare1 : 1;
    /* spare1 - Bits[15:15], RW, default = 1'b0 
       not used
     */
    UINT32 pisettingrank2 : 6;
    /* pisettingrank2 - Bits[21:16], RW, default = 6'b0 
       Pi Setting, Rank 2[br]
                            [5:0] - Pi setting for xxDDR_CLK_P/N[2]
     */
    UINT32 pilogicdelayrank2 : 1;
    /* pilogicdelayrank2 - Bits[22:22], RW, default = 1'b0 
       Pi Logic Delay, Rank 2
     */
    UINT32 rsvd_23 : 1;
    UINT32 pisettingrank3 : 6;
    /* pisettingrank3 - Bits[29:24], RW, default = 6'b0 
       Pi Setting, Rank 3[br]
                            [5:0] - Pi setting for xxDDR_CLK_P/N[3]
     */
    UINT32 pilogicdelayrank3 : 1;
    /* pilogicdelayrank3 - Bits[30:30], RW, default = 1'b0 
       Pi Logic Delay, Rank 3
     */
    UINT32 rsvd_31 : 1;
  } Bits;
  UINT32 Data;
} DDRCRCLKPICODE_CH_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCLKCONTROLS_CH_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1A10)                                                     */
/* Register default value:              0x00066000                            */
#define DDRCRCLKCONTROLS_CH_FNV_DDRIO_DAT7_CH_REG 0x050F4A10
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CLK Controls. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ddrphasextalkenableqnnnh : 1;
    /* ddrphasextalkenableqnnnh - Bits[0:0], RW, default = 1'b0 
       When Phased based xtalk cancellation is enabled, set this to delay cmd to 
       maintain constant Tcwl. 
     */
    UINT32 visabusenable : 1;
    /* visabusenable - Bits[1:1], RW, default = 1'h0 
       Enable Visa debug bus LCB for CLK fub
     */
    UINT32 pirefclkdivenable : 1;
    /* pirefclkdivenable - Bits[2:2], RW, default = 1'h0 
       enable PiRef clock divider in xover
     */
    UINT32 xoveropmode : 1;
    /* xoveropmode - Bits[3:3], RW, default = 1'h0 
       Xover Select between grid-PiRef and PiRef-PiN
     */
    UINT32 clkodtpdnsegen : 3;
    /* clkodtpdnsegen - Bits[6:4], RW, default = 3'b0 
       not used
     */
    UINT32 txon : 1;
    /* txon - Bits[7:7], RW, default = 1'b0 
       Forces the CLK transmitter on, overriding any power downs
     */
    UINT32 intclkon : 1;
    /* intclkon - Bits[8:8], RW, default = 1'b0 
       Forces on all PI enables and the LCB enables in the AFE fubs.  Does not force on 
       D0 PI enable or override weaklock mode. 
     */
    UINT32 repclkon : 1;
    /* repclkon - Bits[9:9], RW, default = 1'b0 
       not used
     */
    UINT32 iolbctl : 2;
    /* iolbctl - Bits[11:10], RW, default = 2'b0 
       Controls IO Loopback.  Ctl[1] enables IOLB mode and Ctl[0] controls if this is a 
       Setup(0) or Hold(1) test 
     */
    UINT32 odtmode : 1;
    /* odtmode - Bits[12:12], RW, default = 1'b0 
       Forces the driver in ODT mode, where both the pullup and pulldown are enabled.  
       Used for Rstray testing 
     */
    UINT32 odtpupsegen : 3;
    /* odtpupsegen - Bits[15:13], RW, default = 3'b011 
       not used
     */
    UINT32 is_rdimm : 1;
    /* is_rdimm - Bits[16:16], RW, default = 1'h0 
       set to drive DC on xxCLK during P6 and S3 if DDR3 + RDIMM is populated
     */
    UINT32 statlegen : 2;
    /* statlegen - Bits[18:17], RW, default = 2'b11 
       static leg enable
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[19:19], RW, default = 1'h0 
       Enable finer vref resolution in ddr4
     */
    UINT32 cttermination : 1;
    /* cttermination - Bits[20:20], RW, default = 1'h0 
       1: Center Tap Termination for DDR3; 0: Vddq Termination for DDR4/Intel SMI 2
     */
    UINT32 rxvref : 7;
    /* rxvref - Bits[27:21], RW, default = 7'b0 
       IOLB Vref Control.  2s compliment encoding from 1/3 Vdd to 2/3 Vdd with step 
       size of Vdd/192 (7.8mV) 
     */
    UINT32 halflsbbit : 1;
    /* halflsbbit - Bits[28:28], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 rsvd_29 : 1;
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[30:30], RW, default = 1'b0 
       level shift hold enable:[br]
                            Holds the HV control values and power down the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[31:31], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
  } Bits;
  UINT32 Data;
} DDRCRCLKCONTROLS_CH_FNV_DDRIO_DAT7_CH_STRUCT;




/* DDRCRCLKTRAINING_CH_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1A1C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCLKTRAINING_CH_FNV_DDRIO_DAT7_CH_REG 0x050F4A1C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Clk Training Register. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 trainingoffset : 6;
    /* trainingoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained PiCode Value.
     */
    UINT32 iodirectionvalid : 1;
    /* iodirectionvalid - Bits[6:6], RW, default = 1'b0 
       IODirectionValid
     */
    UINT32 rsvd_7 : 1;
    UINT32 pclkxoverphasedet : 4;
    /* pclkxoverphasedet - Bits[11:8], ROV, default = 4'b0 
       P Clock Buffer Xover Phase Detect
     */
    UINT32 nclkxoverphasedet : 4;
    /* nclkxoverphasedet - Bits[15:12], ROV, default = 4'b0 
       N Clock Buffer Xover Phase Detect
     */
    UINT32 rsvd_16 : 16;
  } Bits;
  UINT32 Data;
} DDRCRCLKTRAINING_CH_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCLKACIOLB_CH_FNV_DDRIO_DAT7_CH_REG supported on:                      */
/*       EKV (0x401B1A20)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCLKACIOLB_CH_FNV_DDRIO_DAT7_CH_REG 0x050F4A20
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * ACIO Loopback Control Register. Multicast read for CH012 and CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    UINT32 rxvocpassgateen : 8;
    /* rxvocpassgateen - Bits[25:18], RW, default = 8'h0 
       Independent ClkRxVocSel or ClkRxVocPassGateEn for Full RX receiver
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCLKACIOLB_CH_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRDDRTTRAINRESULT_CH_FNV_DDRIO_DAT7_CH_REG supported on:                */
/*       EKV (0x401B1A24)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRDDRTTRAINRESULT_CH_FNV_DDRIO_DAT7_CH_REG 0x050F4A24
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * DDRT Training Result
 */
typedef union {
  struct {
    UINT32 evenoddsamples : 8;
    /* evenoddsamples - Bits[7:0], RW, default = 8'h0000 
       Bit 7 CKP3 early Read ID training result (Even) [br]
                            Bit 6 CKP3 early Read ID training result (Odd)[br]
                            Bit 5 CKN3 early Read ID training result (Even)[br]
                            Bit 4 CKN3 early Read ID training result (Odd)[br]
                            Bit 3 CKP1 early Read ID training result (Even)[br]
                            Bit 2 CKP1 early Read ID training result (Odd)[br]
                            Bit 1 CKN1 early Read ID training result (Even)[br]
                            Bit 0 CKN1 early Read ID training result (Odd)[br]
                            CKP3/CKN3  slot 1 read ID bits   [br]
                            CKP1/CKN1   slot 0 read ID bits[br]
                            Read ID training result (8 bits) Result of sampling on each 
       Read ID input signal, and for even and odd cycles If the even/odd results are 
       recorded in separate bitfields, no need to add logic to only capture every other 
       sample, since the data is available for the BIOS to make a decision. 
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} DDRCRDDRTTRAINRESULT_CH_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMP_SA_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B1C00)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCMDCOMP_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C00
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RWV, default = 6'b0 
       Scomp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RWV, default = 7'b0 
       Tco Comp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RWV, default = 6'b0 
       Rcomp Drv Up Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RWV, default = 6'b0 
       Rcomp Drv Dn Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RWV, default = 3'b010 
       not used
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RWV, default = 4'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMPOFFSET_SA_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1C04)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMPOFFSET_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C04
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 17;
    UINT32 ddrcmdctlrx_en : 12;
    /* ddrcmdctlrx_en - Bits[28:17], RW, default = 12'b0 
       Enable DDRT Parity in CMDCTL
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMPOFFSET_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING_SA_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1C08)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C08
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode0 : 6;
    /* cmdpicode0 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay0 : 2;
    /* cmdpilogicdelay0 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay - add 1 qclk cycle delay.
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode1 : 6;
    /* cmdpicode1 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay1 : 2;
    /* cmdpilogicdelay1 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode2 : 6;
    /* cmdpicode2 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay2 : 2;
    /* cmdpilogicdelay2 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1C0C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C0C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 ddrphasextalkenableqnnnh : 1;
    /* ddrphasextalkenableqnnnh - Bits[0:0], RW, default = 1'b0 
       When Phased based xtalk cancellation is enabled, set this to delay cmd to 
       maintain constant Tcwl. 
     */
    UINT32 visabusenable : 1;
    /* visabusenable - Bits[1:1], RW, default = 1'h0 
       Enable Visa debug bus LCB for CMD fub
     */
    UINT32 pirefclkdivenable : 1;
    /* pirefclkdivenable - Bits[2:2], RW, default = 1'h0 
       enable PiRef clock divider in xover
     */
    UINT32 xoveropmode : 1;
    /* xoveropmode - Bits[3:3], RW, default = 1'h0 
       Xover Select between grid-PiRef and PiRef-PiN
     */
    UINT32 dllmask : 2;
    /* dllmask - Bits[5:4], RW, default = 2'b0 
       On weak lock exit, controls how long the feedback to the phase detector is 
       masked: {0: 1 qclk (required for delayline latency), 1: 2 qclk, 2: 3 qclk, 3: 4 
       qclk} 
     */
    UINT32 drvpupdis : 1;
    /* drvpupdis - Bits[6:6], RW, default = 1'b0 
       This will be connected to all tx, and must toggle 1->0 after a power state
     */
    UINT32 txon : 1;
    /* txon - Bits[7:7], RW, default = 1'b0 
       Forces the CMD transmitter on, overriding any power downs
     */
    UINT32 intclkon : 1;
    /* intclkon - Bits[8:8], RW, default = 1'b0 
       Forces on all PI enables and the LCB enables in the AFE fubs.  Does not force on 
       D0 PI enable or override weaklock mode. 
     */
    UINT32 repclkon : 1;
    /* repclkon - Bits[9:9], RW, default = 1'b0 
       not used
     */
    UINT32 iolbctl : 2;
    /* iolbctl - Bits[11:10], RW, default = 2'b0 
       Controls IO Loopback.  Ctl[1] enables IOLB mode and Ctl[0] controls if this is a 
       Setup(0) or Hold(1) test 
     */
    UINT32 odtmode : 1;
    /* odtmode - Bits[12:12], RW, default = 1'b0 
       Forces the driver in ODT mode, where both the pullup and pulldown are enabled.  
       Used for Rstray testing 
     */
    UINT32 cmdtxeq : 2;
    /* cmdtxeq - Bits[14:13], RW, default = 2'b0 
       not used
     */
    UINT32 earlyweakdrive : 2;
    /* earlyweakdrive - Bits[16:15], RW, default = 2'b0 
       Controls the number of segments enabled during the early warning weak drive: {0: 
       All Enabled, 1: Enable Seg[4], 2: Enable Seg[4:3], 3: Enable Seg[4:2]} 
     */
    UINT32 lvmode : 2;
    /* lvmode - Bits[18:17], RW, default = 2'h0 
       not used
     */
    UINT32 lvmodevalid : 1;
    /* lvmodevalid - Bits[19:19], RW, default = 1'h0 
       not used
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[20:20], RW, default = 1'h0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxvref : 7;
    /* rxvref - Bits[27:21], RW, default = 7'b0 
       IOLB Vref Control.  2s compliment encoding from 1/3 Vdd to 2/3 Vdd with step 
       size of Vdd/192 (7.8mV) 
     */
    UINT32 halflsbbit : 1;
    /* halflsbbit - Bits[28:28], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefdisable : 1;
    /* vrefdisable - Bits[29:29], RW, default = 1'b0 
        Vref Disable
     */
    UINT32 ddr4modeenable : 1;
    /* ddr4modeenable - Bits[30:30], RW, default = 1'b0 
       Ddr4 Mode Enable, set to 1 in CMD2 (Func:5.Addr:C00 and Func:7.Addr:C00) fub to 
       bypass extra delay on parity bit 
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[31:31], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS2_SA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1C10)                                                     */
/* Register default value:              0x01400000                            */
#define DDRCRCMDCONTROLS2_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C10
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 20;
    UINT32 fnvddrcrdllpibypassen : 1;
    /* fnvddrcrdllpibypassen - Bits[20:20], RW, default = 1'b0 
       FNV DDR/PI Bypass Enable
     */
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[21:21], RW, default = 1'h0 
       Enable finer vref resolution in ddr4
     */
    UINT32 rsvd_22 : 1;
    UINT32 cmdrxbiassel : 1;
    /* cmdrxbiassel - Bits[23:23], RW, default = 1'b0 
       CmdRxBiasSel for Cmd Receiver
     */
    UINT32 forcecmdbiason : 1;
    /* forcecmdbiason - Bits[24:24], RW, default = 1'b1 
       ForceCmdBiason for Cmd Receiver
     */
    UINT32 fnvcmdcrxoverbypassen : 1;
    /* fnvcmdcrxoverbypassen - Bits[25:25], RW, default = 1'b0 
       bypass ddrcmdctl xover for FNV
     */
    UINT32 cmdctlecapen : 2;
    /* cmdctlecapen - Bits[27:26], RW, default = 2'b0 
       CmdCtl Buffer CtleCapEn
     */
    UINT32 cmdctleresen : 2;
    /* cmdctleresen - Bits[29:28], RW, default = 2'b0 
       CmdCtl Buffer CtleResEn
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS2_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLCOMP_SA_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B1C14)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCTLCOMP_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C14
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RWV, default = 6'b0 
       Scomp Value
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RWV, default = 7'b0 
       Tco Comp Value
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RWV, default = 6'b0 
       Rcomp Drv Up Value
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RWV, default = 6'b0 
       Rcomp Drv Dn Value
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RWV, default = 3'b010 
       LevelShift Comp Value
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RWV, default = 4'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMP_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLCOMPOFFSET_SA_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1C18)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMPOFFSET_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C18
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    UINT32 cmdctlbufsel : 6;
    /* cmdctlbufsel - Bits[23:18], RW, default = 6'b0 
       CmdBuf or CtlBuf Selection
     */
    UINT32 iodirectionvalid : 1;
    /* iodirectionvalid - Bits[24:24], RW, default = 1'b0 
       Is the Direction (RX/TX Valid)
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMPOFFSET_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS3_SA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1C1C)                                                     */
/* Register default value:              0x0000007F                            */
#define DDRCRCMDCONTROLS3_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C1C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls 3. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 bufdrvsegen : 5;
    /* bufdrvsegen - Bits[4:0], RW, default = 5'b11111 
       Drive strength enable for CmdBuf/CtlBuf.
     */
    UINT32 cmdvrefpgen : 1;
    /* cmdvrefpgen - Bits[5:5], RW, default = 1'b1 
       control the DdrVrefSelDQQnnnH pin on the ddrvrefint generator
     */
    UINT32 odtstrength : 1;
    /* odtstrength - Bits[6:6], RW, default = 1'b1 
       OdtStrength
     */
    UINT32 rsvd_7 : 2;
    UINT32 divby2alignctl : 1;
    /* divby2alignctl - Bits[9:9], RW, default = 1'h0 
       Invert DAlign
     */
    UINT32 divby2disable : 1;
    /* divby2disable - Bits[10:10], RW, default = 1'h0 
       select qclk for cmdxover instead of Dclk
     */
    UINT32 odten : 12;
    /* odten - Bits[22:11], RW, default = 12'h0 
       ODTDis for Cmd and Ctl buf
     */
    UINT32 cmdrefpiclkdelay : 6;
    /* cmdrefpiclkdelay - Bits[28:23], RW, default = 6'h0 
       CmdRefClk
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS3_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLRANKSUSED_SA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1C20)                                                     */
/* Register default value:              0x0000003F                            */
#define DDRCRCTLRANKSUSED_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C20
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Ranks Used. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ranken : 6;
    /* ranken - Bits[5:0], RW, default = 6'b111111 
       Enables output buffers for ctlbufpair 0 to ctlbufpair6 , PI clocks and output 
       datapaths for this rank 
     */
    UINT32 rsvd_6 : 6;
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[12:12], RW, default = 1'b0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[14:13], RW, default = 2'b00 
       used to compensate the GCN skew. The GCN skew information should be[br]
                 extracted by reading and analyzing the final Rcomp code value. The 
       target spec[br] 
                 is to categorize the GCN skew in 3 process condition (mainly slow, 
       fast, typical).[br] 
                 (one example can be just reading the 2 MLS bit of Rcomp code and 
       determine if the[br] 
                 GCN is slow, fast or typical). The gcncomp is used for CTLE.[br]
                 11: gcn in slow corner[br]
                 10: gcn in typical corner[br]
                 01: gcn in fast corner
     */
    UINT32 rsvd_15 : 1;
    UINT32 imodebiashighcm : 1;
    /* imodebiashighcm - Bits[16:16], RW, default = 1'b0 
       Imode bias control
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[17:17], RW, default = 1'b0 
       Rx bias Sel
     */
    UINT32 cmdrxeqqnnnl : 3;
    /* cmdrxeqqnnnl - Bits[20:18], RW, default = 3'b0 
       CmdRxEqQnnnL for RX CmdCtl Buf
     */
    UINT32 rsvd_21 : 3;
    UINT32 ddrcmdrxbiasctl : 3;
    /* ddrcmdrxbiasctl - Bits[26:24], RW, default = 3'b0 
       CmdRxBiasCtl for RX biasgen
     */
    UINT32 cmdpcasbiasclosedloopen : 1;
    /* cmdpcasbiasclosedloopen - Bits[27:27], RW, default = 1'b0 
       CmdPcasBiasClosedLoopEnQnnnH for RX biasgen
     */
    UINT32 ddrcmdrxbiasfoldedlegtrim : 2;
    /* ddrcmdrxbiasfoldedlegtrim - Bits[29:28], RW, default = 2'b0 
       DdrCmdRxBiasFoldedLegTrim for RX biasgen
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCTLRANKSUSED_SA_FNV_DDRIO_DAT7_CH_STRUCT;






/* DDRCRCMDTRAINING_SA_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1C2C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDTRAINING_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C2C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Cmd Training Register. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 trainingoffset : 6;
    /* trainingoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained PiCode Value.
     */
    UINT32 spare0 : 2;
    /* spare0 - Bits[7:6], ROV, default = 2'h0 
       Spare
     */
    UINT32 xoverphasedet : 14;
    /* xoverphasedet - Bits[21:8], ROV, default = 14'b0 
       bits13:6 unused[br]
                            bit5 output of phase detector for I/O Pair 0[br]
                            bit4 output of phase detector for I/O Pair 1[br]
                            bit3 output of phase detector for I/O Pair 2[br]
                            bit2 output of phase detector for I/O Pair 3[br]
                            bit1 output of phase detector for I/O Pair 4[br]
                            bit0 output of phase detector for I/O Pair 5 (only on 
       cmdctla FUBs) 
     */
    UINT32 trainingoffset2 : 6;
    /* trainingoffset2 - Bits[27:22], RW, default = 6'b0 
       Offset added to PiCode. Obtained via xover calibration such that ClkPi aligns 
       with grid clock when PiCode=0. 
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DDRCRCMDTRAINING_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS1_SA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1C30)                                                     */
/* Register default value:              0x03C01000                            */
#define DDRCRCMDCONTROLS1_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C30
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */
    UINT32 rsvd_13 : 9;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 5'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
       [br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled [br] 
                   for deemphasized, and so on.
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING2_SA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1C34)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING2_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C34
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode3 : 6;
    /* cmdpicode3 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay3 : 2;
    /* cmdpilogicdelay3 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode4 : 6;
    /* cmdpicode4 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay4 : 2;
    /* cmdpilogicdelay4 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode5 : 6;
    /* cmdpicode5 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay5 : 2;
    /* cmdpilogicdelay5 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING2_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING3_SA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1C38)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING3_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C38
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode6 : 6;
    /* cmdpicode6 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay6 : 2;
    /* cmdpilogicdelay6 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING3_SA_FNV_DDRIO_DAT7_CH_STRUCT;




/* VSSHIORVREFCONTROL_SA_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1C40)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C40
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings.[br] 
       [br]
               VssHi Field Description:[br]
       [br]
                Field          Bits    Description[br]
               -------         ----    
       ---------------------------------------------------------------------------------------------------------------------------------------------------------------[br] 
               SelCode          23     Selects the code to output:  {0: Code, 1: 
       TCode}[br] 
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode[br] 
               PanicVoltage     21:18  Unsigned integer controlling the voltage error 
       on VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV)[br] 
               CloseLoop        17     Enables the VssHi close loop tracking[br]
               PanicEn          16     Enables the VssHi Panic Driver[br]
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError)[br] 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved[br] 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128}[br] 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMP1_SA_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B1C44)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMP1_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C44
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Cmd Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 5;
    UINT32 panicdrvdn : 10;
    /* panicdrvdn - Bits[14:5], RW, default = 10'b0000000000 
       Panic DrvDn RComp Value.
     */
    UINT32 panicdrvup : 10;
    /* panicdrvup - Bits[24:15], RW, default = 10'b0000000000 
       Panic DrvUp RComp Comp Value.
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP1_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* VSSHIORVREFCONTROL1_SA_FNV_DDRIO_DAT7_CH_REG supported on:                 */
/*       EKV (0x401B1C48)                                                     */
/* Register default value:              0x00000000                            */
#define VSSHIORVREFCONTROL1_SA_FNV_DDRIO_DAT7_CH_REG 0x050F4C48
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Extension for BDX. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 8;
    /* vsshiorvrefctl - Bits[7:0], RW, default = 8'b0 
        BDX-specific extended control signal.
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL1_SA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMP_NA_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B1D00)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCMDCOMP_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D00
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RWV, default = 6'b0 
       Scomp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RWV, default = 7'b0 
       Tco Comp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RWV, default = 6'b0 
       Rcomp Drv Up Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RWV, default = 6'b0 
       Rcomp Drv Dn Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RWV, default = 3'b010 
       not used
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RWV, default = 4'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMPOFFSET_NA_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1D04)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMPOFFSET_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D04
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 17;
    UINT32 ddrcmdctlrx_en : 12;
    /* ddrcmdctlrx_en - Bits[28:17], RW, default = 12'b0 
       Enable DDRT Parity in CMDCTL
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMPOFFSET_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING_NA_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1D08)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D08
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode0 : 6;
    /* cmdpicode0 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay0 : 2;
    /* cmdpilogicdelay0 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay - add 1 qclk cycle delay.
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode1 : 6;
    /* cmdpicode1 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay1 : 2;
    /* cmdpilogicdelay1 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode2 : 6;
    /* cmdpicode2 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay2 : 2;
    /* cmdpilogicdelay2 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS_NA_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1D0C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCONTROLS_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D0C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 ddrphasextalkenableqnnnh : 1;
    /* ddrphasextalkenableqnnnh - Bits[0:0], RW, default = 1'b0 
       When Phased based xtalk cancellation is enabled, set this to delay cmd to 
       maintain constant Tcwl. 
     */
    UINT32 visabusenable : 1;
    /* visabusenable - Bits[1:1], RW, default = 1'h0 
       Enable Visa debug bus LCB for CMD fub
     */
    UINT32 pirefclkdivenable : 1;
    /* pirefclkdivenable - Bits[2:2], RW, default = 1'h0 
       enable PiRef clock divider in xover
     */
    UINT32 xoveropmode : 1;
    /* xoveropmode - Bits[3:3], RW, default = 1'h0 
       Xover Select between grid-PiRef and PiRef-PiN
     */
    UINT32 dllmask : 2;
    /* dllmask - Bits[5:4], RW, default = 2'b0 
       On weak lock exit, controls how long the feedback to the phase detector is 
       masked: {0: 1 qclk (required for delayline latency), 1: 2 qclk, 2: 3 qclk, 3: 4 
       qclk} 
     */
    UINT32 drvpupdis : 1;
    /* drvpupdis - Bits[6:6], RW, default = 1'b0 
       This will be connected to all tx, and must toggle 1->0 after a power state
     */
    UINT32 txon : 1;
    /* txon - Bits[7:7], RW, default = 1'b0 
       Forces the CMD transmitter on, overriding any power downs
     */
    UINT32 intclkon : 1;
    /* intclkon - Bits[8:8], RW, default = 1'b0 
       Forces on all PI enables and the LCB enables in the AFE fubs.  Does not force on 
       D0 PI enable or override weaklock mode. 
     */
    UINT32 repclkon : 1;
    /* repclkon - Bits[9:9], RW, default = 1'b0 
       not used
     */
    UINT32 iolbctl : 2;
    /* iolbctl - Bits[11:10], RW, default = 2'b0 
       Controls IO Loopback.  Ctl[1] enables IOLB mode and Ctl[0] controls if this is a 
       Setup(0) or Hold(1) test 
     */
    UINT32 odtmode : 1;
    /* odtmode - Bits[12:12], RW, default = 1'b0 
       Forces the driver in ODT mode, where both the pullup and pulldown are enabled.  
       Used for Rstray testing 
     */
    UINT32 cmdtxeq : 2;
    /* cmdtxeq - Bits[14:13], RW, default = 2'b0 
       not used
     */
    UINT32 earlyweakdrive : 2;
    /* earlyweakdrive - Bits[16:15], RW, default = 2'b0 
       Controls the number of segments enabled during the early warning weak drive: {0: 
       All Enabled, 1: Enable Seg[4], 2: Enable Seg[4:3], 3: Enable Seg[4:2]} 
     */
    UINT32 lvmode : 2;
    /* lvmode - Bits[18:17], RW, default = 2'h0 
       not used
     */
    UINT32 lvmodevalid : 1;
    /* lvmodevalid - Bits[19:19], RW, default = 1'h0 
       not used
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[20:20], RW, default = 1'h0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxvref : 7;
    /* rxvref - Bits[27:21], RW, default = 7'b0 
       IOLB Vref Control.  2s compliment encoding from 1/3 Vdd to 2/3 Vdd with step 
       size of Vdd/192 (7.8mV) 
     */
    UINT32 halflsbbit : 1;
    /* halflsbbit - Bits[28:28], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefdisable : 1;
    /* vrefdisable - Bits[29:29], RW, default = 1'b0 
        Vref Disable
     */
    UINT32 ddr4modeenable : 1;
    /* ddr4modeenable - Bits[30:30], RW, default = 1'b0 
       Ddr4 Mode Enable, set to 1 in CMD2 (Func:5.Addr:C00 and Func:7.Addr:C00) fub to 
       bypass extra delay on parity bit 
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[31:31], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS2_NA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1D10)                                                     */
/* Register default value:              0x01400000                            */
#define DDRCRCMDCONTROLS2_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D10
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 21;
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[21:21], RW, default = 1'h0 
       Enable finer vref resolution in ddr4
     */
    UINT32 rsvd_22 : 1;
    UINT32 cmdrxbiassel : 1;
    /* cmdrxbiassel - Bits[23:23], RW, default = 1'b0 
       CmdRxBiasSel for Cmd Receiver
     */
    UINT32 forcecmdbiason : 1;
    /* forcecmdbiason - Bits[24:24], RW, default = 1'b1 
       ForceCmdBiason for Cmd Receiver
     */
    UINT32 rsvd_25 : 1;
    UINT32 cmdctlecapen : 2;
    /* cmdctlecapen - Bits[27:26], RW, default = 2'b0 
       CmdCtl Buffer CtleCapEn
     */
    UINT32 cmdctleresen : 2;
    /* cmdctleresen - Bits[29:28], RW, default = 2'b0 
       CmdCtl Buffer CtleResEn
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS2_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLCOMP_NA_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B1D14)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCTLCOMP_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D14
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RWV, default = 6'b0 
       Scomp Value
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RWV, default = 7'b0 
       Tco Comp Value
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RWV, default = 6'b0 
       Rcomp Drv Up Value
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RWV, default = 6'b0 
       Rcomp Drv Dn Value
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RWV, default = 3'b010 
       LevelShift Comp Value
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RWV, default = 4'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMP_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLCOMPOFFSET_NA_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1D18)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMPOFFSET_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D18
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    UINT32 cmdctlbufsel : 6;
    /* cmdctlbufsel - Bits[23:18], RW, default = 6'b0 
       CmdBuf or CtlBuf Selection
     */
    UINT32 iodirectionvalid : 1;
    /* iodirectionvalid - Bits[24:24], RW, default = 1'b0 
       Is the Direction (RX/TX Valid)
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMPOFFSET_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS3_NA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1D1C)                                                     */
/* Register default value:              0x0000007F                            */
#define DDRCRCMDCONTROLS3_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D1C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls 3. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 bufdrvsegen : 5;
    /* bufdrvsegen - Bits[4:0], RW, default = 5'b11111 
       Drive strength enable for CmdBuf/CtlBuf.
     */
    UINT32 cmdvrefpgen : 1;
    /* cmdvrefpgen - Bits[5:5], RW, default = 1'b1 
       control the DdrVrefSelDQQnnnH pin on the ddrvrefint generator
     */
    UINT32 odtstrength : 1;
    /* odtstrength - Bits[6:6], RW, default = 1'b1 
       OdtStrength
     */
    UINT32 rsvd_7 : 2;
    UINT32 divby2alignctl : 1;
    /* divby2alignctl - Bits[9:9], RW, default = 1'h0 
       Invert DAlign
     */
    UINT32 divby2disable : 1;
    /* divby2disable - Bits[10:10], RW, default = 1'h0 
       select qclk for cmdxover instead of Dclk
     */
    UINT32 odten : 12;
    /* odten - Bits[22:11], RW, default = 12'h0 
       ODTDis for Cmd and Ctl buf
     */
    UINT32 cmdrefpiclkdelay : 6;
    /* cmdrefpiclkdelay - Bits[28:23], RW, default = 6'h0 
       CmdRefClk
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS3_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLRANKSUSED_NA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1D20)                                                     */
/* Register default value:              0x0000003F                            */
#define DDRCRCTLRANKSUSED_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D20
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Ranks Used. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ranken : 6;
    /* ranken - Bits[5:0], RW, default = 6'b111111 
       Enables output buffers for ctlbufpair 0 to ctlbufpair6 , PI clocks and output 
       datapaths for this rank 
     */
    UINT32 rsvd_6 : 6;
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[12:12], RW, default = 1'b0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[14:13], RW, default = 2'b00 
       used to compensate the GCN skew. The GCN skew information should be[br]
                 extracted by reading and analyzing the final Rcomp code value. The 
       target spec[br] 
                 is to categorize the GCN skew in 3 process condition (mainly slow, 
       fast, typical).[br] 
                 (one example can be just reading the 2 MLS bit of Rcomp code and 
       determine if the[br] 
                 GCN is slow, fast or typical). The gcncomp is used for CTLE.[br]
                 11: gcn in slow corner[br]
                 10: gcn in typical corner[br]
                 01: gcn in fast corner
     */
    UINT32 rsvd_15 : 1;
    UINT32 imodebiashighcm : 1;
    /* imodebiashighcm - Bits[16:16], RW, default = 1'b0 
       Imode bias control
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[17:17], RW, default = 1'b0 
       Rx bias Sel
     */
    UINT32 cmdrxeqqnnnl : 3;
    /* cmdrxeqqnnnl - Bits[20:18], RW, default = 3'b0 
       CmdRxEqQnnnL for RX CmdCtl Buf
     */
    UINT32 rsvd_21 : 3;
    UINT32 ddrcmdrxbiasctl : 3;
    /* ddrcmdrxbiasctl - Bits[26:24], RW, default = 3'b0 
       CmdRxBiasCtl for RX biasgen
     */
    UINT32 cmdpcasbiasclosedloopen : 1;
    /* cmdpcasbiasclosedloopen - Bits[27:27], RW, default = 1'b0 
       CmdPcasBiasClosedLoopEnQnnnH for RX biasgen
     */
    UINT32 ddrcmdrxbiasfoldedlegtrim : 2;
    /* ddrcmdrxbiasfoldedlegtrim - Bits[29:28], RW, default = 2'b0 
       DdrCmdRxBiasFoldedLegTrim for RX biasgen
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCTLRANKSUSED_NA_FNV_DDRIO_DAT7_CH_STRUCT;






/* DDRCRCMDTRAINING_NA_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1D2C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDTRAINING_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D2C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Cmd Training Register. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 trainingoffset : 6;
    /* trainingoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained PiCode Value.
     */
    UINT32 spare0 : 2;
    /* spare0 - Bits[7:6], ROV, default = 2'h0 
       Spare
     */
    UINT32 xoverphasedet : 14;
    /* xoverphasedet - Bits[21:8], ROV, default = 14'b0 
       bits13:6 unused[br]
                            bit5 output of phase detector for I/O Pair 0[br]
                            bit4 output of phase detector for I/O Pair 1[br]
                            bit3 output of phase detector for I/O Pair 2[br]
                            bit2 output of phase detector for I/O Pair 3[br]
                            bit1 output of phase detector for I/O Pair 4[br]
                            bit0 output of phase detector for I/O Pair 5 (only on 
       cmdctla FUBs) 
     */
    UINT32 trainingoffset2 : 6;
    /* trainingoffset2 - Bits[27:22], RW, default = 6'b0 
       Offset added to PiCode. Obtained via xover calibration such that ClkPi aligns 
       with grid clock when PiCode=0. 
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DDRCRCMDTRAINING_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS1_NA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1D30)                                                     */
/* Register default value:              0x03C01000                            */
#define DDRCRCMDCONTROLS1_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D30
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */
    UINT32 rsvd_13 : 9;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 5'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
       [br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled [br] 
                   for deemphasized, and so on.
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING2_NA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1D34)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING2_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D34
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode3 : 6;
    /* cmdpicode3 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay3 : 2;
    /* cmdpilogicdelay3 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode4 : 6;
    /* cmdpicode4 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay4 : 2;
    /* cmdpilogicdelay4 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode5 : 6;
    /* cmdpicode5 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay5 : 2;
    /* cmdpilogicdelay5 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING2_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING3_NA_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1D38)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING3_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D38
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode6 : 6;
    /* cmdpicode6 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay6 : 2;
    /* cmdpilogicdelay6 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING3_NA_FNV_DDRIO_DAT7_CH_STRUCT;




/* VSSHIORVREFCONTROL_NA_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1D40)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D40
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings.[br] 
       [br]
               VssHi Field Description:[br]
       [br]
                Field          Bits    Description[br]
               -------         ----    
       ---------------------------------------------------------------------------------------------------------------------------------------------------------------[br] 
               SelCode          23     Selects the code to output:  {0: Code, 1: 
       TCode}[br] 
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode[br] 
               PanicVoltage     21:18  Unsigned integer controlling the voltage error 
       on VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV)[br] 
               CloseLoop        17     Enables the VssHi close loop tracking[br]
               PanicEn          16     Enables the VssHi Panic Driver[br]
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError)[br] 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved[br] 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128}[br] 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMP1_NA_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B1D44)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMP1_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D44
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Cmd Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 5;
    UINT32 panicdrvdn : 10;
    /* panicdrvdn - Bits[14:5], RW, default = 10'b0000000000 
       Panic DrvDn RComp Value.
     */
    UINT32 panicdrvup : 10;
    /* panicdrvup - Bits[24:15], RW, default = 10'b0000000000 
       Panic DrvUp RComp Comp Value.
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP1_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* VSSHIORVREFCONTROL1_NA_FNV_DDRIO_DAT7_CH_REG supported on:                 */
/*       EKV (0x401B1D48)                                                     */
/* Register default value:              0x00000000                            */
#define VSSHIORVREFCONTROL1_NA_FNV_DDRIO_DAT7_CH_REG 0x050F4D48
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Extension for BDX. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 8;
    /* vsshiorvrefctl - Bits[7:0], RW, default = 8'b0 
        BDX-specific extended control signal.
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL1_NA_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMP_SB_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B1E00)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCMDCOMP_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E00
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RWV, default = 6'b0 
       Scomp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RWV, default = 7'b0 
       Tco Comp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RWV, default = 6'b0 
       Rcomp Drv Up Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RWV, default = 6'b0 
       Rcomp Drv Dn Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RWV, default = 3'b010 
       not used
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RWV, default = 4'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMPOFFSET_SB_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1E04)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMPOFFSET_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E04
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 17;
    UINT32 ddrcmdctlrx_en : 12;
    /* ddrcmdctlrx_en - Bits[28:17], RW, default = 12'b0 
       Enable DDRT Parity in CMDCTL
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMPOFFSET_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING_SB_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1E08)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E08
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode0 : 6;
    /* cmdpicode0 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay0 : 2;
    /* cmdpilogicdelay0 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay - add 1 qclk cycle delay.
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode1 : 6;
    /* cmdpicode1 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay1 : 2;
    /* cmdpilogicdelay1 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode2 : 6;
    /* cmdpicode2 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay2 : 2;
    /* cmdpilogicdelay2 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS_SB_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1E0C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCONTROLS_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E0C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 ddrphasextalkenableqnnnh : 1;
    /* ddrphasextalkenableqnnnh - Bits[0:0], RW, default = 1'b0 
       When Phased based xtalk cancellation is enabled, set this to delay cmd to 
       maintain constant Tcwl. 
     */
    UINT32 visabusenable : 1;
    /* visabusenable - Bits[1:1], RW, default = 1'h0 
       Enable Visa debug bus LCB for CMD fub
     */
    UINT32 pirefclkdivenable : 1;
    /* pirefclkdivenable - Bits[2:2], RW, default = 1'h0 
       enable PiRef clock divider in xover
     */
    UINT32 xoveropmode : 1;
    /* xoveropmode - Bits[3:3], RW, default = 1'h0 
       Xover Select between grid-PiRef and PiRef-PiN
     */
    UINT32 dllmask : 2;
    /* dllmask - Bits[5:4], RW, default = 2'b0 
       On weak lock exit, controls how long the feedback to the phase detector is 
       masked: {0: 1 qclk (required for delayline latency), 1: 2 qclk, 2: 3 qclk, 3: 4 
       qclk} 
     */
    UINT32 drvpupdis : 1;
    /* drvpupdis - Bits[6:6], RW, default = 1'b0 
       This will be connected to all tx, and must toggle 1->0 after a power state
     */
    UINT32 txon : 1;
    /* txon - Bits[7:7], RW, default = 1'b0 
       Forces the CMD transmitter on, overriding any power downs
     */
    UINT32 intclkon : 1;
    /* intclkon - Bits[8:8], RW, default = 1'b0 
       Forces on all PI enables and the LCB enables in the AFE fubs.  Does not force on 
       D0 PI enable or override weaklock mode. 
     */
    UINT32 repclkon : 1;
    /* repclkon - Bits[9:9], RW, default = 1'b0 
       not used
     */
    UINT32 iolbctl : 2;
    /* iolbctl - Bits[11:10], RW, default = 2'b0 
       Controls IO Loopback.  Ctl[1] enables IOLB mode and Ctl[0] controls if this is a 
       Setup(0) or Hold(1) test 
     */
    UINT32 odtmode : 1;
    /* odtmode - Bits[12:12], RW, default = 1'b0 
       Forces the driver in ODT mode, where both the pullup and pulldown are enabled.  
       Used for Rstray testing 
     */
    UINT32 cmdtxeq : 2;
    /* cmdtxeq - Bits[14:13], RW, default = 2'b0 
       not used
     */
    UINT32 earlyweakdrive : 2;
    /* earlyweakdrive - Bits[16:15], RW, default = 2'b0 
       Controls the number of segments enabled during the early warning weak drive: {0: 
       All Enabled, 1: Enable Seg[4], 2: Enable Seg[4:3], 3: Enable Seg[4:2]} 
     */
    UINT32 lvmode : 2;
    /* lvmode - Bits[18:17], RW, default = 2'h0 
       not used
     */
    UINT32 lvmodevalid : 1;
    /* lvmodevalid - Bits[19:19], RW, default = 1'h0 
       not used
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[20:20], RW, default = 1'h0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxvref : 7;
    /* rxvref - Bits[27:21], RW, default = 7'b0 
       IOLB Vref Control.  2s compliment encoding from 1/3 Vdd to 2/3 Vdd with step 
       size of Vdd/192 (7.8mV) 
     */
    UINT32 halflsbbit : 1;
    /* halflsbbit - Bits[28:28], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefdisable : 1;
    /* vrefdisable - Bits[29:29], RW, default = 1'b0 
        Vref Disable
     */
    UINT32 ddr4modeenable : 1;
    /* ddr4modeenable - Bits[30:30], RW, default = 1'b0 
       Ddr4 Mode Enable, set to 1 in CMD2 (Func:5.Addr:C00 and Func:7.Addr:C00) fub to 
       bypass extra delay on parity bit 
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[31:31], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS2_SB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1E10)                                                     */
/* Register default value:              0x01400000                            */
#define DDRCRCMDCONTROLS2_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E10
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 21;
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[21:21], RW, default = 1'h0 
       Enable finer vref resolution in ddr4
     */
    UINT32 rsvd_22 : 1;
    UINT32 cmdrxbiassel : 1;
    /* cmdrxbiassel - Bits[23:23], RW, default = 1'b0 
       CmdRxBiasSel for Cmd Receiver
     */
    UINT32 forcecmdbiason : 1;
    /* forcecmdbiason - Bits[24:24], RW, default = 1'b1 
       ForceCmdBiason for Cmd Receiver
     */
    UINT32 rsvd_25 : 1;
    UINT32 cmdctlecapen : 2;
    /* cmdctlecapen - Bits[27:26], RW, default = 2'b0 
       CmdCtl Buffer CtleCapEn
     */
    UINT32 cmdctleresen : 2;
    /* cmdctleresen - Bits[29:28], RW, default = 2'b0 
       CmdCtl Buffer CtleResEn
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS2_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLCOMP_SB_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B1E14)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCTLCOMP_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E14
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RWV, default = 6'b0 
       Scomp Value
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RWV, default = 7'b0 
       Tco Comp Value
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RWV, default = 6'b0 
       Rcomp Drv Up Value
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RWV, default = 6'b0 
       Rcomp Drv Dn Value
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RWV, default = 3'b010 
       LevelShift Comp Value
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RWV, default = 4'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMP_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLCOMPOFFSET_SB_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1E18)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMPOFFSET_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E18
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    UINT32 cmdctlbufsel : 6;
    /* cmdctlbufsel - Bits[23:18], RW, default = 6'b0 
       CmdBuf or CtlBuf Selection
     */
    UINT32 iodirectionvalid : 1;
    /* iodirectionvalid - Bits[24:24], RW, default = 1'b0 
       Is the Direction (RX/TX Valid)
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMPOFFSET_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS3_SB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1E1C)                                                     */
/* Register default value:              0x0000007F                            */
#define DDRCRCMDCONTROLS3_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E1C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls 3. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 bufdrvsegen : 5;
    /* bufdrvsegen - Bits[4:0], RW, default = 5'b11111 
       Drive strength enable for CmdBuf/CtlBuf.
     */
    UINT32 cmdvrefpgen : 1;
    /* cmdvrefpgen - Bits[5:5], RW, default = 1'b1 
       control the DdrVrefSelDQQnnnH pin on the ddrvrefint generator
     */
    UINT32 odtstrength : 1;
    /* odtstrength - Bits[6:6], RW, default = 1'b1 
       OdtStrength
     */
    UINT32 rsvd_7 : 2;
    UINT32 divby2alignctl : 1;
    /* divby2alignctl - Bits[9:9], RW, default = 1'h0 
       Invert DAlign
     */
    UINT32 divby2disable : 1;
    /* divby2disable - Bits[10:10], RW, default = 1'h0 
       select qclk for cmdxover instead of Dclk
     */
    UINT32 odten : 12;
    /* odten - Bits[22:11], RW, default = 12'h0 
       ODTDis for Cmd and Ctl buf
     */
    UINT32 cmdrefpiclkdelay : 6;
    /* cmdrefpiclkdelay - Bits[28:23], RW, default = 6'h0 
       CmdRefClk
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS3_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLRANKSUSED_SB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1E20)                                                     */
/* Register default value:              0x0000003F                            */
#define DDRCRCTLRANKSUSED_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E20
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Ranks Used. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ranken : 6;
    /* ranken - Bits[5:0], RW, default = 6'b111111 
       Enables output buffers for ctlbufpair 0 to ctlbufpair6 , PI clocks and output 
       datapaths for this rank 
     */
    UINT32 rsvd_6 : 6;
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[12:12], RW, default = 1'b0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[14:13], RW, default = 2'b00 
       used to compensate the GCN skew. The GCN skew information should be[br]
                 extracted by reading and analyzing the final Rcomp code value. The 
       target spec[br] 
                 is to categorize the GCN skew in 3 process condition (mainly slow, 
       fast, typical).[br] 
                 (one example can be just reading the 2 MLS bit of Rcomp code and 
       determine if the[br] 
                 GCN is slow, fast or typical). The gcncomp is used for CTLE.[br]
                 11: gcn in slow corner[br]
                 10: gcn in typical corner[br]
                 01: gcn in fast corner
     */
    UINT32 rsvd_15 : 1;
    UINT32 imodebiashighcm : 1;
    /* imodebiashighcm - Bits[16:16], RW, default = 1'b0 
       Imode bias control
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[17:17], RW, default = 1'b0 
       Rx bias Sel
     */
    UINT32 cmdrxeqqnnnl : 3;
    /* cmdrxeqqnnnl - Bits[20:18], RW, default = 3'b0 
       CmdRxEqQnnnL for RX CmdCtl Buf
     */
    UINT32 rsvd_21 : 3;
    UINT32 ddrcmdrxbiasctl : 3;
    /* ddrcmdrxbiasctl - Bits[26:24], RW, default = 3'b0 
       CmdRxBiasCtl for RX biasgen
     */
    UINT32 cmdpcasbiasclosedloopen : 1;
    /* cmdpcasbiasclosedloopen - Bits[27:27], RW, default = 1'b0 
       CmdPcasBiasClosedLoopEnQnnnH for RX biasgen
     */
    UINT32 ddrcmdrxbiasfoldedlegtrim : 2;
    /* ddrcmdrxbiasfoldedlegtrim - Bits[29:28], RW, default = 2'b0 
       DdrCmdRxBiasFoldedLegTrim for RX biasgen
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCTLRANKSUSED_SB_FNV_DDRIO_DAT7_CH_STRUCT;






/* DDRCRCMDTRAINING_SB_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1E2C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDTRAINING_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E2C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Cmd Training Register. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 trainingoffset : 6;
    /* trainingoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained PiCode Value.
     */
    UINT32 spare0 : 2;
    /* spare0 - Bits[7:6], ROV, default = 2'h0 
       Spare
     */
    UINT32 xoverphasedet : 14;
    /* xoverphasedet - Bits[21:8], ROV, default = 14'b0 
       bits13:6 unused[br]
                            bit5 output of phase detector for I/O Pair 0[br]
                            bit4 output of phase detector for I/O Pair 1[br]
                            bit3 output of phase detector for I/O Pair 2[br]
                            bit2 output of phase detector for I/O Pair 3[br]
                            bit1 output of phase detector for I/O Pair 4[br]
                            bit0 output of phase detector for I/O Pair 5 (only on 
       cmdctla FUBs) 
     */
    UINT32 trainingoffset2 : 6;
    /* trainingoffset2 - Bits[27:22], RW, default = 6'b0 
       Offset added to PiCode. Obtained via xover calibration such that ClkPi aligns 
       with grid clock when PiCode=0. 
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DDRCRCMDTRAINING_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS1_SB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1E30)                                                     */
/* Register default value:              0x03C01000                            */
#define DDRCRCMDCONTROLS1_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E30
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */
    UINT32 rsvd_13 : 9;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 5'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
       [br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled [br] 
                   for deemphasized, and so on.
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING2_SB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1E34)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING2_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E34
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode3 : 6;
    /* cmdpicode3 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay3 : 2;
    /* cmdpilogicdelay3 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode4 : 6;
    /* cmdpicode4 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay4 : 2;
    /* cmdpilogicdelay4 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode5 : 6;
    /* cmdpicode5 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay5 : 2;
    /* cmdpilogicdelay5 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING2_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING3_SB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1E38)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING3_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E38
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode6 : 6;
    /* cmdpicode6 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay6 : 2;
    /* cmdpilogicdelay6 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING3_SB_FNV_DDRIO_DAT7_CH_STRUCT;




/* VSSHIORVREFCONTROL_SB_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1E40)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E40
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings.[br] 
       [br]
               VssHi Field Description:[br]
       [br]
                Field          Bits    Description[br]
               -------         ----    
       ---------------------------------------------------------------------------------------------------------------------------------------------------------------[br] 
               SelCode          23     Selects the code to output:  {0: Code, 1: 
       TCode}[br] 
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode[br] 
               PanicVoltage     21:18  Unsigned integer controlling the voltage error 
       on VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV)[br] 
               CloseLoop        17     Enables the VssHi close loop tracking[br]
               PanicEn          16     Enables the VssHi Panic Driver[br]
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError)[br] 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved[br] 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128}[br] 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMP1_SB_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B1E44)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMP1_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E44
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Cmd Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 5;
    UINT32 panicdrvdn : 10;
    /* panicdrvdn - Bits[14:5], RW, default = 10'b0000000000 
       Panic DrvDn RComp Value.
     */
    UINT32 panicdrvup : 10;
    /* panicdrvup - Bits[24:15], RW, default = 10'b0000000000 
       Panic DrvUp RComp Comp Value.
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP1_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* VSSHIORVREFCONTROL1_SB_FNV_DDRIO_DAT7_CH_REG supported on:                 */
/*       EKV (0x401B1E48)                                                     */
/* Register default value:              0x00000000                            */
#define VSSHIORVREFCONTROL1_SB_FNV_DDRIO_DAT7_CH_REG 0x050F4E48
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Extension for BDX. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 8;
    /* vsshiorvrefctl - Bits[7:0], RW, default = 8'b0 
        BDX-specific extended control signal.
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL1_SB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMP_NB_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B1F00)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCMDCOMP_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F00
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RWV, default = 6'b0 
       Scomp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RWV, default = 7'b0 
       Tco Comp Value, Normally updated by Comp FSM via CompBus. can be overriden via 
       message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RWV, default = 6'b0 
       Rcomp Drv Up Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RWV, default = 6'b0 
       Rcomp Drv Dn Value, Normally updated by Comp FSM via CompBus. can be overriden 
       via message channel if periodical retrain is disabled. 
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RWV, default = 3'b010 
       not used
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RWV, default = 4'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMPOFFSET_NB_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1F04)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMPOFFSET_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F04
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 17;
    UINT32 ddrcmdctlrx_en : 12;
    /* ddrcmdctlrx_en - Bits[28:17], RW, default = 12'b0 
       Enable DDRT Parity in CMDCTL
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMPOFFSET_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING_NB_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1F08)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F08
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode0 : 6;
    /* cmdpicode0 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay0 : 2;
    /* cmdpilogicdelay0 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay - add 1 qclk cycle delay.
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode1 : 6;
    /* cmdpicode1 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay1 : 2;
    /* cmdpilogicdelay1 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode2 : 6;
    /* cmdpicode2 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay2 : 2;
    /* cmdpilogicdelay2 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS_NB_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1F0C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCONTROLS_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F0C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 ddrphasextalkenableqnnnh : 1;
    /* ddrphasextalkenableqnnnh - Bits[0:0], RW, default = 1'b0 
       When Phased based xtalk cancellation is enabled, set this to delay cmd to 
       maintain constant Tcwl. 
     */
    UINT32 visabusenable : 1;
    /* visabusenable - Bits[1:1], RW, default = 1'h0 
       Enable Visa debug bus LCB for CMD fub
     */
    UINT32 pirefclkdivenable : 1;
    /* pirefclkdivenable - Bits[2:2], RW, default = 1'h0 
       enable PiRef clock divider in xover
     */
    UINT32 xoveropmode : 1;
    /* xoveropmode - Bits[3:3], RW, default = 1'h0 
       Xover Select between grid-PiRef and PiRef-PiN
     */
    UINT32 dllmask : 2;
    /* dllmask - Bits[5:4], RW, default = 2'b0 
       On weak lock exit, controls how long the feedback to the phase detector is 
       masked: {0: 1 qclk (required for delayline latency), 1: 2 qclk, 2: 3 qclk, 3: 4 
       qclk} 
     */
    UINT32 drvpupdis : 1;
    /* drvpupdis - Bits[6:6], RW, default = 1'b0 
       This will be connected to all tx, and must toggle 1->0 after a power state
     */
    UINT32 txon : 1;
    /* txon - Bits[7:7], RW, default = 1'b0 
       Forces the CMD transmitter on, overriding any power downs
     */
    UINT32 intclkon : 1;
    /* intclkon - Bits[8:8], RW, default = 1'b0 
       Forces on all PI enables and the LCB enables in the AFE fubs.  Does not force on 
       D0 PI enable or override weaklock mode. 
     */
    UINT32 repclkon : 1;
    /* repclkon - Bits[9:9], RW, default = 1'b0 
       not used
     */
    UINT32 iolbctl : 2;
    /* iolbctl - Bits[11:10], RW, default = 2'b0 
       Controls IO Loopback.  Ctl[1] enables IOLB mode and Ctl[0] controls if this is a 
       Setup(0) or Hold(1) test 
     */
    UINT32 odtmode : 1;
    /* odtmode - Bits[12:12], RW, default = 1'b0 
       Forces the driver in ODT mode, where both the pullup and pulldown are enabled.  
       Used for Rstray testing 
     */
    UINT32 cmdtxeq : 2;
    /* cmdtxeq - Bits[14:13], RW, default = 2'b0 
       not used
     */
    UINT32 earlyweakdrive : 2;
    /* earlyweakdrive - Bits[16:15], RW, default = 2'b0 
       Controls the number of segments enabled during the early warning weak drive: {0: 
       All Enabled, 1: Enable Seg[4], 2: Enable Seg[4:3], 3: Enable Seg[4:2]} 
     */
    UINT32 lvmode : 2;
    /* lvmode - Bits[18:17], RW, default = 2'h0 
       not used
     */
    UINT32 lvmodevalid : 1;
    /* lvmodevalid - Bits[19:19], RW, default = 1'h0 
       not used
     */
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[20:20], RW, default = 1'h0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxvref : 7;
    /* rxvref - Bits[27:21], RW, default = 7'b0 
       IOLB Vref Control.  2s compliment encoding from 1/3 Vdd to 2/3 Vdd with step 
       size of Vdd/192 (7.8mV) 
     */
    UINT32 halflsbbit : 1;
    /* halflsbbit - Bits[28:28], RW, default = 1'b0 
       enables fine grain Vref control, shift down the Vref voltage by ~0.5 stepsize
     */
    UINT32 vrefdisable : 1;
    /* vrefdisable - Bits[29:29], RW, default = 1'b0 
        Vref Disable
     */
    UINT32 ddr4modeenable : 1;
    /* ddr4modeenable - Bits[30:30], RW, default = 1'b0 
       Ddr4 Mode Enable, set to 1 in CMD2 (Func:5.Addr:C00 and Func:7.Addr:C00) fub to 
       bypass extra delay on parity bit 
     */
    UINT32 xovercal : 1;
    /* xovercal - Bits[31:31], RW, default = 1'b0 
       Enables Xover Calibration Training Mode
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS2_NB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1F10)                                                     */
/* Register default value:              0x01400000                            */
#define DDRCRCMDCONTROLS2_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F10
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 21;
    UINT32 ddr4extend : 1;
    /* ddr4extend - Bits[21:21], RW, default = 1'h0 
       Enable finer vref resolution in ddr4
     */
    UINT32 rsvd_22 : 1;
    UINT32 cmdrxbiassel : 1;
    /* cmdrxbiassel - Bits[23:23], RW, default = 1'b0 
       CmdRxBiasSel for Cmd Receiver
     */
    UINT32 forcecmdbiason : 1;
    /* forcecmdbiason - Bits[24:24], RW, default = 1'b1 
       ForceCmdBiason for Cmd Receiver
     */
    UINT32 rsvd_25 : 1;
    UINT32 cmdctlecapen : 2;
    /* cmdctlecapen - Bits[27:26], RW, default = 2'b0 
       CmdCtl Buffer CtleCapEn
     */
    UINT32 cmdctleresen : 2;
    /* cmdctleresen - Bits[29:28], RW, default = 2'b0 
       CmdCtl Buffer CtleResEn
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS2_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLCOMP_NB_FNV_DDRIO_DAT7_CH_REG supported on:                        */
/*       EKV (0x401B1F14)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCTLCOMP_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F14
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 scomp : 6;
    /* scomp - Bits[5:0], RWV, default = 6'b0 
       Scomp Value
     */
    UINT32 tcocomp : 7;
    /* tcocomp - Bits[12:6], RWV, default = 7'b0 
       Tco Comp Value
     */
    UINT32 rcompdrvup : 6;
    /* rcompdrvup - Bits[18:13], RWV, default = 6'b0 
       Rcomp Drv Up Value
     */
    UINT32 rcompdrvdown : 6;
    /* rcompdrvdown - Bits[24:19], RWV, default = 6'b0 
       Rcomp Drv Dn Value
     */
    UINT32 lscomp : 3;
    /* lscomp - Bits[27:25], RWV, default = 3'b010 
       LevelShift Comp Value
     */
    UINT32 spare : 4;
    /* spare - Bits[31:28], RWV, default = 4'b0 
       Spare
     */
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMP_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLCOMPOFFSET_NB_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1F18)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMPOFFSET_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F18
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Comp Offset Values. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 
 * versions are not supported. 
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 18;
    UINT32 cmdctlbufsel : 6;
    /* cmdctlbufsel - Bits[23:18], RW, default = 6'b0 
       CmdBuf or CtlBuf Selection
     */
    UINT32 iodirectionvalid : 1;
    /* iodirectionvalid - Bits[24:24], RW, default = 1'b0 
       Is the Direction (RX/TX Valid)
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCTLCOMPOFFSET_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS3_NB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1F1C)                                                     */
/* Register default value:              0x0000007F                            */
#define DDRCRCMDCONTROLS3_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F1C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls 3. Multicast read for CMDCTL_CH012 and CMDCTL_CH345 versions are 
 * not supported. 
 */
typedef union {
  struct {
    UINT32 bufdrvsegen : 5;
    /* bufdrvsegen - Bits[4:0], RW, default = 5'b11111 
       Drive strength enable for CmdBuf/CtlBuf.
     */
    UINT32 cmdvrefpgen : 1;
    /* cmdvrefpgen - Bits[5:5], RW, default = 1'b1 
       control the DdrVrefSelDQQnnnH pin on the ddrvrefint generator
     */
    UINT32 odtstrength : 1;
    /* odtstrength - Bits[6:6], RW, default = 1'b1 
       OdtStrength
     */
    UINT32 rsvd_7 : 2;
    UINT32 divby2alignctl : 1;
    /* divby2alignctl - Bits[9:9], RW, default = 1'h0 
       Invert DAlign
     */
    UINT32 divby2disable : 1;
    /* divby2disable - Bits[10:10], RW, default = 1'h0 
       select qclk for cmdxover instead of Dclk
     */
    UINT32 odten : 12;
    /* odten - Bits[22:11], RW, default = 12'h0 
       ODTDis for Cmd and Ctl buf
     */
    UINT32 cmdrefpiclkdelay : 6;
    /* cmdrefpiclkdelay - Bits[28:23], RW, default = 6'h0 
       CmdRefClk
     */
    UINT32 rsvd_29 : 3;
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS3_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCTLRANKSUSED_NB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1F20)                                                     */
/* Register default value:              0x0000003F                            */
#define DDRCRCTLRANKSUSED_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F20
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CTL Ranks Used. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 ranken : 6;
    /* ranken - Bits[5:0], RW, default = 6'b111111 
       Enables output buffers for ctlbufpair 0 to ctlbufpair6 , PI clocks and output 
       datapaths for this rank 
     */
    UINT32 rsvd_6 : 6;
    UINT32 lvlshft_holden : 1;
    /* lvlshft_holden - Bits[12:12], RW, default = 1'b0 
       level shift hold enable[br]
                            Holds the HV control values and power down mode the level 
       shifters.[br] 
                            Apply '1' after training.  Slow timing - not advised for 
       fast enabling 
     */
    UINT32 rxbiasgcncomp : 2;
    /* rxbiasgcncomp - Bits[14:13], RW, default = 2'b00 
       used to compensate the GCN skew. The GCN skew information should be[br]
                 extracted by reading and analyzing the final Rcomp code value. The 
       target spec[br] 
                 is to categorize the GCN skew in 3 process condition (mainly slow, 
       fast, typical).[br] 
                 (one example can be just reading the 2 MLS bit of Rcomp code and 
       determine if the[br] 
                 GCN is slow, fast or typical). The gcncomp is used for CTLE.[br]
                 11: gcn in slow corner[br]
                 10: gcn in typical corner[br]
                 01: gcn in fast corner
     */
    UINT32 rsvd_15 : 1;
    UINT32 imodebiashighcm : 1;
    /* imodebiashighcm - Bits[16:16], RW, default = 1'b0 
       Imode bias control
     */
    UINT32 rxbiassel : 1;
    /* rxbiassel - Bits[17:17], RW, default = 1'b0 
       Rx bias Sel
     */
    UINT32 cmdrxeqqnnnl : 3;
    /* cmdrxeqqnnnl - Bits[20:18], RW, default = 3'b0 
       CmdRxEqQnnnL for RX CmdCtl Buf
     */
    UINT32 rsvd_21 : 3;
    UINT32 ddrcmdrxbiasctl : 3;
    /* ddrcmdrxbiasctl - Bits[26:24], RW, default = 3'b0 
       CmdRxBiasCtl for RX biasgen
     */
    UINT32 cmdpcasbiasclosedloopen : 1;
    /* cmdpcasbiasclosedloopen - Bits[27:27], RW, default = 1'b0 
       CmdPcasBiasClosedLoopEnQnnnH for RX biasgen
     */
    UINT32 ddrcmdrxbiasfoldedlegtrim : 2;
    /* ddrcmdrxbiasfoldedlegtrim - Bits[29:28], RW, default = 2'b0 
       DdrCmdRxBiasFoldedLegTrim for RX biasgen
     */
    UINT32 rsvd_30 : 2;
  } Bits;
  UINT32 Data;
} DDRCRCTLRANKSUSED_NB_FNV_DDRIO_DAT7_CH_STRUCT;






/* DDRCRCMDTRAINING_NB_FNV_DDRIO_DAT7_CH_REG supported on:                    */
/*       EKV (0x401B1F2C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDTRAINING_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F2C
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Cmd Training Register. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 trainingoffset : 6;
    /* trainingoffset - Bits[5:0], RW, default = 6'b0 
       Offset added to Trained PiCode Value.
     */
    UINT32 spare0 : 2;
    /* spare0 - Bits[7:6], ROV, default = 2'h0 
       Spare
     */
    UINT32 xoverphasedet : 14;
    /* xoverphasedet - Bits[21:8], ROV, default = 14'b0 
       bits13:6 unused[br]
                            bit5 output of phase detector for I/O Pair 0[br]
                            bit4 output of phase detector for I/O Pair 1[br]
                            bit3 output of phase detector for I/O Pair 2[br]
                            bit2 output of phase detector for I/O Pair 3[br]
                            bit1 output of phase detector for I/O Pair 4[br]
                            bit0 output of phase detector for I/O Pair 5 (only on 
       cmdctla FUBs) 
     */
    UINT32 trainingoffset2 : 6;
    /* trainingoffset2 - Bits[27:22], RW, default = 6'b0 
       Offset added to PiCode. Obtained via xover calibration such that ClkPi aligns 
       with grid clock when PiCode=0. 
     */
    UINT32 rsvd_28 : 4;
  } Bits;
  UINT32 Data;
} DDRCRCMDTRAINING_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCONTROLS1_NB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1F30)                                                     */
/* Register default value:              0x03C01000                            */
#define DDRCRCMDCONTROLS1_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F30
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Controls 1. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdoutputensel : 12;
    /* cmdoutputensel - Bits[11:0], RW, default = 12'b0 
       CmdOutputEnable
     */
    UINT32 selectcompcode : 1;
    /* selectcompcode - Bits[12:12], RW, default = 1'b1 
       Select CompCode between cmd or ctl
     */
    UINT32 rsvd_13 : 9;
    UINT32 dqstatdflt : 2;
    /* dqstatdflt - Bits[23:22], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 odtstatdflt : 2;
    /* odtstatdflt - Bits[25:24], RW, default = 2'b11 
       Static leg enable
     */
    UINT32 rsvd_26 : 1;
    UINT32 txeq : 5;
    /* txeq - Bits[31:27], RW, default = 5'b0 
       Write Equalization codes.  5 bit control, but only 24 of them are meaningful. 
       [br] 
                   The decoding is 1-1 matched, means 0 is 0 legs enabled for 
       deemphasized, 1 is 1 leg enabled [br] 
                   for deemphasized, and so on.
     */
  } Bits;
  UINT32 Data;
} DDRCRCMDCONTROLS1_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING2_NB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1F34)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING2_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F34
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode3 : 6;
    /* cmdpicode3 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay3 : 2;
    /* cmdpilogicdelay3 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 1;
    UINT32 cmdpicode4 : 6;
    /* cmdpicode4 - Bits[14:9], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay4 : 2;
    /* cmdpilogicdelay4 - Bits[16:15], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_17 : 1;
    UINT32 cmdpicode5 : 6;
    /* cmdpicode5 - Bits[23:18], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay5 : 2;
    /* cmdpilogicdelay5 - Bits[25:24], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_26 : 6;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING2_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDPICODING3_NB_FNV_DDRIO_DAT7_CH_REG supported on:                   */
/*       EKV (0x401B1F38)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING3_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F38
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * CMD Pi Settings. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 cmdpicode6 : 6;
    /* cmdpicode6 - Bits[5:0], RW, default = 6'b0 
       Cmd Pi Code[br]
                            [5:0] - Pi setting with [5] being the PiMSB sel
     */
    UINT32 cmdpilogicdelay6 : 2;
    /* cmdpilogicdelay6 - Bits[7:6], RW, default = 2'b00 
       Cmd Pi Logic Delay
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} DDRCRCMDPICODING3_NB_FNV_DDRIO_DAT7_CH_STRUCT;




/* VSSHIORVREFCONTROL_NB_FNV_DDRIO_DAT7_CH_REG supported on:                  */
/*       EKV (0x401B1F40)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F40
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Multicast read for CH012 and CH345 versions are not supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 24;
    /* vsshiorvrefctl - Bits[23:0], RW, default = 24'h4d8238 
         Channel 0 fubs : VssHi adjust setting. Channel1 fubs: DIMM VREF adjust 
       settings.[br] 
       [br]
               VssHi Field Description:[br]
       [br]
                Field          Bits    Description[br]
               -------         ----    
       ---------------------------------------------------------------------------------------------------------------------------------------------------------------[br] 
               SelCode          23     Selects the code to output:  {0: Code, 1: 
       TCode}[br] 
               GainBoost        22     Enable gain boost is panic mode by updating 
       code/tcode[br] 
               PanicVoltage     21:18  Unsigned integer controlling the voltage error 
       on VssHi that will trigger the panic driver.  Step size of VccDDQ/192 (0 to 
       117mV)[br] 
               CloseLoop        17     Enables the VssHi close loop tracking[br]
               PanicEn          16     Enables the VssHi Panic Driver[br]
               BWError          15:14  Limits the loop gain (ie: difference between 
       code and tcode) to +/- 2^(SlowBWError)[br] 
               OpenLoop         13     Operates VssHi in open loop mode with the target 
       Vref code directly driving the output DAC with no feedback involved[br] 
               SampleDivider    12:10  Controls the loop bandwidth by dividing down the 
       input clock: {0: Qclk, 1: Qclk/2, 2: Qclk/4, 3: Qclk/8, 4: Qclk/16, 5: Qclk/32, 
       6: Qclk/64, 7: Qclk/128}[br] 
               LoBWDivider      9:8    For the slow integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               HiBWDivider      7:6    For the fast integrator, selects the net number 
       of samples in a given direction to trip the filter.  {0: 4 samples, 1: 16 
       samples, 2: 32 samples, 3: 64 samples}[br] 
               Target           5:0    Unsigned integer controlling the target VssHi 
       voltage.  Step size of VccDDQ/192 and starting value of VccDDQ*20/192 
     */
    UINT32 rsvd_24 : 8;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* DDRCRCMDCOMP1_NB_FNV_DDRIO_DAT7_CH_REG supported on:                       */
/*       EKV (0x401B1F44)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMP1_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F44
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * Cmd Comp Values. Multicast read for CH012 and CH345 versions are not supported.
 */
typedef union {
  struct {
    UINT32 rsvd_0 : 5;
    UINT32 panicdrvdn : 10;
    /* panicdrvdn - Bits[14:5], RW, default = 10'b0000000000 
       Panic DrvDn RComp Value.
     */
    UINT32 panicdrvup : 10;
    /* panicdrvup - Bits[24:15], RW, default = 10'b0000000000 
       Panic DrvUp RComp Comp Value.
     */
    UINT32 rsvd_25 : 7;
  } Bits;
  UINT32 Data;
} DDRCRCMDCOMP1_NB_FNV_DDRIO_DAT7_CH_STRUCT;


/* VSSHIORVREFCONTROL1_NB_FNV_DDRIO_DAT7_CH_REG supported on:                 */
/*       EKV (0x401B1F48)                                                     */
/* Register default value:              0x00000000                            */
#define VSSHIORVREFCONTROL1_NB_FNV_DDRIO_DAT7_CH_REG 0x050F4F48
/* Struct format extracted from XML file EKV\1.22.1.EKV.xml.
 * VssHi Controls on Channel 0 only. DIMM Vref Controls on  Channel1, databyte 7, 
 * only. Extension for BDX. Multicast read for CH012 and CH345 versions are not 
 * supported. 
 */
typedef union {
  struct {
    UINT32 vsshiorvrefctl : 8;
    /* vsshiorvrefctl - Bits[7:0], RW, default = 8'b0 
        BDX-specific extended control signal.
     */
    UINT32 rsvd_8 : 24;
  } Bits;
  UINT32 Data;
} VSSHIORVREFCONTROL1_NB_FNV_DDRIO_DAT7_CH_STRUCT;


#endif /* FNV_DDRIO_DAT7_CH_h */
