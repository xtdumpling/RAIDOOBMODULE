#
# This file contains an 'Intel Pre-EFI Module' and is licensed
# for Intel CPUs and Chipsets under the terms of your license
# agreement with Intel or your vendor.  This file may be
# modified by the user, subject to additional terms of the
# license agreement
#
#------------------------------------------------------------------------------
#
# Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>
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
#  ProcessorStartup.i
#
# Abstract:
#
#
#------------------------------------------------------------------------------
# MACROs used by Processor Startup Driver

.macro GET_ABS_ADDR OFFSET_A

  movd    %mm2, %esi
  subl    $\OFFSET_A, %esi
  negl    %esi

.endm

.macro CALL_MMX3 RoutineLabel

  GET_ABS_ADDR  1f
  movd    %esi, %mm3                    # save ReturnAddress into MM3
  jmp     \RoutineLabel                 # Relative jump
1:

.endm

.macro RET_ESI_MMX3 MACRO

  movd    %mm3, %esi                    # restore return address from MM3
  jmp     *%esi                         # Absolute jump

.endm


.macro CALL_MMX4 RoutineLabel

  GET_ABS_ADDR  1f
  movd    %esi, %mm4                      # save ReturnAddress into MM4
  jmp     \RoutineLabel                   # Relative jump
1:

.endm


.macro RET_ESI_MMX4

  movd    %mm4, %esi                      # restore return address from MM4
  jmp     *%esi                           # Absolute jump

.endm

.macro CALL_MMX5 RoutineLabel

  GET_ABS_ADDR  1f
  movd    %esi, %mm5                      # save ReturnAddress into MM5
  jmp     \RoutineLabel                   # Relative jump
1:

.endm

.macro RET_ESI_MMX5

  movd    %mm5, %esi                      # restore return address from MM5
  jmp     *%esi                           # Absolute jump

.endm

.macro CALL_MMX6 RoutineLabel

  GET_ABS_ADDR  1f
  movd    %esi, %mm6                      # save ReturnAddress into MM6
  jmp     \RoutineLabel                   # Relative jump
1:

.endm

.macro RET_ESI_MMX6

  movd    %mm6, %esi                      # restore return address from MM6
  jmp     *%esi                           # Absolute jump

.endm


#
# POST code macro
#
.macro PORT80 Value
  movb $\Value, %al
  outb %al, $0x80
.endm

## EQUATEs used by Processor Startup Driver
.equ                MSR_XAPIC_BASE, 0x01B


# Abstract:
#
#   IA32 architecture MSRs
#
#------------------------------------------------------------------------------

.equ                          MTRR_CAP, 0x00FE
.equ                          MTRR_PHYS_BASE_0, 0x0200
.equ                          MTRR_PHYS_MASK_0, 0x0201
.equ                          MTRR_PHYS_BASE_1, 0x0202
.equ                          MTRR_PHYS_MASK_1, 0x0203
.equ                          MTRR_PHYS_BASE_2, 0x0204
.equ                          MTRR_PHYS_MASK_2, 0x0205
.equ                          MTRR_PHYS_BASE_3, 0x0206
.equ                          MTRR_PHYS_MASK_3, 0x0207
.equ                          MTRR_PHYS_BASE_4, 0x0208
.equ                          MTRR_PHYS_MASK_4, 0x0209
.equ                          MTRR_PHYS_BASE_5, 0x020A
.equ                          MTRR_PHYS_MASK_5, 0x020B
.equ                          MTRR_PHYS_BASE_6, 0x020C
.equ                          MTRR_PHYS_MASK_6, 0x020D
.equ                          MTRR_PHYS_BASE_7, 0x020E
.equ                          MTRR_PHYS_MASK_7, 0x020F
.equ                          MTRR_PHYS_BASE_8, 0x0210
.equ                          MTRR_PHYS_MASK_8, 0x0211
.equ                          MTRR_PHYS_BASE_9, 0x0212
.equ                          MTRR_PHYS_MASK_9, 0x0213
.equ                          MTRR_FIX_64K_00000, 0x0250
.equ                          MTRR_FIX_16K_80000, 0x0258
.equ                          MTRR_FIX_16K_A0000, 0x0259
.equ                          MTRR_FIX_4K_C0000, 0x0268
.equ                          MTRR_FIX_4K_C8000, 0x0269
.equ                          MTRR_FIX_4K_D0000, 0x026A
.equ                          MTRR_FIX_4K_D8000, 0x026B
.equ                          MTRR_FIX_4K_E0000, 0x026C
.equ                          MTRR_FIX_4K_E8000, 0x026D
.equ                          MTRR_FIX_4K_F0000, 0x026E
.equ                          MTRR_FIX_4K_F8000, 0x026F
.equ                          MTRR_DEF_TYPE, 0x02FF

.equ                          MTRR_MEMORY_TYPE_UC, 0x00
.equ                          MTRR_MEMORY_TYPE_WC, 0x01
.equ                          MTRR_MEMORY_TYPE_WT, 0x04
.equ                          MTRR_MEMORY_TYPE_WP, 0x05
.equ                          MTRR_MEMORY_TYPE_WB, 0x06

.equ                          MTRR_DEF_TYPE_E, 0x0800
.equ                          MTRR_DEF_TYPE_FE, 0x0400
.equ                          MTRR_PHYSMASK_VALID, 0x0800

#
# Define the high 32 bits of MTRR masking
# This should be read from CPUID EAX = 080000008h, EAX bits [7:0]
# But for most platforms this will be a fixed supported size so it is
# fixed to save space.
#
.equ                          MTRR_PHYS_MASK_VALID, 0x0800
.equ                          MTRR_PHYS_MASK_HIGH, 0x00000000       # For 36 bit addressing
#MTRR_PHYS_MASK_HIGH           EQU 0000000FFh      ; For 40 bit addressing

.equ                          IA32_MISC_ENABLE, 0x1A0
.equ                          FAST_STRING_ENABLE_BIT, 0x01


.equ                          VIRTUAL_MSR_MCA_ON_NON_NEW_CACHABLE_MMIO_EN_ADDR, 0x061
.equ                          MCAONNONNEMCACHEABLEMMIO_BIT, 1

.equ                          CR0_CACHE_DISABLE, 0x040000000
.equ                          CR0_NO_WRITE, 0x020000000

.equ                          IA32_PLATFORM_ID, 0x017
.equ                          IA32_BIOS_UPDT_TRIG, 0x079
.equ                          IA32_BIOS_SIGN_ID, 0x08B
.equ                          PLATFORM_INFO, 0x0CE
.equ                          NO_EVICT_MODE, 0x2E0
.equ                          NO_EVICTION_ENABLE_BIT, 0x01

#
# MSR definitions
#
.equ                          MSR_IA32_PLATFORM_ID, 0x0017
.equ                          MSR_APIC_BASE, 0x001B
.equ                          MSR_DCU_MODE, 0x0031
.equ                          MSR_SOCKET_ID, 0x0039
.equ                          MSR_IA32_FEATURE_CONTROL, 0x003A
.equ                          MSR_IA32_BIOS_UPDT_TRIG, 0x0079
.equ                          MSR_IA32_BIOS_SIGN_ID, 0x008B
.equ                          MSR_PLATFORM_INFO, 0x00CE
.equ                          MSR_CLOCK_CST_CONFIG_CONTROL, 0x00E2
.equ                          MSR_CLOCK_FLEX_MAX, 0x0194
.equ                          MSR_IA32_PERF_STS, 0x0198
.equ                          MSR_IA32_PERF_CTL, 0x0199
.equ                          MSR_IA32_MISC_ENABLES, 0x01A0
.equ                          MSR_IA32_DCA_CAP, 0x01F9
.equ                          MSR_POWER_CTL, 0x01FC
.equ                          MSR_IA32_MC8_MISC2, 0x0288
.equ                          MSR_IA32_MC7_CTL, 0x041C
.equ                          MSR_IA32_MC7_STATUS, 0x0419
.equ                          MSR_IA32_MC8_STATUS, 0x041D
.equ                          MSR_PRIMARy_PLANE_CFG_CTRL, 0x0601
.equ                          MSR_CPU_BUS_NUMBER, 0x0300
.equ                          MSR_IA32_DEBUG_INTERFACE, 0x0C80   # IA32_DEBUG_INTERFACE_MSR
.equ                          MSR_BOOT_GUARD_SACM_INFO, 0x013A   # BootGuard Info MSR
.equ                          MSR_BC_PBEC, 0x0139   # BootGuard PBE TIMER MSR


#
# Cache control macro
#
.macro DISABLE_CACHE
    movl  cr0, %eax
    orl   (CR0_CACHE_DISABLE + CR0_NO_WRITE), %eax
    wbinvd
    movl  %eax, cr0
.endm

.macro ENABLE_CACHE
    movl  cr0, %eax
    andl  ~(CR0_CACHE_DISABLE + CR0_NO_WRITE), %eax
    wbinvd
    movl  %eax, cr0
.endm

#------------------------------------------------------------------------------
# macro to read a PCI config dword via lagacy CF8/CFC method
#------------------------------------------------------------------------------
.macro READ_PCI_DWORD_LEGACY
# input: EAX = legacy PCI address format with bit31 = 1
# output: EBX = dword register data
#         DX is destroyed
   movw  $0xcf8, %dx
   outl  %eax, %dx
   xchgl %ebx, %eax
   addw  $4, %dx
   in   %dx, %eax
   xchgl %ebx, %eax
.endm

# macro to write a PCI config dword via lagacy CF8/CFC method
# input:  EAX = legacy PCI address format with bit31 = 1
#         EBX = dword data to write
# output: DX is destroyed
.macro WRITE_PCI_DWORD_LEGACY
   movw  $0xcf8, %dx
   outl  %eax, %dx
   xchgl %ebx, %eax 
   addw  $4, %dx
   outl  %eax, %dx
   xchgl %ebx, %eax
.endm

.equ UpdateHeaderStruc_dHeaderVersion,        0        # Header version#
.equ UpdateHeaderStruc_dUpdateRevision,       4        # Update revision#
.equ UpdateHeaderStruc_dDate,                 8        # Date in binary (08/13/07 as 0x081
.equ UpdateHeaderStruc_dProcessorSignature,  12        # CPU type, family, model, stepping
.equ UpdateHeaderStruc_dChecksum,            16        # Checksum
.equ UpdateHeaderStruc_dLoaderRevision,      20        # Update loader version#
.equ UpdateHeaderStruc_dProcessorFlags,      24        # Processor Flags
.equ UpdateHeaderStruc_dDataSize,            28        # Size of encrypted data
.equ UpdateHeaderStruc_dTotalSize,           32        # Total size of update in bytes
.equ UpdateHeaderStruc_bReserved,            36
.equ UpdateHeaderStruc_END,                  48

.equ ext_sig_hdr_count,                       0
.equ ext_sig_hdr_checksum,                    4
.equ ext_sig_hdr_rsvd,                        8
.equ ext_sig_hdr_END,                        20

.equ ext_sig_processor,                       0
.equ ext_sig_flags,                           4
.equ ext_sig_checksum,                        8
.equ ext_sig_END,                            12
