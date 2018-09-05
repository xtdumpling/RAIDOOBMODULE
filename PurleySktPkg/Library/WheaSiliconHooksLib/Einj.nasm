;------------------------------------------------------------------------------
;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;
; Copyright (c)2009-2015 Intel Corporation. All rights reserved
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
;   Einj.nasm
;
; Abstract:
;
;   Assembly code of Error Injection Related Functions
;
;------------------------------------------------------------------------------

  SECTION .text

;------------------------------------------------------------------------------
; VOID
; IA32API
; TurnOnQuiesce (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(TurnOnQuiesce)
ASM_PFX(TurnOnQuiesce): ;NEAR PUBLIC
;Tun ON Quiesce
  xor edx, edx
  mov ecx, 0x50
  mov eax, 01
  wrmsr

  ret

;------------------------------------------------------------------------------
; VOID
; IA32API
; TurnOffQuiesce (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(TurnOffQuiesce)
ASM_PFX(TurnOffQuiesce): ;NEAR PUBLIC
;Tun Off Quiesce
  xor edx, edx
  mov ecx, 0x50
  mov eax, 02
  wrmsr

  ret

;------------------------------------------------------------------------------
; VOID
; IA32API
; AsmMfence (
;   VOID
;   );
;------------------------------------------------------------------------------
global ASM_PFX(AsmMfence)
ASM_PFX(AsmMfence): ;NEAR PUBLIC
    mfence
    ret

