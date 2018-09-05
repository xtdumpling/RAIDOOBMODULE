;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 2007 - 2014 Intel Corporation. All rights reserved
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
;   LaunchBiosAcm.asm
;
; Abstract:
;
;   Assembly code of LaunchBiosAcm
;
;------------------------------------------------------------------------------

    .XLIST
    INCLUDE txt.inc
    INCLUDE stackless32.inc
    .LIST

    .686P
    .MODEL FLAT,C
    .CODE
    .MMX
    .XMM

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
LaunchBiosAcm   PROC   PUBLIC AcmBase:DWORD, EsiValue:DWORD

    LOCAL   SavedGdtr[8]:BYTE
    LOCAL   SavedIdtr[8]:BYTE

    ACM_SIZE_TO_CACHE  TEXTEQU  <xmm0>
    ACM_BASE_TO_CACHE  TEXTEQU  <xmm1>
    NEXT_MTRR_INDEX    TEXTEQU  <xmm2>
    NEXT_MTRR_SIZE     TEXTEQU  <xmm3>

    pushfd
    pushad

    ; check if the AcmBase is a good address (none zero)
    cmp     AcmBase, 0      ; bad ACM address?
    je      SkipLaunch      ; Yes, then exit without any farther work

    ;
    ; Check to see if we need to save state for return to BIOS after GETSEC
    ;
    mov     ebx, EsiValue   ; ebx = GETSEC leaf number
    or      ebx, ebx        ; have we been called to run SCLEAN?
    jz      BeginLaunch     ; if so, we don't need to save state.

    cmp     ebx, 5          ; have we been called to run ClearSecretsFlag?
    je      BeginLaunch     ; if so, we don't need to save state.

    sgdt    SavedGdtr       ; save value of gdtr in local variable
    sidt    SavedIdtr       ; save value of idtr in local variable
    lea     ebx, SavedGdtr  ; NOTE: This assumes segs will always be zero-based
                            ; NOTE: This assumption is valid per PEI CIS
    MOVDIN3                 ; mm3 = address of our local variable
    mov     ebx, ebp
    MOVDIN4                 ; mm4 = ebp
    xor     ebx, ebx
    mov     bx, ss
    MOVDIN5                 ; mm5 = ss

    ;
    ; Save control registers
    ;
    mov     ebx, cr4
    push    ebx
    mov     ebx, cr0
    push    ebx

    ;
    ; Save MISC ENABLE MSR
    ;
    mov     ecx, IA32_MISC_ENABLE
    rdmsr
    push    eax
    push    edx

    ;
    ; Save MTRR
    ;
    mov     ecx, IA32_MTRR_CAP
    rdmsr
    and     eax, 0FFh
    shl     eax, 1
    mov     ecx, eax

SaveNextMtrr:
    add     ecx, IA32_MTRR_PHYSBASE0 - 1
    rdmsr
    push    edx
    push    eax
    sub     ecx, IA32_MTRR_PHYSBASE0 - 1
    loop    SaveNextMtrr

    ; Save def MTRR type
    mov     ecx, IA32_MTRR_DEF_TYPE
    rdmsr
    push    eax
    push    edx

    ; Save the rest of the segment registers
    xor     ebx, ebx
    mov     bx, gs
    push    ebx
    mov     bx, fs
    push    ebx
    mov     bx, es
    push    ebx
    mov     bx, ds
    push    ebx

    ;
    ; For reloading CS
    ;
    mov     bx, cs
    push    ebx
    mov     ebx, OFFSET ReloadCS
    push    ebx

BeginLaunch:
    ;
    ; Now that all of our state is on the stack, save esp
    ;
    mov     ebx, esp
    MOVDIN7                 ; mm7 = esp

    ;--------------------------------
    ; Begin to launch ACM
    ;--------------------------------

    ;
    ; Save the parameters passed to us
    ;
    mov     ebx, AcmBase    ; save address of BIOS ACM in MMX0 
    MOVDIN0
    mov     ebx, EsiValue   ; save value of ESI for GETSEC[ENTERACCS] in MMX2 
    MOVDIN2

    ;
    ; Enable SMXE, SSE and debug extensions 
    ;
    mov         eax, cr4
    or          eax, CR4_OSFXSR_MASK + CR4_DE_MASK + CR4_SMXE
    mov         cr4, eax

    ;
    ; Disable cache
    ;
    mov         eax, cr0    ; set CR0:CD and CR0:NE, clear CR0:NW
    or          eax, CR0_CD_MASK OR CR0_NE_MASK
    and         eax, NOT CR0_NW_MASK
    mov         cr0, eax

    ;If LockConfig func 06, we jump pass this invd 
    MOVDOUT2
    cmp     ebx, 06h
    jz      OverScleanInvd
    ;
    ; Check to see how we should invalidate the cache
    ;
    MOVDOUT2
    or      ebx, ebx        ; have we been called to run SCLEAN?
    ; bugbug, current MCU 20A still need skip wbinvd for LT-SX
    ;jz      ScleanInvd
    jmp      ScleanInvd
    wbinvd                  ; Nope, SCHECK.  Writeback is necessary
    jmp     OverScleanInvd

ScleanInvd:
    invd                    ; Yep, SCLEAN. Invalidate the cache

OverScleanInvd:
    ;
    ; Disable MTRRs, set Default Type to UC
    ;
    mov     ecx, IA32_MTRR_DEF_TYPE
    xor     eax, eax
    xor     edx, edx
    wrmsr

    ;
    ; Clear all of the Variable MTRRs
    ;
    mov     ecx, IA32_MTRR_CAP
    rdmsr
    and     eax, 0FFh
    shl     eax, 1
    mov     ecx, eax
    xor     eax, eax
    xor     edx, edx
ClearMtrrContinue:        
    add     ecx, IA32_MTRR_PHYSBASE0 - 1
    wrmsr
    sub     ecx, IA32_MTRR_PHYSBASE0 - 1
    loop    ClearMtrrContinue

    ;
    ; Determine size of AC module
    ;
    MOVDOUT0                            ; ebx = AcmBase
    mov         esi, ebx                ; esi = AcmBase
    mov         eax, [esi+ACMOD_SIZE]   ; ebx = size of ACM
    shl         eax, 2                  ;  ...in bytes (ACM header has size in dwords)

    ;
    ; Round up to page size
    ;
    mov         ecx, eax                        ; Save
    and         ecx, 0FFFFF000h                 ; Number of pages in AC module
    and         eax, 0FFFh                      ; Number of "less-than-page" bytes
    jz          rounded
    mov         eax, 1000h                      ; Add the whole page size

rounded:           
    add         eax, ecx                        ; eax = rounded up AC module size

    ;
    ; Initialize "locals"
    ;
    sub     ecx, ecx
    movd    NEXT_MTRR_INDEX, ecx    ; Start from MTRR0

    ;
    ; Save remaining size to cache
    ;
    movd    ACM_SIZE_TO_CACHE, eax  ; Size of ACM that must be cached
    movd    ACM_BASE_TO_CACHE, esi  ; Base ACM address

nextMtrr:
    ;
    ; Get remaining size to cache
    ;
    movd    eax, ACM_SIZE_TO_CACHE
    and     eax, eax
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
    movd    esi, ACM_BASE_TO_CACHE
    bsf     ecx, esi                ; Get index of lowest bit set in base address
    ;
    ; Convert index into size to be cached by next MTRR
    ;
    mov     edx, 1h
    shl     edx, cl                 ; Alignment is in edx
    cmp     edx, eax                ; What is bigger, alignment or remaining size?
    jbe     gotSize                 ; JIf aligment is less
    ;
    ; Remaining size is bigger. Get the biggest part of it, 2^N in size
    ;
    bsr     ecx, eax                ; Get index of highest set bit
    ;
    ; Convert index into size to be cached by next MTRR
    ;
    mov     edx, 1
    shl     edx, cl                 ; Size to cache
        
gotSize:
    mov     eax, edx
    movd    NEXT_MTRR_SIZE, eax     ; Save

    ;
    ; Compute MTRR mask value:  Mask = NOT (Size - 1)
    ;                                                     
    dec     eax                     ; eax - address of last byte in rounded AC module size
    not     eax                     ; eax contains low 32 bits of mask
    or      eax, MTRR_VALID         ; Set valid bit
    ;
    ; Program mask register
    ;
    mov     ecx, IA32_MTRR_PHYSMASK0 ; setup variable mtrr 
    movd    ebx, NEXT_MTRR_INDEX
    add     ecx, ebx

    mov     edx, 0Fh        ; 8K range (FFFFFFE800)
    wrmsr
    ;
    ; Program base register
    ;
    xor     edx, edx
    mov     ecx, IA32_MTRR_PHYSBASE0 ; setup variable mtrr 
    add     ecx, ebx                 ; ebx is still NEXT_MTRR_INDEX

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
    mov     ecx, IA32_MTRR_DEF_TYPE
    xor     edx, edx
    mov     eax, MTRR_ENABLE
    wrmsr
 
    ;
    ; Re-enable cache
    ;
    mov     eax, cr0
    and     eax, NOT CR0_CD_MASK
    mov     cr0, eax

    ;
    ; Clean all MCi_STATUS MSR registers
    ; SCLEAN will generate GPF otherwise
    ;
    mov     ecx, MCG_CAP
    rdmsr
    movzx   ebx, al                 ; ebx = MCR bank count 
    sub     eax, eax                ; Write 0 into all MCi_STATUS registers
    sub     edx, edx                                         
    mov     ecx, MC0_STATUS

McaErrorCleanLoopStart: 
    wrmsr
    dec     ebx
    jz      CallGetsec
    add     ecx, 4                  ; ecx = number of MSRs per bank
    jmp     McaErrorCleanLoopStart

    ;
    ; Call GETSEC[ENTERACCS]
    ;
CallGetsec:
    mov     eax, ENTERACCS          ; eax = ENTERACCS
    MOVDOUT2                        ; ebx = ACM function
    mov     esi, ebx                ; esi = ACM function
    MOVDOUT0                        ; ebx = AcmBase
    mov     ecx, [ebx+ACMOD_SIZE]   ; ecx = size of ACM in dwords
    shl     ecx, 2                  ; ecx = size of ACM in bytes
    xor     edx, edx
    xor     edi, edi
    _GETSEC

    ;
    ; Check if we need to restore BIOS state and configuration
    ;
    MOVDOUT2
    or      ebx, ebx        ; Have we been called to run SCLEAN?
    jz      ResetSystem     ; Jump to ResetSystem, no-need to restore state,
                            ; because system will reset soon.
    cmp     ebx, 5          ; Have we been called to run ClearSecretsFlag?
    je      ResetSystem     ; Jump to ResetSystem, no-need to restore state,
                            ; because system will reset soon.

    MOVDOUT3                ; ebx = the address of our local variable
    lgdt    FWORD PTR [ebx]
    lidt    FWORD PTR [ebx-8]

    MOVDOUT4
    mov     ebp, ebx        ; restore ebp
    MOVDOUT7
    mov     esp, ebx        ; restore esp
    MOVDOUT5
    mov     ss, bx          ; restore ss

    ;
    ; Reload cs register 
    ;
    retf
ReloadCS:

    ;
    ; Restore the rest of the segment registers
    ;
    pop     ebx
    mov     ds, bx
    pop     ebx
    mov     es, bx
    pop     ebx
    mov     fs, bx
    pop     ebx
    mov     gs, bx

    ;
    ; Clear out the cache 
    ;
    mov         eax, cr0            ; Set CR0:CD and CR0:NE, clear CR0:NW
    or          eax, CR0_CD_MASK OR CR0_NE_MASK
    and         eax, NOT CR0_NW_MASK
    mov         cr0, eax
    wbinvd                          ; Flush and invalidate the cache

    ;
    ; Restore def MTRR type
    ;
    mov     ecx, IA32_MTRR_DEF_TYPE
    pop     edx
    pop     eax
    wrmsr

    ;
    ; Reset Variable MTRRs to Pre-GETSEC state
    ;
    mov     ecx, IA32_MTRR_CAP
    rdmsr
    and     eax, 0FFh
    shl     eax, 1
    mov     ecx, eax
    mov     ebx, eax
    shl     ebx, 3     ; ebx = the total length for MTRR register
    add     esp, ebx   ; adjust esp to skip MTRR register
    mov     ebx, esp
    sub     ebx, 4     ; let ebx point to head of MTRR register now

RestoreNextMtrr:
    add     ecx, IA32_MTRR_PHYSBASE0 - 1
    mov     edx, [ebx]
    mov     eax, [ebx - 4]
    wrmsr
    sub     ecx, IA32_MTRR_PHYSBASE0 - 1
    sub     ebx, 8
    loop    RestoreNextMtrr

    ;
    ; Restore MISC ENABLE MSR
    ;
    pop     edx
    pop     eax
    mov     ecx, IA32_MISC_ENABLE
    wrmsr

    ;
    ; Restore control registers
    ;
    pop     ebx
    mov     cr0, ebx
    pop     ebx
    mov     cr4, ebx

SkipLaunch:
    popad
    popfd
    _EMMS
    ret

ResetSystem:
    mov     dx, 0CF9h
    mov     al, 0Ah
    out     dx, al
    jmp     @f
@@:
    jmp     @f
@@:
IFDEF POWERON
    mov     al, 03h
ELSE
    mov     al, 06h
ENDIF
    out     dx, al
DeadLoop:
    jmp     $           ; should never get here, but just in case

LaunchBiosAcm   ENDP

END
