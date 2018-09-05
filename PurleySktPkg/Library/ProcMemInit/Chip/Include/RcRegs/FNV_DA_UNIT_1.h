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

#ifndef FNV_DA_UNIT_1_h
#define FNV_DA_UNIT_1_h

#include "DataTypes.h"

/* Device and Function specifications:                                        */
/* For all target CPUs:                                                       */
/* FNV_DA_UNIT_1_DEV 0                                                        */
/* FNV_DA_UNIT_1_FUN 3                                                        */

/* DA_CXFIFO_TRAININGRESET_FNV_DA_UNIT_1_REG supported on:                    */
/*       EKV (0x40003800)                                                     */
/* Register default value:              0x00000000                            */
#define DA_CXFIFO_TRAININGRESET_FNV_DA_UNIT_1_REG 0x05034800
/* Struct format extracted from XML file EKV\0.0.3.EKV.xml.
 * Resets CX FIFOs pointers in core during training 
 */
typedef union {
  struct {
    UINT32 cxfifo_trainreset : 1;
    /* cxfifo_trainreset - Bits[0:0], RW, default = 1'h0 
       This resets CX FIFO pointers in training
     */
    UINT32 rsvd : 31;
    /* rsvd - Bits[31:1], n/a, default = n/a 
       Padding added by header generation tool.
     */
  } Bits;
  UINT32 Data;
} DA_CXFIFO_TRAININGRESET_FNV_DA_UNIT_1_STRUCT;
















































#endif /* FNV_DA_UNIT_1_h */
