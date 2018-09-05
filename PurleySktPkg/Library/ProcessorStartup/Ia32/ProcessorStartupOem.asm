;
; This file contains an 'Intel Pre-EFI Module' and is licensed
; for Intel CPUs and Chipsets under the terms of your license 
; agreement with Intel or your vendor.  This file may be      
; modified by the user, subject to additional terms of the    
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2012, Intel Corporation. All rights reserved.<BR>
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
;  ProcessorStartupOem.asm
;
; Abstract:
;
;
;------------------------------------------------------------------------------
        .586P
        .XMM
IFDEF MINIBIOS_BUILD
        .MODEL  SMALL
ELSE
        .MODEL  SMALL, c
ENDIF

IFNDEF MINIBIOS_BUILD
        EXTRN   PcdGet32 (PcdFlashNvStorageMicrocodeBase):DWORD
        EXTRN   PcdGet32 (PcdFlashNvStorageMicrocodeSize):DWORD
ENDIF        
        EXTERN  AmILegacySocketNoStack:NEAR32
        EXTERN  GetMmcfgNoStack:NEAR32
        include ProcessorStartupPlatform.inc
        include ProcessorStartupUncore.inc
        include ProcessorStartup.inc

   .data

;-------------------------------------------------------
; Microcode Update region
;-------------------------------------------------------

; Microcode updates must be aligned on a paragraph boundary. If more
; than microcode update is included in the microcode storage block,
; each microcode update be forced to to align on paragraph boundary.
IFDEF MINIBIOS_BUILD
align 16
PUBLIC microcode_region_start
PUBLIC microcode_region_size
; This is the microcode update region that will be searched and loaded
; by CPU reference code.

microcode_region_start label byte
   ; Include the first microcode update here
   ;
    include ucode\null_patch_bios_skx.inc               ; SKX Pre-Si dummy patch

align 16
   ; Include the next microcode update here
   ;
    include ucode\mef306f1_80000013.inc                 ; HSX B0 Stepping

align 16
   ; Include the next microcode update here
   ;
    include ucode\mef306f2_c0000007.inc                 ; HSX C0 Stepping

align 16
   ; Include the next microcode update here
   ;
microcode_region_size EQU ($-microcode_region_start)  ; size of entire microcode update region above
ENDIF
;------------------------------------------------------------------
; ROM-based options. Values in each field can be customized by OEM
;------------------------------------------------------------------
nem_initializers   TEXTEQU  <CODE_REGION_BASE_ADDRESS,\
                             CODE_REGION_SIZE,\
                             DATA_STACK_BASE_ADDRESS,\
                             DATA_STACK_SIZE>

IFDEF MINIBIOS_BUILD
oem_cpu_rom_setup_options     cpu_rom_setup_options <\
         offset microcode_region_start,\                  ; uCode region base address
         microcode_region_size,\                          ; uCode region total size
         nem_initializers\                                ; NEM parameters
         >
ENDIF
;----------------------------------------------------------------------------
;       STARTUP_SEG  S E G M E N T  STARTS
;----------------------------------------------------------------------------
STARTUP_SEG SEGMENT PARA PUBLIC 'CODE' USE32
IFDEF MINIBIOS_BUILD


;-----------------------------------------------------------------
;   Executed by SBSP and NBSP
;   Returns build-time platform options related to CPU early init
; Input:
;   Stack not available
;   mmx6 = routine return address
; Output:
;   ebp = pointer to structure cpu_rom_setup_option
;   All other registers destroyed
;-----------------------------------------------------------------

oem_get_cpu_rom_setup_options         PROC   PUBLIC

   ; OEM can override this default implementation

   mov   ebp, offset oem_cpu_rom_setup_options
   RET_ESI_MMX6                   ; return to caller

oem_get_cpu_rom_setup_options         ENDP
ENDIF


;-----------------------------------------------------------------
; This routine returns CPU-specific platform setup options/settings 
; to the caller via ECX register
;-----------------------------------------------------------------
;   Executed by SBSP and NBSP
; Input:
;   Stack not available
;   mmx5 = routine return address
;   MMCFG/CPUBUSNO has been configured so node BSPs can access CSRs on socket 0, and vice-versa
;   xmm registers initialized by EnableAccessCSR routine
;      xmm1 register [31:24] contains local Socket ID
;      xmm3 register contains MMCFG_RULE
;   xmm4,xmm6 registers contain the IIO bus number array initialized by EnableAccessCSR routine
;       xmm4: socket 0 - 3, xmm6: 4 - 7
;   xmm5,xmm7 registers contain the Uncore bus number array initialized by EnableAccessCSR routine
;       xmm5: socket 0 - 3, xmm7: 4 - 7
;  Output: 
;  Output: 
;    ECX[0]  - DCU_MODE select   0/1:   32KB 8-way non-ECC (hardware default) / 16KB 4-way ECC 
;    ECX[1]  - debugInterfaceEn   1:   Enable Debug Interface
;    ECX[31:2] reserved
;   All other general purpose registers destroyed
;-----------------------------------------------------------------

GetPlatformCpuSettings PROC      PUBLIC

;
; DCU_MODE Select setup data is stored(copied) into sticky scratch pad register SSR07[24] on the warm boot path.
; Read the scratch pad register and returns the DCU_MODE setting to caller
;
   CALL_MMX6  GetMmcfgNoStack                      ;[edi] = MMCFG base
   add   edi, CSR_BIOSSCRATCHPAD7
   xor   edx, edx

   mov   ebx, dword ptr ds:[edi]                   ; ebx = CSR data

   test  ebx, BIT5                                 ; "Comlepted Cold Reset Flow"?
   jz    short gdms_50                             ; no
   test  ebx,  BIT27                               ; ebx[27] = debugInterfaceEn info saved from cold boot path
   jz    @f
   or    edx, CPU_OPTIONS_BIT_DEBUG_INTERFACE_EN   ; return debugInterfaceEn = 1
@@:
   test  ebx,  BIT24                               ; ebx[24] = DCU Mode Select info saved from cold boot path
   jz    short gdms_50
   or    edx, CPU_OPTIONS_BIT_DCU_MODE_SEL         ; return DCU_MODE select = 1

gdms_50:
   mov   ecx, edx                                  ; return data in ecx
   RET_ESI_MMX5

GetPlatformCpuSettings ENDP


; Stub dummy hook for MiniBIOS implementation
_OEMCpuSecInit  PROC  PUBLIC
       ret

_OEMCpuSecInit  ENDP

;-----------------------------------------------------------------
; This routine returns the address of CPU microcode update
;-----------------------------------------------------------------
;   Executed by SBSP and NBSP
; Input:
;   Stack not available
;   mmx4 = routine return address
; Output:
;   EAX = Zero       No matching microcode found
;   EAX = Non-Zero   Linear address of CPU microcode update header that matches the current CPU
;   All other registers destroyed
;-----------------------------------------------------------------
OemFindMicrocode PROC      PUBLIC

IFDEF MINIBIOS_BUILD
   CALL_MMX6   oem_get_cpu_rom_setup_options  ;

   mov   esi, [ebp].cpu_rom_setup_options.ucode_region_addr ; esi = Start address of ucode region
ELSE
   mov   esi, PcdGet32 (PcdFlashNvStorageMicrocodeBase)  ; esi = Start address of ucode region
   add   esi, 60h   ;bypass fv header
ENDIF
   ; Get processor signature and platform ID from the installed processor
   ; and save into registers for later use
   ; ebx = processor signature
   ; edx = platform ID
   mov   eax, 1
   cpuid
   mov   ebx, eax
   mov   ecx, MSR_IA32_PLATFORM_ID
   rdmsr
   mov   ecx, edx
   shr   ecx, 50-32
   and   ecx, 7h
   mov   edx, 1
   shl   edx, cl

   ; Current register usage
   ; esi -> microcode update to check
   ; ebx = processor signature
   ; edx = platform ID

check_main_header:
   ; Check for valid microcode header
   ; Minimal test checking for header version and loader version as 1
   mov   eax, dword ptr 1
   cmp   [esi].UpdateHeaderStruc.dHeaderVersion, eax
   jne   advance_fixed_size
   cmp   [esi].UpdateHeaderStruc.dLoaderRevision, eax
   jne   advance_fixed_size

   ; Check if signature and plaform ID match
   cmp   ebx, [esi].UpdateHeaderStruc.dProcessorSignature
   jne   @f
   test  edx, [esi].UpdateHeaderStruc.dProcessorFlags
   jnz   load_check  ; Jif signature and platform ID match

@@:
   ; Check if extended header exists
   ; First check if total_size and data_size are valid
   xor   eax, eax
   cmp   [esi].UpdateHeaderStruc.dTotalSize, eax
   je    next_microcode
   cmp   [esi].UpdateHeaderStruc.dDataSize, eax
   je    next_microcode

   ; Then verify total size - sizeof header > data size
   mov   ecx, [esi].UpdateHeaderStruc.dTotalSize
   sub   ecx, sizeof UpdateHeaderStruc
   cmp   ecx, [esi].UpdateHeaderStruc.dDataSize
   jbe   next_microcode    ; Jif extended header does not exist

   ; Check if total size fits in microcode region
   mov   edi, esi
   add   edi, [esi].UpdateHeaderStruc.dTotalSize
IFDEF MINIBIOS_BUILD
   mov   ecx, [ebp].cpu_rom_setup_options.ucode_region_addr
   add   ecx, [ebp].cpu_rom_setup_options.ucode_region_size
ELSE
   mov   ecx, PcdGet32 (PcdFlashNvStorageMicrocodeBase)
   add   ecx, PcdGet32 (PcdFlashNvStorageMicrocodeSize)
ENDIF
   cmp   edi, ecx
   ja    exit_not_found              ; Jif address is outside of ucode region

   ; Set edi -> extended header
   mov   edi, esi
   add   edi, sizeof UpdateHeaderStruc
   add   edi, [esi].UpdateHeaderStruc.dDataSize

   ; Get count of extended structures
   mov   ecx, [edi].ext_sig_hdr.count

   ; Move pointer to first signature structure
   add   edi, sizeof ext_sig_hdr

check_ext_sig:
   ; Check if extended signature and platform ID match
   cmp   [edi].ext_sig.processor, ebx
   jne   @f
   test  [edi].ext_sig.flags, edx
   jnz   load_check     ; Jif signature and platform ID match
@@:
   ; Check if any more extended signatures exist
   add   edi, sizeof ext_sig
   loop  check_ext_sig

next_microcode:
   ; Advance just after end of this microcode
   xor   eax, eax
   cmp   [esi].UpdateHeaderStruc.dTotalSize, eax
   je    @f
   add   esi, [esi].UpdateHeaderStruc.dTotalSize
   jmp   check_address
@@:
   add   esi, dword ptr 2048
   jmp   check_address

advance_fixed_size:
   ; Advance by 4X dwords
   add   esi, dword ptr 1024

check_address:
   ; Address >= microcode region address + microcode region size?
IFDEF MINIBIOS_BUILD
   mov   eax, [ebp].cpu_rom_setup_options.ucode_region_addr
   add   eax, [ebp].cpu_rom_setup_options.ucode_region_size
ELSE
   mov   eax, PcdGet32 (PcdFlashNvStorageMicrocodeBase)
   add   eax, PcdGet32 (PcdFlashNvStorageMicrocodeSize)
ENDIF
   cmp   esi, eax
   jae   exit_not_found        ;Jif address is outside of ucode region
   jmp   check_main_header


load_check:
   ; Get the revision of the current microcode update loaded
    mov ecx, MSR_IA32_BIOS_SIGN_ID
        xor   eax, eax                  ; Clear EAX
        xor     edx, edx                            ; Clear EDX
        wrmsr                                           ; Load 0 to MSR at 8Bh
        mov   eax, 1
        cpuid
        mov   ecx, MSR_IA32_BIOS_SIGN_ID
        rdmsr                         ; Get current microcode signature

   ; Verify this microcode update is not already loaded
   cmp   [esi].UpdateHeaderStruc.dUpdateRevision, edx
   je    exit_not_found

found_microcode:
   mov   eax, esi                 ; EAX contains the linear address of the start of the Microcode Update Header
   jmp   short exit

exit_not_found:
   xor   eax, eax                 ; ucode not found

exit:
   RET_ESI_MMX4                   ; return to caller

OemFindMicrocode ENDP

STARTUP_SEG ENDS

END

