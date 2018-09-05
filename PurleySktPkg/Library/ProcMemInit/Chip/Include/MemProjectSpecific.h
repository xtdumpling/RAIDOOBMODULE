/**
 This file contains an 'Intel Pre-EFI Module' and is licensed
 for Intel CPUs and Chipsets under the terms of your license
 agreement with Intel or your vendor.  This file may be
 modified by the user, subject to additional terms of the
 license agreement
**/
/*************************************************************************
 *
 * Memory Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2016 Intel Corporation All Rights Reserved.
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

#include "SysFunc.h"
#include "Cpgc.h"

//
// Place any project specific defines or structures here.
//
#define MAX_STROBE_IMODE  (MAX_STROBE)

#define FNV_TXVREF_DEFAULT  0x54
#define FNV_CMDVREF_DEFAULT 0x22

//------------------------------------------------------------------------------------------

#ifndef MEM_WR_LEVELINGHOOKS_H
#define MEM_WR_LEVELINGHOOKS_H

#define CHIP_TX_PI_SAMPLE_YAMDELAY 180*16

#define CHIP_CPG_MISCODTCTL_MCDDC_CTL_STRUCT CPGC_MISCODTCTL_MCDDC_CTL_STRUCT

#endif //MEM_WR_LEVELINGHOOKS_H

//------------------------------------------------------------------------------------------
#define DDRT_SUPPORTED_FREQUENCIES 5
extern UINT8 DdrtCASLatencyRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 DdrtCASLatencyAdderRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 DdrtCASWriteLatencyRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 DdrtCASWriteLatencyAdderRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 DdrtCASLatencyLRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 DdrtCASLatencyAdderLRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 DdrtCASWriteLatencyLRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 DdrtCASWriteLatencyAdderLRDIMM[DDRT_SUPPORTED_FREQUENCIES];

extern UINT8 EkvDdrtCASLatencyRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 EkvDdrtCASLatencyAdderRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 EkvDdrtCASWriteLatencyRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 EkvDdrtCASWriteLatencyAdderRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 EkvDdrtCASLatencyLRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 EkvDdrtCASLatencyAdderLRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 EkvDdrtCASWriteLatencyLRDIMM[DDRT_SUPPORTED_FREQUENCIES];
extern UINT8 EkvDdrtCASWriteLatencyAdderLRDIMM[DDRT_SUPPORTED_FREQUENCIES];
#ifndef MEM_CMD_CLKHOOKS_H
#define MEM_CMD_CLKHOOKS_H


//SKX change
#define TX_RON_28 28   
#define TX_RON_30 30
#define TX_RON_33 33

typedef struct _CHIP_CLEAR_PARITY_RESULTS_STRUCT
{
  DUMMY_REG                             tCrap;
  DUMMY_REG                             errSignals;
}CHIP_CLEAR_PARITY_RESULTS_STRUCT, *PCHIP_CLEAR_PARITY_RESULTS_STRUCT;


#define CPGC_MISCODTCTL_MCDDC_CHIP_STRUCT CPGC_MISCODTCTL_MCDDC_CTL_STRUCT

#endif //MEM_CMD_CLKHOOKS_H

MRC_STATUS PopulateMemChipPolicy (PSYSHOST host);

//-----------------------------------------------------------------------
