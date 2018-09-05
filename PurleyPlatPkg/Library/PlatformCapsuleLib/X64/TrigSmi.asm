;
; This file contains a 'Sample Driver' and is licensed as such
; under the terms of your license agreement with Intel or your
; vendor.  This file may be modified by the user, subject to  
; the additional terms of the license agreement               
;
;
; Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
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
;   TrigSmi.asm
;
; Abstract:
;
;   TrigSmi for capsule update. SMI number is 0x27 and 0x28.
;
; Notes:
;
;------------------------------------------------------------------------------

      TITLE   TrigSmi.asm: Assembly code 


text    SEGMENT

;------------------------------------------------------------------------------
;  VOID
;  SendCapsuleSmi (
;    UINTN Addr
;    )
;------------------------------------------------------------------------------
SendCapsuleSmi PROC PUBLIC
  push  rbp
  mov   rbp, rsp
  sub   rsp, 8
  push  rax
  push  rbx
  mov   rbx, rcx
  mov   rax, 0EF27h
  out   0B2h, AL   
  pop   rbx
  pop   rax
  mov   rsp, rbp
  pop   rbp
  ret   0
SendCapsuleSmi ENDP

;------------------------------------------------------------------------------
;  VOID
;  GetUpdateStatusSmi (
;    UINTN Addr
;    )
;------------------------------------------------------------------------------
GetUpdateStatusSmi PROC PUBLIC
  push  rbp
  mov   rbp, rsp
  sub   rsp, 8
  push  rax
  push  rbx
  mov   rbx, rcx
  mov   rax, 0EF28h
  out   0B2h, AL   
  pop   rbx
  pop   rax
  mov   rsp, rbp
  pop   rbp
  ret   0
GetUpdateStatusSmi ENDP

text ENDS
END
