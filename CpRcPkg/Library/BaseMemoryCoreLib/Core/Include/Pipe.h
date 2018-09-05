//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

@copyright
  Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  Pipe.h

@brief
  This file contains Pipe related data

**/

#ifndef _PIPE_H_
#define _PIPE_H_

#include "DataTypes.h"

//
// Define PIPE Error Code Here
//
#define PIPE_NO_SBSP_FOUND  FBD_INIT_ERR (28) // Failure to find SBSP
//
// Data Structure for Memory HOB SBSP-NBSP Communication Pipe
//
#define PIPE_ACK                        0
#define PIPE_REQUEST                    1
#define PIPE_WRITE_COMMAND              0
#define PIPE_READ_COMMAND               1
#define PIPE_QUERY_COMMAND              2
#define PIPE_CLOSE_COMMAND              3
#define PIPE_RUN_MEMORY_COMMAND         4
#define PIPE_GET_MEMORY_STATUS_COMMAND  5
#define PIPE_SET_MEMORY_STATUS_COMMAND  6
#define PIPE_SET_PLATFORM_COMMAND       7
#define PIPE_SUCCESS                    0
#define PIPE_INVALID                    1
#define PIPE_TOGGLE_ERROR               2
#define PIPE_INCOMPLETE_ERROR           3

#define PIPE_ID_KEY                     0x18
#define COMMAND_PIPE_LOCK(id)           (((PIPE_ID_KEY | id) << 11) | 1)
#define COMMAND_PIPE_UNLOCK(id)         ((PIPE_ID_KEY | id) << 11)
#define CSR_TYPE_UBOX                   0x0000
#define WRITE_COMMAND                   (1 << PIPE_WRITE_COMMAND)
#define READ_COMMAND                    (1 << PIPE_READ_COMMAND)
#define QUERY_COMMAND                   (1 << PIPE_QUERY_COMMAND)
#define CLOSE_COMMAND                   (1 << PIPE_CLOSE_COMMAND)
#define RUN_MEMORY_COMMAND              (1 << PIPE_RUN_MEMORY_COMMAND)
#define GET_MEMORY_STATUS_COMMAND       (1 << PIPE_GET_MEMORY_STATUS_COMMAND)
#define SET_MEMORY_STATUS_COMMAND       (1 << PIPE_SET_MEMORY_STATUS_COMMAND)
#define SET_PLATFORM_SPECIFIC_COMMAND   (1 << PIPE_SET_PLATFORM_COMMAND)

typedef struct {
  UINT32  CmdRequest : 1; // Request Indicator (0:Invalid or 1:Valid)
  // Responder must clear this bit on return
  //
  UINT32  InternalCmd : 3;  // Command Type:
  // 000b:Write
  // 001b:Read
  // 010b:Query
  // 011b:Semaphore Exit
  // 100b:Run Memory Init
  // 101b:Get Memory Init Status. SBSP read FBD
  //      Status in data pipe if Status = success
  // 110b:Set Memory Init Status. NBSP read FBD
  // 111b:Run Platform Specific Needs (like Mirroring)
  //
  UINT32  Width_ : 2; // Number of valid bytes in data pipe
  UINT32  Toggle : 1; // Toggle bit
  UINT32  End_ : 1;   // Indicator of transaction completed
  UINT32  Begin : 1;  // Indicator of begining of transaction
  UINT32  Status : 2; // 000b:Success
  // 001b:Invalid Protocol Error
  // 010b:Toggle bit Error
  // 011b:Package Error
  //
  UINT32  TargetId : 4;     // TargetId
  UINT32  NumOfBytes : 17;  // Number of bytes returned for a query
  // or number of bytes remained during data transfer
  //
} PIPE;

typedef union {
  PIPE    Pipe;
  UINT32  RawPipe;
} COMMAND_PIPE;

//
// Pipe CSR definitions
//
#ifndef _CSR_H_
#define _CSR_H_

#define BMP_GLOBAL_CPU_CSR_BASE1        0xFC000000
#define BMP_LOCAL_CPU_CSR_BASE1         0xFEB20000
#define LocalCpuCsrAddress1(Reg)        (UINT32) ((BMP_LOCAL_CPU_CSR_BASE1) | (Reg & 0xFFFF))
#define GlobalCpuCsrAddress1(Cpu, Reg)  (UINT32) ((BMP_GLOBAL_CPU_CSR_BASE1) | (((Cpu) & 0x7) << 21) | ((Reg) & 0xFFFF))

#define CPU_PROTECTED_DRAM_ADDRESS      0x3FFF000000000
#define CPU_PROTECTED_DRAM_SIZE         64      // 64GB Size
#define CPU_GLOBAL_CSR_BIT_SHIFT        0x14    // 20 bits, 1M Bytes Size
#define CPU_BOX_CSR_BIT_SHIFT           0xC     // 12 bits, 4K Bytes Size
#define CPU_CACHING_AGENT_NUM           8       // Four CPE (cache agent)
#define CPU_CORES_PER_SOCKET            8
#define CPU_THREADS_PER_CORE            2
#define CPU_CORE_BITS_MASK              ((1 << CPU_THREADS_PER_CORE) - 1)
#define CSR_TYPE_UBOX                   0x0000
#define CSR_TYPE_WBOX                   0x0001
#define CSR_TYPE_SBOX0                  0x0002
#define CSR_TYPE_SBOX1                  0x0003
#define CSR_TYPE_BBOX0                  0x0004  // BBOX0 and ZBOX0 have same offset
#define CSR_TYPE_BBOX1                  0x0006  // BBOX1 and ZBOX1 have same offset
#define CSR_TYPE_ZBOX0                  0x0005
#define CSR_TYPE_ZBOX1                  0x0007
#define CSR_TYPE_CPE0                   0x0008
#define CSR_TYPE_CPE1                   0x0009
#define CSR_TYPE_CPE2                   0x000A
#define CSR_TYPE_CPE3                   0x000B
#define CSR_TYPE_CPE4                   0x000C
#define CSR_TYPE_CPE5                   0x000D
#define CSR_TYPE_CPE6                   0x000E
#define CSR_TYPE_CPE7                   0x000F
#define CSR_TYPE_RBOX0                  0x0010
#define CSR_TYPE_RBOX1                  0x0011
#define CSR_TYPE_PBOX                   0x0018
#define CSR_TYPE_PBOX1                  0x0019

//
// Beckton register set
// BOX Device Number
// BOX Func number
//
#define BOX_FUNC0 0x00
#define BOX_FUNC1 0x01
#define BOX_FUNC2 0x02
#define BOX_FUNC3 0x03
#define BOX_FUNC4 0x04
#define BOX_FUNC5 0x05
#define BOX_FUNC6 0x06
#define BOX_FUNC7 0x07

//
// BOX DEVID
//
#define UBOX_DEVID_F0 0x2B00
#define UBOX_DEVID_F1 0x2B01
#define UBOX_DEVID_F2 0x2B02
#define UBOX_DEVID_F3 0x2B03
#define UBOX_DEVID_F4 0x2B68
#define UBOX_DEVID_F5 0x2B69
#define UBOX_DEVID_F6 0x2B6A
#define UBOX_DEVID_F7 0x2B6B

//
// Pipe Dispatcher Commands
//
#define PIPE_DISPATCH_NULL            0
#define PIPE_DISPATCH_MEMORY_INIT     1
#define PIPE_DISPATCH_QPI_INIT        2
#define PIPE_DISPATCH_QPI_ADAPTATION  3
#define PIPE_DISPATCH_SYNCH_PSYSHOST  4
#define PIPE_RD_MSR                   5
#define PIPE_WR_MSR                   6
#define PIPE_DISPATCH_MEM_OL_DATA_READY     7
#define PIPE_DISPATCH_DETECT_DIMM_DONE      7
#define PIPE_DISPATCH_CHECK_STATUS          8
#define PIPE_DISPATCH_DIMMRANK_DONE         9
#define PIPE_DISPATCH_MEMINIT_DONE          10
#define PIPE_DISPATCH_FREEZE          0x80
#define PIPE_DISPATCH_EXIT            0x99

//
// Functions for finding local socket Id and SBSP socket Id
// Note: if CSI reference code changes BOOT_INITIALIZATION_DATA data structure, change accordingly.
//
#define SKT_INVALID_ID      (UINT16) (-1)
#define SKT_INFO_VALID_BIT  62    // ~BOOT_INITIALIZATION_DATA.SocketInfoValid bit[62]
#define SKT_SBSP_ID_SHIFT   40    // ~BOOT_INITIALIZATION_DATA.BootInitData.SocketInfo bits[47:40]
#define SKT_SBSP_ID_MASK    0xFF  // ~BOOT_INITIALIZATION_DATA.BootInitData.SocketInfo bits[47:40]
// SOCKET ID
//
#define SOCKET_ID_MASK  0x7

typedef struct {
  UINT32  AgentType;      // Refer to CSI_AGENT_TYPE
  UINT32  AgentId;        // Node ID Offset
  UINT32  CsrType;        // CSR Box Type or DID+FID
  UINT32  Reserved;
} CPU_AGENT_MAPPING_INFO;

typedef enum {
  ConfigAgentType   = 0,  // ~(UBOX | PBOX | JBOX | RBOX)
  FirmwareAgentType,
  RouterAgentType,
  Rsvd,
  LtAgentType,
  IoProxyAgentType,
  HomeAgentType,          // ~(BBOX0/ZBOX0 | BBOX1/ZBOX1)
  CacheAgentType          // ~(CPE0 | CPE1 | CPE2 | CPE3)
} CPU_AGENT_TYPE;

#endif // _CSR_H_

#endif // _PIPE_H_
