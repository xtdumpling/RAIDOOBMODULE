//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      Memory Reference Code                                             *
// *                                                                        *
// *      ESS - Enterprise Silicon Software                                 *
// *                                                                        *
// *      Copyright (c) 2015 Intel Corp.                                    *
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
// *      Data structures for DDR4 ODT Activation Table                     *
// *                                                                        *
// **************************************************************************
--*/

#ifndef ddr4_odt_activation_table_h
#define ddr4_odt_activation_table_h

#include "DataTypes.h"

struct odtActStruct ddr4OdtActTable[] = {

   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (SR_DIMM << 7)           | // slot0
   (EMPTY_DIMM << 10)       | // slot1
   (EMPTY_DIMM << 13) ,        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (EMPTY_DIMM << 10)       | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (EMPTY_DIMM << 10)       | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (EMPTY_DIMM << 10)       | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (SR_DIMM << 7)           | // slot0
   (SR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (SR_DIMM << 7)           | // slot0
   (SR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (DR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (DR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (DR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (DR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (SR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (SR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (SR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (SR_DIMM << 7)           | // slot0
   (DR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (SR_DIMM << 7)           | // slot0
   (DR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (SR_DIMM << 7)           | // slot0
   (DR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (DR_DIMM << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (DR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (DR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (DR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (SR_DIMM << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (SR_DIMM << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (SR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (SR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },

   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK2 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK3 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK2 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK3 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK1)   |
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (EMPTY_DIMM << 10)       | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (EMPTY_DIMM << 10)       | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK2 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (EMPTY_DIMM << 10)       | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK3 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (EMPTY_DIMM << 10)       | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK2 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0)   |
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK3 << 2)             | // target rank
   (AEP_DIMM_TYPE << 7)           | // slot0
   (LR_DIMM << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK1)   |
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK1 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK2 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
       // write
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0)   |
      0,
   } },
   // Config index
   { (DIMM0 << 0)             | // target dimm
   (RANK3 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
      0,
       // write
      ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK1)   |
      0,
   } },
   // Config index
   { (DIMM1 << 0)             | // target dimm
   (RANK0 << 2)             | // target rank
   (LR_DIMM << 7)           | // slot0
   (AEP_DIMM_TYPE << 10)          | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
      MCODT_ACT << 15  |
      0,
       // write
      0,
   } },
   // Config index
   { (DIMM0 << 0) |      // target dimm
   (RANK0 << 2) |        // target rank
   (QR_DIMM << 7) |      // slot0
   (EMPTY_DIMM << 10) |  // slot1
   (EMPTY_DIMM << 13),   // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     0,
     // write
     0,
   } },
   // Config index
   { (DIMM0 << 0) |      // target dimm
   (RANK1 << 2) |        // target rank
   (QR_DIMM << 7) |      // slot0
   (EMPTY_DIMM << 10) |  // slot1
   (EMPTY_DIMM << 13),   // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     0,
     // write
     0,
   } },
   // Config index
   { (DIMM0 << 0) |      // target dimm
   (RANK2 << 2) |        // target rank
   (QR_DIMM << 7) |      // slot0
   (EMPTY_DIMM << 10) |  // slot1
   (EMPTY_DIMM << 13),   // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     0,
     // write
     0,
   } },
   // Config index
   { (DIMM0 << 0) |      // target dimm
   (RANK3 << 2) |        // target rank
   (QR_DIMM << 7) |      // slot0
   (EMPTY_DIMM << 10) |  // slot1
   (EMPTY_DIMM << 13),   // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     0,
     // write
     0,
   } },
   // Config index
   { (DIMM0 << 0) |      // target dimm
   (RANK0 << 2) |        // target rank
   (QR_DIMM << 7) |      // slot0
   (QR_DIMM << 10) |     // slot1
   (EMPTY_DIMM << 13),   // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK2) |
     0,
     // write
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK2) |
     0,
   } },
   // Config index
   { (DIMM0 << 0) |      // target dimm
   (RANK1 << 2) |        // target rank
   (QR_DIMM << 7) |      // slot0
   (QR_DIMM << 10) |     // slot1
   (EMPTY_DIMM << 13),   // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK2) |
     0,
     // write
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK2) |
     0,
   } },
   // Config index
   { (DIMM0 << 0) |      // target dimm
   (RANK2 << 2) |        // target rank
   (QR_DIMM << 7) |      // slot0
   (QR_DIMM << 10) |     // slot1
   (EMPTY_DIMM << 13),   // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK2) |
     0,
     // write
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK2) |
     0,
   } },
   // Config index
   { (DIMM0 << 0) |      // target dimm
   (RANK3 << 2) |        // target rank
   (QR_DIMM << 7) |      // slot0
   (QR_DIMM << 10) |     // slot1
   (EMPTY_DIMM << 13),   // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK2) |
     0,
     // write
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM1 * MAX_RANK_DIMM + RANK2) |
     0,
   } },
   // Config index
   { (DIMM1 << 0) | // target dimm
   (RANK0 << 2) | // target rank
   (QR_DIMM << 7) | // slot0
   (QR_DIMM << 10) | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK2) |
     0,
     // write
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK2) |
     0,
   } },
   // Config index
   { (DIMM1 << 0) | // target dimm
   (RANK1 << 2) | // target rank
   (QR_DIMM << 7) | // slot0
   (QR_DIMM << 10) | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK2) |
     0,
     // write
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK2) |
     0,
   } },
   // Config index
   { (DIMM1 << 0) | // target dimm
   (RANK2 << 2) | // target rank
   (QR_DIMM << 7) | // slot0
   (QR_DIMM << 10) | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK2) |
     0,
     // write
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK2) |
     0,
   } },
   // Config index
   { (DIMM1 << 0) | // target dimm
   (RANK3 << 2) | // target rank
   (QR_DIMM << 7) | // slot0
   (QR_DIMM << 10) | // slot1
   (EMPTY_DIMM << 13),        // slot2

   // ODT activation matrix
   {   // read
     MCODT_ACT << 15 |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK2) |
     0,
     // write
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK0) |
     ODT_ACT << (DIMM0 * MAX_RANK_DIMM + RANK2) |
     0,
   } },
};

#define  MAX_DDR4_ODT_ACT_CONFIG    28

#endif //ddr4_odt_activation_table_h
