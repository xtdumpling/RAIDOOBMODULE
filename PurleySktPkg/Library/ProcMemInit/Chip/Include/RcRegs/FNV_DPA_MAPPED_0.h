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

#ifndef FNV_DPA_MAPPED_0_h
#define FNV_DPA_MAPPED_0_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* FNV_DPA_MAPPED_0_DEV 0                                                     */
/* FNV_DPA_MAPPED_0_FUN 0                                                     */











/* MB_SMM_CMD_FNV_DPA_MAPPED_0_REG supported on:                              */
/*       EKV (0x40000204)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_CMD_FNV_DPA_MAPPED_0_REG 0x05004204
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_CMD_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_NONCE_0_FNV_DPA_MAPPED_0_REG supported on:                          */
/*       EKV (0x40000208)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_NONCE_0_FNV_DPA_MAPPED_0_REG 0x05004208
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_NONCE_0_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_NONCE_1_FNV_DPA_MAPPED_0_REG supported on:                          */
/*       EKV (0x4000020C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_NONCE_1_FNV_DPA_MAPPED_0_REG 0x0500420C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_NONCE_1_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_NONCE_2_FNV_DPA_MAPPED_0_REG supported on:                          */
/*       EKV (0x40000210)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_NONCE_2_FNV_DPA_MAPPED_0_REG 0x05004210
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_NONCE_2_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_NONCE_3_FNV_DPA_MAPPED_0_REG supported on:                          */
/*       EKV (0x40000214)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_NONCE_3_FNV_DPA_MAPPED_0_REG 0x05004214
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_NONCE_3_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000218)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_REG 0x05004218
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x4000021C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_REG 0x0500421C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000220)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_REG 0x05004220
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000224)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_REG 0x05004224
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000228)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_REG 0x05004228
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x4000022C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_REG 0x0500422C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000230)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_REG 0x05004230
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000234)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_REG 0x05004234
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000238)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_REG 0x05004238
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x4000023C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_REG 0x0500423C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000240)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_REG 0x05004240
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000244)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_REG 0x05004244
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000248)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_REG 0x05004248
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x4000024C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_REG 0x0500424C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000250)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_REG 0x05004250
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000254)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_REG 0x05004254
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000258)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_REG 0x05004258
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x4000025C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_REG 0x0500425C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000260)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_REG 0x05004260
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000264)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_REG 0x05004264
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000268)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_REG 0x05004268
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x4000026C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_REG 0x0500426C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000270)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_REG 0x05004270
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000274)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_REG 0x05004274
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000278)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_REG 0x05004278
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x4000027C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_REG 0x0500427C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000280)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_REG 0x05004280
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000284)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_REG 0x05004284
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000288)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_REG 0x05004288
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x4000028C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_REG 0x0500428C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000290)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_REG 0x05004290
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_INPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000294)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_INPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_REG 0x05004294
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_INPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_STATUS_FNV_DPA_MAPPED_0_REG supported on:                           */
/*       EKV (0x40000298)                                                     */
/* Register default value:              0x00000001                            */
#define MB_SMM_STATUS_FNV_DPA_MAPPED_0_REG 0x05004298
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_STATUS_FNV_DPA_MAPPED_0_STRUCT;




/* MB_SMM_OUTPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400002A0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_REG 0x050042A0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400002A4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_REG 0x050042A4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400002A8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_REG 0x050042A8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400002AC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_REG 0x050042AC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400002B0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_REG 0x050042B0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400002B4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_REG 0x050042B4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400002B8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_REG 0x050042B8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400002BC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_REG 0x050042BC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400002C0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_REG 0x050042C0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400002C4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_REG 0x050042C4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002C8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_REG 0x050042C8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002CC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_REG 0x050042CC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002D0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_REG 0x050042D0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002D4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_REG 0x050042D4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002D8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_REG 0x050042D8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002DC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_REG 0x050042DC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002E0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_REG 0x050042E0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002E4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_REG 0x050042E4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002E8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_REG 0x050042E8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002EC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_REG 0x050042EC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002F0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_REG 0x050042F0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002F4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_REG 0x050042F4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002F8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_REG 0x050042F8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x400002FC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_REG 0x050042FC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x40000300)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_REG 0x05004300
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x40000304)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_REG 0x05004304
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x40000308)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_REG 0x05004308
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x4000030C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_REG 0x0500430C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x40000310)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_REG 0x05004310
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x40000314)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_REG 0x05004314
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x40000318)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_REG 0x05004318
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_OUTPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_REG supported on:                */
/*       EKV (0x4000031C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_OUTPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_REG 0x0500431C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_OUTPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_STRUCT;


/* MB_SMM_ABORT_FNV_DPA_MAPPED_0_REG supported on:                            */
/*       EKV (0x40000320)                                                     */
/* Register default value:              0x00000000                            */
#define MB_SMM_ABORT_FNV_DPA_MAPPED_0_REG 0x05004320
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_SMM_ABORT_FNV_DPA_MAPPED_0_STRUCT;




/* MB_OS_CMD_FNV_DPA_MAPPED_0_REG supported on:                               */
/*       EKV (0x40000404)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_CMD_FNV_DPA_MAPPED_0_REG 0x05004404
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_CMD_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_NONCE_0_FNV_DPA_MAPPED_0_REG supported on:                           */
/*       EKV (0x40000408)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_NONCE_0_FNV_DPA_MAPPED_0_REG 0x05004408
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_NONCE_0_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_NONCE_1_FNV_DPA_MAPPED_0_REG supported on:                           */
/*       EKV (0x4000040C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_NONCE_1_FNV_DPA_MAPPED_0_REG 0x0500440C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_NONCE_1_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_NONCE_2_FNV_DPA_MAPPED_0_REG supported on:                           */
/*       EKV (0x40000410)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_NONCE_2_FNV_DPA_MAPPED_0_REG 0x05004410
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_NONCE_2_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_NONCE_3_FNV_DPA_MAPPED_0_REG supported on:                           */
/*       EKV (0x40000414)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_NONCE_3_FNV_DPA_MAPPED_0_REG 0x05004414
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_NONCE_3_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_REG supported on:                   */
/*       EKV (0x40000418)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_REG 0x05004418
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_REG supported on:                   */
/*       EKV (0x4000041C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_REG 0x0500441C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_REG supported on:                   */
/*       EKV (0x40000420)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_REG 0x05004420
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_REG supported on:                   */
/*       EKV (0x40000424)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_REG 0x05004424
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_REG supported on:                   */
/*       EKV (0x40000428)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_REG 0x05004428
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_REG supported on:                   */
/*       EKV (0x4000042C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_REG 0x0500442C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_REG supported on:                   */
/*       EKV (0x40000430)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_REG 0x05004430
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_REG supported on:                   */
/*       EKV (0x40000434)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_REG 0x05004434
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_REG supported on:                   */
/*       EKV (0x40000438)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_REG 0x05004438
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_REG supported on:                   */
/*       EKV (0x4000043C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_REG 0x0500443C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000440)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_REG 0x05004440
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000444)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_REG 0x05004444
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000448)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_REG 0x05004448
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x4000044C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_REG 0x0500444C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000450)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_REG 0x05004450
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000454)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_REG 0x05004454
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000458)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_REG 0x05004458
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x4000045C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_REG 0x0500445C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000460)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_REG 0x05004460
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000464)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_REG 0x05004464
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000468)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_REG 0x05004468
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x4000046C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_REG 0x0500446C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000470)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_REG 0x05004470
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000474)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_REG 0x05004474
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000478)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_REG 0x05004478
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x4000047C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_REG 0x0500447C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000480)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_REG 0x05004480
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000484)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_REG 0x05004484
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000488)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_REG 0x05004488
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x4000048C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_REG 0x0500448C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000490)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_REG 0x05004490
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_INPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x40000494)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_INPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_REG 0x05004494
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_INPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_STATUS_FNV_DPA_MAPPED_0_REG supported on:                            */
/*       EKV (0x40000498)                                                     */
/* Register default value:              0x00000001                            */
#define MB_OS_STATUS_FNV_DPA_MAPPED_0_REG 0x05004498
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_STATUS_FNV_DPA_MAPPED_0_STRUCT;




/* MB_OS_OUTPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x400004A0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_REG 0x050044A0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_0_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x400004A4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_REG 0x050044A4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_1_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x400004A8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_REG 0x050044A8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_2_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x400004AC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_REG 0x050044AC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_3_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x400004B0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_REG 0x050044B0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_4_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x400004B4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_REG 0x050044B4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_5_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x400004B8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_REG 0x050044B8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_6_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x400004BC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_REG 0x050044BC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_7_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x400004C0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_REG 0x050044C0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_8_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_REG supported on:                  */
/*       EKV (0x400004C4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_REG 0x050044C4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_9_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004C8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_REG 0x050044C8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_10_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004CC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_REG 0x050044CC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_11_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004D0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_REG 0x050044D0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_12_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004D4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_REG 0x050044D4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_13_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004D8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_REG 0x050044D8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_14_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004DC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_REG 0x050044DC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_15_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004E0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_REG 0x050044E0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_16_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004E4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_REG 0x050044E4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_17_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004E8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_REG 0x050044E8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_18_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004EC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_REG 0x050044EC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_19_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004F0)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_REG 0x050044F0
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_20_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004F4)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_REG 0x050044F4
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_21_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004F8)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_REG 0x050044F8
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_22_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x400004FC)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_REG 0x050044FC
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_23_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000500)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_REG 0x05004500
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_24_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000504)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_REG 0x05004504
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_25_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000508)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_REG 0x05004508
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_26_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x4000050C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_REG 0x0500450C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_27_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000510)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_REG 0x05004510
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_28_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000514)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_REG 0x05004514
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_29_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x40000518)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_REG 0x05004518
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_30_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_OUTPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_REG supported on:                 */
/*       EKV (0x4000051C)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_OUTPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_REG 0x0500451C
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_OUTPUT_PAYLOAD_31_FNV_DPA_MAPPED_0_STRUCT;


/* MB_OS_ABORT_FNV_DPA_MAPPED_0_REG supported on:                             */
/*       EKV (0x40000520)                                                     */
/* Register default value:              0x00000000                            */
#define MB_OS_ABORT_FNV_DPA_MAPPED_0_REG 0x05004520
/* Struct format extracted from XML file EKV\0.0.0.EKV.xml.
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
} MB_OS_ABORT_FNV_DPA_MAPPED_0_STRUCT;














#endif /* FNV_DPA_MAPPED_0_h */
