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
;   Einj.asm
;
; Abstract:
;
;   Assembly code of Error Injection Related Functions
;
;------------------------------------------------------------------------------
 
  .CODE

;------------------------------------------------------------------------------
; VOID
; IA32API
; TurnOnQuiesce (
;   VOID
;   );
;------------------------------------------------------------------------------
TurnOnQuiesce   PROC ;NEAR PUBLIC
;Tun ON Quiesce
  xor edx, edx
  mov ecx, 50h
  mov eax, 01
  wrmsr
  
  ret

TurnOnQuiesce ENDP

;------------------------------------------------------------------------------
; VOID
; IA32API
; TurnOffQuiesce (
;   VOID
;   );
;------------------------------------------------------------------------------
TurnOffQuiesce   PROC ;NEAR PUBLIC
;Tun Off Quiesce
  xor edx, edx
  mov ecx, 50h
  mov eax, 02
  wrmsr
  
  ret

TurnOffQuiesce ENDP


;------------------------------------------------------------------------------
; VOID
; IA32API
; AsmMfence (
;   VOID
;   );
;------------------------------------------------------------------------------
AsmMfence   PROC ;NEAR PUBLIC
    mfence
    ret
AsmMfence   ENDP

      END
