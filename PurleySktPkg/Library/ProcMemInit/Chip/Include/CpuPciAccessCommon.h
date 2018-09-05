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
#ifndef  _CPU_PCI_ACCESS_COMMON_H_
#define  _CPU_PCI_ACCESS_COMMON_H_

/**

  Indetifies the bus number for given SocId & BoxType

  @param host            - Pointer to sysHost, the system host (root) structure
  @param SocId           - CPU Socket Node number (Socket ID)
  @param BoxType         - Box Type; values come from CpuPciAccess.h
  @param BoxInst         - IIO PCIE Box Instance
  @param FuncBlk         - Function Block within IIO DFX
  @param CpuCsrAccessVar - Pointer to CSR access data

  @retval (UINT32) PCI bus number

**/
UINT32
GetBusNumber (
  PSYSHOST host,
  UINT8    SocId,
  UINT8    BoxType,
  UINT8    BoxInst,
  UINT8    FuncBlk,
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar
  );

/**

  Indetifies the device number for given SocId & BoxType

  @param host            - Pointer to sysHost, the system host (root) structure
  @param BoxType         - Box Type; values come from CpuPciAccess.h
  @param BoxInst         - IIO PCIE Box Instance
  @param FuncBlk         - Function Block within IIO DFX
  @param CpuCsrAccessVar - Pointer to CSR access data

  @retval (UINT32) PCI device number

**/
UINT32
GetDeviceNumber (
  PSYSHOST host,
  UINT8    BoxType,
  UINT8    BoxInst,
  UINT8    FuncBlk,
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar
  );

/**

  Indetifies the function number for given SocId & BoxType

  @param host            - Pointer to sysHost, the system host (root) structure
  @param BoxType         - Box Type; values come from CpuPciAccess.h
  @param BoxInst         - IIO PCIE Box Instance
  @param FuncBlk         - Function Block within IIO DFX
  @param CpuCsrAccessVar - Pointer to CSR access data

  @retval (UINT32) PCI function number

**/
UINT32
GetFunctionNumber (
  PSYSHOST host,
  UINT8    BoxType,
  UINT8    BoxInst,
  UINT8    FuncBlk,
  CPU_CSR_ACCESS_VAR *CpuCsrAccessVar
  );

#endif   // _CPU_PCI_ACCESS_COMMON_H_


