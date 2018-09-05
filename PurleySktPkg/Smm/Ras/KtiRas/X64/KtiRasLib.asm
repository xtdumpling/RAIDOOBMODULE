;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
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
;  KtiRasLib.asm
;
; Abstract:
;
;  This file contains libraries written in assembly
;
;------------------------------------------------------------------------------

text  SEGMENT

MSR_TKRMODE_PATCH              EQU   052h
MSR_BBOX_FEATURE_PATCH         EQU   055h
MSR_BBOX_FEATURE_PATCH1        EQU   056h

FW_SCRATCH_22                  EQU   0FEB204D8h
PLAT_MISC_CTRL                 EQU   0FEB20294h

BMP_C0_STEPPING                EQU   05h

BMP_MODEL                      EQU   0Eh
WSM_EX_MODEL                   EQU   0Fh

;------------------------------------------------------------------------------
;UINT8
;GetCpuSteppingx64 (
;  VOID
;  );
;------------------------------------------------------------------------------
GetCpuSteppingx64 PROC  

  Push     Rbx
  Push     Rcx
  Push     Rdx

  Mov      Rax, 1
  Cpuid
  And      Rax, 0Fh ; Extract Stepping ID

  Pop      Rdx
  Pop      Rcx
  Pop      Rbx
  Ret

GetCpuSteppingx64 ENDP 


;------------------------------------------------------------------------------
;UINT8
;GetCpuModelx64 (
;  VOID
;  );
;------------------------------------------------------------------------------
GetCpuModelx64 PROC  

  Push     Rbx
  Push     Rcx
  Push     Rdx

  Mov      Rax, 1
  Cpuid
  And      Rax, 0F0h ; Extract Stepping ID
  shr      Rax, 4;

  Pop      Rdx
  Pop      Rcx
  Pop      Rbx
  Ret

GetCpuModelx64 ENDP 

EfiDisableInterrupts_1    PROC   
    cli
    ret
EfiDisableInterrupts_1    ENDP

EfiHalt_1    PROC    
    hlt
    ret
EfiHalt_1    ENDP

;------------------------------------------------------------------------------
;This function is called in RasMpLinkMileStone12() after RasMpLinkMileStone10().
;RasMpLinkMileStone10() handles coherence and updates UBx_CR_U_PCSR_FW_SCRATCH_22.
;Therefore, entering here, UBx_CR_U_PCSR_FW_SCRATCH_22 ready has the bits value for APs to write to MSR's
;In addition to MSR_TKRMODE_PATCH 0x52, MSR_BBOX_FEATURE_PATCH 0x55 is added to update BB0_CR_B_CCSR_DEBUG_REG1  
;
;Apply the tracker mode patch for BMP sighting S3472299 (ECO_ID: 774905). 
;Apply to only CPUs with NHM-EX stepping C0 and above
;
;RAS_MPLINK_STATUS
;TkrModePatchRas( 
;  VOID
;  );
;------------------------------------------------------------------------------
TkrModePatchRas PROC 

  Push     Rbx
  Push     Rcx
  Push     Rdx
  Push     Rsi


  Mov      Ecx, MSR_TKRMODE_PATCH
  Rdmsr
  And      Eax, 0FFFFFFFDh  ; Clear the tracker mode patch bit
  Mov      Esi, FW_SCRATCH_22 ;
  Mov      Ebx, Dword Ptr [esi]
  Test     Ebx, 0100h ; BIT 8 is updated by SBSP with the setup value
  Jz       @f
  Or       Eax, 2  ; Set the tracker mode patch bit

@@:
  Or       Eax, 080h  ; Set the lock bit
  Wrmsr

  call     GetCpuModelx64
  cmp      al, WSM_EX_MODEL
  jne      TkrModePatchRasExit

  Mov      Esi, PLAT_MISC_CTRL
  Mov      Dword Ptr [Esi], 1           ; set Locked Tap Override for the pbox margining registers
  Mov      Ecx, MSR_BBOX_FEATURE_PATCH
  Rdmsr
  And      Eax, 0FC7FFFFFh              ; Clear bit[25:23]
  And      Edx, 0FC7FFFFFh              ; Clear bit[25:23]
  Mov      Esi, FW_SCRATCH_22           ;
  Mov      Esi, Dword Ptr [Esi]         ; Bit[30:28] has the 3 bit values
  Shr      Esi, 5                       ; bit 28-23
  And      Esi, 003800000h              ; Extract bit[25:23]
  or       Eax, Esi
  or       Edx, Esi
  Wrmsr


  Mov      Ecx, MSR_BBOX_FEATURE_PATCH1
  Rdmsr
  And      Eax, NOT (30000000h)         ; clear dispclscnflt(BIT29) and frcfwdinvitoe(BIT28)
  And      Edx, NOT (30000000h)         ;
  Test     Esi, 000800000h              ; apply DAS w/a if DAS is enabled, i.e. MSR_BBOX_FEATURE_PATCH1 bit23 
  jz       @f                           ;
  Or       Eax, 30000000h               ; set dispclscnflt(BIT29) and frcfwdinvitoe(BIT28)
  Or       Edx, 30000000h
@@:
  Wrmsr


  Mov      Esi, PLAT_MISC_CTRL
  Mov      Dword Ptr [Esi], 0           ; clear Locked Tap Override for the pbox margining registers

TkrModePatchRasExit:
  Xor      Rax, Rax	                ; Indicate Success
  Pop      Rsi
  Pop      Rdx
  Pop      Rcx
  Pop      Rbx
  Ret

TkrModePatchRas ENDP

text  ENDS
END