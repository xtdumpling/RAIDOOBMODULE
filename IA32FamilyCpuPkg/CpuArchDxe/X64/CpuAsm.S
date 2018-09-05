//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
#-------------------------------------------------------------------------------
#
# Copyright (c) 2006 - 2013, Intel Corporation. All rights reserved.<BR>
# This software and associated documentation (if any) is furnished
# under a license and may only be used or copied in accordance
# with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be
# reproduced, stored in a retrieval system, or transmitted in any
# form or by any means without the express written consent of
# Intel Corporation.

#
#
# Module Name:
#
#   CpuAsm.S
#
# Abstract:
#
#
#-------------------------------------------------------------------------------

.text

.equ          CODE_SELECTOR, 0x38
.equ          DATA_SELECTOR, 0x30

ASM_GLOBAL ASM_PFX(InitializeGdt)
ASM_PFX(InitializeGdt):

  movabsq $Gdtr, %rdx

  movw    %cs,%ax             # Get the selector data from our code image
  movw    %ax,%ds
  lgdt    (%rdx)

SelectorRld:
  movw    $DATA_SELECTOR, %ax # Update the Base for the new selectors, too
  movw    %ax,%ds
  movw    %ax,%es
  movw    %ax,%fs
  movw    %ax,%gs
  movw    %ax,%ss

  ret

#-----------------------------------------------------------------------------;
# data
#-----------------------------------------------------------------------------;

        .p2align 4

 Gdtr:   .word GDT_END - GDT_BASE - 1
         .quad GDT_BASE

#-----------------------------------------------------------------------------;
#   global descriptor table (GDT)
#-----------------------------------------------------------------------------;

        .p2align 4

GDT_BASE:
# null descriptor
#.equ                NULL_SEL, $-GDT_BASE # Selector [0]
    .word 0 # limit 15:0
    .word 0 # base  15:0
    .byte 0 # base  23:16
    .byte 0 #
    .byte 0 # type & limit 19:16
    .byte 0 # base  31:24

# linear data segment descriptor
#.equ            LINEAR_SEL, $-GDT_BASE # Selector [0x8]
    .word 0xffff # limit 15:0
    .word 0     # base  15:0
    .byte 0     # base  23:16
    .byte 0x92  #present, ring 0, data, expand-up writable
    .byte 0xcf  # type & limit 19:16
    .byte 0     # base  31:24

# linear code segment descriptor
#.equ            LINEAR_CODE_SEL, $-GDT_BASE # Selector [0x10]
    .word 0xffff    # limit 15:0
    .word 0        # base  15:0
    .byte 0        # base  23:16
    .byte 0x9a     # present, ring 0, code, expand-up writable
    .byte 0xcf     # type & limit 19:16
    .byte 0        # base  31:24

# system data segment descriptor
#.equ            SYS_DATA_SEL, $-GDT_BASE # Selector [0x18]
    .word 0xffff   # limit 15:0
    .word 0        # base  15:0
    .byte 0        # base  23:16
    .byte 0x92    # type & limit 19:16
    .byte 0xcf
    .byte 0        # base  31:24

# system code segment descriptor
#.equ            SYS_CODE_SEL, $-GDT_BASE # Selector [0x20]
    .word 0xffff   # limit 15:0
    .word 0    # base  15:0
    .byte 0    # base  23:16
    .byte 0x9a # type & limit 19:16
    .byte 0xcf
    .byte 0    # base  31:24

# spare segment descriptor             ; Selector [0x28]
#.equ        SPARE3_SEL, $-GDT_BASE
    .word 0 # limit 15:0
    .word 0 # base  15:0
    .byte 0 # base  23:16
    .byte 0 # type & limit 19:16
    .byte 0 # base  31:24
    .byte 0

# 64bit data segment descriptor
#.equ                  LONG_MODE_DATA_SEL, $-GDT_BASE # Selector [0x30]
    .word 0xffff # limit 15:0
    .word 0    # base  15:0
    .byte 0    # base  23:16
    .byte 0x92 # type & limit 19:16
    .byte 0xcf
    .byte 0    # base  31:24

# 64bit code segment descriptor
#.equ              LONG_MODE_CODE, $-GDT_BASE # Selector [0x38]
   .word 0xffff # limit 15:0
   .word 0    # base  15:0
   .byte 0    # base  23:16
   .byte 0x9a # type & limit 19:16
   .byte 0xaf
   .byte 0    # base  31:24

# spare segment descriptor
#.equ        SPARE5_SEL, $-GDT_BASE       # Selector [0x40]
    .word 0 # limit 15:0
    .word 0 # base  15:0
    .byte 0 # base  23:16
    .byte 0 # type & limit 19:16
    .byte 0 # base  31:24
    .byte 0

GDT_END:

