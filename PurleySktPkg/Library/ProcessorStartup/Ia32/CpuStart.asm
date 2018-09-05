;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Intel Restricted Secret
;
; CPU Reference Code
;
; Copyright (c) 2007-2015, Intel Corporation.
;
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
; This program has been developed by Intel Corporation.
; Licensee has Intel's permission to incorporate this source code
; into their product, royalty free.  This source code may NOT be
; redistributed to anyone without Intel's written permission.
;
; Intel specifically disclaims all warranties, express or
; implied, and all liability, including consequential and other
; indirect damages, for the use of this code, including liability
; for infringement of any proprietary rights, and including the
; warranties of merchantability and fitness for a particular
; purpose.  Intel does not assume any responsibility for any
; errors which may appear in this code nor any responsibility to
; update it.
;
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Module Name:
;
;   CpuStart.asm
;
; Abstract:
;
;   This is the bridge between .asm and .c code
;------------------------------------------------------------------------------
   .listall
   .686p
   .xmm
   .model   flat,c

.XLIST
   ; Files generated during build time...do not edit!!!
   include cpuhost.inc
.LIST


INCLUDE ProcessorStartupPlatform.inc
INCLUDE ProcessorStartup.inc
INCLUDE ProcessorStartupUncore.inc

.data
   DD    12345678h

.code

CpuStart  PROC  PUBLIC
   ; Assumptions:
   ;   Executed by SBSP only
   ;   Microcode Update loaded on all BSPs
   ;   NEM enabled on all BSPs (Cache As Ram available)
   ;   MMCFG/BusNo configured on all BSPs
   ;   No system memoy is available

   ; Inputs:
   ;   None
   ; Register Usage:
   ;   All others destroyed
   ;

  ; Call into C code
   extern CpuMain:NEAR32
   call CpuMain

   ; Check for NEM data stack overflow
   call CheckNemDataStackDepth

   ; Disable No-Eviction Mode (NEM)
   call  NemExit                 ; this call will not return because stack will no longer be available after NEM is disabled

CpuStart  ENDP

;----------------------------------------------------------------------------
;
;  NemExit
;
;  Purpose:
;   Exits the cache configuration as specified in
;   the BIOS Writers Guide for No-Eviction Mode (NEM).
;
;  Assumptions:
;  (1) Processor operating mode is flat 32-bit protected mode.
;  (2) Only SBSP executes this routine
;  (3) Stack is available
;  (4) Any NEM-based data required by the BIOS must be copied to the initial
;      system memory.  The initial system memory must be in the un-cacheable
;      (UC) state.
;
;  Inputs:
;     None
;
;  Outputs:
;     Preserved: esp, ebp, esi
;     Destroyed: all other registers
;
;----------------------------------------------------------------------------
NemExit   PROC

   ;Set CR0.CD bit to globally disable cache
   ;mov  eax, cr0
   ;or   eax, 40000000h
   ;mov  cr0, eax

   ;  Invalidate the cache.  Execute the INVD instruction to flush the cache.
   ;  After this point, cache data is no longer valid.
   ;
   invd

   ;  Disable the MTRRs by clearing the IA32_MTRR_DEF_TYPE MSR E flag.
   ;
   mov   ecx, MTRR_DEF_TYPE            ; Load the MTRR default type index
   rdmsr
   and   eax, NOT (DWORD PTR MTRR_DEF_TYPE_E)   ; Disable all MTRRs
   wrmsr

   ;  Disable No-Eviction Mode Run State by clearing
   ;  NO_EVICT_MODE MSR 2E0h bit [1] = 0.
   ;
   mov   ecx, MSR_NO_EVICT_MODE
   rdmsr
   btr   eax, 1
   wrmsr

   ;  Disable No-Eviction Mode Setup State by clearing
   ;  NO_EVICT_MODE MSR 2E0h bit [0] = 0.
   ;
   ;rdmsr
   btr   eax, 0
   wrmsr

   ;
   ; Renable fast strings after NEM exit
   ; FAST_STRING_ENABLE = 1
   ;
   mov   ecx, IA32_MISC_ENABLE
   rdmsr
   or    eax, FAST_STRING_ENABLE_BIT
   wrmsr
   
   ; Read EMULATION_FLAG CSR  and set the Emulation_flag in xmm0
   ;
   mov   eax, CSR_LEGACY_EMULATION_FLAG_DMI
   READ_PCI_DWORD_LEGACY                             ; ebx = reg data
   test  ebx, (BIT2+BIT1+BIT0)                       ; 0 = real hardware
   jnz    @f

   ;
   ; Enable mcaonnonnemcacheablemmio.  This MSR cannot be read
   ;
   mov   ecx, VIRTUAL_MSR_MCA_ON_NON_NEW_CACHABLE_MMIO_EN_ADDR
   mov   eax, MCAONNONNEMCACHEABLEMMIO_BIT
   xor   edx, edx
   wrmsr

@@:
   ;  Configure system memory and cache as specified in the IA-32 Intel® Architecture
   ;  Software Developer’s Manual, Volume 3: System Programming Guide.  E.G. initialize
   ;  any remaining memory and MTRRs and enable cache by setting CR0.CD = 0.
   ;

   jmp $       ; stack is no longer available for "ret"
   ;  Continue with POST.
   ;
   ret

NemExit  ENDP


;------------------------------------------------------------------------------
; CheckNemDataStackDepth
;
; Check how much of NEM data/stack region memory we have used up.
; Do not enable stack check except in test environment!
;
; Input:
;       Processor Operating Mode: 32-bit flat mode,
;       32-bit addressability from 0 thru 4GB.
;       CS = Flat-mode segment address of the segment this code is executing out of.
;       DS = Selector that points to a descriptor with
;               Base  = 000000000h
;               Limit = 0FFFFFFFFh
;               Present, DPL = 0, Expand-up, Read-Write, Granularity = 4K
;       ES, SS, FS, GS = Same settings as DS.
;
; Output:
;       EAX = Amount of stack consumed
;       Hangs with jmp $ if stack overflow is detected.
;
; Destroys:
;       EAX, EBX, ECX, EDX, ESI, EDI, EBP.
;       Processor MTRRs as needed.
;
; Note:
;------------------------------------------------------------------------------
CheckNemDataStackDepth  PROC    NEAR    PUBLIC

   mov   esi, DATA_STACK_BASE_ADDRESS
   add   esi, DATA_STACK_SIZE
   sub   esi, 40h                      ; esi -> Bottom of Stack
   cld

   mov   edi, DATA_STACK_BASE_ADDRESS  ; edi -> Max Limit of Top of stack (bottom of NEM data region)
   mov   ecx, DATA_STACK_SIZE
   shr   ecx, 2                        ; Divide by 4 (loop count in DWord)
   mov   eax, 5AA55AA5h                ; data pattern used to initialize the entire stack region
   cmp   eax, DWORD PTR es:[edi]        ; last dword at max limit of top of stack over-written?
   jne   Nem_Stack_Overflow            ; yes
   repe  scas DWORD PTR es:[edi]       ; scan for initial data pattern in stack region for the first mismatch

   mov   eax, esi
   sub   eax, edi

   ;
   ; Stack depth in EAX
   ;

   cmp   eax, DATA_STACK_SIZE
   jb    @f    ; Jump if stack overflow not detected

Nem_Stack_Overflow:
   ; Stack usage has reached top of stack (bottom of CAR data region).
   ; Potential stack overflow detected
   mov   al, 0FBh
   out   80h, al
   jmp   $

@@:
   ret

CheckNemDataStackDepth  ENDP

;MAKE_STACKS

END
