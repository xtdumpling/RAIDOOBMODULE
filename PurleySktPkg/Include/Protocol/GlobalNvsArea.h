//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2004 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file GlobalNvsArea.h

  Definition of the global NVS area protocol.  This protocol 
  publishes the address and format of a global ACPI NVS buffer used as a communications
  buffer between SMM code and ASL code.

  Note:  Data structures defined in this protocol are not naturally aligned.

**/

#ifndef _EFI_GLOBAL_NVS_AREA_H_
#define _EFI_GLOBAL_NVS_AREA_H_

//
// Global NVS Area Protocol GUID
//
#define EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID \
{ 0x74e1e48, 0x8132, 0x47a1, 0x8c, 0x2c, 0x3f, 0x14, 0xad, 0x9a, 0x66, 0xdc }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiGlobalNvsAreaProtocolGuid;

//
// Global NVS Area definition
//
#pragma pack (1)

typedef struct {
  // IOAPIC Start
  UINT32 PlatformId;
  UINT32 IoApicEnable;
  UINT8  ApicIdOverrided  :1;
  UINT8  RES0           :7;          
  // IOAPIC End

  // Power Management Start
  UINT8  TpmEnable      :1;
  UINT8  CStateEnable   :1;
  UINT8  C3Enable       :1;
  UINT8  C6Enable       :1;
  UINT8  C7Enable       :1;
  UINT8  MonitorMwaitEnable :1;
  UINT8  PStateEnable   :1;
  UINT8  EmcaEn         :1;
  UINT8  HWAllEnable    :2;
  UINT8  KBPresent      :1;
  UINT8  MousePresent   :1;
  UINT8  TStateEnable   :1;
  UINT8  TStateFineGrained: 1;
  UINT8  OSCX           :1;
  UINT8  RESX           :1;          
  // Power Management End

  // RAS Start
  UINT8  CpuChangeMask;
  UINT8  IioChangeMask;
  UINT64 IioPresentBitMask;
  UINT32 SocketBitMask;           //make sure this is at 4byte boundary
  UINT32 ProcessorApicIdBase[8];
  UINT64 ProcessorBitMask[8];
  UINT16 MemoryBoardBitMask;
  UINT16 MemoryBoardChgEvent;
  UINT32 MmCfg;
  UINT32 TsegSize;
  UINT64 MemoryBoardBase[8];
  UINT64 MemoryBoardRange[8];
  UINT32 SmiRequestParam[4];
  UINT32 SciRequestParam[4];
  UINT64 MigrationActionRegionAddress;
  UINT8  Cpu0Uuid[16];
  UINT8  Cpu1Uuid[16]; 
  UINT8  Cpu2Uuid[16]; 
  UINT8  Cpu3Uuid[16]; 
  UINT8  Cpu4Uuid[16];
  UINT8  Cpu5Uuid[16]; 
  UINT8  Cpu6Uuid[16]; 
  UINT8  Cpu7Uuid[16]; 
  UINT8  CpuSpareMask;   
  UINT8  Mem0Uuid[16];  
  UINT8  Mem1Uuid[16]; 
  UINT8  Mem2Uuid[16]; 
  UINT8  Mem3Uuid[16]; 
  UINT8  Mem4Uuid[16];  
  UINT8  Mem5Uuid[16]; 
  UINT8  Mem6Uuid[16]; 
  UINT8  Mem7Uuid[16]; 
  UINT8  Mem8Uuid[16];  
  UINT8  Mem9Uuid[16]; 
  UINT8  Mem10Uuid[16]; 
  UINT8  Mem11Uuid[16]; 
  UINT8  Mem12Uuid[16];  
  UINT8  Mem13Uuid[16]; 
  UINT8  Mem14Uuid[16]; 
  UINT8  Mem15Uuid[16]; 
  UINT16 MemSpareMask;
  UINT64 EmcaL1DirAddr;
  UINT32 ProcessorId;
  UINT8  PcieAcpiHotPlugEnable;
  // RAS End

  // VTD Start
  UINT64 DrhdAddr[3];   
  UINT64 AtsrAddr[3];   
  UINT64 RhsaAddr[3];
  // VTD End
  
  // SR-IOV WA Start
  UINT8  WmaaSICaseValue;  
  UINT16 WmaaSISeg;        
  UINT8  WmaaSIBus;        
  UINT8  WmaaSIDevice;     
  UINT8  WmaaSIFunction;   
  UINT8  WmaaSISts; 
  UINT8  WheaSupportEn;
  // SR-IOV End

  // BIOS Guard Start
  UINT64  BiosGuardMemAddress;
  UINT8   BiosGuardMemSize;
  UINT16  BiosGuardIoTrapAddress;
  // APTIOV_SERVER_OVERRIDE_RC_START
  UINT8   BiosGuardIoTrapLength;
  // APTIOV_SERVER_OVERRIDE_RC_END
  UINT8   CpuSkuNumOfBitShift;
  // BIOS Guard End
  
  // USB3 Start
  UINT8   XhciMode;
  UINT8   HostAlertVector1;
  UINT8   HostAlertVector2;
  // USB3 End
  
  // HWPM Start
  UINT8   HWPMEnable:2; //HWPM
  UINT8   AutoCstate:1; //HWPM
  UINT8   HwpInterrupt:1; //HWP Interrupt
  UINT8   RES1:4;       //reserved bits
  // HWPM End

  // PCIe Multi-Seg Start
  // for 8S support needs max 32 IIO IOxAPIC being enabled!
  UINT8   BusBase[48];		  // MAX_SOCKET * MAX_IIO_STACK. Note: hardcode due to ASL constraint. 
  UINT8   PCIe_MultiSeg_Support;    // Enable /Disable switch
  // for 8S support needs matching to MAX_SOCKET!
  UINT8   PcieSegNum[8];	  // Segment number array. Must match MAX_SOCKET. Note: hardcode due to ASL constraint.
  // PCIe Multi-seg end

  // Performance Start
  UINT8   SncAnd2Cluster;           //1=SncEn and NumCluster=2, otherwise 0     
  // Performance End

  // XTU Start
  UINT32      XTUBaseAddress;                             // 193 XTU Base Address
  UINT32      XTUSize;                                    // 197 XTU Entries Size
  UINT32      XMPBaseAddress;                             // 201 XTU Base Address
  UINT8       DDRReferenceFreq;                           // 205 DDR Reference Frequency
  UINT8       Rtd3Support;                                // 206 Runtime D3 support.
  UINT8       Rtd3P0dl;                                   // 207 User selctable Delay for Device D0 transition.
  UINT8       Rtd3P3dl;                                   // 208 User selctable Delay for Device D0 transition.
  // XTU End

  // FPGA Root Port Bus
  UINT8   FpgaBusBase[8];
  UINT8   FpgaBusLimit[8];

  // FPGA present bit
  UINT8   FpgaPresent[8];

  // FPGA Resource Allocation
  UINT32  VFPBMemBase[8];
  UINT32  VFPBMemLimit[8];
  
  // FPGA KTI present bitmap
  UINT32   FpgaKtiPresent;       
  // FPGA Bus for KTI
  UINT8   FpgaKtiBase[8];
 } BIOS_ACPI_PARAM;

#pragma pack ()

//
// Global NVS Area Protocol
//
typedef struct _EFI_GLOBAL_NVS_AREA_PROTOCOL {
  BIOS_ACPI_PARAM     *Area;
} EFI_GLOBAL_NVS_AREA_PROTOCOL;

#endif
