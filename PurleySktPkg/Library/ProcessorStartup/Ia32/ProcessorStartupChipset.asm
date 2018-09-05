;
; This file contains an 'Intel Pre-EFI Module' and is licensed
; for Intel CPUs and Chipsets under the terms of your license
; agreement with Intel or your vendor.  This file may be
; modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2015, Intel Corporation. All rights reserved.<BR>
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
;  ProcessorStartupChipset.asm
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



        include ProcessorStartupPlatform.inc
        include ProcessorStartupChipset.inc
        include ProcessorStartup.inc

        EXTERN  GetLocalSktId:NEAR32
        EXTERN  AmILegacySocketNoStack:NEAR32
        EXTERN  GetMmcfgBus_NoStack:NEAR32
        EXTERN  GetMmcfgNoStack:NEAR32
        EXTERN  GetMmcfgBase:NEAR32
IFNDEF MINIBIOS_BUILD
        EXTERN  PcdGet16 (PcdPchAcpiIoPortBaseAddress):WORD
        EXTERN  PcdGet16 (PcdTcoBaseAddress):WORD
ENDIF
        EXTERN  GetBusNum:NEAR32
;----------------------------------------------------------------------------
;       STARTUP_SEG  S E G M E N T  STARTS
;----------------------------------------------------------------------------
STARTUP_SEG SEGMENT PARA PUBLIC 'CODE' USE32

IFDEF PCH_LBG_ASM
PlatformInitialization PROC NEAR PUBLIC

        ;
        ; Enable MCH Bar
        ;
        mov     esi, PCHBAR_REG
        mov     eax, (PCH_BASE_ADDRESS + 1)
        mov     Dword Ptr [esi], eax
        
        ;
        ; Enable PCR base address in PCH
        ;
        mov   esi, PCH_PCR_BASE_REG
        mov   eax, PCH_PCR_BASE
        mov   Dword Ptr [esi], eax
        mov   esi, PCH_P2SB_CMD_REG
        mov   al,  Byte Ptr [esi]
        or    al,  2                                    ; Enable bit1 MSE
        mov   Byte Ptr [esi], al

        ;
        ; PCI Enumeratuion is not done till later in DXE
        ; Initlialize SPI BAR to a default value till enumeration is done
        ;
        mov   eax,  (PCI_SPI_BASE + 10h)                 ; B0:D31:F5:R10
        mov   dx,   0CF8h
        out   dx,   eax
        mov   dx,   0CFCh
        mov   eax,  PCH_SPI_BAR
        out   dx,   eax

        ;
        ; PCI Enumeration is not done till later in DXE
        ; Enable memory space decoding for SPI
        ;
        mov   eax,  (PCI_SPI_BASE + 4h)                 ; B0:D31:F5:R10
        mov   dx,   0CF8h
        out   dx,   eax
        mov   dx,   0CFCh
        mov   eax,  02h
        out   dx,   eax


        ;
        ;
        ; Program ACPI PM Base.
        ;

        mov     esi, PCH_PMC_PMBASE_PCI_ADDR
IFDEF MINIBIOS_BUILD
        mov     ax, PCH_ACPI_BASE_ADDRESS + 1h
ELSE
        mov     ax, PcdGet16 (PcdPchAcpiIoPortBaseAddress)
ENDIF
        inc     ax
        mov     Word Ptr [esi], ax

        ;
        ; Program ACPI PM  Base under DMI device
        ;

        mov     esi, (PCH_PCR_BASE + 00EF27B4h)        ; SBCR[DMI] + 27B4h
IFDEF MINIBIOS_BUILD
        mov     eax, PCH_ACPI_BASE_ADDRESS + 00FC0001h ;Program "ACPI Base Address", PCR[DMI] + 27B4h[23:18, 15:2, 0] to [0x3F, D31:F2 Offset 40h bit[15:2],  1]
ELSE
        xor     eax, eax                                ;set eax to 0
        mov     ax,  PcdGet16 (PcdPchAcpiIoPortBaseAddress)
        add     eax, 00FC0001h                         ;Program "ACPI Base Address", PCR[DMI] + 27B4h[23:18, 15:2, 0] to [0x3F, D31:F2 Offset 40h bit[15:2],  1]
ENDIF
        mov     Dword Ptr [esi], eax
        mov     esi, (PCH_PCR_BASE + 00EF27B8h)        ; SBCR[DMI] + 27B8h
        mov     eax,  023A8h                           ;Program "ACPI Base Address", PCR[DMI] + 27B8h[31:0] to [0x23A8]
        mov     Dword Ptr [esi], eax

        ;
        ; Program PWRMBASE in PMC Device
        ;
        mov   eax,  (PCI_PMC_BASE + 48h)                 ; B0:D31:F2:R48
        mov   dx,   0CF8h
        out   dx,   eax
        mov   dx,   0CFCh
        mov   eax,  PCH_PWRM_BAR
        out   dx,   eax

        ;
        ;Program "PM Base Address Memory Range Base" PCR[DMI] + 27ACh[15:0] to the same value programmed in D31:F2 Offset 48h bit[31:16], this has an implication of making sure the PWRMBASE to be 64KB aligned.
        ;Program "PM Base Address Memory Range Limit" PCR[DMI] + 27ACh[31:16] to the value programmed in D31:F2 Offset 48h bit[31:16], this has an implication of making sure the memory allocated to PWRMBASE to be 64KB in size.
        ;
        mov     esi, (PCH_PCR_BASE + 00EF27ACh)     ; SBCR[DMI] + 27ACh
        mov     edx, PCH_PWRM_BAR
        and     edx, 0FFFF0000h
        mov     eax, PCH_PWRM_BAR
        shr     eax, 16
        add     eax, edx
        mov     Dword Ptr [esi], eax

        ;
        ; Program "PM Base Control" PCR[DMI] + 27B0h[31, 30:0] to [1, 0x23A8].
        ;
        mov     esi, (PCH_PCR_BASE + 00EF27B0h)     ; SBCR[DMI] + 27B0h
        mov     eax, 0800023A8h
        mov     Dword Ptr [esi], eax

        ;
        ; Set PWRM Enable (PWRM_EN) and ACPI Enable (EN)
        ;
        mov     esi, PCH_PMC_ACPICNTL_PCI_ADDR
        or      Word Ptr [esi], 00000180h

        ;
        ; Program and Enable TCO Base.
        ;
        mov     esi, PCH_SMBUS_TCOBASE_REG
IFDEF MINIBIOS_BUILD
        mov     ax, PCH_TCO_BASE_ADDRESS + 1
ELSE
        mov     ax, PcdGet16 (PcdTcoBaseAddress)
        inc     ax
ENDIF
        mov     Word Ptr [esi], ax
        mov     esi, PCH_SMBUS_TCOCTL_REG + 1
        or      Byte Ptr [esi], 01h

        ;
        ; Program and Enable TCO Base under DMI device
        ;
        mov     esi, (PCH_PCR_BASE + 00EF2778h)     ; SBCR[DMI] + 2778h
IFDEF MINIBIOS_BUILD
        mov     ax, PCH_TCO_BASE_ADDRESS + 3h
ELSE
        mov     ax, PcdGet16 (PcdTcoBaseAddress)
        add     ax, 3h
ENDIF
        mov     Word Ptr [esi], ax

        ;
        ; Enable HPET decode in PCH.
        ;
        mov     esi, PCH_HPET_REG
        mov     al,  PCH_HPET_AE
        mov     Byte Ptr [esi], al
        mov     al,  Byte Ptr [esi]                     ; read back
        xor     eax, eax
        mov     esi, HPET_COMP_1
        mov     Dword Ptr [esi], eax
        mov     esi, HPET_COMP_2
        mov     Dword ptr [esi], eax

        ;
        ; Enable the upper 128-byte bank of RTC RAM for SPT/LBG
        ;
        mov     esi, (PCH_PCR_BASE + PCH_PCR_RTC_CONF)
        mov     al, Byte Ptr [esi]
        or      al, PCH_PCR_RTC_CONF_UCMOS_EN
        mov     Byte Ptr [esi], al

        ;
        ; Enable GPP_A_16 as Native1 to active CLKOUT 48Mhz for SIO clock
        ;
        mov     esi, (PCH_PCR_BASE + 000AF0480h)
        mov     eax, Dword Ptr[esi]
        or      eax, 000000400h
        mov     Dword Ptr[esi], eax

        ret
PlatformInitialization ENDP
ENDIF


EarlyChipsetInit  PROC    NEAR    PUBLIC

        CALL_MMX6 AmILegacySocketNoStack                ; am I legacy socket?
        jnz   eci_exit                                  ; no, skip

IFDEF PCH_LBG_ASM
        ;
        ; Enable PCR base address in PCH
        ;
        mov   esi, PCH_PCR_BASE_REG
        mov   eax, PCH_PCR_BASE
        mov   Dword Ptr [esi], eax
        mov   esi, PCH_P2SB_CMD_REG
        mov   al,  Byte Ptr [esi]
        or    al,  2                                    ; Enable bit1 MSE
        mov   Byte Ptr [esi], al
        
        ;
        ; Send Port80 codes to LPC.
        ;
        mov     edi,  (PCH_PCR_BASE + PCH_PCR_DMI_GCS_CONF + 1)     ; IOSF-SB DMI endpoint GCS register
        and     Byte Ptr [edi], 0f7h                                ; Clear bit [11]: Reserved Page Route
ELSE
        ;
        ; Program RCBA adress only for WBG
        ;
        CALL_MMX6 GetMmcfgNoStack                       ; edi = mmcfg.base
        mov   ebx,0F80F0h                               ; PCH csr at 0:31:0:F0
        mov   DWORD PTR ds:[edi][ebx], SB_RCBA OR 1     ; Set the Enable bit.
        ;
        ; Send Port80 codes to LPC.
        ;
        mov     edi,  SB_RCBA + 3410h                 ; RCBA + 3410h, ICH_RCRB_GCS
        and     DWORD PTR [edi], NOT (1 SHL 2)        ; Clear bit [2]
ENDIF
        ;
        ; APTIOV_SERVER_OVERRIDE_RC_START : Mask external interrupts
        ;
        
        mov al, 0FFh
        out 21h, al    ;; Mask off all interrupts in master 8259
        out 0a1h, al   ;; Mask off all interrupts in slave 8259
	
	;
	; APTIOV_SERVER_OVERRIDE_RC_END : Mask external interrupts
	;
eci_exit:
        RET_ESI_MMX3
EarlyChipsetInit  ENDP

MidChipsetInit    PROC    NEAR    PUBLIC
        RET_ESI_MMX3
MidChipsetInit    ENDP

LateChipsetInit    PROC    NEAR    PUBLIC

        ;
        ; Enable SPI prefetching
        ;
IFDEF PCH_LBG_ASM
        mov    eax, 0FD0DCh                             ; PCH reg at 0:31:5:DCh (SPI Controller CFG space)
ELSE
        mov    eax, 0F80DCh                             ; PCH reg at 0:31:0:DCh
ENDIF
        call   GetBusNum                                ; EAX[27:20] updated with current bus#
        mov    ebx, eax
        call   GetMmcfgBase                             ; esi = MMCFG Base Address
        or     Dword Ptr [esi][ebx], BIT3               ; Bits [3:2] = 10b - enable prefetching and caching.

IFNDEF PCH_LBG_ASM
        ;
        ; Program and Enable ACPI PM Base.
        ;
        and     ebx, 0FFF00000h                           ; clear D:F:R filed
        or      ebx, PCH_HSX_LPC_PMBASE_PCI_ADDR AND 0FFFFFh  ; PCH reg at 0:31:0:40h
IFDEF MINIBIOS_BUILD
        mov     eax, PCH_ACPI_BASE_ADDRESS + 1
ELSE
        xor     eax, eax
        mov     ax, PcdGet16 (PcdPchAcpiIoPortBaseAddress)
        inc     ax
ENDIF
        mov     Dword Ptr [esi][ebx], eax

        and     ebx, 0FFF00000h
        or      ebx, PCH_HSX_LPC_ACPICNTL_PCI_ADDR AND 0FFFFFh
        or      Dword Ptr [esi][ebx], BIT7

        ;
        ; Enable GPIO BASE I/O registers
        ;
        mov     eax, PCI_LPC_BASE + 48h
        mov     dx, 0CF8h
        out     dx, eax
        mov     eax, GPIO_BASE_ADDRESS
        add     dx, 4
        out     dx, eax

        mov     eax, PCI_LPC_BASE + 4Ch
        mov     dx, 0CF8h
        out     dx, eax
        add     dx, 4
        in      al, dx
        or      al, BIT4     ; GPIOBASE Enable
        out     dx, al

        ;GPIO_USE_SEL2 Register -> 1 = GPIO 0 = Native
        mov     dx, GPIO_BASE_ADDRESS + R_GPIO_USE_SEL2
        in      eax, dx
        or      eax, 002h    ;Enable GPIO33
        out     dx, eax
        ;GP_IO_SEL2 Register -> 1 = Input 0 = Output if Native Mode not care
        mov     dx, GPIO_BASE_ADDRESS + R_GPIO_IO_SEL2
        in      eax, dx
        or      eax, 002h    ;Configure GPIO33 as Input
        out     dx, eax
ENDIF
        ;
        ; Enable HPET decode in PCH.
        ;
IFDEF PCH_LBG_ASM
        mov     esi, PCH_HPET_CFG_PCI_ADDR
        mov     eax, PCH_HPET_DECODE
ELSE
        mov     esi, PCH_RCRB_BASE + PCH_RCRB_HPET
        mov     eax, PCH_HPET_DECODE
ENDIF
        mov     Dword Ptr [esi], eax
        mov     eax, Dword ptr [esi]
        xor     eax, eax
        mov     esi, HPET_COMP_1
        mov     Dword Ptr [esi], eax
        mov     esi, HPET_COMP_2
        mov     Dword ptr [esi], eax

        ;
        ; Enable the upper 128-byte bank of RTC RAM for WBG
        ;
IFNDEF PCH_LBG_ASM
        mov     esi, PCH_RCRB_BASE + PCH_RCRB_RTC_CONF
        mov     eax, Dword Ptr [esi]
        or      eax, PCH_RCRB_RTC_CONF_UCMOS_EN
        mov     Dword Ptr [esi], eax
ENDIF
IFDEF PCH_LBG_ASM
        call    PlatformInitialization
ENDIF

; APTIOV_SERVER_OVERRIDE_RC_START : Added to halt TCO timer
        ;
        ; Halt TCO timer.
        ;  
        mov		dx, MKF_TCO_BASE_ADDRESS ; #TcoBase - Tco Base address register is changed in LBG
        or		dx, ICH_IOREG_TCO1_CNT
        in		ax, dx
        or		ax, BIT_TCO_TMR_HLT
        out		dx, ax

        ;
        ; Reload timer to prevent timeout
        ;
        mov   dx, MKF_TCO_BASE_ADDRESS ; #TcoBase - Tco Base address register is changed in LBG
        mov   ax, 4
        out   dx, ax
        
        ;
        ; Clear TCO_STS
        ;
        mov   dx, MKF_TCO_BASE_ADDRESS ; #TcoBase - Tco Base address register is changed in LBG
        or    dx, ICH_IOREG_TCO1_STS
        mov   ax, 008h
        out   dx, ax
        
        ;
        ; Clear SMI_STS.TCO_STS
        ;
        mov   dx, PCH_ACPI_BASE_ADDRESS
        or    dx, 034h                     ; SMI_STS offset 34h
        mov   ax, 02000h
        out   dx, ax
; APTIOV_SERVER_OVERRIDE_RC_END : Added to halt TCO timer
 
        ret
LateChipsetInit    ENDP


STARTUP_SEG ENDS

END

