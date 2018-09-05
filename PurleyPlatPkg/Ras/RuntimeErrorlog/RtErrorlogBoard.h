//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2016, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//  
//  History
// 
//
//
//  Rev. 1.01
//   Bug Fixed:  Fixed that PERR could not work.. 
//   Reason:     
//   Auditor:    Chen Lin
//   Date:       Nov/11/2016
//  
//  Rev. 1.00
//      Bug Fixed:  Fix cannot log SERR/PERR. 
//      Reason:     
//      Auditor:    Chen Lin
//      Date:       Sep/09/2016
// 
//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//
//*****************************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		RtErrorLogBoard.h
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _RT_ERRORLOG_BOARD_H_
#define _RT_ERRORLOG_BOARD_H_

#include "token.h"
#include  <RtErrorlogElinks.h>
#include "ErrorRecords.h"
#include <IndustryStandard\Acpi.h>
#include <Protocol\PlatformRasPolicyProtocol.h>
#include <PciExpress.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PciExpressLib.h>
#include "CommonErrorHandlerDefs.h"
#include "RcRegs.h"
#if SMCPKG_SUPPORT
#define ASC_293920LPE_VID         0x9005
#define ASC_293920LPE_DID         0x8017
#define ASC_293920LPE_VID_DID     ( ASC_293920LPE_VID |( ASC_293920LPE_DID << 16) )     
#define PCI_BRIDGE_CLASS_CODE     0x0604

#ifndef PCI_SCC
#define PCI_SCC                     0x000A        // Sub Class Code Register
#endif
#ifndef PCI_SBUS
#define PCI_SBUS                    0x0019        // Secondary Bus Number Register
#endif 
#ifndef PCI_SECSTATUS
#define PCI_SECSTATUS               0x001E        // Secondary Status Register
#endif 
#ifndef PCI_STS_MASTER_DATA_PARITY_ERROR
#define PCI_STS_MASTER_DATA_PARITY_ERROR         0x0100
#endif
#endif
// Changes done for LastBoot Error handler support
extern  EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;

//
// PCI Error Status Bits
//
#define PCI_STS_MSTR_DATA_PERR      BIT8
#define PCI_STS_SIG_TRGT_ABRT       BIT11
#define PCI_STS_REC_TRGT_ABRT       BIT12
#define PCI_STS_SIG_SERR            BIT14
#define PCI_STS_DET_PERR            BIT15

#define EFI_PCI_CAPABILITY_ID_PCIEXP                0x10
#define EFI_PCI_CAPABILITY_PTR                      0x34
#define EFI_PCI_STATUS_CAPABILITY                       BIT4   ///< 0x0010

#define LASTBOOT_PCIE_ERROR                             0x0F

typedef enum {
  Unknown       = 0,
  PciSource ,
  CsiSource,
  MemSource,
  ProcSource,
  UncoreSource,
  IohQpiSource,
  IohCoreSource,
  IohVtdSource,
  IohItcSource,
  IohOtcSource,
  IohDmaSource,
  CoreSource,
  PcuSource,
  UpiSource,
  ChaSource
} ERROR_SOURCE; 

#pragma pack(1)

typedef struct {
  UINT8   				Socket;
  UINT8   				Branch;
  UINT8   				MemoryController;
  UINT8   				FbdChannel;
  UINT8   				DdrChannel;
  UINT8   				Dimm;
} MEM_ERROR_LOCATION;

typedef struct {
  ERROR_SOURCE       ErrorType;
  UINT8                           ErrorSeverity;
} ERROR_HEADER;

typedef struct {
  ERROR_HEADER   	Header;
  UINT8   			ErrorSubType;
  UINT8   			Socket;
  UINT8             Stack;
  UINT8   			Port;
  UINT8   			Data[1];
} ERROR_DATA_TYPE1;

typedef struct _IOH_QPI_COMBINED_ERROR_RECORD {
  ERROR_DATA_TYPE1             IohQpiErrorRecord;
}IOH_QPI_COMBINED_ERROR_RECORD;

typedef struct _IOH_CORE_COMBINED_ERROR_RECORD {
  ERROR_DATA_TYPE1             IohCoreErrorRecord;
}IOH_CORE_COMBINED_ERROR_RECORD;

typedef struct _IOH_VTD_COMBINED_ERROR_RECORD {
  ERROR_DATA_TYPE1             IohVtdErrorRecord;
}IOH_VTD_COMBINED_ERROR_RECORD;

typedef struct _ITC_COMBINED_ERROR_RECORD {
  ERROR_DATA_TYPE1             ItcErrorRecord;
} ITC_COMBINED_ERROR_RECORD;

typedef struct _OTC_COMBINED_ERROR_RECORD {
  ERROR_DATA_TYPE1             OtcErrorRecord;
} OTC_COMBINED_ERROR_RECORD;

typedef struct _DMA_COMBINED_ERROR_RECORD {
  ERROR_DATA_TYPE1             DmaErrorRecord;
} DMA_COMBINED_ERROR_RECORD;

typedef struct {
  //
  // Header-like information...
  //
  ERROR_HEADER    Header;
  //
  // Essential information...
  //
  UINT8                                   Socket;
  UINT8                                   UpiPort;
  BOOLEAN                            Valid;
  UINT8                                   CorrectedErrorCount;
  UINT64				  Address;
  UINT16                                 ModelSpecificCode; // Will go as extended data...
  UINT16                                 MachineCheckCode; // Will go as extended data...
} UPI_ERROR_RECORD;

typedef struct {
  //
  // Header-like information...
  //
ERROR_HEADER    Header;
  //
  // Essential information...
  //
  UINT8                                   Socket;
  UINT8                                   CoreMcaBank;
  BOOLEAN                            Valid;
  UINT8                                   CorrectedErrorCount;
  UINT16                                  ModelSpecificCode; // Will go as extended data...
  UINT16                                  MachineCheckCode; // Will go as extended data...
} CORE_ERROR_RECORD;

typedef struct {
  //
  // Header-like information...
  //
ERROR_HEADER    Header;
  //
  // Essential information...
  //
  UINT8                                   Socket;
  BOOLEAN                                 Valid;
  UINT8                                   CorrectedErrorCount;
  UINT16                                  ErrorAddress;
  UINT16                                  ModelSpecificCode; // Will go as extended data...
  UINT16                                  MachineCheckCode; // Will go as extended data...
} PCU_ERROR_RECORD;

typedef struct {
  //
  // Header-like information...
  //
  ERROR_HEADER    Header;
  //
  // Essential information...
  //
  UINT8                                   Socket;
  UINT8                                   CoreId;
  UINT8					 				  ThreadId;
  UINT8                                   ChaMcaBank;
  BOOLEAN                            Valid;
  UINT8                                   CorrectedErrorCount;
  UINT64				  				  Address;
  UINT16                                  ModelSpecificCode; // Will go as extended data...
  UINT16                                  MachineCheckCode; // Will go as extended data...
} CHA_ERROR_RECORD;


typedef struct {
  //
  // Header-like information
  //
  ERROR_HEADER    Header;
  // Standard fields
  UINT64                    ValidBits;
  UINT64                    ErrorStatus;
  UINT64                    PhysAddr;
  UINT64                    PhysAddrMask;
  UINT16                    Node;
  UINT16                    Card;
  UINT16                    Module;
  UINT16                    Bank;
  UINT16                    Device;
  UINT16                    Row;
  UINT16                    Col;
  UINT16                    BitPosition;
  UINT64                    RequesterId;
  UINT64                    ResponderId;
  UINT64                    TargetId;
  UINT8                     ErrorType;
  UINT8                     Extended;
  UINT16                    RankNumber;
  UINT16                    CardHandle;
  UINT16                    ModuleHandle;
  MCA_BANK_INFO             McaBankInfo;
  UINT8                     ErrorEvent;  // OEM Hook Support (ErrorEvent - 0 SDDC 1 ADDDC 2 RANK SPARING)
  UINT64                    RankAddress;
  UINT8                     OtherMcBankCount;  // Total valid MCBanks
  MCA_BANK_INFO             OtherMcBankInfo[MAX_NUM_MCA_BANKS];  // Total valid MC Banks data
} AMI_MEMORY_ERROR_SECTION;

typedef struct {
  //
  // Header-like information
  //
  ERROR_HEADER    Header;
  // Standard fields
  UINT64                      ValidBits;
  UINT32                      PortType;
  UINT32                      Version;
  UINT32                      CommandStatus;
  UINT32                      Reserved;
  PCIE_ERROR_SECTION_DEV_ID   DeviceId;
  PCIE_ERROR_SECTION_DEV_SER  DeviceSerial;
  PCIE_ERROR_BRIDGE_STS_CNTL  BridgeControlSts;
  UINT8                       CapStruct[60];
  UINT8                       AerInfo[96];
  UINT32                      Uncerrsts;
  UINT32                      Corerrsts;
  UINT32                      Rperrsts;
  UINT8                       ErrorType;
  UINT16                      PcieDeviceStatus;
  UINT32                      XpCorErrSts;
  UINT32                      XpUncErrSts;
} AMI_PCIE_ERROR_SECTION;

typedef struct {
//
// Header-like information...
//
ERROR_HEADER    Header;
  // Standard fields
  UINT64                    ValidBits;
  UINT8                     Type;
  UINT8                     Isa;
  UINT8                     ErrorType;
  UINT8                     Operation;
  UINT8                     Flags;
  UINT8                     Level;
  UINT16                    Resv1;
  UINT64                    VersionInfo;
  UINT8                     BrandString[128];
  UINT64                    ApicId;
  UINT64                    TargetAddr;
  UINT64                    RequestorID;
  UINT64                    ResponderID;
  UINT64                    InstuctionIP;
  MCA_BANK_INFO             McaBankInfo;
  UINT8                     OtherMcBankCount;  // Total valid MCBanks
  MCA_BANK_INFO             OtherMcBankInfo[MAX_NUM_MCA_BANKS];  // Total valid MC Banks data
} AMI_PROCESSOR_GENERIC_ERROR_SECTION;

#pragma pack()

#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
