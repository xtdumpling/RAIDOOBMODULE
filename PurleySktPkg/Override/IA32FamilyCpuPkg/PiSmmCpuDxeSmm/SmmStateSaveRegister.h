/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  SMM State Save register header file.

  Copyright (c) 2012, Intel Corporation. All rights reserved. <BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _SMM_STATE_SAVE_REGISTER_H_
#define _SMM_STATE_SAVE_REGISTER_H_

#define MSR_CR0                      0xC00
#define MSR_CR3                      0xC01
#define MSR_EFLAGS                   0xC02

#define MSR_RIP                      0xC04
#define MSR_DR6                      0xC05
#define MSR_DR7                      0xC06
#define MSR_TR_LDTR                  0xC07
#define MSR_GS_FS                    0xC08
#define MSR_DS_SS                    0xC09
#define MSR_CS_ES                    0xC0A
#define MSR_IO_MISC_INFO             0xC0B
#define MSR_IO_MEM_ADDR              0xC0C
#define MSR_RDI                      0xC0D
#define MSR_RSI                      0xC0E
#define MSR_RBP                      0xC0F
#define MSR_RSP                      0xC10
#define MSR_RBX                      0xC11
#define MSR_RDX                      0xC12
#define MSR_RCX                      0xC13
#define MSR_RAX                      0xC14
#define MSR_R8                       0xC15
#define MSR_R9                       0xC16
#define MSR_R10                      0xC17
#define MSR_R11                      0xC18
#define MSR_R12                      0xC19
#define MSR_R13                      0xC1A
#define MSR_R14                      0xC1B
#define MSR_R15                      0xC1C

#define MSR_SMM_REVID                0xC21

#define MSR_LDTR_BASE                0xC2C
#define MSR_IDTR_BASE                0xC2D
#define MSR_GDTR_BASE                0xC2E

#define MSR_CR4                      0xC37
  
#endif // _SMM_STATE_SAVE_REGISTER_H_
