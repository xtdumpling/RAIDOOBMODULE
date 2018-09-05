;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Copyright (c)  1999 - 2009 Intel Corporation. All rights reserved
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.`
;
; Module Name:
;
;  CpuQuiesceHandler.asm
;
; Abstract:
;
;  This file contains supporting procs for Quiesce
;
;------------------------------------------------------------------------------

;
;BKL_PORTING:
;   The Quiesce code needs to be updated for Platform
;

text  SEGMENT

EXTERN mMirrorEnabled:BYTE

QUIESCE_CACHE_CODE_SIZE     EQU   010000h ; 64K

MKF_I_AM_MONARCH	        EQU      0ffh
MKF_I_AM_MONARCH_AP             EQU      0feh
	
MONARCH_STACK_OFFSET        EQU   030000h
MONARCH_AP_STACK_OFFSET     EQU   (MONARCH_STACK_OFFSET - 0400h) ; 1K apart
QUIESCE_CACHE_DATA_SIZE     EQU   010000h ; 64K

QUIESCE_CTL1_MSR            EQU   050h
B_QUIESCE_CTL1_QUIESCE      EQU   01h   ; bit0
B_QUIESCE_CTL1_UNQUIESCE    EQU   02h   ; bit1
B_QUIESCE_CTL1_UNCORE_FENCE EQU   04    ; bit2  

MSR_MISC_FEATURE_CONTROL    EQU   01A4h

;------------------------------------------------------------------------------
; VOID
; SaveMisc_Feature_Msr (
;   rcx //pointer to memory where Msr value being saved
;   );
;------------------------------------------------------------------------------
Save_Misc_Feature_Msr PROC

 push rbx

 mov rbx, rcx       ;svae pointer
 mov ecx, MSR_MISC_FEATURE_CONTROL
 rdmsr

 and rax, 0FFFFFFFFh
 shl rdx, 32 
 or  rax, rdx
 
 mov qword ptr [ebx], rax
 mfence

 pop rbx
    ret

Save_Misc_Feature_Msr ENDP

;return UINT64
GetTimerTick PROC
    rdtsc
    shl     rdx, 20h
    or      rax, rdx
    ret

GetTimerTick ENDP

;return EFI_STATUS
QuiesceAcquireLock   PROC  PUBLIC

  mov         al, 0FFh
;;;TryAgain:
  xchg        al, byte ptr [rcx]
  cmp         al, 0
  jz          GetLock

;;  pause
      
;;  jmp         TryAgain       

GetLock:
        ret
        
QuiesceAcquireLock   ENDP

QuiesceReleaseLock   PROC  PUBLIC

  mov         al, 0
  xchg        al, byte ptr [rcx]
        
  ret
        
QuiesceReleaseLock   ENDP


;------------------------------------------------------------------------------
;EFI_STATUS
;DoQuiesce (
;  IN UINT64  CacheBase,                //rcx
;  IN UINT64 MMCfgBase,             //rdx
;  IN UINT64 MiscFeatureControlValue, //r8
;  IN BOOLEAN  SMTFlag              //r9
;  );
; r13b: Mirror Enable flag used in Quiesce cache code
;------------------------------------------------------------------------------
DoQuiesce PROC  

  push rbp
  push rbx
  push rsp
  push r13

  mov r13b, mMirrorEnabled ; get mirror enable flag
  mov rbp, rcx          ; save quiesce base

  ; save old cr3 
  mov rax, cr3    
  push rax                ; save old cr3 in old stack
  ; now setup Stack from Quiesce 
  mov rax, rsp
  add rcx, MONARCH_STACK_OFFSET
  mov rsp, rcx      ; set stack on quiesce
  push rax          ; save old stack pointer

  mov  r10, MKF_I_AM_MONARCH
  call rbp


  mov rbx, rax      ; save return Status in rbx

  ; restore old statck
  pop rax           ; 
  mov rsp, rax      ; restore old rsp back

  pop rax           ; get old cr3 back
  mov cr3, rax

  mov rax, rbx      ; return Status in rax

  pop r13
  pop rsp
  pop rbx
  pop rbp

  ret

DoQuiesce ENDP 


;;------------------------------------------------------------------------------
;EFI_STATUS
;Monarch_Ap_DoQuiesce( 
;  IN UINT64 CacheBase,               //rcx
;  IN EFI_PHYSICAL_ADDRESS MMCFGBase, //rdx
;  IN UINT64 MiscFeatureControlValue, //r8
;  BOOLEAN   SMTFlag                  //r9 always 1
;  );
;------------------------------------------------------------------------------
Monarch_Ap_DoQuiesce PROC 

  push rbp
  push rbx
  push rsp

  mov rbp, rcx          ; save quiesce base 

  ; save old cr3 
  mov rax, cr3    
  push rax                ; save old cr3 in old stack

  ; now save old rsp and setp up Monarch AP stack for Quiesce
  add rcx, MONARCH_AP_STACK_OFFSET  ;point to (last address - 1K) of cache
  mov rbx, rsp    
  mov rsp, rcx      ; update rsp to pointer new stack
  push rbx          ; save old stack pointer in cache data stack

  mov  r10, MKF_I_AM_MONARCH_AP
  call rbp          ; run out of Cache

  mov rbx, rax      ; save return Status
  ; restore old statck
  pop rax           ; 
  mov rsp, rax      ; restore old rsp back

  pop rax           ; get old cr3
  mov cr3, rax

  mov rax, rbx      ; restore return Status

  pop rsp
  pop rbx
  pop rbp

  ret

Monarch_Ap_DoQuiesce ENDP 


text  ENDS

END


