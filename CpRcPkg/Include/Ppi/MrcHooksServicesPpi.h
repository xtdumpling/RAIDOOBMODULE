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
  MrcHooksServicesPpi.h

@brief
  This file defines an MRC Hooks Services PPI to be called by OEM hook function
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
#ifndef _MRC_HOOKS_SERVICES_PPI_H_
#define _MRC_HOOKS_SERVICES_PPI_H_

///
/// PPI revision information
/// This PPI will be extended in a backwards compatible manner over time
/// Added interfaces should be documented here with the revisions added
/// Revision 1:  Initial revision
///
#define MRC_HOOKS_SERVICES_PPI_REVISION  1

typedef struct _MRC_HOOKS_SERVICES_PPI MRC_HOOKS_SERVICES_PPI;
extern EFI_GUID gMrcHooksServicesPpiGuid;

typedef
UINT32
(EFIAPI *INITIALIZE_USB2_DEBUG_PORT) (
  SYSHOST  *Host
  );

typedef
UINT32
(EFIAPI *DISCOVER_USB2_DEBUG_PORT_API) (
  SYSHOST  *Host
  );  

typedef
UINT32
(EFIAPI *RC_VPRINTF) (
  SYSHOST    *Host,
  CONST INT8 *Format,
  INT8       *Marker
  );

typedef
VOID
(EFIAPI *WRITE_MSR_PIPE) (
  SYSHOST        *Host,
  UINT8          Socket,
  UINT32         Msr,
  UINT64_STRUCT  MsrReg
  );

typedef
UINT32
(EFIAPI *SET_PROMOTE_WARNING_EXCEPTION) (
  SYSHOST  *Host,
  UINT8    MajorCode,
  UINT8    MinorCode
  );

typedef
UINT32
(EFIAPI *READ_SMB) (
  SYSHOST          *Host,
  UINT8            Socket,
  struct smbDevice Device,
  UINT8            ByteOffset,
  UINT8            *Data
  );

typedef
UINT32
(EFIAPI *WRITE_SMB) (
  SYSHOST          *Host,
  UINT8            Socket,
  struct smbDevice Device,
  UINT8            ByteOffset,
  UINT8            *Data
  );

typedef
VOID
(EFIAPI *OUTPUT_CHECKPOINT) (
  SYSHOST             *Host,
  UINT8               MajorCode,
  UINT8               MinorCode,
  UINT16              Data
  );

typedef
VOID
(EFIAPI *LOG_WARNING) (
  SYSHOST         *Host,
  UINT8           MajorCode,
  UINT8           MinorCode,
  UINT32          LogData
  );

typedef
VOID
(EFIAPI *OUTPUT_WARNING) (
  SYSHOST         *Host,
  UINT8           MajorCode,
  UINT8           MinorCode,
  UINT8           socket,
  UINT8           ch,
  UINT8           dimm,
  UINT8           rank
  );

typedef
UINT32
(EFIAPI *READ_SPD) (
  SYSHOST          *Host,
  UINT8            Socket,
  UINT8            Channel,
  UINT8            Dimm,
  UINT16           ByteOffset,
  UINT8            *Data
  );

typedef
VOID
(EFIAPI *FIXED_DELAY_MEM) (
  SYSHOST *host,
  UINT32   usDelay
);

struct _MRC_HOOKS_SERVICES_PPI {
  //
  // This member specifies the revision of the MRC_HOOKS_SERVICES_PPI. This field is used to
  // indicate backwards compatible changes to the PPI. Platform code that produces
  // this PPI must fill with the correct revision value for MRC code
  // to correctly interpret the content of the PPI fields.
  //
  UINT32                                Revision;
  INITIALIZE_USB2_DEBUG_PORT            InitializeUsb2DebugPort;
  DISCOVER_USB2_DEBUG_PORT_API          DiscoverUsb2DebugPort;
  RC_VPRINTF                            rcVprintf;
  WRITE_MSR_PIPE                        WriteMsrPipe;
  SET_PROMOTE_WARNING_EXCEPTION         SetPromoteWarningException;
  READ_SMB                              ReadSmb;
  WRITE_SMB                             WriteSmb;
  OUTPUT_CHECKPOINT                     OutputCheckpoint;
  LOG_WARNING                           LogWarning;
  OUTPUT_WARNING                        OutputWarning;
  READ_SPD                              ReadSpd;
  FIXED_DELAY_MEM                       FixedDelayMem;
};

#endif  //_MRC_HOOKS_SERVICES_PPI_
