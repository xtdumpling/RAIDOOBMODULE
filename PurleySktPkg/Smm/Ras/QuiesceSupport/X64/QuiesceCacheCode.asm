;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Copyright (c)  1999 - 2010 Intel Corporation. All rights reserved
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
;  QuiesceCacheCode.asm
;
; Abstract:
;
;  This is the code that run out of cache while program CSRs during Quiesce
;
;------------------------------------------------------------------------------

;
;BKL_PORTING:
;   The Quiesce code needs to be updated for Platform
;

text  SEGMENT

EXTERNDEF   QuiesceCacheCodeTemplate:BYTE
EXTERNDEF   QuiesceCacheCodeSize:DWORD 

 


QUIESCE_CACHE_CODE_SIZE     EQU   010000h ; 64K

MKF_I_AM_MONARCH	          EQU   0ffh
MKF_I_AM_MONARCH_AP         EQU   0feh

THREAD_QUIESCE_STAKC_SIZE   EQU   0400h                          ; 1k 
MONARCH_STACK_OFFSET        EQU   030000h
MONARCH_AP_STACK_OFFSET     EQU   (MONARCH_STACK_OFFSET - THREAD_QUIESCE_STAKC_SIZE) ; 1K apart
QUIESCE_CACHE_DATA_SIZE     EQU   010000h ; 64K

QUIESCE_STATUS_LOCATION     EQU   (MONARCH_AP_STACK_OFFSET - THREAD_QUIESCE_STAKC_SIZE) ;QuiesceStatusLocation

QUIESCE_CTL1_MSR            EQU   050h
B_QUIESCE_CTL1_QUIESCE      EQU   01h   ; bit0
B_QUIESCE_CTL1_UNQUIESCE    EQU   02h   ; bit1
B_QUIESCE_CTL1_EVIC_PENDING EQU   04    ; bit2  
B_QUIESCE_CTL1_QUIESCE_CAP  EQU   080h  ; bit7

MSR_MISC_FEATURE_CONTROL    EQU   01A4h

B_RECONFIG_DONE             EQU   000000002h  ; bit1
B_MONARCH_AP_READY          EQU   000000010h  ; bit4
B_MONARCH_AP_DONE           EQU   000000020h  ; bit5


;------------------------------------------------------------------------------
; UINTN
; ReadCr3 (
;   VOID
;   );
;------------------------------------------------------------------------------
ReadCr3  PROC
    mov     rax, cr3
    ret
ReadCr3  ENDP




ALIGN   16
; note: Put all code that will be in cache betweeen  
;       QuiesceCacheCodeTemplate label and QuiesceCodeInCacheSize
QuiesceCacheCodeTemplate   LABEL   BYTE



; this proc do nothing but CSR program
; The CSRs need to be programmed are all inside Quiesce Table
; build after which RAS event decided
; The Quiesce Table Data Entry contains following format:
;typedef struct {
;  UINT8                 Operation;
;  UINT8                 AccessWidth;
;  UINT64                CsrAddress;
;  QUIESCE_DATA          CsrData;
;} QUIESCE_DATA;
;
; input 
;       rdx: MMCfg Base
;       r8:  MiscFeatureControlValue
;       r9[7:0]:  SMTFlag
;       r10: Monarch flag
;       rbp: cache base
;       r13b: MirrorEnable flag
;
;       Reg usage:
;         r8: Misc Val
;         r9b: SMTFlag
;         r10: Monarch Flag
;         r11: MMCfg base
;

QuiesceRunOutOfCache PROC 

  push rbx
  push rsi
  push rdi
  push rbp
  push r12

  cmp r10, MKF_I_AM_MONARCH
  jne SetMsr1A4

  ; I am Monarch
  mov r11, rdx              ; save MMCfg in r11

  ; Quiesce system
  mov ecx, QUIESCE_CTL1_MSR
  mov eax, 1
  xor edx, edx
  wrmsr 
  nop
  nop

SetMsr1A4:  
  mov ecx,  MSR_MISC_FEATURE_CONTROL  ; msr(0x1A4)
  mov  eax, 0Fh                       ; 1A4 is core scope and has saved, no need to save again
  xor  edx, edx                   
  wrmsr

updatePageTable:
  ; update cr3 to bring in new page table
  mov rax, rbp  ;get cache base
  add rax, (QUIESCE_CACHE_CODE_SIZE + 1000h) ; 4K apart
  mov cr3, rax                        ; flush out TLB

  cmp r10, MKF_I_AM_MONARCH
  jne do_monarch_ap_loop  ; monarch AP don't need to bring code into MLC, Monarch will do

  ; read Quiesce Code area
  mov rsi, rbp                  ; get cache base 
;;;;;;  mov rcx, QUIESCE_CACHE_CODE_SIZE / 64  ;;;load 64K code
  mov rcx, QUIESCE_CACHE_CODE_SIZE / 512 ;;;;load 8K code
 
ReadQuiesceCode:
  mov  eax, [esi]
  add  esi, 64
  loopd  ReadQuiesceCode

  mov rsi, rbp                  ; get cache base 
  add rsi, 011000h              ;
  mov rcx, (QUIESCE_CACHE_CODE_SIZE - 09000h) / 64 ; page table max 28K
ReadPageTable:
  mov  eax, [esi]
  add  esi, 64
  loop  ReadPageTable

  mov rsi, rbp              ; get cache base
  add rsi, 20000h           ; data base is 128K away from cache base
  mov rdi, rsi                    
; R/W data area!!!!!!!
  mov rcx, QUIESCE_CACHE_DATA_SIZE / 64 

CopyQuiesceData:  
  mov  eax, [esi]
  mov  [edi], eax
  add  esi, 64
  add  edi, 64
  loopd  CopyQuiesceData

  ; now poll evict penging 
poll_evict_penging:
  mov ecx, QUIESCE_CTL1_MSR
  rdmsr
  and eax, B_QUIESCE_CTL1_EVIC_PENDING
  jnz poll_evict_penging  

  mov rsi, rbp                              ; get cache base back
  add rsi, 2 * QUIESCE_CACHE_CODE_SIZE      ; rsi point to Quiesce data 

  ; Reconfig system, based on Quiesce Table Data 
do_sys_reConfig:
  mov ah, (QuiesceDataStruc ptr [rsi]).AccessWidth   ; get AccessWidth
  mov al, (QuiesceDataStruc ptr [rsi]).Operation     ; get operation
  mov rdi, (QuiesceDataStruc ptr [rsi]).CsrAddress   ; get CsrAddress
  
  ; ceck operation
  cmp al, 1             ; Write operation?
  je  do_Csrwrite       ; br, if yes
  cmp al, 2             ; Poll Operation?
  je  do_Csrpoll        ; br, if yes


  cmp al, -1            ; OperationEnd?
  jne errorAndReturn
  xor ebx, ebx          ; everything ok, so clear error flag
  jmp sys_reConfig_done ; must be OperationEnd


do_Csrpoll:
  mov ecx, (QuiesceDataStruc ptr [rsi]).CsrData.AndMask       ; read AndMask
  mov edx, (QuiesceDataStruc ptr [rsi]).CsrData.DataMask.Data ; read Data

  cmp ah, 1
  je  poll_byte
  cmp ah, 2
  je  poll_word

poll_dword:
  mov ebx, dword ptr[rdi]     ; read *Address
  and ebx, ecx                ; AND with AndMask
  cmp ebx, edx                ; check if match DataMask
  jne poll_dword
  jmp next_data

poll_word:
  mov bx, word ptr[rdi]       ; read *Address
  and bx, cx
  cmp bx, dx
  jne poll_word
  jmp next_data

poll_byte:
  mov bl, byte ptr[rdi]       ; read *Address
  and bl, cl
  cmp bl, dl
  jne poll_byte
  jmp next_data

do_Csrwrite:
  xor al, al              ; don't need al any more, so use it for modify or direct write

  mov ecx, (QuiesceDataStruc ptr [rsi]).CsrData.AndMask         ; read AndMask
  mov edx, (QuiesceDataStruc ptr [rsi]).CsrData.DataMask.OrMask ; read OrMask

  ; check CsrData Mask
  cmp ecx, 0  ;;dword ptr[esi+12], 0  ; check AND mask
  jne read_modify                                       ; br, if not 0
  jmp check_width 
  
read_modify:
  mov al, 01h                 ; set read modify flag

check_width:
  cmp ah, 1
  je  byte_access
  cmp ah, 2 
  je  word_access

  ; must be dword access
  cmp al, 1                   ; read modify?
  jne write_dword             ; br, if no
  mov ebx, dword ptr[rdi]     ; read *Address
  and ebx, ecx                ; *Address & AndMask
  or  edx, ebx                ; OR with OrMask
write_dword:
  mov dword ptr [rdi], edx    ; write result to *Address
  jmp next_data

word_access:
  cmp dl, 1                   ; read modify?
  jne write_word
  mov bx, word ptr[rdi]       ; read *Address
  and bx, cx                  ; *Address AND with AndMask
  or  dx,  bx                 ; OR with OrMask
write_word:
  mov word ptr [rdi], dx      ; write result to *Address
  jmp next_data

byte_access:
  cmp dl, 1                   ; read modify?
  jne write_byte
  mov bl, byte ptr[rdi]       ; read *Address
  and bl, cl                  ; *Address & AndMask
  or  dl, bl                  ; OR with OrMask
write_byte:
  or  bl, cl                ; OR with OrMask
  mov byte ptr [rdi], dl      ; write result to *Address

next_data:
  add esi, sizeof(QuiesceDataStruc)    ; QuiesceDataEntrySize   ; add sizeof (QUIESCE_TABLE_DATA)
  jmp do_sys_reConfig
  
errorAndReturn:
  mov rbx, 08000000000000015h          ; set error = EFI_ABORTED

sys_reConfig_done:

unquiesce:
  ; Monarch set QUIESCE_CTR.UnQuiesce it
  mov ecx, QUIESCE_CTL1_MSR
  mov eax, B_QUIESCE_CTL1_UNQUIESCE
  xor edx, edx
  wrmsr

  jmp restore_prefetch_control

do_monarch_ap_loop:
  ; infor Monarch, AP is ready for reconfigure
  mov esi, ebp              ; code base
  add esi, QUIESCE_STATUS_LOCATION
  mov eax, dword ptr[esi]
  or  eax, B_MONARCH_AP_READY
  mov dword ptr[esi], eax

ap_check_again:
  mov   eax, dword ptr [esi]
  test  eax, B_RECONFIG_DONE       ; Monarch Reconfigure Done?
  jz    ap_check_again              ; br, if no
  
restore_prefetch_control: 
  ; restore proc's prefetch control
  mov ecx, MSR_MISC_FEATURE_CONTROL
  mov rax, r8   ;AP_Misc_feature_control   ; eax contain Misc_feature_control[31:0]
  mov rdx, rax
  shr rdx, 20h                    ; edx contains Misc_feature_control[63:32]
  wrmsr

  cmp r10, MKF_I_AM_MONARCH 
  jne monarch_ap_done_restore  ; br, if i am monarch

  ; set monarch Status = Reconfig_done
  mov edi, ebp
  add edi, QUIESCE_STATUS_LOCATION   ; read UBox FW scratch csr 10
  mov eax, dword ptr [edi]
  or  eax, B_RECONFIG_DONE       ; set Monarch Reconfig Done
  mov dword ptr [edi], eax

  ; wait until MonarchAP done
check_monarch_ap:
  cmp  r9b, 1                     ; check if SMT enabled
  jne  quiesce_in_cache_done      ; br, if no Monarch AP
  mov  eax, dword ptr [edi]
  test eax, B_MONARCH_AP_DONE     ; test bit1, for Monarch AP done flag
  jz   check_monarch_ap    
  jmp  quiesce_in_cache_done      ; SMT enable, Monarch AP already did shared msr 1A4 

monarch_ap_done_restore:
  ; i am Monarch AP, set my Status = AP_DONE
  mov  edi, ebp
  add  edi, QUIESCE_STATUS_LOCATION     ; read UBox FW 22 which is Monarch AP status
  mov  eax, dword ptr [edi]
  or   eax, B_MONARCH_AP_DONE
  mov  dword ptr [edi], eax
  xor  rbx, rbx                     ; set Status OK
  jmp  quiesce_in_cache_done

quiesce_in_cache_done:

  mov rax, rbx              ; return flag in rax

  pop r12
  pop rbp
  pop rdi
  pop rsi
  pop rbx 
 
  ret

QuiesceRunOutOfCache ENDP


;typedef struct {
;  UINT32                AndMask;
;
;  union {
;    UINT32              Data;
;   UINT32              OrMask;
;  } DataMask;
;
;} QUIESCE_DATA_MASK;

;typedef struct {
;  QUIESCE_OPERATION     Operation
;  UINT32                AccessWidth;
;  UINT64                CsrAddress;
;  QUIESCE_DATA_MASK     CsrData;
;} QUIESCE_DATA;
DATA_MASK UNION
  Data            dd ?
  OrMask          dd ?
DATA_MASK ENDS

QUIESCE_DATA_MASK STRUC
  AndMask         dd        ? ; And MASK data
  DataMask        DATA_MASK <> ; 
QUIESCE_DATA_MASK ENDS

QuiesceDataStruc STRUC
  Operation       db                ?  ; enum: 1=write, 2=poll, 3-Run Sv Quiesce code, -1=OperationEnd
  AccessWidth     db                ?  ;
  CsrAddress      dq                ?  ;
  CsrData         QUIESCE_DATA_MASK <> ;
QuiesceDataStruc  ENDS

QuiesceCacheCodeSize DD  $ - QuiesceCacheCodeTemplate


text  ENDS

END


