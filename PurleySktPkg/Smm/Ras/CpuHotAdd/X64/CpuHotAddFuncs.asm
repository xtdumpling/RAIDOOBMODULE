;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;-------------------------------------------------------------------------------
;
; Copyright (c) 2011 - 2015 Intel Corporation. All rights reserved
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
;   CpuHotAddFuncs.asm
; 
; Abstract:
; 
;   This is the assembly code for EM64T MP support
;
;-------------------------------------------------------------------------------

VacantFlag             Equ   00h
NotVacantFlag          Equ   0ffh


;text      SEGMENT
.code


AsmRsm             PROC    PUBLIC
        rsm
        ret
AsmRsm             ENDP

AsmAcquireMPLock   PROC  PUBLIC

        mov         al, NotVacantFlag
TryGetLock:
        xchg        al, byte ptr [rcx]
        cmp         al, VacantFlag
        jz          LockObtained

        pause
        jmp         TryGetLock       

LockObtained:
        ret
        
AsmAcquireMPLock   ENDP

AsmReleaseMPLock   PROC  PUBLIC

        mov         al, VacantFlag
        xchg        al, byte ptr [rcx]
        
        ret
  
AsmReleaseMPLock   ENDP

;IFDEF LT_FLAG
MOVQIN0                         EQU db 48h, 0fh, 06eh, 0c0h ; movq mm0, rax
MOVQOUT0                        EQU db 48h, 0fh, 07eh, 0c0h ; movq rax, mm0
MOVDOUT0                        EQU db 0fh, 07eh, 0c0h ; movd eax, mm0

MOVQIN1                         EQU db 48h, 0fh, 06eh, 0c8h ; movq mm1, rax
MOVQOUT1                        EQU db 48h, 0fh, 07eh, 0c8h ; movq rax, mm1
MOVDOUT1                        EQU db 0fh, 07eh, 0c8h ; movd eax, mm1

MOVQIN2                         EQU db 48h, 0fh, 06eh, 0d0h ; movq mm2, rax
MOVQOUT2                        EQU db 48h, 0fh, 07eh, 0d0h ; movq rax, mm2
MOVDOUT2                        EQU db 0fh, 07eh, 0d0h ; movd eax, mm2

MOVQIN3                         EQU db 48h, 0fh, 06eh, 0d8h ; movq mm3, rax
MOVQOUT3                        EQU db 48h, 0fh, 07eh, 0d8h ; movq rax, mm3
MOVDOUT3                        EQU db 0fh, 07eh, 0d8h ; movd eax, mm3

MOVQIN4                         EQU db 48h, 0fh, 06eh, 0e0h ; movq mm4, rax
MOVQOUT4                        EQU db 48h, 0fh, 07eh, 0e0h ; movq rax, mm4
MOVDOUT4                        EQU db 0fh, 07eh, 0e0h ; movd eax, mm4

MOVQIN5                         EQU db 48h, 0fh, 06eh, 0e8h ; movq mm5, rax
MOVQOUT5                        EQU db 48h, 0fh, 07eh, 0e8h ; movq rax, mm5
MOVDOUT5                        EQU db 0fh, 07eh, 0e8h ; movd eax, mm5

MOVQIN6                         EQU db 48h, 0fh, 06eh, 0f0h ; movq mm6, rax
MOVQOUT6                        EQU db 48h, 0fh, 07eh, 0f0h ; movq rax, mm6
MOVDOUT6                        EQU db 0fh, 07eh, 0f0h ; movd eax, mm6

MOVQIN7                         EQU db 48h, 0fh, 06eh, 0f8h ; movq mm7, rax
MOVQOUT7                        EQU db 48h, 0fh, 07eh, 0f8h ; movq rax, mm7
MOVDOUT7                        EQU db 0fh, 07eh, 0f8h ; movd eax, mm7

_EMMS                           EQU db 0fh, 77h

;-----------------------------------------------------------------------------
;  Macro: PUSHA_64
; 
;  Description: Saves all registers on stack
;  
;  Input:   None
; 
;  Output:  None
;-----------------------------------------------------------------------------
PUSHA_64   macro
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
endm

;-----------------------------------------------------------------------------
;  Macro: POPA_64
; 
;  Description: Restores all registers from stack
;  
;  Input:   None
; 
;  Output:  None
;-----------------------------------------------------------------------------
POPA_64   macro
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
endm

;
; MSRs
;
IA32_CR_FEATURE_CONTROL            EQU 03Ah
IA32_MISC_ENABLE                   EQU 1A0h
LT_OPT_IN_MSR_VALUE                EQU 01111111100000011b ; 0FF03h
LT_OPT_IN_VMX_ONLY_MSR_VALUE       EQU 00000000000000101b ; 00005h

ACMOD_SIZE                         EQU 24

CR0_NE_MASK                        EQU (1 SHL 5)
CR0_NW_MASK                        EQU (1 SHL 29)
CR0_CD_MASK                        EQU (1 SHL 30)
CR0_PG_MASK                        EQU (1 SHL 31)
CR4_DE_MASK                        EQU (1 SHL 3)
CR4_OSFXSR_MASK                    EQU (1 SHL 9)

;-----------------------------------------------------------------------------
; MTRRs
;
IA32_MTRR_PHYSBASE0                EQU 200H 
IA32_MTRR_PHYSMASK0                EQU 201H 
IA32_MTRR_PHYSBASE1                EQU 202H 
IA32_MTRR_PHYSMASK1                EQU 203H 
IA32_MTRR_PHYSBASE2                EQU 204H 
IA32_MTRR_PHYSMASK2                EQU 205H 
IA32_MTRR_PHYSBASE3                EQU 206H 
IA32_MTRR_PHYSMASK3                EQU 207H 
IA32_MTRR_PHYSBASE4                EQU 208H 
IA32_MTRR_PHYSMASK4                EQU 209H 
IA32_MTRR_PHYSBASE5                EQU 20AH 
IA32_MTRR_PHYSMASK5                EQU 20BH 
IA32_MTRR_PHYSBASE6                EQU 20CH 
IA32_MTRR_PHYSMASK6                EQU 20DH 
IA32_MTRR_PHYSBASE7                EQU 20EH 
IA32_MTRR_PHYSMASK7                EQU 20FH 
IA32_MTRR_FIX64K_00000             EQU 250H 
IA32_MTRR_FIX16K_80000             EQU 258H 
IA32_MTRR_FIX16K_A0000             EQU 259H 
IA32_MTRR_FIX4K_C0000              EQU 268H 
IA32_MTRR_FIX4K_C8000              EQU 269H 
IA32_MTRR_FIX4K_D0000              EQU 26AH 
IA32_MTRR_FIX4K_D8000              EQU 26BH 
IA32_MTRR_FIX4K_E0000              EQU 26CH 
IA32_MTRR_FIX4K_E8000              EQU 26DH 
IA32_MTRR_FIX4K_F0000              EQU 26EH 
IA32_MTRR_FIX4K_F8000              EQU 26FH 
IA32_CR_PAT                        EQU 277H 
IA32_MTRR_DEF_TYPE                 EQU 2FFH 
IA32_MTRR_CAP                      EQU 0FEH
  IA32_MTRR_SMRR_SUPPORT_BIT       EQU (1 SHL 11)
IA32_FEATURE_CONTROL               EQU 03AH
  IA32_SMRR_ENABLE_BIT             EQU (1 SHL 3)

;
; Only low order bits are assumed
;
MTRR_MASK                          EQU 0FFFFF000h

MTRR_ENABLE                        EQU (1 SHL 11)
MTRR_FIXED_ENABLE                  EQU (1 SHL 10)

MTRR_VALID                         EQU (1 SHL 11)
UC                                 EQU 00h
WB                                 EQU 06h

MTRR_VCNT                          EQU 8

MTRR_MASK_4K_LOW                   EQU 0fffff800h
MTRR_MASK_4K_HIGH                  EQU 0fh
UINT32_4K_MASK                     EQU 0fffff000h

ACPIBASE                           EQU  08000FA40h  ;D31:F2:R40h
ACPIBASE_MASK                      EQU  0FF00h
SMI_EN_OFFSET                      EQU  030h

;----------------------------------------------------------------------------
; APIC definitions
;
IA32_APIC_BASE                     EQU 001Bh  ; APIC base MSR
IA32_APIC_BASE_G_XAPIC             EQU BIT11
IA32_APIC_BASE_M_XAPIC             EQU BIT10
IA32_APIC_BASE_BSP                 EQU 0100h
BASE_ADDR_MASK                     EQU 0FFFFF000h
APIC_ID                            EQU 20h
ICR_LOW                            EQU 300h
ICR_HIGH                           EQU 310h
SPURIOUS_VECTOR_1                  EQU 0F0h

;-----------------------------------------------------------------------------
; Features support & enabling
;
CR4_VMXE                           EQU (1 SHL 13)
CR4_SMXE                           EQU (1 SHL 14)
CR4_PAE_MASK                       EQU (1 SHL 5)

CPUID_VMX                          EQU (1 SHL 5)
CPUID_SMX                          EQU (1 SHL 6)

;-----------------------------------------------------------------------------
; Machne check architecture MSR registers
;        
MCG_CAP                            EQU 179h
MCG_STATUS                         EQU 17Ah
MCG_CTL                            EQU 17Bh
MC0_CTL                            EQU 400h
MC0_STATUS                         EQU 401h
MC0_ADDR                           EQU 402h
MC0_MISC                           EQU 403h

_GETSEC                            EQU db 0fh, 37h

ENTERACCS                          EQU 02h
PARAMETERS                         EQU 06h

LaunchBiosAcm      PROC  ;near PUBLIC
;    START_FRAME
;    MAKE_LOCAL BIOS_GDT[2]:QWORD
;    MAKE_LOCAL BIOS_IDT[2]:QWORD
;    END_FRAME

     enter  030h,0

BIOS_GDT  EQU [rbp-10h]
BIOS_IDT  EQU [rbp-20h]

    ACM_SIZE_TO_CACHE  TEXTEQU  <xmm0>
    ACM_BASE_TO_CACHE  TEXTEQU  <xmm1>
    NEXT_MTRR_INDEX    TEXTEQU  <xmm2>
    NEXT_MTRR_SIZE     TEXTEQU  <xmm3>

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
    and     rax, 0FFh
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

    mov     dx, 0cf8h
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
    and     al, 0FEh
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
    mov     rdx, OFFSET ReloadCS   ; Assume it is below 4G
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
    or          rax, CR4_OSFXSR_MASK + CR4_DE_MASK + CR4_SMXE
    mov         cr4, rax

    ;
    ; Disable cache
    ;
    mov         rax, cr0    ; set CR0:CD and CR0:NE, clear CR0:NW
    or          rax, CR0_CD_MASK OR CR0_NE_MASK
    and         rax, NOT CR0_NW_MASK
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
    and     rax, 0FFh
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
    and         rcx, 0FFFFF000h                 ; Number of pages in AC module
    and         rax, 0FFFh                      ; Number of "less-than-page" bytes
    jz          rounded
    mov         rax, 1000h                      ; Add the whole page size

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
    mov     rdx, 1h
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
    or      rax, MTRR_VALID         ; Set valid bit
    ;
    ; Program mask register
    ;
    mov     rcx, IA32_MTRR_PHYSMASK0 ; setup variable mtrr 
    xor     rbx, rbx
    movd    ebx, NEXT_MTRR_INDEX
    add     rcx, rbx

    mov     rdx, 0Fh        ; 8K range (FFFFFFE800)
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
    mov     rax, MTRR_ENABLE
    wrmsr
 
    ;
    ; Re-enable cache
    ;
    mov     rax, cr0
    and     rax, NOT CR0_CD_MASK
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
    mov     rcx, 20h               ; Hardcode Compatible mode segment
    shl     rcx, 32
    mov     rdx, OFFSET Compatible ; assume address < 4G
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
    mov     ecx, 0c0000080h ; EFER MSR number.
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
    mov     ecx, DWORD PTR [rbx+ACMOD_SIZE]   ; ecx = size of ACM in dwords
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
    lgdt    fword ptr [rax]

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
    mov     ecx, 33h
    mov     cr0, rcx

    ;
    ; Enable PAE in CR4
    ;
    mov     ecx, 668h OR CR4_SMXE
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
    mov     ecx, 0c0000080h
    xor     edx, edx
    mov     eax, 500h
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
    lgdt    fword ptr [rax]
    lidt    fword ptr [rax-10h]

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
    or          rax, CR0_CD_MASK OR CR0_NE_MASK
    and         rax, NOT CR0_NW_MASK
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
    and     rax, 0FFh
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
    mov     dx, 0CF9h
    mov     al, 0Ah
    out     dx, al
    jmp     @f
@@:
    jmp     @f
@@:
    mov     al, 0Eh
    out     dx, al
DeadLoop:
    jmp     $           ; should never get here, but just in case
        
        ret
LaunchBiosAcm      ENDP
;ENDIF  ;LT_FLAG

;text    ENDS

END
