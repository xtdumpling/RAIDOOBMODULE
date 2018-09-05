#
# This file contains a 'Sample Driver' and is licensed as such
# under the terms of your license agreement with Intel or your
# vendor.  This file may be modified by the user, subject to  
# the additional terms of the license agreement               
#
#
# Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>
# This software and associated documentation (if any) is furnished
# under a license and may only be used or copied in accordance
# with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be
# reproduced, stored in a retrieval system, or transmitted in any
# form or by any means without the express written consent of
# Intel Corporation.
#
# Module Name:
#
#   TrigSmi.asm
#
# Abstract:
#
#   TrigSmi for capsule update. SMI number is 0x27 and 0x28.
#
# Notes:
#
#------------------------------------------------------------------------------

.text

#------------------------------------------------------------------------------
#  VOID
#  SendCapsuleSmi (
#    UINTN Addr
#    )
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(SendCapsuleSmi)
ASM_PFX(SendCapsuleSmi):
  push  %rbp
  mov   %rsp, %rbp
  sub   $8, %rsp
  push  %rax
  push  %rbx
  mov   %rcx, %rbx
  movq  $0xEF27, %rax
  outb  %al, $0x0B2
  pop   %rbx
  pop   %rax
  mov   %rbp, %rsp
  pop   %rbp
  ret   $0

#------------------------------------------------------------------------------
#  VOID
#  GetUpdateStatusSmi (
#    UINTN Addr
#    )
#------------------------------------------------------------------------------
ASM_GLOBAL ASM_PFX(GetUpdateStatusSmi)
ASM_PFX(GetUpdateStatusSmi):
  push  %rbp
  mov   %rsp, %rbp
  sub   $8, %rsp
  push  %rax
  push  %rbx
  mov   %rcx, %rbx
  movq  $0xEF28, %rax
  outb  %al, $0x0B2
  pop   %rbx
  pop   %rax
  mov   %rbp, %rsp
  pop   %rbp
  ret   $0
