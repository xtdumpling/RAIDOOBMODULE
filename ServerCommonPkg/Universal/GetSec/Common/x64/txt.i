#
# This file contains an 'Intel Peripheral Driver' and is      
# licensed for Intel CPUs and chipsets under the terms of your
# license agreement with Intel or your vendor.  This file may 
# be modified by the user, subject to additional terms of the 
# license agreement                                           
#
#------------------------------------------------------------------------------
#
# Copyright (c) 2007 - 2016 Intel Corporation. All rights reserved
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
#   txt.inc
#
# Abstract:
#
#------------------------------------------------------------------------------

#
# MSRs
#
.equ                               IA32_CR_FEATURE_CONTROL, 0x03A
.equ                               IA32_MISC_ENABLE, 0x1A0
.equ                               LT_OPT_IN_MSR_VALUE, 0x0FF03 # 01111111100000011b  # 0FF03
.equ                               LT_OPT_IN_VMX_ONLY_MSR_VALUE, 0x00005 # 00000000000000101b  # 00005

.equ                               ACMOD_SIZE, 24

.equ                               CR0_NE_MASK, (1 << 5)
.equ                               CR0_NW_MASK, (1 << 29)
.equ                               CR0_CD_MASK, (1 << 30)
.equ                               CR0_PG_MASK, (1 << 31)
.equ                               CR4_DE_MASK, (1 << 3)
.equ                               CR4_OSFXSR_MASK, (1 << 9)

#-----------------------------------------------------------------------------
# MTRRs
#
.equ                               IA32_MTRR_PHYSBASE0, 0x200
.equ                               IA32_MTRR_PHYSMASK0, 0x201
.equ                               IA32_MTRR_PHYSBASE1, 0x202
.equ                               IA32_MTRR_PHYSMASK1, 0x203
.equ                               IA32_MTRR_PHYSBASE2, 0x204
.equ                               IA32_MTRR_PHYSMASK2, 0x205
.equ                               IA32_MTRR_PHYSBASE3, 0x206
.equ                               IA32_MTRR_PHYSMASK3, 0x207
.equ                               IA32_MTRR_PHYSBASE4, 0x208
.equ                               IA32_MTRR_PHYSMASK4, 0x209
.equ                               IA32_MTRR_PHYSBASE5, 0x20A
.equ                               IA32_MTRR_PHYSMASK5, 0x20B
.equ                               IA32_MTRR_PHYSBASE6, 0x20C
.equ                               IA32_MTRR_PHYSMASK6, 0x20D
.equ                               IA32_MTRR_PHYSBASE7, 0x20E
.equ                               IA32_MTRR_PHYSMASK7, 0x20F
.equ                               IA32_MTRR_FIX64K_00000, 0x250
.equ                               IA32_MTRR_FIX16K_80000, 0x258
.equ                               IA32_MTRR_FIX16K_A0000, 0x259
.equ                               IA32_MTRR_FIX4K_C0000, 0x268
.equ                               IA32_MTRR_FIX4K_C8000, 0x269
.equ                               IA32_MTRR_FIX4K_D0000, 0x26A
.equ                               IA32_MTRR_FIX4K_D8000, 0x26B
.equ                               IA32_MTRR_FIX4K_E0000, 0x26C
.equ                               IA32_MTRR_FIX4K_E8000, 0x26D
.equ                               IA32_MTRR_FIX4K_F0000, 0x26E
.equ                               IA32_MTRR_FIX4K_F8000, 0x26F
.equ                               IA32_CR_PAT, 0x277
.equ                               IA32_MTRR_DEF_TYPE, 0x2FF
.equ                               IA32_MTRR_CAP, 0x0FE
.equ                               IA32_MTRR_SMRR_SUPPORT_BIT, (1 << 11)
.equ                               IA32_FEATURE_CONTROL, 0x03A
.equ                               IA32_SMRR_ENABLE_BIT, (1 << 3)

#
# Only low order bits are assumed
#
.equ                               MTRR_MASK, 0x0FFFFF000

.equ                               MTRR_ENABLE, (1 << 11)
.equ                               MTRR_FIXED_ENABLE, (1 << 10)

.equ                               MTRR_VALID, (1 << 11)
.equ                               UC, 0x00
.equ                               WB, 0x06

.equ                               MTRR_VCNT, 8

.equ                               MTRR_MASK_4K_LOW, 0x0fffff800
.equ                               MTRR_MASK_4K_HIGH, 0x0f
.equ                               UINT32_4K_MASK, 0x0fffff000

#----------------------------------------------------------------------------
# APIC definitions
#
.equ                               IA32_APIC_BASE, 0x001B   # APIC base MSR
.equ                               BASE_ADDR_MASK, 0x0FFFFF000
.equ                               APIC_ID, 0x20
.equ                               ICR_LOW, 0x300
.equ                               ICR_HIGH, 0x310
.equ                               SPURIOUS_VECTOR_1, 0x0F0

#-----------------------------------------------------------------------------
# Features support & enabling
#
.equ                               CR4_VMXE, (1 << 13)
.equ                               CR4_SMXE, (1 << 14)
.equ                               CR4_PAE_MASK, (1 << 5)

.equ                               CPUID_VMX, (1 << 5)
.equ                               CPUID_SMX, (1 << 6)

#-----------------------------------------------------------------------------
# Machne check architecture MSR registers
#        
.equ                               MCG_CAP, 0x179
.equ                               MCG_STATUS, 0x17A
.equ                               MCG_CTL, 0x17B
.equ                               MC0_CTL, 0x400
.equ                               MC0_STATUS, 0x401
.equ                               MC0_ADDR, 0x402
.equ                               MC0_MISC, 0x403

.macro _GETSEC
.byte 0x0f, 0x37
.endm

.equ                               ENTERACCS, 0x02
.equ                               PARAMETERS, 0x06


