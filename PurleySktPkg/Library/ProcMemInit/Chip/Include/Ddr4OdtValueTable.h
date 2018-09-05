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
// *      Data structures for DDR4 ODT Value Table                     *
// *                                                                        *
// **************************************************************************
--*/


#ifndef ddr4_odt_value_table_h
#define ddr4_odt_value_table_h

#include "DataTypes.h"

struct ddr4OdtValueStruct ddr4OdtValueTable[] = {

   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      74,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_48, // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      74,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_48, // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      74,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_48, // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      74,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_48, // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      74,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_48, // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_60, // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_60, // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_60, // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_60, // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      75,                                 // MC Vref percent
      71,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_60, // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_40 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_40 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_40 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_40 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_40 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_40 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_40 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_40 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (EMPTY_DIMM << 10)    |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      77,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_40 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_40 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      77,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_34,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_34,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      77,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_34,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_34,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      77,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_34,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_34,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      77,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_34,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_34,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      77,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_34,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_48 << 3)   | DDR4_RTT_NOM_34,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      85,                                 // MC Vref percent
      82,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      85,                                 // MC Vref percent
      82,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      85,                                 // MC Vref percent
      82,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      85,                                 // MC Vref percent
      82,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      85,                                 // MC Vref percent
      82,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      79,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_34,               // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      79,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_34,               // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      79,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_34,               // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      79,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_34,               // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      79,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_34,               // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      79,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      79,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      79,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      79,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      79,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_120 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,              // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      81,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      81,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      81,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      81,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (DR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      81,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      81,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,               // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      81,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      81,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      81,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (DR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      84,                                 // MC Vref percent
      81,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_60 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank0
             (DDR4_RTT_WR_240 << 6)   | (DDR4_RTT_NOM_60 << 3)   | DDR4_RTT_NOM_240,              // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,                // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (SR_DIMM << 7)        |             // slot0
      (LR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      78,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_1866 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      77,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2133 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      77,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2400 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      77,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2666 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      77,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
      // Config index
      (DDR_2933 << 0)       |             // freq
      (LR_DIMM << 7)        |             // slot0
      (SR_DIMM << 10)       |             // slot1
      (EMPTY_DIMM << 13),                 // slot2
      MCODT_50,                           // MC ODT encoding
      83,                                 // MC Vref percent
      77,                                 // DRAM Vref percent
      {
         { // dimm0
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm1
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_80 << 6)   | (DDR4_RTT_NOM_34 << 3)   | DDR4_RTT_NOM_DIS,  // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         },
         { // dimm2
             // Rtt_wr              Rtt_prk               Rtt_nom
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank0
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank1
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS,             // rank2
             (DDR4_RTT_WR_DIS << 6)   | (DDR4_RTT_NOM_DIS << 3)   | DDR4_RTT_NOM_DIS              // rank3
         }
      }
   },
   {
     // Config index
     (DDR_1866 << 0) |            // freq
     (QR_DIMM << 7) |             // slot0
     (EMPTY_DIMM << 10) |         // slot1
     (EMPTY_DIMM << 13),          // slot2
     MCODT_50,                    // MC ODT encoding
     77,                          // MC Vref percent
     74,                          // DRAM Vref percent
     {
       { // dimm0
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm1
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       },
       { // dimm2
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       }
     }
   },
   {
     // Config index
     (DDR_2133 << 0) |            // freq
     (QR_DIMM << 7) |             // slot0
     (EMPTY_DIMM << 10) |         // slot1
     (EMPTY_DIMM << 13),          // slot2
     MCODT_50,                    // MC ODT encoding
     77,                          // MC Vref percent
     74,                          // DRAM Vref percent
     {
       { // dimm0
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm1
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       },
       { // dimm2
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       }
     }
   },
   {
     // Config index
     (DDR_2400 << 0) |            // freq
     (QR_DIMM << 7) |             // slot0
     (EMPTY_DIMM << 10) |         // slot1
     (EMPTY_DIMM << 13),          // slot2
     MCODT_50,                    // MC ODT encoding
     77,                          // MC Vref percent
     74,                          // DRAM Vref percent
     {
       { // dimm0
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm1
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       },
       { // dimm2
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       }
     }
   },
   {
     // Config index
     (DDR_2666 << 0) |            // freq
     (QR_DIMM << 7) |             // slot0
     (EMPTY_DIMM << 10) |         // slot1
     (EMPTY_DIMM << 13),          // slot2
     MCODT_50,                    // MC ODT encoding
     77,                          // MC Vref percent
     74,                          // DRAM Vref percent
     {
       { // dimm0
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm1
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       },
       { // dimm2
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       }
     }
   },
   {
     // Config index
     (DDR_2933 << 0) |            // freq
     (QR_DIMM << 7) |             // slot0
     (EMPTY_DIMM << 10) |         // slot1
     (EMPTY_DIMM << 13),          // slot2
     MCODT_50,                    // MC ODT encoding
     77,                          // MC Vref percent
     74,                          // DRAM Vref percent
     {
       { // dimm0
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm1
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       },
       { // dimm2
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       }
     }
   },
   {
     // Config index
     (DDR_1866 << 0) |            // freq
     (QR_DIMM << 7) |             // slot0
     (QR_DIMM << 10) |            // slot1
     (EMPTY_DIMM << 13),          // slot2
     MCODT_50,                    // MC ODT encoding
     85,                          // MC Vref percent
     81,                          // DRAM Vref percent
     {
       { // dimm0
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_80,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_80,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm1
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_120,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_120,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm2
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       }
     }
   },
   {
     // Config index
     (DDR_2133 << 0) |            // freq
     (QR_DIMM << 7) |             // slot0
     (QR_DIMM << 10) |            // slot1
     (EMPTY_DIMM << 13),          // slot2
     MCODT_50,                    // MC ODT encoding
     85,                          // MC Vref percent
     81,                          // DRAM Vref percent
     {
       { // dimm0
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_80,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_80,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm1
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_120,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_120,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm2
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       }
     }
   },
   {
     // Config index
     (DDR_2400 << 0) |            // freq
     (QR_DIMM << 7) |             // slot0
     (QR_DIMM << 10) |            // slot1
     (EMPTY_DIMM << 13),          // slot2
     MCODT_50,                    // MC ODT encoding
     85,                          // MC Vref percent
     81,                          // DRAM Vref percent
     {
       { // dimm0
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_80,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_80,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm1
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_120,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_120,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm2
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       }
     }
   },
   {
     // Config index
     (DDR_2666 << 0) |            // freq
     (QR_DIMM << 7) |             // slot0
     (QR_DIMM << 10) |            // slot1
     (EMPTY_DIMM << 13),          // slot2
     MCODT_50,                    // MC ODT encoding
     85,                          // MC Vref percent
     81,                          // DRAM Vref percent
     {
       { // dimm0
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_80,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_80,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm1
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_120,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_120,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm2
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       }
     }
   },
   {
     // Config index
     (DDR_2933 << 0) |            // freq
     (QR_DIMM << 7) |             // slot0
     (QR_DIMM << 10) |            // slot1
     (EMPTY_DIMM << 13),          // slot2
     MCODT_50,                    // MC ODT encoding
     85,                          // MC Vref percent
     81,                          // DRAM Vref percent
     {
       { // dimm0
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_80,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_80,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm1
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_120,  // rank0
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS,  // rank1
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_120,  // rank2
         (DDR4_RTT_WR_240 << 6) | (DDR4_RTT_NOM_240 << 3) | DDR4_RTT_NOM_DIS   // rank3
       },
       { // dimm2
         // Rtt_wr              Rtt_prk               Rtt_nom
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank0
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank1
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS, // rank2
         (DDR4_RTT_WR_DIS << 6) | (DDR4_RTT_NOM_DIS << 3) | DDR4_RTT_NOM_DIS  // rank3
       }
     }
   },
};

#define  MAX_DDR4_ODT_VALUE_CONFIG    60

#endif //ddr4_odt_value_table_h
