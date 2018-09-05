//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.03
//    Bug Fix : 1. [Fixes] Fixed the system cannot enter to TXT Environment when
//              using TPM 1.2 on production platform.
//              2. [Fixes] Fixed TPM 2.0 BIOS-Based Provisioning table doesn't 
//              set PW flag to PS LCP Data for production platform.
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Jul/20/2017
//
//  Rev. 1.02
//    Bug Fix : Fixed TPM 1.2 command sending failed and SMC_IPMICmd30_68_1B 
//              data 2 bit 4 cannot be set after TPM 1.2 is provisioned.
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Jun/08/2017
//
//  Rev. 1.01
//    Bug Fix : N/A.
//    Reason  : Implemented PPI-x OOB to check TPM provisioning table signature.
//    Auditor : Joe Jhang
//    Date    : Jan/10/2017
//
//  Rev. 1.00
//    Bug Fix : N/A.
//    Reason  : Initialized source code from Intel.
//    Auditor : Joe Jhang
//    Date    : Jan/09/2017
//
//**************************************************************************//
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This file contains definitions for BIOS provisioning tool.

  Copyright (c) 2014, Intel Corporation. All rights Reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _TPM_PROVISION_H_
#define _TPM_PROVISION_H_

#include "SmcTpm12CommandLibEx.h"

extern EFI_GUID gSetupVariableGuid;

#pragma pack (1)

typedef struct {              //Definition for an Index                         SHA 256      SHA256       SHA256       SHA1         SHA1         SHA1
  UINT32 NvIndex;             // Index handle                                   03 00 80 01  01 00 80 01  04 00 80 01  03 00 80 01  01 00 80 01  04 00 80 01
  UINT8  bWrite;              // Indicates if BIOS writes the data              00           01           00           00           01           00
  UINT8  bWriteProtect;       // Indicates if BIOS write-protects the data      00           01           00           00           01           00
  UINT16 Reserved;            // Reserved for future use                        00 00        00 00        00 00        00 00        00 00        00 00
  UINT32 CreateCmdSize;       // size of TPM command to create the index        4D 00 00 00  4D 00 00 00  4D 00 00 00  41 00 00 00  41 00 00 00  41 00 00 00
  UINT16 CreateCmdOffset;     // Offset from table start to TPM create command  FF FF        FF FF        FF FF        FF FF        FF FF        FF FF
  UINT32 WriteCmdSize;        // size of TPM command to write the index         00 00 00 00  69 00 00 00  00 00 00 00  00 00 00 00  69 00 00 00  00 00 00 00
  UINT16 WriteCmdOffset;      // Offset from table start to TPM wrie command    FF FF        FF FF        FF FF        FF FF        FF FF        FF FF
} NV_INFO;                    //20 bytes
   
typedef struct {              // TPM1.2 Provisioning Table 
  UINT64  Signature;          // "TPM1Prov"
  UINT32  Length;             // Total length if this binary table
  UINT8   Major;              // Major Version                                  01
  UINT8   Minor;              // Minor Version                                  00
  UINT16  Reserved[3];        // Reserved for future use                        00 00 00 00 00 00
  UINT8   UserInfo[32];       // Reserved - not used by prov driver             31 31 2D 32 35 2D 32 30 31 34 20 54 50 4D 31 2E 32 20 50 72 6F 76 20 47 65 6E 20 54 6F 6F 6C 00
  UINT16  Reserved1;          // Reserved for future use                        00 00
  UINT8   NumIndexes;         // Number of NV Indexes to provision              04
  UINT8   NvInfoSize;         // Size of NV_INFO for a sanity check 20 bytes    14
  NV_INFO Params[8];          // Indexes to be provisioned 20 x 4 bytes
} PROV1_TABLE;                // 148 bytes with 4 indexes

typedef struct {              // TPM2 ProvParameters (one per Hash Alg)            sha1           sha256
  UINT16 HashAlgID;           // Hash algorithm                                    04 00          0B 00  
  UINT16 HashDigestSize;      // Size of hash digest                               14 00          20 00
  UINT32 SetPhPolSize;        // Size of SetPhPol command                          33 00 00 00    3F 00 00 00
  UINT16 SetPhPolOffset;      // Offset from table start to PH authPolicy command  FF FF          FF FF
  UINT16 Reserved;            // Reserved for future use                           00 00          00 00
  UINT8  NumIndexes;          // Number of indexes to create                       03             03
  UINT8  NvInfoSize;          // Size of NV_INFO for a sanity check                14             14
  UINT16 ParamsOffset;        // Offset of array of NV_INFO Indexes for this algorithm                   
} ALG_INFO;                   // 16 bytes
  
typedef struct {              // TPM2 Binary Provisioning table
  UINT64    Signature;          // "TPM2Prov"
  UINT32    Length;             // Total length if this binary table
  UINT8     Major;            // Major Version                                  01
  UINT8     Minor;            // Minor Version                                  00
  UINT16    Reserved[3];      // Reserved for future use                        00 00 00 00 00 00 
  UINT8     UserInfo[32];     // Reserved not used by prov driver               00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  UINT16    Reserved1;        // Reserved for future use                        00 00
  UINT8     NumAlgs;          // Number of algorithms to test                   02
  UINT8     AlgInfoSize;      // Size of ALG_INFO sanity check 16 bytes         10
  ALG_INFO  AlgParams[4];     // array of information for each algorithm
} PROV2_TABLE;        

#pragma pack ()

#define TPM12_PROVISION_TABLE_GUID { \
    0x67ef90c3, 0x9815, 0x454e, 0xbf, 0xfa, 0x70, 0x10, 0x72, 0xb9, 0xba, 0x4 \
  }

#define TPM20_PROVISION_TABLE_GUID { \
    0x7dbac6b6, 0x4b71, 0x4953, 0x90, 0xbf, 0x39, 0x21, 0x81, 0xc1, 0xa9, 0x5c \
  }

#define TPM12_PROVISION_TABLE_SIGNATURE SIGNATURE_64 ('T','P','M','1','P','r','o','v')

#define TPM20_PROVISION_TABLE_SIGNATURE SIGNATURE_64 ('T','P','M','2','P','R','O','V')

#endif  

