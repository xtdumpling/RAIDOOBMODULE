//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.00
//   Bug Fix:  1.Add Manufacturing Date in Part number field 's tail (date :Year/Week) of Smbios Type 17.  
//   Reason:   
//   Auditor:  Chen Lin
//   Date:     Jul/01/2016
//
//***************************************************************************
//
//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file MemoryMapData.h

  GUID used for Memory Map Data entries in the HOB list.

**/

#ifndef _MEMORY_MAP_GUID_H_
#define _MEMORY_MAP_GUID_H_

#include "SysHost.h"
#include "UncoreCommonIncludes.h"
#include "PartialMirrorGuid.h"
#include "Token.h" // Supermicro
// {F8870015-6994-4b98-95A2-BD56DA91C07F}
#define EFI_MEMORY_MAP_GUID \
  { \
    0xf8870015,0x6994,0x4b98,0x95,0xa2,0xbd,0x56,0xda,0x91,0xc0,0x7f \
  }

extern EFI_GUID gEfiMemoryMapGuid;
extern CHAR16   EfiMemoryMapVariable[];

//
//  System Memory Map HOB information
//

#pragma pack(1)
struct RankDevice {
  UINT8   enabled;                // 0 = disabled,  1 = enabled
  UINT8   logicalRank;            // Logical Rank number (0 - 7)
  UINT16  rankSize;               // Units of 64 MB
};

struct PersisentDpaMap
{
    UINT32 perRegionDPAOffset;
    UINT32 SPALimit;
};

struct DimmDevice {
  UINT8  Present;
  BOOLEAN Enabled;
  UINT8  aepDimmPresent;     // 1 - This is a AEP Dimm
  UINT8  X4Present;
  UINT8  NumRanks;
  UINT8  keyByte;
  UINT8  actKeyByte2;        // Actual module type reported by SPD
  UINT8  actSPDModuleOrg;    // Actual number of DRAM ranks and device width
  UINT8  actSPDSDRAMBanks;   // Actual number of banks and DRAM density
  UINT8  SPDMemBusWidth;     // Width of the Primary bus and extension
  UINT8  dimmTs;             // Thermal sensor data.
  BOOLEAN newDimm;           // 0 - DIMM is not new to the system for this boot 1 - DIMM is new to the system for this boot (AEP DIMM only)
  UINT16 nonVolCap;          // Non-volatile capacity (AEP DIMM only)
  UINT16 DimmSize;
  UINT32 NVmemSize;
  UINT16 SPDMMfgId;          // Module Mfg Id from SPD
// Supermicro +
#if SMCPKG_SUPPORT
    UINT16 SPDModDate;		// Module Manufacturing Date SMC R1.00
#endif
// Supermicro - 
  UINT16 SPDSDRAMBanks;
  UINT16 VendorID;
  UINT16 DeviceID;
  UINT16 RevisionID;
  UINT32 perRegionDPA;       // DPA of PMEM that Nfit needs
  struct PersisentDpaMap perDPAMap[MAX_UNIQUE_NGN_DIMM_INTERLEAVE]; // DPA map
  UINT8  manufacturer[NGN_MAX_MANUFACTURER_STRLEN];  // Manufacturer
  UINT8  serialNumber[NGN_MAX_SERIALNUMBER_STRLEN];  // Serial Number
  UINT8  modelNumber[NGN_MAX_MODELNUMBER_STRLEN];    // Model Number
  struct RankDevice  rankList[MAX_RANK_DIMM];
  UINT16 InterfaceFormatCode;
  UINT16 subSysVendorID;
  UINT16 subSysDeviceID;
  UINT16 subSysRevisionID;
  UINT8  manufacturingLocation;  // Manufacturing location for the NVDIMM
  UINT16 manufacturingDate;      // Date the NVDIMM was manufactured
  INT32  commonTck;
  BOOLEAN IsNvDimm;
  UINT8   EnergyType;     // 0: 12V aux power; 1: dedicated backup energy source; 2: no backup energy source
  UINT8   nvCtrlSmbAddr;  // NVDIMM SMBUS ADDRESS
  UINT16  funcdesc;       // NVDIMM FUNCTION DESCRIPTOR
  UINT32  nvDimmStatus;   // NVDIMMSTATUS 
//APTIOV_SERVER_OVERRIDE_RC_START
  //RSD type 197 Memory Ext Info  
  struct firmwareRev firmwareRevision; /* Firmware Revision */  
  UINT8  ApiVersion;
  UINT16  MaxTdp;
//APTIOV_SERVER_OVERRIDE_RC_END
};

struct ChannelDevice {
  UINT8 Enabled;                            // 0 = channel disabled, 1 = channel enabled
  UINT8 Features;                           // Bit mask of features to enable or disable
  UINT8 MaxDimm;                            // Number of DIMM
  UINT8 NumRanks;                           // Number of ranks on this channel
  UINT8 chFailed;
  UINT8 ngnChFailed;
  UINT8 SpareLogicalRank[MAX_SPARE_RANK];    // Logical rank, selected as Spare
  UINT8 SparePhysicalRank[MAX_SPARE_RANK];   // Physical rank, selected as spare
  UINT16 SpareRankSize[MAX_SPARE_RANK];      // spare rank size
  UINT8 EnabledLogicalRanks; // Bitmap of Logical ranks that are enabled
  UINT16 rirLimit[MAX_RIR];  // Rank Interleave Register Limits
  UINT8  rirWay[MAX_RIR];    // Rank Interleave Register Ways
  UINT8  rirValid[MAX_RIR];  // RIR valid bit
  UINT32 rirOffset[MAX_RIR][MAX_RIR_WAYS];
  UINT8  rirIntList[MAX_RIR][MAX_RIR_WAYS];
  struct DimmDevice  DimmInfo[MAX_DIMM];
};

struct memcontroller {
  UINT8    TADintList[TAD_RULES][MAX_TAD_WAYS];     // TAD interleave list for node
  UINT8    TADChnIndex[TAD_RULES][MAX_TAD_WAYS];    // Corresponding TAD channel indexes
  UINT32   TADOffset[TAD_RULES][MAX_TAD_WAYS];      // Corresponding TAD offsets
  struct   TADTable TAD[TAD_RULES];                 // TAD table
  UINT32   MemSize;
  BOOLEAN  IsMapped;                                // Is this node mapped to system address space
  UINT64   WpqFlushHintAddress;
};

struct socket {
  UINT8    imcEnabled[MAX_IMC];
  UINT8    SADintList[SAD_RULES][MC_MAX_NODE];     // SAD interleave list
  struct   SADTable SAD[SAD_RULES];                 // SAD table
  struct   memcontroller imc[MAX_IMC];
  struct   ChannelDevice  ChannelInfo[MAX_CH];
};

struct SystemMemoryMapElement {
  UINT8    NodeId;         // Node ID of the HA Owning the memory
  UINT8    SocketId;       // Socket Id of socket that has his memory - ONLY IN NUMA
  UINT8    ImcInterBitmap; // IMC interleave bitmap for this DRAM rule - ONLY IN NUMA
  UINT32   BaseAddress;    // Base Address of the element in 64MB chunks
  UINT32   ElementSize;    // Size of this memory element in 64MB chunks
};

struct SystemMemoryMapHob {
  UINT32   lowMemBase;     // Mem base in 64MB units for below 4GB mem.
  UINT32   lowMemSize;     // Mem size in 64MB units for below 4GB mem.
  UINT32   highMemBase;    // Mem base in 64MB units for above 4GB mem.
  UINT32   highMemSize;    // Mem size in 64MB units for above 4GB mem.
  UINT32   memSize;      // Total physical memory size
  UINT16   memFreq;      // Mem Frequency
  UINT8    memMode;      // 0 - Independent, 1 - Lockstep
  UINT8    DimmType;
  UINT16   DramType;
  UINT8    DdrVoltage;
  UINT8    AepDimmPresent; // If at least one Aep Dimm Present (used by Nfit), then this should get set
  UINT8    SADNum;
  UINT8    XMPProfilesSup;
  UINT8    cpuType;
  UINT8    cpuStepping;
  UINT8    SystemRasType;
  UINT8    RasModesEnabled; // RAS modes that are enabled
  UINT8    ExRasModesEnabled; // Extended RAS modes that are enabled
  UINT8    RasModesSupported; //RAS modes that are supported by current memory population.
  UINT8    sncEnabled;      // 0 - SNC disabled for this configuration, 1 - SNC enabled for this configuration
  UINT8    NumOfCluster;
  UINT8    NumChPerMC;
  UINT8    numberEntries;   // Number of Memory Map Elements
  UINT8    maxIMC;
  UINT8    maxCh;
  struct   SystemMemoryMapElement Element[MAX_SOCKET * SAD_RULES];
  struct   memSetup MemSetup;
  struct   socket Socket[MAX_SOCKET];
  struct   memTiming  profileMemTime[2];
  struct   warningLog status;
  UINT32   WarningLog[MAX_WARNING_LOGS];
#ifdef BDAT_SUPPORT
#ifdef SSA_FLAG
  UINT32    bssaBdatSize;
  UINT8     bssaNumberHobs[MAX_NUMBER_SSA_BIOS_RESULTS];
  EFI_GUID  bssaBdatGuid[MAX_NUMBER_SSA_BIOS_RESULTS];
  UINT8     bssaNumberDistinctGuids;
#endif //SSA_FLAG
#endif //BDAT_SUPPORT
  UINT8     pprStatus[MAX_PPR_ADDR_ENTRIES];
  RASMEMORYINFO  RasMeminfo;
  // APTIOV_SERVER_OVERRIDE_RC_START : Added to skip LastBoot Error Handler for cold boot.
  BOOLEAN   IsColdBoot;
  // APTIOV_SERVER_OVERRIDE_RC_END : Added to skip LastBoot Error Handler for cold boot.
};

#pragma pack()

#endif
