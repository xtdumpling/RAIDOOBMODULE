//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/**
// **************************************************************************
// *                                                                        *
// *      Intel Restricted Secret                                           *
// *                                                                        *
// *      Reference Code                                                    *
// *                                                                        *
// *      ESS - Enterprise Silicon Software                                 *
// *                                                                        *
// *      Copyright (c) 2004 - 2014 Intel Corp.                             *
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
// *      Data structures for DDR frequency POR                             *
// *                                                                        *
// **************************************************************************
**/
#ifndef _mempor_h
#define _mempor_h

#include "SysHost.h"
#include "SysFunc.h"

#define NOT_SUP     0xFF

struct DimmPOREntryStruct freqTable[] = {
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 1, 1, RDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 1, 1, RDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 1, 1, LRDIMM, SPD_VDD_120,  4,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 1, 1, LRDIMM, SPD_VDD_120,  8,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 1, 1, NVMDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 1, 1, NVMDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 1, RDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 1, RDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 1, LRDIMM, SPD_VDD_120,  4,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 1, LRDIMM, SPD_VDD_120,  8,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 1, NVMDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 1, NVMDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, RDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, RDIMM, SPD_VDD_120,  4,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, LRDIMM, SPD_VDD_120,  8,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, LRDIMM, SPD_VDD_120, 16,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  3,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  3,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  3,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  3,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  5,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  9,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  5,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_2S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  9,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 1, 1, RDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 1, 1, RDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 1, 1, LRDIMM, SPD_VDD_120,  4,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 1, 1, LRDIMM, SPD_VDD_120,  8,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 1, 1, NVMDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 1, 1, NVMDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 1, RDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 1, RDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 1, LRDIMM, SPD_VDD_120,  4,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 1, LRDIMM, SPD_VDD_120,  8,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 1, NVMDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 1, NVMDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, RDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, RDIMM, SPD_VDD_120,  4,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, LRDIMM, SPD_VDD_120,  8,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, LRDIMM, SPD_VDD_120, 16,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  3,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  3,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  3,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  3,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  5,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  9,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  5,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_4S, SPD_TYPE_DDR4, 2, 2, NVMDIMM, SPD_VDD_120,  9,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 1, 1, UDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 1, 1, UDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 1, 1, RDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 1, 1, RDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 2, 1, UDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 2, 1, UDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 2, 1, RDIMM, SPD_VDD_120,  1,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 2, 1, RDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 2, 2, UDIMM, SPD_VDD_120,  2,DDR_2400, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 2, 2, UDIMM, SPD_VDD_120,  4,DDR_2400, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 2, 2, RDIMM, SPD_VDD_120,  2,DDR_2666, NOT_SUP},
  {CPU_FAMILY_SKX,SOCKET_HEDT, SPD_TYPE_DDR4, 2, 2, RDIMM, SPD_VDD_120,  4,DDR_2666, NOT_SUP},
};

#pragma pack()

#endif  // _mempor_h
