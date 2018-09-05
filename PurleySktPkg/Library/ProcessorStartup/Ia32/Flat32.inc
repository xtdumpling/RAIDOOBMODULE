;
; This file contains an 'Intel Pre-EFI Module' and is licensed
; for Intel CPUs and Chipsets under the terms of your license 
; agreement with Intel or your vendor.  This file may be      
; modified by the user, subject to additional terms of the    
; license agreement                                           
;
;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2010, Intel Corporation. All rights reserved.<BR>
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
;   Flat32.inc
;
; Abstract:
;
;   Platform constants and macros
;
;------------------------------------------------------------------------------
;MACROS

BIT0                          EQU 01h
BIT1                          EQU 02h
BIT2                          EQU 04h
BIT3                          EQU 08h
BIT4                          EQU 10h
BIT5                          EQU 20h
BIT6                          EQU 40h
BIT7                          EQU 80h
BIT8                          EQU 100h
BIT9				EQU	200h
BIT10				EQU	400h
BIT11				EQU	800h
BIT12				EQU	1000h
BIT13				EQU	2000h
BIT14				EQU	4000h
BIT15				EQU	8000h
BIT16				EQU	10000h
BIT17				EQU	20000h
BIT18				EQU	40000h
BIT19				EQU	80000h
BIT20				EQU	100000h
BIT21				EQU	200000h
BIT22				EQU	400000h
BIT23				EQU	800000h
BIT24				EQU	1000000h
BIT25				EQU	2000000h
BIT26				EQU	4000000h
BIT27				EQU	8000000h
BIT28				EQU	10000000h
BIT29				EQU	20000000h
BIT30				EQU	40000000h
BIT31				EQU	80000000h

;IA32 Arch specific equates
;------------------------------------------------------------------------------

MTRR_PHYS_BASE_0              EQU 0200h
MTRR_PHYS_MASK_0              EQU 0201h
MTRR_PHYS_BASE_1              EQU 0202h
MTRR_PHYS_MASK_1              EQU 0203h
MTRR_PHYS_BASE_2              EQU 0204h
MTRR_PHYS_MASK_2              EQU 0205h
MTRR_PHYS_BASE_3              EQU 0206h
MTRR_PHYS_MASK_3              EQU 0207h
MTRR_PHYS_BASE_4              EQU 0208h
MTRR_PHYS_MASK_4              EQU 0209h
MTRR_PHYS_BASE_5              EQU 020Ah
MTRR_PHYS_MASK_5              EQU 020Bh
MTRR_PHYS_BASE_6              EQU 020Ch
MTRR_PHYS_MASK_6              EQU 020Dh
MTRR_PHYS_BASE_7              EQU 020Eh
MTRR_PHYS_MASK_7              EQU 020Fh
MTRR_PHYS_BASE_8              EQU 0210h
MTRR_PHYS_MASK_8              EQU 0211h
MTRR_PHYS_BASE_9              EQU 0212h
MTRR_PHYS_MASK_9              EQU 0213h
MTRR_FIX_64K_00000            EQU 0250h
MTRR_FIX_16K_80000            EQU 0258h
MTRR_FIX_16K_A0000            EQU 0259h
MTRR_FIX_4K_C0000             EQU 0268h
MTRR_FIX_4K_C8000             EQU 0269h
MTRR_FIX_4K_D0000             EQU 026Ah
MTRR_FIX_4K_D8000             EQU 026Bh
MTRR_FIX_4K_E0000             EQU 026Ch
MTRR_FIX_4K_E8000             EQU 026Dh
MTRR_FIX_4K_F0000             EQU 026Eh
MTRR_FIX_4K_F8000             EQU 026Fh
MTRR_DEF_TYPE                 EQU 02FFh

MTRR_MEMORY_TYPE_UC           EQU 00h
MTRR_MEMORY_TYPE_WC           EQU 01h
MTRR_MEMORY_TYPE_WT           EQU 04h
MTRR_MEMORY_TYPE_WP           EQU 05h
MTRR_MEMORY_TYPE_WB           EQU 06h

MTRR_DEF_TYPE_E               EQU 0800h
MTRR_DEF_TYPE_FE              EQU 0400h
MTRR_PHYSMASK_VALID           EQU 0800h

;
; Define the high 32 bits of MTRR masking
; This should be read from CPUID EAX = 080000008h, EAX bits [7:0]
; But for most platforms this will be a fixed supported size so it is 
; fixed to save space.
;
MTRR_PHYS_MASK_VALID          EQU 0800h
MTRR_PHYS_MASK_HIGH           EQU 00000000Fh      ; For 36 bit addressing
;MTRR_PHYS_MASK_HIGH           EQU 0000000FFh      ; For 40 bit addressing

FV_MAIN_BASE                  EQU 0FFFFFFFCh


