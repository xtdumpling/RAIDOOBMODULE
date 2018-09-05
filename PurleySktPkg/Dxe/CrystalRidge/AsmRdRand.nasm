;------------------------------------------------------------------------------
;
; Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
; This program and the accompanying materials
; are licensed and made available under the terms and conditions of the BSD License
; which accompanies this distribution.  The full text of the license may be found at
; http://opensource.org/licenses/bsd-license.php.
;
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
;
; Module Name:
;
;   AsmRdRand.Asm
;
; Abstract:
;
;   Implementation for 16-, 32-, and 64-bit invocations of RDRAND instruction under 64bit platform.
;
; Notes:
;
;   Visual Studio coding practices do not use inline asm since multiple compilers and
;   architectures are supported assembler not recognizing rdrand instruction so using DB's.
;
;------------------------------------------------------------------------------

    SECTION .text

;------------------------------------------------------------------------------
;  Generate a 64 bit random number
;    Return TRUE if RN generated successfully, or FALSE if not
;
;  BOOLEAN EFIAPI RdRand64Step (UINT64 *Random);   RCX
;------------------------------------------------------------------------------
global ASM_PFX(RdRand64Step)
ASM_PFX(RdRand64Step):
    ; rdrand   rax                 ; generate a 64 bit RN into rax, CF=1 if RN generated ok, otherwise CF=0
    db     0x48, 0xf, 0xc7, 0xf0   ; rdrand r64: "REX.W + 0F C7 /6 ModRM:r/m(w)"
    jb     rn64_ok                 ; jmp if CF=1
    xor    rax, rax                ; reg=0 if CF=0
    ret                            ; return with failure status
rn64_ok:
    mov    [rcx], rax
    mov    rax, 1
    ret

