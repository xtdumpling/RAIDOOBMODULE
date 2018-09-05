;
; This file contains an 'Intel Peripheral Driver' and is      
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may 
; be modified by the user, subject to additional terms of the 
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 2007 Intel Corporation. All rights reserved
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
;   txt.inc
;
; Abstract:
;
;------------------------------------------------------------------------------

;
; MSRs
;
IA32_CR_FEATURE_CONTROL            EQU 03Ah
IA32_MISC_ENABLE                   EQU 1A0h
LT_OPT_IN_MSR_VALUE                EQU 01111111100000011b ; 0FF03h
LT_OPT_IN_VMX_ONLY_MSR_VALUE       EQU 00000000000000101b ; 00005h

ACMOD_SIZE                         EQU 24

CR0_NE_MASK                        EQU (1 SHL 5)
CR0_NW_MASK                        EQU (1 SHL 29)
CR0_CD_MASK                        EQU (1 SHL 30)
CR0_PG_MASK                        EQU (1 SHL 31)
CR4_DE_MASK                        EQU (1 SHL 3)
CR4_OSFXSR_MASK                    EQU (1 SHL 9)

;-----------------------------------------------------------------------------
; MTRRs
;
IA32_MTRR_PHYSBASE0                EQU 200H 
IA32_MTRR_PHYSMASK0                EQU 201H 
IA32_MTRR_PHYSBASE1                EQU 202H 
IA32_MTRR_PHYSMASK1                EQU 203H 
IA32_MTRR_PHYSBASE2                EQU 204H 
IA32_MTRR_PHYSMASK2                EQU 205H 
IA32_MTRR_PHYSBASE3                EQU 206H 
IA32_MTRR_PHYSMASK3                EQU 207H 
IA32_MTRR_PHYSBASE4                EQU 208H 
IA32_MTRR_PHYSMASK4                EQU 209H 
IA32_MTRR_PHYSBASE5                EQU 20AH 
IA32_MTRR_PHYSMASK5                EQU 20BH 
IA32_MTRR_PHYSBASE6                EQU 20CH 
IA32_MTRR_PHYSMASK6                EQU 20DH 
IA32_MTRR_PHYSBASE7                EQU 20EH 
IA32_MTRR_PHYSMASK7                EQU 20FH 
IA32_MTRR_FIX64K_00000             EQU 250H 
IA32_MTRR_FIX16K_80000             EQU 258H 
IA32_MTRR_FIX16K_A0000             EQU 259H 
IA32_MTRR_FIX4K_C0000              EQU 268H 
IA32_MTRR_FIX4K_C8000              EQU 269H 
IA32_MTRR_FIX4K_D0000              EQU 26AH 
IA32_MTRR_FIX4K_D8000              EQU 26BH 
IA32_MTRR_FIX4K_E0000              EQU 26CH 
IA32_MTRR_FIX4K_E8000              EQU 26DH 
IA32_MTRR_FIX4K_F0000              EQU 26EH 
IA32_MTRR_FIX4K_F8000              EQU 26FH 
IA32_CR_PAT                        EQU 277H 
IA32_MTRR_DEF_TYPE                 EQU 2FFH 
IA32_MTRR_CAP                      EQU 0FEH
  IA32_MTRR_SMRR_SUPPORT_BIT       EQU (1 SHL 11)
IA32_FEATURE_CONTROL               EQU 03AH
  IA32_SMRR_ENABLE_BIT             EQU (1 SHL 3)

;
; Only low order bits are assumed
;
MTRR_MASK                          EQU 0FFFFF000h

MTRR_ENABLE                        EQU (1 SHL 11)
MTRR_FIXED_ENABLE                  EQU (1 SHL 10)

MTRR_VALID                         EQU (1 SHL 11)
UC                                 EQU 00h
WB                                 EQU 06h

MTRR_VCNT                          EQU 8

MTRR_MASK_4K_LOW                   EQU 0fffff800h
MTRR_MASK_4K_HIGH                  EQU 0fh
UINT32_4K_MASK                     EQU 0fffff000h

;----------------------------------------------------------------------------
; APIC definitions
;
IA32_APIC_BASE                     EQU 001Bh  ; APIC base MSR
BASE_ADDR_MASK                     EQU 0FFFFF000h
APIC_ID                            EQU 20h
ICR_LOW                            EQU 300h
ICR_HIGH                           EQU 310h
SPURIOUS_VECTOR_1                  EQU 0F0h

;-----------------------------------------------------------------------------
; Features support & enabling
;
CR4_VMXE                           EQU (1 SHL 13)
CR4_SMXE                           EQU (1 SHL 14)
CR4_PAE_MASK                       EQU (1 SHL 5)

CPUID_VMX                          EQU (1 SHL 5)
CPUID_SMX                          EQU (1 SHL 6)

;-----------------------------------------------------------------------------
; Machne check architecture MSR registers
;        
MCG_CAP                            EQU 179h
MCG_STATUS                         EQU 17Ah
MCG_CTL                            EQU 17Bh
MC0_CTL                            EQU 400h
MC0_STATUS                         EQU 401h
MC0_ADDR                           EQU 402h
MC0_MISC                           EQU 403h

_GETSEC                            EQU db 0fh, 37h

ENTERACCS                          EQU 02h
PARAMETERS                         EQU 06h

