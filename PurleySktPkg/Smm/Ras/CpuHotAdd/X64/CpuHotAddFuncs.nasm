;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;-------------------------------------------------------------------------------
;
; Copyright (c) 2011 - 2016 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
;
; Module Name:
;
;   CpuHotAddFuncs.nasm
;
; Abstract:
;
;   This is the assembly code for EM64T MP support
;
;-------------------------------------------------------------------------------

%define VacantFlag 0x0
%define NotVacantFlag 0xff

;text      SEGMENT
DEFAULT REL
SECTION .text

global ASM_PFX(AsmRsm)
ASM_PFX(AsmRsm):
        rsm
        ret

global ASM_PFX(AsmAcquireMPLock)
ASM_PFX(AsmAcquireMPLock):

        mov         al, NotVacantFlag
TryGetLock:
        xchg        al, byte [rcx]
        cmp         al, VacantFlag
        jz          LockObtained

        pause
        jmp         TryGetLock

LockObtained:
        ret

global ASM_PFX(AsmReleaseMPLock)
ASM_PFX(AsmReleaseMPLock):

        mov         al, VacantFlag
        xchg        al, byte [rcx]

        ret

;IFDEF LT_FLAG
%define MOVQIN0 db 0x48, 0xf, 0x6e, 0xc0 ; movq mm0, rax
%define MOVQOUT0 db 0x48, 0xf, 0x7e, 0xc0 ; movq rax, mm0
%define MOVDOUT0 db 0xf, 0x7e, 0xc0 ; movd eax, mm0

%define MOVQIN1 db 0x48, 0xf, 0x6e, 0xc8 ; movq mm1, rax
%define MOVQOUT1 db 0x48, 0xf, 0x7e, 0xc8 ; movq rax, mm1
%define MOVDOUT1 db 0xf, 0x7e, 0xc8 ; movd eax, mm1

%define MOVQIN2 db 0x48, 0xf, 0x6e, 0xd0 ; movq mm2, rax
%define MOVQOUT2 db 0x48, 0xf, 0x7e, 0xd0 ; movq rax, mm2
%define MOVDOUT2 db 0xf, 0x7e, 0xd0 ; movd eax, mm2

%define MOVQIN3 db 0x48, 0xf, 0x6e, 0xd8 ; movq mm3, rax
%define MOVQOUT3 db 0x48, 0xf, 0x7e, 0xd8 ; movq rax, mm3
%define MOVDOUT3 db 0xf, 0x7e, 0xd8 ; movd eax, mm3

%define MOVQIN4 db 0x48, 0xf, 0x6e, 0xe0 ; movq mm4, rax
%define MOVQOUT4 db 0x48, 0xf, 0x7e, 0xe0 ; movq rax, mm4
%define MOVDOUT4 db 0xf, 0x7e, 0xe0 ; movd eax, mm4

%define MOVQIN5 db 0x48, 0xf, 0x6e, 0xe8 ; movq mm5, rax
%define MOVQOUT5 db 0x48, 0xf, 0x7e, 0xe8 ; movq rax, mm5
%define MOVDOUT5 db 0xf, 0x7e, 0xe8 ; movd eax, mm5

%define MOVQIN6 db 0x48, 0xf, 0x6e, 0xf0 ; movq mm6, rax
%define MOVQOUT6 db 0x48, 0xf, 0x7e, 0xf0 ; movq rax, mm6
%define MOVDOUT6 db 0xf, 0x7e, 0xf0 ; movd eax, mm6

%define MOVQIN7 db 0x48, 0xf, 0x6e, 0xf8 ; movq mm7, rax
%define MOVQOUT7 db 0x48, 0xf, 0x7e, 0xf8 ; movq rax, mm7
%define MOVDOUT7 db 0xf, 0x7e, 0xf8 ; movd eax, mm7

%define _EMMS db 0xf, 0x77

;-----------------------------------------------------------------------------
;  Macro: PUSHA_64
;
;  Description: Saves all registers on stack
;
;  Input:   None
;
;  Output:  None
;-----------------------------------------------------------------------------
%macro PUSHA_64 0
    push    rsp
    push    rbp
    push    rax
    push    rbx
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15
%endmacro

;-----------------------------------------------------------------------------
;  Macro: POPA_64
;
;  Description: Restores all registers from stack
;
;  Input:   None
;
;  Output:  None
;-----------------------------------------------------------------------------
%macro POPA_64 0
    pop    r15
    pop    r14
    pop    r13
    pop    r12
    pop    r11
    pop    r10
    pop    r9
    pop    r8
    pop    rdi
    pop    rsi
    pop    rdx
    pop    rcx
    pop    rbx
    pop    rax
    pop    rbp
    pop    rsp
%endmacro

;
; MSRs
;
%define IA32_CR_FEATURE_CONTROL 0x3A
%define IA32_MISC_ENABLE 0x1A0
%define LT_OPT_IN_MSR_VALUE 01111111100000011b ; 0FF03h
%define LT_OPT_IN_VMX_ONLY_MSR_VALUE 00000000000000101b ; 00005h

%define ACMOD_SIZE 24

%define CR0_NE_MASK (1 SHL 5)
%define CR0_NW_MASK (1 SHL 29)
%define CR0_CD_MASK (1 SHL 30)
%define CR0_PG_MASK (1 SHL 31)
%define CR4_DE_MASK (1 SHL 3)
%define CR4_OSFXSR_MASK (1 SHL 9)

;-----------------------------------------------------------------------------
; MTRRs
;
%define IA32_MTRR_PHYSBASE0 0x200
%define IA32_MTRR_PHYSMASK0 0x201
%define IA32_MTRR_PHYSBASE1 0x202
%define IA32_MTRR_PHYSMASK1 0x203
%define IA32_MTRR_PHYSBASE2 0x204
%define IA32_MTRR_PHYSMASK2 0x205
%define IA32_MTRR_PHYSBASE3 0x206
%define IA32_MTRR_PHYSMASK3 0x207
%define IA32_MTRR_PHYSBASE4 0x208
%define IA32_MTRR_PHYSMASK4 0x209
%define IA32_MTRR_PHYSBASE5 0x20A
%define IA32_MTRR_PHYSMASK5 0x20B
%define IA32_MTRR_PHYSBASE6 0x20C
%define IA32_MTRR_PHYSMASK6 0x20D
%define IA32_MTRR_PHYSBASE7 0x20E
%define IA32_MTRR_PHYSMASK7 0x20F
%define IA32_MTRR_FIX64K_00000 0x250
%define IA32_MTRR_FIX16K_80000 0x258
%define IA32_MTRR_FIX16K_A0000 0x259
%define IA32_MTRR_FIX4K_C0000 0x268
%define IA32_MTRR_FIX4K_C8000 0x269
%define IA32_MTRR_FIX4K_D0000 0x26A
%define IA32_MTRR_FIX4K_D8000 0x26B
%define IA32_MTRR_FIX4K_E0000 0x26C
%define IA32_MTRR_FIX4K_E8000 0x26D
%define IA32_MTRR_FIX4K_F0000 0x26E
%define IA32_MTRR_FIX4K_F8000 0x26F
%define IA32_CR_PAT 0x277
%define IA32_MTRR_DEF_TYPE 0x2FF
%define IA32_MTRR_CAP 0xFE
%define IA32_MTRR_SMRR_SUPPORT_BIT (1 SHL 11)
%define IA32_FEATURE_CONTROL 0x3A
%define IA32_SMRR_ENABLE_BIT (1 SHL 3)

;
; Only low order bits are assumed
;
%define MTRR_MASK 0xFFFFF000

%define MTRR_ENABLE (1 SHL 11)
%define MTRR_FIXED_ENABLE (1 SHL 10)

%define MTRR_VALID (1 SHL 11)
%define UC 0x0
%define WB 0x6

%define MTRR_VCNT 8

%define MTRR_MASK_4K_LOW 0xfffff800
%define MTRR_MASK_4K_HIGH 0xf
%define UINT32_4K_MASK 0xfffff000

%define ACPIBASE 0x8000FA40  ;D31:F2:R40h
%define ACPIBASE_MASK 0xFF00
%define SMI_EN_OFFSET 0x30

;----------------------------------------------------------------------------
; APIC definitions
;
%define IA32_APIC_BASE 0x1B  ; APIC base MSR
%define IA32_APIC_BASE_G_XAPIC BIT11
%define IA32_APIC_BASE_M_XAPIC BIT10
%define IA32_APIC_BASE_BSP 0x100
%define BASE_ADDR_MASK 0xFFFFF000
%define APIC_ID 0x20
%define ICR_LOW 0x300
%define ICR_HIGH 0x310
%define SPURIOUS_VECTOR_1 0xF0

;-----------------------------------------------------------------------------
; Features support & enabling
;
%define CR4_VMXE (1 SHL 13)
%define CR4_SMXE (1 SHL 14)
%define CR4_PAE_MASK (1 SHL 5)

%define CPUID_VMX (1 SHL 5)
%define CPUID_SMX (1 SHL 6)

;-----------------------------------------------------------------------------
; Machne check architecture MSR registers
;
%define MCG_CAP 0x179
%define MCG_STATUS 0x17A
%define MCG_CTL 0x17B
%define MC0_CTL 0x400
%define MC0_STATUS 0x401
%define MC0_ADDR 0x402
%define MC0_MISC 0x403

%define _GETSEC db 0xf, 0x37

%define ENTERACCS 0x2
%define PARAMETERS 0x6

global ASM_PFX(LaunchBiosAcm)
ASM_PFX(LaunchBiosAcm):  ;near PUBLIC
;    START_FRAME
;    MAKE_LOCAL BIOS_GDT[2]:QWORD
;    MAKE_LOCAL BIOS_IDT[2]:QWORD
;    END_FRAME

     enter  0x30,0

%define BIOS_GDT [rbp-0x10]
%define BIOS_IDT [rbp-0x20]

%define ACM_SIZE_TO_CACHE  xmm0
%define ACM_BASE_TO_CACHE  xmm1
%define NEXT_MTRR_INDEX    xmm2
%define NEXT_MTRR_SIZE     xmm3

;-----------------------------------------------------------------------------
;  Procedure:   LaunchBiosAcm
;
;  Input:       AcmBase  - Base address of LT BIOS ACM in flash
;               EsiValue -
;
;  Output:
;
;  Stack:       Available at beginning of routine, but routine goes 'stackless'
;
;  Registers:
;
;  Description: Setup GETSEC environment (protected mode, mtrrs, etc) and
;                 invoke GETSEC:ENTERACCS with requested BIOS ACM entry point.
;
;-----------------------------------------------------------------------------

    ;
    ; Save the general purpose register state
    ;
    pushfq
    PUSHA_64

    ;
    ; Save the parameters passed to us
    ;
    mov     rax, rcx        ; save address of BIOS ACM in MMX0
    MOVQIN0
    mov     rax, cr3        ; save page table in MMX1
    MOVQIN1
    mov     rax, rdx        ; save value of ESI for GETSEC[ENTERACCS] in MMX2
    MOVQIN2
    or      rdx, rdx        ; have we been called to run SCLEAN?
    jz      BeginLaunch     ; if so, we don't need to save state.
    cmp     rbx, 5          ; have we been called to run ClearSecretsFlag?
    je      BeginLaunch     ; if so, we don't need to save state.

    sgdt  BIOS_GDT              ; save gdtr
    sidt  BIOS_IDT              ; save idtr

    lea   rax, BIOS_GDT         ; rax = address of saved gdtr
    MOVQIN3                     ; mm3 = address of saved gdtr

    mov     rax, rbp
    MOVQIN4                 ; mm4 = rbp
    xor     rax, rax
    mov     ax, ss
    MOVQIN5                 ; mm5 = ss
    mov     ax, cs
    MOVQIN6                 ; mm6 = cs

    ;
    ; Save control registers
    ;
    mov     rax, cr4
    push    rax
    mov     rax, cr0
    push    rax

    ;
    ; Save MISC ENABLE MSR
    ;
    mov     rcx, IA32_MISC_ENABLE
    rdmsr
    push    rax
    push    rdx

    ;
    ; Save MTRR
    ;
    mov     rcx, IA32_MTRR_CAP
    rdmsr
    and     rax, 0xFF
    shl     rax, 1
    mov     rcx, rax

SaveNextMtrr:
    add     rcx, IA32_MTRR_PHYSBASE0 - 1
    rdmsr
    push    rdx
    push    rax
    sub     rcx, IA32_MTRR_PHYSBASE0 - 1
    loop    SaveNextMtrr

    ; Save def MTRR type
    mov     rcx, IA32_MTRR_DEF_TYPE
    rdmsr
    push    rax
    push    rdx

    mov     dx, 0xcf8
    mov     eax, ACPIBASE
    out     dx, eax
    add     dx, 4
    in      eax, dx
    mov     dx, ax
    and     dx, ACPIBASE_MASK
    add     dx, SMI_EN_OFFSET   ;SMI_EN
    in      al, dx
    push    rdx        ;get PMBase to disable SMI_EN, TBD
    push    rax        ;
    and     al, 0xFE
    out     dx, al     ;disable SMI

    ; Set BSP bit
    mov     rcx, IA32_APIC_BASE
    rdmsr
    push    rax
    push    rdx
    or      eax, IA32_APIC_BASE_BSP
    wrmsr

    ; Save the rest of the segment registers
    xor     rax, rax
    mov     ax, gs
    push    rax
    mov     ax, fs
    push    rax
    mov     ax, es
    push    rax
    mov     ax, ds
    push    rax

    ;
    ; For reloading CS to Long Mode
    ;
    mov     rcx, cs
    shl     rcx, 32
    mov     rdx, ReloadCS   ; Assume it is below 4G
    or      rcx, rdx
    push    rcx

BeginLaunch:
    ;
    ; Now that all of our state is on the stack, save esp
    ;
    mov     rax, rsp
    MOVQIN7                 ; mm7 = rsp

    ;--------------------------------
    ; Begin to launch ACM
    ;--------------------------------

    ;
    ; Enable SMXE, SSE and debug extensions
    ;
    mov         rax, cr4
    or          rax, 0x200 + 0x8 + 0x4000 ;CR4_OSFXSR_MASK + CR4_DE_MASK + CR4_SMXE
    mov         cr4, rax

    ;
    ; Disable cache
    ;
    mov         rax, cr0    ; set CR0:CD and CR0:NE, clear CR0:NW
    or          rax, 0x40000000 | 0x20 ;CR0_CD_MASK OR CR0_NE_MASK
    and         rax, ~0x20000000
    mov         cr0, rax

    ;
    ; Check to see how we should invalidate the cache
    ;
    MOVQOUT2
    or      rax, rax        ; have we been called to run SCLEAN?
    jz      ScleanInvd
    wbinvd                  ; Nope, SCHECK.  Writeback is necessary
    jmp     OverScleanInvd

ScleanInvd:
    invd                    ; Yep, SCLEAN. Invalidate the cache

OverScleanInvd:
    ;
    ; Disable MTRRs, set Default Type to UC
    ;
    mov     rcx, IA32_MTRR_DEF_TYPE
    xor     rax, rax
    xor     rdx, rdx
    wrmsr

    ;
    ; Clear all of the Variable MTRRs
    ;
    mov     rcx, IA32_MTRR_CAP
    rdmsr
    and     rax, 0xFF
    shl     rax, 1
    mov     rcx, rax
    xor     rax, rax
    xor     rdx, rdx
ClearMtrrContinue:
    add     rcx, IA32_MTRR_PHYSBASE0 - 1
    wrmsr
    sub     rcx, IA32_MTRR_PHYSBASE0 - 1
    loop    ClearMtrrContinue

    ;
    ; Determine size of AC module
    ;
    MOVQOUT0                            ; rax = AcmBase
    mov         rsi, rax                ; rsi = AcmBase
    mov         rax, [rsi+ACMOD_SIZE]   ; rax = size of ACM
    shl         rax, 2                  ;  ...in bytes (ACM header has size in dwords)

    ;
    ; Round up to page size
    ;
    mov         rcx, rax                        ; Save
    and         rcx, 0xFFFFF000                 ; Number of pages in AC module
    and         rax, 0xFFF                      ; Number of "less-than-page" bytes
    jz          rounded
    mov         rax, 0x1000                      ; Add the whole page size

rounded:
    add         rax, rcx                        ; rax = rounded up AC module size
    mov         rbx, rax                        ; rbx = rounded up AC module size

    ;
    ; Initialize "locals"
    ;
    xor     rcx, rcx
    movd    NEXT_MTRR_INDEX, ecx    ; Start from MTRR0

    ;
    ; Save remaining size to cache
    ;
    movd    ACM_SIZE_TO_CACHE, ebx  ; Size of ACM that must be cached
    movd    ACM_BASE_TO_CACHE, esi  ; Base ACM address

nextMtrr:
    ;
    ; Get remaining size to cache
    ;
    xor     rax, rax
    movd    eax, ACM_SIZE_TO_CACHE
    and     rax, rax
    jz      done                    ; If no left size - we are done
    ;
    ; Determine next size to cache.
    ; We start from bottom up. Use the following algorythm:
    ; 1. Get our own alignment. Max size we can cache equals to our alignment
    ; 2. Determine what is bigger - alignment or remaining size to cache.
    ;    If aligment is bigger - cache it.
    ;      Adjust remaing size to cache and base address
    ;      Loop to 1.
    ;    If remaining size to cache is bigger
    ;      Determine the biggest 2^N part of it and cache it.
    ;      Adjust remaing size to cache and base address
    ;      Loop to 1.
    ; 3. End when there is no left size to cache or no left MTRRs
    ;
    xor     rsi, rsi
    movd    esi, ACM_BASE_TO_CACHE
    bsf     rcx, rsi                ; Get index of lowest bit set in base address
    ;
    ; Convert index into size to be cached by next MTRR
    ;
    mov     rdx, 0x1
    shl     rdx, cl                 ; Alignment is in rdx
    cmp     rdx, rax                ; What is bigger, alignment or remaining size?
    jbe     gotSize                 ; JIf aligment is less
    ;
    ; Remaining size is bigger. Get the biggest part of it, 2^N in size
    ;
    bsr     rcx, rax                ; Get index of highest set bit
    ;
    ; Convert index into size to be cached by next MTRR
    ;
    mov     rdx, 1
    shl     rdx, cl                 ; Size to cache

gotSize:
    mov     rax, rdx
    movd    NEXT_MTRR_SIZE, eax     ; Save

    ;
    ; Compute MTRR mask value:  Mask = NOT (Size - 1)
    ;
    dec     rax                     ; rax - size to cache less one byte
    not     eax                     ; eax contains low 32 bits of mask
    or      rax, 0x800 ;MTRR_VALID         ; Set valid bit
    ;
    ; Program mask register
    ;
    mov     rcx, IA32_MTRR_PHYSMASK0 ; setup variable mtrr
    xor     rbx, rbx
    movd    ebx, NEXT_MTRR_INDEX
    add     rcx, rbx

    mov     rdx, 0xF        ; 8K range (FFFFFFE800)
    wrmsr
    ;
    ; Program base register
    ;
    xor     rdx, rdx
    mov     rcx, IA32_MTRR_PHYSBASE0 ; setup variable mtrr
    add     ecx, ebx                 ; ebx is still NEXT_MTRR_INDEX

    xor     rax, rax
    movd    eax, ACM_BASE_TO_CACHE
    or      eax, WB                 ; set type to write back
    wrmsr
    ;
    ; Advance and loop
    ; Reduce remaining size to cache
    ;
    movd    ebx, ACM_SIZE_TO_CACHE
    movd    eax, NEXT_MTRR_SIZE
    sub     ebx, eax
    movd    ACM_SIZE_TO_CACHE, ebx

    ;
    ; Increment MTRR index
    ;
    movd    ebx, NEXT_MTRR_INDEX
    add     ebx, 2
    movd    NEXT_MTRR_INDEX, ebx
    ;
    ; Increment base address to cache
    ;
    movd    ebx, ACM_BASE_TO_CACHE
    movd    eax, NEXT_MTRR_SIZE
    add     ebx, eax
    movd    ACM_BASE_TO_CACHE, ebx

    jmp     nextMtrr
done:

    ;
    ; Re-enable Variable MTRRs
    ;
    mov     rcx, IA32_MTRR_DEF_TYPE
    xor     rdx, rdx
    mov     rax, 0x800 ;MTRR_ENABLE
    wrmsr

    ;
    ; Re-enable cache
    ;
    mov     rax, cr0
    and     rax, ~0x40000000
    mov     cr0, rax

    ;
    ; Clean all MCi_STATUS MSR registers
    ; SCLEAN will generate GPF otherwise
    ;
    mov     rcx, MCG_CAP
    rdmsr
    movzx   rbx, al                 ; rbx = MCR bank count
    xor     rax, rax                ; Write 0 into all MCi_STATUS registers
    xor     rdx, rdx
    mov     rcx, MC0_STATUS

McaErrorCleanLoopStart:
    wrmsr
    dec     rbx
    jz      CallGetsec
    add     rcx, 4                  ; rcx = number of MSRs per bank
    jmp     McaErrorCleanLoopStart

CallGetsec:
    ;
    ; Change to Compatible Segment
    ;
    mov     rcx, 0x20               ; Hardcode Compatible mode segment
    shl     rcx, 32
    mov     rdx, Compatible ; assume address < 4G
    or      rcx, rdx
    push    rcx
    retf

Compatible:
    ;
    ; disable paging
    ;
    mov     rax, cr0
    btr     eax, 31 ; set PG=0
    mov     cr0, rax
    ;
    ; set EFER.LME = 0 to leave long mode
    ;
    mov     ecx, 0xc0000080 ; EFER MSR number.
    rdmsr                   ; Read EFER.
    btr     eax, 8          ; Set LME=0.
    wrmsr                   ; Write EFER.

    ;
    ; Call GETSEC[ENTERACCS]
    ;
    MOVDOUT2                        ; eax = ACM function
    mov     esi, eax                ; esi = ACM function
    MOVDOUT0                        ; eax = AcmBase
    mov     ebx, eax                ; ebx = AcmBase
    mov     ecx, DWORD [rbx+ACMOD_SIZE]   ; ecx = size of ACM in dwords
    shl     ecx, 2                            ; ecx = size of ACM in bytes
    xor     edx, edx
    xor     edi, edi
    mov     eax, ENTERACCS          ; eax = ENTERACCS

    _GETSEC

    ;
    ; Check if we need to restore BIOS state and configuration
    ;
    MOVDOUT2
    or      eax, eax        ; Have we been called to run SCLEAN?
    jz      ResetSystem     ; Jump to ResetSystem, no-need to restore state,
                            ; because system will reset soon.
    cmp     eax, 5          ; Have we been called to run ClearSecretsFlag?
    je      ResetSystem     ; Jump to ResetSystem, no-need to restore state,
                            ; because system will reset soon.

    ;
    ; Reload the GDTR
    ;
    MOVDOUT3                ; ebx = the address of our local variable
    lgdt    [rax]

    ;
    ; Restore the stack
    ;
    MOVDOUT4
    mov     ebp, eax        ; restore ebp
    MOVDOUT7
    mov     esp, eax        ; restore esp
    MOVDOUT5
    mov     ss, ax          ; restore ss

    ;
    ; Enable protected mode
    ;
    mov     ecx, 0x33
    mov     cr0, rcx

    ;
    ; Enable PAE in CR4
    ;
    mov     ecx, 0x668 | 0x4000 ;CR4_SMXE
    mov     cr4, rcx

    ;
    ; Reload CR3
    ;
    MOVQOUT1
    mov     cr3, rax      ; restore page table from MMX1

    ;
    ; Setup re-enable of paging
    ;

    ;
    ; Set EFER.LME to re-enable ia32-e
    ;
    mov     ecx, 0xc0000080
    xor     edx, edx
    mov     eax, 0x500
    wrmsr

    ;
    ; Re-enable paging
    ;
    mov     rbx, cr0
    bts     ebx, 31
    mov     cr0, rbx

    ;
    ; Now we're in Compatibility mode
    ;
    MOVDOUT3                ; rax = the address of our local variable
    lgdt    [rax]
    lidt    [rax-0x10]

    ;
    ; Reload cs register
    ;
    retf
ReloadCS:
    ;
    ; Now we're in Long Mode
    ;

    ;
    ; Restore the rest of the segment registers
    ;
    pop     rax
    mov     ds, ax
    pop     rax
    mov     es, ax
    pop     rax
    mov     fs, ax
    pop     rax
    mov     gs, ax

    ; restore BSP bit
    mov     rcx, IA32_APIC_BASE
    pop     rdx
    pop     rax
    wrmsr
    ; restore SMI setting
;   mov     dx,0430h
    pop     rax   ;test_test for debug purpose
    pop     rdx   ;test_test for debug purpose
    out     dx, al

    ;
    ; Clear out the cache
    ;
    mov         rax, cr0            ; Set CR0:CD and CR0:NE, clear CR0:NW
    or          rax, 0x40000000 | 0x20 ;$CR0_CD_MASK | $CR0_NE_MASK
    and         rax, ~0x20000000 ; ~CR0_NW_MASK
    mov         cr0, rax
    wbinvd                          ; Flush and invalidate the cache

    ;
    ; Restore def MTRR type
    ;
    mov     rcx, IA32_MTRR_DEF_TYPE
    pop     rdx
    pop     rax
    wrmsr

    ;
    ; Reset Variable MTRRs to Pre-GETSEC state
    ;
    mov     rcx, IA32_MTRR_CAP
    rdmsr
    and     rax, 0xFF
    shl     rax, 1
    mov     rcx, rax
    mov     rbx, rax
    shl     rbx, 4     ; rbx = the total length for MTRR register
    add     rsp, rbx   ; adjust rsp to skip MTRR register
    mov     rbx, rsp
    sub     rbx, 8     ; let rbx point to head of MTRR register now

RestoreNextMtrr:
    add     rcx, IA32_MTRR_PHYSBASE0 - 1
    mov     rdx, [rbx]
    mov     rax, [rbx - 8]
    wrmsr
    sub     rcx, IA32_MTRR_PHYSBASE0 - 1
    sub     rbx, 16
    loop    RestoreNextMtrr

    ;
    ; Restore MISC ENABLE MSR
    ;
    pop     rdx
    pop     rax
    mov     rcx, IA32_MISC_ENABLE
    wrmsr

    ;
    ; Restore control registers
    ;
    pop     rax
    mov     cr0, rax
    pop     rax
    mov     cr4, rax

    wbinvd                          ; Flush and invalidate the cache

    POPA_64
    popfq

    _EMMS
    leave
    ret 0

ResetSystem:
    mov     dx, 0xCF9
    mov     al, 0xA
    out     dx, al
    jmp     .0
.0:
    jmp     .1
.1:
    mov     al, 0xE
    out     dx, al
DeadLoop:
    jmp     $           ; should never get here, but just in case

        ret
;ENDIF  ;LT_FLAG

;text    ENDS

