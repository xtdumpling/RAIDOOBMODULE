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

#ifndef FNV_DDRIO_DAT_h
#define FNV_DDRIO_DAT_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* FNV_DDRIO_DAT_DEV 22                                                       */
/* FNV_DDRIO_DAT_FUN 6                                                        */

/* RXGROUP0N0RANK0_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6200)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK0_FNV_DDRIO_DAT_REG 0x05104200
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N0RANK0_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N0RANK1_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6204)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK1_FNV_DDRIO_DAT_REG 0x05104204
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N0RANK1_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N0RANK2_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6208)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK2_FNV_DDRIO_DAT_REG 0x05104208
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N0RANK2_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N0RANK3_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B620C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK3_FNV_DDRIO_DAT_REG 0x0510420C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N0RANK3_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N0RANK4_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6210)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK4_FNV_DDRIO_DAT_REG 0x05104210
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N0RANK4_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N0RANK5_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6214)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK5_FNV_DDRIO_DAT_REG 0x05104214
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N0RANK5_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N0RANK6_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6218)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK6_FNV_DDRIO_DAT_REG 0x05104218
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N0RANK6_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N0RANK7_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B621C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N0RANK7_FNV_DDRIO_DAT_REG 0x0510421C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N0RANK7_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N0RANK0_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6220)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK0_FNV_DDRIO_DAT_REG 0x05104220
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N0RANK0_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N0RANK1_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6224)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK1_FNV_DDRIO_DAT_REG 0x05104224
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N0RANK1_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N0RANK2_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6228)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK2_FNV_DDRIO_DAT_REG 0x05104228
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N0RANK2_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N0RANK3_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B622C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK3_FNV_DDRIO_DAT_REG 0x0510422C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N0RANK3_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N0RANK4_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6230)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK4_FNV_DDRIO_DAT_REG 0x05104230
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N0RANK4_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N0RANK5_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6234)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK5_FNV_DDRIO_DAT_REG 0x05104234
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N0RANK5_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N0RANK6_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6238)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK6_FNV_DDRIO_DAT_REG 0x05104238
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N0RANK6_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N0RANK7_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B623C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N0RANK7_FNV_DDRIO_DAT_REG 0x0510423C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N0RANK7_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N0RANK0_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6240)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK0_FNV_DDRIO_DAT_REG 0x05104240
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N0RANK0_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N0RANK1_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6244)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK1_FNV_DDRIO_DAT_REG 0x05104244
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N0RANK1_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N0RANK2_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6248)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK2_FNV_DDRIO_DAT_REG 0x05104248
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N0RANK2_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N0RANK3_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B624C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK3_FNV_DDRIO_DAT_REG 0x0510424C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N0RANK3_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N0RANK4_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6250)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK4_FNV_DDRIO_DAT_REG 0x05104250
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N0RANK4_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N0RANK5_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6254)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK5_FNV_DDRIO_DAT_REG 0x05104254
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N0RANK5_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N0RANK6_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6258)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK6_FNV_DDRIO_DAT_REG 0x05104258
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N0RANK6_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N0RANK7_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B625C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N0RANK7_FNV_DDRIO_DAT_REG 0x0510425C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N0RANK7_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N0RANK0_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6260)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK0_FNV_DDRIO_DAT_REG 0x05104260
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N0RANK0_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N0RANK1_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6264)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK1_FNV_DDRIO_DAT_REG 0x05104264
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N0RANK1_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N0RANK2_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6268)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK2_FNV_DDRIO_DAT_REG 0x05104268
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N0RANK2_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N0RANK3_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B626C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK3_FNV_DDRIO_DAT_REG 0x0510426C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N0RANK3_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N0RANK4_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6270)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK4_FNV_DDRIO_DAT_REG 0x05104270
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N0RANK4_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N0RANK5_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6274)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK5_FNV_DDRIO_DAT_REG 0x05104274
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N0RANK5_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N0RANK6_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6278)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK6_FNV_DDRIO_DAT_REG 0x05104278
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N0RANK6_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N0RANK7_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B627C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N0RANK7_FNV_DDRIO_DAT_REG 0x0510427C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N0RANK7_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN0RANK0_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6280)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK0_FNV_DDRIO_DAT_REG 0x05104280
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN0RANK0_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN0RANK1_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6284)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK1_FNV_DDRIO_DAT_REG 0x05104284
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN0RANK1_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN0RANK2_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6288)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK2_FNV_DDRIO_DAT_REG 0x05104288
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN0RANK2_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN0RANK3_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B628C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK3_FNV_DDRIO_DAT_REG 0x0510428C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN0RANK3_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN0RANK4_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6290)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK4_FNV_DDRIO_DAT_REG 0x05104290
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN0RANK4_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN0RANK5_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6294)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK5_FNV_DDRIO_DAT_REG 0x05104294
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN0RANK5_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN0RANK6_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6298)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK6_FNV_DDRIO_DAT_REG 0x05104298
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN0RANK6_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN0RANK7_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B629C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN0RANK7_FNV_DDRIO_DAT_REG 0x0510429C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN0RANK7_FNV_DDRIO_DAT_STRUCT;


/* TXXTALKN0_FNV_DDRIO_DAT_REG supported on:                                  */
/*       EKV (0x401B62A0)                                                     */
/* Register default value:              0x00000000                            */
#define TXXTALKN0_FNV_DDRIO_DAT_REG 0x051042A0
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXXTALKN0_FNV_DDRIO_DAT_STRUCT;


/* RXVREFCTRLN0_FNV_DDRIO_DAT_REG supported on:                               */
/*       EKV (0x401B62A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN0_FNV_DDRIO_DAT_REG 0x051042A4
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXVREFCTRLN0_FNV_DDRIO_DAT_STRUCT;


/* DATATRAINFEEDBACKN0_FNV_DDRIO_DAT_REG supported on:                        */
/*       EKV (0x401B62A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN0_FNV_DDRIO_DAT_REG 0x051042A8
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DATATRAINFEEDBACKN0_FNV_DDRIO_DAT_STRUCT;


/* DDRCRFSMCAL_FNV_DDRIO_DAT_REG supported on:                                */
/*       EKV (0x401B62C8)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRFSMCAL_FNV_DDRIO_DAT_REG 0x051042C8
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRFSMCAL_FNV_DDRIO_DAT_STRUCT;


/* DATACONTROL4_FNV_DDRIO_DAT_REG supported on:                               */
/*       EKV (0x401B62CC)                                                     */
/* Register default value:              0x801F0000                            */
#define DATACONTROL4_FNV_DDRIO_DAT_REG 0x051042CC
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DATACONTROL4_FNV_DDRIO_DAT_STRUCT;


/* ATTACK1SEL_FNV_DDRIO_DAT_REG supported on:                                 */
/*       EKV (0x401B62D0)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK1SEL_FNV_DDRIO_DAT_REG 0x051042D0
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} ATTACK1SEL_FNV_DDRIO_DAT_STRUCT;


/* COMPDATA0_FNV_DDRIO_DAT_REG supported on:                                  */
/*       EKV (0x401B62D4)                                                     */
/* Register default value:              0x00028000                            */
#define COMPDATA0_FNV_DDRIO_DAT_REG 0x051042D4
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} COMPDATA0_FNV_DDRIO_DAT_STRUCT;


/* COMPDATA1_FNV_DDRIO_DAT_REG supported on:                                  */
/*       EKV (0x401B62D8)                                                     */
/* Register default value:              0x04010000                            */
#define COMPDATA1_FNV_DDRIO_DAT_REG 0x051042D8
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} COMPDATA1_FNV_DDRIO_DAT_STRUCT;


/* DATAOFFSETTRAIN_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B62DC)                                                     */
/* Register default value:              0x00000000                            */
#define DATAOFFSETTRAIN_FNV_DDRIO_DAT_REG 0x051042DC
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DATAOFFSETTRAIN_FNV_DDRIO_DAT_STRUCT;




/* ATTACK0SEL_FNV_DDRIO_DAT_REG supported on:                                 */
/*       EKV (0x401B62E4)                                                     */
/* Register default value:              0x00000000                            */
#define ATTACK0SEL_FNV_DDRIO_DAT_REG 0x051042E4
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} ATTACK0SEL_FNV_DDRIO_DAT_STRUCT;


/* DATACONTROL0_FNV_DDRIO_DAT_REG supported on:                               */
/*       EKV (0x401B62E8)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL0_FNV_DDRIO_DAT_REG 0x051042E8
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DATACONTROL0_FNV_DDRIO_DAT_STRUCT;


/* DATACONTROL1_FNV_DDRIO_DAT_REG supported on:                               */
/*       EKV (0x401B62EC)                                                     */
/* Register default value:              0x00000000                            */
#define DATACONTROL1_FNV_DDRIO_DAT_REG 0x051042EC
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DATACONTROL1_FNV_DDRIO_DAT_STRUCT;


/* DATACONTROL2_FNV_DDRIO_DAT_REG supported on:                               */
/*       EKV (0x401B62F0)                                                     */
/* Register default value:              0x07800000                            */
#define DATACONTROL2_FNV_DDRIO_DAT_REG 0x051042F0
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DATACONTROL2_FNV_DDRIO_DAT_STRUCT;


/* DATACONTROL3_FNV_DDRIO_DAT_REG supported on:                               */
/*       EKV (0x401B62F4)                                                     */
/* Register default value:              0x2198DFCF                            */
#define DATACONTROL3_FNV_DDRIO_DAT_REG 0x051042F4
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DATACONTROL3_FNV_DDRIO_DAT_STRUCT;


/* VSSHIORVREFCONTROL_DAT_FNV_DDRIO_DAT_REG supported on:                     */
/*       EKV (0x401B62F8)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_DAT_FNV_DDRIO_DAT_REG 0x051042F8
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} VSSHIORVREFCONTROL_DAT_FNV_DDRIO_DAT_STRUCT;




/* RXGROUP0N1RANK0_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6300)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK0_FNV_DDRIO_DAT_REG 0x05104300
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N1RANK0_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N1RANK1_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6304)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK1_FNV_DDRIO_DAT_REG 0x05104304
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N1RANK1_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N1RANK2_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6308)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK2_FNV_DDRIO_DAT_REG 0x05104308
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N1RANK2_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N1RANK3_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B630C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK3_FNV_DDRIO_DAT_REG 0x0510430C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N1RANK3_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N1RANK4_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6310)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK4_FNV_DDRIO_DAT_REG 0x05104310
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N1RANK4_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N1RANK5_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6314)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK5_FNV_DDRIO_DAT_REG 0x05104314
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N1RANK5_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N1RANK6_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6318)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK6_FNV_DDRIO_DAT_REG 0x05104318
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N1RANK6_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP0N1RANK7_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B631C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP0N1RANK7_FNV_DDRIO_DAT_REG 0x0510431C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP0N1RANK7_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N1RANK0_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6320)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK0_FNV_DDRIO_DAT_REG 0x05104320
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N1RANK0_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N1RANK1_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6324)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK1_FNV_DDRIO_DAT_REG 0x05104324
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N1RANK1_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N1RANK2_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6328)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK2_FNV_DDRIO_DAT_REG 0x05104328
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N1RANK2_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N1RANK3_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B632C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK3_FNV_DDRIO_DAT_REG 0x0510432C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N1RANK3_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N1RANK4_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6330)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK4_FNV_DDRIO_DAT_REG 0x05104330
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N1RANK4_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N1RANK5_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6334)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK5_FNV_DDRIO_DAT_REG 0x05104334
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N1RANK5_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N1RANK6_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6338)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK6_FNV_DDRIO_DAT_REG 0x05104338
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N1RANK6_FNV_DDRIO_DAT_STRUCT;


/* RXGROUP1N1RANK7_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B633C)                                                     */
/* Register default value:              0x00000000                            */
#define RXGROUP1N1RANK7_FNV_DDRIO_DAT_REG 0x0510433C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXGROUP1N1RANK7_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N1RANK0_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6340)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK0_FNV_DDRIO_DAT_REG 0x05104340
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N1RANK0_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N1RANK1_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6344)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK1_FNV_DDRIO_DAT_REG 0x05104344
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N1RANK1_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N1RANK2_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6348)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK2_FNV_DDRIO_DAT_REG 0x05104348
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N1RANK2_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N1RANK3_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B634C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK3_FNV_DDRIO_DAT_REG 0x0510434C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N1RANK3_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N1RANK4_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6350)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK4_FNV_DDRIO_DAT_REG 0x05104350
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N1RANK4_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N1RANK5_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6354)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK5_FNV_DDRIO_DAT_REG 0x05104354
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N1RANK5_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N1RANK6_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6358)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK6_FNV_DDRIO_DAT_REG 0x05104358
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N1RANK6_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP0N1RANK7_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B635C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP0N1RANK7_FNV_DDRIO_DAT_REG 0x0510435C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP0N1RANK7_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N1RANK0_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6360)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK0_FNV_DDRIO_DAT_REG 0x05104360
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N1RANK0_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N1RANK1_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6364)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK1_FNV_DDRIO_DAT_REG 0x05104364
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N1RANK1_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N1RANK2_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6368)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK2_FNV_DDRIO_DAT_REG 0x05104368
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N1RANK2_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N1RANK3_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B636C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK3_FNV_DDRIO_DAT_REG 0x0510436C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N1RANK3_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N1RANK4_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6370)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK4_FNV_DDRIO_DAT_REG 0x05104370
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N1RANK4_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N1RANK5_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6374)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK5_FNV_DDRIO_DAT_REG 0x05104374
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N1RANK5_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N1RANK6_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6378)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK6_FNV_DDRIO_DAT_REG 0x05104378
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N1RANK6_FNV_DDRIO_DAT_STRUCT;


/* TXGROUP1N1RANK7_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B637C)                                                     */
/* Register default value:              0x00000000                            */
#define TXGROUP1N1RANK7_FNV_DDRIO_DAT_REG 0x0510437C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} TXGROUP1N1RANK7_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN1RANK0_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6380)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK0_FNV_DDRIO_DAT_REG 0x05104380
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN1RANK0_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN1RANK1_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6384)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK1_FNV_DDRIO_DAT_REG 0x05104384
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN1RANK1_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN1RANK2_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6388)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK2_FNV_DDRIO_DAT_REG 0x05104388
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN1RANK2_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN1RANK3_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B638C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK3_FNV_DDRIO_DAT_REG 0x0510438C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN1RANK3_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN1RANK4_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6390)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK4_FNV_DDRIO_DAT_REG 0x05104390
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN1RANK4_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN1RANK5_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6394)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK5_FNV_DDRIO_DAT_REG 0x05104394
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN1RANK5_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN1RANK6_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B6398)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK6_FNV_DDRIO_DAT_REG 0x05104398
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN1RANK6_FNV_DDRIO_DAT_STRUCT;


/* RXOFFSETN1RANK7_FNV_DDRIO_DAT_REG supported on:                            */
/*       EKV (0x401B639C)                                                     */
/* Register default value:              0x00000000                            */
#define RXOFFSETN1RANK7_FNV_DDRIO_DAT_REG 0x0510439C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXOFFSETN1RANK7_FNV_DDRIO_DAT_STRUCT;




/* RXVREFCTRLN1_FNV_DDRIO_DAT_REG supported on:                               */
/*       EKV (0x401B63A4)                                                     */
/* Register default value:              0x00000000                            */
#define RXVREFCTRLN1_FNV_DDRIO_DAT_REG 0x051043A4
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} RXVREFCTRLN1_FNV_DDRIO_DAT_STRUCT;


/* DATATRAINFEEDBACKN1_FNV_DDRIO_DAT_REG supported on:                        */
/*       EKV (0x401B63A8)                                                     */
/* Register default value:              0x00000000                            */
#define DATATRAINFEEDBACKN1_FNV_DDRIO_DAT_REG 0x051043A8
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DATATRAINFEEDBACKN1_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCMDCOMP_FNV_DDRIO_DAT_REG supported on:                               */
/*       EKV (0x401B6800)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCMDCOMP_FNV_DDRIO_DAT_REG 0x05104800
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCMDCOMP_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCMDCOMPOFFSET_FNV_DDRIO_DAT_REG supported on:                         */
/*       EKV (0x401B6804)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMPOFFSET_FNV_DDRIO_DAT_REG 0x05104804
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCMDCOMPOFFSET_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCMDPICODING_FNV_DDRIO_DAT_REG supported on:                           */
/*       EKV (0x401B6808)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING_FNV_DDRIO_DAT_REG 0x05104808
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCMDPICODING_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCMDCONTROLS_FNV_DDRIO_DAT_REG supported on:                           */
/*       EKV (0x401B680C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCONTROLS_FNV_DDRIO_DAT_REG 0x0510480C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCMDCONTROLS_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCMDCONTROLS2_FNV_DDRIO_DAT_REG supported on:                          */
/*       EKV (0x401B6810)                                                     */
/* Register default value:              0x01400000                            */
#define DDRCRCMDCONTROLS2_FNV_DDRIO_DAT_REG 0x05104810
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCMDCONTROLS2_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCTLCOMP_FNV_DDRIO_DAT_REG supported on:                               */
/*       EKV (0x401B6814)                                                     */
/* Register default value:              0x04000000                            */
#define DDRCRCTLCOMP_FNV_DDRIO_DAT_REG 0x05104814
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCTLCOMP_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCTLCOMPOFFSET_FNV_DDRIO_DAT_REG supported on:                         */
/*       EKV (0x401B6818)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCTLCOMPOFFSET_FNV_DDRIO_DAT_REG 0x05104818
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCTLCOMPOFFSET_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCMDCONTROLS3_FNV_DDRIO_DAT_REG supported on:                          */
/*       EKV (0x401B681C)                                                     */
/* Register default value:              0x0000007F                            */
#define DDRCRCMDCONTROLS3_FNV_DDRIO_DAT_REG 0x0510481C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCMDCONTROLS3_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCTLRANKSUSED_FNV_DDRIO_DAT_REG supported on:                          */
/*       EKV (0x401B6820)                                                     */
/* Register default value:              0x0000003F                            */
#define DDRCRCTLRANKSUSED_FNV_DDRIO_DAT_REG 0x05104820
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCTLRANKSUSED_FNV_DDRIO_DAT_STRUCT;






/* DDRCRCMDTRAINING_FNV_DDRIO_DAT_REG supported on:                           */
/*       EKV (0x401B682C)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDTRAINING_FNV_DDRIO_DAT_REG 0x0510482C
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCMDTRAINING_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCMDCONTROLS1_FNV_DDRIO_DAT_REG supported on:                          */
/*       EKV (0x401B6830)                                                     */
/* Register default value:              0x03C01000                            */
#define DDRCRCMDCONTROLS1_FNV_DDRIO_DAT_REG 0x05104830
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCMDCONTROLS1_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCMDPICODING2_FNV_DDRIO_DAT_REG supported on:                          */
/*       EKV (0x401B6834)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING2_FNV_DDRIO_DAT_REG 0x05104834
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCMDPICODING2_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCMDPICODING3_FNV_DDRIO_DAT_REG supported on:                          */
/*       EKV (0x401B6838)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDPICODING3_FNV_DDRIO_DAT_REG 0x05104838
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCMDPICODING3_FNV_DDRIO_DAT_STRUCT;




/* VSSHIORVREFCONTROL_CMD_FNV_DDRIO_DAT_REG supported on:                     */
/*       EKV (0x401B6840)                                                     */
/* Register default value:              0x004D8238                            */
#define VSSHIORVREFCONTROL_CMD_FNV_DDRIO_DAT_REG 0x05104840
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} VSSHIORVREFCONTROL_CMD_FNV_DDRIO_DAT_STRUCT;


/* DDRCRCMDCOMP1_FNV_DDRIO_DAT_REG supported on:                              */
/*       EKV (0x401B6844)                                                     */
/* Register default value:              0x00000000                            */
#define DDRCRCMDCOMP1_FNV_DDRIO_DAT_REG 0x05104844
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} DDRCRCMDCOMP1_FNV_DDRIO_DAT_STRUCT;


/* VSSHIORVREFCONTROL1_FNV_DDRIO_DAT_REG supported on:                        */
/*       EKV (0x401B6848)                                                     */
/* Register default value:              0x00000000                            */
#define VSSHIORVREFCONTROL1_FNV_DDRIO_DAT_REG 0x05104848
/* Struct format extracted from XML file EKV\1.22.6.EKV.xml.
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
} VSSHIORVREFCONTROL1_FNV_DDRIO_DAT_STRUCT;


#endif /* FNV_DDRIO_DAT_h */
