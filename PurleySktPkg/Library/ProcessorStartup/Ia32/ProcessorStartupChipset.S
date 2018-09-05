#
# This file contains an 'Intel Pre-EFI Module' and is licensed
# for Intel CPUs and Chipsets under the terms of your license
# agreement with Intel or your vendor.  This file may be
# modified by the user, subject to additional terms of the
# license agreement
#
#------------------------------------------------------------------------------
#
# Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>
# This software and associated documentation (if any) is furnished
# under a license and may only be used or copied in accordance
# with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be
# reproduced, stored in a retrieval system, or transmitted in any
# form or by any means without the express written consent of
# Intel Corporation.
#
# Module Name:
#
#  ProcessorStartupChipset.asm
#
# Abstract:
#
#
#------------------------------------------------------------------------------

        .include "ProcessorStartupPlatform.i"
        .include "ProcessorStartupChipset.i"
        .include "ProcessorStartup.i"

        .extern GetLocalSktId
        .extern AmILegacySocketNoStack
        .extern GetMmcfgBus_NoStack
        .extern GetMmcfgNoStack
        .extern GetMmcfgBase
#ifndef MINIBIOS_BUILD
        .extern PcdGet16 (PcdPchAcpiIoPortBaseAddress)
        .extern PcdGet16 (PcdTcoBaseAddress)
#endif
        .extern GetBusNum
#----------------------------------------------------------------------------
#       STARTUP_SEG  S E G M E N T  STARTS
#----------------------------------------------------------------------------

.code32

#ifdef PCH_LBG_ASM

ASM_GLOBAL ASM_PFX(PlatformInitialization)
ASM_PFX(PlatformInitialization):

        #
        # Enable MCH Bar
        #
        movl    $PCHBAR_REG, %esi
        movl    $(PCH_BASE_ADDRESS + 1), %eax
        movl    %eax, (%esi)

        #
        # Enable PCR base address in PCH
        #
        movl  $PCH_PCR_BASE_REG, %esi
        movl  $PCH_PCR_BASE, %eax
        movl  %eax, (%esi)
        movl  $PCH_P2SB_CMD_REG, %esi
        movb  (%esi), %al
        orb   $2, %al                                   # Enable bit1 MSE
        movb  %al, (%esi)

        #
        # PCI Enumeratuion is not done till later in DXE
        # Initlialize SPI BAR to a default value till enumeration is done
        #
        movl  $(PCI_SPI_BASE + 0x10), %eax
        movw  $0xCF8, %dx
        outl  %eax, %dx
        movw  $0xCFC, %dx
        movl  $PCH_SPI_BAR, %eax
        outl  %eax, %dx

        #
        # PCI Enumeration is not done till later in DXE
        # Enable memory space decoding for SPI
        #
        movl  $(PCI_SPI_BASE + 4), %eax
        movw  $0xCF8, %dx
        outl  %eax, %dx
        movw  $0xCFC, %dx
        movl  $0x2, %eax
        outl  %eax, %dx


        #
        #
        # Program ACPI PM Base.
        #

        movl    $PCH_PMC_PMBASE_PCI_ADDR, %esi
#ifdef MINIBIOS_BUILD
        movw    $PCH_ACPI_BASE_ADDRESS + 1h, %ax
#else
        movw    PcdGet16 (PcdPchAcpiIoPortBaseAddress), %ax
#endif
        incw    %ax
        movw    %ax, (%esi)

        #
        # Program ACPI PM  Base under DMI device
        #

        movl    $(PCH_PCR_BASE + 0x00EF27B4), %esi
#ifdef MINIBIOS_BUILD
        movl    $(PCH_ACPI_BASE_ADDRESS + 0x00FC0001), %eax
#else
        xorl    %eax, %eax                              #set eax to 0
        movw    PcdGet16 (PcdPchAcpiIoPortBaseAddress), %ax
        addl    $0x0FC0001, %eax                       #Program "ACPI Base Address", PCR[DMI] + 27B4h[23:18, 15:2, 0] to [0x3F, D31:F2 Offset 40h bit[15:2],  1]
#endif
        movl    %eax, (%esi)
        movl    $(PCH_PCR_BASE + 0x00EF27B8), %esi
        movl    $0x23A8, %eax                          #Program "ACPI Base Address", PCR[DMI] + 27B8h[31:0] to [0x23A8]
        movl    %eax, (%esi)

        #
        # Program PWRMBASE in PMC Device
        #
        movl  $(PCI_PMC_BASE + 0x48), %eax
        movw  $0xCF8, %dx
        outl  %eax, %dx
        movw  $0xCFC, %dx
        movl  $PCH_PWRM_BAR, %eax
        outl  %eax, %dx

        #
        #Program "PM Base Address Memory Range Base" PCR[DMI] + 27ACh[15:0] to the same value programmed in D31:F2 Offset 48h bit[31:16], this has an implication of making sure the PWRMBASE to be 64KB aligned.
        #Program "PM Base Address Memory Range Limit" PCR[DMI] + 27ACh[31:16] to the value programmed in D31:F2 Offset 48h bit[31:16], this has an implication of making sure the memory allocated to PWRMBASE to be 64KB in size.
        #
        movl    $(PCH_PCR_BASE + 0x00EF27AC), %esi
        movl    $PCH_PWRM_BAR, %edx
        andl    $0xFFFF0000, %edx
        movl    $PCH_PWRM_BAR, %eax
        shrl    $16, %eax
        addl    %edx, %eax
        movl    %eax, (%esi)

        #
        # Program "PM Base Control" PCR[DMI] + 27B0h[31, 30:0] to [1, 0x23A8].
        #
        movl    $(PCH_PCR_BASE + 0x00EF27B0), %esi
        movl    $0x800023A8, %eax
        movl    %eax, (%esi)

        #
        # Set PWRM Enable (PWRM_EN) and ACPI Enable (EN)
        #
        movl    $PCH_PMC_ACPICNTL_PCI_ADDR, %esi
        orw     $0x0000180, (%esi)

        #
        # Program and Enable TCO Base.
        #
        movl    $PCH_SMBUS_TCOBASE_REG, %esi
#ifdef MINIBIOS_BUILD
        movw    $(PCH_TCO_BASE_ADDRESS + 1), %ax
#else
        movw    PcdGet16 (PcdTcoBaseAddress), %ax
        incw    %ax
#endif
        movw    %ax, (%esi)
        movl    $PCH_SMBUS_TCOCTL_REG, %esi
        orw     $0x100, (%esi)

        #
        # Program and Enable TCO Base under DMI device
        #
        movl    $(PCH_PCR_BASE + 0x00EF2778), %esi
#ifdef MINIBIOS_BUILD
        movw    $(PCH_TCO_BASE_ADDRESS + 3), %ax
#else
        movw    PcdGet16 (PcdTcoBaseAddress), %ax
        addw    $0x3, %ax
#endif
        movw    %ax, (%esi)

        #
        # Enable HPET decode in PCH.
        #
        movl    $PCH_HPET_REG, %esi
        movb    $PCH_HPET_AE, %al
        movb    %al, (%esi)
        movb    (%esi), %al                             # read back
        xorl    %eax, %eax
        movl    $HPET_COMP_1, %esi
        movl    %eax, (%esi)
        movl    $HPET_COMP_2, %esi
        movl    %eax, (%esi)

        #
        # Enable the upper 128-byte bank of RTC RAM for SPT/LBG
        #
        movl    $(PCH_PCR_BASE + PCH_PCR_RTC_CONF), %esi
        movb    (%esi), %al
        orb     $PCH_PCR_RTC_CONF_UCMOS_EN, %al
        movb    %al, (%esi)

        #
        # Enable GPP_A_16 as Native1 to active CLKOUT 48Mhz for SIO clock
        #
        movl    $(PCH_PCR_BASE + 0x000AF0480), %esi
        movl    (%esi), %eax
        orl     $0x00000400, %eax
        movl    %eax, (%esi)

        ret
#endif

ASM_GLOBAL ASM_PFX(EarlyChipsetInit)
ASM_PFX(EarlyChipsetInit):

        CALL_MMX6 AmILegacySocketNoStack
        jnz   eci_exit                                  # no, skip

#ifdef PCH_LBG_ASM
        #
        # Enable PCR base address in PCH
        #
        movl    $PCH_PCR_BASE_REG, %esi
        movl    $PCH_PCR_BASE, %eax
        movl    %eax, (%esi)
        movl    $PCH_P2SB_CMD_REG, %esi
        movb    (%esi), %al
        orb     $2, %al                                    # Enable bit1 MSE
        movb    %al, (%esi)

        #
        # Send Port80 codes to LPC.
        #
        movl    $(PCH_PCR_BASE + PCH_PCR_DMI_GCS_CONF + 1), %edi
        andb    $0xf7, (%edi)
#else
        #
        # Program RCBA adress only for WBG
        #
        CALL_MMX6 GetMmcfgNoStack
        movl    $0xF80F0, %ebx                            # PCH csr at 0:31:0:F0
        movl    $(SB_RCBA | 1), %ds:(%edi,%ebx)

        #
        # Send Port80 codes to LPC.
        #
        movl    $(SB_RCBA + 0x3410), %edi
        andl    $(~(1 << 2)), (%edi)
#endif
        #
        # APTIOV_SERVER_OVERRIDE_RC_START : Mask external interrupts
        #
        
        movb $0FFh, %al
        outb %al, $21h     ## Mask off all interrupts in master 8259
        outb %al, $0a1h    ## Mask off all interrupts in slave 8259
	
	#
	# APTIOV_SERVER_OVERRIDE_RC_END : Mask external interrupts
	#

eci_exit: 
        RET_ESI_MMX3

ASM_GLOBAL ASM_PFX(MidChipsetInit)
ASM_PFX(MidChipsetInit):
        RET_ESI_MMX3

ASM_GLOBAL ASM_PFX(LateChipsetInit)
ASM_PFX(LateChipsetInit):

        #
        # Enable SPI prefetching
        #
#ifdef PCH_LBG_ASM
        movl   $0xFD0DC, %eax                           # PCH reg at 0:31:5:DCh (SPI Controller CFG space)
#else
        movl   $0xF80DC, %eax                           # PCH reg at 0:31:0:DCh
#endif
        call   GetBusNum                                # EAX[27:20] updated with current bus#
        movl   %eax, %ebx
        call   GetMmcfgBase                             # esi = MMCFG Base Address
        orl    $(BIT3), (%esi,%ebx)

#ifndef PCH_LBG_ASM
        #
        # Program and Enable ACPI PM Base.
        #
        andl    $0xFFF00000, %ebx                         # clear D:F:R filed
        orl     $(PCH_HSX_LPC_PMBASE_PCI_ADDR & 0x0FFFFF), %ebx
#ifdef MINIBIOS_BUILD
        movl    $(PCH_ACPI_BASE_ADDRESS + 1), %eax
#else
        xorl    %eax, %eax
        movw    PcdGet16 (PcdPchAcpiIoPortBaseAddress), %ax
        incw    %ax
#endif
        movl    %eax, (%esi,%ebx)

        andl    $0xFFF00000, %ebx
        orl     $(PCH_HSX_LPC_ACPICNTL_PCI_ADDR & 0x0FFFFF), %ebx
        orl     $(BIT7), (%esi,%ebx)

        #
        # Enable GPIO BASE I/O registers
        #
        movl    $(PCI_LPC_BASE + 0x48), %eax
        movw    $0xCF8, %dx
        outl    %eax, %dx
        movl    $GPIO_BASE_ADDRESS, %eax
        addw    $4, %dx
        outl    %eax, %dx

        mov     $(PCI_LPC_BASE + 0x4C), %eax
        movw    $0xCF8, %dx
        outl    %eax, %dx
        addw    $4, %dx
        in      %dx, %al
        orb     $BIT4, %al    # GPIOBASE Enable
        outb    %al, %dx

        #GPIO_USE_SEL2 Register -> 1 = GPIO 0 = Native
        movw    $(GPIO_BASE_ADDRESS + R_GPIO_USE_SEL2), %dx
        in      %dx, %eax
        orl     $0x02, %eax  #Enable GPIO33
        outl    %eax, %dx
        #GP_IO_SEL2 Register -> 1 = Input 0 = Output if Native Mode not care
        movw    $(GPIO_BASE_ADDRESS + R_GPIO_IO_SEL2), %dx
        in      %dx, %eax
        orl     $0x02, %eax  #Configure GPIO33 as Input
        outl    %eax, %dx
#endif
        #
        # Enable HPET decode in PCH.
        #
#ifdef PCH_LBG_ASM
        movl    $(PCH_HPET_CFG_PCI_ADDR), %esi
        movl    $(PCH_HPET_DECODE), %eax
#else
        movl    $(PCH_RCRB_BASE + PCH_RCRB_HPET), %esi
        movl    $PCH_HPET_DECODE, %eax
#endif
        movl    %eax, (%esi)
        movl    (%esi), %eax
        xorl    %eax, %eax
        movl    $HPET_COMP_1, %esi
        movl    %eax, (%esi)
        movl    $HPET_COMP_2, %esi
        movl    %eax, (%esi)

        #
        # Enable the upper 128-byte bank of RTC RAM for WBG
        #
#ifndef PCH_LBG_ASM
        movl    $(PCH_RCRB_BASE + PCH_RCRB_RTC_CONF), %esi
        movl    (%esi), %eax
        orl     $PCH_RCRB_RTC_CONF_UCMOS_EN, %eax
        movl    %eax, (%esi)
#endif
#ifdef PCH_LBG_ASM
        call    PlatformInitialization
#endif

# APTIOV_SERVER_OVERRIDE_RC_START : Added to halt TCO timer
        #
        # Halt TCO timer.
        #
        movw		$MKF_TCO_BASE_ADDRESS, %dx ; #TcoBase - Tco Base address register is changed in LBG
        orw		$ICH_IOREG_TCO1_CNT, %dx
        inw		%dx, %ax
        orw		$BIT_TCO_TMR_HLT, %ax
        outw		%ax, %dx

        #
        # Reload timer to prevent timeout
        #
        movw   $MKF_TCO_BASE_ADDRESS, %dx  #TcoBase - Tco Base address register is changed in LBG
        movw   $4h, %ax
        outw   %ax, %dx
        
        #
        # Clear TCO_STS
        #
        movw   $MKF_TCO_BASE_ADDRESS, %dx   #TcoBase - Tco Base address register is changed in LBG
        orw    $ICH_IOREG_TCO1_STS, %dx
        movw   $008h, %ax
        outw   %ax, %dx
        
        #
        # Clear SMI_STS.TCO_STS
        #
        movw   $PCH_ACPI_BASE_ADDRESS, %dx
        orw    $034h, %dx,                      # SMI_STS offset 34h
        movw   $02000h, %ax
        outw   %ax, %dx
# APTIOV_SERVER_OVERRIDE_RC_END : Added to halt TCO timer
 
        ret
