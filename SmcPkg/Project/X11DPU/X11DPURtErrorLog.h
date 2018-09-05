//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2017, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//  
//  History
//
//  Rev. 1.00
//    Bug Fix : Initialize revision
//    Reason  : 
//    Auditor : Jimmy Chiu
//    Date    : Jul/03/2017
//  
//**********************************************************************//
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		X11DPURtErrorLog.h
//
// Description:	 Runtime error log SMC includes and declarations 
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _RT_ERR_LOG_SMC_H
#define _RT_ERR_LOG_SMC_H

#include "token.h"
#include <AmiDxeLib.h>
#include <Protocol/SmbusHc.h>
//#include <Protocol/SmmSmbus.h>
//#include <Protocol/SmmBase2.h>
#include <Ppi/Smbus2.h>
#include <IndustryStandard/SmBus.h>
#include <Library/SmmServicesTableLib.h>

#define MAX_IMC                 2
#define MAX_NUM_MCA_BANKS       32
#define NODE_TO_SKT(node)       (node / MAX_IMC)
#define NODE_TO_MC(node)        (node % MAX_IMC)

VOID SmcClrMemErrCmosFlag (VOID);

EFI_GUID gEfiSmcSmmSmbusProtocolGuid = {
  0x72e40094, 0x2ee1, 0x497a, 0x8f, 0x33, 0x4c, 0x93, 0x4a, 0x9e, 0x9c, 0xd
};

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
  ERROR_SOURCE              ErrorType;
  UINT8                     ErrorSeverity;
} ERROR_HEADER;

typedef struct {
  BOOLEAN                   Valid;
  UINT32                    BankNum;
  UINT32                    ApicId;
  UINT32                    BankType;
  UINT32                    BankScope;
  UINT64                    McaStatus;
  UINT64                    McaAddress;
  UINT64                    McaMisc;
} MCA_BANK_INFO;

typedef struct {
  //
  // Header-like information
  //
  ERROR_HEADER              Header;
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

typedef struct{
    UINT8	DIMM_NUM[3];
}IMC_DIMM_NUM;

#pragma pack()

#endif
