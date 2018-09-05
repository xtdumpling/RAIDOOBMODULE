//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/*************************************************************************
 *
 * Memory Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2016 Intel Corporation All Rights Reserved.
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
 *
 ************************************************************************/

//
//  This file contains function subsets of BIOS SSA API functions
//

#ifndef __BIOS_SSA_FUNC__
#define __BIOS_SSA_FUNC__

#include "SysFunc.h"

#ifdef SSA_FLAG

//RC SIM has MINIBIOS_BUILD defined: 'Stitched SSA code' - Included in RC Sim & excluded from the Minibios
#ifdef MINIBIOS_BUILD
#else
#define EV_STITCHED_CONTENT_ENBL  //Not skipping code for the full bios
#endif //MINIBIOS_BUILD


//
// BiosSsaMemoryServerConfig.c
//
VOID    SSAGetLogicalRank (PSYSHOST host, UINT8 Socket, UINT8 Channel, UINT8 Dimm, UINT8 Rank, UINT8 *logicalRankMapping);
//
// BiosSsaMemoryConfig.c
//
UINT8   SSAGetChannelInSocket (UINT8 Controller, UINT8 Channel);

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG

#endif // __BIOS_SSA_FUNC__
