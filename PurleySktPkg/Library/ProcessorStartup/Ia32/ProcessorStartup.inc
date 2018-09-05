;
; This file contains an 'Intel Pre-EFI Module' and is licensed
; for Intel CPUs and Chipsets under the terms of your license
; agreement with Intel or your vendor.  This file may be
; modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2015, Intel Corporation. All rights reserved.<BR>
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
; Module Name:
;
;  ProcessorStartup.inc
;
; Abstract:
;
;
;------------------------------------------------------------------------------
; MACROs used by Processor Startup Driver
GET_ABS_ADDR MACRO OFFSET_A

; Calculate address of OFFSET_A
  movd esi, mm2
  sub  esi, offset cs:OFFSET_A
  neg  esi

ENDM

CALL_MMX3 MACRO RoutineLabel

  LOCAL ReturnAddress

  GET_ABS_ADDR ReturnAddress

  movd    mm3, esi                      ; save ReturnAddress into MM3
  jmp     RoutineLabel                  ; Relative jump
ReturnAddress:

ENDM

RET_ESI_MMX3 MACRO

  movd    esi, mm3                      ; restore return address from MM3
  jmp     esi                           ; Absolute jump

ENDM


CALL_MMX4 MACRO RoutineLabel

  LOCAL ReturnAddress

  GET_ABS_ADDR ReturnAddress

  movd    mm4, esi                      ; save ReturnAddress into MM4
  jmp     RoutineLabel                  ; Relative jump
ReturnAddress:

ENDM


RET_ESI_MMX4 MACRO

  movd    esi, mm4                      ; restore return address from MM4
  jmp     esi                           ; Absolute jump

ENDM

CALL_MMX5 MACRO  RoutineLabel

  LOCAL ReturnAddress

  GET_ABS_ADDR ReturnAddress

  movd    mm5, esi                      ; save ReturnAddress into MM5
  jmp     RoutineLabel                  ; Relative jump
ReturnAddress:

ENDM

RET_ESI_MMX5 MACRO

  movd    esi, mm5                      ; restore return address from MM5
  jmp     esi                           ; Absolute jump

ENDM

CALL_MMX6 MACRO  RoutineLabel

  LOCAL ReturnAddress

  GET_ABS_ADDR ReturnAddress

  movd    mm6, esi                      ; save ReturnAddress into MM6
  jmp     RoutineLabel                  ; Relative jump
ReturnAddress:

ENDM

RET_ESI_MMX6 MACRO

  movd    esi, mm6                      ; restore return address from MM6
  jmp     esi                           ; Absolute jump

ENDM


;
; POST code macro
;
PORT80  MACRO Value
  mov al, Value
  out 80h, al
ENDM

;; EQUATEs used by Processor Startup Driver
MSR_XAPIC_BASE      EQU 01Bh


; Abstract:
;
;   IA32 architecture MSRs
;
;------------------------------------------------------------------------------

MTRR_CAP                      EQU 00FEh
MTRR_PHYS_BASE_0              EQU 0200h
MTRR_PHYS_MASK_0              EQU 0201h
MTRR_PHYS_BASE_1              EQU 0202h
MTRR_PHYS_MASK_1              EQU 0203h
MTRR_PHYS_BASE_2              EQU 0204h
MTRR_PHYS_MASK_2              EQU 0205h
MTRR_PHYS_BASE_3              EQU 0206h
MTRR_PHYS_MASK_3              EQU 0207h
MTRR_PHYS_BASE_4              EQU 0208h
MTRR_PHYS_MASK_4              EQU 0209h
MTRR_PHYS_BASE_5              EQU 020Ah
MTRR_PHYS_MASK_5              EQU 020Bh
MTRR_PHYS_BASE_6              EQU 020Ch
MTRR_PHYS_MASK_6              EQU 020Dh
MTRR_PHYS_BASE_7              EQU 020Eh
MTRR_PHYS_MASK_7              EQU 020Fh
MTRR_PHYS_BASE_8              EQU 0210h
MTRR_PHYS_MASK_8              EQU 0211h
MTRR_PHYS_BASE_9              EQU 0212h
MTRR_PHYS_MASK_9              EQU 0213h
MTRR_FIX_64K_00000            EQU 0250h
MTRR_FIX_16K_80000            EQU 0258h
MTRR_FIX_16K_A0000            EQU 0259h
MTRR_FIX_4K_C0000             EQU 0268h
MTRR_FIX_4K_C8000             EQU 0269h
MTRR_FIX_4K_D0000             EQU 026Ah
MTRR_FIX_4K_D8000             EQU 026Bh
MTRR_FIX_4K_E0000             EQU 026Ch
MTRR_FIX_4K_E8000             EQU 026Dh
MTRR_FIX_4K_F0000             EQU 026Eh
MTRR_FIX_4K_F8000             EQU 026Fh
MTRR_DEF_TYPE                 EQU 02FFh

MTRR_MEMORY_TYPE_UC           EQU 00h
MTRR_MEMORY_TYPE_WC           EQU 01h
MTRR_MEMORY_TYPE_WT           EQU 04h
MTRR_MEMORY_TYPE_WP           EQU 05h
MTRR_MEMORY_TYPE_WB           EQU 06h

MTRR_DEF_TYPE_E               EQU 0800h
MTRR_DEF_TYPE_FE              EQU 0400h
MTRR_PHYSMASK_VALID           EQU 0800h

;
; Define the high 32 bits of MTRR masking
; This should be read from CPUID EAX = 080000008h, EAX bits [7:0]
; But for most platforms this will be a fixed supported size so it is
; fixed to save space.
;
MTRR_PHYS_MASK_VALID          EQU 0800h
MTRR_PHYS_MASK_HIGH           EQU 00000000Fh      ; For 36 bit addressing
;MTRR_PHYS_MASK_HIGH           EQU 0000000FFh      ; For 40 bit addressing

IA32_MISC_ENABLE              EQU 1A0h
FAST_STRING_ENABLE_BIT        EQU 01h


VIRTUAL_MSR_MCA_ON_NON_NEW_CACHABLE_MMIO_EN_ADDR EQU 061h
MCAONNONNEMCACHEABLEMMIO_BIT  EQU 1h

CR0_CACHE_DISABLE             EQU 040000000h
CR0_NO_WRITE                  EQU 020000000h

IA32_PLATFORM_ID              EQU 017h
IA32_BIOS_UPDT_TRIG           EQU 079h
IA32_BIOS_SIGN_ID             EQU 08Bh
PLATFORM_INFO                 EQU 0CEh
NO_EVICT_MODE                 EQU 2E0h
NO_EVICTION_ENABLE_BIT        EQU 01h

;
; MSR definitions
;
MSR_IA32_PLATFORM_ID          EQU 0017h
MSR_APIC_BASE                 EQU 001Bh
MSR_DCU_MODE                  EQU 0031h
MSR_SOCKET_ID                 EQU 0039h
MSR_IA32_FEATURE_CONTROL      EQU 003Ah
MSR_IA32_BIOS_UPDT_TRIG       EQU 0079h
MSR_IA32_BIOS_SIGN_ID         EQU 008Bh
MSR_PLATFORM_INFO             EQU 00CEh
MSR_CLOCK_CST_CONFIG_CONTROL  EQU 00E2h
MSR_CLOCK_FLEX_MAX            EQU 0194h
MSR_IA32_PERF_STS             EQU 0198h
MSR_IA32_PERF_CTL             EQU 0199h
MSR_IA32_MISC_ENABLES         EQU 01A0h
MSR_IA32_DCA_CAP              EQU 01F9h
MSR_POWER_CTL                 EQU 01FCh
MSR_IA32_MC8_MISC2            EQU 0288h
MSR_IA32_MC7_CTL              EQU 041Ch
MSR_IA32_MC7_STATUS           EQU 0419h
MSR_IA32_MC8_STATUS           EQU 041Dh
MSR_PRIMARy_PLANE_CFG_CTRL    EQU 0601h
MSR_CPU_BUS_NUMBER            EQU 0300h
MSR_IA32_DEBUG_INTERFACE      EQU 0C80h  ; IA32_DEBUG_INTERFACE_MSR
MSR_BOOT_GUARD_SACM_INFO      EQU 013Ah  ; BootGuard Info MSR
MSR_BC_PBEC                   EQU 0139h  ; BootGuard PBE TIMER MSR
;


;
; POST code macro
;
PORT80  MACRO Value
  mov al, Value
  out 80h, al
ENDM

;
; Cache control macro
;
DISABLE_CACHE macro
    mov   eax, cr0
    or    eax, CR0_CACHE_DISABLE + CR0_NO_WRITE
    wbinvd
    mov   cr0, eax
endm

ENABLE_CACHE  macro
    mov   eax, cr0
    and   eax, NOT (CR0_CACHE_DISABLE + CR0_NO_WRITE)
    wbinvd
    mov   cr0, eax
endm

;------------------------------------------------------------------------------
; macro to read a PCI config dword via lagacy CF8/CFC method
;------------------------------------------------------------------------------
READ_PCI_DWORD_LEGACY macro
; input: EAX = legacy PCI address format with bit31 = 1
; output: EBX = dword register data
;         DX is destroyed
   mov   dx, 0cf8h
   out   dx, eax
   xchg  eax, ebx
   add   dx, 4
   in    eax, dx
   xchg  eax, ebx
endm

; macro to write a PCI config dword via lagacy CF8/CFC method
; input:  EAX = legacy PCI address format with bit31 = 1
;         EBX = dword data to write
; output: DX is destroyed
WRITE_PCI_DWORD_LEGACY macro
   mov   dx, 0cf8h
   out   dx, eax
   xchg  eax, ebx
   add   dx, 4
   out   dx, eax
   xchg  eax, ebx
endm


UpdateHeaderStruc  STRUC
  dHeaderVersion        dd  ?           ; Header version#
  dUpdateRevision       dd  ?           ; Update revision#
  dDate                 dd  ?           ; Date in binary (08/13/07 as 0x08132007)
  dProcessorSignature   dd  ?           ; CPU type, family, model, stepping
  dChecksum             dd  ?           ; Checksum
  dLoaderRevision       dd  ?           ; Update loader version#
  dProcessorFlags       dd  ?           ; Processor Flags
  dDataSize             dd  ?           ; Size of encrypted data
  dTotalSize            dd  ?           ; Total size of update in bytes
  bReserved             db  12 dup(?)   ; 12 bytes reserved
UpdateHeaderStruc ENDS



ext_sig_hdr   STRUC
  count        dd ?
  checksum     dd ?
  rsvd         dd 3 dup (?)
ext_sig_hdr ENDS


ext_sig   STRUC
  processor    dd ?
  flags        dd ?
  checksum     dd ?
ext_sig   ENDS

