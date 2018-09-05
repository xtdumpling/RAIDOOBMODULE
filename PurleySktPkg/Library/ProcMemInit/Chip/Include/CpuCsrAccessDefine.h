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
 *
 ************************************************************************/
#ifndef  _CPU_CSR_ACCESS_DEFINE_H_
#define  _CPU_CSR_ACCESS_DEFINE_H_
//#include <Library/CsrToPcieAddress.h>
#include <SysFunc.h>
#include <CsrToPcieAddress.h>
#include <CpuPciAccessCommon.h>


typedef enum {
  BUS_CLASS = 0,
  DEVICE_CLASS = 1,
  FUNCTION_CLASS = 2
} BDF_CLASS;

UINT32
GetSegmentNumber (
  IN USRA_ADDRESS    *Address
  );

UINT32
GetBDFNumber (
  IN USRA_ADDRESS    *Address,
  CPU_CSR_ACCESS_VAR          *CpuCsrAccessVar,
  IN UINT8                    BDFType
  );

UINT32
GetCpuCsrAddress (
  UINT8    SocId,
  UINT8    BoxInst,
  UINT32   Offset,
  UINT8    *Size
  );

UINT32
GetMmcfgAddress(
  PSYSHOST host
  );

VOID
GetCpuCsrAccessVar_RC (
  PSYSHOST host,
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar
  );

VOID
EnableCsrAddressCache(
  struct sysHost             *host
);

VOID
DisableCsrAddressCache(
  struct sysHost             *host
);

#endif   // _CPU_CSR_ACCESS_DEFINE_H_
