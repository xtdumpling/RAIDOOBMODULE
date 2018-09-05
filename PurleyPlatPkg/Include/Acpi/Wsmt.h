/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2017 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/


#ifndef _WSMT_H_
#define _WSMT_H_

//
// Statements that include other files
//

#include <IndustryStandard/Acpi.h>

// signature "WSMT"
#define ACPI_WSMT_SIGNATURE                       0x544D5357
#define ACPI_WSMT_LENGTH                          40

#define ACPI_OEM_WSMT_REVISION                    0x00000000

#define EFI_ACPI_WSMT_REVISION                    0x00000001

#define WSMT_PROTECTION_FLAG                      (BIT0 | BIT1 | BIT2 )


#pragma pack(1)
  
typedef union {
  struct {
    //
    //BIT0: 
    //If set, expresses that for all synchronous SMM entries,\
    //SMM will validate that input and output buffers lie entirely within the expected fixed memory regions. 
    //
    UINT32   FixedCommBuffers:1;
    //
    //BIT1: COMM_BUFFER_NESTED_PTR_PROTECTION
    //If set, expresses that for all synchronous SMM entries, SMM will validate that input and \
    //output pointers embedded within the fixed communication buffer only refer to address ranges \ 
    //that lie entirely within the expected fixed memory regions.
    //
    UINT32   CommBufferNestedPointerProtection:1;
    //
    //BIT2: SYSTEM_RESOURCE_PROTECTION
    //Firmware setting this bit is an indication that it will not allow reconfiguration of system resources via non-architectural mechanisms.   
    //
    UINT32   SystemResourceProtection:1;
    UINT32   Reserved:29;
  } Bits;
  UINT32 Flags;  
} EFI_ACPI_WSMT_PROTECTION_FLAGS;

//  WSMT ACPI table 
//
typedef struct _ACPI_WINDOWS_SMM_SECURITY_MITIGATIONS_TABLE {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  EFI_ACPI_WSMT_PROTECTION_FLAGS          ProtectionFlags;
} ACPI_WINDOWS_SMM_SECURITY_MITIGATIONS_TABLE;

#pragma pack()


#endif //_WSMT_H_
