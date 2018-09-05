//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

   Reference Code

   ESS - Enterprise Silicon Software

   INTEL CONFIDENTIAL

@copyright
  Copyright 2016 -  Intel Corporation All Rights Reserved.

  The source code contained or described herein and all documents
  related to the source code ("Material") are owned by Intel
  Corporation or its suppliers or licensors. Title to the Material
  remains with Intel Corporation or its suppliers and licensors.
  The Material contains trade secrets and proprietary and confidential
  information of Intel or its suppliers and licensors. The Material
  is protected by worldwide copyright and trade secret laws and treaty
  provisions.  No part of the Material may be used, copied, reproduced,
  modified, published, uploaded, posted, transmitted, distributed, or
  disclosed in any way without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other
  intellectual property right is granted to or conferred upon you
  by disclosure or delivery of the Materials, either expressly,
  by implication, inducement, estoppel or otherwise. Any license
  under such intellectual property rights must be express and
  approved by Intel in writing.

@file
  MrcHooksChipServicesPpi.h

@brief
  This file defines an MRC Hooks Chip Layer Services PPI to be called by OEM hook function
  implementations.  This is a short term compatibility design.
  This allows OEM hooks that were previously linked with MRC source directly
  to now link with a library that calls the same MRC functions via a PPI.

  Long-term, Implementations of the OEM hooks should use more standard 
  UEFI capabilities and this PPI will be obsoleted.
  It is expected that the use of functions will expand as we manage change
  in a compatibility focused manner.
  Please limit your use of these capabilities in preference of more standard
  options like standard PCI IO PPI and common MdePkg libraries, standard BaseLib
  string functions, etc.

**/
#ifndef _MRC_HOOKS_CHIP_SERVICES_PPI_H_
#define _MRC_HOOKS_CHIP_SERVICES_PPI_H_

///
/// PPI revision information
/// This PPI will be extended in a backwards compatible manner over time
/// Added interfaces should be documented here with the revisions added
/// Revision 1:  Initial revision
///
#define MRC_HOOKS_CHIP_SERVICES_PPI_REVISION  1

typedef struct _MRC_HOOKS_CHIP_SERVICES_PPI MRC_HOOKS_CHIP_SERVICES_PPI;
extern EFI_GUID gMrcHooksChipServicesPpiGuid;

typedef 
UINT32 
(EFIAPI *READ_CPU_PCI_CFG_EX) (
  SYSHOST  *Host,
  UINT8    SocId,
  UINT8    BoxInstance,
  UINT32   Offset
  );

typedef
VOID
(EFIAPI *WRITE_CPU_PCI_CFG_EX) (
  SYSHOST  *Host,
  UINT8    SocId,
  UINT8    BoxInstance,
  UINT32   Offset,
  UINT32   Data
  );

typedef
KTI_STATUS
(EFIAPI *SEND_MAIL_BOX_CMD_TO_PCODE) (
  SYSHOST             *Host,
  UINT8               Cpu,
  UINT32              Command,
  UINT32              Data
  );

typedef
VOID
(EFIAPI *FATAL_ERROR) (
  SYSHOST         *Host,
  UINT8           MajorCode,
  UINT8           MinorCode
  );

typedef
VOID
(EFIAPI *DEBUG_PRINT_MEM_FUNCTION) (
  SYSHOST    *Host,
  UINT32     DbgLevel,
  UINT8      Socket,
  UINT8      Channel,
  UINT8      Dimm,
  UINT8      Rank,
  UINT8      Strobe,
  UINT8      Bit,
  INT8       *Format,
  INT8       *Marker
  );

struct _MRC_HOOKS_CHIP_SERVICES_PPI {
  //
  // This member specifies the revision of the MRC_HOOKS_CHIP_SERVICES_PPI. This field is used to
  // indicate backwards compatible changes to the PPI. Platform code that produces
  // this PPI must fill with the correct revision value for MRC code
  // to correctly interpret the content of the PPI fields.
  //
  UINT32                                Revision;
  READ_CPU_PCI_CFG_EX                   ReadCpuPciCfgEx;                   
  WRITE_CPU_PCI_CFG_EX                  WriteCpuPciCfgEx;
  SEND_MAIL_BOX_CMD_TO_PCODE            SendMailBoxCmdToPcode;
  FATAL_ERROR                           FatalError;
  DEBUG_PRINT_MEM_FUNCTION              DebugPrintMemFunction;
};

#endif  //_MRC_HOOKS_CHIP_SERVICES_PPI_
