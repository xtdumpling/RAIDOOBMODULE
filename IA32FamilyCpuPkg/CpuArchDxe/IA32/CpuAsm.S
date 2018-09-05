//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
#------------------------------------------------------------------------------
#
# Copyright (c) 1999 - 2013, Intel Corporation. All rights reserved.<BR>
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
#   CpuAsm.S
# 
# Abstract:
# 
#   Assembly code that supports IA32 CPU architectural protocol
#
#------------------------------------------------------------------------------

.text


#---------------------------------------;
# _InitializeSelectors                  ;
#----------------------------------------------------------------------------;
# 
# Protocol prototype
#   InitializeSelectors (
#   )
#           
# Routine Description:
# 
#  Creates an new GDT in RAM.  The problem is that our former selectors
#  were ROM based and the EFI OS Loader does not manipulate the machine state 
#  to change them (as it would for a 16-bit PC/AT startup code that had to
#  go from Real Mode to flat mode).
# 
# Arguments:
# 
# 
# Returns: 
# 
#   Nothing
#
# 
# Input:  [ebp][0]  = Original ebp
#         [ebp][4]  = Return address
#          
# Output: Nothing
#           
# Destroys: Nothing
#-----------------------------------------------------------------------------;

.equ          CODE_SELECTOR, 0x10
.equ          DATA_SELECTOR, 0x18

ASM_GLOBAL ASM_PFX(InitializeGdt)
ASM_PFX(InitializeGdt):
  pushl   %ebp                # C prolog
  movl    %esp,%ebp
  pushal
  movl    $Gdtr, %edi

  movw    %cs,%ax             # Get the selector data from our code image          
  .byte 0x66
  movw    %ax,%es
  lgdt    %es:(%edi)

  .byte 0x67
  .byte 0xea                # Far Jump Offset:Selector to reload CS
  .long      SelectorRld
  .word      CODE_SELECTOR
SelectorRld:
  movw    $DATA_SELECTOR, %ax # Update the Base for the new selectors, too
  .byte 0x66
  movw    %ax,%ds
  .byte 0x66
  movw    %ax,%es
  .byte 0x66
  movw    %ax,%fs
  .byte 0x66
  movw    %ax,%gs
  .byte 0x66
  movw    %ax,%ss

  popal
  popl    %ebp
  ret

#-----------------------------------------------------------------------------;
# data
#-----------------------------------------------------------------------------;

        .p2align 4

Gdtr:    .word GDT_END - GDT_BASE - 1
         .long GDT_BASE

#-----------------------------------------------------------------------------;
#   global descriptor table (GDT)
#-----------------------------------------------------------------------------;

        .p2align 4

GDT_BASE: 
# null descriptor
# .equ                NULL_SEL, $-GDT_BASE # Selector [0]
        .word 0         # limit 15:0
        .word 0         # base 15:0
        .byte 0         # base 23:16
        .byte 0         # type
        .byte 0         # limit 19:16, flags
        .byte 0         # base 31:24

# linear data segment descriptor
# .equ            LINEAR_SEL, $-GDT_BASE # Selector [0x8]
        .word 0xFFFF    # limit 0xFFFFF
        .word 0         # base 0
        .byte 0
        .byte 0x92      # present, ring 0, data, expand-up, writable
        .byte 0xCF              # page-granular, 32-bit
        .byte 0

# linear code segment descriptor
# .equ            LINEAR_CODE_SEL, $-GDT_BASE # Selector [0x10]
        .word 0xFFFF    # limit 0xFFFFF
        .word 0         # base 0
        .byte 0
        .byte 0x9A      # present, ring 0, data, expand-up, writable
        .byte 0xCF              # page-granular, 32-bit
        .byte 0

# system data segment descriptor
# .equ            SYS_DATA_SEL, $-GDT_BASE # Selector [0x18]
        .word 0xFFFF    # limit 0xFFFFF
        .word 0         # base 0
        .byte 0
        .byte 0x92      # present, ring 0, data, expand-up, writable
        .byte 0xCF              # page-granular, 32-bit
        .byte 0

# system code segment descriptor
# .equ            SYS_CODE_SEL, $-GDT_BASE
        .word 0xFFFF    # limit 0xFFFFF
        .word 0         # base 0
        .byte 0
        .byte 0x9A      # present, ring 0, data, expand-up, writable
        .byte 0xCF              # page-granular, 32-bit
        .byte 0

# spare segment descriptor
# .equ        SPARE3_SEL, $-GDT_BASE
        .word 0         # limit 0xFFFFF
        .word 0         # base 0
        .byte 0
        .byte 0         # present, ring 0, data, expand-up, writable
        .byte 0         # page-granular, 32-bit
        .byte 0

# spare segment descriptor
# .equ        SPARE4_SEL, $-GDT_BASE
        .word 0         # limit 0xFFFFF
        .word 0         # base 0
        .byte 0
        .byte 0         # present, ring 0, data, expand-up, writable
        .byte 0         # page-granular, 32-bit
        .byte 0

# spare segment descriptor
# .equ        SPARE5_SEL, $-GDT_BASE
        .word 0         # limit 0xFFFFF
        .word 0         # base 0
        .byte 0
        .byte 0         # present, ring 0, data, expand-up, writable
        .byte 0         # page-granular, 32-bit
        .byte 0

GDT_END: 

