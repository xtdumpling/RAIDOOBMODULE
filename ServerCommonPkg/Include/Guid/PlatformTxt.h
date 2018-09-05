//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license 
// agreement with Intel or your vendor.  This file may be      
// modified by the user, subject to additional terms of the    
// license agreement                                           
//
/*++

Copyright (c)  2007 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformTxt.h
    
Abstract:

  GUID used for Platform Txt information in the HOB list.

--*/

#ifndef _EFI_PLATFORM_TXT_H_
#define _EFI_PLATFORM_TXT_H_

#include <Uefi.h>


extern EFI_GUID gEfiPlatformTxtDeviceMemoryGuid;

extern EFI_GUID gEfiPlatformTxtPolicyDataGuid;

//
// TXT Device Memory
//
// Platform MUST expose this Hob and Size field should be page-aligned non-zero.
// If Address field is not zero, TXT driver will use it,
// or TXT driver will allocate 1M aligned memory for it.
// NOTE: DMA protection memory region address top is just below TSEG. Zero-Address
// means no DPR support in platform.
//
typedef struct {
  EFI_PHYSICAL_ADDRESS     NoDMATableAddress;
  UINT64                   NoDMATableSize;
  EFI_PHYSICAL_ADDRESS     SINITMemoryAddress;
  UINT64                   SINITMemorySize;
  EFI_PHYSICAL_ADDRESS     TXTHeapMemoryAddress;
  UINT64                   TXTHeapMemorySize;
  EFI_PHYSICAL_ADDRESS     DMAProtectionMemoryRegionAddress;
  UINT64                   DMAProtectionMemoryRegionSize;
} EFI_PLATFORM_TXT_DEVICE_MEMORY;

//
// Policy
//
// Platform MUST expose this Hob. BIOSAcm field should not be zero.
//
typedef struct {
  //
  // ACM information
  //
  EFI_PHYSICAL_ADDRESS     BiosAcmAddress;
  EFI_PHYSICAL_ADDRESS     StartupAcmAddressInFit;
  EFI_PHYSICAL_ADDRESS     StartupAcmAddress;
  //
  // Misc Policy
  //
  UINT32                   MiscPolicy;
  //
  // BiosOs region. It should be at least 2 for LCP PD.
  //
  UINT32                   BiosOsDataRegionRevision;
  EFI_PHYSICAL_ADDRESS     LcpPolicyDataBase;
  UINT64                   LcpPolicyDataSize;
  //
  // 64-bits TXT Scratch Register
  //
  EFI_PHYSICAL_ADDRESS     TxtScratchAddress;
  //
  // Bios ACM Policy
  //
  UINT32                   BiosAcmPolicy;
  //
  // BiosOsData (SINIT consumption, chipset specific)
  //
  UINT64                   FlagVariable;
  //
  // DPR Lock
  //
//  UINT32                   DprLock;  
  //
  // Clear TPM Aux Index
  //
  UINT8                    TxtClearAuxIndex;
} EFI_PLATFORM_TXT_POLICY_DATA;

//
// BIOS ACM policy
//
#define EFI_PLATFORM_TXT_POLICY_BIOS_ACM_CHECK_REGISTERED_VERSION  0x01

//
// Misc policy
//

//
// SCHECK should be performed in TXT BIOS spec 1.0.
// However, some platform does not need this.
//
#define EFI_PLATFORM_TXT_POLICY_MISC_NO_SCHECK_IN_RESUME 0x1
//
// Some platform need not programming NODMA table, it uses
// chipset-specific DMA protection memory region.
// If this flag is set, no NODMA register will not be touched.
//
#define EFI_PLATFORM_TXT_POLICY_MISC_NO_NODMA_TABLE      0x2

//
// Note: User can also use below macro to disable the feature
// at build time. The same meaning as above.
//
//#define EFI_PLATFORM_TXT_POLICY_NO_SCHECK_IN_RESUME
// Enable the line below for the LTSX support. Need to debug this, due to effect on LT data area setting. -wesam
#define EFI_PLATFORM_TXT_POLICY_NO_NODMA_TABLE

// Enable the LTSX flag. This will be a super set of the LT feature. 
// In here, some of the LT features will be used, but other featurs 
// will need to be disabled. So use this flag to control this at build time

#define EFI_PLATFORM_TXT_SERVER_EXTENSION
#pragma pack (push, 1)
typedef struct {
  UINT32                   Revision;
  UINT32                   BiosSinitSize;
} EFI_TXT_BIOS_OS_DATA_REGION_V1;

typedef struct {
  UINT32                   Revision;
  UINT32                   BiosSinitSize;
  UINT64                   LcpPolicyDataBase;
  UINT64                   LcpPolicyDataSize;
  UINT32                   NumberOfLogicalProcessors;
  UINT64                   FlagVariable;
} EFI_TXT_BIOS_OS_DATA_REGION_V3;

typedef struct {
  UINT32                   Revision;
  UINT32                   BiosSinitSize;
  UINT64                   LcpPolicyDataBase;
  UINT64                   LcpPolicyDataSize;

  UINT32                   NumberOfLogicalProcessors;
} EFI_TXT_BIOS_OS_DATA_REGION_V2;
#pragma pack (pop)
#endif
