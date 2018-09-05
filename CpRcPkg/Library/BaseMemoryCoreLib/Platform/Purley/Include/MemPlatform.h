//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 ************************************************************************
 *
 *  PURPOSE:
 *
 *      This file contains memory detection and initialization for
 *      IMC and DDR3 modules compliant with JEDEC specification.
 *
 ************************************************************************/

#ifndef  _mem_platform_h
#define  _mem_platform_h

#include "DataTypes.h"
#include "MemPlatformCommon.h"

#ifdef   SERIAL_DBG_MSG
#define  MRC_TRACE  1
#endif


//
// Compatible BIOS Data Structure
//
#define BDAT_SUPPORT    1  //Memory Data Schema 4 and RMT Schema 5 of BDAT 4.0

//
// QR support
//
#define  QR_DIMM_SUPPORT 1

//
// Define to enable DIMM margin checking
//
#define  MARGIN_CHECK   1

//
// Define to enable SODIMM module support
//
#define  SODIMM_SUPPORT    1

//
// Define to enable ME UMA support
//
//#define ME_SUPPORT_FLAG   1

//
// Define to enable XMP
//
#define XMP_SUPPORT     1

// Define to enable DEBUG for NVMCTLR (LATE CMD CLK)
//#define DEBUG_LATECMDCLK      1

// Define to enable MRS Stacking
//#define MRS_STACKING    1

//
// Define to max ppr
//
#define MAX_PPR_ADDR_ENTRIES           20

//
//-------------------------------------
// DVP Platform-specific defines
//-------------------------------------
//
#ifdef   DVP_PLATFORM
#endif   // DVP_PLATFORM

//
//-------------------------------------
// CRB Platform-specific defines
//-------------------------------------
//
#ifdef   CRB_PLATFORM
#endif   // CRB_PLATFORM

#ifndef MAX_HA
#define MAX_HA              2                   // Number of Home Agents / IMCs
#endif

#ifdef BDAT_SUPPORT
#ifdef SSA_FLAG
#define MAX_NUMBER_SSA_BIOS_RESULTS    2  //i.e. max. no. of GUIDS: 1.) For BIOS SSA tools  2.) Stitched in RMT results
#endif //SSA_FLAG
#endif //BDAT_SUPPORT

//SKX_TODO: I have removed NonPOR elements, I will delete this line before submit

#endif   // _mem_platform_h
